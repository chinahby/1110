#ifndef _AEE_BT_EXT_FTP_H_
#define _AEE_BT_EXT_FTP_H_

/*===========================================================================
FILE:      AEEBTExtFTP.h

SERVICES:  BlueTooth File Transfer Profile Interface

GENERAL DESCRIPTION: BREW interface to BT FTP

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtFTP

        Copyright © 2004-2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEFile.h"

// BREW generated ID
#define AEECLSID_BLUETOOTH_FTP      0x010220b9

// event types returned via HandleEvent()
enum
{
  /* for servers */
  AEEBT_FTP_EVT_REG_DONE,       // registration process done
  AEEBT_FTP_EVT_DEREG_DONE,     // deregistration process done
  AEEBT_FTP_EVT_CONN_REQ,       // a client wants to connect
  AEEBT_FTP_EVT_CD_REQ,         // set path request received from remote device
  AEEBT_FTP_EVT_LS_REQ,         // folder list req received from remote device
  AEEBT_FTP_EVT_PUT_REQ,        // client copying a file/folder to server
  AEEBT_FTP_EVT_GET_REQ,        // client copying a file/folder from server
  AEEBT_FTP_EVT_DELETE_REQ,     // client deleting a file/folder from server

  /* for clients */
  AEEBT_FTP_EVT_AUTH_REQ,       // server wants authentication
  AEEBT_FTP_EVT_ABORTED,        // client abort done

  /* common to servers and clients */
  AEEBT_FTP_EVT_CONNECTED,      // connection is up
  AEEBT_FTP_EVT_CONN_FAILED,    // connection could not be brought up
  AEEBT_FTP_EVT_DISCONNECTED,   // connection is down
  AEEBT_FTP_EVT_PATH_SET,       // path is set on remote server
  AEEBT_FTP_EVT_FOLDER_CREATED, // a new folder created on remote server
  AEEBT_FTP_EVT_FOLDER_LISTING, // content of current folder on remote server
  AEEBT_FTP_EVT_PUT_DNE,        // file/folder copied to remote device
  AEEBT_FTP_EVT_GET_DNE,        // file/folder copied from remote device
  AEEBT_FTP_EVT_DELETE_DNE,     // file/folder deleted from remote device
  AEEBT_FTP_EVT_PROGRESS_IND    // data transfer progress indication
};

// error codes for AEEBT_FTP_EVT_*
// DEPRECATED - DO NOT ADD ERROR CODES TO THIS ENUMERATION!
enum
{
  // DEPRECATED
  AEEBT_FTP_ERR_NONE          = AEEBT_ERR_NONE,            // no worry
  AEEBT_FTP_ERR_BAD_PARAM     = AEEBT_ERR_BAD_PARAM,       // for any function
  AEEBT_FTP_ERR_RESOURCE      = AEEBT_ERR_RESOURCE,        // lack of resource (memory, ...)
  AEEBT_FTP_ERR_DB_FULL       = AEEBT_ERR_SD_DB_FULL,      // SDDB is full
  AEEBT_FTP_ERR_REC_EXISTS    = AEEBT_ERR_SD_REC_EXISTS,   // server record already in SDDB
  AEEBT_FTP_ERR_REC_NOT_FOUND = AEEBT_ERR_SD_REC_NOT_FOUND,// server record not found in SDDB
  AEEBT_FTP_ERR_OBJ_NOT_FOUND = AEEBT_ERR_OBEX_NOT_FOUND,  // defined but never used
  AEEBT_FTP_ERR_OBJ_EXISTS    = AEEBT_ERR_UNKNOWN,         // defined but never used
  AEEBT_FTP_ERR_NOT_ALLOWED   = AEEBT_ERR_OBEX_METHOD_NOT_ALLOWED,// defined but never used 
  AEEBT_FTP_ERR_FAILED        = AEEBT_ERR_UNKNOWN          // defined but never used
  // DEPRECATED
};

enum
{
  AEEBT_FTP_AUTH_NONE,
  AEEBT_FTP_AUTH_PASSWORD,
  AEEBT_FTP_AUTH_USERID_AND_PASSWORD
};
typedef uint8 AEEBTFTPAuthentication;

