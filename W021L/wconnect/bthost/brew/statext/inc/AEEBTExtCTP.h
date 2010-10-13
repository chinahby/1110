#ifndef _AEE_BT_EXT_CTP_H_
#define _AEE_BT_EXT_CTP_H_

/*===========================================================================
FILE:      AEEBTExtCTP.h

SERVICES:  BlueTooth Cordless Telephony Profile

GENERAL DESCRIPTION: BREW interface to the BT Cordless Telephony Profile

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtICP

Copyright (c) 2004-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


// BREW generated ID
#define AEECLSID_BLUETOOTH_CTP     0x0102C52D


// event types returned via HandleEvent()
enum
{
  AEEBT_CTP_EVT_CONNECTED,                // We are now connected to GW
  AEEBT_CTP_EVT_CONNECT_FAILED,           // Connection to GW failed
  AEEBT_CTP_EVT_DISCONNECTED,             // We are now disconnected

  AEEBT_CTP_EVT_REG_DONE,                 // Register done
  AEEBT_CTP_EVT_DEREG_DONE,               // Deregister done

  AEEBT_CTP_EVT_CONN_REQ,                 // CTP GW wants to connect

  AEEBT_CTP_EVT_CALL_SETUP_REQ,           // Call setup request
  AEEBT_CTP_EVT_CALL_SETUP_SUCCESS,       // Call setup succeded
  AEEBT_CTP_EVT_CALL_SETUP_FAILED,        // Call setup failed
  AEEBT_CTP_EVT_CALL_INFO_REQ,            // Call information indication

  AEEBT_CTP_EVT_DTMF_START,               // Start generating DTMF tone
  AEEBT_CTP_EVT_DTMF_STOP,                // Stop generating DTMF tone

  AEEBT_CTP_EVT_CALL_RELEASED,            // Call has been terminated
  AEEBT_CTP_EVT_CALL_ESTABLISHED,         // Outgoing call has been setup
  AEEBT_CTP_EVT_OUTGOING_STATUS           // Status of outgoing call setup
};

// error codes for AEEBT_CTP_EVT_*
enum
{
  AEEBT_CTP_ERR_NONE,
  AEEBT_CTP_ERR_BAD_PARAM,
  AEEBT_CTP_ERR_RESOURCE,
  AEEBT_CTP_ERR_REC_EXISTS,
  AEEBT_CTP_ERR_DB_FULL,
  AEEBT_CTP_ERR_FAILED
};

typedef struct _IBTExtCTP IBTExtCTP;

AEEINTERFACE(IBTExtCTP)
{
   INHERIT_IQueryInterface(IBTExtCTP);

   int   (*Connect)                 (IBTExtCTP*                po, 
                                     const AEEBTBDAddr*        pBDAddr);
   int   (*AcceptConnection)        (IBTExtCTP*                po,
                                     const AEEBTBDAddr*        pBDAddr,
                                     boolean                   bAccept);
   int   (*Disconnect)              (IBTExtCTP*                po);
   int   (*ClearCall)               (IBTExtCTP*                po,
                                     AEEBTCTPCauseType         cause);
   int   (*EstablishCall)           (IBTExtCTP*                po,
                                     const AEEBTCTPNumberType* pCallingNum,
                                     const AEEBTCTPNumberType* pCalledNum);
   int   (*Info)                    (IBTExtCTP*                po,
                                     boolean                   bSendingCmplt,
                                     char                      keypadFacility,
                                     const AEEBTCTPNumberType* pCallingNum,
                                     const AEEBTCTPNumberType* pCalledNum,
                                     AEEBTCTPAudioControlType  audioControl);
   int   (*IncomingStatus)          (IBTExtCTP*                po,
                                     AEEBTCTPCallStatusType    status);
   int   (*AcceptCall)              (IBTExtCTP*                po);
   int   (*StartDTMF)               (IBTExtCTP*                po,
                                     char                      tone);
   int   (*StopDTMF)                (IBTExtCTP*                po);
   int   (*Register)                (IBTExtCTP*                po);
   int   (*Deregister)              (IBTExtCTP*                po);
   int   (*RegisterRecall)          (IBTExtCTP*                po);
};


#define IBTEXTCTP_AddRef(p)                  \
        AEEGETPVTBL((p),IBTExtCTP)->AddRef((p))
        
#define IBTEXTCTP_Release(p)                 \
        AEEGETPVTBL((p),IBTExtCTP)->Release((p))
        
#define IBTEXTCTP_QueryInterface(p,i,ppo)    \
        AEEGETPVTBL((p),IBTExtCTP)->QueryInterface((p),(i),(ppo))
        
#define IBTEXTCTP_Connect(p,a)             \
        AEEGETPVTBL((p),IBTExtCTP)->Connect((p),(a))
        
#define IBTEXTCTP_AcceptConnection(p,a,b)             \
        AEEGETPVTBL((p),IBTExtCTP)->AcceptConnection((p),(a),(b))
        
#define IBTEXTCTP_Disconnect(p)             \
        AEEGETPVTBL((p),IBTExtCTP)->Disconnect((p))
        
#define IBTEXTCTP_ClearCall(p,c)             \
        AEEGETPVTBL((p),IBTExtCTP)->ClearCall((p),(c))
        
#define IBTEXTCTP_EstablishCall(p,pcgn,pcdn)             \
        AEEGETPVTBL((p),IBTExtCTP)->EstablishCall((p),(pcgn),(pcdn))
        
#define IBTEXTCTP_Info(p,c,k,pg,pd,ac)             \
        AEEGETPVTBL((p),IBTExtCTP)->Info((p),(c),(k),(pg),(pd),(ac))
        
#define IBTEXTCTP_IncomingStatus(p,s)             \
        AEEGETPVTBL((p),IBTExtCTP)->IncomingStatus((p),(s))
        
#define IBTEXTCTP_AcceptCall(p)             \
        AEEGETPVTBL((p),IBTExtCTP)->AcceptCall((p))
        
#define IBTEXTCTP_StartDTMF(p,t)             \
        AEEGETPVTBL((p),IBTExtCTP)->StartDTMF((p),(t))

#define IBTEXTCTP_StopDTMF(p)             \
        AEEGETPVTBL((p),IBTExtCTP)->StopDTMF((p))
        
#define IBTEXTCTP_Register(p)             \
        AEEGETPVTBL((p),IBTExtCTP)->Register((p))
        
#define IBTEXTCTP_Deregister(p)             \
        AEEGETPVTBL((p),IBTExtCTP)->Deregister((p))
        
#define IBTEXTCTP_RegisterRecall(p)             \
        AEEGETPVTBL((p),IBTExtCTP)->RegisterRecall((p))
   
/*=====================================================================


=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTEXTCTP

Description: BlueTooth Cordless Telephony Profile

  This interface allows the use of the TG role in the Bluetooth CTP/TCS
  profile.

  The use of IBTEXTCTP consists of sending commands and receiving events.
  These events are delivered to IBTEXTCTP clients as notifications, hence
  the clients must register for CTP notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_CTP.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_CTP_EVT_* events when the notification mask
  is NMASK_BT_CTP.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                                 pData->
  -----                                ----------------------------
  AEEBT_CTP_EVT_CONNECTED             bdAddr - BD address of remote device
  AEEBT_CTP_EVT_CONNECT_FAILED,       bdAddr - BD address of remote device  
  AEEBT_CTP_EVT_DISCONNECTED          bdAddr - BD address of remote device

  AEEBT_CTP_EVT_REG_DONE,             uError   
  AEEBT_CTP_EVT_DEREG_DONE,           uError

  AEEBT_CTP_EVT_CONN_REQ,             bdAddr

  AEEBT_CTP_EVT_CALL_SETUP_REQ,       bdAddr
  AEEBT_CTP_EVT_CALL_SETUP_SUCCESS,   bdAddr
  AEEBT_CTP_EVT_CALL_SETUP_FAILED,    bdAddr  
  AEEBT_CTP_EVT_CALL_INFO_REQ,        bdAddr, CtpObject

  AEEBT_CTP_EVT_DTMF_START,           bdAddr, CtpObject
  AEEBT_CTP_EVT_DTMF_STOP,            bdAddr

  AEEBT_CTP_EVT_CALL_RELEASED,        bdAddr, CtpObject
  AEEBT_CTP_EVT_OUTGOING_STATUS,      bdAddr, CtpObject
  
  AEEBT_CTP_EVT_CONNECTED             bdAddr
  AEEBT_CTP_EVT_DISCONNECTED          bdAddr

Interfaces:

  IBTEXTCTP provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTCTP_AddRef()
    IBTEXTCTP_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTCTP_QueryInterface()

=======================================================================

Function:  IBTEXTCTP_Connect()

Description: 
   Initiates the connection to CTP Gateway specified by pBDAddr.

Prototype:
   int IBTEXTCTP_Connect( IBTExtCTP*          po, 
                          const AEEBTBDAddr*  pBDAddr);

Parameters:
   po           - pointer to the IBTEXTCTP object
   pBDAddr      - pointer to the BT device address of CTP Gateway

Return value:
  SUCCESS   - the request to connect to CTP Gateway was queued and that
              AEEBT_CTP_EVT_CONNECTED or AEEBT_CTP_EVT_CONNECT_FAILED 
              event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  AEEBT_CTP_EVT_CONNECTED or
  AEEBT_CTP_EVT_CONNECT_FAILED

=======================================================================

Function:  IBTEXTCTP_AcceptConnection()

Description:
  Issue this command to accept or reject connection from CTP Gateway.

Prototype:
  int IBTEXTCTP_AcceptConnection( IBTExtCTP*          po,
                                  const AEEBTBDAddr*  pBDAddr,
                                  boolean             bAccept);

Parameters:
  po                    - pointer to the IBTExtCTP object
  pBDAddr               - pointer to BT device address of CTP Gateway
  bAccept               - TRUE => accept connection, else reject

Return value:
  SUCCESS   - the request to accept or reject the connection request 
              was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  None.

=======================================================================

Function:  IBTEXTCTP_Disconnect()

Description:
  Disconnect from the CTP gateway 

Prototype:
  int IBTEXTCTP_Disconnect( IBTExtCTP*          po );

Parameters:
  po                    - pointer to the IBTExtCTP object

Return value:
  SUCCESS   - the request to disconnect from CTP gateway was queued and that
              AEEBT_CTP_EVT_DISCONNECTED event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_CTP_EVT_DISCONNECTED with the one of the error codes:
    AEEBT_CTP_ERR_NONE
    AEEBT_CTP_ERR_NO_CONN

=======================================================================

Function:  IBTEXTCTP_ClearCall()

Description:
  Release (terminate) current call or reject incoming call from
  CTP gateway

Prototype:
  int IBTEXTCTP_ClearCall( IBTExtCTP*          po,
                           AEEBTCTPCauseType   cause );

Parameters:
  po                    - pointer to the IBTExtCTP object
  cause                 - release reason

Return value:
  SUCCESS   - the request to terminate call was queued and that
              AEEBT_CTP_EVT_CALL_RELEASED event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_CTP_EVT_CALL_RELEASE_IND

=======================================================================

Function:  IBTEXTCTP_EstablishCall()

Description:
  Setup an outgoing call 

Prototype:
  int IBTEXTCTP_EstablishCall( IBTExtCTP*                po,
                               const AEEBTCTPNumberType* pCallingNum,
                               const AEEBTCTPNumberType* pCalledNum );

Parameters:
  po                    - pointer to the IBTExtCTP object
  pCallingNum           - calling party number
  pCalledNum            - called party number

Return value:
  SUCCESS   - the request to establish call was queued and that
              AEEBT_CTP_EVT_CALL_ESTABLISHED event will be generated at a 
              later time.  Moreover, one or more 
              AEEBT_CTP_EVT_OUTGOING_STATUS event may be generated prior
              to AEE_CTP_EVT_CALL_ESTABLISHED to indicate status of
              call establishment on remote device.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_CTP_EVT_CALL_ESTABLISHED
  AEEBT_CTP_EVT_OUTGOING_STATUS

=======================================================================

Function:  IBTEXTCTP_Info()

Description:
  Send additional information needed to establish a call through the
  CTP gateway.  This API must be called only after EstablishCall() 
  has been called and in response to AEEBT_CTP_EVT_CALL_INFO_REQ event.

Prototype:
  int IBTEXTCTP_Info( IBTExtCTP*                po,
                      boolean                   bSendingCmplt,
                      char                      keypadFacility,
                      const AEEBTCTPNumberType* pCallingNum,
                      const AEEBTCTPNumberType* pCalledNum,
                      AEEBTCTPAudioControlType  audioControl );

Parameters:
  po                    - pointer to the IBTExtCTP object
  keypadFacility        - bluetooth character set
                          0x16 = register recall
                          0x05 = dialling pause   
  pCallingNum           - calling party number
  pCalledNum            - called party number
  audioControl          - command for increasing/decreasing 
                          volume/mic gain
                          

Return value:
  SUCCESS   - the request to send call info was queued.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  None

=======================================================================

Function:  IBTEXTCTP_IncomingStatus()

Description:
  Report status of an incoming call establishment.  Use is optional
  but can be called repeatedly after receiving AEEBT_CTP_EVT_CALL_SETUP_REQ
  event and before issuing AcceptCall().

Prototype:
  int IBTEXTCTP_IncomingStatus( IBTExtCTP*             po,
                                AEEBTCTPCallStatusType status );

Parameters:
  po                    - pointer to the IBTExtCTP object
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

Function:  IBTEXTCTP_AcceptCall()

Description:
  Accept incoming call.  Optionally, before calling this command, the
  application can call IncomingStatus() to report call establishment
  status.

Prototype:
  int IBTEXTCTP_AcceptCall( IBTExtCTP* po );

Parameters:
  po                    - pointer to the IBTExtCTP object

Return value:
  SUCCESS   - the request to accept call has been queued.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  None

=======================================================================

Function:  IBTEXTCTP_StartDTMF()

Description:
  Start transmission of DTMF tone.  Should be called only after a call
  has been established.

Prototype:
  int IBTEXTCTP_StartDTMF( IBTExtCTP* po,
                           char       tone );

Parameters:
  po                    - pointer to the IBTExtCTP object
  tone                  - character on the keypad representing the tone

Return value:
  SUCCESS   - the request to start DTMF tone transmission has been queued.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  None

=======================================================================

Function:  IBTEXTCTP_StopDTMF()

Description:
  Stop DTMF tone transmission. 
  
Prototype:
  int IBTEXTCTP_StopDTMF( IBTExtCTP* po );

Parameters:
  po                    - pointer to the IBTExtCTP object

Return value:
  SUCCESS   - the request to stop DTMF tone transmission has been queued.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  None

=======================================================================

Function:  IBTEXTCTP_Register()

Description:
  Initializes the CTP terminal on the local BT device.  The local 
  service discovery database will be updated with the new service record.
  
Prototype:
  int IBTEXTCTP_Register( IBTExtCTP* po );

Parameters:
  po                    - pointer to the IBTExtCTP object

Return value:
  SUCCESS   - the request to register has been queued and
              AEEBT_CTP_EVT_REG_DONE will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  None

=======================================================================

Function:  IBTEXTCTP_Deregister()

Description:
  Terminates CTP terminal.  Service record will be removed form the
  local service discovery database.
  
Prototype:
  int IBTEXTCTP_Deregister( IBTExtCTP* po );

Parameters:
  po                    - pointer to the IBTExtCTP object

Return value:
  SUCCESS   - the request to deregister has been queued and
              AEEBT_CTP_EVT_DEREG_DONE will be generated at a later
              time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  None

=======================================================================

Function:  IBTEXTCTP_RegisterRecall()

Description:
  Sends a register recall (hook flash) signal to the CTP gateway.
  Should be called only after a call has been established.
  
Prototype:
  int IBTEXTCTP_RegisterRecall( IBTExtCTP* po );

Parameters:
  po                    - pointer to the IBTExtCTP object

Return value:
  SUCCESS   - the request to register recall has been queued.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  None

=======================================================================

=======================================================================*/
#endif /* _AEE_BT_EXT_CTP_H_ */
