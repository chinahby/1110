/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  T R A M P O L I N E    S E R V I C E S

GENERAL DESCRIPTION
  This file contains the definitions for the Trampoline Services that are
  provided on MSMs that have a Prioritized Interrupt Controller (PIC).
  This file contains the table of values for priorities for each of the
  interrupt sources. It also contains a table of flags that indicate
  when the status bits for each of the interrupts should be cleared
  (before or after calling the ISR).

EXTERNALIZED FUNCTIONS

  tramp_init
    This function sets all ISRs to the default ISR, initializes the PIC
    hardware, and programs tramp_isr() to be called by REX whenever
    there is an interrupt in the System.

  tramp_restore
    This function initialized the PIC hardware and restores the
    enabled interrupts to the state they were in prior to power
    collapse.

  tramp_set_isr
    This function is used to set an ISR for a particular interrupt
    source.  This function enables or disables the mask corresponding to
    this interrupt in the IRQ_ENABLE register depending if the interrupt
    is being set or disabled.
  
  tramp_is_interrupt_pending
    Find out if an interrupt is waiting to be serviced.  It only makes sense
    to call this function for a contet where the interrupt in question is
    locked out, e.g. from a higher priority interrupt context or from inside
    INTLOCK/INTFREE.

  tramp_clear_interrupt
    Function is used to clear the bit or bits for a specified interrupt,
    this function should only be used in the case where the interrupt needs
    to be cleared outside of the "tramp_isr" context.

  tramp_set_interrupt
    Function is used to cause interrupt to fire.  If a secondary interrupt is
    used as the input, then the consolidated interrupt will be set, but the
    secondary interrupt does not have a way to set it, so the handling of the
    interrupt might not get the desired result.

INTERNALIZED FUNCTIONS

  tramp_default_handler
    Default handler does nothing.

  tramp_sub_isr
    This function is called when an IRQ interrupt occurs and the interrupt
    contains sub interrupt. This function discovers which of the sub
    interrupt(s) have fired and handles them appropriately.

    The corresponding mask in the IRQ_ENABLE register is cleared upon
    entering and reenable upon exiting if it exist.  Either way a copy
    of the mask is maintianed within the consolidated interrupt entry in
    the primary interrupt table.

    There are two cases for each of the sub interrupts.
      Case I - cleared before calling the ISR. 
      CaseII - cleared after calling the ISR.

  tramp_set_sub_isr
    This function enables or disables the source interrupts which have been 
    consolidated into a single interrupt which is processed by the PIC HW. If
    the source interrupt is being disabled this function will determine if any
    other interrupts remain enabled for the PIC interrupt, if so then the ISR
    information returned for this function will be set to tramp_sub_isr, 
    otherwise it will be set to tramp_set_default. This function will also    
    determine what the interrupt number is for the PIC interrupt associated
    with the input interrupt and provide this information to the calling 
    routine.

  tramp_init_hardware
    This function sets the detection type, FIQ or IRQ, and priority for
    all primary interrupts.  It also sets sets detection type and
    polarity for secondary interrupts.  These settings are statically
    defined in trampi_*7500.c.
    
    It then clears the status registers for the IRQ and FIQ interrupts,
    and masks all IRQ and FIQ interrupts. Interrupts are masked until
    the corresponding bit in the IRQ_ENABLE register is set.

  tramp_isr
    This function is called when an IRQ interrupt occurs. This functjon reads
    from the IRQ_VEC_INDEX_RD of the PIC and jumps to the ISR corresponding
    to the value returned by the register, until a NON_VECTOR value is
    returned by the register, signalling there are no pending interrupts.
  
    The corresponding mask in the IRQ_ENABLE register is cleared depending
    on two cases.
    Case I - cleared before calling the ISR.
    CaseII - cleared after calling the ISR. In this case, we cannot simply
             unlock interrupts and call the ISR, because the interrupt we
             are processing will show up again.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Function tramp_init() should be called before any of the Tramp Services
  can be used.

Copyright (c) 2000, 2001, 2004 - 2008 by QUALCOMM Incorporated.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
software is regulated by the U.S. Government. Diversion contrary to
U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tramp/tramp.c#7 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/21/08   gfr     Fix tramp_is_any_interrupt_pending to use status registers.
06/20/08   vph     Added tramp_is_any_interrupt_pending()
07/20/07   vph     Ported to QSC11X0 from QSC6055.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "msm.h"                /* Includes for specific HW mappings    */
#include "arm.h"                /* definition of ARM interrupt values   */
#include "rex.h"                /* REX declarations.                    */
#include "msg.h"                /* Message logging macro                */
#include "err.h"                /* Error logging macro                  */

#include "tramp.h"              /* Trampoline typedefs and prototypes   */
#include "trampi.h"

#ifdef TIMETEST
  #include "timetest.h"
#endif

/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/
static uint32 tramp_is_shutdown = FALSE;


/*=========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION TRAMP_DEFAULT_HANDLER

DESCRIPTION
  Default handler does nothing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tramp_default_isr( void )
{
  /* Do nothing */
} /* end tramp_default_isr */



