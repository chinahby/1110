#ifndef AEEIQOSFLOW_H
#define AEEIQOSFLOW_H
/*===========================================================================

FILE: AEEIQoSFlow.h

SERVICES: AEE Quality of Service Flow definitions


DESCRIPTION: Base level definitions, typedefs, etc. for AEE QoS Flow

   Copyright (c) 2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
===========================================================================*/

#include "AEEIQI.h"
#include "AEEIxOpts.h"

#define AEEIID_IQoSFlow    0x0102b6cb

//**********************************************************************
//
// IQoSFlow Interface
//
//**********************************************************************
typedef xOpt AEEQoSFlowOpt;


//
// IP Traffic Class
//
typedef uint32 AEEQOSFLOW_IPTrafficClass;

// IP Traffic Class values:
#define  AEEQOSFLOW_TRAFFIC_CLASS_CONVERSATIONAL   0
#define  AEEQOSFLOW_TRAFFIC_CLASS_STREAMING        1
#define  AEEQOSFLOW_TRAFFIC_CLASS_INTERACTIVE      2
#define  AEEQOSFLOW_TRAFFIC_CLASS_BACKGROUND       3

//
// Latency
//
typedef uint32 AEEQOSFLOW_Latency;

//
// Latency Variance
//
typedef uint32 AEEQOSFLOW_LatencyVariance;

//
// Min Policed Packet Size , not applicable to UMTS
//
typedef uint32 AEEQOSFLOW_MinPolicedPacketSize;

//
// Max Allowed Packet Size
//
typedef uint32 AEEQOSFLOW_MaxAllowedPacketSize;

//
// UMTS Residual Bit Error Rate , only applicable to UMTS
//
typedef uint32 AEEQOSFLOW_UMTSResidualBitErrorRate;

// UMTS Residual Bit Error Rate values:
#define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE1      1        // 5*10-2
#define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE2      2        // 1*10-2
#define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE3      3        // 5*10-3
#define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE4      4        // 4*10-3
#define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE5      5        // 1*10-3
#define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE6      6        // 1*10-4
#define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE7      7        // 1*10-5
#define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE8      8        // 1*10-6
#define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE9      9        // 6*10-8
#define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE10     10       // Reserved

//
// UMTS Traffic Priority , only applicable to UMTS
//
typedef uint32 AEEQOSFLOW_UMTSTrafficPriority;

// UMTS Traffic Priority values:
#define AEEQOSFLOW_UMTS_TRAFFIC_HANDLING_PRI1        1      // Priority Level 1
#define AEEQOSFLOW_UMTS_TRAFFIC_HANDLING_PRI2        2      // Priority Level 2
#define AEEQOSFLOW_UMTS_TRAFFIC_HANDLING_PRI3        3      // Priority Level 3

//
// CDMA Profile Id , only applicable to CDMA
//
typedef uint32 AEEQOSFLOW_CDMAProfileId;

// CDMA Profile Id values (obsolete):
#define AEEQOSFLOW_CDMA_QOS_PROFILE_1        0x01       // Profile 1

//
// WLAN User Priority, only applicable to WLAN
//
typedef uint32 AEEQOSFLOW_WLANUserPriority;

// WLAN User Priority values:
#define AEEQOSFLOW_WLAN_USER_PRI_BEST_EFFORT        0 // Bes Effort
#define AEEQOSFLOW_WLAN_USER_PRI_BACKGROUND         1 // Background
#define AEEQOSFLOW_WLAN_USER_PRI_RESERVED           2 // Spare
#define AEEQOSFLOW_WLAN_USER_PRI_EXCELLENT_EFFORT   3 // Excellent Effort
#define AEEQOSFLOW_WLAN_USER_PRI_CONTROLLED_LOAD    4 // Controlled Load
#define AEEQOSFLOW_WLAN_USER_PRI_VIDEO              5 // Video
#define AEEQOSFLOW_WLAN_USER_PRI_VOICE              6 // Voice
#define AEEQOSFLOW_WLAN_USER_PRI_NETWORK_CONTROL    7 // Network Control

//
// IMS Signaling Context, only applicable to UMTS
//
typedef boolean AEEQOSFLOW_UMTSImsSignalingContext;

//
// High Priority Data, only applicable to UMTS
//
typedef boolean AEEQOSFLOW_UMTSHighPriorityData;

//
// WLAN Minimum Service Interval, only applicable to WLAN
//
typedef uint32 AEEQOSFLOW_WLANMinServiceInterval;

//
// WLAN Maximum Service Interval, only applicable to WLAN
//
typedef uint32 AEEQOSFLOW_WLANMaxServiceInterval;

//
// WLAN Inactivity Interval, only applicable to WLAN
//
typedef uint32 AEEQOSFLOW_WLANInactivityInterval;

//
// CDMA Flow Priority, only applicable to CDMA
//
typedef uint8 AEEQOSFLOW_CDMAFlowPriority;

//
// Data Rate MIN MAX
//
typedef struct
{
   uint32 max_rate;           // bps, Rmax
   uint32 guaranteed_rate;    // bps, Rg
} AEEQOSFLOW_DataRateMinMax;


//
// Data Rate Token Bucket
//
typedef struct
{
   uint32 peak_rate;          // bps, Rp
   uint32 token_rate;         // bps, Rb
   uint32 size;               // bytes, B
} AEEQOSFLOW_DataRateTokenBucket;

