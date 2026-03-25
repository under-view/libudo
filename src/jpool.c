#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "log.h"
#include "futex.h"
#include "jpool.h"

/*
 * So, far no CPU has 512 cores.
 */
#define THREADS_MAX (1<<9)
#define JOB_QUEUE_MEMBER_SIZE (3 * sizeof(udo_atomic_u32))

/*
 * @brief Structure defining information about the job to execute.
 *        Is used in udo_jpool_add_job(3) to add a job to the job
 *        queue.
 *
 * @member func - Function pointer to a function for thread to execute.
 * @member arg  - Argument to pass to function.
 */
struct udo_jpool_job
{
	void (*func)(void *arg);
	void *arg;
};


/*
 * @brief Structure defining information about the queue.
 *
 * @member job_free - Futex used to wake threads or put them
 *                    to sleep if jobs are available.
 * @member front    - Byte offset to the front of the queue.
 * @member rear     - Byte offset to the rear of the queue.
 * @member data     - Starting address caller may store data in.
 * @member size     - Byte size of queue associated with thread.
 */
struct udo_jpool_queue
{
	udo_atomic_u32 *job_free;
	udo_atomic_u32 *front;
	udo_atomic_u32 *rear;
	void           *data;
	uint32_t       size;
};


/*
 * @brief Structure defining information used by threads.
 *
 * @member tid   - Array of POSIX thread ID's.
 * @member queue - Structure keeping track of
 *                 current jobs a thread can
 *                 run.
 */
struct udo_jpool_thread
{
	pthread_t              tid;
	struct udo_jpool_queue queue;
};


/*
 * @brief Structure defining the udo_jpool (Udo Job Pool) instance.
 *
 * @member err          - Stores information about the error that occured
 *                        for the given instance and may later be retrieved
 *                        by caller.
 * @member free         - If structure allocated with calloc(3) member will be
 *                        set to true so that, we know to call free(3) when
 *                        destroying the instance.
 * @member queue_sz     - Byte size of @queue_data.
 * @member queue_data   - Shared memory buffer storing actual
 *                        addresses to jobs.
 * @member cur_thread   - Current thread index whose queue will have
 *                        work placed in it.
 * @member thread_count - Amount of threads in the pool.
 * @member threads      - Array of threads storing location of each
 *                        threads queue and unique ID.
 */
struct udo_jpool
{
	struct udo_log_error_struct err;
	bool                        free;
	uint32_t                    queue_sz;
	void                        *queue_data;
	udo_atomic_u32              *cur_thread;
	uint32_t                    thread_count;
	struct udo_jpool_thread     threads[THREADS_MAX];
};


/*****************************************
 * Start of global to C source functions *
 *****************************************/

UDO_STATIC_INLINE
uint32_t
p_queue_get_job_free (const struct udo_jpool_queue *queue)
{
	return __atomic_load_n(queue->job_free, __ATOMIC_ACQUIRE);
}


UDO_STATIC_INLINE
uint32_t
p_queue_get_rear (const struct udo_jpool_queue *queue)
{
	return __atomic_load_n(queue->rear, __ATOMIC_ACQUIRE);
}


UDO_STATIC_INLINE
uint32_t
p_queue_get_front (const struct udo_jpool_queue *queue)
{
	return __atomic_load_n(queue->front, __ATOMIC_ACQUIRE);
}


UDO_STATIC_INLINE
uint32_t
p_queue_add_rear (const struct udo_jpool_queue *queue)
{
	return __atomic_fetch_add(queue->rear, \
		sizeof(struct udo_jpool_job), \
		__ATOMIC_SEQ_CST);
}


UDO_STATIC_INLINE
uint32_t
p_queue_add_front (const struct udo_jpool_queue *queue)
{
	return __atomic_fetch_add(queue->front, \
		sizeof(struct udo_jpool_job), \
		__ATOMIC_SEQ_CST);
}


UDO_STATIC_INLINE
bool
p_queue_can_get_job (const struct udo_jpool_queue *queue)
{
	return p_queue_get_rear(queue) > p_queue_get_front(queue);
}


UDO_STATIC_INLINE
bool
p_queue_empty (const struct udo_jpool_queue *queue)
{
	return !p_queue_get_rear(queue);
}


UDO_STATIC_INLINE
bool
p_queue_full (const struct udo_jpool_queue *queue)
{
	return p_queue_get_rear(queue) >= queue->size;
}


UDO_STATIC_INLINE
bool
p_queue_can_loop (const struct udo_jpool_queue *queue)
{
	return p_queue_get_job_free(queue) != 0x66AFB55C;
}


UDO_STATIC_INLINE
void
p_queue_reset (const struct udo_jpool_queue *queue)
{
	__atomic_store_n(queue->job_free, 0, \
			__ATOMIC_RELEASE);
	__atomic_store_n(queue->front, 0, \
			__ATOMIC_RELEASE);
	__atomic_store_n(queue->rear, 0, \
			__ATOMIC_RELEASE);
}


UDO_STATIC_INLINE
void
p_queue_job_reset (struct udo_jpool_job *job)
{
	job->func = NULL;
	job->arg = NULL;
}


UDO_STATIC_INLINE
struct udo_jpool_job *
p_queue_get_job (struct udo_jpool_queue *queue)
{
	return (void *) ((char *) queue->data) + \
		p_queue_add_front(queue);
}


static void *
p_run_thread (void *p_queue)
{
	struct udo_jpool_job *job;
	struct udo_jpool_queue *queue = p_queue;

	while (p_queue_can_loop(queue)) {
		udo_futex_wait_cond(queue->job_free, \
			p_queue_can_get_job(queue));

		job = p_queue_get_job(queue);
		if (job->func) {
			job->func(job->arg);
			p_queue_job_reset(job);
		}

		if (p_queue_full(queue))
			p_queue_reset(queue);
	}

	return NULL;
}

