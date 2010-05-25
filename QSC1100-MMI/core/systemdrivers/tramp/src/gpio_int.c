/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          G R O U P   I N T E R R U P T   S E R V I C E S

GENERAL DESCRIPTION
   This module contains services for the GPIO Group Interrupt Block.  All 
   GPIO's can be configured as interrupts.  

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

EXTERNALIZED FUNCTIONS
  gpio_int_init()
    Initialize the GPIO interrupt logic involving the CDMA ASIC
  gpio_int_set_handler
    Register (or unregister) a service handler to a GPIO_INT source
  gpio_int_example_usage
    Reference code.  GPIO_INT_TEST_EXAMPLE must be enabled.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Call gpio_int_init() to initialize the logic, and then gpio_int_set_handler()
  as needed.  This must be done *after* the ASIC has been initialized,
  as the initialization procedure resets the interrupt block(s).

Copyright (c) 2004, 2005, 2006 by QUALCOMM, Incorporated.
All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tramp/gpio_int.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/06   gfr     Fix gpio_int_get_reg_offset to work with split GPIO_INT
                   registers.
10/20/06   gfr     Enable error messages on L4 builds.
09/07/06   jhs     Dynamic MPM settings support.
08/31/06   jhs     Fix not waking up from Apps Sleep.
                   Fix deadlock where LEVEL interrupts from Apps were not
                   cleared or disabled.
08/30/06   gfr     Removed broken preprocessor check.
08/24/06   wer     Modified gpio_interrupt_isr() to pass parameter to
                   service_handler().
05/23/06    cc     Added handling of split GPIO INT number groups in one GPIO 
                   register in gpio_interrupt_isr(). 
05/18/06   jhs     Added support to monitor GPIO_INTs during Apps Power
                   Collapse / Sleep.
01/24/06   jhs     Turned on GPIO_INTs for both Modem and Apps for ASIC 2.0
12/02/05   jhs     Changed feature protection for Rev 1 ASIC vs. Rev 2 ASIC.
09/26/05   jhs     Reimplemented gpio_int_restore to be more efficient.
08/04/05   jhs     Added ASIC 2.0 functionality.
06/29/05    cr     Further L4 FEATURE and code.
06/28/05    cr     On a branch of verion#8, add function prototypes to make
                   L4/linux compiler happy.
06/22/05   jhs     Added support for power restore.
06/08/05   jhs     Fixed incorrect register read for group0_isr
06/01/05   jhs     Fixed clearing of interrupt when set.
05/25/05   jhs     Added ERR_FATAL if used from Apps.
01/21/05   jhs     Code review comment changes.
01/10/05   jhs     Added while loop for gpio interrupt processing since we
                   don't get another interrupt if one is still pending
12/01/04   jhs     Change to match correct mappings for 7500 GPIOs
11/18/04    cr     Finish gpio_int_table implementation so that it matches
                   gpio_int.h declaration.
10/15/04    cr     Change hwio_gpio_int_table decl so that it is compiled/linked
                   to .constdata section rather than .data section. Required 
                   to be able to use HWIO_*(GPIO_*...) before boot_ram_init().
07/15/04   ptm     Comment out references to GPIO regs in gpio_int_init
                   because they are not defined in RUMI 3
06/17/04   donb    Renamed GPIO_INT registers to use GPIO_OLD... name until 
                   updated to match new HW files.
04/21/04   ddh     Ported from GPIO_INT_6500.c for MSM7500.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "tramp.h"
#include "err.h"
#include "gpio_int.h"
#include "gpio_inti.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Structure storing the details for a group interrupt */
typedef struct
{
  gpio_int_polarity_type  polarity;
  gpio_int_detect_type    detect;
  gpio_int_handler_type   isr;
} gpio_int_isr_info_type;


hwio_gpio_int_table_t hwio_gpio_int_table =
                        (hwio_gpio_int_table_t)&gpio_int_table;

/* Table of registered GPIO_INT handler functions */
static gpio_int_isr_info_type gpio_int_handler_tab[NUM_GPIO_INTS];


/*===========================================================================
 
                      PRIVATE FUNCTION DECLARATIONS
                      
===========================================================================*/


/*===========================================================================

FUNCTION GPIO_INT_DEFAULT_HANDLER

DESCRIPTION
  Default handler does nothing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void gpio_int_default_handler( void )
{

} /* end of gpio_int_default_handler */


