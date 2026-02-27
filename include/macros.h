#ifndef UDO_MACROS_H
#define UDO_MACROS_H

#include <stdint.h>   /* For uintptr_t */

/*
 * Used to prevent C++ name mangling when
 * using library in a C++ software package.
 */
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
 * Macros used to define size of buffer(s)
 * that store paths to file's or just
 * their file name.
 */
#define UDO_DIR_NAME_MAX  (1<<12)
#define UDO_FILE_NAME_MAX (1<<8)
#define UDO_FILE_PATH_MAX (UDO_DIR_NAME_MAX+UDO_FILE_NAME_MAX)


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
 * @brief Converts string to an unsigned integer.
 *        Simple one way hash function.
 *
 * @param str - Pointer to string to hash.
 *
 * @return
 * 	on success: unsigned integer representing string
 * 	on failure: unsigned integer representing string
 */
#define UDO_STRTOU(str) \
	__extension__ \
	({ \
		unsigned int hash=0; \
		const char *s = str; \
		while (*s) hash += *s++; \
		hash; \
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
