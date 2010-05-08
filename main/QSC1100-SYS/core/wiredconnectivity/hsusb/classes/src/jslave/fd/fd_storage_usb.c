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

#include <jusb_common.h>
#include <jusb_core_fd.h>
#include <jusb_chp9.h>
#include "fd_storage.h"

#define USB_PR_BULK     0x50    /* Bulk-only */
#define USB_SC_SCSI     0x06    /* Transparent SCSI */
#define USB_SC_UFI      0x04    /* UFI */

#define DEV_CLASS       USB_CLASS_MASS_STORAGE
#define DEV_SUBCLASS    USB_SC_SCSI     
#define DEV_PROTOCOL    USB_PR_BULK
#define MAX_LUNS        8

/* Command Block Wrapper */
struct bulk_cb_wrap {
    juint32_t    signature;      /* Contains 'USBC' */
    juint32_t    tag;            /* Unique per-command id */
    juint32_t    data_size;      /* Size of the data, in bytes */
    juint8_t     flags;          /* Direction in bit 7 */
    juint8_t     lun;            /* Logical Unit (LUN) number (normally 0) */
    juint8_t     length;         /* Length (in bytes) of the CDB;
                                   Value should be <= MAX_COMMAND_SIZE. */
    juint8_t     CDB[16];        /* Command Data Block */
};

#define USB_BULK_CB_WRAP_LEN    31
#define USB_BULK_CB_SIG         0x43425355      /* Spells out USBC */
#define USB_BULK_IN_FLAG        0x80

/* Command Status Wrapper */
struct bulk_cs_wrap {
    juint32_t    Signature;              /* Should = 'USBS' */
    juint32_t    Tag;                    /* Same as original command */
    juint32_t    Residue;                /* Amount of data not transferred */
    juint8_t     Status;                 /* See below */
};

#define USB_BULK_CS_WRAP_LEN    13
#define USB_BULK_CS_SIG         0x53425355      /* Spells out 'USBS' */

/* Bulk-only class specific requests */
#define USB_BULK_RESET_REQUEST          0xff
#define USB_BULK_GET_MAX_LUN_REQUEST    0xfe

/* Function prototypes */
static jresult_t control_msg(void *context, void *buffer);

/* Structs used to pass the desired pipe configuration to the Core */
static pipe_desc_t pipe_desc_array[] = {
    {
        /* max_pkt_size_high            */ 512,
        /* max_pkt_size_full            */ 64,
        /* type                         */ PIPE_BULK,
        /* direction                    */ DIRECTION_OUT,
        /* sync_type                    */ 0,
        /* usage_type                   */ 0,
        /* transaction_per_microframe   */ 0,
        /* pipe                         */ 0, /* Filled by the Core */
        /* nak_rate                     */ { 0 }
    }, 
    {
        /* max_pkt_size_high            */ 512,
        /* max_pkt_size_full            */ 64,
        /* type                         */ PIPE_BULK,
        /* direction                    */ DIRECTION_IN,
        /* sync_type                    */ 0,
        /* usage_type                   */ 0,
        /* transaction_per_microframe   */ 0,
        /* pipe                         */ 0, /* Filled by the Core */
        /* nak_rate                     */ { 0 } 
    }
};

static alt_interface_desc_t alt_desc_array[] = {
    {
        /* pipe_count                   */ 2,
        /* pipes                        */ pipe_desc_array
    }
};

static interface_desc_t if_desc_array[] = {
    {
        /* control_msg                  */ control_msg, 
        /* alt_ifs                      */ alt_desc_array,
        /* alt_if_num                   */ 1,
        /* alt_if_curr                  */ 0,
        /* extra_descriptor_size        */ 0,
        /* extra_descriptor             */ 0,
        /* number                       */ 0, /* Filled by the Core */
        /* if_class                     */ DEV_CLASS, 
        /* if_subclass                  */ DEV_SUBCLASS,
        /* if_protocol                  */ DEV_PROTOCOL,
        /* if_string                    */ 0    
    }
};

static fd_desc_t fd_desc = {
    /* speed            */ SPEED_HIGH, 
    /* current speed    */ SPEED_UNKNOWN, 
    /* interface_count  */ 1,
    /* interfaces       */ if_desc_array
};

