;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                   BOOT DATA
;
; GENERAL DESCRIPTION
;   This module contains CODE, RW, ZI region linker symbol defintions.
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
; Copyright (c) 2004-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;============================================================================
;
;                           MODULE INCLUDE FILES
;
;============================================================================

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;
; when      who     what, where, why
; --------  -----   ----------------------------------------------------------
;04/09/09   dk      Added memset kind of functionality for optimised
;                   ZI init timings.   
;04/24/07   amw     Added additional symbols to resolve lint warnings.
;07/23/05   wd      L4 Merge.
;12/01/04   chs     Modified to make it compatible for NOR/NAND boot.
;10/22/04   tkuo    Ported from MSM6250.
;
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================

#include "customer.h"
#include "armasm.h"

        ; Import the linker generated symbols that correspond to the base
        ; addresses and sizes of the boot code data areas in both ROM and RAM.
#ifndef FEATURE_L4
        IMPORT |Load$$MODEM_DATA_1$$Base|
        IMPORT |Image$$MODEM_DATA_1$$Base|
        IMPORT |Image$$MODEM_DATA_1$$Length|

        IMPORT |Load$$MODEM_DATA_2$$Base|
        IMPORT |Image$$MODEM_DATA_2$$Base|
        IMPORT |Image$$MODEM_DATA_2$$Length|


        IMPORT |Load$$MAIN_APP_DATA_1$$Base|
        IMPORT |Image$$MAIN_APP_DATA_1$$Base|
        IMPORT |Image$$MAIN_APP_DATA_1$$Length|

        IMPORT |Load$$BB_RAM$$Base|
        IMPORT |Image$$BB_RAM$$Base|
        IMPORT |Image$$BB_RAM$$Length|
        IMPORT |Image$$UNINIT_RAM$$Base|
        IMPORT |Image$$UNINIT_RAM$$Length|
        IMPORT |Image$$UNINIT_RAM$$ZI$$Base|
        IMPORT |Image$$UNINIT_RAM$$ZI$$Length|
        IMPORT |Image$$ZI_REGION$$Base|
        IMPORT |Image$$ZI_REGION$$ZI$$Base|
        IMPORT |Image$$ZI_REGION$$ZI$$Length|

        IMPORT |Load$$AMSS_IMEM_VEC_TBL$$Base|
        IMPORT |Image$$AMSS_IMEM_VEC_TBL$$Base|
        IMPORT |Image$$AMSS_IMEM_VEC_TBL$$Length|
#endif
#if !defined(FEATURE_L4) || defined(FEATURE_L4_MSMSLEEP)
        IMPORT |Image$$INTERNAL_BOOT_RAM_1$$Base|
        IMPORT |Image$$INTERNAL_BOOT_RAM_1$$Length|
        IMPORT |Load$$INTERNAL_BOOT_RAM_1$$Base|

#endif
        IMPORT |Image$$INTERNAL_BOOT_RAM_2$$Base|
        IMPORT |Image$$INTERNAL_BOOT_RAM_2$$Length|
        IMPORT |Load$$INTERNAL_BOOT_RAM_2$$Base|

        IMPORT |Image$$MAIN_APP_1$$Base|
        IMPORT |Image$$MAIN_APP_1$$Length|
        IMPORT |Image$$MAIN_APP_2$$Base|
        IMPORT |Image$$MAIN_APP_2$$Length|
        IMPORT |Image$$MAIN_APP_3$$Base|
        IMPORT |Image$$MAIN_APP_3$$Length|
        IMPORT |Image$$MAIN_APP_4$$Base|
        IMPORT |Image$$MAIN_APP_4$$Length|

#ifdef FEATURE_SHARED_SEGMENT
        IMPORT |Image$$SHARED_RAM$$Base|
        IMPORT |Image$$SHARED_RAM$$ZI$$Length|
#endif
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

        ; Export the renamed linker symbols for use by the other boot modules.
