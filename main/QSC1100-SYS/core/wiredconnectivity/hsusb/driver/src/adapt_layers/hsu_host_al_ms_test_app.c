/*==============================================================================

High Speed USB Host MS Test Application

GENERAL DESCRIPTION
Implementation of the High Speed USB Host MS Test Application.
In its initialization function, the application defines a task.
The task registers with the usbhost api for MS notifications. Then it waits for
a signal to start a test. This signal is sent to it when the host API notifies 
the test app that a mass storage was successfully "opened".
Currently, the test app is intended for working with a single connected MS device.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

#include "hsu_host_al_ms_test_app.h"
#include "hsu_common.h"
#include "usbhost_api.h"
#include <rex.h>
#include <task.h>
#include "time_svc.h"

#define STACK_SIZE_IN_BYTES 2048
#define HSU_HOST_AL_MS_TEST_APP_START_TEST_SIG 0x200
#define HSU_HOST_AL_MS_TEST_APP_TRANSFER_COMPLETION_SIG 0x400

#define READ_BUFFER_SIZE 65536 /* 64KB */

static boolean test_read_rate = TRUE; /* FALSE means testing write rate */

/* Stack and tcb for the task */
unsigned char hsu_host_al_ms_test_app_stack[STACK_SIZE_IN_BYTES];
rex_tcb_type  hsu_host_al_ms_test_app_tcb;
static usbhost_client_id_type hsu_host_al_ms_test_app_client_id;
static usbhost_dev_handle_type hsu_host_al_ms_test_app_dev_handle = USBHOST_INVALID_DEV_HNDL;
static usbhost_dev_instance_type hsu_host_al_ms_test_app_dev_inst;
static uint16 hsu_host_al_ms_test_app_num_read_iterations = 1;
static float hsu_host_al_ms_test_app_read_rate;
static float hsu_host_al_ms_test_app_write_rate;
static boolean hsu_host_al_ms_test_app_dev_connected = FALSE;
static uint32 hsu_host_al_ms_test_app_num_blocks;
static uint32 hsu_host_al_ms_test_app_block_size;

static uint8 hsu_host_al_ms_test_app_read_buffer_index = 0;
static uint8 hsu_host_al_ms_test_app_read_buffer0[READ_BUFFER_SIZE];
static uint8 hsu_host_al_ms_test_app_read_buffer1[2*READ_BUFFER_SIZE];
static uint8 hsu_host_al_ms_test_app_read_buffer2[4*READ_BUFFER_SIZE];
static uint8 hsu_host_al_ms_test_app_read_buffer3[8*READ_BUFFER_SIZE];

static uint8* hsu_host_al_ms_test_app_read_buffer[4] = {hsu_host_al_ms_test_app_read_buffer0,
                                                        hsu_host_al_ms_test_app_read_buffer1,
                                                        hsu_host_al_ms_test_app_read_buffer2,
                                                        hsu_host_al_ms_test_app_read_buffer3
                                                        };
static uint32 hsu_host_al_ms_test_app_read_buffer_size[4] = {READ_BUFFER_SIZE, 
                                                             2*READ_BUFFER_SIZE,                                                              
                                                             4*READ_BUFFER_SIZE,
                                                             8*READ_BUFFER_SIZE
                                                             };

/*===========================================================================

FUNCTION hsu_host_al_ms_test_app_close_req_cb

DESCRIPTION
This function is called when another client tries to open a device
that is currently owned by the High Speed USB Host MS test application.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static void hsu_host_al_ms_test_app_close_req_cb
(
  usbhost_dev_handle_type dev_handle,
  char* asking_client_desc,
  usbhost_client_id_type asking_client_id,
  void* param
)
{
  HSU_USE_PARAM(dev_handle);
  HSU_USE_PARAM(asking_client_desc);
  HSU_USE_PARAM(asking_client_id);
  HSU_USE_PARAM(param);
}

/*===========================================================================

FUNCTION hsu_host_al_ms_test_app_open_notify_cb

DESCRIPTION
A callback that notifies the High Speed USB Host MS Test Application
of the result of a request to open a connected device.

DEPENDENCIES
None.

RETURN VALUE
Relevant only if the open_status parameter is USBHOST_OPEN_NOT_AVAILABLE.
Returning USBHOST_OPEN_RET_RESCHEDULE means that we still want to be queued 
for opening the device and returning USBHOST_OPEN_RET_UNSCHEDULE
means that we no longer want to open the device.
We always return USBHOST_OPEN_RET_RESCHEDULE.

SIDE EFFECTS
None.

===========================================================================*/
static usbhost_open_ret_type hsu_host_al_ms_test_app_open_notify_cb
(
  usbhost_dev_instance_type dev_inst, 
  usbhost_dev_handle_type dev_handle,
  usbhost_open_status open_status,
  char* holding_client_desc,
  void* param
)
{
  HSU_USE_PARAM(param);
  HSU_USE_PARAM(holding_client_desc);

  if (open_status == USBHOST_OPEN_AVAILABLE)
  {
    /* Save the device handle */
    hsu_host_al_ms_test_app_dev_handle = dev_handle;

    (void)rex_set_sigs(&hsu_host_al_ms_test_app_tcb, 
                       HSU_HOST_AL_MS_TEST_APP_START_TEST_SIG);
  }

  return USBHOST_OPEN_RET_RESCHEDULE;
}

