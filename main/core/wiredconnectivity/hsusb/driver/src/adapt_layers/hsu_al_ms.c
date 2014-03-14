/*==============================================================================

High Speed USB Mass Storage Adaptation Layer

GENERAL DESCRIPTION
Implementation of the Mass Storage Adaptation Layer.
This includes implementation of the API functions defined in fd_storage_app.h .
These functions are called by the mass storage function driver.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_ms.c#23 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
03/08/06  sk      Initial version

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_HS_USB_UICC
#include "hsu_usb_uicc.h"
#endif /* FEATURE_HS_USB_UICC */

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

/* MAX_UINT16 is defined only in AEEStdDef.h -> BREW code,
that's why we need the define here as well */
#ifndef MAX_UINT16
#define MAX_UINT16 65535
#endif

#ifdef FEATURE_HS_USB_RAMDRIVE
#error code not present
#endif /* FEATURE_HS_USB_RAMDRIVE */

#include "fs_hotplug.h"

#include "queue.h"
#include "hsu_al_ms.h"
#include "hsu_config_selector.h"
#include "hsu_common.h"
#include "rex.h"
#include "dog.h"
#include "task.h"

#include "uw_device_mass.h"
#include "jerrno.h"

#include "port.h"   /* os_safe_enter/leave */

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
typedef enum {
  LUN_STATUS_NOT_READY,
  LUN_STATUS_READY,
  LUN_STATUS_MEDIA_CHANGED
}
hsu_al_ms_lun_status_enum;

/* This struct represents a block device, e.g., sdcc.
** The 'context' parameter to the functions is intended for cases
** where a block device has several instances, e.g., several sdcc drives.
*/
typedef struct 
{
  boolean (*read) (void* context, uint32 start_address, uint16 num_blocks, byte* buff);
  boolean (*write) (void* context, uint32 startAddress, uint16 num_blocks, byte* buff);
  boolean (*get_capacity) (void* context, uint32* block_size, uint32* num_blocks);
  /* Obtain exclusive access to the block device */
  boolean (*lock)(void* context);
  /* Relinquish exclusive access to the block device */
  boolean (*unlock)(void* context);
  /* Try to access the media, to see if it's there */
  boolean (*media_is_present)(void* context, boolean was_present);
  /* Open the media for i/o operations */
  boolean (*open)(void* context);
  /* Close the media for i/o operations */
  boolean (*close)(void* context);
}
hsu_al_ms_block_device_info_type;

/* A command, that is to be inserted into a queue
** and processed by the mass storage adaptation layer task.
*/
typedef struct 
{
  q_link_type   link;
  void          (*func)(void *);
  void          *arg;
} hsu_al_ms_cmd_type;

/* This struct contains the parameters for a vread() or vwrite() operation.
** It is used as the argument for a read or write command which is inserted
** to the command queue.
*/
typedef struct 
{
  uint8   device_index;
  uint8   lun_index;
  uint32  start_address;
  uint16  num_blocks;
  byte*   buff;
}
hsu_al_ms_readwrite_cmd_arg_type;

typedef struct 
{
  /* LUN index */
  uint8                             index;
  /* For future use - enable/disable LUNs at run time */
  boolean                           enabled;
  /* Number of polling timer expirations since the last time
  ** this LUN was actually polled.
  */
  boolean                           removable;
  /* TRUE for removable media, i.e. SDCC */
  /* FALSE for non-removable media, i.e FTL, RAMDRIVE,UICC */
  uint32                            polling_counter;
  hsu_al_ms_lun_status_enum         status;
  hsu_al_ms_block_device_info_type* block_device_ptr;
  /* Argument to the block device functions */
  void*                             context;
  /* Argument for read_cmd */
  hsu_al_ms_readwrite_cmd_arg_type  read_arg;
  /* Argument for write_cmd */
  hsu_al_ms_readwrite_cmd_arg_type  write_arg;
  /* For performing vread() */
  hsu_al_ms_cmd_type                read_cmd;
  /* For performing vwrite() */
  hsu_al_ms_cmd_type                write_cmd;
  /* Called after reading from or writing to the block device */
  completion_callback               io_completion_callback;
  void*                             io_completion_callback_arg;
  /* A flag used to fake media detach whenever there is a write error with 
  fast CSW configured or vunmount call for removable media */
  boolean                           media_change_requested;
  /* For the block device information */
  uint32                            block_size;
  uint32                            num_blocks;
  boolean                           read_only;
}
hsu_al_ms_lun_info_type;

typedef struct 
{
  /* Size of the luns array */
  uint8                      num_luns;
  /* A dynamically allocated array. Allocated (once) when the task initializes. */
  hsu_al_ms_lun_info_type*   luns;
}
hsu_al_ms_device_info_type;

/*==============================================================================
Constants and Macros
==============================================================================*/
#define NUM_DEVICES                    1

#ifndef HS_USB_MS_MAX_NUM_LUNS
#define MAX_NUM_LUNS                   7
#else
#define MAX_NUM_LUNS                   HS_USB_MS_MAX_NUM_LUNS
#endif /* HS_USB_MS_MAX_NUM_LUNS */

#define POLLING_TIMEOUT_MILLISECS      1000
/* Number of polling timer intervals to wait before polling
** for a LUN with no media.
*/
#define POLLING_NOP_LIMIT              5

/* Task signals */
#define  HSU_AL_MS_CMD_Q_SIG      0x0001 /* New command in the command queue */
#define  HSU_AL_MS_POLLING_SIG    0x0002 /* Time to perform polling */
#define  HSU_AL_MS_RPT_TIMER_SIG  0x0004 /* Dog signal */

#define HSU_AL_MS_IMPLEMENTED_SCSI_VERSION 0x2 /* SCSI-2 */

/*==============================================================================
Variables
==============================================================================*/
static rex_timer_type hsu_al_ms_rpt_timer;        /* dog report timer     */
static rex_timer_type hsu_al_ms_polling_timer;    /* polling timer     */

/* For INQUIRY data (vget_desc()) */
#ifdef FEATURE_VERSION_KK5
static char* inquiry_data_vendor = "Lemon C9";
#else
static char* inquiry_data_vendor = "Qualcomm";
#endif
static char* inquiry_data_product = "MMC Storage     ";
static char* inquiry_data_prod_rev = "2.31";

/* TRUE when the adaptation layer needs to shut down - 
** stop polling and executing commands. It is set to TRUE 
** by hsu_al_ms_disable().
*/
static boolean going_down = FALSE;

/* TRUE after the adaptation layer has been enabled,
** which happens once the first LUN is opened by the stack.
*/
static boolean hsu_al_ms_enabled = FALSE;

/* Command queue. Used for performing async operations  */
static q_type hsu_al_ms_cmd_q;

/* Hotplug block device info. */
static hsu_al_ms_block_device_info_type hsu_al_ms_hotplug_block_device;

#ifdef FEATURE_HS_USB_RAMDRIVE
#error code not present
#endif /* FEATURE_HS_USB_RAMDRIVE */

/* Info regarding all devices */
static hsu_al_ms_device_info_type hsu_al_ms_devices[NUM_DEVICES];

/* Info regarding LUNs */
static hsu_al_ms_lun_info_type hsu_al_ms_luns[MAX_NUM_LUNS];

static uint8 hsu_al_ms_device_index;

/*==============================================================================

Exported Variables

==============================================================================*/

/* Stack and tcb for the task */
unsigned char hsu_al_ms_stack[HSU_AL_MS_STACK_SIZE * sizeof(rex_stack_word_type)];
rex_tcb_type        hsu_al_ms_tcb;

