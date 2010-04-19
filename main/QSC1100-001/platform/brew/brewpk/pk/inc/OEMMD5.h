/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMMD5.h

SERVICES:  OEM MD5 Interface

GENERAL DESCRIPTION:
   Definitions, typedefs, etc. for OEM MD5 System

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef OEMMD5_H
#define OEMMD5_H

#include "AEEComdef.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**************************************************************************/
/*                     MD5 SYSTEM EXPORTED OPERATIONS                     */
/**************************************************************************/
/* MD5 context. */
typedef struct {
  uint32 state[4];        /* state (ABCD) */
  uint32 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  uint8  buffer[64];      /* input buffer */
} OEMMD5_CTX;

#ifdef __cplusplus
extern "C" {
#endif

extern void OEMMD5_Init (OEMMD5_CTX *);
extern void OEMMD5_Update (OEMMD5_CTX *, uint8 *, uint32);
extern void OEMMD5_Final (uint8 [16], OEMMD5_CTX *);

#if defined(__cplusplus)
}
#endif


/*========================================================================
  INTERFACES DOCUMENTATION
==========================================================================
Interface Name: OEMMD5 
==========================================================================
Function: OEMMD5_Init()

Description:
   Begins an MD5 operation, writing a new context.

Prototype:
   void OEMMD5_Init( OEMMD5_CTX *context );

Parameters:
   *context:  Pointer to a context that will be initialized.

Return Value:
   None

Comments:
   None

Side Effects:

See Also:

========================================================================

Function: OEMMD5_Update()

Description:
  Continues an MD5 message-digest operation, processing another message
  block, and updating the context.

Prototype:
   void  OEMMD5_Update( OEMMD5_CTX *context, uint8 *input, uint32 inputLen );

Parameters:
   *context:  MD5 context.
   *input:    Input block.
   inputLen:  Length of the input block.

Return Value:
   None

Comments:

Side Effects:

See Also:

========================================================================

Function: OEMMD5_Final()

Description:
  Ends an MD5 message-digest operation, writing the message digest and
  zeroizing the context.

Prototype:
   void  OEMMD5_Final( uint8 digest[16], OEMMD5_CTX *context );

Parameters:
   digest:  Message digest.
   *context:  Pointer to the MD5 context.

Return Value:
   none

Comments:

Side Effects:

See Also:

========================================================================*/

#endif /* OEMMD5_H */