enum
{
  AEEBT_FTP_REQ_NONE,
  AEEBT_FTP_REQ_CD,   // set path
  AEEBT_FTP_REQ_LS,   // list folder
  AEEBT_FTP_REQ_PUT,  // put
  AEEBT_FTP_REQ_GET,  // get
  AEEBT_FTP_REQ_DEL   // delete
};
typedef uint8 AEEBTFTPRequestType;

enum
{
  AEEBT_FTP_STAT_SUCCESS,
  AEEBT_FTP_STAT_NOT_ALLOWED,
  AEEBT_FTP_STAT_FILESYS_FULL,
  AEEBT_FTP_STAT_EXISTS,
  AEEBT_FTP_STAT_NOT_FOUND,
  AEEBT_FTP_STAT_BAD_REQ
};
typedef uint8 AEEBTFTPStatus;

typedef struct
{
  char                    szServiceName[ AEEBT_MAX_SERVICENAME_LEN ];
  AEEBTFTPAuthentication  auth;
  boolean                 bReadOnly;
  boolean                 bBrowseAllowed;
} AEEBTFTPRegistration;

typedef struct _IBTExtFTP IBTExtFTP;

AEEINTERFACE(IBTExtFTP)
{
  INHERIT_IQueryInterface(IBTExtFTP);

  /* for servers */
  int (*Register)(IBTExtFTP* po, const AEEBTFTPRegistration* pReg);
  int (*Deregister)(IBTExtFTP* po);
  int (*AcceptConnection)(IBTExtFTP* po, const AEEBTBDAddr* pBDAddr, 
                          boolean bAccept, const AECHAR* pwRootFolder);
  int (*SendResult)(IBTExtFTP* po, AEEBTFTPRequestType reqType, 
                    AEEBTFTPStatus result, const AECHAR* pwName);

  /* for clients */
  int (*Connect)(IBTExtFTP* po, const AEEBTBDAddr* pBDAddr, 
                 uint8 uChannelNumber);
  int (*Abort)(IBTExtFTP* po);
  int (*SetPath)(IBTExtFTP* po, const AECHAR* pwPathName,         // cd
                 AEEBTFTPSetPathFlag flag);
  int (*CreateFolder)(IBTExtFTP* po, const AECHAR* pwDirName);    // md
  int (*ListFolder)(IBTExtFTP* po, const AECHAR* pwDirName,       // ls
                    const AECHAR* pwDirList);
  int (*Put)(IBTExtFTP* po, AEEBTFTPObject objType,               // put
             const AECHAR* pwLocalName, const AECHAR* pwRemoteName );
  int (*Get)(IBTExtFTP* po, AEEBTFTPObject objType,               // get
             const AECHAR* pwLocalName,
             const AECHAR* pwRemoteName );
  int (*Delete)(IBTExtFTP* po, const AECHAR* pwRemoteName);       // rm

  /* common to servers and clients */
  int (*Authenticate)(IBTExtFTP* po, const AEEBTBDAddr* pBDAddr, 
                      const AECHAR* pwUserID, const AECHAR* pwPassWord);
  int (*Disconnect)(IBTExtFTP* po);

  /* data needed to know if the authentication is enabled in the case 
   * of the client.
   */  
  int (*AuthData)(IBTExtFTP* po, const AEEBTFTPRegistration* pReg);
  int (*GetProgressInfo)(IBTExtFTP* po, AEEBTProgressInfo* pProgressInfo);
  
};


#define IBTEXTFTP_AddRef(p)                               \
        AEEGETPVTBL((p),IBTExtFTP)->AddRef((p))

#define IBTEXTFTP_Release(p)                              \
        AEEGETPVTBL((p),IBTExtFTP)->Release((p))

#define IBTEXTFTP_QueryInterface(p,i,ppo)                 \
        AEEGETPVTBL((p),IBTExtFTP)->QueryInterface((p),(i),(ppo))

#define IBTEXTFTP_Register(p,pr)                          \
        AEEGETPVTBL((p),IBTExtFTP)->Register((p),(pr))

#define IBTEXTFTP_Deregister(p)                           \
        AEEGETPVTBL((p),IBTExtFTP)->Deregister((p))

#define IBTEXTFTP_AcceptConnection(p,pa,b,pr)             \
        AEEGETPVTBL((p),IBTExtFTP)->AcceptConnection((p),(pa),(b),(pr))

#define IBTEXTFTP_SendResult(p,rq,rs,pf)                  \
        AEEGETPVTBL((p),IBTExtFTP)->SendResult((p),(rq),(rs),(pf))
        
