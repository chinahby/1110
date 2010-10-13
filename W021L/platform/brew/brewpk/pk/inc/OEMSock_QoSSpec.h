/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

   Q O S S P E C   A P I   H E A D E R   F I L E

DESCRIPTION

 The OEM Socket Quality of Service Specification Header File.
 Contains declarations for Quality of Service OEM layer data structures.


        Copyright © 2000-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
#ifndef OEMSOCK_QOSSPEC_H
#define OEMSOCK_QOSSPEC_H


#include "AEE_OEMComdef.h"
#include "AEEComdef.h"
#include "AEEError.h"


/*===========================================================================

                      GLOBAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  QoS general definitions
---------------------------------------------------------------------------*/

typedef uint32 oem_qos_handle_type;

/*---------------------------------------------------------------------------
  IP definitions
---------------------------------------------------------------------------*/

typedef enum
{
  OEM_IP_V4 = 4,
  OEM_IP_V6 = 6
} oem_ip_version_enum_type;

struct oem_in_addr
{
  uint32 s_addr;   /* socket ipv4 address */
};

struct oem_in6_addr
{
   union
   {
      uint8   u6_addr8[16];
      uint16  u6_addr16[8];
      uint32  u6_addr32[4];
      uint64  u6_addr64[2];
   } in6_u;
};

typedef enum
{
  OEM_IPPROTO_TCP          = 6,                                /* TCP Protocol */
  OEM_IPPROTO_UDP          = 17,                               /* UDP Protocol */
  OEM_NO_NEXT_HDR          = 59
} oem_ip_protocol_enum_type;


/*---------------------------------------------------------------------------
  QoS flow definitions
---------------------------------------------------------------------------*/
typedef enum
{
  OEM_IP_TRF_CLASS_CONVERSATIONAL   = 0,
  OEM_IP_TRF_CLASS_STREAMING        = 1,
  OEM_IP_TRF_CLASS_INTERACTIVE      = 2,
  OEM_IP_TRF_CLASS_BACKGROUND       = 3
} oem_ip_traffic_class_enum_type;

typedef enum
{
  OEM_UMTS_RES_BIT_ERR_RATE1        = 1,        /* 5*10-2 */
  OEM_UMTS_RES_BIT_ERR_RATE2        = 2,        /* 1*10-2 */
  OEM_UMTS_RES_BIT_ERR_RATE3        = 3,        /* 5*10-3 */
  OEM_UMTS_RES_BIT_ERR_RATE4        = 4,        /* 4*10-3 */
  OEM_UMTS_RES_BIT_ERR_RATE5        = 5,        /* 1*10-3 */
  OEM_UMTS_RES_BIT_ERR_RATE6        = 6,        /* 1*10-4 */
  OEM_UMTS_RES_BIT_ERR_RATE7        = 7,        /* 1*10-5 */
  OEM_UMTS_RES_BIT_ERR_RATE8        = 8,        /* 1*10-6 */
  OEM_UMTS_RES_BIT_ERR_RATE9        = 9,        /* 6*10-8 */
  OEM_UMTS_RES_BIT_ERR_RATE10       = 10        /* Reserved */
} oem_umts_residual_ber_enum_type;

typedef enum
{
  OEM_UMTS_TRF_HANDLING_PRI1        = 1,        /* Priority Level 1 */
  OEM_UMTS_TRF_HANDLING_PRI2        = 2,        /* Priority Level 2 */
  OEM_UMTS_TRF_HANDLING_PRI3        = 3         /* Priority Level 3 */
} oem_umts_trf_handling_pri_enum_type;

typedef uint16 oem_cdma_flow_spec_profile_id_type;
typedef uint8  oem_cdma_flow_priority_type;

typedef enum
{
   OEM_WLAN_USER_PRI_BEST_EFFORT       = 0, /* Best Effort */
   OEM_WLAN_USER_PRI_BACKGROUND        = 1, /* Background */
   OEM_WLAN_USER_PRI_RESERVED          = 2, /* Spare */
   OEM_WLAN_USER_PRI_EXCELLENT_EFFORT  = 3, /* Excellent Effort */
   OEM_WLAN_USER_PRI_CONTROLLED_LOAD   = 4, /* Controlled Load */
   OEM_WLAN_USER_PRI_VIDEO             = 5, /* Video */
   OEM_WLAN_USER_PRI_VOICE             = 6, /* Voice */
   OEM_WLAN_USER_PRI_NETWORK_CONTROL   = 7  /* Network Control */
} oem_wlan_user_pri_enum_type;

