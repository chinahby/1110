/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/ftp_client.c#3 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-11-13   ns  For FTP disconnect remove the already not connected check
  2009-01-14   gs  Added support for sending object size info in fops->write()

*    #19       27 Apr 2007            GS
* Updated FileWriteCfm() to fix issue in canceling operation if write
* fails during FTP GET.
*
*    #18       28 Feb 2007            GS
* Updated ClientGetFolderData() to not call folderListCB if Continue
* response has no data.
*
*    #17       28 Feb 2007            MH
* Full access info added to challenge call back.
*
*    #16       18 Oct 2006            PR
* Merged OI Code v3.0.14 and added new code to handle Abort.
*
*    #15       10 Oct 2006            GS
* Updated to use modified OI_FTP_OPEN function prototype.
*
*    #14       19 Sept 2006           MH
* Changed OI client connect command
*
*    #13       18 Jul 2006            DM
* Aborting the Client Get operation if write to local file system fails
*
*    #11-12    10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #10       23 Jun 2006            JH
* Modified to work with updated OBEX code which supports JSR82.
*
*    #9        18 Nov 2005            GS
* Fixing FileCloseCB() again so that appropriate status code is sent only 
* in CLIENT_STATE_GETTING case.
*
*    #8        17 Nov 2005            GS
* Updated FileCloseCB() to return appropriate status code in all calls to
* file close callback.
*
*    #7        22 Jul 2005            JH
* Fixed ClientGetFolderData() to avoid calling folderListCB twice with no data.
*
*    #6        08 Jul 2005            JH
* Added status parameter to all calls to the file close callback.
*
*    #5        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
*    #4        10 Nov 2004            JH
* Modified FileReadCfm such that OI_OBEXCLI_Put will not return with an
*  invalid parameter error.
*
*    #3        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*             
===========================================================================*/
/**
@file
@internal
File Transfer Profile client
*/

/**********************************************************************************
  $Revision: #3 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#define __OI_MODULE__ OI_MODULE_FTP_CLI

#include "oi_bt_assigned_nos.h"
#include "oi_ftp_utils.h"
#include "oi_ftp_client.h"
#include "oi_dispatch.h"
#include "oi_std_utils.h"
#include "oi_memmgr.h"
#include "oi_ftp_sys.h"
#include "oi_obexcli.h"
#include "oi_argcheck.h"
#include "oi_assert.h"
#include "oi_securitydb.h"

#include "oi_debug.h"
#include "oi_assert.h"

#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"


/**
 * These are the various client connection states.
 */

typedef enum {
    CLIENT_STATE_DISCONNECTED,
    CLIENT_STATE_AUTHENTICATING,
    CLIENT_STATE_CONNECTED,
    CLIENT_STATE_PUTTING,
    CLIENT_STATE_GETTING,
    CLIENT_STATE_DELETING,
    CLIENT_STATE_LISTING_FOLDER,
    CLIENT_STATE_SETTING_PATH,
    CLIENT_STATE_PUTTING_TREE,
    CLIENT_STATE_GETTING_TREE,
    CLIENT_STATE_ABORTING
} FTP_CLIENT_STATE;

/**
 * Struct for an FTP client connection.
 */

typedef struct {

    OI_FTP_HANDLE file;                               /**< current open file */
    const OI_FTP_FILESYS_FUNCTIONS *fops;             /**< file operations */
    OI_OBEX_UNICODE remoteName;                       /**< name of remote file or folder */
    OI_UINT16 maxRead;                                /**< maximum read for the current connection */
    FTP_CLIENT_STATE state;                           /**< state of this ftp conntection */
    OI_BOOL final;                                    /**< OBEX operation is completing */
    OI_OBEXCLI_CONNECTION_HANDLE id;                  /**< The underlying OBEX client connection */
    OI_FTP_CLIENT_EVENT_CB eventCB;                   /**< Callback function to report operation completion */
    OI_FTP_CLIENT_AUTHENTICATION_CB authenticationCB; /**< Callback function to get authentication information */
    OI_FTP_CLIENT_FOLDER_LIST_CB folderListCB;        /**< Callback function to report folder listing info */
    OI_FTP_CLIENT_CANCEL_CFM cancelCfm;               /**< Callback function to report the completion of an operation cancellation */
    OI_FTP_PARSE_INFO *path;                          /**< for folder push and pull */
    OI_UINT8 pathIndex;                               /**< length of current path array */
    OI_UINT8 pathDepth;                               /**< current local path depth */
    OI_UINT8 maxPathDepth;                            /**< maximum path depth (configurable) */
    OI_STATUS treeError;                              /**< propogates error from folder push/pull */
    OI_CONNECT_POLICY connectPolicy;                  /**< Security policy for this connection */
    DISPATCH_CB_HANDLE pendingClose;                  /**< Close operation that has been registered with the dispatcher */
    DISPATCH_CB_HANDLE pendingTreeDone;               /**< Tree_Done operation that has been registered with the dispatcher */
    OI_BOOL fileOpCfmPending;                         /**< Waiting for file operation code to call our cfm function */
    OI_BOOL disconnected;                             /**< Received disconnect indication, waiting to call event callback. */
#ifndef OI_CODE
    OI_BOOL treePutGet;                               /**< A Tree PUT or a GET operation has started. */
    OI_UINT32 objSize;
#endif
} FTP_CLIENT;


static FTP_CLIENT *client;

#define IS_CLIENT_CONNECTED   (OI_INIT_FLAG_VALUE(FTP_CLI) && (client->state >= CLIENT_STATE_CONNECTED))



#define bool2str(b) ((b) ? "TRUE" : "FALSE")

#if defined(OI_DEBUG)
#error code not present
#else

#define ClientStateText(state) ""

#define setState(newState)                      \
    do {                                        \
        client->state = (newState);             \
    } while (0)

#endif

#define VALIDATE_CFM_STATE(_fn, _state_test)                            \
    do {                                                                \
        if (!OI_INIT_FLAG_VALUE(FTP_CLI) ||                             \
            !(_state_test) ||                                           \
            !client->fileOpCfmPending) {                                \
            OI_LOG_ERROR(("%s() called at wrong time (state %s valid? %s \
                          confirm pending? %s).\n", \
                          (_fn), ClientStateText(client->state),        \
                          bool2str(_state_test),                        \
                          bool2str(client->fileOpCfmPending)));         \
            return;                                                     \
        }                                                               \
        client->fileOpCfmPending = FALSE;                               \
        if (client->disconnected) {                                     \
            OI_DBGPRINT2(("Closing file 0x%x\n", client->file));        \
            FileClose(OI_OBEX_NOT_CONNECTED);                           \
            return;                                                     \
        }                                                               \
    } while (0)




/** ************************************************************************
 *
 *                   Forward declarations
 *
 * *************************************************************************/

static void FileClose(OI_STATUS status);

static void ClientGetRcvData(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                             OI_OBEX_HEADER_LIST *rspHeaders,
                             OI_STATUS rcvStatus);

static void ClientPutCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                         OI_OBEX_HEADER_LIST *rspHeaders,
                         OI_STATUS status);

static void TreePut(DISPATCH_ARG *darg);

static void TreePut_BuildList(DISPATCH_ARG *darg);

static void TreeGet(void);

static void TreeGet_BuildList(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                              OI_OBEX_HEADER_LIST *rspHeaders,
                              OI_STATUS obexStatus);

/* ************************************************************************ */


static OI_STATUS CopyName(OI_OBEX_UNICODE *to,
                          OI_OBEX_UNICODE *from)
{
    OI_UINT16 size;

    OI_ASSERT(to->str == NULL);

    if ((from == NULL) || (from->str == NULL) || (from->str[from->len -1] != 0)) {
        OI_DBGPRINT(("CopyName: Invalid \"from\" name:  from = %x   from->str = \"%?S\"   nul terminator? %B\n",
                     from, from ? from->str : (OI_CHAR16*)"\0", from ? from->len : 0,
                     from ? (from->str[from->len - 1] == 0) : FALSE));
        return OI_STATUS_INVALID_PARAMETERS;
    }
    /*
     * An OBEX unicode string must be null terminated.
     */
    if (from->str[from->len -1] != 0) {
        OI_DBGPRINT(("OBEX unicode string must be null-terminated\n"));
        return OI_STATUS_INVALID_PARAMETERS;
    }
    size = from->len * sizeof(OI_CHAR16);
    to->str = OI_Malloc(size);
    if (to->str == NULL) {
        return OI_STATUS_OUT_OF_MEMORY;
    } else {
        OI_MemCopy(to->str, from->str, size);
        to->len = from->len;
    }
    return OI_OK;
}


static void ClientConnectCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                             OI_BOOL readOnly,
#ifndef OI_CODE
                             OI_OBEX_HEADER_LIST *hdrs, /* ignored */
#endif
                             OI_STATUS status)
{
    OI_FTP_CLIENT_EVENT_CB  clientCb;

    OI_DBGTRACE(("ClientConnectCfm (connectionId = 0x%x, readOnly = %s, status = %!)\n",
                 connectionId, bool2str(readOnly), status));

    OI_ASSERT(connectionId == client->id);

    if (OI_SUCCESS(status)) {
        OI_DBGPRINT2(("FTP client connected %s\n", readOnly ? " read-only" : ""));
        setState(CLIENT_STATE_CONNECTED);
        /*
         * Establish the best read size for file read operations.
         */
        client->maxRead = OI_OBEXCLI_OptimalBodyHeaderSize(connectionId);
    }

    /*
     * If the attempt failed, the client record needs to be freed before invoking the
     * callback because the application might call OI_FTPClient_Connect() from the
     * event callback.  Therefore, we must save the callback address.
     */
    clientCb = client->eventCB;

    /*
     * Clean up if the connection attempt failed.
     */
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT2(("FTP client connect failed %!\n", status));
        OI_Free(client);
        client = NULL;
        OI_INIT_FLAG_PUT_FLAG(FALSE, FTP_CLI);
    }

    clientCb(connectionId, OI_FTP_CLIENT_CONNECTED, status);
}

