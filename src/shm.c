#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "log.h"
#include "futex.h"
#include "shm.h"

/*
 * Maximum amount of processes
 * allowed to watch shared memory.
 */
#define SHM_PROC_MAX (1<<4)
#define SHM_FILE_NAME_MAX (1<<5)

#define UDO_FUTEX_LOCK 1
#define UDO_FUTEX_UNLOCK 0

/*
 * @brief Structure defining the cando_shm_proc
 *        (Cando Shared Memory Process) instance.
 *
 * @member rd_fux  - Pointer to a given process read futex
 *                   stored in front segment of shared memory.
 * @member wr_fux  - Pointer to a given process write futex
 *                   stored in front segment of shared memory.
 * @member data    - Unsigned long long int storing the integer
 *                   representation of a pointer to a location
 *                   within shared memory. This pointer is a
 *                   given processes shared memory segment
 *                   staring address.
 * @member data_sz - Stores the size of a given processes
 *                   shared memory segment.
 */
struct cando_shm_proc
{
	udo_atomic_u32  *rd_fux;
	udo_atomic_u32  *wr_fux;
	udo_atomic_addr data;
	size_t          data_sz;
};


/*
 * @brief Structure defining the cando_shm instance.
 *
 * @member err      - Stores information about the error that occured
 *                    for the given instance and may later be retrieved
 *                    by caller.
 * @member free     - If structure allocated with calloc(3) member will be
 *                    set to true so that, we know to call free(3) when
 *                    destroying the instance.
 * @member fd       - Open file descriptor to POSIX shared memory.
 * @member shm_file - Name of the POSIX shared memory file starting with '/'.
 * @member data     - Pointer to mmap(2) map'd shared memory data.
 * @member data_sz  - Total size of the shared memory region mapped with mmap(2).
 * @member procs    - An array storing the shared memory locations
 *                    of each processes futexes and data.
 */
struct cando_shm
{
	struct udo_log_error_struct err;
	bool                        free;
	int                         fd;
	char                        shm_file[SHM_FILE_NAME_MAX];
	void                        *data;
	size_t                      data_sz;
	struct cando_shm_proc       procs[SHM_PROC_MAX];
};


/***************************************
 * Start of cando_shm_create functions *
 ***************************************/

