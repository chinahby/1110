#ifndef MCLOG_H
#define MCLOG_H

/*===========================================================================

                  Main Control Event and Logging Functions

GENERAL DESCRIPTION
  This module contains utilities and functions related to logging of events
  and DIAG/DM packets inside MC

Copyright (c) 2000-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mclog.h_v   1.10   02 Oct 2002 22:52:32   louiel  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mclog.h#2 $ $DateTime: 2009/06/02 00:23:21 $ $Author: anishg $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/09   ag      Support to display network parameters on UI
02/15/07   sb      Removed featurization from VOIP states.
02/14/07   sb      Added VOIP logging structure.
11/21/06   sb      Added support for VOIP-1X handoffs.
07/10/06   fc      Added log packet definition for UMTS.
06/13/06   fc      Added support for status service order.
06/02/06   fc      Added IHO/AHO/APHO event definitions.
10/15/05   fc      Cleanup NCC featurization.
07/05/05   sb      Added MEID support.
06/17/05   fc      Fixed the issue of MER counters being incremented 
                   incorrectly if multiple GPM received in the assigned slot.
                   Added the suport for MER benchmark in non-slotted mode.
03/23/05   fc      Merged the following from MSMSHARED:
03/25/05   an       Changed the DIAG SUBSYS CMD values for DF
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/25/04   fc      Merged the following from MSMSHARED:
02/17/05   sb       Added event to indicate a registration was suppressed.
09/27/04   va      Merged from REL B/C archive:
07/20/04   bkm      Update MER support for 6100.
07/08/04   bkm      Re-organize support for MER counters.
08/23/04   az      Rel D Negative SCI - changes for dynamic feature
08/17/04   az      Diag command MC_DIAG_SUBSYS_SET_CUR_PREF_SCI for setting
                   current preferred sci
06/24/04   sb      Release C addendum changes for PLCM.
06/16/04   fc      Added DF support for Sync ID and Reconnect message.
03/15/04   yll     Added one more byte payload in WAKEUP_SLOT event.
12/16/03   fc      Added support for EVENT_SEG_UPM_ADDR_MISMATCH.
12/02/03   bkm     Updated CH_IND/EXT_CH_IND event reporting.
11/06/03   bkm     Added diag command to retrieve Paging MER counters.
10/17/03   ph      added support for new DIAG interface to retrieve MC state.
10/03/03   bkm     Add match type to EVENT_CP_MATCHED_MSG.
08/01/03   yll     Added support for Dynamic Feature.
12/02/02   yll     Added event logging for wakeup slot, matched msg, call
                   release, pref system reselection, etc.
11/07/02   az      Mainlined FEATURE_IS95B
10/02/02   yll     Linted the code.
10/02/02   az      Mainlined FEATURE_IS2000
03/27/02   sb      Added bcch_acquired event.
03/27/02   jrp     Moved mclog_ms_handoff_type outside FEATURE_IS2000_REL_A ifdef
03/26/02   va      Added mclog_ms_handoff_type for HO event reporting.
03/14/02   va      Added mclog_event_cc_instantiated_type and mclog_event_cc
                   _conref_change_type
02/26/02   fc      Added support for common channel monitored event.
02/13/02   va      Support for CC related events
06/26/01   sb      Merged the following from common archive:
           06/06/01   lcc     Added BS P_REV, and F/R RCs for FCH, DCCH,
                      and SCH to the IS2000 status packet.
04/27/01   fc      Changed function prototype of
                   mclog_event_report_pch_acquired.
04/18/01   fc      Featurized mclog_report_event_ch_ind under FEATURE_IS2000.
04/12/01   fc      Added/Moved all events logging data types / function
                   prototypes from mc.h and mccdma.h to here.
04/10/01   jq      Created the file to handle logging and diagpkt needs.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "log.h"
#include "diagpkt.h"
#include "mccapdf.h"
#include "mccdma.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

typedef PACKED struct
{
  boolean enable;
  word code_chan;         /* code for current channel - cdma only */
  byte qof_mask_id;

} chan_info_type;

