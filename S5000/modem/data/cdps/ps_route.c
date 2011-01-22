/*===========================================================================

                    P R O T O C O L   S E R V I C E S

                    I P   R O U T I N G   M O D U L E

                          D E F I N I T I O N S

GENERAL DESCRIPTION
  The IP routing module enables IP address based routing between
  up interfaces as well as policy based routing.
  Routing decisions are based on policies defined as ACLs with
  IP address routing as the default fallback mechanism.

  ACLs (Access Control Lists) provide a set of rules to define
  how suitable an interface is for transmission of a given IP
  packet. The rules allow consideration of parameters such as
  port numbers, quality-of-service identifiers, air-interface
  type and the like and hence allow routing decisions beyond the
  regular address based routing. A set of rules is also referred
  to as the interfaces routing policy. The syntax of ACL rules is
  defined in ps_aclrules.h.

  This file provides the API to the routing module. It is independent
  of the IP version used. The API functions differentiate between
  IPv4 and IPv6 and call the appropriate internal functions (routei_...),
  which are version specific.

EXTERNALIZED FUNCTIONS
  route_add()         - Add a route to the routing table
  route_delete()      - Remove a route from the routing table
  route_update()      - Update an existing route in the routing table
  route_get()         - Find a route via ACLs and the routing table
  route_flush()       - Flush all routes or all routes for one iface
  route_acl_add()     - Add an ACL for a specific interface
  route_acl_delete()  - Remove an ACL for a specific interface
  route_acl_control() - En- / disable an existing ACL temporarily

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
  
  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_route.c_v   1.14   24 Feb 2003 10:56:00   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_route.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/21/03    usb    Fixed ACL deny list parsing.
02/20/03    om     More comments, added FEATURE_DATA_PS, fixed ACL deny list.
02/12/03    om     Added MSGs and consistency check to route_add,
                   fixed INTLOCKs.
11/25/02    usb    Init best_priority (in route_get()) to ACL_DEFAULT_CLASS 
                   instead of 0.
11/19/02    om     Added ACL_DENY list to prevent table lookups from 
                   overriding ACL results.
11/05/02    om     Changed ACL en-disable into acl_control, allow iface Id.
10/15/02    om     Added ACL en-/disabling functions.
09/25/02    om     Updates for forced routing.
09/24/02    om     Added pointer check to post-processor call.
09/18/02    om     Fixed call to ACL postprocessor.
09/09/02    om     Updates for ACL postprocessors.
08/30/02    om     Updated signature of routei_delete_v4().
08/13/02    mvl    updated ps_iface state macros for name change
03/08/02    om     Created Module
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#ifdef FEATURE_DATA_PS

#include "msg.h"
#include "err.h"
#include "assert.h"
#include "errno.h"
#include "rex.h"

#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ps_route.h"
#include "ps_routei.h"
#include "psglobal.h"


/*===========================================================================

                        DATA DEFINITIONS

===========================================================================*/

/* Temporary until REX's errno is there */
errno_enum_type  _errno;

/* Scratch space used by route_get to keep track of */
/* disabled and ACL_DENY'd interfaces               */
ps_iface_type*   _route_deny_list[ROUTEI_V4_ROUTING_TABLE_SIZE];