/*===========================================================================

FUNCTION GPIO_INTERRUPT_ISR

DESCRIPTION
   This function is called from the main group0 or group1 interrupt
   handler routine for each register in that group.

   Note that the status register contains all GPIOs that have met the
   interrupt polarity criteria and not only the GPIOs that have
   generated interrupts.  As a result, this function masks the
   INT_STATUS register value so that only legitimate (been registered
   and configured as a GPIO_INT) GPIO_INTs are handled.

DEPENDENCIES
  gpio_int_init() is called to setup this interrupt and initialize the
  associated interrupt controllers in the CDMA ASIC.

RETURN VALUE
  None

SIDE EFFECTS
  Clears interrupts in the process of servicing them.

===========================================================================*/
static void gpio_interrupt_isr( uint32 reg_index )
{
  uint32 masked_gpio_int_status;
  uint32 reg_offset = 0;
  uint32 mask = 0;
  uint32 status_mask;
  uint32 group_mask_in_reg;
  gpio_invoke_int_handler_type service_handler;

  /* ------------------------------------------------------------------------
     Check the status register and vector to the appropriate ISR
  ------------------------------------------------------------------------ */

  status_mask = HWIO_INI(GPIO_INT_EN, reg_index);
  masked_gpio_int_status =
    HWIO_INMI(GPIO_INT_STATUS, reg_index, status_mask);

  /* One physical GPIO INT register could handle two or more split GPIO INT 
     number groups. This mask represent the group indexed by reg_index */
  group_mask_in_reg = ~( (1 << gpio_int_table[reg_index].starting_bit) - 1) & 
                       ( (1 << (gpio_int_table[reg_index].starting_bit +
                                gpio_int_table[reg_index].num_ints) ) - 1);
  /* Process the GPIO INT groups indexed by reg_index only here */
  masked_gpio_int_status &= group_mask_in_reg;


#ifdef GPIO_INT_ISR_LOOP_UNTIL_CLEAR
#error ISR_LOOP_UNTIL_CLEAR
#endif

  if (masked_gpio_int_status != 0)
  {
    /* Determine the source of the interrupt */
    for (reg_offset = 0; reg_offset < gpio_int_table[reg_index].num_ints;
         reg_offset++)
    {
      mask = 1U << (reg_offset + gpio_int_table[reg_index].starting_bit);
      
      if (masked_gpio_int_status & mask)
      {
        break;
      }
    }

    /* Grab the service handler */
    service_handler = (gpio_invoke_int_handler_type)
     (gpio_int_handler_tab[reg_offset + gpio_int_table[reg_index].offset].isr);

    /* Invoke the service handler */
    ((*(service_handler))
      ((gpio_int_type)(reg_offset + gpio_int_table[reg_index].offset)));

    /* Clear the interrupt */
    HWIO_OUTI(GPIO_INT_CLEAR, reg_index, mask);
  }
} /* end gpio_interrupt_isr */



/*===========================================================================

FUNCTION GPIO_GROUP0_INTERRUPT_ISR

DESCRIPTION
   This function is invoked when an unmasked GPIO_INT interrupt is
   asserted.  Note that the status register contains all GPIOs that
   have met the interrupt polarity criteria and not only the GPIOs
   that have generated interrupts.  As a result, this function masks
   the INT_STATUS register value so that only legitimate (been
   registered and configured as a GPIO_INT) GPIO_INTs are handled.

DEPENDENCIES
  gpio_int_init() is called to setup this interrupt and initialize the
  associated interrupt controllers in the CDMA ASIC.

RETURN VALUE
  None

SIDE EFFECTS
  Clears interrupts in the process of servicing them.

===========================================================================*/
static void gpio_group0_interrupt_isr( void )
{
  uint32 idx, reg_index;

  for (idx = 0; idx < NUM_GROUP0_INT_REGISTERS; idx++) 
  {
    reg_index = gpio_int_group0_table[idx];

    /* Call generic processing routine */
    gpio_interrupt_isr(reg_index);
  }

} /* end gpio_group0_interrupt_isr */




/*===========================================================================

FUNCTION GPIO_GROUP1_INTERRUPT_ISR

DESCRIPTION
   This function is invoked when an unmasked GPIO_GROUP2 GPIO_INT
   interrupt is asserted.  Note that the status register contains all
   GPIOs that have met the interrupt polarity criteria and not only
   the GPIOs that have generated interrupts.  As a result, this
   function masks the INT_STATUS register value so that only
   legitimate (been registered and configured as a GPIO_INT) GPIO_INTs
   are handled.

DEPENDENCIES
  gpio_int_init() is called to setup this interrupt and initialize the
  associated interrupt controllers in the CDMA ASIC.

RETURN VALUE
  None

SIDE EFFECTS
  Clears interrupts in the process of servicing them.

===========================================================================*/
static void gpio_group1_interrupt_isr( void )
{
  uint32 idx, reg_index;

  /* ------------------------------------------------------------------------
     Check each status register to see where the interrupt came from
  ------------------------------------------------------------------------ */

  for (idx = 0; idx < NUM_GROUP1_INT_REGISTERS; idx++) 
  {
    reg_index = gpio_int_group1_table[idx];

    /* Call generic processing routine */
    gpio_interrupt_isr(reg_index);
  }

} /* end gpio_group1_interrupt_isr */



