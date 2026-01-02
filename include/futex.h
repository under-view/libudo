#ifndef UDO_FUTEX_H
#define UDO_FUTEX_H

#include "macros.h"

/*
 * @brief Structure passed to udo_futex_create(3) used
 *        to define size of shared memory and amount of
 *        futexes contained at the start of shared memory.
 *
 * @member size  - Size of shared memory block.
 * @member count - Amount of futexes stored in a single
 *                 shared memory block.
 */
struct udo_futex_create_info
{
	size_t       size;
	unsigned int count;
};


/*
 * @brief Allocates shared memory space that may be used
 *        to store a futex. This function usage should
 *        be limited to processes/threads that were created
 *        via fork() or pthread_create(). For processes
 *        created without fork() (i.e seperate application)
 *        see shm.c implementation. By default all futexes
 *        are initialize in the locked state.
 *
 * @param futex_info - Implementation uses a pointer to a
 *                     struct udo_futex_create_info
 *                     no other implementation may be passed
 *                     to this parameter.
 *
 * @return
 *	on success: Pointer to a udo_atomic_u32
 *	on failure: NULL
 */
UDO_API
udo_atomic_u32 *
udo_futex_create (const void *futex_info);


/*
 * @brief Atomically updates futex value to the locked state.
 *        If value can't be changed inform kernel that a
 *        process needs to be put to sleep. Sets errno to
 *        EINTR if a call to udo_futex_unlock_force()
 *        is made.
 *
 * @param fux - Pointer to 32-bit integer storing futex.
 */
UDO_API
void
udo_futex_lock (udo_atomic_u32 *fux);


/*
 * @brief Atomically update futex value to the unlocked state.
 *        Then inform kernel to wake up all processes/threads
 *        watching the futex.
 *
 * @param fux - Pointer to 32-bit integer storing futex.
 */
UDO_API
void
udo_futex_unlock (udo_atomic_u32 *fux);


/*
 * @brief Atomically update futex value to the force unlocked state.
 *        Then inform kernel to wake up all processes/threads
 *        watching the futex. When force unlocking if a
 *        process/thread is waiting on the lock. Process/thread
 *        will exit setting errno to EINTR. Recommended to use
 *        function in a signal handler.
 *
 * @param fux - Pointer to 32-bit integer storing futex.
 */
UDO_API
void
udo_futex_unlock_force (udo_atomic_u32 *fux);


/*
 * @brief Frees any allocated memory and closes FD's (if open)
 *        created after udo_futex_create() call.
 *
 * @param fux - Pointer to 32-bit integer storing futex.
 */
UDO_API
void
udo_futex_destroy (udo_atomic_u32 *fux);

#endif /* UDO_FUTEX_H */
