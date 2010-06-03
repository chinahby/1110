/*==============================================================================

USB Serial I/Fs (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB

GENERAL DESCRIPTION
Contains the implementation of the API for the serial functions' adapt. layer

EXTERNALIZED FUNCTIONS
hsu_al_ser_init
hsu_al_ser_open
hsu_al_ser_close
hsu_al_ser_transmit  
hsu_al_ser_flush_tx

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_init must be called prior to performing any other operations

Copyright (c) 2006 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_ser.c#25 $

when      who     what, where, why
--------  ---     ------------------------------------------------------
05/08/06  esh     First revision of file

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_al_ser.h"

#include "customer.h" /* For FEATURE_HSU_TEST to work */

#include "jerrno.h"
#include "uw_device_cdc_acm.h"
#include "uw_device_cdc_obex.h"
#include "uw_device_cdc.h"

#include "hsu_al_ser_i.h"
#include "hsu_al_ser_base.h"
#include "hsu_al_ser_acm.h"
#include "hsu_al_ser_obex.h"
#include "hsu_common.h"

#include "err.h"

#ifdef T_MSM7600
#error code not present
#else
#include "dsm_init.h"  /* Koel does not have diagdsm.h */
#endif /* T_MSM7600 */

#include "hsu_conf_sel_stack_utils.h"

#include "hsu_config_selector.h"

#include "hsu_conf_sel_types.h"
#include "hsu_conf_sel_i.h"

#include "rex.h" /* For rex_timer_cnt_type */

#include "hsu_core_config.h"
#include "hsu_conf_sel_core_config.h"

#include "jusb_common.h"
#include "jusb_core.h"
#include "cdc_fd.h"

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/

/* NMEA port */
#ifdef FEATURE_HS_USB_NMEA 
#define NUM_OF_NMEA_OBEX_USERS 1
#else
#define NUM_OF_NMEA_OBEX_USERS 0
#endif

/* AT commands port */
#ifdef FEATURE_HS_USB_SER3_PORT 
#define NUM_OF_SER3_PORT_OBEX_USERS 1
#else
#define NUM_OF_SER3_PORT_OBEX_USERS 0
#endif

/* Diag port */
#define NUM_OF_DIAG_OBEX_USERS 1

#define MAX_NUM_OF_OBEX_USERS ((NUM_OF_NMEA_OBEX_USERS) + \
                               (NUM_OF_SER3_PORT_OBEX_USERS) + \
                               (NUM_OF_DIAG_OBEX_USERS))

/* Max size of memory pools used */
#ifdef FEATURE_HS_USB_ACM_PERF_TESTS
#error code not present
#else
#if defined(FEATURE_HS_USB_MEM_OPTIMIZATION)
# define SIZE_OF_MEM_POOL_IN_BYTES 0x400 /* 1 KBytes */
#else
# define SIZE_OF_MEM_POOL_IN_BYTES 0x4000 /* 16 KBytes */
#endif /* FEATURE_HS_USB_MEM_OPTIMIZATION */
#endif /* FEATURE_HS_USB_ACM_PERF_TESTS */

#define SIZE_OF_MDM_MEM_POOL_IN_BYTES (SIZE_OF_MEM_POOL_IN_BYTES)

#ifdef FEATURE_HSU_TEST
#error code not present
#else /* !FEATURE_HSU_TEST */
# ifdef FEATURE_HS_USB_OBEX_PERF_TESTS
#error code not present
#else
#if defined(FEATURE_HS_USB_MEM_OPTIMIZATION)
   /* DIAG buffer needs to be at least as big as single DIAG DSM item */
#  define SIZE_OF_DIAG_MEM_POOL_IN_BYTES 0x800 /* 2 KBytes */
#else
#  define SIZE_OF_DIAG_MEM_POOL_IN_BYTES 0x2000 /* 8 KBytes */
#endif  /* FEATURE_HS_USB_MEM_OPTIMIZATION */
# endif /* FEATURE_HS_USB_OBEX_PERF_TESTS */
#endif /* FEATURE_HSU_TEST */

#define SIZE_OF_NMEA_MEM_POOL_IN_BYTES 0x2000 /* 8 KBytes*/

#ifdef FEATURE_HS_USB_SER3_PORT
/* According to hsu_al_ser_base_transmit() the max_buff_size_for_write
 should be smaller than the size_of_dsm_item. Worst case is DIAG TX 
 DSM items which are 2KB in size so SIZE_OF_USB_SER3_MEM_POOL_IN_BYTES was 
 increased to 2Kbyte */
 #define SIZE_OF_USB_SER3_MEM_POOL_IN_BYTES 0x800 /* 2 KBytes */
#endif /* FEATURE_HS_USB_SER3_PORT */

/* Typical size of memory buffers to be used */
#define TYPICAL_BUFF_SIZE_FOR_MDM_READ (SIZE_OF_MDM_MEM_POOL_IN_BYTES)
/* For now, use the entire pool */
#define TYPICAL_BUFF_SIZE_FOR_DIAG_READ (SIZE_OF_DIAG_MEM_POOL_IN_BYTES)
/* For now, use the entire pool */
#define TYPICAL_BUFF_SIZE_FOR_NMEA_READ (SIZE_OF_NMEA_MEM_POOL_IN_BYTES)
/* For now, use the entire pool */
  
#ifdef FEATURE_HS_USB_SER3_PORT
#define TYPICAL_BUFF_SIZE_FOR_USB_SER3_READ (SIZE_OF_USB_SER3_MEM_POOL_IN_BYTES)
/* For now, use the entire pool */
#endif /* FEATURE_HS_USB_SER3_PORT */

/* The following are used in hsu_al_ser_close, when no app callback is passed */
#define AL_SER_CLOSE_SLEEP_INTERVAL_TIMEOUT_IN_MSECS (100)
#define AL_SER_CLOSE_MAX_TIMEOUT_IN_MSECS \
  (10*AL_SER_CLOSE_SLEEP_INTERVAL_TIMEOUT_IN_MSECS)

#ifdef FEATURE_HSU_TEST
#error code not present
#else /* !FEATURE_HSU_TEST */

#if !defined(FEATURE_USB_CDC_ACM) || !defined(FEATURE_USB_DIAG)
#error("At least one of the following is NOT defined: FEATURE_USB_CDC_ACM, \
  FEATURE_USB_DIAG")
#endif /* !FEATURE_USB_CDC_ACM || !FEATURE_USB_DIAG */

#define AL_SER_PORT_MDM (SIO_PORT_USB_MDM)
#define AL_SER_PORT_DIAG (SIO_PORT_USB_DIAG)

#ifdef FEATURE_USB_DIAG_NMEA
#define AL_SER_PORT_NMEA (SIO_PORT_USB_NMEA)
#else /* !FEATURE_USB_DIAG_NMEA */
#define AL_SER_PORT_NMEA (SIO_PORT_NULL)
#endif /* FEATURE_USB_DIAG_NMEA */

#endif /* FEATURE_HSU_TEST */

#ifdef FEATURE_HS_USB_SER3_PORT
#define AL_SER_PORT_USB_SER3 (SIO_PORT_USB_SER3)
#endif /* FEATURE_HS_USB_SER3_PORT */

#ifdef FEATURE_HSU_TEST
#error code not present
#elif !defined(HS_USB_AGGREGATION_TIMER_FOR_MDM_MSECS)
#define HS_USB_AGGREGATION_TIMER_FOR_MDM_MSECS 0
#endif /* FEATURE_HSU_TEST */

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Variables
==============================================================================*/
static struct
{
  /*
  Each of the CDC I/Fs:
  */
  boolean                     mdm_held_by_app;
  hsu_al_ser_acm_data         mdm_acm_data;
    /* ACM data struct for Modem I/F, with indication if it is used by app */
  uint8                       num_of_initialized_acm_instances;
  /* 
  * The number of ACM instances that were allocated and initialized (for now, 1 is the 
  * maximal possible value because there's only one instance of the 
  * modem function)
  */

  cdc_handle_t                acm_fd_handle[HSU_MAX_NUM_OF_CORES][HSU_MAX_NUM_OF_CONFIGS];
  /* 
  * Keeps the handles or the FDs that the stack initialized.
  * When the configuration will be changed we will use the corresponding handle 
  * for the stack's FD.
  */

  boolean                     obex_slot_avail_for_app[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_OBEX_USERS];
  hsu_al_ser_obex_data        data_for_apps_using_obex_if[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_OBEX_USERS];
    /* Array for obex I/Fs, with indication if a slot is avail for usage or not */

  cdc_handle_t                obex_fd_handle[HSU_MAX_NUM_OF_CORES][HSU_MAX_NUM_OF_CONFIGS][MAX_NUM_OF_OBEX_USERS];
  /* 
  * Keeps the handles or the FDs that the stack initialized.
  * When the configuration will be changed we will use the corresponding handle 
  * for the stack's FD. 
  */
  uint8                       num_of_initialized_obex_instances[HSU_MAX_NUM_OF_CORES][HSU_MAX_NUM_OF_CONFIGS];
  /* 
  * The number of OBEX instances that were allocated and initialized
  */