#define IBTEXTFTP_Connect(p,pa,c)                         \
        AEEGETPVTBL((p),IBTExtFTP)->Connect((p),(pa),(c))

#define IBTEXTFTP_Abort(p)                                \
        AEEGETPVTBL((p),IBTExtFTP)->Abort((p))

#define IBTEXTFTP_SetPath(p,pd,f)                         \
        AEEGETPVTBL((p),IBTExtFTP)->SetPath((p),(pd),(f))

#define IBTEXTFTP_CreateFolder(p,pd)                      \
        AEEGETPVTBL((p),IBTExtFTP)->CreateFolder((p),(pd))

#define IBTEXTFTP_ListFolder(p,pd,pf)                     \
        AEEGETPVTBL((p),IBTExtFTP)->ListFolder((p),(pd),(pf))

#define IBTEXTFTP_Put(p,t,pl,pr)                          \
        AEEGETPVTBL((p),IBTExtFTP)->Put((p),(t),(pl),(pr))

#define IBTEXTFTP_Get(p,t,pl,pr)                          \
        AEEGETPVTBL((p),IBTExtFTP)->Get((p),(t),(pl),(pr))

#define IBTEXTFTP_Delete(p,pn)                            \
        AEEGETPVTBL((p),IBTExtFTP)->Delete((p),(pn))

#define IBTEXTFTP_Authenticate(p,pa,id,pw)                \
        AEEGETPVTBL((p),IBTExtFTP)->Authenticate((p),(pa),(id),(pw))

#define IBTEXTFTP_ListLocalFolder(p,pd,pf)                \
        AEEGETPVTBL((p),IBTExtFTP)->ListLocalFolder((p),(pd),(pf))

#define IBTEXTFTP_Disconnect(p)                           \
        AEEGETPVTBL((p),IBTExtFTP)->Disconnect((p))

#define IBTEXTFTP_AuthData(p,pr)                          \
        AEEGETPVTBL((p),IBTExtFTP)->AuthData((p),(pr))

#define IBTEXTFTP_GetProgressInfo(p,i)                          \
        AEEGETPVTBL((p),IBTExtFTP)->GetProgressInfo((p),(i))
/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtFTP

Description: BlueTooth File Transfer Profile

  Consult the Bluetooth ISOD for description of FTP.
  
  The use of IBTExtFTP consists of sending commands and receiving events.  
  These events are delivered to IBTExtFTP clients as notifications, hence
  the clients must register for FTP notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_FTP.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.  However, not all events are associated with 
  a command.  Some events are asyncronously generated such as a pin request
  sent by remote device.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_FTP_EVT_* events when the notification mask
  is NMASK_BT_FTP.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                                   pData->
  -----                                   ----------------------------
  AEEBT_FTP_EVT_REG_DONE                  uError
  AEEBT_FTP_EVT_DEREG_DONE                uError
  AEEBT_FTP_EVT_CONN_REQ                  ConnReq
  AEEBT_FTP_EVT_CD_REQ                    AccessReq
  AEEBT_FTP_EVT_LS_REQ                    AccessReq
  AEEBT_FTP_EVT_MD_REQ                    AccessReq
  AEEBT_FTP_EVT_PUT_REQ                   AccessReq
  AEEBT_FTP_EVT_GET_REQ,                  AccessReq
  AEEBT_FTP_EVT_DELETE_REQ                AccessReq

  AEEBT_FTP_EVT_AUTH_REQ                  bUserIDRequired

  AEEBT_FTP_EVT_CONNECTED                 bdAddr - BD addr of remote device
  AEEBT_FTP_EVT_CONN_FAILED               bdAddr - BD addr of remote device
  AEEBT_FTP_EVT_DISCONNECTED              bdAddr - BD addr of remote device
  AEEBT_FTP_EVT_PATH_SET                  uError
  AEEBT_FTP_EVT_FOLDER_CREATED            uError
  AEEBT_FTP_EVT_FOLDER_LISTING            uError
  AEEBT_FTP_EVT_PUT_DNE                   uError
  AEEBT_FTP_EVT_OBJ_GET                   uError
  AEEBT_FTP_EVT_DELETE_DNE                uError
  AEEBT_FTP_EVT_PROGRESS_IND              ProgressInfo
  
Interfaces:

  IBTEXTFTP provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTFTP_AddRef()
    IBTEXTFTP_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTFTP_QueryInterface()

