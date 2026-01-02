#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "log.h"
#include "futex.h"
#include "tpool.h"

/*
 * @brief Structure defining the udo_tpool instance.
 *
 * @member err  - Stores information about the error that occured
 *                for the given instance and may later be retrieved
 *                by caller.
 * @member free - If structure allocated with calloc(3) member will be
 *                set to true so that, we know to call free(3) when
 *                destroying the instance.
 */
struct udo_tpool
{
	struct udo_log_error_struct err;
	bool                        free;
	void                        *queue;
};


/***************************************
 * Start of udo_tpool_create functions *
 ***************************************/

struct udo_tpool *
udo_tpool_create (struct udo_tpool *p_tpool,
                  const void *p_tpool_info)
{
	struct udo_tpool *tpool = p_tpool;
	const struct udo_tpool_create_info *tpool_info = p_tpool_info;

	struct udo_futex_create_info futex_info;

	if (!tpool_info) {
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

	futex_info.count = 1;
	futex_info.size = tpool_info->size;
	tpool->queue = udo_futex_create(&futex_info);
	if (!(tpool->queue)) {
		udo_tpool_destroy(tpool);
		return NULL;
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

	udo_futex_destroy(tpool->queue);

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
