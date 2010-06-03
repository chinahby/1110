/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                   I P . C

GENERAL DESCRIPTION
 Upper half of IP, consisting of send/receive primitives,
  Not needed when running as a standalone gateway.
  Copyright 1991 Phil Karn, KA9Q
  Additional material Copyright 1992 William Allen Simpson
  fragment reassembly removed.
 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ip.c_v   1.8   19 Feb 2003 14:33:04   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ip.c#2 $ $DateTime: 2008/06/27 15:37:54 $ $Author: praveenp $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/27/08    pp     Fixed RVCT compiler warnings.
06/05/03    om     Fixed call to IPSec tx function for ESP packets.
05/27/03    om     Added IPSec ESP processing on the receive path.
04/23/03    ss     Modified ip_route() to use IS_SOCKOPT_SET() macro
04/17/03    aku    Free dsm packet in ip_receive() if mobile receives packet
                   with unknown protocol type.
02/19/03    usb    Setting IP version field of ip_fltr_info in ip_input()
02/13/03    usb    Moved msg macros in ip_receive to MSG_LOW
02/12/03    om     Added UDP routing info use in ip_send().
01/31/03    usb    Fixed IP pkt forwarding, fixed IP routing for local 
                   packets and loopback iface, added new arg to ntohip() 
                   based on the new prototype.
10/03/02    rc     Commented out ps_inbound_acl.h.
09/25/01    om     Updates for forced routing.
08/30/02    om     Updates to use socket's routing cache.
08/01/02    om     Re-added temp. call to pppi_outgoing() to skip routing.
08/01/02    mvl    Removed ip_(de)reg_pkt_handler() and iface.h as they
                   weren't used.
07/31/02    om     Split fragmentation and utilities into separate files.
07/31/02    usb    Removed include psi.h
07/26/02    om     Update to include routing and fixes.
07/17/02    aku    Fixed bug in ip_free_reasm()
05/02/02    mvl    File cleanup and added meta_info to ip_send()
04/17/02    rc     Added support to call IP pkt handler function if registered
                   to process IP packets instead of the default ppp_outgoing() 
03/18/02    usb    Added dsm_free_packet in ip_fragment_handler() to free the
                   incoming fragment if dsm_pushdown fails (CR21183).
03/01/02    vsk    removed dsm_free_packet() being called in case of 
                   ppp_outgoing() returns an error
02/01/02    jay    2 bytes of offset is pushed before the dsm item is
                   enqueued.(CR19891) 
12/21/01    dwp    Add or !FEATURE_DATA_PS to some DSM tracer settings. This
                   is temporary until 5200 syncs up to latest DSM.
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
07/23/01    pjb    Return type of dsm_pushdown changed.  Changed the 
                   location/prefix of command names.   
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth 
                   parameter.Made changes to handle dsm_pushdown() 
                   returning FALSE. 
                   Handled the case when htonip() returns NULL.                  
06/08/00    rc     Removed ovld of link ptr in dsm items
05/20/00    snn    Added support for IP Reassembly.
05/04/00    mvl    Changed calls to ppp_outgoing() to reflect new interface.
08/17/99    smp    Added include files msg.h & err.h.
08/09/99    mvl    Added network model changes.
07/07/99    ak     In ip_route, check for ip fragments via the MF flag. If
                   a fragment, then dump ip packet (we do not support ip
                   fragmentation).
04/16/99    hcg    Fixed comment for ip_recv().  The function does not support
                   IP re-assembly.
02/24/99   na/hcg  Added DSSock sockets support.  Includes UDP support, so
                   UDP no longer exclusive to Browser interface.
01/09/99    jjw    changed to generic Browser interface
10/27/98    ldg    ARM porting changes.
01/27/98    ldg    Removed the DS_SLIM switch.
06/17/97    fkm    FEATURE_xxx Updates (via Automatic Scripts)
06/12/97    ldg    Added new FEATURE_DS_PSTATS
02/13/97    na     Removed the ps_ppp_tx_wm. CTIA show build.
11/14/96    jjw    Added changes to support Unwired Planet (UDP) operation
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#if defined (FEATURE_DS) || defined(FEATURE_DATA_PS)

