/*===========================================================================

                  F I X E D   P O I N T   S E R V I C E S
                           H E A D E R   F I L E

DESCRIPTION
  This file contains delarations necessary to use the fixed point services.

Copyright (c) 1991,1992,1993 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1994-2000 by QUALCOMM, Incorporated.  All Rights Reserved.
=============================================================================


=============================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/sleepctl/main/latest/src/fixed.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/26/01   ajn     SRCH/SLEEP VU tip remerge - first check in
10/31/00   ajn     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "fixed.h"

#include "msg.h"


/*===========================================================================

FUNCTION FIXED_GET_NUM_BITS

DESCRIPTION
   Determines the number of significant bits in the argument.

DEPENDENCIES
   None

RETURN VALUE
   Number of significant bits

SIDE EFFECTS
   None

===========================================================================*/

LOCAL int fixed_get_num_bits( uint32 x )
{
  int num_bits = 0;

  while ( x > 0 )
  {
    x >>= 1;
    num_bits++;
  }

  return num_bits;

} /* get_num_bits */


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
)
{
  uint32    quotient = 0;
    /* Result of the division */

  uint16    shift;
    /* Number of bits the quotient and dividend may be shifted up after each
       iteration */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( divisor == 0 )
  {
    MSG_ERROR("Division by zero: %ld / %ld * 2^-%d",
		dividend, divisor, binary_point);

	*remainder = 0;

	return 0;
  }


  /* Divisor must be at most 31 bits. */

  if ( divisor >= 0x80000000ul )
  {
    MSG_ERROR("Divisor too large: %ld / %ld * 2^-%d",
		dividend, divisor, binary_point);

	*remainder = 0;

	return 0;
  }


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  shift = 32 - fixed_get_num_bits( divisor );
    /* After dividing by an "n" bit number, the remainder must have no more
       than "n" bits.  This means the remainder will have at least "32-n"
       leading zeros, so we can shift by at least this amount. */

  /*
     Repeat the following reduction until the quotient and dividend have been
     shifted up to eliminate the binary_point:

     Ie)  A / (B * 2^-n) == A * 2^n / (B * 2^0)
  */

  while ( binary_point > 0 )
  {

    if ( shift > binary_point )
    {
      shift = binary_point;
        /* For the last iteration, only shift as much as needed */
    }

    quotient += dividend / divisor;
    dividend  = dividend % divisor;
      /* Compute partial result */

    quotient <<= shift;
    dividend <<= shift;
      /* Slide the bits over */

    binary_point -= shift;
      /* And eliminate this amount of shifting */

  } /* while */

  /* Quotient now contains an unscaled value, and the dividend has been
     scaled up by 2^binary_point. */

  quotient += dividend / divisor;
  dividend  = dividend % divisor;
    /* Compute the final result */

  *remainder = dividend;
    /* Store the residual (scaled up by 2^binary_point) */

  return quotient;

} /* fixed_divide */


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
)
{
  uint32    quotient;
    /* Result of the division */

  uint32    remainder;
    /* Remainder after division */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  quotient = fixed_divide( dividend, divisor, binary_point, &remainder );
    /* Divide ... and get the remainder */

  /* if (remainder/divisor >= 0.5) round up */

  if ( remainder * 2 >= divisor )
  {
	  quotient++;
  }

  return quotient;
}

