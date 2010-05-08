#ifndef REXARM_H
#define REXARM_H
/*==========================================================================

                     REX ARM Definitions Header File

DESCRIPTION
  Definitions for offsets into various REX and ARM data structures.  This
  is reused by REX ARM assembly files
  
Copyright (c) 1997-2003, 2007 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/core/kernel/rex/main/latest/rexarm.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/07   cz      Added support for DAC based memory protection feature from umr.
06/30/03    gr     Updated field offsets.
03/20/02    dc     Added support for FEATURE_SOFTWARE_PROFILE
06/06/01    gr     Defined FEATURE_REX_APC if FEATURE_REX_TIMER_EX is
                   defined.
11/17/00   crr     Increased interrupt frame size an offsets to handled 
                   nested interrupts. Deleted duplicate #define of TCB_LED_OFFSET
02/19/99    ms     Redefined some offsets in REX_INTERRUPT_FRAME to match with
                   the new context frame. 
02/12/99    ms     Added REX_TCB_LED_OFFSET definition for TIMETEST.
01/18/99    ms     Moved ARM_TRAP_FRAME related #defines to boot_trap.h
12/16/98   jct     Changed this file from genarm.h to rexarm.h as there was
                   no general purpose code in this any longer
12/10/98   jkl     Included r12 in the context frame.
11/02/98   ms      Revised for new context.
09/13/98   hcg     Renamed trap frames.
07/10/98   jct     Revised for coding standard, removed unused code
01/01/98   bwj     Created

===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "target.h"
#include "customer.h"

/*===========================================================================
 
                             DEFINITIONS

===========================================================================*/

#if defined(FEATURE_STACK_CHECK) && defined(FEATURE_LOG_FUNCTIONS)
   #define REX_LOG_ENABLE  0x12345678
   #define REX_LOG_DISABLE 0xdeaddead
#endif

#ifdef FEATURE_REX_TIMER_EX
   /* The extended timer API requires APCs.
   */
   #ifndef FEATURE_REX_APC
      #define FEATURE_REX_APC
   #endif
#endif

#ifdef FEATURE_ZREX
#error code not present
#else
   #define LINK_FIELD_SIZE  0
   #define TCB_EXTRA_SIZE   0
#endif

/* Offsets in rex_tcb_type
*/
#define REX_TCB_SIZE                    ( 0x18 + TCB_EXTRA_SIZE               )
#define REX_TCB_STACK_POINTER_OFFSET    ( LINK_FIELD_SIZE                     )
#define REX_TCB_STACK_LIMIT_OFFSET      ( 0x04 + REX_TCB_STACK_POINTER_OFFSET )
#define REX_TCB_SLICES_OFFSET           ( 0x04 + REX_TCB_STACK_LIMIT_OFFSET   )
#define REX_TCB_SIGS_OFFSET             ( 0x04 + REX_TCB_SLICES_OFFSET        )
#define REX_TCB_WAIT_OFFSET             ( 0x04 + REX_TCB_SIGS_OFFSET          )
#define REX_TCB_PRI_OFFSET              ( 0x04 + REX_TCB_WAIT_OFFSET          )

#ifdef FEATURE_DAC_MEMORY_PROTECTION
  #define REX_TCB_DAC_OFFSET            ( 0x04 + REX_TCB_PRI_OFFSET           )
#else
  #define REX_TCB_DAC_OFFSET            ( REX_TCB_PRI_OFFSET                  )
#endif

#ifdef FEATURE_REX_PROFILE
   #define REX_TCB_TIME_SAMPLE_OFFSET   ( 0x04 + REX_TCB_DAC_OFFSET           )
   #define REX_TCB_MAX_INTLOCK_OFFSET   ( 0x04 + REX_TCB_TIME_SAMPLE_OFFSET   )
#ifdef TIMETEST
   #define REX_TCB_LED_OFFSET           ( 0x04 + REX_TCB_MAX_INTLOCK_OFFSET   )
#ifdef FEATURE_SOFTWARE_PROFILE
   #define REX_TCB_NUMTICKS_OFFSET      ( 0x04 + REX_TCB_LED_OFFSET           )
#endif
#else /* TIMETEST */
#ifdef FEATURE_SOFTWARE_PROFILE
   #define REX_TCB_NUMTICKS_OFFSET      ( 0x04 + REX_TCB_MAX_INTLOCK_OFFSET   )
#endif
#endif /* TIMETEST */
#else /* FEATURE_REX_PROFILE */
#ifdef TIMETEST
   #define REX_TCB_LED_OFFSET           ( 0x04 + REX_TCB_DAC_OFFSET           )
#ifdef FEATURE_SOFTWARE_PROFILE
   #define REX_TCB_NUMTICKS_OFFSET      ( 0x04 + REX_TCB_LED_OFFSET           )
#endif
#else /* TIMETEST */
#ifdef FEATURE_SOFTWARE_PROFILE
   #define REX_TCB_NUMTICKS_OFFSET      ( 0x04 + REX_TCB_DAC_OFFSET           )
#endif
#endif /* TIMETEST */
#endif /* FEATURE_REX_PROFILE */


/* Context frame information.
*/
#define REX_CF_SIZE                     0x40

/* Offsets in rex_timer_type
*/
#define REX_TIMER_SIZE                  0x24
#define REX_TIMER_CNT_OFFSET            0x10
#define REX_TIMER_TCB_PTR_OFFSET        0x18
#define REX_TIMER_SIGS_OFFSET           0x1C

/* Offsets in REX_STACK_FRAME
*/
#define REX_STACK_FRAME_SIZE            0x8
#define REX_STACK_FRAME_LR_OFFSET       0x0
#define REX_STACK_FRAME_SP_OFFSET       0x4

/* Offsets in REX_INTERRUPT_FRAME
*/
#define REX_INTERRUPT_FRAME_SIZE        0x40
#define REX_INTERRUPT_FRAME_SPSR_OFFSET 0x00
#define REX_INTERRUPT_FRAME_R_OFFSET    0x04
#define REX_INTERRUPT_FRAME_LR_OFFSET   0x38       
#define REX_INTERRUPT_FRAME_PC_OFFSET   0x3c

#endif /* END REXARM_H */

