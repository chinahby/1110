/*=============================================================================
  
                R F  D r i v e r  A P I  L a y e r  G e n e r i c
                     I n t e r f a c e  D e f i n i t i o n

GENERAL DESCRIPTION
  This file provides the API layer of the RF driver.  It provides the
  RF driver API layer generic internal interface and accesses the services
  of the RF driver logical layer.  This generic interface is not directly
  exported to the upper layers 

EXTERNALIZED FUNCTIONS
  rfapi_init
  rfapi_get_device_descriptor
  rfapi_enter_mode
  rfapi_tune_to_chan
  rfapi_enable_tx
  rfapi_disable_tx
  rfapi_start_tx
  rfapi_stop_tx
  rfapi_is_sleeping
  rfapi_sleep
  rfapi_stay_asleep
  rfapi_wakeup
  rfapi_capture_loop_values
  rfapi_restore_loop_values
  rfapi_stop_loops
  rfapi_start_loops
  rfapi_do_tx_pwr_limit
  rfapi_do_temp_comp
  rfapi_set_tx_pwr_limit
  rfapi_get_rx_agc_val
  rfapi_get_tx_agc_val
  rfapi_log_agc
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  See rf.h file for initialization and sequencing requirements.

Copyright (c) 2000 - 2007  by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rfapi.c#2 $ 
  $DateTime: 2008/11/19 11:46:56 $ 
  $Author: lesp $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/14/08   lp      More Klocwork fixes
10/08/06   sr      Klocwork fixes
08/06/07   jfc     Featurized code for EDGE-only builds
05/17/07   ra      Fixed function index to rfapi_trigger_gps_bbc_update();
04/13/07   ad      Set rfllumts_set_primary_path() for Parked Mode 
04/11/07   zg      Fixed the function index bug in rfapi_exit_gps().
03/07/07   ra      Fixed compiler error
03/06/07   ra      Added rfllgps_exit_gps()
02/28/07   ycl     Fix compiler warnings.
02/05/07   ycl     Merge to 7200 tip.
01/23/07   ra      Added rfapi_trigger_gps_bbc_update()
01/11/07   ra      Added  RFAPI_EXIT_GPS()
12/18/06   jfc     Added rfapi_is_sleeping()
10/17/06   ycl     Modifications for initial MSM7600 build.
10/21/05   sar     Fixed Lint error.
10/20/05   sar     Added function rfapi_is_band_chan_supported().
05/25/05   sar     Added support for RFCOM_RECEIVER_DUAL device.
04/26/05   sar     Added support for rfapi_get_band_mask().
09/30/04   aks     Merge in Hidra PLT RF changes.
09/17/03   sar     Added api rfapi_get_state() to allow querying of the internal
                   state of the multimode rf api.
09/15/03   sar     Fixed a typo in rfapi_disable_rx() where rfapi_disable_rx_type
                   was erroneously written as rfapi_disable_tx_type.
03/27/03   rv      Added MM API for frequency maintenance algorithm in WCDMA.
03/05/03   eh      Added a state parameter in rf_api_init to indicate whether
                   RF NV is valid or not. Added code to update the state parameter.
11/04/02   sd      Added support for intersystem measurements.
10/10/02   sar     Included 1X function header file. Added 1X functions to the device_descriptor_table.
09/10/02   sd      Added support for multimode API that includes GSM drivers.
07/01/02   sd      Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "rex.h"

#include "rfapi.h"
#include "rfll.h"

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FEATURE_CDMA1X
#include "rfll1x.h"
#endif

#ifdef FEATURE_CGPS
#error code not present
#endif 

#include "assert.h"


//{{{TODO_7600
/*---------------------------------------------------------------------------
  Reference to device 0 (primary transceiver) descriptor.
---------------------------------------------------------------------------*/
rfapi_device_desc_type rfapi_dev0_ptr = NULL;
/*---------------------------------------------------------------------------
  Reference to device 1 (diversity receiver) descriptor.
---------------------------------------------------------------------------*/
rfapi_device_desc_type rfapi_dev1_ptr = NULL;

boolean rf_error_flag = FALSE;
//}}}

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*---------------------------------------------------------------------------
  Mode specific macros used for device table definition.
---------------------------------------------------------------------------*/

#ifdef FEATURE_WCDMA
#error code not present
#else
#define WCDMA( x )      NULL
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM
#error code not present
#else
#define GSM( x )        NULL
#endif /* FEATURE_GSM */

#ifdef FEATURE_CDMA1X
#define CDMA1X( x )      x
#else
#define CDMA1X( x )      NULL
#endif /* FEATURE_CDMA1X */

#ifdef FEATURE_CGPS
#error code not present
#else
#define GPS( x )      NULL
#endif /* FEATURE_CGPS */

/*---------------------------------------------------------------------------
  RF driver logical layer type definitions for handlers corresponding to
  each API function exported to the upper layers by the RF driver API.
---------------------------------------------------------------------------*/

/* 0. rfapi_init() */ 
typedef rfll_device_desc_type* (*rfapi_init_type)
(
  rfcom_device_enum_type,
  rex_tcb_type *,
  rex_sigs_type,
  void (*task_wait_func_ptr)( rex_sigs_type )
); 

/* 1. rfapi_get_band_mask() */ 
typedef sys_band_mask_type (*rfapi_get_band_mask_type)(void); 

/* 2. rfapi_is_band_chan_supported */
typedef boolean (*rfapi_is_band_chan_supported_type)
(
    rfll_device_desc_type *,
    sys_band_class_e_type band, 
    word chan
);

/* 3. rfapi_enter_mode() */
typedef uint32 (*rfapi_enter_mode_type)
(
  rfll_device_desc_type *,
  rfcom_mode_enum_type,
  const void *,
  rfcom_cb_handler_type
);

/* 4. rfapi_tune_to_chan() */
typedef uint32 (*rfapi_tune_to_chan_type)
(
  rfll_device_desc_type *,
  uint16,
  rfcom_synth_tune_enum_type,
  const void *,
  rfcom_cb_handler_type
);

/* 5. rfapi_enable_tx() */
typedef uint32 (*rfapi_enable_tx_type)
(
  rfll_device_desc_type *,
  const void *,
  rfcom_cb_handler_type
);

/* 6. rfapi_enable_rx() */
typedef uint32 (*rfapi_enable_rx_type)
(
  rfll_device_desc_type *,
  const void *,
  rfcom_cb_handler_type
);

/* 7. rfapi_disable_tx() */
typedef void (*rfapi_disable_tx_type)
(
  rfll_device_desc_type *
);

/* 8. rfapi_disable_rx() */
typedef void (*rfapi_disable_rx_type)
(
  rfll_device_desc_type *
);

/* 9. rfapi_start_tx() */
typedef void (*rfapi_start_tx_type)
(
  rfll_device_desc_type *
);

/* 10. rfapi_stop_tx() */
typedef void (*rfapi_stop_tx_type)
(
  rfll_device_desc_type *
);
  

//TODO_7600
/* 11. rfapi_sleep() */
typedef uint32 (*rfapi_sleep_type)
(
rfll_device_desc_type *,
const void *,
rfcom_cb_handler_type
);


/* 12. rfapi_stay_asleep() */
typedef void (*rfapi_stay_asleep_type)
(
  rfll_device_desc_type *
);
  
/* 13. rfapi_wakeup() */
typedef uint32 (*rfapi_wakeup_type)
(
  rfll_device_desc_type *,
  rfcom_mode_enum_type,
  const void *,
  rfcom_cb_handler_type
);

/* 14. rfapi_capture_loop_values() */
typedef void (*rfapi_capture_loop_values_type)
(
 rfll_device_desc_type *,
 rfcom_loop_data_type *
);

/* 15. rfapi_restore_loop_values() */
typedef void (*rfapi_restore_loop_values_type)
(
 rfll_device_desc_type *,
 rfcom_loop_data_type *
);

/* 16. rfapi_stop_loops() */
typedef void (*rfapi_stop_loops_type)
(
  rfll_device_desc_type *
);

/* 17. rfapi_start_loops() */
typedef void (*rfapi_start_loops_type)
(
  rfll_device_desc_type *
);

/* 18. rfapi_setup_intersystem_meas() */
typedef uint32 (*rfapi_setup_intersystem_meas_type)
(
  rfll_device_desc_type *,
  rfcom_mode_enum_type,
  const void *,
  rfcom_cb_handler_type
);

/* 19. rfapi_stop_intersystem_meas() */
typedef uint32 (*rfapi_stop_intersystem_meas_type)
(
  rfll_device_desc_type *,
  const void *,
  rfcom_cb_handler_type
);

/* 20. rfapi_tune_to_intersystem_chan() */
typedef uint32 (*rfapi_tune_to_intersystem_chan_type)
(
  rfll_device_desc_type *,
  rfcom_mode_enum_type,
  uint16,
  const void *,
  rfcom_cb_handler_type
);

/* 21. rfapi_do_tx_pwr_limit() */
typedef int16 (*rfapi_do_tx_pwr_limit_type)
(
  rfll_device_desc_type *
);

/* 22. rfapi_do_temp_comp() */
typedef void (*rfapi_do_temp_comp_type)
(
  rfll_device_desc_type *
);

/* 23. rfapi_set_tx_pwr_limit() */
typedef void (*rfapi_set_tx_pwr_limit_type)
(
  rfll_device_desc_type *,
  int16
  #ifdef FEATURE_WCDMA_MAX_TX_PWR_NV_LIM_FIX
,rfcom_txplim_channel_type
  #endif  
);


/* 24. rfapi_get_tx_pwr_limit() */
typedef int16 (*rfapi_get_tx_pwr_limit_type)
(
rfll_device_desc_type *
);

/* 25. rfapi_get_rx_agc_val() */
typedef int16 (*rfapi_get_rx_agc_val_type)
(
  rfll_device_desc_type *
);

/* 26. rfapi_get_receive_agc_val() */
typedef int16 (*rfapi_get_receive_agc_val_type)
(
rfll_device_desc_type *,
rfcom_rxagc_type,
rfcom_rxagc_unit_type
);

/* 27. rfapi_get_receive_agc_val_for_rscp() */
typedef int16 (*rfapi_get_receive_agc_val_for_rscp_type)
(
rfll_device_desc_type *,
rfcom_rxagc_type
);

/* 28. rfapi_get_tx_agc_val() */
typedef int16 (*rfapi_get_tx_agc_val_type)
(
  rfll_device_desc_type *
);

#ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
/* 29. rfapi_get_min_tx_power_val() */
typedef int16 (*rfapi_get_min_tx_power_val_type)
(
rfll_device_desc_type *
);
#endif

/* 30. rfapi_log_agc() */
typedef void (*rfapi_log_agc_type)
(
  rfll_device_desc_type *
);

/* 31. rfapi_read_trk_lo_adj_vs_temp() */
typedef uint16 (*rfapi_read_trk_lo_adj_vs_temp_type)
(
  rfll_device_desc_type *
);

/* 32. rfapi_read_rot_freq_vs_temp() */
typedef int16 (*rfapi_read_rot_freq_vs_temp_type)
(
  rfll_device_desc_type *
);

/* 33. rfapi_update_freq_error_table() */
typedef void (*rfapi_update_freq_error_table_type)
(
  rfll_device_desc_type *,
  uint16 trkloadjval, 
  int16 rotatorval, 
  boolean donotupdaterotatorflag
);

/* 34. rfapi_save_freq_error_table() */
typedef void (*rfapi_save_freq_error_table_type)
(
  rfll_device_desc_type *,
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
);


#ifdef FEATURE_RF_AFC
/* 35. rfapi_afc_get_vco_default() */
typedef int16 (*rfapi_afc_get_vco_default_type)
(
rfll_device_desc_type *
);

/* 36. rfapi_afc_get_vco_slope() */
typedef void (*rfapi_afc_get_vco_slope_type)
(
rfll_device_desc_type *,
uint16 *vco_slope,
uint8 *vco_slope_range
);

/* 37. rfapi_afc_get_rgs_data() */
typedef void (*rfapi_afc_get_rgs_data_type)
(
rfll_device_desc_type *,
rfcom_afc_rgs_data_type *rgs_data
);

/* 38. rfapi_afc_update_rgs_data() */
typedef void (*rfapi_afc_update_rgs_data_type)
(
rfll_device_desc_type *,
rfcom_afc_rgs_data_type *rgs_data
);

/* 39. rfapi_afc_save_nv_data() */
typedef boolean (*rfapi_afc_save_nv_data_type)
(
rfll_device_desc_type *,
/* Device descriptor */
rfcom_afc_nv_item_mask_type nv_item_mask,
/* Mask to specify nv items to save */
rex_tcb_type *caller_tcb_ptr,
/* Calling task's TCB pointer */
rex_sigs_type task_nv_wait_sig,
/* Task signal to wait for when reading NV */
void (*task_wait_func_ptr)( rex_sigs_type )
/* Task's wait function to be called when reading NV */
);

#endif /* FEATURE_RF_AFC */

#ifdef FEATURE_COMPRESSED_MODE
/* 40. rfapi_init_cm() */
typedef void (*rfapi_init_cm_type)
(
rfll_device_desc_type *
);

/* 41. rfapi_get_wtow_rf_ctl_buf_type */
typedef void (*rfapi_get_wtow_rf_ctl_buf_type)
(
rfll_device_desc_type *,
uint16,
void *
);
#endif /* FEATURE_COMPRESSED_MODE */

#ifdef FEATURE_RF_ENABLE_GTOW
/* 42. rfapi_build_gtow_scripts_type */
typedef void (*rfapi_build_gtow_scripts_type)
(
rfll_device_desc_type *,
uint16
);
#endif /* FEATURE_RF_ENABLE_GTOW */

