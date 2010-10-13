#ifndef _OI_OPP_SYS_H
#define _OI_OPP_SYS_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_opp_sys.h#1 $ 

$Log: $
*
*    #3       18 Oct  2006           PR
*  Merged OI Code v3.0.14rev1revQ
*
===========================================================================*/
/**
 @file  

 object system abstraction layer used by the the Object Push Profile client and
 server

 This interface is described entirely as callback functions. The application
 must provide the sets of functions to the OPP client and OPP server
 when initializing these two services.

 Callbacks and errors are mutually exclusive.  
    If a function below returns OI_OK, the callback must be called, 
    either from within the function or on a later thread of execution.
    
    If a function below returns anything other than OI_OK, the callback
    must NOT be called.

 Note that if you wish your application to be
 BQB-compliant, you should generate error codes consistent with
 BLUEmagic 3.0 best practices; see the sample code for examples.

*/

/** \addtogroup OPP OPP APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_status.h"
#include "oi_stddefs.h"
#include "oi_obexspec.h"

/**
 * This type represents an opaque object that uniquely identifies an object that
 * has been opened for either reading or writing.
 */
typedef void* OI_OPP_HANDLE;


/**
 * This type represents a connection id for an OPP client or server connection
 * that is making calls into the object system abstraction layer.
 */
typedef OI_UINT16 OI_OPP_CONNECTION;


/* ********************************************************************************
 * The object functions are all asynchronous. However implementations are
 * permitted to call the callback function from within the function if the
 * operation can be completed quickly. This will be the case for many
 * implementations.
 * ********************************************************************************/



/**
 * A function of this type is called to confirm the success or failure opening
 * an object for reading
 *
 * @param objHandle   A handle that can be used to read the object contents.
 *
 * @param name        A null-terminated unicode string name of the object that was opened.
 *
 * @param type        A null-terminated ascii string for the object type. For example
 *                    for a vCard the type string is "text/x-vCard"
 *
 * @param size        If the object was opened for reading, size is the number
 *                    of bytes that can be read from the object. If the object
 *                    was opened for writing the value is undefined.
 *
 * @param status      OI_OK if the object could be opened, an error if the object
 *                    could not be opened.
 *
 * @param oppConnection identifies which OPP client or server connection is 
 *                   performing this operation.
 */
typedef void (*OI_OPP_OPEN_READ_CFM)(OI_OPP_HANDLE objHandle,
                                     const OI_OBEX_UNICODE *name,
                                     const OI_CHAR *type,
                                     OI_UINT32 size,
                                     OI_STATUS status,
                                     OI_OPP_CONNECTION oppConnection);

/**
 * This function and corresponding callback is needed by an OPP client and server.
 *
 * @param name           a null-terminated unicode string name of the object to
 *                       be opened. A NULL name means open the default object.
 *                       The default object is the owner's business card.
 *
 * @param type           a null-terminated ascii string for the object type.
 *
 * @param openCfm        the function that will be called when the open completes.
 *
 * @param oppConnection  identifies the OPP client or server connection that is 
 *                       performing this operation.
 */
typedef OI_STATUS (*OI_OPP_OPEN_READ)(const OI_OBEX_UNICODE *name,
                                      const OI_CHAR *type,
                                      OI_OPP_OPEN_READ_CFM openCfm,
                                      OI_OPP_CONNECTION oppConnection);


/**
 * A function of this type is called to confirm the success or failure opening
 * an object for writing.
 *
 * @param objHandle      A handle that can be used to write the object contents.
 *
 * @param status         OI_OK if the object could be opened to write or
 *                       an error if the object could not be opened.
 *
 * @param oppConnection  identifies the OPP client or server connection that is 
 *                       performing this operation.
 */
typedef void (*OI_OPP_OPEN_WRITE_CFM)(OI_OPP_HANDLE objHandle,
                                      OI_STATUS status,
                                      OI_OPP_CONNECTION oppConnection);

/**
 * This function and corresponding callback is needed by an OPP client and server.
 *
 * @param name           a NULL terminated unicode string name of the object to
 *                       be opened. A NULL name means that the remote OPP device
 *                       did not provide a file name for the object.  (This can
 *                       happen when pulling a default object from a device that
 *                       follows the IrDA OBEX spec over the Bluetooth OPP spec.)  
 *
 * @param type           a NULL terminated ascii string for the object type.
 *
 * @param objSize        a hint about the size of the object.
 *
 * @param data           This is always NULL 
 *
 * @param len            This is always 0
 *
 * @param openCfm        the function to be called when the open completes.
 *
 * @param oppConnection  identifies the OPP client or server connection that is 
 *                       performing this operation.
 */
