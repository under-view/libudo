#ifndef UDO_LOG_H
#define UDO_LOG_H

#include "macros.h"

/*
 * @brief enum udo_log_level_type (Handy Log Level Type)
 *
 *        Sets which messages of a given type to print and is used to
 *        help determine which ANSI Escape Codes to utilize.
 *
 * @macro UDO_LOG_NONE    - Term color
 * @macro UDO_LOG_SUCCESS - Green
 * @macro UDO_LOG_ERROR   - Red
 * @macro UDO_LOG_INFO    - Light purple
 * @macro UDO_LOG_WARNING - Yellow
 * @macro UDO_LOG_RESET   - Term color
 * @macro UDO_LOG_ALL     - All above colors
 */
enum udo_log_level_type
{
	UDO_LOG_NONE    = 0x00000000,
	UDO_LOG_SUCCESS = 0x00000001,
	UDO_LOG_ERROR   = 0x00000002,
	UDO_LOG_INFO    = 0x00000004,
	UDO_LOG_WARNING = 0x00000008,
	UDO_LOG_RESET   = 0x00000010,
	UDO_LOG_ALL     = 0xFFFFFFFF
};


/*
 * @brief Sets which type of messages that are allowed
 *        to be printed to an open file.
 *
 * @param level - 32-bit integer representing the type of log to print to
 *                an open file. Each log type has a different color.
 */
UDO_API
void
udo_log_set_level (enum udo_log_level_type level);


/*
 * @brief Sets the internal global write file descriptor
 *        to caller define file descriptor.
 *
 *        Default is set to STDOUT_FILENO.
 *
 * @param fd - File descriptor to an open file
 */
UDO_API
void
udo_log_set_write_fd (const int fd);


/*
 * @brief Sets the internal global ansi color
 *        storage array to remove the ansi colors
 *        and replace with "[LOG_TYPE] ".
 */
UDO_API
void
udo_log_remove_colors (void);


/*
 * @brief Sets the internal global ansi color
 *        storage array to it's original values.
 */
UDO_API
void
udo_log_reset_colors (void);


/*
 * @brief enum udo_log_error_type
 *
 *        Enum with enumerators defining and error type
 *        Add on userspace error codes should be well out
 *        of range of any known common error code.
 *
 * @macro UDO_LOG_ERR_UNCOMMON       - Errors that can't be given a common
 *                                     error string are given this error code.
 *                                     Caller would then need to set buffer themselves.
 * @macro UDO_LOG_ERR_INCORRECT_DATA - Code for incorrect data passed in function arguments
 */
enum udo_log_error_type
{
	UDO_LOG_ERR_UNCOMMON       = 0x1000,
	UDO_LOG_ERR_INCORRECT_DATA = 0x1001,
};


/*
 * @brief Structure used to store and acquire
 *        error string and code for multiple
 *        struct context's.
 *
 * @member code   - Error code or errno
 * @member buffer - Buffer to store error string
 */
struct udo_log_error_struct
{
	unsigned int code;
	char         buffer[(1<<9)];
};


/*
 * @brief Returns a string with the error defined given
 *        caller provided a context with first members
 *        of the context being a struct udo_log_error_struct.
 *
 * @param context - Pointer to an arbitrary context.
 *                  Start of context must be a struct udo_log_error_struct.
 *
 * @returns
 * 	on success: Passed context error string
 * 	on failure: NULL
 */
UDO_API
const char *
udo_log_get_error (const void *context);


/*
 * @brief Returns unsigned integer with the error code
 *        given caller provided a context with first members
 *        of the context being a struct udo_log_error_struct.
 *
 * @param context - Pointer to an arbitrary context.
 *                  Start of context must be a struct udo_log_error_struct.
 *
 * @returns
 * 	on success: Passed context error code or errno
 * 	on failure: UINT32_MAX
 */
UDO_API
unsigned int
udo_log_get_error_code (const void *context);


/*
 * @brief Sets struct udo_log_error_struct members value
 *
 * @param context - Pointer to an arbitrary context.
 *                  Start of context must be a struct udo_log_error_struct.
 * @param code    - Error code to set for a @context.
 *                  May be errno or enum udo_log_error_type
 * @param fmt     - Format of the log passed to va_args.
 * @param ...     - Variable list arguments
 */
UDO_API
void
udo_log_set_error_struct (void *context,
                          const unsigned int code,
                          const char *fmt,
                          ...);


/*
 * @brief Provides applications/library way to write to an open file
 *        with a time stamp and ansi color codes to colorize
 *        different message.
 *
 * @param type - The type of color to use with log
 * @param fmt  - Format of the log passed to va_args
 * @param ...  - Variable list arguments
 */
UDO_API
void
udo_log_time (enum udo_log_level_type type,
              const char *fmt,
              ...);


/*
 * @brief Provides applications/library way to write to an open file
 *        without time stamp with ansi color codes to colorize
 *        different message.
 *
 * @param type - The type of color to use with log
 * @param fmt  - Format of the log passed to va_args
 * @param ...  - Variable list arguments
 */
UDO_API
void
udo_log_notime (enum udo_log_level_type type,
                const char *fmt,
                ...);


/*
 * Should only be used by bellow macros
 */
const char *
udo_log_get_tcolor (enum udo_log_level_type type);

/*
 * Macros defined to structure the message
 * timestamp - [file:line] message
 */
#define udo_log(log_type, fmt, ...) \
	udo_log_time(log_type, "[%s:%d] %s" fmt, \
	             __FILE_NAME__,  __LINE__, \
	             udo_log_get_tcolor(log_type), \
	             ##__VA_ARGS__)

#define udo_log_success(fmt, ...) \
	udo_log_time(UDO_LOG_SUCCESS, "[%s:%d] %s" fmt, \
	             __FILE_NAME__, __LINE__, \
	             udo_log_get_tcolor(UDO_LOG_SUCCESS), \
	             ##__VA_ARGS__)

#define udo_log_info(fmt, ...) \
	udo_log_time(UDO_LOG_INFO, "[%s:%d] %s" fmt, \
	             __FILE_NAME__, __LINE__, \
	             udo_log_get_tcolor(UDO_LOG_INFO), \
	             ##__VA_ARGS__)

#define udo_log_warning(fmt, ...) \
	udo_log_time(UDO_LOG_WARNING, "[%s:%d] %s" fmt, \
	             __FILE_NAME__, __LINE__, \
	             udo_log_get_tcolor(UDO_LOG_WARNING), \
	             ##__VA_ARGS__)

#define udo_log_error(fmt, ...) \
	udo_log_time(UDO_LOG_ERROR, "[%s:%d] %s" fmt, \
	             __FILE_NAME__, __LINE__, \
	             udo_log_get_tcolor(UDO_LOG_ERROR), \
	             ##__VA_ARGS__)

#define udo_log_print(log_type, fmt, ...) \
	udo_log_notime(log_type, fmt, ##__VA_ARGS__)

#define udo_log_set_error(ptr, code, fmt, ...) \
	udo_log_set_error_struct(ptr, code, "[%s:%d] " fmt, \
	                         __FILE_NAME__, __LINE__, ##__VA_ARGS__)

#endif /* UDO_LOG_H */
