
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mclog.c_v   1.19   02 Oct 2002 22:52:30   louiel  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mclog.c#2 $ $DateTime: 2009/06/02 00:23:21 $ $Author: anishg $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/09   ag      Support to display network parameters on UI
08/21/08   ag      Included cdma2kdiag.h as part of DIAG VU decoupling process
20/03/08   bb      Fixed klocwork errors
11/21/06   sb      Added support for VOIP-1X handoffs.
06/02/06   fc      Added IHO/AHO/APHO event report function.
05/01/06   an      Fixed Lint Errors
04/29/06   fc      Fixed lint error.
04/24/06   fc      Fixed Rel A featurization.
10/15/05   fc      Cleanup NCC featurization.
10/10/05   fh      Removed including of dec.h. changed dec_version to txc_dec_version                     
07/11/05   fh      Cleaned up header file including from MUX:
                     changed rxc.h to rxcmc.h
07/07/05   va      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
06/17/05   fc      Fixed the issue of MER counters being incremented 
                   incorrectly if multiple GPM received in the assigned slot.
                   Added the suport for MER benchmark in non-slotted mode.
05/03/05   fc      Merged the following from MSMSHARED:
04/27/05   an       Added support for 4GV service option
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/25/05   fc      Merged the following from MSMSHARED:
02/17/05   sb       Added event to indicate a registration was suppressed.
09/27/04   va      Merged from REL B/C archive:
07/08/04   bkm      Re-organize support for MER counters.
08/23/04   az      Rel D Negative SCI - changes for dynamic feature
08/18/04   fc      Removed silk incl flags on overhead messages.
08/17/04   az      Diag command MC_DIAG_SUBSYS_SET_CUR_PREF_SCI for setting
                   current preferred sci
06/28/04   bkm     Merged the following from IS2000_REL_B_C:
  06/24/04   sb      Release C addendum changes for PLCM.
06/16/04   fc      Added DF support for Sync ID and Reconnect message. 
06/08/04   bkm     Fixed typos in feature names within comments.
06/04/04   bkm     Lint cleanup.
05/28/04   bkm     Merged the following from IS2000_REL_B_C:                                                       
  03/15/04   yll     Added the slept flag in the SMS_WAKEUP event.
04/08/04   bkm     Channel mask updates.
01/05/04   bkm     Misc changes for DV F-PDCH and channel masks.
12/16/03   fc      Added support for EVENT_SEG_UPM_ADDR_MISMATCH.
12/02/03   bkm     Updated CH_IND/EXT_CH_IND event reporting.
11/06/03   bkm     Added diag command to retrieve Paging MER counters.
11/03/03   ph      fix r-dcch message logging.
10/24/03   ph      include mccccim.h to rid of compiler warning
10/23/03   va      Changed caii_ext_ch_ind3_pilot_rec_type to
                   mccdma_pilot_info_type
10/17/03   ph      added support for new DIAG interface to retrieve MC state.
10/03/03   bkm     Add match type to mclog_report_matched_msg().
10/03/03   bkm     Corrected setting of msg_id in mclog_report_event_msg_rxed() 
                   to exclude 2-bit prot_disc and removed PREV check from 
                   mclog_report_event_msg_txed().
09/11/03   yll     Added featurization for ACP include file.
09/09/03   yll     Dynamic Feature query directly from DF NV item.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/01/03   yll     Added support for Dynamic Feature.
07/23/03   bkm     Some more lint cleanup.
07/23/03   lh      Zero init static psist to make tools happy.
06/02/03   bkm     Lint cleanup.
04/17/03   bkm     Removed call to diag_nv_read to read the NAM in mclog_stat()
                   as the info in diagnv should be current.
12/02/02   yll     Added event logging for wakeup slot, matched msg, call
                   release, pref system reselection, etc.
10/02/02   yll     Linted the code.
10/02/02   az      Mainlined FEATURE_IS2000
09/30/02   sb      Mainlined FEATURE_SD20.
09/20/02   fc      Included mcsys.h only if FEATURE_SD20 is not defined.
08/22/02   va      Fixed the error when release substate can be reported as
                   WFA in events for REL_A off builds.
07/02/02   az      Added new voice service option FEATURE_SO_VOICE_SMV
06/13/02   yll     Changed the BCSMS feature name to be under Rel A CC.
04/09/02   yll     Fixed the logging mask for IS2000 RelA common channel.
03/27/02   sb      Added bcch_acquired event.
03/27/02   va      Fixed compiler issues for non-RELA build.
03/25/02   va      Added support for converting CC related events into CP state
                   change events for non-RELA builds.
03/11/02   va      Merged from MSM CP VU 119.
  02/20/02 az      Featurized 13K voice service option
02/26/02   fc      Added support for event common channel monitored.
02/22/02   fc      Obsolete event QPCH in use for Release A common channels.
02/13/02   sb      Merged the following from the common archive:
                   01/12/02   sst     Modified to use the new srch_* files that replaced srchdrv.c
                                      Includes a function name change:
                                      srch_get_dmod_id -> srch_util_get_dmod_id
                                      Also removed reference to enc_version() and replaced it
                                      with the function call srch_util_get_dmod_id()
02/13/02   va      Added support for CC related events
08/08/01   yll     Merged in the following from MSM archive:
                    Added ifdef to solve problem with non-IS2000 builds.
                    Added support for SO54, SO55.
                    Corrected some #ifdefs.
06/26/01   sb      Merged the following from common archive:
           06/11/01   lcc     Corrected compilation errors for non-IS2000 targets.
           06/06/01   lcc     Added BS P_REV, and F/R RCs for FCH, DCCH, and SCH
                      to the IS2000 status packet.
04/27/01   fc      Changed function prototype of
                   mclog_event_report_pch_acquired.
04/18/01   fc      Featurized mclog_report_event_ch_ind under FEATURE_IS2000.
                   Changed mclog_report_event_f_fdcch_acquired to report
                   F-FCH acquired event for non-IS2000 targets.
04/12/01   fc      Added/Moved events logging from mc.c and mccsup.c to here.
04/12/01   fc      Fixed compilation problem of mclog_get_is2k_status() with
                   SD20.
04/10/01   jq      Created the file to handle logging and diagpkt needs.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "mccccl.h"
#include "mccdma.h"
#include "rxcmc.h"
#include "diagbuf.h"
#include "diagi.h"
#include "cdma2kdiag.h"
#include "msg.h"

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

#include "mar.h"
#include "bit.h"
#include "mccap.h"
#include "mccsch.h"
#include "err.h"
#include "cai.h"
#include "event.h"
#include "mclog.h"
#include "mccdma.h"
#include "mccccim.h"
#include "snm.h"

/*lint -e818 -e655 */

/*===========================================================================

                     VARIABLES AND ROUTINES

===========================================================================*/
/*---------------------------------------------------------------------------
  CRC counts for paging and sync channels
---------------------------------------------------------------------------*/
static rxc_crc_cnt_type  rx_sc_crc_cnt={0,0};  /* catch rx's counts here */
static rxc_crc_cnt_type  sc_crc_cnt={0,0};
static rxc_crc_cnt_type  rx_pc_crc_cnt={0,0};  /* catch rx's counts here */
static rxc_crc_cnt_type  pc_crc_cnt={0,0};

/*--------------------------------------------------------------------------
  DB item data buffer for db reads
--------------------------------------------------------------------------*/
static db_items_value_type diag_db;

/*--------------------------------------------------------------------------
  Network Parameters values
--------------------------------------------------------------------------*/
static uint32 mclog_inter_freq_idle_ho_count = 0;
static uint32 mclog_hard_ho_count = 0;

/*--------------------------------------------------------------------------
   Holding info for events reporting
--------------------------------------------------------------------------*/
#ifndef FEATURE_IS2000_P2
LOCAL struct
{
  #ifdef FEATURE_AUTH
  boolean using_private_lcm;
  /* TRUE if the mobile is using the private long code mask; FALSE otherwise */
  #endif /* FEATURE_AUTH */

  cai_plcm_type public_long_code_mask_type;
  /* The type of public long code mask currently in use */
  mclog_event_op_mode_type curr_op_mode;
    /* Current operation mode */
  word curr_state;
   /* Current call processing state */
  boolean pagech_acq;
   /* Flag indicates whether paging channel is acquired for the first time
      after power up or operation mode change */

  #ifdef FEATURE_IS2000_REL_A_CC
  boolean bcch_acq;
    /* Flag indicates whether broadcast control channel is acquired for the
       first time after power up or operation mode change. */
  #endif /* FEATURE_IS2000_REL_A_CC */

  #ifndef FEATURE_IS2000_REL_A
  /* Needed for NON-RELA targets to report CP state change event
  correctly */
  mclog_event_cp_state_type tc_substate;
  #endif

} mclog_event = {

  #ifdef FEATURE_AUTH
  FALSE,
  #endif /* FEATURE_AUTH */

  CAI_PLCM_TYPE_ESN,
  MCLOG_OP_MODE_POWERUP, CDMA_SYS_DET_S, FALSE  /*lint !e641 */

  #ifdef FEATURE_IS2000_REL_A_CC
  ,FALSE
  #endif /* FEATURE_IS2000_REL_A_CC */

  #ifndef FEATURE_IS2000_REL_A
  ,CDMA_MAX_S //Initialize to an invalid value to catch errors
  #endif /* !defined(FEATURE_IS2000_REL_A) */

};
#else /* !defined(FEATURE_IS2000_P2) */
LOCAL struct
{
#ifdef FEATURE_AUTH
  boolean using_private_lcm;
  /* TRUE if the mobile is using the private long code mask; FALSE otherwise */
#endif /* FEATURE_AUTH */
  cai_plcm_type public_long_code_mask_type;
  /* The type of public long code mask currently in use */
  mclog_event_op_mode_type curr_op_mode;
    /* Current operation mode */
  word curr_state;
   /* Current call processing state */
  boolean pagech_acq;
   /* Flag indicates whether paging channel is acquired for the first time
      after power up or operation mode change */
#ifdef FEATURE_IS2000_REL_A_CC
  boolean bcch_acq;
    /* Flag indicates whether broadcast control channel is acquired for the
       first time after power up or operation mode change. */
#endif /* FEATURE_IS2000_REL_A_CC */
#ifndef FEATURE_IS2000_REL_A
  /* Needed for NON-RELA targets to report CP state change event
  correctly */
  mclog_event_cp_state_type tc_substate;
#endif

} mclog_event = {
#ifdef FEATURE_AUTH
  FALSE,
#endif /* FEATURE_AUTH */
  CAI_PLCM_TYPE_ESN,
  MCLOG_OP_MODE_POWERUP, CDMA_SYS_DET_S, FALSE
#ifdef FEATURE_IS2000_REL_A_CC
         ,FALSE
#endif /* FEATURE_IS2000_REL_A_CC */
#ifndef FEATURE_IS2000_REL_A
        ,CDMA_MAX_S //Initialize to an invalid value to catch errors
  #endif /* !defined(FEATURE)IS2000_REL_A) */

};
#endif /* !defined(FEATURE_IS2000_P2) */

