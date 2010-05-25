/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/ftp_server.c#2 $ 

 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2008-01-14   gs  Modified fops->write callback signature to pass file size
                  info to upper layer.
 2008-05-06   gs  Allow disconnect ind to go to the app irrespective of
                  whether there is a pending file sys operation confirmation 
                  or not.

*    #20        12 Feb 2008           RS
* Handle the situation when the remote sends type header with
* no string.
*
*    #19        21 May 2007           SSK
* Fix incorrect order of parameters in ftpserver open call.
*
*    #18        28 Feb 2007            MH
* Modified disconnect indication to support optional headers
*
*    #17        13 Feb 2007            MH
* Modified Set path indication to support optional headers
*
*    #16       18 Oct 2006            PR
* Merged OI Code v3.0.14
*
*    #15     26 Oct 2006              MH
* Modified srv confirm set path and srv connect indication call back to 
* support all headers
*
*    #14       10 Oct 2006            GS
* Added support for parsing object size in FTP Put request.
*
*    #13       12 Sep 2006            RP
* Modified to return "OI_OBEX_UNAUTHORIZED" errorcode, in the case when a
* folder creation is tried, when the server is in readonly mode.
*
*    #12       29 Jul 2006            BK
* Modified for obex client authentication.
*
*    #10-11    10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #9        23 Jun 2006            JH
* Modified to work with updated OBEX code which supports JSR82.
*
*    #8        05 Oct 2005            JH
* Removed "static" qualifier from IsBaseName().
*
*    #7        04 Oct 2005            GS
* Modified ServerGetInd() to process partial GET request instead of throwing
* it away.  Fixes interop issue with BlueSoliel 1.4 stack.
*
*    #6        08 Jul 2005            JH
* Added status parameter to all calls to the file close callback.
*
*    #5        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
*    #4        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
*    #3        10 Aug 2004            JH
* Modified ServerGetCleanup section in ServerGetInd to invoke the file close
*  callback after a folder browse operation.

===========================================================================*/
/**
@file
@internal

File Transfer Profile server.
*/

/**********************************************************************************
  $Revision: #2 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#define __OI_MODULE__ OI_MODULE_FTP_SRV

#include "oi_ftp_server.h"
#include "oi_ftp_sys.h"
#include "oi_obexsrv.h"
#include "oi_std_utils.h"
#include "oi_memmgr.h"
#include "oi_debug.h"
#include "oi_assert.h"
#include "oi_bt_assigned_nos.h"
#include "oi_sdpdb.h"
#include "oi_sdp_utils.h"
#include "oi_argcheck.h"
#include "oi_securitydb.h"
#include "oi_devmgr.h"
#include "oi_dispatch.h"

#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"

/*
 * Service class ID list
 */

static const OI_DATAELEM ServiceClassIDList[] = {
    OI_ELEMENT_UUID32(OI_UUID_OBEXFileTransfer)
};

/*
 * Profile descriptor list
 */

static const OI_DATAELEM Profile0[] = {
    OI_ELEMENT_UUID32(OI_UUID_OBEXFileTransfer),
    OI_ELEMENT_UINT16(0x0100) /* version 1.0 */
};

static const OI_DATAELEM ProfileDescriptorList[] = {
    OI_ELEMENT_SEQ(Profile0)
};

/*
 * SDP Attribute lists
 */

static const OI_SDPDB_ATTRIBUTE ServiceDescription[] = {
    { OI_ATTRID_ServiceClassIDList,                   OI_ELEMENT_SEQ(ServiceClassIDList) },
    { OI_ATTRID_BluetoothProfileDescriptorList,       OI_ELEMENT_SEQ(ProfileDescriptorList) }
};



/*
 * Note that the order of these values is important.
 */
typedef enum {
    FTP_SERVER_IDLE      = 0,
    FTP_SERVER_CONNECTED = 1,
    FTP_GET_FILE_PENDING      = 2,
    FTP_GET_FILE              = 3,
    FTP_BROWSE_FOLDER_PENDING = 4,
    FTP_BROWSE_FOLDER         = 5,
    FTP_PUT_FILE_PENDING      = 6,
    FTP_PUT_FILE              = 7,
    FTP_DELETE_FILE           = 8,
    FTP_SET_FOLDER            = 9
} FTP_SERVER_STATE;


/*
 * Struct for an OBEX FTP server connection.
 */

typedef struct {

    FTP_SERVER_STATE state;
    OI_FTP_HANDLE file;
    const OI_FTP_FILESYS_FUNCTIONS *fops; /* file operations */

    OI_OBEX_BYTESEQ earlyBody;            /* Body received in the first put packet */

    OI_UINT16 maxRead;                    /* maximum read for the current connection */

    OI_BOOL fileOpCfmPending;
    OI_BOOL connected;
    DISPATCH_CB_HANDLE closeCB;

    OI_BOOL final;
    OI_BOOL unauthorized;
    OI_BOOL incompleteGet;

    OI_BOOL readOnly;                     /* Server session is read only */
    OI_BOOL allowBrowse;                  /* Is folder browsing allowed */

    struct {
        OI_BOOL readOnly;               /* default access for unauthorized clients */
        OI_BOOL allowBrowse;            /* default browsing for unauthorized clients */
    } defaultAccess;

    OI_BOOL createFolderDisallowed;     /* client allows create folder but server does not */

    OI_OBEXSRV_CONNECTION_HANDLE id;

    OI_FTP_CONNECTION_IND connectInd;
    OI_FTP_DISCONNECTION_IND disconnectInd;

    OI_OBEX_UNICODE fileName;

    OI_OBEX_UNICODE rootFolder;
    OI_UINT8 folderLevel;               /* 0 means at root folder */
    OI_UINT8 reqLevel;     

    OI_UINT32 srecHandle;      /* Service record handle for the FTP service. */
    OI_UINT8 channel;

    OI_CONNECT_POLICY connectPolicy;

    OI_SDPDB_ATTRIBUTE sdpAttributes[1]; /**< Protocol Descriptor list */

} FTP_SERVER;


static FTP_SERVER *server;


#if defined(OI_DEBUG)
#error code not present
#else

