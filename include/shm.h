#ifndef UDO_SHM_H
#define UDO_SHM_H

#include "macros.h"

/*
 * Stores information about the udo_shm instace.
 */
struct udo_shm;


/*
 * @brief Structure passed to udo_shm_create() used
 *        to define shared memory file name, shm size,
 *        and process count.
 *
 * @param shm_file   - Shared memory file name. Must start
 *                     with the character '/'.
 * @param shm_size   - Size of shared memory.
 * @param proc_count - Amount of processes able to read and
 *                     write to and from the shared memory
 *                     block.
 */
struct udo_shm_create_info
{
	const char   *shm_file;
	size_t       shm_size;
	unsigned int proc_count;
};


/*
 * @brief Creates POSIX shared memory and futexes.
 *        Each process gets:
 *        	1. Read futex (initialized to locked)
 *        	2. Write futex (initialized to unlocked)
 *        	3. Segment within shared memory to store data
 *
 * @param shm      - May be NULL or a pointer to a struct udo_shm.
 *                   If NULL memory will be allocated and return to
 *                   caller. If not NULL address passed will be used
 *                   to store the newly created struct udo_shm
 *                   instance.
 * @param shm_info - Implementation uses a pointer to a
 *                   struct udo_shm_create_info
 *                   no other implementation may be passed to
 *                   this parameter.
 *
 * @returns
 *	on success: Pointer to a struct udo_shm
 *	on failure: NULL
 */
UDO_API
struct udo_shm *
udo_shm_create (struct udo_shm *shm,
                const void *shm_info);


/*
 * @brief Structure defining what operations to perform
 *        and data to retrieve during calls to
 *        udo_shm_data_read() and udo_shm_data_write().
 *
 * @member data       - Pointer to a buffer that will either be used
 *                      to store shm data or write to shm data.
 * @member size       - Size in bytes to read from or write to shared memory.
 * @member proc_index - Index of process to write data to or read data from.
 */
struct udo_shm_data_info
{
	void         *data;
	size_t       size;
	unsigned int proc_index;
};


/*
 * @brief Reads data stored in shared memory at
 *        caller defined offset and writes into
 *        a caller defined buffer.
 *
 * @param shm      - Pointer to a valid struct udo_shm.
 * @param shm_info - Must pass a pointer to a struct udo_shm_data_info.
 *
 * @returns
 *	on success: 0
 *	on failure: -1
 */
UDO_API
int
udo_shm_data_read (struct udo_shm *shm,
                   const void *shm_info);


/*
 * @brief Write data stored in caller defined buffer
 *        into shared memory at a caller defined
 *        shared memory offset.
 *
 * @param shm      - Pointer to a valid struct udo_shm.
 * @param shm_info - Must pass a pointer to a struct udo_shm_data_info.
 *
 * @returns
 *	on success: 0
 *	on failure: -1
 */
UDO_API
int
udo_shm_data_write (struct udo_shm *shm,
                    const void *shm_info);


/*
 * @brief Returns file descriptor to the POSIX shared memory
 *        created after call to udo_shm_create().
 *
 * @param shm - Pointer to a valid struct udo_shm.
 *
 * @returns
 *	on success: File descriptor to POSIX shared memory
 *	on failure: -1
 */
UDO_API
int
udo_shm_get_fd (struct udo_shm *shm);


/*
 * @brief Returns starting address of a processes segment
 *        in the mmap(2) map'd POSIX shared memory buffer
 *        created after call to udo_shm_create().
 *
 * @param shm        - Pointer to a valid struct udo_shm.
 * @param proc_index - Process index to acquire it's shared
 *                     memory segment starting address.
 *
 * @returns
 *	on success: Pointer to processes SHM segment
 *	on failure: NULL
 */
UDO_API
void *
udo_shm_get_data (struct udo_shm *shm,
                  const unsigned int proc_index);


/*
 * @brief Returns size of a given process POSIX shared
 *        memory segment size created after call to
 *        udo_shm_create().
 *
 * @param shm        - Pointer to a valid struct udo_shm.
 * @param proc_index - Process index to acquire it's shared
 *                     memory segment size.
 *
 * @returns
 *	on success: Size of processes SHM segment
 *	on failure: Maximum size or -1
 */
UDO_API
size_t
udo_shm_get_data_size (struct udo_shm *shm,
                       const unsigned int proc_index);


/*
 * @brief Frees any allocated memory and closes FD's (if open) create after
 *        udo_shm_create() call.
 *
 * @param shm - Pointer to a valid struct udo_shm.
 */
UDO_API
void
udo_shm_destroy (struct udo_shm *shm);


/*
 * @brief Returns size of the internal structure. So,
 *        if caller decides to allocate memory outside
 *        of API interface they know the exact amount
 *        of bytes.
 *
 * @returns
 *	on success: sizeof(struct udo_shm)
 *	on failure: sizeof(struct udo_shm)
 */
UDO_API
int
udo_shm_get_sizeof (void);

#endif /* UDO_SHM_H */
