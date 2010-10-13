#ifndef AEEIMCASTSESSION_H
#define AEEIMCASTSESSION_H

/*===================================================

FILE: AEEIMcastSession.h

SERVICES:  BREW Multicast Session Interface

DESCRIPTION:
   Interface definitions, data structures, etc. 
   for Multicast Session Interface

======================================================

   Copyright (c) 2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEEIModel.h"
#include "AEECallback.h"
#include "AEEMcastTypes.h"

//-----------------------------------------------------------------
// Types
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Interface
//-----------------------------------------------------------------

// Interface ID
#define AEEIID_IMcastSession 0x01031dfc

#if defined(AEEINTERFACE_CPLUSPLUS)

struct IMcastSession : public IModel 
{
   virtual int CDECL Join (void* pGroupAddr) = 0;
   virtual int CDECL Leave (void) = 0;
   virtual int CDECL GetGroupAddr (void* pGroupAddr, int* pnSize) = 0;
   virtual int CDECL SelectNetworkEx (int nNetwork, int16 nFamily) = 0;
   virtual int CDECL AddRetryCB (AEECallback* pcbRetry) = 0;
   virtual int CDECL ExtFunc (AEECLSID id, void *pBuf, int nBufSize) = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_IMcastSession(iname) \
   INHERIT_IModel(iname); \
   int        (*Join)(iname* po, void* pGroupAddr); \
   int        (*Leave)(iname* po); \
   int        (*GetGroupAddr)(iname* po, void* pGroupAddr, int* pnSize);  \
   int        (*SelectNetworkEx)(iname* po, int nNetwork, int16 nFamily); \
   int        (*AddRetryCB)(iname* po, AEECallback* pcbRetry);            \
   int        (*ExtFunc)(iname* po, AEECLSID id, void *pBuf, int nBufSize)\
              // Extension Function to allow future enhancement to the interface without 
              // breaking its binary compatibility.

// declare the actual IMcastSession interface
AEEINTERFACE_DEFINE(IMcastSession);


//-----------------------------------------------------------------
// Methods
//-----------------------------------------------------------------

static __inline uint32 IMcastSession_AddRef(IMcastSession *me)
{
   return AEEGETPVTBL(me, IMcastSession)->AddRef(me);
}

static __inline uint32 IMcastSession_Release(IMcastSession *me)
{
   return AEEGETPVTBL(me, IMcastSession)->Release(me);
}

static __inline int IMcastSession_QueryInterface(IMcastSession *me, 
                                            AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me, IMcastSession)->QueryInterface(me, cls, ppo);
}

static __inline int IMcastSession_SelectNetworkEx(IMcastSession *me, int nNetwork, int16 nFamily)
{
   return AEEGETPVTBL(me, IMcastSession)->SelectNetworkEx(me, nNetwork, nFamily);
}

static __inline int IMcastSession_AddRetryCB(IMcastSession* me, AEECallback* pcbRetry)
{
   return AEEGETPVTBL(me, IMcastSession)->AddRetryCB(me, pcbRetry);
}

static __inline int IMcastSession_Join(IMcastSession* me, void* pGroupAddr)
{
   return AEEGETPVTBL(me, IMcastSession)->Join(me, pGroupAddr);
}

static __inline int IMcastSession_Leave(IMcastSession* me)
{
   return AEEGETPVTBL(me, IMcastSession)->Leave(me);
}

static __inline int IMcastSession_AddListener(IMcastSession* me, ModelListener *pListener)
{
   return AEEGETPVTBL(me, IMcastSession)->AddListener(me, pListener);
}

static __inline int IMcastSession_GetGroupAddr(IMcastSession* me, void* pGroupAddr, int* pnSize)
{
   return AEEGETPVTBL(me, IMcastSession)->GetGroupAddr(me, pGroupAddr, pnSize);
}

static __inline int IMcastSession_ExtFunc(IMcastSession* me, AEECLSID id, void *pBuf, int nBufSize)
{
   return AEEGETPVTBL(me, IMcastSession)->ExtFunc(me, id, pBuf, nBufSize);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IMcastSession

This interface represents a Multicast Session. Applications may use 
IMcastSession to receive data which is sent to a multicast group.

This interface deprecates the use of ISockPort socket options 
AEE_IP_ADD_MEMBERSHIP and AEE_IP_DROP_MEMBERSHIP.

NOTE: Your application must have a privilege level of Network or All to be able
to invoke the functions in this interface.

In order to use a Multicast Session, a client should:
- Create a new IMcastSession object.
- Select the desired data network by calling IMcastSession_SelectNetworkEx().  
- Register a listener callback for receiving multicast events by using 
  IMcastSession_AddListener().
- Call IMcastSession_Join() with the desired multicast group address and port.
  IMcastSession_Join() performs an asynchronous operation, and hence a 
  return value of AEE_SUCCESS only means that the Join request has been 
  submitted. In case IMcastSession_Join() returns AEE_NET_WOULDBLOCK, 
  use IMcastSession_AddRetryCB() to register a callback which will be 
  resumed when the IMcastSession_Join() operation should be retried.
- Wait for a multicast event which indicates the status of the request. 
  The multicast event will be delivered through the registered listener 
  callback function. Receipt of a successful multicast event 
  (i.e. AEE_MCAST_REGISTER_SUCCESS_EV) means that the client has joined 
  the multicast group. 
- Now the application can start receiving data sent to the multicast group, 
  using sockets. Create a UDP socket with ISockPort; Select the proper data 
  network; and bind the socket to the multicast group address and port. 
  Now, calling ISockPort_RecvFrom() may read data sent to the group.
- When the Multicast Session is not needed anymore, call IMcastSession_Leave().
  IMcastSession_Leave() performs an asynchronous operation, and hence a 
  return value of AEE_SUCCESS only means that the request has been submitted.
- Wait for a multicast event which indicates that the session was closed. The 
  event will be delivered through the registered listener callback. 
  Receipt of the AEE_MCAST_DEREGISTERED_EV event means that the multicast
  session is closed and its resources are released.

The following header file is required:~
   BREW 4.0 - AEEIMcastSession.h
   BREW 3.1 - AEEMcastSession.h

=============================================================================

IMcastSession_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IMcastSession_Release()

=============================================================================

IMcastSession_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   IMcastSession_AddRef()

==============================================================================

IMcastSession_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

==============================================================================

IMcastSession_AddListener()

Description:
   This function is inherited from IModel_AddListener().

   The ModelEvent object received at the listener callback contains:
   - pModel: pointer to the originating object. Use IQI to query it for 
             IMcastSession interface
   - evCode: the multicast event that occurred (AEEMcastEvent).
   - dwParam: additional information regarding the cause of the event 
              (AEEMcastInfoCode)

See Also:
   None

=======================================================================

IMcastSession_SelectNetworkEx()

Description:
   This function selects a specific data network for this multicast session.
   After the data network is selected, the data network may not be changed
   during the lifetime of the multicast object. 
   
   When an IMcastSession instance is created, no network is selected. 
   If an application calls IMcastSession_Join() before calling 
   IMcastSession_SelectNetworkEx(), a default network will be selected.  

   It is recommended for clients to select the network explicitly
   using IMcastSession_SelectNetworkEx(), prior to calling IMcastSession_Join().
  
Prototype:
   int IMcastSession_SelectNetworkEx(IMcastSession *me, int nNetwork, int16 nFamily)
   
Parameters:
   me: the interface pointer
   nNetwork: data network type (AEE_NETWORK_*)
   nFamily: address family (AEE_AF_*)

Return Value:
   AEE_SUCCESS: the network and family were selected.
   AEE_NET_EINVAL: function arguments are not valid.
   AEE_NET_EOPNOTSUPP: the network has already been selected.
   AEE_EBADSTATE: operation is not applicable in current session state

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   Data networks

=======================================================================

IMcastSession_AddRetryCB()

Description:
   This function should be used by clients in order to be notified when the
   IMcastSession_Join() operation should be retried.

   In the case where IMcastSession_Join() returns AEE_NET_WOULDBLOCK, 
   IMcastSession_Join() should be called again later to obtain the final result
   (failure or success).

   For notification when to call IMcastSession_Join() again, the client should
   register a callback through IMcastSession_AddRetryCB(). Note that when the 
   client's callback is resumed, it is not guaranteed that a subsequent call 
   to IMcastSession_Join() will complete, so the client must be prepared to
   receive AEE_NET_WOULDBLOCK again.

Prototype:
   int IMcastSession_AddRetryCB(IMcastSession *me, AEECallback* pcbRetry)

Parameters:
   me: the interface pointer
   pcbRetry: the client's callback.

Return Value:
   AEE_SUCCESS: client's callback was stored successfully.
   AEE_EBADSTATE: IMcastSession state is not suitable for this operation.
   AEE_EBADPARM: pcbRetry is NULL.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IMcastSession_Join()

=======================================================================
   
IMcastSession_Join()
     
Description:
   This function issues a request for joining a multicast group.

   Prior to calling IMcastSession_Join(), a client has to select a 
   network (see IMcastSession_SelectNetworkEx()), and start listening for 
   multicast events (see IMcastSession_AddListener()).

   IMcastSession_Join() can be called only for a session which is not
   currently registered for a group - either because the session has not
   joined a group, or its registration failed (AEE_MCAST_REGISTER_FAILURE_EV), 
   or the session was deregistered from the group (AEE_MCAST_DEREGISTERED_EV).
   
   A return value of AEE_SUCCESS indicates that the request was submitted. 
   After IMcastSession_Join() returns AEE_SUCCESS, the client has to wait
   for a multicast event which indicates a successful registration
   (AEE_MCAST_REGISTER_SUCCESS_EV), before it can start receiving data 
   sent to the multicast group.
  
   For receiving data sent to the multicast group, the client socket needs
   to select the same network (see ISockPort_SelectNetwork()),
   and to be bound to the same group address and port (see ISockPort_Bind()).
       
   Also note the possibility of an AEE_NET_WOULDBLOCK return value. In that 
   case, IMcastSession_Join() has to be called again to obtain the final result 
   (failure or success). 
   
   For notification of when to call IMcastSession_Join() again, the client should
   register a callback through IMcastSession_AddRetryCB(). Note that when the 
   client's callback is resumed, it is not guaranteed that a subsequent call 
   to IMcastSession_Join() will complete, so the client must be prepared to
   receive AEE_NET_WOULDBLOCK again.
          
Prototype:
   int IMcastSession_Join(IMcastSession* me, void* pGroupAddr)

Parameters:
   me: the interface pointer
   pGroupAddr: multicast group address and port to join - a pointer to 
         an initialized AEESockAddrStorage or AEESockAddrInet structure.

Return Value:
   AEE_SUCCESS:    request was submitted.
   AEE_EBADSTATE:  object state is not suitable for this operation.
   AEE_EBADPARM:   pGroupAddr is NULL or invalid.
   AEE_NET_WOULDBLOCK: the operation was not completed - register a retry 
               callback via IMcastSession_AddRetryCB().
   
   Other error codes are also possible.
  
Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:   
   AEEMcastEvent
   IMcastSession_AddListener()
   IMcastSession_AddRetryCB()
   IMcastSession_Leave()
   IMcastSession_SelectNetworkEx()

=======================================================================

IMcastSession_Leave()

Description:
   This function issues a request for leaving a multicast group.
   
   Note that a return value of AEE_SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that multicast session 
   resources are released yet.

   After IMcastSession_Leave() returns AEE_SUCCESS, the client may listen 
   (see IMcastSession_AddListener()) for multicast events and wait for 
   an event which indicates that the multicast session was released 
   (AEE_MCAST_DEREGISTERED_EV). 

   IMcastSession_Leave() can be called only for a multicast session on which
   the client has previously requested IMcastSession_Join().

Prototype:
   int IMcastSession_Leave(IMcastSession* me)

Parameters:
   me: the interface pointer

Return Value:
   AEE_SUCCESS: the request was issued. 
   AEE_EBADSTATE: IMcastSession state is not suitable for this operation.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   AEEMcastEvent
   IMcastSession_AddListener()
   IMcastSession_Join()

=======================================================================
IMcastSession_GetGroupAddr()

Description:
   This function retrieves the multicast group address and port,
   if the multicast session object is currently registered to a group.

Prototype:
   int IMcastSession_GetGroupAddr(IMcastSession* me, void* pGroupAddr, int* pnSize)

Parameters:
   me: the interface pointer
   pGroupAddr [out]: place holder for the group to which the session is registered - 
      a pointer to an initialized AEESockAddrStorage or AEESockAddrInet structure.   
   pnSize [in/out]: a pointer to the size of the group address.  
         Prior to the call, this size should be initialized to the size of 
         the buffer that pGroupAddr points to. Upon return, this size will 
         reflect the number of bytes written into pGroupAddr.
   
Return Value:
   AEE_SUCCESS: pGroupAddr now contains the multicast group address and port.
   AEE_EBADSTATE: IMcastSession state is not suitable for this operation.
   AEE_EBADPARM: NULL or invalid parameter
   AEE_EBUFFERTOOSMALL: size in pnSize is too small for the group address

   Other error codes are also possible.
   
Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IMcastSession_Join()
   IMcastSession_Leave()

=======================================================================*/

#endif    // AEEIMCASTSESSION_H