static void cbw_read_complete(request_t *request);

/**
 * Function name:  test_and_set_cmd_in_progress
 * Description:    Completion callback, which is called at the end of a
 *                 command block wrapper (CBW) read.
 *                 Sets the command to status ''in progress''.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: Last command status
 * Scope:        Local
 **/
static __INLINE__ jbool_t test_and_set_cmd_in_progress(fd_storage_t *fd)
{
    /* If no command was in progress, function should return 0 */
    jbool_t old = fd->current_cmd_status;
    fd->current_cmd_status = 1;
    return old;
}

/**
 * Function name:  clear_cmd_in_progress
 * Description:    Clears a command's ''in progress'' status
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Local
 **/
static __INLINE__ void clear_cmd_in_progress(fd_storage_t *fd)
{
    fd->current_cmd_status = 0;
}

/**
 * Function name:  handle_cbw
 * Description:    Handles a command block wrapper (CBW) request.
 * Parameters: 
 *     @fd:     (IN) FD context
 *     @buffer: (IN) CBW buffer
 *     @size:   (IN) Buffer size, in bytes
 *
 * Return value: None
 * Scope:        Global
 **/
void handle_cbw(fd_storage_t *fd, void *buffer, juint32_t size)
{
    struct bulk_cb_wrap *cbw = (struct bulk_cb_wrap *)buffer;
    juint8_t command[MAX_COMMAND_SIZE];

    DBG_V(DSLAVE_MS_USB, ("MASS: Got CBW [%d] Tag: %x\n", size, 
        le32toh(cbw->tag)));

    if (size < USB_BULK_CB_WRAP_LEN)
    {
        DBG_W(DSLAVE_MS_USB, ("MASS: Received buffer size (%d) is smaller "
            "than the valid CBW buffer size (%d).\n",
            size, USB_BULK_CB_WRAP_LEN));
        goto error;
    }
    
    if (le32toh(cbw->signature) != USB_BULK_CB_SIG)
    {
        DBG_W(DSLAVE_MS_USB, ("MASS: Bad CBW signature.\n"));
        goto error;
    }

    if (test_and_set_cmd_in_progress(fd))
    {
        DBG_W(DSLAVE_MS_USB, ("MASS: Got CBW when one is already in "
            "progress."));
        goto error;
    }

    /* Make a copy of the command so do_scsi_command can use the buffer freely */
    j_memcpy(&command, cbw->CDB, sizeof(command));

    do_scsi_command(fd->scsi_device, command, cbw->length,
        le32toh(cbw->data_size), le32toh(cbw->tag), cbw->lun,
        cbw->flags & USB_BULK_IN_FLAG,
        (juint8_t *)fd->cmd_request->buffer.vaddr);

    return;

error:
    fatal_processing_error(fd);
}

/**
 * Function name:  send_csw_callback
 * Description:    Completion callback, which is called at the completion of a
 *                 command status wrapper (CSW) request (send_csw).
 * Parameters: 
 *     @request: (IN) Core request
 *
 * Return value: None
 * Scope:        Global
 **/
void send_csw_callback(request_t *request)
{
    fd_storage_t *fd = (fd_storage_t *)request->context;
    request_status_t status = request->status;

    DBG_V(DSLAVE_MS_USB, ("MASS: CSW sent %d out of %d.\n",
        request->bytes_transferred, request->transfer_size));

    /* Request is ready for next transfer */
    request->status = REQUEST_READY;

    if (status != REQUEST_COMPLETED ||
        request->bytes_transferred != request->transfer_size)
    {
        DBG_W(DSLAVE_MS_USB, ("MASS: Error sending CSW. Stalling IN.\n"));
        if (status != REQUEST_CANCELLED)
            core_abort_pipe(fd->core_ctx, fd->in_pipe);
        core_stall_pipe(fd->core_ctx, fd->in_pipe, 1);
    }
    else
    {
        jresult_t rc;

        /* Get next Command (CBW) */
        DBG_V(DSLAVE_MS_USB, ("MASS: Waiting for next CBW.\n"));
        clear_cmd_in_progress(fd);
        rc = post_read(fd, fd->cmd_request, USB_BULK_CB_WRAP_LEN, 
            cbw_read_complete, fd);
        if (rc)
        {
            DBG_E(DSLAVE_MS_USB, ("Failed to post next CBW (%d)\n", rc));
        }
    }
}

