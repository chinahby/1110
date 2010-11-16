/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/opp_client.c#2 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-01-14  gs   Added support for allowing connection can be terminated
                   before it is accepted by remote device.

*    #13       18 Oct  2006           PR
*  Merged OI Code v3.0.14rev1revQ
*
*    #12       19 Sept 2006           MH
*  Changed to support OI client connect
*
*    #10-11    10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #9        23 Jun 2006            JH
* Modified to work with updated OBEX code which supports JSR82.
*
*    #8        19 Jun 2006            MP
* OBEX now supports the nameless header obex objects
*
*    #7        16 Nov 2005            GS
* Don't call CloseObject in OI_OPPClient_Abort().  Instead, do the cleanup
* after Abort response is received.  This will prevent BT PF from getting
* stuck in a bad state if Disconnect is called too soon.
*
*    #6        12 Aug 2005            JH
* Check for NULL pointer before calling OI_Dispatch_RegisterFunc in CloseObject.
* BT PF will need to generate the object close req.
*
*    #5        08 Jul 2005            JH
* Modified OI_OPPClient_Abort() to call OI_OBEXCLI_Put()/OI_OBEXCLI_Get()
*  immediately after OI_OBEXCLI_AbortOperation() to trigger an immediate abort.
*
*    #4        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
*    #3        26 Jul 2004            JH
* Removed a case where CloseObject is called redundantly in PullOpenCfm
*  (when client->final is true).
*
===========================================================================*/
/**
@file
@internal
  
Object Push Client
*/

/**********************************************************************************
  $Revision: #2 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#define __OI_MODULE__ OI_MODULE_OPP_CLI

#include "oi_bt_assigned_nos.h"
#include "oi_memmgr.h"
#include "oi_obexcli.h"
#include "oi_opp_client.h"
#include "oi_opp_common.h"
#include "oi_dispatch.h"

#include "oi_utils.h"
#include "oi_debug.h"
#include "oi_argcheck.h"
#include "oi_assert.h"
#include "oi_securitydb.h"

#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"


/**
 * These are the various client connection states.
 */

typedef enum {
    CLIENT_STATE_DISCONNECTED,      /**< Initial state */
#ifndef OI_CODE
    CLIENT_STATE_CONNECTING,        /**< Connection in progress */
#endif
    CLIENT_STATE_CONNECTED,         /**< Connected, no operations in progress */
    CLIENT_STATE_PUSH_PENDING,      /**< Waiting on OFS read open confirm before starting 1st PUT */
    CLIENT_STATE_PUSHING,           /**< Putting an object */
    CLIENT_STATE_PULL_PENDING,      /**< From issuing 1st get until OFS write open confirm is called */
    CLIENT_STATE_PULLING,           /**< While getting and OFS writing */
    CLIENT_STATE_ABORTING
} OPP_CLIENT_STATE;


/**
 * Struct for an OPP client connection.
 */

typedef struct {

    OPP_CLIENT_STATE state;                 /**< State of this connection */
    OI_OPP_GENERIC_OBJECT rcvObj;           /**< Keeps track of received object state */
    OI_OBEXCLI_CONNECTION_HANDLE id;        /**< The underlying OBEX client connection */
    OI_OPP_CLIENT_EVENT_CB eventCB;         /**< Callback function to report operation completion */
    OI_OPP_CLIENT_CANCEL_CFM cancelCfm;     /**< Callback function to report operation cancellation completion */
    const OI_OPP_OBJSYS_FUNCTIONS *objops;  /**< Interface to an object filing system */
    OI_OPP_HANDLE handle;                   /**< The handle for object operations */
    OI_BOOL final;                          /**< Indicates if this is a final packet */
    OI_UINT16 maxReadSize;                  /**< Maximum packet read size */
    OI_CONNECT_POLICY connectPolicy;        /**< Security policy for this connection */
    DISPATCH_CB_HANDLE pendingClose;        /**< Close operation that has been registered with the dispatcher */
    OI_BOOL     ofsCfmPending;              /**< Waiting for Object File System to call our cfm function */
    OI_BOOL     disconnected;               /**< Received disconnect indication, waiting to call event callback. */

} OPP_CLIENT;