typedef PACKED struct
{
  word    pilot_pn;     /* Pilot PNs currently in the active set */
  byte pilot_rec_type;
  chan_info_type fch;
  chan_info_type dcch;
  chan_info_type sch;

} mclog_active_set_type;

typedef PACKED struct
{
  boolean active;
  byte rate;
  byte walsh_id;

} mclog_aset_rsch_type;

LOG_RECORD_DEFINE(LOG_ASET_INFO_C)
  byte    rf_mode;                /* 0->analog,  1->cdma         */
  byte    band_class;             /* 0->Cellular, 1->PCS etc              */
  word    curr_chan;              /* center frequency channel             */
  byte    num_pilots;             /* number of pilots */

  mclog_active_set_type aset[CAI_N6M];
  byte    fsch_rate;      // the rate is the same for all FSCHs, only valid when at least one F-SCH is active
  mclog_aset_rsch_type rsch; // we only support one supplemental CH

LOG_RECORD_END

typedef LOG_ASET_INFO_C_type       log_aset_info_type;

typedef PACKED struct
{
  boolean enable;
  byte gating_rate; /* only applicable when control hold is enabled */

} mclog_control_hold_type;

DIAGPKT_REQ_DEFINE(DIAG_IS2000_STATUS_F)
DIAGPKT_REQ_END

DIAGPKT_RSP_DEFINE( DIAG_IS2000_STATUS_F )
  byte    rf_mode;                /* 0->analog,  1->cdma         */
  byte    band_class;             /* 0->Cellular, 1->PCS etc              */

  word    cdma_rx_state;          /* current state for cdma only          */
  int1    cdma_good_frames;       /* whether or not frames we are
                                     receiving are good -- cdma only      */
  word    analog_corrected_frames;/* good frame count - analog only       */
  word    analog_bad_frames;      /* bad frame count - analog only        */
  word    analog_word_syncs;      /*  -- analog only                      */

  word    entry_reason;           /* entry reason                         */
  word    curr_chan;              /* center frequency channel             */

  word    sid;                    /* Current System ID                    */
  word    nid;                    /* Current Network ID                   */
  word    locaid;                 /* Current Location ID                  */
  word    rssi;                   /* Current RSSI level                   */
  byte    power;                  /* Current mobile output power level    */
  byte    num_pilots;             /* number of pilots */

  mclog_active_set_type aset[CAI_N6M];

  mclog_control_hold_type control_hold;
  byte    fsch_rate;      // the rate is the same for all FSCHs
  mclog_aset_rsch_type rsch; // we only support one supplemental CH
  byte    bs_p_rev;
  PACKED struct
  {
    byte  for_rc;
    byte  rev_rc;
  } fch, dcch, sch;
DIAGPKT_RSP_END

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumerations for event report                                           */

/* Operation modes */
typedef enum 
{
  MCLOG_OP_MODE_POWERUP, /* Power up mode */
  MCLOG_OP_MODE_AMPS,    /* Analog operation mode */
  MCLOG_OP_MODE_CDMA     /* CDMA operation mode */

} mclog_event_op_mode_type;