=======================================================================

Function:  IBTEXTFTP_Register()

Description:
  Server application uses this command to initialize the FTP server and 
  registers it in the service discovery database (SDDB.)

Prototype:
  int IBTEXTFTP_Register( IBTExtFTP*                  po,
                          const AEEBTFTPRegistration* pReg );

Parameters:
  po        - pointer to the IBTExtFTP object
  pReg      - pointer to registration info 

Return value:
  SUCCESS   - the request to register with SDDB was queued and that
              AEEBT_FTP_EVT_REG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or unrecognized supported formats

Event:
  AEEBT_FTP_EVT_REG_DONE with accompanying error code of one of the
  following values:
    AEEBT_FTP_ERR_NONE
    AEEBT_FTP_ERR_REC_EXISTS
=======================================================================

Function:  IBTEXTFTP_Deregister()

Description: 
  Terminates the FTP server and deregisters it from the SDDB.

Prototype:
  int IBTEXTFTP_Deregister( IBTExtFTP* po );

Parameters:
  po           - pointer to the IBTExtFTP object

Return value:
  SUCCESS   - the request to deregister with SDDB was queued and that
              AEEBT_FTP_EVT_DEREG_DONE event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory

Event:
  AEEBT_FTP_EVT_DEREG_DONE with accompanying error code of one of the
  following values:
    AEEBT_FTP_ERR_NONE
    AEEBT_FTP_ERR_REC_NOT_FOUND

=======================================================================

Function:  IBTEXTFTP_AcceptConnection()

Description: 
  The server application must issue this command to accept or reject a 
  connection request from a client.

Prototype:
  int IBTEXTFTP_AcceptConnection( IBTExtFTP* po, 
                                  const AEEBTBDAddr* pBDAddr,
                                  boolean bAccept,
                                  const AECHAR* pwRoot );

Parameters:
  po        - pointer to the IBTEXTFTP object
  pBDAddr   - BD address of the requesting client
  bAccept   - TRUE to indicate server accepted connection request
  pwRoot    - root folder; ignored if bAccept is FALSE

Return value:
  SUCCESS   - the request to accept/reject the connection request was queued
              and that AEEBT_FTP_EVT_CONNECTED event will be generated at a 
              later time (if the server accepted the connection.)
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers

Event:
  AEEBT_FTP_EVT_CONNECTED with the accompanying error code AEEBT_FTP_ERR_NONE.

=======================================================================

Function:  IBTEXTFTP_SendResult()

Description: 
  The server application issues this command to inform the driver of the result
  of the current operation.

Prototype:
  int IBTEXTFTP_SendResult( IBTExtFTP*          po, 
                            AEEBTFTPRequestType reqType, 
                            AEEBTFTPStatus      result, 
                            const AECHAR*       pwName );

Parameters:
  po        - pointer to the IBTEXTFTP object
  reqType   - to what request (put/get/delete/...) this result is
  result    - result of the requested operation
  pwName    - file/folder on which put/get/delete was performed, or
              file containing folder listing as result of browse folder request, 
              or folder created by make folder request,
              or current folder selected by set path request

Return value:
  SUCCESS   - the driver has been informed of the result of the last operation
  EFAILED   - invalid object

Event:
  None.

=======================================================================

Function:  IBTEXTFTP_Connect()

Description:
  Called by client application to connect to remote FTP server.

Prototype:

  int IBTEXTFTP_Connect( IBTExtFTP* po, const AEEBTBDAddr* pBDAddr, 
                         uint8 uChannelNumber );

Parameters:

  po              - pointer to the IBTExtFTP object
  pBDAddr         - BD address of FTP server
  uChannelNumber  - the server channel number to use to establish connection

Return value:
  SUCCESS   - the request to connect to FTP server was queued and that
              an appropriate event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointers or invalid channel number

Event:
  If server requires authentication, the event AEEBT_FTP_EVT_AUTH_REQ will
  be generated to get user ID and password from the app.
  
  If no authentication is required, either AEEBT_FTP_EVT_CONNECTED or
  AEEBT_FTP_EVT_CONN_FAILED will be generated, depending on whether the
  server accepts the connection request.

=======================================================================

Function:  IBTEXTFTP_Abort()

Description:
  Client application can issue this command to abort the current operation.

Prototype:
  int IBTEXTFTP_Abort( IBTExtFTP* po );

