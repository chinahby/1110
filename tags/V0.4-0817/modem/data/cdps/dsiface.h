#ifndef DSIFACE_H
#define DSIFACE_H
/*===========================================================================

                              D S I F A C E . H
                   
DESCRIPTION

  The Data Services interface file. This contains functions used by 
  the various modules to access network interface.

  NOTE: THESE FUNCTIONS ARE FOR INTERNAL USE ONLY AND ARE SUBJECT TO CHANGE!

Copyright (c) 2001- 2002 by QUALCOMM, Incorporated.  All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/17/02    rc     Fixed a compiler warning.
11/29/01    mvl    removed declaration of dsiface_mip_reset().  Moved several
                   declarations/definitions into dsiface_def.h. 
11/26/01    js     Added prototype and supporting data structures for
                   dsiface_get_opt(),dsiface_set_opt(), and
                   dsiface_ioctl().
11/13/01    mvl    Added dsiface_up() as it was missing, also removed
                   dsiface_notify_iface_down().
11/01/01    mvl    Removed the dsiface_starting() call as it is not used
08/29/01    mvl    Added functions to set and get DNS addresses.
07/06/01    mvl    added prototype for dsiface_starting()
06/14/01    mvl    Added the iface parameter to the
                   dsiface_notify_iface_down() call. 
06/07/01    mvl    Added two more functions to support Mobile IP.
05/18/00    mvl    Added functions to support Mobile IP.
05/10/01    mvl    Changed dsiface_ip_addr_set() to dsiface_set_ip_addr()
                   naming to make meaning clearer.
04/16/01    js     Created module. 
===========================================================================*/
/*===========================================================================

                       INCLUDE FILES FOR THE MODULE
                       
===========================================================================*/
#include "comdef.h"
#include "dsiface_def.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  address type for some of the IP address functions
---------------------------------------------------------------------------*/
typedef enum 
{
  IFACE_IPv4_DNS_PRIMARY   = 1,
  IFACE_IPv4_DNS_SECONDARY = 2
} iface_addr_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION DSIFACE_DETERMINE_MIP_MODE()

DESCRIPTION
  This function informs the Mobile IP State machine that IPCP has completed
  on the inteface passed in.  It also resets the mobile IP mode for the
  interface in question.

PARAMETERS
  iface: the interface in question.

RETURN VALUE
  TRUE:  if M.IP mode is supportable
  FALSE: if M.IP mode is not-supportable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean dsiface_determine_mip_mode
(
   iface_stack_enum_type iface
);
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
FUNCTION DSIFACE_DOWN()

DESCRIPTION
  This function signals the Mobile IP stack when the interface has gone down.
  It calls the dssnet_ppp_down_cb().

PARAMETERS
  iface: the interface in question

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsiface_down
(
  iface_stack_enum_type iface
);

/*===========================================================================
FUNCTION DSIFACE_UP()

DESCRIPTION
  This function signals the Mobile IP stack when the interface has come up -
  and so should be called when IPCP has completed on the interface in
  question.  It also resets the value of the force_sip_next_call variable to
  false. It informs the DSSNET State machine whether M.IP is supported or
  not.

PARAMETERS
  iface: the interface in question

RETURN VALUE
  None

DEPENDENCIES
  The PPP state machines for LCP and IPCP must be in the Opened state.

SIDE EFFECTS
  None
===========================================================================*/
void dsiface_up
(
  iface_stack_enum_type iface
);

/*===========================================================================
FUNCTION DSIFACE_SET_IP_ADDR

DESCRIPTION
  Setting global variable Ip_addr. Reset the socket if necessary. 

DEPENDENCIES
  None. 
    
PARAMETERS
  iface       - interface stack type (PPP Um or PPP Rm)
  new_ip_addr - IP address to be assigned to Ip_addr

RETURN VALUE
  None. 

SIDE EFFECTS
  None.
===========================================================================*/
void dsiface_set_ip_addr
(
  iface_stack_enum_type iface,  /* which interface?                        */
  uint32 new_addr               /* IP address to be assigned to Ip_addr    */
);

/*===========================================================================
FUNCTION DSIFACE_GET_IP_ADDR()

DESCRIPTION
  This function returns the IP address for the interface in question.

PARAMETERS
  iface: the interface in question.

RETURN VALUE
  The IP address of the interface in question.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsiface_get_ip_addr
(
  iface_stack_enum_type iface   /* which interface?                        */
);

/*===========================================================================
FUNCTION DSIFACE_SET_DNS_ADDR()

DESCRIPTION
  This function sets the DNS address to be used on the specified iface - this
  may never be an iface specific item, but just in case.

PARAMETERS
  iface:    Interface to set the address for
  addr_type:The type of IP address to set (unicast, dns,) 
  new_addr: IP address to be assigned to interface - this is expected to be
            in HOST order.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsiface_set_dns_addr
(
  iface_stack_enum_type iface,  /* which interface                         */
  iface_addr_type addr_type,    /* which address to get - primary/secondary*/
  uint32 new_addr               /* IP address to be assigned               */
);

/*===========================================================================
FUNCTION DSIFACE_GET_DNS_ADDR()

DESCRIPTION
  This function returns the DNS sever IP address - either the primary or
  secondsry depending on the enum pasesed in.

PARAMETERS
  iface:     the interface in question.
  addr_type: the type of IP address to fetch

RETURN VALUE
  The DNS servers IP address, in HOST order.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsiface_get_dns_addr
(
  iface_stack_enum_type iface,  /* which interface?                        */
  iface_addr_type addr_type     /* which address to get - primary/secondary*/
);

#endif /* DSIFACE_H */
