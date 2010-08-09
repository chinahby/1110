#ifndef OSBL_BOOT_MSM_H
#define OSBL_BOOT_MSM_H

/*===========================================================================

               O S  S e c o n d a r y   B o o t   L o a d e r
               M S M - s p e c i f i c  C o m m o n  B o o t
                           H e a d e r  F i l e

DESCRIPTION
 Contains target-specific defines related to common boot code.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/07   ly      Create

============================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "osbl_error_handler.h"
#include "osbl_target.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Map the ELF loader error handler to the SBL error handler */
#define BOOT_ELF_VERIFY( cond, err ) \
  BL_VERIFY( cond, err + BL_ERR_ELF_START )
#define BOOT_ELF_VERIFY2( cond, err, ret_val )  BOOT_ELF_VERIFY( cond, err )

/* Map the boot ELF loader error handler to the SBL error handler */
#define BOOT_SEC_ELF_VERIFY( cond, err )  BOOT_ELF_VERIFY( cond, err )
#define BOOT_SEC_ELF_VERIFY2( cond, err, ret_val )  BOOT_ELF_VERIFY2( cond, err, ret_val )

/* Map the NAND boot flash access error handler to the SBL error handler */
#define BFA_NAND_VERIFY( cond ) \
  BL_VERIFY( cond, BL_ERR_BOOT_FLASH_ACCESS )
#define BFA_NAND_VERIFY2( cond, ret_val )  BFA_NAND_VERIFY( cond )

/* Map the NOR boot flash access error handler to the SBL error handler */
#define BFA_NOR_VERIFY( cond )  BFA_NAND_VERIFY( cond )
#define BFA_NOR_VERIFY2( cond, ret_val )  BFA_NAND_VERIFY2( cond, ret_val )

/* Map the boot clobber error handler to the SBL error handler */
#define BOOT_CLOBBER_VERIFY( cond ) \
  BL_VERIFY( cond, BL_ERR_CLOBBER_PROTECTION_VIOLATION )

/* Map boot clobber global address check to OSBL macro */
#define BOOT_CLOBBER_GLOBAL_ADDR_CHECK( addr ) \
  OSBL_VALID_ADDR_INC_IRAM( (addr), BL_ERR_IMG_DEST_PTR_INVALID )

#endif /* OSBL_BOOT_MSM_H*/