/*===========================================================================

                      FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION   ROUTE_ADD()

DESCRIPTION
  This function allows to add a route to the DMSS routing
  table.

DEPENDENCIES
  None
  
PARAMETERS
  type        - Specifies whether it is a host or network route
  destination - IP address of the routing destination in network order
  netmask     - IP network mask the destination address belongs to
  gateway     - IP address of the routing gateway in network order
  if_ptr      - Specifies a local interface to route to
  metric      - Specifies the hop metric of this route
  cost_ptr    - Pointer to route cost (optional, set to zero for n/a)

RETURN VALUE
  0 for success, -1 for failure (_errno contains error code).
  
SIDE EFFECTS
  None
===========================================================================*/
int route_add
(
  route_type_enum_type  type,
  ip_addr_type          destination,
  ip_addr_type          netmask,
  ip_addr_type          gateway,
  ps_iface_type*        if_ptr,
  uint8                 metric,
  uint16*               cost_ptr
)
{
  int index;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Only IPv4 is supported for now */
  ASSERT( destination.type == IPV4_ADDR );
  ASSERT( netmask.type == IPV4_ADDR );
  ASSERT( gateway.type == IPV4_ADDR );

  /* Check if an entry for this route already exists */
  for ( index=0; index<routei_v4_routing_table.table_size_used; index++ )
  {
    if ( (routei_v4_routing_table.entry[index].dst_addr 
          ==
          destination.addr.v4)
         &&
         (routei_v4_routing_table.entry[index].gateway 
          == 
          gateway.addr.v4) )
    {
      _errno = E_NOT_ALLOWED;
      return -1;
    }
  }

  /* Allocate a new entry for this route ... */
  if ( (index = routei_alloc_v4( type, 
                                 destination.addr.v4, 
                                 metric,
                                 cost_ptr )) 
       == -1 )
  {
    ERR( "RT: Table is full !", 0, 0, 0 );
    _errno = E_NOT_AVAILABLE;
    return -1;
  }

  /* ... and fill in the routing info */
  MSG_HIGH( "RT: Adding 0x%x/0x%x gw 0x%x",
            ntohl( destination.addr.v4 ),
            ntohl( netmask.addr.v4 ),
            ntohl( gateway.addr.v4 ) );
  switch( if_ptr->name )
  {
    case CDMA_SN_IFACE:
      MSG_HIGH( "    via 1x/SN if 0x%x (metric %d)", if_ptr, metric, 0 );
      break;
    case CDMA_AN_IFACE:
      MSG_HIGH( "    via 1x/AN if 0x%x (metric %d)", if_ptr, metric, 0 );
      break;
    case UMTS_IFACE:
      MSG_HIGH( "    via UMTS if 0x%x (metric %d)", if_ptr, metric, 0 );
      break;
    case SIO_IFACE:
      MSG_HIGH( "    via SIO if 0x%x (metric %d)", if_ptr, metric, 0 );
      break;
    case LO_IFACE:
      MSG_HIGH( "    via LO if 0x%x (metric %d)", if_ptr, metric, 0 );
      break;
    default:
      MSG_HIGH( "    via if 0x%x (metric %d)", if_ptr, metric, 0 );
  }
  routei_v4_routing_table.entry[index].dst_addr = destination.addr.v4;
  routei_v4_routing_table.entry[index].dst_netmask = netmask.addr.v4;
  routei_v4_routing_table.entry[index].gateway = gateway.addr.v4;
  routei_v4_routing_table.entry[index].if_ptr = if_ptr;

  /* Set the flags for this route according to it's type */
  switch( type )
  {
    case ROUTE_DEFAULT:
      routei_v4_routing_table.entry[index].flags.gateway = TRUE;
      routei_v4_routing_table.entry[index].flags.host = FALSE;
      break;

    case ROUTE_HOST:
      routei_v4_routing_table.entry[index].flags.gateway = 
        ! ( (destination.addr.v4 & netmask.addr.v4) 
            ==
            (PS_IFACE_IP_V4_ADDR( if_ptr ) & netmask.addr.v4) );
      routei_v4_routing_table.entry[index].flags.host = TRUE;
      break;

    case ROUTE_NET:
      routei_v4_routing_table.entry[index].flags.gateway =
        ! ( (destination.addr.v4 & netmask.addr.v4)
            ==
            (PS_IFACE_IP_V4_ADDR( if_ptr ) & netmask.addr.v4) );
      routei_v4_routing_table.entry[index].flags.host = FALSE;
      break;

    default:
      ERR( "RT: Unknown route type %d", type, 0, 0 );
  }
  routei_v4_routing_table.entry[index].flags.icmp_created = FALSE;
  routei_v4_routing_table.entry[index].flags.icmp_modified = FALSE;

  routei_v4_routing_table.entry[index].metric = metric;
  routei_v4_routing_table.entry[index].flags.metric_is_valid = TRUE;

  /* Cost is an optional dimensionless parameter */
  if ( cost_ptr )
  {
    routei_v4_routing_table.entry[index].cost = *cost_ptr;
    routei_v4_routing_table.entry[index].flags.cost_is_valid = TRUE;
  }
  else
  {
    routei_v4_routing_table.entry[index].cost = 0;
    routei_v4_routing_table.entry[index].flags.cost_is_valid = FALSE;
  }

  /* Sanity check the table */
  if ( routei_check_v4_table() != 0 )
  {
    MSG_ERROR( "Rt: Deleting bad v4 entry %d", index, 0, 0 );
    routei_free_v4( index );
  }

  return 0;

} /* route_add() */


