#ifndef _AEE_BT_EXT_ICP_H_
#define _AEE_BT_EXT_ICP_H_

/*===========================================================================
FILE:      AEEBTExtICP.h

SERVICES:  BlueTooth Intercom Profile

GENERAL DESCRIPTION: BREW interface to the BT Intercom Profile

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtICP

Copyright (c) 2004-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

// BREW generated ID
#define AEECLSID_BLUETOOTH_ICP     0x0102C52E

// ICP AEE Event base (since ICP and CTP share the same event notification
// mask)
#define AEEBT_ICP_EVT_BASE         0x0100


// event types returned via HandleEvent()
enum
{
  AEEBT_ICP_EVT_CONNECTED = AEEBT_ICP_EVT_BASE, 
                                          // We are now connected to GW
  AEEBT_ICP_EVT_CONNECT_FAILED,           // Connection to GW failed
  AEEBT_ICP_EVT_DISCONNECTED,             // We are now disconnected

  AEEBT_ICP_EVT_REG_DONE,                 // Register done
  AEEBT_ICP_EVT_DEREG_DONE,               // Deregister done

  AEEBT_ICP_EVT_CONN_REQ,                 // ICP GW wants to connect

  AEEBT_ICP_EVT_CALL_SETUP_REQ,           // Call setup request
  AEEBT_ICP_EVT_CALL_SETUP_SUCCESS,       // Call setup succeded
  AEEBT_ICP_EVT_CALL_SETUP_FAILED,        // Call setup failed
  AEEBT_ICP_EVT_CALL_INFO_REQ,            // Call information indication

  AEEBT_ICP_EVT_CALL_RELEASED,            // Call has been terminated
  AEEBT_ICP_EVT_CALL_ESTABLISHED,         // Outgoing call has been setup
  AEEBT_ICP_EVT_OUTGOING_STATUS           // Status of outgoing call setup
};

// error codes for AEEBT_ICP_EVT_*
enum
{
  AEEBT_ICP_ERR_NONE,
  AEEBT_ICP_ERR_BAD_PARAM,
  AEEBT_ICP_ERR_RESOURCE,
  AEEBT_ICP_ERR_REC_EXISTS,
  AEEBT_ICP_ERR_DB_FULL,
  AEEBT_ICP_ERR_FAILED
};

typedef struct _IBTExtICP IBTExtICP;

AEEINTERFACE(IBTExtICP)
{
   INHERIT_IQueryInterface(IBTExtICP);

   int   (*Connect)                 (IBTExtICP*                po, 
                                     const AEEBTBDAddr*        pBDAddr);
   int   (*AcceptConnection)        (IBTExtICP*                po,
                                     const AEEBTBDAddr*        pBDAddr,
                                     boolean                   bAccept);
   int   (*Disconnect)              (IBTExtICP*                po);
   int   (*ClearCall)               (IBTExtICP*                po,
                                     AEEBTICPCauseType         cause);
   int   (*EstablishCall)           (IBTExtICP*                po,
                                     const AEEBTICPNumberType* pCallingNum,
                                     const AEEBTICPNumberType* pCalledNum);
   int   (*IncomingStatus)          (IBTExtICP*                po,
                                     AEEBTICPCallStatusType    status);
   int   (*AcceptCall)              (IBTExtICP*                po);
   int   (*Register)                (IBTExtICP*                po);
   int   (*Deregister)              (IBTExtICP*                po);
};


#define IBTEXTICP_AddRef(p)                  \
        AEEGETPVTBL((p),IBTExtICP)->AddRef((p))
        
#define IBTEXTICP_Release(p)                 \
        AEEGETPVTBL((p),IBTExtICP)->Release((p))
        
#define IBTEXTICP_QueryInterface(p,i,ppo)    \
        AEEGETPVTBL((p),IBTExtICP)->QueryInterface((p),(i),(ppo))
        
#define IBTEXTICP_Connect(p,a)             \
        AEEGETPVTBL((p),IBTExtICP)->Connect((p),(a))
        
#define IBTEXTICP_AcceptConnection(p,a,b)             \
        AEEGETPVTBL((p),IBTExtICP)->AcceptConnection((p),(a),(b))
        
#define IBTEXTICP_Disconnect(p)             \
        AEEGETPVTBL((p),IBTExtICP)->Disconnect((p))
        
#define IBTEXTICP_ClearCall(p,c)             \
        AEEGETPVTBL((p),IBTExtICP)->ClearCall((p),(c))
        
#define IBTEXTICP_EstablishCall(p,pcgn,pcdn)             \
        AEEGETPVTBL((p),IBTExtICP)->EstablishCall((p),(pcgn),(pcdn))
        
#define IBTEXTICP_IncomingStatus(p,s)             \
        AEEGETPVTBL((p),IBTExtICP)->IncomingStatus((p),(s))
        
#define IBTEXTICP_AcceptCall(p)             \
        AEEGETPVTBL((p),IBTExtICP)->AcceptCall((p))
        
#define IBTEXTICP_Register(p)             \
        AEEGETPVTBL((p),IBTExtICP)->Register((p))
        
#define IBTEXTICP_Deregister(p)             \
        AEEGETPVTBL((p),IBTExtICP)->Deregister((p))
   
/*=====================================================================


=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTEXTICP

Description: BlueTooth Intercom Profile

  The use of IBTEXTICP consists of sending commands and receiving events.
  These events are delivered to IBTEXTICP clients as notifications, hence
  the clients must register for ICP notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_ICP.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_ICP_EVT_* events when the notification mask
  is NMASK_BT_ICP.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                                 pData->
  -----                                ----------------------------
  AEEBT_ICP_EVT_CONNECTED             bdAddr - BD address of remote device
  AEEBT_ICP_EVT_CONNECT_FAILED,       bdAddr - BD address of remote device  
  AEEBT_ICP_EVT_DISCONNECTED          bdAddr - BD address of remote device

  AEEBT_ICP_EVT_REG_DONE,             uError   
  AEEBT_ICP_EVT_DEREG_DONE,           uError

  AEEBT_ICP_EVT_CONN_REQ,             bdAddr

  AEEBT_ICP_EVT_CALL_SETUP_REQ,       bdAddr
  AEEBT_ICP_EVT_CALL_SETUP_SUCCESS,   bdAddr
  AEEBT_ICP_EVT_CALL_SETUP_FAILED,    bdAddr  
  AEEBT_ICP_EVT_CALL_INFO_REQ,        bdAddr, IcpObject

  AEEBT_ICP_EVT_CALL_RELEASED,        bdAddr, IcpObject
  AEEBT_ICP_EVT_OUTGOING_STATUS,      bdAddr, IcpObject
  
  AEEBT_ICP_EVT_CONNECTED             bdAddr
  AEEBT_ICP_EVT_DISCONNECTED          bdAddr

Interfaces:

  IBTEXTICP provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTICP_AddRef()
    IBTEXTICP_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTICP_QueryInterface()

=======================================================================

Function:  IBTEXTICP_Connect()

Description: 
   Initiates the connection to ICP Gateway specified by pBDAddr.

Prototype:
   int IBTEXTICP_Connect( IBTExtICP*          po, 
                          const AEEBTBDAddr*  pBDAddr);

Parameters:
   po           - pointer to the IBTEXTICP object
   pAddr        - pointer to the BT device address of ICP Gateway

Return value:
  SUCCESS   - the request to connect to ICP Gateway was queued and that
              AEEBT_ICP_EVT_CONNECTED or AEEBT_ICP_EVT_CONNECT_FAILED 
              event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  AEEBT_ICP_EVT_CONNECTED or
  AEEBT_ICP_EVT_CONNECT_FAILED

=======================================================================

Function:  IBTEXTICP_AcceptConnection()

Description:
  Issue this command to accept or reject connection from ICP Gateway.

Prototype:
  int IBTEXTICP_AcceptConnection( IBTExtICP*          po,
                                  const AEEBTBDAddr*  pBDAddr,
                                  boolean             bAccept);

Parameters:
  po                    - pointer to the IBTExtICP object
  pBDAddr               - pointer to BT device address of ICP Gateway
  bAccept               - TRUE => accept connection, else reject

Return value:
  SUCCESS   - the request to accept or reject the connection request 
              was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  None.

=======================================================================

Function:  IBTEXTICP_Disconnect()

Description:
  Disconnect from the remote ICP device.

Prototype:
  int IBTEXTICP_Disconnect( IBTExtICP* po );

Parameters:
  po                    - pointer to the IBTExtICP object

Return value:
  SUCCESS   - the request to disconnect from ICP gateway was queued and that
              AEEBT_ICP_EVT_DISCONNECTED event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_ICP_EVT_DISCONNECTED with the one of the error codes:
    AEEBT_ICP_ERR_NONE
    AEEBT_ICP_ERR_NO_CONN

=======================================================================

Function:  IBTEXTICP_ClearCall()

Description:
  Release (terminate) current call or reject incoming call from
  ICP gateway

Prototype:
  int IBTEXTICP_ClearCall( IBTExtICP*        po,
                           AEEBTICPCauseType cause );

Parameters:
  po                    - pointer to the IBTExtICP object
  cause                 - release reason

Return value:
  SUCCESS   - the request to terminate call was queued and that
              AEEBT_ICP_EVT_CALL_RELEASED event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_ICP_EVT_CALL_RELEASE_IND

=======================================================================

Function:  IBTEXTICP_EstablishCall()

Description:
  Setup an outgoing call 

Prototype:
  int IBTEXTICP_EstablishCall( IBTExtICP*                po,
                               const AEEBTICPNumberType* pCallingNum,
                               const AEEBTICPNumberType* pCalledNum );

Parameters:
  po                    - pointer to the IBTExtICP object
  pCallingNum           - calling party number
  pCalledNum            - called party number

Return value:
  SUCCESS   - the request to establish call was queued and that
              AEEBT_ICP_EVT_CALL_ESTABLISHED event will be generated at a 
              later time.  Moreover, one or more 
              AEEBT_ICP_EVT_OUTGOING_STATUS event may be generated prior
              to AEE_ICP_EVT_CALL_ESTABLISHED to indicate status of
              call establishment on remote device.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_ICP_EVT_CALL_ESTABLISHED
  AEEBT_ICP_EVT_OUTGOING_STATUS

=======================================================================

Function:  IBTEXTICP_IncomingStatus()

Description:
  Report status of an incoming call establishment.  Use is optional
  but can be called repeatedly after receiving AEEBT_ICP_EVT_CALL_SETUP_REQ
  event and before issuing AcceptCall().

Prototype:
  int IBTEXTICP_IncomingStatus( IBTExtICP*             po,
                                AEEBTICPCallStatusType status );

Parameters:
  po                    - pointer to the IBTExtICP object
  status                - incoming call status.  The value must be passed
                          in accending order in successive IncomingStatus
                          call. e.g, CALL_STATUS STATUS_PROCEEDING must not 
                          be sent after CALL_STATUS_ALERTING has been passed

Return value:
  SUCCESS   - the request to send call status has been queued.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  None

=======================================================================

=======================================================================

Function:  IBTEXTICP_AcceptCall()

Description:
  Accept incoming call.  Optionally, before calling this command, the
  application can call IncomingStatus() to report call establishment
  status.

Prototype:
  int IBTEXTICP_AcceptCall( IBTExtICP* po );

Parameters:
  po                    - pointer to the IBTExtICP object

Return value:
  SUCCESS   - the request to accept call has been queued.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  None
  
=======================================================================

Function:  IBTEXTICP_Register()

Description:
  Initializes the ICP terminal on the local BT device.  The local 
  service discovery database will be updated with the new service record.
  
Prototype:
  int IBTEXTICP_Register( IBTExtICP* po );

Parameters:
  po                    - pointer to the IBTExtICP object

Return value:
  SUCCESS   - the request to register has been queued and
              AEEBT_ICP_EVT_REG_DONE will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  None

=======================================================================

Function:  IBTEXTICP_Deregister()

Description:
  Terminates ICP terminal.  Service record will be removed form the
  local service discovery database.
  
Prototype:
  int IBTEXTICP_Deregister( IBTExtICP* po );

Parameters:
  po                    - pointer to the IBTExtICP object

Return value:
  SUCCESS   - the request to deregister has been queued and
              AEEBT_ICP_EVT_DEREG_DONE will be generated at a later
              time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  None

=======================================================================

=======================================================================*/
#endif /* _AEE_BT_EXT_ICP_H_ */