/*==============================================================================

FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_ms_get_num_devices

DESCRIPTION
This function returns the number of mass storage devices
(that is, the number of mass storage USB interfaces)

DEPENDENCIES
None.

RETURN VALUE
The number of mass storage devices.

SIDE EFFECTS
None.
===========================================================================*/
uint8 hsu_al_ms_get_num_devices(void)
{
  return NUM_DEVICES;
}

/*===========================================================================

FUNCTION enqueue_cmd

DESCRIPTION
This function adds a command to the command queue, and sends a signal that
indicates this to the task.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void enqueue_cmd
(
 hsu_al_ms_cmd_type* cmd_ptr /* Pointer to the command to be added */
 )
{
  q_put(&hsu_al_ms_cmd_q, &cmd_ptr->link);
  /* Send signal to let the task know there are waiting commands in the queue */
  (void)rex_set_sigs(&hsu_al_ms_tcb, HSU_AL_MS_CMD_Q_SIG);
}

/* Forward declaration */
static void initialize_polling(void);

/*===========================================================================

FUNCTION hsu_al_ms_enable

DESCRIPTION
Enables operation of the adaptation layer, by activating the polling timer.

DEPENDENCIES
The mass storage adaptation layer task must be created first.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_ms_enable(void)
{
  going_down = FALSE;

  HSU_MSG_HIGH("Starting hsu_al_ms_enable()",0,0,0);
  initialize_polling();
  HSU_MSG_HIGH("Exiting hsu_al_ms_enable()",0,0,0);
}

/*===========================================================================

FUNCTION hsu_al_ms_stop

DESCRIPTION
This function disables operation of the adaptation layer,
and also stops the timer that's responsible for kicking the dog.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_ms_stop(void)
{
  HSU_MSG_HIGH("hsu_al_ms_stop(): entry",0,0,0);

  /* Stop polling timer */
  (void)rex_clr_timer(&hsu_al_ms_polling_timer);
  /* Stop dog timer */
  (void)rex_clr_timer(&hsu_al_ms_rpt_timer);

  /* Clear polling signal, command signal and dog signal*/
  (void)rex_clr_sigs(&hsu_al_ms_tcb, 
    HSU_AL_MS_POLLING_SIG | HSU_AL_MS_CMD_Q_SIG | 
    HSU_AL_MS_RPT_TIMER_SIG
    );

  /* Let the task know that no further commands should be executed */
  going_down = TRUE;

  HSU_MSG_HIGH("hsu_al_ms_stop(): exit",0,0,0);
}

/*===========================================================================

FUNCTION hsu_al_ms_disable

DESCRIPTION
Disables operation of the adaptation layer, 
by deactivating the polling timer and clearing the command queue.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_ms_disable(void)
{
  juint8_t                          dev_index, lun_index;
  hsu_al_ms_lun_info_type*          lun_ptr;

  hsu_al_ms_device_index = 0;

  HSU_MSG_HIGH("Starting hsu_al_ms_disable()",0,0,0);

  /* Let the task know that no further commands should be executed */
  going_down = TRUE;

  /* Stop polling timer */
  (void)rex_clr_timer(&hsu_al_ms_polling_timer);

  /* 
  * Clear polling signal. 
  * No need to clear the command single, since we want to empty the queue. 
  */
  (void)rex_clr_sigs(&hsu_al_ms_tcb, HSU_AL_MS_POLLING_SIG);

  /* close and unlock the block devices */
  for (dev_index=0; dev_index<NUM_DEVICES; dev_index++)
  {
    for (lun_index=0; lun_index<hsu_al_ms_devices[dev_index].num_luns; lun_index++)
    {
      lun_ptr = &hsu_al_ms_devices[dev_index].luns[lun_index];

      if (lun_ptr->enabled && lun_ptr->removable)
      {
        if (!lun_ptr->block_device_ptr->close(lun_ptr->context))
        {
          HSU_MSG_ERROR("Failed to close LUN %d of device %d.",
            lun_index, dev_index, 0);
        }
      }

      if (lun_ptr->enabled && !lun_ptr->block_device_ptr->unlock(lun_ptr->context))
      {
        HSU_MSG_ERROR("Failed to unlock LUN %d of device %d.",
          lun_index, dev_index, 0);
      }
    }
  }

  HSU_MSG_HIGH("Exiting hsu_al_ms_disable()",0,0,0);
}

/*===========================================================================

FUNCTION execute_next_cmd

DESCRIPTION
Get the next command from the queue and execute it.

DEPENDENCIES
None.

RETURN VALUE
FALSE if the queue was empty, TRUE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static boolean execute_next_cmd(void)
{
  hsu_al_ms_cmd_type* cmd_ptr = q_get(&hsu_al_ms_cmd_q);

  if (cmd_ptr == NULL)
  {
    /* No command to execute */
    return FALSE;
  }

  if (!going_down)
  {
    cmd_ptr->func(cmd_ptr->arg);
  }
  return TRUE;
}

/*===========================================================================

FUNCTION poll_lun

DESCRIPTION
Try to access the underlying block device of the lun, and update
the lun status accordingly.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void poll_lun
(
 hsu_al_ms_lun_info_type* lun_ptr /* Pointer to the LUN info */
 )
{
  boolean ready, was_present;
  boolean removable = lun_ptr->removable;

  was_present = (lun_ptr->status==LUN_STATUS_NOT_READY)?FALSE:TRUE;
  ready = lun_ptr->block_device_ptr->media_is_present(lun_ptr->context,was_present);

  if (removable)
  {
    if (was_present && !ready)
    {
      HSU_MSG_HIGH("poll_lun(): Removal detected in LUN %d",lun_ptr->index,0,0);
    }

    if (!was_present && ready)
    {
      HSU_MSG_HIGH("poll_lun(): Insertion detected in LUN %d",lun_ptr->index,0,0);
    }
  }

  if (!ready)
  {
    lun_ptr->status = LUN_STATUS_NOT_READY;
  }
  else
  {
    if (lun_ptr->status == LUN_STATUS_NOT_READY)
    {
      HSU_MSG_HIGH("get_capacity(): Start for lun=%d", lun_ptr->index,0,0);
      if (!lun_ptr->block_device_ptr->get_capacity(
        lun_ptr->context, 
        &lun_ptr->block_size, 
        &lun_ptr->num_blocks))
      {
        HSU_MSG_ERROR("get_capacity() failed for LUN %d", lun_ptr->index,0,0);
      }
      else
      {
        HSU_MSG_HIGH("get_capacity(): lun=%d, block_size=%d, num_blocks=%d",
          lun_ptr->index,lun_ptr->block_size,lun_ptr->num_blocks);

        if (removable)
        {
          lun_ptr->status = LUN_STATUS_MEDIA_CHANGED;
        }
        else
        {
          lun_ptr->status = LUN_STATUS_READY;
        }
      }
    }

    if (!was_present)
    {
      /* Reset the fast CSW write ok flag, when media is inserted back in */
      lun_ptr->media_change_requested = FALSE;
    }
  }
} /* poll_lun */

