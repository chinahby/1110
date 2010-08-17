#ifndef PS_IFACE_IPFLTR_H
#define PS_IFACE_IPFLTR_H

/*===========================================================================

                          P S _ I F A C E _ I P F L T R . H

DESCRIPTION
  Header file for declaration of functions to use IP filters for inbound
  iface specific processing of IP packets received on that iface.

EXTERNALIZED FUNCTIONS

  PS_IFACE_IPFLTR_ADD()
    adds filters to the iface
  
  PS_IFACE_IPFLTR_DELETE() 
    deletes filters from the iface
  
  PS_IFACE_IPFLTR_EXECUTE() 
    executes filter for the processing inbound IP pkts

  Copyright (c)2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
                      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface_ipfltr.h_v   1.0   07 Feb 2003 20:14:34   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_iface_ipfltr.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/28/03    usb    created file
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA
#include "ps_iface.h"
#include "ps_ipfltr_defs.h"
#include "ps_acl.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

typedef acl_ipfilter_info_type ip_filter_info_type;

/*---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

                    

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_ADD()

DESCRIPTION
  This function adds IP filter rules to the specified iface. The filter is 
  tied to a socket, all packets passign through the filter are passed up to 
  that socket. Any existing filter for the socket is deleted before adding 
  the new filter.

DEPENDENCIES
  Caller is responsible for adding a reference to this iface to SCB so that 
  filter could be deleted when the socket is closed. 

PARAMETERS
  iface_ptr - iface to which filter should be added
  filter_type - Type of filter
  num_filters - number of filters passed in
  fi_ptr_arr - array of ptr to filters of the specified type
  sockfd - socket associated with the filters

RETURN VALUE
  Number of filters successfully added, 0 if none could be added

SIDE EFFECTS
  None.

===========================================================================*/
int ps_iface_ipfltr_add
( 
  ps_iface_type        *iface_ptr,
  ip_filter_enum_type  filter_type,
  uint8                num_filters,
  void                 *fi_ptr_arr,
  sint15               sockfd
);


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_DELETE()

DESCRIPTION
  This function deletes all the existing IP filter rules for a socket from 
  the specified iface.
  
DEPENDENCIES
  Caller is responsible for deleting the reference to this iface from SCB.
  
PARAMETERS
  iface_ptr - this iface ptr
  sockfd - socket associated with the filter

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ps_iface_ipfltr_delete
( 
  ps_iface_type        *iface_ptr,
  sint15               sockfd
);


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_EXECUTE()

DESCRIPTION
  This function executes all the filters in an iface to see if any of 
  those passes criteria specified in the information passed in. Processing 
  is done until the first match is found and socket descriptor associated 
  with that filter is returned. For DEFAULT filter type, each field set in
  the filter is compared against the input info using a fixed set of rules. 
  For ACL type filters, the ACL function is called which can contain more
  complex and different types of rules.

DEPENDENCIES
  None
  
PARAMETERS
  iface_ptr - iface ptr on which the IP pkt arrived
  ip_filter_info_ptr - Ptr to IP pkt information block to apply filter on
  
RETURN VALUE
  0 for no match, 
  sockfd of the matching socket if a successful match is found

SIDE EFFECTS
  None.

===========================================================================*/
sint15 ps_iface_ipfltr_execute
( 
  ps_iface_type        *iface_ptr,  
  ip_filter_info_type  *ip_filter_info_ptr
);

#endif /* FEATURE_DATA */
#endif /* PS_IFACE_IPFLTR_H */
