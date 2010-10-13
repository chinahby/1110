/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        C R C    S E R V I C E S

GENERAL DESCRIPTION
  This module contains functions for calculating a checksum for the configuration 
  data used in ROM code.

REFERENCES
  CRC-30, CAI Section 6.7.1.2.2, 7.7.1.2.2, 7.7.2.2.2

EXTERNALIZED FUNCTIONS
  boot_crc_30_step - calculate a step-by-step 30-bit CRC over disjunct data.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2004,2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_crc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/04   tkuo    Update comments.
05/17/04   tkuo    Ported from pbl_crc.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "boot_crc.h"                /* CRC typedefs and definitions         */
#include "boot_util.h"               /* Use boot utility function to convert
                                     ** byte to dword for 32 access */ 

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* The CRC table size is based on how many bits at a time we are going
** to process through the table.  Given that we are processing the data
** 8 bits at a time, this gives us 2^8 (256) entries.
*/
#define CRC_TAB_SIZE    256             /* 2^CRC_TAB_BITS      */

/* CRC table for 30 bit CRC, with generator polynomial 0x6030b9c7,
** calculated 8 bits at a time, MSB first.
*/
const dword boot_crc30_table[ CRC_TAB_SIZE ] = {
  0x00000000, 0x2030b9c7, 0x2051ca49, 0x0061738e,
  0x20932d55, 0x00a39492, 0x00c2e71c, 0x20f25edb,
  0x2116e36d, 0x01265aaa, 0x01472924, 0x217790e3,
  0x0185ce38, 0x21b577ff, 0x21d40471, 0x01e4bdb6,
  0x221d7f1d, 0x022dc6da, 0x024cb554, 0x227c0c93,
  0x028e5248, 0x22beeb8f, 0x22df9801, 0x02ef21c6,
  0x030b9c70, 0x233b25b7, 0x235a5639, 0x036aeffe,
  0x2398b125, 0x03a808e2, 0x03c97b6c, 0x23f9c2ab,
  0x240a47fd, 0x043afe3a, 0x045b8db4, 0x246b3473,
  0x04996aa8, 0x24a9d36f, 0x24c8a0e1, 0x04f81926,
  0x051ca490, 0x252c1d57, 0x254d6ed9, 0x057dd71e,
  0x258f89c5, 0x05bf3002, 0x05de438c, 0x25eefa4b,
  0x061738e0, 0x26278127, 0x2646f2a9, 0x06764b6e,
  0x268415b5, 0x06b4ac72, 0x06d5dffc, 0x26e5663b,
  0x2701db8d, 0x0731624a, 0x075011c4, 0x2760a803,
  0x0792f6d8, 0x27a24f1f, 0x27c33c91, 0x07f38556,
  0x2824363d, 0x08148ffa, 0x0875fc74, 0x284545b3,
  0x08b71b68, 0x2887a2af, 0x28e6d121, 0x08d668e6,
  0x0932d550, 0x29026c97, 0x29631f19, 0x0953a6de,
  0x29a1f805, 0x099141c2, 0x09f0324c, 0x29c08b8b,
  0x0a394920, 0x2a09f0e7, 0x2a688369, 0x0a583aae,
  0x2aaa6475, 0x0a9addb2, 0x0afbae3c, 0x2acb17fb,
  0x2b2faa4d, 0x0b1f138a, 0x0b7e6004, 0x2b4ed9c3,
  0x0bbc8718, 0x2b8c3edf, 0x2bed4d51, 0x0bddf496,
  0x0c2e71c0, 0x2c1ec807, 0x2c7fbb89, 0x0c4f024e,
  0x2cbd5c95, 0x0c8de552, 0x0cec96dc, 0x2cdc2f1b,
  0x2d3892ad, 0x0d082b6a, 0x0d6958e4, 0x2d59e123,
  0x0dabbff8, 0x2d9b063f, 0x2dfa75b1, 0x0dcacc76,
  0x2e330edd, 0x0e03b71a, 0x0e62c494, 0x2e527d53,
  0x0ea02388, 0x2e909a4f, 0x2ef1e9c1, 0x0ec15006,
  0x0f25edb0, 0x2f155477, 0x2f7427f9, 0x0f449e3e,
  0x2fb6c0e5, 0x0f867922, 0x0fe70aac, 0x2fd7b36b,
  0x3078d5bd, 0x10486c7a, 0x10291ff4, 0x3019a633,
  0x10ebf8e8, 0x30db412f, 0x30ba32a1, 0x108a8b66,
  0x116e36d0, 0x315e8f17, 0x313ffc99, 0x110f455e,
  0x31fd1b85, 0x11cda242, 0x11acd1cc, 0x319c680b,
  0x1265aaa0, 0x32551367, 0x323460e9, 0x1204d92e,
  0x32f687f5, 0x12c63e32, 0x12a74dbc, 0x3297f47b,
  0x337349cd, 0x1343f00a, 0x13228384, 0x33123a43,
  0x13e06498, 0x33d0dd5f, 0x33b1aed1, 0x13811716,
  0x14729240, 0x34422b87, 0x34235809, 0x1413e1ce,
  0x34e1bf15, 0x14d106d2, 0x14b0755c, 0x3480cc9b,
  0x3564712d, 0x1554c8ea, 0x1535bb64, 0x350502a3,
  0x15f75c78, 0x35c7e5bf, 0x35a69631, 0x15962ff6,
  0x366fed5d, 0x165f549a, 0x163e2714, 0x360e9ed3,
  0x16fcc008, 0x36cc79cf, 0x36ad0a41, 0x169db386,
  0x17790e30, 0x3749b7f7, 0x3728c479, 0x17187dbe,
  0x37ea2365, 0x17da9aa2, 0x17bbe92c, 0x378b50eb,
  0x185ce380, 0x386c5a47, 0x380d29c9, 0x183d900e,
  0x38cfced5, 0x18ff7712, 0x189e049c, 0x38aebd5b,
  0x394a00ed, 0x197ab92a, 0x191bcaa4, 0x392b7363,
  0x19d92db8, 0x39e9947f, 0x3988e7f1, 0x19b85e36,
  0x3a419c9d, 0x1a71255a, 0x1a1056d4, 0x3a20ef13,
  0x1ad2b1c8, 0x3ae2080f, 0x3a837b81, 0x1ab3c246,
  0x1b577ff0, 0x3b67c637, 0x3b06b5b9, 0x1b360c7e,
  0x3bc452a5, 0x1bf4eb62, 0x1b9598ec, 0x3ba5212b,
  0x3c56a47d, 0x1c661dba, 0x1c076e34, 0x3c37d7f3,
  0x1cc58928, 0x3cf530ef, 0x3c944361, 0x1ca4faa6,
  0x1d404710, 0x3d70fed7, 0x3d118d59, 0x1d21349e,
  0x3dd36a45, 0x1de3d382, 0x1d82a00c, 0x3db219cb,
  0x1e4bdb60, 0x3e7b62a7, 0x3e1a1129, 0x1e2aa8ee,
  0x3ed8f635, 0x1ee84ff2, 0x1e893c7c, 0x3eb985bb,
  0x3f5d380d, 0x1f6d81ca, 0x1f0cf244, 0x3f3c4b83,
  0x1fce1558, 0x3ffeac9f, 0x3f9fdf11, 0x1faf66d6
};

