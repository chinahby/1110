/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          C L O C K  R E G I M E
                        D E V I C E   M A N A G E R
                   C O R E  I M P L E M E N T A T I O N

                     
GENERAL DESCRIPTION
  This module implements the core portion of the clock regime device
  manager.  This code should be portable across all targets.

EXTERNALIZED FUNCTIONS
  clk_regime_devman_init
  clk_regime_devman_register
  clk_regime_devman_deregister
  clk_regime_devman_request
  clk_regime_devman_verbose
  clk_regime_devman_param

INITIALIZATION AND SEQUENCING REQUIREMENTS
  clk_regime_device_manager_init must be called before any other functions


Copyright(c) 2006 by QUALCOMM, Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/clkregim/devman/clkrgm_devman.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/07   gfr     Lint cleanup.
01/29/07   gfr     Workaround for broken windows compiler.
12/15/06   gfr     Fix for determining new minimum speed.
12/08/06   gfr     Add intlocks around critical sections.
12/07/06   gfr     Compiler warning fix.
12/05/06   gfr     Initial revision.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "clkregim.h"
#include "clkrgm_devman.h"
#include "clkrgmi_devman.h"



/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/* Magic word to validate client */
#define CLKRGM_DEVMAN_CLIENT_MAGIC  0xBEADED

/* Max of 32 clients since we use a 32 bit tag field.  Could easily be
   increased by using two such words if necessary */
#define CLKRGM_DEVMAN_MAX_CLIENTS 32

/* Maximum speed change handlers per device.  Very low right now as nobody
   is using this except possibly MDP. */
#define CLKRGM_DEVMAN_MAX_SPEED_CHANGE_HANDLERS 4

/* Logging macro - Windows compiler seems to have a problem concatenating
   strings. */
#ifdef FEATURE_WINCE
#error code not present
#else
  #define CLKRGM_DEVMAN_MSG(fmt, a, b, c) \
    if (clkrgm_devman.verbose) MSG_HIGH("[DevMan] " fmt, a, b, c);
#endif


/* --------------------------------------------------------------------------
   Device manager local storage type
-------------------------------------------------------------------------- */

typedef struct
{
  /* Mask of clients requesting each operating level for each device */
  uint32 operating_level_mask[CLKRGM_NUM_DEVICES][CLKRGM_NUM_DEVICE_OPER_LEVELS];

  /* Current operating level */
  clkrgm_device_operating_level_type operating_level[CLKRGM_NUM_DEVICES];

  /* Current minimum speeds */
  clkrgm_device_minimum_speed_type minimum_speed[CLKRGM_NUM_DEVICES];

  /* Device speed change notification handlers */
  clkrgm_device_speed_change_handler_type speed_change_handlers
    [CLKRGM_NUM_DEVICES][CLKRGM_DEVMAN_MAX_SPEED_CHANGE_HANDLERS];

  /* Parameter change handler, exists in clock regime driver */
  clkrgm_devman_handler_type devman_handler;

  /* Mask of client tags in use */
  uint32 used_tags;

  /* Head of client list */
  clkrgm_devman_client_type *client_list;

  /* Number of currently registered clients */
  uint32 num_clients;

  /* Verbose flag */
  boolean verbose;

} clkrgm_devman_type;



/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                             LOCAL STORAGE

===========================================================================*/

/* Device manager collected parameters */
static clkrgm_devman_type clkrgm_devman;



/*===========================================================================

                             EXTERNAL REFERENCES

===========================================================================*/



/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_GRAB_TAG

  DESCRIPTION   This function returns the next available tag.

  PARAMETERS    None.

  DEPENDENCIES  clk_regime_devman_init must have been previously called.

  RETURN VALUE  The tag.

  SIDE EFFECTS  None.

==========================================================================*/

static uint32 clk_regime_devman_grab_tag (void)
{
  uint32 tag;

  /* Traverse bits in the tag mask until we find one not set */
  for (tag = 0; tag < CLKRGM_DEVMAN_MAX_CLIENTS; tag++)
  {
    if ((clkrgm_devman.used_tags & (1 << tag)) == 0)
    {
      clkrgm_devman.used_tags |= (1 << tag);
      return tag;
    }
  }

  /* If this happens then we need to increase the max number of clients */
  ERR_FATAL("Out of clients!", 0, 0, 0);

  /* Make compiler happy */
  return 0;                                                   /*lint !e527*/

} /* clk_regime_devman_grab_tag */



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_DROP_TAG

  DESCRIPTION   This function drops the given tag.

  PARAMETERS    tag - the tag to drop.

  DEPENDENCIES  clk_regime_devman_init must have been previously called.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

