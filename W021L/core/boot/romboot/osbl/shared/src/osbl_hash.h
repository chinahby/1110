#ifndef OSBL_HASH_H
#define OSBL_HASH_H

/*===========================================================================

               O S  S e c o n d a r y   B o o t   L o a d e r
                           H a s h  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the OSBL SHA-1 hash module, and declarations for the public functions
  in the module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_hash.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/07   MJS     Initial revision, ported from MDM7800 and QSD8650.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_hash_if.h"
#include "boot_flash_trans_if.h"
#include "secboot_api.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define OSBL_HASH_NAME "osbl_hash"
#define OSBL_HASH_VERSION 1

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

FUNCTION  OSBL_HASH_INIT_AND_REGISTER

DESCRIPTION
  Function to initialize and register the OSBL hash module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_hash_init_and_register( void );

#endif /* OSBL_HASH_H */

