#ifndef SRCH_GENLOG_H
#define SRCH_GENLOG_H
/*===========================================================================

                  S R C H L O G    H E A D E R    F I L E

DESCRIPTION
  This module contains functions pertaining to generalized
  searcher log packets

    Copyright (c) 2004 - 2009
                    by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_genlog.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/09   tjc     Added power control logging subpacket
12/15/08   vks     Created version 2 of RDDS log packet
10/06/08   vks     Reverted previous change
09/08/08   vks     Updated srch_genlog_rf_sub_v1 to include 
                   FEATURE_SRCH_SINGLE_ANTENNA
08/21/08   vks     Added FEATURE_SRCH_HAS_NO_INTERNAL_LOG_PACKETS to remove
                   internal log packets
04/12/07   sst     Added support for QLIC logging
02/13/07   pa      Fixed typo in variable name.
11/10/06   tjc     Update RX diversity autoswitching log packet
11/08/06   tjc     Created RX diversity autoswitching log packet
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
03/24/06   kwo     Renamed frame ser to frame sec
03/22/06   kwo     Added srch_idle_util_build_demod_stats_sub
01/30/05   pa/kwo  Changed timeline profile subpacket to version 2
01/16/06   awj     Moved to srch_stm interface
09/22/05   sst     Update Acq2.0 log subpacket to version 2
07/15/05   bt      Added profiling for ZZ2_1 timeline.
07/14/05   sfm     Mainlined FEATURE_1X_SRCH_USES_ROTATOR_AFC
06/20/05   ejv     Added srch_genlog_tc_set_maint_sub_v1 and
                   srch_genlog_tc_set_maint.
05/19/05   ejv     Moved SRCH_LOG_TNG_TC_STATS_C from 0x11A3 to 0x11A9.
05/13/05   ejv     Replaced srch_genlog_sub_id_type with uint8.
05/04/05   sst     File re-organization
05/02/05   ejv     Added set maint log packet and sub-packet IDs.
04/01/05   kwo     Added SRCH_LOG_NS_SRCH_C
04/01/05   sfm     Added srch_genlog_triage() and related definitions
03/31/05   sfm     Added timeline profiling.
03/31/05   sst     Updated comment for position in srch1_results to Cx2
03/22/05   sst     Removed references to srch4drv_t.h, changed reference to
                   srch4_srch_type to uint32 in logging functions
03/11/05   kwo     Modified field names for radio config in srch status sub
03/10/05   ddh     Added srch4drv_t.h include and changed variable type in
                   srch_genlog_1x_srch4_dump
11/29/04   sst     Added local define acq log pkt buffer size
10/29/04   ejv     Added srch_genlog_tc_stats and IDs for TC call stats
                   sub-packet and TC stats log code.  Added TC call stats
                   struct srch_genlog_tc_call_stats_sub_v1 from srchtc.c.
10/18/04   kwo     Updated sleep stats packet to include RF band and chan
10/15/04   bt      Added FEATURE_ZZ2_1
10/13/04   dmw     Added support for QPCH genlog sub-packet.
10/08/04   sst     Modified Acq log packet
10/07/04   kwo     Renamed gen log packets to SRCH_LOG_TNG...
10/06/04   kwo     Added support for finger channel estimate packet
10/06/04   jcm     Added sleep stats subpacket
10/01/04   sst     Added support for Acq2 logging
09/23/04   kwo     Added generalized demod status subpacket
09/16/04   jcm     Added generalized AFC subpacket
09/08/04   jcm     Rename exported genlog types to include srch_
09/08/04   jcm     Added generalized srch1 subpacket
09/03/04   jcm     Added generalized srch4 subpacket
09/03/04   jcm     Modified genlog to log multiple packet simultaneously
08/27/04   dmw     Added structs for active and finger info
08/17/04   kwo     Updated fields in rf subpacket (again)
08/09/04   kwo     Update fields for rf subpacket
08/03/04   kwo     Initial revision.

===========================================================================*/

/*-------------------------------------------------------------------------
      Includes
-------------------------------------------------------------------------*/
#include "srchmc.h"
#include "srch4trans_t.h"
#include "srch_afc_t.h"
#include "srch_hw_t.h"
#include "stm.h"


/*-------------------------------------------------------------------------
      Log Code IDs
-------------------------------------------------------------------------*/
/* Searcher has 22 log codes assigned starting with
   LOG_SEARCHER_RESERVED_CODES_BASE_C and ending with
   LOG_SEARCHER_LAST_2_C
   The log codes are generic to the diag interface and their usage is defined
   here by searcher.  Any of those 22 codes not defined here or in genlog.h
   are considered unused. Internal log codes should be taken starting from the
   last and working toward the first. */

#define SRCH_LOG_TNG_FINGER_STATUS_C          (0x119A)
#define SRCH_LOG_TNG_1X_SEARCHER_DUMP_C       (0x119B)
#define SRCH_LOG_TNG_GENERAL_STATUS_C         (0x119C)
#define SRCH_LOG_TNG_SRCH_DEMOD_INFO_C        (0x119D)
#define SRCH_LOG_TNG_SLEEP_STATS_C            (0x119E)
#define SRCH_LOG_TNG_QPCH_C                   (0x119F)
#define SRCH_LOG_TNG_AFC_INFO_C               (0x11A0)
#define SRCH_LOG_TNG_ACQ_INFO_C               (0x11A1)
#define SRCH_LOG_TNG_FING_CH_EST_C            (0x11A2)
#define SRCH_LOG_TNG_QLIC_INFO_C              (0x11A3)

/* internal searcher log codes */
#define SRCH_LOG_TNG_RX_DIV_AUTO_C            (0x11A7)
#define SRCH_LOG_TNG_TC_STATS_C               (0x11A9)
#define SRCH_LOG_TNG_TC_SET_MAINT_C           (0x11AA)
#define SRCH_LOG_TNG_ZZ_PROF_C                (0x11AB)
#define SRCH_LOG_TNG_TRIAGE_C                 (0x11AC)
#define SRCH_LOG_NS_SRCH_C                    (0x11AD)
#define SRCH_LOG_SECT_DUMP_C                  (0x11AE)
#define SRCH_LOG_FING_DRIVER_C                (0x11AF)

