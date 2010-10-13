#ifndef _AEE_BT_EXT_SAP_H_
#define _AEE_BT_EXT_SAP_H_

/*===========================================================================
FILE:      AEEBTExtSAP.h

SERVICES:  BlueTooth SIM Access Profile Interface

GENERAL DESCRIPTION: BREW interface to BT SAP

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtSAP

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

// BREW generated ID
#define AEECLSID_BLUETOOTH_SAP    	0x0102c52a

// event types returned via HandleEvent()
enum
{
  /* for servers */
  AEEBT_SAP_EVT_REG_DONE,               // registration process done
  AEEBT_SAP_EVT_DEREG_DONE,             // deregistration process done
  AEEBT_SAP_EVT_CONN_REQ,               // a client wants to connect
  AEEBT_SAP_EVT_CONN_CFM,               // result of accepting connection req
  AEEBT_SAP_EVT_DISC_CFM,               // result of disconnecting
  AEEBT_SAP_EVT_RSP_SENT,               // response/status sent to client
  AEEBT_SAP_EVT_TX_APDU_REQ,            // client wants to send cmd-APDU to SIM
  AEEBT_SAP_EVT_TX_ATR_REQ,             // client wants ATR from SIM
  AEEBT_SAP_EVT_PWR_SIM_OFF_REQ,        // client wants to power off the SIM
  AEEBT_SAP_EVT_PWR_SIM_ON_REQ,         // client wants to power on the SIM
  AEEBT_SAP_EVT_RESET_SIM_REQ,          // client wants reset the SIM
  AEEBT_SAP_EVT_CARD_READER_STATUS_REQ, // client wants status of card reader

  /* for clients */
  AEEBT_SAP_EVT_CONN_RSP,               // result of connection req
  AEEBT_SAP_EVT_DISC_RSP,               // result of disconnection req
  AEEBT_SAP_EVT_DISC_IND,               // server initiating disconnection
  AEEBT_SAP_EVT_REQ_SENT,               // request has been sent to server
  AEEBT_SAP_EVT_TX_APDU_RSP,            // rsp-APDU from SIM
  AEEBT_SAP_EVT_TX_ATR_RSP,             // ATR from SIM is available
  AEEBT_SAP_EVT_PWR_SIM_OFF_RSP,        // result of powering off the SIM
  AEEBT_SAP_EVT_PWR_SIM_ON_RSP,         // result of powering on the SIM
  AEEBT_SAP_EVT_RESET_SIM_RSP,          // result of resetting the SIM
  AEEBT_SAP_EVT_CARD_READER_STATUS,     // status of card reader is received
  AEEBT_SAP_EVT_SIM_STATUS,             // status of the SIM is received
  AEEBT_SAP_EVT_ERR_RSP                 // request sent to server was malformed

};

/* SAP Connection Status - SAP specs, rev 0.95c, section 5.3.2 */
/*
enum
{
  AEEBT_SAP_CONN_ERR_NONE,
  AEEBT_SAP_CONN_ERR_FAILED,
  AEEBT_SAP_CONN_ERR_MAX_MSG_SIZE_UNSUPPORTED,
  AEEBT_SAP_CONN_ERR_MAX_MSG_SIZE_TOO_SMALL,
  AEEBT_SAP_CONN_ERR_MAX
};
typedef uint8 AEEBTSAPConnStatus;
*/

enum
{
  AEEBT_PF_SAP_RESULT_ERR_NONE,
  AEEBT_PF_SAP_RESULT_ERR_NO_REASON,
  AEEBT_PF_SAP_RESULT_ERR_CARD_INACCESSIBLE,
  AEEBT_PF_SAP_RESULT_ERR_CARD_ALREADY_OFF,
  AEEBT_PF_SAP_RESULT_ERR_CARD_REMOVED,
  AEEBT_PF_SAP_RESULT_ERR_CARD_ALREADY_ON,
  AEEBT_PF_SAP_RESULT_ERR_DATA_UNAVAILABLE,
  AEEBT_PF_SAP_RESULT_ERR_MAX
};
typedef uint8 AEEBTSAPResult;


