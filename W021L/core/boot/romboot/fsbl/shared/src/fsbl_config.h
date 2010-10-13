#ifndef FSBL_CONFIG_H
#define FSBL_CONFIG_H

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

$Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_config.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_fsbl_config_if.h"
#include "boot_clk_speed.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define FSBL_CONFIG_VERSION 1
#define FSBL_CONFIG_NAME "fsbl_config"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

FUNCTION FSBL_CONFIG_GET_CLOCK_SPEED

DESCRIPTION
  This function will return the set clock speed in MHz.

DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern const boot_clk_speed_type* fsbl_config_get_clock_speed
(
  void
);

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
extern const void* fsbl_config_get_root_cert
(
  void
);

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
extern boot_fsbl_config_if_type * fsbl_config_init(void);

#endif /* FSBL_CONFIG_H */