#define ServerStateText(state) ""

#define setState(newState)                      \
    do {                                        \
        server->state = (newState);             \
    } while (0)

#define CHK_CFM_STATE(_fn, _state)                                      \
    do {                                                                \
        if ((server->state != (_state)) || !server->fileOpCfmPending) { \
            OI_LOG_ERROR(("%s() called at wrong time (state valid? %B   confirm pending? %B).\n", \
                          (_fn), (server->state == (_state)),           \
                          server->fileOpCfmPending));                   \
            return;                                                     \
        }                                                               \
        server->fileOpCfmPending = FALSE;                               \
        if (!server->connected) {                                       \
            setupDeferredFileClose(OI_OBEX_NOT_CONNECTED);              \
            return;                                                     \
        }                                                               \
    } while (0)

#endif 





/*
 * dispatcher calls us so we can call close()
 */
static void deferredFileClose(DISPATCH_ARG *darg)
{
    OI_STATUS status = Dispatch_GetArg(darg, OI_STATUS);
    OI_OBEXSRV_CONNECTION_HANDLE tmpId;
    FTP_SERVER_STATE snapState;
    OI_FTP_HANDLE file;

    OI_DBGTRACE(("deferredFileClose (<*darg = %x>)\n", darg));

    snapState = server->state;
    file = server->file;
    server->closeCB = 0;

    if (server->connected) {
        setState(FTP_SERVER_CONNECTED);
    } else {
        setState(FTP_SERVER_IDLE);
    }

    OI_FreeIf(&server->fileName.str);
    server->file = NULL;

    if (file) {
        server->fops->close(file, (OI_FTP_CONNECTION)server->id, status);
    }

    /* If we're disconnected, call the disconnect indication now. */
    if (!server->connected) {
        /* Clear out server context - we are no longer connected. */
        tmpId = server->id;
        server->id = 0;
        server->connected = FALSE;

        /* If the connection was in place, call the application disconnect
         * indication callback if one was provided. */
        if ((snapState != FTP_SERVER_IDLE) && (server->disconnectInd != NULL)) {
            server->disconnectInd(tmpId);
        }
    }
}

static void setupDeferredFileClose(OI_STATUS status)
{
    DISPATCH_ARG                darg;
#ifdef OI_CODE
#error code not present
#endif

    OI_DBGTRACE(("setupDeferredFileClose (status = %!)\n", status));

    Dispatch_SetArg(darg, status);
#ifdef OI_CODE
#error code not present
#else
    deferredFileClose(&darg);
#endif
}

static OI_STATUS SaveFileName(OI_OBEX_UNICODE *name)
{
    OI_UINT16 size = name->len * sizeof(OI_CHAR16);

    OI_ASSERT(server->fileName.str == NULL);

    server->fileName.str = OI_Malloc(size);
    if (server->fileName.str == NULL) {
        return OI_STATUS_OUT_OF_MEMORY;
    } else {
        OI_MemCopy(server->fileName.str, name->str, size);
        server->fileName.len = name->len;
    }
    return OI_OK;
}


/**
 * Check that a file or folder name is a base name. That is, check that the name
 * does not include relative or absolute path. 
 *
 * Rejects names that contain "/", backslash, "..", or ":"
 */
#ifdef OI_CODE
#error code not present
#else
/* This function is also used in opp_common.c. */
OI_BOOL IsBaseName(OI_OBEX_UNICODE *name)
#endif
{
    OI_INT i;

    if ((name == NULL) || (name->str == NULL)) {
        /* An empty name is always a base name. */
        return TRUE;
    }

    /*
     * Reject . ..
     */
    if (name->str[0] == (OI_CHAR16) '.') {
        if ((name->len == 1) || (name->str[1] == 0)) {
            goto NotBase;
        }
        if (name->str[1] == ((OI_CHAR16) '.')  && ((name->len == 2) || (name->str[2] == 0))) {
            goto NotBase;
        }
    }
    /*
     * Reject : / \  
     */
    for (i = 0; i < name->len; ++i) {
        OI_CHAR16 c = name->str[i];
        if ((c == (OI_CHAR16)(':')) || (c == (OI_CHAR16)('/')) || (c == (OI_CHAR16)('\\'))) {
            goto NotBase;
        }
    }
    return TRUE;

NotBase:

    OI_DBGPRINT2(("Disallowing folder name %S", name->str));
    return FALSE;
}


static void FileDeleteCfm(OI_STATUS status,
                          OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("FileDeleteCfm (status = %!, ftpConnection = 0x%x)\n", status, ftpConnection));

    CHK_CFM_STATE("FileDeleteCfm", FTP_DELETE_FILE);

    setState(FTP_SERVER_CONNECTED);

    status = OI_OBEXSRV_PutResponse(server->id, NULL, status);
    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("Put response failed %!\n", status));
    }
}


static void FileWriteCfm(OI_FTP_HANDLE handle,
                         OI_STATUS status,
                         OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("FileWriteCfm (handle = 0x%x, status = %!, ftpConnection = 0x%x)\n",
                 handle, status, ftpConnection));

    CHK_CFM_STATE("FileWriteCfm", FTP_PUT_FILE);

    /*
     * If we are continuing let obex know.
     */
    if (OI_SUCCESS(status) && !server->final) {
        status = OI_OBEXSRV_PutResponse(server->id, NULL, OI_OBEX_CONTINUE);
        if (!OI_SUCCESS(status)) {
            OI_LOG_ERROR(("Put response failed %!\n", status));
        }
        return;
    }

    /*
     * If we got an error or we are done, close the file and report status to obex.
     */
    if (server->file) {
        /*
         * This code might be running on application thread via FileOpenWriteCfm, so we
         * must put the fops close() on dispatcher.
     */
        setupDeferredFileClose(status);
    }
    status = OI_OBEXSRV_PutResponse(server->id, NULL, status);
    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("Put response failed %!\n", status));
    }
}


