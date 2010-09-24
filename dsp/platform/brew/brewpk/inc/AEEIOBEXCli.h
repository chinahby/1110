#ifndef AEEIOBEXCLI_H
#define AEEIOBEXCLI_H
/*=============================================================================
FILE:         AEEIOBEXCli.h

SERVICES:     IOBEXCli Interface

DESCRIPTION:  IOBEXCli is the interface that supports OBEX protocol
              in client role.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*============================================================================
 Includes
============================================================================*/
#include "AEEIQI.h"
#include "AEEOBEXComDefs.h"
#include "AEEIOBEXHeader.h"
#include "AEEISignal.h"

/*=============================================================================
 Interface ID
=============================================================================*/

#define AEEIID_IOBEXCli       0x01053c89


/*============================================================================
 OBEX Client Interface Definition
============================================================================*/
#define INHERIT_IOBEXCli(iname) \
   INHERIT_IQI(iname);\
                                                                    \
   int (*GetSupportedTransports) (iname            *po,             \
                                  AEEOBEXTransport *pSuppTransport, \
                                  int              nLen,            \
                                  int              *pnLenReq);      \
                                                               \
   int (*SetTransport)    (iname              *po,             \
                           AEEOBEXTransport   transType,       \
                           const char         *pszTransport);  \
                                                               \
   int (*CreateHeaderList)(iname              *po,             \
                           IOBEXHeader        **ppiObexHdr );  \
   int (*Authenticate)    (iname              *po,             \
                           AEEOBEXCharSet     charSet,         \
                           uint8              *pRealm,         \
                           int                nRealmLen,       \
                           boolean            uid,             \
                           boolean            acs);            \
                                                               \
   int (*SendCmd)         (iname              *po,             \
                           AEEOBEXRequestType obexReq,         \
                           IOBEXHeader        *piObexHdr,      \
                           AEEOBEXFlags       flags,           \
                           boolean            bFinal);         \
                                                               \
   int (*OnEventNotify)   (iname              *po,             \
                           ISignal            *piSignal);      \
                                                               \
   int (*GetEvent)        (iname              *po,             \
                           AEEOBEXEventType   *pEventId,       \
                           IOBEXHeader        **ppiOBEXHdr,    \
                           AEEOBEXCmdStatus   *pCmdStatus,     \
                           AEEOBEXFlags       *pFlags);        \
                                                               \
   int (*SetUidPassword)  (iname              *po ,            \
                           const uint8        *pUid,           \
                           int                nUidLen,         \
                           const uint8        *pPswd,          \
                           int                nPswdlen);       \
                                                               \
   int (*GetAuthChallenge)(iname              *po,             \
                           boolean            *pbUserIdReq,    \
                           boolean            *pbFullAccess,   \
                           AEEOBEXCharSet     *pCharSet,       \
                           uint8              *pRealm,         \
                           int                nRealmLen,       \
                           int                *pnRealmLenReq );\
                                                               \
   int (*GetAuthResponse) (iname              *po,             \
                           AEEOBEXAuthResp    *pAuthResponse); \
                                                               \
   int (*GetParams)       (iname              *po,             \
                           AEEOBEXParamType   paramType,       \
                           uint32             *pU32,           \
                           uint8              *pParam,         \
                           int                nParamLen,       \
                           int                *pnParamLenReq)
AEEINTERFACE_DEFINE(IOBEXCli);

/*============================================================================
 Interface Accessor Functions
============================================================================*/

static __inline uint32 IOBEXCli_AddRef( IOBEXCli *p )
{
   return AEEGETPVTBL((p),IOBEXCli)->AddRef( p );
}

static __inline uint32 IOBEXCli_Release( IOBEXCli *p )
{
   return AEEGETPVTBL((p),IOBEXCli)->Release( p );
}

static __inline int IOBEXCli_QueryInterface( IOBEXCli *p,
                                             AEECLSID idReq, void **ppo )
{
   return AEEGETPVTBL((p),IOBEXCli)->QueryInterface( p, idReq, ppo );
}

static __inline int  IOBEXCli_GetSupportedTransports( IOBEXCli *p,
                                                      AEEOBEXTransport *pTransport,
                                                      int nLen,
                                                      int *pnLenReq)
{
   return AEEGETPVTBL((p),IOBEXCli)->GetSupportedTransports( p,pTransport, nLen, pnLenReq );
}

static __inline int IOBEXCli_SetTransport( IOBEXCli         *p,
                                           AEEOBEXTransport transport,
                                           const char       *pszTransParams)
{
   return  AEEGETPVTBL((p),IOBEXCli)->SetTransport( p, transport, pszTransParams );
}

