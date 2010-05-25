#ifndef AEEIOBEXSRV_H
#define AEEIOBEXSRV_H
/*=============================================================================
FILE:         AEEIOBEXSrv.h

SERVICES:     IOBEXSrv Interface

DESCRIPTION:  IOBEXSrv interface that supports OBEX protocol
              in server role.

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
#include "AEEIOBEXSrvSession.h"
#include "AEEISignal.h"

/*=============================================================================
 Interface ID
=============================================================================*/

#define AEEIID_IOBEXSrv          0x01053c88



/*============================================================================
 OBEX Server Interface Definition
============================================================================*/

#define INHERIT_IOBEXSrv(iname) \
   INHERIT_IQI(iname);\
                                                                      \
   int (*GetSupportedTransports)(iname             *po,               \
                                 AEEOBEXTransport  *pTransports,      \
                                 int               nLen,              \
                                 int               *pnLenReq);        \
                                                                      \
   int (*Register)        (iname                   *p,                \
                           AEEOBEXTransport        transType,         \
                           uint8                   *pTarget,          \
                           int                     nTargetLen,        \
                           const char              *pszTransport);    \
                                                                      \
   int (*Deregister)      (iname                   *po);              \
                                                                      \
   int (*OnEventNotify)   (iname                   *po,               \
                           ISignal                 *piSignal);        \
                                                                      \
   int (*GetEvent)        (iname                   *po,               \
                           AEEOBEXEventType        *pdwId,            \
                           AEEOBEXCmdStatus        *pCmdStatus,       \
                           IOBEXSrvSession         **ppiSrvSession);  \
                                                                      \
   int (*GetParams)       (iname                   *po,               \
                           AEEOBEXParamType        paramType,         \
                           uint32                  *pU32,             \
                           uint8                   *pParam,           \
                           int                     nParamLen,         \
                           int                     *pnParamLenReq)
AEEINTERFACE_DEFINE(IOBEXSrv);


/*============================================================================
 Interface Accessor Functions for IOBEXSrv
============================================================================*/

static __inline uint32 IOBEXSrv_AddRef( IOBEXSrv *p )
{
   return AEEGETPVTBL((p),IOBEXSrv)->AddRef( p );
}

static __inline uint32 IOBEXSrv_Release( IOBEXSrv *p )
{
   return AEEGETPVTBL((p),IOBEXSrv)->Release( p );
}

static __inline int IOBEXSrv_QueryInterface( IOBEXSrv *p,
                                             AEECLSID idReq, void **ppo )
{
   return AEEGETPVTBL((p),IOBEXSrv)->QueryInterface(p, idReq, ppo);
}

static __inline int IOBEXSrv_GetSupportedTransports( IOBEXSrv *p,
                                                     AEEOBEXTransport *pTransport,
                                                     int              nLen,
                                                     int              *pnLenReq)
{
   return AEEGETPVTBL((p),IOBEXSrv)->GetSupportedTransports( p ,pTransport, nLen, pnLenReq );
}

static __inline int IOBEXSrv_Register( IOBEXSrv          *p,
                                       AEEOBEXTransport  transType,
                                       uint8             *pTarget,
                                       int               nTargetLen,
                                       const char        *pszTransport)
{
   return AEEGETPVTBL((p),IOBEXSrv)->Register( p, transType,
                                               pTarget,nTargetLen,pszTransport);
}

static __inline int IOBEXSrv_Deregister( IOBEXSrv *p )
{
   return AEEGETPVTBL((p),IOBEXSrv)->Deregister( p );
}

static __inline int IOBEXSrv_OnEventNotify(IOBEXSrv  *p,
                                           ISignal   *piSignal )
{
   return AEEGETPVTBL((p),IOBEXSrv)->OnEventNotify( p ,piSignal );
}

static __inline int IOBEXSrv_GetEvent(IOBEXSrv           *p,
                                      AEEOBEXEventType   *pEventID,
                                      AEEOBEXCmdStatus   *pCmdStatus,
                                      IOBEXSrvSession    **ppiSrvSession)
{
   return AEEGETPVTBL((p),IOBEXSrv)->GetEvent( p ,pEventID, pCmdStatus,
                                               ppiSrvSession);
}
    
static __inline int IOBEXSrv_GetParams( IOBEXSrv       *p,
                                        AEEOBEXParamType  param,
                                        uint32            *pU32,
                                        uint8             *pParam,
                                        int               nParamLen,
                                        int               *pnParamLenReq)
{
   return AEEGETPVTBL((p),IOBEXSrv)->GetParams( p, param, pU32,pParam, nParamLen,
                                                pnParamLenReq);
}