static OPP_CLIENT *client;


#define IS_CLIENT_CONNECTED   (OI_INIT_FLAG_VALUE(OPP_CLI) && (client->state >= CLIENT_STATE_CONNECTED))
#define IS_CLIENT_IDLE        (OI_INIT_FLAG_VALUE(OPP_CLI) && (client->state == CLIENT_STATE_CONNECTED))


#ifdef OI_DEBUG
#error code not present
#else

#define ClientStateText(state)  ("")

#define setState(newState)  (client->state = newState)

#endif  /* OI_DEBUG */


/*********************** FORWARD DEFINITIONS ***************************/

static void ClientPutCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                         OI_OBEX_HEADER_LIST *rspHeaders,
                         OI_STATUS status);

static void CloseObject(OI_STATUS status);


/******************* END OF FORWARD DEFINITIONS ************************/


static void ClientConnectCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                             OI_BOOL readOnly, /* not used for OPP */
#ifndef OI_CODE
                             OI_OBEX_HEADER_LIST *hdrs, /* ignored */
#endif
                             OI_STATUS status)
{
    OI_ASSERT(connectionId == client->id);

    OI_DBGTRACE(("ClientConnectCfm %!, id = %d", status, connectionId));
    
    if (OI_SUCCESS(status)) {
        OI_DBGPRINT2(("OPP client connected"));
        setState(CLIENT_STATE_CONNECTED);
        /*
         * Establish the maximum read size for object read operations.
         */
        client->maxReadSize = OI_OBEXCLI_OptimalBodyHeaderSize(client->id);
    }
    client->eventCB(connectionId, OI_OPP_CLIENT_CONNECTED, status);
    /*
     * Clean up if the connection attempt failed.
     */
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT2(("OPP client connect failed %!", status));
        OI_Free(client);
        client = NULL;
        OI_INIT_FLAG_PUT_FLAG(FALSE, OPP_CLI);
    }
}

#ifdef OI_CODE
#error code not present
#else
static void ClientDisconnectInd(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                OI_OBEX_HEADER_LIST *hdrs) /* ignored */
#endif
{
    OI_DBGTRACE(("ClientDisconnectInd, id = %d", connectionId));

    /*
     * record the disconnect event for subsequent processes 
     */
    client->disconnected = TRUE;         

    /*
     * If close is already pending, just call it now
     */
    if (client->pendingClose) {
        OI_DBGPRINT2(("Disconnect : calling close immediately"));
        OI_Dispatch_CallFunc(client->pendingClose);
        /* 
         * The client object has (hopefully) been freed by the pendingClose().
         * Disconnect has been handled completely.
         */
        return;
    }
    if (client->ofsCfmPending) {
        /*
         * We're waiting for an OFS confirm callback, just have to keep waiting
         */
    } else {
        /*
         * Status to be reported to OFS and app depends on whether we're idle or pushing/pulling
         */
        CloseObject(client->state > CLIENT_STATE_CONNECTED? OI_OBEX_NOT_CONNECTED : OI_OK);
    }
}


/**
 * Connect to remote OBEX object push profile server.
 */