/*-------------------------------------------------------------------------
      Packet Version Codes
-------------------------------------------------------------------------*/

#define SRCH_GENLOG_VERSION_1                       1
#define SRCH_GENLOG_VERSION_2                       2
#define SRCH_GENLOG_VERSION_3                       3

/*-------------------------------------------------------------------------
      Sub-packet IDs
-------------------------------------------------------------------------*/
enum
{
  SRCH_SUB_ID_GENERAL_STATUS  = 1,
  SRCH_SUB_ID_RF,
  SRCH_SUB_ID_ACTIVE_INFO,
  SRCH_SUB_ID_FINGER_INFO,
  SRCH_SUB_ID_FILT_PILOT_SYMBOL,
  SRCH_SUB_ID_SEARCHER_1,
  SRCH_SUB_ID_SEARCHER_4,
  SRCH_SUB_ID_SLEEP_STATS,
  SRCH_SUB_ID_QPCH,
  SRCH_SUB_ID_AFC,
  SRCH_SUB_ID_TC_DEMOD_STATUS,
  SRCH_SUB_ID_CH_EST,
  SRCH_SUB_ID_ACQ,
  SRCH_SUB_ID_TC_CALL_STATS,
  SRCH_SUB_ID_ZZ_PROF,
  SRCH_SUB_ID_TRIAGE,
  SRCH_SUB_ID_TC_SET_MAINT,
  SRCH_SUB_ID_IDLE_DEMOD_STATUS,
  SRCH_SUB_ID_IDLE_SRCH_FILTER,
  SRCH_SUB_ID_TC_SRCH_FILTER,
  SRCH_SUB_ID_RX_DIV_AUTO,
  SRCH_SUB_ID_QLIC_INFO,
  SRCH_SUB_ID_TC_POWER_CTL
};

typedef enum
{
  SRCH_PKT_ID_FINGER_STATUS  = 0,
  SRCH_PKT_ID_1X_SRCH1_DUMP,
  SRCH_PKT_ID_1X_SRCH4_DUMP,
  SRCH_PKT_ID_GENERAL_STATUS,
  SRCH_PKT_ID_SRCH_DEMOD_INFO,
  SRCH_PKT_ID_SLEEP_STATS,
  SRCH_PKT_ID_QPCH,
  SRCH_PKT_ID_AFC,
  SRCH_PKT_ID_FING_CH_EST,
  SRCH_PKT_ID_ACQ,
  SRCH_PKT_ID_TC_STATS,
  SRCH_PKT_ID_ZZ_PROF,
  SRCH_PKT_ID_TRIAGE,
  SRCH_PKT_ID_TC_SET_MAINT,
  SRCH_PKT_ID_RX_DIV_AUTO,
  SRCH_PKT_ID_QLIC_INFO,

  SRCH_NUM_GENLOG_PACKETS

} srch_genlog_packet_id_type;


/*-------------------------------------------------------------------------
      General Header Data Types
-------------------------------------------------------------------------*/
typedef struct
{
  uint8                    id;         /* sub packet id */
  uint8                    ver;        /* sub packet version */
  uint16                   size;       /* sub packet size */

} srch_genlog_sub_header_type;


typedef struct
{
  uint8  version;                      /* generalized packet version */
  uint8  num_subpackets;               /* number of subpackets */
  uint16 reserved;                     /* padding so subpackets are aligned */

} srch_genlog_hdr_type;


/*-------------------------------------------------------------------------
      Variable Types
-------------------------------------------------------------------------*/

typedef void (*sub_commit_func_type)
(
 srch_genlog_packet_id_type   id,
 srch_genlog_sub_header_type *sub
);


/*-------------------------------------------------------------------------
      Defines for Log packets and subpackets
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_QPCH_C                   (0x119F)                    */
/*-------------------------------------------------------------------------*/
/* Array Field Maximums */
#define LOG_QPCH_MAX_IND2                           6
#define SRCH_GENLOG_MAX_QPCH_IND_CFG  ((LOG_QPCH_MAX_IND2 *                  \
                                        sizeof(srch_genlog_ind_data_type)) + \
                                       (LOG_QPCH_MAX_IND2 *                  \
                                        sizeof(srch_genlog_qpch_cfg_type)))

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_ACQ_INFO_C               (0x11A1)                    */
/*-------------------------------------------------------------------------*/
/* Acq log packet bugger size
   - MUST equal SRCH_ACQ_LOG_BUFFER_SIZE in srchacq_sm_t.h */
#define SRCH_GENLOG_ACQ_LOG_BUFFER_SIZE           128

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_QLIC_INFO_C              (0x11A3)                    */
/*-------------------------------------------------------------------------*/
/* Number of QLIC Engines/Sectors */
#define QLIC_MAX_NUM_SECTORS                        6

/* Number of Walsh Code Powers reported by MDSP */
#define QLIC_NUM_WCP                              128

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_TC_STATS_C               (0x11A9)                    */
/*-------------------------------------------------------------------------*/
/* Erasure histogram size */
#define SRCH_ERA_HIST_SIZ                           7

/* Message retransmit histogram size */
#define SRCH_RTX_MAX                               15

/* ECIO table size - MUST equal ECIO_TAB_SIZ in srchi.h */
#define SRCH_ECIO_TAB_SIZ                          64

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_TRIAGE_C                 (0x11AC)                    */
/*-------------------------------------------------------------------------*/
/* max # of bytes logged per triage run */
#define TRIAGE_LOG_BUF_SIZE                      2000



