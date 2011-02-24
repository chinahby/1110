/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_fsbl_handler.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "boot_comdef.h"
#include "dbl_fsbl_handler.h"
#include "dbl_error_handler.h"
#include "dbl_auth.h"
#include "dbl.h"
#include "dbl_mc.h"
#include "dbl_target_accessor.h"
#include "boot_clobber_prot.h"
#include "boot_util.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

static boolean                 fsbl_dload_initialized = FALSE;
static boot_fsbl_if_type       fsbl_interface;
static boot_dload_if_type      *fsbl_dload_if = NULL;
extern const byte fsbl_hash[];

/* size of hash to use for progressive boot.  Needs to match tools */
#define PROGRESSIVE_DIGEST_SIZE 20
/* Buffer to store the hash values calculated using parallel hash and load */
byte fsbl_hash_data[PROGRESSIVE_DIGEST_SIZE];

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  DBL_FSBL_INIT_INTERFACE

DESCRIPTION
  This function initializes the FSBL functionality.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dbl_fsbl_init_interface
(
  const dbl_shared_data_type *shared_data   /* Shared data         */ 
)
{
  boot_hash_if_type     *hash_if;     
  boot_auth_if_type     *auth_if;
  boot_fsbl_if_type     *fsbl_if;
  boot_fsbl_config_if_type *config_if;
  void                     *cert_ptr;
  void                     *fsbl_base_addr;
  uint32                    fsbl_size;

  fsbl_if = shared_data->fsbl_if_ptr;
  
  DBL_VERIFY((fsbl_if != NULL), (uint32)DBL_ERR_NULL_PTR);   

  /* Pass the DBL Error Handler to FSBL for use */
  fsbl_if->error_handler_init( &bl_err_if );

  /*------------------------------------------------------------------
    Get the config_if for FSBL. If we cannot set the config_if
    all the other interface will not work correctly.
  ------------------------------------------------------------------*/
  config_if = fsbl_if->config_init();
  DBL_VERIFY((config_if != NULL), (uint32)DBL_ERR_NULL_FSBL_CONFIG_IF_PTR);

  /*------------------------------------------------------------------
    1. Set the clock speed
    2. Set the root_cert_ptr. 
  ------------------------------------------------------------------*/
  config_if->vtbl->set_clock_speed( dbl_get_configured_clock_speed() );
  cert_ptr = (void *)dbl_get_root_cert_ptr();
  config_if->vtbl->set_root_cert_ptr( cert_ptr );

  /*------------------------------------------------------------------
    FSBL has been initialized now get access to all other interface.
  ------------------------------------------------------------------*/
  hash_if = fsbl_if->hash_sha1_init();
  DBL_VERIFY((hash_if != NULL), (uint32)DBL_ERR_NULL_HASH_IF_PTR);  
  
  auth_if = fsbl_if->auth_x509_init();
  DBL_VERIFY((auth_if != NULL), (uint32)DBL_ERR_NULL_AUTH_IF_PTR);  
  
  /*------------------------------------------------------------------
    Check whether the DBL running on this target can use high speed 
    DLOAD provided by FSBL.
  ------------------------------------------------------------------*/
  if (dbl_is_high_speed_dload_supported())
  {
    fsbl_dload_if = fsbl_if->dload_init();
    DBL_VERIFY((fsbl_dload_if != NULL), (uint32)DBL_ERR_NULL_DLOAD_IF_PTR);    

    fsbl_dload_initialized = TRUE;
  } 

  /*------------------------------------------------------------------
    Update DBL's Authentication routine.
  ------------------------------------------------------------------*/  
  dbl_auth_update_hash_auth_if(shared_data, hash_if, auth_if);

  /*------------------------------------------------------------------
    Setup clobber protection for FSBL memory range.
  ------------------------------------------------------------------*/  
  fsbl_if->get_memory_range( &fsbl_base_addr, &fsbl_size );
  boot_clobber_add_global_protection_region( fsbl_base_addr, fsbl_size );
  
}/* dbl_fsbl_init_interface */

/*=========================================================================
FUNCTION  DBL_FSBL_VEFIRY_HASH

DESCRIPTION
  This function will verfiy the calculated HASH and the stored HASH.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dbl_fsbl_verfiy_hash 
(
  void
)
{
  if ( bByteCompare(fsbl_hash_data, fsbl_hash, PROGRESSIVE_DIGEST_SIZE) != 0 )
  {
    DBL_ERR_FATAL((uint32)DBL_ERR_FSBL_HASH_DOES_NOT_MATCH);
  }
} /* dbl_fsbl_verfiy_hash */

/*===========================================================================

FUNCTION  DBL_FSBL_DLOAD

DESCRIPTION
  This function will call's FSBl downloader routine only if FSBL has been
  initialized otherwise it will call loop_here.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dbl_fsbl_dload
(
  void
)
{
  if (fsbl_dload_initialized == TRUE)
  {
    /*-----------------------------------------------------------------------
      Once FSBL is loaded then the handler will point to correct function.
    -----------------------------------------------------------------------*/
    DBL_VERIFY((fsbl_dload_if != NULL), (uint32)DBL_ERR_NULL_DLOAD_IF_PTR);   
    (void) fsbl_dload_if->vtbl->enter_dload();
  }

  /* Otherwise just loop here */
  dbl_loop_here();

} /* dbl_fsbl_dload */

/*===========================================================================

FUNCTION  DBL_FSBL_INIT

DESCRIPTION
  This function initializes the FSBL functionality.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
dbl_error_type dbl_fsbl_init
(
  dbl_shared_data_type *shared_data   /* Shared data         */ 
)
{
  boolean status = FALSE;
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR); 

  shared_data->dbl_state = DBL_STATE_INIT_FSBL_IMG;

  /*------------------------------------------------------------------
    Vefiry the image HASH value.... 
  ------------------------------------------------------------------*/
  dbl_auth_calc_hash(shared_data, fsbl_hash_data);
  dbl_fsbl_verfiy_hash();
  
  /*------------------------------------------------------------------
    Verify entry pointer is ok
  ------------------------------------------------------------------*/
  DBL_VERIFY(shared_data->entry_ptr_init,    (uint32)DBL_ERR_IMG_NOT_FOUND);

  /*------------------------------------------------------------------
   Store the FSBL's interface in DBL's global structure....
  ------------------------------------------------------------------*/
  shared_data->fsbl_if_ptr = &fsbl_interface;
  
  /*------------------------------------------------------------------
   Call FSBL's Init Function
  ------------------------------------------------------------------*/
  status = ((boot_fsbl_func_tbl *)(void*)(shared_data->entry_ptr))->init(shared_data->fsbl_if_ptr, shared_data->pbl_shared_data);

  /*------------------------------------------------------------------
   Verfiy the status from FSBL Image
  ------------------------------------------------------------------*/  
  if (status == FALSE)
  {
    return DBL_ERR_FAILED_TO_INIT_FSBL;
  }
  
  dbl_fsbl_init_interface(shared_data);
  
  shared_data->fsbl_init_called = TRUE;
  return DBL_ERR_NONE;

}/* dbl_fsbl_init */

