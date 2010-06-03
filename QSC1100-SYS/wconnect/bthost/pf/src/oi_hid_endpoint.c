/**
 * @file
 * @internal
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_HID

#include "oi_common.h"
#include "oi_bt_assigned_nos.h"
#include "oi_memmgr.h"
#include "oi_assert.h"
#include "oi_argcheck.h"
#include "oi_mbuf.h"
#include "oi_hid_endpoint.h"
#include "oi_hid_consts.h"
#include "oi_l2cap_mbuf.h"
#include "oi_std_utils.h"
#include "oi_securitydb.h"
#include "oi_init_flags.h"

#ifndef OI_CODE
#define BT_MSG_LAYER  BT_MSG_GN
#endif
/*
 * How long to wait for a transaction to complete.
 */
#define HID_TRANSACTION_TIMEOUT  OI_SECONDS(3)


/**
 * Connection policy.  Hardcoded with the exception of security policy which we
 * will get from the security database.
 */
static const OI_CONNECT_POLICY  ConstconnectPolicy =  {
    OI_UUID_HumanInterfaceDeviceService, /* OI_UINT32           serviceUuid32 ; */
    OI_SEC_REQUIRE_NOTHING,              /* securityPolicy is a place holder, get real policy at runtime */
    FALSE,                               /* OI_BOOL             mustBeMaster ; */
    NULL,                                /* OI_L2CAP_FLOWSPEC   *flowspec;    */
    0                                    /* OI_UINT8            powerSavingDisables ;    */
};

static OI_STATUS SendFragment(HID_CHANNEL *chan,
                              OI_BOOL first);

static void L2capControlConnectInd(OI_L2CAP_PSM psm, OI_BD_ADDR *addr, OI_L2CAP_CID cid);
static void L2capControlConnectCfm(OI_L2CAP_PSM psm, OI_UINT16 inMTU, OI_UINT16 outMTU, OI_STATUS result);
static void L2capControlDisconnectInd(OI_L2CAP_CID cid, OI_STATUS reason);

static void L2capInterruptConnectInd(OI_L2CAP_PSM psm, OI_BD_ADDR *addr, OI_L2CAP_CID cid);
static void L2capInterruptConnectCfm(OI_L2CAP_PSM psm, OI_UINT16 inMTU, OI_UINT16 outMTU, OI_STATUS result);
static void L2capInterruptDisconnectInd(OI_L2CAP_CID cid, OI_STATUS reason);

static void DoInterruptDisconnect(HID_CONNECTION *conn);
static void DoControlDisconnect(HID_CONNECTION *conn);

static void L2capRecvDataInd(OI_L2CAP_CID cid, OI_UINT8 *buf, OI_UINT16 len);
static void L2capWriteCfmM(OI_L2CAP_CID cid, OI_MBUF *buf, OI_STATUS status);

#ifdef OI_DEBUG
#error code not present
#endif

static OI_STATUS OI_HIDEndpoint_NewConnection(const OI_BD_ADDR *addr,
                                              OI_BOOL initiator,
                                              HID_CONNECTION **conn);

static void OI_HIDEndpoint_FreeConnection(HID_CONNECTION *conn);


/*
 * State for Hid service.
 */
static HID_THIS *HID_This;


/** "Safe" disconnect wrappers -- ensure that cleanup takes place no matter what
 */
typedef struct {
    OI_L2CAP_DISCONNECT_IND cb;
    OI_L2CAP_CID cid;
    OI_STATUS reason;
} DISCONNECT_PARAMS;

static void InvokeDisconnectCallback(DISPATCH_ARG *arg)
{
    DISCONNECT_PARAMS *params = Dispatch_GetPointerFromArg(arg, DISCONNECT_PARAMS);

    params->cb(params->cid, params->reason);
}

/* Attempts to disconnect an L2CAP channel. If the disconnect request fails, the
 * disconnect indication will be called anyway. It's possible that it may be
 * called more than once as a result, so, "caveat disconnect-indicatee"
 */
static void DoDisconnect(OI_L2CAP_DISCONNECT_IND cb, OI_L2CAP_CID cid)
{
    DISPATCH_ARG arg;
    DISCONNECT_PARAMS params;
    OI_STATUS status;

    status = OI_L2CAP_Disconnect(cid);
    if (!OI_SUCCESS(status)) {
        params.cb = cb;
        params.cid = cid;
        params.reason = status;

        Dispatch_SetArg(arg, params);

        status = OI_Dispatch_RegisterFunc(InvokeDisconnectCallback, &arg, NULL);
        if (!OI_SUCCESS(status)) {
            OI_LOG_ERROR(("OI_Dispatch_RegisterFunc failed: %!\n", status));
        }
    }
}

static void DoInterruptDisconnect(HID_CONNECTION *conn)
{
    if (conn->hid_interrupt.cid != 0) {
        conn->hid_interrupt.state = HID_DISCONNECTING;
        DoDisconnect(L2capInterruptDisconnectInd, conn->hid_interrupt.cid);
    } else {
        OI_LOG_ERROR(("DoInterruptDisconnect called on connection with cid 0"));
    }
}

static void DoControlDisconnect(HID_CONNECTION *conn)
{
    if (conn->control.cid != 0) {
        conn->control.state = HID_DISCONNECTING;
        DoDisconnect(L2capControlDisconnectInd, conn->control.cid);
    } else {
        OI_LOG_ERROR(("DoInterruptDisconnect called on connection with cid 0"));
    }
}

