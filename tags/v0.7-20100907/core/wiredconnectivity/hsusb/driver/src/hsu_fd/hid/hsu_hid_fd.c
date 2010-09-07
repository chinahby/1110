/*==============================================================================

                       High Speed USB HID Function Driver                    

GENERAL DESCRIPTION
Implementation of a minimal HID Function Driver, for fast dead battery charging.
Implement only the necessary functionality for successful enumeration as a 
generic HID device.
This includes implementation of the API functions defined in hid_fd.h. 
These functions are accessed by the boot fast charge API.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/hsu_fd/hid/hsu_hid_fd.c#2 $
$DateTime: 2008/04/24 00:48:51 $ $Author: merez $    

when      who     what, where, why
--------  ---     ----------------------------------------------------------
01/14/08   ab      Initial version
==============================================================================*/

/*==============================================================================
INCLUDE FILES FOR MODULE
==============================================================================*/
#include <jusb_common.h>
#include <jusb_core_fd.h>
#include "hsu_hid_fd.h"

/*==============================================================================
LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
#define HSU_HID_FD_DEV_CLASS        0x03    /* HID Class                    */
#define HSU_HID_FD_DEV_SUBCLASS     0x00    /* Not a boot device            */
#define HSU_HID_FD_DEV_PROTOCOL     0x00    /* No specific device protocol  */
#define HSU_HID_FD_PACKET_SIZE_HIGH 0x0200  /* MAX Packet size for HS       */
#define HSU_HID_FD_PACKET_SIZE_FULL 0x0040  /* MAX Packet size for FS       */
#define HSU_HID_FD_DESC_SIZE        0x09    /* Length of HID Descriptor     */
#define HSU_HID_FD_DESC_TYPE        0x21    /* Type of HID Descriptor       */
#define HSU_HID_FD_REPORT_DESC_SIZE 0x22    /* Length of Report Descriptor  */
#define HSU_HID_FD_REPORT_DESC_TYPE 0x22    /* Type of Report Descriptor    */

/*==============================================================================
Typedefs
==============================================================================*/
/* Main HID FD structure definition, holds the HID FD context  */
typedef struct _hsu_hid_fd_ctx_t
{
    void      *core_ctx;     /* Core context                              */
    fd_ops_t  fd_ops;        /* Core callbacks                            */
    fd_desc_t *fd_desc;      /* Descriptor for this FD                    */
    request_t *ep0_request;  /* Buffer for EP0 requests processing        */
    jbool_t   is_enabled;    /* Set true when enabled & not suspended     */
} hsu_hid_fd_ctx_t;

/* HID Descriptor structure definition, as defined in HID Device Class spec */
typedef PACKED struct _hsu_hid_fd_hid_desc
{
    uint8   bLength;                /* HID descriptor length                */
    uint8   bHIDDescriptorType;     /* HID descriptor type                  */
    uint16  bcdHID;                 /* Version of HID spec                  */
    uint8   bCountryCode;           /* Country code                         */
    uint8   bNumDescriptors;        /* Number of HID descriptors to follow  */
    uint8   bReportDescriptorType;  /* Report descriptor type               */
    uint16  wDescriptorLength;      /* Report descriptor length             */
} hsu_hid_fd_hid_desc_t;

/*==============================================================================
Forward Definitions
==============================================================================*/
static jresult_t hsu_hid_fd_control_msg(void *context, void *buffer);

/*==============================================================================
Variables
==============================================================================*/

/* Endpoint variable, used to pass desired EP configuration to the 
   core. Implements the mandatory HID interrupt EP */  