/*-------------------------------------------------------------------------
      Types for Log packets and subpackets
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*     RF Subpacket in TNG logs:                                           */
/*       SRCH_LOG_TNG_FINGER_STATUS_C          (0x119A)                    */
/*       SRCH_LOG_TNG_1X_SEARCHER_DUMP_C       (0x119B)                    */
/*       SRCH_LOG_TNG_GENERAL_STATUS_C         (0x119C)                    */
/*       SRCH_LOG_TNG_SRCH_DEMOD_INFO_C        (0x119D)                    */
/*       SRCH_LOG_TNG_FING_CH_EST_C            (0x11A2)                    */
/*       SRCH_LOG_TNG_TC_STATS_C               (0x11A3)                    */
/*       SRCH_LOG_TNG_RX_DIV_AUTO_C            (0x11A7)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  srch_genlog_sub_header_type   hdr;   /* subpacket header */
  uint8    rx0_band_class;             /* CAI band class for rx0 */  
  uint8    rx1_band_class;             /* CAI band class for rx1 */
  uint16   rx0_channel;                /* CDMA channel # for rx 0 */  
  uint16   rx1_channel;                /* CDMA channel # for rx 1 */
  int8     rx0_agc;                    /* AGC for rx0 */
  int8     rx1_agc;                    /* AGC for rx1 */
  int16    tx_pwr;                     /* CDMA Transmit power */
  int8     tx_gain_adj;                /* Transmit Power Control Value */
  uint8    tx_pwr_limit;               /* Transmit Power Limit Value */

} srch_genlog_rf_sub_v1;

/*-------------------------------------------------------------------------*/
/*     General Status Subpacket in TNG logs:                               */
/*       SRCH_LOG_TNG_GENERAL_STATUS_C         (0x119C)                    */
/*       SRCH_LOG_TNG_SRCH_DEMOD_INFO_C        (0x119D)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  srch_genlog_sub_header_type   hdr;   /* subpacket header */
  uint8    srch_state;                 /* searcher state */
  uint8    padding;                    /* padding for alignment */
  uint8    for_fch_sch0_rc;            /* forward radio config */
  uint8    for_sch1_dcch_rc;           /* forward radio config */

} srch_genlog_srch_status_sub_v1;

/*-------------------------------------------------------------------------*/
/*     Active Info Subpacket in TNG logs:                                  */
/*       SRCH_LOG_TNG_GENERAL_STATUS_C         (0x119C)                    */
/*       SRCH_LOG_TNG_SRCH_DEMOD_INFO_C        (0x119D)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  uint16 pilot_pn;                     /* pilot pn */
  uint8  cell;                         /* cell number */
  uint8  num_peaks;                    /* number of peaks for this active */
  uint32 window_center;                /* center of window */
  uint32 support_chan_mask;            /* mask of supported channels */
  uint32 peak_pos[ SRCH_MAX_PEAKS ];   /* storage for max peaks positions */
  uint16 peak_energy[ SRCH_MAX_PEAKS ];/* storage for max peaks energies */
  #if SRCH_MAX_PEAKS & 1
  uint16 reserved;                     /* if SRCH_MAX_PEAKS is odd, we need
                                          to reserve space */
  #endif

} srch_genlog_active_info_type;

typedef struct
{
  srch_genlog_sub_header_type hdr;     /* subpacket header */
  uint16  coh_int_leng;                /* coherent integration length */
  uint8   ncoh_int_leng;               /* non-coherent integration length */
  uint8   trunc_factor;                /* truncation factor */
  uint8   num_actives;                 /* Number of Actives */
  uint8   num_peaks;                   /* number of peaks */
  uint16  ref_pn;                      /* Reference Sector's PN */
  srch_genlog_active_info_type active_info[ SRCH_ASET_MAX ];

} srch_genlog_active_info_sub_v1;

/*-------------------------------------------------------------------------*/
/*     Finger Info Subpacket in TNG logs:                                  */
/*       SRCH_LOG_TNG_FINGER_STATUS_C          (0x119A)                    */
/*       SRCH_LOG_TNG_GENERAL_STATUS_C         (0x119C)                    */
/*       SRCH_LOG_TNG_SRCH_DEMOD_INFO_C        (0x119D)                    */
/*       SRCH_LOG_TNG_FING_CH_EST_C            (0x11A2)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  uint16   pilot_pn;                   /* finger's pilot assignment */
  uint16   rssi;                       /* current RSSI. If ANT_CONF =2,
                                          then this is RSSI for the paired
                                          finger. */
  uint16   rx0_rssi;                   /* RSSI for RX0, valid only
                                          for ANT_CONF=2 */
  uint16   rx1_rssi;                   /* RSSI for RX1, valid only
                                          for ANT_CONF=2 */
  uint32   position_x8;                /* absolute finger pos in Cx8 */
  uint32   enabled_channel_mask;       /* refer to format description */
  uint16   status;                     /* refer to format description */
  uint8    qlic_sect_id;               /* QLIC sector ID bitmask
                                          [7]   QLIC Enable Flag
                                          [6]   QLIC Override Flag
                                          [5:0] QLIC Sector ID (0-5) */
  uint8    reserved;                   /* you guessed it! Reserved! */

} srch_genlog_finger_info_type;

typedef struct
{
  srch_genlog_sub_header_type hdr;     /* subpacket header */

  uint8    pilot_filter_gain_fact;     /* gPF=PILOT_FILTER_GAIN_FACT/128 */
  uint8    div_comb_algo;              /* diversity combining algorithm:
                                          0 - MMSE
                                          1 - SPW */
  uint8    antenna_config;             /* 0 = RX0 (fingers operating on Rx0)
                                          1 = RX1 (fingers operating on Rx1)
                                          2 = DIV (receive diversity is ON)
                                          3 = Simultaneous Mode
                                             (fingers operating on Rx0)
                                          4 = Simultaneous Mode
                                             (fingers operating on Rx1)
                                             All other values are reserved */
  uint8    num_fingers;                /* number of finger records */

  /* Info records for each Finger */
  srch_genlog_finger_info_type fing_info[SRCH_ASET_MAX];

} srch_genlog_fing_info_sub_v1;