/** L2CAP connection status callbacks ***************************************/

static void L2capControlConnectInd(OI_L2CAP_PSM psm,
                                   OI_BD_ADDR *addr,
                                   OI_L2CAP_CID cid)
{
    HID_CONNECTION *conn = NULL;
    OI_STATUS status;

    OI_DBGPRINT(("L2capControlConnectInd"));
    OI_DBGPRINT2(("   psm=0x%02x addr=%: cid=0x%02x", psm, addr, cid));

    OI_ASSERT(psm == OI_PSM_HID_CONTROL);
    OI_ASSERT(OI_INIT_FLAG_VALUE(HID) != 0);

    /* Race condition, session no longer exists */
    if (!OI_INIT_FLAG_VALUE(HID)) {
        return;
    }

    status = OI_HIDEndpoint_NewConnection(addr, FALSE /* We're not the initiator */, &conn);
    if (OI_SUCCESS(status)) {
        conn->handle = cid;

        conn->control.cid = cid;
        conn->control.state = HID_CONNECTING;
        conn->control.expectingMore = FALSE;

        HID_This->HID_EndpointCallbacks->connectInd(addr, GET_HANDLE(conn));
        /* The application will call Accept as appropriate */

    } else {
        /* Refuse the connection */
        OI_L2CAP_Accept(L2capControlConnectCfm,
                        L2capControlDisconnectInd,
                        L2capRecvDataInd,
                        cid,
                        conn ? (OI_L2CAP_CONNECT_PARAMS *)conn->control.l2capParams : NULL,
                        FALSE,
                        &HID_This->connectPolicy);
    }
}

static void L2capInterruptConnectInd(OI_L2CAP_PSM psm,
                                     OI_BD_ADDR *addr,
                                     OI_L2CAP_CID cid)
{
    OI_BOOL accept;
    HID_CONNECTION *conn;
#ifndef OI_CODE
    OI_L2CAP_CONNECT_PARAMS *l2capParams = NULL;
#else
#error code not present
#endif

    OI_DBGPRINT(("L2capInterruptConnectInd"));
    OI_DBGPRINT2(("   psm=0x%02x addr=%: cid=0x%02x", psm, addr, cid));

    OI_ASSERT(psm == OI_PSM_HID_INTERRUPT);
    OI_ASSERT(OI_INIT_FLAG_VALUE(HID) != 0);

    /* Race condition, session no longer exists */
    if (!OI_INIT_FLAG_VALUE(HID))
        return;

    conn = OI_HIDEndpoint_GetConnectionByBDAddr(addr);

    if (conn == NULL) {
        OI_LOG_ERROR(("No connection associated with this BDADDR."));
        accept = FALSE;
    } else {
        if (conn->control.state != HID_CONNECTED) {
            OI_LOG_ERROR(("Control channel must be connected first. Refusing."));
            accept = FALSE;
        } else {
            conn->hid_interrupt.cid = cid;
            conn->hid_interrupt.state = HID_CONNECTING;
            l2capParams = conn->hid_interrupt.l2capParams;
            accept = TRUE;
        }
    }

    OI_L2CAP_Accept(L2capInterruptConnectCfm,
                    L2capInterruptDisconnectInd,
                    L2capRecvDataInd,
                    cid,
                    l2capParams,
                    accept,
                    &HID_This->connectPolicy);
}


static void L2capControlConnectCfm(OI_L2CAP_CID cid, OI_UINT16 inMTU, OI_UINT16 outMTU, OI_STATUS result)
{
    OI_STATUS status;
    HID_CONNECTION *conn;
#ifndef OI_CODE
    OI_L2CAP_CONNECT_PARAMS l2capParams;  
#endif 

    OI_DBGPRINT(("L2capControlConnectCfm"));
    OI_ASSERT(OI_INIT_FLAG_VALUE(HID) != 0);

    /* Race condition, session no longer exists */
    if (!OI_INIT_FLAG_VALUE(HID))
        return;

    conn = OI_HIDEndpoint_GetConnectionByCid(cid);
    if (conn == NULL) {
        /* Couldn't find a matching connection */
        OI_LOG_ERROR(("Unexpected L2cap connection confirmation"));
    } else if (!OI_SUCCESS(result)) {
        /* The connection failed; notify the application */
        HID_CONNECTION_HANDLE handle = GET_HANDLE(conn);
        OI_LOG_ERROR(("L2capControlConnectCfm: %!", result));

        OI_HIDEndpoint_FreeConnection(conn);
        HID_This->HID_EndpointCallbacks->connectCfm(handle, result);
    } else if (conn->control.state != HID_CONNECTING) {
        /* We didn't know about a pending connection */
        OI_LOG_ERROR(("Unexpected L2cap connection confirmation"));
    } else {
        conn->control.state = HID_CONNECTED;
        conn->control.inMTU = inMTU;
        conn->control.outMTU = outMTU;
        if (conn->initiator) {
#ifndef OI_CODE
            l2capParams.inMTU      = OI_L2CAP_MTU_MIN;
            l2capParams.outFlushTO = OI_L2CAP_FLUSHTO_INFINITE;
            conn->hid_interrupt.l2capParams = &l2capParams;
#endif
            status = OI_L2CAP_Connect(L2capInterruptConnectCfm,
                                      L2capInterruptDisconnectInd,
                                      L2capRecvDataInd,
                                      OI_PSM_HID_INTERRUPT,
                                      &conn->addr,
                                      (OI_L2CAP_CONNECT_PARAMS *)conn->hid_interrupt.l2capParams,
                                      &conn->hid_interrupt.cid,
                                      &HID_This->connectPolicy);
            OI_DBGPRINT(("OI_L2CAP_Connect: %!", status));
            if (OI_SUCCESS(status)) {
                conn->hid_interrupt.state = HID_CONNECTING;
                conn->hid_interrupt.inMTU = 0;
                conn->hid_interrupt.outMTU = 0;
            } else {
                /*
                 * Control channel was established but the interrupt channel
                 * failed. We need to disconnect the control channel.
                 */
                conn->state = HID_CONNECT_INCOMPLETE;
                DoControlDisconnect(conn);
            }
        }
    }
}

