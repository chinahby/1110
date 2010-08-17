/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                 I C M P . C

GENERAL DESCRIPTION
 Internet Control Message Protocol (ICMP)
  Copyright 1991 Phil Karn, KA9Q
 

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


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1995-2008 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_icmp.c_v   1.5   12 Feb 2003 20:35:30   omichael  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_icmp.c#3 $ $DateTime: 2008/06/27 15:37:54 $ $Author: praveenp $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/28/08    pp     Fixed RVCT compiler warnings.
04/03/08    pp     Klocwork fixes.
09/11/03    sv     Changed icmpi_ntoh to return ERROR if we are unable
                   to push the complete ICMP header.
04/23/03    ss     Modified code to reflect new socket options handling
02/12/03    om     Use SCB's routing cache only.
11/25/02    usb    Added routing cache for icmp sockets.
09/10/02    aku    Removed global Ip_addr.
08/30/02    om     Updates to use incoming iface for replies, routing
                   for outgoing requests.
07/31/02    usb    Added include ps_iputil.h
07/31/02    usb    Renamed psmisc.h to ps_utils.h
05/02/02    mvl    File cleanup and changed calls to ip_send() to reflect new
                   prototype
02/12/02    dwp    Cleanup header inclusion(s) for non DS builds.
12/21/01    dwp    Add or !FEATURE_DATA_PS to some DSM tracer settings. This 
                   is temporary until 5200 syncs up to latest DSM.
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
07/30/01    mvl    Modified a message to give info on unrecognized ICMP msgs
07/23/01    pjb    Return type of dsm_pushdown changed.  
07/13/01    mvl    Fixed compile bug so will compile with sockets feature off
06/04/01  jd/mvl   resolved IP address inconsistency, temp. fix removed
05/10/01    jd     Added temporary fix to get correct destination IP addr in
                   icmp_output
05/10/01    jd     Removed extra call to ip_send() in icmp_output() by making 
                   ttl parameter a variable.
03/27/01    js     Updated module to support arbitrary ICMP message types. 
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth 
                   parameter.Made changes to handle dsm_pushdown() 
                   returning FALSE.
                   Handled the case when icmpi_hton()() returns NULL.                                    
01/27/98    ldg    Removed the DS_SLIM switch.
11/06/97    ldg    Removed 'extern' keyword from function definition.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
06/12/97    ldg    Added new FEATURE_DS_PSTATS
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#if defined (FEATURE_DS) || defined (FEATURE_DATA_PS)
#include "psglobal.h"
#include "dsm.h"
#include "ps_iface.h"
#include "ps_ip.h"
#include "ps_iputil.h"
#include "ps_icmp.h"
#include "netuser.h"
#include "ps_utils.h"
#include "msg.h"
#include "err.h"
#include "dssocki.h"
#include "dssocket.h"
#include "assert.h"
#include "dssicmp.h"
#include <memory.h>

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      PUBLIC FUNCTION DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================

FUNCTION ICMPI_NTOHICMP()

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
)
{
  struct icmp_hdr curr_icmp_hdr;
  union icmp_args curr_icmp_args;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Pullup the generic icmp header and store in the temporary header struct. 
  -------------------------------------------------------------------------*/
  if (dsm_pullup( bpp, 
                  (byte *) &curr_icmp_hdr, 
                  sizeof( struct icmp_hdr)) != sizeof( struct icmp_hdr))
  {
      ERR("Error in dsm_pullup()",0,0,0);
      return -1;
  }
  /*-------------------------------------------------------------------------
    Get type and code information. 
  -------------------------------------------------------------------------*/
  icmp->info.type = curr_icmp_hdr.type;
  icmp->info.code = curr_icmp_hdr.code;

  /*-------------------------------------------------------------------------
    Process arguments based on type. 
  -------------------------------------------------------------------------*/
  switch( icmp->info.type)
  {
    case ICMP_ECHO:
    case ICMP_TIMESTAMP:
    case ICMP_INFO_RQST:
      /*---------------------------------------------------------------------
       Pullup the icmp arguments and store in the temporary argument struct. 
      ---------------------------------------------------------------------*/
      if (dsm_pullup( bpp, 
                      (byte *) &curr_icmp_args, 
                      sizeof( struct echo_s)) != sizeof( struct echo_s))
      {
        ERR("Error in dsm_pullup()",0,0,0);
        return -1;
      }
      icmp->arg.echo.id  = ntohs(curr_icmp_args.echo.id);
      icmp->arg.echo.seq = ntohs(curr_icmp_args.echo.seq);
      break;
    
    default:
      MSG_MED( "ICMP msg type %d rx'd, not handled internally", icmp->info.type, 0, 0);
      /*---------------------------------------------------------------------
        Push down the ICMP header information (type, code, and 0 
        checksum) again to reflect the changes. This action is
        not performed for the pre-defined types, because for those
        types, we don't care about the header info. 
      ---------------------------------------------------------------------*/
      if (dsm_pushdown( bpp, 
                        (byte *) &curr_icmp_hdr, 
                        sizeof( struct icmp_hdr), 
                        DSM_DS_SMALL_ITEM_POOL) != sizeof(struct icmp_hdr))
      {
        ERR("Out of Memory",0,0,0);
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif
        return -1;
      }
      break; 
  }

  /*------------------------------------------------------------------------
     Everything was successful! Terminate normally. 
  ------------------------------------------------------------------------*/
  return 0;
} /* icmpi_ntoh */

