/*===========================================================================


hsu_host_api.c

DESCRIPTION :                                       

High Speed USB Host Implementation.
This file contains the API layer implementation which acts as a device 
manager between clients and peripherals

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

EDIT HISTORY FOR FILE

$Header:
$DateTime:
=============================================================================
when      who  what, where, why
--------  ---  -------------------------------------------------------------
28/06/06  sk   File created. 

---------------------------------------------------------------------------*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "usbhost_api.h"
#include "hsu_host_api_i.h"
#include "hsu_host_al_hid.h"
#include "hsu_common.h"

#include "assert.h"
#include "string.h" 
#include "rex.h"
#include "msg.h"

#include "err.h"

#include "jtypes.h"
#include "jos.h"
#include <jusb.h>
#include <usbdi.h>
#include <usbdi_util.h>

#include "AEEstd.h"

#ifndef T_REXNT
#include "dev_monitor.h"
#endif /* T_REXNT */

/* Maximum number of clients that can register for a specific device type */
#define USBHOST_MAX_CLIENTS_PER_DEVICE_TYPE 3
/* Maximum number of clients waiting to open a device */
#define USBHOST_PENDING_QUEUE_LENGTH 3

#define USBHOST_CLIENT_POOL_SIZE \
((uint32)USBHOST_MAX_PDEV_TYPE * USBHOST_MAX_CLIENTS_PER_DEVICE_TYPE)

#define USBHOST_PENDING_CLIENT_POOL_SIZE \
(USBHOST_MAX_DEVICES * USBHOST_PENDING_QUEUE_LENGTH)

/* Array of devices currently connected - An entry is added
in the first available slot when a device is inserted. The entry is
removed when the device is unplugged */
static usbhost_dev_entry_type usbhost_dev_table[USBHOST_MAX_DEVICES];

/* Pool of linked-list records, to be used in usbhost_client_table */
static usbhost_client_entry_type usbhost_client_pool[USBHOST_CLIENT_POOL_SIZE];
/* Data corresponding to the usbhost_client_pool records */
static usbhost_client_data_type usbhost_client_data_pool[
                                            USBHOST_CLIENT_POOL_SIZE];
/* Pool of linked-list records, to be used in the pending queues */
static usbhost_client_entry_type usbhost_pending_client_pool[
                                            USBHOST_PENDING_CLIENT_POOL_SIZE];

/* Array of pointers to clients currently registered for each device type 
- indexed by the device type. If multiple clients are registered for a 
device of the same type then a linked list is maintained. Entries
are added at registration time and removed at deregistration time */
static usbhost_client_entry_type* usbhost_client_table[USBHOST_MAX_PDEV_TYPE];

/* Array of pointers to IOCTL functions that handle specific types
** of devices.
*/
static usbhost_ioctl_func_type
usbhost_ioctl_functions[USBHOST_MAX_PDEV_TYPE];

/* Array of pointers to functions that retrieve the usbd handle for a device */
static usbhost_get_usbd_handle_func_type
  usbhost_get_usbd_handle_functions[USBHOST_MAX_PDEV_TYPE];

/* Pending queue of open requests which have not been responded to - indexed 
by the device instance type. If multiple clients are waiting for a device 
instance then a linked list is maintained. An entry is added when a device
is unavailable for use. The entry may be removed if the client subsequently
chooses to not keep it's request enqueued OR if the device becomes available
at a later point */
static usbhost_client_entry_type * 
usbhost_pending_open_queue[USBHOST_MAX_DEVICES];

/* To assign unique client IDs */
static usbhost_client_id_type usbhost_client_id_num = 0;

/* Critical section data-structure, used for synchronizing access
** to the data structures.
*/
static rex_crit_sect_type usbhost_crit_sect;

/* Device handle generator - handles are uniquely created across all devices
*/
static usbhost_dev_handle_type usbhost_dev_hndl_cnt = 0;

#ifndef T_REXNT
static devmon_methods_t usbhost_devmon_methods;

/*======================================================================

FUNCTION usbhost_devmon_attached

DESCRIPTION
This callback function is called when a device is successfully attached to 
the stack.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

=======================================================================*/

static void usbhost_devmon_attached(mdev_t dev, mdev_t parent)
{
  devmon_info_t dev_info;
  devmon_info_t parent_info;

  HSU_MSG_HIGH("usbhost_devmon_attached, device = 0x%x, parent =  0x%x", dev, parent, 0);

  devmon_get_info(dev, &dev_info);
  devmon_get_info(parent, &parent_info);
}

/*======================================================================

FUNCTION usbhost_devmon_detached

DESCRIPTION
This callback function is called when a device is successfully detached from 
the stack.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

=======================================================================*/
static void usbhost_devmon_detached(mdev_t dev, mdev_t parent)
{
  HSU_MSG_HIGH("usbhost_devmon_detached, device = 0x%x, parent =  0x%x", dev, parent, 0);
}

/*======================================================================

FUNCTION usbhost_devmon_allow_attach

DESCRIPTION
This callback function is called when a device attach is detected and 
before the device is actually attached to the stack. The return value 
of this function determines whether to complete the device attach 
(i.e. allow the attach) or not.

DEPENDENCIES
None.

RETURN VALUE
Returns zero to allow the device attach, or a non-zero value to reject 
the attach.

SIDE EFFECTS
None.

=======================================================================*/
static jresult_t usbhost_devmon_allow_attach(mdev_t dev)
{
  HSU_MSG_HIGH("usbhost_devmon_allow_attach, device = 0x%x", dev, 0, 0);

  return HSU_JSUCCESS;
}

/*======================================================================

FUNCTION usbhost_devmon_notify

DESCRIPTION
This callback function is called to notify on device different states in 
connect and disconnect operations.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

=======================================================================*/
static jresult_t usbhost_devmon_notify(devmon_msg_t msg, mdev_t dev, juint32_t param)
{
  HSU_MSG_HIGH("usbhost_devmon_notify, device = 0x%x, msg = %d", dev, msg, 0);

  return HSU_JSUCCESS;
}
#endif /* T_REXNT */


