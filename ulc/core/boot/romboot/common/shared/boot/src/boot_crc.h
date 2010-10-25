#ifndef BOOT_CRC_H
#define BOOT_CRC_H
/*===========================================================================

              C R C    S E R V I C E S    D E C L A R A T I O N S

DESCRIPTION
  The following declarations are for use with the CRC Services.

Copyright(c) 2004,2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_crc.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/05   tkuo    Change the prototype to 32 bit access.
05/17/04   tkuo    Rename to boot_crc.h.
04/13/04   tkuo    Ported from jnand_crc.h

===========================================================================*/

#include "boot_comdef.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* Residual CRC value to compare against return value of crc_30_calc().
** Use crc_30_calc() to calculate a 30 bit CRC, and append it to the buffer.
** When crc_30_calc() is applied to the unchanged result, it returns CRC_30_OK.
*/
#define  CRC_30_OK  0x0B105AA5

/* Mask for CRC-30 polynomial:
**
**      x^30+x^29+x^21+x^20+x^15+x^13+x^12+x^11+x^8+x^7+x^6+x^2+x^1+1
**
** Note:  the x^30 tap is left off, it's implicit.
*/
#define CRC_30_POLYNOMIAL       0x6030B9C7

/* Seed value for CRC register.  The all ones seed allows detection of
** an entire data stream of zeroes.
*/
#define CRC_30_SEED             0x3FFFFFFF

/* Seed value for CRC30 register.  The CRC30 seed is inverted prior to
** being used in the step-wise CRC-30.  This behaves as CRC_30_SEED.
*/
#define CRC_30_STEP_SEED        (~((dword) CRC_30_SEED))

/* Seed value for 32 bit CRC */
#define CRC_32_SEED             0x00000000UL

/* Mask for 32 bit CRC polynomial:
 *
 *   x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+1
 *
 * Note: the x^32 tap can be left off from the polynomial as it is implicit.
 */
#define CRC_32_POLYNOMIAL       0x04c11db7UL

/* Most significant bit mask for CRC.
 */
#define CRC_32_MSB_MASK         0x80000000UL

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION BOOT_CRC_30_STEP

DESCRIPTION
  This function calculates a 30-bit CRC over a specified number of data
  bits.  It can be used to produce a CRC and to check a CRC.  The CRC value
  passed in is used to continue the CRC calculation from a previous call,
  this allows this routine to be used to CRC discontinuous data.

DEPENDENCIES
  None

RETURN VALUE
  Returns a double word holding 30 bits which are the contents of the
  CRC register as calculated over the specified data bits.  If this
  function is being used to check a CRC, then the return value will be
  equal to CRC_30_OK (defined in crc.h) if the CRC checks correctly.
  
  NOTE: The caller is expected to mask the bottom 30 bits from the value, 

SIDE EFFECTS
  None

===========================================================================*/

extern dword boot_crc_30_step
(
  dword seed,
    /* Either the result of a previous boot_crc_30_step() or CRC_16_STEP_SEED
    ** the first time the routine is called.  Note that the input is
    ** inverted prior to use, to counteract the inversion applied when
    ** it was returned as a result of the previous step.
    */

  byte *buf_ptr,
    /* Pointer to bytes containing the data to CRC.  The bit stream starts
    ** in the MS bit of the first byte.
    */

  dword len
    /* Number of data bits to calculate the CRC over */
);

/*===========================================================================

FUNCTION BOOT_CRC_32_CALC

DESCRIPTION
  This function calculates a 32-bit CRC over a specified number of data
  bits.  It can be used to produce a CRC and to check a CRC.  The CRC value
  passed in is used to continue the CRC calculation from a previous call,
  this allows this routine to be used to CRC discontinuous data.

DEPENDENCIES
  None

RETURN VALUE
  Returns a double word holding 30 bits which are the contents of the
  CRC register as calculated over the specified data bits.  If this
  function is being used to check a CRC, then the return value will be
  equal to CRC_30_OK (defined in crc.h) if the CRC checks correctly.
  
  NOTE: The caller is expected to mask the bottom 30 bits from the value, 

SIDE EFFECTS
  None

===========================================================================*/
extern dword boot_crc_32_calc
(
  byte *buf_ptr,
    /* Pointer to bytes containing the data to CRC.  The bit stream starts
    ** in the MS bit of the first byte.
    */

  dword len,
    /* Number of data bits to calculate the CRC over */

  dword seed
    /* Either the result of a previous boot_crc_30_step() or CRC_16_STEP_SEED
    ** the first time the routine is called.  Note that the input is
    ** inverted prior to use, to counteract the inversion applied when
    ** it was returned as a result of the previous step.
    */
);

#endif /* BOOT_CRC_H */