static pipe_desc_t hsu_hid_fd_pipe_desc_array[] = {
    {
        HSU_HID_FD_PACKET_SIZE_HIGH, /* max_pkt_size_high                    */
        HSU_HID_FD_PACKET_SIZE_FULL, /* max_pkt_size_full                    */
        PIPE_INTR,                   /* type                                 */
        DIRECTION_IN,                /* direction                            */
        0,                           /* sync_type                            */
        0,                           /* usage_type                           */
        0,                           /* transaction_per_microframe           */
        0,                           /* pipe: Filled by the Core             */
        { 0x0a },                    /* nak_rate                             */
        0,                           /* extra_descriptor_size                */
        NULL,                        /* extra_descriptor                     */
        0,                           /* poll_interval_high                   */
        NULL,                        /* fd_handle                            */
        NULL,                        /* dcd_handle                           */
        0,                           /* Packet size for current speed        */
        PIPE_UNKNOWN,                /* pipe status                          */
        {NULL, NULL}                 /* requests queue                       */
    }
};

/* Alternative interface variable. holds the interface's pipes */
static alt_interface_desc_t hsu_hid_fd_alt_desc_array[] = {
    {
        1,                          /* pipe_count  */
        hsu_hid_fd_pipe_desc_array  /* pipes       */
    }
};

/* HID Descriptor variable, points to HID Report Descriptor */
static hsu_hid_fd_hid_desc_t hsu_hid_fd_hid_desc = {
    HSU_HID_FD_DESC_SIZE,   /* HID descriptor length               */
    HSU_HID_FD_DESC_TYPE,   /* HID descriptor type                 */ 
    0x0110,                 /* Version of HID spec                 */ 
    0x00,                   /* Country code                        */ 
    0x01,                   /* Number of HID descriptors to follow */
    HSU_HID_FD_REPORT_DESC_TYPE,   /* Report descriptor type       */
    HSU_HID_FD_REPORT_DESC_SIZE    /* Report descriptor length     */
};

/* HID Report Descriptor. Implements a generic HID compliant device. This is a 
   made up report descriptor, which describes a vendor specific type of HID 
   device USAGE, with two vendor defined bytes of INPUT & OUTPUT data */
static uint8 hsu_hid_fd_report_desc[] = {
    0x06, 0xA0, 0xFF,   /* USAGE PAGE (vendor defined)          */
    0x09, 0xA5,         /* USAGE (vendor defined)               */
    0xA1, 0x01,         /* COLLECTION (Application)             */
    0x09, 0xA6,         /*   USAGE (vendor defined)             */
    0x09, 0xA7,         /*   USAGE (vendor defined              */
    0x15, 0x80,         /*   LOGICAL MINIMUM (-127)             */
    0x25, 0x7F,         /*   LOGICAL MAXIMUM (128)              */
    0x75, 0x08,         /*   REPORT SIZE (8) (bits)             */
    0x95, 0x02,         /*   REPORT COUNT (2) (fields)          */
    0x81, 0x02,         /*   INPUT (Data, Variable, Absolute)   */
    0x09, 0xA9,         /*   USAGE (vendor defined)             */
    0x15, 0x80,         /*   LOGICAL MINIMUM (-127)             */
    0x25, 0x7F,         /*   LOGICAL MAXIMUM (128)              */
    0x75, 0x08,         /*   REPORT SIZE (8) (bits)             */
    0x95, 0x02,         /*   REPORT COUNT  (2) (fields)         */
    0x91, 0x02,         /*   OUTPUT (Data, Variable, Absolute)  */
    0xC0                /* END COLLECTION                       */           

};

/* Interface variable for the HID device. Implements a HID class 
   device, with no specific subclass or protocol */
static interface_desc_t hsu_hid_fd_if_desc_array[] = {
    {
            hsu_hid_fd_control_msg,         /* control_msg                  */
            hsu_hid_fd_alt_desc_array,      /* alt_ifs                      */
            1,                              /* alt_if_num                   */
            0,                              /* alt_if_curr                  */
            (uint8*)&hsu_hid_fd_hid_desc,   /* extra_descriptor             */
            HSU_HID_FD_DESC_SIZE,           /* extra_descriptor_size        */
            UWD_UNDEFINED_INTERFACE,        /* number: to be filled by core */
            HSU_HID_FD_DEV_CLASS,           /* if_class                     */
            HSU_HID_FD_DEV_SUBCLASS,        /* if_subclass                  */
            HSU_HID_FD_DEV_PROTOCOL,        /* if_protocol                  */
            0                               /* if_string                    */
    }
};

