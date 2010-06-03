;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==
;
;                         SCLK MSM REG WRITE ROUTINES
;
; GENERAL DESCRIPTION
;
;   Some of MSM registers clocked off of the sleep clock needs to run in very
;   predictable manner such that it needs to be ported into assembly.
;
;   The following functions pre-cache themselves, and then access the MSM
;   registers in a way that avoids EBI1 bus accesses.
;
; EXTERNALIZED FUNCTIONS
;
;   sclk_reg_set_min
;     Write a value to an MSM register.  Value will be constrained to be at
;     least a certain amount greater than another readable MSM register.
;
;   sclk_reg_set_sync
;     Write a value to an MSM register.  Repeat writing the value to the MSM
;     register until a sync status bit transitions high.
;
;   sclk_reg_set_min_sync
;     Write a value to an MSM register.  Value will be constrained to be at
;     least a certain amount greater than another readable MSM register.
;     Repeat writing the value to the MSM register until a sync status bit
;     transitions high.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
;   None.
;
; Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==
;
;                            EDIT HISTORY FOR FILE                            
;
;  $Header: //source/qcom/qct/modem/mcs/sleepctl/main/latest/src/sclk_asm.s#1 $
;
;  This section contains comments describing changes made to the module.
;  Notice that changes are listed in reverse chronological order.
;
;  When        Who  What, where, why
;  ----------  ---  -----------------------------------------------------------
;  08/15/2005  ajn  Initial version of file (based on timer_asm.s)
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



;==============================================================================
;
;                            MODULE INCLUDE FILES
;
;==============================================================================

#include "customer.h"
#include "msm.h"



;==============================================================================
;
;                               MODULE EXPORTS
;
;==============================================================================

    EXPORT  sclk_reg_set_min
    EXPORT  sclk_reg_set_sync
    EXPORT  sclk_reg_set_min_sync


;==============================================================================
;
;                                     AREA
;
; The sclk register access code needs to execute what effectively is a
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

    AREA    SCLK_ASM, CODE, READONLY, ALIGN=6


    ; One ICache line can hold 8 ARM instructions, or 16 Thumb instructions.
    ; To fit as much as possible in one ICache line, use Thumb mode!
        
    CODE16



;==============================================================================
;
; SCLK_REG_SET_MIN(val, min, write_reg, read_reg)
;
; DESCRIPTION
;   Read an MSM register, ensure "val" is at least "min" larger than the
;   value read, and write the thresholded value to the write register.
;
;   MSM registers are accessed in a direct manner with no use of ROM or RAM
;   during the critical section to avoid bus contention.
;
; ARGUMENTS
;   r0 : val - the value to write to the register
;   r1 : min - value written must be at least "min" larger than value read
;   r2 : write_reg - address of the MSM register to be written
;   r3 : read_reg - address of the MSM register to be read
;
; SIDE EFFECTS
;   Function code is pre-cached to avoid EBI1 bus stalls
;
;==============================================================================


;==============================================================================
;
; SCLK_REG_SET_SYNC(val, mask, write_reg, sync_reg)
;
; DESCRIPTION
;   Write an MSM register, ensuring that "mask" is set in "sync_reg" after the
;   write operation.  Repeat writes until "mask" is set.
;
;   MSM registers are accessed in a direct manner with no use of ROM or RAM
;   during the critical section to avoid bus contention.
;
; ARGUMENTS
;   r0 : val - the value to write to the register
;   r1 : mask - the mask that must be set in the sync register after the write
;   r2 : write_reg - address of the MSM register to be written
;   r3 : sync_reg - address of the MSM register containing mask bits
;
; SIDE EFFECTS
;   Function code is pre-cached to avoid EBI1 bus stalls
;
;==============================================================================

sclk_reg_set_min

    ; This first instruction sits at the start of a 32-byte ICache line 
    ; boundary.  Fetching this instruction will cause the remaining 15
    ; 2-byte thumb instructions to be pulled into the ICache.
    
    b       min_entry           ;                                          0x00
    
    
