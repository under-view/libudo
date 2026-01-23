#ifndef UDO_MACROS_H
#define UDO_MACROS_H

#include <stdint.h>   /* For uintptr_t */
#include <sys/mman.h> /* For mprotect(2) */

/* Prevent c++ name mangling */
#ifdef __cplusplus
#define UDO_API extern "C"
#else
#define UDO_API
#endif

/*
 * Informs the compiler that you expect a variable
 * to be unused and instructs compiler to not issue
 * a warning on the variable.
 */
#define UDO_UNUSED __attribute__((unused))


/*
 * "always_inline" instructs GCC to
 * 1. Ignore flag -fno-inline
 * 2. Don't produce external definition of a
 *    function with external linkage.
 * 3. Ignore inlining limits. Use alloca to inline.
 */
#define UDO_INLINE inline __attribute__((always_inline))
#define UDO_STATIC_INLINE static inline __attribute__((always_inline))


/*
 * Min & Max macro definitions with type safety.
 */
#define UDO_MAX(a,b) \
	({ typeof (a) _a = (a); \
	   typeof (b) _b = (b); \
	   _a > _b ? _a : _b; })


#define UDO_MIN(a,b) \
	({ typeof (a) _a = (a); \
	   typeof (b) _b = (b); \
	   _a < _b ? _a : _b; })


/*
 * Memory alignment values must be a power of 2 number.
 */
#define UDO_BYTE_ALIGN(bytes, power_two_align) \
	(bytes+(power_two_align-1))&~(power_two_align-1)


/*
 * Define typical page size without including
 * limits.h header.
 */
#define UDO_PAGE_SIZE (1<<12)


/*
 * Retrieves the starting address of the page @ptr resides in.
 */
#define UDO_PAGE_GET(ptr) \
	((void*)((uintptr_t)ptr & ~(UDO_PAGE_SIZE-1)))


/*
 * @brief Sets a grouping a pages write-only
 *
 * @param ptr  - Pointer to buffer caller wants write-only
 * @param size - Size of data that needs to be set write-only
 */
#define UDO_PAGE_SET_WRITE(ptr, size) \
	__extension__ \
	({ \
		int err = -1; \
		void *page = UDO_PAGE_GET(ptr); \
		err = mprotect(page, size, PROT_WRITE); \
		err; \
	})


/*
 * @brief Sets a grouping a pages read-only
 *
 * @param ptr  - Pointer to buffer caller wants write-only
 * @param size - Size of data that needs to be set write-only
 */
#define UDO_PAGE_SET_READ(ptr, size) \
	__extension__ \
	({ \
		int err = -1; \
		void *page = UDO_PAGE_GET(ptr); \
		err = mprotect(page, size, PROT_READ); \
		err; \
	})


/*
 * @brief Create atomic variable type of a
 *        caller defined data type.
 *
 * @param name - Name of atomic type.
 * @param type - Data type of the atomic type.
 */
#define UDO_ATOMIC_DEF(name, type) \
	typedef _Atomic __typeof__(type) name;

UDO_ATOMIC_DEF(udo_atomic_int, int);
UDO_ATOMIC_DEF(udo_atomic_bool, unsigned char);
UDO_ATOMIC_DEF(udo_atomic_u32, unsigned int);
UDO_ATOMIC_DEF(udo_atomic_addr, unsigned char *);

#endif /* UDO_MACROS_H */
