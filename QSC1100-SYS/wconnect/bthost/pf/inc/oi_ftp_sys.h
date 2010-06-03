#ifndef _OI_FTP_SYS_H
#define _OI_FTP_SYS_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/inc/oi_ftp_sys.h#2 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-01-14  gs   Updated OI_FTP_WRITE() signature to include object size info.
*
*    #5        18 Oct  2006           PR
*  Merged OI Code v3.0.14rev1revQ
*
*    #4        10 Oct 2006            GS
* Added object size parameter in OI_FTP_OPEN.
*
*    #3        08 Jul 2005            JH
* Modified definition of OI_FTP_CLOSE to include status as a parameter.
*
===========================================================================*/
/**
 @file  

 File system abstraction layer used by the the File Transfer Profile client and
 server.

 This interface is described entirely as callback functions. The application
 must provide the appropriate sets of functions to the FTP client and FTP server
 when initializing these two services. An Bluetooth compliant implementation of
 an FTP server must return certain specific error codes to report various
 operation failure modes. Please refer to the Windows and/or Linux
 implementations of these functions for more information.


 Callbacks and errors are mutually exclusive.  If you invoke a
 callback from one of the functions below, that function must return
 OI_OK.  If you do not invoke a callback, the function must return an
 error message.  Note that if you wish your application to be
 BQB-compliant, you should generate error codes consistent with
 BLUEmagic 3.0 best practices; see the sample code for examples.

*/

/** \addtogroup FTP FTP APIs */
/**@{*/

/**********************************************************************************
  $Revision: #2 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_status.h"
#include "oi_stddefs.h"
#include "oi_obexspec.h"


typedef OI_UINT16 OI_FTP_CONNECTION;

typedef enum {
    OI_FTP_MODE_READ,  /**< open a file for reading */
    OI_FTP_MODE_WRITE  /**< open a file for writing */
} OI_FTP_MODE;


typedef void* OI_FTP_HANDLE;


/* ********************************************************************************
 * The file and folder functions are all asynchronous. However implementations
 * are permitted to call the callback function from within the function if the
 * file operation can be completed quickly. This will be the case for many
 * implementations.
 * ********************************************************************************/




/**
 * A function of this type is called to confirm the success or failure of a
 * folder browse operation and return a handle that can be used to read the
 * folder contents. The folder contents can be read as a file and the handle
 * will be closed when the contents have been read.
 *
 * @param handle         A handle that can be used to read the folder contents.
 *                       The folder contents are returned as XML as defined by
 *                       the OBEX specification.  The handle must not be NULL.
 *
 * @param size           The total size of the XML representation of the folder contents
 *
 * @param status         OI_OK if the folder could be opened for browsing or an
 *                       error if the folder could not be opened.
 *
 * @param ftpConnection  identifies which FTP client or server connection is 
 *                       performing this operation.
 */

typedef void (*OI_FTP_BROWSE_FOLDER_CFM)(OI_FTP_HANDLE handle,
                                         OI_UINT32 size,
                                         OI_STATUS status,
                                         OI_FTP_CONNECTION ftpConnection);

/**
 * This function and corresponding callback is only needed by an FTP server.
 *
 * Open a handle to read the contents of a folder.
 *
 * The folder contents can read from the handle like a file.
 * The handle will be closed when the contents have been read. 
 *
 * @param name             The unicode name for the folder or NULL. If not null
 *                         the folder must be a subfolder of the current folder.
 *
 * @param folderBrowseCfm  The function that will be called when the folder
 *                         contents are available to be read.
 *
 * @param ftpConnection    identifies which FTP client or server connection is
 *                         performing this operation.
 */

typedef OI_STATUS (*OI_FTP_BROWSE_FOLDER)(const OI_OBEX_UNICODE *name,
                                          OI_FTP_BROWSE_FOLDER_CFM folderBrowseCfm,
                                          OI_FTP_CONNECTION ftpConnection);


