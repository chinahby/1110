;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                   B O O T    R E S E T    H A N D L E R
;
; GENERAL DESCRIPTION
;   This module contains functions for AMSS bootup.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   Control may also return to this module if main() exits
;   unexpectedly, if an unexpected hardware or software interrupt occurs, or
;   if a fatal error occurs.
;
; Copyright (c) 2004-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
;  $PVCSPath:  $
;  $Header: //depot/asic/qsc1100/drivers/boot/boot_reset_handler.s#2 $
; when      who     what, where, why
; --------  -----   ----------------------------------------------------------
; 09/14/05  tbg     Added support for logging resets and detecting soft resets.
; 07/23/05  wd      L4 Merge.
; 12/01/04  chs     Modified to make it compatible for NOR/NAND boot.
; 10/18/04  tkuo    Created. 
;============================================================================
;                    I N C L U D E D    H E A D E R S
;============================================================================

#include "customer.h"
#include "msm.h"
#include "miheader.h"
#include "boot_stacks.h"
#include "armasm.h"
;=======================================================================
;
;                             MODULE DATA 
;
;=======================================================================

Mode_TRUSTED                     EQU 0x04
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
#ifndef FEATURE_USE_SEMIHOSTING_SWI
        ; Generate linker error if any code is linked into the build tha
        ; uses the semihosting SWI interface
        IMPORT  __use_no_semihosting_swi
#endif

        IMPORT  svc_stack
        IMPORT  mmu_invalidate_data_cache
        IMPORT  mmu_invalidate_instruction_cache
        IMPORT  cache_mmu_disable
        IMPORT  boot_error_entry
        IMPORT  boot_main_ctl
        
#ifdef FEATURE_ERR_EXTENDED_STORE
        IMPORT rex_jettison_core
        IMPORT boot_process_err_reset_check
#ifdef FEATURE_DETECT_SOFT_RESETS
        IMPORT Soft_Reset_Handler
#endif
#endif
        
        IMPORT  __rt_entry
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================
        ; Export the symbols __main and _main to prevent the lin
        ; including the standard runtime library and startup routine.

        EXPORT __main
        EXPORT _main

        EXPORT  Reset_Handler
        
#ifdef FEATURE_ERR_EXTENDED_STORE
#ifdef FEATURE_LOG_RESET_REGISTERS

        EXPORT  boot_err_reset_check
#endif
#endif

;=======================================================================
; MACRO amss_setup_stack_ptr
; ARGS
;   NONE
; DESCRIPTION
;   This macro will setup the right stack pointer
;=======================================================================
   MACRO
$label  amss_setup_stack_ptr $p1,$p2,$p3
    ldr     $p1, =BOOT_STACK_PTR                
    mov     $p3, $p1                 
   MEND
   
;============================================================================
; Microprocessor RESET Handler
;============================================================================
    AREA    RST_Handler, CODE, READONLY

    ENTRY

__main
_main

Reset_Handler


#ifdef FEATURE_ERR_EXTENDED_STORE
#ifdef FEATURE_LOG_RESET_REGISTERS
    
    ; The steps taken below will ensure that the current register contents are
    ; preserved for logging at a later time if it is determined that a reset
    ; has occurred


#ifdef FEATURE_DETECT_SOFT_RESETS
    ; The first thing we need to do is check to see what type of reset this
    ; is.  There are three main possibilities:
    ;
    ; 1) Normal (power on) reset
    ; 2) HW Watchdog timeout, other HW initiated reset
    ; 3) Soft reset (branch to zero address)
    ;
    ; The first two can boot up normally, but the third condition (soft reset)
    ; may cause crashes/aborts when trying to reinitialize the HW.  So, we need
    ; to catch it and send it to an abort handler, much like any other exception.
    ; 
    ; To distinguish between these various resets, the boot loader will set a flag 
    ; in SVC r12.  If not set, then we have a soft reset.
	
    ; First, switch to FIQ mode, and save the scratch registers we will need to use.
    ; The FIQ mode registers are used for temporary storage because FIQ mode is
    ; not currently used on 6275.  If we ever need to preserve the FIQ mode
    ; registers after a reset, a new method will be needed.
    ; 
    ; Also, in order to compare to the flag to detect soft resets, we will have to
    ; use up one register from the current mode (r10 has been chosen for this).
    ldr     r10, =RESET_REGISTER_FLAG    
    cmp     r12, r10
    bne     Soft_Reset_Handler
    ; If flag is set, we are coming from bootloader, so clear flag and proceed as normal
    ; We will need to restore correct r12 value from FIQ mode register (done later)
    mov     r12, #0
