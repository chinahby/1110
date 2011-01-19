#ifdef FEATURE_BT_EXTPF_CTP
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/tcs_cc.c#1 $ 

$Log: $
*
*    #6        21 Dec 2006          DM
* Changed the OI_L2CAP_Register() interface to pass Default config parameters 
* to be used for the registered PSM
*
*    #5        14 Mar 2006            RY
* Added proper featurization.
*
*    #4        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
===========================================================================*/

/**
 * @file
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_TCS

#include "oi_tcs_spec.h"
#include "oi_tcs_msg.h"
#include "oi_tcs_cc.h"
#include "oi_tcs_helper.h"
#include "oi_tcs_coding.h"
#include "oi_l2cap.h"
#include "oi_devmgr.h"
#include "oi_hcispec.h"
#include "oi_argcheck.h"
#include "oi_dispatch.h"
#include "oi_l2cap_group.h"
#include "oi_connect_policy.h"
#include "oi_securitydb.h"

#include "oi_bt_assigned_nos.h"
#include "oi_std_utils.h"
#include "oi_assert.h"

#include "oi_bt_profile_config.h"
#include "oi_init_flags.h"
#include "oi_config_table.h"

#define CHECK_INIT OI_CHECK_INIT(TCS)

/* Constants for use with Allocate Call */
#define INCOMING 0
#define OUTGOING 1

/** The TCS profile private data */
OI_TCS_DATA *OI_Tcs_Data = NULL;

/* Forward declarations */

static void connectInd(OI_L2CAP_PSM psm,
                       OI_BD_ADDR *addr,
                       OI_L2CAP_CID cid);

static TCS_CALL_INFO *GetCallBySCOBearer(OI_HCI_CONNECTION_HANDLE handle);

static void ConnectBearerComplete(TCS_CALL_INFO *call, OI_STATUS status);
static OI_STATUS ContinueDisconnect(TCS_CALL_INFO *call);

#ifdef OI_DEBUG
#error code not present
#endif

/* SCO callbacks */

static void SCOConnectCompleteCB(OI_STATUS status,
                                 OI_HCI_CONNECTION_HANDLE handle,
                                 OI_BD_ADDR *addr)
{
    TCS_CALL_INFO *call;

    OI_DBGPRINT(("SCO bearer connection complete (%:) - %d\n", addr, handle));

    call = OI_Tcs_GetCall(addr);
    if (!call) {
        OI_LOG_ERROR(("No call found for completed SCO connection\n"));
        goto Reject;
    } 
    if (TCS_STATE_NULL == call->state) {
        OI_DBGPRINT(("Rejecting completed SCO connection, call state is NULL\n"));
        goto Reject;
    }
    if (OI_TCS_BEARER_LINK_TYPE_SCO != call->bearer.type) {
        OI_DBGPRINT(("Rejecting completed SCO connection, not selected bearer type\n"));
        goto Reject;
    }
    
    if (OI_SUCCESS(status)) {
        call->handle.scoHandle = handle;
    } else {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("SCO bearer connection failed"));
#endif
        call->handle.scoHandle = 0;
    }
    ConnectBearerComplete(call, status);
    return;

Reject:
    OI_DEVMGR_ScoDisconnect(handle);
}

static void SCODisconnectCompleteCB(OI_STATUS status,
                                    OI_HCI_CONNECTION_HANDLE connectionHandle,
                                    OI_STATUS reason)
{
    TCS_CALL_INFO *call;

    OI_DBGPRINT(("Disconnected SCO bearer connection - %d\n", connectionHandle));
    
    call = GetCallBySCOBearer(connectionHandle);
    if (!call) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("No call found for handle"));
#endif
    } else {
        call->handle.scoHandle = 0;
        call->bearerState = CH_ST_DISCONNECTED;

        if ((TCS_ST_OUTGOING_DISCONNECT == call->tcsState) ||
            (TCS_ST_INCOMING_DISCONNECT == call->tcsState)) {
            ContinueDisconnect(call);
        } else {
            if (OI_TCS_CAUSE_NONE == call->cause) {
                call->cause = OI_TCS_CAUSE_BEARER_CAPABILITY_UNAVAILABLE;
            }
            OI_TCS_ClearCall(&call->addr, call->cause);
        }
    }
}

static OI_BOOL SCOConnectIndCB(OI_BD_ADDR *pBdAddr,
                               OI_UINT32 deviceClass)
{
    TCS_CALL_INFO *call;

    OI_DBGPRINT2(("Incoming SCO bearer connection (%:)", pBdAddr));

    call = OI_Tcs_GetCall(pBdAddr);
    if (!call) {
        OI_LOG_ERROR(("No call found for attempted SCO connection\n"));
        return FALSE;
    }
    if (TCS_STATE_NULL == call->state) {
        OI_DBGPRINT(("Rejecting incoming SCO connection, call state is NULL\n"));
        return FALSE;
    }
    if (OI_TCS_BEARER_LINK_TYPE_SCO != call->bearer.type) {
        OI_DBGPRINT(("Rejecting incoming SCO connection, not selected bearer type\n"));
        return FALSE;
    }
    
    if (CH_ST_DISCONNECTED == call->bearerState) {
        call->bearerState = CH_ST_CONNECT_IN_PROGRESS;
        return TRUE;
    } else {
        OI_DBGPRINT(("Rejecting attempted SCO connection, bearer connection already exists or in progress\n"));
        return FALSE;
    }
}


static OI_DEVMGR_SCO_SIGNAL_CALLBACKS scoCallbacks = {
    SCOConnectCompleteCB,
    SCODisconnectCompleteCB,
    SCOConnectIndCB
};


/** The handler for the setup timeout. Terminate the L2CAP channel as a result. */
static void SetupTimeoutHandler(DISPATCH_ARG *arg)
{
    OI_DBGPRINT2(("Intercom setup timed out!\n"));
    OI_Tcs_InternalDisconnect(&OI_Tcs_Data->calls[0], OI_OK);
}

/**
 * This function allocates a call data structure from the list of calls.
 * A zero CID indicates that the call is not in use.
 *
 * @param   addr        the address of the remote device assocaited with the call
 * @param   callType    the incoming
 *
 * @return  NULL if no connection could be allocated
 */
static TCS_CALL_INFO *AllocateCall(OI_BD_ADDR *addr,
                                   OI_UINT callType)
{
    OI_INT i;
    TCS_CALL_INFO *call = NULL;

    if (OI_IS_ZERO_ADDR(addr)) {
        return NULL;
    }

    for (i = 0; i < OI_Tcs_Data->numCalls; i++) {
        /* Disallow duplicate connections */
        if (SAME_BD_ADDR(&OI_Tcs_Data->calls[i].addr, addr)) {
            OI_DBGPRINT(("Connection already exists, AllocateCall failing\n"));
            return NULL;
        }
        if (!call && IsCallFree(&OI_Tcs_Data->calls[i])) {
            call = &OI_Tcs_Data->calls[i];
        }
    }
    /* Initialize */
    if (call) {
        OI_ASSERT((INCOMING == callType) || (OUTGOING == callType));
        if (OUTGOING == callType) {
            call->outgoing = TRUE;
        } else {
            call->outgoing = FALSE;
        }
        call->intercom = FALSE;
        call->timerType = TCS_TNULL;
        call->cid = 0;
        call->addr = *addr;
        call->state = TCS_STATE_NULL;

        call->bearer.info.acl.flowspec = NULL;
        call->callingNumber.digits = NULL;
        call->calledNumber.digits = NULL;

        call->tcsState = TCS_ST_DISCONNECTED;
        call->connectInProgress = FALSE;
        call->connectResult = OI_STATUS_NOT_CONNECTED;
        call->controlState = CH_ST_DISCONNECTED;
        call->bearerState = CH_ST_DISCONNECTED;
        call->clState = CH_ST_DISCONNECTED;
    }
    return call;
}

TCS_CALL_INFO *OI_Tcs_GetCall(OI_BD_ADDR *addr)
{
    OI_INT i;

    if (OI_IS_ZERO_ADDR(addr)) {
        return NULL;
    }

    for (i = 0; i < OI_Tcs_Data->numCalls; i++) {
        if (SAME_BD_ADDR(&OI_Tcs_Data->calls[i].addr, addr)) {
            return &OI_Tcs_Data->calls[i];
        }
    }
    return NULL;
}

TCS_CALL_INFO *OI_Tcs_GetCallByCID(OI_L2CAP_CID cid)
{
    OI_INT i;

    if (0 == cid) {
        return NULL;
    }

    for (i = 0; i < OI_Tcs_Data->numCalls; i++) {
        if (OI_Tcs_Data->calls[i].cid == cid) { 
            /* CID will be zero if not in use, so don't bother checking if the block is free */
            return &OI_Tcs_Data->calls[i];
        }
    }
    return NULL;
}

