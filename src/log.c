#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <stdint.h>
#include <stdarg.h>
#include <sys/file.h>
#define HAVE_POSIX_TIMER
#include <time.h>
#ifdef CLOCK_MONOTONIC
# define CLOCKID CLOCK_MONOTONIC
#else
# define CLOCKID CLOCK_REALTIME
#endif

#include "log.h"

static int writefd = STDOUT_FILENO;
static enum udo_log_level_type log_level = UDO_LOG_NONE;

/* ANSI Escape Codes, terminal colors */
static const char *tcolors[] =
{
	[UDO_LOG_NONE]    = "",
	[UDO_LOG_SUCCESS] = "\e[32;1m",
	[UDO_LOG_ERROR]   = "\e[31;1m",
	[UDO_LOG_INFO]    = "\e[35;1m",
	[UDO_LOG_WARNING] = "\e[33;1m",
	[UDO_LOG_RESET]   = "\x1b[0m"
};


UDO_STATIC_INLINE
const char *
p_get_error (const unsigned int code)
{
	switch (code) {
		case UDO_LOG_ERR_INCORRECT_DATA:
			return "Incorrect data passed";
		default:
			return NULL;
	}

	return NULL;
}


void
udo_log_set_level (enum udo_log_level_type level)
{
	log_level = level;
}


void
udo_log_set_write_fd (const int fd)
{
	writefd = fd;
}


void
udo_log_remove_colors (void)
{
	tcolors[UDO_LOG_SUCCESS] = "[SUCCESS] ";
	tcolors[UDO_LOG_ERROR]   = "[ERROR] ";
	tcolors[UDO_LOG_INFO]    = "[INFO] ";
	tcolors[UDO_LOG_WARNING] = "[WARN] ";
}


void
udo_log_reset_colors (void)
{
	tcolors[UDO_LOG_SUCCESS] = "\e[32;1m";
	tcolors[UDO_LOG_ERROR]   = "\e[31;1m";
	tcolors[UDO_LOG_INFO]    = "\e[35;1m";
	tcolors[UDO_LOG_WARNING] = "\e[33;1m";
}


const char *
udo_log_get_error (const void *context)
{
	if (!context)
		return NULL;

	return ((struct udo_log_error_struct*)context)->buffer;
}


unsigned int
udo_log_get_error_code (const void *context)
{
	if (!context)
		return UINT32_MAX;

	return ((struct udo_log_error_struct*)context)->code;
}


void
udo_log_set_error_struct (void *context,
                          const unsigned int code,
                          const char *fmt,
                          ...)
{
	va_list args;

	int offset = 0;

	const char *string = NULL;

	struct udo_log_error_struct *error = context;

	if (!error)
		return;

	error->code = code;

	va_start(args, fmt);
	vsnprintf(error->buffer, sizeof(error->buffer), fmt, args);
	va_end(args);

	string = p_get_error(error->code);
	if (string) {
		offset = strnlen(error->buffer, sizeof(error->buffer));
		strncpy(error->buffer+offset, string, sizeof(error->buffer)-offset);
	}
}


void
udo_log_time (enum udo_log_level_type type,
              const char *fmt,
              ...)
{
	va_list args;
	time_t rawtime;

	char buffer[26];

	if (!(type & log_level))
		return;

	/* create message time stamp */
	rawtime = time(NULL);

	/* generate time */
	strftime(buffer, sizeof(buffer), "%F %T ", \
		localtime_r(&rawtime, &(struct tm){}));
	flock(writefd, LOCK_EX);
	dprintf(writefd, "%s", buffer);

	va_start(args, fmt);
	vdprintf(writefd, fmt, args);
	va_end(args);

	/* Reset terminal colors */
	dprintf(writefd, "%s", tcolors[UDO_LOG_RESET]);
	fsync(writefd);
	flock(writefd, LOCK_UN);
}


void
udo_log_notime (enum udo_log_level_type type,
                const char *fmt,
                ...)
{
	va_list args;

	if (!(type & log_level))
		return;

	/* Set terminal color */
	dprintf(writefd, "%s", tcolors[type]);

	va_start(args, fmt);
	vdprintf(writefd, fmt, args);
	va_end(args);

	/* Reset terminal colors */
	dprintf(writefd, "%s", tcolors[UDO_LOG_RESET]);
	fsync(writefd);
}


const char *
udo_log_get_tcolor (enum udo_log_level_type type)
{
	return tcolors[type];
}