static void clk_regime_devman_drop_tag (uint32 tag)
{
  clkrgm_devman.used_tags &= ~(1UL << tag);

} /* clk_regime_devman_drop_tag */



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_ADD_SPEED_CHANGE_HANDLER

  DESCRIPTION   This function adds the given speed change handler to the
                device list.

  PARAMETERS    device - device
                handler - handler

  DEPENDENCIES  clk_regime_devman_init must have been previously called.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

static void clk_regime_devman_add_speed_change_handler
(
  clkrgm_device_type                       device,
  clkrgm_device_speed_change_handler_type  handler
)
{
  uint32 i;  /* index */

  /* --------------------------------------------------------------------- */
  
  /* Go through list and find first empty slot */
  for (i = 0; i < CLKRGM_DEVMAN_MAX_SPEED_CHANGE_HANDLERS; i++)
  {
    /* If this handler is already registered, we are done */
    if (clkrgm_devman.speed_change_handlers[device][i] == handler)
    {
      return;
    }

    if (clkrgm_devman.speed_change_handlers[device][i] == NULL)
    {
      break;
    }
  }

  /* Check that we are not full */
  if (i == CLKRGM_DEVMAN_MAX_SPEED_CHANGE_HANDLERS)
  {
    MSG_ERROR("Too many speed change handlers for device %d", device, 0, 0);
    return;
  }

  /* Add the handler */
  clkrgm_devman.speed_change_handlers[device][i] = handler;

} /* clk_regime_devman_add_speed_change_handler */



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_REMOVE_SPEED_CHANGE_HANDLER

  DESCRIPTION   This function removes the given speed change handler to
                the device list.

  PARAMETERS    device  - which device to watch
                handler - handler previously registered

  DEPENDENCIES  clk_regime_devman_init must have been previously called.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

static void clk_regime_devman_remove_speed_change_handler
(
  clkrgm_device_type                       device,
  clkrgm_device_speed_change_handler_type  handler
)
{
  uint32 i;  /* index */

  /* --------------------------------------------------------------------- */
  
  /* Go through list and find handler */
  for (i = 0; i < CLKRGM_DEVMAN_MAX_SPEED_CHANGE_HANDLERS; i++)
  {
    if (clkrgm_devman.speed_change_handlers[device][i] == handler)
    {
      break;
    }
  }

  /* Check that we found the handler in question */
  if (i == CLKRGM_DEVMAN_MAX_SPEED_CHANGE_HANDLERS)
  {
    MSG_ERROR(
      "Could not find speed change handler %x for device %d",
      handler, device, 0);
    return;
  }

  /* Shift other handlers down */
  for (; i < CLKRGM_DEVMAN_MAX_SPEED_CHANGE_HANDLERS - 1; i++)
  {
    clkrgm_devman.speed_change_handlers[device][i] =
      clkrgm_devman.speed_change_handlers[device][i + 1];

    /* If we hit the end then we are done */
    if (clkrgm_devman.speed_change_handlers[device][i] == NULL)
    {
      break;
    }
  }

  /* Clear out last entry */
  clkrgm_devman.speed_change_handlers[device][i] = NULL;

} /* clk_regime_devman_remove_speed_change_handler */



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_SPEED_CHANGE_HANDLER

  DESCRIPTION   Internal handler passed to clock regime driver used to
                send all speed change notifications for a particular device.

  PARAMETERS    device        - which device changed speed
                position      - pre- or post- speed change
	              old_speed_khz - old speed in kilohertz
	              new_speed_khz - new speed in kilohertz

  DEPENDENCIES  clk_regime_devman_init must have been previously called.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