/* 43. rfapi_pa_is_on() */
typedef uint8 (*rfapi_pa_is_on_type)
(
rfll_device_desc_type *
);

/* 44. rfapi_get_rf_dbg_scr_param() */
typedef uint8 (*rfapi_get_rf_dbg_scr_param_type)
(
rfll_device_desc_type *,
rfcom_dbg_scr_param_type param,
void *pBuf
);

/* 45. rfapi_get_rf_warmup_time() */
typedef uint16 (*rfapi_get_rf_warmup_time_type)
(
  rfll_device_desc_type *
);

/* 46. rfapi_set_primary_path() */
typedef void (*rfapi_set_primary_path_type)
(
  rfll_device_desc_type *,
  rf_path_enum_type
);

/* 47. rfapi_multimode_wcdma_handover() */
typedef void (*rfapi_multimode_wcdma_handover_type)
(
  rfll_device_desc_type *,
  rfll_device_desc_type *
);

/* 48. rfapi_multimode_cdma_handover() */
typedef void (*rfapi_multimode_cdma_handover_type)
(
  rfll_device_desc_type *,
  rfll_device_desc_type *,
  boolean
);

/* 49. rfapi_ioctl() */
typedef rfcom_ioctl_return_type (*rfapi_ioctl_type)
(
  rfll_device_desc_type *,
  rfcom_ioctl_param_type request,
  void *pbuf,
  int32 length
);
#if defined(FEATURE_CGPS)
#error code not present
#endif
/* 51. rfapi_is_sleeping() */
typedef boolean (*rfapi_is_sleeping_type)
(
  rfll_device_desc_type *
);
#if defined(FEATURE_CGPS)
#error code not present
#endif
/*---------------------------------------------------------------------------
  Device descriptor type definition for API layer of RF driver.  Data is
  maintained in a structure of this type for each instance of the RF driver.
---------------------------------------------------------------------------*/
typedef struct
{
  /* Pointer to logical layer device descriptor. */
  rfll_device_desc_type *ll_dev_desc_ptr;
  
  /*---- RF driver logical layer handlers for API functions. ---*/

  /* 0. rfapi_init() */
  rfapi_init_type rfapi_init;

  /* 1. rfapi_get_band_mask() */ 
  rfapi_get_band_mask_type rfapi_get_band_mask; 

  /* 2. rfapi_is_band_chan_supported() */
  rfapi_is_band_chan_supported_type rfapi_is_band_chan_supported[RFCOM_NUM_MODES];

  /* 3. rfapi_enter_mode() */
  rfapi_enter_mode_type rfapi_enter_mode[RFCOM_NUM_MODES];

  /* 4. rfapi_tune_to_chan() */
  rfapi_tune_to_chan_type rfapi_tune_to_chan[RFCOM_NUM_MODES];

  /* 5. rfapi_enable_tx() */
  rfapi_enable_tx_type rfapi_enable_tx[RFCOM_NUM_MODES];
  
  /* 6. rfapi_enable_rx() */
  rfapi_enable_rx_type rfapi_enable_rx[RFCOM_NUM_MODES];

  /* 7. rfapi_disable_tx() */
  rfapi_disable_tx_type rfapi_disable_tx[RFCOM_NUM_MODES];
  
  /* 8. rfapi_disable_rx() */
  rfapi_disable_rx_type rfapi_disable_rx[RFCOM_NUM_MODES];

  /* 9. rfapi_start_tx() */
  rfapi_start_tx_type rfapi_start_tx[RFCOM_NUM_MODES];
  
  /* 10. rfapi_stop_tx() */
  rfapi_stop_tx_type rfapi_stop_tx[RFCOM_NUM_MODES];
  
  /* 11. rfapi_sleep() */
  rfapi_sleep_type rfapi_sleep[RFCOM_NUM_MODES];

  /* 12. rfapi_stay_asleep() */
  rfapi_stay_asleep_type rfapi_stay_asleep[RFCOM_NUM_MODES];
  
  /* 13. rfapi_wakeup() */
  rfapi_wakeup_type rfapi_wakeup[RFCOM_NUM_MODES];

  /* 14. rfapi_capture_loop_values() */
  rfapi_capture_loop_values_type rfapi_capture_loop_values[RFCOM_NUM_MODES];

  /* 15. rfapi_restore_loop_values() */
  rfapi_restore_loop_values_type rfapi_restore_loop_values[RFCOM_NUM_MODES];

  /* 16. rfapi_stop_loops() */
  rfapi_stop_loops_type rfapi_stop_loops[RFCOM_NUM_MODES];

  /* 17. rfapi_start_loops() */
  rfapi_start_loops_type rfapi_start_loops[RFCOM_NUM_MODES];

  #if defined (FEATURE_RF_ENABLE_CM) || defined (FEATURE_RF_ENABLE_GTOW)
  /* 18. rfapi_setup_intersystem_meas() */
  rfapi_setup_intersystem_meas_type
                            rfapi_setup_intersystem_meas[RFCOM_NUM_MODES];

  /* 19. rfapi_stop_intersystem_meas() */
  rfapi_stop_intersystem_meas_type rfapi_stop_intersystem_meas[RFCOM_NUM_MODES];

  /* 20. rfapi_tune_to_intersystem_chan() */
  rfapi_tune_to_intersystem_chan_type
                            rfapi_tune_to_intersystem_chan[RFCOM_NUM_MODES];
  #endif

  /* 21. rfapi_do_tx_pwr_limit() */
  rfapi_do_tx_pwr_limit_type rfapi_do_tx_pwr_limit[RFCOM_NUM_MODES];

  /* 22. rfapi_do_temp_comp() */
  rfapi_do_temp_comp_type rfapi_do_temp_comp[RFCOM_NUM_MODES];

  /* 23. rfapi_set_tx_pwr_limit() */
  rfapi_set_tx_pwr_limit_type rfapi_set_tx_pwr_limit[RFCOM_NUM_MODES];

  /* 24. rfapi_get_tx_pwr_limit() */
  rfapi_get_tx_pwr_limit_type rfapi_get_tx_pwr_limit[RFCOM_NUM_MODES];
  
  /* 25. rfapi_get_rx_agc_val() */
  rfapi_get_rx_agc_val_type rfapi_get_rx_agc_val[RFCOM_NUM_MODES];

  /* 26. rfapi_get_receive_agc_val() */
  rfapi_get_receive_agc_val_type rfapi_get_receive_agc_val[RFCOM_NUM_MODES];

  /* 27. rfapi_get_receive_agc_valfor_rscp() */
  rfapi_get_receive_agc_val_for_rscp_type rfapi_get_receive_agc_val_for_rscp[RFCOM_NUM_MODES];

  /* 28. rfapi_get_tx_agc_val() */
  rfapi_get_tx_agc_val_type rfapi_get_tx_agc_val[RFCOM_NUM_MODES];

  #ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
  /* 29. rfapi_get_min_tx_power_val() */
  rfapi_get_min_tx_power_val_type rfapi_get_min_tx_power_val[RFCOM_NUM_MODES];
  #endif
  
  /* 30. rfapi_log_agc() */
  rfapi_log_agc_type rfapi_log_agc[RFCOM_NUM_MODES];

  /* 31. rfapi_read_trk_lo_adj_vs_temp() */
  rfapi_read_trk_lo_adj_vs_temp_type rfapi_read_trk_lo_adj_vs_temp[RFCOM_NUM_MODES];
  
  /* 32. rfapi_read_rot_freq_vs_temp() */
  rfapi_read_rot_freq_vs_temp_type rfapi_read_rot_freq_vs_temp[RFCOM_NUM_MODES];

  /* 33. rfapi_update_freq_error_table() */
  rfapi_update_freq_error_table_type rfapi_update_freq_error_table[RFCOM_NUM_MODES];
  
  /* 34. rfapi_save_freq_error_table() */
  rfapi_save_freq_error_table_type rfapi_save_freq_error_table[RFCOM_NUM_MODES];

  #ifdef FEATURE_RF_AFC

  /* 35. rfapi_afc_get_vco_default() */
  rfapi_afc_get_vco_default_type rfapi_afc_get_vco_default[RFCOM_NUM_MODES];

  /* 36. rfapi_afc_get_vco_slope() */
  rfapi_afc_get_vco_slope_type rfapi_afc_get_vco_slope[RFCOM_NUM_MODES];

  /* 37. rfapi_afc_get_rgs_data() */
  rfapi_afc_get_rgs_data_type rfapi_afc_get_rgs_data[RFCOM_NUM_MODES];

  /* 38. rfapi_afc_update_rgs_data() */
  rfapi_afc_update_rgs_data_type rfapi_afc_update_rgs_data[RFCOM_NUM_MODES];

  /* 39. rfapi_afc_save_nv_data() */
  rfapi_afc_save_nv_data_type rfapi_afc_save_nv_data[RFCOM_NUM_MODES];

  #endif /* FEATURE_RF_AFC */

  #ifdef FEATURE_COMPRESSED_MODE
  /* 40. rfapi_init_cm() */
  rfapi_init_cm_type rfapi_init_cm[RFCOM_NUM_MODES];

  /* 41. rfapi_get_wtow_rf_ctl_buf() */
  rfapi_get_wtow_rf_ctl_buf_type rfapi_get_wtow_rf_ctl_buf[RFCOM_NUM_MODES];
  #endif /* FEATURE_COMPRESSED_MODE */

  #ifdef FEATURE_RF_ENABLE_GTOW
  /* 42. rfapi_build_gtow_scripts() */
  rfapi_build_gtow_scripts_type rfapi_build_gtow_scripts[RFCOM_NUM_MODES];
  #endif /* FEATURE_RF_ENABLE_GTOW */

  /* 43. rfapi_pa_is_on() */
  rfapi_pa_is_on_type rfapi_pa_is_on[RFCOM_NUM_MODES];

  /* 44. rfapi_get_rf_dbg_scr_param() */
  rfapi_get_rf_dbg_scr_param_type rfapi_get_rf_dbg_scr_param[RFCOM_NUM_MODES];

  /* 45. rfapi_get_rf_warmup_time() */
  rfapi_get_rf_warmup_time_type rfapi_get_rf_warmup_time[RFCOM_NUM_MODES];

  /* 46. rfapi_set_primary_path() */
  rfapi_set_primary_path_type rfapi_set_primary_path[RFCOM_NUM_MODES];

  /* 47. rfapi_multimode_wcdma_handover() */
  rfapi_multimode_wcdma_handover_type rfapi_multimode_wcdma_handover[RFCOM_NUM_MODES];

  /* 48. rfapi_multimode_cdma_handover() */
  rfapi_multimode_cdma_handover_type rfapi_multimode_cdma_handover[RFCOM_NUM_MODES];

  /* 49. rfapi_ioctl() */
  rfapi_ioctl_type rfapi_ioctl[RFCOM_NUM_MODES];

  #if defined (FEATURE_CGPS)
#error code not present
  #endif /* FEATURE_CGPS */
  /* 51. rfapi_is_sleeping() */
  rfapi_is_sleeping_type rfapi_is_sleeping[RFCOM_NUM_MODES];
  #if defined (FEATURE_CGPS)
#error code not present
  #endif /* FEATURE_CGPS */
} device_descriptor_type;

