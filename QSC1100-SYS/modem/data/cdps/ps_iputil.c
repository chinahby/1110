/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    I P   I N T E R N A L   U T I L I T Y

                             F U N C T I O N S


GENERAL DESCRIPTION
  IP header conversion routines
  Copyright 1991 Phil Karn, KA9Q
 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iputil.c_v   1.2   31 Jan 2003 18:16:46   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_iputil.c#2 $ $DateTime: 2008/09/01 00:45:48 $ $Author: praveenp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/08   pp      Deprecated word from assembly block is re-worded w/ _dots_.
01/31/03   usb     Added an extra arg to ntohip() to support hdr copying only
09/10/02   aku     Removed global Ip_addr.
08/05/02   usb     Moved lcsum() from ps_utils to this file.
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module.
07/23/01   pjb     Save the reserved bit of the TCP header. 
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth 
                   parameter.Made changes to handle dsm_pushdown() 
                   returning FALSE.                  
12/14/98   ldg     Removed anforward-slash asterisk from inside a comment.
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
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#if defined (FEATURE_DS) || defined (FEATURE_DATA_PS)

#include "psglobal.h"
#include "dsbyte.h"
#include "dsm.h"
#include "memory.h"
#include "internet.h"
#include "ps_iface.h"
#include "ps_tcp.h"

#include "ps_ip.h"
#include "ps_iputil.h"
#include "ps_iphdr.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/


/*===========================================================================

FUNCTION HTONIP

DESCRIPTION
  This function will convert IP header in host format to network
  dsm_item_type. If cflag != 0, take checksum from structure, otherwise
  compute it automatically.

 NOTE: This 1st parameter is passed as the pointer to a pointer 
       to dsm_item_type in order to re-establish the front of the datagram.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void htonip
(
  struct dsm_item_s  **bpp,  /* address of ptr to datagram to ip_send func
                                NOTE: this pointer value may be changed    */

  register struct ip *ip,    /* pointer to IP header for datagram          */
  int                cflag   /* current checksum calculation               */
)
{
  uint16         hdr_len;     /* that would be header length   */
  register uint8 *cp;         /* pointer for loading up packet */
  uint16         fl_offs;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdr_len = IPLEN + ip->optlen;
  if(hdr_len > IPLEN + IP_MAXOPT)
  {
    hdr_len = IPLEN + IP_MAXOPT;
  }

  /*-------------------------------------------------------------------------
    Create sufficient room for header on front end of packet 
    then assign byte pointer and being loading of network-ordered
    IP header fields.
  -------------------------------------------------------------------------*/
  if (dsm_pushdown( bpp, NULL, hdr_len, DSM_DS_SMALL_ITEM_POOL) < hdr_len)
  {
    dsm_free_packet(bpp);
    return;
  }
  cp = (*bpp)->data_ptr;

  *cp++ = (ip->version << 4) | (hdr_len >> 2);
  *cp++ = ip->tos;

  cp = put16( cp, ip->length);
  cp = put16( cp, ip->id);

  fl_offs = ip->offset >> 3;
  if( ip->flags.congest)
  {
    fl_offs |= 0x8000;
  }
  if( ip->flags.df)
  {
    fl_offs |= 0x4000;
  }
  if( ip->flags.mf)
  {
    fl_offs |= 0x2000;
  }
  cp    = put16( cp, fl_offs);

  *cp++ = ip->ttl;
  *cp++ = ip->protocol;

  if( cflag)
  {
    /* Use checksum from host structure */
    cp = put16(cp,ip->checksum);
  } 
  else 
  {
    /* Clear checksum for later recalculation */
    *cp++ = 0;
    *cp++ = 0;
  }
  cp = put32(cp,ip->source);
  cp = put32(cp,ip->dest);

  if(ip->optlen != 0)
  {
    memcpy(cp,ip->options,MIN(ip->optlen,IP_MAXOPT));
  }

  /* If requested, recompute checksum and insert into header */
  if(!cflag)
  {
    put16( (*bpp)->data_ptr + 10, cksum(NULL,*bpp,hdr_len) );
  }
}

