/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              T C P H D R

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1995-2008 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcphdr.c_v   1.0   08 Aug 2002 11:19:52   akhare  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_tcphdr.c#2 $ $DateTime: 2008/04/03 08:46:24 $ $Author: praveenp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/08    pp     Klocwork fixes.
09/11/03   sv      Modified htontcp to return if dsm_pushdown fails.
07/31/02   usb     Added include ps_iputil.h
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module.
08/14/01   pjb     Fixed dsm packet size overflow problem
07/23/01   pjb     Changed call to dsm_offset_new_buffer to reserve space 
                   for header.  Save the reserved bit from the TCP header.  
08/25/00   rc      Changed dsm_pushdown() to take take pool id as fourth 
                   parameter.Made changes to handle dsm_pushdown() 
                   returning FALSE.                                    
01/27/98   ldg     Removed the DS_SLIM switch.
06/25/97   jgr     Added ifdef FEATURE_DS over whole file
11/14/96   jjw     Changes to support Unwired Planet (UDP operation)
05/24/96   jjw     Gemini & ISS2 Data Services release.
11/13/95   jjw     Changed min/max macros to upper case
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* TCP header conversion routines
 * Copyright 1991 Phil Karn, KA9Q
 * Additional material Copyright 1992 William Allen Simpson
 */
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#if defined (FEATURE_DS) || defined(FEATURE_DATA_PS)
#include "psglobal.h"
#include "dsbyte.h"
#include "dsm.h"
#include "ps_iface.h"
#include "ps_ip.h"
#include "ps_iputil.h"
#include "ps_tcp.h"
#include "internet.h"
#include "memory.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/


/*===========================================================================

FUNCTION HTONTCP

DESCRIPTION
  This function will Convert TCP header in host format into mbuf ready for
  transmission, link in data (if any). If ph != NULL, compute checksum,
  otherwise take checksum from tcph->checksum

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void htontcp
(
  struct   dsm_item_s    **bpp,
  register struct tcp    *tcph,
  pseudo_header_type *ph 
)
{
  uint16 hdrlen;
  register uint8 *cp;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdrlen = TCPLEN;
  if(tcph->optlen > 0 && tcph->optlen <= TCP_MAXOPT)
  {
    hdrlen += tcph->optlen;
  } 
  else if(tcph->mss != 0)
  {
    hdrlen += MSS_LENGTH;
  }
  if (*bpp == NULL && 
      (*bpp = dsm_offset_new_buffer(DSM_DS_LARGE_ITEM_POOL,
				    PS_MAX_HDR)) == NULL)
  {
    return;
  }
  if (dsm_pushdown(bpp,NULL,hdrlen,DSM_DS_SMALL_ITEM_POOL) != hdrlen) 
  {
    dsm_free_packet(bpp);
    return;
  }

  cp = (*bpp)->data_ptr;
  cp = put16(cp,tcph->source);
  cp = put16(cp,tcph->dest);
  cp = put32(cp,tcph->seq);
  cp = put32(cp,tcph->ack);
  *cp++ = hdrlen << 2 | ((tcph->flags.reserved >> 1) & 0x0f); /* Offset field */
  *cp = (tcph->flags.reserved & 0x01) ? 0x80 : 0;
  if(tcph->flags.congest)
  {
    *cp |= 64;
  }
  if(tcph->flags.urg)
  {
    *cp |= 32;
  }
  if(tcph->flags.ack)
  {
    *cp |= 16;
  }
  if(tcph->flags.psh)
  {
    *cp |= 8;
  }
  if(tcph->flags.rst)
  {
    *cp |= 4;
  }
  if(tcph->flags.syn)
  {
    *cp |= 2;
  }
  if(tcph->flags.fin)
  {
    *cp |= 1;
  }
  cp++;
  cp = put16(cp,tcph->wnd);
  if(ph == NULL)
  {
    /* Use user-supplied checksum */
    cp = put16(cp,tcph->checksum);
  } 
  else 
  {
    /* Zero out checksum field for later recalculation */
    *cp++ = 0;
    *cp++ = 0;
  }
  cp = put16(cp,tcph->up);

  /* Write options, if any */
  if(hdrlen > TCPLEN)
  {
    if(tcph->optlen > 0 && tcph->optlen <= TCP_MAXOPT)
    {
      (void)memcpy(cp,tcph->options,tcph->optlen);
    } 
    else if(tcph->mss != 0)
    {
      *cp++ = MSS_KIND;
      *cp++ = MSS_LENGTH;
      cp = put16(cp,tcph->mss);
    }
  }
  /* Recompute checksum, if requested */
  if(ph != NULL)
  {
    put16( (*bpp)->data_ptr + 16, cksum(ph,*bpp,ph->length) );
  }
}

