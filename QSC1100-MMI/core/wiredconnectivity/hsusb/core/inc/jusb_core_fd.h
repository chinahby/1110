/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _JUSB_CORE_FD_H
#define _JUSB_CORE_FD_H

#include "jusb_common.h"
#include "jusb_chp9.h"

/*HSU addition*/
#include "hsu_vendor_specific.h"
/*End of HSU addition*/

#define MAX_PIPE_COUNT  16

typedef struct {
    /* Number of pipes in this alt iface */
    juint8_t      pipe_count;

    /* Array of pipes for this alt iface*/
    pipe_desc_t  *pipes;

    /* Pass extra, class specific, descriptor data in the Config descriptor */
    juint8_t     *extra_descriptor;
    juint16_t    extra_descriptor_size;

} alt_interface_desc_t;

typedef jresult_t (*if_control_msg_fn)(void *context, void *buffer);

typedef struct {
    /* Ep handler for this interface */
    if_control_msg_fn control_msg;

    /* Pointer to alt-interface array */
    alt_interface_desc_t *alt_ifs;
    juint8_t     alt_if_num;
    juint8_t     alt_if_curr;

    /* Pass extra, class specific, descriptor data in the Config descriptor */
    juint8_t     *extra_descriptor;
    juint32_t    extra_descriptor_size;

#define UWD_UNDEFINED_INTERFACE 0xFF
    /* Actual interface number (set by Core) */
    juint8_t     number;

    /* FD Class specific identification */
    juint8_t     if_class;
    juint8_t     if_subclass;
    juint8_t     if_protocol;

    /* Optional string descriptor index to describe this interface */
    juint8_t     if_string;
} interface_desc_t;

/* Configuration descriptior */
typedef struct {
    device_speed_t speed;
    device_speed_t current_speed;               /* Filled in by Core */
    juint8_t     interface_count;
    interface_desc_t *interfaces;
    struct usb_interface_assoc_descriptor *iad;
} fd_desc_t;

/* Functions exported by the Function Driver for Core
 *  get_pipe_config     - Returns the pipe configuration the FD requires
 *  enable              - Enable the Function Driver
 *  disable             - Disable the Function Driver
 *
 *  suspend             - Notify the FD on suspend
 *  resume              - Notify the FD on resume
 *
 *  uninit              - Un-initialize the Function Driver
 */
typedef struct {
    fd_desc_t *(*get_pipe_config)(context_t context);

    jresult_t (*enable)(context_t context);
    void     (*disable)(context_t context);

    jresult_t (*suspend)(context_t context);
    jresult_t (*resume)(context_t context);

    jresult_t (*uninit)(context_t context);
} fd_ops_t;

/* Function exported by Core for Function Driver
 *  core_send_data           - Post a data send request
 *  core_read_data           - Post a data read request
 *  
 *  core_stall_pipe          - Set the STALL state on a pipe
 *  core_abort_pipe          - Cancel all pending requests on a pipe
 *
 *  core_get_available_iface - Get the number of the next available interface
 *
 *  core_register_fd         - Register the Function Driver with Core
 *
 *  core_request_alloc       - Allocate a request object
 *  core_request_free        - Free a request object
 *
 *  core_add_msos_string_descriptor - Add a string descriptor to be included
 *                                    in the response to a GET_MS_DESCRIPTOR
 *                                    request
 *  core_add_string_descriptor      - Add a string descriptor to be included in
 *                                    the response to a GET_DESCRIPTOR request
 *  core_release_string_descriptor  - Remove a string descriptor
 *  core_change_string_descriptor   - Change a string descriptor
 *  
 *  core_handle_tx_completion          - Handle completion for IN transfers
 */
jresult_t core_send_data(void *ctx, pipe_desc_t *pipe , request_t *request);
jresult_t core_read_data(void *ctx, pipe_desc_t *pipe, request_t *request);

void core_stall_pipe(void *ctx, pipe_desc_t *pipe,
    juint8_t flag); /* 0/1 - Off/On */
void core_abort_pipe(void *ctx, pipe_desc_t *pipe);

juint8_t core_get_available_iface(void *ctx);

jresult_t core_register_fd(void *ctx, fd_ops_t *ops, context_t context);

request_t *core_request_alloc(juint32_t buffer_size, void *address);
/* HSU Temp Fix for Jungo CR B49754: Start */
request_t *core_request_alloc_ex(juint32_t buffer_size, void *address, juint16_t align);
/* HSU Temp Fix for Jungo CR B49754: End */
void core_request_free(request_t *request);

void core_add_msos_string_descriptor(void *ctx, juint8_t vendor_code);

juint8_t core_add_string_descriptor(void *ctx, char *string,
                                    /* HSU addition */
                                    hsu_str_idx_desc_type str_desc);
                                    /* End of HSU addition */

void core_release_string_descriptor(void *ctx, juint8_t index);
void core_change_string_descriptor(void *ctx, juint8_t id, char *string);
void core_handle_tx_completion(void *ctx, pipe_desc_t *pipe);

juint8_t core_get_controller_index(void *ctx);

/* Request Access macros  */
#define REQUEST_GET_BUFFER(request) (request->buffer)
#define REQUEST_SET_BUFFER(request, buf) (request->buffer = buf)
#define REQUEST_SET_CALLBACK(request, callback, context) \
        (request->complete = callback ; request->context = context)
#define REQUEST_GET_CONTEXT(request) (request->context)

/* Standard pipe descriptors */
#define DEFAULT_HIGHSPEED_BULK_IN_PIPE
#define DEFAULT_FULLSPEED_INTR_PIPE

/* HSU Addition Begin */
#ifdef FEATURE_HS_USB_POLLING
void *core_get_core_context(uint8 core_idx);
void core_set_fd_ops(void *core_ctx, fd_ops_t *fd_ops);
pipe_desc_t *core_get_pipe_desc(void *core_ctx, juint8_t direction, juint8_t ep_num);
boolean core_is_configured(void *core_ctx);
#endif /* FEATURE_HS_USB_POLLING */
/* HSU Addition End */
#endif

