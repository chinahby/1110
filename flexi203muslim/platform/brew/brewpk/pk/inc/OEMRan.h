/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMRan.h

SERVICES:  Random Number Generator Interface

GENERAL DESCRIPTION:
	Definitions, typedefs, etc. for Random Number Generation

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
#ifndef OEMRAN_H
#define OEMRAN_H

#include "AEEComdef.h"

#define OEM_DH_RAND 20 // Number of bytes of Random number generated in DH by the random number generator

/**************************************************************************/
/*                        EXPORTED OPERATIONS                             */
/**************************************************************************/
extern void   OEMRan_Seed ( uint32 seed );
extern uint32 OEMRan_Next ( void );
extern void OEMRan_GetNonPseudoRandomBytes(byte *pbRand, int *pcbLen);

/*========================================================================
  INTERFACES DOCUMENTATION
=======================================================================
OEMRan Interface
=======================================================================

Function: OEMRan_Seed()

Description:
  Seeds the random number generator.

Prototype:
   void  OEMRan_Seed
   (
      uint32 seed
   );

Parameters:
   seed: seed value for random number generator

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=========================================================================

Function: OEMRan_Next()

Description:
  Returns the next number in the sequence.

Prototype:
   uint32 OEMRan_Next
   (
      void
   );

Parameters:
   None

Return Value:
   Returns the next number in the sequence

Comments:
   None

Side Effects:
   None

See Also:
   None

=========================================================================

Function: OEMRan_GetNonPseudoRandomBytes()

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

=========================================================================*/
#endif











