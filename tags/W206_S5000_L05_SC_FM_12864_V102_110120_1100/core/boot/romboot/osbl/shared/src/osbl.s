;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;     O S S E C O N D A R Y  B O O T   L O A D E R
;
; GENERAL DESCRIPTION
;   This file contains the Qualcomm Operating System Boot Loader (OSBL) which 
;   performs the following functions:
;        
;      - Continue on setting up the hardware.
;      - Load operating system such as L4 Kernel.
;      - Bring up application boot loader.
;      - Load the reset of AMSS image if necessary. 
;      - Transfers control to operating system. 
;
;   The OSBL is written to perform the above functions with optimal speed.   
;   It also attempts to minimize the execution time and hence reduce boot time.
;
; EXTERNALIZED SYMBOLS
;   osbl_loop_here
;   osbl_set_stack_ptr
;   __main
;   _main
;   
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   
;
; Copyright (c) 2005 by QUALCOMM, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl.s#1 $
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 10/09/07   ly      Create
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#include "osbl_target.h"
#include "msmhwioreg.h"

        
;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================
;            
Mode_SVC                EQU    0x13
Mode_ABT                EQU    0x17
Mode_UND                EQU    0x1b
Mode_USR                EQU    0x10
Mode_FIQ                EQU    0x11
Mode_IRQ                EQU    0x12

I_Bit                   EQU    0x80
F_Bit                   EQU    0x40

        
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================

    ; Import the external symbols that are referenced in this module.
    IMPORT osbl_undefined_instruction_c_handler
    IMPORT osbl_swi_c_handler
    IMPORT osbl_prefetch_abort_c_handler
    IMPORT osbl_data_abort_c_handler            
    IMPORT osbl_reserved_c_handler
    IMPORT osbl_irq_c_handler
    IMPORT osbl_fiq_c_handler
    IMPORT osbl_nested_exception_c_handler
    IMPORT osbl_main_ctl
    IMPORT memcpy

        
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    ; Export the external symbols that are referenced in this module.
    EXPORT osbl_loop_here
    EXPORT osbl_set_stack_ptr

    ; Export the symbols __main and _main to prevent the linker from
    ; including the standard runtime library and startup routine.
    EXPORT   __main
    EXPORT   _main


;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================

    ;--------------------------------------------------------------------------
    ; Data area is in IRAM and location must be defined in scatter load
    ; file.  Data area starts at the top of IRAM.
    ;--------------------------------------------------------------------------
    PRESERVE8
    AREA    OSBL_VECTOR_TABLE, DATA, READWRITE
        
    ;--------------------------------------------------------------------------
    ; Exception vectors table located at the top of IRAM.  This is initialized
    ; to the OSBL exception handlers.  This region will be copied into IRAM 
    ; to replace the vectors which were used by the PBL.
    ;--------------------------------------------------------------------------

unused_reset_vector
    DCD     0x00000000
undefined_instruction_vector  
    DCD     osbl_undefined_instruction_nested_handler
swi_vector
    DCD     osbl_swi_c_handler
prefetch_abort_vector
    DCD     osbl_prefetch_abort_nested_handler
data_abort_vector
    DCD     osbl_data_abort_nested_handler
reserved_vector
    DCD     osbl_reserved_c_handler
irq_vector
    DCD     osbl_irq_c_handler
fiq_vector
    DCD     osbl_fiq_c_handler


;============================================================================
; Qualcomm OS BOOT LOADER ENTRY POINT
;============================================================================

    AREA    OSBL_ENTRY, CODE, READONLY
    CODE32
    ENTRY
        
__main
_main