/* Structure of information needed for MER counter support */
mclog_mer_type mclog_mer;

/*--------------------------------------------------------------------------
   Holding info for Diag Subsystem Dispatch command processing.
--------------------------------------------------------------------------*/

/* Dynamic Feature related command handlers. Note they are not under */
/* FEATURE_IS2000_DYNAMIC_FEATURE, because even if the feature is    */
/* disabled, we still may receive these commands from Tools          */

PACKED void * mc_diag_df_items_query_handler (PACKED void *, uint16);
PACKED void * mc_diag_df_item_verify_handler (PACKED void *, uint16);
PACKED void * mc_diag_df_items_write_handler (PACKED void *, uint16);
PACKED void * mc_diag_get_nw_params(PACKED void *, uint16);

/* MC State Query handler */
PACKED void * mclog_diag_state_query_handler (PACKED void *, uint16);

/* Paging MER Stats diag command handler */
PACKED void *mclog_paging_mer_stat (PACKED void* req_pkt, uint16 pkt_len);

/* set the current preferred slot cycle index */
PACKED void *mclog_diag_set_curr_pref_sci (PACKED void* req_pkt, uint16 pkt_len);

/* 1X_CP Diag Subsystem Dispatch Command. The dispatch table. */
static const diagpkt_user_table_entry_type mc_diag_tbl[] =
{
  {MC_DIAG_SUBSYS_STATE_QUERY, MC_DIAG_SUBSYS_STATE_QUERY, mclog_diag_state_query_handler},
  {MC_DIAG_SUBSYS_PAGING_MER_STAT, MC_DIAG_SUBSYS_PAGING_MER_STAT, mclog_paging_mer_stat},
  {MC_DIAG_SUBSYS_SET_CUR_PREF_SCI, MC_DIAG_SUBSYS_SET_CUR_PREF_SCI, mclog_diag_set_curr_pref_sci},
  {MC_DIAG_SUBSYS_DF_ITEMS_QUERY, MC_DIAG_SUBSYS_DF_ITEMS_QUERY, mc_diag_df_items_query_handler},
  {MC_DIAG_SUBSYS_DF_ITEM_VERIFY, MC_DIAG_SUBSYS_DF_ITEM_VERIFY, mc_diag_df_item_verify_handler},
  {MC_DIAG_SUBSYS_DF_ITEMS_WRITE, MC_DIAG_SUBSYS_DF_ITEMS_WRITE, mc_diag_df_items_write_handler},
  {MC_DIAG_SUBSYS_GET_NW_PARAMS, MC_DIAG_SUBSYS_GET_NW_PARAMS, mc_diag_get_nw_params}
};

/*===========================================================================

FUNCTION MCLOG_GOOD_FRAMES formerly DIAGP_GOOD_FRAMES

DESCRIPTION
  Check for good frames
  This function is taken from diagp.c

DEPENDENCIES
  None.

RETURN VALUE
  0 if bad, 1 if good, -1 if indeterminate

SIDE EFFECTS
  None.

===========================================================================*/

