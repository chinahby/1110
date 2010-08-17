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

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_acl.c_v   1.2   12 Feb 2003 20:35:28   omichael  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_acl.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $
                    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/03   om      Added comments to identify the class array contents.
03/08/02   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#include "ps_aclrules.h"

/* <EJECT> */ 
/*===========================================================================

                        DATA DEFINITIONS

===========================================================================*/

/* Priority lookup table to map rule classes into routing priorities */
int acl_class_priorities[ACL_NUM_CLASSES] = 
{
  0,  /* ACL_DEFAULT_CLASS */
  1,  /* ACL_IFGROUP_CLASS */
  6,  /* ACL_IFNAME_CLASS  */
  5,  /* ACL_IPADDR_CLASS  */
  4,  /* ACL_IPPORT_CLASS  */
  3,  /* ACL_QOS_CLASS     */
  2   /* ACL_APP_CLASS     */
};