static void clk_regime_devman_speed_change_handler
(
  clkrgm_device_type               device,
  clkrgm_device_speed_change_type  position,
  uint32                           old_speed_khz,
  uint32                           new_speed_khz
)
{
  uint32 i;  /* index */

  /* --------------------------------------------------------------------- */

  /* Check that the speed actually changed */
  if (old_speed_khz == new_speed_khz)
  {
    return;
  }
 
  /* Invoke the handlers */
  for (i = 0; i < CLKRGM_DEVMAN_MAX_SPEED_CHANGE_HANDLERS &&
       clkrgm_devman.speed_change_handlers[device][i] != NULL; i++)
  {
    (*clkrgm_devman.speed_change_handlers[device][i])(
      device, position, old_speed_khz, new_speed_khz);
  }
  
} /* clk_regime_devman_speed_change_handler */



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
)
{
  uint32 i, j;

  /* ------------------------------------------------------------------------
     Init all master parameters to default
  ------------------------------------------------------------------------ */

  for (i = 0; i < CLKRGM_NUM_DEVICES; i++)
  {
    for (j = 0; j < CLKRGM_NUM_DEVICE_OPER_LEVELS; j++)
    {
      clkrgm_devman.operating_level_mask[i][j] = 0;
    }

    clkrgm_devman.operating_level[i] = CLKRGM_DEVICE_OPER_LEVEL_NORMAL;

    clkrgm_devman.minimum_speed[i] = CLKRGM_DEVICE_SPEED_DEFAULT;
  }

  /* ------------------------------------------------------------------------
     Start with no clients
  ------------------------------------------------------------------------ */

  clkrgm_devman.client_list = NULL;
  clkrgm_devman.num_clients = 0;
  clkrgm_devman.used_tags = 0;

  /* ------------------------------------------------------------------------
     Save the given handler
  ------------------------------------------------------------------------ */

  clkrgm_devman.devman_handler = handler;

} /* clk_regime_devman_init */



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_REGISTER

  DESCRIPTION   This function registers a new client with the device
                manager.  Should never be called on a client structure
                previously registered.

  PARAMETERS    client - the client structure to register

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_devman_register
(
  clkrgm_devman_client_type       *client,
  clkrgm_devman_client_group_type  client_group
)
{
  uint32 i, isav;
  clkrgm_devman_client_type *client_index;

  /* ------------------------------------------------------------------------
     Validate client
  ------------------------------------------------------------------------ */

  if (client == NULL)
  {
    MSG_ERROR("clk_regime_devman_register: null client", 0, 0, 0);
    return;
  }

  /* ------------------------------------------------------------------------
     Lock interrupt to protect clkrgm_devman
  ------------------------------------------------------------------------ */

  INTLOCK_SAV(isav);

  /* ------------------------------------------------------------------------
     Do not allow multiple register calls.  Since we do not control how
     the client structure is allocated, we cannot check magic so we use
     the client_list instead.
  ------------------------------------------------------------------------ */

  for (client_index = clkrgm_devman.client_list;
       client_index != NULL && client_index != client;
       client_index = client_index->next);

  if (client_index == client)
  {
    MSG_ERROR("Client %d already registered.", 0, 0, 0);
    INTFREE_SAV(isav);
    return;
  }

  /* ------------------------------------------------------------------------
     Logging
  ------------------------------------------------------------------------ */

  CLKRGM_DEVMAN_MSG("Client %d registered", client, 0, 0);

  /* ------------------------------------------------------------------------
     Init all parameters to default
  ------------------------------------------------------------------------ */

  for (i = 0; i < CLKRGM_NUM_DEVICES; i++)
  {
    client->params[i].operating_level = CLKRGM_DEVICE_OPER_LEVEL_NORMAL;
    client->params[i].minimum_speed = CLKRGM_DEVICE_SPEED_DEFAULT;
    client->params[i].speed_change_handler = NULL;
  }

  /* ------------------------------------------------------------------------
     Assign a tag and magic
  ------------------------------------------------------------------------ */

  client->tag = clk_regime_devman_grab_tag();
  client->magic = CLKRGM_DEVMAN_CLIENT_MAGIC;
  client->group = client_group;

  /* ------------------------------------------------------------------------
     Insert at head of list
  ------------------------------------------------------------------------ */

  if (clkrgm_devman.client_list != NULL)
  {
    clkrgm_devman.client_list->prev = client;
  }
  client->next = clkrgm_devman.client_list;
  client->prev = NULL;
  clkrgm_devman.client_list = client;
  clkrgm_devman.num_clients++;

  /* ------------------------------------------------------------------------
     Unlock interrupts
  ------------------------------------------------------------------------ */

  INTFREE_SAV(isav);

} /* clk_regime_devman_register */



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_DEREGISTER

  DESCRIPTION   This function deregisters a client with the device
                manager.  Should never be called on a client structure
                that has not been previously registered.

  PARAMETERS    client - the client structure to deregister

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_devman_deregister
(
  clkrgm_devman_client_type *client
)
{
  clkrgm_device_type       device;
  clkrgm_device_param_type param;

  /* ------------------------------------------------------------------------
     Validate client
  ------------------------------------------------------------------------ */

  if (client == NULL || client->magic != CLKRGM_DEVMAN_CLIENT_MAGIC)
  {
    MSG_ERROR(
      "Invalid client in deregister: client=%d, magic=%d", client,
      client == NULL ? 0 : client->magic, 0);
    return;
  }

  /* ------------------------------------------------------------------------
     Lock interrupt to protect clkrgm_devman
  ------------------------------------------------------------------------ */

  INTLOCK();

  /* ------------------------------------------------------------------------
     Check if any parameters are not default
  ------------------------------------------------------------------------ */

  for (device = (clkrgm_device_type)0; device < CLKRGM_NUM_DEVICES; device++)
  {
    /* Operating level */
    if (client->params[device].operating_level !=
          CLKRGM_DEVICE_OPER_LEVEL_NORMAL)
    {
      param.operating_level = CLKRGM_DEVICE_OPER_LEVEL_NORMAL;
      clk_regime_devman_request(
        client, device, CLKRGM_DEVICE_REQUEST_SET_OPERATING_LEVEL,
        &param);
    }

    /* Device minimum speed */
    if (client->params[device].minimum_speed != CLKRGM_DEVICE_SPEED_DEFAULT)
    {
      param.minimum_speed = CLKRGM_DEVICE_SPEED_DEFAULT;
      clk_regime_devman_request(
        client, device, CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED,
        &param);
    }

    /* Speed change handler */
    if (client->params[device].speed_change_handler != NULL)
    {
      param.speed_change_handler = NULL;
      clk_regime_devman_request(
        client, device, CLKRGM_DEVICE_REQUEST_SPEED_CHANGE_NOTIFICATION,
        &param);
    }
  }

  /* ------------------------------------------------------------------------
     Release the client tag
  ------------------------------------------------------------------------ */

  clk_regime_devman_drop_tag(client->tag);
  client->magic = 0;

  /* ------------------------------------------------------------------------
     Remove the client from the list
  ------------------------------------------------------------------------ */

  if (client->next != NULL)
  {
    client->next->prev = client->prev;
  }
  if (client->prev != NULL)
  {
    client->prev->next = client->next;
  }
  if (client == clkrgm_devman.client_list)
  {
    clkrgm_devman.client_list = client->next;
  }
  clkrgm_devman.num_clients--;

  /* Just in case, wipe out the pointers */
  client->prev = NULL;
  client->next = NULL;

  /* ------------------------------------------------------------------------
     Unlock interrupt
  ------------------------------------------------------------------------ */

  INTFREE();

  /* ------------------------------------------------------------------------
     Logging
  ------------------------------------------------------------------------ */

  CLKRGM_DEVMAN_MSG("Client %d deregistered", client, 0, 0);

} /* clk_regime_devman_deregister */



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_HANDLE_SPEED_CHANGE_NOTIFICATION

  DESCRIPTION   This function handles a client changing their speed
                change notification handler.

  PARAMETERS    client - the client structure 
                device - the device in question
                param  - the new handler

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