typedef enum
{
  OEM_IPFLOW_MASK_NONE                  = 0x00000000,
  OEM_IPFLOW_MASK_TRF_CLASS             = 0x00000001,
  OEM_IPFLOW_MASK_DATA_RATE             = 0x00000002,
  OEM_IPFLOW_MASK_LATENCY               = 0x00000004,
  OEM_IPFLOW_MASK_LATENCY_VAR           = 0x00000008,
  OEM_IPFLOW_MASK_PKT_ERR_RATE          = 0x00000010,
  OEM_IPFLOW_MASK_MIN_POLICED_PKT_SIZE  = 0x00000020,
  OEM_IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE  = 0x00000040,
  OEM_IPFLOW_MASK_UMTS_RES_BER          = 0x00000080,
  OEM_IPFLOW_MASK_UMTS_TRF_PRI          = 0x00000100,
  OEM_IPFLOW_MASK_CDMA_PROFILE_ID       = 0x00000200,
  OEM_IPFLOW_MASK_WLAN_USER_PRI         = 0x00000400,
  OEM_IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL = 0x00000800,
  OEM_IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL = 0x00001000,
  OEM_IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL  = 0x00002000,
  OEM_IPFLOW_MASK_NOMINAL_SDU_SIZE          = 0x00004000,
  OEM_IPFLOW_MASK_CDMA_FLOW_PRIORITY    = 0x00008000
} oem_ipflow_field_mask_enum_type;

typedef uint32 oem_ipflow_field_mask_type;

typedef enum
{
  OEM_DATA_RATE_FORMAT_MIN_MAX_TYPE,
  OEM_DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE
} oem_ipflow_data_rate_format_type;

typedef struct
{
  oem_ipflow_field_mask_type      field_mask;
  oem_ipflow_field_mask_type      err_mask;

  oem_ip_traffic_class_enum_type  trf_class;          /* TC */

  struct
  {
    oem_ipflow_data_rate_format_type  format_type;

    union
    {
      struct
      {
        uint32      max_rate;           /* bps, Rmax */
        uint32      guaranteed_rate;    /* bps, Rg */
      } min_max;

      struct
      {
        uint32      peak_rate;          /* bps, Rp */
        uint32      token_rate;         /* bps, Rb */
        uint32      size;               /* bytes, B */
      } token_bucket;
    } format;
  } data_rate;

  uint32        latency;            /* msec, L */
  uint32        latency_var;        /* msec, Lvar */

  struct                            /* ratio, Eper = m * 10 ** (-p) */
  {
    uint16      multiplier;         // Multiplication factor, m
    uint16      exponent;           // -ve of Base 10 exponent, p
  } pkt_err_rate;

  uint32      min_policed_pkt_size;   /* bytes, Smin */
  uint32      max_allowed_pkt_size;   /* bytes, Smax */

  struct
  {
     boolean  is_fixed; /* is size fixed */
     uint32   size;     /* SDU size */
  } nominal_sdu_size;

  struct
  {
    oem_umts_residual_ber_enum_type         res_ber;        /* Erber */
    oem_umts_trf_handling_pri_enum_type     trf_pri;        /* Ptrf */
  } umts_params;

  struct
  {
    oem_cdma_flow_spec_profile_id_type  profile_id;     /* I */
    oem_cdma_flow_priority_type         flow_priority;  /* CDMA Flow Priority */
  }	cdma_params;

  /* WLAN only QOS parameters */
  struct
  {
     oem_wlan_user_pri_enum_type     user_priority;        /* User Priority */
     uint32                          min_service_interval; /* In micro sec  */
     uint32                          max_service_interval; /* In micro sec  */
     uint32                          inactivity_interval;  /* In micro sec  */
  } wlan_params;

} oem_ip_flow_type;


typedef struct
{
  oem_ip_flow_type      req_flow;         /* Required flow type           */
  oem_ip_flow_type      min_req_flow;     /* Minimum required flow type   */
  /* To allow auxiliary flow specs */
  uint8                 num_aux_flows;    /* # of auxiliary flows in the list */
  oem_ip_flow_type *    aux_flow_list_ptr;/* Ptr to array of auxiliary flows  */
} oem_ip_flow_spec_type;