//
// Packet Error Rate
//
typedef struct
{
   uint16      multiplier;    // Multiplication factor, m
   uint16      exponent;      // -ve of Base 10 exponent, p

} AEEQOSFLOW_PacketErrorRate;

//
// Nominal SDU Size
//
typedef struct
{
   boolean  is_fixed;
   uint32   size;             // Whether the size is fixed or variable
} AEEQOSFLOW_NominalSDUSize;  // SDU size

//
// Defines to set various Quality of Service Flow Options
//

// Define for selecting a unique XOPT_VARIABLE value, based on id.
#define AEEQOSFLOWOPT_VARIABLE_UNIQUE(id)  XOPT_VARIABLE_UNIQUE(id)

// For the following option IDs, pVals are 32-bit quantities, no need to copy
#define AEEQOSFLOWOPT_IP_TRAFFIC_CLASS             (XOPT_32BIT + 0)
#define AEEQOSFLOWOPT_LATENCY                      (XOPT_32BIT + 1)
#define AEEQOSFLOWOPT_LATENCY_VARIANCE             (XOPT_32BIT + 2)
#define AEEQOSFLOWOPT_MIN_POLICED_PACKET_SIZE      (XOPT_32BIT + 3)
#define AEEQOSFLOWOPT_MAX_ALLOWED_PACKET_SIZE      (XOPT_32BIT + 4)
#define AEEQOSFLOWOPT_UMTS_RESIDUAL_BIT_ERROR_RATE (XOPT_32BIT + 5)
#define AEEQOSFLOWOPT_UMTS_TRAFFIC_PRIORITY        (XOPT_32BIT + 6)
#define AEEQOSFLOWOPT_CDMA_PROFILE_ID              (XOPT_32BIT + 7)
#define AEEQOSFLOWOPT_WLAN_USER_PRI                (XOPT_32BIT + 8)
#define AEEQOSFLOWOPT_WLAN_MIN_SERVICE_INTERVAL    (XOPT_32BIT + 9)
#define AEEQOSFLOWOPT_WLAN_MAX_SERVICE_INTERVAL    (XOPT_32BIT + 10)
#define AEEQOSFLOWOPT_WLAN_INACTIVITY_INTERVAL     (XOPT_32BIT + 11)
#define AEEQOSFLOWOPT_CDMA_FLOW_PRI                (XOPT_32BIT + 12)
#define AEEQOSFLOWOPT_UMTS_IMS_SIGNALING_CONTEXT   (XOPT_32BIT + 13)
#define AEEQOSFLOWOPT_UMTS_HIGH_PRIORITY_DATA      (XOPT_32BIT + 14)

// For the following option IDs, pVals are pointers to opaque, copyable structures,
// the low 12 bits of the id specify length of the option, in bytes.

#define AEEQOSFLOWOPT_DATA_RATE_MIN_MAX            (AEEQOSFLOWOPT_VARIABLE_UNIQUE(0) \
                                                    + sizeof(AEEQOSFLOW_DataRateMinMax))

#define AEEQOSFLOWOPT_DATA_RATE_TOKEN_BUCKET       (AEEQOSFLOWOPT_VARIABLE_UNIQUE(1) \
                                                    + sizeof(AEEQOSFLOW_DataRateTokenBucket))

#define AEEQOSFLOWOPT_PACKET_ERROR_RATE            (AEEQOSFLOWOPT_VARIABLE_UNIQUE(2) \
                                                    + sizeof(AEEQOSFLOW_PacketErrorRate))


#define AEEQOSFLOWOPT_NOMINAL_SDU_SIZE             (AEEQOSFLOWOPT_VARIABLE_UNIQUE(3) \
                                                    + sizeof(AEEQOSFLOW_NominalSDUSize))

#define AEEQOSFLOWOPT_END                          XOPT_END
#define AEEQOSFLOWOPT_ANY                          XOPT_ANY
#define AEEQOSFLOWOPT_COPYOPTS                     XOPT_COPYOPTS

#if defined(AEEINTERFACE_CPLUSPLUS)