enum 
{
  AEEBT_SAP_STAT_CHG_UNKNOWN_ERROR,
  AEEBT_SAP_STAT_CHG_CARD_RESET,
  AEEBT_SAP_STAT_CHG_CARD_INACCESSIBLE,
  AEEBT_SAP_STAT_CHG_CARD_REMOVED,
  AEEBT_SAP_STAT_CHG_CARD_INSERTED,
  AEEBT_SAP_STAT_CHG_CARD_RECOVERED,
  AEEBT_SAP_STAT_CHG_MAX
};
typedef uint8 AEEBTSAPStatusChange;


typedef struct _IBTExtSAP IBTExtSAP;

AEEINTERFACE(IBTExtSAP)
{
  INHERIT_IQueryInterface(IBTExtSAP);

  /* for servers */
  int (*Register)             (IBTExtSAP* po, const char* szServiceName,
                               uint16 uMinFrameSize, uint16 uMaxFrameSize,
                               uint8* pRxMsg, uint16* pRxMsgLen);
  int (*Deregister)           (IBTExtSAP* po);
  int (*AcceptConnection)     (IBTExtSAP* po, boolean bAccept);
  int (*SendAPDUFromSIM)      (IBTExtSAP* po, AEEBTSAPResult result,
                               const uint8* pRsp, uint16 uRspLen);
  int (*SendATRFromSIM)       (IBTExtSAP* po, AEEBTSAPResult result,
                               const uint8* pRsp, uint16 uRspLen);
  int (*PowerSIMOffRespond)   (IBTExtSAP* po, AEEBTSAPResult result);
  int (*PowerSIMOnRespond)    (IBTExtSAP* po, AEEBTSAPResult result);
  int (*ResetSIMRespond)      (IBTExtSAP* po, AEEBTSAPResult result);
  int (*SendCardReaderStatus) (IBTExtSAP* po, AEEBTSAPResult result, 
                               uint8 status);
  int (*ReportSIMStatus)      (IBTExtSAP* po, AEEBTSAPStatusChange status);

  /* for clients */
  int (*Connect)              (IBTExtSAP* po, const AEEBTBDAddr* pServerBDAddr, 
                               uint8 uChannelNumber,
                               uint16 uMinFrameSize, uint16 uMaxFrameSize,
                               uint8* pRxMsg, uint16* pRxMsgLen);
  int (*SendAPDUToSIM)        (IBTExtSAP* po, 
                               const uint8* pReq, uint16 uReqLen);
  int (*GetATRFromSIM)        (IBTExtSAP* po);
  int (*PowerSIMOff)          (IBTExtSAP* po);
  int (*PowerSIMOn)           (IBTExtSAP* po);
  int (*ResetSIM)             (IBTExtSAP* po);
  int (*GetCardReaderStatus)  (IBTExtSAP* po);

  /* common to servers and clients */
  int (*Disconnect)           (IBTExtSAP* po, boolean bSrvConn, 
                               boolean bImmediately);
};


#define IBTEXTSAP_AddRef(p)                               \
        AEEGETPVTBL((p),IBTExtSAP)->AddRef((p))

#define IBTEXTSAP_Release(p)                              \
        AEEGETPVTBL((p),IBTExtSAP)->Release((p))

#define IBTEXTSAP_QueryInterface(p,i,ppo)                 \
        AEEGETPVTBL((p),IBTExtSAP)->QueryInterface((p),(i),(ppo))

#define IBTEXTSAP_Register(p,pn,mn,mx,pd,pl)              \
        AEEGETPVTBL((p),IBTExtSAP)->Register((p),(pn),(mn),(mx),(pd),(pl))

#define IBTEXTSAP_Deregister(p)                           \
        AEEGETPVTBL((p),IBTExtSAP)->Deregister((p))

#define IBTEXTSAP_AcceptConnection(p,b)                   \
        AEEGETPVTBL((p),IBTExtSAP)->AcceptConnection((p),(b))

#define IBTEXTSAP_Connect(p,pa,c,mn,mx,pd,pl)             \
        AEEGETPVTBL((p),IBTExtSAP)->Connect((p),(pa),(c),(mn),(mx),(pd),(pl))

#define IBTEXTSAP_Disconnect(p,b1,b2)                     \
        AEEGETPVTBL((p),IBTExtSAP)->Disconnect((p),(b1),(b2))

#define IBTEXTSAP_SendAPDUToSIM(p,pr,l)                   \
        AEEGETPVTBL((p),IBTExtSAP)->SendAPDUToSIM((p),(pr),(l))