/*---------------------------------------------------------------------------
  QoS filter definitions
---------------------------------------------------------------------------*/

/* IPV4 hdr fields */
typedef enum
{
  OEM_IPFLTR_MASK_IP4_SRC_ADDR      = 0x01,
  OEM_IPFLTR_MASK_IP4_DST_ADDR      = 0x02,
  OEM_IPFLTR_MASK_IP4_NEXT_HDR_PROT = 0x04,
  OEM_IPFLTR_MASK_IP4_TOS           = 0x08
} oem_ipfltr_ip4_hdr_field_mask_enum_type;

/* IPV6 hdr fields */
typedef enum
{
   OEM_IPFLTR_MASK_IP6_SRC_ADDR      = 0x01,
   OEM_IPFLTR_MASK_IP6_DST_ADDR      = 0x02,
   OEM_IPFLTR_MASK_IP6_NEXT_HDR_PROT = 0x04,
   OEM_IPFLTR_MASK_IP6_TRAFFIC_CLASS = 0x08,
   OEM_IPFLTR_MASK_IP6_FLOW_LABEL    = 0x10
} oem_ipfltr_ip6_hdr_field_mask_enum_type;

/* Higher level protocol hdr parameters */

/* TCP hdr fields */
typedef enum
{
  OEM_IPFLTR_MASK_TCP_SRC_PORT      = 0x01,
  OEM_IPFLTR_MASK_TCP_DST_PORT      = 0x02
} oem_ipfltr_tcp_hdr_field_mask_enum_type;

/* UDP hdr fields */
typedef enum
{
  OEM_IPFLTR_MASK_UDP_SRC_PORT      = 0x01,
  OEM_IPFLTR_MASK_UDP_DST_PORT      = 0x02
} oem_ipfltr_udp_hdr_field_mask_enum_type;


typedef uint8 oem_ipfltr_ip4_hdr_field_mask_type;
typedef uint8 oem_ipfltr_ip6_hdr_field_mask_type;
typedef uint8 oem_ipfltr_tcp_hdr_field_mask_type;
typedef uint8 oem_ipfltr_udp_hdr_field_mask_type;

/* QoS IP filter */
typedef struct
{
  /* Mandatory Parameter - IP version of the filter (v4 or v6)	*/
  oem_ip_version_enum_type  ip_vsn;

  /* Filter parameter values, the ones set in field masks are only valid */
  /* Corresponding err mask is set if a parameter value is invalid */
  union
  {
    struct
    {
      oem_ipfltr_ip4_hdr_field_mask_type      field_mask;  /* In mask   */
      oem_ipfltr_ip4_hdr_field_mask_type      err_mask;    /* Out mask  */

      struct
      {
        struct oem_in_addr  addr;
        struct oem_in_addr  subnet_mask;
      } src;

      struct
      {
        struct oem_in_addr  addr;
        struct oem_in_addr  subnet_mask;
      } dst;

      struct
      {
        uint8 val;
        uint8 mask;
      } tos;

      uint8 next_hdr_prot;
    } v4;

   struct
   {
      oem_ipfltr_ip6_hdr_field_mask_type      field_mask;  /* In mask   */
      oem_ipfltr_ip6_hdr_field_mask_type      err_mask;    /* Out mask  */

      struct
      {
        struct oem_in6_addr  addr;
        uint8                prefix_len;
      } src;

      struct
      {
        struct oem_in6_addr  addr;
        uint8                prefix_len;
      } dst;

      struct
      {
        uint8 val;
        uint8 mask;
      } trf_cls;

      uint32   flow_label;
      uint8    next_hdr_prot;   /* This is transport level protocol header */
    } v6;
  } ip_hdr;

  /* next_hdr_prot field in v4 or v6 hdr must be set to specify a    */
  /* parameter from the next_prot_hdr                                */
  union
  {
    struct
    {
      oem_ipfltr_tcp_hdr_field_mask_type      field_mask;  /* In mask   */
      oem_ipfltr_tcp_hdr_field_mask_type      err_mask;    /* Out mask  */

      struct
      {
        uint16    port;
        uint16    range;
      } src;

      struct
      {
        uint16    port;
        uint16    range;
      } dst;
    } tcp;

    struct
    {
      oem_ipfltr_udp_hdr_field_mask_type      field_mask;  /* In mask   */
      oem_ipfltr_udp_hdr_field_mask_type      err_mask;    /* Out mask  */

      struct
      {
        uint16    port;
        uint16	  range;
      } src;

      struct
      {
        uint16    port;
        uint16    range;
      } dst;
    } udp;

  } next_prot_hdr;
} oem_ip_filter_type;

