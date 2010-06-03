/*===========================================================================

                   P R O T O C O L   S E R V I C E S

              I N T E R N A L   R O U T I N G   T A B L E

GENERAL DESCRIPTION
  This file defines the structure and management functions for
  the kernel's routing table. The table is a fixed size array
  for lookup efficiency.

  Functions and data structures are specific to the IP version
  and named accordingly.

EXTERNALIZED FUNCTIONS
  routei_alloc_v4()       - Allocate an entry in the IPv4 routing table
  routei_free_v4()        - Free an entry in the IPv4 routing table
  routei_check_v4_table() - Check all IPv4 entries for consistency
  routei_get_v4_route()   - Do an IPv4 address-based routing lookup
  routei_acl_list()       - Accessor function for the registered-ACL list

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/24/08    pp     Fixed RVCT compiler warnings.
02/20/03    om     More comments, added FEATURE_DATA_PS.
02/12/03    om     Added routei_check_v4_table(), fixed adding of routes.
08/30/02    om     Updated signature of routei_free_v4().
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

#include "ps_acl.h"
#include "ps_ip_addr.h"
#include "ps_iface.h"

#include "ps_route.h"
#include "ps_routei.h"

/* <EJECT> */ 
/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Temporary until REX's errno is there */
extern errno_enum_type  _errno;

/* List of routing ACLs */
acl_type* routei_acl_list_ptr = NULL;

/* Definition of the actual table */
routei_v4_routing_table_type routei_v4_routing_table;

/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION   ROUTEI_ALLOC_V4()

DESCRIPTION
  This functions returns the index to an entry in the routing
  table to be used for a new route.
  Route entries are prioritized as HOST -> NETWORK -> DEFAULT.

DEPENDENCIES
  The new destination and gateway are not already in the table.
  
PARAMETERS
  rtype  - Specifies the type of entry to allocate

RETURN VALUE
  Index of the new entry, -1 if the table is full
  
SIDE EFFECTS
  None
===========================================================================*/
int routei_alloc_v4
(
  route_type_enum_type  rt_type,
  uint32                destination,
  uint8                 metric,
  uint16*               cost_ptr
)
{
  int hst_range = 0;
  int net_range = 0;
  int index = 0;
  int j;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Chack if the table is already full */
  if ( routei_v4_routing_table.table_size_used 
       == 
       ROUTEI_V4_ROUTING_TABLE_SIZE )
  {
    return -1;
  }

  /* Determine the host-, net-, and default route entry ranges */
  if ( routei_v4_routing_table.entry[hst_range].flags.host )
  {
    while( routei_v4_routing_table.entry[hst_range].flags.host ) hst_range++;
  }
  net_range = hst_range;
  if ( (routei_v4_routing_table.entry[net_range].flags.gateway)
         && 
         (routei_v4_routing_table.entry[net_range].dst_addr 
          != 
          ROUTE_DEFAULT_DST) )
  {
    while( (routei_v4_routing_table.entry[net_range].flags.gateway)
           && 
           (routei_v4_routing_table.entry[net_range].dst_addr 
            != 
            ROUTE_DEFAULT_DST) ) net_range++;
  }
  ASSERT( net_range <= routei_v4_routing_table.table_size_used );

  /* Allocate an entry in the requested group. Each group is sorted */
  /* by ascending cost and then metric to speed up routing lookups. */
  switch( rt_type )
  {
    case ROUTE_HOST:
      /* Insert route into the first group */
      index = 0;
      for ( j=0; j<hst_range; j++ )
      {
        if ( routei_v4_routing_table.entry[j].dst_addr == destination )
        {
          if ( ROUTEI_V4_ROUTE_IS_CHEAPER( j, metric, cost_ptr ) )
          {
            /* Found a more expensive route to same dst, insert before */
            index = j;

            hst_range++;
            break;
          }
          else
          {
            /* Found a cheaper route to same dst, insert after */
            index = j + 1;
          }
        }
      }
      hst_range++;      
      break;

    case ROUTE_NET:
      /* Insert route after all host routes */
      index = hst_range;
      for ( j=hst_range; j<net_range; j++ )
      {
        if ( routei_v4_routing_table.entry[j].dst_addr == destination )
        {
          if ( ROUTEI_V4_ROUTE_IS_CHEAPER( j, metric, cost_ptr ) )
          {
            /* Found a more expensive route to same dst, insert before */
            index = j;

            net_range++;
            break;
          }
          else
          {
            /* Found a cheaper route to same dst, insert after */
            index = j + 1;
          }
        }
      }
      net_range++;      
      break;

    case ROUTE_DEFAULT:
      /* Insert route after all host and network routes */
      index = net_range;
      for ( j=net_range; 
            j<routei_v4_routing_table.table_size_used;
            j++ )
      {
        if ( routei_v4_routing_table.entry[j].dst_addr == destination )
        {
          if ( ROUTEI_V4_ROUTE_IS_CHEAPER( j, metric, cost_ptr ) )
          {
            /* Found a more expensive route to same dst, insert before */
            index = j;
            break;
          }
          else
          {
            /* Found a cheaper route to same dst, insert after */
            index = j + 1;
          }
        }
      }
      break;

    default:
      ERR( "RT: Unknown route type %d", rt_type, 0, 0 );
      return -1;
  }

  /* Move all entries above <index> one up */
  for ( j=routei_v4_routing_table.table_size_used; j>index; j-- )
  {
    memcpy( &(routei_v4_routing_table.entry[j]),
            &(routei_v4_routing_table.entry[j-1]),
            sizeof( routei_v4_routing_table.entry[0] ) );
  }
  routei_v4_routing_table.table_size_used++;
  routei_v4_routing_table.default_start_index = hst_range + net_range;

  return index;

} /* routei_alloc_v4() */