#include "msg.h"
#include "err.h"
#include "assert.h"
#include "memory.h"
#include "errno.h"

#include "psglobal.h"
#include "dsm.h"
#include "internet.h"
#include "ps_ppp.h"
#include "psi.h"

#include "ps_icmp.h"
#include "ps_tcp.h"
#include "ps_udp.h"

#include "ps_acl.h"
#include "ps_inbound_acl.h"
#include "ps_route.h"

#include "ps_ip.h"
#include "ps_iputil.h"
#include "ps_iphdr.h"
#include "ps_ipfrg.h"

#ifdef FEATURE_SEC_IPSEC
#include "secipsesp.h"
#include "secipsiface.h"
#endif /* FEATURE_SEC_IPSEC */


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

struct mib_entry Ip_mib[20] =
{
 /*  */                     0,
 /* ipForwarding */         1,
 /* ipDefaultTTL */         MAXTTL,
 /* ipInReceives */         0,
 /* ipInHdrErrors */        0,
 /* ipInAddrErrors */       0,
 /* ipForwDatagrams */      0,
 /* ipInUnknownProtos */    0,
 /* ipInDiscards */         0,
 /* ipInDelivers */         0,
 /* ipOutRequests */        0,
 /* ipOutDiscards */        0,
 /* ipOutNoRoutes */        0,
 /* ipReasmTimeout */       TLB,
 /* ipReasmReqds */         0,
 /* ipReasmOKs */           0,
 /* ipReasmFails */         0,
 /* ipFragOKs */            0,
 /* ipFragFails */          0,
 /* ipFragCreates */        0,
};

/*---------------------------------------------------------------------------
  Array of stats names, stored as character strings.
  The strings could be defined within the array of name pointers,
  but the compiler would put them in CONST.
---------------------------------------------------------------------------*/
LOCAL const byte ip_stats_strings[IP_STATS_COUNT][16] =
{
  "InReceives     ",
  "InHdrErrors    ",
  "InUnknownProtos",
  "InDelivers     ",
  "OutPackets     "
};

/*---------------------------------------------------------------------------
  Array of pointers to stats names.
---------------------------------------------------------------------------*/
const byte * const ip_stats_name_ptr[IP_STATS_COUNT] =
{
  ip_stats_strings[0],
  ip_stats_strings[1],
  ip_stats_strings[2],
  ip_stats_strings[3],
  ip_stats_strings[4]
};

/* IP flag to en-/disable IP forwarding */
boolean ps_ip_forwarding = TRUE;

uint16 Id_cntr = 0;      /* Datagram serial number */