#ifndef FEATURE_L4
        EXPORT Load__MODEM_DATA_1__Base
        EXPORT Image__MODEM_DATA_1__Base
        EXPORT Image__MODEM_DATA_1__Length
        EXPORT Load__MODEM_DATA_2__Base
        EXPORT Image__MODEM_DATA_2__Base
        EXPORT Image__MODEM_DATA_2__Length
        EXPORT Load__MAIN_APP_DATA_1__Base
        EXPORT Image__MAIN_APP_DATA_1__Base
        EXPORT Image__MAIN_APP_DATA_1__Length

        EXPORT Load__BB_RAM__Base
        EXPORT Image__BB_RAM__Base
        EXPORT Image__BB_RAM__Length
        EXPORT Image__UNINIT_RAM__Base
        EXPORT Image__UNINIT_RAM__Length
        EXPORT Image__UNINIT_RAM__ZI__Base
        EXPORT Image__UNINIT_RAM__ZI__Length
        EXPORT Image__ZI_REGION__Base
        EXPORT Image__ZI_REGION__ZI__Base
        EXPORT Image__ZI_REGION__ZI__Length

        EXPORT Load__AMSS_IMEM_VEC_TBL__Base;
        EXPORT Image__AMSS_IMEM_VEC_TBL__Base;
        EXPORT Image__AMSS_IMEM_VEC_TBL__Length;        

#endif
#if !defined(FEATURE_L4) || defined(FEATURE_L4_MSMSLEEP)
        EXPORT Image__INTERNAL_BOOT_RAM_1__Base
        EXPORT Image__INTERNAL_BOOT_RAM_1__Length
        EXPORT Load__INTERNAL_BOOT_RAM_1__Base

#endif
        EXPORT Image__INTERNAL_BOOT_RAM_2__Base
        EXPORT Image__INTERNAL_BOOT_RAM_2__Length
        EXPORT Load__INTERNAL_BOOT_RAM_2__Base

        EXPORT Image__MAIN_APP_1__Base
        EXPORT Image__MAIN_APP_1__Length
        EXPORT Image__MAIN_APP_2__Base
        EXPORT Image__MAIN_APP_2__Length
        EXPORT Image__MAIN_APP_3__Base
        EXPORT Image__MAIN_APP_3__Length
        EXPORT Image__MAIN_APP_4__Base
        EXPORT Image__MAIN_APP_4__Length


#ifdef FEATURE_SHARED_SEGMENT
        EXPORT Image__SHARED_RAM__Base
        EXPORT Image__SHARED_RAM__Length
#endif

        EXPORT  boot_zi_init  
;============================================================================
;                       BOOT BLOCK DATA LOCATIONS
;
;  Locations and sizes of data areas in ROM and RAM are imported from the
;  linker and stored as data items that are used at runtime by the boot
;  kernel routines.
;============================================================================

        AREA    BOOTSYS_DATA, DATA, READONLY

        ; The $$ convention used by the linker is replaced to avoid the need
        ; for the -pcc option required by the ARM compiler when symbols that
        ; include $$ are used in 'C' code.

#ifndef FEATURE_L4
Load__BB_RAM__Base
        DCD |Load$$BB_RAM$$Base|

Image__BB_RAM__Base
        DCD |Image$$BB_RAM$$Base|

Image__BB_RAM__Length
        DCD |Image$$BB_RAM$$Length|

Load__MODEM_DATA_1__Base
        DCD |Load$$MODEM_DATA_1$$Base|

Image__MODEM_DATA_1__Base
        DCD |Image$$MODEM_DATA_1$$Base|

Image__MODEM_DATA_1__Length
        DCD |Image$$MODEM_DATA_1$$Length|

Load__MODEM_DATA_2__Base
        DCD |Load$$MODEM_DATA_2$$Base|

Image__MODEM_DATA_2__Base
        DCD |Image$$MODEM_DATA_2$$Base|

Image__MODEM_DATA_2__Length
        DCD |Image$$MODEM_DATA_2$$Length|

Load__MAIN_APP_DATA_1__Base
        DCD |Load$$MAIN_APP_DATA_1$$Base|

Image__MAIN_APP_DATA_1__Base
        DCD |Image$$MAIN_APP_DATA_1$$Base|

Image__MAIN_APP_DATA_1__Length
        DCD |Image$$MAIN_APP_DATA_1$$Length|


Image__UNINIT_RAM__Base
        DCD |Image$$UNINIT_RAM$$Base|

Image__UNINIT_RAM__Length
        DCD |Image$$UNINIT_RAM$$Length|

Image__UNINIT_RAM__ZI__Base
        DCD |Image$$UNINIT_RAM$$ZI$$Base|

Image__UNINIT_RAM__ZI__Length
        DCD |Image$$UNINIT_RAM$$ZI$$Length|

Image__ZI_REGION__Base
        DCD |Image$$ZI_REGION$$Base|

Image__ZI_REGION__ZI__Base
        DCD |Image$$ZI_REGION$$ZI$$Base|

Image__ZI_REGION__ZI__Length
        DCD |Image$$ZI_REGION$$ZI$$Length|

