#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <linux/futex.h>  /* Definition of FUTEX_* constants */
#include <sys/syscall.h>  /* Definition of SYS_* constants */
#include <sys/mman.h>

#include "log.h"
#include "futex.h"

#define UDO_FUTEX_LOCK 1
#define UDO_FUTEX_UNLOCK 0
#define UDO_FUTEX_UNLOCK_FORCE 0x66AFB55C
#define CONTENTION_LOOP_CNT 999999999

/*****************************************
 * Start of global to C source functions *
 *****************************************/

UDO_STATIC_INLINE
int
futex (void *uaddr,
       int op,
       uint32_t val,
       const struct timespec *timeout,
       void *uaddr2,
       uint32_t val3)
{
	return syscall(SYS_futex, uaddr, op, val,
	               timeout, uaddr2, val3);
}

/***************************************
 * End of global to C source functions *
 ***************************************/


/***************************************
 * Start of udo_futex_create functions *
 ***************************************/

udo_atomic_u32 *
udo_futex_create (const void *p_futex_info)
{
	unsigned int f;

	udo_atomic_u32 *fux;

	const struct udo_futex_create_info *futex_info = p_futex_info;

	if (!futex_info ||
	    !(futex_info->count) ||
	    !(futex_info->size))
	{
		udo_log_error("Incorrect data passed\n");
		return NULL;
	}

	/* mmap will just allocate a page anyways */
	fux = mmap(NULL, futex_info->size,
	           PROT_READ|PROT_WRITE,
	           MAP_SHARED|MAP_ANONYMOUS,
	           -1, 0);
	if (fux == (void*)-1) {
		udo_log_error("mmap: %s\n", strerror(errno));
		return NULL;
	}

	for (f = 0; f < futex_info->count; f++) {
		__atomic_store_n((udo_atomic_u32 *) \
			((char*)fux+(f*sizeof(udo_atomic_u32))),
			1, __ATOMIC_RELEASE);
	}

	return fux;
}

/*************************************
 * End of udo_futex_create functions *
 *************************************/


/*************************************
 * Start of udo_futex_lock functions *
 *************************************/

UDO_STATIC_INLINE
unsigned char
p_is_futex_funlock (udo_atomic_u32 *fux)
{
	return __atomic_load_n(fux, __ATOMIC_ACQUIRE) == UDO_FUTEX_UNLOCK_FORCE;
}


void
udo_futex_lock (udo_atomic_u32 *fux)
{
	int i;

	if (!fux)
		return;

	/* Handle Low Contention Case (Spin Loop/Spin Lock) */
	for (i = 0; i < CONTENTION_LOOP_CNT; i++) {
		if (__atomic_compare_exchange_n(fux, \
			&(udo_atomic_u32){UDO_FUTEX_UNLOCK}, \
			UDO_FUTEX_LOCK, 1, __ATOMIC_SEQ_CST, \
			__ATOMIC_SEQ_CST))
		{
			return;
		} else if (p_is_futex_funlock(fux)) {
			errno = EINTR;
			return;
		}
	}

	/* Blocking Or Sleeping Wait */
	while (1) {
		if (p_is_futex_funlock(fux))
		{
			errno = EINTR;
			return;
		}

		if (__atomic_compare_exchange_n(fux, \
			&(udo_atomic_u32){UDO_FUTEX_UNLOCK}, \
			UDO_FUTEX_LOCK, 0, __ATOMIC_SEQ_CST, \
			__ATOMIC_SEQ_CST))
		{
			return;
		}

		futex(fux, FUTEX_WAIT, UDO_FUTEX_LOCK, NULL, NULL, 0);
	}
}

/***********************************
 * End of udo_futex_lock functions *
 ***********************************/


/***************************************
 * Start of udo_futex_unlock functions *
 ***************************************/

void
udo_futex_unlock (udo_atomic_u32 *fux)
{
	if (!fux)
		return;

	__atomic_store_n(fux, UDO_FUTEX_UNLOCK, __ATOMIC_RELEASE);
	futex(fux, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);
}


void
udo_futex_unlock_force (udo_atomic_u32 *fux)
{
	if (!fux)
		return;

	__atomic_store_n(fux, UDO_FUTEX_UNLOCK_FORCE, __ATOMIC_RELEASE);
	futex(fux, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);
}

/*************************************
 * End of udo_futex_unlock functions *
 *************************************/


/****************************************
 * Start of udo_futex_destroy functions *
 ****************************************/

void
udo_futex_destroy (udo_atomic_u32 *fux)
{
	if (!fux)
		return;

	munmap(fux, sizeof(udo_atomic_u32));
}

/**************************************
 * End of udo_futex_destroy functions *
 **************************************/
