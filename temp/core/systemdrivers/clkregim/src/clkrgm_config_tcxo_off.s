;*====*====*====*====*====*====*====*====*====*====*====*====*====*====
;
;            SWITCH ARM TO 32 KHZ, DISABLE TCXO, SLEEP, WAKEUP
;
; GENERAL DESCRIPTION
; Contains assembly code to disable TCXO for NOR/PSRAM configuration
; and clock switching for both NOR/PSRAM.
;
; EXTERNALIZED FUNCTIONS
; set_micro_to_32khz_and_kill_tcxo
; Set the ARM clock SleepClk (32Khz) power saving mode and turns off
; TCXO. Assumption is ARM is opearting at TCXO, when this function
; called.
;
; IMPORTANT NOTE
;
; The assembly code in this file is hand coded to get optimal clock
; switching performance. The code has been written to load the clock
; switching code into cache and the registers have been hand picked
; to make sure that no external access occurs during clock switching.
; It is very important that this code is left unmodified to ensure
; proper operation. There is no user modifiable code in this file.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
; Copyright (c) 1998,1999 by QUALCOMM, Incorporated. All Rights Reserved
; Copyright (c) 2000,2001, 2002, 2003 by QUALCOMM, Incorporated.
;               All Rights Reserved
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====


;*====*====*====*====*====*====*====*====*====*====*====*====*====*====
;
;                           EDIT HISTORY FOR FILE
;  This section contains comments describing changes made to the module.
;  Notice that changes are listed in reverse chronological order.
;
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====


;======================================================================
;
;                           MODULE INCLUDE FILES
;
;======================================================================
#include "customer.h"
#include "msm.h"
;=======================================================================
;
;                             MODULE IMPORTS
;
;=======================================================================
;=======================================================================
;
;                             MODULE EXPORTS
;
;=======================================================================
;
;
  EXPORT  set_micro_to_32khz_and_kill_tcxo
;
;===================================================================
  AREA    BOOT_PREPARE_TCXO_OFF, CODE, READONLY , ALIGN=5
  CODE32
;===================================================================

ARM_AHB_EBI1_EBI2_RATE_SEL_MASK         EQU     0x00000FE0
ARM_SRC_SEL_MASK                        EQU     0x00000010
SET_ARM_DIV_MOD0_TO_DIV1                EQU     0xFFFFFF0F
SET_ARM_DIV_MOD1_TO_DIV1                EQU     0xFFFFFFF0

ARM_SRC_SEL0_MASK                       EQU     0xFFFFFFFC
SET_ARM_SRC_SEL0_TO_SLEEPCLK            EQU     0x00000002

ARM_SRC_SEL1_MASK                       EQU     0xFFFFFFF3
SET_ARM_SRC_SEL1_TO_SLEEPCLK            EQU     0x00000008
SET_ARM_SRC_SEL_VALUE                   EQU     0xFFFFFFEF
SET_ARM_SRC_SEL_TO_SRC0                 EQU     0x00000000
SET_ARM_SRC_SEL_TO_SRC1                 EQU     0x00000010
  
SET_ARM_SRC_SEL0_TO_XO                  EQU     0x00000000
SET_ARM_SRC_SEL1_TO_XO                  EQU     0x00000000

ARM_SLEEP_ENABLE                        EQU     0x00004000
ARM_SLEEP_ENABLE_MASK                   EQU     0xFFFFBFFF

;PMIC_CTL_TCXO                           EQU     0x00000004

;;; Need to access the the PMIC_CONTROL register
PMIC_CONTROL_REG_ADDR                   RN      R0
PMIC_CONTROL_DISABLE_VAL                RN      R1
PMIC_CONTROL_ENABLE_VAL                 RN      R2

ARM_CLK_MOD_REG_ADDR                    RN      R3

ARM_CLK_CFG_REG_ADDR                    RN      R4
ARM_CLK_CFG_REG_VAL                     RN      R5
SAVED_ARM_CLK_CFG_REG_VAL               RN      R6

SAVED_ARM_CLK_SRC_VAL                   RN      R7
SAVED_ARM_MUX_SRC_VAL                   RN      R8
ARM_CLK_CFG_EN_SLEEP                    RN      R9



;===================================================================
; MACRO precache_entry
; ARGS
;   
; DESCRIPTION
;   Fetches codes to cache
;===================================================================
      MACRO
$lbl precache_entry
$lbl ldr    r0,=$lbl.1                     ; PRECACHE INSTRUCTIONS BEGIN
     ldr    r1,=$lbl.2                      ; PRECACHE INSTRUCTIONS END
