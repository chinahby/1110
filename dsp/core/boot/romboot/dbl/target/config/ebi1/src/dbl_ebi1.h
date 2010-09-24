#ifndef DBL_EBI1_H
#define DBL_EBI1_H

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

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/config/ebi1/dbl_ebi1.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "dbl_flash_if.h"
#include "boot_clk_speed.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/


/*===========================================================================

FUNCTION  DBL_EBI1_CONFIGURE

DESCRIPTION
 This function will configure the controller on the EBI1 interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_ebi1_configure
(
  const boot_clk_speed_type *configured_clk_speed
);

/*===========================================================================

FUNCTION  DBL_EBI1_SHARE_DATA

DESCRIPTION
 This function will save the DDR information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_ebi1_share_data
(
  dbl_if_shared_ddr_device_info_type *ddr_dev
);

#endif  /* DBL_EBI1_H */

