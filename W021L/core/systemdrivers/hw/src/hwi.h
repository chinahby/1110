#ifndef HWI_H
#define HWI_H

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

  $Header: //depot/asic/qsc1100/drivers/hw/hwi.h#1 $ $DateTime: 2007/05/17 13:25:59 $ $Author: hvo $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/06   gmy     SC2x now drops PS_HOLD enabling power-off to complete
11/17/05   jac     added hw_generate_watchdog_reset and hw_enable_boot_ram
01/31/05   sr      First version

===========================================================================*/
#include "comdef.h"             /* Definitions for byte, word, etc.     */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* PS_HOLD Control */
#define HW_PS_HOLD_ENABLE   (TRUE)
#define HW_PS_HOLD_DISABLE  (FALSE)

/*===========================================================================

FUNCTION hw_generate_watchdog_reset
  None


DESCRIPTION
  Generates a watchdog reset

===========================================================================*/
void hw_generate_watchdog_reset( void );

/*===========================================================================

FUNCTION hw_enable_boot_ram
  None


DESCRIPTION
  Enable the boot RAM for the sleep code

===========================================================================*/
void hw_enable_boot_ram( void );

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
extern void hw_set_ps_hold( boolean bState );

#endif
