#ifndef AEEBTSPPSERVER_H
#define AEEBTSPPSERVER_H
/*=============================================================================
FILE:         AEEBTSPPServer.h

SERVICES:     IBTSPP server Interfaces

DESCRIPTION:  IBTSPPServer is the BREW interface that supports Bluetooth Serial
              Port server Profile.
===============================================================================
Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
#include "AEEBTSPP.h"

/*=============================================================================
 Interface ID
=============================================================================*/
#define AEEIID_BT_SPP_SERVER       0x01041694

/*============================================================================
 SPP Server Interface Definition
============================================================================*/
typedef struct IBTSPPServer IBTSPPServer;

AEEINTERFACE(IBTSPPServer)
{
   INHERIT_IBTSPP(IBTSPPServer);

   int (*EnableService) (IBTSPPServer*      po,
                         BTSecurityLevel    secLevel,
                         uint32             uServerChannel,
                         boolean            bUnRegSdp,
                         BTResult*          pResult,
                         AEECallback*       pResultCb);
   int (*DisableService)(IBTSPPServer*      po,
                         BTResult*          pResult,
                         AEECallback*       pResultCb);
   int (*OnConnRequest) (IBTSPPServer*      po,
                         AEECallback*       pAuthCb,
                         BTEventData*       pEvData);
   int (*Authorize)     (IBTSPPServer*      po,
                         const BDAddress*   pBDAddr,
                         boolean            bAccept,
                         BTResult*          pResult,
                         AEECallback*       pResultCb);
};

// Standard IPort Methods
static __inline uint32 IBTSPPSRV_AddRef (IBTSPPServer* p)
{
   return AEEGETPVTBL((p),IBTSPPServer)->AddRef((p));
}

static __inline uint32 IBTSPPSRV_Release (IBTSPPServer* p)
{
   return AEEGETPVTBL((p),IBTSPPServer)->Release((p));
}

static __inline int IBTSPPSRV_QueryInterface (IBTSPPServer* p,
                                              AEECLSID      clsid,
                                              void**        ppo )
{
   return AEEGETPVTBL((p),IBTSPPServer)->QueryInterface((p),(clsid),(ppo));
}

static __inline int32 IBTSPPSRV_Read(IBTSPPServer* p, 
                                     char* pcBuf,
                                     int32 cbBuf )
{
   return AEEGETPVTBL((p),IBTSPPServer)->Read((p),(pcBuf),(cbBuf));
}

static __inline void IBTSPPSRV_Readable(IBTSPPServer* p,
                                        AEECallback* pcb )
{
   AEEGETPVTBL((p),IBTSPPServer)->Readable((p),(pcb));
}

static __inline int IBTSPPSRV_GetLastError(IBTSPPServer* p)
{
   return AEEGETPVTBL((p),IBTSPPServer)->GetLastError((p));
}

static __inline int32 IBTSPPSRV_Write(IBTSPPServer* p, 
                                      const char* pcBuf,
                                      int32 cbBuf )
{
   return AEEGETPVTBL((p),IBTSPPServer)->Write((p),(pcBuf),(cbBuf));
}

static __inline void IBTSPPSRV_Writeable(IBTSPPServer* p,
                                        AEECallback* pcb )
{
   AEEGETPVTBL((p),IBTSPPServer)->Writeable((p),(pcb));
}

static __inline int IBTSPPSRV_IOCtl (IBTSPPServer* p,
                                     int option,
                                     int val )
{
   return AEEGETPVTBL((p),IBTSPPServer)->IOCtl((p),(option),(val));
}

static __inline int IBTSPPSRV_Close(IBTSPPServer* p)
{
   return AEEGETPVTBL((p),IBTSPPServer)->Close((p));
}

static __inline int IBTSPPSRV_Open (IBTSPPServer* p,
                                    const char* szPort)
{
   return AEEGETPVTBL((p),IBTSPPServer)->Open((p),(szPort));
}

// Standard SPP Methods
static __inline int IBTSPPSRV_OnStatusChange (IBTSPPServer* p, 
                                              AEECallback* pRCb)
{
   return AEEGETPVTBL((p),IBTSPPServer)->OnStatusChange((p),(pRCb));
}

static __inline int IBTSPPSRV_OnModemStatusChange (IBTSPPServer* p, 
                                                   AEECallback* pRCb)
{
   return AEEGETPVTBL((p),IBTSPPServer)->OnModemStatusChange((p),(pRCb));
}