static int
p_shm_create (struct cando_shm *shm,
              const struct cando_shm_create_info *shm_info)
{
	unsigned int p;

	int err = -1, len;

	size_t data_off, fux_off, proc_data_sz;

	if (!(shm_info->proc_count) || \
	    shm_info->proc_count >= SHM_PROC_MAX)
	{
		udo_log_set_error(shm, UDO_LOG_ERR_UNCOMMON,
		                  "Unsupported process count (%u:%u)",
		                  shm_info->proc_count, SHM_PROC_MAX);
		return -1;
	}

	if (!(shm_info->shm_size)) {
		udo_log_set_error(shm, UDO_LOG_ERR_UNCOMMON,
		                  "Shared memory size must not be zero",
		                  shm_info->shm_size);
		return -1;
	}

	if (shm_info->shm_file[0] != '/') {
		udo_log_set_error(shm, UDO_LOG_ERR_UNCOMMON,
		                  "Shared memory file name '%s' doesn't start with '/'",
		                  shm_info->shm_file);
		return -1;
	}

	len = strnlen(shm_info->shm_file, SHM_FILE_NAME_MAX);
	if (len >= SHM_FILE_NAME_MAX) {
		udo_log_set_error(shm, UDO_LOG_ERR_UNCOMMON,
		                  "Shared memory '%s' name length to long",
		                  shm_info->shm_file);
		return -1;
	}

	strncpy(shm->shm_file, shm_info->shm_file, len);
	shm->fd = shm_open(shm->shm_file, O_RDWR|O_CREAT, 0644);
	if (shm->fd == -1) {
		udo_log_set_error(shm, errno, "shm_open: %s", strerror(errno));
		return -1;
	}

	shm->data_sz = shm_info->shm_size;
	err = ftruncate(shm->fd, shm->data_sz);
	if (err == -1) {
		udo_log_set_error(shm, errno, "ftruncate: %s", strerror(errno));
		return -1;
	}

	shm->data = mmap(NULL, shm->data_sz,
	                 PROT_READ|PROT_WRITE,
	                 MAP_SHARED, shm->fd, 0);
	if (err == -1) {
		udo_log_set_error(shm, errno, "mmap: %s", strerror(errno));
		return -1;
	}

	/*
	 * First 4 bytes of shared memory used to store
	 * amount of processes watching shared memory.
	 *
	 * The next X amount of bytes (2 * 4 * proc_count)
	 * stores each processes read/write futexes.
	 */
	if (__atomic_load_n((udo_atomic_u32*)shm->data, \
	    __ATOMIC_ACQUIRE) >= shm_info->proc_count)
	{
		udo_log_set_error(shm, UDO_LOG_ERR_UNCOMMON,
		                  "Unsupported process count (%u:%u)",
		                  shm_info->proc_count, SHM_PROC_MAX);
		return -1;
	}

	__atomic_add_fetch((udo_atomic_u32*)shm->data, 1, __ATOMIC_SEQ_CST);

	fux_off = sizeof(udo_atomic_u32);
	data_off = fux_off + (2 * sizeof(udo_atomic_u32) * shm_info->proc_count);
	proc_data_sz = (shm->data_sz - data_off) / shm_info->proc_count;

	for (p = 0; p < shm_info->proc_count; p++) {
		shm->procs[p].data_sz = proc_data_sz;
		shm->procs[p].data = (udo_atomic_addr)((char*)shm->data + data_off);

		shm->procs[p].rd_fux = (udo_atomic_u32*)((char*)shm->data + fux_off);
		shm->procs[p].wr_fux = (udo_atomic_u32*)((char*)shm->data + \
				fux_off + sizeof(udo_atomic_u32));

		/* Initialize read futex to lock state */
		__atomic_compare_exchange_n(shm->procs[p].rd_fux, \
			&(udo_atomic_u32){UDO_FUTEX_UNLOCK}, \
			UDO_FUTEX_LOCK, 0, __ATOMIC_SEQ_CST, \
			__ATOMIC_SEQ_CST);

		/* Initialize write futex to unlocked state (just in case). */
		__atomic_compare_exchange_n(shm->procs[p].wr_fux, \
			&(udo_atomic_u32){UDO_FUTEX_UNLOCK}, \
			UDO_FUTEX_UNLOCK, 0, __ATOMIC_SEQ_CST, \
			__ATOMIC_SEQ_CST);

		data_off += proc_data_sz;
		fux_off += (2 * sizeof(udo_atomic_u32));
	}

	return 0;
}


struct cando_shm *
cando_shm_create (struct cando_shm *p_shm,
                  const void *p_shm_info)
{
	int err = -1;

	struct cando_shm *shm = p_shm;

	const struct cando_shm_create_info *shm_info = p_shm_info;

	if (!shm) {
		shm = calloc(1, sizeof(struct cando_shm));
		if (!shm) {
			udo_log_error("calloc: %s\n", strerror(errno));
			return NULL;
		}

		shm->free = true;
	}

	err = p_shm_create(shm, shm_info);
	if (err == -1) {
		udo_log_error("%s\n", udo_log_get_error(shm));
		cando_shm_destroy(shm);
		return NULL;
	}

	return shm;
}

/*************************************
 * End of cando_shm_create functions *
 *************************************/


/*************************************
 * Start of cando_shm_data functions *
 *************************************/

UDO_STATIC_INLINE
unsigned char
p_check_proc_index (struct cando_shm *shm,
                    const unsigned int proc_index)
{
	return proc_index > __atomic_load_n((udo_atomic_u32*) \
			shm->data, __ATOMIC_ACQUIRE);
}


