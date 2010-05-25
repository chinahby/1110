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
#include "boot_sleep_macros.s"

;=======================================================================
;
;                          MODULE EXPORTS
;
;=======================================================================

; Export the symbols __main and _main to prevent the linker from
; including the standard runtime library and startup routine.

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

;=======================================================================
;
;                          MODULE PROCEDURES
;
;=======================================================================

    AREA    BOOT_PREPARE_TCXO_OFF, CODE, READONLY , ALIGN=5
    CODE32

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
; N/A
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
; N/A
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
; N/A
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
