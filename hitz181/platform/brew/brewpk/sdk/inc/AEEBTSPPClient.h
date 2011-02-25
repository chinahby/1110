#ifndef AEEBTSPPCLIENT_H
#define AEEBTSPPCLIENT_H
/*=============================================================================
FILE:         AEEBTSPPClient.h

SERVICES:     IBTSPP client Interfaces

DESCRIPTION:  IBTSPPClient is the BREW interface that supports Bluetooth Serial
              Port profile in client role
===============================================================================
Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
#include "AEEBTSPP.h"

/*=============================================================================
 Interface ID
=============================================================================*/
#define AEEIID_BT_SPP_CLIENT       0x01041695

/*============================================================================
 SPP Client Interface Definition
============================================================================*/
typedef struct IBTSPPClient IBTSPPClient;

AEEINTERFACE(IBTSPPClient)
{
   INHERIT_IBTSPP(IBTSPPClient);

   int (*FindServersInit) (IBTSPPClient*      po,
                           uint32             dwSearchDuration,
                           uint32             dwMaxServers);
   int (*FindServersNext) (IBTSPPClient*      po,
                           uint32*            pSize,
                           AEECallback*       pCb);
   int (*GetServerInfo)   (IBTSPPClient*      po,
                           BTSPPDeviceInfo*   pDevice);
   int (*Connect)         (IBTSPPClient*      po,
                           const BDAddress*   pBDAddr,
                           uint32             uServerChannel,
                           BTResult*          pResult,
                           AEECallback*       pResultCb);
   int (*CancelServerSearch)(IBTSPPClient*      po,
                             AEECallback*       pResultCb );
};

// Standard IPort Methods
static __inline uint32 IBTSPPCLI_AddRef (IBTSPPClient* p)
{
   return AEEGETPVTBL((p),IBTSPPClient)->AddRef((p));
}

static __inline uint32 IBTSPPCLI_Release (IBTSPPClient* p)
{
   return AEEGETPVTBL((p),IBTSPPClient)->Release((p));
}

static __inline int IBTSPPCLI_QueryInterface (IBTSPPClient* p,
                                              AEECLSID      clsid,
                                              void**        ppo )
{
   return AEEGETPVTBL((p),IBTSPPClient)->QueryInterface((p),(clsid),(ppo));
}

static __inline int32 IBTSPPCLI_Read(IBTSPPClient* p, 
                                     char*         pcBuf,
                                     int32         cbBuf )
{
   return AEEGETPVTBL((p),IBTSPPClient)->Read((p),(pcBuf),(cbBuf));
}

static __inline void IBTSPPCLI_Readable(IBTSPPClient* p,
                                        AEECallback* pcb )
{
   AEEGETPVTBL((p),IBTSPPClient)->Readable((p),(pcb));
}

static __inline int IBTSPPCLI_GetLastError(IBTSPPClient* p)
{
   return AEEGETPVTBL((p),IBTSPPClient)->GetLastError((p));
}

static __inline int32 IBTSPPCLI_Write(IBTSPPClient* p, 
                                      const char* pcBuf,
                                      int32 cbBuf )
{
   return AEEGETPVTBL((p),IBTSPPClient)->Write((p),(pcBuf),(cbBuf));
}

static __inline void IBTSPPCLI_Writeable(IBTSPPClient* p,
                                        AEECallback* pcb )
{
   AEEGETPVTBL((p),IBTSPPClient)->Writeable((p),(pcb));
}

static __inline int IBTSPPCLI_IOCtl (IBTSPPClient* p,
                                     int option,
                                     int val )
{
   return AEEGETPVTBL((p),IBTSPPClient)->IOCtl((p),(option),(val));
}

static __inline int IBTSPPCLI_Close(IBTSPPClient* p)
{
   return AEEGETPVTBL((p),IBTSPPClient)->Close((p));
}

static __inline int IBTSPPCLI_Open (IBTSPPClient* p,
                                    const char* szPort)
{
   return AEEGETPVTBL((p),IBTSPPClient)->Open((p),(szPort));
}

// Standard SPP Methods
static __inline int IBTSPPCLI_OnStatusChange (IBTSPPClient* p, 
                                              AEECallback* pRCb)
{
   return AEEGETPVTBL((p),IBTSPPClient)->OnStatusChange((p),(pRCb));
}