/*===========================================================================

FUNCTION NTOHIP

DESCRIPTION
  This function will extract an IP header from the passed item. If copy_only 
  flag is set the ip header is only copied and not pulled out from passed in 
  item, otherwise the contents are actually pulled out. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

/* Extract an IP header from mbuf */

int ntohip
(
  register struct ip *ip,
  struct dsm_item_s  **bpp,
  boolean            copy_only
)
{
  int ihl;
  uint16 fl_offs;
  uint8 ipbuf[IPLEN];
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (copy_only)
  {
    if(dsm_extract(*bpp,0,ipbuf,IPLEN) != IPLEN)
    {
      return -1;
    }
  }
  else
  {
    if(dsm_pullup(bpp,ipbuf,IPLEN) != IPLEN)
    {
      return -1;
    }
  }

  ip->version       = (ipbuf[0] >> 4) & 0xf;
  ip->tos           = ipbuf[1];
  ip->length        = get16( &ipbuf[2]);
  ip->id            = get16( &ipbuf[4]);
  fl_offs           = get16( &ipbuf[6]);
  ip->offset        = (fl_offs & 0x1fff) << 3;
  ip->flags.mf      = (fl_offs & 0x2000) ? 1 : 0;
  ip->flags.df      = (fl_offs & 0x4000) ? 1 : 0;
  ip->flags.congest = (fl_offs & 0x8000) ? 1 : 0;
  ip->ttl           = ipbuf[8];
  ip->protocol      = ipbuf[9];
  ip->checksum      = get16( &ipbuf[10]);
  ip->source        = get32( &ipbuf[12]);
  ip->dest          = get32( &ipbuf[16]);

  ihl = (ipbuf[0] & 0xf) << 2;
  if(ihl < IPLEN)
  {
    /* Bogus packet; header is too short */
    ip->optlen = 0;
    return -1;
  }
  if ( (ip->optlen = ihl - IPLEN) != 0 ) 
  {
    if (copy_only)
    {
      if(dsm_extract(*bpp,IPLEN,ip->options,ip->optlen) != ip->optlen)
      {
        return -1;
      }
    }    
    else
    {
      if ( dsm_pullup(bpp,ip->options,ip->optlen) < ip->optlen )
      {
        return -1;
      }
    }
  }
  return ihl;
}

/*===========================================================================

FUNCTION EAC

DESCRIPTION
  This function will perform end-around-carry adjustment

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

uint16 eac
( 
  register uint32 sum     /* Carries in high order 16 bits */
)
{
  register uint16 csum;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while((csum = sum >> 16) != 0)
  {
    sum = csum + (sum & 0xffffL);
  }
  return (uint16) (sum & 0xffffL);        /* Chops to 16 bits */
}

/*===========================================================================

FUNCTION CHKSUM

DESCRIPTION
  This function will checksum the passed memory item.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

uint16 cksum
(
  pseudo_header_type   *ph,
  register struct      dsm_item_s *m,
  uint16               len 
)
{
  register uint32 sum = 0L;
  register uint32 csum;
  register uint16 cnt, total;
  register uint8  *up;
  uint16          csum1;
  int             swap = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sum pseudo-header, if present */
  if(ph != NULL)
  {
    sum  = hiword(ph->source);
    sum += loword(ph->source);
    sum += hiword(ph->dest);
    sum += loword(ph->dest);
    sum += ph->protocol;
    sum += ph->length;
  }
  /* Now do each mbuf on the chain */
  for(total = 0; m != NULL && total < len; m = m->pkt_ptr) 
  {
    cnt  = MIN(m->used, len - total);
    up   = m->data_ptr;
    csum = 0L;

    /* pointer dependent !!! */
    if((long)up & 1)
    {
      /* Handle odd leading byte */
      if(swap)
      {
        csum = *up++;
      }
      else
      {
        csum = (uint16)*up++ << 8;
      }
      cnt--;
      swap = !swap;
    }
    if(cnt > 1)
    {
      /* Have the primitive checksumming routine do most of
       * the work. At this point, up is guaranteed to be on
       * a short boundary
       */
      csum1 = lcsum((uint16 *)up, cnt >> 1);
      if(swap)
      {
        csum1 = (csum1 << 8) | (csum1 >> 8);
      }
      csum += csum1;
    }
    /* Handle odd trailing byte */
    if(cnt & 1)
    {
      if(swap)
      {
        csum += up[--cnt];
      }
      else
      {
        csum += (uint16)up[--cnt] << 8;
      }
      swap = !swap;
    }
    sum   += csum;
    total += m->used;
  }
  /* Do final end-around carry, complement and return */
  return (uint16)(~eac(sum) & 0xffff);
}


