#ifndef OSBL_FLASH_H
#define OSBL_FLASH_H
/*===========================================================================

               OS  S e c o n d a r y   B o o t   L o a d e r
                               F l a s h 
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the OSBL
  software to intialize the boot flash device module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_flash.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "osbl.h"
#include "boot_flash_trans_nand.h"
#include "boot_flash_trans_nor.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/* Forward declarations */
struct flash_partition_table;
struct bad_block_table;


/* Definition of the OSBL flash shared data structure */
typedef struct osbl_flash_shared_data_type
{
  struct flash_partition_table  *sys_partition_ptr;
  uint32                         amss_header_offset;

  struct
  {
    struct bad_block_table    *bad_block_table_ptr;
  } nand;

} osbl_flash_shared_data_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  OSBL_SETUP_FLASH_MPU

DESCRIPTION
  Wrapper function that calls the setup_mpu function in the flash device
  interface.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_setup_flash_mpu( bl_shared_data_type *bl_shared_data );

/*===========================================================================

FUNCTION  OSBL_SHARE_FLASH_DATA

DESCRIPTION
  Wrapper function that calls the share_data function in the flash device
  interface.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_share_flash_data( bl_shared_data_type *bl_shared_data );

/*===========================================================================

FUNCTION  OSBL_FLASH_INIT

DESCRIPTION
  Initializes the boot (flash) device.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_flash_init( bl_shared_data_type *bl_shared_data );

#endif /* OSBL_FLASH_H */