OI_STATUS OI_OPPClient_Connect(OI_BD_ADDR *addr,
                               OI_UINT channel,
                               OI_OPP_CLIENT_CONNECTION_HANDLE *connectionId,
                               OI_OPP_CLIENT_EVENT_CB eventCB,
                               const OI_OPP_OBJSYS_FUNCTIONS *objectFunctions)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_OPPClient_Connect %:", addr));

    ARGCHECK(eventCB && objectFunctions);

    /* 
     * Current implementation only allows one OPP connection at a time. 
     */
    if (OI_INIT_FLAG_VALUE(OPP_CLI)) {
        return OI_STATUS_ALREADY_CONNECTED;
    }

    client = OI_Calloc(sizeof(OPP_CLIENT));
    if (client == NULL) {
        return OI_STATUS_NO_RESOURCES;
    }

    client->eventCB = eventCB;
    client->objops = objectFunctions;
    
    /*
     * Add security policy to our connection policy
     */

    client->connectPolicy.serviceUuid32 = OI_UUID_OBEXObjectPush;
    client->connectPolicy.mustBeMaster = FALSE; 
    client->connectPolicy.flowspec = NULL;
    client->connectPolicy.powerSavingDisables = 0;
    client->connectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(client->connectPolicy.serviceUuid32);

    status = OI_OBEXCLI_Connect(addr,
                                (OI_UINT8) channel,
                                NULL,
                                ClientConnectCfm,
                                ClientDisconnectInd,
                                NULL, /* no authentication challenge */
#ifndef OI_CODE
                                NULL, /* no authentication response */
#endif
                                connectionId,
                                &client->connectPolicy);

    if (OI_SUCCESS(status)) {
        client->id = *connectionId;
        /* 
         * Set our initialization flag TRUE - we're initialized 
         */
        OI_INIT_FLAG_PUT_FLAG(TRUE, OPP_CLI);

#ifndef OI_CODE
        client->state = CLIENT_STATE_CONNECTING;
#endif
    } else {
        OI_Free(client);
        OI_DBGPRINT(("OI_OBEXCLI_Connect failed (%!)", status));
    }

    return status;
}


OI_STATUS OI_OPPClient_Disconnect(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_OPPClient_Disconnect, id %d", connectionId));

#ifdef OI_CODE
#error code not present
#else
    if (!IS_CLIENT_CONNECTED && client->state != CLIENT_STATE_CONNECTING ) {
#endif
        OI_DBGPRINT(("Not connected"));
        return OI_OBEX_NOT_CONNECTED;
    }
#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXCLI_Disconnect(client->id, NULL);
#endif
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("OI_OBEXCLI_Disconnect failed (%!)", status));
    }
    return status;
}


static void CloseObjectCB(DISPATCH_ARG *darg)
{
    OI_STATUS status = Dispatch_GetArg(darg, OI_STATUS);
    OPP_CLIENT_STATE snapState;

    if (client == NULL) {
        OI_LOG_ERROR(("CloseObject - client has been freed"));
        return;
    }

    snapState = client->state;
    client->pendingClose = 0;

    /*
     * Free any saved header data.
     */
    OI_FreeIf(&client->rcvObj.name.str);
    OI_FreeIf(&client->rcvObj.type.data);
    OI_MemZero(&client->rcvObj, sizeof(OI_OPP_GENERIC_OBJECT));

    /*
     * Reset state before calling event callback in case the callback function
     * calls right back into OPP.
     */
    client->final = FALSE;
    if (client->disconnected) {
        setState(CLIENT_STATE_DISCONNECTED);
    } else if (client->state > CLIENT_STATE_CONNECTED) {
        setState(CLIENT_STATE_CONNECTED);
    }
    
    switch (snapState) {
        case CLIENT_STATE_PUSHING:
            client->objops->Close(client->handle, status, (OI_OPP_CONNECTION) client->id);
            /* falling through */
        case CLIENT_STATE_PUSH_PENDING:
            client->eventCB(client->id, OI_OPP_CLIENT_PUSH_COMPLETE, status);
            break;
        case CLIENT_STATE_PULLING:
            client->objops->Close(client->handle, status, (OI_OPP_CONNECTION) client->id);
            /* falling through */
        case CLIENT_STATE_PULL_PENDING:
            client->eventCB(client->id, OI_OPP_CLIENT_PULL_COMPLETE, status);
            break;
        default:
            break;
    }

    /* If disconnected, tell the application and re-initialize ourselves */
    if (client->disconnected) {
        client->eventCB((OI_OPP_CONNECTION) client->id, OI_OPP_CLIENT_DISCONNECT, OI_OK);
        OI_Free(client);
        client = NULL;
        OI_INIT_FLAG_PUT_FLAG(FALSE, OPP_CLI);
    }
}


/*
 * Put a callback function on the dispatcher to close the object.
 */