/*===========================================================================

FUNCTION   ROUTEI_FREE_V4()

DESCRIPTION
  This functions frees up a used entry in the routing table.

DEPENDENCIES
  None
  
PARAMETERS
  rt_index  - Index of entry to remove

RETURN VALUE
  Number of remaining entries for success or -1 if index is not valid
  
SIDE EFFECTS
  None
===========================================================================*/
int routei_free_v4
(
  uint32  rt_index
)
{
  int i;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check on the index */
  if ( rt_index >= routei_v4_routing_table.table_size_used )
  {
    MSG_ERROR( "RTI: Del index too large (%d/%d)",
               rt_index, routei_v4_routing_table.table_size_used, 0 );
    return -1;
  }

  /* Move all entries above one down and adjust table size */
  for ( i=rt_index; i<routei_v4_routing_table.table_size_used; i++ )
  {
    memcpy( &(routei_v4_routing_table.entry[i]),
            &(routei_v4_routing_table.entry[i+1]),
            sizeof( routei_v4_routing_table.entry[0] ) );
  }
  routei_v4_routing_table.table_size_used--;

  /* Adjust starting index of default entries */
  if ( rt_index < routei_v4_routing_table.default_start_index )
  {
    routei_v4_routing_table.default_start_index--;
  }

  return routei_v4_routing_table.table_size_used;

} /* routei_free_v4() */


/*===========================================================================

FUNCTION   ROUTEI_CHECK_V4_TABLE()

DESCRIPTION
  This functions does a consistency check on the v4 routing table.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  0 for success, otherwise the entry # to the offending line
  
SIDE EFFECTS
  None
===========================================================================*/
int routei_check_v4_table( void )
{
  int i;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Loop through entries and check them */
  for ( i = routei_v4_routing_table.table_size_used - 1; i >= 0; i-- )
  {
    if ( routei_v4_routing_table.entry[i].if_ptr == NULL )
    {
      MSG_ERROR( "RTv4: NULL iface in entry %d", i, 0, 0 );
      break;
    }
  }

  return ( i + 1 );

} /* routei_check_v4_table() */


/*===========================================================================

FUNCTION   ROUTEI_GET_V4_ROUTE()

DESCRIPTION
  This functions returns the index to an entry in the routing
  table for a given destination. A NULL gateway pointer is
  intended for a regular routing lookup, a specified gateway 
  allows to find the entry index of a known route, e.g. to 
  update or delete it.
  The result is purely based on IP address based routing.
  
DEPENDENCIES
  None
  
PARAMETERS
  destination  - Destination address
  gateway_ptr  - Optional pointer to gateway for this route

RETURN VALUE
  Index of the routing entry, -1 if no matching entry was found
  
SIDE EFFECTS
  None
===========================================================================*/
int routei_get_v4_route
(
  uint32   destination,
  uint32*  gateway_ptr
)
{
  int      index;
  boolean  rt_found = FALSE;

  /* Search the routing table from the top (host routes first,  */
  /* then network and default routes), optimization for default */
  /* destinations to skip all host and network routes.          */
  for (  index = ( (destination == ROUTE_DEFAULT_DST) ? 
                   routei_v4_routing_table.default_start_index : 0 );
         index<routei_v4_routing_table.table_size_used;
         index++ )
  {
    if ( (ps_iface_state( routei_v4_routing_table.entry[index].if_ptr ) 
          !=
          IFACE_DISABLED)
         &&
         ((destination & routei_v4_routing_table.entry[index].dst_netmask) 
         ==
         (routei_v4_routing_table.entry[index].dst_addr
          &
          routei_v4_routing_table.entry[index].dst_netmask)) )
    {
      /* Check if an explicit gateway was specified */
      if ( gateway_ptr != NULL )
      {
        if ( routei_v4_routing_table.entry[index].gateway
             == *gateway_ptr )
        {
          rt_found = TRUE;
          break;
        }
      }
      else
      {
        rt_found = TRUE;
        break;
      }
    }
  }

  if ( rt_found )
  {
    return index;
  }
  else
  {
    return -1;
  }
} /* routei_get_v4_route() */


/*===========================================================================

FUNCTION   ROUTEI_ACL_LIST()

DESCRIPTION
  This functions returns the pointer to the first ACL in the
  kernel's list of routing ACLs.
  
DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  Pointer to the list of registered ACLs
  
SIDE EFFECTS
  None
===========================================================================*/
acl_type* routei_acl_list( void )
{
  return routei_acl_list_ptr;

} /* routei_acl_list() */

#endif /* FEATURE_DATA_PS */
