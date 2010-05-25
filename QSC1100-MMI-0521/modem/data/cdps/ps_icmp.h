#ifndef ICMP_H
#define ICMP_H

/*===========================================================================

                   I C M P    H E A D E R    F I L E

DESCRIPTION
  Internet Control Message Protocol (ICMP)
  RFC 792

EXTERNALIZED FUNCTIONS
  icmp_input()
   This function Process an incoming ICMP packet. The only ICMP messages 
   that are processed are 1) Echo, 2) Info Request, and 3) Timestamp. Each 
   of these messages will be echoed back to the ICMP peer in the base 
   station. Other ICMP messages will be handled by dssicmp_ps_input() in 
   dssicmp.c 
    
  icmp_output()
    This function does the following:
      1. Converts from host to network all the elements of the ICMP header
         and inserts the ICMP header in the ICMP packet.  
      2. Creates the pseudo-header and calculates ICMP checksum.
      3. Sends ICMP packet down to IP.
 
  icmp_get_stats()
    This function will copy ICMP stats (enumerated in icmp_stats_enum)
    into the given array.
    
  icmp_reset_stats()
    This function will reset those ICMP stats which can be reported
    with icmp_get_stats.

Copyright (c) 1995 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 1997 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_icmp.h_v   1.0   08 Aug 2002 11:19:50   akhare  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_icmp.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/21   js      Updated ICMP header definition and introduced new modules 
                   to support arbitrary ICMP message types. 
06/12/97   ldg     Added new FEATURE_DS_PSTATS
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/

#include "psglobal.h"
#include "customer.h"       /* Customer Specific Features */

#include "dsm.h"
#include "internet.h"
#include "ps_ip.h"
#include "ps_iface.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*========================================================================== 
  Internet Control Message Protocol 
===========================================================================*/

/*---------------------------------------------------------------------------
   Message types 
---------------------------------------------------------------------------*/
#define ICMP_ECHO_REPLY        0  /* Echo Reply */
#define ICMP_DEST_UNREACH      3  /* Destination Unreachable */
#define ICMP_QUENCH            4  /* Source Quench */
#define ICMP_REDIRECT          5  /* Redirect */
#define ICMP_ECHO              8  /* Echo Request */
#define ICMP_TIME_EXCEED      11  /* Time-to-live Exceeded */
#define ICMP_PARAM_PROB       12  /* Parameter Problem */
#define ICMP_TIMESTAMP        13  /* Timestamp */
#define ICMP_TIME_REPLY       14  /* Timestamp Reply */
#define ICMP_INFO_RQST        15  /* Information Request */
#define ICMP_INFO_REPLY       16  /* Information Reply */
#define ICMP_ADDR_MASK        17  /* Address mask request */
#define ICMP_ADDR_MASK_REPLY  18  /* Address mask reply */
#define ICMP_FQDN_REQUEST     37  /* Domain Name Request */
#define ICMP_FQDN_REPLY       38  /* Domain Name Reply */
#define ICMP_TYPES            39  /* (next) */

/*---------------------------------------------------------------------------
  Generic ICMP header information. 
---------------------------------------------------------------------------*/
struct icmp_hdr
{
  uint8 type;
  uint8 code;
  uint16 chksum; 

};

/*---------------------------------------------------------------------------
  ICMP arguments. 
---------------------------------------------------------------------------*/
union icmp_args 
{
    uint16 mtu;
    uint32 unused;
    uint8  pointer;
    ip4a   address;
    struct echo_s
    {
      uint16 id;
      uint16 seq;
    } echo;
};

/*---------------------------------------------------------------------------
  Internal type definitnion for ICMP header. 
---------------------------------------------------------------------------*/
struct icmp 
{
  struct icmp_hdr info;
  union icmp_args arg;
};


/*---------------------------------------------------------------------------
  Structure definition for the ICMP Control block.
---------------------------------------------------------------------------*/
struct icmp_cb
{
  struct scb_s *scb_ptr;        /* Ptr to socket ctl block                 */
  q_type rcvq;                  /* receive queue for the socket        */
  uint8  type;                   
  uint8  code;
};


#define ICMPLEN   8  /* Length of ICMP header on the net */

/*---------------------------------------------------------------------------
  Destination Unreachable codes 
---------------------------------------------------------------------------*/
#define ICMP_NET_UNREACH  0   /* Net unreachable */
#define ICMP_HOST_UNREACH  1  /* Host unreachable */
#define ICMP_PROT_UNREACH  2  /* Protocol unreachable */
#define ICMP_PORT_UNREACH  3  /* Port unreachable */
#define ICMP_FRAG_NEEDED  4   /* Fragmentation needed and DF set */
#define ICMP_ROUTE_FAIL   5   /* Source route failed */
#define ICMP_ADMIN_PROHIB  13 /* Administrativly prohibbited */

#define NUNREACH    14  /* (next) */

/*---------------------------------------------------------------------------
  Time Exceeded codes 
---------------------------------------------------------------------------*/
#define ICMP_TTL_EXCEED   0  /* Time-to-live exceeded */
#define ICMP_FRAG_EXCEED  1  /* Fragment reassembly time exceeded */

#define NEXCEED     2  /* (next) */

/*--------------------------------------------------------------------------- 
  Redirect message codes 
---------------------------------------------------------------------------*/
#define ICMP_REDR_NET  0  /* Redirect for the network */
#define ICMP_REDR_HOST 1  /* Redirect for the host */
#define ICMP_REDR_TOS  2  /* Redirect for Type of Service, or-ed with prev */

#define NREDIRECT  3  /* (next) */

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION HTONICMP

DESCRIPTION
 This function will generate an ICMP header in network byte order, link data, 
 and compute the checksum.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int icmpi_hton
(
  struct icmp   *icmp,
  dsm_item_type **bpp
);


/*===========================================================================

FUNCTION NTOHICMP

DESCRIPTION
 This function will Pull off ICMP header information.

DEPENDENCIES
  None

RETURN VALUE
  0 if the header extraction was successful, else -1

SIDE EFFECTS
  None
===========================================================================*/
int icmpi_ntoh
(
  struct icmp   *icmp,
  dsm_item_type **bpp
);



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

/*===========================================================================

FUNCTION ICMP_OUTPUT

DESCRIPTION
  This function does the following:
  1. Converts from host to network all the elements of the ICMP header
     and inserts the ICMP header in the ICMP packet.  
  2. Creates the pseudo-header and calculates ICMP checksum.
  3. Sends ICMP packet down to IP.

DEPENDENCIES
  The source IP address should be correct.

RETURN VALUE
  None. 

SIDE EFFECTS
  None. 

===========================================================================*/extern void icmp_output
(
  ip4a       *dest_ip_ptr,               
  struct icmp        *icmp_hdr_ptr,                    
  dsm_item_type      *bp,                              
  struct icmp_cb     *icmp_cb_ptr,
  uint16             length                                    
)
;


/*===========================================================================

FUNCTION ICMP_INPUT

DESCRIPTION
 This function Process an incoming ICMP packet. The only ICMP messages that
 are processed are 1) Echo, 2) Info Request, and 3) Timestamp. Each of these
 messages will be echoed back to the ICMP peer in the base station. Other 
 ICMP messages will be handled by dssicmp_ps_input() in dssicmp.c 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void icmp_input
(
  struct ps_iface_s  *if_ptr,
  struct ip          *ip,
  dsm_item_type      *bp,
  int                 rxbroadcast
);


#endif  /* ICMP_H */