#ifdef OI_CODE
#error code not present
#else
static void ClientDisconnectInd(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                OI_OBEX_HEADER_LIST *hdrs) /* ignored */
#endif
{
    OI_DBGTRACE(("ClientDisconnectInd (connectionId = 0x%x)\n", connectionId));

    /*
     * record the disconnect event for subsequent processes
     */
    client->disconnected = TRUE;

    /*
     * If there was a close pending we need to make sure it is called now
     * because we are about to free the client.
     */
    if (client->pendingTreeDone) {
        OI_DBGPRINT2(("Disconnect: calling Tree Done immediately\n"));
        OI_Dispatch_CallFunc(client->pendingTreeDone);
        return;
    } else if (client->pendingClose) {
        OI_DBGPRINT2(("Disconnect: calling close immediately\n"));
        OI_Dispatch_CallFunc(client->pendingClose);
        return;
    }

    if (client->fileOpCfmPending) {
        /*
         * We're waiting for a file op confirm callback, just have to keep
         * waiting
         */
    } else {
        /*
         * Status to be reported to the file op code and app depends on
         * whether we're idle or getting/putting.
         */
        OI_DBGPRINT2(("Closing file 0x%x\n", client->file));
        FileClose(client->state > CLIENT_STATE_CONNECTED ? OI_OBEX_NOT_CONNECTED : OI_OK);
    }
}


static void ClientAuthenticationInd(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
#ifndef OI_CODE
                                    OI_BOOL fullAccess,
#endif
                                    OI_BOOL userIdRequired)
{
    OI_DBGTRACE(("ClientAuthenticationInd (connectionId = 0x%x, userIdRequired = %s)\n",
                 connectionId, bool2str(userIdRequired)));

    OI_ASSERT(connectionId == client->id);

    setState(CLIENT_STATE_AUTHENTICATING);
    client->authenticationCB(connectionId, userIdRequired);
}


/**
 * Connect to remote OBEX file transfer profile server.
 */

OI_STATUS OI_FTPClient_Connect(OI_BD_ADDR *addr,
                               OI_UINT channel,
                               OI_FTP_CLIENT_CONNECTION_HANDLE *connectionId,
                               OI_FTP_CLIENT_EVENT_CB eventCB,
                               OI_FTP_CLIENT_AUTHENTICATION_CB authenticationCB,
                               const OI_FTP_FILESYS_FUNCTIONS *fileOperations)
{
    static const OI_BYTE target[OI_OBEX_UUID_SIZE] = OI_OBEX_FILE_BROWSING_UUID;
    OI_OBEX_HEADER_LIST hdrList;
    OI_OBEX_HEADER hdr;
    OI_STATUS status;

    OI_DBGTRACE(("OI_FTPClient_Connect (addr = %:, channel = %d, <*connectionId = %x>,\
                 <*eventCB() = %x>, <*authenticationCB() = %x>, <*fileOperations = %x>)\n",\
                 addr, channel, connectionId, eventCB, authenticationCB, fileOperations));

    ARGCHECK(eventCB && authenticationCB);

    /* 
     * Current implementation only allows one FTP connection at a time. 
     */
    if (OI_INIT_FLAG_VALUE(FTP_CLI)) {
        return OI_STATUS_ALREADY_CONNECTED;
    }

    client = OI_Calloc(sizeof(FTP_CLIENT));
    if (client == NULL) {
        return OI_STATUS_NO_RESOURCES;
    }

    client->eventCB = eventCB;
    client->authenticationCB = authenticationCB;
    client->fops = fileOperations;

    /*
     * Add security policy to our connection policy
     */
    client->connectPolicy.serviceUuid32 = OI_UUID_OBEXFileTransfer;
    client->connectPolicy.mustBeMaster = FALSE; 
    client->connectPolicy.flowspec = NULL;
    client->connectPolicy.powerSavingDisables = 0;
    client->connectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(client->connectPolicy.serviceUuid32) ;
    /*
     * Get maximum recursion depth from config file. Zero means recursive folder put/get
     * is not supported.
     */
    client->maxPathDepth = OI_CONFIG_TABLE_GET(FTP_CLI)->max_folder_tree_depth;
    /*
     * Target header specifies the OBEX file browsing service.
     */
    hdr.id = OI_OBEX_HDR_TARGET;
    hdr.val.body.data = (OI_BYTE*) target;
    hdr.val.body.len = OI_OBEX_UUID_SIZE;

    hdrList.list = &hdr;
    hdrList.count = 1;
    status = OI_OBEXCLI_Connect(addr,
                                (OI_UINT8) channel,
                                &hdrList,
                                ClientConnectCfm,
                                ClientDisconnectInd,
                                ClientAuthenticationInd,
#ifndef OI_CODE
                                NULL, /*Challenge Response Call back */
#endif
                                connectionId,
                                &client->connectPolicy);

    if (OI_SUCCESS(status)) {
        client->id = *connectionId;
        /* 
         * Set our initialization flag TRUE - we're initialized 
         */
        OI_INIT_FLAG_PUT_FLAG(TRUE, FTP_CLI) ;
    } else {
        OI_Free(client);
        OI_DBGPRINT(("OI_OBEXCLI_Connect failed (%!)\n", status));
    }
    return status;
}


OI_STATUS OI_FTPClient_Disconnect(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_FTPClient_Disconnect (connectionId = 0x%x)\n", connectionId));


#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXCLI_Disconnect(client->id, NULL);
#endif
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("OI_OBEXCLI_Disconnect failed (%!)\n", status));
    }
    return status;
}


OI_STATUS OI_FTPClient_AuthenticationRsp(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                                         const OI_BYTE *userId,
                                         OI_UINT8 userIdLen,
                                         const OI_CHAR *password)
{
    OI_DBGTRACE(("OI_FTPClient_AuthenticationRsp (connectionId = 0x%x, userId = \"%s\",\
                 userIdLen = %d, password = \"%s\")\n",
                 connectionId, (userId ? userId : (const OI_BYTE*)"<null>"), userIdLen, password));


    if (OI_INIT_FLAG_VALUE(FTP_CLI) && (client->state == CLIENT_STATE_AUTHENTICATING)) {
        return OI_OBEXCLI_AuthenticationResponse(client->id, userId, userIdLen, password);
    } else {
        return OI_OBEX_INVALID_OPERATION;
    }
}


static void UnwindSetpathCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
#ifndef OI_CODE
                       OI_OBEX_HEADER_LIST *hdrs, /* ignored */
#endif
                       OI_STATUS status)
{
    OI_DBGTRACE(("UnwindSetpathCfm (connectionId = 0x%x, status = %!)\n",
                 connectionId, status));
    /* empty */
}


static void UnwindCfm(OI_STATUS cfmStatus,
                      OI_FTP_CONNECTION ftpConnection)
{
    OI_STATUS status;

    OI_DBGTRACE(("UnwindCfm (cfmStatus = %!, ftpConnection = 0x%x)\n",
                 cfmStatus, ftpConnection));

    /* Set the remote server to the <root> directory too. */
#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXCLI_SetPath(client->id, NULL, TRUE, FALSE, NULL, UnwindSetpathCfm);
#endif
    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("Failed to set server's path to <root>: %!\n", status));
    }
}


/*
 * Complete a TreePut or TreeGet operation.
 */
static void Tree_DoneCB(DISPATCH_ARG *darg)
{
    OI_STATUS status = Dispatch_GetArg(darg, OI_STATUS);
    OI_FTP_CLIENT_EVENT event;
    OI_UINT i;

    OI_DBGTRACE(("Tree_DoneCB (<*darg = %x>)\n", darg));
    OI_DBGPRINT2(("darg => status: %!\n", status));

    client->pendingTreeDone = 0;

    OI_ASSERT((client->state == CLIENT_STATE_GETTING_TREE) || (client->state == CLIENT_STATE_PUTTING_TREE));

    if (OI_SUCCESS(status)) {
        OI_DBGPRINT(("Tree operation complete\n"));
        OI_ASSERT(client->pathDepth == 0);

        OI_Free(client->path);
        client->path = NULL;
        /*
         * Let application know we are done.
         */
        if (client->state == CLIENT_STATE_GETTING_TREE) {
            event = OI_FTP_CLIENT_TREE_GET_COMPLETE;
        } else {
            event = OI_FTP_CLIENT_TREE_PUT_COMPLETE;
        }
        setState(CLIENT_STATE_CONNECTED);
        client->eventCB(client->id, event, OI_OK);
    } else {
        OI_DBGPRINT(("Tree operation failed %!\n", status));
        OI_DBGPRINT2(("Closing file 0x%x\n", client->file));
        FileClose(status);
        if (client->file) {
            OI_DBGPRINT2(("Calling file op CLOSE handle = %x, status = %!\n", client->file, status));
#ifdef OI_CODE
#error code not present
#else
            client->fops->close(client->file, (OI_FTP_CONNECTION)client->id, OI_OK);
#endif
            client->file = 0;
        }
        for (i = 0; i <= client->pathIndex; ++i) {
            OI_FreeIf(&client->path[i].name.str);
            OI_FreeIf(&client->path[i].xmlStream);
        }
        OI_Free(client->path);
        client->path = NULL;
        client->treeError = status;
        /*
         * Pop the directory stack back to root.
         */
        OI_DBGPRINT2(("Calling file op SET_FOLDER dir = <root>\n"));
        (void) client->fops->setFolder(NULL, 0, FALSE, UnwindCfm, (OI_FTP_CONNECTION)client->id);
        /*
         * Let application know we are done.
         */
        if (client->state == CLIENT_STATE_GETTING_TREE) {
            event = OI_FTP_CLIENT_TREE_GET_COMPLETE;
        } else {
            event = OI_FTP_CLIENT_TREE_PUT_COMPLETE;
        }
        setState(CLIENT_STATE_CONNECTED);
        client->eventCB(client->id, event, status);
    }
}

