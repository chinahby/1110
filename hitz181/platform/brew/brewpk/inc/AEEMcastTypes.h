#ifndef AEEMCASTTYPES_H
#define AEEMCASTTYPES_H
/*=============================================================================

FILE:         AEEMcastTypes.h

SERVICES:     None

DESCRIPTION:  Common mcast types and definitions

===============================================================================
   Copyright (c) 2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=============================================================================*/

#define AEE_MCAST_REGISTER_SUCCESS_EV                       0x1
#define AEE_MCAST_REGISTER_FAILURE_EV                       0x2
#define AEE_MCAST_DEREGISTERED_EV                           0x3

typedef int AEEMcastEvent;

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

#define AEE_MCAST_MFLO_INFO_CODE_BASE                         0x300
#define AEE_MCAST_MFLO_INFO_CODE_IP_OR_PORT_NOT_SUPPORTED     0x301
#define AEE_MCAST_MFLO_INFO_CODE_NO_AUTHORIZATION             0x302
#define AEE_MCAST_MFLO_INFO_CODE_NO_SYSTEM_COVERAGE           0x303
#define AEE_MCAST_MFLO_INFO_CODE_MAX_FLOW_REACHED             0x304

#define AEE_MCAST_DVBH_INFO_CODE_IP_OR_PORT_NOT_FOUND         0x01061993
#define AEE_MCAST_DVBH_INFO_CODE_SYSTEM_UNAVAILABLE           0x01061994
#define AEE_MCAST_DVBH_INFO_CODE_BAD_REQUEST                  0x01061995
#define AEE_MCAST_DVBH_INFO_CODE_REQUEST_CONFLICT             0x01061996
#define AEE_MCAST_DVBH_INFO_CODE_DUP_REQUEST                  0x01061997
#define AEE_MCAST_DVBH_INFO_CODE_MAX_FLOWS_REACHED            0x01061998

// Note that new info code ID need to assigned uniquely using the class id generator !

typedef int AEEMcastInfoCode;


/*=====================================================================
DATA STRUCTURE DOCUMENTATION
======================================================================
AEEMcastEvent

Description:
   This data type is used for multicast events.

Definition:
   #define AEE_MCAST_REGISTER_SUCCESS_EV     0x1
   #define AEE_MCAST_REGISTER_FAILURE_EV     0x2
   #define AEE_MCAST_DEREGISTERED_EV         0x3

   typedef int AEEMcastEvent;

Members:
   AEE_MCAST_REGISTER_SUCCESS_EV:~
      This event is generated in response to  multicast Join() operation
      that has returned AEE_SUCCESS, to indicate that the client is now registered
      with the requested multicast group.

   AEE_MCAST_REGISTER_FAILURE_EV:~
      This event is generated in response to multicast Join() operation
      that has returned AEE_SUCCESS, to indicate that the client request has
      eventually failed, and the client was NOT registered with the requested
      multicast group.

      The cause of this event may be indicated by an AEEMcastInfoCode.

      After receiving this event, the client may reuse the object for joining
      another multicast group.

   AEE_MCAST_DEREGISTERED_EV:~
      This event is generated in response to multicast Leave() operation
      that has returned AEE_SUCCESS, to indicate that the client is no longer
      registered with a multicast group.

      This event is also generated if the client was removed from a multicast
      group upon request from the network side.

      The cause of this event may be indicated by an AEEMcastInfoCode.

      After receiving this event, the client may reuse the object for joining
      another multicast group.

Comments:
   None.

See Also:
   AEEMcastInfoCode

======================================================================

AEEMcastInfoCode

Description:
   This data type is used to deliver additional information regarding the cause
   of a multicast event. Most information codes are specific to a
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
   
   #define AEE_MCAST_MFLO_INFO_CODE_BASE                         0x300
   #define AEE_MCAST_MFLO_INFO_CODE_IP_OR_PORT_NOT_SUPPORTED     0x301
   #define AEE_MCAST_MFLO_INFO_CODE_NO_AUTHORIZATION             0x302
   #define AEE_MCAST_MFLO_INFO_CODE_NO_SYSTEM_COVERAGE           0x303
   #define AEE_MCAST_MFLO_INFO_CODE_MAX_FLOW_REACHED             0x304

   #define AEE_MCAST_DVBH_INFO_CODE_IP_OR_PORT_NOT_FOUND         0x01061993
   #define AEE_MCAST_DVBH_INFO_CODE_SYSTEM_UNAVAILABLE           0x01061994
   #define AEE_MCAST_DVBH_INFO_CODE_BAD_REQUEST                  0x01061995
   #define AEE_MCAST_DVBH_INFO_CODE_REQUEST_CONFLICT             0x01061996
   #define AEE_MCAST_DVBH_INFO_CODE_DUP_REQUEST                  0x01061997
   #define AEE_MCAST_DVBH_INFO_CODE_MAX_FLOWS_REACHED            0x01061998

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
      
   AEE_MCAST_DVBH_INFO_CODE_IP_OR_PORT_NOT_FOUND     
      In DVBH, flow IP address or port are not supported.
   AEE_MCAST_DVBH_INFO_CODE_SYSTEM_UNAVAILABLE          
      In DVBH, failure in lower layers.
   AEE_MCAST_DVBH_INFO_CODE_BAD_REQUEST                           
      In DVBH, request was  malformed
   AEE_MCAST_DVBH_INFO_CODE_REQUEST_CONFLICT
      In DVBH, the current request conflicts with a previous one.
   AEE_MCAST_DVBH_INFO_CODE_DUP_REQUEST
      In DVBH, the current request is a duplicate of a previous one.
   AEE_MCAST_DVBH_INFO_CODE_MAX_FLOWS_REACHED
       In DVBH, maximum number of flows was reached.
   
  
Comments:
   None.

See Also:
   AEEMcastEvent

======================================================================*/
#endif  // AEEMCASTTYPES_H
