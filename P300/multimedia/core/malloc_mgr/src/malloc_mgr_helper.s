#include "customer.h"
	PRESERVE8
        CODE32 ; ARM mode

        AREA ||.text||, CODE, READONLY

        ;
        ; int MALLOCMGR_SYSTEM_CHECK_TEXT_SECTION_POINTER(void *ptr);
        ;

        IMPORT MALLOCMGR_SYSTEM_CHECK_TEXT_SECTION_POINTER

#ifdef FEATURE_L4
#error code not present
#else
	;
        ; void void rex_task_preamble(void (*func_ptr)( dword arg ), dword arg);
        ;

        IMPORT rex_task_preamble
#endif

        ;-----------------------------------------------------------------------
        ; signed long MALLOCMGR_UNWIND_STACK(void **, unsigned long);
        ;-----------------------------------------------------------------------
        ; inputs:
	;
        ; r0 -- pointer to buffer
        ; r1 -- buffer length (in bytes)
        ; lr -- return address from this function
        ;-----------------------------------------------------------------------
	; output:
	;
	; r0 -- length of recorded backtrace (in bytes)
        ;
        ;       If the backtrace is invalid, then r0 contains the NEGATIVE of
        ;       the backtrace that was collected until we detected that it was
        ;       invalid.
        ;-----------------------------------------------------------------------
	; internal registers:
	;
	; r2 -- Initialized to point to the input buffer;  for each stack frame
	;       that MALLOCMGR_UNWIND_STACK unwinds, the return address in 
	;       that stack frame is written to the location pointed to by r2, 
	;       and r2 is incremented by 4.
	;
	; r3 -- For each stack frame that we unwind, we set r3 to be the return
	;       address in that frame.  We write r3 to the location poitned to
	;       by r2 and increment r2 by 4.
	;
        ; r7 -- Running stack pointer;  we initialize this to the sp upon 
        ;       entering this function, and we advance it towards higher 
        ;       addresses as we unwind the stack.
        ;
        ; r4, r5, r6, r8 -- scratch
        ;-----------------------------------------------------------------------

        ;-----------------------------------------------------------------------
	; description:
        ;-----------------------------------------------------------------------
	; This function attempts to unwind the stack, starting with the return 
	; address of its called, and working down the stack it it sees a return
	; address that of zero.   For each stack frame, this function writes the
	; return address to the input buffer at an increasing index.  Thus, when
	; this function returns, [r0, #0] will contain the return address of the
	; caller of MALLOCMGR_UNWIND_STACK, [r0, #4] will contain the 
	; return address of the caller, and so on.
        ;-----------------------------------------------------------------------
    
    ; cannot use when FEATURE_STACK_CHECK is defined: that feature requires
    ; R10 to be reserved for use by the compiler. Empty function in this case.
    
MALLOCMGR_UNWIND_STACK PROC

#ifdef FEATURE_STACK_CHECK
        mov     r0, #0                  ; return 0;
	bx	lr
#else
	stmfd    sp!,{r4-r11,lr}	; Stack frame is 0x24 bytes deep.  This value
        add      r7, sp, #0x24  	; is used below directly, so make sure you 
                                	; change it if you add or remove registers from
                                	; the stack!  Register r7 to value of the stack
                                	; pointer when this function was called.

	mov	r2, r0          	; Copy the buffer pointer to r2.
	mov	r0, #0          	; Set the return value to zero.
	mov	r3, lr          	; Initialize r3 to the return address to the 
                                	; caller.


	; MAIN LOOP. Restart here after PUSH (or equivalent) instruction is found.
MAINLOOP
	cmp	r0, r1	        	; compare current bt depth with bt_buffer_length
        ldmeqfd sp!, {r4-r11,pc} 	; return if we have reached the max.

        mov     r8, #0          	; running counter containing the number of words
                                	; in the currect stack frame after processing these
					; instructions:
					;     SUB sp, #0xXX	;SUB(4) pageA7-101 in ARM Ref Man
					;     ADD r13, Rm	;ADD(4) pageA7-8 

        mov     r9, #0			; instruction counter to compare against the MAX while
					; backtracing each function on the call stack. This helps
					; HeapTracker stop in the event it gets stuck in an infinite
					; loop.

	mov     r11, #0			; r11 hold the register number in the event
					; ADD r13, Rm is found.

	
	; ARM OR THUMB MODE DETECTION.
	; Check to see whether the caller is in Thumb or ARM mode.
	; If the caller is in Thumb mode, bit zero of its return address
	; is set to 1.  If the caller is in ARM mode, bit zero of its
	; return address is set to 0.  We need to know the mode of the 
	; caller because the return instruction has a different encoding in
	; ARM mode and Thumb mode.
	; Most of the code is currently in Thumb mode, thus we do not support
	; ARM mode at the moment. The support will be added in the near future.
	and	r4, r3, #1	; get the last bit
	cmp	r4, #0		; compare against 0 (ARM mode)
	beq	ERROR_ARMMODE	; if equals to 0, meaning we are backtracing
				; ARM instructions, then return.


	; THUMB MODE. In this mode, we are looking for these instructions:
	; PUSH {<registers>, r14}	;PUSH pageA7-78
	; SUB sp, #0xXX
	; ADD r13, Rm
	; LDR Rd, [PC, #<immed_8> * 4]	;LDR(3) pageA7-46. This instruction is
	;				 valid only when ADD r13, Rm is detected.
        bic     r3, r3, r4      ; Since r3 actually points to an even address, 
                                ; but its value is odd, we need to clear the 
                                ; least-significant bit.  Since r4 is equal to
                                ; 1 already, we can use the BIC instruction to
                                ; accomplish this task.

        sub     r3, r3, #2      ; Since r3 points to the return address, r3 - 2
                                ; or r3 - 4 is the address of the instruction 
                                ; before that.  We do not know what the 
                                ; instruction was, so we just move back by two 
                                ; bytes.


	; THUMB MODE LOOP
	; max number of instruction to backtrace
	; in the event PUSH instruction is not found for a long time
THUMB_LOOP
	add	r9, r9, #1	; inc number of instructions so far
	cmp	r9, #1000	; max of instructions to stop
	beq	ERROR_MAXCOUNT	; branch forward to label 1 to handle error

	; load instruction into r10 and r3 -= 2. r10 holds the original
	; instruction value until the end of the loop.
	ldrh	r10, [r3], #-2	; load an unsigned half-word. The upper
                                ; 16 bits of r10 will be cleared.
				; r10 = [r3]; // upper half
				; r3 -= 2;    // r3 points to previous instruction

	; save the non-opcode in MSB of r5 in the event we process the SUB,
	; ADD, LDR, and PUSH instructions. In most cases, the non-opcode takes
	; up the last byte of the instruction.
        mov     r5, r10, lsl #24


	; Thumb PUSH instruction has the encoding:
	; 1011 010R XXXX XXXX
	; Where R is set to 1 if the LR is in the list. This is mostly the case.
	; So can safely mask out the LSB (byte).
	and     r4, r10, #0xFF00; Zero out the bits the LS byte

        cmp     r4, #0x0000B500 ; If the resulting value is 0x0000B500, 
        beq     THUMB_PUSH	; then we've encountered the caller's PUSH     
                                ; instruction.  If not, proceed to see whether
                                ; we have encountered a SUB sp instruction.

        ; Sometimes the compiler generates SUB sp, #immed instructions to push
        ; local variables temporarily on the stack.  The compiler always adds
        ; to the stack when it exits scope, however, and executes the same POP
        ; instruction with the push instruction.   If any local variables are
        ; allocated on the stack, then there may be several SUB instructions for
        ; all variables across all scopes in the function, and they are always
        ; close to the top of the function, shortly after the PUSH instruction 
        ; itself.  If we encounter such an instruction, we get the value of the
        ; immediate argument from it and add its value in register r8 (using
        ; r6 as scratch), which we then add to register r5 after we count the 
        ; number of bits in r5.  Note that the value in the immediate field of
        ; the SUB instruction is a number of long words, not a number of bytes!

 
	; Thumb SUB instruction has the encoding:
	; 1011 0000 1XXX XXXX
        ldr     r4, =0xB080FF80	; Since the upper 16 bits of r6 are zero, 
				; the upper 16 bits of the mask in r4 do not
				; matter in the AND instruction below.
        and     r6, r10, r4	; 0xFF80: Mask out the non-opcode bits.  
        cmp     r6, r4, lsr #16	; 0xB080: A SUB sp, {...} instruction?
        bne     THUMB_ADD	; If no, go on and look for ADD r13, Rm instr.
        mov     r6, r5, lsr #24	; If yes, save the immediate argument in r6.
        and     r6, r6, #0x7f	; Bit 7 is not a part of the register bitmask.
        add     r8, r8, r6	; Add to running counter.
        b       THUMB_LOOP	; go back to look for the PUSH instruction.

	
	; Thumb ADD instruction has the encoding:
	; 0100 0100 HHXX XXXX
	; We are looking specifically for ADD r13, Rm instruction. We have seen this
	; when JAVA is enabled on certain targets. ADD r13, Rm looks like this:
	; 0100 0100 1XXX X101
	; where XXX X represents Rm.
	; and 1....101 represents Rd, which is r13 in this case.
THUMB_ADD
	ldr     r4, =0x4485FF87	; set r4 to be the mask
	and	r6, r10, r4	; use lower half of r4 to look for ADD r13, Rm
	cmp     r6, r4, lsr #16	; user upper half of r4 to cmp to 0x4485
	bne	THUMB_LDR	; if not equal, go on and look for LDR instruction.
	mov     r6, r5, lsr #24	; r6 now contains r13 and Rm
	and	r6, r6, #0x78	; determine which register is Rm
	mov     r11, r6, lsr #3	; shift right by 3 to get Rm and save it in r11
	b       THUMB_LOOP	; go back to look for the PUSH instruction.


	; Thumb LDR instruction has the ecoding:
	; 0100 1RRR XXXX XXXX
	; where RRR is a register that contains number of bytes to be put
	; on the stack. And XXXX XXXX is [PC, #<immed_8> * 4], which is
	; number of bytes that are added to the PC in order to determine
	; the address, which is read into RRR.
THUMB_LDR
	cmp	r11, #0		; check if r11 contains Rm in ADD r13, Rm
	beq	THUMB_LOOP	; if r11 is not set, so no Rm. Go back and look for PUSH.
	mov     r4, #0x48	; r4 = 0100 1000; which is LDR opcode in the first 5 bits
	orr	r4, r4, r11	; r4 = 0100 1RRR, where RRR contains Rm of ADD r13, Rm
				; now we have r4 as our mask.
	mov	r6, r10, lsr #8	; r6 contains only LDR Rm portion of the instr.
	cmp	r4, r6		; compare r6 and our mask r4.
	bne	THUMB_LOOP	; not LDR Rm, go back and look for PUSH
	; at this point, we have r5 holding the immed_8, which is the offset that holds
	; the actual size of the local variable on the stack. This offset is relative
	; to the PC at this point. To figure out the PC, we need to use r3, which is our
	; "PC" running in reverse. PC = r3+offset+immed_8, where offset is 4 in Thumb mode.
	add	r4, r3, #4	; r4 = our "PC" running in reverse is 4 bytes back 
				; from the real PC.
	add	r4, r4, r5, LSR #22	; r4 = r4 + (r5 >> 22) r5 holds the immed_8 at
					; the MSB. Since it is immed_8 * 4, we shift right
					; 22 instead of 24.
	ldr	r6, [r4]	; r6 = size value, which is negative and in bytes.
	mov	r4, #0		; r4 = 0;
	sub	r6, r4, r6	; r6 = r4 - r6;
	mov	r4, r6, lsr #2	; r4 = number of words
	add	r8, r8, r4	; add number of words to running counter
	mov	r11, #0		; reset Rm holder, which is r11
	b	THUMB_LOOP	; move on and look for PUSH instruction


	; We found the PUSH instruction, and we have a bitmask in r5 with 
	; a bit set for every register saved on the stack.  
	; 
        ; The code below uses the divide-and-conquer algorithm for counting
        ; bits (population count), which sums adjacent pairs of bits, then 
        ; adjacent pair sums, and so on, to produce a population count in 
        ; register r5.  The implementation is adapted from the 
        ; "ARM System Developer's Guide", pp. 252-253, and the idea from 
        ; "Hacker's delight", chapter 5, pp 65-72.  
        ;
        ; FIXME:  I WILL HAVE TO REWRITE THE CODE BELOW BECAUSE OF LICENSING
        ;         ISSUES WITH THE "ARM SYSTEM DEVELOPER'S GUIDE".
        ;
	; NOTE: This code is common to both the ARM- and Thumb-mode cases.  The
	;       assumption is that r5 contains a bit mask for the regsiers that
        ;       are saved on the stack of the caller.  The bitmask need not be 
        ;       aligned in any way and could be between 1- and 32-bit wide.
        ;
THUMB_PUSH
	ldr     r4, =0x49249249    ; 01001001001001001001001001001001
        and     r6, r5, r4, LSL #1 ; x00x00x00x00x00x00x00x00x00x00x0
        sub     r5, r5, r6, LSR #1 ; uuzuuzuuzuuzuuzuuzuuzuuzuuzuuzuu
        and     r6, r5, r4, LSR #1 ; 00z00z00z00z00z00z00z00z00z00z00
        add     r5, r5, r6         ; vv0vv0vv0vv0vv0vv0vv0vv0vv0vv0vv
        ;
        ; triplets summed, uu=x+y, vv=x+y+z
        ;
        ldr     r4, =0xC71C71C7    ; 11000111000111000111000111000111
        add     r5, r5, r5, LSR #3 ; ww0vvwww0vvwww0vvwww0vvwww0vvwww
        and     r5, r5, r4         ; ww000www000www000www000www000www
        ;
        ; each www is the sum of six adjacent bits
        ;
        add     r5, r5, r5, LSR #6  ; sum the w instances
        add     r5, r5, r5, LSR #12
        add     r5, r5, r5, LSR #24
        and     r5, r5, #63         ; mask out irrelevant bits

        ; r5 contains the number of registers on the stack.  r7 points to the
        ; top of the stack of the caller of the function whose last frame we
        ; unwinded (in the first iteration of this loop, this would be the
        ; caller of this function).  We add the number of registers on the 
        ; stack of the caller, times 4, in order to obtain the address of
        ; the return address of the caller (which follows the arguments).  
        ; We read that return address into register r3,  and advance register
        ; r7 by another 4 bytes (skipping over the saved return address) to
        ; position it to the top of the previous frame.  
        add     r5, r5, r8		; Add any local data to the stack pointer.
	add     r7, r7, r5, lsl #2    	; Points to the return address to CALLER.


	; Now that we are at the PUSH instruction, we should be at the beginning
	; of the CALLEE function. Save r3+2+1 (thump mode) to r8.
        add     r8, r3, #0x3		; r8 contains the begining addr of CALLEE
					; since r3 is off by 2 bytes of loading the
					; the upper half.
	; load the addr of the function that acts as the reference point to stop.
	; On REX, we're using rex_task_preamble. On L4, we're using __thread_stub.
	ldr     r6, %f98		; load addr of reference point
	cmp	r8, r6                	; compare CALLEE addr against reference point
	ldmeqfd sp!, {r4-r11,pc}       	; if YES, return.


	; continue loading the return address to the CALLER into r3
	ldr     r3, [r7], #4		; advance r7 4 more bytes and
					; load the return address to the CALLER into
					; r3
        ; Check to see if the return address to the CALLER is valid.
	; If it is not, then quit.
 	stmfd   sp!, {r0-r3}          	; Save r0-r3 on the stack
        mov     r0, r3                	; Write the pointer to r0 (argument)
        blx     MALLOCMGR_SYSTEM_CHECK_TEXT_SECTION_POINTER 	; call func 
        mov     r6, r0                	; Save result in r6
        ldmfd   sp!, {r0-r3}          	; Restore r0-r3 from the stack.
        cmp     r6, #0                	; If r6 is zero, then the pointer is not
        beq     ERROR_NOTTEXT          	; in the text section, in which case we quit


	; Write the return address and advance the buffer pointer.  Then 
        ; proceed to unwind one more stack frame.
	str	r3, [r2], #0x4	; write return address to CALLER to bt_buffer
	add	r0, r0, #0x4	; Increment the buffer index.
	b	MAINLOOP	; Repeat the loop.


	; Errors detected. Negates bt_buffer_length and returns.
	; Possible errors are:  (1) ARM mode (not supported at the moment)
	;			(2) reach max of 1000 instructions
	;			(3) return addr not in TEXT section
ERROR_ARMMODE
#ifdef FEATURE_L4
#error code not present
#endif
	b	ERROR
ERROR_MAXCOUNT
	b	ERROR
ERROR_NOTTEXT
	b	ERROR			; debugging purpose
	
ERROR
	mov     r6, #0			; set r6 to 0
        sub     r0, r6, r0		; negate r0 (bt_buffer_length)
	ldmfd   sp!, {r4-r11,pc} 	; restore stackframe of caller




;1       ; ARM Mode.  In this mode, we are looking for the instruction 
        ; STMFD sp!,{..., pc}, which is the ARM-mode insruction for full 
        ; descending stacks (It is a mnemonic equivalent to STMDB).  The opcode
        ; of this instruction is 0xE92Dnnnn, where the least-significant half-
        ; word nnnn is a bitmask of the registers to push from the stack.  As
        ; with Thumb mode, in order to reach the link register on the stack of a
        ; the caller, we need to know now many registers were pushed on it, 
        ; and this is exactly what the number of set bits in nnnn tells us.
;        sub     r3, r3, #8      ; Since r3 points to the return address, r3 - 4
                                ; is the address of the branch instruction to
                                ; the subroutine in this stack frame, so r3 - 8
                                ; is the address of the instruction before that.
                                ; We start checking from that instruction 
                                ; backwards.
	
;2	ldr	r4, [r3], #-4	     ; Load r4 with the opcode at [r3] and 
;	mov	r5, r4, lsl #16      ; retreat r3 by four bytes.  Load r5 with 
;	ldr	r6, =0xFFFF4000      ; the register mask in its MS halfword and 
;	and	r4, r4, r6           ; zero out the bits in that halfword in r4.
;	ldr	r6, =0xE92D4000      ; If the resulting value is 0xE92D4000,  
;	cmp	r4, r6               ; then we've encountered the caller's   
;	beq	%b5		     ; STMFD instruction.  If not, go back and
                                     ; repeat.  We compare against 0xE92D4000
                                     ; and not 0xE92D0000 to make sure that the
                                     ; lr register was pushed on the stack with
                                     ; the other register--an additional 
                                     ; precaution.

        ; Write here for SUB sp, sp, #immed case


;	b	%b2                  ; Branch to count the bits in r5 and to
                                     ; extract the next instruction.   Note that
                                     ; bitmask in r5 does not contain the entry
                                     ; for the link register, just as the code
                                     ; handling the return-address-extraction
                                     ; expects (since the bitmask of the PUSH
                                     ; instruction in Thumb mode does not list
                                     ; the lr register).
#endif
        endp

#ifdef FEATURE_L4
#error code not present
#else
98      DCD     rex_task_preamble
#endif

        EXPORT MALLOCMGR_UNWIND_STACK

        IMPORT ||Lib$$Request$$armlib||, WEAK

#ifndef FEATURE_STACK_CHECK
        ASSERT {SWST} = {FALSE}
        ASSERT {NOSWST} = {TRUE}
#endif
        ASSERT {ENDIAN} = "little"
        ASSERT {ROPI} = {FALSE}
        ASSERT {RWPI} = {FALSE}
        ASSERT {NOT_SHL} = {TRUE}
        ASSERT {SHL1} = {FALSE}
        ASSERT {SHL2} = {FALSE}

        END