/*======================================================================

FUNCTION usbhost_api_init

DESCRIPTION
Initialize the data structures for the API. Called from tmc. 

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

=======================================================================*/
void usbhost_api_init
( 
  void
)
{ 
  uint32 i;

  /* Initialize the table entries */
  for (i = 0; i < (uint32) USBHOST_CLIENT_POOL_SIZE; i++)
  {
    usbhost_client_pool[i].entry_is_empty = TRUE;
    usbhost_client_pool[i].data = &usbhost_client_data_pool[i];
  }

  for (i = 0; i < (uint32) USBHOST_PENDING_CLIENT_POOL_SIZE; i++)
  {
    usbhost_pending_client_pool[i].entry_is_empty = TRUE;
  }

  for (i = 0; i < (uint32) USBHOST_MAX_PDEV_TYPE; i++)
  {
    usbhost_client_table[i] = NULL;
    usbhost_ioctl_functions[i] = NULL;
    usbhost_get_usbd_handle_functions[i] = NULL;
  }

  for (i = 0; i < USBHOST_MAX_DEVICES ; i++)
  {
    usbhost_dev_table[i].entry_is_empty = TRUE;
    usbhost_pending_open_queue[i] = NULL;
  }

  usbhost_client_id_num = 0;
  usbhost_dev_hndl_cnt = 0;

#ifndef T_REXNT
  usbhost_devmon_methods.allow_attach = usbhost_devmon_allow_attach;
  usbhost_devmon_methods.attached = usbhost_devmon_attached;
  usbhost_devmon_methods.detached = usbhost_devmon_detached;
  usbhost_devmon_methods.devmon_notify_h = usbhost_devmon_notify;

  devmon_register(&usbhost_devmon_methods);
#endif

  USBHOST_GLOBAL_LOCK_INIT();
} /* usbhost_api_init() */

/*======================================================================

FUNCTION usbhost_power_vbus

DESCRIPTION
API to acquire / relinquish the power to VBUS
boolean acquire : TRUE : acquire VBUS
FALSE : relinquish VBUS
int8 session_time : If acquire is TRUE - then this indicates the
time till which VBUS will be powered to detect a device.
A value of -1 applies power indefinitely.  				  

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

=======================================================================*/
void usbhost_power_vbus
(
  int8    session_time, 
  boolean acquire
)
{
  HSU_USE_PARAM(session_time);
  HSU_USE_PARAM(acquire);
} /* usbhost_power_vbus() */

/*======================================================================

FUNCTION usbhost_alloc_client_id

DESCRIPTION
Returns a unique ID. Called from usbhost_register. The caller must have 
this mutexed.

DEPENDENCIES
None.

RETURN VALUE
A unique client ID

SIDE EFFECTS
None.

=======================================================================*/
static usbhost_client_id_type usbhost_alloc_client_id
(
  void
)
{
  if (usbhost_client_id_num < USBHOST_MAX_CLIENTS) 
  {
    usbhost_client_id_num++;
  }
  else
  {
    usbhost_client_id_num = USBHOST_INVALID_CLIENT_ID;
  }

  return usbhost_client_id_num;
} /* usbhost_alloc_client_id() */


/*======================================================================

FUNCTION usbhost_map_hndl_to_inst

DESCRIPTION
Maps a device handle back to the associated device instance being 
used by the specified client.

DEPENDENCIES
None.

RETURN VALUE
usbhost_dev_entry_type pointer or NULL

SIDE EFFECTS
None.

=======================================================================*/
static usbhost_dev_entry_type * usbhost_map_hndl_to_inst
(
  usbhost_client_id_type  client_id,
  usbhost_dev_handle_type dev_hndl
)
{
  uint32                      i, j;
  boolean                     hndl_found = FALSE;
  usbhost_dev_entry_type *    dev_entry_ptr = NULL;


  /* We need to locate the device instance referenced by the client
  ** that is using it and the device handle that the client opened 
  ** it with.
  */
  for (i = 0; 
    (i < (uint32) USBHOST_MAX_DEVICES) && (hndl_found == FALSE); 
    i++)
  {
    if (!usbhost_dev_table[i].entry_is_empty)
    {
      for (j = 0; 
        (j < USBHOST_MAX_DEV_HANDLES) && (hndl_found == FALSE);
        j++)
      {
        /* We found a device table entry so see if it has the desired
        ** device handle.
        */
        if (usbhost_dev_table[i].dev_hndl_tbl[j].dev_hndl == dev_hndl)
        {
          /* Found the device handle. Now see if the client that
          ** is using it matches.  If so we have found the correct entry.
          ** Otherwise, the requested client isn't the one that owns the
          ** handle so no go.
          */
          if ((usbhost_dev_table[i].dev_hndl_tbl[j].client_data != NULL) &&
              (usbhost_dev_table[i].dev_hndl_tbl[j].client_data->client_id == client_id))
          {
            dev_entry_ptr = &usbhost_dev_table[i];
          }

          hndl_found = TRUE;
        }
      } /* for j */
    }
  } /* for i */

  return (dev_entry_ptr);
} /* usbhost_map_hndl_to_inst() */

/*======================================================================

FUNCTION usbhost_alloc_dev_hndl

DESCRIPTION
Allocates the next available handle for the device instance.  If no
handle is available it will return 0.

DEPENDENCIES
None.

RETURN VALUE
usbhost_dev_handle_type

SIDE EFFECTS
None.

=======================================================================*/
static usbhost_dev_handle_type usbhost_alloc_dev_hndl
(
  usbhost_dev_entry_type *          dev_entry,
  const usbhost_client_entry_type * client_ptr
)
{
  /* Handles just keep incrementing */
  uint32 i;        
  usbhost_dev_handle_type dev_hndl = 0;

  for (i = 0; i < USBHOST_MAX_DEV_HANDLES; i++)
  {
    /* Search the table -- an entry of USBHOST_INVALID_DEV_HNDL indicates that 
    ** the slot is available.
    */
    if (dev_entry->dev_hndl_tbl[i].dev_hndl == USBHOST_INVALID_DEV_HNDL)
    {
      /* Device handles are unique across all devices for all open requests */
      usbhost_dev_hndl_cnt++;

      dev_entry->dev_hndl_tbl[i].dev_hndl = usbhost_dev_hndl_cnt;
      dev_entry->dev_hndl_tbl[i].client_data = client_ptr->data;

      dev_entry->dev_hndl_max = MAX(dev_entry->dev_hndl_max,
        dev_entry->dev_hndl_tbl[i].dev_hndl);

      dev_hndl = dev_entry->dev_hndl_tbl[i].dev_hndl;

      HSU_MSG_HIGH("Device Handle %d allocated for device instance %d, pdev_type=%d",
        dev_entry->dev_hndl_tbl[i].dev_hndl, 
        dev_entry->dev_inst_id, 
        dev_entry->pdev_type);
      break;
    }
  } /* for */

  return (dev_hndl);
} /* usbhost_alloc_dev_hndl() */