/**
 * Function name:  send_csw
 * Description:    Sends a command status wrapper (CSW) request to the host.
 *                 All SCSI commands must finish processing by calling this
 *                 function.
 * Parameters: 
 *     @fd:      (IN) FD context
 *     @tag:     (IN) Tag received in the matching CBW
 *     @residue: (IN) Amount of data (in bytes) that was NOT transferred in
 *                    the data stage
 *     @status:  (IN) Command status
 *
 * Return value: None
 * Scope:        Global
 **/
void send_csw(fd_storage_t *fd, juint32_t tag, juint32_t residue, juint8_t status)
{
    jresult_t rc;
    struct bulk_cs_wrap *csw = (struct bulk_cs_wrap *)
        (fd->cmd_request->buffer.vaddr);

    if (!command_in_progress(fd))
        return;

    if (status)
    {
        DBG_V(DSLAVE_MS_USB, ("MASS: Sending CSW [Tag: %x], Status 0x%x, "
            "Residue %d.\n", tag, status, residue));
    }

    csw->Signature      = htole32(USB_BULK_CS_SIG);
    csw->Tag            = htole32(tag);
    csw->Residue        = htole32(residue);
    csw->Status         = status;

    rc = post_write(fd, fd->cmd_request, USB_BULK_CS_WRAP_LEN, 
        send_csw_callback, fd);
    if (rc)
        fatal_processing_error(fd);
}

/**
 * Function name:  post_write
 * Description:    Sends data to the host.
 * Parameters: 
 *     @fd:       (IN) FD context 
 *     @req:      (IN) Request to use for the transfer
 *     @size:     (IN) Transfer size, in bytes
 *     @callback: (IN) Completion callback
 *     @context:  (IN) Context for the completion callback
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Global
 **/
jresult_t post_write(fd_storage_t *fd, request_t *req, juint32_t size, 
    callback_t callback, context_t context)
{
    jresult_t rc = 0;

    req->transfer_size = size;
    req->complete = callback;
    req->context = context;

    rc = core_send_data(fd->core_ctx, fd->in_pipe, req);
    if (rc)
    {
        DBG_W(DSLAVE_MS_USB, ("MASS: Error writing data. Stalling IN pipe.\n"));
        core_abort_pipe(fd->core_ctx, fd->in_pipe);
        core_stall_pipe(fd->core_ctx, fd->in_pipe, 1);
    }

    return rc;
}

/**
 * Function name:  post_read
 * Description:    Sends a read request to the host.
 * Parameters: 
 *     @fd:       (IN) FD context
 *     @req:      (IN) Request to use for the transfer
 *     @size:     (IN) Trasfer size, in bytes
 *     @callback: (IN) Completion callback
 *     @context:  (IN) Context for the completion callback
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Global
 **/
jresult_t post_read(fd_storage_t *fd, request_t *req, juint32_t size, 
    callback_t callback, context_t context)
{
    jresult_t rc = 0;

    req->transfer_size = size;
    req->complete = callback;
    req->context = context;

    DBG_V(DSLAVE_MS_USB, ("Posting read on pipe %d, size %d, to FD %p.\n",
        fd->out_pipe->address, size, fd));

    rc = core_read_data(fd->core_ctx, fd->out_pipe, req);
    if (rc)
    {
        DBG_W(DSLAVE_MS_USB, ("MASS: Error posting a read request - 0x%x. "
            "Stalling OUT pipe.\n", rc));
        core_stall_pipe(fd->core_ctx, fd->out_pipe, 1);
    }

    return rc;
}

/**
 * Function name:  stop_io
 * Description:    Cancels the current USB I/O request.
 * Parameters: 
 *     @fd:       (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **/
void stop_io(fd_storage_t *fd)
{
    core_abort_pipe(fd->core_ctx, fd->out_pipe);
}

