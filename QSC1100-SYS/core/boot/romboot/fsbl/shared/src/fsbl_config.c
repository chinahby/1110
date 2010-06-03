/*===========================================================================


                B o o t   F S B L   C o n f i g u r a t i o n
                     M o d u l e  I n t e r f a c e
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to
  FSBL configuration setting.

EXTERNALIZED FUNCTIONS
  fsbl_config_get_clock_speed
  fsbl_config_get_root_cert
  fsbl_config_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.

============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_config.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/30/07   ANB     Initial version.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "fsbl_config.h"
#include "fsbl.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

typedef struct fsbl_config_type
{
  /*----------------------------------- 
   * Clients module interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure. */
  boot_fsbl_config_if_type    pub_if;

  const boot_clk_speed_type  *clock_speed;
  const void*                 root_cert_ptr;
}fsbl_config_type;


/* FSBL configuration module private data structure */
static fsbl_config_type fsbl_config_priv;

/* OEM's Root Certificate */
extern const uint8 oem_root_x509_cert[];

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*=========================================================================

FUNCTION FSBL_CONFIG_SET_CLOCK_SPEED

DESCRIPTION
  This function needs to be called inorder to let FSBL know the clock speed
  at which the system is running.

DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void fsbl_config_set_clock_speed
(
  const boot_clk_speed_type *clock_speed  /* Clock at which system is running */
)
{
  if ( clock_speed != NULL )
  {
    fsbl_config_priv.clock_speed = clock_speed;
  }
}

/*=========================================================================

FUNCTION FSBL_CONFIG_SET_ROOT_CERT_PTR

DESCRIPTION
  This function needs to be called inorder to let FSBL know the location of
  the root certificate.

DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void fsbl_config_set_root_cert_ptr
(
  void *root_cert_ptr /* Pointer to root cert location   */
)
{
  if ( root_cert_ptr != NULL )
  {
    fsbl_config_priv.root_cert_ptr = root_cert_ptr;
  }
}

/*=========================================================================

FUNCTION FSBL_CONFIG_GET_CLOCK_SPEED

DESCRIPTION
  This function will return the set clock speed.

DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
const boot_clk_speed_type* fsbl_config_get_clock_speed
(
  void
)
{
  return (fsbl_config_priv.clock_speed);
}

/*=========================================================================

FUNCTION FSBL_CONFIG_GET_ROOT_CERT

DESCRIPTION
  This function will return the set root certificate.

DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
const void* fsbl_config_get_root_cert
(
  void
)
{
  return (fsbl_config_priv.root_cert_ptr);
}

/*=========================================================================

FUNCTION  FSBL_CONFIG_INIT

DESCRIPTION
  Function to initialize an instance of the FSBL configuration module.  

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the public hash interface.

SIDE EFFECTS
  None

===========================================================================*/
boot_fsbl_config_if_type * fsbl_config_init(void)
{
  static const boot_module_version_type vsn =
  {
    FSBL_CONFIG_NAME,
    BOOT_MODULE_VERSION( BOOT_FSBL_CONFIG_IF_VERSION, FSBL_CONFIG_VERSION )
  };
  static boot_fsbl_config_vtbl_type vtbl =
  {
    fsbl_config_set_clock_speed,
    fsbl_config_set_root_cert_ptr
  };

  /* Setup the module version info and virtual function table */
  fsbl_config_priv.pub_if.vsn = &vsn;
  fsbl_config_priv.pub_if.vtbl = &vtbl;

  /* Return a pointer to the public interface */
  return &fsbl_config_priv.pub_if;
}