static __inline int IBTSPPSRV_OnLineError (IBTSPPServer* p, 
                                           AEECallback* pRCb)
{
   return AEEGETPVTBL((p),IBTSPPServer)->OnLineError((p),(pRCb));
}

static __inline int IBTSPPSRV_Disconnect (IBTSPPServer* p,
                                          BTResult* pR, 
                                          AEECallback* pRCb)
{

   return AEEGETPVTBL((p),IBTSPPServer)->Disconnect((p),(pR),(pRCb));
}

static __inline int IBTSPPSRV_GetStatus (IBTSPPServer* p,
                                         BTSPPStatus* pS)
{

   return AEEGETPVTBL((p),IBTSPPServer)->GetStatus((p),(pS));
}

static __inline int IBTSPPSRV_GetModemStatus (IBTSPPServer* p,
                                              BTSPPModemStatus* pS)
{

   return AEEGETPVTBL((p),IBTSPPServer)->GetModemStatus((p),(pS));
}

static __inline int IBTSPPSRV_GetLineError (IBTSPPServer* p,
                                            BTSPPLineError* pLineError)

{
   return AEEGETPVTBL((p),IBTSPPServer)->GetLineError((p),(pLineError));
}

static __inline int IBTSPPSRV_Configure ( IBTSPPServer* p, 
                                          const BTSPPConfig* pCfg,
                                          BTResult* pR,
                                          AEECallback* pRCb)
{
   return AEEGETPVTBL((p),IBTSPPServer)->Configure((p),(pCfg),(pR),(pRCb));
}

static __inline int IBTSPPSRV_IOCtlEx  ( IBTSPPServer* p, 
                                         BTSPPIOCtlCommand IOctlCmd,
                                         BTSPPModemStatus* pModemStatus, 
                                         AEECallback* pResultCb)
{
   return AEEGETPVTBL((p),IBTSPPServer)->IOCtlEx((p),(IOctlCmd),(pModemStatus),
                                                 (pResultCb));
}


// SPP Server Methods
static __inline int IBTSPPSRV_EnableService( IBTSPPServer* p,
                                             BTSecurityLevel sl,
                                             int  scn,
                                             boolean  bur,
                                             BTResult* pr,
                                             AEECallback* pRcb )
{


   return AEEGETPVTBL((p),IBTSPPServer)->EnableService((p),(sl),(scn),
                                                       (bur),(pr),(pRcb));

}

static __inline int IBTSPPSRV_DisableService( IBTSPPServer* p,
                                              BTResult* pR, 
                                              AEECallback* pRcb)
{


   return AEEGETPVTBL((p),IBTSPPServer)->DisableService((p),(pR),(pRcb));

}

static __inline int IBTSPPSRV_OnConnRequest( IBTSPPServer*  p,
                                             AEECallback*   pCb,
                                             BTEventData*   pEv )
{


   return AEEGETPVTBL((p),IBTSPPServer)->OnConnRequest((p),(pCb),(pEv));

}

static __inline int IBTSPPSRV_Authorize ( IBTSPPServer* p,
                                          const BDAddress* pBDAddr, 
                                          boolean bAccept,
                                          BTResult* pResult, 
                                          AEECallback* pResultCb )

{


   return AEEGETPVTBL((p),IBTSPPServer)->Authorize((p),(pBDAddr),(bAccept),
                                                   (pResult),(pResultCb));

}

