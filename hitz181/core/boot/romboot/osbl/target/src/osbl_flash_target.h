#ifndef OSBL_FLASH_TARGET_H
#define OSBL_FLASH_TARGET_H
/*===========================================================================

               OS  S e c o n d a r y   B o o t   L o a d e r
                         F l a s h  T a r g e t
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

$Header: //depot/asic/qsc1100/romboot/osbl/osbl_flash_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "osbl.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/


extern uint8 bl_page_buf[];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  OSBL_FLASH_TARGET_INIT

DESCRIPTION
  This function determines the flash type booting from and sets up
  parameters for flash access.

DEPENDENCIES
  None
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void osbl_flash_target_init( bl_shared_data_type *bl_shared_data );

#endif /* OSBL_FLASH_TARGET_H */

