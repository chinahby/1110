/*==============================================================================

High Speed USB Host Mass Storage Adaptation Layer

GENERAL DESCRIPTION
Implementation of the Host Mass Storage Adaptation Layer.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when      who     what, where, why
--------  ---     --------------------------------------------------------------
05/07/06  sk      Initial version

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_host_al_ms.h"
#include "hsu_host_api_i.h"
#include "uw_host_mass.h"
#include "hsu_common.h"
#include "usbhost_api.h"
#include "jerrno.h"
#include "port.h"
#include "task.h"
#include "hsu_al_task.h"

#ifdef FEATURE_HS_USB_HOST_MS_PERF
#error code not present
#endif
#ifndef FEATURE_HSU_TEST
#include "event.h"
#include "hsu_event_defs.h"
#endif /*FEATURE_HSU_TEST*/

#define HSU_HOST_AL_MS_ENTER_SUSPEND_TIMEOUT  6000

/* Callbacks passed to disk_register_handler() */
static struct disk_handle_cb hsu_host_al_ms_disk_callbacks;

typedef enum
{
  HSU_HOST_AL_MS_MEDIA_NOT_PRESENT,
  HSU_HOST_AL_MS_MEDIA_PRESENT,
  HSU_HOST_AL_MS_MEDIA_CHANGED
}
hsu_host_al_ms_media_state_type;

/* Class-specific data for a mass storage device. A pointer to this type
** is stored in the device entry.
*/
typedef struct hsu_host_al_ms_device_data_s
{
  void*                           dev;
  uint8                           device_type;
  boolean                         media_is_removable;
  hsu_host_al_ms_media_state_type media_state;
  jresult_t                       current_transfer_status;
  rex_tcb_type*                   current_transfer_calling_task;
  uint32                          current_transfer_completion_sig;
  boolean                         lun_is_suspended;
  rex_timer_type                  enter_suspend_timer;
  uint8                           xfers_counter;
}
hsu_host_al_ms_device_data_type;

/* Pool of hsu_host_al_ms_device_data_type structures,
** to be used by connected devices 
*/
static hsu_host_al_ms_device_data_type 
  hsu_host_al_ms_device_data_pool[USBHOST_MAX_DEVICES];
static boolean 
  hsu_host_al_ms_device_data_pool_entry_is_free[USBHOST_MAX_DEVICES];
static rex_crit_sect_type hsu_host_al_ms_data_structures_lock;

/*===========================================================================

FUNCTION hsu_host_al_ms_device_attach

DESCRIPTION
This is the callback function called by the mass storage class driver
when a mass storage device is attached. It is called once for each LUN.

DEPENDENCIES
None.

RETURN VALUE
0 for success, JENOMEM when an entry cannot be added to the device table.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t hsu_host_al_ms_device_attach
(
  void *dev,
  void **context
)
{
  hsu_host_al_ms_device_data_type* dev_data_ptr;
  uint32                           dev_data_index, i;
  usbhost_dev_entry_type*          dev_entry_ptr;
  struct device_info                device_info_instance;
  jresult_t                        result;
#ifndef FEATURE_HSU_TEST
  uint16  dev_event_payload; /* help variable for event_report_payload */ 