  /* The following would point to the applicable slot in the above array,
  once hsu_al_ser_open has been issued, and would then be used instead of 
  referencing the above array. */
  hsu_al_ser_obex_data*       diag_obex_ptr;
#ifdef FEATURE_HS_USB_NMEA    
  hsu_al_ser_obex_data*       nmea_obex_ptr;
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
  hsu_al_ser_obex_data*       ser3_obex_ptr;
#endif /* FEATURE_HS_USB_SER3_PORT */

  /* Data for contiguous memory pools, for allocating buffers for cdc_read/cdc_write */
  /* TODO: these pools must be uncached ? */
#ifndef FEATURE_HSU_TEST
  byte*                       mem_pool_for_mdm_rx;
  byte*                       mem_pool_for_mdm_tx;
#else /* FEATURE_HSU_TEST */
#error code not present
#endif /* !FEATURE_HSU_TEST */

#ifndef FEATURE_HSU_TEST
  byte*                       mem_pool_for_diag_rx;
  byte*                       mem_pool_for_diag_tx;
#else /* FEATURE_HSU_TEST */
#error code not present
#endif /* !FEATURE_HSU_TEST */

#ifdef FEATURE_HS_USB_NMEA
#ifndef FEATURE_HSU_TEST
  byte*                       mem_pool_for_nmea_rx;
  byte*                       mem_pool_for_nmea_tx;
#else /* FEATURE_HSU_TEST */
#error code not present
#endif /* !FEATURE_HSU_TEST */
#endif /* FEATURE_HS_USB_NMEA */

#ifdef FEATURE_HS_USB_SER3_PORT
#ifndef FEATURE_HSU_TEST
  byte*                       mem_pool_for_ser3_rx;
  byte*                       mem_pool_for_ser3_tx;
#else /* FEATURE_HSU_TEST */
#error code not present
#endif /* !FEATURE_HSU_TEST */
#endif /* FEATURE_HS_USB_SER3_PORT */
  
  /* Storage for close and flush_tx callbacks supplied by the app */
  /* Since the app supplies void f(void) callbacks, whereas hsu_al_ser_obex 
  (and later on hsu_al_ser_acm) would like to use AL callbacks with signature 
  void f(hsu_al_ser_base_data_for_ser_if*), then the app callbacks need
  to be saved here and this layer would pass "internal" ones, so once the 
  internal AL callback is called, in the end it will call the app callback, 
  stored here. */
  sio_vv_func_ptr_type        app_close_cb_arr_for_obex[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_OBEX_USERS];
  sio_vv_func_ptr_type        app_flush_tx_cb_arr_for_obex[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_OBEX_USERS];

  sio_vv_func_ptr_type        mdm_close_cb;
  sio_vv_func_ptr_type        mdm_flush_tx_cb;  
  uint8                       curr_config_idx;
} hsu_al_ser_static_data;
  /* This file's variable */


/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
/* The following functions are for managing the OBEX data array: */
static hsu_al_ser_obex_data* get_avail_obex_data_slot_for_fd(cdc_handle_t handle);
static hsu_al_ser_obex_data* get_and_alloc_obex_data_slot_for_app
(
  sio_port_id_type port_id
);
static void release_obex_data_slot_from_app
(
  hsu_al_ser_obex_data* data_for_obex_if_ptr
);

/* The following functions are to guarantee OBEX data arrays, as well as ACM data struct
are valid prior to any hsu_al_ser_open: */
static void init_obex_data_arrays_once(void);
static void init_acm_data_once(void);

/* The following functions are for managing app supplied flush tx and close callbacks, 
for OBEX I/Fs */
static void add_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr,
  sio_vv_func_ptr_type app_close_cb
);
static sio_vv_func_ptr_type get_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
);
static void remove_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
);

static void add_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr,
  sio_vv_func_ptr_type app_flush_tx_cb
);
static sio_vv_func_ptr_type get_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
);
static void remove_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
);

/* The following functions are the callbacks given by hsu_al_ser to either the
OBEX or ACM data structs to be invoked in case of flush_tx or close 
(respectively) */
static void al_ser_on_close(hsu_al_ser_base_data_for_ser_if* data_for_ser_if_ptr);
static void al_ser_on_flush_tx(hsu_al_ser_base_data_for_ser_if* data_for_ser_if_ptr);

static boolean data_ptr_is_freed_from_app
(
  const hsu_al_ser_base_data_for_ser_if* data_if_ptr
);

static uint8 al_ser_get_obex_slot_idx_for_sio_port
(
  sio_port_id_type port_id,
  uint8* core_index
);
/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

