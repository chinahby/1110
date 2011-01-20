;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;               D I S P   A S S E M B L Y   S E R V I C E S
;
; GENERAL DESCRIPTION
;
; EXTERNALIZED FUNCTIONS
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
; Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
; 
;                           EDIT HISTORY FOR FILE
;  This section contains comments describing changes made to the module.
;  Notice that changes are listed in reverse chronological order.
;
;$Header: //source/qcom/qct/multimedia/display/mdp1/lcd_qsc1100/main/latest/src/disp.s#1 $
;   
;when       who     what, where, why
;--------   ---     ----------------------------------------------------------
;07/11/03   dle     Removed featurization.
;07/23/02   dle     Added support for 6050 bus sizing
;07/16/02   dle     Added disp_copy for 16-bit mode
;04/25/02   dle     Initial version
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                           MODULE INCLUDE FILES
;
;============================================================================

#include "armasm.h"        
#include "disp_options.h"        

;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
                
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

        EXPORT memset16
        EXPORT disp_copy16
        EXPORT disp_copy32
        EXPORT disp_capture16
        EXPORT disp_capture32

        CODE32
        AREA    DISP32, CODE, READONLY

 ENTRY_NODE     memset16
        /* r0 destination */
        /* r1 pattern     */
        /* r2 count       */

        stmdb  sp!,{r0-r11}

        /* see if starting at 16-bit boundary */
        ands   r3,r0,#2
        beq    memset161
        /* handle 16-bit boundary at beginning */
        strh   r1,[r0]
        sub    r2,r2,#1
        add    r0,r0,#2
memset161
        /* create 32-bit patterns */
        mov    r4,r1,lsl #16
        orr    r4,r4,r1
        mov    r5,r4
        mov    r6,r4
        mov    r7,r4
        mov    r8,r4
        mov    r9,r4
        mov    r10,r4
        mov    r11,r4

        /* r3 is number of 8x32-bit words */
        movs   r3,r2,lsr #4
        beq    memset163
memset162
        /* store 8x32 */
        stmia  r0!,{r4-r11}
        subs   r3,r3,#1
        bne    memset162
memset163
        ands   r3,r2,#0x8
        beq    memset164
        /* store 4x32 */
        stmia  r0!,{r4-r7}
memset164
        ands   r3,r2,#0x4
        beq    memset165
        /* store 2x32 */
        stmia  r0!,{r4-r5}
memset165
        ands   r3,r2,#0x2
        beq    memset166
        /* store 1x32 */
        stmia  r0!,{r4}
memset166
        ands   r3,r2,#0x1
        beq    memset167
        /* store 1x16 */
        strh   r1,[r0]
memset167
        ldmia  sp!,{r0-r11}
        bx     lr

 ENTRY_NODE_END
                                               
 ENTRY_NODE     disp_copy32
        /* r0 source             */
        /* r1 destination        */
        /* r2 2-byte word count  */

        stmdb  sp!,{r0-r11}

        /* If the first location is not at 32-bit boundary, */
        /* take care of that first. */
        mov   r3,#2
        ands  r3,r3,r0
        beq   disp_copy32_loop1
        ldrh  r3,[r0]
        strh  r3,[r1]
        add   r0,r0,#2
        sub   r2,r2,#1
disp_copy32_loop1
        /* Read and write 36 bytes each loop */
        cmp   r2,#18
        bmi   disp_copy32_loop2
        sub   r2,r2,#18
        ldmia r0!,{r3-r11}
        stmia r1,{r3-r11}
        b     disp_copy32_loop1
disp_copy32_loop2
        /* Read and write 12 bytes each loop */
        cmp   r2,#6
        bmi   disp_copy32_loop2_end
        sub   r2,r2,#6
        ldmia r0!,{r3-r5}
        stmia r1,{r3-r5}
        b     disp_copy32_loop2
disp_copy32_loop2_end
        /* Read and write 4 bytes each loop */
        movs   r3,r2,lsr #1
        beq   disp_copy32_loop3_end
disp_copy32_loop3
        ldmia r0!,{r4}
        str   r4,[r1]
        subs   r3,r3,#1
        bne    disp_copy32_loop3
disp_copy32_loop3_end
        /* Do we have 2-byte word left? */
        mov   r3,#1
        ands  r3,r3,r2
        beq   disp_copy32_end
        ldrh  r3,[r0]
        strh  r3,[r1]
disp_copy32_end
        ldmia  sp!,{r0-r11}
        bx     lr

 ENTRY_NODE_END

 ENTRY_NODE     disp_capture32
        /* r0 source             */
        /* r1 destination        */
        /* r2 2-byte word count  */

        stmdb  sp!,{r0-r11}

        /* If the first location is not at 32-bit boundary, */
        /* take care of that first. */
        mov   r3,#2
        ands  r3,r3,r1
        beq   disp_capture32_loop1
        ldrh  r3,[r0]
        strh  r3,[r1]
        add   r1,r1,#2
        sub   r2,r2,#1
