/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/opp_server.c#2 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-14-09  gs   Updated deferredObjectClose() to send disconnect indication 
                   even if connection is still pending accept.

*    #17       14 Jan 2009            GS
* 
*
*    #16       26 Feb 2008            GS
* Updated ServerDisconnectInd() to send object close notification always 
* irrespective of whether OFS confirm callback was issued by the app or not.
*
*    #15       20 Jun 2007            GS
* Re-added changes made in #10
*
*    #14       28 Feb 2007            MH
* Modified srv disconnect indication to support optional headers
*
*    #13       10 Nov  2006           PR
* Merged OI Code v3.0.14rev1revQ
*
*    #12       26 Oct 2006            MH
* Modified srv connect indication to support all headers
*
*    #11        29 July 2006          BK
* Modified to initiate obex authentication from client
*
*    #10       14 Jul 2006            GS
* Updated IsSupportedType() to confirm to revised OPP supported formats
* definition.
*
*    #8-9      10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #7        23 Jun 2006            JH
* Modified to work with updated OBEX code which supports JSR82.
*
*    #6        04 Mar 2005            JH
* Eliminated unnecessary call to InitFormatsAttribute() which causes memory 
*  leak.
*
*    #5        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*         
*    #4        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
*    #3        26 Jul 2004            JH
* Modified code such that object close is executed immediately (in OPPDone) 
*  rather than scheduled to be invoked later by dispatcher. This resolves the
*  problem where, in back to back object operations (e.g. in object exchange)
*  the close request of the first operation happens after the open request
*  of the second operation.
*
===========================================================================*/
/**
@file
@internal
  
Object Push Profile server
*/

/**********************************************************************************
  $Revision: #2 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#define __OI_MODULE__ OI_MODULE_OPP_SRV

#include "oi_opp_server.h"
#include "oi_opp_common.h"
#include "oi_obexspec.h"
#include "oi_obexsrv.h"
#include "oi_utils.h"
#include "oi_memmgr.h"
#include "oi_dispatch.h"
#include "oi_debug.h"
#include "oi_assert.h"
#include "oi_bt_assigned_nos.h"
#include "oi_sdpdb.h"
#include "oi_sdp_utils.h"
#include "oi_std_utils.h"
#include "oi_securitydb.h"
#include "oi_devmgr.h"

#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"

/*
 * Service class ID list
 */

static const OI_DATAELEM ServiceClassIDList[] = {
    OI_ELEMENT_UUID32(OI_UUID_OBEXObjectPush)
};

/*
 * Profile descriptor list
 */

static const OI_DATAELEM Profile0[] = {
    OI_ELEMENT_UUID32(OI_UUID_OBEXObjectPush),
    OI_ELEMENT_UINT16(0x0100) /* version 1.0 */
};

static const OI_DATAELEM ProfileDescriptorList[] = {
    OI_ELEMENT_SEQ(Profile0)
};

/*
 * SDP Attribute lists
 */

static const OI_SDPDB_ATTRIBUTE ServiceDescription[] = {
    { OI_ATTRID_ServiceClassIDList,             OI_ELEMENT_SEQ(ServiceClassIDList) },
    { OI_ATTRID_BluetoothProfileDescriptorList, OI_ELEMENT_SEQ(ProfileDescriptorList) }
};



/*
 * Note that the order of these states is important.
 */
typedef enum {
    OPP_SERVER_IDLE       = 0, /* doing nothing right now */
    OPP_SERVER_CONNECTING = 1, /* server has indicated a connection */
    OPP_SERVER_CONNECTED  = 2, /* server is connected */
    OPP_PULL_PENDING      = 3, /* pull has been requested */
    OPP_CLIENT_PULLING    = 4, /* pull is in progress  */
    OPP_PUSH_PENDING      = 5, /* push has been requested */
    OPP_CLIENT_PUSHING    = 6  /* push is in progress  */
} OPP_SERVER_STATE;

/*
 * Struct for an object push connection. This implementation only allows one
 * OPP server.
 */

typedef struct {

    OPP_SERVER_STATE state;

    OI_OPP_HANDLE handle;

    OI_UINT16 maxReadSize;

    OI_OPP_GENERIC_OBJECT rcvObj;

    OI_OBEXSRV_CONNECTION_HANDLE id;

    const OI_OPP_OBJSYS_FUNCTIONS *objops;

    OI_OPP_SERVER_CONNECT_IND connectInd;
    OI_OPP_SERVER_DISCONNECT_IND disconnectInd;

    OI_BOOL allowPush;
    OI_BOOL allowPull;

    OI_BOOL final;

    OI_UINT32 srecHandle;
    OI_UINT8 channel;

    OI_CONNECT_POLICY connectPolicy;

    OI_OPP_SERVER_OBJECT_FORMATS supportedFormats;  /**< Supported formats as a bit vector.*/

    OI_SDPDB_ATTRIBUTE sdpAttributes[2]; /**< Supported format and Protocol Descriptor list */

    DISPATCH_CB_HANDLE closeCB;

    OI_BOOL     ofsCfmPending;          /**< Waiting for Object File System to call our cfm function */
    OI_BOOL     disconnected;           /**< Received disconnect indication, waiting to call event callback. */
    OI_BOOL     deferredPPError;        /**< Push/Pull error occurred while awaiting OFS cfm. */
    OI_STATUS   deferredErrorStatus;    /**< The specific error whose handling has been deferred */
    
} OPP_SERVER;


static OPP_SERVER *server;


#ifdef OI_DEBUG
#error code not present
#else

#define ServerStateText(state)  ("")

#define setState(newState)  (server->state = newState)

#endif  /* OI_DEBUG */


/*
 * Tries to figure out the type of the object being pushed from the suffix on the
 * name.
 */

