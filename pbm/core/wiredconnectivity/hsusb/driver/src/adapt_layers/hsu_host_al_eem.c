/*==============================================================================

High Speed USB Host CDC/EEM Adaptation Layer 

GENERAL DESCRIPTION
Implementation of the High Speed USB CDC/EEM Adaptation Layer.
This includes implementation of the API functions defined by the CDC/EEM 
class driver in uw_host_cdc_eem.h (such as callback functions), and the SIO API 
exposed to the applications.

EXTERNALIZED FUNCTIONS
hsu_host_al_eem_init
hsu_host_al_eem_data_open
hsu_host_al_eem_data_close
hsu_host_al_eem_data_transmit
hsu_host_al_eem_ioctl
hsu_host_al_eem_data_flush_tx

INITALIZATION AND SEQUENCING REQUIREMENTS
The HS-USB stack must be initialized and call hsu_host_al_eem_init, before all 
other functions in this header file can be used.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_host_al_eem.c#7 $
$DateTime: 2009/02/05 01:40:34 $ $Author: merez $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
05/19/08  ab      Initial version
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" 
#include "hsu_common.h"
#include "sio.h"
#include "err.h"
#include "rex.h"
#include "hsu_al_dsm_utils.h"
#include "hsu_al_dmov_api.h"
#include "uw_host_cdc_eem.h"
#include "uw_device_cdc.h"
#include "port.h"
#include "hsu_conf_sel_stack_utils.h"
#include "jerrno.h"
#include "jtypes.h"
#include "hsu_host_al_eem.h"
#include "hsu_al_task.h"

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
#define HSU_HOST_AL_EEM_MAX_ETHERNET_FRAME_LENGTH 1514
/* The maximal length of an Ethernet frame, sent by the application, including:
1500 bytes for Ethernet data + 14 bytes for Ethernet header */

#define HSU_HOST_AL_EEM_TX_BUFFER_LENGTH \
  HSU_HOST_AL_EEM_MAX_ETHERNET_FRAME_LENGTH + 6
/* The maximal length of the TX buffer holding the Ethernet frame, including:
1514 bytes for Ethernet data & header + 4 bytes CRC + 2 bytes EEM header */

#define HSU_HOST_AL_EEM_MAX_INSTANCES 1
/* Number of attached EEM devices the EEM-AL can handle */

#define HSU_HOST_AL_EEM_MAX_WAIT_FOR_FLUSH_MS 2000
/* Max time to wait for flush Tx to finish */

#define HSU_HOST_AL_EEM_SLEEP_INTERVAL_MS 100
/* Time to sleep between checks if flush Tx is already finished */

#define HSU_HOST_AL_EEM_SUSPEND_TIMEOUT_MS 6000
/* Inactivity time before enterting suspend */

#ifdef FEATURE_DSM_LARGE_ITEMS
#define HSU_HOST_AL_EEM_DSM_POOL_ID DSM_DS_LARGE_ITEM_POOL
#else
#define HSU_HOST_AL_EEM_DSM_POOL_ID DSM_DS_SMALL_ITEM_POOL
#endif /*FEATURE_DSM_LARGE_ITEMS */

/*==============================================================================
Typedefs
==============================================================================*/
/* Enum representing the current state of the EEM-AL instance */
typedef enum
{
  HSU_HOST_AL_EEM_INSTANCE_STATE_NONE,
  /* Initial value, before hsu_host_al_eem_init was called by the config 
  selector */
  HSU_HOST_AL_EEM_INSTANCE_STATE_INITIALIZED,
  /* hsu_host_al_eem_init was called and the EEM-AL has been initialized. */
  HSU_HOST_AL_EEM_INSTANCE_STATE_OPENED
  /* hsu_host_al_eem_data_open was called */
} hsu_host_al_eem_instance_states_enum;

/* Enum representing the current state of the TX/RX buffer used for the 
* Bulk-IN/OUT EP. */
typedef enum
{
  HSU_HOST_AL_EEM_BUFFER_STATE_IDLE, 
  /* Free for use */
  HSU_HOST_AL_EEM_BUFFER_STATE_PENDING,
  /* Buffer is pending in stack (for read operations) */
  HSU_HOST_AL_EEM_BUFFER_STATE_FULL,
  /* Buffer is full with data */
} hsu_host_al_eem_buffer_state_enum;

/* This struct contains the application level CB, that should be called 
from the EEM-AL */
typedef struct
{
  sio_vpu_func_ptr_type enable_disable_cb_ext;
  /* The application CB to be called when the enabled/disabled state of the 
  instance changes */
  void *                enable_disable_ext_user_data;
  /* The user data to be passed when the application CB above is invoked */
} hsu_host_al_eem_app_cb_type;

/* This structure contains information related to the buffer that is used by 
* the USB stack for data read. */
typedef struct 
{
  void* data_rx_buffer;
  /* Pointer to class driver managed buffer */
  hsu_host_al_eem_buffer_state_enum data_read_state; 
  /* State of RX buffer: Idle / Pending / Full */
  uint32  data_read_num_bytes;
  /* The number of bytes read by the stack, and written to the buffer */
} hsu_host_al_eem_data_read_buffer_info_type;

/* This structure contains information related to the buffer that is used by 
* the USB stack for data write or echo write */
typedef struct 
{
  /* For UT This buffer is defined statically. Otherwise, it is allocated in
  hsu_host_al_eem_init() */
#ifndef FEATURE_HSU_TEST
  uint8*  tx_buffer;
#else
#error code not present
#endif
  uint32  tx_buf_num_of_bytes; 
  /* The number of bytes actually used in the buffer */
  hsu_host_al_eem_buffer_state_enum tx_buf_state;
  /* State of TX buffer: Just Idle or Pending */
} hsu_host_al_eem_write_buffer_data_type;

/* This struct contains information related to the data plane of the EEM-AL 
instance */
typedef struct
{
  sio_stream_id_type                stream_id;
  /* The unique SIO identifier of the data plane */
  hsu_host_al_eem_data_read_buffer_info_type read_buf_info;
  /* Details of RX buffer that is used by the USB stack */ 
  boolean                           inbound_flow_is_enabled;
  /* Is this instance inbound flow currently enabled */
  hsu_host_al_eem_write_buffer_data_type write_buffer_info;
  /* Details of the TX buffer given to the USB stack */
  hsu_host_al_eem_write_buffer_data_type echo_buffer_info;
  /* Details of the buffer used for echo, given to the USB stack */
  dsm_watermark_type *              rx_queue_ptr;      
  /* The RX WMQ provided by the application */
  dsm_watermark_type *              tx_queue_ptr; 
  /* The TX WMQ provided by the application */
} hsu_host_al_eem_data_info_type;

/* This struct contains information of the EEM-AL instance */
typedef struct 
{
  uwh_cdc_dev_h                    handle;
  /* Handle to the attached device */
  hsu_host_al_eem_instance_states_enum state;
  /* The current state of the instance */
  boolean                         device_is_attached;
  /* Indicates whether the instance is enabled (TRUE) or disabled (FALSE) */
  hsu_host_al_eem_app_cb_type     app_callbacks;
  /* Application level CBs */
  hsu_host_al_eem_data_info_type  data_info;
  /* Information related to the data plane of the instance */
  boolean                         flush_was_issued;
  /* Indicates whether the application asked to flush TX queue */
} hsu_host_al_eem_instance_info_type;

/*==============================================================================
Variables
==============================================================================*/
/* The instance information for each connected device (currently only 1 device 
is supported */
static hsu_host_al_eem_instance_info_type 
eem_al_instance_info[HSU_HOST_AL_EEM_MAX_INSTANCES];

/* Critical section for accessing instance information */
static rex_crit_sect_type eem_al_critical_section;

/* Callbacks from EEM Class Driver */
static uwh_eem_callbacks_t eem_al_cd_cbs;

/* Number of attached EEM devices */
static uint8 eem_al_num_of_attahced_devices = 0;

/* Timer for entering suspend */
static rex_timer_type	eem_al_suspend_timer;

/* Is the EEM CD suspended? */
static boolean eem_al_is_suspended;

/*==============================================================================
Forward Declarations
==============================================================================*/

static void eem_al_rx_comp_cb
(
 uwh_cdc_app_h app_ctx, 
 jresult_t status, 
 void *buf,
 juint32_t size, 
 void *priv
);

static void eem_al_tx_comp_cb
(
 uwh_cdc_app_h app_ctx, 
 jresult_t status, 
 void *buf,
 juint32_t lengh, 
 void *priv
);

static void eem_al_echo_comp_cb
(
 uwh_cdc_app_h app_ctx, 
 jresult_t status, 
 void *buf,
 juint32_t lengh, 
 void *priv
);

static void eem_al_suspend_timer_cb(unsigned long callback_param);
static void eem_al_resume(void);

/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================

FUNCTION      EEM_INVOKE_APP_ENABLE_DISABLE_CBS

DESCRIPTION   
Invokes the application DTR callback, if it was registered. 

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void eem_invoke_app_enable_disable_cbs
(
 hsu_host_al_eem_instance_info_type* instance_info 
)
{
  HSU_ASSERT(instance_info != NULL);

  if(instance_info->app_callbacks.enable_disable_cb_ext != NULL)
  {
    HSU_MSG_MED("eem_invoke_app_enable_disable_cbs(): EEM-AL calling application callback (handle=%d)", 
      instance_info->handle, 0, 0);
    instance_info->app_callbacks.enable_disable_cb_ext(
      instance_info->app_callbacks.enable_disable_ext_user_data);
  }
  else
  {
    HSU_MSG_ERROR("eem_invoke_app_enable_disable_cbs(): Callbacks \
                  not registered", 0, 0, 0);
  }
} /* eem_invoke_app_enable_disable_cbs */