/* Call processing states */
typedef enum
{
  CDMA_SYS_DET_S,    /* System determination state */
  CDMA_PIL_ACQ_S,    /* Pilot channel acquisition state */
  CDMA_SYNC_ACQ_S,   /* Sync channel acquisition state */
  CDMA_TIME_CHG_S,   /* Timing change state */
  CDMA_IDLE_S,       /* Idle state */
  CDMA_UPDATE_OVH_S, /* Update Overhead Information */
  CDMA_ORIG_S,       /* Mobile Station Origination Attempt */
  CDMA_PAGE_RES_S,   /* Page Response */
  CDMA_ORD_RES_S,    /* Mobile Station Order/Message Response */
  CDMA_REG_S,        /* Registration Access */
  CDMA_MSG_S,        /* Message transmission */
  CDMA_PACA_CAN_S,   /* PACA Cancel Substate */
  CDMA_TCI_S,        /* Traffic Channel Initialization */

  #ifdef FEATURE_IS2000_REL_A
  CDMA_TRAFFIC_S,        /* Traffic Channel State */
  #else
  CDMA_WFO_S,        /* Waiting For Order */
  CDMA_WFA_S,        /* Waiting for Mobile Station Answer */
  CDMA_CONV_S,       /* Conversation */
  #endif

  CDMA_RELEASE_S,    /* Release */
  
  /* The following states are for a VOIP-1X handoff */
  CDMA_WAIT_FOR_SRDM_S,   /* Wait for a SRDM from CM */
  CDMA_GET_TIMING_S,      /* Transfer timing from DO */
  CDMA_WAIT_FOR_UHDM_S,   /* Wait for a UHDM from CM */
  CDMA_GO_TO_TCH_S,       /* Go to the Traffic Channel */

  CDMA_MAX_S

} mclog_event_cp_state_type;

#ifdef FEATURE_IS2000_REL_A
/* Call control substates -- cannot have more than 16, including 0 (event payload is 4 bits) */
#define MAX_NUM_CALL_CONTROL_SUBSTATES 16

typedef enum
{
  CDMA_CC_WFO_S,     /* Waiting for Order */
  CDMA_CC_WFA_S,     /* Waiting for Answer */
  CDMA_CC_CONV_S,    /* Conversation */
  CDMA_CC_RELEASE_S, /* Release */
  CDMA_CC_MAX_S      /* Maximum value, used for range-checking */

} mclog_event_cc_state_type;

typedef PACKED struct
{
  unsigned int old_con_ref : 8;
  unsigned int new_con_ref : 8;

} mclog_event_cc_conref_change_type;

typedef PACKED struct
{
  unsigned int con_ref : 8;
  unsigned int init_substate : 4;

} mclog_event_cc_instantiated_type;
#endif

typedef PACKED struct
{
  unsigned short pn[6];
  PACKED struct
  {
    unsigned int s1       : 1;
    unsigned int s2       : 1;
    unsigned int s3       : 1;
    unsigned int s4       : 1;
    unsigned int s5       : 1;
    unsigned int s6       : 1;
    unsigned int reserved : 10;
  } in_sch_aset;

  PACKED struct
  {
    unsigned int sr1       : 1;
    unsigned int sr2       : 1;
    unsigned int sr3       : 1;
    unsigned int sr4       : 1;
    unsigned int sr5       : 1;
    unsigned int sr6       : 1;
    unsigned int reserved  : 10;
  } pwr_comb_ind;

} mclog_ms_handoff_type;

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

typedef PACKED struct
{
  unsigned int pch_active          : 1;
  #ifdef FEATURE_IS2000_REL_A_CC
  unsigned int fccch_active        : 1;
  unsigned int pri_bcch_active     : 1;
  #endif /* FEATURE_IS2000_REL_A_CC */
  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  unsigned int non_pri_bcch_active : 1;
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
  unsigned int qpch_active         : 1;
  unsigned int reserved            : 11;

} event_common_channel_monitored_type;

#ifdef FEATURE_IS2000_REL_A_CC
typedef enum
{
  MCLOG_PART_ADDR_MISMATCH = 0, /* Partial address mismatch */
  MCLOG_FULL_ADDR_MISMATCH = 1  /* Full address mismatch */

} mclog_seg_upm_addr_mismatch_type;

typedef PACKED struct
{
  byte addr_mismatch_type; 
    /* Type of address mismatch */
  byte segment_seq;    
    /* Sequence number of last UPM segment processsed by MS that results in
       address mismatch */

} mclog_event_seg_upm_addr_mismatch_type;
#endif /* FEATURE_IS2000_REL_A_CC */

