/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                      S T A R T U P   C O D E
;
; GENERAL DESCRIPTION
;   This module is a minimal wrapper for C code intended to run
;   as a standalone downloaded module in RAM.  It assumes that all
;   necessary initialization for normal standby operation has
;   already been performed by the ROM-resident code (or other code
;   run prior to this module).  It simply sets up the segment
;   registers for small model and calls a C function called main_c.
;
;   The C function should not return; if it does, this code sits
;   in a tight loop until the watchdog expires.
;
; EXTERNALIZED FUNCTIONS
;   None.  However, the first code byte of this file is the entry
;   point for the entire program.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   All necessary initialization for normal CPU operation must be
;   completed before jumping into this module.
;
;   This module must be linked first, since the first code byte
;   is the entry point, expected to be at the first address.
;
;   Copyright (c) 2002-2003, 2008 QUALCOMM Incorporated. 
;   All Rights Reserved.
;   Qualcomm Confidential and Proprietary
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=======================================================================
;
;                          EDIT HISTORY FOR MODULE
;
;  This section contains comments describing changes made to the module.
;  Notice that changes are listed in reverse chronological order.
;
;  $Header: //depot/asic/msmshared/tools/hostdl/HOSTDL.14.00/common/ap_startup.s#5 $ $DateTime: 2008/12/24 05:08:57 $ $Author: rpulyala $
;  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/common/ap_startup.s#1 
;
; when       who     what, where, why
; --------   ---     ---------------------------------------------------
; 12/04/08   rk      Changes from BOOT team to support Download in AMSS
; 12/22/08   rk      EHOSTDL - changes to supply seperate stack bases 
;                              for IMEM and RAM
; 11/25/08   rk      EHOSTDL - added call to initialize system settings 
;                            when cfg_data is not available in flash
; 05/28/08   sv      Fix RVCT compiler warnings
; 07/12/03   drh     Add support for buffered IO
; 10/20/03   drh     Enable instruction cache
; 06/16/03   drh     Remove ifdef from term functions, always compile
; 10/30/02   drh     Corrected some minor typos in comments
; 10/29/02   drh     Add conditionally assembled term_put/get
; 09/18/02   drh     Changed comment.
;                    Added software breakpoint 
; 04/28/02   drh     Put call to asb_and_msm_init back in, but contents
;                    of this function are what they originally were
;                    intended to be, not all the hw init stuff.
; 04/10/02   drh     Moved call to asb_and_msm_init into main_c
;                    See detailed comments below as to why.
;01/16/02    drh     Initial version of armprg to support the software 
;                    downloads over UART on a single MSM target built in
;                    the DMSS directory using DMSS header files.  
;                    Based on code cleanup effort by lnr 11/2001.
;
;=======================================================================*/


/*======================================================================
;
;                  REGISTER DEFINITIONS
;
;=======================================================================*/
#include "customer.h"
// CPSR Control Masks 
#define PSR_Thumb_Mask       0x20
#define PSR_Fiq_Mask         0x40
#define PSR_Irq_Mask         0x80
#define PSR_Mode_Mask        0x1f

// Processor mode definitions
#define PSR_User        0x10
#define PSR_Fiq         0x11
#define PSR_Irq         0x12
#define PSR_Supervisor  0x13
#define PSR_Abort       0x17
#define PSR_Undef       0x1b
#define PSR_System      0x1f
#define PSR_Thumb       0x20


/*============================================================================
;
;                  STACK DEFINITIONS
;
;============================================================================*/

//  Used as flashprg stack (FLSH_Stack_Size). Allocate 16K for the    
//  stack.  A largish stack is required because      
//  the packet buffer will be allocated on the stack.
//  These stacks are defined at dword in the C code and are defined
//  as being this size divided by 4.  Creating them in the C code
//  as dword prevents unaligned accesses.

#define SVC_Stack_Size   0x4000

//  Define other stacks, these can probably be combined to use
//  the same stack

#define IRQ_Stack_Size   0x800
#define Abort_Stack_Size 0x100
#define Undef_Stack_Size 0x100

/*============================================================================
;
;                  STARTUP CODE
;
;============================================================================*/


      CODE32
      
      IMPORT  main_c
      IMPORT  memory_init
      IMPORT  asb_and_msm_init
      IMPORT  abort_stack
      IMPORT  undef_stack
      IMPORT  irq_stack
      IMPORT  svc_stack

#ifdef EHOSTDL_RELOCATE_DATA_AREAS
      IMPORT  mc_init
#endif

      EXPORT  startup

      PRESERVE8  ; This module preserves 8 byte stack alignment

      AREA    BootRom, CODE, READONLY

      ENTRY

/*============================================================================
;
; Name: blx
;
; Description: Calls a function from ARM state without having to know whether
;    that function is ARM or Thumb state code.
;
; Arguments: 
;   destreg - The register that contains the address of the function to be
;   called.
;
; Registers modified: lr
;       
; MODE: Any
; STATE: ARM. Can call either ARM or Thumb state functions.
;
;============================================================================*/

   // call a function without knowing if it is ARM or THUMB code
   MACRO   
      blxx     $destreg
      ROUT

      tst     $destreg, #0x01         // Test for thumb mode call.

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