/*===========================================================================

FUNCTION    GPIO_INT_GET_REG_OFFSET

DESCRIPTION
  Given a GPIO_INT enum, this function will return an index into the
  gpio_int_table register table and an index within that register.
  These values pinpoint the bit needed to be set for this GPIO_INT.

DEPENDENCIES
  Assumes that the GPIO_INT specified is within the valid range of
  GPIO_INTs.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gpio_int_get_reg_offset
(
  const gpio_int_type which_group_int, 
    /* Interrupt in question */
  uint32 *reg_index, 
    /* Pointer to resulting register index */
  uint32 *reg_offset
    /* Pointer to resulting offset within the above register */
)
{
  int32 idx;
  gpio_int_type first_int;
  gpio_int_type last_int;

  for (idx = NUM_GROUP_INT_REGISTERS - 1; idx >= 0; idx--)
  {
    first_int = (gpio_int_type)gpio_int_table[idx].offset;
    last_int = (gpio_int_type)(first_int + gpio_int_table[idx].num_ints - 1);

    if (which_group_int >= first_int && which_group_int <= last_int)
    {
      *reg_index = (uint32)idx;
      *reg_offset = which_group_int - gpio_int_table[idx].offset +
                    gpio_int_table[idx].starting_bit;
      return;
    }
  }

  /* Flag an error if we did not find the offset */
  MSG_ERROR("Could not find gpio int offset (int=%d)", which_group_int, 0, 0); 
  *reg_index = 0;
  *reg_offset = 0;
} /* end gpio_int_get_reg_offset */



/*===========================================================================
 
                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
boolean gpio_int_set_handler
(
  gpio_int_type          which_group_int,  
      /* Select which GPIO_INT is being handled */ 
  gpio_int_polarity_type polarity,         
      /* Trigger on low/high signal */
  gpio_int_handler_type  handler           
      /* ISR to be executed */
)
{
  uint32 register_index  = 0;
  uint32 register_offset = 0;
  uint32 mask;
  uint32 gpio_int_index;

  /* Validate the given group interrupt */
  if ( ((int32)which_group_int < FIRST_GROUP_INT) ||
       ((int32)which_group_int > LAST_GROUP_INT ) )
  {
    MSG_ERROR("Invalid GPIO_INT specified - %d", which_group_int, 0, 0);
    return FALSE;
  }
  
  /* ------------------------------------------------------------------------
     Determine which register and offset refer to this interrupt
  ------------------------------------------------------------------------ */

  gpio_int_get_reg_offset( which_group_int, 
                           &register_index, 
                           &register_offset );

  /* Create the mask for setting/clearing the appropriate register bit */
  mask = (1 << register_offset);

  /* Get the gpio_int index */
  gpio_int_index = which_group_int - FIRST_GROUP_INT;
  

  /* ------------------------------------------------------------------------
     Set up GPIO_INT handler, and unmask the interrupt
  ------------------------------------------------------------------------ */

  /* Disable interrupts */
  INTLOCK();

  /* If the handler is NULL, set it to the default */
  if (handler == NULL) 
  {
    gpio_int_handler_tab[gpio_int_index].isr = gpio_int_default_handler;
  
    /* Mask this interrupt */
    HWIO_OUTMI(GPIO_INT_EN, register_index, mask, 0);
  }
  else
  {
    /* Register the handler for this particular GROUP interrupt source */
    gpio_int_handler_tab[gpio_int_index].isr = handler;
  
    /* Establish the polarity for the interrupt */
    if ( polarity == ACTIVE_HIGH ) 
    {
      HWIO_OUTMI(GPIO_INT_POLARITY, register_index, mask, mask);
      gpio_int_handler_tab[gpio_int_index].polarity = ACTIVE_HIGH;
    }
    else
    {
      HWIO_OUTMI(GPIO_INT_POLARITY, register_index, mask, 0);
      gpio_int_handler_tab[gpio_int_index].polarity = ACTIVE_LOW;
    }

    /* Clear the interrupt source before unmasking it */
    HWIO_OUTI(GPIO_INT_CLEAR, register_index, mask);

    /* Unmask this interrupt */
    HWIO_OUTMI(GPIO_INT_EN, register_index, mask, mask);
  }

  /* Restore interrupts */
  INTFREE();            
  
  return TRUE;
}  /* gpio_int_set_handler */



