#ifndef BOOT_FSBL_CONFIG_IF_H
#define BOOT_FSBL_CONFIG_IF_H

/*===========================================================================

                B o o t   F S B L   C o n f i g u r a t i o n
                     M o d u l e  I n t e r f a c e
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to
  FSBL configuration setting.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_fsbl_config_if.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/31/07   anb     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_clk_speed.h"
#include "boot_module_base.h"
#include "boot_singleton.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FSBL_CONFIG_IF_VERSION 1


/* Definition of the download module virtual function table */
typedef struct boot_fsbl_config_vtbl_type
{
  /*=========================================================================

  FUNCTION POINTER  SET_CLOCK_SPEED

  DESCRIPTION
    This function needs to be called inorder to let FSBL know the clock speed
    at which the system is running.

  DEPENDENCIES
    None
  
  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void (*set_clock_speed)
  (
    const boot_clk_speed_type *clock_speed  /* Clock at which system is running */
  );  

  /*=========================================================================

  FUNCTION POINTER  SET_ROOT_CERT_PTR

  DESCRIPTION
    This function needs to be called inorder to let FSBL know the location of
    the root certificate.

  DEPENDENCIES
    None
  
  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void (*set_root_cert_ptr)
  (
    void *root_cert_ptr /* Pointer to root cert location   */
  );  
} boot_fsbl_config_vtbl_type;


/* Definition of public interface to download module */
typedef struct boot_fsbl_config_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_fsbl_config_vtbl_type *vtbl;

} boot_fsbl_config_if_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Declare the singleton, do it here so we can access the singleton below */
BOOT_SINGLETON_DECLARE(boot_fsbl_config_if);

/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_FSBL_CONFIG_SET_CLOCK_SPEED

DESCRIPTION
  This function needs to be called inorder to let FSBL know the clock speed
  at which the system is running.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline void boot_fsbl_config_set_clock_speed
(
  const boot_clk_speed_type *clock_speed
)
{
  boot_fsbl_config_if_get_singleton()->vtbl->set_clock_speed( clock_speed );
} /* boot_fsbl_config_set_clock_speed() */

/*=========================================================================

FUNCTION  SET_ROOT_CERT_PTR

DESCRIPTION
  This function needs to be called inorder to let FSBL know the location of
  the root certificate.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline void boot_fsbl_config_set_root_cert_ptr( void *root_cert_ptr )
{
  boot_fsbl_config_if_get_singleton()->vtbl->set_root_cert_ptr( root_cert_ptr );
} /* boot_fsbl_config_set_root_cert_ptr() */

#endif /* BOOT_FSBL_CONFIG_IF_H */