/* CRC table for 32 bit CRC with generator polynomial 0x04c11db7,
** calculated 8 bits at a time, MSB first.
*/
const dword boot_crc32_table[ CRC_TAB_SIZE ] = {  
  0x00000000UL,  0x04c11db7UL,  0x09823b6eUL,  0x0d4326d9UL,
  0x130476dcUL,  0x17c56b6bUL,  0x1a864db2UL,  0x1e475005UL,
  0x2608edb8UL,  0x22c9f00fUL,  0x2f8ad6d6UL,  0x2b4bcb61UL,
  0x350c9b64UL,  0x31cd86d3UL,  0x3c8ea00aUL,  0x384fbdbdUL,
  0x4c11db70UL,  0x48d0c6c7UL,  0x4593e01eUL,  0x4152fda9UL,
  0x5f15adacUL,  0x5bd4b01bUL,  0x569796c2UL,  0x52568b75UL,
  0x6a1936c8UL,  0x6ed82b7fUL,  0x639b0da6UL,  0x675a1011UL,
  0x791d4014UL,  0x7ddc5da3UL,  0x709f7b7aUL,  0x745e66cdUL,
  0x9823b6e0UL,  0x9ce2ab57UL,  0x91a18d8eUL,  0x95609039UL,
  0x8b27c03cUL,  0x8fe6dd8bUL,  0x82a5fb52UL,  0x8664e6e5UL,
  0xbe2b5b58UL,  0xbaea46efUL,  0xb7a96036UL,  0xb3687d81UL,
  0xad2f2d84UL,  0xa9ee3033UL,  0xa4ad16eaUL,  0xa06c0b5dUL,
  0xd4326d90UL,  0xd0f37027UL,  0xddb056feUL,  0xd9714b49UL,
  0xc7361b4cUL,  0xc3f706fbUL,  0xceb42022UL,  0xca753d95UL,
  0xf23a8028UL,  0xf6fb9d9fUL,  0xfbb8bb46UL,  0xff79a6f1UL,
  0xe13ef6f4UL,  0xe5ffeb43UL,  0xe8bccd9aUL,  0xec7dd02dUL,
  0x34867077UL,  0x30476dc0UL,  0x3d044b19UL,  0x39c556aeUL,
  0x278206abUL,  0x23431b1cUL,  0x2e003dc5UL,  0x2ac12072UL,
  0x128e9dcfUL,  0x164f8078UL,  0x1b0ca6a1UL,  0x1fcdbb16UL,
  0x018aeb13UL,  0x054bf6a4UL,  0x0808d07dUL,  0x0cc9cdcaUL,
  0x7897ab07UL,  0x7c56b6b0UL,  0x71159069UL,  0x75d48ddeUL,
  0x6b93dddbUL,  0x6f52c06cUL,  0x6211e6b5UL,  0x66d0fb02UL,
  0x5e9f46bfUL,  0x5a5e5b08UL,  0x571d7dd1UL,  0x53dc6066UL,
  0x4d9b3063UL,  0x495a2dd4UL,  0x44190b0dUL,  0x40d816baUL,
  0xaca5c697UL,  0xa864db20UL,  0xa527fdf9UL,  0xa1e6e04eUL,
  0xbfa1b04bUL,  0xbb60adfcUL,  0xb6238b25UL,  0xb2e29692UL,
  0x8aad2b2fUL,  0x8e6c3698UL,  0x832f1041UL,  0x87ee0df6UL,
  0x99a95df3UL,  0x9d684044UL,  0x902b669dUL,  0x94ea7b2aUL,
  0xe0b41de7UL,  0xe4750050UL,  0xe9362689UL,  0xedf73b3eUL,
  0xf3b06b3bUL,  0xf771768cUL,  0xfa325055UL,  0xfef34de2UL,
  0xc6bcf05fUL,  0xc27dede8UL,  0xcf3ecb31UL,  0xcbffd686UL,
  0xd5b88683UL,  0xd1799b34UL,  0xdc3abdedUL,  0xd8fba05aUL,
  0x690ce0eeUL,  0x6dcdfd59UL,  0x608edb80UL,  0x644fc637UL,
  0x7a089632UL,  0x7ec98b85UL,  0x738aad5cUL,  0x774bb0ebUL,
  0x4f040d56UL,  0x4bc510e1UL,  0x46863638UL,  0x42472b8fUL,
  0x5c007b8aUL,  0x58c1663dUL,  0x558240e4UL,  0x51435d53UL,
  0x251d3b9eUL,  0x21dc2629UL,  0x2c9f00f0UL,  0x285e1d47UL,
  0x36194d42UL,  0x32d850f5UL,  0x3f9b762cUL,  0x3b5a6b9bUL,
  0x0315d626UL,  0x07d4cb91UL,  0x0a97ed48UL,  0x0e56f0ffUL,
  0x1011a0faUL,  0x14d0bd4dUL,  0x19939b94UL,  0x1d528623UL,
  0xf12f560eUL,  0xf5ee4bb9UL,  0xf8ad6d60UL,  0xfc6c70d7UL,
  0xe22b20d2UL,  0xe6ea3d65UL,  0xeba91bbcUL,  0xef68060bUL,
  0xd727bbb6UL,  0xd3e6a601UL,  0xdea580d8UL,  0xda649d6fUL,
  0xc423cd6aUL,  0xc0e2d0ddUL,  0xcda1f604UL,  0xc960ebb3UL,
  0xbd3e8d7eUL,  0xb9ff90c9UL,  0xb4bcb610UL,  0xb07daba7UL,
  0xae3afba2UL,  0xaafbe615UL,  0xa7b8c0ccUL,  0xa379dd7bUL,
  0x9b3660c6UL,  0x9ff77d71UL,  0x92b45ba8UL,  0x9675461fUL,
  0x8832161aUL,  0x8cf30badUL,  0x81b02d74UL,  0x857130c3UL,
  0x5d8a9099UL,  0x594b8d2eUL,  0x5408abf7UL,  0x50c9b640UL,
  0x4e8ee645UL,  0x4a4ffbf2UL,  0x470cdd2bUL,  0x43cdc09cUL,
  0x7b827d21UL,  0x7f436096UL,  0x7200464fUL,  0x76c15bf8UL,
  0x68860bfdUL,  0x6c47164aUL,  0x61043093UL,  0x65c52d24UL,
  0x119b4be9UL,  0x155a565eUL,  0x18197087UL,  0x1cd86d30UL,
  0x029f3d35UL,  0x065e2082UL,  0x0b1d065bUL,  0x0fdc1becUL,
  0x3793a651UL,  0x3352bbe6UL,  0x3e119d3fUL,  0x3ad08088UL,
  0x2497d08dUL,  0x2056cd3aUL,  0x2d15ebe3UL,  0x29d4f654UL,
  0xc5a92679UL,  0xc1683bceUL,  0xcc2b1d17UL,  0xc8ea00a0UL,
  0xd6ad50a5UL,  0xd26c4d12UL,  0xdf2f6bcbUL,  0xdbee767cUL,
  0xe3a1cbc1UL,  0xe760d676UL,  0xea23f0afUL,  0xeee2ed18UL,
  0xf0a5bd1dUL,  0xf464a0aaUL,  0xf9278673UL,  0xfde69bc4UL,
  0x89b8fd09UL,  0x8d79e0beUL,  0x803ac667UL,  0x84fbdbd0UL,
  0x9abc8bd5UL,  0x9e7d9662UL,  0x933eb0bbUL,  0x97ffad0cUL,
  0xafb010b1UL,  0xab710d06UL,  0xa6322bdfUL,  0xa2f33668UL,
  0xbcb4666dUL,  0xb8757bdaUL,  0xb5365d03UL,  0xb1f740b4UL
};