/*=============================================================================
   INTERFACE DOCUMENTATION
=========================================================================

Interface Name: IOBEXSrv

Description: (Generic) OBEX Profile

   This interface enables BREW applications to use OBEX protocol to
   exchange data objects with a remote device.Each instance of 
   IOBEXSrv represents an OBEX server.A BREW application can instantiate
   multiple Server Objects each servicing multiple clients provided the
   underlying implementation supports this. Requests from a client are handled
   by the IOBEXSrvSession Interface.
   The server can be registered over different transports.The supported
   transports are available to the application via 
   IOBEXSrv_GetSupportedTransports().

   IOBEXSrv_OnEventNotify() should be called to register a signal that 
   will be set when the application is to be notified of an event.The 
   event is retrieved by calling IOBEXSrv_GetEvent().
   An OBEX server can be registered by calling IOBEX_ServerRegister().When 
   the server receives a request from a new client, AEEOBEX_EVT_SRV_NEW_CLI
   is sent to the application. The data corresponding to this event is a 
   pointer to session object.The application can service 
   requests from this client using the session object.
   To deregister the server, the application should call IOBEXSrv_Deregister()
   De registering the server will terminate any sessions that were being serviced.

===H2>
   Usage example:
===/H2>
 ===pre>  
   ===== To Register for events;

   // create a signal for example, use ISignalCBFactory_CreateSignal()
   IOBEXSrv_OnEventNotify(pMe->piSrv, piSignal);

   ===== To retrieve and handle events when signal is triggered:

   // re-enable signal
   ISignalCtl_Enable (pMe->piSrvSignalCtl); 
   
   while ((retValue = IOBEXSrv_GetEvent (pMe->piSrv, &event, &cmdStat, &pSsn)) != AEE_ENOMORE)
   {
      // if retValue == AEE_SUCCESS;

      switch (event)
      {
         case AEEOBEX_EVT_SRV_REG_CMPL:
         ...
         case AEEOBEX_EVT_SRV_DEREG_CMPL:
         ...
         case AEEOBEX_EVT_SRV_NEW_CLI:
      }
   }
   ===== To Register a server:

   // see if the transport is supported 
   IOBEXSrv_GetSupportedTransports(pMe->piSrv, &transport, transLen, &reqTransLen);

   // set the required transport parameters.For E.g if transport is Bluetooth RFCOMM
   IOBEXSrv_Register(pMe->piSrv,
                     AEEOBEX_TRANS_BT_RFCOMM, NULL, 0,
                     "service_name=ftp server;bt_security=1;uuid16=08"

   ===== To DeRegister a server:

   // disconnect and release any existing sessions
   if (pMe->pSsn) 
   {
      IOBEXSrvSession_Disconnect(pMe->pSsn);
      IOBEXSrvSession_Release(pMe->pSsn);
   }

   IOBEXSrv_Deregister(pMe->piSrv, NULL);

   ===== To handle new session
   // in event handler

   switch (event)
   {
      case AEEOBEX_EVT_SRV_NEW_CLI:
      // new session
      // create a signal for example, use ISignalCBFactory_CreateSignal()
      IOBEXSrvSession_OnEventNotify(piSsn, piSignal);

      // Handle session events
      ...
   }
===/pre>

==============================================================================

IOBEXSrv_AddRef()

Description:
   Inherited from IQI_AddRef().

See Also:
   IOBEXSrv_Release()

==============================================================================

IOBEXSrv_Release()

Description:
   Inherited from IQI_Release().

See Also:
   IOBEXSrv_AddRef()
   
==============================================================================

IOBEXSrv_QueryInterface()

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IOBEXSrv-derived class


Prototype:
   int IOBEXSrv_QueryInterface (IOBEXSrv* po, AEECLSID clsReq, 
                                   void** ppo);

Parameters:
   po [i]     : the object
   clsReq [i] : Class ID of the interface to query.
   ppo [o]    : Pointer to the interface.

Return Value:
   AEE_SUCCESS : on success, 
   Otherwise   : an error code.

Comments:
   After the API returns,if the content of ppo is a NULL pointer, 
   the queried interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.


==============================================================================
Function: IOBEXSrv_GetSupportedTransports

Description:
   This function is called by Server application to retrieve the supported
   transports.

Prototype:
   int  IOBEXSrv_GetSupportedTransports(IOBEXSrv         *po,
                                        AEEOBEXTransport *pTransport,
                                        int              nLen,
                                        int              *pnLenReq);

Parameters
   po [i]         : pointer to the IOBEXSrv object
   pTransport [o] : array of supported transports
   nLen [i]       : number of elements in the array
   pnLenReq[o]    : number of elements required in the array to return all
                    transports

Return Value:
   AEE_SUCCESS         : the information was returned successfully
   AEE_BADPARAM        : Bad parameters
   AEE_EBUFFERTOOSMALL : the number of elements required in the array is 
                         insufficient to return all transports

See Also :
AEEOBEXTransport

==============================================================================
IOBEXSrv_Register()

Description: 
   This method registers an OBEX server over a particular transport.
   Transport parameters are sent to the implementation as a null terminated
   string containing name-value pairs separated by a delimiter ';'.The format
   of the string is "<name1>=<value1>;<name2>=<value2>". For E.g if the 
   transport is Bluetooth RFCOMM ,the transport parameters could be the 
   service name and security(AEEBT_SEC_AUTHENTICATE). The transport
   string would be "service_name=ftp server;security=1". 
   For a list of name-value pairs see AEEOBEXTransport.
   

Prototype:
   IOBEXSrv_Register( IOBEXSrv             *p,
                      AEEOBEXTransport     transType,
                      uint8                *pTarget,
                      int                  nTargetLen,
                      const char           *pszTransport)
Parameters
   p [i]            : pointer to the IOBEXSrv object
   transType [i]    : transport type
   pszTarget [i]    : buffer containing target Id
   nLen [i]         : target Id length  
   pszTransport [i] : null terminated string containing transport 
                      parameters

Return Value:

   AEE_SUCCESS      : the request to register the server was initiated, wait for 
                      AEEOBEX_EVT_SRV_REG_CMPL to be sent to the application 
   AEE_ENOMEMORY    : no memory
   AEE_EBADPARM     : bad parameters or unsupported transport

Event(s):             AEEOBEX_EVT_SRV_REG_CMPL is sent to the application when 
                      registration is complete.
See Also:
   AEEOBEXTransport
   AEEOBEXSrvRegInfo
   IOBEXSrv_Deregister()


===============================================================================

Function: IOBEXSrv_Deregister()

Description:
   De Registers the OBEX server.The server will no longer be available to service
   requests from clients.

Prototype:
   int IOBEXSrv_Deregister(IOBEXSrv  *po );

Parameters
   po [i]     : pointer to the IOBEXSrv object 

Return Value:
   AEE_SUCCESS : the request to deregister the server was queued,
                 AEEOBEX_EVT_SRV_DEREG_CMPL will be sent at a later point
   AEE_EFAILED : unexpected error

Event(s):        AEEOBEX_EVT_SRV_DEREG_CMPL is sent to the application when 
                 de-registration is complete.
See Also:
   IOBEXSrv_Register()

===============================================================================

Function: IOBEXSrv_OnEventNotify()

Description:
   Registers a signal which is set when the application is to be notified of an
   event.

Prototype:
   IOBEXSrv_OnEventNotify(IOBEXSrv    *p,
                          ISignal     *piSignal );
Parameters
   po [i]         : pointer to the IOBEXSrv object 
   piSignal [i]   : signal that is set when an event is to be delivered to the app

Return Value:
   AEE_SUCCESS    : the signal was successfully registered
   AEE_EBADPARM   : bad parameters

See Also:
   IOBEXSrv_GetEvent()

===============================================================================

Function: IOBEXSrv_GetEvent()

Description:
   After the signal registered via IOBEXSrv_OnEventNotify() is triggered,
   the application can retrieve the event by calling this method.
   
   The list of events and corresponding data are as follows:
===pre>
   Events                                                         Data structure 
------------------------------------------------------------------------------------
   AEEOBEX_EVT_SRV_REG_CMPL           AEEOBEXCmdStatus
   AEEOBEX_EVT_SRV_DEREG_CMPL      AEEOBEXCmdStatus
   AEEOBEX_EVT_SRV_NEW_CLI            IOBEXSrvSession
===/pre>
Prototype:
   int IOBEXSrv_GetEvent(IOBEXSrv           *p,
                         AEEOBEXEventType   *pEventId,
                         AEEOBEXCmdStatus   *pCmdStatus,
                         IOBEXSrvSession    **ppiSrvSession)
Parameters
   po [i]            : pointer to the IOBEXSrv object 
   pEventId [o]      : event type
   pCmdStatus [o]    : command status
   ppiSrvSession [o] : on return of the API, this location will contain
                       pointer to the server session object if 
                       event is AEEOBEX_EVT_SRV_NEW_CLI. 
                       NULL otherwise. 

Return Value:
   AEE_SUCCESS       : the event was successfully read
   AEE_EBADPARM      : bad parameters
   AEE_ENOMORE       : No event to deliver to the application 

See Also:
   AEEOBEXEventType
   IOBEXSrv_OnEventNotify()

===============================================================================

Function: IOBEXSrv_GetParams()

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


Prototype:
   int IOBEXSrv_GetParams( IOBEXSrv          *po,
                           AEEOBEXParamType  paramType,
                           uint32            *pU32,
                           uint8             *pParam,
                           int               nParamLen,
                           int               *pnParamLenReq);
Parameters
   po [i]              : pointer to the IOBEXSrv object
   paramType [i]       : parameter type
   pU32 [o]            : place holder for a 4 byte value
   pParam [o]          : buffer to hold the parameter 
   nParamLen [i]       : length of the buffer
   pnParamLenReq [o]   : if return value is AEE_EBUFFERTOOSMALL, this 
                         location contains length of the buffer
                         required to hold the parameter

Return Value:
   AEE_SUCCESS         : parameters returned successfully
   AEE_EBADPARM        : invalid parameters
   AEE_EBUFFERTOOSMALL : buffer size that was passed to this function 
                         is insufficient.
   AEE_ENOSUCH         : no such parameter available 

See Also:
   AEEOBEXParamType


===============================================================================*/
#endif /*AEEOBEXSERVER_H*/