/*---------------------------------------------------------------------------
  Table of device descriptors.  One descriptor for each device supported
  by the RF driver.  Note: All table API function pointer entries should
  be initialized to a valid pointer and not NULL.
---------------------------------------------------------------------------*/
device_descriptor_type device_descriptor_table[RFCOM_MAX_DEVICES] =
{
  /*---------------------------------------------
    RFLL_TRANSCEIVER_0: Device 0 descriptor table.
  ---------------------------------------------*/
  {
    /* Pointer to logical layer device descriptor. */
    NULL,                               

    /*----- RF driver API function entries. -----*/

    /* 0. rfapi_init() */
    rfll_init,                                   /* All modes handler */

    /* 1. rfapi_get_band_mask() */
    rfll_get_band_mask,                          /* All modes handler */

    /* 2. rfapi_is_band_chan_supported() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X( rfll1x_is_band_chan_supported),    /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */  
    },        

    /* 3. rfapi_enter_mode() */
    //TODO_7600 rfll_park
    { CDMA1X(rfll_park),                         /* Parked mode handler */ 
      WCDMA( rfllumts_init_wcdma ),              /* WCDMA mode handler */  
      GSM( rfllgsm_init_gsm ),                   /* EGSM mode handler  */  
      CDMA1X( rfll1x_enter_mode ),               /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */  
    },

    /* 4. rfapi_tune_to_chan() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_tune_to_chan ),            /* WCDMA mode handler */  
      GSM( rfllgsm_tune_to_chan ),               /* EGSM mode handler  */  
      CDMA1X( rfll1x_tune_to_chan ),             /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },    

    /* 5. rfapi_enable_tx() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_enable_tx ),               /* WCDMA mode handler */  
      GSM( rfllgsm_enable_tx ),                  /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },    
    
    /* 6. rfapi_enable_rx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X( rfll1x_enable_rx),                 /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */  
    },     

    /* 7. rfapi_disable_tx() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_disable_tx ),              /* WCDMA mode handler */  
      GSM( rfllgsm_disable_tx ),                 /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },    
    
    /* 8. rfapi_disable_rx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X(rfll1x_disable_rx),                 /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */  
    },     

    /* 9. rfapi_start_tx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },    

    /* 10. rfapi_stop_tx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */  
    },     

    /* 11. rfapi_sleep() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_sleep ),                   /* WCDMA mode handler */  
      GSM( rfllgsm_sleep ),                      /* EGSM mode handler  */  
      CDMA1X(rfll1x_sleep),                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */  
    },   

    /* 12. rfapi_stay_asleep() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_stay_asleep ),             /* WCDMA mode handler */  
      GSM( rfllgsm_stay_asleep ),                /* EGSM mode handler  */  
      CDMA1X( rfll1x_stay_asleep ),              /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },    

    /* 13. rfapi_wakeup() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_wakeup ),                  /* WCDMA mode handler */  
      GSM( rfllgsm_wakeup ),                     /* EGSM mode handler  */  
      CDMA1X(rfll1x_wakeup),                     /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },   

    /* 14. rfapi_capture_loop_values() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */  
    },    

    /* 15. rfapi_restore_loop_values() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */  
    },    

    /* 16. rfapi_stop_loops() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },       

    /* 17. rfapi_start_loops() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */  
    },   

    #if defined (FEATURE_RF_ENABLE_CM) || defined (FEATURE_RF_ENABLE_GTOW)
    /* 18. rfapi_setup_intersystem_meas() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_setup_intersystem_meas ),  /* WCDMA mode handler */  
      GSM( rfllgsm_setup_intersystem_meas ),     /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */  
    }, 

    /* 19. rfapi_stop_intersystem_meas() */
    { NULL,                                      /* Parked mode handler */
      WCDMA( rfllumts_stop_intersystem_meas ),   /* WCDMA mode handler */  
      GSM( rfllgsm_stop_intersystem_meas ),      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */  
    },    

    /* 20. rfapi_tune_to_intersystem_chan() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_tune_to_intersystem_chan ),/* WCDMA mode handler */  
      GSM( rfllgsm_tune_to_intersystem_chan ),   /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */  
    },
    #endif 

    /* 21. rfapi_do_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_do_tx_pwr_limit ),         /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */    
      NULL,                                      /* GPS mode handler  */  
    },      

    /* 22. rfapi_do_temp_comp() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_do_temp_comp ),            /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */  
    },      

    /* 23. rfapi_set_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_set_tx_pwr_limit ),        /* WCDMA mode handler */ 
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */    
      NULL,                                      /* GPS mode handler  */  
    },       

    /* 24. rfapi_get_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_tx_pwr_limit ),        /* WCDMA mode handler */ 
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },            

    /* 25. rfapi_get_rx_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_rx_agc_val ),          /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },                                       

    /* 26. rfapi_get_receive_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_receive_agc_val ),     /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },                                     

    /* 27. rfapi_get_receive_agc_val_for_rscp() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_receive_agc_val_for_rscp ), /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */  
    },  

    /* 28. rfapi_get_tx_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_tx_agc_val ),          /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */  
    },       

    #ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
    /* 29. rfapi_get_min_tx_power_val() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_min_tx_power_val ),    /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */  
    },  
    #endif

    /* 30. rfapi_log_agc() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_log_agc ),                 /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */  
    },      

    /* 31. rfapi_read_trk_lo_adj_vs_temp() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },        
    
    /* 32. rfapi_read_rot_freq_vs_temp() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */  
    },     
    
    /* 33. rfapi_update_freq_error_table() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },           
    
    /* 34. rfapi_save_freq_error_table() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },          
  
    #ifdef FEATURE_RF_AFC
    /* 35. rfapi_afc_get_vco_default() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_afc_get_vco_default ),     /* WCDMA mode handler */  
      GSM( rfllumts_afc_get_vco_default ),       /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */  
    },

    /* 36. rfapi_afc_get_vco_slope() */
    {  
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_afc_get_vco_slope ),       /* WCDMA mode handler */  
      GSM( rfllumts_afc_get_vco_slope ),         /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */    
      NULL,                                      /* GPS mode handler  */  
    },

    /* 37. rfapi_afc_get_rgs_data() */
    {  
     NULL,                                      /* Parked mode handler */ 
     WCDMA( rfllumts_afc_get_rgs_data ),        /* WCDMA mode handler */  
     GSM( rfllumts_afc_get_rgs_data ),          /* GSM mode handler  */  
     NULL,                                      /* 1x mode handler */  
     NULL,                                      /* GPS mode handler  */  
    },

    /* 38. rfapi_afc_update_rgs_data() */
    {  
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_afc_update_rgs_data ),     /* WCDMA mode handler */  
      GSM( rfllumts_afc_update_rgs_data ),       /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },

    /* 39. rfapi_afc_save_nv_data() */
    {  
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_afc_save_nv_data ),        /* WCDMA mode handler */  
      GSM( rfllumts_afc_save_nv_data ),          /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },
    #endif /* FEATURE_RF_AFC */

    #ifdef FEATURE_COMPRESSED_MODE
    /* 40. rfapi_init_cm() */
    {  
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_init_cm ),                 /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */  
    },

    /* 41. rfapi_get_wtow_rf_ctl_buf */
    {
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_wtow_rf_ctl_buf ),     /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },
    #endif /* FEATURE_COMPRESSED_MODE */

    #ifdef FEATURE_RF_ENABLE_GTOW
    /* 42. rfapi_build_gtow_scripts */
    {
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_build_gtow_scripts ),      /* WCDMA mode handler */  
      GSM( rfllumts_build_gtow_scripts ),        /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },
    #endif /* FEATURE_RF_ENABLE_GTOW */

    /* 43. rfapi_pa_is_on() */
    { NULL,                                         /* Parked mode handler */ 
      WCDMA(rfllutms_pa_is_on),                     /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                         /* GPS mode handler  */  
    },     

    /* 44. rfapi_get_rf_dbg_scr_param() */
    { NULL,                                         /* Parked mode handler */ 
      WCDMA(rfllumts_get_wcdma_rf_dbg_scr_param),   /* WCDMA mode handler */  
      GSM(rfllgsm_get_gsm_rf_dbg_scr_param),        /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */  
    },

    /* 45. rfapi_get_rf_warmup_time() */
    { NULL,                                         /* Parked mode handler */ 
      WCDMA(rfllumts_get_rf_warmup_time),           /* WCDMA mode handler */  
      GSM(rfllgsm_get_rf_warmup_time),              /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                         /* GPS mode handler  */  
    },

    /* 46. rfapi_set_primary_path() */
    { WCDMA(rfllumts_set_primary_path),             /* Parked mode handler */ 
      WCDMA(rfllumts_set_primary_path),             /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                         /* GPS mode handler  */  
    },

    /* 47. rfapi_multimode_wcdma_handover() */
    { NULL,                                         /* Parked mode handler */ 
      WCDMA(rfllumts_multimode_wcdma_handover),     /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                         /* GPS mode handler  */  
    },

    /* 48. rfapi_multimode_cdma_handover() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      CDMA1X(rfll1x_multimode_cdma_handover),       /* 1x mode handler */
      NULL,                                         /* GPS mode handler  */  
    },

    /* 49. rfapi_ioctl() */
    { rfllpark_ioctl,                            /* Parked mode handler */ 
      WCDMA(rfllumts_ioctl),                     /* WCDMA mode handler */  
      GSM(rfllgsm_ioctl),                        /* GSM mode handler  */  
      CDMA1X(rfll1x_ioctl),                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */  
    },
    #if defined(FEATURE_CGPS)
#error code not present
    #endif 
    /* 51. rfapi_is_sleeping() */
    { NULL,                                       /* Parked mode handler */ 
      WCDMA(rfllumts_is_sleeping),                /* WCDMA mode handler */  
      GSM(rfllgsm_is_sleeping),                   /* EGSM mode handler  */  
      CDMA1X(rfll1x_is_sleeping),                 /* 1x mode handler */
      NULL,                                       /* GPS mode handler  */  
    },
    #if defined (FEATURE_CGPS)
