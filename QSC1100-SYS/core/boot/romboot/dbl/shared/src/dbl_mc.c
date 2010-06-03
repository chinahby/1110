/*=============================================================================

                   D E V I C E    B O O T    L O A D E R 
                         M a i n  C o n t r o l 

GENERAL DESCRIPTION
  This file contains the function interface for DBL execution.

EXTERNALIZED FUNCTIONS
  dbl_main_ctl
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_mc.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/30/07   ANB     Initial version.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "dbl.h"
#include "dbl_mc.h"
#include "dbl_ram_init.h"
#include "dbl_flash_mc.h"
#include "dbl_error_handler.h"
#include "dbl_partition.h"
#include "dbl_configure.h"
#include "dbl_loader.h"
#include "dbl_auth.h"
#include "dbl_fsbl_handler.h"
#include "dbl_target_accessor.h"
#include "boot_util.h"

/*=============================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
=============================================================================*/
dbl_shared_data_type dbl_shared_data;

#pragma arm section rwdata = "NZI", zidata = "NZI"
uint32  dbl_stack_base;
#pragma arm section rwdata, zidata

/* Type define for state initialization functions */
typedef void (*init_fn_type)(dbl_shared_data_type*);

/*---------------------------------------------------------------------------
                DBL based function pointers.
---------------------------------------------------------------------------*/
static const init_fn_type dbl_init_func_tbl[] =
{
  /*------------------------------------------------------------------
   Init AUTH Routines. This function should also verify that the data 
   from PBL has not been altered.
  ------------------------------------------------------------------*/
  dbl_auth_init,
  
  /*------------------------------------------------------------------
   Update Flash drivers.
  ------------------------------------------------------------------*/
  dbl_flash_init,
  
  /*------------------------------------------------------------------
   Configure the system to run @ low speed.
      1. Configure CLK.
      2. Configure EBI2.
      3. Configure EBI1.
  ------------------------------------------------------------------*/  
  dbl_configure_hardware,

  /*------------------------------------------------------------------
   Get the Partition Table's location if the flash device supports it.
  ------------------------------------------------------------------*/
  dbl_partition_table_supported,
  
  /*------------------------------------------------------------------
   Load FSBL image.
  ------------------------------------------------------------------*/
  dbl_load_fsbl_image,

  /*------------------------------------------------------------------
   Load OSBL image.
  ------------------------------------------------------------------*/
  dbl_load_osbl_image,
  
  /*------------------------------------------------------------------
   Authenticate OSBL Image.
  ------------------------------------------------------------------*/
  dbl_auth_image,
  
  /*------------------------------------------------------------------
   Last entry in the table.
  ------------------------------------------------------------------*/
  NULL 
};

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  DBL_COPY_DATA_FOR_OSBL

DESCRIPTION
  This funcion will copy the data over for OSBL.
      
DEPENDENCIES
  None
  
RETURN VALUE
  It will never return.

SIDE EFFECTS
  None

===========================================================================*/
static void dbl_copy_data_for_osbl(void)
{
  static boot_dbl_if_shared_info_type dbl_data_for_osbl;
  static dbl_if_device_configuration_info_type configuration_data;
  static const boot_dbl_fn_tbl_type dbl_fn_tbl = 
  {
    dbl_configure_hardware_at_high_speed
  };  
  
  /*-----------------------------------------------------------------------
    Store the clock speed.
  -----------------------------------------------------------------------*/
  dbl_data_for_osbl.clk_speed                 = dbl_shared_data.clk_speed;

  /*-----------------------------------------------------------------------
    Pass the pointer that PBL passes to DBL.
  -----------------------------------------------------------------------*/
  dbl_data_for_osbl.pbl_shared_data           = dbl_shared_data.pbl_shared_data;

  /*-----------------------------------------------------------------------
    Let OSBL know where the System Partition Table is located.
  -----------------------------------------------------------------------*/
  dbl_data_for_osbl.sys_partition_pointer     = dbl_shared_data.sys_partition_pointer;
  
  /*-----------------------------------------------------------------------
    Share flash & DDR related data with OSBL.
  -----------------------------------------------------------------------*/  
  dbl_get_shared_config_data(&configuration_data);
  dbl_data_for_osbl.configuration_shared_data = &configuration_data;

  /*-----------------------------------------------------------------------
    Share the clobber protection table
  -----------------------------------------------------------------------*/  
  dbl_data_for_osbl.clobber_tbl_pointer = boot_clobber_export_global_table();
  
  /*-----------------------------------------------------------------------
    Update FSBL information in the shared structure.
  -----------------------------------------------------------------------*/
  if (dbl_shared_data.fsbl_init_called == TRUE)
  {
    dbl_data_for_osbl.fsbl_init   = TRUE;
    dbl_data_for_osbl.fsbl_if_ptr = dbl_shared_data.fsbl_if_ptr;
  }
  else
  {
    dbl_data_for_osbl.fsbl_init   = FALSE;
    dbl_data_for_osbl.fsbl_if_ptr = NULL;
  }

  /*-----------------------------------------------------------------------
    Pass the function's which DBL wants to expose to OSBL.
  -----------------------------------------------------------------------*/
  dbl_data_for_osbl.fn_tbl = &dbl_fn_tbl;
  
  /*-----------------------------------------------------------------------
    Update the Pointer which will be passed to OSBL.
  -----------------------------------------------------------------------*/
  dbl_shared_data.dbl_data_for_osbl       = &dbl_data_for_osbl;
}/* dbl_copy_data_for_osbl */

