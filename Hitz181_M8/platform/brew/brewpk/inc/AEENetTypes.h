#ifndef AEENETTYPES_H
#define AEENETTYPES_H
/*=============================================================================

FILE:         AEENetTypes.h

SERVICES:     None

DESCRIPTION:  Common network types and definitions

===============================================================================
   Copyright (c) 2006-2008 QUALCOMM Incorporated.
               All Rights Reserved.
   Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEEStdDef.h"

//
// Data networks
//
#define AEE_NETWORK_DEFAULT              (0x000)            // Default network
#define AEE_NETWORK_CDMA_SN              (0x001)            // CDMA Service Network
#define AEE_NETWORK_CDMA(profile)        (0x001|(profile)<<16) // CDMA Service Networks
#define AEE_NETWORK_CDMA_BCMCS           (0x002)            // CDMA BroadCast/MultiCast Service
#define AEE_NETWORK_UMTS(pdp)            (0x008|(pdp)<<16)  // UMTS (Packet Data Profile)
#define AEE_NETWORK_WLAN                 (0x010)            // WLAN Network
#define AEE_NETWORK_MFLO_MCAST           (0x020)            // MediaFLO Multicast Service
#define AEE_NETWORK_DVBH_MCAST           (0x040)            // DVB-H Multicast Service
#define AEE_NETWORK_LOOPBACK             (0x080)            // Loopback
#define AEE_NETWORK_MBMS                 (0x100)            // Multimedia Broadcast Multicast Service
#define AEE_NETWORK_IWLAN_3GPP(pdp)      (0x200|(pdp)<<16)  // 3G (UMTS) via IWLAN
#define AEE_NETWORK_IWLAN_3GPP2(profile) (0x400|(profile)<<16) // 3G (CDMA) via WLAN
#define AEE_NETWORK_IWLAN_3GPP2_SN       (0x400)            // 3G (CDMA) via WLAN
#define AEE_NETWORK_SLIP                 (0x800)            // Serial Line IP
#define AEE_NETWORK_3GPP_ANY(pdp)        (0x1000|(pdp)<<16)  // 3G via WLAN or directly via 3GPP (UMTS) network
#define AEE_NETWORK_3GPP2_ANY(profile)   (0x2000|(profile)<<16) // 3G via WLAN or directly via 3GPP2 (CDMA) network
#define AEE_NETWORK_3GPP2_SN_ANY         (0x2000)            // 3G via WLAN or directly via 3GPP2 (CDMA) network
#define AEE_NETWORK_UICC                 (0x4000)            // Universal Integrated Circuit Card 
#define AEE_NETWORK_UW_FMC               (0x8000)            // FMC Via WLAN Network
//
// for INetwork Get/SetOpt(INET_OPT_*_RLP3)
//
typedef struct AEERLP3Cfg
{
   byte ucFwdNakRounds;              /* num forward NAK rounds (3 max) */
   byte aucFwdNaksPerRound[3];       /* NAKs per round, forward */
   byte ucRevNakRounds;              /* num reverse NAK rounds (3 max)*/
   byte aucRevNaksPerRound[3];       /* NAKs per round, reverse */
} AEERLP3Cfg;

//
// for INetwork_GetOpt(INET_OPT_UDP_URGENT)
//
typedef struct AEEUDPUrgent
{
   boolean bUrgentSupported;
   uint16  nUrgentLimit;
} AEEUDPUrgent;


//
// Network performance stats.
//
typedef struct
{
   // current session fields...

   uint32   dwOpenTime;       // seconds since data network connection established
   uint32   dwActiveTime;     // seconds the data network connection was actually
                              //   "active", defined as "with open sockets"
   uint32   dwBytes;          // bytes sent on current data network connection
   uint32   dwRate;           // rate of transfer on current data network connection,
                              //   bytes/sec while "active" (dwBytes/dwActiveTime)

   // accumulators, averages

   uint32   dwTotalOpenTime;  // seconds for all open data network sessions
   uint32   dwTotalActiveTime;// seconds for all active data network sessions
   uint32   dwTotalBytes;     // bytes sent (all connections)
   uint32   dwTotalRate;      // average xfer rate (dwTotalBytes/dwTotalActiveTime)
} AEENetStats;


