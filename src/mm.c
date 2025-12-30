#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include "log.h"
#include "macros.h"
#include "mm.h"

/*
 * @brief Structure defining udo_mm (UDO Memory Mapped) instance.
 *
 * @member err     - Stores information about the error that occured
 *                   for the given instance and may later be retrieved.
 * @member buff_sz - Full size of the struct udo_mm instance.
 *                   Not all bytes in the buffer are writable.
 * @member data_sz - Full size of the caller writable data.
 * @member ab_sz   - The amount of available bytes the caller
 *                   can still write to.
 * @member offset  - Buffer offset used when allocating new blocks
 *                   in constant time.
 */
struct udo_mm
{
	struct udo_log_error_struct err;
	size_t                      buff_sz;
	size_t                      data_sz;
	size_t                      ab_sz;
	size_t                      offset;
};


static void *
priv_new_virtual_memory_mapping (struct udo_mm *mm, const size_t size)
{
	void *data = NULL;

	size_t offset = 0, new_data_sz = 0;

	offset = sizeof(struct udo_mm);

	new_data_sz = (mm) ? mm->buff_sz + size : offset + size;

	data = mmap(NULL, new_data_sz,
		    PROT_READ|PROT_WRITE,
		    MAP_PRIVATE|MAP_ANONYMOUS,
		    -1, 0);
	if (data == (void*)-1) {
		udo_log_error("mmap: %s\n", strerror(errno));
		return NULL;
	} else {
		memset(data, 0, new_data_sz);

		/*
		 * This is okay because the goal would be
		 * to allocate as much memory as possible
		 * early on. So, that remapping can be
		 * avoided.
		 */
		if (mm) {
			memcpy(data, mm, mm->buff_sz);
			munmap(mm, mm->buff_sz);
		}

		mm = data;
		mm->offset = offset;
		mm->buff_sz = new_data_sz;
		mm->data_sz = mm->ab_sz = new_data_sz - offset;
	}

	return mm;
}


struct udo_mm *
udo_mm_alloc (struct udo_mm *mm, const size_t size)
{
	struct udo_mm *ret = mm;

	if (!mm) {
		ret = priv_new_virtual_memory_mapping(mm, size);
	} else if (mm && (mm->data_sz <= size)) {
		ret = priv_new_virtual_memory_mapping(mm, size);
	}

	return ret;
}


void *
udo_mm_sub_alloc (struct udo_mm *mm, const size_t size)
{
	void *data = NULL;

	if (!mm) {
		udo_log_error("Incorrect data passed\n");
		return NULL;
	}

	if (mm->ab_sz <= size) {
		udo_log_set_error(mm, UDO_LOG_ERR_UNCOMMON,
		                  "Cannot allocate %lu bytes only %lu bytes left.",
		                  size, mm->ab_sz);
		return NULL;
	}

	data = (void*)((char*)mm + mm->offset);

	mm->ab_sz -= size;
	mm->offset += size;

	return data;
}


void
udo_mm_free (struct udo_mm *mm,
             void *data,
             const size_t size)
{
	size_t copy_sz = 0;

	void *mv_data = NULL;

	if (!mm || !data || !size) {
		udo_log_error("Incorrect data passed\n");
		return;
	}

	memset(data, 0, size);

	mv_data = (void*)((char*)data+size);
	copy_sz = ((uintptr_t)(((char*)mm)+mm->offset))-((uintptr_t)data);

	memcpy(data, mv_data, copy_sz);

	mm->ab_sz += size;
	mm->offset -= size;
}


void
udo_mm_destroy (struct udo_mm *mm)
{
	if (!mm)
		return;

	munmap(mm, mm->buff_sz);
}