#error code not present
    #endif 
  },

  /*---------------------------------------------
    RFLL_RECEIVER_1: Device 1 descriptor table.
  ---------------------------------------------*/
  {
    /* Pointer to logical layer device descriptor. */
    NULL,                               

    /*----- RF driver API function entries. -----*/

    /* 0. rfapi_init() */
    rfll_init,                                   /* All modes handler */

    /* 1. rfapi_get_band_mask() */
    rfll_get_band_mask,                          /* All modes handler */

    /* 2. rfapi_is_band_chan_supported() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X( rfll1x_is_band_chan_supported),    /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */  
    },   

    /* 3. rfapi_enter_mode() */
    { CDMA1X(rfll_park),                         /* Parked mode handler */ 
      WCDMA( rfllumts_init_wcdma ),              /* WCDMA mode handler */  
      GSM( rfllgsm_init_gsm ),                   /* EGSM mode handler  */  
      CDMA1X( rfll1x_enter_mode ),               /* 1x mode handler */ 
      #ifdef FEATURE_CGPS
#error code not present
      #else
      NULL,
      #endif 
    }, 

    /* 4. rfapi_tune_to_chan() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_tune_to_chan ),            /* WCDMA mode handler */  
      GSM( rfllgsm_tune_to_chan ),               /* EGSM mode handler  */  
      CDMA1X( rfll1x_tune_to_chan ),             /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },     

    /* 5. rfapi_enable_tx() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_enable_tx ),               /* WCDMA mode handler */  
      GSM( rfllgsm_enable_tx ),                  /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },     

    /* 6. rfapi_enable_rx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X( rfll1x_enable_rx),                 /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },     

    /* 7. rfapi_disable_tx() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_disable_tx ),              /* WCDMA mode handler */  
      GSM( rfllgsm_disable_tx ),                 /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },    

    /* 8. rfapi_disable_rx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X(rfll1x_disable_rx),                 /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },                     

    /* 9. rfapi_start_tx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },     

    /* 10. rfapi_stop_tx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },     

    /* 11. rfapi_sleep() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_sleep ),                   /* WCDMA mode handler */  
      GSM( rfllgsm_sleep ),                      /* EGSM mode handler  */  
      CDMA1X(rfll1x_sleep),                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },

    /* 12. rfapi_stay_asleep() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_stay_asleep ),             /* WCDMA mode handler */  
      GSM( rfllgsm_stay_asleep ),                /* EGSM mode handler  */  
      CDMA1X( rfll1x_stay_asleep ),              /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },

    /* 13. rfapi_wakeup() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_wakeup ),                  /* WCDMA mode handler */  
      GSM( rfllgsm_wakeup ),                     /* EGSM mode handler  */  
      CDMA1X(rfll1x_wakeup),                     /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },   

    /* 14. rfapi_capture_loop_values() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },    

    /* 15. rfapi_restore_loop_values() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },   

    /* 16. rfapi_stop_loops() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },    

    /* 17. rfapi_start_loops() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    #if defined (FEATURE_RF_ENABLE_CM) || defined (FEATURE_RF_ENABLE_GTOW)
    /* 18. rfapi_setup_intersystem_meas() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_setup_intersystem_meas ),  /* WCDMA mode handler */  
      GSM( rfllgsm_setup_intersystem_meas ),     /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },
  
    /* 19. rfapi_stop_intersystem_meas() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_stop_intersystem_meas ),   /* WCDMA mode handler */  
      GSM( rfllgsm_stop_intersystem_meas ),      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    }, 

    /* 20. rfapi_tune_to_intersystem_chan() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_tune_to_intersystem_chan ),/* WCDMA mode handler */  
      GSM( rfllgsm_tune_to_intersystem_chan ),   /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },  
    #endif 

    /* 21. rfapi_do_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_do_tx_pwr_limit ),         /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },

    /* 22. rfapi_do_temp_comp() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_do_temp_comp ),            /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },

    /* 23. rfapi_set_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_set_tx_pwr_limit ),        /* WCDMA mode handler */ 
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },   

    /* 24. rfapi_get_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */ 
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },    

    /* 25. rfapi_get_rx_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_rx_agc_val ),          /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    }, 

    /* 26. rfapi_get_receive_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },

    /* 27. rfapi_get_receive_agc_val_for_rscp() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },  

    /* 28. rfapi_get_tx_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },  

    #ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
    /* 29. rfapi_get_min_tx_power_val() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },  
    #endif
    
    /* 30. rfapi_log_agc() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_log_agc ),                 /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },    

   /* 31. rfapi_read_trk_lo_adj_vs_temp() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },   
    
    /* 32. rfapi_read_rot_freq_vs_temp() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },  
    
    /* 33. rfapi_update_freq_error_table() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },  
    
    /* 34. rfapi_save_freq_error_table() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },    
    
    #ifdef FEATURE_RF_AFC
    /* 35. rfapi_afc_get_vco_default() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },

    /* 36. rfapi_afc_get_vco_slope() */
    {  
      NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },

    /* 37. rfapi_afc_get_rgs_data() */
    {  
     NULL,                                       /* Parked mode handler */ 
     NULL,                                       /* WCDMA mode handler */  
     NULL,                                       /* GSM mode handler  */  
     NULL,                                       /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },

    /* 38. rfapi_afc_update_rgs_data() */
    {  
      NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },

    /* 39. rfapi_afc_save_nv_data() */
    {  
      NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },
    #endif /* FEATURE_RF_AFC */

    #ifdef FEATURE_COMPRESSED_MODE
    /* 40. rfapi_init_cm() */
    {  
      NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },

    /* 41. rfapi_get_wtow_rf_ctl_buf */
    {
      NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },
    #endif /* FEATURE_COMPRESSED_MODE */

    #ifdef FEATURE_RF_ENABLE_GTOW
    /* 42. rfapi_build_gtow_scripts */
    {
      NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },
    #endif /* FEATURE_RF_ENABLE_GTOW */

    /* 43. rfapi_pa_is_on() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 44. rfapi_get_rf_dbg_scr_param() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    }, 

    /* 45. rfapi_get_rf_warmup_time() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
     },

    /* 46. rfapi_set_primary_path() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 47. rfapi_multimode_wcdma_handover() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      CDMA1X(rfll1x_multimode_wcdma_handover),      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

     /* 48. rfapi_multimode_cdma_handover() */
    { NULL,                                         /* Parked mode handler */ 
      WCDMA(rfllumts_multimode_cdma_handover),      /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },  

    /* 49. rfapi_ioctl() */
    { rfllpark_ioctl,                            /* Parked mode handler */ 
      WCDMA(rfllumts_ioctl),                     /* WCDMA mode handler */  
      GSM(rfllgsm_ioctl),                        /* GSM mode handler  */  
      CDMA1X(rfll1x_ioctl),                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    #if defined(FEATURE_CGPS)
#error code not present
    #endif 

    /* 51. rfapi_is_sleeping() */
    { NULL,                                       /* Parked mode handler */ 
      WCDMA(rfllumts_is_sleeping),                /* WCDMA mode handler */  
      GSM(rfllgsm_is_sleeping),                   /* EGSM mode handler  */  
      CDMA1X(rfll1x_is_sleeping),                 /* 1x mode handler */
      NULL,                                       /* GPS mode handler  */  
    },
    #if defined (FEATURE_CGPS)
#error code not present
    #endif 
  },
  /*---------------------------------------------
    RFLL_RECEIVER_DIV: Device 2 descriptor table.
  ---------------------------------------------*/
  {
    /* Pointer to logical layer device descriptor. */
    NULL,                               

    /*----- RF driver API function entries. -----*/

    /* 0. rfapi_init() */
    rfll_init,                                   /* All modes handler */

    /* 1. rfapi_get_band_mask() */
    rfll_get_band_mask,                          /* All modes handler */

    /* 2. rfapi_is_band_chan_supported() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X( rfll1x_is_band_chan_supported),    /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 3. rfapi_enter_mode() */
    { CDMA1X(rfll_park),                         /* Parked mode handler */ 
      WCDMA( rfllumts_init_wcdma ),              /* WCDMA mode handler */  
      GSM( rfllgsm_init_gsm ),                   /* EGSM mode handler  */  
      CDMA1X( rfll1x_enter_mode ),               /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 4. rfapi_tune_to_chan() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_tune_to_chan ),            /* WCDMA mode handler */  
      GSM( rfllgsm_tune_to_chan ),               /* EGSM mode handler  */  
      CDMA1X( rfll1x_tune_to_chan ),             /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },

    /* 5. rfapi_enable_tx() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_enable_tx ),               /* WCDMA mode handler */  
      GSM( rfllgsm_enable_tx ),                  /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 6. rfapi_enable_rx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X( rfll1x_enable_rx),                 /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },

    /* 7. rfapi_disable_tx() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_disable_tx ),              /* WCDMA mode handler */  
      GSM( rfllgsm_disable_tx ),                 /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },

    /* 8. rfapi_disable_rx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X(rfll1x_disable_rx),                 /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 9. rfapi_start_tx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 10. rfapi_stop_tx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */  
      NULL,                                      /* GPS mode handler  */
    },

    /* 11. rfapi_sleep() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_sleep ),                   /* WCDMA mode handler */  
      GSM( rfllgsm_sleep ),                      /* EGSM mode handler  */  
      CDMA1X(rfll1x_sleep),                      /* 1x mode handler */ 
      NULL,                                      /* GPS mode handler  */
    },

    /* 12. rfapi_stay_asleep() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_stay_asleep ),             /* WCDMA mode handler */  
      GSM( rfllgsm_stay_asleep ),                /* EGSM mode handler  */  
      CDMA1X( rfll1x_stay_asleep ),              /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 13. rfapi_wakeup() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_wakeup ),                  /* WCDMA mode handler */  
      GSM( rfllgsm_wakeup ),                     /* EGSM mode handler  */  
      CDMA1X(rfll1x_wakeup),                     /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 14. rfapi_capture_loop_values() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 15. rfapi_restore_loop_values() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 16. rfapi_stop_loops() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 17. rfapi_start_loops() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    #if defined (FEATURE_RF_ENABLE_CM) || defined (FEATURE_RF_ENABLE_GTOW)
    /* 18. rfapi_setup_intersystem_meas() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_setup_intersystem_meas ),  /* WCDMA mode handler */  
      GSM( rfllgsm_setup_intersystem_meas ),     /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
  
    /* 19. rfapi_stop_intersystem_meas() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_stop_intersystem_meas ),   /* WCDMA mode handler */  
      GSM( rfllgsm_stop_intersystem_meas ),      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 20. rfapi_tune_to_intersystem_chan() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_tune_to_intersystem_chan ),/* WCDMA mode handler */  
      GSM( rfllgsm_tune_to_intersystem_chan ),   /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    #endif 

    /* 21. rfapi_do_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_do_tx_pwr_limit ),         /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 22. rfapi_do_temp_comp() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_do_temp_comp ),            /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 23. rfapi_set_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_set_tx_pwr_limit ),        /* WCDMA mode handler */ 
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 24. rfapi_get_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_tx_pwr_limit ),        /* WCDMA mode handler */ 
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 25. rfapi_get_rx_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_rx_agc_val ),          /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 26. rfapi_get_receive_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_receive_agc_val ),     /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 27. rfapi_get_receive_agc_val_for_rscp() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_receive_agc_val_for_rscp ),/* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
  
    /* 28. rfapi_get_tx_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_tx_agc_val ),          /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    #ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
    /* 29. rfapi_get_min_tx_power_val() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },  
    #endif

    /* 30. rfapi_log_agc() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_log_agc ),                 /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

   /* 31. rfapi_read_trk_lo_adj_vs_temp() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    
    /* 32. rfapi_read_rot_freq_vs_temp() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    
    /* 33. rfapi_update_freq_error_table() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    
    /* 34. rfapi_save_freq_error_table() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
  
 #ifdef FEATURE_RF_AFC
    /* 35. rfapi_afc_get_vco_default() */
    {  
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_afc_get_vco_default ),     /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 36. rfapi_afc_get_vco_slope() */
    {  
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_afc_get_vco_slope ),       /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 37. rfapi_afc_get_rgs_data() */
    {  
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_afc_get_rgs_data ),        /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 38. rfapi_afc_update_rgs_data() */
    {  
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_afc_update_rgs_data ),     /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 39. rfapi_afc_save_nv_data() */
    {  
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_afc_save_nv_data ),        /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
#endif /* FEATURE_RF_AFC */

    #ifdef FEATURE_COMPRESSED_MODE
    /* 40. rfapi_init_cm() */
    {  
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_init_cm ),                 /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 41. rfapi_get_wtow_rf_ctl_buf */
    {
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_wtow_rf_ctl_buf ),     /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    #endif /* FEATURE_COMPRESSED_MODE */

    #ifdef FEATURE_RF_ENABLE_GTOW
    /* 42. rfapi_build_gtow_scripts */
    {
      NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_build_gtow_scripts ),      /* WCDMA mode handler */  
      GSM( rfllumts_build_gtow_scripts ),        /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    #endif /* FEATURE_RF_ENABLE_GTOW */
    
    /* 43. rfapi_pa_is_on() */
    { NULL,                                         /* Parked mode handler */ 
      WCDMA(rfllutms_pa_is_on),                     /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 44. rfapi_get_rf_dbg_scr_param() */
    { NULL,                                         /* Parked mode handler */ 
      WCDMA(rfllumts_get_wcdma_rf_dbg_scr_param),   /* WCDMA mode handler */  
      GSM(rfllgsm_get_gsm_rf_dbg_scr_param),        /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 45. rfapi_get_rf_warmup_time() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 46. rfapi_set_primary_path() */
    { WCDMA(rfllumts_set_primary_path),             /* Parked mode handler */ 
      WCDMA(rfllumts_set_primary_path),             /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 47. rfapi_multimode_wcdma_handover() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 48. rfapi_multimode_cdma_handover() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },
  
    /* 49. rfapi_ioctl() */
    { rfllpark_ioctl,                            /* Parked mode handler */ 
      WCDMA(rfllumts_ioctl),                     /* WCDMA mode handler */  
      GSM(rfllgsm_ioctl),                        /* GSM mode handler  */  
      CDMA1X(rfll1x_ioctl),                      /* 1x mode handler */     
      NULL,                                      /* GPS mode handler  */
    },

    #if defined(FEATURE_CGPS)
#error code not present
    #endif 

    /* 51. rfapi_is_sleeping() */
    { NULL,                                       /* Parked mode handler */ 
      WCDMA(rfllumts_is_sleeping),                /* WCDMA mode handler */  
      GSM(rfllgsm_is_sleeping),                   /* EGSM mode handler  */  
      CDMA1X(rfll1x_is_sleeping),                 /* 1x mode handler */
      NULL,                                       /* GPS mode handler  */  
    },
    #if defined (FEATURE_CGPS)
#error code not present
    #endif 
  },

  /*---------------------------------------------
    RFLL_RECEIVER_DUAL: Device 2 descriptor table.
  ---------------------------------------------*/
  {
    /* Pointer to logical layer device descriptor. */
    NULL,                               

    /*----- RF driver API function entries. -----*/

    /* 0. rfapi_init() */
    rfll_init,                                   /* All modes handler */

    /* 1. rfapi_get_band_mask() */
    rfll_get_band_mask,                          /* All modes handler */

    /* 2. rfapi_is_band_chan_supported() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X( rfll1x_is_band_chan_supported),    /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 3. rfapi_enter_mode() */
   //TODO_7600 rfll_park
    { CDMA1X(rfll_park),                         /* Parked mode handler */ 
      WCDMA( rfllumts_init_wcdma ),              /* WCDMA mode handler */  
      GSM( rfllgsm_init_gsm ),                   /* EGSM mode handler  */  
      CDMA1X( rfll1x_enter_mode ),               /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 4. rfapi_tune_to_chan() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_tune_to_chan ),            /* WCDMA mode handler */  
      GSM( rfllgsm_tune_to_chan ),               /* EGSM mode handler  */  
      CDMA1X( rfll1x_tune_to_chan ),             /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 5. rfapi_enable_tx() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_enable_tx ),               /* WCDMA mode handler */  
      GSM( rfllgsm_enable_tx ),                  /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 6. rfapi_enable_rx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X( rfll1x_enable_rx),                 /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 7. rfapi_disable_tx() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_disable_tx ),              /* WCDMA mode handler */  
      GSM( rfllgsm_disable_tx ),                 /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 8. rfapi_disable_rx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      CDMA1X(rfll1x_disable_rx),                 /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 9. rfapi_start_tx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 10. rfapi_stop_tx() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 11. rfapi_sleep() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_sleep ),                   /* WCDMA mode handler */  
      GSM( rfllgsm_sleep ),                      /* EGSM mode handler  */  
      CDMA1X(rfll1x_sleep),                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 12. rfapi_stay_asleep() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_stay_asleep ),             /* WCDMA mode handler */  
      GSM( rfllgsm_stay_asleep ),                /* EGSM mode handler  */  
      CDMA1X( rfll1x_stay_asleep ),              /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 13. rfapi_wakeup() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_wakeup ),                  /* WCDMA mode handler */  
      GSM( rfllgsm_wakeup ),                     /* EGSM mode handler  */  
      CDMA1X(rfll1x_wakeup),                     /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 14. rfapi_capture_loop_values() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 15. rfapi_restore_loop_values() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 16. rfapi_stop_loops() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 17. rfapi_start_loops() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    #if defined (FEATURE_RF_ENABLE_CM) || defined (FEATURE_RF_ENABLE_GTOW)
    /* 18. rfapi_setup_intersystem_meas() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_setup_intersystem_meas ),  /* WCDMA mode handler */  
      GSM( rfllgsm_setup_intersystem_meas ),     /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
  
    /* 19. rfapi_stop_intersystem_meas() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_stop_intersystem_meas ),   /* WCDMA mode handler */  
      GSM( rfllgsm_stop_intersystem_meas ),      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 20. rfapi_tune_to_intersystem_chan() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_tune_to_intersystem_chan ),/* WCDMA mode handler */  
      GSM( rfllgsm_tune_to_intersystem_chan ),   /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    #endif 

    /* 21. rfapi_do_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_do_tx_pwr_limit ),         /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 22. rfapi_do_temp_comp() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_do_temp_comp ),            /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 23. rfapi_set_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_set_tx_pwr_limit ),        /* WCDMA mode handler */ 
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 24. rfapi_get_tx_pwr_limit() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_tx_pwr_limit ),        /* WCDMA mode handler */ 
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 25. rfapi_get_rx_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_rx_agc_val ),          /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 26. rfapi_get_receive_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 27. rfapi_get_receive_agc_val_for_rscp() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 28. rfapi_get_tx_agc_val() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_get_tx_agc_val ),          /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    #ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
    /* 29. rfapi_get_min_tx_power_val() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* GSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },  
    #endif

    /* 30. rfapi_log_agc() */
    { NULL,                                      /* Parked mode handler */ 
      WCDMA( rfllumts_log_agc ),                 /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

   /* 31. rfapi_read_trk_lo_adj_vs_temp() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    
    /* 32. rfapi_read_rot_freq_vs_temp() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    
    /* 33. rfapi_update_freq_error_table() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    
    /* 34. rfapi_save_freq_error_table() */
    { NULL,                                      /* Parked mode handler */ 
      NULL,                                      /* WCDMA mode handler */  
      NULL,                                      /* EGSM mode handler  */  
      NULL,                                      /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    
    #ifdef FEATURE_RF_AFC
    /* 35. rfapi_afc_get_vco_default() */
    { NULL,                                       /* Parked mode handler */ 
      NULL,                                       /* WCDMA mode handler */  
      NULL,                                       /* GSM mode handler  */  
      NULL,                                       /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 36. rfapi_afc_get_vco_slope() */
    {  
      NULL,                                       /* Parked mode handler */ 
      NULL,                                       /* WCDMA mode handler */  
      NULL,                                       /* GSM mode handler  */  
      NULL,                                       /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    }

    /* 37. rfapi_afc_get_rgs_data() */
    {  
     NULL,                                       /* Parked mode handler */ 
     NULL,                                       /* WCDMA mode handler */  
     NULL,                                       /* GSM mode handler  */  
     NULL,                                       /* 1x mode handler */   
       NULL,                                      /* GPS mode handler  */
    },

    /* 38. rfapi_afc_update_rgs_data() */
    {  
      NULL,                                       /* Parked mode handler */ 
      NULL,                                       /* WCDMA mode handler */  
      NULL,                                       /* GSM mode handler  */  
      NULL,                                       /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 39. rfapi_afc_save_nv_data() */
    {  
      NULL,                                       /* Parked mode handler */ 
      NULL,                                       /* WCDMA mode handler */  
      NULL,                                       /* GSM mode handler  */  
      NULL,                                       /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    #endif /* FEATURE_RF_AFC */

    #ifdef FEATURE_COMPRESSED_MODE
    /* 40. rfapi_init_cm() */
    {  
      NULL,                                       /* Parked mode handler */ 
      NULL,                                       /* WCDMA mode handler */  
      NULL,                                       /* GSM mode handler  */  
      NULL,                                       /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 41. rfapi_get_wtow_rf_ctl_buf */
    {
      NULL,                                       /* Parked mode handler */ 
      NULL,                                       /* WCDMA mode handler */  
      NULL,                                       /* GSM mode handler  */  
      NULL,                                       /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    #endif /* FEATURE_COMPRESSED_MODE */

    #ifdef FEATURE_RF_ENABLE_GTOW
    /* 42. rfapi_build_gtow_scripts */
    {
      NULL,                                       /* Parked mode handler */ 
      NULL,                                       /* WCDMA mode handler */  
      NULL,                                       /* GSM mode handler  */  
      NULL,                                       /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },
    #endif /* FEATURE_RF_ENABLE_GTOW */

    /* 43. rfapi_pa_is_on() */
    { NULL,                                       /* Parked mode handler */ 
      NULL,                                       /* WCDMA mode handler */  
      NULL,                                       /* GSM mode handler  */  
      NULL,                                       /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 44. rfapi_get_rf_dbg_scr_param() */
    { NULL,                                       /* Parked mode handler */ 
      NULL,                                       /* WCDMA mode handler */  
      NULL,                                       /* GSM mode handler  */  
      NULL,                                       /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 45. rfapi_get_rf_warmup_time() */
    { NULL,                                       /* Parked mode handler */ 
      NULL,                                       /* WCDMA mode handler */  
      NULL,                                       /* GSM mode handler  */  
      NULL,                                       /* 1x mode handler */   
      NULL,                                      /* GPS mode handler  */
    },

    /* 46. rfapi_set_primary_path() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 47. rfapi_multimode_wcdma_handover() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 48. rfapi_multimode_cdma_handover() */
    { NULL,                                         /* Parked mode handler */ 
      NULL,                                         /* WCDMA mode handler */  
      NULL,                                         /* GSM mode handler  */  
      NULL,                                         /* 1x mode handler */
      NULL,                                      /* GPS mode handler  */
    },

    /* 49. rfapi_ioctl() */
    { rfllpark_ioctl,                            /* Parked mode handler */ 
      WCDMA(rfllumts_ioctl),                     /* WCDMA mode handler */  
      GSM(rfllgsm_ioctl),                        /* GSM mode handler  */  
      CDMA1X(rfll1x_ioctl),                      /* 1x mode handler */     
      NULL,                                      /* GPS mode handler  */
    },

    #if defined(FEATURE_CGPS)
#error code not present
    #endif 

    /* 51. rfapi_is_sleeping() */
    { NULL,                                       /* Parked mode handler */ 
      WCDMA(rfllumts_is_sleeping),                /* WCDMA mode handler */  
      GSM(rfllgsm_is_sleeping),                   /* EGSM mode handler  */  
      CDMA1X(rfll1x_is_sleeping),                 /* 1x mode handler */
      NULL,                                       /* GPS mode handler  */  
    },
    #if defined (FEATURE_CGPS)
#error code not present
    #endif 
  }
};

/*---------------------------------------------------------------------------
  Reference to device descriptors.
---------------------------------------------------------------------------*/

rfapi_device_desc_type rfapi_dev_ptr[RFCOM_MAX_DEVICES] = 
{
    (void *) NULL,  /* RFCOM_TRANSCEIVER_0   */
    (void *) NULL,  /* RFCOM_RECEIVER_1      */
    (void *) NULL,  /* RFCOM_RECEIVER_DIV    */
    (void *) NULL   /* RFCOM_RECEIVER_DUAL   */

};


/*=============================================================================
   
                              FUNCTION DEFINITIONS

==============================================================================*/

/*===========================================================================

FUNCTION  VERIFY_DESCRIPTOR 

DESCRIPTION
  Verifies the device descriptor pointer passed in is valid.  If the
  pointer is invalid, an ASSERT is thrown.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  ASSERT is thrown if device descriptor pointer is invalid.

===========================================================================*/
static __inline void verify_descriptor
(
  device_descriptor_type *desc_ptr      /* Pointer to device descriptor */
)
{
  /* Verify device has been successfully initialized and
     descriptor is valid. */
  ASSERT( desc_ptr != NULL );
}

/*===========================================================================

FUNCTION  VERIFY_MODE 

DESCRIPTION
  Verifies the mode passed in is valid.  If the mode is invalid, an
  ASSERT is thrown.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  ASSERT is thrown if mode is invalid.

===========================================================================*/
static __inline void verify_rf_mode
(
  rfcom_mode_enum_type mode             /* RF mode to verify */
)
{ 
  // Note: Normally enum should be integer
  // However, ARM 2.1 treats enum type as unsigned integer
  // Remove the first comparison for compiler warnings, need to put back 
  // if enum type is signed or compiler fix the bug.
  //ASSERT( mode >= RFCOM_PARKED_MODE && mode < RFCOM_NUM_MODES );
  ASSERT( mode < RFCOM_NUM_MODES );
}

/*===========================================================================

FUNCTION  RFAPI_INIT 

DESCRIPTION
  Initializes the RF device specified (primary radio transceiver or
  secondary receiver for MSM6500).  Reads common and mode specific RF NV
  parameters from NV and initializes common and mode specific NV tables.
  Initializes common hardware and mode specific RF hardware for each mode
  supported (WCDMA, GSM, CDMA, HDR, GPS, etc.) to a low power parked/sleep
  state.  RF driver is placed in parked/sleep state.
  It returns a state information of whether NV is iniatialized or not.
  If RF NV is initalized it returns non-zero.

DEPENDENCIES
  This function must be called before any other RF driver function.
  May require special parked mode MDSP image to be loaded before calling
  this function (TBD).

RETURN VALUE
  If initialization is successful, returns a pointer to the device
  descriptor to be used for all other RF driver calls to this device,
  otherwise returns NULL.

SIDE EFFECTS
  None

===========================================================================*/
rfapi_device_desc_type rfapi_init
(
  rfcom_device_enum_type device,        /* RF device to initialize */
  rex_tcb_type *caller_tcb_ptr,         /* Calling task's TCB pointer */
  rex_sigs_type task_nv_wait_sig,       /* Task signal to wait for when
                                           reading NV */
  void (*task_wait_func_ptr)( rex_sigs_type ),
                                        /* Task's wait function to be called
                                           when reading NV */
  int *state                            /* state whether rf init success or not */
)
{
  rfapi_init_type api_ptr;
  device_descriptor_type *desc_ptr;
  rfll_device_desc_type *ll_desc_ptr;

  // Note: Normally enum should be integer
  // However, ARM 2.1 treats enum as unsigned integer
  // Remove the first comparison for compiler warnings, need to put back 
  // if enum is signed or compiler fix the bug.
  /* Verify device is valid. */
//  ASSERT( device >=0 && device < RFCOM_MAX_DEVICES );

  /* Verify device is valid. */
  ASSERT( device < RFCOM_MAX_DEVICES );

  /* Set pointer to device descriptor. */
  desc_ptr = &device_descriptor_table[device];
  
  /* Set pointer to API handler function table entry.  Note: This API
     function is not mode dependent. */
  api_ptr = desc_ptr->rfapi_init;

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    ll_desc_ptr = api_ptr( device, caller_tcb_ptr, task_nv_wait_sig,
                           task_wait_func_ptr );

    /* Update pointer to logical layer device descriptor and return
       API descriptor pointer if device was successfully initialized. */
    if ( ll_desc_ptr != NULL )
    {
      desc_ptr->ll_dev_desc_ptr = ll_desc_ptr;
      
      /* return state info */
      *state = (int) ll_desc_ptr->state;

      return (rfapi_device_desc_type) desc_ptr;
    }
  }

  /* Device initialization failed. */
  return NULL;
  
} /* rfapi_init() */

/*===========================================================================

FUNCTION  RFAPI_GET_DEVICE_DESCRIPTOR

DESCRIPTION
  Gets the device descriptor of device that has been initialized.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns device descriptor of device that has been initialized.  If
  device has not been initialized, returns a NULL pointer. 

SIDE EFFECTS
  None

===========================================================================*/
rfapi_device_desc_type rfapi_get_device_descriptor
(
  rfcom_device_enum_type device         /* RF device to get descriptor for */
)
{
  device_descriptor_type *desc_ptr;


  // Note: Normally enum should be integer
  // However, ARM 2.1 treats enum type as unsigned integer
  // Remove the first comparison for compiler warnings, need to put back 
  // if enum type is signed or compiler fix the bug.
  /* Verify device is valid. */
  //  ASSERT( device >= 0 && device < RFCOM_MAX_DEVICES );

  /* Verify device is valid. */
  ASSERT( device < RFCOM_MAX_DEVICES );

  /* Set pointer to device descriptor. */
  desc_ptr = &device_descriptor_table[device];

  /* Check if device was successfully initialized and return device
     descriptor pointer if it was. */
  if ( desc_ptr->ll_dev_desc_ptr != NULL )
  {
    return (rfapi_device_desc_type) desc_ptr;
  }

  /* Device has not been initialized. */
  return NULL;
  
} /* rfapi_get_device_descriptor() */

/*===========================================================================

FUNCTION  RFAPI_ENTER_MODE

DESCRIPTION
  Initializes RF hardware for mode of operation specified by parameter
  rf_mode (WCDMA, GSM, CDMA, HDR, GPS, PARKED/SLEEP, etc.).  Places RF
  driver in Rx state of operation in mode of operation specified, except
  when mode specified is PARKED/SLEEP.  Typically, control loops are not
  started by this function and must be started by calling
  rfapi_start_loops().  Control loops are typically in override mode,
  outputting only the override value.  The actual state of the control
  loops after calling this function are mode dependent.

  Callback function is called when RF hardware has settled and is ready
  for use.  For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context.

DEPENDENCIES
  rfapi_init() must have already been called.  
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfapi_enter_mode
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */         
  rfcom_mode_enum_type rf_mode,         /* RF mode */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */  
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfapi_enter_mode_type api_ptr;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Verify mode is valid. */ 
  verify_rf_mode( rf_mode );

  /* Set pointer to API handler function table entry. */
  api_ptr = desc_ptr->rfapi_enter_mode[rf_mode];

  /* Call handler function if function pointer not NULL else call
     callback function immediately and return delay of 0. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( desc_ptr->ll_dev_desc_ptr, rf_mode, user_data_ptr,
                    cb_handler );
  }
  else
  {
    if ( cb_handler != NULL )
    {
      cb_handler( RFCOM_TIME_DELAY_EXPIRED, (void *) user_data_ptr );
    }
    return 0;
  }
  
} /* rfapi_enter_mode() */

#if defined (FEATURE_CGPS)
#error code not present
#endif /* FEATURE_CGPS */

/*===========================================================================

FUNCTION  RFAPI_TUNE_TO_CHAN

DESCRIPTION
  Tunes the UHF Rx/Tx synthesizers to the channel specified.  The channel
  is interpreted based on operating mode.  The input parameter synths
  specifies whether transmit, receive, or both synthesizers should be
  tuned to the specified channel.

  Callback function is called when tuning is complete and synthesizers have
  locked.  For support of multiple events occurring during synthesizer lock
  time, event parameter is passed into callback, as well as a pointer to
  user data that was passed in by caller.  Note: Callback executes in
  interrupt context.

DEPENDENCIES
  rfapi_init() must have already been called.
  rfapi_enter_mode() must have already been called.  
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for
  synthesizers to lock.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfapi_tune_to_chan
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */         
  uint16 channel,                       /* Mode dependent channel */
  rfcom_synth_tune_enum_type synths,    /* Synthesizer(s) to tune */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_tune_to_chan_type api_ptr;
  rfcom_mode_enum_type mode;

  #ifdef T_QUASAR_BB
#error code not present
  #endif
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  // Note: Normally enum should be integer
  // However, ARM 2.1 treats enum type as unsigned integer
  // Remove the first comparison for compiler warnings, need to put back 
  // if enum type is signed or compiler fix the bug.
  /* Verify synthesizer()s to tune are valid. */
  //ASSERT( synths >= RFCOM_TUNE_TX && synths < RFCOM_NUM_TUNES );

  /* Verify synthesizer()s to tune are valid. */
  ASSERT( synths < RFCOM_NUM_TUNES );

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API handler function table entry. */
  api_ptr = desc_ptr->rfapi_tune_to_chan[mode];

  /* Call handler function if function pointer not NULL else call
     callback function immediately and return delay of 0. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( ll_desc_ptr, channel, synths, user_data_ptr, cb_handler );
  }
  else
  {
    if ( cb_handler != NULL )
    {
      cb_handler( RFCOM_TIME_DELAY_EXPIRED, (void *) user_data_ptr );
    }
    return 0;
  }
  
} /* rfapi_tune_to_chan() */

/*===========================================================================

FUNCTION  RFAPI_ENABLE_TX

DESCRIPTION
  Enables transmitter for mode of operation RF driver is currently
  operating in, typically preparing transmitter for access attempts.
  Turns on LDOs for transmit hardware.  Turns on PA.  Places RF driver in
  Rx/Tx state of operation.

  Callback function is called when RF hardware has settled and is ready
  for use. For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context. 

DEPENDENCIES
  rfapi_init() must have already been called.
  rfapi_enter_mode() must have already been called.
  rfapi_tune_to_chan() should have already been called.
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfapi_enable_tx
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Callback handler */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_enable_tx_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_enable_tx[mode];

  /* Call handler function if function pointer not NULL else call
     callback function immediately and return delay of 0. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( ll_desc_ptr, user_data_ptr, cb_handler );
  }
  else
  {
    if ( cb_handler != NULL )
    {
      cb_handler( RFCOM_TIME_DELAY_EXPIRED, (void *) user_data_ptr );
    }
    return 0;
  }
  
} /* rfapi_enable_tx() */
/*===========================================================================

FUNCTION  RFAPI_ENABLE_RX

DESCRIPTION
  
  Enables a receiver chain for the specified mode of operation. Turns 
  on LDOs for transmit hardware.  Turns on PA.  Places receiver chain 
  in Rx state of operation.

  Callback function is called when Rx hardware has settled and is ready
  for use. For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context. 
  
  Calling module must wait for the delay msec before assuming 
  RF enabled. This operation will not tune the RF chain.

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.  
  
RETURN VALUE
  Returns the wait time for this operation.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfapi_enable_rx
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Callback handler */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_enable_rx_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return FALSE;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_enable_rx[mode];

  /* Call handler function if function pointer not NULL else call
     callback function immediately and return delay of 0. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( ll_desc_ptr, user_data_ptr, cb_handler );
  }
  else
  {
    if ( cb_handler != NULL )
    {
      cb_handler( RFCOM_TIME_DELAY_EXPIRED, (void *) user_data_ptr );
    }
    return 0;
  }
  
} /* rfapi_enable_rx() */

/*===========================================================================

FUNCTION  RFAPI_DISABLE_TX

DESCRIPTION
  Disables transmitter for mode of operation RF driver is currently
  operating in.  Turns off LDOs for transmit hardware.  Turns off PA.
  Places RF driver in Rx state of operation.

DEPENDENCIES
  rfapi_enable_tx() should have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_disable_tx
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_disable_tx_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_disable_tx[mode];

  /* Call handler function if function pointer not NULL. */
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr );
  }
  
} /* rfapi_disable_tx() */

/*===========================================================================

FUNCTION  RFAPI_DISABLE_RX

DESCRIPTION
  
  Disbles a receiver chain. Performs all the necessary activities to minimize 
  current drawn by the RF chain which has been disabled.


DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_disable_rx
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_disable_rx_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_disable_rx[mode];

  /* Call handler function if function pointer not NULL. */
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr );
  }
  
} /* rfapi_disable_tx() */