static OI_CHAR* ObjTypeFromSuffix(OI_OBEX_UNICODE *name)
{
    OI_CHAR suffix[5];
    OI_UINT len;
    OI_INT i;

    /*
     * The spec says that a OBEX name should be a null terminated but we 
     * are a little more paranoid.
     */
    for (len = name->len; len > 0; --len) {
        if (name->str[len - 1] != 0) {
            break;
        }
    }
    /*
     * A suffixed name must be at least 5 characters long (e.g. x.vcf).
     */
    if (len >= 5) {
        /*
         * Convert suffix (including '.') to ascii.
         */
        for (i = 0; i < 4; ++i) {
            suffix[i] = (OI_CHAR) (name->str[len - 4 + i] & 0xFF);
        }
        suffix[i] = 0;
        OI_DBGPRINT2(("ObjecTypeFromSuffix %s", suffix));
        if (OI_Strcmp(suffix, OI_OBEX_VCARD_SUFFIX) == 0) {
            return OI_OBEX_VCARD_TYPE;
        }
        if (OI_Strcmp(suffix, OI_OBEX_VCALENDAR_SUFFIX) == 0) {
            return OI_OBEX_VCALENDAR_TYPE;
        }
        if (OI_Strcmp(suffix, OI_OBEX_VNOTE_SUFFIX) == 0) {
            return OI_OBEX_VNOTE_TYPE;
        }
        if (OI_Strcmp(suffix, OI_OBEX_VMESSAGESUFFIX) == 0) {
            return OI_OBEX_VMESSAGE_TYPE;
        }
        if (OI_Strcmp(suffix, OI_OBEX_ICALENDAR_SUFFIX) == 0) {
            return OI_OBEX_ICALENDAR_TYPE;
        }
    }
    return "";
}


/*
 * Brute force check for supported format type.
 */
static OI_BOOL IsSupportedType(OI_CHAR *type, 
                               OI_UINT typeLen,
                               OI_OPP_SERVER_OBJECT_FORMATS formats)
{
#ifdef OI_CODE
#error code not present
#endif
    if (OI_StrncmpInsensitive(type, OI_OBEX_VCARD_TYPE, (OI_UINT16)typeLen) == 0) {
        return formats & (OI_OPP_SERVER_OBJ_FORMAT_VCARD_2_1 | OI_OPP_SERVER_OBJ_FORMAT_VCARD_3_0);
    }
    if (OI_StrncmpInsensitive(type, OI_OBEX_VCALENDAR_TYPE, (OI_UINT16)typeLen) == 0) {
        return (formats & OI_OPP_SERVER_OBJ_FORMAT_VCAL_1_0);
    }
    if (OI_StrncmpInsensitive(type, OI_OBEX_ICALENDAR_TYPE, (OI_UINT16)typeLen) == 0) {
        return (formats & OI_OPP_SERVER_OBJ_FORMAT_ICAL_2_0);
    }
    if (OI_StrncmpInsensitive(type, OI_OBEX_VNOTE_TYPE, (OI_UINT16)typeLen) == 0) {
        return (formats & OI_OPP_SERVER_OBJ_FORMAT_VNOTE);
    }
    if (OI_StrncmpInsensitive(type, OI_OBEX_VMESSAGE_TYPE, (OI_UINT16)typeLen) == 0) {
        return (formats & OI_OPP_SERVER_OBJ_FORMAT_VMESSAGE);
    }
#ifndef OI_CODE
    if ( (formats & OI_OPP_SERVER_OBJ_FORMAT_ANY) == OI_OPP_SERVER_OBJ_FORMAT_ANY ) {
        return TRUE;
    }
#endif
    return FALSE;
}

/*
 * Struct for deferred objops->close() call
 */
typedef struct  {
    OI_OPP_HANDLE       objHandle;
    OI_STATUS           status;
    OI_OPP_CONNECTION   serverId;
} DEFERRED_OBJ_CLOSE_ARGS;


static void deferredObjectClose(DISPATCH_ARG *darg)
{
    OI_STATUS status = Dispatch_GetArg(darg, OI_STATUS);
    OPP_SERVER_STATE snapState;

    if (server == NULL) {
        OI_LOG_ERROR(("deferredObjectClose - server has been freed"));
        return;
    }

    snapState = server->state;
    server->closeCB = 0;

    /*
     * Free any saved header data.
     */
    OI_FreeIf(&server->rcvObj.name.str);
    OI_FreeIf(&server->rcvObj.type.data);
    OI_MemZero(&server->rcvObj, sizeof(OI_OPP_GENERIC_OBJECT));

    /*
     * Reset state before calling event callback in case the callback function
     * calls right back into OPP.
     */
    if (server->disconnected) {
        setState(OPP_SERVER_IDLE);
    } else if (server->state > OPP_SERVER_CONNECTED) {
        setState(OPP_SERVER_CONNECTED);
    }

    /*
     * If there is an open object, close the object.
     */
    if (NULL != server->handle) {
        /*
         * We cannot call back into the application on this thread so we must
         * complete the object close on the dispatch thread.
         */
        server->objops->Close(server->handle, status, (OI_OPP_CONNECTION)server->id);
        server->handle = NULL;
    }
    
    /*
     * If we're disconnected, call the disconnect indication now
     */
    if (server->disconnected) {
        OI_OBEXSRV_CONNECTION_HANDLE savedId = server->id;
        /*
         * Clear out server context - we are no longer connected.
         */
        server->id = 0;
        setState(OPP_SERVER_IDLE);
        server->disconnected = FALSE;
        /*
         * If the connection was in place, call the application disconnect
         * indication callback if one was provided.
         */
#ifdef OI_CODE
#error code not present
#else
        if ((snapState >= OPP_SERVER_CONNECTING) && (server->disconnectInd != NULL)) {
#endif
            server->disconnectInd(savedId);
        }
    }
}