#endif /*FEATURE_HSU_TEST*/


  HSU_MSG_HIGH("Starting hsu_host_al_ms_device_attach().",0,0,0);

  /* Find a free entry for the class-specific data */

  rex_enter_crit_sect(&hsu_host_al_ms_data_structures_lock);
  
  dev_data_index = 0;
  dev_data_ptr = NULL;
  for (i=0; i<USBHOST_MAX_DEVICES; i++)
  {
    if (hsu_host_al_ms_device_data_pool_entry_is_free[i])
    {
      dev_data_index = i;
      hsu_host_al_ms_device_data_pool_entry_is_free[i] = FALSE;
      dev_data_ptr = &hsu_host_al_ms_device_data_pool[i];
      dev_data_ptr->lun_is_suspended = FALSE;
      dev_data_ptr->xfers_counter = 0;
      break;
    }
  }

  rex_leave_crit_sect(&hsu_host_al_ms_data_structures_lock);

  if (dev_data_ptr == NULL)
  {
    HSU_MSG_ERROR("hsu_host_al_ms_device_attach(): No free entry in hsu_host_al_ms_device_data_pool",
                  0,0,0);
#ifndef FEATURE_HSU_TEST
    dev_event_payload = (uint16)HS_USB_CANT_ADD_MS;
    event_report_payload(EVENT_HS_USB_OPERATIONAL_ERROR, 
          sizeof(uint16),(void*)(&dev_event_payload));
#endif /*FEATURE_HSU_TEST*/
    return JENOMEM;
  }

  dev_data_ptr->dev = dev;
  dev_data_ptr->media_state = HSU_HOST_AL_MS_MEDIA_NOT_PRESENT;

  (void)rex_set_timer(&(dev_data_ptr->enter_suspend_timer), 
    HSU_HOST_AL_MS_ENTER_SUSPEND_TIMEOUT);

  /* Get the device type and check if it has removable media */
  result = disk_get_device_info(dev_data_ptr->dev, &device_info_instance);
  if (result != 0)
  {
    HSU_MSG_ERROR("hsu_host_al_ms_device_attach(): disk_get_device_info returned error number %d",
      result,0,0);
    /* No point in adding this device - we'll free up the data and return */
    hsu_host_al_ms_device_data_pool_entry_is_free[dev_data_index] = TRUE;
    return result;
  }
  dev_data_ptr->device_type = device_info_instance.devtype;
  dev_data_ptr->media_is_removable = device_info_instance.removable;

  /* Create a new device entry and add it to the device
  ** table maintained by the host api module.
  */
  jsafe_leave();
  dev_entry_ptr = usbhost_add_device(USBHOST_DEV_TYPE_BLOCK, 
                                     USBHOST_PDEV_TYPE_MASS_STORAGE,
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
                                     dev_data_ptr);
  jsafe_enter();

  if (dev_entry_ptr == NULL)
  {
    /* Free up the data */
    HSU_MSG_ERROR("hsu_host_al_ms_device_attach(): usbhost_add_device() returned NULL.",
                  0,0,0);
    hsu_host_al_ms_device_data_pool_entry_is_free[dev_data_index] = TRUE;
#ifndef FEATURE_HSU_TEST
    dev_event_payload = (uint16)HS_USB_CANT_ADD_MS;
    event_report_payload(EVENT_HS_USB_OPERATIONAL_ERROR, 
          sizeof(uint16), (void*)(&dev_event_payload));
#endif /*FEATURE_HSU_TEST*/
    return JENOMEM;
  }

  /* We use the pointer to the device  entry to represent the device.
  ** The context is passed to the disk_detach() callback, and
  ** this enables us to easily handle the detachment.
  */
  *context = (void*)dev_entry_ptr;
  
#ifndef FEATURE_HSU_TEST
  /* Send debug event on success */
  event_report(EVENT_HS_USB_MSD_CONNECT) ;
#endif /*FEATURE_HSU_TEST*/
  
  HSU_MSG_HIGH("Exiting hsu_host_al_ms_device_attach().",0,0,0);
  return 0;
}

/*===========================================================================

FUNCTION hsu_host_al_ms_device_detach

DESCRIPTION
This is the callback function called by the mass storage class driver
when a mass storage device is detached. It is called once for each LUN.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_ms_device_detach
(
  void *dev,
  
  /* The context set for the device by the adaptation layer
  ** in the disk_attach() callback.
  */
  void *context
)
{
  uint32 i;
  usbhost_dev_entry_type* dev_entry_ptr = (usbhost_dev_entry_type*)(context);
  hsu_host_al_ms_device_data_type* dev_data_ptr = NULL;

  HSU_MSG_HIGH("Starting hsu_host_al_ms_device_detach().",0,0,0);

  dev_data_ptr = (hsu_host_al_ms_device_data_type*)dev_entry_ptr->class_specific_data;

  HSU_USE_PARAM(dev);
  HSU_PARAM_COULD_BE_CONST(context);
  
  /* stop the suspend timer */
  rex_clr_timer(&(dev_data_ptr->enter_suspend_timer));

  usbhost_remove_device(dev_entry_ptr);

  /* Free up the class-specific data */
  for (i=0; i<USBHOST_MAX_DEVICES; i++)
  {
    if (dev_data_ptr == &hsu_host_al_ms_device_data_pool[i])
    {
      hsu_host_al_ms_device_data_pool_entry_is_free[i] = TRUE;
      break;
    }
  }
  HSU_MSG_HIGH("Exiting hsu_host_al_ms_device_detach().",0,0,0);
#ifndef FEATURE_HSU_TEST
  /* Send debug event on success */
  event_report(EVENT_HS_USB_MSD_DISCONECT ) ;
#endif /*FEATURE_HSU_TEST*/
}