static void L2capInterruptConnectCfm(OI_L2CAP_CID cid,
                                     OI_UINT16 inMTU,
                                     OI_UINT16 outMTU,
                                     OI_STATUS result)
{
    HID_CONNECTION_HANDLE handle;
    HID_CONNECTION *conn;

    OI_DBGPRINT(("L2capInterruptConnectCfm"));
    OI_ASSERT(OI_INIT_FLAG_VALUE(HID) != 0);

    /* Race condition, session no longer exists */
    if (!OI_INIT_FLAG_VALUE(HID)) {
        return;
    }

    conn = OI_HIDEndpoint_GetConnectionByCid(cid);
    if ((conn == NULL) || (conn->hid_interrupt.state != HID_CONNECTING)) {
        OI_LOG_ERROR(("Unexpected L2cap connection confirmation"));
        return;
    }

    handle = GET_HANDLE(conn);
    if (OI_SUCCESS(result)) {
        conn->state = HID_CONNECTED;
        conn->hid_interrupt.state = HID_CONNECTED;
        conn->hid_interrupt.inMTU = inMTU;
        conn->hid_interrupt.outMTU = outMTU;
        conn->protocol = OI_HID_PROTOCOL_REPORT;
    } else {
        OI_HIDEndpoint_FreeConnection(conn);
    }
    HID_This->HID_EndpointCallbacks->connectCfm(handle, result);
}

static void L2capControlDisconnectInd(OI_L2CAP_CID cid,
                                      OI_STATUS reason)
{
    HID_CONNECTION *conn;
    OI_DBGPRINT(("L2capControlDisconnectInd: %!", reason));
    OI_ASSERT(OI_INIT_FLAG_VALUE(HID) != 0);

    /* Race condition, session no longer exists */
    if (!OI_INIT_FLAG_VALUE(HID)) {
        return;
    }

    conn = OI_HIDEndpoint_GetConnectionByCid(cid);
    if (conn == NULL) {
        OI_LOG_ERROR(("Unexpected L2cap disconnect indication"));
        return;
    }

    if (conn->state == HID_CONNECT_INCOMPLETE) {
        /*
         * Still need to confirm the connection failure to the app.
         */
        HID_CONNECTION_HANDLE handle = GET_HANDLE(conn);
        OI_HIDEndpoint_FreeConnection(conn);
        HID_This->HID_EndpointCallbacks->connectCfm(handle, OI_FAIL);
        return;
    }

    if (conn->hid_interrupt.state == HID_IDLE) {
        /* The interrupt channel was (correctly) closed first, so we're
         * responsible for notifying the app */

        HID_CONNECTION_HANDLE handle = GET_HANDLE(conn);
        OI_HIDEndpoint_FreeConnection(conn);
        HID_This->HID_EndpointCallbacks->disconnectInd(handle, reason);
    } else {
        OI_DBGPRINT(("Control channel disconnected before hid_interrupt channel"));
        /*
         * For some reason the control channel dropped before the interrupt
         * channel did. We'll force the interrupt channel to drop just in case
         * the remote side isn't going to do it for us.
         */
        conn->control.cid = 0;
        conn->control.state = HID_IDLE;
        conn->control.inMTU = 0;
        conn->control.outMTU = 0;

        DoInterruptDisconnect(conn);
    }
}

static void L2capInterruptDisconnectInd(OI_L2CAP_CID cid, OI_STATUS reason)
{
    HID_CONNECTION *conn;

    OI_DBGPRINT(("L2capInterruptDisconnectInd: %!", reason));
    OI_ASSERT(OI_INIT_FLAG_VALUE(HID) != 0);

    /* Race condition, session no longer exists */
    if (!OI_INIT_FLAG_VALUE(HID)) {
        return;
    }
    conn = OI_HIDEndpoint_GetConnectionByCid(cid);
    if (conn == NULL) {
        OI_LOG_ERROR(("Unexpected L2cap disconnect indication"));
        return;
    }

    conn->hid_interrupt.cid = 0;
    conn->hid_interrupt.state = HID_IDLE;
    conn->hid_interrupt.inMTU = 0;
    conn->hid_interrupt.outMTU = 0;

    if (conn->state == HID_DISCONNECTING) {
        /* If we initiated the disconnection, we need to disconnect the control
         * channel now */
        DoControlDisconnect(conn);
    } else {
        conn->state = HID_DISCONNECTING;
    }

    if (conn->control.state == HID_IDLE) {
        /* The control channel was already (incorrectly) closed, so we're
         * responsible for freeing the connection and notifying the app */
        HID_CONNECTION_HANDLE handle = GET_HANDLE(conn);
        OI_HIDEndpoint_FreeConnection(conn);
        HID_This->HID_EndpointCallbacks->disconnectInd(handle, reason);
    }
}

