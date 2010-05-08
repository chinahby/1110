;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                   B O O T    R E S E T    H A N D L E R
;
; GENERAL DESCRIPTION
;   This module contains the reset handler function that is executed after
;   reset of the ARM processor.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
; Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when      who     what, where, why
; --------  -----   ---------------------------------------------------------
; 10/18/05  rp      Moved boot_hw_powerdown() into this file from
;                   boot_error_handler.s
; 07/23/05  wd      L4 Merge.
; 05/09/05  rp      Created from boot_reset_handler.                             
;============================================================================

;============================================================================
;
;                             MODULE INCLUDES
;
;============================================================================

#include "customer.h"
#include "armasm.h"
#include "msm.h"
;=======================================================================
;
;                             MODULE DATA 
;
;=======================================================================

;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

#ifndef FEATURE_L4
        IMPORT  mmu_clean_and_invalidate_data_cache
        IMPORT  mmu_invalidate_instruction_cache
#endif
        IMPORT  cache_mmu_disable
        IMPORT  boot_hw_powerdown
;============================================================================
        AREA    Misc_target_code, CODE, READONLY
        CODE16                             ; 16 bit Thumb instruction set
#ifdef FEATURE_USES_CLK_BUSY_WAIT_NOPS
;----------------------------------------------------------------------------
; CLK_BUSY_WAIT_NOP_LOOPS
;
;  DESCRIPTION - Delays the system for the given number of microseconds
;  DEPENDENCY  - Interrupts should be locked by caller
;
;----------------------------------------------------------------------------
clk_busy_wait_nop_loops
        EXPORT  clk_busy_wait_nop_loops
        ALIGN
        lsr  r0, r0, #0x2    ; divide by 4 to calculate the loop count
nop_loops        
        nop
        nop
        sub  r0, r0, #1
        bgt  nop_loops
        bx   lr
#endif
        EXPORT  jump_armprg
        ALIGN
jump_armprg
         mov r7,r0
#ifndef FEATURE_L4
         ldr r0,=mmu_clean_and_invalidate_data_cache
         blx r0
         ldr r0,=mmu_invalidate_instruction_cache
         blx r0
         ldr r0,=cache_mmu_disable
         blx r0
         bx r7
#else /* FEATURE_L4 */
#error code not present
#endif

;----------------------------------------------------------------------------
; BOOT POWERDOWN ENTRY POINT
;
; DESCRIPTION
;   The boot powerdown routine is entered in the event of a return from the
;   boot kernel.
;----------------------------------------------------------------------------

 LEAF_NODE boot_powerdown_entry

        ; Disable IRQ and FIQ interrupts in the event that the boot image
        ; has been entered from the application with interrupts enabled.
        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask

#if defined FEATURE_STACK_CHECK
        eor     sl, sl, sl
#endif

        ; Enter the boot powerdown routine.  There is no return from this
        ; routine.

        ldr     r4, =boot_hw_powerdown
        bx      r4
 LEAF_NODE_END


;============================================================================
;============================================================================
        END