/*-------------------------------------------------------------------------*/
/*     Finger Filtered Pilot Subpacket in TNG logs:                        */
/*       SRCH_LOG_TNG_SRCH_DEMOD_INFO_C        (0x119D)                    */
/*       SRCH_LOG_TNG_FING_CH_EST_C            (0x11A2)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  srch_genlog_sub_header_type hdr;     /* subpacket header */

  uint8    antenna_config;             /* 0 = RX0 (fingers operating on Rx0)
                                          1 = RX1 (fingers operating on Rx1)
                                          2 = DIV (receive diversity is ON)
                                          3 = Simultaneous Mode
                                             (fingers operating on Rx0)
                                          4 = Simultaneous Mode
                                             (fingers operating on Rx1)
                                             All other values are reserved */

  uint8    pilot_filter_gain_fact;     /* gPF=PILOT_FILTER_GAIN_FACT/128 */

  uint8    num_fingers;                /* number of finger records */
  uint8    padding;

  /* Info records for each Finger */
  struct
  {
    int16  rx0_filt_I;
    int16  rx0_filt_Q;
    int16  rx1_filt_I;
    int16  rx1_filt_Q;

  } filtered_info[SRCH_ASET_MAX];

} srch_genlog_fing_filtered_pilot_sub_v1;

/*-------------------------------------------------------------------------*/
/*     Channel Estimater Subpacket in TNG logs:                            */
/*       SRCH_LOG_TNG_GENERAL_STATUS_C         (0x119C)                    */
/*       SRCH_LOG_TNG_SRCH_DEMOD_INFO_C        (0x119D)                    */
/*       SRCH_LOG_TNG_QPCH_C                   (0x119F)                    */
/*       SRCH_LOG_TNG_FING_CH_EST_C            (0x11A2)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  srch_genlog_sub_header_type   hdr;   /* subpacket header */
  uint32   ch_est;                     /* Channel estimate in linear ecio */

} srch_genlog_ch_est_v1;


/*-------------------------------------------------------------------------
      Types for External Log packets
-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_FINGER_STATUS_C          (0x119A)                    */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_1X_SEARCHER_DUMP_C       (0x119B)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  uint16   pos;                        /* peak position in Cx2 resolution */
  uint16   eng;                        /* peak energy */

} srch_genlog_srch1_result_type;

typedef struct
{
  srch_genlog_sub_header_type   hdr;   /* subpacket header */
  uint8    num_peaks;                  /* number of peaks returned */
  uint8    pilot_rec;                  /* pilot record type */
  uint8    gain;                       /* searcher pregain */
  uint8    non_coh;                    /* non-coherent int length */
  uint16   coh;                        /* coherent int length */
  uint16   pilot_pn;                   /* pilot pn */
  uint32   pilot_set;                  /* pilot set membership mask */
  uint16   win_pos;                    /* pn pos of window start (Chips)*/
  uint16   win_size;                   /* search window size (Chips) */
  srch_genlog_srch1_result_type results[ SRCH_NUM_PEAKS ];

} srch_genlog_srch1_sub_v1;

typedef struct
{
  uint32   peak_pos;                   /* peak position in Cx16 resolution */
  uint16   peak_eng;                   /* peak energy */
  uint16   reserved;

} srch_genlog_srch4_result_type;

typedef struct
{
  uint16   pilot_ant_burst_len_qof;    /* pilot, antenna, burst length, QOF */
  uint16   coh_trunc_coh_len;          /* coherent truncation and length */
  uint8    noncoh_len;                 /* non-coherent length */
  uint8    phase;                      /* phase */
  uint16   walsh_index;                /* walsh index */
  uint16   win_start;                  /* window start position in Chips */
  uint16   win_size;                   /* window size position in Chips */
  uint32   pilot_set_mask;             /* pilot set mask */
  srch_genlog_srch4_result_type   results[ SRCH_MAX_PEAKS ];

} srch_genlog_srch4_task_type;

typedef struct
{
  srch_genlog_sub_header_type   hdr;   /* subpacket header */
  uint8    tech;                       /* srch4 technology */
  uint8    queue;                      /* srch4 queue used */
  uint8    priority;                   /* search priority */
  uint8    qpch_cntl;                  /* QPCH control */
  int16    freq_offset;                /* frequency offset */
  uint8    num_tasks;                  /* number of tasks programmed */
  uint8    num_peaks;                  /* number of peaks programmed */
  srch_genlog_srch4_task_type   tasks[ SRCH4_MAX_NUM_TASKS ];

} srch_genlog_srch4_sub_v1;


/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_GENERAL_STATUS_C         (0x119C)                    */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_SRCH_DEMOD_INFO_C        (0x119D)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  srch_genlog_sub_header_type   hdr;   /* subpacket header */
  int32    ffpc_set_point;             /* Cur fch ffpc setpoint */
  int16    fpcsc_gain;                 /* Subchannel gain in dB */
  uint16   fch_ect_raw;                /* FCH Ect */
  uint16   fch_ecp_raw;                /* FCH Ecp */
  uint16   fch_nt_raw;                 /* FCH Nt  */
  uint8    fpc_enabled;                /* Forward Power Control Enabled ? */
  uint8    fpc_mode;                   /* Current Forward Power Control Mode */
  uint16   fpc_history;                /* Latest Forward Power Control hist */
  uint16   rpc_history;                /* Latest Reverse Power Control hist */
  uint8    current_rc;                 /* Current radio config during frame */
  uint8    decode_rate;                /* Current rate decision on frame */
  uint8    erasure;                    /* Indicator if frame is erasure */
  uint8    padding[3];
  uint16   frame_status[4];            /* CRC, Quality bits for all frame siz */
  uint16   ser[4];                     /* SER for decode as a full rate frame */
  int32    energy[4];                  /* Energy metrics for all frame sizes */

} srch_genlog_tc_demod_stats_sub_v1;


enum
{
  DEMOD_STATS_PCH=0,
  DEMOD_STATS_BCCH,
  DEMOD_STATS_FCCCH,
};