static TCS_CALL_INFO *GetCallByACLBearer(OI_L2CAP_CID cid)
{
    OI_INT i;

    if (0 == cid) {
        return NULL;
    }

    for (i = 0; i < OI_Tcs_Data->numCalls; i++) {
        if ((OI_Tcs_Data->calls[i].bearer.type == OI_TCS_BEARER_LINK_TYPE_ACL) && (OI_Tcs_Data->calls[i].handle.cid == cid)) {
            /* CID will be zero if not in use, so don't bother checking if the block is free */
            return &OI_Tcs_Data->calls[i];
        }
    }
    return NULL;
}

static TCS_CALL_INFO *GetCallBySCOBearer(OI_HCI_CONNECTION_HANDLE handle)
{
    OI_INT i;

    if (0 == handle) {
        return NULL;
    }

    for (i = 0; i < OI_Tcs_Data->numCalls; i++) {
        if ((OI_Tcs_Data->calls[i].bearer.type == OI_TCS_BEARER_LINK_TYPE_SCO) && 
            (OI_Tcs_Data->calls[i].handle.scoHandle == handle)) {
            /* SCO handle will be zero if not in use, so don't bother checking if the block is free */
            return &OI_Tcs_Data->calls[i];
        }
    }
    return NULL;
}

void OI_Tcs_FreeCall(TCS_CALL_INFO *call)
{
    OI_DBGPRINT2(("Free call\n"));

    OI_ASSERT(TCS_STATE_NULL == call->state);
    OI_ASSERT(CH_ST_DISCONNECTED == call->controlState)

    INIT_BD_ADDR(&call->addr);
    call->cid = 0;
    call->intercom = FALSE;
}

/** Get the psm for the active call. */
static OI_L2CAP_PSM OI_TCS_ActivePSM(TCS_CALL_INFO *call)
{
    if (call->intercom) {
        return OI_PSM_TCS;
    } else {
        return OI_PSM_TCS_CORDLESS;
    }
}

/*******************************************************

  L2CAP Bearer Callbacks

*******************************************************/

static void BearerConnectCB(OI_L2CAP_CID cid,
                            OI_UINT16 inMTU,
                            OI_UINT16 outMTU,
                            OI_STATUS result)
{
    TCS_CALL_INFO *call;

    OI_DBGPRINT(("L2CAP bearer connection complete - %d\n", cid));

    call = GetCallByACLBearer(cid);
    if (!call) {
        OI_LOG_ERROR(("No call found for completed L2CAP connection\n"));
        goto Reject;
    }
    if (TCS_STATE_NULL == call->state) {
        OI_DBGPRINT(("Rejecting completed bearer connection, call state is NULL\n"));
        goto Reject;
    } else if (OI_TCS_BEARER_LINK_TYPE_ACL != call->bearer.type) {
        OI_DBGPRINT(("Rejecting completed bearer connection, incorrect type\n"));
        goto Reject;
    }

    if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("L2CAP bearer connection failed"));
#endif
    } else {
        call->handle.cid = cid;
    }
    ConnectBearerComplete(call, result);
    return;

Reject:
    OI_L2CAP_Disconnect(cid);
}

static void BearerDisconnectInd(OI_L2CAP_CID cid,
                                OI_STATUS reason)
{
    TCS_CALL_INFO *call;

    OI_DBGPRINT(("Disconnected L2CAP bearer connection - %d\n", cid));

    call = GetCallByACLBearer(cid);
    if (!call) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("No call found for channel"));
#endif
    } else {
        call->handle.cid = 0;
        call->bearerState = CH_ST_DISCONNECTED;

        if ((TCS_ST_OUTGOING_DISCONNECT == call->tcsState) ||
            (TCS_ST_INCOMING_DISCONNECT == call->tcsState)) {
            ContinueDisconnect(call);
        } else {
            OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_BEARER_CAPABILITY_UNAVAILABLE);
        }
    }
}

static void BearerRecvDataInd(OI_L2CAP_CID cid,
                              OI_UINT8 *buf,
                              OI_UINT16 length)
{
    TCS_CALL_INFO *call;

    call = GetCallByACLBearer(cid);
    if (!call) {
        OI_LOG_ERROR(("Bearer data received for nonexistent channel\n"));
        return;
    }

    OI_Tcs_Data->callbacks.callDataInd(&call->addr, buf, length);
}


/*******************************************************

  L2CAP Callbacks

*******************************************************/

static void CompleteConnect(TCS_CALL_INFO *call, OI_STATUS result)
{
    OI_BD_ADDR addr = call->addr;
    call->connectInProgress = FALSE;
    if (OI_SUCCESS(result)) {
        SetTcsState(call, TCS_ST_CONNECTED);
    } else {
        SetTcsState(call, TCS_ST_DISCONNECTED);
        OI_Tcs_FreeCall(call);
    }
    OI_Tcs_Data->callbacks.connectCfm(&addr, result);
}

static void AddMemberCB(OI_L2CAP_CID cid,
                         OI_BD_ADDR *addr,
                         OI_STATUS result)
{
    TCS_CALL_INFO *call;

    call = OI_Tcs_GetCall(addr);
    if (call) {
        if (OI_SUCCESS(result)) {
            OI_DBGPRINT(("Added %: to connectionless group\n", addr));
            call->clState = CH_ST_CONNECTED;
            CompleteConnect(call, result);
        } else {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Failed to add member to group"));
#endif
            OI_Tcs_InternalDisconnect(call, result); 
        }
    } else {
        OI_LOG_ERROR(("Add member callback, but no call exists\n"));
        OI_L2CAP_GroupRemoveMember(cid, addr);
    }
}

static void removeCB(OI_BD_ADDR *addr)
{
    TCS_CALL_INFO *call;

    OI_DBGPRINT(("%: removed from connectionless group\n", addr));
    call = OI_Tcs_GetCall(addr);
    if (call) {
        call->clState = CH_ST_DISCONNECTED;

        if (TCS_ST_CONNECTED == call->tcsState) {
            /*
             * Assume that if the other side removes itself from the group that it
             * is disconnecting.  In either case, we can't have a control connection 
             * and no group connection, so disconnect.
             */
            SetTcsState(call, TCS_ST_INCOMING_DISCONNECT);
        }
        ContinueDisconnect(call);
    }
}

static void connectCompleteCB(OI_L2CAP_CID cid,
                              OI_UINT16 inMTU,
                              OI_UINT16 outMTU,
                              OI_STATUS result)
{
    TCS_CALL_INFO *call;

    OI_DBGPRINT(("L2CAP control connection complete - %d\n", cid));

    call = OI_Tcs_GetCallByCID(cid);
    if (call) {
        if (OI_SUCCESS(result)) {
            call->controlState = CH_ST_CONNECTED;
            call->outMTU = outMTU;

            result = OI_DEVMGR_RegisterScoApplication(&scoCallbacks, &call->addr);
            if (!OI_SUCCESS(result)) {
                OI_Tcs_InternalDisconnect(call, result);
                return;
            }

            if (call->intercom && !call->outgoing) {
                OI_Dispatch_RegisterTimedFunc(SetupTimeoutHandler, NULL, INTERCOM_SETUP_TIMEOUT, &call->intercomSetupTimer);
            }

            if (OI_TCS_ROLE_TERMINAL == OI_Tcs_Data->role) {
                CompleteConnect(call, result);
            } else {
                result = OI_L2CAP_GroupAddMember(AddMemberCB, OI_Tcs_Data->groupCID, &call->addr);
                if (OI_SUCCESS(result)) {
                    SetTcsState(call, TCS_ST_ADD_TO_GROUP);
                } else {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("Failed to add member to group"));
#endif
                    OI_Tcs_InternalDisconnect(call, result);
                }
            }
        } else {
            OI_DBGPRINT2(("L2CAP control connection failed - %!\n", result));            
            CompleteConnect(call, result);
        }
    } else {
        OI_LOG_ERROR(("No existing call for connnection\n"));
        OI_L2CAP_Disconnect(cid);
    }
}

static void disconnectInd(OI_L2CAP_CID cid,
                          OI_STATUS reason)
{
    TCS_CALL_INFO *call;

    OI_DBGPRINT(("Disconnected L2CAP control connection - %d\n", cid));
    call = OI_Tcs_GetCallByCID(cid);
    if (call) {
        call->controlState = CH_ST_DISCONNECTED;
        OI_Tcs_CancelTimer(call);

        if (TCS_ST_ADD_TO_GROUP == call->tcsState ||
            TCS_ST_CONNECTED == call->tcsState) {
            SetTcsState(call, TCS_ST_INCOMING_DISCONNECT);
        }
        ContinueDisconnect(call);
    } else {
        OI_DBGPRINT2(("No waiting calls on disconnected cid %d\n", cid));
    }
}

