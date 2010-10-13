/*============================================================================

                 Strobe Flash Device Driver source file

   DESCRIPTION
     This file contains the definitions needed for the strobe
     device driver.

   Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/flashdriver/main/latest/src/strobe_flash.c#1 $ $DateTime: 2008/05/09 16:34:47 $ $Author: karenp $

 when      who    what, where, why
 --------  ---    -------------------------------------------------------
 10/21/05  dl     Resolve lint issues.
 10/18/04  lrw    Add strobe_flash_redeye_reduction().
 09/08/04  lrw    Adopted from MSM6250 to MSM6500 for Initial release. 

============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "strobe_flash.h"
#include "clk.h"
#include "camera.h"
/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/

#ifdef FEATURE_STROBE_FLASH
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

boolean strobe_flash_init (void)
{
  /* re-configure GPIO 29 and 17 for strobe flash */
  gpio_tlmm_config(GPIO_OUT_29);
//  gpio_tlmm_config(GPIO_OUT_17);

  strobe_flash_stop_charging();
  strobe_flash_stop_trigger(); 
  return TRUE;
}

#endif /* FEATURE_STROBE_FLASH*/

#ifdef FEATURE_STROBE_FLASH

/*===========================================================================

FUNCTION      STROBE_FLASH_START_CHARGING

DESCRIPTION
              OEM implements this function. Starts charging the strobe device.
	      This function can be changed to install a callback if needed.

DEPENDENCIES
  None

RETURN VALUE
              TRUE - charging of strobe device started.
	      FALSE - charging of strobe device cannot be started.

SIDE EFFECTS
  None

===========================================================================*/

boolean strobe_flash_start_charging(void)
{
      /* Configure as output  */
      BIO_TRISTATE(BIO_GPIO_29_REG, BIO_GPIO_29_M, BIO_OUTPUT_ENA_V);
  
      /* output high to GPIO 29 - flash charge to start charging */
      BIO_TRISTATE(BIO_GPIO_29_REG, BIO_GPIO_29_M, BIO_GPIO_29_M);
      BIO_OUT(BIO_GPIO_29_REG, BIO_GPIO_29_M, BIO_GPIO_29_HI_V);

  return TRUE;
}
#endif /* FEATURE_STROBE_FLASH*/

#ifdef FEATURE_STROBE_FLASH
/*===========================================================================

FUNCTION      STROBE_FLASH_STOP_CHARGING

DESCRIPTION
              OEM implements this function. Stops charging the strobe device.
	      This function can be changed to install a callback if needed.

DEPENDENCIES
  None

RETURN VALUE
        TRUE - charging of strobe device stopped.
	      FALSE - charging of strobe device cannot be stopped.

SIDE EFFECTS
  None

===========================================================================*/

boolean strobe_flash_stop_charging(void)
{
      /* Configure as output  */
      BIO_TRISTATE(BIO_GPIO_29_REG, BIO_GPIO_29_M, BIO_OUTPUT_ENA_V);
  
      /* output low to GPIO 29 - flash charge to stop charging */
      BIO_TRISTATE(BIO_GPIO_29_REG, BIO_GPIO_29_M, BIO_GPIO_29_M);
      BIO_OUT(BIO_GPIO_29_REG, BIO_GPIO_29_M, BIO_GPIO_29_LO_V);

  return TRUE;
}
#endif /* FEATURE_STROBE_FLASH*/

#ifdef FEATURE_STROBE_FLASH
/*===========================================================================

FUNCTION      STROBE_FLASH_READY

DESCRIPTION
              OEM implements this function. 

DEPENDENCIES
  None

RETURN VALUE
              TRUE - strobe device is fully charged and ready to be fired. 
	      FALSE - strobe device is not fully charged and cannot be fired.

SIDE EFFECTS
  None

===========================================================================*/

boolean strobe_flash_is_ready(void)
{

  return TRUE;
}
#endif /* FEATURE_STROBE_FLASH*/

#ifdef FEATURE_STROBE_FLASH
/*===========================================================================

FUNCTION      STROBE_FLASH_START_TRIGGER

DESCRIPTION
              OEM implements this function. This function is executed
	      in interrupt context. 

DEPENDENCIES
  None

RETURN VALUE
              TRUE - strobe flash trigger signal is enabled to flash.
	      FALSE - strobe flash is not able to trigger.

SIDE EFFECTS
  This function must not call any function that does not support interrupt
  context. Must minimize the execution and return as soon as possible.

===========================================================================*/

boolean strobe_flash_start_trigger(void)
{
  strobe_flash_stop_charging(); // Stanley flash pecific

  /* use USB_SUSPEND GPIO for flash trigger */
  /* Configure as output  */
  BIO_TRISTATE( BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_OUTPUT_ENA_V); 
  /* output high to Flash Trigger */
  BIO_TRISTATE(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_M);
  BIO_OUT(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_ON_V);

  return TRUE;
}
#endif /* FEATURE_STROBE_FLASH*/


#ifdef FEATURE_STROBE_FLASH
/*===========================================================================

FUNCTION      STROBE_FLASH_STOP_TRIGGER

DESCRIPTION
              OEM implements this function. This function is executed
	      in interrupt context. 

DEPENDENCIES
  None

RETURN VALUE
              TRUE - strobe flash trigger signal is disabled.
	      FALSE - strobe flash trigger signal can not be disabled.

SIDE EFFECTS
  This function must not call any function that does not support interrupt
  context. Must minimize the execution and return as soon as possible.

===========================================================================*/

boolean strobe_flash_stop_trigger(void)
{
   /* use USB_SUSPEND GPIO for flash trigger */
   /* Configure as output  */
   BIO_TRISTATE(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_OUTPUT_ENA_V); 
   /* output high to Flash Trigger */
   BIO_TRISTATE(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_M);
   BIO_OUT(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_OFF_V);

  return TRUE;
}
#endif /* FEATURE_STROBE_FLASH*/

#ifdef FEATURE_STROBE_FLASH
/*===========================================================================

FUNCTION      STROBE_FLASH_REDEYE_REDUCTION

DESCRIPTION
              OEM implements this function. This function is executed
	      in interrupt context. 

DEPENDENCIES
  None

RETURN VALUE
              void 

SIDE EFFECTS
  This function must not call any function that does not support interrupt
  context. Must minimize the execution and return as soon as possible.

===========================================================================*/
#define FLASH_COUNT 3

void strobe_flash_redeye_reduction(void)
{
uint16  i; 
uint16  trigger_time;
uint16  charging_time;

   trigger_time = 10;
   charging_time = 150;
   /* Do short flashes several times to reduce red eye */
  
   for (i = 0; i < FLASH_COUNT; i++)
   {  
     strobe_flash_start_trigger();
     clk_busy_wait(trigger_time);
     strobe_flash_stop_trigger();
     strobe_flash_start_charging(); // for stanley strobe flash only
     camera_timed_wait(charging_time);
   }
   camera_timed_wait(300);
   
}
#endif /* FEATURE_STROBE_FLASH*/
