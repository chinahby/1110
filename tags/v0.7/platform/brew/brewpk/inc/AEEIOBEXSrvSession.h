#ifndef AEEIOBEXSRVSESSION_H
#define AEEIOBEXSRVSESSION_H
/*=============================================================================
FILE:         AEEIOBEXSrvSession.h

SERVICES:     IOBEXSrvSession  Interface

DESCRIPTION:  IOBEXSrvSession represents a client- server session

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*============================================================================
 Includes
============================================================================*/
#include "AEEOBEXComDefs.h" 
#include "AEEIQI.h"
#include "AEEIOBEXHeader.h"
#include "AEEISignal.h"

/*=============================================================================
 Interface ID
=============================================================================*/

#define AEEIID_IOBEXSrvSession      0x01058559


/*=============================================================================
 Event data  Structure Definition
=============================================================================*/

typedef struct
{
   AEEOBEXPacket    packet;
   AEEOBEXFlags     flags;
   AEEOBEXCmdStatus cmdStatus;
}  AEEOBEXEvent;

/*============================================================================
 OBEX Server Session Interface Definition
============================================================================*/

#define INHERIT_IOBEXSrvSession(iname) \
   INHERIT_IQI(iname);\
    int (*CreateHeaderList) (iname                 *po,                \
                             IOBEXHeader           **ppiObexHdr );     \
                                                                       \
    int (*Authenticate)     (iname                  *po,               \
                             AEEOBEXCharSet         charSet,           \
                             uint8                  *pRealm,           \
                             int                    nRealmLen,         \
                             boolean                uid,               \
                             boolean                acs);              \
                                                                       \
    int (*OnEventNotify)    (iname                  *po,               \
                             ISignal                *piSignal);        \
                                                                       \
    int (*GetEvent)         (iname                  *po,               \
                             AEEOBEXEventType       *pdwId,            \
                             IOBEXHeader            **ppiOBEXHdr,      \
                             AEEOBEXEvent           *pEventData);      \
                                                                       \
    int (*SendResponse)     (iname                  *po,               \
                             AEEOBEXRequestType     request,           \
                             IOBEXHeader            *piOBEXHdr,        \
                             AEEOBEXCmdStatus       cmdStatus,         \
                             AEEOBEXFlags           flags);            \
                                                                       \
    int (*Disconnect)       (iname                  *po);              \
                                                                       \
    int (*SetUidPassword)   (iname                  *po,               \
                             const uint8            *pUid,             \
                             int                    nUidLen,           \
                             const uint8            *pPswd,            \
                             int                    nPswdlen);         \
                                                                       \
    int (*GetAuthChallenge)  (iname                 *po,               \
                              boolean               *pbUidReq,         \
                              boolean               *pbFullAccess,     \
                              AEEOBEXCharSet        *pCharSet,         \
                              uint8                 *pRealm,           \
                              int                   nRealmLen,         \
                              int                   *pnRealmLenReq );  \
                                                                       \
    int (*GetAuthResponse)  (iname                  *po,               \
                             AEEOBEXAuthResp        *pAuthResp);       \
                                                                       \
    int (*GetParams)        (iname                  *po,               \
                             AEEOBEXParamType       paramType,         \
                             uint32                 *pU32,             \
                             uint8                  *pParam,           \
                             int                    nParamLen,         \
                             int                    *pnParamLenReq)
AEEINTERFACE_DEFINE(IOBEXSrvSession);

/*============================================================================
 Interface Accessor Functions for IOBEXSrvSession
============================================================================*/

static __inline uint32 IOBEXSrvSession_AddRef( IOBEXSrvSession *p )
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->AddRef( p );
}

static __inline uint32 IOBEXSrvSession_Release( IOBEXSrvSession *p )
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->Release( p );
}

static __inline int IOBEXSrvSession_QueryInterface( IOBEXSrvSession *p,
                                             AEECLSID idReq, void **ppo )
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->QueryInterface(p, idReq, ppo);
}

