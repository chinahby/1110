#ifndef PS_ACL_H
#define PS_ACL_H
/*===========================================================================

                   P R O T O C O L   S E R V I C E S

                A C C E S S   C O N T R O L   L I S T S

                          D E F I N I T I O N S

GENERAL DESCRIPTION
  Access control lists are used to define IP packet filters. These
  filters can be used e.g. for firewall functionality, address 
  translation for selected ranges, or to build policy based routing 
  rules.
  ACLs are defined as integer functions, which take information
  about the IP packet filtered as parameters. The return value
  is either '0' for a deny (packet drop) or the highest priority
  rule class for which a match was found. 
  This file defines a common ACL function prototype and the data
  types to pass policy information to the ACL rules.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_acl.h_v   1.6   26 Nov 2002 14:49:14   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_acl.h#3 $ $DateTime: 2008/07/03 12:28:16 $ $Author: praveenp $
                              
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/08    pp     Added data_session_profile_id to policy info 
                   [OMH 1b dev].
06/06/03    om     Added ipsec_required flag.
05/27/03    om     Added is_secure flag to IP info (used by IPSec).
10/15/02    om     Added disabled flag to ACL type.
09/25/02    om     Added forced routing to policy info.
09/24/02    om     Added UMTS PDP contect config to policy info.
09/09/02    om     Updates for ACL postprocessors.
08/30/02    om     Updated ipfilter and policy info definitions.
03/08/02    om     Created Module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#include "ps_iphdr.h"

/* <EJECT> */ 
/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* Pre-typedef's */
typedef struct acl_ipfilter_info_s acl_ipfilter_info_type;
typedef struct acl_policy_info_s   acl_policy_info_type;

/*-------------------------------------------------------------------------
  ACL function types
-------------------------------------------------------------------------*/

/* An actual ACL's rules are grouped as this function type */
typedef int (*acl_fptr_type)
(
  acl_ipfilter_info_type*  ipfilter_info_ptr,
  acl_policy_info_type*    policy_info_ptr,
  void*                    this_if_ptr
);

/* Some ACL's require postprocessing after the routing decision */
/* this is the post-processor's function type.                  */
typedef void (*acl_post_process_fptr_type)
(
  acl_ipfilter_info_type*  ipfilter_info_ptr,
  acl_policy_info_type*    policy_info_ptr,
  void*                    this_if_ptr
);

/*-------------------------------------------------------------------------
  ACL data types
-------------------------------------------------------------------------*/

/* IP packet information needed by ipfilter (IP header + extra info) */
struct acl_ipfilter_info_s
{
  /* IP header information from ip.h */
  ip_version_enum_type  ip_vsn;
  union
  {   
    struct ip v4;
    /* Add v6 header here when supported */
  } ip_hdr;

  /* Info about protocols above IP */
  union
  {
    struct
    {
      uint8       type;              /* ICMP message type                  */
      uint8       code;              /* ICMP message code                  */
    }           icmp;
    struct
    {
      uint16      src_port;          /* Source UDP port number             */
      uint16      dst_port;          /* Destination UDP port number        */
    }           udp;
    struct
    {
      uint16      src_port;          /* Source TCP port number             */
      uint16      dst_port;          /* Destination TCP port number        */
    }           tcp;
  }           ptcl_info;             /* Protocol specific information      */

  boolean     ipsec_required;        /* Packet need sESP processing        */
  boolean     is_secure;             /* Packet is secured by IPSec         */
  boolean     is_brdcst;             /* Packet has a broadcast destination */
  boolean     is_local;              /* Identifies packets for local host  */
  
  void*       if_ptr;                /* Pointer to xmit / rcv interface    */

  /* Accessor macro to cast the interface pointer correctly */
  #define ACL_IPFI_IF_PTR( _ipfi_ptr )  \
            ((ps_iface_type*) ((_ipfi_ptr)->if_ptr))

  void*       if_cache;              /* Interface from socket route cache */

  /* Accessor macro to cast the cache pointer correctly */
  #define ACL_IPFI_IF_CACHE( _ipfi_ptr )    \
            ((ps_iface_type*) ((_ipfi_ptr)->if_cache))

};

/* Policy related information about an IP packet for routing ACLs */
typedef enum
{
  ACL_QOS_UNSPEC = 0,
  ACL_QOS_OPT_COST,
  ACL_QOS_OPT_THROUGHPUT,
  ACL_QOS_OPT_LATENCY
} acl_qos_enum_type;

/* UMTS PDP config info struct (TBD) */
typedef void* acl_pdp_ctx_cfg_type;

struct acl_policy_info_s
{
  /* General policy based routing information */
  uint32                   if_id;      /* Interface name directly specified */
  acl_qos_enum_type        qos;        /* Desired QOS type for this packet  */
  acl_pdp_ctx_cfg_type     pdp_ctx_cfg_ptr; /* UMTS PDP context config info */
  uint32                   gateway_v4_addr; /* IPv4 gateway a. for this pkt */
  boolean                  orig_from_sock; /* Generated locally (not fwd'd) */

  /* Indication whether postprocessing after the selection is required */
  acl_post_process_fptr_type  post_process_fptr;

  /* Indication whether routing should check the IF state */
  boolean                  force_route;
  int32                    data_session_profile_id;
};

/* ACL handles provided by the originator allow         */
/* organization in a linked list by the routing module. */
typedef struct acl_s acl_type;
struct acl_s
{
  acl_fptr_type   acl_fptr;
  void*           if_ptr;
  boolean         disabled;
  uint32          pkt_count;
  acl_type*       next_ptr;
};

#define ACL_IF_PTR( _a_ptr )  ((ps_iface_type*) ((_a_ptr)->if_ptr))

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


#endif    /* PS_ACL_H */