// Network down reasons
#define AEE_NET_DOWN_REASON_NOT_SPECIFIED       0
#define AEE_NET_DOWN_REASON_UMTS_REATTACH_REQ   1
#define AEE_NET_DOWN_REASON_CLOSE_IN_PROGRESS   2
#define AEE_NET_DOWN_REASON_NW_INITIATED_TERMINATION  3
#define AEE_NET_DOWN_REASON_INSUFFICIENT_RESOURCES    4
#define AEE_NET_DOWN_REASON_UNKNOWN_APN         5
#define AEE_NET_DOWN_REASON_UNKNOWN_PDP         6
#define AEE_NET_DOWN_REASON_AUTH_FAILED         7
#define AEE_NET_DOWN_REASON_GGSN_REJECT         8
#define AEE_NET_DOWN_REASON_ACTIVATION_REJECT   9
#define AEE_NET_DOWN_REASON_OPTION_NOT_SUPPORTED     10
#define AEE_NET_DOWN_REASON_OPTION_UNSUBSCRIBED 11
#define AEE_NET_DOWN_REASON_OPTION_TEMP_OOO     12
#define AEE_NET_DOWN_REASON_NSAPI_ALREADY_USED  13
#define AEE_NET_DOWN_REASON_REGULAR_DEACTIVATION     14
#define AEE_NET_DOWN_REASON_QOS_NOT_ACCEPTED    15
#define AEE_NET_DOWN_REASON_NETWORK_FAILURE     16
#define AEE_NET_DOWN_REASON_TFT_SEMANTIC_ERROR  17
#define AEE_NET_DOWN_REASON_TFT_SYNTAX_ERROR    18
#define AEE_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT 19
#define AEE_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR     20
#define AEE_NET_DOWN_REASON_FILTER_SYNTAX_ERROR 21
#define AEE_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT    22
#define AEE_NET_DOWN_REASON_INVALID_TRANSACTION_ID    23
#define AEE_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC   24
#define AEE_NET_DOWN_REASON_INVALID_MANDATORY_INFO    25
#define AEE_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED  26
#define AEE_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE 27
#define AEE_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT      28
#define AEE_NET_DOWN_REASON_CONDITIONAL_IE_ERROR      29
#define AEE_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE 30
#define AEE_NET_DOWN_REASON_PROTOCOL_ERROR      31
#define AEE_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE            32
#define AEE_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING   0x0106218a
#define AEE_NET_DOWN_REASON_LLC_SNDCP_FAILURE    0x0106218c
#define AEE_NET_DOWN_REASON_APN_TYPE_CONFLICT    0x0106218e
#define AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_UNABLE_TO_PROCESS 0x0107446b   
#define AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_UNSUPPORTED_VERS 0x0107446c 
#define AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_INSUFFICIENT_CHALLANGES 0x0107446e 
#define AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_RAND_NOT_FRESH 0x0107446f 
#define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_AFTER_AUTH 0x01074470 
#define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_BEFORE_AUTH 0x01074471  
#define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_TEMP_DENIED_ACCESS 0x01074472
#define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NOT_SUBSCRIBED 0x01074473  
#define AEE_NET_DOWN_REASON_EAP_SUCCESS 0x01075af2
#define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_REALM_UNAVAILABLE 0x01075af3
#define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NAME_UNAVAILABLE 0x01075af4
#define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_CALL_BARRED 0x01075af5
#define AEE_NET_DOWN_REASON_IPSEC_GW_UNREACHABLE 0x01074476 
#define AEE_NET_DOWN_REASON_IPSEC_AUTH_FAILED 0x01074477 
#define AEE_NET_DOWN_REASON_IPSEC_CERT_INVALID 0x01074478 
#define AEE_NET_DOWN_REASON_IPSEC_INTERNAL_ERROR 0x01074479  

// Network down reasons which are generated because the call is terminated by the mobile
#define AEE_NET_DOWN_REASON_INTERNAL_MIN                  1000
#define AEE_NET_DOWN_REASON_INTERNAL_ERROR                1001
#define AEE_NET_DOWN_REASON_INTERNAL_CALL_ENDED           1002
#define AEE_NET_DOWN_REASON_INTERNAL_UNKNOWN_CAUSE_CODE   1003
#define AEE_NET_DOWN_REASON_INTERNAL_MAX                  1999

typedef int AEENetDownReason;

// Domain name
#define NETWORK_MAX_DOMAIN_NAME_SIZE    256

typedef struct
{
   char domain_name[NETWORK_MAX_DOMAIN_NAME_SIZE];
} AEENetDomainName;

//
// Network types definitions, for IOCTL_GET_BEARER_TECH
//

// Primary bearer technology
#define AEE_BEARER_TECH_CDMA 1
#define AEE_BEARER_TECH_UMTS 2
#define AEE_BEARER_TECH_WLAN 3
typedef uint32 AEEBearerTech;

// CDMA sub-technologies. Masks are being used, since the phone
// may simultaneously support more than one technology.
#define AEE_CDMA_BEARER_TECH_1X              0x01
#define AEE_CDMA_BEARER_TECH_EVDO_REV0       0x02
#define AEE_CDMA_BEARER_TECH_EVDO_REVA       0x04
typedef uint32 AEECDMABearerTech;

// struct holding information regarding a CDMA network.
typedef struct AEECDMABearerTechInfo
{
  AEECDMABearerTech  uCDMABearerTech;
} AEECDMABearerTechInfo;

