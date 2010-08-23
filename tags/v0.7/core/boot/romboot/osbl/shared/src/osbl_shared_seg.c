
/*===========================================================================

                        Operating System Boot Loader
                         S h a r e d  S e g m e n t

DESCRIPTION
  This source file contains declarations and definitions for OSBL
  shared segment support.

EXTERNALIZED FUNCTIONS
  osbl_init_shared_segment
  osbl_save_shared_segment_info

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_shared_seg.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/07   ly      Create
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "osbl_target.h"
#include "osbl_msm.h"
#include "osbl_error_handler.h"
#include "osbl_shared_seg.h"
#include "boot_sec_elf_loader_if.h"
#include "boot_pbl_accessor.h"

#ifdef FEATURE_SHARED_SEGMENT
  #include "boot_shared_seg.h"
  #include "bio.h"
  #include "boot_hw_reset.h"
  #include "boot_shared_sw_id.h"
  #include "boot_shared_nor_partition_table.h"
#ifdef FEATURE_PBOOT_BLOCK_SHARED_SEG
  #include "boot_shared_progressive_boot_block.h"
#endif
#endif /* FEATURE_SHARED_SEGMENT */

/*============================================================================
   External symbol declarations.
=============================================================================*/
#if defined(FEATURE_PMIC_WDOG) && defined(FEATURE_SHARED_SEGMENT)
extern void boot_pm_save_power_on_status(void);
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FEATURE_SHARED_SEGMENT
/*=========================================================================

FUNCTION  OSBL_ELF_SHARED_SEGMENT_FILTER

DESCRIPTION
  This is an elf segment filter for shared segments.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the segment flags indicate the segment is shared, otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean osbl_elf_shared_segment_filter
(
  uint32 type,   /* Segment type field in program header */
  uint32 flags   /* Segment flags field in program header */
)
{
  return ( MI_PBT_ACCESS_TYPE_VALUE(flags) == MI_PBT_SHARED_SEGMENT );
  
} /* osbl_elf_shared_segment_filter() */

/*===========================================================================

FUNCTION  OSBL_INIT_SHARED_SEGMENT

DESCRIPTION
 This function will initialize the shared segment

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS

===========================================================================*/
void osbl_init_shared_segment( bl_shared_data_type *bl_shared_data )
{
  uint32 num_segment;
  boolean shared_found = FALSE;

  OSBL_VERIFY( ( bl_shared_data != NULL &&
                 bl_shared_data->amss_elf_if != NULL ),
               BL_ERR_NULL_PTR_PASSED );

  num_segment = ELF_FIND_SEGMENT_START;
  while ( ( num_segment = 
              boot_sec_elf_find_next_filtered_segment( bl_shared_data->amss_elf_if,
                                                       osbl_elf_shared_segment_filter,
                                                       num_segment ) ) !=
          ELF_FIND_SEGMENT_NOT_FOUND )
  {
    uint32 mem_size;
    void *vaddr;
    if( shared_found )
    {
      /* Current design only allows for one shared segment */
      OSBL_ERR_FATAL(BL_ERR_OSBL);
    }

    shared_found = TRUE;

    vaddr = boot_sec_elf_get_segment_vaddr( bl_shared_data->amss_elf_if,
                                             num_segment );
    mem_size = boot_sec_elf_get_segment_memory_size( bl_shared_data->amss_elf_if,
                                                     num_segment );
    boot_shared_seg_osbl_init( vaddr, mem_size );
  }

  OSBL_VERIFY( shared_found, BL_ERR_OSBL );

}/* osbl_init_shared_segment() */

/*===========================================================================

FUNCTION  OSBL_SAVE_SHARED_SEGMENT_INFO

DESCRIPTION
 This function call routines to save shared info before jumping to AMSS

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void osbl_save_shared_segment_info( bl_shared_data_type *bl_shared_data )
{
  OSBL_VERIFY( ( bl_shared_data != NULL &&
                 bl_shared_data->amss_elf_if != NULL ),
               BL_ERR_NULL_PTR_PASSED );

  /*-------------------------------------------------------------------------
   Enable HW Reset detection.
  --------------------------------------------------------------------------*/
  osbl_hw_reset_shared_info_init(); 

  /*-------------------------------------------------------------------------
   Save bio settings to shared segment for AMSS
  --------------------------------------------------------------------------*/
  osbl_save_bio_shared_info();     

  /*-------------------------------------------------------------------------
   Save OSBL and AMSS SW_ID in Shared seg 
  --------------------------------------------------------------------------*/
  osbl_save_sw_id_shared_info();        

  /*-------------------------------------------------------------------------
   Save the location of NOR Partition Table for AMSS. 
  --------------------------------------------------------------------------*/
  if ( boot_pbl_get_flash_type() == NOR_FLASH ) 
  {
      boot_nor_partition_table_osbl_init(); 
  }

#ifdef FEATURE_PMIC_WDOG 
  /*-------------------------------------------------------------------------
    Save power on status
  --------------------------------------------------------------------------*/
  boot_pm_save_power_on_status();
#endif /* FEATURE_PMIC_WDOG */

#ifdef FEATURE_PBOOT_BLOCK_SHARED_SEG
  /*------------------------------------------------------------------------- 
     Initialize shared segment for progressive boot info block 
  --------------------------------------------------------------------------*/
  boot_pboot_block_osbl_init( boot_sec_elf_loader_cast_parent_if( bl_shared_data->amss_elf_if ) );
#endif /* FEATURE_PBOOT_BLOCK_SHARED_SEG */

} /* osbl_save_shared_segment_info() */
#endif /* FEATURE_SHARED_SEGMENT */

