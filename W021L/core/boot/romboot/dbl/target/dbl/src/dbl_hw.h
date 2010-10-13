#ifndef DBL_HW_H
#define DBL_HW_H

/*=============================================================================

                      D e v i c e  B o o t  L o a d e r
                            H e a d e r  F i l e
GENERAL DESCRIPTION
  This file provides an interface to device bootloader to set and get the
  root certificate.
  
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/dbl_hw.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_hw.h"
#include "boot_clk_speed.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
extern void dbl_hw_clk_busy_wait(uint32 us);

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
extern void dbl_hw_set_configured_clock_speed(const boot_clk_speed_type *clock_speed);

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
extern const boot_clk_speed_type* dbl_hw_get_configured_clock_speed(void);

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
extern boolean dbl_hw_is_high_speed_dload_supported(void);

#endif  /* DBL_HW_H */