static void setupDeferredObjectClose(OI_STATUS status)
{
    DISPATCH_ARG            darg;
#ifdef OI_CODE
#error code not present
#endif
        /*
     * There can only be one object open at any one time, so there can only be one object close
     * pending at any one time.
         */
    OI_ASSERT(!server->closeCB);        // there can only be one object
    
    OI_DBGPRINT2(("setupDeferredObjectClose"));
    Dispatch_SetArg(darg, status);
#ifdef OI_CODE
#error code not present
#else
   deferredObjectClose(&darg);
#endif
}


static void PushWriteCfm(OI_OPP_HANDLE handle,
                         OI_STATUS status,
                         OI_OPP_CONNECTION oppContext)
{
    OI_STATUS retVal;

    OI_DBGTRACE(("PushWriteCfm %!", status));

    /*
     * First check we are expecting this call.
     */
    if (!OI_INIT_FLAG_VALUE(OPP_SRV)) {
        OI_LOG_ERROR(("%!", OI_STATUS_NOT_INITIALIZED));
        return;
    }
    else if ((server->state != OPP_CLIENT_PUSHING) || !server->ofsCfmPending) {
        OI_LOG_ERROR(("Push write confirm called at wrong time"));
        return;
    }

    /* 
     * Confirm is no longer pending 
     */
    server->ofsCfmPending = FALSE;

    /*
     * Check for disconnect that occurred during wait for this confirm.
     */
    if (server->disconnected) {
        setupDeferredObjectClose(OI_OBEX_NOT_CONNECTED);
        return;
    }

    if (OI_SUCCESS(status)) {
        /*
         * Report continue or done to client.
         */
#ifdef OI_CODE
#error code not present
#else
        if (server->final) {
          server->objops->Close(server->handle, status, (OI_OPP_CONNECTION) server->id);
          status = OI_OBEXSRV_PutResponse(server->id, NULL, OI_OK);
          OI_FreeIf(&server->rcvObj.name.str);
          OI_FreeIf(&server->rcvObj.type.data);
          OI_MemZero(&server->rcvObj, sizeof(OI_OPP_GENERIC_OBJECT));
          server->state = OPP_SERVER_CONNECTED;
        }
        else {
          status = OI_OBEXSRV_PutResponse(server->id, NULL, OI_OBEX_CONTINUE);
        }
#endif /* OI_CODE */
    } else {
        /*
         * Report error to client.
         */
        retVal = OI_OBEXSRV_PutResponse(server->id, NULL, status);
        if (!OI_SUCCESS(retVal)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("OI_OBEXSRV_PutResponse failed"));
#endif
        }
    }
#ifdef OI_CODE
#error code not present
#else
    if (!OI_SUCCESS(status)) {
#endif
        setupDeferredObjectClose(status);
    }
}


/*
 * Return first body header.
 */
static void FirstBodyHeader(DISPATCH_ARG *darg)
{
    OI_STATUS status;

    server->ofsCfmPending = TRUE;
    status = server->objops->Write(server->handle,
                                   server->rcvObj.objData.data,
                                   server->rcvObj.objData.len,
                                   PushWriteCfm,
                                   (OI_OPP_CONNECTION) server->id);
    server->rcvObj.objData.data = NULL;
    server->rcvObj.objData.len = 0;
    if (!OI_SUCCESS(status)) {
        OI_STATUS   retVal;
    
        OI_LOG_ERROR(("OFS Write() failed %!", status));
        
        server->ofsCfmPending = FALSE;
        /*
         * Report error to client.
         */
        retVal = OI_OBEXSRV_PutResponse(server->id, NULL, status);
        if (!OI_SUCCESS(retVal)) {
            OI_LOG_ERROR(("OI_OBEXSRV_PutResponse failed %!", retVal));
        }
        setupDeferredObjectClose(status);
    }
}


static void PushOpenCfm(OI_OPP_HANDLE handle,
                        OI_STATUS status,
                        OI_OPP_CONNECTION oppContext)
{
    OI_DBGTRACE(("PushOpenCfm %!", status));

    /*
     * First check we are expecting this call.
     */
    if (!OI_INIT_FLAG_VALUE(OPP_SRV)) {
        OI_LOG_ERROR(("%!", OI_STATUS_NOT_INITIALIZED));
        return;
    }
    else if ((server->state != OPP_PUSH_PENDING) || !server->ofsCfmPending) {
        OI_LOG_ERROR(("Push open confirm called at wrong time"));
        return;
    }

    /* 
     * Confirm is no longer pending 
     */
    server->ofsCfmPending = FALSE;
    
    /*
     * If OFS open was successful, we must eventuall call the object close() method.
     * If OFS open failed, we must NOT call the object close() method.
     * Handle is the trigger to call close() or not.
     */
    server->handle = OI_SUCCESS(status)? handle:NULL;
    
    /*
     * Check for disconnect that occurred during wait for this confirm.  
     */
    if (server->disconnected) {
        setupDeferredObjectClose(OI_OBEX_NOT_CONNECTED);
        /* 
         * Disconnected so there's nothing to communicate to the client.
         */
        return;
    }

    if (OI_SUCCESS(status)) {
        server->handle = handle;
        setState(OPP_CLIENT_PUSHING);
        (void) OI_Dispatch_RegisterFunc(FirstBodyHeader, NULL, NULL);
    } else {
        OI_OBEXSRV_PutResponse(server->id, NULL, status);
        setupDeferredObjectClose(status);
    }
}


