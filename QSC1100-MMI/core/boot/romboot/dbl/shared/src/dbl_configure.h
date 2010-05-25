#ifndef DBL_CONFIGURE_H
#define DBL_CONFIGURE_H

/*=============================================================================

                      D e v i c e  B o o t  L o a d e r
                            H e a d e r  F i l e
GENERAL DESCRIPTION
  This file provides an interface to device bootloader to configure the
  hardware.
  
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_configure.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "dbl.h"
#include "boot_clk_speed.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  DBL_CONFIGURE_HARDWARE_AT_HIGH_SPEED

DESCRIPTION
  This function will configure the hardware.
   1. Configure the clock.
   2. Configure EBI1.

DEPENDENCIES
  This function is in IMEM so in-order to configure the hardware it's caller's
  responsibility to have the stack in IMEM and there is no access to DDR
  when this function is called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern const boot_clk_speed_type* dbl_configure_hardware_at_high_speed
(
  const boot_clk_speed_type *config_hw_speed
);

/*===========================================================================

FUNCTION  DBL_CONFIGURE_HARDWARE

DESCRIPTION
  This function will configure the hardware.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_configure_hardware
(
  dbl_shared_data_type   *shared_data
);

#endif  /* DBL_CONFIGURE_H */

