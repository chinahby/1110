#ifndef BOOT_SHARED_PBOOT_BLOCK_H
#define BOOT_SHARED_PBOOT_BLOCK_H

/*===========================================================================

                         B o o t  C F G  T a b l e
 
GENERAL DESCRIPTION
  This file contains the code to such that the processed config data can be
  stored in shared memory by OEMSBL and then later used by AMSS code.
  
EXTERNALIZED FUNCTIONS
  None
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/boot_shared_progressive_boot_block.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/06   anb      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#endif /* _ARM_ASM_ */

#include "miprogressive.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifndef _ARM_ASM_

extern progressive_boot_block_type *pboot_block_ptr;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  BOOT_PBOOT_BLOCK_AMSS_GET

DESCRIPTION
 This function will get the Progressive Boot Block Initialized in the OEMSBL

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void boot_pboot_block_amss_get(void);

#ifdef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  BOOT_PBOOT_BLOCK_OEMSBL_INIT

DESCRIPTION
 This function will init Progressive Boot Block in OEMSBL.

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void boot_pboot_block_oemsbl_init(void);

#endif /* BUILD_BOOT_CHAIN */

#endif /* _ARM_ASM_ */

#endif /* BOOT_SHARED_PBOOT_BLOCK_H */