int1 mclog_good_frames
(
  rxc_crc_cnt_type     *cur,    /* Current good CRCs */
  rxc_crc_cnt_type     *old     /* Old good CRCs */
)
{
  static word psist = 0;
  int1 ret_val;

  /*-----------------------------------------------------------------------
    If the good and bad counters have reset, then reset the old values.
  -----------------------------------------------------------------------*/

  if( cur->good_crcs < old->good_crcs || cur->bad_crcs < old->bad_crcs ) {
    /* we've gone back to zero... */
    old->good_crcs = 0;
    old->bad_crcs = 0;
  }

  if( cur->good_crcs > old->good_crcs ) {
    /* we've had some good frames */
    old->good_crcs = cur->good_crcs;
    old->bad_crcs = cur->bad_crcs;
    psist = 2;
    ret_val = 1;                  /* good frames */
  }

  else if( cur->bad_crcs > old->bad_crcs+4 ) {
    /* no good frames, more than 5 bad */
    ret_val = 0;                  /* bad frames */
  }

  else if( psist != 0 ) {
    psist--;
    ret_val = 1;                  /* still return good for a few */
  }

  else {
    ret_val = -1;                 /* don't know */
  }

  return (ret_val);

} /* mclog_good_frames */

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
  PACKED void *req_pkt,            /* pointer to request packet  */
  word len                 /* length of request packet   */
)
{
  word              i;
  mcc_status_type entry; /* CDMA and Analog last entry reason */
  boolean           digital;
  diag_rsp_type *rsp_ptr;
  const int rsp_len = FSIZ(diag_rsp_type, stat);

  rsp_ptr = (diag_rsp_type *) diagbuf_pkt_alloc(rsp_len);

  /* if rsp_ptr is NULL return from this function */
  if( rsp_ptr == NULL)
  {
	  MSG_HIGH("Out of free DIAG Pkts",0,0,0);
      return NULL;
  }

  /*----------------------------------------------------------------------
    Fill in the packet with status information.
  -----------------------------------------------------------------------*/

  rsp_ptr->stat.cmd_code = DIAG_STATUS_F;

  rsp_ptr->stat.demod    = (byte)srch_util_get_dmod_id();
  rsp_ptr->stat.decode   = rxc_dec_version();
  rsp_ptr->stat.inter    = (byte)srch_util_get_dmod_id();
  rsp_ptr->stat.esn      = diagnv.esn;

  /* Get the current mode.  If RF mode is not Analog ( CDMA or PCN ) then
     declare digital,  otherwise declare analog
  */

  db_get ( DB_RF_MODE, &diag_db );

  #ifdef T_QUASAR_BB
#error code not present
  #endif

  if ( diag_db.rf_mode == DB_RF_ANALOG )
  {
    rsp_ptr->stat.rf_mode = 0;
  }
  else if ((diag_db.rf_mode == DB_RF_CDMA)    ||
           (diag_db.rf_mode == DB_RF_900MHZ))
  {
    /* --------------------------------------------------------
    ** For now, have the DM treat 900MHZ mode just like regular
    ** CDMA cellular mode ( there's really no difference except
    ** for the frequency range anyway)
    ** -------------------------------------------------------- */
    rsp_ptr->stat.rf_mode = 1;
  }
  else if ( diag_db.rf_mode == DB_RF_PCN )
  {
    rsp_ptr->stat.rf_mode = 2;
  }
  else if ( diag_db.rf_mode == DB_RF_SLEEP )
  {
    rsp_ptr->stat.rf_mode = 3;
  }
  else if ( diag_db.rf_mode == DB_RF_GPS )
  {
    rsp_ptr->stat.rf_mode = 4;
  }
  else if ( diag_db.rf_mode == DB_RF_HDR )
  {
    rsp_ptr->stat.rf_mode = 5;
  }
  else
  {
    MSG_MED ("Unexpected rf mode in database: %d", diag_db.rf_mode, 0, 0 );
  }

  digital = ( rsp_ptr->stat.rf_mode == DB_RF_ANALOG ) ? FALSE : TRUE; /*lint !e641 */

  /* Now that we know mode,  get the last entry reason */
  mc_get_entry(&entry);
  if ( digital ) {
    rsp_ptr->stat.entry_reason = entry.cdma; /*lint !e641 */
  }
  else
  {
    #ifdef FEATURE_ACP
#error code not present
    #endif /* FEATURE_ACP */
  }

  /* MIN1 and MIN2 for the current NAM */
  for (i = 0; i<NV_MAX_MINS; i++ ) {
    rsp_ptr->stat.min1[i] = diagnv.min1[diagnv.current_nam][i];
    rsp_ptr->stat.min2[i] = diagnv.min2[diagnv.current_nam][i];
  }

  rsp_ptr->stat.orig_min = 0;

  /* Always provide rx state data, even if in analog */

  /* rxc_get_state returns an enum type which uses no negative values */
  switch( rsp_ptr->stat.cdma_rx_state = (word) rxc_get_state() )
  {
    case RXC_SC_STATE:
      rxc_get_sc_crc ( &rx_sc_crc_cnt );
      rsp_ptr->stat.cdma_good_frames =
            mclog_good_frames( &rx_sc_crc_cnt, &sc_crc_cnt );
      break;

    case RXC_PC_STATE:
      rxc_get_pc_crc ( &rx_pc_crc_cnt );
      rsp_ptr->stat.cdma_good_frames =
            mclog_good_frames( &rx_pc_crc_cnt, &pc_crc_cnt );
      break;

    default:
      rsp_ptr->stat.cdma_good_frames = -1;
  }

  #ifdef FEATURE_ACP
#error code not present
  #else /* not FEATURE_ACP */
  rsp_ptr->stat.analog_corrected_frames = 0;
  rsp_ptr->stat.analog_bad_frames = 0;
  rsp_ptr->stat.analog_word_syncs = 0;
  #endif /* FEATURE_ACP */

  db_get ( DB_CHANNEL, &diag_db );
  rsp_ptr->stat.curr_chan = diag_db.channel;

  db_get ( DB_CODE_CHANNEL, &diag_db );
  rsp_ptr->stat.cdma_code_chan = diag_db.code_channel;

  db_get (DB_PILOT_PN_OFF, &diag_db );
  rsp_ptr->stat.pilot_base = diag_db.pilot_pn_off;

  db_get ( DB_SID, &diag_db );
  rsp_ptr->stat.sid = diag_db.sid;

  db_get ( DB_NID, &diag_db );
  rsp_ptr->stat.nid = diag_db.nid;

  db_get ( DB_LOCAID, &diag_db );
  rsp_ptr->stat.locaid = diag_db.locaid;

  db_get ( DB_RSSI, &diag_db );
  rsp_ptr->stat.rssi = diag_db.rssi;

  db_get ( DB_POWER, &diag_db );
  rsp_ptr->stat.power = diag_db.power;

  return (rsp_ptr);

} /* mclog_stat */  /*lint !e715 */

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
  word   len                 /* length of request packet   */
    /*lint -esym(715,req_pkt,len) */

)
{
  word i;                        /* Loop index */
  word mode;                     /* rxc mode   */
  static mar_stat_type mar;      /* buffer for Markov statistics */
  diag_rsp_type   *rsp_ptr;
  const int rsp_len = FSIZ(diag_rsp_type, markov);

  rsp_ptr = (diag_rsp_type *) diagbuf_pkt_alloc(rsp_len);

  /* if rsp_ptr is NULL return from this function */
  if( rsp_ptr == NULL)
  {
	  MSG_HIGH("Out of free DIAG Pkts",0,0,0);
      return NULL;
  }  

  /*----------------------------------------------------------------------
    Retreive bytes from mar and rx modules.  Use mar_get_stat to fill
    in the mar data buffer.
  -----------------------------------------------------------------------*/
  rsp_ptr->markov.cmd_code = DIAG_MARKOV_F;

  mode = rxc_get_mode();

  switch (mode) 
  {
    case CAI_SO_NULL:
      rsp_ptr->markov.mode = DIAG_SO_NONE;
      break;

    case CAI_SO_VOICE_IS96A:
      rsp_ptr->markov.mode = DIAG_SO_VOICE_96A;
      break;

    case CAI_SO_LOOPBACK:
      rsp_ptr->markov.mode = DIAG_SO_LOOPBACK;
      break;

    case CAI_SO_MARKOV:
      rsp_ptr->markov.mode = DIAG_SO_MARKOV;
      break;

    #ifndef FEATURE_NO_13K_VOICE_SUPPORT
    case CAI_SO_VOICE_13K:
      rsp_ptr->markov.mode = DIAG_SO_VOICE13;
      break;
    #endif

    case CAI_SO_MARKOV_13K:
      rsp_ptr->markov.mode = DIAG_SO_MARKOV_13K;
      break;

    case CAI_SO_MARKOV_SO54:
      rsp_ptr->markov.mode = DIAG_SO_MARKOV_SO54;
      break;

    case CAI_SO_LOOPBACK_SO55:
      rsp_ptr->markov.mode = DIAG_SO_LOOPBACK_SO55;
      break;

    case CAI_SO_RS1_MARKOV:
      rsp_ptr->markov.mode = DIAG_SO_RS1_MARKOV;
      break;

    case CAI_SO_RS2_MARKOV:
      rsp_ptr->markov.mode = DIAG_SO_RS2_MARKOV;
      break;

    case CAI_SO_LOOPBACK_13K:
      rsp_ptr->markov.mode = DIAG_SO_LOOPBACK_13K;
      break;

    case CAI_SO_VOICE_EVRC:
      rsp_ptr->markov.mode = DIAG_SO_EVRC;
      break;

    #ifdef FEATURE_SO_VOICE_SMV
    case CAI_SO_VOICE_SMV:
      rsp_ptr->markov.mode = CAI_SO_VOICE_SMV;
      break;
    #endif

    #ifdef FEATURE_VOC_4GV
    case CAI_SO_VOICE_4GV_NB:
      rsp_ptr->markov.mode = CAI_SO_VOICE_4GV_NB;
      break;
    case CAI_SO_VOICE_4GV_WB:
      rsp_ptr->markov.mode = CAI_SO_VOICE_4GV_WB;
      break;
    #endif /* FEATURE_VOC_4GV */

    #ifndef FEATURE_NO_13K_VOICE_SUPPORT
    case CAI_SO_VOICE_13K_IS733:
      rsp_ptr->markov.mode = DIAG_SO_VOICE_13K_IS733;
      break;
    #endif

    #if defined(FEATURE_IS95B_MDR)
    case CAI_SO_MDR_PKT_DATA_FRS1_RRS1:
      rsp_ptr->markov.mode = DIAG_SO_MDR_PKT_DATA_FRS1_RRS1;
      break;

    case CAI_SO_MDR_PKT_DATA_FRS1_RRS2:
      rsp_ptr->markov.mode = DIAG_SO_MDR_PKT_DATA_FRS1_RRS2;
      break;

    case CAI_SO_MDR_PKT_DATA_FRS2_RRS1:
      rsp_ptr->markov.mode = DIAG_SO_MDR_PKT_DATA_FRS2_RRS1;
      break;

    case CAI_SO_MDR_PKT_DATA_FRS2_RRS2:
      rsp_ptr->markov.mode = DIAG_SO_MDR_PKT_DATA_FRS2_RRS2;
      break;

    case CAI_SO_MDR_DPT_FRS1_RRS1:
      rsp_ptr->markov.mode = DIAG_SO_MDR_DPT_FRS1_RRS1;
      break;

    case CAI_SO_MDR_DPT_FRS2_RRS2:
      rsp_ptr->markov.mode = DIAG_SO_MDR_DPT_FRS2_RRS2;
      break;
    #endif /* defined(FEATURE_IS95B_MDR) */

    default:
      switch(mode)
      {
        #ifdef FEATURE_DS
        /* DS_CASE_DATA is a macro containing several cases.
           It is defined in ds.h. */
        DS_CASE_DATA
           /* all the data service options */
          rsp_ptr->markov.mode = DIAG_SO_DATA;
          break;
        #endif   /* ifdef FEATURE_DS */

        default:
          rsp_ptr->markov.mode = DIAG_SO_NONE;
          break;
      }

      break;
  }

  mar_get_stat( &mar );

  rsp_ptr->markov.markov_rate = mar.m_rate; /*lint !e641 */

  /* rxc_get_state returns an enum type which uses no negative values */
  rsp_ptr->markov.rx_state = (word) rxc_get_state();
  rxc_get_pc_crc ( &rx_pc_crc_cnt );
  if( (rsp_ptr->markov.rx_state == RXC_PC_STATE ) && /*lint !e641 */
        mclog_good_frames( &rx_pc_crc_cnt, &pc_crc_cnt ) == 0 ) {
    rsp_ptr->markov.rx_state |= 0x8000;
  }

  /*------------------------------------------------------------------------
    Update frame information.
  -------------------------------------------------------------------------*/
  rsp_ptr->markov.total_frames = mar.total_frames;
  rsp_ptr->markov.bad_frames = mar.bad_frames;
  for( i=0; i< 4; i++ ) {
    rsp_ptr->markov.good_errs[i] = mar.good_errs[i];
  }

  for( i=0; i< 10; i++ ) {
    rsp_ptr->markov.data[0][i] = mar.mar_test[0][i];
    rsp_ptr->markov.data[1][i] = mar.mar_test[1][i];
    rsp_ptr->markov.data[2][i] = mar.mar_test[2][i];
    rsp_ptr->markov.data[3][i] = mar.mar_test[3][i];
  }

  rsp_ptr->markov.bit_errs = mar.bit_errs;

  return (rsp_ptr);

} /* mclog_markov */

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
)
{
  diag_rsp_type   *rsp_ptr;
  const int rsp_len = FSIZ(diag_rsp_type, m_reset);

  /*-----------------------------------------------------------------------
    Reset the statistics and bark out a response packet.
  -----------------------------------------------------------------------*/
  rxc_markov_reset();

  rsp_ptr = (diag_rsp_type *) diagbuf_pkt_alloc(rsp_len);

  /* if rsp_ptr is NULL return from this function */
  if( rsp_ptr == NULL)
  {
	  MSG_HIGH("Out of free DIAG Pkts",0,0,0);
      return NULL;
  }

  rsp_ptr->m_reset.cmd_code = DIAG_MARKOV_RESET_F;

  return (rsp_ptr);

} /* mclog_markov_reset */  /*lint !e715 */

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

/*lint -esym(715, req_pkt) */
PACKED void * mclog_get_is2k_status
(
  PACKED void *req_pkt,      /* pointer to request packet  */
  word len                   /* length of request packet   */
)
{
  mcc_status_type entry; /* CDMA and Analog last entry reason */
  boolean           digital;
  byte fsch_rate;
  mclog_aset_rsch_type rsch;

  DIAG_IS2000_STATUS_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_IS2000_STATUS_F_rsp_type);

  rsp_ptr = (DIAG_IS2000_STATUS_F_rsp_type *) diagbuf_pkt_alloc(rsp_len);

  /* if rsp_ptr is NULL return from this function */
  if( rsp_ptr == NULL)
  {
	  MSG_HIGH("Out of free DIAG Pkts",0,0,0);
      return NULL;
  }

  diagpkt_set_cmd_code(rsp_ptr, DIAG_IS2000_STATUS_F);

  db_get ( DB_RF_MODE, &diag_db );
  rsp_ptr->rf_mode = diag_db.rf_mode; /*lint !e641 */
  rsp_ptr->band_class = mc_get_band_class();

  switch( rsp_ptr->cdma_rx_state = (word) rxc_get_state() )
  {
    case RXC_SC_STATE:
      rxc_get_sc_crc ( &rx_sc_crc_cnt );
      rsp_ptr->cdma_good_frames =
            mclog_good_frames( &rx_sc_crc_cnt, &sc_crc_cnt );
      break;

    case RXC_PC_STATE:
      rxc_get_pc_crc ( &rx_pc_crc_cnt );
      rsp_ptr->cdma_good_frames =
            mclog_good_frames( &rx_pc_crc_cnt, &pc_crc_cnt );
      break;

    default:
      rsp_ptr->cdma_good_frames = -1;
  }

  #ifdef FEATURE_ACP
