#ifndef PS_IPFLTR_DEFS_H
#define PS_IPFLTR_DEFS_H

/*===========================================================================

                          P S _ I P F L T R _ D E F S . H

DESCRIPTION

  Data structure definition for inbound IP filters

  Copyright (c)2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
                      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ipfltr_defs.h_v   1.0   07 Feb 2003 20:14:44   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ipfltr_defs.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/28/03    usb    created file
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      GLOBAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*---------------------------------------------------------------------------
  Type of IP filters, for ACL_T
---------------------------------------------------------------------------*/

typedef enum
{
  IP_INBOUND_FILTER_DEFAULT_TYPE,      /* A default set of rules executing 
                                          on specified parameters          */
  IP_INBOUND_FILTER_ACL_TYPE,          /* An acl func ptr type containing 
                                          configurable list of rules       */
  IP_INBOUND_FILTER_MAX_TYPE
} ip_filter_enum_type;


/*---------------------------------------------------------------------------
  Specified parameter mask for IP_INBOUND_FILTER_DEFAULT_TYPE
---------------------------------------------------------------------------*/
typedef enum
{
  IPFLTR_IP_SRC_ADDR_MASK        = 0x00000001,
  IPFLTR_IP_DST_ADDR_MASK        = 0x00000002,
  IPFLTR_TCP_SRC_PORT_MASK       = 0x00000100,
  IPFLTR_TCP_DST_PORT_MASK       = 0x00000200,
  IPFLTR_UDP_SRC_PORT_MASK       = 0x00010000,
  IPFLTR_UDP_DST_PORT_MASK       = 0x00020000,
  IPFLTR_ICMP_MSG_CODE_MASK      = 0x01000000,
  IPFLTR_ICMP_MSG_TYPE_MASK      = 0x02000000
} ip_filter_param_mask_enum_type;


/*---------------------------------------------------------------------------
  Data structure carrying parameters for IP_INBOUND_FILTER_DEFAULT_TYPE
---------------------------------------------------------------------------*/
typedef struct
{
  ip_filter_param_mask_enum_type field_mask;

  union
  {
    struct
    {
      uint32 src_addr;
      uint32 dst_addr;
    } v4;

    /* later add v6 specific information here */
  } ip_hdr;

  union
  {
    struct
    {
      uint16 src_port;
      uint16 dst_port;
    } tcp;
    
    struct
    {
      uint16 src_port;
      uint16 dst_port;
    } udp;
    
    struct
    {
      uint16 type;
      uint16 code;
    } icmp;
  } prot_hdr;
} ip_filter_type;



#endif /* FEATURE_DATA */
#endif /* PS_IPFLTR_DEFS_H */