static __inline int IOBEXSrvSession_CreateHeaderList(IOBEXSrvSession    *p,
                                              IOBEXHeader **ppiObexHdr)
{
   return  AEEGETPVTBL((p),IOBEXSrvSession)->CreateHeaderList(p, ppiObexHdr);
}

static __inline int IOBEXSrvSession_Authenticate( IOBEXSrvSession *p,
                                                  AEEOBEXCharSet  charSet,
                                                  uint8           *pRealm,
                                                  int             nRealmLen,
                                                  boolean         uid,
                                                  boolean         acs)
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->Authenticate( p, charSet,pRealm,
                                            nRealmLen, uid, acs );
}

static __inline int IOBEXSrvSession_SetUidPassword( IOBEXSrvSession *p,
                                           const uint8      *pUid,
                                           int              nUidLen,
                                           const uint8      *pPswd,
                                           int              nPswdLen)
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->SetUidPassword( p, pUid, nUidLen
                                                            ,pPswd, nPswdLen );
}

static __inline int IOBEXSrvSession_SendResponse( IOBEXSrvSession  *p,
                                            AEEOBEXRequestType     request,
                                            IOBEXHeader            *piOBEXHdr,
                                            AEEOBEXCmdStatus       cmdStatus,
                                            AEEOBEXFlags           flags)
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->SendResponse( p, request, piOBEXHdr,
                                                     cmdStatus, flags );
}

static __inline int IOBEXSrvSession_AcceptConnection( IOBEXSrvSession  *p,
                                                      IOBEXHeader      *piOBEXHdr,
                                                      AEEOBEXCmdStatus  cmdStatus )
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->SendResponse( p, AEEOBEX_REQ_CONNECT,
                                                piOBEXHdr,  cmdStatus, 0 );
}

static __inline int IOBEXSrvSession_Disconnect( IOBEXSrvSession  *p )
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->Disconnect( p );
}

static __inline int IOBEXSrvSession_OnEventNotify(IOBEXSrvSession  *p,
                                                  ISignal      *piSignal )
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->OnEventNotify( p ,piSignal );
}

static __inline int IOBEXSrvSession_GetEvent(IOBEXSrvSession       *p,
                                          AEEOBEXEventType         *pdwId,
                                          IOBEXHeader              **ppiOBEXHdr,
                                          AEEOBEXEvent             *pEventData)
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->GetEvent( p ,pdwId,ppiOBEXHdr, pEventData);
}

static __inline int IOBEXSrvSession_GetAuthChallenge(IOBEXSrvSession  *p,
                                                   boolean            *pbUidReq,
                                                   boolean            *pbFullAccess,
                                                   AEEOBEXCharSet     *pCharSet,
                                                   uint8              *pRealm,
                                                   int                nRealmLen,
                                                   int                *pnRealmLenReq )
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->GetAuthChallenge(p,pbUidReq,pbFullAccess,
                                                             pCharSet,pRealm,nRealmLen,pnRealmLenReq);
}

static __inline int IOBEXSrvSession_GetAuthResponse(IOBEXSrvSession      *p,
                                                    AEEOBEXAuthResp  *pAuthResp)
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->GetAuthResponse(p,pAuthResp);
}

static __inline int IOBEXSrvSession_GetParams( IOBEXSrvSession  *p,
                                           AEEOBEXParamType  param,
                                           uint32            *pU32,
                                           uint8             *pParam,
                                           int               nParamLen,
                                           int               *pnParamLenReq)
{
   return AEEGETPVTBL((p),IOBEXSrvSession)->GetParams(p, param, pU32, pParam, nParamLen,
                                                      pnParamLenReq);
}