// UMTS sub-technologies. Masks are being used, since the phone
// may simultaneously support more than one technology.
#define AEE_UMTS_BEARER_TECH_WCDMA         0x01
#define AEE_UMTS_BEARER_TECH_GPRS          0x02
#define AEE_UMTS_BEARER_TECH_HSDPA         0x04
#define AEE_UMTS_BEARER_TECH_HSUPA         0x08
#define AEE_UMTS_BEARER_TECH_EDGE          0x10
typedef uint32 AEEUTMSBearerTech;

// struct holding information regarding a UMTS network.
typedef struct AEEUMTSBearerTechInfo
{
  AEEUTMSBearerTech  uUMTSBearerTech;
} AEEUMTSBearerTechInfo;

// struct holding the complete bearer technology information
typedef struct AEEBearerTechType
{
   AEEBearerTech uBearerTech;
   union
   {
      AEECDMABearerTechInfo acbCDMABearerTechInfo;
      AEEUMTSBearerTechInfo aubUMTSBearerTechInfo;
   } uBearerTechInfo;

} AEEBearerTechType;

//
// Outage event definitions
//
#define AEE_NET_OUTAGE_INVALID     0
#define AEE_NET_OUTAGE_VALID       1
#define AEE_NET_OUTAGE_STARTED     2
#define AEE_NET_OUTAGE_EXPIRED     3
typedef int AEEOutageStatus;

// struct holding information regarding outage events
typedef struct AEEOutageInfo {
  AEEOutageStatus nStatus;
  int             nTimeToOutage;
  uint32          uDuration;
} AEEOutageInfo;

//
// IPv6 Prefix definitions
//

// IPv6 prefix states
#define AEE_IPV6_PREFIX_NOT_EXIST   0
#define AEE_IPV6_PREFIX_TENTATIVE   1
#define AEE_IPV6_PREFIX_VALID       2
#define AEE_IPV6_PREFIX_DEPRECATED  3
typedef uint16 AEEIPv6PrefixState;

// struct holding information regarding an IPv6 Prefix
typedef struct AEEIPv6PrefixInfo {
   AEEIPv6PrefixState   uState;
   uint16               uPrefixLen;
} AEEIPv6PrefixInfo;

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

=======================================================================

AEENetDownReason

Description:
   This data type is an enumeration of supported Network Down Reasons.

Definition:
   #define AEE_NET_DOWN_REASON_NOT_SPECIFIED       0
   #define AEE_NET_DOWN_REASON_UMTS_REATTACH_REQ   1
   #define AEE_NET_DOWN_REASON_CLOSE_IN_PROGRESS   2
   #define AEE_NET_DOWN_REASON_NW_INITIATED_TERMINATION  3
   #define AEE_NET_DOWN_REASON_INSUFFICIENT_RESOURCES    4
   #define AEE_NET_DOWN_REASON_UNKNOWN_APN         5
   #define AEE_NET_DOWN_REASON_UNKNOWN_PDP         6
   #define AEE_NET_DOWN_REASON_AUTH_FAILED         7
   #define AEE_NET_DOWN_REASON_GGSN_REJECT         8
   #define AEE_NET_DOWN_REASON_ACTIVATION_REJECT   9
   #define AEE_NET_DOWN_REASON_OPTION_NOT_SUPPORTED     10
   #define AEE_NET_DOWN_REASON_OPTION_UNSUBSCRIBED 11
   #define AEE_NET_DOWN_REASON_OPTION_TEMP_OOO     12
   #define AEE_NET_DOWN_REASON_NSAPI_ALREADY_USED  13
   #define AEE_NET_DOWN_REASON_REGULAR_DEACTIVATION     14
   #define AEE_NET_DOWN_REASON_QOS_NOT_ACCEPTED    15
   #define AEE_NET_DOWN_REASON_NETWORK_FAILURE     16
   #define AEE_NET_DOWN_REASON_TFT_SEMANTIC_ERROR  17
   #define AEE_NET_DOWN_REASON_TFT_SYNTAX_ERROR    18
   #define AEE_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT 19
   #define AEE_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR     20
   #define AEE_NET_DOWN_REASON_FILTER_SYNTAX_ERROR 21
   #define AEE_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT    22
   #define AEE_NET_DOWN_REASON_INVALID_TRANSACTION_ID    23
   #define AEE_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC   24
   #define AEE_NET_DOWN_REASON_INVALID_MANDATORY_INFO    25
   #define AEE_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED  26
   #define AEE_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE 27
   #define AEE_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT      28
   #define AEE_NET_DOWN_REASON_CONDITIONAL_IE_ERROR      29
   #define AEE_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE 30
   #define AEE_NET_DOWN_REASON_PROTOCOL_ERROR      31
   #define AEE_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE            32

   #define AEE_NET_DOWN_REASON_INTERNAL_MIN                  1000
   #define AEE_NET_DOWN_REASON_INTERNAL_ERROR                1001
   #define AEE_NET_DOWN_REASON_INTERNAL_CALL_ENDED           1002
   #define AEE_NET_DOWN_REASON_INTERNAL_UNKNOWN_CAUSE_CODE   1003
   #define AEE_NET_DOWN_REASON_INTERNAL_MAX                  1999
   #define AEE_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING   0x0106218a
   #define AEE_NET_DOWN_REASON_LLC_SNDCP_FAILURE             0x0106218c
   #define AEE_NET_DOWN_REASON_APN_TYPE_CONFLICT             0x0106218e
   #define AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_UNABLE_TO_PROCESS 0x0107446b   
   #define AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_UNSUPPORTED_VERS 0x0107446c 
   #define AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_INSUFFICIENT_CHALLANGES 0x0107446e 
   #define AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_RAND_NOT_FRESH 0x0107446f 
   #define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_AFTER_AUTH 0x01074470 
   #define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_BEFORE_AUTH 0x01074471  
   #define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_TEMP_DENIED_ACCESS 0x01074472
   #define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NOT_SUBSCRIBED 0x01074473  
   #define AEE_NET_DOWN_REASON_EAP_SUCCESS 0x01075af2
   #define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_REALM_UNAVAILABLE 0x01075af3
   #define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NAME_UNAVAILABLE 0x01075af4
   #define AEE_NET_DOWN_REASON_EAP_NOTIFICATION_CALL_BARRED 0x01075af5
   #define AEE_NET_DOWN_REASON_IPSEC_GW_UNREACHABLE 0x01074476 
   #define AEE_NET_DOWN_REASON_IPSEC_AUTH_FAILED 0x01074477 
   #define AEE_NET_DOWN_REASON_IPSEC_CERT_INVALID 0x01074478 
   #define AEE_NET_DOWN_REASON_IPSEC_INTERNAL_ERROR 0x01074479 

   ** Additional down reasons can be used, depending on class implementation. 
      For example see AEENetwork.bid **

   typedef int AEENetDownReason;