/*===========================================================================
FUNCTION hsu_al_ser_init()
===========================================================================*/
boolean hsu_al_ser_init
(
  void 
)
{
#ifndef FEATURE_HSU_TEST
  
  uint32 total_bytes_to_alloc = 0;
  byte* mem_block_for_cdc_serial_instances = NULL;

  /* Needed sizes for serial I/Fs, 32 byte aligned 
   We multiply by 2: one for write and one for read. NMEA has only write */
  const uint32 TOTAL_BYTES_ALIGNED_TO_ALLOC_FOR_ACM = 
    2 * HSU_ROUND_UP_TO_32(SIZE_OF_MDM_MEM_POOL_IN_BYTES);
  const uint32 TOTAL_BYTES_ALIGNED_TO_ALLOC_FOR_FIRST_OBEX = 
    2 * HSU_ROUND_UP_TO_32(SIZE_OF_DIAG_MEM_POOL_IN_BYTES);
#ifdef FEATURE_HS_USB_NMEA    
  const uint32 TOTAL_BYTES_ALIGNED_TO_ALLOC_FOR_SECOND_OBEX = 
    2 * HSU_ROUND_UP_TO_32(SIZE_OF_NMEA_MEM_POOL_IN_BYTES);
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
  const uint32 TOTAL_BYTES_ALIGNED_TO_ALLOC_FOR_THIRD_OBEX = 
    2 * HSU_ROUND_UP_TO_32(SIZE_OF_USB_SER3_MEM_POOL_IN_BYTES);
#endif /* FEATURE_HS_USB_SER3_PORT */
#endif /* !FEATURE_HSU_TEST */

  hsu_al_ser_static_data.curr_config_idx = 0;

  /* Init all serial instances no matter how many, so that if an application 
  wishes to open the corresponding HSU port, it will receive a proper error
  (otherwise, an assert is issued in hsu_al_ser_open) */
  init_acm_data_once();
  init_obex_data_arrays_once();
  
  #ifndef FEATURE_HSU_TEST

  total_bytes_to_alloc = TOTAL_BYTES_ALIGNED_TO_ALLOC_FOR_ACM + 
    TOTAL_BYTES_ALIGNED_TO_ALLOC_FOR_FIRST_OBEX;

  #ifdef FEATURE_HS_USB_NMEA
  total_bytes_to_alloc += TOTAL_BYTES_ALIGNED_TO_ALLOC_FOR_SECOND_OBEX;
  #endif /* FEATURE_HS_USB_NMEA */
  
#ifdef FEATURE_HS_USB_SER3_PORT
  total_bytes_to_alloc += TOTAL_BYTES_ALIGNED_TO_ALLOC_FOR_THIRD_OBEX;
#endif /* FEATURE_HS_USB_SER3_PORT */
  
  mem_block_for_cdc_serial_instances = 
    hsu_conf_sel_stack_utils_alloc_stack_memory(total_bytes_to_alloc);
  if (mem_block_for_cdc_serial_instances == NULL)
  {
    ERR("hsu_al_ser_init: Cannot alloc mem. for serial funcs' AL. Required: %u bytes", 
      total_bytes_to_alloc, 0, 0);
    HSU_ASSERT((uint32)("mem. alloc failed") == NULL);
    return FALSE;
  }

  hsu_al_ser_static_data.mem_pool_for_mdm_rx = mem_block_for_cdc_serial_instances;
  mem_block_for_cdc_serial_instances += 
    HSU_ROUND_UP_TO_32(SIZE_OF_MDM_MEM_POOL_IN_BYTES);

  hsu_al_ser_static_data.mem_pool_for_mdm_tx = mem_block_for_cdc_serial_instances;
  mem_block_for_cdc_serial_instances += 
    HSU_ROUND_UP_TO_32(SIZE_OF_MDM_MEM_POOL_IN_BYTES);

  hsu_al_ser_static_data.mem_pool_for_diag_rx = mem_block_for_cdc_serial_instances;
  mem_block_for_cdc_serial_instances += 
    HSU_ROUND_UP_TO_32(SIZE_OF_DIAG_MEM_POOL_IN_BYTES);

  hsu_al_ser_static_data.mem_pool_for_diag_tx = mem_block_for_cdc_serial_instances;
  mem_block_for_cdc_serial_instances += 
    HSU_ROUND_UP_TO_32(SIZE_OF_DIAG_MEM_POOL_IN_BYTES);
#ifdef FEATURE_HS_USB_NMEA
  /* Allocate mem. for NMEA */
  hsu_al_ser_static_data.mem_pool_for_nmea_rx = mem_block_for_cdc_serial_instances;
  mem_block_for_cdc_serial_instances += 
    HSU_ROUND_UP_TO_32(SIZE_OF_NMEA_MEM_POOL_IN_BYTES);

  hsu_al_ser_static_data.mem_pool_for_nmea_tx = mem_block_for_cdc_serial_instances;
  mem_block_for_cdc_serial_instances += 
    HSU_ROUND_UP_TO_32(SIZE_OF_NMEA_MEM_POOL_IN_BYTES);
#endif /* FEATURE_HS_USB_NMEA */


#ifdef FEATURE_HS_USB_SER3_PORT
  /* Allocate mem. for SER3 port */
  hsu_al_ser_static_data.mem_pool_for_ser3_rx = mem_block_for_cdc_serial_instances;
  mem_block_for_cdc_serial_instances += 
    HSU_ROUND_UP_TO_32(SIZE_OF_USB_SER3_MEM_POOL_IN_BYTES);

  hsu_al_ser_static_data.mem_pool_for_ser3_tx = mem_block_for_cdc_serial_instances;
  mem_block_for_cdc_serial_instances += 
    HSU_ROUND_UP_TO_32(SIZE_OF_USB_SER3_MEM_POOL_IN_BYTES);
#endif /* FEATURE_HS_USB_SER3_PORT */

  /* Check that there is no memory overflow */
  if ((hsu_al_ser_static_data.mem_pool_for_mdm_rx + total_bytes_to_alloc) <
      (mem_block_for_cdc_serial_instances))
  {
    HSU_ASSERT("hsu_al_ser_init(): Memory overflow! " == NULL);
  }

  #endif /* ! FEATURE_HSU_TEST */

  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_ser_open()
===========================================================================*/
sio_status_type hsu_al_ser_open
( 
  sio_open_type *open_ptr
)
{
  boolean ret_val;
  rex_timer_cnt_type tx_aggr_delay_in_msecs = 0;

  if (open_ptr == NULL)
  {
    HSU_MSG_ERROR("hsu_al_ser_open: Received NULL open_ptr!", 0, 0, 0);
    return SIO_BADP_S;
  }

  /* TODO: Validation function */
  /* i.e.: validate stream_id != SIO_NO_STREAM_ID */

  HSU_MSG_HIGH("hsu_al_ser_open: Entered", 0, 0, 0);

  switch (open_ptr->port_id)
  {
    case AL_SER_PORT_MDM:
    {
      HSU_MSG_HIGH("hsu_al_ser_open: Opening Modem", 0, 0, 0);

      tx_aggr_delay_in_msecs = HS_USB_AGGREGATION_TIMER_FOR_MDM_MSECS;

      ret_val = hsu_al_ser_acm_open(&(hsu_al_ser_static_data.mdm_acm_data), 
        open_ptr, DSM_DS_LARGE_ITEM_POOL, hsu_al_ser_static_data.mem_pool_for_mdm_rx, 
        hsu_al_ser_static_data.mem_pool_for_mdm_tx, TYPICAL_BUFF_SIZE_FOR_MDM_READ, 
        SIZE_OF_MDM_MEM_POOL_IN_BYTES, tx_aggr_delay_in_msecs);

      if (ret_val == FALSE)
      {
        HSU_MSG_ERROR("hsu_al_ser_open: hsu_al_ser_base_open for port MDM (%d)\
                      returned failure", open_ptr->port_id, 0, 0);

        /* TODO: invalidate fields in mdm_acm_data? */

        return SIO_BADP_S;
      }

      hsu_al_ser_static_data.mdm_held_by_app = TRUE;

      break;
    }
    case AL_SER_PORT_DIAG:
    {
      HSU_MSG_HIGH("hsu_al_ser_open: Opening DIAG", 0, 0, 0);

      hsu_al_ser_static_data.diag_obex_ptr = 
        get_and_alloc_obex_data_slot_for_app(AL_SER_PORT_DIAG);

      if (hsu_al_ser_static_data.diag_obex_ptr == NULL)
      {
        HSU_MSG_ERROR("hsu_al_ser_open: DIAG port already taken", 0, 0, 0);
        return SIO_BADP_S;
      }

      ret_val = hsu_al_ser_base_open((hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.diag_obex_ptr, 
        open_ptr, 
        DSM_DIAG_ITEM_POOL, hsu_al_ser_static_data.mem_pool_for_diag_rx, 
        hsu_al_ser_static_data.mem_pool_for_diag_tx, 
        TYPICAL_BUFF_SIZE_FOR_DIAG_READ, 
        SIZE_OF_DIAG_MEM_POOL_IN_BYTES,
        TYPICAL_BUFF_SIZE_FOR_DIAG_READ, tx_aggr_delay_in_msecs);

      if (ret_val == FALSE)
      {
        HSU_MSG_ERROR("hsu_al_ser_open: hsu_al_ser_base_open for port DIAG (%d)\
                      returned failure", open_ptr->port_id, 0, 0);

        release_obex_data_slot_from_app(hsu_al_ser_static_data.diag_obex_ptr);

        hsu_al_ser_static_data.diag_obex_ptr = NULL;
        return SIO_BADP_S;
      }

      break;
    }
#ifdef FEATURE_HS_USB_NMEA
    case AL_SER_PORT_NMEA:
    {
      HSU_MSG_HIGH("hsu_al_ser_open: Opening NMEA", 0, 0, 0);

      hsu_al_ser_static_data.nmea_obex_ptr = 
        get_and_alloc_obex_data_slot_for_app(AL_SER_PORT_NMEA);
   
      if (hsu_al_ser_static_data.nmea_obex_ptr == NULL)
      {
        HSU_MSG_ERROR("hsu_al_ser_open: NMEA port already taken", 0, 0, 0);
        return SIO_BADP_S;
      }

      ret_val = hsu_al_ser_base_open((hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.nmea_obex_ptr, 
        open_ptr, 
        DSM_DS_SMALL_ITEM_POOL, hsu_al_ser_static_data.mem_pool_for_nmea_rx,
        hsu_al_ser_static_data.mem_pool_for_nmea_tx, 
        TYPICAL_BUFF_SIZE_FOR_NMEA_READ, SIZE_OF_NMEA_MEM_POOL_IN_BYTES,
        TYPICAL_BUFF_SIZE_FOR_NMEA_READ, tx_aggr_delay_in_msecs);

      if (ret_val == FALSE)
      {
        HSU_MSG_ERROR("hsu_al_ser_open: hsu_al_ser_base_open for port NMEA (%d)\
                      returned failure", open_ptr->port_id, 0, 0);

        release_obex_data_slot_from_app(hsu_al_ser_static_data.nmea_obex_ptr);

        hsu_al_ser_static_data.nmea_obex_ptr = NULL;
        return SIO_BADP_S;
      }

      break;      
    }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
      {
        HSU_MSG_HIGH("hsu_al_ser_open: Opening USB_SER3 port", 0, 0, 0);
        
        hsu_al_ser_static_data.ser3_obex_ptr = 
          get_and_alloc_obex_data_slot_for_app(AL_SER_PORT_USB_SER3);

        if (hsu_al_ser_static_data.ser3_obex_ptr == NULL)
        {
          HSU_MSG_ERROR("hsu_al_ser_open: SER3 port already taken", 0, 0, 0);
          return SIO_BADP_S;
        }
          
        ret_val = hsu_al_ser_base_open((hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.ser3_obex_ptr, 
          open_ptr, DSM_DS_LARGE_ITEM_POOL, 
          hsu_al_ser_static_data.mem_pool_for_ser3_rx, 
          hsu_al_ser_static_data.mem_pool_for_ser3_tx, 
          TYPICAL_BUFF_SIZE_FOR_USB_SER3_READ, 
          SIZE_OF_USB_SER3_MEM_POOL_IN_BYTES,
          TYPICAL_BUFF_SIZE_FOR_USB_SER3_READ, tx_aggr_delay_in_msecs);

        if (ret_val == FALSE)
        {
          HSU_MSG_ERROR("hsu_al_ser_open: hsu_al_ser_base_open for port USB_SER3 (%d)\
                        returned failure", open_ptr->port_id, 0, 0);

          release_obex_data_slot_from_app(hsu_al_ser_static_data.ser3_obex_ptr);

          hsu_al_ser_static_data.ser3_obex_ptr = NULL;
          return SIO_BADP_S;
        }

        break;
      }
#endif /* FEATURE_HS_USB_SER3_PORT */
    default:
    {
      HSU_MSG_ERROR("hsu_al_ser_open: Port: %d not supported", 
        open_ptr->port_id, 0, 0);
      return SIO_BADP_S;
    }
  }

  HSU_MSG_HIGH("hsu_al_ser_open: Exit Successfully", 0, 0, 0);

  return SIO_DONE_S;
}

/*===========================================================================
FUNCTION hsu_al_ser_close()
===========================================================================*/
void hsu_al_ser_close
( 
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  sio_vv_func_ptr_type close_done_cb
)
{
  boolean abort_io_operations_immediately = TRUE;

  uint32 remaining_msecs_to_wait_for_close = AL_SER_CLOSE_MAX_TIMEOUT_IN_MSECS;

  hsu_al_ser_base_data_for_ser_if* common_data_ptr = NULL;

  HSU_USE_PARAM(stream_id);

  HSU_MSG_HIGH("hsu_al_ser_close: Entered", 0, 0, 0);

  switch (port_id)
  {
    case AL_SER_PORT_MDM:
    {
      common_data_ptr = 
        (hsu_al_ser_base_data_for_ser_if*)&(hsu_al_ser_static_data.mdm_acm_data);
      if (close_done_cb != NULL)
      {
        /* Save original App callback: */
        hsu_al_ser_static_data.mdm_close_cb = close_done_cb;

        /* Don't abort I/O operations, but rather wait until Tx is flushed. */
        abort_io_operations_immediately = FALSE;

        HSU_MSG_HIGH("hsu_al_ser_close (MDM): (with cb) would be completed async", 
          0, 0, 0);
      }

      hsu_al_ser_acm_close(&(hsu_al_ser_static_data.mdm_acm_data), 
        al_ser_on_close, abort_io_operations_immediately);

      break;
    }
    case AL_SER_PORT_DIAG:
    {
      common_data_ptr = (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.diag_obex_ptr;
      if (close_done_cb != NULL)
      {
        /* Save original App callback: */
        add_app_close_cb_for_obex(hsu_al_ser_static_data.diag_obex_ptr, close_done_cb);

        /* Don't abort I/O operations, but rather wait until Tx is flushed. */
        abort_io_operations_immediately = FALSE;

        HSU_MSG_HIGH("hsu_al_ser_close (DIAG): (with cb) would be completed async", 
          0, 0, 0);

      }

      hsu_al_ser_obex_close(hsu_al_ser_static_data.diag_obex_ptr, al_ser_on_close, 
        abort_io_operations_immediately);

      break;
    }
#ifdef FEATURE_HS_USB_NMEA
    case AL_SER_PORT_NMEA:
    {
      common_data_ptr = (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.nmea_obex_ptr;
      if (close_done_cb != NULL)
      {
        /* Save original App callback: */
        add_app_close_cb_for_obex(hsu_al_ser_static_data.nmea_obex_ptr, close_done_cb);

        /* Don't abort I/O operations, but rather wait until Tx is flushed. */
        abort_io_operations_immediately = FALSE;

        HSU_MSG_HIGH("hsu_al_ser_close (NMEA): (with cb) would be completed async", 
          0, 0, 0);
      }

      hsu_al_ser_base_close(common_data_ptr, al_ser_on_close, 
        abort_io_operations_immediately);

      break;
    }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
      {
        common_data_ptr = (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.ser3_obex_ptr;
        if (close_done_cb != NULL)
        {
          /* Save original App callback: */
          add_app_close_cb_for_obex(hsu_al_ser_static_data.ser3_obex_ptr, close_done_cb);

          /* Don't abort I/O operations, but rather wait until Tx is flushed. */
          abort_io_operations_immediately = FALSE;

          HSU_MSG_HIGH("hsu_al_ser_close (USB_SER3): (with cb) would be completed async", 
            0, 0, 0);
        }

        hsu_al_ser_base_close(common_data_ptr, al_ser_on_close, 
          abort_io_operations_immediately);

        break;
      }
#endif /* FEATURE_HS_USB_SER3_PORT */
    default:
    {
      HSU_MSG_ERROR("hsu_al_ser_close: Port: %d not supported", 
        port_id, 0, 0);
      HSU_ASSERT((uint32)"hsu_al_ser_close: Invalid port" == NULL);
    }
  }

  /* If close_done_cb == NULL, there is a need for a rex_sleep loop, 
  to simulate synchronous call 
  (without modifying the code of the upper apps) */

  if (close_done_cb == NULL)
  {
    HSU_MSG_HIGH("hsu_al_ser_close: Waiting for close (no cb) to complete", 0, 0, 0);

    while (remaining_msecs_to_wait_for_close > 0)
    {
      if (data_ptr_is_freed_from_app(common_data_ptr) == TRUE)
      {
        break;
      }
      /* else: sleep for a certain interval and retry: */

      HSU_MSG_LOW("hsu_al_ser_close: rex_sleep for %u msecs. Remaining: %u msecs", 
        AL_SER_CLOSE_SLEEP_INTERVAL_TIMEOUT_IN_MSECS, 
        remaining_msecs_to_wait_for_close, 0);

      rex_sleep(AL_SER_CLOSE_SLEEP_INTERVAL_TIMEOUT_IN_MSECS);

      remaining_msecs_to_wait_for_close -= 
        AL_SER_CLOSE_SLEEP_INTERVAL_TIMEOUT_IN_MSECS;
    }
    /* Since there is no return value, this is the only way to indicate that 
    despite waiting sufficient time, this layer has yet to finish the close
    operation. */
    HSU_ASSERT(data_ptr_is_freed_from_app(common_data_ptr) == TRUE);

    HSU_MSG_HIGH("hsu_al_ser_close: close (no cb) complete", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION hsu_al_ser_transmit()
===========================================================================*/
void hsu_al_ser_transmit
( 
  sio_stream_id_type stream_id,
  sio_port_id_type port_id,
  dsm_item_type *tx_ptr
)
{
  hsu_al_ser_base_data_for_ser_if* common_data_ptr = NULL;

  HSU_USE_PARAM(stream_id);

  if (tx_ptr == NULL)
  {
    ERR("hsu_al_ser_transmit: tx_ptr is NULL", 0, 0, 0);
    HSU_ASSERT((uint32)"hsu_al_ser_transmit: Illegal params!" == NULL);
  }

  switch (port_id)
  {
    case AL_SER_PORT_MDM:
    {
      common_data_ptr = 
        (hsu_al_ser_base_data_for_ser_if*)&(hsu_al_ser_static_data.mdm_acm_data);
      break;
    }
    case AL_SER_PORT_DIAG:
    {
      common_data_ptr = (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.diag_obex_ptr;

      break;
    }
#ifdef FEATURE_HS_USB_NMEA
    case AL_SER_PORT_NMEA:
    {
      common_data_ptr = (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.nmea_obex_ptr;

      break;
    }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
    {
      common_data_ptr = (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.ser3_obex_ptr;
      break;
    }
#endif /* FEATURE_HS_USB_SER3_PORT */
    default:
    {
      HSU_MSG_ERROR("hsu_al_ser_transmit: Port: %d not supported", 
        port_id, 0, 0);
      HSU_ASSERT((uint32)"hsu_al_ser_transmit: Invalid port" == NULL);
    }
  }

  HSU_ASSERT(common_data_ptr != NULL);

  hsu_al_ser_base_transmit(common_data_ptr, tx_ptr);
}

/*===========================================================================
FUNCTION hsu_al_ser_flush_tx()
===========================================================================*/
void hsu_al_ser_flush_tx
( 
  sio_stream_id_type stream_id,
  sio_port_id_type port_id,
  sio_vv_func_ptr_type flush_tx_done_cb
)
{
  HSU_USE_PARAM(stream_id);

  switch (port_id)
  {
    case AL_SER_PORT_MDM:
    {
     
      /* Save original App callback: */
      hsu_al_ser_static_data.mdm_flush_tx_cb = flush_tx_done_cb;
      
      hsu_al_ser_base_flush_tx(
        (hsu_al_ser_base_data_for_ser_if*)&(hsu_al_ser_static_data.mdm_acm_data), 
         al_ser_on_flush_tx);

      break;
    }
    case AL_SER_PORT_DIAG:
    {
      if (flush_tx_done_cb != NULL)
      {
        /* Save original App callback - if not NULL */
        add_app_flush_tx_cb_for_obex(hsu_al_ser_static_data.diag_obex_ptr, flush_tx_done_cb);
      }
      hsu_al_ser_base_flush_tx((hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.diag_obex_ptr, 
                                al_ser_on_flush_tx);
      
      break;
    }
#ifdef FEATURE_HS_USB_NMEA
    case AL_SER_PORT_NMEA:
    {
      if (flush_tx_done_cb != NULL)
      {
        /* Save original App callback: */
        add_app_flush_tx_cb_for_obex(hsu_al_ser_static_data.nmea_obex_ptr, flush_tx_done_cb);
      }
      hsu_al_ser_base_flush_tx((hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.nmea_obex_ptr, 
                                al_ser_on_flush_tx);
      
      break;
    }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
      {
        if (flush_tx_done_cb != NULL)
        {
          /* Save original App callback: */
          add_app_flush_tx_cb_for_obex(hsu_al_ser_static_data.ser3_obex_ptr, flush_tx_done_cb);
        }
        hsu_al_ser_base_flush_tx((hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.ser3_obex_ptr,
                                  al_ser_on_flush_tx);
      
        break;
      }
#endif /* FEATURE_HS_USB_SER3_PORT */
    default:
    {
      HSU_MSG_ERROR("hsu_al_ser_flush_tx: Port: %d not supported", 
        port_id, 0, 0);
      HSU_ASSERT((uint32)"hsu_al_ser_flush_tx: Invalid port" == NULL);
    }
  }
}

/*===========================================================================
FUNCTION hsu_al_ser_ioctl()
===========================================================================*/
void hsu_al_ser_ioctl
(
  sio_stream_id_type stream_id,
  sio_port_id_type port_id,
  sio_ioctl_cmd_type ioctl_cmd,
  sio_ioctl_param_type* ioctl_param_ptr
)
{
  hsu_al_ser_obex_data* obex_data_ptr = NULL;

  HSU_USE_PARAM(stream_id);

  switch (port_id)
  {
    case AL_SER_PORT_MDM:
    {
      if (hsu_al_ser_acm_ioctl(&(hsu_al_ser_static_data.mdm_acm_data),
        ioctl_cmd, ioctl_param_ptr) == FALSE)
      {
        HSU_MSG_ERROR("hsu_al_ser_ioctl: hsu_al_ser_acm_ioctl returned FALSE", 
          0, 0, 0);
        HSU_ASSERT(
          (uint32)"hsu_al_ser_ioctl failed" == NULL);
      }
      return;
    }
    case AL_SER_PORT_DIAG:
    {
      obex_data_ptr = hsu_al_ser_static_data.diag_obex_ptr;
      break;
    }
#ifdef FEATURE_HS_USB_NMEA
    case AL_SER_PORT_NMEA:
    {
      obex_data_ptr = hsu_al_ser_static_data.nmea_obex_ptr;
      break;
    }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
      {
        obex_data_ptr = hsu_al_ser_static_data.ser3_obex_ptr;
        break;
      }
#endif /* FEATURE_HS_USB_SER3_PORT */
    default:
    {
      HSU_MSG_ERROR("hsu_al_ser_ioctl: Port: %d not supported", 
        port_id, 0, 0);
      HSU_ASSERT((uint32)"Invalid port" == NULL);
    }
  }

  HSU_ASSERT(obex_data_ptr != NULL);

  /* TODO: How to verify if an I/O Ctl indeed came from an SMD? 
  Because, otherwise commands such as ASSERT_CD are an error.*/
  if (hsu_al_ser_obex_ioctl(obex_data_ptr, ioctl_cmd, ioctl_param_ptr) == FALSE)
  {
    HSU_MSG_ERROR("hsu_al_ser_ioctl: Port: %d does not support I/O ctl: %u", port_id, 
      ioctl_cmd, 0);
  }
}

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR CDC FDs (acm_fd_api.h & obex_fd_api.h)

==============================================================================*/
/*==============================================================================

FUNCTION      GET_AND_ALLOC_ACM_INSTANCE_INFO_FOR_FD

DESCRIPTION   
Called from acm_init, to find an instance_info_ptr to be used for the specific 
FD instance. 
Currently, since only one instance of the modem driver is supported, the 
function simply returns the address of hsu_al_ser_static_data.mdm_acm_data.
In the future, this implementation will have to change. 

DEPENDENCIES  
None.

RETURN VALUE  
A pointer to an ecm_instance_info.

SIDE EFFECTS  
None.

==============================================================================*/
static hsu_al_ser_acm_data* get_and_alloc_acm_instance_info_for_fd
(
 cdc_handle_t      handle 
 /* handle to this instance of the FD, to be used in further API calls */
)
{
  /* In multi configuration, we use the same instance for all the configurations.
  Therefore, we want to keep the handle for the first instance only.
  In case a different instance of ACM should be used for each instance,
  this implementation should be changed to keep the handle for each instance. */
  if (hsu_al_ser_static_data.num_of_initialized_acm_instances == 0)
  {
    /* Mark slot as belonging to this ACM FD handle */
    hsu_al_ser_static_data.mdm_acm_data.base_data.instance_of_fd_used = handle;

  }

  /* We save the handle of all the FDs that the stack initialized. 
  When the configuration will be changed we will use the corresponding handle 
  for the stack's FD. */
  hsu_al_ser_static_data.acm_fd_handle[HSU_CORE_IDX_PRIMARY][hsu_al_ser_static_data.num_of_initialized_acm_instances] = handle;

  ++hsu_al_ser_static_data.num_of_initialized_acm_instances;

  return &(hsu_al_ser_static_data.mdm_acm_data);
}


/*===========================================================================
FUNCTION obex_init()
===========================================================================*/
jresult_t obex_init
(
  cdc_handle_t handle, 
  obex_callbacks_t* cbs, 
  obex_init_info_t* info, 
  cdc_appctx_t* ctx
)
{
  hsu_al_ser_obex_data* obex_data_instance_ptr = NULL;

  HSU_MSG_HIGH("obex_init: entered", 0, 0, 0);

  /* Make sure data structs are valid, prior to working on them: */
  init_obex_data_arrays_once();

  if (handle == NULL || cbs == NULL || info == NULL || ctx == NULL)
  {
    HSU_MSG_ERROR("obex_init: At least one of the args is invalid:\
                  handle=0x%x, cbs=0x%x, info=0x%x", 
                  handle, cbs, info);
    HSU_MSG_ERROR("obex_init: At least one of the args is invalid (cont): ctx=0x%x",
      ctx, 0, 0);
    return JEINVAL;
  }

  /* Allocate a data slot for this OBEX FD instance */
  obex_data_instance_ptr = get_avail_obex_data_slot_for_fd(handle);

  /* Must not happen: There should be enough slots to begin with... */
  HSU_ASSERT(obex_data_instance_ptr != NULL);

  /* Let hsu_al_ser_obex fill the data, as well as marking obex_data_instance_ptr 
  as belonging to handle. */
  hsu_al_ser_obex_populate_obex_specific_data(obex_data_instance_ptr, handle, 
    cbs, info, ctx, (const uint8**)hsu_al_ser_static_data.num_of_initialized_obex_instances);

  HSU_MSG_HIGH("obex_init: exited", 0, 0, 0);

  return HSU_JSUCCESS;
}

/*===========================================================================
FUNCTION acm_init()
===========================================================================*/
jresult_t acm_init
(
  cdc_handle_t handle, 
  acm_callbacks_t* cbs, 
  acm_init_info_t* info, 
  cdc_appctx_t* ctx
)
{
  hsu_al_ser_acm_data* acm_data_instance_ptr = NULL;
  
  acm_data_instance_ptr = get_and_alloc_acm_instance_info_for_fd(handle);

  /* Make sure data struct is valid, prior to working on it: */
  init_acm_data_once();

  if (handle == NULL || cbs == NULL || info == NULL || ctx == NULL)
  {
    HSU_MSG_ERROR("acm_init: At least one of the args is invalid:\
                  handle=0x%x, cbs=0x%x, info=0x%x", 
                  handle, cbs, info);
    HSU_MSG_ERROR("acm_init: At least one of the args is invalid (cont): ctx=0x%x",
      ctx, 0, 0);
    return JEINVAL;
  }

  /* Let hsu_al_ser_acm fill the data, as well as marking acm_data_instance_ptr 
  as belonging to handle. */
  hsu_al_ser_acm_populate_acm_specific_data(acm_data_instance_ptr, handle, 
    cbs, info, ctx);

  return HSU_JSUCCESS;
}

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE INTERNAL HEADER FILE (hsu_al_ser_i.h)

==============================================================================*/

/* TODO: Need to guard alloc/free with crit section, due to the following scenario: */
/* App X called transmit, DMOV worked on the chain and the FD sends to host. */
/* The same app calls transmit yet again - wants to allocate buffer, 
but at the same time, The FD calls Tx done callback, which wants to free the buffer */

/*===========================================================================
FUNCTION hsu_al_ser_i_alloc_buffer_for_read_write()
===========================================================================*/
void* hsu_al_ser_i_alloc_buffer_for_read_write
(
  byte* mem_pool_ptr, 
  juint32_t buff_size  
)
{
  boolean mem_pool_found = FALSE;
  HSU_ASSERT(mem_pool_ptr != NULL);

  /* For now, just give the entire buffer... */
  if ((mem_pool_ptr == hsu_al_ser_static_data.mem_pool_for_mdm_rx) || 
    (mem_pool_ptr == hsu_al_ser_static_data.mem_pool_for_mdm_tx))
  {
    mem_pool_found = TRUE;
    HSU_ASSERT(buff_size <= SIZE_OF_MDM_MEM_POOL_IN_BYTES);
  }

  if ((mem_pool_ptr == hsu_al_ser_static_data.mem_pool_for_diag_rx) || 
    (mem_pool_ptr == hsu_al_ser_static_data.mem_pool_for_diag_tx))
  {
    mem_pool_found = TRUE;
    HSU_ASSERT(buff_size <= SIZE_OF_DIAG_MEM_POOL_IN_BYTES);
  }

#ifdef FEATURE_HS_USB_NMEA
  if ((mem_pool_ptr == hsu_al_ser_static_data.mem_pool_for_nmea_tx) ||
    (mem_pool_ptr == hsu_al_ser_static_data.mem_pool_for_nmea_rx))
  {
    mem_pool_found = TRUE;
    HSU_ASSERT(buff_size <= SIZE_OF_NMEA_MEM_POOL_IN_BYTES);
  }
#endif /* FEATURE_HS_USB_NMEA */

#ifdef FEATURE_HS_USB_SER3_PORT
  if ((mem_pool_ptr == hsu_al_ser_static_data.mem_pool_for_ser3_tx) ||
     (mem_pool_ptr == hsu_al_ser_static_data.mem_pool_for_ser3_rx))
  {
    mem_pool_found = TRUE;
    HSU_ASSERT(buff_size <= SIZE_OF_USB_SER3_MEM_POOL_IN_BYTES);
  }
#endif /* FEATURE_HS_USB_SER3_PORT */
  /* Make sure this mem. pool matches one of the above pools. */
  HSU_ASSERT(mem_pool_found);

  return mem_pool_ptr;
}

/*===========================================================================
FUNCTION hsu_al_ser_i_alloc_buffer_for_read_write()
===========================================================================*/
void hsu_al_ser_i_free_buffer_from_read_write
(
  byte* mem_pool_ptr, 
  void* buffer_ptr
)
{
  /* For now, do nothing */
  HSU_PARAM_COULD_BE_CONST(mem_pool_ptr);
  HSU_PARAM_COULD_BE_CONST(buffer_ptr);

  HSU_ASSERT(mem_pool_ptr != NULL);
  HSU_ASSERT(buffer_ptr != NULL);
}

#if 0
static void* alloc_buffer_for_read_write(int if_num, int32 buff_size)
{
  void* ret_val = NULL;

  if ((if_num < MAX_NUM_OF_SER_IFS) 
    && (buff_size <= SIZE_OF_MEM_POOL_IN_BYTES))
  {
    /*
    BEGIN CRIT SECTION
    */
    rex_enter_crit_sect(&(hsu_al_ser_static_data.crit_sect_for_mem_pool[if_num]));

    /*
    Reason for CRIT SECTION: for the same I/F, there could be an Rx operation INT at the same time as the DS Task is Tx'ing
    */
    if (hsu_al_ser_static_data.mem_pool_for_cdc_is_used[if_num] == FALSE)
    {
      hsu_al_ser_static_data.mem_pool_for_cdc_is_used[if_num] = TRUE;

      ret_val = (void*)(hsu_al_ser_static_data.mem_pool_for_cdc[if_num]);
    }
    /*
    END CRIT SECTION
    */
    rex_leave_crit_sect(&(hsu_al_ser_static_data.crit_sect_for_mem_pool[if_num]));
  }

  return ret_val;
}

static void free_buffer_from_read_write(int if_num, void* buffer)
{
  if ((if_num < MAX_NUM_OF_SER_IFS) && (buffer != NULL))
  {
    /*
    BEGIN CRIT SECTION
    */
    rex_enter_crit_sect(&(hsu_al_ser_static_data.crit_sect_for_mem_pool[if_num]));

    hsu_al_ser_static_data.mem_pool_for_cdc_is_used[if_num] = FALSE;
    /*
    END CRIT SECTION
    */
    rex_leave_crit_sect(&(hsu_al_ser_static_data.crit_sect_for_mem_pool[if_num]));
  }
}
#endif /* 0 */


/*=============================================================================
PUBLIC FUNCTION DEFINITIONS FOR MODULE UNIT TESTS (ut)
=============================================================================*/
#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* defined(FEATURE_HSU_TEST) && !defined(T_REXNT) */

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */


/*==============================================================================

INTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================

FUNCTION get_avail_obex_data_slot_for_fd

DESCRIPTION
Called whenever obex_init is called. Allows association between OBEX FD and
a data slot. 
In order to mark the returned value us unavailable, the caller is responsible 
for setting the field: instance_of_fd_used. Once the caller has done that,
it "owns" this slot (so if another obex_init is called, the other caller 
will not use that slot).

DEPENDENCIES
None

RETURN VALUE
hsu_al_ser_obex_data* - Pointer to the next available struct.
NULL - if no more available structs.

SIDE EFFECTS
None

===========================================================================*/
static hsu_al_ser_obex_data* get_avail_obex_data_slot_for_fd
(
 cdc_handle_t handle
)
{
  hsu_al_ser_obex_data* returned_value = NULL;
#ifndef FEATURE_HSU_TEST
  core_t *core = ((cdc_ctx_t*)handle)->core_ctx;
  uint8 cur_core_index = core->core_number;
  uint8 cur_config_index = core->curr_config;
#else
#error code not present
#endif /* FEATURE_HSU_TEST */
 
  uint8 cur_obex_user;

  cur_obex_user = hsu_al_ser_static_data.
    num_of_initialized_obex_instances[cur_core_index][cur_config_index-1];

  /* The same data_for_apps_using_obex_if is used for all the configurations, 
     so we assume that the same obex interface number is used in all the
     configurations. For example - DIAG is always first etc. 
     An example for a case that won't work will be:
     Config 1 - Diag, NMEA, Ser3, Config 2 - only NMEA. In this case we will use
     data_for_apps_using_obex_if of DIAG of the first config for NMEA of the second 
     config and we assume that the same application uses this instance (we have a port 
     open with DIAG and we didn't open a new port for NMEA when setting to config 2). */
  returned_value = 
    &(hsu_al_ser_static_data.data_for_apps_using_obex_if[cur_core_index][cur_obex_user]);

  /* We save the handle for all the stack's initialized FDs. 
     When the configuration will be changed we will use the corresponding handle 
     for the stack's FD */
  hsu_al_ser_static_data.obex_fd_handle[cur_core_index][cur_config_index-1][cur_obex_user] = handle;

  /* We set only the handle of the first configuration. If a different configuration  
    will be set we will switch to the corresponding handle for the stack's FD */
  returned_value->base_data.instance_of_fd_used = handle;
  returned_value->obex_if_data.core_idx = cur_core_index;
  ++hsu_al_ser_static_data.
    num_of_initialized_obex_instances[cur_core_index][cur_config_index-1];

  return returned_value;
}

/*===========================================================================

FUNCTION get_and_alloc_obex_data_slot_for_app

DESCRIPTION

Called whenever hsu_al_ser_open is called. 
Allows association between application (DIAG or NMEA) and a data slot. 
The returned value will be used by the adaptation layer to represent the app 
which is using the OBEX FD and called hsu_al_ser_open.

DEPENDENCIES
None

RETURN VALUE
hsu_al_ser_obex_data* - Pointer to the next available struct.
NULL - if no more available structs.

SIDE EFFECTS
obex_slot_avail_for_app[MATCHING_INDEX][MATCHING_INDEX] is set to FALSE

===========================================================================*/
static hsu_al_ser_obex_data* get_and_alloc_obex_data_slot_for_app
(
 sio_port_id_type port_id /* SIO port id representing the app using the
                          OBEX FD */
)
{
  uint8 obex_data_slot_index;
  uint8 core_index;
  hsu_al_ser_obex_data* returned_value = NULL;

  obex_data_slot_index = al_ser_get_obex_slot_idx_for_sio_port(port_id, &core_index);
  if (hsu_al_ser_static_data.obex_slot_avail_for_app[core_index][obex_data_slot_index] == FALSE)
  {
    HSU_MSG_ERROR("get_and_alloc_obex_data_slot_for_app: already allocated for"
      "port_id: %d", port_id, 0, 0);
    return NULL;
  }

  hsu_al_ser_static_data.obex_slot_avail_for_app[core_index][obex_data_slot_index] = FALSE;
  returned_value = 
    &(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]);

  return returned_value;
}

/*===========================================================================

FUNCTION release_obex_data_slot_from_app

DESCRIPTION
Typically called whenever hsu_al_ser_close is called, or serious error encountered.
This will set the given hsu_al_ser_obex_data entry as available 
(obex_slot_avail_for_app[MATCHING_INDEX][MATCHING_INDEX] = TRUE).

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
obex_slot_avail_for_app[MATCHING_INDEX][MATCHING_INDEX] is set to TRUE

===========================================================================*/
static void release_obex_data_slot_from_app
(
  hsu_al_ser_obex_data* data_for_obex_if_ptr 
    /* Pointer to OBEX data structure to release */
)
{
  boolean found_match_for_if_ptr = FALSE;
  int obex_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_obex_if_ptr != NULL);

  /* No need for critical section, even if the following scenario can occur:
  DIAG calls hsu_al_ser_close and in the middle, NMEA calls hsu_al_ser_open.
  Then this is only a boolean 
  (doesn't matter which of them checked it first or set it to TRUE).
  */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    core_index = data_for_obex_if_ptr->obex_if_data.core_idx;

    if (data_for_obex_if_ptr ==
      &(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]))
    {
      hsu_al_ser_static_data.obex_slot_avail_for_app[core_index][obex_data_slot_index] = TRUE;
      
      found_match_for_if_ptr = TRUE;
      break;
    }
  }

  if (found_match_for_if_ptr == FALSE)
  {
    ERR("release_obex_data_slot_from_app: No match for 0x%x", 
      data_for_obex_if_ptr, 0, 0);
    HSU_ASSERT((uint32)"No match for if_ptr" == NULL);
  }
}

/*===========================================================================

FUNCTION init_obex_data_arrays_once

DESCRIPTION
Sets the OBEX data structs to default values. This process is done only once, 
and may be invoked either from hsu_al_ser_init or the very first call to 
obex_init (whichever comes first).

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void init_obex_data_arrays_once
(
  void
)
{
  int obex_data_slot_index;
  uint8 core_idx = 0;
  uint8 config_idx = 0;

  static boolean performed_init_of_obex_slots = FALSE;

  /* Make sure this is done only once (step 0). 
  In case the stack goes down, uninit callbacks will be called, 
  so when the stack goes back up - the values are as if init was called. */
  if (performed_init_of_obex_slots == TRUE)
  {
    HSU_MSG_HIGH("init_obex_data_arrays_once: was already called. Doing nothing...", 
      0, 0, 0);
    return ;
  }

  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for(core_idx = 0 ; core_idx < HSU_MAX_NUM_OF_CORES; ++core_idx)
    {
      hsu_al_ser_static_data.obex_slot_avail_for_app[core_idx][obex_data_slot_index] = TRUE;

      hsu_al_ser_obex_init(
        &(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_idx][obex_data_slot_index]));

      hsu_al_ser_static_data.app_close_cb_arr_for_obex[core_idx][obex_data_slot_index] = NULL;
      hsu_al_ser_static_data.app_flush_tx_cb_arr_for_obex[core_idx][obex_data_slot_index] = NULL;
    }
  }

  /* hsu_al_ser_open will do the "real" binding between OBEX instance and app */
  hsu_al_ser_static_data.diag_obex_ptr = NULL;
#ifdef FEATURE_HS_USB_NMEA
  hsu_al_ser_static_data.nmea_obex_ptr = NULL;
#endif /* FEATURE_HS_USB_NMEA */

#ifdef FEATURE_HS_USB_SER3_PORT
  hsu_al_ser_static_data.ser3_obex_ptr = NULL;
#endif /* FEATURE_HS_USB_SER3_PORT */

  for(core_idx = 0 ; core_idx < HSU_MAX_NUM_OF_CORES; ++core_idx)
  {
    for(config_idx = 0 ; config_idx < HSU_MAX_NUM_OF_CONFIGS; ++config_idx)
    {
      hsu_al_ser_static_data.num_of_initialized_obex_instances[core_idx][config_idx] = 0;
    }
  }

  memset(hsu_al_ser_static_data.obex_fd_handle, 0, sizeof (hsu_al_ser_static_data.obex_fd_handle));
  performed_init_of_obex_slots = TRUE;
}

/*===========================================================================

FUNCTION init_acm_data_once

DESCRIPTION
Sets the ACM data struct to default values. This process is done only once, 
and may be invoked either from hsu_al_ser_init or acm_init (whichever comes first).

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void init_acm_data_once
(
  void
)
{
  static boolean performed_init_of_acm_data = FALSE;

  /* Make sure this is done only once (step 0). 
  In case the stack goes down, uninit callbacks will be called, 
  so when the stack goes back up - the values are as if init was called. */
  if (performed_init_of_acm_data == TRUE)
  {
    HSU_MSG_HIGH("init_acm_data_once: was already called. Doing nothing...", 
      0, 0, 0);
    return ;
  }

  hsu_al_ser_acm_init(&(hsu_al_ser_static_data.mdm_acm_data));

  hsu_al_ser_static_data.mdm_flush_tx_cb = NULL;
  hsu_al_ser_static_data.mdm_close_cb = NULL;

  hsu_al_ser_static_data.num_of_initialized_acm_instances = 0;
  memset(hsu_al_ser_static_data.acm_fd_handle, 0, sizeof (hsu_al_ser_static_data.acm_fd_handle));

  performed_init_of_acm_data = TRUE;
}

/*===========================================================================

FUNCTION add_app_close_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, adds the supplied callback which will
be invoked when the supplied data struct had been freed from the app.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void add_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr,
             /* Data structure, which the supplied callback corresponds to */
  sio_vv_func_ptr_type app_close_cb
                                                  /* App supplied callback */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  /* Assuming caller made sure the close callback isn't NULL */
  HSU_ASSERT((data_for_ser_if_ptr != NULL) && (app_close_cb != NULL));

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {

      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_close_cb_arr_for_obex[core_index][obex_data_slot_index] = app_close_cb;
        return;
      }
    }
  }
  HSU_ASSERT((uint32)"add_app_close_cb_for_obex: couldn't find slot" == NULL);
}

/*===========================================================================

FUNCTION get_app_close_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, returns the supplied callback which will
be invoked when the supplied data struct had been freed from the app.

DEPENDENCIES
None

RETURN VALUE
The app close callback, corresponding to the given OBEX/ACM data struct.

SIDE EFFECTS
None

===========================================================================*/
static sio_vv_func_ptr_type get_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        return hsu_al_ser_static_data.app_close_cb_arr_for_obex[core_index][obex_data_slot_index];
      }
    }
  }
  HSU_ASSERT((uint32)"get_app_close_cb_for_obex: couldn't find slot" == NULL);
  return NULL;
}

/*===========================================================================

FUNCTION remove_app_close_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, removes the stored callback which 
would have been invoked when the supplied data struct had been freed from 
the app.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void remove_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_close_cb_arr_for_obex[core_index][obex_data_slot_index] = NULL;
        return;
      }
    }
  }
  HSU_ASSERT((uint32)"remove_app_close_cb_for_obex: couldn't find slot" == NULL);
}

/*===========================================================================

FUNCTION add_app_flush_tx_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, adds the supplied callback which will
be invoked when the supplied data struct has transmitted all pending data.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void add_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr,
             /* Data structure, which the supplied callback corresponds to */
  sio_vv_func_ptr_type app_flush_tx_cb
                                                  /* App supplied callback */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_flush_tx_cb_arr_for_obex[core_index][obex_data_slot_index] = app_flush_tx_cb;
        return;
      }
    }
  }
  HSU_ASSERT((uint32)"add_app_flush_tx_cb_for_obex: couldn't find slot" == NULL);
}