/*===========================================================================

FUNCTION  

DESCRIPTION
  

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_multimode_wcdma_handover
(
  rfapi_device_desc_type dev_desc_c0,       /* Chain 0 device descriptor */
  rfapi_device_desc_type dev_desc_c1        /* Chain 1 device descriptor */
)
{
  device_descriptor_type *desc_ptr[2];
  rfll_device_desc_type *ll_desc_ptr[2];
  rfapi_multimode_wcdma_handover_type api_ptr[2];
  rfcom_mode_enum_type mode[2];

  desc_ptr[0] = (device_descriptor_type*) dev_desc_c0;
  desc_ptr[1] = (device_descriptor_type*) dev_desc_c1;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr[0] );
  verify_descriptor( desc_ptr[1] );

  ll_desc_ptr[0] = desc_ptr[0]->ll_dev_desc_ptr;
  ll_desc_ptr[1] = desc_ptr[1]->ll_dev_desc_ptr;
  mode[0] = ll_desc_ptr[0]->mode;
  mode[1] = ll_desc_ptr[1]->mode;

  /* Check that Chain 0 is WCDMA and Chain 1 is 1x */
  if ( (mode[0] != RFCOM_WCDMA_MODE) ||
       (mode[1] != RFCOM_1X_MODE) )
  {
    MSG_ERROR("Can't do Multimode WCDMA Handover when (c0,c1) modes are (%d,%d)",
              mode[0],mode[1],0);
    /* do nothing after this */
  }
  else
  {
    api_ptr[0] = desc_ptr[0]->rfapi_multimode_wcdma_handover[RFCOM_WCDMA_MODE];
    api_ptr[1] = desc_ptr[1]->rfapi_multimode_wcdma_handover[RFCOM_1X_MODE];
    if ( (api_ptr[0] != NULL) && (api_ptr[1] != NULL) )
    {
      /* Sleep Chain 0 which has WCDMA */
      api_ptr[0]( ll_desc_ptr[0], ll_desc_ptr[1] );

      /* Disable Chain 1 and enable Chain 0 for CDMA */
      api_ptr[1]( ll_desc_ptr[0], ll_desc_ptr[1] );
    }
    else
    {
      MSG_ERROR("No APIs defined for Multimode WCDMA Handover",0,0,0);
    } 
  }
}