/*===========================================================================

FUNCTION eem_setup_read

DESCRIPTION
Sets up a new read operation, allowing the attached device to send BULK IN 
transfer.

DEPENDENCIES
This function is assumed to be called in stack context, so the giant mutex 
is already locked. No need to re-lock it. 

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static void eem_setup_read
(
 void* instance_info_arg
)
{
  jresult_t result = HSU_JSUCCESS;
  hsu_host_al_eem_instance_info_type* instance_info = 
    (hsu_host_al_eem_instance_info_type*)instance_info_arg;

  HSU_ASSERT(instance_info != NULL);
  HSU_ASSERT(instance_info->data_info.read_buf_info.data_read_state == 
    HSU_HOST_AL_EEM_BUFFER_STATE_IDLE);

  /* Buffer transition to PENDING state */
  instance_info->data_info.read_buf_info.data_read_state = 
    HSU_HOST_AL_EEM_BUFFER_STATE_PENDING;

  /* Send the buffer to the EEM Class Driver */
  result = uwh_eem_start_rx(instance_info->handle, 
    eem_al_rx_comp_cb, instance_info, 1);

  if(result != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "eem_setup_read(): uwh_eem_start_rx returned error %d (handle=%d)", 
      result, instance_info->handle, 
      0);
    /* Buffer transition to IDLE state */
    instance_info->data_info.read_buf_info.data_read_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;
  }
} /* eem_setup_read */

/*==============================================================================

FUNCTION      eem_clear_data

DESCRIPTION   
Clears internal data of the HSU EEM-AL. Zeros all the instance specific fields. 
Called on startup, from hsu_host_al_eem_init(). 

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void eem_clear_data(void)
{
  uint8 i;

  /* Initialize static data */
  /**************************/
  rex_init_crit_sect(&eem_al_critical_section);
  eem_al_num_of_attahced_devices = 0;

  for (i=0; i<HSU_HOST_AL_EEM_MAX_INSTANCES; i++)
  {
    /* Initialize instance data */
    /****************************/
    /* General data */
    eem_al_instance_info[i].handle = NULL;
    eem_al_instance_info[i].state = HSU_HOST_AL_EEM_INSTANCE_STATE_NONE;
    eem_al_instance_info[i].device_is_attached = FALSE;
    eem_al_instance_info[i].app_callbacks.enable_disable_cb_ext = NULL;
    eem_al_instance_info[i].app_callbacks.enable_disable_ext_user_data = NULL;
    eem_al_instance_info[i].flush_was_issued = FALSE;

    /* Data Info */
    eem_al_instance_info[i].data_info.stream_id = SIO_NO_STREAM_ID;
    eem_al_instance_info[i].data_info.inbound_flow_is_enabled = FALSE; 
    eem_al_instance_info[i].data_info.read_buf_info.data_read_num_bytes = 0;
    eem_al_instance_info[i].data_info.read_buf_info.data_read_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;
    eem_al_instance_info[i].data_info.read_buf_info.data_rx_buffer = NULL;
    eem_al_instance_info[i].data_info.write_buffer_info.tx_buf_num_of_bytes = 0;
    eem_al_instance_info[i].data_info.write_buffer_info.tx_buf_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;
    eem_al_instance_info[i].data_info.echo_buffer_info.tx_buf_num_of_bytes = 0;
    eem_al_instance_info[i].data_info.echo_buffer_info.tx_buf_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;
    eem_al_instance_info[i].data_info.rx_queue_ptr = NULL;
    eem_al_instance_info[i].data_info.tx_queue_ptr = NULL;
  }
} /* eem_clear_data */

/*==============================================================================

FUNCTION      EEM_OPEN_PTR_IS_VALID

DESCRIPTION   
Validates that certain fields of the sio_open_type parameter passed to 
hsu_host_al_eem_open() are as expected. 

DEPENDENCIES  
None.

RETURN VALUE  
TRUE/ FALSE

SIDE EFFECTS  
None.

==============================================================================*/
static boolean eem_open_ptr_is_valid
(
 const sio_open_type * open_ptr
)
{
  HSU_ASSERT(open_ptr != NULL);

  if(open_ptr->stream_id == SIO_NO_STREAM_ID)
  {
    HSU_MSG_ERROR(
      "eem_open_ptr_is_valid(): stream_id==SIO_NO_STREAM_ID",0,0,0);
    return FALSE;
  }

  if(open_ptr->tx_queue == NULL) 
  {
    HSU_MSG_ERROR(
      "eem_open_ptr_is_valid(): TX WMQ is not defined",0,0,0);
    return FALSE;
  }

  if(open_ptr->rx_queue == NULL)
  {
    HSU_MSG_ERROR(
      "eem_open_ptr_is_valid(): RX WM is not defined", 0,0,0);
    return FALSE;
  }

  if(open_ptr->rx_func_ptr != NULL)
  {
    HSU_MSG_ERROR(
      "eem_open_ptr_is_valid(): rx_func_ptr is not NULL, but not supported by HS-USB!", 
      0, 0, 0);
    return FALSE;
  }

  if(open_ptr->stream_mode != SIO_GENERIC_MODE)
  {
    HSU_MSG_ERROR(
      "eem_open_ptr_is_valid(): stream_mode!=SIO_GENERIC_MODE (%d)", 
      open_ptr->stream_mode,0,0);
    return FALSE;
  }

  if(open_ptr->rx_bitrate != SIO_BITRATE_BEST)
  {
    HSU_MSG_ERROR(
      "eem_open_ptr_is_valid(): rx_bitrate!=SIO_BITRATE_BEST (%d)", 
      open_ptr->rx_bitrate,0,0);
    return FALSE;
  }

  if(open_ptr->tx_bitrate != SIO_BITRATE_BEST)
  {
    HSU_MSG_ERROR(
      "eem_open_ptr_is_valid(): tx_bitrate!=SIO_BITRATE_BEST (%d)", 
      open_ptr->tx_bitrate,0,0);
    return FALSE;
  }

  if(open_ptr->tail_char_used != FALSE)
  {
    HSU_MSG_ERROR(
      "eem_open_ptr_is_valid(): tail_char_used!=FALSE", 0,0,0);
    return FALSE;
  }

  if(open_ptr->rx_flow != SIO_FCTL_BEST)
  {
    HSU_MSG_ERROR(
      "eem_open_ptr_is_valid(): rx_flow!=SIO_FCTL_BEST", 
      open_ptr->rx_flow,0,0);
    return FALSE;
  }

  if(open_ptr->tx_flow != SIO_FCTL_BEST)
  {
    HSU_MSG_ERROR(
      "eem_open_ptr_is_valid(): tx_flow!=SIO_FCTL_BEST", 
      open_ptr->tx_flow,0,0);
    return FALSE;
  }
  return TRUE;
} /* eem_open_ptr_is_valid */

/*==============================================================================

FUNCTION      EEM_GET_INSTANCE_INFO_FROM_STREAM_ID

DESCRIPTION   
Returns a pointer to the hsu_host_al_eem_instance_info_type that matches the 
given stream id.
Currently, since only one instance of attached device is supported, the 
function just verifies that the stream id matches the current signal data
channel stream id.

DEPENDENCIES  
None.

RETURN VALUE  
Pointer to the hsu_host_al_eem_instance_info_type that matches the given stream
id.

SIDE EFFECTS  
None.
==============================================================================*/
static hsu_host_al_eem_instance_info_type * eem_get_instance_info_from_stream_id
(
 sio_stream_id_type  stream_id
)
{
  /* Current implementation supports only 1 instance, located at index 0 */
  if(stream_id != eem_al_instance_info[0].data_info.stream_id) 
  {
    HSU_MSG_ERROR("eem_get_instance_info_from_stream_id(): unknown stream_id=%d - data channel's=%d", 
      stream_id, eem_al_instance_info[0].data_info.stream_id, 0);
    return NULL;
  }
  else
  {
    return &eem_al_instance_info[0];
  }
} /* eem_get_instance_info_from_stream_id */

/*==============================================================================

FUNCTION      EEM_GET_AVAILABLE_INSTANCE_INFO

DESCRIPTION   
Returns a pointer to an available hsu_host_al_eem_instance_info_type in 
eem_al_instance_info[]. 
Since the current implementation supports only signal attached device, simply 
return a pointer to eem_al_instance_info[0].

DEPENDENCIES  
None.

RETURN VALUE  
Pointer to available hsu_host_al_eem_instance_info_type

SIDE EFFECTS  
None.
==============================================================================*/
static hsu_host_al_eem_instance_info_type * eem_get_available_instance_info()
{
  /* Current implementation supports only 1 instance, located at index 0 */
  if (eem_al_instance_info[0].data_info.stream_id != SIO_NO_STREAM_ID)
  {
    HSU_MSG_ERROR("eem_get_available_instance_info(): no available instance", 
      0,0,0);
    return NULL;
  }
  else
  {
    return &eem_al_instance_info[0];
  }

} /* eem_get_available_instance_info */