struct IQoSFlow : public IxOpts
{
   virtual int CDECL ExtFunc (int nOption, uint32 dwVal) = 0;
   int CDECL RemoveOptAtIndex(int32 nIndex)
   {
      return RemoveOpt(XOPT_ANY, nIndex);
   }
   int CDECL GetOptAtIndex(int32 nIndex, AEEQoSFlowOpt *pflo)
   {
      return GetOpt(XOPT_ANY, nIndex, pflo);
   }
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

// Define IQoSFlow interface methods
#define INHERIT_IQoSFlow(iname) \
   INHERIT_IxOpts(iname); \
   int      (*ExtFunc)(iname* po, int nOption, uint32 dwVal)

// declare the actual interface
AEEINTERFACE_DEFINE(IQoSFlow);

static __inline uint32 IQoSFlow_AddRef(IQoSFlow *me)
{
   return AEEGETPVTBL(me,IQoSFlow)->AddRef(me);
}

static __inline uint32 IQoSFlow_Release(IQoSFlow *me)
{
   return AEEGETPVTBL(me,IQoSFlow)->Release(me);
}

static __inline int IQoSFlow_QueryInterface(IQoSFlow *me, AEECLSID id, void **ppo)
{
   return AEEGETPVTBL(me,IQoSFlow)->QueryInterface(me, id, ppo);
}

static __inline int IQoSFlow_AddOpt(IQoSFlow *me, AEEQoSFlowOpt *aflo)
{
   return AEEGETPVTBL(me,IQoSFlow)->AddOpt(me, aflo);
}

static __inline int IQoSFlow_RemoveOpt(IQoSFlow *me, int32 nOptId)
{
   return AEEGETPVTBL(me,IQoSFlow)->RemoveOpt(me, nOptId, 0);
}

static __inline int IQoSFlow_RemoveOptAtIndex(IQoSFlow *me, int32 nIndex)
{
   return AEEGETPVTBL(me,IQoSFlow)->RemoveOpt(me, XOPT_ANY, nIndex);
}

static __inline int IQoSFlow_GetOpt(IQoSFlow *me, int32 nOptId, AEEQoSFlowOpt *pflo)
{
   return AEEGETPVTBL(me,IQoSFlow)->GetOpt(me, nOptId, 0, pflo);
}

static __inline int IQoSFlow_GetOptAtIndex(IQoSFlow *me, int32 nIndex, AEEQoSFlowOpt *pflo)
{
   return AEEGETPVTBL(me,IQoSFlow)->GetOpt(me, XOPT_ANY, nIndex, pflo);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEQOSFLOW_IPTrafficClass

Description:
  Specifies the overall class of traffic to which the IP flow belongs.
  This parameter provides hints to the network about traffic characteristics
  and helps it setup the physical channels appropriately.

Definition:

  #define  AEEQOSFLOW_TRAFFIC_CLASS_CONVERSATIONAL   0
  #define  AEEQOSFLOW_TRAFFIC_CLASS_STREAMING        1
  #define  AEEQOSFLOW_TRAFFIC_CLASS_INTERACTIVE      2
  #define  AEEQOSFLOW_TRAFFIC_CLASS_BACKGROUND       3

  typedef uint32 AEEQOSFLOW_IPTrafficClass;

Members:
   AEEQOSFLOW_TRAFFIC_CLASS_CONVERSATIONAL:
   Conversation traffic is characterized by a low transfer time because of the
   conversational nature of the traffic and limited delay variation of the
   end-to-end flow, to preserve the time relation (variation) between information
   entities of the stream.  ~
   An example of an application bearing such traffic is voice or video conferencing.  ~
   Fundamental characteristics for QOS:
   - Preserve time relation (variation) between information entities of the stream
   - Stringent and low delay

   AEEQOSFLOW_TRAFFIC_CLASS_STREAMING:
   Streaming traffic is characterized by a limited delay variation of the
   end-to-end flow, to preserve the time relation (variation) between information
   entities of the stream.  ~
   The highest acceptable delay variation is application dependent.  ~
   An example of an application bearing such traffic is video streaming. ~
   Fundamental characteristics for QOS:
   - Preserve time relation (variation) between information entities of the stream

   AEEQOSFLOW_TRAFFIC_CLASS_INTERACTIVE:
   Interactive traffic is characterized by the request response pattern of the
   end-user and round trip delay time is therefore one of the key attributes.
   Another characteristic is that the content of the packets shall be transparently
   transferred. ~
   An example of an application bearing such traffic is web browsing. ~
   Fundamental characteristics for QoS: ~
   - Request response pattern
   - Low round trip delay
   - Preserve payload content

   AEEQOSFLOW_TRAFFIC_CLASS_BACKGROUND:
   Background traffic is more or less delivery time insensitive although it requires
   that the content of the packets are transparently transferred. ~
   An example of an application bearing such traffic is background email download. ~
   Fundamental characteristics for QoS:
   - The destination is not expecting the data within a certain time
   - Preserve payload content

Comments:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFLOW_DataRateMinMax

Description:
   Specifies the rate at which the data is transmitted over the network. ~
   Data rate can be specified in one of the two ways - a set of maximum rate
   and guaranteed rate values or in the token bucket parameter style.  ~
   This parameter is used for specifying the set of maximum rate and guaranteed
   rate values.

Definition:

   typedef struct
   {
      uint32 max_rate;           // bps, Rmax ~
      uint32 guaranteed_rate;    // bps, Rg   ~
   } AEEQOSFLOW_DataRateMinMax;

Members:
   max_rate: Maximum required data rate (in bits per second).  ~
   guaranteed_rate: Minimum guaranteed data rate (in bits per second).

Comments:
   A QoS Flow can only contain a single Data Rate specification.   ~
   Either an AEEQOSFLOW_DataRateMinMax or an AEEQOSFLOW_DataRateTokenBucket
   can be specified.

Version:
   Introduced BREW Client 3.1

See Also:
   AEEQOSFLOW_DataRateTokenBucket

=======================================================================
AEEQOSFLOW_DataRateTokenBucket

Description:
   Specifies the rate at which the data is transmitted over the network, ~
   Data rate can be specified in one of the two ways - a set of maximum rate
   and guaranteed rate values or in the token bucket parameter style. ~
   This paramter is used for specifying the set of token bucket parameter values.

Definition:

   typedef struct
   {
      uint32 peak_rate;          // bps, Rp  ~
      uint32 token_rate;         // bps, Rb  ~
      uint32 size;               // bytes, B ~
   } AEEQOSFLOW_DataRateTokenBucket;

Members:
   peak_rate: Maximum rate at which the data can be transmitted when token
              bucket is full (in bits per second) ~
   token_rate: Rate at which the tokens will be put in the token bucket (in
               bits per second). A token is required to be present in the
               bucket to send a byte of data.  ~
   size: Maximum tokens that can be accumulated at any instance (in bytes).
         This controls the size of the burst that is allowed at any given time.  ~

Comments:
   A QoS Flow can only contain a single Data Rate specification.
   Either an AEEQOSFLOW_DataRateMinMax or an AEEQOSFLOW_DataRateTokenBucket
   can be specified.

Version:
   Introduced BREW Client 3.1

See Also:
   AEEQOSFLOW_DataRateMinMax

=======================================================================
AEEQOSFLOW_Latency

Description:
   Specifies the maximum delay (in milliseconds) that can be tolerated by an
   IP packet during the transfer through the RAN.  ~
   The delay is defined as the time from a request to transfer the packet on MS
   to the time when it is received by the IP node across RAN which is serving
   the data session (e.g. GGSN or PDSN)

Definition:
   typedef uint32 AEEQOSFLOW_Latency;

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFLOW_LatencyVariance

Description:
   Specifies the difference between the maximum and minimum delay
   (in milliseconds) that can be tolerated by an IP packet during the transfer
   through the RAN. ~
   This is essentially the maximum tolerated delay jitter. ~
   This parameter is not supported by UMTS network and hence is ignored if the
   underlying network is UMTS.

Definition:
   typedef uint32 AEEQOSFLOW_LatencyVariance;

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFLOW_PacketErrorRate

Description:
   Specifies the maximum packet error rate that can be tolerated by an IP flow.
   The error rate is defined as a ratio of packets received in error to total
   packets transmitted and is specified by two parameters - a multiplier and a
   negative base 10 exponent.
   So the packet error rate is defined as,
	E = m * 10 ** (-p)
   The network technology may use an alternate packet error rate value that it
   is capable of supporting nearest to the one specified.

Definition:

   typedef struct
   {
      uint16      multiplier;  ~
      uint16      exponent;

   } AEEQOSFLOW_PacketErrorRate;

Members:
   multiplier: Multiplication factor, m. ~
   exponent: -ve of Base 10 exponent, p. ~

Comments:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFLOW_NominalSDUSize

Description:
   Specifies the typical SDU size that the application expects to use over
   the traffic stream. It may be either a fixed size or variable size.
   If the corresponding bit in the field mask is set, this parameter is considered
   valid only if:
   1) The subparameter size is greater than zero.
   2) The subparameter size is equal to max_allowed_pkt_size when max_allowed_pkt_size
      is also specified in the flow specification and if the subparameter is_fixed is TRUE.
   3) The subparameter size is less than or equal to max_allowed_pkt_size when
      max_allowed_pkt_size is also specified in the flow specification and if the subparameter
      is_fixed is FALSE.