/*======================================================================

FUNCTION usbhost_free_dev_hndl

DESCRIPTION
Frees a device instance device handle making it available for reuse.

DEPENDENCIES
None.

RETURN VALUE
void

SIDE EFFECTS
None.

=======================================================================*/
static void usbhost_free_dev_hndl
(
  usbhost_dev_entry_type * dev_entry,
  usbhost_dev_handle_type  dev_hndl
 )
{
  uint32 i;

  if (dev_hndl == USBHOST_INVALID_DEV_HNDL)
  {
    ERR("Invalid device handle %d for device instance %d, pdev_type=%d",
      dev_hndl, dev_entry->dev_inst_id, dev_entry->pdev_type);
    return;
  }

  /* Go over the handles of the given device entry */
  for (i = 0; i < USBHOST_MAX_DEV_HANDLES; i++)
  {
    /* Search the table -- an entry of USBHOST_INVALID_DEV_HNDL indicates an 
    ** available slot.
    */
    if (dev_entry->dev_hndl_tbl[i].dev_hndl == dev_hndl)
    {
      /* Found it. Now clear the handle. */
      dev_entry->dev_hndl_tbl[i].dev_hndl = USBHOST_INVALID_DEV_HNDL;
      dev_entry->dev_hndl_tbl[i].client_data = NULL;

      HSU_MSG_HIGH("Device Handle %d freed for device instance %d, pdev_type=%d",
        dev_hndl, dev_entry->dev_inst_id, dev_entry->pdev_type);
      break;
    }
  }
} /* usbhost_free_dev_hndl() */

/*======================================================================

FUNCTION usbhost_register

DESCRIPTION
Register function : the client uses this function to register with the API

Actions performed  :

1. add the client to the table

2. Check for devices of the same type ; if there are any connected then
call the callback function to notify the client of the presence of all
devices of that type.

Parameters :
pdev_type - Device type for which the client is registering
client_desc - A description string of the client
client_sigs - Set of signals specified by the client
callback - clients's callback function pointer used for notifying
the clients of device instances. 
param - Scratchpad pointer passed by the client. This is passed
back in every callback invocation.

DEPENDENCIES
None.

RETURN VALUE
Returns a unique client ID if registrations was successful, else
returns an invalid client ID. 
SIDE EFFECTS
None.

=======================================================================*/
usbhost_client_id_type usbhost_register
(
  /* Device type */
  usbhost_pdev_type pdev_type,

  /* Client description string */
  const char * client_desc,

  /* Set of signals and dog rpt function specified by the client */
  usbhost_task_table_type client_sigs,

  /* Callback function pointer for device notifications*/
  void (* dev_notify_cb)
  (
  /* Device type */
  usbhost_pdev_type dev_type,

  /* Device status - connected / disconnected */
  usbhost_dev_status_type dev_stat, 

  /* Device instance */
  usbhost_dev_instance_type dev_inst_id,

  /* Client Scratchpad */
  void *
  ),

  /* Void * pointer for client to specify a pointer */
  /* This pointer will be passed back on each callback function */
  void * param
)
{
  uint32 i;
  usbhost_client_entry_type *  client_ptr = NULL;
  usbhost_client_entry_type *  client_ptr_new = NULL;
  usbhost_client_id_type       client_id_num;

  HSU_MSG_HIGH("Starting usbhost_register(). pdev_type=%d.", pdev_type, 0, 0);

  /* Get the next client id number available if there is one */
  client_id_num = usbhost_alloc_client_id();
  
  if (client_id_num == USBHOST_INVALID_CLIENT_ID)
  {
    HSU_MSG_ERROR("usbhost_register(): Ran out of client IDs.",0,0,0);
    HSU_ASSERT("usbhost_register(): Ran out of client IDs."==NULL);
    return USBHOST_INVALID_CLIENT_ID;
  }

  if ((pdev_type >= USBHOST_MAX_PDEV_TYPE) ||
    (dev_notify_cb == NULL) ||
    (rex_is_in_irq_mode()))
  {
    HSU_MSG_ERROR("usbhost_register() : Invalid parameters / rex in IRQ mode",0,0,0);
    return USBHOST_INVALID_CLIENT_ID;
  }

  USBHOST_GLOBAL_LOCK();

  /* XXX - Feature enhancement - register for all devices */

  for (i=0; i<USBHOST_CLIENT_POOL_SIZE; i++)
  {
    if (usbhost_client_pool[i].entry_is_empty)
    {
      client_ptr_new = &usbhost_client_pool[i];
      break;
    }
  }

  if (client_ptr_new == NULL)
  {
    HSU_MSG_ERROR("usbhost_register() : no available entry in client pool.", 0, 0, 0);
    USBHOST_GLOBAL_UNLOCK();
    return USBHOST_INVALID_CLIENT_ID;
  }

  client_ptr_new->data->client_id      = client_id_num;
  client_ptr_new->data->pdev_type      = pdev_type;
  client_ptr_new->data->dev_notify_cb  = dev_notify_cb;
  client_ptr_new->data->open_notify_cb = NULL;
  client_ptr_new->data->param          = param;

  /* Save client specific signals and dog report function pointer */
  client_ptr_new->data->completion_sig = client_sigs.sem_wait_sig;
  client_ptr_new->data->dog_rpt_sig = client_sigs.dog_rpt_sig;
  client_ptr_new->data->dog_rpt_fn_ptr = client_sigs.dog_rpt_fn_ptr;
  client_ptr_new->data->registered_task = rex_self();

  if (client_desc != NULL) 
  {
#ifdef T_REXNT
#error code not present
#else
    (void) std_strlcpy(client_ptr_new->data->client_desc, client_desc,
      USBHOST_CLIENT_DESC_SIZE);
#endif
  }
  else 
  {
    client_ptr_new->data->client_desc[0] = '\0';
  }

  client_ptr_new->data->close_req_cb = NULL;
  client_ptr_new->next = NULL;

  /* Insert element in table at the tail of the list to ensure notifications 
  ** are in the same order the registrations are received */
  if (usbhost_client_table[pdev_type] == NULL)
  {
    usbhost_client_table[pdev_type] = client_ptr_new;
  }
  else
  {
    client_ptr = usbhost_client_table[pdev_type];

    while (client_ptr->next != NULL)
    {
      client_ptr = client_ptr->next;
    }

    /* we are at the last element of the list */  
    client_ptr->next = client_ptr_new;
  }

  /* client has been registered ; check if there are any devices of the type
  ** requested by the client ; if so call the callback function
  */
  for (i = 0; i < USBHOST_MAX_DEVICES; i++)
  {
    if ((!usbhost_dev_table[i].entry_is_empty) && 
        (usbhost_dev_table[i].pdev_type == pdev_type))
    {
      /* Call the device notification callback function */
      dev_notify_cb(pdev_type, 
                    USBHOST_DEV_CONNECTED,
                    usbhost_dev_table[i].dev_inst_id,
                    param);
    }
  }
  
  client_ptr_new->entry_is_empty = FALSE;
  USBHOST_GLOBAL_UNLOCK();

  HSU_MSG_HIGH("Exiting usbhost_register(). pdev_type=%d. client_id=%d.", 
               pdev_type, client_ptr_new->data->client_id, 0);
  return (client_ptr_new->data->client_id);
} /* usbhost_register() */

