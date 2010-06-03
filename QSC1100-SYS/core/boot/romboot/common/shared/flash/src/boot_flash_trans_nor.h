#ifndef BOOT_FLASH_TRANS_NOR_H
#define BOOT_FLASH_TRANS_NOR_H

/*===========================================================================

                           B o o t   L o a d e r
            N O R  F l a s h  T r a n s l a t i o n  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the boot-specific NOR flash translation module/layer, and declarations
  for the public functions in the module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_flash_trans_nor.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_flash_trans_if.h"

#ifdef BOOT_MSM_H
#include BOOT_MSM_H
#endif /* BOOT_MSM_H */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_TRANS_NOR_VERSION 1
#define BOOT_FLASH_TRANS_NOR_NAME "boot_flash_trans_nor"

#define BOOT_FLASH_TRANS_FACTORY_NOR_VERSION 1
#define BOOT_FLASH_TRANS_FACTORY_NOR_NAME "boot_flash_trans_nor_factory"


/*=========================================================================

MACRO  BFA_NOR_VERIFY

DESCRIPTION
  Macro to verify a condition and transition to an error state if the
  condition is not met.  Default error bahavior is to return FALSE to
  the caller.  This behavior can be over-ridden by re-defining the macro
  in BOOT_MSM_H.

DEPENDENCIES
  None
  
RETURN VALUE
  FALSE if the condition was not met.

SIDE EFFECTS
  None

===========================================================================*/
#ifndef BFA_NOR_VERIFY
#define BFA_NOR_VERIFY( cond ) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      return FALSE; \
    } \
  } while(0)
#endif /* BFA_NOR_VERIFY */

/*=========================================================================

MACRO  BFA_NOR_VERIFY2

DESCRIPTION
  Macro to verify a condition and transition to an error state if the
  condition is not met.  Default bahavior is to return the value "retval"
  given as a parameter to the caller.  This form of the macro is used
  by routines that do not return boolean.  The default behavior can
  be over-ridden by re-defining the macro in BOOT_MSM_H.

DEPENDENCIES
  None
  
RETURN VALUE
  The error value passed as parameter if the condition was not met.

SIDE EFFECTS
  None

===========================================================================*/
#ifndef BFA_NOR_VERIFY2
#define BFA_NOR_VERIFY2( cond, ret_val ) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      return (ret_val); \
    } \
  } while(0)
#endif /* BFA_NOR_VERIFY2 */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_NOR_FACTORY_INIT

DESCRIPTION
  Initializes the NOR boot translation factory.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the NOR translation factory interface

SIDE EFFECTS
  None

===========================================================================*/
extern boot_flash_trans_factory_if_type* boot_flash_trans_nor_factory_init
(
  void
);

#endif /* BOOT_FLASH_TRANS_NOR_H */

