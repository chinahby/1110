/*=============================================================================

FILE: OEMMD5.c

SERVICES: OEM Functions for MD5

GENERAL DESCRIPTION:
A sample implementation of the OEMMD5 functions declared in
OEMMD5.h.
  
PUBLIC CLASSES AND STATIC FUNCTIONS:

   OEMMD5_Init
   OEMMD5_Update
   OEMMD5_Final

INITIALIZATION AND SEQUENCING REQUIREMENTS:
No additional module-level initialization or sequencing requirements.

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include <stdlib.h>
#include "OEMFeatures.h"
#include "md5.h"
#include "AEEComdef.h"
#include "OEMMD5.h"

/*==================================================================
Function: OEMMD5_Init

Description: Begins an MD5 operation, writing a new context.
==================================================================*/
void  OEMMD5_Init (OEMMD5_CTX *context)
{
     MD5Init ((MD5_CTX *)context);
}

/*==================================================================
Function: OEMMD5_Update

Description: Continues an MD5 message-deigest operation, processing
             another message block, and updating the context.
==================================================================*/
void  OEMMD5_Update (OEMMD5_CTX *context, uint8 *input, uint32 inputLen)
{
    MD5Update ((MD5_CTX *)context, input, inputLen);
}

/*==================================================================
Function: OEMMD5_Final

Description: Ends an MD5 message-digest operation, writing the message
             digest, and zeroizing the context.

==================================================================*/
void  OEMMD5_Final (uint8 digest[16], OEMMD5_CTX *context)
{
    MD5Final (digest, (MD5_CTX *)context);
}

