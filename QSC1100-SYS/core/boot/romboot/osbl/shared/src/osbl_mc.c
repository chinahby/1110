/*=============================================================================

               OS  S e c o n d a r y   B o o t   L o a d e r
                         M a i n  C o n t r o l

GENERAL DESCRIPTION
  This file contains the main control for OSBL execution.

EXTERNALIZED FUNCTIONS
  osbl_main_ctl
  osbl_do_procedures

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_mc.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/09/07   ly      Created
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <string.h>
#include "osbl.h"
#include "osbl_mc.h"
#include "osbl_target.h"
#include "osbl_mc_target.h"
#include "osbl_error_handler.h"
#include "boot_flash_trans_if.h"
#include "boot_clobber_prot.h"
#include "boot_pbl_accessor.h"
#include "boot_clk_speed.h"
#include "osbl_ram_init.h"
#include "cache_mmu.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*---------------------------------------------------------------------------
   Global variable declarations  
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  OSBL SHARED DATA STRUCTURE
---------------------------------------------------------------------------*/
bl_shared_data_type bl_shared_data = 
{
  /* CLK Bus Speed                                          */
  0,

  /* Data shared from the DBL and FSBL (deep copied)         */
  NULL,

  /* Data shared by boot devices (flash modules)             */
  NULL,

  /* Pointer to the AMSS elf loader private data             */
  NULL,

  /* Entry pointer for AMSS (will branch here when exiting)  */
  NULL,

  /* Target-specific shared data structure, if any */
  NULL
};


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  OSBL_RETRIEVE_SHARED_INFO_FROM_DBL

DESCRIPTION
  This function retreieves shared data from the DBL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void osbl_retrieve_shared_info_from_dbl
(
  bl_shared_data_type *bl_shared_data,
  boot_dbl_if_shared_info_type *dbl_shared_info_ptr
)
{
  static boot_dbl_if_shared_info_type osbl_dbl_shared_data;
  static boot_fsbl_if_type osbl_fsbl_if;
  static boot_pbl_shared_data_type osbl_pbl_shared_data;
  static boot_clk_speed_type osbl_clk_speed;

  OSBL_VERIFY( ( dbl_shared_info_ptr != NULL ) &&
               ( dbl_shared_info_ptr->fsbl_if_ptr != NULL ) &&
               ( dbl_shared_info_ptr->pbl_shared_data != NULL ) &&
               ( dbl_shared_info_ptr->clk_speed != NULL ) &&
               ( dbl_shared_info_ptr->clobber_tbl_pointer != NULL ),
               BL_ERR_NULL_PTR_PASSED );

  /* Begin deep copy of DBL shared data to OSBL address space.  This
   * allows DBL memory to be reused or disabled without affecting OSBL 
   * functionality */

  /* Copy the DBL shared data structure to OSBL address space. */
  memcpy( &osbl_dbl_shared_data,
          dbl_shared_info_ptr,
          sizeof(osbl_dbl_shared_data) );

  /* Copy the FSBL interface to OSBL address space since it actually
   * resides within the DBL address space. */
  memcpy( &osbl_fsbl_if,
          dbl_shared_info_ptr->fsbl_if_ptr,
          sizeof(osbl_fsbl_if) );

  /* Copy the PBL shared data structure to OSBL address space. */
  memcpy( &osbl_pbl_shared_data, 
          dbl_shared_info_ptr->pbl_shared_data,
          sizeof(osbl_pbl_shared_data) );

  /* Export our copy of the DBL shared data structure */
  bl_shared_data->dbl_shared_data = &osbl_dbl_shared_data;

  /* Override the FSBL interface structure with our copy */
  osbl_dbl_shared_data.fsbl_if_ptr = &osbl_fsbl_if;

  /* Override the PBL shared data structure with our copy */
  osbl_dbl_shared_data.pbl_shared_data = &osbl_pbl_shared_data;

  /* Intialize the PBL accessor routines */
  boot_pbl_interface_init( &osbl_pbl_shared_data );

  /* Copy clock speed structure */
  memcpy( &osbl_clk_speed,
          osbl_dbl_shared_data.clk_speed,
          sizeof(osbl_clk_speed) );
  bl_shared_data->clk_speed = &osbl_clk_speed;

  /* Import the DBL clobber table structure */
  boot_clobber_import_global_table( osbl_dbl_shared_data.clobber_tbl_pointer );
  osbl_dbl_shared_data.clobber_tbl_pointer = NULL;
  
  /* Assign NULL to sub-structures that we don't copy now. Target-specific
   * code can copy these if they need them. */
  osbl_dbl_shared_data.configuration_shared_data = NULL;
  
  osbl_target_retrieve_shared_info_from_dbl( bl_shared_data, 
                                             dbl_shared_info_ptr );

} /* osbl_retrieve_shared_info_from_dbl() */

