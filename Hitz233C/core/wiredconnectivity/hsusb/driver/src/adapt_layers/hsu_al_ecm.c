/*==============================================================================

High Speed USB Ethernet Networking control Model (CDC/ECM) Adaptation Layer

GENERAL DESCRIPTION
Implementation of the High Speed USB CDC/ECM Adaptation Layer.
This includes implementation of the API functions defined by the CDC/ECM 
function driver in ecm_fd_api.h (such as callback functions), and the API 
exposed to the Applications (sio_usb like API).

EXTERNALIZED FUNCTIONS
hsu_al_ecm_config
hsu_al_ecm_control_open
hsu_al_ecm_data_open
hsu_al_ecm_control_close
hsu_al_ecm_data_close
hsu_al_ecm_control_transmit
hsu_al_ecm_data_transmit
hsu_al_ecm_ioctl
hsu_al_ecm_data_flush_tx
hsu_al_ecm_control_close
hsu_al_ecm_data_close

INITALIZATION AND SEQUENCING REQUIREMENTS
The stack must be initialized first (which causes ecm_init to be called), 
before all other functions in this header file can be used.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_ecm.c#32 $
$DateTime: 2009/02/11 06:48:45 $ $Author: ablay $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
05/17/06  ke      Initial version

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_al_ecm.h"
#include "customer.h" /* For FEATURE_HSU_TEST to work */
#include "hsu_common.h"
#include "err.h"
#include "rex.h"

#include "hsu_al_dsm_utils.h"
#include "hsu_al_dmov_api.h"
#include "hsu_al_task.h"
#include "hsu_cdc_fd_api.h"
#include "hsu_ecm_fd_api.h"
#include "port.h"

#include "hsu_conf_sel_stack_utils.h"
#include "hsu_bus_mode_notifier.h"

#if defined(FEATURE_HSU_TEST) || defined(FEATURE_HSU_ECM_APP_STUB)
#error code not present
#endif

#include "jerrno.h"
#include "jtypes.h"

#include <stdio.h>

#include "hsu_core_config.h"
#include "hsu_conf_sel_core_config.h"

#include "jusb_common.h"

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
#define HSU_ECM_MAX_ETHERNET_FRAME_LENGTH 1514
/* The maximal length of an Ethernet frame, sent by the application */ 
#ifdef FEATURE_DATA_QMI_QOS
#define HSU_AL_ECM_QOS_HEADER_SIZE 6 
/* Header size added when QOS is enabled*/
#endif /*FEATURE_DATA_QMI_QOS*/

#ifdef FEATURE_HS_USB_TLP
  #define HSU_AL_ECM_NUM_OF_AGGREGATED_PACKETS  4
  #ifdef FEATURE_DATA_QMI_QOS
    #define HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES \
          HSU_AL_ECM_NUM_OF_AGGREGATED_PACKETS* \
            (HSU_ECM_MAX_ETHERNET_FRAME_LENGTH + TLP_HEADER_SIZE + \
             HSU_AL_ECM_QOS_HEADER_SIZE)   
   /*4 Ethernet frames + 6 bytes header for QOS on each Ethernet frame*/
  #else
    #define HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES \
          HSU_AL_ECM_NUM_OF_AGGREGATED_PACKETS* \
            (HSU_ECM_MAX_ETHERNET_FRAME_LENGTH + TLP_HEADER_SIZE ) /*4 Ethernet frames */
  #endif  /*FEATURE_DATA_QMI_QOS*/
  #ifdef HSUSB_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES
    #define HSU_AL_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES HSUSB_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES
  #else
    #define HSU_AL_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES 0x4000   /*16 Kbytes = 16384*/
  #endif /* HSUSB_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES */
  /* The size of the data buffer if TLP protocol is enabled. Contains several
  ethernet frames. The size was defined as in ACM. Perhaps will be modified 
  later*/
#else
  #define HSU_AL_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES HSU_ECM_MAX_ETHERNET_FRAME_LENGTH
  #ifdef FEATURE_DATA_QMI_QOS
    #define HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES HSU_ECM_MAX_ETHERNET_FRAME_LENGTH + \
            HSU_AL_ECM_QOS_HEADER_SIZE
      /* The maximal length of an Ethernet frame, sent by the application + 6 bytes
         header for QoS */
  #else
    #define HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES HSU_ECM_MAX_ETHERNET_FRAME_LENGTH
      /* The maximal length of an Ethernet frame, sent by the application */
  #endif  /*FEATURE_DATA_QMI_QOS*/
#endif

#define TLP_SYNC_BITS 0xF800
/* Used for updating the packet length in TLP protocol*/

#ifdef T_REXNT
#error code not present
#else
  #define HSU_AL_ECM_PACKED_SIZES_ARRAY_SIZE CHAIN_DESC_ARRAY_SIZE
#endif

#define HSU_ENCAPS_CMD_MAX_SIZE 0x1000  /*4K*/ 
/* According to DS team the size of largest QMI msg is 4K (Sep. 2008) */

#define QMI_ETH_VENDOR_ID  (0x0250F3)
/* Needed for MAC address generation. See ds_rmnet_sm.c, line 1173 */

#define HSU_ECM_MAC_ADDR_AS_STR_LENGTH 13
/* Length of a MAC address, when the format is a string "0123456789ab" */

#ifdef FEATURE_DSM_LARGE_ITEMS
#define HSU_ECM_AL_DSM_POOL_ID DSM_DS_LARGE_ITEM_POOL
#else
#define HSU_ECM_AL_DSM_POOL_ID DSM_DS_SMALL_ITEM_POOL
#endif /*FEATURE_DSM_LARGE_ITEMS */

#define HSU_EL_ECM_CONNECTION_SPEED_CHANGED_TO_UP 0x40000

/*TODO: remove! and also in hsu_al_ecm_ioctl()*/
#define HSU_TLP_VERSION_NONE 0 
/* as defined by rmnet_sio_tlp_ver_type, in ds_rmnet_sio.h line 80 */

#define AL_SER_ECM_DTR_ASSERTED_MASK (0x1)
/* See CDC Spec. Section 6.2.14 */

#define HSU_AL_ECM_SUBCLASS         0xFF
#define HSU_AL_ECM_PROTOCOL         0xFF


/*==============================================================================
Typedefs
==============================================================================*/
/*
* Enum representing the current state of an adaptation layer instance.
*/
typedef enum
{
  HSU_ECM_INSTANCE_STATE_NONE,
  /* initial value, before hsu_al_ecm_init was called */
  HSU_ECM_INSTANCE_STATE_INITIALIZED,
  /* hsu_al_ecm_init was called, but neither hsu_al_ecm_control_open nor 
  hsu_al_ecm_data_open were called */
  HSU_ECM_INSTANCE_STATE_HALF_OPENED,
  /* hsu_al_ecm_init was called, and one of either and hsu_al_ecm_control_open 
  or hsu_al_ecm_data_open (but not both yet). This state means there's one
  more call to hsu_al_ecm_*_open() expected before the instance is 
  considered fully OPENED. */
  HSU_ECM_INSTANCE_STATE_OPENED
  /* hsu_al_ecm_init, hsu_al_ecm_control_open and hsu_al_ecm_data_open were 
  called */
} hsu_al_ecm_instance_states_enum;

/*
* Enum representing the current state of a read/write buffer used for the Bulk 
* EP.
*/
typedef enum
{
  HSU_ECM_BUFFER_STATE_IDLE, 
  /* Free for use */
  HSU_ECM_BUFFER_STATE_PENDING_AT_STACK,
  /* given to a cdc_read/cdc_write operation, which still hasn't completed */
  HSU_ECM_BUFFER_STATE_READY_FOR_DMOV,
  /* returned from read_complete_cb, but inbound flow is disabled, so
  awaiting it to be enabled, so that it will send it to the DMOV for 
  scattering, and then pass it to the application */ 
  HSU_ECM_BUFFER_STATE_PENDING_AT_DMOV
  /* given to the DMOV scatter/gather API, which still hasn't completed */
} hsu_al_ecm_buffer_state_enum;

/*
* This struct contains the application level CB, that should be called 
* from the hsu_al_ecm callback invoked by the FD. 
*/
typedef struct
{
  /* Host to device request handlers  */
  /************************************/
  sio_vpu_func_ptr_type enable_disable_cb_ext;
  /* the application CB to be called when the enabled/disabled state of the 
  instance changes */
  void *                enable_disable_ext_user_data;
  /* the user data to be passed when the application CB above is invoked */
} hsu_al_ecm_app_cb_type;

struct hsu_al_ecm_instance_info_type; /* forward declaration */
typedef struct hsu_al_ecm_instance_info_type hsu_al_ecm_instance_info_type;


/*
* This structure contains information related to a buffer, given to the stack
* for data read operations.
* 
* Synchronization: 
* - The buffer itself is static. Its content is only changed by the USB stack 
*   (ECM FD). 
* - The state field is not guarded because read/write operations of integers 
*   are considered "atomic" operations.
*/
typedef struct 
{
#ifndef FEATURE_HSU_TEST
  uint8 * data_read_buffer;
#else /* FEATURE_HSU_TEST */
#error code not present
#endif /* ! FEATURE_HSU_TEST */
  /* A word on synchronization: The address of the buffer is constant, as 
  * this is a static variable. The content of the buffer is only changed in 
  * cdc_read(), meaning in the Stack's context. 
  * It's content is not used until read_complete_cb() is called. 
  */
  hsu_al_ecm_buffer_state_enum data_read_state; 
  /* whether it is idle, pending at the stack, or pending at the DMOV.
  * used for correctness verification and for synchronization. 
  *
  * IMPORTANT! This field provides synchronization only if the following 
  * guidelines are kept:
  * - Whenever a function related to this structure is called, ASSERT that 
  *   the state is as expected.
  * - change the state to the next state AFTER all changes to other fields of
  *   the structure have been made. 
  */

  dsm_item_type* dsm_chains_array_for_tlp_scatter[HSU_AL_ECM_PACKED_SIZES_ARRAY_SIZE];
  /*This array holds pointers to dsm chains that we scattered by the data mover
    from the buffer we received. */

  uint32  data_read_num_bytes;
  /* the number of bytes read by the stack, and written to the buffer */

  hsu_al_ecm_instance_info_type * instance_ptr;
  /* this is needed since the hsu_al_ecm_data_read_buffer_info_type structure 
  is used as the parameter for hsu_cdc_fd_api_cdc_read. */
} hsu_al_ecm_data_read_buffer_info_type;

/*
* This structure contains information related to a buffer, given to the stack
* for control write operations.
*/
typedef struct 
{
  uint8   ctrl_tx_buffer[HSU_ENCAPS_CMD_MAX_SIZE];
  uint32  ctrl_tx_num_of_bytes; /* the number of bytes actually used in the buffer */
} hsu_al_ecm_control_write_buffer_data_type;

/*
* This structure contains information related to a buffer, given to the stack
* for bulk write operations.
*/
typedef struct 
{
#ifndef FEATURE_HSU_TEST
  uint8 * data_tx_buffer;
#else /* FEATURE_HSU_TEST */
#error code not present
#endif /* ! FEATURE_HSU_TEST */
  
  packet_size_and_addr_in_dest_buff_pair_type packet_sizes_array[HSU_AL_ECM_PACKED_SIZES_ARRAY_SIZE];

  uint32  data_tx_buf_num_of_bytes; 
  /* the number of bytes actually used in the buffer */
  hsu_al_ecm_buffer_state_enum  data_tx_buf_state;
  /* whether it is idle, pending at the stack, or pending at the DMOV.
  * used for correctness verification and for synchronization. 
  *
  * IMPORTANT! This field provides synchronization only if the following 
  * guidelines are kept:
  * - Whenever a function related to this structure is called, ASSERT that 
  *   the state is as expected.
  * - change the state to the next state AFTER all changes to other fields of
  *   the structure have been made. 
  */

  hsu_al_ecm_instance_info_type *  instance_ptr;
  /* this is needed since the hsu_al_ecm_data_write_buffer_data_type structure 
  is used as the parameter for hsu_cdc_fd_api_cdc_write. */
} hsu_al_ecm_data_write_buffer_data_type;

/*
* This struct contains information related to the control plane of a specific
* network function instance
*/
typedef struct 
{
  sio_stream_id_type                stream_id;
  /* the unique identifier of this instance's control plane
  The unique ID is allocated by sio_control_open() */
  dsm_watermark_type *              rx_queue_ptr;      
  /* the RX WM Q provided by the application */
  dsm_watermark_type *              tx_queue_ptr; 
  /* the TX WM Q provided by the application */
  sio_rx_func_ptr_type              rx_func_ptr;
  /* the RX notification callback provided by the application */
  hsu_al_ecm_control_write_buffer_data_type write_buffer_data;
  /* Details of a buffer given to the stack using cdc_send_encap_response */ 
} hsu_al_ecm_control_info_type;

/* This struct contains information needed for re-sending a Connection Speed 
Changed Notification to the host. The resending might be needed in case when
the instance was disabled or the device was suspended. */
typedef struct 
{
  boolean                           resend;
  boolean                           is_send_opr_in_progress;
  ecm_conn_speed_t                  connection_speed_args;
} hsu_al_ecm_conn_speed_mem_type;

/*
* This struct contains information related to the data plane of a specific
* network function instance
*/
typedef struct
{
  sio_stream_id_type                stream_id;
  /* the unique identifier of this instance's data plane. 
  The unique ID is allocated by sio_open() */

  /* Details of 2 buffers given to the stack using cdc_read */ 
  hsu_al_ecm_data_read_buffer_info_type  read_buf_1_info;
  hsu_al_ecm_data_read_buffer_info_type  read_buf_2_info;

  boolean                           inbound_flow_is_enabled;

  boolean                           dl_tlp_enabled;
  /*Indication whether downlink packet bundling is enabled (TLP)*/
  boolean                           ul_tlp_enabled;
  /*Indication whether uplink packet bundling is enabled (TLP)*/

  hsu_al_ecm_data_write_buffer_data_type write_buffer_data;
  /* Details of a buffer given to the stack using cdc_write */ 
  dsm_watermark_type *              rx_queue_ptr;      
  /* the RX WM Q provided by the application */
  dsm_watermark_type *              tx_queue_ptr; 
  /* the TX WM Q provided by the application */
  sio_rx_func_ptr_type           rx_func_ptr;
  /* the RX notification callback provided by the application */

  /* parameters for sending a Connection Speed Changed Notification to the host, 
  either directly through hsu_ecm_fd_api_notify_conn_speed_change(), 
  or using the AL task, through defer_call_to_notif_con_speed_ch()*/
  ecm_conn_speed_t                  connection_speed_args;

  hsu_al_ecm_conn_speed_mem_type    conn_speed_mem;
} hsu_al_ecm_data_info_type;

/*
* This struct contains information of a specific network function instance
*/
struct hsu_al_ecm_instance_info_type
{
  cdc_handle_t                    handle;
  /* A unique handle for this instance, that is used for the various stack 
  APIs (CDC's or ECM's) */
  hsu_al_ecm_instance_states_enum state;
  /* the current state of this instance */
  boolean                         instance_is_enabled;
  /* Indicates whether this instance is enabled (TRUE) or disabled (FALSE) */
  hsu_al_ecm_app_cb_type          app_callbacks;
  /* the application level CB, to be called from the hsu_al_ecm callbacks, 
  that are invoked by the FD. */
  hsu_al_ecm_control_info_type    control_info;
  /* information related to the control plane of this instance */
  hsu_al_ecm_data_info_type       data_info;
  /* information related to the data plane of this instance */
  boolean             ctrl_line_state;
  /* Control Line State: See CDC Spec. Section 6.2.14 */
};

/*==============================================================================
Variables
==============================================================================*/
/* 
* The instance information (for now, there's only one instance of the 
* network function)
*/
static hsu_al_ecm_instance_info_type ecm_instance_info;

/* 
* Keeps the handles or the FDs that the stack initialized.
* When the configuration will be changed we will use the corresponding handle 
* for the stack's FD.
*/
cdc_handle_t ecm_fd_handle[HSU_MAX_NUM_OF_CORES][HSU_MAX_NUM_OF_CONFIGS];


/* 
* The number of ECM instances that were allocated and initialized (for now, 1 is the 
* maximal possible value because there's only one instance of the 
* network function and one configuration). In case of multi config it can be more than 1.
*/
static uint8 hsu_al_ecm_num_of_initialized_instances = 0;

static rex_crit_sect_type hsu_al_ecm_critical_section;

static hsu_bus_mode_enum hsu_al_ecm_suspend_resume_mode = HSU_BUS_MODE_SUSPENDED;

/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/
static void hsu_al_ecm_write_comp_cb
(
 cdc_iostatus_t  status, 
 void *          buffer_ptr,
 cdc_appctx_t    ctx
 );

static void hsu_al_ecm_encap_resp_comp_cb
(
 cdc_iostatus_t  status,
 juint32_t       bytes_sent,
 cdc_appctx_t    ctx
 );

static void remember_conn_speed_notif
(
 hsu_al_ecm_instance_info_type * context_ptr
);

static boolean hsu_al_ecm_read_comp_err_handle
(
 hsu_al_ecm_instance_info_type *         context_ptr,
 hsu_al_ecm_data_read_buffer_info_type * read_comp_buf_data_ptr
 );

/*==============================================================================

FUNCTION      HSU_AL_ECM_CONFIG

DESCRIPTION   
Called by the hsu_config_selector when the device stack is initialized for the 
first time after power up. 
The function requests from the hsu_config_selector the maximal amount of 
memory it may require, and initializes the buffers passed to the HS-USB HW 
according to the allocated address. 

DEPENDENCIES  
This function should be called before any other function is called, 
specifically before ecm_init is called.

RETURN VALUE  
TRUE for success, FALSE if the memory allocation by the hsu_config_selector 
failed. 

SIDE EFFECTS  
None. 

==============================================================================*/
boolean hsu_al_ecm_config(void)
{
#ifndef FEATURE_HSU_TEST
  byte * allocated_start_addr = NULL;

  /* We need to allocate memory for the buffers one at the time. 
  Those buffers must be 32 bytes aligned for the DMOV to be happy. 
  The function hsu_conf_sel_stack_utils_alloc_stack_memory() returns a pointer 
  that's 32 bytes aligned, but the size of the buffers allocated isn't. 
  So, by calling hsu_conf_sel_stack_utils_alloc_stack_memory() 3 times, we 
  guarantee that all buffers are 32 bytes aligned */
  allocated_start_addr = 
    hsu_conf_sel_stack_utils_alloc_stack_memory(HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES);
  if(allocated_start_addr == NULL)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_config(): hsu_conf_sel_stack_utils_alloc_stack_memory #1 failed",
      0,0,0);
    HSU_ASSERT(
      (char*)"hsu_al_ecm_config(): hsu_conf_sel_stack_utils_alloc_stack_memory failed"
      == NULL);
    return FALSE;
  }
  ecm_instance_info.data_info.read_buf_1_info.data_read_buffer = 
    (uint8 *)(allocated_start_addr);

  allocated_start_addr = 
    hsu_conf_sel_stack_utils_alloc_stack_memory(HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES);
  if(allocated_start_addr == NULL)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_config(): hsu_conf_sel_stack_utils_alloc_stack_memory #2 failed",
      0,0,0);
    HSU_ASSERT(
      (char*)"hsu_al_ecm_config(): hsu_conf_sel_stack_utils_alloc_stack_memory failed"
      == NULL);
    return FALSE;
  }
  ecm_instance_info.data_info.read_buf_2_info.data_read_buffer = 
    (uint8 *)(allocated_start_addr);

  allocated_start_addr = 
    hsu_conf_sel_stack_utils_alloc_stack_memory(HSU_AL_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES);
  if(allocated_start_addr == NULL)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_config(): hsu_conf_sel_stack_utils_alloc_stack_memory #3 failed",
      0,0,0);
    HSU_ASSERT(
      (char*)"hsu_al_ecm_config(): hsu_conf_sel_stack_utils_alloc_stack_memory failed"
      == NULL);
    return FALSE;
  }
  ecm_instance_info.data_info.write_buffer_data.data_tx_buffer = 
    (uint8 *)(allocated_start_addr);

  return TRUE;

#else /* FEATURE_HSU_TEST */
#error code not present
#endif /* ! FEATURE_HSU_TEST */
} /* hsu_al_ecm_config */