/*===========================================================================

FUNCTION hsu_host_al_ms_media_insert

DESCRIPTION
This is the callback function called by the mass storage class driver
when media is inserted into a mass storage device

DEPENDENCIES
None.

RETURN VALUE
0 for success, JENOMEM when an entry cannot be added to the device table.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t hsu_host_al_ms_media_insert
(
 void *dev,
 void **context
)
{
  /* The context was set by the device_attach callback.
  ** *context is a pointer to the device entry.
  */
  usbhost_dev_entry_type* dev_entry_ptr;
  hsu_host_al_ms_device_data_type* dev_data_ptr;
 
  HSU_MSG_HIGH("Starting hsu_host_al_ms_media_insert().",0,0,0);

  HSU_USE_PARAM(dev);
  HSU_PARAM_COULD_BE_CONST(context);

  dev_entry_ptr = (usbhost_dev_entry_type*)(*context);

  HSU_ASSERT(dev_entry_ptr!=NULL);

  dev_data_ptr = 
    (hsu_host_al_ms_device_data_type*)dev_entry_ptr->class_specific_data;

  /* Update media state */
  /* No need for synchronization here, since the media_remove callback 
  ** cannot be called until this one returns, and the ioctl function
  ** only changes the state if it is HSU_HOST_AL_MS_MEDIA_CHANGED.
  */
  if (dev_data_ptr->media_state == HSU_HOST_AL_MS_MEDIA_NOT_PRESENT)
  {
    dev_data_ptr->media_state = HSU_HOST_AL_MS_MEDIA_CHANGED;
  }

  HSU_MSG_HIGH("Exiting hsu_host_al_ms_media_insert().",0,0,0);
  return 0;
}

/*===========================================================================

FUNCTION hsu_host_al_ms_media_remove

DESCRIPTION
This is the callback function called by the mass storage class driver
when media is removed from a mass storage device

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_ms_media_remove
(
 void *dev,
 void *context
 )
{
  /* The context was set by the device_attach callback.
  ** *context is a pointer to the device entry.
  */
  usbhost_dev_entry_type* dev_entry_ptr;
  hsu_host_al_ms_device_data_type* dev_data_ptr;

  HSU_MSG_HIGH("Starting hsu_host_al_ms_media_remove().",0,0,0);

  HSU_USE_PARAM(dev);
  HSU_PARAM_COULD_BE_CONST(context);

  dev_entry_ptr = (usbhost_dev_entry_type*)context;
  dev_data_ptr = 
    (hsu_host_al_ms_device_data_type*)dev_entry_ptr->class_specific_data;

  /* Update media state */
  /* This needs to be protected, because the ioctl function may change
  ** the state from HSU_HOST_AL_MS_MEDIA_CHANGED to HSU_HOST_AL_MS_MEDIA_PRESENT.
  */
  rex_enter_crit_sect(&hsu_host_al_ms_data_structures_lock);
  dev_data_ptr->media_state = HSU_HOST_AL_MS_MEDIA_NOT_PRESENT;
  rex_leave_crit_sect(&hsu_host_al_ms_data_structures_lock);

  HSU_MSG_HIGH("Exiting hsu_host_al_ms_media_remove().",0,0,0);
}

