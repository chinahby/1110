/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          C L O C K  R E G I M E
                      R E S O U R C E  M A N A G E R
                   C O R E  I M P L E M E N T A T I O N

                     
GENERAL DESCRIPTION
  This module implements the core portion of the clock regime resource
  manager.  This code should be portable across all targets.

EXTERNALIZED FUNCTIONS
  clk_regime_resource_enable
  clk_regime_resource_disable
  clk_regime_resource_required
  clk_regime_resource_required_by_client
  clk_regime_rm_init
  clk_regime_rm_init_clk_info
  clk_regime_rm_register_handler
  clk_regime_rm_clk_required
  clk_regime_rm_set_exclusive

INITIALIZATION AND SEQUENCING REQUIREMENTS
  clk_regime_rm_init must be called before any other functions


Copyright(c) 2006 by QUALCOMM, Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/clkregim/rm/clkrgm_rm.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
16/09/07    th     Init shared memory section for WinMob
05/15/07   gfr     Make internal data shared for WinCE.
02/26/07   gfr     Lint cleanup.
01/30/07   gfr     Added missing header.
12/21/06   gfr     Added sanity check for more than 32 clients.
08/17/06   gfr     Fixed bug in clk_regime_resource_required_by_client
                   for clients greater than 7.
08/08/06   gfr     Added clk_regime_rm_verbose function.
07/26/06   gfr     Add "force_enabled" flag to prevent disabling a clock
                   during debugging.
07/20/06   gfr     Replace CLKRGM_RM_DEBUG by a dynamic flag.
06/08/06   gfr     Fix compiler warning.
06/06/06   gfr     Disable CLKRGM_RM_DEBUG by default.
06/02/06   gfr     Always enable the clock just in case.
02/09/06   gfr     Initial revision.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"

#ifdef FEATURE_CLKREGIM_RM

#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "clkregim.h"
#include "clkrgm_rm.h"
#include "clkrgmi_rm.h"




/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/* Information stored per-resource */
typedef struct
{
   /* Resource request/release handler */
   clkrgm_rm_handler_type handler;

   /* Map of which clients need the given resource */
   uint32                 client_list;

   /* Indicates if this resource can be required by multiple clients */
   boolean                exclusive;

} clkrgm_rm_rsrc_info_type;


/* Collection of local parameters */
typedef struct
{
   /* Flag set to enable printing a ton of messages */
   boolean                  verbose;

   /* Flag set when a resource request is being processed */
   boolean                  active;

   /* Resource information */
   clkrgm_rm_rsrc_info_type resource[CLKRGM_NUM_OF_RESOURCES];

} clkrgm_rm_type;


/* Message display macro to wrap verbose flag check */
#define CLKRGM_RM_MSG(str, p1, p2, p3)  \
  if (clkrgm_rm.verbose)                \
  {                                     \
    MSG_HIGH(str, p1, p2, p3);          \
  }



/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                             LOCAL STORAGE

===========================================================================*/

#if defined(FEATURE_WINCE) && !defined(FEATURE_WINCE_BOOTLOADER)
#error code not present
#endif

/* Resource manager collected parameters */
static clkrgm_rm_type clkrgm_rm = {0};

#if defined(FEATURE_WINCE) && !defined(FEATURE_WINCE_BOOTLOADER)
#error code not present
#endif



/*===========================================================================

                             EXTERNAL REFERENCES

===========================================================================*/



/*===========================================================================

                             FUNCTION DEFINITIONS

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

void clk_regime_rm_init (void)
{
   /* Sanity */
   if (CLKRGM_NUM_OF_CLIENTS >= 32)                           /*lint !e506*/
   {
      ERR_FATAL("Need code changes to support more than 32 clients", 0, 0, 0);
   }

   /* Setup the handlers */
   clk_regime_rm_setup_handlers();

} /* clk_regime_rm_init */



/*==========================================================================

  FUNCTION      CLK_REGIME_RM_VERBOSE

  DESCRIPTION   This function sets the resource manager to verbose mode.

  PARAMETERS    enable - whether or not to be verbose.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_rm_verbose (boolean enable)
{
   /* Save flag */
   clkrgm_rm.verbose = enable;

} /* clk_regime_rm_verbose */



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
)
{
   /* Sanity check input */
   if (resource >= CLKRGM_NUM_OF_RESOURCES)
   {
      MSG_ERROR("Invalid resource: r=%d", resource, 0, 0);
      return;
   }

   /* Add handler */
   clkrgm_rm.resource[resource].handler = handler;
}



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
)
{
   /* Sanity check input */
   if (resource >= CLKRGM_NUM_OF_RESOURCES)
   {
      MSG_ERROR("Invalid resource: r=%d", resource, 0, 0);
      return;
   }

   /* Add handler */
   clkrgm_rm.resource[resource].exclusive = TRUE;
}