/*==============================================================================

FUNCTION      IS_CDC_APP_CONTEXT_VALID

DESCRIPTION   
Verifies that the cdc_appctx_t given by CDC stack functions contains a valid 
value. 

Currently, since only one instance of the network driver is supported, It does 
so by comparing the context to the static ecm_instance_info structure address. 
In the future, it will go over all existing instances and verify that there's 
one that satisfies this requirement. 

DEPENDENCIES  
None.

RETURN VALUE  
TRUE if the context is valid, FALSE if not.

SIDE EFFECTS  
None.
==============================================================================*/
static boolean cdc_app_context_is_valid
(
 cdc_appctx_t  context
 )
{
  if(context != &ecm_instance_info)
  {
    HSU_MSG_ERROR(
      "cdc_app_context_is_valid(): context=0x%04x, &ecm_instance_info = 0x%04x",
      context, &ecm_instance_info, 0);
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* cdc_app_context_is_valid */

/*==============================================================================

FUNCTION      DEFER_CALL_TO_CDC_READ

DESCRIPTION   
Called in the AL Task's context.
Used when we need to call the stack's cdc_read API from an interrupt context 
(DMOV). In such cases, we enqueue a cmd to the AL task, and signal it. 
When the AL task gets the signal and handles the command, it calls this 
function, but from the AL task's context. Then, we can safely call the API
hsu_cdc_fd_api_cdc_read(), which locks the giant mutex and calls cdc_read(). 

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void defer_call_to_cdc_read(void* args)
{
  jresult_t jresult;
  hsu_al_ecm_data_read_buffer_info_type * cdc_read_args = NULL;
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  HSU_ASSERT(args != NULL);
  cdc_read_args = (hsu_al_ecm_data_read_buffer_info_type *)(args);
  context_ptr = cdc_read_args->instance_ptr;
  HSU_ASSERT(context_ptr != NULL);

  /* 
  * Verify that the instance is not disabled. If it is disabled, do not issue
  * a new cdc_read 
  */
  /* Lock Giant mutex */
  jsafe_enter();
  if(context_ptr->instance_is_enabled == FALSE)
  {
    cdc_read_args->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;

    /* Release Giant mutex */
    jsafe_leave();

    HSU_MSG_MED(
      "defer_call_to_cdc_read(): NOT calling cdc_read, since instance is disabled\
      (handle=%d)",
      context_ptr->handle,0,0);
    return;
  }

  jresult = hsu_cdc_fd_api_cdc_read(context_ptr->handle, 
    cdc_read_args->data_read_buffer, 
    HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES);

  /* Release Giant mutex */
  jsafe_leave();

  if(jresult != HSU_JSUCCESS)
  {
    cdc_read_args->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR(
      "defer_call_to_cdc_read(): hsu_cdc_fd_api_cdc_read returned error %d.\
      buffer=0x%x, handle=%d", 
      jresult,
      cdc_read_args->data_read_buffer,
      context_ptr->handle);
    HSU_ASSERT(
      (uint32)"defer_call_to_cdc_read(): hsu_cdc_fd_api_cdc_read returned error" 
      == NULL);
  }
} /* defer_call_to_cdc_read */

/*==============================================================================

FUNCTION      DEFER_CALL_TO_NOTIF_CON_SPEED_CH

DESCRIPTION  
This unction should be called when the Gaint Mutex is locked!
It can be called from 2 contexts:
- Application context (from enable/disable inbound flow)
- HSUSB-0 context from hsu_al_ecm_notif_conn_speed_ch_comp_cb() in case of 
  resending a notification in memory
Used when we need to call the stack's ecm_notify_conn_speed_change API. 

DEPENDENCIES  
This function should be called when the Gaint Mutex is locked

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void defer_call_to_notif_con_speed_ch(hsu_al_ecm_instance_info_type* context_ptr)
{
  jresult_t jresult;
  /* 
  * Verify that the instance is not disabled. If it is disabled, do not issue
  * a new cdc_read 
  */
  if(context_ptr->instance_is_enabled == FALSE)
  {
    HSU_MSG_HIGH(
      "defer_call_to_notif_con_speed_ch(): Instance is disabled (handle=%d). Ignoring.",
      context_ptr->handle,0,0);

    /* If the notification is disable_inbound_flow then we need to 
       remember and resend it. We don't need to check upstream since it's 
       always up*/
    if (context_ptr->data_info.connection_speed_args.downstream_bitrate == 0)
    {
      remember_conn_speed_notif(context_ptr);
    }
    return;
  }

  jresult = hsu_ecm_fd_api_notify_conn_speed_change(
    context_ptr->handle, 
    &(context_ptr->data_info.connection_speed_args));

  if(jresult == ESUSPENDED)
  {
    /* We need to remember the notification and resend upon resume since the host
      remembers the flow state before suspension.*/
    remember_conn_speed_notif(context_ptr);
   
    /* 
    * The device is suspended (for example - the USB cable was disconnected). 
    */
    HSU_MSG_HIGH(
      "defer_call_to_notif_con_speed_ch(): hsu_ecm_fd_api_notify_conn_speed_change\
      returned ESUSPENDED. Remembering notification (handle=%d)",
      context_ptr->handle,0,0);
    return;
  }

  if(jresult != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "defer_call_to_notif_con_speed_ch(): \
      hsu_ecm_fd_api_notify_conn_speed_change returned error %d.\
      connection.down=0x%x, handle=%d", 
      jresult,
      context_ptr->data_info.connection_speed_args.downstream_bitrate,
      context_ptr->handle);
    HSU_ASSERT(
      (uint32)"defer_call_to_notif_con_speed_ch(): \
              hsu_ecm_fd_api_notify_conn_speed_change returned error" 
              == NULL);
  }
} /* defer_call_to_notif_con_speed_ch */

/*==============================================================================

FUNCTION      SEND_OR_REMEMBER_CON_SPEED_CH

DESCRIPTION  
This function is called in the application context.
It checks if we're currently in the middle of sending the previous connection 
speed notification. If so - the current notification is remembered and will be 
sent upon receiving a hsu_al_ecm_notif_conn_speed_ch_comp_cb. 

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void send_or_remember_con_speed_ch(void *args)
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  HSU_ASSERT(args != NULL);
  context_ptr = (hsu_al_ecm_instance_info_type *)(args);

  jsafe_enter();

  /* Verify that the last connection_speed_notification was already sent.
  This needs to be protected because this value is checked
  in app context but will be updated by HSUSB0 */
  if (context_ptr->data_info.conn_speed_mem.is_send_opr_in_progress)
  {
    HSU_MSG_LOW("send_or_remember_con_speed_ch: sending of last notif is in progress"
      " Remembering current notification.",0,0,0);
    remember_conn_speed_notif(context_ptr);
#ifdef T_REXNT
#error code not present
#endif
    jsafe_leave();
    return;
  }

  context_ptr->data_info.conn_speed_mem.is_send_opr_in_progress = TRUE;
  defer_call_to_notif_con_speed_ch(context_ptr);
  jsafe_leave();
}/*send_or_remember_con_speed_ch*/

/*==============================================================================

FUNCTION      DEFER_CALL_TO_CDC_WRITE

DESCRIPTION   
Called in the AL Task's context.
Used when we need to call the stack's cdc_write API from an interrupt context 
(DMOV). In such cases, we enqueue a cmd to the AL task, and signal it. 
When the AL task gets the signal and handles the command, it calls this 
function, but from the AL task's context. Then, we can safely lock the giant 
mutex and call cdc_write. 

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void defer_call_to_cdc_write(void* args)
{
  jresult_t jresult;
  hsu_al_ecm_data_write_buffer_data_type * cdc_write_args = NULL;
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  HSU_ASSERT(args != NULL);
  cdc_write_args = (hsu_al_ecm_data_write_buffer_data_type *)(args);
  context_ptr = cdc_write_args->instance_ptr;
  HSU_ASSERT(context_ptr != NULL);

  /* The following is to prevent the scenario:
  1. DMOV gather API was invoked
  2. DMOV gather done called (in INT context), enqueues a command to the AL task.
  3. AL task is scheduled, defer_call_to_cdc_write() is called, and 
  3. (If AL task doesn't lock the Giant mutex) AL task just about to issue cdc_write...
  4. The CDC FD invokes the disable_cb.
  5. cdc_write is called, but the I/F is already disabled!
  */

  /* Lock Giant mutex */
  jsafe_enter();

  /* Here it's safe to check if enabled/disabled - as only the FD 
  can modify these values - and right now it does not have the giant mutex */
  if (context_ptr->instance_is_enabled == FALSE)
  {
    /* All enqueue and dequeue operations are protected by this critical section. */
    rex_enter_crit_sect(&hsu_al_ecm_critical_section);

    /* ignore this frame, and any other data pending in the data TX WM Q */
    cdc_write_args->data_tx_buf_num_of_bytes = 0;
    cdc_write_args->data_tx_buf_state = HSU_ECM_BUFFER_STATE_IDLE;
    dsm_empty_queue(context_ptr->data_info.tx_queue_ptr);

    rex_leave_crit_sect(&hsu_al_ecm_critical_section);

    /* Release Giant mutex */
    jsafe_leave();

    HSU_MSG_HIGH(
      "defer_call_to_cdc_write: Instance is disabled (handle=%d), Ignoring.", 
      context_ptr->handle, 0, 0);
    return;
  }

  jresult = hsu_cdc_fd_api_cdc_write(context_ptr->handle, 
    cdc_write_args->data_tx_buffer, 
    cdc_write_args->data_tx_buf_num_of_bytes);

  /* If the write operation failed because the device was suspended, treat it as 
  if the hsu_al_ecm_read_comp_cb was invoked with status CANCELED (no need to release the 
  Giant mutex since the callback is usually invoked from the FD's context, when 
  it is locked) */
  if (jresult == ESUSPENDED)
  {
    /* No need to free buffer, it will be done in tx_done: */
    hsu_al_ecm_write_comp_cb(CDC_TRANSFER_CANCELLED, 
      (void *)(cdc_write_args->data_tx_buffer), 
      (cdc_appctx_t)context_ptr);

    /* Release Giant mutex */
    jsafe_leave();

    HSU_MSG_HIGH(
      "defer_call_to_cdc_write: hsu_cdc_fd_api_cdc_write returned ESUSPENDED, \
      cancelled TX (handle = %d)", 
      context_ptr->handle, 0, 0);
    return;
  }

  /* Release Giant mutex */
  jsafe_leave();

  if(jresult != HSU_JSUCCESS)
  {

    cdc_write_args->data_tx_buf_num_of_bytes = 0;
    cdc_write_args->data_tx_buf_state = HSU_ECM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR(
      "defer_call_to_cdc_write(): hsu_cdc_fd_api_cdc_write returned error %d.\
      buffer=0x%x, handle=%d", 
      jresult,
      cdc_write_args->data_tx_buffer,
      context_ptr->handle);
    HSU_ASSERT(
      (uint32)"defer_call_to_cdc_write(): hsu_cdc_fd_api_cdc_write returned error" 
      == NULL);
  }
} /* defer_call_to_cdc_write */


/*==============================================================================

FUNCTION      INVOKE_APP_ENABLE_DISABLE_CBS

DESCRIPTION   
Invokes the application DTR callbacks, if they were registered. 

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void invoke_app_enable_disable_cbs
(
 hsu_al_ecm_instance_info_type * context_ptr
 )
{
  HSU_ASSERT(context_ptr != NULL);

  if(context_ptr->app_callbacks.enable_disable_cb_ext != NULL)
  {
    HSU_MSG_MED(
      "invoke_app_enable_disable_cbs(): calling application ext CB (handle=%d)",
      context_ptr->handle,0,0);
    context_ptr->app_callbacks.enable_disable_cb_ext(
      context_ptr->app_callbacks.enable_disable_ext_user_data);
  }
} /* invoke_app_enable_disable_cbs */

/*===========================================================================

FUNCTION hsu_al_ecm_alloc_buffer_and_setup_read

DESCRIPTION
Sets up a new read operation for all idle buffers
 
DEPENDENCIES
This function is assumed to be called in stack context, so the giant mutex 
is already locked. No need to re-lock it. 

RETURN VALUE
None.

SIDE EFFECTS
None

===========================================================================*/
static void hsu_al_ecm_alloc_buffer_and_setup_read
(
 hsu_al_ecm_instance_info_type* context_ptr
)
{
  jresult_t result = HSU_JSUCCESS;
  if (context_ptr->data_info.read_buf_1_info.data_read_state == 
      HSU_ECM_BUFFER_STATE_IDLE)
  {
    context_ptr->data_info.read_buf_1_info.data_read_state = 
      HSU_ECM_BUFFER_STATE_PENDING_AT_STACK;
    result = cdc_read(context_ptr->handle, 
      context_ptr->data_info.read_buf_1_info.data_read_buffer, 
      HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES);
    if(result != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR("hsu_al_ecm_alloc_buffer_and_setup_read(): cdc_read #1 returned error %d", 
        result,0,0);
      HSU_ASSERT(
        (uint32)"hsu_al_ecm_alloc_buffer_and_setup_read(): cdc_read #1 returned error" == NULL);
      /* When temporary ASSERT is removed - clean up the fields that were already 
      set. */
    }
  }
  if (context_ptr->data_info.read_buf_2_info.data_read_state == 
    HSU_ECM_BUFFER_STATE_IDLE)
  {
    context_ptr->data_info.read_buf_2_info.data_read_state = 
      HSU_ECM_BUFFER_STATE_PENDING_AT_STACK;
    result = cdc_read(context_ptr->handle, 
      context_ptr->data_info.read_buf_2_info.data_read_buffer, 
      HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES);
    if(result != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR("hsu_al_ecm_alloc_buffer_and_setup_read(): cdc_read #2 returned error %d", 
        result,0,0);
      HSU_ASSERT(
        (uint32)"hsu_al_ecm_alloc_buffer_and_setup_read(): cdc_read #2 returned error" == NULL);
      /* When temporary ASSERT is removed - consider flushing the previous buffer 
      item given to the stack, and clean up the fields that were already set. */
    }
  }
}
/*==============================================================================

FUNCTION      HSU_AL_ECM_SUSPEND_RESUME_MODE_CHANGED_CB

DESCRIPTION   
  Invoked when the USB stack gets suspended/resumed.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
  None.

==============================================================================*/
static void hsu_al_ecm_suspend_resume_mode_changed_cb
(
  hsu_bus_mode_enum new_mode,
 void* app_data
 )
{
  hsu_al_ecm_instance_info_type* context_ptr = (hsu_al_ecm_instance_info_type*)app_data;
  if (hsu_al_ecm_suspend_resume_mode == new_mode)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_suspend_resume_mode_changed_cb(): The mode is the same as before: %d",
      new_mode,0,0);
  }
 
  /* If we were suspended and now we are resumed we need to issue cdc_read
  on all idle buffers */
  if ((hsu_al_ecm_suspend_resume_mode != new_mode) &&
    (new_mode == HSU_BUS_MODE_RESUMED) && 
    (context_ptr->instance_is_enabled) &&
    (context_ptr->state == HSU_ECM_INSTANCE_STATE_OPENED) &&
    (context_ptr->data_info.inbound_flow_is_enabled)
    )
  {
    hsu_al_ecm_alloc_buffer_and_setup_read((hsu_al_ecm_instance_info_type*)app_data);
  }

  hsu_al_ecm_suspend_resume_mode = new_mode;

  /* Check if there is connection speed notification that needs to be resent
  and resend if needed. This can be true if we were suspended */
  /* TODO: When we'll have multiple RmNet instances we need to use
           context_ptr and receive it as parameter for this function */
  if ((ecm_instance_info.data_info.conn_speed_mem.resend == TRUE) &&
    (hsu_al_ecm_suspend_resume_mode == HSU_BUS_MODE_RESUMED))
  {
    ecm_instance_info.data_info.conn_speed_mem.resend = FALSE ;
    ecm_instance_info.data_info.connection_speed_args = 
      ecm_instance_info.data_info.conn_speed_mem.connection_speed_args ;
    send_or_remember_con_speed_ch((void*)(&ecm_instance_info));
  }
}

/*==============================================================================

FUNCTION      HSU_AL_ECM_ENABLED_CB

DESCRIPTION   
Called by the FD when the device has successfully enumerated. 

DEPENDENCIES  
ecm_init() must have been called for this instance.

RETURN VALUE  
jresult_t, currently only HSU_JSUCCESS is possible (failures cause an ASSERT).

SIDE EFFECTS  
None.

==============================================================================*/
static jresult_t hsu_al_ecm_enabled_cb
(
 cdc_appctx_t ctx
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  if(cdc_app_context_is_valid(ctx) == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_enabled_cb(): ctx param invalid" == NULL);
  }

  context_ptr = (hsu_al_ecm_instance_info_type *)(ctx);
  HSU_MSG_HIGH("hsu_al_ecm_enabled_cb(): Enter (handle=%d)",
    context_ptr->handle,0,0);

  /* 
  * instance_is_enabled doesn't require a critical section protection, because
  * the only access to it in the application's context, is from 
  * hsu_al_ecm_ioctl(), which performs a simple read. Since this is a boolean, 
  * the read operation is "atomic". 
  */ 
  if(context_ptr->instance_is_enabled)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_enabled_cb(): the instance is already enabled (handle=%d)!!!",
      context_ptr->handle,0,0);
    /* This CB should be called only for REAL state changes */
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_enabled_cb(): called when the instance is already \
              enabled!!!" == NULL);
    /* When this is removed, do nothing, just - 
    return HSU_JSUCCESS; 
    */
  }

#ifndef T_REXNT
  /* Set the FD handle to the current configuration's FD */
  hsu_al_ecm_handle_set_configuration();
#endif /* T_REXNT */


  /* Provide 2 buffers for read to the HS-USB stack */ 
  /**************************************************/
  /* We're called from a stack function, so the giant mutex is already locked. 
  No need to re-lock it. */
  hsu_al_ecm_alloc_buffer_and_setup_read(context_ptr);

  context_ptr->instance_is_enabled = TRUE;
  context_ptr->ctrl_line_state = FALSE;

  /* Check if there is connection speed notification that needs to be resent
     and resend if needed. */
  if ((context_ptr->data_info.conn_speed_mem.resend == TRUE) &&
      (hsu_al_ecm_suspend_resume_mode == HSU_BUS_MODE_RESUMED))
  {
    context_ptr->data_info.conn_speed_mem.resend = FALSE ;
    context_ptr->data_info.connection_speed_args = 
      context_ptr->data_info.conn_speed_mem.connection_speed_args ;
    send_or_remember_con_speed_ch((void*)context_ptr);
  }
  return HSU_JSUCCESS;
} /*hsu_al_ecm_enabled_cb*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_DISABLED_CB

DESCRIPTION   
Called by the FD when the stack identifies that the device has been detached 
from the host. 

DEPENDENCIES  
ecm_init() and hsu_al_ecm_enabled_cb() must have been called for this 
instance.

RETURN VALUE  
jresult_t, currently only HSU_JSUCCESS is possible (failures cause an ASSERT).

SIDE EFFECTS  
All pending read and writes on Bulk are aborted.

==============================================================================*/
static jresult_t hsu_al_ecm_disabled_cb
(
 cdc_appctx_t ctx
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  if(cdc_app_context_is_valid(ctx) == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_disabled_cb(): ctx param invalid" == NULL);
  }

  context_ptr = (hsu_al_ecm_instance_info_type *)ctx; 
  HSU_MSG_HIGH("hsu_al_ecm_disabled_cb(): Enter (handle=%d)",
    context_ptr->handle,0,0);

  if(context_ptr->instance_is_enabled == FALSE)
  {
    /* This CB should be called only for REAL state changes */
    HSU_MSG_ERROR(
      "hsu_al_ecm_disabled_cb(): the instance is already disabled!!! (handle=%d)",
      context_ptr->handle,0,0);
    HSU_ASSERT( 
      (uint32)"hsu_al_ecm_disabled_cb(): called when the instance is already \
              disabled!!!" == NULL);
    /* When this is removed, do nothing, just - 
    return HSU_JSUCCESS; */
  }

  context_ptr->instance_is_enabled = FALSE;
  context_ptr->ctrl_line_state = FALSE;
  invoke_app_enable_disable_cbs(context_ptr);

  /* TODO ECM - Do we want to call cdc_abort_read and cdc_abort_write our selves? 
  We can also wait for the stack to that for us... 
  Maybe abort control transfers as well? How? */

  return HSU_JSUCCESS;
} /*hsu_al_ecm_disabled_cb*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_ENCAP_COMMAND_CB

DESCRIPTION   
Called by the FD when a "Send Encapsulated Command" request has arrived from 
the host. 

DEPENDENCIES  
ecm_init() and hsu_al_ecm_control_open() must have been called for this 
instance.

RETURN VALUE  
jresult_t: HSU_JSUCCESS for success, currently all failures cause an ASSERT
(When the temporary ASSERT is removed, the value JENOMEM will be returned for 
DSM allocation failure, and JEINVAL for scattering failure)

SIDE EFFECTS  
None.
==============================================================================*/
static jresult_t hsu_al_ecm_encap_command_cb
(
 void *        command_ptr, 
 juint16_t     size,
 cdc_appctx_t  ctx
 )
{
  dsm_item_type * dsm_chain_head_ptr;
  boolean bool_result;
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  HSU_ASSERT(command_ptr != NULL);
  HSU_ASSERT(size != 0);
  HSU_PARAM_COULD_BE_CONST(command_ptr);

  if(cdc_app_context_is_valid(ctx) == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_encap_command_cb(): ctx param invalid" == NULL);
  }
  context_ptr = (hsu_al_ecm_instance_info_type *)ctx;

  HSU_MSG_HIGH("hsu_al_ecm_encap_command_cb(): called with size=%d (handle=%d)",
    size, context_ptr->handle, 0);

  if(context_ptr->state != HSU_ECM_INSTANCE_STATE_OPENED)
  {
    /* This is possible and legitimate if the ECM is part of the composition, 
    * but the application isn't using the USB link, and hasn't called sio_open(s) 
    */
    HSU_MSG_HIGH(
      "hsu_al_ecm_encap_command_cb(): Instance is not opened, ignoring CTRL msg (handle=%d)",
      context_ptr->handle, 0, 0);
    return HSU_JSUCCESS;
  }

  if(context_ptr->instance_is_enabled == FALSE)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_encap_command_cb(): instance is disabled! (handle=%d)",
      context_ptr->handle,0,0);
    HSU_ASSERT(
      (char *)"hsu_al_ecm_encap_command_cb(): instance is disabled!" == NULL);
  }

  /* Scatter buffer into DSM items */
  dsm_chain_head_ptr = 
    hsu_al_dsm_utils_generate_dsm_chain(HSU_ECM_AL_DSM_POOL_ID, size);
  if(dsm_chain_head_ptr == NULL)
  {
    HSU_MSG_ERROR("hsu_al_ecm_encap_command_cb - large DSM items allocation failed. "  
                  "Falling back to small DSM items",0 ,0 ,0);
    dsm_chain_head_ptr = 
        hsu_al_dsm_utils_generate_dsm_chain(DSM_DS_SMALL_ITEM_POOL, size);
    if(dsm_chain_head_ptr == NULL)
    {
      HSU_ASSERT((uint32)"hsu_al_ecm_encap_command_cb(): failed allocating "
                         "both large and small DSM items" == NULL);
      return JENOMEM;
    }
  }

