#ifndef __CLKRGMI_DEVMAN_H__
#define __CLKRGMI_DEVMAN_H__
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          C L O C K  R E G I M E
                        D E V I C E   M A N A G E R
                   I N T E R N A L   D E F I N I T I O N S

                     
GENERAL DESCRIPTION
  This module defines the internal interface of the clock regime device
  manager.  This code should be portable across all targets.

EXTERNALIZED FUNCTIONS
  clk_regime_devman_init
  clk_regime_devman_param
  clk_regime_devman_verbose

INITIALIZATION AND SEQUENCING REQUIREMENTS
  clk_regime_devman_init must be called before any other functions


Copyright(c) 2006 by QUALCOMM, Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/clkregim/devman/clkrgmi_devman.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/06   gfr     Add intlocks around critical sections.
12/05/06   gfr     Initial revision.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "clkrgm_devman.h"



/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/* --------------------------------------------------------------------------
   Device request handler type
-------------------------------------------------------------------------- */

typedef void (*clkrgm_devman_handler_type)
(
  clkrgm_device_type          device,
  clkrgm_device_request_type  request,
  clkrgm_device_param_type   *param
);



/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/


/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_INIT

  DESCRIPTION   This function initializes the device manager.  Should
                be called at clock regime init.

  PARAMETERS    handler - the callback handler when a device parameter
                          changes.  Will execute with interrupts locked.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_devman_init
(
  clkrgm_devman_handler_type handler
);



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_PARAM

  DESCRIPTION   This function returns the current consolidated parameter
                for the given device and request.

  PARAMETERS    device  - device in question
                request - type of parameter
                param   - where to store the results

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_devman_param
(
  clkrgm_device_type          device,
  clkrgm_device_request_type  request,
  clkrgm_device_param_type   *param
);



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_VERBOSE

  DESCRIPTION   This function configures verbose mode in the device
                manager driver.

  PARAMETERS    verbose - TRUE to enable verbosity

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_devman_verbose (boolean verbose);


#endif /* !__CLKRGMI_DEVMAN_H__ */

