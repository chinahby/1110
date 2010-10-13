/*===========================================================================

                   P R O T O C O L   S E R V I C E S

       A C C E S S   C O N T R O L   L I S T   D E F I N I T I O N S

            F O R   T H E   I N B O U N D   I P   F I L T E R

GENERAL DESCRIPTION
  The PS inbound filter is used to protect the mobile device
  from malicious traffic.
  This filter is application specific and should be re-evaluated
  for each model.
  All firewall type filter rules should use the ACL_DEFAULT_CLASS.
  Any specific rules to identify packets for this host (e.g.
  destination address matches interface address) should use the
  ACL_IPADDR_CLASS, which is used to tag local traffic.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
                      
  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_inbound_acl.c_v   1.6   19 Feb 2003 14:32:04   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_inbound_acl.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/13/03    usb    Updated rule names to reflect operation on this iface
02/19/03    usb    Allowing all addresses to go up the stack for now.
02/12/03    om     Fixed loopback if rule.
01/31/03    usb    Added new rule to check IP version and loopback iface,
                   moved DENY rules before local check rule
11/25/02    usb    Updated DENY rules as per the new format.
09/13/02    om     Update to includes.
03/08/02    om     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ps_iface.h"

/* <EJECT> */ 
/*===========================================================================

                 ACCESS CONTROL LIST NAME DEFINITIONS

===========================================================================*/

ACL_DEF( ps_inbound_acl )
ACL_START
  DENY( NOT( IP_VERSION_IS( 4 ) ) )
  PASS_QUICK(ACL_IFNAME_CLASS, THIS_IFACE_NAME_IS( LO_IFACE ) )
  DENY( DST_ADDR_IS_IN( IPV4( 127,0,0,1 ), 0xffffffff ) )
  DENY( SRC_ADDR_IS_IN( LOCALHOST, 0xffffffff ) )
  /*-------------------------------------------------------------------------
    For now accept all other src/dst addresses. This will pass all non 
    loopback pkts up the stack including broadcast and multicast pkts.
    It needs to be fixed later to handle broadcast/multicast more 
    appropriately and also ifaces with null addresses. 
  -------------------------------------------------------------------------*/
  PASS( ACL_IPADDR_CLASS, TRUE )
//  PASS( ACL_IPADDR_CLASS, DST_ADDR_IS_IN( LOCALHOST, 0xffffffff ) )
ACL_END
