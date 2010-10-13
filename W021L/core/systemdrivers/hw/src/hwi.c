/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              H A R D W A R E   U T I L I T I E S

GENERAL DESCRIPTION
  This module contains hardware specific utilites that use Private registers.


Copyright (c) 1992-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6050/vcs/hw.c_v   1.6   13 Mar 2002 07:22:54   dlee  $
  $Header: //depot/asic/qsc1100/drivers/hw/hwi.c#2 $ $DateTime: 2008/02/20 15:22:37 $ $Author: bcalder $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/06   gmy     SC2x now drops PS_HOLD enabling power-off to complete
11/17/05   jac     added hw_generate_watchdog_reset and hw_enable_boot_ram
12/14/04   sr      First version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "msm.h"                /* Registers for HW/SW version          */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* WDOG timer count until WDOG expires in 32KHz sleep clocks. */
#define WDOG_COUNT  1000

/*===========================================================================

FUNCTION hw_generate_watchdog_reset
  None


DESCRIPTION
  Generates a watchdog reset

===========================================================================*/
void hw_generate_watchdog_reset( void )
{
  /* Set watchdog counter to a known value so it expires soon. */
  HWIO_OUT( WDOG_TEST, WDOG_COUNT );
  HWIO_OUT( WDOG_TEST_LOAD,
            HWIO_FMSK( WDOG_TEST_LOAD, LOAD ) );

  /* Wait for watchdog to expire */
  for( ;; )
  {
    /*loop forever until watchdog expire*/
  };
}

/*===========================================================================

FUNCTION hw_enable_boot_ram
  None


DESCRIPTION
  Enable the boot RAM for the sleep code

===========================================================================*/
void hw_enable_boot_ram( void )
{
  /* ----------------------------------------------------------------------
   Enable the boot RAM for the sleep code
  ---------------------------------------------------------------------- */
  HWIO_OUTF( ARM_CLK_EN_REG, IMEM_CLK_SRC_EN, 1 );
}


/*===========================================================================

FUNCTION HW_SET_PS_HOLD

DESCRIPTION
  This function sets the state of the PS_HOLD line to the PMIC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hw_set_ps_hold( boolean bState )
{
   if (bState)
   {
      /* Turn on PS_HOLD. Prevents PMIC from dropping power to the phone */
      HWIO_PMIC_CONTROL_OUTM((HWIO_PMIC_CONTROL_PS_HOLD_BMSK |         \
                                 HWIO_PMIC_CONTROL_PS_HOLD_OE_BMSK),   \
                             (HWIO_PMIC_CONTROL_PS_HOLD_BMSK |         \
                                 HWIO_PMIC_CONTROL_PS_HOLD_OE_BMSK));
   }
   else
   {
      /* Turn off PS_HOLD. Commands PMIC to perfom power-down sequence */
      HWIO_PMIC_CONTROL_OUTM((HWIO_PMIC_CONTROL_PS_HOLD_BMSK |         \
                                 HWIO_PMIC_CONTROL_PS_HOLD_OE_BMSK),   \
                             HWIO_PMIC_CONTROL_PS_HOLD_OE_BMSK);
   }
} /* end of hw_set_ps_hold */