#define IBTEXTSAP_SendAPDUFromSIM(p,r,pr,l)               \
        AEEGETPVTBL((p),IBTExtSAP)->SendAPDUFromSIM((p),(r),(pr),(l))

#define IBTEXTSAP_GetATRFromSIM(p)                        \
        AEEGETPVTBL((p),IBTExtSAP)->GetATRFromSIM((p))

#define IBTEXTSAP_SendATRFromSIM(p,r,pr,l)                \
        AEEGETPVTBL((p),IBTExtSAP)->SendATRFromSIM((p),(r),(pr),(l))

#define IBTEXTSAP_PowerSIMOff(p)                          \
        AEEGETPVTBL((p),IBTExtSAP)->PowerSIMOff((p))

#define IBTEXTSAP_PowerSIMOffRespond(p,r)                 \
        AEEGETPVTBL((p),IBTExtSAP)->PowerSIMOffRespond((p),(r))

#define IBTEXTSAP_PowerSIMOn(p)                           \
        AEEGETPVTBL((p),IBTExtSAP)->PowerSIMOn((p))

#define IBTEXTSAP_PowerSIMOnRespond(p,r)                  \
        AEEGETPVTBL((p),IBTExtSAP)->PowerSIMOnRespond((p),(r))

#define IBTEXTSAP_ResetSIM(p)                             \
        AEEGETPVTBL((p),IBTExtSAP)->ResetSIM((p))

#define IBTEXTSAP_ResetSIMRespond(p,r)                    \
        AEEGETPVTBL((p),IBTExtSAP)->ResetSIMRespond((p),(r))

#define IBTEXTSAP_ReportSIMStatus(p,s)                    \
        AEEGETPVTBL((p),IBTExtSAP)->ReportSIMStatus((p),(s))

#define IBTEXTSAP_GetCardReaderStatus(p)                  \
        AEEGETPVTBL((p),IBTExtSAP)->GetCardReaderStatus((p))

#define IBTEXTSAP_SendCardReaderStatus(p,r,s)             \
        AEEGETPVTBL((p),IBTExtSAP)->SendCardReaderStatus((p),(r),(s))

/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtSAP

Description: BlueTooth SIM Access Profile

  Consult the Bluetooth ISOD for description of SAP.
  
  The use of IBTExtSAP consists of sending commands and receiving events.  
  These events are delivered to IBTExtSAP clients as notifications, hence
  the clients must register for SAP notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_SAP.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.  However, not all events are associated with 
  a command.  Some events are asyncronously generated such as a pin request
  sent by remote device.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_SAP_EVT_* events when the notification mask
  is NMASK_BT_SAP.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                                 pData->
  -----                                 ----------------------------
  *** for servers ***
  AEEBT_SAP_EVT_REG_DONE                uError
  AEEBT_SAP_EVT_DEREG_DONE              uError
  AEEBT_SAP_EVT_CONN_REQ                bdAddr - of remote device
  AEEBT_SAP_EVT_TX_APDU_REQ             
  AEEBT_SAP_EVT_TX_ATR_REQ              
  AEEBT_SAP_EVT_PWR_SIM_OFF_REQ         
  AEEBT_SAP_EVT_PWR_SIM_ON_REQ          
  AEEBT_SAP_EVT_RESET_SIM_REQ           
  AEEBT_SAP_EVT_CARD_READER_STATUS_REQ  

  *** for clients ***
  AEEBT_SAP_EVT_TX_APDU_RSP             
  AEEBT_SAP_EVT_TX_ATR_RSP              
  AEEBT_SAP_EVT_PWR_SIM_OFF_RSP         
  AEEBT_SAP_EVT_PWR_SIM_ON_RSP          
  AEEBT_SAP_EVT_RESET_SIM_RSP           
  AEEBT_SAP_EVT_CARD_READER_STATUS      
  AEEBT_SAP_EVT_SIM_STATUS              
  AEEBT_SAP_EVT_ERR_RSP                 

  *** common to servers and clients ***
  AEEBT_SAP_EVT_CONNECTED               bdAddr - of remote device
  AEEBT_SAP_EVT_CONN_FAILED             bdAddr - of remote device
  AEEBT_SAP_EVT_DISCONNECTED            none
  