#if defined(FEATURE_HSU_TEST) || defined(FEATURE_HSU_ECM_APP_STUB)
#error code not present
#endif /* defined(FEATURE_HSU_TEST) || defined(FEATURE_HSU_ECM_APP_STUB) */

  /* For control channel messages, the DMOV is not used. */
  bool_result = hsu_al_dsm_utils_scatter_buffer_without_dmov(command_ptr, 
    size, dsm_chain_head_ptr);
  if(bool_result == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_encap_command_cb(): \
              hsu_al_dsm_utils_scatter_buffer_without_dmov() failed" == NULL);
    return JEINVAL;
  }

  /* Enqueue DSM chain to Control RX WMQ */
  HSU_MSG_MED(
    "hsu_al_ecm_encap_command_cb(): enqueuing dsm_chain=0x%x to CTRL RX WMQ (handle=%d)",
    dsm_chain_head_ptr, context_ptr->handle, 0);

  if (context_ptr->control_info.rx_func_ptr)
  {
    context_ptr->control_info.rx_func_ptr(&dsm_chain_head_ptr);
  }
  else if(context_ptr->control_info.rx_queue_ptr)
  {
    dsm_enqueue(context_ptr->control_info.rx_queue_ptr, &dsm_chain_head_ptr);
  }
  else
  {
    HSU_ASSERT((char*)"hsu_al_ecm_encap_command_cb(): Neither RX WM nor "
               "rx_func_ptr is defined" == NULL);
  }

  return HSU_JSUCCESS;
} /*hsu_al_ecm_encap_command_cb*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_SET_PKT_FILTER_CB

DESCRIPTION   
Called by the FD when a "Set Ethernet Packet Filter" request has arrived from 
the host. 
This is a mandatory request, so a CB must be supplied, but this request is not 
handled by the AMSS, so currently - its invocation causes an ASSERT.

DEPENDENCIES  
None.

RETURN VALUE  
JEOPNOTSUPP - operation not supported. 

SIDE EFFECTS  
None.

==============================================================================*/
static jresult_t hsu_al_ecm_set_pkt_filter_cb
(
 juint16_t     pf_bitmask,
 cdc_appctx_t  ctx
 )
{
  if(cdc_app_context_is_valid(ctx) == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_set_pkt_filter_cb(): ctx param invalid" == NULL);
  }

  HSU_MSG_LOW("hsu_al_ecm_set_pkt_filter_cb(): pf_bitmask = 0x%04x",
    pf_bitmask,0,0);

  HSU_ASSERT(
    (uint32)"hsu_al_ecm_set_pkt_filter_cb(): not yet implemented" == NULL);

  return JEOPNOTSUPP;
} /*hsu_al_ecm_set_pkt_filter_cb*/

/*==============================================================================

FUNCTION      PERFORM_CONTROL_TRANSMIT

DESCRIPTION   
Called by either hsu_al_ecm_control_transmit() (application context) or by
hsu_al_ecm_encap_resp_comp_cb() (FD context), when it is allowed to  
send a DSM chain to the stack for transmission. 
When the function is called, the transmit buffer is already marked as full 
(the field ctrl_tx_num_of_bytes != 0), to ensure that further calls by the  
application to hsu_al_ecm_control_transmit() will cause the DSM chain to be 
enqueued to the control TX WM Q.

the function gathers the DSM chain into a preallocated buffer, and gives it to
the cdc_send_encap_response() function of the USB stack. 

The stack is responsible to generate a "response available" notification, and 
then send the message after the host sends an IN token on the control channel
following the notification. 

DEPENDENCIES  
ecm_init() and hsu_al_ecm_control_open() must have been called for this 
instance.
the control_info.write_buffer_data.ctrl_tx_num_of_bytes field must have 
been set to the DSM chain's length prior to the call to this function.

RETURN VALUE  
None.

SIDE EFFECTS  
None. 

==============================================================================*/
static void perform_control_transmit
( 
 hsu_al_ecm_instance_info_type *  context_ptr, 
 dsm_item_type *                  tx_ptr
 )
{
  jresult_t result;

  HSU_ASSERT(context_ptr != NULL);
  HSU_ASSERT(tx_ptr != NULL);

  /* Gather the DSM chain into a contiguous buffer */
  /************************************************/
  /* 
  * this is done using memcpy operations, and not by using the DMOV, since 
  * control messages are rather small, and it's probably not worth using the 
  * DMOV 
  */
  if(hsu_al_dsm_utils_gather_buffer_without_dmov(
    tx_ptr, /* chain_to_gather */
    context_ptr->control_info.write_buffer_data.ctrl_tx_buffer,
    sizeof(context_ptr->control_info.write_buffer_data.ctrl_tx_buffer)) 
    == FALSE)
  {
    HSU_MSG_ERROR(
      "perform_control_transmit(): \
      hsu_al_dsm_utils_gather_buffer_without_dmov failed",
      0,0,0);
    HSU_MSG_ERROR("  dsm_length_packet=%d, buf size=%d", 
      dsm_length_packet(tx_ptr),
      sizeof(context_ptr->control_info.write_buffer_data.ctrl_tx_buffer),0);

    dsm_free_packet(&tx_ptr);
    HSU_ASSERT(
      (uint32)"perform_control_transmit(): \
              hsu_al_dsm_utils_gather_buffer_without_dmov failed" == NULL);
  }

  /* Now that the message is in the buffer, no need for the DSM chain... */
  dsm_free_packet(&tx_ptr); 

  HSU_MSG_MED(
    "perform_control_transmit(): calling cdc_send_encap_response, len=%d, tx_ptr=%d (handle=%d)",
    context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes, 
    tx_ptr,
    context_ptr->handle);

#if defined(FEATURE_HSU_TEST) || defined(FEATURE_HSU_ECM_APP_STUB)
#error code not present
#endif /* defined(FEATURE_HSU_TEST) || defined(FEATURE_HSU_ECM_APP_STUB) */

  /* Send the buffer to the stack, to be sent on the control EP, to the host */
  /***************************************************************************/
  /* Lock Giant mutex */
  jsafe_enter();
  if(context_ptr->instance_is_enabled == FALSE)
  {
    rex_enter_crit_sect(&hsu_al_ecm_critical_section);
    context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 0;
    dsm_empty_queue(context_ptr->control_info.tx_queue_ptr);
    rex_leave_crit_sect(&hsu_al_ecm_critical_section);

    /* Release Giant mutex */
    jsafe_leave();

    HSU_MSG_HIGH(
      "perform_control_transmit(): Instance is disabled (handle=%d), Ignoring.",
      context_ptr->handle,0,0);
    return;
  }

  result = hsu_cdc_fd_api_send_encap_response(context_ptr->handle, 
    (void *)context_ptr->control_info.write_buffer_data.ctrl_tx_buffer,
    (juint32_t)context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes);

  if(result == ESUSPENDED)
  {
    /* 
    * The device is suspended (for example - the USB cable was disconnected). 
    * The handling is the same as if the TX was CANCELED. Meaning, just send 
    * the next control TX, if there is one 
    */
    /* The call to this must be performed when the giant mutex is locked, as
    would happen if the FD was the one calling this function. */
    hsu_al_ecm_encap_resp_comp_cb(CDC_TRANSFER_CANCELLED, 
      0,
      (cdc_appctx_t)(context_ptr));

    /* Release Giant mutex */
    jsafe_leave();

    HSU_MSG_HIGH(
      "perform_control_transmit(): hsu_cdc_fd_api_send_encap_response returned \
      ESUSPENDED. Cancelled current TX (handle=%d)",
      context_ptr->handle,0,0);
    return;
  }

  /* Release Giant mutex */
  jsafe_leave();

  if(result == JEBUSY)
  {
    HSU_MSG_ERROR(
      "perform_control_transmit(): Ctl Tx already in progress, \
      how is that possible?",
      0,0,0);
    HSU_MSG_ERROR(
      "  write_buffer_data.ctrl_tx_num_of_bytes=%d, tx_ptr=0x%x",
      context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes,tx_ptr,0);
    HSU_ASSERT(
      (uint32)"perform_control_transmit(): Ctl Tx already in progress, \
              how is that possible?" == NULL);
  }
  else if(result != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "perform_control_transmit(): cdc_send_encap_response failed with %d",
      result,0,0);
    HSU_ASSERT(
      (uint32)"perform_control_transmit(): cdc_send_encap_response failed" 
      == NULL);
  }
  /* Note: if one of the asserts is replaced with error handling, consider 
  adding the following section:     
  rex_enter_crit_sect(&hsu_al_ecm_critical_section);
  context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 0;
  rex_leave_crit_sect(&hsu_al_ecm_critical_section);
  to the callers' functions. 
  If we were to do this in this function, and there would be another message in 
  the WM Q, the app. context might take over and send ANOTHER control message, 
  which will go to the host BEFORE the one already queued. 
  */
} /* perform_control_transmit */

/*==============================================================================

FUNCTION      HSU_AL_ECM_ENCAP_RESP_COMP_CB

DESCRIPTION   
Called by the FD to notify of the completion of a request made through the 
function cdc_send_encap_response().
The sequence completed includes the "Response Available" notification and the 
"Get Encapsulated command" request. 

DEPENDENCIES  
ecm_init() and hsu_al_ecm_control_open() must have been called for this 
instance.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_encap_resp_comp_cb
(
 cdc_iostatus_t  status, 
 juint32_t       bytes_sent, 
 cdc_appctx_t    ctx
 )
{
  dsm_item_type * dsm_chain_ptr;
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  if(cdc_app_context_is_valid(ctx) == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_encap_resp_comp_cb(): ctx param invalid" 
      == NULL);
  }
  context_ptr = (hsu_al_ecm_instance_info_type *)ctx;

  switch(status)
  {
  case CDC_TRANSFER_ERROR:
    {
      /* We assume that a failure at this stage (asynchronous) is due to a non-fatal 
      error, such as: temporarily out of resources (TD), bus error, etc...
      and therefore we assume it is OK to send the next control message */
      HSU_MSG_ERROR(
        "hsu_al_ecm_encap_resp_comp_cb(): status=CDC_TRANSFER_ERROR, bytes_sent=%d",
        bytes_sent,0,0);
      break; /* code continues after the end of the switch */
    }
  case CDC_TRANSFER_CANCELLED:
    {
      /* 
      * The device is suspended (for example - the USB cable is disconnected). 
      * We should keep TXing as long as we did not get a disabled_cb.
      */
      HSU_MSG_HIGH(
        "hsu_al_ecm_encap_resp_comp_cb(): status=CANCELLED, bytes_sent=%d (handle=%d)",
        bytes_sent, context_ptr->handle, 0);
      break; /* code continues after the end of the switch */
    }
  case CDC_TRANSFER_OK:
    {
      HSU_MSG_HIGH("hsu_al_ecm_encap_resp_comp_cb():status=OK, bytes_sent=%d (handle=%d)",
        bytes_sent, context_ptr->handle, 0);
      break; /* code continues after the end of the switch */
    }
  default:
    {
      HSU_MSG_ERROR(
        "hsu_al_ecm_encap_resp_comp_cb(): unknown status=%d, bytes_sent=%d (handle=%d)",
        status, bytes_sent, context_ptr->handle);
      HSU_ASSERT(
        (uint32)"hsu_al_ecm_encap_resp_comp_cb(): unknown status" 
        == NULL);
    }
  } /* switch */

  /* no need to notify the application */

  /* 
  * the write_buffer_data structure is accessed both from the FD task's 
  * context and from the application context when hsu_al_ecm_control_transmit 
  * is called. therefore it is protected by a critical section. Note that we 
  * protect the ctrl_tx_num_of_bytes field, as a means to control access to the 
  * buffer itself as well. When the field is != 0, the structure cannot be 
  * changed in the application task's context (unless the transfer failed), but  
  * only in the FD task context, when the control transfer completes. 
  */

  /* Initiate the next control TX transmission if there is such pending */
  rex_enter_crit_sect(&hsu_al_ecm_critical_section);
  dsm_chain_ptr = dsm_dequeue(context_ptr->control_info.tx_queue_ptr);
  if(dsm_chain_ptr == NULL)
  {
    context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 0;
    rex_leave_crit_sect(&hsu_al_ecm_critical_section);
    return;
  }

  context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 
    dsm_length_packet(dsm_chain_ptr);
  rex_leave_crit_sect(&hsu_al_ecm_critical_section);

  HSU_MSG_MED(
    "hsu_al_ecm_encap_resp_comp_cb(): another CTRL TX, dsm_chain_ptr=0x%x (handle=%d)",
    dsm_chain_ptr, context_ptr->handle, 0);
  perform_control_transmit(context_ptr, dsm_chain_ptr);
} /*hsu_al_ecm_encap_resp_comp_cb*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_NOTIF_NET_CONN_COMP_CB

DESCRIPTION   
Called by the FD to notify of the completion of a 
cdc_notify_network_connection() (which is used to send the "Network 
Connection" notification. 
This is a mandatory notification, so a CB must be supplied, but this 
notification is not used by the AMSS, so its completion CB should never be 
called, and therefore causes an ASSERT.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_notif_net_conn_comp_cb
(
 cdc_iostatus_t  status, 
 cdc_appctx_t    ctx
 )
{
  if(cdc_app_context_is_valid(ctx) == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_notif_net_conn_comp_cb(): ctx param invalid" 
      == NULL);
  }

  HSU_USE_PARAM(status);

  HSU_ASSERT(
    (uint32)"hsu_al_ecm_notif_net_conn_comp_cb(): This notification isn't\
            used by the AMSS, how come its sending has completed?!" 
            == NULL);
} /*hsu_al_ecm_notif_net_conn_comp_cb*/

#ifdef FEATURE_HS_USB_TLP
/*==============================================================================

FUNCTION    hsu_al_ecm_enable_dl_tlp  

DESCRIPTION   
This function is called when (and if) a setup packet is received from the host 
that request to enable DL TLP (this setup packet is received as a result of 
sending an MDLM descriptor by the device)

NOTE:
TLP is enabled for all ecm instances! So when we add support for multiple ecm
instances we need to make sure that the following doesn't occur:
- The host enables TLP for ecm instance 1 and leaves TLP disabled for ecm
  instance 2.
- Since TLP will be enabled for all ecm instances, the device will work in
  TLP on both ecm instances but the host will work in TLP mode only with the 
  first ecm instance -> the second ecm instance will be nonfunctional!

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
void hsu_al_ecm_enable_dl_tlp(void)
{
  HSU_MSG_HIGH("*_enable_dl_tlp: Entered", 0, 0, 0);

  if(ecm_instance_info.state == HSU_ECM_INSTANCE_STATE_NONE)
  {
    HSU_MSG_ERROR("*_enable_dl_tlp: ECM instance was not initialized yet!",0,0,0);
    return;
  }
  ecm_instance_info.data_info.dl_tlp_enabled = TRUE;
  HSU_MSG_HIGH("*_enable_dl_tlp: Exited", 0, 0, 0);
}

/*==============================================================================

FUNCTION    hsu_al_ecm_enable_ul_tlp  

DESCRIPTION   
This function is called when (and if) a setup packet is received from the host 
that request to enable UL TLP (this setup packet is received as a result of 
sending an MDLM descriptor by the device)

NOTE:
TLP is enabled for all ecm instances! So when we add support for multiple ecm
instances we need to make sure that the following doesn't occur:
- The host enables TLP for ecm instance 1 and leaves TLP disabled for ecm
  instance 2.
- Since TLP will be enabled for all ecm instances, the device will work in
  TLP on both ecm instances but the host will work in TLP mode only with the 
  first ecm instance -> the second ecm instance will be nonfunctiona!

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
void hsu_al_ecm_enable_ul_tlp(void)
{
  HSU_MSG_HIGH("*_enable_ul_tlp: Entered", 0, 0, 0);

  if(ecm_instance_info.state == HSU_ECM_INSTANCE_STATE_NONE)
  {
    HSU_MSG_ERROR("*_enable_ul_tlp: ECM instance was not initialized yet!",0,0,0);
    return;
  }

  /* Update boolean indicating the host supports TLP */
  ecm_instance_info.data_info.ul_tlp_enabled = TRUE;
  HSU_MSG_HIGH("*_enable_ul_tlp: Exited", 0, 0, 0);
}
#endif /*FEATURE_HS_USB_TLP*/

#ifdef T_REXNT
#error code not present
#endif /*T_REXNT*/
/*==============================================================================

FUNCTION      HSU_AL_ECM_NOTIF_CONN_SPEED_CH_COMP_CB

DESCRIPTION   
Called by the FD to notify of the completion of a 
ecm_notify_conn_speed_change() (which is used to send the "Connection Speed 
Change" notification). 

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_notif_conn_speed_ch_comp_cb
(
 cdc_iostatus_t  status, 
 cdc_appctx_t    ctx
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  if(cdc_app_context_is_valid(ctx) == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_notif_conn_speed_ch_comp_cb(): ctx param\
              invalid" == NULL);
  }
  context_ptr = (hsu_al_ecm_instance_info_type *)(ctx);

  if(status != CDC_TRANSFER_OK)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_notif_conn_speed_ch_comp_cb(): failed with status=%d",
      status,0,0);
  }
  else
  {
    HSU_MSG_LOW("hsu_al_ecm_notif_conn_speed_ch_comp_cb(): CDC_TRANSFER_OK",
      0,0,0);
  }

  /* Check if there is another notification waiting to be sent.
  This function is called in HSUSB0 context so the mutex is locked */
  if (context_ptr->data_info.conn_speed_mem.resend)
  {
    HSU_MSG_LOW("hsu_al_ecm_notif_conn_speed_ch_comp_cb(): Resending notification. "
      "new_peed = %d",
      ecm_instance_info.data_info.conn_speed_mem.connection_speed_args.downstream_bitrate,
      0,0);
    ecm_instance_info.data_info.conn_speed_mem.resend = FALSE;
    ecm_instance_info.data_info.connection_speed_args = 
      ecm_instance_info.data_info.conn_speed_mem.connection_speed_args ;
    defer_call_to_notif_con_speed_ch(context_ptr);
  }
  else
  {
    context_ptr->data_info.conn_speed_mem.is_send_opr_in_progress = FALSE;
  }
#ifdef T_REXNT
#error code not present
#endif
} /*hsu_al_ecm_notif_conn_speed_ch_comp_cb*/

/*==============================================================================

FUNCTION      GET_DATA_READ_BUF_INFO_FROM_PTR

DESCRIPTION   
Find out which of the two buffers the instance holds, is the buffer whose
pointer is given. Returns the info structure for this buffer. 

DEPENDENCIES  
None.

RETURN VALUE    
The information structure of the given buffer.
NULL if there was no match (caller should consider ASSERTing)

SIDE EFFECTS  
None.

==============================================================================*/
static hsu_al_ecm_data_read_buffer_info_type * 
get_data_read_buf_info_from_ptr
(
 hsu_al_ecm_instance_info_type * context_ptr,
 uint8 *                         buffer_ptr
 )
{
  HSU_ASSERT(context_ptr != NULL);
  HSU_ASSERT(buffer_ptr != NULL);

  /* since the data_read_buffer is static, its address never changes, and 
  therefore this can be done without critical_section protection */
  if(context_ptr->data_info.read_buf_1_info.data_read_buffer == buffer_ptr)
  {
    return ( &(context_ptr->data_info.read_buf_1_info));
  }
  if(context_ptr->data_info.read_buf_2_info.data_read_buffer == buffer_ptr)
  {
    return (&(context_ptr->data_info.read_buf_2_info));
  }

  HSU_MSG_ERROR(
    "get_data_read_buf_info_from_ptr(): buffer_ptr=0x%x unknown. \
    handle=%d",
    buffer_ptr,context_ptr->handle,0);
  HSU_MSG_ERROR("  read_buf_1_info: state=%d, buffer=0x%04x",
    context_ptr->data_info.read_buf_1_info.data_read_state,
    context_ptr->data_info.read_buf_1_info.data_read_buffer, 0);
  HSU_MSG_ERROR("  read_buf_2_info: state=%d, buffer=0x%x",
    context_ptr->data_info.read_buf_2_info.data_read_state,
    context_ptr->data_info.read_buf_2_info.data_read_buffer, 0);

  return NULL; /* caller should ASSERT on this ! */
} /* get_data_read_buf_info_from_ptr */