Load__AMSS_IMEM_VEC_TBL__Base
        DCD |Load$$AMSS_IMEM_VEC_TBL$$Base|

Image__AMSS_IMEM_VEC_TBL__Base
        DCD |Image$$AMSS_IMEM_VEC_TBL$$Base|

Image__AMSS_IMEM_VEC_TBL__Length
        DCD |Image$$AMSS_IMEM_VEC_TBL$$Length|

        
#endif
#if !defined(FEATURE_L4) || defined(FEATURE_L4_MSMSLEEP)
Image__INTERNAL_BOOT_RAM_1__Base
        DCD |Image$$INTERNAL_BOOT_RAM_1$$Base|

Image__INTERNAL_BOOT_RAM_1__Length
        DCD |Image$$INTERNAL_BOOT_RAM_1$$Length|

Load__INTERNAL_BOOT_RAM_1__Base
        DCD |Load$$INTERNAL_BOOT_RAM_1$$Base|
#endif
Image__INTERNAL_BOOT_RAM_2__Base
        DCD |Image$$INTERNAL_BOOT_RAM_2$$Base|

Image__INTERNAL_BOOT_RAM_2__Length
        DCD |Image$$INTERNAL_BOOT_RAM_2$$Length|

Load__INTERNAL_BOOT_RAM_2__Base
        DCD |Load$$INTERNAL_BOOT_RAM_2$$Base|


Image__MAIN_APP_1__Base
        DCD |Image$$MAIN_APP_1$$Base|
Image__MAIN_APP_1__Length
        DCD |Image$$MAIN_APP_1$$Length|
Image__MAIN_APP_2__Base
        DCD |Image$$MAIN_APP_2$$Base|
Image__MAIN_APP_2__Length
        DCD |Image$$MAIN_APP_2$$Length|
Image__MAIN_APP_3__Base
        DCD |Image$$MAIN_APP_3$$Base|
Image__MAIN_APP_3__Length
        DCD |Image$$MAIN_APP_3$$Length|
Image__MAIN_APP_4__Base
        DCD |Image$$MAIN_APP_4$$Base|
Image__MAIN_APP_4__Length
        DCD |Image$$MAIN_APP_4$$Length|


#ifdef FEATURE_SHARED_SEGMENT
Image__SHARED_RAM__Base
        DCD |Image$$SHARED_RAM$$Base|
Image__SHARED_RAM__Length
        DCD |Image$$SHARED_RAM$$ZI$$Length|
#endif
;============================================================================
;                      BOOT BLOCK COPYRIGHT SEGMENT
;
;  This segment contains the copyright notice which is contained in the
;  Boot Block.
;============================================================================
        AREA    BOOTSYS_COPYRIGHT, DATA, READONLY

        DCB     "Copyright (c) 2004,2005,2006,2007,2009"
        DCB     "by QUALCOMM, Incorporated."
        DCB     "All rights reserved."

;============================================================================

        AREA    BOOT_MISC, CODE, READONLY
        CODE32                               ; 32 bit ARM instruction set
;============================================================================

;======================================================================
; We perform our zi init in doing assembly.
; syntax is r0 contains the destination address
; r1 is the size to be copied.
;======================================================================
boot_zi_init

; First save all registers
   stmfd    sp!,  {r0-r9,lr}   
   mov      r8,  r0 ;r0 is destination
   mov      r9,  r1

;Initialise the registers to zero.
   mov r0,#0x0
   mov r1,r0
   mov r2,r0
   mov r3,r0
   mov r4,r0
   mov r5,r0
   mov r6,r0
   mov r7,r0  
 
   cmp      r9,  #0x20; check bigger than 32 bytes or not.
   bge      loop_for_32byte_copy

check_left_over
   cmp      r9,#0      ; check if any left over bytes.
   bgt      loop_for_byte_copy

boot_zi_init_end
   ldmfd    sp!,  {r0-r9,pc}; return

loop_for_32byte_copy
   stmia r8!,  {r0-r7}   ; store  32 bytes each time
   sub       r9,r9,#0x20
   cmp      r9,#0x20 
   bge      loop_for_32byte_copy
   blt      check_left_over    ; check whether there is left over bytes.

loop_for_byte_copy
   strb      r0,[r8],#1
   sub      r9,r9,#1;
   cmp      r9,#0x0 
   bgt      loop_for_byte_copy
   ble      boot_zi_init_end  ; end the byte copy.

;============================================================================

        END

;============================================================================