Interfaces:

  IBTEXTSAP provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTSAP_AddRef()
    IBTEXTSAP_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTSAP_QueryInterface()

=======================================================================

Function:  IBTEXTSAP_Register()

Description:
  Server application uses this command to initialize the SAP server and 
  register it in the service discovery database (SDDB.)

Prototype:
  int IBTEXTSAP_Register( IBTExtSAP* po, const char* szServiceName,
                          uint16 uMinFrameSize, uint16 uMaxFrameSize,
                          uint8* pRxMsg, uint16* pRxMsgLen );

Parameters:
  po            - pointer to the IBTExtSAP object
  szServiceName - service name to store in SDDB; must not be NULL
  uMinFrameSize - the minimum message size to be used
  uMaxFrameSize - the maximum message size to be used
  pRxMsg        - destination for request data from client
  pRxMsgLen     - destination for length of request data

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s) or (min > max)
  EBADSTATE - already registered

Event:
  AEEBT_SAP_EVT_REG_DONE with accompanying error code of one of the
  following values:
    AEEBT_SAP_ERR_NONE
    AEEBT_SAP_ERR_REC_EXISTS
    
=======================================================================

Function:  IBTEXTSAP_Deregister()

Description: 
  Terminates the SIM Access server and deregisters it from the SDDB.

Prototype:
  int IBTEXTSAP_Deregister( IBTExtSAP* po );

Parameters:
  po           - pointer to the IBTExtSAP object

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - not registered

Event:
  AEEBT_SAP_EVT_DEREG_DONE with accompanying error code of one of the
  following values:
    AEEBT_SAP_ERR_NONE
    AEEBT_SAP_ERR_REC_NOT_FOUND

=======================================================================

Function:  IBTEXTSAP_AcceptConnection()

Description: 
  The server application issues this command to accept or reject a 
  connection request from a client.

Prototype:
  int IBTEXTSAP_AcceptConnection( IBTExtSAP* po, boolean bAccept );

Parameters:
  po         - pointer to the IBTEXTSAP object
  bAccept    - TRUE if server accepts connection request

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)

Event:
  AEEBT_SAP_EVT_CONNECTED with the accompanying error code AEEBT_SAP_ERR_NONE.

=======================================================================

Function:  IBTEXTSAP_Connect()

Description:
  Called by client application to connect to remote SAP server.

Prototype:

  int IBTEXTSAP_Connect( IBTExtSAP* po, const AEEBTBDAddr* pServerBDAddr, 
                         uint8 uChannelNumber, uint16 uMinFrameSize, 
                         uint16 uMaxFrameSize, uint8* pRxMsg, 
                         uint16* pRxMsgLen );

Parameters:
  po              - pointer to the IBTExtSAP object
  pServerBDAddr   - BD address of SAP server
  uChannelNumber  - the server channel number to use to establish connection
  uMinFrameSize   - the minimum message size to be used
  uMaxFrameSize   - the maximum message size to be used
  pRxMsg          - destination for responses/status from server
  pRxMsgLen       - destination for length of request responses/status

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or invalid channel number

Event:
  AEEBT_SAP_EVT_CONNECTED or
  AEEBT_SAP_EVT_CONN_FAILED

======================================================================

Function:  IBTEXTSAP_Disconnect()

Description:
  Either server or client application can use this command to bring down the
  existing connection between them.

Prototype:
  int IBTEXTSAP_Disconnect( IBTExtSAP* po, boolean bServerConn, 
                            boolean bImmediately );

Parameters:
  po            - pointer to the IBTExtSAP object
  bServerConn   - TRUE indicates bringing down the server connection
                  FALSE the client connection
  bImmediately  - TRUE if server wants client to disconnect immediately,
                  FALSE if APDUs exchange is allowed to finish first;
                  unused for client

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection

Event:
  AEEBT_SAP_EVT_DISCONNECTED with the one of the error codes:
    AEEBT_SAP_ERR_NONE
    AEEBT_SAP_ERR_NO_CONN

=======================================================================

Function:  IBTEXTSAP_SendAPDUToSIM()

Description:
  Client application issues this command to request server to transfer
  command APDU to the SIM on the server.

Prototype:
  int IBTEXTSAP_SendAPDUToSIM( IBTExtSAP* po, const AEEBTByteSequence* pReq );