/*===========================================================================

FUNCTION BOOT_CRC_30_STEP

DESCRIPTION
  This function calculates a 30-bit CRC over a specified number of data
  bits.  It can be used to produce a CRC and to check a CRC.  The CRC value
  passed in is used to continue the CRC calculation from a previous call,
  this allows this routine to be used to CRC discontinuous data.

DEPENDENCIES
  The input parameters have to multiple of 4 data. We can only process up to
  bytes. There is no bit processing is allowed. 


RETURN VALUE
  Returns a double word holding 30 bits which are the contents of the
  CRC register as calculated over the specified data bits.  If this
  function is being used to check a CRC, then the return value will be
  equal to CRC_30_OK (defined in crc.h) if the CRC checks correctly.
  
  NOTE: The caller is expected to mask the bottom 30 bits from the value, 

SIDE EFFECTS
  None

===========================================================================*/

dword boot_crc_30_step
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
)
{
  dword data, crc_30;

  /* Generate a CRC-30 by looking up the transformation in a table and
  ** XOR-ing it into the CRC, one byte at a time.
  */
  for (crc_30 = ~seed & 0x3FFFFFFF; len >= 8; len -= 8, (void)bLoad8(buf_ptr++)) {
    crc_30 = boot_crc30_table[ ((crc_30 >> (30 - 8)) ^ bLoad8(buf_ptr)) & 0xff ]
                ^ (crc_30 << 8);
  }

  /* Finish calculating the CRC over the trailing data bits
  **
  ** XOR the MS bit of data with the MS bit of the CRC.
  ** Shift the CRC and data left 1 bit.
  ** If the XOR result is 1, XOR the generating polynomial in with the CRC.
  */
  if (len != 0) {

    data = ((dword) bLoad8(buf_ptr)) << (30-8); /* Align data MSB with CRC MSB */

    while (len-- != 0) {
      if ( ((crc_30 ^ data) & (1L << 29)) != 0 ) {       /* Is MSB of XOR a 1 */

        crc_30 <<= 1;                   /* Left shift CRC          */
        crc_30 ^= CRC_30_POLYNOMIAL;    /* XOR polynomial into CRC */

      } else {

        crc_30 <<= 1;                   /* Left shift CRC          */

      }

      data <<= 1;                       /* Left shift data         */
    }
  }

  crc_30 = ~crc_30;                     /* 1's complement the CRC     */
  return( crc_30 & 0x3FFFFFFF );        /* mask the result to 30 bits */

} /* end of boot_crc_30_calc */

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