/***************************************
 * End of global to C source functions *
 ***************************************/


/***************************************
 * Start of udo_jpool_create functions *
 ***************************************/

struct udo_jpool *
udo_jpool_create (struct udo_jpool *p_jpool,
                  const void *p_jpool_info)
{
	int err;
	pthread_t thread;
	struct udo_jpool_queue *queue;
	uint32_t t, queue_sz, offset, data_off;
	struct udo_futex_create_info futex_info;

	struct udo_jpool *jpool = p_jpool;
	const struct udo_jpool_create_info *jpool_info = p_jpool_info;

	if (!jpool_info || \
	    !(jpool_info->size) || \
	    !(jpool_info->count) || \
	    (jpool_info->count >= THREADS_MAX))
	{
		udo_log_error("Incorrect data passed\n");
		return NULL;
	}

	if (!jpool) {
		jpool = calloc(1, sizeof(struct udo_jpool));
		if (!jpool) {
			udo_log_error("calloc: %s\n", strerror(errno));
			return NULL;
		}

		jpool->free = true;
	}

	offset = sizeof(udo_atomic_u32);
	data_off = offset + (JOB_QUEUE_MEMBER_SIZE * jpool_info->count);

	futex_info.count = 1; /* Byte align on 4K boundary */
	futex_info.size = UDO_BYTE_ALIGN(data_off + \
		(jpool_info->size * jpool_info->count),
		UDO_PAGE_SIZE);
	jpool->queue_data = udo_futex_create(&futex_info);
	if (!(jpool->queue_data)) {
		udo_jpool_destroy(jpool);
		return NULL;
	}

	jpool->queue_sz = futex_info.size;
	jpool->thread_count = jpool_info->count;
	jpool->cur_thread = (udo_atomic_u32 *) jpool->queue_data;
	queue_sz = (jpool->queue_sz - data_off) / jpool->thread_count;

	__atomic_store_n(jpool->cur_thread, 0, __ATOMIC_RELEASE);

	for (t = 0; t < jpool->thread_count; t++) {
		queue = &(jpool->threads[t].queue);

		queue->size = queue_sz;
		queue->data = (void *) ((char *) \
			jpool->queue_data) + data_off;

		queue->job_free = (void *) ((char *) \
			jpool->queue_data + offset);

		queue->front = (void *) ((char *) jpool->queue_data + \
			offset + sizeof(udo_atomic_u32));

		queue->rear = (void *) ((char *)jpool->queue_data + \
			offset + (2 * sizeof(udo_atomic_u32)));

		p_queue_reset(queue);
		err = pthread_create(&thread, NULL, p_run_thread, queue);
		if (err) {
			udo_log_error("pthread_create: %s\n", strerror(errno));
			udo_jpool_destroy(jpool);
			return NULL;
		}

		data_off += queue_sz;
		offset += JOB_QUEUE_MEMBER_SIZE;
		jpool->threads[t].tid = thread; thread = 0;
	}

	return jpool;
}

/*************************************
 * End of udo_jpool_create functions *
 *************************************/


/****************************************
 * Start of udo_jpool_add_job functions *
 ****************************************/

int
udo_jpool_add_job (struct udo_jpool *jpool,
                   void (*func)(void *arg),
                   void *arg)
{
	uint32_t tid;
	struct udo_jpool_job *job;
	struct udo_jpool_queue *queue;

	if (!jpool) {
		udo_log_error("Incorrect data passed\n");
		return -1;
	}

	if (!func || !arg) {
		udo_log_set_error(jpool, UDO_LOG_ERR_INCORRECT_DATA, "");
		return -1;
	}

	do {
		tid = __atomic_load_n(jpool->cur_thread, \
					__ATOMIC_ACQUIRE);
		queue = &(jpool->threads[tid].queue);
		tid = (tid + 1) % jpool->thread_count;
		__atomic_store_n(jpool->cur_thread, \
			tid, __ATOMIC_RELEASE);
	} while (p_queue_full(queue));

	job = (void *) ((char *) queue->data) + \
		p_queue_add_rear(queue);

	job->func = func;
	job->arg = arg;

	udo_futex_wake_cond(queue->job_free);

	return 0;
}

/**************************************
 * End of udo_jpool_add_job functions *
 **************************************/


/****************************************
 * Start of udo_jpool_destroy functions *
 ****************************************/

void
udo_jpool_destroy (struct udo_jpool *jpool)
{
	uint32_t t;
	struct udo_jpool_queue *queue;

	if (!jpool)
		return;

	for (t = 0; t < jpool->thread_count; t++) {
		queue = &(jpool->threads[t].queue);
		udo_futex_unlock_force(queue->job_free);
		udo_futex_wake_cond(queue->job_free);
		pthread_join(jpool->threads[t].tid, NULL);
	}

	udo_futex_destroy((udo_atomic_u32*) \
	                  jpool->queue_data, \
	                  jpool->queue_sz);

	if (jpool->free) {
		free(jpool);
	} else {
		memset(jpool, 0, sizeof(struct udo_jpool));
	}
}

/**************************************
 * End of udo_jpool_destroy functions *
 **************************************/


/*************************************************
 * Start of non struct udo_jpool param functions *
 *************************************************/

int
udo_jpool_get_sizeof (void)
{
	return sizeof(struct udo_jpool);
}

/***********************************************
 * End of non struct udo_jpool param functions *
 ***********************************************/