/*===========================================================================

FUNCTION perform_polling

DESCRIPTION
Poll all LUNs whose block device has removable media.
This is done periodically, via a timer.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void perform_polling(void)
{
  juint8_t                  dev_index, lun_index;
  hsu_al_ms_lun_info_type*  lun_ptr;
  boolean                   need_to_poll;

  for (dev_index=0; dev_index<NUM_DEVICES; dev_index++)
  {
    for (lun_index=0; lun_index<hsu_al_ms_devices[dev_index].num_luns; lun_index++)
    {
      lun_ptr = &hsu_al_ms_devices[dev_index].luns[lun_index];
      need_to_poll = FALSE;
      if (lun_ptr->removable)
      {
        if (lun_ptr->status == LUN_STATUS_NOT_READY)
        {
          lun_ptr->polling_counter++;
          if (lun_ptr->polling_counter == POLLING_NOP_LIMIT)
          {
            need_to_poll = TRUE;
          }
        }
        else
        {
          need_to_poll = TRUE;
        }
        if (need_to_poll)
        {
          poll_lun(&hsu_al_ms_devices[dev_index].luns[lun_index]);
          lun_ptr->polling_counter = 0;
        }
      }
    }
  }
}

/*===========================================================================

FUNCTION initialize_polling

DESCRIPTION
Prepare for polling (lock LUNs, initialize data structures and polling timer).
This is done when the adaptation layer is enabled
(that is, when the mass storage function driver becomes active)

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void initialize_polling(void)
{
  juint8_t                          dev_index, lun_index;
  hsu_al_ms_lun_info_type*          lun_ptr;
  hsu_al_ms_block_device_info_type* block_device_ptr;

  for (dev_index=0; dev_index<NUM_DEVICES; dev_index++)
  {
    for (lun_index=0; lun_index<hsu_al_ms_devices[dev_index].num_luns; lun_index++)
    {
      lun_ptr = &hsu_al_ms_devices[dev_index].luns[lun_index];
      block_device_ptr = lun_ptr->block_device_ptr;
      lun_ptr->status = LUN_STATUS_NOT_READY;

      /* Make sure the first timer expiration causes polling */
      lun_ptr->polling_counter = POLLING_NOP_LIMIT-1;

      if (!block_device_ptr->lock(lun_ptr->context))
      {
        /* Can't lock the LUN --> Can't use it. */
        HSU_MSG_ERROR("perform_initial_polling: Can't lock LUN.", 0, 0, 0);
        lun_ptr->enabled = FALSE;
        continue;
      }
      lun_ptr->enabled = TRUE;
      /* Inialilize fast CSW write error flag */
      lun_ptr->media_change_requested = FALSE;
    }
  }

  (void)rex_set_timer(&hsu_al_ms_polling_timer, POLLING_TIMEOUT_MILLISECS);
}

/*===========================================================================

FUNCTION read_impl

DESCRIPTION
This function performs a read operation (from a specified LUN).
It is pointed to by a read command, which is inserted into the command
queue when the function driver calls vread().

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void read_impl
(
 void *arg /* Run-time type is hsu_al_ms_readwrite_cmd_arg_type* */
 )
{
  hsu_al_ms_readwrite_cmd_arg_type* read_arg;
  hsu_al_ms_lun_info_type*          lun_ptr;
  boolean                           success = FALSE;

  HSU_ASSERT(arg!=NULL);

  /* arg points to a hsu_al_ms_readwrite_cmd_arg_type struct */
  read_arg = (hsu_al_ms_readwrite_cmd_arg_type*)arg;
  /* Note: we assume that the (device_index,lun_index) pair is valid.
  ** This is checked by vread().
  */
  lun_ptr = &hsu_al_ms_devices[read_arg->device_index].luns[read_arg->lun_index];

  if (lun_ptr->status == LUN_STATUS_NOT_READY)
  {
    jsafe_enter();
    if (lun_ptr->io_completion_callback!=NULL)
    {
      lun_ptr->io_completion_callback(lun_ptr->io_completion_callback_arg, 
        FALSE
        );
    }
    jsafe_leave();
    return;
  }

  success = lun_ptr->block_device_ptr->read(lun_ptr->context, 
    read_arg->start_address, 
    read_arg->num_blocks, 
    read_arg->buff
    );

  jsafe_enter();
  if (lun_ptr->io_completion_callback!=NULL)
  {
    if (success)
    {
      /* Call completion callback */
      lun_ptr->io_completion_callback(lun_ptr->io_completion_callback_arg, 
        TRUE
        );
    }
    else
    {
      lun_ptr->io_completion_callback(lun_ptr->io_completion_callback_arg, 
        FALSE
        );
    }
  }
  jsafe_leave();

  if (!success)
  {
    /* Failed to read --> should check if the media is there */
    HSU_MSG_ERROR("read_impl(): Read error. lun_num=%d, strt_blk=%d, num_blks=%d",
      read_arg->lun_index, read_arg->start_address, read_arg->num_blocks);
    poll_lun(lun_ptr);
  }
} /* read_impl */

/*===========================================================================

FUNCTION write_impl

DESCRIPTION
This function performs a write operation (from a specified LUN).
It is pointed to by a write command, which is inserted into the command
queue when the function driver calls vwrite().

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void write_impl
(
 void *arg /* Run-time type is hsu_al_ms_readwrite_cmd_arg_type* */
 )
{
  /* arg points to a hsu_al_ms_readwrite_cmd_arg_type struct */
  hsu_al_ms_readwrite_cmd_arg_type* write_arg = (hsu_al_ms_readwrite_cmd_arg_type*)arg;
  hsu_al_ms_lun_info_type* lun_ptr = 
    &hsu_al_ms_devices[write_arg->device_index].luns[write_arg->lun_index];
  boolean success = FALSE;

  if (lun_ptr->status == LUN_STATUS_NOT_READY)
  {
    jsafe_enter();
    if (lun_ptr->io_completion_callback!=NULL)
    {
      lun_ptr->io_completion_callback(lun_ptr->io_completion_callback_arg, 
        FALSE
        );
    }
    jsafe_leave();
    return;
  }

  success = lun_ptr->block_device_ptr->write(lun_ptr->context, 
    write_arg->start_address, 
    write_arg->num_blocks,
    write_arg->buff
    );

  jsafe_enter();
  if (lun_ptr->io_completion_callback!=NULL)
  {
    if (success)
    {
      /* Call completion callback */
      lun_ptr->io_completion_callback(lun_ptr->io_completion_callback_arg, 
        TRUE
        );
    }
    else
    {
      lun_ptr->io_completion_callback(lun_ptr->io_completion_callback_arg, 
        FALSE
        );
    }
  }
  jsafe_leave();

  if (!success)
  {
    HSU_MSG_ERROR("write_impl(): Write error. lun_num=%d, strt_blk=%d, num_blks=%d",
      write_arg->lun_index, write_arg->start_address, write_arg->num_blocks);

#ifdef FEATURE_HS_USB_FAST_CSW
    /* Mark fast CSW write error, so vready() will return false */ 
    lun_ptr->media_change_requested = TRUE;
#else 
    /* Failed to write --> should check if the media is there */
    poll_lun(lun_ptr);
#endif /* FEATURE_HS_USB_FAST_CSW */  
  }
} /* write_impl */