static OI_STATUS ObjPushInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                            OI_OBEX_HEADER_LIST *cmdHeaders,
                            OI_STATUS obexStatus)
{
    OI_UINT typeLen;
    OI_CHAR *objType;
    OI_STATUS status = obexStatus;

    OI_DBGTRACE(("ObjPushInd %!", obexStatus));

    /* 
     * If an OFS confirm is pending, status must indicate a disconnect event.  Normally,
     *   there cannot be a push indication until we have responded to the previous push 
     *   indication, disconnect is the exception.
     */
    if (server->ofsCfmPending) {
        OI_ASSERT(OI_OBEX_NOT_CONNECTED == obexStatus);
        if (OI_OBEX_NOT_CONNECTED == obexStatus) {
            /* 
             * Ignore this error - we are depending on Obex to call our disconnect indication.
             */
            OI_DBGPRINT(("ObjPushInd while ofsCfmPending, ignoring status %!\n", obexStatus));             
            return OI_OK;
        }
            /*
             * This should never happen, log an error and let the chips fall
             */
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Inappropriate ObjPushInd with status"));
#endif
            return OI_STATUS_INTERNAL_ERROR;
    }

    /*
     * If OBEX reported an error the PUSH operation has been terminated so
     * all we need to do to cleanup is close the object. We return OI_OK
     * because OBEX knows about the error.
     */
    if (!OI_SUCCESS(obexStatus) && (obexStatus != OI_OBEX_CONTINUE)) {
        setupDeferredObjectClose(obexStatus);
        return OI_OK;
    }

    /*
     * Check that the server is allowing PUSH on this connection.
     */
    if (!server->allowPush) {
        OI_DBGPRINT2(("OPP server ObjPushInd - server disallows push"));
        return OI_OBEX_ACCESS_DENIED;
    }

    if (cmdHeaders == NULL) {
        status = OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
        goto ObjPushError;
    }

    if (server->state == OPP_SERVER_CONNECTED) {
        setState(OPP_PUSH_PENDING);
    }

    if ((server->state != OPP_CLIENT_PUSHING) && (server->state != OPP_PUSH_PENDING)) {
        return OI_OBEX_SERVICE_UNAVAILABLE;
    }

    if (obexStatus == OI_OBEX_CONTINUE) {
        server->final = FALSE;
    } else {
        server->final = TRUE;
    }

    status = OPPCommon_ParseHeaders(&server->rcvObj, cmdHeaders);
    if (!OI_SUCCESS(status)) {
        goto ObjPushError;
    }

    if (server->state == OPP_PUSH_PENDING) {
        /*
         * Wait until we have seen a body header before attempting to open the
         * object. This ensures that we have all the other headers we need.
         */
        if (server->final || (server->rcvObj.objData.data != NULL)) {
            /*
             * Name is mandatory.
             */
            if (server->rcvObj.name.str == NULL) {
                status = OI_OBEX_ACCESS_DENIED;
                goto ObjPushError;
            }
            /*
             * Some broken implementations do not provide a type header. So we
             * try to figure out the type from the suffix on the object name.
             *
             * Some broken implementations do not null terminate the type string
             * so we need the size too.
             */
            if (server->rcvObj.type.data == NULL) {
                objType = ObjTypeFromSuffix(&server->rcvObj.name);
                typeLen = OI_StrLen(objType);
            } else {
                objType = (OI_CHAR*) server->rcvObj.type.data;
                typeLen = server->rcvObj.type.len;
            }
            /*
             * Check we support this object type.
             */
            if (!IsSupportedType(objType, typeLen, server->supportedFormats)) {
                status = OI_OBEX_UNSUPPORTED_MEDIA_TYPE;
                goto ObjPushError;
            }
            /*
             * Try and open the object.
             */
            server->ofsCfmPending = TRUE;
            status = server->objops->OpenWrite(&server->rcvObj.name,
                                               objType,
                                               server->rcvObj.objSize,
                                               PushOpenCfm,
                                               (OI_OPP_CONNECTION) server->id);
            if (!OI_SUCCESS(status)) {
                server->ofsCfmPending = FALSE;
            }
        } else {
            /*
             * Get more data from the client.
             */
            status = OI_OBEXSRV_PutResponse(connectionId, NULL, obexStatus);
        }
    } else {
        /*
         * Write data to the object 
         */
        server->ofsCfmPending = TRUE;
        status = server->objops->Write(server->handle,
                                       server->rcvObj.objData.data,
                                       server->rcvObj.objData.len,
                                       PushWriteCfm,
                                       (OI_OPP_CONNECTION) server->id);
        if (!OI_SUCCESS(status)) {
            server->ofsCfmPending = FALSE;
        }
        server->rcvObj.objData.data = NULL;
        server->rcvObj.objData.len = 0;
    }

    if (!OI_SUCCESS(status)) {
        goto ObjPushError;
    }

    return OI_OK;

ObjPushError:
#ifdef OI_CODE
#error code not present
#else
    BT_ERR("OPP server object push failed 0x%x", status, 0, 0);
#endif
    setupDeferredObjectClose(status);
    return status;
}