typedef struct
{
  uint8                  num_filters;      /* Num filters in the list     */
  oem_ip_filter_type    *list_ptr;         /* List of filters             */
} oem_ip_filter_spec_type;


/*---------------------------------------------------------------------------
  QoS Spec definitions
---------------------------------------------------------------------------*/

typedef enum
{
  OEM_QOS_MASK_RX_FLOW                = 0x001,
  OEM_QOS_MASK_RX_MIN_FLOW            = 0x002,
  OEM_QOS_MASK_RX_AUXILIARY_FLOWS     = 0x004,
  OEM_QOS_MASK_TX_FLOW                = 0x008,
  OEM_QOS_MASK_TX_MIN_FLOW            = 0x010,
  OEM_QOS_MASK_TX_AUXILIARY_FLOWS     = 0x020,
  OEM_QOS_MASK_RX_FLTR_MODIFY         = 0x040,
  OEM_QOS_MASK_TX_FLTR_MODIFY         = 0x080
} oem_qos_spec_field_mask_enum_type;

typedef uint16 oem_qos_spec_field_mask_type;

typedef struct
{
  oem_qos_spec_field_mask_type  field_mask;

  struct
  {
    oem_ip_flow_spec_type     flow_template;    /* traffic flow template   */
    oem_ip_filter_spec_type   fltr_template;    /* traffic filter template */
  } rx;

  struct
  {
    oem_ip_flow_spec_type     flow_template;  /* traffic flow template   */
    oem_ip_filter_spec_type   fltr_template;  /* traffic filter template */
  } tx;
} oem_qos_spec_type;

/*---------------------------------------------------------------------------
  Primary QoS Spec definitions
---------------------------------------------------------------------------*/

typedef struct
{
  oem_qos_spec_field_mask_type   field_mask;         /* How is QoS modified?   */
  oem_ip_flow_spec_type          rx_flow_template;   /* QoS in Rx direction    */
  oem_ip_flow_spec_type          tx_flow_template;   /* QoS in Tx direction    */
} oem_primary_qos_spec_type;

#if defined(__cplusplus)
extern "C" {
#endif

   // function declarations

#if defined(__cplusplus)
}
#endif


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

oem_ip_version_enum_type

Description:
   Specifies the IP version

Definition:

typedef enum
{
  OEM_IP_V4 = 4,
  OEM_IP_V6 = 6
} oem_ip_version_enum_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_in_addr

Description:
   Specifies the IPv4 address

Definition:
   struct oem_in_addr
   {
     uint32 s_addr;
   };

Members:
   s_addr: IPv4 address

Comments:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   oem_in6_addr

=======================================================================

oem_in6_addr

Description:
   Specifies the IPv6 address

Definition:
   struct oem_in6_addr
   {
      union
      {
         uint8   u6_addr8[16];
         uint16  u6_addr16[8];
         uint32  u6_addr32[4];
         uint64  u6_addr64[2];
      } in6_u;
   };

Members:
   in6_u: IPv6 address

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   oem_in_addr

=======================================================================

oem_ip_protocol_enum_type

Description:
   Specifies the IP protocol

Definition:

typedef enum
{
  OEM_IPPROTO_TCP          = 6,   // TCP protocol
  OEM_IPPROTO_UDP          = 17,  // UDP protocol
  OEM_NO_NEXT_HDR          = 59,  // See next header for protocol
} oem_ip_protocol_enum_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_ip_traffic_class_enum_type

Description:
   Specifies the IP traffic class

Definition:

typedef enum
{
  OEM_IP_TRF_CLASS_CONVERSATIONAL   = 0,
  OEM_IP_TRF_CLASS_STREAMING        = 1,
  OEM_IP_TRF_CLASS_INTERACTIVE      = 2,
  OEM_IP_TRF_CLASS_BACKGROUND       = 3
} oem_ip_traffic_class_enum_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_umts_residual_ber_enum_type

Description:
   Specifies the UMTS residual bit error ratio

Definition:

typedef enum
{
  OEM_UMTS_RES_BIT_ERR_RATE1        = 1,        // 5*10-2
  OEM_UMTS_RES_BIT_ERR_RATE2        = 2,        // 1*10-2
  OEM_UMTS_RES_BIT_ERR_RATE3        = 3,        // 5*10-3
  OEM_UMTS_RES_BIT_ERR_RATE4        = 4,        // 4*10-3
  OEM_UMTS_RES_BIT_ERR_RATE5        = 5,        // 1*10-3
  OEM_UMTS_RES_BIT_ERR_RATE6        = 6,        // 1*10-4
  OEM_UMTS_RES_BIT_ERR_RATE7        = 7,        // 1*10-5
  OEM_UMTS_RES_BIT_ERR_RATE8        = 8,        // 1*10-6
  OEM_UMTS_RES_BIT_ERR_RATE9        = 9,        // 6*10-8
  OEM_UMTS_RES_BIT_ERR_RATE10       = 10        // Reserved
} oem_umts_residual_ber_enum_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_umts_trf_handling_pri_enum_type

Description:
   Specifies the UMTS traffic handling priority

Definition:

typedef enum
{
  OEM_UMTS_TRF_HANDLING_PRI1        = 1,        // Priority Level 1
  OEM_UMTS_TRF_HANDLING_PRI2        = 2,        // Priority Level 2
  OEM_UMTS_TRF_HANDLING_PRI3        = 3         // Priority Level 3
} oem_umts_trf_handling_pri_enum_type;


Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_cdma_flow_spec_profile_id_type

Description:
   Specifies the CDMA QoS profile id

Definition:

typedef uint16 oem_cdma_flow_spec_profile_id_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_cdma_flow_priority_type

Description:
   Specifies CDMA QoS flow priority

Definition:

typedef uint8 oem_cdma_flow_priority_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================

oem_wlan_user_pri_enum_type

Description:
   Specifies the WLAN user priority

Definition:

typedef enum
{
   OEM_WLAN_USER_PRI_BEST_EFFORT       = 0,
   OEM_WLAN_USER_PRI_BACKGROUND        = 1,
   OEM_WLAN_USER_PRI_RESERVED          = 2,
   OEM_WLAN_USER_PRI_EXCELLENT_EFFORT  = 3,
   OEM_WLAN_USER_PRI_CONTROLLED_LOAD   = 4,
   OEM_WLAN_USER_PRI_VIDEO             = 5,
   OEM_WLAN_USER_PRI_VOICE             = 6,
   OEM_WLAN_USER_PRI_NETWORK_CONTROL   = 7
} oem_wlan_user_pri_enum_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1.4

See Also:
   None

=======================================================================

oem_ipflow_field_mask_type

Description:
   Specifies the IP flow field mask, which specifies what
   are the valid fields.

Definition:

typedef enum
{
  OEM_IPFLOW_MASK_TRF_CLASS             = 0x00000001,
  OEM_IPFLOW_MASK_DATA_RATE             = 0x00000002,
  OEM_IPFLOW_MASK_LATENCY               = 0x00000004,
  OEM_IPFLOW_MASK_LATENCY_VAR           = 0x00000008,
  OEM_IPFLOW_MASK_PKT_ERR_RATE          = 0x00000010,
  OEM_IPFLOW_MASK_MIN_POLICED_PKT_SIZE  = 0x00000020,
  OEM_IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE  = 0x00000040,
  OEM_IPFLOW_MASK_UMTS_RES_BER          = 0x00000080,
  OEM_IPFLOW_MASK_UMTS_TRF_PRI          = 0x00000100,
  OEM_IPFLOW_MASK_CDMA_PROFILE_ID       = 0x00000200,
  OEM_IPFLOW_MASK_WLAN_USER_PRI         = 0x00000400,
  OEM_IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL = 0x00000800,
  OEM_IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL = 0x00001000,
  OEM_IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL  = 0x00002000,
  OEM_IPFLOW_MASK_NOMINAL_SDU_SIZE          = 0x00004000,
  OEM_IPFLOW_MASK_CDMA_FLOW_PRIORITY    = 0x00008000
} oem_ipflow_field_mask_enum_type;