#ifdef FEATURE_2ND_LVL_INTR
/*===========================================================================

FUNCTION     TRAMP_SUB_ISR

DESCRIPTION
  This function is called when an IRQ interrupt occurs and the interrupt
  contains sub interrupt. This function discovers which of the sub
  interrupt(s) have fired and handles them appropriately.

  The corresponding mask in the IRQ_ENABLE register is cleared upon
  entering and reenable upon exiting if it exist.  Either way a copy
  of the mask is maintianed within the consolidated interrupt entry in
  the primary interrupt table.

  There are two cases for each of the sub interrupts.
    Case I - cleared before calling the ISR. 
    CaseII - cleared after calling the ISR.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void  tramp_sub_isr
(
  tramp_isr_type int_num
)
{
  uint32   sub_tbl_idx;   /* Base index for the second level ints */
  uint32   new_idx;       /* Index for accessing table entry */
  uint32   status;        /* Contents of status register */
  uint32   clear_bit;     /* Mask used to update registers */
  uint32   cnt;           /* Loop control variable */

  if ( int_num >= TRAMP_NUM_PIC_INTS ) {
    ERR_FATAL("Interrupt number is out of bound", 0, 0, 0);
    return;
  }

  /* Retrieve the index for the first of the second level 
   * interrupts which have been consolidated into the one which
   * was detected
   */
  sub_tbl_idx = (uint32)( tramp_isr_tbl[int_num].sub_tbl_idx - 
                          TRAMP_NUM_PIC_INTS );

  /* Read the status register to determine which interrupts occured */
  status = inpdw( tramp_sub_isr_tbl[sub_tbl_idx].status_reg );

  if( status != 0 )
  {
    /* Loop through each of the entries that have been consolidated into 
    * this interrupt, processing each that have occurred
    */
    for( cnt = 0; cnt < tramp_isr_tbl[int_num].sub_cnt; cnt++ )
    {
      new_idx = sub_tbl_idx + cnt;
      if( ( ( status & tramp_sub_isr_tbl[new_idx].status_bit ) != 0 ) &&
          ( tramp_sub_isr_tbl[new_idx].isr_ptr != 
            (tramp_invoke_isr_ptr_type)tramp_default_isr ) )
      {
        /* Determine which bit to clear */
        clear_bit = tramp_sub_isr_tbl[new_idx].clr_bit;

        if( tramp_sub_isr_tbl[new_idx].clr_when == TRAMP_CLR_BEFORE )
        {
          if( tramp_sub_isr_tbl[new_idx].clear_reg )
          {
            /* Clear the interrupt */
            outpdw(tramp_sub_isr_tbl[new_idx].clear_reg, clear_bit );
          }
          
          /* Call the ISR to process the interrupt */
          (tramp_sub_isr_tbl[new_idx].isr_ptr) 
            ((tramp_isr_type)(new_idx + TRAMP_NUM_PIC_INTS));
        } /* end if CLR_BEFORE */
        else
        {
          /* Call the ISR to process the interrupt */
          (tramp_sub_isr_tbl[new_idx].isr_ptr) 
            ((tramp_isr_type)(new_idx + TRAMP_NUM_PIC_INTS));

          if( tramp_sub_isr_tbl[new_idx].clear_reg )
          {
            /* Clear the interrupt */
            outpdw( tramp_sub_isr_tbl[new_idx].clear_reg, clear_bit );
          }
        } /* end if CLR_AFTER */

        tramp_sub_isr_tbl[new_idx].cnt++;

      } /* end if status bit set */
    } /* end for cnt < sub_cnt */
  } /* end if status != 0 */

} /* end tramp_sub_isr */
#endif /* FEATURE_2ND_LVL_INTR */