/****************************************************************************************
** Hotplug Block Device functions. 
** Pointers to these functions are stored in hsu_al_ms_hotplug_block_device.
****************************************************************************************/
/*===========================================================================

FUNCTION hsu_al_ms_hotplug_block_device_read

DESCRIPTION
Read blocks via the hotplug API

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_al_ms_hotplug_block_device_read 
(
 void*   context, /* The device handle needed for hotplug api functions */
 uint32  start_address, /* Block number to start from */
 uint16  num_blocks, /* Number of blocks to read */
 byte*   buff /* Buffer to read into */
 )
{
  /*hotplug_dev_read returns 0 on success and -1 on error*/
  if (hotplug_dev_read((struct hotplug_device*)context, start_address,
    (byte*)buff,num_blocks) == 0)
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION hsu_al_ms_hotplug_block_device_write

DESCRIPTION
Write blocks via the hotplug API

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_al_ms_hotplug_block_device_write
(
 void*   context, /* The device handle needed for hotplug api functions */
 uint32  start_address, 
 uint16  num_blocks, 
 byte*   buff
 )
{
  /*hotplug_dev_write returns 0 on success and -1 on error*/
  if (hotplug_dev_write((struct hotplug_device*)context, start_address,
    (byte*)buff,num_blocks) == 0)
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION hsu_al_ms_hotplug_block_device_get_capacity

DESCRIPTION
Get the block size and number of blocks for the current media

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_al_ms_hotplug_block_device_get_capacity
(
 void*   context, /* device handle */
 uint32* block_size, /* out parameter */
 uint32* num_blocks /* out parameter */
 )
{
  uint16 block_size_tmp = 0;
  /* hotplug_dev_get_size returns 0 on success and -1 on error*/
  if (hotplug_dev_get_size ((struct hotplug_device *)context,
    num_blocks,&block_size_tmp) == 0)
  {
    *block_size = block_size_tmp;
    return TRUE;
  }
  *block_size = 0;
  *num_blocks = 0;
  return FALSE;
}

/*===========================================================================

FUNCTION hsu_al_ms_hotplug_block_device_lock

DESCRIPTION
Lock the hotplug device drive so that applications cannot use it

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_al_ms_hotplug_block_device_lock
(
 void* context /*device handle*/
 )
{
  struct hotplug_device* hdev = (struct hotplug_device*)context;

  if (hotplug_lock_dev(hdev) != 0)
  {
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_ms_hotplug_block_device_unlock

DESCRIPTION
Unlock the hotplug device drive so that applications can use it

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_al_ms_hotplug_block_device_unlock
(
 void* context /* device handle */
 )
{
  struct hotplug_device* hdev = (struct hotplug_device*)context;

  if (NULL != hdev)
  {
    if (hotplug_unlock_dev(hdev) != 0)
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_ms_hotplug_block_device_media_is_present

DESCRIPTION
Check if the media is in the drive.

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_al_ms_hotplug_block_device_media_is_present
(
 void* context,  /*device handle*/
 boolean was_present
 )
{
  HSU_USE_PARAM(was_present);
  return (boolean)hotplug_dev_is_present((struct hotplug_device *)context);
}

/*===========================================================================

FUNCTION hsu_al_ms_hotplug_block_device_open

DESCRIPTION
Open the device, before we start using it. Irrelevant for sdcc.

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_al_ms_hotplug_block_device_open
(
 void* context /*device handle*/
 )
{
  HSU_ASSERT(context != NULL); 
  /* hotplug_dev_open returns 0 on success and -1 on error */
  if (hotplug_dev_open((struct hotplug_device*)context) == 0)
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION hsu_al_ms_hotplug_block_device_close

DESCRIPTION
Close the device, once we've stopped using it. Irrelevant for sdcc.

DEPENDENCIES
None.

RETURN VALUE
TRUE if successful, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_al_ms_hotplug_block_device_close
(
 void* context /*device handle*/
 )
{
  if (!hotplug_dev_close((struct hotplug_device *)context))
  {
    return TRUE;
  }
  return FALSE;
}

/******************************************************************************/
/*===========================================================================
FUNCTION      hsu_al_ms_get_num_luns

DESCRIPTION
This function counts the total number of luns

DEPENDENCIES
None.

RETURN VALUE
Number of luns

SIDE EFFECTS
None.
===========================================================================*/
uint8 hsu_al_ms_get_num_luns(void)
{
  uint8 num_luns = 0;
  struct hotplug_device* hdev = NULL; 
  uint8 num_of_mmc_controllers = 0;
  uint8 num_of_mmc_luns = 0;

  /* Count all MMC/SDCC devices */
  hdev = hotplug_find_dev_by_type(HOTPLUG_TYPE_MMC, NULL);
  while (hdev != NULL) 
  {
    num_of_mmc_controllers++;
    hdev = hotplug_find_dev_by_type (HOTPLUG_TYPE_MMC, hdev);
  }
  num_of_mmc_luns = hsu_conf_sel_number_of_sdcc_luns(); 
  num_luns += MIN (num_of_mmc_controllers, num_of_mmc_luns);

  /* Count all FTL devices */
  hdev = hotplug_find_dev_by_type(HOTPLUG_TYPE_FTL, NULL);
  while (hdev != NULL) 
  {
    num_luns++;
    hdev = hotplug_find_dev_by_type (HOTPLUG_TYPE_FTL, hdev);
  }

  /* Count RAMDRIVE*/
#ifdef FEATURE_HS_USB_RAMDRIVE
#error code not present
#endif /*FEATURE_HS_USB_RAMDRIVE*/

#ifdef FEATURE_HS_USB_UICC
  /* Count UICC LUN only if allowed by UIM NV item */
  if (hsu_uicc_is_enabled_by_nv())
  {
    /* Count all USB devices */
    hdev = hotplug_find_dev_by_type(HOTPLUG_TYPE_USB_MS, NULL);
    if (hdev != NULL) 
    {
      num_luns++;
    }
  }
#endif /* FEATURE_HS_USB_UICC */

  return num_luns;
}

/*===========================================================================
FUNCTION init_lun

DESCRIPTION
This function initiates all the relevant fields in the given lun data structure


DEPENDENCIES
None.

RETURN VALUE

SIDE EFFECTS
None.
===========================================================================*/
static void init_lun
(
 hsu_al_ms_lun_info_type* lun_ptr, 
 uint8 lun_index,
 void *context, /* For hotplug devices this is the device handle*/
 hsu_al_ms_block_device_info_type *block_device,
 boolean is_removable
 )
{
  HSU_ASSERT(lun_ptr != NULL);
  lun_ptr->index = lun_index;
  lun_ptr->enabled = TRUE;
  lun_ptr->context = context;
  lun_ptr->status = LUN_STATUS_NOT_READY;  
  lun_ptr->io_completion_callback = NULL;
  lun_ptr->removable = is_removable;
  lun_ptr->media_change_requested = FALSE;

  lun_ptr->read_cmd.func = read_impl;
  lun_ptr->read_arg.device_index = 0;
  lun_ptr->read_arg.lun_index = lun_index;
  lun_ptr->read_cmd.arg = &lun_ptr->read_arg;
  (void)q_link(&lun_ptr->read_cmd, &lun_ptr->read_cmd.link);

  lun_ptr->write_cmd.func = write_impl;
  lun_ptr->write_arg.device_index = 0;
  lun_ptr->write_arg.lun_index = lun_index;
  lun_ptr->write_cmd.arg = &lun_ptr->write_arg;
  (void)q_link(&lun_ptr->write_cmd, &lun_ptr->write_cmd.link);

  lun_ptr->block_device_ptr = block_device;
  lun_ptr->block_size = 0;
  lun_ptr->num_blocks = 0;
  /* Is the media read only? Currently we just assume it's not read only. */
  lun_ptr->read_only = FALSE;
}

#ifdef FEATURE_HS_USB_UICC
/*===========================================================================
FUNCTION hsu_al_ms_init_usb_hotplug_lun

DESCRIPTION

DEPENDENCIES
None.

RETURN VALUE

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_ms_init_usb_hotplug_lun()
{
  struct hotplug_device* hdev = 
    hotplug_find_dev_by_type(HOTPLUG_TYPE_USB_MS, NULL);
  if (hdev != NULL  &&  hsu_al_ms_devices[0].num_luns < MAX_NUM_LUNS) 
  {
    init_lun(&hsu_al_ms_devices[0].luns[hsu_al_ms_devices[0].num_luns],
      hsu_al_ms_devices[0].num_luns,(void*)hdev,&hsu_al_ms_hotplug_block_device, TRUE);
    hsu_al_ms_devices[0].num_luns++;
  }
}
#endif /* FEATURE_HS_USB_UICC */

/*===========================================================================
FUNCTION hsu_al_ms_init_hotplug_lun

DESCRIPTION
This function initiates luns for all hotplug devices of the given type

DEPENDENCIES
None.

RETURN VALUE

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_ms_init_hotplug_lun
(
 hotplug_dev_type dev_type,
 boolean  removble
 )
{
  struct hotplug_device* hdev  = NULL;
  uint8 max_num_of_mmc_luns_from_nv = 0;
  uint8 initialized_mmc_luns = 0;

  if (dev_type == HOTPLUG_TYPE_MMC)
  {
    max_num_of_mmc_luns_from_nv = hsu_conf_sel_number_of_sdcc_luns(); 
  }

  hdev = hotplug_find_dev_by_type(dev_type, NULL);
  while (hdev != NULL  &&  hsu_al_ms_devices[0].num_luns < MAX_NUM_LUNS) 
  {
    if (dev_type == HOTPLUG_TYPE_MMC && initialized_mmc_luns >= max_num_of_mmc_luns_from_nv)
    {
      return;
    }
    else
    {
      init_lun(&hsu_al_ms_devices[0].luns[hsu_al_ms_devices[0].num_luns],
        hsu_al_ms_devices[0].num_luns,(void*)hdev,&hsu_al_ms_hotplug_block_device,removble);
      hsu_al_ms_devices[0].num_luns++;
      hdev = hotplug_find_dev_by_type (dev_type, hdev);
      if (dev_type == HOTPLUG_TYPE_MMC)
      {
        initialized_mmc_luns++;
      }
    }
  }
}
/*===========================================================================

FUNCTION init_data_structures

DESCRIPTION
This function initiates all the relevant data structures

DEPENDENCIES
Must be called before any operations on the MS device 

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void init_data_structures()
{
  static boolean was_already_called = FALSE;
  if(was_already_called == FALSE)
  {
    was_already_called = TRUE;

    HSU_MSG_HIGH("init_data_structures(): entry",0,0,0);

    /*****************************************************************************
    * Block device info
    *****************************************************************************/ 
    hsu_al_ms_hotplug_block_device.media_is_present = 
      hsu_al_ms_hotplug_block_device_media_is_present;
    hsu_al_ms_hotplug_block_device.close = hsu_al_ms_hotplug_block_device_close;
    hsu_al_ms_hotplug_block_device.get_capacity = 
      hsu_al_ms_hotplug_block_device_get_capacity;
    hsu_al_ms_hotplug_block_device.lock = hsu_al_ms_hotplug_block_device_lock;
    hsu_al_ms_hotplug_block_device.open = hsu_al_ms_hotplug_block_device_open;
    hsu_al_ms_hotplug_block_device.read = hsu_al_ms_hotplug_block_device_read;
    hsu_al_ms_hotplug_block_device.unlock = hsu_al_ms_hotplug_block_device_unlock;
    hsu_al_ms_hotplug_block_device.write = hsu_al_ms_hotplug_block_device_write;

#ifdef FEATURE_HS_USB_RAMDRIVE
#error code not present
#endif /* FEATURE_HS_USB_RAMDRIVE */
    /****************************************************************************/

    /*****************************************************************************
    * Initialize LUN info 
    *****************************************************************************/ 
    hsu_al_ms_devices[0].luns = &hsu_al_ms_luns[0];
    hsu_al_ms_devices[0].num_luns = 0 ;

    /* Init MMC/SDCC lun(s) */
    hsu_al_ms_init_hotplug_lun(HOTPLUG_TYPE_MMC,TRUE);

    /* Init FTL lun(s) */
    hsu_al_ms_init_hotplug_lun(HOTPLUG_TYPE_FTL,FALSE);

#ifdef FEATURE_HS_USB_RAMDRIVE
#error code not present
#endif /*FEATURE_HS_USB_RAMDRIVE*/

#ifdef FEATURE_HS_USB_UICC
    /* Initialize USB MS LUN (UICC) only if allowed by UIM NV item */
    if (hsu_uicc_is_enabled_by_nv())
    {
      hsu_al_ms_init_usb_hotplug_lun();
    }
#endif /* FEATURE_HS_USB_UICC */

    HSU_MSG_HIGH("init_data_structures(): exit",0,0,0);
  }
}

/*===========================================================================

FUNCTION hsu_al_ms_init

DESCRIPTION
This function initializes the adaptation layer. 
It is called when the task initializes.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_ms_init(void)
{
  HSU_MSG_HIGH("Starting hsu_al_ms_init()",0,0,0);

  /* Index of next entry to be used in the device array */
  hsu_al_ms_device_index = 0;

  hsu_al_ms_enabled = FALSE;

  /* initialize command queue */
  (void)q_init(&hsu_al_ms_cmd_q);

  /* Create timers */
  /* Dog */
  rex_def_timer( &hsu_al_ms_rpt_timer, &hsu_al_ms_tcb, HSU_AL_MS_RPT_TIMER_SIG );
  (void)rex_set_timer ( &hsu_al_ms_rpt_timer, DOG_HSU_AL_MS_RPT_TIME );
  /* Polling */
  rex_def_timer( &hsu_al_ms_polling_timer, &hsu_al_ms_tcb, HSU_AL_MS_POLLING_SIG );

  HSU_MSG_HIGH("Exiting hsu_al_ms_init()",0,0,0);
} /* hsu_al_ms_init */

/*===========================================================================

FUNCTION hsu_al_ms_dog_rpt

DESCRIPTION
This function kicks the watchdog timer.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_ms_dog_rpt(void)
{
  dog_report(DOG_HSU_AL_MS_RPT); 
  (void) rex_set_timer ( &hsu_al_ms_rpt_timer, DOG_HSU_AL_MS_RPT_TIME );
}

/*===========================================================================

FUNCTION hsu_al_ms_task

DESCRIPTION
This is the task function for the mass storage adaptation layer task.
It is passed in the call to rex_def_task that creates the task.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_al_ms_task
(
 dword parm       /* Ignored */
 )
{
  rex_sigs_type       rex_sigs; /* REX signals word  */

  HSU_USE_PARAM(parm);

  /* Perform initialization */
  hsu_al_ms_init();

  task_start(
    HSU_AL_MS_RPT_TIMER_SIG,           /* report timer signal for task */
    DOG_HSU_AL_MS_RPT,                 /* watchdog report */
    &hsu_al_ms_rpt_timer               /* pointer to report timer */
    );

  /* Never exit this loop... */
  for (;;)
  { /*  while (TRUE) and Lint happy */

    /* Set REX wait with mask of all possible input signals */
    rex_sigs = rex_wait(
      HSU_AL_MS_RPT_TIMER_SIG |
      HSU_AL_MS_CMD_Q_SIG |
      HSU_AL_MS_POLLING_SIG |
      TASK_STOP_SIG |         /* Stop the task */
      TASK_OFFLINE_SIG);      /* Go off line */

    if (rex_sigs & HSU_AL_MS_RPT_TIMER_SIG)
    {
      /* Clear it */
      (void)rex_clr_sigs(&hsu_al_ms_tcb, HSU_AL_MS_RPT_TIMER_SIG);

      /* Kick the watchdog */
      hsu_al_ms_dog_rpt();
    }

    if (rex_sigs & HSU_AL_MS_CMD_Q_SIG)
    {
      /* Clear it */
      (void)rex_clr_sigs(&hsu_al_ms_tcb, HSU_AL_MS_CMD_Q_SIG);

      /* Execute all commands in the queue */
      while (execute_next_cmd()){}
    }

    if (rex_sigs & HSU_AL_MS_POLLING_SIG)
    {
      /* Clear it */
      (void)rex_clr_sigs(&hsu_al_ms_tcb, HSU_AL_MS_POLLING_SIG);

      perform_polling();

      /* Reset the polling timer */
      (void)rex_set_timer(&hsu_al_ms_polling_timer, POLLING_TIMEOUT_MILLISECS);
    }

    if (rex_sigs & TASK_STOP_SIG)
    {
      /* Clear it */
      (void)rex_clr_sigs(&hsu_al_ms_tcb, TASK_STOP_SIG);

      hsu_al_ms_stop();

      /* Signal back to TMC */
      task_stop();
    }

    if (rex_sigs & TASK_OFFLINE_SIG)
    {
      /* Clear it */
      (void)rex_clr_sigs(&hsu_al_ms_tcb, TASK_OFFLINE_SIG);

      hsu_al_ms_stop();

      /* Signal back to TMC */
      task_offline();
    }
  }
} /* hsu_al_ms_task */

/************************************************************************/
/* Block Device <-> MSFD Interface                 
*************************************************************************/

/*===========================================================================

FUNCTION device_index_is_valid

DESCRIPTION
This is a helper function, not part of the API for the function driver.
It checks if the given index is a valid device index.

DEPENDENCIES
None.

RETURN VALUE
TRUE if the index is valid, FALSE otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static boolean device_index_is_valid
(
 juint8_t dev_index
 )
{
  return (dev_index < NUM_DEVICES);
}

/*===========================================================================

FUNCTION get_lun_ptr

DESCRIPTION
This is a helper function, not part of the API for the function driver.
It retrieves a pointer to the LUN info that corresponds to the given
(device index, lun index) pair.

DEPENDENCIES
None.

RETURN VALUE
NULL if the (device index, lun index) pair is invalid.
Otherwise, a pointer to the corresponding LUN info.

SIDE EFFECTS
None.
===========================================================================*/
static hsu_al_ms_lun_info_type* get_lun_ptr
(
 juint8_t dev_index, 
 juint8_t lun_index
 )
{
  if (!device_index_is_valid(dev_index))
  {
    return NULL;
  }

  if (lun_index >= hsu_al_ms_devices[dev_index].num_luns)
  {
    return NULL;
  }

  return &hsu_al_ms_devices[dev_index].luns[lun_index];
}

/*===========================================================================

FUNCTION vlun_count

DESCRIPTION
This function returns (the number of LUNs in the specified device)-1

DEPENDENCIES
None.

RETURN VALUE
(the number of LUNs in the specified device)-1 if the device index is legal,
EINVAL otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t vlun_count
(
 msfd_appctx_t ctx,
 juint8_t*  count
 )
{
  uint8 dev_index = (uint8)((uint32)ctx);

  if (!device_index_is_valid(dev_index) || (count==NULL))
  {
    HSU_MSG_ERROR("vlun_count(): Invalid dev_index or count.",0,0,0);
    return JEINVAL;
  }

  *count = hsu_al_ms_devices[dev_index].num_luns;
  return 0;
}

/*===========================================================================

FUNCTION vget_desc

DESCRIPTION
This function fills in (in out parameters)
required information for INQUIRY responses.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void vget_desc
(
 msfd_appctx_t ctx,
 juint8_t   vlun, 
 /* out parameters that need to be filled in by the function */
 juint32_t* dev_type, 
 juint8_t*   version,
 char**     vendor,
 char**     product,
 char**     prod_rev
 )
{
  HSU_USE_PARAM(ctx);
  HSU_USE_PARAM(vlun);

  *dev_type = 0;
  *version = HSU_AL_MS_IMPLEMENTED_SCSI_VERSION;
  *vendor = inquiry_data_vendor;
  *product = inquiry_data_product;
  *prod_rev = inquiry_data_prod_rev;
}

