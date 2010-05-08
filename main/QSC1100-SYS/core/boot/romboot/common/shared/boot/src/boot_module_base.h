#ifndef BOOT_MODULE_BASE_H
#define BOOT_MODULE_BASE_H

/*===========================================================================

                          B o o t  L o a d e r
                B a s e  M o d u l e  I n t e r f a c e
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the base module interface.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  BOOT_MODULE_BUILD_VERSION must be defined by a makefile.
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_module_base.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/* Constructs a uint32 with the module revision number */
#define BOOT_MODULE_VERSION( major, minor ) \
  ( ( ( (major) & 0xFF ) << 24 ) | \
    ( ( (minor) & 0xFF ) << 16 ) | \
    ( (BOOT_MODULE_BUILD_VERSION) & 0xFFFF ) )


/* The definition of the module versioning data structure */
typedef struct boot_module_version_type
{
  /* Pointer to a string representing the module name */
  const char *name;

  /* A uint32 representing the module version, contains a major, minor and
   * release number similar to the HAL version number */
  uint32 ver;

} boot_module_version_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


#endif /* BOOT_MODULE_BASE_H */

