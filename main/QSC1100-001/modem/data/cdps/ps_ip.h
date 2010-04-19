#ifndef PS_IP_H
#define PS_IP_H
/*===========================================================================

                   M O D E L    H E A D E R    F I L E

DESCRIPTION


   Copyright (c) 1995-2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip.h_v   1.1   30 Aug 2002 20:59:24   omichael  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ip.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/30/02    om     Updated ip_send to use routing cache info.
08/01/02    mvl    Removed ip_(de)reg_pkt_handler() and iface.h as they
                   weren't used.
05/03/02    mvl    File cleanup and added meta_info to ip_send() prototype
04/17/02    rc     Defined internal ip_pkt_fn_ptr_type to make it
                   build for all targets
05/20/00    snn    Added support for IP fragments Reassembly.
12/14/98    ldg    Commented out extern declaration for Raw_ip.
10/27/98    ldg    Merged from ARM port: Include comdef.h, iface.h.
06/12/97    ldg    Added new FEATURE_DS_PSTATS
11/14/96    jjw    Unwired Planet Upgrade (ie added UDP)
11/09/95    jjw    IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "comdef.h"
#include "dsm.h"
#include "ps_meta_info.h"
#include "ps_iface.h"
#include "internet.h"
#include "psglobal.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define TLB    30     /* Default reassembly timeout, sec */
#define IPVERSION  4
#define IP_CS_OLD  1  /* use saved checksum */
#define IP_CS_NEW  0  /* calculate checksum */

/* SNMP MIB variables, used for statistics and control. See RFC 1066 */
extern struct mib_entry    Ip_mib[];
#define ipForwarding       Ip_mib[1].value.integer
#define ipDefaultTTL       Ip_mib[2].value.integer
#define ipInReceives       Ip_mib[3].value.integer
#define ipInHdrErrors      Ip_mib[4].value.integer
#define ipInAddrErrors     Ip_mib[5].value.integer
#define ipForwDatagrams    Ip_mib[6].value.integer
#define ipInUnknownProtos  Ip_mib[7].value.integer
#define ipInDiscards       Ip_mib[8].value.integer
#define ipInDelivers       Ip_mib[9].value.integer
#define ipOutRequests      Ip_mib[10].value.integer
#define ipOutDiscards      Ip_mib[11].value.integer
#define ipOutNoRoutes      Ip_mib[12].value.integer
#define ipReasmTimeout     Ip_mib[13].value.integer
#define ipReasmReqds       Ip_mib[14].value.integer
#define ipReasmOKs         Ip_mib[15].value.integer
#define ipReasmFails       Ip_mib[16].value.integer
#define ipFragOKs          Ip_mib[17].value.integer
#define ipFragFails        Ip_mib[18].value.integer
#define ipFragCreates      Ip_mib[19].value.integer

#define NUMIPMIB  19

/*---------------------------------------------------------------------------
  Type to index the array of IP stats.
---------------------------------------------------------------------------*/
typedef enum
{
  IP_STATS_IN_RECEIVES,       /* The total number of input datagrams received
                                 from interfaces, including those received
                                 in error. */

  IP_STATS_IN_HDR_ERRORS,     /* The numbe of input datagrams discarded due to
                                 errors in their IP headers, including bad
                                 checksums, version number mismatch, other
                                 format errors, time-to-live exceeded, errors
                                 discovered in processing thier IP options,
                                 etc. */

  IP_STATS_IN_UNKNOWN_PROTOS, /* The number of locally-addressed datagrams
                                 received successfully but discarded because
                                 of an unknown or unsupported protocol. */

  IP_STATS_IN_DELIVERS,       /* The total number of input datagrams succ-
                                 essfully delivered to IP user-protocols
                                 (including ICMP). */

  IP_STATS_OUT_REQUESTS,      /* The total number of IP datagrams which local
                                 IP user-protocols (including ICMP) supplied
                                 to IP in requests for transmission. */

  IP_STATS_COUNT              /* The number of IP stats. */
} ip_stats_enum;

/*---------------------------------------------------------------------------
  Array of pointers to stats names.
---------------------------------------------------------------------------*/
extern const byte * const ip_stats_name_ptr[IP_STATS_COUNT];