/*===========================================================================

FUNCTION NTOHTCP

DESCRIPTION
  This function will Pull TCP header off mbuf 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

int ntohtcp
(
  register struct tcp *tcph,
  struct   dsm_item_s **bpp 
)
{
  int hdrlen,i;
  int kind,kindlen;
  register int flags;
  uint8 hdrbuf[TCPLEN],*cp;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  i = dsm_pullup(bpp,hdrbuf,TCPLEN);

  /* Note that the results will be garbage if the header is too short.
   * We don't check for this because returned ICMP messages will be
   * truncated, and we at least want to get the port numbers.
   */
  tcph->source        = get16( &hdrbuf[0]);
  tcph->dest          = get16( &hdrbuf[2]);
  tcph->seq           = get32( &hdrbuf[4]);
  tcph->ack           = get32( &hdrbuf[8]);
  flags               = hdrbuf[13];
  tcph->flags.reserved = ((hdrbuf[12] << 1) & 0x1e) | ((flags & 0x80) ? 0x1 : 0);
  tcph->flags.congest = (flags & 64) ? 1 : 0;
  tcph->flags.urg     = (flags & 32) ? 1 : 0;
  tcph->flags.ack     = (flags & 16) ? 1 : 0;
  tcph->flags.psh     = (flags & 8) ? 1 : 0;
  tcph->flags.rst     = (flags & 4) ? 1 : 0;
  tcph->flags.syn     = (flags & 2) ? 1 : 0;
  tcph->flags.fin     = (flags & 1) ? 1 : 0;
  tcph->wnd           = get16( &hdrbuf[14]);
  tcph->checksum      = get16( &hdrbuf[16]);
  tcph->up            = get16( &hdrbuf[18]);
  tcph->mss           = 0;

  /* Check for option field.
   */
  hdrlen = (hdrbuf[12] & 0xf0) >> 2;
  if(i < TCPLEN || hdrlen < TCPLEN) 
  {
    tcph->optlen = 0;
    return -1;  /* Header smaller than legal minimum */
  }

  if ( (tcph->optlen = hdrlen - TCPLEN) == 0 )
  {
    return hdrlen;  /* No options, all done */
  }

  if ( dsm_pullup(bpp,tcph->options,tcph->optlen) < tcph->optlen ) 
  {
    /* Remainder too short for options length specified */
    return -1;
  }

  /* Process options */
  for( cp=tcph->options, i=tcph->optlen; i>0;)
  {
    kind = *cp++;
    /* Process single-byte options */

    switch( kind)
    {
    case EOL_KIND:
      i--;
      cp++;
      return hdrlen;  /* End of options list */

    case NOOP_KIND:
      i--;
      cp++;
      continue;  /* Go look for next option */
    }
    /* All other options have a length field */
    kindlen = *cp++;

    /* Process valid multi-byte options */
    switch( kind)
    {
    case MSS_KIND:
      if(kindlen == MSS_LENGTH)
      {
        tcph->mss = get16(cp);
      }
      break;
    }
    kindlen  = MAX(2,kindlen); /* Enforce legal minimum */
    i       -= kindlen;
    cp      += kindlen - 2;
  }
  return hdrlen;
}
#endif /* FEATURE_DS || FEATURE_DATA_PS */