/*==============================================================================

FUNCTION      HSU_AL_ECM_DMOV_SCATTER_COMP_CB

DESCRIPTION   
Called when the DMOV completes a scatter operation.
The function enqueues the DSM chain into the application's data RX WM Q, and 
then initiates the next cdc_read operation with the buffer that was scattered, 
and is now available for the next cdc_read.
Called in interrupt context!!! 

DEPENDENCIES  
None.

RETURN VALUE    
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_dmov_scatter_comp_cb
(
 boolean         success,      /* was DMOV scatter operation successful? */
 dsm_item_type * dsm_chain_head_ptr, /* DSM chain that was scattered */
 void *          buffer_ptr,   /* Buffer that was scattered */
 void *          user_data_ptr /* User data provided when operation was invoked */
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  hsu_al_ecm_data_read_buffer_info_type * read_buffer_info = NULL;

  HSU_ASSERT(dsm_chain_head_ptr != NULL);
  HSU_ASSERT(buffer_ptr != NULL);
  HSU_ASSERT(user_data_ptr != NULL);

  context_ptr = (hsu_al_ecm_instance_info_type *)(user_data_ptr);

  read_buffer_info = get_data_read_buf_info_from_ptr(
    context_ptr, 
    buffer_ptr);
  if(read_buffer_info == NULL)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_dmov_scatter_comp_cb(): unknown buffer" == NULL);
  }

  HSU_ASSERT(read_buffer_info->data_read_state == 
    HSU_ECM_BUFFER_STATE_PENDING_AT_DMOV);

  if(success == FALSE)
  {
    dsm_free_packet(&dsm_chain_head_ptr);
    read_buffer_info->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR(
      "hsu_al_ecm_dmov_scatter_comp_cb(): operation failed. handle=%d",
      context_ptr->handle,0,0);
    HSU_MSG_ERROR(
      "  dsm_chain_head_ptr=0x%x, buffer_ptr=0x%x",
      dsm_chain_head_ptr,buffer_ptr,0);
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_dmov_scatter_comp_cb(): DMOV was unsuccessful" 
      == NULL);
  }

  /* Enqueue DSM chain to the application's data RX WM Q */
  /*******************************************************/

  /* It is possible that a previous frame caused inbound flow to be disabled 
  while this frame was waiting for the DMOV. 
  In such a case, we release the DSM chain and do not pass over the received 
  buffer to the application. Save it as is, and pass it only when the inbound 
  flow is enabled. */
  if(context_ptr->data_info.inbound_flow_is_enabled == FALSE)
  {
    HSU_MSG_MED(
      "handle_read_complete(): Received a buffer, but inbound flow is disabled.\
      Saving it for when it's enabled (handle=%d)",
      context_ptr->handle,0,0);
    read_buffer_info->data_read_state = HSU_ECM_BUFFER_STATE_READY_FOR_DMOV;
    dsm_free_packet(&dsm_chain_head_ptr);
    return;
  }

  if(context_ptr->data_info.rx_func_ptr)
  {
    context_ptr->data_info.rx_func_ptr(&dsm_chain_head_ptr);
  }
  else if(context_ptr->data_info.rx_queue_ptr)
  {
    dsm_enqueue(context_ptr->data_info.rx_queue_ptr, &dsm_chain_head_ptr);
  }
  else
  {
    HSU_ASSERT((char*)"hsu_al_ecm_dmov_scatter_comp_cb(): Neither RX WM nor "
               "rx_func_ptr is defined" == NULL);
  }
  /* This might cause the hiwater_func() to be called SYNCHRONOUSLY!!! 
  This is why we make the same check twice - the first time is in case a previous 
  chain caused the inbound flow to be disabled, the second is if this current 
  chain caused it. */

  if(context_ptr->data_info.inbound_flow_is_enabled == FALSE)
  {
    /* the dsm_enqueue caused the hiwater_func to be called, which disabled 
    inbound flow. 
    DO NOT give another buffer to the stack using cdc_read, since we don't 
    want to handle anymore incoming data */
    HSU_MSG_MED(
      "hsu_al_ecm_dmov_scatter_comp_cb(): NOT calling cdc_read, since inbound\
      flow is disabled (handle=%d)",
      context_ptr->handle,0,0);

    read_buffer_info->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;
    return;
  }

  /* Use the buffer for a new cdc_read operation */
  /***********************************************/  

  /* 
  * Verify that the instance is not disabled. If it is disabled, do not issue
  * a new cdc_read 
  */
  if(context_ptr->instance_is_enabled == FALSE)
  {
    read_buffer_info->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;
    HSU_MSG_MED(
      "hsu_al_ecm_dmov_scatter_comp_cb(): NOT calling cdc_read, since instance\
      is disabled (handle=%d)",
      context_ptr->handle,0,0);
    return;
  }

  read_buffer_info->data_read_state = HSU_ECM_BUFFER_STATE_PENDING_AT_STACK;

  /* 
  * We're in interrupt context, and therefore cannot call cdc_read since this 
  * requires locking the Giant mutex. Therefore, we will use the AL task to 
  * get a non interrupt context, and lock the giant mutex and call cdc_read 
  * from the function , which will be called in the AL task's context. 
  */
  hsu_al_task_enqueue_cmd(
    defer_call_to_cdc_read,
    (void*)read_buffer_info);
} /* hsu_al_ecm_dmov_scatter_comp_cb */

#ifdef FEATURE_HS_USB_TLP
/*==============================================================================

FUNCTION      HSU_AL_ECM_FREE_DSM_CHAINS_ARRAY

DESCRIPTION   
This function receives an array holding pointers to dsm chains and frees them

DEPENDENCIES  
None.

RETURN VALUE    
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_free_dsm_chains_array
(
 dsm_item_type **dsm_chains_array_for_tlp_scatter,
 uint32          arr_size
 )
{
  uint32  indx = 0;
  if (dsm_chains_array_for_tlp_scatter == NULL)
  {
    HSU_MSG_ERROR("*_ecm_free_dsm_chains_array: invalid param!",0,0,0);
    HSU_ASSERT((uint32)"*_ecm_free_dsm_chains_array: invalid param!" == NULL);
    return;
  }
  while((indx < arr_size) && dsm_chains_array_for_tlp_scatter[indx] != NULL)
  {
    dsm_free_packet(&dsm_chains_array_for_tlp_scatter[indx++]);
  }
}

/*==============================================================================

FUNCTION      HSU_AL_ECM_DMOV_TLP_SCATTER_COMP_CB

DESCRIPTION   
Called when the DMOV completes a scatter operation.
The function enqueues the DSM chains into the application's data RX WM Q, and 
then initiates the next cdc_read operation with the buffer that was scattered, 
and is now available for the next cdc_read.
Called in interrupt context!!! 

DEPENDENCIES  
None.

RETURN VALUE    
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_dmov_tlp_scatter_comp_cb
(
 boolean         success,      /* was DMOV scatter operation successful? */
 dsm_watermark_type *src_queue_to_gather, /*Ignored! used for gathering*/
 void* buffer_ptr,            /*scattered buffer*/
 uint32 num_bytes_copied,    /*Ignored! used for gathering*/
 packet_size_and_addr_in_dest_buff_pair_type* packet_sizes_array, 
                             /*Ignored! used for gathering*/
 dsm_item_type **dsm_chains_array_for_tlp_scatter,
        /*ptr to the array holding the scattered dsm chains*/
 void* user_data_ptr
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  hsu_al_ecm_data_read_buffer_info_type * read_buffer_info = NULL;

  uint32 dsm_chains_array_indx = 0;

  HSU_ASSERT(dsm_chains_array_for_tlp_scatter != NULL);
  HSU_ASSERT(buffer_ptr != NULL);
  HSU_ASSERT(user_data_ptr != NULL);

  HSU_USE_PARAM(src_queue_to_gather);
  HSU_USE_PARAM(num_bytes_copied);
  HSU_USE_PARAM(packet_sizes_array);

  context_ptr = (hsu_al_ecm_instance_info_type *)(user_data_ptr);

  read_buffer_info = get_data_read_buf_info_from_ptr(
    context_ptr, 
    buffer_ptr);
  if(read_buffer_info == NULL)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_dmov_scatter_comp_cb(): unknown buffer" == NULL);
    return;
  }

  HSU_ASSERT(read_buffer_info->data_read_state == 
    HSU_ECM_BUFFER_STATE_PENDING_AT_DMOV);

  if(success == FALSE)
  {
    hsu_al_ecm_free_dsm_chains_array(dsm_chains_array_for_tlp_scatter,HSU_AL_ECM_PACKED_SIZES_ARRAY_SIZE);  
    read_buffer_info->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR(
      "hsu_al_ecm_dmov_scatter_comp_cb(): operation failed. handle=%d",
      context_ptr->handle,0,0);
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_dmov_scatter_comp_cb(): DMOV was unsuccessful" 
      == NULL);
  }

  /* Enqueue DSM chains to the application's data RX WM Q */
  /*******************************************************/

  /* It is possible that a previous buffer caused inbound flow to be disabled 
  while this buffer was waiting for the DMOV. 
  In such a case, we release all the DSM chains and do not pass over the received 
  buffer to the application. Save it as is, and pass it only when the inbound 
  flow is enabled. */
  if(context_ptr->data_info.inbound_flow_is_enabled == FALSE)
  {
    HSU_MSG_MED(
      "handle_read_complete(): Received a buffer, but inbound flow is disabled.\
      Saving it for when it's enabled (handle=%d)",
      context_ptr->handle,0,0);

    hsu_al_ecm_free_dsm_chains_array(dsm_chains_array_for_tlp_scatter,HSU_AL_ECM_PACKED_SIZES_ARRAY_SIZE);    
    read_buffer_info->data_read_state = HSU_ECM_BUFFER_STATE_READY_FOR_DMOV;
    return;
  }

  while (read_buffer_info->dsm_chains_array_for_tlp_scatter[dsm_chains_array_indx] != NULL)
  {
    if(context_ptr->data_info.rx_func_ptr)
    {
      context_ptr->data_info.rx_func_ptr(&read_buffer_info->dsm_chains_array_for_tlp_scatter[dsm_chains_array_indx++]);
    }
    else if(context_ptr->data_info.rx_queue_ptr)
    {
      dsm_enqueue(context_ptr->data_info.rx_queue_ptr, 
        &read_buffer_info->dsm_chains_array_for_tlp_scatter[dsm_chains_array_indx++]);
    }
    else
    {
      HSU_ASSERT((char*)"hsu_al_ecm_dmov_tlp_scatter_comp_cb(): Neither RX WM"
               " nor rx_func_ptr is defined" == NULL);
    }
  }

  if(context_ptr->data_info.inbound_flow_is_enabled == FALSE)
  {
    /* the dsm_enqueue caused the hiwater_func to be called, which disabled 
    inbound flow. 
    DO NOT give another buffer to the stack using cdc_read, since we don't 
    want to handle anymore incoming data */
    HSU_MSG_MED(
      "hsu_al_ecm_dmov_scatter_comp_cb(): NOT calling cdc_read, since inbound\
      flow is disabled (handle=%d)",
      context_ptr->handle,0,0);

    read_buffer_info->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;
    return;
  }

  /* Use the buffer for a new cdc_read operation */
  /***********************************************/  

  /* 
  * Verify that the instance is not disabled. If it is disabled, do not issue
  * a new cdc_read 
  */
  if(context_ptr->instance_is_enabled == FALSE)
  {
    read_buffer_info->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;
    HSU_MSG_MED(
      "hsu_al_ecm_dmov_scatter_comp_cb(): NOT calling cdc_read, since instance\
      is disabled (handle=%d)",
      context_ptr->handle,0,0);
    return;
  }

  read_buffer_info->data_read_state = HSU_ECM_BUFFER_STATE_PENDING_AT_STACK;

  /* 
  * We're in interrupt context, and therefore cannot call cdc_read since this 
  * requires locking the Giant mutex. Therefore, we will use the AL task to 
  * get a non interrupt context, and lock the giant mutex and call cdc_read 
  * from the function , which will be called in the AL task's context. 
  */
  hsu_al_task_enqueue_cmd(
    defer_call_to_cdc_read,
    (void*)read_buffer_info);
} /* hsu_al_ecm_dmov_scatter_comp_cb */
#endif /*FEATURE_HS_USB_TLP*/

/*==============================================================================

FUNCTION      HANDLE_READ_COMPLETE

DESCRIPTION   
Called from hsu_al_ecm_read_comp_cb (FD's context) for successful 
OUT transfer completions.

Can also be called following inbound flow enabling (from the function
hsu_al_ecm_ioctl, command SIO_IOCTL_INBOUND_FLOW_ENABLE).
In this case, it is called in the application's context.

Scatters the read buffer into a DSM chain using the DMOV. 
The rest of the "work" is done in the completion CB supplied to the DMOV.

DEPENDENCIES  
Called following a successful cdc_read operation.

RETURN VALUE    
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void handle_read_complete
(
 hsu_al_ecm_instance_info_type *         context_ptr, 
 hsu_al_ecm_data_read_buffer_info_type * read_comp_buf_data_ptr
 )
{
  dsm_item_type * dsm_chain_head_ptr = NULL;
  boolean ret_val;

  HSU_ASSERT(context_ptr != NULL);
  HSU_ASSERT(read_comp_buf_data_ptr != NULL);
  HSU_ASSERT(read_comp_buf_data_ptr->data_read_num_bytes != 0);

  /* 1. data_read_state = HSU_ECM_BUFFER_STATE_READY_FOR_DMOV means that
        this function was called because of inbound flow enable
     2. data_read_state = HSU_ECM_BUFFER_STATE_PENDING_AT_STACK means that 
        this function was called by the stack
  */
  HSU_ASSERT((read_comp_buf_data_ptr->data_read_state == 
    HSU_ECM_BUFFER_STATE_PENDING_AT_STACK) ||
    (read_comp_buf_data_ptr->data_read_state == 
    HSU_ECM_BUFFER_STATE_READY_FOR_DMOV));

  if(context_ptr->data_info.inbound_flow_is_enabled == FALSE)
  {
    HSU_MSG_MED(
      "handle_read_complete(): Received a buffer, but inbound flow is disabled.\
      Saving it for when it's enabled (handle=%d)",
      context_ptr->handle,0,0);
    /* Do not pass over the received buffer to the application. 
    Save it as is, and pass it only when the inbound flow is enabled. */
    read_comp_buf_data_ptr->data_read_state = HSU_ECM_BUFFER_STATE_READY_FOR_DMOV;
    return ;
  }

  /* Initialize a DMOV scatter operation */
  /***************************************/  
  /* Don't want to flood DIAG... 
  HSU_MSG_LOW(
  "handle_read_complete(): Calling DMOV scatter with dsm_chain=0x%x, size=%d (handle=%d)",
  dsm_chain_head_ptr, 
  read_comp_buf_data_ptr->data_read_num_bytes, 
  context_ptr->handle); */

  read_comp_buf_data_ptr->data_read_state = HSU_ECM_BUFFER_STATE_PENDING_AT_DMOV;

#ifdef FEATURE_HS_USB_TLP
  if (context_ptr->data_info.ul_tlp_enabled)
  {
    uint16 max_segment_size = HSU_ECM_MAX_ETHERNET_FRAME_LENGTH ;
#ifdef FEATURE_DATA_QMI_QOS
    max_segment_size += HSU_AL_ECM_QOS_HEADER_SIZE ;
#endif
    ret_val = hsu_al_dmov_api_scatter_dsm_chain_for_tlp(
      (void *)(read_comp_buf_data_ptr->data_read_buffer),
      read_comp_buf_data_ptr->data_read_num_bytes,
      hsu_al_ecm_dmov_tlp_scatter_comp_cb,
      context_ptr,
      (dsm_item_type**)(read_comp_buf_data_ptr->dsm_chains_array_for_tlp_scatter),
      HSU_AL_ECM_PACKED_SIZES_ARRAY_SIZE,
      TLP_HEADER_SIZE,
      HSU_ECM_AL_DSM_POOL_ID, max_segment_size);
  }
  else
#endif /*FEATURE_HS_USB_TLP*/
  {
    /* Allocate a DSM chain long enough to scatter the entire received buffer */ 
    /**************************************************************************/  
    dsm_chain_head_ptr = hsu_al_dsm_utils_generate_dsm_chain(
      HSU_ECM_AL_DSM_POOL_ID, 
      read_comp_buf_data_ptr->data_read_num_bytes);  
    if(dsm_chain_head_ptr == NULL)
    {
      HSU_MSG_ERROR("handle_read_complete(): Failed allocating large DSM items. "
        "Falling back to small DSM items",0 ,0 ,0);
      dsm_chain_head_ptr = hsu_al_dsm_utils_generate_dsm_chain(
        DSM_DS_SMALL_ITEM_POOL, 
        read_comp_buf_data_ptr->data_read_num_bytes);  
      if(dsm_chain_head_ptr == NULL)
      {
        if (!hsu_al_ecm_read_comp_err_handle(context_ptr,read_comp_buf_data_ptr))
        {
          HSU_MSG_ERROR("handle_read_complete(): "
            "failed allocating both large and small DSM items and returning the buffer to the stack.",
                        0, 0, 0);
        }
        else
        {
          HSU_MSG_ERROR("hsu_al_ecm_encap_command_cb(): "
            "failed allocating both large and small DSM items", 0, 0, 0);
        }
        return;
      }
    }
    ret_val = hsu_al_dmov_api_scatter_dsm_chain(
      (void *)(read_comp_buf_data_ptr->data_read_buffer),
      read_comp_buf_data_ptr->data_read_num_bytes,
      dsm_chain_head_ptr,
      hsu_al_ecm_dmov_scatter_comp_cb,
      context_ptr);
  }

  if(ret_val == FALSE)
  {
    dsm_free_packet(&dsm_chain_head_ptr);
    read_comp_buf_data_ptr->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR(
      "handle_read_complete(): DMOV scatter failed. handle=%d, buffer=0x%x,\
      size=%d",
      context_ptr->handle, 
      read_comp_buf_data_ptr->data_read_buffer, 
      read_comp_buf_data_ptr->data_read_num_bytes);
    (void)hsu_al_ecm_read_comp_err_handle(context_ptr,read_comp_buf_data_ptr);
  }
} /* handle_read_complete */ 

