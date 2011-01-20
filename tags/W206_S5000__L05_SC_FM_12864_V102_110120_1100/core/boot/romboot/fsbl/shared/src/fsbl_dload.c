/*===========================================================================

                     F A I L  S A F E  B o o t  L o a d e r
                        D o w n l o a d e r  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the FSBL downloader module, and declarations for the public functions
  in the module.

EXTERNALIZED FUNCTIONS
  fsbl_dload_init
  sbl_dload_auth_flash_prog

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_dload.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/30/07   ANB     Initial version.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "fsbl_dload.h"
#include "fsbl_auth_x509.h"
#include "fsbl.h"
#include "boot_msm.h"
#include "fsbl_hw.h"

#ifdef FEATURE_DLOAD_SECURITY
#include "miheader.h"
#endif /* FEATURE_DLOAD_SECURITY */

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

typedef struct boot_dload_type
{
  /*----------------------------------- 
   * Clients module interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure. */
  boot_dload_if_type  pub_if;
} fsbl_dload_type;

/* FSBL module private data structure */
static fsbl_dload_type fsbl_dload_priv;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION  FSBL_DLOAD_ENTRY

DESCRIPTION
  This function enters the downloader to support QPST download from the
  FSBL.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Enters the downloader and never returns.

=============================================================================*/
static boolean fsbl_dload_entry( void )
{
#ifdef FSBL_ENABLED_DLOAD
  /* Setup HW and enter dload mode...*/
  fsbl_hw_init_and_enter_dload();
#endif

  /* In case something drastic happens, loop here */
  fsbl_loop_here();

  return TRUE;

} /* fsbl_dload_entry() */


/*=========================================================================

FUNCTION  FSBL_DLOAD_INIT

DESCRIPTION
  Function to initialize an instance of the FSBL dload module.  

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the public hash interface.

SIDE EFFECTS
  None

===========================================================================*/
boot_dload_if_type * fsbl_dload_init(void)
{
  static const boot_module_version_type vsn =
  {
    FSBL_DLOAD_NAME,
    BOOT_MODULE_VERSION( BOOT_DLOAD_IF_VERSION, FSBL_DLOAD_VERSION )
  };
  static boot_dload_vtbl_type vtbl =
  {
    fsbl_dload_entry
  };

  /* Setup the module version structure and virtual function table */
  fsbl_dload_priv.pub_if.vsn = &vsn;
  fsbl_dload_priv.pub_if.vtbl = &vtbl;

  /* Return a pointer to the public interface */
  return &fsbl_dload_priv.pub_if;
}

#ifdef FEATURE_DLOAD_SECURITY
/*===========================================================================

FUNCTION  SBL_DLOAD_AUTH_FLASH_PROG

DESCRIPTION
  This function will authenticate the flash programmer downloaded by
  the QPST if security is enabled.  The image pointer argument points
  to the start of the signed image header that was downloaded to RAM
  by the QPST.
 
DEPENDENCIES
  FSBL must have been initialized by DBL
  
RETURN VALUE
  A pointer to the entry point of the flash programmer code image if
  the flash programmer authentication is successful, otherwise NULL if
  the authentication failed.  

SIDE EFFECTS
  None

===========================================================================*/
func_ptr_type sbl_dload_auth_flash_prog
( 
  mi_boot_image_header_type *image_ptr    /* Ptr to header at start of
                                             signed flash programmer image */
)
{
  boot_auth_if_type *auth_if = NULL;

  /* Verify the image pointer is not NULL. */
  if ( image_ptr == NULL )
  {
    return NULL;
  }

  /* Initialize (possibly re-initialize) the auth module */
  auth_if = fsbl_auth_x509_init();
  if ( auth_if == NULL )
  {
    return NULL;
  }

  /* Authenticate flash programmer if security is enabled on chip. */
  if ( auth_if->vtbl->authenticate( image_ptr ) )
  {
    /* Authentication was successful. */
    return (func_ptr_type) image_ptr->image_dest_ptr;
  }
  else
  {
    /* Authentication failed. */
    return NULL;
  }
} /* sbl_dload_auth_flash_prog() */
#endif /* FEATURE_DLOAD_SECURITY */
