;/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                      S T A R T U P   C O D E
;
; GENERAL DESCRIPTION
;   This module contains the entry point for our Flash Programming Plug-In 
;   for Lauterbach JTAG/ICD TRACE32. All it does is setups stack and calls
;   our main 'C' function "main_c".
;
;   The C function does return and expects a breakpoint there. This is the
;   case because the FLASH programming plug-in specification requires it.
;   The break-point is used to give control back to TRACE32 Debugger so 
;   that it may fill up the buffer and call our plug-in again. This cycle
;   continues until FLASH programming is complete.
;
; EXTERNALIZED FUNCTIONS
;   None.  
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
;   This module must be linked first, specifically the AREA "StartHere", 
;   since this is our entry point. This is done in our scatter load file.
;
;  Copyright (c) 1998, 1999, 2004, 2008 QUALCOMM Incorporated. 
;  All Rights Reserved.
;  Qualcomm Confidential and Proprietary


;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

;/*============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $PVCSPath:  L:/src/asw/MSM6050/vcs/jfstart.s_v   1.1   24 Oct 2001 18:32:56   donb  $
; $Header: //depot/asic/msmshared/tools/jflash/JFLASH.03.00/jfstart.s#2 $ $DateTime: 2008/06/09 12:32:26 $ $Author: c_surven $
;
; when      who    what, where, why
; --------  -----  ----------------------------------------------------------
; 05/27/08  sv     Fix RVCT compiler warnings
; 07/28/04  drh    Add call to memory init function
; 10/01/99  lss    Renamed blx to blatox since blx is a keyword in Assembler
;                  of ARM SDK 2.5
; 12/02/98  lss    Created.
;============================================================================*/

;/*============================================================================
;
;                  REGISTER DEFINITIONS
;
;============================================================================*/

; CPSR Control Masks 
PSR_Fiq_Mask    EQU     0x40
PSR_Irq_Mask    EQU     0x80

; Processor mode definitions 
PSR_Supervisor  EQU     0x13


;/*============================================================================
;
;                  STACK DEFINITIONS
;
;============================================================================*/


; Stack sizes
SVC_Stack_Size  EQU     0x1000
    

;/*============================================================================
;
;                  STARTUP CODE
;
;============================================================================*/


        IMPORT  main_c
        IMPORT  svc_stack
        IMPORT  memory_init
        IMPORT  zero_init_needed

        EXPORT  __main

        PRESERVE8    ; This module preserves 8 byte stack alignment

        AREA    StartHere, CODE, READONLY
        CODE32
__main
        ENTRY

;////////////////////////////////////////////////////////////////////////////////
;//
;// Name: blatox
;//
;// Description: Calls a function from ARM state without having to know whether that 
;//    that function is ARM or Thumb state code.
;//
;// Arguments: 
;//   destreg - The register that contains the address of the function to be called.
;//
;// Registers modified: lr
;//       
;// MODE: Any
;// STATE: ARM. Can call either ARM or Thumb state functions.
;//
;////////////////////////////////////////////////////////////////////////////////

;                                        // call a function without knowing if it is ARM or THUMB code
        MACRO
        blatox  $destreg
        ROUT

        tst     $destreg, #0x01         ;// Test for thumb mode call.

        ldrne   lr, =%1
        ldreq   lr, =%2
        bx      $destreg
1
        CODE16
        bx      pc
        ALIGN
        CODE32
2
        MEND



;=======================================================================
; MACRO mmu_set_default_cfg
; ARGS
;   NONE
; DESCRIPTION
;   Sets a default value into the mmu control register
;   we do not really need to do this, but we do it to
;   know exactly the state of the processor
;=======================================================================
   MACRO
   mmu_set_default_cfg
   ldr    r0, = 0x00050078
   MCR    p15, 0, r0, c1, c0, 0
   MEND
;=======================================================================


;=======================================================================
; MACRO mmu_enable_i_cache
; ARGS
;  NONE
; DESCRIPTION
;  Enables the I Cache
;  Does a read modify write, so no other bits in the control register a
;  affected
;=======================================================================
   MACRO
   mmu_enable_i_cache
   mrc     p15, 0 , r0, c1, c0, 0
   orr     r0, r0, # (1 << 12 )
   mcr     p15, 0 , r0, c1, c0, 0
   MEND
;=======================================================================



;/*============================================================================
;
; FUNCTION STARTUP
;
; DESCRIPTION
;   This function just setup the stack and calls "c" routine main_c.
;
; FORMAL PARAMETERS
;   None.
;
; DEPENDENCIES
;   Any initialization required for normal CPU operation must have
;   been performed prior to calling this module. This can be achieved
;   in the startup script files using data.set command to write to
;   appropriate MSM/ASB registers.
;
; RETURN VALUE
;   This function never returns.
;
; SIDE EFFECTS
;
;============================================================================*/

; Supervisor Mode
; Set up the Supervisor stack pointer.
        msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
        ldr     r13, =svc_stack+SVC_Stack_Size


; ======================================================================
; Enable the instruction cache
; ======================================================================
   mmu_set_default_cfg
   mmu_enable_i_cache

; ======================================================================
; Initialize memory for C only once
;   The test/set of the global variable must be done here in assembly
;   because if we access a global variable in the C function, the
;   compiler will construct a PUSH/POP of registers and since we will
;   have just zeroed the stack, we will pop zero into R14 and then
;   branch to zero.  With no use of globals in the C function,
;   the compiler will generate a bx r14 for the return and all will
;   work correctly.
; ======================================================================

         ldr   r2, =zero_init_needed
         ldr   r1, [r2]
         cmp   r1, #0x0
         beq   skipmem
         
         ldr     a1, =memory_init
         blatox  a1

; indicate zero init completed
         ldr   r2, =zero_init_needed
         mov   r0, #0x0
         str   r0, [r2]

        
; Enter C code execution
        
skipmem        
        ldr     a1, =main_c
        blatox  a1

; Software breakpoint code
; TRACE32 requires this so that we can hand back the control to TRACE32
; Debugger when main_c returns
swbp    DCD     0xEEDEEEDE


; Terminal emulation entry point for putting a character.
; When called, it will put or get a charactr from the
; JTAG debugger terminal emulation window.  These function
; are incompatible with the JTAG debugger FLASH.* functions
; and will cause the debugger to hang if both are used.

        CODE16
        EXPORT  term_put
        ALIGN
term_put
        bx      pc
        DCW     0x0   ; Literal to align next instruction to word boundary
                      ; to avoid compiler warning
        CODE32
        ALIGN
term_put_arm
        mov     r2, #2
put_again
        mrc     p14, 0, r1, c0, c0
        and     r1, r1, r2
        cmp     r1, #0
        bne     put_again

        mcr     p14, 0, r0, c1, c0

        bx      lr

; Get terminal character.
        CODE16
        EXPORT  term_get
        ALIGN
term_get
        bx      pc
        DCW     0x0   ; Literal to align next instruction to word boundary
        CODE32
        ALIGN
term_get_arm
        mov     r2, #1
get_again
        mrc     p14, 0, r1, c0, c0
        and     r1, r1, r2
        cmp     r1, #0
        beq     get_again

        mrc     p14, 0, r0, c1, c0

        bx      lr
   
        END