static __inline int IBTSPPCLI_OnModemStatusChange (IBTSPPClient* p, 
                                                   AEECallback* pRCb)
{
   return AEEGETPVTBL((p),IBTSPPClient)->OnModemStatusChange((p),(pRCb));
}

static __inline int IBTSPPCLI_OnLineError (IBTSPPClient* p, 
                                           AEECallback* pRCb)
{
   return AEEGETPVTBL((p),IBTSPPClient)->OnLineError((p),(pRCb));
}

static __inline int IBTSPPCLI_Disconnect (IBTSPPClient* p,
                                          BTResult* pR, 
                                          AEECallback* pRCb)
{

   return AEEGETPVTBL((p),IBTSPPClient)->Disconnect((p),(pR),(pRCb));
}

static __inline int IBTSPPCLI_GetStatus (IBTSPPClient* p,
                                         BTSPPStatus* pS)

{

   return AEEGETPVTBL((p),IBTSPPClient)->GetStatus((p),(pS));
}

static __inline int IBTSPPCLI_GetModemStatus (IBTSPPClient* p,
                                              BTSPPModemStatus* pS)

{

   return AEEGETPVTBL((p),IBTSPPClient)->GetModemStatus((p),(pS));
}

static __inline int IBTSPPCLI_GetLineError (IBTSPPClient* p,
                                            BTSPPLineError* pLineError)

{

   return AEEGETPVTBL((p),IBTSPPClient)->GetLineError((p),(pLineError));
}

static __inline int IBTSPPCLI_Configure ( IBTSPPClient* p, 
                                          const BTSPPConfig* pCfg,
                                          BTResult* pR,
                                          AEECallback* pRCb)
{

   return AEEGETPVTBL((p),IBTSPPClient)->Configure((p),(pCfg),(pR),(pRCb));
}

static __inline int IBTSPPCLI_IOCtlEx  ( IBTSPPClient* p, 
                                         BTSPPIOCtlCommand IOctlCmd,
                                         BTSPPModemStatus* pModemStatus, 
                                         AEECallback* pResultCb)
{


   return AEEGETPVTBL((p),IBTSPPClient)->IOCtlEx((p),(IOctlCmd),(pModemStatus),
                                                 (pResultCb));
}

// SPP Client Methods
static __inline int IBTSPPCLI_FindServersInit( IBTSPPClient* p,
                                               int  searchDuration,
                                               int   maxServers)
{


   return AEEGETPVTBL((p),IBTSPPClient)->FindServersInit((p),(searchDuration),
                                                         (maxServers));

}

static __inline int IBTSPPCLI_FindServersNext( IBTSPPClient* p,
                                               uint32* pSize,
                                               AEECallback*  pCb)
{

   return AEEGETPVTBL((p),IBTSPPClient)->FindServersNext((p),(pSize),(pCb));

}
static __inline int IBTSPPCLI_GetServerInfo ( IBTSPPClient*    p, 
                                              BTSPPDeviceInfo* pServerInfo)
{


   return AEEGETPVTBL((p),IBTSPPClient)->GetServerInfo((p),(pServerInfo));

}

static __inline int IBTSPPCLI_Connect ( IBTSPPClient*      p,
                                        const BDAddress*   pBDAddr,
                                        int                serverChannel,
                                        BTResult*          pResult,
                                        AEECallback*       pResultCb)
{

   return AEEGETPVTBL((p),IBTSPPClient)->Connect((p),(pBDAddr),(serverChannel)
                                                 ,(pResult),(pResultCb));

}

static __inline int IBTSPPCLI_CancelServerSearch ( IBTSPPClient* p,
                                                   AEECallback*  pCb)
{

   return AEEGETPVTBL((p),IBTSPPClient)->CancelServerSearch((p),(pCb));

}