#ifdef FEATURE_2ND_LVL_INTR
/*===========================================================================

FUNCTION     TRAMP_SET_SUB_ISR

DESCRIPTION
  This function enables or disables the source interrupts which have been 
  consolidated into a single interrupt which is processed by the PIC HW. If
  the source interrupt is being disabled this function will determine if any
  other interrupts remain enabled for the PIC interrupt, if so then the ISR
  information returned for this function will be set to tramp_sub_isr, 
  otherwise it will be set to tramp_set_default. This function will also    
  determine what the interrupt number is for the PIC interrupt associated
  with the input interrupt and provide this information to the calling 
  routine.

DEPENDENCIES
  Expects interrupts to be locked.

RETURN VALUE
  int_info  Interrupt number corresponding for the interrupt in which 
            the specified interrupt has been consolidated into AND
                  the ISR to be use by the consolidated interrupt

SIDE EFFECTS
  None
===========================================================================*/
static void tramp_set_sub_isr
(
  /* Interrupt information */
  tramp_int_info_type*    info
)
{

  /* Address of enable register */
  uint32 enable_addr;

  /* Mask to use for enable register */
  uint32 enable_mask;

  /* Specific enable bit for this interrupt */
  uint32 enable_bit;

  /* Index into sub interrupt enable table */ 
  uint32 enable_idx;

  /* Address of clear register */
  volatile uint32 *clear_addr;

  /* Specific clear bit for this interrupt */
  uint32 clear_bit;
  
  tramp_isr_ptr_type old_isr;

  /* Index for tramp_sub_isr_tbl  */
  uint32 idx; 

  /* Holder for read check valued */
  uint32 check_val;

  /* Determine the table index to use for the tramp_sub_isr_tbl */   
  idx = (uint32)(info->int_num - TRAMP_NUM_PIC_INTS);

  if ( idx >= TRAMP_NUM_SUB_INTS ) {
    ERR_FATAL("Sub tramp interrupt index is out of bound", 0, 0, 0);
  }

  /* What HW address is used to enable/disable this interrupt */
  enable_addr = tramp_sub_isr_tbl[idx].enable_reg;

  /* What bit within the enable address is designated for this
   * interrupt
   */
  enable_bit = tramp_sub_isr_tbl[idx].en_bit;

  /* What bits to check? */
  enable_mask = tramp_isr_tbl[tramp_sub_isr_tbl[idx].pic_int_num].sub_en_mask;

  /* Grab the index into the sub interrupt enable table */
  enable_idx = tramp_isr_tbl[tramp_sub_isr_tbl[idx].pic_int_num].sub_en_idx;

  /* What HW address is used to clear this interrupt */
  clear_addr = tramp_sub_isr_tbl[idx].clear_reg;

  /* What bit within the clear address is designated for this
   * interrupt
   */
  clear_bit = tramp_sub_isr_tbl[idx].clr_bit;

  /* Store the old isr pointer */
  old_isr = (tramp_isr_ptr_type)tramp_sub_isr_tbl[idx].isr_ptr;

  /* Set the ISR for this specific interrupt */
  tramp_sub_isr_tbl[idx].isr_ptr = (tramp_invoke_isr_ptr_type)info->isr_ptr;

  /* Set or clear bit appropriately */
  if( info->isr_ptr == (tramp_invoke_isr_ptr_type)tramp_default_isr )
  {
    /* Write out the enable mask */
    TRAMP_EN( enable_addr, enable_bit, ~(enable_bit) );   
    /* Clear the interrupt.  This is to work around level primary
     *  sources staying asserted when the secondary source is
     *  disabled, but the secondary source is pending.
     */
    if( clear_addr != NULL )
    {
      outpdw( clear_addr, clear_bit );
    }
  }
  else
  {
    /* Clear the interrupt if not set previously */
    if( old_isr == tramp_default_isr )
    {
      if( clear_addr != NULL )
      {
        outpdw( clear_addr, clear_bit );
      }
    }
    /* Write out the enable mask */
    TRAMP_EN( enable_addr, enable_bit, (enable_bit) ); 
  }

  /* Determine if there are any interrupts still enabled for 
   * this consolidated interrupt 
   */
  if( enable_idx != TRAMP_EN_MAX )
  {
    TRAMP_EN_CHECK( enable_idx, enable_mask, check_val );
    if( check_val == 0 )
    {
      info->isr_ptr = (tramp_invoke_isr_ptr_type)tramp_default_isr;
    }
    else
    {
      info->isr_ptr = tramp_sub_isr;
    }
  }

  /* Determine what the PIC interrupt number is */
  info->int_num = (tramp_isr_type)(tramp_sub_isr_tbl[idx].pic_int_num);

} /* end tramp_set_sub_isr */  
#endif /* FEATURE_2ND_LVL_INTR */


/*===========================================================================

FUNCTION     TRAMP_INIT_HARDWARE

DESCRIPTION
  This function sets the detection type, FIQ or IRQ, and priority for
  all primary interrupts.  It also sets sets detection type and
  polarity for secondary interrupts.  These settings are statically
  defined in trampi_*7500.c.
  
  It then clears the status registers for the IRQ and FIQ interrupts,
  and masks all IRQ and FIQ interrupts. Interrupts are masked until
  the corresponding bit in the IRQ_ENABLE register is set.

DEPENDENCIES
  tramp_init_proc_spec must already have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tramp_init_hardware( void )
{
  /* Address of priority register for this interrupt */
  uint32 prio_address; 

  /* Priority type for this interrupt */
  uint32 priority_val;

  /* Address of egde or level detection register for this interrupt */
  volatile uint32 *detect_address;
  
  /* Mask of the detection register */
  uint32 detect_mask;

  /* Detection type for this interrupt */
  tramp_detect_type detect_val;

  /* Address of the select register for this interrupt */
  volatile uint32 *select_address;

  /* Mask of the select register */
  uint32 select_mask;

  /* Index loop variable */
  uint32 int_idx;

  /* Bit to manipulated for primary interrupt setup */
  uint32 int_bit;
  
#ifdef FEATURE_2ND_LVL_INTR
  /* Specific detect bit for this interrupt */
  uint32 detect_bit;

  /* Address of enable register */
  volatile uint32 *polarity_address;
  
  /* Mask to use for polarity register */
  uint32 pol_mask;

  /* Specific enable bit for this interrupt */
  uint32 pol_bit;