static void L2capRecvDataInd(OI_L2CAP_CID cid, OI_UINT8 *data, OI_UINT16 len)
{

    OI_UINT8 transType;
    OI_UINT8 transParam;
    OI_BOOL final;
    HID_CONNECTION *conn;

    OI_DBGPRINT2(("Control channel (%d bytes): %@", len, data, len));

    OI_ASSERT(OI_INIT_FLAG_VALUE(HID) != 0);

    /* Race condition, session no longer exists */
    if (!OI_INIT_FLAG_VALUE(HID))
        return;

    conn = OI_HIDEndpoint_GetConnectionByCid(cid);
    if (conn == NULL) {
        OI_LOG_ERROR(("Unexpected L2capRecvDataInd"));
        return;
    }

    if (len < 1) {
        OI_LOG_ERROR(("0 bytes received"));
        return;
    }

    OI_ASSERT(len <= conn->control.inMTU);
    final = (len < conn->control.inMTU);

    transType = HID_TRANSACTION_HEADER_GET_TYPE(data[0]);
    transParam = HID_TRANSACTION_HEADER_GET_PARAM(data[0]);
    data++;
    len--;

    OI_DBGPRINT2(("calling recvInd(handle=%d, transType=%1x, transParam=%1x, len=%d, final=%d):\n%@",
                  GET_HANDLE(conn), transType, transParam, len, final, data, len));

    if (cid == conn->control.cid) {
        HID_This->HID_EndpointCallbacks->controlRecvInd(GET_HANDLE(conn), transType, transParam, data, len, final);
    } else if (cid == conn->hid_interrupt.cid) {
        HID_This->HID_EndpointCallbacks->interruptRecvInd(GET_HANDLE(conn), transType, transParam, data, len, final);
    } else {
        OI_LOG_ERROR(("recvDataInd cid (%d) doesn't match control (%d) or interrupt (%d)", cid, conn->control.cid, conn->hid_interrupt.cid));
    }
}

#ifndef OI_CODE
static void HostEpTransactionTimeoutCB(DISPATCH_ARG *darg)
#else
#error code not present
#endif
{
    HID_CONNECTION *conn = Dispatch_GetArg(darg, HID_CONNECTION*);

    OI_LOG_ERROR(("Didn't get transaction response within timeout period"));

    if (conn == NULL) {
        OI_LOG_ERROR(("Invalid HID host handle in transaction timeout"));
        return;
    }
    conn->timeoutCB = 0;

    if (HID_This->HID_EndpointCallbacks->transactionTimeoutInd) {
        HID_This->HID_EndpointCallbacks->transactionTimeoutInd(conn);
    }
}

/** Exposed API ************************************************************/

void OI_HIDEndpoint_SetTransactionTimeout(HID_CONNECTION_HANDLE handle)
{
    HID_CONNECTION *conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    DISPATCH_ARG darg;

    OI_ASSERT(conn);

    Dispatch_SetArg(darg, conn);
    OI_DBGPRINT2(("Setting transaction timeout on handle %d", handle));
#ifndef OI_CODE
    OI_Dispatch_RegisterTimedFunc(HostEpTransactionTimeoutCB, &darg, HID_TRANSACTION_TIMEOUT, &conn->timeoutCB);
#else
#error code not present
#endif
}


void OI_HIDEndpoint_CancelTransactionTimeout(HID_CONNECTION_HANDLE handle)
{
    HID_CONNECTION *conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        OI_LOG_ERROR(("CancelTransactionTimeout called on invalid handle %d", handle));
    } else if (conn->timeoutCB) {
        OI_DBGPRINT2(("Canceling transaction timeout on handle %d", handle));
        OI_Dispatch_CancelFunc(conn->timeoutCB);
        conn->timeoutCB = 0;
    } else {
        OI_DBGPRINT2(("CancelTransactionTimeout called, but no transaction on handle %d", handle));
    }
}

#ifdef OI_DEBUG
#error code not present
#endif
#ifdef OI_DEBUG
#error code not present
#else
#define ASSERT_CONSISTENCY()
#endif

static OI_STATUS OI_HIDEndpoint_NewConnection(const OI_BD_ADDR *addr,
                                              OI_BOOL initiator,
                                              HID_CONNECTION **conn)
{
    OI_UINT i;
    HID_CONNECTION *empty = NULL;

    ASSERT_CONSISTENCY();
    OI_ASSERT (conn);

    for (i = 0; i < HID_This->HID_MaxConnections; ++i) {
        if (HID_This->HID_Connections[i].state == HID_IDLE) {
            empty = &HID_This->HID_Connections[i];
        } else if (SAME_BD_ADDR(addr, &HID_This->HID_Connections[i].addr)) {
            OI_LOG_ERROR(("A connection to %: already exists", addr));
            OI_DBGPRINT(("Connection to %: is currently \"%s\"", addr, Debug_StateString(HID_This->HID_Connections[i].state)));
            return OI_STATUS_ALREADY_CONNECTED;
        }
    }

    if (empty) {
        *conn = empty;
        HID_This->connectionCount++;
        (*conn)->state = HID_CONNECTING;
        (*conn)->addr = *addr;
        (*conn)->initiator = initiator;
        return OI_OK;
    } else {
        OI_LOG_ERROR(("Too many connections. Refusing connection allocation request."));
        return OI_STATUS_NO_RESOURCES;
    }
}