/*============================================================================
INTERFACE DOCUMENTATION
==============================================================================
IBTSPPServer Interface

Description:
   This interface enables BREW applications to use Bluetooth Serial Port 
   Profile (SPP) to exchange data with a remote device.  Each instance of 
   IBTSPPServer supports one connection in a server role.  A BREW application
   can instantiate multiple server objects each serving an SPP client.  
   A BREW application can instantiate one IBTSPPServer object and one 
   IBTSPPClient object. Both these objects can be in a connection provided
   the underlying Bluetoth implementation supports this.

   Typically, an SPP Server application would create an SPP object using 
   IBT_GetSPPServer() and then call IBTSPPSRV_OnConnRequest() to register an 
   authorization callback with BREW to be invoked when a connection request
   comes in.It should then enable SPP service with IBTSPPSRV_EnableService().
   Depending on the security settings, the authorization callback and the PIN 
   key notification could arrive in different orders.
   When the Authorize callback is invoked, server applications can accept/reject
   the request using IBTSPPSRV_Authorize().

   For an application to support SPP Client role, after getting an IBTSPPClient 
   object via IBT_GetSPPClient(), it should search for the SPP server using 
   IBTSPPCLI_FindServers() which may result in multiple SPP servers to select
   from.The client application can then try to connect to a particular SPP
   server by calling function IBTSPPCLI_Connect().

   If everything goes well and connection is established, the applications
   should call Readable() function to register a callback to be invoked
   when data is received from remote device.  When the Readable callback
   is invoked,the application should retrieve the received data by calling
   the Read() function.For sending data to remote device, the applications
   should call Write() function until the function returns AEEPORT_WAIT
   at which time the applications should call IBTSPP_Writeable() to register
   a Writeable callback.

   Either side can terminate the connection using Disconnect().The Client
   application would be informed of a server initiated disconnect when the 
   status of the stream goes to AEEBTSPP_ST_CLOSED.
   The server application would be informed of a client initiated disconnect 
   when the status of the stream goes back to AEEBTSPP_ST_OPENED.

==============================================================================
IBTSPPSRV_AddRef()

Description:
   This function is inherited from IBASE_AddRef().

See Also:
   IBTSPPSRV_Release()
   
==============================================================================
IBTSPPSRV_Release()

Description:
   This function is inherited from IBASE_Release().

See Also:
   IBTSPPSRV_AddRef()
   
==============================================================================
IBTSPPSRV_QueryInterface()

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IBTSPP-derived class

Prototype:
   int IBTSPPSRV_QueryInterface (IBTSPPServer* po, AEECLSID clsReq, 
                                 void** ppo);
   
Parameters:
   po [i]     : the object
   clsReq [i] : Class ID of the interface to query.
   ppo [o]    : Pointer to the interface.

Return Value:
   SUCCESS: on success, 
   Otherwise: an error code.

Comments:
   If ppo is back a NULL pointer, the queried interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.

==============================================================================
IBTSPPSRV_Read()

Description:
   Inherited from IPORT_Read().
   Retrieve the received data from remote device.If pDest == NULL,
   the function tries to flush maxBytes2Read bytes from the receive buffer.
   
Prototype:
   int32 IBTSPPSRV_Read(IBTSPPServer* po, char* pDest, int32 maxBytes2Read);
   
Parameters:
   po [i]             : the object
   pDest [o]          : destination for received data
   maxBytes2Read [i]  : number of bytes destination can hold

Return values:
   number of bytes received/ flushed, a value greater than 0
   AEEPORT_WAIT       : no data available
   AEEPORT_ERROR      : invalid parameters or no SPP stream

See Also:
   IBTSPPSRV_Readable()

==============================================================================
IBTSPPSRV_Readable()

Description:
   Inherited from IPORT_Readable().
   Registers a function to be invoked when receive queue has data.
      
Prototype:
   void IBTSPPSRV_Readable(IBTSPPServer* po, AEECallback* pcb);
  
Parameters:
   po [i]     : the object
   pcb [i]    : pre-initialized AEECallback pointing to function to be called
                when receive queue is no longer empty

Return Value:
	None

See Also:
   IBTSPPSRV_Read()

==============================================================================
IBTSPPSRV_GetLastError()

Description:
   Inherited from IPORT_GetLastError().
   This function returns the last error that occurred with Read() or Write().

Prototype:
   int IBTSPPSRV_GetLastError(IBTSPPServer* po);
  
Parameters:
   po [i]     : the object
   
Return Value:
   The most recently occurred error.  This could be any BREW error code.
   The most common ones are listed below.

   AEEPORT_ENOTOPEN  : SPP port is not open
   EBADSTATE         : SPP port is in an incompatible state

See Also:
   IBTSPPSRV_Read()
   IBTSPPSRV_Write()

==============================================================================
IBTSPPSRV_Write()

Description:
   Inherited from IPORT_Write().
   This function sends data to remote device.
   
Prototype:
   int32 IBTSPPSRV_Write(IBTSPPServer* po, const char* pSrc, 
                         int32 bytes2Write);
 
Parameters:
   po [i]           : the object
   pSrc [i]         : data source
   bytes2Write [i]  : number of bytes in source

Return values:
   number of bytes queued to be sent, a value greater than 0
   AEEPORT_WAIT     : if the transmit queue is full
   AEEPORT_ERROR    : invalid parameters or no SPP stream

See Also:
   IBTSPPSRV_Writeable()
   
==============================================================================
IBTSPPSRV_Writeable()

Description:
   Inherited from IPORT_Writeable().
   Registers a callback to be invoked when transmit queue can accept more 
   data to be sent out.  
   
Prototype:
   void IBTSPPSRV_Writeable(IBTSPPServer* po, AEECallback* pcb);
  
Parameters:
   po [i]     : the object
   pcb [i]    : pre-initialized AEECallback pointing to function to be called
                when transmit queue is no longer full

Return Value:
	None

See Also:
   IBTSPPSRV_Write()
   IBTSPPSRV_OnStatusChange()

==============================================================================
IBTSPPSRV_IOCtl()

Description:
   Inherited from IPORT_IOCtl().
   This function is not supported.  IOCtlEx() should be used.
   
Return values: ECLASSNOTSUPPORT
   
==============================================================================
IBTSPPSRV_Open()

Description:
   Inherited from IPORT_Open().
   This function is not supported.  Instead, IBTSPPSRV_EnableService() 
   should be used by server application
   
Return values: ECLASSNOTSUPPORT

==============================================================================
IBTSPPSRV_Close()

Description:
   Inherited from IPORT_Close().
   This function is not supported.  DisableService() should be used.
   
Return values: ECLASSNOTSUPPORT   

==============================================================================
IBTSPPSRV_EnableService()
   
Description:
   This method opens up an SPP stream that listens for a connection request 
   from an SPP client and advertises SPP service to inquiring devices.

   User of this API can specify the level of security to be enforeced on the 
   connection, and whether the application should be asked to authorize
   the connection.

   If security level is AUTHEN or AUTHEN & ENCRYPT, and if a potential SPP
   client has not been authenticated before, the AEEBT_EV_PIN_REQ notification
   will be broadcast to all applications that have registered for it.
   The interested application should call IBT_GetNotificationData() to retrieve
   the additional data to identify the remote device to authenticate.
   IBTDEVDB_Bond() should be used to send the PIN key to BT driver.

   Before calling IBTSPPSRV_EnableService(),the application should 
   call IBTSPPSRV_OnConnRequest() to register a callback with BREW, to be 
   invoked when a connection request comes in.
   
   The application should use IBTSPPSRV_Authorize() to authorize or reject the 
   connection request.  If IBTSPPSRV_OnConnRequest() is not called,
   connection requests will be accepted automatically.
   
   The result callback registered via IBTSPPSRV_EnableService is called once an
   SPP stream is opened and ready for connection requests. If a uuid other than
   AEEBT_SCU_INVALID was passed to IBT_GetSPPServer(), the server would be registered
   with SDP.

   The associated SPP stream will stay opened until IBTSPPSRV_DisableService()
   is called.

Prototype:
   int IBTSPPSRV_EnableService(IBTSPPServer*    po,
                               BTSecurityLevel  secLevel,
                               uint32           uServerChannel,
                               boolean          bUnRegSdp,
                               BTResult*        pResult,
                               AEECallback*     pResultCb);

Parameters:
   po [i]            : the object
   secLevel [i]      : Security level for Mode 2 security
                       if NONE, no security will be enforced
                       if AUTHEN only, PIN request notification will be generated
                       if AUTHEN & ENCRYPT, same as AUTH only, but data sent
                       over the connection will also be encrypted
   uServerChannel[i] : server channel to register the server on.If zero
                       the next available server channel will be assigned.
   bUnRegSdp[i]      : TRUE indicates if the service should be unregistered
                       with SDP when a connection is set up. 
   pResult [o]       : result
   pResultCb [i]     : called to deliver result of enabling the service

Return values:
   SUCCESS   : pending, just wait for pResultCb to be called                    
   EBADPARM  : one or more input pointers are invalid
   EBADSTATE : already enabled

See Also:
   IBTSPPSRV_DisableService()
   IBTSPPSRV_Authorize()
   BTSecurityLevel
   BTResult

==============================================================================
IBTSPPSRV_OnConnRequest()
   
Description:
   This method allows the server application to be notified when a client
   tries to connect to it. It should be called before IBTSPPSRV_EnableService()
   is called and subsequently when the pAuthCb callback is invoked.

Prototype:
   int IBTSPPSRV_OnConnRequest( IBTSPPServer*     po,
                                AEECallback*      pAuthCb,
                                BTEventData*      pEvData,);

Parameters:
   po [i]         : the object
   pAuthCb [i]    : called to deliver connection request
   pEvData [o]    : authorization data
 
Return values:
   SUCCESS   : callback successfully registered
   EBADPARM  : one or more input pointers are invalid
   

See Also:
   IBTSPPSRV_EnableService()
   IBTSPPSRV_Authorize()
   BTEventData

   
==============================================================================
IBTSPPSRV_DisableService()
   
Description:
   This method closes the SPP stream associated with this SPP server.  It also
   cancels the Authorization callback.
   
Prototype:
   int IBTSPPSRV_DisableService(IBTSPPServer*     po,
                                BTResult*         pResult,
                                AEECallback*      pResultCb);

Parameters:
   po [i]         : the object
   pResult [o]    : result
   pResultCb [i]  : called to deliver result of disabling the service

Return values:
   SUCCESS   : pending, just wait for pResultCb to be called                    
   EBADPARM  : one or more input pointers are invalid
   EBADSTATE : server not enabled

See Also:
   IBTSPPSRV_EnableService()
   BTResult

==============================================================================
IBTSPPSRV_Authorize()

Description:
   This method enables application implementing the SPP server role to either
   accept or reject the connection request from the client whose BD address 
   is given.
   
Prototype:
   int IBTSPPSRV_Authorize(IBTSPPServer*     po,
                           const BDAddress*  pBDAddr,
                           boolean           bAccept,
                           BTResult*         pResult,
                           AEECallback*      pResultCb);

Parameters:
   po [i]         : the object
   pBDAddr [i]    : BD address of remote device
   bAccept [i]    : TRUE to accept the connection, FALSE to reject
   pResult [o]    : result
   pResultCb      : called to deliver authorization result

Return values:
   SUCCESS   : pending, just wait for pResultCb to be called                    
   EBADPARM  : one or more input pointers are invalid
   EBADSTATE : no SPP stream

See Also:
   BDAddress   
==============================================================================
IBTSPPSRV_Disconnect()

Description:
   This API terminates the SPP connection. For SPP server, 
   the SPP stream goes back to listening.
   
Prototype:
   int IBTSPPSRV_Disconnect(IBTSPPServer* po,
                            BTResult*     pResult,
                            AEECallback*  pResultCb);
 
Parameters:
   po [i]         : the object
   pResult[o]     : result of Disconnect operation
   pResultCb      : called to deliver Disconnect result

Return values:
   SUCCESS   : pending, just wait for pResultCb to be called                    
   EBADPARM  : one or more input pointers are invalid
   EBADSTATE : no SPP stream or stream not connected.

See Also:
   BTResult
==============================================================================
IBTSPPSRV_OnStatusChange()

Description:
   This API registers a callback to be invoked when there's a change in the
   SPP stream state associated with the SPP object.  It should be called as soon as
   the object is created, and again after the registered callback is invoked.
   Once the callback is invoked, the application should retrieve the current
   status via IBTSPPSRV_GetStatus()
   
Prototype:
   int IBTSPPSRV_OnStatusChange(IBTSPPServer*     po, 
                                AEECallback*      pStatusCb);
  
Parameters:
   po [i]        : the object
   pStatusCb     : called to inform the application,
                   SPP state has changed

Return values:
   SUCCESS   : pStatusCb has been registered
   EBADPARM  : one or more input pointers are invalid

==============================================================================
IBTSPPSRV_OnModemStatusChange()

Description:
   This API registers a callback to be invoked when there's a change in the
   modem status associated with the SPP object. It should be called as soon as
   the object is created, and again after the registered callback is invoked.
   Once the callback is invoked, the application should retrieve the current
   modem status via IBTSPPSRV_GetModemStatus()
   
Prototype:
   int IBTSPPSRV_OnModemStatusChange(IBTSPPServer*  po, 
                                     AEECallback*   pStatusCb);
 
Parameters:
   po [i]               : the object
   pStatusCb            : called to inform application,modem status has changed

Return values:
   SUCCESS   : pStatusCb has been registered
   EBADPARM  : one or more input pointers are invalid
   

==============================================================================
IBTSPPSRV_OnLineError()

Description:
   This API registers a callback to be invoked when a line error occurs on the
   SPP stream associated with the SPP object.  It should be called as soon as
   the object is created, and again after the registered callback is invoked.
   Once the callback is invoked, the application should retrieve the current
   line error via IBTSPPSRV_GetLineError()
   
   
Prototype:
   int IBTSPPSRV_OnLineError(IBTSPPServer*  po, 
                             AEECallback*   pLineErrorCb);
 
Parameters:
   po [i]               : the object
   pLineErrorCb         : called to inform application there are line errors

Return values:
   SUCCESS   : pLineErrorCb has been registered
   EBADPARM  : one or more input pointers are invalid
   

==============================================================================
IBTSPPSRV_GetStatus()

Description:
   This API retrieves the stream's current status.  The streamID field of 
   the status indicates the stream whose status is being retrieved.
   
Prototype:
   int IBTSPPSRV_GetStatus(IBTSPPServer* po, BTSPPStatus* pStatus);
   
Parameters:
   po [i]         : the object
   pStatus [o]    : status of the stream

Return values:
   SUCCESS   : pStatus contains current state of the stream                    
   EBADPARM  : one or more input pointers are invalid
   
See Also:
   BTSPPStatus
==============================================================================
IBTSPPSRV_GetModemStatus()

Description:
   This API retrieves the stream's current modem status .  The streamID field
   of the status indicates the stream whose modem status is being retrieved.
   
Prototype:
   int IBTSPPSRV_GetModemStatus(IBTSPPServer* po, BTSPPModemStatus* pStatus);
  
Parameters:
   po [i]         : the object
   pStatus [o]    : modem status of the stream

Return values:
   SUCCESS   : pStatus contains current modem status of the stream                  
   EBADPARM  : one or more input pointers are invalid
   
See Also:
   BTSPPModemStatus
==============================================================================
IBTSPPSRV_GetLineError()

Description:
   This API retrieves the stream's current line error .  The streamID field
   of the status indicates the stream whose line error is being retrieved.
   
Prototype:
   int IBTSPPSRV_GetLineError(IBTSPPServer* po, BTSPPLineError* pLineError);

Parameters:
   po [i]            : the object
   pLineError [o]    : line error that occured on the stream

Return values:
   SUCCESS   : pLineError contains line error that occured on the stream                     
   EBADPARM  : one or more input pointers are invalid
   
See Also:
   BTSPPLineError
      
==============================================================================
IBTSPPSRV_Configure()

Description:
   This API configures the stream.  Note that the configuration does not 
   actually affect the connection, only the configuration settings are sent 
   to the remote device.
   
Prototype:
   int IBTSPPSRV_Configure(IBTSPPServer* po, BTSPPConfig* pConfig,
                           BTResult* pR, AEECallback* pRCb);  

Parameters:
   po [i]         : the object
   pConfig [i/o]  : configuration info according to RS232 specs
   pR[o]          : Result of the configuration change  
   pRCb           : called to deliver the configuration
                    change result. 
Return values:
   SUCCESS   : pending, just wait for pRCb to be called 
   EBADPARM  : one or more input pointers are invalid
   EBADSTATE : no SPP stream

See Also:
   BTSPPConfig
   BTResult   
==============================================================================
IBTSPPSRV_IOCtlEx()

Description:
   This API can be used to control or configure the stream.
   AEEBTSPP_IOCTL_SET_DTE and AEEBTSPP_IOCTL_SET_DCE can be sent
   when a stream is not present.
   
Prototype:
   int IBTSPPSRV_IOCtlEx(IBTSPPServer*       po,
                         BTSPPIOCtlCommand   IOctlCmd,
                         BTSPPModemStatus*   pModemStatus,
                         AEECallback*        pResultCb);
 
Parameters:
   po [i]           : the object
   IOctlCmd [i]     : the IO Control command to perform
   pModemStatus [o] : modem status holder
   pResultCb        : called to deliver the result

Return values:
   SUCCESS   : pending, just wait for pResultCb to be called 
   EBADPARM  : one or more input pointers are invalid
   EBADSTATE : no SPP stream

See Also:
   BTSPPIOCtlCommand
   BTSPPModemStatus   
============================================================================*/

#endif //AEEBTSPPSERVER_H