/* Obsolete, use mclog_event_wakeup_slot_v2_type */
typedef byte mclog_event_wakeup_slot_type;

typedef PACKED struct
{
  byte  slot_mask;
    /* Wakeup slot mask: 
     *   Page Slot:           1
     *   Broadcast Page Slot: 2
     *   Broadcast Data Slot: 4
     */
  boolean slept;
    /* Indiate if Search has slept before wakeup. */

} mclog_event_wakeup_slot_v2_type;

/* This enum is defined specifically for reporting the match
** type in EVENT_CP_MATCHED_MSG.  Note that the MS, BCAST, ANN
** values allow for a bit-mask-like combinations if needed
** in the future. 
*/
typedef enum
{
  MCLOG_MSG_MATCH             = 0,      /* a catch all case */
  MCLOG_PAGE_MS_DIR_MATCH     = 1,      /* PAGE_MATCH */
  MCLOG_PAGE_BCAST_MATCH      = 2,      /* BROADCAST_MATCH */
  MCLOG_PAGE_ANN_MATCH        = 4       /* ANNOUCEMENT_MATCH */

} mclog_match_type;

typedef PACKED struct
{
  byte msg_type;       /* csch msg id */
  byte match_type;     /* contains mclog_match_type as byte */

} mclog_event_cp_matched_msg_type;

typedef byte mclog_event_release_type;

typedef enum
{
  MCLOG_RELEASE_MS_ORDER,          /* MS Release Order */
  MCLOG_RELEASE_BS_ORDER,          /* BS Release Order */
  MCLOG_RELEASE_ERM_ALL_CH,        /* ERM w/ CH_ID = all channels */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_ */

  MCLOG_RELEASE_MAX

} mclog_release_reason_type;

typedef PACKED struct
{
  unsigned short pn[CAI_N6M];
  PACKED struct
  {
    unsigned int sr2       : 1;
    unsigned int sr3       : 1;
    unsigned int sr4       : 1;
    unsigned int sr5       : 1;
    unsigned int sr6       : 1;
    unsigned int reserved  : 11;
  } sr;

} mclog_fch_dcch_acq_type;

typedef byte mclog_event_registration_suppressed_type;

typedef PACKED struct
{
  byte using_private_lcm;           /* True if mobile is using private long code mask */
  byte public_long_code_mask_type;  /* Type of public long code mask */

} mclog_event_long_code_mask_type;

/* ----------------------------------
** Event Report Packet for Idle, Reacq,
** Fast Reacq, and Access handoffs.
** ---------------------------------- */
typedef PACKED struct
{
  uint16 new_pilot;
  int8 ecio_old_pilot;
  uint8 ecio_old_pilot_frac;
  int8 ecio_new_pilot;
  uint8 ecio_new_pilot_frac;
  uint16 new_freq;
  uint8 new_band_class;
  uint8 idle_ho_type;  /* mcc_idle_ho_enum_type cast as uint8 */    

} mclog_event_idl_acc_handoff_type;

typedef enum
{
  MCLOG_IDLE_HO = 0,
  MCLOG_IDLE_HO_REACQ,
  MCLOG_IDLE_HO_FAST_REACQ_CHEAP,
  MCLOG_IDLE_HO_FAST_REACQ_EXPENSIVE

} mclog_idle_ho_enum_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Diag Subsystem Dispatch Command Code CALLP1X. see diagcmd.h */
#define MC_DIAG_SUBSYS_STATE_QUERY      2
#define MC_DIAG_SUBSYS_PAGING_MER_STAT  3
#define MC_DIAG_SUBSYS_SET_CUR_PREF_SCI 6
#define MC_DIAG_SUBSYS_DF_ITEMS_QUERY   7
#define MC_DIAG_SUBSYS_DF_ITEM_VERIFY   8
#define MC_DIAG_SUBSYS_DF_ITEMS_WRITE   9
#define MC_DIAG_SUBSYS_GET_NW_PARAMS   10