/* Frees any fragment buffers associated with a connection, cancles the
 * transaction timeout if present, and resets all connection state to idle.
 * This function should be called *prior* to any application notification which
 * might cause a reconnection attempt.
 */
static void OI_HIDEndpoint_FreeConnection(HID_CONNECTION *conn)
{
    OI_ASSERT(conn != NULL);
    OI_ASSERT(conn->state != HID_IDLE);

    ASSERT_CONSISTENCY();

    if (conn->timeoutCB) {
        OI_HIDEndpoint_CancelTransactionTimeout(GET_HANDLE(conn));
    }

    OI_MemZero(conn, sizeof(*conn));
    HID_This->connectionCount--;
}


HID_CONNECTION *OI_HIDEndpoint_GetConnectionByHandle(HID_CONNECTION_HANDLE handle)
{
    OI_UINT i;

    if (!OI_INIT_FLAG_VALUE(HID)) {
        OI_DBGPRINT(("HID not initialized"));
        return NULL;
    }

    ASSERT_CONSISTENCY();

    if (handle == 0) {
        return NULL;
    }

    for (i = 0; i < HID_This->HID_MaxConnections; ++i) {
        if (HID_This->HID_Connections[i].state != HID_IDLE && handle == GET_HANDLE(&HID_This->HID_Connections[i])) {
            return &HID_This->HID_Connections[i];
        }
    }

    return NULL;
}

HID_CONNECTION *OI_HIDEndpoint_GetConnectionByBDAddr(OI_BD_ADDR const *addr)
{
    OI_UINT i;
    ASSERT_CONSISTENCY();

    if (!OI_INIT_FLAG_VALUE(HID)) {
        OI_DBGPRINT(("HID not initialized"));
        return NULL;
    }

    if (addr == NULL) {
        return NULL;
    }

    for (i = 0; i < HID_This->HID_MaxConnections; ++i) {
        if (HID_This->HID_Connections[i].state != HID_IDLE && SAME_BD_ADDR(addr, &HID_This->HID_Connections[i].addr)) {
            return &HID_This->HID_Connections[i];
        }
    }

    return NULL;
}

HID_CONNECTION *OI_HIDEndpoint_GetConnectionByCid(OI_L2CAP_CID cid)
{
    OI_UINT i;
    ASSERT_CONSISTENCY();

    if (!OI_INIT_FLAG_VALUE(HID)) {
        OI_DBGPRINT(("HID not initialized"));
        return NULL;
    }

    if (cid == 0) {
        return NULL;
    }

    for (i = 0; i < HID_This->HID_MaxConnections; ++i) {
        if ((HID_This->HID_Connections[i].control.cid) == cid || (HID_This->HID_Connections[i].hid_interrupt.cid == cid)) {
            return &HID_This->HID_Connections[i];
        }
    }

    return NULL;
}

HID_CHANNEL *OI_HIDEndpoint_GetChannel(HID_CONNECTION_HANDLE handle, OI_L2CAP_CID psm)
{
    HID_CONNECTION *conn;
    ASSERT_CONSISTENCY();

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return NULL;
    }
    switch (psm) {
        case OI_PSM_HID_CONTROL:
            return &conn->control;
        case OI_PSM_HID_INTERRUPT:
            return &conn->hid_interrupt;
        default:
            return NULL;
    }
}

OI_STATUS OI_HIDEndpoint_Register(HID_ENDPOINT_CALLBACKS const *cb,
                                  OI_UINT maxConnections,
                                  HID_THIS **hidThis)
{
    OI_STATUS status;
#ifndef OI_CODE
    OI_L2CAP_CONNECT_PARAMS params;
#endif 

    if (OI_INIT_FLAG_VALUE(HID)) {
        return OI_STATUS_ALREADY_REGISTERED;
    }

    ARGCHECK(cb != NULL);
    ARGCHECK(cb->connectInd && cb->connectCfm && cb->disconnectInd && cb->controlRecvInd && cb->interruptRecvInd);

    HID_This = OI_Calloc(sizeof(HID_THIS));
    if (HID_This == NULL) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    HID_This->connectionCount = 0;
    HID_This->connectPolicy = ConstconnectPolicy;
    HID_This->HID_MaxConnections = maxConnections;
    HID_This->HID_Connections = OI_Calloc(sizeof(HID_CONNECTION) * maxConnections);
    HID_This->HID_EndpointCallbacks = cb;

    if (HID_This->HID_Connections == NULL) {
        status = OI_STATUS_OUT_OF_MEMORY;
        goto failure;
    }

    /* get security policy. */
    HID_This->connectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(HID_This->connectPolicy.serviceUuid32);

#ifndef OI_CODE
    params.inMTU      = OI_L2CAP_MTU_MIN;
    params.outFlushTO = OI_L2CAP_FLUSHTO_INFINITE;
    status = OI_L2CAP_Register(OI_PSM_HID_CONTROL, L2capControlConnectInd, &params);
#else
#error code not present
#endif
    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("Couldn't regster OI_PSM_HID_CONTROL: %!", status));
        goto failure;
    }

