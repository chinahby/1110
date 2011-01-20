/*=============================================================================

               O S  S e c o n d a r y   B o o t   L o a d e r
                  F l a s h   I n i t i a l i z a t i o n

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the OSBL
  software to intialize the boot flash device module.

EXTERNALIZED FUNCTIONS
  osbl_setup_flash_mpu
  osbl_share_flash_data

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
=============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_flash.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/07   MJS     Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <string.h>
#include "boot_comdef.h"
#include "osbl.h"
#include "osbl_flash.h"
#include "boot_flash_dev_if.h"
#include "osbl_flash_target.h"
#include "osbl_error_handler.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/


/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/


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
void osbl_setup_flash_mpu( bl_shared_data_type *bl_shared_data )
{
  boolean rc;

  OSBL_VERIFY( ( bl_shared_data != NULL ), BL_ERR_NULL_PTR_PASSED );

  rc = boot_flash_dev_setup_mpu( bl_shared_data );
  OSBL_VERIFY( rc, BL_ERR_OSBL );

} /* osbl_setup_flash_mpu() */

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
void osbl_share_flash_data( bl_shared_data_type *bl_shared_data )
{
  static osbl_flash_shared_data_type osbl_flash_shared_data;

  boolean rc;

  OSBL_VERIFY( ( bl_shared_data != NULL ), BL_ERR_NULL_PTR_PASSED );

  memset( &osbl_flash_shared_data, 0, sizeof(osbl_flash_shared_data) );

  bl_shared_data->boot_dev_shared_data = &osbl_flash_shared_data;
  
  rc = boot_flash_dev_share_data( bl_shared_data );
  OSBL_VERIFY( rc, BL_ERR_OSBL );

} /* osbl_share_flash_data() */

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
void osbl_flash_init( bl_shared_data_type *bl_shared_data )
{
  /* Nother to do here for now, just invoke the target-specific function */

  osbl_flash_target_init( bl_shared_data );

} /* osbl_flash_init() */