static void FileOpenWriteCfm(OI_FTP_HANDLE handle,
                             OI_UINT32 size,
                             OI_STATUS status,
                             OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("FileOpenWriteCfm (handle = 0x%x, size = %d, status = %!, ftpConnection = 0x%x)\n",
                 handle, size, status, ftpConnection));

    CHK_CFM_STATE("FileOpenWriteCfm", FTP_PUT_FILE_PENDING);

    if (!OI_SUCCESS(status)) {
        /*
         * File open did not succeed.
         */
        status = OI_OBEXSRV_PutResponse(server->id, NULL, status);
        if (!OI_SUCCESS(status)) {
            OI_LOG_ERROR(("Put response failed %!\n", status));
        }
        setState(FTP_SERVER_CONNECTED);
        return;
    }

    OI_ASSERT(handle != NULL);
    if (handle == NULL) {
        OI_LOG_ERROR(("Parameter check: %!\n", OI_STATUS_INVALID_PARAMETERS));
        return;
    }

    server->file = handle;
    setState(FTP_PUT_FILE);

    /* For the FileWriteCfm() call later. */
    server->fileOpCfmPending = TRUE;

    /*
     * Was there some initial data to put?
     */
    if (server->earlyBody.data) {
#ifdef OI_CODE
#error code not present
#else
       status = server->fops->write(server->file, server->earlyBody.data, server->earlyBody.len, FileWriteCfm, (OI_FTP_CONNECTION)server->id, size);
#endif
        server->earlyBody.data = NULL;
        if (OI_SUCCESS(status)) {
            return;
        }
    }
    /*
     * There was nothing to write but the response is the same as if there was.
     */
    FileWriteCfm(server->file, status, (OI_FTP_CONNECTION)server->id);
}


static OI_STATUS ServerPutInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                              OI_OBEX_HEADER_LIST *cmdHeaders,
                              OI_STATUS obexStatus)
{
    OI_STATUS status;
    OI_INT i;
    OI_OBEX_UNICODE *uname = NULL;
    OI_OBEX_HEADER *bodyHdr = NULL;
#ifndef OI_CODE
    OI_UINT32 objSize = 0;
#endif

    OI_DBGTRACE(("ServerPutInd (connectionId = 0x%x, <*cmdHeaders = %x>, obexStatus = %!)\n",
                 connectionId, cmdHeaders, obexStatus));

    /*
     * Check this operation is allowed at this time.
     */
    if ((server->state != FTP_PUT_FILE) && (server->state != FTP_SERVER_CONNECTED)) {
        return OI_OBEX_SERVICE_UNAVAILABLE;
    }

    /*
     * Check that put (or delete) operation is allowed for this connection.
     */
    if (server->readOnly) {
        return OI_OBEX_UNAUTHORIZED;
    }

    if (OI_SUCCESS(obexStatus) || (obexStatus == OI_OBEX_CONTINUE)) {
        server->final = (obexStatus != OI_OBEX_CONTINUE);
    } else {
        status = OI_OK; /* Obex told us about the error */
        goto ServerPutError;
    }

    /*
     * Parse the command headers.
     */
    for (i = 0; i < cmdHeaders->count; ++i) {
        switch (cmdHeaders->list[i].id) {
            case OI_OBEX_HDR_NAME:
                uname = &(cmdHeaders->list[i].val.name);
                /*
                 * Only allow base file or folder names.
                 */
                if (!IsBaseName(uname)) {
                    status = OI_OBEX_NOT_FOUND;
                    goto ServerPutError;
                }
                break;
#ifndef OI_CODE
            case OI_OBEX_HDR_LENGTH:
                objSize = cmdHeaders->list[i].val.length;
                break;
#endif
            case OI_OBEX_HDR_BODY:
            case OI_OBEX_HDR_END_OF_BODY:
                bodyHdr = &cmdHeaders->list[i];
                break;
            default:
                /* Other headers are ignored. */
                break;
        }
    }

    /*
     * Continuing a put operation?
     */
    if (server->state == FTP_PUT_FILE) {
        /*
         * If we didn't get a body header we should return an error.
         */
        if (bodyHdr == NULL) {
            status = OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
            goto ServerPutError;
        }
        server->fileOpCfmPending = TRUE;
#ifdef OI_CODE
#error code not present
#else
        status = server->fops->write(server->file, bodyHdr->val.body.data, bodyHdr->val.body.len, FileWriteCfm, (OI_FTP_CONNECTION)server->id, objSize);
#endif
        if (!OI_SUCCESS(status)) {
            goto ServerPutError;
        }
        return OI_OK;
    }

    /*
     * Final with no body is a delete operation.
     */
    if (server->final && (bodyHdr == NULL)) {
        if (uname == NULL) {
            status = OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
        } else {
            setState(FTP_DELETE_FILE);
            server->fileOpCfmPending = TRUE;
            status = server->fops->delete(uname, FileDeleteCfm, (OI_FTP_CONNECTION)server->id);
        }
        if (!OI_SUCCESS(status)) {
            server->fileOpCfmPending = FALSE;
            goto ServerPutError;
        }
        return OI_OK;
    }

    /*
     * A file put operation requires a name for the file to put.
     */
    if ((uname == NULL) || (uname->len == 0)) {
        status = OI_OBEX_NOT_FOUND;
        goto ServerPutError;
    }

    status = SaveFileName(uname);
    if (!OI_SUCCESS(status)) {
        goto ServerPutError;
    }

    /*
     * The OBEX specification recommends but does not require that body
     * headers follow the intial put request packet. So, if there is body
     * data in this put command we need to hold onto the header until we are
     * ready to write it to the file.
     */
    if ((bodyHdr != NULL) && (bodyHdr->val.body.len != 0)) {
        server->earlyBody = bodyHdr->val.body;
    } else {
        server->earlyBody.data = NULL;
        server->earlyBody.len = 0;
    }

    setState(FTP_PUT_FILE_PENDING);
    server->fileOpCfmPending = TRUE;
#ifdef OI_CODE
#error code not present
#else
    status = server->fops->open(uname, OI_FTP_MODE_WRITE, objSize, FileOpenWriteCfm, (OI_FTP_CONNECTION)server->id);
#endif
    if (!OI_SUCCESS(status)) {
        server->fileOpCfmPending = FALSE;
        goto ServerPutError;
    }

    return OI_OK;

ServerPutError:

    OI_DBGPRINT(("FTP PUT failed: %!\n", obexStatus));
    setupDeferredFileClose(obexStatus);
    return status;
}