/*
 * Connection oriented channel data received indicator.
 */
static void recvDataInd(OI_L2CAP_CID cid,
                        OI_UINT8 *buf,
                        OI_UINT16 length)
{
    TCS_CALL_INFO *call;
    
    call = OI_Tcs_GetCallByCID(cid);
    if (call) {
        OI_TCS_Recv_Msg(call, buf, length);
    } else {
        /* Connection exists, but no call established or in process */
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Received data from channel, but no call exists"));
#endif
    }
}

/*
 * Connectionless channel data received indicator.  Only 
 * terminals listen to the connectionless channel.
 */
static void clRecvInd(OI_L2CAP_PSM psm,
                      OI_BYTE *data,
                      OI_UINT16 dataLen)
{
    OI_ASSERT(psm == OI_PSM_TCS_CORDLESS);

    /* There is only one call for a TL, so use that one. */
    OI_TCS_Recv_Msg(&OI_Tcs_Data->calls[0], data, dataLen);
}

/*
 * L2CAP control channel connection request indicator
 */
static void connectInd(OI_L2CAP_PSM psm,
                       OI_BD_ADDR *addr,
                       OI_L2CAP_CID cid)
{
    OI_STATUS result;
    TCS_CALL_INFO *call;

    call = OI_Tcs_GetCall(addr);
    if (call && (TCS_ST_CONNECTED == call->tcsState)) {
        /* A call to the address already exists, so assume this is the bearer channel. */
        OI_BOOL accept;

        OI_DBGPRINT2(("Incoming L2CAP bearer connection - %d\n", cid));

        if (TCS_STATE_NULL == call->state) {
            OI_DBGPRINT(("Rejecting incoming bearer connection, call state is NULL\n"));
            OI_L2CAP_Accept(connectCompleteCB, disconnectInd, recvDataInd, cid, NULL, FALSE,
                            &OI_Tcs_Data->tcsConnectPolicy);
        } else if (OI_TCS_BEARER_LINK_TYPE_ACL != call->bearer.type) {
            OI_DBGPRINT(("Rejecting incoming bearer connection, incorrect type\n"));
            OI_L2CAP_Accept(connectCompleteCB, disconnectInd, recvDataInd, cid, NULL, FALSE,
                            &OI_Tcs_Data->tcsConnectPolicy);
        } else if (CH_ST_DISCONNECTED == call->bearerState) {
            accept = TRUE;
            call->handle.cid = cid;

            result = OI_L2CAP_Accept(BearerConnectCB, BearerDisconnectInd, BearerRecvDataInd,
                                     cid, NULL, accept, &OI_Tcs_Data->tcsConnectPolicy);
            if (OI_SUCCESS(result)) {
                call->bearerState = CH_ST_CONNECT_IN_PROGRESS;
            } else {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("Bearer establishment failed, clearing call"));
#endif
                OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_CALL_REJECTED);
            }
        } else {
            OI_DBGPRINT2(("Rejecting connection request from %:, connection already exists or in progress", addr));
            OI_L2CAP_Accept(connectCompleteCB, disconnectInd, recvDataInd, cid, NULL, FALSE,
                            &OI_Tcs_Data->tcsConnectPolicy);
        }
    } else {
        OI_DBGPRINT2(("Incoming L2CAP control connection - %d\n", cid));

        call = AllocateCall(addr, INCOMING);
        if (call) {
            call->cid = cid;
            if (OI_PSM_TCS == psm) {
                call->intercom = TRUE;
            }
            SetTcsState(call, TCS_ST_INCOMING_CONNECT);
            OI_Tcs_Data->callbacks.connectInd(addr);
        } else {
            /*
             * The connection already exists, or there are no more free calls
             * to allocation, so reject the request.
             */
            OI_DBGPRINT2(("Rejecting connection request from %:", addr));
            OI_L2CAP_Accept(connectCompleteCB, disconnectInd, recvDataInd, cid, NULL, FALSE,
                            &OI_Tcs_Data->tcsConnectPolicy);
        }
    }
}

/*******************************************************

  Initialization functions

*******************************************************/

/**
 * This function initializes the TCS module.
 *
 * @param role          OI_TCS_ROLE_TERMINAL or OI_TCS_ROLE_GATEWAY
 * @param intercomOnly  TRUE if a terminal should only support the Intercom Profile.  Valid only if role is OI_TCS_ROLE_TERMINAL.
 * @param cb            the callbacks to use for TCS
 */
OI_STATUS OI_TCS_Register(OI_UINT8 role,
                          OI_BOOL intercomOnly,
                          const OI_TCS_CALLBACKS *cb)
{
    OI_STATUS result;
    OI_BOOL tcsReg = FALSE;
    OI_BOOL cordlessReg = FALSE;
    OI_BOOL groupReg = FALSE;

    ARGCHECK(cb);
    ARGCHECK(cb->connectInd);
    ARGCHECK(cb->connectCfm);
    ARGCHECK(cb->disconnectCfm);
    ARGCHECK(cb->establishCfm);
    ARGCHECK(cb->establishInd);
    ARGCHECK(cb->infoInd);
    ARGCHECK(cb->releaseInd);
    ARGCHECK(cb->outgoingStatusCB);
    ARGCHECK(cb->callDataInd);
    if (cb->startDTMF || cb->stopDTMF) {
        ARGCHECK(cb->startDTMF);
        ARGCHECK(cb->stopDTMF);
    }
    if (cb->startDTMFAck || cb->startDTMFReject || cb->stopDTMFAck) {
        ARGCHECK(cb->startDTMFAck);
        ARGCHECK(cb->startDTMFReject);
        ARGCHECK(cb->stopDTMFAck);
    }

    /* Make sure initialization only happens once - currently, the init flag is boolean */
    if (OI_INIT_FLAG_VALUE(TCS)) {
        return OI_STATUS_ALREADY_INITIALIZED;
    }

    OI_Tcs_Data = OI_Malloc(sizeof(*OI_Tcs_Data));
    if (NULL == OI_Tcs_Data) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    /* Initialize data structure */
    OI_Tcs_Data->callbacks = *cb;
    OI_Tcs_Data->queue = NULL;
    OI_Tcs_Data->supportCtp = !intercomOnly;
    OI_Tcs_Data->role = role;
    if (OI_TCS_ROLE_TERMINAL == role) {
        OI_Tcs_Data->numCalls = 1;
    } else {
        OI_Tcs_Data->numCalls = GATEWAY_MAX_CALLS;
    }
    OI_Tcs_Data->calls = OI_Calloc(sizeof(TCS_CALL_INFO) * OI_Tcs_Data->numCalls);
    if (OI_Tcs_Data->calls == NULL) {
        result = OI_STATUS_OUT_OF_MEMORY;
        goto initDone;
    }

    /* Set connection policy.  The gateway must be master */
    if (OI_Tcs_Data->supportCtp) {
        OI_Tcs_Data->tcsConnectPolicy.serviceUuid32 = OI_UUID_CordlessTelephony;
    } else {
        OI_Tcs_Data->tcsConnectPolicy.serviceUuid32 = OI_UUID_Intercom;
    }
    OI_Tcs_Data->tcsConnectPolicy.flowspec = NULL;
    OI_Tcs_Data->tcsConnectPolicy.powerSavingDisables = 0;
    OI_Tcs_Data->tcsConnectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(OI_Tcs_Data->tcsConnectPolicy.serviceUuid32);
    OI_Tcs_Data->tcsConnectPolicy.mustBeMaster = (OI_TCS_ROLE_GATEWAY == role) ? TRUE : FALSE;

    /* 
     * Acquire L2CAP resources.  These depend on the role: Intercom requires TCS,
     * CTP TL requires TCS and CORDLESS for the connectionless channel, and the CTP GW 
     * requires CORDLESS for the connection and connectionless channels.
     */
    if (OI_Tcs_Data->supportCtp) {
#ifdef OI_CODE
#error code not present
#else
        result = OI_L2CAP_Register(OI_PSM_TCS_CORDLESS, connectInd, NULL);
#endif
        if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Failed to register cordless PSM"));
#endif
            goto initDone;
        }
        cordlessReg = TRUE;
    }
    if (OI_TCS_ROLE_TERMINAL == role) {
#ifdef OI_CODE
#error code not present
#else
        result = OI_L2CAP_Register(OI_PSM_TCS, connectInd, NULL);
#endif
        if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Failed to register intercom PSM"));
#endif
            goto initDone;
        }
        tcsReg = TRUE;

        /* Connectionless messages are only valid for CTP */
        if (OI_Tcs_Data->supportCtp) {
            result = OI_L2CAP_EnableCLT(OI_PSM_TCS_CORDLESS, clRecvInd);
            if (OI_SUCCESS(result)) {
                OI_Tcs_Data->calls[0].clState = CH_ST_CONNECTED;
            } else {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("Failed to register connectionless reception callback"));
#endif
                goto initDone;
            }
            cordlessReg = TRUE;
        }
    } else if (OI_TCS_ROLE_GATEWAY == role) {
        /* GW only exists under CTP */
        if (!OI_Tcs_Data->supportCtp) {
            OI_DBGPRINT(("Attempted to use intercom with gateway - rejected\n"));
            result = OI_STATUS_INVALID_PARAMETERS;
            goto initDone;
        }

        result = OI_L2CAP_GroupCreate(OI_PSM_TCS_CORDLESS, &OI_Tcs_Data->groupCID, removeCB);
        if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Failed to create L2CAP group"));
#endif
            goto initDone;
        }
        groupReg = TRUE;
    } else {
        result = OI_STATUS_INVALID_ROLE;
        goto initDone;
    }

