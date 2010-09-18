#ifndef DBL_EBI2_H
#define DBL_EBI2_H

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

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/config/ebi2/dbl_ebi2.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*=========================================================================== 
                           INCLUDE FILES
===========================================================================*/

#include "boot_clk_speed.h"
/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/


/*===========================================================================

FUNCTION  DBL_EBI2_CONFIGURE

DESCRIPTION
 This function will configure the controller on the EBI2 interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_ebi2_configure
(
  const boot_clk_speed_type *configured_clk_speed 
);

#endif  /* DBL_EBI2_H */


