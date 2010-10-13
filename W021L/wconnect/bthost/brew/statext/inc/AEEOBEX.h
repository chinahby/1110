#ifndef _AEE_OBEX_H
#define _AEE_OBEX_H

/*===========================================================================
FILE:      AEEOBEX.h

SERVICES:  OBEX BREW extension

GENERAL DESCRIPTION: BREW interface to the OBEX profile (NT-OBEX).
                     This file contains all the data structures and
                     data types used by the IOBEX interface

PUBLIC CLASSES AND STATIC FUNCTIONS:



        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/inc/AEEOBEX.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
  $Change: 815296 $
                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
2/28/07    MH      Added support for server disconnect response.
2/06/07    MH      Added support for additional OBEX response codes
1/29/07    MH      Added support for 128Bit UUID and service name registration 
11/28/06   MH      IOBEX Interface documentation update.
10/26/06   MH      Modified confirm set path to support all headers.
9/25/06    MH      Added response and command status codes. 
9/19/06    MH      Added Server Initiating Authentication command 
8/25/06    bh      compile erros fixed
8/14/06    bh      Updated comments for Client Authentication API
7/28/06    bh      modified for obex client authentication
6/23/06    jh      modified definition of IOBEXClient_Disconnect, SetPath,
                   IOBEXServer_AcceptConnect and Register to support JSR82.
3/24/04    sp      corrected featurization so that header file is not featurized
11/13/04   sp      initial checkin. unit tested for IOBEXServer API

=============================================================================*/


#include "AEE.h"

#include "AEEModel.h"
#include "AEEOBEXCommon.h"

//  BREW generated ID
#define AEECLSID_OBEXSERVER     0x0102d079
//BREW generated id
#define AEECLSID_OBEXCLIENT     0x0102efb1


//The IOBEX object
typedef struct _IOBEX IOBEX;

//We present a IOBEXClient and IOBEXServer interface to the app developer
//behind the scenes, its an IOBEX object that maintains a vtable that points to either
//the client or the server interface
typedef  IOBEX IOBEXServer;
typedef  IOBEX IOBEXClient;

//Transport related stuff

/* For Server Registration Purposes
   configuration structs that can tell the transports to prepare by adding
   service discovery or security settings to their databases etc
*/
typedef struct
{
    AEEOBEX_BT_SD_UUIDType      uuid;
    /*Set TRUE for 128 Bit UUID*/
    boolean                     is_uuid128;
    AEEOBEX_BT_SD_UUID128Type   uuid128;  
    char                        serv_name_str[AEE_OBEXSERVER_MAX_SERVICENAME_LEN];
    AEEOBEXBTSecurityType       security;
    /* etc */
} AEEOBEXBTConfigStructType;


//Note that only the bt config type is included in this struct even though the transport config
//type may include other transport types. If we discover that additional parameters are needed
//to support these transports, then we will add additional structs as needed.
typedef struct
{
    AEEOBEXTransportConfigType  transports;
    AEEOBEXBTConfigStructType   bt_config;
} AEEOBEXTransportConfigStructType;


/**
 * @name OBEX Header types
 * OBEX headers consist of a single-byte header ID followed by zero or more bytes
 * of header value. The most significant 2 bits of the header ID identify
 * one of four different kinds of header.
 * @{
 */
#define OBEX_HDR_ID_UNICODE  0x00 /**< null-terminated Unicode text, length prefixed with 2-byte unsigned integer */
#define OBEX_HDR_ID_BYTESEQ  0x40 /**< byte sequence, length prefixed with 2-byte unsigned integer */
#define OBEX_HDR_ID_UINT8    0X80 /**< 1-byte quantity */
#define OBEX_HDR_ID_UINT32   0xC0  /**< 4-byte quantity, transmitted in network byte order (big endian) */

#define OBEX_HDR_KIND(h)      ((h) & 0xC0)
#define OBEX_VAR_LEN_HDR(h)   (((h) & 0x80) == 0)

// error codes for AEE_OBEX_EVT_*. This code is filled into the uStatusCode field of the Notification (EVT_NOTIFY) or into
//the ModelEvent's dwParam field

enum _AEEOBEXStatusType
{
    AEE_OBEX_ERR_NONE,                  //all is well!
    AEE_OBEX_ERR_BAD_PARAM,             //bad params....may be returned for any function
    AEE_OBEX_ERR_CONTINUE,              //continue
    AEE_OBEX_ERR_RESOURCE,              //out of memory or no more connections
    AEE_OBEX_MISSING_HEADER,
    AEE_OBEX_CONNECT_FAILED,            //failed for unknown reasons
    AEE_OBEX_TIMEOUT,
    AEE_OBEX_NO_SUCH_FOLDER,
    AEE_OBEX_ERR_NOT_ALLOWED,           //operation is not allowed
    AEE_OBEX_INCOMPLETE_PACKET,         // Incomplete Packet
    AEE_OBEX_ERR_OBJ_EXISTS,            //cannot over-write object
	AEE_OBEX_ERR_LENGTH_REQUIRED,       // Length required
    AEE_OBEX_DATABASE_LOCKED,           // Database Locked
    AEE_OBEX_NOT_MODIFIED,              // Not Modified
    AEE_OBEX_GONE,                      // Gone
    AEE_OBEX_REQ_TIMEOUT,               // Request Timeout  
    AEE_OBEX_PRECONDITION_FAILED,       // Precondition failed
    AEE_OBEX_SERVICE_UNAVAILABLE,       // Service unavailable
    AEE_OBEX_NOT_FOUND,                 // Not Found  
    AEE_OBEX_VALUE_NOT_ACCEPTABLE,      // Not Acceptable
    AEE_OBEX_DATABASE_FULL,             // Database Full
    AEE_OBEX_INTERNAL_SERVER_ERROR,     // Internal Server Error
    AEE_OBEX_UNSUPPORTED_MEDIA_TYPE,    // Unsupported Media Type
    AEE_OBEX_UNAUTHORIZED,              // UnAuthorized
    AEE_OBEX_PARTIAL_CONTENT,           // Partial content
    AEE_OBEX_ERR_FAILED                 //unknown failure
} ;
typedef uint8 AEEOBEXStatusType;


