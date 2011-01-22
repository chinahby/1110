;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==
;
;                             TIMER CORE ROUTINES
;
; GENERAL DESCRIPTION
;   Some of the timer code needs to run in a very predictable manner such that 
;   it needs to be ported into assembly.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   None.
;
; Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==


;==============================================================================
;
;                            EDIT HISTORY FOR FILE                            
;
;  $Header: //depot/asic/msmshared/services/time/timer_asm.s#6 $
;
;  This section contains comments describing changes made to the module.
;  Notice that changes are listed in reverse chronological order.
;
;  When        Who  What, where, why
;  ----------  ---  -----------------------------------------------------------
;  09/26/2005  jhs  Changed one extra read of AGPT_MATCH_VAL after writing to
;                   four dummy reads.  This is to account for a 5 AHB clock
;                   cycle delay from when the value is written and when it
;                   actually posts.
;  09/12/2005  jhs  Added extra read of AGPT_MATCH_VAL after writing
;                   match_val...
;  08/17/2005  jhs  Added an APPS_PROC version.
;  08/16/2005  ajn  Added padding to always 32-byte align set_next_int_entry
;  08/10/2005  ajn  Can't use co-processor 15, or won't work with L4.
;  08/05/2005  ajn  Initial version of file
;
;==============================================================================


;==============================================================================
;
;                             MODULE INCLUDE FILES
;
;==============================================================================

#include "customer.h"
#include "msm.h"

;-=============================================================================
;
;                               MACRO DEFINITIONS
;
;==============================================================================

#define REG_OFFSET ( HWIO_ADDR(AGPT_COUNT_VAL) - HWIO_ADDR(AGPT_MATCH_VAL) )

    
;-=============================================================================
;
;                               DATA DEFINITIONS
;
;==============================================================================

#ifdef IMAGE_APPS_PROC

    AREA TIMER_DATA, DATA, ALIGN=2

last_set_time
        DCD 0

#endif

;==============================================================================
;
;                            EXTERNALIZED FUNCTIONS
;
; timer_set_next_interrupt_core
;   Sets the TIMETICK_MATCH_VAL register for the next timer interrupt
;
;==============================================================================

    EXPORT  timer_set_next_interrupt_core

;==============================================================================
;
;                                     AREA
;
; Part of the timer code needs to execute what effectively is a
; "read-modify-write" operation, where the write is not significantly delayed
; from the read.  When code or data is fetched from EBI1 memory, burst
; activity by another EBI1 bus master (such as the ADSP) will cause the ARM
; processor to stall until that burst activity of the other bus master
; completes.  These stalls can be 200us or more.
;
; To avoid a stall due to data fetches from main memory, all data needed by the
; critical code section is moved to registers prior to executing the code.
;
; To avoid a stall due to instruction code fetches from main memory is more
; complicated.  The code must be loaded into the Instruction Cache (ICache)
; prior to execution.  One method is to carefully program coprocessor 15 to
; load the code into ICache, but that requires supervisor priviledges, and
; may not work well with "L4".  A second method, the one used here, is to
; rely upon the ICache line-fill behaviour.  When an instruction fetch causes
; an ICache miss, the entire cache line (8 words, or 32 bytes) is read into
; memory, starting with the critical word the fetched instruction is in.
; Thus, before the cache line is filled, the ARM can begin executing the code.
; If the first instruction is a branch to code in another cache line, the
; line-fill will complete before the fetch & cache line-fill for the second
; code section is started.  Thus, once the second code section execution
; begins, the first cache line-fill must be complete, and a branch back to that
; section will execute entirely from ICache.
;
; This second method relies on:
;   - the branch + critical code section fitting in one ICache line (8 words)
;   - the second code section not kicking the 1st section out of ICache.
;
; To ensure the above, the code will be aligned on a multiple of two cache
; lines, or 16 word (2^6=64 byte) boundary.  
;
;==============================================================================

    AREA    TIMER_ASM, CODE, READONLY, ALIGN=6


    ; One ICache line can hold 8 ARM instructions, or 16 Thumb instructions.
    ; To fit as much as possible in one ICache line, use Thumb mode!
        
    CODE16

