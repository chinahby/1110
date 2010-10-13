#ifndef OSBL_FLASH_DEV_NAND_MPROC_H
#define OSBL_FLASH_DEV_NAND_MPROC_H

/*===========================================================================

                O S  S e c o n d a r y   B o o t   L o a d e r
                 N A N D  F l a s h  D e v i c e  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the QCSBL NAND flash device module, and declarations for the public
  functions in the module.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_flash_dev_nand_mproc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_flash_dev_nand_if.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION  OSBL_FLASH_DEV_NAND_SETUP_NAND_MPU

DESCRIPTION
  Configures the NAND MPU.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
a
===========================================================================*/
extern boolean osbl_flash_dev_nand_setup_nand_mpu
(
  bl_shared_data_type    *bl_shared_data
);

#endif /* OSBL_FLASH_DEV_NAND_MPROC_H */

