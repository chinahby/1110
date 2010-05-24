;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                   BOOT EXCEPTION HANDLERS
;
; GENERAL DESCRIPTION
;   This module contains functions and data blocks pertaining to the Bootstrap
;   process which should reside in the permanent (non-erasable) Boot Block
;   area.  It is responsible for setting up the hardware chip select
;   registers, verifying the ROM image,  performing RAM tests, zeroing out the
;   main RAM area, setting up default interrupt vectors and ultimately passing
;   control to the boot code in main ROM.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   Routines within this module automatically obtain control of the processor
;   following a reset. Control may also return to this module if main() exits
;   unexpectedly, if an unexpected hardware or software interrupt occurs, or
;   if a fatal error occurs.
;
; Copyright (c) 2004,2005 QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; when      who     what, where, why
; --------  -----   ---------------------------------------------------------
; 09/14/05  tbg     Added FEATURE_DETECT_SOFT_RESETS
; 07/11/05  ly      Added back support for BREW Debugger
; 05/06/05  tbg     Added support for error handlers to change processor modes
;                   when needed
; 01/24/05  rp      Merged in BREW debugger suuport code from 6550
; 01/24/05  rp      Fixed a memory leak problem in progressive boot
;                   abort handler
; 12/06/04  ts      Removed changes for spurious data aborts.
; 09/21/04   rp      Added Progressive boot support
; 06/24/04  bgc     Added handler for MSM6550 rev A spurious data aborts.
; 04/15/04  AP      Ported for MSM6550 from MSM6500.
;============================================================================
;============================================================================
;
;                           MODULE INCLUDE FILES
;
;============================================================================

#include "boot_trap.h"
#include "armasm.h"
#include "boot_stacks.h"
#ifndef FEATURE_THINUI
#include "OEMFeatures.h"
#endif
#ifdef FEATURE_PROGRESSIVE_BOOT
#include "target.h"
#include "customer.h"
#endif

#ifdef FEATURE_PROGRESSIVE_BOOT
FSR_STATUS_PAGE_PERMISSION_MASK    EQU 0x0000000d
#endif
#ifdef FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS
FSR_STATUS_ALIGNMENT_MASK          EQU 0x0000000d
FSR_STATUS_ALIGNMENT_VALUE         EQU 0x00000001
MMU_ENDIANNESS_MASK                EQU 0x00000080
MMU_LITTLE_ENDIAN_VALUE               EQU 0x00000000
#endif 
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
        IMPORT  abort_stack
        IMPORT  undef_stack
        IMPORT  boot_default_exception_handler        
        IMPORT  boot_default_signal_handler                
        IMPORT  boot_error_handler
        IMPORT  rex_jettison_core
        IMPORT  __default_signal_handler_c
#ifdef FEATURE_PROGRESSIVE_BOOT
        IMPORT  rex_save_context
        IMPORT  rex_curr_task
        IMPORT  pager_queue_next_page_request
        IMPORT  rex_sched_without_save
        IMPORT  pager_tcb
        IMPORT  pager_is_task_started
        IMPORT  pager_all_pages_loaded
        IMPORT  pager_load_page
        IMPORT  boot_error_handler
#endif
#ifdef FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS
        IMPORT  saved_abort_stack_pointer
#endif /*FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS */ 
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================
#ifdef FEATURE_PROGRESSIVE_BOOT
        EXPORT Progressive_Boot_Abort_Handler
#endif
        EXPORT Abort_Handler
        EXPORT Undefined_Handler
        EXPORT SWI_Handler
        EXPORT Prefetch_Handler
        EXPORT Reserved_Handler
        EXPORT __default_signal_handler
        EXPORT _sys_exit
        EXPORT boot_restore_exception_frame
        EXPORT boot_set_abort_mode
#if defined FEATURE_ERR_EXTENDED_STORE && defined FEATURE_DETECT_SOFT_RESETS
        EXPORT Soft_Reset_Handler
#endif
#ifdef FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS
        EXPORT boot_restore_misaligned_exception_frame
        EXPORT boot_is_data_abort_misaligned_access
        EXPORT boot_get_endianness
#endif /*FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS */


#ifdef FEATURE_PROGRESSIVE_BOOT