#endif /* FEATURE_2ND_LVL_INTR */

  /* Loop through all the primary interrupts and do all the
   * initalization for each interrupt.
   */
  for( int_idx = 0; int_idx < TRAMP_NUM_PIC_INTS; int_idx++ )
  {
    /* Range check the priority of the interrupt */
    if( tramp_isr_tbl[int_idx].priority == TRAMP_PRIO_LVL_INVALID )
    {
      ERR_FATAL( "TRAMP: This is an invalid priority for interrupt %d", 
               int_idx, 0, 0 );
    }

    /* Get the priority type for this interrupt from the interrupt
     * table 
     */
    priority_val =(uint32)( tramp_isr_tbl[int_idx].priority &
                            TRAMP_PRIO_MASK );

    /* Get the detect type register address for this interrupt from
     * the interrupt table.
     */
    detect_address = tramp_isr_tbl[int_idx].detect_reg;

    /* Get the detection type for this interrupt from the interrupt
     * table
     */
    detect_val = tramp_isr_tbl[int_idx].edge_or_level;

    /* Get the select register address for this interrupt from the
     * interrupt table
     */
    select_address = tramp_isr_tbl[int_idx].select_reg;

    /* Get the priority register address for this interrupt from
     * inverting the interrupt index. 
     */
    prio_address = PRIORITY_n( (unsigned int)
                              TRAMP_NUM_PIC_INTS -
                              int_idx - 1 );
    
    /* Determine which mask register we need to adjust */
    if( (TRAMP_NUM_PIC_INTS - int_idx - 1) >= NUM_INT_BITS_IN_REG )
    {
      /* Invert index and convert to bit offset in register */
      int_bit = (tramp_isr_type)( (TRAMP_NUM_PIC_INTS - int_idx) - 
                  NUM_INT_BITS_IN_REG - 1 );
    }
    else
    {
      /* Invert index */
      int_bit = (tramp_isr_type)( TRAMP_NUM_PIC_INTS - int_idx - 1 );
    }

    /* If the interrupt has a edge or level detection register address
     * defined, then read the current value.  If the interrupt is level
     * detection then clear the corresponding bit in the mask, otherwise
     * set the corresponding bit in the mask.  Write the mask back out
     * to the detection register.
     */
    if( detect_address != NULL )
    {
      /* Read current value of detect register */
      detect_mask = inpdw( detect_address );

      /* If interrupt is LEVEL then clear the correct bit */
      if( detect_val == TRAMP_LEVEL_DETECT )
      {
        detect_mask &= ~( 1UL << int_bit );
      }
      /* Otherwise the interrupt is EDGE, set the correct bit */
      else
      {
        detect_mask |= ( 1UL << int_bit );
      } 

      /* Write new value to detect register */
      outpdw( detect_address, detect_mask );
    }

    /* If the interrupt has a select register address defined, then
     * read the current value.  Then set the corresponding bit in the
     * mask Write the mask back out to the select register.
     */
    if( select_address != NULL )
    {
      /* Read current value of select register */
      select_mask = inpdw( select_address );
      /* Set the correct bit */
      select_mask |= ( 1UL << int_bit );
      /* Write new value to select register */
      outpdw( select_address, select_mask );
    }

    /* Write the priority to the priority register. */
    outpdw( prio_address, priority_val );
  } /* end for loop */
  
#ifdef FEATURE_2ND_LVL_INTR
  /* Loop through all the secondary interrupts and do all the
   * initalization for each interrupt.
   */
  for( int_idx = 0; 
       int_idx < TRAMP_NUM_INTS - TRAMP_NUM_PIC_INTS; 
       int_idx++ )
  {
    /* What HW address is used to set edge or level detection of this
     * interrupt
     */
    detect_address = tramp_sub_isr_tbl[int_idx].detect_reg;

    /* What bit within the detection address is designated for this
     * interrupt
     */
    detect_bit = tramp_sub_isr_tbl[int_idx].detect_bit;

    /* What HW address is used to set polarity detection of this
     * interrupt
     */
    polarity_address = tramp_sub_isr_tbl[int_idx].pol_reg;

    /* What bit within the polarity address is designated for this
     * interrupt
     */
    pol_bit = tramp_sub_isr_tbl[int_idx].pol_bit;

    /* If there is a edge or level detection register for this interrupt, 
     * read the value and then set or clear this bit appropriately and
     * write the new value back.
     */
    if( detect_address != NULL )
    {
      /* Read current value of detect register */
      detect_mask = inpdw( detect_address );

      /* If interrupt is LEVEL then clear the correct bit */
      if( tramp_sub_isr_tbl[int_idx].edge_or_level == TRAMP_LEVEL_DETECT )
      {
        detect_mask &= ~(detect_bit);
      }
      /* Otherwise the interrupt is EDGE, set the correct bit */
      else
      {
        detect_mask |= (detect_bit);
      } 

      /* Write new value to detect register */
      outpdw( detect_address, detect_mask );
    }

    /* If there is a polarity detection register for this interrupt, 
     * read the value and then set or clear this bit appropriatly and
     * write the new value back.
     */
    if( polarity_address != NULL )
    {
      /* Read current value of polarity register */
      pol_mask = inpdw( polarity_address );

      /* If interrupt is NEGATIVE then clear the correct bit */
      if( tramp_sub_isr_tbl[int_idx].polarity == TRAMP_NEG_POLARITY )
      {
        pol_mask &= ~(pol_bit);
      }
      /* Otherwise the interrupt is POSITIVE, set the correct bit */
      else
      {
        pol_mask |= (pol_bit);
      } 

      /* Write new value to polarity register */
      outpdw( polarity_address, pol_mask );
    }
  } /* end for loop */
#endif /* FEATURE_2ND_LVL_INTR */

  /******** IRQ/FIQ Initialization **********/

  /* Clear the IRQ Status registers */
  HWIO_OUT(IRQ_CLEAR_0, CLEAR_ALL_MASK);
  HWIO_OUT(IRQ_CLEAR_1, CLEAR_ALL_MASK);

  /* Mask all IRQ interrupts. Interrupts are masked until the
   * corresponding bit in the IRQ_ENABLE register is set.
   */
  HWIO_OUT(IRQ_EN_0, DISABLE_ALL_MASK);
  HWIO_OUT(IRQ_EN_1, DISABLE_ALL_MASK);

} /* end tramp_init_hardware */