/*===========================================================================

FUNCTION hsu_host_al_ms_test_app_dev_notify_cb

DESCRIPTION
A callback that notifies the High Speed USB Host MS Test Application
when a mass storage device is connected or disconnected.
Upon connection, this function requests to open the device.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static void hsu_host_al_ms_test_app_dev_notify_cb
(
  usbhost_pdev_type pdev_type, 
  usbhost_dev_status_type dev_stat, 
  usbhost_dev_instance_type dev_inst,
  void * param
 )
{
  usbhost_status_type status;

  HSU_USE_PARAM(param);

  if (dev_stat ==  USBHOST_DEV_CONNECTED)
  {
    if (hsu_host_al_ms_test_app_dev_connected == TRUE)
    {
      /* We can only handle a single connected device */
      return;
    }

    hsu_host_al_ms_test_app_dev_connected = TRUE;
    hsu_host_al_ms_test_app_dev_inst = dev_inst;
    /* Device connected --> open it */
    status = usbhost_req_open(hsu_host_al_ms_test_app_client_id,
                              dev_inst,
                              hsu_host_al_ms_test_app_open_notify_cb,
                              hsu_host_al_ms_test_app_close_req_cb);
    if (status != USBHOST_SUCCESS)
    {
      HSU_MSG_ERROR("hsu_host_al_ms_test_app_dev_notify_cb(): usbhost_req_open() failed.",
        0,0,0);
    }
  }
  else
  {
    if (dev_inst == hsu_host_al_ms_test_app_dev_inst)
    {
      hsu_host_al_ms_test_app_dev_handle = USBHOST_INVALID_DEV_HNDL;
      hsu_host_al_ms_test_app_dev_connected = FALSE;
    }
  }
}

/*===========================================================================

FUNCTION hsu_host_al_ms_test_app_test_read_rate

DESCRIPTION
Read blocks from the connected device and calculate the throughput.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_ms_test_app_test_read_rate(void)
{
  uint64                        start_time, end_time;
  usbhost_status_type           status;
  uint32                        i;
  usbhost_IoctlTransfer         transfer_params;
  uint32                        blocks_in_buffer, blocks_to_read, remaining_blocks;
  uint32                        block_offset;
  double                        megabits, secs;

  HSU_MSG_HIGH("hsu_host_al_ms_test_app_test_read_rate: entry.", 0,0,0);

  hsu_host_al_ms_test_app_read_rate = 0;
  blocks_in_buffer = 
    hsu_host_al_ms_test_app_read_buffer_size[hsu_host_al_ms_test_app_read_buffer_index] / hsu_host_al_ms_test_app_block_size;
  start_time = time_get_ms_native();

  for (i=0; i<hsu_host_al_ms_test_app_num_read_iterations; i++)
  {
    remaining_blocks = hsu_host_al_ms_test_app_num_blocks;
    block_offset = 0;
    while (remaining_blocks > 0) 
    {
      blocks_to_read = MIN(blocks_in_buffer, remaining_blocks);
      transfer_params.buffer = hsu_host_al_ms_test_app_read_buffer[hsu_host_al_ms_test_app_read_buffer_index];
      transfer_params.length = blocks_to_read * hsu_host_al_ms_test_app_block_size;
      transfer_params.startBlock = block_offset;
      transfer_params.numberBlocks = blocks_to_read;
      
      status = usbhost_ioctl(hsu_host_al_ms_test_app_client_id,
                             hsu_host_al_ms_test_app_dev_handle,
                             USBHOST_IOCTL_STOR_BLOCK_READ,
                             (void *) &transfer_params);

      if (status != USBHOST_SUCCESS)
      {
        HSU_ASSERT("Failed to read"==NULL);
        HSU_MSG_ERROR("hsu_host_al_ms_test_app_test_read_rate: Failed to read %d blocks from offset %d.",
                       blocks_to_read, block_offset, 0);
      }

      block_offset += blocks_to_read;
      remaining_blocks -= blocks_to_read;
    }
  }

  end_time = time_get_ms_native();

  {
    uint64 tmp1 = (uint64)hsu_host_al_ms_test_app_num_read_iterations * 
                  hsu_host_al_ms_test_app_num_blocks * hsu_host_al_ms_test_app_block_size * 8;
    float tmp2 = 1024.0*1024.0;
    megabits =  tmp1/tmp2;
  }
  secs = (end_time - start_time)/1000.0;
  hsu_host_al_ms_test_app_read_rate = (float)(megabits / secs);
  HSU_MSG_HIGH("hsu_host_al_ms_test_app_test_read_rate: Done reading.", 0, 0, 0);
  HSU_MSG_HIGH("hsu_host_al_ms_test_app_test_read_rate: Rate is %f.", hsu_host_al_ms_test_app_read_rate, 0, 0);
} /* hsu_host_al_ms_test_app_test_read_rate */

