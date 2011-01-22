;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                     SDRAM SELF REFRESH AND TCXO SHUTDOWN
;
; GENERAL DESCRIPTION
; Puts SDRAM to self resfresh and shutdowns TCXO
;
; EXTERNALIZED FUNCTIONS
;   set_sdram_self_refresh_and_pcollapse
;     prepares various registers to power saving mode, turns off TCXO
;     and shuts down the digital core power.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
;
; Copyright (c) 1998,1999 by QUALCOMM, Incorporated.  All Rights Reserved.
; Copyright (c) 2000-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
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
#include "msmhwioreg.h"

#undef FEATURE_L4_MSMSLEEP_ROSC

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

       EXPORT  set_sdram_self_refresh_and_pcollapse
    AREA    BOOT_PREPARE_SLEEP, CODE, READONLY

    CODE32
;=======================================================================
; SET_SDRAM_SELF_REFRESH_AND_PCOLLAPSE
;
; DESCRIPTION
;   Function to set the SDRAM in self refresh mode and switch the
; tcxo and micro clock off (invokes power collapse)
;
; SIDE FFECTS
; None
;=======================================================================

set_sdram_self_refresh_and_pcollapse
; ======================================================================
; Save the registers
; ======================================================================
;    stmfd sp!, {r4-r12,lr} don't do this because of L4 context save 

; ======================================================================
; Disable the MMU
; ======================================================================
; N/A
; ======================================================================
; Pre-fetch the code for tcxo sutdown to cache
; ======================================================================
; N/A
; ======================================================================
; Put SDRAM to self refresh mode
; ======================================================================
    put_sdram_self_refresh
; ======================================================================
; Turn off all unnecessary MSM clocks
; ======================================================================
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

;;; (r11,r12 reserved for HALTA, HALTB)

    ldr r9, =HWIO_ADDR(MICRO_CLK_OFF)
;;; (r9 reserved for MICRO_CLK_OFF)

; ======================================================================
; Power down ARM
; ======================================================================
    ldr r2, =0x1
    ldr r1, =0x0
    str r2, [r9]
    str r1, [r9]
;;; (r9 is free to be reused)
; ======================================================================
; MCLK is no longer oscillating, so the uP will halt.  MCLK will start
; oscillating again as soon as the next non-masked interrupt fires.
;---------------------------------------------------------------------
;---------------------------------------------------------------------
; An interrupt has fired!  Restore MCLK to its pre-sleep rate, and
; return to the caller (which should free interrupts, allowing the
; interrupt to be vectored).
;

; ======================================================================
;Enable the MMU
; ======================================================================
; N/A
; ======================================================================
; restore the registers and return
; ======================================================================
;   ldmfd   sp!, {r4-r12,lr}
    bx     lr
    END