/*===========================================================================

FUNCTION safe_disk_suspend_resume

DESCRIPTION
Wrapper function that locks the giant mutex, calls disk_suspend() 
or disk_resume(),depending on the suspend parameter,
and unlocks the giant mutex.

RETURN VALUE
The result of the call to disk_suspend() or disk_resume().

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t safe_disk_suspend_resume(void *dev, boolean suspend)
{
  jresult_t result = HSU_JSUCCESS;

  HSU_MSG_HIGH("safe_disk_suspend_resume() - Entry.",0,0,0);

#ifndef T_REXNT
  jsafe_enter();
  if (suspend)
  {
    result = disk_suspend(dev);
  }
  else
  {
    result = disk_resume(dev);
  }
  jsafe_leave();
#endif

  HSU_MSG_HIGH("safe_disk_suspend_resume() - Exit.",0,0,0);
  return result;
} /* safe_disk_suspend_resume */
/*===========================================================================
FUNCTION hsu_host_al_ms_enter_suspend

DESCRIPTION
Called when the suspend timer expires. Enters the LUN into suspend.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_ms_enter_suspend(void* dev_inx_val)
{
  jresult_t result;
  uint32     dev_inx = (uint32)dev_inx_val;

  /* This jsafe_leave (and jsafe_enter() at the end of the function)
  was added in order to fix the following scenario:
  This function is called from AL tsak context (see hsu_host_al_ms_enter_suspend_cb()). 
  In handle_al_task_commands() we lock the giant mutex before performing the command and 
  release it at the end. So at this point at the code we locked the giant mutex and then 
  we lock hsu_host_al_ms_data_structures_lock.
  When performing safe_disk_suspend_resume() in usbd_transfer() we call os_msleep() where 
  the giant mutex is released while we wait on a certain signal. 
  We need to exit the critical sections we entered in LIFO order so this was added in order 
  to change the order of entering the critical sections
  */
  jsafe_leave();
  rex_enter_crit_sect(&hsu_host_al_ms_data_structures_lock);

  if (!hsu_host_al_ms_device_data_pool_entry_is_free[dev_inx])
  {
    /* If a transfer is already pending, don't suspend, reschedule the suspend 
       timer */
    if (hsu_host_al_ms_device_data_pool[dev_inx].xfers_counter)
    {
      HSU_MSG_ERROR("hsu_host_al_ms_enter_suspend: failed to enter into suspend mode, xfer pending",
        0, 0, 0);
      rex_leave_crit_sect(&hsu_host_al_ms_data_structures_lock);
      jsafe_enter();

      /* restart the suspend timer */
      rex_set_timer(&(hsu_host_al_ms_device_data_pool[dev_inx].enter_suspend_timer), 
        HSU_HOST_AL_MS_ENTER_SUSPEND_TIMEOUT);

      return;
    }
    /* We want to prevent the following deadlock:
    - AL_TASK performs this function and locks the hsu_host_al_ms_data_structures_lock
    - The cable is disconnected and HS-USB2 (or 0 doesn't matter) start running
    - HS-USB2 locks the giant mutex and performs device_detach 
    - AL_TASK resumes the CPU and starts safe_disk_suspend_resume where it wants
    to lock the giant mutex. Since HS-USB2 is holding it, AL_TASK is waiting for it
    to be released
    - HS-USB2 resumes CPU. At the end of the media detach sequence HS-USB2 calls 
    hsu_host_al_ms_media_remove() where it wants to lock 
    hsu_host_al_ms_data_structures_lock but since it's locked by AL_TASK we have a
    deadlock
    */
    rex_leave_crit_sect(&hsu_host_al_ms_data_structures_lock);
    result = safe_disk_suspend_resume(hsu_host_al_ms_device_data_pool[dev_inx].dev, TRUE);
    rex_enter_crit_sect(&hsu_host_al_ms_data_structures_lock);
    if (result < 0)
    {
      HSU_MSG_ERROR("hsu_host_al_ms_enter_suspend: failed to enter into suspend mode [code:%d]",
        result, 0, 0);
    }
    else
    {
      hsu_host_al_ms_device_data_pool[dev_inx].lun_is_suspended = TRUE;
    }
  }
  rex_leave_crit_sect(&hsu_host_al_ms_data_structures_lock);
  jsafe_enter();
} /* hsu_host_al_ms_enter_suspend */

/*===========================================================================
FUNCTION hsu_host_al_ms_enter_suspend_cb

DESCRIPTION
Called when the suspend timer expires. Enters the LUN into suspend.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_ms_enter_suspend_cb(unsigned long callback_param)
{
  hsu_al_task_enqueue_cmd(hsu_host_al_ms_enter_suspend, (void *)callback_param);
}

/*===========================================================================

FUNCTION hsu_host_al_ms_async_transfer_cb

DESCRIPTION
The callback function passed to disk_async_transfer().

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_ms_async_transfer_cb
(
  /* Argument passed to disk_async_transfer(). 
  ** This is a pointer to the device's class specific data.
  */
  void *arg,
  /* Status of the transfer */
  jresult_t status
)
{
  hsu_host_al_ms_device_data_type* const dev_data_ptr = 
    (hsu_host_al_ms_device_data_type*)arg;

  HSU_ASSERT(dev_data_ptr!=NULL);

  dev_data_ptr->current_transfer_status = status;
  dev_data_ptr->xfers_counter--;

  /* Signal the completion of the transfer */
  (void)rex_set_sigs(dev_data_ptr->current_transfer_calling_task, 
                     dev_data_ptr->current_transfer_completion_sig);
}