Members:
   AEE_NET_DOWN_REASON_NOT_SPECIFIED: The network was closed for an un specified reason.
   e.g. the network was closed successfully by the mobile.
   Introduced BREW Client 3.1.4 SP1.

   AEE_NET_DOWN_REASON_UMTS_REATTACH_REQ: This reason indicates a network
   request for a PDP context reactivation after a GGSN restart.
   Introduced BREW Client 3.1.4 SP1.

   AEE_NET_DOWN_REASON_CLOSE_IN_PROGRESS: This reason indicates that the network
   connection request was rejected as the previous connection of the same network
   is currently being closed.
   Introduced BREW Client 3.1.4 SP1.

   AEE_NET_DOWN_REASON_NW_INITIATED_TERMINATION: This reason indicates that
   the session was terminated by the network.
   Introduced BREW Client 3.1.4 SP1.

   AEE_NET_DOWN_REASON_INSUFFICIENT_RESOURCES: This reason indicates that
   PDP context activation request, secondary PDP context activation request or
   PDP context modification request cannot be accepted due to insufficient resources.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_UNKNOWN_APN: This reason indicates that the requested
   service was rejected by the external packet data network because the access
   point name was not included although required or if the access point name
   could not be resolved.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_UNKNOWN_PDP: This reason indicates that the requested
   service was rejected by the external packet data network because the PDP
   address or type could not be recognized.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_AUTH_FAILED: This reason indicates that the requested
   service was rejected by the external packet data network due to a failed
   user authentication.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_GGSN_REJECT: This reason indicates that GGSN has
   rejected the activation request.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_ACTIVATION_REJECT: This reason indicates that the
   activation request rejected with unspecified reason.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_OPTION_NOT_SUPPORTED: This reason indicates that the
   request service option is not supported by the PLMN.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_OPTION_UNSUBSCRIBED: This reason indicates that the
   requested service option is not subscribed for.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_OPTION_TEMP_OOO: This reason indicates that the MCS
   cannot service the request because of temporary outage of one or more
   functions required for supporting the service.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_NSAPI_ALREADY_USED: This reason indicates that the NSAPI
   requested by the MS in the PDP context activation request is already used by
   another active PDP context of this MS.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_REGULAR_DEACTIVATION: This reason indicates that the
   regular MS or network initiated PDP context deactivation.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_QOS_NOT_ACCEPTED: This reason indicates that the new
   QoS cannot be accepted by the UE that were indicated by the network in the
   PDP Context Modification procedure.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_NETWORK_FAILURE: This reason indicates that the PDP
   context deactivation is caused by an error situation in the network.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_TFT_SEMANTIC_ERROR: This reason indicates that there is
   a semantic error in the TFT operation included in a secondary PDP context
   activation request or an MS-initiated PDP context modification.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_TFT_SYNTAX_ERROR: This reason indicates that there is
   syntactical error in the TFT operation included in a secondary PDP context
   activation request or an MS-initiated PDP context modification.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT: This reason indicates that the
   PDP context identified by the Linked TI IE the secondary PDP context
   activation request is not active.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR: This reason indicates that there
   is one or more semantic errors in packet filter(s) of the TFT included in a
   secondary PDP context activation request or an MS-initiated PDP context
   modification.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_FILTER_SYNTAX_ERROR: This reason indicates that there
   is one or more syntactical errors in packet filter(s) of the TFT included
   in a secondary PDP context activation request or an MS-initiated PDP context
   modification.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT: This reason indicates that the
   network has already activated a PDP context without TFT.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_INVALID_TRANSACTION_ID: This reason indicates that the
   equipment sending this cause has received a message with a transaction
   identifier which is not currently in use on the MS-network interface.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC: This reason indicates that
   the message is semantically incorrect.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_INVALID_MANDATORY_INFO: This reason indicates that
   mandatory information is invalid.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED: This reason indicates that the
   message type is non-existent or is not supported.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE: This reason indicates that
   the message type is not compatible with the protocol state.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT: This reason indicates that the
   information element is non-existent or is not implemented.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_CONDITIONAL_IE_ERROR: This reason indicates that a
   conditional IE Error occurred.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE: This reason
   indicates that the message is not compatible with the current protocol state.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_PROTOCOL_ERROR: This reason is used to report a protocol
   error event only when no other cause in the protocol error class applies.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE: This reason indicates that the call
   is terminated but the cause is not mapped to a network down reason yet.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING: This reason indicates
   that the operator determined barring.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_LLC_SNDCP_FAILURE: This reason indicates that there was an LLC
   SNDCP failure.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_APN_TYPE_CONFLICT: This reason indicates that there was an APN
   type conflict.
   Introduced BREW Client 3.1.5 SP01.

   // The following network down reasons are generated when the call is terminated by the mobile

   AEE_NET_DOWN_REASON_INTERNAL_MIN: Should not be used.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_INTERNAL_ERROR: This reason indicates that the call is
   terminated because of some internal error.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_INTERNAL_CALL_ENDED: This reason indicated that the call
   is terminated locally by the mobile.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_INTERNAL_UNKNOWN_CAUSE_CODE: This reason indicated that the
   call is terminated locally by the mobile but the cause is not mapped to a
   network down reason yet.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_INTERNAL_MAX: Should not be used.
   Introduced BREW Client 3.1.5 SP01.

   AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_UNABLE_TO_PROCESS:This reason indicates that 
   Extensible Authentication Protocol client unable to process error occurred.   
   Introduced BREW Client 3.1.5 SP02.

   AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_UNSUPPORTED_VERS:This reason indicates that
   Extensible Authentication Protocol client unsupported versions error occurred.   
   Introduced BREW Client 3.1.5 SP02. 

   AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_INSUFFICIENT_CHALLANGES:This reason indicates that
   Extensible Authentication Protocol client insufficient challenges error occurred.   
   Introduced BREW Client 3.1.5 SP02. 

   AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_RAND_NOT_FRESH:This reason indicates that
   Extensible Authentication Protocol client rand not fresh error occurred.   
   Introduced BREW Client 3.1.5 SP02. 

   AEE_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_AFTER_AUTH:This reason indicates that
   Extensible Authentication Protocol notification general failure error occurred.   
   Introduced BREW Client 3.1.5 SP02. 

   AEE_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_BEFORE_AUTH:This reason indicates that
   Extensible Authentication Protocol notification general failure error occurred.   
   Introduced BREW Client 3.1.5 SP02.   

   AEE_NET_DOWN_REASON_EAP_NOTIFICATION_TEMP_DENIED_ACCESS:This reason indicates that
   Extensible Authentication Protocol notification temporary denied access error occurred.   
   Introduced BREW Client 3.1.5 SP02. 

   AEE_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NOT_SUBSCRIBED:This reason indicates that
   Extensible Authentication Protocol notification user not subscribed error occurred.   
   Introduced BREW Client 3.1.5 SP02.   

   AEE_NET_DOWN_REASON_EAP_SUCCESS: Extensible Authentication Protocol success.
   Introduced BREW Client 3.1.5 SP02.

   AEE_NET_DOWN_REASON_EAP_NOTIFICATION_REALM_UNAVAILABLE:Extensible Authentication Protocol- 
   notification realm unavailable.
   Introduced BREW Client 3.1.5 SP02.

   AEE_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NAME_UNAVAILABLE:Extensible Authentication Protocol-
   notification user name unavailable.
   Introduced BREW Client 3.1.5 SP02.

   AEE_NET_DOWN_REASON_EAP_NOTIFICATION_CALL_BARRED:Extensible Authentication Protocol-
   notification call barred.
   Introduced BREW Client 3.1.5 SP02.

   AEE_NET_DOWN_REASON_IPSEC_GW_UNREACHABLE:This reason indicates that IPSEC gateway
   unreachable error occurred.
   Introduced BREW Client 3.1.5 SP02. 

   AEE_NET_DOWN_REASON_IPSEC_AUTH_FAILED:This reason indicates that IPSEC authentication
   failed error occurred.
   Introduced BREW Client 3.1.5 SP02.  

   AEE_NET_DOWN_REASON_IPSEC_CERT_INVALID:This reason indicates that IPSEC cert invalid
   error occurred.
   Introduced BREW Client 3.1.5 SP02.  

   AEE_NET_DOWN_REASON_IPSEC_INTERNAL_ERROR:This reason indicates that IPSEC internal
   error occurred.
   Introduced BREW Client 3.1.5 SP02. 