/*==============================================================================

FUNCTION      EEM_APP_CONTEXT_IS_VALID

DESCRIPTION   
Verifies that the uwh_cdc_app_h given by CDC stack functions contains a valid 
value. 

Currently, since only one attached EEM device is supported, it does so by 
comparing the context to eem_al_instance_info[0].

DEPENDENCIES  
None.

RETURN VALUE  
TRUE if the context is valid, FALSE if not.

SIDE EFFECTS  
None.
==============================================================================*/
static boolean eem_app_context_is_valid
(
 uwh_cdc_app_h app_ctx
)
{
  /* Check that this is the single device that is handled by the EEM-AL */
  if(app_ctx != &eem_al_instance_info[0])
  {
    HSU_MSG_ERROR(
      "eem_app_context_is_valid(): context=0x%04x, &eem_al_instance_info[0] = 0x%04x", 
      app_ctx, &eem_al_instance_info[0], 0);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* eem_app_context_is_valid */


/*==============================================================================

FUNCTION      EEM_TX_DSM_CHAIN

DESCRIPTION   
Transmits a given DSM chain using the EEM class driver.
This function is called by the hsu_host_al_eem_data_transmit when the TX buffer 
is available, or by eem_al_tx_comp_cb, when previous TX has finished,
and the TX buffer has become available.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
static void eem_tx_dsm_chain
(
 dsm_item_type* tx_ptr,
 hsu_host_al_eem_instance_info_type* instance_info
)
{
  jresult_t jresult;
  void* buffer;

  HSU_ASSERT(tx_ptr != NULL);
  HSU_ASSERT(instance_info != NULL);

  /* Mark the buffer as being used */
  instance_info->data_info.write_buffer_info.tx_buf_state = 
    HSU_HOST_AL_EEM_BUFFER_STATE_FULL;

  /* First, gather DSM chain into the TX buffer */
  /**********************************************/
  instance_info->data_info.write_buffer_info.tx_buf_num_of_bytes = 
    dsm_length_packet(tx_ptr);

  /* In case buffer is too long (>1514 bytes) free the DSM chain and return */
  if (instance_info->data_info.write_buffer_info.tx_buf_num_of_bytes > 
    HSU_HOST_AL_EEM_MAX_ETHERNET_FRAME_LENGTH)
  {
    dsm_free_packet(&tx_ptr);
    instance_info->data_info.write_buffer_info.tx_buf_num_of_bytes = 0;
    instance_info->data_info.write_buffer_info.tx_buf_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR("eem_tx_dsm_chain(): dsm chain is too long=0x%x, (handle=%d)", 
      tx_ptr, instance_info->handle, 0);
    return;
  }

  /* Gather the DSM chain into the Tx buffer */
  jresult = hsu_al_dmov_api_gather_dsm_chain(
    tx_ptr, 
    instance_info->data_info.write_buffer_info.tx_buffer, 
    HSU_HOST_AL_EEM_TX_BUFFER_LENGTH, 
    NULL, NULL);

  /* In case of gather error, free the DSM chain and return */
  if(jresult == FALSE)
  {
    dsm_free_packet(&tx_ptr);
    instance_info->data_info.write_buffer_info.tx_buf_num_of_bytes = 0;
    instance_info->data_info.write_buffer_info.tx_buf_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR("eem_tx_dsm_chain(): gather failed dsm_chain_head=0x%x, (handle=%d)", 
      tx_ptr, instance_info->handle, 0);
    return;
  }

  /* Second, check that the device is attached */
  /*********************************************/
  dsm_free_packet(&tx_ptr);

  /* Here it's safe to check if attached/detached, as only the CD can modify 
  * these values - and right now it does not have the giant mutex */
  if (instance_info->device_is_attached == FALSE)
  {
    /* All enqueue and dequeue operations are protected by this critical section */
    rex_enter_crit_sect(&eem_al_critical_section);

    /* Ignore this frame, and any other data pending in the data TX WMQ */
    instance_info->data_info.write_buffer_info.tx_buf_num_of_bytes = 0;
    instance_info->data_info.write_buffer_info.tx_buf_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;
    HSU_ASSERT(instance_info->data_info.tx_queue_ptr != NULL);
    dsm_empty_queue(instance_info->data_info.tx_queue_ptr);

    rex_leave_crit_sect(&eem_al_critical_section);

    HSU_MSG_HIGH(
      "eem_tx_dsm_chain: Device is not attached (handle=%d), Ignoring.", 
      instance_info->handle, 0, 0);
    return;
  }

  /* Third, send the buffer down the stack */
  /*****************************************/

  buffer = instance_info->data_info.write_buffer_info.tx_buffer;

  (void)eem_al_resume();

  jresult = uwh_eem_send_data_packet(
    instance_info->handle, 
    buffer,
    instance_info->data_info.write_buffer_info.tx_buf_num_of_bytes, 
    0, 
    eem_al_tx_comp_cb, 
    instance_info);

  /* We don't handle ECANCELED, because it can't happen, as we haven't 
     requested to suspend */
  if (jresult != HSU_JSUCCESS)
  {
    instance_info->data_info.write_buffer_info.tx_buf_num_of_bytes = 0;
    instance_info->data_info.write_buffer_info.tx_buf_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR(
      "eem_tx_dsm_chain: uwh_eem_send_data_packet returned an error %d (handle=%d)", 
      jresult, instance_info->handle, 0);

    return;
  }

  return;
} /* eem_tx_dsm_chain */

/*==============================================================================

FUNCTION      EEM_HANDLE_READ_COMPLETE

DESCRIPTION   
Called from eem_al_rx_comp_cb (CD's context) for IN transfer 
completions.

Can also be called following inbound flow enabling (from the function
hsu_host_al_eem_ioctl, command SIO_IOCTL_INBOUND_FLOW_ENABLE).
In this case, it is called in the application's context.

Scatters the read buffer into a DSM chain, and enqueue the DSM chain into the 
app's RX WMQ.

DEPENDENCIES  
None.

RETURN VALUE    
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void eem_handle_read_complete
(
 hsu_host_al_eem_instance_info_type* instance_info
)
{
  dsm_item_type * dsm_chain_head_ptr = NULL;

  HSU_ASSERT(instance_info != NULL);
  HSU_ASSERT(instance_info->data_info.read_buf_info.data_rx_buffer != NULL);
  HSU_ASSERT(instance_info->data_info.read_buf_info.data_read_num_bytes != 0);
  HSU_ASSERT(instance_info->data_info.read_buf_info.data_read_state == 
    HSU_HOST_AL_EEM_BUFFER_STATE_FULL);

  /* Handle the case where inbound flow is disabled, by ignoring that frame. 
     We can't save the frame because the Class Driver will free the buffer */
  if (!instance_info->data_info.inbound_flow_is_enabled)
  {
    HSU_MSG_MED(
      "eem_handle_read_complete(): Received a buffer, but inbound flow is\
      disabled. Saving it for when it's enabled (handle=%d)",
      instance_info->handle,0,0);
    instance_info->data_info.read_buf_info.data_read_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_PENDING;
    return;
  }

  /* If the channel is not open, ignore the received frame */
  if(instance_info->state != HSU_HOST_AL_EEM_INSTANCE_STATE_OPENED)
  {
    HSU_MSG_MED(
      "eem_handle_read_complete(): ignoring the frame, since channel is\
      not opened (handle=%d)", instance_info->handle,0,0);

    instance_info->data_info.read_buf_info.data_read_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_PENDING;
    instance_info->data_info.read_buf_info.data_read_num_bytes = 0;
    instance_info->data_info.read_buf_info.data_rx_buffer = NULL;
    return;
  }

  /* Allocate DSM chain with the right size */
  dsm_chain_head_ptr = hsu_al_dsm_utils_generate_dsm_chain(
    HSU_HOST_AL_EEM_DSM_POOL_ID, 
    instance_info->data_info.read_buf_info.data_read_num_bytes);  

  if(dsm_chain_head_ptr == NULL)
  {
    /* Failed generating DSM chain, no DSM items */
    instance_info->data_info.read_buf_info.data_read_num_bytes = 0;
    instance_info->data_info.read_buf_info.data_rx_buffer = NULL;
    instance_info->data_info.read_buf_info.data_read_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_PENDING;

    HSU_MSG_ERROR("eem_handle_read_complete(): out of DSM items (handle=%d,\
                  size=%d)", instance_info->handle, 
                  instance_info->data_info.read_buf_info.data_read_num_bytes, 0);
    return;
  }

  /* Scatter the buffer into the new DSM chain */
  if (hsu_al_dmov_api_scatter_dsm_chain(
    (void *)(instance_info->data_info.read_buf_info.data_rx_buffer),
    instance_info->data_info.read_buf_info.data_read_num_bytes,
    dsm_chain_head_ptr,
    NULL,
    instance_info) == FALSE)
  {
    /* Failed scatter operation */
    dsm_free_packet(&dsm_chain_head_ptr);
    instance_info->data_info.read_buf_info.data_read_num_bytes = 0;
    instance_info->data_info.read_buf_info.data_rx_buffer = NULL;
    instance_info->data_info.read_buf_info.data_read_state = 
      HSU_HOST_AL_EEM_BUFFER_STATE_PENDING;

    HSU_MSG_ERROR(
      "eem_handle_read_complete(): Scatter failed. handle=%d, buffer=0x%x, \
      size=%d", instance_info->handle, 
      instance_info->data_info.read_buf_info.data_rx_buffer, 
      instance_info->data_info.read_buf_info.data_read_num_bytes);
    return;
  }

  /* Enqueue DSM chain to the application's data RX WMQ */
  HSU_ASSERT(instance_info->data_info.rx_queue_ptr != NULL);
  dsm_enqueue(instance_info->data_info.rx_queue_ptr, &dsm_chain_head_ptr);

  instance_info->data_info.read_buf_info.data_read_state = 
    HSU_HOST_AL_EEM_BUFFER_STATE_PENDING;
} /* eem_handle_read_complete */ 

/*==============================================================================

FUNCTION      EEM_ENABLE_INBOUND_FLOW

DESCRIPTION   
This function performs the required operations for enabling inbound flow.
If a buffer is waiting to be delivered to the app, deliver it. Also, setup a new 
bulk IN transfer.

Note: This function is called from the application's context,

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
static void eem_enable_inbound_flow
(
 hsu_host_al_eem_instance_info_type* instance_info
)
{
  HSU_ASSERT(instance_info != NULL);

  /* Check if the inbound flow is already enabled */
  if(instance_info->data_info.inbound_flow_is_enabled == TRUE)
  {
    HSU_MSG_LOW("eem_enable_inbound_flow(): inbound flow is already enabled", 
      0, 0, 0);
    return; 
  }      

  instance_info->data_info.inbound_flow_is_enabled = TRUE;

  /* The first time this IOCTL is used, is right after sio_open(), before 
  * enumeration is finished and attached_cb() was invoked from the stack. 
  * In this special case, we only need to set the flow to enabled. */
  if(instance_info->device_is_attached == FALSE)
  { 
    HSU_MSG_MED(
      "eem_enable_inbound_flow(): Device is not attached, just marking inbound\
      flow as enabled", 0,0,0);
    return;
  }

  jsafe_enter();
  (void)eem_al_resume();
  jsafe_leave();

  /* Check if there's a buffer waiting to be sent to the application */
  if(instance_info->data_info.read_buf_info.data_read_state == 
    HSU_HOST_AL_EEM_BUFFER_STATE_FULL)
  {
    eem_handle_read_complete(instance_info);
  }
  /* Setup a new bulk IN transfer */
  else if(instance_info->data_info.read_buf_info.data_read_state == 
    HSU_HOST_AL_EEM_BUFFER_STATE_IDLE)

  {
    hsu_al_task_enqueue_cmd(eem_setup_read, (void*)instance_info);
  }

  return;
} /* eem_enable_inbound_flow */

