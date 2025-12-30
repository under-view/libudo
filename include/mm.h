#ifndef UDO_MM_H
#define UDO_MM_H

/*
 * This interface was built to force caller to
 * be more consciously concern about virtual
 * heap memory management.
 */

#include "macros.h"

/*
 * Stores information about the cando_mm instance.
 */
struct cando_mm;


/*
 * @brief Returns pointer to an allocated heap memory.
 *        The goal of this is to allocate a large block
 *        of memory once. If re-allocation required pass
 *        the previous large block to clone all data.
 *
 *        Addresses returned from function should not
 *        be used to write to. Writable addresses
 *        are return from a call to cando_mm_sub_alloc(3).
 *
 * @param mm   - If NULL the inital allocation will be performed.
 *               If not NULL must pass a pointer to a struct cando_mm.
 * @param size - Size of data caller may allocate. If the
 *               size is greater than the larger block
 *               remapping of memory will occur.
 *
 * @return
 *	on success: Pointer to struct cando_mm
 *	on failure: NULL
 */
struct cando_mm *
cando_mm_alloc (struct cando_mm *mm, const size_t size);


/*
 * @brief Returns pointer to an allocated heap memory
 *        segment. From an allocated large block of
 *        memory sub-allocate from that larger block.
 *
 *        Addresses returned from function can be
 *        used for writing.
 *
 * @param mm   - Must pass a pointer to a struct cando_mm.
 * @param size - Size of buffer to sub-allocate.
 *
 * @return
 * 	on success: Pointer to writable memory
 *	on failure: NULL
 */
void *
cando_mm_sub_alloc (struct cando_mm *mm, const size_t size);


/*
 * @brief Wipes the bytes at a given subregion of memory.
 *        Shifts the memory after the subregion up to a
 *        tracked buffer offset over to the subregion
 *        up to the new buffer offset.
 *
 *        NOTE: This function should be used sparingly
 *        as the caller would have to keep track of the
 *        new pointer address for every object allocated.
 *        It's better to only allocate memory if you know
 *        the address it resides in won't change. Usages
 *        of bounded buffer for strings is encouraged.
 *
 * @param mm   - Must pass a pointer to a struct cando_mm.
 * @param data - Address to the data caller wants to zero out.
 * @param size - Size of data to zero out
 */
void
cando_mm_free (struct cando_mm *mm,
               void *data,
               const size_t size);


/*
 * @brief Free's the large block of allocated memory created after
 *        cando_mm_alloc(3) call.
 *
 * @param mm - Must pass a pointer to a struct cando_mm.
 */
void
cando_mm_destroy (struct cando_mm *mm);

#endif /* UDO_MM_H */