/*===========================================================================

FUNCTION TRAMP_ISR

DESCRIPTION
  This function is called when an IRQ interrupt occurs. This function reads
  from the IRQ_VEC_INDEX_RD of the PIC and jumps to the ISR corresponding
  to the value returned by the register, until a NON_VECTOR value is
  returned by the register, signalling there are no pending interrupts.

  The corresponding mask in the IRQ_ENABLE register is cleared depending
  on two cases.
  Case I - cleared before calling the ISR.
  CaseII - cleared after calling the ISR. In this case, we cannot simply
           unlock interrupts and call the ISR, because the interrupt we
           are processing will show up again.

DEPENDENCIES
  Assumption - Assuming that we come into tramp with INTLOCK in
               effect - the  way it has always worked.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tramp_isr( )
{
  /* Vector index from PIC register */
  uint32 vect_idx = TR_NON_VECTOR;

  /* For our local table */
  tramp_isr_type tbl_idx = TRAMP_NO_INTS;

  /* Address of the IRQ_ENABLE register */
  volatile uint32 *en_reg_address;

  /* Mask for enable register */
  uint32 enable_mask;

  /* Address of the IRQ_CLEAR register */
  volatile uint32 *clear_reg_address;

  /*
   * NOTE: Two distinct cases are made for obtaining the interrupt
   * index.
   *
   * IRQ_VEC_INDEX_RD contains the index to the interrupt vector of the
   * current highest priority active interrupt. In order to implement the
   * "smart function" whereby multiple interrupts can be serviced without
   * switching back and forth between TASK and IRQ mode, we first read
   * this register.  Note that the act of reading this register tells the
   * PIC that we are done processing the ISR that was read the previous
   * time.  So we cannot arbitrarily read this register - we have to be 
   * done with the previous ISR.
   *
   * IRQ_VEC_PEND_RD contains the index to the interrupt vector of the
   * current highest priority active interrupt.  All reads for 
   * interrupts after FIRST reading IRQ_VEC_INDEX_RD should be done using
   * IRQ_VEC_PEND_RD. Note that the act of reading this register tells the
   * PIC that we are done processing the ISR that was read the previous
   * time.  So we cannot arbitrarily read this register - we have to
   * be done with the previous ISR.
   */

  /* Read interrupt vector index from VEC_INDEX_RD */
  vect_idx = HWIO_INM( IRQ_VEC_INDEX_RD, HWIO_RMSK(IRQ_VEC_INDEX_RD) );

  /* SW Workaround to be able to exit loop on two differnt vectors */
  while ( vect_idx != TR_NON_VECTOR )
  {
    /* Convert vector index to a table index */
    tbl_idx = (tramp_isr_type)( TRAMP_NUM_PIC_INTS - 
                                (tramp_isr_type)vect_idx - 1 );

#ifdef TIMETEST
    TIMETEST_ISR_ID( tramp_isr_tbl[tbl_idx].id );
#endif

    /* At this point we are technically executing the ISR, even
     * though we are not in the main ISR task. Unlock interrupts to
     * allow pre-emption.
     */
      rex_int_free();

    /* Obtain register addresses for this interrupt from the
     * interrupt table
     */
    clear_reg_address = tramp_isr_tbl[tbl_idx].clear_reg;
    en_reg_address = tramp_isr_tbl[tbl_idx].enable_reg;

    /* Correct offset for higher level registers */
    if ( vect_idx >= NUM_INT_BITS_IN_REG)
    {
      vect_idx -= NUM_INT_BITS_IN_REG;
    }

    /* Case 1.  Interrupt needs to be cleared before calling the ISR */
    if( tramp_isr_tbl[tbl_idx].clr_when == TRAMP_CLR_BEFORE )
    {
      /* Clear the interrupt */
      outpdw( clear_reg_address, (1 << vect_idx) );  
    
      /* Call the registered ISR */
      (tramp_isr_tbl[tbl_idx].isr_ptr) (tbl_idx);  
    }
    /* Case 2. Interrupt needs to be cleared after calling the ISR.
     * In this case, we cannot simply unlock interrupts and call the
     * ISR, because the interrupt we are processing will show up
     * again. This will happen recursively and quickly blow us past
     * our IRQ stack. So before unlocking we need to mask off this
     * interrupt.
     */
    else
    {
      /* Disable the interrupt using a read, modify, write procedure */
      INTLOCK();
      enable_mask = inpdw( en_reg_address );
      outpdw( en_reg_address, ( enable_mask & ~( 1UL << vect_idx ) ) ); 
      INTFREE()

      /* Call the registered ISR */
      (tramp_isr_tbl[tbl_idx].isr_ptr) (tbl_idx);  

      /* Clear the interrupt */
      outpdw( clear_reg_address, (1 << vect_idx) );

      /* If the interrupt has not been set to disable by calling
       * tramp_set_isr from some context, then enable the interrupt
       * again.
       */
      if( (tramp_invoke_isr_ptr_type)tramp_isr_tbl[tbl_idx].isr_ptr !=
          (tramp_invoke_isr_ptr_type)tramp_default_isr )
      {
        /* Enable interrupt using a read, modify, write procedure */
        INTLOCK();
        enable_mask = inpdw( en_reg_address );
        outpdw( en_reg_address, ( enable_mask | ( 1UL << vect_idx ) ) );
        INTFREE();
      }
    }

    /* Increment the count so that we know how many times this
     * interrupt has fired.
     */
    tramp_isr_tbl[tbl_idx].cnt++;

    vect_idx = HWIO_INM(IRQ_VEC_INDEX_PEND_RD,
                        HWIO_RMSK(IRQ_VEC_INDEX_PEND_RD));
    /* Lock interrupt after reading index pending register */
    rex_int_lock();
  } /* end while */
} /* end tramp_isr */


/*===========================================================================

FUNCTION     TRAMP_INIT_REX

DESCRIPTION
  This function programs tramp_isr() to be called by REX whenever there is
  an interrupt in the System.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tramp_init_rex( void )
{ 
  /* Set tramp_isr to be called whenever there is an IRQ interrupt */
  rex_set_interrupt_vector( P_IRQ_VECT, tramp_isr );

  /* Enable the IRQ interrupt */
  rex_enable_interrupt(P_IRQ_VECT);

  MSG_LOW("Interrupt Trampoline initialized", 0, 0, 0);
} /* end tramp_init_rex */