/*===========================================================================

FUNCTION vready

DESCRIPTION
This function checks whether the underlying block devices is ready
for performing i/o operations.

DEPENDENCIES
None.

RETURN VALUE
TRUE (1) if ready, FALSE (0) if not, negative number for error. 
Note that if the media has changed and there has not been a call to vchanged() since, 
FALSE is returned.

SIDE EFFECTS
None.
===========================================================================*/
static jbool_t vready
(
 msfd_appctx_t ctx,
 juint8_t vlun
 )
{
  hsu_al_ms_lun_info_type* lun_ptr = get_lun_ptr((uint8)((uint32)ctx), vlun);

  if (lun_ptr == NULL)
  {
    /* This should not happen */
    HSU_ASSERT(lun_ptr!=NULL);
    return FALSE;
  }

  if (!lun_ptr->enabled)
  {
    return FALSE;
  }

  if (!lun_ptr->removable)
  {
    /* For removable devices, polling is done periodically. 
    ** For other devices, we need to do it here. */
    poll_lun(lun_ptr);
  }

  return (jbool_t)((lun_ptr->status==LUN_STATUS_READY) && 
    !lun_ptr->media_change_requested);
}

/*===========================================================================

FUNCTION vchanged

DESCRIPTION
This function checks whether the media of the underlying device has changed
(that is, removed and then inserted) since the last call to vchanged() for the 
specified LUN.

DEPENDENCIES
None.

RETURN VALUE
TRUE (1) if media has changed, FALSE (0) if not, negative number for error. 
TRUE is returned only for the first call to vchanged() after the media changes.

SIDE EFFECTS
None.
===========================================================================*/
static jbool_t vchanged
(
 msfd_appctx_t ctx,
 juint8_t vlun
 )
{
  jbool_t media_changed;
  hsu_al_ms_lun_info_type* lun_ptr = get_lun_ptr((uint8)((uint32)ctx), vlun);

  if (lun_ptr == NULL)
  {
    /* This should not happen */
    HSU_ASSERT(lun_ptr!=NULL);
    return FALSE;
  }

  media_changed = (jbool_t)(lun_ptr->status==LUN_STATUS_MEDIA_CHANGED);
  if (media_changed)
  {
    HSU_MSG_HIGH("vchanged(): returning TRUE for lun=%d",vlun,0,0);
    lun_ptr->status = LUN_STATUS_READY;
  }

  return media_changed;
}