#error code not present
  #else /* not FEATURE_ACP */
  rsp_ptr->analog_corrected_frames = 0;
  rsp_ptr->analog_bad_frames = 0;
  rsp_ptr->analog_word_syncs = 0;
  #endif /* FEATURE_ACP */

  digital = ( rsp_ptr->rf_mode == DB_RF_ANALOG ) ? FALSE : TRUE; /*lint !e641 */

  /* Now that we know mode,  get the last entry reason */

  mc_get_entry(&entry);

  if ( digital ) {
    rsp_ptr->entry_reason = entry.cdma; /*lint !e641 */
  }
  else
  {
    #ifdef FEATURE_ACP
#error code not present
    #endif /* FEATURE_ACP */
  }

  db_get ( DB_CHANNEL, &diag_db );
  rsp_ptr->curr_chan = diag_db.channel;

  db_get ( DB_SID, &diag_db );
  rsp_ptr->sid = diag_db.sid;

  db_get ( DB_NID, &diag_db );
  rsp_ptr->nid = diag_db.nid;

  db_get ( DB_LOCAID, &diag_db );
  rsp_ptr->locaid = diag_db.locaid;

  db_get ( DB_RSSI, &diag_db );
  rsp_ptr->rssi = diag_db.rssi;

  db_get ( DB_POWER, &diag_db );
  rsp_ptr->power = diag_db.power;

  db_get ( DB_BS_P_REV, &diag_db );
  rsp_ptr->bs_p_rev = diag_db.bs_p_rev;

  // The following status is applicable only if on traffic channel
  if ( (cdma.curr_state & 0xff00) == CDMA_TC )
  {
    const caii_srv_cfg_type  *config = snm_get_current_config();

    rsp_ptr->num_pilots = mccccl_code_channel_list.get_aset_info
                    (rsp_ptr->aset, &fsch_rate, &rsch) ;
    rsp_ptr->fsch_rate = fsch_rate;
    rsp_ptr->rsch=rsch;

    #ifdef FEATURE_IS2000_CHS
    if (mccap_item_is_supported(MCCAP_DF_CHS))
    {
      mccccl_code_channel_list.get_control_hold_info(&(rsp_ptr->control_hold));
    }
    else
    #endif /* FEATURE_IS2000_CHS */
    {
      rsp_ptr->control_hold.enable = FALSE;
    }

    // Now all the RCs
    // Note: FCH/DCCH are always on all pilots
    if ( rsp_ptr->aset[0].fch.enable )
    {
      rsp_ptr->fch.for_rc = config->for_fch_rc;
      rsp_ptr->fch.rev_rc = config->rev_fch_rc;
    }
    #ifdef FEATURE_IS2000_P2
    if ( rsp_ptr->aset[0].dcch.enable )
    {
      rsp_ptr->dcch.for_rc = config->for_dcch_rc;
      rsp_ptr->dcch.rev_rc = config->rev_dcch_rc;
    }
    #endif // FEATURE_IS2000_P2

    #ifdef FEATURE_IS2000_SCH
    if ( mccsch_sch_is_active( MCCSCH_FORWARD, 0 ) )
    {
      rsp_ptr->sch.for_rc = config->for_sch[0].sch_chn_cfg.sch_rc;
    }
    if ( rsp_ptr->rsch.active )
    {
      rsp_ptr->sch.rev_rc = config->rev_sch[0].sch_chn_cfg.sch_rc;
    }
    #endif // FEATURE_IS2000_SCH
  }

  return rsp_ptr;

} /* mclog_get_is2k_status */
/*lint +esym(715, req_pkt) */

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
)
{
  /* Update internal data and report event if operation mode changes */
  if (new_op_mode != mclog_event.curr_op_mode)
  {
    mclog_event.curr_op_mode = new_op_mode;
    event_report_payload(EVENT_OP_MODE_CHANGE, sizeof(event_op_mode_change_type)
      , &new_op_mode);

    /* Initialize paging channel acquisition flag */
    mclog_report_event_pch_acquired(FALSE, 0, 0);

    #ifdef FEATURE_IS2000_REL_A_CC
    /* Initialize broadcast control channel acquisition flag */
    mclog_report_event_bcch_acquired(FALSE, 0, 0);
    #endif /* FEATURE_IS2000_REL_A_CC */

  }
} /* mclog_report_event_op_mode */

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
  cai_rx_channel_id_type channel_id, /* Channel ID */
  byte *msg_ptr /* Pointer to message received in external format */
)
{
  event_message_type event_msg_rxed;
  event_msg_rxed.channel = (uint8) channel_id;

  switch (channel_id)
  {
    case CAI_SYNCH:
    case CAI_PCH:
    case CAI_BCCH:
    case CAI_F_CCCH:
      /* Msg id is first 6 bits of message in external format */
      event_msg_rxed.msg_id = *msg_ptr & 0x3F;
      break;

    case CAI_F_FCH:
    case CAI_F_DCCH:
      /* Msg id is first 8 bits of message in external format */
      event_msg_rxed.msg_id = *msg_ptr;
      break;
   
   case CAI_F_CACH:
   default:
     MSG_ERROR("Msg on unsupported channel?", 0,0,0);
     event_msg_rxed.msg_id = 0xFF;  /* Unsupported channel */
     break;
  }
  event_report_payload(EVENT_MESSAGE_RECEIVED,
    sizeof(event_message_type), (void *) &event_msg_rxed);
  
} /* mclog_report_event_msg_rxed */

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
  cai_tx_channel_id_type channel_id, /* Channel ID */
  byte *msg_ptr /* Message transmitted in external format */
)
{
  event_message_type event_msg_txed;
  event_msg_txed.channel = (uint8) channel_id;

  switch (channel_id)
  {
    case CAI_ACH:
    case CAI_EACH:
      /* Msg id is the lsb 6 bits of the second msg byte in external format */
      event_msg_txed.msg_id = *(++msg_ptr) & 0x3F;
      break;
    
    case CAI_R_FCH:
    case CAI_R_DCCH:
      /* Msg id is the second byte of message in external format */
      event_msg_txed.msg_id = *(++msg_ptr);
      break;

    case CAI_R_CCCH:
    default:
      MSG_ERROR("Msg on unsupported channel?", 0,0,0);
      event_msg_txed.msg_id = 0xFF;  /* Unsupported channel */
      break; 
  }

  event_report_payload(EVENT_MESSAGE_TRANSMITTED,
    sizeof(event_message_type), (void *) &event_msg_txed);

} /* mclog_report_event_msg_txed */

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
  cai_timer_id_type timer_id /* Timer ID */
)
{
  event_report_payload(EVENT_TIMER_EXPIRED, sizeof(event_timer_expired_type),
    &timer_id);
} /* mclog_report_event_timer */

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
  cai_counter_id_type counter_id /* Counter ID */
)
{
  event_report_payload(EVENT_COUNTER_THRESHOLD,
    sizeof(event_counter_threshold_type), &counter_id);
} /* mclog_report_event_counter */

/*===========================================================================

FUNCTION MCLOG_MAP_EVENT_CP_STATE

DESCRIPTION
  Convert CP state to event CP state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

mclog_event_cp_state_type mclog_map_event_cp_state
(
  word curr_state /* Current state */
)
{
  mclog_event_cp_state_type cp_state = CDMA_MAX_S;

  if (curr_state == CDMA_SYS_DET_S) /*lint !e641 */
  {
    cp_state = (mclog_event_cp_state_type) curr_state;
  }
  else
  {
    switch (MAIN_STATE(curr_state))
    {
      case CDMA_ENTER:
        cp_state = CDMA_SYS_DET_S;
        break;

      case CDMA_INIT:
        if (curr_state == CDMA_PIL_ACQ) /*lint !e641 */
        {
          cp_state = CDMA_PIL_ACQ_S;
        }
        else if ((curr_state == CDMA_SYNC_ACQ) || (curr_state == CDMA_SYNC_MSG)) /*lint !e641 */
        {
          cp_state = CDMA_SYNC_ACQ_S;
        }
        else
        {
          cp_state = CDMA_TIME_CHG_S;
        }
        break;

      case CDMA_IDLE:
        if (curr_state == CDMA_OVHD) /*lint !e641 */
        {
          cp_state = CDMA_UPDATE_OVH_S;
        }
        else
        {
          cp_state = CDMA_IDLE_S;
        }
        break;

      case CDMA_SYS_ACC:
        cp_state = (mclog_event_cp_state_type)
          (CDMA_UPDATE_OVH_S + curr_state - CDMA_SYS_ACC); /*lint !e641 */
        break;

      case CDMA_TC:
        #ifdef FEATURE_IS2000_REL_A
        cp_state = (mclog_event_cp_state_type) (CDMA_TCI_S + curr_state - /*lint !e641 */
                   CDMA_TC);
        #else
        if ( curr_state == CDMA_RELEASE)
        {
          cp_state = CDMA_RELEASE_S;
        }
        else
        {
          /* For WFO, WFA & CONV this function is not even called anymore */
          cp_state = (mclog_event_cp_state_type) (CDMA_TCI_S + curr_state -
                   CDMA_TC);
        }
        #endif /*FEATURE_IS2000_REL_A*/
        break;

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

      default:
        ERR("Unknown CP state", 0, 0, 0);
        break;
    } /* end switch */
  } /* end if */

  return cp_state;

} /* end mclog_map_event_cp_state */

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
)
{
  event_call_processing_state_change_type event_cp_state;
  boolean slotted_mode;

  #ifndef FEATURE_IS2000_REL_A_CC
  boolean qpch_in_use;
  #endif /* !FEATURE_IS2000_REL_A_CC */

  /* CP state changes */
  if (new_state != mclog_event.curr_state)
  {
    /* Event report: Call processing state change */
    event_cp_state.old_state =
      (uint8) mclog_map_event_cp_state(mclog_event.curr_state);
    event_cp_state.new_state = (uint8) mclog_map_event_cp_state(new_state);

    if (event_cp_state.old_state != event_cp_state.new_state)
    {
      #ifdef FEATURE_IS2000_REL_A
      event_report_payload(EVENT_HIGH_LEVEL_CALL_PROCESSING_STATE_CHANGE,
        sizeof(event_call_processing_state_change_type),
        (void *) &event_cp_state);
      #else
      if (new_state != CDMA_TRAFFIC)
      {
        if (mclog_event.curr_state == CDMA_TRAFFIC)
        {
          /* This is the high level CP state but we need the current CC
          state to know which state in TC MS was in */
          event_cp_state.old_state = mclog_event.tc_substate;
        }
        /* These are reported by Call control report event function,
           so skip it here */
        event_report_payload(EVENT_CALL_PROCESSING_STATE_CHANGE,
          sizeof(event_call_processing_state_change_type),
          (void *) &event_cp_state);
      }
      #endif

    }

    /* Enter slotted mode of operation */
    if (new_state == CDMA_SLOTTED) /*lint !e641 */
    {
      /* Events report: Slotted mode operation */
      slotted_mode = TRUE;
      event_report_payload(EVENT_SLOTTED_MODE_OPERATION,
        sizeof(event_slotted_mode_operation_type), &slotted_mode);

      #ifndef FEATURE_IS2000_REL_A_CC
      /* Events report: QPCH in use */
      if (mccap_item_is_supported(MCCAP_DF_QPCH))
      {
        qpch_in_use = FALSE;
      }
      else if (!cur_bs_ptr->csp.sp.ext_sys_parameter ||
               !P_REV_IS_6_OR_GREATER)
      {
        qpch_in_use = FALSE;
      }
      else
      {
        qpch_in_use = cur_bs_ptr->csp.esp.qpch_supported;
      }

      event_report_payload(EVENT_QPCH_IN_USE,
        sizeof(event_qpch_in_use_type), &qpch_in_use);
      #endif /* !FEATURE_IS2000_REL_A_CC */

    }
    /* Event report: Slotted mode operation exit */
    else if (mclog_event.curr_state == CDMA_SLOTTED) /*lint !e641 */
    {
      slotted_mode = FALSE;
      event_report_payload(EVENT_SLOTTED_MODE_OPERATION,
        sizeof(event_slotted_mode_operation_type), &slotted_mode);
    }
  }

  /* Records current CP state */
  mclog_event.curr_state = new_state;

} /* mclog_report_event_cp_state */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION MCLOG_MAP_EVENT_CC_STATE

