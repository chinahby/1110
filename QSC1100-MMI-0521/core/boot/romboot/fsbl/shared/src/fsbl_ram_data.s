;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                                FSBL RAM DATA
;
; GENERAL DESCRIPTION
;   Get Region information from fsbl.map file.
;         
; Copyright (c) 2007      by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
        
;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;  $Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_ram_data.s#1 $
;
; when      who     what, where, why
; --------  -----   --------------------------------------------------- ------
; 07/30/07   ANB     Initial version.
;============================================================================

                
;============================================================================
;
;                           MODULE INCLUDE FILES
;
;============================================================================


;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================
        ; Import the linker generated symbols that correspond to the base
        ; addresses and sizes of the boot code including both DATA and CODE.

        IMPORT |Image$$FSBL_RAM_ZI$$ZI$$Base|
        IMPORT |Image$$FSBL_RAM_ZI$$ZI$$Length|
        IMPORT |Load$$FSBL_RAM_RW$$Base|
        IMPORT |Image$$FSBL_RAM_RW$$Base|
        IMPORT |Image$$FSBL_RAM_RW$$Length|        
        
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

        ; Export the renamed linker symbols for use by the other boot modules.
        EXPORT Image__FSBL_RAM_ZI__Base
        EXPORT Image__FSBL_RAM_ZI__Length
        EXPORT Load__FSBL_RAM_RW__Base
        EXPORT Image__FSBL_RAM_RW__Base
        EXPORT Image__FSBL_RAM_RW__Length        

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

Image__FSBL_RAM_ZI__Base
        DCD |Image$$FSBL_RAM_ZI$$ZI$$Base|

Image__FSBL_RAM_ZI__Length
        DCD |Image$$FSBL_RAM_ZI$$ZI$$Length|

Load__FSBL_RAM_RW__Base
        DCD |Load$$FSBL_RAM_RW$$Base|
        
Image__FSBL_RAM_RW__Base
        DCD |Image$$FSBL_RAM_RW$$Base|

Image__FSBL_RAM_RW__Length
        DCD |Image$$FSBL_RAM_RW$$Length|

        END