static void Tree_Done(OI_STATUS status)
{
    DISPATCH_ARG darg;

    OI_DBGTRACE(("Tree_Done (status = %!)\n", status));
#ifndef OI_CODE
    client->treePutGet = FALSE;
#endif
    Dispatch_SetArg(darg, status);
    OI_Dispatch_RegisterFunc(Tree_DoneCB, &darg, &client->pendingTreeDone);
}


static void FileCloseCB(DISPATCH_ARG *darg)
{
    OI_STATUS status = Dispatch_GetArg(darg, OI_STATUS);
    FTP_CLIENT_STATE snapState;
    OI_FTP_HANDLE file;

    OI_DBGTRACE(("FileCloseCB (<*darg = %x>)\n", darg));
    OI_DBGPRINT2(("darg => status: %!\n", status));

    snapState = client->state;

    client->pendingClose = 0;
#ifndef OI_CODE
    client->objSize = 0;
#endif
    if ((client->state != CLIENT_STATE_GETTING_TREE) &&
        (client->state != CLIENT_STATE_PUTTING_TREE)) {
        if (client->disconnected) {
            setState(CLIENT_STATE_DISCONNECTED);
        } else if (client->state > CLIENT_STATE_CONNECTED) {
            setState(CLIENT_STATE_CONNECTED);
        }
    }

    if (client->remoteName.str) {
        OI_Free(client->remoteName.str);
        client->remoteName.str = NULL;
    }

    file = client->file;
    client->file = 0;

    switch (snapState) {
        case CLIENT_STATE_PUTTING:
            OI_DBGPRINT2(("Calling file op CLOSE handle = %x, status = %!\n", file, status));
#ifdef OI_CODE
#error code not present
#else
            client->fops->close(file, (OI_FTP_CONNECTION)client->id, OI_OK);
#endif
            client->eventCB(client->id, OI_FTP_CLIENT_PUT_COMPLETE, status);
            break;
        case CLIENT_STATE_GETTING:
            OI_DBGPRINT2(("Calling file op CLOSE handle = %x, status = %!\n", file, status));
#ifdef OI_CODE
#error code not present
#else
            client->fops->close(file, (OI_FTP_CONNECTION)client->id, status);
#endif
            client->eventCB(client->id, OI_FTP_CLIENT_GET_COMPLETE, status);
            break;
        case CLIENT_STATE_PUTTING_TREE:
            OI_DBGPRINT2(("Calling file op CLOSE handle = %x, status = %!\n", file, status));
#ifdef OI_CODE
#error code not present
#else
            client->fops->close(file, (OI_FTP_CONNECTION)client->id, OI_OK);
#endif
            if (OI_SUCCESS(status)) {
                TreePut(NULL);
            } else {
                Tree_Done(status);
            }
            return;
        case CLIENT_STATE_GETTING_TREE:
            OI_DBGPRINT2(("Calling file op CLOSE handle = %x, status = %!\n", file, status));
#ifdef OI_CODE
#error code not present
#else
            client->fops->close(file, (OI_FTP_CONNECTION)client->id, OI_OK);
#endif
            if (OI_SUCCESS(status)) {
                TreeGet();
            } else {
                Tree_Done(status);
            }
            return;
        default:
            break;
    }

    /* If disconnected, tell the application and re-initialize ourselves */
    if (client->disconnected) {
        OI_FTP_CONNECTION id = (OI_FTP_CONNECTION)client->id;
        OI_FTP_CLIENT_EVENT_CB eventCB = client->eventCB;

        OI_Free(client);
        client = NULL;

        eventCB(id, OI_FTP_CLIENT_DISCONNECT, OI_OK);

        OI_INIT_FLAG_PUT_FLAG(FALSE, FTP_CLI);
    }
}


/*
 * Put a callback function on the dispatcher to complete the close.
 */
static void FileClose(OI_STATUS status)
{
    DISPATCH_ARG darg;

    OI_DBGTRACE(("FileClose (status = %!)\n", status));

    Dispatch_SetArg(darg, status);
    (void) OI_Dispatch_RegisterFunc(FileCloseCB, &darg, &client->pendingClose);
}



static void FileReadCfm(OI_FTP_HANDLE handle,
                        OI_BYTE *data,
                        OI_UINT16 len,
                        OI_STATUS status,
                        OI_FTP_CONNECTION ftpConnection)
{
    OI_OBEX_HEADER hdr;
    OI_OBEX_HEADER_LIST hdrList;

    OI_DBGTRACE(("FileReadCfm (handle = 0x%x, <*data = %x>, len = %d, status = %!, ftpConnection = 0x%x)\n",
                 handle, data, len, status, ftpConnection));

    VALIDATE_CFM_STATE("FileReadCfm", ((client->state == CLIENT_STATE_PUTTING) ||
                                       (client->state == CLIENT_STATE_PUTTING_TREE)));

    if (!OI_SUCCESS(status)) {
        if (status == OI_STATUS_END_OF_FILE) {
            client->final = TRUE;
            status = OI_OK;
        }
    }
    if (OI_SUCCESS(status)) {
        hdr.val.body.data = data;
        hdr.val.body.len = len;
        hdr.id = (client->final) ? OI_OBEX_HDR_END_OF_BODY : OI_OBEX_HDR_BODY;
        hdrList.list = &hdr;
        hdrList.count = 1;
        status = OI_OBEXCLI_Put(client->id, &hdrList, ClientPutCfm, client->final ? OI_OK : OI_OBEX_CONTINUE);
    } else {
#ifdef OI_CODE                        
#error code not present
#else
        OI_LOG_ERROR(("File read returned"));
#endif /* OI_CODE */
        status = OI_OBEXCLI_Put(client->id, NULL, ClientPutCfm, status);
    }
    /*
     * Close the file if there was an error.
     */
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Put failed"));
#endif
        OI_DBGPRINT2(("Closing file 0x%x\n", client->file));
        FileClose(status);
    }
}


static void ClientPutCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                         OI_OBEX_HEADER_LIST *rspHeaders,
                         OI_STATUS status)
{
    OI_STATUS result;

    OI_DBGTRACE(("ClientPutCfm (connectionId = 0x%x, <*rspHeaders = %x>, status = %!)\n",
                 connectionId, rspHeaders, status));

    if (status == OI_OBEX_CONTINUE) {
        /*
         * Check that we expect this response
         */
        if (client->final) {
            OI_LOG_ERROR(("OBEX protocol error - did not expect continue\n"));
            result = OI_OBEXCLI_Abort(client->id, NULL);
            if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("Abort operation failed"));
#endif
            }
            status = OI_OBEX_ERROR;
        } else {
            client->fileOpCfmPending = TRUE;
            OI_DBGPRINT2(("Calling file op READ handle = %x, size = %d\n", client->file, client->maxRead));
            status = client->fops->read(client->file, client->maxRead, FileReadCfm, (OI_FTP_CONNECTION)client->id);
            if (OI_SUCCESS(status)) {
                return;
            }
        }
    }
    /*
     * We got here because the put operation is complete or we got an error.
     * In either case we need to close the file.
     */
    client->fileOpCfmPending = FALSE;
    OI_DBGPRINT2(("Closing file 0x%x\n", client->file));
    FileClose(status);
}


/*
 * Called when a file open operation completes.
 *
 */
static void FileOpenPutCfm(OI_FTP_HANDLE handle,
                           OI_UINT32 size,
                           OI_STATUS status,
                           OI_FTP_CONNECTION ftpConnection)
{
    OI_OBEX_HEADER hdrs[2];
    OI_OBEX_HEADER_LIST hdrList;

    OI_DBGTRACE(("FileOpenPutCfm (handle = 0x%x, size = %d, status = %!, ftpConnection = 0x%x)\n",
                 handle, size, status, ftpConnection));

    VALIDATE_CFM_STATE("FileOpenPutCfm", ((client->state == CLIENT_STATE_PUTTING) ||
                                          (client->state == CLIENT_STATE_PUTTING_TREE)));

    if (!OI_SUCCESS(status)) {
        OI_Free(client->remoteName.str);
        client->remoteName.str = NULL;
        setState(CLIENT_STATE_CONNECTED);
        client->eventCB(client->id, OI_FTP_CLIENT_PUT_COMPLETE, OI_OBEX_ACCESS_DENIED);
    } else {
        client->file = handle;
        hdrs[0].id = OI_OBEX_HDR_NAME;
        hdrs[0].val.name = client->remoteName;
        hdrs[1].id = OI_OBEX_HDR_LENGTH;
        hdrs[1].val.length = size;
        hdrList.list = hdrs;
        hdrList.count = 2;
        status = OI_OBEXCLI_Put(client->id, &hdrList, ClientPutCfm, OI_OBEX_CONTINUE);
        if (!OI_SUCCESS(status)) {
            OI_DBGPRINT2(("Closing file 0x%x\n", client->file));
            FileClose(status);
        }
    }
}


static OI_STATUS PutFile(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                         OI_OBEX_UNICODE *localName,
                         OI_OBEX_UNICODE *remoteName)
{
    OI_STATUS status;

    OI_DBGPRINT2(("FTP client PutFile local: %S -> remote: %S\n",
                  localName->str, (remoteName ? remoteName->str : localName->str)));

    /*
     * If remoteName is not specified this means the local and remote file
     * names are the same.
     */
    if (remoteName == NULL) {
        remoteName = localName;
    }
    status = CopyName(&client->remoteName, remoteName);
    if (!OI_SUCCESS(status)) {
        return status;
    }
    client->final = FALSE;
    client->fileOpCfmPending = TRUE;
    OI_DBGPRINT2(("Calling file op OPEN localName = %S, mode = READ\n", localName->str));
#ifdef OI_CODE
#error code not present
#else
    status = client->fops->open(localName, OI_FTP_MODE_READ, 0, FileOpenPutCfm, (OI_FTP_CONNECTION)client->id);
#endif
    if (!OI_SUCCESS(status)) {
        client->fileOpCfmPending = FALSE;
        OI_Free(client->remoteName.str);
        client->remoteName.str = NULL;
    }
    return status;
}