static void PullOpenCfm(OI_OPP_HANDLE handle,
                        const OI_OBEX_UNICODE *name,
                        const OI_CHAR *type,
                        OI_UINT32 size,
                        OI_STATUS status,
                        OI_OPP_CONNECTION oppContext)
{
    OI_OBEX_HEADER hdrs[3];
    OI_OBEX_HEADER_LIST hdrList;

    OI_DBGTRACE(("PullOpenCfm %!", status));

    /*
     * First check we are expecting this call.
     */
    if (!OI_INIT_FLAG_VALUE(OPP_SRV)) {
        OI_LOG_ERROR(("%!", OI_STATUS_NOT_INITIALIZED));
        return;
    }
    else if ((server->state != OPP_PULL_PENDING) || !server->ofsCfmPending) {
        OI_LOG_ERROR(("Pull open confirm called at wrong time"));
        return;
    }
    
    /* 
     * Confirm is no longer pending 
     */
    server->ofsCfmPending = FALSE;
    
    /*
     * If OFS open was successful, we must eventuall call the object close() method.
     * If OFS open failed, we must NOT call the object close() method.
     * Handle is the trigger to call close() or not.
     */
    server->handle = OI_SUCCESS(status)? handle:NULL;
    
    /*
     * Check for disconnect that occurred during wait for this confirm.  
     */
    if (server->disconnected) {
        setupDeferredObjectClose(OI_OBEX_NOT_CONNECTED);
        /* 
         * Disconnected so there's nothing to communicate to the client.
         */
        return;
    }

    /*
     * If push/pull error occurred while awaiting the cfm, fail the current operation
     */
    if (server->deferredPPError) {
        server->deferredPPError = FALSE;
        status = server->deferredErrorStatus;
    }

    /* 
     * Now finally comes the 'normal' handling of the confirm 
     */
    if (!OI_SUCCESS(status)) {
        /*
         * Reject the pull request by reporting an error status to the client.
         */
        (void) OI_OBEXSRV_GetResponse(server->id, NULL, status);
    } else {
        /*
         * Send the basic info for the default object to the client.
         */
        hdrList.count = 0;
        hdrList.list = hdrs;

        hdrs[hdrList.count].id = OI_OBEX_HDR_LENGTH;
        hdrs[hdrList.count].val.length = size;
        ++hdrList.count;
        hdrs[hdrList.count].id = OI_OBEX_HDR_NAME;
        hdrs[hdrList.count].val.name = *name;
        ++hdrList.count;
        hdrs[hdrList.count].id = OI_OBEX_HDR_TYPE;
        hdrs[hdrList.count].val.type.data = (OI_BYTE*) type;
        hdrs[hdrList.count].val.type.len = OI_StrLen(type) + 1; /* include nul terminator */
        ++hdrList.count;

        status = OI_OBEXSRV_GetResponse(server->id, &hdrList, OI_OBEX_CONTINUE);
    }

    if (OI_SUCCESS(status)) {
        setState(OPP_CLIENT_PULLING);
    } else {
        setupDeferredObjectClose(status);
    }
}


/*
 * Called when to confirm read of data from an object. Passes in the data read
 * from the object.
 */

static void PullReadCfm(OI_OPP_HANDLE handle,
                        OI_BYTE *data,
                        OI_UINT16 len,
                        OI_STATUS status,
                        OI_OPP_CONNECTION oppContext)
{
    OI_STATUS obexStatus;
    OI_OBEX_HEADER bodyHdr;
    OI_OBEX_HEADER_LIST hdrList;

    OI_DBGTRACE(("PullReadCfm %!", status));

    /*
     * First check we are expecting this call.
     */
    if (!OI_INIT_FLAG_VALUE(OPP_SRV)) {
        OI_LOG_ERROR(("%!", OI_STATUS_NOT_INITIALIZED));
        return;
    }
    else if ((server->state != OPP_CLIENT_PULLING) || !server->ofsCfmPending) {
        OI_LOG_ERROR(("Pull read confirm called at wrong time"));
        return;
    }

    /* 
     * Confirm is no longer pending 
     */
    server->ofsCfmPending = FALSE;

    /*
     * Check for disconnect that occurred during wait for this confirm.
     */
    if (server->disconnected) {
        setupDeferredObjectClose(OI_OBEX_NOT_CONNECTED);
        return;
    }

    if (!OI_SUCCESS(status) && (status != OI_STATUS_END_OF_FILE)) {
        /*
         * Report error to client.
         */
        OI_OBEXSRV_GetResponse(server->id, NULL, status);
    } else {
        /*
         * Send the next body header.
         */
        if (status == OI_STATUS_END_OF_FILE) {
            obexStatus = OI_OK;
            bodyHdr.id = OI_OBEX_HDR_END_OF_BODY;
        } else {
            obexStatus = OI_OBEX_CONTINUE;
            bodyHdr.id = OI_OBEX_HDR_BODY;
        }
        bodyHdr.val.body.len = (OI_UINT16) len;
        bodyHdr.val.body.data = data;
        hdrList.count = 1;
        hdrList.list = &bodyHdr;
        /*
         * Send the GET command response to client.
         */
        status = OI_OBEXSRV_GetResponse(server->id, &hdrList, obexStatus);
    }
    /*
     * Close the object if there was an error 
     */
    if (!OI_SUCCESS(status)) {
        setupDeferredObjectClose(status);
    }
}


/*
 * The only object that can be PULLED is the default object - the owner's
 * business card.
 */