initDone:

    if (OI_SUCCESS(result)) {
        OI_INIT_FLAG_PUT_FLAG(TRUE, TCS);
    } else {
        if (tcsReg) {
            OI_L2CAP_Deregister(OI_PSM_TCS);
        }
        if (cordlessReg) {
            OI_L2CAP_DisableCLT(OI_PSM_TCS_CORDLESS);
            OI_Tcs_Data->calls[0].clState = CH_ST_DISCONNECTED;
            OI_L2CAP_Deregister(OI_PSM_TCS_CORDLESS);
        }
        if (groupReg) {
            OI_L2CAP_GroupClose(OI_Tcs_Data->groupCID);
        }
        if (OI_Tcs_Data->calls) {
            OI_FreeIf(&OI_Tcs_Data->calls);
        }
        OI_FreeIf(&OI_Tcs_Data);
    }

    return result;
}

OI_STATUS OI_TCS_Deregister(void)
{
    OI_STATUS result;

    CHECK_INIT;

    /*
     * Release L2CAP resources
     */
    if (OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) {
        result = OI_L2CAP_GroupClose(OI_Tcs_Data->groupCID);
        if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("L2CAP group close failed"));
#endif
            return result;
        }
    } else if (OI_TCS_ROLE_TERMINAL == OI_Tcs_Data->role) {
        if (OI_Tcs_Data->supportCtp) {
            result = OI_L2CAP_DisableCLT(OI_PSM_TCS_CORDLESS);
            if (OI_SUCCESS(result)) {
                OI_Tcs_Data->calls[0].clState = CH_ST_DISCONNECTED;
            } else {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("L2CAP deregister connectionless channel failed"));
#endif
                return result;
            }
        }
        result = OI_L2CAP_Deregister(OI_PSM_TCS);
        if (!OI_SUCCESS(result)) {
            OI_DBGPRINT(("L2CAP deregister intercom PSM failed - %!\n", result));
            return result;
        }
    }
    if (OI_Tcs_Data->supportCtp) {
        result = OI_L2CAP_Deregister(OI_PSM_TCS_CORDLESS);
        if (!OI_SUCCESS(result)) {
            OI_DBGPRINT(("L2CAP deregister cordless PSM failed - %!\n", result));
            return result;
        }
    }

    OI_FreeIf(&OI_Tcs_Data->calls);
    OI_FreeIf(&OI_Tcs_Data);

    OI_INIT_FLAG_PUT_FLAG(FALSE, TCS);
    return OI_OK;
}

/*******************************************************

  Internal functions

*******************************************************/

/**
 * Create the designated bearer channel.
 */
OI_STATUS OI_Tcs_ConnectBearer(TCS_CALL_INFO *call)
{
    OI_STATUS result;

    OI_DBGPRINT2(("Establishing bearer\n"));

    OI_ASSERT((OI_TCS_BEARER_LINK_TYPE_SCO == call->bearer.type)
              || (OI_TCS_BEARER_LINK_TYPE_ACL == call->bearer.type)
              || (OI_TCS_BEARER_LINK_TYPE_NONE == call->bearer.type));

    if (CH_ST_DISCONNECTED != call->bearerState) {
        OI_DBGPRINT2(("Connect bearer requested, but bearer already connected"));
        return OI_STATUS_ALREADY_CONNECTED;
    }

    if (OI_TCS_BEARER_LINK_TYPE_SCO == call->bearer.type) {
        result = OI_DEVMGR_ScoCreateConnection(&call->addr);
        if (OI_SUCCESS(result)) {
            call->bearerState = CH_ST_CONNECT_IN_PROGRESS;
        }
    } else if (OI_TCS_BEARER_LINK_TYPE_ACL == call->bearer.type) {
        /*
         * L2CAP does not support QoS at this time, so we can't pass the flow spec down
         */
        result = OI_L2CAP_Connect(BearerConnectCB, BearerDisconnectInd, BearerRecvDataInd,
                                  OI_TCS_ActivePSM(call), &call->addr, NULL, &call->handle.cid,
                                  &OI_Tcs_Data->tcsConnectPolicy);
        if (OI_SUCCESS(result)) {
            call->bearerState = CH_ST_CONNECT_IN_PROGRESS;
        }
    } else {
        /* OI_TCS_BEARER_LINK_TYPE_NONE */
        result = OI_OK;
    }

    if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Bearer establishment failed, clearing call"));
#endif
        OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_CALL_REJECTED);
    }
    return result;
}

/**
 * Disconnect an established bearer channel.
 */
OI_STATUS OI_Tcs_DisconnectBearer(TCS_CALL_INFO *call)
{
    OI_STATUS result = OI_STATUS_NOT_CONNECTED;

    if (OI_TCS_BEARER_LINK_TYPE_NONE == call->bearer.type) {
        return OI_OK;
    }

    if (CH_ST_DISCONNECTED == call->bearerState || CH_ST_DISCONNECT_IN_PROGRESS == call->bearerState) {
        OI_LOG_ERROR(("Disconnect bearer called while not connected\n"));
        return OI_STATUS_NOT_CONNECTED;
    }
    if (CH_ST_CONNECT_IN_PROGRESS == call->bearerState) {
        call->bearerState = CH_ST_DISCONNECTED;
        return OI_OK;
    }

    if (OI_TCS_BEARER_LINK_TYPE_SCO == call->bearer.type) {
        OI_DBGPRINT2(("Disconnecting SCO bearer channel\n"));
        result = OI_DEVMGR_ScoDisconnect(call->handle.scoHandle);
        if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Failed to disconnect SCO bearer channel"));
#endif
        }
    } else if (OI_TCS_BEARER_LINK_TYPE_ACL == call->bearer.type) {
        OI_DBGPRINT2(("Disconnecting ACL bearer channel\n"));
        result = OI_L2CAP_Disconnect(call->handle.cid);
        if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Failed to disconnect ACL bearer channel"));
#endif
        }
    }

    if (OI_SUCCESS(result)) {
        call->bearerState = CH_ST_DISCONNECT_IN_PROGRESS;
    }
    return result;
}

/*
 * The application may have requested a new bearer type from the incoming
 * one during bearer selection.  call->bearer must be updated to reflect 
 * the application view of the call.  
 * This also enables in-band tones if the bearer type is SCO.
 */
