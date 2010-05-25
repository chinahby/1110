#ifndef OSBL_FLASH_NAND_OTP_UID_H
#define OSBL_FLASH_NAND_OTP_UID_H
/*===========================================================================

               O E M  S e c o n d a r y  B o o t  L o a d e r
                         F l a s h  O T P  U I D
                          H e a d e r  F i l e

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the OSBL
  Flash OTP UID functionality.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_flash_nand_otp_uid.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/07   MJS     Initial version.

============================================================================*/

/*===========================================================================
  
                           INCLUDE FILES
                           
===========================================================================*/
#include "boot_flash_dev_nand_if.h"

/*===========================================================================
  
                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION OSBL_GET_WM_UUID

DESCRIPTION
  This function retrieves page 0 from the flash redundant block in the 
  OTP area, extracts and verifies the flash device UID (Unique IDentifier),
  and stores the value in shared memory for use by Windows Mobile as the UUID
  (Universally Unique IDentifier).

DEPENDENCIES
  NAND device must exist for boot or EFS.

RETURN VALUE
  None

===========================================================================*/
extern void osbl_get_wm_uuid( void );

#endif /* OSBL_FLASH_NAND_OTP_UID_H */

