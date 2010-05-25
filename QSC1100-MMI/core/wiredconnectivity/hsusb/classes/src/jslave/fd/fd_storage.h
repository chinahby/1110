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
#ifndef _FD_STORAGE_H_
#define _FD_STORAGE_H_

#include <jusb_common.h>
#include <jusb_core_fd.h>

/* Size of SCSI buffer */
#define SCSI_BUFFER_SIZE (512 * 64) 

/* Size of SCSI command */
#define MAX_COMMAND_SIZE        16

/* Maximum size of command's data stage (not read/write) */
#define SCSI_MAX_REQUEST_SIZE   200

/* Number of transfer buffers */
#define SCSI_TRANSFER_BUFFERS   2

/* Mass storage command status codes */
#define USB_STATUS_PASS         0
#define USB_STATUS_FAIL         1
#define USB_STATUS_PHASE_ERROR  2

/* Mass storage function driver (FD) states */
typedef enum {
    STATE_DISABLED = 0,
    STATE_ENABLED  = 1
} mass_states_t;

/* Main mass storage FD structure */
typedef struct {
    /* Core context */
    void *core_ctx;

    /* IN/OUT pipes */
    pipe_desc_t *in_pipe;
    pipe_desc_t *out_pipe;
    
    /* Current state */
    mass_states_t state;

    /* State before suspend */
    mass_states_t prev_state;

    /* Total LUNs, as reported by scsi_init */
    juint8_t     total_luns;
    
    /* Double buffer for data I/O */
    juint32_t scsi_buffer_size;
    buffer_t io_buffer[SCSI_TRANSFER_BUFFERS];
    request_t *io_request[SCSI_TRANSFER_BUFFERS];

    /* Mass command processing */
    request_t *cmd_request;

    /* Buffer for EP0 requests processing */
    request_t *ep0_request;

    /* True if there is currently a command being processed.
     * We accept CBW only when there is no other command. */
    jbool_t current_cmd_status;

    /* Pointer to the SCSI device */
    void *scsi_device;

    /* Descriptor for this FD */
    fd_desc_t *fd_desc;
} fd_storage_t;

/******************
 * USB Functions
 * ****************/

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
void send_csw(fd_storage_t *fd, juint32_t tag, juint32_t residue, juint8_t status);

/**
 * Function name:  post_write
 * Description:    Sends data to the host.
 * Parameters: 
 *     @fd:       (IN) FD context 
 *     @req:      (IN) Request to perform write on
 *     @size:     (IN) Transfer size, in bytes
 *     @callback: (IN) Completion callback
 *     @context:  (IN) Context for the completion callback
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Global
 **/
jresult_t post_write(fd_storage_t *fd, request_t *req, juint32_t size, 
    callback_t callback, context_t context);
/**
 * Function name:  post_read
 * Description:    Sends a read request to the host.
 * Parameters: 
 *     @fd:       (IN) FD context
 *     @req:      (IN) Request to perform write on
 *     @size:     (IN) Transfer size, in bytes
 *     @callback: (IN) Completion callback
 *     @context:  (IN) Context for the completion callback
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Global
 **/
jresult_t post_read(fd_storage_t *fd, request_t *req, juint32_t size,
    callback_t callback, context_t context);

/**
 * Function name:  stop_io
 * Description:    Cancels the current USB I/O request.
 * Parameters: 
 *     @fd:       (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **/
void stop_io(fd_storage_t *fd);

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
void fatal_processing_error(fd_storage_t *fd);

/**
 * Function name:  abort_out_pipe
 * Description:    Aborts pending read requests.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **/
void abort_out_pipe(fd_storage_t *fd);

/**
 * Function name:  stall_in_pipe
 * Description:    Stalls the IN data pipe.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **/
void stall_in_pipe(fd_storage_t *fd);

/**
 * Function name:  stall_out_pipe
 * Description:    Stalls the OUT data pipe.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Global
 **/
void stall_out_pipe(fd_storage_t *fd);

/**
 * Function name:  clear_cmd_in_progress
 * Description:    Returns the command's current status.
 *                 Used to check whether a command is still in progress.
 * Parameters: 
 *     @fd: (IN) FD context
 *
 * Return value: None
 * Scope:        Local
 **/
static __INLINE__ jbool_t command_in_progress(fd_storage_t *fd)
{
    return fd->current_cmd_status;
}


/******************
 * SCSI Functions
 * ****************/

/**
 * Function name:  do_scsi_command
 * Description:    Handles a SCSI command received from the host.
 * Parameters: 
 *     @scsi_dev:  (IN) SCSI device context 
 *     @command:   (IN) SCSI command buffer
 *     @size:      (IN) Command size, in bytes
 *     @data_size: (IN) Size of data (in bytes) for the command's data stage
 *     @tag:       (IN) Tag for matching the CSW to the original CBW tag
 *     @lun:       (IN) LUN number
 *     @direction  (IN) Transfer direction: 0 = OUT / 1 = IN
 *     @buffer:    (IN) Buffer to be used for USB data transfers
 *
 * Return value: None
 * Scope:        Global
 **/
void do_scsi_command(void *scsi_dev, juint8_t *command, juint32_t size,
    juint32_t data_size, juint32_t tag, juint8_t lun,
    jbool_t direction, juint8_t *buffer);

/**
 * Function name:  scsi_init
 * Description:    Initializes the SCSI layer.
 * Parameters: 
 *     @fd:      (IN)  FD context
 *     @luns:    (OUT) Number of LUNs
 *     @context: (OUT) SCSI device context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Global
 **/
jresult_t scsi_init(fd_storage_t *fd, juint8_t *luns, void **scsi_dev);
/**
 * Function name:  scsi_uninit
 * Description:    Un-initializes the SCSI layer.
 * Parameters: 
 *     @scsi_dev: (IN) SCSI device context
 *
 * Return value: None
 * Scope:        Global
 **/
void scsi_uninit(void *scsi_dev);

/**
 * Function name:  scsi_enable
 * Description:    Enables all LUNs on the specified SCSI device.
 * Parameters: 
 *     @scsi_dev: (IN) SCSI device context 
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:        Global
 **/
jresult_t scsi_enable(void *scsi_dev);
/**
 * Function name:  scsi_disable
 * Description:    Disables all LUNs on the specified SCSI device.
 * Parameters: 
 *     @scsi_dev:  (IN) SCSI device context
 *
 * Return value: None
 * Scope:        Global
 **/
void scsi_disable(void *scsi_dev);


#endif

