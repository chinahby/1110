;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;               A P P L I C A T I O N   B O O T    S E R V I C E S
;
; GENERAL DESCRIPTION
;
; EXTERNALIZED FUNCTIONS
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
; Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;  This section contains comments describing changes made to the module.
;  Notice that changes are listed in reverse chronological order.
;
;$Header: //depot/asic/msmshared/drivers/boot/bootapp.s#2 $
;
;when       who     what, where, why
;--------   ---     ----------------------------------------------------------
;12/01/04   tkuo    Ported from MSM6275.       
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                           MODULE INCLUDE FILES
;
;============================================================================

#include "customer.h"
#include "armasm.h"
#include "target.h"


;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================

        ; Import the linker generated symbols that correspond to the base
        ; addresses and sizes of the application code and data areas in both
        ; ROM and RAM.

        IMPORT |Load$$APP_RAM$$Base|
        IMPORT |Image$$APP_RAM$$Base|
        IMPORT |Image$$APP_RAM$$Length|
	
#ifndef FEATURE_L4
        IMPORT IRQ_Handler
        IMPORT FIQ_Handler
#endif

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

        ; Export the renamed linker symbols for use by the other boot modules.

        EXPORT Load__APP_RAM__Base
        EXPORT Image__APP_RAM__Base
        EXPORT Image__APP_RAM__Length

        ; Export handles to functions defined here.

        EXPORT RAM_IRQ_VECTOR
        EXPORT RAM_FIQ_VECTOR

;============================================================================
;                        APPLICATION INTERRUPT VECTORS
;
;  The IRQ and FIQ interrupts are exported from the boot image to the
;  application image through RAM.
;  This area must be located at the bottom of application RAM.
;============================================================================

        CODE32
        AREA    BOOTAPP_IVECT, CODE, READONLY
RAM_IRQ_VECTOR
#ifndef FEATURE_L4
        b       IRQ_Handler            ; ARM IRQ interrupt
#endif
RAM_FIQ_VECTOR
#ifndef FEATURE_L4
        b       FIQ_Handler            ; ARM FIQ interrupt
#endif

        CODE32
        AREA    APP_RESET, CODE, READONLY

;============================================================================
;                         APPLICATION DATA LOCATIONS
;
;  Locations and sizes of data areas in ROM and RAM are imported from the
;  linker and stored as data items that are used at runtime by the application
;  initialization routines.
;============================================================================

        ; The $$ convention used by the linker is replaced to avoid the need
        ; for the -pcc option required by the ARM compiler when symbols that
        ; include $$ are used in C code.

Load__APP_RAM__Base
        DCD |Load$$APP_RAM$$Base|

Image__APP_RAM__Base
        DCD |Image$$APP_RAM$$Base|

Image__APP_RAM__Length
        DCD |Image$$APP_RAM$$Length|

;==========================================================================
; The following section is for Assembler routines callable from C
;==========================================================================

        CODE16

; Place code here, or consider using inline assembly directly in your C file.

        END

