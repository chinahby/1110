#ifndef BOOT_DLOAD_IF_H
#define BOOT_DLOAD_IF_H

/*===========================================================================

                    B o o t   D L O A D   L o a d e r
                     M o d u l e  I n t e r f a c e
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to
  the boot DLOAD module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_dload_if.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/31/07   anb     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_module_base.h"
#include "boot_singleton.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_DLOAD_IF_VERSION 1


/* Definition of the module virtual function table */
typedef struct boot_dload_vtbl_type
{
  /*=========================================================================

  FUNCTION POINTER  ENTER_DLOAD

  DESCRIPTION
    This function enters the downloader to support QPST download.

  DEPENDENCIES
    None
  
  RETURN VALUE
    FALSE if cannot dload mode.

  SIDE EFFECTS
    Enters the downloader and never returns.

  ===========================================================================*/
  boolean (*enter_dload)
  (
    void
  );  
} boot_dload_vtbl_type;


/* Definition of public interface to download module */
typedef struct boot_dload_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_dload_vtbl_type *vtbl;

} boot_dload_if_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Declare the download singleton */
BOOT_SINGLETON_DECLARE(boot_dload_if);

/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_DLOAD_ENTER_DLOAD

DESCRIPTION
  This function enters the downloader to support QPST download.

DEPENDENCIES
  None
  
RETURN VALUE
  FALSE if cannot dload mode.

SIDE EFFECTS
  Enters the downloader and never returns.

===========================================================================*/
__inline boolean boot_dload_enter_dload( void )
{
  return boot_dload_if_get_singleton()->vtbl->enter_dload();
} /* boot_dload_enter_dload() */

#endif /* BOOT_DLOAD_IF_H */