/*===========================================================================

FUNCTION  OSBL_INIT_FSBL_MODULES

DESCRIPTION
  Initialize modules exported by the FSBL.  These can be overridden
  by the target code if desired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void osbl_init_fsbl_modules( bl_shared_data_type *bl_shared_data )
{
  OSBL_VERIFY( ( bl_shared_data != NULL &&
                 bl_shared_data->dbl_shared_data != NULL ),
               BL_ERR_NULL_PTR_PASSED );

  /* Initialize FSBL hash and authentication modules */
  if ( bl_shared_data->dbl_shared_data->fsbl_init ) 
  {
    /* Pass OSBL error handler to FSBL for use */
    bl_shared_data->dbl_shared_data->fsbl_if_ptr->error_handler_init( &bl_err_if );

    /* Manually register the FSBL downloader module */
    boot_dload_if_register_singleton(
      bl_shared_data->dbl_shared_data->fsbl_if_ptr->dload_init() );

    /* Manually register the FSBL hashing module */
    boot_hash_if_register_singleton(
      bl_shared_data->dbl_shared_data->fsbl_if_ptr->hash_sha1_init() );

    /* Manually register the FSBL authentication module */
    boot_auth_if_register_singleton(
      bl_shared_data->dbl_shared_data->fsbl_if_ptr->auth_x509_init() );

    /* Manually register the FSBL config module */
    boot_fsbl_config_if_register_singleton(
      bl_shared_data->dbl_shared_data->fsbl_if_ptr->config_init() );
  }
} /* osbl_init_fsbl_modules() */


/*===========================================================================

FUNCTION  OSBL_DO_PROCEDURES

DESCRIPTION
   Processes an array of OSBL procedures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_do_procedures
(
  bl_shared_data_type *bl_shared_data,
  osbl_procedure_func_type *procs
)
{
  osbl_procedure_func_type *cur_proc;

  OSBL_VERIFY( bl_shared_data != NULL && procs != NULL, BL_ERR_NULL_PTR );

  for ( cur_proc = procs; *cur_proc != NULL; cur_proc++ )
  {
    (*cur_proc)( bl_shared_data );
  }
} /* osbl_do_procedures() */

/*===========================================================================

FUNCTION  OSBL_MAIN_CTL

DESCRIPTION
   The OSBL Main Controller
        
DEPENDENCIES
  None
  
RETURN VALUE
  It will never return.

SIDE EFFECTS
  None

===========================================================================*/
void osbl_main_ctl
(
  boot_dbl_if_shared_info_type *dbl_shared_info_ptr
)
{
  /*-----------------------------------------------------------------------
   * Create the vector table for exception handlers.
   *-----------------------------------------------------------------------*/
  osbl_create_vector_table();

  /*-----------------------------------------------------------------------
   * Enable the instruction cache.
   *-----------------------------------------------------------------------*/
  mmu_enable_instruction_cache();

  /*-----------------------------------------------------------------------
   * Initialize memory according to scatter load file.
   *-----------------------------------------------------------------------*/
  osbl_ram_init();

  /* Retrieve info passed from DBL/PBL and put in bl_shared_data structure */
  osbl_retrieve_shared_info_from_dbl( &bl_shared_data, dbl_shared_info_ptr );

  /* Initialize the FSBL modules */
  osbl_init_fsbl_modules( &bl_shared_data );

  /* Update the FSBL clock speed structure to use the OSBL copy */
  boot_fsbl_config_set_clock_speed( bl_shared_data.clk_speed );

  /* Add the OSBL address range to the memory protection */
  boot_clobber_add_global_protection_region( (void*) OSBL_CODE_BASE,
                                             OSBL_MAX_IMAGE_SIZE );

  /*-----------------------------------------------------------------------
   * Process the target-dependent OSBL procedures
   *-----------------------------------------------------------------------*/
  osbl_do_procedures( &bl_shared_data,
                      osbl_main_procs );

  /* Post-condition for OSBL procedures */
  OSBL_VERIFY( bl_shared_data.amss_entry_ptr != NULL, BL_ERR_NULL_PTR );

  /* Disable caches and MMU before jumping to AMSS. */
  cache_mmu_disable();

  /*------------------------------------------------------------------------
   * Transfer control to the AMSS. 
   *-----------------------------------------------------------------------*/
  bl_shared_data.amss_entry_ptr();

} /* osbl_main_ctl() */