static OI_STATUS SelectBearer(TCS_CALL_INFO *call, OI_TCS_BEARER *bearer)
{
    OI_STATUS result;

    if (TCS_STATE_CALL_PRESENT == call->state) {
        /*
         * Release the memory used by the flowspec if it's no longer needed.
         */
        if ((OI_TCS_BEARER_LINK_TYPE_ACL == call->bearer.type) &&
            (OI_TCS_BEARER_LINK_TYPE_ACL != bearer->type)) {
            OI_FreeIf(&call->bearer.info.acl.flowspec);
        }

        if (bearer) {
            if (OI_TCS_BEARER_LINK_TYPE_ACL == bearer->type) {
                if (bearer->info.acl.flowspec) {
                    if ((OI_TCS_BEARER_LINK_TYPE_ACL == call->bearer.type) && call->bearer.info.acl.flowspec) {
                        /*
                         * We already have an existing valid flowspec, so no need to malloc space for the
                         * new one.
                         */
                    } else {
                        call->bearer.info.acl.flowspec = OI_Malloc(sizeof(OI_L2CAP_FLOWSPEC));
                        if (!call->bearer.info.acl.flowspec) {
                            OI_LOG_ERROR(("Out of memory!\n"));
                            return OI_STATUS_OUT_OF_MEMORY;
                        }
                    }
                    OI_MemCopy(call->bearer.info.acl.flowspec, bearer->info.acl.flowspec, sizeof(OI_L2CAP_FLOWSPEC));
                } else if (OI_TCS_BEARER_LINK_TYPE_ACL == call->bearer.type) {
                    OI_FreeIf(&call->bearer.info.acl.flowspec);
                } else {
                    call->bearer.info.acl.flowspec = NULL;
                }
                call->bearer.type = OI_TCS_BEARER_LINK_TYPE_ACL;
                call->bearer.info.acl.layer2 = bearer->info.acl.layer2;
                call->bearer.info.acl.layer3 = bearer->info.acl.layer3;
            } else {
                OI_MemCopy(&call->bearer, bearer, sizeof(OI_TCS_BEARER));
            }
        } else {
            /* Setup default call parameters - defined by TCS spec F:3 7.4.3 */
            call->bearer.type = OI_TCS_BEARER_LINK_TYPE_SCO;
            call->bearer.info.sco.packetType = OI_TCS_BEARER_PACKET_TYPE_HV3;
            call->bearer.info.sco.coding = OI_TCS_BEARER_CODING_CVSD;
        }

        /*
         * In-band tones are supported by default in this TCS implementation.  Per the spec,
         * only the CTP GW supports them.
         */
        if ((OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) &&
            (OI_TCS_BEARER_LINK_TYPE_SCO == call->bearer.type) &&
            (CH_ST_DISCONNECTED == call->bearerState)) {

            result = OI_Tcs_ConnectBearer(call);
            if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("Connect bearer for in-band tones failed"));
#endif
            } else {
                result = OI_Tcs_Send_Progress(call, OI_TCS_PROGRESS_INFO_AVAILABLE, 0, 0);
                if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("Send PROGRESS for in-band tones failed"));
#endif
                }
            }
        }
    }
    return OI_OK;
}

static void ConnectBearerComplete(TCS_CALL_INFO *call, OI_STATUS status)
{
    OI_STATUS result;

    if (OI_SUCCESS(status)) {
        OI_DBGPRINT2(("Bearer connection completed\n"));
        call->bearerState = CH_ST_CONNECTED;

        if (TCS_STATE_CONNECT_RECEIVED == call->state) {
            result = OI_Tcs_Send_ConnectAck(call, NULL, 0);
            if (OI_SUCCESS(result)) {
                SetCallState(call, TCS_STATE_ACTIVE);
                if (OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) {
                    OI_Tcs_ClearAllNonSelected(call);
                }
                OI_Tcs_Data->callbacks.establishCfm(&call->addr, OI_OK);
            } else {
                OI_LOG_ERROR(("Send CONNECT ACK failed in bearer connect complete - %!\n"));
            }
        }
    } else {
        call->bearerState = CH_ST_DISCONNECTED;
        /*
         * If we're still connected try to clear the call, otherwise a disconnect has
         * begun and we need to continue that.
         */
        if (CH_ST_CONNECTED == call->controlState) {
            OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_BEARER_CAPABILITY_UNAVAILABLE);
        } else {
            ContinueDisconnect(call);
        }
    }
}

OI_STATUS OI_Tcs_SetupCallInfo(TCS_CALL_INFO *call, 
                               OI_TCS_CONNECT_INFO *info)
{
    OI_STATUS status;

    if (info->bearer) {
        OI_MemCopy(&call->bearer, info->bearer, sizeof(OI_TCS_BEARER));
        if (OI_TCS_BEARER_LINK_TYPE_ACL == info->bearer->type) {
            if (info->bearer->info.acl.flowspec) {
                call->bearer.info.acl.flowspec = OI_Malloc(sizeof(OI_L2CAP_FLOWSPEC));
                if (!call->bearer.info.acl.flowspec) {
                    OI_LOG_ERROR(("Out of memory!\n"));
                    status = OI_STATUS_OUT_OF_MEMORY;
                    goto Error;
                }
                OI_MemCopy(call->bearer.info.acl.flowspec, info->bearer->info.acl.flowspec, sizeof(OI_L2CAP_FLOWSPEC));
            }
        }
    } else {
        /* Setup default call parameters - defined by TCS spec F:3 7.4.3 */
        call->bearer.type = OI_TCS_BEARER_LINK_TYPE_SCO;
        call->bearer.info.sco.packetType = OI_TCS_BEARER_PACKET_TYPE_HV3;
        call->bearer.info.sco.coding = OI_TCS_BEARER_CODING_CVSD;
    }
    call->callClass = info->callClass;
    call->sendingComplete = info->sendingComplete;
    call->signal = info->signal;
    if (info->callingNumber) {
        status = OI_Tcs_CopyNumber(&call->callingNumber, info->callingNumber);
        if (!OI_SUCCESS(status)) {
            goto Error;
        }
    }
    if (info->calledNumber) {
        status = OI_Tcs_CopyNumber(&call->calledNumber, info->calledNumber);
        if (!OI_SUCCESS(status)) {
            goto Error;
        }
    }
    call->cause = OI_TCS_CAUSE_NONE;
    call->handle.scoHandle = 0;
    return OI_OK;

Error:
    OI_Tcs_ReleaseCallInfo(call);
    return status;
}

OI_STATUS OI_Tcs_ReleaseCallInfo(TCS_CALL_INFO *call)
{
    if (OI_TCS_BEARER_LINK_TYPE_ACL == call->bearer.type) {
        OI_FreeIf(&call->bearer.info.acl.flowspec);
    }
    OI_FreeIf(&call->callingNumber.digits);
    OI_FreeIf(&call->calledNumber.digits);    
    return OI_OK;
}

/*
 * This is a convenience function used only by OI_TCS_EstablishCall.  After
 * completion, the call information will be initialized with the connect info
 * passed in from the user and the state will be CALL INITIATED.
 *
 * @param call  The call to setup.  May be NULL if point to multipoint call
 */
static void SetupCall(TCS_CALL_INFO *call,
                      OI_TCS_CONNECT_INFO *info)
{
    OI_BOOL broadcast = !call;
    OI_UINT i;
    OI_STATUS result;

    OI_ASSERT(info);

    if (broadcast) {
        for (i = 0; i < OI_Tcs_Data->numCalls; ++i) {
            call = &OI_Tcs_Data->calls[i];
            if (!IsCallFree(call)) {
                result = OI_Tcs_SetupCallInfo(call, info);
                if (!OI_SUCCESS(result)) {
                    OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_TEMPORARY_FAILURE);
                    continue;
                }
                SetCallState(call, TCS_STATE_CALL_INITIATED);
            }
        }
    } else {
        result = OI_Tcs_SetupCallInfo(call, info);
        if (!OI_SUCCESS(result)) {
            OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_TEMPORARY_FAILURE);
            return;
        }
        SetCallState(call, TCS_STATE_CALL_INITIATED);
    }
}

OI_BOOL OI_Tcs_ActiveCallExists(void)
{
    OI_UINT i;

    if (OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) {
        for (i = 0; i < OI_Tcs_Data->numCalls; ++i) {
            if (!IsCallFree(&OI_Tcs_Data->calls[i]) && 
                (TCS_STATE_ACTIVE == OI_Tcs_Data->calls[i].state)) {
                return TRUE;
            }
        }
    } else {
        if (TCS_STATE_ACTIVE == OI_Tcs_Data->calls[0].state) {
            return TRUE;
        }
    }
    return FALSE;
}

OI_BOOL OI_Tcs_CallInProgress(void)
{
    OI_UINT i;

    if (OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) {
        for (i = 0; i < OI_Tcs_Data->numCalls; ++i) {
            if (!IsCallFree(&OI_Tcs_Data->calls[i]) && 
                (TCS_STATE_NULL != OI_Tcs_Data->calls[i].state)) {
                return TRUE;
            }
        }
    } else {
        if (TCS_STATE_NULL != OI_Tcs_Data->calls[0].state) {
            return TRUE;
        }
    }
    return FALSE;
}

/*******************************************************

  External functions

*******************************************************/

/**
 * Generic call establishment function.
 *
 * @param addr  address of remote device to connect to.  NULL indicates the call should be offered for all devices.
 * @param info  call info
 */