OI_STATUS OI_FTPClient_Put(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                           OI_OBEX_UNICODE *localName,
                           OI_OBEX_UNICODE *remoteName)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_FTPClient_Put (connectionId = 0x%x, <*localName = %x>, <*remoteName = %x>)\n",
                 connectionId, localName, remoteName));

    ARGCHECK((localName != NULL) && (localName->len > 0));

    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("Not connected\n"));
        return OI_OBEX_NOT_CONNECTED;
    }
    if (client->state > CLIENT_STATE_CONNECTED) {
        OI_DBGPRINT(("Operation already in progress\n"));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    setState(CLIENT_STATE_PUTTING);
    status = PutFile(connectionId, localName, remoteName);
    if (!OI_SUCCESS(status)) {
        setState(CLIENT_STATE_CONNECTED);
    }
    return status;
}


static void ClientDeleteCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                            OI_OBEX_HEADER_LIST *rspHeaders,
                            OI_STATUS status)
{
    OI_DBGTRACE(("ClientDeleteCfm (connectionId = 0x%x, <*rspHeaders = %x>, status = %!)\n",
                 connectionId, rspHeaders, status));

    setState(CLIENT_STATE_CONNECTED);
    client->eventCB(connectionId, OI_FTP_CLIENT_DELETE_COMPLETE, status);
}


/*
 * Per the OBEX spec, DELETE is actually implemented as a PUT command with no
 * body header.
 */

OI_STATUS OI_FTPClient_Delete(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                              OI_OBEX_UNICODE *name)
{
    OI_STATUS status;
    OI_OBEX_HEADER hdrs[1];
    OI_OBEX_HEADER_LIST hdrList;

    OI_DBGTRACE(("OI_FTPClient_Delete (connectionId = 0x%x, <*name = %x>)\n",
                 connectionId, name));

    ARGCHECK((name != NULL) && (name->len > 0));

    OI_DBGPRINT2(("FTP client delete file: %S\n", name->str));

    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("Not connected\n"));
        return OI_OBEX_NOT_CONNECTED;
    }
    if (client->state > CLIENT_STATE_CONNECTED) {
        OI_DBGPRINT(("Operation already in progress\n"));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    /*
     * First header is the unicode name for the file or folder to delete.
     */
    hdrs[0].id = OI_OBEX_HDR_NAME;
    hdrs[0].val.name = *name;
    hdrList.list = hdrs;
    hdrList.count = 1;
    status = OI_OBEXCLI_Put(client->id, &hdrList, ClientDeleteCfm, OI_OK);
    if (OI_SUCCESS(status)) {
        setState(CLIENT_STATE_DELETING);
    }
    return status;
}

static void GetAbortCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId)
{
    OI_DBGTRACE(("GetAbortCfm (connectionId = 0x%x)\n", connectionId));
    OI_DBGPRINT2(("Closing file 0x%x\n", client->file));
    FileClose(OI_OBEX_FILEOP_ERROR);
}

static void FileWriteCfm(OI_FTP_HANDLE handle,
                         OI_STATUS status,
                         OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("FileWriteCfm (handle = 0x%x, status = %!, ftpConnection 0x%x)\n",
                 handle, status, ftpConnection));

    VALIDATE_CFM_STATE("FileWriteCfm", ((client->state == CLIENT_STATE_GETTING) ||
                                        (client->state == CLIENT_STATE_GETTING_TREE)));

    if (!client->final) {
        if (!OI_SUCCESS(status)) {
            OI_DBGPRINT(("File write returned error %!\n", status));

            /*
             * Let OBEX know we are terminating the GET because of an error.
             */
            status = OI_OBEXCLI_Abort(client->id, GetAbortCfm);
#ifndef OI_CODE
            if (status == OI_STATUS_PENDING) {
                /* In good state to abort.  Send Abort now */
                BT_MSG_DEBUG("FileWriteCfm: File write error.. Aborting", 0, 0, 0); 
                status = OI_OBEXCLI_Get(client->id, NULL, ClientGetRcvData, TRUE);
            }
#endif
    } else {
            status = OI_OBEXCLI_Get(client->id, NULL, ClientGetRcvData, TRUE);
            if (OI_SUCCESS(status)) {
                return;
            }
            OI_DBGPRINT(("OI_OBEXCLI_Get returned error %!\n", status));
        }
    }

    if (client->final || !OI_SUCCESS(status)) {
        OI_DBGPRINT2(("Closing file 0x%x\n", client->file));
        FileClose(status);
    }
}


static void ClientGetRcvData(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                             OI_OBEX_HEADER_LIST *rspHeaders,
                             OI_STATUS rcvStatus)
{
    OI_STATUS status;
    OI_OBEX_HEADER *hdr;
    OI_OBEX_HEADER *bodyHdr = NULL;
    OI_UINT i;

    OI_DBGTRACE(("ClientGetRcvData (connectionId = 0x%x, <*rspHeaders = %x>, rcvStatus = %!)\n",
                 connectionId, rspHeaders, rcvStatus));

    if ((client->state != CLIENT_STATE_GETTING) && (client->state != CLIENT_STATE_GETTING_TREE)) {
        OI_DBGPRINT(("ClientGetRcvData not currently getting\n"));
        return;
    }

    if (!OI_SUCCESS(rcvStatus) && (rcvStatus != OI_OBEX_CONTINUE)) {
        status = rcvStatus;
    } else {
        /*
         * We are only interested in certain headers.
         */
        for (i = 0; i < rspHeaders->count; ++i) {
            hdr = &rspHeaders->list[i];
            switch (hdr->id) {
                case OI_OBEX_HDR_BODY:
                case OI_OBEX_HDR_END_OF_BODY:
                    bodyHdr = hdr;
                    break;
                case OI_OBEX_HDR_LENGTH:
#ifndef OI_CODE
                    client->objSize = hdr->val.length;
#endif
                    OI_DBGPRINT2(("Receiving file of length %d\n", hdr->val.length));
                    break;
                default:
                    OI_DBGPRINT2(("Received unexpected obex header %2x\n", hdr->id));
                    break;
            }
        }

        client->final = (rcvStatus == OI_OK);

        client->fileOpCfmPending = TRUE;

#ifdef OI_CODE
#error code not present
#else
#ifdef FEATURE_BT_OBEX_DBL_BUF_WRITE
        /* Allow zero length header to go to write function. */
        if (bodyHdr == NULL) {
            /*
             * Nothing to write to file this time.
             */
            FileWriteCfm(client->file, OI_OK, client->id);
            return;
        }
#else
        if ((bodyHdr == NULL) || (bodyHdr->val.body.len == 0)) {
            /*
             * Nothing to write to file this time.
             */
            FileWriteCfm(client->file, OI_OK, client->id);
            return;
        }
#endif /* FEATURE_BT_OBEX_DBL_BUF_WRITE */

#endif

        OI_DBGPRINT2(("Calling file op WRITE handle = %x, size = %d\n", client->file, bodyHdr->val.body.len));
#ifdef OI_CODE
#error code not present
#else
        status = client->fops->write(client->file, bodyHdr->val.body.data, bodyHdr->val.body.len, FileWriteCfm, (OI_FTP_CONNECTION)client->id, client->objSize);
#endif
    }
    /*
     * An error - close the file.
     */
    if (!OI_SUCCESS(status)) {
        client->fileOpCfmPending = FALSE;
        OI_DBGPRINT2(("Closing file 0x%x\n", client->file));
        FileClose(status);
    }
}


static void FileOpenGetCfm(OI_FTP_HANDLE handle,
                           OI_UINT32 size,
                           OI_STATUS status,
                           OI_FTP_CONNECTION ftpConnection)
{
    OI_OBEX_HEADER hdr;
    OI_OBEX_HEADER_LIST hdrList;

    OI_DBGTRACE(("FileOpenGetCfm (handle = 0x%x, size = %d, status = %!, ftpConnection = 0x%x)\n",
                 handle, size, status, ftpConnection));

    VALIDATE_CFM_STATE("FileOpenGetCfm", ((client->state == CLIENT_STATE_GETTING) ||
                                          (client->state == CLIENT_STATE_GETTING_TREE)));

    if (!OI_SUCCESS(status)) {
        OI_Free(client->remoteName.str);
        client->remoteName.str = NULL;
        setState(CLIENT_STATE_CONNECTED);
        client->eventCB(client->id, OI_FTP_CLIENT_GET_COMPLETE, OI_OBEX_ACCESS_DENIED);
    } else {
        client->file = handle;
        hdr.id = OI_OBEX_HDR_NAME;
        hdr.val.name = client->remoteName;
        hdrList.list = &hdr;
        hdrList.count = 1;
        status = OI_OBEXCLI_Get(client->id, &hdrList, ClientGetRcvData, TRUE);
        if (!OI_SUCCESS(status)) {
            OI_DBGPRINT2(("Closing file 0x%x\n", client->file));
            FileClose(status);
        }
    }
}


static OI_STATUS GetFile(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                         OI_OBEX_UNICODE *localName,
                         OI_OBEX_UNICODE *remoteName)
{
    OI_STATUS status;

    /*
     * If remoteName is not specified this means the local and remote file
     * names are the same.
     */
    if (remoteName == NULL) {
        remoteName = localName;
    }

    OI_DBGPRINT2(("FTP client GetFile local: \"%S\" <- remote: \"%S\"\n",
                  localName->str, remoteName->str));

    status = CopyName(&client->remoteName, remoteName);
    if (!OI_SUCCESS(status)) {
        return status;
    }
    client->final = FALSE;
    client->fileOpCfmPending = TRUE;
    OI_DBGPRINT2(("Calling file op OPEN localName = \"%S\", mode = WRITE\n", localName->str));
#ifdef OI_CODE
#error code not present
#else
    status = client->fops->open(localName, OI_FTP_MODE_WRITE, 0, FileOpenGetCfm, (OI_FTP_CONNECTION)client->id);
#endif
    if (!OI_SUCCESS(status)) {
        client->fileOpCfmPending = FALSE;
        OI_Free(client->remoteName.str);
        client->remoteName.str = NULL;
    }
    return status;
}


