#ifndef DBL_CLK_H
#define DBL_CLK_H

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

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/config/clk/dbl_clk.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*=========================================================================== 
                           INCLUDE FILES
===========================================================================*/
#include "boot_comdef.h"
#include "boot_clk_speed.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION  DBL_CLK_GET_SPEED_SET_BY_PBL

DESCRIPTION
  This function will return the clock speed set by the PBL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern const boot_clk_speed_type* dbl_clk_get_speed_set_by_pbl( void );

/*===========================================================================

FUNCTION  DBL_CLK_CONFIGURE_LOW_SPEED

DESCRIPTION
 This function will configure the CLK interface.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the new clock speed structure.

SIDE EFFECTS
  None

===========================================================================*/
const boot_clk_speed_type* dbl_clk_configure
(
  const boot_clk_speed_type *shared_clk_speed
);

#endif  /* DBL_CLK_H */