OI_STATUS OI_TCS_EstablishCall(OI_BD_ADDR *addr,
                               OI_TCS_CONNECT_INFO *info)
{
    OI_BOOL broadcast = !addr;
    OI_STATUS result;
    TCS_CALL_INFO *call;

    OI_DBGTRACE(("OI_TCS_EstablishCall %s\n", addr ? OI_BDAddrText(addr) : "multi-point"));

    /*
     * Validate arguments
     */
    CHECK_INIT;
    ARGCHECK(info);
    if ((info->callClass != OI_TCS_CALL_CLASS_EXTERNAL)
        && (info->callClass != OI_TCS_CALL_CLASS_INTERCOM)
        && (info->callClass != OI_TCS_CALL_CLASS_SERVICE)
        && (info->callClass != OI_TCS_CALL_CLASS_EMERGENCY)) {

        return OI_STATUS_INVALID_PARAMETERS;
    }
    if (info->bearer != NULL) {
        ARGCHECK((info->bearer->type == OI_TCS_BEARER_LINK_TYPE_ACL)
                 || (info->bearer->type == OI_TCS_BEARER_LINK_TYPE_SCO)
                 || (info->bearer->type == OI_TCS_BEARER_LINK_TYPE_NONE));
    }
    if (info->callingNumber && info->callingNumber->calling != TRUE) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    if (info->calledNumber && info->calledNumber->calling != FALSE) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    if (broadcast && (OI_Tcs_Data->role != OI_TCS_ROLE_GATEWAY)) {
        return OI_STATUS_INVALID_ROLE;
    }
     
    /*
     * TCS only supports point-to-point calls, so make sure that no call is 
     * currently in progress.
     */
    if (OI_Tcs_CallInProgress()) {
        return OI_TCS_CALL_IN_PROGRESS;
    }

    /*
     * Begin the call
     */
    if (broadcast) {
        call = NULL;
    } else {
        call = OI_Tcs_GetCall(addr);
        if (!call) {
            return OI_STATUS_NOT_CONNECTED;
        }
        if (TCS_ST_CONNECTED != call->tcsState) {
            return OI_STATUS_NOT_CONNECTED;
        }
    }

    result = OI_Tcs_Send_Setup(call, info->callClass, info->sendingComplete, info->bearer, 
        info->signal, info->callingNumber, info->calledNumber, info->companySpecific, info->companySpecificSize);
    if (OI_SUCCESS(result)) {
        SetupCall(call, info);
    }
    return result;
}


OI_STATUS OI_TCS_Connect(OI_BD_ADDR *addr,
                         OI_L2CAP_PSM psm)
{
    TCS_CALL_INFO *call;
    OI_STATUS result;

    ARGCHECK(addr);

    CHECK_INIT;
    if (OI_Tcs_Data->supportCtp) {
        ARGCHECK((OI_PSM_TCS == psm) || (OI_PSM_TCS_CORDLESS == psm));
    } else {
        ARGCHECK(OI_PSM_TCS == psm);
    }

    call = OI_Tcs_GetCall(addr);
    if (call) {
        return OI_STATUS_ALREADY_CONNECTED;
    }
    call = AllocateCall(addr, OUTGOING);
    if (!call) {
        return OI_STATUS_NO_RESOURCES;
    }

    if (OI_PSM_TCS == psm) {
        call->intercom = TRUE;
    }

    result = OI_L2CAP_Connect(connectCompleteCB, disconnectInd, recvDataInd,
                              psm, addr, NULL, &call->cid, &OI_Tcs_Data->tcsConnectPolicy);
    if (OI_SUCCESS(result)) {
        SetTcsState(call, TCS_ST_OUTGOING_CONNECT);
        call->connectInProgress = TRUE;
    } else {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("L2CAP Connect Failed"));
#endif
    }

    if (!OI_SUCCESS(result)) {
        OI_Tcs_FreeCall(call);
    }
    return result;
}


static void DisconnectTimeout(DISPATCH_ARG *darg)
{
    TCS_CALL_INFO *call;

    OI_DBGPRINT2(("Incoming side initiated disconnect, but did not complete it"));

    call = Dispatch_GetArg(darg, TCS_CALL_INFO *);

    SetTcsState(call, TCS_ST_OUTGOING_DISCONNECT);
    ContinueDisconnect(call);
}

static OI_STATUS ContinueDisconnect(TCS_CALL_INFO *call)
{
    OI_STATUS result = OI_OK;
    OI_BOOL disconnectComplete;
    OI_BOOL connectInProgress;
    OI_STATUS connectResult;
    OI_BD_ADDR addr;
    OI_UINT numConnections;
    OI_UINT i;
    DISPATCH_ARG darg;

    /*
     * Disconnect is complete when all links are disconnected and it is 
     * safe to release the per connection resources.
     */
    if (TCS_ST_OUTGOING_DISCONNECT == call->tcsState) {
        if (CH_ST_CONNECTED == call->bearerState) {
            OI_DBGPRINT2(("Disconnecting bearer channel\n"));
            result = OI_Tcs_DisconnectBearer(call);
        } else {
            if ((OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) && (CH_ST_CONNECTED == call->clState)) {
                OI_DBGPRINT2(("Disconnecting connection-less channel\n"));
                result = OI_L2CAP_GroupRemoveMember(OI_Tcs_Data->groupCID, &call->addr);
                if (OI_SUCCESS(result)) {
                    call->clState = CH_ST_DISCONNECTED;
                }
            } else {
                result = OI_OK;
            }

            if (OI_SUCCESS(result) && CH_ST_CONNECTED == call->controlState) {
                OI_DBGPRINT2(("Disconnecting control channel\n"));
                result = OI_L2CAP_Disconnect(call->cid);
                if (OI_SUCCESS(result)) {
                    call->controlState = CH_ST_DISCONNECT_IN_PROGRESS;
                }
            }
        }
        if (!OI_SUCCESS(result)) {
            /*
             * At least one of the connections is open, so return state to point where disconnect can 
             * be tried again.
             */
            OI_LOG_ERROR(("Disconnect failed inside ContinueDisconnect, reverting to CONNECTED state\n"));
            SetTcsState(call, TCS_ST_CONNECTED);
        }
    } else {
        /*
         * Incoming side initiated disconnect.  Start a timer for this side to initiate it if the other
         * side does not complete the disconnect.
         */
        if (!OI_Dispatch_IsValidHandle(call->disconnectTimeout)) {
            Dispatch_SetArg(darg, call);
            result = OI_Dispatch_RegisterTimedFunc(DisconnectTimeout, &darg, OI_SECONDS(10), &call->disconnectTimeout);
            if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("Register disconnect timeout handler failed"));
#endif
            }
        }
    }

    disconnectComplete = (CH_ST_DISCONNECTED == call->bearerState);
    disconnectComplete &= (OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) ? (CH_ST_DISCONNECTED == call->clState) : TRUE;
    disconnectComplete &= (CH_ST_DISCONNECTED == call->controlState);
    if (disconnectComplete) {
        /*
         * If all links are disconnected we can let the application know about the disconnect.
         */
        SetTcsState(call, TCS_ST_DISCONNECTED);
        if (OI_Dispatch_IsValidHandle(call->disconnectTimeout)) {
            OI_Dispatch_CancelFunc(call->disconnectTimeout);
        }

        numConnections = 0;
        for (i = 0; i < OI_Tcs_Data->numCalls; ++i) {
            if (!IsCallFree(&OI_Tcs_Data->calls[i])) {
                ++numConnections;
            }
        }
        if (0 == numConnections) {
            OI_Tcs_CancelTimer(NULL);
            OI_DEVMGR_DeregisterScoApplication(&scoCallbacks);
        }

        if (call->intercom && OI_Dispatch_IsValidHandle(call->intercomSetupTimer)) {
            OI_Dispatch_CancelFunc(call->intercomSetupTimer);
        }

        if (call->state != TCS_STATE_NULL) {
            OI_Tcs_Data->callbacks.releaseInd(&call->addr, OI_TCS_CAUSE_NO_CIRCUIT_AVAILABLE);
        }
        OI_Tcs_ReleaseCallInfo(call);
        SetCallState(call, TCS_STATE_NULL);

        addr = call->addr;
        connectInProgress = call->connectInProgress;
        connectResult = call->connectResult;
        OI_Tcs_FreeCall(call);

        if (connectInProgress) {
            OI_Tcs_Data->callbacks.connectCfm(&addr, connectResult);
        } else {
            OI_Tcs_Data->callbacks.disconnectCfm(&addr);
        }
    }
    return result;
}


/**
 * Shut down all the existing connections and notify the application
 *
 * @param call      data structure for call
 * @param reason    This is the status code returned in the CONNECT_CFM if a 
 *                  connection is in progress when this function is called
 */