/*==============================================================================

FUNCTION      EEM_DISABLE_INBOUND_FLOW

DESCRIPTION   
This function performs the required operations for disabling inbound flow.

Note: It is called from the application's context.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
static void eem_disable_inbound_flow
(
 hsu_host_al_eem_instance_info_type* instance_info
)
{
  jresult_t jresult;
  HSU_ASSERT(instance_info != NULL);

  /* Check if the inbound flow is already disabled */
  if(instance_info->data_info.inbound_flow_is_enabled == FALSE)
  {
    HSU_MSG_LOW(
      "eem_disable_inbound_flow(): inbound flow is already disabled",
      0,0,0);
    return; /* nothing to do */
  }

  /* Abort the Rx */
  jsafe_enter();
  jresult = uwh_eem_stop_rx(instance_info->handle);
  jsafe_leave();

  if (jresult != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "eem_disable_inbound_flow(): Could not abort Rx (error %d)", 
      jresult,0,0);
  }

  instance_info->data_info.inbound_flow_is_enabled = FALSE;
} /* eem_disable_inbound_flow */

/*==============================================================================

FUNCTION      EEM_SEND_ECHO

DESCRIPTION   
This function sends an Echo Response EEM command. It is invoked by the 
eem_al_echo_cb callback, when receiving an Echo request.

Note: It is called from the application's context.

DEPENDENCIES  
None.

RETURN VALUE  
TRUE if Echo was sent successfully down the stack.
FALSE in any other case.

SIDE EFFECTS  
None.
==============================================================================*/
static boolean eem_send_echo
(
 hsu_host_al_eem_instance_info_type *instance_info,
 void *buffer, 
 juint16_t size
)
{
  jresult_t jresult;

  rex_enter_crit_sect(&eem_al_critical_section);

  /* If another Echo Response is already in process, return. That means that
  the EEM-AL will ignore Echo Request from the device, if the previous 
  Echo Request is still in process */
  if (instance_info->data_info.echo_buffer_info.tx_buf_state !=
    HSU_HOST_AL_EEM_BUFFER_STATE_IDLE)
  {
    rex_leave_crit_sect(&eem_al_critical_section);
    HSU_MSG_MED(
      "eem_send_echo(): Echo already in progress, ignoring", 0,0,0);
    return FALSE;
  }

  HSU_ASSERT(size <= HSU_HOST_AL_EEM_MAX_ETHERNET_FRAME_LENGTH);

  /* Update the echo_buffer data structure */
  memcpy(instance_info->data_info.echo_buffer_info.tx_buffer, buffer, 
    size);
  instance_info->data_info.echo_buffer_info.tx_buf_num_of_bytes = size;
  instance_info->data_info.echo_buffer_info.tx_buf_state = 
    HSU_HOST_AL_EEM_BUFFER_STATE_FULL;

  (void)eem_al_resume();

  rex_leave_crit_sect(&eem_al_critical_section);

  /* Send the Echo Response down the stack */
  jresult = uwh_eem_echo(instance_info->handle, 
    instance_info->data_info.echo_buffer_info.tx_buffer, 
    size, eem_al_echo_comp_cb, instance_info, TRUE/*response*/);

  HSU_MSG_LOW(
    "eem_send_echo(): finished sending echo (jresult=%d)", jresult, 0, 0);
  return (jresult == HSU_JSUCCESS) ? TRUE:FALSE;
} /* eem_send_echo */

/*===========================================================================

FUNCTION EEM_AL_COMMAND_SUSPEND

DESCRIPTION
Call the CD API to enter suspend. Run from AL TASK context because suspend
handling in USB stack can cause long delays.

DEPENDENCIES
The Giant Mutex should be locked prior to calling this function.

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void eem_al_command_suspend(void* empty_param)
{
  jresult_t jresult;

  HSU_USE_PARAM(empty_param);

  /* Don't need to lock the Giant Mutex, because AL-TASK already did that */
  /* Abort the Rx */
  jresult = uwh_eem_stop_rx(eem_al_instance_info[0].handle);
  if (jresult != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "eem_al_command_suspend(): Could not abort Rx (error %d)", 
      jresult,0,0);
  }

  /* Suspend attached device (currently support 1 device) */
  HSU_MSG_MED(
    "eem_al_command_suspend(): suspending", 
    0,0,0);
  jresult = uwh_eem_suspend(eem_al_instance_info[0].handle);
  HSU_MSG_MED(
    "eem_al_command_suspend(): suspended [jresult=%d]", 
    jresult,0,0);

  if (jresult != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "eem_al_command_suspend(): Could not enter suspend (error %d)", 
      jresult,0,0);
  }
  else
  {
    rex_enter_crit_sect(&eem_al_critical_section);
    eem_al_is_suspended = TRUE;
    rex_leave_crit_sect(&eem_al_critical_section);
  }
} /* eem_al_command_suspend */

/*==============================================================================

FUNCTION      EEM_AL_SUSPEND_TIMER_CB

DESCRIPTION   
This function is called when the suspend timer is expired. It enqueues a command
to the HSU-AL task to notify the USB stack it can enter suspend.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
static void eem_al_suspend_timer_cb(unsigned long callback_param)
{
  HSU_USE_PARAM(callback_param);
  HSU_MSG_HIGH("eem_al_suspend_timer_cb(): Suspend timer expired" ,0,0,0);

  hsu_al_task_enqueue_cmd(eem_al_command_suspend, NULL);
} /* eem_al_suspend_timer_cb */

/*===========================================================================

FUNCTION EEM_AL_COMMAND_RESUME

DESCRIPTION
Call the CD API to resume. Run from client context because we assume that
after resume we can call the EEM CD RX/TX APIs immediately, and we don't
want these calls to fail.

DEPENDENCIES
The Giant Mutex should be locked prior to calling this function.

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void eem_al_command_resume(void* empty_param)
{
  jresult_t jresult;

  HSU_USE_PARAM(empty_param);

  /* Resume attached device (currently support 1 device) */
  HSU_MSG_MED(
    "eem_al_command_resume(): resuming", 0,0,0);
  jresult = uwh_eem_resume(eem_al_instance_info[0].handle);
  HSU_MSG_MED(
    "eem_al_command_resume(): resumed [jresult=%d]", jresult,0,0);

  if (jresult != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "eem_al_command_resume(): Could not resume (error %d)", jresult,0,0);
    return;
  }

  rex_enter_crit_sect(&eem_al_critical_section);
  eem_al_is_suspended = FALSE;
  rex_leave_crit_sect(&eem_al_critical_section);

  if(eem_al_instance_info[0].data_info.inbound_flow_is_enabled == FALSE)
  {
    HSU_MSG_MED(
      "eem_al_command_resume(): not initaiting another read, since inbound flow\
      is disabled (handle=%d)", eem_al_instance_info[0].handle,0,0);
    return;
  }
  if(eem_al_instance_info[0].device_is_attached == FALSE)
  {
    HSU_MSG_MED(
      "eem_al_command_resume(): not initaiting another read, since device is\
      not attached (handle=%d)", eem_al_instance_info[0].handle,0,0);
    return;
  }

  /* Restart the polling of the bulk IN endpoint, only when there is no pending
     transfer already */
  if(eem_al_instance_info[0].data_info.read_buf_info.data_read_state == 
    HSU_HOST_AL_EEM_BUFFER_STATE_IDLE)
  {
    hsu_al_task_enqueue_cmd(eem_setup_read, (void*)&eem_al_instance_info[0]);
  }

} /* eem_al_command_resume */

/*==============================================================================

FUNCTION      EEM_AL_RESUME

DESCRIPTION   
This function is called when there's USB activity, so in case the EEM interface is
in suspend state, it resumes the interface.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
static void eem_al_resume(void)
{
  rex_enter_crit_sect(&eem_al_critical_section);

  /* We can't use the AL task, becasue after resuming we assume we can use */
  /* the EEM interface for RX/TX immediately. */
  if (eem_al_is_suspended)
  {
    rex_leave_crit_sect(&eem_al_critical_section);
    eem_al_command_resume(NULL);
  }
  else
  {  
    /* Re-arm the suspend timer */
    (void)rex_set_timer(&eem_al_suspend_timer, 
      HSU_HOST_AL_EEM_SUSPEND_TIMEOUT_MS);
    rex_leave_crit_sect(&eem_al_critical_section);
  }
} /* eem_al_resume */

/*==============================================================================
SIO API FUNCTIONS
===============================================================================*/

/*==============================================================================

FUNCTION      HSU_HOST_AL_EEM_DATA_OPEN

DESCRIPTION   
Called by the application to allocate and configure the data plane for the 
EEM.

DEPENDENCIES  
hsu_host_al_eem_init() must have been called.

RETURN VALUE  
SIO_DONE_S: SIO stream as specified in open_ptr has been successfully opened.
SIO_BADP_S:  Bad Parameter

SIDE EFFECTS  
None.

==============================================================================*/
sio_status_type  hsu_host_al_eem_data_open
(
 sio_open_type * open_ptr 
)
{
  hsu_host_al_eem_instance_info_type* instance_info;

  HSU_MSG_HIGH("hsu_host_al_eem_data_open(): Enter", 0,0,0);
  HSU_ASSERT(open_ptr != NULL);

  /* Verify parameters inside open_ptr */
  if(eem_open_ptr_is_valid(open_ptr) == FALSE)
  {
    HSU_MSG_ERROR("hsu_host_al_eem_data_open(): invalid arguments",0,0,0);
    return SIO_BADP_S;
  }

  /* Verify that there is free instance available */
  instance_info = eem_get_available_instance_info();
  if (instance_info == NULL)
  {
    HSU_MSG_ERROR("hsu_host_al_eem_data_open(): no available instance for\
                  stream %d", open_ptr->stream_id,0,0);
    return SIO_BADP_S;
  }

  /* Progress to OPENED state */
  if(instance_info->state == HSU_HOST_AL_EEM_INSTANCE_STATE_INITIALIZED)
  {
    instance_info->state = HSU_HOST_AL_EEM_INSTANCE_STATE_OPENED;
  }
  else
  {
    HSU_MSG_ERROR("hsu_host_al_eem_data_open(): state = %d, expected\
                  INITIALIZED", instance_info->state,0,0);
    return SIO_BADP_S;
  }

  /* Save the app's passed parameters in EEM-AL data structure */ 
  instance_info->data_info.stream_id = open_ptr->stream_id;
  instance_info->data_info.rx_queue_ptr = open_ptr->rx_queue;
  instance_info->data_info.tx_queue_ptr = open_ptr->tx_queue;

  return SIO_DONE_S;
} /* hsu_host_al_eem_data_open */