//event types returned via EventCallback()
enum _AEEOBEXEventType
{
    /* for servers */
    AEE_OBEX_EVT_SRV_CON_REQ,

    /*For Server The first event can be Auth Request event and need
    to be notification event*/

    AEE_OBEX_EVT_SRV_AUTH_REQ
};

//event types sent via IMODEL_Notify
enum _AEEOBEXModelNotification
{
    AEE_OBEX_EVT_CONNECTED = AEE_OBEX_EVT_SRV_AUTH_REQ + 1,
    AEE_OBEX_EVT_SRV_DCN_IND,
    AEE_OBEX_EVT_SRV_GET_REQ,
    AEE_OBEX_EVT_SRV_PUT_REQ,
    AEE_OBEX_EVT_SRV_SET_PATH_REQ,
    
    /* for clients */
    AEE_OBEX_EVT_CLI_CON_CFM,
    AEE_OBEX_EVT_CLI_CONN_FAILED,
    AEE_OBEX_EVT_CLI_AUTH_REQ,
    AEE_OBEX_EVT_CLI_DCN_IND,
    AEE_OBEX_EVT_CLI_PUT_CFM,
    AEE_OBEX_EVT_CLI_GET_RCV_DATA,
    AEE_OBEX_EVT_CLI_SET_PATH_CFM,
    AEE_OBEX_EVT_CLI_AUTH_RESP

};

//type of authentication requested
enum _AEEOBEXAuthenticationType
{
    AEE_OBEX_AUTH_NONE,
    AEE_OBEX_AUTH_PASSWORD,
    AEE_OBEX_AUTH_USERID_AND_PASSWORD
}  ;
typedef uint8 AEEOBEXAuthenticationType;

//CmdStatus that the server includes in its response to a OBEX client request

enum _AEEOBEXCmdStatusType
{
    AEE_OBEX_STAT_SUCCESS,
    AEE_OBEX_STAT_CONTINUE,
    AEE_OBEX_STAT_COMMAND_ERROR,
    AEE_OBEX_STAT_OBEX_CONNECTION_TIMEOUT,
    AEE_OBEX_STAT_OBEX_CONNECT_FAILED,
    AEE_OBEX_STAT_OBEX_DISCONNECT_FAILED,
    AEE_OBEX_STAT_ERROR,
    AEE_OBEX_STAT_INCOMPLETE_PACKET,
    AEE_OBEX_STAT_LENGTH_REQUIRED,
    AEE_OBEX_STAT_NOT_CONNECTED,
    AEE_OBEX_STAT_NO_MORE_CONNECTIONS,
    AEE_OBEX_STAT_OPERATION_IN_PROGRESS,

    AEE_OBEX_STAT_PUT_RESPONSE_ERROR,
    AEE_OBEX_STAT_GET_RESPONSE_ERROR,
    AEE_OBEX_STAT_ERROR_REQUIRED_HEADER_NOT_FOUND,
    AEE_OBEX_STAT_SERVICE_UNAVAILABLE,

    AEE_OBEX_STAT_TOO_MANY_HEADER_BYTES,
    AEE_OBEX_STAT_UNKNOWN_COMMAND,
    AEE_OBEX_STAT_UNSUPPORTED_VERSION,
    AEE_OBEX_STAT_CLIENT_ABORTED_COMMAND,
    AEE_OBEX_STAT_BAD_PACKET,
    AEE_OBEX_STAT_BAD_REQUEST,
    AEE_OBEX_STAT_OBJECT_OVERFLOW,
    AEE_OBEX_STAT_NOT_FOUND,
    AEE_OBEX_STAT_ACCESS_DENIED,
    AEE_OBEX_STAT_VALUE_NOT_ACCEPTABLE,
    AEE_OBEX_STAT_PACKET_OVERFLOW,
    AEE_OBEX_STAT_NO_SUCH_FOLDER,
    AEE_OBEX_STAT_NAME_REQUIRED,
    AEE_OBEX_STAT_PASSWORD_TOO_LONG,
    AEE_OBEX_STAT_PRECONDITION_FAILED,
    AEE_OBEX_STAT_UNAUTHORIZED,
    AEE_OBEX_STAT_NOT_IMPLEMENTED,
    AEE_OBEX_STAT_INVALID_AUTH_DIGEST,
    AEE_OBEX_STAT_INVALID_OPERATION,
    AEE_OBEX_STAT_DATABASE_FULL,
    AEE_OBEX_STAT_DATABASE_LOCKED,
    AEE_OBEX_STAT_INTERNAL_SERVER_ERROR,
    AEE_OBEX_STAT_UNSUPPORTED_MEDIA_TYPE,
    AEE_OBEX_STAT_PARTIAL_CONTENT,
    AEE_OBEX_STAT_METHOD_NOT_ALLOWED,
    AEE_OBEX_STATSRV_INCOMPLETE_GET,
    AEE_OBEX_STAT_FOLDER_BROWSING_NOT_ALLOWED,
    AEE_OBEX_STAT_SERVER_FORCED_DISCONNECT,
    AEE_OBEX_STAT_NOT_MODIFIED,
    AEE_OBEX_STAT_GONE,
    AEE_OBEX_STAT_REQ_TIMEOUT,
    AEE_OBEX_STAT_CONNECTION_NOT_FOUND

};
typedef uint8 AEEOBEXCmdStatusType;