OI_STATUS OI_FTPClient_Get(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                           OI_OBEX_UNICODE *localName,
                           OI_OBEX_UNICODE *remoteName)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_FTPClient_Get (connectionId = 0x%x, <*localName = %x>, <*remoteName = %x>)\n",
                 connectionId, localName, remoteName));

    ARGCHECK((localName != NULL) && (localName->len > 0));
    OI_DBGPRINT2(("    localName = \"%S\"    remoteName = \"%?S\"\n",
                  localName->str, remoteName ? remoteName->str : (OI_CHAR16*)"\0",
                  remoteName ? remoteName->len : 0));


    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("Not connected\n"));
        return OI_OBEX_NOT_CONNECTED;
    }
    if (client->state > CLIENT_STATE_CONNECTED) {
        OI_DBGPRINT(("Operation already in progress\n"));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    setState(CLIENT_STATE_GETTING);
    status = GetFile(connectionId, localName, remoteName);
    if (!OI_SUCCESS(status)) {
        setState(CLIENT_STATE_CONNECTED);
    }
    return status;
}

static void AbortCfm(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId)
{
    OI_DBGTRACE(("AbortCfm (connectionId = 0x%x)\n", connectionId));

    if (client->cancelCfm) {
        client->cancelCfm(connectionId);
    }
#ifdef OI_CODE
#error code not present
#else
    if(client->state > CLIENT_STATE_CONNECTED)
      FileClose(OI_OBEX_CLIENT_ABORTED_COMMAND);
#endif
}

OI_STATUS OI_FTPClient_Cancel(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                              OI_FTP_CLIENT_CANCEL_CFM cancelCfm)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_FTPClient_Cancel (connectionId = 0x%x, <*cancelCfm() = %x>)\n",
                 connectionId, cancelCfm));

    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("Not connected\n"));
        return OI_OBEX_NOT_CONNECTED;
    }
    /*
     * Any operation that might require multiple OBEX round-trips can be
     * aborted.
     */
    switch (client->state) {
        case CLIENT_STATE_PUTTING:
        case CLIENT_STATE_GETTING:
        case CLIENT_STATE_PUTTING_TREE:
        case CLIENT_STATE_GETTING_TREE:
        case CLIENT_STATE_LISTING_FOLDER:
            OI_DBGPRINT(("Client aborting operation %d\n", client->state));
            client->cancelCfm = cancelCfm;
#ifdef OI_CODE
#error code not present
#else
            if(!client->final)
            {
              status = OI_OBEXCLI_Abort((OI_OBEXCLI_CONNECTION_HANDLE)client->id, AbortCfm);
            }
            else
            {
              status = OI_STATUS_INVALID_STATE;
            }

            if (client->state == CLIENT_STATE_PUTTING_TREE ||
                client->state == CLIENT_STATE_GETTING_TREE ) {
                client->treePutGet = TRUE;
            }
#endif
            break;

        default:
            status = OI_OBEX_INVALID_OPERATION;
    }

    return status;
}

static void SetpathCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
#ifndef OI_CODE
                       OI_OBEX_HEADER_LIST *hdrs, /* ignored */
#endif
                       OI_STATUS status)
{
    OI_DBGTRACE(("SetpathCfm (connectionId = 0x%x, status = %!)\n",
                 connectionId, status));

    /*
     * Map errors to return more specific error information.
     */
    switch(status) {
        case OI_OBEX_NOT_FOUND:
            status = OI_OBEX_NO_SUCH_FOLDER;
            break;
        case OI_OBEX_METHOD_NOT_ALLOWED:
            status = OI_OBEX_FOLDER_BROWSING_NOT_ALLOWED;
            break;
        default:
            /* no mapping */
            break;
    }
#ifndef OI_CODE
    client->treePutGet = FALSE;
#endif
    setState(CLIENT_STATE_CONNECTED);
    client->eventCB(client->id, OI_FTP_CLIENT_SETPATH_COMPLETE, status);
}


OI_STATUS OI_FTPClient_SetPath(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                               OI_OBEX_UNICODE *folder,
                               OI_FTP_CLIENT_SETPATH_CONTROL setPathCtrl)
{
    OI_BOOL uplevel = FALSE;

    OI_DBGTRACE(("OI_FTPClient_SetPath (connectionId = 0x%x, <*folder = %x>, setPathCtrl = %s)\n",
                 connectionId, folder,
                 ((setPathCtrl == OI_FTP_CLIENT_SETPATH_TO_PARENT) ? "PARENT" :
                  (setPathCtrl == OI_FTP_CLIENT_SETPATH_TO_ROOT) ? "ROOT" : "FOLDER")));

    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("Not connected\n"));
        return OI_OBEX_NOT_CONNECTED;
    }
    if (client->state > CLIENT_STATE_CONNECTED) {
        OI_DBGPRINT(("Operation already in progress\n"));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    if (setPathCtrl == OI_FTP_CLIENT_SETPATH_TO_PARENT) {
        uplevel = TRUE;
    } else {
        uplevel = FALSE;
        if (setPathCtrl == OI_FTP_CLIENT_SETPATH_TO_ROOT) {
            /*
             * Folder cannot be specified when setting path to root.
             */
            if ((folder != NULL) && (folder->len > 0)) {
                return OI_STATUS_INVALID_PARAMETERS;
            }
        }
    }
    setState(CLIENT_STATE_SETTING_PATH);
#ifdef OI_CODE
#error code not present
#else
    return OI_OBEXCLI_SetPath(client->id, folder, TRUE, uplevel, NULL, SetpathCfm);
#endif
}


static void NewSubfolderCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
#ifndef OI_CODE
                            OI_OBEX_HEADER_LIST *rspHeaders, /* ignored */
#endif
                            OI_STATUS status)
{
    OI_DBGTRACE(("NewSubfolderCfm (connectionId = 0x%x, status = %!)\n",
                 connectionId, status));

    setState(CLIENT_STATE_CONNECTED);
    client->eventCB(connectionId, OI_FTP_CLIENT_NEW_SUBFOLDER_COMPLETE, status);
}


OI_STATUS OI_FTPClient_NewSubfolder(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                                    OI_OBEX_UNICODE *folder)
{
    OI_DBGTRACE(("OI_FTPClient_NewSubfolder (connectionId = 0x%x, <*folder = %x>)\n",
                 connectionId, folder));

    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("Not connected\n"));
        return OI_OBEX_NOT_CONNECTED;
    }
    if (client->state > CLIENT_STATE_CONNECTED) {
        OI_DBGPRINT(("Operation already in progress\n"));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    /*
     * New folders are created using the set path command with dontcreate = FALSE.
     */
    setState(CLIENT_STATE_SETTING_PATH);
#ifdef OI_CODE
#error code not present
#else
    return OI_OBEXCLI_SetPath(client->id, folder, FALSE, FALSE, NULL, NewSubfolderCfm);
#endif
}


static void ClientGetFolderData(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                OI_OBEX_HEADER_LIST *rspHeaders,
                                OI_STATUS obexStatus)
{
    OI_STATUS status = obexStatus;
    OI_OBEX_BYTESEQ *folderData = NULL;
    OI_INT i;

    OI_DBGTRACE(("ClientGetFolderData (connectionId = 0x%x, <*rspHeaders = %x>, obexStatus = %!)\n",
                 connectionId, rspHeaders, obexStatus));

    if ((obexStatus == OI_OK) || (obexStatus == OI_OBEX_CONTINUE)) {
        /*
         * Get the body header, ignore any other headers.
         */
        for (i = 0; i < rspHeaders->count; ++i) {
            OI_OBEX_HEADER *hdr = rspHeaders->list + i;

            if ((hdr->id == OI_OBEX_HDR_BODY) || (hdr->id == OI_OBEX_HDR_END_OF_BODY)) {
                folderData = &hdr->val.body;
                break;
            }
        }
        /*
         * Continue status means there is more data to get.
         */
        if (obexStatus == OI_OBEX_CONTINUE) {
#ifdef OI_CODE
#error code not present
#else
            /* Some devices send continue response with no data.  
               Do not issue callback in that case*/
            if ((folderData != NULL) && (folderData->len != 0)) {
#endif
                client->folderListCB(connectionId, folderData, OI_OK);
            }
            status = OI_OBEXCLI_Get(client->id, NULL, ClientGetFolderData, TRUE);
            if (OI_SUCCESS(status)) {
                return;
            }
        } else {
#ifdef OI_CODE
#error code not present
#else
            if ((folderData != NULL) && (folderData->len != 0)) {
#endif
                client->folderListCB(connectionId, folderData, OI_OK);
            }
            setState(CLIENT_STATE_CONNECTED);
            client->folderListCB(connectionId, NULL, OI_OK);
            return;
        }
    } 
    /*
     * Map errors to return more specific error information.
     */
    switch(status) {
        case OI_OBEX_NOT_FOUND:
            status = OI_OBEX_NO_SUCH_FOLDER;
            break;
        case OI_OBEX_METHOD_NOT_ALLOWED:
            status = OI_OBEX_FOLDER_BROWSING_NOT_ALLOWED;
            break;
        default:
            /* no mapping */
            break;
    }
    client->folderListCB(connectionId, NULL, status);
    setState(CLIENT_STATE_CONNECTED);
}