/*==============================================================================

FUNCTION          HSU_AL_HOST_EEM_DATA_CLOSE

DESCRIPTION
This function closes the EEM data channel. All pending TX/RX will be canceled.
No TX flush is performed, so the function will return after the channel has 
been successfully closed, without any callback mechanism.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
The RX and TX WMQs are emptied and pending TX/RX are canceled.

==============================================================================*/
void hsu_host_al_eem_data_close
( 
 sio_stream_id_type stream_id,
#ifdef FEATURE_HSUSB_WRAPPER_FREE
 sio_port_id_type   port_id,     /* Not used */   
#endif /* FEATURE_HSUSB_WRAPPER_FREE */
 void              (*close_func_ptr)(void)  /* Not used */   
)
{
  hsu_host_al_eem_instance_info_type *instance_info;

  HSU_USE_PARAM(close_func_ptr);
#ifdef FEATURE_HSUSB_WRAPPER_FREE
  HSU_USE_PARAM(port_id);
#endif /* FEATURE_HSUSB_WRAPPER_FREE */
  /* Get the instance related to this stream_id */
  instance_info = eem_get_instance_info_from_stream_id(stream_id);
  if (instance_info == NULL)
  {
    HSU_MSG_ERROR("hsu_host_al_eem_data_close(): Unknown stream_id=%d", 
      stream_id, 0, 0);
    return;
  }

  /* Move to CLOSED state */
  if(instance_info->state == HSU_HOST_AL_EEM_INSTANCE_STATE_OPENED)
  {
    instance_info->state = HSU_HOST_AL_EEM_INSTANCE_STATE_INITIALIZED;
    instance_info->flush_was_issued = FALSE;
  }
  else
  {
    HSU_MSG_ERROR("hsu_host_al_eem_data_close(): state = %d, expected\
                  OPENED", instance_info->state,0,0);
    return;
  }

  rex_enter_crit_sect(&eem_al_critical_section);

  HSU_ASSERT(instance_info->data_info.tx_queue_ptr != NULL);
  HSU_ASSERT(instance_info->data_info.rx_queue_ptr != NULL);

  instance_info->data_info.inbound_flow_is_enabled = FALSE;
  instance_info->data_info.stream_id = SIO_NO_STREAM_ID;

  /* Empty DSM Tx and Rx WMQs */
  dsm_empty_queue(instance_info->data_info.tx_queue_ptr);
  dsm_empty_queue(instance_info->data_info.rx_queue_ptr);
  instance_info->data_info.rx_queue_ptr = NULL;
  instance_info->data_info.tx_queue_ptr = NULL;

  /* If the device related to this instance is attached, abort RX/TX */
  if (instance_info->device_is_attached)
  {
    jsafe_enter();
    uwh_eem_stop_rx(instance_info->handle);
    uwh_eem_stop_rx(instance_info->handle);
    jsafe_leave();
  }

  rex_leave_crit_sect(&eem_al_critical_section);
} /* hsu_host_al_eem_data_close */

/*==============================================================================

FUNCTION                  HSU_HOST_AL_EEM_DATA_TRANSMIT 

DESCRIPTION
Called by the application, when it wishes to send an Ethernet frame on the data
channel. The data is given as a chain of DSM items. 

The function gathers the DSM chain into a preallocated buffer and then gives it 
to the USB stack. 

DEPENDENCIES
hsu_host_al_eem_init() must have been called.

RETURN VALUE
None

SIDE EFFECTS
None.

==============================================================================*/
void hsu_host_al_eem_data_transmit
( 
 sio_stream_id_type  stream_id,
 sio_port_id_type    port_id,
 dsm_item_type *     tx_ptr 
)
{
  hsu_host_al_eem_instance_info_type *instance_info;

  HSU_USE_PARAM(port_id);
  HSU_ASSERT(tx_ptr != NULL); 

  /* Get the instance related to this stream_id */
  instance_info = eem_get_instance_info_from_stream_id(stream_id);
  if (instance_info == NULL)
  {
    HSU_MSG_ERROR("hsu_host_al_eem_data_transmit(): Unknown stream_id=%d", 
      stream_id, 0, 0);
    return;
  }

  /* Validate that the state of the instance is OPENED */
  if(instance_info->state != HSU_HOST_AL_EEM_INSTANCE_STATE_OPENED)
  {
    dsm_free_packet(&tx_ptr);

    HSU_MSG_ERROR(
      "hsu_host_al_eem_data_transmit(): Instance state=%d (expected opened)", 
      instance_info->state, 0, 0);
    HSU_ASSERT(
      (void*)"hsu_host_al_eem_data_transmit(): Instance is not opened."
      == NULL);
    return;
  }

  /* If no device is attached, skip the transmission */
  if(instance_info->device_is_attached == FALSE)
  {
    HSU_MSG_MED(
      "hsu_host_al_eem_data_transmit: No device is attached (stream_id=%d).\
      Discarding frame 0x%x.", stream_id, tx_ptr, 0);
    dsm_free_packet(&tx_ptr);
    return;
  }

  /* If there's already a data transmit in progress, enqueue the message to 
  * the data TX WMQ.
  * Critical section needed: USB Stack context may call dsm_dequeue in the 
  * middle of the next block, resulting in a DSM chain left in TX WMQ */

  rex_enter_crit_sect(&eem_al_critical_section);

  if(instance_info->data_info.write_buffer_info.tx_buf_state != 
    HSU_HOST_AL_EEM_BUFFER_STATE_IDLE)
  {
    HSU_MSG_LOW("hsu_host_al_eem_data_transmit(): data TX already in\
                progress, enqueuing DSM chain to WMQ", 0,0,0);
    HSU_ASSERT(instance_info->data_info.tx_queue_ptr != NULL);
    dsm_enqueue(instance_info->data_info.tx_queue_ptr, &tx_ptr);
    rex_leave_crit_sect(&eem_al_critical_section);
    return;
  }

  rex_leave_crit_sect(&eem_al_critical_section);

  /* No TX in progress, so transmit the DSM chain */
  jsafe_enter();
  eem_tx_dsm_chain(tx_ptr, instance_info);
  jsafe_leave();

} /* hsu_host_al_eem_data_transmit */

/*==============================================================================

FUNCTION      HSU_HOST_AL_EEM_IOCTL

DESCRIPTION   
This function is used to control various features of the adaptation layer.  
The behavior of this function depends on the cmd & parameters issued.
Supported IOCTLs:
- SIO_IOCTL_ENABLE_DTR_EVENT_EXT
- SIO_IOCTL_DISABLE_DTR_EVENT_EXT
- SIO_IOCTL_DTE_READY_ASSERTED
- SIO_IOCTL_INBOUND_FLOW_ENABLE
- SIO_IOCTL_INBOUND_FLOW_DISABLE

DEPENDENCIES  
hsu_host_al_eem_init() must have been called.
hsu_host_al_eem_data_open() must have been called.
When the command is SIO_IOCTL_ENABLE_DTR_EVENT - 'param' cannot be NULL.

RETURN VALUE  
None.

SIDE EFFECTS  
Depends on the command issued.
==============================================================================*/
void hsu_host_al_eem_ioctl
(
 sio_stream_id_type      stream_id,
 sio_port_id_type        port_id,
 sio_ioctl_cmd_type      cmd,
 sio_ioctl_param_type *  param
)
{
  hsu_host_al_eem_instance_info_type *instance_info;

  HSU_USE_PARAM(port_id);
  HSU_PARAM_COULD_BE_CONST(param);

  /* Get the instance related to this stream_id */
  instance_info = eem_get_instance_info_from_stream_id(stream_id);
  if (instance_info == NULL)
  {
    HSU_MSG_ERROR("hsu_host_al_eem_data_transmit(): Unknown stream_id=%d", 
      stream_id, 0, 0);
    return;
  }

  /* Can only handle this request if hsu_host_al_eem_data_open() was called */
  if(instance_info->state != HSU_HOST_AL_EEM_INSTANCE_STATE_OPENED)
  {
    HSU_MSG_ERROR(
      "hsu_host_al_eem_ioctl(): Called while instance's state=%d, not OPENED\
      (handle=%d)",
      instance_info->state , instance_info->handle, 0);
    return;
  }

  switch(cmd)
  {
  case SIO_IOCTL_ENABLE_DTR_EVENT_EXT:
    {
      /* Register callback which informs the caller of change in the state  
      * of of the device ("DTR")*/
      HSU_MSG_MED("hsu_host_al_eem_ioctl(): got SIO_IOCTL_ENABLE_DTR_EVENT_EXT\
                  (stream_id=%d)", stream_id,0,0);
      HSU_ASSERT(param != NULL);
      instance_info->app_callbacks.enable_disable_cb_ext = 
        (*(sio_ioctl_enable_cb_event_ext_type *)param).cb_func;
      instance_info->app_callbacks.enable_disable_ext_user_data = 
        (*(sio_ioctl_enable_cb_event_ext_type *)param).cb_data;
      eem_invoke_app_enable_disable_cbs(instance_info);
      break;
    }

  case SIO_IOCTL_DISABLE_DTR_EVENT_EXT:
    {
      /* Unregister the application callback */
      HSU_MSG_MED("hsu_host_al_eem_ioctl(): got SIO_IOCTL_DISABLE_DTR_EVENT_EXT\
                  (handle=%d)", instance_info->handle,0,0);
      instance_info->app_callbacks.enable_disable_cb_ext = NULL;
      instance_info->app_callbacks.enable_disable_ext_user_data = NULL;
      break;
    }
  case SIO_IOCTL_DTE_READY_ASSERTED:
    {
      /* Return the current state of the device ("DTR") */
      HSU_MSG_MED("hsu_host_al_eem_ioctl(): got SIO_IOCTL_DTE_READY_ASSERTED,\
                  returning %d", instance_info->device_is_attached, 0, 0);
      HSU_ASSERT(param != NULL);
      *(((sio_ioctl_param_type *)param)->dte_ready_asserted) = 
        instance_info->device_is_attached;
      break;
    }
  case SIO_IOCTL_INBOUND_FLOW_ENABLE:
    {
      /* Allow IN transfers to arrive and be queued to the application's
      * data RX WMQ.
      * Called by the application in the following cases:
      * - After hsu_host_al_eem_open() has been called
      * - When the data RX WMQ low WM threshold was reached */
      HSU_MSG_MED("hsu_host_al_eem_ioctl(): got SIO_IOCTL_INBOUND_FLOW_ENABLE\
                  (stream_id=%d)", stream_id,0,0);
      eem_enable_inbound_flow(instance_info);
      break;
    }
  case SIO_IOCTL_INBOUND_FLOW_DISABLE:
    {
      /* Called when the data RX WMQ high WM threshold is reached.
      * Should prevent additional OUT transfers, until hsu_host_al_eem_ioctl() 
      * is called with the command SIO_IOCTL_INBOUND_FLOW_ENABLE. */
      HSU_MSG_MED("hsu_host_al_eem_ioctl(): got SIO_IOCTL_INBOUND_FLOW_DISABLE\
                  (handle=%d)", instance_info->handle,0,0);
      eem_disable_inbound_flow(instance_info);
      break;
    }
  case SIO_IOCTL_SET_FLOW_CTL:
    {
      /* Supporting only SIO_FCTL_BEST flow control */
      HSU_MSG_MED("hsu_host_al_eem_ioctl(): got SIO_IOCTL_SET_FLOW_CTL",0,0,0);
      HSU_ASSERT(param != NULL);
      if(((sio_ioctl_param_type *)param)->flow_ctl.rx_flow != SIO_FCTL_BEST)
      {
        HSU_MSG_ERROR("hsu_host_al_eem_ioctl(): SIO_IOCTL_SET_FLOW_CTL with\
                      rx_flow=%d, when only SIO_FCTL_BEST is supported",
                      ((sio_ioctl_param_type *)param)->flow_ctl.rx_flow,0,0);
        HSU_ASSERT((void*)"hsu_host_al_eem_ioctl(): SIO_IOCTL_SET_FLOW_CTL\
                          with rx_flow !=SIO_FCTL_BEST" == NULL);
      }
      if(((sio_ioctl_param_type *)param)->flow_ctl.tx_flow != SIO_FCTL_BEST)
      {
        HSU_MSG_ERROR("hsu_host_al_eem_ioctl(): SIO_IOCTL_SET_FLOW_CTL with\
                      tx_flow=%d, when only SIO_FCTL_BEST is supported", 
                      ((sio_ioctl_param_type *)param)->flow_ctl.tx_flow,0,0);
        HSU_ASSERT((void*)"hsu_host_al_eem_ioctl(): SIO_IOCTL_SET_FLOW_CTL\
                          with tx_flow != SIO_FCTL_BEST" == NULL);
      }
      break;
    }
  case SIO_IOCTL_GET_FLOW_CTL:
    {
      /* Supporting only SIO_FCTL_BEST flow control */
      HSU_MSG_MED("hsu_host_al_eem_ioctl(): got SIO_IOCTL_GET_FLOW_CTL",0,0,0);
      HSU_ASSERT(param != NULL);
      ((sio_ioctl_param_type *)param)->flow_ctl.tx_flow = SIO_FCTL_BEST;
      ((sio_ioctl_param_type *)param)->flow_ctl.rx_flow = SIO_FCTL_BEST;
      break;
    }
  default:
    {
      /* Unknown command */
      HSU_MSG_ERROR("hsu_host_al_eem_ioctl(): cmd %d not supported!",cmd,0,0);
      HSU_ASSERT((void*)"hsu_host_al_eem_ioctl(): unsupported IOCTL command" 
        == NULL);
    }
  } /* switch(cmd) */
} /* hsu_host_al_eem_ioctl */