DESCRIPTION
  Convert CP state to event call control state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

mclog_event_cc_state_type mclog_map_event_cc_state
(
  cdma_cc_state_type curr_state /* Current state */
)
{
  mclog_event_cc_state_type cc_state;

   /* The payload for this event is 4 bits, make sure we are not exceeding the max */
  if (CDMA_CC_MAX_S > MAX_NUM_CALL_CONTROL_SUBSTATES)  /*lint !e506 !e774 !e641 */ 
  {
    ERR( "Too many Call Control Substates", 0, 0, 0 );
  }

  /*lint -e641*/
  cc_state = (mclog_event_cc_state_type) (CDMA_CC_WFO_S + (int) curr_state - CDMA_CC);
  /*lint +e641*/

  return cc_state;

} /* end mclog_map_event_cc_state */
#endif //FEATURE_IS2000_REL_A

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

/*lint -esym(715,con_ref) */
void mclog_report_event_cc_create
(
 uint8 con_ref,
 cdma_cc_state_type new_state
)
{
  #ifdef FEATURE_IS2000_REL_A
  mclog_event_cc_instantiated_type cc_event = {0};
  #else
  event_call_processing_state_change_type event_cp_state;
  #endif

  #ifdef FEATURE_IS2000_REL_A
  cc_event.con_ref = (int)con_ref;
  /*lint -e641*/
  cc_event.init_substate = (uint8) (CDMA_CC_WFO_S + (int) new_state - CDMA_CC) ;
  /*lint +e641*/

  event_report_payload(EVENT_CALL_CONTROL_INSTANTIATED,
    sizeof(mclog_event_cc_instantiated_type), (void *) &cc_event);
  #else
  /* If we are here, it means the old state is TC Init */
  event_cp_state.old_state = CDMA_TCI_S;
  event_cp_state.new_state = CDMA_WFO_S+(new_state-CDMA_CC_WFO); //lint !e734

  event_report_payload(EVENT_CALL_PROCESSING_STATE_CHANGE,
    sizeof(event_call_processing_state_change_type), (void *) &event_cp_state);
  #endif

} /* mclog_report_event_cc_create */
/*lint +esym(715,con_ref) */

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

/*lint -esym(715, con_ref) */
void mclog_report_event_cc_state
(
 cdma_cc_state_type old_state,
 cdma_cc_state_type new_state,
 uint8 con_ref
)
{
  #ifdef FEATURE_IS2000_REL_A
  event_call_control_state_change_type event_cc_state={0,0,0};
  #else
  event_call_processing_state_change_type cp_state;
  #endif

  if (new_state != old_state)
  {
    #ifdef FEATURE_IS2000_REL_A
    event_cc_state.con_ref = (int)con_ref;
    /* Event report: Call Control State Change */
    event_cc_state.old_substate = (uint8) mclog_map_event_cc_state(old_state);
    event_cc_state.new_substate = (uint8) mclog_map_event_cc_state(new_state);

    event_report_payload(EVENT_CALL_CONTROL_STATE_CHANGE,
             sizeof(event_call_control_state_change_type),
             (void *) &event_cc_state);
    #else
    /* For non Release A targets report corresponding Call processing
    state change event */
    if (new_state == CDMA_CC_RELEASE)
    {
      /* No need to send this event here, The high level call processing
      evemnt reporting will take care of this one */
      return;
    }
    cp_state.new_state = (mclog_event_cp_state_type) (CDMA_WFO_S + new_state -
                                            CDMA_CC);
    cp_state.old_state = (mclog_event_cp_state_type) (CDMA_WFO_S + old_state -
                                            CDMA_CC);

    event_report_payload(EVENT_CALL_PROCESSING_STATE_CHANGE,
      sizeof(event_call_processing_state_change_type), (void *) &cp_state);

    #endif
  }

  #ifndef FEATURE_IS2000_REL_A
  mclog_event.tc_substate = (mclog_event_cp_state_type) (CDMA_WFO_S + new_state -
                                            CDMA_CC);
  #endif

} /* mclog_report_event_cc_state */
/*lint +esym(715, con_ref) */

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
)
{
  event_pch_acquired_type event_pch_acquired;

  if (pagech_acq)
  {
    /* -------------------------------------------------------------------
    ** MS acquires the paging channel for the first time after power up or
    ** operation mode change.
    ** ------------------------------------------------------------------- */
    if (!mclog_event.pagech_acq)
    {
      /* Events report: Paging channel acquired */
      event_pch_acquired.pagech = pagech;
      event_pch_acquired.pn_offset = pn_offset;
      event_report_payload(EVENT_PCH_ACQUIRED, sizeof(event_pch_acquired_type),
        (void *) &event_pch_acquired);

      /* Record paging channel acquisition */
      mclog_event.pagech_acq = pagech_acq;
    }
  }
  else
  {
    mclog_event.pagech_acq = pagech_acq;
  }

} /* mclog_report_event_pch_acquired */

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_BCCH_ACQUIRED