/* Configuration variable for the HID device */
static fd_desc_t hsu_hid_fd_conf_desc = {
        SPEED_HIGH,                 /* speed                                 */
        SPEED_UNKNOWN,              /* current speed                         */
        1,                          /* interface_count                       */
        hsu_hid_fd_if_desc_array,   /* interfaces                            */
        NULL                        /* struct usb_interface_assoc_descriptor */
};

/*==============================================================================
FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/

/**************************************************************************
* Function name:  hsu_hid_fd_ep0_callback
* Description:    Callback function called once transfer on ep0 is done 
* Parameters:
*     @request: (IN) Core request
*
* Return value:   None
* Scope:          Local
**************************************************************************/
static void hsu_hid_fd_ep0_complete_cb(request_t *req)
{ 
    HSU_ASSERT(req != NULL);
    HSU_MSG_LOW("hsu_hid_fd_ep0_complete_cb entry: status=%d", req->status,0,0);

    if (req->status != REQUEST_COMPLETED)
    {
        HSU_MSG_ERROR("failed sending data through ep0: status=%d", req->status,0,0);
    }
    
    req->status = REQUEST_READY;
}

/**************************************************************************
* Function name:  hsu_hid_fd_control_write
* Description:    Send response on ep0
* Parameters: 
*     @ctx:     (IN) HIDFD context
*     @buffer:  (IN) Pointer to buffer with data
*     @size:    (IN) Size of buffer
* Return value: 0 for success, or an error code 
* Scope:        Local
**************************************************************************/
static int hsu_hid_fd_control_write(hsu_hid_fd_ctx_t* context, void *buffer, 
                               uint16 size)
{
    jresult_t rc = -1;
    hsu_hid_fd_ctx_t *ctx = (hsu_hid_fd_ctx_t*)context; 

    HSU_MSG_LOW("hsu_hid_fd_control_write:entered", 0,0,0);
    HSU_ASSERT((context != NULL) && (buffer != NULL));

    if (ctx->ep0_request->status == REQUEST_INPROGRESS)
    {
        rc = JEBUSY;
    }
    else
    {
        ctx->ep0_request->status = REQUEST_READY;
        rc = core_send_data(ctx->core_ctx, 0, ctx->ep0_request);
    }

    if (rc < 0)
    {
        HSU_MSG_ERROR("hsu_hid_fd_control_write:failed to send response", 0,0,0);
    }

    return rc;
}

/**************************************************************************
* Function name:  hsu_hid_fd_control_msg
* Description:    Handle ep0 control messages
* Parameters: 
*     @ctx:     (IN) HIDFD context
*     @buffer:  (IN) Pointer to buffer with setup request
* Return value: 0 for success, or an error code 
* Scope:        Local
**************************************************************************/
static jresult_t hsu_hid_fd_control_msg(void *context, void *buffer)
{
    jresult_t rc = JEOPNOTSUPP;
    struct usb_ctrlrequest *setup = (struct usb_ctrlrequest*)buffer;
    hsu_hid_fd_ctx_t *ctx = (hsu_hid_fd_ctx_t*)context;

    HSU_MSG_HIGH("hidfd_control_msg:entered", 0,0,0);
    HSU_ASSERT((context != NULL) && (buffer != NULL));
    
    /* Check and make sure our ep0 request can be used */
    if (ctx->ep0_request->status == REQUEST_INPROGRESS)
    {
        HSU_MSG_ERROR("hsu_hid_fd_control_msg:ep0_req is busy! can not handle request %d", 
            setup->bRequest,0,0); 
        return JEBUSY;
    }

    /* Handle only REPORT_DESCRIPTOR request. */
    /* Convert setup->wValue to 8 bits host format */
    if (setup->bRequest == USB_REQ_GET_DESCRIPTOR && 
        (((setup->wValue) >> 8) & 0x00ff) == HSU_HID_FD_REPORT_DESC_TYPE)
    {
        rc =  hsu_hid_fd_control_write(context, (void*)hsu_hid_fd_report_desc, 
            HSU_HID_FD_REPORT_DESC_SIZE);
    }

    HSU_MSG_HIGH("hidfd_control_msg:exited normally...", 0,0,0);
    return rc;
}