Definition:

  typedef struct
  {
      boolean  is_fixed;
      uint32   size;
  } AEEQOSFLOW_NominalSDUSize;

Members:
   is_fixed:   Whether the size is fixed or variable
   size:       SDU size

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEQOSFLOW_MinPolicedPacketSize

Description:
   Specifies the minimum packet size (in bytes) that will be policed
   for QOS guarantees. This option is not supported by UMTS network and hence
   is ignored if the underlying network is UMTS.

Definition:
   typedef uint32 AEEQOSFLOW_MinPolicedPacketSize;

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFLOW_MaxAllowedPacketSize

Description:
   Specifies the maximum packet size (in bytes) allowed in the IP flow.

Definition:
   typedef uint32 AEEQOSFLOW_MaxAllowedPacketSize;

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFLOW_UMTSResidualBitErrorRate

Description:
  Specifies the undetected bit error ratio for each IP flow in the delivered
  packets. For equal error protection, only one value is needed. ~
  If no error detection is requested for a subflow, residual bit error ratio
  indicates the bit error ratio in that subflow of the delivered SDUs.
  This parameter is only supported by UMTS network and is ignored by all other
  network technologies

Definition:

  #define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE1      1
  #define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE2      2
  #define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE3      3
  #define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE4      4
  #define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE5      5
  #define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE6      6
  #define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE7      7
  #define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE8      8
  #define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE9      9
  #define AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE10    10

  typedef uint32 AEEQOSFLOW_UMTSResidualBitErrorRate;

Members:
   AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE1:  5*10-2 ~
   AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE2:  1*10-2 ~
   AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE3:  5*10-3 ~
   AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE4:  4*10-3 ~
   AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE5:  1*10-3 ~
   AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE6:  1*10-4 ~
   AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE7:  1*10-5 ~
   AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE8:  1*10-6 ~
   AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE9:  6*10-8 ~
   AEEQOSFLOW_UMTS_RES_BIT_ERR_RATE10: Reserved ~

Comments:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFLOW_UMTSTrafficPriority

Description:
  Specifies the relative priority between various subflows of an IP flow.
  The parameter is only supported by UMTS network and is ignored by all other
  network technologies. ~
  The Traffic handling priority value is ignored if the Traffic Class is
  Conversation class, Streaming class or Background class.  ~
  Within the interactive class, there is a definite need to differentiate
  between bearer qualities.	~
  This is handled by using the traffic handling priority attribute,
  to allow network to schedule traffic accordingly. ~
  By definition, priority is an alternative to absolute guarantees,
  and thus these two attribute types cannot be used together for a single bearer.

Definition:

  #define AEEQOSFLOW_UMTS_TRAFFIC_HANDLING_PRI1        1
  #define AEEQOSFLOW_UMTS_TRAFFIC_HANDLING_PRI2        2
  #define AEEQOSFLOW_UMTS_TRAFFIC_HANDLING_PRI3        3

  typedef uint32 AEEQOSFLOW_UMTSTrafficPriority;

