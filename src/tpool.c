#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>

#include "log.h"
#include "futex.h"
#include "tpool.h"

/*
 * @brief Structure defining information about the job to execute.
 *
 * @member func - Function pointer to a function for thread to execute.
 * @member arg  - Argument to pass to function.
 */
struct udo_tpool_job
{
	void (*func)(void *arg);
	void *arg;
};


/*
 * @brief Structure defining information about the job queue.
 *
 * @member has_job - Futex determining if queue has an available job.
 * @member front   - Pointer to the front of the queue.
 * @member rear    - Pointer to the job at the back of the queue.
 */
struct udo_tpool_jobqueue
{
	udo_atomic_u32       *has_job;
	struct udo_tpool_job *front;
	struct udo_tpool_job *rear;
};


/*
 * @brief Structure defining the udo_tpool instance.
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
struct udo_tpool
{
	struct udo_log_error_struct err;
	bool                        free;
	unsigned int                thread_count;
	struct udo_tpool_jobqueue   *queue;
};


/*****************************************
 * Start of global to C source functions *
 *****************************************/

void *
p_run_thread (void *p_tool)
{
	struct udo_tpool *tpool = p_tool;
	udo_log_info("thread_count = %d\n", tpool->thread_count);
	return NULL;
}

/***************************************
 * End of global to C source functions *
 ***************************************/


/***************************************
 * Start of udo_tpool_create functions *
 ***************************************/

struct udo_tpool *
udo_tpool_create (struct udo_tpool *p_tpool,
                  const void *p_tpool_info)
{
	int err;

	unsigned int t;

	pthread_t thread;

	struct udo_tpool *tpool = p_tpool;
	const struct udo_tpool_create_info *tpool_info = p_tpool_info;

	struct udo_futex_create_info futex_info;

	if (!tpool_info ||
	    !(tpool_info->count) ||
	    !(tpool_info->size))
	{
		udo_log_error("Incorrect data passed\n");
		return NULL;
	}

	if (!tpool) {
		tpool = calloc(1, sizeof(struct udo_tpool));
		if (!tpool) {
			udo_log_error("calloc: %s\n", strerror(errno));
			return NULL;
		}

		tpool->free = true;
	}

	tpool->thread_count = tpool_info->count;

	futex_info.count = 1;
	futex_info.size = tpool_info->size;
	tpool->queue = (struct udo_tpool_jobqueue*) \
		udo_futex_create(&futex_info);
	if (!(tpool->queue)) {
		udo_tpool_destroy(tpool);
		return NULL;
	}

	for (t = 0; t < tpool->thread_count; t++) {
		err = pthread_create(&thread, NULL, p_run_thread, tpool);
		if (err) {
			udo_log_error("pthread_create: %s\n", strerror(errno));
			udo_tpool_destroy(tpool);
			return NULL;
		}

		err = pthread_detach(thread);
		if (err) {
			udo_log_error("pthread_detach: %s\n", strerror(errno));
			udo_tpool_destroy(tpool);
			return NULL;
		}

		memset(&thread, 0, sizeof(thread));
	}

	return tpool;
}

/*************************************
 * End of udo_tpool_create functions *
 *************************************/


/****************************************
 * Start of udo_tpool_destroy functions *
 ****************************************/

void
udo_tpool_destroy (struct udo_tpool *tpool)
{
	if (!tpool)
		return;

	udo_futex_destroy((udo_atomic_u32*)tpool->queue);

	if (tpool->free) {
		free(tpool);
	} else {
		memset(tpool, 0, sizeof(struct udo_tpool));
	}
}

/**************************************
 * End of udo_tpool_destroy functions *
 **************************************/


/*************************************************
 * Start of non struct udo_tpool param functions *
 *************************************************/

int
udo_tpool_get_sizeof (void)
{
	return sizeof(struct udo_tpool);
}

/***********************************************
 * End of non struct udo_tpool param functions *
 ***********************************************/