/*===========================================================================

FUNCTION  DBL_SHARED_DATA_INIT

DESCRIPTION
  This funcion will initialize DBL's shared data.
      
DEPENDENCIES
  None
  
RETURN VALUE
  It will never return.

SIDE EFFECTS
  None

===========================================================================*/
static void dbl_shared_data_init(boot_pbl_shared_data_type *pbl_shared)
{
  qmemset((uint8*) &dbl_shared_data, 0x0, sizeof(dbl_shared_data_type));
  dbl_shared_data.pbl_shared_data       = pbl_shared;
} /* dbl_init_shared_data */

/*===========================================================================

FUNCTION  DBL_INTERFACE_TO_TARGET_INIT

DESCRIPTION
  This funcion will initialize DBL's target interface
      
DEPENDENCIES
  None
  
RETURN VALUE
  It will never return.

SIDE EFFECTS
  None

===========================================================================*/
static void dbl_interface_to_target_init(void)
{
  dbl_target_accessor_init(&dbl_shared_data);
  dbl_shared_data.clk_speed = dbl_get_clock_speed_set_by_pbl();
  dbl_clobber_prot_init();
} /* dbl_init_shared_data */

/*===========================================================================

FUNCTION  DBL_MAIN_CTL

DESCRIPTION
   The DBL Main Controller performs the following functions:       
      - Copies Data passed from PBL
      - INIT's RW and ZI region
      - INIT's flash driver
      - Configures External Hardware
      - Loads FSBL image
      - Loads OSBL image
      
DEPENDENCIES
  None
  
RETURN VALUE
  It will never return.

SIDE EFFECTS
  None

===========================================================================*/
void dbl_main_ctl(boot_pbl_shared_data_type *pbl_shared)
{
  uint32 i=0;

  /*-----------------------------------------------------------------------
   Create the vector table for exception handlers.
  -----------------------------------------------------------------------*/
  dbl_create_vector_table();

  /*-----------------------------------------------------------------------
    Enable the ARM926 instruction cache; Please refer to ARM9 tech menu.
  -----------------------------------------------------------------------*/
  dbl_enable_arm9_instruction_cache();
   
  /*------------------------------------------------------------------
   Initialize the RW-ZI region according to scatter load file.
  ------------------------------------------------------------------*/
  dbl_ram_init();

  /*-----------------------------------------------------------------------
    Save data from PBL and INIT the NZI part...
  -----------------------------------------------------------------------*/
  dbl_shared_data_init(pbl_shared);

  /*-----------------------------------------------------------------------
    Initialize target interface which can be different from target to
    target.
  -----------------------------------------------------------------------*/
  dbl_interface_to_target_init();
     
  /*-----------------------------------------------------------------------
    Call each function in table.
  -----------------------------------------------------------------------*/
  for ( i=0; dbl_init_func_tbl[i] != NULL; i++ )
  {
    dbl_init_func_tbl[i](&dbl_shared_data);
  }

  /*------------------------------------------------------------------
   Copy data over for OSBL.
  ------------------------------------------------------------------*/
  dbl_copy_data_for_osbl();

  /*------------------------------------------------------------------
   Verify entry pointer is ok
  ------------------------------------------------------------------*/
  DBL_VERIFY(dbl_shared_data.entry_ptr_init,    (uint32)DBL_ERR_IMG_NOT_FOUND);
  DBL_VERIFY(dbl_shared_data.entry_ptr != NULL, (uint32)DBL_ERR_IMG_NOT_FOUND);
  
  /*------------------------------------------------------------------
   Transfer control to the OSBL
  ------------------------------------------------------------------*/
  dbl_shared_data.entry_ptr(dbl_shared_data.dbl_data_for_osbl);
  
} /* dbl_main_ctl */

