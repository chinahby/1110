/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   D I S T A N C E - B A S E D   R E G I S T R A T I O N   S E R V I C E S

GENERAL DESCRIPTION
  Library of routines to calculate the distance the mobile has moved from
  where it previously registered and whethere it should re-register.

REFERENCES
  IS-95 section 6.6.5.1.4 Distance based Registration

EXTERNALIZED FUNCTIONS
  dist_reg
    Called to check whether distance based registration is necessary.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  If the mobile has never registered and doesn't contain a permanent
  record of the previous lat and long distance based registration
  should not be called as it has no meaning.

GENERAL
  For lattitude measurements 90  deg = 0x13C680, -90  deg = 0x2C3980
  For longitude measurements 180 deg = 0x278D00, -180 deg = 0x587300
  There is thus headroom to multiply these numbers by 0x100 in a long int.
  Delta lat and delta long are limited to 72.8 deg as this value, when
  shifted down by 4 and squared is the limit of a dword.

Copyright (c) 1993-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/distreg.c_v   1.1   03 Oct 2002 10:51:06   phosabet  $
  $Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/distreg.c#2 $ $DateTime: 2009/11/03 05:45:40 $ $Author: sshahi $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/09   ssh     Fixed error in the distance calculation logic
04/26/06   an      Fixed Lint Errors
10/03/02   ph      Lint Cleanup
08/16/99   jq      Commented out include msg.h line to clear a compiler warning
02/29/93   gb      Created Module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "distreg.h"
// #include "msg.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* General Constants */

#define PI      0x324        /* pi radians in format xx.xx */
#define D180    0xB400       /* 180 degrees in format xx.xx */
#define LONG180 0x278D00     /* 180 degrees in 23 bit format */
#define LONG360 0x4F1A00     /* 360 degrees in 23 bit format */


/* Cosine Constants as binary word fractions in format xx.xx */

#define A   0x2AAA
#define B   0x222
#define C   0xC

/* Cosine Constants array */

int cos_coeff[3] = { A, -B, C };


word cosine
(
  int value                 /* angle in radians in hex format XX.XX ) */
);

word square_root
(
  long int value            /* 32 bit signed integer value */
);

/*===========================================================================

FUNCTION REG_DIST

DESCRIPTION
  This function calculates the distance the mobile has moved since it last
  registered and checks whether distance-based registration is required.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if distance based registration is required,  FALSE if not.

SIDE EFFECTS
  None.
===========================================================================*/
//!!! move back into procedure
  long delta_lat;              /* change in base station lattitude */
  long delta_long;             /* change in base station longitude */
  word dist;                   /* distance moved */

boolean reg_dist
(
  long reg_lat1,               /* previous base station lattitude */
  long reg_lat2,               /* current base station lattitude */
  long reg_long1,              /* previous base station longitude */
  long reg_long2,              /* current base station longitude */
  word dist_reg                /* distance to trigger registration */
)
{
  /*lint -e737 */
  /* sign extend all the 22 and 23 bit coordinates to full 32 bit integers */
  reg_lat1 = EXTEND22 ( reg_lat1 );
  reg_long1 = EXTEND23 ( reg_long1 );
  reg_lat2 = EXTEND22 ( reg_lat2 );
  reg_long2 = EXTEND23 ( reg_long2 );
  /*lint +e737 */

//  MSG_ERROR ( "lat1 %ld, long1 %ld", reg_lat1, reg_long1, 0 );
//  MSG_ERROR ( "lat2 %ld, long2 %ld", reg_lat2, reg_long2, 0 );
//  MSG_ERROR ( "dist_reg %d", dist_reg, 0, 0 );

  /* calculate change in lattitude */
  delta_lat = reg_lat2 - reg_lat1;
  delta_lat = ( delta_lat < 0 ) ? - delta_lat : delta_lat;

  /* calculate change in longitude with lattitude factor */
  delta_long = ( reg_long2 - reg_long1 ) * 
      ( cosine( (int) ((((reg_lat1 * 256 ) / 14400 ) * PI ) / D180 ) ) / 256 );

  /* compensate for the guy who drives across the dateline */
  delta_long = ( delta_long < 0 ) ? - delta_long : delta_long;
  delta_long = ( delta_long > LONG180 ) ? LONG360 - delta_long : delta_long;

  /* take the square root, round up and divide by 16 */
  if (( delta_lat > 0x10000 ) || ( delta_long > 0x10000 ))
  {
    dist = square_root (
       ( delta_lat >> 4 ) * ( delta_lat >> 4 ) + //lint !e704 >> of signed. It can't be -ve, so OK.
       ( delta_long >> 4 ) * ( delta_long >> 4 ) ); //lint !e704 >> of signed. It can't be -ve, so OK.
  }
  else               /* use higher precision */
  {
    dist = ( square_root ( ( delta_lat * delta_lat ) +
                        ( delta_long * delta_long ) ) + 0x8 ) >> 4;        
  }

//  MSG_HIGH ( "Delta lat %lx, long %lx ", delta_lat, delta_long, 0);
//  MSG_HIGH ( "RegDist %x, distance %x ", dist_reg, dist, 0);

  if ( dist_reg < dist )
  {
    return ( TRUE );
  }
  else
  {
    return ( FALSE );
  }
}  /* reg_dist */

