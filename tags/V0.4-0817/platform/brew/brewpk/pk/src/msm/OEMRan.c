/*======================================================
FILE:  OEMRan.c

SERVICES:  OEM Random Number Generation Functions

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
	List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "ran.h"
#include "OEMStdLib.h"
#include "OEMRan.h" 
#include "OEMMD5.h"

#if defined(FEATURE_DH)
extern byte* get_rand_from_dh(void);
#endif// defined(FEATURE_DH)


void OEMRan_Seed ( uint32 seed )
{
   ran_seed( seed );
  return;
}


uint32 OEMRan_Next ( void )
{
   return( ran_next()>>1 );
}


/*=========================================================================

Function: 
   OEMRan_GetNonPseudoRandomBytes

Description:
  Return 20 bytes of crypto quality random data. This routine need not 
  return new random data more than about once every 100ms.
  
  If you don't have a source of pure random numbers, fill the buffer with 
  zeros and return.

Prototype:
   void OEMRan_GetNonPseudoRandomBytes(byte *pbRand, int *pcbLen);

Parameters:
   pbRand - buffer to fill with random data
   pcbLen - length of buffer on input, length of random data on output

Return Value:
   None

Comments:
   Brew internally produces cryptographic quality random numbers using key 
   stroke and network timing and randomness in the memory manager. However 
   if an additional very high quality source of randomness is available it 
   can be fed in to the pool here. A good example would be noise taken of 
   the phone antenna. A bad example would be randomness from key strokes 
   or time. 

   If you have more good random data, return it 20 bytes at a time when 
   called here. If the pool you have is larger than 20 bytes, used SHA-1 or 
   MD5 to reduce it to 20 bytes. If you don't have good random data this 
   should return a buffer of zeros.

   No concern should be given here for accumulating or stirring the random 
   pool. This is all handled internally.

Side Effects:

See Also:

=========================================================================*/
void OEMRan_GetNonPseudoRandomBytes(byte *pbRand, int *pcbLen)
{
   byte *pbDH = NULL;

   if (pbRand && pcbLen) {
#if defined(FEATURE_DH)
      pbDH = get_rand_from_dh();
#endif

      // clear buffer
      memset(pbRand, 0, *pcbLen);

      // fill with random data from a purely random source (DH in this case)
      if (pbDH != NULL) {
         *pcbLen = MIN(*pcbLen, OEM_DH_RAND);
         memmove(pbRand, pbDH, *pcbLen);
      }
   }
}
