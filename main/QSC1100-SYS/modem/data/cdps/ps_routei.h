#ifndef ROUTEI_H
#define ROUTEI_H
/*===========================================================================

                   P R O T O C O L   S E R V I C E S

              I N T E R N A L   R O U T I N G   T A B L E

                          D E F I N I T I O N S

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

Copyright (c) 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/03    om     More comments.
02/12/03    om     Added prototype for routei_check_v4_table().
08/30/02    om     Updated signature of routei_free_v4().
03/08/02    om     Created Module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

/* <EJECT> */ 
/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

/* Static size of the routing table */
#define ROUTEI_V4_ROUTING_TABLE_SIZE    10

/* Macro to compare route costs, prefers cost over metric */
#define ROUTEI_V4_ROUTE_IS_CHEAPER( _index, _metric, _cost_ptr )             \
          (routei_v4_routing_table.entry[_index].flags.cost_is_valid ?       \
            (_cost_ptr ?                                                     \
               (*_cost_ptr < routei_v4_routing_table.entry[_index].cost)     \
               : (_metric < routei_v4_routing_table.entry[_index].metric))   \
            : (_metric < routei_v4_routing_table.entry[_index].metric) )

/* <EJECT> */ 
/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* Structure for a routing entry */
typedef struct
{
  uint32                dst_addr;     /* Destinations address / network id */
  uint32                dst_netmask;  /* Destination network mask          */
  uint32                gateway;      /* Gateway address                   */
  ps_iface_type*        if_ptr;       /* Pointer to local interface        */

  struct
  {
    unsigned int gateway:1;           /* Set if entry is an indirect route */
    unsigned int host:1;              /* Set if entry is a host route      */
    unsigned int icmp_created:1;      /* Set if entry was created by ICMP  */
    unsigned int icmp_modified:1;     /* Set if entry was modified by ICMP */

    unsigned int metric_is_valid:1;   /* Used flags for the fields quali-  */
    unsigned int cost_is_valid:1;     /* fying the attributes of a route   */
  } flags;

  uint8                 metric;       /* RIP metric (hop count)            */
  uint16                cost;         /* Dimensionless cost of route       */

  uint32                pkt_count;    /* Number of packets sent via route  */
} routei_v4_route_entry_type;

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* List of routing ACLs */
extern acl_type* routei_acl_list_ptr;

/* Definition of the actual table - the default_start_index allows */
/* a lookup for a DEFAULT destination to skip host and network     */
/* routes for efficiency (most lookups just need a default route). */
typedef struct
{
  routei_v4_route_entry_type  entry[ROUTEI_V4_ROUTING_TABLE_SIZE];
  uint32                      table_size_used;
  uint32                      default_start_index;
} routei_v4_routing_table_type;

extern routei_v4_routing_table_type routei_v4_routing_table;

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
  None
  
PARAMETERS
  rtype  - Specifies the type of entry to allocate

RETURN VALUE
  Index of the new entry
  
SIDE EFFECTS
  None
===========================================================================*/
int routei_alloc_v4
(
  route_type_enum_type  rt_type,
  uint32                destination,
  uint8                 metric,
  uint16*               cost_ptr
);

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
);

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
int routei_check_v4_table( void );

/*===========================================================================

FUNCTION   ROUTEI_GET_V4_ROUTE()

DESCRIPTION
  This functions returns the index to an entry in the routing
  table for a given destination.
  The result is purely based on IP address routing.
  
DEPENDENCIES
  None
  
PARAMETERS
  destination  - Destination address

RETURN VALUE
  Index of the routing entry
  
SIDE EFFECTS
  None
===========================================================================*/
int routei_get_v4_route
(
  uint32   destination,
  uint32*  gateway
);

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
  Pointer to ACL list
  
SIDE EFFECTS
  None
===========================================================================*/
acl_type* routei_acl_list( void );

#endif    /* ROUTEI_H */