/**
 * A function of this type is called to confirm the success or failure of a file
 * open.
 *
 * @param handle        A handle that can be used to read the file contents.
 *                      The handle must not be NULL.
 *
 * @param size          If the file was opened for reading size is the number of
 *                      bytes that can be read from the file. If the file was
 *                      opened for writing the value is undefined.
 *
 * @param status         OI_OK if the folder could be opened with the required
 *                       mode or an error if the file could not be opened.
 *
 * @param ftpConnection  identifies which FTP client or server connection is 
 *                       performing this operation.
 */

typedef void (*OI_FTP_OPEN_CFM)(OI_FTP_HANDLE handle,
                                OI_UINT32 size,
                                OI_STATUS status,
                                OI_FTP_CONNECTION ftpConnection);

/**
 * This function and corresponding callback is needed by an FTP client and server.
 *
 * @param name           a unicode string name of the file to be opened.
 *
 * @param mode           indicates if the file is being opened for read or write
 *                       access.
 *
 * @param openCfm        the function that will be called when the open
 *                       completes.
 *
 * @param ftpConnection  identifies which FTP client or server connection is 
 *                       performing this operation.
 */

typedef OI_STATUS (*OI_FTP_OPEN)(const OI_OBEX_UNICODE *name,
                                 OI_FTP_MODE mode,
#ifndef OI_CODE
                                 OI_UINT32 size,
#endif
                                 OI_FTP_OPEN_CFM openCfm,
                                 OI_FTP_CONNECTION ftpConnection);

/**
 * This function does not use a callback function. It is assumed that file
 * close will complete asynchronously or in the case of a failure will report or
 * log an error with the application.  
 *
 * @param handle         a handle for an open file.
 *
 * @param ftpConnection  identifies which FTP client or server connection is 
 *                       performing this operation.
 *
 * @param status         OI_OK if the file operation completed succesfully. An
 *                       error status if the file operation terminated before it
 *                       was complete.
 */

#ifdef OI_CODE
#error code not present
#else
typedef void (*OI_FTP_CLOSE)(OI_FTP_HANDLE handle,
                             OI_FTP_CONNECTION ftpConnection,
                             OI_STATUS status);
#endif /* OI_CODE */

/**
 * A function of this type is called to confirm the success or failure of a file
 * read.
 *
 * @param data           a pointer to a buffer containing the data read.
 *
 * @param len            the number of bytes read.
 *
 * @param status         a status code indicating if the read was succesful:
 *                       - OI_OK if the read completed
 *                       - OI_STATUS_END_OF_FILE if the read was successful and the
 *                         end of file has been reached.
 *                       - An error status indicating that the read failed.
 *
 * @param ftpConnection  identifies which FTP client or server connection is 
 *                       performing this operation.
 *
 */

typedef void (*OI_FTP_READ_CFM)(OI_FTP_HANDLE handle,
                                OI_BYTE *data,
                                OI_UINT16 len,
                                OI_STATUS status,
                                OI_FTP_CONNECTION ftpConnection);

/**
 * This function and corresponding callback is needed by an FTP client and server.
 *
 * @param handle         a handle previously returned by an OI_FTP_OPEN_CFM
 *                       function
 *
 * @param maxRead        the maximum number of bytes to read from the file on
 *                       this call.
 *
 * @param readCfm        the function that will be called when the read
 *                       completes.
 *
 * @param ftpConnection  identifies which FTP client or server connection is 
 *                       performing this operation.
 */


typedef OI_STATUS (*OI_FTP_READ)(OI_FTP_HANDLE handle,
                                 OI_UINT16 maxRead,
                                 OI_FTP_READ_CFM readCfm,
                                 OI_FTP_CONNECTION ftpConnection);


/**
 * A function of this type is called to confirm the success or failure of a
 * file write operation.
 *
 * @param handle         a handle previously returned by an OI_FTP_OPEN_CFM
 *                       function
 *
 * @param status         indicates the success or failure of the write
 *                       operation.
 *
 * @param ftpConnection  identifies which FTP client or server connection is 
 *                       performing this operation.
 */

