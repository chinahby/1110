/*=============================================================================

                  F A I L  S A F E  B o o t  L o a d e r
                         M a i n  C o n t r o l 

GENERAL DESCRIPTION
  This file contains the function interface for FSBL execution.

EXTERNALIZED FUNCTIONS
  fsbl_init
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_mc.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/30/07   ANB     Initial version.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "fsbl_mc.h"
#include "fsbl_ram_init.h"
#include "fsbl.h"
#include "fsbl_hw.h"
#include "boot_fsbl_if.h"
#include "fsbl_hash_sha1.h" 
#include "fsbl_auth_x509.h"
#include "fsbl_dload.h"
#include "fsbl_config.h"
#include "fsbl_error_handler.h"
#include "fsbl_target.h"
#include "cache_mmu.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
//#ifndef USE_DM
const int hwio_interrupts_locked_by_user = 0;
//#endif
/*---------------------------------------------------------------------------
   Type declarations.  
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                OEMSBL Initialization Function Table
---------------------------------------------------------------------------*/
static void (* const fsbl_init_func_tbl[])(void) =
{
  fsbl_ram_init,
  fsbl_hw_init,
#ifdef BOOT_USES_DCACHE
  boot_cache_mmu_init,
#endif
  NULL 
};


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION  FSBL_UPDATE_CLIENTS_INTERFACE

DESCRIPTION
  This function performs the initialization necessary for FSBL operation.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Calls error handler and never returns if the pointer passed in is NULL.

=============================================================================*/
static void fsbl_update_clients_interface
(
  boot_fsbl_if_type *fsbl_if     /* Pointer to the clients interface */
)
{
  if ( fsbl_if != NULL )
  {
    fsbl_if->hash_sha1_init      = fsbl_hash_sha1_init;
    fsbl_if->auth_x509_init      = fsbl_auth_x509_init;
    fsbl_if->dload_init          = fsbl_dload_init;
    fsbl_if->config_init         = fsbl_config_init;
    fsbl_if->get_memory_range    = fsbl_target_get_memory_range;
    fsbl_if->error_handler_init  = fsbl_error_handler_init;
  }
} /* fsbl_update_clients_interface */

/*=============================================================================

FUNCTION  FSBL_INIT

DESCRIPTION
  This function performs the initialization necessary for FSBL operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
boolean fsbl_init
(
  boot_fsbl_if_type *fsbl_if,      /* Pointer to the clients interface */
  boot_pbl_shared_data_type *pbl_shared_data /* Pointer to PBL Shared Data */
)
{
  uint32 i;

  /* Call each function in the initialization table. */
  for ( i = 0; fsbl_init_func_tbl[i] != NULL; i++ )
  {
    fsbl_init_func_tbl[i]();
  }

  (void)fsbl_target_init(pbl_shared_data);

  fsbl_update_clients_interface(fsbl_if);

  return TRUE;
 
} /* fsbl_init() */