typedef struct
{
  srch_genlog_sub_header_type   hdr;    /* subpacket header */
  uint8  channel;                       /* Channel */
  uint8  crc;                           /* Frame CRC if supported */
  uint16 frame_sec;                     /* Frame Symbol error count */
  uint32 frame_energy;                  /* Frame Symbol energy */
} srch_genlog_idle_demod_stats_sub_v1;


typedef struct
{
  srch_genlog_sub_header_type   hdr;    /* Subpacket header */
  uint8    fpc_enabled;                 /* Forward Power Control Enabled ? */
  uint8    fpc_mode;                    /* Current Forward Power Control Mode */
  uint16   fpc_history_valid;           /* Whether the FPC history is valid */
  uint16   fpc_history;                 /* Latest Forward Power Control hist */
  uint16   rpc_history_valid;           /* Whether the RPC history is valid */
  uint16   rpc_history;                 /* Latest Reverse Power Control hist */
  uint16   padding;
  uint16   rpc_cell_history[6];         /* Cell-specific RPC history */
  uint16   fing_pwr_ctl_to_cell[6];     /* Finger <-> cell mapping */
} srch_genlog_tc_power_ctl_sub_v1;

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_SLEEP_STATS_C            (0x119E)                    */
/*-------------------------------------------------------------------------*/

enum
{
  SRCH_GENLOG_REACQ_NOT_ATTEMPTED = 0,
  SRCH_GENLOG_REACQ_SUCCESS,
  SRCH_GENLOG_REACQ_FAILURE,
  SRCH_GENLOG_REACQ_FAKE

};

enum
{
  SRCH_GENLOG_TECH_SLEEP_NOT_ATTEMPTED = 0,
  SRCH_GENLOG_TECH_SLEPT_IS2000,
  SRCH_GENLOG_TECH_SLEPT_IS95A,
  SRCH_GENLOG_TECH_SLEPT_ONLINE_QPCH,
  SRCH_GENLOG_TECH_NO_TIME_TO_SLEEP,
  #ifdef FEATURE_ZZ2_1
  SRCH_GENLOG_TECH_SLEPT_OFFLINE_QPCH
  #endif /* FEATURE_ZZ2_1 */

};

typedef struct
{
  uint16   rf_band_chan;               /* rf band and channel the mobile
                                          reacq'ed on */
  uint16   pilot_pn;                   /* current active pilot */
  int16    slew;                       /* reacq slew */
  uint16   time;                       /* time to reacq */
  uint8    type;                       /* type of reacq */
  uint8    nbrs_searched;              /* number of neighbors searched
                                          during prio */
  uint16   old_pilot_pn;               /* if hand-off, old active pilot */
  uint32   lecio;                      /* current active ecio */
  uint32   old_lecio;                  /* if hand-off, old active ecio */

} srch_genlog_reacq_type;

typedef struct
{
  uint16   reserved1;
  uint8    reserved2;
  uint8    nbrs_searched;              /* number of neighbors searched
                                          during prio */

} srch_genlog_prio_type;

typedef union
{
  srch_genlog_reacq_type   srch_genlog_reacq;
  srch_genlog_prio_type    srch_genlog_prio;

} srch_genlog_unified_sched_type;

typedef struct
{
  uint32   sleep_time;                 /* number of chips mobile slept */
  int16    rf_agc;                     /* RF AGC at Decode boundary */
  uint16   setup_time;                 /* time from stable RF and mobile
                                          ready to demod */

} srch_genlog_tech_type;

#ifdef FEATURE_ZZ2_1
typedef struct
{
  uint32   sleep_time;                 /* number of chips mobile slept */
  int16    rf_agc_wakeup;              /* RF AGC at Wakeup */
  int16    rf_agc_rec_end;             /* RF AGC at end of recording */
  uint16   timeline_active_time;       /* time in 100us units the TL was active */
  uint16   record_time;                /* time in chips spent recording */
  uint8    antenna;                    /* which antenna was used */
  uint8    queue;                      /* which queue was used */
  uint8    num_paths;                  /* number of demod paths */
  uint8    reserved;

} srch_genlog_offline_tech_type;
#endif /* FEATURE_ZZ2_1 */

typedef union
{
  srch_genlog_tech_type          srch_genlog_tech;
  #ifdef FEATURE_ZZ2_1
  srch_genlog_offline_tech_type  srch_genlog_offline_tech;
  #endif /* FEATURE_ZZ2_1 */

} srch_genlog_unified_tech_type;

typedef struct
{
  srch_genlog_sub_header_type   hdr;   /* subpacket header */
  uint8    slot_type;                  /* slot type (gen page,etc...)*/
  uint8    sci;                        /* SCI */
  uint16   slot_num;                   /* slot number */
  uint8    reacq_pkt_id;               /* reacq packet id */
  uint8    reacq_pkt_size;             /* reacq packet size */
  uint8    tech_pkt_id;                /* tech packet id */
  uint8    tech_pkt_size;              /* tech packet size */
  uint8    data_packets[ sizeof(srch_genlog_unified_sched_type) +
                         sizeof(srch_genlog_unified_tech_type) ];

} srch_genlog_sleep_stats_sub_v1;

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_QPCH_C                   (0x119F)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  uint8    status;                     /* Indicator demodulation result */
  uint8    type;                       /* Indicator type */
  uint8    thi;                        /* THI indicator threshhold for
                                          indicator of type */
  uint8    thb;                        /* THB indicator threshhold for
                                          indicator of type */
  uint16   position;                   /* Position of indicator */
  int16    ind_i_amp;                  /* Indicator amplitude on I channel */
  int16    ind_q_amp;                  /* Indicator amplitude on Q channel */
  uint16   com_pilot_eng;              /* Common Pilot Energy */
  uint16   div_pilot_eng;              /* Diversity Pilot Energy */
  uint16   reserved;

} srch_genlog_ind_data_type;

