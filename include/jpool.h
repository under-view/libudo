#ifndef UDO_JPOOL_H
#define UDO_JPOOL_H

#include <inttypes.h>
#include "macros.h"

/*
 * Stores information about the udo_jpool instance.
 * jpool - Job pool.
 */
struct udo_jpool;


/*
 * @brief Structure passed to udo_jpool_create() used
 *        to define size of shared memory queue and
 *        the amount of threads to create.
 *
 * @param size  - Size of shared memory used to store
 *                the queue shared between all threads.
 * @param count - Amount of threads able to read and
 *                write to and from the shared memory
 *                block.
 */
struct udo_jpool_create_info
{
	size_t       size;
	unsigned int count;
};


/*
 * @brief Creates pool a threads to execute task.
 *
 * @param jpool      - May be NULL or a pointer to a struct udo_jpool.
 *                     If NULL memory will be allocated and return to
 *                     caller. If not NULL address passed will be used
 *                     to store the newly created struct udo_jpool
 *                     instance.
 * @param jpool_info - Implementation uses a pointer to a
 *                     struct udo_jpool_create_info
 *                     no other implementation may be passed to
 *                     this parameter.
 *
 * @returns
 *	on success: Pointer to a struct udo_jpool
 *	on failure: NULL
 */
UDO_API
struct udo_jpool *
udo_jpool_create (struct udo_jpool *jpool,
                  const void *jpool_info);


/*
 * @brief Adds a job to the job queue for threads
 *        to then later execute.
 *
 * @param jpool - Pointer to a valid struct udo_jpool.
 * @param func  - Pointer to function that a seperate
 *                thread will execute.
 * @param arg   - Pointer to a memory which will be
 *                passed as the argument to @func.
 *
 * @returns
 *	on success: Queue buffer byte offset
 *	on failure: -1 or UINT32_MAX
 */
UDO_API
uint32_t
udo_jpool_add_job (struct udo_jpool *jpool,
                   void (*func)(void *arg),
                   void *arg);


/*
 * @brief Frees any allocated memory and closes FD's (if open) create after
 *        udo_jpool_create() call.
 *
 * @param jpool - Pointer to a valid struct udo_jpool.
 */
UDO_API
void
udo_jpool_destroy (struct udo_jpool *jpool);


/*
 * @brief Returns size of the internal structure. So,
 *        if caller decides to allocate memory outside
 *        of API interface they know the exact amount
 *        of bytes.
 *
 * @returns
 *	on success: sizeof(struct udo_jpool)
 *	on failure: sizeof(struct udo_jpool)
 */
UDO_API
int
udo_jpool_get_sizeof (void);

#endif /* UDO_JPOOL_H */