/*===========================================================================

FUNCTION get_app_flush_tx_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, returns the supplied callback which will
be invoked when the supplied data struct has transmitted all pending data.

DEPENDENCIES
None

RETURN VALUE
The app flush tx callback, corresponding to the given OBEX/ACM data struct.

SIDE EFFECTS
None

===========================================================================*/
static sio_vv_func_ptr_type get_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < MAX_NUM_OF_OBEX_USERS ; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        return hsu_al_ser_static_data.app_flush_tx_cb_arr_for_obex[core_index][obex_data_slot_index];
      }
    }
  }
  HSU_ASSERT((uint32)"get_app_flush_tx_cb_for_obex: couldn't find slot" == NULL);
  return NULL;
}

/*===========================================================================

FUNCTION remove_app_flush_tx_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, removes the stored callback which 
would have been invoked when the supplied data struct has transmitted all 
pending data.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void remove_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < MAX_NUM_OF_OBEX_USERS ; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_flush_tx_cb_arr_for_obex[core_index][obex_data_slot_index] = NULL;
        return;
      }
    }
  }
  HSU_ASSERT((uint32)"remove_app_flush_tx_cb_for_obex: couldn't find slot" == NULL);
}

/*===========================================================================

FUNCTION al_ser_on_close

DESCRIPTION
Serves as an internal close callback for the OBEX/ACM data structs.
Once invoked, will do all the needed cleanup and invoke the app close callback.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_on_close
(
  hsu_al_ser_base_data_for_ser_if* data_for_ser_if_ptr
                   /* Data structure, which this callback was invoked from */
)
{
  sio_vv_func_ptr_type app_close_cb = NULL;

  HSU_MSG_HIGH("al_ser_on_close: Entered", 0, 0, 0);

  /* Check that data_for_ser_if_ptr is a valid pointer */
  if ((data_for_ser_if_ptr == (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.diag_obex_ptr) 
#ifdef FEATURE_HS_USB_NMEA
    ||(data_for_ser_if_ptr == (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.nmea_obex_ptr)
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    ||(data_for_ser_if_ptr == (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.ser3_obex_ptr)
#endif /* FEATURE_HS_USB_SER3_PORT */
    )
  {
    release_obex_data_slot_from_app((hsu_al_ser_obex_data*)data_for_ser_if_ptr);

    /* Finally call app callback: */
    app_close_cb = get_app_close_cb_for_obex((hsu_al_ser_obex_data*)data_for_ser_if_ptr);
    remove_app_close_cb_for_obex((hsu_al_ser_obex_data*)data_for_ser_if_ptr);
  }
  else if (data_for_ser_if_ptr == 
    (hsu_al_ser_base_data_for_ser_if*)&(hsu_al_ser_static_data.mdm_acm_data))
  {
    /* Release mdm acm from app */
    hsu_al_ser_static_data.mdm_held_by_app = FALSE;

    app_close_cb = hsu_al_ser_static_data.mdm_close_cb;
    hsu_al_ser_static_data.mdm_close_cb = NULL;
  }
  else
  {
    HSU_ASSERT((uint32)"al_ser_on_close: no match" == NULL);
  }

  if (app_close_cb != NULL)
  {
    app_close_cb();
  }

  HSU_MSG_HIGH("al_ser_on_close: Exit Successfully", 0, 0, 0);
}

/*===========================================================================

FUNCTION al_ser_on_flush_tx

DESCRIPTION
Serves as an internal flush tx callback for the OBEX/ACM data structs.
Once invoked, will invoke the app flush tx callback.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_on_flush_tx
(
  hsu_al_ser_base_data_for_ser_if* data_for_ser_if_ptr
                   /* Data structure, which this callback was invoked from */
)
{
  sio_vv_func_ptr_type app_flush_tx_cb = NULL;

  HSU_PARAM_COULD_BE_CONST(data_for_ser_if_ptr);

  /* Check that data_for_ser_if_ptr is a valid pointer */
  if ((data_for_ser_if_ptr == (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.diag_obex_ptr) 
#ifdef FEATURE_HS_USB_NMEA
    ||(data_for_ser_if_ptr == (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.nmea_obex_ptr)
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    ||(data_for_ser_if_ptr == (hsu_al_ser_base_data_for_ser_if*)hsu_al_ser_static_data.ser3_obex_ptr)
#endif /* FEATURE_HS_USB_SER3_PORT */
    )
  {
    app_flush_tx_cb = get_app_flush_tx_cb_for_obex((hsu_al_ser_obex_data*)data_for_ser_if_ptr);
    if (app_flush_tx_cb != NULL)
    {
      remove_app_flush_tx_cb_for_obex((hsu_al_ser_obex_data*)data_for_ser_if_ptr);
    }

  }
  else if (data_for_ser_if_ptr == 
    (hsu_al_ser_base_data_for_ser_if*)&(hsu_al_ser_static_data.mdm_acm_data))
  {
    app_flush_tx_cb = hsu_al_ser_static_data.mdm_flush_tx_cb;
    hsu_al_ser_static_data.mdm_flush_tx_cb = NULL;
  }
  else
  {
    HSU_ASSERT((uint32)"al_ser_on_flush_tx: no match" == NULL);
  }

  if (app_flush_tx_cb !=NULL)
  {
    /* Finally call app callback: */
    app_flush_tx_cb();
  }
}

/*===========================================================================

FUNCTION data_ptr_freed_from_app

DESCRIPTION
Checks if this data pointer is no longer taken by an app, 
used by hsu_al_ser_close, to determine whether or not to continue rex_sleep 
(when a callback is not supplied).

DEPENDENCIES
None

RETURN VALUE
TRUE - Data ptr no longer held by app. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
static boolean data_ptr_is_freed_from_app
(
  const hsu_al_ser_base_data_for_ser_if* data_if_ptr
                                                /* Data structure to check */
)
{
  int obex_index = 0;
  uint8 core_index;

  HSU_ASSERT(data_if_ptr != NULL);

  for (obex_index = 0 ; obex_index < MAX_NUM_OF_OBEX_USERS ; ++obex_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; ++core_index)
    {
      if (data_if_ptr == 
        (hsu_al_ser_base_data_for_ser_if*)(&hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_index]))
      {
        if (hsu_al_ser_static_data.obex_slot_avail_for_app[core_index][obex_index] == TRUE)
        {
          return TRUE;
        }
        /* No point in continuing the search*/
        return FALSE;
      }
    }
  }

  if (data_if_ptr == 
    (hsu_al_ser_base_data_for_ser_if*)&(hsu_al_ser_static_data.mdm_acm_data))
  {
    if (hsu_al_ser_static_data.mdm_held_by_app == FALSE)
    {
      return TRUE;
    }
    return FALSE;
  }
  
  /* No match found */
  HSU_ASSERT((uint32)"No match found" == NULL);
  
  /* (To eliminate VC++ warning)*/
  return FALSE;
}

/*===========================================================================

FUNCTION al_ser_get_obex_slot_idx_for_sio_port

DESCRIPTION
Returns the index of the proper OBEX slot for the given port. 
It is assumed that port is using OBEX FD.

DEPENDENCIES
None

RETURN VALUE
Index corresponding to the OBEX slot for the given SIO port ID.

SIDE EFFECTS
None

===========================================================================*/
static uint8 al_ser_get_obex_slot_idx_for_sio_port
(
  sio_port_id_type port_id, /* SIO port id representing the app using the
                           OBEX FD */
  uint8* core_index        /* The core index for this SIO port */         
)
{
  *core_index = HSU_CORE_IDX_PRIMARY;

  /* For now, implementing a hard coded switch case */
  switch (port_id)
  {
    case AL_SER_PORT_DIAG:
    {
#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) && defined(FEATURE_HS_USB_LEGACY_DEVICE)
      if (hsu_conf_sel_diag_is_on_legacy_port())
      {
        *core_index = HSU_CORE_IDX_SECONDARY;
      }
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE && FEATURE_HS_USB_LEGACY_DEVICE */
      return 0;
    }
    case AL_SER_PORT_NMEA:
    {
      return 1;
    }
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
    {
      return 2;
    }
#endif /* FEATURE_HS_USB_SER3_PORT */
    default: 
    {
      HSU_MSG_ERROR("al_ser_get_obex_slot_idx_for_sio_port: Port: %d not supported", 
        port_id, 0, 0);      
      HSU_ASSERT((uint32)"port not supported" == NULL);

      return MAX_NUM_OF_OBEX_USERS;
    }
  }
}

#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND
/*===========================================================================
FUNCTION hsu_al_ser_enable_byte_pad()
NOTE:
Byte-pad is enabled for all serial instances! 
===========================================================================*/
void hsu_al_ser_enable_byte_pad(void)
{
  int indx = 0;
  uint8 core_index;

  HSU_MSG_HIGH("*_enable_byte_pad: Entered", 0, 0, 0);
  
  /* Update boolean indicating the host supports overcomming HW 1/2 byte remainder bug */
  if (hsu_al_ser_static_data.mdm_acm_data.base_data.instance_enabled_by_cdc_fd)
  {
    hsu_al_ser_static_data.mdm_acm_data.base_data.overcome_1_2_byte_hw_crc_err = TRUE;
  }

  for (indx = 0; indx < MAX_NUM_OF_OBEX_USERS; indx++)
  {
    for (core_index = 0; core_index < HSU_MAX_NUM_OF_CORES; core_index++)
    {
      if (hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][indx].base_data.instance_enabled_by_cdc_fd)
      {
        hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][indx].base_data.overcome_1_2_byte_hw_crc_err = TRUE;
      }
    }
  }
  HSU_MSG_LOW("*_enable_byte_pad: Exited", 0, 0, 0);
}

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */

