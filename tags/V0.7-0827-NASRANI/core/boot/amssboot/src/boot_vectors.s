;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                   I N T E R U P T    V E C T O R S
;
; GENERAL DESCRIPTION
;
; EXTERNALIZED FUNCTIONS
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
; Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; when      who     what, where, why
; --------  -----   ----------------------------------------------------------
; 11/30/07  ps      Adding support for SECBOOT 2.0
; 07/23/05  wd      L4 Merge.
; 12/20/04  tkuo    Removed FEATURE_NONSEC_NOR.
; 12/18/04  tkuo    Make address at 0x4c be #define in miheader.h.  
; 12/07/04  chs     Modified to make it compatible for NOR/NAND boot.
; 12/01/04  tkuo    Ported from 6250
;=======================================================================
;
;                           MODULE INCLUDE FILES
;
;=======================================================================

#include "customer.h"
#include "msm.h"
#include "miheader.h"
         
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================

        IMPORT  Reset_Handler
        IMPORT  Undefined_Handler
        IMPORT  SWI_Handler

#ifdef FEATURE_PROGRESSIVE_BOOT        
        IMPORT  Progressive_Boot_Abort_Handler
#else
        IMPORT  Prefetch_Handler
        IMPORT  Abort_Handler
#endif        
        IMPORT  Reserved_Handler
        IMPORT  RAM_IRQ_VECTOR
        IMPORT  RAM_FIQ_VECTOR
;=======================================================================
;
;                             MODULE EXPORTS
;
;=======================================================================

        EXPORT boot_info_symbol
        EXPORT boot_mob_model
        EXPORT boot_cust_mob_model
        EXPORT boot_block_ver
        EXPORT stamp_mob_model
        EXPORT stamp_md5_hash
        EXPORT chk_security_code_ptr

;============================================================================
; Setup interrupt / exception vectors
;============================================================================
; Reference:            ARM7TDMI Data Sheet
; Document Number:      ARM DDI 0029E
; Issued:               August 1995
; Section:              3.9.9
;
; ADDRESS               EXCEPTION               MODE ON ENTRY
;----------------------------------------------------------------------------
; 0x00000000            Reset                   Supervisor
;----------------------------------------------------------------------------
; 0x00000004            Undefined instruction   Undefined
;----------------------------------------------------------------------------
; 0x00000008            Software Interrupt      Supervisor
;----------------------------------------------------------------------------
; 0x0000000C            Abort (prefetch)        Abort
;----------------------------------------------------------------------------
; 0x00000010            Abort (data)            Abort
;----------------------------------------------------------------------------
; 0x00000014            Reserved                Reserved
;----------------------------------------------------------------------------
; 0x00000018            IRQ                     IRQ
;----------------------------------------------------------------------------
; 0x0000001C            FIQ                     FIQ
;============================================================================


        AREA    Int_Vect, CODE, READONLY
        CODE32                         ; 32 bit ARM instruction set.

reset_vector
    DCD     Reset_Handler
undefined_instruction_vector  
    DCD     Undefined_Handler
swi_vector
    DCD     SWI_Handler
#ifdef FEATURE_PROGRESSIVE_BOOT        
 prefetch_abort_vector
    DCD     Progressive_Boot_Abort_Handler
data_abort_vector
    DCD     Progressive_Boot_Abort_Handler
#else
prefetch_abort_vector
    DCD     Prefetch_Handler
data_abort_vector
    DCD     Abort_Handler
#endif
reserved_vector
    DCD     Reserved_Handler
irq_vector
    DCD     RAM_IRQ_VECTOR
fiq_vector
    DCD     RAM_FIQ_VECTOR

;=======================================================================
;                  BOOT BLOCK INFORMATIONAL  SEGMENT
;
;=======================================================================

        AREA    Boot_Block_Info, CODE, READONLY
        CODE32                         ; 32 bit ARM instruction set.
; This is used by PST to locate this informational block
boot_info_symbol
        DCB     "QCOM"

;=======================================================================
; The family number for this phone
; This number MUST be defined.
;=======================================================================
boot_mob_model
        DCW     CUST_MOB_MODEL