static void CommonOpenReadCfm(OI_FTP_HANDLE handle,
                            OI_UINT32 size,
                            OI_STATUS status,
                              OI_FTP_CONNECTION ftpConnection,
                              FTP_SERVER_STATE nextState)
{
    OI_OBEX_HEADER hdr;
    OI_OBEX_HEADER_LIST hdrList;

    if (!OI_SUCCESS(status)) {
        /*
         * File was not opened so it doesn't need to be closed but we need to
         * report the error to obex.
         */
        setState(FTP_SERVER_CONNECTED);
        status = OI_OBEXSRV_GetResponse(server->id, NULL, status);
        if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Get response failed "));
#endif
        }
        return;
    }

    OI_ASSERT(handle != NULL);
    if (handle == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Parameter check: Invalid Paramater "));
#endif
        return;
    }

    server->file = handle;
    setState(nextState);

    /*
     * Respond with the length of the file.
     */
    hdr.id = OI_OBEX_HDR_LENGTH;
    hdr.val.length = size;
    hdrList.list = &hdr;
    hdrList.count = 1;
    status = OI_OBEXSRV_GetResponse(server->id, &hdrList, OI_OBEX_CONTINUE);
    if (!OI_SUCCESS(status)) {
        /*
         * Close the file and cleanup.  Cannot call application on application thread, so
         * put the fops close() on dispatcher.
         */
        setupDeferredFileClose(status);
        server->file = NULL;
    }
}


static void FileOpenReadCfm(OI_FTP_HANDLE handle,
                            OI_UINT32 size,
                            OI_STATUS status,
                            OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("FileOpenReadCfm (handle = 0x%x, size = %d, status = %!, ftpConnection = 0x%x)\n",
                 handle, size, status, ftpConnection));

    CHK_CFM_STATE("FileOpenReadCfm", FTP_GET_FILE_PENDING);

    CommonOpenReadCfm(handle, size, status, ftpConnection, FTP_GET_FILE);
}

static void BrowseFolderOpenCfm(OI_FTP_HANDLE handle,
                                OI_UINT32 size,
                                OI_STATUS status,
                                OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("BrowseFolderCfm (handle = 0x%x, size = %d, status = %!, ftpConnection = 0x%x)\n",
                 handle, size, status, ftpConnection));

    CHK_CFM_STATE("BrowseFolderOpenCfm", FTP_BROWSE_FOLDER_PENDING);

    CommonOpenReadCfm(handle, size, status, ftpConnection, FTP_BROWSE_FOLDER);
}


static void CommonFileReadCfm(OI_FTP_HANDLE handle, 
                        OI_BYTE *data,
                        OI_UINT16 len,
                        OI_STATUS status,
                        OI_FTP_CONNECTION ftpConnection)
{
    OI_OBEX_HEADER hdr;
    OI_OBEX_HEADER_LIST hdrList;

    if (!OI_SUCCESS(status) && (status != OI_STATUS_END_OF_FILE)) {
        setupDeferredFileClose(status);

        status = OI_OBEXSRV_GetResponse(server->id, NULL, status);
        if (!OI_SUCCESS(status)) {
            OI_LOG_ERROR(("Get response failed %!\n", status));
        }
        return;
    }

    if (status == OI_STATUS_END_OF_FILE) {
        hdr.id = OI_OBEX_HDR_END_OF_BODY;
        status = OI_OK;
    } else {
        hdr.id = OI_OBEX_HDR_BODY;
        status = OI_OBEX_CONTINUE;
    }

    hdr.val.body.data = data;
    hdr.val.body.len = len;
    hdrList.list = &hdr;
    hdrList.count = 1;

    status = OI_OBEXSRV_GetResponse(server->id, &hdrList, status);
    if (!OI_SUCCESS(status)) {
        setupDeferredFileClose(status);
    }
}

static void FileReadCfm(OI_FTP_HANDLE handle,
                        OI_BYTE *data,
                        OI_UINT16 len,
                        OI_STATUS status,
                        OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("FileReadCfm (handle = 0x%x, <*data = %x>, len = %d, status = %!, ftpConnection = 0x%x)\n",
                 handle, data, len, status, ftpConnection));

    CHK_CFM_STATE("FileReadCfm", FTP_GET_FILE);

    CommonFileReadCfm(handle, data, len, status, ftpConnection);
    }

static void BrowseFolderCfm(OI_FTP_HANDLE handle,
                            OI_BYTE *data,
                            OI_UINT16 len,
                            OI_STATUS status,
                            OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("BrowseFolderCfm (handle = 0x%x, <*data = %x>, len = %d, status = %!, ftpConnection = 0x%x)\n",
                 handle, data, len, status, ftpConnection));

    CHK_CFM_STATE("BrowseFolderCfm", FTP_BROWSE_FOLDER);

    CommonFileReadCfm(handle, data, len, status, ftpConnection);
}