/**************************************************************************
* Function name:  hsu_hid_fd_copy_fd_desc
* Description:    Allocate all FD descriptors and copies them into
*                 one structure.
* Parameters: 
*
* Return value: FD descriptor, or NULL in case of an error.
* Scope:        Local
**************************************************************************/
static fd_desc_t *hsu_hid_fd_copy_fd_desc(void)
{
    pipe_desc_t             *pipe = NULL;
    alt_interface_desc_t    *alt = NULL;
    interface_desc_t        *iface = NULL;
    fd_desc_t               *desc = NULL;
    hsu_hid_fd_hid_desc_t   *hid = NULL;

    HSU_MSG_LOW("hsu_hid_fd_copy_fd_desc:entered", 0,0,0);

    do {
        /* Copy Pipe data */
        pipe = jmalloc(sizeof(hsu_hid_fd_pipe_desc_array), 0);
        if (!pipe)
        {
            break;
        }

        (void) j_memcpy(pipe, hsu_hid_fd_pipe_desc_array, 
            sizeof(hsu_hid_fd_pipe_desc_array));

        /* Copy alternative interface data */
        alt = jmalloc(sizeof(hsu_hid_fd_alt_desc_array), 0);
        if (!alt)
        {
            break;
        }

        (void) j_memcpy(alt, hsu_hid_fd_alt_desc_array, 
            sizeof(hsu_hid_fd_alt_desc_array));

        /* Copy HID descriptor data */
        hid = jmalloc(sizeof(hsu_hid_fd_hid_desc), 0);
        if (!hid)
        {
            break;
        }

        (void) j_memcpy((void*)hid, (void*)&hsu_hid_fd_hid_desc, 
            sizeof(hsu_hid_fd_hid_desc));

        /* Copy interface data */
        iface = jmalloc(sizeof(hsu_hid_fd_if_desc_array), 0);
        if (!iface)
        {
            break;
        }

        (void) j_memcpy(iface, hsu_hid_fd_if_desc_array, 
            sizeof(hsu_hid_fd_if_desc_array));

        /* Copy configuration data */
        desc = jmalloc(sizeof(hsu_hid_fd_conf_desc), 0);
        if (!desc)
        {
            break;
        }

        (void) j_memcpy(desc, &hsu_hid_fd_conf_desc, 
            sizeof(hsu_hid_fd_conf_desc));

        /* Set up pointers */
        alt->pipes = pipe;
        iface->alt_ifs = alt;
        iface->extra_descriptor = (uint8*)hid;
        desc->interfaces = iface;

        HSU_MSG_LOW("hsu_hid_fd_copy_fd_desc:exited normally...", 0,0,0);
        return desc;

    } while (0);

    /* Error conditions */
    if (pipe)
    {
        jfree(pipe);
    }

    if (alt)
    {
        jfree(alt);
    }

    if (hid)
    {
        jfree((void*)hid);
    }

    if (iface)
    {
        jfree(iface);
    }

    return NULL;
}

/*==============================================================================
REGISTER FUNCTIONS TO CORE:

    hsu_hid_fd_get_pipe_config
    hsu_hid_fd_enable
    hsu_hid_fd_disable
    hsu_hid_fd_suspend
    hsu_hid_fd_resume
    hsu_hid_fd_unint
==============================================================================*/

/**************************************************************************
* Function name:  hsu_hid_fd_get_pipe_config
* Description:    Called by the core to return the configuration descriptor 
*                 for the HID device
* Parameters:
*     @context: (IN) FD context
*
* Return value: FD descriptor
* Scope:        Local
**************************************************************************/
static fd_desc_t *hsu_hid_fd_get_pipe_config(context_t context)
{
    hsu_hid_fd_ctx_t *ctx = (hsu_hid_fd_ctx_t*)context;

    HSU_MSG_HIGH("hsu_hid_fd_get_pipe_config:entered", 0,0,0);
    HSU_ASSERT(context != NULL);

    return ctx->fd_desc;
}