/*==========================================================================

  FUNCTION      CLK_REGIME_RESOURCE_REQUIRED

  DESCRIPTION   Returns whether or not the given resource is currently
                required by any clients.

  PARAMETERS    resource - which resource to check

  DEPENDENCIES  None.

  RETURN VALUE  boolean - true if the resource is required

  SIDE EFFECTS  None.

==========================================================================*/

boolean clk_regime_resource_required
(
   clk_regime_resource_type resource
)
{
   return (clkrgm_rm.resource[resource].client_list != 0);
}



/*==========================================================================

  FUNCTION      CLK_REGIME_RESOURCE_REQUIRED_BY_CLIENT

  DESCRIPTION   Returns whether or not the given resource is currently
                required by the given client.

  PARAMETERS    client   - which client to check
                resource - which resource to check

  DEPENDENCIES  None.

  RETURN VALUE  boolean - true if the resource is required by this client

  SIDE EFFECTS  None.

==========================================================================*/

boolean clk_regime_resource_required_by_client
(
   clk_regime_client_type    client,
   clk_regime_resource_type  resource
)
{
   return ((clkrgm_rm.resource[resource].client_list 
            & (1UL << (uint32)client)) != 0);
}



/*==========================================================================

  FUNCTION      CLKRGM_RM_ADD_CLIENT_TO_RESOURCE

  DESCRIPTION   Marks the given resource as being required by the given
                client.

  PARAMETERS    client   - which client to add
                resource - which resource to add the client to

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

static void clkrgm_rm_add_client_to_resource
(
   clk_regime_client_type    client,
   clk_regime_resource_type  resource
)
{
   clkrgm_rm.resource[resource].client_list |= (1UL << (uint32)client);

   CLKRGM_RM_MSG("Client %d requested resource %d (clients=%x)",
                 client, resource, clkrgm_rm.resource[resource].client_list);

} /* clkrgm_rm_add_client_to_resource */



/*==========================================================================

  FUNCTION      CLKRGM_RM_REMOVE_CLIENT_TO_RESOURCE

  DESCRIPTION   Marks the given resource as not being required by the given
                client.

  PARAMETERS    client   - which client to remove
                resource - which resource to remove the client from

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

static void clkrgm_rm_remove_client_from_resource
(
   clk_regime_client_type    client,
   clk_regime_resource_type  resource
)
{
   clkrgm_rm.resource[resource].client_list &= ~(1UL << (uint32)client);

   CLKRGM_RM_MSG("Client %d released resource %d (clients=%x)",
                 client, resource, clkrgm_rm.resource[resource].client_list);

} /* clkrgm_rm_remove_client_from_resource */



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
)
{
   /* Init the structure */
   clk_info->clock_id = id;
   clk_info->externally_enabled = FALSE;
   clk_info->usage_count = 0;
 
} /* clk_regime_rm_init_clk_info */



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
)
{
   return (clk_info->usage_count > 0 || clk_info->externally_enabled);

} /* clk_regime_rm_clk_required */



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
)
{
   boolean clk_required;  /* whether the clock was previously required */

   /*---------------------------------------------------------------*/

   /* Check if the clock is already enabled or not */
   clk_required = clk_regime_rm_clk_required(clk_info);

   /* If we are not active this must be an external request */
   if (!clkrgm_rm.active)
   {
      /* Flag this clock as enabled by an external client */
      clk_info->externally_enabled = TRUE;
   }
   else
   {
      /* Increase usage count */
      clk_info->usage_count++;

      CLKRGM_RM_MSG("Clock %d requested (usage=%d)",
                    clk_info->clock_id, clk_info->usage_count, 0);
   }

   /* If clock was off before, then it was just enabled */
   if (!clk_required)
   {
      CLKRGM_RM_MSG("Clock %d enabled", clk_info->clock_id, 0, 0);
   }

   /* Always enable the clock just in case */
   return TRUE;

} /* clk_regime_rm_clk_enable */



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
)
{
   boolean clk_required;  /* whether the clock is still required */

   /*---------------------------------------------------------------*/
   
   /* If RM is not active this is an external request */
   if (!clkrgm_rm.active)
   {
      /* Flag this clock as no longer enabled by an external client */
      clk_info->externally_enabled = FALSE;

      /* Disable regime if no other resources need it */
      clk_required = clk_regime_rm_clk_required(clk_info);

      /* Check if we need this clock */
      if (clk_required)
      {
         MSG_ERROR("Clock %d usage count %d, leaving on", clk_info->clock_id,
                   clk_info->usage_count, 0);
      }
   }
   else
   {
      /* Sanity check usage count - no harm in disabling again if this fails */
      if (clk_info->usage_count == 0)
      {
         MSG_ERROR("Usage count already zero, clock=%d", clk_info->clock_id, 0, 0);
         return TRUE;
      }

      clk_info->usage_count--;

      CLKRGM_RM_MSG("Clock %d released (usage=%d)",
                    clk_info->clock_id, clk_info->usage_count, 0);

      /* Disable regime if no other resources need it */
      clk_required = clk_regime_rm_clk_required(clk_info);
   }

   /* Debug message, and check for disable override */
   if (!clk_required)
   {
      if (clk_info->force_enabled)
      {
         CLKRGM_RM_MSG("Clock %d disable overridden", clk_info->clock_id, 0, 0);
         clk_required = TRUE;
      }
      else
      {
         CLKRGM_RM_MSG("Clock %d disabled", clk_info->clock_id, 0, 0);
      }
   }

   /* Return TRUE if the clock is no longer required */
   return !clk_required;

} /* clk_regime_rm_clk_disable */