/*=========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION     TRAMP_INIT

DESCRIPTION
  This function sets all ISRs to the default ISR, initializes the PIC
  hardware and calls the appropriate OS initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tramp_init( void )
{
  /* Counter */
  uint32 i;

  /* All isr to default */
  for ( i = 0; i < TRAMP_NUM_PIC_INTS; i++ )  
  {
    tramp_isr_tbl[i].isr_ptr = (tramp_invoke_isr_ptr_type)tramp_default_isr;
  }

#ifdef FEATURE_2ND_LVL_INTR
  /* All isr to default */
  for ( i = 0; i < (TRAMP_NUM_INTS - TRAMP_NUM_PIC_INTS); i++ )  
  {
    tramp_sub_isr_tbl[i].isr_ptr = (tramp_invoke_isr_ptr_type)tramp_default_isr;
  }
#endif /* FEATURE_2ND_LVL_INTR */

  /* Init processor specific hardware */
  tramp_init_proc_spec();

  /* Init hardware */
  tramp_init_hardware();

  /* Turn on ability to change the enable masks */
  tramp_is_shutdown = FALSE;

  /* Call OS specific init */
  tramp_init_rex();

} /* end tramp_init */


/*===========================================================================

FUNCTION     TRAMP_SET_ISR

DESCRIPTION
  This function is used to set an ISR for a particular interrupt
  source.  This function enables or disables the mask corresponding to
  this interrupt in the IRQ_ENABLE register depending if the interrupt
  is being set or disabled.

  If the pointer to the ISR is NULL, the interrupt is being disabled.

DEPENDENCIES
  tramp_init has been called or if we have power collapsed and
  restored then tramp_restore has been called.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tramp_set_isr
(
  /* Interrupt number */ 
  tramp_isr_type           which_isr,
  /* ISR to be installed for this interrupt */
  tramp_isr_ptr_type       isr_ptr
)
{

  /* Address of enable register for this interrupt */
  volatile uint32 *enable_address;

  /* Mask of the enable register */
  uint32 enable_mask;

  /* Address of clear register for this interrupt */
  volatile uint32 *clear_address;

  /* Holder of old isr */
  tramp_invoke_isr_ptr_type old_isr;

  /* Interrupt information */
  tramp_int_info_type  int_info;

  if( tramp_is_shutdown )
  {
    MSG_ERROR( "Tramp in restart mode, all interrupts disabled", 0, 0, 0 );
    return;
  }

  /* Check for an invalid interrupt */
  if( which_isr >= TRAMP_NUM_INTS )
  {
    ERR_FATAL( "Invalid interrupt specified for tramp_set_isr", 0, 0, 0 );
    return;
  }

  /* Disable interrupts while changing table and PIC registers */
  INTLOCK();
 
  /* Store ISR pointer in interrupt information */
  int_info.isr_ptr = (tramp_invoke_isr_ptr_type)isr_ptr; 

  /* If passed-in ISR is NULL, change it to tramp_default_isr */
  if( int_info.isr_ptr == NULL )
  { 
    int_info.isr_ptr = (tramp_invoke_isr_ptr_type)tramp_default_isr;
  }

  /* Store ISR number in interrupt information */
  int_info.int_num = which_isr;

#ifdef FEATURE_2ND_LVL_INTR
  /* Determine if this interrupt is processed by the PIC directly or
   * if it has been consolidated with other interrupts 
   */
  if( int_info.int_num >= TRAMP_NUM_PIC_INTS )
  {
    /* This indicates an interrupt that has been consolidated with one
     * or more other interrupts. Set up this interrupt then determine 
     * what the "int_num" should be. If the consolidated interrupt is
     * being disabled (isr_ptr = NULL) BUT there are other 
     * interrupts enabled for the "PIC" isr then isr_ptr will not be
     * NULL upon returning from tramp_set_sub_isr. 
     */
    tramp_set_sub_isr(&int_info);

    if (int_info.int_num<0 || int_info.int_num>=TRAMP_NUM_PIC_INTS) {
      ERR_FATAL("Tramp index is out of bound", 0, 0, 0);
      return;
    }
  }
#endif /* FEATURE_2ND_LVL_INTR */

  /* Get the enable register address for this interrupt from the
   * interrupt table
   */
  enable_address = tramp_isr_tbl[int_info.int_num].enable_reg;

  /* Grab the old isr pointer */
  old_isr = tramp_isr_tbl[int_info.int_num].isr_ptr;

  /* Load our local table with the function ptr */
  tramp_isr_tbl[int_info.int_num].isr_ptr = int_info.isr_ptr;

  /* Get the clear register address for this interrupt from the
   * interrupt table
   */
  clear_address = tramp_isr_tbl[int_info.int_num].clear_reg;

  /* Determine which mask register we need to adjust */
  if( (TRAMP_NUM_PIC_INTS - int_info.int_num - 1) >= NUM_INT_BITS_IN_REG )
  {
    /* Invert index and convert to bit offset in register */
    int_info.int_num = (tramp_isr_type)(
                         (TRAMP_NUM_PIC_INTS - int_info.int_num) - 
                         NUM_INT_BITS_IN_REG - 1 );
  }
  else
  {
    /* Invert index */
    int_info.int_num = (tramp_isr_type)(
                         TRAMP_NUM_PIC_INTS - int_info.int_num - 1 );
  } 

  /* Enable or Disable the interupt appropriately.  This is done using
   * a read, modify, write procedure with interrupts locked
   */
  if( int_info.isr_ptr == (tramp_invoke_isr_ptr_type)tramp_default_isr )
  {
    enable_mask = inpdw( enable_address );
    outpdw( enable_address, ( enable_mask & ~( 1UL << int_info.int_num ) ) );   

    /* Clear on disable */
    outpdw( clear_address, (1UL << int_info.int_num) );
  }
  else
  {
    if( old_isr == (tramp_invoke_isr_ptr_type)tramp_default_isr)
    {
      outpdw( clear_address, (1UL << int_info.int_num) );
    }

    enable_mask = inpdw( enable_address );
    outpdw( enable_address, ( enable_mask | ( 1UL << int_info.int_num ) ) );
  }

  INTFREE();

} /* end tramp_set_isr */


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
void tramp_block_till_deasserted
(
  tramp_isr_type which_isr,        /* Select which ISR is being checked    */
  dword timeout_us                 /* Timeout (in microsec) for blocking   */
)
{
}