typedef uint32 oem_ipflow_field_mask_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_ipflow_data_rate_format_type

Description:
   Specifies the data rate format

Definition:

typedef enum
{
  OEM_DATA_RATE_FORMAT_MIN_MAX_TYPE,
  OEM_DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE
} oem_ipflow_data_rate_format_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_ip_flow_type

Description:
   Specifies the ip flow

Definition:

typedef struct
{
   oem_ipflow_field_mask_type      field_mask;
   oem_ipflow_field_mask_type      err_mask;

   oem_ip_traffic_class_enum_type  trf_class;

   struct
   {
      oem_ipflow_data_rate_format_type  format_type;

      union
      {
         struct
         {
            uint32      max_rate;
            uint32      guaranteed_rate;
         } min_max;

         struct
         {
            uint32      peak_rate;
            uint32      token_rate;
            uint32      size;
         } token_bucket;
      } format;
   } data_rate;

   uint32        latency;
   uint32        latency_var;

   struct
   {
      uint16      multiplier;
      uint16      exponent;
   } pkt_err_rate;

   uint32      min_policed_pkt_size;
   uint32      max_allowed_pkt_size;

   struct
   {
      boolean  is_fixed;
      uint32   size;
   } nominal_sdu_size;

   struct
   {
      oem_umts_residual_ber_enum_type         res_ber;
      oem_umts_trf_handling_pri_enum_type     trf_pri;
   } umts_params;

   struct
   {
      oem_cdma_flow_spec_profile_id_type  profile_id;
      oem_cdma_flow_priority_type         flow_priority;
   } cdma_params;

   struct
   {
      oem_wlan_user_pri_enum_type     user_priority;
      uint32                          min_service_interval;
      uint32                          max_service_interval;
      uint32                          inactivity_interval;
   } wlan_params;

} oem_ip_flow_type;

Comments:
   1. A bit in field_mask shall be set for each parameter specified.
      If a bit is not set, a default value for the corresponding parameter
      is used.

   2. If one or more parameters are incorrrectly specified, the bits in err_mask
      indicate those parameters.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_ip_flow_spec_type

Description:
   Specifies a IP flow spec

Definition:

typedef struct
{
   oem_ip_flow_type      req_flow;         // Required flow type
   oem_ip_flow_type      min_req_flow;     // Minimum required flow type
   uint8                 num_aux_flows;    // # of auxiliary flows in the list
   oem_ip_flow_type *    aux_flow_list_ptr;// Ptr to array of auxiliary flows
} oem_ip_flow_spec_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_ipfltr_ip4_hdr_field_mask_type

Description:
   Specifies a IPv4 header field mask

Definition:

typedef enum
{
  OEM_IPFLTR_MASK_IP4_SRC_ADDR      = 0x01,
  OEM_IPFLTR_MASK_IP4_DST_ADDR      = 0x02,
  OEM_IPFLTR_MASK_IP4_NEXT_HDR_PROT = 0x04,
  OEM_IPFLTR_MASK_IP4_TOS           = 0x08
} oem_ipfltr_ip4_hdr_field_mask_enum_type;

typedef uint8 oem_ipfltr_ip4_hdr_field_mask_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   oem_ipfltr_ip6_hdr_field_mask_type

=======================================================================

oem_ipfltr_ip6_hdr_field_mask_type

Description:
   Specifies an IPv6 header field mask

Definition:

typedef enum
{
   OEM_IPFLTR_MASK_IP6_SRC_ADDR      = 0x01,
   OEM_IPFLTR_MASK_IP6_DST_ADDR      = 0x02,
   OEM_IPFLTR_MASK_IP6_NEXT_HDR_PROT = 0x04,
   OEM_IPFLTR_MASK_IP6_TRAFFIC_CLASS = 0x08,
   OEM_IPFLTR_MASK_IP6_FLOW_LABEL    = 0x10
} oem_ipfltr_ip6_hdr_field_mask_enum_type;

typedef uint8 oem_ipfltr_ip6_hdr_field_mask_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   oem_ipfltr_ip4_hdr_field_mask_type