void rfapi_multimode_cdma_handover
(
  rfapi_device_desc_type dev_desc_c0,       /* Chain 0 device descriptor */
  rfapi_device_desc_type dev_desc_c1,       /* Chain 1 device descriptor */
  boolean enable_c1
)
{
  device_descriptor_type *desc_ptr[2];
  rfll_device_desc_type *ll_desc_ptr[2];
  rfapi_multimode_cdma_handover_type api_ptr[2];
  rfcom_mode_enum_type mode[2];

  desc_ptr[0] = (device_descriptor_type*) dev_desc_c0;
  desc_ptr[1] = (device_descriptor_type*) dev_desc_c1;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr[0] );
  verify_descriptor( desc_ptr[1] );

  ll_desc_ptr[0] = desc_ptr[0]->ll_dev_desc_ptr;
  ll_desc_ptr[1] = desc_ptr[1]->ll_dev_desc_ptr;
  mode[0] = ll_desc_ptr[0]->mode;
  mode[1] = ll_desc_ptr[1]->mode;


  /* Check that Chain 0 is CDMA and Chain 1 is CDMA */
  if ( mode[0] != RFCOM_1X_MODE )
  {
    MSG_ERROR("Can't do Multimode CDMA Handover when (c0,c1) modes are (%d,%d)",
              mode[0],mode[1],0);
  }
  else
  {
    /* CDMA1X CDMA Handover function for TRANSCEIVER_0 */
    api_ptr[0] = desc_ptr[0]->rfapi_multimode_cdma_handover[RFCOM_1X_MODE];
    /* WCDMA CDMA Handover function for RECEIVER_0 */
    api_ptr[1] = desc_ptr[1]->rfapi_multimode_cdma_handover[RFCOM_WCDMA_MODE];

    if ( (api_ptr[0] != NULL) && (api_ptr[1] != NULL) )
    {
      /* Disable Chain 0 which has CDMA */
      api_ptr[0]( ll_desc_ptr[0], ll_desc_ptr[1], enable_c1 );

      /* Enter WCDMA mode on Chain 1 */
      api_ptr[1]( ll_desc_ptr[0], ll_desc_ptr[1], enable_c1 );

      /*TBD need to change 1x state machine to reflect that we're in a MM state */
    }
    else
    {
      MSG_ERROR("No APIs defined for Multimode CDMA Handover",0,0,0);
    } 
  }
}

/*===========================================================================

FUNCTION  

DESCRIPTION

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_multimode_handover
(
  rfapi_device_desc_type dev_desc_c0,       /* Chain 0 device descriptor */
  rfapi_device_desc_type dev_desc_c1,       /* Chain 1 device descriptor */
  rf_multimode_handover_cmd_type dev_cmd_c0,/* Chain 0 mode command */
  rf_multimode_handover_cmd_type dev_cmd_c1 /* Chain 1 mode command */
)
{
  if ( (dev_cmd_c0 == RF_MM_CMD_CDMA) && (dev_cmd_c1 == RF_MM_CMD_SHUTDOWN) )
  {
    rfapi_multimode_wcdma_handover(dev_desc_c0, dev_desc_c1);
  }
  else if ( (dev_cmd_c0 == RF_MM_CMD_WCDMA) && (dev_cmd_c1 == RF_MM_CMD_NOP) )
  {
    rfapi_multimode_cdma_handover(dev_desc_c0, dev_desc_c1, FALSE);
  }
  else if ( (dev_cmd_c0 == RF_MM_CMD_WCDMA) && (dev_cmd_c1 == RF_MM_CMD_CDMA) )
  {
    rfapi_multimode_cdma_handover(dev_desc_c0, dev_desc_c1, TRUE);
  }
  else
  {
    MSG_ERROR("Unsupported multimode configuration (%d,%d):",
              dev_cmd_c0, dev_cmd_c1, 0);
  }
}

/*===========================================================================

FUNCTION  RFAPI_START_TX

DESCRIPTION
  Enables closed power control loops to control transmit power while on
  dedicated traffic channel.  Enables this only if current operating mode
  requires this.  Turns on PA.  May enable other mode specific functionality
  necessary for transmit operation while on the dedicated traffic channel.
  Can be used to enable Tx after hard handoff.

DEPENDENCIES
  rfapi_enable_tx() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_start_tx
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_start_tx_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_start_tx[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr );
  }
  
} /* rfapi_start_tx() */

/*===========================================================================

FUNCTION  RFAPI_STOP_TX

DESCRIPTION
  Disables closed power control loops or other mode specific functionality
  enabled by rfapi_start_tx().  Turns off PA.  Can be used to disable Tx
  during hard handoff.

DEPENDENCIES
  rfapi_start_tx() should have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_stop_tx
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_stop_tx_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_stop_tx[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr );
  }
  
} /* rfapi_stop_tx() */

/*===========================================================================

FUNCTION  RFAPI_IS_SLEEPING

DESCRIPTION
  Queries whether the RF is sleeping right now.

DEPENDENCIES
  None
  
RETURN VALUE
  True if RF is in sleep state, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean rfapi_is_sleeping ( rfapi_device_desc_type dev_desc )
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_is_sleeping_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return FALSE;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_is_sleeping[mode];

  /* Call handler function if function pointer not NULL */
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr ));
  } 

  return FALSE;
  
} /* rfapi_is_sleeping */


/*===========================================================================

FUNCTION  RFAPI_SLEEP

DESCRIPTION
  Places the majority of RF hardware in sleep mode for the currently
  enabled mode of operation and begins transition of RF driver to sleep
  state.  Actual state of hardware after calling this function is
  operating mode dependent and may require a call to rfapi_stay_asleep()
  later in the sleep timeline to complete the shutdown of all RF hardware
  for sleep.  This is the first stage of the transition to sleep state.

DEPENDENCIES
  Driver must be in the Rx state of operation.  
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfapi_sleep
(
rfapi_device_desc_type dev_desc,       /* Device descriptor */
const void *user_data_ptr,            /* Pointer to user data passed into
                                         callback */
rfcom_cb_handler_type cb_handler      /* Call back handler */  

)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_sleep_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_sleep[mode];

  /* Call handler function if function pointer not NULL else call
     callback function immediately and return delay of 0. */  
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr, user_data_ptr, cb_handler ));
  } else
  {
    if ( cb_handler != NULL )
    {
      cb_handler( RFCOM_TIME_DELAY_EXPIRED, (void *) user_data_ptr );
    }
    return (0);
  }

} /* rfapi_sleep() */


/*===========================================================================

FUNCTION  RFAPI_STAY_ASLEEP

DESCRIPTION
  Completes transition of RF hardware to sleep state.  The actual
  functionality performed is mode dependent and is the final stage of
  placing the RF hardware and drivers in sleep state.  Calling this
  function to enter the final sleep state may not be required for some
  modes of operation.  An example of what this function may do is
  tri-state the track low adjust PDM output after TCXO has been shutoff.  

DEPENDENCIES
  rfapi_sleep() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_stay_asleep
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_stay_asleep_type api_ptr;
  rfcom_mode_enum_type mode;

  #ifdef T_QUASAR
#error code not present
  #endif
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_stay_asleep[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr );
  }
  
} /* rfapi_stay_asleep() */

/*===========================================================================

FUNCTION  RFAPI_WAKEUP

DESCRIPTION
  Wakes up RF hardware from sleep mode to the mode of operation specified
  by input parameter rf_mode (WCDMA, GSM, CDMA, HDR, GPS, etc.).  Places
  RF driver in Rx state.  Typically, control loops are not started by this
  function and must be started by calling rfapi_start_loops().  Control
  loops are typically placed in override mode, outputting the override
  value when loop clocks become active.  The actual state of the control
  loops after calling this function are mode dependent.

  Callback function is called when RF hardware wakeup has completed.  For
  support of multiple events occurring during RF wakeup time, event
  parameter is passed into callback, as well as a pointer to user data that
  was passed in by caller.  Note: Callback executes in interrupt context. 

DEPENDENCIES
  rfapi_sleep() must have already been called and rfapi_stay_asleep()
  may have been called.

RETURN VALUE
  Returns total delay in microseconds this function will wait for the RF
  hardware to wakeup.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfapi_wakeup
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  rfcom_mode_enum_type rf_mode,         /* RF mode */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */  
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfapi_wakeup_type api_ptr;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Verify mode is valid. */ 
  verify_rf_mode( rf_mode );

  /* Set pointer to API handler function table entry. */
  api_ptr = desc_ptr->rfapi_wakeup[rf_mode];

  /* Call handler function if function pointer not NULL else call
     callback function immediately and return delay of 0. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( desc_ptr->ll_dev_desc_ptr, rf_mode, user_data_ptr,
                    cb_handler );
  }
  else
  {
    if ( cb_handler != NULL )
    {
      cb_handler( RFCOM_TIME_DELAY_EXPIRED, (void *) user_data_ptr );
    }
    return 0;
  }
  
} /* rfapi_wakeup() */