/*===========================================================================

FUNCTION do_read_write

DESCRIPTION
This is a helper function, invoked by hsu_host_al_ms_ioctl_func() 
in order to perform a read or write operation. The function returns
after the operation is completed.

DEPENDENCIES
None.

RETURN VALUE
USBHOST_SUCCESS on success, USBHOST_ERROR on failure.

SIDE EFFECTS
None.
===========================================================================*/
static usbhost_status_type do_read_write
(
  const usbhost_dev_entry_type* dev_entry_ptr,
  const usbhost_IoctlTransfer* transfer_params,
  boolean is_read_operation
)
{
  rex_sigs_type sigs, sigs_to_wait;
  jresult_t result, status;
  uint32 direction;
  usbhost_client_data_type* client_data_ptr;
  hsu_host_al_ms_device_data_type* dev_data_ptr;
  boolean dog_reporting_enabled;

  if (is_read_operation)
  {
    direction = FROM_DEVICE;
  }
  else
  {
    direction = TO_DEVICE;
  }

  client_data_ptr = dev_entry_ptr->curr_client->data;

  HSU_ASSERT(client_data_ptr->completion_sig != 0);
  if (client_data_ptr->completion_sig == 0)
  {
    HSU_MSG_ERROR("do_read_write: client_data_ptr->completion_sig can't be 0.",
      0, 0, 0);
    return USBHOST_ERROR;
  }

  dev_data_ptr = 
    (hsu_host_al_ms_device_data_type*)dev_entry_ptr->class_specific_data;
  dev_data_ptr->current_transfer_calling_task = rex_self();

  dog_reporting_enabled = TRUE;

  if (dev_data_ptr->current_transfer_calling_task == client_data_ptr->registered_task)
  {
    /* This is the "expected" behavior - the task that registered is the one that
    ** performs ioctl operations.
    */
    dev_data_ptr->current_transfer_completion_sig = client_data_ptr->completion_sig;
  }
  else
  {
    /* Use a "default" completion signal */
    dev_data_ptr->current_transfer_completion_sig = FS_OP_COMPLETE_SIG;

    /* No dog reporting (since we do not know the appropriate dog reporting function */
    dog_reporting_enabled = FALSE;
  }

  dog_reporting_enabled = dog_reporting_enabled && 
                          (client_data_ptr->dog_rpt_fn_ptr != NULL) && 
                          (client_data_ptr->dog_rpt_sig != 0);

  sigs_to_wait = dev_data_ptr->current_transfer_completion_sig;
  if (dog_reporting_enabled)
  {
    sigs_to_wait |= client_data_ptr->dog_rpt_sig;
  }
  
  dev_data_ptr->current_transfer_status = 0;

  rex_clr_timer(&(dev_data_ptr->enter_suspend_timer));

  result = 0;
  rex_enter_crit_sect(&hsu_host_al_ms_data_structures_lock);
  if (dev_data_ptr->lun_is_suspended)
  {
    dev_data_ptr->lun_is_suspended = FALSE;
    result = safe_disk_suspend_resume(dev_data_ptr->dev, FALSE);
  }

  /* restart the suspend timer */
  rex_set_timer(&(dev_data_ptr->enter_suspend_timer), 
    HSU_HOST_AL_MS_ENTER_SUSPEND_TIMEOUT);

  if (result < 0)
  {
    HSU_MSG_ERROR("do_read_write: failed to exit suspend mode [code:%d]",
      result, 0, 0);
    rex_leave_crit_sect(&hsu_host_al_ms_data_structures_lock);
    return USBHOST_ERROR;
  }

  (void)rex_clr_sigs(dev_data_ptr->current_transfer_calling_task, 
                     dev_data_ptr->current_transfer_completion_sig);

  jsafe_enter();
  result = disk_async_transfer(dev_data_ptr->dev, 
                               hsu_host_al_ms_async_transfer_cb,
                               dev_entry_ptr->class_specific_data,
                               transfer_params->buffer,
                               0, /* Buffer is not DMA-able */
                               transfer_params->startBlock,
                               transfer_params->numberBlocks,
                               direction);

  if (result < 0)
  {
    HSU_MSG_ERROR("do_read_write: disk_async_transfer returned error code %d",
                  result, 0, 0);
    jsafe_leave();
    rex_leave_crit_sect(&hsu_host_al_ms_data_structures_lock);
    return USBHOST_ERROR;
  }

  dev_data_ptr->xfers_counter++;
  jsafe_leave();
  /* The calls to safe_disk_suspend_resume and disk_async_transfer must be
     atomic, so only now its safe to release the critical section */
  rex_leave_crit_sect(&hsu_host_al_ms_data_structures_lock);

  /* Wait for completion, while also taking care of the dog */
  for(;;) 
  {
    sigs = rex_wait(sigs_to_wait);

    if (sigs & client_data_ptr->dog_rpt_sig)
    {
      (void) rex_clr_sigs(dev_data_ptr->current_transfer_calling_task, 
                          client_data_ptr->dog_rpt_sig);

      if (client_data_ptr->dog_rpt_fn_ptr!=NULL)
      {
        client_data_ptr->dog_rpt_fn_ptr();
      }
    }

    if (sigs & dev_data_ptr->current_transfer_completion_sig)
    {
      (void) rex_clr_sigs(dev_data_ptr->current_transfer_calling_task, 
                          dev_data_ptr->current_transfer_completion_sig);
      break;
    }
  }

  status = dev_data_ptr->current_transfer_status;
  if (status == 0)
  {
    return USBHOST_SUCCESS;
  }
  else
  {
    if (is_read_operation)
    {
      HSU_MSG_ERROR("do_read_write: Failed read operation. Status is %d.",
                     status,0,0);
    }
    else
    {
      HSU_MSG_ERROR("do_read_write: Failed write operation. Status is %d.",
                    status,0,0);
    }
    
    return USBHOST_ERROR;
  }
}