#ifndef OI_CODE
    status = OI_L2CAP_Register(OI_PSM_HID_INTERRUPT, L2capInterruptConnectInd, &params);
#else
#error code not present
#endif

    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("Couldn't regster OI_PSM_HID_INTERRUPT: %!", status));
        goto failure;
    }

    OI_INIT_FLAG_PUT_FLAG(TRUE, HID);

    /*
     * Pass "HID_This" pointer back to caller
     */
    *hidThis = HID_This;
    ASSERT_CONSISTENCY();

    return OI_OK;

failure:
    OI_FreeIf(&HID_This->HID_Connections);
    OI_L2CAP_Deregister(OI_PSM_HID_CONTROL);
    OI_L2CAP_Deregister(OI_PSM_HID_INTERRUPT);
    OI_FreeIf(&HID_This);

    return status;
}

OI_STATUS OI_HIDEndpoint_Deregister(void)
{
    OI_STATUS result;

    if (!OI_INIT_FLAG_VALUE(HID)) {
        return OI_STATUS_NOT_REGISTERED;
    }
    ASSERT_CONSISTENCY();

    /* Don't allow de-registration if connections are alive */
    if(HID_This->connectionCount){
        return OI_STATUS_STILL_CONNECTED;
    }

    result = OI_L2CAP_Deregister(OI_PSM_HID_CONTROL);
    if (!OI_SUCCESS(result)) {
        return result;
    }
    result = OI_L2CAP_Deregister(OI_PSM_HID_INTERRUPT);
    if (!OI_SUCCESS(result)) {
        return result;
    }

    HID_This->HID_EndpointCallbacks = NULL;
    HID_This->HID_MaxConnections = 0;
    OI_FreeIf(&HID_This->HID_Connections);
    OI_FreeIf(&HID_This);

    OI_INIT_FLAG_PUT_FLAG(FALSE, HID);

    return OI_OK;
}

OI_STATUS OI_HIDEndpoint_Connect(OI_BD_ADDR const *addr,
                                 OI_HID_CONNECTION_PARAMS const *params,
                                 HID_CONNECTION_HANDLE *handle)
{
    OI_STATUS status;
    HID_CONNECTION *conn;
#ifndef OI_CODE
    OI_L2CAP_CONNECT_PARAMS l2capParams;
#endif 

    OI_CHECK_INIT(HID);
    ASSERT_CONSISTENCY();


    status = OI_HIDEndpoint_NewConnection(addr, TRUE /* We are the initiator */, &conn);
    if (!OI_SUCCESS(status)) {
        return status;
    }

#ifndef OI_CODE
    l2capParams.inMTU         = OI_L2CAP_MTU_MIN;
    l2capParams.outFlushTO    = OI_L2CAP_FLUSHTO_INFINITE;
    conn->control.l2capParams = &l2capParams;
#endif

    status = OI_L2CAP_Connect(L2capControlConnectCfm,
                              L2capControlDisconnectInd,
                              L2capRecvDataInd,
                              OI_PSM_HID_CONTROL,
                              &conn->addr,
                              (OI_L2CAP_CONNECT_PARAMS *)conn->control.l2capParams,
                              &conn->control.cid,
                              &HID_This->connectPolicy);
    if (OI_SUCCESS(status)) {
        conn->handle = conn->control.cid;
        conn->control.state = HID_CONNECTING;
        conn->state = HID_CONNECTING;
        *handle = conn->control.cid;
    } else {
        OI_HIDEndpoint_FreeConnection(conn);
    }

    return status;
}

OI_STATUS OI_HIDEndpoint_Accept(HID_CONNECTION_HANDLE handle,
                                OI_HID_CONNECTION_PARAMS const *params,
                                OI_BOOL allow)
{
    HID_CONNECTION *conn;
    OI_STATUS       status;

    OI_DBGPRINT(("OI_HIDEndpointAccept %d %s", handle, allow ? "TRUE" : "FALSE"));

    OI_CHECK_INIT(HID);
    ASSERT_CONSISTENCY();

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }

    conn->control.l2capParams = params ? params->controlParams : NULL;
    conn->control.l2capQos = params ? params->controlQos : NULL;
    conn->hid_interrupt.l2capParams = params ? params->interruptParams : NULL;
    conn->hid_interrupt.l2capQos = params ? params->controlQos : NULL;

    status = OI_L2CAP_Accept(L2capControlConnectCfm,
                             L2capControlDisconnectInd,
                             L2capRecvDataInd,
                             conn->control.cid,
                             (OI_L2CAP_CONNECT_PARAMS *)conn->control.l2capParams,
                             allow,
                             &HID_This->connectPolicy);

    if ((!allow) || !OI_SUCCESS(status)) {
        OI_HIDEndpoint_FreeConnection(conn);
    }
    return status;
}

OI_STATUS OI_HIDEndpoint_Disconnect(HID_CONNECTION_HANDLE handle)
{
    HID_CONNECTION *conn;

    OI_CHECK_INIT(HID);
    ASSERT_CONSISTENCY();

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        OI_DBGPRINT(("Disconnect requested for unknown handle %d", handle));
        return OI_STATUS_INVALID_HANDLE;
    }
    if (conn->state != HID_CONNECTED) {
        return OI_STATUS_NOT_CONNECTED;
    }

    OI_DBGPRINT(("Disconnecting handle %d...", handle));

    conn->state = HID_DISCONNECTING;
    DoInterruptDisconnect(conn);
    return OI_OK;
}