/*===========================================================================

                    PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION IP_SEND()

DESCRIPTION
  This function is used by the upper layers (UDP and TCP) to send
  an IP datagram. 
  (Modeled after the example interface on p 32 of RFC 791.)

DEPENDENCIES
  None

RETURN VALUE
  0 for success and -1 for failure (ps_errno contains error code)

SIDE EFFECTS
  None
===========================================================================*/
int ip_send
(
  ip4a               source,          /* source address                   */
  ip4a               dest,            /* Destination address              */
  uint8              protocol,        /* Protocol */
  uint8              tos,             /* Type of service */
  uint8              ttl,             /* Time-to-live */
  dsm_item_type*     send_dg_ptr,     /* Pointer to incoming datagram     */
  uint16             length,          /* Optional length of data portion  */
  uint16             id,              /* Optional identification          */
  uint8              df,              /* Don't-fragment flag */
  ps_meta_info_type* meta_info_ptr,   /* Metainfo about packet */
  ps_iface_type*     tx_if_cache,     /* TX interface cache entry */
  int16*             ps_errno         /* Error code for failure case */
)
{
  acl_ipfilter_info_type ip_filter_info;  /* IP info (hdr + policiy info ) */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize the ip packet info and save the socket's routing cache */
  memset( &ip_filter_info, 0, sizeof( acl_ipfilter_info_type ) );

  /* Check for UDP packets which contain per-packet routing info */
  if ( meta_info_ptr )
  {
    if ( meta_info_ptr->routing_cache
       &&
       (protocol == IPPROTO_UDP) )
  {
    /* UDP routing is per packet, use the meta-info's routing cache */
    ip_filter_info.if_cache = meta_info_ptr->routing_cache;
  }
  else
  {
    /* Everybody else has the routing info in the SCB */
    ip_filter_info.if_cache = tx_if_cache;
  }

    if ( meta_info_ptr->ipsec_required )
    {
      ip_filter_info.ipsec_required = TRUE;
    }
  }
  else
  {
    /* Everybody else has the routing info in the SCB */
    ip_filter_info.if_cache = tx_if_cache;
  }
  
  /* Use the ID to check for ESP processing */
  if ( (protocol == IPPROTO_TCP) && (id > 0) )
  {
    ip_filter_info.ipsec_required = TRUE;
    id = 0;
  }

  /* Update statistics */
  ipOutRequests++;

  /* Use implicit length of DSM item chain */
  if(length == 0 && send_dg_ptr != NULL)
  {
    length = dsm_length_packet(send_dg_ptr);
  }

  /* Set defaults for unspecified values */
  if(id == 0)
  {
    id = Id_cntr++;
  }
  if(ttl == 0)
  {
    ttl = ipDefaultTTL;
  }

  /* Fill in IP header */
  ip_filter_info.ip_vsn                  = IP_V4;
  ip_filter_info.ip_hdr.v4.version       = IPVERSION;
  ip_filter_info.ip_hdr.v4.tos           = tos;
  ip_filter_info.ip_hdr.v4.length        = IPLEN + length;
  ip_filter_info.ip_hdr.v4.id            = id;
  ip_filter_info.ip_hdr.v4.offset        = 0;
  ip_filter_info.ip_hdr.v4.flags.mf      = 0;
  ip_filter_info.ip_hdr.v4.flags.df      = df;
  ip_filter_info.ip_hdr.v4.flags.congest = 0;
  ip_filter_info.ip_hdr.v4.ttl           = ttl;
  ip_filter_info.ip_hdr.v4.protocol      = protocol;
  ip_filter_info.ip_hdr.v4.source        = source;
  ip_filter_info.ip_hdr.v4.dest          = dest;
  ip_filter_info.ip_hdr.v4.optlen        = 0;

  /*-------------------------------------------------------------------------
    Set filter interface to NULL indicating it's a locally 
    generated packet.
  -------------------------------------------------------------------------*/
  ip_filter_info.if_ptr = NULL;

  /*-------------------------------------------------------------------------
    Fill in IP header in network byte order. NOTE: The passed in 'send_dg_ptr'
    field may be changed to account for IP header bytes needed.
  -------------------------------------------------------------------------*/
  htonip( &send_dg_ptr, &(ip_filter_info.ip_hdr.v4), IP_CS_NEW);
  if (send_dg_ptr == NULL)
  {
    return 0;
  }

  send_dg_ptr->kind = DSM_PS_IP_COMPRESS;

  /*------------------------------------------------------------------------
    Removed the tcp_ppp_wm - so directly calling function instead of queueing
    item up in a watermark.
   ------------------------------------------------------------------------*/
#ifdef FEATURE_DSM_MEM_CHK
  send_dg_ptr->tracer = DSM_PPP_TX_WM;
#endif //FEATURE_DSM_MEM_CHK

  /* Find out which interface this should go to */
  return ip_route( &ip_filter_info, 
                   &meta_info_ptr,
                   &send_dg_ptr, 
                   ps_errno );

} /* ip_send() */


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
)
{
  /*----------------------------------------------------------------------
    Look at protocol field of Extracted IP header to determine the
    transport layer for processing and possible reply, either TCP or ICMP.
  ----------------------------------------------------------------------*/

  ipInDelivers++;

  switch( ip_filter_info_ptr->ip_hdr.v4.protocol)
  {
    case TCP_PTCL:
      tcp_input( ACL_IPFI_IF_PTR( ip_filter_info_ptr ),
                 &(ip_filter_info_ptr->ip_hdr.v4),
                 bp,
                 ip_filter_info_ptr->is_brdcst );
      break;

    case UDP_PTCL:
      udp_input( ACL_IPFI_IF_PTR( ip_filter_info_ptr ),
                 &(ip_filter_info_ptr->ip_hdr.v4),
                 bp,
                 ip_filter_info_ptr->is_brdcst );
      break;

    case ICMP_PTCL:
      icmp_input( ACL_IPFI_IF_PTR( ip_filter_info_ptr ),
                  &(ip_filter_info_ptr->ip_hdr.v4),
                  bp,
                  ip_filter_info_ptr->is_brdcst );
      break;

    case ESP_PTCL:
#ifdef FEATURE_SEC_IPSEC
      secipsesp_input( ACL_IPFI_IF_PTR( ip_filter_info_ptr ),
                       &(ip_filter_info_ptr->ip_hdr.v4),
                       bp,
                       ip_filter_info_ptr->is_brdcst );
#else /* FEATURE_SEC_IPSEC */
      MSG_ERROR( "IP: Got IPSec pkt w/o support", 0, 0, 0 );
      dsm_free_packet( &bp );
#endif /* FEATURE_SEC_IPSEC */
      break;

    default:
      ERR( "Invalid Protocol Field: %x", 
           ip_filter_info_ptr->ip_hdr.v4.protocol, 0, 0 );
      /*---------------------------------------------------------------------
        Free the packet.  
      ---------------------------------------------------------------------*/
      dsm_free_packet(&bp);
      ipInUnknownProtos++;
      break;
  }
} /* ip_receive() */


