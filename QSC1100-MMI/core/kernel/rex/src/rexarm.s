;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;         R E X  A R M   P O R T  A S S E M B L Y   R O U T I N E S
;
; GENERAL DESCRIPTION
;   This module contains the ARM port of REX functions that needed to be
;   coded in assembly.
;
; EXTERNALIZED FUNCTIONS
;   rex_sched
;     If the current task is not the best task, a context switch occurs
;   rex_int_lock
;     Locks interrupts
;   rex_int_free
;     Unlocks interrupts
;   rex_ints_are_locked
;     Returns interrupt status
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   The bulk of the REX code is contained in rex.c.
;
;  Copyright (c) 1998-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                      EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: //source/qcom/qct/core/kernel/rex/main/latest/rexarm.s#2 $
;
; when      who    what, where, why
; --------  -----  -----------------------------------------------------------
; 01/24/08   umr   Assert DAC in rex_apc_epilog.
; 08/22/07   cz    Added optimization for DAC based memory protection feature from umr.
; 07/13/07   cz    Added support for DAC based memory protection feature from umr.
; 02/21/07   tbg   Fixed access to rex_trace_enable variable
; 09/21/05   cr    Added PC logging functionality as an overloaded functionality
;                  of stack checking.
; 08/10/05   ck    Fixed an issue that corrupted the TCB  of the current
;                  task when an FIQ arrived and changed the
;                  rex_best_task while IRQ was re-scheduling tasks.
; 11/11/04   rp    Added rex_sched_force() for Progressive Boot
; 09/21/04   rp    Added Progressive boot support
; 08/11/04   is    Added REX profiling for new time VU.
; 06/03/04   gfr   Rex intlock profiling support.
; 09/15/03   kvd   Modified function_level  profiling to work with
;                   TIMETEST_INV_TRIGGER feature
; 09/10/03   gr    Added a function to check if interrupts are locked.
; 08/13/03   kvd   added code to write 0000 marker and timetest data in the
;                  reverse order if TIMETEST_INV_TRIGGER is defined.
; 06/24/03   gr    Added code to check for FIQ stack overflow.
; 06/20/03   gr    Deleted code for software profiling using FIQ. This no
;                  longer works.
; 02/24/03   gr    Fixed a timetest bug.
; 05/14/02   gr    Made changes to support FEATURE_STACK_CHECK.
; 03/27/02   gr    Removed all references to rex_defer_sched, which is no
;                  longer used.
; 03/20/02   dc    Added support for FEATURE_SOFTWARE_PROFILE
; 08/15/01   gr    Made changes to support stack checking.
;                  Deleted APC-related code that is no longer required.
; 07/25/01   om    Fixed TIMETEST for nested ISRs
; 07/16/01   om    Updated TIMETEST functionality to profile multipl. tasks
; 05/29/01   kar   Fixed call to FIQ handler which was compiled out causing phone
;                  to reset
; 12/19/00   jct   Merged MSM5000, MSM5200, MSP1000 versions of REX
; 08/18/00   bgc   Added r8-r11 to the task context for task switches.
; 06/20/00   jcw   Added support for interworking
; 03/27/00   tim   Added timetest support, fixed bugs with rex_int_nest_level
; 11/30/99   tim   Added code to rex_sched to defer task swap if a TASKLOCK
; 06/14/99   tac   Added support for pre-emptive interrupts.
; 05/19/99   ms    Not passing rex_curr_task to rex_report_stack_overflow,
;                     since this value is available as a global anyway.
; 04/07/99   sk    Removed warnings for ARM 2.5 SDT.
; 03/19/99   ms    Defined stack overflow handlers.
; 02/19/99   ms    Optimized IRQ_Handler to save complete context only when
;                  required. Also running in Supervisor mode.
;                  Removed macros save_context_from_interrupt,
;                  save_context_from_task and restore_context, since these are
;                  not being used any more.
;                  Modified rex_sched to conform to IRQ_Handler optimization.
; 02/12/99   ms    Added support for TIMETEST.
; 02/04/99   ms    Replaced 'blx' with 'blatox'.
;                  Use of fiq_stack and call to rex_fiq_vector were featurized
;                  inside FIQ_Handler using FEATURE_INT_125_FIQ.
; 01/18/99   ms    Moved context frame related macros from armasm.h.
; 11/30/98  jkl    Added Interrupt Vectors. Revised and cleaned up code
; 09/29/98  jkl    Adapted for new context frame
; 08/14/98  jct    Changed processing in irq to save context and restore to
;                  best_task on exit.  Changed rex_sched to save/restore same
;                  context frame as irq and to update curr_task to best_task
;                  regardless of interrupt level
; 05/29/98  jbh    Created file
;=============================================================================

#include "arm.h"
#include "rexarm.h"
#include "armasm.h"
#include "customer.h"
#ifdef FEATURE_DAC_MEMORY_PROTECTION
  #include "armmmu.h"
#endif

#ifdef TIMETEST
   #define REXARM_S
   #include "timetest.h"
#endif

#ifdef FEATURE_PROGRESSIVE_BOOT
#include "boot_trap.h"
#endif

;=============================================================================
;
;                            REGION and SIZE
;
;=============================================================================

  AREA RexRom, CODE, READONLY
  CODE32


;=============================================================================
;
;                       INCLUDE FILES FOR MODULE
;
;=============================================================================

;=============================================================================
;
;              CONSTANTS, MACROS AND STRUCTURE TYPES FOR MODULE
;
;=============================================================================

;=============================================================================
;
;                      IMPORTED FUNCTIONS AND SYMBOLS
;
;=============================================================================
  IMPORT rex_curr_task           ; These are from rex.c
  IMPORT rex_best_task
  IMPORT rex_task_to_schedule
  IMPORT rex_irq_vector
  IMPORT rex_fiq_vector
  IMPORT rex_sched_allow
  IMPORT rex_int_nest_level
  IMPORT rex_cpsr_value
#ifdef FEATURE_STACK_CHECK
  IMPORT sys_stack
  IMPORT rex_report_stack_overflow
  IMPORT __16__rt_stkovf_split_small
#endif

#if defined FEATURE_INT_125_FIQ
  IMPORT fiq_stack               ; from bootmem.c
#endif

  IMPORT irq_stack

#if defined TIMETEST
  IMPORT rex_led_buf
  #ifdef TIMETEST_PROF
    IMPORT timetest_isr_markers
    IMPORT timetest_isr_level
  #endif
  #ifdef TIMETEST_FUNCS
    IMPORT timetest_parse_token
    IMPORT timetest_markers
  #endif
#endif

#if defined FEATURE_SOFTWARE_PROFILE
  IMPORT rex_profile_enabled
  IMPORT rex_IRQnumticks
  IMPORT rex_pnumticks
  IMPORT rex_time_tick2
#endif

#if defined FEATURE_REX_PROFILE
  IMPORT rex_intlocked
#endif

#if defined(FEATURE_STACK_CHECK) && defined(FEATURE_LOG_FUNCTIONS)

REX_LOG_SIZE      EQU    T_REX_LOG_SIZE
REX_CURR_LOG_PTR  EQU    T_REX_LOG_BASE
REX_START_LOG_PTR EQU    T_REX_LOG_BASE + 0x08
REX_END_LOG_PTR   EQU    T_REX_LOG_BASE + T_REX_LOG_SIZE - 0x30
#endif

#if defined FEATURE_REX_STAT
#error code not present
#endif

#if defined(FEATURE_REX_TRACE) && !defined(BUILD_BOOT_CHAIN)
  IMPORT rex_trace_enabled
  IMPORT rex_trace_task_leave
  IMPORT rex_trace_task_enter
#endif

#if defined(FEATURE_USE_TIME_VU) && defined(FEATURE_REX_PROFILE)
  ; for the sleep xlat HW reg
  #include "msmhwioreg.h"

  IMPORT rextp_in_irq_count
  IMPORT rex_profiling_is_enabled
  IMPORT rextp_last_swap_timetick
  IMPORT rex_total_time_samples
  IMPORT rextp_max_elapse_timetick
  IMPORT rex_kernel_tcb
#endif
#ifdef FEATURE_DAC_MEMORY_PROTECTION
  IMPORT mmu_set_dac
  IMPORT mmu_dac_shadow
#endif
;=============================================================================
;
;                      EXPORTED FUNCTIONS AND SYMBOLS (See Macro Leaf_Node)
;
;=============================================================================
;   EXPORT IRQ_Handler                 ; Irq handler
;   EXPORT FIQ_Handler                 ; Fiq handler
;   EXPORT rex_sched                   ; The scheduler
;   EXPORT rex_start_task              ; Start task in Rex_init
;   EXPORT rex_int_lock                ; Disable interrupts
;   EXPORT rex_int_free                ; Enable interrupts
;   EXPORT rex_ints_are_locked         ; Return interrupt status
;   EXPORT rex_set_CPSR                ; Set CPSR
;   EXPORT rex_get_CPSR                ; Get CPSR
;   EXPORT DBGbreak                    ; Debug Break
;   EXPORT rex_apc_epilog              ; Post-processing after an APC call
;   EXPORT rex_enable_pc_profile       ; Enables the PC dump feature
;   EXPORT rex_disable_pc_profile      ; Disables the PC dump feature
;=============================================================================

;=============================================================================
;
; FUNCTION IRQ_handler
;
; DESCRIPTION
;    Entry point of the IRQ interrupt handling services
;
; FORMAL PARAMETERS
;   None
;
; DEPENDENCIES
;   tramp_init must have been initialized.
;
; RETURN VALUE
;   None
;
; SIDE EFFECTS
;   too many to describe
;
; ALGORITHM
;   1. Save context
;   2. Jump to IRQ mode
;   3. ifdef TIMETEST, write to port
;   4. Call the registered ISR
;   5. Restore processor status word
;   6. Switch mode back to previous
;   7. check for tasklock being in effect, if so, skip next block
;      and branch to no_switch.  No_swithc does not return
;   8. Check for context_switch required, if so, branch to task_swapped
;      routine, otherwise fall through to no_switch.
;   9. no_switch: restore previous task context, RETURN to task.
;  10. task_swapped: We get here only via a branch, not via a fall-through.
;      restore context of task that needs to be context-switched in.
;
;=============================================================================
  EXPORT IRQ_Handler
  ALIGN
  ROUT