/*=============================================================================
 DATA STRUCTURE DOCUMENTATION
============================================================================
AEEOBEXEvent

Description
   Event data that accompanies an event

Definition

typedef struct
{
   AEEOBEXPacket    packet;
   AEEOBEXFlags     flags;
   AEEOBEXCmdStatus cmdStatus;
}  AEEOBEXEvent;

Members:
   packet           : indicates if this packet was intermediate or
                      the final packet in a request
   flags            : bit map of flags
   cmdStatus        : indicates status corresponding to the event
   
See Also :
   AEEOBEXPacket
   AEEOBEXFlags
   AEEOBEXCmdStatus

=============================================================================
   INTERFACE DOCUMENTATION
===============================================================================

Interface Name: IOBEXSrvSession

Description: 
   
   This interface enables OBEX server applications to service requests
   from a client.When the server receives the AEEOBEX_EVT_SRV_NEW_CLI event,
   it receives a pointer to a session object.
   All further requests from this client are delivered to the 
   application via this interface.

===H2>
   Usage example:
===/H2>
===pre>
   ===== To Register for events;

   // create a signal for example, use ISignalCBFactory_CreateSignal()
   IOBEXSrvSession_OnEventNotify(pMe->piSsn, piSignal);

   
   ===== To retrieve and handle events when signal is triggered:

   // re-enable signal
   ISignalCtl_Enable (pMe->piSsnSignalCtl); 
   
   while (IOBEXSrvSession_GetEvent (pMe->piSsn, &evtId, &piHdr, &evtData) != AEE_ENOMORE)
   {
      switch (event)
      {
         case AEEOBEX_EVT_SRV_CONN_REQ:
         ...
         case AEEOBEX_EVT_SRV_GET_REQ:
         ...
         case AEEOBEX_EVT_ERROR:
      }
   }
   
   ===== To accept/ reject connection

   switch (event)
   {
      case AEEOBEX_EVT_SRV_CONN_REQ:
      
      // To accept the connection
      IOBEXSrvSession_AcceptConnection(pMe->piSsn, NULL, AEEOBEX_STAT_SUCCESS);

      OR

      // To Reject the connection, with an error status
      IOBEXSrvSession_AcceptConnection(pMe->piSsn, NULL, AEEOBEX_STAT_NO_RESOURCE);
   }

   ===== To authenticate the client before connection
   
   // in the event handler when there is a connection request 
   switch (event)
   {
      case AEEOBEX_EVT_SRV_CONN_REQ:

      IOBEXSrvSession_Authenticate();
   }
   ...
   ...

   // authentication completes
   switch (event)
   {
      case AEEOBEX_EVT_AUTH_COMPL:
      
      // To accept the connection
      IOBEXSrvSession_AcceptConnection(pMe->piSsn, NULL, AEEOBEX_STAT_SUCCESS);

      OR

      // To Reject the connection, with an error status
      IOBEXSrvSession_AcceptConnection(pMe->piSsn, NULL, AEEOBEX_STAT_NO_RESOURCE);
   }

   ===== To handle authentication challenge event 

   // In the event handler 
   switch (event)
   {
      ...

      case AEEOBEX_EVT_AUTH_CHALLENGE:

      // client has requested for authentication
      IOBEXSrvSession_GetAuthChallenge(pMe->piSsn,&uid,&acs,&charSet, &bufRealm, bufRelamLen, &reqLen);

      // get the password and user id ( if required) from the user
      IOBEXSrvSession_SetUidPassword(pMe->piSsn,uid, uidLen, pswd, pswdLen);

   }
   
   ===== To handle authentication response event 

   // In the event handler 
   switch (event)
   {
      case AEEOBEX_EVT_AUTH_RESPONSE:

      
      IOBEXSrvSession_GetAuthResponse(pMe->piSsn,&authResp);

      // get the password from the user and send to implementation
      IOBEXSrvSession_SetUidPassword(pMe->piSsn,NULL,0, pswd, pswdLen);
   }

   ===== To send response to the client

   // In the event handler 
   switch (event)
   {
      case AEEOBEX_EVT_SRV_GET_REQ:
      
      //  Parse the request
      ...

      // respond with the data
      ret = IOBEXSrvSession_CreateHeaderList(pMe->pSsn, &piHdr);

      if (AEE_SUCCESS == ret)
      {
         IOBEXHeader_AddByteSeq(piHdr, AEEOBEX_HDR_BODY, data, dataLen);
      
         IOBEXSrvSession_SendResponse(pMe->pSsn, AEEOBEX_REQ_GET, piHdr, AEEOBEX_STAT_CONTINUE, 0);

         // done sending the header list
         IOBEXHeader_Release(piHdr);
      }

   ===== To disconnect from the client

   IOBEXSrvSession_Disconnect(pMe->piSsn);

   // Object can no longer be used
   IOBEXSrvSession_Release(pMe->piSsn);

   ===== To handle event error

   switch (event)
   {
      case AEEOBEX_EVT_ERROR:

      // Object can no longer be used
      IOBEXSrvSession_Release(pMe->piSsn);
   }

   ===== To handle disconnect Request

   switch (event)
   {
      case AEEOBEX_EVT_SRV_DISC_REQ:
      IOBEXSrvSession_SendResponse(pMe->piSsn, AEEOBEX_REQ_DISCONNECT, NULL, AEEOBEX_STAT_SUCCESS, 0);
      IOBEXSrvSession_Release(pMe->piSsn);
   }

===/pre>
===============================================================================

Function :IOBEXSrvSession_AddRef()

Description:
   Inherited from IQI_AddRef().

See Also:
   IOBEXSrvSession_Release()

==============================================================================

IOBEXSrvSession_Release()

Description:
   Inherited from IQI_Release().

See Also:
   IOBEXSrvSession_AddRef()
   
==============================================================================

IOBEXSrvSession_QueryInterface()

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IOBEXSrvSession-derived class


Prototype:
   int IOBEXServer_QueryInterface (IOBEXSrvSession* po, AEECLSID clsReq, 
                                   void** ppo);

Parameters:
   po [i]     : the object
   clsReq [i] : Class ID of the interface to query.
   ppo [o]    : Pointer to the interface.

Return Value:
   AEE_SUCCESS  : on success, 
   Otherwise    : an error code.

Comments:
   After the API returns,if the content of ppo is a NULL pointer 
   the queried interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.


==============================================================================

Function: IOBEXSrvSession_CreateHeaderList

Description:
   The app can call this method to get a pointer to OBEX header list object.
   The App should add headers to this list by calling appropriate methods
   on the IOBEXHeader interface.After the app has sent this header with a 
   response, it should call IOBEXHeader_Release() to free the list.

Prototype:
  int IOBEXSrvSession_CreateHeaderList(IOBEXSrvSession  *po, 
                                       IOBEXHeader      **ppObexHdrList );

Parameters
   po [i]            : pointer to the IOBEXSrvSession  object
   ppObexHdrList [o] : place holder for pointer to IOBEXHeader Interface

Return Value:
   AEE_SUCCESS     : the header interface was returned
   AEE_ENOMEMORY   : no resources to create IOBEX header. This may 
                     happen if IOBEXHeader_Release()was not called
                     on previous header objects 

Event(s):
   None

See Also:
   IOBEXHeader

==============================================================================
Function : IOBEXSrvSession_Authenticate()

Description:
   This method will cause authentication to be initiated for the current request.
   The server may call this in response to a request from the client.
   Most often, the server would want to authenticate 
   the client when it receives a connect request.Calling this method
   will cause the authentication challenge header to be sent to the client.

Prototype:
   int IOBEXSrvSession_Authenticate( IOBEXSrvSession       *po,
                                     AEEOBEXCharSet        charSet,
                                     uint8                 *pRealm,
                                     int                   nRealmLen,
                                     boolean               bUidReqd,
                                     boolean               bFullAccess);
Parameters
   po [i]           : pointer to the object
   charSet [i]      : character set in which the realm string is encoded
   pRealm [i]       : pointer to realm information.
   nRealmLen [i]    : realm len 
   bUidReqd [i]     : userID Reqd.
                      TRUE if the server requires UserID from the client
                      FALSE, if the server does not require UID from the client.
   bFullAccess [i]  : Type of Access
                      TRUE, for full access
                      FALSE, for readonly access. 

Return Value:
   AEE_SUCCESS      : the information was set. AEEOBEX_EVT_AUTH_COMPL
                      is sent to the application to indicate the completion of 
                      authentication procedure
   AEE_EFAILED      : No resources to store the information.
   AEE_EUNSUPPORTED : sent when the implementation does not support authentication
                      for the current request.For e.g authentication may be 
                      supported only for the CONNECT request.
Event(s):
   AEEOBEX_EVT_AUTH_COMPL is sent when the authentication procedure is 
   complete.

See Also:
   AEEOBEXCharSet
   AEEOBEXAuthResp
   
   
==============================================================================

Function: IOBEXSrvSession_SetUidPassword

Description:
   The application should call this function to provide information that is 
   required for authentication.The API should be called when the events
   AEEOBEX_EVT_AUTH_RESPONSE and AEEOBEX_EVT_AUTH_CHALLENGE are received.
   
   When called in response to AEEOBEX_EVT_AUTH_CHALLENGE the application should
   send user ID and password for which the client should grant
   access.

   When called in response to  AEEOBEX_EVT_AUTH_RESPONSE, user ID is the ID
   obtained from  AEEOBEXAuthResp struct that accompanies the event.
   The password corresponds to the Uid obtained from AEEOBEXAuthResp.


Prototype:
   int IOBEXSrvSession_SetUidPassword(IOBEXSrvSession  *po,
                                      const uint8      *pUserID,
                                      int              nUidLen,
                                      const uint8      *pPassword
                                      int              nPswdLen );
Parameters
   po [i]         : pointer to the IOBEXSrvSession object
   pUserID [i]    : pointer to userID
   nUidLen [i]    : user id length
   pPassword [i]  : password for userID
   nPswdLen [i]   : password length

Return Value:
   AEE_SUCCESS    : the userId and password were successfully copied.
   AEE_ENOMEMORY  : could not save parameters due to lack of memory
   AEE_EBADPARM   : invalid paramaters

See Also:
   AEEOBEXAuthResp
   

===============================================================================

Function: IOBEXSrvSession_AcceptConnection()

Description:
   This method accepts or rejects a CONNECT request from an OBEX client.

Prototype:
   int IOBEXSrvSession_AcceptConnection( IOBEXSrvSession  *p,
                                         IOBEXHeader      *pHdr,
                                         AEEOBEXCmdStatus cmdStat);
Parameters
   po [i]        : pointer to the IOBEXSrvSession object
   pHdr [i]      : pointer to the headers to be sent to the OBEX client.
   cmdStatus [i] : status to be sent in the connect response
                   AEEOBEX_STAT_SUCCESS indicates connect request
                   should be accepted
                   All other status values indicate that connection
                   should not be accepted.

Return Value:
   AEE_SUCCESS   : the request to accept/reject the connection request was 
                   queued.
   AEE_ENOMEMORY : could not queue command due to lack of memory
   AEE-EBADPARM  : invalid parameters

Event(s):
   AEEOBEX_EVT_CONN_COMPL is sent when the connection procedure is 
   complete.

See Also:
   IOBEXHeader
   AEEOBEXCmdStatus
   
===============================================================================

Function: IOBEXSrvSession_SendResponse()

Description:
   This method sends a response to the GET,PUT,SETPATH, DISCONNECT request
   that was received from a client.

Prototype:
   int IOBEXSrvSession_SendResponse( IOBEXSrvSession      *po,
                                     AEEOBEXRequestType   request,
                                     IOBEXHeader          *pOBEXHdr,
                                     AEEOBEXCmdStatus     cmdStatus,
                                     AEEOBEXFlags         flags );

Parameters
   po [i]         : pointer to the IOBEXServer object
   request[i]     : the request type for E.g SET PATH, GET, PUT for which this 
                    response is being sent
   pObexHdr [i]   : pointer to the IOBEXHeader interface that contains 
                    list of headers to be sent to OBEX client
   cmdStatus [i]  : the status to be sent in the OBEX response
   flags [i]      : flags: currently not used
   
Return Value:
   AEE_SUCCESS    : the response has been queued
   AEE_EBADPARM   : invalid parameters
   AEE_EBADSTATE  : bad state
   AEE_EFAILED    : unable to send the OBEX response

Event(s):
   None

See Also:
   IOBEXHeader
   AEEOBEXCmdStatus
   AEEOBEXRequestType

===============================================================================

Function: IOBEXSrvSession_Disconnect 

Description:
   This method forcefully severs the connection with the client.It may involve
   tearing down of the transport connection.

Prototype:
   int IOBEXSrvSession_Disconnect(IOBEXSrvSession *po);

Parameters
   po [i]       : pointer to the IOBEXServer object

Return Value:
   AEE_SUCCESS   : the request to disconnect from remote device was queued.
   AEE_ENOMEMORY : could not queue command due to lack of memory
   AEE_EBADSTATE : bad state


===============================================================================

Function: IOBEXSrvSession_OnEventNotify()

Description:
   Registers a signal which is set when the application is to be notified of an
   event.

Prototype:
   IOBEXSrvSession_OnEventNotify(IOBEXSrvSession *p,
                                 ISignal         *piSignal );
Parameters
   po [i]       : pointer to the IOBEXSrvSession object 
   piSignal [i] : signal that is set when an event is to be delivered to the app

Return Value:
   AEE_SUCCESS   : the signal was successfully registered
   AEE_EBADPARAM : bad parameters

See Also:
   IOBEXSrvSession_GetEvent()

===============================================================================

Function: IOBEXSrvSession_GetAuthChallenge()

Description:
   This function should be called to retrieve the authentication challenge
   parameters when the application receives the AEEOBEX_EVT_AUTH_CHALLENGE
   event.

Prototype:
   int IOBEXSrvSession_GetAuthChallenge(IOBEXSrvSession  *po,
                                        boolean         *pbUidReq,
                                        boolean         *pbFullAccess,
                                        AEEOBEXCharSet  *pCharSet,
                                        uint8           *pRealm,
                                        int             nRealmLen,
                                        int             *pnRealmLenReq )

Parameters
   po [i]             : pointer to the IOBEXSrvSession object
   pbUidReq [o]       : indicates if the client requested for Uid
   pbFullAccess [o]   : place holder for access granted by the client
   pCharSet [o]       : character set used for encoding the realm
   pRealm [o]         : realm string
   nRealmLen [i]      : buffer allocated for the realm string
   pnRealmLenReq [o]  : if the buffer allocated for the realm
                        string is insufficient, this location contains the 
                        required buffer length

Return Value:
   AEE_SUCCESS         : information successfully retrieved
   AEE_EBUFFERTOOSMALL : buffer size insufficient for realm string
   AEE_EBADPARM        : invalid paramaters
   AEE_ENOSUCH         : No auth challenge params present

See Also:
   AEEOBEXCharSet

=============================================================================

Function: IOBEXSrvSession_GetAuthResponse()

Description:
   This function should be called to retrieve the authentication response
   parameters when the application receives the AEEOBEX_EVT_AUTH_RESPONSE
   event.

Prototype:
   int IOBEXSrvSession_GetAuthResponse(IOBEXSrvSession  *po,
                                       AEEOBEXAuthResp  *pAuthResp)
Parameters
   po [i]          : pointer to the IOBEXSrvSession object
   pAuthResp [o]   : pointer to the buffer to read authentication response
                     parameters
   
Return Value:
   AEE_SUCCESS      : information successfully retrieved
   AEE_EBADPARM     : invalid paramaters
   AEE_ENOSUCH      : No auth response params present

See Also:
   AEEOBEXAuthResp

=============================================================================

Function: IOBEXSrvSession_GetEvent()

Description:
   After the signal registered via IOBEXSrvSession_OnEventNotify() is triggered,
   the application can retrieve the event by calling this method.
   
   The list of events and corresponding data structures are as follows:
===/pre>

   Events                           Parameters that contain data
----------------------------------------------------------------------------------------------------------------
   AEEOBEX_EVT_SRV_CONN_REQ       : ppiOBEXHdr
   AEEOBEX_EVT_SRV_DISC_REQ       : ppiOBEXHdr
   AEEOBEX_EVT_SRV_PUT_REQ        : ppiOBEXHdr, pEventData->packet
   AEEOBEX_EVT_SRV_GET_REQ        : ppiOBEXHdr, pEventData->packet 
   AEEOBEX_EVT_SRV_SETPATH_REQ    : ppiOBEXHdr, pEventData->flags
   AEEOBEX_EVT_SRV_CONN_COMPL     : pEventData->cmdStatus
   AEEOBEX_EVT_AUTH_CHALLENGE     : IOBEXSrvSession_GetAuthChallenge()
                                    to be invoked to read auth response 
                                    parameters
   AEEOBEX_EVT_AUTH_RESPONSE      : IOBEXSrvSession_GetAuthResponse()
                                    to be invoked to read auth response 
                                    parameters
   AEEOBEX_EVT_AUTH_COMPL         : pEventData->cmdStatus
   AEEOBEX_EVT_ERROR              : pEventData->cmdStatus
===/pre>

Prototype:
   int IOBEXServer_GetEvent(IOBEXSrvSession    *p,
                            AEEOBEXEventType   *pEventId,
                            IOBEXHeader        *ppiOBEXHdr,
                            AEEOBEXEvent       *pEventData);
                            
Parameters
   po [i]         : pointer to the IOBEXSrvSession object 
   pEventId[o]    : place holder for event type
   ppiOBEXHdr [o] : optional headers sent with the request
   pEventData [o] : event Data
   
Return Value:
   AEE_SUCCESS    : the data was successfully read
   AEE_EBADPARAM  : bad parameters
   AEE_ENOMORE    : No event to deliver to the application 

See Also:
   AEEOBEXEventType
   IOBEXHeader
   AEEOBEXEvent
   IOBEXSrvSession_OnEventNotify()

===============================================================================

Function: IOBEXSrvSession_GetParams()

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
   AEEOBEXPARAM_BT_RFCOMM_SCN       uint32
   AEEOBEXPARAM_MAX_PACKET_SIZE     uint32


Prototype:
   int IOBEXSrvSession_GetParams( IOBEXSrvSession   *po,
                                  AEEOBEXParamType  paramType,
                                  uint32            *pU32,
                                  uint8             *pParam,
                                  int               nParamLen,
                                  int               *pnParamLenReq);
Parameters
   po [i]              : pointer to the IOBEXClient object
   paramType [i]       : parameter type
   pU32[o]             : place holder for a 4 byte parameter
   pParam [o]          : buffer to hold the parameter 
   nParamLen [i]       : length of the buffer
   pnParamLenReq [o]   : if return value is AEE_EBUFFERTOOSMALL, this
                         location contains length of the buffer
                         required to hold the parameter

Return Value:
   AEE_SUCCESS         : parameters returned successfully
   AEE_EBADPARAM       : invalid input  parameters
   AEE_EBUFFERTOOSMALL : buffer size that was passed to this function 
                         is too small.
   AEE_ENOSUCH         : no such parameter available 
See Also :
   AEEOBEXParamType


===============================================================================*/
#endif /* AEEIOBEXSRVSESSION*/