static OI_STATUS ObjPullInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                            OI_OBEX_HEADER_LIST *cmdHeaders,
                            OI_STATUS obexStatus)
{
    OI_STATUS status = OI_OK;
    OI_INT i;

    OI_DBGTRACE(("ObjPullInd %!", obexStatus));

    /*
     * If OBEX reported an error the PULL operation has been terminated so
     * all we need to do to cleanup is close the object. We return OI_OK
     * because OBEX already knows about the error.
     */
    if (!OI_SUCCESS(obexStatus) && (obexStatus != OI_OBEX_CONTINUE)) {
        if (server->ofsCfmPending) {
            /*
             * We are waiting for OFS cfm callback, the only time no can close the object yet.
             * Record the fact the a push/pull error occurred and the object
             * close will be handled by the ofs cfm callback.
             * In fact, the only error that can occur while awaiting OFS cfm is DISCONNECT.
             */
            if (obexStatus != OI_OBEX_NOT_CONNECTED) {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("Internal error, ObjPullInd (%!) while awaiting OFS cfm"));
#endif
            } 
            OI_DBGPRINT(("Deferring error handling (%!) until OFS cfm completed", obexStatus));
            server->deferredPPError = TRUE;
            server->deferredErrorStatus = obexStatus;            
        } else {
            setupDeferredObjectClose(obexStatus);
        }    
        return OI_OK;
    }

    /*
     * In the absence of push/pull errors, there should never be a OFS cfm outstanding.
     */
    OI_ASSERT(!server->ofsCfmPending);
    if (server->ofsCfmPending) {
#ifdef OI_CODE        
#error code not present
#else
        OI_LOG_ERROR(("Internal error, ObjPullInd (%!) while awaiting OFS cfm"));
#endif
        server->deferredPPError = TRUE;
        server->deferredErrorStatus = OI_OBEX_ACCESS_DENIED;
        return OI_OBEX_ACCESS_DENIED;
    } 

    /*
     * Check that the server is allowing PULL on this connection.
     */
    if (!server->allowPull) {
        OI_DBGPRINT2(("OPP server ObjPullInd - server disallows pull"));
        return OI_OBEX_ACCESS_DENIED;
    }

    /*
     * If the PULL operation is now complete close the object.
     */
    if (obexStatus == OI_OK) {
        goto ObjPullComplete;
    }

    OI_ASSERT(obexStatus == OI_OBEX_CONTINUE);

    if (server->state == OPP_SERVER_CONNECTED) {
        OI_BD_ADDR addr;
        OI_OBEX_BYTESEQ *type = NULL;

        status = OI_OBEXSRV_GetClientAddr(connectionId, &addr);
        if (!OI_SUCCESS(status)) {
            goto ObjPullComplete;
        }

        OI_DBGPRINT(("OPP server pull request from %:", &addr));

        /*
         * Starting a PULL operation - parse the headers 
         */
        for (i = 0; i < cmdHeaders->count; ++i) {
            OI_OBEX_HEADER *tmpHdr = cmdHeaders->list + i;
            switch (tmpHdr->id) {
                case OI_OBEX_HDR_NAME:
                    /*
                     * Section 4.3.2 of Bluetooth OPP spec says that the name
                     * header must NOT be used for object pull, but the example
                     * in section 8.4.1 of the IrDA OBEX spec (version 1.2)
                     * shows the use of an empty name header. So we will allow
                     * an empty name header...
                     */
                    if (tmpHdr->val.name.len != 0) {
                        status = OI_OBEX_ACCESS_DENIED;
                        OI_LOG_ERROR(("Name header not allowed for object pull"));
                        goto ObjPullComplete;
                    }
                    break;
                case OI_OBEX_HDR_TYPE:
                    type = &(tmpHdr->val.type);
                    break;
                default:
                    /* Ignore other headers. */
                    break;
            }
        }
        if ((type == NULL) || (type->len == 0)) {
            OI_DBGPRINT2(("Object pull no type header"));
            status = OI_OBEX_ACCESS_DENIED;
            goto ObjPullComplete;
        }
        /*
         * vcard is the only object type allowed for object pull.
         * Some broken implementations do not null terminate the type string
         * so we perform string comparison without considering the string terminator.
         */
        if (0 != OI_StrncmpInsensitive( OI_OBEX_VCARD_TYPE, 
                                        (OI_CHAR*) type->data, 
                                        (OI_UINT16) OI_StrLen(OI_OBEX_VCARD_TYPE))) {
            status = OI_OBEX_NOT_FOUND;
            goto ObjPullComplete;
        }
        /*
         * Try to open the default object.  Specify the default object explicitly, type->data from
         * opp client may not be null terminted (contrary to spec).
         */
        setState(OPP_PULL_PENDING);
        server->ofsCfmPending = TRUE;
        status = server->objops->OpenRead(NULL, (OI_CHAR*) OI_OBEX_VCARD_TYPE, PullOpenCfm, (OI_OPP_CONNECTION) server->id);
        if (!OI_SUCCESS(status)) {
            server->ofsCfmPending = FALSE;
            goto ObjPullComplete;
        }
    } else {
        if (server->state != OPP_CLIENT_PULLING) {
            return OI_OBEX_SERVICE_UNAVAILABLE;
        }
        /*
         * Read from the default object.
         */
        server->ofsCfmPending = TRUE;
        status = server->objops->Read(server->handle, server->maxReadSize, PullReadCfm, (OI_OPP_CONNECTION) server->id);
        if (status == OI_STATUS_END_OF_FILE) {
            PullReadCfm(server->handle, NULL, 0, status, (OI_OPP_CONNECTION) server->id);
            status = OI_OK;
        }
        if (!OI_SUCCESS(status)) {
            server->ofsCfmPending = FALSE;
            OI_DBGPRINT(("read from default object returned %!", status));
            goto ObjPullComplete;
        }
    }

    return OI_OK;


ObjPullComplete:

    setupDeferredObjectClose(status);
    return status;
}


OI_STATUS OI_OPP_AcceptConnect(OI_OPP_SERVER_CONNECTION_HANDLE connectionId,
                               OI_BOOL allowPush,
                               OI_BOOL allowPull)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_OPP_AcceptConnect"));

    if (!OI_INIT_FLAG_VALUE(OPP_SRV)) {
        return OI_STATUS_NOT_INITIALIZED;
    }
    if ((server->state != OPP_SERVER_CONNECTING) || (connectionId != server->id)) {
        return OI_OBEX_INVALID_OPERATION;
    }
    if (!allowPush && !allowPull) {
#ifdef OI_CODE
#error code not present
#else
        status = OI_OBEXSRV_AcceptConnect(server->id, FALSE, NULL, OI_OBEX_ACCESS_DENIED);
#endif
        setState(OPP_SERVER_IDLE);
        server->id = 0;
    } else {
#ifdef OI_CODE
#error code not present
#else
        status = OI_OBEXSRV_AcceptConnect(server->id, TRUE, NULL, OI_OK);
#endif
        if (OI_SUCCESS(status)) {
            /*
             * Save access rights.
             */
            server->allowPush = allowPush;
            server->allowPull = allowPull;
            /*
             * Establish the maximum read size for object read operations.
             */
            server->maxReadSize = OI_OBEXSRV_OptimalBodyHeaderSize(server->id);
            /*
             * Server is connected.
             */
            setState(OPP_SERVER_CONNECTED);
        } else {
            server->id = 0;
            setState(OPP_SERVER_IDLE);
        }
    }
    return status;
}