IRQ_Handler

#ifdef T_QSC1100
    ldr     r4, [sp], #4   ; pop out r4 from stack used by pbl_irq_handler
#endif

    sub     lr, lr, #4     ; adjust return address (ARM requirement)

; --------------------------------------------------------------------
; Some registers will be trashed by the interrupt handler function
; which is a C routine. Save those registers on the interrupt stack.
; Save the SPSR too. That will be trashed by the subsequent nested
; interrupt.
; --------------------------------------------------------------------
    stmfd   sp!, {r0-r3, r10, r12, r14}
    mrs     r0, SPSR
    stmfd   sp!, {r0}
#if defined(FEATURE_STACK_CHECK) && defined(FEATURE_LOG_FUNCTIONS)
    stmfd   sp!, {r14}
#endif    

; --------------------------------------------------------------------
; Check for interrupt stack overflow.
; --------------------------------------------------------------------
#ifdef FEATURE_STACK_CHECK
#if defined(FEATURE_LOG_FUNCTIONS)
    ldr     sl, =0xffffffff
#else
    ldr     sl, =irq_stack
#endif    
        cmp     r13, sl
        bcs     __irq_no_stkovrflow
        bl      __16__rt_stkovf_split_small
__irq_no_stkovrflow
#endif
#if defined(FEATURE_LOG_FUNCTIONS)
    ldmfd   sp!, {r14}
#endif    

; --------------------------------------------------------------------
; Increment the nesting level counter.
; --------------------------------------------------------------------
    ldr     r0, =rex_int_nest_level
    ldrh    r1, [r0]
    add     r1, r1, #1
    strh    r1, [r0]

; --------------------------------------------------------------------
; Switch to System Mode and save r14 on stack.
; --------------------------------------------------------------------
    msr     CPSR_c, #PSR_System:OR:PSR_Irq_Mask
    stmfd   sp!, {r14}

#ifdef FEATURE_REX_STAT
#error code not present
#endif

#if ((defined TIMETEST) && (! defined TIMETEST_PROF)) \
    || ((defined TIMETEST_PROF) && (defined TIMETEST_NO_ISRS))
; --------------------------------------------------------------------
; TIMETEST_INT is a special value denoting Interrupt.
; Write that value to the TIMETEST port.
; --------------------------------------------------------------------

#ifdef TIMETEST_INV_TRIGGER
#error code not present
#else /* TIMETEST_INV_TRIGGER */

    mov     r0, #TIMETEST_INT       ; Interrupt id on the Led
    ldr     r1, =rex_led_buf        ; Load contents of leds buffer
    ldr     r2, =TIMETEST_PORT      ; Write to the leds
    strh    r0, [r2]
    strh    r0, [r1]                ; Save new leds contents

#if defined TIMETEST_PROF
    ldr     r0, =timetest_isr_level ; Load the ISR level var
    strb    r1, [r0]                ; Save the current ISR level
#endif
#if (defined TIMETEST_NO_ISRS) && (defined TIMETEST_EXPLICIT_TRIGGER)
    mov     r0, #0                  ; Write token '0000' to led port
    strh    r0, [r2]                ; dito
#endif

#endif /* TIMETEST_INV_TRIGGER */

#else
#if defined TIMETEST_PROF
    ldr     r0, =timetest_isr_level ; Load the ISR level var
    strb    r1, [r0]                ; Save the current ISR level
    mov     r2, #TIMETEST_ISR_NESTED_ID  ; Get the reserved value 0xff
    ldr     r0, =timetest_isr_markers
    strb    r2, [r0, r1]
#endif
#endif

#if defined(FEATURE_USE_TIME_VU) && defined(FEATURE_REX_PROFILE)
    ldr     r2, =rex_profiling_is_enabled        ; load rex_profiling_is_enabled flag
    ldrb    r2, [r2]                             ; dereference the flag
    cmp     r2, #1                               ; compare with TRUE
    bne     profile_skip_task                    ; skip if FALSE

    ldr     r0, =rex_int_nest_level
    ldrh    r1, [r0]
    cmp     r1, #1
    bhi     profile_skip_task                    ; IRQ within IRQ

    ldr     a3, =HWIO_SLEEP_XTAL_TIMETICK_COUNT_ADDR
    ldr     a4, [a3]                 ; load first sleep xlat timetick