static __inline int IOBEXCli_CreateHeaderList(IOBEXCli    *p,
                                              IOBEXHeader **ppiObexHdr)
{
   return  AEEGETPVTBL((p),IOBEXCli)->CreateHeaderList(p, ppiObexHdr);
}

static __inline int IOBEXCli_Authenticate( IOBEXCli        *p,
                                           AEEOBEXCharSet  charSet,
                                           uint8           *pRealm,
                                           int             nRealmLen,
                                           boolean         uid,
                                           boolean         acs )
{
   return AEEGETPVTBL((p),IOBEXCli)->Authenticate( p, charSet, pRealm,
                                                   nRealmLen, uid, acs );
}

static __inline int IOBEXCli_SendCmd( IOBEXCli           *p,
                                      AEEOBEXRequestType obexReq,
                                      IOBEXHeader        *piObexHdr,
                                      AEEOBEXFlags       flags,
                                      boolean            bFinal )
{
   return  AEEGETPVTBL((p),IOBEXCli)->SendCmd( p,obexReq,
                                               piObexHdr,flags,bFinal);
}

static __inline int IOBEXCli_Connect( IOBEXCli     *p,
                                      IOBEXHeader  *pHdr )
{
   return  AEEGETPVTBL((p),IOBEXCli)->SendCmd( p,AEEOBEX_REQ_CONNECT,
                                               pHdr,0,TRUE);
}

static __inline int IOBEXCli_Put( IOBEXCli       *p,
                                  IOBEXHeader    *pHdr,
                                  boolean        bFinal )
{
   return AEEGETPVTBL((p),IOBEXCli)->SendCmd( p, AEEOBEX_REQ_PUT,
                                                 pHdr, 0,
                                                 bFinal);
}

static __inline int IOBEXCli_Get( IOBEXCli        *p,
                                  IOBEXHeader     *pHdr,
                                  boolean         bFinal )
{
   return AEEGETPVTBL((p),IOBEXCli)->SendCmd( p, AEEOBEX_REQ_GET, pHdr,
                                      0, bFinal);
}

static __inline int IOBEXCli_SetPath( IOBEXCli     *p,
                                      IOBEXHeader  *pHdr,
                                      boolean      bUpLevel,
                                      boolean      bDontCreate)
{
   AEEOBEXFlags flags = 0;
   
   flags = ( bUpLevel ) ? ( flags | AEEOBEX_FLAG_BACKUP ) : flags;
   flags = ( bDontCreate ) ? ( flags | AEEOBEX_FLAG_DONT_CREATE_DIR ) : flags;

   return AEEGETPVTBL((p),IOBEXCli)->SendCmd( p,AEEOBEX_REQ_SETPATH,pHdr,
                                         flags, TRUE);
}

static __inline int IOBEXCli_Abort( IOBEXCli  *p ,IOBEXHeader  *pHr)
{
   return AEEGETPVTBL((p),IOBEXCli)->SendCmd( p,AEEOBEX_REQ_ABORT, pHr,
                                               0, TRUE);
}

static __inline int IOBEXCli_Disconnect(IOBEXCli     *p,
                                        IOBEXHeader  *piHdr )
{
   return AEEGETPVTBL((p),IOBEXCli)->SendCmd( p,AEEOBEX_REQ_DISCONNECT,
                                              piHdr, 0, TRUE);
}

static __inline int IOBEXCli_OnEventNotify(IOBEXCli     *p,
                                           ISignal      *piSignal )
{
   return AEEGETPVTBL((p),IOBEXCli)->OnEventNotify( p ,piSignal );
}

static __inline int IOBEXCli_GetEvent(IOBEXCli           *p,
                                      AEEOBEXEventType   *pEvent,
                                      IOBEXHeader        **ppiHdr,
                                      AEEOBEXCmdStatus   *pCmdStatus,
                                      AEEOBEXFlags       *pFlags)
{
   return AEEGETPVTBL((p),IOBEXCli)->GetEvent( p ,pEvent, ppiHdr, pCmdStatus,
                                                pFlags);
}

static __inline int IOBEXCli_SetUidPassword(IOBEXCli         *p,
                                            const uint8      *pUid,
                                            int              nUidLen,
                                            const uint8      *pPswd,
                                            int              nPswdLen)
{
   return AEEGETPVTBL((p),IOBEXCli)->SetUidPassword( p, pUid, nUidLen
                                                    ,pPswd, nPswdLen );
}

static __inline int IOBEXCli_GetAuthChallenge(IOBEXCli       *p,
                                          boolean            *pbUserIdReq,
                                          boolean            *pbFullAccess,
                                          AEEOBEXCharSet     *pCharSet,
                                          uint8              *pRealm,
                                          int                nRealmLen,
                                          int                *pnRealmLenReq )
{
   return AEEGETPVTBL((p),IOBEXCli)->GetAuthChallenge(p,pbUserIdReq,
                                                      pbFullAccess,pCharSet,
                                                      pRealm,nRealmLen,
                                                      pnRealmLenReq);
}

