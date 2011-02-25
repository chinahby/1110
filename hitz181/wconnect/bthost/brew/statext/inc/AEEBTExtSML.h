#ifndef _AEE_BT_EXT_SML_H_
#define _AEE_BT_EXT_SML_H_

/*===========================================================================
FILE:      AEEBTExtSML.h

SERVICES:  BlueTooth Basic Pring Profile Interface

GENERAL DESCRIPTION: BREW interface to BT SML

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtSML

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEFile.h"

// BREW generated ID
#define AEECLSID_BLUETOOTH_SML     0x01023991

// event types returned via HandleEvent()
enum
{
  AEEBT_SML_EVT_REG_DONE,       
  AEEBT_SML_EVT_DEREG_DONE,   
  AEEBT_SML_EVT_CONN_REQ, 

  AEEBT_SML_EVT_OP_ABORTED,
  AEEBT_SML_EVT_CLI_GET_REQ,
  AEEBT_SML_EVT_CLI_PUT_REQ,
  AEEBT_SML_EVT_SRV_GET_REQ,
  AEEBT_SML_EVT_SRV_PUT_REQ,

  AEEBT_SML_EVT_PUT_CFM,
  AEEBT_SML_EVT_GET_RCV_DATA,

  AEEBT_SML_EVT_CONNECTED, 
  AEEBT_SML_EVT_CONN_FAILED, 
  AEEBT_SML_EVT_DISCONNECTED 
};

// error codes for AEEBT_SML_EVT_*
// DEPRECATED - DO NOT ADD ERROR CODES TO THIS ENUMERATION!
enum
{  
  //DEPRECATED
  AEEBT_SML_ERR_NONE          = AEEBT_ERR_NONE,            // no worry
  AEEBT_SML_ERR_BAD_PARAM     = AEEBT_ERR_BAD_PARAM,       // for any function
  AEEBT_SML_ERR_RESOURCE      = AEEBT_ERR_RESOURCE,        // lack of resource (memory, ...)
  AEEBT_SML_ERR_DB_FULL       = AEEBT_ERR_SD_DB_FULL,      // SDDB is full
  AEEBT_SML_ERR_REC_EXISTS    = AEEBT_ERR_SD_REC_EXISTS,   // server record already in SDDB
  AEEBT_SML_ERR_REC_NOT_FOUND = AEEBT_ERR_SD_REC_NOT_FOUND,// server record not found in SDDB
  AEEBT_SML_ERR_FAILED        = AEEBT_ERR_UNKNOWN          // general failure
  // DEPRECATED
};

typedef struct _IBTExtSML IBTExtSML;

AEEINTERFACE(IBTExtSML)
{
  INHERIT_IQueryInterface(IBTExtSML);

  int (*RegisterClient)           (IBTExtSML*                 po,
                                   const char*                pServiceUUID,
                                   boolean                    bAuth);
  int (*RegisterServer)           (IBTExtSML*                 po,
                                   const char*                pServiceUUID,
                                   boolean                    bAuth);
  int (*DeregisterClient)         (IBTExtSML*                 po);
  int (*DeregisterServer)         (IBTExtSML*                 po);
  int (*AcceptConnection)         (IBTExtSML*                 po,
                                   boolean                    bAccept);
  int (*Connect)                  (IBTExtSML*                 po, 
				                           const AEEBTBDAddr*         pBDAddr, 
                                   uint8                      uChannelNumber,
			                             const char*                pTarget);
  int (*Disconnect)               (IBTExtSML*                 po);
  int (*Get)                      (IBTExtSML*                 po,
                                   const char*                pType);
  int (*Put)                      (IBTExtSML*                 po,
                                   const char*                pType,
                                   const AEEBTSMLbyteSeqType* pData);
};

#define IBTEXTSML_AddRef(p)                               \
        AEEGETPVTBL((p),IBTExtSML)->AddRef((p))

#define IBTEXTSML_Release(p)                              \
        AEEGETPVTBL((p),IBTExtSML)->Release((p))

#define IBTEXTSML_QueryInterface(p,i,ppo)                 \
        AEEGETPVTBL((p),IBTExtSML)->QueryInterface((p),(i),(ppo))

#define IBTEXTSML_RegisterClient(p,ps,a)                        \
        AEEGETPVTBL((p),IBTExtSML)->RegisterClient((p),(ps),(a))
        
#define IBTEXTSML_RegisterServer(p,ps,a)                        \
        AEEGETPVTBL((p),IBTExtSML)->RegisterServer((p),(ps),(a))

#define IBTEXTSML_DeregisterClient(p)                           \
        AEEGETPVTBL((p),IBTExtSML)->DeregisterClient((p))
        
#define IBTEXTSML_DeregisterServer(p)                           \
        AEEGETPVTBL((p),IBTExtSML)->DeregisterServer((p))

#define IBTEXTSML_AcceptConnection(p,b)                         \
        AEEGETPVTBL((p),IBTExtSML)->AcceptConnection((p),(b))
        
#define IBTEXTSML_Connect(p,pa,c,pt)                         \
        AEEGETPVTBL((p),IBTExtSML)->Connect((p),(pa),(c),(pt))

#define IBTEXTSML_Disconnect(p)                           \
        AEEGETPVTBL((p),IBTExtSML)->Disconnect((p))
        
#define IBTEXTSML_Get(p,pt)                    \
        AEEGETPVTBL((p),IBTExtSML)->Get((p),(pt))
        
#define IBTEXTSML_Put(p,pt,pd)         \
        AEEGETPVTBL((p),IBTExtSML)->Put((p),(pt),(pd))

/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtSML

Description: BlueTooth SyncML Profile

  Consult the Bluetooth ISOD for description of SML.
  
  The use of IBTExtSML consists of sending commands and receiving events.  
  These events are delivered to IBTExtSML clients as notifications, hence
  the clients must register for SML notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_SML.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.  However, not all events are associated with 
  a command.  Some events are asyncronously generated such as a pin request
  sent by remote device.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_SML_EVT_* events when the notification mask
  is NMASK_BT_SML.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                                   pData->
  -----                                   ----------------------------
  AEEBT_SML_EVT_CONN_REQ                  bdAddr (of local device)
  AEEBT_SML_EVT_REG_DONE                  uError
  AEEBT_SML_EVT_DEREG_DONE                uError

  AEEBT_SML_EVT_OP_ABORTED                uError
  AEEBT_SML_EVT_CLI_GET_REQ               SmlObject
  AEEBT_SML_EVT_CLI_PUT_REQ               SmlObject
  AEEBT_SML_EVT_SRV_GET_REQ               SmlObject
  AEEBT_SML_EVT_SRV_PUT_REQ               SmlObject

  AEEBT_SML_EVT_PUT_CFM                   SmlObject
  AEEBT_SML_EVT_GET_RCV_DATA              SmlObject


  AEEBT_SML_EVT_CONNECTED                 bdAddr (of remote device)
  AEEBT_SML_EVT_CONN_FAILED               bdAddr
  AEEBT_SML_EVT_DISCONNECTED              bdAddr

Interfaces:

  IBTEXTSML provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTSML_AddRef()
    IBTEXTSML_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTSML_QueryInterface()
    
=======================================================================

Function:  IBTEXTSML_RegisterClient()

Description:
  Server application uses this command to initialize the SML client and 
  registers it in the service discovery database (SDDB.)

Prototype:
  int IBTEXTSML_RegisterClient(
                          IBTExtSML*     po,
				                  const char*    pServiceUUID,
                          boolean        bAuth);

Parameters:
  po                    - pointer to the IBTExtSML object
  pServiceUUID          - 128-bit service UUID
  bAuth                 - Whether to enable authentication

Return value:
  SUCCESS   - the request to register with SDDB was queued and that
              AEEBT_SML_EVT_REG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or unrecognized supported formats

Event:
  AEEBT_SML_EVT_REG_DONE with accompanying error code of one of the
  following values:
    AEEBT_SML_ERR_NONE
    AEEBT_SML_ERR_REC_EXISTS
    
=======================================================================

Function:  IBTEXTSML_RegisterServer()

Description:
  Server application uses this command to initialize the SML server and 
  registers it in the service discovery database (SDDB.)

Prototype:
  int IBTEXTSML_RegisterClient(
                          IBTExtSML*     po,
				                  const char*    pServiceUUID,
                          boolean        bAuth);

Parameters:
  po                    - pointer to the IBTExtSML object
  pServiceUUID          - 128-bit service UUID
  bAuth                 - Whether to enable authentication

Return value:
  SUCCESS   - the request to register with SDDB was queued and that
              AEEBT_SML_EVT_REG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or unrecognized supported formats

Event:
  AEEBT_SML_EVT_REG_DONE with accompanying error code of one of the
  following values:
    AEEBT_SML_ERR_NONE
    AEEBT_SML_ERR_REC_EXISTS

=======================================================================

Function:  IBTEXTSML_DeregisterClient()

Description: 
  Terminates the Bluetooth SyncML client and deregisters it 
  from the SDDB.

Prototype:
  int IBTEXTSML_DeregisterClient( IBTExtSML* po );

Parameters:
  po                    - pointer to the IBTExtSML object

Return value:
  SUCCESS   - the request to deregister with SDDB was queued and that
              AEEBT_SML_EVT_DEREG_DONE event will be generated at a later time
  ENOMEMORY - could not queue command due to lack of memory

Event:
  AEEBT_SML_EVT_DEREG_DONE with accompanying error code of one of the
  following values:
    AEEBT_SML_ERR_NONE
    AEEBT_SML_ERR_REC_NOT_FOUND
    
=======================================================================

Function:  IBTEXTSML_DeregisterServer()

Description: 
  Terminates the Bluetooth SyncML server and deregisters it 
  from the SDDB.

Prototype:
  int IBTEXTSML_DeregisterServer( IBTExtSML* po );

Parameters:
  po                    - pointer to the IBTExtSML object

Return value:
  SUCCESS   - the request to deregister with SDDB was queued and that
              AEEBT_SML_EVT_DEREG_DONE event will be generated at a later time
  ENOMEMORY - could not queue command due to lack of memory

Event:
  AEEBT_SML_EVT_DEREG_DONE with accompanying error code of one of the
  following values:
    AEEBT_SML_ERR_NONE
    AEEBT_SML_ERR_REC_NOT_FOUND

=======================================================================

Function:  IBTEXTOPP_AcceptConnection()

Description: 
  The server application must issue this command to accept or reject a 
  connection request from a client. 

Prototype:
  int IBTEXTSML_AcceptConnection( IBTExtSML* po,
                                  boolean    bAccept );

Parameters:
  po                    - pointer to the IBTExtSML object
  bAccept               - whether to accept connection

Return value:
  SUCCESS   - the request to accept/reject the connection request was queued
              and that AEEBT_SML_EVT_CONNECTED event will be generated at a 
              later time (if the server accepted the connection.)
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  AEEBT_SML_EVT_CONNECTED with the accompanying error code AEEBT_SML_ERR_NONE.

=======================================================================

Function:  IBTEXTSML_Connect()

Description:
  Called by client application to connect to remote SML server.

Prototype:

  int IBTEXTSML_Connect(
                          IBTExtSML*         po,
				       				    const AEEBTBDAddr* pBDAddr, 
                          uint8              uChannelNumber,
			                    const char*        pTarget);

Parameters:

  po              - pointer to the IBTExtSML object
  pBDAddr         - BD address of SML server
  uChannelNumber  - the server channel number to use to establish connection
  target          - UUID of the target service

Return value:
  SUCCESS   - the request to connect to SML server was queued and that
              AEEBT_SML_EVT_CONNECTED or AEEBT_SML_EVT_CONN_FAILED event will 
              be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or invalid channel number

Event:
  AEEBT_SML_EVT_CONNECTED or
  AEEBT_SML_EVT_CONN_FAILED

=======================================================================

Function:  IBTEXTSML_Disconnect()

Description:
  Client can call this interface to terminate the OBEX connection
  with the server.

Prototype:
  int IBTEXTSML_Disconnect( IBTExtSML* po );

Parameters:
  po          - pointer to the IBTExtSML object

Return value:
  SUCCESS   - the request to disconnect from remote device was queued and that
              AEEBT_SML_EVT_DISCONNECTED event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_SML_EVT_DISCONNECTED with the one of the error codes:
    AEEBT_SML_ERR_NONE
    AEEBT_SML_ERR_NO_CONN

=======================================================================

Function:  IBTEXTSML_Get()

Description:
  Client or server issues this command to recieve an SML object

Prototype:
  int IBTEXTSML_Get( 
                          IBTExtSML*     po,
                          const char*    pType);

Parameters:
  po              - pointer to the IBTExtSML object
  pType           - MIME Media Type version

Return value:
  SUCCESS   - the request to get the object has been queued
              and that AEEBT_SML_EVT_CLI_GET_RSP or AEEBT_SML_EVT_SRV_GET_RSP
              event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_SML_EVT_CLI_GET_REQ 
  AEEVT_SML_EVT_SRV_GET_REQ

=======================================================================

Function:  IBTEXTSML_Put()

Description:
  Client or server issues this command to send a SML object

Prototype:
  int IBTEXTSML_Put( 
                          IBTExtSML*           po,
                          const char*          pType,
                          AEEBTSMLbyteSeqType* pData);

Parameters:

  po              - pointer to the IBTExtSML object
  pType           - object MIME type
  pData           - pointer to data stream

Return value:
  SUCCESS   - the request to put the SML object has been queued
              and that AEEBT_SML_EVT_CLI_PUT_REQ or AEEBT_SML_EVT_SRV_PUT_REQ
              event will be generated  at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_SML_EVT_CLI_PUT_REQ
  AEEBT_SML_EVT_SRV_PUT_REQ
  
=======================================================================*/
#endif /* _AEE_BT_EXT_SML_H_ */