/*======================================================================

FUNCTION usbhost_enqueue_client

DESCRIPTION
If a client requests to open a device and some other client is using
the device, we add the requesting client to the pending_queue; this
will be dequeued from the pending_queue once usbhost_close is called
by the current client

Actions performed  :
Add client to the pending queue
Parameters :
dev_inst_id - Device instance ID
client   - Pointer to the client for client information

DEPENDENCIES
None.

RETURN VALUE

None.

SIDE EFFECTS

=======================================================================*/
static void usbhost_enqueue_client
(
  /* Device instance */
  usbhost_dev_instance_type dev_inst_id,

  /* Pointer to client for client information */
  const usbhost_client_entry_type * client
)
{
  uint32 i;
  usbhost_client_entry_type * client_ptr = 
                        usbhost_pending_open_queue[dev_inst_id];
  usbhost_client_entry_type * client_ptr_new = NULL;

  boolean client_present = FALSE;

  if ((dev_inst_id >= USBHOST_MAX_DEVICES) || (client == NULL))
  {
    HSU_MSG_ERROR("usbhost_enqueue_client() : Invalid parameters.", 0, 0, 0);
    return;  
  }

  while (client_ptr != NULL)
  {
    if (client_ptr->data->client_id == client->data->client_id) 
    {
      /* Don't enqueue the request if it's already in the pending queue */
      client_present = TRUE;
      break;
    }

    client_ptr = client_ptr->next;
  }

  if (client_present == FALSE)
  {
    /* We have the last element in the queue - add the new element after that*/
    /* First look for an available linked-list record */
    for (i=0; i<USBHOST_PENDING_CLIENT_POOL_SIZE; i++)
    {
      if (usbhost_pending_client_pool[i].entry_is_empty)
      {
        client_ptr_new = &usbhost_pending_client_pool[i];
        client_ptr_new->entry_is_empty = FALSE;
        break;
      }
    }

    if (client_ptr_new == NULL) 
    {
      HSU_MSG_ERROR("usbhost_enqueue_client(): usbhost_pending_client_pool is full.",
                    0,0,0);
    }
    else 
    {
      /* Don't allocate memory for data */
      client_ptr_new->data = client->data;
      client_ptr_new->next = NULL;

      if (usbhost_pending_open_queue[dev_inst_id] == NULL)
      {
        /* No client currently in the pending queue - add to the head */
        usbhost_pending_open_queue[dev_inst_id] = client_ptr_new;
      }
      else
      {
        client_ptr_new->next = usbhost_pending_open_queue[dev_inst_id];
        /* client currently in the pending queue - add New to the head */
        usbhost_pending_open_queue[dev_inst_id] = client_ptr_new;
      }
    }
  }
} /* usbhost_enqueue_client() */

/*======================================================================

FUNCTION usbhost_dequeue_client

DESCRIPTION
Remove a client entry from the pending queue.

Parameters :
dev_inst_id - Device instance ID
client   - Pointer to client for client info

DEPENDENCIES
None.

RETURN VALUE

None.

SIDE EFFECTS
None.

=======================================================================*/
static void usbhost_dequeue_client
(
  /* Device instance */
  usbhost_dev_instance_type         dev_inst_id,

  /* pointer to client for client information */
  const usbhost_client_entry_type * client
)
{
  usbhost_client_entry_type * client_ptr = 
                              usbhost_pending_open_queue[dev_inst_id];
  usbhost_client_entry_type * client_ptr_prev = NULL;

  if ((dev_inst_id >= USBHOST_MAX_DEVICES) || (client == NULL))
  {
    HSU_MSG_ERROR("usbhost_dequeue_client() : Invalid parameters.", 0, 0, 0);
    return;
  }

  while (client_ptr != NULL)
  {
    if (client_ptr->data->client_id == client->data->client_id)
    {
      /* Found the client in the pending queue - remove it */
      if (client_ptr_prev == NULL)
      {
        usbhost_pending_open_queue[dev_inst_id] = NULL;
      }
      else
      {
        client_ptr_prev->next = client_ptr->next;
      }

      client_ptr->entry_is_empty = TRUE;
      break;
    }

    client_ptr_prev = client_ptr;
    client_ptr = client_ptr->next;
  }
} /* usbhost_dequeue_client() */

/*======================================================================

FUNCTION usbhost_deregister

DESCRIPTION
Dergister function : the client uses this function to deregister with the API

Actions performed  :
1. Check if client exists in the table
If so, remove the client from the table ; free up memory
2. If the client is in the pending queue for the device type
it is deregistering for, then remove it.

Parameters : 
client_id  - client ID for the client assigned at registration time
pdev_type  - Device type for which the client is deregistering

DEPENDENCIES
None.

RETURN VALUE
USBHOST_SUCCESS - if deregistration was successful
USBHOST_ERROR   - if deregistration was not succesful


SIDE EFFECTS
None.

=======================================================================*/
usbhost_status_type usbhost_deregister
(
 /* Client ID */
 usbhost_client_id_type client_id,

 /* Device type */
 usbhost_pdev_type pdev_type
 )
{
  usbhost_client_entry_type  * client_ptr = NULL;
  usbhost_client_entry_type  * client_ptr_prev = NULL;
  uint32 i;

  HSU_MSG_HIGH("Starting usbhost_deregister(). client_id=%d. pdev_type=%d.", 
               client_id, pdev_type, 0);

  if ((pdev_type >= USBHOST_MAX_PDEV_TYPE) || (rex_is_in_irq_mode()))
  {
    HSU_MSG_ERROR("usbhost_deregister() : Invalid parameters", 0, 0, 0);
    return USBHOST_ERROR;
  }

  /* XXX - Future Enhancements - deregister for all devices */
  USBHOST_GLOBAL_LOCK();

  for (client_ptr = usbhost_client_table[pdev_type];
    client_ptr != NULL;
    client_ptr = client_ptr->next)
  {
    if (client_ptr->data->client_id == client_id)
    {
      break;
    }
    client_ptr_prev = client_ptr;
  }

  if (client_ptr == NULL) 
  {
    HSU_MSG_ERROR("usbhost_deregister(): Client not found.",0,0,0);
    USBHOST_GLOBAL_UNLOCK();
    /* We did not find the client */
    return USBHOST_ERROR;
  }

  /* found the client -  need to free up memory */

  if (client_ptr_prev == NULL)
  {
    /* We are at the head */
    usbhost_client_table[pdev_type] = client_ptr->next;
  }
  else
  {
    /* We found a client in the middle of the list */
    client_ptr_prev->next = client_ptr->next;
  }
  
  client_ptr->next = NULL;
  client_ptr->entry_is_empty = TRUE;

  /* We found the client in the client list - it may be in the pending queue 
  ** - remove it 
  */
  for (i = 0 ; i < USBHOST_MAX_DEVICES ; i++) 
  {
    if (usbhost_dev_table[i].pdev_type != pdev_type)
    {
      continue;
    }
    
    for (client_ptr = usbhost_pending_open_queue[i];
         client_ptr != NULL ; 
         client_ptr = client_ptr->next)
    {
      if (client_ptr->data->client_id == client_id)
      {   
        usbhost_dequeue_client(usbhost_dev_table[i].dev_inst_id, client_ptr);
      }
    }
  }

  USBHOST_GLOBAL_UNLOCK();

  HSU_MSG_HIGH("Exiting usbhost_deregister(). client_id=%d. pdev_type=%d.", 
    client_id, pdev_type, 0);
  return USBHOST_SUCCESS;
} /* usbhost_deregister() */

