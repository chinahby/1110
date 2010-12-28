#ifndef PS_ROUTE_H
#define PS_ROUTE_H
/*===========================================================================

                        D A T A   S E R V I C E S

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

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/03    om     More comments.
11/05/02    om     Added enum and union for ACL identifiers.
03/08/02    om     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#include "ps_acl.h"
#include "ps_ip_addr.h"
#include "ps_iface.h"

/* <EJECT> */ 
/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* Types of routes */
typedef enum
{
  ROUTE_DEFAULT = 0,                  /* Default route (dst 0.0.0.0)       */
  ROUTE_HOST,                         /* Host specific route               */
  ROUTE_NET                           /* Network specific route            */
} route_type_enum_type;

/* Priority of an ACL */
typedef enum
{
  ROUTE_ACL_ANY_PRIORITY = 0,          /* Append ACL to DMSS list          */
  ROUTE_ACL_TOP_PRIORITY,              /* Insert ACL at the top of a group */
  ROUTE_ACL_LEAST_PRIORITY             /* Insert ACL at the end of a group */
} route_acl_priority_enum_type;

/* Define the address for default routes */
#define ROUTE_DEFAULT_DST  0x00000000

/* Identifying an ACL by its pointer or associated iface for control */
typedef enum
{
  ROUTE_ACL_PTR_ID = 0,
  ROUTE_ACL_IFACE_ID = 1
} route_acl_id_enum_type;

typedef union
{
  acl_type*       acl_ptr;
  ps_iface_type*  if_ptr;
} route_acl_id_type;

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION   ROUTE_ADD

DESCRIPTION
  This function allows to add a route to the DMSS routing
  table.

DEPENDENCIES
  None

PARAMETERS
  type        - Specifies whether it is a host or network route
  description - IP address of the routing destination in network order
  gateway     - IP address of the routing gateway in network order
  if_ptr      - Specifies a local interface to route to (optional)
  metric      - Specifies the hop metric of this route
  ds_errno    - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (ds_errno contains error code).

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
);

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
  0 for success, -1 for failure (ds_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_delete
(
  ip_addr_type   destination,
  ip_addr_type*  gateway
);

/*===========================================================================

FUNCTION   ROUTE_UPDATE

DESCRIPTION
  This function allows to update an existing entry in the
  DMSS routing table.

DEPENDENCIES
  None

PARAMETERS
  description - IP address of the routing destination in network order
  gateway     - IP address of the routing gateway in network order
  if_ptr      - Specifies a local interface to route to (optional)
  metric      - Specifies the hop metric of this route
  ds_errno    - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (ds_errno contains error code).

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
);

/*===========================================================================

FUNCTION   ROUTE_GET

DESCRIPTION
  This function allows to retrieve a route from the DMSS
  routing table.

DEPENDENCIES
  None

PARAMETERS
  ip_fiilter_ptr  - Ptr to packet's IP information
  acl_pi_ptr      - Ptr to packet's policy information
  ip_addr_only    - Don't use ACLs for routing decision if true
  gateway         - Ptr to IP address of the routing gateway in network order
  if_ptr          - Ptr local interface route goes through
  ds_errno        - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (ds_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_get
(
  acl_ipfilter_info_type*  ip_filter_ptr,
  acl_policy_info_type*    acl_pi_ptr,
  boolean                  ip_addr_only,
  ip_addr_type*            gateway_ptr,
  ps_iface_type**          if_ptr
);

/*===========================================================================

FUNCTION   ROUTE_FLUSH

DESCRIPTION
  This function allows to flush the complete DMSS routing table.

DEPENDENCIES
  None

PARAMETERS
  iface_ptr  - Pointer to a specific interface to flush all routes
               for, set to NULL to flush entire routing table
  ds_errno   - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (ds_errno contains error code).

SIDE EFFECTS
  No IP traffic can be sent or forwarded until a new route
  is added to the DMSS routing table.
===========================================================================*/
int route_flush
(
  ps_iface_type*  ifcace_ptr
);

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
  acl_ptr   - Ptr to ACL, memory provided by caller
  priority  - Priority for this ACL within the ACL list
  if_class  - Class within the ACL priority is to be set

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int route_acl_add
(
  acl_type*                          new_acl_ptr,
  route_acl_priority_enum_type       priority
);

/*===========================================================================

FUNCTION   ROUTE_ACL_DELETE

DESCRIPTION
  This function allows to remove an existing ACL from the
  DMSS ACL list for policy based routing.

DEPENDENCIES
  None

PARAMETERS
  del_acl_ptr  - Ptr to ACL, memory provided by caller
  ps_errno     - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_acl_delete
(
  acl_type*  del_acl_ptr
);

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
);

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
  ps_errno  - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_acl_set_priority
(
  acl_type*                          acl_ptr,
  route_acl_priority_enum_type       priority
);

#endif    /* PS_ROUTE_H */