//Data supplied by the OEBX server app during registration
typedef struct
{
    AEEOBEXByteSeqType          *targetPtr;
    AEEOBEXAuthenticationType   auth;
    boolean                     bReadOnly;
    boolean                     authRequired;
    AEEOBEXTransportConfigStructType  transportConfig;
} AEEOBEXRegistrationInfoType;

//The IOBEX is an abstract "base class"
#define INHERIT_IOBEX(iname) \
    INHERIT_IQueryInterface(iname); \
    \
    int (*AuthResponse)(IOBEX *po , const char *pwUserID, const char *pwPassword); \
    int (*GetSupportedTransports)(IOBEX *po)


//the IOBEXServer class
AEEINTERFACE(IOBEXServer)
{
    INHERIT_IOBEX(IOBEXServer);

    /* for servers */
    int (*Register)(IOBEXServer *po, const AEEOBEXRegistrationInfoType *pReg,
                    uint8 *pSrvChNum);
    int (*Deregister)(IOBEXServer* po);
    int (*Initialize)(IOBEXServer *po, uint32 obexHandle);
    int (*AcceptConnection)(IOBEXServer *po,
                            boolean bAccept,
                            AEEOBEXHdrType *headersPtr,
                            uint8 headersCnt,
                            AEEOBEXCmdStatusType cmdStatus );
    int (*ConfirmSetPath)(IOBEXServer *po,
                          AEEOBEXHdrType   *headersPtr,
                          uint8   headersCnt,
                          AEEOBEXCmdStatusType cmdStatus);
    int (*PutResponse)(IOBEXServer *po,
                       AEEOBEXHdrType   *headersPtr,
                       uint8   headersCnt,
                       AEEOBEXCmdStatusType cmdStatus);
    int (*GetResponse)(IOBEXServer *po,
                       AEEOBEXHdrType   *headersPtr,
                       uint8   headersCnt,
                       AEEOBEXCmdStatusType cmdStatus);
    int (*ForceDisconnect)(IOBEXServer *po);
    int (*Authenticate)(IOBEXServer *po, 
                       AEEOBEXDeviceAddressType *pAddr,
                       AEEOBEXRealm *realmInfo,
                       boolean bUidReqd,
                       boolean bFullAccess);
    int (*DisconnectResponse)(IOBEXServer *po,
                       AEEOBEXHdrType   *headersPtr,
                       uint8   headersCnt,
                       AEEOBEXCmdStatusType cmdStatus);
};

//the IOBEXClient class
AEEINTERFACE(IOBEXClient)
{
    INHERIT_IOBEX(IOBEXClient);

    /* for clients */
    int (*Connect)(IOBEXClient *po,
                   const AEEOBEXDeviceAddressType *pAddr,
                   AEEOBEXHdrType  *headersPtr,
                   uint8            headersCnt);

    int (*Disconnect)(IOBEXClient *po,
                      AEEOBEXHdrType *headersPtr,
                      uint8 headersCnt);
    int (*Put)(IOBEXClient *po,
               AEEOBEXHdrType  *headersPtr,
               uint8  headersCnt,
               AEEOBEXCmdStatusType cmdStatus);

    int (*Get)(IOBEXClient *po,
               AEEOBEXHdrType  *headersPtr,
               uint8  headersCnt,
               boolean final);
    int (*SetPath)(IOBEXClient *po,
                   const AECHAR *pwPathName,
                   boolean upLevel,
                   boolean dontCreate,
                   AEEOBEXHdrType *headersPtr,
                   uint8 headersCnt);
    int (*AbortOperation)(IOBEXClient *po);
    int (*Authenticate)(IOBEXClient *po,
                        AEEOBEXDeviceAddressType *pAddr,
                        AEEOBEXRealm *realmInfo,
                        boolean bUidReqd,
                        boolean bFullAccess);    

};



#define IOBEXServer_AddRef(p)                               \
      AEEGETPVTBL((p),IOBEXServer)->AddRef((p))

#define IOBEXServer_Release(p)                               \
      AEEGETPVTBL((p),IOBEXServer)->Release((p))

#define IOBEXServer_QueryInterface(p,i,ppo)                               \
      AEEGETPVTBL((p),IOBEXServer)->QueryInterface((p),(i),(ppo))

#define IOBEXServer_AuthResponse(p, uid, pw) \
      AEEGETPVTBL((p),IOBEXServer)->AuthResponse((p), (uid), (pw))

#define IOBEXServer_GetSupportedTransports(p) \
      AEEGETPVTBL((p),IOBEXServer)->GetSupportedTransports((p))

#define IOBEXServer_Register(p,ri,pscn)                               \
      AEEGETPVTBL((p),IOBEXServer)->Register((p),(ri),(pscn))

