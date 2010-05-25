;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                   S T A C K    D E F I N I T I O N 
;
; GENERAL DESCRIPTION
;     This module reserves space for stacks in RAM. The locations of these stacks
;     are specified in the scatter loading file. The stack sizes are defined in 
;     "boot_bsp.h". The NOINIT attribute will prevent linker from reserving space
;     in the executible file. 
;
; EXTERNALIZED FUNCTIONS
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
; Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights reserved.
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
; --------  -----   ----------------------------------------------------------
; 12/01/04  tkuo    Ported from MSM6250.
;----------------------------------------------------------------------------
;                    INCLUDED HEADER FILES
;----------------------------------------------------------------------------

#include "armasm.h"
#include  "boot_stacks.h"

;----------------------------------------------------------------------------
;                    EXTERNS and FORWARD REFERENCES
;----------------------------------------------------------------------------

;============================================================================
; Stack Tables
;============================================================================

        AREA    Stacks,DATA,NOINIT,ALIGN=5
        EXPORT  fiq_stack
        EXPORT  irq_stack
        EXPORT  svc_stack
        EXPORT  abort_stack
        EXPORT  sys_stack
        EXPORT  undef_stack

        EXPORT  boot_stack_initialize

fiq_stack   %       FIQ_Stack_Size
irq_stack   %       IRQ_Stack_Size
svc_stack   %       SVC_Stack_Size
abort_stack %       Abort_Stack_Size
sys_stack   %       Sys_Stack_Size
undef_stack %       Undef_Stack_Size

;============================================================================
;
;                PROCESSOR MODE STACK POINTER INITIALIZATION
;
;============================================================================

       AREA     Boot_Stack_Init, CODE, READONLY
       CODE32
       ALIGN

;============================================================================
; BOOT STACK INITIALIZE
;
;  DESCRIPTION - Initializes the stacks for all modes used
;
;============================================================================
boot_stack_initialize
; --- Initialize stack pointer registers MUST BE PERFORMED IN THIS ORDER ---
; IRQ Mode
        ; Enter IRQ mode and set up the IRQ stack pointer
        msr     CPSR_c, #PSR_Irq:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
        ldr     r13, =irq_stack+IRQ_Stack_Size

;System Mode
        ;Enter System mode and set up the System stack pointer
        msr     CPSR_c, #PSR_System:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
        ldr     r13, =sys_stack+Sys_Stack_Size

;FIQ Mode
        ;Enter FIQ mode and set up the FIQ stack pointer
        msr     CPSR_c, #PSR_Fiq:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
        ldr     r13, =fiq_stack+FIQ_Stack_Size
;Abort Mode
        ;Enter Abort mode and set up the Abort stack pointer

        msr     CPSR_c, #PSR_Abort:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
        ldr     r13, =abort_stack+Abort_Stack_Size
;Undefined Mode
        ;Enter Undefined mode and set up the Undefined stack pointer

        msr     CPSR_c, #PSR_Undef:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
        ldr     r13,=undef_stack+Undef_Stack_Size
;Supervisor Mode
        ;Enter Supervisor mode and then return without assigning stack pointer.

        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask

        blx    lr 

;============================================================================
;============================================================================
        END

