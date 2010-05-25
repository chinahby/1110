#ifndef BOOT_ELF_LOADER_H
#define BOOT_ELF_LOADER_H

/*===========================================================================

               G e n e r i c  E L F  L o a d e r  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definitions and declarations for the public
  functions in the ELF loader module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/elf_loader/boot_elf_loader.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_elf_loader_if.h"

#ifdef BOOT_MSM_H
#include BOOT_MSM_H
#endif /* BOOT_MSM_H */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_ELF_LOADER_VERSION 1
#define BOOT_ELF_LOADER_NAME "boot_elf_loader"

#define BOOT_ELF_LOADER_FACTORY_VERSION 1
#define BOOT_ELF_LOADER_FACTORY_NAME "boot_elf_loader_factory"


/*=========================================================================

MACRO  BOOT_ELF_VERIFY

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
#ifndef BOOT_ELF_VERIFY
#define BOOT_ELF_VERIFY(cond, err) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      return ( elf_ptr->last_error = (err) ); \
    } \
  } while(0)
#endif /* BOOT_ELF_VERIFY */

/*=========================================================================

MACRO  BOOT_ELF_VERIFY2

DESCRIPTION
  Macro to verify a condition and transition to an error state if the
  condition is not met.  Default bahavior is to set the last_error field
  in the private elf loader data structure and return the value "retval"
  given as a parameter to the caller.  This form of the macro is used
  by elf loader routines that do not return boot_elf_error_type.  The
  default behavior can be over-ridden by re-defining the macro in BOOT_MSM_H.

DEPENDENCIES
  None
  
RETURN VALUE
  The error value passed as parameter if the condition was not met.

SIDE EFFECTS
  None

===========================================================================*/
#ifndef BOOT_ELF_VERIFY2
#define BOOT_ELF_VERIFY2(cond, err, retval) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      elf_ptr->last_error = (err); \
      return retval; \
    } \
  } while(0)
#endif /* BOOT_ELF_VERIFY2 */

/*=========================================================================

MACRO  BOOT_ELF_BSS_INIT

DESCRIPTION
  Macro to initialize a region of BSS in an elf segment.  The default
  implementation is to use memset() to zero out the memory, but this
  behavior can be over-ridden by re-defining the macro in BOOT_MSM_H.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifndef BOOT_ELF_BSS_INIT
#define BOOT_ELF_BSS_INIT( bss_offset, bss_len) \
  memset( (void*) (bss_offset), 0, (bss_len) )
#endif /* BOOT_ELF_BSS_INIT */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*=========================================================================

FUNCTION  BOOT_ELF_LOADER_FACTORY_INIT_AND_REGISTER

DESCRIPTION
  Initializes the boot ELF loader factory and registers the singleton.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void boot_elf_loader_factory_init_and_register( void );

#endif /* BOOT_ELF_LOADER_H */

