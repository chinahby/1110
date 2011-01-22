/*===========================================================================

                                  U D P . C

DESCRIPTION

  The Data Services UDP protocol interface file. This contains all the 
  functions used by the data services to use the UDP protocol.
 
EXTERNALIZED FUNCTIONS
  udp_output()
    Sends UDP packet out to IP, performing pseudo-header checksum 
    calculation.

  udp_input()
    Called from IP layer, and hands off UDP packet to UDP protocol layer.

Copyright (c) 1998-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_udp.c_v   1.6   12 Feb 2003 20:35:44   omichael  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_udp.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/28/03    ss     Modified udp_input() to pass payload length to 
                   process_rx_pkt
04/23/03    ss     Modified code to reflect new socket options handling
04/15/03    sv     Changed udp_input to search for a valid socket before
                   calling dssudp_rx_packet
02/12/03    om     Removed routing dependency on ACB.
10/17/02    ss     Uncommented the code in udp_input() to drop malformed pkts
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
09/10/02    aku    Removed global Ip_addr.
08/30/02    om     Updates to use socket's routing cache.
07/31/02    usb    Added include ps_iputil.h
07/31/02    usb    Removed include psi.h
07/21/02    na     Fixed cut & paste extra code 
07/16/02    aku    Include dssudp.h and dsbyte.h 
07/15/02    na     Added support for FEATURE_DATA_MM.
05/02/02    mvl    File cleanup and added meta_info to output() and changed
                   calls to ip_send() to reflect new prototype
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
07/23/01    pjb    Return type of dsm_pushdown changed. 
02/14/01    js     Modified udp_output() to support socket option 
                   DSS_IP_TTL. 
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth 
                   parameter.Made changes to handle dsm_pushdown() 
                   returning FALSE.                                    
05/20/00    snn    Removed some code to support the IP Fragmentation.
08/05/99    rc     Added support for UDP debug statistics for Sockets.
07/07/99    ak     In udp_input, if udp length (as spec'd in hdr) does not
                   match ip pkt length (as spec'd in hdr), then dump pkt.
                   Should help guard against fragments (if it passes chksum
                   test).  Also, in udp_output, now allow fragmentation - 
                   DF bit is not set.
04/19/99    hcg    Added check to ensure that payload length was same size
                   as the packet length.  Fixed memory leak caused by 
                   mis-sized packets being sent from faulty IWF.
04/05/99    hcg    Incorporated code review changes and comments.  Moved
                   definition of maximum UDP payload length to udp.h.
12/09/98   na/hcg  Created module.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE
                       
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#if defined (FEATURE_DS) || defined (FEATURE_DATA_PS)
#if defined(FEATURE_DS_SOCKETS)
#include "assert.h"
#include "psglobal.h"
#include "netuser.h"
#include "dsm.h"
#include "ps_udp.h"
#include "ps_ip.h"
#include "ps_iputil.h"
#include "msg.h"
#include "err.h"
#include "dssudp.h"
#include "dsbyte.h"


/*===========================================================================

                         GLOBAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION UDP_OUTPUT()

DESCRIPTION
  This function does the following:
  1. Converts from host to network all the elements of the UDP header
     and inserts the UDP header in the UDP packet.  Host to network 
     conversion is implicit to the put16() function.
  2. Creates the pseudo-header and calculates UDP checksum.
  3. Sends UDP packet down to IP.

DEPENDENCIES
  The source IP address should be correct, i.e, IPCP should have negotiated
  the right IP address when this function is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void udp_output
(
  ip4a               *dest_ip_ptr,                  /* Destination IP addr */
  udp_hdr_type       *udp_hdr_ptr,                  /* UDP header          */
  dsm_item_type      *item_ptr,                     /* UDP payload         */
  struct udp_cb      *udp_cb_ptr,                   /* UDP control block   */
  ps_meta_info_type  *meta_info_ptr                 /* ptr to meta info    */
)
{
  pseudo_header_type  ph;                   /* pseudo header               */
  register byte      *cp;                   /* Used to store temp pointers */
  uint16              checksum;             /* stores UDP checksum         */
  int16               ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate UDP protocol header and fill it in.
  -------------------------------------------------------------------------*/
  if (dsm_pushdown(&item_ptr,
                   NULL,
                   sizeof( udp_hdr_type),
                   DSM_DS_SMALL_ITEM_POOL) < sizeof( udp_hdr_type))
  {
    ERR("Out of Memory!",0,0,0);
    dsm_free_packet(&item_ptr);
    PS_META_INFO_FREE(&meta_info_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Fill in header - Fields are converted from host to network byte 
    ordering in the put16() function, found in psmisc.c.
  -------------------------------------------------------------------------*/
  cp = item_ptr->data_ptr;
  cp = put16(cp,udp_hdr_ptr->source);                       /* Source port */
  cp = put16(cp,udp_hdr_ptr->dest);                    /* Destination port */
  cp = put16(cp,udp_hdr_ptr->length);                            /* Length */
  *cp++ = 0;                                             /* Clear checksum */
  *cp-- = 0;                              /* Set ptr to beginning of cksum */

  /*-------------------------------------------------------------------------
    Make up pseudo header for checksum calculation
  -------------------------------------------------------------------------*/
  ph.length   = udp_hdr_ptr->length;
  ph.source = (meta_info_ptr ? 
                 PS_IFACE_IP_V4_ADDR( 
                   ((ps_iface_type*) (meta_info_ptr->routing_cache)) ) 
               : 0);
  ph.dest     = *dest_ip_ptr;
  ph.protocol = UDP_PTCL;                               

  /*------------------------------------------------------------------------
    All zeros and all ones is equivalent in one's complement arithmetic;
    the spec requires us to change zeros into ones to distinguish an
    all-zero checksum from no checksum at all.
  -------------------------------------------------------------------------*/
  if((checksum = cksum(&ph,item_ptr,ph.length)) == 0)
  {
    checksum = 0xffff;
  }
  (void) put16(cp,checksum);

#ifdef FEATURE_DSM_MEM_CHK
  item_ptr->tracer = DSM_IP_SND_PROC;
#endif /* FEATURE_DSM_MEM_CHK */

#ifdef FEATURE_DS_SOCKETS

  /*-----------------------------------------------------------------------
    If UDP control block is not NULL, further checking whether or not scb
    pointer is NULL. If so, call ip_send() with default TTL. 
  -----------------------------------------------------------------------*/
  if (udp_cb_ptr != NULL && udp_cb_ptr->scb_ptr != NULL)
  {
    ASSERT(udp_cb_ptr->scb_ptr->sockfd != SOCKAVAIL);
  }

  (void)ip_send( ph.source,
                 ph.dest,
                 UDP_PTCL, 
                 0,
                 udp_cb_ptr && udp_cb_ptr->scb_ptr ? 
                   udp_cb_ptr->scb_ptr->ip_ttl : 0,
                 item_ptr,
                 ph.length,   
                 0,
                 0,
                 meta_info_ptr,
                 udp_cb_ptr && udp_cb_ptr->scb_ptr ? 
                   udp_cb_ptr->scb_ptr->routing_cache : NULL,
                 &ps_errno);

#else /* if FEATURE_DS_SOCKETS is not defined */

  (void)ip_send( ph.source,
                 ph.dest,
                 UDP_PTCL, 
                 0,
                 0,
                 item_ptr,
                 ph.length,   
                 0,
                 0,
                 meta_info_ptr,
                 NULL,
                 &ps_errno);

#endif /* FEATURE_DS_SOCKETS */

}  /* udp_output */



/*===========================================================================
FUNCTION UDP_INPUT()

DESCRIPTION
  This function is called from the IP layer and hands off the UDP packet to
  the UDP protocol layer.
  
  This function runs in the context of the PS task.
  
  It strips off the UDP header, checks the checksum and then passes the UDP
  Payload to the application layer.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void udp_input
(
  struct ps_iface_s  *iface,       /* Incoming interface (ignored)         */
  struct ip          *ip,          /* IP header                            */
  struct dsm_item_s  *data_ptr,    /* Data field, if any                   */
  int                 rxbroadcast  /* Incoming broadcast - discard if true */
)
{
  pseudo_header_type ph;           /* pseudo header for calc'ing checksum  */
  word               calc_checksum;                 /* Calculated checksum */
  udp_hdr_type       udp_hdr;                     /* The UDP header fields */
  word               udp_payload_len;   /* payload length, from udp header */
  uint32             dsm_payload_len;   /* payload length from the dsm pkt */
  struct udp_cb    * ucb_ptr;                /* UDP protocol control block */
  struct connection  conn;               /* temp connection variable       */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Extract the udp header from the UDP packet. Check to see if the header 
    sent was accurate.
  -------------------------------------------------------------------------*/
  if ( dsm_extract( data_ptr, 0, &udp_hdr, sizeof( udp_hdr_type)) 
       != sizeof( udp_hdr_type)) 
  {
      ERR( "UDP packet has an invalid header" ,0, 0, 0);
      dsm_free_packet( &data_ptr);
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif
      return;
  }

  /*-------------------------------------------------------------------------
    Check if the checksum field is all zeros. If it is zeros, the sender did
    not calculate the checksum, so we do not verify it.
    Note, there is no need to convert the checksum to host format because
    we are checking it against symmetric values ( 0 and FFFF).
  -------------------------------------------------------------------------*/
  if ( udp_hdr.chksum != 0)
  {

    /*-----------------------------------------------------------------------
      Build pseudo header then calculate UDP checksum. Pushdown calculated
      UDP checksum and destination IP address for use in next layer 
      (DevRevPDU) then queue and tell UI a UP client UDP packet is
       available.
    -----------------------------------------------------------------------*/
    ph.source     = ip->source;
    ph.dest       = ip->dest;
    ph.protocol   = ip->protocol;
    ph.length     = ip->length - IPLEN - ip->optlen;

    calc_checksum = cksum( &ph, data_ptr, ph.length);

    if ( calc_checksum != 0)
    {
      MSG_HIGH( "Found bad UDP checksum" ,0, 0, 0);
      dsm_free_packet( &data_ptr);
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif
      return;
     }

  } /* udp_hdr cksum != 0 */

  /*-------------------------------------------------------------------------
    Pullup the udp header from the UDP packet. Check to see if the header 
    sent was accurate.
  -------------------------------------------------------------------------*/
  if ( dsm_pullup( &data_ptr, &udp_hdr, sizeof( udp_hdr_type)) 
                 != sizeof( udp_hdr_type)) 
  {
    ERR( "UDP packet has an invalid header" ,0, 0, 0);
    dsm_free_packet( &data_ptr);
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif
    return;
  }

  /*-------------------------------------------------------------------------
    Check the length field of the udp header. Check if the length is more 
    than the max. payload length. Compare the length specified 
    in the header against the actual length in the payload. If the header
    length is more than the actual length, dump the packet. Note, if the 
    header length was less than the actual payload then it is OK to send
    the packet to the UP layer.
  -------------------------------------------------------------------------*/
  udp_payload_len = get16((byte *) &(udp_hdr.length)) - sizeof(udp_hdr_type);

  /*-------------------------------------------------------------------------
    There is nothing much to be done if the payload length is 0. Simply free
    the packet, nothing to put on the rcvq
  -------------------------------------------------------------------------*/
  if(udp_payload_len == 0)
  {
    dsm_free_packet(&data_ptr);
    return;
  }

  dsm_payload_len = dsm_length_packet( data_ptr );
  
  /*-------------------------------------------------------------------------
    Drop the packet if it is malformed.
  -------------------------------------------------------------------------*/
  if ( udp_payload_len > dsm_payload_len )
  {
    ERR(" Length specified in UDP hdr is invalid", 0,0, 0);
    dsm_free_packet( &data_ptr);
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif
    return;
  }

  /*-------------------------------------------------------------------------
     Check to see that the IP length and UDP length jibe.  This will
     further help weed out IP fragments.

  ph.length = ip->length - IPLEN - ip->optlen - (uint16)sizeof(udp_hdr_type);

  if (ph.length != udp_payload_len)
  {
     ERR("UDP hdr len != IP len %d %d",
          udp_payload_len,
          ph.length, 
          0
     );
     dsm_free_packet( &data_ptr);
     return;
  }
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Ensure that the actual payload length is the size of the packet.  If
    not, trim the packet to the correct length.  This prevents memory leaks
    caused by faulty IWF's that may incorrectly insert additional bytes
    due to their own memory leaks.
  -------------------------------------------------------------------------*/
  if ( udp_payload_len < dsm_payload_len )
  {
    dsm_trim_packet( &data_ptr, udp_payload_len);
  }

  conn.local.address  = ip->dest;
  conn.local.port     = dss_ntohs(udp_hdr.dest);
  conn.remote.address = ip->source;
  conn.remote.port    = dss_ntohs(udp_hdr.source);

  ucb_ptr = dssudp_lookup_conn(&conn);

  if(ucb_ptr == NULL)
  {
    dsm_free_packet(&data_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Push the IP source address, port number and the payload length on the
    item ptr. These fields are required by the application layer.
  -------------------------------------------------------------------------*/
  if ((dsm_pushdown( &data_ptr, &ip->source, sizeof(ip->source),
                     DSM_DS_SMALL_ITEM_POOL ) != sizeof(ip->source)) ||
      (dsm_pushdown( &data_ptr, &(udp_hdr.source), sizeof(udp_hdr.source),
                     DSM_DS_SMALL_ITEM_POOL ) != sizeof(udp_hdr.source)) ||
      (dsm_pushdown( &data_ptr, &udp_payload_len, sizeof(udp_payload_len),
                     DSM_DS_SMALL_ITEM_POOL ) != sizeof(udp_payload_len)))
  {
    ERR("Out of Memory!", 0, 0, 0);
    dsm_free_packet( &data_ptr);
    return;
  }

  dssudp_process_rx_pkt( ucb_ptr, data_ptr, udp_payload_len ); 
} /* udp_input() */

#endif /* FEATURE_DS_SOCKETS */
#endif /* FEATURE_DS || FEATURE_DATA_PS */
