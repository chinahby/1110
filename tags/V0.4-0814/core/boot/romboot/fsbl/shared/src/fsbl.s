;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;           F A I L  S A F E  B O O T  L O A D E R        
;
; GENERAL DESCRIPTION
;   This file contains the Fail Safe Boot Loader (FSBL) which 
;   performs the following functions:
;        
;      - Sets up the hardware with target specific functions.
;      - Set up environment before loading AMSS image.
;      - Set up environment after loading AMSS image. 
;
;   The FSBL is written to perform the above functions with optimal speed.   
;   It also attempts to minimize the execution time and reduce boot time.
;
; EXTERNALIZED SYMBOLS
;   
;   
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;   
;
; Copyright (c) 2007 by QUALCOMM, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
        
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;  $Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl.s#1 $
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 07/30/07   ANB     Initial version.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#include "boot_fsbl_if.h"

;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================

        
;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
    ; Import the external symbols that are referenced in this module.
    IMPORT fsbl_init

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================
    ; Export the external symbols that are referenced in this module.
    EXPORT fsbl_loop_here
        
;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================

;=======================================================================
;                       BOOT BLOCK DATA LOCATIONS
;
;  Locations and sizes of data areas in ROM and RAM are imported from th
;  linker and stored as data items that are used at runtime by the boot
;  kernel routines.
;=======================================================================

; Data area is in RAM and location must be defined in scatter load
; file.          
;--------------------------------------------------------------------------
; Function Dispatch table for FSBL 
; Note: These functions are the interface between DBL/OSBL and FSBL. 
;--------------------------------------------------------------------------

    PRESERVE8
    AREA    FSBL_FUNCTION_TABLE, DATA, READONLY
    DCD  fsbl_init

;======================================================================
; FSBL code.
;======================================================================
 AREA    FSBL_CODE, CODE, READONLY
 CODE32
 ENTRY
       
fsbl_loop_here
loophere
    b loophere

    END
