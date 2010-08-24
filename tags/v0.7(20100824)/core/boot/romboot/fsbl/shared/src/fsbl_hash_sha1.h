#ifndef FSBL_HASH_SHA1_H
#define FSBL_HASH_SHA1_H

/*===========================================================================

                  F A I L  S A F E  B o o t  L o a d e r
                      S H A - 1  H a s h  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the FSBL SHA-1 hash module, and declarations for the public functions
  in the module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_hash_sha1.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_hash_if.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define FSBL_HASH_SHA1_NAME "fsbl_hash_sha1"
#define FSBL_HASH_SHA1_VERSION 1


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

FUNCTION  FSBL_HASH_SHA1_INIT

DESCRIPTION
  Function to initialize an instance of the FSBL hash module.  The flash_if
  parameter is optional and is used to connect the flash copy callback routine 
  with the flash access interface, to support incrementally hashing each block
  of data from flash as it is loaded.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the public hash interface.

SIDE EFFECTS
  None

===========================================================================*/
extern boot_hash_if_type * fsbl_hash_sha1_init(void);

#endif /* FSBL_HASH_SHA1_H */

