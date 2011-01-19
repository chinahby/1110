;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                   B O O T    ERROR    H A N D L E R
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
; Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; when      who     what, where, why
; --------  -----   ----------------------------------------------------------
; 10/18/05  rp      Removed boot_hw_powerdown and moved it into boot_target.s
;                   Changed dload_entry to invoke boot_dload_transition()
;                   
; 07/23/05  wd      L4 Merge.
; 12/01/04  tkuo    Created.
;============================================================================
;
;                           MODULE INCLUDE FILES
;
;============================================================================
#include "customer.h"
#include "armasm.h"
#include "boot_stacks.h"
#include "msmhwioreg.h"
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
#if defined(FEATURE_L4)        
#error code not present
#endif

#ifdef FEATURE_QPST_DLOAD_THRU_OEMSBL
        IMPORT  boot_dload_transition
#elif defined(FEATURE_L4)        
#error code not present
#else
        IMPORT  boot_downloader
#endif
        IMPORT  lcd_message
	IMPORT  svc_stack
        IMPORT  clk_regime_init_for_dload
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

        EXPORT boot_error_handler
        EXPORT dload_entry
        EXPORT boot_error_entry        
        EXPORT boot_clear_sl
        EXPORT Error_Fatal
          
		PRESERVE8
        AREA  ERR_HANDLER,  CODE, READONLY


;=======================================================================
; MACRO amss_setup_stack_pointer
; ARGS
;   NONE
; DESCRIPTION
;   This macro will setup the right stack pointer
;=======================================================================
   MACRO
$label  amss_setup_stack_pointer $p1,$p2,$p3
    ldr     $p1, =BOOT_STACK_PTR                
    mov     $p3, $p1                 
$label
   MEND


;============================================================================
; BOOT ERROR HANDLER
;
; DESCRIPTION
;   The boot error handler will is entered if either an error occurs during
;   the system boot sequence.  An LCD error message is displayed, followed
;   by a jump into the download sequence.  Note that pointers to error messages
;   will have already been placed in r0, and is sent as a parameter to the
;   LCD message driver.
;============================================================================

boot_error_handler

        ; This is temporary and should be removed once the LCD drivers have been
        ; re-written as either leaf nodes or in assembly.  Our LCD message
        ; routines currently need a stack, something which is not desirable.
        ; This call will be removed as part of the re-write.
        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        ldr     r13, =svc_stack+SVC_Stack_Size

#if defined FEATURE_STACK_CHECK
        ldr     sl, =svc_stack
#endif

        ; Initialize the LCD and display the error message that is located at
        ; the address currently in register r0.
        ldr     r1, =lcd_message           ; Display LCD Message.
        blatox  r1

;----------------------------------------------------------------------------
; DLOAD ENTRY
;
; DESCRIPTION
;   The boot kernel is entered if an error condition has occurred, or
;   if the application code transfers control back to the boot block to
;   download a new application image.
;----------------------------------------------------------------------------

 LEAF_NODE dload_entry

#ifdef FEATURE_L4
#error code not present
#else
        ; Restore supervisor mode stack for use by boot image in the event
        ; that the boot image has been entered from the application.  Also
        ; disable IRQ and FIQ interrupts in the event that the boot image
        ; has been entered from the application with interrupts enabled.

        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        ldr     r13, =svc_stack+SVC_Stack_Size
#endif

#if defined FEATURE_STACK_CHECK
        ldr     sl, =svc_stack
#endif

        ; Enter the boot kernel, upon return from the boot kernel the only
        ; action left to take is to powerdown.
#ifdef FEATURE_QPST_DLOAD_THRU_OEMSBL
        ldr     r4, =boot_dload_transition

#elif defined( FEATURE_L4)
#error code not present
#else
        ldr     r4, =boot_downloader
#endif
        blatox  r4

 LEAF_NODE_END


;----------------------------------------------------------------------------
; BOOT ERROR ENTRY
;
; DESCRIPTION
;   The boot kernel is entered if an error condition has occurred, or
;   if the application code transfers control back to the boot block to
;   download a new application image.
;----------------------------------------------------------------------------

 LEAF_NODE boot_error_entry

        ; We need to use a stack which will not be overwritten by boot_ram_init.
        ; Once boot_ram_init has been called, we can switch over to another
        ; stack (that will be done in dload_entry)

        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        ;--------------------------------------------------------------
        ; Till the RAM is initialized, we cannot setup the stack for di
        ; in RAM. But there is an internal boot RAM at 0x58000000 which
        ; as a temporary stack till the RAM is initialized.
        ;--------------------------------------------------------------
amss_secured_label0  amss_setup_stack_pointer r0,r1,r13   

        ldr     r4, =clk_regime_init_for_dload
        blatox  r4        
        
        blx dload_entry

 LEAF_NODE_END
;============================================================================
; BOOT_CLEAR_SL
;
;  DESCRIPTION -
;     C prototype - void boot_clear_sl(void);
;     This function clears the SL register (R10).
;============================================================================
 LEAF_NODE boot_clear_sl
        eor     r10, r10, r10
 LEAF_NODE_END

;============================================================================
; ERROR FATAL
;
;  DESCRIPTION - Called when a fatal error occurs.  Sends a message to the 
;                LCD, and then enters the downloader.
; 
;  PARAMETERS
;    r0 points to a string to be output to the LCD.
;
;============================================================================
 LEAF_NODE Error_Fatal
 
        ; Disable interrupts                                                                       
        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        
        ; Output string pointed to by r0 to the LCD.  
        ldr     r1, =lcd_message
        blatox  r1
 #if defined(FEATURE_DISABLE_DLOAD_ENTRY_ON_ERROR_FATAL) 
loop_here
; Kick the watch dog 
        ldr     r0, = HWIO_WDOG_RESET_ADDR 
        ldr     r1, = HWIO_WDOG_RESET_WATCH_DOG_BMSK
        str     r1, [r0]
        b  loop_here
 #else
        ; Enter downloader
        ldr     r1, =dload_entry
        bx      r1                          ; Does not return
 #endif      
        
        
 LEAF_NODE_END
 
      END