OI_STATUS OI_Tcs_InternalDisconnect(TCS_CALL_INFO *call, OI_STATUS reason)
{
    if (call->connectInProgress) {
        call->connectResult = reason;
    }

    if (TCS_ST_DISCONNECTED == call->tcsState ||
        TCS_ST_INCOMING_DISCONNECT == call->tcsState ||
        TCS_ST_OUTGOING_DISCONNECT == call->tcsState) {
        /*
         * Disconnect has already begun or finished, so do nothing
         */
        return OI_OK;
    }

    SetTcsState(call, TCS_ST_OUTGOING_DISCONNECT);
    return ContinueDisconnect(call);
}

OI_STATUS OI_TCS_Disconnect(OI_BD_ADDR *addr)
{
    TCS_CALL_INFO *call;

    CHECK_INIT;
    ARGCHECK(addr);

    call = OI_Tcs_GetCall(addr);
    if (!call) {
        return OI_STATUS_NOT_FOUND;
    }
    if (TCS_ST_ADD_TO_GROUP == call->tcsState ||
        TCS_ST_CONNECTED == call->tcsState) {
        return OI_Tcs_InternalDisconnect(call, OI_STATUS_NOT_CONNECTED);
    } else {
        return OI_STATUS_NOT_CONNECTED;
    }
}

OI_STATUS OI_TCS_AcceptConnection(OI_BD_ADDR *addr,
                                  OI_BOOL accept)
{
    TCS_CALL_INFO *call;
    OI_STATUS result;

    CHECK_INIT;
    ARGCHECK(addr);

    call = OI_Tcs_GetCall(addr);
    if (!call) {
        return OI_STATUS_NOT_FOUND;
    }

    if (TCS_ST_INCOMING_CONNECT != call->tcsState) {
        return OI_STATUS_INVALID_STATE;
    }

    if (accept) {
        result = OI_L2CAP_Accept(connectCompleteCB, disconnectInd, recvDataInd, call->cid,
                                 NULL, TRUE, &OI_Tcs_Data->tcsConnectPolicy);
        if (OI_SUCCESS(result)) {
            call->connectInProgress = TRUE;
        } else {
            SetTcsState(call, TCS_ST_DISCONNECTED);
            OI_Tcs_FreeCall(call);
        }
    } else {
        result = OI_L2CAP_Accept(connectCompleteCB, disconnectInd, recvDataInd, call->cid,
                                 NULL, FALSE, &OI_Tcs_Data->tcsConnectPolicy);
        SetTcsState(call, TCS_ST_DISCONNECTED);
        OI_Tcs_FreeCall(call);
    }
    return result;
}


/**
 * This function clears a call in progress.
 *
 * @param addr   The Bluetooth device address associated with the call to disconnect
 * @param cause  the reason for the disconnect (0 to specify none)
 *
 */
OI_STATUS OI_TCS_ClearCall(OI_BD_ADDR *addr,
                           OI_TCS_CAUSE cause)
{
    OI_STATUS result = OI_OK;
    TCS_CALL_INFO *call;
    OI_UINT i;

    CHECK_INIT;

    if (!addr) {
        /*
         * Clear a multi-point call.  This operation is only valid if no call
         * is yet active.  In other words, the calling side has disconnected 
         * during call establishment.
         */
        if (OI_TCS_ROLE_GATEWAY != OI_Tcs_Data->role) {
            return OI_STATUS_INVALID_ROLE;
        }
        if (OI_Tcs_ActiveCallExists()) {
            return OI_STATUS_INVALID_STATE;
        }

        OI_Tcs_CancelTimer(NULL);

        for (i = 0; i < OI_Tcs_Data->numCalls; ++i) {
            call = &OI_Tcs_Data->calls[i];
            if (!IsCallFree(call) && (TCS_STATE_NULL != call->state)) {
                result = OI_Tcs_Send_Release(call, cause, 0);
                if (OI_SUCCESS(result)) {
                    SetCallState(call, TCS_STATE_RELEASE_REQ);
                } else {
                    OI_DBGPRINT(("Send RELEASE failed for %: - %!\n", &call->addr, result));
                    break;
                }
            }
        }
    } else {
        call = OI_Tcs_GetCall(addr);
        if (!call) {
            OI_DBGPRINT2(("Attempt to clear %:, but no call exists", addr));
            return OI_STATUS_NOT_CONNECTED;
        }

        if ((TCS_STATE_CALL_PRESENT == call->state) || (TCS_STATE_CALL_INITIATED == call->state)) {
            OI_Tcs_CancelTimer(call);
            if ((CH_ST_CONNECTED == call->bearerState) || (CH_ST_CONNECT_IN_PROGRESS == call->bearerState)) {
                OI_Tcs_DisconnectBearer(call);
            }
            result = OI_Tcs_Send_ReleaseComplete(call, cause, 0);
            if (OI_SUCCESS(result)) {
                OI_Tcs_ReleaseCallInfo(call);
                SetCallState(call, TCS_STATE_NULL);
            }
        } else if ((TCS_STATE_RELEASE_RECEIVED == call->state) || (TCS_STATE_RELEASE_COLLISION_RECEIVED == call->state)) {
            if (TCS_STATE_RELEASE_RECEIVED == call->state) { 
                result = OI_Tcs_Send_ReleaseComplete(call, cause, 0);
                if (!OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("Send RELEASE COMPLETE failed"));
#endif
                }
            }
            OI_Tcs_ReleaseCallInfo(call);
            SetCallState(call, TCS_STATE_NULL);
            OI_Tcs_Data->callbacks.releaseInd(&call->addr, cause);
            if (call->intercom) { 
                OI_Tcs_InternalDisconnect(call, OI_OK);
            }
        } else if ((TCS_STATE_NULL != call->state) &&
            (TCS_STATE_CALL_PRESENT != call->state) &&
            (TCS_STATE_CALL_INITIATED != call->state) &&
            (TCS_STATE_DISCONNECT_REQ != call->state) && 
            (TCS_STATE_DISCONNECT_IND != call->state) &&
            (TCS_STATE_RELEASE_REQ != call->state)) {

            OI_Tcs_CancelTimer(call);
            result = OI_Tcs_Send_Disconnect(call, cause, OI_TCS_PROGRESS_INFO_AVAILABLE, 0, 0);
            if (OI_SUCCESS(result)) {
                call->cause = cause;
                SetCallState(call, TCS_STATE_DISCONNECT_REQ);
            } else {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("Send DISCONNECT in OI_TCS_ClearCall failed"));
#endif
            }
        } else {
            result = OI_STATUS_INVALID_STATE;
        }
    }
    return result;
}


OI_STATUS OI_TCS_Info(OI_BD_ADDR *addr,
                      OI_BOOL sendingComplete,
                      OI_CHAR keypadFacility,
                      OI_TCS_NUMBER *callingNumber,
                      OI_TCS_NUMBER *calledNumber,
                      OI_TCS_AUDIO_CONTROL audioControl,
                      OI_UINT8 *companySpecific,
                      OI_UINT8 companySpecificSize)
{
    TCS_CALL_INFO *call;
    OI_STATUS result;

    CHECK_INIT;
    ARGCHECK(addr);

    call = OI_Tcs_GetCall(addr);
    if (!call) {
        return OI_STATUS_NOT_CONNECTED;
    }

    if (TCS_STATE_OVERLAP_SENDING == call->state) {
        result = OI_Tcs_Send_Information(call, sendingComplete, keypadFacility,
            callingNumber, calledNumber, audioControl, companySpecific, companySpecificSize, TCS_T304);
    } else if (TCS_STATE_OUTGOING_CALL_PROCEEDING == call->state ||
               TCS_STATE_CALL_DELIVERED == call->state ||
               TCS_STATE_CONNECT_RECEIVED == call->state ||
               TCS_STATE_ACTIVE == call->state) {

        result = OI_Tcs_Send_Information(call, sendingComplete, keypadFacility,
            callingNumber, calledNumber, audioControl, companySpecific, companySpecificSize, TCS_TNULL);
    } else {
        result = OI_STATUS_INVALID_STATE;
    }
    return result;
}


/**
 * This function notifies the TCS module of an application event.
 *
 * @param addr    The Bluetooth device address associated with the call
 * @param status  The status of which to notify TCS
 * @param bearer  NULL if none, only used in first response
 */
