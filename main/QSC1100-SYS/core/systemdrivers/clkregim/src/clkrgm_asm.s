;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                     Clock Regime Assembly Functions
;
; GENERAL DESCRIPTION
;
; Contains macros and defines for the clock regime and sleep code.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
; Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

#include "msm.h"

;===================================================================
; Externalized functions
;===================================================================
  EXPORT clk_regime_switch_asm

;===================================================================
  AREA  CLK_REGIME, CODE, READONLY, ALIGN=5
  CODE32
  PRESERVE8
;===================================================================
; MACRO precache_entry
; ARGS
;   
; DESCRIPTION
; 
;===================================================================
     MACRO
$lbl precache_entry
$lbl ldr    r0,=$lbl.1                        ; PRECACHE INSTRUCTIONS BEGIN
    ldr    r1,=$lbl.2                        ; PRECACHE INSTRUCTIONS END
$lbl.0 mcr    p15,0,r0,c7,c13,1            ; LOAD ICACHE
    add    r0,r0,#32                       ; INCREMENT OFFSET
    cmp    r0,r1                           ; COMPARE TO END OFFSET
    blo    $lbl.0                            ; LOOP TILL DONE
$lbl.1 ; NOP                             ; BEGIN PRECACHE ADDRESS
    MEND
;===================================================================
; MACRO precache_exit
; ARGS
;
; DESCRIPTION
;
;===================================================================
    MACRO
$lbl precache_exit
$lbl.2 ; NOP                                   ; END PRECACHE ADDRESS
    MEND


;=======================================================================
;  clk_regime_switch_asm
;
; DESCRIPTION
;   Function to switch to a new arm_crc global clock source.  The new source
;   should already be setup in ARM_CFG and ARM_MOD_REG on the non-active
;   arm_crc source.  This function will switch and set and set the dividers 
;   (or set dividers, and switch) without accessing ROM or RAM.  This should
;   be called only from within intlock.
;
; PARAMETERS
;   r0 : Logical OR together the rate_sel values in ARM_CFG
;        (EBI2_RATE_SEL | EBI1_RATE_SEL | AHB_RATE_SEL | ARM_RATE_SEL )
;   r1 : up/down flag to indicate the order of the writes.  If 1 (speeding up)
;        write rate_sels first, then src_sel.  If 0, first change source, then
;        set rate_sels.
;
; SIDE EFFECTS
;   The arm and bus speeds change.
;
;=======================================================================
clk_regime_switch_asm FUNCTION
    stmfd sp!, {r4-r5}                  ; PUSH REGISTERS
    FRAME PUSH {r4-r5}
    mov r4, r0; 
    mov r5, r1; 

clk_regime_switch_asm_cached precache_entry

    ; Prepare the mask of the rate_sel bits
    ldr    r2,=HWIO_FMSK(ARM_CFG_REG,EBI2_RATE_SEL):OR: \
               HWIO_FMSK(ARM_CFG_REG,EBI1_RATE_SEL):OR: \
               HWIO_FMSK(ARM_CFG_REG,AHB_RATE_SEL):OR:  \
               HWIO_FMSK(ARM_CFG_REG,ARM_RATE_SEL)

    ldr    r0,=HWIO_ADDR(ARM_CFG_REG)
    ldr    r3,[r0]                  ; R3 is ARM_CFG_REG value

    tst    r5,#1
    beq    %FT1 ; switch down
    ; Switch up
    ; In a switch up to a faster rate, set the dividers first, then change
    ; the source.
    
    bic    r3,r3,r2   ; clear the rate_sel bits
    orr    r3,r3,r4   ; prepare the rate_sel values
    str    r3,[r0]    ; Execute the rate_sel divider change.

    eor    r3,r3,#HWIO_FMSK(ARM_CFG_REG,ARM_SRC_SEL)
    str    r3,[r0]    ; Perform the source switch

    b %FT2 ; return
1   ; Label for switch_down
    ; In a switch down to a slower rate, change to the slower source, 
    ; then set the rate_sel dividers.
    
    eor    r3,r3,#HWIO_FMSK(ARM_CFG_REG,ARM_SRC_SEL)
    str    r3,[r0]    ; Perform the source switch

    bic    r3,r3,r2   ; clear the rate_sel bits
    orr    r3,r3,r4   ; prepare the rate_sel values
    str    r3,[r0]    ; Execute the rate_sel divider change.

2   ; Label for return
    ldmfd sp!, {r4-r5}                  ; POP REGISTERS + RETURN
    bx lr
    ENDFUNC
    LTORG
clk_regime_switch_asm_cached precache_exit

  END