static OI_STATUS ServerGetInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                              OI_OBEX_HEADER_LIST *cmdHeaders,
                              OI_STATUS obexStatus)
{
    OI_INT i;
    OI_STATUS status;
    OI_OBEX_BYTESEQ *type = NULL;
    OI_OBEX_UNICODE *uname = NULL;

    OI_DBGTRACE(("ServerGetInd (connectionId = 0x%x, <*cmdHeaders = %x>, obexStatus = %!)\n",
                 connectionId, cmdHeaders, obexStatus));

    /*
     * If there was an error or the GET operation is now complete, close the
     * file and free memory allocated.
     */
    if (OI_SUCCESS(obexStatus) || ((obexStatus != OI_OBEX_CONTINUE) && (obexStatus != OI_OBEXSRV_INCOMPLETE_GET))) {
        OI_DBGPRINT(("ServerGetInd terminated: status %!", obexStatus));
        status = OI_OK;
        goto ServerGetCleanup;
    }

    /*
     * Continuing a file or folder get operation.
     */
    if (!server->incompleteGet && ((server->state == FTP_GET_FILE) || (server->state == FTP_BROWSE_FOLDER))) {
        server->fileOpCfmPending = TRUE;
        if (server->state == FTP_GET_FILE) {
        status = server->fops->read(server->file, server->maxRead, FileReadCfm, (OI_FTP_CONNECTION)server->id);
        } else {
            status = server->fops->read(server->file, server->maxRead, BrowseFolderCfm, (OI_FTP_CONNECTION)server->id);
        }
        if (!OI_SUCCESS(status)) {
            OI_DBGPRINT2(("File read returned %!", status));
            server->fileOpCfmPending = FALSE;
            goto ServerGetCleanup;
        }
        return OI_OK;
    }

    if (server->state == FTP_SERVER_CONNECTED) {
        /*
         * Assume file get unless we see a folder listing type
         */
        setState(FTP_GET_FILE_PENDING);
    } else {
        /*
         *  We must be still doing the get
         */
        if (!server->incompleteGet) {
        return OI_OBEX_SERVICE_UNAVAILABLE;
        }
    }

    /*
     * Check headers for file get or folder browse operation.
     */
    for (i = 0; i < cmdHeaders->count; ++i) {
        OI_OBEX_HEADER *tmpHdr = cmdHeaders->list + i;
        switch (tmpHdr->id) {
            case OI_OBEX_HDR_NAME:
                uname = &(tmpHdr->val.name);
                /*
                 * Only allow base file or folder names.
                 */
                if (!IsBaseName(uname)) {
                    status = OI_OBEX_NOT_FOUND;
                    OI_DBGPRINT(("FTP GET failed: Requested name is not base name: \"%?S\"\n",
                                 uname->str, uname->len));
                    goto ServerGetCleanup;
                }
                break;
            case OI_OBEX_HDR_TYPE:
                type = &(tmpHdr->val.type);
                /*
                 * The only type allowed is the folder listing type
                 */
                /*
                 * In case the type data was not sent it would be 
                 * wrong to assume the operation is for folder listing.
                 * Hence allow the cleanup operation to proceed.
                 */
#ifndef OI_CODE
                if ( ( type == NULL ) || ( type->data == NULL ) ) {
                    status = OI_OBEX_ACCESS_DENIED;
                    OI_DBGPRINT(("FTP GET failed: no string found with type hdr\n"));
                    goto ServerGetCleanup;
                }
#endif
                if (!OI_StrcmpInsensitive((OI_CHAR*) type->data, OI_OBEX_FOLDER_LISTING_TYPE) == 0) {
                    status = OI_OBEX_ACCESS_DENIED;
                    OI_DBGPRINT(("FTP GET failed: Requested type is not %s: \"%s\"\n",
                                 OI_OBEX_FOLDER_LISTING_TYPE, (OI_CHAR*) type->data));
                    goto ServerGetCleanup;
                }
                /*
                 * Know we know we are doing a folder browse
                 */
                setState(FTP_BROWSE_FOLDER_PENDING);

                break;
            default:
                /* Ignore other headers. */
                break;
        }
    }

    /*
     *  If the get is incomplete we need to send a continue response.
     */
    server->incompleteGet = (obexStatus == OI_OBEXSRV_INCOMPLETE_GET);
    if (server->incompleteGet) {
        if (uname && (uname->len > 0)) {
            status = SaveFileName(uname);
            if (!OI_SUCCESS(status)) {
                OI_DBGPRINT(("FTP GET failed: Could not save file name \"%?S\": %!\n",
                             uname->str, uname->len, status));
                goto ServerGetCleanup;
            }
        }
        status =  OI_OBEXSRV_GetResponse(connectionId, NULL, OI_OBEX_CONTINUE);
        if (!OI_SUCCESS(status)) {
            OI_DBGPRINT(("FTP GET failed: Sending OBEX GET response: %!\n", status));
            goto ServerGetCleanup;
        }
        return OI_OK;
    }

    /*
     * Get request is complete
     */

    if (server->fileName.str != NULL) {
        if (uname) {
            OI_LOG_ERROR(("File name received twice!!!"));
        }
        uname = &server->fileName;
    }
    if (server->state == FTP_GET_FILE_PENDING) {
        /*
         * Cannot get a file without a file name
         */
        if ((uname == NULL) || (uname->len == 0)) {
            status = OI_OBEX_ACCESS_DENIED;
        } else {
            server->fileOpCfmPending = TRUE;
#ifdef OI_CODE
#error code not present
#else
            status = server->fops->open(uname, OI_FTP_MODE_READ, 0, FileOpenReadCfm, (OI_FTP_CONNECTION)server->id);
#endif
            if (!OI_SUCCESS(status)) {
                server->fileOpCfmPending = FALSE;
            }
        }
    } else {
        /*
         * If browsing is not allowed only allow the contents of the current
         * folder to be listed.
         */
        if ((uname != NULL) && !server->allowBrowse) {
            status = OI_OBEX_METHOD_NOT_ALLOWED;
        } else {
            server->fileOpCfmPending = TRUE;

            status = server->fops->browseFolder(uname, BrowseFolderOpenCfm, (OI_FTP_CONNECTION)server->id);
            if (!OI_SUCCESS(status)) {
                server->fileOpCfmPending = FALSE;
            }
            }
        }

    if (OI_SUCCESS(status)) {
        return OI_OK;
    }

    OI_DBGPRINT(("FTP GET failed: %!\n", status));

ServerGetCleanup:
    setupDeferredFileClose(obexStatus);

    return status;
}



static void SetFolderCfm(OI_STATUS status,
                         OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("SetFolderCfm (status = %!, ftpConnection = 0x%x)\n",
                 status, ftpConnection));

    CHK_CFM_STATE("SetFolderCfm", FTP_SET_FOLDER);

    setState(FTP_SERVER_CONNECTED);

    if (OI_SUCCESS(status)) {
        server->folderLevel = server->reqLevel;
    } else {
        /*
         * If the client wanted to create but was disallowed then the error
         * status code needs to be changed to unauthorized.
         */
        if ((status == OI_OBEX_NOT_FOUND) && server->createFolderDisallowed) {
#ifdef OI_CODE  
#error code not present
#else
            status = OI_OBEX_UNAUTHORIZED;
#endif
        
        }
    }