/*===========================================================================

FUNCTION  RFAPI_CAPTURE_LOOP_VALUES

DESCRIPTION
  Captures all tracking loop and PDM data that applies to a particular
  mode and state of operation.  Data is stored in union pointed to by
  data_ptr.  This data should not be accessed by caller. Data storage
  is allocated by the caller to allow the caller, for example, to
  maintain lists of loop values, when scanning inter-frequency or
  inter-RAT neighbor lists.  Loop values captured are restored by
  calling rfapi_restore_loop_values().

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_capture_loop_values
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  rfcom_loop_data_type *data_ptr        /* Pointer to location to store
                                           captured data. */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_capture_loop_values_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_capture_loop_values[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr, data_ptr );
  }
  
} /* rfapi_capture_loop_values() */

/*===========================================================================

FUNCTION  RFAPI_RESTORE_LOOP_VALUES

DESCRIPTION
  Restores all tracking loop and PDM data contained in union pointed to
  by data_ptr.  Data restored should have been previously captured with
  a call to rfapi_capture_loop_values().  Data is restored based on
  current mode of operation and RF driver state.

DEPENDENCIES
  rfapi_capture_loop_values() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_restore_loop_values
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  rfcom_loop_data_type *data_ptr        /* Pointer to data that will be
                                           restored */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_restore_loop_values_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_restore_loop_values[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr, data_ptr );
  }
  
} /* rfapi_restore_loop_values() */

/*===========================================================================

FUNCTION  RFAPI_STOP_LOOPS

DESCRIPTION
  Stops and freezes all tracking loops active in the current operating
  mode and RF driver state.  Keeps PDMs at constant value by outputting a
  fixed duty cycle PDM waveform.

DEPENDENCIES
  Tracking loops should be running.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_stop_loops
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_stop_loops_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_stop_loops[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr );
  }
  
} /* rfapi_stop_loops() */

/*===========================================================================

FUNCTION  RFAPI_START_LOOPS

DESCRIPTION
  Enables and unfreezes all tracking loops that are relevant to the
  current operating mode and RF driver state.

DEPENDENCIES
  Tracking loops should be stopped.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_start_loops
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_start_loops_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_start_loops[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr );
  }
  
} /* rfapi_start_loops() */

#if defined (FEATURE_RF_ENABLE_CM) || defined (FEATURE_RF_ENABLE_GTOW)
/*===========================================================================

FUNCTION  RFAPI_SETUP_INTERSYSTEM_MEAS

DESCRIPTION
  Sets up RF hardware for an intersystem measurement.  The mode of
  operation specified defines the mode to which the intersystem
  measurement will be made.  This function should be called when a
  measurement of an operating mode different from the current mode
  mode must be made to evaluate the possibility of handoff.
  
  Callback function is called when RF hardware has settled and is ready
  for use.  For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context.

DEPENDENCIES
  rfapi_init() must have already been called.
  rfapi_enter_mode() must have already been called.  
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfapi_setup_intersystem_meas
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */         
  rfcom_mode_enum_type rf_mode,         /* RF mode of intersystem measurement */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */  
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfapi_setup_intersystem_meas_type api_ptr;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Verify mode is valid. */ 
  verify_rf_mode( rf_mode );

  /* Set pointer to API handler function table entry. */
  api_ptr = desc_ptr->rfapi_setup_intersystem_meas[rf_mode];

  /* Call handler function if function pointer not NULL else call
     callback function immediately and return delay of 0. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( desc_ptr->ll_dev_desc_ptr, rf_mode, user_data_ptr,
                    cb_handler );
  }
  else
  {
    if ( cb_handler != NULL )
    {
      cb_handler( RFCOM_TIME_DELAY_EXPIRED, (void *) user_data_ptr );
    }
    return 0;
  }
  
} /* rfapi_setup_intersystem_meas() */

/*===========================================================================

FUNCTION  RFAPI_STOP_INTERSYSTEM_MEAS

DESCRIPTION
  Changes the configuration of the RF hardware so that it is no longer
  setup to make an intersystem measurement.  This function is typically
  called upon completion of an intersystem measurement.
  
  Callback function is called when RF hardware has settled and is ready
  for use.  For support of multiple events occurring during RF hardware
  settling time, event parameter is passed into callback, as well as a
  pointer to user data that was passed in by caller.  Note: Callback
  executes in interrupt context. 

DEPENDENCIES
  rfapi_setup_intersystem_meas() must have already been called.
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfapi_stop_intersystem_meas
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Callback handler */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_stop_intersystem_meas_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get intersystem mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->intersystem_mode;
  
  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_stop_intersystem_meas[mode];

  /* Call handler function if function pointer not NULL else call
     callback function immediately and return delay of 0. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( ll_desc_ptr, user_data_ptr, cb_handler );
  }
  else
  {
    if ( cb_handler != NULL )
    {
      cb_handler( RFCOM_TIME_DELAY_EXPIRED, (void *) user_data_ptr );
    }
    return 0;
  }
  
} /* rfapi_stop_intersystem_meas() */

/*===========================================================================

FUNCTION  RFAPI_TUNE_TO_INTERSYSTEM_CHAN

DESCRIPTION
  Tunes the UHF Rx synthesizer to the channel specified or configures the
  RF driver in preparation for a tune.  The channel is interpreted based
  on the intersystem measurement operating mode specified.

  Callback function is called when tuning is complete and synthesizer has
  locked or when RF driver configuration in preparation for a tune is
  complete.  For support of multiple events occurring during synthesizer
  lock time, event parameter is passed into callback, as well as a pointer
  to user data that was passed in by caller.  Note: Callback executes in
  interrupt context.

DEPENDENCIES
  rfapi_init() must have already been called.
  rfapi_enter_mode() must have already been called.  
  
RETURN VALUE
  Returns total delay in microseconds this function will wait for
  synthesizers to lock if a tune is performed.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfapi_tune_to_intersystem_chan
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */         
  rfcom_mode_enum_type rf_mode,         /* RF mode of intersystem measurement */
  uint16 channel,                       /* Mode dependent channel */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfapi_tune_to_intersystem_chan_type api_ptr;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Verify mode is valid. */ 
  verify_rf_mode( rf_mode );

  /* Set pointer to API handler function table entry. */
  api_ptr = desc_ptr->rfapi_tune_to_intersystem_chan[rf_mode];

  /* Call handler function if function pointer not NULL else call
     callback function immediately and return delay of 0. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( desc_ptr->ll_dev_desc_ptr, rf_mode, channel,
                    user_data_ptr, cb_handler );
  }
  else
  {
    if ( cb_handler != NULL )
    {
      cb_handler( RFCOM_TIME_DELAY_EXPIRED, (void *) user_data_ptr );
    }
    return 0;
  }
  
} /* rfapi_tune_to_intersystem_chan() */
#endif /* #if defined (FEATURE_RF_ENABLE_CM) || defined (FEATURE_RF_ENABLE_GTOW) */

/*===========================================================================

FUNCTION  RFAPI_DO_TX_PWR_LIMIT

DESCRIPTION
  Adjusts the Tx power.  Reads HDET value and adjusts Tx power based
  on HDET value, temperature, and frequency.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int16 rfapi_do_tx_pwr_limit
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_do_tx_pwr_limit_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return -1;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_do_tx_pwr_limit[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr ));
  }
  
  return 0;

} /* rfapi_do_tx_pwr_limit() */

/*===========================================================================

FUNCTION  RFAPI_DO_TEMP_COMP

DESCRIPTION
  Performs temperature compensation.  Adjusts AGC linearizer tables
  based on temperature.  Must be called at 10 second intervals to perform
  temperature compensation.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_do_temp_comp
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_do_temp_comp_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_do_temp_comp[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr );
  }
  
} /* rfapi_do_temp_comp() */

/*===========================================================================

FUNCTION  RFAPI_SET_TX_PWR_LIMIT

DESCRIPTION
  Sets the Tx power limit to value given in units of dBm.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_set_tx_pwr_limit
(
  rfapi_device_desc_type dev_desc,      /* Device descriptor */
  int16 tx_limit_dbm                    /* Tx power limit in dBm */
  #ifdef FEATURE_WCDMA_MAX_TX_PWR_NV_LIM_FIX
,rfcom_txplim_channel_type chan_type
  #endif
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_set_tx_pwr_limit_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_set_tx_pwr_limit[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
  #ifdef FEATURE_WCDMA_MAX_TX_PWR_NV_LIM_FIX
    api_ptr( ll_desc_ptr, tx_limit_dbm, chan_type );
  #else
    api_ptr( ll_desc_ptr, tx_limit_dbm );
  #endif
  }
  
} /* rfapi_set_tx_pwr_limit() */


/*===========================================================================

FUNCTION  RFAPI_GET_TX_PWR_LIMIT

DESCRIPTION
  Gets the Tx power limit value in units of dBm.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int16 rfapi_get_tx_pwr_limit
(
rfapi_device_desc_type dev_desc      /* Device descriptor */

)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_get_tx_pwr_limit_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_get_tx_pwr_limit[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr ));
  }

  return (0);

} /* rfapi_get_tx_pwr_limit() */


/*===========================================================================

FUNCTION  RFAPI_GET_RX_AGC_VAL

DESCRIPTION
  Returns the Rx AGC value in units of dBm.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Rx AGC value in units of dBm.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfapi_get_rx_agc_val
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_get_rx_agc_val_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_get_rx_agc_val[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( ll_desc_ptr );
  }

  /* Return invalid Rx AGC value. */
  return 1000;
  
} /* rfapi_get_rx_agc_val() */

/*===========================================================================

FUNCTION  rfapi_get_receive_agc_val

DESCRIPTION
  Returns Rx AGC value.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Rx AGC value.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfapi_get_receive_agc_val
(
rfapi_device_desc_type dev_desc,       /* Device descriptor */
rfcom_rxagc_type rxagc_type,
rfcom_rxagc_unit_type rxagc_unit
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_get_receive_agc_val_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_get_receive_agc_val[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr, rxagc_type, rxagc_unit ));
  }

  /* Return invalid Rx AGC value. */
  return (1000);

} /* rfapi_get_receive_agc_val() */


/*===========================================================================

FUNCTION  RFAPI_GET_RECEIVE_AGC_VAL_FOR_RSCP

DESCRIPTION
  Returns the Rx AGC value for RSCP in units of dBm.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Rx AGC value for RSCP in units of dBm.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfapi_get_receive_agc_val_for_rscp
(
rfapi_device_desc_type dev_desc,       /* Device descriptor */
rfcom_rxagc_type rxagc_type
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_get_receive_agc_val_for_rscp_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_get_receive_agc_val_for_rscp[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr, rxagc_type ));
  }

  /* Return invalid Rx AGC value. */
  return (1000);

} /* rfapi_get_receive_agc_val_for_rscp() */


/*===========================================================================

FUNCTION  RFAPI_GET_TX_AGC_VAL

DESCRIPTION
  Returns the Tx AGC value in units of dBm.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Tx AGC value in units of dBm.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfapi_get_tx_agc_val
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_get_tx_agc_val_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_get_tx_agc_val[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( ll_desc_ptr );
  }

  /* Return invalid Tx AGC value. */
  return -1000;
  
} /* rfapi_get_tx_agc_val() */

#ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
/*===========================================================================

FUNCTION  rfapi_get_min_tx_power_val

DESCRIPTION
  Returns Minimum Tx Power value.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Rx AGC value.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfapi_get_min_tx_power_val
(
rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_get_min_tx_power_val_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_get_min_tx_power_val[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr ));
  }

  /* Return Invalid Minimum Tx Power value. */
  return (-1000);

} /* rfapi_get_min_tx_power_val() */
#endif

/*===========================================================================

FUNCTION  RFAPI_LOG_AGC

DESCRIPTION
  Logs AGC data values for QXDM.

DEPENDENCIES
  rfapi_init() must have already been called.
  rfapi_enter_mode() must have already been called.  
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_log_agc
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_log_agc_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_log_agc[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr );
  }
  
} /* rfapi_log_agc() */

/*===========================================================================

FUNCTION  RFAPI_READ_TRK_LO_ADJ_VS_TEMP

DESCRIPTION
  This function returns a Trk_Lo_Adj value based on current temperature
  from the rfi_trk_lo_adj_vs_temp table.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Trk_Lo_Adj value in PDM counts.

SIDE EFFECTS
  None

===========================================================================*/
uint16 rfapi_read_trk_lo_adj_vs_temp
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_read_trk_lo_adj_vs_temp_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_read_trk_lo_adj_vs_temp[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( ll_desc_ptr );
  }

  /* Return default value. */
  return (2047);
  
} /* rfapi_read_trk_lo_adj_vs_temp() */


/*===========================================================================

FUNCTION  RFAPI_READ_ROT_FREQ_VS_TEMP

DESCRIPTION
  This function returns the searcher finger's rotator value based on current 
  temperature from the rfi_rot_freq_vs_temp table.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  Rotator value in HW units.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfapi_read_rot_freq_vs_temp
(
  rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_read_rot_freq_vs_temp_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_read_rot_freq_vs_temp[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( ll_desc_ptr );
  }

  /* Return default value. */
  return (0);
  
} /* rfapi_read_rot_freq_vs_temp() */