/*============================================================================
;
; FUNCTION STARTUP
;
; DESCRIPTION
;   This function just sets up the segment registers and calls
;   the C function main_c.  If that function returns, a tight loop
;   waits for a watchdog reset.
;
; FORMAL PARAMETERS
;   None.
;
; DEPENDENCIES
;   Any initialization required for normal CPU operation must have
;   been performed prior to calling this module.
;
;   The entry point is intended to be jumped into by a downloader.
;
; RETURN VALUE
;   This function never returns.
;
; SIDE EFFECTS
;
;============================================================================*/

// All GPIO init/config is moved to C code in asb_and_msm_init

startup
#ifndef FEATURE_DLOAD_IN_AMSS
   
   ldr     r1,  =0xdeadbeef             
   ldr     r2,  =0xdeadbeef
   ldr     r3,  =0xdeadbeef
   ldr     r4,  =0xdeadbeef
   ldr     r5,  =0xdeadbeef
   ldr     r6,  =0xdeadbeef
   ldr     r7,  =0xdeadbeef
   ldr     r8,  =0xdeadbeef
   ldr     r9,  =0xdeadbeef
   ldr     r11, =0xdeadbeef
   ldr     r12, =0xdeadbeef
   ldr     r13, =0xdeadbeef
   ldr     r14, =0xdeadbeef
                                        
   // Inhibit interrupts
   mrs     a1, CPSR                
   orr     a2, a1, #PSR_Irq_Mask
   msr     CPSR_c, a2

   // Abort Mode
   // Enter Abort mode and set up the stack pointer
   msr     CPSR_c, #PSR_Abort:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
   ldr     r13, =abort_stack+Abort_Stack_Size

   // Undef Mode
   // Enter Undef mode and set up the stack pointer
   msr     CPSR_c, #PSR_Undef:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
   ldr     r13,=undef_stack+Undef_Stack_Size

   // IRQ Mode
   // Enter IRQ mode and set up the IRQ stack pointer
   msr     CPSR_c, #PSR_Irq:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
   ldr     r13, =irq_stack+IRQ_Stack_Size

   // Supervisor Mode
   // Set up the Supervisor stack pointer.
   // = FLSH_Stack_size
   msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask

   // EHOSTDL_RELOCATE_DATA_AREAS: Defined in ap_config.min when building HOSTDL
   //    for enandprg. Flag to indicate compiling the code relating to relocating
   //    the data areas to IMEM/RAM
   // EHOSTDL_IMEM_STACK_BASE: Defined in boot_target_param.min. Indicates the stack base address
   //    when in IMEM
   // EHOSTDL_RAM_STACK_BASE: Defined in boot_target_param.min. Indicates the stack base address
   //    when in RAM

#ifdef EHOSTDL_RELOCATE_DATA_AREAS
   ldr     r13, =EHOSTDL_IMEM_STACK_BASE

   ldr     a1,=mc_init
   blxx    a1
#else
   ldr     r13, =svc_stack+SVC_Stack_Size
#endif
#endif 
   // Initialize memory for C
   ldr     a1, =memory_init
   blxx     a1
#ifndef FEATURE_DLOAD_IN_AMSS
  

; ======================================================================
; Enable the instruction cache
; ======================================================================
   mmu_set_default_cfg
   mmu_enable_i_cache

#ifdef EHOSTDL_RELOCATE_DATA_AREAS
   ldr     r13, =EHOSTDL_RAM_STACK_BASE   
#endif 
#endif
   // Enter C code execution
   ldr     a1, =main_c
   bx      a1


; Software breakpoint code
; main_c never returns, but if it did, a breakpoint here
; would catch it
swbp    DCD     0xEEDEEEDE


#ifdef USE_BUFFERED_TERMIO        

; Terminal emulation entry point for put/get a buffer of characters.

; Buffered I/O uses a spot breakpoint on the host side.
; This does not work with T32 flash.* commands used
; in JFLASH and JNAND.  XScale processor does not implement
; regular I/O like our MSM does, so this must be used
; but is really only useful in ARMPRG/NANDPRG style
; flash programmers which are getting the data to be
; programmed from a UART or USB interface.

        CODE16
        EXPORT  term_blip
term_blip
        bx      lr

        EXPORT  output_port
        EXPORT  input_port

; This is in RAM, so just put it here.  These are the buffers
; used.  The first 32-bit word is the count of characters.
; The rest of the buffer is data, one character in every
; 32-bit word.  Character counter in printf file will only fill
; with 252 words of data, ensuring no overflows.
;
; Host side is invoked with:
;
; term.protocol bs
; term.view output_port input_port
; term.write output_port term.out
; b.s term_blip /s /onchip

output_port
        % 256
input_port
        % 256

   
#else /* Normal terminal IO */


; Terminal emulation entry point for put/get a character.
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
   
#endif /* USE_BUFFERED_TERMIO */


;  Jump to address passed in.
;  Used to fake a reset by passing zero
        CODE16
        EXPORT  jmp_abs
        ALIGN
jmp_abs
        bx      pc
        DCW     0x0   ; Literal to align next instruction to word boundary
        CODE32
        ALIGN
jmp_abs_arm
        mov r7,r0
        bx r7
        

   
   END
   