OI_STATUS OI_HIDEndpoint_SendSimple(HID_CONNECTION_HANDLE handle,
                                    OI_L2CAP_PSM channel,
                                    OI_BYTE transType,
                                    OI_BYTE transParam)
{
    OI_STATUS status;
    HID_CONNECTION *conn;
    OI_MBUF *buf;

    OI_CHECK_INIT(HID);
    ASSERT_CONSISTENCY();

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }

    buf = OI_MBUF_Alloc(0 /* NULL mbuf */);
    if (buf == NULL) {
        return OI_STATUS_OUT_OF_MEMORY;
    }
    status = OI_HIDEndpoint_SendMbuf(OI_HIDEndpoint_FreeMbufCb, handle, channel, transType, transParam, buf);
    if (!OI_SUCCESS(status)) {
        OI_MBUF_Free(buf);
    }

    return status;
}

OI_STATUS OI_HIDEndpoint_SendByte(HID_CONNECTION_HANDLE handle,
                                  OI_L2CAP_PSM channel,
                                  OI_BYTE transType,
                                  OI_BYTE transParam,
                                  OI_BYTE byte)
{
    OI_MBUF *buf;
    OI_STATUS status;
    HID_CONNECTION *conn;
    HID_CHANNEL *chan;

    OI_CHECK_INIT(HID);
    ASSERT_CONSISTENCY();

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }

    chan = OI_HIDEndpoint_GetChannel(handle, channel);
    if (chan == NULL) {
        OI_LOG_ERROR(("channel must be OI_PSM_HID_INTERRUPT or OI_PSM_HID_CONTROL"));
        return OI_STATUS_INVALID_PARAMETERS;
    }

    buf = OI_MBUF_Wrap(&byte, 1, MBUF_COPY);
    if (buf == NULL) {
        return OI_STATUS_OUT_OF_MEMORY;
    }
    chan->sendCfm = NULL;
    status = OI_HIDEndpoint_SendMbuf(OI_HIDEndpoint_FreeMbufCb,
                                     handle,
                                     channel,
                                     transType,
                                     transParam,
                                     buf);
    if (!OI_SUCCESS(status)) {
        OI_MBUF_Free(buf);
    }

    return status;
}

OI_STATUS OI_HIDEndpoint_SendData(HID_ENDPOINT_SEND_CFM sendCfm,
                                  HID_CONNECTION_HANDLE handle,
                                  OI_L2CAP_PSM channel,
                                  OI_BYTE transType,
                                  OI_BYTE transParam,
                                  OI_BYTE const *data,
                                  OI_UINT16 len)
{
    OI_MBUF *buf;
    OI_STATUS status;
    HID_CONNECTION *conn;
    HID_CHANNEL *chan;

    OI_CHECK_INIT(HID);
    ASSERT_CONSISTENCY();

    OI_DBGPRINT2(("SendData(sendCfm=%08x, handle=%d, chan=%d, transType=%1x, transParam=%1x, len=%d):\n%@",
                  sendCfm, handle, channel, transType, transParam, len, data, len));

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }

    chan = OI_HIDEndpoint_GetChannel(handle, channel);
    if (chan == NULL) {
        OI_LOG_ERROR(("channel must be OI_PSM_HID_INTERRUPT or OI_PSM_HID_CONTROL"));
        return OI_STATUS_INVALID_PARAMETERS;
    }

    buf = OI_MBUF_Wrap((OI_BYTE *)data, len, MBUF_KEEP);
    if (buf == NULL) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    status = OI_HIDEndpoint_SendMbuf(OI_HIDEndpoint_FreeMbufCb,
                                     handle,
                                     channel,
                                     transType,
                                     transParam,
                                     buf);
    if (!OI_SUCCESS(status)) {
        chan->sendCfm = NULL;
        chan->clientDataPtr = NULL;
        chan->clientDataLen = 0;
        OI_MBUF_Free(buf);
    } else {
        chan->sendCfm = sendCfm;
        chan->clientDataPtr = data;
        chan->clientDataLen = len;
    }

    return status;
}

OI_STATUS OI_HIDEndpoint_SendMbuf(HID_ENDPOINT_SEND_CFM sendCfm,
                                  HID_CONNECTION_HANDLE handle,
                                  OI_L2CAP_PSM channel,
                                  OI_BYTE transType,
                                  OI_BYTE transParam,
                                  OI_MBUF *buf)
{
    HID_CONNECTION *conn;
    HID_CHANNEL *chan;
    OI_STATUS status;

    OI_CHECK_INIT(HID);
    ASSERT_CONSISTENCY();

    OI_DBGPRINT2(("SendMBuf(transType=%1x, transParam=%1x)", transType, transParam));

    ARGCHECK(buf != NULL);

    conn = OI_HIDEndpoint_GetConnectionByHandle(handle);
    if (conn == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    }

    if (conn->state != HID_CONNECTED) {
        if (conn->state == HID_DISCONNECTING) {
            OI_DBGPRINT(("Connetion going away. Not sending report."));
        } else {
            OI_LOG_ERROR(("connection isn't in connected state"));
        }
        return OI_STATUS_NOT_CONNECTED;
    }

    chan = OI_HIDEndpoint_GetChannel(handle, channel);
    if (chan == NULL) {
        OI_LOG_ERROR(("channel must be OI_PSM_HID_INTERRUPT or OI_PSM_HID_CONTROL"));
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (chan->buf != NULL) {
        return OI_STATUS_WRITE_IN_PROGRESS;
    }

    chan->buf = buf;

    chan->sendCfm = sendCfm;
    chan->transType = transType;
    chan->transParam = transParam;

    status = SendFragment(chan, TRUE /* first */);

    if (!OI_SUCCESS(status)) {
        chan->buf = NULL;
    }
    return status;
}

void OI_HIDEndpoint_FreeMbufCb(HID_CHANNEL *chan, OI_STATUS status)
{
    ASSERT_CONSISTENCY();

    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("WriteCfm error in HIDEndpoint_FreeMbufCb: %!", status));
    }

    if (chan && chan->buf) {
        OI_MBUF_Free(chan->buf);
    }
}