extern uint16 Id_cntr;   /* Datagram serial number */

/* Structure for handling raw IP user sockets */

struct raw_ip 
{
  struct raw_ip *next;   /* Linked list pointer */

  struct dsm_item_s *rcvq;            /* receive queue */
  void (*r_upcall)(struct raw_ip *);
  int protocol;                       /* Protocol */
  int user;                           /* User linkage */
};

/* List of TCP port numbers to be given priority queuing */

extern int Tcp_interact[];

/*---------------------------------------------------------------------------
  This is the type declaration for the callback function that is
  registered for processing IP packets received on a IP interface for a 
  PDP Type IP sockets call.
---------------------------------------------------------------------------*/
typedef int (*ip_pkt_fn_ptr_type)(
                                                         /* Input interface */
                struct dsm_item_s *item_ptr,                 /*Input packet */
                uint8  *source,               /*media source address if any */
                int    broadcast,  /*True if pkt had link broadcast address */
                void   *context_data           /* context data for callback */
                );  

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION IP_SEND()

DESCRIPTION
  This function will Send an IP datagram. Modeled after the example interface
  on p 32 of RFC 791

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int ip_send
(
  ip4a               source,          /* source address                   */
  ip4a               dest,            /* Destination address              */
  uint8              protocol,        /* Protocol                         */
  uint8              tos,             /* Type of service                  */
  uint8              ttl,             /* Time-to-live                     */
  dsm_item_type     *send_dg_ptr,     /* Pointer to incoming datagram     */
  uint16             length,          /* Optional length of data portion  */
  uint16             id,              /* Optional identification          */
  uint8              df,              /* Don't-fragment flag              */
  ps_meta_info_type* meta_info_ptr,   /* Metainfo about packet            */
  ps_iface_type*     tx_if_cache,     /* TX interface cache entry         */
  int16*             ps_errno         /* Error code for failure case      */
);

/*===========================================================================
FUNCTION IP_RECEIVE()

DESCRIPTION
  This function dispatches completed datagrams to the proper transport
  module.  Note that this function DOES NOT perform IP re-assembly, and
  thus, receipt of any IP fragments will be dispatched to the transport
  layer as a full IP datagram.  The transport layer will discard each
  fragment.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ip_receive
(
  acl_ipfilter_info_type  *ip_filter_info_ptr, /* Pkt hdr and info */
  dsm_item_type           *bp                  /* Data portion */
);

/*===========================================================================
FUNCTION IP_ROUTE()

DESCRIPTION
  This function determines which interface a pecket to send on
  given its IP address info and any policy info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int ip_route
(
  acl_ipfilter_info_type*  ip_filter_info_ptr, /* Pkt and policy info */
  ps_meta_info_type**      meta_info_ptr,      /* Policy info. e.g. from SCB */
  dsm_item_type**          ip_pkt_ptr,         /* Input packet */
  int16*                   ps_errno            /* Error code for faillure */
);

/*===========================================================================

FUNCTION IP_INPUT

DESCRIPTION
  This function accepts received IP packet(-fragment)s from network
  interfaces and applies ipfilter for the DMSS inbound ACL.
  It then forwards the packet to ip_route for local delivery
  respectively forwarding.

DEPENDENCIES
  None

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int ip_input
(
  struct ps_iface_s  *in_if_ptr,  /* Input interface */
  struct dsm_item_s  *bp,         /* Input packet */
  uint8              *source,     /* Media source address, if any */
  int                 broadcast   /* Packet had link broadcast address */
);

/*===========================================================================

FUNCTION IP_GET_STATS()

DESCRIPTION
  This function will copy stats (enumerated in ip_stats_enum) into
  the given array.

DEPENDENCIES
  The given array has at least IP_STATS_COUNT elements.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ip_get_stats
(
  word *stats_ptr /* Pointer to array into which stats will be copied */
);


/*===========================================================================

FUNCTION IP_RESET_STATS()

DESCRIPTION
  This function will reset those IP stats which can be reported
  with ip_get_stats.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ip_reset_stats( void );

#endif /* PS_IP_H */
