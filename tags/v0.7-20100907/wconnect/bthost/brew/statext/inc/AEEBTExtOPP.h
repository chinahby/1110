#ifndef _AEE_BT_EXT_OPP_H_
#define _AEE_BT_EXT_OPP_H_

/*===========================================================================
FILE:      AEEBTExtOPP.h

SERVICES:  BlueTooth Object Push Profile Interface

GENERAL DESCRIPTION: BREW interface to BT OPP

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtOPP

        Copyright © 2004-2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEFile.h"

// BREW generated ID
#define AEECLSID_BLUETOOTH_OPP     0x0101f703

// event types returned via HandleEvent()
  /* for servers */
#define AEEBT_OPP_EVT_REG_DONE   0 // registration process done
#define AEEBT_OPP_EVT_DEREG_DONE 1 // deregistration process done
#define AEEBT_OPP_EVT_CONN_REQ   2 // a client wants to connect
#define AEEBT_OPP_EVT_PUSH_REQ   3 // client wants to push object to server
#define AEEBT_OPP_EVT_PULL_REQ   4 
  // client wants to pull server's default object

  /* for clients */
#define AEEBT_OPP_EVT_OBJ_PUSHED 5 // current push operation completed
#define AEEBT_OPP_EVT_OBJ_PULLED 6 // server's default business card pulled
#define AEEBT_OPP_EVT_OP_ABORTED 7 // current operation aborted
#define AEEBT_OPP_EVT_CONNECTING   12 // client connect operation in progress

  /* common to servers and clients */
#define AEEBT_OPP_EVT_CONNECTED     8 // connection is up
#define AEEBT_OPP_EVT_CONN_FAILED   9 // connection could not be brought up
#define AEEBT_OPP_EVT_DISCONNECTED 10 // connection is down
#define AEEBT_OPP_EVT_PROGRESS     11 // data transfer in progress


// error codes for AEEBT_OPP_EVT_*
// DEPRECATED - DO NOT ADD ERROR CODES TO THIS ENUMERATION!
enum
{
  // DEPRECATED
  AEEBT_OPP_ERR_NONE          = AEEBT_ERR_NONE,            // no worry
  AEEBT_OPP_ERR_BAD_PARAM     = AEEBT_ERR_BAD_PARAM,       // for any function
  AEEBT_OPP_ERR_RESOURCE      = AEEBT_ERR_RESOURCE,        // lack of resource (memory, ...)
  AEEBT_OPP_ERR_DB_FULL       = AEEBT_ERR_SD_DB_FULL,      // SDDB is full
  AEEBT_OPP_ERR_REC_EXISTS    = AEEBT_ERR_SD_REC_EXISTS,   // server record already in SDDB
  AEEBT_OPP_ERR_REC_NOT_FOUND = AEEBT_ERR_SD_REC_NOT_FOUND,// server record not found in SDDB
  AEEBT_OPP_ERR_FAILED        = AEEBT_ERR_UNKNOWN          // general failure
  // DEPRECATED
};

// OPP formats
#define AEEBT_OPP_FORMAT_VCARD_2_1  0x01
#define AEEBT_OPP_FORMAT_VCARD_3_0  0x02
#define AEEBT_OPP_FORMAT_VCAL_1_0   0x04
#define AEEBT_OPP_FORMAT_ICAL_2_0   0x08
#define AEEBT_OPP_FORMAT_VNOTE      0x10
#define AEEBT_OPP_FORMAT_VMESSAGE   0x20
#define AEEBT_OPP_FORMAT_ANY        0xFFFF0000
#define AEEBT_OPP_FORMAT_ALL        0xFFFF003F

typedef uint32  AEEBTOPPFormat;

typedef struct _IBTExtOPP IBTExtOPP;

AEEINTERFACE(IBTExtOPP)
{
  INHERIT_IQueryInterface(IBTExtOPP);

  /* for servers */
  int (*Register)(IBTExtOPP* po, AEEBTOPPFormat supportedFormat,
                  const char* szServiceName);
  int (*Deregister)(IBTExtOPP* po);
  int (*AcceptConnection)(IBTExtOPP* po, const AEEBTBDAddr* pBDAddr, 
                          boolean bAllowPush, boolean bAllowPull);
  /* for clients */
  int (*Connect)(IBTExtOPP* po, const AEEBTBDAddr* pBDAddr, 
                 uint8 uChannelNumber);
  int (*Abort)(IBTExtOPP* po);
  int (*Push)(IBTExtOPP* po, const AECHAR* pwFileName, AEEBTObjectType objType);
  int (*Pull)(IBTExtOPP* po, const AECHAR* pwFileName);

  /* common to servers and clients */
  int (*Disconnect)(IBTExtOPP* po);
  int (*GetProgressInfo)(IBTExtOPP* po, AEEBTProgressInfo* pProgressInfo);
};


#define IBTEXTOPP_AddRef(p)                               \
        AEEGETPVTBL((p),IBTExtOPP)->AddRef((p))

#define IBTEXTOPP_Release(p)                              \
        AEEGETPVTBL((p),IBTExtOPP)->Release((p))