#ifdef OI_CODE
#error code not present
#else
    OI_OBEXSRV_ConfirmSetpath(server->id, NULL, status );
#endif
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Confirm setpath failed"));
#endif
    }
}


#ifdef OI_CODE
#error code not present
#else
static OI_STATUS ServerSetPathInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                  OI_OBEX_UNICODE *folder,
                                  OI_BOOL upLevel,
                                  OI_BOOL create,
                                  OI_OBEX_HEADER_LIST *headers
                                  )
#endif
{
    OI_STATUS status;

    OI_DBGTRACE(("ServerSetPathInd (connectionId = 0x%x, folder = \"%?S\", upLevel = %B, create = %B)\n",
                 connectionId, (folder ? folder->str : (OI_CHAR16*)"\0<\0n\0u\0l\0l\0>\0"), (folder ? folder->len : 6),
                 upLevel, create));

    /*
     * Override create flag if the server doesn't allow create.
     */
    if (create && server->readOnly) {
        server->createFolderDisallowed = TRUE;
        create = FALSE;
    } else {
        server->createFolderDisallowed = FALSE;
    }

    /*
     * NULL or empty folder name means set to root or parent folder
     */
    if ((folder == NULL) || (folder->str == NULL) || (folder->str[0] == 0)) {
        /*
         * Create is illegal with a null folder.
         */
        if (create) {
            return OI_OBEX_INVALID_OPERATION;
        }
        if (upLevel) {
            if (!server->allowBrowse) {
                return OI_OBEX_METHOD_NOT_ALLOWED;
            }
            /*
             * Check we are not aleady at the root folder
             */
            if (server->folderLevel == 0) {
                return OI_OBEX_NOT_FOUND;
            }

            OI_DBGPRINT2(("Set path to parent\n"));
            server->reqLevel = server->folderLevel - 1;
            folder = NULL;
            create = FALSE;
        } else {
            OI_DBGPRINT2(("Set path to root\n"));
            server->reqLevel = 0;
            folder = &server->rootFolder;
        }
    } else {
        /*
         * Reject this request if the server does allow folder browsing
         * for the current connection.
         */
        if (!server->allowBrowse) {
            return OI_OBEX_METHOD_NOT_ALLOWED;
        }
        /*
         * Check we have a valid folder name
         */
        if (folder->len == 0) {
            return OI_OBEX_NOT_FOUND;
        }
        if (!IsBaseName(folder)) {
            return OI_OBEX_NOT_FOUND;
        }
        OI_DBGPRINT2(("OBEX server set path %S\n", folder->str));
        server->reqLevel = server->folderLevel + 1;
    }

    setState(FTP_SET_FOLDER);
    server->fileOpCfmPending = TRUE;
    status = server->fops->setFolder(folder, server->reqLevel, create, SetFolderCfm, (OI_FTP_CONNECTION)server->id);
    /*
     * If the client wanted to create but was disallowed then the error
     * status code needs to be changed to unauthorized.
     */
    if ((status == OI_OBEX_NOT_FOUND) && server->createFolderDisallowed) {
#ifdef OI_CODE
#error code not present
#else
          status = OI_OBEX_UNAUTHORIZED;
#endif
    }

    if (!OI_SUCCESS(status)) {
        server->fileOpCfmPending = FALSE;
    }

    return status;
}


OI_STATUS OI_FTPServer_AuthenticationRsp(OI_FTP_SERVER_CONNECTION_HANDLE connectionId,
                                         const OI_CHAR *password,
                                         OI_BOOL readOnly,
                                         OI_BOOL allowBrowse)
{
    OI_DBGTRACE(("OI_FTPServer_AuthenticationRsp (connectionId = 0x%x, password = \"%s\", readOnlud = %B, allowBrowse = %B)\n",
                 connectionId, password, readOnly, allowBrowse));

    if (!OI_INIT_FLAG_VALUE(FTP_SRV)) {
        return OI_STATUS_NOT_INITIALIZED;
    }
    if ((server->state != FTP_SERVER_IDLE) || !server->unauthorized) {
        return OI_OBEX_INVALID_OPERATION;
    } else {
        server->readOnly = readOnly;
        server->allowBrowse = allowBrowse;
        return OI_OBEXSRV_AuthenticationResponse(server->id, password, readOnly);
    }
}


/*
 *
 */

#ifdef OI_CODE
#error code not present
#else
static void ServerConnectInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                             OI_BOOL unauthorized,
                             OI_BYTE *userId,
                             OI_UINT8 userIdLen,
                             OI_OBEX_HEADER_LIST *cmd_headers_ptr)
#endif
{
    OI_STATUS status;
    OI_BD_ADDR clientAddr;

    OI_DBGTRACE(("ServerConnectInd (connectionId = 0x%x, unauthorized = %B, userId = \"%?s\", userIdLen = %d)\n",
                 connectionId, unauthorized, userId, userIdLen, userIdLen));

    /*
     * Check that the server is not already in use.
     */
    if (server->state > FTP_SERVER_IDLE) {
#ifdef OI_CODE
#error code not present
#else
        status = OI_OBEXSRV_AcceptConnect(connectionId, FALSE, NULL, OI_STATUS_NO_RESOURCES);
#endif
        if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Accept connect failed"));
#endif
        }
        return;
    }

    server->unauthorized = unauthorized;
    server->readOnly = server->defaultAccess.readOnly;
    server->allowBrowse = server->defaultAccess.allowBrowse;
    server->id = connectionId;

    /*
     * Pass address of connecting client to application.
     */
    status = OI_OBEXSRV_GetClientAddr(connectionId, &clientAddr);
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        status = OI_OBEXSRV_AcceptConnect(connectionId, FALSE, NULL, OI_FAIL);
#endif
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Accept connect failed "));
#endif
        }
    } else {
        server->connectInd(&clientAddr, unauthorized, userId, userIdLen, connectionId);
    }
}


/*
 * After the root folder has been succefully set we can finally accept the
 * connection.
 */

static void SetRootFolderCfm(OI_STATUS status,
                             OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("SetRootFolderCfm (status = %!, ftpConnection = 0x%x)\n",
                 status, ftpConnection));

    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("Could not set the root folder for this connection %!", status));