/*************** MC_DIAG_SUBSYS_DF_ITEMS_QUERY ***************/

/* Query the DF items. CP returns a string indicating the supported
 * DF items, with their corresponding values and other information */
typedef PACKED struct
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEMS_QUERY */
  diagpkt_subsys_header_type    hdr; 

} mc_diag_df_items_query_req_type;

typedef PACKED struct
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEMS_QUERY */
  diagpkt_subsys_header_type    hdr;
  /* Status indicates whether Dynamic Feature is supported */
  byte                          status;
  /* DF String buffer */
  char                          str[MCCAP_DF_MAX * (MAX_DF_STR_LEN + 6)]; 

} mc_diag_df_items_query_rsp_type;

/*************** MC_DIAG_SUBSYS_DF_ITEM_VERIFY ***************/

/* Verify a change made by the user */
typedef PACKED struct
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEM_VERIFY */
  diagpkt_subsys_header_type      hdr; 
  /* First byte is the item no. Second byte is the item value and the third
   * byte is the esn hash */
  char                            str[3];

} mc_diag_df_item_verify_req_type; 

typedef PACKED struct
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEM_VERIFY */
  diagpkt_subsys_header_type      hdr; 
  /* Status is TRUE if the esn hash in the request string matches with the phones
   * esn hash */
  byte                            status;
  char                            str[MCCAP_DF_MAX * (MAX_DF_STR_LEN + 6)]; 

} mc_diag_df_item_verify_rsp_type;

/*************** MC_DIAG_SUBSYS_DF_ITEMS_WRITE ***************/

/* Write the DF items back to the NV */
typedef PACKED struct
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEMS_WRITE */
  diagpkt_subsys_header_type      hdr; 
  char                            str[MCCAP_DF_MAX * (MAX_DF_STR_LEN + 6)]; 

} mc_diag_df_items_write_req_type; 

typedef PACKED struct
{
  /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_ITEMS_WRITE */
  diagpkt_subsys_header_type      hdr; 
  /* Status indicates whether DF NV write is successful */
  byte                            status;

} mc_diag_df_items_write_rsp_type;

/*************** MC_DIAG_SUBSYS_STATE_QUERY ***************/

/* Query the MC state.   */
#define MCLOG_MAX_NUM_CONNECTIONS CAI_MAX_NUM_CONNECTIONS

typedef PACKED struct
{
  diagpkt_subsys_header_type      hdr; 

} mclog_diag_state_query_req_type; 

typedef PACKED struct
{
  diagpkt_subsys_header_type      hdr; 
  uint8                           mc_state; /* same as returned by phone state - 63 */
  uint16                          event_count; /* same as returned by phone state - 63 */
   /* All information below is relevant only if mc_state is DIAG_D_CONV_S (0x86) */
  uint8                           num_cc;   /* Num Call Control Instances */
  PACKED struct
  {
    uint8    call_id;
    uint8    con_ref;
    uint16   so;
    uint8    sr_id;
    uint8    substate;
   } cc[MCLOG_MAX_NUM_CONNECTIONS];

} mclog_diag_state_query_rsp_type;

/*************** MC_DIAG_SUBSYS_SET_CUR_PREF_SCI ***************/

/* Set the current preferred slot cycle index. */

typedef PACKED struct
{
  int8 curr_pref_sci;

} mc_diag_sci_type;

typedef PACKED struct
{
  diagpkt_subsys_header_type    hdr; 
    /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_SET_CUR_PREF_SCI */
  mc_diag_sci_type              sci;

} mc_diag_cur_pref_sci_req_type;