/**************************************************************************
* Function name:  hsu_hid_fd_enable
* Description:    Called by the core when the interface is enabled
* Parameters:
*     @context: (IN) FD context
*
* Return value: 0 for success, or an error code 
* Scope:        Local
**************************************************************************/
static jresult_t hsu_hid_fd_enable(context_t context)
{
    hsu_hid_fd_ctx_t *ctx = (hsu_hid_fd_ctx_t *)context;

    HSU_MSG_HIGH("hsu_hid_fd_enable:entered", 0,0,0);
    HSU_ASSERT(context != NULL);
    HSU_ASSERT(ctx->is_enabled == FALSE);

    if (ctx->is_enabled)
    {
        return JEPERM;
    }

    ctx->is_enabled = TRUE;
    return 0;
}

/**************************************************************************
* Function name:  hsu_hid_fd_disable
* Description:    Called by the core when the interface is disabled
* Parameters:
*     @context: (IN) FD context
*
* Return value: 0 for success, or an error code 
* Scope:        Local
**************************************************************************/
static void hsu_hid_fd_disable(context_t context)
{
    hsu_hid_fd_ctx_t *ctx = (hsu_hid_fd_ctx_t*)context;

    HSU_MSG_HIGH("hsu_hid_fd_disable:entered", 0,0,0);
    HSU_ASSERT(context != NULL);
    HSU_ASSERT(ctx->is_enabled == TRUE);

    if (!ctx->is_enabled)
    {
        return;
    }

    ctx->is_enabled = FALSE;
}

/**************************************************************************
* Function name:  hsu_hid_fd_suspend
* Description:    Called by the core when the interface is suspended
* Parameters:
*     @context: (IN) FD context
*
* Return value: 0 for success, or an error code 
* Scope:        Local
**************************************************************************/
static jresult_t hsu_hid_fd_suspend(context_t context)
{
    HSU_MSG_HIGH("hsu_hid_fd_suspend:entered", 0,0,0);
    HSU_ASSERT(context != NULL);

    return 0;
}

/**************************************************************************
* Function name:  hsu_hid_fd_resume
* Description:    Called by the core when the interface is resumed
* Parameters:
*     @context: (IN) FD context
*
* Return value: 0 for success, or an error code 
* Scope:        Local
**************************************************************************/
static jresult_t hsu_hid_fd_resume(context_t context)
{
    HSU_MSG_HIGH("hsu_hid_fd_resume:entered", 0,0,0);
    HSU_ASSERT(context != NULL);

    return 0;
}

/**************************************************************************
* Function name:  hsu_hid_fd_uninit
* Description:    Called by the core to un-initiallize the interface
* Parameters:
*     @context: (IN) FD context
*
* Return value: 0 for success, or an error code 
* Scope:        Local
**************************************************************************/
static jresult_t hsu_hid_fd_uninit(context_t context)
{
    hsu_hid_fd_ctx_t *ctx = (hsu_hid_fd_ctx_t*)context;

    HSU_MSG_HIGH("hsu_hid_fd_uninit:entered", 0,0,0);
    HSU_ASSERT(context != NULL);

    /* Free requests */
    if (ctx->ep0_request)
    {
        core_request_free(ctx->ep0_request);
        ctx->ep0_request = NULL;
    }

    // Free all allocated descriptors
    if (ctx->fd_desc->interfaces->alt_ifs->pipes)
    {
        jfree(ctx->fd_desc->interfaces->alt_ifs->pipes);
    }

    if (ctx->fd_desc->interfaces->alt_ifs)
    {
        jfree(ctx->fd_desc->interfaces->alt_ifs);
    }

    if (ctx->fd_desc->interfaces->extra_descriptor)
    {
        jfree(ctx->fd_desc->interfaces->extra_descriptor);
    }

    if (ctx->fd_desc->interfaces->extra_descriptor)
    {
        jfree(ctx->fd_desc->interfaces);
    }

    if (ctx->fd_desc)
    {
        jfree(ctx->fd_desc);
    }

    if (ctx)
    {
        jfree(ctx);
    }

    HSU_MSG_HIGH("hsu_hid_fd_uninit:exited normally...", 0,0,0);
    return 0;
}