Comments:
   Note that a class implementing this interface may define 
   additional network down reasons.


Version:
   First set of net down reasons introduced in BREW Client 3.1.4 SP1.
   Additional reasons were added in BREW 3.1.5 SP01 and SP01 as detailed
   above for each type of reason.

See Also:
   INetwork_GetLastNetDownReason()

=======================================================================

Data networks

Description:
   Definitions of the known data networks. These definitions are used to
   select the network in operations such as INetwork_SelectNetwork().

Definition:
#define AEE_NETWORK_DEFAULT              (0x000)
#define AEE_NETWORK_CDMA_SN              (0x001)
#define AEE_NETWORK_CDMA(profile)        (0x01|((uint32)(profile)<<16))
#define AEE_NETWORK_CDMA_BCMCS           (0x002)
#define AEE_NETWORK_UMTS(pdp)            (0x008|(pdp)<<16)
#define AEE_NETWORK_WLAN                 (0x010)
#define AEE_NETWORK_MFLO_MCAST           (0x020)
#define AEE_NETWORK_DVBH_MCAST           (0x040)
#define AEE_NETWORK_LOOPBACK             (0x080)
#define AEE_NETWORK_MBMS                 (0x100)
#define AEE_NETWORK_IWLAN_3GPP(pdp)      (0x200|(pdp)<<16)  // 3G (UMTS) via IWLAN
#define AEE_NETWORK_IWLAN_3GPP2(profile) (0x400|(profile)<<16) // 3G (CDMA) via WLAN
#define AEE_NETWORK_IWLAN_3GPP2_SN       (0x400)            // 3G via WLAN or directly via 3GPP2 (CDMA) network
#define AEE_NETWORK_SLIP                 (0x800)            // Serial Line IP
#define AEE_NETWORK_3GPP_ANY(pdp)        (0x1000|(pdp)<<16)  // 3G via WLAN or directly via 3GPP (UMTS) network
#define AEE_NETWORK_3GPP2_ANY(profile)   (0x2000|(profile)<<16) // 3G via WLAN or directly via 3GPP2 (CDMA) network
#define AEE_NETWORK_3GPP2_SN_ANY         (0x2000)            // 3G via WLAN or directly via 3GPP2 (CDMA) network
#define AEE_NETWORK_UICC                 (0x4000)            // Universal Integrated Circuit Card 
#define AEE_NETWORK_UW_FMC               (0x8000)            // FMC Via WLAN Network