/*==========================================================================

  FUNCTION      CLK_REGIME_RESOURCE_ENABLE

  DESCRIPTION   Called when a client requires the use of a particular
                clock regime resource.  Does not support nesting.

  PARAMETERS    client   - which client is making the request
                resource - the resource being enabled

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_resource_enable
(
   clk_regime_client_type    client,
   clk_regime_resource_type  resource
)
{
   boolean has_resource;  /* whether the client already has the resource */
   
   /*---------------------------------------------------------------*/

   /* Sanity check input */
   if (client >= CLKRGM_NUM_OF_CLIENTS || resource >= CLKRGM_NUM_OF_RESOURCES)
   {
      MSG_ERROR("Invalid client/resource: c=%d, r=%d", client, resource, 0);
      return;
   }

   INTLOCK();

   /* Mark the resource manager as active */
   clkrgm_rm.active = TRUE;

   /* Check if this client already has this resource */
   has_resource = clk_regime_resource_required_by_client(client, resource);

   /* Assign resource if necessary - we do not support nested resource
      requests from the same client, it is up to the client to ensure that
      this does not happen.  */
   if (!has_resource)
   {
      /* Check if this is an exclusive resource already in use */
      if (clkrgm_rm.resource[resource].exclusive &&
          clk_regime_resource_required(resource))
      {
         MSG_ERROR("Resource %d is exclusive, used by %x (c=%d)",
                   resource, clkrgm_rm.resource[resource].client_list, client);
      }
      else
      {
         /* Add client to resource list */
         clkrgm_rm_add_client_to_resource(client, resource);
      
         /* Execute the handler if it exists */
         if (clkrgm_rm.resource[resource].handler != NULL)
         {
            clkrgm_rm.resource[resource].handler(client, resource, CLKRGM_RM_ENABLE);
         }
      }
   }

   /* We are done */
   clkrgm_rm.active = FALSE;

   INTFREE();
   
} /* clk_regime_resource_enable */



/*==========================================================================

  FUNCTION      CLK_REGIME_RESOURCE_DISABLE

  DESCRIPTION   Called when a client no longer requires the use of a
                particular clock regime resource.

  PARAMETERS    client   - which client is making the request
                resource - the resource being disabled

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_resource_disable
(
   clk_regime_client_type    client,
   clk_regime_resource_type  resource
)
{
   boolean has_resource;  /* whether the client has the resource */
   
   /*---------------------------------------------------------------*/

   if (client >= CLKRGM_NUM_OF_CLIENTS || resource >= CLKRGM_NUM_OF_RESOURCES)
   {
      MSG_ERROR("Invalid client/resource: c=%d, r=%d", client, resource, 0);
      return;
   }

   INTLOCK();

   /* Mark the resource manager as active */
   clkrgm_rm.active = TRUE;

   /* Check if this client has this resource */
   has_resource = clk_regime_resource_required_by_client(client, resource);

   if (has_resource)
   {
      /* Remove client from resource list */
      clkrgm_rm_remove_client_from_resource(client, resource);

      /* Call the handler if it exists */
      if (clkrgm_rm.resource[resource].handler != NULL)
      {
         clkrgm_rm.resource[resource].handler(client, resource, CLKRGM_RM_DISABLE);
      }
   }

   /* We are done */
   clkrgm_rm.active = FALSE;

   INTFREE();

} /* clk_regime_resource_disable */


#endif  /* FEATURE_CLKREGIM_RM */