OI_STATUS OI_OPPServer_ForceDisconnect(OI_OPP_SERVER_CONNECTION_HANDLE connectionId)
{
    if (!OI_INIT_FLAG_VALUE(OPP_SRV)) {
        return OI_STATUS_NOT_INITIALIZED;
    }
    if (connectionId != server->id) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    if (server->state < OPP_SERVER_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    }
    
    OI_DBGTRACE(("OI_OPPServer_ForceDisconnect %d, state = %s", connectionId, ServerStateText(server->state)));
    return OI_OBEXSRV_ForceDisconnect(server->id);
}


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
    OI_BD_ADDR clientAddr;
    OI_STATUS status = OI_OK;

    OI_DBGTRACE(("ServerConnectInd"));

    /*
     * OPP does not permit OBEX authentication.
     */
    if (unauthorized) {
        status = OI_OBEX_INVALID_OPERATION;
    } else {
        if (server->id != 0) {
            /*
             * Connection already exists
             */
            status = OI_STATUS_NO_RESOURCES;
        } else {
            server->id = connectionId;
            /*
             * Flush any remaining old context
             */
            server->ofsCfmPending = FALSE;
            server->disconnected = FALSE;
            server->deferredPPError = FALSE;
            
            /*
             * If the application wants to know about connection indications
             * call the connect indication callback, otherwise simply accept the
             * connection.
             */
            if (server->connectInd == NULL) {
                setState(OPP_SERVER_CONNECTING);
                status =  OI_OPP_AcceptConnect(connectionId, TRUE, TRUE);
                if (!OI_SUCCESS(status)) {
                    OI_LOG_ERROR(("OI_OPP_AcceptConnect failed %!", status));
                    setState(OPP_SERVER_IDLE);
                }
                
            } else {
                /*
                 * Pass address of connecting client to application.
                 */
                status = OI_OBEXSRV_GetClientAddr(connectionId, &clientAddr);
                if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("OI_OBEXSRV_GetClientAddr (id = %d) failed"));
#endif
                } else {
                    setState(OPP_SERVER_CONNECTING);
                server->connectInd(&clientAddr, connectionId);
            }
        }
    }
    }
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        (void) OI_OBEXSRV_AcceptConnect(connectionId, FALSE, NULL, status);
#endif
    }
}

#ifdef OI_CODE
#error code not present
#else
static void ServerDisconnectInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                            OI_OBEX_HEADER_LIST *headers)
#endif

{
    OI_DBGTRACE(("ServerDisconnectInd"));

    /* 
     * record the disconnect event for subsequent processes 
     */
    server->disconnected = TRUE;         

    /*
     * If close is already pending on the dispatcher, just call it now
     */
#ifdef OI_CODE
#error code not present
#else
    setupDeferredObjectClose(server->state > OPP_SERVER_CONNECTED? OI_OBEX_NOT_CONNECTED : OI_OK);
#endif
}
#ifndef OI_CODE
static OI_OBEXSRV_CB_LIST CBList = {
    ServerConnectInd,
    ServerDisconnectInd,
    ObjPullInd,
    ObjPushInd,
    NULL,    /* no setpath */
    NULL /* AUTH CB */
};
#else
#error code not present
#endif /* OI_CODE */

#define MAX_OPP_FORMAT   8

#ifdef OI_CODE
#error code not present
#endif /* OI_CODE */