/*======================================================================

FUNCTION usbhost_req_open

DESCRIPTION
After the client has successfully registered and has been notified
of the the presence of a device of the same type, the client
uses this function to open a communication path with the device

Actions performed  :
1. Check if it's a valid device instance id
2. Check if the client has registered for the device type
3. If there is no other device then call the open_notify_fn ;
4. If there is another client using the device then
- remember that a client is waiting on a particular device
- send close_notify to the current client

Parameters :
client id - client ID for the client assigned at registration time
dev_inst_id  - Device instance for which the client is requesting to 
get a handle
open_notify_cb - Callback function specified by the client. This is
invoked by the API when it needs to notify the client
of the device being available or not available.            
close_req_cb - Callback function specified by the client. This is 
invoked by the API when it requests the client to close
a communication path (handle) with a device.

DEPENDENCIES
None.

RETURN VALUE
USBHOST_SUCCESS - if open was processed successfully
USBHOST_ERROR   - if open was not processed succesfully


SIDE EFFECTS
None.

=======================================================================*/
usbhost_status_type usbhost_req_open
(
 /* Client ID */
 usbhost_client_id_type client_id,

 /* Device Instance */
 usbhost_dev_instance_type dev_inst_id,

 /* Open callback function */
 usbhost_open_ret_type (* open_notify_cb)
 (
 /* Device instance for which the client is being notified */
 usbhost_dev_instance_type,

 /* Device handle allocated for the communication path */
 usbhost_dev_handle_type,

 /* status of notification - whether available or not */
 usbhost_open_status,

 /* If not available - the client description of the client
 which currently holds the handle and is not giving up the 
 handle */
 char *,

 /* Client scratchpad */
 void *
 ),

 /* Close callback function */
 void (* close_req_cb)
 (
 /* Device handle of the device the API is requesting the client to 
 close */
 usbhost_dev_handle_type,

 /* The client description of the client which is wanting to gain
 access to the device */
 char *,

 /* The client ID of the client which is wanting to gain access
 to the device */
 usbhost_client_id_type,

 /* Client scratchpad */
 void *
 )
 )
{
  usbhost_client_entry_type * client_ptr = NULL;
  usbhost_dev_entry_type *    dev_entry_ptr = NULL;
  usbhost_pdev_type           pdev_type;

  HSU_MSG_HIGH("Starting usbhost_req_open.",0,0,0);

  /* Must specify a close req callback function */
  if ((close_req_cb == NULL)   || 
    (open_notify_cb == NULL) ||
    (dev_inst_id >= USBHOST_MAX_DEVICES) ||
    (rex_is_in_irq_mode()))
  {
    HSU_MSG_ERROR("usbhost_req_open() : Invalid paramaters or in IRQ mode", 0, 0, 0); 
    return USBHOST_ERROR;
  }

  /* Check to see if it's a valid dev_inst */
  if (usbhost_dev_table[dev_inst_id].entry_is_empty)
  {
    /* There is no device with this instance id */
    HSU_MSG_ERROR("usbhost_req_open() : No device for given device instance %d.", 
                  dev_inst_id, 0, 0); 
    return USBHOST_ERROR;
  }

  dev_entry_ptr = &usbhost_dev_table[dev_inst_id];

  /* Loop through all clients who have registered for this to see 
  if the current client is there */
  USBHOST_GLOBAL_LOCK();

  pdev_type = dev_entry_ptr->pdev_type;
  HSU_ASSERT(pdev_type < USBHOST_MAX_PDEV_TYPE);
  for (client_ptr = usbhost_client_table[pdev_type];
       client_ptr != NULL;
       client_ptr=client_ptr->next)
  {
    if (client_ptr->data->client_id == client_id)
    {
      break;
    }
  }

  if (client_ptr == NULL)
  {
    HSU_MSG_ERROR("usbhost_req_open() : No client with client_id=%d is registered for pdev_type=%d.",
                  client_id, pdev_type, 0); 

    USBHOST_GLOBAL_UNLOCK();

    /* We didn't find the client registered */
    return USBHOST_ERROR;
  }
  else
  {
    /* We found the client - memorize the open and close notify functions */
    client_ptr->data->open_notify_cb = open_notify_cb;
    client_ptr->data->close_req_cb = close_req_cb;
  }

  if (dev_entry_ptr->curr_client == NULL)
  {
    usbhost_dev_handle_type dev_hndl;
    usbhost_open_status     dev_open_stat = USBHOST_OPEN_AVAILABLE;

    /* curr_client must be set before the callback is called because the client
    ** may immediately call another API function (e.g. ioctl()), which then checks
    ** to ensure that the device entry's curr_client pointer is set.
    */
    dev_entry_ptr->curr_client = client_ptr;

    /* Allocate a device handle if one is available */
    if ((dev_hndl = usbhost_alloc_dev_hndl(dev_entry_ptr, client_ptr)) == 
        USBHOST_INVALID_DEV_HNDL)
    {
      dev_open_stat = USBHOST_OPEN_NO_HANDLES;
    }
    else
    {
      dev_entry_ptr->curr_open_hndl = dev_hndl;
    }

    HSU_MSG_HIGH("REQ_OPEN performed by client %d on device instance %d, handle=%d",
      client_id, dev_inst_id, dev_hndl);

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    /* No other client  - we can call the open_notify_cb*/
    (void) client_ptr->data->open_notify_cb(dev_inst_id,
      dev_hndl,
      dev_open_stat,
      (char *) NULL,
      client_ptr->data->param);
  }
  else
  {
    /* Check to see if the client already has the device open */
    if (dev_entry_ptr->curr_client->data->client_id == client_id) 
    {
      USBHOST_GLOBAL_UNLOCK();
      return USBHOST_ERROR;
    }

    usbhost_enqueue_client(dev_inst_id, client_ptr);

    /* Notify current user of the device */
    dev_entry_ptr->curr_client->data->close_req_cb(dev_entry_ptr->curr_open_hndl,
      client_ptr->data->client_desc,
      client_id,
      dev_entry_ptr->curr_client->data->param);
  }

  USBHOST_GLOBAL_UNLOCK();

  HSU_MSG_HIGH("Exiting usbhost_req_open.",0,0,0);
  return USBHOST_SUCCESS;
} /* usbhost_req_open() */