=======================================================================

oem_ipfltr_tcp_hdr_field_mask_type

Description:
   Specifies a TCP header field mask

Definition:

typedef enum
{
  OEM_IPFLTR_MASK_TCP_SRC_PORT      = 0x01,
  OEM_IPFLTR_MASK_TCP_DST_PORT      = 0x02
} oem_ipfltr_tcp_hdr_field_mask_enum_type;

typedef uint8 oem_ipfltr_tcp_hdr_field_mask_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_ipfltr_udp_hdr_field_mask_type

Description:
   Specifies a UDP header field mask

Definition:

typedef enum
{
  OEM_IPFLTR_MASK_UDP_SRC_PORT      = 0x01,
  OEM_IPFLTR_MASK_UDP_DST_PORT      = 0x02
} oem_ipfltr_udp_hdr_field_mask_enum_type;

typedef uint8 oem_ipfltr_udp_hdr_field_mask_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_ip_filter_type

Description:
   Specifies the IP filter parameters for an IP filter type.

Definition:

typedef struct
{
  // Mandatory Parameter - IP version of the filter (v4 or v6)
  oem_ip_version_enum_type  ip_vsn;

  union
  {
    struct
    {
      oem_ipfltr_ip4_hdr_field_mask_type      field_mask; // In mask
      oem_ipfltr_ip4_hdr_field_mask_type      err_mask;     // Out mask

      struct
      {
        struct oem_in_addr  addr;
        struct oem_in_addr  subnet_mask;
      } src;

      struct
      {
        struct oem_in_addr  addr;
        struct oem_in_addr  subnet_mask;
      } dst;

      struct
      {
        uint8 val;
        uint8 mask;
      } tos;

      uint8 next_hdr_prot;
    } v4;

    struct
    {
       oem_ipfltr_ip6_hdr_field_mask_type      field_mask;  // In mask
       oem_ipfltr_ip6_hdr_field_mask_type      err_mask;    // Out mask

       struct
       {
          struct oem_in6_addr  addr;
          uint8                prefix_len;
       } src;

       struct
       {
          struct oem_in6_addr  addr;
          uint8                prefix_len;
       } dst;

       struct
       {
          uint8 val;
          uint8 mask;
       } trf_cls;

       uint32   flow_label;
       uint8    next_hdr_prot;   // This is transport level protocol header
    } v6;
  } ip_hdr;

  // next_hdr_prot field in v4 or v6 hdr must be set to specify a
  // parameter from the next_prot_hdr
  union
  {
    struct
    {
      oem_ipfltr_tcp_hdr_field_mask_type      field_mask;  // In mask
      oem_ipfltr_tcp_hdr_field_mask_type      err_mask;    // Out mask

      struct
      {
        uint16    port;
        uint16    range;
      } src;

      struct
      {
        uint16    port;
        uint16    range;
      } dst;
    } tcp;

    struct
    {
      oem_ipfltr_udp_hdr_field_mask_type      field_mask;  // In mask
      oem_ipfltr_udp_hdr_field_mask_type      err_mask;    // Out mask

      struct
      {
        uint16    port;
        uint16	   range;
      } src;

      struct
      {
        uint16    port;
        uint16    range;
      } dst;
    } udp;

  } next_prot_hdr;
} oem_ip_filter_type;

typedef struct
{
  uint8                  num_filters;      // Num filters in the list
  oem_ip_filter_type    *list_ptr;         // List of filters
} oem_ip_filter_spec_type;


Comments:
  1. A bit in field_mask shall be set for each parameter specified,
     if a bit is not set the corresponding parameter is ignored.
  2. If one or more parameters are incorrectly specified,
     the bits in err_mask indicate those parameters on return.
  3. Each filter will have an IP version associated with it which is either
     v4 or v6 and shall be specified by ip_vsn.
  4. If a parameter from next header protocol (TCP, UDP etc.) is specified
     in a filter, the Protocol Number for v4 filters or Next Header for
     v6 filters shall be specified.
  5. In most cases IP source address in Tx filters and IP destination address
     in Rx filters is not required to be specified.
  6. IP destination address in Tx filters and IP source address in Rx filters
     can be specified as address ranges using the subnet_mask (for v4 filters)
     or prefix_len (for v6 filters) parameters.
     A subnet_mask may be set to all 1's or prefix_len may be set to 128
     to specify a single address value.
  7. A nonzero value for range may be set to specify a range of port numbers
     starting from the value specified in port [port to port+range],
     otherwise range shall be set to zero.
  8. The following fields shall be specified in network byte order:
      a. IPv4 addresses
      b. IPv4 subnet masks
      c. IPv6 addresses (prefix length shall be in host order)
      d. TCP and UDP port numbers (port ranges shall be in host order)
      e. IPv6 flow label

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_ip_filter_spec_type

