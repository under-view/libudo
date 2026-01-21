#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>

#include "log.h"
#include "futex.h"
#include "jpool.h"

/*
 * @brief Structure defining information about the job to execute.
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
 * @brief Structure defining information about the job queue.
 *
 * @member num_jobs - Number of jobs in the queue.
 * @member front    - Byte offset to the front of the queue.
 * @member rear     - Bytes offset to the back of the queue.
 */
struct udo_jpool_jobqueue
{
	udo_atomic_u32 num_jobs;
	udo_atomic_u32 front;
	udo_atomic_u32 rear;
};


/*
 * @brief Structure defining the udo_jpool instance.
 *
 * @member err          - Stores information about the error that occured
 *                        for the given instance and may later be retrieved
 *                        by caller.
 * @member free         - If structure allocated with calloc(3) member will be
 *                        set to true so that, we know to call free(3) when
 *                        destroying the instance.
 * @member thread_count - Amount of threads in the pool.
 * @member queue        - Structure keeping track of current job
 *                        a thread can run.
 */
struct udo_jpool
{
	struct udo_log_error_struct err;
	bool                        free;
	unsigned int                thread_count;
	struct udo_jpool_jobqueue   *queue;
};


/*****************************************
 * Start of global to C source functions *
 *****************************************/

void *
p_run_thread (void *p_tool)
{
	struct udo_jpool *jpool = p_tool;
	udo_log_info("thread_count = %d\n", jpool->thread_count);
	return NULL;
}

UDO_STATIC_INLINE
void
p_reset_queue (struct udo_jpool_jobqueue *queue)
{
	__atomic_store_n(&(queue->front),
			sizeof(struct udo_jpool_jobqueue),
			__ATOMIC_RELEASE);
	__atomic_store_n(&(queue->rear),
			sizeof(struct udo_jpool_jobqueue),
			__ATOMIC_RELEASE);
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

	if (!jpool_info ||
	    !(jpool_info->count) ||
	    !(jpool_info->size))
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

	futex_info.count = 3;
	futex_info.size = jpool_info->size;
	jpool->queue = (struct udo_jpool_jobqueue *) \
		udo_futex_create(&futex_info);
	if (!(jpool->queue)) {
		udo_jpool_destroy(jpool);
		return NULL;
	}

	p_reset_queue(jpool->queue);

	for (t = 0; t < jpool->thread_count; t++) {
		err = pthread_create(&thread, NULL, p_run_thread, jpool);
		if (err) {
			udo_log_error("pthread_create: %s\n", strerror(errno));
			udo_jpool_destroy(jpool);
			return NULL;
		}

		err = pthread_detach(thread);
		if (err) {
			udo_log_error("pthread_detach: %s\n", strerror(errno));
			udo_jpool_destroy(jpool);
			return NULL;
		}

		memset(&thread, 0, sizeof(thread));
	}

	return jpool;
}

/*************************************
 * End of udo_jpool_create functions *
 *************************************/


/****************************************
 * Start of udo_jpool_destroy functions *
 ****************************************/

void
udo_jpool_destroy (struct udo_jpool *jpool)
{
	if (!jpool)
		return;

	udo_futex_destroy((udo_atomic_u32*)jpool->queue);

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