#define IOBEXServer_Deregister(p)                               \
      AEEGETPVTBL((p),IOBEXServer)->Deregister((p))

#define IOBEXServer_Initialize(p, h)                               \
      AEEGETPVTBL((p),IOBEXServer)->Initialize((p),(h))

#define IOBEXServer_AcceptConnection(p,ba,hp,hc,cmdStatus)                               \
      AEEGETPVTBL((p),IOBEXServer)->AcceptConnection((p),(ba),(hp),(hc),(cmdStatus))

#define IOBEXServer_ConfirmSetPath(p, hp, hc, cmdStatus)                               \
      AEEGETPVTBL((p),IOBEXServer)->ConfirmSetPath((p), (hp), (hc), (cmdStatus))

#define IOBEXServer_PutResponse(p, hp, hc, cmdStatus)                               \
      AEEGETPVTBL((p),IOBEXServer)->PutResponse((p), (hp), (hc), (cmdStatus))

#define IOBEXServer_GetResponse(p, hp, hc, cmdStatus)                               \
      AEEGETPVTBL((p),IOBEXServer)->GetResponse((p), (hp), (hc), (cmdStatus))

#define IOBEXServer_ForceDisconnect(p)                               \
      AEEGETPVTBL((p),IOBEXServer)->ForceDisconnect((p))

#define IOBEXServer_Authenticate(p, pscn, realmInfo, uid, access) \
      AEEGETPVTBL((p),IOBEXServer)->Authenticate((p), (pscn), (realmInfo), (uid), (access))      

#define IOBEXServer_DisconnectResponse(p, hp, hc, cmdStatus)                               \
      AEEGETPVTBL((p),IOBEXServer)->DisconnectResponse((p), (hp), (hc), (cmdStatus))

#define IOBEXClient_AddRef(p)                               \
      AEEGETPVTBL((p),IOBEXClient)->AddRef((p))

#define IOBEXClient_Release(p)                               \
      AEEGETPVTBL((p),IOBEXClient)->Release((p))

#define IOBEXClient_QueryInterface(p,i,ppo)                               \
      AEEGETPVTBL((p),IOBEXClient)->QueryInterface((p),(i),(ppo))

#define IOBEXClient_Authenticate(p, addr, realmInfo, uid, access)                      \
      AEEGETPVTBL((p),IOBEXClient)->Authenticate((p), (addr), (realmInfo), (uid), (access))      

#define IOBEXClient_AuthResponse(p, uid, pw) \
      AEEGETPVTBL((p),IOBEXClient)->AuthResponse((p), (uid), (pw))

#define IOBEXClient_GetSupportedTransports(p) \
      AEEGETPVTBL((p),IOBEXClient)->GetSupportedTransports((p))

#define IOBEXClient_Connect(p, addr, hp, hc)                               \
      AEEGETPVTBL((p),IOBEXClient)->Connect((p), (addr), (hp), (hc))

#define IOBEXClient_Disconnect(p, hp, hc)                               \
      AEEGETPVTBL((p),IOBEXClient)->Disconnect((p),(hp),(hc))

#define IOBEXClient_Put(p, hp, hc, cmdStatus)                               \
      AEEGETPVTBL((p),IOBEXClient)->Put((p), (hp), (hc), (cmdStatus))

#define IOBEXClient_Get(p, hp, hc, final)                               \
      AEEGETPVTBL((p),IOBEXClient)->Get((p), (hp), (hc), (final))

#define IOBEXClient_SetPath(p, path, ul, dc, hp, hc)                               \
      AEEGETPVTBL((p),IOBEXClient)->SetPath((p), (path), (ul), (dc), (hp), (hc))

#define IOBEXClient_AbortOperation(p)                               \
      AEEGETPVTBL((p),IOBEXClient)->AbortOperation((p))