#ifdef IMAGE_APPS_PROC
;==============================================================================
;
; TIMER_SET_NEXT_INTERRUPT_CORE
;
; DESCRIPTION
;   Read and write timer registers in a direct manner with no use of ROM or
;   RAM during the critical section to avoid EBI1 bus contention.
;
; ARGUMENTS
;   r0 : match_val      - the time of next interrupt in sleep clocks
;   r1 : origin         - an origin for roll-over safe comparisons
;   r2 : MIN_REARM_SCLK - the number of SCLKs needed to safely write MATCH_VAL
;
; SIDE EFFECTS
;   None
;
;==============================================================================


; TODO
; Seperate into three sections
; first is time dependant pad to put _enter into second
; second is setup and begining of SW workaround
; third is end of SW work around and data

timer_set_next_interrupt_core

    ; This first instruction sits at the start of a 32-byte ICache line
    ; boundary.  Fetching this instruction will cause the remaining 15
    ; 2-byte thumb instructions to be pulled into the ICache.
    
    b       set_next_int_entry  ;                                          0x00
    

    ; The following 11-14 thumb instructions must be executed from ICache.
    ; Since the EBI1 bus is not accessed, so ADSP burst activity will not
    ; cause this time-critical code to stall mid-execution.

set_next_int_core

    ; Read the current slow clock counter value, or "now"
    
    ldr     r7, [r5, #REG_OFFSET]  ; Read AGPT_COUNT value                 0x02
    
read_count_loop
    mov     r3, r7                 ; Save last AGPT_COUNT read value       0x04
    ldr     r7, [r5, #REG_OFFSET]  ; Read AGPT_COUNT value again ...       0x06
    cmp     r3, r7                 ;   ... and again until the same value  0x08
    bne     read_count_loop        ;   is read twice.                      0x0A


    ; Compare the time from an arbitrary origin to the match value with
    ; the time from an arbitrary origin to a little beyond the current time.
    ; Select the larger of the two time deltas.

    add     r7, r2              ; r7 = "now" - origin + MIN_REARM_SCLK     0x0C
    cmp     r7, r0              ; Is "now - origin + MIN_REARM" larger     0x0E
    bhs     r7_is_max           ;   than "match_val - origin"?             0x10
    
    mov     r7, r0              ; max( now-origin+MIN, match_val-origin)   0x12

r7_is_max

    ; Convert from a time delta back to an absolute value, and
    ; set the timetick match register to that value.

    add     r7, r1              ; Undo origin translation                  0x14
    str     r7, [r5]            ; And write match_val to match register    0x16

#ifdef T_MSM7500
#error code not present
#else


    mov     r0, r7              ; Move value we wrote to r0                0x18

    pop     {r4-r7,pc}          ; Return r0, the value written             0x1A

    nop                         ; Padding to ensure that                   0x1C
    nop                         ;   "set_next_int_entry" begins on         0x1E
                                ;   a 32-byte cache line boundary.

#endif

    ; End of code which must exist in a single 32-byte ICache cache line.

set_next_int_entry

    ; This code starts, or will fall, in the second 32-byte ICache line.
    ; It will not be fetched until the previous cache-line fill has completed,
    ; which guarentees no ADSP induced bus-stalls will occur in the above
    ; code, once we branch back to it.

    push    {r4-r7,lr}          ; Save registers                           0x20
     
    adr     r3, msm_reg_addr    ; Get the address of the address data      0x22
    ldmia   r3!, {r5-r6}        ; r5 = &match, r6 = &last_set_time         0x24

    ldr     r4,[r6]             ; r4 = last_set_time                       0x26
    
    sub     r0, r1              ; r0 = match_val - origin                  0x28
    sub     r2, r1              ; r2 = MIN_REARM_SCLK - origin             0x2A
    
    b       set_next_int_core   ; Jump to code section cached in ICache.   0x2C


    ALIGN   4
    
msm_reg_addr

    dcd     HWIO_ADDR(AGPT_MATCH_VAL)
    dcd     last_set_time

#ifdef T_MSM7500
#error code not present
#endif

    
; IMAGE_APPS_PROC
#else

;==============================================================================
;
; TIMER_SET_NEXT_INTERRUPT_CORE
;
; DESCRIPTION
;   Read and write timer registers in a direct manner with no use of ROM or
;   RAM during the critical section to avoid EBI1 bus contention.
;
; ARGUMENTS
;   r0 : match_val      - the time of next interrupt in sleep clocks
;   r1 : origin         - an origin for roll-over safe comparisons
;   r2 : MIN_REARM_SCLK - the number of SCLKs needed to safely write MATCH_VAL
;
; SIDE EFFECTS
;   None
;
;==============================================================================

timer_set_next_interrupt_core

    ; This first instruction sits at the start of a 32-byte ICache line
    ; boundary.  Fetching this instruction will cause the remaining 15
    ; 2-byte thumb instructions to be pulled into the ICache.
    
    b       set_next_int_entry  ;                                          0x00
    

    ; The following 11-14 thumb instructions must be executed from ICache.
    ; Since the EBI1 bus is not accessed, so ADSP burst activity will not
    ; cause this time-critical code to stall mid-execution.

set_next_int_core

    ; Read the current slow clock counter value, or "now"
    
    ldr     r0, [r5]            ; Read TIMETICK_COUNT value                0x02
    
read_count_loop
    mov     r3, r0              ; Save last TIMETICK_COUNT read value      0x04
    ldr     r0, [r5]            ; Read TIMETICK_COUNT value again ...      0x06
    cmp     r3, r0              ;   ... and again until the same value     0x08
    bne     read_count_loop     ;   is read twice.                         0x0A


    ; Compare the time from an arbitrary origin to the match value with
    ; the time from an arbitrary origin to a little beyond the current time.
    ; Select the larger of the two time deltas.

    add     r0, r2              ; r0 = "now" - origin + MIN_REARM_SCLK     0x0C
    cmp     r0, r4              ; Is "now - origin + MIN_REARM" larger     0x0E
    bhs     r0_is_max           ;   than "match_val - origin"?             0x10
    
    mov     r0, r4              ; max( now-origin+MIN, match_val-origin)   0x12

r0_is_max

    ; Convert from a time delta back to an absolute value, and
    ; set the timetick match register to that value.

    add     r0, r1              ; Undo origin translation                  0x14
    str     r0, [r6]            ; And write match_val to match register    0x16


#if defined HWIO_SLEEP_XTAL_TIMETICK_MATCH_VAL_STATUS_SYNC_STATUS_BMSK

    ; Get the latch status.  If the SYNC_STATUS bit is not set, then the
    ; write did not take effect, and this process must be repeated.

    ldr     r3, [r7]            ; Read STATUS register                     0x18
    lsl     r3, #31-HWIO_SLEEP_XTAL_TIMETICK_MATCH_VAL_STATUS_SYNC_STATUS_SHFT
    bpl     set_next_int_core   ; while (SYNC_STATUS == 0)                 0x1C
    pop     {r4-r7,pc}          ; Return r0, the value written             0x1E
    
#else

    pop     {r4-r7,pc}          ; Return r0, the value written             0x18
    
    nop                         ; Padding to ensure that                   0x1A
    nop                         ;   "set_next_int_entry" begins on         0x1C
    nop                         ;   a 32-byte cache line boundary.         0x1E
    
#endif

    ; End of code which must exist in a single 32-byte ICache cache line.


    
set_next_int_entry

    ; This code starts in the second 32-byte ICache line.
    ; It will not be fetched until the previous cache-line fill has completed,
    ; which guarentees no ADSP induced bus-stalls will occur in the above
    ; code, once we branch back to it.

    push    {r4-r7,lr}          ; Save registers                           0x20
     
    adr     r3, msm_reg_addr    ; Get the address of the address data      0x22
    ldmia   r3!, {r5-r7}        ; r5 = &count, r6 = &match, r7 = &status   0x24
    
    mov     r4, r0              ; r4 = match_val ...                       0x26
    sub     r4, r1              ; r4 = match_val - origin                  0x28
    sub     r2, r1              ; r2 = MIN_REARM_SCLK - origin             0x2A
    
    b       set_next_int_core   ; Jump to code section cached in ICache.   0x2C


    ALIGN   4
    
msm_reg_addr

    dcd     HWIO_ADDR(SLEEP_XTAL_TIMETICK_COUNT)
    dcd     HWIO_ADDR(SLEEP_XTAL_TIMETICK_MATCH_VAL)
    
#if defined HWIO_SLEEP_XTAL_TIMETICK_MATCH_VAL_STATUS_SYNC_STATUS_BMSK
    dcd     HWIO_ADDR(SLEEP_XTAL_TIMETICK_MATCH_VAL_STATUS)
#else
    dcd     0
#endif

; IMAGE_APPS_PROC
#endif


    END