#ifndef T_REXNT
/*==============================================================================

FUNCTION          HSU_AL_SER_HANDLE_SET_CONFIGURATION

DESCRIPTION
This function is called when the enabled callback is received.
This function sets the FD handle according to the current configuration FD, for
a given core_index.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Note that data_for_apps_using_obex_if of the first configuration is used for all 
configurations, and that its variables are not cleared when switching to a new 
configuration.

==============================================================================*/

void hsu_al_ser_handle_set_configuration(uint8 core_index)
{
  uint8 obex_users_idx = 0;
  uint8 config_idx = 0;

  config_idx = core_get_curr_configuration(core_index);
  HSU_MSG_LOW("hsu_al_ser_handle_set_configuration(): config_idx = %d.", config_idx, 0, 0);

  if ((config_idx == 0) || (config_idx == hsu_al_ser_static_data.curr_config_idx))
  {
    HSU_MSG_LOW("hsu_al_ser_handle_set_configuration(): config_idx = %d, nothing to do.", 
      config_idx, 0, 0);
    return;
  }

  /* Set modem FD instance according to new configuration */
  hsu_al_ser_static_data.mdm_acm_data.base_data.instance_of_fd_used =
    hsu_al_ser_static_data.acm_fd_handle[core_index][config_idx-1];

  /* Set OBEX FD instances according to new configuration */
  for (obex_users_idx = 0; obex_users_idx < MAX_NUM_OF_OBEX_USERS; ++obex_users_idx)
  {
    hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_users_idx].base_data.instance_of_fd_used = 
      hsu_al_ser_static_data.obex_fd_handle[core_index][config_idx-1][obex_users_idx];
  }
  
  hsu_al_ser_static_data.curr_config_idx = config_idx;
}
#endif /* T_REXNT */

