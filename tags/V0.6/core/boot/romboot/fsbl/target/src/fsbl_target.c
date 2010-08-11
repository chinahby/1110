/*=============================================================================

                  F A I L  S A F E  B o o t  L o a d e r
                              T A R G E T 

GENERAL DESCRIPTION
  Target-specific FSBL functionality.

EXTERNALIZED FUNCTIONS
  fsbl_target_init
  fsbl_target_get_memory_range
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/fsbl/fsbl_target.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/30/07   PS      Initial version.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_pbl_accessor.h"
#include "fsbl_target.h"
#include "fsbl_error_handler.h"
#include "secboot_api.h"
#include "customer.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

FUNCTION  FSBL_TARGET_PBL_ERROR_HANDLER_WRAPPER

DESCRIPTION
  Function which has interface similar to pbl_error_handler used for wrapper 
  to fsbl_error_handler. This wrapper is passed to pbl via PBL's 
  init_shared_routines.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void fsbl_target_pbl_error_handler_wrapper
(
  const char* filename_ptr,         /* File this function was called from -
                                       defined by __FILE__ */
  uint32      line,                 /* Line number this function was called
                                       from - defined by  __LINE__ */
  pbl_err_code_enum_type  err_code, /* Error code to indicate which error
                                       by toggling UART_DP_TX_DATA pin */
  uint32  err_log                   /* Error log to help debug */
)
{
  bl_err_if.error_handler(filename_ptr, line, (uint32)err_code);
}/* fsbl_target_pbl_error_handler_wrapper */

/*===========================================================================

FUNCTION  FSBL_PBL_SHA1_HASH_WRAPPER

DESCRIPTION
  Function which has interface similar to pbl_error_handler used for wrapper 
  to dbl_error_handler_interface. This wrapper is passed to pbl via PBL's 
  init_shared_routines.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void fsbl_target_pbl_sha1_hash_wrapper
(
  const uint8* buff_ptr,
  uint32 buff_size,
  uint8* digest_ptr
)
{
  secboot_SHA1_api( buff_ptr, buff_size, digest_ptr );
}

/*=============================================================================

FUNCTION  FSBL_TARGET_INIT

DESCRIPTION
  This function performs the target specific initialization necessary 
  for FSBL operation.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  
=============================================================================*/
static boolean fsbl_target_pbl_interface_init
(
  boot_pbl_shared_data_type *pbl_shared_data /* Pointer to PBL Shared Data */
)
{
  boolean ret_value;

  ret_value = boot_pbl_interface_init( pbl_shared_data );
  if( ret_value != TRUE) 
  {
      return ret_value;
  }
  ret_value = boot_pbl_init_shared_routine( fsbl_target_pbl_error_handler_wrapper,
                                            fsbl_target_pbl_sha1_hash_wrapper );

  return ret_value;

} /* fsbl_target_pbl_interface_init() */

/*=============================================================================

FUNCTION  FSBL_TARGET_INIT

DESCRIPTION
  This function performs the target specific initialization necessary 
  for FSBL operation.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  

=============================================================================*/
boolean fsbl_target_init
(
  boot_pbl_shared_data_type *pbl_shared_data /* Pointer to PBL Shared Data */
)
{
  return( fsbl_target_pbl_interface_init( pbl_shared_data ) );
} /* fsbl_target_init() */

/*===========================================================================

FUNCTION  FSBL_TARGET_GET_MEMORY_RANGE

DESCRIPTION
    Function to retreive the memory address range occupied by the FSBL.
    Stores the FSBL base address at the location pointed to by "base_addr"
    and the FSBL max size at the location poitner to by "size".

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void fsbl_target_get_memory_range( void **base_addr, uint32 *size )
{
  if ( base_addr != NULL )
  {
    *base_addr = (void*) MI_NOR_FSBL_IMG_DEST_ADDR;
  }
  if ( size != NULL )
  {
    *size =  (uint32) ( ((uint32) Image__FSBL_RAM_ZI__Base + Image__FSBL_RAM_ZI__Length) - 
                        (uint32)MI_NOR_FSBL_IMG_DEST_ADDR );
  }
} /* fsbl_target_get_memory_range() */

