#ifndef AEEMCASTSESSION2_H
#define AEEMCASTSESSION2_H
/*====================================================

FILE: AEEMcastSession2.h

SERVICES:  BREW Multicast Session Interface

GENERAL DESCRIPTION:
   Interface definitions, data structures, etc. 
   for BREW Multicast Session Interface that supports bundling and new events semantics.
   The headers are in beta form and may be changed in future releases.

        Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEModel.h"
#include "AEEMcastSession.h"

//-----------------------------------------------------------------
// Types
//-----------------------------------------------------------------

#define AEE_MCAST_STATUS_EV               (AEE_MCAST_DEREGISTERED_EV+1)

#define AEE_MCAST_BCMCS2_INFO_CODE_CANCELLED 0x0106d850
#define AEE_MCAST_BCMCS2_INFO_CODE_UNABLE_TO_MONITOR 0x0106d852
#define AEE_MCAST_BCMCS2_INFO_CODE_TIMEOUT 0x0106d853
#define AEE_MCAST_BCMCS2_INFO_CODE_LOST 0x0106d854
#define AEE_MCAST_BCMCS2_INFO_CODE_SYS_UNAVAILABLE 0x0106d855
#define AEE_MCAST_BCMCS2_INFO_CODE_AN_REJECT_NOT_AVAILABLE 0x0106f7f8
#define AEE_MCAST_BCMCS2_INFO_CODE_AN_REJECT_NOT_TRANSMITTED 0x0106f7f9
#define AEE_MCAST_BCMCS2_INFO_CODE_AN_REJECT_INVALID_AUTH_SIG 0x0106f7fa
#define AEE_MCAST_BCMCS2_INFO_CODE_UNAVAILABLE 0x0106dd10
#define AEE_MCAST_BCMCS2_INFO_CODE_NO_MAPPING 0x0106dd12
#define AEE_MCAST_BCMCS2_INFO_CODE_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP 0x0106dd13
#define AEE_MCAST_BCMCS2_INFO_CODE_REQUESTED 0x0106dd14
#define AEE_MCAST_BCMCS2_MAX_FLOWS_REACHED 0x0106ddd5
#define AEE_MCAST_BCMCS_JOIN_REQ_IN_PROGRESS 0x0106dd15
#define AEE_MCAST_BCMCS_FLOW_REQUEST_SENT 0x0106dd16
#define AEE_MCAST_BCMCS_INFO_CODE_MAX_MONITORED_FLOWS 0x0106dd18
#define AEE_MCAST_BCMCS_INFO_CODE_MONITORED 0x0106dd19
#define AEE_MCAST_BCMCS_REGISTRATION_SUCCESS 0x0106e604
#define AEE_MCAST_BCMCS_REGISTRATION_NOT_ALLOWED 0x0106e605
#define AEE_MCAST_BCMCS_REGISTRATION_FAILED 0x0106e606
#define AEE_MCAST_BCMCS_FLOW_DEREGISTERED 0x0106f7fb

// Note that new info code ID need to assigned uniquely using the class id generator!

//-----------------------------------------------------------------
// Interface
//-----------------------------------------------------------------

// Interface ID
#define AEEIID_MCASTSESSION2 0x0106d84f

typedef struct IMcastSession2 IMcastSession2;

AEEINTERFACE(IMcastSession2)
{
   INHERIT_IModel(IMcastSession2);
   
   int        (*Join)(IMcastSession2* me, void* pGroupAddr);
   int        (*Leave)(IMcastSession2* me);
   int        (*GetGroupAddr)(IMcastSession2* me, void* pGroupAddr, int* pnSize);
   int        (*SelectNetworkEx)(IMcastSession2* me, int nNetwork, int16 nFamily);
   int        (*AddRetryCB)(IMcastSession2* me, AEECallback* pcbRetry);
   
   // Extension Function to allow future enhancement to the interface without 
   // breaking its binary compatibility.
   int        (*ExtFunc)(IMcastSession2* me, AEECLSID id, void *pBuf, int nBufSize);
};

//-----------------------------------------------------------------
// Methods
//-----------------------------------------------------------------

static __inline uint32 IMCASTSESSION2_AddRef(IMcastSession2 *me)
{
   return AEEGETPVTBL(me, IMcastSession2)->AddRef(me);
}

static __inline uint32 IMCASTSESSION2_Release(IMcastSession2 *me)
{
   return AEEGETPVTBL(me, IMcastSession2)->Release(me);
}

static __inline int IMCASTSESSION2_QueryInterface(IMcastSession2 *me, 
                                                  AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me, IMcastSession2)->QueryInterface(me, cls, ppo);
}

static __inline int IMCASTSESSION2_SelectNetworkEx(IMcastSession2 *me, int nNetwork, int16 nFamily)
{
   return AEEGETPVTBL(me, IMcastSession2)->SelectNetworkEx(me, nNetwork, nFamily);
}

static __inline int IMCASTSESSION2_AddRetryCB(IMcastSession2* me, AEECallback* pcbRetry)
{
   return AEEGETPVTBL(me, IMcastSession2)->AddRetryCB(me, pcbRetry);
}

static __inline int IMCASTSESSION2_Join(IMcastSession2* me, void* pGroupAddr)
{
   return AEEGETPVTBL(me, IMcastSession2)->Join(me, pGroupAddr);
}

static __inline int IMCASTSESSION2_Leave(IMcastSession2* me)
{
   return AEEGETPVTBL(me, IMcastSession2)->Leave(me);
}

static __inline int IMCASTSESSION2_AddListener(IMcastSession2* me, ModelListener *pListener)
{
   return AEEGETPVTBL(me, IMcastSession2)->AddListener(me, pListener);
}

static __inline int IMCASTSESSION2_GetGroupAddr(IMcastSession2* me, void* pGroupAddr, int* pnSize)
{
   return AEEGETPVTBL(me, IMcastSession2)->GetGroupAddr(me, pGroupAddr, pnSize);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================
AEEMcastEvent

Description:
   This data type is used for multicast events.

Definition:
   In AEEMcastSession.h:
   #define AEE_MCAST_REGISTER_SUCCESS_EV     1
   #define AEE_MCAST_REGISTER_FAILURE_EV     2
   #define AEE_MCAST_DEREGISTERED_EV         3

   typedef int AEEMcastEvent;

   In AEEMcastSession2.h:
   #define AEE_MCAST_STATUS_EV               4
  
Members: 
   AEE_MCAST_REGISTER_SUCCESS_EV:~
      This event is generated in response to IMCASTSESSION2_Join(), IMCASTBUNDLE_Join() 
      (for sessions whose abRegSetupFlags entries were TRUE) or IMCASTBUNDLE_Register() operation 
      that has returned SUCCESS, to indicate that the client is now registered 
      with the requested multicast group.

   AEE_MCAST_REGISTER_FAILURE_EV:~
      This event is generated in response to IMCASTSESSION2_Join(), IMCASTBUNDLE_Join() 
      (for sessions whose abRegSetupFlags entries were TRUE) or IMCASTBUNDLE_Register() operation 
      that has returned SUCCESS, to indicate that the client request has 
      eventually failed, and the client was NOT registered with the requested 
      multicast group.

      The cause of this event may be indicated by an AEEMcastInfoCode in 
      dwParam in the ModelEvent parameter to the listener callback function.
 
      After receiving this event, the client must call IMCASTSESSION2_Leave() explicitly
      in order to clean the multicast session internal resources and to allow reuse of 
      the multicast session object for the purpose joining a multicast group.
   
   AEE_MCAST_DEREGISTERED_EV:~
      This event is generated in response to IMCASTSESSION2_Leave() or IMCASTBUNDLE_Leave() 
      operation that has returned SUCCESS, to indicate that the client is no longer 
      registered with a multicast group.
   
      This event is also generated if the client was removed from a multicast 
      group upon request from the network side. 
      
      The cause of this event may be indicated by an AEEMcastInfoCode in 
      dwParam in the ModelEvent parameter to the listener callback function.

      After receiving this event, the client may reuse the multicast 
      session object for joining a multicast group.

   AEE_MCAST_STATUS_EV:~
      This event is generated in response to IMCASTSESSION2_Join(), IMCASTBUNDLE_Join() 
      or IMCASTBUNDLE_Register() operation that has returned SUCCESS, to indicate the 
      current status of the session. This event can also be generated due to changed 
      network conditions and not only as a response to a client operation.
      This event is for technology specific status notification, including transitional 
      error conditions
      
Comments:
   None.

See Also:
  IMCASTSESSION2_AddListener()
  AEEMcastInfoCode

======================================================================
  
AEEMcastInfoCode

Description:
   This data type is used to deliver additional information regarding the cause
   of a multicast event. It is delivered as dwParam in the ModelEvent parameter 
   to a listener callback function. Most information codes are specific to the
   multicast technology, such as MediaFLO or CDMA BCMCS.

Definition:

   In AEEMcastSession.h:   
   typedef int AEEMcastInfoCode;

   In AEEMcastSession2.h:
   #define AEE_MCAST_BCMCS2_INFO_CODE_CANCELLED                           0x0106d850
   #define AEE_MCAST_BCMCS2_INFO_CODE_UNABLE_TO_MONITOR                   0x0106d852
   #define AEE_MCAST_BCMCS2_INFO_CODE_TIMEOUT                             0x0106d853
   #define AEE_MCAST_BCMCS2_INFO_CODE_LOST                                0x0106d854
   #define AEE_MCAST_BCMCS2_INFO_CODE_SYS_UNAVAILABLE                     0x0106d855
   #define AEE_MCAST_BCMCS2_INFO_CODE_UNAVAILABLE                         0x0106dd10
   #define AEE_MCAST_BCMCS2_INFO_CODE_NO_MAPPING                          0x0106dd12
   #define AEE_MCAST_BCMCS2_INFO_CODE_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP 0x0106dd13
   #define AEE_MCAST_BCMCS2_INFO_CODE_REQUESTED                           0x0106dd14
   #define AEE_MCAST_BCMCS2_MAX_FLOWS_REACHED                             0x0106ddd5
   #define AEE_MCAST_BCMCS_JOIN_REQ_IN_PROGRESS                           0x0106dd15
   #define AEE_MCAST_BCMCS_FLOW_REQUEST_SENT                              0x0106dd16
   #define AEE_MCAST_BCMCS_INFO_CODE_MAX_MONITORED_FLOWS                  0x0106dd18
   #define AEE_MCAST_BCMCS_INFO_CODE_MONITORED                            0x0106dd19
   #define AEE_MCAST_BCMCS_REGISTRATION_SUCCESS                           0x0106e604
   #define AEE_MCAST_BCMCS_REGISTRATION_NOT_ALLOWED                       0x0106e605
   #define AEE_MCAST_BCMCS_REGISTRATION_FAILED                            0x0106e606

Members: 
   AEE_MCAST_BCMCS2_INFO_CODE_CANCELLED:
      In CDMA BCMCS, a new Flow Request received before the previous one is done processing.
      Cancel the remaining lookup and start processing the new Request (no-op).
   AEE_MCAST_BCMCS2_INFO_CODE_UNABLE_TO_MONITOR: 
      In CDMA BCMCS, flow can not be monitored, because its channel assignment conflicts 
      with other flows.
   AEE_MCAST_BCMCS2_INFO_CODE_TIMEOUT:
      In CDMA BCMCS, Broadcast Supervision Timeout. The AT is on a system where
      broadcast is available. The AT timed out waiting for the flow to appear in 
      the bcovhd message.
   AEE_MCAST_BCMCS2_INFO_CODE_LOST:
      In CDMA BCMCS, The AT lost acquisition and temporarily disabled its monitored flows.
      It is treated as if the flows weren't in the BroadcastOverhead message. 
      Another flow status will be sent when the AT reacquires.
   AEE_MCAST_BCMCS2_INFO_CODE_SYS_UNAVAILABLE:
      In CDMA BCMCS, the AT cannot process the flow request because
      the BCMCS protocol is in CLOSED state (HDR not acquired, etc.).
   AEE_MCAST_BCMCS2_INFO_CODE_NO_MAPPING:
      In CDMA BCMCS, The BCDB XML file does not contain any mapping for the current subnet.
   AEE_MCAST_BCMCS2_INFO_CODE_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP:
      A lookup in the database can't find a FlowID for the given Multicast IP in the current subnet.
   AEE_MCAST_BCMCS2_INFO_CODE_REQUESTED:
      Registered but not included in broadcast overhead message.
   AEE_MCAST_BCMCS2_MAX_FLOWS_REACHED:
      We've reached the max number of flows that can be REQUESTED.
   AEE_MCAST_BCMCS_JOIN_REQ_IN_PROGRESS:
      Join request successfully received.
   AEE_MCAST_BCMCS_FLOW_REQUEST_SENT:
      Flow Request successfully sent to lower layers.
   AEE_MCAST_BCMCS_INFO_CODE_MAX_MONITORED_FLOWS:
      We've reached the max number of flows that can be MONITORED.
   AEE_MCAST_BCMCS_INFO_CODE_MONITORED: 
      Success - Flow is monitored - Flow is active and included in broadcast overhead msg.
   AEE_MCAST_BCMCS_REGISTRATION_SUCCESS:
      Registration Request was successful.
   AEE_MCAST_BCMCS_REGISTRATION_NOT_ALLOWED:
      Registration is not allowed at this time, Please try again later.
   AEE_MCAST_BCMCS_REGISTRATION_FAILED:
      Lower layers attempted to send the registration but the message did not go through.
    
Comments:
   None.

See Also:
   AEEMcastEvent
   IMCASTSESSION2_AddListener()

======================================================================

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IMcastSession2

   This interface represents a Multicast Session. Applications may use 
   IMcastSession2 to receive data which is sent to a multicast group.

   This interface deprecates the use of ISocket and ISockPort socket options 
   AEE_IP_ADD_MEMBERSHIP and AEE_IP_DROP_MEMBERSHIP for CDMA BCMCS multicast.

   This interface can be used in IMcastBundle operations. See IMcastBundle.h.

   IMcastSession can not be used in IMcastBundle. IMcastSession2 supports new 
   event semantics, such as support for the new STATUS event. IMCastSession 
   is deprecated on mobiles and data networks that support IMcastSession2.

   NOTE: Your application must have a privilege level of Network or All to be able
   to invoke the functions in this interface.

   In order to use a Multicast Session, a client should:
   - Create a new IMcastSession2 object.
   - Select the desired data network by calling IMCASTSESSION2_SelectNetworkEx().  
   - Register a listener callback for receiving multicast events by using 
     IMCASTSESSION2_AddListener().
   - Call IMCASTSESSION2_Join() or IMCASTBUNDLE_Join() with the desired multicast 
     group address and port. IMCASTSESSION2_Join() and IMCASTBUNDLE_Join() perform
     an asynchronous operation, and hence a return value of SUCCESS only means that 
     the Join request has been submitted. In case AEE_NET_WOULDBLOCK was returned, 
     use IMCASTSESSION2_AddRetryCB() or IMCASTBUNDLE_AddRetryCB() to register a 
     callback which will be resumed when the IMCASTSESSION2_Join() or IMCASTSBUNDLE_Join() 
     operation should be retried.
     If the client chose to use IMCASTBUNDLE_Join(), it may choose to perform a
     "join without registration". See AEEMcastBundle.h for more details.
   - Wait for a multicast event which indicates the status of the request. 
     The multicast event will be delivered through the registered listener 
     callback function. Receipt of a successful multicast event 
     (i.e. AEE_MCAST_REGISTER_SUCCESS_EV) means that the client has joined 
     the multicast group. AEE_MCAST_STATUS_EV is also recieved in earlier stages.
     If the client performed "join without registration", it will not join the multicast
     group until it performs registration using "IMCASTBUNDLE_Register(). See AEEMcastBundle.h
     for more details.
   - Now the application can start receiving data sent to the multicast group, 
     using sockets. Create a UDP socket with ISockPort; Select the proper data 
     network; and bind the socket to the multicast group address and port. 
     Now, calling ISOCKPORT_RecvFrom() will read data sent to the group.
   - When the Multicast Session is not needed anymore, call IMCASTSESSION2_Leave() or 
     IMCASTBUNDLE_Leave(). IMCASTSESSION2_Leave() and IMCASTBUNDLE_Leave() perform an 
     asynchronous operation, and hence a return value of SUCCESS only means that the 
     request has been submitted.
   - Wait for a multicast event which indicates that the session was closed. The 
     event will be delivered through the registered listener callback. 
     Receipt of the AEE_MCAST_DEREGISTERED_EV event means that the multicast
     session is closed and its resources are released.

   The following header file is required:~
   AEEMcast2Session.h
  
=============================================================================

IMCASTSESSION2_AddRef()

Description:
   This function is inherited from IBASE_AddRef().

See Also:
   IMCASTSESSION2_Release()

=============================================================================

IMCASTSESSION2_Release()

Description:
   This function is inherited from IBASE_Release().

See Also:
   IMCASTSESSION2_AddRef()

==============================================================================

IMCASTSESSION2_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

==============================================================================

IMCASTSESSION2_AddListener()

Description:
   This function is inherited from IMODEL_AddListener().

   The ModelEvent object received at the listener callback contains:
   - pModel: pointer to the originating object. Use IQI to query it for 
             IMcastSession2 interface
   - evCode: the multicast event that occurred (AEEMcastEvent).
   - dwParam: additional information regarding the cause of the event 
              (AEEMcastInfoCode)

See Also:
   None

=======================================================================

IMCASTSESSION2_SelectNetworkEx()

Description:
   This function selects a specific data network for this multicast session.
   After the data network is selected, the data network may not be changed
   during the lifetime of the multicast object. 
   
   When an IMcastSession2 instance is created, no network is selected. 
   If an application calls IMCASTSESSION2_Join() before calling 
   IMCASTSESSION2_SelectNetworkEx(), a default network will be selected by
   BREW.  

   It is recommended for the application to select the network explicitly
   using IMCASTSESSION2_SelectNetworkEx(), prior to calling IMCASTSESSION2_Join().
  
Prototype:
   int IMCASTSESSION2_SelectNetworkEx(IMcastSession2 *me, int nNetwork, int16 nFamily)
   
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
   Introduced BREW Client <3.1.5 SP02>

See Also:
   Data networks

=======================================================================

IMCASTSESSION2_AddRetryCB()

Description:
   This function should be used by clients in order to be notified when the
   IMCASTSESSION2_Join() operation should be retried.

   In the case where IMCASTSESSION2_Join() returns AEE_NET_WOULDBLOCK, 
   IMCASTSESSION2_Join() should be called again later to obtain the final result
   (failure or success).

   For notification when to call IMCASTSESSION2_Join() again, the client should
   register a callback through IMCASTSESSION2_AddRetryCB(). Note that when the 
   client's callback is resumed, it is not guaranteed that a subsequent call 
   to IMCASTSESSION2_Join() will complete, so the client must be prepared to
   receive AEE_NET_WOULDBLOCK again.

Prototype:
   int IMCASTSESSION2_AddRetryCB(IMcastSession2 *me, AEECallback* pcbRetry)

Parameters:
   me: the interface pointer
   pcbRetry: the client's callback.

Return Value:
   SUCCESS: client's callback was stored successfully.
   EBADSTATE: IMcastSession2 state is not suitable for this operation.
   EBADPARM: pcbRetry is NULL.

Comments:
   None.

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   IMCASTSESSION2_Join()

=======================================================================
   
IMCASTSESSION2_Join()
     
Description:
   This function issues a request for joining a multicast group.

   Prior to calling IMCASTSESSION2_Join(), the application should select a 
   network (see IMCASTSESSION2_SelectNetworkEx()), and start listening for 
   multicast events (see IMCASTSESSION2_AddListener()).

   IMCASTSESSION2_Join() can be called only for a session which is not
   currently registered for a group - either because the session has not
   joined a group, or its registration failed (AEE_MCAST_REGISTER_FAILURE_EV), 
   or the session was deregistered from the group (AEE_MCAST_DEREGISTERED_EV).
   
   A return value of SUCCESS indicates that the request was submitted. 
   After IMCASTSESSION2_Join() returns SUCCESS, the client has to wait
   for a multicast event which indicates a successful registration
   (AEE_MCAST_REGISTER_SUCCESS_EV), before it can start receiving data 
   sent to the multicast group.

   AEE_MCAST_STATUS_EV is received at earlier stages of the operation.
  
   For receiving data sent to the multicast group, the client socket needs
   to select the same network (see ISOCKPORT_SelectNetwork()),
   and to be bound to the same group address and port (see ISOCKPORT_Bind()).
       
   Also note the possibility of an AEE_NET_WOULDBLOCK return value. In that 
   case, IMCASTSESSION2_Join() has to be called again to obtain the final result 
   (failure or success). 
   
   For notification of when to call IMCASTSESSION2_Join() again, the client should
   register a callback through IMCASTSESSION2_AddRetryCB(). Note that when the 
   client's callback is resumed, it is not guaranteed that a subsequent call 
   to IMCASTSESSION2_Join() will complete, so the client must be prepared to
   receive AEE_NET_WOULDBLOCK again.
          
Prototype:
   int IMCASTSESSION2_Join(IMcastSession2* me, void* pGroupAddr)

Parameters:
   me: the interface pointer
   pGroupAddr: multicast group address and port to join - a pointer to 
               an initialized AEESockAddrStorage or AEESockAddrInet structure.

Return Value:
   SUCCESS:    request was submitted.
   EBADSTATE:  object state is not suitable for this operation.
   EBADPARM:   pGroupAddr is NULL or invalid.
   AEE_NET_WOULDBLOCK: the operation was not completed - register a retry 
                       callback via IMCASTSESSION2_AddRetryCB().
   
   Other error codes are also possible.
  
Comments:
   None.

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   AEEMcastEvent
   IMCASTSESSION2_AddListener()
   IMCASTSESSION2_AddRetryCB()
   IMCASTSESSION2_Leave()
   IMCASTSESSION2_SelectNetworkEx()

=======================================================================
IMCASTSESSION2_Leave()

Description:
   This function issues a request for leaving a multicast group.
   
   Note that a return value of SUCCESS indicates that the request was 
   successfully issued but it doesn't indicate that multicast session 
   resources are released yet.

   After IMCASTSESSION2_Leave() returns SUCCESS, the client may listen 
   (see IMCASTSESSION2_AddListener()) for multicast events and wait for 
   an event which indicates that the multicast session was released 
   (AEE_MCAST_DEREGISTERED_EV). 

   IMCASTSESSION2_Leave() can be called only for a multicast session on which
   the application has previously requested IMCASTSESSION2_Join().

Prototype:
   int IMCASTSESSION2_Leave(IMcastSession2* me)

Parameters:
   me: the interface pointer

Return Value:
   SUCCESS: the request was issued. 
   EBADSTATE: IMcastSession2 state is not suitable for this operation.

Comments:
   None.

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   AEEMcastEvent
   IMCASTSESSION2_AddListener()
   IMCASTSESSION2_Join()

=======================================================================
IMCASTSESSION2_GetGroupAddr()

Description:
   This function retrieves the multicast group address and port,
   if the multicast session object is currently registered to a group.

Prototype:
   int IMCASTSESSION2_GetGroupAddr(IMcastSession2* me, void* pGroupAddr, int* pnSize)

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
   EBADSTATE: IMcastSession2 state is not suitable for this operation.
   EBADPARM: NULL or invalid parameter
   EBUFFERTOOSMALL: size in pnSize is too small for the group address

   Other error codes are also possible.
   
Comments:
   None.

Version:
   Introduced BREW Client <3.1.5 SP02>

See Also:
   IMCASTSESSION2_Join()
   IMCASTSESSION2_Leave()
                    
=======================================================================*/


#endif    // AEEMCASTSESSION2_H