static __inline int IOBEXCli_GetAuthResponse(IOBEXCli        *p,
                                             AEEOBEXAuthResp *pAuthResp)
{
   return AEEGETPVTBL((p),IOBEXCli)->GetAuthResponse(p,pAuthResp);
}

static __inline int IOBEXCli_GetParams( IOBEXCli          *p,
                                        AEEOBEXParamType  param,
                                        uint32            *pU32,
                                        uint8             *pParam,
                                        int               nParamLen,
                                        int               *pnParamLenReq)
{
   return AEEGETPVTBL((p),IOBEXCli)->GetParams( p, param, pU32,pParam, nParamLen,
                                                   pnParamLenReq );
}

/*=============================================================================
    INTERFACE DOCUMENTATION
===============================================================================

Interface Name: IOBEXCli

Description: (Generic) OBEX Profile
   
   This interface enables BREW applications to use OBEX protocol to
   exchange data objects with a remote device.Each instance of 
   IOBEXCli supports one connection in a client role.A BREW application
   can instantiate multiple client objects each being in a connection
   simultaneously.

   The Client can communicate with an OBEX server over different transports 
   The supported transports can be obtained by calling 
   IOBEXCli_GetSupportedTransports().
   
   The OBEX client should set the transport and information related to the
   server it intends to communicate with, by calling IOBEXCli_SetTransport().
   It can then initiate OBEX operations like CONNECT, GET, PUT. Commands are sent
   along with the optional OBEX header list.

   The application is notified of the completion of an operation or other
   asynchronous events via the signal that is registered via 
   IOBEXCli_OnEventNotify().Events can be obtained by calling
   IOBEXCli_GetEvent().
   An OBEX operation can be aborted by calling IOBEXCli_Abort().

   The client can initiate authentication for a particular operation 
   by calling IOBEXCli_Authenticate() followed by the command
   The implementation is responsible for forming the digest-challenge and
   adding the authentication challenge header to the header list.
   If the client initiates authentication, it will be notified when the server
   responds with the authentication response header via the event
   AEEOBEX_EVT_AUTH_RESPONSE.The client should indicate to the implementation,
   the password by calling IOBEXCli_SetUidPassword().
   The result of the authentication procedure is indicated by
   AEEOBEX_EVT_AUTH_COMPL event.

   The server can authenticate the client when the client initiates an
   operation .AEEOBEX_EVT_AUTH_CHALENGE is sent to the application
   when the authentication challenge header is received.
   On receiving this event, the application should indicate to the underlying
   implementation the User id and password required for authentication.
   This is done by calling IOBEXCli_SetUidPassword().Creation of the 
   digest-response and the authentication response headers is handled 
   by the OBEX implementation.

===H2>
   Usage example:
===/H2>
===pre> 
   ===== To Register for events;

   // create a signal for example, use ISignalCBFactory_CreateSignal()
   IOBEXCli_OnEventNotify(pMe->piCli, piSignal);

   ===== To connect to server:

   // see if the transport is supported 
   IOBEXCli_GetSupportedTransports(pMe->piCli, &transport, transLen, &reqTransLen);

   // set the required transport parameters.For E.g if transport is Bluetooth RFCOMM
   IOBEXCli_SetTransport(pMe->piCli, AEEOBEX_TRANS_BT_RFCOMM,"server_channel=10;bd_address=9abc17101710");

   // Connect to the OBEX server
   IOBEXCli_Connect(pMe->piCli, NULL);
   
   // wait for AEEOBEX_EVT_CLI_CONN_RESP

   ===== To retrieve and handle events when signal is triggered:

   // re-enable signal
   ISignalCtl_Enable (pMe->piCliSignalCtl); 
   
   while (IOBEXCli_GetEvent (pMe->piCli, &piHdr, &event, &cmdStat, &flag) != AEE_ENOMORE)
   {
      switch (event)
      {
         case AEEOBEX_EVT_CLI_CONN_RESP:
         ...
         case AEEOBEX_EVT_CLI_GET_RESP:
         ...
         case AEEOBEX_EVT_ERROR:
      }
   }
   
   ===== To authenticate the server before connection

   // see if the transport is supported 
   IOBEXCli_GetSupportedTransports(pMe->piCli, &transport, transLen, &reqTransLen);

   // set the req transport parameters.For E.g if transport is Bluetooth RFCOMM
   IOBEXCli_SetTransport(pMe->piCli, AEEOBEX_TRANS_BT_RFCOMM,"server_channel=10;bd_address=9abc17101710");

   IOBEXCli_Authenticate(pMe->piCli,0, NULL, 0, TRUE, TRUE);

   // Connect to the OBEX server
   IOBEXCli_Connect(pMe->piCli, NULL);

   ===== To handle authentication challenge event 

   // In the event handler 
   switch (event)
   {
      case AEEOBEX_EVT_CLI_CONN_RESP:
      ...

      case AEEOBEX_EVT_AUTH_CHALLENGE:

      // server has requested for authentication
      IOBEXCli_GetAuthChallenge(pMe->piCli,&uid,&acs,&charSet, &bufRealm, bufRelamLen, &reqLen);

      // get the password and user id ( if required) from the user. Call
      IOBEXCli_SetUidPassword(pMe->piCli,uid, uidLen, pswd, pswdLen);

   }
   
   ===== To handle authentication response event 

   // In the event handler 
   switch (event)
   {
      case AEEOBEX_EVT_CLI_CONN_RESP:
      ...

      case AEEOBEX_EVT_AUTH_RESPONSE:

      // client had requested for authentication 
      IOBEXCli_GetAuthResponse(pMe->piCli,&authResp);

      // get the password from the user
      IOBEXCli_SetUidPassword(pMe->piCli,NULL,0, pswd, pswdLen);
   }

   ===== To send requests to the server 

   ret = IOBEXCli_CreateHeaderList(pMe->pCli, &piHdr);

   if (AEE_SUCCESS == ret)
   {
      IOBEXHeader_AddByteSeq(piHdr, AEEOBEX_HDR_BODY, data, dataLen);
      
      IOBEXCli_Put(pMe->pCli, piHdr, FALSE);

      // done sending the header list
      IOBEXHeader_Release(piHdr);

      // now wait for the AEEOBEX_EVT_CLI_PUT_RESP event
      ...
   }

   ===== To fetch response received from the server

   ret = IOBEXCli_GetEvent(pMe->pCli, &event, &piHdr, &cmdStatus, &flags);

   // In the event handler 
   switch (event)
   {
      case AEEOBEX_EVT_CLI_GET_RESP:

      if (piHdr != NULL)
      {
         // optionally get the list
         IOBEXHeader_GetList(piHdr, pHdrList, 0, hdrListLen, &reqHdrListLen);
   
         IOBEXHeader_GetByteSeq(piHdr, AEEOBEX_HDR_BODY, &buf, bufLen, &reqBufLen);
   
         // done with the received header list
         IOBEXHeader_Release(piHdr);
      }
      ...
   }
===/pre>

==============================================================================

IOBEXCli_AddRef()

Description:
   Inherited from IQI_AddRef().

See Also:
   IOBEXCli_Release()

==============================================================================

IOBEXCli_Release()

Description:
   Inherited from IQI_Release().

See Also:
   IOBEXCli_AddRef()
   
==============================================================================

IOBEXCli_QueryInterface()

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IOBEXCli-derived class

Prototype:
   int IOBEXCli_QueryInterface (IOBEXCli* po, AEECLSID clsReq, 
                                void** ppo);

Parameters:
   po [i]     : the object
   clsReq [i] : Class ID of the interface to query.
   ppo [o]    : place holder for pointer to the interface.

Return Value:
   SUCCESS: on success, 
   Otherwise: an error code.

Comments:
   On return of the API, if content of ppo is NULL, then the queried 
   interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.

===============================================================================

Function: IOBEXCli_GetSupportedTransports

Description:
   This function is called by client application to retrieve the supported
   transports.

Prototype:
   int  IOBEXCli_GetSupportedTransports(IOBEXCli         *po,
                                        AEEOBEXTransport *pTransport,
                                        int              nLen,
                                        int              *pnLenReq);

Parameters
   po [i]         : pointer to the IOBEXCli object
   pTransport [o] : array of supported transports
   nLen [i]       : number of elements in the array
   pnLenReq[o]    : number of elements required in the array to return all
                    transports

Return Value:
   AEE_SUCCESS         : the information was returned successfully
   AEE_BADPARAM        : Bad parameters
   AEE_EBUFFERTOOSMALL : the number of elements required in the array is insufficient

See Also :
AEEOBEXTransport

==============================================================================

Function: IOBEXCli_SetTransport

Description:
   A client can call this method to set the transport to be used.
   Transport parameters are sent to the implementation as a null terminated
   string containing name-value pairs separated by a delimiter ';'.The format
   of the string is "<name1>=<value1>;<name2>=<value2>". For E.g if the 
   transport is Bluetooth RFCOMM,the transport parameters are the 
   Bluetooth device address and RFCOMM server channel number.
   If the bd address is 0x9abc650000a0, and the server channel is 0x10,
   the transport string would be "bd_address=9abc650000a0;server_channel=10".
   For a list of name-value pairs see AEEOBEXTransport

Prototype:
  int IOBEXCli_SetTransport(IOBEXCli             *po, 
                            AEEOBEXTransport     transType,
                            const char           *pszTransInfo);
Parameters
   po [i]           : pointer to the IOBEXCli object
   transType [i]    : transport type
   pszTransInfo [i] : null terminated string indicating transport parameters

Return Value:
   AEE_SUCCESS         : the information was set successfully
   AEE_ENOMEMORY       : no resources to store the information.
   AEE_EINVALIDFORMAT  : invalid format

Event(s):
   None

See Also:
   IOBEXCli_Connect()
   AEEOBEXTransport

==============================================================================

Function: IOBEXCli_CreateHeaderList

Description:
   A client can call this method to get a pointer to OBEX header list object.
   The client should add headers to this list by calling appropriate methods
   on the IOBEXHeader interface.After the app has sent this header with a 
   command, it should call IOBEXHeader_Release() to free the list.

Prototype:
  int IOBEXCli_CreateHeaderList(IOBEXCli         *po, 
                                IOBEXHeader      **ppObexHdrList );

Parameters
   po [i]            : pointer to the IOBEXCli  object
   ppObexHdrList [o] : place holder for pointer to IOBEXHeader Interface

Return Value:
   SUCCESS         : the header interface was returned
   ENOMEMORY       : no resources to create IOBEX header. This could 
                     happen if IOBEXHeader_Release()was not called
                     on previous header objects 

Event(s):
   None

See Also:
   IOBEXHeader

==============================================================================

Function:IOBEXCli_Authenticate

Description:
   A client can call this method to indicate to the OBEX implementation that
   it should authenticate the server.Calling this function would cause the 
   challenge header to be sent with the next OBEX command. For example, if the 
   server has to be authenticated during connection,the application should
   call IOBEXCli_Authenticate() before calling IOBEXCli_Connect() to 
   initiate authentication during connection.

Prototype:
  int IOBEXCli_Authenticate(IOBEXCli           *po,
                            AEEOBEXCharSet     charSet,
                            uint8              *pRealm,
                            int                nRealmLen,
                            boolean            bUidReqd,
                            boolean            bFullAccess);
Parameters
   po [i]           : pointer to the IOBEXCli object
   charSet [i]      : character set in which the realm string is encoded
   pRealm [i]       : pointer to realm information.
   nRealmLen [i]    : realm len 
   bUidReqd [i]     : indicates if user id is required 
                      TRUE if the client requires UserID from the server
                      FALSE,if the client does not require User Id from the server.
   bFullAccess [i]  : type of Access
                      TRUE for full access
                      FALSE for read only access 

Return Value:
   AEE_SUCCESS      : the information was set successfully
   AEE_ENOMEMORY    : no resources to store the information.
   AEE_EUNSUPPORTED : not supported 

Event(s):
   AEEOBEX_EVT_AUTH_COMPL is sent when the authentication is complete.

See Also:
   IOBEXCli_SetUidPassword()
   IOBEXCli_Connect()
   AEEOBEXCharSet

==============================================================================

Function: IOBEXCli_SetUidPassword

Description:
   The application should call this function to provide information that is 
   required for authentication.The API should be called when the events
   AEEOBEX_EVT_AUTH_RESPONSE and AEEOBEX_EVT_AUTH_CHALLENGE are received.
   
   When called in response to AEEOBEX_EVT_AUTH_CHALLENGE, the client should 
   send user ID and password for which the server should grant
   access.

   When called in response to  AEEOBEX_EVT_AUTH_RESPONSE, user ID is the ID
   obtained from  AEEOBEXAuthResp struct that accompanies the 
   AEEOBEX_EVT_AUTH_RESPONSE event.
   The password corresponds to the Uid obtained from AEEOBEXAuthResp.

Prototype:
   int IOBEXCli_SetUidPassword(IOBEXCli       *po,
                               const uint8    *pUserID,
                               int            nUidLen,
                               const uint8    *pPassword
                               int            nPswdLen );

Parameters
   po [i]         : pointer to the IOBEXCli object
   pUserID [i]    : pointer to userID
   nUidLen [i]    : user id length
   pPassword [i]  : password for userID
   nPswdLen [i]   : password length

Return Value:
   AEE_SUCCESS    : the userId and/or password were successfully copied.
   AEE_ENOMEMORY  : could not save parameters due to lack of memory
   AEE_EBADPARM   : invalid paramaters

See Also:
   AEEOBEXAuthResp


===============================================================================s

Function: IOBEXCli_Connect()

Description:

   This method can be called by the client application to connect 
   to a OBEX server.

Prototype:
   int IOBEXCli_Connect(IOBEXCli        *po,
                        IOBEXHeader     *piObexHdr);

Parameters
   po [i]         : pointer to the IOBEXCli object
   piObexHdr [i]  : pointer to IOBEXHeader object that 
                    contains the OBEX headers for connection.

Return Value:
   AEE_SUCCESS    : the request to connect was queued and
                    AEEOBEX_EVT_CLI_CONN_RESP will be sent when the procedure
                    is complete.
   AEE_ENOMEMORY  : could not queue command due to lack of memory
   AEE_EBADPARM   : invalid parameters

Event(s) :
   AEEOBEX_EVT_CLI_CONN_RESP is sent to indicate the result of the 
   connection procedure

See Also:
   IOBEXHeader
   IOBEXCli_Disconnect()


===============================================================================

Function: IOBEXCli_Disconnect()

Description:
   The client may use this function to bring down the OBEX connection

Prototype:
   int IOBEXCli_Disconnect(IOBEXCli      *po, 
                           IOBEXHeader   *piObexHdr);
Parameters
   po [i]         : pointer to the IOBEXCli object
   piObexHdr [i]  : pointer to the IOBEXHeader object that contains the
                    headers to be sent to the server.This 
                    should be set to NULL if no headers are being sent.

Return Value:
   AEE_SUCCESS    : the request to disconnect from remote device was queued.
                    AEEOBEX_EVT_CLI_DISC_RESP would be sent at a later point.
   AEE_ENOMEMORY  : could not queue command due to lack of memory
   AEE_EBADPARM   : invalid parameters

Event(s) :
   AEEOBEX_EVT_CLI_DISC_RESP is sent to the application to indicate
   result of the disconnect procedure.

See Also:
   IOBEXHeader
   IOBEXCli_Connect()


===============================================================================

Function: IOBEXCli_Put()

Description:
   This method will cause a PUT request to be sent to the server.

Prototype:
   int IOBEXCli_Put(IOBEXCli        *po,
                    IOBEXHeader     *piObexHdr,
                    boolean         bFinal );
Parameters
   po [i]        : pointer to the IOBEXCli object
   piObexHdr [i] : pointer to the list of OBEX headers to be sent
   bFinal [i]    : indicates this is the last block of data
                   to send to the server

Return Value:
   AEE_SUCCESS   : the request to send data block to the remote device was
                   queued.AEEOBEX_EVT_CLI_PUT_RESP would be sent at a later point.
   AEE_ENOMEMORY : could not queue command due to lack of memory
   AEE_EBADPARM  : invalid parameters

Event(s): AEEOBEX_EVT_CLI_PUT_RESP is sent to the application when the server 
          responds to this request.

See Also:
   IOBEXHeader
   IOBEXCli_Connect()

===============================================================================

Function: IOBEXCli_Get()

Description:
   This method will cause a GET request to be sent to the server.

Prototype:
   int IOBEXCli_Get(IOBEXCli        *po,
                    IOBEXHeader     *piObexHdr,
                    boolean         bFinal);
Parameters:
   po [i]         : pointer to the IOBEXCli object
   piObexHdr [i]  : pointer to IOBEXHeader that has the list of
                    headers to be sent to the server.Should be set to 
                    NULL if no headers are being sent.
   bFinal [i]     : TRUE: indicates that this is the last packet
                    describing the object that is being requested and the request
                    phase is complete.
                    FALSE: the client has not finished describing the object

Return Value:
   AEE_SUCCESS    : the request was queued.AEEOBEX_EVT_CLI_GET_RESP would be sent later.
   AEE_ENOMEMORY  : could not queue command due to lack of memory
   AEE_EBADPARM   : invalid input parameters

Event(s) : AEEOBEX_EVT_CLI_GET_RESP is sent to the application when the server 
           responds to this request.

See Also:
   IOBEXHeader
   IOBEXCli_Connect()

===============================================================================

Function: IOBEXCli_SetPath()

Description:
   Client application can use this function to set the path on the server
   for subsequent PUT/GET operations.

Prototype:
   int IOBEXCli_SetPath(IOBEXCli        *po,
                        IOBEXHeader     *piObexHdr,
                        boolean         bUpLevel,
                        boolean         bDontCreate);
Parameters
   po [i]        : pointer to the IOBEXCli object
   piObexHdr [i] : pointer to the list of headers to be sent
   bUpLevel [i]  : change directory up one level
   bDontCreate [i] : Dont create directory if it dosent already exist
                     TRUE  : does not create the directory 
                     FALSE : creates the directory

Return Value:
   AEE_SUCCESS   : the request to set the path on the server was queued.
                   AEEOBEX_EVT_CLI_SETPATH_RESP would be sent when the operation 
                   is complete
   AEE_ENOMEMORY : could not queue command due to lack of memory
   AEE_EBADPARM  : invalid paramaters

Event(s) : AEEOBEX_EVT_CLI_SETPATH_RESP is sent to the application when the server 
           responds to this request.

See Also:
   IOBEXHeader
   IOBEXCli_Connect()
   IOBEXCli_Put()
   IOBEXCli_Get()


================================================================================

Function: IOBEXCli_Abort()

Description:
   This function will abort the current operation.

Prototype:
   int IOBEX_ClientAbort(IOBEXCli     *po,
                         IOBEXHeader  *piObexHdr,);

Parameters
   po [i]         : pointer to the IOBEXCli object
   piObexHdr [i]  : optional OBEX header describing the reason for abort

Return Value:
   AEE_SUCCESS    : Abort initiated
   AEE_EIDLE      : no operation in progress
   AEE_EBADPARM   : invalid paramaters

Event(s): AEEOBEX_EVT_CLI_ABORT_COMPL is sent to the application when the 
          operation is aborted.Note that this event is sent in addition to the
          event which indicates completion of the current operation.
          For E.g. if the PUT operation is being aborted, AEEOBEX_EVT_CLI_PUT_RESP
          would be sent to the application, followed by AEEOBEX_EVT_CLI_ABORT_COMPL.

See Also:
   IOBEXCli_Connect()
   IOBEXCli_Put()
   IOBEXCli_Get()


================================================================================

Function: IOBEXCli_SendCmd()

Description:
   This function sends an OBEX command to the server

Prototype:
   int IOBEXCli_SendCmd( IOBEXCli           *p,
                         AEEOBEXRequestType obexReq,
                         IOBEXHeader        *piObexHdr,
                         AEEOBEXFlags       flags,
                         boolean            bFinal )

Parameters
   po [i]        : pointer to the IOBEXCli object
   obexReq [i]   : OBEX request type
   piObexHdr [i] : optional OBEX header sent with the command
   flags [i]     : optional flags for this command
   bFinal [i]    : indicates if this is the last packet in the request phase

Return Value:
   AEE_SUCCESS   : the request was queued.
   AEE_ENOMEMORY : could not queue command due to lack of memory
   AEE_EBADPARM  : invalid input parameters

Event(s): The event that is generated depends on the 
          OBEX request type.For request types and corresponding events see 
          AEEOBEXEventType

See Also:
   AEEOBEXEventType
   AEEOBEXRequestType
   AEEOBEXFlags
   IOBEXHeader

==============================================================================

Function: IOBEXCli_OnEventNotify()

Description:
   This function should be called to register a signal that will be 
   triggered when an event is ready to be delivered to the application.
   The application can then call IOBEXCli_GetEvent() to retrieve the 
   event.

Prototype:
   int IOBEXCli_OnEventNotify(IOBEXCli    *po,
                              ISignal     *piSignal)

Parameters
   po [i]        : pointer to the IOBEXCli object
   piSignal [i]  : signal to be registered

Return Value:
   AEE_SUCCESS   : signal successfully registered
   AEE_EBADPARM  : invalid paramaters

See Also:
   IOBEXCli_GetEvent()

===============================================================================

Function: IOBEXCli_GetEvent()

Description:
   This function can be used to retrieve events from the interface.
   The application can register to be notified of events by calling
   IOBEXCli_OnEventNotify()


   The list of events and corresponding output parameters

   Events :                      Data structure 

   AEEOBEX_EVT_CLI_CONN_RESP      : AEEOBEXCmdStatus and optional headers
   AEEOBEX_EVT_CLI_GET_RESP       : AEEOBEXCmdStatus and optional headers
   AEEOBEX_EVT_CLI_PUT_RESP       : AEEOBEXCmdStatus and optional headers
   AEEOBEX_EVT_CLI_SETPATH_RESP   : AEEOBEXCmdStatus and optional headers
   AEEOBEX_EVT_CLI_DISC_RESP      : AEEOBEXCmdStatus and optional headers
   AEEOBEX_EVT_CLI_ABORT_COMPL    : AEEOBEXCmdStatus
   AEEOBEX_EVT_AUTH_CHALLENGE     : use IOBEXCli_GetAuthChallenge()
                                    to get the auth challenge params  

   AEEOBEX_EVT_AUTH_RESPONSE      : IOBEXCli_GetAuthResponse()
                                    to get the auth response params
   AEEOBEX_EVT_AUTH_COMPL         : AEEOBEXCmdStatus
   AEEOBEX_EVT_ERROR              : AEEOBEXCmdStatus


Prototype:
   int IOBEXCli_GetEvent(IOBEXCli           *po,
                         AEEOBEXEventType   *pEventId,
                         IOBEXHeader        **ppiHdr,
                         AEEOBEXCmdStatus   *pCmdStatus,
                         AEEOBEXFlags       *pFlags);

Parameters
   po [i]            : pointer to the IOBEXCli object
   pEventId [o]      : place holder for event id for the event being retrieved
   ppiHdr [o]        : if the event is accompanied by headers this location
                       will contain pointer to a IOBEXHeader Object. 
                       NULL otherwise.
   pCmdStatus [o]    : place holder for command status 
   pFlags [o]        : flags; currently not used

Return Value:
   AEE_SUCCESS       : event successfully returned 
   AEE_EBADPARM      : invalid paramaters
   AEE_ENOMORE       : no events available to be retrieved

See Also:
   IOBEXHeader
   AEEOBEXEventType
   AEEOBEXCmdStatus

=======================================================================

Function: IOBEXCli_GetAuthChallenge()

Description:
   This function should be called to retrieve the authentication challenge
   parameters when the client receives the AEEOBEX_EVT_AUTH_CHALLENGE
   event.

Prototype:
   int IOBEXCli_GetAuthChallenge(IOBEXCli        *po,
                                 boolean         *pbUidReq,
                                 boolean         *pbFullAccess,
                                 AEEOBEXCharSet  *pCharSet,
                                 uint8           *pRealm,
                                 int             nRealmLen,
                                 int             *pnRealmLenReq )

Parameters
   po [i]              : pointer to the IOBEXCli object
   pbUidReq [o]        : indicates if the server requested for Uid
   pbFullAccess [o]    : place holder for access granted by the server
   pCharSet [o]        : character set used for encoding the realm
   pRealm [o]          : realm string
   nRealmLen [i]       : buffer allocated for the realm string
   pnRealmLenReq [o]   : if the buffer allocated for the realm
                         string is insufficient, this location contains the 
                         required buffer length

Return Value:
   AEE_SUCCESS         : information successfully retrieved
   AEE_EBUFFERTOOSMALL : buffer size insufficient for realm string
   AEE_EBADPARM        : invalid paramaters
   AEE_ENOSUCH         : No auth challenge params present

=============================================================================

Function: IOBEXCli_GetAuthResponse()

Description:
   This function should be called to retrieve the authentication response
   parameters when the client receives the AEEOBEX_EVT_AUTH_RESPONSE
   event.

Prototype:
   int IOBEXCli_GetAuthResponse(IOBEXCli         *po,
                                AEEOBEXAuthResp  *pAuthResp)
Parameters
   po [i]          : pointer to the IOBEXCli object
   pAuthResp [o]   : pointer to the buffer to read authentication response
                     parameters
   
Return Value:
   AEE_SUCCESS     : information successfully retrieved
   AEE_EBADPARM    : invalid paramaters
   AEE_ENOSUCH     : No auth response parameters present
  
See Also:
   AEEOBEXAuthResp

===============================================================================
Function: IOBEXCli_GetParams()

Description:
   This function can be used to read information from the interface.
   paramType defines the type of the data being read. Accordingly, the output
   buffer is either pU32 or pParam. pU32 returns a parameter which is a 4 byte
   value. pParam can be used to return a byte array or a null terminated string
   containing a name value pair.If the return value is a null terminated string,
   with name value pairs, nParamLen, includes the null terminator.
   The format of the string is "<name1>=<value1>;<name2>=<value2>"

   The parameters that can be read from this interface are

   AEEOBEXPARAM_BT_BDA              six byte array
   AEEOBEXPARAM_MAX_PACKET_SIZE     uint32
   AEEOBEXPARAM_BT_RFCOMM_SCN       uint32


Prototype:
   int IOBEXCli_GetParams( IOBEXCli          *po,
                           AEEOBEXParamType  paramType,
                           uint32            *pU32,
                           uint8             *pParam,
                           int               nParamLen,
                           int               *pnParamLenReq);
Parameters
   po [i]              : pointer to the IOBEXCli object
   paramType [i]       : parameter type
   pU32 [o]            : place holder for a four byte parameter
   pParam [o]          : buffer to hold the parameter 
   nParamLen [i]       : length of the buffer
   pnParamLenReq [o]   : if return value is AEE_EBUFFERTOOSMALL, this 
                         location contains length of the buffer
                         required to hold the parameter


Return Value:
   AEE_SUCCESS         : parameters returned successfully
   AEE_EBADPARM        : invalid parameters
   AEE_EBUFFERTOOSMALL : buffer size that was passed to this function 
                         is too small.
   AEE_ENOSUCH         : no such parameter available 

=======================================================================*/
#endif /*AEEOBEXCLI_H*/


