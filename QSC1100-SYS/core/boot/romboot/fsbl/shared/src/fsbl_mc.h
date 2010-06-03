#ifndef FSBL_MC_H
#define FSBL_MC_H

/*===========================================================================

                  F A I L  S A F E  B o o t  L o a d e r
                         M a i n  C o n t r o l 
                          H e a d e r  F i l e

GENERAL DESCRIPTION
  This header file contains declarations and definitions for main control
  functionality of the FSBL.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_mc.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_fsbl_if.h"
#include "boot_headers.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================

FUNCTION  FSBL_INIT

DESCRIPTION
  This function performs the initialization necessary for FSBL operation.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Calls error handler and never returns if the pointer passed in is NULL.

=============================================================================*/
boolean fsbl_init
(
  boot_fsbl_if_type *fsbl_if,      /* Pointer to the public interface */
  boot_pbl_shared_data_type *pbl_shared_data /* Pointer to PBL Shared Data */

);

#endif  /* FSBL_MC_H */

