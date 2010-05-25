;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                                DBL RAM DATA
;
; GENERAL DESCRIPTION
;   Get Region information from dbl.map file.
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
;  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_ram_data.s#2 $
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

        IMPORT |Load$$DBL_IRAM_VEC_TBL$$Base|
        IMPORT |Image$$DBL_IRAM_VEC_TBL$$Base|
        IMPORT |Image$$DBL_IRAM_VEC_TBL$$Length|
        IMPORT |Image$$DBL_RAM_ZI$$ZI$$Base|
        IMPORT |Image$$DBL_RAM_ZI$$ZI$$Length|
        IMPORT |Load$$DBL_RAM_RW$$Base|
        IMPORT |Image$$DBL_RAM_RW$$Base|
        IMPORT |Image$$DBL_RAM_RW$$Length|
        IMPORT |Image$$DBL_RAM_NZI$$Base|
        IMPORT |Image$$DBL_RAM_NZI$$Length|
           
        
;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

        ; Export the renamed linker symbols for use by the other boot modules.
        EXPORT Load__DBL_IRAM_VEC_TBL__Base;
        EXPORT Image__DBL_IRAM_VEC_TBL__Base;
        EXPORT Image__DBL_IRAM_VEC_TBL__Length;        
        EXPORT Image__DBL_RAM_ZI__Base
        EXPORT Image__DBL_RAM_ZI__Length
        EXPORT Load__DBL_RAM_RW__Base
        EXPORT Image__DBL_RAM_RW__Base
        EXPORT Image__DBL_RAM_RW__Length
        EXPORT Image__DBL_RAM_NZI__Base
        EXPORT Image__DBL_RAM_NZI__Length        

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

Load__DBL_IRAM_VEC_TBL__Base
        DCD |Load$$DBL_IRAM_VEC_TBL$$Base|

Image__DBL_IRAM_VEC_TBL__Base
        DCD |Image$$DBL_IRAM_VEC_TBL$$Base|

Image__DBL_IRAM_VEC_TBL__Length
        DCD |Image$$DBL_IRAM_VEC_TBL$$Length|
        
Image__DBL_RAM_ZI__Base
        DCD |Image$$DBL_RAM_ZI$$ZI$$Base|

Image__DBL_RAM_ZI__Length
        DCD |Image$$DBL_RAM_ZI$$ZI$$Length|

Load__DBL_RAM_RW__Base
        DCD |Load$$DBL_RAM_RW$$Base|
        
Image__DBL_RAM_RW__Base
        DCD |Image$$DBL_RAM_RW$$Base|

Image__DBL_RAM_RW__Length
        DCD |Image$$DBL_RAM_RW$$Length|

Image__DBL_RAM_NZI__Base
        DCD |Image$$DBL_RAM_NZI$$Base|

Image__DBL_RAM_NZI__Length
        DCD |Image$$DBL_RAM_NZI$$Length|


        END