Members:
   AEEQOSFLOW_UMTS_TRAFFIC_HANDLING_PRI1: Priority Level 1 ~
   AEEQOSFLOW_UMTS_TRAFFIC_HANDLING_PRI2: Priority Level 2 ~
   AEEQOSFLOW_UMTS_TRAFFIC_HANDLING_PRI3: Priority Level 3 ~

Comments:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFLOW_CDMAProfileId

Description:
  3GPP TSB58 defines a sub set of standard flow spec parameters for specific
  applications described by a profile id. The standard reserves some profile ids
  to be defined by different network operators. Therefor the mapping between a
  profile id and its equivalent spec parameters is not predefined and depends on
  the operators definitions. ~
  The parameter is only supported by CDMA network and is ignored by all other
  network technologies.  ~
  If this parameter is specified and the underlying network is CDMA, all other
  flow spec parameters are ignored and the profile id is sent to the network
  to request predefined flow spec. ~
  Applications willing to work seamlessly on different network technologies
  including CDMA should also specify other flow spec parameters in addition
  to profile id. ~
  Otherwise if only profile id is specified and the underlying
  network technology is something other than CDMA, then it may not be able to
  request any specific QOS. ~
  For best results the flow spec parameter values specify should correspond to
  the values defined by CDMA standard for the profile id. ~

  AEEQOSFLOW_CDMAProfileId expected values are in the uint16 range (0 to 0xffff).

Definition:

  #define AEEQOSFLOW_CDMA_QOS_PROFILE_1        0x01

  typedef uint32 AEEQOSFLOW_CDMAProfileId;

Members:
   AEEQOSFLOW_CDMA_QOS_PROFILE_1: Profile 1
   This member is no longer in use and is left only to provide backward
   compatibility with BREW versions prior to 3.1.5.

Comments:
   Apps must specify a valid profile id to use QOS over CDMA.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFLOW_WLANUserPriority

Description:
   IEEE 802.1D set of user priority parameters which address how to prioritise
   different classes of user traffic at layer 2.
   The default user priority value is 0 - AEEQOSFLOW_WLAN_USER_PRI_BEST_EFFORT.

Definition:

  typedef uint32 AEEQOSFLOW_WLANUserPriority

   #define AEEQOSFLOW_WLAN_USER_PRI_BEST_EFFORT        0
   #define AEEQOSFLOW_WLAN_USER_PRI_BACKGROUND         1
   #define AEEQOSFLOW_WLAN_USER_PRI_RESERVED           2
   #define AEEQOSFLOW_WLAN_USER_PRI_EXCELLENT_EFFORT   3
   #define AEEQOSFLOW_WLAN_USER_PRI_CONTROLLED_LOAD    4
   #define AEEQOSFLOW_WLAN_USER_PRI_VIDEO              5
   #define AEEQOSFLOW_WLAN_USER_PRI_VOICE              6
   #define AEEQOSFLOW_WLAN_USER_PRI_NETWORK_CONTROL    7