/*===========================================================================

FUNCTION   ROUTE_DELETE

DESCRIPTION
  This function allows to remove a route from the DMSS
  routing table.

DEPENDENCIES
  None
  
PARAMETERS
  description - IP address of the routing destination in network order
  gateway     - IP address of the routing gateway in network order

RETURN VALUE
  0 for success, -1 for failure (_errno contains error code).
  
SIDE EFFECTS
  None
===========================================================================*/
int route_delete
(
  ip_addr_type   destination,
  ip_addr_type*  gateway
)
{
  int ret_val = -1;
  int index;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* For now we only support IPv4 */
  ASSERT( destination.type == IPV4_ADDR );

  /* Ckeck if a specific route to this destination was given */
  if ( gateway == NULL )
  {
    MSG_HIGH( "Flushing all routes for dst 0x%x", 
              destination.addr.v4, 0, 0 );
  }
  else
  {
    ASSERT( gateway->type == IPV4_ADDR );
  }

  /* Find the entry for this route in the table and delete it. These */
  /* could be multiple entries with different gateways.              */
  _errno = E_NO_ROUTE;
  TASKLOCK();
  for ( ;; )
  {
    if ( (index = routei_get_v4_route( destination.addr.v4,
                                       (gateway ? &(gateway->addr.v4)
                                                : NULL) )) 
         == -1 )
    {
      break;
    }
    else
    {
      ret_val = 0;
    }
    
    routei_free_v4( index );
  }
  TASKFREE();

  return ret_val;

} /* route_delete() */


/*===========================================================================

FUNCTION   ROUTE_UPDATE

DESCRIPTION
  This function allows to update an existing entry in the
  DMSS routing table.

DEPENDENCIES
  None
  
PARAMETERS
  destination - IP address of the routing destination in network order
  gateway     - IP address of the routing gateway in network order
  if_ptr      - Specifies a local interface to route to (optional)
  metric      - Specifies the hop metric of this route
  cost_ptr    - Specifies the cost of this route (set to NULL for n/a)

RETURN VALUE
  0 for success, -1 for failure (_errno contains error code).
  
SIDE EFFECTS
  None
===========================================================================*/
int route_update
(
  ip_addr_type    destination, 
  ip_addr_type    gateway,
  ps_iface_type*  if_ptr,
  uint8*          metric_ptr,
  uint16*         cost_ptr
)
{
  int index;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure this route exists */
  if ( (index = routei_get_v4_route( destination.addr.v4,
                                     &(gateway.addr.v4) ))
       == -1 )
  {
    _errno = E_NO_ROUTE;
    return -1;
  }

  /* Update the entrie's TX interface if it is given */ 
  if ( if_ptr )
  {
    routei_v4_routing_table.entry[index].if_ptr = if_ptr;
  }

  /* Update the entrie's metric if it is given */ 
  if ( metric_ptr )
  {
    routei_v4_routing_table.entry[index].metric = *metric_ptr;
    routei_v4_routing_table.entry[index].flags.metric_is_valid = TRUE;
  }

  /* Update the entrie's cost if it is given */ 
  if ( cost_ptr )
  {
    routei_v4_routing_table.entry[index].cost = *cost_ptr;
    routei_v4_routing_table.entry[index].flags.cost_is_valid = TRUE;
  }

  return 0;

} /* route_update() */


