;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;             ARM V5 architecture assembly language MMU support
;
; GENERAL DESCRIPTION
;
; EXTERNALIZED FUNCTIONS
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;
; Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;  This section contains comments describing changes made to the
;  Notice that changes are listed in reverse chronological order.
;
;
;when       who     what, where, why
;--------   ---     ----------------------------------------------------------
; 07/23/05  wd      L4 Merge.
; 01/29/05  chs     Ported from latest MSM6250 build.
; 10/01/04  tkuo    Ported from MSM6250.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;
;============================================================================
;
;                           MODULE INCLUDE FILES
;
#include "customer.h"

   

        AREA    ARM_MMU, DATA, READONLY
        CODE32                         ; 32 bit ARM instruction set.
                                                                        
      
;============================================================================

#if !defined (BUILD_BOOT_CHAIN)
;============================================================================

#if !defined (FEATURE_L4_DEMAND_LOAD_BOOT)
   AREA  APPS_CODE_SEGMENT_START, DATA, READONLY, ALIGN = 12
mmu_4k_pad_apps_code
   % 4096   
#endif
   
   AREA  APPS_DATA_SEGMENT_START, DATA, READONLY, ALIGN = 12
mmu_4k_pad_apps_data
   % 4096 

   AREA  MODEM_CODE_SEGMENT_START, DATA, READONLY, ALIGN = 12
mmu_4k_pad_modem_code
   % 4096    
   
   AREA  MODEM_DATA_SEGMENT_START, DATA, READONLY, ALIGN = 12
mmu_4k_pad_modem_data
   % 4096  
          
   AREA  HASH_SEGMENT_START, DATA, READONLY, ALIGN = 12
mmu_4k_pad_hash
   % 4096
          
   AREA  ZI_SEGMENT_START, DATA, READONLY, ALIGN = 12
mmu_4k_pad_zi_data
   % 4096    
        
   AREA IRAM_SEGMENT_PAD1, DATA, READONLY, ALIGN = 12
mmu_4k_pad_iram_code1
   % 4096  
   
   AREA IRAM_SEGMENT_PAD2, DATA, READONLY, ALIGN = 12
mmu_4k_pad_iram_code2
   % 4096  

   AREA UNUSED_SEGMENT_START, NOINIT, ALIGN = 12
mmu_4k_pad_unused_data
   % 4096   

   AREA UNUSED_SEGMENT_DATA, DATA, READONLY, ALIGN = 12
mmu_4k_pad_unused_data2
   % 4096         
      
   AREA UNUSED_SEGMENT_CODE, NOINIT, ALIGN = 12
mmu_4k_pad_unused_data3
   % 4096

   AREA UNUSED_SEGMENT_DATA2, NOINIT, ALIGN = 12
mmu_4k_pad_unused_data4
   % 4096

   AREA DUMMY_PAD_1, NOINIT
dummy_4_bytes_data_1
   % 4   

  AREA DUMMY_PAD_2, NOINIT
dummy_4_bytes_data_2
   % 4   

#if defined (FEATURE_L4_DEMAND_LOAD_BOOT)
#error code not present
#endif

#endif /* BUILD_BOOT_CHAIN */  
        END