typedef PACKED struct
{
  diagpkt_subsys_header_type      hdr;
    /* cmd_code, subsys_id, subsys_cmd_code: MC_DIAG_SUBSYS_DF_VERIFY */
  mc_diag_sci_type                sci;

} mc_diag_cur_pref_sci_rsp_type;

/*************** MC_DIAG_SUBSYS_GET_NW_PARAMS ***************/

/* Get the network parameters. */

typedef enum
{
  INTER_FREQ_IDLE_HO_COUNT,
  /* Interfrequency idle handoff count since last powerup */

  HARD_HO_COUNT
  /* Traffic channel hard handoff count since last powerup */
} mc_diag_nw_param_type;

typedef PACKED struct
{
  uint32 inter_freq_idle_ho_count;
  /* Interfrequency idle handoff count since last powerup */

  uint32 hard_ho_count;
  /* Traffic channel hard handoff count since last powerup */

  uint16 fwd_sch0_mux_option;
  /* Current Multiplex Option for F-SCH0 */

  uint8 fwd_sch0_radio_config;
  /* Current Radio Configuration for F-SCH0 */

  uint16 rev_sch0_mux_option;
  /* Current Multiplex Option for R-SCH0 */

  uint8 rev_sch0_radio_config;
  /* Current Radio Configuration for R-SCH0 */
  
} mc_diag_nw_params_values_type;

typedef PACKED struct
{
  diagpkt_subsys_header_type    hdr; 
    /* cmd_code, subsys_id, subsys_cmd_code:MC_DIAG_SUBSYS_GET_NW_PARAMS */

} mc_diag_get_nw_params_req_type;

typedef PACKED struct
{
  diagpkt_subsys_header_type      hdr;
    /* cmd_code, subsys_id, subsys_cmd_code, paramater values:
       MC_DIAG_SUBSYS_GET_NW_PARAMS */
  mc_diag_nw_params_values_type     params;

} mc_diag_get_nw_params_rsp_type;


/*************** MC_DIAG_SUBSYS_PAGING_MER_STAT ***************/

/* Defines and Externs for MC_DIAG_PAGING_MER_STAT Diag command */
#define MC_DIAG_PAG_RXED_REAL_SCI_ASGN_SLOT 0x01
#define MC_DIAG_PAG_RXED_REAL_SCI_NEXT_SLOT 0x02
#define MC_DIAG_PAG_RXED_TELE_SCI_ASGN_SLOT 0x04
#define MC_DIAG_PAG_RXED_TELE_SCI_NEXT_SLOT 0x08
#define MC_DIAG_REAL_SCI_PAGE_SLOT          0x10
#define MC_DIAG_TELE_SCI_PAGE_SLOT          0x20

/* One 80ms slot in 20ms */
#define MCLOG_ONE_SLOT 4

/* Variables for MER statistics */
typedef struct
{
  uint32 page_rxed_real_sci_asgn_slot_ctr;
  uint32 page_rxed_real_sci_next_slot_ctr;
  uint32 page_rxed_tele_sci_asgn_slot_ctr;
  uint32 page_rxed_tele_sci_next_slot_ctr;
  uint32 real_sci_page_slot_ctr;
  uint32 tele_sci_page_slot_ctr;
  boolean mer_stat_enabled; 
    /* Indicates if MER statistical operation is enabled for a slot. */
  qword slot_sys_time; 
    /* System time of slot */

} mclog_mer_type;

DIAGPKT_SUBSYS_REQ_DEFINE(CALLP1X, MC_DIAG_SUBSYS_PAGING_MER_STAT)
  byte   counters_bitmask;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(CALLP1X, MC_DIAG_SUBSYS_PAGING_MER_STAT)
  uint32          page_rxed_real_sci_asgn_slot_ctr;
    /* Counter of pages received in assigned slot via real SCI */

  uint32          page_rxed_real_sci_next_slot_ctr;
    /* Counter of pages received in next slot via real SCI*/

  uint32          page_rxed_tele_sci_asgn_slot_ctr;
    /* Counter of pages received in assigned slot via telescoping SCI */

  uint32          page_rxed_tele_sci_next_slot_ctr;
    /* Counter of pages received in next slot via telescoping SCI */

  uint32          real_sci_page_slot_ctr;
    /* Counter of real SCI slot */

  uint32          tele_sci_page_slot_ctr;
    /* Counter of telecoping SCI slot */

