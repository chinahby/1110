#ifndef SRCH_LIB_EXT_H
#define SRCH_LIB_EXT_H
/*====*====*====*====*====*====*====*========*====*====*====*====*====*====*==*

        S E A R C H   T A S K  --
        S E A R C H   L I B R A R Y   E X T E R N A L   H E A D E R

GENERAL DESCRIPTION
  This file contains library interfaces from non-librarized searcher code
  into the search library.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

      Copyright (c) 2004 - 2010
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_lib_ext.h#3 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/18/10   sst     Added TC AGC check flags and thresholds
08/31/09   vks     Added PCH->QPCH channel estimator threshold type
06/25/08   adw     Added min_sleep_setup_time_safewin_ms and _chips
06/20/08   mca     Merge from //depot
03/13/08   sst     Modify FTS version to be determined at runtime
01/11/08   aps     Add RTR6500_WARMUP_OVERHEAD outside of srch library
11/16/07   mca     Fixed compiler warnings
10/29/07   sst     Add (en/dis)able of GOVERN_ACQ_FAIL outside of srch library
                   Unfeaturize all code GOVERN_ACQ_FAIL code except init
06/21/07   sst     Added cgps_sw_overhead_*
06/18/07   mca     Removed use of nv items defines from library
02/22/07   aps     Removed merge errors in previous checkin
02/21/07   aps     Added VCO gain and SRL multiplier knob.
01/23/07   rkc     Removed prototype for srch_srm_init().
11/20/06   aps     Added prototype for srch_ftm_rx_tune().
11/08/06   tjc      Moved tunable RX diversity autoswitching parameters int
                     srch_lib_ext.c/h
07/11/06   tjc     Modified the traffic state to use the state machine module
06/15/06   pa      Added prototypes for functions exported by AFC and Srch SRM
03/22/06   kwo     Added prototype for srch_slot_get_current_pg_slot()
11/07/05   kwo     Removed srch_fing_force_deassign_now()
11/02/05   kwo     Added prototype for srch_get_gen_pilot_set()
09/23/05   ejv     Remove prototypes which are unused or included directly.
05/31/05   rng     Merged in from SHDR Sandbox.
05/24/05   ejv     Added additional prototypes for finger interface functions.
05/18/05   kwo     Updated prototypes for library
05/04/05   kwo     Added prototypes
02/17/05   sst     Moved reference to MDSP_APP_SRCH into srch_lib_ext.c/h to
                    facilite custumer compiles will differing Feature set
01/25/05   kwo     Exported some sched parms outside of library
12/28/04   bt      implementation, first cut

=============================================================================*/


/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "mdsp.h"
#include "rex.h"
#include "srchi.h"
#include "nv_items.h"


/*===========================================================================
             LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* enum to describe which version of Full Time SHDR (FTS) to use */
typedef enum
{
  SRCH_USE_FTS_NONE   = 0,
  SRCH_USE_FTS_3      = 3,
  SRCH_USE_FTS_4S     = 4,
  SRCH_USE_FTS_4D     = 5
} srch_lib_ext_fts_ver_enum;

#define SRCH_USES_FTS_3 ( srch_lib_defs.common.fts_ver == SRCH_USE_FTS_3 )

/** COMMON DEFINES **/

#ifdef FEATURE_SRCH_RXDIV_AUTOSWITCH
typedef struct
{
  const int32  tpr_thresh_rc35;
  const int32  tpr_thresh_rc4;
  const int32  qual_hist_filt_gain;
  const int32  qual_hist_thr;
  const int32  qual_duration;
  const int32  prep_fpc_adj;

} srch_lib_ext_rx_div_auto_defines_type;
#endif /* FEATURE_SRCH_RXDIV_AUTOSWITCH */


typedef struct
{
  const rex_priority_type  srch_pri_max;
  const rex_priority_type  srch_pri;
  const mdsp_app_type      srch_mdsp_app;
  #ifdef FEATURE_SRCH_RXDIV_AUTOSWITCH
  const srch_lib_ext_rx_div_auto_defines_type rx_div_auto;
  #endif /* FEATURE_SRCH_RXDIV_AUTOSWITCH */

  /* speficies the version of FTS to use */
  const srch_lib_ext_fts_ver_enum fts_ver;

  /* AFC knob multiplier setting for VCO gain and SRL values. This setting
     should strictly be left at value AFC_VCO_GAIN_SRL_NORMAL.. Only customers
     with 'non-optimal' TCXOs will need to use the other values.

     NOTE: Using values other than AFC_VCO_GAIN_SRL_NORMAL will have
     performance impacts on both GPS and FLO, especially in moving scenarios */
  const uint8              afc_vco_gain_srl_multiplier;

  const boolean            tc_rxd_agc_ck;
  const int32              tc_rxd_agc_dis_delta;
  const int32              tc_rxd_agc_en_delta;

} srch_lib_ext_common_defines_type;

/** END COMMON DEFINES **/

/** INIT DEFINES **/

typedef struct
{
  const timetick_type cd_govern_dur_ms;
  const boolean       cd_govern;
  const timetick_type acq_govern_dur_ms;
  const boolean       acq_govern;
} srch_lib_ext_init_defines_type;

/** END INIT DEFINES **/

/** IDLE DEFINES **/

