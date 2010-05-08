#ifndef STROBE_FLASH_H
#define STROBE_FLASH_H

/*============================================================================

                 Strobe Flash Device Driver header file

   DESCRIPTION
     This file contains the declarations needed for the strobe
     device driver.

   Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/flashdriver/main/latest/inc/strobe_flash.h#1 $ $DateTime: 2008/05/09 16:34:47 $ $Author: karenp $

 when      who    what, where, why
 --------  ---    -------------------------------------------------------
 10/18/04  lrw    Add strobe_flash_redeye_reduction()
 09/08/04  lrw    Adopted from MSM6250 to MSM6500 for Initial release. 

============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/
#include "comdef.h"

/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/

/*============================================================================
                        DATA DECLARATIONS
============================================================================*/

/*============================================================================
                          MACRO DEFINITIONS
============================================================================*/

/*============================================================================
                          EXPORTED FUNCTIONS
============================================================================*/


/*===========================================================================

FUNCTION      STROBE_FLASH_INIT

DESCRIPTION
              OEM implements this function. Initializes the strobe flash
              control and puts the device in powered down and off state.

DEPENDENCIES
  None

RETURN VALUE
              TRUE - strobe device is available.
	      FALSE - strobe device is absent.

SIDE EFFECTS
  None

===========================================================================*/

boolean strobe_flash_init (void);

/*===========================================================================

FUNCTION      STROBE_FLASH_START_CHARGING

DESCRIPTION
              OEM implements this function. Starts charging the strobe device.

DEPENDENCIES
  None

RETURN VALUE
              TRUE - charging of strobe device started.
	      FALSE - charging of strobe device cannot be started.

SIDE EFFECTS
  None

===========================================================================*/
boolean strobe_flash_start_charging(void);


/*===========================================================================

FUNCTION      STROBE_FLASH_START_CHARGING

DESCRIPTION
              OEM implements this function. 

DEPENDENCIES
  None

RETURN VALUE
              TRUE - charging of strobe device started. 
	      FALSE -  charging of strobe device cannot be started.

SIDE EFFECTS
  None

===========================================================================*/
boolean strobe_flash_stop_charging(void);


/*===========================================================================

FUNCTION      STROBE_FLASH_STOP_CHARGING

DESCRIPTION
              OEM implements this function. 

DEPENDENCIES
  None

RETURN VALUE
              TRUE - charging of strobe device stopped. 
	      FALSE - charging of strobe device cannot be stopped.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean strobe_flash_is_ready(void);


/*===========================================================================

FUNCTION      STROBE_FLASH_START_TRIGGER

DESCRIPTION
              OEM implements this function. This function is executed
	      in interrupt context. 

DEPENDENCIES
  None

RETURN VALUE
              TRUE - strobe flash is triggered to fire.
	      FALSE - strobe flash is not triggered to fire.

SIDE EFFECTS
  This function must not call any function that does not support interrupt
  context. Must minimize the execution and return as soon as possible.

===========================================================================*/
boolean strobe_flash_start_trigger(void);

/*===========================================================================

FUNCTION      STROBE_FLASH_STOP_TRIGGER

DESCRIPTION
              OEM implements this function. This function is executed
	      in interrupt context. 

DEPENDENCIES
  None

RETURN VALUE
              TRUE - strobe flash trigger isgnal is disabled.
	      FALSE - strobe flash trigger signal can not be disabled.

SIDE EFFECTS
  This function must not call any function that does not support interrupt
  context. Must minimize the execution and return as soon as possible.

===========================================================================*/
boolean strobe_flash_stop_trigger(void);

/*===========================================================================

FUNCTION      STROBE_FLASH_REDEYE_REDUCTION

DESCRIPTION
              OEM implements this function. This function is not executed
	      in interrupt context. 

DEPENDENCIES
  None

RETURN VALUE
              VOID

SIDE EFFECTS
  This function must not call any function that does not support interrupt
  context. Must minimize the execution and return as soon as possible.

===========================================================================*/
void strobe_flash_redeye_reduction(void);


#endif /* STROBE_FLASH_H */