/*===========================================================================

FUNCTION  lcsum

DESCRIPTION
  This function computes the 1's-complement sum of the given data buffer.
  This uses a inline assembly routine for the 186 processor which reduces
  CPU utilization greatly. 

  The inline assembly uses an unwound loop method to reduce iterations 
  of loop.

  buf: ptr to the array of bytes that need to be checksummed.
  cnt: number of bytes that need to be checksummed.

DEPENDENCIES
  None

RETURN VALUE
  The ones complement sum of the bytes in the buffer.

SIDE EFFECTS
  None
===========================================================================*/
uint16 lcsum
(
  uint16 *buf,
  uint16 cnt
)
{
#if defined(T_ARM) || defined(T_WINNT) || defined (TARGET_OS_SOLARIS)
  uint32 sum = 0;
  uint16 result;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while(cnt-- != 0)
    sum += *buf++;
  result = eac(sum);

  /* Swap the result because of the (char *) to (int *) type punning */

  result = (result << 8) | (result >> 8);

  return result;
#else
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

_asm{
      push ds

        lds     si,buf          ; ds:si = buf

        mov     cx,cnt          ; cx = cnt
        cld                     ; autoincrement si
        mov     ax,cx
        shr     cx,1            ; cx /= 16, number of loop iterations
        shr     cx,1
        shr     cx,1
        shr     cx,1

        inc     cx              ; make fencepost adjustment for 1st pass
        and     ax,15           ; ax = number of words modulo 16
        lea     bx,l0           ; Load bx with last address
        sub     bx,ax           ; Now index into instr. being used
        shl     ax,1            ; *=2; each set of instr. is 3 bytes
        sub     bx,ax           ; index into jump table
        clc                     ; initialize carry = 0
        mov     dx,0            ; clear accumulated sum
        jmp     bx              ; jump into loop

; Note, since assumption is made about the number of bytes in each set
; of loadsw and adc ( 3 bytes), the order of instructions inside the 
; loop must NOT be changed
;
; Here the real work _g.e.t.s_ done. The numeric labels on the lodsw instructions
; are the targets for the indirect jump we just made.
;
; Each label corresponds to a possible remainder of (count / 16), while
; the number of times around the loop is determined by the quotient.
;
; The loop iteration count in cx has been incremented by one to adjust for
; the first pass.
; 
deloop: lodsw
        adc     dx,ax
l15:    lodsw
        adc     dx,ax
l14:    lodsw
        adc     dx,ax
l13:    lodsw
        adc     dx,ax
l12:    lodsw
        adc     dx,ax
l11:    lodsw
        adc     dx,ax
l10:    lodsw
        adc     dx,ax
l9:     lodsw
        adc     dx,ax
l8:     lodsw
        adc     dx,ax
l7:     lodsw
        adc     dx,ax
l6:     lodsw
        adc     dx,ax
l5:     lodsw
        adc     dx,ax
l4:     lodsw
        adc     dx,ax
l3:     lodsw
        adc     dx,ax
l2:     lodsw
        adc     dx,ax
l1:     lodsw
        adc     dx,ax
l0:     loop    deloop          ; :-)

        adc     dx,0            ; get last carries
        adc     dx,0
        mov     ax,dx           ; result into ax
        xchg    al,ah           ; byte swap result (8088 is little-endian)

        ; return value is in ax.

 pop ds 
} /* end _asm */
#endif /* not T_ARM */

} /* lcsum() */

#endif /* FEATURE_DS || FEATURE_DATA_PS */