/*===========================================================================

FUNCTION   ROUTE_UPDATE_LOCAL_ADDRESS

DESCRIPTION
  This function allows to update the local addres of an interface
  in the routing table (gateway field ).

DEPENDENCIES
  None
  
PARAMETERS
  local_addr   - Ptr to IP address of the routing gateway in network order
  if_ptr       - Ptr local interface route goes through
  _errno     - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (_errno contains error code).
  
SIDE EFFECTS
  None
===========================================================================*/
int route_update_local_addr
(
  ip_addr_type*   local_addr,
  ps_iface_type*  if_ptr
)
{
  _errno = E_NOT_SUPPORTED;
  return -1;

} /* route_update_local_addr() */


/*===========================================================================

FUNCTION   ROUTE_GET

DESCRIPTION
  This function allows to retrieve a route from the DMSS
  routing table.

DEPENDENCIES
  None
  
PARAMETERS
  ip_fi_ptr    - Ptr to packet's IP information
  pol_ptr      - Ptr to packet's policy information
  ip_addr_only - Don't use ACLs for routing decision if true
  gateway      - Ptr to IP address of the routing gateway in network order
  if_ptr       - Ptr local interface route goes through

RETURN VALUE
  0 for success, -1 for failure (_errno contains error code).
  
SIDE EFFECTS
  None
===========================================================================*/
int route_get
(
  acl_ipfilter_info_type*  ip_fi_ptr,
  acl_policy_info_type*    acl_pi_ptr,
  boolean                  ip_addr_based_routing,
  ip_addr_type*            gateway_ptr,
  ps_iface_type**          if_ptr
)
{
  acl_type*                   acl_ptr = NULL;   
  acl_type*                   acl_match_ptr = NULL;   
  acl_post_process_fptr_type  acl_post_process_fptr = NULL;
  ps_iface_type**             deny_list_ptr = _route_deny_list;
  int                         best_priority = ACL_DEFAULT_CLASS;
  int                         temp;
  boolean                     force_route = FALSE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set some default values and check for IPv4 */
  ASSERT( ip_fi_ptr->ip_vsn == IP_V4 );
  *if_ptr = NULL;
  if ( gateway_ptr )
  {
    gateway_ptr->type = IPV4_ADDR;
    gateway_ptr->addr.v4 = 0;
  }

  /* Check if this is forced routing through the cache */
  if ( acl_pi_ptr )
  {
    force_route = acl_pi_ptr->force_route;
  }

  /* Check if we have an available route cached in the IP filter info. */
  /* Use the cached entry only if the interface is not disabled or the */
  /* policy info specifies a forced route.                             */
  if ( ip_fi_ptr
       &&
       ACL_IPFI_IF_CACHE( ip_fi_ptr ) 
       &&
       ( force_route
         ||
         (ps_iface_state( ACL_IPFI_IF_CACHE( ip_fi_ptr ) ) != IFACE_DISABLED) )
     )
  {
    MSG_MED( "RT: Cache match (if = 0x%x[%d])",
             ACL_IPFI_IF_CACHE( ip_fi_ptr ),
             ACL_IPFI_IF_CACHE( ip_fi_ptr )->instance, 0 );
    *if_ptr = ACL_IPFI_IF_CACHE( ip_fi_ptr );
  }
  else
  {
    /* No cached entry, or the cache is invalid - do a lookup */
    TASKLOCK();

    /* First, try to do a policy based match if allowed*/
    if ( ! ip_addr_based_routing )
    {
      /* Loop over all regiestered ACLs to find the best match */
      for ( acl_ptr = routei_acl_list_ptr;
            acl_ptr != NULL;
            acl_ptr = acl_ptr->next_ptr )
      {
        /* Skip the ACL if it is temporarily disabled */
        if ( acl_ptr->disabled )
        {
          *(deny_list_ptr++) = ACL_IF_PTR( acl_ptr );
          continue;
        }

        /* Skip the ACL if the associated iface is disabled */
        if ( ps_iface_state( ACL_IF_PTR( acl_ptr ) ) == IFACE_DISABLED )
        {
          continue;
        }

        /* Find the current ACLs best match */
        temp = (acl_ptr->acl_fptr)( ip_fi_ptr, 
                                    acl_pi_ptr, 
                                    ACL_IF_PTR( acl_ptr ) );

        /* Check if the current ACL's match (priority) is better */
        /* than what we already found.                           */
        if ( temp > best_priority )
        {
          /* Save this ACL's info as a possible candidate */
          best_priority = temp;
          acl_match_ptr = acl_ptr;
          if ( acl_pi_ptr )
          {
            acl_post_process_fptr = acl_pi_ptr->post_process_fptr;
          }
        }
        else if ( temp == ACL_DENY )
        {
          /* If the ACL denied this IP packet, store it in the deny-list */
          *(deny_list_ptr++) = ACL_IF_PTR( acl_ptr );
        }
      }
    }
    
    /* Check if we found a matching route from the ACLs */
    if ( (acl_match_ptr != NULL) && (best_priority != ACL_DEFAULT_CLASS) )
    {
      MSG_MED( "RT: Found ACL match (if = 0x%x[%d])",
               ACL_IF_PTR( acl_match_ptr ),
               ACL_IF_PTR( acl_match_ptr )->instance, 0 );
      *if_ptr = ACL_IF_PTR( acl_match_ptr );
      acl_match_ptr->pkt_count++;

      /* Call the ACL's post-processing routine if required */
      if ( acl_post_process_fptr )
      {
        (*acl_post_process_fptr)( ip_fi_ptr, 
                                  acl_pi_ptr, 
                                  ACL_IF_PTR( acl_match_ptr ) );
      }

      /* Get the gateway address for this route if it's not  */
      /* a point-to-point link (NULL gateway pointer).       */
      if ( gateway_ptr )
      {
        if ( (temp = routei_get_v4_route( ip_fi_ptr->ip_hdr.v4.dest, NULL ))
             !=
             -1 )
        {
          gateway_ptr->addr.v4 = 
            routei_v4_routing_table.entry[temp].gateway;
        }
      }
    }
    else
    {
      /* Do an IP address based route lookup - if the resulting      */
      /* iface is disabled or denied from the ACL lookup route_get() */
      /* will just return E_NO_ROUTE. Adding this into the routing   */
      /* table lookup itself to cover multiple routes to the same    */
      /* destination is TBD.                                         */
      if ( (temp = routei_get_v4_route( ip_fi_ptr->ip_hdr.v4.dest, NULL )) 
           == 
           -1 )
      {
        MSG_ERROR( "RT: No route found for dst 0x%x",
                   ip_fi_ptr->ip_hdr.v4.dest, 0, 0 );
      }
      else
      {
        /*-------------------------------------------------------------------
          Check deny_list_ptr to see if this iface is allowed to route this
          IP pkt. deny_list_ptr points at the next available location which 
          does not have a valid iface entry. So we always decrement it by 
          one and start searching backwards until we have processed 
          the whole list.
        -------------------------------------------------------------------*/
        deny_list_ptr--;
        for ( ; deny_list_ptr >= _route_deny_list; deny_list_ptr-- )
        {
          if ( routei_v4_routing_table.entry[temp].if_ptr 
               ==
               *deny_list_ptr )
          {
            MSG_ERROR( "RT: Found denied interface 0x%x",
                       *deny_list_ptr, 0, 0 );
            temp = -1;
            break;
          }
        }

        if ( temp != -1 )
        {
          MSG_MED( "RT: Found table match (if = 0x%x[%d])",
                   routei_v4_routing_table.entry[temp].if_ptr,
                   routei_v4_routing_table.entry[temp].if_ptr->instance, 0 );
          *if_ptr  = routei_v4_routing_table.entry[temp].if_ptr;
          if ( gateway_ptr )
          {
            gateway_ptr->addr.v4 = routei_v4_routing_table.entry[temp].gateway;
          }
          
          routei_v4_routing_table.entry[temp].pkt_count++;
        }
      }
    }

    TASKFREE();
  }

  /* Check the outcome of the lookup */
  if ( *if_ptr == NULL )
  {
    _errno = E_NO_ROUTE;
    return -1;
  }
  else
  {
    return 0;
  }
} /* route_get() */