#ifdef OI_CODE
#error code not present
#else
        status = OI_OBEXSRV_AcceptConnect(server->id, FALSE, NULL, status);
#endif
        if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
           OI_LOG_ERROR(("Accept connect failed "));
#endif
        }
    } else {
#ifdef OI_CODE
#error code not present
#else
        OI_OBEXSRV_ConfirmSetpath(server->id, NULL, status);
        status = OI_OBEXSRV_AcceptConnect(server->id, TRUE, NULL, OI_OK);
#endif
        if (OI_SUCCESS(status)) {
            setState(FTP_SERVER_CONNECTED);
            /*
             * Establish the best read size for file read operations.
             */
            server->maxRead = OI_OBEXSRV_OptimalBodyHeaderSize(server->id);
        } else {
            OI_DBGPRINT(("OBEX server AcceptConnect returned %!", status));
        }
    }
}


OI_STATUS OI_FTPServer_AcceptConnection(OI_FTP_SERVER_CONNECTION_HANDLE connectionId,
                                        OI_BOOL accept,
                                        const OI_OBEX_UNICODE *rootFolder)
{
    OI_STATUS status = OI_OBEX_ACCESS_DENIED;

    OI_DBGTRACE(("OI_FTPServer_AcceptConnection (connectionId = 0x%x, accept = %B, rootFolder = \"%?S\")\n",
                 connectionId, accept, rootFolder->str, rootFolder->len));

    if (!OI_INIT_FLAG_VALUE(FTP_SRV)) {
        return OI_STATUS_NOT_INITIALIZED;
    }
    if ((server->state != FTP_SERVER_IDLE) || (server->id != connectionId)) {
        return OI_OBEX_INVALID_OPERATION;
    }
    /*
     * If the connection is being rejected or was unauthorized reject the
     * connection.
     */
    if (server->unauthorized || !accept) {
        goto Reject;
    }
    /*
     * Initialize server state information.
     */
    server->rootFolder = *rootFolder;
    server->folderLevel = 0;
    server->reqLevel = 0;
    server->connected = TRUE;

    /*
     * Set the path to the root directory.
     */
    status = server->fops->setFolder(&server->rootFolder, 0, FALSE, SetRootFolderCfm, (OI_FTP_CONNECTION)server->id);
    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("Could not set root folder for FTP server"));
        goto Reject;
    }
    return OI_OK;

Reject:
    
    server->connected = FALSE;
#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXSRV_AcceptConnect(server->id, FALSE, NULL, status);
#endif
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Accept connect failed "));
#endif
    }

    return status;
}


#ifdef OI_CODE
#error code not present
#else
static void ServerDisconnectInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId, 
                                OI_OBEX_HEADER_LIST *headers)
#endif
{
    OI_DBGTRACE(("ServerDisconnectInd (connectionId = 0x%x)\n", connectionId));

    OI_ASSERT(server->id == connectionId);

    server->connected = FALSE;

    if (server->closeCB) {
        OI_DBGPRINT2(("Disconnect: calling close immediately"));
        OI_Dispatch_CallFunc(server->closeCB);
        return;
    }
#ifdef OI_CODE
#error code not present
#endif
        /* Status to be reported to the app depends on whether we're idle or
         * pushing/pulling. */
        setupDeferredFileClose(((server->state == FTP_SERVER_IDLE) ||
                                (server->state == FTP_SERVER_CONNECTED)) ? OI_OK : OI_OBEX_NOT_CONNECTED);
#ifdef OI_CODE
#error code not present
#endif
}


OI_STATUS OI_FTPServer_ForceDisconnect(OI_FTP_SERVER_CONNECTION_HANDLE connectionId)
{
    OI_DBGTRACE(("OI_FTPServer_ForceDisconnect (connectionId = 0x%x)\n", connectionId));

    if (!OI_INIT_FLAG_VALUE(FTP_SRV)) {
        return OI_STATUS_NOT_INITIALIZED;
    }
    if (connectionId != server->id) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    if (server->state < FTP_SERVER_CONNECTED) { 
        return OI_OBEX_NOT_CONNECTED;
    }
    return OI_OBEXSRV_ForceDisconnect(server->id);
}