/*==============================================================================

FUNCTION                HSU_HOST_AL_EEM_DATA_FLUSH_TX 

DESCRIPTION
This function flushes the pending DSM chains from the TX WMQ. The function 
will return synchronously, when the flush operation has finished.
The EEM-AL will try to flash the Tx WMQ and pending Tx buffer, but if not 
finishes within a predefined timed, it will simply cancel the pending Tx 
buffer, and return. 

DEPENDENCIES
hsu_host_al_eem_init() must have been called.

RETURN VALUE
None.

SIDE EFFECTS
The TX WMQ is empty and no pending TX/RX.

==============================================================================*/
void hsu_host_al_eem_data_flush_tx
( 
 sio_stream_id_type  stream_id,             
 sio_port_id_type    port_id,
 void                (*flush_func_ptr)(void)  
)
{
  hsu_host_al_eem_instance_info_type *instance_info;
  uint32 remaining_msecs_to_wait_for_close = 
    HSU_HOST_AL_EEM_MAX_WAIT_FOR_FLUSH_MS;

  HSU_USE_PARAM(flush_func_ptr);
  HSU_USE_PARAM(port_id);

  /* Get the instance related to this stream_id */
  instance_info = eem_get_instance_info_from_stream_id(stream_id);
  if (instance_info == NULL)
  {
    HSU_MSG_ERROR("hsu_host_al_eem_data_flush_tx(): Unknown stream_id=%d", 
      stream_id, 0, 0);
    return;
  }

  rex_enter_crit_sect(&eem_al_critical_section);

  /* Return here if flush was already called */
  if (instance_info->flush_was_issued)
  {
    HSU_MSG_ERROR("hsu_host_al_eem_data_flush_tx(): already issued for\
                  stream_id=%d", stream_id, 0, 0);
    rex_leave_crit_sect(&eem_al_critical_section);
    return;
  }

  /* If the Tx WMQ is empty and no Tx buffer is pending, return now */
  if (dsm_is_wm_empty(instance_info->data_info.tx_queue_ptr) &&
    instance_info->data_info.write_buffer_info.tx_buf_state ==
    HSU_HOST_AL_EEM_BUFFER_STATE_IDLE)
  {
    HSU_MSG_LOW("hsu_host_al_eem_data_flush_tx: flush returns immediately ", 
      0, 0, 0); 
    rex_leave_crit_sect(&eem_al_critical_section);
    return;
  }

  instance_info->flush_was_issued = TRUE;

  /* Wait until the flush TX finishes, and if not finished within 
     HSU_HOST_AL_EEM_MAX_WAIT_FOR_FLUSH_MS msecs, give up & return */
  while (remaining_msecs_to_wait_for_close > 0)
  {
    /* If flush was complete, then return */
    if (instance_info->flush_was_issued == FALSE)
    {
      break;
    }
    /* else: sleep for a certain interval and retry */

    rex_leave_crit_sect(&eem_al_critical_section);

    HSU_MSG_LOW("hsu_host_al_eem_data_flush_tx: rex_sleep for %u msecs.\
                Remaining: %u msecs", HSU_HOST_AL_EEM_SLEEP_INTERVAL_MS, 
                remaining_msecs_to_wait_for_close, 0);

    rex_sleep(HSU_HOST_AL_EEM_SLEEP_INTERVAL_MS);

    remaining_msecs_to_wait_for_close -= 
      HSU_HOST_AL_EEM_SLEEP_INTERVAL_MS;

    rex_enter_crit_sect(&eem_al_critical_section);
  }

  /* If the flush was not finished successfully, abort current TX before 
     returning */
  if (remaining_msecs_to_wait_for_close == 0)
  {
    HSU_MSG_HIGH("hsu_host_al_eem_data_flush_tx: flush was not finished on\
                 time, aborting Tx", 0, 0, 0); 
    jsafe_enter();
    uwh_eem_abort_tx(instance_info->handle);
    jsafe_leave();
  }
  else
  {
    HSU_MSG_LOW("hsu_host_al_eem_data_flush_tx: flush succeeded", 0, 0, 0); 
  }

  rex_leave_crit_sect(&eem_al_critical_section);
} /* hsu_host_al_eem_data_flush_tx */

/*==============================================================================
EEM CLASS DRIVER CALLBACKS
===============================================================================*/

/*==============================================================================

FUNCTION      EEM_AL_ATTACH_CB

DESCRIPTION   
This callback is called by the Class Driver when a EEM device is enumerated.
The function registers the device in the EEM-AL data structure.
Currently only single device is supported, so in case another device attaches,
simply ignore it.

DEPENDENCIES  
None.

RETURN VALUE  
HSU_JSUCCESS if the device enabled. Otherwise the proper error code.

SIDE EFFECTS  
None. 

==============================================================================*/
static jresult_t eem_al_attach_cb
(
 uwh_cdc_dev_h dev_ctx, 
 uwh_cdc_app_h *app_ctx
)
{
  jresult_t jresult;

  *app_ctx = NULL;

  /* Currently supporting only 1 device. So check if a device has previously 
  * attached to the host. If yes, save its dev_ctx in instace_info[0], 
  * otherwise ignore it */

  if (eem_al_num_of_attahced_devices > 0)
  {
    /* This case can happen when connecting more than 1 EEM device */
    HSU_MSG_HIGH(
      "eem_al_attach_cb(): device (handle=%d) attached, when some\
      device is already attached",
      dev_ctx, 0, 0);
    return HSU_JSUCCESS;
  }

  /* Check if it is the device that is already attached */
  if (eem_al_instance_info[0].handle == dev_ctx &&
    eem_al_instance_info[0].device_is_attached)
  {
    /* Device shouldn't be reported as attached when already attached */
    HSU_MSG_ERROR(
      "eem_al_attach_cb(): device (handle=%d) attached, but already\
      attached", dev_ctx, 0, 0);
    HSU_ASSERT((char*)"eem_al_attach_cb(): device attached but\
                      already attached" == NULL);
    return HSU_JSUCCESS;
  }

  *app_ctx = (uwh_cdc_app_h)&eem_al_instance_info[0];
  eem_al_instance_info[0].handle = dev_ctx;
  eem_al_instance_info[0].device_is_attached = TRUE;
  eem_al_num_of_attahced_devices++;

  /* Enable data RX/TX. */
  jresult = uwh_eem_enable(dev_ctx);
  if (jresult != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "eem_al_attach_cb(): device (handle=%d) attached, but can't be\
      enabled", dev_ctx, 0, 0);
    return jresult;
  }

  rex_enter_crit_sect(&eem_al_critical_section);

  (void)eem_al_resume();

  rex_leave_crit_sect(&eem_al_critical_section);

  /* Call the application DTR callback */
  eem_invoke_app_enable_disable_cbs(&eem_al_instance_info[0]);

  return jresult;
} /* eem_al_attach_cb */

