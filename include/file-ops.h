#ifndef UDO_FILE_OPS_H
#define UDO_FILE_OPS_H

#include <sys/types.h>

#include "macros.h"

/*
 * Stores information about the udo_file_ops instance.
 */
struct udo_file_ops;


/*
 * @brief UDO File Operations Create Info Structure
 *
 * @member fname       - Full path to file caller wants to open(2)|creat(2).
 * @member size        - Size in bytes caller newly created file will be.
 *                       If @create_pipe is true this member is ignored.
 * @member offset      - Offset within the file to mmap(2).
 *                       If @create_pipe is true this member is ignored.
 * @member create_pipe - Boolean to enable/disable creation of a pipe(2).
 */
struct udo_file_ops_create_info
{
	const char        *fname;
	unsigned long int size;
	off_t             offset;
	unsigned char     create_pipe : 1;
};


/*
 * @brief Creates or opens caller define file.
 *
 * @param flops     - May be NULL or a pointer to a struct udo_file_ops.
 *                    If NULL memory will be allocated and return to
 *                    caller. If not NULL address passed will be used
 *                    to store the newly created struct udo_file_ops
 *                    instance.
 * @param file_info - Pointer to a struct udo_file_ops_create_info.
 *                    The use of pointer to a void is to limit amount
 *                    of columns required to define a function.
 *
 * @returns
 * 	on success: Pointer to a struct udo_file_ops
 * 	on failure: NULL
 */
UDO_API
struct udo_file_ops *
udo_file_ops_create (struct udo_file_ops *flops,
                     const void *file_info);


/*
 * @brief Adjust file to a size of precisely length bytes.
 *
 * @param flops - Pointer to a valid struct udo_file_ops.
 * @param size  - Size in bytes file will be truncate(2)'d to.
 *
 * @returns
 * 	on success: 0
 * 	on failure: # < 0
 */
UDO_API
int
udo_file_ops_truncate (struct udo_file_ops *flops,
                       const off_t size);


/*
 * @brief UDO File Operations Zero Copy Info
 *
 * @size    - Total size of the data to copy.
 * @in_fd   - Input file descriptor to copy data from.
 * @in_off  - Byte offset in the @in_fd open file to copy from.
 *            NOTE: splice(2) may updates the variable.
 * @out_fd  - Output file descriptor to copy data to.
 * @out_off - Byte offset in the @out_fd open file to copy X amount
 *            of data from the given offset.
 *            NOTE: splice(2) may updates the variable.
 */
struct udo_file_ops_zero_copy_info
{
	size_t size;
	int    in_fd;
	off_t  *in_off;
	int    out_fd;
	off_t  *out_off;
};


/*
 * @brief Sets data in a file at a given offset up to a given size
 *        without copying the buffer into userspace.
 *
 * @param flops     - Pointer to a valid struct udo_file_ops.
 * @param file_info - Pointer to a struct udo_file_ops_zero_copy_info.
 *                    The use of pointer to a void is to limit amount
 *                    of columns required to define a function.
 *
 * @returns
 * 	on success: Amount of bytes splice(2) to/from a pipe(2)
 * 	on failure: -1
 */
UDO_API
ssize_t
udo_file_ops_zero_copy (struct udo_file_ops *flops,
                        const void *file_info);


/*
 * @brief Returns file data stored at a given offset.
 *        Caller would have to copy into a secondary
 *        buffer to exclude new line character like bellow.
 *
 * @code@
 * size_t len;
 * char buffer[32];
 * const void *data = NULL;
 *
 * memset(buffer, 0, sizeof(buffer));
 * data = udo_file_ops_get_data(flops, 54);
 * memccpy(buffer, data, '\n', sizeof(buffer));
 * len = strnlen(buffer, sizeof(buffer));
 * buffer[len-1] = '\0';
 * fprintf(stdout, "%s", buffer);
 *
 * // OR
 * data = udo_file_ops_get_data(flops, 54);
 * fprintf(stdout, "%.*s\n", 32, data);
 * @endcode@
 *
 * @param flops  - Pointer to a valid struct udo_file_ops.
 * @param offset - Byte offset within the file.
 *
 * @returns
 * 	on success: Pointer to file data at a given offset
 * 	on failure: NULL
 */
UDO_API
const void *
udo_file_ops_get_data (struct udo_file_ops *flops,
                       const size_t offset);