/*===========================================================================

FUNCTION IP_INPUT()

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
  ps_iface_type *in_if_ptr,           /* Input interface                   */
  dsm_item_type *bp,                  /* Input packet                      */
  uint8         *source,              /* Media source address, if any      */
  int            broadcast            /* Packet had link broadcast address */
)
{
  acl_ipfilter_info_type  ip_filter_info;    /* IP header being processed  */
  uint16                  ip_len;            /* IP header length */
  int16                   ps_errno;          /* Needed for call to route() */
  int                     result;            /* result of inbound acl proc */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( in_if_ptr == NULL )
  {
    /* Incoming interface cannot be NULL */
    ASSERT(0);
    dsm_free_packet( &bp );
    return -1;
  }

  /* Initialize the ip packet info */
  memset( &ip_filter_info, 0, sizeof( acl_ipfilter_info_type ) );
  ip_filter_info.if_ptr = in_if_ptr;

  ipInReceives++; /* Not locally generated */
  // STAT: in_if_ptr->InIP++;

  if(dsm_length_packet(bp) < IPLEN)
  {
    /* The packet is shorter than a legal IP header */
    ipInHdrErrors++;
    dsm_free_packet(&bp);
    return -1;
  }

  /* Sneak a peek at the IP header's IHL field to find its length */
  ip_len = (bp->data_ptr[0] & 0xf) << 2;
  if(ip_len < IPLEN)
  {
    /* The IP header length field is too small */
    ipInHdrErrors++;
    dsm_free_packet(&bp);
    return -1;
  }

  if(cksum(NULL,bp,ip_len) != 0)
  {
    /* Bad IP header checksum; discard */
    ipInHdrErrors++;
    dsm_free_packet(&bp);
    return -1;
  }

  /*-----------------------------------------------------------------------
    Get the IP header but don't actually remove it from the dsm item in 
    case the pkt requires forwarding instead of being sent up the stack.
  -----------------------------------------------------------------------*/
  ntohip( &(ip_filter_info.ip_hdr.v4), &bp, TRUE );

  if ( ip_filter_info.ip_hdr.v4.version != IP_V4 )
  {
    /* We can't handle this version of IP */
    ipInHdrErrors++;
    dsm_free_packet(&bp);
    return -1;
  }
   
  ip_filter_info.ip_vsn = (ip_version_enum_type)ip_filter_info.ip_hdr.v4.version;

  /*-------------------------------------------------------------------------
    If this is a non fragmented pkt or first fragment of a fragmented 
    pkt, extract the protocol specific information from the pkt
  -------------------------------------------------------------------------*/
  if(ip_filter_info.ip_hdr.v4.offset == 0)
  {
    switch( ip_filter_info.ip_hdr.v4.protocol )
    {
      case TCP_PTCL:
        dsm_extract( bp,
                     ip_len,
                     (void*) &(ip_filter_info.ptcl_info.tcp.src_port), 
                     2 * sizeof( uint16 ) );
        break;

      case UDP_PTCL:
        dsm_extract( bp,
                     ip_len,
                     (void*) &(ip_filter_info.ptcl_info.udp.src_port), 
                     2 * sizeof( uint16 ) );
        break;

      case ICMP_PTCL:
        dsm_extract( bp,
                     ip_len,
                     (void*) &(ip_filter_info.ptcl_info.icmp.type), 
                     2 * sizeof( uint8 ) );
        break;

      default:
        ERR( "IPF: Unknown protocol %d",
             ip_filter_info.ip_hdr.v4.protocol, 0, 0 );
        dsm_free_packet( &bp );
        return -1;
    }
  }

  /* Apply the receiving ipfilter ACL */
  result = ACL_EXEC( ps_inbound_acl, &ip_filter_info, NULL, in_if_ptr );

  if(result > ACL_DEFAULT_CLASS)
  {
    MSG_LOW( "IPF: Got local packet from 0x%x",
             ACL_IPFI_IF_PTR( &ip_filter_info ), 0, 0 );
    ip_filter_info.is_local = TRUE;
  }
  else if(result == ACL_DENY)
  {
    MSG_HIGH( "IPF: Dropping ip pkt from 0x%x",
              ACL_IPFI_IF_PTR( &ip_filter_info ), 0, 0 );
    dsm_free_packet( &bp );
    return -1;  
  }
  else
  {
    /* Deliver the packet to it's destination */
    MSG_LOW( "IPF: Got routable packet from 0x%x",
              ACL_IPFI_IF_PTR( &ip_filter_info ), 0, 0 );
    return ip_route( &ip_filter_info, NULL, &bp, &ps_errno );
  }

  /*-------------------------------------------------------------------------
    Since the packet needs to go up the stack discard the IP header from it.
  -------------------------------------------------------------------------*/
  dsm_pullup(&bp, NULL, ip_len );

  /*-------------------------------------------------------------------------
    Check if we received a fragment. 
    The following table shows what should be done for handling fragments:
    
    ip.flags.mf   ip.offset    Comments
    0              0           This is a complete datagram with no fragments.
                               So, no need to handle these fragments.
    1              0           We received the starting fragment of the 
                               datagram. Handle it. Store the ip header of
                               this fragment and pass it to upper layer.
    0              !0          We received the last fragment of the datagram
                               Handle it. Use this to calculate the total
                               length of the incoming datagram.
    1              !0          We received some intermittent fragment of the 
                               datagram. Handle it.
    
    Note that following cases are not taken care by the implementation:
    1. We donot handle any overlapping of the data in different fragments.
       It is left to upper layers to thrash it off.
    2. We donot have any time outs for fragments. If we donot receive any 
       fragment of a datagram, we wait until the entire reassembly array is
       full. We donot time out , but leave it to the application to take 
       care of it.
  -------------------------------------------------------------------------*/
  if ( !( (ip_filter_info.ip_hdr.v4.flags.mf == 0)
          &&
          (ip_filter_info.ip_hdr.v4.offset == 0) ) )
  {
    /*-----------------------------------------------------------------------
      Received IP fragments process them before giving it to other layers
    -----------------------------------------------------------------------*/
    MSG_HIGH("Got IP fragment to handle",0,0,0);

#if defined (FEATURE_DSM_MEM_CHK) && !defined (FEATURE_DATA_PS)
    bp->tracer = DSM_IP_FRAG_LIST;
#endif //FEATURE_DSM_MEM_CHK

    if ( (bp = ip_fragment_handler( &(ip_filter_info.ip_hdr.v4), bp )) 
         ==
         NULL )
    {
      return 0;
    }
  }

  /* Deliver the packet to the local stack */
  ip_receive( &ip_filter_info, bp );

  return 0;
}  /* ip_input() */