int
cando_shm_data_read (struct cando_shm *shm,
                     const void *p_shm_info)
{
	int data;

	size_t s;

	const struct cando_shm_proc *shm_proc;
	const struct cando_shm_data_info *shm_info = p_shm_info;

	if (!shm)
		return -1;

	if (!shm_info || \
	    !(shm_info->data) || \
	    p_check_proc_index(shm, shm_info->proc_index))
	{
		udo_log_set_error(shm, UDO_LOG_ERR_INCORRECT_DATA, "");
		return -1;
	}

	shm_proc = &(shm->procs[shm_info->proc_index]);

	udo_futex_lock(shm_proc->rd_fux);
	if (errno == EINTR)
		return -errno;

	for (s = 0; s < shm_info->size; s += sizeof(int)) {
		data = __atomic_load_n((udo_atomic_int*) \
			((char*)shm_proc->data + s), \
			__ATOMIC_ACQUIRE);

		__atomic_store_n((udo_atomic_int*) \
			((char*)shm_info->data + s), \
			data, __ATOMIC_RELEASE);

		__atomic_clear((udo_atomic_int*) \
			((char*)shm_proc->data + s),
			__ATOMIC_RELEASE);
	}

	udo_futex_unlock(shm_proc->wr_fux);

	return 0;
}


int
cando_shm_data_write (struct cando_shm *shm,
                      const void *p_shm_info)
{
	size_t s;

	const struct cando_shm_proc *shm_proc;
	const struct cando_shm_data_info *shm_info = p_shm_info;

	if (!shm)
		return -1;

	if (!shm_info || \
	    !(shm_info->data) || \
	    p_check_proc_index(shm, shm_info->proc_index))
	{
		udo_log_set_error(shm, UDO_LOG_ERR_INCORRECT_DATA, "");
		return -1;
	}

	shm_proc = &(shm->procs[shm_info->proc_index]);

	udo_futex_lock(shm_proc->wr_fux);
	if (errno == EINTR)
		return -errno;

	for (s = 0; s < shm_info->size; s += sizeof(int)) {
		__atomic_store_n((udo_atomic_int*) \
			((char*)shm_proc->data + s), \
			*((int*)((char*)shm_info->data + s)), \
			__ATOMIC_RELEASE);
	}

	udo_futex_unlock(shm_proc->rd_fux);

	return 0;
}

/***********************************
 * End of cando_shm_data functions *
 ***********************************/


/************************************
 * Start of cando_shm_get functions *
 ************************************/

int
cando_shm_get_fd (struct cando_shm *shm)
{
	if (!shm)
		return -1;

	return shm->fd;
}


void *
cando_shm_get_data (struct cando_shm *shm,
                    const unsigned int proc_index)
{
	if (!shm || p_check_proc_index(shm, proc_index))
		return NULL;

	return shm->procs[proc_index].data;
}


size_t
cando_shm_get_data_size (struct cando_shm *shm,
                         const unsigned int proc_index)
{
	if (!shm || p_check_proc_index(shm, proc_index))
		return -1;

	return shm->procs[proc_index].data_sz;
}

/************************************
 * Start of cando_shm_get functions *
 ************************************/


/****************************************
 * Start of cando_shm_destroy functions *
 ****************************************/

void
cando_shm_destroy (struct cando_shm *shm)
{
	int value = -1;

	if (!shm)
		return;

	if (shm->data) {
		value = (int) __atomic_sub_fetch((udo_atomic_u32*) \
			shm->data, 1, __ATOMIC_SEQ_CST);
		munmap(shm->data, shm->data_sz);
	}

	close(shm->fd);

	if (value == 0)
		shm_unlink(shm->shm_file);

	if (shm->free) {
		free(shm);
	} else {
		memset(shm, 0, sizeof(struct cando_shm));
	}
}

/****************************************
 * Start of cando_shm_destroy functions *
 ****************************************/


/*************************************************
 * Start of non struct cando_shm param functions *
 *************************************************/

int
cando_shm_get_sizeof (void)
{
	return sizeof(struct cando_shm);
}

/***********************************************
 * End of non struct cando_shm param functions *
 ***********************************************/