Members:
   AEEQOSFLOW_WLAN_USER_PRI_BEST_EFFORT: LAN traffic as we know it today.
   AEEQOSFLOW_WLAN_USER_PRI_BACKGROUND: Bulk transfers and other activities that
   are permitted on the network but that should not impact the use of the network
   by other users and applications.
   AEEQOSFLOW_WLAN_USER_PRI_RESERVED: Spare.
   AEEQOSFLOW_WLAN_USER_PRI_EXCELLENT_EFFORT: (CEO's best effort) The best-effort
   type services that an information services organization would deliver to its
   most improtant customers.
   AEEQOSFLOW_WLAN_USER_PRI_CONTROLLED_LOAD: Important business applications
   subject to some form of "admission control" from pre-planning of the network
   requriement at one extreme to bandwidth reservation per flow at the time the
   flow is started at theother.
   AEEQOSFLOW_WLAN_USER_PRI_VIDEO: Characterized by less than 100 ms delay.
   AEEQOSFLOW_WLAN_USER_PRI_VOICE: Characterized by less than 10 ms delay, and
   hence maximum jitter (one way transmission through the LAN infrastructure of
   a single camus).
   AEEQOSFLOW_WLAN_USER_PRI_NETWORK_CONTROL: Characterized by a "must get there"
   requirement to maintain and support the network infrastructure.

Comments:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   None

=======================================================================
AEEQOSFLOW_WLANMinServiceInterval

Description:
   IEEE 802.11e minimum service interval which is the minimum time duration
   for which a station would poll the access point for any pending data.
   i.e. every service interval (min_service_interval < service interval)
   the station would send a trigger frame to the access point to
   retrieve data for the access channel specified in the t-spec.
   This is only applicable when the station is doing power-save,
   and the access point offers APSD (automatic power-save delivery).

Definition:

   typedef uint32 AEEQOSFLOW_WLANMinServiceInterval

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEQOSFLOW_WLANMaxServiceInterval;

Description:
   IEEE 802.11e maximum service interval which is the maximum time duration
   for which a station would poll the access point for any pending data.
   i.e. every service interval (max_service_interval > service interval)
   the station would send a trigger frame to the access point to
   retrieve data for the access channel specified in the t-spec.
   This is only applicable when the station is doing power-save,
   and the access point offers APSD (automatic power-save delivery).

Definition:

   typedef uint32 AEEQOSFLOW_WLANMaxServiceInterval

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEQOSFLOW_WLANInactivityInterval

Description:
   IEEE 802.11e inactiviny interval which is the period of inactivity
   (no data transfer) after which the t-spec is torn down
   (and is no longer valid).

Definition:

  typedef uint32 AEEQOSFLOW_WLANInactivityInterval

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEQOSFLOW_UMTSImsSignalingContext

Description:
   This parameter indicates that the application wants to establish
   this QoS instance for IMS signalling.
   This parameter is supported by 3GPP networks only and ignored by
   all other network technologies.

Definition:

   typedef boolean AEEQOSFLOW_UMTSImsSignalingContext;

Comments:
   None

Version:
   Introduced BREW Client 4.0.2

See Also:
   None

=======================================================================
AEEQOSFLOW_UMTSHighPriorityData

Description:
   This parameter indicates that the data on this QoS instance
   has high priority and requests the network to expeditedly process
   the data.
   This parameter is supported by 3GPP networks only and ignored by
   all other network technologies.

Definition:

   typedef boolean AEEQOSFLOW_UMTSHighPriorityData;

Comments:
   None

Version:
   Introduced BREW Client 4.0.2

See Also:
   None
=======================================================================
AEEQoSFlowOpt

Description:
   Holds an (id, value) pair for AEEQoSFlow.

Definition:
   typedef xOpt AEEQoSFlowOpt;

Members:
   int32  nId: option id, chosen from one of the QoS Flow options
   void *pVal: option value, depends on nId

Comments:
   AEEQoSFlowOpt structures indicate various parameters of an AEEQoSFlow.
   The structures, when passed to AddOpt, are not themselves copied,
   the contents are copied out.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFLOWOPT_

The prefix AEEQOSFLOWOPT_ is used for QoS Flow options.
=======================================================================

QoS Flow options

Description:
   QoS Flow options specify the policy for handling the QoS Flow.

Definition:
   #define AEEQOSFLOWOPT_IP_TRAFFIC_CLASS             (XOPT_32BIT + 0)

   #define AEEQOSFLOWOPT_LATENCY                      (XOPT_32BIT + 1)

   #define AEEQOSFLOWOPT_LATENCY_VARIANCE             (XOPT_32BIT + 2)

   #define AEEQOSFLOWOPT_MIN_POLICED_PACKET_SIZE      (XOPT_32BIT + 3)

   #define AEEQOSFLOWOPT_MAX_ALLOWED_PACKET_SIZE      (XOPT_32BIT + 4)

   #define AEEQOSFLOWOPT_UMTS_RESIDUAL_BIT_ERROR_RATE (XOPT_32BIT + 5)

   #define AEEQOSFLOWOPT_UMTS_TRAFFIC_PRIORITY        (XOPT_32BIT + 6)

   #define AEEQOSFLOWOPT_CDMA_PROFILE_ID              (XOPT_32BIT + 7)

   #define AEEQOSFLOWOPT_WLAN_USER_PRI                (XOPT_32BIT + 8)

   #define AEEQOSFLOWOPT_WLAN_MIN_SERVICE_INTERVAL    (XOPT_32BIT + 9)

   #define AEEQOSFLOWOPT_WLAN_MAX_SERVICE_INTERVAL    (XOPT_32BIT + 10)

   #define AEEQOSFLOWOPT_WLAN_INACTIVITY_INTERVAL     (XOPT_32BIT + 11)

   #define AEEQOSFLOWOPT_CDMA_FLOW_PRI                (XOPT_32BIT + 12)

   #define AEEQOSFLOWOPT_UMTS_IMS_SIGNALING_CONTEXT   (XOPT_32BIT + 13)

   #define AEEQOSFLOWOPT_UMTS_HIGH_PRIORITY_DATA      (XOPT_32BIT + 14)

   #define AEEQOSFLOWOPT_VARIABLE_UNIQUE(id)          XOPT_VARIABLE_UNIQUE(id)

   #define AEEQOSFLOWOPT_DATA_RATE_MIN_MAX            (AEEQOSFLOWOPT_VARIABLE_UNIQUE(0)
                                                       + sizeof(AEEQOSFLOW_DataRateMinMax))

   #define AEEQOSFLOWOPT_DATA_RATE_TOKEN_BUCKET       (AEEQOSFLOWOPT_VARIABLE_UNIQUE(1)
                                                       + sizeof(AEEQOSFLOW_DataRateTokenBucket))

   #define AEEQOSFLOWOPT_PACKET_ERROR_RATE            (AEEQOSFLOWOPT_VARIABLE_UNIQUE(2)
                                                       + sizeof(AEEQOSFLOW_PacketErrorRate))

   #define AEEQOSFLOWOPT_NOMINAL_SDU_SIZE             (AEEQOSFLOWOPT_VARIABLE_UNIQUE(3) \
                                                       + sizeof(AEEQOSFLOW_NominalSDUSize))

   #define AEEQOSFLOWOPT_END                          XOPT_END

   #define AEEQOSFLOWOPT_ANY                          XOPT_ANY

   #define AEEQOSFLOWOPT_COPYOPTS                     XOPT_COPYOPTS