/**
 * Function name:  fatal_processing_error
 * Description:    Handles a fatal data processing error, including data pipes
 *                 stalling.
* Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **/
void fatal_processing_error(fd_storage_t *fd)
{
    DBG_E(DSLAVE_MS_USB, ("MASS: ******** Fatal processing error *********\n"));
    core_stall_pipe(fd->core_ctx, fd->in_pipe, 1);
    core_stall_pipe(fd->core_ctx, fd->out_pipe, 1);
    clear_cmd_in_progress(fd); 
}

/**
 * Function name:  abort_out_pipe
 * Description:    Aborts pending read requests.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **/
void abort_out_pipe(fd_storage_t *fd)
{
    core_abort_pipe(fd->core_ctx, fd->out_pipe);
}

/**
 * Function name:  stall_in_pipe
 * Description:    Stalls the IN data pipe.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **/
void stall_in_pipe(fd_storage_t *fd)
{
    core_stall_pipe(fd->core_ctx, fd->in_pipe, 1);
}

/**
 * Function name:  stall_out_pipe
 * Description:    Stalls the OUT data pipe.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **/
void stall_out_pipe(fd_storage_t *fd)
{
    core_stall_pipe(fd->core_ctx, fd->out_pipe, 1);
}

/**
 * Function name:  ep0_callback
 * Description:   Callback function called once transfer on ep0 is done 
 * Parameters:
 *     @request: (IN) Core request
 *
 * Return value:   None
 * Scope:          Local
 **/
static void ep0_callback(request_t *request)
{
    if (request->status != REQUEST_COMPLETED)
    {
        DBG_W(DSLAVE_MS_USB, ("MASS: %s - failed sending data through ep0 "
            "%d.\n", request->status == REQUEST_CANCELLED ?
            "Cancelled" : "Error", request->status));
    }
    request->status = REQUEST_READY;
}

/**
 * Function name:  send_ep0_reply
 * Description:    Send a reply to an endpoint 0 command.
 * Parameters: 
 *     @fd:   (IN) FD context 
 *     @size: (IN) Buffer size, in bytes.
 *                 (Data should be already set in fd->ep0_request).
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t send_ep0_reply(fd_storage_t *fd, juint32_t size)
{
    fd->ep0_request->transfer_size = size;

    return core_send_data(fd->core_ctx, 0, fd->ep0_request);
}

/**
 * Function name:  cbw_read_complete
 * Description:    Completion callback, which is called at the completion of a
 *                 command block wrapper (CBW) request (send_csw).
 * Description:    Command block wrapper (CBW) read completion callback.
 *                 Sets the command to status ''in progress''.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: Last command status
 * Scope:        Local
 **/
void cbw_read_complete(request_t *request)
{
    fd_storage_t *fd = (fd_storage_t *)request->context;
    request_status_t status = request->status;

    DBG_V(DSLAVE_MS_USB, ("CBW Complete ep %d size %d status %d\n",
        fd->out_pipe->address, request->transfer_size, status));

    /* Request is ready for next transfer */
    request->status = REQUEST_READY;
   
    if (status != REQUEST_COMPLETED)
    {
        /* HSU addition: Error handling */
        if (status != REQUEST_CANCELLED)
        {
            DBG_W(DSLAVE_MS_USB, ("MASS: Error %d on reading data from out pipe.\n", status));
        }
        else
            DBG_I(DSLAVE_MS_USB, ("MASS: %s - failed reading data from out pipe.\n",
                status == REQUEST_CANCELLED ? "Cancelled" : "Error", status));
        /******* End HSU addition **************************/
    }
    else
    {
        DBG_V(DSLAVE_MS_USB, ("MASS: Got packet (%d)\n", 
            request->bytes_transferred));       

        handle_cbw(fd, request->buffer.vaddr, request->bytes_transferred);
    }
}

