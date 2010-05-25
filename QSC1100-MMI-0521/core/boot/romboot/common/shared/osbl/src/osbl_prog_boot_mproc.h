#ifndef OSBL_PROG_BOOT_MPROC_H
#define OSBL_PROG_BOOT_MPROC_H
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
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/osbl/osbl_prog_boot_mproc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/07   MJS     Ported from MDM7800.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "boot_comdef.h"
#include "miheader.h"
#include "boot_elf_loader_if.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  OSBL_PROG_BOOT_SHARE_AMSS_HEADER

DESCRIPTION
  This function saves the AMSS ELF program header to shared memory

DEPENDENCIES
  None

RETURN VALUE
  TRUE if initialization was successful and input parameters were valid,
  otherwise false.

SIDE EFFECTS
  The provided error handler may be called if an error is detected.

===========================================================================*/
extern void osbl_prog_boot_share_amss_header
( 
  boot_elf_loader_if_type *elf_if,
  image_type        image
);



#endif /* OSBL_PROG_BOOT_MPROC_H */