Members:
===pre>
   AEE_NETWORK_DEFAULT:         Default network. The decision of which network to
                                actually use is deferred to lower layers and will
                                be made upon connection creation.
   AEE_NETWORK_CDMA_SN:         CDMA Service Network
   AEE_NETWORK_CDMA(profile):   CDMA Service Networks, applications shouldn't use
                                profile IDs unless they are carrier-provisioned apps.
   AEE_NETWORK_CDMA_BCMCS:      CDMA BroadCast/MultiCast Service
   AEE_NETWORK_UMTS(pdp):       UMTS network identified by a Packet Data
                                Profile (pdp) number.
   AEE_NETWORK_WLAN:            WLAN Network (Introduced BREW Client 3.1.3)
   AEE_NETWORK_MFLO_MCAST:      MediaFLO Multicast Service
                                (Introduced BREW Client 3.1.4)
   AEE_NETWORK_DVBH_MCAST:      DVB-H Multicast Service 
                                (Introduced BREW Client 3.1.5 SP01)
   AEE_NETWORK_LOOPBACK:        Loopback. Transferring data over loopback sockets
                                does not involve data call bring up.
                                (Introduced BREW Client 3.1.5 SP01)
   AEE_NETWORK_MBMS:            Multimedia Broadcast Multicast Service
   AEE_NETWORK_IWLAN_3GPP(pdp): UMTS Network, identified by a Packet Data
                                Profile (pdp) number, for access via WLAN or 3G bearer.
                                Handset configuration determines if network access
                                is attempted via WLAN or 3G.
   AEE_NETWORK_IWLAN_3GPP2(profile): CDMA Network, identified by Profile number, for
                                     access via WLAN.
                                     Applications shouldn't use profile IDs unless they
                                     are carrier-provisioned apps.
   AEE_NETWORK_IWLAN_3GPP2_SN:  CDMA Network (default) for access via WLAN.
   AEE_NETWORK_SLIP:            Serial Line IP (Introduced BREW Client 3.1.5 SP02)
   AEE_NETWORK_3GPP_ANY(pdp): UMTS Network, identified by a Packet Data
                              Profile (pdp) number, for access via WLAN or 3G bearer.
                              Handset configuration determines if network access
                              is attempted via WLAN or 3G.
   AEE_NETWORK_3GPP2_ANY(profile): CDMA Network, identified by Profile number, for
                                   access via WLAN or 3G bearer. 
                                   Handset configuration determines if network access
                                   is attempted via WLAN or 3G.
                                   Applications shouldn't use profile IDs unless they
                                   are carrier-provisioned apps.
   AEE_NETWORK_3GPP2_SN_ANY: CDMA Network (default) for access via WLAN or 3G bearer.
                             Handset configuration determines if network access
                             is attempted via WLAN or 3G.
   AEE_NETWORK_UICC: Universal Integrated Circuit Card Network. Used by UMTS devices to 
                     communicate with the UICC (sim).
   AEE_NETWORK_UW_FMC:  A proprietary solution for access to CDMA packet data 
                        through WLAN connectivity.