static void CloseObject(OI_STATUS status)
{
    DISPATCH_ARG darg;

    OI_DBGPRINT2(("CloseObject"));
    Dispatch_SetArg(darg, status);
#ifdef OI_CODE
#error code not present
#else
    if (client != NULL)
    {
      (void) OI_Dispatch_RegisterFunc(CloseObjectCB, &darg, &client->pendingClose);
    }
#endif /* OI_CODE */
}


static void PushReadCfm(OI_OPP_HANDLE handle,
                        OI_BYTE *data,
                        OI_UINT16 len,
                        OI_STATUS status,
                        OI_OPP_CONNECTION oppContext)
{
    OI_STATUS obexStatus;
    OI_OBEX_HEADER bodyHdr;
    OI_OBEX_HEADER_LIST hdrList;

    if (!OI_INIT_FLAG_VALUE(OPP_CLI) || (client->state != CLIENT_STATE_PUSHING) || !client->ofsCfmPending) {
        OI_LOG_ERROR(("Push read confirm called at wrong time"));
        return;
    }
    
    OI_DBGPRINT2(("PushReadCfm %d bytes %!", len, status));

    /* 
     * Confirm is no longer pending 
     */
    client->ofsCfmPending = FALSE;


    /*
     * Check for disconnect that occurred during wait for this confirm.
     */
    if (client->disconnected) {
        CloseObject(OI_OBEX_NOT_CONNECTED);
        return;
    }

    if (status == OI_STATUS_END_OF_FILE) {
        status = OI_OK;
        obexStatus = OI_OK;
        bodyHdr.id = OI_OBEX_HDR_END_OF_BODY;
        client->final = TRUE;
    } else {
        obexStatus = OI_OBEX_CONTINUE;
        bodyHdr.id = OI_OBEX_HDR_BODY;
    }
    if (!OI_SUCCESS(status)) {
        /*
         * Let obex know the put operation terminated with an error.  The object will be closed
         * when the putCfm is called with failed status.
         */
        status = OI_OBEXCLI_Put(client->id, NULL, ClientPutCfm, status);
    } else {
        OI_ASSERT((len == 0) || (data != NULL));
        bodyHdr.val.body.len = len;
        bodyHdr.val.body.data = data;
        hdrList.count = 1;
        hdrList.list = &bodyHdr;
        status = OI_OBEXCLI_Put(client->id, &hdrList, ClientPutCfm, obexStatus);
    }
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("PushReadCfm closing object %!", status));
        CloseObject(status);
    }
}


static void ClientPutCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                         OI_OBEX_HEADER_LIST *rspHeaders,
                         OI_STATUS status)
{
    OI_DBGPRINT2(("OPP client put confirm %!", status));

    if (status == OI_OBEX_CONTINUE) {
        if (client->final) {
            /*
             * We should not get a continue response after we have sent the
             * final packet.
             */
            OI_LOG_ERROR(("OBEX incorrect continue reponse"));
            status = OI_OBEX_INVALID_OPERATION;
        } else {
            /* 
             * Set pending first because the confirm may be called from within the OFS function 
             */
            client->ofsCfmPending = TRUE;

            
            status = client->objops->Read(client->handle, client->maxReadSize, PushReadCfm, (OI_OPP_CONNECTION) client->id);

			
            if (status == OI_STATUS_END_OF_FILE) {
                PushReadCfm(client->handle, NULL, 0, status, (OI_OPP_CONNECTION) client->id);
                status = OI_OK;
            }
            if (!OI_SUCCESS(status)) {
                /* 
                 * If OFS returns error, there is no pending confirm callback 
                 */
                client->ofsCfmPending = FALSE;
            }
        }
        if (!OI_SUCCESS(status)) {
            /*
             * Let obex know the put operation terminated with an error.
             */
            OI_OBEXCLI_Put(client->id, NULL, NULL, status);
            CloseObject(status);
        }
    } else {
        /*
         * We are either done or got an error, in either case we need to close
         * the object.
         */
        CloseObject(status);
    }
}


