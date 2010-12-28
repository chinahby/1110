#ifndef FSBL_TARGET_H
#define FSBL_TARGET_H
/*=============================================================================

                  F A I L  S A F E  B o o t  L o a d e r
                                T A R G E T

GENERAL DESCRIPTION
  This file contains the target specific interface for FSBL.

EXTERNALIZED FUNCTIONS
  fsbl_target_init
  fsbl_target_get_verify_code_signature_func_ptr
  fsbl_target_get_verify_x509_chain_func_ptr
  fsbl_target_get_compute_hash_func_ptr  
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/fsbl/fsbl_target.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/30/07   PS      Initial version.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_headers.h"
#include "fsbl.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*=============================================================================
   
                            PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

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
extern void fsbl_target_get_memory_range( void **base_addr, uint32 *size );

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
extern boolean fsbl_target_init
(
  boot_pbl_shared_data_type *pbl_shared_data /* Pointer to PBL Shared Data */
);
#endif /* FSBL_TARGET_H */