typedef struct
{
  uint16   pilot_pn;                   /* Pilot PN index */
  uint8    pi_walsh;                   /* Paging indicator walsh code */
  uint8    pi_pwr_level;               /* Paging indicator power level */
  boolean  bi_supported;               /* Broadcast indicator support */
  uint8    bi_walsh;                   /* Walsh code for broadcast indicators */
  uint8    bi_pwr_level;               /* Power level for broadcast indicator */
  boolean  cci_supported;              /* Config change indicators support */
  uint8    cci_walsh;                  /* Config change indicator walsh code */
  uint8    cci_pwr_level;              /* Config change indicator power level */
  uint16   reserved;                   /* Reserved */

} srch_genlog_qpch_cfg_type;

typedef struct
{
  srch_genlog_sub_header_type hdr;     /* Subpacket header */
  uint16   slot_number;                /* Paging slot */
  uint8    xfer_reason;                /* Reason for going back to idle */
  uint8    rate;                       /* QPCH indicator rate */
  uint8    num_qpch_cfg;               /* Number of overhead data entries */
  uint8    num_entries;                /* Number of indicator entries */
  uint8    reserved[2];                /* Reserved */
  uint8    qpch_ind_cfg[SRCH_GENLOG_MAX_QPCH_IND_CFG];

} srch_genlog_qpch_sub_v1;

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_AFC_INFO_C               (0x11A0)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  uint16   vco_acc;                    /* vco Accumulator */
  uint16   rot_acc;                    /* rotator Accumulator */
  uint16   delta_time;                 /* delta Time */
  uint16   rssi;                       /* rssi */

} srch_genlog_afc_entry_type;

typedef struct
{
  srch_genlog_sub_header_type   hdr;   /* subpacket header */
  qword    timestamp;                  /* system time of first entry */
  uint8    mode;                       /* frequency Tracking Mode */
  uint8    gain;                       /* frequency loop gain */
  uint8    filt_gain;                  /* pilot filter gain */
  uint8    band;                       /* rf band class */
  uint16   chan;                       /* rf cdma channel */
  uint16   vco_slope;                  /* vco slope */
  uint16   therm;                      /* Thermistor reading degC */
  uint16   reserved;
  uint32   num_entries;                /* number of freq entries */
  srch_genlog_afc_entry_type    entries[SRCH_AFC_LOG_BUFFER_SIZE];

} srch_genlog_afc_sub_v1;

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_ACQ_INFO_C               (0x11A1)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  uint8    id;                         /* State/Info identifier */
  uint8    timestamp;                  /* 8 LSB of current time
                                          in Ver1:  1.25ms resolution
                                          in Ver2+: det by opts[0:1] */
  int16    field_1;                    /* determined by id */
  int16    field_2;                    /* determined by id */

} srch_genlog_acq_attempt_type;

typedef struct
{
  srch_genlog_sub_header_type   hdr;  /* subpacket header */
  uint8    num_successes;             /* total number of successes
                                         since power-up */
  uint8    num_failures;              /* total number of failures
                                         since power-up */
  uint16   num_entries;               /* total number of entries
                                         in this packet */
  srch_genlog_acq_attempt_type  attempts[ SRCH_GENLOG_ACQ_LOG_BUFFER_SIZE ];

} srch_genlog_acq_sub_v1;

typedef struct
{
  srch_genlog_sub_header_type   hdr;  /* subpacket header */
  uint16   num_successes;             /* total number of successes
                                         since power-up */
  uint16   num_failures;              /* total number of failures
                                         since power-up */
  uint16   num_entries;               /* total number of entries
                                         in this packet */
  uint16   opts;                      /* bit field to select options
                                         [1:0] - Timestamp Resolution
                                                0 - 1.25ms
                                                1 - 1.00ms
                                                2 - reserved
                                                3 - reserved
                                         [3:2] - Attempt Type
                                                0 - Acquisition
                                                1 - System Det. Dedicated
                                                2 - System Det. Background
                                                3 - reserved
                                         [15:4] - reserved */
  srch_genlog_acq_attempt_type  attempts[ SRCH_GENLOG_ACQ_LOG_BUFFER_SIZE ];

} srch_genlog_acq_sub_v2;

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_FING_CH_EST_C            (0x11A2)                    */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_QLIC_INFO_C              (0x11A3)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  uint16   fing_mask;                 /* mask of fings assn to the eng */
  uint16   phys_sect;                 /* pilot pn of fings assn to the eng */
  uint32   win_max;                   /* the "max" position is equal to
                                         "earliest finger position +
                                          QLIC_WINDOW_SIZE" */
  uint32   win_min;                   /* the "min" position is equal to
                                         "latest finger position -
                                          QLIC_WINDOW_SIZE" */
  uint16   tot_ior;                   /* Total Ior of QLIC sector/engine */
  uint16   known_interference;        /* Known Interference */
  uint16   remaining_interference;    /* Remaining Interference */
  uint16   pilot_wt_sum;              /* Pilot Weighted Sum */
  uint16   pilot_sub[4];              /* Power of Pilot Sub */
  uint16   walsh_code_power[QLIC_NUM_WCP]; /* Walsh Code Powers */
} srch_genlog_qlic_engine_info_type;

typedef struct
{
  srch_genlog_sub_header_type  hdr;        /* subpacket header */
  uint16                       enable;     /* QLIC enabled bit field
                                               [0]    - QLIC Enabled
                                               [1]    - QLIC Override
                                               [15:2] - Reserved */
  uint16                       time_const;  /* QLIC time constant */
  uint16                       num_engines; /* Number of QLIC Engines */
  uint16                       num_wcp;     /* Number of Walsh Code Powers */
  srch_genlog_qlic_engine_info_type engine[ QLIC_MAX_NUM_SECTORS ];
                                            /* Array of QLIC Engines */
} srch_genlog_qlic_info_sub_v1;