/*===========================================================================

FUNCTION   ROUTE_FLUSH

DESCRIPTION
  This function allows to flush entries for a specific interface
  or the complete DMSS routing table.

DEPENDENCIES
  None
  
PARAMETERS
  if_ptr    - Pointer to allow flushing interface specifc routes only
  _errno  - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (_errno contains error code).
  
SIDE EFFECTS
  No IP traffic can be sent or forwarded until a new route
  is added to the DMSS routing table.
===========================================================================*/
int route_flush
(
  ps_iface_type*  if_ptr
)
{
  uint16        i = 0;
  volatile int  t_size;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( if_ptr == NULL )
  {
    /* Flash the whole table */
    MSG_HIGH( "RT: Flushing table", 0, 0, 0 );
    memset( &routei_v4_routing_table, 
            0,
            sizeof( routei_v4_routing_table ) );
    t_size = 0;
  }
  else
  {
    /* Only delete this interface's routes */
    MSG_HIGH( "RT: Flushing table for 0x%x", if_ptr, 0, 0 );
    t_size = routei_v4_routing_table.table_size_used;
    while( (t_size > 0) && (i < t_size) )
    {
      if ( routei_v4_routing_table.entry[i].if_ptr == if_ptr )
      {
        t_size = routei_free_v4( i );
      }
      else
      {
        i++;
      }
    }
  }

  return ((t_size == -1) ? -1 : 0);

} /* route_flush() */


