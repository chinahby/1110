#ifndef OSBL_LOADER_H
#define OSBL_LOADER_H
/*===========================================================================

               OS  S e c o n d a r y   B o o t   L o a d e r
                               F l a s h 
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the QC SBL
  software that determines the type of flash we are booting from (NAND,
  NOR, or SUPERAND).  Once the flash type is known, additional parameters
  for the particular flash type are determined and then the flash is
  configured for access based on this information.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007, 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_loader.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/07/08   MJS     Split AMSS authentication and loading routines.
01/04/08   MJS     Initial revision.
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "miheader.h"
#include "boot_elf_loader_if.h"
#include "boot_flash_dev_if.h"
#include "boot_auth_if.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  OSBL_LOAD_HEADER_AND_IMAGE

DESCRIPTION
  This function loads in the specified image and image header from 
  the boot device to RAM.  The header_ptr parameter is optional, if
  not NULL it will be filled in with the image header data.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void osbl_load_header_and_image
(
  image_type image_id,
  mi_boot_image_header_type *header_ptr      /* image header      */
);

/*===========================================================================

FUNCTION OSBL_INIT_AMSS_IMAGE

DESCRIPTION
  
DEPENDENCIES
  
RETURN VALUE
  
SIDE EFFECTS

===========================================================================*/
extern void osbl_init_amss_image( bl_shared_data_type *bl_shared_data );

/*===========================================================================

FUNCTION OSBL_PROCESS_AMSS_DEBUG_FLAG

DESCRIPTION
  
DEPENDENCIES
  
RETURN VALUE
  
SIDE EFFECTS

===========================================================================*/
extern void osbl_process_amss_debug_flag( bl_shared_data_type *bl_shared_data );

/*===========================================================================

FUNCTION OSBL_AUTH_AMSS_IMAGE

DESCRIPTION
  This function authenticates the AMSS image if authentication is enabled.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  This function calls the error handler and does not return if a
  failure occurs.

==========================================================================*/
extern void osbl_auth_amss_image( bl_shared_data_type *bl_shared_data );

/*===========================================================================

FUNCTION OSBL_LOAD_AMSS_IMAGE

DESCRIPTION
  This function loads the non-paged segments of the amss image and retrieves
  the image entry point.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  This function calls the error handler and does not return if a
  failure occurs.

===========================================================================*/
extern void osbl_load_amss_image( bl_shared_data_type *bl_shared_data );

#endif /* OSBL_LOADER_H */


