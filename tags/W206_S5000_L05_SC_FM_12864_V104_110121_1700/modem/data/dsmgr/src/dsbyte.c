/* Miscellaneous machine independent utilities
 * Copyright 1991 Phil Karn, KA9Q
 Copyright (c) 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
 Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
 */


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsbyte.c_v   1.0   08 Aug 2002 11:19:58   akhare  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/src/dsbyte.c#1 $ $DateTime: 2007/11/05 02:12:42 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
05/24/00    hcg    Added TARGET_OS_SOLARIS to compile with Solaris DSPE.
04/21/00    mvl    Fixed a #define so compiles properly under COMET
01/09/99    jjw    Changed to generic Browser interface
10/27/98    ldg    For T_ARM included C version of TCP checksum routine.
06/16/98    na     Converted the routine that calculates the TCP checksum 
                   into 186 assembly.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
07/22/95    jjw    Created Initial Version
===========================================================================*/


 
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


/*===========================================================================

FUNCTION get16

DESCRIPTION
  This function return the word value from the 2 byte value 

DEPENDENCIES
  None

RETURN VALUE
  A word representing the 2 bytes pointed at by the passed parameter

SIDE EFFECTS
  None
===========================================================================*/

word get16
(
  byte *cp               /* pointer to byte string to get the 16 bits from */
)
{
  register word x;       /*  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  x = *cp++;
  x <<= 8;
  x |= *cp;
  return( x);
} /* get16 */

/*===========================================================================

FUNCTION get32

DESCRIPTION
  This function 
  Machine-independent, alignment insensitive network-to-host long conversion

DEPENDENCIES
  None

RETURN VALUE
  32 bit value representing the 4 network bytes in the passed char string.

SIDE EFFECTS
  None
===========================================================================*/

dword get32
(
  byte *cp               /*  */
)
{
  dword rval;            /*  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rval = *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp;

  return( rval);
} /* get32 */

/*===========================================================================

FUNCTION put16

DESCRIPTION
  This function 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

byte *put16
(
  byte *cp,      /*  */
  word x         /*  */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  *cp++ = x >> 8;
  *cp++ = (x & 0xFF);

  return( cp);
} /* put16 */

/*===========================================================================

FUNCTION put32

DESCRIPTION
  This function 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

byte *put32
( 
  byte *cp,             /*  */
  dword x               /*  */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  *cp++ = x >> 24;
  *cp++ = (x >> 16) & 0xFF;
  *cp++ = (x >> 8) & 0xFF;
  *cp++ = x & 0xFF;
  return( cp);
} /* put32 */