/**************************************************************************
* Function name:  hsu_hid_fd_init
* Description:    Called by the core to initialize the HID function driver
* Parameters: 
*     @ctx: (IN) Core context
*
* Return value: 0 on success, otherwise an error code 
* Scope:        Global
**************************************************************************/
jresult_t hsu_hid_fd_init(void *core_ctx)
{
    jresult_t rc;
    hsu_hid_fd_ctx_t *hidfd_ctx;

    HSU_MSG_HIGH("hsu_hid_fd_init:entered", 0,0,0);
    HSU_ASSERT(core_ctx != NULL);

    /* Allocate HID FD context structure */
    hidfd_ctx = (hsu_hid_fd_ctx_t*)jmalloc(sizeof(hsu_hid_fd_ctx_t), M_ZERO);

    if (!hidfd_ctx)
    {
        HSU_MSG_ERROR("hsu_hid_fd_init:failed to allocate hidfd context", 0,0,0);
        return JENOMEM;
    }

    do {

        hidfd_ctx->core_ctx = core_ctx; /* Point to core */
        hidfd_ctx->is_enabled = FALSE;

        /* Copy configuration descriptor to HID FD context */
        hidfd_ctx->fd_desc = hsu_hid_fd_copy_fd_desc(); 
        if (!hidfd_ctx->fd_desc)
        {
            rc = JENOMEM;
            HSU_MSG_ERROR("hsu_hid_fd_init:failed to copy hid descriptor", 0,0,0);
            break;
        }

        /* Fill in the standard fd_ops functions that core needs */
        hidfd_ctx->fd_ops.get_pipe_config = hsu_hid_fd_get_pipe_config;
        hidfd_ctx->fd_ops.enable = hsu_hid_fd_enable;
        hidfd_ctx->fd_ops.disable = hsu_hid_fd_disable;
        hidfd_ctx->fd_ops.suspend = hsu_hid_fd_suspend;
        hidfd_ctx->fd_ops.resume = hsu_hid_fd_resume;
        hidfd_ctx->fd_ops.uninit = hsu_hid_fd_uninit;

        // Allocate & Initialize EP0 request
        hidfd_ctx->ep0_request  = 
            core_request_alloc(HSU_HID_FD_REPORT_DESC_SIZE, NULL);

        if (!hidfd_ctx->ep0_request)
        {
            rc = JENOMEM;
            HSU_MSG_ERROR("hsu_hid_fd_init:failed to allocate hidfd ep0 request", 0,0,0);
            break;
        }

        /* The completion callback of ep0 is always the same  */
        hidfd_ctx->ep0_request->complete = hsu_hid_fd_ep0_complete_cb;
        hidfd_ctx->ep0_request->context = (void*)hidfd_ctx;
        
        /* Copy the report descriptor into the request buffer */
        (void) j_memcpy(hidfd_ctx->ep0_request->buffer.vaddr, 
            (void*)hsu_hid_fd_report_desc, HSU_HID_FD_REPORT_DESC_SIZE);

        hidfd_ctx->ep0_request->zero = 1;
        hidfd_ctx->ep0_request->transfer_size = HSU_HID_FD_REPORT_DESC_SIZE;

        /* Resigter FD to core */
        rc = core_register_fd(hidfd_ctx->core_ctx, &hidfd_ctx->fd_ops, (context_t)hidfd_ctx);
        HSU_MSG_HIGH("hsu_hid_fd_init:exited normally...", 0,0,0);
        return rc;
    } while (0);

    /* Error condition */
    jfree(hidfd_ctx);
    return JENOMEM;    
}