/** 
 * Function name:  control_msg
 * Description:    Handles a mass storage class-specific endpoint 0 request.
 * Parameters: 
 *     @context: (IN) FD context
 *     @buffer:  (IN) Buffer containing the control request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t control_msg(void *context, void *buffer)
{
    jresult_t rc = JENOTSUP;
    fd_storage_t *fd = (fd_storage_t *)context;
    struct usb_ctrlrequest *req = (struct usb_ctrlrequest *)buffer;

    switch (req->bRequest)
    {
    case USB_BULK_RESET_REQUEST:
        DBG_I(DSLAVE_MS_USB, ("MASS: Bulk Reset Request.\n"));

        if (req->wValue || req->wLength != 0) /* HSU fix */
        {
            DBG_E(DSLAVE_MS_USB, ("MASS: Invalid MS_RESET request.\n"));
            break;
        }

        clear_cmd_in_progress(fd);
        core_abort_pipe(fd->core_ctx, fd->in_pipe);
        core_abort_pipe(fd->core_ctx, fd->out_pipe);

        /* Clear Stalls from pipes */
        core_stall_pipe(fd->core_ctx, fd->in_pipe, 0);
        core_stall_pipe(fd->core_ctx, fd->out_pipe, 0);

        /* Even if it fails, we still Reset since the host wants it */
        rc = send_ep0_reply(fd, 0);

        rc = post_read(fd, fd->cmd_request, USB_BULK_CB_WRAP_LEN, 
            cbw_read_complete, fd);
        /* HSU addition: CR 159898 */
        if (rc)
        {
            DBG_E(DSLAVE_MS_USB, ("Failed to post CBW on RESET_REQUEST (%d)\n", rc));
        }
        /******* End HSU addition **************************/
        break;

    case USB_BULK_GET_MAX_LUN_REQUEST:
        DBG_I(DSLAVE_MS_USB, ("MASS: Get max LUNs request.\n"));

        if (req->wValue || req->wLength != 1) /* HSU fix */
        {
            DBG_E(DSLAVE_MS_USB, ("MASS: Invalid MS_GET_MAX_LUN request.\n"));
            break;
        }
        /* HSU addition: CR 159898 */
        /* Clear Stalls from pipes */
        core_abort_pipe(fd->core_ctx, fd->in_pipe);
        core_stall_pipe(fd->core_ctx, fd->in_pipe, 0);

        core_abort_pipe(fd->core_ctx, fd->out_pipe);
        core_stall_pipe(fd->core_ctx, fd->out_pipe, 0);

        rc = post_read(fd, fd->cmd_request, USB_BULK_CB_WRAP_LEN, 
            cbw_read_complete, fd);
        if (rc)
        {
            DBG_E(DSLAVE_MS_USB, ("Failed to post CBW on GET_MAX_LUN_REQUEST (%d)\n", rc));
        }
        /******* End HSU addition **************************/

        ((juint8_t *)(fd->ep0_request->buffer.vaddr))[0] = fd->total_luns - 1;
        rc = send_ep0_reply(fd, 1);
        break;
        
    default:
        DBG_W(DSLAVE_MS_USB, ("MASS: Unknown EP0 request.\n"));
    }

    return rc;
}

/**
 * Function name:  get_pipe_config
 * Description:    Returns the mass storage function driver's USB descriptors.
 * Parameters: 
 *     @context: (IN) FD context
 *
 * Return value: FD descriptor
 * Scope:        Local
 **/
static fd_desc_t *get_pipe_config(context_t context)
{
    fd_storage_t *fd = (fd_storage_t *)context;

    return fd->fd_desc;
}

#define ALLOC_MSFD_REQUEST(i) \
    do { \
        rc = jdma_alloc(fd->scsi_buffer_size, 32, &(fd->io_buffer[i].vaddr), \
            &(fd->io_buffer[i].dma_addr), M_CACHABLE, \
            &(fd->io_buffer[i].cookie)); \
        if (rc) \
        { \
            DBG_E(DSLAVE_MS_USB, ("enable: Failed to allocate SCSI " \
                "buffers\n")); \
            goto Exit; \
        } \
        fd->io_request[i] = core_request_alloc(fd->scsi_buffer_size, \
            fd->io_buffer[i].vaddr); \
        if (!fd->io_request[i]) \
        { \
            DBG_E(DSLAVE_MS_USB, ("enable: Failed to allocate SCSI " \
                "request\n")); \
            rc = JENOMEM; \
            goto Exit; \
        } \
    } while(0)