rex_sleep_xtal_timetick_loop_in_irq_handler_1

    mov     a2, a4                   ; copy the new count for next comparison
    ldr     a4, [a3]                 ; load sleep xlat timetick again
    cmp     a4, a2                   ; if they do not match, try again
    bne     rex_sleep_xtal_timetick_loop_in_irq_handler_1
    ldr     a1, =rextp_last_swap_timetick

    ; Calculate the elapsed time
    ; Since the cur_tick could have rolled over, the following binary
    ; arithematic trick is used to calculate the difference between
    ; cur_tick and rextp_last_swap_timetick
    ; elapse_timetick = ~rextp_last_swap_timetick + 1 + cur_tick

    ldr     a3, [a1]                 ; a3 = rextp_last_swap_timetick
    str     a2, [a1]                 ; update rextp_last_swap_timetick
    mvn     a4, #0                   ; prepare 0xffff-ffff
    eor     a3, a3, a4               ; ~rextp_last_swap_timetick
    add     a3, a3, #1               ; ~rextp_last_swap_timetick + 1
    add     a3, a3, a2               ; elapse_timetick (a3) = ~rextp_last_swap_timetick + 1 + cur_tick

    ;rex_curr_task->time_samples += elapse_timetick;
    ldr     a1, =rex_curr_task
    ldr     a1, [a1]                 ; a1 = *rex_curr_task
    ldr     a2, [a1, #REX_TCB_TIME_SAMPLE_OFFSET] ; a2 = rex_curr_task->time_samples
    add     a2, a2, a3               ; add elapse_timetick
    str     a2, [a1, #REX_TCB_TIME_SAMPLE_OFFSET] ; save it back to rex_curr_task structure

    ;rex_total_time_samples += elapse_timetick;
    ldr     a1, =rex_total_time_samples
    ldr     a2, [a1]
    add     a2, a2, a3
    str     a2, [a1]

    ; rextp_max_elapse_timetick = MAX(rextp_max_elapse_timetick, elapse_timetick)
    ldr     a1, =rextp_max_elapse_timetick
    ldr     a2, [a1]
    cmp     a3, a2                  ; if elapse_timetick > rextp_max_elapse_timetick
    strhi   a3, [a1]                ;  rextp_max_elapse_timetick =  elapse_timetick

    ldr     r1, =rextp_in_irq_count               ; load rextp_in_irq_count
    ldr     r2, [r1]                            ; dereference the count
    add     r2, r2, #1                          ; increment the count
    str     r2, [r1]                            ; store it back to rextp_in_irq_count

profile_skip_task

#endif

#if defined FEATURE_SOFTWARE_PROFILE
    ldr     r2, =rex_profile_enabled             ; load rex_profile_enabled flag
    ldrb    r2, [r2]                             ; dereference the flag
    cmp     r2, #1                               ; compare with TRUE
    bne     profile_skip_task                    ; skip if FALSE

    ldr     r0, =rex_int_nest_level
    ldrh    r1, [r0]
    cmp     r1, #1
    bhi     profile_skip_task                    ; IRQ within IRQ

    ldr     r2, =rex_time_tick2
    ldr     r1, [r2]
    ldrh    r0, [r1]

    ldr     r2, =rex_pnumticks
    ldrh    r1, [r2]
    strh    r0, [r2]

;=============================================================================
; r0 is tick_tick2 & r1 is rex_pnumticks (both 16 bits)
; overflow calculation of r0-r1 is simply !r1+1+r0
;=============================================================================

    mvn     r2, #0                               ; 0xffffffff
    eor     r1, r1, r2, lsr #16                  ; r1 = r1 eor 0x0000ffff
    add     r1, r1, #0x01
    add     r1, r1, r0                           ; r1 = amount to increment
    and     r1, r1, r2, lsr #16                  ; r1 = r1 and 0x0000ffff

    ldr     r2, =rex_curr_task
    ldr     r2, [r2]
    ldr     r0, [r2, #REX_TCB_NUMTICKS_OFFSET]
    add     r0, r0, r1
    str     r0, [r2, #REX_TCB_NUMTICKS_OFFSET]

profile_skip_task

#endif

#if defined(FEATURE_REX_TRACE) && !defined(BUILD_BOOT_CHAIN)
; --------------------------------------------------------------------
; We just left the current task, we are in system mode now so to get
; the task PC, we need to go to IRQ mode and read r14
; --------------------------------------------------------------------
    ldr     r0, =rex_trace_enabled
    ldrb    r0, [r0]
    cmp     r0,#1
    bne     rex_trace_skip
    stmfd   sp!, {r0-r3,r14}         ; Save what we touch
    msr     CPSR_c, #PSR_Irq:OR:PSR_Irq_Mask     ; switch to IRQ mode
    mov     r0, r14                  ; Get task PC
    msr     CPSR_c, #PSR_System:OR:PSR_Irq_Mask  ; switch back to system mode
    ldr     r1, =rex_trace_task_leave
    blatox  r1
    ldmfd   sp!, {r0-r3,r14}         ; Restore registers
rex_trace_skip
#endif

; --------------------------------------------------------------------
; Call the interrupt handler
;
; The processor is in PSR_System mode which means that the
; current stack is sys_stack. So, sys_stack is loaded to sl.
;
; Most mobile.c rex_init() calls pass irq_stack which is assigned
; to rex_int_stack. sys_stack has been directly coded here to defer
; forcing every target mobile.c to have to change. This change to
; rexarm.s makes variable rex_int_stack obsolete as well as making
; rex_init() API obsolete.
; --------------------------------------------------------------------
#if defined(FEATURE_STACK_CHECK)
#if defined(FEATURE_LOG_FUNCTIONS)
    ldr     sl, =0xffffffff
#else
    ldr     sl, =sys_stack
#endif
#endif
#ifdef FEATURE_DAC_MEMORY_PROTECTION
; --------------------------------------------------------------------
; Lock EFS WR and ZI memory section
; --------------------------------------------------------------------
    ldr     r0, =MMU_PROTECT_EFS_MEMORY_VAL
    mcr     p15, 0 , r0, c3, c0, 0
#endif
    ldr     r3, =rex_irq_vector
    ldr     r3, [r3]
    blatox  r3
#ifdef FEATURE_DAC_MEMORY_PROTECTION
; --------------------------------------------------------------------
; Set EFS WR and ZI memory section to its previous state
; --------------------------------------------------------------------
    ldr     r0, =mmu_dac_shadow
    ldr     r0, [r0]
    mcr     p15, 0 , r0, c3, c0, 0
#endif
; --------------------------------------------------------------------
; Pop r14 from stack and switch to IRQ Mode.
; --------------------------------------------------------------------
    ldmfd   sp!, {r14}
    msr     CPSR_c, #PSR_Irq:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask

; --------------------------------------------------------------------
; Decrement the nesting level counter.
; --------------------------------------------------------------------
    ldr       r0, =rex_int_nest_level
    ldrh      r1, [r0]
    subs      r1, r1, #1      ; Notice it is subs. Set Z flag if r1==0
    strh      r1, [r0]

    ; ----------------------------------------------------------------
    ; Set timetest_isr_level to the new value
    ; ----------------------------------------------------------------
#ifdef TIMETEST_PROF
    ldr       r0, =timetest_isr_level
    strb      r1, [r0]
#endif

;---------------------------------------------------------------------
; Branch if rex_int_nest_level <> 0.
; The Z flag was set (or not set) by the subs instruction above.
;---------------------------------------------------------------------
    bne       nested_interrupt

;---------------------------------------------------------------------
; Is task swap required even though rex_int_nest_level == 0?
; Not required if curr task is same as best task.
;---------------------------------------------------------------------
    ldr     r0, =rex_curr_task
    ldr     r0, [r0]
    ldr     r1, =rex_best_task
    ldr     r1, [r1]
    cmp     r0, r1
    beq     task_swap_not_required

;---------------------------------------------------------------------
; If a TASKLOCK is in effect, then we do not need to even consider
; swapping tasks.
;---------------------------------------------------------------------
                                  ; test for TASKLOCK in effect
    ldr    r2, =rex_sched_allow
    ldr    r2, [r2]
    cmp    r2, #0
    beq    task_swap_not_required

;---------------------------------------------------------------------
; Restore the SPSR from stack.
;---------------------------------------------------------------------
    ldmfd     sp!, {r0}
    msr       SPSR_f, r0
    msr       SPSR_c, r0

;---------------------------------------------------------------------
; Restore the registers from IRQ mode stack.
;---------------------------------------------------------------------
    ldmfd     sp!, {r0-r3, r10, r12, r14}

;---------------------------------------------------------------------
; Save context of outgoing task.
;---------------------------------------------------------------------

    msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask

    sub     sp, sp, #4             ; Space for Return address (pc)
    stmfd   sp!, {r0-r12,lr}    ; Other registers
    sub     sp, sp, #4             ; Space for SPSR
    mov     r1, sp

    ldr     r0, =rex_curr_task         ; Save sp of outgoing task into TCB.
    ldr     r2, [r0]                   ;
    str     r1, [r2, #REX_TCB_STACK_POINTER_OFFSET]

    msr     CPSR_c, #PSR_Irq:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask  ; change to IRQ mode.
                                              ; save Return address into context frame.
    str     r14,  [r1, #REX_INTERRUPT_FRAME_PC_OFFSET]
    mrs     r3, SPSR                          ; save SPSR into context frame.
    str     r3,  [r1, #REX_INTERRUPT_FRAME_SPSR_OFFSET]

                                              ; Back to Supervisor
    msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask

;---------------------------------------------------------------------
; Restore context of incoming task.
;---------------------------------------------------------------------
; Reminder ... r0 = address of rex_curr_task variable.
;---------------------------------------------------------------------

    ldr     r1, =rex_best_task
    ldr     r1, [r1]
#ifdef FEATURE_DAC_MEMORY_PROTECTION
; --------------------------------------------------------------------
; We have r0 = rex_curr_task and r1 = rex_best_task
; Assert the dac from dac offset of rex_best_task
; --------------------------------------------------------------------
    stmfd   sp!, {r0-r3}
    ldr     r0, [r1, #REX_TCB_DAC_OFFSET]
    blx     mmu_set_dac
    ldmfd   sp!, {r0-r3}
#endif
    str     r1, [r0]           ; curr_task <- best_task

#if defined TIMETEST
;---------------------------------------------------------------------
; TIMETEST - writing current task id on leds port
;---------------------------------------------------------------------
    mov     r0, r1          ; r1 contains rex-best-task
    bl      func_timetest
    mov     r1, r0
#endif

#if defined(FEATURE_USE_TIME_VU) && defined(FEATURE_REX_PROFILE)
    ldr     r2, =rex_profiling_is_enabled        ; load rex_profiling_is_enabled flag
    ldrb    r2, [r2]                             ; dereference the flag
    cmp     r2, #1                               ; compare with TRUE
    bne     profile_skip_irq1                    ; skip if FALSE

    ldr     a3, =HWIO_SLEEP_XTAL_TIMETICK_COUNT_ADDR
    ldr     a4, [a3]                 ; load first sleep xlat timetick
rex_sleep_xtal_timetick_loop_in_irq_handler_2

    mov     a2, a4                   ; copy the new count for next comparison
    ldr     a4, [a3]                 ; load sleep xlat timetick again
    cmp     a4, a2                   ; if they do not match, try again
    bne     rex_sleep_xtal_timetick_loop_in_irq_handler_2
    ldr     a1, =rextp_last_swap_timetick

    ; Calculate the elapsed time
    ; Since the cur_tick could have rolled over, the following binary
    ; arithematic trick is used to calculate the difference between
    ; cur_tick and rextp_last_swap_timetick
    ; elapse_timetick = ~rextp_last_swap_timetick + 1 + cur_tick


    ldr     a3, [a1]                 ; a3 = rextp_last_swap_timetick
    str     a2, [a1]                 ; update rextp_last_swap_timetick
    mvn     a4, #0                   ; prepare 0xffff-ffff
    eor     a3, a3, a4               ; ~rextp_last_swap_timetick
    add     a3, a3, #1               ; ~rextp_last_swap_timetick + 1
    add     a3, a3, a2               ; elapse_timetick (a3) = ~rextp_last_swap_timetick + 1 + cur_tick

    ;rex_kernel_tcb.time_samples += elapse_timetick;
    ldr     a1, =rex_kernel_tcb
    ldr     a2, [a1, #REX_TCB_TIME_SAMPLE_OFFSET] ; a2 = rex_kernel_tcb.time_samples
    add     a2, a2, a3               ; add elapse_timetick
    str     a2, [a1, #REX_TCB_TIME_SAMPLE_OFFSET] ; save it back to rex_curr_task structure

    ;rex_total_time_samples += elapse_timetick;
    ldr     a1, =rex_total_time_samples
    ldr     a2, [a1]
    add     a2, a2, a3
    str     a2, [a1]

    ; rextp_max_elapse_timetick = MAX(rextp_max_elapse_timetick, elapse_timetick)
    ldr     a1, =rextp_max_elapse_timetick
    ldr     a2, [a1]
    cmp     a3, a2                  ; if elapse_timetick > rextp_max_elapse_timetick
    strhi   a3, [a1]                ;  rextp_max_elapse_timetick =  elapse_timetick



    ; restore r1
    ldr     r1, =rex_curr_task
    ldr     r1, [r1]

profile_skip_irq1
#endif

#if defined FEATURE_SOFTWARE_PROFILE

; do not alter r1 here

;---------------------------------------------------------------------
; Task(A) -> IRQ -> Task(B)
;---------------------------------------------------------------------
    ldr     r2, =rex_profile_enabled             ; load rex_profile_enabled flag
    ldrb    r2, [r2]                             ; dereference the flag
    cmp     r2, #1                               ; compare with TRUE
    bne     profile_skip_irq1                    ; skip if FALSE

    ldr     r0, =rex_time_tick2
    ldr     r2, [r0]
    ldrh    r0, [r2]

    ldr     r2, =rex_pnumticks
    ldrh    r3, [r2]
    strh    r0, [r2]

;=============================================================================
; r0 is tick_tick2 & r1 is rex_pnumticks (both 16 bits)
; overflow calculation of r0-r1 is simply !r1+1+r0
;=============================================================================

    mvn     r2, #0                               ; 0xffffffff
    eor     r3, r3, r2, lsr #16                  ; r3 = r3 eor 0x0000ffff
    add     r3, r3, #0x01
    add     r3, r3, r0                           ; r3 = amount to increment
    and     r3, r3, r2, lsr #16                  ; r3 = r3 and 0x0000ffff

    ldr     r2, =rex_IRQnumticks
    ldr     r0, [r2]                             ; Load rex_IRQnumticks
    add     r0, r0, r3                           ; increment rex_IRQnumticks

    str     r0, [r2]                             ; Write to rex_IRQnumticks

profile_skip_irq1

#endif

#ifdef FEATURE_REX_STAT
#error code not present
#endif

#if defined(FEATURE_REX_TRACE) && !defined(BUILD_BOOT_CHAIN)
; --------------------------------------------------------------------
; We are entering the new task
; --------------------------------------------------------------------
    ldr     r0, =rex_trace_enabled
    ldrb    r0, [r0]
    cmp     r0,#1
    bne     rex_trace_skip1
    stmfd   sp!, {r1}         ; save what we touch
    ldr     r2, [r1, #REX_TCB_STACK_POINTER_OFFSET]    ; Stack pointer
    ldr     r0, [r2, #64]     ; skip past {r1,SPSR,r0-12,r14} to get pc
    ldr     r1, =rex_trace_task_enter
    blatox  r1
    ldmfd   sp!, {r1}         ; restore registers
rex_trace_skip1
#endif

    ldr     sp, [r1, #REX_TCB_STACK_POINTER_OFFSET]    ; Stack pointer
    ldmfd   sp!, {r0}                                  ; Get SPSR
    msr     SPSR_f, r0
    msr     SPSR_c, r0                                 ; Restore SPSR.
    mov     a2, sp
    add     sp, sp, #REX_CF_SIZE - 4  ; adjust sp
    ldmfd   a2, {r0-r12, r14, pc}^    ; Load and return
                                      ; This will also restore CPSR.

;---------------------------------------------------------------------
; Control comes here when a task swap is not required.
;---------------------------------------------------------------------
task_swap_not_required
    ;---------------------------------------------------------------------
    ; Restore the SPSR from stack.
    ;---------------------------------------------------------------------
    ldmfd     sp!, {r0}
    msr       SPSR_f, r0
    msr       SPSR_c, r0

#if defined TIMETEST
;---------------------------------------------------------------------
; TIMETEST - writing current task id on leds port. If TASKLOCK is in
; effect, this is rex_curr_task, otherwise it is rex_best_task.
;---------------------------------------------------------------------
    ldr     r0, =rex_sched_allow
    ldr     r0, [r0]
    cmp     r0, #0
    bne     load_best_task
    ldr     r0, =rex_curr_task
    b       skip_best_task
load_best_task
    ldr     r0, =rex_best_task
skip_best_task
    ldr     r0, [r0]
    mov r12, lr          ; cannot destroy lr
    bl func_timetest
    mov lr, r12
#endif

#if defined(FEATURE_USE_TIME_VU) && defined(FEATURE_REX_PROFILE)
    ldr     r2, =rex_profiling_is_enabled        ; load rex_profiling_is_enabled flag
    ldrb    r2, [r2]                             ; dereference the flag
    cmp     r2, #1                               ; compare with TRUE
    bne     profile_skip_irq2                    ; skip if FALSE

    ldr     a3, =HWIO_SLEEP_XTAL_TIMETICK_COUNT_ADDR
    ldr     a4, [a3]                 ; load first sleep xlat timetick
rex_sleep_xtal_timetick_loop_in_irq_handler_3

    mov     a2, a4                   ; copy the new count for next comparison
    ldr     a4, [a3]                 ; load sleep xlat timetick again
    cmp     a4, a2                   ; if they do not match, try again
    bne     rex_sleep_xtal_timetick_loop_in_irq_handler_3
    ldr     a1, =rextp_last_swap_timetick
    ldr     a3, [a1]                 ; a3 = rextp_last_swap_timetick
    str     a2, [a1]                 ; update rextp_last_swap_timetick
    mvn     a4, #0                   ; prepare 0xffff-ffff
    eor     a3, a3, a4               ; ~rextp_last_swap_timetick
    add     a3, a3, #1               ; ~rextp_last_swap_timetick + 1
    add     a3, a3, a2               ; elapse_timetick (a3) = ~rextp_last_swap_timetick + 1 + cur_tick

    ;rex_kernel_tcb.time_samples += elapse_timetick;
    ldr     a1, =rex_kernel_tcb
    ldr     a2, [a1, #REX_TCB_TIME_SAMPLE_OFFSET] ; a2 = rex_kernel_tcb.time_samples
    add     a2, a2, a3               ; add elapse_timetick
    str     a2, [a1, #REX_TCB_TIME_SAMPLE_OFFSET] ; save it back to rex_curr_task structure

    ;rex_total_time_samples += elapse_timetick;
    ldr     a1, =rex_total_time_samples
    ldr     a2, [a1]
    add     a2, a2, a3
    str     a2, [a1]

    ; rextp_max_elapse_timetick = MAX(rextp_max_elapse_timetick, elapse_timetick)
    ldr     a1, =rextp_max_elapse_timetick
    ldr     a2, [a1]
    cmp     a3, a2                  ; if elapse_timetick > rextp_max_elapse_timetick
    strhi   a3, [a1]                ;  rextp_max_elapse_timetick =  elapse_timetick


profile_skip_irq2
#endif

#if defined FEATURE_SOFTWARE_PROFILE
;---------------------------------------------------------------------
; Task(A) -> IRQ -> Task(A)
;---------------------------------------------------------------------
    ldr     r2, =rex_profile_enabled             ; load rex_profile_enabled flag
    ldrb    r2, [r2]                             ; dereference the flag
    cmp     r2, #1                               ; compare with TRUE
    bne     profile_skip_irq2                    ; skip if FALSE

    ldr     r2, =rex_time_tick2
    ldr     r1, [r2]
    ldrh    r0, [r1]

    ldr     r2, =rex_pnumticks
    ldrh    r1, [r2]
    strh    r0, [r2]

;=============================================================================
; r0 is tick_tick2 & r1 is rex_pnumticks (both 16 bits)
; overflow calculation of r0-r1 is simply !r1+1+r0
;=============================================================================

    mvn     r2, #0                               ; 0xffffffff
    eor     r1, r1, r2, lsr #16                  ; r1 = r1 eor 0x0000ffff
    add     r1, r1, #0x01
    add     r1, r1, r0                           ; r1 = amount to increment
    and     r1, r1, r2, lsr #16                  ; r1 = r1 and 0x0000ffff

    ldr     r2, =rex_IRQnumticks
    ldr     r0, [r2]                             ; Load rex_IRQnumticks
    add     r0, r0, r1                           ; increment rex_IRQnumticks

    str     r0, [r2]                             ; Write to rex_IRQnumticks

profile_skip_irq2

#endif

#ifdef FEATURE_REX_STAT
#error code not present
#endif

#if defined(FEATURE_REX_TRACE) && !defined(BUILD_BOOT_CHAIN)
; --------------------------------------------------------------------
; We are returning to the task
; --------------------------------------------------------------------
    ldr     r1, =rex_trace_enabled
    ldrb    r1, [r1]
    cmp     r1,#1
    bne     rex_trace_skip2
    stmfd   sp!, {lr}         ; Save what we touch
    mov     r0, lr            ; current lr is the task pc
    ldr     r1, =rex_trace_task_enter
    blatox  r1
    ldmfd   sp!, {lr}         ; Restore registers
rex_trace_skip2
#endif

    ;-----------------------------------------------------------------
    ; Load saved registers and return
    ; This will also restore CPSR.
    ;-----------------------------------------------------------------
    ldmfd   sp!, {r0-r3, r10, r12, pc}^


;---------------------------------------------------------------------
; Control comes here when a nested interrupt continues.
;---------------------------------------------------------------------
nested_interrupt

;---------------------------------------------------------------------
; Restore the SPSR from stack.
;---------------------------------------------------------------------
    ldmfd     sp!, {r0}
    msr       SPSR_f, r0
    msr       SPSR_c, r0


#if (defined TIMETEST_PROF) && (! defined TIMETEST_NO_ISRS)
;---------------------------------------------------------------------
; Restore the old ISR ID for TIMETEST
;---------------------------------------------------------------------
    ldr       r0, =timetest_isr_markers ; Load the ISR ID stack
    ldrb      r2, [r0, r1]              ; Get the old ISRs ID
    ;-----------------------------------------------------------------
    ; r1 has the current nesting level.
    ;-----------------------------------------------------------------
    mov       r3, r1, LSL #8            ; Left shift isr level by 8
    orr       r2, r2, r3                ; OR in nesting level
    mov       r1, #1                    ; Set MSB for returning from
    mov       r1, r1, LSL #15           ; nested interrupts
    orr       r2, r2, r1
    ldr       r1, =TIMETEST_PORT        ; Get the TT port addr

#ifdef TIMETEST_INV_TRIGGER
#error code not present
#else /* TIMETEST_INV_TRIGGER */

    strh      r2, [r1]                  ; Write the old ISR ID out
#ifdef TIMETEST_EXPLICIT_TRIGGER
    mov       r0, #0
    strh      r0, [r1]
#endif

#endif /* TIMETEST_INV_TRIGGER */
#endif

#ifdef FEATURE_REX_STAT
#error code not present
#endif

    ;-----------------------------------------------------------------
    ; Load saved registers and return
    ; This will also restore CPSR.
    ;-----------------------------------------------------------------
    ldmfd   sp!, {r0-r3, r10, r12, pc}^

IRQ_Handler_end

;=============================================================================
;
; FUNCTION FIQ_Handler
;
; DESCRIPTION
;   Entry point of the FIQ interrupt handling services.
;   For future implementation.
;
; FORMAL PARAMETERS
;   None
;
; DEPENDENCIES
;   No task switch should happen in this ISR
;
; RETURN VALUE
;   None
;
; SIDE EFFECTS
;   None
;
;=============================================================================
  EXPORT FIQ_Handler
  ALIGN
  ROUT

FIQ_Handler
#ifdef T_QSC1100
    ldr     r4, [sp], #4      ; pop out r4 from stack used by pbl_fiq_handler
#endif

    sub     lr, lr, #4        ; adjust return address (ARM requirement)
    stmfd   sp!, {a1-a4,lr}

; --------------------------------------------------------------------
; Check for FIQ stack overflow.
; --------------------------------------------------------------------
#ifdef FEATURE_STACK_CHECK
#if defined(FEATURE_LOG_FUNCTIONS)
    ldr     sl, =0xffffffff
#else
    ldr     sl, =fiq_stack
#endif    
    cmp     r13, sl
    bcs     __fiq_no_stkovrflow
    bl      __16__rt_stkovf_split_small
__fiq_no_stkovrflow
#endif

#if defined FEATURE_REX_PC_DUMP
#error code not present
#endif

post_pcdump

#if ((defined TIMETEST) && (! defined TIMETEST_PROF)) \
    || ((defined TIMETEST_PROF) && (defined TIMETEST_NO_ISRS))
; --------------------------------------------------------------------
; TIMETEST_INT is a special value denoting Interrupt.
; Write that value to the TIMETEST port.
; --------------------------------------------------------------------
    mov     r0, #TIMETEST_INT       ; Get that special value.
    ldr     r1, =rex_led_buf
    ldr     r2, =TIMETEST_PORT
    strh    r0, [r2]                ; Write it to TIMETEST port.
    strh    r0, [r1]                ; Shadow this val into rex_led_buf.
#endif

#if defined(FEATURE_STACK_CHECK)
#if defined FEATURE_INT_125_FIQ
#if defined(FEATURE_LOG_FUNCTIONS)
    ldr     sl, =0xffffffff
#else
    ldr   sl, =fiq_stack
#endif
#endif
#endif

; --------------------------------------------------------------------
; Call the interrupt handler
; --------------------------------------------------------------------

#if defined FEATURE_INT_125_FIQ
    ldr     a4, =rex_fiq_vector
    ldr     a4, [a4]
    blatox  a4
#endif

#if defined TIMETEST
; --------------------------------------------------------------------
; Each task is assigned a unique value which is stored in 'leds' field
; in TCB. Write the special value for this task onto TIMETEST port.
; --------------------------------------------------------------------
    ldr     a1, = rex_curr_task
    ldr     a1, [a1]
    bl      func_timetest
#endif


; func_timetest does not destroy a1

; #if defined FEATURE_SOFTWARE_PROFILE
;     bl      func_software_profile
; #endif


    ldmfd   sp!, {a1-a4,pc}^

FIQ_Handler_end


#if defined FEATURE_REX_PC_DUMP
#error code not present
#endif

;=============================================================================
;
; FUNCTION rex_sched
;
; DESCRIPTION
;   This function does the actual task swapping. It should only be called by
;   other rex functions and **NEVER** by the user.
;
;   Typically, rex_sched is called immediately after a REX service function
;   changes the best task pointer. This occurs, for example, when a task
;   of higher priority than the current task is reactivated. Rex_sched deter-
;   mines whether the current task pointer and best task pointer still point
;   to the same task. If so, then the current task is still the best task and
;   no further scheduling action is required. Rex_sched merely returns.
;
;   Otherwise, the current task is no longer the best task. Rex_sched first
;   sets the current task pointer equal to the best task pointer. It then
;   determines whether it has been called from task level or interrupt level.
;   If called from task level, rex_sched saves the context of the 'old'
;   current task and restores the context of the 'new' current task. If called
;   from interrupt level, rex_sched does not perform a context switch; the
;   switch will be deferred until a return is made to task level.
;
; FORMAL PARAMETERS
;   None.
;
; DEPENDENCIES
;   Rex_sched assumes that the following global variables have reasonable
;   values: rex_curr_task, rex_best_task.
;
; RETURN VALUE
;   None.
;
; SIDE EFFECTS
;   Rex_sched may set the current task pointer (rex_curr_task) equal to
;   the best task pointer (rex_best_task) if they are not already the same.
;   Also, rex_sched may cause a task swap to occur from the old current
;   task to the new current task.
;
;=============================================================================
 LEAF_NODE rex_sched
#if defined(FEATURE_STACK_CHECK) && defined(FEATURE_LOG_FUNCTIONS)
    stmfd   sp!, {r14}
#endif    

#ifdef FEATURE_STACK_CHECK
    cmp     r13, sl
    bcs     __rex_sched_no_stkovrflow
    bl      __16__rt_stkovf_split_small
__rex_sched_no_stkovrflow
#endif
#if defined(FEATURE_STACK_CHECK) && defined(FEATURE_LOG_FUNCTIONS)
    ldmfd   sp!, {r14}
#endif   
    mrs   a3, CPSR
    ldr   a1, = rex_cpsr_value
    str   a3, [a1]                 ;Save the CPSR for later.
    orr   a1, a3, #PSR_Irq_Mask:OR:PSR_Fiq_Mask
    msr   CPSR_c, a1

;---------------------------------------------------------------------------
; If we are in interrupt level, just return
;---------------------------------------------------------------------------
    and   a1, a3, #PSR_Mode_Mask
    cmp   a1, #PSR_Supervisor    ; If not in Supervisor mode do not swap
    bne   rex_sched_exit_1       ; until we revert back to task level

;---------------------------------------------------------------------------
; If a TASKLOCK is in effect, just return
;-----------------------------------------------------------------------
                                   ; test for TASKLOCK
   ldr   a2, =rex_sched_allow      ; load scheduling flag
   ldr   a2, [a2]                  ; dereference sched. flag
   cmp   a2, #0                    ; compare with FALSE
   beq   rex_sched_exit_1          ; return

;---------------------------------------------------------------------------
; Make sure we need to task swap
;---------------------------------------------------------------------------
    ldr   a2, =rex_best_task       ; load the best task into a2
    ldr   a2, [a2]                 ; dereference best task
    ldr   a1, = rex_task_to_schedule
    str   a2, [a1]                 ; Save the best task as the one to be
                                   ; scheduled next
;-----------------------------------------------------------------------
; rex_sched_common is an alternate entry point.
; void rex_sched_common(void);
;-----------------------------------------------------------------------
rex_sched_common
    ldr   a1, = rex_cpsr_value
    ldr   a3, [a1]                 ;Get the CPSR value. This will be
                                   ; used later to restore CPSR
    ldr   a4, =rex_curr_task       ; load the current task into a4
    ldr   a1, [a4]                 ; dereference current task
    ldr   a2, = rex_task_to_schedule
    ldr   a2, [a2]                 ; Load the task to be scheduled
                                   ; scheduled next
    cmp   a2, a1                   ; if current task == next task just return
    beq   rex_sched_exit_1
#if defined(FEATURE_USE_TIME_VU) && defined(FEATURE_REX_PROFILE)
    ldr     ip, =rex_profiling_is_enabled        ; load rex_profiling_is_enabled flag
    ldrb    ip, [ip]                             ; dereference the flag
    cmp     ip, #1                               ; compare with TRUE
    bne     profile_in_rex_sched_skip                   ; skip if FALSE

    ldr     ip, =HWIO_SLEEP_XTAL_TIMETICK_COUNT_ADDR
    ldr     a4, [ip]                 ; load first sleep xlat timetick
rex_sleep_xtal_timetick_loop_in_rex_sched

    mov     a2, a4                   ; copy the new count for next comparison
    ldr     a4, [ip]                 ; load sleep xlat timetick again
    cmp     a4, a2                   ; if they do not match, try again
    bne     rex_sleep_xtal_timetick_loop_in_rex_sched
    ldr     ip, =rextp_last_swap_timetick

    ; Calculate the elapsed time
    ; Since the cur_tick could have rolled over, the following binary
    ; arithematic trick is used to calculate the difference between
    ; cur_tick and rextp_last_swap_timetick
    ; elapse_timetick = ~rextp_last_swap_timetick + 1 + cur_tick

    ldr     a1, [ip]                 ; a3 = rextp_last_swap_timetick
    str     a2, [ip]                 ; update rextp_last_swap_timetick
    mvn     a4, #0                   ; prepare 0xffff-ffff
    eor     ip, a1, a4               ; ~rextp_last_swap_timetick
    add     ip, ip, #1               ; ~rextp_last_swap_timetick + 1
    add     ip, ip, a2               ; elapse_timetick (ip) = ~rextp_last_swap_timetick + 1 + cur_tick

    ;rex_curr_task->time_samples += elapse_timetick;
    ldr     a1, =rex_curr_task
    ldr     a1, [a1]                 ; a1 = *rex_curr_task
    ldr     a2, [a1, #REX_TCB_TIME_SAMPLE_OFFSET] ; a2 = rex_curr_task->time_samples
    add     a2, a2, ip               ; add elapse_timetick
    str     a2, [a1, #REX_TCB_TIME_SAMPLE_OFFSET] ; save it back to rex_curr_task structure

    ;rex_total_time_samples += elapse_timetick;
    ldr     a1, =rex_total_time_samples
    ldr     a2, [a1]
    add     a2, a2, ip
    str     a2, [a1]

    ; rextp_max_elapse_timetick = MAX(rextp_max_elapse_timetick, elapse_timetick)
    ldr     a1, =rextp_max_elapse_timetick
    ldr     a2, [a1]
    cmp     ip, a2                  ; if elapse_timetick > rextp_max_elapse_timetick
    strhi   ip, [a1]                ;  rextp_max_elapse_timetick =  elapse_timetick

    ldr   a4, =rex_curr_task       ; load the current task into a4
    ldr   a1, [a4]                 ; dereference current task
    ldr   a2, =rex_task_to_schedule; load the task to be scheduled(best task) into a2
    ldr   a2, [a2]                 ; dereference best task
profile_in_rex_sched_skip
#endif

#if defined(FEATURE_REX_TRACE) && !defined(BUILD_BOOT_CHAIN)
;---------------------------------------------------------------------------
; Call task tracing function
;---------------------------------------------------------------------------
    stmfd   sp!, {a1-a4,r14}
    ldr     a1, =rex_trace_enabled
    ldrb    a1, [a1]
    cmp     a1,#1
    bne     rex_trace_skip3
    mov     a1, lr              ; current lr becomes pc for the outgoing task
    ldr     a2, =rex_trace_task_leave
    blatox  a2
rex_trace_skip3
    ldmfd   sp!, {a1-a4,r14}
#endif

;---------------------------------------------------------------------------
; Set the curr_task to the new value
;---------------------------------------------------------------------------
    str a2, [a4]                 ; set rex_curr_task=rex_best_task
    mov a4, a1                   ; a4 points now to the last (former current) task

;---------------------------------------------------------------------------
; Increment the slice count.
;---------------------------------------------------------------------------
    ldr   a1, [a2, #REX_TCB_SLICES_OFFSET]       ; load up the slice count
    add   a1, a1, #1                             ; increment it
    str   a1, [a2, #REX_TCB_SLICES_OFFSET]       ; store it

; --------------------------------------------------------------------
; Save volatile context of CPU
; --------------------------------------------------------------------
    stmfd   sp!, {lr}               ; Return address.
    sub     sp, sp, #8              ; no need to store r12,r14 in task context.
    stmfd   sp!, {r4-r11}
    sub     sp, sp, #16             ; Subtract a1-a4 location
#ifdef __APCS_INTERWORK
    tst     lr, #1                  ; Test for thumb return address
    orrne   a3, a3, #PSR_Thumb_Mask ; Return in Thumb mode
#else
    orr     a3, a3, #PSR_Thumb_Mask ; Return in Thumb mode
#endif
    stmfd   sp!, {a3}               ; First line on rex_sched saves CPSR in a3!!!

;---------------------------------------------------------------------------
; Save the context on stack
;---------------------------------------------------------------------------
    str     sp, [a4,  #REX_TCB_STACK_POINTER_OFFSET]
    mov     a1, a2                  ; a1 = the current task

;---------------------------------------------------------------------------
; rex_start_task_1 is an alternate entry point.
; void rex_start_task(rex_tcb_type *);
; This implies that a1 = current task tcb pointer.
;---------------------------------------------------------------------------
rex_start_task_1

#if defined TIMETEST
; --------------------------------------------------------------------
; TIMETEST - writing current task leds on timetest port.
; --------------------------------------------------------------------
    bl      func_timetest
#endif

#if defined FEATURE_SOFTWARE_PROFILE
    bl      func_software_profile
#endif

#ifdef FEATURE_DAC_MEMORY_PROTECTION
; --------------------------------------------------------------------
; a1 contains the task getting scheduled. Assert the dac from dac offset
; --------------------------------------------------------------------
    mov a2, a1
    ldr a1, [a1, #REX_TCB_DAC_OFFSET]
    stmfd   sp!, {a2-a4} 
    blx     mmu_set_dac
    ldmfd   sp!, {a2-a4}
    mov a1, a2
#endif
; --------------------------------------------------------------------
; Restore the user state, note this may not be the state saved above
; since the call the rex_sched may have changed which stack the handler
; is working on. Note, a context switch will happen here.
; --------------------------------------------------------------------

    ldr     sp, [a1, #REX_TCB_STACK_POINTER_OFFSET]    ; Load the stack pointer
    ldmfd   sp!, {a1}                                  ; Restore SPSR (in a1)

#ifdef FEATURE_REX_STAT
#error code not present
#endif

#if defined(FEATURE_REX_TRACE) && !defined(BUILD_BOOT_CHAIN)
;---------------------------------------------------------------------------
; Call task tracing function
;---------------------------------------------------------------------------

    ldr     a2, =rex_trace_enabled
    ldrb    a2, [a2]
    cmp     a2,#1
    bne     rex_trace_skip4
    stmfd   sp!, {a1}
    ldr     a1, [sp, #60]     ; Skip past {a1,r0-12,r14} to get pc
    ldr     a2, =rex_trace_task_enter
    blatox  a2
    ldmfd   sp!, {a1}
rex_trace_skip4
#endif

    msr     SPSR_f, a1                 ; Load SPSR
    msr     SPSR_c, a1                 ; Load SPSR
    mov     a1, sp                     ; Load sp in a1.
    add     sp, sp, #REX_CF_SIZE - 4   ; adjust sp
    ldmfd   a1, {r0-r12,lr,pc}^        ; Load and return, sp already adjusted.

; --------------------------------------------------------------------
; If no context swap occurred, the execution exits thru here
; --------------------------------------------------------------------

rex_sched_exit_1
    msr   CPSR_f, a3               ; Restore interrupts as prior to rex_sched
    msr   CPSR_c, a3               ; Restore interrupts as prior to rex_sched

 LEAF_NODE_END

; END rex_sched

;=============================================================================
;
;  FUNCTION rex_start_task
;
;  DESCRIPTION
;    Spawn the task without saving any context. rex_start_task_1 is an entry
;    point in rex_sched, when context is re-installed. Used only by rex_init
;    to initialize user first task. Alternate_entries is an entry_node macro
;    that creates Thumb mode entry points to the scheduler.
;    This function should **NEVER** be called by user.
;
;  DEPENDENCIES
;    rex_curr_task must point to user first task tcb
;
;  RETURN VALUE
;    None
;
; SIDE EFFECTS
;    The user-first-task will be spawned
;
;=============================================================================

 ENTRY_NODE Alternate_entries
        CODE16
        EXPORT  rex_start_task
rex_start_task
        ldr     a4, =rex_start_task_1
        bx      a4

 ENTRY_NODE_END

;=============================================================================
;
;  FUNCTION rex_int_lock
;
;  DESCRIPTION
;    Disable the IRQ and FIQ exceptions.
;
;  DEPENDENCIES
;    None
;
;  PROTOTYPE
;    dword rex_int_lock(void)
;
;  RETURN VALUE
;    The previous state of the IRQ and FIQ flags.
;
;  SIDE EFFECTS
;    None
;
;=============================================================================
 LEAF_NODE rex_int_lock

        mrs     a1, CPSR
        orr     a2, a1, #PSR_Irq_Mask:OR:PSR_Fiq_Mask
        msr     CPSR_c, a2
        and     a1, a1, #PSR_Irq_Mask:OR:PSR_Fiq_Mask
#ifdef FEATURE_REX_STAT
#error code not present
#endif
 LEAF_NODE_END


;=============================================================================
;
;  FUNCTION rex_int_free
;
;  DESCRIPTION
;    Reenable the IRQ and FIQ exceptions.
;
;  DEPENDENCIES
;    None
;
;  PROTOTYPE
;    void rex_int_free(void)
;
;  RETURN VALUE
;    The previous state of the IRQ and FIQ flags.
;
;  SIDE EFFECTS
;    None
;
;=============================================================================
 LEAF_NODE rex_int_free

#ifdef FEATURE_REX_STAT
#error code not present
#endif
        mrs     a1, CPSR
        bic     a2, a1, #PSR_Irq_Mask:OR:PSR_Fiq_Mask
        msr     CPSR_c, a2
        and     a1, a1, #PSR_Irq_Mask:OR:PSR_Fiq_Mask

 LEAF_NODE_END

;=============================================================================
;
;  FUNCTION rex_ints_are_locked
;
;  DESCRIPTION
;    Returns 0 if IRQ is enabled and a non-zero value if IRQ is disabled.
;
;  DEPENDENCIES
;    None
;
;  PROTOTYPE
;    dword rex_ints_are_locked(void)
;
;  RETURN VALUE
;    The state of the IRQ flag.
;
;  SIDE EFFECTS
;    None
;
;=============================================================================
 LEAF_NODE rex_ints_are_locked

        mrs     a1, CPSR
        and     a1, a1, #PSR_Irq_Mask

 LEAF_NODE_END


;==========================================================================
;
;   FUNCTION REX_ENABLE_INTERRUPT
;
;   DESCRIPTION
;    Enables the interrupt associated with id v rex function.
;
;   DEPENDENCIES
;    None.
;
;  PROTOTYPE
;    rex_enable_interrupt( rex_vect_type v )
;
;  RETURN VALUE
;    None.
;
;  SIDE EFFECTS
;    None.
;
;===========================================================================

 LEAF_NODE rex_enable_interrupt

        mrs    r1, CPSR
        cmp    r0, #P_IRQ_VECT
        biceq  r1, r1, #PSR_Irq_Mask
        bicne  r1, r1, #PSR_Fiq_Mask
        msr    CPSR_c, r1

 LEAF_NODE_END

;===========================================================================
;
;   FUNCTION REX_DISABLE_INTERRUPT
;
;   DESCRIPTION
;    Disables the interrupt associated with id v rex function.
;
;   DEPENDENCIES
;    None.
;
;  PROTOTYPE
;    rex_disable_interrupt( rex_vect_type v )
;
;  RETURN VALUE
;    None.
;
;  SIDE EFFECTS
;    None.
;
;===========================================================================

 LEAF_NODE rex_disable_interrupt

        mrs    r1, CPSR
        cmp    r0, #P_IRQ_VECT
        orreq  r1, r1, #PSR_Irq_Mask
        orrne  r1, r1, #PSR_Fiq_Mask
        msr    CPSR_c, r1

 LEAF_NODE_END

;=============================================================================
;
;  ENTRY rex_set_cpsr
;
;  DESCRIPTION
;    Set CPSR to user specified value
;
;  DEPENDENCIES
;    Must be done as an ARM instruction so making this a leaf node causes
;    the compiler to handle getting in and out of ARM/Thumb mode
;
;  REGISTERS USED:
;    CPSR, a1, a2, a3,lr
;
;  SIDE EFFECTS
;    None
;
;=============================================================================

 LEAF_NODE rex_set_cpsr
        mrs     a3, CPSR       ; Save the value of the old CPSR
        mov     a2, lr         ; Save off the shadowed link register.
        msr     CPSR_f, a1
        msr     CPSR_c, a1
        mov     a1, a3         ; Return the old value of the msr
        mov     lr, a2
 LEAF_NODE_END

;=============================================================================
;
;  ENTRY rex_get_cpsr
;
;  DESCRIPTION
;    Get CPSR value
;
;  DEPENDENCIES
;    Must be done as an ARM instruction so making this a leaf node causes
;    the compiler to handle getting in and out of ARM/Thumb mode
;
;  REGISTERS USED:
;    CPSR, r0
;
;  SIDE EFFECTS
;    None
;
;=============================================================================

 LEAF_NODE rex_get_cpsr
        mrs     r0, CPSR       ; Return the value of the cpsr
 LEAF_NODE_END

;=============================================================================
;
;   FUNCTION DBGBreak
;
;   DESCRIPTION -
;     This function does nothing and its only purpose is as a place to set a
;     breakpoint for runtime code warnings.
;
;   DEPENDENCIES
;     None
;
;   RETURN VALUE
;     None
;
;   SIDE EFFECTS
;     None
;
;=============================================================================

 LEAF_NODE      DBGBreak
         b      DBGBreak_32
 LEAF_NODE_END


#if defined TIMETEST
;=============================================================================
;
;   FUNCTION FUNC_TIMETEST
;
;   DESCRIPTION -
;     Read the leds field from the current task TCB.
;     Each task is assigned a unique value.
;     Write that value to TIMETEST_PORT and also shadow it in
;     the global variable rex_led_buf.
;
;   FORMAL PARAMETERS
;     a1 - current TCB pointer
;
;   DEPENDENCIES
;     Only called in ARM mode
;
;   RETURN VALUE
;     None
;
;   SIDE EFFECTS
;     Registers destroyed:   a2, a3, a4, lr
;
;=============================================================================
func_timetest

#if defined TIMETEST_PROF

#ifdef TIMETEST_INV_TRIGGER
#error code not present
#else /* TIMETEST_INV_TRIGGER */

#ifndef TIMETEST_NO_TASKS
    ; write the task id to the led port
    ldr     a4, =TIMETEST_PORT            ; Restore the LED port address
    ldr     a2, [a1, #REX_TCB_LED_OFFSET] ; write task id to led port
    strh    a2, [a4]                      ; dito
#ifdef TIMETEST_EXPLICIT_TRIGGER
    mov     a2, #0                        ; write token '0000' to led port
    strh    a2, [a4]                      ; dito
#endif
#endif

#endif /* TIMETEST_INV_TRIGGER */

#if defined TIMETEST_FUNCS
    ; check if we just wrote an empty marker and do not repeat it
    ldr     a3, =timetest_parse_token
    ldr     a4, [a3]
    ldr     a3, =rex_led_buf
    ldrh    a2, [a3]
    cmp     a2, a4
    beq     done


#ifdef TIMETEST_INV_TRIGGER
#error code not present
#else /* TIMETEST_INV_TRIGGER */

    ; write token 'ffff' to the led port
    ldr     a4, =timetest_parse_token     ; load the parse token into a2
    ldr     a2, [a4]                      ; dito
    ldr     a4, =TIMETEST_PORT            ; Restore the LED port address
    strh    a2, [a4]                      ; write parse token to led port
    strh    a2, [a3]
    mov     a2, #0                        ; write token '0000' to led port
    strh    a2, [a4]                      ; dito

#endif /* TIMETEST_INV_TRIGGER */

    ; load function level counter for task a1 into a3
    ldrh    a2, [a1, #REX_TCB_LED_OFFSET] ; Load Task id (1-31) into a2
    cmp     a2, #0                        ; Sanity check on the task id
    beq     done                          ; return if not set
    mov     a2, a2, LSL #5                ; Index for uint32[32][32] table
    ldr     a4, =timetest_markers         ; Load marker array base address
    ldr     a3, [a4, a2]                  ; Load table entry value

    ; only write function address out if task contains instrumented function
    cmp     a3, #0
    beq     done

    ; load into a2:
    ; timetest_markers[rex_self()->leds][timetest_markers[rex_self()->leds][0]
    mov     a3, a3, LSL #2                ; get offset within task column
    add     a2, a2, a3                    ; get offset into table
    ldr     a3, [a4, a2]                  ; load current func addr
    ldr     a4, =TIMETEST_PORT            ; Restore the LED port address
    mov     a2, #0                        ; load token '0000' into a2

    ; write array value in two half-words (led port is 16-bit wide)
#ifdef TIMETEST_INV_TRIGGER
#error code not present
#else /* TIMETEST_INV_TRIGGER */
    strh    a3, [a4]                      ; write two least significant bytes
    strh    a2, [a4]                      ; write token '0000' to led port
#endif /* TIMETEST_INV_TRIGGER */

    mov     a3, a3, LSR #16               ; shift array value by two bytes
#ifdef TIMETEST_INV_TRIGGER
#error code not present
#else /* TIMETEST_INV_TRIGGER */
    strh    a3, [a4]                      ; write two most significant bytes
    strh    a2, [a4]                      ; write token '0000' to led port
#endif /* TIMETEST_INV_TRIGGER */

    ldr     a4, =rex_led_buf
    ldrh    a3, [a4]                      ; read rex_led_buf
    bic     a3, a3, #&1f                  ; mask lower 5 bits
    orr     a2, a2, a3
    strh    a2, [a4]                      ; Save back in leds buffer
    ldr     a4, =TIMETEST_PORT            ; Write to the Led port
    strh    a2, [a4]

done

#endif

    mov     pc, lr                        ; Return

#else

    ldrh    a2, [a1, #REX_TCB_LED_OFFSET] ; Load Task id (0-31)
    ldr     a4, =rex_led_buf
    ldrh    a3, [a4]                      ; read rex_led_buf
    bic     a3, a3, #&1f                  ; mask lower 5 bits
    orr     a2, a2, a3
    strh    a2, [a4]                      ; Save back in leds buffer
    ldr     a4, =TIMETEST_PORT            ; Write to the Led port
    strh    a2, [a4]

    mov     pc, lr                        ; Return

#endif

;=============================================================================
; TIMETEST endif
;=============================================================================
#endif


#ifdef FEATURE_STACK_CHECK


; For stack checking, the linker needs the following 4 functions defined.
; We only really care about "__rt_stack_overflow".  The other 3 are never
; called.  But we do not want ARM to link in its standard routines, so we
; provide our versions here.  So you can put a breakpoint on
; "__rt_stack_overflow", probably in a .cmm file, and catch stack overflows.


  EXPORT __rt_stackheap_init
__rt_stackheap_init


  EXPORT __rt_heap_extend
__rt_heap_extend


  EXPORT __rt_stack_postlongjmp
__rt_stack_postlongjmp


        mov     pc, lr    ; Do nothing, and return.


; The 3 above functions should never, ever, never get called.  But if for some
; wild ARM reason they do get called, they do absolutely nothing and just
; return.


  EXPORT __rt_stack_overflow
#ifdef FEATURE_LOG_FUNCTIONS  
  
__rt_stack_overflow
        stmfd sp!,    {r0-r3,r10-r12,r14}

        eor     r10, r10, r10    ; Clear the stack limit register.

        ; Call  rex_log_functions
        mov  r0, r14
        ldr  r1, =rex_log_functions
        blx  r1
        ldmfd sp!,    {r0-r3,r10-r12,r14}
        bx      r14
__rt_stack_overflow_end

#else
__rt_stack_overflow


        mov     r0, r13     ; First  paramter for rex_report_stack_overflow.
        mov     r1, r14     ; Second paramter for rex_report_stack_overflow.
        mrs     r2, CPSR    ; Third parameter for rex_report_stack_overflow.


        eor     r10, r10, r10    ; Clear the stack limit register.


        ; Call  void rex_report_stack_overflow(dword sp, dword lr, dword mode);


        ldr     r3, =rex_report_stack_overflow
        blatox  r3


spin_forever
        b       spin_forever   ; Depending upon "rex_report_stack_overflow",
                               ; we may never reach this point.  But if we do,
                               ; just wait for a watch dog reset.


__rt_stack_overflow_end
#endif
#endif


#if defined FEATURE_SOFTWARE_PROFILE
;=============================================================================
;
;   FUNCTION FUNC_SOFTWARE_PROFILE
;
;   DESCRIPTION -
;     Read the TIME_TICK2_COUNT_WH and update rex_pnumticks.
;     Calculate the ticks passed and Write the result to the
;     numticks field of the current task TCB.
;
;   FORMAL PARAMETERS
;     a1 - current TCB pointer
;
;   DEPENDENCIES
;     Only called in ARM mode
;
;   RETURN VALUE
;     None
;
;   SIDE EFFECTS
;     Registers destroyed:   a2, a3, a4, lr
;
;=============================================================================
func_software_profile

    ldr     a2, =rex_profile_enabled             ; load rex_profile_enabled flag
    ldrb    a2, [a2]                             ; dereference the flag
    cmp     a2, #1                               ; compare with TRUE
    bne     profile_skip_func                    ; skip if FALSE

    ldr     a4, =rex_time_tick2
    ldr     a3, [a4]
    ldrh    a2, [a3]

    ldr     a4, =rex_pnumticks
    ldrh    a3, [a4]
    strh    a2, [a4]

;=============================================================================
; a2 is tick_tick2 & a3 is rex_pnumticks (both 16 bits)
; overflow calculation of a2-a3 is simply !a3+1+a2
;=============================================================================

    mvn     a4, #0                               ; 0xffffffff
    eor     a3, a3, a4, lsr #16                  ; a3 = a3 eor 0x0000ffff
    add     a3, a3, #0x01
    add     a3, a3, a2                           ; a3 = amount to increment
    and     a3, a3, a4, lsr #16                  ; a3 = a3 and 0x0000ffff

    ldr     a2, [a1, #REX_TCB_NUMTICKS_OFFSET]   ; load TCB numticks
    add     a2, a2, a3                           ; increment TCB numticks

    str     a2, [a1, #REX_TCB_NUMTICKS_OFFSET]   ; Write to TCB numticks

profile_skip_func

    mov     pc, lr                               ; Return

;=============================================================================
; FEATURE_SOFTWARE_PROFILE endif
;=============================================================================
#endif



#ifdef NO_STDLIB_STKOVF
#if defined FEATURE_STACK_CHECK
;=============================================================================
;
;  FUNCTION __16__rt_stkovf_split_small
;           __16__rt_stkovf_split_big
;
;  DESCRIPTION
;    These functions are stack overflow handlers which are called when a stack
;    over flow is detected.
;
;    __16__rt_stkovf_split_small  is called if the frame is less than 256 bytes.
;    __16__rt_stkovf_split_big    is called if the frame is more than 256 bytes.
;
;    We do not differntiate between small/big frames.
;
;    This function calls a C routine which calls ERR_FATAL.
;
;    It clears the stack limit register before calling the C routine since
;    we do not want the stack check to fire anymore.
;    This routine had to be written in assembly because the ARM tcc compiler
;    does not allow modification of stack limit register in inline assembly
;    code (__asm).
;
;    The C function, rex_report_stack_overflow is required because
;    ERR_FATAL macro is available only in C files.
;
;    Stack checking code is inserted in the beginning of each function by the
;    compiler when code is compiled with swstackcheck option.
;
;  DEPENDENCIES
;    The macros LEAF_NODE and LEAF_NODE_END are being used to define the
;    entry and exit points. Although these macros are meant to be used in
;    functions which do not make function calls, it is acceptable to use
;    them here since this is a suicidal path anyway (ERR_FATAL to be called)
;    and does not need to preserve the stack frame.
;
;  RETURN VALUE
;    None. This function branches to a C routine which never returns.
;
;  SIDE EFFECTS
;    None.
;
;=============================================================================
 #ifdef FEATURE_LOG_FUNCTIONS
  LEAF_NODE  __16__rt_stkovf_split_small

        stmfd sp!,    {r0-r3,r10-r12,r14}

        eor     r10, r10, r10    ; Clear the stack limit register.

        ; Call  rex_log_functions
        mov  r0, r14
        ldr  r1, =rex_log_functions
        blx  r1
        ldmfd sp!,    {r0-r3,r10-r12,r14}
        bx    r14
  LEAF_NODE_END
 #else
  LEAF_NODE  __16__rt_stkovf_split_small

        mov     r0, r13     ; First  paramter for rex_report_stack_overflow.
        mov     r1, r14     ; Second paramter for rex_report_stack_overflow.
        mrs     r2, CPSR    ; Third parameter for rex_report_stack_overflow.

        eor     r10, r10, r10    ; Clear the stack limit register.

        ; Call  void rex_report_stack_overflow(dword sp, dword lr, dword mode);

        ldr     r3, =rex_report_stack_overflow
        blatox  r3

inf_loop
        b       inf_loop    ;  Should never reach this line.
                            ;  If it does then the infinite loop will trigger
                            ;  the watchdog.

  LEAF_NODE_END

 #endif
  LEAF_NODE __16__rt_stkovf_split_big

        ;------------------------------------------------------------------
        ; Simply branch to the routine above.
        ; Do not link because lr should not change.
        ;------------------------------------------------------------------
        b    __16__rt_stkovf_split_small_32

  LEAF_NODE_END

;=============================================================================
; FEATURE_STACK_CHECK endif
;=============================================================================
#endif

#endif /* NO_STDLIB_STKOVF */

#ifdef FEATURE_REX_APC

;=============================================================================
;
;  ENTRY rex_apc_epilog
;
;  DESCRIPTION
;    Performs processing needed after a rex_apc_call call. After the APC
;    has been executed, there are two cases to consider:
;    (a) A context switch is not needed.
;        In this case, just pop the top context frame off the stack of the
;        current thread and restore registers using this frame.
;
;    (b) A context switch is needed.
;        In this case, save the stack pointer of the current thread in its
;        TCB. It is not necessary to save a context frame because one gets
;        pushed when an APC is queued.
;
;  DEPENDENCIES
;    Should only be called when in Supervisor mode.
;
;  REGISTERS USED:
;    SPSR, sp, a1, a2, a3
;
;  SIDE EFFECTS
;    None
;
;=============================================================================
 LEAF_NODE rex_apc_epilog

    ldr     a1, =rex_curr_task
    ldr     a2, [a1]                       ; Load curr task into a2.
    ldr     a3, =rex_best_task
    ldr     a3, [a3]                       ; Load best task into a3.
    cmp     a3, a2                         ; Check if curr_task = best_task.
    beq     rex_apc_epilog_after_swap
    str     sp, [a2, #REX_TCB_STACK_POINTER_OFFSET]
                                             ; Update stack pointer in TCB.
    str     a3, [a1]                         ; Set curr task to best task.
    ldr     sp, [a3, #REX_TCB_STACK_POINTER_OFFSET]
                                             ; Load the new stack pointer.

#ifdef FEATURE_DAC_MEMORY_PROTECTION
; --------------------------------------------------------------------
; We now have a3 = rex_curr_task which is taken from rex_best_task
; Assert the dac from dac offset of rex_best_task
; --------------------------------------------------------------------
    stmfd   sp!, {a1-a4} 
    ldr     a1, [a3, #REX_TCB_DAC_OFFSET]
    blx     mmu_set_dac
    ldmfd   sp!, {a1-a4}
#endif

#ifdef TIMETEST
    mov     a1, a3
    bl      func_timetest
#endif

rex_apc_epilog_after_swap
    ldmfd   sp!, {a2}                        ; Restore SPSR (in a2).

#ifdef FEATURE_REX_STAT
#error code not present
#endif

    msr     SPSR_f, a2                       ; Load SPSR.
    msr     SPSR_c, a2                       ; Load SPSR.
    mov     a2, sp                           ; Load sp in a2.
    add     sp, sp, #REX_CF_SIZE - 4         ; Adjust sp.
    ldmfd   a2, {r0-r12, r14, pc}^           ; Load and return
                                             ; sp already adjusted.
  LEAF_NODE_END

#endif /* FEATURE_REX_APC */

#if defined(FEATURE_STACK_CHECK) && defined(FEATURE_LOG_FUNCTIONS)

rex_log_enable DCD REX_LOG_DISABLE
   EXPORT rex_log_enable

rex_log_functions
    ldr     r1, = rex_log_enable
    ldr     r3, [r1]
    ldr     r2, =REX_LOG_ENABLE
    cmp     r2, r3
    bne     exit_log
    ldr     r10, =REX_CURR_LOG_PTR
    ldr     r12, [r10]
    ldr     r10, =REX_END_LOG_PTR 
    cmp     r12,  r10
    ble     continue_log
    ldr     r12, =REX_START_LOG_PTR 
continue_log
    stmia   r12!, {r0}      ; store the function pointer
    ldr     r10, =REX_CURR_LOG_PTR
    str     r12,  [r10]      ; save the current pointer
exit_log
    bx      lr
#endif /* FEATURE_STACK_CHECK && FEATURE_LOG_FUNCTIONS */

#ifdef FEATURE_PROGRESSIVE_BOOT

;=============================================================================
;
;  ENTRY rex_save_context
;
;  DESCRIPTION
;    Saves the context frame of the current task on to its stack.
;    NOTE: Only works if called from abort mode.
;
;  REGISTERS USED:
;    CPSR, SPSR, sp, lr, r0, r1
;
;=============================================================================
  LEAF_NODE rex_save_context

;    mrs     r0, cpsr
;    stmdb   sp, {r0,r4-r12}             ; save r4 to r12
    stmdb   sp, {r4-r12}             ; save r4 to r12

    add     sp, sp, #ARM_TRAP_FRAME_SIZE
    ldmdb   sp!, {r0}                                 ; get SVC PC
    ; Change to Spervisor mode
    msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
    stmfd   sp!, {r0}                ; Store svc-mode pc

    ; Change to Abort mode
    msr     CPSR_c, #PSR_Abort:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
    ldmdb   sp!, {r0-r1}             ; Get  SVC sp and r14
    ; Change to Spervisor m
    msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
    stmfd   sp!, {r1}                ; Store  SVC r14

    ; Change to Abort mode
    msr     CPSR_c, #PSR_Abort:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
    ldmdb   sp!, {r0-r12}            ; Get  SVC r0--r12
    ; Change to Supervisor mode
    msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
    stmfd   sp!, {r0-r12}            ; Store  SVC r0--r12

    ; Change to Abort mode
    msr     CPSR_c, #PSR_Abort:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
    ldmdb   sp!, {r0}                ; Get  SVC SPSR
    ; Change to Spervisor mode
    msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
    stmfd   sp!, {r0}                ; Store  SVC SPSR

    ldr     r0, =rex_curr_task      ; Save sp of outgoing task into TCB.
    ldr     r0, [r0]
    str     sp, [r0, #REX_TCB_STACK_POINTER_OFFSET]
    ; Change to Abort mode
    msr     CPSR_c, #PSR_Abort:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
;    ldmdb   sp, {r0,r4-r12}          ; Get r4 to r12
    ldmdb   sp, {r4-r12}          ; Get r4 to r12
;    msr     CPSR_cxsf, r0

  LEAF_NODE_END

;=============================================================================
;
;  ENTRY rex_sched_without_save
;
;  DESCRIPTION
;    Schedules the task being passed in r0 without saving the
;    context of the current  task.
;    NOTE 1: This function is meant to be used in a very specific way.
;    NOTE 2: This function schedules rex_best_task regardless of the context
;            from which it is called and whether tasks are locked or not.
;
;  REGISTERS USED:
;    SPSR, sp, r0, r1, a3
;
;=============================================================================
  LEAF_NODE rex_sched_without_save

    ldr     r1, =rex_curr_task         ; Get curr task
    str     r0, [r1]                   ; Set current task to the task to
                                       ; be scheduled

    ; Change to Spervisor mode
    msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask

    ldr     sp, [r0, #REX_TCB_STACK_POINTER_OFFSET]    ; Load the stack
                                                       ; for the task to
                                                       ; be scheduled

    ldmfd   sp!, {r0}                  ; Restore SPSR
    msr     SPSR_f, r0
    msr     SPSR_c, r0

    mov     r0, sp                     ; Load sp in a1.

    add     sp, sp, #REX_CF_SIZE - 4   ; adjust sp
    ldmfd   r0, {r0-r12,lr,pc}^        ; Load and return, sp already adjusted.

  LEAF_NODE_END


;=======================================================================
;
; FUNCTION rex_sched_force
;
; DESCRIPTION
;   This function does the actual task swapping. It should only be used
; for resuming a task from page fault when there is a task lock is in
; effect. This is the same as rex_sched except that a task swap is
; forced when a task lock is in effect. The scheduled task is not the
; rex best task but the aborted task. This is passed as an argument
;
;
; FORMAL PARAMETERS
;   Pointer to the task to be scheduled
;
; DEPENDENCIES
;   Rex_sched assumes that the following global variables have reasonabl
;   values: rex_curr_task, rex_best_task.
;
; RETURN VALUE
;   None.
;
; SIDE EFFECTS
;   Rex_sched_force may set the current task pointer (rex_curr_task) equal
;   to the pointer to the  task being specified if they are not already the
;   same. Also, rex_sched_force may cause a task swap to occur from
;   the old current task to the new current task.
;
;=======================================================================
   CODE32
   EXPORT rex_sched_force
rex_sched_force

#ifdef FEATURE_STACK_CHECK
    cmp     r13, sl
    bcs     __rex_sched_force_no_stkovrflow
    bl      __16__rt_stkovf_split_small
__rex_sched_force_no_stkovrflow
#endif
    ldr   a2, =rex_task_to_schedule  ; save the task to be scheduled
    str   a1, [a2]     ;
    mrs   a3, CPSR                   ; Save the CPSR for later.
                                     ; This will be used in rex_sched

    ldr   a1, = rex_cpsr_value
    str   a3, [a1]
    orr   a1, a3, #PSR_Irq_Mask:OR:PSR_Fiq_Mask
    msr   CPSR_c, a1

   ldr    a1, =rex_sched_common
   bx     a1
; END rex_sched_force


#endif /* FEATURE_PROGRESSIVE_BOOT */
; Termination Token for ARM Assembly files
  END

