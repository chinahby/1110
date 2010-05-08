;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           R E X  A R M   C O R E 
;
; GENERAL DESCRIPTION
;   Capture a "core" dump
;
; EXTERNALIZED FUNCTIONS
;   rex_jettison_core
;
; Copyright (c) 2002-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                      EDIT HISTORY FOR MODULE
;
; $Header: //source/qcom/qct/core/kernel/rex/main/latest/rexcore.s#1 $
;
; when      who    what, where, why
; --------  -----  -----------------------------------------------------------
; 03/31/04  tbg    Fixed more bugs in rex_jettison_core() and added functions
;                  to retrieve the SP and the PC.
; 03/17/04   gr    Fixed a few bugs in rex_jettison_core().
; 03/15/04   gr    Deleted FEATURE_REX_CORE_DUMP. rex_jettison_core() is
;                  used for other debug functionality.
; 11/14/02  jct    Created.
;============================================================================

#include "customer.h"
#include "armasm.h"

;
; Any change to the log header or to the COREDUMP packet structure will
; necessitate a change to REX_CORE.
;
  AREA       REX_CORE, DATA

rex_core_t RN     r7 
           MAP    0,rex_core_t
log_hdr    FIELD 12
svc_r0     FIELD  4
svc_r1     FIELD  4
svc_r2     FIELD  4
svc_r3     FIELD  4
svc_r4     FIELD  4
svc_r5     FIELD  4
svc_r6     FIELD  4
svc_r7     FIELD  4
svc_r8     FIELD  4
svc_r9     FIELD  4
svc_r10    FIELD  4
svc_r11    FIELD  4
svc_r12    FIELD  4
svc_sp     FIELD  4
svc_lr     FIELD  4
svc_spsr   FIELD  4
svc_pc     FIELD  4
sys_sp     FIELD  4
sys_lr     FIELD  4
irq_sp     FIELD  4
irq_lr     FIELD  4
irq_spsr   FIELD  4
abt_sp     FIELD  4
abt_lr     FIELD  4
abt_spsr   FIELD  4
udf_sp     FIELD  4
udf_lr     FIELD  4
udf_spsr   FIELD  4
fiq_r8     FIELD  4
fiq_r9     FIELD  4
fiq_r10    FIELD  4
fiq_r11    FIELD  4
fiq_r12    FIELD  4
fiq_sp     FIELD  4
fiq_lr     FIELD  4
fiq_spsr   FIELD  4
jva_sp     FIELD  4
jva_lr     FIELD  4
jva_spsr   FIELD  4
cpsr       FIELD  4

  IMPORT rex_core

  AREA RexRom, CODE, READONLY
  CODE32

;=============================================================================
; FUNCTION REX_JETTISON_CORE
;
; DESCRIPTION
;    Captures the ARM register state
;    - Store CPSR
;    - For each mode:
;        save reqd regs to rex_core
;    - Return to entry mode
;=============================================================================
  LEAF_NODE rex_jettison_core
    ; Store CPSR
    ; 
    stmfd   sp!, {r0,r2,r7}
rex_jettison_core_a
    mrs     r2, CPSR 
 
    ; Capture Supervisor Mode r0-r14, SPSR
    msr     CPSR_c, #PSR_Supervisor:OR:PSR_Irq_Mask:OR:PSR_Fiq_Mask
    ldr     rex_core_t,=rex_core
    str     r0, svc_r0
    str     r1, svc_r1
    ; Store r2 later after restoring it from the stack
    str     r3, svc_r3
    str     r4, svc_r4
    str     r5, svc_r5
    str     r6, svc_r6
    ; Store r7 later after restoring it from the stack
    str     r8, svc_r8
    str     r9, svc_r9
    str     r10, svc_r10
    str     r11, svc_r11    
    str     r12, svc_r12
    str     sp, svc_sp
    str     lr, svc_lr
    mrs     r0, SPSR
    str     r0, svc_spsr
    ldr     r0, =rex_jettison_core_a
    str     r0, svc_pc

    ; Capture System Mode r13, r14 (no SPSR)
    msr     CPSR_c, #PSR_System:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
    str     sp, sys_sp
    str     lr, sys_lr

    ; Capture IRQ Mode r13, r14, SPSR
    msr     CPSR_c, #PSR_Irq:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
    str     sp, irq_sp
    str     lr, irq_lr
    mrs     r0, SPSR
    str     r0, irq_spsr

    ; Capture FIQ Mode r8, r9, r10, r11, r12, r13, r14, SPSR
    msr     CPSR_c, #PSR_Fiq:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
    str     r8, fiq_r8
    str     r9, fiq_r9
    str     r10, fiq_r10
    str     r11, fiq_r11
    str     r12, fiq_r12
    str     sp, fiq_sp
    str     lr, fiq_lr
    mrs     r0, SPSR
    str     r0, fiq_spsr
    
    ; Capture Abort Mode r13, r14, SPSR
    msr     CPSR_c, #PSR_Abort:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
    str     sp, abt_sp
    str     lr, abt_lr
    mrs     r0, SPSR
    str     r0, abt_spsr

    ; Capture Undefined Mode r13, r14, SPRS
    msr     CPSR_c, #PSR_Undef:OR:PSR_Fiq_Mask:OR:PSR_Irq_Mask
    str     sp, udf_sp
    str     lr, udf_lr
    mrs     r0, SPSR
    str     r0, udf_spsr

    ; Store CPSR value (at time of function call)
    str     r2, cpsr

    ; Return to the mode in which we entered
    msr     CPSR_c, r2

    ldmfd   sp!, {r0,r2}
    str     r2, svc_r2
    ldmfd   sp!, {r2}
    str     r2, svc_r7
    sub     sp, sp, #8
    ldmfd   sp!, {r2,r7}

 LEAF_NODE_END

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;  ENTRY rex_get_sp
;
;  DESCRIPTION
;    Gets sp back to user
;
;  DEPENDENCIES
;     None
;
;  REGISTERS USED:
;     a1, sp
;
;  SIDE EFFECTS
;     None
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 LEAF_NODE rex_get_sp
        mov     a1, sp
 LEAF_NODE_END

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;  ENTRY rex_get_pc
;
;  DESCRIPTION
;     Gets pc back to user
;     Note that we return lr since pc actually points two instructions beyond
;     the current location
;
;  DEPENDENCIES
;     None
;
;  REGISTERS USED:
;     a1, lr
;
;  SIDE EFFECTS
;     None
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 LEAF_NODE rex_get_pc
        mov     a1, lr
 LEAF_NODE_END

; Termination Token for ARM Assembly files
  END 