DESCRIPTION
  Report event broadcast control channel acquired for the first time after
  power up or operation mode change.

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
)
{
  event_bcch_acquired_type event_bcch_acquired;

  if (bcch_acq)
  {
    /* -------------------------------------------------------------------
    ** MS acquires the broadcast control channel for the first time after
    ** power up or operation mode change.
    ** ------------------------------------------------------------------- */
    if (!mclog_event.bcch_acq)
    {
      /* Events report: Broadcast Control Channel acquired */
      event_bcch_acquired.bcch = bcch_walsh_code;
      event_bcch_acquired.pn_offset = pn_offset;
      event_report_payload(EVENT_BCCH_ACQUIRED, sizeof(event_bcch_acquired_type),
        (void *) &event_bcch_acquired);

      /* Record broadcast control channel acquisition */
      mclog_event.bcch_acq = TRUE;
    }
  }
  else
  {
    mclog_event.bcch_acq = FALSE;
  }

} /* mclog_report_event_bcch_acquired */
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
)
{
  event_id_enum_type event_id = EVENT_MAX_ID;

  mclog_event_idl_acc_handoff_type idl_acc_ho_event;
    /* Structure for event reporting */

  /* Prepare idle/access event packet for reporting - EcIo from srch is in
  ** 1/2 db units so divide by 2 and set the fractional portion (.0 or .5) 
  ** based on the lsb. 
  */
  idl_acc_ho_event.new_pilot = new_pilot;
  idl_acc_ho_event.ecio_old_pilot = (ecio_old_pilot / 2);
  idl_acc_ho_event.ecio_old_pilot_frac = ((ecio_old_pilot & 0x01) ? 5 : 0);
  idl_acc_ho_event.ecio_new_pilot = (ecio_new_pilot / 2);
  idl_acc_ho_event.ecio_new_pilot_frac = ((ecio_new_pilot & 0x01) ? 5 : 0);
  idl_acc_ho_event.new_freq = new_freq;
  idl_acc_ho_event.new_band_class = new_band_class;

  if (ho_state == CDMA_IDLE_HO)
  {
    /* Record that we need to generate Idle Handoff Event */
    event_id = EVENT_IDLE_HANDOFF;
  }
  else if (ho_state == CDMA_AEHO)
  {
    /* Record that we need to generate Access Entry Handoff Event */
    event_id = EVENT_ACCESS_ENTRY_HANDOFF;
  }
  else if (ho_state == CDMA_AHO)
  {
    /* Record that we need to generate Access Handoff Event */
    event_id = EVENT_ACCESS_HANDOFF;
  }
  else if (ho_state == CDMA_APHO)
  {
    /* Record that we need to generate Access Probe Handoff Event */
    event_id = EVENT_ACCESS_PROBE_HANDOFF;
  }

  /* Check if we previously determined that an event is needed */
  if (event_id != EVENT_MAX_ID)
  {
    if (event_id == EVENT_IDLE_HANDOFF)
    {
      switch (ho_type)
      {
        case CDMA_RAHO:
          idl_acc_ho_event.idle_ho_type = MCLOG_IDLE_HO_REACQ; /*lint !e641 */
          break;

        case CDMA_FAST_RAHO:
          if ( cheap_idle_ho )
          {
            idl_acc_ho_event.idle_ho_type = MCLOG_IDLE_HO_FAST_REACQ_CHEAP; /*lint !e641 */
          }
          else
          {
            idl_acc_ho_event.idle_ho_type = MCLOG_IDLE_HO_FAST_REACQ_EXPENSIVE; /*lint !e641 */
          }
          break;

        case CDMA_IDLE_HO:
          idl_acc_ho_event.idle_ho_type = MCLOG_IDLE_HO; /*lint !e641 */
          break;

        default:
          idl_acc_ho_event.idle_ho_type = 0xFF;
          MSG_ERROR("Invalid Idle HO type for event", 0,0,0);
          break;
      }

      event_report_payload(event_id, sizeof(mclog_event_idl_acc_handoff_type), 
        (void*) &idl_acc_ho_event);
    }
    else
    {
      /* Subtract off the last byte of the structure - AHO/AEHO/APHO events
         do not need to include field for the type of idle HO. */
      event_report_payload(event_id, 
        sizeof(mclog_event_idl_acc_handoff_type)-sizeof(byte),
        (void*) &idl_acc_ho_event);
    }
  }

} /* mclog_report_event_csch_ho */

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
  cai_chind_type     ota_ch_ind,      /* Channel indicator */
  cai_ext_chind_type  ota_ext_ch_ind  /* Extended Channel indicator */
)
{
  struct 
  {
    uint8 ch_ind;
    uint8 ext_ch_ind;

  } chind_info;

  /* OTA values for CH_IND is 3 bits max and for EXT_CH_IND is 5 bits max
  ** Casts of enums to uint8 make lint happy */
  chind_info.ch_ind = (uint8) ota_ch_ind; 
  chind_info.ext_ch_ind = (uint8) ota_ext_ch_ind; 

  event_report_payload(EVENT_CH_IND_CHANGE, sizeof(chind_info), &chind_info);

} /* mclog_report_event_ch_ind */

/*===========================================================================

REDFUNCTION MCLOG_REPORT_EVENT_F_FDCCH_ACQUIRED

DESCRIPTION
  Report event F-FCH acquired or F-DCCH acquired or F-FCH plus F-DCCH
  acquired.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mclog_report_event_f_fdcch_acquired
(
  void
)
{
  mclog_fch_dcch_acq_type acquire =
                           {{0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
                            {0,0,0,0,0,0}};
  mccdma_pilot_info_type  active_pilots[CAI_N6M];
  int  i, num_pilots;
  mcc_fwd_chan_mask_type fwd_chan_mask;

  /* What forward channels are currently active? */
  fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();

  /* Get the active pilots */
  num_pilots = mccccl_code_channel_list.get_pilots(active_pilots);

  /* Copy the pilot_pn and pwr_comb_ind */
  for (i = 0; i < num_pilots; i++)
  {
    acquire.pn[i] = active_pilots[i].pilot_pn;
    switch (i)
    {
      case 0:
        break;

      case 1:
        acquire.sr.sr2 = active_pilots[i].pwr_comb_ind;
        break;

      case 2:
        acquire.sr.sr3 = active_pilots[i].pwr_comb_ind;
        break;

      case 3:
        acquire.sr.sr4 = active_pilots[i].pwr_comb_ind;
        break;

      case 4:
        acquire.sr.sr5 = active_pilots[i].pwr_comb_ind;
        break;

      case 5:
        acquire.sr.sr6 = active_pilots[i].pwr_comb_ind;
        break;

      default:
        break;
    } /* switch */
  } /* for */

  if ((fwd_chan_mask & MCC_F_FCH) && (fwd_chan_mask & MCC_F_DCCH))  // FCH and DCCH
  {
      event_report(EVENT_FFCH_PLUS_DCCH_ACQUIRED);
  }
  else if (fwd_chan_mask & MCC_F_FCH)                                // FCH only
  {
      event_report_payload(EVENT_FFCH_ACQUIRED,
                           sizeof(mclog_fch_dcch_acq_type),
                           (void *)&acquire);
  } else if (fwd_chan_mask & MCC_F_DCCH)                             // DCCH only 
  { 
    event_report_payload(EVENT_FDCCH_ACQUIRED,
                         sizeof(mclog_fch_dcch_acq_type),
                         (void *)&acquire);
  }
} /* mclog_report_event_f_fdcch_acquired */

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
)
{
  event_common_channel_monitored_type event_common_channel_monitored;

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  event_common_channel_monitored.non_pri_bcch_active =
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  #ifdef FEATURE_IS2000_REL_A_CC
  event_common_channel_monitored.pri_bcch_active     =
  event_common_channel_monitored.fccch_active        =
  #endif /* FEATURE_IS2000_REL_A_CC */

  event_common_channel_monitored.qpch_active         =
  event_common_channel_monitored.pch_active          = FALSE;

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  if (active_channel_mask & MCC_NON_PRI_BCCH)
  {
    event_common_channel_monitored.non_pri_bcch_active = TRUE;
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  if (active_channel_mask == MCC_PCH)
  {
    event_common_channel_monitored.pch_active = TRUE;
    if ((mccap_item_is_supported(MCCAP_DF_QPCH) ||
         mccap_is_enhanced_operation_supported()) &&
        !SP_MSG_NOT_RECVD && cur_bs_ptr->csp.sp.ext_sys_parameter &&
        !ESP_MSG_NOT_RECVD && cur_bs_ptr->csp.esp.p_rev >= P_REV_IS2000 &&
        cur_bs_ptr->csp.esp.qpch_supported)
    {
      event_common_channel_monitored.qpch_active = TRUE;
    }
  }
  
  #ifdef FEATURE_IS2000_REL_A_CC
  else
  {
    if (active_channel_mask & MCC_PRI_BCCH)
    {
      event_common_channel_monitored.pri_bcch_active = TRUE;
    }
    if (active_channel_mask & MCC_FCCCH)
    {
      event_common_channel_monitored.fccch_active = TRUE;
      if (!MCRRP_MSG_NOT_RECVD && cur_bs_ptr->csp.esp.qpch_supported)
      {
        event_common_channel_monitored.qpch_active = FALSE;
      }
    }
  }
  #endif /* FEATURE_IS2000_REL_A_CC */

  event_report_payload(EVENT_COMMON_CHANNEL_MONITORED,
    sizeof(event_common_channel_monitored_type),
    (void *) &event_common_channel_monitored);

} /* end mclog_report_event_common_channel_monitored */

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
)
{
  mclog_event_seg_upm_addr_mismatch_type event_seg_upm_addr_mismatch;

  event_seg_upm_addr_mismatch.addr_mismatch_type = addr_mismatch_type;
  event_seg_upm_addr_mismatch.segment_seq = segment_seq;

  event_report_payload(EVENT_SEG_UPM_ADDR_MISMATCH,
    sizeof(mclog_event_seg_upm_addr_mismatch_type),
    (void *) &event_seg_upm_addr_mismatch);

} /* mclog_report_event_seg_upm_addr_mismatch */
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
)
{
  mclog_event_wakeup_slot_v2_type wakeup;

  wakeup.slot_mask = mask;
  wakeup.slept     = slept;

  event_report_payload(EVENT_SMS_SLOT_WAKEUP_V2,
                       sizeof(mclog_event_wakeup_slot_v2_type),
                       (void *) &wakeup);
  
} /* mclog_report_wakeup_slot */

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
)
{
  mclog_event_cp_matched_msg_type match_event;

  match_event.msg_type = msg_type;
  match_event.match_type = (byte) match_type;

  event_report_payload(EVENT_CP_MATCHED_MSG,
                       sizeof(mclog_event_cp_matched_msg_type),
                       (void *) &match_event);

} /* mclog_report_matched_msg */

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
)
{
  mclog_event_release_type rel_reason;

  rel_reason = (mclog_event_release_type)reason;

  event_report_payload(EVENT_RELEASE,
                       sizeof(mclog_event_release_type),
                        (void *) &rel_reason);

} /* mclog_report_release */

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
)
{

  mclog_event_registration_suppressed_type reg_type = registration_suppressed_type;

  event_report_payload(EVENT_REGISTRATION_SUPPRESSED,
                       sizeof(mclog_event_registration_suppressed_type),
                       (void *) &reg_type);

} /* mclog_report_registration_suppressed */