$lbl.0 mcr    p15,0,r0,c7,c13,1            ; LOAD ICACHE
      add    r0,r0,#32                       ; INCREMENT OFFSET
      cmp    r0,r1                           ; COMPARE TO END OFFSET
      blo    $lbl.0                          ; LOOP TILL DONE
$lbl.1 ; NOP                               ; BEGIN PRECACHE ADDRESS
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
; set_micro_to_32khz_and_kill_tcxo
;
; DESCRIPTION
; Function to switch ARM to 32 kHz, disable XO buffer, put ARM to sleep,
; wake up, enable XO buffer, switch to ARM to 19.2 mHz 
;
; SIDE FFECTS
; The arm and bus speeds change.
;=======================================================================
set_micro_to_32khz_and_kill_tcxo precache_entry
    stmfd sp!, {r4-r12,lr}                      ; PUSH REGISTERS

    ldr    PMIC_CONTROL_REG_ADDR,=HWIO_ADDR(PMIC_CONTROL)      	; REGISTER COPY OF ADDRESS PMIC_CONTROL
    ldr    ARM_CLK_CFG_REG_ADDR,=HWIO_ADDR(ARM_CFG_REG)       	; REGISTER COPY OF ADDRESS ARM_CFG_REG
 ;   ldr    ARM_CLK_MOD_REG_ADDR,=HWIO_ADDR(ARM_MOD_REG)       	; REGISTER COPY OF ADDRESS ARM_MOD_REG
 
    ; Switching ARM from XO to Sleep Clock(32Khz)
    ; As Switching to Sleep Clock is switching to low speed clock from XO
    ; Set ARM_DIV value for the 'x' source path (where x=0or1)
    ; Set ARM_SRC_SELx to Sleep Clock
    ; Change the ARM source path
    ; Then adjust the ARM/AHB/EBI1/EBI2 rate settings (we can skip it)
 
    ; ARM clock domain has two separate mux for selecting its clock sources.
    ; It has two muxes for avoiding clitch without halting its current clock.
    ; So when switch to new clock source, it requires to use the unused mux. 
 
    ldr    ARM_CLK_CFG_REG_VAL,[ARM_CLK_CFG_REG_ADDR]      	; Read ARM_CFG register value
    mov    SAVED_ARM_CLK_CFG_REG_VAL, ARM_CLK_CFG_REG_VAL 	; Temporarily store it, avoid load it again

    ; Bit 4 of ARM_CFG_REG indicates arm_src0 or arm_src1 is currently being used
    tst    ARM_CLK_CFG_REG_VAL,#ARM_SRC_SEL_MASK             	; Is arm_src 1 or arm_src0
    bne    switch_to_src0_slpclk

    ; As the present source is '0' switch to src 1 after adjusting the path for src1
    ; Also pre-compute the configuration for using during ARM is runnings at 32 kHz
    ; The current configuration should be valid to use during wake up.
    ; However, the setup on mux 1 is not always valid, so we need to set it up.
    
    ; Set the ARM_DIV_MOD1(0:3) bits to DIV1
;    ldr    r1,[r6]                          	; Read ARM_MOD_REG register value into r1
;    and    r1,r1,#SET_ARM_DIV_MOD1_TO_DIV1      ; Clear the MOD1 bits(0:3) to DIV1
;    str    r1,[r6]                              ; Write the value into ARM_MOD_REG register

    ; Set the ARM_SRC_SEL1(2:3) bits, 00 = xo, 01 = pll, 10 = sleep, 11 = ext
    and    ARM_CLK_CFG_REG_VAL,ARM_CLK_CFG_REG_VAL,#ARM_SRC_SEL1_MASK   ; Mask the SRC_SEL1 bits(2:3) to zero
    orr    ARM_CLK_CFG_REG_VAL,ARM_CLK_CFG_REG_VAL,#SET_ARM_SRC_SEL1_TO_SLEEPCLK  ; Set the SRC_SEL1 bits(2:3) to Sleep Clk(0x2)

    ; Configure clock source of mux 1, also it good to save for reuse bc both muxes have valid clock source
    mov    SAVED_ARM_CLK_SRC_VAL, ARM_CLK_CFG_REG_VAL
    str    ARM_CLK_CFG_REG_VAL,[ARM_CLK_CFG_REG_ADDR]  ; Write the value into ARM_CFG_REG register
    
    ; Prepare to switch to MUX 1 by setting the ARM_SRC bit(4) to SRC1 path
    mov    SAVED_ARM_MUX_SRC_VAL, ARM_CLK_CFG_REG_VAL
    orr    ARM_CLK_CFG_REG_VAL, ARM_CLK_CFG_REG_VAL,#SET_ARM_SRC_SEL_TO_SRC1      ; Set the value into ARM_CFG_REG register( (bit 4 to 0x1)
   
    b      commit_slpclk

