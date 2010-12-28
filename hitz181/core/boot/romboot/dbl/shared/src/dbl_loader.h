#ifndef DBL_LOADER_H
#define DBL_LOADER_H

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

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_loader.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "dbl.h"
#include "dbl_target_accessor.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  DBL_LOAD_FSBL_IMAGE

DESCRIPTION
  This function interfaces with the flash functions to load the Image for
  execution. This function:
    Loads the header
    Loads the image from location in flash to location in memory 
      specified by the DBL header.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void dbl_load_fsbl_image
(
  dbl_shared_data_type *shared_data
);

/*===========================================================================

FUNCTION  DBL_LOAD_OSBL_IMAGE

DESCRIPTION
  This function interfaces with the flash functions to load the Image for
  execution. This function:
    Loads the header
    Loads the image from location in flash to location in memory 
      specified by the DBL header.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void dbl_load_osbl_image
(
  dbl_shared_data_type *shared_data
);

#endif  /* DBL_LOADER_H */