/*===========================================================================

FUNCTION  RFAPI_UPDATE_FREQ_ERROR_TABLE

DESCRIPTION
  This function updates the rfi_trk_lo_adj_vs_temp and rfi_rot_freq_vs_temp
  tables with the given values. rfi_rot_freq_vs_temp is updated based on 
  the state of donotupdaterotatorflag.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_update_freq_error_table
(
  rfapi_device_desc_type dev_desc,       /* Device descriptor */
  uint16 trkloadjval, 
  int16 rotatorval, 
  boolean donotupdaterotatorflag
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_update_freq_error_table_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }
  
  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_update_freq_error_table[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr, trkloadjval, rotatorval, donotupdaterotatorflag);
  }

} /* rfapi_update_freq_error_table() */

/*===========================================================================

FUNCTION  RFAPI_SAVE_FREQ_ERROR_TABLE

DESCRIPTION
  This function saves the contents of rfi_trk_lo_adj_vs_temp and 
  rfi_rot_freq_vs_temp tables into NV_TRK_LO_ADJ_VS_TEMP and 
  NV_ROT_FREQ_VS_TEMP items in NV.

DEPENDENCIES
  rfapi_init() must have already been called.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_save_freq_error_table
(
  rfapi_device_desc_type dev_desc,       /* Device descriptor */
  rex_tcb_type *caller_tcb_ptr,         /* Calling task's TCB pointer */
  rex_sigs_type task_nv_wait_sig,       /* Task signal to wait for when
                                           reading NV */
  void (*task_wait_func_ptr)( rex_sigs_type )
                                        /* Task's wait function to be called
                                           when reading NV */ 
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_save_freq_error_table_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_save_freq_error_table[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr, caller_tcb_ptr, task_nv_wait_sig,
             task_wait_func_ptr);
  }

} /* rfapi_save_freq_error_table() */

/*===========================================================================

FUNCTION  RFAPI_GET_STATE

DESCRIPTION
  This function returns the internal path configuration state to the calling
  function.

DEPENDENCIES
  rfm_init() must have already been called.
  
RETURN VALUE
  returns the internal state of type rf_path_state_type.

SIDE EFFECTS
  None

===========================================================================*/
rf_path_state_type rfapi_get_state(void)
{
    return rf_path_state;
} /* rfapi_save_freq_error_table() */

/*===========================================================================

FUNCTION RFAPI_GET_BAND_MASK                                   INTERNAL FUNCTION

DESCRIPTION
  Returns enabled/disabled state of bands set in NV item, NV_RF_HW_BC_CONFIG_I.

DEPENDENCIES
  None

RETURN VALUE
 sys_band_mask_type containing enabled/disabled states of bands. 

SIDE EFFECTS
  None

===========================================================================*/
sys_band_mask_type rfapi_get_band_mask
(  
    rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
    device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
    rfapi_get_band_mask_type api_ptr;

    /* Verify device has been successfully initialized and descriptor
       is valid. */
    verify_descriptor( desc_ptr );

    /* Set pointer to API function table entry. */
    api_ptr = desc_ptr->rfapi_get_band_mask;

    /* Call handler function if function pointer not NULL. */  
    if ( api_ptr != NULL )
    {
      return api_ptr();
    }

    /* Device initialization failed. */
    return 0;
  
} /* rfapi_get_band_mask() */

/*=========================================================================

DESCRIPTION RFAPI_IS_BAND_CHAN_SUPPORTED

  This function returns a boolean indicating whether the specified channel
  is supported by the chipset driver in the specified band.  If the band 
  or channel within the band is not supported FALSE is returned; otherwise 
  TRUE is returned.


DEPENDENCIES
  None
   

RETURN VALUE
  boolean:  TRUE   if band/channel is supported by the current devices 
                   configured in the library
            FALSE  if band/channel is not supported by the current devices 
                   configured in the library  

SIDE EFFECTS
  None.
===========================================================================*/
boolean rfapi_is_band_chan_supported
(
    rfapi_device_desc_type dev_desc,      /* Device descriptor */
    sys_band_class_e_type band, 
    word chan
)
{
    device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
    rfll_device_desc_type *ll_desc_ptr;
    rfapi_is_band_chan_supported_type api_ptr;
    rfcom_mode_enum_type mode;

    /* Verify device has been successfully initialized and descriptor
       is valid. */
    verify_descriptor( desc_ptr );

    /* Set pointer to logical layer descriptor and get mode. */
    ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
    mode = ll_desc_ptr->mode;

    if (mode >= RFCOM_NUM_MODES) 
    {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return FALSE;
    }

    /* Verify mode is valid. */ 
    verify_rf_mode( mode );

    /* Set pointer to API function table entry. */
    api_ptr = desc_ptr->rfapi_is_band_chan_supported[mode];

    /* Call handler function if function pointer not NULL. */  
    if ( api_ptr != NULL )
    {
      return api_ptr( ll_desc_ptr, band, chan);
    }

    return FALSE;
} /* rfapi_is_band_chan_supported() */


#ifdef FEATURE_RF_AFC

/*===========================================================================

FUNCTION  RFAPI_AFC_GET_VCO_DEFAULT

DESCRIPTION
  This function returns the vco accumulator default value.

DEPENDENCIES
  rfapi_init() must have already been called.

RETURN VALUE
  Default vco accumulator value in PDM counts.

SIDE EFFECTS
  None

===========================================================================*/
int16 rfapi_afc_get_vco_default( rfapi_device_desc_type dev_desc )
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_afc_get_vco_default_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor is valid.
   */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_afc_get_vco_default[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr ));
  }

  /* Return default value. */
  return(0);
}

/*===========================================================================

FUNCTION  RFAPI_AFC_GET_VCO_SLOPE

DESCRIPTION
  This function provides the slope and range values for vco.

DEPENDENCIES
  rfapi_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_afc_get_vco_slope
(
rfapi_device_desc_type dev_desc,       /* Device descriptor */
uint16 *vco_slope,
uint8 *vco_slope_range
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_afc_get_vco_slope_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor is valid.
   */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_afc_get_vco_slope[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr, vco_slope, vco_slope_range );
  }
}

/*===========================================================================

FUNCTION  RFAPI_AFC_GET_RGS_DATA

DESCRIPTION
  This function provides the data of recent good system, including
  system type, time, temperature, vco and rotator values.

DEPENDENCIES
  rfapi_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_afc_get_rgs_data
(
rfapi_device_desc_type dev_desc,       /* Device descriptor */
rfcom_afc_rgs_data_type *rgs_data
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_afc_get_rgs_data_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor is valid.
   */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_afc_get_rgs_data[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr, rgs_data );
  }
}

/*===========================================================================

FUNCTION  RFAPI_AFC_UPDATE_RGS_DATA

DESCRIPTION
  This function updates the data of recent good system, including
  system type, time, temperature, vco and rotator values. Note that this
  function only updates the ram shadow. Separate function needs to be called
  when needed to save data into NV memory.

DEPENDENCIES
  rfapi_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_afc_update_rgs_data
(
rfapi_device_desc_type dev_desc,       /* Device descriptor */
rfcom_afc_rgs_data_type *rgs_data
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_afc_update_rgs_data_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor is valid.
   */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_afc_update_rgs_data[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr, rgs_data );
  }
}

/*===========================================================================

FUNCTION  RFAPI_AFC_SAVE_NV_DATA

DESCRIPTION
  This function saves the data into NV memory. nv_item_mask specifies which
  particular items to save. Multiple items can be specified by ORing the
  masks.

DEPENDENCIES
  rfapi_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean rfapi_afc_save_nv_data
(
rfapi_device_desc_type dev_desc,
/* Device descriptor */
rfcom_afc_nv_item_mask_type nv_item_mask,
/* Mask to specify nv items to save */
rex_tcb_type *caller_tcb_ptr,
/* Calling task's TCB pointer */
rex_sigs_type task_nv_wait_sig,
/* Task signal to wait for when reading NV */
void (*task_wait_func_ptr)( rex_sigs_type )
/* Task's wait function to be called when reading NV */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_afc_save_nv_data_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor is valid.
   */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return FALSE;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_afc_save_nv_data[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr, nv_item_mask, caller_tcb_ptr,
                     task_nv_wait_sig, task_wait_func_ptr ));
  }

  return (FALSE);
}

#endif /* FEATURE_RF_AFC */

#ifdef FEATURE_COMPRESSED_MODE
/*===========================================================================

FUNCTION  rfapi_init_cm

DESCRIPTION
  This function initializes RF driver for Compressed Mode operation

DEPENDENCIES
  rfapi_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_init_cm
(
rfapi_device_desc_type dev_desc       /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_init_cm_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_init_cm[mode];

  /* Call handler function if function pointer not NULL. */
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr );
  }
} /* rfapi_init_cm */

/*===========================================================================

FUNCTION  rfapi_get_wtow_rf_ctl_buf

DESCRIPTION
  This function retrieves the RF SBI control buffer for Interfreq

DEPENDENCIES
  rfapi_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_get_wtow_rf_ctl_buf
(
rfapi_device_desc_type dev_desc,      /* Device descriptor */
uint16 arfcn,                         /* ARFCN */
void * buf_ptr
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_get_wtow_rf_ctl_buf_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_get_wtow_rf_ctl_buf[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr, arfcn, buf_ptr );
  }

} /* rfapi_get_wtow_rf_ctl_buf() */
#endif /* FEATURE_COMPRESSED_MODE */


#ifdef FEATURE_RF_ENABLE_GTOW
/*===========================================================================

FUNCTION  rfapi_build_gtow_scripts

DESCRIPTION
  This function commands RF driver to build the MDSP control scripts for
  GtoW operation

DEPENDENCIES
  rfapi_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_build_gtow_scripts
(
rfapi_device_desc_type dev_desc,      /* Device descriptor */
uint16 arfcn                          /* ARFCN */
) 
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_build_gtow_scripts_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_build_gtow_scripts[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr, arfcn );
  }

} /* rfapi_build_gtow_scripts() */
#endif /* FEATURE_RF_ENABLE_GTOW */

/*===========================================================================

FUNCTION  RFAPI_PA_IS_ON

DESCRIPTION
  This function checks if the PA is on. It returns 1 if PA is on and 
  0 if it's off. The returned value is a bitmap, bit 0 for PA_ON0 and
  bit 1 for PA_ON1.   

DEPENDENCIES
  rfapi_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint8 rfapi_pa_is_on
(
rfapi_device_desc_type dev_desc      /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_pa_is_on_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_pa_is_on[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr ));
  }

  return (0);
} /* rfapi_pa_is_on() */

/*===========================================================================

FUNCTION  RFAPI_GET_RF_DBG_SCR_PARAM

DESCRIPTION
  This function returns RF parameters for the debug screen.    

DEPENDENCIES
  rfapi_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean rfapi_get_rf_dbg_scr_param
(
rfapi_device_desc_type dev_desc,      /* Device descriptor */
rfcom_dbg_scr_param_type param,
void * buf_ptr
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_get_rf_dbg_scr_param_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return FALSE;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_get_rf_dbg_scr_param[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr, param, buf_ptr ));
  }
  return (FALSE);

} /* rfapi_get_rf_dbg_scr_param() */

/*==========================================================================

FUNCTION RFAPI_GET_RF_WARMUP_TIME

DESCRIPTION
  This function returns the required rf warmup time required by different rf targets 
  to L1 

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  uint16 : RF warmup time in uS.

SIDE EFFECTS
  None
===========================================================================*/
uint16 rfapi_get_rf_warmup_time
(
  rfapi_device_desc_type dev_desc      /* Device descriptor */
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_get_rf_warmup_time_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return 0;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_get_rf_warmup_time[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return api_ptr( ll_desc_ptr );
  }

  return 0;
  
}

/*===========================================================================

FUNCTION 

DESCRIPTION

DEPENDENCIES
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfapi_set_primary_path(rfapi_device_desc_type dev_desc,
                            rf_path_enum_type primary_path)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_set_primary_path_type api_ptr;
  rfcom_mode_enum_type mode;
  
  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;
  
  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_set_primary_path[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    api_ptr( ll_desc_ptr, primary_path );
  }

}


/*=========================================================================

DESCRIPTION RFAPI_IOCTL

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
rfcom_ioctl_return_type rfapi_ioctl
(
  rfapi_device_desc_type dev_desc,
  rfcom_ioctl_param_type request,
  void *pbuf,
  int32 length
)
{
  device_descriptor_type *desc_ptr = (device_descriptor_type*) dev_desc;
  rfll_device_desc_type *ll_desc_ptr;
  rfapi_ioctl_type api_ptr;
  rfcom_mode_enum_type mode;

  /* Verify device has been successfully initialized and descriptor
     is valid. */
  verify_descriptor( desc_ptr );

  /* Set pointer to logical layer descriptor and get mode. */
  ll_desc_ptr = desc_ptr->ll_dev_desc_ptr;
  mode = ll_desc_ptr->mode;

  if (mode >= RFCOM_NUM_MODES) 
  {
      MSG_FATAL("mode is invalid",0,0,0);
      rf_error_flag = TRUE;
      return RFCOM_IOCTL_UNKNOWN_ERROR;
  }

  /* Verify mode is valid. */ 
  verify_rf_mode( mode );

  /* Set pointer to API function table entry. */
  api_ptr = desc_ptr->rfapi_ioctl[mode];

  /* Call handler function if function pointer not NULL. */  
  if ( api_ptr != NULL )
  {
    return (api_ptr( ll_desc_ptr, request, pbuf, length ));
  }

  return (RFCOM_IOCTL_INVALID_DEVICE);
}