/*===========================================================================

FUNCTION ICMPI_HTON()

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
  struct icmp   *icmp,           /* Extracted ICMP header data             */
  dsm_item_type **bp             /* address of pointer to ICMP packet data */
)
{
  uint16          checksum;
  struct icmp_hdr curr_icmp_hdr;
  union icmp_args curr_icmp_args;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif

  /*-------------------------------------------------------------------------
    Fill in generic icmp header fileds into the temporary hdr. 
  -------------------------------------------------------------------------*/
  curr_icmp_hdr.type = icmp->info.type;
  curr_icmp_hdr.code = icmp->info.code;
  curr_icmp_hdr.chksum =0;                             /* Clear checksum  */

  /*-------------------------------------------------------------------------
    Depending on the type of the ICMP packet we are dealing with, fill in
    the argument fields into the temporary argument structure accordingly. 
    Then pushdown the arguments. 
  -------------------------------------------------------------------------*/
  switch( icmp->info.type )
  {
    case ICMP_TIME_REPLY:
    case ICMP_ECHO_REPLY:
    case ICMP_INFO_REPLY:
       curr_icmp_args.echo.id = htons(icmp->arg.echo.id);
       curr_icmp_args.echo.seq =htons( icmp->arg.echo.seq);
       /*--------------------------------------------------------------------
         Pushdown the ICMP arguments. Free pointer if error occured. 
       --------------------------------------------------------------------*/
       if (dsm_pushdown( bp, 
                         (byte *) &curr_icmp_args, 
                         sizeof( struct echo_s),
                         DSM_DS_SMALL_ITEM_POOL) < sizeof( struct echo_s))
       {
         ERR("Out of Memory",0,0,0);
         return -1;
       }
      break;

    default:
      break;
  }/* switch */

  /*------------------------------------------------------------------------
    Push down the generic ICMP header information (type, code, and 0 
    checksum). Free packet if error occured. 
  ------------------------------------------------------------------------*/
  if (dsm_pushdown( bp, 
                    (byte *) &curr_icmp_hdr, 
                    sizeof( struct icmp_hdr), 
                    DSM_DS_SMALL_ITEM_POOL) < sizeof (struct icmp_hdr))
  {
    ERR("Out of Memory",0,0,0);
    return -1; 
  }

  /*------------------------------------------------------------------------- 
    Compute checksum, and stash result 
  -------------------------------------------------------------------------*/
  checksum = cksum( NULL, *bp, dsm_length_packet( *bp));

  /*-------------------------------------------------------------------------
    Pullup the generic icmp header and store in the temporary header struct. 
    Free packet if errors occured. 
  -------------------------------------------------------------------------*/
  if (dsm_pullup( bp, 
                  (byte *) &curr_icmp_hdr, 
                  sizeof( struct icmp_hdr)) != sizeof( struct icmp_hdr))
  {
      ERR("Error in dsm_pullup()",0,0,0);
      return -1;
  }

  /*-------------------------------------------------------------------------
    Converted the computed checksum from host to network order. 
  -------------------------------------------------------------------------*/
  curr_icmp_hdr.chksum = htons( checksum );

  /*------------------------------------------------------------------------
    Push down the ICMP header information (type, code, and 0 
    checksum) again to reflect the changes. Free packet if 
    error occured. 
  ------------------------------------------------------------------------*/
  if (dsm_pushdown( bp, 
                    (byte *) &curr_icmp_hdr, 
                    sizeof( struct icmp_hdr), 
                    DSM_DS_SMALL_ITEM_POOL) < sizeof(struct icmp_hdr))
  {
    ERR("Out of Memory",0,0,0);
    return -1;
  }

  return 0; 
} /* icmpi_hton() */

