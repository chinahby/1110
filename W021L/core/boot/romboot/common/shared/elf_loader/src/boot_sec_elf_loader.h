#ifndef BOOT_SEC_ELF_LOADER_H
#define BOOT_SEC_ELF_LOADER_H

/*===========================================================================

                           B o o t   L o a d e r
                      E L F  L o a d e r  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definitions and declarations for the public
  functions in the boot-specific ELF loader module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/elf_loader/boot_sec_elf_loader.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "miprogressive.h"
#include "boot_flash_trans_if.h"
#include "boot_elf_loader.h"
#include "boot_sec_elf_loader_if.h"

#ifdef BOOT_MSM_H
#include BOOT_MSM_H
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_SEC_ELF_LOADER_VERSION 1
#define BOOT_SEC_ELF_LOADER_NAME "boot_sec_elf_loader"

#define BOOT_SEC_ELF_LOADER_FACTORY_VERSION 1
#define BOOT_SEC_ELF_LOADER_FACTORY_NAME "boot_sec_elf_loader_factory"


/*=========================================================================

MACRO  BOOT_SEC_ELF_VERIFY

DESCRIPTION
  Macro to verify a condition and transition to an error state if the
  condition is not met.  Default bahavior is to set the last_error field
  in the private elf loader data structure and return that value to the
  caller.  This behavior can be over-ridden by re-defining the macro
  in BOOT_MSM_H.

DEPENDENCIES
  None
  
RETURN VALUE
  The error that occurred if the condition was not met.

SIDE EFFECTS
  None

===========================================================================*/
#ifndef BOOT_SEC_ELF_VERIFY
#define BOOT_SEC_ELF_VERIFY(cond, err) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      return ( elf_priv->last_error = (err) ); \
    } \
  } while(0)
#endif /* BOOT_SEC_ELF_VERIFY */

/*=========================================================================

MACRO  BOOT_SEC_ELF_VERIFY2

DESCRIPTION
  Macro to verify a condition and transition to an error state if the
  condition is not met.  Default bahavior is to set the last_error field
  in the private elf loader data structure and return the value "retval"
  given as a parameter to the caller.  This form of the macro is used
  by elf loader routines that do not return boot_sec_elf_error_type.  The
  default behavior can be over-ridden by re-defining the macro in BOOT_MSM_H.

DEPENDENCIES
  None
  
RETURN VALUE
  The error value passed as parameter if the condition was not met.

SIDE EFFECTS
  None

===========================================================================*/
#ifndef BOOT_SEC_ELF_VERIFY2
#define BOOT_SEC_ELF_VERIFY2(cond, err, retval) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      elf_priv->last_error = (err); \
      return retval; \
    } \
  } while(0)
#endif /* BOOT_SEC_ELF_VERIFY2 */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*=========================================================================

FUNCTION  BOOT_SEC_ELF_LOADER_FACTORY_INIT_AND_REGISTER

DESCRIPTION
  Initializes the secure boot ELF loader factory and registers the singleton.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void boot_sec_elf_loader_factory_init_and_register( void );

#endif /* BOOT_SEC_ELF_LOADER_H */