/* <EJECT> */
/*===========================================================================

FUNCTION SQUARE_ROOT

DESCRIPTION
  This function calculates the square root of a long integer.
  The algorithm used is the Newton-Raphson iteration derived from
  the Taylor series expansion of the function
  g( x ) 
  which is
  g( x+h ) = g( x ) + hg'( x ) + r( x, h ) where r( x, h ) is the remainder
  thus, leaving off the remainder we get, in incremental values of x
  g( x[i+1] ) = g( x[i] ) + ( x[i+1] - x[i] ) g'( x[i] )
  solving for x[i+1] with g( x[i+1] ) = 0 yields the approximation
  x[i+1] = x[i] - g( x[i] ) / g'( x[i] )

  which, in our case when g( x ) = x**2 - v = 0, solved when x = v**1/2, is

  x[i+1] = x[i] - (x[i]**2 - v ) / 2x[i] 
         = x[i] - 1/2 ( x[i] - v / x[i]
         = 1/2 ( x[i] + v / x[i] )

  which is the almost intuitive result that the next closest approximation
  is the average of the previous approximation and the square divided by the
  previous approximation.

  The number of iterations needed and indeed convergence depends on the
  choice of starting value x[0]. It can be shown that a number of the order
  of x**n has a square root of the order of x**n/2. In the binary case this
  translates to finding the highest set bit position p and using 1 shifted
  by p/2 to use as the starting value.

  In general the Newton-Raphson iteration will double precision per iteration,
  given a  suitable close starting value and sufficiently accurate arithmetic.
  Thus if x[0] approximates g( x ) with only 1 bit of precision, 4 iterations
  will give you about 16 bits of precision which is all that is needed for
  the square root of a 32 bit number. We use a self terminating precision
  measure i.e. if no more precision is gained stop iteration. By experiment
  this method uses less loops than a fixed 4 iteration method.

DEPENDENCIES
  None.

RETURN VALUE
  The word square root of the long integer input.

SIDE EFFECTS
  None.
===========================================================================*/

word square_root
(
  long int value            /* 32 bit signed integer value */
)
{
  long int aprx = 0x100;
  int i;

  /* Find the 1/2 point of the highest bit set in value as the init aprox */

  for (i=4;i>0;i/=2)
  {
    aprx = (value < (aprx * aprx)) ? aprx >> i : aprx << i; //lint !e704 !e703 >> and << of signed. being used only as +ve
  }

  /* classic square root iteration with 16 bit precision */
  for ( i=0;i<4 && aprx!=0; i++ )
  {
    aprx = ( value / aprx + aprx ) / 2;   
  }
  return ( ( word ) aprx );

} /* square_root */


/* <EJECT> */
/*===========================================================================

FUNCTION COSINE

DESCRIPTION
  This function calculates the cosine of a word with a binary decimal point
  between bytes in the range 0 - pi / 2 radians.
  The algorithm used is a Polynomial Approximation of the form
               n
  P( n, x ) =  S ( a[i] x**i )
              i=0

  The Power series used is based on the general form series for sin( x )/x
  which is
                  3
  Sin( x ) / x =  S ( a[2i] x**2i ) + xe( x )
                 i=0
  
  where |x| <= Pi/2  and the error |xe( x )| <= 2e-5

  substituting (pi/2 - x) for x and multiplying through by x we get

    COS ( x ) = z + az3 + bz5 + cz7
    where z = pi/2 - x
    a = - 0.1666665668
    b = 0.008333025139
    c = - 0.000198074182

  The coefficients are constants defined in the Hex form xx.xx i.e. scaled
  by 256.

DEPENDENCIES
  None.

RETURN VALUE
  A word which is a scaled by 256 factor of the cosine of the input.

SIDE EFFECTS
  None.
===========================================================================*/

word cosine
(
  int value  /* angle in radians in hex format XX.XX ) */
)
{

  int cos, z, i;
  long temp;

  value = ( value < 0 ) ? - value : value;
  temp = cos = z = PI / 2 - value ;   

  for (i=0;i<3;i++)
  {
    temp = ( temp * z * z ) >> 16; //lint !e704 >> of signed. As per header, value should
                                   // be in the range of 0 - pi/2 radians. If that's true,
                                   // the signed quantity here could not be -ve.
    cos -= ( word )(( cos_coeff[i] * temp ) >> 16 ); //lint !e704 >> of signed. As per header, value should
                                                     // be in the range of 0 - pi/2 radians. If that's true,
                                                     // the signed quantity here could not be -ve.
  } 
  return ( cos ); //lint !e734 loss of precision
} /* cosine */