disp_capture32_loop1
        /* Read and write 36 bytes each loop */
        cmp   r2,#18
        bmi   disp_capture32_loop2
        sub   r2,r2,#18
        ldmia r0,{r3-r11}
        stmia r1!,{r3-r11}
        b     disp_capture32_loop1
disp_capture32_loop2
        /* Read and write 12 bytes each loop */
        cmp   r2,#6
        bmi   disp_capture32_loop2_end
        sub   r2,r2,#6
        ldmia r0,{r3-r5}
        stmia r1!,{r3-r5}
        b     disp_capture32_loop2
disp_capture32_loop2_end
        /* Read and write 4 bytes each loop */
        movs   r3,r2,lsr #1
        beq   disp_capture32_loop3_end
disp_capture32_loop3
        ldmia r0,{r4}
        str   r4,[r1]
        add   r1,r1,#4
        subs   r3,r3,#1
        bne    disp_capture32_loop3
disp_capture32_loop3_end
        /* Do we have 2-byte word left? */
        mov   r3,#1
        ands  r3,r3,r2
        beq   disp_capture32_end
        ldrh  r3,[r0]
        strh  r3,[r1]
disp_capture32_end
        ldmia  sp!,{r0-r11}
        bx     lr

 ENTRY_NODE_END

        CODE16
        AREA    DISP16, CODE, READONLY

 ENTRY_NODE     disp_copy16
        /* r0 source              */
        /* r1 destination         */
        /* r2 2-byte word count   */

        push {r0-r7, r14}

        /* If the first location is not at 32-bit boundary, */
        /* take care of that first. */
        mov   r3,#2
        and   r3,r0
        beq   disp_copy16_loop1
        ldrh  r3,[r0]
        strh  r3,[r1]
        add   r0, #2
        sub   r2, #1
        /* Read 20 bytes each loop */
disp_copy16_loop1
        cmp   r2, #10
        bmi   disp_copy16_loop1_end
        sub   r2, #10
        ldmia r0!,{r3-r7}
        strh  r3,[r1]
        lsr   r3,#16
        strh  r3,[r1]
        strh  r4,[r1]
        lsr   r4,#16
        strh  r4,[r1]
        strh  r5,[r1]
        lsr   r5,#16
        strh  r5,[r1]
        strh  r6,[r1]
        lsr   r6,#16
        strh  r6,[r1]
        strh  r7,[r1]
        lsr   r7,#16
        strh  r7,[r1]
        b     disp_copy16_loop1
disp_copy16_loop1_end
        add   r2, #1
        /* Copy 2 bytes at a time to the device */
disp_copy16_loop2
        sub   r2, #1
        beq   disp_copy16_loop2_end
        ldrh  r3, [r0]
        add   r0, #2
        strh  r3, [r1]
        b     disp_copy16_loop2
disp_copy16_loop2_end
        pop   {r0-r7, pc}

 ENTRY_NODE     disp_capture16
        /* r0 source              */
        /* r1 destination         */
        /* r2 2-byte word count   */

        push {r0-r7, r14}

        /* If the first location is not at 32-bit boundary, */
        /* take care of that first. */
        mov   r3,#2
        and   r3,r0
        beq   disp_capture16_loop1
        ldrh  r3,[r0]
        strh  r3,[r1]
        add   r1, #2
        sub   r2, #1
        /* Read 20 bytes each loop */
disp_capture16_loop1
        cmp   r2, #8
        bmi   disp_capture16_loop1_end
        sub   r2, #8
        ldrh  r3,[r0]
        ldrh  r4,[r0]
        lsl   r4,#16
        orr   r4,r3
        ldrh  r3,[r0]
        ldrh  r5,[r0]
        lsl   r5,#16
        orr   r5,r3
        ldrh  r3,[r0]
        ldrh  r6,[r0]
        lsl   r6,#16
        orr   r6,r3
        ldrh  r3,[r0]
        ldrh  r7,[r0]
        lsl   r7,#16
        orr   r7,r3
        stmia r1!,{r4-r7}
        b     disp_capture16_loop1
disp_capture16_loop1_end
        add   r2, #1
        /* Copy 2 bytes at a time to the device */
disp_capture16_loop2
        sub   r2, #1
        beq   disp_capture16_loop2_end
        ldrh  r3, [r0]
        strh  r3, [r1]
        add   r1, #2
        b     disp_capture16_loop2
disp_capture16_loop2_end
        pop   {r0-r7, pc}

 ENTRY_NODE_END

        END