OI_STATUS OI_FTPClient_GetFolderListing(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                                        OI_OBEX_UNICODE *folder,
                                        OI_FTP_CLIENT_FOLDER_LIST_CB folderListCB)
{
    OI_STATUS status;
    OI_OBEX_HEADER hdrs[2];
    OI_OBEX_HEADER_LIST hdrList;

    OI_DBGTRACE(("OI_FTPClient_GetFolderListing (connectionId = 0x%x, <*folder = %x>, <*folderListCB() = %x>)\n",
                 connectionId, folder, folderListCB));

    ARGCHECK(folderListCB != NULL);

    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("Not connected\n"));
        return OI_OBEX_NOT_CONNECTED;
    }
    if (client->state > CLIENT_STATE_CONNECTED) {
        OI_DBGPRINT(("Operation already in progress\n"));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    /*
     * The folder list data is passed back via a callback as it is received.
     */
    client->folderListCB = folderListCB;

    hdrs[0].id = OI_OBEX_HDR_TYPE;
    hdrs[0].val.type.data = (OI_BYTE*) OI_OBEX_FOLDER_LISTING_TYPE;
    hdrs[0].val.type.len = sizeof(OI_OBEX_FOLDER_LISTING_TYPE);
    if ((folder == NULL) || (folder->len == 0) || (folder->str == NULL)) {
        hdrList.count = 1;
    } else {
        hdrs[1].id = OI_OBEX_HDR_NAME;
        hdrs[1].val.name = *folder;
        hdrList.count = 2;
    }
    hdrList.list = hdrs;
    setState(CLIENT_STATE_LISTING_FOLDER);
    status = OI_OBEXCLI_Get(client->id, &hdrList, ClientGetFolderData, TRUE);
    if (!OI_SUCCESS(status)) {
        setState(CLIENT_STATE_CONNECTED);
    }

    return status;
}


static void TreePut_LeaveRemoteCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
#ifndef OI_CODE
                                   OI_OBEX_HEADER_LIST *rspHeaders, /* ignored */
#endif
                                   OI_STATUS status)
{
    OI_DBGTRACE(("TreePut_LeaveRemoteCfm (connectionId = 0x%x, status = %!)\n",
                 connectionId, status));

    if (OI_SUCCESS(status) && (client->pathDepth > 0)) {
        TreePut(NULL);
    } else {
        Tree_Done(status);
    }
}

static void TreePut_LeaveLocalCfm(OI_STATUS status,
                                  OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("TreePut_LeaveLocalCfm (status = %!, ftpConnection = 0x%x)\n",
                 status, ftpConnection));

    if (OI_SUCCESS(status)) {
        OI_DBGPRINT(("TreePut_LeaveLocalCfm path depth = %d\n", client->pathDepth));
        OI_FreeIf(&client->path[client->pathIndex].name.str);
        --client->pathIndex;
#ifdef OI_CODE
#error code not present
#else
        status = OI_OBEXCLI_SetPath(client->id, NULL, TRUE, TRUE, NULL, TreePut_LeaveRemoteCfm);
#endif
        if (!OI_SUCCESS(status)) {
            Tree_Done(status);
        }
    } else {
        OI_DBGPRINT(("TreePut_LeaveLocalCfm error %!\n", status));
        ++client->pathDepth;
    }
}


/*
 * This function is a file ops callback function so must use the dispatcher to
 * call any other functions that might make file ops calls.
 */
static void TreePut_BuildListCfm(OI_FTP_HANDLE handle,
                                 OI_BYTE *data,
                                 OI_UINT16 len,
                                 OI_STATUS status,
                                 OI_FTP_CONNECTION ftpConnection)
{
    OI_FTP_PARSE_INFO *folder = &client->path[client->pathIndex];

    OI_DBGTRACE(("TreePut_BuildListCfm (handle = 0x%x, <*data = %x>, len = %d, status = %!, ftpConnection = 0x%x)\n",
                 handle, data, len, status, ftpConnection));

    VALIDATE_CFM_STATE("TreePut_BuildListCfm", (client->state == CLIENT_STATE_PUTTING_TREE));

    if (!OI_SUCCESS(status) && (status != OI_STATUS_END_OF_FILE)) {
        goto ErrorExit;
    }
    if ((folder->xmlPos + len) > folder->xmlSize) {
        status = OI_STATUS_NO_RESOURCES;
        goto ErrorExit;
    }

    OI_MemCopy(folder->xmlStream + folder->xmlPos, data, len);
    folder->xmlPos += len;

    if ((folder->xmlPos < folder->xmlSize) && (status != OI_STATUS_END_OF_FILE)) {
        /*
         * Keep reading the folder information.
         */
        status = OI_Dispatch_RegisterFunc(TreePut_BuildList, NULL, NULL);
    } else {
        /*
         * Close the XML folder listing handle.
         */
        OI_DBGPRINT2(("Calling file op CLOSE handle = %x, status = %!\n", client->file, status));
#ifdef OI_CODE
#error code not present
#else
        client->fops->close(client->file, (OI_FTP_CONNECTION)client->id, OI_OK);
#endif
        client->file = 0;
        /*
         * Now we have the complete XML stream start parsing it.
         */
        folder->xmlSize = folder->xmlPos; /* actual size of XML stream */
        folder->xmlPos = 0;
        status = OI_Dispatch_RegisterFunc(TreePut, NULL, NULL);
    }

    if (OI_SUCCESS(status)) {
        return;
    }

ErrorExit:

    Tree_Done(status);
}


static void TreePut_BuildList(DISPATCH_ARG *darg)
{
    OI_STATUS status;
    OI_FTP_PARSE_INFO *folder = &client->path[client->pathIndex];

    OI_DBGTRACE(("TreePut_BuildList (<*darg = %x>)\n", darg));

    client->fileOpCfmPending = TRUE;
    OI_DBGPRINT2(("Calling file op READ handle = %x, size = %d\n", client->file, (folder->xmlSize - folder->xmlPos)));
    status = client->fops->read(client->file, (OI_UINT16) (folder->xmlSize - folder->xmlPos), TreePut_BuildListCfm, (OI_FTP_CONNECTION)client->id);
    if (!OI_SUCCESS(status)) {
        Tree_Done(status);
    }
}


static void FolderBrowseLocalCfm(OI_FTP_HANDLE handle,
                                 OI_UINT32 size,
                                 OI_STATUS status,
                                 OI_FTP_CONNECTION ftpConnection)
{
    OI_FTP_PARSE_INFO *folder = &client->path[client->pathIndex];

    OI_DBGTRACE(("FolderBrowseLocalCfm (handle = 0x%x, size = %d, status = %!, ftpConnection = 0x%x)\n",
                 handle, size, status, ftpConnection));

    VALIDATE_CFM_STATE("FolderBrowseLocalCfm", (client->state == CLIENT_STATE_PUTTING_TREE));

    if (!OI_SUCCESS(status)) {
        goto ErrorExit;
    }
    /*
     * Allocate a buffer to receive the folder listing XML stream.
     */
    folder->xmlStream = OI_Malloc(size);
    folder->xmlSize = (OI_UINT16) size;
    folder->xmlPos = 0;
    if (folder->xmlStream == NULL) {
        status = OI_STATUS_NO_RESOURCES;
        goto ErrorExit;
    }
    /*
     * Start reading the folder listing XML
     */
    client->file = handle;
    status = OI_Dispatch_RegisterFunc(TreePut_BuildList, NULL, NULL);
    if (!OI_SUCCESS(status)) {
        goto ErrorExit;
    }
    return;

ErrorExit:
    Tree_Done(status);
}


/*
 * We have created and entered the remote folder. Now we are going to get the
 * folder listing for the local folder.
 */

static void CreateFolderRemoteCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
#ifndef OI_CODE
                                  OI_OBEX_HEADER_LIST *rspHeaders, /* ignored */
#endif
                                  OI_STATUS status)
{
    OI_DBGTRACE(("CreateFolderRemoteCfm (connectionId = 0x%x, status = %!)\n",
                 connectionId, status));

    if (OI_SUCCESS(status)) {
        /*
         * Get the local folder listing.
         */
        client->fileOpCfmPending = TRUE;
        OI_DBGPRINT2(("Calling file op BROWSE_FOLDER\n"));
        status = client->fops->browseFolder(NULL, FolderBrowseLocalCfm, (OI_FTP_CONNECTION)client->id);
        if (!OI_SUCCESS(status)) {
            client->fileOpCfmPending = FALSE;
        }
    }
    if (!OI_SUCCESS(status)) {
        Tree_Done(status);
    }
}


/*
 * We have entered the local folder now create and enter the remote folder.
 */

static void TreePut_EnterLocalCfm(OI_STATUS status,
                                  OI_FTP_CONNECTION ftpConnection)
{
    OI_FTP_PARSE_INFO *info = &client->path[client->pathIndex];

    OI_DBGTRACE(("TreePut_EnterLocalCfm (status = %!, ftpConnection = 0x%x)\n",
                 status, ftpConnection));

    if (OI_SUCCESS(status)) {
        OI_DBGPRINT(("TreePut_EnterLocalCfm path depth = %d\n", client->pathDepth));
        /*
         * This will create the folder on the remote device and enter that
         * folder.
         */
#ifdef OI_CODE
#error code not present
#else
        status = OI_OBEXCLI_SetPath(client->id, &info->name, FALSE, FALSE, NULL, CreateFolderRemoteCfm);
#endif
    } else {
        OI_DBGPRINT(("TreePut_EnterLocalCfm error %!\n", status));
        --client->pathDepth;
    }

    if (!OI_SUCCESS(status)) {
        Tree_Done(status);
    }
}


/*
 * Start or continue parsing the folder listing.
 */