OI_STATUS OI_FTPServer_Register(OI_OBEXSRV_AUTHENTICATION authentication,
                                OI_BOOL readOnly,
                                OI_BOOL allowBrowse,
                                OI_FTP_CONNECTION_IND connectInd,
                                OI_FTP_DISCONNECTION_IND disconnectInd,
                                const OI_FTP_FILESYS_FUNCTIONS *fileOperations,
                                const OI_SDP_STRINGS *strings,
                                OI_UINT8 *channelNumber)
{
    #ifdef OI_CODE
#error code not present
    #else
    static OI_OBEXSRV_CB_LIST CBList = {
        ServerConnectInd,
        ServerDisconnectInd,
        ServerGetInd,
        ServerPutInd,
         ServerSetPathInd,
        NULL
    };
    #endif /* OI_CODE */
    static const OI_BYTE uuid[OI_OBEX_UUID_SIZE] = OI_OBEX_FILE_BROWSING_UUID;
    OI_OBEX_BYTESEQ target = { OI_ARRAYSIZE(uuid), (OI_BYTE*) uuid };
    OI_SDPDB_SERVICE_RECORD srec;
    OI_STATUS status;

    OI_DBGTRACE(("OI_FTPServer_Register (authentication = %s, readOnly = %B, allowBrowse = %B, <*connectInd() = %x>, <*disconnectionInd() = %x>, <*fileOperations = %x>, <*strings = %x>, <*channelNumber = %x>)\n",
                 ((authentication == OI_OBEXSRV_AUTH_NONE) ? "NONE" : ((authentication == OI_OBEXSRV_AUTH_PASSWORD) ?
                                                                       "PASSWORD" : "USERID and PASSWORD")),
                 readOnly, allowBrowse,
                 connectInd, disconnectInd,
                 fileOperations, strings, channelNumber));

    ARGCHECK(fileOperations && connectInd && disconnectInd && strings);

    if (OI_INIT_FLAG_VALUE(FTP_SRV)) {
        return OI_STATUS_ALREADY_REGISTERED;
    }

    server = OI_Calloc(sizeof(FTP_SERVER));
    if (server == NULL) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    /*
     * Store the root folder. The current folder is reset to the root folder at
     * the start of each new connection.
     */
    server->connectInd = connectInd;
    server->disconnectInd = disconnectInd;
    server->fops = fileOperations;
    server->defaultAccess.allowBrowse = allowBrowse;
    server->defaultAccess.readOnly = readOnly;
    
    /*
     * Add security policy to our connection policy
     */
    server->connectPolicy.serviceUuid32 = OI_UUID_OBEXFileTransfer;
    server->connectPolicy.mustBeMaster = FALSE; 
    server->connectPolicy.flowspec = NULL;
    server->connectPolicy.powerSavingDisables = 0;
    server->connectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(server->connectPolicy.serviceUuid32) ;

    /*
     * There is no file open
     */
    server->file = NULL;

    /*
     * Try and get the preferred FTP channel number.
     */
    server->channel = OI_CONFIG_TABLE_GET(FTP_SRV)->rfcomm_channel_pref;
    status = OI_OBEXSRV_RegisterServer(&target, &CBList, authentication, &server->channel, &server->connectPolicy);
    if (!OI_SUCCESS(status) ) {
        goto ErrorExit;
    }

    /* 
     * Register service record
     */
    srec.Attributes = ServiceDescription;
    srec.NumAttributes = OI_ARRAYSIZE(ServiceDescription);
    srec.Strings = strings->attrs;
    srec.NumStrings = strings->num;

    status = OI_SDPDB_AddServiceRecord(&srec, &server->srecHandle);
    if (!OI_SUCCESS(status)) {
        goto ErrorExit;
    }

    /*
     * Create a protocol descriptor list that includes the rfcomm channel
     * number and add this attribute to the service record.
     */
    status = OI_SDP_UTILS_BuildPDLAttribute(OI_UUID_OBEX, server->channel, &server->sdpAttributes[0]);
    if (!OI_SUCCESS(status)) {
        goto ErrorExit;
    }
    /*
     * Set the protocol descriptor list attribute.
     */
    status = OI_SDPDB_SetAttributeList(server->srecHandle, server->sdpAttributes, OI_ARRAYSIZE(server->sdpAttributes));

    if (!OI_SUCCESS(status)) {
        goto ErrorExit;
    }

    OI_DEVMGR_RegisterServiceClass(OI_SERVICE_CLASS_OBJECT_TRANSFER);

    OI_DBGPRINT(("Registered File Transfer Profile on RFCOMM channel %d", server->channel));

    OI_INIT_FLAG_INCREMENT(FTP_SRV);

    /*
     * Return the channel number to the caller.
     */
    *channelNumber = server->channel;

    return OI_OK;
ErrorExit:

#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("OI_FTPServer_Register failed"));
#endif

    if (server) {
        if (server->channel) {
            (void) OI_OBEXSRV_DeregisterServer(server->channel);
        }
        if (server->srecHandle) {
            (void) OI_SDPDB_RemoveAttributeList(server->srecHandle, NULL, NULL);
            OI_SDPDB_FreeAttributeListElements(server->sdpAttributes, OI_ARRAYSIZE(server->sdpAttributes));
            (void) OI_SDPDB_RemoveServiceRecord(server->srecHandle);
        }
        OI_Free(server);
    }

    return status;
}


OI_STATUS OI_FTPServer_Deregister(OI_UINT8 channelNumber)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_FTPServer_Deregister (channelNumber = %d)\n", channelNumber));

    if (!OI_INIT_FLAG_VALUE(FTP_SRV)) {
        return OI_STATUS_NOT_REGISTERED;
    }

    status = OI_OBEXSRV_DeregisterServer(server->channel);
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Error deregistering FTP server"));
#endif
    } else {
        OI_DEVMGR_DeregisterServiceClass(OI_SERVICE_CLASS_OBJECT_TRANSFER);
        (void) OI_SDPDB_RemoveAttributeList(server->srecHandle, NULL, NULL);
        OI_SDPDB_FreeAttributeListElements(server->sdpAttributes, OI_ARRAYSIZE(server->sdpAttributes));
        (void) OI_SDPDB_RemoveServiceRecord(server->srecHandle);
        OI_Free(server);
        server = NULL;
        OI_INIT_FLAG_DECREMENT(FTP_SRV);
    }
    return status;
}

#ifdef OI_DEPRECATED
static OI_UINT8 deprecatedChannel;

/**
 * @deprecated
 */
OI_STATUS OI_FTPServer_Init(OI_OBEXSRV_AUTHENTICATION authentication,
                            OI_BOOL readOnly,
                            OI_BOOL allowBrowse,
                            OI_FTP_CONNECTION_IND connectInd,
                            OI_FTP_DISCONNECTION_IND disconnectInd,
                            const OI_FTP_FILESYS_FUNCTIONS *fileOperations,
                            const OI_SDP_STRINGS *strings)
{
    if (OI_INIT_FLAG_VALUE(OPP_SRV)) {
        return OI_STATUS_ALREADY_INITIALIZED;
    }

    return OI_FTPServer_Register(authentication,
                                 readOnly,
                                 allowBrowse,
                                 connectInd,
                                 disconnectInd,
                                 fileOperations,
                                 strings,
                                 &deprecatedChannel);
}


OI_STATUS OI_FTPServer_Reset(void)
{
    return OI_OK;
}


OI_STATUS OI_FTPServer_Terminate(void)
{
    if (!OI_INIT_FLAG_VALUE(OPP_SRV)) {
        return OI_STATUS_NOT_INITIALIZED;
    } else {
        return OI_FTPServer_Deregister(deprecatedChannel);
    }
}
#endif /* OI_DEPRECATED */

OI_STATUS OI_FTPServer_GetServiceRecord(OI_UINT32 *handle)
{
    if (!handle) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    OI_CHECK_INIT(FTP_SRV);

    *handle = server->srecHandle;
    return OI_OK;
}