/*======================================================================

FUNCTION usbhost_close_req_rsp

DESCRIPTION
This function is called by the client to notify the API whether
it is willing to relinquish control of the device or not.

Parameters :
client_id  - client ID for the client assigned at registration time
client_id_close - client ID for the client requesting access to the
device. 
dev_hndl   - Device handle
response   - Client's response to the API whether to relinquish control
of the device or not.  

DEPENDENCIES

RETURN VALUE
USBHOST_SUCCESS - if notification was valid 
USBHOST_ERROR   - if notification was invalid

SIDE EFFECTS
None.

=======================================================================*/
usbhost_status_type usbhost_close_req_rsp
(
 /* Client ID */
 usbhost_client_id_type client_id,
 /* Client ID of the client requesting access to the device */
 usbhost_client_id_type client_id_close,
 /* Device handle */
 usbhost_dev_handle_type dev_hndl,
 /* Client's response to the API */
 usbhost_close_rsp_type response
 )
{
  usbhost_client_entry_type * client_ptr;
  usbhost_dev_entry_type *    dev_entry_ptr;

  HSU_MSG_HIGH("Starting usbhost_close_req_rsp.",0,0,0);

  /* First we need to map the device handle back to the device instance */
  dev_entry_ptr = usbhost_map_hndl_to_inst(client_id, dev_hndl);

  if ((dev_entry_ptr == NULL) || (rex_is_in_irq_mode()))
  {
    HSU_MSG_ERROR("usbhost_close_req_rsp() : Invalid parameters/ REX in IRQ mode", 0, 0, 0);
    return USBHOST_ERROR;
  }

  HSU_MSG_HIGH("CLOSE_REQ_RSP from client %d on device instance %d, typ=%d",
    client_id, dev_entry_ptr->dev_inst_id, response);

  USBHOST_GLOBAL_LOCK();

  if (response == USBHOST_CLOSE_RSP_RELEASE)
  {
    /* Do nothing - state machine advances only when the client calls 
    usbhost_close */
  }
  else if (response == USBHOST_CLOSE_RSP_RETAIN) 
    /* Client holding the device has decided not to relinquish control */
  {
    usbhost_dev_instance_type dev_inst_id;
    usbhost_open_ret_type     open_ret;

    /* Need the device instance id to index the pending queue */
    dev_inst_id = dev_entry_ptr->dev_inst_id;

    /* the notify function may not be for the head of the queue 
    - find the appropriate client and call it's open_notify_fn */
    for (client_ptr = usbhost_pending_open_queue[dev_inst_id]; 
         client_ptr != NULL; 
         client_ptr = client_ptr->next)
    {
      if (client_ptr->data->client_id == client_id_close) 
      {
        /* Found the client that asked the calling client 
        to relinquish control */
        open_ret = client_ptr->data->open_notify_cb(
                                  dev_inst_id,
                                  USBHOST_INVALID_DEV_HNDL,
                                  USBHOST_OPEN_NOT_AVAILABLE, 
                                  dev_entry_ptr->curr_client->data->client_desc,
                                  client_ptr->data->param);

        if (open_ret == USBHOST_OPEN_RET_UNSCHEDULE)
        {
          /* The client has requested not to enqueue it's request - 
          dequeue it from the pending queue*/
          usbhost_dequeue_client(dev_entry_ptr->dev_inst_id,
                                 client_ptr);
        }
        else if (open_ret == USBHOST_OPEN_RET_RESCHEDULE) 
        {
          /* The client has requested to keep it's request enqueued - 
          ** do nothing
          */
        }
      }  
    } /* for */
  }

  USBHOST_GLOBAL_UNLOCK();

  HSU_MSG_HIGH("Exiting usbhost_close_req_rsp.",0,0,0);
  return USBHOST_SUCCESS;
} /* usbhost_close_req_rsp() */

/*======================================================================

FUNCTION usbhost_close

DESCRIPTION
After the client has successfully registered and has a
communication path open - it can choose to close the
communication path

Actions performed  :
1. Check if it's a valid client
If so set the curr_client to NULL ;
2. See if there is any existing client waiting for the device ;
pick the first in queue and call it's open_notify function

Parameters : 
client_id - client ID for the client assigned at registration time
dev_hndl  - Device handle of the device the client is closing


DEPENDENCIES
None.

RETURN VALUE
USBHOST_SUCCESS - if close was successful
USBHOST_ERROR   - if close was not successful


SIDE EFFECTS
None.

=======================================================================*/
usbhost_status_type usbhost_close
(
 /* Client ID */
 usbhost_client_id_type  client_id,

 /* Device handle */
 usbhost_dev_handle_type dev_hndl
 )
{
  usbhost_client_entry_type * poq_client_ptr;
  usbhost_dev_entry_type *    dev_entry_ptr;

  HSU_MSG_HIGH("Starting usbhost_close().",0,0,0);

  USBHOST_GLOBAL_LOCK();

  /* First we need to map the device handle back to the device instance */
  dev_entry_ptr = usbhost_map_hndl_to_inst(client_id, dev_hndl);

  if ((dev_entry_ptr == NULL) || (rex_is_in_irq_mode()))
  {
    HSU_MSG_ERROR("usbhost_close() : Invalid paramaters / REX in IRQ mode ", 0, 0, 0);
    USBHOST_GLOBAL_UNLOCK();
    return USBHOST_ERROR;
  }

  HSU_MSG_HIGH("CLOSE by client %d on device instance %d",
    client_id, dev_entry_ptr->dev_inst_id, 0);

  /* Free up the device handle */
  usbhost_free_dev_hndl(dev_entry_ptr, dev_hndl);

  dev_entry_ptr->curr_client = NULL;

  /* See if there are any pending requests - if so then call the 
  open_notify function for the first request in the queue */
  poq_client_ptr = usbhost_pending_open_queue[dev_entry_ptr->dev_inst_id];

  if (poq_client_ptr != NULL) 
  {
    usbhost_client_entry_type * client_ptr;
    usbhost_dev_handle_type     poq_dev_hndl;
    usbhost_open_status         poq_dev_open_stat = USBHOST_OPEN_AVAILABLE;

    /* Allocate a device handle if one is available */
    poq_dev_hndl = usbhost_alloc_dev_hndl(dev_entry_ptr, poq_client_ptr);
    if (poq_dev_hndl == USBHOST_INVALID_DEV_HNDL)
    {
      poq_dev_open_stat = USBHOST_OPEN_NO_HANDLES;
    }
    else
    {
      dev_entry_ptr->curr_open_hndl = poq_dev_hndl;
    }

    HSU_MSG_HIGH("usbhost_close(): Calling open_notify_cb for client %d on device instance %d, handle=%d",
      client_id, dev_entry_ptr->dev_inst_id, poq_dev_hndl);

    (void) poq_client_ptr->data->open_notify_cb(dev_entry_ptr->dev_inst_id,
      poq_dev_hndl,                       
      poq_dev_open_stat,
      NULL, 
      poq_client_ptr->data->param);

    /* Set the current client in the device entry to this client. 
    ** We'll need to find the right client_ptr from the clients registered 
    */
    for (client_ptr = usbhost_client_table[dev_entry_ptr->pdev_type]; 
         client_ptr != NULL;
         client_ptr = client_ptr->next)
    {
      if (client_ptr->data->client_id == poq_client_ptr->data->client_id)
      {
        /* Change the curr client in the device table */
        usbhost_dev_table[dev_entry_ptr->dev_inst_id].curr_client = client_ptr;
      }
    }

    /* If we find another client pending at this point then we call the 
    client that's gaining access now with it's close notify to attempt to
    close it right away */
    if (poq_client_ptr->next != NULL)
    {
      poq_client_ptr->data->close_req_cb(poq_dev_hndl,
        poq_client_ptr->next->data->client_desc,
        poq_client_ptr->next->data->client_id,
        poq_client_ptr->data->param);
    }

    usbhost_dequeue_client(dev_entry_ptr->dev_inst_id, poq_client_ptr);
  }

  USBHOST_GLOBAL_UNLOCK();

  HSU_MSG_HIGH("Exiting usbhost_close().",0,0,0);
  return USBHOST_SUCCESS;
} /* usbhost_close() */