typedef OI_STATUS (*OI_OPP_OPEN_WRITE)(const OI_OBEX_UNICODE *name,
                                       const OI_CHAR *type,
                                       OI_UINT32 objSize,
                                       OI_OPP_OPEN_WRITE_CFM openCfm,
                                       OI_OPP_CONNECTION oppConnection);

/**
 * This function does not use a callback function. It is assumed that object
 * close will complete asynchronously or in the case of a failure will report or
 * log an error with the application.  
 *
 * @param handle         A handle for an open object.
 *
 * @param status         OI_OK or an error status if the object is being closed
 *                       because of an error. In this case if the object was
 *                       being written the state of the object is unknown and
 *                       the application may choose to delete the object.
 *
 * @param oppConnection  identifies the OPP client or server connection that is 
 *                       performing this operation.
 */
typedef void (*OI_OPP_CLOSE)(OI_OPP_HANDLE objHandle,
                             OI_STATUS status,
                             OI_OPP_CONNECTION oppConnection);

/**
 * A function of this type is called to confirm the success or failure of a
 * object read.
 *
 * @param objHandle      handle passed to the read call.
 *
 * @param data           a pointer to a buffer containing the data read.
 *
 * @param len            the number of bytes read.
 *
 * @param status         a status code indicating if the read was succesful:
 *                       - OI_OK if the read completed
 *                       - OI_STATUS_END_OF_FILE if the read was successful and
 *                         the end of object has been reached.
 *                       - An error status indicating that the read failed.
 *
 * @param oppConnection  identifies the OPP client or server connection that is 
 *                       performing this operation.
 *
 */
typedef void (*OI_OPP_READ_CFM)(OI_OPP_HANDLE objHandle,
                                OI_BYTE *data,
                                OI_UINT16 len,
                                OI_STATUS status,
                                OI_OPP_CONNECTION oppConnection);

/**
 * This function and corresponding callback is needed by an OPP client that
 * supports object push and and opp server that supports object pull.
 *
 * @param objHandle   a handle previously returned by an OI_OPP_OPEN_CFM function
 *
 * @param maxRead     the maximum number of bytes to read from the object on this
 *                    call.
 *
 * @param readCfm     the function that will be called when the read completes.
 *
 * @param oppConnection  identifies the OPP client or server connection that is 
 *                    performing this operation.
 */
typedef OI_STATUS (*OI_OPP_READ)(OI_OPP_HANDLE objHandle,
                                 OI_UINT16 maxRead,
                                 OI_OPP_READ_CFM readCfm,
                                 OI_OPP_CONNECTION oppConnection);


/**
 * A function of this type is called to confirm the success or failure of a
 * object write operation.
 *
 * @param objHandle      handle passed to the write call.
 *
 * @param status         Indicates the success or failure of the write operation.
 *
 * @param oppConnection  identifies which OPP client or server connection is 
 *                       performing this operation.
 */
typedef void (*OI_OPP_WRITE_CFM)(OI_OPP_HANDLE objHandle,
                                 OI_STATUS status,
                                 OI_OPP_CONNECTION oppConnection);


/**
 * This function and corresponding callback is only required by servers that
 * support object push and clients that support object pull.
 *
 * @param objHandle      a handle previously returned by an OI_OPP_OPEN_CFM function
 *
 * @param buffer         a pointer to a buffer containing the data to be written to
 *                       the object
 *
 * @param bufLen         the number of bytes to write to the object on this call.
 *
 * @param writeCfm       function that will be called when the read completes.
 *
 * @param oppConnection  identifies which OPP client or server connection is 
 *                       performing this operation.
 */
typedef OI_STATUS (*OI_OPP_WRITE)(OI_OPP_HANDLE objHandle,
                                  const OI_BYTE *buffer,
                                  OI_UINT16 bufLen,
                                  OI_OPP_WRITE_CFM writeCfm,
                                  OI_OPP_CONNECTION oppConnection);


/* ********************************************************************************
 * 
 *               Object operations
 *
 * ********************************************************************************/

typedef struct {

    OI_OPP_OPEN_READ       OpenRead;
    OI_OPP_OPEN_WRITE      OpenWrite; 
    OI_OPP_CLOSE           Close;
    OI_OPP_READ            Read;
    OI_OPP_WRITE           Write;

} OI_OPP_OBJSYS_FUNCTIONS;


#ifdef OI_COMPAT_V10
#error code not present
#endif


#endif /* _OI_OPP_SYS_H */