/*===========================================================================

FUNCTION GPIO_INT_SET_DETECT

DESCRIPTION
  Specify whether this GPIO interrupt is LEVEL or EDGE detected.
  
  Note: This function can be applied to NON group GPIO interrupts.

DEPENDENCIES
  gpio_int_init() must have been previously called.

RETURN VALUE
  TRUE = Success, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean gpio_int_set_detect
(
  gpio_int_type         which_group_int,
      /* Which GPIO_INT to set */
  gpio_int_detect_type  detect
      /* Detect interrupts by LEVEL or EDGE */
)
{
  uint32 register_index  = 0;
  uint32 register_offset = 0;
  uint32 mask;
  uint32 gpio_int_index;

  /* Check limit conditions */
  if ( ((int32)which_group_int < FIRST_GROUP_INT) ||
       ((int32)which_group_int > LAST_GROUP_INT ) )
  {
    MSG_ERROR("Invalid GPIO_INT specified - %d", which_group_int, 0, 0);
    return FALSE;
  }
  

  /* ------------------------------------------------------------------------
     Determine which register and offset refer to this interrupt
  ------------------------------------------------------------------------ */

  gpio_int_get_reg_offset( which_group_int, 
                           &register_index, 
                           &register_offset );

  /* Create the mask for setting/clearing the appropriate register bit */
  mask = (1 << register_offset);

  /* Get the gpio_int index */
  gpio_int_index = which_group_int - FIRST_GROUP_INT;
 

  /* ------------------------------------------------------------------------
     Set up GPIO_INT latch type
  ------------------------------------------------------------------------ */

  /* Disable interrupts */
  INTLOCK();                    
  
  /* Establish the detection for this interrupt */
  if ( detect == DETECT_EDGE ) 
  {
    HWIO_OUTMI(GPIO_INT_DETECT_CTL, register_index, mask, mask);
    gpio_int_handler_tab[gpio_int_index].detect = DETECT_EDGE;
  }
  else
  {
    HWIO_OUTMI(GPIO_INT_DETECT_CTL, register_index, mask, 0);
    gpio_int_handler_tab[gpio_int_index].detect = DETECT_LEVEL;
  }

  /* Restore interrupts */
  INTFREE();                    
  
  return TRUE;
}  /* gpio_int_set_detect */



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
void gpio_int_init(void)
{
  uint32 i = 0;
  uint32 mask_all;
  
  INTLOCK();

  /* Initialize all the group int registers */
  for (i = 0; i < NUM_GROUP_INT_REGISTERS; i++) 
  {
    /* Disable interrupts */
    mask_all = HWIO_RMSKI(GPIO_INT_EN, i);
    HWIO_OUTMI(GPIO_INT_EN, i, mask_all, 0);
    
    /* Clear interrupts */
    HWIO_OUTI(GPIO_INT_CLEAR, i, 0);
    
    /* Configure polarities as active HIGH */
    mask_all = HWIO_RMSKI(GPIO_INT_POLARITY, i);
    HWIO_OUTMI(GPIO_INT_POLARITY, i, mask_all, mask_all);
    
    /* Configure for LEVEL detection */
    mask_all = HWIO_RMSKI(GPIO_INT_DETECT_CTL, i);
    HWIO_OUTMI(GPIO_INT_DETECT_CTL, i, mask_all, 0);
  }

  /*
   * Initialize the handler table to the default "do nothing" 
   * handler function
   */
  for (i = 0; i < NUM_GPIO_INTS; i++) 
  {
    gpio_int_handler_tab[i].polarity = ACTIVE_HIGH;
    gpio_int_handler_tab[i].detect = DETECT_LEVEL;
    gpio_int_handler_tab[i].isr = gpio_int_default_handler;
  }

  INTFREE();
  
  #ifdef GPIO_INT_INSTALL_ISR
    /* Install the handler, unmask this irq source, and enable
   * GPIO_GROUP0 interrupts
     */
  tramp_set_isr( TRAMP_GPIO_GROUP0_ISR, gpio_group0_interrupt_isr );
  
    /* Install the handler, unmask this irq source, and enable
   * GPIO_GROUP1 interrupts
     */
  tramp_set_isr( TRAMP_GPIO_GROUP1_ISR, gpio_group1_interrupt_isr );
  #endif /* GPIO_INT_INSTALL_ISR */
  
}  /* gpio_int_init */