typedef struct
{
  /* The following types must never be changed to
     uint32 because then some of the comparisons
     with int32's in the code will fail due to
     promotion from int32 to uint32 */

  const uint8 rf_warmup_base_time_ms;
  const uint8 rf_warmup_cx8_on_time_ms;
  const uint8 warmup_sw_latency_safewin_ms;
  const uint8 tcxo_clock_switch_overhead_ms;
  const uint8 tcxo_warmup_ms;
  const uint8 clk_overhead_ms;
  const uint8 cgps_sw_overhead_ms;
  const uint8 rtr6500_warmup_overhead_ms;
  const uint8 min_sleep_setup_time_safewin_ms;

  const uint16 rf_warmup_base_time_us;
  const uint16 rf_warmup_cx8_on_time_us;
  const uint16 warmup_sw_latency_safewin_us;
  const uint16 tcxo_clock_switch_overhead_us;
  const uint16 tcxo_warmup_us;
  const uint16 clk_overhead_us;
  const uint16 cgps_sw_overhead_us;
  const uint16 rtr6500_warmup_overhead_us;

  const uint16 min_sleep_setup_time_safewin_chips;

} srch_lib_ext_warmup_defines_type;

typedef struct
{

  const uint32  reacq_win_noise_thresh_lecio;
  const int32   fast_raho_srch_penalty;
  const int32   fast_raho_reg_penalty;
  const uint32  failed_reacq_scan_time_ms;

} srch_lib_ext_sched_defines_type;

typedef enum
{
  THRESH_TYPE_OLD,
  THRESH_TYPE_NEW
}srch_lib_ext_ch_est_thresh_tab_type;

typedef struct
{
  const srch_lib_ext_ch_est_thresh_tab_type  ch_est_thresh_tab_type;
} srch_lib_ext_qpch_defines_type;


typedef struct
{
  const srch_lib_ext_warmup_defines_type warmup;
  const srch_lib_ext_sched_defines_type  sched;
  const srch_lib_ext_qpch_defines_type   qpch;

} srch_lib_ext_idle_defines_type;

/** END IDLE DEFINES **/

typedef struct
{
  const nv_items_enum_type nv_cdma_rx_diversity_ctrl_i;
  const nv_items_enum_type nv_cdma_rx_chain_sel_thresh_i;
  const nv_items_enum_type nv_srch_dbg_mask_i;
  const nv_items_enum_type nv_cdma_rx_div_bc_enable_mask_i;

} srch_lib_ext_nv_items_defines_type;

/** TRAFFIC DEFINES **/
/** END TRAFFIC DEFINES **/

typedef struct
{
  const srch_lib_ext_common_defines_type   common;
  const srch_lib_ext_init_defines_type     init;
  const srch_lib_ext_idle_defines_type     idle;
  const srch_lib_ext_nv_items_defines_type nv_items;

} srch_lib_ext_defines_type;


/*-------------------------------------------------------------------------
      Constants and Macros
-------------------------------------------------------------------------*/
extern const srch_lib_ext_defines_type srch_lib_defs;


/*-------------------------------------------------------------------------
      Externalized functions from srchidle_ctl

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern srch_state_enum_type srch_idle_ctl_state(void);

/*-------------------------------------------------------------------------
      Externalized functions from srch_idle_util

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void srch_idle_init(void);
extern boolean srch_idle_slotted_mode_active(void);


/*-------------------------------------------------------------------------
      Externalized functions from srch_slot

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern int16 srch_slot_get_next_slot_offset(void);

/* REVISIT: This function is being called by customers,
   we need to add a public interface for this */
extern uint16 srch_slot_get_current_pg_slot(void);


/*-------------------------------------------------------------------------
      Externalized functions from srch_chan

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void srch_chan_reset (void);

/*-------------------------------------------------------------------------
      Externalized functions from srchzz_qpch_util

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern uint32 srchzz_get_qpch_ch_est(void);


/*-------------------------------------------------------------------------
      Externalized functions from srch_pcg

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern srch_state_enum_type  srch_pcg_state ( void );


/*-------------------------------------------------------------------------
      Externalized functions from l1m

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void l1m_init (void);


/*-------------------------------------------------------------------------
      Externalized functions from srch_fing_driver

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void srch_fing_reset_chans( void );
extern uint32 srch_fing_get_max_fing_cnt( void );
extern void srch_fing_disable( uint32 first_fing, uint32 last_fing );
extern void srch_fing_init( void );
extern void srch_fing_set_dmod_pilot_gain( srch_rx_freq_range_type freq_range );


/*-------------------------------------------------------------------------
      Externalized functions from srch_diag

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void srch_diag_init (void);
extern void srch_get_gen_pilot_set ( void *pkt_ptr );
#ifdef FEATURE_IS95B_MDR
extern void srch_diag_aset_walsh ( srch_walsh_struct_type *ptr );
#endif /* FEATURE_IS95B_MDR */
extern void srch_diag_get_pilot_set ( srch_pilot_set_struct_type *pkt_ptr );

/*-------------------------------------------------------------------------
      Externalized functions from srchtc

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void  srch_tc_save_fpc_info ( boolean fpc_enabled, uint16 mode );

extern srch_state_enum_type  srch_tc_state ( void );

/*-------------------------------------------------------------------------
      Externalized functions from srch afc

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void srch_afc_init( void );
extern int16 srch_afc_read_vco_accum(void);

/*-------------------------------------------------------------------------
      Externalized functions from srch ftm

      For complete prototypes please look in the module header file
-------------------------------------------------------------------------*/
extern void srch_ftm_rx_tune
(
  srch_rx_band_type band,        /* band class     */
  srch_rx_chan_type chan         /* channel number */
);

#endif /* SRCH_LIB_EXT_H */

