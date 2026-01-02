#ifndef UDO_TPOOL_H
#define UDO_TPOOL_H

#include "macros.h"

/*
 * Stores information about the udo_tpool instance.
 * tpool - Thread pool.
 */
struct udo_tpool;


/*
 * @brief Structure passed to udo_tpool_create() used
 *        to define size of shared memory queue and
 *        the amount of threads to create.
 *
 * @param queue_size   - Size of shared memory used to store
 *                       the queue shared between all threads.
 * @param thread_count - Amount of threads able to read and
 *                       write to and from the shared memory
 *                       block.
 */
struct udo_tpool_create_info
{
	size_t       queue_size;
	unsigned int thread_count;
};


/*
 * @brief Creates pool a threads to execute task.
 *
 * @param tpool      - May be NULL or a pointer to a struct udo_tpool.
 *                     If NULL memory will be allocated and return to
 *                     caller. If not NULL address passed will be used
 *                     to store the newly created struct udo_tpool
 *                     instance.
 * @param tpool_info - Implementation uses a pointer to a
 *                     struct udo_tpool_create_info
 *                     no other implementation may be passed to
 *                     this parameter.
 *
 * @return
 *	on success: Pointer to a struct udo_tpool
 *	on failure: NULL
 */
UDO_API
struct udo_tpool *
udo_tpool_create (struct udo_tpool *tpool,
                  const void *tpool_info);


/*
 * @brief Frees any allocated memory and closes FD's (if open) create after
 *        udo_tpool_create() call.
 *
 * @param tpool - Pointer to a valid struct udo_tpool.
 */
UDO_API
void
udo_tpool_destroy (struct udo_tpool *tpool);


/*
 * @brief Returns size of the internal structure. So,
 *        if caller decides to allocate memory outside
 *        of API interface they know the exact amount
 *        of bytes.
 *
 * @return
 *	on success: sizeof(struct udo_tpool)
 *	on failure: sizeof(struct udo_tpool)
 */
UDO_API
int
udo_tpool_get_sizeof (void);

#endif /* UDO_TPOOL_H */