OI_STATUS OI_TCS_IncomingStatus(OI_BD_ADDR *addr,
                                OI_TCS_CALL_STATUS status,
                                OI_TCS_BEARER *bearer,
                                OI_UINT8 *companySpecific,
                                OI_UINT8 companySpecificSize)
{
    OI_STATUS result = OI_STATUS_INVALID_STATE;
    TCS_CALL_INFO *call;

    CHECK_INIT;
    ARGCHECK(addr);

    call = OI_Tcs_GetCall(addr);
    if (!call) {
        return OI_STATUS_NOT_CONNECTED;
    }

    result = SelectBearer(call, bearer);
    if (!OI_SUCCESS(result)) {
        return result;
    }

    switch (status) {
        case OI_TCS_CALL_STATUS_OVERLAP_RECEIVING:
            if (TCS_STATE_CALL_PRESENT == call->state) {
                result = OI_Tcs_Send_SetupAck(call, bearer, OI_TCS_PROGRESS_NONE, 0, 0, companySpecific, companySpecificSize);
                if (OI_SUCCESS(result)) {
                    SetCallState(call, TCS_STATE_OVERLAP_RECEIVING);
                }
            }
            break;

        case OI_TCS_CALL_STATUS_PROCEEDING:
            if (TCS_STATE_CALL_PRESENT == call->state
                || TCS_STATE_OVERLAP_RECEIVING == call->state) {

                result = OI_Tcs_Send_CallProceeding(call, bearer, OI_TCS_PROGRESS_NONE, 0, 0, companySpecific, companySpecificSize);
                if (OI_SUCCESS(result)) {
                    OI_Tcs_CancelTimer(call); // T302
                    SetCallState(call, TCS_STATE_INCOMING_CALL_PROCEEDING);
                }
            }
            break;

        case OI_TCS_CALL_STATUS_ALERTING:
            if (TCS_STATE_CALL_PRESENT == call->state
                || TCS_STATE_OVERLAP_RECEIVING == call->state
                || TCS_STATE_INCOMING_CALL_PROCEEDING == call->state) {

                result = OI_Tcs_Send_Alerting(call, bearer, OI_TCS_PROGRESS_NONE, 0, 0, companySpecific, companySpecificSize);
                if (OI_SUCCESS(result)) {
                    OI_Tcs_CancelTimer(call); // T302
                    SetCallState(call, TCS_STATE_CALL_RECEIVED);
                }
            }
            break;

        default:
            result = OI_STATUS_INVALID_PARAMETERS;
            break;
    }
    return result;
}

OI_STATUS OI_TCS_AcceptCall(OI_BD_ADDR *addr, 
                            OI_TCS_BEARER *bearer)
{
    TCS_CALL_INFO *call;
    OI_STATUS result;

    OI_DBGTRACE(("OI_TCS_AcceptCall %:\n", addr));

    CHECK_INIT;
    ARGCHECK(addr);

    call = OI_Tcs_GetCall(addr);
    if (!call) {
        result = OI_STATUS_NOT_CONNECTED;
    } else if (TCS_STATE_CALL_PRESENT == call->state ||
               TCS_STATE_OVERLAP_RECEIVING == call->state ||
               TCS_STATE_INCOMING_CALL_PROCEEDING == call->state ||
               TCS_STATE_CALL_RECEIVED == call->state) {

        result = SelectBearer(call, bearer);
        if (!OI_SUCCESS(result)) {
            return result;
        }

        if (CH_ST_DISCONNECTED == call->bearerState) {
            result = OI_Tcs_ConnectBearer(call);
            if (!OI_SUCCESS(result)) {
                return result;
            }
        }
        result = OI_Tcs_Send_Connect(call, bearer, 0);
        if (OI_SUCCESS(result)) {
            SetCallState(call, TCS_STATE_CONNECT_REQ);
        }
    } else {
        result = OI_STATUS_INVALID_STATE;
    }
    return result;
}

OI_STATUS OI_TCS_StartDTMF(OI_BD_ADDR *addr,
                           OI_CHAR tone)
{
    TCS_CALL_INFO *call;

    CHECK_INIT;
    ARGCHECK(addr);

    call = OI_Tcs_GetCall(addr);
    if (!call) {
        return OI_STATUS_NOT_CONNECTED;
    }
    if (TCS_STATE_ACTIVE != call->state) {
        return OI_STATUS_INVALID_STATE;
    }
    return OI_Tcs_Send_StartDTMF(call, tone);
}

OI_STATUS OI_TCS_StartDTMFAck(OI_BD_ADDR *addr,
                              OI_CHAR tone)
{
    TCS_CALL_INFO *call;

    CHECK_INIT;
    ARGCHECK(addr);

    call = OI_Tcs_GetCall(addr);
    if (!call) {
        return OI_STATUS_NOT_CONNECTED;
    }
    if (TCS_STATE_ACTIVE != call->state) {
        return OI_STATUS_INVALID_STATE;
    }
    return OI_Tcs_Send_StartDTMFAck(call, tone);
}

OI_STATUS OI_TCS_StartDTMFReject(OI_BD_ADDR *addr,
                                 OI_TCS_CAUSE cause)
{
    TCS_CALL_INFO *call;

    CHECK_INIT;
    ARGCHECK(addr);

    call = OI_Tcs_GetCall(addr);
    if (!call) {
        return OI_STATUS_NOT_CONNECTED;
    }
    if (TCS_STATE_ACTIVE != call->state) {
        return OI_STATUS_INVALID_STATE;
    }
    return OI_Tcs_Send_StartDTMFReject(call, cause);
}

OI_STATUS OI_TCS_StopDTMF(OI_BD_ADDR *addr)
{
    TCS_CALL_INFO *call;

    CHECK_INIT;
    ARGCHECK(addr);

    call = OI_Tcs_GetCall(addr);
    if (!call) {
        return OI_STATUS_NOT_CONNECTED;
    }
    if (TCS_STATE_ACTIVE != call->state) {
        return OI_STATUS_INVALID_STATE;
    }
    return OI_Tcs_Send_StopDTMF(call);
}

OI_STATUS OI_TCS_StopDTMFAck(OI_BD_ADDR *addr,
                             OI_CHAR tone)
{
    TCS_CALL_INFO *call;

    CHECK_INIT;
    ARGCHECK(addr);

    call = OI_Tcs_GetCall(addr);
    if (!call) {
        return OI_STATUS_NOT_CONNECTED;
    }
    if (TCS_STATE_ACTIVE != call->state) {
        return OI_STATUS_INVALID_STATE;
    }
    return OI_Tcs_Send_StopDTMFAck(call, tone);
}

OI_STATUS OI_TCS_CLInfo(OI_BD_ADDR *addr,
                        OI_TCS_AUDIO_CONTROL audioControl,
                        OI_UINT8 *companySpecific,
                        OI_UINT8 companySpecificSize)
{
    TCS_CALL_INFO *call;

    CHECK_INIT;

    if (!addr) {
        call = NULL;
    } else {
        call = OI_Tcs_GetCall(addr);
        if (!call) {
            return OI_STATUS_NOT_CONNECTED;
        }
    }
    return OI_Tcs_Send_CLInfo(call, audioControl, companySpecific, companySpecificSize);
}

OI_STATUS OI_CTP_EstablishCall(OI_BD_ADDR *addr,
                               OI_TCS_BEARER_LINK_TYPE linkType,
                               OI_TCS_NUMBER *callingNumber,
                               OI_TCS_NUMBER *calledNumber)
{
    OI_TCS_CONNECT_INFO info;
    OI_TCS_BEARER bearer;
    TCS_CALL_INFO *call;

    CHECK_INIT;

    ARGCHECK(callingNumber);
    ARGCHECK(calledNumber);

    if (OI_TCS_BEARER_LINK_TYPE_SCO == linkType) {
        bearer.type = OI_TCS_BEARER_LINK_TYPE_SCO;
#ifdef OI_CODE
#error code not present
#else
        /* JTL: The code uses the default values of HV3 everywhere except 
         * for here. I think this is a bug */
        bearer.info.sco.packetType = OI_TCS_BEARER_PACKET_TYPE_HV3;
#endif /* OI_CODE */
        bearer.info.sco.coding = OI_TCS_BEARER_CODING_CVSD;
    } else if (OI_TCS_BEARER_LINK_TYPE_NONE == linkType) {
        bearer.type = OI_TCS_BEARER_LINK_TYPE_NONE;
    } else {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (addr) {
        call = OI_Tcs_GetCall(addr);
        if (!call) {
            return OI_STATUS_NOT_FOUND;
        }
        if (call->intercom) {
            info.callClass = OI_TCS_CALL_CLASS_INTERCOM;
        } else {
            info.callClass = OI_TCS_CALL_CLASS_EXTERNAL;
        }
    } else {
        info.callClass = OI_TCS_CALL_CLASS_EXTERNAL;
    }

    info.sendingComplete = TRUE;
    info.bearer = &bearer;
    info.signal = OI_TCS_SIGNAL_NONE;
    info.callingNumber = callingNumber;
    info.calledNumber = calledNumber;
    info.companySpecific = NULL;
    info.companySpecificSize = 0;

    return OI_TCS_EstablishCall(addr, &info);
}
#endif /* FEATURE_BT_EXTPF_CTP */

