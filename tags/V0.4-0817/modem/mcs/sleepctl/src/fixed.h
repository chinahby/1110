/*===========================================================================

                  F I X E D   P O I N T   S E R V I C E S
                           S O U R C E   F I L E

DESCRIPTION
  This file contains delarations necessary to use the fixed point services.

Copyright (c) 1991,1992,1993 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1994-1999 by QUALCOMM, Incorporated.  All Rights Reserved.
=============================================================================

=============================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/sleepctl/main/latest/src/fixed.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/26/01   ajn     SRCH/SLEEP VU tip remerge - first check in
11/02/00   ajn     Corrected "FIXED_ROUND" macro
10/31/00   ajn     Created file.

===========================================================================*/

#ifndef _FIXED_H
#define _FIXED_H

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                        MACRO DECLARATIONS

===========================================================================*/

#define FIXED_1p0(binary_point)	( 1 << (binary_point) )
  /* 1.0 in fixed point representation */

#define FIXED_0p5(binary_point)  ( 1 << ((binary_point)-1) )
  /* 0.5 in fixed point representation, for rounding */

#define FIXED_FLOOR(value, binary_point) ( (value) >> (binary_point) )
  /* Round a fixed point number down to next uint32 */

#define FIXED_ROUND(value, bp) FIXED_FLOOR( (value) + FIXED_0p5(bp), bp )
  /* Round a fixed point number down to nearest uint32 */

#define FIXED_FRAC(value, bp)  ( (value) & (FIXED_1p0(bp) - 1) )
  /* Fractional part of Fixed Point number */




/*===========================================================================

FUNCTION FIXED_DIVIDE

DESCRIPTION
   Divides an unsigned integer by an unsigned fixed-point divisor,
   resulting in an unsigned integer qoutient and remainder.

   *OR*

   Divides an unsigned integer by an unsigned integer divisor,
   resulting in a fixed-point quotient, and a remainder.

   The binary-point value represents the number of digits after the
   binary point.  For instance, binary_point=14, would imply 18.14 format.

DEPENDENCIES
   Most significant bit of divisor must be zero (31-bit value).

RETURN VALUE
   The quotient from the division.

SIDE EFFECTS
   The remainder reference is updated with the remainder after division.

===========================================================================*/

uint32 fixed_divide
(
  uint32    dividend,
    /* Value to be divided */

  uint32    divisor,
    /* Fixed-point value to divide into dividend. */

  uint16    binary_point,
    /* Number of bits after the "binary-point". */

  uint32 *  remainder
    /* [Out] Remainder of division */
);

/*===========================================================================

FUNCTION FIXED_DIVIDE_ROUNDED

DESCRIPTION
   Divides an unsigned integer by an unsigned fixed-point divisor,
   resulting in an unsigned integer qoutient.

   *OR*

   Divides an unsigned integer by an unsigned integer divisor,
   resulting in a fixed-point quotient.

   The binary-point value represents the number of digits after the
   binary point.  For instance, binary_point=14, would imply 18.14 format.

DEPENDENCIES
   Most significant bit of divisor must be zero (31-bit value).

RETURN VALUE
   The quotient from the division, rounded to nearest LSB.

SIDE EFFECTS
   None

===========================================================================*/

uint32 fixed_divide_rounded
(
  uint32    dividend,
    /* Value to be divided */

  uint32    divisor,
    /* Fixed-point value to divide into dividend. */

  uint16    binary_point
    /* Number of bits after the "binary-point". */
);


#endif /* _FIXED_H */


