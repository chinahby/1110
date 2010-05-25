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

#ifndef _UW_DEVICE_CDC_H_
#define _UW_DEVICE_CDC_H_

#include <jtypes.h>

typedef void * cdc_handle_t;
typedef void * cdc_appctx_t;

enum cdc_init_flags_t
{
    CDC_INIT_BUFFER_DMABLE         = 0x00000001L,
    CDC_INIT_SINGLE_INTERFACE      = 0x00000002L,
    CDC_INIT_VENDOR_SPECIFIC_CLASS = 0x00000004L,
    CDC_INIT_NO_CLASS_DESC         = 0x00000008L, 
    /* HSU addition: API to disable alt. settings */
    CDC_INIT_NO_ALT_SETTINGS       = 0x00000010L, 
    /* HSU addition: API to disable AID when multi-interface */
    CDC_INIT_NO_IAD                = 0x00000020L,
    /* HSU addition: API to identify NMEA instance of OBEX */
    CDC_INIT_BI_DIR_NMEA           = 0x00000040L
    /* HSU addition: API to identify NMEA instance of OBEX */
};

/* USB connection API */
typedef jresult_t (*cdc_enabled_cb)(cdc_appctx_t ctx);
typedef jresult_t (*cdc_disabled_cb)(cdc_appctx_t ctx);
typedef jresult_t (*cdc_uninit_cb)(cdc_appctx_t ctx);

/* Temp fix for B46104: added resume */
typedef jresult_t (*cdc_resume_cb)(cdc_appctx_t ctx);
/* end temp fix */

/*========================
 *     Data plane API
 *=======================*/
jresult_t cdc_write_multiple(cdc_handle_t handle, void **buffers, 
    juint32_t *buf_sizes, juint32_t req_num, juint32_t timeout);
jresult_t cdc_write(cdc_handle_t handle, void *buffer, juint32_t size);
jresult_t cdc_read(cdc_handle_t handle, void *buffer, juint32_t size);
jresult_t cdc_read_multiple(cdc_handle_t handle, void **buffers, 
    juint32_t *buf_sizes, juint32_t req_num, juint32_t timeout);
jresult_t cdc_abort_read(cdc_handle_t handle);
jresult_t cdc_abort_write(cdc_handle_t handle);
jresult_t cdc_handle_tx_completion(cdc_handle_t handle);

typedef enum {
    CDC_TRANSFER_ERROR = -1,
    CDC_TRANSFER_OK = 0,
    CDC_TRANSFER_CANCELLED = 1
} cdc_iostatus_t;

typedef void (*cdc_read_complete_cb)(cdc_iostatus_t status, void *buffer,
    juint32_t size, jbool_t transfer_complete, cdc_appctx_t ctx);

typedef void (*cdc_read_multi_complete_cb)(cdc_iostatus_t *status, void **buffers, 
    juint32_t *bytes_transferred, juint32_t req_num, cdc_appctx_t ctx);

typedef void (*cdc_write_complete_cb)(cdc_iostatus_t status, void *buffer,
    cdc_appctx_t ctx);

typedef void (*cdc_write_multi_complete_cb)(cdc_iostatus_t *status, void **buffers, 
    juint32_t *buf_sizes, juint32_t req_num, cdc_appctx_t ctx);

/*========================
 *   Control class API
 *=======================*/

/* IO completion callback for device to host transfers & notifications */
typedef void (*cdc_io_complete_cb)(cdc_iostatus_t status, cdc_appctx_t ctx);

/* IO completion callback for cdc_send_encap_response() */
typedef void (*cdc_response_complete_cb)(cdc_iostatus_t status,
    juint32_t bytes_sent, cdc_appctx_t ctx);

/*
 * Host to Device - Requests
 */

typedef jresult_t (*cdc_encap_command_cb)(void *command, juint16_t size,
    cdc_appctx_t ctx);

/*
 * Device to Host - Notifications
 */

/* no queueing supported - second call will return JEBUSY */
jresult_t cdc_send_encap_response(cdc_handle_t handle, void *response,
    juint32_t size);


typedef struct
{
    /* General Handlers */
    cdc_enabled_cb              enabled_h;
    cdc_disabled_cb             disabled_h;
    cdc_uninit_cb               uninit_h;
    
    /* Host-to-Device Request Handlers */
    cdc_encap_command_cb        encap_command_h;

    /* Encapsulated Response Completion Handlers */
    cdc_response_complete_cb    encap_response_complete_h;

    /* Transfer Completion Handlers */
    cdc_read_complete_cb        read_complete_h;
    cdc_read_multi_complete_cb  read_multi_complete_h;
    cdc_write_complete_cb       write_complete_h;
    cdc_write_multi_complete_cb write_multi_complete_h;

/* Temp fix for B46104: added resume */
    cdc_resume_cb               resumed_h;
/* end temp fix */

} cdc_callbacks_t;

typedef struct
{
    juint32_t flags;             /* Use cdc_init_flags_t */
    juint16_t max_segment_size;  /* Maximum transfer unit over data plane */
    jbool_t zlp_terminator;      /* Indicates if zero length packet should be 
                                   sent to mark end of transfer */
    juint8_t interface_number;
    char *comm_string;  /* Communication interface string descriptor */
    char *data_string;  /* Data interface string descriptor */

    juint8_t cci_subclass;               /* control-if CDC subclass */
    juint8_t cci_protocol;               /* control-if CDC protocol */
} cdc_init_info_t;

#endif