Parameters:
  po          - pointer to the IBTExtFTP object

Return value:
  SUCCESS   - the current operation has been queued and AEEBT_FTP_EVT_ABORTED 
              event will be generated at a later time.
  EIDLE     - no operation in progress
  EBADSTATE - no connection
  
Event:
  AEEBT_FTP_EVT_ABORTED with error code
    AEEBT_ERR_NONE
    AEEBT_ERR_NOT_CONNECTED
    AEEBT_ERR_INVALID_OPERATION

=======================================================================

Function:  IBTEXTFTP_SetPath()

Description:
  Client application can issue this command to set the path on the remote 
  server for subsequent put/get/delete/browse/create operations.
  
Prototype:
  int IBTEXTFTP_SetPath( IBTExtFTP* po, const AECHAR* pwPathName,
                         AEEBTFTPSetPathFlag flag );

Parameters:
  po          - pointer to the IBTExtFTP object
  pwPathName  - path name to change to; ignored for parent and root
  flag        - to which direction to set path (down/up/root)

Return value:
  SUCCESS   - the request to set the path on the remote server was queued 
              and that AEEBT_FTP_EVT_PATH_SET event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_FTP_EVT_PATH_SET with the one of the error codes:
    AEEBT_FTP_ERR_NONE
    AEEBT_FTP_ERR_NOT_FOUND

=======================================================================

Function:  IBTEXTFTP_CreateFolder()

Description:
  Client application can issue this command to create a folder off of the
  current folder on the remote server.

Prototype:
  int IBTEXTFTP_CreateFolder( IBTExtFTP* po, const AECHAR* pwFolder );

Parameters:
  po        - pointer to the IBTExtFTP object
  pwFolder  - name of folder to create

Return value:
  SUCCESS   - the request to create folder on the remote server was queued 
              and that AEEBT_FTP_EVT_FOLDER_CREATED event will be generated 
              at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_FTP_EVT_FOLDER_CREATED with the one of the error codes:
    AEEBT_FTP_ERR_NONE
    AEEBT_FTP_ERR_NOT_ALLOWED
    AEEBT_FTP_ERR_OBJ_EXISTS

=======================================================================

Function:  IBTEXTFTP_ListFolder()

Description:
  Client application can issue this command to get the listing of a given
  folder on the remote server.  If no folder name is given, listing of the
  current folder on remote server will be returned.

Prototype:
  int IBTEXTFTP_ListFolder( IBTExtFTP* po, const AECHAR* pwFolder,
                            const AECHAR* pwDirList );

Parameters:
  po        - pointer to the IBTExtFTP object
  pwFolder  - name of folder to list; NULL to indicate current folder
  pwDirList - name of file to store folder listing

Return value:
  SUCCESS   - the request to list folder on the remote server was queued 
              and that AEEBT_FTP_EVT_FOLDER_LISTING event will be generated 
              at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_FTP_EVT_FOLDER_LISTING with the one of the error codes:
    AEEBT_FTP_ERR_NONE
    AEEBT_FTP_ERR_NOT_FOUND
    AEEBT_FTP_ERR_RESOURCE

=======================================================================

Function:  IBTEXTFTP_Put()

Description:
  This command can be used by server and client applications.  It should 
  only be called after AEEBT_FTP_EVT_CONNECTED is received.

  Client application issues this command to copy a file/folder to the server.
  
  Server application uses it to send the file/folder that the client is trying
  to get.

Prototype:
  int IBTEXTFTP_Put( IBTExtFTP* po, AEEBTFTPObject objType,
                     const AECHAR* pwLocalName, 
                     const AECHAR* pwRemoteName );

Parameters:
  po            - pointer to the IBTExtFTP object
  objType       - file or folder
  pwLocalName   - name of file/folder in local device
  pwRemoteName  - name of file/folder in remote device

Return value:
  SUCCESS   - the request to copy a file/folder to the remote device was queued 
              and that AEEBT_FTP_EVT_PUT_DNE event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_FTP_EVT_PUT_DNE with the one of the error codes:
    AEEBT_FTP_ERR_NONE
    AEEBT_FTP_ERR_NOT_ALLOWED

=======================================================================

Function:  IBTEXTFTP_Get()

Description:
  This command can be used by server and client applications.  It should 
  only be called after AEEBT_FTP_EVT_CONNECTED is received.
  
  Client application issues this command to copy a file/folder from the server.
  
  Server application uses it to retrieve the file/folder that the client is
  trying to put.