/*** Private functions */

void L2capWriteCfmM(OI_L2CAP_CID cid, OI_MBUF *fragBuf, OI_STATUS status)
{
    HID_CHANNEL *chan;
    OI_BOOL lastPacket;

    OI_DBGPRINT2(("+L2capWriteCfm: %!\n", status));

#ifdef OI_CODE
#error code not present
#endif

    OI_ASSERT(fragBuf != NULL);

#ifndef OI_CODE
    chan = (HID_CHANNEL *)fragBuf->context_ptr;
#else
#error code not present
#endif
    OI_MBUF_Free(fragBuf);

    OI_ASSERT(chan != NULL);
    OI_ASSERT(chan->cid == cid);

    OI_HIDEndpoint_DumpChannel(chan);

    lastPacket = chan->lastFragment;
    /* Send the next packet if one needs to be sent and the last one succeeded
     * */
    if (OI_SUCCESS(status) && !lastPacket) {
        status = SendFragment(chan, FALSE /* Not the first */);
        /* If this fails, fall through just as though this callback had been
         * informed of a previous failure */
    }

    /* Notify the application, either because the final packet was successful,
     * or because an error occured. */
    if (lastPacket || !OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#endif
//        OI_MBUF_Reset(chan->buf);
        chan->sendCfm(chan, status);
        chan->buf = NULL;
    }

    OI_DBGPRINT2(("-L2capWriteCfm"));
}


static OI_STATUS SendFragment(HID_CHANNEL *chan,
                              OI_BOOL first)
{
    OI_UINT16 toSend;
    OI_STATUS status;
    OI_MBUF *fragBuf;

    OI_DBGPRINT2(("SendFragment%s", first ? " first" : ""));
    OI_HIDEndpoint_DumpChannel(chan);

    if (first) {
        OI_MBUF_SetWindow(chan->buf, (OI_UINT16)(chan->outMTU - HID_TRANSACTION_HEADER_LEN));
    } else {
        OI_MBUF_AdvanceWindow(chan->buf, (OI_UINT16)(chan->outMTU - HID_TRANSACTION_HEADER_LEN));
    }

    toSend = OI_MBUF_NumBytes(chan->buf) + HID_TRANSACTION_HEADER_LEN;  /* This is the length of the windowed portion of chan->buf, plus the header that will be appended */
    OI_DBGPRINT2(("toSend = %d", toSend));

    OI_ASSERT(toSend <= chan->outMTU); /* SetWindow has already been called on chan->buf */

    /* HID spec says that there is always a continuation packet following a
     * packet of MTU length */
#ifdef OI_CODE
#error code not present
#else
    if (toSend >= chan->outMTU) {
#endif
        chan->lastFragment = FALSE;
    } else {
        chan->lastFragment = TRUE;
    }

    /*
     * Fragmentation mbuf needs to have room for two cells: one for the header the other for buf.
     */
    fragBuf = OI_MBUF_Alloc(2);
    if (fragBuf == NULL) {
        status = OI_STATUS_OUT_OF_MEMORY;
        goto exit;
    }

    chan->header = HID_TRANSACTION_HEADER(first ? chan->transType : OI_HID_DATC, chan->transParam);

#ifndef OI_CODE
    OI_MBUF_PullMBuf(fragBuf, chan->buf, (OI_UINT16)(chan->outMTU - HID_TRANSACTION_HEADER_LEN));
    status = OI_MBUF_Prepend(fragBuf, &chan->header, HID_TRANSACTION_HEADER_LEN, MBUF_COPY);
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("OI_MBUF_AppendMbuf failed: %!", status));
        goto exit;
    }
#else
#error code not present
#endif

#ifndef OI_CODE
    fragBuf->context_ptr = (void *)chan;
#else
#error code not present
#endif

#ifdef OI_CODE
#error code not present
#endif

#ifndef OI_CODE
    status = OI_L2CAP_WriteMBUF(L2capWriteCfmM, chan->cid, fragBuf, NULL);
#else
#error code not present
#endif

    BT_MSG_HIGH("L2CAP Write mbuf : status=%x",status,0,0);
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("OI_L2CAP_WriteMBUF failed: %!", status));
        goto exit;
    }
#ifdef OI_CODE
#error code not present
#endif

exit:

    OI_HIDEndpoint_DumpChannel(chan);
    OI_DBGPRINT2(("-SendFragment: returning %!", status));


    if (!OI_SUCCESS(status)) {
        if (fragBuf) {
            OI_MBUF_Free(fragBuf);
        }
    }

    return status;
}

#ifdef OI_DEBUG
#error code not present
#endif