SIDE EFFECTS
  None

===========================================================================*/
dword boot_crc_32_calc
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
)
{
  dword data, crc;

  /*-------------------------------------------------------------------------
   Generate the CRC by looking up the transformation in a table and
   XOR-ing it into the CRC, one byte at a time.
  -------------------------------------------------------------------------*/
  for ( crc = seed; len >= 8; len -= 8, (void)bLoad8(buf_ptr++) )
  {
    crc = boot_crc32_table[ ( ( crc >> 24 ) ^ bLoad8(buf_ptr) ) ] ^ ( crc << 8 );
  }

  /*-------------------------------------------------------------------------
   Finish calculating the CRC over the trailing data bits. This is done by
   aligning the remaining data bits with the CRC MSB, and then computing the
   CRC one bit at a time.
  -------------------------------------------------------------------------*/
  if ( len != 0 )
  {
    /*-----------------------------------------------------------------------
     Align data MSB with CRC MSB.
    -----------------------------------------------------------------------*/
    data = ((dword) bLoad8(buf_ptr)) << 24;

    /*-----------------------------------------------------------------------
     While there are bits left, compute CRC one bit at a time.
    -----------------------------------------------------------------------*/
    while (len-- != 0)
    {
      /*---------------------------------------------------------------------
       If the MSB of the XOR combination of the data and CRC is one, then
       advance the CRC register by one bit XOR with the CRC polynomial.
       Otherwise, just advance the CRC by one bit.
      ---------------------------------------------------------------------*/
      if ( ( ( crc ^ data ) & CRC_32_MSB_MASK ) != 0 )
      {
        crc <<= 1;
        crc ^= CRC_32_POLYNOMIAL;
      }      
      else
      {
        crc <<= 1;
      }

      /*---------------------------------------------------------------------
       Advance to the next input data bit and continue.
      ---------------------------------------------------------------------*/
      data <<= 1;
    }
  }

  /*-------------------------------------------------------------------------
   Return the resulting CRC value.
  -------------------------------------------------------------------------*/
  return( crc );

}

