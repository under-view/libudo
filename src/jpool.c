#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <pthread.h>

#include "log.h"
#include "futex.h"
#include "jpool.h"

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
 * @member job_free - Futex used to wake threads or put them
 *                    to sleep if jobs are available.
 * @member front    - Byte offset to the front of the queue.
 * @member rear     - Bytes offset to the rear of the queue.
 */
struct udo_jpool_queue
{
	udo_atomic_u32 *job_free;
	udo_atomic_u32 *front;
	udo_atomic_u32 *rear;
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
 * @member thread_count - Amount of threads in the pool.
 * @member thread_ids   - Array of POSIX thread ID's.
 * @member queue_sz     - Byte size of @queue.
 * @member queue        - Structure keeping track of current jobs
 *                        a thread can run.
 * @member queue_data   - Shared memory buffer storing actual
 *                        addresses to jobs.
 */
struct udo_jpool
{
	struct udo_log_error_struct err;
	bool                        free;
	unsigned int                thread_count;
	pthread_t                   thread_ids[THREADS_MAX];
	size_t                      queue_sz;
	struct udo_jpool_queue      queue;
	void                        *queue_data;
};


/*****************************************
 * Start of global to C source functions *
 *****************************************/

UDO_STATIC_INLINE
void
p_queue_reset (struct udo_jpool_queue *queue)
{
	__atomic_store_n(queue->job_free, \
			0, __ATOMIC_RELEASE);
	__atomic_store_n(queue->front, \
			sizeof(struct udo_jpool_queue), \
			__ATOMIC_RELEASE);
	__atomic_store_n(queue->rear, \
			sizeof(struct udo_jpool_queue), \
			__ATOMIC_RELEASE);
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
		sizeof(struct udo_jpool_queue);
}


UDO_STATIC_INLINE
bool
p_queue_full (const struct udo_jpool_queue *queue,
              const size_t queue_sz)
{
	return p_queue_get_rear(queue) >= \
		(queue_sz - sizeof(struct udo_jpool_job));
}


static struct udo_jpool_job *
p_queue_get_job (struct udo_jpool_queue *queue,
                 const size_t queue_sz,
                 char *queue_data)
{
	uint32_t front;

	front = __atomic_add_fetch(queue->front, \
		sizeof(struct udo_jpool_job), \
		__ATOMIC_RELEASE);

	if (p_queue_full(queue, queue_sz))
		p_queue_reset(queue);

	return (struct udo_jpool_job*)(queue_data+front);
}


static void *
p_run_thread (void *p_tool)
{
	size_t queue_sz;

	char *queue_data = NULL;
	struct udo_jpool_job *job = NULL;

	struct udo_jpool *jpool = p_tool;
	struct udo_jpool_queue *queue = &(jpool->queue);

	queue_sz = jpool->queue_sz;
	queue_data = (char *) jpool->queue_data;

	while (p_queue_get_rear(queue) && \
	       !p_queue_get_job_free(queue))
	{
		udo_futex_wait_cond(queue->job_free, \
			p_queue_can_get_job(queue));

		job = p_queue_get_job(queue, \
			queue_sz, queue_data);
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

	unsigned int t;

	pthread_t thread;

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

	jpool->thread_count = jpool_info->count;

	futex_info.count = 3; /* Byte align on 4K boundary */
	futex_info.size = UDO_BYTE_ALIGN(jpool_info->size, UDO_PAGE_SIZE);
	jpool->queue_data = udo_futex_create(&futex_info);
	if (!(jpool->queue_data)) {
		udo_jpool_destroy(jpool);
		return NULL;
	}

	jpool->queue_sz = futex_info.size;
	jpool->queue.front = (udo_atomic_u32*) \
		jpool->queue_data + sizeof(udo_atomic_u32);
	jpool->queue.rear = (udo_atomic_u32*) \
		jpool->queue_data + (2 * sizeof(udo_atomic_u32));
	jpool->queue.job_free = (udo_atomic_u32*) jpool->queue_data;

	p_queue_reset(&(jpool->queue));

	for (t = 0; t < jpool->thread_count; t++) {
		err = pthread_create(&thread, NULL, p_run_thread, jpool);
		if (err) {
			udo_log_error("pthread_create: %s\n", strerror(errno));
			udo_jpool_destroy(jpool);
			return NULL;
		}

		jpool->thread_ids[t] = thread;
		memset(&thread, 0, sizeof(thread));
	}

	return jpool;
}

/*************************************
 * End of udo_jpool_create functions *
 *************************************/


/****************************************
 * Start of udo_jpool_add_job functions *
 ****************************************/

uint32_t
udo_jpool_add_job (struct udo_jpool *jpool,
                   void (*func)(void *arg),
                   void *arg)
{
	uint32_t ret;

	struct udo_jpool_job job;

	void *insert_queue = NULL;

	if (!jpool) {
		udo_log_error("Incorrect data passed\n");
		return UINT32_MAX;
	}

	if (!func || !arg) {
		udo_log_set_error(jpool, UDO_LOG_ERR_INCORRECT_DATA, "");
		return UINT32_MAX;
	}

	if (p_queue_full(&(jpool->queue), jpool->queue_sz)) {
		udo_log_set_error(jpool, UDO_LOG_ERR_UNCOMMON, \
		                  "Job queue is full.");
		return -1;
	}

	insert_queue = (void *) ((char *) jpool->queue_data + \
		(p_queue_get_rear(&(jpool->queue)) + sizeof(struct udo_jpool_job)));

	job.func = func; job.arg = arg;
	memcpy(insert_queue, &job, sizeof(struct udo_jpool_job));

	ret = __atomic_add_fetch(jpool->queue.rear, \
			sizeof(struct udo_jpool_job), \
			__ATOMIC_SEQ_CST);

	udo_futex_wake_cond(jpool->queue.job_free);

	return ret;
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

	if (!jpool)
		return;

	udo_futex_unlock_force(jpool->queue.job_free);
	udo_futex_wake_cond(jpool->queue.job_free);

	for (t = 0; t < jpool->thread_count; t++)
		pthread_join(jpool->thread_ids[t], NULL);

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