static void PushOpenCfm(OI_OPP_HANDLE handle,
                        const OI_OBEX_UNICODE *name,
                        const OI_CHAR *type,
                        OI_UINT32 size,
                        OI_STATUS status,
                        OI_OPP_CONNECTION oppContext)
{
    OI_UINT16 len;
    OI_OBEX_HEADER hdrs[3];
    OI_OBEX_HEADER_LIST hdrList;

    if (!OI_INIT_FLAG_VALUE(OPP_CLI) || (client->state != CLIENT_STATE_PUSH_PENDING) || !client->ofsCfmPending) {
        OI_LOG_ERROR(("Push open confirm called at wrong time"));
        return;
    }

    OI_DBGPRINT2(("PushOpenCfm %!", status));

    
    /* 
     * Confirm is no longer pending 
     */
    client->ofsCfmPending = FALSE;

    /*
     * Check for disconnect that occurred during wait for this confirm.
     */
    if (client->disconnected) 
	{
        if (OI_SUCCESS(status)) 
		{
            /* 
             * OFS reported a successful open, set state to PUSHING to trigger close() callback.
             */
            setState(CLIENT_STATE_PUSHING);
        }
        CloseObject(OI_OBEX_NOT_CONNECTED);
        return;
    }

    hdrList.list = hdrs;
    hdrList.count = 0;


    if (OI_SUCCESS(status)) 
	{
        setState(CLIENT_STATE_PUSHING);
        client->handle = handle;
        /*
         * Send the initial object headers.
         */
        hdrs[hdrList.count].id = OI_OBEX_HDR_LENGTH;
        hdrs[hdrList.count].val.length = size;
        ++hdrList.count;
        hdrs[hdrList.count].id = OI_OBEX_HDR_NAME;
        hdrs[hdrList.count].val.name = *name;
        ++hdrList.count;
        /*
         * Type header is optional
         */
        if (type != NULL) 
		{
            len = OI_StrLen(type);
            if ( len > 0 )
			{
                hdrs[hdrList.count].id = OI_OBEX_HDR_TYPE;
                hdrs[hdrList.count].val.type.data = (OI_BYTE*) type;
                hdrs[hdrList.count].val.type.len = len + 1; /* include nul termination */
                ++hdrList.count;
            }
        } 
        status = OI_OBEXCLI_Put(client->id, &hdrList, ClientPutCfm, OI_OBEX_CONTINUE);
    }

    if (!OI_SUCCESS(status)) {
        CloseObject(status);
    }
}


OI_STATUS OI_OPPClient_Push(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId,
                            const OI_OBEX_UNICODE *name,
                            const OI_CHAR *type)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_OPPClient_Push, id %d", connectionId));

    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("OPP client not connected"));
        return OI_OBEX_NOT_CONNECTED;
    }
    if (!IS_CLIENT_IDLE) {
        OI_DBGPRINT(("Operation already in progress client state = %s", ClientStateText(client->state)));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    client->final = FALSE;
    setState(CLIENT_STATE_PUSH_PENDING);

    /* 
     * Set pending first because the confirm may be called from within the OFS function 
     */
    client->ofsCfmPending = TRUE;

    status = client->objops->OpenRead(name, type, PushOpenCfm, (OI_OPP_CONNECTION) client->id);
    if (!OI_SUCCESS(status)) {
        setState(CLIENT_STATE_CONNECTED);
        /* 
         * If OFS returns error, there is no pending confirm callback 
         */
        client->ofsCfmPending = FALSE;
    }
    return status;
}

static void PullAbortCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId)
{
    OI_DBGTRACE(("PullAbortCfm, id %d", connectionId));
    CloseObject(OI_OBEX_OFS_ERROR);
}

static void PullWriteCfm(OI_OPP_HANDLE handle,
                         OI_STATUS status,
                         OI_OPP_CONNECTION oppContext);


