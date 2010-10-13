#ifndef AEEMCASTSESSION_H
#define AEEMCASTSESSION_H
/*====================================================

FILE: AEEMcastSession.h

SERVICES:  BREW Multicast Session Interface

GENERAL DESCRIPTION:
	Interface definitions, data structures, etc. 
   for BREW Multicast Session Interface

        Copyright © 2005-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEModel.h"

//-----------------------------------------------------------------
// Types
//-----------------------------------------------------------------

#define AEE_MCAST_REGISTER_SUCCESS_EV     (EVT_MDL_GENERIC_CHANGE+1)
#define AEE_MCAST_REGISTER_FAILURE_EV     (EVT_MDL_GENERIC_CHANGE+2)
#define AEE_MCAST_DEREGISTERED_EV         (EVT_MDL_GENERIC_CHANGE+3)

typedef int AEEMcastEvent;



#define AEE_MCAST_INFO_CODE_NOT_SPECIFIED                      0x0

#define AEE_MCAST_BCMCS_INFO_CODE_BASE                         0x200
#define AEE_MCAST_BCMCS_INFO_CODE_UNABLE_TO_MONITOR            0x201
#define AEE_MCAST_BCMCS_INFO_CODE_ID_NOT_FOUND                 0x202
#define AEE_MCAST_BCMCS_INFO_CODE_NO_MAPPING                   0x203
#define AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT                    0x204
#define AEE_MCAST_BCMCS_INFO_CODE_CANCELLED                    0x205
#define AEE_MCAST_BCMCS_INFO_CODE_UNAVAILABLE                  0x206
#define AEE_MCAST_BCMCS_INFO_CODE_TIMEOUT                      0x207
#define AEE_MCAST_BCMCS_INFO_CODE_LOST                         0x208
#define AEE_MCAST_BCMCS_INFO_CODE_MAX_FLOWS_REACHED            0x209
#define AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_NOT_AVAILABLE      0x20A
#define AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_NOT_TRANSMITTED    0x20B
#define AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_INVALID_AUTH_SIG   0x20C
#define AEE_MCAST_BCMCS_INFO_CODE_REQUESTED                    0x20D
#define AEE_MCAST_BCMCS_INFO_CODE_SYS_UNAVAILABLE              0x20E


#define AEE_MCAST_MFLO_INFO_CODE_BASE                       0x300
#define AEE_MCAST_MFLO_INFO_CODE_IP_OR_PORT_NOT_SUPPORTED   0x301
#define AEE_MCAST_MFLO_INFO_CODE_NO_AUTHORIZATION           0x302
#define AEE_MCAST_MFLO_INFO_CODE_NO_SYSTEM_COVERAGE         0x303
#define AEE_MCAST_MFLO_INFO_CODE_MAX_FLOW_REACHED           0x304

// Note that new info code ID need to assigned uniquely using the class id generator !

typedef int AEEMcastInfoCode;

//-----------------------------------------------------------------
// Interface
//-----------------------------------------------------------------

// Interface ID
#define AEEIID_MCASTSESSION 0x01031dfc

typedef struct IMcastSession IMcastSession;

AEEINTERFACE(IMcastSession)
{
   INHERIT_IModel(IMcastSession);
   
   int        (*Join)(IMcastSession* me, void* pGroupAddr);
   int        (*Leave)(IMcastSession* me);
   int        (*GetGroupAddr)(IMcastSession* me, void* pGroupAddr, int* pnSize);
   int        (*SelectNetworkEx)(IMcastSession* me, int nNetwork, int16 nFamily);
   int        (*AddRetryCB)(IMcastSession* me, AEECallback* pcbRetry);
   
   // Extension Function to allow future enhancement to the interface without 
   // breaking its binary compatibility.
   int        (*ExtFunc)(IMcastSession* me, AEECLSID id, void *pBuf, int nBufSize);
};

//-----------------------------------------------------------------
// Methods
//-----------------------------------------------------------------

static __inline uint32 IMCASTSESSION_AddRef(IMcastSession *me)
{
   return AEEGETPVTBL(me, IMcastSession)->AddRef(me);
}

static __inline uint32 IMCASTSESSION_Release(IMcastSession *me)
{
   return AEEGETPVTBL(me, IMcastSession)->Release(me);
}

static __inline int IMCASTSESSION_QueryInterface(IMcastSession *me, 
                                            AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me, IMcastSession)->QueryInterface(me, cls, ppo);
}

static __inline int IMCASTSESSION_SelectNetworkEx(IMcastSession *me, int nNetwork, int16 nFamily)
{
   return AEEGETPVTBL(me, IMcastSession)->SelectNetworkEx(me, nNetwork, nFamily);
}

static __inline int IMCASTSESSION_AddRetryCB(IMcastSession* me, AEECallback* pcbRetry)
{
   return AEEGETPVTBL(me, IMcastSession)->AddRetryCB(me, pcbRetry);
}

static __inline int IMCASTSESSION_Join(IMcastSession* me, void* pGroupAddr)
{
   return AEEGETPVTBL(me, IMcastSession)->Join(me, pGroupAddr);
}

static __inline int IMCASTSESSION_Leave(IMcastSession* me)
{
   return AEEGETPVTBL(me, IMcastSession)->Leave(me);
}

static __inline int IMCASTSESSION_AddListener(IMcastSession* me, ModelListener *pListener)
{
   return AEEGETPVTBL(me, IMcastSession)->AddListener(me, pListener);
}

static __inline int IMCASTSESSION_GetGroupAddr(IMcastSession* me, void* pGroupAddr, int* pnSize)
{
   return AEEGETPVTBL(me, IMcastSession)->GetGroupAddr(me, pGroupAddr, pnSize);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================
AEEMcastEvent

Description:
   This data type is used for multicast events.

Definition:
   #define AEE_MCAST_REGISTER_SUCCESS_EV     1
   #define AEE_MCAST_REGISTER_FAILURE_EV     2
   #define AEE_MCAST_DEREGISTERED_EV         3

   typedef int AEEMcastEvent;
  
Members: 
   AEE_MCAST_REGISTER_SUCCESS_EV:~
      This event is generated in response to IMCASTSESSION_Join() operation 
      that has returned SUCCESS, to indicate that the client is now registered 
      with the requested multicast group.

   AEE_MCAST_REGISTER_FAILURE_EV:~
      This event is generated in response to IMCASTSESSION_Join() operation 
      that has returned SUCCESS, to indicate that the client request has 
      eventually failed, and the client was NOT registered with the requested 
      multicast group.

      The cause of this event may be indicated by an AEEMcastInfoCode in 
      dwParam in the ModelEvent parameter to the listener callback function.
 
      After receiving this event, the client may reuse the multicast 
      session object for joining a multicast group.
   
   AEE_MCAST_DEREGISTERED_EV:~
      This event is generated in response to IMCASTSESSION_Leave() operation 
      that has returned SUCCESS, to indicate that the client is no longer 
      registered with a multicast group.
   
      This event is also generated if the client was removed from a multicast 
      group upon request from the network side. 
      
      The cause of this event may be indicated by an AEEMcastInfoCode in 
      dwParam in the ModelEvent parameter to the listener callback function.

      After receiving this event, the client may reuse the multicast 
      session object for joining a multicast group.
      
Comments:
   None.

See Also:
  IMCASTSESSION_AddListener()
  AEEMcastInfoCode

======================================================================
  
AEEMcastInfoCode

Description:
   This data type is used to deliver additional information regarding the cause
   of a multicast event. It is delivered as dwParam in the ModelEvent parameter 
   to a listener callback function. Most information codes are specific to the
   multicast technology, such as MediaFLO or CDMA BCMCS.

Definition:

   #define AEE_MCAST_INFO_CODE_NOT_SPECIFIED                     0x0
															     
   #define AEE_MCAST_BCMCS_INFO_CODE_BASE                        0x200
   #define AEE_MCAST_BCMCS_INFO_CODE_UNABLE_TO_MONITOR           0x201
   #define AEE_MCAST_BCMCS_INFO_CODE_ID_NOT_FOUND                0x202
   #define AEE_MCAST_BCMCS_INFO_CODE_NO_MAPPING                  0x203
   #define AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT                   0x204
   #define AEE_MCAST_BCMCS_INFO_CODE_CANCELLED                   0x205
   #define AEE_MCAST_BCMCS_INFO_CODE_UNAVAILABLE                 0x206
   #define AEE_MCAST_BCMCS_INFO_CODE_TIMEOUT                     0x207
   #define AEE_MCAST_BCMCS_INFO_CODE_LOST                        0x208
   #define AEE_MCAST_BCMCS_INFO_CODE_MAX_FLOWS_REACHED           0x209
   #define AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_NOT_AVAILABLE     0x20A
   #define AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_NOT_TRANSMITTED   0x20B
   #define AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_INVALID_AUTH_SIG  0x20C
   #define AEE_MCAST_BCMCS_INFO_CODE_REQUESTED                   0x20D
   #define AEE_MCAST_BCMCS_INFO_CODE_SYS_UNAVAILABLE             0x20E

   #define AEE_MCAST_MFLO_INFO_CODE_BASE                       0x300
   #define AEE_MCAST_MFLO_INFO_CODE_IP_OR_PORT_NOT_SUPPORTED   0x301
   #define AEE_MCAST_MFLO_INFO_CODE_NO_AUTHORIZATION           0x302
   #define AEE_MCAST_MFLO_INFO_CODE_NO_SYSTEM_COVERAGE         0x303
   #define AEE_MCAST_MFLO_INFO_CODE_MAX_FLOW_REACHED           0x304
   
   typedef int AEEMcastInfoCode;

Members: 
   AEE_MCAST_INFO_CODE_NOT_SPECIFIED: 
      Not Specified.
   AEE_MCAST_BCMCS_INFO_CODE_UNABLE_TO_MONITOR: 
      In CDMA BCMCS, flow can not be monitored, because its channel assignment conflicts 
      with other flows.
   AEE_MCAST_BCMCS_INFO_CODE_ID_NOT_FOUND:
      In CDMA BCMCS, A lookup in the XML file can't find a FlowID for the given Multicast 
      IP address in the current subnet
   AEE_MCAST_BCMCS_INFO_CODE_NO_MAPPING:
      In CDMA BCMCS, The BCDB XML file does not contain any mapping for the current subnet.
   AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT:
      In CDMA BCMCS, flow was rejected by the network. In newer targets this code
      was replaced by more specific codes (see more AN_REJECT codes below).
   AEE_MCAST_BCMCS_INFO_CODE_CANCELLED:
      In CDMA BCMCS, a new Flow Request received before the previous one is done processing.
      Cancel the remaining lookup and start processing the new Request (no-op).
   AEE_MCAST_BCMCS_INFO_CODE_UNAVAILABLE:
      In CDMA BCMCS, flow is unavailable.
   AEE_MCAST_BCMCS_INFO_CODE_TIMEOUT:
      In CDMA BCMCS, Broadcast Supervision Timeout. The AT is on a system where
      broadcast is available. The AT timed out waiting for the flow to appear in 
      the bcovhd message.
   AEE_MCAST_BCMCS_INFO_CODE_LOST:
      In CDMA BCMCS, The AT lost acquisition and temporarily disabled its monitored flows.
      It is treated as if the flows weren't in the BroadcastOverhead message. 
      Another flow status will be sent when the AT reacquires.
   AEE_MCAST_BCMCS_INFO_CODE_MAX_FLOWS_REACHED:
      In CDMA BCMCS, maximum number of flows was reached.
   AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_NOT_AVAILABLE:
      In CDMA BCMCS, flow was rejected explicitly by the network - 
      BCMCS Flow ID / BCMCS Program ID not available.
   AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_NOT_TRANSMITTED:
      In CDMA BCMCS, flow was rejected explicitly by the network -
      BCMCS Flow ID / BCMCS Program ID not transmitted.
   AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_INVALID_AUTH_SIG:  
      In CDMA BCMCS, flow was rejected explicitly by the network -
      Invalid authorization signature.
   AEE_MCAST_BCMCS_INFO_CODE_REQUESTED:
      In CDMA BCMCS, flow was registered but not included in
      broadcast overhead msg.
   AEE_MCAST_BCMCS_INFO_CODE_SYS_UNAVAILABLE:
      In CDMA BCMCS, the AT cannot process the flow request because
      the BCMCS protocol is in CLOSED state (HDR not acquired, etc.).
   AEE_MCAST_MFLO_INFO_CODE_IP_OR_PORT_NOT_SUPPORTED:
      In MFLO, flow IP address or port are not supported.
   AEE_MCAST_MFLO_INFO_CODE_NO_AUTHORIZATION:
      In MFLO, no authorization to use this flow.
   AEE_MCAST_MFLO_INFO_CODE_NO_SYSTEM_COVERAGE:
      In MFLO, no system coverage.
   AEE_MCAST_MFLO_INFO_CODE_MAX_FLOW_REACHED:        
      In MFLO, maximum number of flows was reached.
    
Comments:
   None.

See Also:
   AEEMcastEvent
   IMCASTSESSION_AddListener()

======================================================================

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IMcastSession

This interface represents a Multicast Session. Applications may use 
IMcastSession to receive data which is sent to a multicast group.

This interface deprecates the use of ISocket and ISockPort socket options 
AEE_IP_ADD_MEMBERSHIP and AEE_IP_DROP_MEMBERSHIP for CDMA BCMCS multicast.

NOTE: Your application must have a privilege level of Network or All to be able
to invoke the functions in this interface.

In order to use a Multicast Session, a client should:
- Create a new IMcastSession object.
- Select the desired data network by calling IMCASTSESSION_SelectNetworkEx().  
- Register a listener callback for receiving multicast events by using 
  IMCASTSESSION_AddListener().
- Call IMCASTSESSION_Join() with the desired multicast group address and port.
  IMCASTSESSION_Join() performs an asynchronous operation, and hence a 
  return value of SUCCESS only means that the Join request has been 
  submitted. In case IMCASTSESSION_Join() returns AEE_NET_WOULDBLOCK, 
  use IMCASTSESSION_AddRetryCB() to register a callback which will be 
  resumed when the IMCASTSESSION_Join() operation should be retried.
- Wait for a multicast event which indicates the status of the request. 
  The multicast event will be delivered through the registered listener 
  callback function. Receipt of a successful multicast event 
  (i.e. AEE_MCAST_REGISTER_SUCCESS_EV) means that the client has joined 
  the multicast group. 
- Now the application can start receiving data sent to the multicast group, 
  using sockets. Create a UDP socket with ISockPort; Select the proper data 
  network; and bind the socket to the multicast group address and port. 
  Now, calling ISOCKPORT_RecvFrom() may read data sent to the group.
- When the Multicast Session is not needed anymore, call IMCASTSESSION_Leave().
  IMCASTSESSION_Leave() performs an asynchronous operation, and hence a 
  return value of SUCCESS only means that the request has been submitted.
- Wait for a multicast event which indicates that the session was closed. The 
  event will be delivered thru the registered listener callback. 
  Receipt of the AEE_MCAST_DEREGISTERED_EV event means that the multicast
  session is closed and its resources are released.

The following header file is required:~
AEEMcastSession.h
  
=============================================================================

IMCASTSESSION_AddRef()

Description:
   This function is inherited from IBASE_AddRef().

See Also:
   IMCASTSESSION_Release()

=============================================================================

IMCASTSESSION_Release()

Description:
   This function is inherited from IBASE_Release().

See Also:
   IMCASTSESSION_AddRef()

==============================================================================

IMCASTSESSION_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

==============================================================================

IMCASTSESSION_AddListener()

Description:
   This function is inherited from IMODEL_AddListener().

   The ModelEvent object received at the listener callback contains:
   - pModel: pointer to the originating object. Use IQI to query it for 
             IMcastSession interface
   - evCode: the multicast event that occurred (AEEMcastEvent).
   - dwParam: additional information regarding the cause of the event 
              (AEEMcastInfoCode)

See Also:
   None

=======================================================================

IMCASTSESSION_SelectNetworkEx()

Description:
   This function selects a specific data network for this multicast session.
   After the data network is selected, the data network may not be changed
   during the lifetime of the multicast object. 
   
   When an IMcastSession instance is created, no network is selected. 
   If an application calls IMCASTSESSION_Join() before calling 
   IMCASTSESSION_SelectNetworkEx(), a default network will be selected by
   BREW.  

   It is recommended for the application to select the network explicitly
   using IMCASTSESSION_SelectNetworkEx(), prior to calling IMCASTSESSION_Join().
  
Prototype:
   int IMCASTSESSION_SelectNetworkEx(IMcastSession *me, int nNetwork, int16 nFamily)
   
Parameters:
   me: the interface pointer
   nNetwork: data network type (AEE_NETWORK_*)
   nFamily: address family (AEE_AF_*)

Return Value:
   SUCCESS: the network and family were selected.
   AEE_NET_EINVAL: function arguments are not valid.
   AEE_NET_EOPNOTSUPP: the network has already been selected.
   EBADSTATE: operation is not applicable in current session state

   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   Data networks

=======================================================================

IMCASTSESSION_AddRetryCB()

Description:
   This function should be used by clients in order to be notified when the
   IMCASTSESSION_Join() operation should be retried.

   In the case where IMCASTSESSION_Join() returns AEE_NET_WOULDBLOCK, 
   IMCASTSESSION_Join() should be called again later to obtain the final result
   (failure or success).

   For notification when to call IMCASTSESSION_Join() again, the client should
   register a callback through IMCASTSESSION_AddRetryCB(). Note that when the 
   client's callback is resumed, it is not guaranteed that a subsequent call 
   to IMCASTSESSION_Join() will complete, so the client must be prepared to
   receive AEE_NET_WOULDBLOCK again.

Prototype:
   int IMCASTSESSION_AddRetryCB(IMcastSession *me, AEECallback* pcbRetry)

Parameters:
   me: the interface pointer
   pcbRetry: the client's callback.

Return Value:
   SUCCESS: client's callback was stored successfully.
   EBADSTATE: IMcastSession state is not suitable for this operation.
   EBADPARM: pcbRetry is NULL.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IMCASTSESSION_Join()

=======================================================================
   
IMCASTSESSION_Join()
     
Description:
   This function issues a request for joining a multicast group.

   Prior to calling IMCASTSESSION_Join(), the application has to select a 
   network (see IMCASTSESSION_SelectNetworkEx()), and start listening for 
   multicast events (see IMCASTSESSION_AddListener()).

   IMCASTSESSION_Join() can be called only for a session which is not
   currently registered for a group - either because the session has not
   joined a group, or its registration failed (AEE_MCAST_REGISTER_FAILURE_EV), 
   or the session was deregistered from the group (AEE_MCAST_DEREGISTERED_EV).
   
   A return value of SUCCESS indicates that the request was submitted. 
   After IMCASTSESSION_Join() returns SUCCESS, the client has to wait
   for a multicast event which indicates a successful registration
   (AEE_MCAST_REGISTER_SUCCESS_EV), before it can start receiving data 
   sent to the multicast group.
  
   For receiving data sent to the multicast group, the client socket needs
   to select the same network (see ISOCKPORT_SelectNetwork()),
   and to be bound to the same group address and port (see ISOCKPORT_Bind()).
       
   Also note the possibility of an AEE_NET_WOULDBLOCK return value. In that 
   case, IMCASTSESSION_Join() has to be called again to obtain the final result 
   (failure or success). 
   
   For notification of when to call IMCASTSESSION_Join() again, the client should
   register a callback through IMCASTSESSION_AddRetryCB(). Note that when the 
   client's callback is resumed, it is not guaranteed that a subsequent call 
   to IMCASTSESSION_Join() will complete, so the client must be prepared to
   receive AEE_NET_WOULDBLOCK again.
          
Prototype:
   int IMCASTSESSION_Join(IMcastSession* me, void* pGroupAddr)

Parameters:
   me: the interface pointer
   pGroupAddr: multicast group address and port to join - a pointer to 
         an initialized AEESockAddrStorage or AEESockAddrInet structure.

Return Value:
   SUCCESS:    request was submitted.
   EBADSTATE:  object state is not suitable for this operation.
   EBADPARM:   pGroupAddr is NULL or invalid.
   AEE_NET_WOULDBLOCK: the operation was not completed - register a retry 
               callback via IMCASTSESSION_AddRetryCB().
   
   Other error codes are also possible.
  
Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   AEEMcastEvent
   IMCASTSESSION_AddListener()
   IMCASTSESSION_AddRetryCB()
   IMCASTSESSION_Leave()
   IMCASTSESSION_SelectNetworkEx()

=======================================================================
IMCASTSESSION_Leave()

Description:
   This function issues a request for leaving a multicast group.
   
   Note that a return value of SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that multicast session 
   resources are released yet.

   After IMCASTSESSION_Leave() returns SUCCESS, the client may listen 
   (see IMCASTSESSION_AddListener()) for multicast events and wait for 
   an event which indicates that the multicast session was released 
   (AEE_MCAST_DEREGISTERED_EV). 

   IMCASTSESSION_Leave() can be called only for a multicast session on which
   the application has previously requested IMCASTSESSION_Join().

Prototype:
   int IMCASTSESSION_Leave(IMcastSession* me)

Parameters:
   me: the interface pointer

Return Value:
   SUCCESS: the request was issued. 
   EBADSTATE: IMcastSession state is not suitable for this operation.

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   AEEMcastEvent
   IMCASTSESSION_AddListener()
   IMCASTSESSION_Join()

=======================================================================
IMCASTSESSION_GetGroupAddr()

Description:
   This function retrieves the multicast group address and port,
   if the multicast session object is currently registered to a group.

Prototype:
   int IMCASTSESSION_GetGroupAddr(IMcastSession* me, void* pGroupAddr, int* pnSize)

Parameters:
   me: the interface pointer
   pGroupAddr [out]: place holder for the group to which the session is registered - 
      a pointer to an initialized AEESockAddrStorage or AEESockAddrInet structure.   
   pnSize [in/out]: a pointer to the size of the group address.  
         Prior to the call, this size should be initialized to the size of 
         the space that pGroupAddr points to. Upon return, this size will 
         reflect the number of bytes written into pGroupAddr.
   
Return Value:
   SUCCESS: pGroupAddr now contains the multicast group address and port.
   EBADSTATE: IMcastSession state is not suitable for this operation.
   EBADPARM: NULL or invalid parameter
   EBUFFERTOOSMALL: size in pnSize is too small for the group address

   Other error codes are also possible.
   
Comments:
   None.

Version:
   Introduced BREW Client 3.1.5

See Also:
   IMCASTSESSION_Join()
   IMCASTSESSION_Leave()
                    
=======================================================================*/


#endif    // AEEMCASTSESSION_H