/*===========================================================================

FUNCTION ICMP_INPUT()

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
  struct ps_iface_s  *iface,     /* Incoming interface (tx_iface for reply)*/
  struct ip          *ip,        /* Pointer to decoded IP header structure */
  struct dsm_item_s  *item_ptr,  /* Pointer to ICMP message */
  int                 rxbroadcast 
)
{
  struct icmp     icmp;  /* ICMP header */
  uint16 type;           /* Type of ICMP message */
  uint16 length;
  int16  ps_errno;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif

  /*-------------------------------------------------------------------------
    Check if broadcast ICMP packets are to be ignored. 
  -------------------------------------------------------------------------*/
  if( rxbroadcast )
  {
    MSG_LOW( "ICMP Broadcast Rx'd, Ignored", 0, 0, 0);
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif
    dsm_free_packet( &item_ptr );
    return;
  }
  
  /*-------------------------------------------------------------------------
    Checksum verification. If not match, discard item_ptr. 
  -------------------------------------------------------------------------*/
  length = ip->length - IPLEN - ip->optlen;
  if( cksum( NULL,item_ptr,length) != 0)
  {
    MSG_HIGH("Invalid ICMP Checksum, Discard", 0, 0, 0);
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif
    dsm_free_packet( &item_ptr);
    return;
  }
  
  /*-------------------------------------------------------------------------
    Get icmp header from the DSM pool.
  -------------------------------------------------------------------------*/
  if ( icmpi_ntoh( &icmp, &item_ptr) == -1)
  {
    MSG_HIGH("icmpi_ntoh conversion err, discard", 0, 0, 0); 
    dsm_free_packet( &item_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Get type of the icmp msg. 
  -------------------------------------------------------------------------*/
  type = icmp.info.type;

  /*-------------------------------------------------------------------------
    Process the only valid ICMP messages: Echo Request (ping), Timestamp, and
    Info request.
  -------------------------------------------------------------------------*/
  switch( type )
  {
    case ICMP_ECHO:  
      /*-------------------------------------------------------------------
        Change type to ECHO_REPLY, recompute checksum, and return datagram.
      -------------------------------------------------------------------*/
      MSG_MED( "ICMP Echo Request Rx'd", 0, 0, 0);
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif
      icmp.info.type = ICMP_ECHO_REPLY;
      if ( icmpi_hton( &icmp, &item_ptr) == -1)
      {
        MSG_HIGH("icmpi_hton() conversion err, discard", 0, 0, 0); 
        dsm_free_packet( &item_ptr);
        return;
      }
      if (item_ptr != NULL)
      {
#if defined (FEATURE_DSM_MEM_CHK) && !defined (FEATURE_DATA_PS)
        item_ptr->tracer = DSM_IP_SND_PROC;
#endif //FEATURE_DSM_MEM_CHK
        ip_send(ip->dest,
                ip->source,
                ICMP_PTCL,
                ip->tos,
                0,
                item_ptr,
                length,
                0,
                0,
                NULL,
                iface,
                &ps_errno);
      }
      break;

    case ICMP_TIMESTAMP:        
      /*-------------------------------------------------------------------
        Change type to TIME_REPLY, recompute checksum, and return datagram.
        Note the source/dest addresses will be swapped by IP processing.
      -------------------------------------------------------------------*/
      MSG_MED( "ICMP Timestamp Rx'd", 0, 0, 0);
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif
      icmp.info.type = ICMP_TIME_REPLY;
      if ( icmpi_hton( &icmp, &item_ptr) == -1)
      {
        MSG_HIGH("icmpi_hton() conversion err, discard", 0, 0, 0); 
        dsm_free_packet( &item_ptr);
        return;
      }
      if (item_ptr != NULL)
      {
#if defined (FEATURE_DSM_MEM_CHK) && !defined (FEATURE_DATA_PS)
      item_ptr->tracer = DSM_IP_SND_PROC;
#endif //FEATURE_DSM_MEM_CHK
        ip_send(ip->dest,
                ip->source,
                ICMP_PTCL,
                ip->tos,
                0,
                item_ptr,
                length,
                0,
                0,
                NULL,
                iface,
                &ps_errno);
      }
      break;
  
    case ICMP_INFO_RQST:
      /*-------------------------------------------------------------------
        Change type to INFO_REPLY, recompute checksum, and return datagram.
        Note the source/dest addresses will be swapped by IP processing.
      -------------------------------------------------------------------*/
      MSG_MED( "ICMP Info Request Rx'd", 0, 0, 0);
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif
      icmp.info.type = ICMP_INFO_REPLY;
      if ( icmpi_hton( &icmp, &item_ptr) == -1)
      {
        MSG_HIGH("icmpi_hton() conversion err, discard", 0, 0, 0); 
        dsm_free_packet( &item_ptr);
        return;
      }
      if (item_ptr != NULL)
      {
#if defined (FEATURE_DSM_MEM_CHK) && !defined (FEATURE_DATA_PS)
        item_ptr->tracer = DSM_IP_SND_PROC;
#endif //FEATURE_DSM_MEM_CHK
        ip_send(ip->dest,
                ip->source,
                ICMP_PTCL,
                ip->tos,
                0,
                item_ptr,
                length,
                0,
                0,
                NULL,
                iface,
                &ps_errno);
      }
      break;
  
  default:
#ifdef FEATURE_DS_SOCKETS
      /*---------------------------------------------------------------------
        If the ICMP packet we are dealing with is not one of the types 
        defined above, handle it sperately within the PS context. 
      ---------------------------------------------------------------------*/
      if (dssicmp_ps_input( &icmp, item_ptr, &(ip->source) ) == DSS_ERROR) 
#endif /* FEATURE_DS_SOCKETS */
      {
        MSG_HIGH( "Unsupported ICMP message %d, Discard", type, 0, 0);
        dsm_free_packet( &item_ptr);
        return;
      }
  } /* switch */
} /* icmp_input() */

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */


#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

/*===========================================================================

FUNCTION ICMP_OUTPUT()

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

===========================================================================*/
extern void icmp_output
(
  ip4a               *dest_ip_ptr,          /* destination of the delivery */
  struct icmp        *icmp_hdr_ptr,                  /* ptr to ICMP header */
  struct dsm_item_s  *item_ptr,                       /* item to be output */
  struct icmp_cb     *icmp_cb_ptr,                  /* Ptr to ICMP ctl blk */
  uint16             length             /* Optional length of data portion */
)
{
  int16              ps_errno;
  ip_addr_type       ip_addr;
#ifdef FEATURE_DS_SOCKETS
  uint8              ttl = 0;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Call to icmpi_hton() does the following:
      1. Allocate ICMP hdr
      2. Fill in ICMP hdr in network byte order
      3. Compute checksum
  -------------------------------------------------------------------------*/
  MSG_MED(" ICMP_OUTPUT: calling icmpi_hton() ", 0, 0, 0);
  if ( icmpi_hton( icmp_hdr_ptr, &item_ptr) == -1)
  {
    MSG_HIGH("icmpi_hton() conversion err, discard", 0, 0, 0); 
    dsm_free_packet( &item_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Send the ICMP packet down IP. Use the non-default IP_TTL value if
    available. 
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DS_SOCKETS
  if ((icmp_cb_ptr == NULL) || (icmp_cb_ptr->scb_ptr == NULL))
  {
    MSG_ERROR("icmp control block is NULL!", 0, 0, 0);
    dsm_free_packet( &item_ptr);
    return;
  }

  if(icmp_cb_ptr->scb_ptr->sockfd == SOCKAVAIL)
  {
    MSG_ERROR("icmp sockfd is invalid!", 0, 0, 0);
    dsm_free_packet( &item_ptr);
    return;
  }

  ttl = icmp_cb_ptr->scb_ptr->ip_ttl;
  MSG_MED(" ICMP_OUTPUT: calling ip_send(ttl = %d)", ttl, 0, 0);
#else
  MSG_MED(" ICMP_OUTPUT: calling ip_send", 0, 0, 0);
#endif

#if defined (FEATURE_DSM_MEM_CHK) && !defined (FEATURE_DATA_PS)
  item_ptr->tracer = DSM_IP_SND_PROC;
#endif //FEATURE_DSM_MEM_CHK

  /*-----------------------------------------------------------------------
    Retrieve the new IP addr from psiface.
  -----------------------------------------------------------------------*/
  ip_addr.type = IPV4_ADDR;
  ps_iface_get_addr(icmp_cb_ptr->scb_ptr->routing_cache, &ip_addr);

  (void) ip_send(ip_addr.addr.v4,
                 *dest_ip_ptr,
                 ICMP_PTCL,
                 0,
#ifdef FEATURE_DS_SOCKETS
                 ttl,
#else
                 0,
#endif
                 item_ptr,
                 length,
                 0,
                 0,
                 NULL,
                 icmp_cb_ptr->scb_ptr->routing_cache,
                 &ps_errno);
} /* icmp_output () */

#endif /* FEATURE_DS || FEATURE_DATA_PS */