#define IBTEXTOPP_QueryInterface(p,i,ppo)                 \
        AEEGETPVTBL((p),IBTExtOPP)->QueryInterface((p),(i),(ppo))

#define IBTEXTOPP_Register(p,f,pn)                        \
        AEEGETPVTBL((p),IBTExtOPP)->Register((p),(f),(pn))

#define IBTEXTOPP_Deregister(p)                           \
        AEEGETPVTBL((p),IBTExtOPP)->Deregister((p))

#define IBTEXTOPP_AcceptConnection(p,pa,b1,b2)            \
        AEEGETPVTBL((p),IBTExtOPP)->AcceptConnection((p),(pa),(b1),(b2))

#define IBTEXTOPP_Connect(p,pa,c)                         \
        AEEGETPVTBL((p),IBTExtOPP)->Connect((p),(pa),(c))

#define IBTEXTOPP_Abort(p)                                \
        AEEGETPVTBL((p),IBTExtOPP)->Abort((p))

#define IBTEXTOPP_Push(p,pf,t)                            \
        AEEGETPVTBL((p),IBTExtOPP)->Push((p),(pf),(t))

#define IBTEXTOPP_Pull(p,pf)                              \
        AEEGETPVTBL((p),IBTExtOPP)->Pull((p),(pf))

#define IBTEXTOPP_Disconnect(p)                           \
        AEEGETPVTBL((p),IBTExtOPP)->Disconnect((p))

#define IBTEXTOPP_GetProgressInfo(p,i)                           \
        AEEGETPVTBL((p),IBTExtOPP)->GetProgressInfo((p),(i))

/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtOPP

Description: BlueTooth Object Push Profile

  Consult the Bluetooth ISOD for description of OPP.
  
  The use of IBTExtOPP consists of sending commands and receiving events.  
  These events are delivered to IBTExtOPP clients as notifications, hence
  the clients must register for OPP notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_OPP.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.  However, not all events are associated with 
  a command.  Some events are asyncronously generated such as a pin request
  sent by remote device.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_OPP_EVT_* events when the notification mask
  is NMASK_BT_OPP.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                                   pData->
  -----                                   ----------------------------
  AEEBT_OPP_EVT_CONN_REQ                  bdAddr - BD addr of client
  AEEBT_OPP_EVT_REG_DONE                  uError
  AEEBT_OPP_EVT_DEREG_DONE                uError

  AEEBT_OPP_EVT_OBJ_PUSHED                uError
  AEEBT_OPP_EVT_OBJ_PULLED                uError
  AEEBT_OPP_EVT_OP_ABORTED                uError

  AEEBT_OPP_EVT_CONNECTED                 bdAddr - BD addr of remote device
  AEEBT_OPP_EVT_CONN_FAILED               bdAddr
  AEEBT_OPP_EVT_DISCONNECTED              bdAddr
  AEEBT_OPP_EVT_CONNECTING                bdAddr
  AEEBT_OPP_EVT_PROGRESS                  ProgressInfo

Interfaces:

  IBTEXTOPP provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTOPP_AddRef()
    IBTEXTOPP_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTOPP_QueryInterface()

=======================================================================

Function:  IBTEXTOPP_Register()

Description:
  Server application uses this command to initialize the OPP server and 
  registers it in the service discovery database (SDDB.)

Prototype:
  int IBTEXTOPP_Register( IBTExtOPP*     po,
                          AEEBTOPPFormat supportedFormat,
                          const char*    szServiceName );

Parameters:
  po              - pointer to the IBTExtOPP object
  supportedFormat - the formats this OPP server supports
  szServiceName   - service name to store in SDDB; must not be NULL

Return value:
  SUCCESS   - the request to register with SDDB was queued and that
              AEEBT_OPP_EVT_REG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or unrecognized supported formats

Event:
  AEEBT_OPP_EVT_REG_DONE with accompanying error code of one of the
  following values:
    AEEBT_OPP_ERR_NONE
    AEEBT_OPP_ERR_REC_EXISTS
=======================================================================

Function:  IBTEXTOPP_Deregister()

Description: 
  Terminates the Object Push server and deregisters it from the SDDB.

Prototype:
  int IBTEXTOPP_Deregister( IBTExtOPP* po );

Parameters:
  po           - pointer to the IBTExtOPP object

Return value:
  SUCCESS   - the request to deregister with SDDB was queued and that
              AEEBT_OPP_EVT_DEREG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory

Event:
  AEEBT_OPP_EVT_DEREG_DONE with accompanying error code of one of the
  following values:
    AEEBT_OPP_ERR_NONE
    AEEBT_OPP_ERR_REC_NOT_FOUND

=======================================================================

Function:  IBTEXTOPP_AcceptConnection()

Description: 
  The server application must issue this command to accept or reject a 
  connection request from a client.  To reject a connection request, app 
  must set both boolean parameters to FALSE.

Prototype:
  int IBTEXTOPP_AcceptConnection( IBTExtOPP* po, 
                                  const AEEBTBDAddr* pBDAddr,
                                  boolean bAllowPush,
                                  boolean bAllowPull );