/*==============================================================================

FUNCTION          HSU_AL_SER_HANDLE_OBEX_UNINIT

DESCRIPTION
This function is called when OBEX CDC FD notifies it is being 
un-initialized

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_al_ser_handle_obex_uninit
( 
 cdc_appctx_t ctx                           /* Context passed in callback */
)
{
  hsu_al_ser_obex_data* obex_data_ptr = (hsu_al_ser_obex_data*)ctx;
  uint8 config_index;

  HSU_ASSERT(obex_data_ptr->obex_if_data.core_idx < HSU_MAX_NUM_OF_CORES);

  /* Assume that if this function is called for core_idx, for the FIRST configuration,
     it will be called for ALL configurations of that core_idx. So clean data structures 
     for this core_idx on the FIRST call only. */ 
  if (hsu_al_ser_static_data.num_of_initialized_obex_instances[obex_data_ptr->obex_if_data.core_idx][HSU_CONFIG_IDX_0] != 0)
  {
    for (config_index=0; config_index < HSU_MAX_NUM_OF_CONFIGS; config_index++)
    {
      hsu_al_ser_static_data.num_of_initialized_obex_instances[obex_data_ptr->obex_if_data.core_idx][config_index] = 0;
    }

    memset(hsu_al_ser_static_data.obex_fd_handle[obex_data_ptr->obex_if_data.core_idx], 
      0, sizeof (hsu_al_ser_static_data.obex_fd_handle) / HSU_MAX_NUM_OF_CORES);
  }
}

/*==============================================================================

FUNCTION          HSU_AL_SER_HANDLE_ACM_UNINIT

DESCRIPTION
This function is called when ACM CDC FD notifies it is being 
un-initialized

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_al_ser_handle_acm_uninit()
{
  if (hsu_al_ser_static_data.num_of_initialized_acm_instances > 0)
  {
    --hsu_al_ser_static_data.num_of_initialized_acm_instances;
  }
  if (hsu_al_ser_static_data.num_of_initialized_acm_instances == 0)
  {
    memset(hsu_al_ser_static_data.acm_fd_handle, 0, sizeof (hsu_al_ser_static_data.acm_fd_handle));
  }
}