;============================================================================
;   We contiue to disable interrupt and watch dog until we jump to AMSS 
;============================================================================
osbl_reset_handler
    ; ------------------------------------------------------------------
    ; Disable IRQ and FIQ interrupts.
    ; ------------------------------------------------------------------
    msr CPSR_c,#Mode_SVC:OR:I_Bit:OR:F_Bit    

    ;save the passing parameter from DBL
    mov r6, r0

    ; -----------------------------------------------------------------------
    ; Set up a boot stack pointer in Supervisor,Undefined,Abort mode.
    ; -----------------------------------------------------------------------

    ; Set the Stack Pointer address.
    ldr     r0, =OSBL_STACK_BASE                 
    mov     r13, r0
                       
    ; switch to Undefined mode
    msr     CPSR_c,#Mode_UND:OR:I_Bit:OR:F_Bit
    ; in Undefined mode
    mov     r13, r0                 ; set Undefined SP

    ; switch to Abort mode
    msr     CPSR_c,#Mode_ABT:OR:I_Bit:OR:F_Bit
    ; in Abort mode
    mov     r13, r0                 ; set Abort SP

    ; switch back to Supervisor mode
    msr CPSR_c,#Mode_SVC:OR:I_Bit:OR:F_Bit
        
    ; Restore the passing parameter
    mov    r0, r6

    ; ------------------------------------------------------------------
    ; Call functions external to perform OSBL function.    
    ; It should never return.
    ; ------------------------------------------------------------------
    ldr    r5, =osbl_main_ctl    
    blx    r5

;======================================================================
; Called by osbl_error_hanlder only. We perform
; Clean up the registers and loop here until JTAG is connected.
;======================================================================
osbl_loop_here
    mov r0,#0
    mov r1,#0
    mov r2,#0
    mov r3,#0
    mov r4,#0
    mov r5,#0
    mov r6,#0
    mov r7,#0
    mov r8,#0
    mov r9,#0
    mov r10,#0
    mov r11,#0
    mov r12,#0
loophere
    ldr    r0, =HWIO_WDOG_RESET_ADDR
    mov    r1, #0x1
    str    r1, [r0] 
    b loophere

;======================================================================
; OSBL exception handlers that can have nested calls to them.  These
; handlers check for nesting and if it is the first exception they
; call a "C" exception handler that calls the OSBL error handler.
; If it is a nested exception, the "C" exception handler is not
; re-entered and the JTAG interface is enabled immediately. Nesting
; is only a concern for undefined instruction and abort exceptions.     
; Note, a separate exception handler is used for each exception to 
; provide additional debug information (see osbl_error_handler.c for 
; more information).
;======================================================================
osbl_undefined_instruction_nested_handler
    ldr r5,=osbl_undefined_instruction_c_handler 
    b   check_for_nesting
        
osbl_prefetch_abort_nested_handler
    ldr r5,=osbl_prefetch_abort_c_handler 
    b   check_for_nesting

osbl_data_abort_nested_handler
    ldr r5,=osbl_data_abort_c_handler 
    b   check_for_nesting

;======================================================================
; Checks for nested exceptions and then calls the "C" exception 
; handler pointed to by R5 if this is the first time this exception
; has occurred, otherwise it calls the "C" nested exception handler
; that just enables JTAG debug access.  The mode stack pointer is used  
; to determine if a nested exception or a second abort exception has
; occurred.  This is accomplished by comparing the mode stack pointer  
; to the top of the stack that was initially assigned to the stack,
; then decrementing the stack pointer.  If they are equal, it is a 
; first time exception.  
;======================================================================
check_for_nesting        
    ldr r6,=OSBL_STACK_BASE             ; Initial mode stack ptr
    mov r7, r13                             ; Save current stack ptr 
    sub r13,r13,#4                          ; Decrement stack ptr 
    cmp r6, r7                              ; Compare initial and actual
    blxeq r5                                ; First time exception 
    ldr r5,=osbl_nested_exception_c_handler; This is a nested exception
    blx r5

;======================================================================
; This function is used to set up the stack ptr specified by OSBL
; r0 - new OSBL stack address
; r1 - old stack base address
;======================================================================
osbl_set_stack_ptr
    stmfd    sp!, {r0-r2,lr}
    mov      r2, r1                      ;save old stack base addr
    mov      r1, sp                      ;src addr
    sub      r2, r2, r13                 ;size of the stack
    sub      r0, r0, r2                  ;dest addr
    blx      memcpy
    mov      sp, r0                      ;relocate stack ptr
    ldmfd    sp!, {r0-r2,pc}
   
    END