typedef void (*OI_FTP_WRITE_CFM)(OI_FTP_HANDLE handle,
                                 OI_STATUS status,
                                 OI_FTP_CONNECTION ftpConnection);


/**
 * This function and corresponding callback is needed by an FTP client and server.
 *
 * @param handle         a handle previously returned by an OI_FTP_OPEN_CFM
 *                       function
 *
 * @param buffer         a pointer to a buffer containing the data to be written
 *                       to the file
 *
 * @param bufLen         the number of bytes to write to the file on this call.
 *
 * @param writeCfm       the function that will be called when the read
 *                       completes.
 *
 * @param ftpConnection  identifies which FTP client or server connection is 
 *                       performing this operation.
 */
#ifdef OI_CODE
#error code not present
#else
typedef OI_STATUS (*OI_FTP_WRITE)(OI_FTP_HANDLE handle,
                                  const OI_BYTE *buffer,
                                  OI_UINT16 bufLen,
                                  OI_FTP_WRITE_CFM writeCfm,
                                  OI_FTP_CONNECTION ftpConnection,
                                  OI_UINT32 objSize );
#endif

/**
 * A function of this type is called to confirm the success or failure of a file
 * or folder delete operation.
 *
 * @param status         a status code indicating if the delete was successful.
 *                       If the delete was for a folder and the folder could not
 *                       be deleted because it was not empty the status returned
 *                       should be OI_OBEX_PRECONDITION_FAILED.
 *
 * @param ftpConnection  identifies which FTP client or server connection is 
 *                       performing this operation.
 */


typedef void (*OI_FTP_DELETE_CFM)(OI_STATUS status,
                                  OI_FTP_CONNECTION ftpConnection);


/**
 * This function and corresponding callback is only needed by an FTP server.
 *
 * @param name           a unicode string name of the file or folder to be
 *                       deleted.
 
 * @param deleteCfm      the function that will be called when the open
 *                       completes.
 *
 * @param ftpConnection  identifies which FTP client or server connection is
 *                       performing this operation.
 */


typedef OI_STATUS (*OI_FTP_DELETE)(const OI_OBEX_UNICODE *name,
                                   OI_FTP_DELETE_CFM deleteCfm,
                                   OI_FTP_CONNECTION ftpConnection);


/**
 * A function of this type is called to confirm the success or failure of a set
 * folder operation.
 *
 * @param status Indicates the success or failure of the set folder operation.
 *
 * @param ftpConnection    identifies which FTP client or server connection is
 *                         performing this operation.
 */

typedef void (*OI_FTP_SET_FOLDER_CFM)(OI_STATUS status,
                                      OI_FTP_CONNECTION ftpConnection);


/**
 * This function sets the current folder.
 *
 * @param folder        Unicode folder name or NULL. If the folder name is NULL
 *                      the folder is set to the parent folder of the current
 *                      folder.
 *
 * @param level         Level in the folder hierarchy, 0 means the root folder.
 *
 * @param create        If TRUE and the folder does not exist create a new
 *                      folder with that name. Then change to that directory.
 *
 * @param ftpConnection identifies which FTP client or server connection is 
 *                      performing this operation.
 */

typedef OI_STATUS (*OI_FTP_SET_FOLDER)(const OI_OBEX_UNICODE *folder,
                                       OI_UINT8 level,
                                       OI_BOOL create,
                                       OI_FTP_SET_FOLDER_CFM setFolderCfm,
                                       OI_FTP_CONNECTION ftpConnection);


/* ********************************************************************************
 * 
 *                             File operations
 *
 * ********************************************************************************/


typedef struct {

    OI_FTP_BROWSE_FOLDER   browseFolder;
    OI_FTP_OPEN            open;
    OI_FTP_CLOSE           close;
    OI_FTP_READ            read;
    OI_FTP_WRITE           write;
    OI_FTP_DELETE          delete;
    OI_FTP_SET_FOLDER      setFolder;

} OI_FTP_FILESYS_FUNCTIONS;


/**@}*/

#endif /* _OI_FTP_SYS_H */

