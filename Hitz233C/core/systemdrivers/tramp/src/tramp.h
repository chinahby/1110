#ifndef TRAMP_H
#define TRAMP_H
/*===========================================================================

      I N T E R R U P T   T R A M P O L I N E   H E A D E R   F I L E

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM CDMA ASIC interrupt controllers.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2003, 2004, 2005, 2006, 2007, 2008 by QUALCOMM, Incorporated.
All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tramp/tramp.h#2 $ 
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/08   vph     Added tramp_is_any_interrupt_pending()
07/20/07   vph     Port for MSM7500 from MSM6500

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "msm.h"
#include "queue.h"
#include "tramp_msm.h"


/*===========================================================================

                        DATA DECLARATIONS
                        
===========================================================================*/

/* Pointer to an isr function that is passed into tramp services */
typedef void (*tramp_isr_ptr_type) (void);

/* Pointer to an isr function that is passed within tramp services and
   requires the isr type for input 
 */
typedef void (*tramp_invoke_isr_ptr_type) (tramp_isr_type int_num);



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION     TRAMP_INIT

DESCRIPTION
  This function sets all ISRs to the default ISR, initializes the PIC
  hardware, and programs tramp_isr() to be called by REX whenever
  there is an interrupt in the System.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tramp_init( void );

/*===========================================================================

FUNCTION     TRAMP_SET_ISR

DESCRIPTION
  This function is used to set an ISR for a particular interrupt
  source.  This function enables or disables the mask corresponding to
  this interrupt in the IRQ_ENABLE register depending if the interrupt
  is being set or disabled.

  If the pointer to the ISR is NULL, the interrupt is being disabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tramp_set_isr
(
  /* Select which ISR is being set */
  tramp_isr_type       which_isr,     
  /* ISR to be installed for this interrupt */
  tramp_isr_ptr_type   isr_ptr
);

/*===========================================================================

FUNCTION TRAMP_BLOCK_TILL_DEASSERTED

DESCRIPTION
  Empty function no longer needed for use with new interrupt controller
  leaving in until can sync with corresponding sleep1x and sleephdr files

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tramp_block_till_deasserted
(
  tramp_isr_type which_isr,        /* Select which ISR is being checked    */
  dword timeout_us                 /* Timeout (in microsec) for blocking   */
);

/*===========================================================================

FUNCTION TRAMP_IS_ANY_INTERRUPT_PENDING

DESCRIPTION
  Find out if there is any waiting interrupt to be serviced.  It only makes 
  sense to call this function from a context where the interrupt in question is
  locked out, e.g. from a higher priority interrupt context or from inside
  INTLOCK/INTFREE.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE  - some interrupt is waiting to be serviced
  FALSE - no intterupt is waiting to be serviced.

SIDE EFFECTS
  None

===========================================================================*/
boolean tramp_is_any_interrupt_pending(void);

/*===========================================================================

FUNCTION TRAMP_IS_INTERRUPT_PENDING

DESCRIPTION
  Find out if an interrupt is waiting to be serviced.  It only makes sense
  to call this function from a context where the interrupt in question is
  locked out, e.g. from a higher priority interrupt context or from inside
  INTLOCK/INTFREE.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE  - interrupt is waiting to be serviced
  FALSE - the intterupt is not waiting to be serviced

SIDE EFFECTS
  None

===========================================================================*/
extern boolean tramp_is_interrupt_pending
(
  tramp_isr_type int_num
);

/*===========================================================================

FUNCTION TRAMP_CLEAR_INTERRUPT

DESCRIPTION
  Function is used to clear the bit or bits for a specified interrupt,
  this function should only be used in the case where the interrupt needs
  to be cleared outside of the "tramp_isr" context.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  Clears the bit or bits associated with the specified interrupt

===========================================================================*/
extern void tramp_clear_interrupt
(
  tramp_isr_type int_num
);

/*===========================================================================

FUNCTION TRAMP_SET_INTERRUPT

DESCRIPTION
  Function is used to cause interrupt to fire.

  This function will ONLY set primary interrupts, those interrupts
  that feed directly into the interrupt controller.

  There is no mechanism for setting secondary interrupts.  This will
  only cause the consolidated interrupt for the secondary interrupt to
  be set.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt Controller hardware will act as though the specific
  interrupt had fired and the interrupt will be handled as if the
  interrupt had come from the source.

===========================================================================*/
extern void tramp_set_interrupt
(
  tramp_isr_type int_num
);

/*===========================================================================

FUNCTION TRAMP_SHUTDOWN

DESCRIPTION
  Function is used to flush pending interrupts in the PIC and shutdown
  the ability for any IRQ or FIQ interrupts to occur.

  This is meant to be called during independent processor restart (AMSS
  only restart).

DEPENDENCIES
  Must be called with interrupts locked.

  After calling this function, tramp_init or tramp_restore must be
  called to restore the ability to generate IRQ or FIQ interrupts.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt Controller hardware will no longer generate IRQ or FIQ
  signals to the ARM.  

===========================================================================*/
void tramp_shutdown( void );

#endif  /* TRAMP_H */