OI_STATUS OI_OPPServer_Register(OI_OPP_SERVER_CONNECT_IND connectInd,
                                OI_OPP_SERVER_DISCONNECT_IND disconnectInd,
                                const OI_OPP_OBJSYS_FUNCTIONS *objectOperations,
                                OI_OPP_SERVER_OBJECT_FORMATS supportedFormats,
                                const OI_SDP_STRINGS *strings,
                                OI_UINT8 *channelNumber)
{
    OI_STATUS status;
    OI_SDPDB_SERVICE_RECORD srec;

    OI_DBGTRACE(("OI_OPPServer_Register"));

    if (OI_INIT_FLAG_VALUE(OPP_SRV)) {
        return OI_STATUS_ALREADY_REGISTERED;
    }

    /*
     * connectInd and disconnectInd must be both NULL or both non-NULL
     */
    if (((connectInd != NULL) && (disconnectInd == NULL))
        || ((connectInd == NULL) && (disconnectInd != NULL))) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    server = OI_Calloc(sizeof(OPP_SERVER));
    if (server == NULL) {
        status = OI_STATUS_NO_RESOURCES;
        goto ErrorExit;
    }

    /*
     * Store connect and disconnect indication callbacks.
     */
    server->connectInd = connectInd;
    server->disconnectInd = disconnectInd;
    server->objops = objectOperations;

    /*
     * Add security policy to our connection policy
     */
    server->connectPolicy.serviceUuid32 = OI_UUID_OBEXObjectPush;
    server->connectPolicy.mustBeMaster = FALSE; 
    server->connectPolicy.flowspec = NULL;
    server->connectPolicy.powerSavingDisables = 0;
    server->connectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(server->connectPolicy.serviceUuid32);

    /*
     * Try and get the preferred OPP channel number.
     */
    server->channel = OI_CONFIG_TABLE_GET(OPP_SRV)->rfcomm_channel_pref;
    status = OI_OBEXSRV_RegisterServer(NULL, &CBList, OI_OBEXSRV_AUTH_NONE, &server->channel, &server->connectPolicy);
    if (!OI_SUCCESS(status)) {
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
     * Initialize supported formats attribute list.
     */
    server->supportedFormats = supportedFormats;
#ifdef OI_CODE
#error code not present
#endif
    /*
     * Create a protocol descriptor list that includes the rfcomm channel
     * number and add this attribute to the service record.
     */
    status = OI_SDP_UTILS_BuildPDLAttribute(OI_UUID_OBEX, server->channel, &server->sdpAttributes[1]);
    if (!OI_SUCCESS(status)) {
        goto ErrorExit;
    }
    /*
     * Set the supported formats and protocol descriptor list attributes.
     */
    status = OI_SDPDB_SetAttributeList(server->srecHandle, server->sdpAttributes, OI_ARRAYSIZE(server->sdpAttributes));
    if (!OI_SUCCESS(status)) {
        goto ErrorExit;
    }

    OI_DEVMGR_RegisterServiceClass(OI_SERVICE_CLASS_OBJECT_TRANSFER);

    OI_DBGPRINT(("Registered Object Push Profile on RFCOMM channel %d", server->channel));

    OI_INIT_FLAG_INCREMENT(OPP_SRV);

    /*
     * Return the channel number to the caller.
     */
    *channelNumber = server->channel;

    return OI_OK;

ErrorExit:

#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("OI_OPPServer_Register failed"));
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


OI_STATUS OI_OPPServer_Deregister(OI_UINT8 channelNumber)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_OPPServer_Deregister, RFCOMM channel %d", channelNumber));

    if (!OI_INIT_FLAG_VALUE(OPP_SRV)) {
        return OI_STATUS_NOT_REGISTERED;
    }

    status = OI_OBEXSRV_DeregisterServer(server->channel);
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Error deregistering OPP server"));
#endif
    } else {
        OI_DEVMGR_DeregisterServiceClass(OI_SERVICE_CLASS_OBJECT_TRANSFER);
        (void) OI_SDPDB_RemoveAttributeList(server->srecHandle, NULL, NULL);
        OI_SDPDB_FreeAttributeListElements(server->sdpAttributes, OI_ARRAYSIZE(server->sdpAttributes));
        (void) OI_SDPDB_RemoveServiceRecord(server->srecHandle);
        OI_Free(server);
        server = NULL;
        OI_INIT_FLAG_DECREMENT(OPP_SRV);
    }
    return status;
}

#ifdef OI_DEPRECATED
static OI_UINT8 deprecatedChannel;

/**
 * @deprecated
 */
OI_STATUS OI_OPPServer_Init(OI_OPP_SERVER_CONNECT_IND connectInd,
                            OI_OPP_SERVER_DISCONNECT_IND disconnectInd,
                            const OI_OPP_OBJSYS_FUNCTIONS *objectOperations,
                            const OI_DATAELEM *supportedFormatsSeq,
                            const OI_SDP_STRINGS *strings)
{
    OI_UINT i;
    OI_OPP_SERVER_OBJECT_FORMATS supportedFormats = 0;

    if (OI_INIT_FLAG_VALUE(OPP_SRV)) {
        return OI_STATUS_ALREADY_INITIALIZED;
    }

    /*
     * Validate supported formats: a data element list of UINT8 format specifiers.
     */
    if (supportedFormatsSeq->ElemType != OI_DATAELEM_SEQ) {
        OI_LOG_ERROR(("Supported formats must be a data element sequence"));
        return OI_STATUS_INVALID_PARAMETERS;
    }
    for (i = 0; i < supportedFormatsSeq->Size; ++i) {
        OI_DATAELEM *fmt = &supportedFormatsSeq->Value.ElemSeq[i];
        if ((fmt->ElemType != OI_DATAELEM_UINT) || (fmt->Size != sizeof(OI_UINT8))) {
            OI_LOG_ERROR(("Supported formats must be UINT8 data elements"));
            return OI_STATUS_INVALID_PARAMETERS;
        }
        if (fmt->Value.UInt == OI_OBEX_OBJ_FORMAT_ANY) {
            supportedFormats = OI_OPP_SERVER_OBJ_FORMAT_ANY; 
        } else {
            supportedFormats |= 1 << (fmt->Value.UInt - 1);
        }
    }

    OI_DBGPRINT(("Supported formats %lx\n%^", supportedFormats, supportedFormatsSeq));

    return OI_OPPServer_Register(connectInd,
                                 disconnectInd,
                                 (OI_OPP_OBJSYS_FUNCTIONS*) objectOperations,
                                 supportedFormats,
                                 strings,
                                 &deprecatedChannel);
}

/**
 * @deprecated
 */
OI_STATUS OI_OPPServer_Reset(void)
{
    return OI_OK;
}

/**
 * @deprecated
 */
OI_STATUS OI_OPPServer_Terminate(void)
{
    if (!OI_INIT_FLAG_VALUE(OPP_SRV)) {
        return OI_STATUS_NOT_INITIALIZED;
    } else {
        return OI_OPPServer_Deregister(deprecatedChannel);
    }
}
#endif /* OI_DEPRECATED */

OI_STATUS OI_OPPServer_GetServiceRecord(OI_UINT32 *handle)
{
    if (!handle) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    OI_CHECK_INIT(OPP_SRV);

    *handle = server->srecHandle;
    return OI_OK;
}