/*===========================================================================

FUNCTION hsu_host_al_ms_test_app_test_write_rate

DESCRIPTION
Write blocks to the connected device and calculate the throughput.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_host_al_ms_test_app_test_write_rate(void)
{
  uint64                        start_time, end_time;
  usbhost_status_type           status;
  uint32                        i;
  usbhost_IoctlTransfer         transfer_params;
  uint32                        blocks_in_buffer, blocks_to_write, remaining_blocks;
  uint32                        block_offset;
  double                        megabits, secs;

  HSU_MSG_HIGH("hsu_host_al_ms_test_app_test_write_rate: entry.", 0,0,0);

  hsu_host_al_ms_test_app_write_rate = 0;
  blocks_in_buffer = 
    hsu_host_al_ms_test_app_read_buffer_size[hsu_host_al_ms_test_app_read_buffer_index] / hsu_host_al_ms_test_app_block_size;
  start_time = time_get_ms_native();

  for (i=0; i<hsu_host_al_ms_test_app_num_read_iterations; i++)
  {
    remaining_blocks = hsu_host_al_ms_test_app_num_blocks;
    block_offset = 0;
    while (remaining_blocks > 0) 
    {
      blocks_to_write = MIN(blocks_in_buffer, remaining_blocks);
      transfer_params.buffer = hsu_host_al_ms_test_app_read_buffer[hsu_host_al_ms_test_app_read_buffer_index];
      transfer_params.length = blocks_to_write * hsu_host_al_ms_test_app_block_size;
      transfer_params.startBlock = block_offset;
      transfer_params.numberBlocks = blocks_to_write;

      status = usbhost_ioctl(hsu_host_al_ms_test_app_client_id,
        hsu_host_al_ms_test_app_dev_handle,
        USBHOST_IOCTL_STOR_BLOCK_WRITE,
        (void *) &transfer_params);

      if (status != USBHOST_SUCCESS)
      {
        HSU_ASSERT("Failed to write"==NULL);
        HSU_MSG_ERROR("hsu_host_al_ms_test_app_test_write_rate: Failed to write %d blocks from offset %d.",
          blocks_to_write, block_offset, 0);
      }

      block_offset += blocks_to_write;
      remaining_blocks -= blocks_to_write;
    }
  }

  end_time = time_get_ms_native();

  {
    uint64 tmp1 = (uint64)hsu_host_al_ms_test_app_num_read_iterations * 
      hsu_host_al_ms_test_app_num_blocks * hsu_host_al_ms_test_app_block_size * 8;
    float tmp2 = 1024.0*1024.0;
    megabits =  tmp1/tmp2;
  }
  secs = (end_time - start_time)/1000.0;
  hsu_host_al_ms_test_app_write_rate = (float)(megabits / secs);
  HSU_MSG_HIGH("hsu_host_al_ms_test_app_test_write_rate: Done writing.", 0, 0, 0);
  HSU_MSG_HIGH("hsu_host_al_ms_test_app_test_write_rate: Rate is %f.", hsu_host_al_ms_test_app_write_rate, 0, 0);
} /* hsu_host_al_ms_test_app_test_write_rate */

