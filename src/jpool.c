#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <pthread.h>

#include "log.h"
#include "futex.h"
#include "jpool.h"

/*
 * So, far no CPU has 512 cores.
 */
#define THREADS_MAX (1<<9)

/*
 * @brief Structure defining information about the job to execute.
 *        Is used in udo_jpool_add_job(3) to add job a to the job
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
 * @member size     - Byte size of queue associated with thread.
 * @member offset   - Byte offset in larger queue buffer associated
 *                    with thread.
 * @member job_free - Futex used to wake threads or put them
 *                    to sleep if jobs are available.
 * @member front    - Byte offset to the front of the queue.
 * @member rear     - Bytes offset to the rear of the queue.
 * @member data     - Starting address caller may store data in.
 */
struct udo_jpool_queue
{
	udo_atomic_u32 *size;
	udo_atomic_u32 *offset;
	udo_atomic_u32 *job_free;
	udo_atomic_u32 *front;
	udo_atomic_u32 *rear;
	udo_atomic_u32 *data;
};


/*
 * @brief Structuring defining information used by threads.
 *
 * @member thread_id - Array of POSIX thread ID's.
 * @member queue     - Structure keeping track of current jobs
 *                     a thread can run.
 */
struct udo_jpool_thread
{
	pthread_t              thread_id;
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
 *                        threads queue and thread ID.
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
p_queue_get_size (const struct udo_jpool_queue *queue)
{
	return __atomic_load_n(queue->size, __ATOMIC_ACQUIRE);
}


UDO_STATIC_INLINE
uint32_t
p_queue_get_offset (const struct udo_jpool_queue *queue)
{
	return __atomic_load_n(queue->offset, __ATOMIC_ACQUIRE);
}


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
bool
p_queue_can_get_job (const struct udo_jpool_queue *queue)
{
	return p_queue_get_rear(queue) > p_queue_get_front(queue);
}


UDO_STATIC_INLINE
bool
p_queue_empty (const struct udo_jpool_queue *queue)
{
	return p_queue_get_rear(queue) == \
		(p_queue_get_offset(queue) + \
		 sizeof(struct udo_jpool_queue));
}


UDO_STATIC_INLINE
bool
p_queue_full (const struct udo_jpool_queue *queue)
{
	return p_queue_get_rear(queue) >= \
		(p_queue_get_offset(queue) + \
		 p_queue_get_size(queue) - \
		 sizeof(struct udo_jpool_job));
}


UDO_STATIC_INLINE
void
p_queue_reset (struct udo_jpool_queue *queue)
{
	__atomic_store_n(queue->job_free, \
			0, __ATOMIC_RELEASE);
	__atomic_store_n(queue->front, \
			(p_queue_get_offset(queue) + \
		 	sizeof(struct udo_jpool_queue)), \
			__ATOMIC_RELEASE);
	__atomic_store_n(queue->rear, \
			(p_queue_get_offset(queue) + \
		 	sizeof(struct udo_jpool_queue)), \
			__ATOMIC_RELEASE);
}


static struct udo_jpool_job *
p_queue_get_job (struct udo_jpool_queue *queue)
{
	uint32_t front;

	front = __atomic_add_fetch(queue->front, \
		sizeof(struct udo_jpool_job), \
		__ATOMIC_RELEASE);

	if (p_queue_full(queue))
		p_queue_reset(queue);

	return (struct udo_jpool_job *) \
		((char *) queue->data + front);
}


static void *
p_run_thread (void *p_queue)
{
	struct udo_jpool_job *job;
	struct udo_jpool_queue *queue = p_queue;

	while (p_queue_get_rear(queue) && \
	       !p_queue_get_job_free(queue))
	{
		udo_futex_wait_cond(queue->job_free, \
			p_queue_can_get_job(queue));

		job = p_queue_get_job(queue);
		job->func(job->arg);
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
	uint32_t t, queue_size, offset;
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

	futex_info.count = 1; /* Byte align on 4K boundary */
	futex_info.size = UDO_BYTE_ALIGN(jpool_info->size * \
			jpool_info->count, UDO_PAGE_SIZE);
	jpool->queue_data = udo_futex_create(&futex_info);
	if (!(jpool->queue_data)) {
		udo_jpool_destroy(jpool);
		return NULL;
	}

	jpool->queue_sz = futex_info.size;
	jpool->thread_count = jpool_info->count;
	queue_size = (size_t) ((jpool->queue_sz - \
		sizeof(udo_atomic_u32)) / jpool->thread_count);
	jpool->cur_thread = (udo_atomic_u32 *) jpool->queue_data;

	__atomic_store_n(jpool->cur_thread, 0, __ATOMIC_SEQ_CST);

	for (t = 0; t < jpool->thread_count; t++) {
		queue = &(jpool->threads[t].queue);
		offset = sizeof(udo_atomic_u32) + (t * queue_size);
		queue->size = (udo_atomic_u32 *) \
			jpool->queue_data + offset;

		queue->offset = (udo_atomic_u32 *) jpool->queue_data + \
			offset + sizeof(udo_atomic_u32);

		queue->job_free = (udo_atomic_u32 *) jpool->queue_data + \
			offset + (2 * sizeof(udo_atomic_u32));

		queue->front = (udo_atomic_u32 *) jpool->queue_data + \
			offset + (3 * sizeof(udo_atomic_u32));

		queue->rear = (udo_atomic_u32 *) jpool->queue_data + \
			offset + (4 * sizeof(udo_atomic_u32));

		queue->data = (udo_atomic_u32 *) jpool->queue_data + \
			offset + (5 * sizeof(udo_atomic_u32));

		__atomic_store_n(queue->offset, offset, __ATOMIC_RELEASE);

		__atomic_store_n(queue->size, queue_size - \
				sizeof(struct udo_jpool_queue), \
				__ATOMIC_RELEASE);

		p_queue_reset(queue);
		err = pthread_create(&thread, NULL, p_run_thread, queue);
		if (err) {
			udo_log_error("pthread_create: %s\n", strerror(errno));
			udo_jpool_destroy(jpool);
			return NULL;
		}

		jpool->threads[t].thread_id = thread;
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
	uint32_t thread_index;
	struct udo_jpool_job job;
	void *insert_queue = NULL;
	struct udo_jpool_queue *queue;

	if (!jpool) {
		udo_log_error("Incorrect data passed\n");
		return -1;
	}

	if (!func || !arg) {
		udo_log_set_error(jpool, UDO_LOG_ERR_INCORRECT_DATA, "");
		return -1;
	}

	thread_index = __atomic_add_fetch(jpool->cur_thread, \
			1, __ATOMIC_SEQ_CST);
	if (thread_index >= jpool->thread_count) {
		__atomic_store_n(jpool->cur_thread, 0, __ATOMIC_SEQ_CST);
		thread_index = 0;
	}

	queue = &(jpool->threads[thread_index].queue);
	if (p_queue_full(queue)) {
		udo_log_set_error(jpool, UDO_LOG_ERR_UNCOMMON, \
		                  "Job queue is full.");
		return -1;
	}

	insert_queue = (void *) \
		((char *) queue->data + \
		(p_queue_get_rear(queue)) + \
		sizeof(struct udo_jpool_job));

	job.func = func; job.arg = arg;
	memcpy(insert_queue, &job, sizeof(struct udo_jpool_job));

	__atomic_add_fetch(queue->rear, \
		sizeof(struct udo_jpool_job), \
		__ATOMIC_SEQ_CST);

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
	unsigned int t;
	struct udo_jpool_queue *queue;

	if (!jpool)
		return;

	for (t = 0; t < jpool->thread_count; t++) {
		queue = &(jpool->threads[t].queue);
		udo_futex_unlock_force(queue->job_free);
		udo_futex_wake_cond(queue->job_free);
		pthread_join(jpool->threads[t].thread_id, NULL);
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