static void GetRcvData(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                       OI_OBEX_HEADER_LIST *rspHeaders,
                       OI_STATUS rcvStatus)
{
    OI_STATUS status = rcvStatus;

    if (OI_SUCCESS(rcvStatus) || (rcvStatus == OI_OBEX_CONTINUE)) {
        client->final = (rcvStatus == OI_OK);
        status = OPPCommon_ParseHeaders(&client->rcvObj, rspHeaders);
        if (OI_SUCCESS(status)) {
            client->ofsCfmPending = TRUE;
            status = client->objops->Write(client->handle,
                                           client->rcvObj.objData.data,
                                           client->rcvObj.objData.len,
                                           PullWriteCfm,
                                           (OI_OPP_CONNECTION) client->id);
            client->rcvObj.objData.data = NULL;
            client->rcvObj.objData.len = 0;
            if (!OI_SUCCESS(status)) {
                client->ofsCfmPending = FALSE;
            }
        }
    }
    if (!OI_SUCCESS(status)) {
        CloseObject(status);
    }
}


static void PullWriteCfm(OI_OPP_HANDLE handle,
                         OI_STATUS status,
                         OI_OPP_CONNECTION oppContext)
{
    if (!OI_INIT_FLAG_VALUE(OPP_CLI) || (client->state != CLIENT_STATE_PULLING) || !client->ofsCfmPending) {
        OI_LOG_ERROR(("Pull write confirm called at wrong time"));
        return;
    }
    
    OI_DBGPRINT2(("PullWriteCfm %!\n", status));
    
    /* 
     * Confirm is no longer pending 
     */
    client->ofsCfmPending = FALSE;

    /*
     * Check for disconnect that occurred during wait for this confirm.
     */
    if (client->disconnected) {
        CloseObject(OI_OBEX_NOT_CONNECTED);
        return;
    }

    /*
     * If we had not received the final packet call for more.
     */
    if (!client->final) {
        if (!OI_SUCCESS(status)) {
            /*
             * Let OBEX know we are terminating the GET because of an error.
             */
            status = OI_OBEXCLI_Abort(client->id, PullAbortCfm);
#ifndef OI_CODE
            if (status == OI_STATUS_PENDING) {
                /* In good state to abort.  Send Abort now */
                BT_MSG_DEBUG("PullWriteCfm: File write error.. Aborting", 0, 0, 0); 
                status = OI_OBEXCLI_Get(client->id, NULL, GetRcvData, TRUE);
            }
#endif
        } else {
            status = OI_OBEXCLI_Get(client->id, NULL, GetRcvData, TRUE);
        }
    } 
    /*
     * On error or when we are done pulling, close the object.
     */
    if (client->final || !OI_SUCCESS(status)) {
        CloseObject(status);
    }
}


/*
 * Return first body header.
 */
static void FirstBodyHeader(DISPATCH_ARG *darg)
{
    OI_STATUS status;

    /* 
     * Set pending first because the confirm may be called from within the OFS function 
     */
    client->ofsCfmPending = TRUE;

    status = client->objops->Write(client->handle,
                                   client->rcvObj.objData.data,
                                   client->rcvObj.objData.len,
                                   PullWriteCfm,
                                   (OI_OPP_CONNECTION) client->id);
    if (!OI_SUCCESS(status)) {
        /* 
         * If OFS returns error, there is no pending confirm callback 
         */
        client->ofsCfmPending = FALSE;
    }
                                   
    client->rcvObj.objData.data = NULL;
    client->rcvObj.objData.len = 0;
    if (!OI_SUCCESS(status)) {
        CloseObject(status);
    }
}