/*===========================================================================

FUNCTION MCLOG_REPORT_EVENT_LONG_CODE_MASK

DESCRIPTION
  Report a change in the long code mask.

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
)
{
  mclog_event_long_code_mask_type event_lcm;

  if (always_report
      || (cdma.long_code.public_lcm_type != mclog_event.public_long_code_mask_type)
      #ifdef FEATURE_AUTH
      || (cdma.long_code.private_lcm_active != mclog_event.using_private_lcm)
      #endif /* FEATURE_AUTH */
     )
  {
    #ifdef FEATURE_AUTH
    event_lcm.using_private_lcm = cdma.long_code.private_lcm_active;
    #else
    /* if Auth is not defined, the mobile cannot be using the private long code mask. */
    event_lcm.using_private_lcm = FALSE;
    #endif

    event_lcm.public_long_code_mask_type = cdma.long_code.public_lcm_type; /*lint !e641 */

    event_report_payload(EVENT_LONG_CODE_MASK_CHANGED,
                         sizeof(mclog_event_long_code_mask_type),
                         (void *) &event_lcm);

    #ifdef FEATURE_AUTH
    mclog_event.using_private_lcm = cdma.long_code.private_lcm_active;
    #endif /* FEATURE_AUTH */

    mclog_event.public_long_code_mask_type = cdma.long_code.public_lcm_type;
  }

} /* mclog_report_event_long_code_mask */

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
)
{
  word real_slot, real_slot_pos, real_sci_mask;

  if (enable_mer_stat)
  {
    /* Mark MER statistical operation enabled for the slot */
    mclog_mer.mer_stat_enabled = TRUE;

    /* Record system time of the slot */
    qw_equ(mclog_mer.slot_sys_time, slot_sys_time);

    real_sci_mask = mcc_calculate_sci_mask(mccidl_get_slot_cycle_index_s());

    real_slot_pos = ((word)(qw_lo(slot_sys_time)) >> 2) & real_sci_mask;
    real_slot = (mcc_get_idle_pgslot() & real_sci_mask);

    /* Real SCI page slot */
    if (real_slot_pos == real_slot) 
    {
      mclog_mer.real_sci_page_slot_ctr++;
      MSG_MED("Incremented REAL_SCI_PAGE_SLOT to %d, slot: 0x%x", 
        mclog_mer.real_sci_page_slot_ctr, qw_lo(mclog_mer.slot_sys_time), 0);
    }
    /* Telescoping SCI page slot */
    else
    {
      mclog_mer.tele_sci_page_slot_ctr++;
      MSG_MED("Incremented TELE_SCI_PAGE_SLOT to %d, slot: 0x%x", 
        mclog_mer.tele_sci_page_slot_ctr, qw_lo(mclog_mer.slot_sys_time), 0);
    }
  }
  else
  {
    /* Mark MER statistical operation disabled for the slot */
    mclog_mer.mer_stat_enabled = FALSE;
    qw_set(mclog_mer.slot_sys_time, 0, 0);

    MSG_MED("MER stat off, slot: 0x%x", qw_lo(mclog_mer.slot_sys_time), 0, 0); 
  }

} /* mclog_mer_config */

/*===========================================================================

FUNCTION MCLOG_MER_INIT
  This function initializes the information fields for MER counter support.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mclog_mer_init
(
  void
)
{
  mclog_mer.page_rxed_real_sci_asgn_slot_ctr = 0;
  mclog_mer.page_rxed_real_sci_next_slot_ctr = 0;
  mclog_mer.page_rxed_tele_sci_asgn_slot_ctr = 0;
  mclog_mer.page_rxed_tele_sci_next_slot_ctr = 0;
  mclog_mer.real_sci_page_slot_ctr = 0;
  mclog_mer.tele_sci_page_slot_ctr = 0;
  mclog_mer_config(FALSE, 0);

} /* mclog_mer_init */    

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
  boolean in_tele, /* Indicates if it is telescoping sci or not */
  boolean in_slot, /* Indicates if it is in-slot or not */
  qword page_rxed_sys_time /* System time that Page rxed */
)
{
  qword time_diff;
  boolean page_rxed_in_asgn_slot = TRUE;

  /* Perform MER statistical operation if it is enabled for the slot */
  if (mclog_mer.mer_stat_enabled)
  {
    MSG_MED("GPM rxed: 0x%x, slot: 0x%x", 
      qw_lo(page_rxed_sys_time), qw_lo(mclog_mer.slot_sys_time), 0);

    /* Page Msg received before slot boundary. */
    if (qw_cmp(page_rxed_sys_time, mclog_mer.slot_sys_time) == -1)
    {
      MSG_MED("Ignore Page Msg rxed before slot boundary", 0, 0, 0); 
    }

    /* Page Msg received after slot boundary. */
    else
    {
      /* See if Page Msg is received in first 81ms of the slot. */ 
      qw_sub(time_diff, page_rxed_sys_time, mclog_mer.slot_sys_time);
      if (QW_CVT_Q2N(time_diff) > MCLOG_ONE_SLOT) /*lint !e740*/
      {
        /* Page Msg received after first 81ms of the slot */
        page_rxed_in_asgn_slot = FALSE;
        MSG_MED("TIME diff lo: 0x%x hi: 0x%x", qw_lo(time_diff), 
          qw_hi(time_diff), 0);
      }

      /* Real SCI */
      if (!in_tele)
      {
        /* In-slot and Page Msg received within first 81ms of the slot */
        if (in_slot && page_rxed_in_asgn_slot)
        {
          mclog_mer.page_rxed_real_sci_asgn_slot_ctr++;
          MSG_MED("Incremented RXED_REAL_SCI_ASGN_SLOT to %d", 
            mclog_mer.page_rxed_real_sci_asgn_slot_ctr, 0, 0);
        }
        /* Next rather than in-slot or Page Msg received after first 
           81ms of the slot */
        else
        {
          mclog_mer.page_rxed_real_sci_next_slot_ctr++;
          MSG_MED("Incremented RXED_REAL_SCI_NEXT_SLOT to %d", 
            mclog_mer.page_rxed_real_sci_next_slot_ctr, 0, 0);
        }
      }
      /* Telescoping SCI */
      else
      {
        /* In-slot and Page Msg recived within first 81ms of the slot */
        if (in_slot && page_rxed_in_asgn_slot)
        {
          mclog_mer.page_rxed_tele_sci_asgn_slot_ctr++;
          MSG_MED("Incremented RXED_TELE_SCI_ASGN_SLOT to %d", 
            mclog_mer.page_rxed_tele_sci_asgn_slot_ctr, 0, 0);
        }
        /* Next rather than in-slot or Page Msg received after first
           81ms of the slot */
        else
        {
          mclog_mer.page_rxed_tele_sci_next_slot_ctr++;
          MSG_MED("Incremented RXED_TELE_SCI_NEXT_SLOT to %d", 
            mclog_mer.page_rxed_tele_sci_next_slot_ctr, 0, 0);
        }
      }

      /* Mark MER statistical operation for the slot is done. */
      mclog_mer_config(FALSE, 0);
    }
  }

} /* mclog_mer_update_counter */

/*===========================================================================

FUNCTION MC_DIAG_INIT
  This function registers the subsystem dispatch command handler table with
  Diag and performs other mclog initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mc_diag_init 
(
  void
)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_CALLP1X, mc_diag_tbl); //lint !e717 !e641

  /* Initialize MER counter support */
  mclog_mer_init();

} /* mc_log_init */    

/*===========================================================================

FUNCTION MC_DIAG_DF_ITEMS_QUERY_HANDLER
  This function handles the DF items query command from Diag.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

PACKED void * mc_diag_df_items_query_handler
(
  PACKED void *req_ptr, 
  uint16      pkt_len
)
{
  mc_diag_df_items_query_req_type    *dfq_req_ptr;                /* DF query request String */
  mc_diag_df_items_query_rsp_type    *dfq_rsp_ptr;                /* DF query response String */
  uint16                        rsp_len;

  rsp_len = sizeof (mc_diag_df_items_query_rsp_type);

  if (pkt_len != sizeof (mc_diag_df_items_query_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len); 
  }

  dfq_req_ptr = (mc_diag_df_items_query_req_type *) req_ptr;

  if (dfq_req_ptr == NULL)
  {
    return NULL;
  }

  dfq_rsp_ptr = (mc_diag_df_items_query_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_DF_ITEMS_QUERY,
                                        rsp_len);
  
  if (dfq_rsp_ptr == NULL) {
    return NULL;
  }
  
  #ifdef FEATURE_IS2000_DYNAMIC_FEATURE
  /* Discover the DF items supported by the mobile */
  dfq_rsp_ptr->status = mccap_df_query(dfq_rsp_ptr->str);
  #else
  dfq_rsp_ptr->status = FALSE;
  #endif
  
  return dfq_rsp_ptr;

} /* mc_diag_df_items_query_handler */

/*===========================================================================

FUNCTION MC_DIAG_DF_ITEM_VERIFY_HANDLER
  This function handles the DF item verify command from DIAG

DEPENDENCIES
  None

RETURN VALUE
  List of DF items which change their value because of the verify command

SIDE EFFECTS
  None

===========================================================================*/

PACKED void * mc_diag_df_item_verify_handler
(
  PACKED void *req_ptr, 
  uint16      pkt_len
)
{
  mc_diag_df_item_verify_rsp_type     *dfv_rsp_ptr;     /* DF verify response string */
  mc_diag_df_item_verify_req_type     *dfv_req_ptr;     /* DF verify request string */
  uint16                              rsp_len;

  rsp_len = sizeof (mc_diag_df_item_verify_rsp_type);

  if (pkt_len != sizeof (mc_diag_df_item_verify_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len); 
  }

  dfv_req_ptr = (mc_diag_df_item_verify_req_type *) req_ptr;

  if (dfv_req_ptr == NULL)
  {
    return NULL;
  }

  dfv_rsp_ptr = (mc_diag_df_item_verify_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_DF_ITEM_VERIFY,
                                        rsp_len);

  if (dfv_rsp_ptr == NULL)
  {
    return NULL;
  }

  /* Verify the DF items */
  #ifdef FEATURE_IS2000_DYNAMIC_FEATURE
  dfv_rsp_ptr->status = mccap_df_verify(dfv_req_ptr->str, dfv_rsp_ptr->str);
  #else
  dfv_rsp_ptr->status = FALSE;
  #endif
  
  return dfv_rsp_ptr;

} /* mc_diag_df_item_verify_handler */

