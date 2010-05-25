#ifdef FEATURE_BT_EXTPF_CTP
/**
@file  
TCS call control header file
*/

/** \addtogroup Telephony Telephony APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef _TCS_CC_H
#define _TCS_CC_H

#include "oi_debug.h"
#include "oi_stddefs.h"
#include "oi_tcs_spec.h"
#include "oi_tcs.h"
#include "oi_l2cap.h"
#include "oi_dispatch.h"
#include "oi_std_utils.h"

/** The length of the time to wait for receiving a setup message on an
    established L2CAP channel before terminating the channel.  This
    applies to the intercom profile only. */
#define INTERCOM_SETUP_TIMEOUT OI_SECONDS(10)
#define GATEWAY_MAX_CALLS 7

/* Macro to determine is a call data structure is in free. */
#define IsCallFree(call) OI_IS_ZERO_ADDR(&(call)->addr)

/** Transition to a new state. This allows for adding debug info. */
#define SetCallState(c, st) \
    OI_DBGPRINT2(("Call (%:) %s -> %s\n", &(c)->addr, OI_Tcs_CallStateText((c)->state), OI_Tcs_CallStateText((st)))); \
    (c)->state = (st)
#define SetTcsState(c, st) \
    OI_DBGPRINT2(("Connection (%:) %s -> %s", &(c)->addr, TcsStateText((c)->tcsState), TcsStateText((st)))); \
    (c)->tcsState = (st)

typedef enum {
    CH_ST_DISCONNECTED = 0,
    CH_ST_CONNECT_IN_PROGRESS,
    CH_ST_CONNECTED,
    CH_ST_DISCONNECT_IN_PROGRESS
} OI_TCS_CHANNEL_STATE;

typedef enum {
    TCS_ST_DISCONNECTED = 0,
    TCS_ST_INCOMING_CONNECT,
    TCS_ST_OUTGOING_CONNECT,
    TCS_ST_ADD_TO_GROUP,            /**< Only valid for GW when adding TL to connection-less group */
    TCS_ST_CONNECTED,
    TCS_ST_INCOMING_DISCONNECT,
    TCS_ST_OUTGOING_DISCONNECT
} OI_TCS_STATE;

/** The global state for the call. */
typedef struct {

    /** Indicates whether the call is outgoing (TRUE) or incoming (FALSE) from this side. */
    OI_BOOL outgoing;

    /** TRUE if an intercom call. */
    OI_BOOL intercom;

    /** The intercom setup timer handle. */
    DISPATCH_CB_HANDLE intercomSetupTimer;

    /** The handle for the current timer. */
    DISPATCH_CB_HANDLE timerHandle;

    /** The info needed for the current timer. */
    OI_UINT timerType;

    /** The handle for the disconnect completion timeout. */
    DISPATCH_CB_HANDLE disconnectTimeout;

    /** The CID for the call. */
    OI_L2CAP_CID cid;
    
    /** The maximum outgoing MTU for the call. */
    OI_UINT16 outMTU;

    /** The address of the remote device. */
    OI_BD_ADDR addr;
    
    /** The state of the call. */
    OI_UINT8 state;
    
    //The parameters for the initial call establishment

    OI_TCS_BEARER bearer;

    OI_TCS_CALL_CLASS callClass;

    OI_BOOL sendingComplete;

    OI_TCS_SIGNAL signal;
    
    OI_TCS_NUMBER callingNumber;
    
    OI_TCS_NUMBER calledNumber;
    
    OI_TCS_CAUSE cause;
    
    /** The bearer handle */
    union {

        /** The handle for the sco connection. */
        OI_HCI_CONNECTION_HANDLE scoHandle;
        
        /** The handle for the L2CAP connection. */
        OI_L2CAP_CID cid;
        
    } handle;

    /** The state of this call in the profile */
    OI_TCS_STATE tcsState;                  

    /** TRUE while control and connection-less channels are being set up */
    OI_BOOL connectInProgress;              

    /** Status of the connection process */
    OI_STATUS connectResult;                
    
    /** Indicates the state of the control connection */
    OI_TCS_CHANNEL_STATE controlState;      
    
    /** Indicates the state of the bearer connection */
    OI_TCS_CHANNEL_STATE bearerState;       

    /** 
     * Indicates the state of the connection-less connection.  For the TL, this is
     *  always OI_TCS_CONNECTED.  For the GW, this indicates if a call is a member
     *  of the connection-less group. 
     */
    OI_TCS_CHANNEL_STATE clState;

} TCS_CALL_INFO;

typedef struct _TCS_PACKET {

    OI_BYTE *buf;
        
    OI_UINT16 bufLen;

    OI_L2CAP_CID cid;

    OI_UINT8 timerType;

    struct _TCS_PACKET *next;

} TCS_PACKET;

typedef struct {

    /** The call data structures. */
    TCS_CALL_INFO *calls;
    OI_UINT8 numCalls;

    /** The callbacks registered by the application. */
    OI_TCS_CALLBACKS callbacks;

    /** The current role. */
    OI_UINT8 role;

    /** Indicates whether CTP is supported in addition to intercom */
    OI_BOOL supportCtp;

    /** The cid to be used for group traffic. */
    OI_L2CAP_CID groupCID;

    /** Policy for cordless telephony **/
    OI_CONNECT_POLICY tcsConnectPolicy;

    /** The queue for L2CAP packets to be sent. */
    TCS_PACKET *queue;

    /** The handle for the multi-point timer. */
    DISPATCH_CB_HANDLE timerHandle;

    /** The info needed for the multi-point timer. */
    OI_UINT timerType;

} OI_TCS_DATA;

extern OI_TCS_DATA *OI_Tcs_Data;

/* Function declarations */

void OI_Tcs_FreeCall(TCS_CALL_INFO *call);
TCS_CALL_INFO *OI_Tcs_GetCall(OI_BD_ADDR *addr);
TCS_CALL_INFO *OI_Tcs_GetCallByCID(OI_L2CAP_CID cid);
OI_CHAR *OI_Tcs_CallStateText(OI_UINT8 state);
OI_STATUS OI_Tcs_ConnectBearer(TCS_CALL_INFO *call);
OI_STATUS OI_Tcs_DisconnectBearer(TCS_CALL_INFO *call);
OI_BOOL OI_Tcs_ActiveCallExists(void);
OI_BOOL OI_Tcs_CallInProgress(void);
OI_STATUS OI_Tcs_InternalDisconnect(TCS_CALL_INFO *call, OI_STATUS reason);
OI_STATUS OI_Tcs_ClearAllNonSelected(TCS_CALL_INFO *selected);

#endif /* _TCS_CC_H */

/**@}*/
#endif /* FEATURE_BT_EXTPF_CTP */