/*============================================================================
INTERFACE DOCUMENTATION
==============================================================================
IBTSPPClient Interface

Description:
   This interface enables BREW applications to use Bluetooth Serial Port 
   Profile (SPP) to exchange data with a remote device.  Each instance of 
   IBTSPPServer supports one connection in a server role.  A BREW application
   can instantiate multiple server objects each serving an SPP client.  
   A BREW application can instantiate one IBTSPPServer object and one 
   IBTSPPClient object. Both objects can be in a connection provided that
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
   is invoked,the application should retreive the received data by calling
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
IBTSPPCLI_AddRef()

Description:
   This function is inherited from IBASE_AddRef().

See Also:
   IBTSPPCLI_Release()
   
==============================================================================
IBTSPPCLI_Release()

Description:
   This function is inherited from IBASE_Release().

See Also:
   IBTSPPCLI_AddRef()
   
==============================================================================
IBTSPPCLI_QueryInterface()

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IBTSPP-derived class

Prototype:
   int IBTSPPCLI_QueryInterface (IBTSPPClient* po, AEECLSID clsReq, 
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
IBTSPPCLI_Read()

Description:
   Inherited from IPORT_Read().
   Retrieve the received data from remote device. If pDest == NULL,
   the function tries to flush maxBytes2Read bytes from the receive buffer.
   
Prototype:
   int32 IBTSPPCLI_Read(IBTSPPClient* po, char* pDest, int32 maxBytes2Read);

Parameters:
   po [i]             : the object
   pDest [o]          : destination for received data
   maxBytes2Read [i]  : number of bytes destination can hold

Return values:
   number of bytes received/ flushed, a value greater than 0
   AEEPORT_WAIT       : no data available
   AEEPORT_ERROR      : invalid parameters or no SPP stream

See Also:
   IBTSPPCLI_Readable()

==============================================================================
IBTSPPCLI_Readable()

Description:
   Inherited from IPORT_Readable().
   Registers a function to be invoked when receive queue has data.
      
Prototype:
   void IBTSPPCLI_Readable(IBTSPPClient* po, AEECallback* pcb);

Parameters:
   po [i]     : the object
   pcb [i]    : pre-initialized AEECallback pointing to function to be called
                when receive queue is no longer empty

Return Value:
	None

See Also:
   IBTSPPCLI_Read()

==============================================================================
IBTSPPCLI_GetLastError()

Description:
   Inherited from IPORT_GetLastError().
   This function returns the last error that occurred with Read() or Write().

Prototype:
   int IBTSPPCLI_GetLastError(IBTSPPClient* po);

Parameters:
   po [i]     : the object
   
Return Value:
   The most recently occurred error.  This could be any BREW error code.
   The most common ones are listed below.

   AEEPORT_ENOTOPEN  : SPP port is not open
   EBADSTATE         : SPP port is in an incompatible state

See Also:
   IBTSPPCLI_Read()
   IBTSPPCLI_Write()

==============================================================================
IBTSPPCLI_Write()

Description:
   Inherited from IPORT_Write().
   This function sends data to remote device.
   
Prototype:
   int32 IBTSPPCLI_Write(IBTSPPClient* po, const char* pSrc, 
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
   IBTSPPCLI_Writeable()
   
==============================================================================
 IBTSPPCLI_Writeable()

Description:
   Inherited from IPORT_Writeable().
   Registers a callback to be invoked when transmit queue can accept more 
   data to be sent out.  
   
Prototype:
   void IBTSPPCLI_Writeable(IBTSPPClient* po, AEECallback* pcb);

Parameters:
   po [i]     : the object
   pcb [i]    : pre-initialized AEECallback pointing to function to be called
                when transmit queue is no longer full

Return Value:
	None

See Also:
   IBTSPPCLI_Write()
   IBTSPPCLI_OnStatusChange()

==============================================================================
IBTSPPCLI_IOCtl()

Description:
   Inherited from IPORT_IOCtl().
   This function is not supported.  IOCtlEx() should be used.
   
Return values: ECLASSNOTSUPPORT
   
==============================================================================
IBTSPPCLI_Open()

Description:
   Inherited from IPORT_Open().
   This function is not supported instead IBTSPPCLI_Connect() should be
   used.
   
Return values: ECLASSNOTSUPPORT

==============================================================================
IBTSPPCLI_Close()

Description:
   Inherited from IPORT_Close().
   This function is not supported.  Disconnect() should be used.
   
Return values: ECLASSNOTSUPPORT   

==============================================================================
IBTSPPCLI_FindServersInit()

Description:
   This function enables applications implementing the SPP client role to search
   for all SPP servers in the vicinity.  The search will stop when either
   searchDuration seconds have passed, or maxServers have been discovered,
   or it has discovered all SPP servers in the vicinity,whichever comes first.
   If searchDuration and maxServers are 0, the search will stop when it
   has discovered all SPP servers in the vicinity.
   After this function returns SUCCESS, the application should
   call IBTSPPCLI_FindServerNext() to register a callback to be invoked when
   a server is found.

Prototype:
   int IBTSPPCLI_FindServersInit(IBTSPPClient* po,
                                 uint32        searchDuration,
                                 uint32        maxServers);

Parameters:
   po [i]             : the SPP client object
   searchDuration [i] : number of seconds to search
   maxServers [i]     : max number of servers to search for
                    
Return values:
   SUCCESS  : ready to search
   EBADPARM : one or more input pointers are invalid
   EALREADY : Cannot initiate a server search at the moment as one is 
               already in progress.

See Also:
   IBTSPPCLI_FindServersNext()

==============================================================================
IBTSPPCLI_FindServersNext()

Description:
   After IBTSPPCLI_FindServersInit() returns SUCCESS, this function should
   be called to register a callback to be invoked when a server is found.
   The callback will be invoked when the next server is found and the
   application should allocate at least pSize bytes of memory, then call 
   IBTSPPCLI_GetServerInfo() to retrieve the server information into
   the allocated memory.

   This function should be called until the size returned is 0.
   
Prototype:
   int IBTSPPCLI_FindServersNext(IBTSPPClient*      po, 
                                 uint32*            pSize,
                                 AEECallback*       pCb);

Parameters:
   po [i]         : the SPP client object
   pSize [o]      : size of the memory needed for the server information
   pCb            : called to deliver memory size required for the next server
                    
Return values:
   SUCCESS  : pending, just wait for pCb to be called
   EBADPARM : one or more input pointers are invalid

See Also:
   IBTSPPCLI_GetServerInfo()
   
==============================================================================
IBTSPPCLI_GetServerInfo()

Description:
   After the callback registered via IBTSPPCLI_FindServersNext() is invoked,
   the application should allocate enough memory to save the server information.
   This function should be called to retrieve the information on the next 
   server found.
   
Prototype:
   int IBTSPPCLI_GetServerInfo(IBTSPPClient*      po, 
                               BTSPPDeviceInfo*   pServerInfo);

Parameters:
   po [i]            : the SPP client object
   pServerInfo [o]   : destination for server info
                    
Return values:
   SUCCESS  : info for the next server found has been copied to pServerInfo
   EBADPARM : one or more input pointers are invalid 
   EBADITEM : no servers present.

See Also:
   IBTSPPCLI_FindServersNext()
   BTSPPDeviceInfo
  
==============================================================================
IBTSPPCLI_CancelServerSearch()

Description:
   This API cancels a previously initiated server search.
   If a callback has been previously registered using IBTSPPCLI_FindServersNext(),
   the callback will be called with Size = 0.

Prototype:

   int IBTSPPCLI_CancelServerSearch(IBTSPPClient*  po, AEECallback*  pCb);
   
Parameters:
   po [i]           : the object
   pCb              : called to indicate the search has been cancelled
   
Return values:
   SUCCESS   : cancellation initiated,wait for pCb to be called
   EBADPARM  : one or more input pointers are invalid

See Also:
   IBTSPPCLI_FindServersInit()
   IBTSPPCLI_FindServersNext()


==============================================================================
IBTSPPCLI_Connect()

Description:
   This method enables application implementing the SPP client role to connect 
   to the SPP server whose BD address is given.
   
Prototype:
   int IBTSPPCLI_Connect(IBTSPPClient*      po,
                         const BDAddress*   pBDAddr,
                         uint32             uSCN,
                         BTResult*          pResult,
                         AEECallback*       pResultCb);

Parameters:
   po [i]         : the SPP client object
   pBDAddr [i]    : BD address of the server of interest
   uSCN [i]       : server channel number to use; if 0, the underlying BT
                    stack would take care to retrieve SCN from server.If there
                    are multiple servers on the remote peer, it will connect to
                    the first available server.
   pResult [o]    : result of the connect attempt
   pResultCb      : called to deliver connect result

Return values:
   SUCCESS  : pending, just wait for pResult to be called                    
   EBADPARM : one or more input pointers are invalid
   EBADSTATE : Already connected. Bad state.
   
See Also:
   BDAddress
   BTResult
==============================================================================
IBTSPPCLI_Disconnect()

Description:
   This API terminates the SPP connection. For SPP client, the SPP stream 
   will be closed.
   
Prototype:
  int IBTSPPCLI_Disconnect(IBTSPPClient* po,
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
IBTSPPCLI_OnStatusChange()

Description:
   This API registers a callback to be invoked when there's a change in the
   SPP stream state associated with the SPP object.  It should be called as soon as
   the object is created, and again after the registered callback is invoked.
   Once the callback is invoked, the application should retrieve the current
   status via IBTSPPCLI_GetStatus()
   
Prototype:
   int IBTSPPCLI_OnStatusChange(IBTSPPClient*     po, 
                                AEECallback*      pStatusCb);

Parameters:
   po [i]        : the object
   pStatusCb     : called to inform the application,
                   SPP state has changed

Return values:
   SUCCESS   : pStatusCb has been registered
   EBADPARM  : one or more input pointers are invalid

==============================================================================
IBTSPPCLI_OnModemStatusChange()

Description:
   This API registers a callback to be invoked when there's a change in the
   modem status associated with the SPP object. It should be called as soon as
   the object is created, and again after the registered callback is invoked.
   Once the callback is invoked, the application should retrieve the current
   modem status via IBTSPPCLI_GetModemStatus()
   
Prototype:
  int IBTSPPCLI_OnModemStatusChange(IBTSPPClient*  po, 
                                     AEECallback*   pStatusCb);

Parameters:
   po [i]               : the object
   pStatusCb            : called to inform application,modem status has changed

Return values:
   SUCCESS   : pStatusCb has been registered
   EBADPARM  : one or more input pointers are invalid
   

==============================================================================
IBTSPPCLI_OnLineError()

Description:
   This API registers a callback to be invoked when a line error occurs on the
   SPP stream associated with the SPP object.  It should be called as soon as
   the object is created, and again after the registered callback is invoked.
   Once the callback is invoked, the application should retrieve the current
   line error via IBTSPPSRV_GetLineError()
   
   
Prototype:
  int IBTSPPCLI_OnLineError(IBTSPPClient*  po, 
                             AEECallback*   pLineErrorCb);

Parameters:
   po [i]               : the object
   pLineErrorCb         : called to inform application there are line errors

Return values:
   SUCCESS   : pLineErrorCb has been registered
   EBADPARM  : one or more input pointers are invalid
   

==============================================================================
IBTSPPCLI_GetStatus()

Description:
   This API retrieves the stream's current status.  The streamID field of 
   the status indicates the stream whose status is being retrieved.
   
Prototype:
   int IBTSPPCLI_GetStatus(IBTSPPClient* po, BTSPPStatus* pStatus);

Parameters:
   po [i]         : the object
   pStatus [o]    : status of the stream

Return values:
   SUCCESS   : pStatus contains current state of the stream                    
   EBADPARM  : one or more input pointers are invalid

See Also:
  BTSPPStatus   

==============================================================================
IBTSPPCLI_GetModemStatus()

Description:
   This API retrieves the stream's current modem status .  The streamID field
   of the status indicates the stream whose modem status is being retrieved.
   
Prototype:
   int IBTSPPCLI_GetModemStatus(IBTSPPClient* po, BTSPPModemStatus* pStatus);

Parameters:
   po [i]         : the object
   pStatus [o]    : modem status of the stream

Return values:
   SUCCESS   : pStatus contains current modem status of the stream                  
   EBADPARM  : one or more input pointers are invalid
   
See Also:
  BTSPPModemStatus
  
==============================================================================
IBTSPPCLI_GetLineError()

Description:
   This API retrieves the stream's current line error .  The streamID field
   of the status indicates the stream whose line error is being retrieved.
   
Prototype:
   int IBTSPPCLI_GetLineError(IBTSPPClient* po, BTSPPLineError* pLineError);

Parameters:
   po [i]            : the object
   pLineError [o]    : line error that occured on the stream

Return values:
   SUCCESS   : pLineError contains line error that occured on the stream                     
   EBADPARM  : one or more input pointers are invalid
   
See Also:
  BTSPPLineError
  
==============================================================================
IBTSPPCLI_Configure()

Description:
   This API configures the stream.  Note that the configuration does not 
   actually affect the connection, only the configuration settings are sent 
   to the remote device.
   
Prototype:
   int IBTSPPCLI_Configure(IBTSPPClient* po, BTSPPConfig* pConfig,
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
IBTSPPCLI_IOCtlEx()

Description:
   This API can be used to control or configure the stream.
   AEEBTSPP_IOCTL_SET_DTE and AEEBTSPP_IOCTL_SET_DCE can be sent
   when a stream is not present.
   
Prototype:
   int IBTSPPCLI_IOCtlEx(IBTSPPClient*       po,
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

#endif //AEEBTSPPCLIENT_H