/*===========================================================================

FUNCTION vmount

DESCRIPTION
This function performs a mount operation. Currently, this is not supported.

DEPENDENCIES
None.

RETURN VALUE
EOPNOTSUPP (error number defined in the USB stack)

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t vmount
(
 msfd_appctx_t ctx, 
 juint8_t vlun
 )
{
  hsu_al_ms_lun_info_type* lun_ptr = get_lun_ptr((uint8)((uint32)ctx), vlun);

  if (lun_ptr == NULL)
  {
    /* This should not happen */
    HSU_ASSERT(lun_ptr!=NULL);
    return JEINVAL;
  }

  if (lun_ptr->removable)
  {
    lun_ptr->media_change_requested = FALSE;
  }

  return (lun_ptr->status != LUN_STATUS_NOT_READY) ? 0 : JEINVAL;
}

/*===========================================================================

FUNCTION vunmount

DESCRIPTION
This function performs an unmount operation. Currently, this is not supported.

DEPENDENCIES
None.

RETURN VALUE
EOPNOTSUPP (error number defined in the USB stack)

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t vunmount
(
 msfd_appctx_t ctx,
 juint8_t vlun
 )
{
  hsu_al_ms_lun_info_type* lun_ptr = get_lun_ptr((uint8)((uint32)ctx), vlun);

  if (lun_ptr == NULL)
  {
    /* This should not happen */
    HSU_ASSERT(lun_ptr!=NULL);
    return JEINVAL;
  }

  if (lun_ptr->removable)
  {
    lun_ptr->media_change_requested = TRUE;
    return 0;
  }

  return JEOPNOTSUPP;
}