static void TreePut(DISPATCH_ARG *darg)
{
    OI_STATUS status;
    OI_FTP_PARSE_INFO *info = &client->path[client->pathIndex];

    OI_DBGTRACE(("TreePut (<*darg = %x>)\n", darg));

    /*
     * Free previous name string before we allocate one for the next name.
     */
    OI_FreeIf(&info->name.str);

    if (info->xmlPos == 0) {
        /*
         * Start parsing the folder information.
         */
        status = OI_FTP_FolderParseInit(info);
        if (!OI_SUCCESS(status)) {
            goto ErrorExit;
        }
    }

    status = OI_FTP_FolderParseNext(info);
    if (!OI_SUCCESS(status)) {
        goto ErrorExit;
    }

#ifndef OI_CODE
    /* FTP has got an abort from the APP. We have to stop putting the folder */
    if(client->treePutGet)
    {
        status = OI_OBEX_CLIENT_ABORTED_COMMAND;
        goto ErrorExit;
    }
#endif
    if (info->xmlPos == info->xmlSize) {
        OI_DBGPRINT2(("Leave folder (%d) %S\n", client->pathIndex, info->name.str));
        OI_FreeIf(&info->xmlStream);
        OI_DBGPRINT2(("Calling file op SET_FOLDER dir = <root>\n"));
        status = client->fops->setFolder(NULL, --client->pathDepth, FALSE, TreePut_LeaveLocalCfm, (OI_FTP_CONNECTION)client->id);
    } else if (info->isFolder) {
        OI_DBGPRINT2(("Enter folder (%d) %S\n", client->pathIndex, info->name.str));
        if (++client->pathIndex == client->maxPathDepth) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Exceeded maximum folder tree depth"));
#endif
            --client->pathIndex;
            status = OI_STATUS_NO_RESOURCES;
            goto ErrorExit;
        }
        OI_MemZero(&client->path[client->pathIndex], sizeof(OI_FTP_PARSE_INFO));
        status = CopyName(&client->path[client->pathIndex].name, &info->name);
        if (!OI_SUCCESS(status)) {
            --client->pathIndex;
            goto ErrorExit;
        }
        OI_DBGPRINT2(("Calling file op SET_FOLDER dir = %S\n", info->name.str));
        status = client->fops->setFolder(&info->name, ++client->pathDepth, FALSE, TreePut_EnterLocalCfm, (OI_FTP_CONNECTION)client->id);
        if (!OI_SUCCESS(status)) {
            client->fileOpCfmPending = FALSE;
        }
    } else {
        status = PutFile(client->id, &info->name, &info->name);
    }

    if (OI_SUCCESS(status)) {
        return;
    }

ErrorExit:

    Tree_Done(status);
}


/*
 * Folder put recursively traverses the folder tree creating folders and putting
 * files until the entire tree has been copied to the remote device.
 */
OI_STATUS OI_FTPClient_TreePut(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                               OI_OBEX_UNICODE *localName,
                               OI_OBEX_UNICODE *remoteName)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_FTPClient_TreePut (connectionId = 0x%x, <*localName = %x>, <*remoteName = %x>)\n",
                 connectionId, localName, remoteName));

    ARGCHECK((localName != NULL) && (localName->len > 0));

    OI_DBGPRINT2(("Client put folder %S\n", localName->str));

    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("Not connected\n"));
        return OI_OBEX_NOT_CONNECTED;
    }
    if (client->state > CLIENT_STATE_CONNECTED) {
        OI_DBGPRINT(("Operation already in progress\n"));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    /*
     * remoteName is not specified this means the local and remote file names
     * are the same.
     */
    if (remoteName == NULL) {
        remoteName = localName;
    }
    /*
     * Are folder operations allowed?.
     */
    if (client->maxPathDepth == 0) {
        return OI_OBEX_METHOD_NOT_ALLOWED;
    }
    client->pathIndex = 0;
    client->pathDepth = 0;
    client->path = OI_Calloc(sizeof(OI_FTP_PARSE_INFO) * client->maxPathDepth);
    if (client->path == NULL) {
        return OI_STATUS_NO_RESOURCES;
    }
    status = CopyName(&client->path[0].name, remoteName);
    if (OI_SUCCESS(status)) {
        setState(CLIENT_STATE_PUTTING_TREE);
        /*
         * Enter the local folder, this folder must already exist. The current
         * folder is the "root" folder for this transaction so the folder level for
         * the folder we are entering is '1'.
         */
        OI_DBGPRINT2(("Calling file op SET_FOLDER dir = %S\n", localName->str));
        status = client->fops->setFolder(localName, ++client->pathDepth, FALSE, TreePut_EnterLocalCfm, (OI_FTP_CONNECTION)client->id);
        if (!OI_SUCCESS(status)) {
            client->fileOpCfmPending = FALSE;
            OI_Free(client->path[0].name.str);
            setState(CLIENT_STATE_CONNECTED);
        }
    }
    return status;
}



static void TreeGet_LeaveLocalCfm(OI_STATUS status,
                                  OI_FTP_CONNECTION ftpConnection)
{
    OI_DBGTRACE(("TreeGet_LeaveLocalCfm (status = %!, ftpConnection = 0x%x)\n",
                 status, ftpConnection));

    if (OI_SUCCESS(status)) {
        OI_DBGPRINT(("TreeGet_LeaveLocalCfm path depth = %d\n", client->pathDepth));
        OI_FreeIf(&client->path[client->pathIndex].name.str);
        --client->pathIndex;
        if (client->pathDepth > 0) {
            TreeGet();
            return;
        }
    } else {
        ++client->pathDepth;
        OI_DBGPRINT(("TreeGet_LeaveLocalCfm error %!\n", status));
    }
    Tree_Done(status);
}


static void TreeGet_LeaveRemoteCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
#ifndef OI_CODE
                                   OI_OBEX_HEADER_LIST *rspHeaders, /* ignored */
#endif
                                   OI_STATUS status)
{
    OI_DBGTRACE(("TreeGet_LeaveRemoteCfm (connectionId = 0x%x, status = %!)\n",
                 connectionId, status));

    if (OI_SUCCESS(status)) {
        OI_DBGPRINT2(("Calling file op SET_FOLDER dir = <root>\n"));
        status = client->fops->setFolder(NULL, --client->pathDepth, FALSE, TreeGet_LeaveLocalCfm, (OI_FTP_CONNECTION)client->id);
    }
    if (!OI_SUCCESS(status)) {
        client->fileOpCfmPending = FALSE;
        Tree_Done(status);
    }
}


static void TreeGet_BuildList(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                              OI_OBEX_HEADER_LIST *rspHeaders,
                              OI_STATUS obexStatus)
{
    OI_FTP_PARSE_INFO *info = &client->path[client->pathIndex];
    OI_STATUS status = obexStatus;
    OI_OBEX_HEADER *hdr;
    OI_OBEX_BYTESEQ *folderData = NULL;
    OI_INT i;

    OI_DBGTRACE(("TreeGet_BuildList (connectionId = 0x%x, <*rspHeaders = %x>, obexStatus = %!)\n",
                 connectionId, rspHeaders, obexStatus));

    if ((obexStatus == OI_OK) || (obexStatus == OI_OBEX_CONTINUE)) {
        /*
         * We need to know how much memory to allocate for the folder listing
         * XML stream. Hopefully the server will return a length header but if
         * not check for an end-of-body header.
         */
        for (i = 0; i < rspHeaders->count; ++i) {
            hdr = &rspHeaders->list[i];
            if (hdr->id == OI_OBEX_HDR_LENGTH) {
                info->xmlSize = (OI_UINT16) hdr->val.length;
                OI_DBGPRINT2(("info->xmlSize = %d\n", info->xmlSize));
            }
            if ((hdr->id == OI_OBEX_HDR_BODY) || (hdr->id == OI_OBEX_HDR_END_OF_BODY)) {
                folderData = &hdr->val.body;
                if (info->xmlSize < (folderData->len + info->xmlPos)){
                    /* We didn't get length -> Try to infer xmlSize from
                     * folder data length.
                     */
                    OI_DBGPRINT2(("Bogus/No Length header in Folder Listing (%d,%d)\n", info->xmlSize, folderData->len + info->xmlPos));
                    if (info->xmlStream != NULL){
                        void *tmp = OI_Malloc(folderData->len + info->xmlPos);
                        if (tmp != NULL){
                            OI_DBGPRINT2(("Reallocating xmlStream (%d)\n", folderData->len + info->xmlPos));
                            OI_MemCopy(tmp, info->xmlStream, info->xmlPos);
                            OI_Free(info->xmlStream);
                            info->xmlStream = tmp;
                        } else {
                            OI_DBGPRINT2(("tmp == NULL\n"));
                            status = OI_STATUS_NO_RESOURCES;
                            goto ErrorExit;
                        }
                    }
                    info->xmlSize = folderData->len + info->xmlPos;
            }
        }
        }

        if (folderData != NULL) {
        /*
         * Allocate memory for stream if needed.
         */
            if (folderData->len > 0){
        if (info->xmlStream == NULL) {
            if (info->xmlSize == 0) {
                        info->xmlSize = folderData->len;
            }
                    OI_DBGPRINT2(("Alloc xmlStream (%d)\n", info->xmlSize));
            info->xmlStream = OI_Malloc(info->xmlSize);
            info->xmlPos = 0;
            if (info->xmlStream == NULL) {
                        OI_DBGPRINT2(("xmlStream == NULL\n"));
                status = OI_STATUS_NO_RESOURCES;
                goto ErrorExit;
            }
        }

            if ((info->xmlPos + folderData->len) > info->xmlSize) {
                    OI_DBGPRINT2(("xmlPos (%d) + len (%d) > xmlSize (%d)\n", info->xmlPos, folderData->len, info->xmlSize));
                status = OI_STATUS_NO_RESOURCES;
                goto ErrorExit;

            }
            OI_MemCopy(info->xmlStream + info->xmlPos, folderData->data, folderData->len);
            info->xmlPos += folderData->len;
        }
        }
        /*
         * Continue status means there is more folder listing data to get.
         */
        if (obexStatus == OI_OBEX_CONTINUE) {
            status = OI_OBEXCLI_Get(client->id, NULL, TreeGet_BuildList, TRUE);
        } else {
            /*
             * Now we have the complete XML stream start parsing it.
             */
            info->xmlSize = info->xmlPos; /* actual size of XML stream */
            info->xmlPos = 0;
            OI_DBGPRINT2(("Folder information\n%?s\n", info->xmlStream, info->xmlSize));
            TreeGet();
            status = OI_OK;
        }
    }

    if (OI_SUCCESS(status)) {
        return;
    }

ErrorExit:

    OI_FreeIf(&info->xmlStream);
    Tree_Done(status);
    status = OI_OBEXCLI_Abort(client->id, NULL);
    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("Abort operation failed %!\n", status));
    }
}


