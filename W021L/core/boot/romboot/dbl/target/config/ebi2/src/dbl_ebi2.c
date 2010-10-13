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

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/config/ebi2/dbl_ebi2.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================
                            INCLUDE FILES FOR MODULE
=============================================================================*/

#include "dbl_ebi2.h"
#include "dbl_ebi2i.h"
#include "dbl_error_handler.h"
#include "boot_pbl_accessor.h"
#include "dbl_parser.h"


/*=============================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
=============================================================================*/

const static dbl_parser_cfg_data_item_type ebi2_cfg_data_24MHZ[] =
{
  {HWIO_OPERATION,   HWIO_ADDR(LCD_CFG0),                0x04120105  },
  {HWIO_OPERATION,   HWIO_ADDR(LCD_CFG1),                0xC9000000  },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI2_CSn_CFG0, 0),       0x12110109  },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI2_CSn_CFG1, 0),       0x92000000  },
  {HWIO_OPERATION,   HWIO_ADDR(EBI2_SWBM_AHB_PRIORITY),  0x0024      },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000   }
};

const static dbl_parser_cfg_data_item_type ebi2_cfg_data_32MHZ[] =
{
  {HWIO_OPERATION,   HWIO_ADDR(LCD_CFG0),                0x06220107  },
  {HWIO_OPERATION,   HWIO_ADDR(LCD_CFG1),                0xC9000000  },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI2_CSn_CFG0, 0),       0x1211010D  },
  {HWIO_OPERATION,   HWIO_ADDRI(EBI2_CSn_CFG1, 0),       0x93000000  },
  {HWIO_OPERATION,   HWIO_ADDR(EBI2_SWBM_AHB_PRIORITY),  0x0024      },
  /*-----------------------------------------------------------------------
                            End of Configuration
  -----------------------------------------------------------------------*/   
  {END_OF_CFG_DATA,  0x00000000,  0x00000000   }
};

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
)
{
  if( configured_clk_speed->ebi2 == 24 )
  {
    dbl_parse_cfg_data(ebi2_cfg_data_24MHZ);
  }
  else if( configured_clk_speed->ebi2 == 32 )
  {
    dbl_parse_cfg_data(ebi2_cfg_data_32MHZ);
  }
}/* dbl_ebi2_configure */