/*===========================================================================

FUNCTION vopen

DESCRIPTION
This function stores the received information in the data structures
and opens the device for exclusive access.

DEPENDENCIES
None.

RETURN VALUE
0 for success, negative number for error.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t vopen
(
 msfd_appctx_t        ctx,
 juint8_t             vlun,
 juint8_t*            removable,
 completion_callback  callback,
 void*                arg
 )
{
  hsu_al_ms_lun_info_type* lun_ptr = get_lun_ptr((uint8)((uint32)ctx), vlun);
  hsu_al_ms_block_device_info_type* block_device_ptr;

  HSU_MSG_HIGH("Starting vopen() for lun %d",vlun,0,0);
  if (lun_ptr == NULL)
  {
    HSU_MSG_ERROR("vopen(): NULL lun_ptr. vlun=%d",vlun,0,0);
    return JEINVAL;
  }
  block_device_ptr = lun_ptr->block_device_ptr;

  /* If this is the first LUN (of any device) to be opened,
  ** we need to call hsu_al_ms_enable()
  */
  if (!hsu_al_ms_enabled)
  {
    hsu_al_ms_enable();
    hsu_al_ms_enabled = TRUE;
  }

  if (!lun_ptr->enabled)
  {
    HSU_MSG_ERROR("vopen(): LUN is disabled. vlun=%d",vlun,0,0);

    /* let the stack continue with the enumeration anyway */
    return 0;
  }

  lun_ptr->io_completion_callback = callback;
  lun_ptr->io_completion_callback_arg = arg;
  /* Note that *removable=0 was never actually tested because
  Windows doesn't "like" USB Mass Storage block devices 
  which are marked as non-removable. */
  *removable = 1;

  if (!lun_ptr->removable)
  {
    /* For removable devices, polling is done periodically. 
    ** For other devices, we need to do it here. */
    poll_lun(lun_ptr);

    if (!block_device_ptr->open(lun_ptr->context))
    {
      HSU_MSG_ERROR("vopen(): Failed to open block device. vlun=%d",vlun,0,0);
      /* let the stack continue with the enumeration anyway */
      return 0;
    }
  }

  HSU_MSG_HIGH("Exiting vopen() for lun %d",vlun,0,0);
  return 0;
} /* vopen */

/*===========================================================================

FUNCTION vget_media_details

DESCRIPTION
This function retrieves information regarding the media currently inserted
into the underlying block device of the specified LUN.

DEPENDENCIES
None.

RETURN VALUE
0 for success, negative number for error.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t vget_media_details
(
 msfd_appctx_t ctx,
 juint8_t      vlun, 
 juint32_t     *block_size, 
 juint32_t     *num_blocks,
 juint32_t     *read_only
 )
{
  hsu_al_ms_lun_info_type* lun_ptr = get_lun_ptr((uint8)((uint32)ctx), vlun);

  if (lun_ptr == NULL)
  {
    HSU_MSG_ERROR("vget_media_details(): NULL lun_ptr. vlun=%d",vlun,0,0);
    return JEINVAL;
  }

  if (lun_ptr->status == LUN_STATUS_NOT_READY)
  {
    HSU_MSG_ERROR("vget_media_details(): LUN is not ready. vlun=%d",vlun,0,0);
    return JENOMEDIUM;
  }

  *block_size = (juint32_t)lun_ptr->block_size;
  *num_blocks = (juint32_t)lun_ptr->num_blocks;
  *read_only  = (juint32_t)lun_ptr->read_only;

  HSU_MSG_HIGH("vget_media_details(): lun=%d, block_size=%d, num_blocks=%d",
    vlun,*block_size,*num_blocks);
  return 0;
} /* vget_media_details */

/*===========================================================================

FUNCTION vclose

DESCRIPTION
This function relinquishes exclusive access to the device and closes it
(if the device supports such an operation)

DEPENDENCIES
None.

RETURN VALUE
0 for success, negative number for error.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t vclose
(
 msfd_appctx_t ctx,
 juint8_t vlun
 )
{
  hsu_al_ms_lun_info_type* lun_ptr = get_lun_ptr((uint8)((uint32)ctx), vlun);
  hsu_al_ms_block_device_info_type* block_device_ptr;

  HSU_MSG_HIGH("Starting vclose() for lun %d",vlun,0,0);

  if (lun_ptr == NULL)
  {
    HSU_MSG_ERROR("vclose(): NULL lun_ptr. vlun=%d",vlun,0,0);
    return JEINVAL;
  }
  block_device_ptr = lun_ptr->block_device_ptr;

  if (!lun_ptr->enabled)
  {
    HSU_MSG_ERROR("vclose(): LUN is disabled. vlun=%d",vlun,0,0);
    /* let the stack continue anyway */
    return 0;
  }

  if (!lun_ptr->removable)
  {
    if (!block_device_ptr->close(lun_ptr->context))
    {
      HSU_MSG_ERROR("vclose(): Failed to close block device. vlun=%d",vlun,0,0);
      /* let the stack continue anyway */
      return 0;
    }
  }

  HSU_MSG_HIGH("Exiting vclose() for lun %d",vlun,0,0);
  return 0;
} /* vclose */

/*===========================================================================

FUNCTION vread

DESCRIPTION
This function reads blocks from a LUN. The function is asynchronous: 
it returns immediately, and the read operation is done afterwards, 
using a command queue. When the actual read operation is done, 
a completion callback is called.

DEPENDENCIES
vopen() for the specified LUN should be called first, 
in order to set up the completion callback.

RETURN VALUE
0 for success, negative number for error.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t vread
(
 msfd_appctx_t ctx, /* adaptation layer context */
 juint8_t      vlun,  /* LUN index */
 juint8_t*     buffer, /* buffer to read into */
 juint32_t     amount, /* number of blocks to read */
 juint32_t     offset /* index of block to start from (zero based) */
 )
{
  uint8 dev_index = (uint8)((uint32)ctx);
  hsu_al_ms_lun_info_type* lun_ptr = get_lun_ptr(dev_index, vlun);

  HSU_ASSERT(amount <= MAX_UINT16);
  if (lun_ptr == NULL)
  {
    HSU_MSG_ERROR("vread(): NULL lun_ptr. vlun=%d",vlun,0,0);
    return JEINVAL;
  }

  /* Set up parameters for the read operation */
  lun_ptr->read_arg.device_index = dev_index;
  lun_ptr->read_arg.lun_index = vlun;
  lun_ptr->read_arg.num_blocks = (uint16)amount;
  lun_ptr->read_arg.start_address = offset;
  lun_ptr->read_arg.buff = buffer;

  enqueue_cmd(&(lun_ptr->read_cmd));

  return 0;
} /* vread */

/*===========================================================================

FUNCTION vwrite

DESCRIPTION
This function writes blocks to a LUN. The function is asynchronous: 
it returns immediately, and the write operation is done afterwards, 
using a command queue. When the actual write operation is done, 
a completion callback is called.

DEPENDENCIES
vopen() for the specified LUN should be called first, 
in order to set up the completion callback.

RETURN VALUE
0 for success, negative number for error.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t vwrite
(
 msfd_appctx_t ctx, /* Adaptation layer context */
 juint8_t      vlun,  /* LUN index */
 juint8_t*     buffer, /* buffer to write into */
 juint32_t     amount, /* number of blocks to write */
 juint32_t     offset /* index of block to start from (zero based) */
 )
{
  uint8 dev_index = (uint8)((uint32)ctx);
  hsu_al_ms_lun_info_type* lun_ptr = get_lun_ptr(dev_index, vlun);

  HSU_ASSERT(amount <= MAX_UINT16);
  if (lun_ptr == NULL)
  {
    HSU_MSG_ERROR("vwrite(): NULL lun_ptr. vlun=%d",vlun,0,0);
    return JEINVAL;
  }

  lun_ptr->write_arg.device_index = dev_index;
  lun_ptr->write_arg.lun_index = vlun;
  lun_ptr->write_arg.num_blocks = (uint16)amount;
  lun_ptr->write_arg.start_address = offset;
  lun_ptr->write_arg.buff = buffer;

  enqueue_cmd(&(lun_ptr->write_cmd));

  return 0;
} /* vwrite */