/*==============================================================================

FUNCTION      HSU_AL_ECM_READ_COMP_ERR_HANDLE

DESCRIPTION   
Called from hsu_al_ecm_read_comp_cb in case of transfer error (status or 
incomplete transfer) or when DSM items ran out. Returns the buffer to the 
stack by issuing another cdc_read

DEPENDENCIES  
None

RETURN VALUE    
TRUE/FALSE.

SIDE EFFECTS  
None.

==============================================================================*/
static boolean hsu_al_ecm_read_comp_err_handle
(
 hsu_al_ecm_instance_info_type *         context_ptr,
 hsu_al_ecm_data_read_buffer_info_type * read_comp_buf_data_ptr
)
{
  jresult_t jresult;
  /* We assume that the correctnes of the input params was already checked in
     hsu_al_ecm_read_comp_cb so here we only check against NULL value
   */
  HSU_ASSERT(context_ptr != NULL);
  HSU_ASSERT(read_comp_buf_data_ptr != NULL);
  /* 
  * Verify that the instance is not disabled. If it is disabled, do not issue
  * a new cdc_read 
  */
  if(context_ptr->instance_is_enabled == FALSE)
  {
    read_comp_buf_data_ptr->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;
    HSU_MSG_MED(
      "hsu_al_ecm_read_comp_err_handle(): NOT calling cdc_read, since instance\
      is disabled (handle=%d)",
      context_ptr->handle,0,0);
    return FALSE;
  }

  /*
  * If inbound flow is disabled don't issue cdc_read
  * We need to protect this with a critical section because this code is
  * running in Jungo context. Context switch might occure and RmNet application
  * can hit low wm on the rx_wmq -> thus enable inbound flow.
  */
  rex_enter_crit_sect(&hsu_al_ecm_critical_section);
  if (!context_ptr->data_info.inbound_flow_is_enabled)
  {
    read_comp_buf_data_ptr->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;
    HSU_MSG_MED(
      "hsu_al_ecm_read_comp_err_handle(): NOT calling cdc_read, since inbound flow\
      is disabled (handle=%d)", context_ptr->handle,0,0);
    rex_leave_crit_sect(&hsu_al_ecm_critical_section);
    return FALSE;
  }
  rex_leave_crit_sect(&hsu_al_ecm_critical_section);

  read_comp_buf_data_ptr->data_read_state = HSU_ECM_BUFFER_STATE_PENDING_AT_STACK;
  /* We are in stack context, no need to lock mutex */
  jresult = cdc_read(context_ptr->handle, 
    read_comp_buf_data_ptr->data_read_buffer, 
    HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES);

  if(jresult != HSU_JSUCCESS)
  {
    read_comp_buf_data_ptr->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;
    HSU_MSG_ERROR(
      "hsu_al_ecm_read_comp_err_handle(): hsu_cdc_fd_api_cdc_read returned status= 0x%x",
      jresult,0,0);
    return FALSE;
  }
  return TRUE;
}
/*==============================================================================

FUNCTION      HSU_AL_ECM_READ_COMP_CB

DESCRIPTION   
Called by the FD to notify of the completion of a read operation. 
If the read was successful, calls handle_read_complete() which scatters 
the buffer into a DSM chain, enqueues it to the application's data RX WM Q, 
and initiates another cdc_read operation.

DEPENDENCIES  
Called following a cdc_read operation.

RETURN VALUE    
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_read_comp_cb
(
 cdc_iostatus_t  status, 
 void *          buffer_ptr,
 juint32_t       size, 
 jbool_t         transfer_complete, 
 cdc_appctx_t    ctx
 )
{
  hsu_al_ecm_instance_info_type *         context_ptr = NULL;
  hsu_al_ecm_data_read_buffer_info_type * read_comp_buf_data_ptr = NULL;

  HSU_ASSERT(buffer_ptr != NULL);

  HSU_PARAM_COULD_BE_CONST(buffer_ptr);

  if(cdc_app_context_is_valid(ctx) == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_read_comp_cb(): ctx param invalid" == NULL);
  }

  context_ptr = (hsu_al_ecm_instance_info_type *)ctx;

  /* Find out which of the two buffers this CB is intended for */
  /*************************************************************/
  read_comp_buf_data_ptr = get_data_read_buf_info_from_ptr(
    context_ptr,
    buffer_ptr);
  if(read_comp_buf_data_ptr == NULL)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_read_comp_cb(): called with unknown buffer_ptr=0x%x, \
      handle=%d, xfer_comp=%d",
      buffer_ptr,context_ptr->handle,transfer_complete);
    HSU_MSG_ERROR("  read_buf_1_info: state=%d, buffer=0x%04x",
      context_ptr->data_info.read_buf_1_info.data_read_state,
      context_ptr->data_info.read_buf_1_info.data_read_buffer, 0);
    HSU_MSG_ERROR("  read_buf_2_info: state=%d, buffer=0x%x",
      context_ptr->data_info.read_buf_2_info.data_read_state,
      context_ptr->data_info.read_buf_2_info.data_read_buffer, 0);
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_read_comp_cb(): called with unknown buffer_ptr" 
      == NULL);
  }  

  HSU_ASSERT(read_comp_buf_data_ptr->data_read_state == 
    HSU_ECM_BUFFER_STATE_PENDING_AT_STACK);

  read_comp_buf_data_ptr->data_read_num_bytes = size;

  /* Handle according to the status given in the CB */
  /**************************************************/
  switch(status) 
  {
  case CDC_TRANSFER_CANCELLED:
    {
      /* This can happen in 2 cases:
      * 1. Following a call to cdc_abort_read(). This is during a close 
      *    operation ,and should cause us to go back to IDLE state. cdc_read
      *    is not expected to be called again without another call to init/open.
      * 2. When the device becomes suspended. In this case we should not call
      *    cdc_read again until the device is resumed. 
      */ 
      /* sio_close() is not yet supported, so this obviously the second case... */
      read_comp_buf_data_ptr->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;
      HSU_MSG_HIGH(
        "hsu_al_ecm_read_comp_cb():status=CANCELED, buf=0x%x, size=%d (handle=%d)",
        read_comp_buf_data_ptr->data_read_buffer, 
        read_comp_buf_data_ptr->data_read_num_bytes, 
        context_ptr->handle);
      break;
    }
  case CDC_TRANSFER_ERROR:
    {
      HSU_MSG_ERROR(
        "hsu_al_ecm_read_comp_cb(): status=ERROR, buf=0x%x, size=%d (handle=%d)",
        read_comp_buf_data_ptr->data_read_buffer, 
        read_comp_buf_data_ptr->data_read_num_bytes, 
        context_ptr->handle);
      HSU_MSG_ERROR(" xfer_comp=%d", 
        transfer_complete,0,0);

      if (!hsu_al_ecm_read_comp_err_handle(context_ptr,read_comp_buf_data_ptr))
      {
        return;
      }
      break;
    }
  case CDC_TRANSFER_OK:
    {
      if (!size)
      {
        read_comp_buf_data_ptr->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;
        HSU_MSG_ERROR(
          "hsu_al_ecm_read_comp_cb(): size = 0",0,0,0);
        if (!hsu_al_ecm_read_comp_err_handle(context_ptr,read_comp_buf_data_ptr))
        {
          return;
        }
        break;
      }
      if(context_ptr->instance_is_enabled == FALSE)
      {
        HSU_MSG_ERROR(
          "hsu_al_ecm_read_comp_cb(): instance is disabled! (handle=%d)",
          context_ptr->handle,0,0);
        HSU_ASSERT(
          (char *)"hsu_al_ecm_read_comp_cb(): instance is disabled!" == NULL);
      }
      if(transfer_complete != TRUE)
      {
        /* This should not happen, since the buffer we give to the stack are 
        of the size of the largest possible Ethernet frame. therefore a buffer's
        reading should completed with the read completion of a single Ethernet 
        frame. */ 
        read_comp_buf_data_ptr->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;

        HSU_MSG_ERROR(
          "hsu_al_ecm_read_comp_cb(): RX not completed! (status = OK) buf=0x%x, size=%d (handle=%d)",
          read_comp_buf_data_ptr->data_read_buffer, 
          read_comp_buf_data_ptr->data_read_num_bytes, 
          context_ptr->handle);
        if (!hsu_al_ecm_read_comp_err_handle(context_ptr,read_comp_buf_data_ptr))
        {
          return;
        }
      }
      else
      {
        handle_read_complete(context_ptr, read_comp_buf_data_ptr);
      }
      break;
    }
  default:
    {
      read_comp_buf_data_ptr->data_read_state = HSU_ECM_BUFFER_STATE_IDLE;

      HSU_MSG_ERROR(
        "hsu_al_ecm_read_comp_cb(): unknown status=%d, buf=0x%x, size=%d",
        status, read_comp_buf_data_ptr->data_read_buffer, size);
      HSU_MSG_ERROR(" handle=%d, xfer_comp=%d", 
        context_ptr->handle,transfer_complete,0);
      HSU_ASSERT(
        (uint32)"hsu_al_ecm_read_comp_cb() called with unknown status!" 
        == NULL);
    }
  } /* switch(status) */
} /*hsu_al_ecm_read_comp_cb*/


/*==============================================================================

FUNCTION      HSU_AL_ECM_DMOV_GATHER_COMP_CB

DESCRIPTION   
Called when the DMOV completes a gather operation.
The function gives the buffer to the cdc_write() API of the USB stack.
Called in interrupt context!!! 

DEPENDENCIES  
None.

RETURN VALUE    
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_dmov_gather_comp_cb
(
 boolean         success,     /* was DMOV scatter operation successful? */
 dsm_item_type * dsm_chain_head_ptr, /* DSM chain that was gathered */
 void *          buffer_ptr,  /* Buffer that includes the data */
 void *          user_data_ptr/* User data provided when operation was invoked */
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  HSU_ASSERT(dsm_chain_head_ptr != NULL);
  HSU_ASSERT(buffer_ptr != NULL);
  HSU_ASSERT(user_data_ptr != NULL);

  context_ptr = (hsu_al_ecm_instance_info_type *)(user_data_ptr);

  dsm_free_packet(&dsm_chain_head_ptr);

  /* 
  * the write_buffer_data structure is accessed in several contexts:
  * - Application context:  hsu_al_ecm_data_transmit()
  * - Interrupt context:    hsu_al_ecm_dmov_gather_comp_cb()
  * - FD context:           hsu_al_ecm_write_comp_cb()
  *
  * Therefore it is protected by a critical section. Note that we 
  * protect the data_tx_num_of_bytes field, as a means to control access to the 
  * buffer itself as well. When the field is != 0, the structure cannot be 
  * changed in the application's context (unless the transfer failed), but only 
  * in the FD task context, when the transfer completes. 
  */
  HSU_ASSERT(buffer_ptr == 
    context_ptr->data_info.write_buffer_data.data_tx_buffer);
  HSU_ASSERT(context_ptr->data_info.write_buffer_data.data_tx_buf_state == 
    HSU_ECM_BUFFER_STATE_PENDING_AT_DMOV);

  if(success == FALSE)
  {
    context_ptr->data_info.write_buffer_data.data_tx_buf_num_of_bytes = 0;
    context_ptr->data_info.write_buffer_data.data_tx_buf_state = 
      HSU_ECM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR(
      "hsu_al_ecm_dmov_gather_comp_cb(): operation failed. handle=%d",
      context_ptr->handle,0,0);
    HSU_MSG_ERROR(
      "  dsm_chain_head_ptr=0x%x, buffer_ptr=0x%x",
      dsm_chain_head_ptr,buffer_ptr,0);
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_dmov_gather_comp_cb(): DMOV was unsuccessful" 
      == NULL);
  }

  /* Send the buffer down the stack */
  /**********************************/  
  context_ptr->data_info.write_buffer_data.data_tx_buf_state = 
    HSU_ECM_BUFFER_STATE_PENDING_AT_STACK;

  /* 
  * We're in interrupt context, and therefore cannot call cdc_write since this 
  * requires locking the Giant mutex. Therefore, we will use the AL task to 
  * get a non interrupt context, and lock the giant mutex and call cdc_write 
  * from the function , which will be called in the AL task's context. 
  */
  hsu_al_task_enqueue_cmd(
    defer_call_to_cdc_write,
    (void*)(&context_ptr->data_info.write_buffer_data));
} /* hsu_al_ecm_dmov_gather_comp_cb */

#ifdef FEATURE_HS_USB_TLP
/*==============================================================================

FUNCTION      HSU_AL_ECM_UPDATE_PACKET_SIZE_CLEANUP_AND_SEND

DESCRIPTION  
This is a helping function for implementing the TLP protocol.
Called after the DMOV completes a gather operation.
The function goes over the buffer and updates packet size for each 
Ethernet packet (= dsm chain) that we gathered into the buffer. 
It also frees all the dsm chains that we gathered into the buffer and are about 
to send.
After updating the buffer give it to USB stack by issuing issue cdc_write.
Called in the AL task context.

DEPENDENCIES  
None.

RETURN VALUE    
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_update_packet_size_cleanup_and_send(void * args)
{
  hsu_al_ecm_data_write_buffer_data_type * cdc_write_args = NULL;
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  uint32 buffer_ptr = NULL;
  uint8 chain_num = 0;
  uint32 orig_chain_size = 0;

  HSU_ASSERT(args != NULL);
  cdc_write_args = (hsu_al_ecm_data_write_buffer_data_type *)args;
  context_ptr = cdc_write_args->instance_ptr;
  HSU_ASSERT(context_ptr != NULL);

  buffer_ptr = (uint32)cdc_write_args->data_tx_buffer;
  while (cdc_write_args->packet_sizes_array[chain_num].chain_size > 0) {
    orig_chain_size = cdc_write_args->packet_sizes_array[chain_num].chain_size;
    cdc_write_args->packet_sizes_array[chain_num].chain_size |= TLP_SYNC_BITS;
    dsm_free_packet(&cdc_write_args->packet_sizes_array[chain_num].chain_ptr);
    memcpy((void*)buffer_ptr,(void *)(&cdc_write_args->packet_sizes_array[chain_num].chain_size),TLP_HEADER_SIZE);  
    
    buffer_ptr += orig_chain_size + TLP_HEADER_SIZE ;
    chain_num++;
  }
  
  /* Send the buffer down the stack */
  /**********************************/  
  context_ptr->data_info.write_buffer_data.data_tx_buf_state = 
    HSU_ECM_BUFFER_STATE_PENDING_AT_STACK;

  /*We don't really need to call defer_call_to_cdc_write() since we are already 
    in AL task context. We do so in order to prevent code duplication */
  defer_call_to_cdc_write((void*)(&context_ptr->data_info.write_buffer_data));
}

/*==============================================================================

FUNCTION      HSU_AL_ECM_DMOV_TLP_GATHER_COMP_CB

DESCRIPTION  
TODO: update
Called when the DMOV completes a gather operation.
The function gives the buffer to the cdc_write() API of the USB stack.
Called in interrupt context!!! 

DEPENDENCIES  
None.

RETURN VALUE    
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_dmov_tlp_gather_comp_cb
(
 boolean         success,     /* was DMOV gather operation successful? */
 dsm_watermark_type * dsm_wmq_ptr, /* DSM chain that was gathered */
 void *          buffer_ptr,  /* Buffer that includes the data */
 uint32          num_bytes_copied,
 packet_size_and_addr_in_dest_buff_pair_type* packet_sizes_array,
 dsm_item_type **dsm_chains_array_for_tlp_scatter, /*Ignored! used for scattering*/
 void *          user_data_ptr/* User data provided when operation was invoked */
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  HSU_ASSERT(dsm_wmq_ptr != NULL);
  HSU_ASSERT(buffer_ptr != NULL);
  HSU_ASSERT(user_data_ptr != NULL);
  HSU_ASSERT(packet_sizes_array != NULL);
  HSU_USE_PARAM(dsm_chains_array_for_tlp_scatter);
  context_ptr = (hsu_al_ecm_instance_info_type *)(user_data_ptr);

  context_ptr->data_info.write_buffer_data.data_tx_buf_num_of_bytes = 
      num_bytes_copied;

  /* 
  * the write_buffer_data structure is accessed in several contexts:
  * - Application context:  hsu_al_ecm_data_transmit()
  * - Interrupt context:    hsu_al_ecm_dmov_gather_comp_cb()
  * - FD context:           hsu_al_ecm_write_comp_cb()
  *
  * Therefore it is protected by a critical section. Note that we 
  * protect the data_tx_num_of_bytes field, as a means to control access to the 
  * buffer itself as well. When the field is != 0, the structure cannot be 
  * changed in the application's context (unless the transfer failed), but only 
  * in the FD task context, when the transfer completes. 
  */
  HSU_ASSERT(buffer_ptr == 
    context_ptr->data_info.write_buffer_data.data_tx_buffer);
  HSU_ASSERT(context_ptr->data_info.write_buffer_data.data_tx_buf_state == 
    HSU_ECM_BUFFER_STATE_PENDING_AT_DMOV);

  if(success == FALSE)
  {
    context_ptr->data_info.write_buffer_data.data_tx_buf_num_of_bytes = 0;
    context_ptr->data_info.write_buffer_data.data_tx_buf_state = 
      HSU_ECM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR(
      "hsu_al_ecm_dmov_gather_comp_cb(): operation failed. handle=%d",
      context_ptr->handle,0,0);
    HSU_MSG_ERROR(
      "  dsm_wmq_ptr=0x%x, buffer_ptr=0x%x",
      dsm_wmq_ptr,buffer_ptr,0);
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_dmov_gather_comp_cb(): DMOV was unsuccessful" 
      == NULL);
  }

  /* Before calling cdc_write we need to update the size of the packets
  * we are about to send. Since we are in interrupt context, we don't want
  * to do it here.
  */
  hsu_al_task_enqueue_cmd(
    hsu_al_ecm_update_packet_size_cleanup_and_send,
    (void*)(&context_ptr->data_info.write_buffer_data));
} /* hsu_al_ecm_dmov_gather_comp_cb */
#endif /*FEATURE_HS_USB_TLP*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_WRITE_COMPLETE_CB

DESCRIPTION   
Called by the FD to notify of the completion of a write operation. 
The function checks if there's another DSM chain in the data TX WM Q, waiting
to be sent. If so, it initiates its sending.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_write_comp_cb
(
 cdc_iostatus_t  status, 
 void *          buffer_ptr,
 cdc_appctx_t    ctx
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  dsm_item_type * dsm_chain_ptr = NULL;
  boolean ret_val;

  HSU_ASSERT(buffer_ptr != NULL);

  if(cdc_app_context_is_valid(ctx) == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_write_comp_cb(): ctx param invalid" == NULL);
  }
  context_ptr = (hsu_al_ecm_instance_info_type *)ctx; 

  HSU_ASSERT(buffer_ptr == 
    context_ptr->data_info.write_buffer_data.data_tx_buffer);

  HSU_ASSERT(context_ptr->data_info.write_buffer_data.data_tx_buf_state ==
    HSU_ECM_BUFFER_STATE_PENDING_AT_STACK);

  HSU_PARAM_COULD_BE_CONST(buffer_ptr);

  switch(status)
  {
  case CDC_TRANSFER_ERROR:
    {
      /* If a transfer error occurred we ignore it and try to retransmit again*/
      context_ptr->data_info.write_buffer_data.data_tx_buf_num_of_bytes = 0;
      context_ptr->data_info.write_buffer_data.data_tx_buf_state =
        HSU_ECM_BUFFER_STATE_IDLE;

      HSU_MSG_ERROR(
        "hsu_al_ecm_write_comp_cb(): status=ERROR, buf=0x%x (handle=%d)",
        buffer_ptr,context_ptr->handle,0);
      break;
    }
  case CDC_TRANSFER_CANCELLED:
    {
      /* This can happen in 2 cases:
      * 1. Following a call to cdc_abort_write(). This is during a close 
      *    operation ,and should cause us to go back to IDLE state. cdc_write
      *    is not expected to be called again without another call to init/open.
      * 2. The device is suspended (for example - the USB cable is disconnected). 
      *    We should keep TXing as long as we did not get a disabled_cb, since 
      *    the device can be resumed at any time, without us being notified. 
      */ 
      /* sio_close() is not yet supported, so this obviously the second case... */
      HSU_MSG_HIGH(
        "hsu_al_ecm_write_comp_cb(): status=CANCELLED, buf=0x%x (handle=%d)",
        buffer_ptr,context_ptr->handle,0);
      break; /* handling code is after the end of the switch statement */
    }
  case CDC_TRANSFER_OK:
    {
      break; /* handling code is after the end of the switch statement */
    }
  default:
    {
      context_ptr->data_info.write_buffer_data.data_tx_buf_num_of_bytes = 0;
      context_ptr->data_info.write_buffer_data.data_tx_buf_state =
        HSU_ECM_BUFFER_STATE_IDLE;

      HSU_MSG_ERROR(
        "hsu_al_ecm_write_comp_cb(): unknown status=%d, buf=0x%x, handle=%d",
        status, buffer_ptr,context_ptr->handle);
      HSU_ASSERT(
        (uint32)"hsu_al_ecm_write_comp_cb(): called with unknown status!" 
        == NULL);
    }
  }

  /* Initiate the next bulk TX transmission if there is such pending */

  /* Synchronization issue:
  * We need a critical section around the enqueue/dequeue of the WM Q, in order
  * to prevent the following scenario:
  *
  * 1. The application calls hsu_al_ecm_data_transmit() (WMQ is empty, a 
  *    cdc_write() was issued, but hsu_al_ecm_write_comp_cb() for it wasn't 
  *    yet received).
  * 2. The application calls hsu_al_ecm_data_transmit() again, and sees that 
  *    data_tx_buf_state != HSU_ECM_BUFFER_STATE_IDLE (since this is updated by 
  *    hsu_al_ecm_write_comp_cb() and it hasn't been called yet).
  * 3. The stack's DPC calls hsu_al_ecm_write_comp_cb() for the first packet,
  *    which interrupts the second application's TX. 
  *    hsu_al_ecm_write_comp_cb() does dsm_dequeue, and since the WM Q is 
  *    empty, it returns. 
  * 4. The application gets the CPU back, enqueues the chain (now the queue has 
  *    one chain), and returns.
  * Result: We have a frame in the queue that is not transmitted, but 
  *         could and should be transmitted right away!
  */
  rex_enter_crit_sect(&hsu_al_ecm_critical_section);

  if (context_ptr->data_info.dl_tlp_enabled)
  {
    dsm_chain_ptr = hsu_al_dsm_utils_peek_wmq(context_ptr->data_info.tx_queue_ptr);
  }
  else
  {
    dsm_chain_ptr = dsm_dequeue(context_ptr->data_info.tx_queue_ptr);
  }

#ifdef T_REXNT
#error code not present
#endif /*T_REXNT*/

  if( dsm_chain_ptr == NULL)
  {
    context_ptr->data_info.write_buffer_data.data_tx_buf_num_of_bytes = 0;
    context_ptr->data_info.write_buffer_data.data_tx_buf_state =
      HSU_ECM_BUFFER_STATE_IDLE;
    rex_leave_crit_sect(&hsu_al_ecm_critical_section);
    return;
  }

  rex_leave_crit_sect(&hsu_al_ecm_critical_section);

  /*If we support TLP this value will be updated in dmov_comp_cb */
  context_ptr->data_info.write_buffer_data.data_tx_buf_num_of_bytes = 
    dsm_length_packet(dsm_chain_ptr);

  context_ptr->data_info.write_buffer_data.data_tx_buf_state =
    HSU_ECM_BUFFER_STATE_PENDING_AT_DMOV;
  
#ifdef FEATURE_HS_USB_TLP
  if (context_ptr->data_info.dl_tlp_enabled)
  {
    ret_val = hsu_al_dmov_api_gather_dsm_queue_for_tlp(context_ptr->data_info.tx_queue_ptr,
      context_ptr->data_info.write_buffer_data.data_tx_buffer,
      HSU_AL_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES,
      context_ptr->data_info.write_buffer_data.packet_sizes_array,
      HSU_AL_ECM_PACKED_SIZES_ARRAY_SIZE,
      hsu_al_ecm_dmov_tlp_gather_comp_cb,
      context_ptr, TLP_HEADER_SIZE);
  }
  else
#endif /*FEATURE_HS_USB_TLP*/
  {
    ret_val = hsu_al_dmov_api_gather_dsm_chain(
      dsm_chain_ptr,
      context_ptr->data_info.write_buffer_data.data_tx_buffer, 
      HSU_AL_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES, 
      hsu_al_ecm_dmov_gather_comp_cb,
      context_ptr);
  }

  if(ret_val == FALSE)
  {
    if (!context_ptr->data_info.dl_tlp_enabled)
    {
      /*If TLP is enabled the dsm chain was freed in dmov */
      dsm_free_packet(&dsm_chain_ptr);
    }
    context_ptr->data_info.write_buffer_data.data_tx_buf_num_of_bytes = 0;
    context_ptr->data_info.write_buffer_data.data_tx_buf_state =
      HSU_ECM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR(
      "hsu_al_ecm_write_comp_cb(): DMOV gather failed dsm_chain_head=0x%x (handle=%d)",
      context_ptr->handle,dsm_chain_ptr,0);
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_write_comp_cb(): DMOV gather failed" 
      == NULL);
    /* When temporary ASSERT is removed - close? */
  }
} /*hsu_al_ecm_write_comp_cb*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_GET_FAKE_MAC_ADDR_AS_STR

DESCRIPTION   
Creates a fake MAC address. 

The real MAC address in the application is formulated based on information 
sent by the host on the control channel.

However, in ECM, the MAC address should be supplied for the descriptors, 
before there's a QMI link, and therefore we cannot use the same method.

Therefore it was decided that the ECM AL will "make up" a MAC address for the 
descriptors, but the host driver will ignore this address, and use the current
protocol to negotiate the MAC address that will be used for the session.

This will only  become an issue if we want to work with a standard ECM driver 
(either a Linux machine or a non-Qualcomm Windows driver). 

DEPENDENCIES  
None.

RETURN VALUE  
TRUE/FALSE

SIDE EFFECTS  
None.

==============================================================================*/
static boolean hsu_al_ecm_get_fake_mac_addr_as_str
(
 char *  mac_addr_as_str_ptr, 
 uint8   size
 )
{
#if 0
  uint32 random_num = (rand() & 0xFFFFFF); /* 6 byte number */
#endif 

  HSU_ASSERT(mac_addr_as_str_ptr != NULL);

  if(size < HSU_ECM_MAC_ADDR_AS_STR_LENGTH)
  {
    return FALSE;
  }

  (void)_snprintf(mac_addr_as_str_ptr, 
    size, 
    "%02x%02x%02x000000", 
    ((QMI_ETH_VENDOR_ID >> 16) & 0xFF), 
    ((QMI_ETH_VENDOR_ID >> 8) & 0xFF), 
    (QMI_ETH_VENDOR_ID & 0xFF));

#if 0
  (void)_snprintf(mac_addr_as_str_ptr, 
    size, 
    "%02x%02x%02x%06lx", 
    ((QMI_ETH_VENDOR_ID >> 16) & 0xFF), 
    ((QMI_ETH_VENDOR_ID >> 8) & 0xFF), 
    (QMI_ETH_VENDOR_ID & 0xFF),
    random_num);
#endif

  return TRUE;
} /* hsu_al_ecm_get_fake_mac_addr_as_str */ 