/*===========================================================================

FUNCTION MC_DIAG_DF_ITEMS_WRITE_HANDLER
  This function handles the DF items write command from DIAG

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

PACKED void * mc_diag_df_items_write_handler
(
  PACKED void *req_ptr, 
  uint16      pkt_len
)
{
  mc_diag_df_items_write_rsp_type     *dfw_rsp_ptr;           /* DF write response string */
  mc_diag_df_items_write_req_type     *dfw_req_ptr;           /* DF write request string */
  uint16                              rsp_len;

  rsp_len = sizeof (mc_diag_df_items_write_rsp_type);

  /* In case of a write command from QPST, the packet length is variable. But
   * it should exceed the size of the write request packet */
  if (pkt_len > sizeof (mc_diag_df_items_write_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len); 
  }

  dfw_req_ptr = (mc_diag_df_items_write_req_type *) req_ptr;

  if (dfw_req_ptr == NULL)
  {
    return NULL;
  }

  dfw_rsp_ptr = (mc_diag_df_items_write_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_DF_ITEMS_WRITE,
                                        rsp_len);

  if (dfw_rsp_ptr == NULL)
  {
    return NULL;
  }

  #ifdef FEATURE_IS2000_DYNAMIC_FEATURE
  /* Write the DF items to NV */
  dfw_rsp_ptr->status = mccap_df_write(dfw_req_ptr->str);
  #else
  dfw_rsp_ptr->status = FALSE;
  #endif
  
  return dfw_rsp_ptr;

} /* mc_diag_df_items_write_handler */

/*===========================================================================

FUNCTION mclog_diag_state_query_handler
  This function handles the MC state query command from Diag.

DEPENDENCIES
  None

RETURN VALUE
  Returns struct mclog_diag_state_query_rsp_type

SIDE EFFECTS
  None

===========================================================================*/

PACKED void * mclog_diag_state_query_handler
(
  PACKED void *req_ptr, 
  uint16      pkt_len
)
{
  mclog_diag_state_query_rsp_type     *rsp_ptr;
  uint16                               rsp_len;

  if (pkt_len != sizeof (mclog_diag_state_query_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len); 
  }

  rsp_len = sizeof (mclog_diag_state_query_rsp_type);
  rsp_ptr = (mclog_diag_state_query_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_STATE_QUERY,
                                        rsp_len);
  if (rsp_ptr == NULL)
  {
    MSG_HIGH("Out of free DIAG Pkts",0,0,0);
    return NULL;
  }

  rsp_ptr->mc_state = (uint8) mc_state();
  rsp_ptr->event_count = (uint16) diag_get_event_cnt();
  rsp_ptr->num_cc=0;

  if (rsp_ptr->mc_state == DIAG_D_CONV_S)
  {
    /* We need to fill in CC info */
    mccccim_diag_get_state(rsp_ptr);
  }

  return rsp_ptr;

} /* mclog_diag_state_query_handler */

/*===========================================================================

FUNCTION mclog_diag_set_curr_pref_sci
  This function is used to set the current preferred slot cycle index

DEPENDENCIES
  None

RETURN VALUE
  Returns struct

SIDE EFFECTS
  None

===========================================================================*/

PACKED void * mclog_diag_set_curr_pref_sci
(
  PACKED void *req_ptr, 
  uint16      pkt_len
)
{
  mc_diag_cur_pref_sci_rsp_type       *rsp_ptr;
  uint16                               rsp_len;

  if (pkt_len != sizeof (mc_diag_cur_pref_sci_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len); 
  }

  rsp_len = sizeof (mc_diag_cur_pref_sci_rsp_type);
  rsp_ptr = (mc_diag_cur_pref_sci_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_SET_CUR_PREF_SCI,
                                        rsp_len);

  if (rsp_ptr == NULL)
  {
    MSG_HIGH("Out of free DIAG Pkts",0,0,0);
    return NULL;
  }

  rsp_ptr->sci.curr_pref_sci = 
            mcc_set_current_preferred_sci(
                   ((mc_diag_cur_pref_sci_req_type *) req_ptr)->sci.curr_pref_sci
                  );
  return rsp_ptr;

} /* mclog_diag_set_curr_pref_sci */

/*===========================================================================

FUNCTION MC_DIAG_GET_NW_PARAMS
  This function handles the MC_DIAG_SUBSYS_GET_NW_PARAMS command from Diag.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the response packet

SIDE EFFECTS
  None

===========================================================================*/

PACKED void * mc_diag_get_nw_params
(
  PACKED void *req_ptr, 
  uint16      pkt_len
)
{
  mc_diag_get_nw_params_rsp_type       *rsp_ptr;
  uint16                               rsp_len;

  const caii_srv_cfg_type  *config = snm_get_current_config();

  if (pkt_len != sizeof (mc_diag_get_nw_params_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len); 
  }

  rsp_len = sizeof (mc_diag_get_nw_params_rsp_type);
  rsp_ptr = (mc_diag_get_nw_params_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_CALLP1X, /*lint !e641 */
                                        MC_DIAG_SUBSYS_GET_NW_PARAMS,
                                        rsp_len);

  if (rsp_ptr == NULL)
  {
    MSG_HIGH("Out of free DIAG Pkts",0,0,0);
    return NULL;
  }

  rsp_ptr->params.inter_freq_idle_ho_count = mclog_inter_freq_idle_ho_count;
  rsp_ptr->params.hard_ho_count = mclog_hard_ho_count;

  if(!config->for_sch_cc_incl || (config->num_for_sch == 0))
  {
    MSG_MED("Channel config for F-SCH0 is not available", 0, 0, 0);
  }
  rsp_ptr->params.fwd_sch0_mux_option = config->for_sch[0].for_sch_mux;
  rsp_ptr->params.fwd_sch0_radio_config = config->for_sch[0].sch_chn_cfg.sch_rc;
     
  if(!config->rev_sch_cc_incl || (config->num_rev_sch == 0))
  {
    MSG_MED("Channel config for R-SCH0 is not available", 0, 0, 0);
  }
  rsp_ptr->params.rev_sch0_mux_option = config->rev_sch[0].rev_sch_mux;
  rsp_ptr->params.rev_sch0_radio_config = config->rev_sch[0].sch_chn_cfg.sch_rc; 

  return rsp_ptr;

} /* mc_diag_get_nw_params */

/*===========================================================================

FUNCTION MCLOG_PAGING_MER_STAT
  This function handles the Paging MER Statistics command from Diag.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the response packet

SIDE EFFECTS
  None

===========================================================================*/

PACKED void *mclog_paging_mer_stat (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_req_type *req_ptr;
  DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_rsp_type);

  req_ptr = (DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_req_type *)req_pkt;

  if (pkt_len != sizeof (DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  /* Allocate the memory for response */
  rsp_ptr = (DIAG_SUBSYS_CALLP1X_MC_DIAG_SUBSYS_PAGING_MER_STAT_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CALLP1X, MC_DIAG_SUBSYS_PAGING_MER_STAT, /*lint !e641 */
                                 rsp_len );

  if (rsp_ptr != NULL)
  {
    if (req_ptr->counters_bitmask & MC_DIAG_PAG_RXED_REAL_SCI_ASGN_SLOT)
    {
      rsp_ptr->page_rxed_real_sci_asgn_slot_ctr = mclog_mer.page_rxed_real_sci_asgn_slot_ctr;
    }
    else
    {
      mclog_mer.page_rxed_real_sci_asgn_slot_ctr = 0;
      rsp_ptr->page_rxed_real_sci_asgn_slot_ctr = 0;
    }

    if (req_ptr->counters_bitmask & MC_DIAG_PAG_RXED_REAL_SCI_NEXT_SLOT)
    {
      rsp_ptr->page_rxed_real_sci_next_slot_ctr = mclog_mer.page_rxed_real_sci_next_slot_ctr;
    }
    else
    {
      rsp_ptr->page_rxed_real_sci_next_slot_ctr = 0;
      mclog_mer.page_rxed_real_sci_next_slot_ctr = 0;
    }

    if (req_ptr->counters_bitmask & MC_DIAG_PAG_RXED_TELE_SCI_ASGN_SLOT)
    {
      rsp_ptr->page_rxed_tele_sci_asgn_slot_ctr = mclog_mer.page_rxed_tele_sci_asgn_slot_ctr;
    }
    else
    {
      mclog_mer.page_rxed_tele_sci_asgn_slot_ctr = 0;
      rsp_ptr->page_rxed_tele_sci_asgn_slot_ctr = 0;
    }

    if (req_ptr->counters_bitmask & MC_DIAG_PAG_RXED_TELE_SCI_NEXT_SLOT)
    {
      rsp_ptr->page_rxed_tele_sci_next_slot_ctr = mclog_mer.page_rxed_tele_sci_next_slot_ctr;
    }
    else
    {
      rsp_ptr->page_rxed_tele_sci_next_slot_ctr = 0;
      mclog_mer.page_rxed_tele_sci_next_slot_ctr = 0;
    }

    if (req_ptr->counters_bitmask & MC_DIAG_REAL_SCI_PAGE_SLOT)
    {
      rsp_ptr->real_sci_page_slot_ctr = mclog_mer.real_sci_page_slot_ctr;
    }
    else
    {
      rsp_ptr->real_sci_page_slot_ctr = 0;
      mclog_mer.real_sci_page_slot_ctr = 0;
    }

    if (req_ptr->counters_bitmask & MC_DIAG_TELE_SCI_PAGE_SLOT)
    {
      rsp_ptr->tele_sci_page_slot_ctr = mclog_mer.tele_sci_page_slot_ctr;
    }
    else
    {
      rsp_ptr->tele_sci_page_slot_ctr = 0;
      mclog_mer.tele_sci_page_slot_ctr = 0;
    }
  }
  else
  {
    MSG_ERROR ("Null pointer from diagpkt_subsys_alloc", 0,0,0);
  }

  return rsp_ptr ;

} /* mclog_paging_mer_stat */

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

void mclog_inc_nw_param(mc_diag_nw_param_type param_type)
{
  switch(param_type)
  {
    case INTER_FREQ_IDLE_HO_COUNT:
      if((++mclog_inter_freq_idle_ho_count) == 0)
      {
        MSG_MED("mclog_inter_freq_idle_ho_count overflowed!", 0, 0, 0);
      }
      break;

    case HARD_HO_COUNT:
      if((++mclog_hard_ho_count) == 0)
      {
        MSG_MED("mclog_hard_ho_count overflowed!", 0, 0, 0);
      }	      
      break;
    default:
      MSG_MED("invalid parameter type %d", param_type, 0, 0);
  }
}

/*lint +e818 +e655 */