/*======================================================================

FUNCTION usbhost_ioctl

DESCRIPTION
IOCTL function to communicate with the device

Parameters :
client_id - client ID for the client assigned at registration time
dev_hndl  - Device handle 
command   - The device specific IOCTL command
parm      - IOCTL specific parameter 

DEPENDENCIES
None.

RETURN VALUE
USBHOST_SUCCESS - if ioctl was successful
USBHOST_ERROR   - if ioctl was not successful
Other device specific return types

SIDE EFFECTS
None.

=======================================================================*/
usbhost_status_type usbhost_ioctl
(
  /* Client ID */
  usbhost_client_id_type   client_id,
  /* Device handle */
  usbhost_dev_handle_type  dev_hndl,
  /* Device specific command */
  usbhost_ioctl_cmd_type   command,
  /* IOCTL specific parameter */
  void *                   parm
)
{
  usbhost_status_type result;
  usbhost_dev_entry_type* dev_entry_ptr = NULL;

  if (rex_is_in_irq_mode())
  {
    HSU_MSG_ERROR("usbhost_ioctl() : REX in IRQ mode ", 0, 0, 0);
    return USBHOST_ERROR;
  }
  USBHOST_GLOBAL_LOCK();

  /* First we need to map the device handle back to the device instance */
  dev_entry_ptr = usbhost_map_hndl_to_inst(client_id, dev_hndl);
  
  /* Note: In the FS-USB implementation, this is called only after
  ** handling the IOCTL. In order to allow concurrent IOCTL calls,
  ** we need to unlock here. However, this might be a problem if
  ** a different task of the same client closes the handle.
  ** Should we bother with handling this case???
  */
  USBHOST_GLOBAL_UNLOCK();

  if (dev_entry_ptr == NULL)
  {
    HSU_MSG_ERROR("usbhost_ioctl() : No device found for client_id=%d, dev_hndl=%d.",
                  client_id, dev_hndl, 0);
    return USBHOST_ERROR;
  }

  /* Make sure no other IOCTL call for the same device gets through, and also
  ** that the device entry is not removed while we're working.
  */
  rex_enter_crit_sect(&dev_entry_ptr->device_lock);

  result = USBHOST_ERROR;

  switch (command) 
  {
    case USBHOST_IOCTL_GET_DEV_INFO:
      /* This is the only IOCTL command which is not specific to a device type.
      */
      HSU_ASSERT(usbhost_get_usbd_handle_functions[dev_entry_ptr->pdev_type] != NULL);
      if (usbhost_get_usbd_handle_functions[dev_entry_ptr->pdev_type] != NULL  
 #ifdef FEATURE_BT_EXTPF_HID_HOST 
#error code not present
 #else
         )
 #endif /* FEATURE_BT_EXTPF_HID_HOST */
      {
        void* usbd_handle = 
          usbhost_get_usbd_handle_functions[dev_entry_ptr->pdev_type](dev_entry_ptr);

        if (usbd_handle == NULL)
        {
          HSU_MSG_ERROR("usbhost_ioctl(): NULL usbd handle.",0,0,0);
        }
        else
        {
          usb_device_descriptor_t desc;
          usbd_status status;

          jsafe_enter();
          status = usbd_get_device_desc((usbd_device_handle)usbd_handle, 
                                        &desc);
          jsafe_leave();

          if (status != USBD_NORMAL_COMPLETION)
          {
            HSU_MSG_ERROR("usbhost_ioctl(): usbd_get_device_desc returned %d.",status,0,0);
          }
          else
          {
            usbhost_ioctl_devInfo_type * devInfo_parm = 
              (usbhost_ioctl_devInfo_type *) parm;
            HSU_ASSERT(devInfo_parm != NULL);

            devInfo_parm->d_vendorID = UGETW(desc.idVendor);
            devInfo_parm->d_productID = UGETW(desc.idProduct);
            devInfo_parm->d_manufacturerID = desc.iManufacturer;
            devInfo_parm->d_class = desc.bDeviceClass;
            devInfo_parm->d_subclass = desc.bDeviceSubClass;
            devInfo_parm->d_protocol = desc.bDeviceProtocol;

            result = USBHOST_SUCCESS;
          }
        }
      }
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
      break;
    default:
      /* Forward the command to the appropriate handler. */
      HSU_ASSERT(usbhost_ioctl_functions[dev_entry_ptr->pdev_type] != NULL);
      if (usbhost_ioctl_functions[dev_entry_ptr->pdev_type] != NULL)
      {
        result = usbhost_ioctl_functions[dev_entry_ptr->pdev_type](
                                                                dev_entry_ptr,
                                                                command,
                                                                parm);
      }
  }

  rex_leave_crit_sect(&dev_entry_ptr->device_lock);
  return result;
} /* usbhost_ioctl() */

