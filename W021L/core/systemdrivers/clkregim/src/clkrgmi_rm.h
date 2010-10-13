#ifndef CLKRGMI_RM_H
#define CLKRGMI_RM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C L O C K  R E S O U R C E  M A N A G E R  M O D U L E

                     
GENERAL DESCRIPTION
  This module defines the internal interface of the clock regime resource
  manager.

EXTERNALIZED FUNCTIONS
  clk_regime_rm_init
  clk_regime_rm_setup_handlers
  clk_regime_rm_register_handler
  clk_regime_rm_set_exclusive
  clk_regime_rm_init_clk_info
  clk_regime_rm_clk_enable
  clk_regime_rm_clk_disable
  clk_regime_rm_clk_required

INITIALIZATION AND SEQUENCING REQUIREMENTS
  clk_regime_rm_init must be called before any other functions


Copyright(c) 2006 by QUALCOMM, Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/clkregim/rm/clkrgmi_rm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/06   gfr     Added "force_enabled" flag to prevent disabling a clock
                   during debugging.
06/09/06   gfr     Added prototype for clk_regime_rm_setup_handlers
05/09/06   gfr     Initial revision.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"

#ifdef FEATURE_CLKREGIM_RM

#include "comdef.h"
#include "clkregim.h"



/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Type used internally to multiplex request/release to single handler */
typedef enum
{
   CLKRGM_RM_DISABLE,
   CLKRGM_RM_ENABLE
} clk_regime_rm_request_type;


/* Resource handler function pointer type */
typedef void (*clkrgm_rm_handler_type)
(
   clk_regime_client_type       client,
   clk_regime_resource_type     resource,
   clk_regime_rm_request_type   request
);


/* Generic information stored per-clock */
typedef struct
{
   /* Count of client/resource pairs using this clock regime - clock will
      be on if this value is greater than zero */
   uint32   usage_count;

   /* Identifier of this clock - used for logging */
   uint32   clock_id;

   /* Flag indicating if a non-RM client has enabled this clock */
   boolean  externally_enabled;

   /* Debug flag set to keep this clock to remain on all the time.  Note
      that this does not enable the clock, just prevent it being disabled. */
   boolean  force_enabled;

} clkrgm_rm_clk_info_type;




/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/

/*==========================================================================

  FUNCTION      CLK_REGIME_RM_INIT

  DESCRIPTION   This function initializes the resource manager.  Should
                be called at clock regime init.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_rm_init (void);



/*==========================================================================

  FUNCTION      CLK_REGIME_RM_SETUP_HANDLERS

  DESCRIPTION   Sets-up all the target and processor specific handlers.
                The implementation should be defined in a separate file.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_rm_setup_handlers (void);



/*==========================================================================

  FUNCTION      CLK_REGIME_RM_REGISTER_HANDLER

  DESCRIPTION   Called by target-specific code to register a handler to
                manage the given resource.  When requests for that resource
                arrive this handler will be executed.

  PARAMETERS    resource - which resource to register for
                handler  - the handler for the resource

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_rm_register_handler
(
   clk_regime_resource_type resource,
   clkrgm_rm_handler_type   handler
);



/*==========================================================================

  FUNCTION      CLK_REGIME_RM_SET_EXCLUSIVE

  DESCRIPTION   Marks the given resource as "exclusive", which means that
                only one client may own it at a time.  Since no indication
                is passed to a client if a resource request is denied, this
                is used for debugging purposes only.

  PARAMETERS    resource - which resource to mark as exclusive

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_rm_set_exclusive
(
   clk_regime_resource_type resource
);



/*==========================================================================

  FUNCTION      CLK_REGIME_RM_INIT_CLK_INFO

  DESCRIPTION   Initializes the given clk_info structure.

  PARAMETERS    clk_info - pointer to the structure to initialize
                id       - the id for this clock, used for logging

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_rm_init_clk_info
(
   clkrgm_rm_clk_info_type *clk_info,
   uint32                   id
);



/*==========================================================================

  FUNCTION      CLK_REGIME_RM_CLK_REQUIRED

  DESCRIPTION   Returns if the given clock is required by anyone.

  PARAMETERS    clk_info - pointer to the relevant clk info structure

  DEPENDENCIES  None.

  RETURN VALUE  boolean - true if the clock is required

  SIDE EFFECTS  None.

==========================================================================*/

boolean clk_regime_rm_clk_required
(
   clkrgm_rm_clk_info_type *clk_info
);



/*==========================================================================

  FUNCTION      CLK_REGIME_RM_CLK_ENABLE

  DESCRIPTION   Marks the given clock as enabled.  Depending on if the
                resource manager is currently active or not either the
                clock usage count will be increased or the clock will be
                marked as externally enabled.

  PARAMETERS    clk_info - pointer to the relevant clk info structure

  DEPENDENCIES  None.

  RETURN VALUE  boolean - true if the clock can be enabled

  SIDE EFFECTS  None.

==========================================================================*/

boolean clk_regime_rm_clk_enable
(
   clkrgm_rm_clk_info_type *clk_info
);



/*==========================================================================

  FUNCTION      CLK_REGIME_RM_CLK_DISABLE

  DESCRIPTION   Marks the given clock as disabled.  Depending on if the
                resource manager is currently active or not either the
                clock usage count will be decreased or the clock will be
                marked as externally disabled.  If nobody else needs the
                clock then TRUE is returned and it may be disabled in HW.

  PARAMETERS    clk_info - pointer to the relevant clk info structure

  DEPENDENCIES  None.

  RETURN VALUE  boolean - true if the clock can be disabled in HW

  SIDE EFFECTS  None.

==========================================================================*/

boolean clk_regime_rm_clk_disable
(
   clkrgm_rm_clk_info_type *clk_info
);



#endif /* FEATURE_CLKREGIM_RM */

#endif /* CLKRGMI_RM_H */