static void PullOpenCfm(OI_OPP_HANDLE handle,
                        OI_STATUS status,
                        OI_OPP_CONNECTION oppContext)
{
    if (!OI_INIT_FLAG_VALUE(OPP_CLI) || (client->state != CLIENT_STATE_PULL_PENDING) || !client->ofsCfmPending) {
        OI_LOG_ERROR(("Pull open confirm called at wrong time"));
        return;
    }
    
    OI_DBGPRINT2(("PullOpenCfm %!",status));

    /* 
     * Confirm is no longer pending 
     */
    client->ofsCfmPending = FALSE;

    /*
     * Check for disconnect that occurred during wait for this confirm.  
     */
    if (client->disconnected) {
        if (OI_SUCCESS(status)) {
            /* 
             * OFS reported a successful open, set state to PULLING to trigger close() callback.
             */
            setState(CLIENT_STATE_PULLING);
        }
        CloseObject(OI_OBEX_NOT_CONNECTED);
        return;
    }

    if (!OI_SUCCESS(status)) {
        /*
         * If the operation is not complete, let OBEX know we are terminating
         * the GET because of an error.
         */
        if (!client->final) {
            status = OI_OBEXCLI_Abort(client->id, PullAbortCfm);
            if (!OI_SUCCESS(status)) {
                /*
                 * The abort failed, close the object terminating the current operation
                 */
                CloseObject(status);                     
            }
        } else {
            /*
                     * On error when we are done pulling, close the object.
             */
            CloseObject(status);
        }
    } else {
        setState(CLIENT_STATE_PULLING);
        client->handle = handle;
        (void) OI_Dispatch_RegisterFunc(FirstBodyHeader, NULL, NULL);
    }
}


static void GetRcv(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                   OI_OBEX_HEADER_LIST *rspHeaders,
                   OI_STATUS rcvStatus)
{
    OI_STATUS status = rcvStatus;

    if (OI_SUCCESS(rcvStatus) || (rcvStatus == OI_OBEX_CONTINUE)) {
        client->final = (rcvStatus == OI_OK);
        status = OPPCommon_ParseHeaders(&client->rcvObj, rspHeaders);
        /*
         * Wait until we have seen a body header before attempting to open the
         * object.
         */
        if (client->final || (client->rcvObj.objData.data != NULL)) {
            /* The IrMC OBEX spec and Bluetooth OPP spec disagree on whether
             * the NAME header is required or not.  In the name of greatest
             * compatibility, a NULL file name will be passed to the
             * application for the application to deal with the issue.
             */
            if (client->rcvObj.name.str == NULL || client->rcvObj.name.len == 0 ||
                client->rcvObj.name.str[0] == '\0') {
                OI_LOG_ERROR(("Remote device failed to provide mandatory NAME header.\n"));
            } 
            /*
                 * Set pending first because the confirm may be called from within the OFS function 
             */
                client->ofsCfmPending = TRUE;
                
                status = client->objops->OpenWrite(&client->rcvObj.name,
                                                   (OI_CHAR*) client->rcvObj.type.data,
                                                   client->rcvObj.objSize,
                                                   PullOpenCfm,
                                                   (OI_OPP_CONNECTION) client->id);
                if (!OI_SUCCESS(status)) {
                    /* 
                     * If OFS returns error, there is no pending confirm callback 
                     */
                    client->ofsCfmPending = FALSE;
             }
        } else {
            status = OI_OBEXCLI_Get(client->id, NULL, GetRcv, TRUE);
        }
    }
    if (!OI_SUCCESS(status)) {
        CloseObject(status);
    }
}


/*
 * Pull the default object.
 */

OI_STATUS OI_OPPClient_Pull(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId)
{
    static const OI_CHAR typeName[] = OI_OBEX_VCARD_TYPE;
    OI_STATUS status;
    OI_OBEX_HEADER hdr;
    OI_OBEX_HEADER_LIST hdrList;

    OI_DBGTRACE(("OI_OPPClient_Pull, id %d", connectionId));

    if (!IS_CLIENT_CONNECTED) {
        OI_DBGPRINT(("OPP client not connected"));
        return OI_OBEX_NOT_CONNECTED;
    }
    if (!IS_CLIENT_IDLE) {
        OI_DBGPRINT(("Operation already in progress client state = %s", ClientStateText(client->state)));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    client->final = FALSE;
    setState(CLIENT_STATE_PULL_PENDING);

    /*
     * There is only one header required for pulling the default object. A type
     * header initialized to the vcard type string.
     */
    hdr.id = OI_OBEX_HDR_TYPE;
    hdr.val.type.data = (OI_BYTE*) typeName;
    hdr.val.type.len = OI_ARRAYSIZE(typeName);

    hdrList.list = &hdr;
    hdrList.count = 1;
    status = OI_OBEXCLI_Get(client->id, &hdrList, GetRcv, TRUE);

    if (!OI_SUCCESS(status)) {
        setState(CLIENT_STATE_CONNECTED);
    }
    return status;
}



static void AbortCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId)
{
    if (client->cancelCfm) {
        client->cancelCfm((OI_OPP_CLIENT_CONNECTION_HANDLE)connectionId);
    }

    if (client->state == CLIENT_STATE_ABORTING) {
        setState(CLIENT_STATE_CONNECTED);
    }

    CloseObject(OI_OBEX_CLIENT_ABORTED_COMMAND);
}