/*===========================================================================

FUNCTION   ROUTE_ACL_ADD

DESCRIPTION
  This function allows to add an Access Control List to the
  DMSS ACL list used for policy based routing.
  The ACL's priority can be specified globally (no interface
  class) or within a specific interface class.

DEPENDENCIES
  The ACL points to a valid interface instance.
  
PARAMETERS
  new_acl_ptr  - Ptr to new ACL, memory provided by caller
  priority     - Priority for this ACL within the ACL list
  if_class     - Class within the ACL priority is to be set

RETURN VALUE
  0 for success, -1 for failure
  
SIDE EFFECTS
  None
===========================================================================*/
int route_acl_add
( 
  acl_type*                          new_acl_ptr, 
  route_acl_priority_enum_type       priority
)
{
  acl_type*  acl_ptr = routei_acl_list_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize the non-application owned fields */
  new_acl_ptr->disabled  = FALSE;
  new_acl_ptr->pkt_count = 0;
  new_acl_ptr->next_ptr  = NULL;

  /* Check for an empty list first */
  if ( routei_acl_list_ptr == NULL )
  {
    routei_acl_list_ptr = new_acl_ptr;
    return 0;
  }

  switch( priority )
  {
    case ROUTE_ACL_ANY_PRIORITY:
      /* Just append the ACL to the routing list */
      while( acl_ptr->next_ptr != NULL ) acl_ptr = acl_ptr->next_ptr;
      acl_ptr->next_ptr = new_acl_ptr;
      break;

    case ROUTE_ACL_TOP_PRIORITY:
#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#else
      ERR( "RT: Unsupported op", 0, 0, 0 );
#endif        
      break;
      // ...
  }

  return 0;

} /* route_acl_add() */


/*===========================================================================

FUNCTION   ROUTE_ACL_DELETE

DESCRIPTION
  This function allows to remove an existing ACL from the
  DMSS ACL list for policy based routing.

DEPENDENCIES
  None
  
PARAMETERS
  del_acl_ptr  - Ptr to ACL to be deleted from list
  _errno     - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (_errno contains error code).
  
SIDE EFFECTS
  None
===========================================================================*/
int route_acl_delete
(
  acl_type*  del_acl_ptr
)
{
  acl_type*  tmp_ptr = routei_acl_list_ptr;
  acl_type*  acl_ptr = routei_acl_list_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if the first ACL in the list is to be deleted */
  if ( del_acl_ptr == routei_acl_list_ptr )
  {
    routei_acl_list_ptr = del_acl_ptr->next_ptr;
    return 0;
  }

  /* Appearently not - walk through until we find it */
  while ( (acl_ptr != del_acl_ptr)
          &&
          (acl_ptr != NULL ) )
  {
    tmp_ptr = acl_ptr;
    acl_ptr = acl_ptr->next_ptr;
  }

  /* If we found it, adjust the list to exclude it */
  if ( acl_ptr != NULL )
  {
    tmp_ptr->next_ptr = acl_ptr->next_ptr;
    return 0;
  }
  else
  {
    return -1;
  }
} /* route_acl_delete() */