===/pre>

Comments:
   Most of the data network types may be configured on the phone as the default 
   network type. In such case it shall be automatically selected if the application 
   has not made any other specific network selection. 
   In order to select a specific network for data communication (CDMA, UMTS, WLAN, 
   etc.) the application should call SelectNetwork API of the desired interface 
   (ISockPort, INetwork, etc). For IWeb interface, network selection is done through 
   IWEB_GetResponse(), using WEBOPT_NETWORK Web Option followed by the relevant 
   AEE_NETWORK_ data network type. 

See Also:
   INetwork_SelectNetworkEx()
   INetUtils_GetDefaultNetwork()

=======================================================================

AEENetDomainName

Description:
   This data type holds a domain name.

Definition:
   #define NETWORK_MAX_DOMAIN_NAME_SIZE    256

   typedef struct
   {
     char domain_name[NETWORK_MAX_DOMAIN_NAME_SIZE];
   } AEENetDomainName;

Members:
   domain_name: char array to hold the domain name string

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   INetwork_GetSipServerDomainNames()

=======================================================================

AEEBearerTech

*** This is not supported in BREW version 4.0

Description:
   This data type defines the supported primary bearer technologies

Definition:
   typedef uint32 AEEBearerTech;

Members:
   AEE_BEARER_TECH_CDMA: bearer technology is CDMA
   AEE_BEARER_TECH_UMTS: bearer technology is UMTS
   AEE_BEARER_TECH_WLAN: bearer technology is WLAN

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   AEECDMABearerTech
   AEEUTMSBearerTech
   AEECDMABearerTechInfo
   AEEUMTSBearerTechInfo
   AEEBearerTechType
   INetwork_GetBearerTechnology()

=======================================================================

AEECDMABearerTech

*** This is not supported in BREW version 4.0

Description:
   This data type denotes the supported CDMA sub-technologies.
   Masks are being used, since the phone
   may simultaneously support more than one technology.

Definition:
   typedef uint32 AEECDMABearerTech;

Members:
   AEE_CDMA_BEARER_TECH_1X:         CDMA sub-technology is 1x
   AEE_CDMA_BEARER_TECH_EVDO_REV0:  CDMA sub-technology is EVDO Rev 0
   AEE_CDMA_BEARER_TECH_EVDO_REVA:  CDMA sub-technology is EVDO Rev A
Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   AEEBearerTech
   AEEUTMSBearerTech
   AEECDMABearerTechInfo
   AEEUMTSBearerTechInfo
   AEEBearerTechType
   INetwork_GetBearerTechnology()

=======================================================================

AEEUTMSBearerTech

*** This is not supported in BREW version 4.0

Description:
   This data type denotes the supported UMTS sub-technologies.
   Masks are being used, since the phone
   may simultaneously support more than one technology.

Definition:
   typedef uint32 AEEUTMSBearerTech;

Members:
   AEE_UMTS_BEARER_TECH_WCDMA:   UMTS sub-technology is WCDMA
   AEE_UMTS_BEARER_TECH_GPRS:    UMTS sub-technology is GPRS
   AEE_UMTS_BEARER_TECH_HSDPA:   UMTS sub-technology is HSDPA
   AEE_UMTS_BEARER_TECH_HSUPA:   UMTS sub-technology is HSUPA
   AEE_UMTS_BEARER_TECH_EDGE:    UMTS sub-technology is EDGE

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   AEEBearerTech
   AEECDMABearerTech
   AEECDMABearerTechInfo
   AEEUMTSBearerTechInfo
   AEEBearerTechType
   INetwork_GetBearerTechnology()

