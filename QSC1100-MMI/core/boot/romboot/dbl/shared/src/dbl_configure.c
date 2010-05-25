/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_configure.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "dbl_configure.h"
#include "dbl_error_handler.h"
#include "dbl_mc.h"
#include "dbl_target_accessor.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

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
const boot_clk_speed_type* dbl_configure_hardware_at_high_speed
(
  const boot_clk_speed_type *config_hw_speed
)
{
  return dbl_configure_hw_at_high_speed(config_hw_speed);
}

/*===========================================================================

FUNCTION  DBL_CONFIGURE_HARDWARE

DESCRIPTION
  This function will configure the hardware.
   1. Configure the clock.
   2. Configure EBI2.
   3. Configure EBI1.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_configure_hardware
(
  dbl_shared_data_type  *shared_data
)
{
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);
  shared_data->dbl_state = DBL_STATE_CONFIGURE_HW;
  shared_data->clk_speed = dbl_configure_hw_at_low_speed(shared_data->clk_speed);
} /* dbl_configure_hardware */

