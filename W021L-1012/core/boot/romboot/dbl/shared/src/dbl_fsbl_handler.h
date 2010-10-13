#ifndef DBL_FSBL_HANDLER_H
#define DBL_FSBL_HANDLER_H

/*=============================================================================

                      D e v i c e  B o o t  L o a d e r
                            H e a d e r  F i l e
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

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_fsbl_handler.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "dbl.h"
#include "dbl_error_handler.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  DBL_FSBL_INIT

DESCRIPTION
  This function initializes the FSBL functionality.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern dbl_error_type dbl_fsbl_init
(
  dbl_shared_data_type *shared_data   /* Shared data         */ 
);

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
extern void dbl_fsbl_dload
(
  void
);

#endif  /* DBL_FSBL_HANDLER_H */