static boolean clk_regime_devman_handle_speed_change_notification
(
  clkrgm_devman_client_type              *client,
  clkrgm_device_type                      device,
  clkrgm_device_speed_change_handler_type handler,
  clkrgm_device_param_type               *new_param
)
{
  boolean handler_installed;

  /* ------------------------------------------------------------------------
     Check if we have any registered handlers for this device
  ------------------------------------------------------------------------ */

  if (clkrgm_devman.speed_change_handlers[device][0] == NULL)
  {
    handler_installed = FALSE;
  }
  else
  {
    handler_installed = TRUE;
  }

  /* ------------------------------------------------------------------------
     Register new handler
  ------------------------------------------------------------------------ */

  if (handler != client->params[device].speed_change_handler)
  {
    /* Deregister old handler */
    if (client->params[device].speed_change_handler != NULL)
    {
      clk_regime_devman_remove_speed_change_handler(
        device, client->params[device].speed_change_handler);
    }

    client->params[device].speed_change_handler = handler;

    /* Register new handler */
    if (client->params[device].speed_change_handler != NULL)
    {
      clk_regime_devman_add_speed_change_handler(
        device, client->params[device].speed_change_handler);
    }
  }

  /* ------------------------------------------------------------------------
     Check if this was the first handler
  ------------------------------------------------------------------------ */

  if (clkrgm_devman.speed_change_handlers[device][0] == NULL && handler_installed)
  {
    /* We removed the last handler */
    new_param->speed_change_handler = NULL;
    return TRUE;
  }

  if (clkrgm_devman.speed_change_handlers[device][0] != NULL && !handler_installed)
  {
    /* We added the first handler */
    new_param->speed_change_handler = clk_regime_devman_speed_change_handler;
    return TRUE;
  }

  /* No change */
  return FALSE;

} /* clk_regime_devman_handle_speed_change_notification */



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_HANDLE_SET_OPERATING_LEVEL

  DESCRIPTION   This function handles a client setting the speed mode.

  PARAMETERS    client - the client structure 
                device - the device in question
                operating_level - the new speed mode
                new_param - return parameter if the speed mode changes

  DEPENDENCIES  None.

  RETURN VALUE  TRUE if the overall device speed mode changed.  In this
                case new_param will contain the new speed mode.

  SIDE EFFECTS  None.

