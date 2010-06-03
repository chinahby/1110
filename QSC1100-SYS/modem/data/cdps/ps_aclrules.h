#ifndef PS_ACLRULES_H
#define PS_ACLRULES_H
/*===========================================================================

                   P R O T O C O L   S E R V I C E S

                A C C E S S   C O N T R O L   L I S T S

                    R U L E   D E F I N I T I O N S

GENERAL DESCRIPTION
  Access control lists are used to define IP packet filters. These
  filters can be used e.g. for firewall functionality, address 
  translation for selected ranges, or to build policy based routing 
  rules.
  ACLs are defined as integer functions, which take information
  about the IP packet filtered as parameters. The return value
  is either '0' for a deny (packet drop) or the highest priority
  rule class for which a match was found. 
  This file defines a common ACL function prototype as well as some 
  macros to specify ACLs more similar to the usual language based 
  syntax on routers and hosts.
  The following is an example of an ACL:

  ACL_DEF( inbound_filter )
  ACL_START
    DENY( ACL_DEFAULT_CLASS, DST_ADDR_IS_IN( IPV4( 127,0,0,1 ), 0xffffffff ) )
    DENY( ACL_DEFAULT_CLASS, SRC_ADDR_IS_IN( LOCALHOST, 0xffffffff ) )
    PASS( ACL_DEFAULT_CLASS, DST_ADDR_IS_IN( LOCALHOST, 0xffffffff ) )
  ACL_END

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
                      
  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_aclrules.h_v   1.11   12 Feb 2003 20:35:28   omichael  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_aclrules.h#2 $ $DateTime: 2008/06/25 10:34:15 $ $Author: praveenp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/08   pp      Added a rule for arbitration manager support.
05/27/03   om      Added rules for use by IPSec.
05/13/03   usb     Added new rules DIFFERENT_IFACE_REQUIRED and 
                   IFACE_IS_PREF_MODE. Renamed IFACE_IS to REQUIRD_IFACE_IS
                   & IFACE_NAME_IS to THIS_IFACE_NAME_IS. Validating
                   this_iface_ptr at the begining of ACL function.
02/12/03   om      Fixed IFACE_IS() rule, added IFACE_NAME_IS() rule.
11/25/02   usb     Modified rules which perform NULL ptr check
                   to return FALSE instead of ACL_DENY. Init acl_ret_val to
                   ACL_DEFAULT_CLASS instead of 0.
11/19/02    om     Changed ACL_DENY to be -1 (separate from DEFAULT_CLASS).
11/19/02    usb    Added new rule to check if an iface is in use, removed 
                   class parameter from DENY rule.
10/10/02    rc     Added new rule for checking if the context is 
                   sharable (UMTS).
09/25/02    om     Added NULL IP info or policy info resulting in ACL_DENY.
09/13/02    om     Added IP address rules for inbound ACL.
09/09/02    om     Updates for ACL postprocessors.
08/30/02    om     Updated IFACE related rules.
03/08/02    om     Created Module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "assert.h"

#include "ps_acl.h"

/* <EJECT> */ 
/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

/*-------------------------------------------------------------------------
  Macros to specify ACL rules
-------------------------------------------------------------------------*/

/*** Logical expressions ***/
#define AND             &&
#define OR              ||
#define NOT( arg )      (! (arg))
#define ALL             TRUE

/*** Define the result of a rule (pass packet or drop it) ***/
#define ACL_DENY        -1
#define PASS( class, rule )   \
  if ( rule ) acl_ret_val = MAX( acl_ret_val, ACL_PRIORITY( class ) ); 
#define PASS_QUICK( class, rule )   \
  if ( rule ) return MAX( acl_ret_val, ACL_PRIORITY( class ) );
#define DENY( rule )   \
  if ( rule ) return ACL_DENY;

/*** ACL function input parameter validation rules ***/
#define IS_IP_INFO_NULL() ( ipfilter_info_ptr == NULL )
#define IS_POL_INFO_NULL() ( policy_info_ptr == NULL )

