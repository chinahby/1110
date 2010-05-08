#ifndef PS_IPUTIL_H
#define PS_IPUTIL_H
/*===========================================================================

                   I P   I N T E R N A L   U T I L I T Y

                           F U N C T I O N S

DESCRIPTION
  This file contains declarations for internal utility functions
  used by the IP module.

   Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iputil.h_v   1.1   31 Jan 2003 18:20:00   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_iputil.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/03   usb     Added an extra arg to ntohip() to support hdr copying only
07/25/02   om      Created module from ip.h

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/

#include "target.h"
#include "comdef.h"

#include "dsm.h"
#include "ps_tcp.h"

#include "ps_iphdr.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION CKSUM()

DESCRIPTION
  This function calculates the IP checksum.

DEPENDENCIES
  None

RETURN VALUE
  16 bit checksum  

SIDE EFFECTS
  None
===========================================================================*/
uint16 cksum
(
  pseudo_header_type  *ph,
  struct dsm_item_s    *m,
  uint16               len
);

/*===========================================================================

FUNCTION HTONIP()

DESCRIPTION
  This function packs a given header and prependes it to the
  given DSM chain.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void htonip
(
  struct dsm_item_s **bpp,
  struct ip         *ip,
  int               cflag
);

/*===========================================================================

FUNCTION NTOHIP()

DESCRIPTION
  This function will extract an IP header from the passed item. If copy_only 
  flag is set the ip header is only copied and not pulled out from passed in 
  item, otherwise the contents are actually pulled out. 

DEPENDENCIES
  None

RETURN VALUE
  ???

SIDE EFFECTS
  None
===========================================================================*/
int ntohip
(
  struct ip         *ip,
  struct dsm_item_s **bpp,
  boolean           copy_only
);

/*===========================================================================

FUNCTION  lcsum()

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
);

#endif  /* PS_IPUTIL_H */

