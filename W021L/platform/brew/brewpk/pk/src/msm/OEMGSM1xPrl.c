

#include "customer.h" /* Customer configuration file */

#ifdef FEATURE_SD20


/* Only support targets with SD 2.0 support */


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "prl.h"      /* Interface to PRL services */

#include "comdef.h"
#include "bit.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                               Macros
---------------------------------------------------------------------------*/

/* Macro to build parameters for use in b_unpack while unpacking the roaming
** list
*/
#define PARAMS(field) buf_ptr, FPOS(MTYPE, field), FSIZ(MTYPE, field)



#ifdef FEATURE_GSM1x

/*============================================================================

FUNCTION GSM1X_B_UNPACKW

DESCRIPTION
  Given a buffer and an offset, unpacks the requested number of bits into
  a word

DEPENDENCIES
  None
   
RETURN VALUE
  Unpacked item

SIDE EFFECTS
  None
   
============================================================================*/
static word gsm1x_b_unpackw(
   byte src[], 
   word pos, 
   word len
)
{ 
   word result = 0;
   int rshift = 0;

   src += pos/8;
   pos %= 8;

   rshift = MAX( 8 - (pos + len), 0);

   if ( rshift > 0 ) {

     result = MASK_AND_SHIFT(len, pos, rshift, *src);
  
   } else {

      result = MASK(8-pos, pos, *src);
      src++;
      len -= 8 - pos;

      for ( ; len >= 8  ; len-=8)
      {
         result = ( result<<8 ) | *src++;
      }

      if ( len > 0 ) result = ( result<<len ) | (*src >> (8-len));  // if any bits left
   }

   return result;
} /* END b_unpackw */


/*===========================================================================

FUNCTION gsm1x_prl_get_size

DESCRIPTION
  This function retuns the size of the PRL.

DEPENDENCIES
  None.

RETURN VALUE
  PRL size. If prl_ptr is NULL it will return 0.

SIDE EFFECTS
  None.

===========================================================================*/
word gsm1x_prl_get_size(

  byte *prl_ptr
       /* Pointer to the PRL */
)
{
  byte *buf_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (prl_ptr == NULL) return 0;

  /* Define the type used in the PARAMS macro.
  */
  #define MTYPE prl_fixed_roaming_list_type

  /* Pointer to roaming list. Initialize buf_ptr to the start of the roaming list
  */
  buf_ptr = prl_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the PRL size field.
  */
  return gsm1x_b_unpackw(PARAMS(pr_list_size));
}

#endif /* FEATURE_GSM1x */


#endif /* FEATURE_SD20 */

