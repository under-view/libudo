#ifndef CANDO_LOG_H
#define CANDO_LOG_H

#include "macros.h"


/*
 * @brief enum cando_log_level_type (Handy Log Level Type)
 *
 *        Sets which messages of a given type to print and is used to
 *        help determine which ANSI Escape Codes to utilize.
 *
 * @macro CANDO_LOG_NONE    - Term color
 * @macro CANDO_LOG_SUCCESS - Green
 * @macro CANDO_LOG_ERROR   - Red
 * @macro CANDO_LOG_INFO    - Light purple
 * @macro CANDO_LOG_WARNING - Yellow
 * @macro CANDO_LOG_RESET   - Term color
 * @macro CANDO_LOG_ALL     - All above colors
 */
enum cando_log_level_type
{
	CANDO_LOG_NONE    = 0x00000000,
	CANDO_LOG_SUCCESS = 0x00000001,
	CANDO_LOG_ERROR   = 0x00000002,
	CANDO_LOG_INFO    = 0x00000004,
	CANDO_LOG_WARNING = 0x00000008,
	CANDO_LOG_RESET   = 0x00000010,
	CANDO_LOG_ALL     = 0xFFFFFFFF
};


/*
 * @brief Sets which type of messages that are allowed
 *        to be printed to an open file.
 *
 * @param level - 32-bit integer representing the type of log to print to
 *                an open file. Each log type has a different color.
 */
CANDO_API
void
cando_log_set_level (enum cando_log_level_type level);


/*
 * @brief Sets the internal global write file descriptor
 *        to caller define file descriptor.
 *
 *        Default is set to STDOUT_FILENO.
 *
 * @param fd - File descriptor to an open file
 */
CANDO_API
void
cando_log_set_write_fd (const int fd);


/*
 * @brief Sets the internal global ansi color
 *        storage array to remove the ansi colors
 *        and replace with "[LOG_TYPE] ".
 */
CANDO_API
void
cando_log_remove_colors (void);


/*
 * @brief Sets the internal global ansi color
 *        storage array to it's original values.
 */
CANDO_API
void
cando_log_reset_colors (void);


/*
 * @brief enum cando_log_error_type
 *
 *        Enum with macros defining and error type
 *        Add on userspace error codes should be well out
 *        of range of any known common error code.
 *
 * @macro CANDO_LOG_ERR_UNCOMMON       - Errors that can't be given a common
 *                                       error string are given this error code.
 *                                       Caller would then need to set buffer themselves.
 * @macro CANDO_LOG_ERR_INCORRECT_DATA - Code for incorrect data passed in function arguments
 */
enum cando_log_error_type
{
	CANDO_LOG_ERR_UNCOMMON       = 0x1000,
	CANDO_LOG_ERR_INCORRECT_DATA = 0x1001,
};


/*
 * @brief Structure used to store and acquire
 *        error string and code for multiple
 *        struct context's.
 *
 * @member code   - Error code or errno
 * @member buffer - Buffer to store error string
 */
struct cando_log_error_struct
{
	unsigned int code;
	char         buffer[(1<<9)];
};


/*
 * @brief Returns a string with the error defined given
 *        caller provided a context with first members
 *        of the context being a struct cando_log_error_struct.
 *
 * @param context - Pointer to an arbitrary context.
 *                  Start of context must be a struct cando_log_error_struct.
 *
 * @returns
 * 	on success: Passed context error string
 * 	on failure: NULL
 */
CANDO_API
const char *
cando_log_get_error (const void *context);


/*
 * @brief Returns unsigned integer with the error code
 *        given caller provided a context with first members
 *        of the context being a struct cando_log_error_struct.
 *
 * @param context - Pointer to an arbitrary context.
 *                  Start of context must be a struct cando_log_error_struct.
 *
 * @returns
 * 	on success: Passed context error code or errno
 * 	on failure: UINT32_MAX
 */
CANDO_API
unsigned int
cando_log_get_error_code (const void *context);


/*
 * @brief Sets struct cando_log_error_struct members value
 *
 * @param context - Pointer to an arbitrary context.
 *                  Start of context must be a struct cando_log_error_struct.
 * @param code    - Error code to set for a @context.
 *                  May be errno or enum cando_log_error_type
 * @param fmt     - Format of the log passed to va_args.
 * @param ...     - Variable list arguments
 */
CANDO_API
void
cando_log_set_error_struct (void *context,
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
CANDO_API
void
cando_log_time (enum cando_log_level_type type,
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
CANDO_API
void
cando_log_notime (enum cando_log_level_type type,
		  const char *fmt,
		  ...);


/*
 * Should only be used by bellow macros
 */
const char *
cando_log_get_tcolor (enum cando_log_level_type type);

/*
 * Macros defined to structure the message
 * timestamp - [file:line] message
 */
#define cando_log(log_type, fmt, ...) \
	cando_log_time(log_type, "[%s:%d] %s" fmt, \
	               __FILE_NAME__,  __LINE__, \
	               cando_log_get_tcolor(log_type), \
	               ##__VA_ARGS__)

#define cando_log_success(fmt, ...) \
	cando_log_time(CANDO_LOG_SUCCESS, "[%s:%d] %s" fmt, \
	               __FILE_NAME__, __LINE__, \
	               cando_log_get_tcolor(CANDO_LOG_SUCCESS), \
	               ##__VA_ARGS__)

#define cando_log_info(fmt, ...) \
	cando_log_time(CANDO_LOG_INFO, "[%s:%d] %s" fmt, \
	               __FILE_NAME__, __LINE__, \
	               cando_log_get_tcolor(CANDO_LOG_INFO), \
	               ##__VA_ARGS__)

#define cando_log_warning(fmt, ...) \
	cando_log_time(CANDO_LOG_WARNING, "[%s:%d] %s" fmt, \
	               __FILE_NAME__, __LINE__, \
	               cando_log_get_tcolor(CANDO_LOG_WARNING), \
	               ##__VA_ARGS__)

#define cando_log_error(fmt, ...) \
	cando_log_time(CANDO_LOG_ERROR, "[%s:%d] %s" fmt, \
	               __FILE_NAME__, __LINE__, \
	               cando_log_get_tcolor(CANDO_LOG_ERROR), \
	               ##__VA_ARGS__)

#define cando_log_print(log_type, fmt, ...) \
	cando_log_notime(log_type, fmt, ##__VA_ARGS__)

#define cando_log_set_error(ptr, code, fmt, ...) \
	cando_log_set_error_struct(ptr, code, "[%s:%d] " fmt, \
	                           __FILE_NAME__, __LINE__, ##__VA_ARGS__)

#endif /* CANDO_LOG_H */