#endif /* FEATURE_DETECT_SOFT_RESETS */


    msr     CPSR_c, #PSR_Fiq:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
    ; To preserve register values (in case we end up logging a HW watchdog reset),
    ; we need to save some registers to the FIQ mode for temporary keeping.

    mov     r8, r0
    mov     r9, r1
    mov     r0, r14
    mov     r10, r2
    msr     CPSR_c, #PSR_Supervisor:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask    
    mov     r1, r13
    mov     r2, r14
    mov     r12, r0 ; Restore original SVC r12, which was kept in FIQ mode r14
    msr     CPSR_c, #PSR_Fiq:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask    
    mov     r11, r1 ; (r11 = SVC SP)
    mov     r12, r2 ; (r12 = SVC LR)        
    
    ; Now return to SVC mode and proceed with the "normal" boot process.
    ; If this happens to be a watchdog reset, we can not do anything more
    ; with debugging/logging until we have reinitialized the HW.

#endif /* FEATURE_LOG_RESET_REGISTERS */
#endif /* FEATURE_ERR_EXTENDED_STORE */
#if defined(FEATURE_PROGRESSIVE_BOOT) || defined(FEATURE_SUPPORT_MISALIGNED_MEMORY_ACCESS)
;-----------------------------------------------------------------------
; Clear the data and instruction fault status registers
;-----------------------------------------------------------------------
    ldr     r0, =0x0
    mcr     p15, 0, r0, c5, c0, 0
    ldr     r0, =0x0
    mcr     p15, 0, r0, c5, c0, 1
#endif

;-------------------------------------------------------------------------
; Now switch to SVC mode and set up the stack first.
;-------------------------------------------------------------------------
    msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
;-------------------------------------------------------------------------
; Till the RAM is initialized, we cannot setup the stack for different modes 
; in RAM. But there is an internal boot RAM at 0xffff0000 which can be used 
; as a temporary stack till the RAM is initialized.
; For secured boot mode, we use 0x78000000,instead.
;-------------------------------------------------------------------------
amss_secured_label0  amss_setup_stack_ptr r0,r1,r13

#ifdef FEATURE_ERR_EXTENDED_STORE
#ifdef FEATURE_LOG_RESET_REGISTERS
        ; We have a boot stack at this point.
        ; Store r3-r12 to stack, will restore when rex_jettison_core can be called
        stmfd   sp!,{r3-r12}
        ; We need to also remember where we saved the registers on the stack, so we can
        ; retrieve them later.  Place current stack pointer value in FIQ r14.
        mov r3, sp
        msr CPSR_c, #PSR_Fiq:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask        
        mov r14, r3
        msr CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask                
#endif
#endif

;-------------------------------------------------------------------------
; Clear the stack limit register to support software stack limit check
;-------------------------------------------------------------------------
#ifdef FEATURE_STACK_CHECK
    eor     sl, sl, sl   
#endif

;-------------------------------------------------------------------------
; Jump to boot_main_ctl function to execute the boot setup for AMSS code. 
;-------------------------------------------------------------------------
    ldr     r0,=boot_main_ctl
    blx      r0                     
;-------------------------------------------------------------------------
; Movesupervior mode stack pointer to external RAM. 
;-------------------------------------------------------------------------
    msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
    ldr     r13, =svc_stack+SVC_Stack_Size


;-------------------------------------------------------------------------
;At function return, check for return value. If there is an error then
;jump to boot_error_entry otherwise continue to main() where is never returned.
;-------------------------------------------------------------------------
    ldr     r0, =__rt_entry
    blx     r0                     
;============================================================================
;============================================================================
#ifdef FEATURE_ERR_EXTENDED_STORE
#ifdef FEATURE_LOG_RESET_REGISTERS        
boot_err_reset_check     
;============================================================================
; boot_err_reset_check
;
;  DESCRIPTION - Restores original registers to stack, and calls a function
;  to determine if a reset has occurred (in which case it will log the register
;  values).  We do this before we intialize the RAM (so we preserve the current
;  contents).
;
;============================================================================

        stmfd   sp!, {lr}
        
        ; Need to find where registers are stored on stack...this will be in
        ; register r14 of FIQ mode
        
        msr     CPSR_c, #PSR_Fiq:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
        mov     r0, r14
        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask    
        
        ; Load these r3-r12 from stack location
        ldmfd   r0, {r3-r12}
   
   
        ; Restore scratch registers from stack (r0-r3)
        msr     CPSR_c, #PSR_Fiq:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
        mov     r0, r8
        mov     r1, r9
        mov     r2, r10
        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask    

	; Save register contents
        bl      rex_jettison_core
	
	; Several function calls are needed here -- we handle them from
	; a single C function to make things a bit easier.
	;
	; SVC SP and LR (currently in FIQ R11 and R12) will be grabbed by 
        ; this function
	blx     boot_process_err_reset_check

	mov     r0, #0
        ldmfd   sp!, {pc}
#endif
#endif 

    END
