/*=============================================================================

                        S B I   I N I T   M O D U L E

GENERAL DESCRIPTION
  This file contains functions to initialize the SBI library, and assorted
  error handling functions.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
==============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/boot/boot_sbi_init.c#3 $    
$DateTime: 2005/11/10 17:03:35 $ $Author: rajeevp $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/26/05    ty     Initial revision

==============================================================================*/



/*=============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "comdef.h"
#include "target.h"
#include "clkregim.h"

/*=============================================================================

                            EXTERNAL DEFINITIONS

==============================================================================*/
#define SBI_9p6MHZ 2
#define SBI_6p4MHZ 3

extern void sbi_init(int freq);


/*============================================================================

FUNCTION BOOT_SBI_INIT

DESCRIPTION
  Initializes the SBI library

DEPENDENCIES
  bio_gpio_init() has been called,

RETURN VALUE
  Returns 0.

SIDE EFFECTS
  SBI is initialized. GPIOs associated with SBI is also initialized.
============================================================================*/
uint32 boot_sbi_init ( void )
{
#ifdef FEATURE_DUAL_CLK_RGM
	/* enable clk */
	clk_regime_msm_enable(CLK_RGM_SBI_M);
  //  clk_regime_msm_enable(CLK_RGM_SBI_M);
	sbi_init(SBI_9p6MHZ);
#else
	/* enable clk */
	clk_regime_enable(CLK_RGM_MODEM_SBI_M);
	/* initialize SBI */
	sbi_init(SBI_6p4MHZ);
#endif
  return 0;
}

/*===========================================================================

FUNCTION sbi_handle_runtime_error

DESCRIPTION
  This function handles runtime errors within the library.  It is external
  to the library so it can be customized as desired.  If this is called, then
  a critical error has occurred in the library.  It will attempt to recover
  gracefully, but this cannot be guaranteed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void sbi_handle_runtime_error
(
    const char * err_string,
    uint32 parm0,
    uint32 parm1,
    uint32 parm2
)
{
}

/*===========================================================================

FUNCTION sbi_handle_runtime_warning

DESCRIPTION
  This function handles runtime errors within the library.  It is external
  to the library so it can be customized as desired. If this is called, then
  something unexpected has occurred in the library.  The driver will recover
  although some transactions may be lost.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void sbi_handle_runtime_warning
(
    const char * err_string,
    uint32 parm0,
    uint32 parm1,
    uint32 parm2
)
{
}