DIAGPKT_RSP_END

/*===========================================================================

FUNCTION MCLOG_STAT

DESCRIPTION
  This procedure processes a Status request and formats a Status
  Response type.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void * mclog_stat 
(
  PACKED void *req_pkt,      /* pointer to request packet  */
  word len                   /* length of request packet   */
);

/*===========================================================================
FUNCTION MCLOG_MARKOV

DESCRIPTION
  This returns Markov mode statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void * mclog_markov 
(
  PACKED void *req_pkt,      /* pointer to request packet  */
  word len                   /* length of request packet   */
);

/*===========================================================================
FUNCTION MCLOG_MARKOV_RESET

DESCRIPTION
  This procedure resets the markov statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void * mclog_markov_reset 
(
  PACKED void *req_pkt,      /* pointer to request packet  */
  word len                   /* length of request packet   */
);

/*===========================================================================

FUNCTION mclog_get_is2k_status

  This function sends IS2000 related channel information to DM via diag service

DEPENDENCIES
  None

RETURN VALUE
  The next state.

SIDE EFFECTS
  None

===========================================================================*/

PACKED void * mclog_get_is2k_status
(
  PACKED void *req_pkt,        /* pointer to request packet  */
  word len                     /* length of request packet   */
);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_OP_MODE

DESCRIPTION
   Updates internal data and report events if necessary when operation mode
   changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_op_mode
