;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                     SLEEP MACROS
;
; GENERAL DESCRIPTION
;
; Contains macros and defines for the sleep code
; EXTERNALIZED FUNCTIONS
;   None.
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
;
; when      who     what, where, why
; --------  -----   ---------------------------------------------------------
; 02/28/05  ts      Featurized code for doing all initialization in cfg data.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                           MODULE INCLUDE FILES
;
;============================================================================

; Warmup time is in sleep clk cycles (32kHz usually)
TCXO_WARMUP_TIME_CYCLES                           EQU    99

#ifndef FEATURE_INIT_HW_WITH_CFG_DATA
#ifdef BOOT_MODE_NOR
MISC_CLK_CTL_SLEEP_CONFIG_VALUE                   EQU    0x4036005F
#else
MISC_CLK_CTL_SLEEP_CONFIG_VALUE                   EQU    0x4076005F
#endif
#endif
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
;=======================================================================
; MACRO put_sdram_self_refresh
; ARGS
;   NONE
; DESCRIPTION
; Sets the command to put the SDRAM to self refresh mode and waits for
; the self refresh mode is entered
;===================================================================

    MACRO
    put_sdram_self_refresh
#ifndef FEATURE_SC2X_SW_BOOT_NOT_PORT
    ldr    r0, =HWIO_ADDR(MPMC_DY_CNTL)
    ldr    r1,[r0]
    orr    r1,r1,#MPMC_DY_CNTL_WW__REFRESH_MODE_MASK
    str    r1,[r0]
; wait for the SDRAM to be in self refresh mode
    ldr    r0, =HWIO_ADDR(MPMC_STATUS)
    ldr    r1, =MPMC_STATUS_RW__SELF_REFRESH_ACKNOWLEDGE_MASK
wait_for_self_refresh_mode
    ldr    r2,[r0]
    and    r2,r2,#MPMC_STATUS_RW__SELF_REFRESH_ACKNOWLEDGE_MASK:OR:MPMC_STATUS_RW__WRITE_BUFFER_STATUS_MASK
    cmp    r2,r1
    bne    wait_for_self_refresh_mode
#endif
    MEND

;=======================================================================
; MACRO take_sdram_from_self_refresh
; ARGS
;   NONE
; DESCRIPTION
; Sets the command to take the SDRAM out of self refresh mode and
; waits for the normal mode is entered
;===================================================================

    MACRO
    take_sdram_from_self_refresh
#ifndef FEATURE_SC2X_SW_BOOT_NOT_PORT
    ldr    r0, =HWIO_ADDR(MPMC_DY_CNTL)
    ldr    r1,[r0]
    bic    r1,r1,#MPMC_DY_CNTL_WW__REFRESH_MODE_MASK
    str    r1,[r0]
; wait for the SDRAM to be out of self refresh
    ldr    r0, =HWIO_ADDR(MPMC_STATUS)
    ldr    r1, =MPMC_STATUS_RW__SELF_REFRESH_ACKNOWLEDGE_MASK
wait_for_normal_mode
    ldr    r2, [r0]
    and    r2,r2,r1
    cmp    r1,r2
    beq    wait_for_normal_mode
#endif
    MEND

;===================================================================
; MACRO set_mpmc_refresh_rate
; ARGS
;   value of refresh rate in cycles
; DESCRIPTION
; Sets ther refresh rate for the SDRAM to the specified value
;===================================================================

     MACRO
$label    set_mpmc_refresh_rate   $cycles
#ifndef FEATURE_SC2X_SW_BOOT_NOT_PORT
    ldr   r1, = $cycles
    ldr   r0, =HWIO_ADDR(MPMC_DY_REF)
    str   r1, [r0]
#endif
    MEND