/*==============================================================================

FUNCTION      EEM_AL_DETACH_CB

This callback is called by the Class Driver when a EEM device is detached.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None. 

==============================================================================*/
static void eem_al_detach_cb
(
 uwh_cdc_app_h app_ctx
)
{
  jresult_t jresult;
  hsu_host_al_eem_instance_info_type *instance_info;

  if (!eem_app_context_is_valid(app_ctx))
  {
    /* This could actually happen if connecting multiple devices, as we passed 
       NULL for app_ctx in the attach cb. */
    HSU_MSG_HIGH(
      "eem_al_detach_cb(): wrong application context",
      0, 0, 0);
    return;
  }

  instance_info = (hsu_host_al_eem_instance_info_type*)(app_ctx);

  if (!instance_info->device_is_attached)
  {
    /* Device shouldn't be reported as detached when already not attached */
    HSU_MSG_ERROR(
      "eem_al_detach_cb(): device (handle=%d) detached, but\
      already not attached", instance_info->handle, 0, 0);
    return;
  }

  rex_enter_crit_sect(&eem_al_critical_section);
  eem_al_num_of_attahced_devices--;
  instance_info->device_is_attached = FALSE;

  /* If the channel is open, there's no need to maintain the TX WMQ, because
     it will be aborted anyway by the Class Driver */
  if (instance_info->state == HSU_HOST_AL_EEM_INSTANCE_STATE_OPENED)
  {
    HSU_ASSERT(instance_info->data_info.tx_queue_ptr != NULL);
    dsm_empty_queue(instance_info->data_info.tx_queue_ptr);
  }

  (void)rex_clr_timer(&eem_al_suspend_timer);
  eem_al_is_suspended = FALSE;

  rex_leave_crit_sect(&eem_al_critical_section);

  /* Call the application DTR callback */
  eem_invoke_app_enable_disable_cbs(&eem_al_instance_info[0]);

  /* Disable data RX/TX */

  rex_enter_crit_sect(&eem_al_critical_section);

  if (!eem_al_is_suspended)
  {
    jresult = uwh_eem_disable(instance_info->handle);
    if (jresult != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR("eem_al_detach_cb(): device (handle=%d) detached,\
                    but can't be disabled", instance_info->handle, 0, 0);
    }
  }

  rex_leave_crit_sect(&eem_al_critical_section);
} /* eem_al_detach_cb */

/*==============================================================================

FUNCTION      EEM_AL_ECHO_CB

DESCRIPTION   
This callback is called by the Class Driver when an Echo Request or Echo 
Response is received from the device.
The function handles Echo Request by sending an Echo Response command. It 
ignores Echo Response.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None. 

==============================================================================*/
static void eem_al_echo_cb
(
 uwh_cdc_app_h app_ctx, 
 void *buffer, 
 juint16_t size, 
 jbool_t is_echo_response
)
{
  hsu_host_al_eem_instance_info_type *instance_info;

  if (!eem_app_context_is_valid(app_ctx))
  {
    /* This could actually happen if connecting multiple devices */
    HSU_MSG_HIGH("eem_al_echo_cb(): wrong application context",
      0, 0, 0);
    return;
  }

  instance_info = (hsu_host_al_eem_instance_info_type*)(app_ctx);

  HSU_MSG_MED("eem_al_echo_cb(): received echo from USIM\
              (handle=%d)",instance_info->handle, 0, 0);

  if (size > HSU_HOST_AL_EEM_MAX_ETHERNET_FRAME_LENGTH)
  {
    HSU_MSG_HIGH("eem_al_echo_cb(): echo size is too long (size=%d)",
      size, 0, 0);
    return;
  }

  if (is_echo_response == FALSE) /* Echo request */
  {
    if (eem_send_echo(instance_info, buffer, size) == FALSE)
    {
      /* This could actually happen if device is sending echo before the
         previous echo reply was ackd by the stack */
      HSU_MSG_HIGH("eem_al_echo_cb(): couldn't reply to echo",
        0, 0, 0);
      return;
    }
  }
  else if (is_echo_response == TRUE) /* Echo Response */
  {
    HSU_MSG_HIGH("eem_al_echo_cb(): received echo reply",
      0, 0, 0);
  }
} /* eem_al_echo_cb */

/*==============================================================================

FUNCTION      EEM_AL_TX_COMP_CB

DESCRIPTION   
This callback is called by the Class Driver when a previous TX request has been
forwarded to the HW. 
This function sends the next DSM chain from the TX WMQ, if available. 

DEPENDENCIES  
A previous call to uwh_eem_send_data_packet().

RETURN VALUE  
None.

SIDE EFFECTS  
None. 

==============================================================================*/
static void eem_al_tx_comp_cb
(
 uwh_cdc_app_h app_ctx, 
 jresult_t status, 
 void *buf,
 juint32_t length, 
 void *priv
)
{
  dsm_item_type * dsm_chain_ptr = NULL;
  hsu_host_al_eem_instance_info_type *instance_info;

  if (!eem_app_context_is_valid(app_ctx))
  {
    HSU_MSG_ERROR("eem_al_tx_comp_cb(): wrong application context",
      0, 0, 0);
    HSU_ASSERT((void*)"eem_al_tx_comp_cb() failed because app_ctx\
                      is not valid" == NULL);
    return;
  }

  instance_info = (hsu_host_al_eem_instance_info_type*)(app_ctx);

  /* Must be the same buffer we passed to uwh_eem_send_data_packet */
  HSU_ASSERT(buf == 
    instance_info->data_info.write_buffer_info.tx_buffer);
  HSU_ASSERT(instance_info->data_info.write_buffer_info.tx_buf_state ==
    HSU_HOST_AL_EEM_BUFFER_STATE_FULL);
  HSU_PARAM_COULD_BE_CONST(buf);

  switch(status)
  {
  case ECANCEL:
    {
      /* This can happen in case the device has been detached, or the 
         application has closed the channel. Ignore the frame. */
      instance_info->data_info.write_buffer_info.tx_buf_num_of_bytes = 0;
      instance_info->data_info.write_buffer_info.tx_buf_state =
        HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;

      HSU_MSG_HIGH(
        "eem_al_tx_comp_cb(): status=CANCEL, buf=0x%x (handle=%d)",
        buf ,instance_info->handle, 0);
      break;
    }
  case CDC_TRANSFER_OK:
    {
      break; /* handling code is after the end of the switch statement */
    }
  default:
    {
      /* This shouldn't happen in normal operation, but if it does, ignore
         the frame. */
      instance_info->data_info.write_buffer_info.tx_buf_num_of_bytes = 0;
      instance_info->data_info.write_buffer_info.tx_buf_state =
        HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;

      HSU_MSG_ERROR(
        "eem_al_tx_comp_cb(): unknown status=%d, buf=0x%x, handle=%d",
        status, buf,instance_info->handle);
      break;
    }
  }

  /* We need a critical section around the enqueue/dequeue of the WMQ, in order
  * to prevent this code to preempt the CPU while running 
  * hsu_host_al_eem_data_transmit, resulting in a DSM chain left in TX WMQ */ 

  rex_enter_crit_sect(&eem_al_critical_section);

  if (instance_info->state != HSU_HOST_AL_EEM_INSTANCE_STATE_OPENED)
  {
    instance_info->data_info.write_buffer_info.tx_buf_num_of_bytes = 0;
    instance_info->data_info.write_buffer_info.tx_buf_state =
      HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;
    rex_leave_crit_sect(&eem_al_critical_section);
    return;
  }

  /* Take the next DSM chain out of the TX WMQ */
  HSU_ASSERT(instance_info->data_info.tx_queue_ptr != NULL);
  dsm_chain_ptr = dsm_dequeue(instance_info->data_info.tx_queue_ptr);

  /* If there's not pending DSM chain, return now */
  if(dsm_chain_ptr == NULL)
  {
    instance_info->data_info.write_buffer_info.tx_buf_num_of_bytes = 0;
    instance_info->data_info.write_buffer_info.tx_buf_state =
      HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;

    /* If flush was issued, signal hsu_host_al_eem_data_flush_tx that flush 
       was finished (as there's no DSM chains left in the TX WMQ) */
    if (instance_info->flush_was_issued)
    {
      instance_info->flush_was_issued = FALSE;
    }

    rex_leave_crit_sect(&eem_al_critical_section);
    return;
  }

  /* Initiate the next transmission of DSM chain */
  rex_leave_crit_sect(&eem_al_critical_section);

  eem_tx_dsm_chain(dsm_chain_ptr, instance_info);
} /* eem_al_tx_comp_cb */