==========================================================================*/

static boolean clk_regime_devman_handle_set_operating_level
(
  clkrgm_devman_client_type          *client,
  clkrgm_device_type                  device,
  clkrgm_device_operating_level_type  operating_level,
  clkrgm_device_param_type           *new_param
)
{

  /* Remove old operating level from mask */
  if (client->params[device].operating_level != CLKRGM_DEVICE_OPER_LEVEL_NORMAL)
  {
    clkrgm_devman.operating_level_mask[device]
      [client->params[device].operating_level] &= ~(1UL << client->tag);
  }

  /* Add new operating level to mask */
  if (operating_level != CLKRGM_DEVICE_OPER_LEVEL_NORMAL)
  {
    clkrgm_devman.operating_level_mask[device][operating_level] |=
      (1 << client->tag);
  }

  /* Save new operating level */
  client->params[device].operating_level = operating_level;

  /* Determine new operating level for this device */
  for (operating_level = (clkrgm_device_operating_level_type)
         (CLKRGM_NUM_DEVICE_OPER_LEVELS - 1);
       operating_level > CLKRGM_DEVICE_OPER_LEVEL_NORMAL &&
         clkrgm_devman.operating_level_mask[device][operating_level] == 0;
       operating_level--);

  /* Check if the level has changed */
  if (operating_level != clkrgm_devman.operating_level[device])
  {
    clkrgm_devman.operating_level[device] = operating_level;
    new_param->operating_level = operating_level;
    return TRUE;
  }

  /* No state change */
  return FALSE;

} /* clk_regime_devman_handle_set_operating_level */



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_HANDLE_SET_MINIMUM_SPEED

  DESCRIPTION   This function handles a client setting the minimum speed.

  PARAMETERS    client - the client structure 
                device - the device in question
                minimum_speed - the new minimum speed in KHz
                new_param - return parameter if the minimum speed changes

  DEPENDENCIES  None.

  RETURN VALUE  TRUE if the overall device minimum speed changed.  In this
                case new_param will contain the new minimum speed.

  SIDE EFFECTS  None.

==========================================================================*/

