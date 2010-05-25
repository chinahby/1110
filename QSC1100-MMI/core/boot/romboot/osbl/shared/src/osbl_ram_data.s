;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;     O S  S E C O N D A R Y  B O O T   L O A D E R
;                          R A M  D A T A
;
; GENERAL DESCRIPTION
;  Get Data from qcsbl.map file. We use this data to load Code region to 
;  RAM, load RW region to RAM, and zero out ZI region in RAM.
;        
; Copyright (c) 2005      by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                       EDIT HISTORY FOR MODULE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
;  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_ram_data.s#1 $
;
; when      who     what, where, why
; --------  -----   ---------------------------------------------------------
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*        

        
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

        IMPORT |Load$$OSBL_VEC_TBL$$Base|
        IMPORT |Image$$OSBL_VEC_TBL$$Base|
        IMPORT |Image$$OSBL_VEC_TBL$$Length|
        IMPORT |Load$$OSBL_RAM_RW$$Base|
        IMPORT |Image$$OSBL_RAM_RW$$Base|
        IMPORT |Image$$OSBL_RAM_RW$$Length|
        IMPORT |Image$$OSBL_RAM_ZI$$ZI$$Base|
        IMPORT |Image$$OSBL_RAM_ZI$$ZI$$Length|

        
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

        ; Export the renamed linker symbols for use by the other boot modules.

        EXPORT Load__OSBL_VEC_TBL__Base
        EXPORT Image__OSBL_VEC_TBL__Base
        EXPORT Image__OSBL_VEC_TBL__Length
        EXPORT Load__OSBL_RAM_RW__Base
        EXPORT Image__OSBL_RAM_RW__Base
        EXPORT Image__OSBL_RAM_RW__Length
        EXPORT Image__OSBL_RAM_ZI__Base
        EXPORT Image__OSBL_RAM_ZI__Length


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

Load__OSBL_VEC_TBL__Base
        DCD |Load$$OSBL_VEC_TBL$$Base|

Image__OSBL_VEC_TBL__Base
        DCD |Image$$OSBL_VEC_TBL$$Base|

Image__OSBL_VEC_TBL__Length
        DCD |Image$$OSBL_VEC_TBL$$Length|

Load__OSBL_RAM_RW__Base
        DCD |Load$$OSBL_RAM_RW$$Base|

Image__OSBL_RAM_RW__Base
        DCD |Image$$OSBL_RAM_RW$$Base|

Image__OSBL_RAM_RW__Length
        DCD |Image$$OSBL_RAM_RW$$Length|
        
Image__OSBL_RAM_ZI__Base
        DCD |Image$$OSBL_RAM_ZI$$ZI$$Base|

Image__OSBL_RAM_ZI__Length
        DCD |Image$$OSBL_RAM_ZI$$ZI$$Length|
        END