/*==============================================================================

FUNCTION      EEM_AL_RX_COMP_CB

DESCRIPTION   
This callback is called by the Class Driver when a previous RX request has been
forwarded from the HW. This function calls eem_handle_read_complete to handle
the new received buffer.

DEPENDENCIES  
A previous call to uwh_eem_start_rx().

RETURN VALUE  
None.

SIDE EFFECTS  
None. 

==============================================================================*/
static void eem_al_rx_comp_cb
(
 uwh_cdc_app_h app_ctx, 
 jresult_t status, 
 void *buf,
 juint32_t size, 
 void *priv
)
{
  hsu_host_al_eem_instance_info_type *instance_info;

  if (!eem_app_context_is_valid(app_ctx))
  {
    HSU_MSG_ERROR("eem_al_rx_comp_cb(): wrong application context",
      0, 0, 0);
    HSU_ASSERT((void*)"eem_al_rx_comp_cb() failed because app_ctx\
                      is not valid" == NULL);
    return;
  }

  instance_info = (hsu_host_al_eem_instance_info_type*)(app_ctx);

  HSU_PARAM_COULD_BE_CONST(buf);

  HSU_ASSERT(instance_info->data_info.read_buf_info.data_read_state
    == HSU_HOST_AL_EEM_BUFFER_STATE_PENDING);

  /* Handle according to the status given in the CB */
  /**************************************************/
  switch(status) 
  {
  case ECANCEL:
    {
      /* This can happen in case the device has been detached, or the 
      * application has closed the channel. In any case, ignore the frame
      * and don't issue another read. */
      instance_info->data_info.read_buf_info.data_read_state
        = HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;
      instance_info->data_info.read_buf_info.data_read_num_bytes = 0;
      instance_info->data_info.read_buf_info.data_rx_buffer = NULL;

      HSU_MSG_HIGH(
        "eem_al_rx_comp_cb():status=CANCELED, buf=0x%x, size=%d (handle=%d)", 
        buf,
        instance_info->data_info.read_buf_info.data_read_num_bytes,  
        instance_info->handle);
      break;
    }
  case CDC_TRANSFER_OK:
    {
      HSU_ASSERT(size != 0);
      HSU_ASSERT(buf != NULL);

      rex_enter_crit_sect(&eem_al_critical_section);

      /* Handle the case in which the application has close the channel after
         this callback was invoked - just ignore this frame */
      if (instance_info->state != HSU_HOST_AL_EEM_INSTANCE_STATE_OPENED)
      {
        instance_info->data_info.read_buf_info.data_read_num_bytes = 0;
        instance_info->data_info.read_buf_info.data_rx_buffer = NULL;

        HSU_MSG_HIGH(
          "eem_al_rx_comp_cb():expected state=OPENED, state is %d", 
          instance_info->state, 0, 0);
        rex_leave_crit_sect(&eem_al_critical_section);
        break;
      }

      /* The channel is opened and the application is ready to accept the new
         frame */
      instance_info->data_info.read_buf_info.data_read_state
        = HSU_HOST_AL_EEM_BUFFER_STATE_FULL;
      instance_info->data_info.read_buf_info.data_read_num_bytes = size;
      instance_info->data_info.read_buf_info.data_rx_buffer = buf;

      (void)eem_al_resume();

      rex_leave_crit_sect(&eem_al_critical_section);

      eem_handle_read_complete(instance_info);
      break;
    }
  default:
    {
      /* In case of transfer error, ignore the frame */
      instance_info->data_info.read_buf_info.data_read_num_bytes = 0;
      instance_info->data_info.read_buf_info.data_rx_buffer = NULL;

      HSU_MSG_ERROR("eem_al_rx_comp_cb(): unknown status=%d,\
                    buf=0x%x, size=%d", status, buf, size);
    } /* default */
  } /* switch(status) */
} /* eem_al_rx_comp_cb */

/*==============================================================================

FUNCTION      EEM_AL_ECHO_COMP_CB

DESCRIPTION   
This callback is called by the Class Driver when a previous Echo request has been
forwarded to the HW. 
This function updates the echo_buffer_info data structure, such that the next 
Echo can be sent by the EEM-AL when needed.

DEPENDENCIES  
A previous call to uwh_eem_echo().

RETURN VALUE  
None.

SIDE EFFECTS  
None. 

==============================================================================*/
static void eem_al_echo_comp_cb
(
 uwh_cdc_app_h app_ctx, 
 jresult_t status, 
 void *buf,
 juint32_t size, 
 void *priv
)
{
  hsu_host_al_eem_instance_info_type *instance_info;

  if (!eem_app_context_is_valid(app_ctx))
  {
    HSU_MSG_ERROR("eem_al_echo_comp_cb(): wrong application context",
      0, 0, 0);
    HSU_ASSERT((void*)"eem_al_echo_comp_cb() failed because app_ctx\
                      is not valid" == NULL);
    return;
  }

  instance_info = (hsu_host_al_eem_instance_info_type*)(app_ctx);

  /* Must be the same buffer we passed to uwh_eem_echo */
  HSU_ASSERT(buf == 
    instance_info->data_info.echo_buffer_info.tx_buffer);
  HSU_ASSERT(instance_info->data_info.echo_buffer_info.tx_buf_state ==
    HSU_HOST_AL_EEM_BUFFER_STATE_FULL);
  HSU_PARAM_COULD_BE_CONST(buf);

  rex_enter_crit_sect(&eem_al_critical_section);

  switch(status)
  {
  case ECANCEL:
    {
      /* This can happen in case the device has been detached, or the 
         application has closed the channel. Ignore it. */
      instance_info->data_info.echo_buffer_info.tx_buf_num_of_bytes = 0;
      instance_info->data_info.echo_buffer_info.tx_buf_state =
        HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;

      HSU_MSG_HIGH(
        "eem_al_echo_comp_cb(): status=CANCEL, buf=0x%x (handle=%d)",
        buf ,instance_info->handle, 0);
      break;
    }
  case CDC_TRANSFER_OK:
    {
      (void)eem_al_resume();
      break; /* handling code is after the end of the switch statement */
    }
  default:
    {
      /* This shouldn't happen in normal operation, but if it does, ignore
         the frame. */
      instance_info->data_info.echo_buffer_info.tx_buf_num_of_bytes = 0;
      instance_info->data_info.echo_buffer_info.tx_buf_state =
        HSU_HOST_AL_EEM_BUFFER_STATE_IDLE;

      HSU_MSG_ERROR(
        "eem_al_echo_comp_cb(): unknown status=%d, buf=0x%x, handle=%d",
        status, buf,instance_info->handle);
    }
  }

  rex_leave_crit_sect(&eem_al_critical_section);
} /* eem_al_echo_comp_cb */

/*==============================================================================
HS-USB SOFTWARE API FUNCTIONS
===============================================================================*/

/*==============================================================================

FUNCTION      HSU_HOST_AL_EEM_INIT

DESCRIPTION   
This function initializes the adaptation layer. It is called by the 
HSUSB hsu_config_selector before the host stack is initialized. 

DEPENDENCIES  
This function should be called before any other function in this header is 
called.

RETURN VALUE  
TRUE for success, FALSE for failure.

SIDE EFFECTS  
None. 

==============================================================================*/
boolean hsu_host_al_eem_init(void)
{
  byte * allocated_start_addr = NULL;
  jresult_t jresult;
  int i;
#if !defined (FEATURE_HSUSB_WRAPPER_FREE) && !defined (FEATURE_HSU_TEST)
  sio_device_info_type sio_info;
#endif /* !FEATURE_HSUSB_WRAPPER_FREE && !FEATURE_HSU_TEST */

  /* Initialize memory for all supported instances */
  for (i=0; i<HSU_HOST_AL_EEM_MAX_INSTANCES; i++)
  {
    eem_al_instance_info[i].state = HSU_HOST_AL_EEM_INSTANCE_STATE_INITIALIZED;
    eem_al_instance_info[i].flush_was_issued = FALSE;

#ifndef FEATURE_HSU_TEST

    /* Allocate memory for the TX buffer and Echo command buffer*/ 
    allocated_start_addr = 
      hsu_conf_sel_stack_utils_alloc_stack_memory(2*HSU_HOST_AL_EEM_TX_BUFFER_LENGTH);

    if(allocated_start_addr == NULL)
    {
      HSU_MSG_ERROR(
        "hsu_host_al_eem_init(): hsu_conf_sel_stack_utils_alloc_stack_memory failed",
        0,0,0);
      HSU_ASSERT(
        (char*)"hsu_host_al_eem_init(): hsu_conf_sel_stack_utils_alloc_stack_memory failed"
        == NULL);
      return FALSE;
    }

    /* For now assume handling only 1 device instance, at index 0 */
    eem_al_instance_info[i].data_info.write_buffer_info.tx_buffer = 
      (uint8 *)(allocated_start_addr);

    allocated_start_addr += HSU_HOST_AL_EEM_TX_BUFFER_LENGTH;

    eem_al_instance_info[i].data_info.echo_buffer_info.tx_buffer = 
      (uint8 *)(allocated_start_addr);

#endif /* !FEATURE_HSU_TEST */

  }

  eem_clear_data();

  /* Register callbacks from the EEM Class Driver */
  eem_al_cd_cbs.attach = eem_al_attach_cb;
  eem_al_cd_cbs.detach = eem_al_detach_cb;
  eem_al_cd_cbs.echo = eem_al_echo_cb;
  eem_al_cd_cbs.response_hint = NULL;
  eem_al_cd_cbs.suspend_hint = NULL;

  /* Define the timer for entering suspend */
  rex_def_timer_ex(&eem_al_suspend_timer,
    eem_al_suspend_timer_cb,
    0);
  eem_al_is_suspended = FALSE;

  jresult = uwh_eem_register(&eem_al_cd_cbs);

  if (jresult != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "hsu_host_al_eem_init(): uwh_eem_register failed (jresult=%d)",
      jresult,0,0);
    HSU_ASSERT(
      (char*)"hsu_host_al_eem_init(): uwh_eem_register failed"
      == NULL);
    return FALSE;
  }

  /* Change state of all supported instances to INITIALIZED */
  for (i=0; i<HSU_HOST_AL_EEM_MAX_INSTANCES; i++)
  {
    eem_al_instance_info[i].state = HSU_HOST_AL_EEM_INSTANCE_STATE_INITIALIZED;
  }

#if !defined (FEATURE_HSUSB_WRAPPER_FREE) && !defined (FEATURE_HSU_TEST)

  sio_info.HW_open = hsu_host_al_eem_data_open;
  sio_info.HW_close = hsu_host_al_eem_data_close;
  sio_info.HW_ioctl = hsu_host_al_eem_ioctl;
  sio_info.HW_flush_tx = hsu_host_al_eem_data_flush_tx;
  sio_info.HW_transmit = hsu_host_al_eem_data_transmit;
  sio_info.HW_disable_device = NULL;
  sio_info.HW_control_transmit = NULL;
  sio_info.HW_control_close = NULL;
  sio_info.HW_control_open = NULL;
  sio_info.forward_port = FALSE;
  sio_info.device_id = DEVICE_ID(8, 2);

  if (sio_register_driver(&sio_info) != 1)
  {
    HSU_MSG_ERROR(
      "hsu_host_al_eem_init(): sio_register_driver failed", 0, 0, 0);
    return FALSE;
  }
#endif /* !FEATURE_HSUSB_WRAPPER_FREE && !FEATURE_HSU_TEST */

  return TRUE;
} /* hsu_host_al_eem_init */