/* Wrapper function that locks the giant mutex, calls disk_get_media_info()
** and unlocks the giant.
*/
/*===========================================================================

FUNCTION safe_disk_get_media_info

DESCRIPTION
Wrapper function that locks the giant mutex, calls disk_get_media_info()
and unlocks the giant.

RETURN VALUE
The result of the call to disk_get_media_info().

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t safe_disk_get_media_info(void *dev, struct media_info *pinfo)
{
  jresult_t result;

  jsafe_enter();
  result = disk_get_media_info(dev, pinfo);
  jsafe_leave();

  return result;
}

/*===========================================================================

FUNCTION hsu_host_al_ms_ioctl_func

DESCRIPTION
This function is invoked by usbhost_ioctl() in order to handle
mass storage specific IOCTLs.

DEPENDENCIES
None.

RETURN VALUE
USBHOST_SUCCESS on success. Otherwise, an appropriate
usbhost_status_type value.

SIDE EFFECTS
None.
===========================================================================*/
static usbhost_status_type hsu_host_al_ms_ioctl_func
(
  /* Information regarding the device */
  const usbhost_dev_entry_type* dev_entry_ptr,
  /* Device specific command */
  usbhost_ioctl_cmd_type command,
  /* IOCTL specific parameter
  ** passed from the application
  */
  void * param
)
{
  jresult_t result;
  usbhost_status_type ret = USBHOST_SUCCESS;
  struct media_info media_info_instance;
  hsu_host_al_ms_device_data_type* dev_data_ptr = 
    (hsu_host_al_ms_device_data_type*)dev_entry_ptr->class_specific_data;

  if (dev_data_ptr == NULL)
  {
    HSU_MSG_ERROR("hsu_host_al_ms_ioctl_func: NULL device data.",
      0,0,0);
    HSU_ASSERT(dev_data_ptr!=NULL);
    return USBHOST_ERROR;
  }

  switch(command) 
  {
  case USBHOST_IOCTL_STOR_STORAGE_TYPE:
  case USBHOST_IOCTL_STOR_MEDIA_WRITE_PROTECT:
  case USBHOST_IOCTL_STOR_BLOCK_SIZE:
  case USBHOST_IOCTL_STOR_NUMBER_OF_BLOCKS:
  case USBHOST_IOCTL_STOR_BLOCK_READ:
  case USBHOST_IOCTL_STOR_BLOCK_WRITE:
    if (param==NULL)
    {
      HSU_MSG_ERROR("hsu_host_al_ms_ioctl_func: IOCTL command=%d, NULL parameter 'param'.",
                    command,0,0);
      ret = USBHOST_ERROR;
      HSU_ASSERT(param!=NULL);
      break;
    }
    break;
  default:
    break;
  }

  if (ret==USBHOST_ERROR)
  {
    return ret;
  }

  switch(command)
  {
  case USBHOST_IOCTL_STOR_CHECK_MEDIA:
    if (!dev_data_ptr->media_is_removable)
    {
      /* return success */
      break;
    }

    /* Make sure state does not change to USBHOST_ERROR_MEDIA_NOT_PRESENT
    ** after we determine it is HSU_HOST_AL_MS_MEDIA_CHANGED and before
    ** we update it to HSU_HOST_AL_MS_MEDIA_PRESENT.
    */
    rex_enter_crit_sect(&hsu_host_al_ms_data_structures_lock);
    switch(dev_data_ptr->media_state) {
    case HSU_HOST_AL_MS_MEDIA_NOT_PRESENT:
      ret = USBHOST_ERROR_MEDIA_NOT_PRESENT;
    	break;
    case HSU_HOST_AL_MS_MEDIA_PRESENT:
      ret = USBHOST_SUCCESS;
      break;
    case HSU_HOST_AL_MS_MEDIA_CHANGED:
      ret = USBHOST_ERROR_MEDIA_CHANGED;
      /* Now that we've seen that the media has changed, 
      ** the state of the media becomes 'present'.
      */
      dev_data_ptr->media_state =  HSU_HOST_AL_MS_MEDIA_PRESENT;
      break;
    default:
      ret = USBHOST_ERROR;
    }
    rex_leave_crit_sect(&hsu_host_al_ms_data_structures_lock);
    break;
  case USBHOST_IOCTL_STOR_STORAGE_TYPE:
    *((uint8*)param) = dev_data_ptr->device_type;
    break;
  case USBHOST_IOCTL_STOR_MEDIA_REMOVABLE:
    if (!dev_data_ptr->media_is_removable)
    {
      /* According to the FS USBHOST_API documentation,
      ** returning USBHOST_ERROR indicates non-removable media.
      */
      ret = USBHOST_ERROR;
    }
    
    break;
  case USBHOST_IOCTL_STOR_MEDIA_WRITE_PROTECT:
    result = safe_disk_get_media_info(dev_data_ptr->dev, &media_info_instance);
    if (result != 0)
    {
      HSU_MSG_ERROR("USBHOST_IOCTL_STOR_MEDIA_WRITE_PROTECT: disk_get_media_info returned error number %d",
        result,0,0);
      ret = USBHOST_ERROR;
      break;
    }

    if (media_info_instance.write_protect == TRUE)
    {
      *((uint32*)param) = SCS_INFO_MEDIA_WRITE_PROTECTED;
    }
    else
    {
      *((uint32*)param) = SCS_INFO_MEDIA_NOT_WRITE_PROTECTED;
    }

    break;
  case USBHOST_IOCTL_STOR_BLOCK_SIZE:
    result = safe_disk_get_media_info(dev_data_ptr->dev, &media_info_instance);
    if (result != 0)
    {
      HSU_MSG_ERROR("USBHOST_IOCTL_STOR_BLOCK_SIZE: disk_get_media_info returned error number %d",
        result,0,0);
      ret = USBHOST_ERROR;
      break;
    }

    *((uint32*)param) = media_info_instance.sector_size;
    break;
  case USBHOST_IOCTL_STOR_NUMBER_OF_BLOCKS:
    result = safe_disk_get_media_info(dev_data_ptr->dev, &media_info_instance);
    if (result != 0)
    {
      HSU_MSG_ERROR("USBHOST_IOCTL_STOR_NUMBER_OF_BLOCKS: disk_get_media_info returned error number %d",
        result,0,0);
      ret = USBHOST_ERROR;
      break;
    }

    *((uint32*)param) = media_info_instance.sector_count;
    break;
  case USBHOST_IOCTL_STOR_BLOCK_READ:
    ret = do_read_write(dev_entry_ptr,
                        (usbhost_IoctlTransfer*)param,
                        TRUE);
    break;
  case USBHOST_IOCTL_STOR_BLOCK_WRITE:
    ret = do_read_write(dev_entry_ptr,
                        (usbhost_IoctlTransfer*)param,
                        FALSE);
    break;
  default:
    HSU_MSG_ERROR("hsu_host_al_ms_ioctl_func: unknown command code %d.",
      command,0,0);
    HSU_ASSERT((uint32)"hsu_host_al_ms_ioctl_func: unknown command" == NULL);
    ret = USBHOST_ERROR;
    break;
  }

  return ret;
}