/*==============================================================================

FUNCTION      HSU_AL_ECM_CLEAR

DESCRIPTION   
Clears internal data of the HSU ECM AL that are not specific for a certain 
instance, and also, the first time this is called, zeros all the instance 
specific fields. 
Called when the instance counter hits zero. 
This happens on power-up, when ecm_init is called for the first time ever, and 
then whenever hsu_al_ecm_uninit is called (number of calls to ecm_init equals 
number of calls to hsu_al_ecm_uninit).

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_clear(void)
{
  static boolean was_already_called = FALSE;
  if(was_already_called == FALSE)
  {
    was_already_called = TRUE;

    /* Initialize static data */
    /**************************/
    rex_init_crit_sect(&hsu_al_ecm_critical_section);

    /* Initialize instance data */
    /****************************/
    /* General data */
    ecm_instance_info.handle = NULL;
    ecm_instance_info.state = HSU_ECM_INSTANCE_STATE_NONE;
    ecm_instance_info.instance_is_enabled = FALSE;
    ecm_instance_info.app_callbacks.enable_disable_cb_ext = NULL;
    ecm_instance_info.app_callbacks.enable_disable_ext_user_data = NULL;

    /* Control Info */
    ecm_instance_info.control_info.stream_id = SIO_NO_STREAM_ID;
    ecm_instance_info.control_info.rx_queue_ptr = NULL;
    ecm_instance_info.control_info.tx_queue_ptr = NULL;
    ecm_instance_info.control_info.rx_func_ptr = NULL;
    ecm_instance_info.control_info.write_buffer_data.ctrl_tx_num_of_bytes = 0;

    /* Data Info */
    ecm_instance_info.data_info.stream_id = SIO_NO_STREAM_ID;
    ecm_instance_info.data_info.read_buf_1_info.data_read_state = 
      HSU_ECM_BUFFER_STATE_IDLE;
    ecm_instance_info.data_info.read_buf_1_info.data_read_num_bytes = 0;
    ecm_instance_info.data_info.read_buf_1_info.instance_ptr = &ecm_instance_info;
    ecm_instance_info.data_info.read_buf_2_info.data_read_state = 
      HSU_ECM_BUFFER_STATE_IDLE;
    ecm_instance_info.data_info.read_buf_2_info.data_read_num_bytes = 0;
    ecm_instance_info.data_info.read_buf_2_info.instance_ptr = &ecm_instance_info;
    ecm_instance_info.data_info.inbound_flow_is_enabled = FALSE; 
    ecm_instance_info.data_info.dl_tlp_enabled = FALSE;
    ecm_instance_info.data_info.ul_tlp_enabled = FALSE;

    ecm_instance_info.data_info.write_buffer_data.data_tx_buf_num_of_bytes = 0;
    ecm_instance_info.data_info.write_buffer_data.data_tx_buf_state = 
      HSU_ECM_BUFFER_STATE_IDLE;
    ecm_instance_info.data_info.write_buffer_data.instance_ptr = &ecm_instance_info;

    ecm_instance_info.data_info.rx_queue_ptr = NULL;
    ecm_instance_info.data_info.tx_queue_ptr = NULL;
    ecm_instance_info.data_info.rx_func_ptr = NULL;
    ecm_instance_info.data_info.connection_speed_args.downstream_bitrate = 0;
    ecm_instance_info.data_info.connection_speed_args.upstream_bitrate = 0;
    ecm_instance_info.data_info.conn_speed_mem.resend = FALSE;
    ecm_instance_info.data_info.conn_speed_mem.is_send_opr_in_progress = FALSE;
  }
} /* hsu_al_ecm_clear */

/*==============================================================================

FUNCTION      HSU_AL_ECM_UNINIT_CB

DESCRIPTION   
Called by the FD when a certain instance of the network function should be 
un-initialized.

DEPENDENCIES  
ecm_init() for the same instance must have been called.

RETURN VALUE  
JEINVAL if the context is invalid, HSU_JSUCCESS for success.

SIDE EFFECTS  
None.

==============================================================================*/
static jresult_t hsu_al_ecm_uninit_cb(cdc_appctx_t ctx)
{
  jresult_t jresult = HSU_JSUCCESS;
  hsu_al_ecm_instance_info_type * instance_info_ptr = NULL;
  hsu_bus_mode_cb_info_struct suspend_resume_callback_info;

  if(cdc_app_context_is_valid(ctx) == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_uninit_cb(): ctx param invalid" == NULL);
    return JEINVAL;
  }

  instance_info_ptr = (hsu_al_ecm_instance_info_type *)(ctx);
  HSU_MSG_HIGH("hsu_al_ecm_uninit_cb(): Enter (handle=%d)", 
    instance_info_ptr->handle,0,0);

  suspend_resume_callback_info.bus_mode_cb = hsu_al_ecm_suspend_resume_mode_changed_cb;
  suspend_resume_callback_info.app_data = ctx;
  hsu_unregister_bus_mode_notif_cb(suspend_resume_callback_info);

  if(instance_info_ptr->instance_is_enabled == TRUE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_uninit_cb(): called when instance_is_enabled is TRUE" 
      == NULL);
  }

  HSU_MSG_LOW("hsu_al_ecm_uninit_cb(): called for instance with handle %d",
    instance_info_ptr->handle,0,0);

  /* protects access to several static fields */
  rex_enter_crit_sect(&hsu_al_ecm_critical_section); 
  /* uninit only FD related members. Application related members are
  untouched */
  instance_info_ptr->handle = NULL;

  rex_leave_crit_sect(&hsu_al_ecm_critical_section);

  if (hsu_al_ecm_num_of_initialized_instances > 0)
  {
    --hsu_al_ecm_num_of_initialized_instances;
  }
  if (hsu_al_ecm_num_of_initialized_instances == 0)
  {
    memset(ecm_fd_handle, 0, sizeof(ecm_fd_handle));
  }

  return jresult;
} /* hsu_al_ecm_uninit_cb */

/*==============================================================================

FUNCTION      GET_AND_ALLOC_ECM_INSTANCE_INFO_FOR_FD

DESCRIPTION   
Called from ecm_init, to find an instance_info_ptr to be used for the specific 
FD instance. 
Currently, since only one instance of the network driver is supported, the 
function simply returns the static ecm_instance_info's address.
In the future, this implementation will have to change. 

DEPENDENCIES  
None.

RETURN VALUE  
A pointer to an ecm_instance_info.

SIDE EFFECTS  
None.

==============================================================================*/
static hsu_al_ecm_instance_info_type * get_and_alloc_ecm_instance_info_for_fd
(
 cdc_handle_t      handle
 /* handle to this instance of the FD, to be used in further API calls */
)
{
  /* In multi configuration, we use the same instance for all the configurations.
  Therefore, we want to keep the handle for the first instance only.
  In case a different instance of ECM should be used for each instance,
  this implementation should be changed to keep the handle for each instance. */
  if (hsu_al_ecm_num_of_initialized_instances == 0)
  {
    ecm_instance_info.handle = handle;
    if (ecm_instance_info.state == HSU_ECM_INSTANCE_STATE_NONE)
    {
      ecm_instance_info.state = HSU_ECM_INSTANCE_STATE_INITIALIZED;
    }

#ifdef FEATURE_HS_USB_TLP
    ecm_instance_info.data_info.dl_tlp_enabled = FALSE;
    ecm_instance_info.data_info.ul_tlp_enabled = FALSE;
#endif /*FEATURE_HS_USB_TLP*/
  }

  /* We save the handle of all the FDs that the stack initialized. 
  When the configuration will be changed we will use the corresponding handle 
  for the stack's FD */
  ecm_fd_handle[HSU_CORE_IDX_PRIMARY][hsu_al_ecm_num_of_initialized_instances] = handle;

  ++hsu_al_ecm_num_of_initialized_instances; 

  return &ecm_instance_info;
} /* get_and_alloc_ecm_instance_info_for_fd */

/*==============================================================================

FUNCTION      GET_AND_ALLOC_ECM_INSTANCE_INFO_FOR_APP

DESCRIPTION   
Called from hsu_al_ecm_control_open() and hsu_al_ecm_data_open(), 
to find an instance_info_ptr to be used for the specific APP instance. 

Currently, since only one instance of the network driver is supported, the 
function simply returns the static ecm_instance_info's address.

In the future, this implementation will have to change, so that the same 
ecm_instance_info would be returned for the 2 calls for a certain instance.

DEPENDENCIES  
None.

RETURN VALUE  
A pointer to an ecm_instance_info.

SIDE EFFECTS  
None.

==============================================================================*/
static hsu_al_ecm_instance_info_type * get_and_alloc_ecm_instance_info_for_app
(
 void
 )
{
  if((ecm_instance_info.state != HSU_ECM_INSTANCE_STATE_INITIALIZED) &&
    (ecm_instance_info.state != HSU_ECM_INSTANCE_STATE_HALF_OPENED))
  {
    HSU_MSG_ERROR(
      "get_and_alloc_ecm_instance_info_for_app(): unexpected state %d.", 
      ecm_instance_info.state,0,0);
    HSU_ASSERT(
      (char *)"get_and_alloc_ecm_instance_info_for_app(): unexpected state." 
      == NULL);
  }
  return &ecm_instance_info;
} /* get_and_alloc_ecm_instance_info_for_app */

/*===========================================================================

FUNCTION hsu_al_ecm_set_control_line_state_cb

DESCRIPTION
Called whenever the ECM FD receives a SET_CONTROL_LINE_STATE from the host.
It will simply check if the DTR (Data Terminal Ready) is different than what 
was previously saved. If so, will update the new DTR and invoke the RmNet's 
DTR flag changed callback.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t hsu_al_ecm_set_control_line_state_cb
(
 juint16_t ctrl_sig_bitmap, /* Bitmap representing the control line's 
                            state */
 cdc_appctx_t ctx           /* Context provided to the ECM FD, in ecm_init*/
)
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  boolean is_dtr_asserted; 
    
  if ((ctrl_sig_bitmap & AL_SER_ECM_DTR_ASSERTED_MASK) == AL_SER_ECM_DTR_ASSERTED_MASK)
  {
    is_dtr_asserted = TRUE;
  }
  else
  {
    is_dtr_asserted = FALSE;
  }
  
  if(cdc_app_context_is_valid(ctx) == FALSE)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_set_control_line_state_cb(): ctx param invalid" == NULL);
  }

  context_ptr = (hsu_al_ecm_instance_info_type *)(ctx);

  /* Check if the DTR bit is different between the given ctrl_sig_bitmap and
  the current ctrl_line_state: */
  if (context_ptr->ctrl_line_state != is_dtr_asserted)
  {
    context_ptr->ctrl_line_state = !context_ptr->ctrl_line_state;
    invoke_app_enable_disable_cbs(context_ptr);
  }

  return HSU_JSUCCESS;
}





/*==============================================================================

FUNCTION      HSU_AL_ECM_REGISTER_BUS_MODE_NOTIFICATIONS

DESCRIPTION   
Called by the ecm_init upon stack initialization. 

DEPENDENCIES  
None.

RETURN VALUE  
HSU_JSUCCESS for success.

SIDE EFFECTS  
None.

==============================================================================*/
static jresult_t hsu_al_ecm_register_bus_mode_notifications(hsu_al_ecm_instance_info_type * context_ptr)
{
  hsu_bus_mode_cb_info_struct suspend_resume_callback_info;
  static boolean was_already_called = FALSE;
  if(was_already_called == FALSE)
  {
    was_already_called = TRUE;

    suspend_resume_callback_info.bus_mode_cb = hsu_al_ecm_suspend_resume_mode_changed_cb;
    suspend_resume_callback_info.app_data = context_ptr;
    if(hsu_register_bus_mode_notif_cb(suspend_resume_callback_info)
      == FALSE)
    {
      HSU_ASSERT((uint32)"ecm_init(): hsu_register_bus_mode_notif_cb failed" 
        == NULL);
      return EUNKNOWN;
    }
  }
  return HSU_JSUCCESS;
}

/*==============================================================================

FUNCTION      ECM_INIT

DESCRIPTION   
Called by the FD upon stack initialization. 

DEPENDENCIES  
None.

RETURN VALUE  
HSU_JSUCCESS for success.

SIDE EFFECTS  
None.

==============================================================================*/
jresult_t ecm_init
(
 cdc_handle_t      handle, 
 /* handle to this instance of the FD, to be used in further API calls */
 ecm_callbacks_t * cbs_ptr,
 /* OUT parameter - adaptation layer CBs to be invoked by the FD */
 ecm_init_info_t * info_ptr, 
 /* OUT parameter - application / adaptation layer information to be used by 
 * the FD for descriptor building */
 cdc_appctx_t *    ctx_ptr
 /* OUT parameter - adaptation layer context to be passed by the FD when CBs 
 * are invoked */
 )
{  
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  HSU_ASSERT(cbs_ptr != NULL);
  HSU_ASSERT(info_ptr != NULL);  

  HSU_MSG_HIGH("ecm_init(): Enter (handle=%d)", handle,0,0);

  /* This function has a static flag to verify that the clear is done only once */
  hsu_al_ecm_clear();

  /*****************************************************************************
  * Info
  ****************************************************************************/ 
  info_ptr->basic.flags = (juint32_t)(CDC_INIT_BUFFER_DMABLE | 
    CDC_INIT_SINGLE_INTERFACE |
    CDC_INIT_VENDOR_SPECIFIC_CLASS |
    CDC_INIT_NO_CLASS_DESC |
    CDC_INIT_NO_ALT_SETTINGS);

#ifdef FEATURE_HS_USB_TLP
    /* In order for the MDLM descriptors to be sent to the host in enumeration
    CDC_INIT_NO_CLASS_DESC must not be used. */
    info_ptr->basic.flags &= (~(juint32_t)CDC_INIT_NO_CLASS_DESC); 
#endif /*FEATURE_HS_USB_TLP*/

#ifdef FEATURE_HS_USB_TLP
  /* max_segment_size effects ZLP. If TLP is disabled but we define
     max_segment_size = HSU_AL_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES (> 1514)
     then the buffer passed to the stack will always be less than
     max_segment_size, thus ZLP check will be enabled, which is ok */
  info_ptr->basic.max_segment_size = HSU_AL_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES;
#else
  info_ptr->basic.max_segment_size = HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES;
#endif /*FEATURE_HS_USB_TLP*/
  if(hsu_al_ecm_get_fake_mac_addr_as_str(
    info_ptr->mac_addr, 
    sizeof(info_ptr->mac_addr)) != TRUE)
  {
    HSU_ASSERT(
      (uint32)"ecm_init(): hsu_al_ecm_get_fake_mac_addr_as_str failed" == NULL);
  }

  info_ptr->basic.cci_subclass = HSU_AL_ECM_SUBCLASS;
  info_ptr->basic.cci_protocol = HSU_AL_ECM_PROTOCOL;

  info_ptr->eth_statistics = 0;
  /* Flags defined in ecm_enum_stats_t, 0 means no capabilities */


  /* 
  * The num_power_filters field contains the number of pattern filters that are
  * available for causing wake-up of the host. 
  * It is related to the ECM requestSetEthernetPowerManagementPatternFilter, 
  * which is optional, and not supported by us. 
  * From the CDC/ECM spec: Some hosts are able to conserve energy and stay 
  * quiet in a “sleeping” state while not being used. USB Networking devices   
  * may provide special pattern filtering hardware that enables it to wake up  
  * the attached host on demand when something is attempting to contact the  
  * host (e.g., an incoming web browser connection). 
  * Primitives are needed in management plane to negotiate the setting of these 
  * special filters.
  */
  info_ptr->num_power_filters = 0;  /* no power filters */

  /* mc_filters - Contains the number of multi cast filters that can be 
  configured by the host. */ 
  /* 
  * From the CDC/ECM Spec:
  * 0  - The device performs perfect multi cast address filtering (no hashing).
  * 1 - The device uses imperfect multi cast address filtering (hashing). 
  *      Here, the host software driver must perform further qualification 
  *      itself to achieve perfect filtering.
  */
  info_ptr->mc_filters.perfect_filtering = 1; 
  /* bit field can have the values 0 or 1, otherwise Lint is unhappy */

  /* 
  * From the CDC/ECM Spec:
  * The num_filters field contains the number of multi cast filters that
  * can be configured by the host.
  * If this value is 0, the device does not support the ECM request
  * SetEthernetMulticastFilters.
  */
  info_ptr->mc_filters.num_filters = 0; 

  /* Enable support for ZLP */
  info_ptr->basic.zlp_terminator = TRUE;

  /* Communication interface string descriptor */
  info_ptr->basic.comm_string = NULL;
  /* data interface string descriptor */
  info_ptr->basic.data_string = NULL;

  /*****************************************************************************
  * Callbacks
  ****************************************************************************/ 
  cbs_ptr->basic.enabled_h = hsu_al_ecm_enabled_cb;
  cbs_ptr->basic.disabled_h = hsu_al_ecm_disabled_cb;
  cbs_ptr->basic.uninit_h = hsu_al_ecm_uninit_cb;
  cbs_ptr->basic.encap_command_h = hsu_al_ecm_encap_command_cb;
  cbs_ptr->set_multicast_filter_h = NULL;
  cbs_ptr->set_power_filter_h = NULL;
  cbs_ptr->get_power_filter_h = NULL;
  cbs_ptr->set_packet_filter_h = hsu_al_ecm_set_pkt_filter_cb;
  cbs_ptr->get_stats_h = NULL;
  cbs_ptr->set_control_line_state_h = hsu_al_ecm_set_control_line_state_cb;

  /* Request completion handlers */
  cbs_ptr->basic.encap_response_complete_h = 
    hsu_al_ecm_encap_resp_comp_cb;
  cbs_ptr->get_power_filter_complete_h = NULL;
  cbs_ptr->get_stats_complete_h = NULL;

  /* Notification completion handlers */
  cbs_ptr->notify_net_conn_complete_h = hsu_al_ecm_notif_net_conn_comp_cb;
  cbs_ptr->notify_speed_change_complete_h = 
    hsu_al_ecm_notif_conn_speed_ch_comp_cb;

  /* Transfer completion handlers */
  cbs_ptr->basic.read_complete_h = hsu_al_ecm_read_comp_cb;
  cbs_ptr->basic.write_complete_h = hsu_al_ecm_write_comp_cb;

  /* TODO: Callback now NULL so there won't be surprises when resumed_h is used 
  */
  cbs_ptr->basic.resumed_h = NULL;

  /*****************************************************************************
  * Initialize the instance's info_ptr structure
  ****************************************************************************/ 
  context_ptr = get_and_alloc_ecm_instance_info_for_fd(handle);
  HSU_ASSERT(context_ptr != NULL);

  *ctx_ptr = context_ptr;

  /* This function has a static flag to verify that the clear is done only once */
  return hsu_al_ecm_register_bus_mode_notifications(context_ptr);

} /* ecm_init */ 

