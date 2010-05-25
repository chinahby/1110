#ifndef OSBL_DLOAD_H
#define OSBL_DLOAD_H

/*===========================================================================

               O E M  S e c o n d a r y  B o o t  L o a d e r
                          D o w n  L o a d e r 
                          H e a d e r  F i l e

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the QPST
  download functionality of the OSBL.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_dload.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/21/05   SD      Added support for QPST download security.
10/13/05   SD      Initial revision.
            
============================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_flash_dev_if.h"
#include "osbl.h"

#ifdef FEATURE_DLOAD_SECURITY
#include "miheader.h"
#endif /* FEATURE_DLOAD_SECURITY */


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DLOAD_SECURITY
/* Re-map the SBL dload authentication routine */
#define sbl_dload_auth_flash_prog   osbl_dload_auth_flash_prog
#endif /* FEATURE_DLOAD_SECURITY */

/* Pointer to a function that has no arguments and returns nothing. */
typedef void (*func_ptr_type)(void);

/* Forward declaration */
struct bl_shared_data_type;

#ifdef FEATURE_OSBL_MULTI_TASK
extern int enter_dload;
#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================

FUNCTION  OSBL_DLOAD_ENTRY

DESCRIPTION
  This function enters the downloader to support QPST download from the
  OSBL. It also tries to bring up splash screen by loading and booting AARM

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Enters the downloader and never returns.

=============================================================================*/
extern void osbl_dload_entry
(
  struct bl_shared_data_type *bl_shared_data
);

#ifdef FEATURE_DLOAD_HW_RESET_DETECT
/*===========================================================================

FUNCTION  OSBL_DLOAD_RESET_SET_SPLASH_FLAGS

DESCRIPTION
  This function will update the hw reset detect flags for splash screen.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None                                                               

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_dload_reset_set_splash_flags
(
  struct bl_shared_data_type *bl_shared_data
);

/*===========================================================================

FUNCTION  OSBL_DLOAD_GET_RESET_STATUS

DESCRIPTION
  This function will check the flag to see if a reset was detected.
 
DEPENDENCIES
  This function must be called before smem_init or smem_boot_init so that 
   the reset flag does not get cleared by smem
  
RETURN VALUE
  None                                                               

SIDE EFFECTS
  Updates cached value of the reset status

===========================================================================*/
extern void osbl_dload_get_reset_status
(
  bl_shared_data_type *bl_shared_data
);
#endif /* FEATURE_DLOAD_HW_RESET_DETECT */
/*===========================================================================

FUNCTION  OSBL_DLOAD_CHECK

DESCRIPTION
  This function will check to see if the downloader should be entered
  for QPST download, and enter the downloader if directed to.
 
DEPENDENCIES
  Download ID must be present in IRAM if downloader is to be entered.
  
RETURN VALUE
  None                                                               

SIDE EFFECTS
  Boot may be halted and QPST downloader entered.

===========================================================================*/
extern void osbl_dload_check( bl_shared_data_type *bl_shared_data );

#ifdef FEATURE_DLOAD_SECURITY
/*===========================================================================

FUNCTION  OSBL_DLOAD_AUTH_FLASH_PROG

DESCRIPTION
  This function will authenticate the flash programmer downloaded by
  the QPST if security is enabled.  The image pointer argument points
  to the start of the signed image header that was downloaded to RAM
  by the QPST.
 
DEPENDENCIES
  None
  
RETURN VALUE
  A pointer to the entry point of the flash programmer code image if
  the flash programmer authentication is successful, otherwise NULL if
  the authentication failed.  

SIDE EFFECTS
  None

===========================================================================*/
func_ptr_type osbl_dload_auth_flash_prog
( 
  mi_boot_image_header_type *image_ptr    /* Ptr to header at start of
                                             signed flash programmer image */
);
#endif /* FEATURE_DLOAD_SECURITY */

#endif  /* OSBL_DLOAD_H */

