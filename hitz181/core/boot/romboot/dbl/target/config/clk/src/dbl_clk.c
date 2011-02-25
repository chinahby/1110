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

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/config/clk/dbl_clk.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================
                            INCLUDE FILES FOR MODULE
=============================================================================*/
#include <string.h>
#include "boot_comdef.h"
#include "dbl_clk.h"
#include "dbl_clki.h"
#include "boot_pbl_accessor.h"
#include "boot_msm.h"
#include "dbl_error_handler.h"
#include "dbl_hw.h"

/*=============================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
=============================================================================*/

/*-----------------------------------------------------------------------
  Structure to remember clock settings.
-----------------------------------------------------------------------*/
typedef struct
{
  const boot_clk_speed_type *previous_clk_speed;
  const boot_clk_speed_type *configured_clk_speed;
}dbl_clk_local_type;


extern const boot_clk_speed_type dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl;
extern const boot_clk_speed_type dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl;

/*-----------------------------------------------------------------------
  Local variable to remember clock settings.
-----------------------------------------------------------------------*/
static dbl_clk_local_type dbl_clk_local = { NULL, NULL };

/*=============================================================================   
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

FUNCTION  DBL_CLK_CONFIGURE_PLL_MICRO_HCLK

DESCRIPTION
 This function will configure the PLL.

DEPENDENCIES
  None

RETURN VALUE
  the HCLK speed.

SIDE EFFECTS
  None

===========================================================================*/
static const boot_clk_speed_type* dbl_clk_configure_pll_micro_hclk
(
  const dbl_clk_cfg_data_type  *clk_settings
)
{
  dbl_parse_cfg_data(clk_settings->clk_cfg_data);
  return clk_settings->new_clk_speed;
}

/*===========================================================================

FUNCTION  DBL_CLK_CONFIGURE

DESCRIPTION
 This function will configure the CLK interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static const boot_clk_speed_type* dbl_clock_configure
(
  const boot_clk_speed_type           *shared_clk_speed,
  const dbl_clk_cfg_data_type         *clk_settings
)
{
  /*-----------------------------------------------------------------------
    First verify that the current clock speed is compatible with the
    new configuration.
  -----------------------------------------------------------------------*/ 
  DBL_VERIFY( memcmp( clk_settings->old_clk_speed,
                      shared_clk_speed,
                      sizeof(*shared_clk_speed) ) == 0,
              DBL_ERR_CLK_SPEED_NOT_SUPPORTED );

  /*-----------------------------------------------------------------------
    Tell the hardware interface what the current clock speed is.
  -----------------------------------------------------------------------*/ 
  dbl_hw_set_configured_clock_speed( shared_clk_speed );
  dbl_clk_local.previous_clk_speed = shared_clk_speed;

  /*-----------------------------------------------------------------------
    Configure PLL and All the required clock....
  -----------------------------------------------------------------------*/ 
  dbl_clk_local.configured_clk_speed = 
    dbl_clk_configure_pll_micro_hclk( clk_settings );

  /*-----------------------------------------------------------------------
    Tell the hardware interface what is the clock speed.
  -----------------------------------------------------------------------*/ 
  dbl_hw_set_configured_clock_speed( dbl_clk_local.configured_clk_speed );

  /*-----------------------------------------------------------------------
    Update the global interface.
  -----------------------------------------------------------------------*/ 
  return dbl_clk_local.configured_clk_speed;
  
} /* dbl_clk_configure */

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
const boot_clk_speed_type* dbl_clk_get_speed_set_by_pbl( void )
{
  const boot_clk_speed_type *clk_speed = NULL;
  uint32 pbl_clk_speed;

  pbl_clk_speed = boot_pbl_get_clock_speed();  

  if ( pbl_clk_speed == dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl.arm9 )
  {
    clk_speed = &dbl_clk_tcxo_arm_20_ebi1_20_ebi2_20_pbl;
  }
  else if ( pbl_clk_speed == dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl.arm9 )
  {
    clk_speed = &dbl_clk_pll_384_arm_48_ebi1_48_ebi2_24_pbl;
  }
  else
  {
    DBL_ERR_FATAL( DBL_ERR_CLK_SPEED_NOT_SUPPORTED );
  }

  return clk_speed;
}

/*===========================================================================

FUNCTION  DBL_CLK_CONFIGURE

DESCRIPTION
 This function will configure the CLK interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
const boot_clk_speed_type* dbl_clk_configure
(
  const boot_clk_speed_type *shared_clk_speed
)
{
  const dbl_clk_cfg_data_type *clk_settings;

  clk_settings = dbl_clk_get_ptr_to_settings( shared_clk_speed );
  if (clk_settings != NULL)
  {
  	  return dbl_clock_configure( shared_clk_speed, clk_settings );
  }
  else 
  {
  	 DBL_ERR_FATAL( DBL_ERR_CLK_SPEED_NOT_SUPPORTED );
	 return (const boot_clk_speed_type*)NULL;
  }
} /* dbl_clk_configure */