Members:
   AEEQOSFLOWOPT_IP_TRAFFIC_CLASS:
   Traffic class option (see AEEQOSFLOW_IPTrafficClass)

   AEEQOSFLOWOPT_LATENCY:
   Latency option (see AEEQOSFLOW_Latency)

   AEEQOSFLOWOPT_LATENCY_VARIANCE:
   Latency Variance option (see AEEQOSFLOW_LatencyVariance)

   AEEQOSFLOWOPT_MIN_POLICED_PACKET_SIZE:
   Min Packet size option (see AEEQOSFLOW_MinPolicedPacketSize)

   AEEQOSFLOWOPT_MAX_ALLOWED_PACKET_SIZE:
   Max Packet size option (see AEEQOSFLOW_MaxAllowedPacketSize)

   AEEQOSFLOWOPT_UMTS_RESIDUAL_BIT_ERROR_RATE:
   Bit Error rate option (see AEEQOSFLOW_UMTSResidualBitErrorRate)

   AEEQOSFLOWOPT_UMTS_TRAFFIC_PRIORITY:
   Traffic priority option (see AEEQOSFLOW_UMTSTrafficPriority)

   AEEQOSFLOWOPT_CDMA_PROFILE_ID:
   CDMA profile id option (see AEEQOSFLOW_CDMAProfileId).
   This is a mandatory option in case of QoS over CDMA.

   AEEQOSFLOWOPT_WLAN_USER_PRI:
   WLAN user priority. This priority refers to 802.1D user priorities which
   maps to a specific WLAN access category (see AEEQOSFLOW_WLANUserPriority)

   AEEQOSFLOWOPT_WLAN_MIN_SERVICE_INTERVAL:
   WLAN minimum service interval. This interval refers to 802.11e
   minimum service interval (see AEEQOSFLOW_WLANMinServiceInterval).

   AEEQOSFLOWOPT_WLAN_MAX_SERVICE_INTERVAL:
   WLAN maximum service interval. This interval refers to 802.11e
   maximum service interval (see AEEQOSFLOW_WLANMaxServiceInterval).

   AEEQOSFLOWOPT_WLAN_INACTIVITY_INTERVAL:
   WLAN inactivity interval. This inactivity interval refers to 802.1e
   inactivity interval (see AEEQOSFLOW_WLANInactivityInterval).

   AEEQOSFLOWOPT_CDMA_FLOW_PRI:
   CDMA flow priority option (see AEEQOSFLOW_CDMAFlowPriority).

   AEEQOSFLOWOPT_UMTS_IMS_SIGNALING_CONTEXT:
   UMTS IMS signaling context option (see AEEQOSFLOW_UMTSImsSignalingContext).

   AEEQOSFLOWOPT_UMTS_HIGH_PRIORITY_DATA:
   UMTS high priority data option (see AEEQOSFLOW_UMTSHighPriorityData).

   AEEQOSFLOWOPT_DATA_RATE_MIN_MAX:
   Date Rate option (see AEEQOSFLOW_DataRateMinMax )

   AEEQOSFLOWOPT_DATA_RATE_TOKEN_BUCKET:
   Date Rate option (see AEEQOSFLOW_DataRateTokenBucket)

   AEEQOSFLOWOPT_PACKET_ERROR_RATE:
   Packet Error Rate option (see AEEQOSFLOW_UMTSResidualBitErrorRate)

   AEEQOSFLOWOPT_NOMINAL_SDU_SIZE:
   Nominal SDU Size option (see AEEQOSFLOW_NominalSDUSize)

Comments:
   1. Applications must not request both AEEQOSFLOWOPT_DATA_RATE_MIN_MAX and
      AEEQOSFLOWOPT_DATA_RATE_TOKEN_BUCKET in the same IQoSFlow. If one of
      them was specified, then trying to AddOpt() the other one will fail.

   2. Applications must not specify an option more then once.
      AddOpt() will fail for an option that already exists in the IQoSFlow.

Version:
   Introduced BREW Client 3.1

See Also:
   None
=======================================================================

AEEQOSFLOW_CDMAFlowPriority

Description:
   Specifies relative priority among QoS flows. The value could be used
   by the network to provide admission control and resource allocation.
   This parameter is supported by 3GPP2 networks only and is ignored
   by all other network technologies.

   AEEQOSFLOW_CDMAFlowPriority expected values are in the uint8 range.

   See TIA-835.4-D spec for more information.

Definition:

   typedef uint8 AEEQOSFLOW_CDMAFlowPriority;

Comments:
   This parameter is not mandatory for Apps that use QOS over CDMA.
   Low value is high in priority.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================

AEEQOSFLOW_NominalSDUSize

Description:
   Specifies the typical SDU size that the application expects to use over
   this t-spec traffic stream. It may be either a fixed size or variable size.

Definition:

  typedef struct
  {
      boolean  is_fixed;
      uint32   size;
  } AEEQOSFLOW_NominalSDUSize;

Members:
   is_fixed:   Whether the size is fixed or variable
   size:       SDU size

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEQOSFLOW_WLANMinServiceInterval

Description:
   IEEE 802.11e minimum service interval which is the minimum time duration
   for which a station would poll the access point for any pending data.
   i.e. every service interval (min_service_interval < service interval)
   the station would send a trigger frame to the access point to
   retrieve data for the access channel specified in the t-spec.
   This is only applicable when the station is doing power-save,
   and the access point offers APSD (automatic power-save delivery).

Definition:

   typedef uint32 AEEQOSFLOW_WLANMinServiceInterval

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None


=======================================================================
AEEQOSFLOW_WLANMaxServiceInterval

Description:
   IEEE 802.11e maximum service interval which is the maximum time duration
   for which a station would poll the access point for any pending data.
   i.e. every service interval (max_service_interval > service interval)
   the station would send a trigger frame to the access point to
   retrieve data for the access channel specified in the t-spec.
   This is only applicable when the station is doing power-save,
   and the access point offers APSD (automatic power-save delivery).