#ifndef FEATURE_SRCH_HAS_NO_INTERNAL_LOG_PACKETS
/*-------------------------------------------------------------------------
      Types for Internal Log packets
-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_RX_DIV_AUTO_C            (0x11A7)                    */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_SRCH_RXDIV_AUTOSWITCH
typedef struct
{
  /* subpacket header */
  srch_genlog_sub_header_type hdr;

  /* current state */
  uint32 cur_state;

  /* diversity enabled? */
  uint8 diversity_enabled;

  /* capacity/quality decsions */
  uint8 tpr_decision;
  uint8 quality_decision;

  /* reserved */
  uint8 reserved;

  /* Ect/Ecp instantaneous/filtered values */
  int32 tpr_inst;
  int32 tpr_filt;

  /* number of SHO pilots */
  uint16 sho_pilots;

} srch_genlog_rx_div_auto_sub_v1;

typedef struct
{
  /* subpacket header */
  srch_genlog_sub_header_type hdr;

  /* current state */
  uint32 cur_state;

  /* next state */
  uint32 next_state;

  /* diversity enabled? */
  uint8 diversity_enabled;

  /* diversity granted? */
  /* This tells us whether the ALT chain is ready or not. This is useful
      because there could be situations when the autoswitching algorithm
      requests for Diversity to be enabled but the chain may not yet be
      ready */
  uint8 diversity_granted;

  /* capacity/quality decsions */
  uint8 tpr_decision;
  uint8 quality_decision;

  /* Ect/Ecp instantaneous/filtered values */
  int32 tpr_inst;
  /* For version 2 log packet, tpr_filt = SHO * tpr_inst */
  int32 tpr_filt;

  /* number of SHO pilots */
  uint16 sho_pilots;
  uint16 reserved1;

  /* Adding additional fields*/

  /* Ect/Ecp related */
  int32 tpr_comp;
  int32 tpr_thresh;

  /* FER related */
  int32 qual_hist_filt;
  int32 qual_hist_thr;

} srch_genlog_rx_div_auto_sub_v2;
#endif /* FEATURE_SRCH_RXDIV_AUTOSWITCH */

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_TC_STATS_C               (0x11A9)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  /* subpacket header */
  srch_genlog_sub_header_type hdr;

  /* Current RXC radio config */
  uint16 cur_rc;

  /* TC algo status */
  uint16 alg_status;

  /* Total number of frames and fwd erasures during this call */
  uint32 frames_tot;
  uint32 frames_era;

  /* Tx Off events and frames receved while Tx off */
  uint32 stop_tx;
  uint32 stop_tx_frames;

  /* Total number of searches of various sets */
  struct
  {
    uint32 aset;
    uint32 cset;
    uint32 pset;
    uint32 nset;
    uint32 rset;
    uint32 uset;

  } search_num;

  /* Total autonomous report events */
  struct
  {
    uint32 add;
    uint32 drop;
    uint32 comp;

  } events;

  /* Total number of PSMMs generated by report events */
  struct
  {
    uint32 tadd;
    uint32 tdrop;
    uint32 tcomp;
    uint32 pmro;
    uint32 pmrm;

  } psmm;

  /* Total of all the frame-quality readings in this call */
  uint32 fq_frames;
  uint32 fch_ect;
  uint32 fch_ecp;
  uint32 fch_nt;
  int32  fch_cor;

  /* Total number of fwd-rev messages */
  uint32 tot_fwd_msg;
  uint32 tot_rev_msg;

  struct
  {
    /* List the sizes used for the following tables */
    uint16 ho_state_siz;
    uint16 rtx_siz;
    uint16 era_siz;
    uint16 ecio_tab_siz;

    /* Number of frames in the various states of handoff */
    uint32 ho_state[SRCH_ASET_MAX];

    /* Number of retransmissions for rev messages */
    uint32 rtx_rev_msg[SRCH_RTX_MAX];

    /* Erasure histogram (measured every SRCHTC_FRAME_CHECK frames) */
    uint32 era[SRCH_ERA_HIST_SIZ];

    /* Combined EcIo histogram */
    uint32 ecio[SRCH_ECIO_TAB_SIZ];

    /* Rx histogram */
    uint32 rx[SRCH_ECIO_TAB_SIZ];

    /* Tx histogram */
    uint32 tx[SRCH_ECIO_TAB_SIZ];

    /* Adj histogram */
    uint32 adj[SRCH_ECIO_TAB_SIZ];

  } hist;

} srch_genlog_tc_call_stats_sub_v1;

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_TC_SET_MAINT_C           (0x11AA)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  srch_genlog_sub_header_type hdr;      /* subpacket header */

  uint8                aset_cnt;        /* number of aset members */
  uint8                cset_cnt;        /* number of cset members */

  uint8                soft_ho_thr_add; /* 1/2 dB */
  uint8                soft_ho_thr_drop[ SRCH_ASET_MAX ]; /* 1/2 dB */
  uint8                reserved1;
  uint16               t_tdrop;         /* drop timer in 26.6msec units */
  uint32               time_now;        /* 26.6ms unit reference */

  struct
  {
    int16              pilot;           /* pilot PN */
    uint16             reserved;
    uint32             raw_eng;         /* raw energy in lecio */
    uint32             filt_eng;        /* filtered energy in lecio */
    uint32             drop_timer;      /* count in 26.6msec units */
  } active[ SRCH_ASET_MAX ];

  struct
  {
    int16              pilot;           /* pilot PN */
    uint16             set;             /* set type, can or pre-can */
    uint8              t_comp_state[ SRCH_ASET_MAX ]; /* t_comp value */
    int16              pc_state;        /* pre-can state, if applicable */
    uint32             raw_eng;         /* raw energy in lecio */
    uint32             filt_eng;        /* filtered energy in lecio */
    uint32             drop_timer;      /* count in 26.6msec units */
  } candidate[ SRCH_CSET_MAX ];

} srch_genlog_tc_set_maint_sub_v1;

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_ZZ_PROF_C                (0x11AB)                    */
/*-------------------------------------------------------------------------*/

/* Version 2 */
typedef struct
{
  srch_genlog_sub_header_type hdr;     /* subpacket header */
  uint32   timeline;                   /* timeline type */
  uint32   wakeup[ 4 ];                /* wakeup struct */
  uint32   rtc_enable[ 5 ];            /* rtc_enable struct */
  uint32   common[ 6 ];                /* common struct */
  uint32   sub[ 7 ];                   /* online or offline sub */

} srch_genlog_zz_prof_sub_v2;

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_TRIAGE_C                 (0x11AC)                    */
/*-------------------------------------------------------------------------*/