Parameters:
  po            - pointer to the IBTEXTOPP object
  pBDAddr       - BD address of the requesting client
  bAllowPush    - TRUE if client is allowed to push objects to this server
  bAllowPull    - TRUE if client is allowed to pull objects from this server

Return value:
  SUCCESS   - the request to accept/reject the connection request was queued
              and that AEEBT_OPP_EVT_CONNECTED event will be generated at a 
              later time (if the server accepted the connection.)
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  AEEBT_OPP_EVT_CONNECTED with the accompanying error code AEEBT_OPP_ERR_NONE.

=======================================================================

Function:  IBTEXTOPP_Connect()

Description:
  Called by client application to connect to remote OPP server.

Prototype:

  int IBTEXTOPP_Connect( IBTExtOPP* po, const AEEBTBDAddr* pBDAddr, 
                         uint8 uChannelNumber );

Parameters:

  po              - pointer to the IBTExtOPP object
  pBDAddr         - BD address of OPP server
  uChannelNumber  - the server channel number to use to establish connection

Return value:
  SUCCESS   - the request to connect to OPP server was queued and that
              AEEBT_OPP_EVT_CONNECTED or AEEBT_OPP_EVT_CONN_FAILED event will 
              be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or invalid channel number

Event:
  AEEBT_OPP_EVT_CONNECTED or
  AEEBT_OPP_EVT_CONN_FAILED

=======================================================================

Function:  IBTEXTOPP_Abort()

Description:
  Client application can issue this command to abort the current operation.

Prototype:
  int IBTEXTOPP_Abort( IBTExtOPP* po );

Parameters:
  po          - pointer to the IBTExtOPP object

Return value:
  SUCCESS   - the current operation has been aborted.
  EIDLE     - no operation in progress
  EBADSTATE - no connection
  
Event:
  None.

=======================================================================

Function:  IBTEXTOPP_Push()

Description:
  This command can be used by server and client applications.  It should 
  only be called after AEEBT_OPP_EVT_CONNECTED is received.

  Client application issues this command to push an object to the server.
  
  Server application uses it to send the default VCard being pulled by the 
  client.  If the default VCard does not exist, pwFileName should be set to
  NULL.

Prototype:
  int IBTEXTOPP_Push( IBTExtOPP* po, const AECHAR* pwFileName, 
                      AEEBTObjectType objType );

Parameters:
  po          - pointer to the IBTExtOPP object
  pwFileName  - the file containing the object being pushed to remote device
  objType     - type of object being pushed

Return value:
  SUCCESS   - the request to push an object to the remote device was queued 
              and that AEEBT_OPP_EVT_OBJ_PUSHED event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_OPP_EVT_OBJ_PUSHED

=======================================================================

Function:  IBTEXTOPP_Pull()

Description:
  This command can be used by server and client applications.
  
  Client application issues this command to pull the server's default object.
  It should be called only after connection has already been established, i.e,
  after AEEBT_OPP_EVT_CONNECTED is received.
  
  Server application uses it to retrieve the object being pushed by the client.
  In server role, app can reject the push request by passing NULL for 2nd parameter 
  (pwFileName = NULL).

Prototype:
  int32 IBTEXTOPP_Pull( IBTExtOPP* po, const AECHAR* pwFileName );

Parameters:

  po        - pointer to the IBTExtOPP object
  pFileName - the file where object pulled from remote device will be stored

Return value:
  SUCCESS   - the request to pull an object from the remote device was queued 
              and that AEEBT_OPP_EVT_OBJ_PULLED event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_OPP_EVT_OBJ_PULLED
  
=======================================================================

Function:  IBTEXTOPP_Disconnect()

Description:
  Either server or client application can use this command to bring down the
  existing connection between them.  This API can also be used to cancel
  a connect operation.

Prototype:
  int IBTEXTOPP_Disconnect( IBTExtOPP* po );

Parameters:
  po          - pointer to the IBTExtOPP object

Return value:
  SUCCESS   - the request to disconnect from remote device was queued and that
              AEEBT_OPP_EVT_DISCONNECTED event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_OPP_EVT_DISCONNECTED with the one of the error codes:
    AEEBT_OPP_ERR_NONE
    AEEBT_OPP_ERR_NO_CONN

=========================================================================

Function:  IBTEXTOPP_GetProgressInfo()

Description:
  Either server or client application can use this API to read the total
  number of bytes transferred. This API should be invoked when 
  AEEBT_OPP_EVT_PROGRESS event is received.

Prototype:
  int IBTEXTOPP_GetProgressInfo( IBTExtOPP* po,
                                 AEEBTProgressInfo* pProgressInfo );

Parameters:
  po            - pointer to the IBTExtOPP object
  pProgressInfo - pointer to structure that holds the size of the object
                  and the number of bytes that have alreay been 
                  transferred.
Return value:
  SUCCESS   - Information successfully returned
  EBADPARM  - NULL pointers

Event:
  None
=======================================================================*/
#endif /* _AEE_BT_EXT_OPP_H_ */
