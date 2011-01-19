/*=============================================================================

               Operating System  B o o t   L o a d e r
                           D o w n  L o a d e r

GENERAL DESCRIPTION
  This file contains the OSBL functionality needed to support QPST
  download from the OSBL.

EXTERNALIZED FUNCTIONS
  osbl_dload_entry
  osbl_dload_check

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/core/romboot/osbl/shared/src/osbl_dload_uniproc.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/16/08    tnk     Brached from MSM_shared and included the fastenum
11/07/07   ly      Create
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "osbl.h"
#include "osbl_dload.h"
#include "osbl_hw_init.h"

#ifdef FEATURE_DLOAD_SECURITY
#include "osbl_mc.h"
#include "miheader.h"
#endif /* FEATURE_DLOAD_SECURITY */

#include "boot_dload.h"
#include "boot_dload_if.h"
#ifdef FEATURE_DLOAD_HW_RESET_DETECT
#include "boot_hw_reset.h"
#endif

#ifdef FEATURE_OSBL_MULTI_TASK
int enter_dload = 0;
#endif

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION  OSBL_DLOAD_ENTRY

DESCRIPTION
  This function enters the downloader to support QPST download from the
  OSBL.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Enters the downloader and never returns.

=============================================================================*/
void osbl_dload_entry( bl_shared_data_type *bl_shared_data )
{
  /* Enter downloader for QPST */
  boot_dload_enter_dload();
 
} /* osbl_dload_entry() */

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
  if (
#ifdef FEATURE_DLOAD_HW_RESET_DETECT
       (boot_hw_reset_dload_entry() == TRUE) ||
#endif
       (boot_dload_entry() == TRUE)
     )
  {
    /* Enter downloader for QPST */
    #ifdef FEATURE_OSBL_MULTI_TASK
    enter_dload = 1;
    #else
    osbl_dload_entry( bl_shared_data );
    #endif
  }
else
 {
    
#if defined(FEATURE_QHSUSB_FAST_ENUMERATION) && defined(FEATURE_QHSUSB) 
 do_fast_enum();
#endif

    } /* osbl_dload_check() */
}
#ifdef FEATURE_DLOAD_SECURITY
/*===========================================================================

FUNCTION  OSBL_DLOAD_AUTH_FLASH_PROG

DESCRIPTION
  This function will authenticate the flash programmer downloaded by
  the QPST if security is enabled.  The image pointer argument points
  to the start of the signed image header that was downloaded to RAM
  by the QPST.

DEPENDENCIES

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