sclk_reg_set_sync

    ; This instruction sits near the start of a 32-byte ICache line boundary.
    ; Fetching this instruction will cause the remaining 14 2-byte thumb
    ; instructions to be pulled into the ICache.
    
    b       sync_entry          ;                                          0x02
    


    ;---------------------------------------------------------------------
    ; The following 14 thumb instructions must be executed from ICache.
    ; Since the EBI1 bus is not accessed, ADSP burst activity will not
    ; cause this time-critical code to stall mid-execution.
    ;---------------------------------------------------------------------


    ;---------------------------------------------------------------------
    ; The next 11 ICached instructions belong to sclk_reg_set_min
    ;---------------------------------------------------------------------
    
min_core_function

    ; Read the current slow clock counter value
    
    ldr     r4, [r3]            ; Read the counter register value          0x04
    
read_count_loop
    mov     r5, r4              ; Save last value read value               0x06
    ldr     r4, [r3]            ; Read counter register value again ...    0x08
    cmp     r4, r5              ;   ... and again until the same value     0x0A
    bne     read_count_loop     ;   is read twice.                         0x0C


    ; Compare the given set value with the a little above the current
    ; count.  Select the larger of the two values.

    add     r4, r1              ; r0 = count + min                         0x0E
    cmp     r0, r4              ; Is "count + min" larger                  0x10
    bhs     r0_is_max           ;   than the given set value               0x12
    
    mov     r0, r4              ; r0 = max( count+MIN, val )               0x14

r0_is_max

    ; Set the register to be written to the larger of the two values.
    str     r0, [r2]            ;                                          0x16


    ; End of code which must exist in a single 32-byte ICache cache line.

    pop     {r4-r5,pc}          ; Return r0, the value written             0x18



    ;---------------------------------------------------------------------
    ; The next 5 instructions belong to sclk_reg_set_sync
    ; Only the first 2 instructions (str/ldr) must be in the ICache.
    ;---------------------------------------------------------------------
    
sync_core_function

    ; Set the register to be written to the required value.

    str     r0, [r2]            ; And write match_val to match register    0x1A


    ; Get the latch status.  If the SYNC_STATUS bit is not set, then the
    ; write did not take effect, and this process must be repeated.

    ldr     r4, [r3]            ; Read STATUS register                     0x1C

    ; End of code which must exist in a single 32-byte ICache cache line.
    
    tst     r4, r1              ; Is the SYNC_STATUS bit set?              0x1E
    beq     sync_core_function  ; Repeat until it is                       0x20

sync_exit

    pop     {r4,pc}             ; Return r0, the value written             0x22



    ;---------------------------------------------------------------------
    ; Entry of sclk_reg_set_min
    ;---------------------------------------------------------------------
    
min_entry

    ; This code starts in the second 32-byte Instruction Cache line.
    ; It will not be fetched until the previous cache-line fill has
    ; completed, which guarentees no ADSP induced bus-stalls will occur in
    ; the above code, once we branch back to it.

    push    {r4-r5,lr}          ; Save registers                           0x24
    
    b       min_core_function   ; Jump to code section cached in ICache.   0x26



    ;---------------------------------------------------------------------
    ; Entry of sclk_reg_set_sync
    ;---------------------------------------------------------------------
    
sync_entry

    ; This code starts in the second 32-byte Instruction Cache line.
    ; It will not be fetched until the previous cache-line fill has
    ; completed, which guarentees no ADSP induced bus-stalls will occur in
    ; the above code, once we branch back to it.

    push    {r4,lr}             ; Save registers                           0x28
    
    b       sync_core_function  ; Jump to code section cached in ICache.   0x2A