Prototype:
  int IBTEXTFTP_Get( IBTExtFTP* po, AEEBTFTPObject objType,
                     const AECHAR* pwLocalName, 
                     const AECHAR* pwRemoteName );

Parameters:

  po            - pointer to the IBTExtFTP object
  objType       - file or folder
  pwLocalName   - name of file/folder in local device
  pwRemoteName  - name of file/folder in remote device

Return value:
  SUCCESS   - the request to copy a file/folder from the remote device was  
              queued and that AEEBT_FTP_EVT_GET_DNE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers

Event:
  AEEBT_FTP_EVT_GET_DNE with the one of the error codes:
    AEEBT_FTP_ERR_NONE
    AEEBT_FTP_ERR_NOT_FOUND
  
=======================================================================

Function:  IBTEXTFTP_Delete()

Description:
  Client application can issue this command to delete a file/folder from
  the remote server.

Prototype:
  int IBTEXTFTP_Delete( IBTExtFTP* po, const AECHAR* pwName );

Parameters:
  po        - pointer to the IBTExtFTP object
  pwName    - name of file/folder to delete from remote server

Return value:
  SUCCESS   - the request to delete file/folder from the remote server was 
              queued and that AEEBT_FTP_EVT_DELETE_DNE event will be 
              generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - invalid pointers
  
Event:
  AEEBT_FTP_EVT_DELETE_DNE with the one of the error codes:
    AEEBT_FTP_ERR_NONE
    AEEBT_FTP_ERR_NOT_ALLOWED

=======================================================================

Function:  IBTEXTFTP_Authenticate()

Description:
  Either server or client application can use this command to authenticate the
  connection being brought up.
  
  Client application should issue this command in response to event 
  AEEBT_FTP_EVT_AUTH_REQ.
  
  Server application should issue this command when receiving a connection
  request.

Prototype:
  int IBTEXTFTP_Authenticate( IBTExtFTP* po, const AEEBTBDAddr* pBDAddr, 
                              const AECHAR* pwUserID,
                              const AECHAR* pwPassword );

Parameters:
  po          - pointer to the IBTExtFTP object
  pBDAddr     - BD address of remote device
  pwUserID    - user ID for client
  pwPassword  - password for the user ID

Return value:
  SUCCESS   - the request to authenticate the connection being brought up was 
              queued and that an appropriate event will be generated at a 
              later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  For server, if the authentication passes, the event AEEBT_FTP_EVT_CONN_REQ 
  will be generated again with bUnauthorized set to FALSE.  Server can then
  accept or reject the connection request.
  
  For client, depending on whether server accepts the connection request, 
  either the event AEEBT_FTP_EVT_CONNECTED or AEEBT_FTP_EVT_CONN_FAILED 
  will be generated.

=======================================================================

Function:  IBTEXTFTP_Disconnect()

Description:
  Either server or client application can use this command to bring down the
  existing connection between them.

Prototype:
  int IBTEXTFTP_Disconnect( IBTExtFTP* po );

Parameters:
  po          - pointer to the IBTExtFTP object

Return value:
  SUCCESS   - the request to disconnect from remote device was queued and that
              AEEBT_FTP_EVT_DISCONNECTED event will be generated at a later time.
  ENOMEMORY - could not queue command due to lack of memory
  EBADPARM  - NULL pointer

Event:
  AEEBT_FTP_EVT_DISCONNECTED with the one of the error codes:
    AEEBT_FTP_ERR_NONE
    AEEBT_FTP_ERR_NO_CONN
=======================================================================

Function:  IBTEXTFTP_GetProgressInfo()

Description:
  Either server or client application can use this API to retrieve
  information about the progress of data transfer. This API can be
  invoked when AEEBT_FTP_EVT_PROGRESS_IND is received.

Prototype:
  int IBTEXTFTP_GetProgressInfo( IBTExtFTP* po, AEEBTProgressInfo* pProgressInfo );

Parameters:
  po            - pointer to the IBTExtFTP object
  pProgressInfo - pointer to structure that holds the size of the object
                  and the number of bytes that have alreay been 
                  transferred.

Return value:
  SUCCESS   - information successfully returned.
  EBADPARM  - NULL pointer

Event:
  None
=======================================================================*/
#endif /* _AEE_BT_EXT_FTP_H_ */
