;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                     SDRAM SELF REFRESH AND TCXO SHUTDOWN
;
; GENERAL DESCRIPTION
; Puts SDRAM to self resfresh and shutdowns TCXO
;
; EXTERNALIZED FUNCTIONS
;   set_sdram_self_refresh_and_tcxo_off
;     prepares various registers to power saving mode and turns off TCXO
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
;
; Copyright (c) 1998,1999 by QUALCOMM, Incorporated.  All Rights Reserved.
; Copyright (c) 2000,2001 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;  This section contains comments describing changes made to the module.
;  Notice that changes are listed in reverse chronological order.
;
;  09/21/04 rp   Removed unused imports
;  8/11/04  bgc Changed ref clock and removed wait loop for 32kHz sleep.
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                           MODULE INCLUDE FILES
;
;============================================================================
#include "customer.h"
#include "msm.h"
#include "mpmc.h"
#include "boot_sleep_macros.s"

;This time is in TCXO clock cycle units
IO_UNFREEZE_TIME_CYCLES     EQU   3000

;=======================================================================
;
;                             MODULE IMPORTS
;
;=======================================================================
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    ; Export the symbols __main and _main to prevent the linker from
    ; including the standard runtime library and startup routine.

       EXPORT  set_sdram_self_refresh_and_tcxo_off
    AREA    BOOT_PREPARE_SLEEP, CODE, READONLY

    CODE32
;=======================================================================
; SET_SDRAM_SELF_REFRESH_AND_TCXO_OFF
;
; DESCRIPTION
;   Function to set the SDRAM in self refresh mode and switch the
; tcxo and micro clock off
;
; SIDE FFECTS
; None
;=======================================================================

set_sdram_self_refresh_and_tcxo_off
; ======================================================================
; Save the registers
; ======================================================================
    stmfd sp!, {r4-r12,lr}
    mov    r4, r0
    mov    r5, r1
    mov    r6, r2
;;; (r4,r5,r6 reserved for TCXO shutdown values)

; ======================================================================
;Disable the MMU
; ======================================================================
   mrc     p15, 0 , r0, c1, c0, 0
   bic     r0, r0, # 0x1
   mcr     p15, 0 , r0, c1, c0, 0

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
; ======================================================================
; put the SDRAM to self refresh mode
; ======================================================================
    put_sdram_self_refresh
; ======================================================================
; Turn off all unnecessary MSM clocks
; ======================================================================
;            MICRO_CLK_FS_ON   tonyo added
    ldr    r0, =HWIO_ADDR(MSM_CLK_FS_ON)
    ldr    r1,  =0x60         ; set footswitch MICRO_CLK_UP_FS_ON
    ldr    r0, =HWIO_ADDR(MSM_CLK_HALTA)
    ldr    r1, [r0]
#ifdef T_MSM6800B
    orr    r1, r1, #HWIO_FMSK(MSM_CLK_HALTA,SDRAMC_HCLK_HALT)
    orr    r1, r1, #HWIO_FMSK(MSM_CLK_HALTA,EBI1_FB_CLK_HALT)
#else
    orr    r1, r1, #HWIO_FMSK(MSM_CLK_HALTA,MPMC_HCLK_HALT)
    orr    r1, r1, #HWIO_FMSK(MSM_CLK_HALTA,MPMC_FB_CLK_HALT)
#endif
    str    r1, [r0]      ; disable the MPMC clock

#ifndef FEATURE_SATURN
; ======================================================================
; Turn off the analog die
; ======================================================================
    ldr    r0,  =HWIO_ADDR(SLEEP_N_ADIE)
    ldr    r1,  =0
    str    r1, [r0]
#endif

    ldr r9, =HWIO_ADDR(MICRO_CLK_OFF)
;;; (r9 reserved for MICRO_CLK_OFF)

#ifndef FEATURE_USE_MPM_TCXO_CONTROL

; ======================================================================
;Switch MCLK to sleep XTAL, but do NOT update the clock configuration
;structures.  This way, we can easily switch back to the "non-sleep"
;MCLK later.
; ======================================================================
    ldr r2, =0x1
    ldr r1, =0x0
;;; (r1,r2 reserved for #0, #1 usage)

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
    
#endif    

; ======================================================================
;Power down ARM
; ======================================================================
   ldr r2, =0x1
   ldr r1, =0x0
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
;;; (r0 is free to be reused)
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
    nop
    nop

#else
#error code not present
#endif

; ======================================================================
; Turn back MPMC clock regime
; ======================================================================
    ldr    r0, =HWIO_ADDR(MSM_CLK_HALTA)
    ldr    r1, [r0]
#ifdef T_MSM6800B
    bic    r1, r1, #HWIO_FMSK(MSM_CLK_HALTA,SDRAMC_HCLK_HALT)
    bic    r1, r1, #HWIO_FMSK(MSM_CLK_HALTA,EBI1_FB_CLK_HALT)
#else
    bic    r1, r1, #HWIO_FMSK(MSM_CLK_HALTA,MPMC_HCLK_HALT)
    bic    r1, r1, #HWIO_FMSK(MSM_CLK_HALTA,MPMC_FB_CLK_HALT)
#endif
    str    r1, [r0]
;            MICRO_CLK_FS_ON   tonyo added   
    ldr    r0, =HWIO_ADDR(MSM_CLK_FS_ON)
    ldr    r1, =0x0         ; remove footswitch MICRO_CLK_UP_FS_ON    
; ======================================================================
; Take the SDRAM  from self refresh mode
; ======================================================================
    take_sdram_from_self_refresh

#ifndef FEATURE_SATURN
; ======================================================================
; Turn on the analog die
; ======================================================================
    ldr    r0,   =HWIO_ADDR(SLEEP_N_ADIE)
    ldr    r1,   =1
    str    r1,   [r0]
#endif

; ======================================================================
;Enable the MMU
; ======================================================================
   mrc     p15, 0 , r0, c1, c0, 0
   orr     r0, r0, # 0x1
   mcr     p15, 0 , r0, c1, c0, 0
end_address
; ======================================================================
; restore the registers and return
; ======================================================================
   ldmfd   sp!, {r4-r12,lr}
    bx     lr
    END