;==============================================================================
;
; SCLK_REG_SET_MIN_SYNC(val, min, mask, write_reg, read_reg, sync_reg)
;
; DESCRIPTION
;   Read an MSM register, ensure "val" is at least "min" larger than the
;   value read, and write the thresholded value to the write register.
;   Repeat the read-modify-write until "mask" is set in "sync_reg".
;
;   MSM registers are accessed in a direct manner with no use of ROM or RAM
;   during the critical section to avoid bus contention.
;
; ARGUMENTS
;   r0 : val - the value to write to the register
;   r1 : min - value written must be at least "min" larger than value read
;   r2 : mask - the mask that must be set in the sync register after the write
;   r3 : write_reg - address of the MSM register to be written
;   ** : read_reg - address of the MSM register to be read
;   ** : sync_reg - address of the MSM register containing mask bits
;
; SIDE EFFECTS
;   Function code is pre-cached to avoid EBI1 bus stalls
;
;==============================================================================

    ALIGN 64                    ; Get back on a double ICache line boundary

sclk_reg_set_min_sync

    ; This first instruction sits at the start of a 32-byte ICache line 
    ; boundary.  Fetching this instruction will cause the remaining 15
    ; 2-byte thumb instructions to be pulled into the ICache.
    
    b       min_sync_entry      ;                                          0x00
    

    ; The following 14 thumb instructions must be executed from ICache.
    ; Since the EBI1 bus is not accessed, ADSP burst activity will not
    ; cause this time-critical code to stall mid-execution.


min_sync_core_func

    ; Read the current slow clock counter value
    
    ldr     r6, [r4]            ; Read the counter register value          0x02
    
min_sync_read_loop
    mov     r7, r6              ; Save last value read value               0x04
    ldr     r6, [r4]            ; Read counter register value again ...    0x06
    cmp     r6, r7              ;   ... and again until the same value     0x08
    bne     min_sync_read_loop  ;   is read twice.                         0x0A


    ; Compare the given set value with the a little above the current
    ; count.  Select the larger of the two values.

    add     r6, r1              ; r0 = count + min                         0x0C
    cmp     r0, r6              ; Is "count + min" larger                  0x0E
    bhs     min_sync_r0_is_max  ;   than the given set value               0x10
    
    mov     r0, r6              ; r0 = max( count+MIN, val )               0x12

min_sync_r0_is_max

    ; Set the register to be written to the larger of the two values.
    str     r0, [r3]            ;                                          0x14


    ; "r0" was either unchanged, or increased to "count+MIN".  This is
    ; the value that will be returned from the function.  Also, if the
    ; branch below is taken, the new "r0" value will be used in the
    ; comparison above.  If "r0" was unchanged, the code operates as
    ; expected.  If "r0" was changed to count+MIN, since the count value
    ; will increase in future iterations, the new "count+MIN" value will
    ; be selected, as desired.
    

    ; Get the latch status.  If the SYNC_STATUS bit is not set, then the
    ; write did not take effect, and this process must be repeated.

    ldr     r6, [r5]            ; Read STATUS register                     0x16

    ; End of code which must exist in a single 32-byte ICache cache line.
    
    tst     r6, r2              ; Is the SYNC_STATUS bit set?              0x18
    beq     min_sync_core_func  ; Repeat until it is                       0x1A

    pop     {r4-r7,pc}          ; Return r0, the value written             0x1C

    
min_sync_entry

    ; This code starts in the second 32-byte Instruction Cache line.
    ; It will not be fetched until the previous cache-line fill has completed,
    ; which guarentees no ADSP induced bus-stalls will occur in the above code,
    ; once we branch back to it.

    push    {r4-r7,lr}          ; Save registers                           0x1E

    add     r7,sp,#0x14         ; Point to arguments passed on stack       0x20
    ldmia   r7!,{r4,r5}         ; Load the stack arguments to r4,r5        0x22
    
    b       min_sync_core_func  ; Jump to code section cached in ICache.   0x24


    END
