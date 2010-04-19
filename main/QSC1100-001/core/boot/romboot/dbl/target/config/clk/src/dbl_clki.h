#ifndef DBL_CLKI_H
#define DBL_CLKI_H

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

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/config/clk/dbl_clki.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*=========================================================================== 
                           INCLUDE FILES
===========================================================================*/
#include "boot_comdef.h"
#include "dbl_parser.h"
#include "dbl_clk.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

typedef struct
{
  const boot_clk_speed_type              *old_clk_speed;
  const boot_clk_speed_type              *new_clk_speed;
  const dbl_parser_cfg_data_item_type     *clk_cfg_data;
}dbl_clk_cfg_data_type;

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

FUNCTION  DBL_CLK_GET_PTR_TO_LOW_SPEED_SETTINGS

DESCRIPTION
 This function will configure the CLK interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern const dbl_clk_cfg_data_type* dbl_clk_get_ptr_to_settings
(
  const boot_clk_speed_type *shared_clk_speed
);

#endif  /* DBL_CLKI_H */

