#ifndef GPIO_INT_H
#define GPIO_INT_H
/*===========================================================================

          G R O U P   I N T E R R U P T   S E R V I C E S

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM CDMA ASIC GROUP interrupt controllers.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2003, 2004, 2005, 2006, 2007 by QUALCOMM, Incorporated.
All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tramp/gpio_int.h#1 $ 
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/07   vph     Ported for QSC11X0 from QSC6055

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "msm.h"
#include "gpio_int_msm.h"



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Polarity types for GPIO Interrupts */
typedef enum
{
   ACTIVE_LOW = 0,
   ACTIVE_HIGH
} gpio_int_polarity_type;

/* EDGE/LEVEL detection types for GPIO Interrupts */
typedef enum
{
   DETECT_LEVEL = 0,
   DETECT_EDGE
} gpio_int_detect_type;

/* Callback type for GPIO Interrupts */
typedef void (*gpio_int_handler_type )(void);

/* Pointer to a gpio int handler that is passed within tramp services and
   requires the gpio number for input 
 */
typedef void (*gpio_invoke_int_handler_type)(gpio_int_type gpio_int_num);

/* Indicator that GPIO_INT Group 0 occured */
#define GPIO_INT_GROUP0_FIRED 0x1
/* Indicator that GPIO_INT Group 1 occured */
#define GPIO_INT_GROUP1_FIRED 0x2



/*===========================================================================
 
                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION GPIO_INT_INIT

DESCRIPTION
  Initialize the interrupt vector for the interrupt trampoline and
  configure the microprocessor and CDMA ASIC interrupt controllers
  for operation with it.

DEPENDENCIES
  This function should be called after tramp_init().

RETURN VALUE
  None

SIDE EFFECTS
  This disables all of the multiplexed CDMA ASIC interrupts.

===========================================================================*/
extern void gpio_int_init( void );


/*===========================================================================

FUNCTION GPIO_INT_RESTORE

DESCRIPTION
  Restores the GPIO_INT settings from before Power Collapse.

DEPENDENCIES
  This function should be called after tramp_restore().

RETURN VALUE
  None

SIDE EFFECTS
  Hardware will be restored to pre-power collapse state.
  Interrupts that occured during power collapse might have been lost.

===========================================================================*/
extern void gpio_int_restore( void );


#ifdef MSMHW_EDGE_LEVEL_INT_LATCH_OPTION
/*===========================================================================

FUNCTION GPIO_INT_SET_DETECT

DESCRIPTION
  Specify whether this GPIO interrupt is LEVEL or EDGE detected.
  
  Note: This function can be applied to NON group GPIO interrupts, 
        ie. GPIOs 0..4 as well as GPIOs 5..47.

DEPENDENCIES
  gpio_int_init() must have been previously called.

RETURN VALUE
  TRUE = Success, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean gpio_int_set_detect(gpio_int_type, gpio_int_detect_type);
#endif


/*===========================================================================

FUNCTION GPIO_INT_SET_HANDLER

DESCRIPTION
  Specify the address of the handler funtion for a given GROUP
  interrupt source.  Note that this function also sets up handlers for
  GPIO2_GROUP interrupts.  
  
  If the address is NULL, the interrupt is disabled, by masking it out
  in the CDMA ASIC GROUP interrupt controller.

DEPENDENCIES
  gpio_int_init() must have been previously called.
  gpio_int_set_detect should be called first, in order to set the
  detection type (LEVEL or EDGE).

RETURN VALUE
   boolean - FALSE => set action failed
             TRUE  => set action passed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean gpio_int_set_handler( gpio_int_type, 
                                     gpio_int_polarity_type, 
                                     gpio_int_handler_type );


/*===========================================================================

FUNCTION GPIO_INT_STORE_POWER_COLLAPSE_SETTINGS

DESCRIPTION
  Retrieves the GPIO_INT settings and places the information in shared
  memory.

DEPENDENCIES
  gpio_int_init() must have been previously called.

RETURN VALUE
  None

SIDE EFFECTS
  Shared memory will be updated with the current GPIO_INT settings.

===========================================================================*/
void gpio_int_store_power_collapse_settings( void );


/*===========================================================================

FUNCTION GPIO_INT_LOAD_POWER_COLLAPSE_SETTINGS

DESCRIPTION
  Retrieves the GPIO_INT settings from shared memory and updates the
  GPIO_INT software tables.

DEPENDENCIES
  gpio_int_store_power_collpase_settings() must have been previously called.

RETURN VALUE
  None

SIDE EFFECTS
  GPIO_INTS from the Application ARM will now be monitored by the
  Modem ARM.

===========================================================================*/
void gpio_int_load_power_collapse_settings( void );


/*===========================================================================

FUNCTION GPIO_INT_UNLOAD_POWER_COLLAPSE_SETTINGS

DESCRIPTION
  Turns off monitoring of Apps GPIO_INTs

DEPENDENCIES
  gpio_int_store_power_collpase_settings() must have been previously called.

RETURN VALUE
  None

SIDE EFFECTS
  GPIO_INTS from the Application ARM will no longer be monitored by
  the Modem ARM.

===========================================================================*/
void gpio_int_unload_power_collapse_settings( void );

#endif /* GPIO_INT_H */