/*===========================================================================

FUNCTION TRAMP_IS_ANY_INTERRUPT_PENDING

DESCRIPTION
  Find out if there is any waiting to be serviced.  It only makes sense
  to call this function from a context where the interrupt in question is
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
boolean tramp_is_any_interrupt_pending(void)
{
  /* Check if any interrupt is pending and enabled. */
  if ( (HWIO_IN(IRQ_STATUS_0) & HWIO_IN(IRQ_EN_0)) ||
       (HWIO_IN(IRQ_STATUS_1) & HWIO_IN(IRQ_EN_1)) ||
       (HWIO_IN(FIQ_STATUS_0) & HWIO_IN(FIQ_EN_0)) ||
       (HWIO_IN(FIQ_STATUS_1) & HWIO_IN(FIQ_EN_1)) )
  {
    return TRUE;
  }

  /* Nothing pending */
  return FALSE;
}


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
  FALSE - the intterupt is not waiting to be serviced or invalid interrupt

SIDE EFFECTS
  None

===========================================================================*/
boolean tramp_is_interrupt_pending
(
  /* Interrupt number */
  tramp_isr_type int_num
)
{
  /* Interrupt mask to test in the status register */
  uint32 mask_val;

  /* Value read from the register */
  uint32 val;

  /* Check for an invalid interrupt */
  if( int_num >= TRAMP_NUM_INTS )
  {
    MSG_ERROR("invalid interrupt specified", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_2ND_LVL_INTR
  /* Check to see if a interrupt is a consolidated interrupt */
  if( int_num >= TRAMP_NUM_PIC_INTS )
  {
    /* Adjust interrupt number to index into sub interrupt table */
    int_num -= TRAMP_NUM_PIC_INTS;

    /* Read current status using the stored status register address */
    val = inpdw( tramp_sub_isr_tbl[int_num].status_reg );
   
    /* Get mask value from sub interrupt table */
    mask_val = tramp_sub_isr_tbl[int_num].status_bit; 
  }
  else
#endif /* FEATURE_2ND_LVL_INTR */
  {
    /* Read current status using the stored status register address */
    val = inpdw( tramp_isr_tbl[int_num].status_reg );

    /* Determine which mask register we need to adjust */
    if( (TRAMP_NUM_PIC_INTS - int_num - 1) >= NUM_INT_BITS_IN_REG )
    {
      /* Invert index and convert to bit offset in register */
      int_num = (tramp_isr_type)(
                 (TRAMP_NUM_PIC_INTS - int_num) - 
                  NUM_INT_BITS_IN_REG - 1 );
    }
    else
    {
      /* Invert index */
      int_num = (tramp_isr_type)(
                 TRAMP_NUM_PIC_INTS - int_num - 1 );
    } 
    mask_val = (1 << int_num);
  }
  
  return ( ((mask_val & val) != 0) ? TRUE : FALSE );

} /* end tramp_is_interrupt_pending */


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
void tramp_clear_interrupt
(
  tramp_isr_type int_num
)
{  
#ifdef FEATURE_2ND_LVL_INTR
  /* Index into sub interrupt table */
  tramp_isr_type sub_idx;

  /* Mask used to update registers */
  uint32           mask;
#endif /* FEATURE_2ND_LVL_INTR */

  /* Clear register address */
  volatile uint32 *clear_reg;

  /* Check for an invalid interrupt */
  if( int_num >= TRAMP_NUM_INTS )
  {
    MSG_ERROR("invalid interrupt specified", 0, 0, 0);
    return;
  }
#ifdef FEATURE_2ND_LVL_INTR
  /* Check if this is a sub-interrupt */
  else if( int_num >= TRAMP_NUM_PIC_INTS )
  {
    /* Compute the sub interrupt index */
    sub_idx = (tramp_isr_type)( int_num - TRAMP_NUM_PIC_INTS );

    /* If there is a valid clear register for the interrupt */
    if( tramp_sub_isr_tbl[sub_idx].clear_reg )
    {
      /* Determine which bit to clear */
      mask = tramp_sub_isr_tbl[sub_idx].clr_bit;
      /* Clear the interrupt */
      outpdw(tramp_sub_isr_tbl[sub_idx].clear_reg, mask );
    }
  
    /* Set the interrupt number to the PIC interrupt for this
     * consolidated interrupt so can clear that too
     */
    int_num = (tramp_isr_type)( tramp_sub_isr_tbl[sub_idx].pic_int_num );
  
    if (int_num<0 || int_num>=TRAMP_NUM_PIC_INTS) {
      ERR_FATAL("Tramp index is out of bound", 0, 0, 0);
      return;
    }
  }
#endif /* FEATURE_2ND_LVL_INTR */

  clear_reg = tramp_isr_tbl[int_num].clear_reg;
  
  /* Determine which mask register we need to adjust */
  if( (TRAMP_NUM_PIC_INTS - int_num - 1) >= NUM_INT_BITS_IN_REG )
  {
    /* Invert index and convert to bit offset in register */
    int_num = (tramp_isr_type)(
                (TRAMP_NUM_PIC_INTS - int_num) - 
                NUM_INT_BITS_IN_REG - 1 );
  }
  else
  {
    /* Invert index */
    int_num = (tramp_isr_type)(
                TRAMP_NUM_PIC_INTS - int_num - 1 );
  } 

  outpdw( clear_reg, (1 << int_num) );

} /* end tramp_clear_interrupt */


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
void tramp_set_interrupt
(
  tramp_isr_type int_num
)
{
  
  /* Interrupt set register address */
  volatile uint32 *set_int_addr;

  /* Interrupt detect register address */
  volatile uint32 *detect_addr;

  /* Mask of the detection register */
  uint32 detect_mask;

  /* Detection type for this interrupt */
  tramp_detect_type detect_val;

  /* Indicator of need for software fix */
  boolean soft_fix = FALSE;
  
  /* Check for an invalid interrupt */
  if( int_num >= TRAMP_NUM_INTS )
  {
    MSG_ERROR("invalid interrupt specified", 0, 0, 0);
    return;
  }

#ifdef FEATURE_2ND_LVL_INTR
  /* Check to see if a interrupt is a consolidated interrupt */
  if( int_num >= TRAMP_NUM_PIC_INTS )
  {
    /* Adjust interrupt number to index into sub interrupt table */
    int_num -= TRAMP_NUM_PIC_INTS;

    /* Get the consolidated interrupt index from sub interrupt table */
    int_num = (tramp_isr_type)tramp_sub_isr_tbl[int_num].pic_int_num; 
    
    /* Check for an invalid interrupt */
    if( int_num >= TRAMP_NUM_PIC_INTS ) {
      ERR_FATAL( "Invalid interrupt specified for tramp_set_isr", 0, 0, 0 );
      return;
    }
  }
#endif /* FEATURE_2ND_LVL_INTR */

  /* Get the software interrupt set register address */
  set_int_addr = tramp_isr_tbl[int_num].set_reg;

  /* Get the detection type register address */
  detect_addr = tramp_isr_tbl[int_num].detect_reg;

  /* Get the detection type for this interrupt from the interrupt table */
  detect_val = tramp_isr_tbl[int_num].edge_or_level;

  /* Determine which mask register we need to adjust */
  if( (TRAMP_NUM_PIC_INTS - int_num - 1) >= NUM_INT_BITS_IN_REG )
  {
    /* Invert index and convert to bit offset in register */
    int_num = (tramp_isr_type)(
                (TRAMP_NUM_PIC_INTS - int_num) - 
                NUM_INT_BITS_IN_REG - 1 );
  }
  else
  {
    /* Invert index */
    int_num = (tramp_isr_type)(TRAMP_NUM_PIC_INTS - int_num - 1 );
  } 

  /* SOFTWARE FIX: DOES NOT ALLOW LEVEL INTS TO BE SOFTWARE TRIGGERED.
   * If there is a edge or level detection register for this interrupt 
   * and the interrupt is level detect, read the value and then set
   * the correct bit to make the interrupt an edge detect.
   */
  if( detect_addr != NULL )
  {
    if( detect_val == TRAMP_LEVEL_DETECT )
    {
      detect_mask = inpdw( detect_addr );
      detect_mask |= ( 1UL << int_num );
      outpdw( detect_addr, detect_mask );

      /* Store the fact that we are using the software fix so don't
       * have to do nested if later.
       */
      soft_fix = TRUE;
    }
  }

  /* Write to the correct bit in the interrupt set register */
  outpdw( set_int_addr, (1 << int_num) );

  /* SOFTWARE FIX: DOES NOT ALLOW LEVEL INTS TO BE SOFTWARE TRIGGERED.
   * If there is a edge or level detection register for this interrupt 
   * and the interrupt is level detect, read the value and then set
   * the correct bit to make the interrupt a level detect again.
   */
  if( soft_fix )
  {
    detect_mask = inpdw( detect_addr );
    detect_mask &= ~( 1UL << int_num );
    outpdw( detect_addr, detect_mask );
  }

} /* end tramp_set_interrupt */


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
void tramp_shutdown()
{

  /* Loop counter */
  uint32 iterations;

  /* Set flag to indicate we are in shutdown mode. */
  tramp_is_shutdown = TRUE;

  /* Disable IRQ */
  HWIO_OUT( IRQ_EN_0, DISABLE_ALL_MASK );
  HWIO_OUT( IRQ_EN_1, DISABLE_ALL_MASK );

  /* Read back the ENABLE registers so that the HW has enough time to
   * update internally.
   */
  (void)HWIO_IN( IRQ_EN_0 );
  (void)HWIO_IN( IRQ_EN_1 );

  /* Loop through and clear out all pending interrupts.  Loop through
   * the TRAMP_PRIO_LVL_MAX.
   */
  for( iterations = 0; iterations < TRAMP_PRIO_LVL_MAX; iterations++ )
  {
    (void)HWIO_INM(IRQ_VEC_INDEX_PEND_RD, HWIO_RMSK(IRQ_VEC_INDEX_PEND_RD));
  }

} /* end tramp_shutdown */


