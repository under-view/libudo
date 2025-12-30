#ifndef UDO_FUTEX_H
#define UDO_FUTEX_H

#include "macros.h"

/*
 * @brief Allocates shared memory space that may be used
 *        to store a futex. This function usage should
 *        be limited to processes/threads that were created
 *        via fork() or pthread_create(). For processes
 *        created without fork() see shm.c implementation.
 *        By default all futexes are initialize in the locked
 *        state.
 *
 * @param count - Amount of futexes stored in a single
 *                shared memory block.
 *
 * @return
 *	on success: Pointer to a udo_atomic_u32
 *	on failure: NULL
 */
UDO_API
udo_atomic_u32 *
cando_futex_create (const unsigned int count);


/*
 * @brief Atomically updates futex value to the locked state.
 *        If value can't be changed inform kernel that a
 *        process needs to be put to sleep. Sets errno to
 *        EINTR if a call to cando_futex_unlock_force()
 *        is made.
 *
 * @param fux - Pointer to 32-bit integer storing futex.
 */
UDO_API
void
cando_futex_lock (udo_atomic_u32 *fux);


/*
 * @brief Atomically update futex value to the unlocked state.
 *        Then inform kernel to wake up all processes/threads
 *        watching the futex.
 *
 * @param fux - Pointer to 32-bit integer storing futex.
 */
UDO_API
void
cando_futex_unlock (udo_atomic_u32 *fux);


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
cando_futex_unlock_force (udo_atomic_u32 *fux);


/*
 * @brief Frees any allocated memory and closes FD's (if open)
 *        created after cando_futex_create() call.
 *
 * @param fux - Pointer to 32-bit integer storing futex.
 */
UDO_API
void
cando_futex_destroy (udo_atomic_u32 *fux);

#endif /* UDO_FUTEX_H */
