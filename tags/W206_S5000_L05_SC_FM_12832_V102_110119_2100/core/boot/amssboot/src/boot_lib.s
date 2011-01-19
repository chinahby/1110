;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                   B O O T    R E S E T    H A N D L E R
;
; GENERAL DESCRIPTION
;   This module contains library functions for boot-up.                 
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
; Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when      who     what, where, why
; --------  -----   ---------------------------------------------------------
; 03/30/07  cr      SC2X change such that kernel call supports running
;                   SDRAM self-refresh code in ICache pre-fetch.
; 02/02/06  ly      Created
;============================================================================

;============================================================================
;
;                             MODULE INCLUDES
;
;============================================================================

#include "customer.h"
#include "armasm.h"

;Cannot pass FEATURE_LIBRARY_ONLY as comipler option as in C
;Define it before it is used in msm.h
#define FEATURE_LIBRARY_ONLY

#include "msm.h"
#include "mpmc.h"

#define WDOG_COUNT 1000

#define PSR_Fiq_Mask         0x40
#define PSR_Irq_Mask         0x80
;=======================================================================
;
;                             MODULE DATA 
;
;=======================================================================

;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
        IMPORT  clk_regime_sdram_self_refresh_and_wdog_reset 

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================
        EXPORT  set_sdram_self_refresh_and_wdog_reset
;============================================================================
        AREA    Boot_lib_code, CODE, READONLY
        CODE32                               ; 32 bit ARM instruction set

;=======================================================================
; SET_SDRAM_SELF_REFRESH_AND_WDOG_RESET
;
; DESCRIPTION
; This function performs the following action:
;     1. Disable interrupt
;     2. Pre-fetch code for putting sdram to self-refresh and triggering
;        wdog reset to cache
;     3. Put sdram into self-refresh mode
;     4. Trigger wdog reset 
;
; SIDE FFECTS
;   Never returns and cause reset to occur
;=======================================================================
set_sdram_self_refresh_and_wdog_reset

; ======================================================================
; Save the registers
; ======================================================================
   stmfd sp!, {r0-r4,lr}
   bl clk_regime_sdram_self_refresh_and_wdog_reset
; ======================================================================
; restore the registers and return
; ======================================================================
   ldmfd   sp!, {r0-r4,pc}
   
;============================================================================
;============================================================================
        END
