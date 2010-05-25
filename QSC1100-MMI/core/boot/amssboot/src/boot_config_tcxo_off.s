;*====*====*====*====*====*====*====*====*====*====*====*====*====*====
;
;                     TCXO SHUTDOWN  AND CLOCK SWITCHING
;
; GENERAL DESCRIPTION
; Contains assembly code to shutdwon TCXO for NOR/PSRAM configuration
; and clock switching for both NOR/PSRAM and NAND/SDRAM configurations.
;
; EXTERNALIZED FUNCTIONS
;   config_tcxo_off
;     prepares various registers to power saving mode and turns off TCXO
;
;   turn_off_micro_clk
;     Turns off the ARM clock to save power
;
;   switch_micro_clk_delayed
;     Switches the micro clock from TCXO to 146 Mhz smoothly
;
; IMPORTANT NOTE
;
;   The assembly code in this file is hand coded to get optimal clock
;   switching performance. The code has been written to load the clock
;   switching code into cache and the registers have been hand picked
;   to make sure that no external access occurs during clock switching.
;   It is very important that this code is left unmodified to ensure
;   proper operation. There is no user modifiable code in this file.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
;
; Copyright (c) 1998,1999 by QUALCOMM, Incorporated. All Rights Reserved
; Copyright (c) 2000,2001, 2002, 2003, 2004 by QUALCOMM, Incorporated.
;               All Rights Reserved
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====


;*====*====*====*====*====*====*====*====*====*====*====*====*====*====
;
;                           EDIT HISTORY FOR FILE
;  This section contains comments describing changes made to the module.
;  Notice that changes are listed in reverse chronological order.
;
;  8/11/04 bgc Changed ref clock and removed wait loop for 32kHz sleep.
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====


;======================================================================
;
;                          MODULE INCLUDES
;
;======================================================================

#include "customer.h"
#include "msm.h"
#include "mpmc.h"
#include "boot_sleep_macros.s"

;=======================================================================
;
;                          MODULE EXPORTS
;
;=======================================================================

; Export the symbols __main and _main to prevent the linker from
; including the standard runtime library and startup routine.

       EXPORT  config_tcxo_off
       EXPORT  turn_off_micro_clk
       EXPORT  switch_micro_clk_high_delayed
       EXPORT  switch_micro_clk_low_delayed

;=======================================================================
;
;                          MODULE DEFINES
;
;=======================================================================

; Number of HCLK cycles to pause after switching the MCLK.  This
; value corresponds to a duration of 5 microseconds when the HCLK
; is 73 MHz.

CLOCK_SWITCH_PAUSE    EQU   360

;This time is in TCXO clock cycle units
IO_UNFREEZE_TIME_CYCLES     EQU   3000

;=======================================================================
;
;                          MODULE PROCEDURES
;
;=======================================================================

    AREA    BOOT_PREPARE_TCXO_OFF, CODE, READONLY , ALIGN=5
    CODE32

;=======================================================================
; CONFIG_TCXO_OFF
;
; DESCRIPTION
;   Function to turn off TCXO and switch the micro clock off
;
; SIDE FFECTS
; None
;=======================================================================

config_tcxo_off
; ======================================================================
; Save the registers
; ======================================================================
    stmfd sp!, {r4-r12,lr}
    mov   r4,  r0
    mov   r5,  r1
    mov   r6,  r2
;;; (r4,r5,r6 reserved for TCXO shutdown values)

; ======================================================================
; Pre-fetch the code for tcxo sutdown to cache
; ======================================================================
    ldr    r0, = start_address
    ldr    r1, = end_address
prefetch_loop
    mcr    p15,0,r0,c7,c13,1
    add    r0, r0, #32
    cmp    r0, r1
    blo    prefetch_loop
start_address

#ifndef FEATURE_SATURN
; ======================================================================
; Turn off the analog die
; ======================================================================
    ldr    r0,  =HWIO_ADDR(SLEEP_N_ADIE)
    ldr    r1,  =0
    str    r1, [r0]
#endif

; ======================================================================
;Switch MCLK to sleep XTAL, but do NOT update the clock configuration
;structures.  This way, we can easily switch back to the "non-sleep"
;MCLK later.
; ======================================================================

    ldr r2, =0x1
    ldr r1, =0x0