/*==============================================================================

FUNCTION      HSU_AL_ECM_OPEN_PTR_IS_VALID

DESCRIPTION   
Validates that certain fields of the sio_open_type parameter passed to 
hsu_al_ecm_control_open() and hsu_al_ecm_data_open() are as expected. 

DEPENDENCIES  
None.

RETURN VALUE  
TRUE/ FALSE

SIDE EFFECTS  
None.

==============================================================================*/
static boolean hsu_al_ecm_open_ptr_is_valid
(
 const sio_open_type * open_ptr
 )
{
  HSU_ASSERT(open_ptr != NULL);
  HSU_ASSERT(open_ptr->port_id == SIO_PORT_USB_NET_WWAN);

  if(open_ptr->stream_id == SIO_NO_STREAM_ID)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_open_ptr_is_valid(): stream_id==SIO_NO_STREAM_ID",0,0,0);
    return FALSE;
  }

  /* TODO ECM - in qmux_init, they forget to set stream_type, so it remains 0, 
  which is RX_ONLY... 
  For now, skip this check but try to ask them to set it to SIO_STREAM_RXTX */
  /*
  if(open_ptr->stream_type != SIO_STREAM_RXTX)
  {
  HSU_MSG_ERROR(
  "hsu_al_ecm_open_ptr_is_valid(): stream_type!=SIO_STREAM_RXTX (%d)", 
  open_ptr->stream_type,0,0);
  return FALSE;
  }
  */

  if(open_ptr->tx_queue == NULL) 
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_open_ptr_is_valid(): TX WM Q is not defined",0,0,0);
    return FALSE;
  }

  /* sio_open and sio_control_open allow the open_ptr->rx_queue to be NULL if 
  * open_ptr->rx_func_ptr is not NULL. 
  * However, the RmNet application doesn't exploit that option, and uses the 
  * WMQ only. 
  * Therefore, HS-USB, at least at first, supports only RX WMQ, and not RX CBs 
  */
  if((open_ptr->rx_queue == NULL) && (open_ptr->rx_func_ptr == NULL))
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_open_ptr_is_valid(): Neither RX WM nor rx_func_ptr is defined",
      0,0,0);
    return FALSE;
  }

  if(open_ptr->stream_mode != SIO_GENERIC_MODE)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_open_ptr_is_valid(): stream_mode!=SIO_GENERIC_MODE (%d)", 
      open_ptr->stream_mode,0,0);
    return FALSE;
  }

  if(open_ptr->rx_bitrate != SIO_BITRATE_BEST)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_open_ptr_is_valid(): rx_bitrate!=SIO_BITRATE_BEST (%d)", 
      open_ptr->rx_bitrate,0,0);
    return FALSE;
  }

  if(open_ptr->tx_bitrate != SIO_BITRATE_BEST)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_open_ptr_is_valid(): tx_bitrate!=SIO_BITRATE_BEST (%d)", 
      open_ptr->tx_bitrate,0,0);
    return FALSE;
  }

  if(open_ptr->tail_char_used != FALSE)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_open_ptr_is_valid(): tail_char_used!=FALSE", 0,0,0);
    return FALSE;
  }

  if(open_ptr->rx_flow != SIO_FCTL_BEST)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_open_ptr_is_valid(): rx_flow!=SIO_FCTL_BEST", 
      open_ptr->rx_flow,0,0);
    return FALSE;
  }

  if(open_ptr->tx_flow != SIO_FCTL_BEST)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_open_ptr_is_valid(): tx_flow!=SIO_FCTL_BEST", 
      open_ptr->tx_flow,0,0);
    return FALSE;
  }
  return TRUE;
} /* hsu_al_ecm_open_ptr_is_valid */

/*==============================================================================

FUNCTION      HSU_AL_ECM_CONTROL_OPEN

DESCRIPTION   
Called by the application to allocate and configure the control plane for the 
network device.

Note: Unlike sio_control_open, rx_queue MUST be defined in the open_ptr.
Also, rx_func_ptr are not supported. 

DEPENDENCIES  
ecm_init() must have been called for this instance.

RETURN VALUE  
SIO_DONE_S: SIO stream as specified in open_ptr has been successfully opened.
SIO_BADP_S:  Bad Parameter

SIDE EFFECTS  
None.

==============================================================================*/
sio_status_type hsu_al_ecm_control_open
(
 sio_open_type * open_ptr
 )
{
  hsu_al_ecm_instance_info_type * context_ptr;

  HSU_MSG_HIGH("hsu_al_ecm_control_open(): Enter", 0,0,0);

  HSU_ASSERT(open_ptr != NULL);

  if(hsu_al_ecm_open_ptr_is_valid(open_ptr) == FALSE)
  {
    HSU_MSG_ERROR("hsu_al_ecm_control_open(): open_ptr has some invalid values", 
      0,0,0);
    return SIO_BADP_S;
  }

  context_ptr = get_and_alloc_ecm_instance_info_for_app();
  if(context_ptr == NULL)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_control_open(): \
              get_and_alloc_ecm_instance_info_for_app failed" == NULL);
  }

  HSU_ASSERT(context_ptr->control_info.stream_id == SIO_NO_STREAM_ID);

  context_ptr->control_info.stream_id = open_ptr->stream_id;
  /* This is the instance's unique id */ 
  context_ptr->control_info.rx_queue_ptr = open_ptr->rx_queue;
  context_ptr->control_info.tx_queue_ptr = open_ptr->tx_queue;
  context_ptr->control_info.rx_func_ptr = open_ptr->rx_func_ptr;

  if(context_ptr->state == HSU_ECM_INSTANCE_STATE_HALF_OPENED)
  {
    context_ptr->state = HSU_ECM_INSTANCE_STATE_OPENED;
  }
  else if(context_ptr->state == HSU_ECM_INSTANCE_STATE_INITIALIZED)
  {
    context_ptr->state = HSU_ECM_INSTANCE_STATE_HALF_OPENED;
  }
  else
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_control_open(): state = %d, expected INITIALIZED or HALF_OPENED",
      context_ptr->state,0,0);
    HSU_ASSERT(
      (char *)"hsu_al_ecm_control_open(): unexpected state. Expected INITIALIZED or HALF_OPENED" 
      == NULL);
  }

  return SIO_DONE_S;
} /* hsu_al_ecm_control_open */

/*==============================================================================

FUNCTION      HSU_AL_ECM_DATA_OPEN

DESCRIPTION   
Called by the application to allocate and configure the data plane for the 
network function.
Note: Unlike sio_open, rx_queue MUST be defined in the open_ptr.
Also, rx_func_ptr are not supported. 

DEPENDENCIES  
ecm_init() and hsu_al_ecm_control_open() must have been called for this 
instance.

RETURN VALUE  
SIO_DONE_S: SIO stream as specified in open_ptr has been successfully opened.
SIO_BADP_S:  Bad Parameter

SIDE EFFECTS  
This causes 2 buffers to be given to the USB stack, to be used when OUT 
transfers arrive. 

==============================================================================*/
sio_status_type  hsu_al_ecm_data_open
(
 sio_open_type * open_ptr 
 )
{
  hsu_al_ecm_instance_info_type * context_ptr;

  HSU_MSG_HIGH("hsu_al_ecm_data_open(): Enter", 0,0,0);

  HSU_ASSERT(open_ptr != NULL);

  if(hsu_al_ecm_open_ptr_is_valid(open_ptr) == FALSE)
  {
    HSU_MSG_ERROR("hsu_al_ecm_data_open(): open_ptr has some invalid values", 
      0,0,0);
    return SIO_BADP_S;
  }

  context_ptr = get_and_alloc_ecm_instance_info_for_app();

  if(context_ptr == NULL)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_data_open(): \
              get_and_alloc_ecm_instance_info_for_app failed" == NULL);
  }

  HSU_ASSERT(context_ptr->data_info.stream_id == SIO_NO_STREAM_ID);

  context_ptr->data_info.stream_id = open_ptr->stream_id;
  /* This is the instance's unique id */ 
  context_ptr->data_info.rx_queue_ptr = open_ptr->rx_queue;
  context_ptr->data_info.tx_queue_ptr = open_ptr->tx_queue;
  context_ptr->data_info.rx_func_ptr = open_ptr->rx_func_ptr;

  if(context_ptr->state == HSU_ECM_INSTANCE_STATE_HALF_OPENED)
  {
    context_ptr->state = HSU_ECM_INSTANCE_STATE_OPENED;
  }
  else if(context_ptr->state == HSU_ECM_INSTANCE_STATE_INITIALIZED)
  {
    context_ptr->state = HSU_ECM_INSTANCE_STATE_HALF_OPENED;
  }
  else
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_data_open(): state = %d, expected INITIALIZED or HALF_OPENED",
      context_ptr->state,0,0);
    HSU_ASSERT(
      (char *)"hsu_al_ecm_data_open(): unexpected state. Expected INITIALIZED or HALF_OPENED" 
      == NULL);
  }

  return SIO_DONE_S;
} /* hsu_al_ecm_data_open */

/*==============================================================================

FUNCTION      GET_INSTANCE_INFO_FROM_STREAM_ID

DESCRIPTION   
Returns a pointer to the ecm_instance_info_type that matches the given stream 
id.
Currently, since only one instance of the network driver is supported, the 
function just verifies that the stream id matches the control/data channel's 
stream id.
In the future, it will go over all existing instances and return the relevant 
one. 

DEPENDENCIES  
None.

RETURN VALUE  
Pointer to the ecm_instance_info_type that matches the given stream id.

SIDE EFFECTS  
None.
==============================================================================*/
static hsu_al_ecm_instance_info_type * get_instance_info_from_stream_id
(
 sio_stream_id_type  stream_id
 )
{
  if((stream_id != ecm_instance_info.data_info.stream_id) &&
    (stream_id != ecm_instance_info.control_info.stream_id))
  {
    HSU_MSG_ERROR(
      "get_instance_info_from_stream_id(): unknown stream_id=%d - \
      data channel's=%d, control channel's=%d",
      stream_id, 
      ecm_instance_info.data_info.stream_id, 
      ecm_instance_info.control_info.stream_id);
    return NULL;
  }
  else
  {
    return &ecm_instance_info;
  }
} /* get_instance_info_from_stream_id */

/*==============================================================================

FUNCTION      HSU_AL_ECM_CONTROL_TRANSMIT

DESCRIPTION   
Called by the application, when it wishes to send a message on the control
channel. The message is given as a chain of DSM items. 

The function gathers the DSM chain into a preallocated buffer, and gives it to
the cdc_send_encap_response() function of the USB stack. 

The stack is responsible to generate a "response available" notification, and 
then send the message after the host sends an IN token on the control channel
following the notification. 

Only one control message can be sent per instance at a time. 
Meaning, from the moment cdc_send_encap_response was called, and until the 
hsu_al_ecm_encap_resp_comp_cb was called, additional calls to 
cdc_send_encap_response() will fail!
therefore, additional calls to hsu_al_ecm_control_transmit cause the DSM chain
to be queued to the control TX WM Q.

DEPENDENCIES  
ecm_init() and hsu_al_ecm_control_open() must have been called for this 
instance.

RETURN VALUE  
None.

SIDE EFFECTS  
None. 
==============================================================================*/
void hsu_al_ecm_control_transmit
( 
 sio_stream_id_type  stream_id, 
 dsm_item_type *     tx_ptr
 )
{
  hsu_al_ecm_instance_info_type * context_ptr;

  HSU_ASSERT(tx_ptr != NULL); /* as done in usbdc_control_write() */

  context_ptr = get_instance_info_from_stream_id(stream_id);
  if(context_ptr == NULL)
  {
    HSU_ASSERT(
      (uint32)"*_control_transmit(): Unknown stream_id" == NULL);
    dsm_free_packet(&tx_ptr);
  }
  HSU_MSG_HIGH("*_control_transmit(): Enter. tx_ptr=0x%x (handle=%d)", 
    tx_ptr, context_ptr->handle, 0);

  if(context_ptr->state != HSU_ECM_INSTANCE_STATE_OPENED)
  {
    dsm_free_packet(&tx_ptr);

    HSU_MSG_ERROR(
      "*_control_transmit(): Instance is not opened (state=%d, handle=%d)",
      context_ptr->state, context_ptr->handle, 0);
    HSU_ASSERT(
      (uint32)"*_control_transmit(): Instance is not opened." == NULL);
  }

  /* 
  * if the disabled CB was invoked, there's no point in sending data to the 
  * host, since we're not connected. 
  */
  if(context_ptr->instance_is_enabled == FALSE)
  {
    /* Disabled CB was called. This should not cause an assert, since it is a 
    * common scenario - 
    * Following the call to the Application's dtr_cb, a QMI message is sent to 
    * notify the host that the packet data session was ended. But the host is 
    * not connected...
    */
    HSU_MSG_MED(
      "*_control_transmit: Instance is disabled (handle=%d). Discarding\
      message 0x%x.",
      context_ptr->handle, tx_ptr, 0);
    dsm_free_packet(&tx_ptr);
    return;
  }

  /* If there's already a control transmit in progress,*/ 
  /* enqueue the message to the control TX WM Q        */
  /*****************************************************/
  rex_enter_crit_sect(&hsu_al_ecm_critical_section);
  if(context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes != 0)
  {
    /* There's already a control transmit in progress */
    /* Enqueue DSM chain to Control RX WMQ */
    HSU_MSG_MED(
      "*_control_transmit(): CTRL TX already in progress, \
      enqueuing DSM chain to WMQ",
      0,0,0);
    HSU_ASSERT(context_ptr->control_info.tx_queue_ptr != NULL);
    dsm_enqueue(context_ptr->control_info.tx_queue_ptr, &tx_ptr);
    rex_leave_crit_sect(&hsu_al_ecm_critical_section);
    return;
  }

  context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 
    dsm_length_packet(tx_ptr);
  rex_leave_crit_sect(&hsu_al_ecm_critical_section);

  perform_control_transmit(context_ptr, tx_ptr);
} /* hsu_al_ecm_control_transmit */

/*==============================================================================

FUNCTION                  HSU_AL_ECM_DATA_TRANSMIT 

DESCRIPTION
Called by the application, when it wishes to send data on the bulk end point.
The data (Ethernet frame) is given as a chain of DSM items. 

The function gathers the DSM chain into a preallocated buffer using the DMOV, 
and then gives it to the cdc_write() function of the USB stack. 

DEPENDENCIES
ecm_init() and hsu_al_ecm_control_open() and hsu_al_ecm_data_open() must have 
been called for this instance.

RETURN VALUE
None

SIDE EFFECTS
None.

==============================================================================*/
void hsu_al_ecm_data_transmit
( 
 sio_stream_id_type  stream_id,
 sio_port_id_type    port_id,
 dsm_item_type *     tx_ptr /* Packet for transmission */
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  boolean ret_val;
  uint32 curr_packet_length = 0;

  HSU_USE_PARAM(port_id);
  HSU_ASSERT(tx_ptr != NULL); 
  /* this is checked in sio_transmit(), so it's safe to ASSERT it */

  context_ptr = get_instance_info_from_stream_id(stream_id);
  if(context_ptr == NULL)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_data_transmit(): Unknown stream_id" == NULL);
    dsm_free_packet(&tx_ptr);
  }

  if(context_ptr->state != HSU_ECM_INSTANCE_STATE_OPENED)
  {
    dsm_free_packet(&tx_ptr);

    HSU_MSG_ERROR(
      "hsu_al_ecm_data_transmit(): Instance state=%d (expected opened) (handle=%d)",
      context_ptr->state, context_ptr->handle, 0);
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_data_transmit(): Instance is not opened." 
      == NULL);
  }

  /* 
  * if the disabled CB was invoked, there's no point in sending data to the 
  * host, since we're not connected. 
  */
  if(context_ptr->instance_is_enabled == FALSE)
  {
    HSU_MSG_MED(
      "hsu_al_ecm_data_transmit: Instance is disabled (handle=%d). Discarding frame 0x%x.",
      context_ptr->handle, tx_ptr, 0);
    dsm_free_packet(&tx_ptr);
    return;
  }

  curr_packet_length = dsm_length_packet(tx_ptr);
  /*Check that the size of the packet is legal. 
    QOS is not enabled for downlink*/
  if ( curr_packet_length > HSU_ECM_MAX_ETHERNET_FRAME_LENGTH)
  {
    HSU_MSG_ERROR("hsu_al_ecm_data_transmit(): discard ethernet frame "
      "larger than 1514 bytes (size=%d)", curr_packet_length, 0, 0); 
    dsm_free_packet(&tx_ptr);
    return;
  }

#ifdef FEATURE_HS_USB_ECM_DEBUG
  if(0 == context_ptr->data_info.tx_queue_ptr->current_cnt)
  {
    HSU_MSG_LOW("hsu_al_ecm_data_transmit(): adding %d bytes to empty TX WM Q",
      curr_packet_length,0,0);
  }
#endif /* FEATURE_HS_USB_ECM_DEBUG */
  
  /* If there's already a data transmit in progress,*/ 
  /* enqueue the message to the data TX WM Q        */
  /**************************************************/
  /* Please refer to hsu_al_ecm_write_comp_cb() for documentation about the 
  * need for a critical section here */
  if(context_ptr->data_info.write_buffer_data.data_tx_buf_state != 
    HSU_ECM_BUFFER_STATE_IDLE)
  {
    /* There's already a bulk transmit in progress */
    /* Enqueue DSM chain to Data RX WMQ */
    HSU_ASSERT(context_ptr->data_info.tx_queue_ptr != NULL);
    rex_enter_crit_sect(&hsu_al_ecm_critical_section);
    dsm_enqueue(context_ptr->data_info.tx_queue_ptr, &tx_ptr);
    rex_leave_crit_sect(&hsu_al_ecm_critical_section);
    return;
  }


  /* Initialize a DMOV gather operation */
  /***************************************/  
  /* Don't want to flood DIAG... 
  HSU_MSG_LOW(
  "hsu_al_ecm_data_transmit(): Calling DMOV gather with dsm_head=0x%x (handle=%d)",
  tx_ptr, context_ptr->handle, 0); */

  context_ptr->data_info.write_buffer_data.data_tx_buf_num_of_bytes = 
    curr_packet_length;
  context_ptr->data_info.write_buffer_data.data_tx_buf_state = 
    HSU_ECM_BUFFER_STATE_PENDING_AT_DMOV;

#ifdef FEATURE_HS_USB_TLP
  if (context_ptr->data_info.dl_tlp_enabled)
  {
    /*If TLP is enabled we need to enqueue the packet since it will be
    dequeued in hsu_al_dmov_api_gather_dsm_queue_for_tlp*/
    dsm_enqueue(context_ptr->data_info.tx_queue_ptr, &tx_ptr);
    ret_val = hsu_al_dmov_api_gather_dsm_queue_for_tlp(context_ptr->data_info.tx_queue_ptr,
      context_ptr->data_info.write_buffer_data.data_tx_buffer,
      HSU_AL_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES,
      context_ptr->data_info.write_buffer_data.packet_sizes_array,
      HSU_AL_ECM_PACKED_SIZES_ARRAY_SIZE,
      hsu_al_ecm_dmov_tlp_gather_comp_cb,
      context_ptr, TLP_HEADER_SIZE);
  }
  else  /*context_ptr->data_info.dl_tlp_enabled == FALSE*/
#endif /*FEATURE_HS_USB_TLP*/
  {
    ret_val = hsu_al_dmov_api_gather_dsm_chain(
      tx_ptr, 
      context_ptr->data_info.write_buffer_data.data_tx_buffer, 
      HSU_AL_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES, 
      hsu_al_ecm_dmov_gather_comp_cb, 
      context_ptr);
  }

  if(ret_val == FALSE)
  {
    dsm_free_packet(&tx_ptr);
    context_ptr->data_info.write_buffer_data.data_tx_buf_num_of_bytes = 0;
    context_ptr->data_info.write_buffer_data.data_tx_buf_state = 
      HSU_ECM_BUFFER_STATE_IDLE;

    HSU_MSG_ERROR(
      "hsu_al_ecm_data_transmit(): DMOV gather failed dsm_chain_head=0x%x (handle=%d)",
      context_ptr->handle,tx_ptr,0);
  #ifdef T_REXNT
#error code not present
  #endif /* T_REXNT */

  }
} /* hsu_al_ecm_data_transmit */

/*==============================================================================

FUNCTION      SEND_CONN_SPEED_CHANGE_NOTIF

DESCRIPTION   
Sends a "Connection Speed Changed" notification to the host, indicating either 
that the down link is down (which prevents additional OUT transfers) or back 
up (which causes OUT transfers to resume).

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
static void send_conn_speed_change_notif
(
 hsu_al_ecm_instance_info_type * context_ptr,
 boolean                         is_downlink_up
 )
{
  boolean           defer_call = FALSE;

  if(context_ptr->instance_is_enabled == FALSE)
  {
    HSU_MSG_MED(
      "send_conn_speed_change_notif(): Instance is disabled. Ignoring.\
      (handle=%d)",
      context_ptr->handle,0,0);
    /* TODO ECM - when the enabled_cb is invoked, should we send this notification? */
    return;
  }

  if(is_downlink_up == TRUE)
  {
    context_ptr->data_info.connection_speed_args.downstream_bitrate = 
      HSU_EL_ECM_CONNECTION_SPEED_CHANGED_TO_UP;
    /* In FS, 0x40000 (256 kbps) is used, to indicate a speed of ~300 kbps. 
    It is OK that the value is not accurate, since our peer is a proprietary 
    driver, which treats this value as a boolean (connection exists or not). 
    */
  }
  else
  {
    context_ptr->data_info.connection_speed_args.downstream_bitrate = 0;
  }

  /* upstream is always enabled */
  context_ptr->data_info.connection_speed_args.upstream_bitrate = 
    HSU_EL_ECM_CONNECTION_SPEED_CHANGED_TO_UP; 

#ifndef T_REXNT
  if(rex_is_in_irq_mode())
  {
    /* We're in interrupt context. We should deffer the call to 
    hsu_ecm_fd_api_notify_conn_speed_change to the AL task's context */
    defer_call = TRUE;
  }
#endif

  if(defer_call == TRUE)
  {
    hsu_al_task_enqueue_cmd(send_or_remember_con_speed_ch, context_ptr);
  }
  else
  {
    HSU_MSG_LOW(
      "send_conn_speed_change_notif(): calling defer_call_to_notif_con_speed_ch,\
      with connection.down=0x%x, handle=%d", 
      context_ptr->data_info.connection_speed_args.downstream_bitrate,
      context_ptr->handle, 0);
    send_or_remember_con_speed_ch((void*)context_ptr);
  }  
} /* send_conn_speed_change_notif */