/*======================================================================

FUNCTION usbhost_notify_clients

DESCRIPTION
Once a device is connected/disconnected ; call the
callback functions for all the clients currently registered to
use the device.

Parameters :
client_id - pdev_type 
dev_inst_id - Device instance identifier
status    - status event that occurred - device inserted / removed. 

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

=======================================================================*/
static void usbhost_notify_clients
(
 /* Device type */
 usbhost_pdev_type pdev_type ,
 /* Device handle */
 usbhost_dev_instance_type dev_inst_id,
 /* Status event type for the device - device connected / disconnected */
 usbhost_dev_status_type status)
{
  usbhost_client_entry_type  * client_ptr = NULL;

  for (client_ptr = usbhost_client_table[pdev_type]; 
    client_ptr != NULL;
    client_ptr=client_ptr->next)
  {
    if ((client_ptr->data != NULL) && 
      (client_ptr->data->pdev_type == pdev_type) &&
      (client_ptr->data->dev_notify_cb != NULL))
    { 
      client_ptr->data->dev_notify_cb(pdev_type,
        status,
        dev_inst_id,
        client_ptr->data->param);
    }
  }
} /* usbhost_notify_clients() */

/*****************************************************************************
** Functions used by the adaptation layers.
******************************************************************************/
/*===========================================================================

FUNCTION usbhost_set_ioctl_func

DESCRIPTION
This function sets the function pointer to the function
that handles IOCTLs for a specific device types. This function
should be called by the adaptation layer that handles each device type.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void usbhost_set_ioctl_func
(
  /* Physical device type (e.g., mass storage) */
  usbhost_pdev_type pdev_type, 
  /* Pointer to a function that handles IOCTL commands
  ** for devices of the specified type.
  */
  usbhost_ioctl_func_type func
)
{
  HSU_ASSERT(pdev_type < (usbhost_pdev_type)USBHOST_MAX_PDEV_TYPE);
  usbhost_ioctl_functions[pdev_type] = func;
}

/*===========================================================================

FUNCTION usbhost_set_usbd_handle_func

DESCRIPTION
This function sets the function pointer to the function
that retrieves the usbd handle for a device.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void usbhost_set_usbd_handle_func
(
 /* Physical device type (e.g., mass storage) */
 usbhost_pdev_type pdev_type, 
 /* Pointer to a function */
 usbhost_get_usbd_handle_func_type func
)
{
  HSU_ASSERT(pdev_type < (usbhost_pdev_type)USBHOST_MAX_PDEV_TYPE);
  usbhost_get_usbd_handle_functions[pdev_type] = func;
}

/*===========================================================================

FUNCTION usbhost_add_device

DESCRIPTION
This function adds an entry to the device table. It is called by each 
adaptation layer that receives a notification of a new device from the stack.

DEPENDENCIES
None.

RETURN VALUE
If successful, a pointer to the new entry. Otherwise, NULL.

SIDE EFFECTS
None.
===========================================================================*/
usbhost_dev_entry_type* usbhost_add_device
(
  usbhost_dev_type dev_type,
  usbhost_pdev_type pdev_type,
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
  void* class_specific_data
)
{
  uint32 i,j;

  USBHOST_GLOBAL_LOCK();

  /* Find the first open slot */
  for (i = 0 ; i < USBHOST_MAX_DEVICES; i++)
  {
    if (usbhost_dev_table[i].entry_is_empty)
    {
      break;
    }
  }

  if (i == USBHOST_MAX_DEVICES)
  {
    /* Ignore registering this device - there cannot be so many devices 
    connected */
    USBHOST_GLOBAL_UNLOCK();
    return NULL;
  }

  usbhost_dev_table[i].entry_is_empty = FALSE;

  memset(&usbhost_dev_table[i], 0, sizeof(usbhost_dev_entry_type));

  usbhost_dev_table[i].dev_inst_id = (usbhost_dev_instance_type) i;
  usbhost_dev_table[i].curr_client = NULL;
  usbhost_dev_table[i].dev_type    = dev_type;
  usbhost_dev_table[i].pdev_type   = pdev_type;
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
  rex_init_crit_sect(&usbhost_dev_table[i].device_lock);
  usbhost_dev_table[i].class_specific_data = class_specific_data;

  for (j=0; j<USBHOST_MAX_DEV_HANDLES; j++)
  {
    usbhost_dev_table[i].dev_hndl_tbl[j].dev_hndl = USBHOST_INVALID_DEV_HNDL;
  }
  
  /* check if there are any clients registered for this 
  - if so then call the callback function */
  usbhost_notify_clients(usbhost_dev_table[i].pdev_type, 
                         usbhost_dev_table[i].dev_inst_id,
                         USBHOST_DEV_CONNECTED);

  USBHOST_GLOBAL_UNLOCK();
  return &usbhost_dev_table[i];
}

/*===========================================================================

FUNCTION usbhost_remove_device

DESCRIPTION
This function removes an entry from the device table. It is called by each 
adaptation layer that receives a notification (from the stack) of the removal
of a device.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void usbhost_remove_device
(
  usbhost_dev_entry_type* dev_entry_ptr
)
{
  usbhost_dev_instance_type   dev_inst_id;
  usbhost_client_entry_type * client_ptr = NULL;
  usbhost_client_entry_type * client_ptr_temp = NULL;

  HSU_ASSERT(dev_entry_ptr != NULL);
  HSU_ASSERT(dev_entry_ptr->entry_is_empty == FALSE);

  USBHOST_GLOBAL_LOCK();
  rex_enter_crit_sect(&dev_entry_ptr->device_lock);

  dev_inst_id = dev_entry_ptr->dev_inst_id;
  /* Call the callback function for all clients registered for this device */
  usbhost_notify_clients(dev_entry_ptr->pdev_type, 
                         dev_inst_id,
                         USBHOST_DEV_DISCONNECTED);

  /* Release the device table entry and make the slot available */
  dev_entry_ptr->entry_is_empty = TRUE;

  /* clear up the pending queue if there are any clients currently 
  waiting for a response from the device manager */
  if (usbhost_pending_open_queue[dev_inst_id] != NULL)
  {
    client_ptr = usbhost_pending_open_queue[dev_inst_id];

    usbhost_pending_open_queue[dev_inst_id] = NULL;

    while (client_ptr != NULL) 
    {
      client_ptr_temp = client_ptr->next;
      client_ptr->entry_is_empty = TRUE;
      client_ptr->data = NULL;
      client_ptr->next = NULL;
      client_ptr = client_ptr_temp;
    }
  }
  
  rex_leave_crit_sect(&dev_entry_ptr->device_lock);
  USBHOST_GLOBAL_UNLOCK();
}