/*
 * @brief Returns file data stored at a given line.
 *        Caller would have to copy into a secondary
 *        buffer to exclude new line character like bellow.
 *
 * @code@
 * size_t len;
 * char buffer[32];
 * const char *line = NULL;
 *
 * memset(buffer, 0, sizeof(buffer));
 * line = udo_file_ops_get_line(flops, 4);
 * memccpy(buffer, line, '\n', sizeof(buffer));
 * len = strnlen(buffer, sizeof(buffer));
 * buffer[len-1] = '\0';
 * fprintf(stdout, "%s", buffer);
 *
 * // OR
 * line = udo_file_ops_get_line(flops, 4);
 * fprintf(stdout, "%.*s\n", 32, line);
 * @endcode@
 *
 * @param flops - Pointer to a valid struct udo_file_ops.
 * @param line  - Line number in file to get data from.
 *
 * @returns
 * 	on success: Pointer to file data at a given line
 * 	on failure: NULL
 */
UDO_API
const char *
udo_file_ops_get_line (struct udo_file_ops *flops,
                       const size_t line);


/*
 * @brief Returns the amount of lines a file contains.
 *
 * @param flops - Pointer to a valid struct udo_file_ops.
 *
 * @returns
 * 	on success: Line count
 * 	on failure: size_t max size or -1
 */
UDO_API
size_t
udo_file_ops_get_line_count (struct udo_file_ops *flops);


/*
 * @brief Returns file descriptor to open file.
 *
 * @param flops - Pointer to a valid struct udo_file_ops.
 *
 * @returns
 * 	on success: File descriptor to open file
 * 	on failure: -1
 */
UDO_API
int
udo_file_ops_get_fd (struct udo_file_ops *flops);


/*
 * @brief Returns size of the mmap(2)'d buffer associated
 *        with the open file.
 *
 * @param flops - Pointer to a valid struct udo_file_ops.
 *
 * @returns
 * 	on success: Size of the mmap(2) buffer
 * 	on failure: size_t max size or -1
 */
UDO_API
size_t
udo_file_ops_get_alloc_size (struct udo_file_ops *flops);


/*
 * @brief Returns size of the data within the open file.
 *
 * @param flops - Pointer to a valid struct udo_file_ops.
 *
 * @returns
 * 	on success: Size of data in file
 * 	on failure: size_t max size or -1
 */
UDO_API
size_t
udo_file_ops_get_data_size (struct udo_file_ops *flops);


/*
 * @brief Return file name of open file associated with
 *        the struct udo_file_ops context.
 *
 * @param flops - Pointer to a valid struct udo_file_ops.
 *
 * @returns
 * 	on success: File name of open file
 * 	on failure: NULL
 */
UDO_API
const char *
udo_file_ops_get_filename (struct udo_file_ops *flops);


/*
 * @brief Return directory path of open file associated
 *        with the struct udo_file_ops context.
 *
 * @param flops - Pointer to a valid struct udo_file_ops.
 *
 * @returns
 * 	on success: Directory path a file resides in
 * 	on failure: NULL
 */
UDO_API
const char *
udo_file_ops_get_dirname (struct udo_file_ops *flops);


/*
 * @brief UDO File Operations Set Data Info
 *
 * @member offset - Byte offset within the file.
 * @member size   - Size in bytes to copy into file at @offset.
 * @member data   - Data to copy at the given file offset.
 */
struct udo_file_ops_set_data_info
{
	size_t     offset;
	size_t     size;
	const void *data;
};


/*
 * @brief Sets data in a file at a given offset up to a given size.
 *
 * @param flops     - Pointer to a valid struct udo_file_ops.
 * @param file_info - Pointer to a struct udo_file_ops_set_data_info.
 *                    The use of pointer to a void is to limit amount
 *                    of columns required to define a function.
 *                 
 * @returns
 * 	on success: 0
 * 	on failure: -1
 */
UDO_API
int
udo_file_ops_set_data (struct udo_file_ops *flops,
                       const void *file_info);


/*
 * @brief Frees any allocated memory and closes FD's (if open) created after
 *        udo_file_ops_create() call.
 *
 * @param flops - Pointer to a valid struct udo_file_ops.
 */
UDO_API
void
udo_file_ops_destroy (struct udo_file_ops *flops);


/*
 * @brief Returns size of the internal structure. So,
 *        if caller decides to allocate memory outside
 *        of API interface they know the exact amount
 *        of bytes.
 *
 * @return
 *	on success: sizeof(struct udo_file_ops)
 *	on failure: sizeof(struct udo_file_ops)
 */
UDO_API
int
udo_file_ops_get_sizeof (void);


/*
 * @brief Updates the file descriptor to new status flags.
 *
 * @param fd    - File descriptor who's flags to update.
 * @param flags - Set the file status flags to the value
 *                specified by parameter. For more information
 *                on status flags see fcntl(2) and open(2).
 *
 * @return
 *	on success: 0
 *	on failure: -1
 */
UDO_API
int
udo_file_ops_set_fd_flags (const int fd, const int flags);

#endif /* UDO_FILE_OPS_H */