/*==============================================================================

FUNCTION      ENABLE_INBOUND_FLOW

DESCRIPTION   
This function performs the required operations for enabling inbound flow:
- Marks the flag 'inbound_flow_is_enabled' accordingly
- Sends a Connection Speed Changed notification to the host
- If there's a buffer waiting to be sent to the application, it calls the 
function handle_read_complete() with this buffer.
- If one of the 2 read buffers is idle, it gives it to the stack using the 
cdc_read() API.

Note: It is called from the application's context

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
static void enable_inbound_flow
(
 hsu_al_ecm_instance_info_type * context_ptr
 )
{
  HSU_MSG_HIGH("enable_inbound_flow(): Enter",0,0,0);

  if(context_ptr->data_info.inbound_flow_is_enabled == TRUE)
  {
    HSU_MSG_LOW("enable_inbound_flow(): inbound flow is already enabled",
      0,0,0);
    return;/* nothing to do */
  }      

  context_ptr->data_info.inbound_flow_is_enabled = TRUE;

  if(context_ptr->instance_is_enabled == FALSE)
  {
    /* 
    * The first time this IOCTL is used, is right after sio_open(), before 
    * enabled_cb() was invoked from the stack. 
    *In this special case, we only need to set the flow to enabled.
    */
    HSU_MSG_MED(
      "enable_inbound_flow(): instance is disabled. Just marking inbound flow\
      as enabled.",
      0,0,0);
    return;
  }

  /* Make sure the control stream was opened */
  if(context_ptr->control_info.stream_id == SIO_NO_STREAM_ID)
  {
    /* 
    * This means we are in the middle of the initialization. 
    * right after sio_open(), before sio_control_open(). 
    * In this special case, we only need to set the flow to enabled.
    */
    HSU_MSG_MED(
      "enable_inbound_flow(): instance is in the middle of initialization.\
      Just marking inbound flow as enabled.",
      0,0,0);
    return;
  }

  send_conn_speed_change_notif(context_ptr, TRUE);

  /* If there's a buffer waiting to be sent to the application, do so */
  /********************************************************************/
  if(context_ptr->data_info.read_buf_1_info.data_read_state == 
    HSU_ECM_BUFFER_STATE_READY_FOR_DMOV)
  {
    HSU_MSG_MED(
      "enable_inbound_flow(): buffer 1 ready to be scattered by DMOV",
       0,0,0);
    handle_read_complete(context_ptr, 
                         &(context_ptr->data_info.read_buf_1_info));
  }

  if(context_ptr->data_info.read_buf_2_info.data_read_state ==
    HSU_ECM_BUFFER_STATE_READY_FOR_DMOV)
  {
    /* If both buffers are waiting for DMOV it means that when inbound flow
         was disabled both buffers were pending at stack*/
    HSU_MSG_MED(
      "enable_inbound_flow(): buffer 2 ready to be scattered by DMOV",
      0,0,0);
    handle_read_complete(context_ptr, 
                         &(context_ptr->data_info.read_buf_2_info));
  }

  /* For each IDLE buffer - give it to the stack */
  /***********************************************/
  if(context_ptr->data_info.read_buf_1_info.data_read_state == 
    HSU_ECM_BUFFER_STATE_IDLE)
  {
    HSU_MSG_MED(
      "enable_inbound_flow(): calling cdc_read with buf 1 ",0, 0, 0);

    context_ptr->data_info.read_buf_1_info.data_read_state = 
                              HSU_ECM_BUFFER_STATE_PENDING_AT_STACK;
    defer_call_to_cdc_read((void *)(&(context_ptr->data_info.read_buf_1_info)));
  }

  if(context_ptr->data_info.read_buf_2_info.data_read_state == 
    HSU_ECM_BUFFER_STATE_IDLE)
  {
    HSU_MSG_MED(
      "enable_inbound_flow(): calling cdc_read with buf 2",0,0, 0);

    context_ptr->data_info.read_buf_2_info.data_read_state = 
                              HSU_ECM_BUFFER_STATE_PENDING_AT_STACK;
    defer_call_to_cdc_read((void *)(&(context_ptr->data_info.read_buf_2_info)));
  }

  HSU_MSG_HIGH("enable_inbound_flow(): Exit",0,0,0);
} /* enable_inbound_flow */

/*==============================================================================

FUNCTION      DISABLE_INBOUND_FLOW

DESCRIPTION   
This function performs the required operations for disabling inbound flow:
- Marks the flag 'inbound_flow_is_enabled' to FALSE
- Sends a Connection Speed Changed notification to the host

Note: It is called from the application's context

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
static void disable_inbound_flow
(
 hsu_al_ecm_instance_info_type * context_ptr
 )
{
  if(context_ptr->data_info.inbound_flow_is_enabled == FALSE)
  {
    HSU_MSG_LOW(
      "disable_inbound_flow(): inbound flow is already disabled",
      0,0,0);
    return; /* nothing to do */
  }

  context_ptr->data_info.inbound_flow_is_enabled = FALSE;

  /* Make sure the control stream was opened */
  HSU_ASSERT(context_ptr->control_info.stream_id != SIO_NO_STREAM_ID);

  send_conn_speed_change_notif(context_ptr, FALSE);
} /* disable_inbound_flow */

/*==============================================================================

FUNCTION      REMEMBER_CONN_SPEED_NOTIF

DESCRIPTION   
This function checks whether we need to remember the current connection
speed notification and to resend it when possible.


DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
static void remember_conn_speed_notif
(
 hsu_al_ecm_instance_info_type * context_ptr
)
{
  /* If there's already a message "in memory", that is the opposite of this one, 
  forget them both */
  if ((context_ptr->data_info.conn_speed_mem.resend == TRUE) &&
    (context_ptr->data_info.conn_speed_mem.connection_speed_args.downstream_bitrate != 
    context_ptr->data_info.connection_speed_args.downstream_bitrate))
  {
    HSU_MSG_LOW("remember_conn_speed_notif: canceling the remembered notif. "
      "remembered_speed=%d, new_speed = %d",
      context_ptr->data_info.conn_speed_mem.connection_speed_args.downstream_bitrate,
      context_ptr->data_info.connection_speed_args.downstream_bitrate,0);
    context_ptr->data_info.conn_speed_mem.resend = FALSE;
  }
  else /* We need to resend the msg */
  {
    HSU_MSG_LOW("remember_conn_speed_notif: remembering notification to send: new_speed = %d",
      context_ptr->data_info.connection_speed_args.downstream_bitrate,0,0);
    context_ptr->data_info.conn_speed_mem.resend = TRUE;
    context_ptr->data_info.conn_speed_mem.connection_speed_args = 
      context_ptr->data_info.connection_speed_args;
  }
}
/*==============================================================================

FUNCTION      HSU_AL_ECM_IOCTL

DESCRIPTION   
This function is used to control various features of the USB function.  
The behavior of this function depends on the 'cmd' issued.

DEPENDENCIES  
The instance must be in the OPEN state, meaning the ecm_init(), 
hsu_al_ecm_control_open() and hsu_al_ecm_data_open() must have been called for
this instance.
When the command is SIO_IOCTL_ENABLE_DTR_EVENT - 'param' cannot be NULL.

RETURN VALUE  
None.

SIDE EFFECTS  
Depends on the command issued.
==============================================================================*/
void hsu_al_ecm_ioctl
(
 sio_stream_id_type      stream_id,
 sio_port_id_type        port_id,
 sio_ioctl_cmd_type      cmd,
 sio_ioctl_param_type *  param
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  sio_ioctl_usb_tlp_settings_type settings;
  sio_ioctl_usb_reg_tlp_change_cb_type * tlp_change_cb_param;
  boolean dte_ready;

  HSU_USE_PARAM(port_id);
  HSU_PARAM_COULD_BE_CONST(param);

  context_ptr = get_instance_info_from_stream_id(stream_id);
  if(context_ptr == NULL)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_ioctl(): Unknown stream_id" == NULL);
  }

  if((context_ptr->state != HSU_ECM_INSTANCE_STATE_OPENED) &&
    (context_ptr->state != HSU_ECM_INSTANCE_STATE_HALF_OPENED))
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_ioctl(): Called while instance's state=%d, not OPENED/HALF_OPENED\
      (handle=%d)",
      context_ptr->state, context_ptr->handle, 0);
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_ioctl(): Instance is not opened." == NULL);
  }

  HSU_ASSERT(port_id == SIO_PORT_USB_NET_WWAN);

  switch(cmd)
  {
  case SIO_IOCTL_ENABLE_DTR_EVENT_EXT:
    {
      HSU_MSG_MED("hsu_al_ecm_ioctl(): got SIO_IOCTL_ENABLE_DTR_EVENT_EXT (handle=%d)",
        context_ptr->handle,0,0);
      /* Register callback which informs the caller of change in the state of 
      of the device (enabled/disabled). 
      The callback function must act quickly since it is runs in the DPC 
      context (perhaps simply issue a signal). */
      ASSERT(param != NULL);
      context_ptr->app_callbacks.enable_disable_cb_ext = 
        (*(sio_ioctl_enable_cb_event_ext_type *)param).cb_func;
      context_ptr->app_callbacks.enable_disable_ext_user_data = 
        (*(sio_ioctl_enable_cb_event_ext_type *)param).cb_data;
      invoke_app_enable_disable_cbs(context_ptr);
      break;
    }

  case SIO_IOCTL_DISABLE_DTR_EVENT_EXT:
    {
      HSU_MSG_MED("hsu_al_ecm_ioctl(): got SIO_IOCTL_DISABLE_DTR_EVENT_EXT (handle=%d)",
        context_ptr->handle,0,0);
      /* Unregister call-back for change in state of data terminal ready indication */
      context_ptr->app_callbacks.enable_disable_cb_ext = NULL;
      context_ptr->app_callbacks.enable_disable_ext_user_data = NULL;
      break;
    }
  case SIO_IOCTL_DTE_READY_ASSERTED:
    {
      dte_ready = context_ptr->ctrl_line_state; 
      HSU_MSG_MED(
        "hsu_al_ecm_ioctl(): got SIO_IOCTL_DTE_READY_ASSERTED, returning %d",
        dte_ready, 0, 0);
      /* Retrieve the state of the DTR signal (1 if the instance is resumed, 
      0 if it is suspended */
      HSU_ASSERT(param != NULL);
      *(((sio_ioctl_param_type *)param)->dte_ready_asserted) = dte_ready;
      break;
    }
  case SIO_IOCTL_INBOUND_FLOW_ENABLE:
    {
      /* 
      * Should allow OUT transfers to arrive and be queued to the application's
      * data RX WM Q, and also send a Connection Speed Changed Notification to 
      * the host. This is done in the application context. 
      * The current behavior of the RmNet application is as described below, 
      * and might change in the future.
      * Called by the application in the following cases:
      * - After hsu_al_ecm_control_open() and hsu_al_ecm_data_open() were called 
      * - When the data RX WM Q low WM threshold was reached (currently at 1514 
      *   bytes)
      * - When a previously active packet data session has disconnected
      */
      HSU_MSG_MED("hsu_al_ecm_ioctl(): got SIO_IOCTL_INBOUND_FLOW_ENABLE (handle=%d)",
        context_ptr->handle,0,0);
      enable_inbound_flow(context_ptr);
      break;
    }
  case SIO_IOCTL_INBOUND_FLOW_DISABLE:
    {
      /* 
      * Called when the data RX WM Q high WM threshold is reached (currently at 
      * 6056 bytes). This is done in the DMOV context (interrupt context), from 
      * the hsu_al_ecm_dmov_scatter_comp_cb(). 
      * Should prevent additional OUT transfers, until hsu_al_ecm_ioctl() is 
      * called with the command SIO_IOCTL_INBOUND_FLOW_ENABLE.
      * Should also send a Connection Speed Changed Notification to the host.
      */
      HSU_MSG_MED("hsu_al_ecm_ioctl(): got SIO_IOCTL_INBOUND_FLOW_DISABLE (handle=%d)",
        context_ptr->handle,0,0);
      disable_inbound_flow(context_ptr);
      break;
    }
  case SIO_IOCTL_SET_FLOW_CTL:
    {
      HSU_MSG_MED("hsu_al_ecm_ioctl(): got SIO_IOCTL_SET_FLOW_CTL",0,0,0);
      HSU_ASSERT(param != NULL);
      if(((sio_ioctl_param_type *)param)->flow_ctl.rx_flow != SIO_FCTL_BEST)
      {
        HSU_MSG_ERROR(
          "hsu_al_ecm_ioctl(): SIO_IOCTL_SET_FLOW_CTL with rx_flow=%d, when only\
          SIO_FCTL_BEST is supported", 
          ((sio_ioctl_param_type *)param)->flow_ctl.rx_flow,0,0);
        HSU_ASSERT(
          (uint32)"hsu_al_ecm_ioctl(): SIO_IOCTL_SET_FLOW_CTL with rx_flow !=\
                  SIO_FCTL_BEST" == NULL);
      }
      if(((sio_ioctl_param_type *)param)->flow_ctl.tx_flow != SIO_FCTL_BEST)
      {
        HSU_MSG_ERROR(
          "hsu_al_ecm_ioctl(): SIO_IOCTL_SET_FLOW_CTL with tx_flow=%d, when only\
          SIO_FCTL_BEST is supported", 
          ((sio_ioctl_param_type *)param)->flow_ctl.tx_flow,0,0);
        HSU_ASSERT(
          (uint32)"hsu_al_ecm_ioctl(): SIO_IOCTL_SET_FLOW_CTL with tx_flow !=\
                  SIO_FCTL_BEST" == NULL);
      }
      break;
    }
  case SIO_IOCTL_GET_FLOW_CTL:
    {
      HSU_MSG_MED("hsu_al_ecm_ioctl(): got SIO_IOCTL_GET_FLOW_CTL",0,0,0);
      HSU_ASSERT(param != NULL);
      ((sio_ioctl_param_type *)param)->flow_ctl.tx_flow = SIO_FCTL_BEST;
      ((sio_ioctl_param_type *)param)->flow_ctl.rx_flow = SIO_FCTL_BEST;
      break;
    }
  case SIO_IOCTL_REG_TLP_CHANGE_CB:
    {
      HSU_MSG_MED("hsu_al_ecm_ioctl(): got SIO_IOCTL_REG_TLP_CHANGE_CB",0,0,0);
      /* 
      * We need to call the CB function to notify what the TLP state is (no TLP).
      *
      * For now, there's no need to register the callback function, as we do 
      * not support TLP, and so no change can occur. 
      */
      HSU_ASSERT(param != NULL);
      settings.version = HSU_TLP_VERSION_NONE;
      settings.uplink_tlp_on = FALSE;
      settings.downlink_tlp_on = FALSE;
      tlp_change_cb_param = (sio_ioctl_usb_reg_tlp_change_cb_type *)param;
      tlp_change_cb_param->callback(
        settings, 
        tlp_change_cb_param->user_data);
      break;        
    }
  default:
    {
      HSU_MSG_ERROR("hsu_al_ecm_ioctl(): cmd %d not supported!",cmd,0,0);
      HSU_ASSERT(
        (uint32)"hsu_al_ecm_ioctl(): unsupported IOCTL command" == NULL);
    }
  } /* switch(cmd) */
} /* hsu_al_ecm_ioctl */

/*==============================================================================

FUNCTION                HSU_AL_ECM_DATA_FLUSH_TX 

DESCRIPTION
TODO ECM

DEPENDENCIES
TODO ECM

RETURN VALUE
None.

SIDE EFFECTS
TODO ECM

==============================================================================*/
void hsu_al_ecm_data_flush_tx
( 
 sio_stream_id_type  stream_id,             
 sio_port_id_type    port_id,
 void                (*flush_func_ptr)(void)  
 /* Function to call when transmission is complete. */
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = 
    get_instance_info_from_stream_id(stream_id);
  if(context_ptr == NULL)
  {
    HSU_ASSERT(
      (uint32)"hsu_al_ecm_data_flush_tx(): Unknown stream_id" == NULL);
  }

  HSU_USE_PARAM(flush_func_ptr);
  HSU_USE_PARAM(port_id);

  HSU_ASSERT(
    (uint32)"hsu_al_ecm_data_flush_tx() called, but not yet implemented!" 
    == NULL);
} /* hsu_al_ecm_data_flush_tx */

/*==============================================================================

FUNCTION                    HSU_AL_ECM_CONTROL_CLOSE

DESCRIPTION
Called by the application to deallocate and un-configure the control plane for 
the network device.

DEPENDENCIES
ecm_init() must have been called for this instance.

RETURN VALUE
None.

SIDE EFFECTS
Empties the control plane watermark queues.

==============================================================================*/
void hsu_al_ecm_control_close
( 
  sio_stream_id_type  stream_id,
  void                (*close_func_ptr)(void)  
 /* Function to call when transmission is complete.   */
)
{
  hsu_al_ecm_instance_info_type * context_ptr = 
    get_instance_info_from_stream_id(stream_id);

  HSU_USE_PARAM(close_func_ptr);

  HSU_MSG_HIGH("hsu_al_ecm_control_close(): Enter.", 0, 0, 0);

  if(context_ptr == NULL)
  {
    HSU_MSG_ERROR("hsu_al_ecm_control_close(): Unknown stream_id.", 0, 0, 0);

    HSU_ASSERT(
      (uint32)"hsu_al_ecm_control_close(): Unknown stream_id" == NULL);

    return;
  }

  context_ptr->control_info.stream_id = SIO_NO_STREAM_ID;

  dsm_empty_queue(context_ptr->control_info.rx_queue_ptr);
  context_ptr->control_info.rx_queue_ptr = NULL;

  dsm_empty_queue(context_ptr->control_info.tx_queue_ptr);
  context_ptr->control_info.tx_queue_ptr = NULL;

  switch (context_ptr->state)
  {
    case HSU_ECM_INSTANCE_STATE_HALF_OPENED:

      context_ptr->state = HSU_ECM_INSTANCE_STATE_INITIALIZED;
      break;

    case HSU_ECM_INSTANCE_STATE_OPENED:

      context_ptr->state = HSU_ECM_INSTANCE_STATE_HALF_OPENED;
      break;

    default:

      HSU_MSG_ERROR(
        "hsu_al_ecm_control_close(): state = %d, expected OPENED or HALF_OPENED",
        context_ptr->state, 0, 0);

      HSU_ASSERT(
        (char *)"hsu_al_ecm_control_close(): "
        "unexpected state. Expected OPENED or HALF_OPENED" == NULL);

      break;

  }  /* switch (context_ptr->state) */

  HSU_MSG_HIGH("hsu_al_ecm_control_close(): Exit.", 0, 0, 0);

} /* hsu_al_ecm_control_close */ 

/*==============================================================================

FUNCTION          HSU_AL_ECM_DATA_CLOSE

DESCRIPTION
Called by the application to deallocate and un-configure the data plane for the 
network function.

DEPENDENCIES
ecm_init() must have been called for this instance.

RETURN VALUE
None.

SIDE EFFECTS
Empties the data plane watermark queues.

==============================================================================*/
void hsu_al_ecm_data_close
( 
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  void              (*close_func_ptr)(void)    
  /* Function to call when transmission is complete. */
)
{
  hsu_al_ecm_instance_info_type * context_ptr = 
    get_instance_info_from_stream_id(stream_id);

  HSU_USE_PARAM(close_func_ptr);

  if(context_ptr == NULL)
  {
    HSU_MSG_ERROR("hsu_al_ecm_data_close(): Unknown stream_id.", 0, 0, 0);

    HSU_ASSERT(
      (uint32)"hsu_al_ecm_data_close(): Unknown stream_id" == NULL);

    return;
  }

  HSU_USE_PARAM(port_id);

  HSU_MSG_HIGH("hsu_al_ecm_data_close(): Enter.", 0, 0, 0);

  context_ptr->data_info.stream_id = SIO_NO_STREAM_ID;

  dsm_empty_queue(context_ptr->data_info.rx_queue_ptr);
  context_ptr->data_info.rx_queue_ptr = NULL;

  dsm_empty_queue(context_ptr->data_info.tx_queue_ptr);
  context_ptr->data_info.tx_queue_ptr = NULL;

  switch (context_ptr->state)
  {
    case HSU_ECM_INSTANCE_STATE_HALF_OPENED:

      context_ptr->state = HSU_ECM_INSTANCE_STATE_INITIALIZED;
      break;

    case HSU_ECM_INSTANCE_STATE_OPENED:

      context_ptr->state = HSU_ECM_INSTANCE_STATE_HALF_OPENED;
      break;

    default:

      HSU_MSG_ERROR(
        "hsu_al_ecm_data_close(): state = %d, expected OPENED or HALF_OPENED",
        context_ptr->state, 0, 0);

      HSU_ASSERT(
        (char *)"hsu_al_ecm_data_close(): "
        "unexpected state. Expected OPENED or HALF_OPENED" == NULL);

      break;

  }  /* switch (context_ptr->state) */

  HSU_MSG_HIGH("hsu_al_ecm_data_close(): Exit.", 0, 0, 0);

} /* hsu_al_ecm_data_close */

#ifndef T_REXNT
/*==============================================================================

FUNCTION          HSU_AL_ECM_HANDLE_SET_CONFIGURATION

DESCRIPTION
This function is called when the enabled callback is received.
This function sets the FD handle according to the current configuration FD.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_al_ecm_handle_set_configuration()
{
  uint8 config_idx = 0;

  config_idx = core_get_curr_configuration(HSU_CORE_IDX_PRIMARY);
  HSU_MSG_LOW("hsu_al_ecm_handle_set_configuration(): config_idx = %d.", config_idx, 0, 0);
  if (config_idx == 0)
  {
    HSU_MSG_LOW("hsu_al_ecm_handle_set_configuration(): config_idx = 0, nothing to do.", 0, 0, 0);
    return;
  }

  ecm_instance_info.handle = ecm_fd_handle[HSU_CORE_IDX_PRIMARY][config_idx-1];
}
#endif /* T_REXNT */

