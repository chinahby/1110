#ifndef PM_VREG_TARGET_H
#define PM_VREG_TARGET_H

/*===========================================================================

                             Q S D  8 6 5 0
        V o l t a g e  R e g u l a t o r  I n i t i a l i z a t i o n
                          H e a d e r  F i l e

GENERAL DESCRIPTION
  This header file contains declarations and definitions for initializing
  and managing the PMIC voltage regulators.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/pm_vreg_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/27/07   MJS     Increase MSMC1 and MSMC2 to 1.3V for high speed Scorpion.
10/17/06   MJS     Removed RF vreg constants.
09/05/06    ly     Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "pm.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* USB Voltage setting = 3.3 Volts */
#define PM_VREG_USB_LEVEL         3300

#endif  /* PM_VREG_TARGET_H */