Description:
   Specifies a IP filter spec

Definition:

typedef struct
{
  uint8                  num_filters;      // Num filters in the list
  oem_ip_filter_type    *list_ptr;         // List of filters
} oem_ip_filter_spec_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_qos_spec_field_mask_type

Description:
   Specifies a QoS spec field mask

Definition:

typedef enum
{
   OEM_QOS_MASK_RX_FLOW                = 0x001,
   OEM_QOS_MASK_RX_MIN_FLOW            = 0x002,
   OEM_QOS_MASK_RX_AUXILIARY_FLOWS     = 0x004,
   OEM_QOS_MASK_TX_FLOW                = 0x008,
   OEM_QOS_MASK_TX_MIN_FLOW            = 0x010,
   OEM_QOS_MASK_TX_AUXILIARY_FLOWS     = 0x020,
   OEM_QOS_MASK_RX_FLTR_MODIFY         = 0x040,
   OEM_QOS_MASK_TX_FLTR_MODIFY         = 0x080
} oem_qos_spec_field_mask_enum_type;

typedef uint16 oem_qos_spec_field_mask_type;

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

oem_qos_spec_type

Description:
   Specifies a QoS spec

Definition:

typedef struct
{
  oem_qos_spec_field_mask_type  field_mask;

  struct
  {
    oem_ip_flow_spec_type     flow_template;    // traffic flow template
    oem_ip_filter_spec_type   fltr_template;    // traffic filter template
  } rx;

  struct
  {
    oem_ip_flow_spec_type		flow_template;  // traffic flow template
    oem_ip_filter_spec_type	fltr_template;  // traffic filter template
  } tx;
} oem_qos_spec_type;

Comments:
   1. For each direction for which the QOS is requested, corresponding bit in
      field_mask shall be set.
   2. For each direction for which the QOS is requested, a filter spec containing
      one or more filters belonging to same IP version shall be specified.
   3. For each direction for which the QOS is requested, a flow spec with the
      req_flow type shall be specified which is the most suitable flow type.
   4. For each direction for which the QOS is requested, the min_req_flow may be
      specified which is the minimum required flow type.
      It is invalid to only specify min_req_flow type without specifying
      req_flow type.
   5. For each direction for which the QOS is requested, the aux_flow_list_ptr
      may be specified which is a set of num_aux_flows flow types
      (in decreasing order of desirability) in between req_flow and min_req_flow.
      (Introduced BREW Client 3.1.5).

Version:
   Introduced BREW Client 3.1

See Also:
   None

======================================================================

oem_primary_qos_spec_type

Description:
   Specifies a Primary QoS spec

Definition:

typedef struct
{
  oem_qos_spec_field_mask_type   field_mask;
  oem_ip_flow_spec_type          rx_flow_template;
  oem_ip_flow_spec_type          tx_flow_template;
} oem_primary_qos_spec_type;


Comments:
   1. For each direction in which the Primary QoS is requested to be modified,
      corresponding bit in field_mask shall be set.
   2. For each direction in which the Primary QoS is requested to be modified,
      a flow spec with the req_flow type shall be specified which is the most
      suitable flow type.
   3. For each direction in which the Primary QoS is requested to be modified,
      the min_req_flow may be specified which is the minimum required flow type.
      It is invalid to only specify min_req_flow type without specifying
      req_flow type.
   4. For each direction in which the Primary QoS is requested to be modified,
      the aux_flow_list_ptr may be specified which is a set of num_aux_flows
      flow types (in decreasing order of desirability) in between req_flow and
      min_req_flow.

Version:
   Introduced BREW Client <3.1.5> SP01

See Also:
   None

====================================================================== */

#endif /* OEMSOCK_QOSSPEC_H */