OI_STATUS OI_OPPClient_Cancel(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId,
                              OI_OPP_CLIENT_CANCEL_CFM cancelCfm)
{
    OI_STATUS status;

    OI_DBGTRACE(("OI_OPPClient_Cancel, id %d", connectionId));
    
    if (!IS_CLIENT_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    }
    if (IS_CLIENT_IDLE) {
        return OI_OBEX_INVALID_OPERATION;
    }
#ifdef OI_CODE
#error code not present
#else
    /* If we know that we have recived the final packet
       we need not send the Abort Out */
    if(!client->final)
    {
        status = OI_OBEXCLI_Abort(client->id, AbortCfm);
        client->cancelCfm = cancelCfm;
    }
    else
    {
        status = OI_STATUS_INVALID_STATE;
    }
#endif /* OI_CODE */
    return status;
}


/**************************************************************************
 *                         Deprecated
 **************************************************************************/

#ifdef OI_DEPRECATED
static OI_OPP_CLIENT_EVENT_CB  deprecatedEventCB;
static const OI_OPP_OBJSYS_FUNCTIONS *deprecatedObjectFunctions;

/**
 * @deprecated
 */
OI_STATUS OI_OPPClient_Connect_10(OI_BD_ADDR *addr,
                                  OI_UINT channel,
                                  OI_OPP_CLIENT_CONNECTION_HANDLE *connectionId)
{
    return OI_OPPClient_Connect(addr, channel, connectionId, deprecatedEventCB, deprecatedObjectFunctions);
}

/**
 * @deprecated
 */
OI_STATUS OI_OPPClient_Init(OI_OPP_CLIENT_EVENT_CB eventCB,
                            const OI_OPP_OBJSYS_FUNCTIONS *objectFunctions)
{
    deprecatedEventCB = eventCB;
    deprecatedObjectFunctions = (OI_OPP_OBJSYS_FUNCTIONS*) objectFunctions;
    return OI_OK;
}

/**
 * @deprecated
 */
OI_STATUS OI_OPPClient_Terminate(void)
{
    deprecatedEventCB = NULL;
    deprecatedObjectFunctions = NULL;
    return OI_OK;
}

/**
 * @deprecated
 */
OI_STATUS OI_OPPClient_Abort(OI_OPP_CLIENT_CONNECTION_HANDLE connectionId)
{
#ifndef OI_CODE
    OI_STATUS status;
    OPP_CLIENT_STATE snapState = client->state;
#endif
    if (!IS_CLIENT_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    }
    if (IS_CLIENT_IDLE) {
        return OI_OBEX_INVALID_OPERATION;
    }
#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXCLI_AbortOperation(client->id);
 
    /* 
     * To abort immediately, without waiting for a put/get response from the
     * server, call the put/get command immediately after the abort command.
     */

	
    switch (snapState) {
        case CLIENT_STATE_PUSHING:
            (void) OI_OBEXCLI_Put(client->id, NULL, ClientPutCfm, OI_OK);
            break;
        case CLIENT_STATE_PULLING:
            (void) OI_OBEXCLI_Get(client->id, NULL, GetRcvData, TRUE);        
            break;
        default:
            break;
    }
    return status;
#endif /* OI_CODE */
}

#endif /* OI_DEPRECATED */