/*** Pointer checks on the policy and IP filter info */
#define USE_IP_INFO( rule )  \
  ( (ipfilter_info_ptr) ? (rule) : FALSE )
#define USE_POL_INFO( rule )  \
  ( (policy_info_ptr) ? (rule) : FALSE )
#define USE_IP_AND_POL_INFO( rule )  \
  ( (ipfilter_info_ptr && policy_info_ptr) ? (rule) : FALSE )

/*** Simpler access to IP header and socket info ***/
#define IP_VERSION_IS( ipvsn )   \
  USE_IP_INFO( ipfilter_info_ptr->ip_hdr.v4.version == ipvsn )

#define REQUIRED_IFACE_IS( ifname )   \
  USE_POL_INFO( (policy_info_ptr->if_id & ifname) == ifname )

/*** Required i/f name explicitly specified, different from this one ***/
#define DIFFERENT_IFACE_REQUIRED()   \
  USE_POL_INFO( (policy_info_ptr->if_id & IFACE_MASK) ?  \
    ((policy_info_ptr->if_id & ((ps_iface_type*) this_if_ptr)->name) !=  \
      ((ps_iface_type*) this_if_ptr)->name) : FALSE )

#define THIS_IFACE_NAME_IS( ifname )   \
  ( ((ps_iface_type*) this_if_ptr)->name == ifname )

#define IFACE_IS_UP( weight )   \
  ( ((ps_iface_type*) this_if_ptr)->if_state == IF_STATE_UP) ?              \
    ((acl_ret_val = ACL_PRIORITY( class ) + weight)                         \
     == (ACL_PRIORITY( class ) + weight))                                   \
    : 0 )

#define IFACE_SUPPORTS_ROUTE_ON_DEMAND( )                                   \
  USE_POL_INFO( policy_info_ptr->orig_from_sock                             \
                  | ((ps_iface_type*) this_if_ptr)->can_route_on_demand )

#define SRC_ADDR_IS_IN( netid, netmask )  \
  USE_IP_INFO( (ipfilter_info_ptr->ip_hdr.v4.source & netmask) == netid )
#define DST_ADDR_IS_IN( netid, netmask )  \
  USE_IP_INFO( (ipfilter_info_ptr->ip_hdr.v4.dest & netmask) == netid )

#define SRC_PORT_IS_IN( range_start, range_end )
#define DST_PORT_IS_IN( range_start, range_end )

#define DST_IS_BRDCST  USE_IP_INFO( ipfilter_info_ptr->is_brdcst )

#define PROTOCOL_IS_TCP   \
  USE_IP_INFO( ipfilter_info_ptr->ip_hdr.protocol == TCP_PTCL )
#define PROTOCOL_IS_UDP   \
  USE_IP_INFO( ipfilter_info_ptr->ip_hdr.protocol == UDP_PTCL )
#define PROTOCOL_IS_ICMP  \
  USE_IP_INFO( ipfilter_info_ptr->ip_hdr.protocol == ICMP_PTCL )
#define PROTOCOL_IS_ESP   \
  USE_IP_INFO( ipfilter_info_ptr->ip_hdr.protocol == ESP_PTCL )

#define ICMP_TYPE_IS( type )  \
  USE_IP_INFO( ipfilter_info_ptr->icmp_type == type )
#define ICMP_CODE_IS( code )  \
  USE_IP_INFO( ipfilter_info_ptr->icmp_code == code )

#define TYPE_OF_SVC_IS( tos_id )  \
  USE_IP_INFO( ipfilter_info_ptr->ip_hdr.tos == tos_id )

#define LOCALHOST  \
  USE_IP_INFO( PS_IFACE_IP_V4_ADDR( ACL_IPFI_IF_PTR( ipfilter_info_ptr ) ) )

#define IPV4( arg1, arg2, arg3, arg4 )   \
  ((uint32)( ((arg1 & 0xff) << 24)                                          \
           | ((arg2 & 0xff) << 16)                                          \
           | ((arg3 & 0xff) << 8)                                           \
           | ((arg4 & 0xff)) ) )

#define PKT_IS_SECURE   \
  USE_IP_INFO( ipfilter_info_ptr->is_secure )