static boolean clk_regime_devman_handle_set_minimum_speed
(
  clkrgm_devman_client_type        *client,
  clkrgm_device_type                device,
  clkrgm_device_minimum_speed_type  minimum_speed,
  clkrgm_device_param_type         *new_param
)
{
  clkrgm_devman_client_type *client_index;  /* Linked list index */

  /* --------------------------------------------------------------------- */

  /* Only QDSP services group can set minimum speed for ADSP and MDSP.
     Right now we need to relax this for MDSP however. */
  if ((/*device == CLKRGM_DEVICE_MDSP ||*/ device == CLKRGM_DEVICE_ADSP) &&
      client->group != CLKRGM_DEVMAN_CLIENT_GROUP_QDSP_SERVICES)
  {
    MSG_ERROR(
      "Client group %d not allowed to set min speed for %d", client->group,
      device, 0);
    return FALSE;
  }

  /* Save the new minimum speed */
  client->params[device].minimum_speed = minimum_speed;

  /* If the new minimum is lower than the current one we need to redetermine
     the absolute minimum since this client may have previously been the
     lowest already.  So we go through the list and find the new minimum. */
  if (minimum_speed < clkrgm_devman.minimum_speed[device])
  {
    minimum_speed = CLKRGM_DEVICE_SPEED_DEFAULT;
    for (client_index = clkrgm_devman.client_list; client_index != NULL;
         client_index = client_index->next)
    {
      if (client_index->params[device].minimum_speed > minimum_speed)
      {
        minimum_speed = client_index->params[device].minimum_speed;
      }
    }
  }

  /* Check for new minimum speed */
  if (clkrgm_devman.minimum_speed[device] != minimum_speed)
  {
    clkrgm_devman.minimum_speed[device] = minimum_speed;
    new_param->minimum_speed = minimum_speed;
    return TRUE;
  }

  /* No minimum speed change */
  return FALSE;

} /* clk_regime_devman_handle_set_minimum_speed */



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_REQUEST

  DESCRIPTION   This function processes a device manager request

  PARAMETERS    client - the client structure 
                device - the device in question
                request - the request type
                param  - the new bus mode

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_devman_request
(
  clkrgm_devman_client_type *client,
  clkrgm_device_type         device,
  clkrgm_device_request_type request,
  clkrgm_device_param_type  *param
)
{
  boolean                    param_changed;
  clkrgm_device_param_type   new_param;

  /* ------------------------------------------------------------------------
     Validate the client
  ------------------------------------------------------------------------ */

  if (client == NULL || client->magic != CLKRGM_DEVMAN_CLIENT_MAGIC)
  {
    MSG_ERROR(
      "Invalid client in request: client=%d, magic=%d", client,
      client == NULL ? 0 : client->magic, 0);
    return;
  }

  /* ------------------------------------------------------------------------
     Logging
  ------------------------------------------------------------------------ */

  CLKRGM_DEVMAN_MSG(
    "Request: device=%d, request=%d, param=%d", device, request,
    *(uint32 *)param);

  /* ------------------------------------------------------------------------
     Lock interrupts
  ------------------------------------------------------------------------ */

  INTLOCK();

  /* ------------------------------------------------------------------------
     Call the appropriate request handler
  ------------------------------------------------------------------------ */

  switch (request)
  {
    case CLKRGM_DEVICE_REQUEST_SET_OPERATING_LEVEL:
      param_changed = clk_regime_devman_handle_set_operating_level(
                        client, device, param->operating_level, &new_param);
      break;

    case CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED:
      param_changed = clk_regime_devman_handle_set_minimum_speed(
                        client, device, param->minimum_speed, &new_param);
      break;

    case CLKRGM_DEVICE_REQUEST_SPEED_CHANGE_NOTIFICATION:
      param_changed = clk_regime_devman_handle_speed_change_notification(
                        client, device, param->speed_change_handler,
                        &new_param);
      break;

    default:
      MSG_ERROR("Unsupported request: req=%d, device=%d", request, device, 0);
      return;
  }

  /* ------------------------------------------------------------------------
     Forward the request to the main driver handler if necessary
  ------------------------------------------------------------------------ */

  if (param_changed)
  {
    (*clkrgm_devman.devman_handler)(device, request, &new_param);
  }

  /* ------------------------------------------------------------------------
     Unlock interrupts
  ------------------------------------------------------------------------ */

  INTFREE();

} /* clk_regime_devman_request */



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
)
{
  switch (request)
  {
    case CLKRGM_DEVICE_REQUEST_SET_OPERATING_LEVEL:
      param->operating_level = clkrgm_devman.operating_level[device];
      break;

    case CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED:
      param->minimum_speed = clkrgm_devman.minimum_speed[device];
      break;

    case CLKRGM_DEVICE_REQUEST_SPEED_CHANGE_NOTIFICATION:
      if (clkrgm_devman.speed_change_handlers[device][0] == NULL)
      {
        param->speed_change_handler = NULL;
      }
      else
      {
        param->speed_change_handler = clk_regime_devman_speed_change_handler;
      }
      break;

    default:
      MSG_ERROR("Unsupported request: req=%d, device=%d", request, device, 0);
      break;
  }

} /* clk_regime_devman_param */



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_VERBOSE

  DESCRIPTION   This function configures verbose mode in the device
                manager driver.

  PARAMETERS    verbose - TRUE to enable verbosity

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_devman_verbose (boolean verbose)
{
  /* Save new mode */
  clkrgm_devman.verbose = verbose;

} /* clk_regime_devman_verbose */


