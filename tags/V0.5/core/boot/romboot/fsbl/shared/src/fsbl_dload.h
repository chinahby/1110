#ifndef FSBL_DLOAD_H
#define FSBL_DLOAD_H

/*===========================================================================

                     F A I L  S A F E  B o o t  L o a d e r
                        D o w n l o a d e r  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the FSBL downloader module, and declarations for the public functions
  in the module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_dload.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_dload_if.h"

#ifdef FEATURE_DLOAD_SECURITY
#include "miheader.h"
#endif /* FEATURE_DLOAD_SECURITY */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define FSBL_DLOAD_NAME "fsbl_dload"
#define FSBL_DLOAD_VERSION 1

/* Pointer to a function that has no arguments and returns nothing. */
typedef void (*func_ptr_type)(void);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
extern boot_dload_if_type * fsbl_dload_init(void);

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
);
#endif /* FEATURE_DLOAD_SECURITY */


#endif /* FSBL_DLOAD_H */

