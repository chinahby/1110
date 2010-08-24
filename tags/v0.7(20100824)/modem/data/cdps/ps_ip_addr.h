#ifndef PS_IP_ADDR_H
#define PS_IP_ADDR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              I P _ A D D R . H

GENERAL DESCRIPTION
  This is the header file that defines all of the IP addressing types and
  macros. 

     Copyright (c)2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip_addr.h_v   1.5   11 Oct 2002 09:15:46   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ip_addr.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/01/02    mvl    Added gateway address to the v4_net_info.
09/17/02    mvl    renamed dns_prefix to dns_suffix.
09/02/02    mvl    file cleanup.
07/01/02    mvl    Simplified names.
03/22/02    mvl    created file
===========================================================================*/



/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"

/*---------------------------------------------------------------------------
TYPEDEF IP_ADDR_ENUM_TYPE

DESCRIPTION
  An enum that defines all of the address types supported - used to
  discriminate the union below.

  NOTE: The values are chosen to easy debugging.
---------------------------------------------------------------------------*/
typedef enum
{
  IP_ANY_ADDR     = 0,
  IPV4_ADDR       = 4,
  IPV6_ADDR       = 6,
  IP_ADDR_INVALID = 255

} ip_addr_enum_type;

/*---------------------------------------------------------------------------
MACRO IP_ADDR_TYPE_IS_VALID()

DESCRIPTION
  This macro returns a boolean indicating whether the address type passed in
  is valid.

PARAMETERS
  addr_type: the address type being checked

RETURN VALUE
  TRUE: if addr_type is valid
  FALSE: otherwise.
===========================================================================*/
#define IP_ADDR_TYPE_IS_VALID( addr_type )      \
  ((addr_type == IP_ANY_ADDR) ||                \
   (addr_type == IPV4_ADDR)   ||                \
   (addr_type == IPV6_ADDR))

/*---------------------------------------------------------------------------
TYPEDEF IP_ADDR_TYPE

DESCRIPTION
  structure which is a discriminated union that defines the IP addresses that
  we support.
---------------------------------------------------------------------------*/
typedef struct
{
  ip_addr_enum_type type; 

  union
  {
    uint32 v4;
#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif 
  } addr;

} ip_addr_type;

/*---------------------------------------------------------------------------
TYPEDEF IP_V4_CONFIG_TYPE

DESCRIPTION
  All of the IP configuration information
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 gateway;
  uint32 primary_dns;
  uint32 secondary_dns;
  char  *dns_default_suffix;
  uint16 dns_default_suffix_len;
  uint16 mtu;
  uint16 mru;

} ip_v4_net_info_type;

#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */

#endif /* PS_IP_ADDR_H */

