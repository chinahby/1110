#ifndef SECBOOT_SHA1_H_
#define SECBOOT_SHA1_H_
/*===========================================================================

                         S E C U R I T Y    S E R V I C E S

                            S E C U R E   B O O T  S H A.1

                                 H E A D E R  F I L E

FILE:  secboot_sha1.h

DESCRIPTION:
       This module implements the Secure Hash Alghorithm (SHA.1).

EXTERNALIZED FUNCTIONS
       pbl_SHA1Hash - Interface to this module.

        Copyright © 2004 - 2007 QUALCOMM Incorported. All Rights Reserved.
===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/sec/secboot_sha1.h#2 $
$DateTime: 2008/01/28 10:45:19 $
$Author: wctan $

when       who                 what, where, why
--------   ----                -------------------------------------------
08/23/07   MJS                 Added const qualifiers.
06/06/04   John Alayari        1. Added comments and headings to functions and
                                  corrected some indentations to comply
                                  with ASW Security Services Coding Guideline.
                                  2. Added casting of (unsigned char) to
                                  pbl_SHA1Final function to eliminated warnings
                                  and possible data loss.
           Steve Reid <steve@edmweb.com>      100% Public Domain

===========================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "secboot_i.h"
#include "secboot_api.h"

typedef struct
{
    unsigned int	count[2];
    unsigned long state[5];
    uint8         buffer[64];

   /* added so we don't have to step on    */
   /* data when hashing more than 64 bytes.*/
	  uint8         scratchbuffer[64];
} SHA1_CTX;

/* workers ...*/
void pbl_SHA1Init(SHA1_CTX* context);
void pbl_SHA1Transform(unsigned long state[5], uint8 buffer[64]);
void pbl_SHA1Update(SHA1_CTX* context, const uint8* data, uint32 len);
void pbl_SHA1Final(uint8 *digest_ptr, SHA1_CTX* context);

#endif