;=======================================================================
; Customer Mobile Model Number is held here.
;
; This variable is only a placeholder for a number which the customer ca
;     use to differentiate various models in the same family of phones.
;
; Important :
;    Exactly one DCW must appear in the final code at this location.
;    Otherwise, the next symbol, boot_block_ver, will move up in the mem
;    map and lead to problems.
;=======================================================================
boot_cust_mob_model
        DCW     CUST_MOB_MODEL

; This tells us what version of the informational block we have got
boot_block_ver
        ; This is the current version number
        DCW     1
        ; This is the minimum version number for backwards compatability
        DCW     1

;=======================================================================
;                       APPLICATION BOOT CHECK SEGMENT
;
;  This segment reserves 32 bytes of space for the check values written
;  after the build has completed.  They are initialized to 0xFF so value
;  can be written directly into the flash part, like the security code.
;
;  This area must be located at the top of application ROM space.
;=======================================================================

stamp_mob_model
        DCW     CUST_MOB_MODEL         ; Space for the mobile model numb

stamp_md5_hash
        DCW     &FFFF
        DCD     &FFFFFFFF              ; and the MD5 hash value which is
        DCD     &FFFFFFFF              ; as a checksum across the boot i
        DCD     &FFFFFFFF
        DCW     &FFFF

reserved
        DCW     &FFFF
        DCD     &FFFFFFFF

chk_security_code_ptr                  ; Space for the 64 bit security c
                                       ; The label is exported so the se
        DCD     &FFFFFFFF
        DCD     &FFFFFFFF              ; code can be used for access con
                                       ; of the serial link.
;=========================================================================
; This field cannot be moved since ROM code will use this field to allocate
; the address of configuration data stored in NOR.
;=========================================================================
#ifdef FEATURE_LOOPING_DATA_ABORT_HANDLER
        ALIGN   4
        
        MACRO   ;;blxx
$label  blxx    $destreg
; call a function without knowing if it is ARM or THUMB code
        ROUT
$label  tst     $destreg, #0x01 ; Test for thumb mode call.
        adrne   lr, %1+1; Use Thumb return glue code if Thumb routine
        adreq   lr, %2  ; Otherwise skip glue code
        bx      $destreg
1
        CODE16
        bx      pc      ; Glue code to return us to ARM mode
        ALIGN   4       ; Padding
        CODE32
2
        ROUT
        MEND
        
        IMPORT  lcd_debug_message
        
New_Abort_Handler 
; Without this code we handle both Arm and thumb mode aborts
;        stmfd   sp!, {r0}
;        mrs     r0, spsr
;        tst     r0, #0x20               ; Test Thumb mode bit
;        beq     Handle_ARM_Data_Abort
;        ldmfd   sp!, {r0}
;        b       Abort_Handler           ; Let the original handler
                                        ; take care of Thumb mode aborts
Handle_ARM_Data_Abort
        ldmfd   sp!, {r0}
        stmfd   sp!, {r0-r12,r14}       ; Save all non-banked registers
                                        ; plus LR
        mrs     r0, spsr
        stmfd   sp!, {r0}               ; plus SPSR

        ; Disable all interrupts
        mrs     r0, cpsr
        orr     r0, r0, #0xC0           ; Set IRQ and FIQ masked
        msr     cpsr_c, r0              ; Do it
        bl      Doggy
        adr     r0, Data_Abort_Message
        ldr     ip, =lcd_debug_message  ; Better not have a data abort!!!
        blxx    ip
        mov     r0, #0
        
loop_abort
        bl      Doggy
        cmp     r0, #0
        beq     loop_abort

        ldmfd   sp!, {r0}
        msr     spsr_fsxc, r0           ; restore SPSR
        ldmfd   sp!, {r0-r12,r14}       ; and everything else
        subs    pc, r14, #8             ; return from abort to aborting inst

Doggy
        ; Toggle the watchdog
        ldr     r2, =HWIO_ADDR(SLEEP_CTL) ;0x80000700         ; SLEEP_CTL_WB
        ldr     r1, =HWIO_FMSK(SLEEP_CTL,WATCH_DOG) ; #1
        strb    r1, [r2]
        mov     r1, #0
        strb    r1, [r2]
        bx      lr
        
Data_Abort_Message
; Message for ARM mode data aborts only
;        dcb     "  ARM Mode  "
; Message for ARM and Thumb mode data aborts
        dcb     "ARM/Thm Mode"
        dcb     " Data Abort "
        dcb     " (Looping)  "
        dcb     0
#endif
        END