=======================================================================

AEECDMABearerTechInfo

*** This is not supported in BREW version 4.0

Description:
   This data type denotes the CDMA related information.

Definition:
   typedef struct AEECDMABearerTechInfo
   {
      AEECDMABearerTech  uCDMABearerTech;
   } AEECDMABearerTechInfo;

Members:
   uCDMABearerTech:  The CDMA sub-technology

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   AEEBearerTech
   AEECDMABearerTech
   AEEUMTSBearerTech
   AEEUMTSBearerTechInfo
   AEEBearerTechType
   INetwork_GetBearerTechnology()

=======================================================================

AEEUMTSBearerTechInfo

*** This is not supported in BREW version 4.0

Description:
   This data type denotes the UMTS related information.

   typedef struct AEEUMTSBearerTechInfo
   {
      AEEUTMSBearerTech  uUMTSBearerTech;
   } AEEUMTSBearerTechInfo;

Members:
   uUMTSBearerTech:  The UMTS sub-technology

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   AEEBearerTech
   AEECDMABearerTech
   AEEUMTSBearerTech
   AEECDMABearerTechInfo
   AEEBearerTechType
   INetwork_GetBearerTechnology()

=======================================================================

AEEBearerTechType

*** This is not supported in BREW version 4.0

Description:
   This data type holds the complete bearer technology information.

   typedef struct AEEBearerTechType
   {
      AEEBearerTech uBearerTech;
      union
      {
         AEECDMABearerTechInfo acbCDMABearerTechInfo;
         AEEUMTSBearerTechInfo aubUMTSBearerTechInfo;
      } uBearerTechInfo;

   } AEEBearerTechType;

Members:
   uBearerTech:      The primary bearer technology
   uBearerTechInfo:  Additional information regarding the bearer technology

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   AEEBearerTech
   AEECDMABearerTech
   AEEUMTSBearerTech
   AEECDMABearerTechInfo
   AEEUMTSBearerTechInfo
   INetwork_GetBearerTechnology()

=======================================================================

AEEOutageStatus

*** This is not supported in BREW version 4.0

Description:
   This data type defines the various statuses associated with an
   outage info struct.

Definition:
   typedef int AEEOutageStatus;

Members:
   AEE_NET_OUTAGE_VALID:    Outage information is valid.
   AEE_NET_OUTAGE_INVALID:  Outage information is invalid.
   AEE_NET_OUTAGE_STARTED:  Outage already started.
   AEE_NET_OUTAGE_EXPIRED:  Outage expired.

Comments:
   Note that in future versions, additional states may be added.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   AEEOutageInfo
   INetwork_GetOutageEvtInfo()

=======================================================================

AEEOutageInfo

*** This is not supported in BREW version 4.0

Description:
   This data type holds the information regarding an outage.

   typedef struct AEEOutageInfo {
      AEEOutageStatus nStatus;
      int             nTimeToOutage;
      uint32          uDuration;
   } AEEOutageInfo;

Members:
   nStatus:           The status of the outage.
   nTimeToOutage:     Time, in milliseconds, to the beginning of the
                      outage.
                      This value might be negative if outage already
                      started or has expired.
   uDuration:         Duration, in milliseconds, of the outage.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   AEEOutageStatus
   INetwork_GetOutageEvtInfo()

=======================================================================

AEEIPv6PrefixState

*** This is not supported in BREW version 4.0

Description:
   This data type holds the possible states of an IPv6 prefix.

Definition:
   typedef uint16 AEEIPv6PrefixState;

Members:
   AEE_IPV6_PREFIX_NOT_EXIST:    The prefix does not exist.
   AEE_IPV6_PREFIX_TENTATIVE:    The prefix is pending for DAD
                                 (Duplicate Address Detection) verification.
                                 For more details about DAD, see RFC 2462.
   AEE_IPV6_PREFIX_VALID:        The prefix is valid.
   AEE_IPV6_PREFIX_DEPRECATED:   The prefix has been deprecated.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   AEEIPv6PrefixInfo

=======================================================================

AEEIPv6PrefixInfo

*** This is not supported in BREW version 4.0

Description:
   This data type holds information regarding an IPv6 Prefix.

Definition:
   typedef struct AEEIPv6PrefixInfo {
      AEEIPv6PrefixState   uState;
      uint16               uPrefixLen;
   } AEEIPv6PrefixInfo;

Members:
   uState: The state of the prefix.
   uPrefixLen: The length of the prefix.

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   AEEIPv6PrefixState
   INetwork_GetIPv6PrefixInfo()
=======================================================================*/


#endif  // AEENETTYPES_H
