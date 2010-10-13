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
#ifndef _UW_DEVICE_MASS_H_
#define _UW_DEVICE_MASS_H_

#include "jtypes.h"

typedef void *msfd_appctx_t;

typedef struct {
    juint8_t interface_number;
    juint32_t scsi_buffer_size; /* Must be aligned with the maximum possible
                                  supported block size */
} msfd_init_info_t;

/**
 * Type name:      completion_cb
 * Description:    Mass Storage Function Driver callback function type.
 *                 A function of this type is passed as the callback
 *                 parameter to vopen() and is called by the application
 *                 when an I/O or flush request has been completed.
 * Parameters: 
 *      @ context: (IN) Private mass storage function driver context
 *      @ status:  (IN) The status of the request:
 *                      TRUE if the request was processed successfully;
 *                      FALSE otherwise
 *
 * Return value: None
 * Scope: Local
 **/
typedef void (*completion_callback)(void *context, jbool_t status);


/*****************************************************************************\
 * Structure of customer callback functions required by the Mass Storage
 * Function Driver.
\*****************************************************************************/
typedef struct { 
    /**
    * Function name:  vget_desc
    * Description:    Returns a description of a virtual logical unit. 
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:      (IN)  Application context as returned by msfd_app_init()
    *     @vlun:     (IN)  Virtual logical unit index
    *     @dev_type: (OUT) Pointer to SCSI Device type index
    *     @vendor:   (OUT) Pointer to an 8 characters vendor ID string
    *     @product:  (OUT) Pointer to a 16 characters product ID string
    *     @revision: (OUT) Pointer to the device's revision version
    *
    * Return value: None
    * Scope: Local
    **/
void (*vget_desc)(msfd_appctx_t ctx, juint8_t vlun, juint32_t *dev_type, 
        /* HSU temp fix B56842 */ juint8_t *version,
        char **vendor, char **product, char **revision);
    /**
    * Function name:  vready
    * Description:    Checks if a virtual logical unit is ready for IO access.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:  (IN) Application context as returned by msfd_app_init()
    *     @vlun: (IN) Virtual logical unit index
    *
    * Return value: TRUE if the logical unit is ready for I/O;
    *               FALSE otherwise
    * Scope: Local
    **/
    jbool_t (*vready)(msfd_appctx_t ctx, juint8_t vlun);

    /**
    * Function name:  vchanged
    * Description:    Checks if a virtual logical unit's media has changed.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:  (IN) Application context as returned by msfd_app_init()
    *     @vlun: (IN) Virtual logical unit index
    *
    * Return value: TRUE if the logical unit's media has changed;
    *               FALSE otherwise
    * Scope: Local
    **/
    jbool_t (*vchanged)(msfd_appctx_t ctx, juint8_t vlun);

    /**
    * Function name:  vmount
    * Description:    Attempts to initiate a Mount (Load) operation
    *                 (if supported).
    *                 The function must start the mount and return immediately.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:  (IN) Application context as returned by msfd_app_init()
    *     @vlun: (IN) Virtual logical unit index
    *
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*vmount)(msfd_appctx_t ctx, juint8_t vlun);

    /**
    * Function name:  vunmount
    * Description:    Attempts to initiate an Un-Mount (Eject) operation
    *                 (if supported).
    *                 The function must start the un-mount and return
    *                 immediately.
    * 
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:  (IN) Application context as returned by msfd_app_init()
    *     @vlun: (IN) Virtual logical unit index
    *
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*vunmount)(msfd_appctx_t ctx, juint8_t vlun);

    /**
    * Function name:  vlun_count
    * Description:    Returns the number of virtual logical units supported
    *                 by the device.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:   (IN)  Application context as returned by msfd_app_init()
    *     @count: (OUT) Pointer to the number of virtual logical units
    *                   supported by the device
    *
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*vlun_count)(msfd_appctx_t ctx, juint8_t *count);

    /**
    * Function name:  vget_media_details
    * Description:    Returns the details of the media inserted into a
    *                 virtual logical unit.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:        (IN)  Application context as returned by msfd_app_init()
    *     @vlun:       (IN)  Virtual logical unit index
    *     @block_size: (OUT) Pointer to size of block, in bytes
    *                        (e.g. 512 / 1024 / 2048 / etc.)
    *     @num_blocks: (OUT) Pointer to size of the virtual logical unit,
    *                        in blocks
    *     @read_only:  (OUT) Pointer to a boolean value:
    *                        TRUE for a read-only virtual logical unit;
    *                        False otherwise
    *
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*vget_media_details)(msfd_appctx_t ctx, juint8_t vlun, 
        juint32_t *block_size, juint32_t *num_blocks, juint32_t *read_only);

    /**
    * Function name:  vopen
    * Description:    Opens a virtual logical unit for exclusive access.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:       (IN) Application context as returned by msfd_app_init()
    *     @vlun:      (IN) Virtual logical unit index
    *     @removable: (OUT) Pointer to a boolean value:
    *                       TRUE if the media on the virtual logical unit is
    *                       removable;
    *                       FALSE otherwise
    *     @callback:  (IN) Mass Storage Function Driver callback to be called
    *                      when a read, write or flush operation is completed
    *     @arg:       (IN) Mass Storage Function Driver context argument to be
    *                      passed to the callback
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*vopen)(msfd_appctx_t ctx, juint8_t vlun,juint8_t *removable,
        completion_callback callback, void *arg);

    /**
    * Function name:  vlcose
    * Description:    Closes a virtual logical unit.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:  (IN) Application context as returned by msfd_app_init()
    *     @vlun: (IN) Virtual logical unit index
    *
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*vclose)(msfd_appctx_t ctx, juint8_t vlun);

    /**
    * Function name:  vread
    * Description:    Reads a block of data from a virtual logical unit.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:    (IN)  Application context as returned by msfd_app_init()
    *     @vlun:   (IN)  Virtual logical unit index
    *     @buffer: (OUT) Pointer to a buffer to be filled with the read data
    *     @amount: (IN)  Number of blocks to read
    *     @offset: (IN)  Zero-based offset (in blocks) within the virtual
    *                    logical unit from which to read
    *
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*vread)(msfd_appctx_t ctx, juint8_t vlun, juint8_t *buffer, 
        juint32_t amount, juint32_t offset);

    /**
    * Function name:  vwrite
    * Description:    Writes a block of data to a virtual logical unit.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:    (IN)  Application context as returned by msfd_app_init()
    *     @vlun:   (IN)  Virtual logical unit index
    *     @buffer: (OUT) Pointer to a buffer containing the data to write
    *     @amount: (IN)  Number of blocks to write
    *     @offset: (IN)  Zero-based offset (in blocks) within the virtual
    *                    logical unit to write to
    *
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*vwrite)(msfd_appctx_t ctx, juint8_t vlun, juint8_t *buffer,
        juint32_t amount, juint32_t offset);

    /**
    * Function name:  vverify
    * Description:    Verifies that a block of data can be read.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:    (IN) Application context as returned by msfd_app_init()
    *     @vlun:   (IN) Virtual logical unit index
    *     @amount: (IN) Number of blocks to read
    *     @offset: (IN) Zero-based offset (in blocks) within the virtual
    *                   logical unit from which to read
    *
    * Return value: The number of blocks that could be read successfully;
    *               Error code on failure
    * Scope: Local
    **/
    jint32_t (*vverify)(msfd_appctx_t ctx, juint8_t vlun, juint32_t amount, 
        juint32_t offset);

    /**
    * Function name:  vcancel
    * Description:    Cancels the current I/O request for a
    *                 virtual logical unit.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:  (IN) Application context as returned by msfd_app_init()
    *     @vlun: (IN) Virtual logical unit index
    *
    * Return value: None
    * Scope: Local
    **/
    void (*vcancel)(msfd_appctx_t ctx, juint8_t vlun);

    /**
    * Function name:  vflush
    * Description:    Flushes all memory cache to persistent memory.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:  (IN) Application context as returned by msfd_app_init()
    *     @vlun: (IN) Virtual logical unit index
    *
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*vflush)(msfd_appctx_t ctx, juint8_t vlun);

#define ALLOW_CACHE     1
#define DISALLOW_CACHE  0
    /**
    * Function name:  vallow_cache
    * Description:    Sets caching on/off.
    * Requirement:    Mandatory
    * Parameters:
    *     @ctx:  (IN) Application context as returned by msfd_app_init()
    *     @vlun: (IN) Virtual logical unit index
    *     @flag: (IN) Cache flag:
    *                 ALLOW_CACHE:    Sets cache on
    *                 DISALLOW_CACHE: Sets cache off
    *
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*vallow_cache)(msfd_appctx_t ctx, juint8_t vlun, juint8_t flag);
   
#ifdef CONFIG_SCSI_TRANSPARENT_COMMAND
    /**
    * Function name:  vtransparent
    * Description:    Passes a vendor-specific ("transparent") SCSI command
    *                 from the host to the application.
    * Requirement:    Optional
    *                 If the application supports vendor-specific requests it
    *                 can configure the device stack with the
    *                 CONFIG_SCSI_TRANSPARENT_COMMAND flag, in which case the
    *                 vtransparent callback must be implemented.
    *                 Otherwise, this callback field is not defined and the mass
    *                 storage function driver ignores vendor-specific requests.
    * Parameters:
    *     @ctx:  (IN) Application context as returned by msfd_app_init()
    *     @vlun: (IN) Virtual logical unit index
    *     @cmd:  (IN) SCSI command
    *
    * Return value: 0 on success; Sense data on failure
    * Scope: Local
    **/
    jresult_t (*vtransparent)(msfd_appctx_t ctx, juint8_t vlun, void *cmd);
#endif

    /* HSU addition: SCSI commands hook */

    /**
    * Function name:  scsi_in_handler
    * Description:    Application handler for IN SCSI command. This cb is also called 
    *                 for commands with dCBWDataTransferLength = 0. For such commands
    *                 the direction is ignored.
    * Requirement:    Optional
    * Parameters:
    *     @ctx:           (IN) Application context as returned by msfd_app_init()
    *     @vlun:          (IN) Virtual logical unit index
    *     @command        (IN) SCSI command
    *     @size           (IN) size of SCSI command
    *     @data_size      (IN) max size for IN data
    *     @buffer         (OUT) Pointer to the a buffer to be filled with the IN data
    *     @buf_size       (IN) size of IN data buffer
    *     @reply_len      (OUT) number of bytes to send to host as IN data.
    *     @res            (OUT) result of the handled command
    *     @is_req_handled (IN) is TRUE if this request is handled in the USB stack
    *
    * Return value: TRUE - if the command was handled
                    FALSE - otherwise
    * Scope: Local
    **/
    jbool_t (*scsi_in_handler)(msfd_appctx_t ctx, juint8_t vlun,
      const juint8_t *command, juint32_t size, juint32_t data_size, juint8_t* buffer,
      juint32_t buf_size, juint32_t* reply_len, jresult_t *res, jbool_t is_req_handled);

    /**
    * Function name:  scsi_out_handler
    * Description:    Application handler for OUT SCSI command. 
    * Requirement:    Optional
    * Parameters:
    *     @ctx:           (IN) Application context as returned by msfd_app_init()
    *     @vlun:          (IN) Virtual logical unit index
    *     @command        (IN) SCSI command
    *     @size           (IN) size of SCSI command
    *     @data_size      (IN) size of the data to be received in the DATA stage
    *     @res            (OUT) result of the handled command
    *     @is_req_handled (IN) is TRUE if this request is handled in the USB stack
    *
    * Return value: TRUE - if the command is handled by the application
    *               FALSE - otherwise
    * Scope: Local
    **/
    jbool_t (*scsi_out_handler)(msfd_appctx_t ctx, juint8_t vlun, const juint8_t *command, 
      juint32_t size, juint32_t data_size, jresult_t *res, jbool_t is_req_handled);

    /**
    * Function name:  scsi_out_data_stage_handler
    * Description:    Application cb to be called upon receiving data for SCSI 
    *                 OUT commands.
    *                 This cb will be called only if the application returned 
    *                 true in scsi_out_handler(). 
    * Requirement:    Optional
    * Parameters:
    *     @ctx:    (IN)  Application context as returned by msfd_app_init()
    *     @vlun:   (IN)  Virtual logical unit index
    *     @buffer: (IN) Pointer to a buffer containing the received data 
    *     @amount: (IN)  Number of bytes received
    *     @res     (OUT) result of the handled command 
    *
    * Return value: None
    * Scope: Local
    **/
    void (*scsi_out_data_stage_handler)(msfd_appctx_t ctx, juint8_t vlun, 
      juint8_t *buffer, juint32_t amount,jresult_t *res);

    /**
    * Function name:  handled_cmd_ind
    * Description:    Application indication about a SCSI command which was
    *                 handled by the MS FD.
    *    
    *                 Note: This function is obsolete. 
    *                 Please use scsi_in_handler() and scsi_out_handler()
    *
    * Requirement:    Optional
    * Parameters:
    *     @ctx:      (IN) Application context as returned by msfd_app_init()
    *     @vlun:     (IN) Virtual logical unit index
    *     @command   (IN) SCSI command
    *     @size      (IN) size of SCSI command
    *
    * Return value: None
    * Scope: Local
    **/
    void (*handled_cmd_ind)(msfd_appctx_t ctx, juint8_t vlun,
      const juint8_t *command, juint32_t size);

    /**
    * Function name:  unhandled_cmd_in
    * Description:    Application callback for IN SCSI command which was
    *                 not handled by the MS FD.    
    *
    *                 Note: This function is obsolete. Please use scsi_in_handler()
    *
    * Requirement:    Optional
    * Parameters:
    *     @ctx:      (IN) Application context as returned by msfd_app_init()
    *     @vlun:     (IN) Virtual logical unit index
    *     @command   (IN) SCSI command
    *     @size      (IN) size of SCSI command
    *     @data_size (IN) max size for IN data
    *     @buffer    (OUT) Pointer to the a buffer to be filled with the IN data
    *     @buf_size  (IN) size of IN data buffer
    *     @reply_len (OUT) number of bytes to send to host as IN data.
    *
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*unhandled_cmd_in)(msfd_appctx_t ctx, juint8_t vlun,
      const juint8_t *command, juint32_t size, juint32_t data_size, juint8_t* buffer,
      juint32_t buf_size, juint32_t* reply_len);

    /* HSU addition end: SCSI commands hook */

    /**
    * Function name:  msfd_app_uninit
    * Description:    Application un-initialization function.
    * Requirement:    Optional
    * Parameters:
    *     @ctx:  (IN) Application context as returned by msfd_app_init()
    *
    * Return value: 0 on success, otherwise an error code
    * Scope: Local
    **/
    jresult_t (*msfd_app_uninit)(msfd_appctx_t ctx);

} msfd_ops_t;

/**
* Function name:  msfd_app_init
* Description:    Application initialization function.
*                 This function is called from the scsi_init() function
*                 during the Mass Storage Function Driver's initialization.
* Requirement:    Mandatory
* Parameters:
*     @msfd_ops: (OUT)  Pointer to the application's mass storage callback
*                       functions.
*     @ctx:      (OUT): Pointer to the application's context
*     @info:     (OUT): Pointer to info struct, the application can set 
*                       interface number using the struct
*
* Return value: 0 on success, otherwise an error code
* Scope: Global
**/
jresult_t msfd_app_init(msfd_ops_t *ops, msfd_appctx_t *ctx, 
    msfd_init_info_t *info);
#endif