/*=======================================================================
   INTERFACE DOCUMENTATION
=========================================================================

Interface Name: IOBEX

Description: (Generic) OBEX Profile

  The use of IOBEX consists of sending commands and receiving a model change
  notification via IMODEL_NOTIFY. The user of this interface needs to do a
  IMODEL_AddListener() and provide a call back function.

  An OBEX Server application may register itself by creating an instance of IOBEX and calling
  IOBEX_ServerRegister(). When a remote client initiates a connection, an event is delivered
  to the application via ISHELL_NOTIFY(). The application therefore needs to register for
  notifications via ISHELL_RegisterNotify() and mask to use is NMASK_OBEX_GOEP.
  See AEEOBEXNotifier.h for more information on IOBEXNotifier.

  If a command returns SUCCESS, then the model listener is guaranteed to be notified.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event is AEE_OBEX_EVT_SRV_CON_REQ  and the notification mask
  is NMASK_OBEX_GOEP.
  The pData field of the AEENotify is a pointer to
  OBEXNotificationDataType. For more information about OBEXNotificationDataType, please review
  the comments for the data structure.

  Note that we only support one IOBEX object per target-ID. If server needs to
  support multiple targets, it needs to create an IOBEX obj per instance

  Steps to Register a server: (pseudo-code)
  1. ISHELL_CreateInstance(ishell, AEECLSID_IOBEXServer, &pIOBEXServer);
  2. AEEOBEXRegistrationInfoType reg;
     AEEOBEXByteSeqType target;

     char tgt[] = {'S','Y','N','C','M','L','-','S','Y','N','C'}
     target.len = 11;
     target.data = (byte*)tgt;
     reg.targetPtr = &target;
     reg.auth = AEE_OBEX_AUTH_NONE;
     reg.bReadOnly = FALSE;
     reg.transportConfig.transports = <all supported transports>
         AEE_OBEX_BLUETOOTH_TRANSPORT | AEE_OBEX_IRDA_TRANSPORT | AEE_OBEX_USB_TRANSPORT | AEE_OBEX_RS232_TRANSPORT;
     reg.transportConfig.bt_config.uuid = <some BT UUID>;
     reg.transportConfig.bt_config.security = AEEOBEX_BT_SEC_NONE;
  3. IOBEXServer_Register(pIOBEXServer, pReg);  <check the return value please!> 
      App may now exit 

  <Steps taken by the application when it receives the AEE_OBEX_EVT_SRV_CON_REQ event> 
  1.  check mask to ensure its NMASK_OBEX_GOEP and the evt is AEE_OBEX_EVT_SRV_CON_REQ
  2.  OBEXNotificationDataType *pData = (OBEXNotificationDataType*)aeenotify.pData;
      if (data.classID == <MYCLASSID>) this evt is meant for me
      {
          IOBEXServer *pIOBEXServer;
          ISHELL_CreateInstance(ishell, AEECLSID_IOBEXServer, &pIOBEXServer);
          IOBEXServer_Init(pIOBEXServer, pData.obexHandle); <retrieves the data associated with the registered server>
          <prompt user to see if they want to accept incoming connection>
          cmdStatus = AEE_OBEX_STAT_SUCCESS;
          bAccept = TRUE;
          <register a listener using IMODEL_AddListener()>
          IOBEXServer.AcceptConnection(pIOBEXServer, bAccept, cmdStatus);
          ...
          ...
      }

 Event                                   pData->
  -----                                  ----------------------------
  <For servers>
  AEE_OBEX_EVT_SRV_CON_REQ               connReq
  AEE_OBEX_EVT_SRV_AUTH_REQ              authReq    

  IMODEL Notifications:                  AEEOBEXModelNotificationDataType sent via IMODEL_Notify():
  ---------------------                  -----------------------------
  <For servers>
  AEE_OBEX_EVT_CONNECTED                 base.dwParam (0 =>connected/1 => failed)
  AEE_OBEX_EVT_SRV_DCN_IND               targetId
  AEE_OBEX_EVT_SRV_GET_REQ               accessReq
  AEE_OBEX_EVT_SRV_PUT_REQ               accessReq
  AEE_OBEX_EVT_SRV_SET_PATH_REQ          setPathReq

  <For clients>
  AEE_OBEX_EVT_CLI_CON_CFM               readOnly (true/false)
  AEE_OBEX_EVT_CLI_CONN_FAILED           base.dwParam contains the cmdStatus received from server
  AEE_OBEX_EVT_CLI_AUTH_REQ              authReq
  AEE_OBEX_EVT_CLI_AUTH_RESP             authResp
  AEE_OBEX_EVT_CLI_DCN_IND               no data. just the event is sent
  AEE_OBEX_EVT_CLI_PUT_CFM               accessReq
  AEE_OBEX_EVT_CLI_GET_RCV_DATA          accessReq
  AEE_OBEX_EVT_CLI_SET_PATH_CFM          base.dwParam contains the cmdStatus rx from server

Interfaces:


  IBASE  -                      AEE.h
    IOBEX_AddRef()
    IOBEX_Release()
  IQUERYINTERFACE -             AEE.h
    IOBEX_QueryInterface()

=======================================================================

Function: IOBEXServer_Register

Description:
  The application registers with IOBEX. Registering with IOBEX ensures that
  the server is ready to provide the OBEX service. When a remote client 
  initiates a connection, an EVT_NOTIFY is sent to the server. 
  The Server app in turn creates a new instance of the IOBEXServer object,
  and initializes it using the AEEHandle that it received via the ConnReq 
  struct in the NotificationData. The Server is then ready for handling 
  the OBEX connection and calls IOBEXServer_AcceptConnection()

Prototype:
  int IOBEXServer_Register(IOBEX *po, const AEEOBEXRegistrationInfoType  *pReg, uint8 *pScn)
Parameters
  po          - pointer to the IOBEX object
  pReg        - pointer to Registration info
  pScn        - pointer to the variable where Bluetooth RFCOMM server channel 
                number is returned (applicable to Bluetooth transport only)  

Return Value:

  SUCCESS   - the request to register was successful
  ENOMEMORY - could not complete registration due to error
  EBADPARM  - NULL pointers or unrecognized supported formats
  EFAILED   - Failed due to either out of resources, out of RFCOMM channels, or
              some other error
Event(s):
None
=======================================================================
Function: IOBEXServer_Deregister

Description:
  Removes the registration from IOBEX. At this point, the server
  will no longer receive an event notifications for CONN_REQ
  If the server is processing a connection, then the connection is broken by IOBEXServer
  implicitly via ForceDisconnect(). The app needs to  remove its model listener
  and release the IOBEX object.
Prototype:
  int IOBEXServer_Deregister(IOBEX* po);

Parameters
  po          - pointer to the IOBEX object

Return Value:
  SUCCESS   - the request to deregister with SDDB was queued
  ENOMEMORY - could not complete request due to lack of memory
  EFAILED   - unexpected error
Event(s):
  None
=======================================================================
Function: IOBEXServer_Initialize

Description:
   Used by a server application to initialize the newly created IOBEX object
   using the obex handle that was returned in the event. This handle
   is used to associate the new IOBEX with the data associated with the
   IOBEX object that was used to register the service in the first place
Prototype:
  int IOBEXServer_Initialize(IOBEX* po, uint32 obexHandle);

Parameters
  po          - pointer to the IOBEX object
  obexHandle  - the obex handle associated with the IOBEX object used to
                register the service
Return Value:
  SUCCESS   - the request to deregister with SDDB was queued
  ENOMEMORY - could not complete request due to lack of memory
  EFAILED   - unexpected error
Event(s):
  None
=======================================================================
Function: IOBEXServer_AcceptConnection

Description:
  The server application must issue this command to accept or reject a
  connection request from an OBEX client.

Prototype:
   int IOBEXServer_AcceptConnection(IOBEX *po,
                              boolean bAccept, 
                              AEEOBEXHdrType *headersPtr,
                              uint8 headersCnt,
                              AEEOBEXCmdStatusType cmdStatus);
Parameters
  po          - pointer to the IOBEX object
  bAccept     - TRUE to indicate server accepted connection request
  headersPtr  - the pointer to the headers to be sent to the OBEX client. 
  headersCnt  - the number of headers contained in the memory region at headersPtr
                if headersPtr is NULL, the headersCnt must be zero.
  cmdStatus   - Status of the connect request

Return Value:
  SUCCESS   - the request to accept/reject the connection request was queued
              and that AEE_OBEX_EVT_CONNECTED event will be generated at a
              later time (if the server accepted the connection.)
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Model Callback:
  The application will receive a Model change notification with the payload
  indicating the event: AEE_OBEX_EVT_CONNECTED with the accompanying status
  in the ModelEvent's dwParam (1 == failed, 0 == connected)
=======================================================================
Function: IOBEXServer_ConfirmSetPath

Description:
   Server sends the response to the SETPATH request using this command

Prototype:
  int IOBEXServer_ConfirmSetPath(IOBEX *po, 
                                AEEOBEXHdrType   *headersPtr,
                                uint8   headersCnt,
                                AEEOBEXCmdStatusType cmdStatus);

Parameters
  po          - pointer to the IOBEX object
  headersPtr  - the pointer to the headers to be sent to the OBEX client. Note that headers
                are optional in the case of a ConfirmSetPath. So, this pointer may be NULL
  headersCnt  - the number of headers contained in the memory region at headersPtr
                if headersPtr is NULL, the headersCnt must be zero.
  cmdStatus   - the status of the requested operation

Return Value:
  SUCCESS - all is well. the request has been queued
  FAILED - unable to queue the command

Event(s):
  None
=======================================================================
Function: IOBEXServer_PutResponse

Description:
  Server responds to a PUT request using this command
Prototype:
  int IOBEXServer_PutResponse(IOBEX *po, AEEOBEXHdrType *headersPtr,
                         uint8   headersCnt,
                         AEEOBEXCmdStatusType cmdStatus);
Parameters
  po          - pointer to the IOBEX object
  headersPtr  - the pointer to the headers to be sent to the OBEX client. Note that headers
                are optional in the case of a PutResponse. So, this pointer may be NULL
  headersCnt  - the number of headers contained in the memory region at headersPtr
                if headersPtr is NULL, the headersCnt must be zero.
  cmdStatus   - the status of the requested operation

Return Value:
  SUCCESS - all is well. the request has been queued
  FAILED - unable to queue the command

Event(s):
  None
=======================================================================
Function: IOBEXServer_GetResponse

Description:
  Server responds to the GET request using this command

Prototype:
   int IOBEXServer_GetResponse(IOBEX *po, AEEOBEXHdrType  *headersPtr,
                         uint8   headersCnt,
                         AEEOBEXCmdStatusType cmdStatus);
Parameters
  po          - pointer to the IOBEX object
  headersPtr  - the pointer to the headers to be send to the OBEX client.
                Must NOT be NULL
  headersCnt  - the number of headers contained in the memory region at headersPtr
  cmdStatus   - the status of the requested operation

Return Value:
  SUCCESS - all is well. the request has been queued
  FAILED - unable to queue the command

Event(s):
  None
=======================================================================
Function: IOBEXServer_ForceDisconnect

Description:
  Server may forcibly sever the connection with the client using this function

Prototype:
   int IOBEXServer_ForceDisconnect(IOBEX *po);
Parameters
  po          - pointer to the IOBEX object
Return Value:
  SUCCESS   - the request to disconnect from remote device was queued and that
              AEE_OBEX_EVT_SRV_DCN_IND event will be sent at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Model Callback:
  The application will receive a Model change notification with the payload
  indicating the event: AEE_OBEX_EVT_SRV_DCN_IND
=======================================================================
Function: IOBEXServer_Authenticate

Description:
  The Server application should use this API before calling the accept 
  connect(but after server registration) API inorder to store the realm
  information that would be needed to send to the client dring authentication.
  If the authentication information has the descption string(REALM) for
  the user id and the password, the same needs to be updated using this
  API.

Prototype:
  int IOBEXServer_Authenticate(IOBEX *po,  
                                AEEOBEXDeviceAddressType *pAddr,
                                AEEOBEXRealm *realmInfo,
                                boolean bUidReqd,
                                boolean bFullAccess);
Parameters
  po               - pointer to the IOBEX object
  pAddr            - The address of the device to connect to.
  realmInfo        - The address of the structure that contains the
                     realm infomation.
  bUidReqd         - userID Reqd.
                     = TRUE if the server requires UserID from the client
                     = FALSE, if the server does not require UID from the client.
  bFullAccess      - Type of Access
                    = TRUE - Full access is required
                    = FALSE - Readonly access.
                     
Return Value:
  SUCCESS    - The information is stored successfully in the database
  EFAILED    - No resources to store the information.

Event(s):
    None

=======================================================================
Function: IOBEXServer_AuthResponse

Description:
  Server application can use this command to authenticate the
  connection being brought up.
  
  Server application should issue this command when receiving a connection
  request.

Prototype:
   int IOBEXServer_AuthResponse(IOBEX *po , const AECHAR *pwUserID,
                          const AECHAR *pwPassword, boolean readOnly);
Parameters
  po          - pointer to the IOBEX object
  pwUserID    - userID
  pwPassword  - password for userID
  
Return Value:
  SUCCESS   - the request to authenticate the connection being brought up was
              queued and that an appropriate event will be generated at a
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Model Callback:
  For server, if the authentication passes, the event AEE_OBEX_EVT_SRV_CON_REQ
  will be sent again with bUnauthorized set to FALSE.  Server can then
  accept or reject the connection request.

=======================================================================
Function: IOBEXServer_DisconnectResponse

Description:
  Server application can use this command to disconnect
  OBEX connection.
  
  Server application should issue this command when it gets 
  AEE_OBEX_EVT_SRV_DCN_IND model event request.

Prototype:
   int IOBEXServer_DisconnectResponse(IOBEX *po, AEEOBEXHdrType *headersPtr,
                                    uint8   headersCnt,
                                    AEEOBEXCmdStatusType cmdStatus);
Parameters
  po          - pointer to the IOBEX object
  headersPtr  - the pointer to the headers to be sent to the OBEX client. Note that headers
                are optional in the case of a DisconnectResponse. So, this pointer may be NULL
  headersCnt  - the number of headers contained in the memory region at headersPtr
                if headersPtr is NULL, the headersCnt must be zero.
  cmdStatus   - the status of the requested operation

Return Value:
  SUCCESS - all is well. the request has been queued
  FAILED - unable to queue the command

Model Callback:
  There is no model call back, once the application calls API, it
  shall disconnect if this API returns SUCCESS. 
  
=======================================================================
Function: IOBEXServer_GetSupportedTransports

Description:
   This function is called by server application to retrieve the supported
   transports. (BT, IrDA, USB, RS232 etc)

Prototype:
   int IOBEXServer_GetSupportedTransports(IOBEX *po);

Parameters
  po                    - pointer to the IOBEX object


Return Value:
  AEEOBEXTransportConfigType

Event(s):
None
=======================================================================
Function: IOBEXClient_Connect

Description:
   Called by client application to connect to remote OBEX server
Prototype:
  int IOBEXClient_Connect(IOBEX *po,
                    const AEEOBEXDeviceAddressType *pAddr,
                    AEEOBEXHdrType  *headersPtr,
                    uint8            headersCnt);

Parameters
  po          - pointer to the IOBEX object
  pAddr       - The address of the device to connect to.
  headersPtr  - The headers to be sent for setting up the connection
                the application may free this ptr as soon as the function returns
  headersCnt  - The number of headers
Return Value:

  SUCCESS   - the request to connect was queued and
              an appropriate event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or invalid channel number

Model Callback:
  If remote server requires authentication, the event
  AEE_OBEX_EVT_CLI_AUTH_REQ will be sent to get user ID and
  password from the app.

  If no authentication is required, either AEE_OBEX_EVT_CLI_CON_CFM or
  AEE_OBEX_EVT_CLI_CONN_FAILED will be sent depending on whether the
  server accepted or rejected the connection request

=======================================================================
Function: IOBEXClient_Disconnect

Description:
    The client may use this function to bring down the OBEX connection

Prototype:
   int IOBEXClient_Disconnect(IOBEX *po, 
                              AEEOBEXHdrType  *headersPtr,
                              uint8 headersCnt);
Parameters
  po          - pointer to the IOBEX object
  headersPtr  - The pointer to the headers to be send to the OBEX client.
  headersCnt  - The number of headers
                if headersPtr is NULL, the headersCnt must be zero.

Return Value:
  SUCCESS   - the request to disconnect from remote device was queued and that
              AEE_OBEX_EVT_CLI_DCN_IND event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Model Callback:
  AEE_OBEX_EVT_CLI_DCN_IND
=======================================================================
Function: IOBEXClient_Put

Description:
   This command is used by a client application to perform an OBEX PUT
   operation. It should only be called after app receives the
   AEE_OBEX_EVT_CLI_CON_CFM event

Prototype:
   int IOBEXClient_Put(IOBEX *po,
                 AEEOBEXHdrType  *headersPtr,
                 uint8  headersCnt,
                 AEEOBEXCmdStatusType cmdStatus);
Parameters
  po          - pointer to the IOBEX object
  headersPtr  - pointer to the headers to be sent
                the application may free this ptr as soon as the function returns
  headersCnt  - number of headers to copy
  cmdStatus   - the status of the requested operation

Return Value:

  SUCCESS   - the request to copy a file/folder to the remote device was queued
              and that AEE_OBEX_EVT_CLI_PUT_CFM event will be sent at a
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Model Callback:
 AEE_OBEX_EVT_CLI_PUT_CFM with the following status codes: (in dwParam of ModelEvent)
  AEE_OBEX_ERR_NONE
  AEE_OBEX_ERR_NOT_ALLOWED
=======================================================================
Function: IOBEXClient_Get

Description:
    This command is used by a client application to perform an OBEX GET
   operation. It should only be called after app receives the
   AEE_OBEX_EVT_CLI_CON_CFM event
Prototype:
   int IOBEXClient_Get(IOBEX *po,
                 AEEOBEXHdrType  *headersPtr,
                 uint8  headersCnt,
                 boolean final);
Parameters
  po          - pointer to the IOBEX object
  headersPtr  - pointer to the headers to be  sent
                the application may free this data as soon as the function returns
  headersCnt  - number of headers to copy
  final       - flag indicates that this is the final datablock of GET
                operation
Return Value:
 SUCCESS   - the request to copy a file/folder from the remote device was
              queued and that AEE_OBEX_EVT_CLI_GET_RCV_DATA event will be
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Model Callback:
  AEE_OBEX_EVT_CLI_GET_RCV_DATA with the following status code (in dwParam of ModelEvent)
  AEE_OBEX_ERR_NONE  or AEE_OBEX_ERR_FAILED

=======================================================================
Function: IOBEXClient_SetPath

Description:
  Client application can issue this command to set the path on the remote
  server for subsequent put/get operations

Prototype:
   int IOBEXClient_SetPath(IOBEX *po,
                     const AECHAR *pwPathName,
                     boolean upLevel,
                     boolean dontCreate,
                     AEEOBEXHdrType *headersPtr,
                     uint8 headersCnt);
Parameters
  po          - pointer to the IOBEX object
  pwPathName  - the directory path to change to
  upLevel     - cd up or down or root
  dontCreate  - flag indicating whether to create a directory
                (if it doesnt exist)
  headersPtr  - pointer to the headers to be  sent
                the application may free this data as soon as the function returns
  headersCnt  - number of headers to copy
                
Return Value:
  SUCCESS   - the request to set the path on the remote server was queued
              and that AEE_OBEX_EVT_CLI_SET_PATH_CFM event will be generated
              at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Model Callback:
  AEE_OBEX_EVT_CLI_SET_PATH_CFM with one of the following status codes: (in dwParam of ModelEvent)
  AEE_OBEX_ERR_NONE or AEE_OBEX_ERR_FAILED
=======================================================================
Function: IOBEXClient_AbortOperation

Description:
  Client application can issue this command to abort the current operation.
Prototype:
   int IOBEX_ClientAbortOperation(IOBEX *po);
Parameters
  po          - pointer to the IOBEX object

Return Value:
  SUCCESS   - the current operation has been aborted.
  EIDLE     - no operation in progress
  EBADSTATE - no connection

Event(s):
  None

=======================================================================
Function: IOBEXClient_Authenticate

Description:
  The Client application should use this API before calling the connect
  API inorder to indicate to the obex client that an authentication 
  procedure should be initiated suring connection.
  If the authentication information has the descption string(REALM) for
  the user id and the password, the same needs to be updated using this
  API.

  Note: This API does not authenticte the server. It takes the data that 
  is needed to authenticate the server.
  Client application should call this API just before it calls the
  IOBEXClient_Connect API.

Prototype:
  int IOBEXClient_Authenticate(IOBEX *po, 
                                AEEOBEXDeviceAddressType *pAddr,
                                AEEOBEXRealm *realmInfo,
                                boolean bUidReqd,
                                boolean bFullAccess);
Parameters
  po               - pointer to the IOBEX object
  pAddr            - The address of the device to connect to.
  realmInfo        - The address of the structure that contains the
                     realm in fomation.
  bUidReqd         - userID Reqd.
                     = TRUE if the client requires UserID from the server
                     = FALSE, if the server does not require UID from the server.
  bFullAccess      - Type of Access
                    = TRUE - Full access is required
                    = FALSE - Readonly access.

Return Value:
  SUCCESS    - The information is stored successfully in the database
  EFAILED    - No resources to store the information.

Event(s):
 None
=======================================================================

Function: IOBEXClient_GetSupportedTransports

Description:
   This function is called by client application to retrieve the supported
   transports. (BT, IrDA, USB, RS232 etc)

Prototype:
   int IOBEXClient_GetSupportedTransports(IOBEX *po);

Parameters
  po                    - pointer to the IOBEX object


Return Value:
  AEEOBEXTransportConfigType

Event(s):
None
=======================================================================

Function: IOBEXClient_AuthResponse

Description:
  Client application can use this command to authenticate the
  connection being brought up.

  Client application should issue this command in response to event
  AEE_OBEX_EVT_CLI_AUTH_REQ.

Prototype:
   int IOBEXClient_AuthResponse(IOBEX *po , const AECHAR *pwUserID,
                          const AECHAR *pwPassword);
Parameters
  po          - pointer to the IOBEX object
  pwUserID    - userID
  pwPassword  - password for userID
  
Return Value:
  SUCCESS   - the request to authenticate the connection being brought up was
              queued and that an appropriate event will be generated at a
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Model Callback:
  For client, depending on whether server accepts the connection request,
  either the event AEE_OBEX_EVT_CLI_CON_CFM or AEE_OBEX_EVT_CLI_CONN_FAILED
  will be sent

=======================================================================*/
#endif /*_AEE_OBEX_H */