(
  mclog_event_op_mode_type new_op_mode
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_MSG_RXED

DESCRIPTION
  Report event Message received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_msg_rxed
(
  cai_rx_channel_id_type channel_id,
  byte *msg_ptr
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_MSG_TXED

DESCRIPTION
  Report event Message transmitted.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_msg_txed
(
  cai_tx_channel_id_type channel_id,
  byte *msg_ptr
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_TIMER

DESCRIPTION
  Report event Timers T1m, T5m, T20m, T21m, T30m, T72m, T40m, T41m, T42m,
  T50m, T51m, T52m, T53m, T55m, T57m, T64m, T65m or T68m expired.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_timer
(
  cai_timer_id_type timer_id
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_COUNTER

DESCRIPTION
  Report event Counters N1m, N14m, N15m, N2m, N3m, N5m, N11m threshold
  reached.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_counter
(
  cai_counter_id_type counter_id
);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CP_STATE

DESCRIPTION
  Report events Call processing state change, Slotted mode operation. and
  QPCH in use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_cp_state
(
  word new_state
);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CC_CREATE

DESCRIPTION
  Reports Call Control instantiated event for Release A and a
  corresponding CP state change event for NON-Release A

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_cc_create
(
 uint8 con_ref,
 cdma_cc_state_type new_state
);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CC_STATE

DESCRIPTION
  Report event Call Control State Change

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_cc_state
(
 cdma_cc_state_type old_state,
 cdma_cc_state_type new_state,
 uint8 con_ref
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_PCH_ACQUIRED

DESCRIPTION
  Report event paging channel acquired for the first time after power up or
  operation mode change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_pch_acquired
(
  boolean pagech_acq,
    /* Flag indicates whether MS acquires the paging channel */
  byte pagech,
    /* Paging channel number of the acquired PCH */
  word pn_offset
    /* PN offset of the BS whose paging channel is acquired */
);

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_BCCH_ACQUIRED

DESCRIPTION
  Report event broadcast control channel acquired for the first time after power up or
  operation mode change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_bcch_acquired
(
  boolean bcch_acq,
    /* Flag indicates whether MS acquires the broadcast control channel */
  byte bcch_walsh_code,
    /* Walsh code of the acquired broadcast control channel */
  word pn_offset
    /* PN offset of the BS whose broadcast control channel is acquired */
);
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_CSCH_HO

DESCRIPTION
  Report event idle handoff, access probe handoff and access handoff.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_csch_ho
(
  cdma_ho_type ho_state,
  boolean cheap_idle_ho,
  word new_pilot,
  word new_freq,
  byte new_band_class,
  int8 ecio_old_pilot,
  int8 ecio_new_pilot,
  cdma_ho_type ho_type
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_CH_IND

DESCRIPTION
  Report event channel indicator change.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_ch_ind
(
  cai_chind_type     ota_ch_ind,       /* Channel indicator */
  cai_ext_chind_type  ota_ext_ch_ind /* Extended Channel indicator */
);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_F_FDCCH_ACQUIRED

DESCRIPTION
  Report event F-FCH acquired or F-DCCH acquired or F-FCH plus F-DCCH
  acquired.

DEPENDENCIES
  mcc_event.ch_ind must already be set correctly.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_f_fdcch_acquired(void);

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_COMMON_CHANNEL_MONITORED

DESCRIPTION
  Report event common channel monitored.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_common_channel_monitored
(
  byte active_channel_mask
);

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_SEG_UPM_ADDR_MISMATCH

DESCRIPTION
  Report event segmented Universal Page Message address mismatch.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_seg_upm_addr_mismatch
(
  byte addr_mismatch_type,
  byte segment_seq
);
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION MCLOG_REPORT_WAKEUP_SLOT

DESCRIPTION
  Report event of the wakeup slot

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_wakeup_slot
(
  byte      mask,
  boolean   slept
);

/*===========================================================================

FUNCTION MCLOG_REPORT_MATCHED_MSG

DESCRIPTION
  Report the csch mobile addressed message, after it passes address matching
  and duplicate detection.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_matched_msg
(
  byte  msg_type,
  mclog_match_type  match_type
);

/*===========================================================================

FUNCTION MCLOG_REPORT_RELEASE

DESCRIPTION
  Report the call release with release reason

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_release
(
  mclog_release_reason_type reason
);

/*===========================================================================

FUNCTION MCLOG_REPORT_REGISTRATION_SUPPRESSED

DESCRIPTION
  Report that a Registration Message was suppressed.  The payload is the
  type of Registration that was suppressed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_registration_suppressed
(
  byte registration_suppressed_type
);

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_LONG_CODE_MASK

DESCRIPTION
  Report a change in the long code mask.  The mobile may switch between
  the private long code mask and public long code masks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_long_code_mask
(
  boolean always_report
  /* Normally, we check the new value against the old value to see if we
    need to report, but in the case of ECAM, we always need to report */
);

/*===========================================================================

FUNCTION MC_DIAG_INIT
  This function registers the subsystem dispatch command handler table with
  Diag.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mc_diag_init (void);

/*===========================================================================

FUNCTION MCLOG_MER_CONFIG
  This function enables or disables MER counter update. If MER counter update
  is enabled, update real/telecoping SCI page slot counters accordingly. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mclog_mer_config
(
  boolean enable_mer_stat,
  qword slot_sys_time
);

/*===========================================================================

FUNCTION MCLOG_MER_UPDATE_COUNTER
  This function updates MER counters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mclog_mer_update_counter
(
  boolean in_tele, /* Indicates if it is telescoping or not */
  boolean in_slot, /* Indicates if it is in-slot or not */
  qword   page_rxed_sys_time /* System time that Page rxed */
);

/*===========================================================================

FUNCTION MCLOG_INC_NW_PARAM
  This function increments of the value of the network paramater.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mclog_inc_nw_param
(
  mc_diag_nw_param_type param_type
);

#endif  /* MCLOG_H */

