;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                  D E V I C E    B O O T    L O A D E R        
;
; GENERAL DESCRIPTION
;   This file contains the Device Boot Loader (DBL) which 
;   performs the following functions:
;        
;      - Load FSBL image
;      - Load OSBL image
;
;   The DBL is written to perform the above functions with optimal speed.   
;   It also attempts to minimize the execution time and reduce boot time.
;
; EXTERNALIZED SYMBOLS
;   
;   
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   
;
; Copyright (c) 2007 by QUALCOMM, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
        
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl.s#2 $
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 07/30/07   ANB     Initial version.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#include "dbl.h"
#include "msmhwioreg.h"

;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================
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
    IMPORT dbl_undefined_instruction_c_handler
    IMPORT dbl_swi_c_handler
    IMPORT dbl_prefetch_abort_c_handler
    IMPORT dbl_data_abort_c_handler            
    IMPORT dbl_reserved_c_handler
    IMPORT dbl_irq_c_handler
    IMPORT dbl_fiq_c_handler
    IMPORT dbl_nested_exception_c_handler
    IMPORT dbl_main_ctl
    IMPORT dbl_stack_base

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================
    ; Export the external symbols that are referenced in this module.
    EXPORT   dbl_loop_here
    EXPORT   dbl_enable_arm9_instruction_cache

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
    AREA    DBL_VECTOR_TABLE, DATA, READONLY
        
    ;--------------------------------------------------------------------------
    ; Exception vectors table located at the top of IRAM.  This is initialized
    ; to the DBL exception handlers.  This region will be copied into IRAM 
    ; to replace the vectors which were used by the PBL.
    ;--------------------------------------------------------------------------

unused_reset_vector
    DCD     0x00000000
undefined_instruction_vector  
    DCD     dbl_undefined_instruction_nested_handler
swi_vector
    DCD     dbl_swi_c_handler
prefetch_abort_vector
    DCD     dbl_prefetch_abort_nested_handler
data_abort_vector
    DCD     dbl_data_abort_nested_handler
reserved_vector
    DCD     dbl_reserved_c_handler
irq_vector
    DCD     dbl_irq_c_handler
fiq_vector
    DCD     dbl_fiq_c_handler

;=======================================================================
;                       BOOT BLOCK DATA LOCATIONS
;
;  Locations and sizes of data areas in ROM and RAM are imported from th
;  linker and stored as data items that are used at runtime by the boot
;  kernel routines.
;=======================================================================

;======================================================================
; DBL code.
;======================================================================
 AREA    DBL_CODE, CODE, READONLY
 CODE32
 ENTRY

__main
_main

;============================================================================
;   We contiue to disable interrupt and watch dog until we jump to OSBL 
;============================================================================
dbl_reset_handler

    ; ------------------------------------------------------------------
    ; Disable IRQ and FIQ interrupts.
    ; ------------------------------------------------------------------
    msr CPSR_c,#Mode_SVC:OR:I_Bit:OR:F_Bit    

    ; ------------------------------------------------------------------
    ; Save the passing parameter from PBL
    ; ------------------------------------------------------------------    
    mov r7, r0

    ; ------------------------------------------------------------------
    ; Save Stack base from PBL which DBL will use for all the modes.
    ; ------------------------------------------------------------------
    ldr r6, =dbl_stack_base
    str r13,[r6]
    mov r0, r13
    
    ; -----------------------------------------------------------------------
    ; Set up a boot stack pointer in Supervisor,Undefined,Abort mode.
    ; DBL uses the same stack address since there is no                   
    ; return after jumping to error_handler. We disable IRQ and FIQ
    ; interrupts. We also initialize dummy stack point for unused IRQ, FIQ, 
    ; User and System mode. 
    ; -----------------------------------------------------------------------
                       
    ; switch to Undefine mode
    msr     CPSR_c,#Mode_UND:OR:I_Bit:OR:F_Bit

    ; in Undefined mode
    mov     r13, r0                 ; set Undefined SP

    ; switch to Abort mode
    msr     CPSR_c,#Mode_ABT:OR:I_Bit:OR:F_Bit

    ; in Abort mode
    mov     r13, r0                 ; set Abort SP
    ; switch back to Supervisor mode
    msr CPSR_c,#Mode_SVC:OR:I_Bit:OR:F_Bit

    ; ------------------------------------------------------------------
    ; Call functions external to perform DBL functions.    
    ; It should never return. r0 has the pointer location
    ; sent from PBL.
    ; ------------------------------------------------------------------
    ; Restore the passing parameter
    mov    r0, r7
    ldr    r5, =dbl_main_ctl    
    blx    r5

;======================================================================
; Called by dbl_error_hanlder only. We perform
; Clean up the registers and loop here until JTAG is connected.
;======================================================================
dbl_loop_here
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
; Called by dbl_main_ctl to enable instruction cache
;======================================================================
dbl_enable_arm9_instruction_cache
    mrc    p15, 0, r0, c1, c0, 0  ; Read from CP15 Control Register
    orr    r0, r0, # (1 << 12 )   ; Enable Level one Instruction Cache
    mcr    p15, 0, r0, c1, c0, 0  ; Write back to CP15 Control Register
    mov    r0, #0                 ; Return success
    bx     lr

;======================================================================
; DBL exception handlers that can have nested calls to them.  These
; handlers check for nesting and if it is the first exception they
; call a "C" exception handler that calls the DBL error handler.
; If it is a nested exception, the "C" exception handler is not
; re-entered and the JTAG interface is enabled immediately. Nesting
; is only a concern for undefined instruction and abort exceptions.     
; Note, a separate exception handler is used for each exception to 
; provide additional debug information (see dbl_error_handler.c for 
; more information).
;======================================================================
dbl_undefined_instruction_nested_handler
    ldr r5,=dbl_undefined_instruction_c_handler 
    b   check_for_nesting
        
dbl_prefetch_abort_nested_handler
    ldr r5,=dbl_prefetch_abort_c_handler 
    b   check_for_nesting

dbl_data_abort_nested_handler
    ldr r5,=dbl_data_abort_c_handler 
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
    ldr r8,=dbl_stack_base                  ; Initial mode stack ptr
    ldr r6,[r8]                             ; Initial mode stack ptr
    mov r7, r13                             ; Save current stack ptr 
    sub r13,r13,#4                          ; Decrement stack ptr 
    cmp r6, r7                              ; Compare initial and actual
    blxeq r5                                ; First time exception 
    ldr r5,=dbl_nested_exception_c_handler  ; This is a nested exception
    blx r5

    END