/*===========================================================================

FUNCTION vverify

DESCRIPTION
This function is intended for verifying that a sequence of blocks
on the block device can be read. No special operation is needed, 
since success or failure of read and write operations is reported in the 
completion callback. Thus, we assume that the verification always succeeds.

DEPENDENCIES
None.

RETURN VALUE
The amount of blocks that were verified.

SIDE EFFECTS
None.
===========================================================================*/
static jint32_t vverify
(
 msfd_appctx_t ctx,
 juint8_t      vlun,
 juint32_t     amount,
 juint32_t     offset
 )
{
  HSU_USE_PARAM(ctx);
  HSU_USE_PARAM(vlun);
  HSU_USE_PARAM(offset);

  /* Report success */
  return (jint32_t)amount;
}

/*===========================================================================

FUNCTION vcancel

DESCRIPTION
This function is intended for canceling an i/o operation.
Currently,  all i/o operations are performed synchronously 
with respect to the block devices, and thus no special operation is needed.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void vcancel
(
 msfd_appctx_t ctx, 
 juint8_t vlun
 )
{
  HSU_USE_PARAM(ctx);
  HSU_USE_PARAM(vlun);
}

/*===========================================================================

FUNCTION vflush

DESCRIPTION
When the underlying block device of a LUN uses cache,
this function is intended for committing data onto the block device.
Currently, we do not support cache, so this function does nothing.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t vflush
(
 msfd_appctx_t ctx, 
 juint8_t vlun
 )
{
  HSU_USE_PARAM(ctx);
  HSU_USE_PARAM(vlun);

  return 0;
}

/*===========================================================================

FUNCTION vallow_cache

DESCRIPTION
When the underlying block device of a LUN supports cache,
this function is intended for enabling or disabling caching.
Currently, we do not support cache, so this function does nothing.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t vallow_cache
(
 msfd_appctx_t ctx, 
 juint8_t      vlun, 
 juint8_t      flag
 )
{
  HSU_USE_PARAM(ctx);
  HSU_USE_PARAM(vlun);
  HSU_USE_PARAM(flag);

  return 0;
}

/*===========================================================================

FUNCTION vtransparent

DESCRIPTION
This function handle vendor-specific ("transparent") SCSI commands sent from
the host to the application.

DEPENDENCIES
None.

RETURN VALUE
0 on success; Sense data on failure.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t vtransparent
(
 msfd_appctx_t ctx,
 juint8_t vlun,
 void *cmd
 )
{
  HSU_USE_PARAM(ctx);
  HSU_USE_PARAM(vlun);
  HSU_USE_PARAM(cmd);

  return 0;
}

/******************************************************************************/

/*===========================================================================

FUNCTION msfd_app_uninit

DESCRIPTION
This function stops the adaptation layer's operation.

DEPENDENCIES
None.

RETURN VALUE
0 for success, negative number for error.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t msfd_app_uninit
(
 msfd_appctx_t ctx
 )
{
  HSU_USE_PARAM(ctx);

  HSU_MSG_HIGH("msfd_app_uninit(): entry",0,0,0);

  if (hsu_al_ms_enabled)
  {
    hsu_al_ms_disable();
    hsu_al_ms_enabled = FALSE;
  }

  HSU_MSG_HIGH("msfd_app_uninit(): exit",0,0,0);

  return 0;
}

/*===========================================================================

FUNCTION hsu_al_ms_get_device_index

DESCRIPTION
This function calculates the index of the MS instance in hsu_al_ms_devices
according to the number of cores and configs in each core.
Currently we use the same instance for all cores and configs.

DEPENDENCIES
None.

RETURN VALUE
The index of the MS instance in hsu_al_ms_devices.

SIDE EFFECTS
None.
===========================================================================*/
static uint8 hsu_al_ms_get_device_index(void)
{
  uint8 device_index_to_return = 0;

  /* For multi configuration implementation:
  For now we use the same instance of MS for all the configurations (assuming there 
  is the same number of instances in each).

  In order to use different instances of MS for each configuration one should 
  increase NUM_DEVICES accordingly (to allocate more memory in hsu_al_ms_devices)
  and change the implementation of this function to fit his algorithm */

  device_index_to_return = hsu_al_ms_device_index%NUM_DEVICES;
  ++hsu_al_ms_device_index;
  return device_index_to_return;
}
/*===========================================================================

FUNCTION msfd_app_init

DESCRIPTION
This function fills in the callbacks needed for the function driver's operation,
and kick-starts the adaptation layer's operation.

DEPENDENCIES
None.

RETURN VALUE
0 for success, negative number for error.

SIDE EFFECTS
None.
===========================================================================*/
jresult_t msfd_app_init
(
 /* A structure that needs to be filled with callbacks that the function driver
 ** will call in order to interact with the block devices.
 */
 msfd_ops_t *msfd_ops,
 /* Placeholder for the application context. We use the index to the device array. */
 msfd_appctx_t *ctx,
 /* the application can set interface number using this struct */
 msfd_init_info_t *info
 )
{
  HSU_MSG_HIGH("msfd_app_init(): entry",0,0,0);

  /* This function has a static flag to verify we initialize the data structures 
  only once. */
  init_data_structures();

  msfd_ops->vallow_cache = vallow_cache;
  msfd_ops->vcancel = vcancel;
  msfd_ops->vchanged = vchanged;
  msfd_ops->vclose = vclose;
  msfd_ops->vflush = vflush;
  msfd_ops->vget_desc = vget_desc;
  msfd_ops->vget_media_details = vget_media_details;
  msfd_ops->vlun_count = vlun_count;
  msfd_ops->vmount = vmount;
  msfd_ops->vopen = vopen;
  msfd_ops->vread = vread;
  msfd_ops->vready = vready;
  msfd_ops->vunmount = vunmount;
  msfd_ops->vverify = vverify;
  msfd_ops->vwrite = vwrite;
  msfd_ops->vtransparent = vtransparent;
  msfd_ops->scsi_in_handler = NULL;
  msfd_ops->handled_cmd_ind = NULL;
  msfd_ops->scsi_out_handler = NULL;
  msfd_ops->scsi_out_data_stage_handler = NULL;
  /* Note: unhandled_cmd_in is obsolete. scsi_in_handler should be used 
  in future development*/
  msfd_ops->unhandled_cmd_in = NULL;
  msfd_ops->msfd_app_uninit = msfd_app_uninit;

  info->scsi_buffer_size = HSU_SCSI_BUFFER_SIZE;

  *ctx = (msfd_appctx_t)hsu_al_ms_get_device_index();;

  /* hsu_al_ms_enable(); */
  if (hsu_al_ms_enabled)
  {

    HSU_MSG_HIGH("msfd_app_init(): ERROR. ctx returned to the stack is %d.",
      *ctx,0,0);
    return JEIO;
  }

  HSU_MSG_HIGH("msfd_app_init(): exit. ctx returned to the stack is %d.",
    *ctx,0,0);

  return 0;
}

void hsu_al_ms_disable_polling(void)
{
  (void)rex_clr_timer(&hsu_al_ms_polling_timer);
}