typedef struct
{
  srch_genlog_sub_header_type hdr;     /* subpacket header */
  uint32  data_size;                   /* # bytes in data[] */
  uint8   data[ TRIAGE_LOG_BUF_SIZE ]; /* triage log data  */

} srch_genlog_triage_sub_v1;

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_NS_SRCH_DEBUG_C              (0x11AD)                    */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_SECT_DUMP_C                  (0x11AE)                    */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_FING_DRIVER_C                (0x11AF)                    */
/*-------------------------------------------------------------------------*/
#endif /* FEATURE_SRCH_HAS_NO_INTERNAL_LOG_PACKETS */


/*-------------------------------------------------------------------------
      Exported  Functions
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_FINGER_STATUS_C          (0x119A)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_FINGER_STATUS

DESCRIPTION
  This function sends logs a finger status packet using the generalized
  searcher logging format.  This packet shall include a rf sub-packet
  and a finger status sub-packet and may include additional sub-packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_finger_status( void );


/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_1X_SEARCHER_DUMP_C       (0x119B)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_1X_SRCH4_DUMP

DESCRIPTION
  This function builds a 1x Searcher Dump packet for searcher 4.  This packet
  shall include the rf subpacket and the searcher4 subpacket plus any
  additional subpacket included by the srch4 driver.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_1x_srch4_dump
(
  uint32 queue,                /* queue to log */
  uint32 num_tasks             /* number of tasks to log */
);

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_GENERAL_STATUS_C         (0x119C)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_GENERAL_STATUS

DESCRIPTION
  This function sends logs a general searcher status packet using the
  generalized searcher logging format.  This packet shall include a
  rf sub-packet, a active info sub-packet, and a finger status
  sub-packet and may include additional sub-packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_general_status( void );

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_SRCH_DEMOD_INFO_C        (0x119D)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_DEMOD_INFO

DESCRIPTION
  This function sends logs a demod info log packet in the  generalized
  searcher logging format.  This packet shall include a
  rf sub-packet, a active info sub-packet, and a finger status
  sub-packet and may include additional sub-packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_demod_info( void );

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_SLEEP_STATS_C            (0x119E)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_SLEEP_STATS

DESCRIPTION
  This function sends logs a searcher sleep stats packet using the
  generalized searcher logging format.  This packet shall include a
  sleep stats subpacket and may include additional sub-packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_sleep_stats( void );

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_QPCH_C                   (0x119F)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_QPCH

DESCRIPTION
  This function builds a QPCH Info packet using the generalized searcher
  logging format.  This packet shall include a QPCH sub-packet and may
  include additional sub-packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_qpch( void );

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_AFC_INFO_C               (0x11A0)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_AFC

DESCRIPTION
  This function builds an AFC Info packet using the generalized searcher
  logging format.  This packet shall include an AFC Info sub-packet and may
  include additional sub-packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_afc( void );

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_ACQ_INFO_C               (0x11A1)                    */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_ACQ2
/*===========================================================================

FUNCTION SRCH_GENLOG_ACQ

DESCRIPTION
  This function builds an ACQ2 Info packet using the generalized searcher
  logging format.  This packet shall include an ACQ2 Info sub-packet and may
  include additional sub-packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_acq( void );
#endif /* FEATURE_ACQ2 */

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_FING_CH_EST_C            (0x11A2)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_FING_CH_EST

DESCRIPTION
  This function sends logs a finger channel estimated log packet in the
  generalized searcher logging format.  This packet shall include a
  rf sub-packet, a finger status sub-packet, a pilot filtered sub-packet
  and may include additional sub-packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_fing_ch_est( void );

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_QLIC_INFO_C              (0x11A3)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_QLIC

DESCRIPTION
  This function builds an ACQ2 Info packet using the generalized searcher
  logging format.  This packet shall include an ACQ2 Info sub-packet and may
  include additional sub-packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_qlic( void );

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_RX_DIV_AUTO_C            (0x11A7)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_RX_DIV_AUTO

DESCRIPTION
  This function builds a RX Diversity Autoswitching packet using the
  generalized searcher logging format.  This packet shall include a RF
  sub-packet and a RX div auto sub-packet.  It may include additional
  sub-packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_rx_div_auto
(
  stm_state_type next_state         /* next state*/
);

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_TC_STATS_C               (0x11A9)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_TC_STATS

DESCRIPTION
  This function builds a TC statistics packet using the generalized searcher
  logging format.  This packet shall include a TC call stats sub-packet and
  a TC demod stats sub-packet.  It may include additional sub-packets.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_tc_stats( void );

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_TC_SET_MAINT_C           (0x11AA)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_TC_SET_MAINT

DESCRIPTION
  This function builds a TC set maintenance packet using the generalized
  searcher logging format.  This packet shall include a TC set maintenance
  sub-packet only.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_tc_set_maint(void);

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_ZZ_PROF_C                (0x11AB)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_ZZ_PROF

DESCRIPTION
  This function logs a ZZ Timeline Profile packet using the generalized
  searcher logging format.  This packet shall include a zz_prof subpacket only.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_zz_prof( void );

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_TRIAGE_C                 (0x11AC)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_TRIAGE

DESCRIPTION
  This function sends logs a triage packet using the  generalized searcher
  logging format.  This packet shall include a triage subpacket only.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_triage( void );

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_NS_SRCH_DEBUG_C              (0x11AD)                    */
/*-------------------------------------------------------------------------*/
/* find in srch_sched_sm.c */

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_SECT_DUMP_C                  (0x11AE)                    */
/*-------------------------------------------------------------------------*/

/* find in srch_sect.c */

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_FING_DRIVER_C                (0x11AF)                    */
/*-------------------------------------------------------------------------*/

/* find in srch_fing_driver.c */

#endif /* SRCH_GENLOG_H */