/*===========================================================================

FUNCTION hsu_host_al_ms_test_app_task

DESCRIPTION
This is the task function for the host mass storage adaptation layer test app 
task. It is passed in the call to rex_def_task that creates the task.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_host_al_ms_test_app_task
(
  dword parm       /* Ignored */
)
{
  rex_sigs_type       rex_sigs; /* REX signals word  */
  usbhost_task_table_type client_sigs;
  usbhost_status_type status;

  HSU_USE_PARAM(parm);

  client_sigs.dog_rpt_fn_ptr = NULL;
  client_sigs.dog_rpt_sig = 0;
  client_sigs.sem_wait_sig = HSU_HOST_AL_MS_TEST_APP_TRANSFER_COMPLETION_SIG;
  client_sigs.wait_sig = 0;

  hsu_host_al_ms_test_app_client_id = usbhost_register(
                                      USBHOST_PDEV_TYPE_MASS_STORAGE,
                                      "HSU HOST AL MS TEST APP",
                                      client_sigs,
                                      hsu_host_al_ms_test_app_dev_notify_cb,
                                      NULL);

  if (hsu_host_al_ms_test_app_client_id == USBHOST_INVALID_CLIENT_ID)
  {
    HSU_MSG_ERROR("hsu_host_al_ms_test_app_task(): Can't register for MS notifications.",
      0,0,0);
    return;
  }

  for (;;)
  { /*  while (TRUE) and Lint happy */

    /* Set REX wait with mask of all possible input signals */
    rex_sigs = rex_wait(
      HSU_HOST_AL_MS_TEST_APP_START_TEST_SIG);

    if (rex_sigs & HSU_HOST_AL_MS_TEST_APP_START_TEST_SIG)
    {
      boolean media_is_present, ok_to_proceed;
      uint8 i;

      /* Clear the signal */
      (void)rex_clr_sigs(&hsu_host_al_ms_test_app_tcb, HSU_HOST_AL_MS_TEST_APP_START_TEST_SIG);

      media_is_present = FALSE;
      ok_to_proceed = TRUE;
      /* Wait for media to be detected */
      for (i=0; i<6; i++)
      { 
        rex_sleep(500);
        status = usbhost_ioctl(hsu_host_al_ms_test_app_client_id,
          hsu_host_al_ms_test_app_dev_handle,
          USBHOST_IOCTL_STOR_CHECK_MEDIA,
          NULL);
        if (status == USBHOST_ERROR)
        {
          HSU_ASSERT("Can't check media"==NULL);
          HSU_MSG_ERROR("hsu_host_al_ms_test_app_task: Can't check media.",
            0,0,0);
          ok_to_proceed = FALSE;
          break;
        }
        else if (status != USBHOST_ERROR_MEDIA_NOT_PRESENT)
        {
          media_is_present = TRUE;
          break;
        }
      }

      if (!media_is_present)
      {
        HSU_MSG_HIGH("hsu_host_al_ms_test_app_task: no media.",0,0,0);
        ok_to_proceed = FALSE;
      }

      if (ok_to_proceed)
      {
        status = usbhost_ioctl(hsu_host_al_ms_test_app_client_id,
          hsu_host_al_ms_test_app_dev_handle,
          USBHOST_IOCTL_STOR_NUMBER_OF_BLOCKS,
          (void *) &hsu_host_al_ms_test_app_num_blocks);
        if (status != USBHOST_SUCCESS)
        {
          HSU_ASSERT("Can't get number of blocks"==NULL);
          HSU_MSG_ERROR("hsu_host_al_ms_test_app_task: Can't get number of blocks.",
            0,0,0);
          ok_to_proceed = FALSE;
        }
      }

      if (ok_to_proceed)
      {
        status = usbhost_ioctl(hsu_host_al_ms_test_app_client_id,
          hsu_host_al_ms_test_app_dev_handle,
          USBHOST_IOCTL_STOR_BLOCK_SIZE,
          (void *) &hsu_host_al_ms_test_app_block_size);
        if (status != USBHOST_SUCCESS)
        {
          HSU_ASSERT("Can't get block size"==NULL);
          HSU_MSG_ERROR("hsu_host_al_ms_test_app_task: Can't get block size.",
            0,0,0);
          ok_to_proceed = FALSE;
        }
      }

      if (ok_to_proceed)
      {
        HSU_MSG_HIGH("hsu_host_al_ms_test_app_task: num_blocks=%d size=%d",
          hsu_host_al_ms_test_app_num_blocks, hsu_host_al_ms_test_app_block_size, 0);

        /* Start the test */
        if (test_read_rate)
        {
          hsu_host_al_ms_test_app_test_read_rate();
        }
        else
        {
          hsu_host_al_ms_test_app_test_write_rate();
        }
      }
    }
  }
}

/*===========================================================================

FUNCTION hsu_host_al_ms_test_app_init

DESCRIPTION
Initialization function. Creates the test app's task.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_host_al_ms_test_app_init(void)
{
  rex_def_task_ext(&hsu_host_al_ms_test_app_tcb,
    hsu_host_al_ms_test_app_stack,
    STACK_SIZE_IN_BYTES,
    TASK_PRIORITY(HSU_OTHER_PRI_ORDER),
    hsu_host_al_ms_test_app_task,
    0L,
    "HSU HOST AL MS TEST APP",
    FALSE);
}
