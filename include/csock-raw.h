#ifndef UDO_CSOCK_RAW_H
#define UDO_CSOCK_RAW_H

#include "macros.h"

#include <linux/can.h>

/*
 * Stores information about the udo_csock_raw instace.
 */
struct udo_csock_raw;


/*
 * @brief Structure passed to udo_csock_raw_create(3)
 *        used to define how to create the CAN socket.
 *
 * @member iface - Must pass textual CAN interface
 *                 name in string format.
 */
struct udo_csock_raw_create_info
{
	const char *iface;
};


/*
 * @brief Creates a socket that may be utilized
 *        for sending & receiving CAN frames. On
 *        a caller specified CAN interface.
 *
 * @param csock      - May be NULL or a pointer to a struct udo_csock_raw.
 *                     If NULL memory will be allocated and return to
 *                     caller. If not NULL address passed will be used
 *                     to store the newly created struct udo_csock_raw
 *                     instance.
 * @param csock_info - Implementation uses a pointer to a
 *                     struct udo_csock_raw_create_info
 *                     no other implementation may be passed to
 *                     this parameter.
 *
 * @returns
 *	on success: Pointer to a struct udo_csock_raw
 *	on failure: NULL
 */
UDO_API
struct udo_csock_raw *
udo_csock_raw_create (struct udo_csock_raw *csock,
                      const void *csock_info);


/*
 * @brief Send CAN frame through socket bounded to CAN interface
 *        specified by a call to udo_csock_raw_create(3).
 *
 * @param csock      - Must pass a pointer to a struct udo_csock_raw.
 * @param frame      - Pointer to frame buffer to send through socket.
 * @param csock_info - Reserved for future usage. For now used
 *                     to set the flag argument of send(2).
 *
 * @returns
 *	on success: Size of CAN frame
 *	on failure: # < 0
 */
UDO_API
ssize_t
udo_csock_raw_send_data (struct udo_csock_raw *csock,
                         const struct can_frame *frame,
                         const void *csock_info);


/*
 * @brief Receive CAN frame from socket bounded to CAN interface
 *        specified by a call to udo_csock_raw_create(3).
 *
 * @param csock      - Must pass a pointer to a struct udo_csock_raw.
 * @param frame      - Pointer to buffer to store data received from a socket.
 * @param csock_info - Reserved for future usage. For now used
 *                     to set the flag argument of recv(2).
 *
 * @returns
 *	on success: Size of CAN frame
 *	on failure: # < 0
 */
UDO_API
ssize_t
udo_csock_raw_recv_data (struct udo_csock_raw *csock,
                         struct can_frame *frame,
                         const void *csock_info);


/*
 * @brief Acquire socket file descriptor associated with
 *        struct udo_csock_raw instance.
 *
 * @param csock - Must pass a pointer to a struct udo_csock_raw.
 *
 * @returns
 *	on success: RAW socket file descriptor
 *	on failure: -1
 */
UDO_API
int
udo_csock_raw_get_fd (struct udo_csock_raw *csock);


/*
 * @brief Acquire textual CAN interface name in string format
 *        associated with struct udo_csock_raw instance.
 *
 * @param csock - Must pass a pointer to a struct udo_csock_raw.
 *
 * @returns
 *	on success: Textual CAN interface name in string format
 *	on failure: NULL
 */
UDO_API
const char *
udo_csock_raw_get_iface (struct udo_csock_raw *csock);


/*
 * @brief Frees any allocated memory and closes FD's (if open)
 *        created after udo_csock_raw_create() call.
 *
 * @param csock - Pointer to a valid struct udo_csock_raw.
 */
UDO_API
void
udo_csock_raw_destroy (struct udo_csock_raw *csock);


/*
 * @brief Returns size of the internal structure. So,
 *        if caller decides to allocate memory outside
 *        of API interface they know the exact amount
 *        of bytes.
 *
 * @returns
 *	on success: sizeof(struct udo_csock_raw)
 *	on failure: sizeof(struct udo_csock_raw)
 */
UDO_API
int
udo_csock_raw_get_sizeof (void);

#endif /* UDO_CSOCK_RAW_H */