/*===========================================================================

FUNCTION hsu_host_al_ms_get_usbd_handle

DESCRIPTION
This function retrieves the usbd handle associated with a given device.

DEPENDENCIES
None.

RETURN VALUE
A valid usbd handle on success. NULL otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static void* hsu_host_al_ms_get_usbd_handle
(
  const usbhost_dev_entry_type* dev_entry_ptr
)
{
  void* ret;
  hsu_host_al_ms_device_data_type* dev_data_ptr = 
    (hsu_host_al_ms_device_data_type*)dev_entry_ptr->class_specific_data;

  if (dev_data_ptr == NULL)
  {
    HSU_MSG_ERROR("hsu_host_al_ms_get_usbd_handle: NULL device data.",
      0,0,0);
    HSU_ASSERT(dev_data_ptr!=NULL);
    return NULL;
  }

  jsafe_enter();
  ret = disk_get_usbd_handle(dev_data_ptr->dev);
  jsafe_leave();

  return ret;
}

/*===========================================================================

FUNCTION hsu_host_al_ms_init

DESCRIPTION
This function initializes the adaptation layer.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_host_al_ms_init(void)
{
  uint32 i;
  jresult_t jresult;

  HSU_MSG_HIGH("Starting hsu_host_al_ms_init().",0,0,0);

  /* Initialize data structures */
  rex_init_crit_sect(&hsu_host_al_ms_data_structures_lock);
  for (i=0; i<USBHOST_MAX_DEVICES; i++)
  {
    hsu_host_al_ms_device_data_pool_entry_is_free[i] = TRUE;
    /* Define the suspend timer */
    hsu_host_al_ms_device_data_pool[i].lun_is_suspended = FALSE;
    rex_def_timer_ex(&(hsu_host_al_ms_device_data_pool[i].enter_suspend_timer),
      hsu_host_al_ms_enter_suspend_cb,
      i);
  }

  /* Let the host api know how to handle mass storage specific IOCTLs. */
  usbhost_set_ioctl_func(USBHOST_PDEV_TYPE_MASS_STORAGE, 
                         hsu_host_al_ms_ioctl_func);

  usbhost_set_usbd_handle_func(USBHOST_PDEV_TYPE_MASS_STORAGE, 
                               hsu_host_al_ms_get_usbd_handle);

  /* Let the mass storage class driver know which functions to call 
  ** on attach and detach.
  */
  hsu_host_al_ms_disk_callbacks.device_attach = hsu_host_al_ms_device_attach;
  hsu_host_al_ms_disk_callbacks.device_detach = hsu_host_al_ms_device_detach;
  hsu_host_al_ms_disk_callbacks.media_insert = hsu_host_al_ms_media_insert;
  hsu_host_al_ms_disk_callbacks.media_remove = hsu_host_al_ms_media_remove;

  jresult = register_disk_handler(&hsu_host_al_ms_disk_callbacks);
  if (jresult != 0)
  {
    HSU_MSG_ERROR("hsu_host_al_ms_init(): register_disk_handler returned error %d .",
                  jresult,0,0);
    return FALSE;
  }

#ifndef T_REXNT
#ifdef FEATURE_HS_USB_HOST_MS_PERF
#error code not present
#endif
#endif

  HSU_MSG_HIGH("Exiting hsu_host_al_ms_init().",0,0,0);
  return TRUE;
}

/*===========================================================================

FUNCTION hsu_host_al_ms_uninit

DESCRIPTION
This function un-initializes the adaptation layer.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_host_al_ms_uninit(void)
{
    int i = 0;
  HSU_MSG_HIGH("Starting hsu_host_al_ms_uninit().",0,0,0);

  /* Stop handling mass storage specific IOCTLs. */
  usbhost_set_ioctl_func(USBHOST_PDEV_TYPE_MASS_STORAGE, NULL);

  usbhost_set_usbd_handle_func(USBHOST_PDEV_TYPE_MASS_STORAGE, NULL);
  for (i=0; i<USBHOST_MAX_DEVICES; i++)
  {
      rex_clr_timer(&(hsu_host_al_ms_device_data_pool[i].enter_suspend_timer));
  }

  unregister_disk_handler();

  HSU_MSG_HIGH("Exiting hsu_host_al_ms_uninit().",0,0,0);
  return TRUE;
}