/*===========================================================================

FUNCTION IP_ROUTE()

DESCRIPTION
  This function finds the appropriate interface to send a
  given IP packet on and forwards it appropriately.

DEPENDENCIES
  None

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code)

   ps_errno can contain:
   E_NOTALLOWED - Pkt forwarding is disabled
   E_NOROUTE    - No route to destination could be found
   E_NONET      - Route to destination is cuurently not accessible

SIDE EFFECTS
  None
===========================================================================*/
int ip_route
(
  acl_ipfilter_info_type*  ip_filter_info_ptr, /* Pkt and policy info */
  ps_meta_info_type**      meta_info_ptr,      /* Policy info e.g. from SCB */
  dsm_item_type**          ip_pkt_ptr,         /* Input packet */
  int16*                   ps_errno            /* Error code for faillure */
)
{
  ps_iface_type*  rt_if_ptr = NULL;
  ip_addr_type    gateway;
  int             ret_val = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  do
  {
    /* Check if this host allows forwarding between interfaces. */
    /* The if_ptr in the filter info is set to the receiving    */
    /* interface or NULL for locally generated packets.         */
    if ( ACL_IPFI_IF_PTR( ip_filter_info_ptr )
         &&
         ! ps_ip_forwarding )
    {
      MSG_MED( "IPRoute: Not allowed to fwd pkt from %d",
               (ACL_IPFI_IF_PTR( ip_filter_info_ptr ) ?
                ACL_IPFI_IF_PTR( ip_filter_info_ptr )->name : -1), 0, 0 );

      *ps_errno = E_NOT_ALLOWED;
      ret_val = -1;
      break;
    }

    /* Check if this is a packet from a system app and force */
    /* routing via the cached interface.                     */
    if ( meta_info_ptr && *meta_info_ptr )
    {
      if ( IS_SOCKOPT_SET((*meta_info_ptr)->sock_opts, DSS_SO_SYS_SOCK) )
      {
        (*meta_info_ptr)->policy_info.force_route = TRUE;
      }
    }
    
    /* Get the route for this destination and deliver pkt to acc. interface */
    if ( route_get( ip_filter_info_ptr,
                    (meta_info_ptr ? 
                       (*meta_info_ptr ? &((*meta_info_ptr)->policy_info) 
                                       : NULL)
                       : NULL),
                    FALSE,
                    &gateway,
                    &rt_if_ptr ) == -1 )
    {
      MSG_HIGH( "IPRoute: No route, discard pkt from %d",
                (ACL_IPFI_IF_PTR( ip_filter_info_ptr ) ? 
                 ACL_IPFI_IF_PTR( ip_filter_info_ptr )->name : -1 ), 
                0, 0 );

      *ps_errno = E_NO_ROUTE;
      break;
    }
    
    /* Save the routing result in the ip filter info */
    ip_filter_info_ptr->if_ptr = (void*) rt_if_ptr;

#ifdef FEATURE_SEC_IPSEC
    /* Check if the packet needs IPSec processing first */
    if ( ip_filter_info_ptr->ipsec_required )
    {
      rt_if_ptr = &secips_iface;
    }
#endif

    /* Call the interface's write function w/ the next hop's address */
    ret_val = ps_iface_tx_cmd(
                rt_if_ptr,
                ip_pkt_ptr, 
                meta_info_ptr );
  } while( 0 );

  if ( ret_val == -1 )
  {    
    dsm_free_packet( ip_pkt_ptr );
    PS_META_INFO_FREE( meta_info_ptr );
  }

  return ret_val;

}  /* ip_route() */


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
-==========================================================================*/
void ip_get_stats
(
  word *stats_ptr /* Pointer to array into which stats will be copied */
)
{
  stats_ptr[IP_STATS_IN_RECEIVES] = (word)ipInReceives;
  stats_ptr[IP_STATS_IN_HDR_ERRORS] = (word)ipInHdrErrors;
  stats_ptr[IP_STATS_IN_UNKNOWN_PROTOS] = (word)ipInUnknownProtos;
  stats_ptr[IP_STATS_IN_DELIVERS] = (word)ipInDelivers;

  stats_ptr[IP_STATS_OUT_REQUESTS] = (word)ipOutRequests;
} /* ip_get_stats() */


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
void ip_reset_stats(void)
{
  ipInReceives = 0;
  ipInHdrErrors = 0;
  ipInUnknownProtos = 0;
  ipInDelivers = 0;

  ipOutRequests = 0;
} /* ip_reset_stats() */

#endif /* FEATURE_DS || FEATURE_DATA_PS */