/*** Checks for policy related parameters ***/
#define QOS_IS( qos_type )  \
  USE_POL_INFO( policy_info_ptr->qos == qos_type )

/*** UMTS PDP Context Configurations (for now always true) ***/
#define QOS_PDP_CTX_CFG_IS_SUPPORTED( _yes_or_no, _post_processor_fptr )    \
  USE_POL_INFO( _yes_or_no ?                                                \
      ((policy_info_ptr->post_process_fptr = _post_processor_fptr) != NULL) \
      : FALSE )
#define QOS_PDP_CTX_IS_SHARED( _ctx_is_sharable )  \
  USE_POL_INFO( _ctx_is_sharable                                            \
                &&                                                          \
                (policy_info_ptr->pdp_ctx_cfg_ptr == NULL) )

#define IFACE_IS_IN_USE( in_use_fptr, post_proc_fptr )                      \
  USE_POL_INFO( in_use_fptr( this_if_ptr ) ? TRUE                           \
      : ((policy_info_ptr->post_process_fptr = post_proc_fptr) == NULL) )
      
/*** Determine if this iface is preferred mode for packet data calls */
/* Function ptr is defined as - boolean (*fptr)(void *) ***/
#define IFACE_IS_PREF_MODE(pref_mode_fptr, arg)  pref_mode_fptr( (arg) )
  
#define APP_PRIORITY_CHECK(proc_priority_fptr)                              \
        proc_priority_fptr(policy_info_ptr,this_if_ptr)

/*---------------------------------------------------------------------------
  ACL function macros
---------------------------------------------------------------------------*/

/*** Syntactical definition as a group of rules via a function ***/
#define ACL_EXEC( name, arg1, arg2, arg3 ) name( arg1, arg2, arg3 )
#define ACL_DEF( name )   \
  int name                                                                  \
  (                                                                         \
    acl_ipfilter_info_type*  ipfilter_info_ptr,                             \
    acl_policy_info_type*    policy_info_ptr,                               \
    void*                    this_if_ptr                                    \
  )
#define ACL_START  { int acl_ret_val = ACL_DEFAULT_CLASS;                   \
                     if (policy_info_ptr != NULL)                           \
                     policy_info_ptr->post_process_fptr = NULL;             \
                     if(this_if_ptr == NULL)                                \
                     {                                                      \
                       ASSERT(0);                                           \
                       return ACL_DENY;                                     \
                     }
  
#define ACL_END    return acl_ret_val; } 

/*** Syntactical declaration of the ACL's postprocessor ***/
#define ACL_POST_PROC_DEF( name )  \
  void name                                                                 \
  (                                                                         \
    acl_ipfilter_info_type*  ipfilter_info_ptr,                             \
    acl_policy_info_type*    policy_info_ptr,                               \
    void*                    this_if_ptr                                    \
  )

/*** Map a rule's class into a routing priority ***/
#define ACL_PRIORITY( class )  acl_class_priorities[class]

/* <EJECT> */ 
/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* Classes of ACL rules to prioritize multiple matches:        */
/* The DEFAULT_CLASS is used to specify the default fallback   */
/* priority, e.g., for routing ACLs the decision between two   */
/* DEFAULT_CLASS priorities would be based on IP address based */
/* routing (always priority 1). The additional rule classes    */
/* allow tp prioritize between individual policies, e.g., a    */
/* directly specified interface type would have a higher       */
/* priority than a QOS type or a source IP address.            */
typedef enum
{
  ACL_DEFAULT_CLASS = 0,
  ACL_IFGROUP_CLASS = 1,
  ACL_IFNAME_CLASS  = 2,
  ACL_IPADDR_CLASS  = 3,
  ACL_IPPORT_CLASS  = 4,
  ACL_QOS_CLASS     = 5,
  ACL_APP_CLASS     = 6,
  ACL_NUM_CLASSES
} acl_rule_class_enum_type;

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Priority lookup table to map rule classes into routing priorities */
extern int acl_class_priorities[ACL_NUM_CLASSES];


#endif    /* PS_ACLRULES_H */