Definition:

   typedef uint32 AEEQOSFLOW_WLANMaxServiceInterval

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None


=======================================================================
AEEQOSFLOW_WLANInactivityInterval

Description:
   IEEE 802.11e inactivity interval which is the period of inactivity
   (no data transfer) after which the t-spec is torn down
   (and is no longer valid).

Definition:

  typedef uint32 AEEQOSFLOW_WLANInactivityInterval

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IQoSFlow

Description:
   IQoSFlow interface encapsulates a Quality of Service Flow specification
   where the flow spec defines the treatment required for an IP flow, i.e.
   QoS paramteres such as required bit rate etc. ~
   An IQoSFlow interface instance is obtained via the IEnv_CreateInstance()
   mechanism.

The following header file is required:
   BREW 4.0 - AEEIQoSFlow.h
   BREW 3.1 - AEEQoSFlow.h

=============================================================================

IQoSFlow_AddRef()

Description:
   This method increments the reference count of the IQoSFlow Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 IQoSFlow_AddRef(IQoSFlow * po)

Parameters:
   po [in]: Pointer to the IQoSFlow Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSFlow_Release()

=============================================================================

IQoSFlow_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQoSFlow_Release(IQoSFlow * po)

Parameters:
   po [in]: Pointer to the IQoSFlow Interface object

Return Value:
   Decremented reference count for the object.

   The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSFlow_AddRef()

==============================================================================

Function: IQoSFlow_QueryInterface()

Description:

   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interface is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:

   int IQoSFlow_QueryInterface(IQoSFlow *po, AEECLSID id, void **p)

Parameters:
   po [in]: Pointer to IQoSFlow interface
   id [in]: A globally unique id to identify the entity (interface or data) that we
            are trying to query.
   p [out]: Pointer to the data or interface that we want to retrieve.  If the interface
            is not available, this is set to NULL.

Return Value:
   AEE_SUCCESS: on success,
   AEE_ECLASSNOTSUPPORT: if class ID not supported

Comments:

   On failure, QueryInterface() must set *p to NULL.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

Function: IQoSFlow_AddOpt()

Description:
   Append the list of AEEQoSFlowOpts to the current QoS Flow option set

Prototype:

  int IQoSFlow_AddOpt(IQoSFlow *po, AEEQoSFlowOpt *apwoz)

Parameters:
  po: the interface pointer
  apwoz: array of AEEQoSFlowOpt structs, terminated by a AEEQoSFlowOpt with
         nId set to AEEQOSFLOWOPT_END

Return Value:
   AEE_SUCCESS: options were added successfully
   AEE_ENOMEMORY: could not allocated enough memory
   AEE_EBADITEM: on of the items either:
            (1) has same option ID as an existing option in the QoS Flow.
            (2) conflicts with another option in the QoS Flow.

Comments:
   The AEEQOSFLOW_DataRateMinMax and AEEQOSFLOW_DataRateTokenBucket options are
   mutually exclusive.

 Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

Function: IQoSFlow_RemoveOpt()

Description:
   Removes the AEEQoSFlowOpt matching nOptId

Prototype:

   int IQoSFlow_RemoveOpt(IQoSFlow *po, int32 nOptId)

Parameters:
   po:  the interface pointer
   nOptId: id of the option to remove from the options list

Return Value:
   AEE_SUCCESS: if the option was removed successfully
   AEE_EFAILED: if the specified AEEQoSFlowOpt couldn't be found

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

Function: IQoSFlow_RemoveOptAtIndex()

Description:
   Removes the AEEQoSFlowOpt at nIndex

Prototype:

   int IQoSFlow_RemoveOptAtIndex(IQoSFlow *po, int32 nIndex)

Parameters:
   po:  the interface pointer
   nIndex: id of the option to remove from the options list

Return Value:
   AEE_SUCCESS: if the option was removed successfully
   AEE_EFAILED: if the specified index couldn't be found

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

Function: IQoSFlow_GetOpt()

Description:

   Find an AEEQoSFlowOpt, in the list of AEEQoSFlowOpts in po, matching nOptId.
   pwo is filled in with a the matching AEEQoSFlowOpt contents.

Prototype:
   int IQoSFlow_GetOpt(IQoSFlow *po, int32 nOptId, AEEQoSFlowOpt *pwo)

Parameters:
   po :  the interface pointer
   nOptId: id of the option to find in the options list
   pwo: where to stick the found AEEQoSFlowOpt

Return Value:
   AEE_SUCCESS:if the option was retrieved successfully

   AEE_EFAILED: if the specified AEEQoSFlowOpt couldn't be retrieved

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=============================================================================

Function: IQoSFlow_GetOptAtIndex()

Description:
   Find the AEEQoSFlowOpt, in the list of AEEQoSFlowOpts in po, at nIndex.
   pwo is filled in with a the matching AEEQoSFlowOpt contents.

Prototype:
   int IQoSFlow_GetOptAtIndex(IQoSFlow *po, int32 nIndex, AEEQoSFlowOpt *pwo)

Parameters:
   po :  the interface pointer
   nIndex: Option Index.
   pwo: where to stick the found AEEQoSFlowOpt

Return Value:

   AEE_SUCCESS:if the option was retrieved successfully

   AEE_EFAILED: if the specified index couldn't be found

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================*/

#endif  // AEEIQOSFLOW_H