/*===========================================================================

FUNCTION   ROUTE_ACL_CONTROL

DESCRIPTION
  This function en- and disables an ACL to be used for routing lookups.
  ACLs are enabled by default when registered.

DEPENDENCIES
  The ACL points to a valid interface instance.
  
PARAMETERS
  acl_id_type  - Specifies whether ACL is identified by its pointer
                 or its associated interface
  acl_id       - ACL identifier
  acl_enable   - Control flag (TRUE for enabling, FALSE for disabling)

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void route_acl_control
( 
  route_acl_id_enum_type   acl_id_type,
  route_acl_id_type        acl_id,
  boolean                  acl_enable
)
{
  acl_type*  acl_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  TASKLOCK();

  /* Check how the ACL is given to us */
  switch ( acl_id_type )
  {
    case ROUTE_ACL_PTR_ID:
      /* The caller gave us the ACL pointer - adjust the ACL's state */
      if ( acl_id.acl_ptr )
      {
        if ( acl_enable )
        {
          MSG_HIGH( "RT: ACL 0x%x enabled.", acl_id.acl_ptr, 0, 0 );
          acl_id.acl_ptr->disabled = FALSE;
        }
        else
        {
          MSG_HIGH( "RT: ACL 0x%x disabled.", acl_id.acl_ptr, 0, 0 );
          acl_id.acl_ptr->disabled = TRUE;
        }
      }
      else
      {
        MSG_ERROR( "RT: Bad ACL pointer", 0, 0, 0 );
      }
      break;

    case ROUTE_ACL_IFACE_ID:
      /* The caller gave us the associated iface pointer - find */
      /* the corresponding ACL and adjust it's state.           */
      for ( acl_ptr = routei_acl_list_ptr;
            acl_ptr != NULL;
            acl_ptr = acl_ptr->next_ptr )
      {
        if ( acl_ptr->if_ptr == acl_id.if_ptr )
        {
          if ( acl_enable )
          {
            MSG_HIGH( "RT: ACL 0x%x enabled.", acl_ptr, 0, 0 );
            acl_ptr->disabled = FALSE;
          }
          else
          {
            MSG_HIGH( "RT: ACL 0x%x disabled.", acl_ptr, 0, 0 );
            acl_ptr->disabled = TRUE;
          }
        }
      }
      break;
      
    default:
      MSG_ERROR( "RT: Invalid ACL Id type %d", acl_id_type, 0, 0 );
  }
          
  TASKFREE();

} /* route_acl_control() */


/*===========================================================================

FUNCTION   ROUTE_ACL_SET_PRIORITY

DESCRIPTION
  This function allows to update an ACL's priority globally
  or within a specific interface class.

DEPENDENCIES
  None
  
PARAMETERS
  acl_ptr   - Ptr to ACL, memory provided by caller
  priority  - Priority for this ACL within the ACL list
  if_class  - Class within the ACL priority is to be set
  _errno  - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (_errno contains error code).
  
SIDE EFFECTS
  None
===========================================================================*/
int route_acl_set_priority
(
  acl_type*                          acl_ptr, 
  route_acl_priority_enum_type       priority
)
{
  if ( route_acl_delete( acl_ptr ) == -1 )
  {
    _errno = E_INVALID_ARG;
    return -1;
  }

  return route_acl_add( acl_ptr, priority );

} /* route_acl_set_priority() */

#endif /* FEATURE_DATA_PS */