/*
 * We have entered the local folder. Now we are going to get the
 * folder listing for the remote folder.
 */

static void TreeGet_EnterLocalCfm(OI_STATUS status,
                                  OI_FTP_CONNECTION ftpConnection)
{
    OI_OBEX_HEADER typeHdr;
    OI_OBEX_HEADER_LIST hdrList;

    OI_DBGTRACE(("TreeGet_EnterLocalCfm (status = %!, ftpConnection = 0x%x)\n",
                 status, ftpConnection));

    if (OI_SUCCESS(status)) {
        OI_DBGPRINT(("TreeGet_EnterLocalCfm path depth = %d\n", client->pathDepth));
        /*
         * Get the remote folder listing.
         */
        typeHdr.id = OI_OBEX_HDR_TYPE;
        typeHdr.val.type.data = (OI_BYTE*) OI_OBEX_FOLDER_LISTING_TYPE;
        typeHdr.val.type.len = sizeof(OI_OBEX_FOLDER_LISTING_TYPE);
        hdrList.count = 1;
        hdrList.list = &typeHdr;
        status = OI_OBEXCLI_Get(client->id, &hdrList, TreeGet_BuildList, TRUE);
    } else {
        OI_DBGPRINT(("TreeGet_EnterLocalCfm error %!\n", status));
        --client->pathDepth;
    }
    if (!OI_SUCCESS(status)) {
        Tree_Done(status);
    }
}


/*
 * If the local folder was succesfully created enter the corresponding remote
 * folder.
 */

static void TreeGet_EnterRemoteCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
#ifndef OI_CODE
                                   OI_OBEX_HEADER_LIST *rspHeaders, /* ignored */
#endif
                                   OI_STATUS status)
{
    OI_FTP_PARSE_INFO *info = &client->path[client->pathIndex];

    OI_DBGTRACE(("TreeGet_EnterRemoteCfm (connectionId = 0x%x, status = %!)\n",
                 connectionId, status));

    OI_DBGPRINT2(("Enter folder %?S\n", info->name.str, info->name.len));

    if (OI_SUCCESS(status)) {
        /*
         * This will create the folder on the local device and enter that folder.
         */
        OI_DBGPRINT2(("Calling file op SET_FOLDER dir = %S\n", info->name.str));
        status = client->fops->setFolder(&info->name, ++client->pathDepth, TRUE, TreeGet_EnterLocalCfm, (OI_FTP_CONNECTION)client->id);
    }
    if (!OI_SUCCESS(status)) {
        client->fileOpCfmPending = FALSE;
        Tree_Done(status);
    }
}


/*
 * Start or continue parsing the folder listing.
 */

static void TreeGet(void)
{
    OI_STATUS status;
    OI_FTP_PARSE_INFO *info = &client->path[client->pathIndex];

    OI_DBGTRACE(("TreeGet (<void>)\n"));

    /*
     * Free previous name string before we allocate one for the next name.
     */
    OI_FreeIf(&info->name.str);

    if (info->xmlPos == 0) {
        /*
         * Start parsing the folder information.
         */
        status = OI_FTP_FolderParseInit(info);
        if (!OI_SUCCESS(status)) {
            goto ErrorExit;
        }
    }

    status = OI_FTP_FolderParseNext(info);
    if (!OI_SUCCESS(status)) {
        goto ErrorExit;
    }

#ifndef OI_CODE
    /* FTP has got an abort from the APP. We have to stop putting the folder */
    if(client->treePutGet)
    {
        status = OI_OBEX_CLIENT_ABORTED_COMMAND;
        goto ErrorExit;
    }
#endif

    /*
     * Check for the end of the xml stream.
     */
    if (info->xmlPos == info->xmlSize) {
        OI_DBGPRINT2(("TreeGet leaving folder %d\n", client->pathIndex));
        OI_Free(info->xmlStream);
        info->xmlStream = 0;
#ifdef OI_CODE
#error code not present
#else
        status = OI_OBEXCLI_SetPath(client->id, NULL,  TRUE, TRUE, NULL, TreeGet_LeaveRemoteCfm);
#endif
    } else if (info->isFolder) {
        if (++client->pathIndex == client->maxPathDepth) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Exceeded maximum folder tree depth"));
#endif
            --client->pathIndex;
            status = OI_STATUS_NO_RESOURCES;
            goto ErrorExit;
        }
        OI_DBGPRINT2(("TreeGet entering folder %d\n", client->pathIndex));
        OI_MemZero(&client->path[client->pathIndex], sizeof(OI_FTP_PARSE_INFO));
        status = CopyName(&client->path[client->pathIndex].name, &info->name);
        if (!OI_SUCCESS(status)) {
            --client->pathIndex;
            goto ErrorExit;
        }
#ifdef OI_CODE
#error code not present
#else
        status = OI_OBEXCLI_SetPath(client->id, &info->name, TRUE, FALSE, NULL, TreeGet_EnterRemoteCfm);
#endif
    } else {
        status = GetFile(client->id, &info->name, NULL);
    }

    if (OI_SUCCESS(status)) {
        return;
    }

ErrorExit:

    Tree_Done(status);
}


/*
 * Tree get recursively traverses the folder tree creating folders and getting
 * files until the entire tree has been copied to the local device.
 */
OI_STATUS OI_FTPClient_TreeGet(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId,
                               OI_OBEX_UNICODE *localName,
                               OI_OBEX_UNICODE *remoteName)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_FTPClient_TreeGet (connectionId = 0x%x, <*localName = %x>, <*remoteName = %x>)\n",
                 connectionId, localName, remoteName));

    ARGCHECK((localName != NULL) && (localName->len > 0));

    OI_DBGPRINT2(("Client get folder tree %S\n", localName->str));

    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("Not connected\n"));
        return OI_OBEX_NOT_CONNECTED;
    }
    if (client->state > CLIENT_STATE_CONNECTED) {
        OI_DBGPRINT(("Operation already in progress\n"));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    /*
     * remoteName is not specified this means the local and remote file names
     * are the same.
     */
    if (remoteName == NULL) {
        remoteName = localName;
    }
    /*
     * Are folder operations allowed?.
     */
    if (client->maxPathDepth == 0) {
        return OI_OBEX_METHOD_NOT_ALLOWED;
    }
    client->pathIndex = 0;
    client->pathDepth = 0;
    client->path = OI_Calloc(sizeof(OI_FTP_PARSE_INFO) * client->maxPathDepth);
    if (client->path == NULL) {
        return OI_STATUS_NO_RESOURCES;
    }
    /*
     * We want to create a local folder with the appropriate name so copy the
     * local name into the first path entry.
     */
    status = CopyName(&client->path[0].name, localName);
    /*
     * Entering the remote folder first ensures that the requested remote folder
     * actually exists before we go and create a corresponding local folder.
     */
    if (OI_SUCCESS(status)) {
        setState(CLIENT_STATE_GETTING_TREE);
#ifdef OI_CODE
#error code not present
#else
        status = OI_OBEXCLI_SetPath(client->id, remoteName, TRUE, FALSE, NULL, TreeGet_EnterRemoteCfm);
#endif
        if (!OI_SUCCESS(status)) {
            OI_Free(client->path[0].name.str);
            setState(CLIENT_STATE_CONNECTED);
        }
    }
    return status;
}


#ifdef OI_DEPRECATED
/**************************************************************************
 *                         Deprecated
 **************************************************************************/

static OI_FTP_CLIENT_EVENT_CB deprecatedEventCB;
static const OI_FTP_FILESYS_FUNCTIONS *deprecatedFileOperations;
static OI_FTP_CLIENT_AUTHENTICATION_CB deprecatedAuthenticationCB;

/**
 * @deprecated
 */
OI_STATUS OI_FTPClient_Init(OI_FTP_CLIENT_EVENT_CB eventCB,
                            OI_FTP_CLIENT_AUTHENTICATION_CB authenticationCB,
                            const OI_FTP_FILESYS_FUNCTIONS *fileOperations)
{
    ARGCHECK(eventCB != NULL);
    ARGCHECK(authenticationCB != NULL);

    deprecatedEventCB = eventCB;
    deprecatedFileOperations = fileOperations;
    deprecatedAuthenticationCB = authenticationCB;
    return OI_OK;
}

/**
 * @deprecated
 */
OI_STATUS OI_FTPClient_Connect_10(OI_BD_ADDR *addr,
                                  OI_UINT channel,
                                  OI_FTP_CLIENT_CONNECTION_HANDLE *connectionId)
{
    return OI_FTPClient_Connect(addr,
                                channel,
                                connectionId,
                                deprecatedEventCB,
                                deprecatedAuthenticationCB,
                                deprecatedFileOperations);
}

/**
 * @deprecated
 */
OI_STATUS OI_FTPClient_Reset(void)
{
    return OI_OK;
}

/**
 * @deprecated
 */
OI_STATUS OI_FTPClient_Terminate(void)
{
    deprecatedEventCB = NULL;
    deprecatedFileOperations = NULL;
    deprecatedAuthenticationCB = NULL;
    return OI_OK;
}

/**
 * @deprecated
 */
OI_STATUS OI_FTPClient_Abort(OI_FTP_CLIENT_CONNECTION_HANDLE connectionId)
{
    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("Not connected\n"));
        return OI_OBEX_NOT_CONNECTED;
    }
    /*
     * Any operation that might require multiple OBEX round-trips can be
     * aborted.
     */
    switch (client->state) {
        case CLIENT_STATE_PUTTING:
        case CLIENT_STATE_GETTING:
        case CLIENT_STATE_PUTTING_TREE:
        case CLIENT_STATE_GETTING_TREE:
        case CLIENT_STATE_LISTING_FOLDER:
            OI_DBGPRINT(("Client aborting operation %d\n", client->state));
            return OI_OBEXCLI_AbortOperation(client->id);
        default:
            return OI_OBEX_INVALID_OPERATION;
    }
}

#endif /* OI_DEPRECATED */