#define FREE_MSFD_REQUEST(i) \
    do { \
        if (fd->io_buffer[i].cookie) \
        { \
            jdma_free(fd->io_buffer[i].cookie); \
            fd->io_buffer[i].cookie = NULL; \
            if (fd->io_request[i]) \
            { \
                core_request_free(fd->io_request[i]); \
                fd->io_request[i] = NULL; \
            } \
        } \
    } while (0)

/**
 * Function name:  enable
 * Description:    Enables the mass storage function driver.
 * Parameters: 
 *     @context: (IN) FD context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Local
 **/
static jresult_t enable(context_t context)
{
    jresult_t rc = 0;
    jint_t i;
    fd_storage_t *fd = (fd_storage_t *)context;

    if (fd->state == STATE_ENABLED)
    {
        DBG_W(DSLAVE_MS_USB, ("enable: Trying to enable already enabled FD\n"));
        goto Exit;
    }

    for (i = 0; i < SCSI_TRANSFER_BUFFERS; i++)
        ALLOC_MSFD_REQUEST(i);

    /* Get the pipe numbers that the Core assigned to our descriptors */
    DBG_I(DSLAVE_MS_USB, ("FD: Pipe %p is %d.\n",
        &fd->fd_desc->interfaces[0].alt_ifs[0].pipes[0],
        fd->fd_desc->interfaces[0].alt_ifs[0].pipes[0].address));

    fd->out_pipe = &fd->fd_desc->interfaces[0].alt_ifs[0].pipes[0];
    fd->in_pipe  = &fd->fd_desc->interfaces[0].alt_ifs[0].pipes[1];

    rc = scsi_enable(fd->scsi_device);
    if (rc)
        goto Exit;

    rc = post_read(fd, fd->cmd_request, USB_BULK_CB_WRAP_LEN,
        cbw_read_complete, fd);
    if (rc)
    /* HSU addition: CR 159898 */
    {
        DBG_E(DSLAVE_MS_USB, ("Failed to post CBW on enable (%d)\n", rc));
        goto Exit;
    }
    /******* End HSU addition **************************/

    fd->state = STATE_ENABLED;

Exit:
    if (rc)
    {
        for (i = 0; i < SCSI_TRANSFER_BUFFERS; i++)
            FREE_MSFD_REQUEST(i);
    }

    return rc;
}

/**
 * Function name:  disable
 * Description:    Disables the mass storage function driver.
 * Parameters: 
 *     @context: (IN) FD context
 *
 * Return value: None
 * Scope:        Local
 **/
static void disable(void *context)
{
    fd_storage_t *fd = (fd_storage_t *)context;
    jint_t i;

    if (fd->state == STATE_DISABLED)
        return;

    fd->state = STATE_DISABLED;

    /* HSU addition: prevent crash on bus reset */
    scsi_disable(fd->scsi_device);
    /* End HSU addition */

    /* Abort pending requests */
    if (fd->in_pipe)
    {
        core_abort_pipe(fd->core_ctx, fd->in_pipe);
        fd->in_pipe = NULL;
    }
    
    if (fd->out_pipe)
    {
        core_abort_pipe(fd->core_ctx, fd->out_pipe);
        fd->out_pipe = NULL;
    }

    clear_cmd_in_progress(fd);

    /* HSU deletion: moved up
    scsi_disable(fd->scsi_device);
    End HSU deletion */

    for (i = 0; i < SCSI_TRANSFER_BUFFERS; i++)
        FREE_MSFD_REQUEST(i);
}

/**
 * Function name:  suspend
 * Description: Called when bus SUSPEND received from the host. Disable the 
 *              function driver.
 * Parameters: 
 *     @context: (IN) FD context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t suspend(void *context)
{
    fd_storage_t *fd = (fd_storage_t *)context;

    fd->prev_state = fd->state;
    if (fd->state == STATE_ENABLED)
        disable(context);

    return 0;
}

static jresult_t resume(void *context)
{
    jresult_t rc = 0;
    fd_storage_t *fd = (fd_storage_t *)context;

    if (fd->prev_state == STATE_ENABLED)
    {
        rc = enable(context);
        if (rc)
            DBG_E(DSLAVE_MS_USB, ("resume: Failed to resume FD (%d)\n", rc));
    }
    return rc;
}

/**
 * Function name:  uninit
 * Description:    Un-initializes the mass storage function driver.
 * Parameters: 
 *     @context: (IN) FD context  
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Local
 **/