switch_to_src0_slpclk
    ; As the present source is '1' switch to src 0 after adjusting the path for src0

    ; Set the ARM_DIV_MOD0(4:7) bits to DIV1
;    ldr    r1,[r6]                          	; Read ARM_MOD_REG register value into r1
;    and    r1,r1,#SET_ARM_DIV_MOD0_TO_DIV1      ; Set the MOD0 bits(4:7) to DIV1
;    str    r1,[r6]                              ; Write the value into ARM_MOD_REG register

    ; Set the ARM_SRC_SEL0(0:1) bits, 00 = xo, 01 = pll, 10 = sleep, 11 = ext
    and    ARM_CLK_CFG_REG_VAL,ARM_CLK_CFG_REG_VAL,#ARM_SRC_SEL0_MASK  ; Mask the SRC_SEL0 bits(0:1) to zero
    orr    ARM_CLK_CFG_REG_VAL,ARM_CLK_CFG_REG_VAL,#SET_ARM_SRC_SEL0_TO_SLEEPCLK  ; Set the SRC_SEL0 bits(0:1) to Sleep Clk(0x2)

    ; Configure clock source of mux 0, also it good to save for reuse bc both muxes have valid clock source
    mov    SAVED_ARM_CLK_SRC_VAL, ARM_CLK_CFG_REG_VAL
    str    ARM_CLK_CFG_REG_VAL,[ARM_CLK_CFG_REG_ADDR]                 ; Write the value into ARM_CFG_REG register

    ; Prepare to switch to MUX 1 by setting the ARM_SRC bit(4) to SRC0 path
    mov    SAVED_ARM_MUX_SRC_VAL, ARM_CLK_CFG_REG_VAL
    and    ARM_CLK_CFG_REG_VAL, ARM_CLK_CFG_REG_VAL,#SET_ARM_SRC_SEL_VALUE        ; Clear the ARM_SRC bit(4)
    orr    ARM_CLK_CFG_REG_VAL, ARM_CLK_CFG_REG_VAL,#SET_ARM_SRC_SEL_TO_SRC0      ; Set the value into ARM_CFG_REG register( (bit 4 to 0x0)

commit_slpclk
    ; Avoid computing as much as we can when ARM is running at 32 kHz.
    ; It helps to precompute all neccessary configuration values which
    ; need to use during ARM is running at 32 kHz.
    orr     ARM_CLK_CFG_EN_SLEEP,ARM_CLK_CFG_REG_VAL,#ARM_SLEEP_ENABLE       	; Set bit 14 to put ARM into sleep


    ldr    PMIC_CONTROL_ENABLE_VAL,[PMIC_CONTROL_REG_ADDR]      ; Read PMIC_CONTROL register value into r2
    and    PMIC_CONTROL_DISABLE_VAL,PMIC_CONTROL_ENABLE_VAL,#0x3; As this register has got only 0:2 bits valid,
;;precache label here to reduce cache (size) usage
    str    ARM_CLK_CFG_REG_VAL,[ARM_CLK_CFG_REG_ADDR]   ; Set the ARM_CFG_REG register with the new source path
    nop                                                 ; Wait for the hardware clock switch latency
    nop
    nop
;;; We are now running at 32 kHz!!!!!!!!!!!   
;;; Disable XO
    str    PMIC_CONTROL_DISABLE_VAL,[PMIC_CONTROL_REG_ADDR] 	; Write the value into PMIC_CONTROL register

;;; Consider to use coprocessor instruction, but keep these code line
;;; MCR    p15, 0, R10, c7, c0, 4 (STBYWFI) cleans write-cache at 32kHz...not allowed, PSRAM fail at 32kHz. 
    str    ARM_CLK_CFG_EN_SLEEP,[ARM_CLK_CFG_REG_ADDR]   ; Write the value to ARM_CFG_REG to push ARM into sleep.
    ; << ** HALT TILL INTERRUPT ** >>
;;; Wake the ARM up
    str    ARM_CLK_CFG_REG_VAL,[ARM_CLK_CFG_REG_ADDR]           ; Write the value to ARM_CFG_REG

;;; Enable XO buffer
    str    PMIC_CONTROL_ENABLE_VAL,[PMIC_CONTROL_REG_ADDR]   	; Write the value to PMIC_CONTROL register

;;; Restore the ARM clock to where it configured prior going to sleep    
    str    SAVED_ARM_MUX_SRC_VAL, [ARM_CLK_CFG_REG_ADDR]

;;; It is now running at 19.2 mHz
    ldmfd sp!, {r4-r12,pc}                  	; POP REGISTERS + RETURN
    LTORG
set_micro_to_32khz_and_kill_tcxo precache_exit
    END