;============================================================================
; PROGRESSIVE BOOT ABORT HANDLER
;
;  DESCRIPTION - Called to handle all memory aborts. The cause could be
;  Data abort or pre-fetch abort due to page fault, mis-aligned data access
;  Here we put an exception_frame on the stack, call
;  into C to handle the abort, and then restore the exception frame to
;return
;============================================================================
	PRESERVE8
        AREA BOOTSYS_BOOT_CODE, CODE
        CODE32

Progressive_Boot_Abort_Handler
; Disable interrupts
        msr     CPSR_c, #PSR_Abort:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        sub     lr, lr, #8
        ; save exception frame.
        ;  Save supervisor mode pc
        sub     sp, sp, #ARM_TRAP_FRAME_SIZE
        str     lr, [sp, #ARM_TRAP_FRAME_PC_OFFSET]
        stmib   sp, {r0-r12}                         ;
        mrs     r12, SPSR
        str     r12, [sp, #ARM_TRAP_FRAME_SPSR_OFFSET] ;  Save SPSR
        ; switch to SVC mode to get SP and LR
        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        mov     r0, lr
        mov     r1, sp
;switch back to abort mode
        msr     CPSR_c, #PSR_Abort:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        str     r0, [sp, #ARM_TRAP_FRAME_LR_OFFSET]
        str     r1, [sp, #ARM_TRAP_FRAME_SP_OFFSET]

; If the pager task has already completed, skip to normal abort handler
; Do not want to make function call here - access variable directly
        ldr    r0, =pager_all_pages_loaded
        ldr    r0, [r0]
        cmp    r0, #0
        bne    other_aborts

; See if the abort is due to a page fault. If it is due to a page fault,
; then we need to load the page containing the aborted address. If the
; abort is not due to a page fault, then go to the regular abort
; handling
; Read the fault status register to determine the cause of the fault.
; There are two fault status registers:
; 1. Data fault status register -- for data abort
; 2. Instruction fault status register -- for prefetch abort
; Both are read in conditional sequence to determine the cause of abort
;
; Read data fault status register  (data FSR)
        mrc p15, 0, r0, c5, c0, 0
        and r0, r0, #FSR_STATUS_PAGE_PERMISSION_MASK
        cmp r0, #FSR_STATUS_PAGE_PERMISSION_MASK
        beq abort_is_data
        mrc p15, 0, r0, c5, c0, 1
        and r0, r0, #FSR_STATUS_PAGE_PERMISSION_MASK
        cmp r0, #FSR_STATUS_PAGE_PERMISSION_MASK
        beq abort_is_inst
        b   other_aborts
abort_is_data
; Get the abort MVA from data fault address register
        mrc p15, 0, r4, c6, c0, 0
; Clear the data fault status register
        ldr     r0, =0x0
        mcr     p15, 0, r0, c5, c0, 0
        b   check_load_context
abort_is_inst
; The aborted instruction is lr-4. Since we subtracted 8 from lr above
; add 4 to lr to get the correct address
        add   lr, lr, #4
        str   lr, [sp, #ARM_TRAP_FRAME_PC_OFFSET]   ;  Save supervisor
        mov   r4, lr
; Clear the instruction fault status register
;
        ldr     r0, =0x0
        mcr     p15, 0, r0, c5, c0, 1
check_load_context
; check if the abort happened in interrupt context
; error fatal if yes
        and   r12, r12, #PSR_Mode_Mask
        cmp   r12, #PSR_Supervisor    ; If not in Supervisor mode
        bne   boot_error_handler               ;  jump to error
        
; If the abort happened before any tasks have been started
; load the page in the abort handler context itself
        ldr    r0, =rex_curr_task
        ldr    r0, [r0]
        mov    r5, r0                    ; Save  current task pointer
        cmp    r0, #0
        beq    load_in_abort_context
; If the pager task is not started. Load the page in abort context
        ldr    r0, =pager_is_task_started
        blx    r0
        cmp    r0, #0
        bne    load_in_task_context
load_in_abort_context
        mov    r0, r4
        ldr    r1, =pager_load_page
        blx    r1
        b      restore_exception_frame
load_in_task_context
        ldr     r0, =rex_save_context
        blx     r0
; Adjust the stack pointer back to the proper value
        add     sp, sp, #ARM_TRAP_FRAME_SIZE
; Get the tcb pointer to the current task and send the aborted MVA and
; the tcb pointer to the pager task
       mov      r0, r5   ; task pointer
       mov      r1, r4   ; virtual address
       ldr      r2, =pager_queue_next_page_request
       blx      r2
; Schedule the pager task to page in a new page containing the aborted
; address
       b        rex_sched_without_save  ; does not return
other_aborts
        nop
        ; restore registers for original abort handler to process
        ldmib   sp, {r0-r12}        
        nop
        add     sp, sp, #ARM_TRAP_FRAME_SIZE
        ; mov     a1, sp
        b       Abort_Handler
restore_exception_frame
        ; restore exception frame.
        ldr     r0, [sp, #ARM_TRAP_FRAME_SPSR_OFFSET]    ;  Reload  SPSR
        msr     SPSR_f, r0
        msr     SPSR_c, r0
        ldmib   sp, {r0-r12}
        nop
        ;  Use pc as supervisor mode lr.
        ldr     lr, [sp, #ARM_TRAP_FRAME_PC_OFFSET]
        add     sp, sp, #ARM_TRAP_FRAME_SIZE

        movs    pc, lr          ; return after restoring exception frame.

#endif

;============================================================================
; BOOT ABORT HANDLER
;
;  DESCRIPTION - Called to handle all memory aborts. The only current cause is
;    mis-aligned data access. Here we put an exception_frame on the stack, call
;    into C to handle the abort, and then restore the exception frame to return.
;============================================================================
	PRESERVE8
        AREA BOOTSYS_BOOT_CODE, CODE
        CODE32                  ; 32 bit ARM instruction set

Abort_Handler

#ifdef T_QSC1100
        ldr     r4, [sp], #4   ; pop out r4 from stack used by pbl_abort_handler
#endif

        ; Save lr to stack 
        stmfd   sp!, {lr}
	; Call rex_jettison_core to save register values
        bl      rex_jettison_core                
#ifdef FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS
        ; Save abort stack pointer; this will be used later if we
        ; need to handle the data abort due to misaligned access
        mov     r0, sp
        add     r0, r0, #4 
        ldr     r1, = saved_abort_stack_pointer
        str     r0, [r1]
#endif /* FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS */        
#if defined(FEATURE_STACK_CHECK)
        pusha   sp, sl
        ldr     sl, =abort_stack
#endif
        ; We calculate where the stack pointer should be right now  so
        ; we can check to see if this is a re-entrant call.
        ldr     r2, =abort_stack+Abort_Stack_Size
        sub     r2, r2, #4
#if defined FEATURE_STACK_CHECK
        sub     r2, r2, #4
#endif
        cmp     r2, sp
        ldrne   r0, =re_entrant_abort_msg
        bne     boot_error_handler

        ; Put exception type and r14 value in r0, r1 to pass to handler
        mov     r0, #ARM_CAUSE_DATA_ABORT
        ldmfd   sp, {r1} ; (stack pointer not adjusted on purpose -- allows reentrant checks)
        
        b       boot_default_exception_handler ; will not return


;============================================================================
;
; BOOT UNDEFINED HANDLER
;
; DESCRIPTION
;   The default undefined handler prints an error message and then spins 
;   until power is removed by the user.  
;
; FORMAL ARGUMENTS
;   None
;
; DEPENDENCIES
;   None
;
; RETURN VALUE
;   None
;
; SIDE EFFECTS
;   None
;
;============================================================================

	PRESERVE8
        AREA BOOTSYS_BOOT_CODE, CODE
        CODE32                  ; 32 bit ARM instruction set

default_undefined_handler

#ifdef T_QSC1100
        ldr     r4, [sp], #4   ; pop out r4 from stack used by pbl_undefined_handler
#endif

        ; Save lr to stack 
        stmfd   sp!, {lr}
        ; Call rex_jettison_core to save register values
        bl      rex_jettison_core                
        
        ; Put exception type and r14 value in r0, r1 to pass to handler
        mov     r0, #ARM_CAUSE_UNDEF_INSTR
        ldmfd   sp!, {r1}
        
        ; Call handler
        b       boot_default_exception_handler ; will not return

Undefined_Handler
#if defined (FEATURE_BREW_DEBUGGER)
    b UNDEF_Handler 
#else
    b default_undefined_handler
#endif 

;============================================================================
;
; UNDEF_Handler
;
; DESCRIPTION
;   This is the interrupt handler that is called when the BREW Debugger causes
;   an undefined interrupt. If the g_debugFlag is set, the interrupt 
;   would be handled by the BREW Debugger via brew_undef_handler.
;
; FORMAL ARGUMENTS
;   None
;
; DEPENDENCIES
;   None
;
; RETURN VALUE
;   None
;
; SIDE EFFECTS
;   None
;
;============================================================================
#if defined (FEATURE_BREW_DEBUGGER)
   IMPORT brew_undef_handler
   IMPORT g_debugFlag
   ALIGN
   ROUT
UNDEF_Handler
        stmfd sp!, {r7-r8}
   ; check the debug flag
        ldr r7, =g_debugFlag
        ldr r8, [r7]
        cmp r8, #1
        ldmfd sp!, {r7-r8}
   ; if debug_flag not set, branch to default_undefined_handler
         bne default_undefined_handler 
   ; else continue with handler after restoring the registers r7 and r8
         b brew_undef_handler
UNDEF_Handler_end
#endif

;============================================================================
;
; BOOT SWI HANDLER
;
; DESCRIPTION
;   The default software interrupt handler prints an error message and  
;   then spins until power is removed by the user.  
;
; FORMAL ARGUMENTS
;   None
;
; DEPENDENCIES
;   None
;
; RETURN VALUE
;   None
;
; SIDE EFFECTS
;   None
;
;============================================================================
SWI_Handler

#ifdef T_QSC1100
        ldr     r4, [sp], #4   ; pop out r4 from stack used by pbl_swi_handler
#endif

        ; Save lr to stack 
        stmfd   sp!, {lr}
	; Call rex_jettison_core to save register values
        bl      rex_jettison_core                
        
        ; Put exception type and r14 value in r0, r1 to pass to handler
        mov     r0, #ARM_CAUSE_SOFTWARE_INT
        ldmfd   sp, {r1} ; leave r14 on stack to help debug
        
        ; Call handler
        b       boot_default_exception_handler ; will not return




;============================================================================
;
; BOOT PREFETCH ABORT HANDLER
;
; DESCRIPTION
;   The default prefetch abort handler prints an error message and  
;   then spins until power is removed by the user.  
;
; FORMAL ARGUMENTS
;   None
;
; DEPENDENCIES
;   None
;
; RETURN VALUE
;   None
;
; SIDE EFFECTS
;   None
;
;============================================================================
	PRESERVE8
        AREA BOOTSYS_BOOT_CODE, CODE
        CODE32                  ; 32 bit ARM instruction set

Prefetch_Handler

#ifdef T_QSC1100
        ldr     r4, [sp], #4   ; pop out r4 from stack used by pbl_prefetch_abort_handler
#endif

        ; Save lr to stack 
        stmfd   sp!, {lr}
	; Call rex_jettison_core to save register values
        bl      rex_jettison_core                
        
        ; Put exception type and r14 value in r0, r1 to pass to handler
        mov     r0, #ARM_CAUSE_PREFETCH_ABORT
        ldmfd   sp!, {r1}
        
        ; Call handler
        b       boot_default_exception_handler ; will not return

;============================================================================
;
; BOOT RESERVED EXCEPTION HANDLER
;
; DESCRIPTION
;   The default reserved exception handler prints an error message and  
;   then spins until power is removed by the user.  
;
; FORMAL ARGUMENTS
;   None
;
; DEPENDENCIES
;   None
;
; RETURN VALUE
;   None
;
; SIDE EFFECTS
;   None
;
;============================================================================

Reserved_Handler

#ifdef T_QSC1100
        ldr     r4, [sp], #4   ; pop out r4 from stack used by pbl_reserved_handler
#endif

        ; Save lr to stack 
        stmfd   sp!, {lr}
	; Call rex_jettison_core to save register values
        bl      rex_jettison_core                
        
        ; Put exception type and r14 value in r0, r1 to pass to handler
        mov     r0, #ARM_CAUSE_RESERVED_INT
        ldmfd   sp, {r1} ; leave r14 on stack to help debug
        
        ; Call handler
        b       boot_default_exception_handler ; will not return

#if defined FEATURE_ERR_EXTENDED_STORE && defined FEATURE_DETECT_SOFT_RESETS
;============================================================================
;
; SOFT RESET HANDLER
;
; DESCRIPTION
;   Called if a soft reset (branch to zero) was detected by the boot code.
;
; FORMAL ARGUMENTS
;   None
;
; DEPENDENCIES
;   None
;
; RETURN VALUE
;   None
;
; SIDE EFFECTS
;   None
;
;============================================================================
Soft_Reset_Handler

        ; Save lr to stack 
        stmfd   sp!, {lr}
	; Call rex_jettison_core to save register values
        bl      rex_jettison_core                
        
        ; Put exception type and r14 value in r0, r1 to pass to handler
        mov     r0, #ARM_CAUSE_SOFT_RESET
        ldmfd   sp, {r1} ; leave r14 on stack to help debug
        
        ; Call handler
        b       boot_default_exception_handler ; will not return
#endif

#ifdef FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS

;============================================================================
; BOOT_RESTORE_MISALIGNED_EXCEPTION_FRAME
;
; DESCRIPTION 
;   Restores the exception frame after misaligned access
; DEPENDENCIES
;   None
; FORMAL ARGUMENTS
;   Pointer to the exception frame in R0
;
; RETURN VALUE
;   None
;
; SIDE EFFECTS
;   The exception frame is restored
;

;============================================================================

	PRESERVE8
        AREA BOOTSYS_BOOT_CODE, CODE
        CODE32                  ; 32 bit ARM instruction set

boot_restore_misaligned_exception_frame;
        mov     sp, r0
        mov     r2, r0
        ldr     r0, [sp, #ARM_TRAP_FRAME_SPSR_OFFSET]    ;  Reload  SPSR
; Switch to the mode that caused the data abort to restore the proper
; stack pointer
        mov     r1, r0
        and     r1, r1, #PSR_Mode_Mask
        orr     r1, r1, #PSR_Irq_Mask:OR:PSR_Fiq_Mask
        msr     CPSR_c, r1
        ldr     sp, [r2, #ARM_TRAP_FRAME_SP_OFFSET]
; Switch the mode back to abort
        msr     CPSR_c, #PSR_Abort:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        msr     SPSR_f, r0
        msr     SPSR_c, r0                                                                    
        ldmib   sp, {r0-r12}
        nop
        ;  Use pc as supervisor mode lr.
        ldr     lr, [sp, #ARM_TRAP_FRAME_PC_OFFSET]
; Restore Abort mode stack        
        ldr     sp, =saved_abort_stack_pointer
        ldr     sp, [sp]
        movs    pc, lr          ; return after restoring exception frame.



;============================================================================
; BOOT_IS_DATA_ABORT_MISALIGNED_ACCESS
;
; DESCRIPTION 
;   Checks if a data abort is due to misaligned access
; DEPENDENCIES
;   None
;
; RETURN VALUE
;   1 if misaligned access, 0 if not
;
; SIDE EFFECTS
;   None
;

;============================================================================

	PRESERVE8
        AREA BOOTSYS_BOOT_CODE, CODE
        CODE32                  ; 32 bit ARM instruction set

boot_is_data_abort_misaligned_access;
; Read data fault status register  (data FSR)
        mrc    p15, 0, r0, c5, c0, 0
        and    r0, r0, #FSR_STATUS_ALIGNMENT_MASK
        cmp    r0, #FSR_STATUS_ALIGNMENT_VALUE
        moveq  r0, #1
        movne  r0, #0
; Clear the data fault status register
        ldr    r1, =0x0
        mcr    p15, 0, r1, c5, c0, 0
        bx     lr


;============================================================================
; BOOT_GET_ENDIANNESS
;
; DESCRIPTION 
;   Returns the endianness
; DEPENDENCIES
;   None
;
; RETURN VALUE
;   0 if little endian, 1 if big endian
;
; SIDE EFFECTS
;   None
;

;============================================================================

	PRESERVE8
        AREA BOOTSYS_BOOT_CODE, CODE
        CODE32                  ; 32 bit ARM instruction set

boot_get_endianness;
; Read CP15 register
        mrc    p15, 0, r0, c1, c0, 0
        and    r0, r0, #MMU_ENDIANNESS_MASK
        cmp    r0, #MMU_LITTLE_ENDIAN_VALUE
        moveq  r0, #0
        movne  r0, #1
        bx     lr
 
#endif /*FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS */

;============================================================================
; __DEFAULT_SIGNAL_HANDLER
;
;  DESCRIPTION -  
;     C prototype - void __default_signal_handler(int,int)
;     This is a wrapper for __default_signal_handler_c().  It will disable
;     interrupts before calling the function.
;============================================================================
 LEAF_NODE __default_signal_handler
        ; Parameters passed in r0, r1 -- do not modify these values
        
        stmfd sp!, {r2}
        
       ; Disable interrupts
        mrs r2, CPSR
        orr r2, r2, #PSR_Irq_Mask:OR:PSR_Fiq_Mask
        msr CPSR_c, r2
        
        ldmfd sp!, {r2}
        
        ; Call _default_signal_handler_c, which will determine which
        ; signal was raised and pass it along to the default_signal_handler
	ldr r3, =__default_signal_handler_c 
	bx r3 ;  Does not return
 LEAF_NODE_END

;============================================================================
; _sys_exit
;
;  DESCRIPTION -  
;  Overrides ARM library version of sys_exit.  The ARM library uses the
;  semihosting SWI interface, which we do not support, and so we provide our
;  own function here.  Our software should not be calling exit()
;  regardless. This disables interrupts and calls the c function
;  _sys_exit_c.
;============================================================================
 LEAF_NODE _sys_exit
	; Parameters passed in r0, but currently we are not using this value
        ; the r0 value that is passed to the default_signal_handler needs to
        ; be the cause value for this signal. 
        
        ; Disable interrupts
        mrs r0, CPSR
        orr r0, r0, #PSR_Irq_Mask:OR:PSR_Fiq_Mask
        msr CPSR_c, r0
        
        ; Set cause value to "exit"
	mov r0, #ARM_CAUSE_SYSTEM_EXIT
        ldr r3, =boot_default_signal_handler
	bx r3 ;  Does not return
 LEAF_NODE_END


 LEAF_NODE boot_restore_exception_frame
        ; restore exception frame.
        ldr     r0, [sp, #ARM_TRAP_FRAME_SPSR_OFFSET]    ;  Reload  SPSR
        msr     SPSR_f, r0
        msr     SPSR_c, r0
        ldmib   sp, {r0-r14}^
        nop
        ldr     lr, [sp, #ARM_TRAP_FRAME_PC_OFFSET]      ;  Use pc as supervisor mode lr.
        add     sp, sp, #ARM_TRAP_FRAME_SIZE

        movs    pc, lr          ; return after restoring exception frame.
 LEAF_NODE_END


 LEAF_NODE boot_set_abort_mode
        
        ; force switch to abort mode
        mov     r0, lr
        msr     CPSR_c, #PSR_Abort:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
        ; reset stack pointer here if desired
        ; ldr     r13, =abort_stack+Abort_Stack_Size
        bx      r0   ; return to calling function
 
 LEAF_NODE_END


;----------------------------------------------------------------------------
;  BOOT EXCEPTION MESSAGE TABLE
;
;  The boot exception message table contains the messages to be displayed 
;  on the LCD in the event of an unexpected interrupt.
;----------------------------------------------------------------------------

re_entrant_abort_msg
        DCB     " Re-entrant  Data Abort ",0
;;;              123456789AB+123456789AB+


#ifdef EXTENDED_STORE_TEST
        CODE16
        EXPORT boot_undefined
        ALIGN
        
        ; This function will allow us to generate an undefined instruction
        ; exception for testing purposes.
boot_undefined
        ldr r0,=undef_inst
        bx r0

undef_inst
  DCW &de00
#endif

 
        END