;;; (r1,r2 reserved for #0, #1 usage)

    ldr r9, =HWIO_ADDR(MICRO_CLK_OFF)
;;; (r9 reserved for MICRO_CLK_OFF)

#ifndef FEATURE_USE_MPM_TCXO_CONTROL
    
    ldr r7, =HWIO_ADDR(SWITCH_REF_CLK_SEL)
    str r1, [r7]
    ldr r3, =HWIO_ADDR(MICRO_CLK_MD)
    str r1, [r3]
    ldr r10, =HWIO_ADDR(MICRO_CLK_NS)
    str r2, [r10]

    ldr r14, =HWIO_ADDR(SWITCH_CLK)
;;; (r7,r3,r10,r14 clock switch)

    str r2, [r14] ; switch clock to 32kHz pending in 1-2 tcxo clks
    nop
    nop
    
; ======================================================================
;Power down TCXO
;NOTE: Do not alter the GPIO_REG_0 (ie. BIO_OUTs) until we
;turn the TCXO GPIO back on.  If you do alter the
;register, the change will get overwritten when we turn
;the TCXO GPIO back on.
; ======================================================================
    str   r6, [r4]
; ======================================================================
;Power down ARM
; ======================================================================
    ldr r2, =0x1
    ldr r1, =0x0
    
#endif

    str   r2, [r9]
    str   r1, [r9]
;;; (r9 is free to be reused)
; ======================================================================
;MCLK is no longer oscillating, so the uP will halt.  MCLK will start
;oscillating again as soon as the next non-masked interrupt fires.
;---------------------------------------------------------------------
;---------------------------------------------------------------------
;An interrupt has fired!  Restore MCLK to its pre-sleep rate, and
;return to the caller (which should free interrupts, allowing the
;interrupt to be vectored).
;


#ifndef FEATURE_USE_MPM_TCXO_CONTROL

;Power-up external TCXO clock generator
; ======================================================================
    str   r5, [r4]
;;; (r4,r5,r6 are free to be reused now)

; ======================================================================
;   Wait for TCXO to warm up
; ======================================================================
#ifndef T_MSM6800B
    ldr   r0, =HWIO_ADDR(PAUSE_TIMER)
    ldr   r1, =TCXO_WARMUP_TIME_CYCLES
    str   r1, [r0] ; (PAUSE_TIMER)
    ldr   r1, =0x0 ; (restore r1 = 0)
#endif
;;; (r0,r11 is free to be reused)
; ======================================================================
; Quickly switch MCLK to TCXO, allowing us to set
; the PLL up much faster than on the sleep XTAL.
; ======================================================================
    str r1, [r7]  ; (SWITCH_REF_CLK_SEL)
    str r1, [r3]  ; (MICRO_CLK_MD)
    str r1, [r10] ; (MICRO_CLK_NS)
    str r2, [r14] ; (SWITCH_CLK)
;;; (r3,r7,r10,r14 are free to be used)
;;; (r1,r2 no longer caching 0, 1 and free for use)

#else
#error code not present
#endif

#ifndef FEATURE_SATURN
; ======================================================================
; Turn on the analog die
; ======================================================================

    ldr    r0,   =HWIO_ADDR(SLEEP_N_ADIE)
    ldr    r1,   =1
    str    r1,   [r0]
#endif
end_address
; ======================================================================
; restore the registers and return
; ======================================================================
   ldmfd sp!, {r4-r12,lr}
   bx    lr

;=======================================================================
;  turn_off_micro_clk
;
; DESCRIPTION
;   Function to turn off the ARM clock
; PARAMETERS
; None
; SIDE FFECTS
; None
;=======================================================================
turn_off_micro_clk
; ======================================================================
; Save the registers
; ======================================================================
    stmfd sp!, {r0-r4,lr}
; ======================================================================
; Pre-fetch the code for arm clock switch to cache
; ======================================================================
    ldr   r0, = start_turn_off_micro_clk_address
    ldr   r1, = end_turn_off_micro_clk_address
prefetch_turn_off_micro_clk_loop
    mcr   p15,0,r0,c7,c13,1
    add   r0, r0, #32
    cmp   r0, r1
    blo   prefetch_turn_off_micro_clk_loop
start_turn_off_micro_clk_address
    mov   r4, #0xc
; ======================================================================
;Load the address of the power off register
; ======================================================================
    ldr   r0, =HWIO_ADDR(MICRO_CLK_OFF)
; ======================================================================
;Write a 0x1 to MICRO_CLK_OFF_WB.  A further write of 0 will shut
;off MCLK.
; ======================================================================
    mov   r1, #1
    str   r1, [r0]
; ======================================================================
;Prepare values for the impending powerdown */
; ======================================================================
    mov   r1, #0
    mov   r2, #0
    mov   r3, #0
; ======================================================================
; Current register state:
;         r0 = MICRO_CLK_OFF_WB
;         r1 = 0x0
;         r2 = current MCLK divider value
;         r3 = current HCLK divider value
; We perform a store multiple in order to ensure that all chip selects
;are high before powering the uP off.  If we perform a normal store,
;the ROM0 chip select will still be asserted LOW in order to pre-fetch
;the next instruction.  Since the store multiple takes more than
;one instruction to execute, it stalls the pipeline, hence allowing
;the chip selects to go high.
; ======================================================================

    stmia r0!, {r1, r2, r3} ; Sleep will occur during this write:
                            ;  - Write a zero to MICRO_CLK_OFF_WB
                            ;  - Write r1 to empty address in MSM map
                            ; (address of HWIO_ADDR(MICRO_CLK_OFF) + 4)

; ======================================================================
turn_off_micro_clk_nop_loop
    NOP
    NOP
    sub   r4, r4, #1
    cmp   r4, #0
    bgt   turn_off_micro_clk_nop_loop
   ldmfd sp!, {r0-r4,pc}

end_turn_off_micro_clk_address

;=======================================================================
; SWITCH_MICRO_CLK_HIGH_DELAYED
;
; DESCRIPTION
;   Function to switch the micro clock to a HIGHER frequency.
;
; PARAMETERS
;         r0 = target HCLK frequency HCLK_DIV register value
;         r1 = target HCLK frequency PAUSE_TIMER register value
;
; SIDE FFECTS
;   The MCLK and/or HCLK frequencies are changed.
;=======================================================================
switch_micro_clk_high_delayed

; ======================================================================
; Save the non-volatile registers that are used in this function.
; ======================================================================

    stmfd sp!, {r4-r7,lr}

; ======================================================================
; Pre-load the register addresses required for clock switching.
; ======================================================================

    mov   r2,  #1
    ldr   r3,  = HWIO_ADDR(SWITCH_CLK)
    ldr   r4,  = HWIO_ADDR(HCLK_DIV)
    ldr   r5,  = HWIO_ADDR(PAUSE_TIMER)

; ======================================================================
; Pre-cache the instructions required for clock switching.
; ======================================================================

    ldr   r6,  = start_switch_high_address
    ldr   r7,  = end_switch_high_address

prefetch_switch_high_loop

    mcr   p15,0,r6,c7,c13,1
    add   r6, r6, #32
    cmp   r6, r7
    blo   prefetch_switch_high_loop

; ======================================================================
; Start of cached instruction region.
; ======================================================================

start_switch_high_address

; ======================================================================
; Switch the HCLK divider first.
; ======================================================================

    str   r0, [r4]

; ======================================================================
; Switch the MCLK frequency second.
; ======================================================================

    str   r2, [r3]

; ======================================================================
; Pause to allow any transients to settle.
; ======================================================================

    str   r1, [r5]

; ======================================================================
; Restore the saved registers and return.
; ======================================================================

   ldmfd sp!, {r4-r7,pc}

; ======================================================================
; End of cached instruction region.
; ======================================================================

end_switch_high_address


;=======================================================================
; SWITCH_MICRO_CLK_LOW_DELAYED
;
; DESCRIPTION
;   Function to switch the micro clock to a LOWER frequency.
;
; PARAMETERS
;         r0 = target HCLK frequency HCLK_DIV register value
;         r1 = target HCLK frequency PAUSE_TIMER register value
;
; SIDE FFECTS
;   The MCLK and/or HCLK frequencies are changed.
;=======================================================================
switch_micro_clk_low_delayed

; ======================================================================
; Save the non-volatile registers that are used in this function.
; ======================================================================

    stmfd sp!, {r4-r7,lr}

; ======================================================================
; Pre-load the register addresses required for clock switching.
; ======================================================================

    mov   r2,  #1
    ldr   r3,  = HWIO_ADDR(SWITCH_CLK)
    ldr   r4,  = HWIO_ADDR(HCLK_DIV)
    ldr   r5,  = HWIO_ADDR(PAUSE_TIMER)

; ======================================================================
; Pre-cache the instructions required for clock switching.
; ======================================================================

    ldr   r6,  = start_switch_low_address
    ldr   r7,  = end_switch_low_address

prefetch_switch_low_loop

    mcr   p15,0,r6,c7,c13,1
    add   r6, r6, #32
    cmp   r6, r7
    blo   prefetch_switch_low_loop

; ======================================================================
; Start of cached instruction region.
; ======================================================================

start_switch_low_address

; ======================================================================
; Switch the MCLK frequency first.
; ======================================================================

    str   r2, [r3]

; ======================================================================
; Switch the HCLK divider second.
; ======================================================================

    str   r0, [r4]

; ======================================================================
; Pause to allow any transients to settle.
; ======================================================================

    str   r1, [r5]

; ======================================================================
; Restore the saved registers and return.
; ======================================================================

   ldmfd sp!, {r4-r7,pc}

; ======================================================================
; End of cached instruction region.
; ======================================================================

end_switch_low_address

    END
