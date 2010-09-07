/*=============================================================================

                        O S  B o o t  L o a d e r
                           D o w n  L o a d e r 

GENERAL DESCRIPTION
  This file contains the OSBL functionality needed to support QPST
  download from the OSBL.

EXTERNALIZED FUNCTIONS
  osbl_dload_entry_with_splash_screen
  osbl_dload_check
  osbl_dload_get_reset_status
  osbl_dload_reset_set_splash_flags
  osbl_dload_auth_flash_prog
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_dload_mproc.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/13/08   MJS     Don't boot the ARM11 when entering dload for WM builds.
01/14/08   MJS     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "osbl_dload.h"
#include "osbl_hw_init.h"
#include "osbl_mc_target.h"

#ifdef FEATURE_DLOAD_SECURITY
#include "miheader.h"
#endif /* FEATURE_DLOAD_SECURITY */

#ifdef FEATURE_DLOAD_HW_RESET_DETECT
#include "boot_hw_reset.h"
#include "osbl_aarm_boot.h"
#endif /* FEATURE_DLOAD_HW_RESET_DETECT */

#include "boot_dload.h"
#include "boot_auth_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#ifdef FEATURE_DLOAD_HW_RESET_DETECT
static boot_hw_reset_type osbl_dload_reset_status = BOOT_HW_RESET_NONE;
#endif /* FEATURE_DLOAD_HW_RESET_DETECT */

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION  OSBL_DLOAD_ENTRY

DESCRIPTION
  This function enters the downloader to support QPST download from the
  OSBL. It also tries to bring up splash screen by loading appsbl and 
  booting AARM

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Enters the downloader and never returns.

=============================================================================*/
void osbl_dload_entry( bl_shared_data_type *bl_shared_data )
{

#if defined(FEATURE_DLOAD_HW_RESET_DETECT) && !defined(FEATURE_BOOT_IMAGE_WINMOB)
  static uint32 dload_entry_count = 0;

  dload_entry_count++; 

  if( dload_entry_count == 1 && bl_shared_data != NULL )
  {
    /* Entering dload routine for the first time */

    /* Display splash screen (done by APPSBL) */
    osbl_do_procedures( bl_shared_data, osbl_dload_splash_procs );
  }
#endif /* FEATURE_DLOAD_HW_RESET_DETECT && !FEATURE_BOOT_IMAGE_WINMOB */

  /* If we are entering the dload routine for 2nd time, it means that there
     was error in loading appsbl image in earlier entry to this routine and 
     thus we enter dload without loading appsbl image. 
     i.e without displaying splash screen 
   */

  /* Enter downloader for QPST */
  boot_dload_enter_dload();
 
} /* osbl_dload_entry() */

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
void osbl_dload_reset_set_splash_flags( bl_shared_data_type *bl_shared_data )
{
  if( osbl_dload_reset_status == BOOT_HW_RESET_DETECT_ENABLED )
  {
    /* Set flags for RESET splash screen */
    boot_hw_reset_detect_set(BOOT_HW_RESET_SPLASH_RESET);
  }
  else
  {
    /* Set flags for DLOAD splash screen */
    boot_hw_reset_detect_set(BOOT_HW_RESET_SPLASH_DLOAD);
  }
} /* osbl_dload_reset_set_splash_flags() */

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
void osbl_dload_get_reset_status
(
  bl_shared_data_type *bl_shared_data
)
{
  /* Init reset detect functionality.  Optional (will be called internally if
   * not initialized), but allows us to better support boot-up scripts if
   * we explicity call it here.
   */
  boot_hw_reset_detect_init();

  osbl_dload_reset_status = boot_hw_reset_detect_status();
} /* osbl_dload_get_reset_status() */
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
void osbl_dload_check( bl_shared_data_type *bl_shared_data )
{
  /* Determine if the downloader should be entered at this time,
     instead of continuing with the normal boot process. */
#ifdef FEATURE_DLOAD_HW_RESET_DETECT
  if (( boot_dload_entry( ) == TRUE ) ||
      ( osbl_dload_reset_status == BOOT_HW_RESET_DETECT_ENABLED ))
#else
  if ( boot_dload_entry( ) == TRUE )
#endif /* FEATURE_DLOAD_HW_RESET_DETECT */
  {
    /* Enter downloader for QPST */  
    osbl_dload_entry( bl_shared_data );
  }
  
} /* osbl_dload_check() */

#ifdef FEATURE_DLOAD_SECURITY
/*===========================================================================

FUNCTION  SBL_DLOAD_AUTH_FLASH_PROG

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
)
{
  /* Verify the image pointer is not NULL. */
  if( image_ptr == NULL )
  {
    return NULL;
  }

  /* Authenticate flash programmer if security is enabled on chip. */
  if ( boot_auth_authenticate( image_ptr ) )
  {
    /* Authentication was successful. */
    return (func_ptr_type) image_ptr->image_dest_ptr;
  }
  else
  {
    /* Authentication failed. */
    return NULL;
  }
  
} /* osbl_dload_auth_flash_prog() */
#endif /* FEATURE_DLOAD_SECURITY */