static jresult_t uninit(context_t context)
{
    fd_storage_t *fd = (fd_storage_t *)context;

    disable(fd);

    scsi_uninit(fd->scsi_device);

    if (fd->ep0_request)
        core_request_free(fd->ep0_request);

    if (fd->cmd_request)
        core_request_free(fd->cmd_request);

    /* Free FD Descriptor */
    jfree(fd->fd_desc->interfaces->alt_ifs->pipes);
    jfree(fd->fd_desc->interfaces->alt_ifs);
    jfree(fd->fd_desc->interfaces);
    jfree(fd->fd_desc);

    jfree(fd);

    return 0;
}

static fd_ops_t fd_ops = {
/*  get_pipe_config     */ get_pipe_config,
/*  enable              */ enable,
/*  disable             */ disable,      
/*  suspend             */ suspend,
/*  resume              */ resume,
/*  uninit              */ uninit
};

/**
 * Function name:  copy_fd_desc
 * Description:    Allocate all FD descriptors and copies them into
 *                 one structure.
 * Parameters: 
 *
 * Return value: FD descriptor
 * Scope:        Local
 **/
static fd_desc_t *copy_fd_desc(void)
{
    pipe_desc_t         *pipe = NULL;
    alt_interface_desc_t *alt = NULL;
    interface_desc_t    *iface = NULL;
    fd_desc_t           *desc = NULL;
    
    pipe = jmalloc(sizeof(pipe_desc_array), 0);
    if (!pipe)
        goto Error;

    j_memcpy(pipe, pipe_desc_array, sizeof(pipe_desc_array));

    alt = jmalloc(sizeof(alt_desc_array), 0);
    if (!alt)
        goto Error;

    j_memcpy(alt, alt_desc_array, sizeof(alt_desc_array));

    iface = jmalloc(sizeof(if_desc_array), 0);
    if (!iface)
        goto Error;

    j_memcpy(iface, if_desc_array, sizeof(if_desc_array));
    
    desc = jmalloc(sizeof(fd_desc), 0);
    if (!desc)
        goto Error;

    j_memcpy(desc, &fd_desc, sizeof(fd_desc));
    
    alt->pipes = pipe;
    iface->alt_ifs = alt;
    desc->interfaces = iface;

    return desc;

Error:
    if (pipe)
        jfree(pipe);

    if (alt)
        jfree(alt);

    if (iface)
        jfree(iface);

    if (desc)
        jfree(desc);

    return NULL;
}

/**
 * Function name:  mass_storage_init
 * Description:    Initializes the mass storage function driver.
 * Parameters: 
 *     @ctx: (IN) Core context
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope:        Global
 **/
jresult_t mass_storage_init(void *ctx)
{
    jresult_t rc;
    fd_storage_t *fd;

    fd = jmalloc(sizeof(fd_storage_t), M_ZERO);
    if (!fd)
        return JENOMEM;

    fd->core_ctx = ctx;
    fd->state = STATE_DISABLED;
    
    fd->fd_desc = copy_fd_desc();
    if (!fd->fd_desc)
    {
        jfree(fd);
        return JENOMEM;
    }

    /* Allocate requests */                    
    fd->ep0_request = core_request_alloc(1, NULL);
    if (!fd->ep0_request)
    {
        DBG_E(DSLAVE_MS_USB, ("mass_storage_init: Failed to allocate ep0 "
            "request\n"));
        return JENOMEM;
    }

    fd->cmd_request = core_request_alloc(SCSI_MAX_REQUEST_SIZE, NULL);
    if (!fd->cmd_request)
    {
        DBG_E(DSLAVE_MS_USB, ("mass_storage_init: Failed to allocate cmd "
            "request\n"));
        return JENOMEM;
    }

    /* The completion callback of ep0 is always the same */
    fd->ep0_request->complete = ep0_callback;
    fd->ep0_request->context = (void *)fd;

    rc = scsi_init(fd, &fd->total_luns, &fd->scsi_device);
    if (rc)
        return rc;

    return core_register_fd(fd->core_ctx, &fd_ops, (context_t)fd);
}

