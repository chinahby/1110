/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION
  This file provides the minimal hardware setup required for the PBL to
  execute.
  
EXTERNALIZED FUNCTIONS
  dbl_hw_is_auth_enabled
  dbl_hw_get_msm_hw_id
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/dbl_hw.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "dbl_hw.h"
#include "boot_msm.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*---------------------------------------------------------------------------
  Local Variable to remember the clock speed.
---------------------------------------------------------------------------*/
static uint32 dbl_hw_mahb_clk_speed_mhz = 20;
static const boot_clk_speed_type *dbl_hw_clk_speed = NULL;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  DBL_HW_CLK_BUSY_WAIT

DESCRIPTION
  This function provides a busy wait for the caller.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_hw_clk_busy_wait(uint32 us)
{
  uint32 wait_remaining_cycle =0;/* how many cycles we have to wait*/
  uint32 pause_cycle=0;/* max is 65535. unit is clock cycle*/

  /* How many cycles we should stall? */
  wait_remaining_cycle = dbl_hw_mahb_clk_speed_mhz * us;

  /* Each penalty of write is 13 cycles */

  while (wait_remaining_cycle >= 13)
  {
    /* Max of PAUSE TIME stall time is 65535 cycles */
    pause_cycle = MIN(65535, (wait_remaining_cycle-13));
    HWIO_OUT (PAUSE_TIMER, pause_cycle);
    wait_remaining_cycle -= (pause_cycle + 13);
  }
}

/*===========================================================================

FUNCTION  DBL_HW_SET_CONFIGURED_CLOCK_SPEED

DESCRIPTION
  This function needs to be called by Clock Configuration routine so 
  that the local variable can be updated with the correct settings.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_hw_set_configured_clock_speed(const boot_clk_speed_type *clock_speed)
{
  if ( clock_speed != NULL )
  {
    dbl_hw_clk_speed = clock_speed;
  }
}

/*===========================================================================

FUNCTION  DBL_HW_GET_CONFIGURED_CLOCK_SPEED

DESCRIPTION
  This function will return configured clock speed.

DEPENDENCIES
  None

RETURN VALUE
  uint32 configured clock speed in MHz

SIDE EFFECTS
  None

===========================================================================*/
const boot_clk_speed_type *dbl_hw_get_configured_clock_speed(void)
{
  return dbl_hw_clk_speed;
}

/*===========================================================================

FUNCTION  DBL_HW_IS_HIGH_SPEED_DLOAD_SUPPORTED

DESCRIPTION
  Even though FSBL might support high speed DLOAD mode the HW might not allow
  DBL to use this funtionality. FSBL is a library and it shares the stack of
  the calling SBL (DBL and OSBL).
  
  For example:
  -----------
  In targets where ADSP memory is use as IMEM... bytes access to ADSP memory
  is not allowed. PMIC Library and SBI library are doing lot of byte access
  and hence DBL cannot use this method to enter into DLOAD mode.  

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if High Speed DLOAD Supported
  FALSE if High Speed DLOAD Supported

SIDE EFFECTS
  None

===========================================================================*/
boolean dbl_hw_is_high_speed_dload_supported(void)
{
  /* ULC is using IMEM Boot-up. */
  return TRUE;
}