Parameters:
  po    - pointer to the IBTExtSAP object
  pReq  - the request containing the command APDU

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_SendAPDUFromSIM()

Description:
  Server application issues this command to send response-APDU from SIM
  to client.  pRsp is ignored if result indicates error.

Prototype:
  int IBTEXTSAP_SendAPDUFromSIM( IBTExtSAP* po, AEEBTSAPResult result,
                                 const AEEBTByteSequence* pRsp );

Parameters:
  po      - pointer to the IBTExtSAP object
  result  - result of the request
  pReq    - the message containing the response-APDU from the SIM

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_GetATRFromSIM()

Description:
  Client application issues this command to request server to transfer
  the ATR (Answer To Reset) from the SIM on the server.

Prototype:
  int IBTEXTSAP_GetATRFromSIM( IBTExtSAP* po );

Parameters:
  po    - pointer to the IBTExtSAP object

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_SendATRFromSIM()

Description:
  Server application issues this command to send ATR from SIM to client.

Prototype:
  int IBTEXTSAP_SendATRFromSIM( IBTExtSAP* po, AEEBTSAPResult result,
                                const AEEBTByteSequence* pRsp );

Parameters:
  po      - pointer to the IBTExtSAP object
  result  - result of the request
  pReq    - the message containing the ATR from the SIM

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_PowerSIMOff()

Description:
  Client application issues this command to request server to power off
  the SIM.

Prototype:
  int IBTEXTSAP_PowerSIMOff( IBTExtSAP* po );

Parameters:
  po    - pointer to the IBTExtSAP object

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_PowerSIMOffRespond()

Description:
  Server application issues this command to respond to client's request
  to power off the SIM.

Prototype:
  int IBTEXTSAP_PowerSIMOffRespond( IBTExtSAP* po, AEEBTSAPResult result );

Parameters:
  po      - pointer to the IBTExtSAP object
  result  - result of the request

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_PowerSIMOn()

Description:
  Client application issues this command to request server to power off
  the SIM.

Prototype:
  int IBTEXTSAP_PowerSIMOn( IBTExtSAP* po );

Parameters:
  po    - pointer to the IBTExtSAP object

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_PowerSIMOnRespond()

Description:
  Server application issues this command to respond to client's request
  to power on the SIM.

Prototype:
  int IBTEXTSAP_PowerSIMOnRespond( IBTExtSAP* po, AEEBTSAPResult result );

Parameters:
  po      - pointer to the IBTExtSAP object
  result  - result of the request

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_ResetSIM()

Description:
  Client application issues this command to request server to reset
  the SIM.

Prototype:
  int IBTEXTSAP_ResetSIM( IBTExtSAP* po );

Parameters:
  po    - pointer to the IBTExtSAP object

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_ResetSIMRespond()

Description:
  Server application issues this command to respond to client's request
  to reset the SIM.

Prototype:
  int IBTEXTSAP_ResetSIMRespond( IBTExtSAP* po, AEEBTSAPResult result );

Parameters:
  po      - pointer to the IBTExtSAP object
  result  - result of the request

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_GetCardReaderStatus()

Description:
  Client application issues this command to request the card reader 
  status from the server.

Prototype:
  int IBTEXTSAP_GetCardReaderStatus( IBTExtSAP* po );

Parameters:
  po    - pointer to the IBTExtSAP object

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_SendCardReaderStatus()

Description:
  Server application issues this command to send the card reader 
  status to the client.

Prototype:
  int IBTEXTSAP_SendCardReaderStatus( IBTExtSAP* po, AEEBTSAPResult result,
                                      uint8 status );

Parameters:
  po      - pointer to the IBTExtSAP object
  result  - result of the request
  status  - status of the card reader, see GSM 11.14 for detail

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTSAP_ReportSIMStatus()

Description:
  Server application issues this command to send SIM status to the client.

Prototype:
  int IBTEXTSAP_ReportSIMStatus( IBTExtSAP* po, AEEBTSAPStatusChange status );

Parameters:
  po      - pointer to the IBTExtSAP object
  status  - SIM status

Return value:
  SUCCESS   - the command was queued
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointer(s)
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================*/
#endif /* _AEE_BT_EXT_SAP_H_ */
