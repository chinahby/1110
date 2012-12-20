/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C D M A   T R A F F I C   C H A N N E L   S T A T E
                    H A N D O F F   R O U T I N E S

GENERAL DESCRIPTION
  This module contains handoff support routines for the CDMA Sub-system,
  Mobile Station Control on the Traffic Channel state.

EXTERNALIZED FUNCTIONS

  *********************
  * General Functions *
  *********************

  MCCTCHO_TC_INIT
    - Called upon entering Traffic Channel to perform any initialization this
      module requires.

  MCCTCHO_TC_EXIT
    - Called when leaving the traffic channel to perform any cleanup this
      module requires.

  MCCTCHO_PROCESS_FM_HO_MSG
    - Processes Analog Handoff Direction Message

  MCCTCHO_HANDOFF_TO_CURRENT_ASET
    - Fakes handoff to current ASET (T_TMOBILE only)

  *********************
  * Hard/Soft Handoff *
  *********************

  MCCTCHO_PROCESS_GENERIC_HANDOFF_MSG
    - First called when any Handoff Direction Message (Original, Extended,
      General) is received.  For a soft handoff with implicit action time,
      this function performs the entire handoff in one call.  Otherwise
      the first call sets up the handoff and returns, waiting for the action
      time (txc_frame_sig).  All hard handoffs are converted to use an
      explicit action time.
    - If an explicit action time is used, this function is called a second time
      at the action time of the message.  If the handoff is a soft handoff,
      this second call will complete the handoff.  For a hard handoff, the
      second call performs all the actions required at the action time of the
      message, including waiting for another frame to pass (i.e. waiting for
      txc_frame_sig to be set again) and tuning the RF, if necessary.

  MCCTCHO_PROCESS_FAILED_HHO
    - Called upon receipt of the SRCH_ABORT_R report or the expiration of the
      handoff timer, to perform the processing associated with the failure
      of a Return-If-Fail hard handoff.  We wait for a SRCH_CF_NSET_RPT
      if required, otherwise we send SRCH_RIF_HHO_FAILURE_F, which results in
      SRCH sending SRCH_RIF_HHO_FAILURE_R.

  MCCTCHO_PROCESS_SRCH_CF_NSET_RPT
    - Called upon receipt of the search report containing the strengths of
      the candidate frequency neighbors that were searched.  This occurs both
      for handoff and candidate frequency search.
    - For handoff, the neighbors reported are those we searched before
      returning from the failed handoff.  We send SRCH_RIF_HHO_FAILURE_F
      at this time, which results in SRCH sending SRCH_RIF_HHO_FAILURE_R.

  MCCTCHO_RETURN_TO_SERV_FREQ
    - Processes the return to the serving configuration either after a failed
      Return-If-Fail Hard Handoff or after a Candidate Frequency Search visit.
    - For handoff, this function is called upon receipt of the
      SRCH_RIF_HHO_FAILURE_R.

  MCCTCHO_POST_PROCESS_HANDOFF
    - Called upon receipt of RXC_TX_ON_R.  If the handoff was successful it
      performs all the processing required to complete the handoff.  If the
      handoff was a failure it sends the Candidate Frequency Search Report.

  ******************************
  * Candidate Frequency Search *
  ******************************

  MCCTCHO_PROCESS_CAND_FREQ_SRCH_REQ_MSG
    - First called when the Candidate Frequency Search Request Message is
      received.  An implicit action time is converted to be explicit, and
      we wait for the txc_frame_sig to be set before processing the search
      request.
    - Called a second time at the action time of the message.  Sends the
      new candidate frequency search information to SRCH and starts/stops
      any candidate frequency searching as required.

  MCCTCHO_PROCESS_CAND_FREQ_SRCH_CTRL_MSG
    - First called when the Candidate Frequency Search Control Message is
      received. An implicit action time is converted to be explicit, and
      we wait for the txc_frame_sig to be set before processing the search
      control.
    - Called a second time at the action time of the message.  Starts/stops
      any candidate frequency searching as required.

  MCCTCHO_START_CF_MEAS
    - Called upon receipt of the SRCH_CFS_READY_R report, which
      indicates SRCH needs to perform a visit to the Candidate Frequency.
      This function tunes to the candidate frequency and starts the searching.
      When SRCH is done it will send SRCH_CF_MEAS_DONE_R or SRCH_CF_NSET_RPT.

  MCCTCHO_PROCESS_SRCH_CF_NSET_RPT
    - Called upon receipt of the search report containing the strengths of
      the candidate frequency neighbors that were searched.  This occurs both
      for handoff and candidate frequency search.
    - For candidate frequency search, this report signifies that the candidate
      frequency search is complete (all neighbors in the candidate frequency
      search set have been searched).  This also implies that this visit to the
      candidate frequency is over and we should tune back.  This is done by
      calling mcctcho_return_to_serv_freq() directly.

  MCCTCHO_RETURN_TO_SERV_FREQ
    - Processes the return to the serving configuration either after a failed
      Return-If-Fail Hard Handoff or after a Candidate Frequency Search visit.
    - For candidate frequency search, this function is called upon receipt of
      the SRCH_CF_MEAS_DONE_R report, or from mcctcho_process_srch_cf_nset_
      rpt(), if SRCH finished searching the last neighbors in the candidate
      frequency search set during this visit.

  **************************
  * Action Time Processing *
  **************************

  MCCTCHO_PROCESS_TXC_FRAME_SIG
    - Called when the txc_frame_sig is set.  This signal is set at the action
      time (actually at the encoder interrupt during the frame preceeding the
      action frame boundary), of a handoff, candidate frequency search request,
      or candidate frequency search control.  When called, this function calls
      the appropriate function to process the message for which the action
      time applies.  Those function are:
      - mcctcho_process_generic_handoff_msg().
      - mcctcho_process_cand_freq_srch_req_msg().
      - mcctcho_process_cand_freq_srch_ctrl_msg().
    - The message processing functions for messages with explicit action times
      are actually called twice; 1) directly from mcctc.c when the message is
      received and 2) from here, at the action time.
    - There is currently only room for one message with a pending action time.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1990-2010 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctcho.c_v   1.43   03 Oct 2002 10:51:10   phosabet  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mcctcho.c#3 $ $DateTime: 2010/11/09 07:33:32 $ $Author: sshahi $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/10   ssh     Added processing of SMCM with use_time as TRUE.
06/01/09   ag      Support to display network parameters on UI
05/13/09   pk      Fixing compiler warnings
04/22/09   pk      Changed IS2OOO_SCH to IS2000_SCH 
02/12/09   ag      Delaying the implicit action time transaction for CFSRQM
                   so that MS sends CFSRSM within T56 time.
08/25/08   pk      Included the right clkregim header file for different targets
08/21/08   ag      Included cdma2kdiag.h as part of DIAG VU decoupling process
07/21/08   jj      On HO failure, MC calls SD function to update the SD 
                   parameters.
06/26/08   pk      When DCVS is on, vote for the higher clock rate upon receiving
                   the UHDM and vote for the lower clock rate upon sending the 
                   Handoff Completion message.
06/19/08   ag      Fixed compiler warnings.
05/19/08   ag      Reverted the previous change.
05/19/08   ag      Corrected the UHDM validation issues.
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
03/29/08   bb      Fixing Klocwork errors
12/26/07   ss      Changes to enforce band class checkings for CFSRQM in srch_type
                   SRCH_NO_CFS along with SRCH_SINGLE_CFS and SRCH_PERIODIC_CFS.
12/19/07   pg      Start CFS transaction when srch_type in CFSREQM is no_CFS
                   so that RF parameter for the next srch can be passed to
                   Searcher.
12/14/07   an      Added Passport Mode
08/29/07   pg      Tell srch to return to serving frequency when getting a 
                   SRCH_CF_NSET_RPT after aborted the current periodic search.
05/11/07   pg      Lint Cleanup.
04/04/06   pg      Do not start CFS transaction when srch_type in CFSREQM or 
                   CFSCTLM is NO_CFS.
                   Stop pending CFS transactions when issues SRCH_STOP_CFS_F
                   to Srch. 
02/14/07   sb      Added event for VOIP-1X handoff.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
10/12/06   fc      Fixed the issue of sending bogus CFSRM before the 
                   completion all NSET searches.
09/30/006  fc      Added P_REV check for mob_qos update.
09/14/06   fc      Fixed the issue of reporting cf_total_rx_pwr as 
                   sf_total_rx_pwr in CFSRM.
09/05/06   fc      Fixed compilation error for Rel C being turned off.
08/29/06   fh      AKA/AES: cleaned up the usage of AES capability functions
08/29/06   fh      AKA/AES: check AKA/AES fields first before service negotiation.
                            This makes sure the AES ui_encrypt_mode validation is
                            against the BS capability in UHDM.
07/29/06   fh      AKA/AES: update the ui_encrypt_mode and SR_ID information in 
                            AES status for voice encryption after handoff is 
                            successfully performed.
08/29/06   pg      Added support for release D Packet Zone Hysteresis.
07/24/06   fh      AKA/AES: fixed a bug in setting BS AES capability according
                            to UHDM
07/18/06   fc      Fixed merge error.
07/13/06   fh      AKA/AES fix: fixed a bug that sets wrong ordq when UHDM is rejected
                   due to AKA/AES validation
07/11/06   fc      Updated function prototype for mcc_send_cm_srv_sys_info.
07/07/06   fc      Obsolete unreferenced variable cdma.code_chan.
                   Added logging support for UMTS.
06/23/06   pg      RPC cleanup.
06/12/06   fc      Added the handling of release_to_idle_ind, mob_qos and
                   register_in_idle parameters from UHDM.
06/09/06   fc      Removed extern function definition for srch_agc_to_dbm. 
06/09/06   fh      AES related change: when AES feature is defined, the validation
                    of encrypt_mode field in function validate_handoff_msg is not needed.
                    The field will be validated according to AES procedure 
                    under AES feature.
06/02/06   fc      Cleanup umts interface..
05/30/06   fc      Fixed lint errors.
05/17/06   fh      Merged in new development of AKA/AES features
04/24/06   fc      Fixed Rel A featurization.
04/24/06   an      Fixed lint errors
03/06/06   fh      Added AES support
01/20/06   fc      Merged the following :
           fh      Added support for AKA and MI.
12/20/05   sb      Replaced cm_mc_rpt_free_q with cm_mc_rpt_get_buf_else_err_fatal().
10/15/05   fc      Fixed featurization errors.
10/03/05   an      Fixed lint errors
09/29/05   sb      Replaced DB_VOICE_PRIVACY with report to CM.
07/24/05   sb      Added support for tri-band under FEATURE_T53.
07/20/05   sb      Removed error check for private_lcm=1 and plcm_type_incl=1.
07/11/05   fh      Cleaned up header file including from MUX:
                     changed txc.h to txcmc.h
                     added including of rxcmc.h
07/11/05   va      Moved p_rev check for MEID UHDM to the top of validate fn
07/07/05   va      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
05/31/05   fc      Merged the following from MSMSHARED:
05/31/05   va       Moved RF tuning to SRCH as part of SHDR changes
04/04/05   fc      Merged the following from MSMSHARED:
03/31/05   fc       Added support for EPZID.
03/30/05   fh       Added code to update db info after successful Handoffs in 
                    traffic state
03/28/05   fc      Made FCS also available to Rel D.
03/23/05   fc      Merged the following from MSMSHARED
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated 
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/01/05   fc      Merged the following from MSMSHARED:
02/23/05   sb       Set the ch_ind to FCH-only for GHDM, EHDM and HDM.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/10/05   va      Merged follwoing to MSMSHARED Tip:
02/03/05   fc       Fixed lint error.
02/08/05   sb      Renamed functions as per code review comments.
02/03/05   az      Changes for Rel D DV TDSO.
01/03/05   va      Changed qw_add to use qw_inc instead
12/29/04   va      Added support for TX off procedures due to CFS
12/02/04   sb      Optimized UHDM validation.
11/24/04   sb      Removed unnecessary condition in validation checking.
11/22/04   sb      Miscellaneous fixes for Rel C DV Handoffs.
11/05/04   va      Merged following from REL_B_C archive:
10/01/04   pg        Set chm_supported to 1 for p_rev less than 9.
11/05/04   sb      Added support for Rel C DV handoffs.
10/25/04   az      Supported for Rel C sdb_supported bit in ITSPM and UHDM
10/01/04   sb      Fixed lint warning.
09/27/04   va      Merged from REL B/C archive:
08/19/04   yll      Clean up the MC/SRCH interface.
08/23/04   pg       Clear all retry delays if requested by UHDM instead of just
                    CAI_RETRY_SCRM.
08/11/04   pg       Added Control Hold Mode for release C addendum UHDM.
07/21/04   sj       Added support for umts to 1x handover.
07/01/04   va       Corrected lint errors
06/24/04   sb       Release C addendum changes for PLCM.
06/24/04   fc       Separate the event processing for sunfunction timer 
                    expiration and SCR transaction completion.
08/19/04   ht      Change flag to force a manual reset of DV TDSO
                   counters after a HO.
08/06/04   bkm     Added function mcctcho_is_hho_in_progress(). 
08/05/04   va      Changed the implicit AT of CFS Req mesg to be 2 frames
                   later (Giving time for CFS resp to go out)
08/02/04   va      Removed mcctcho_handle_cfs_at_arrival fn prototype
                   Removed tuning for CFS from MC
07/29/04   ht      Added TDSO support for Rel C DV channels
07/19/04   sb      Fixed merge mistake.
06/29/04   bkm     Resolve some merge issues.
06/28/04   bkm     Fixed compile issue.
06/28/04   bkm     Merged the following from IS2000_REL_B_C:
  06/24/04   sb      Release C addendum changes for PLCM.
  06/24/04   fc      Separate the event processing for sunfunction timer 
                     expiration and SCR transaction completion.
06/16/04   va      Added support for HHO/CFS/GPS clean up
06/16/04   fc      Release C addendum changes for Sync ID.
06/14/04   sb      Updated field names as per Release C Addendum.
06/08/04   bkm     Fixed typos in feature names within comments.
06/03/04   bkm     Merged the following from IS2000_REL_B_C:
  05/24/04   fc      Included mccsyncid.h.
05/28/04   bkm     Added FEATURE_GPS_MODE around mcctcsup_update_off_time_info().
05/28/04   bkm     Merged the following from IS2000_REL_B_C:
  05/19/04   sb      Made merge official via integrate.
  05/17/04   sb      Added update for UHDM p-rev 9 Off Time Reporting fields.
  05/14/04   sb      Merged the following from msmshared:
    04/08/04   ph      Migrate call_id to ACP when processing AHDM.
    03/05/04   bkm     Lint cleanup.
  04/19/04   sb      Added function mcctcho_update_long_code_mask_info.
  04/09/04   sb      Moved UHDM private long code validation.
  03/19/04   sb      Added support for Release C Decoupling PLCM from ESN
  03/12/04   fc      Added support for Release C fast call setup.
  01/29/04   yll     Fixed a wrong featurization.
04/08/04   bkm     Channel mask updates.
01/20/04   bkm     More CHIND to channel mask updates.
01/13/04   bkm     Added notes on more things to add for DV.
01/05/04   bkm     Misc updates for DV F-PDCH support and also removed
                   function used to set frame offset from MC context.
12/09/03   sb      Renamed thx_info struct to thx_and_ccsh_info.
12/02/03   bkm     Split internal chind into forward and reverse channel masks.
11/04/03   yll     Changes with srch_tune_rfm() interface.
10/29/03   sst     Correctly featurized variable rf_div in mcctcho_tune_rf()
10/29/03   sst     T_M6500 in last checkin changed to proper T_MSM6500
10/29/03   sst     Surrounded SRCH_HAS_PILOT_GAIN_CNTL with T_M6500
10/24/03   sst     Changed RF tuning calls with srch_tune_rfm(), featurized 
                   as SRCH_HAS_PILOT_GAIN_CNTL, also added band_class param
                   to mcctcho_tune_rf()
10/23/03   va      Changed caii_ext_ch_ind3_pilot_rec_type to
                   mccdma_pilot_info_type
10/23/03   yll     Fixed a problem that in HHO we should tune RF based on
                   the RX diversity status.
09/29/03   bkm     Use common srch function to convert Rx AGC to DBM.
09/11/03   yll     Added FEATURE_ACP to analog code.
09/03/03   sb      Moved error message to mccccim_process_event.
08/21/03   va      Stored the value of srch_period from CFS Req Msg. W/o this, 
                   can cause CFS Control mesg to be rejected unnecessarily.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/06/03   sb      Added support for CDMA Off Time Reporting.
08/01/03   ph      Added chind to snm_eval_config() call.
08/01/03   yll     Added support for Dynamic Feature.
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
05/23/03   va      Conformed to the new prototype of mccccl : is_in_active_set
05/20/03   fc      Moved the saving of serving config and the update of CCL 
                   for UHDM with RIF to mcctxns_ho_pre.
04/15/03   bkm     Send Reject Order if invalid SEARCH_TYPE in CFS msgs.
02/25/03   va      Removed raising and lowering MC's priority during 
                   CFS & HHO
01/24/03   lh      Modified pzid change processing interface.
12/18/02   ph      Allowing for the SRV event to be called when we get a U/GHDM
                   with no SCR, but NNSCR is included.
12/04/02   fc      Re-instate NNSCR fix for G/UHDM.
11/07/02   az      Removed FEATURE_IS2000_REV0_PREADDENDUM
11/07/02   az      Mainlined FEATURE_IS95B
11/07/02   az      Removed the feature TMOBILE
10/03/02   ph      Lint Cleanup
10/02/02   az      Mainlined FEATURE_IS2000
09/30/02   sb      Mainlined FEATURE_SD20.
09/25/02   bkm     Ignore a BS request to gate reverse FCH if this optional
                   IS2000 feature is disabled.
09/16/02   ph      Expanded scope of NNSCR evaluation to cover all scenarios.
09/13/02   fc      Fixed compilation errors for FEATURE_GPSONE_UPDATE_BSID.
09/06/02   va      Initialized new_fpc_pri_chan in mcctcho_ho_data data struct.
08/05/02   va      Check for fpc_mode when HDM puts MS in CH.
07/25/02   ph      Fixed paranthesis for Rev SCH assignment check in validate_handoff_msg()
07/24/02   sb      Moved srch_offset_nghbr to ccl.
06/21/02   ph      changed cmmc.h to cmxll.h
06/20/02   az      Fixed compiler warnings
06/05/02   fc      Moved the reporting of handoff events out of transaction
                   server context.
06/03/02   fc      Fixed the reporting of handoff events at action time.
05/31/02   va      Added VP2 support, fixed problem with max_rate error
                   checking.
05/20/02   sb      Added reject order if num_pilots is 0 in UHDM.
05/07/02   fc      Corrected hard/soft handoff event payload.
05/07/02   lcc     Corrected a feature #ifdef in validate_handoff_msg.
04/17/02   sb      Event cleanup.
03/26/02   va      Fixed the issue of SCH assignment not being cancelled upon
                   RIF HHO failure.
03/19/02   va      Added support for new version of the Handoff events.
03/11/02   va      Merged from MSM archive.
  2/07/02  hxw     Added FEATURE_GPSONE_UPDATE_BSID to quickly pick up proper 
                   Base Station ID.                    
02/22/02   fc      Added support for Release A common channels.
02/13/02   va      Merged from MSM archive.
  1/31/02  hxw     Added FEATURE_GPSONE_UPDATE_BSID to quickly pick up proper 
                   Base Station ID.                    
  01/18/02 lcc     In mcctcho_process_tx_stop, added update of CCL after saving
                   serving config for RIF HHO case if it's a UHDM since for this
                   case CCL update is delayed.
  01/04/02 ht      Enclosed mccsch_sch_is_active() within FEATURE_IS2000_SCH.
01/17/02   va      Merged with 1.49 in MSM archive.
 12/10/01  lcc     Corrected #ifdef for MAAHO in mcctcho_process_tx_stop such
                   that HHO works when FEATURE_IS95B_MAHHO is turned off.
01/14/02   lh      Changed return interface for CC info processing in SCM and UHDM.
12/13/01   va      Merged from SVD branch
  11/30/01 va      Added support for  Call control Instance management for
                   RIF-HHO.
  08/28/01 lh      Added support for Concurrent Service.
10/26/01   gs      Merged from MSM_CP.05.00.100.01.00
                   CFS tunes now use RF_CFS_TUNE_WAIT
10/23/01   jrp     Merged the following from common archive (MSM_CP.05.00.99)
                   10/15/01   ph      Fixed misleading message w.r.t. Control Hold and Gating Rate in UHDM.
                   10/05/01   lcc     Updates SCH assignment with UHDM before despatching the
                                      HO transaction.
10/15/01   kk      Fixed compiler errors when Release A is not defined.
10/02/01   ph      Merge from CP VU 96
                   09/18/01   lcc     Corrected validation of SCH mux options in when validating
                                      UHDM which includes SCR.
                   09/13/01   lcc     Changed an FCH assignment to stop to use a "start SCH"
                                      transaction instead of "stop SCH" since a "stop" is also
                                      considered an assignment (which overrides earlier ones).
                   08/22/01   lcc     For UHDM, moved update of CCL and SCH info to pre-processor
                                      of handoff transaction (in mcctxns) so that it is executed
                                      before the SCH transaction that follows, if included.
08/16/01   kk      Merge from main archive CP VU 89.
  08/08/01 yll     Unblock the searcher's GPS visit request after a soft HO.
08/09/01   lh      Merged following from MSM_CP.05.00.87  
  07/13/01 lcc     In validate_handoff_msg, moved error checking of CH_IND out
                   of if (ho->sch_incl).
06/26/01   va      Merged the following from MSM_CP.05.00.81
  06/14/01 lcc     Added cancellation of pending/current F/R-SCH assignments
                   when performing a hard handoff to conform to IS2000 Rel 0
                   Addendum 2.
  06/13/01 ht      (jq) Enclosed mccsch_verify_fwd_assignment() call
                   inside FEATURE_IS2000_SCH.
  06/01/01 jq      Wrapped some code in FEATURE_IS95B_MAHHO.
  05/29/01 fc      Changed snm_update_no_neg_configuartion function prototype.
  05/29/01 ht      (kjn) Added a #ifdef FEATURE_DS.
  05/22/01 lcc     Corrected start time of SCH to be based on UHDM action time.
  05/18/01 va      Fixed the issue which caused inter-band HHO to fail. (Moved the band
                   select before RF freq tuning).
  05/14/01 jq      Modified mcctcho_tc_init().

06/14/01   va      Added AHDM primitive going to MCCCCIM.  Changed prototype of
                   send_rej_order.
05/25/01   va      Changed the argument to snm_update_non_neg_cfg
05/16/01   fc      Merged the following from main archive:
                   05/03/01   va      Fixed issues with p_rev not updating 
                                      correctly only if parms_incl is set to 
                                      1 in UHDM.
                                      Changed RXC_RETURNED_F payload to match
                                      the new interface.
                   05/03/01   jq      Added checking for valid SCH asgn after HO
                   05/02/01   fc      Added support of T_ADD_ABORT.
                   04/25/01   lcc     1. Added validation of SCCL if an UHDM 
                                         contains a F-SCH assignment.
                                      2. Corrected validation of SCH assignment
                                         in UHDM to check for inclusion of 
                                         assignment rather than config info.
                   04/25/01   jq      Added ifdef to clear a compile error.
                   04/20/01   fc      Changed handoff events' payload to use bit
                                      field definition.
                                      Fixed merge errors.
                   04/16/01   lcc     1. Replaced some ASSERT statements with 
                                         MSG_ERROR which should
                                         have been the case.
                                      2. Optimized hard handoff such that tuning
                                         RF and other things are done in 
                                         parallel instead of sequentially.
                   04/13/01   va      Fixed bugs related to packet zone id 
                                      updates.
                   04/12/01   fc      Added support of events report: Channel
                                      indicator change,
                                      F-FCH acquired, F-DCCH acquired and F-FCH 
                                      plus F-DCCH acquired.
                   04/10/01   jq      Modified UHDM processing to support P2 and
                                      Control Hold
                   04/06/01   va      Fixed action time handling for CFS req and
                                      Control messages.
                   03/28/01   fc      For GHDM or UHDM specified only NNSCR,
                                      just save NNSCR and don't treat it as 
                                      service connect.
                                      Fixed incorrect condition checkings based 
                                      on service_included.
                   03/22/01   lcc     1. Added notification of HHO success to 
                                         TXC even for non-RIF HHO.
                                         Otherwise, preambles won't be sent.
                                      2. Calls function tdso_notify_hho_success
                                         to perform HHO post-processing for TDSO
                                         only if it is a full TDSO call.  It is
                                         used to do that regardless of the call
                                         type.
05/10/01   lh      Added 307K support for F-SCH.
05/04/01   va      Verified cc_info_incl AND 3x in UHDM against our capability.
                   Added support for passing p_rev_in_use to MUX layers.
04/18/01   va      Added support for Rel A UHDM
03/08/01   fc      Added the initialization of event_handoff in
                   pre_process_handoff_msg to avoid garbage reported in handoff
                   payload.
03/06/01   fc      Added support of events report: Soft handoff, Hard Handoff
                   and Unsuccesful hard handoff.
02/27/01   mai     Changed FEATURE_GPSONE to FEATURE_GPS_MODE to move code needed
                   only for GPS part of position location to a new #define.
02/26/01   sko     Removed JCDMA feature to poll CDMA_CH by CM.
02/22/01   fc      Cleanup debug messages.
02/16/01   fc      Added support for system determination 2.0. All changes 
                   are featurized under FEATURE_SD20.
02/01/01   pj      Deprecated RXC_RIF_HHO_OR_CFS_F completely.
01/27/01   ks      Calling TDSO to inform a successful Hardhandoff is performed.
01/24/01   ych     Merged T-53/JCDMA features for SK.
01/24/01   jq      Changed naming of some variables and functions so that the
                   software can compile with FEATURE_IS2000_SCH undefined. 
01/08/01   jq      Made some naming changes to prepare for IS2000_P2.
01/05/01   djd     Fixed IS2000 Trimode build problem in mcctcho_process_tx_stop().
12/15/00   lh      Add support for MS originated Long Code Transistion Request
                   Order
12/11/00   yll     Fixed a compiling error when undef FEATURE_IS95B_MAHHO.
11/13/00   lcc     Added support for reporting Band Class and CDMA channel
                   change events.  Moved update of p_rev into
                   mcc_udpate_bs_p_rev.
09/21/00   ks      Fixed the if statement while SCH verification in
                   validate_handoff_msg() function.
09/14/00   jq      Added UHDM SCH verification in validate_handoff_msg().
08/23/00   lcc     1. Changed mcctcho_generate_aset to return sup_state and removed
                      transaction_num from function parameter.
                   2. Included "sup_state" in generate_aset_default_sups.
                   3. Corrected lint warnings.
07/31/00   yll     Fixed the problem where a periodic CFS blocks the GPS
                   search.
07/28/00   fc      Mainline FEATURE_CS_ACTION_TIME_TRANS.
07/21/00   va      Fixed  a bug, mccsch_update_assign_with_uhdm should
                   be called only if sch_incl=1
07/17/00   va      Support for CFS transactions, rev pwr cntl delay and
                   8th rate gating
07/10/00   yll     Backed the RXC_RIFHHO_CFS_GPS_F to RXC_RIF_HHO_OR_CFS_F to
                   minimize the effect on MSM5000.
07/07/00   va      Fixed CFS bug where by the CF set may not be correctly updated
                   sometimes.
07/07/00   cah     Non95B & non-Special MDR compiler error fixes to
                   pre_process_handoff_msg() and mcctcho_process_tx_stop().
07/06/00   yll     More GPS changes to correctly handle the GPS visit request
                   when a single/periodic CFS is in progress.
07/06/00   yll     Added GPS support: set the signal to block searcher's GPS
                   visit request when a handoff or single shot CFS is in
                   progress. Provide a function to check if a handoff or
                   single shot CFS is in progress.
06/30/00   fc      Modified mcctcho_process_generic_handoff_msg() to process
                   SCRM pilot strength reporting offset and retry delay.
06/29/00   ks      Aborting the PPSMM procedure only if the Mobile receives a
                   HDM message and it is in the process of Hard Handoff. For
                   Soft Handoff, PPSMM is not cancelled.
06/19/00   ht      Add condition to allow MS to return to serving system
                   band after a CFS search.
06/13/00   jq      Fixed a BAND_CLASS checking error for cdma2000
06/12/00   lcc     As part of unifying action time processing, migrated transactions
                   HO, SCR, FPC_SUBCHAN_GAIN, and PWR_CTRL_MSG to mcctxns.  Added
                   featurization FEATURE_CS_ACTION_TIME_TRANS for new code which will
                   be mainlined eventually.
06/05/00   lcc     Corrected a featurization problem with snm.h.
06/02/00   lcc     1. Changes to conform to CDMA-2000 published addendum.
                   2. Miscellaneous minor cleanup.
06/02/00   jq      Fixed a feature define problem in
                   do_successful_handoff_operations ().
05/25/00   ks      Added support for Neighbor Search Window offsets.
05/16/00   lcc     Added processing for R-SCH assignment.
04/24/00   lcc     Added processing of SCH assignment.  Removed 3G data prototype
                   code.
04/24/00   va      Rearranged code in initialization of mcc_rx_buf.ho in
                   send_ho_cmds_to_lower_layers so that the compiler does not
                   try to optimize the code.
04/20/00   va      For MSM3100, RXC_TUNE_RF_F cmd is not sent now if RIF=0
04/18/00   lcc     Corrected a problem with handoff when FEATURE_3G_DATA_PROTOTYPE
                   is defined.  Also correct a merge error with frame_offset.
04/18/00   va      Added function prototype for mcctcho_process_tx_stop
04/17/00   va      Fixed the msg_type initialization bug for HO for MSM3100.
04/17/00   va      Checked for parms_incl flag in UHDM processing.
04/11/00   va      Fixed some issues with non FEATURE_IS95B compiles.
04/07/00   va      Merged the following from MSM3100_CP.03.00.17:
  03/14/00 ry      CR 12118 and 12467: Fixed the problem of receiving a good
                   frame right before the mobile tunes to the TF/CF during a
                   HHO or CFS.  This is done by sending the RXC_TUNE_RF_F
                   command to RXC at action time to inform RXC to start
                   ignoring good frame
03/28/00   ry      Fixed a compiler error introduced from the previous merge
03/24/00   ry      Merged the following from MSM3100_CP.02.00.10:
           ry      CR12839 Updated mcctcho_process_cand_freq_srch_req_msg() to
                   check for unsupported band class upon receipt of a CFSReqM
           ram     CR12762: In the function send_cand_freq_srch_rpt_msg() we
                   were using an unsigned variable to hold the rx power reported
                   by the searcher. This resulted in sending the incorrect sf
                   rx power value in the CFSReport Message. The variable has
                   benn changed to signed integer.
           dna     CR12461 Changed tf_aset to cf_srch_results, since it stores
                   results from CF search as well as failed handoffs. Increased
                   size from 6 to 46 to allow for CF search set to fit.
03/22/00   lcc     Corrected CR 13284.  ASSERT in pre_process_handoff_msg
                   should be placed inside UHDM only.
02/16/00   lcc     Changed mcctcho_ho_data to include the HO message.
                   Changed pre_process_handoff_msg and validate_handoff_msg
                   to return error code instead of booleam.
                   Miscellaneous comment fixes.
02/13/00   va      Addendum Changes
01/27/00   va      Merged in changes from GV40401:
                   Removed mcc_handoff_timer. Added timeout and is_hho as payload
                   to the RXC_RIF_HHO_OR_CFS_F command.
           ry      Updated send_cand_freq_srch_rpt_msg() to correctly report
                   the SF total received power
12/20/99   va       Fixed bug with restoring fpc_subchan_gain
11/15/99   va      IS2000 Changes:
                   Action time procesing for HO mesgs have been moved to lower layers
                   All HO parameters needed for post processing is now stored in
                   mcctcho_ho_data.
11/03/99   lcc     Merged from GD4021:
           doj     Added comment clarifying the updating of PACKET_ZONE_ID.
           ry      Updated the calculation of the off times such that they
                   are now reported in 20-ms unit instead of milliseconds.
           ry      Fixed a problem with not initializing the cdma_freq and
                   band_class fields of the SRCH_TC_ASET_F command when both
                   FEATURE_IS95B and FEATURE_SPECIAL_MDR are off
           ry      Updated to NOT pass the current CFS sequence number to the
                   SRCH_CF_NSET_F command
           ry      Added support to save and restore SCRs
           ry      Added support to handle the race condition between soft HO
                   with an explicit action time and CFS
           ry      Converted implicit action time to explicit for LCM change
           ry      Added support to save and restore supplementals for
                   Return-If-Fail HHO
           ry      Instead of saving the active set in mcctcho_active_set,
                   always use the mccccl_code_channel_list_object to generate
                   the current active set
           ry      Fixed a problem with periodic search not starting after HHO
           ry      Fixed a problem with soft HO that was introduced in recent
                   code changes
           lh      Serving system update for CM is added.
           ry      Fixed implicit action time processing for HHO.  The fix also
                   provides partial support for two action times, as long as
                   one is not from a (E|G)HDM
           ry      Fixed a race condition which happens when the mobile
                   receives a GHDM, and prior to the action time of the GHDM,
                   the periodic timer for the CFS expires and the mobile starts
                   performing a CFS.  Note that this fix doesn't handle soft HO
           ry      Fixed the duplicate the SRCH_RIF_HHO_FAILURE_F problem
           ry      Added a check in CFSReqM processing to reject analog search
           ry      Correctly handled the SEARCH_SET field in the CFSReqM
           ry      Fixed a compilation problem with both FEATURE_IS95B and
                   FEATURE_SPECIAL_MDR undefined
           ry      Removed the #ifdef around the function to update the code
                   channel list - mccccl_code_channel_list.update_all() -
                   since generate_aset() and generate_aset_default_sups() both
                   depend on the code channel list
10/20/99  lcc      Added IS2000 MDR support.
09/20/99  lcc      Mainlined call to update_all.  Merged in fix on compilation
                   problem when FEATURE_AUTH is on.
08/26/99   jq      Corrected a mistake in
                   update_srch_parms_and_active_set_notify_srch() which used
                   the wrong duration timer in SCAM-GHDM linking
08/19/99   ry      Added support for rejecting a GHDM/CFSReqM/CFSCtrlM based
                   on search period too short - Added cf_srch_get_off_times()
                   and implemented cf_srch_period_too_short()
           ry      Instead of relying on CFSRM_SEQ and CFSCM_SEQ to identify
                   the current search request, we now maintain our own sequence
                   number because CFSRM_SEQ and CFSCM_SEQ might not be unique
                   across each other
08/18/99   ram     Enclosed the function call mccccl_code_channel_list.update_all
                   with #if defined(FEATURE_IS95B_MDR) || defined(FEATURE_SPECIAL_MDR)
                   since it's called only when both the pilots and supplemental
                   channels need to be updated.
08/13/99   doj     Removed update_packet_zone_id and update_pwr_cntl_step
                   stubs.  Replaced calls with
                   mccsyobj_system.pzid.process_when_active and
                   process_pwr_cntl_step respectively.  Added validation
                   checking for FEATURE_SPECIAL_MDR back into
                   validate_handoff_msg.  Added assignment of PWR_CNTL_STEP
                   in mcctcho_ho_data.  Changed
                   mcctcho_ho_data.use_pwr_cntl_step to
                   mcctcho_ho_data.pwr_cntl_step in call to
                   process_pwr_cntl_step.
08/09/99   ry      Added support for both single and periodic CFS.  New
                   functions:
                      cf_srch_current()
                      mcctcho_process_cand_freq_srch_ctrl_msg()
                      mcctcho_start_cf_meas()
                      process_cand_freq_srch_common()
                      send_cand_freq_srch_resp_msg()
           ry      Updated action time processing such that only a frame offset
                   handoff will change an implicit action time to explcit;
                   hence, removed determine_handoff_action()
           ry      Updated pre_process_handoff_msg() to reject a GHDM if a
                   hard handoff is requested and FEATURE_SPECIAL_MDR is defined
           ry      Checked if message encryption is possible only if the
                   handoff message is requesting message encryption
08/08/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
                   Changed FEATURE_IS95B_CFS to FEATURE_IS95B_MAHHO
                   Renamed mcctcho_post_ho_data to mcctcho_ho_data
                   Moved mcctcho_get_time_in_1_25ms_unit() to txc.c and
                   renamed the function txc_get_time_in_1_25ms_unit()
                   Implemented cf_srch_set_empty()
08/06/99   doj     Added sign extension to add_intercept and drop_intercept.
                   Fixed bug in which soft slope parameters were zeroed for
                   P_REV_IN_USE < 4 only if SRCH_INCLUDED was TRUE.  We want
                   it to always happen if P_REV_IN_USE < 4.
08/02/99   doj     Eliminated checking of return values for
                   mccccl_code_channel_list.update_all and
                   mccccl_code_channel_list.update_sups per code review
                   comments from Code Channel List Manager.
07/27/99   fc      Defined link_completed only if FEATURE_IS95B_MDR is defined
                   and initialized subs_needed to FALSE in
                   update_srch_parms_and_active_set_notify_srch() to eliminate
                   errors from ARM 2.50 compiler.
07/26/99   jq      Fixed some minor errors due to code review. Added #ifdef
                   around link_completed.
07/22/99   jq      Added #ifdef around the SCAM-GHDM linking data
07/22/99   jq      Added SCAM-GHDM linking
07/09/99   ry      Added support for IS95B HHO and CFS
06/17/99   kmp     Changed T_AUTH to FEATURE_AUTH.
06/15/99   doj     Added support for service configuration via handoff
                   message.
06/14/99   ry      Set return_if_ho_fail to FALSE if EHDM
                   Added more message output
05/18/99   doj     Created module.  Consolidated the various handoff
                   processing interfaces into one interface.  Took
                   MCCTCHO_SEND_HANDOFF_COMPLETE_MSG,
                   MCCTCHO_INIT_FRAME_OFFSET_HANDOFF,
                   MCCTCHO_PROCESS_FM_HO_MSG and
                   MCCTCHO_HANDOFF_TO_CURRENT_ASET out of MCCTCSUP.C to
                   include in this module.  Look in MCCTCSUP.C (v1.19) for
                   history prior to this date.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "mcctcho.h"
#include "target.h"
#include "customer.h"
#include "assert.h"
#include "diag.h"
#include "cdma2kdiag.h"
#include "err.h"
#include "mc.h"
#include "mccap.h"
#include "mccccl.h"
#include "mccdma.h"
#include "mccrxtx.h"
#include "mccsyobj.h"
#include "mcctci.h"
#include "mcctcsup.h"
#include "msg.h"
#include "rxtx.h"
#include "srv.h"
#include "ts.h"
#include "rxcmc.h"
#include "txcmc.h"
#include "snm.h"
#include "cmxll.h"
#include "mccccim.h"
#include "enc.h"
#ifdef FEATURE_OTASP
#include "otaspi.h"
#endif /* FEATURE_OTASP */

#if defined(FEATURE_IS95B_MDR)
#include "mccscm.h"
#endif // if defined(FEATURE_IS95B_MDR)

#include "cai.h"

#ifdef FEATURE_IS2000_TDSO
#include "tdso.h"
#endif

#ifdef FEATURE_IS2000_SCH
  #include "mccsch.h"
#endif
#include "mctrans.h"
#include "mcctxns.h"

#include "event.h"
#include "mclog.h"

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

#ifdef FEATURE_IS2000_REL_C
#include "mccreg.h"
#endif /* FEATURE_IS2000_REL_C */

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C_FCS ) || FEATURE_IS2000_REL_D */

#if defined(FEATURE_IS2000_REL_C_DV) || defined(FEATURE_IS2000_DV_SIG_TEST)
#error code not present
#endif

#if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif

#if defined( FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING ) || \
    defined( FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING )
  #ifdef CLKRGM_MSM_H
    #include CLKRGM_MSM_H
  #else
    #include "clkrgm_msm.h"
  #endif
#endif
 
/* Function Prototype */
void mcctcho_start_sch_assignments
(
  caii_ho_msg_type * ho
);

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

static boolean action_time_pending = FALSE;
  /* Indicator of whether an action time is pending */
static mcctcho_ho_data_type mcctcho_ho_data;
  /* Post hand-off data */


/* Search period values according to Table 6.6.6.2.8.3.2-1 of IS-95B
 * (units are in milliseconds)
 */
const dword srch_period_value[16] =
{
    480,   960,  2000,  2960,  4000,   4960,  10000,  20000,
  30000, 40000, 50000, 60000, 80000, 100000, 150000, 200000
};

mcctcho_cand_freq_srch_parm_type cand_freq_srch_parm;
  /* Candidate Frequency Search parameters */

#if defined (FEATURE_AUTH) || defined (FEATURE_IS2000_REL_C) || defined(FEATURE_MEID_SUPPORT)
static mcctcho_serv_freq_config_type serv_freq_config;
  /* Serving Frequency configuration */
#endif /* FEATURE_AUTH || FEATURE_IS2000_REL_C || FEATURE_MEID_SUPPORT */

#define MAX_VAL_FOR_OFF_TIME 0x3F
  /* Maximum value the mobile can report for TOTAL_OFF_TIME_FWD,
     MAX_OFF_TIME_FWD, TOTAL_OFF_TIME_REV, and MAX_OFF_TIME_REV */

#define MAX_VAL_FOR_SF_RX_PWR 0x1F
  /* Maximum value the mobile can report for SF_TOTAL_RX_PWR */

/*===========================================================================

                EXTERNAL DATA REFERENCES OF THIS MODULE

The following items stored in cdma_ms_type structure are used as read only:
  cdma.mob_cai_rev
  cdma.band_class
  cdma.vp_desired
  cdma.curr_state - For CDMA-to-Analog handoff processing only

The following items stored in cdma_ms_type structure are used as read/write:
  cdma.cdmach
  cdma.frame_offset
  cdma.encrypt_mode
  cdma.p_rev_in_use
  cdma.hdm_seq
  cdma.serv_neg

The following regional structures to MC task are used as read/write:
  mcc_ho_real     - Stores the latest searcher window sizes and parameters
                    for active set management (t_add, etc.).  This is used
                    to fill in fields of the SRCH_TC_ASET_F command if the
                    values are not included in the received handoff message.
  mcc_exit        - Used for CDMA-to-Analog handoff processing, to store
                    the reason for exiting CDMA mode.

The following command buffers regional to MC task are used to send commands:
  mcc_srch_buf
  mcc_rxtx_buf
  mcc_rx_buf

===========================================================================*/

/*===========================================================================

FUNCTION MCCTCHO_TC_INIT

DESCRIPTION
  This function initializes all necessary handoff data upon entering the
  traffic state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcctcho_tc_init( void )
{
  /* Initalization per section 6.6.4.2 of IS-95B */
  cand_freq_srch_parm.cf_cdma_freq = 0;
  cand_freq_srch_parm.cf_band_class = 0;
  cand_freq_srch_parm.tf_wait_time = 0x0F;
  cand_freq_srch_parm.num_pilots = 0;
  cand_freq_srch_parm.cur_srch_type = SRCH_NO_CFS;
  cand_freq_srch_parm.cur_srch_seq_num = 0;

  cand_freq_srch_parm.cf_srch_offset_incl = 0;

  mcctcho_ho_data.hard_handoff_in_progress = FALSE;

  mcctcho_ho_data.cfs_in_progress = FALSE;
  MSG_LOW("cfs_in_progress initialized to 0.", 0, 0, 0);

  mcctcho_ho_data.pending_cfs_meas_rpt_to_be_sent = FALSE;

  /* Clear action time pending flag */
  action_time_pending = FALSE;
} /* mcctcho_tc_init */

/*===========================================================================

FUNCTION MCCTCHO_TC_EXIT

DESCRIPTION
  This function initializes all necessary handoff data upon exiting the
  traffic state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function reverts MC back to its lower-than-SRCH priority

===========================================================================*/

void mcctcho_tc_exit( void )
{
  /* -------------------------------------------------
  ** Clear indicators that hard handoff is in progress
  ** ------------------------------------------------- */
  mcctcho_ho_data.hard_handoff_in_progress = FALSE;

  mcctcho_ho_data.cfs_in_progress = FALSE;
  MSG_LOW("cfs_in_progress is set to 0.", 0, 0, 0);

  mcctcho_ho_data.pending_cfs_meas_rpt_to_be_sent = FALSE;

  /* Clear action time pending flag */
  action_time_pending = FALSE;

} /* mcctcho_tc_exit */

/*===========================================================================

FUNCTION SEND_HANDOFF_COMPLETE_MSG

DESCRIPTION
  This function sends a Handoff Completion Message (ack required)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_handoff_complete_msg( void )
{
  rxtx_cmd_type *cmd_ptr;
    /* Pointer to command to send to layer 2 task */
  mccdma_pilot_info_type pilot_rec[CAI_N6M];
   /* Active set pilot record */
  byte i;
    /* Counter variable */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
      /* report to DIAG that a significant event is about to take place */
      diag_inc_event_cnt();

    /* -----------------------------------------
    ** Fill in Handoff Completion Message fields
    ** ----------------------------------------- */
    cmd_ptr->tc_msg.msg.comp.msg_type = CAI_HO_COMP_MSG;

    cmd_ptr->tc_msg.msg.comp.last_hdm_seq = cdma.hdm_seq;

    cmd_ptr->tc_msg.msg.comp.num_pilots =
      mccccl_code_channel_list.get_pilots( pilot_rec );

    for (i=0; i < cmd_ptr->tc_msg.msg.comp.num_pilots; i++)
    {
      cmd_ptr->tc_msg.msg.comp.pilot_pn[i] = pilot_rec[i].pilot_pn;
    }

    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    /* Send message to RXTX to be transmitted to the base station */
    rxtx_cmd( cmd_ptr );
  }

} /* send_handoff_complete_msg */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION SEND_EXT_HANDOFF_COMPLETE_MSG

DESCRIPTION
  This function sends an Extended Handoff Completion Message (ack required)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_ext_handoff_complete_msg( void )
{
  rxtx_cmd_type *cmd_ptr;
    /* Pointer to command to send to layer 2 task */
  mccdma_pilot_info_type pilot_rec[CAI_N6M];
   /* Active set pilot record */
  byte i;
    /* Counter variable */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
      /* report to DIAG that a significant event is about to take place */
      diag_inc_event_cnt();

    /* -----------------------------------------
    ** Fill in Handoff Completion Message fields
    ** ----------------------------------------- */
    cmd_ptr->tc_msg.msg.ehcm.msg_type = CAI_EHOCM_MSG;

    cmd_ptr->tc_msg.msg.ehcm.last_hdm_seq = cdma.hdm_seq;

    cmd_ptr->tc_msg.msg.ehcm.num_pilots =
      mccccl_code_channel_list.get_pilots( pilot_rec );

    for (i=0; i < cmd_ptr->tc_msg.msg.ehcm.num_pilots; i++)
    {
      cmd_ptr->tc_msg.msg.ehcm.act_pil_rec[i].pilot_pn = pilot_rec[i].pilot_pn;
      if (pilot_rec[i].add_pilot_rec_incl)
      {
         cmd_ptr->tc_msg.msg.ehcm.act_pil_rec[i].pilot_rec_incl = 
           populate_pil_rec_info_in_rtc_msg( (cai_pilot_rec_type * )&pilot_rec[i].pilot_rec_type,
                                             &pilot_rec[i].add_pilot_rec,
                                             &cmd_ptr->tc_msg.msg.ehcm.act_pil_rec[i].pilot_rec_type,
                                             &cmd_ptr->tc_msg.msg.ehcm.act_pil_rec[i].rec ); /*lint !e826 */
      }
      else
      {
        cmd_ptr->tc_msg.msg.ehcm.act_pil_rec[i].pilot_rec_incl = FALSE;
      }
    }

    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    /* Send message to RXTX to be transmitted to the base station */
    rxtx_cmd( cmd_ptr );
  }

} /* send_ext_handoff_complete_msg */
#endif //FEATURE_IS2000_REL_A

/*===========================================================================

FUNCTION START_CF_SRCH

DESCRIPTION
   This function is called upon the receipt of a Candidate Frequency Search
   Request/Control Message to start a single/periodic search, depending on
   the search type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function might also start a single or periodic CFS

===========================================================================*/

void start_cf_srch
(
  byte srch_type
)
{
  cand_freq_srch_parm.cur_srch_type = srch_type;

  /* If the SEARCH_TYPE field of the Candidate Frequency Search Request/Control
   * Message is equal to '01' or '03', perform a single/periodic search
   * accordingly
   */
  if ( ( srch_type == SRCH_SINGLE_CFS ) || ( srch_type == SRCH_PERIODIC_CFS ) )
  {
    mcc_srch_buf.start_cfs.hdr.cmd = SRCH_START_CFS_F;
    mcc_srch_buf.start_cfs.cfs_seq = cand_freq_srch_parm.cur_srch_seq_num;
    mcc_srch_buf.start_cfs.search_type = cand_freq_srch_parm.cur_srch_type;
    mcc_srch_cmd( &mcc_srch_buf );
    MSG_MED("Start CFS type %d, seq %d", srch_type, 
            mcc_srch_buf.start_cfs.cfs_seq, 0);

    #ifdef FEATURE_GPS_MODE
#error code not present
    #endif /* FEATURE_GPS_MODE */

  }

} /* start_cf_srch */

/*===========================================================================

FUNCTION STOP_CF_SRCH

DESCRIPTION
  This function is called to stop a single/periodic CFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function will stop a single/periodic CFS

===========================================================================*/

void stop_cf_srch()
{
  MSG_MED( "Aborting CFS. Stop the pending CFS transaction.", 0, 0, 0 );
  mcc_srch_buf.stop_cfs.hdr.cmd = SRCH_STOP_CFS_F;
  mcc_srch_buf.stop_cfs.cfs_seq = cand_freq_srch_parm.cur_srch_seq_num;
  mcc_srch_cmd( &mcc_srch_buf );

  mctrans_void_trans(MCTRANS_CFS_NSET);
  mctrans_void_trans(MCTRANS_CFS_CTRL);
  mctrans_void_trans(MCTRANS_CFS_GPS_PREP);

  cand_freq_srch_parm.cur_srch_type = SRCH_NO_CFS;

} /* stop_cf_srch */

/*===========================================================================

FUNCTION SEND_CAND_FREQ_SRCH_RESP_MSG

DESCRIPTION
  This function sends a Candidate Frequency Search Response Message
  (ack required)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_cand_freq_srch_resp_msg
(
  byte last_cfsrm_seq
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
    /* report to DIAG that a significant event is about to take place */
    diag_inc_event_cnt();

    /* ------------------------------------------
    ** Fill in Candidate Frequency Report Message
    ** ------------------------------------------ */
    cmd_ptr->tc_msg.msg.cfs_rsp.msg_type = CAI_CFS_RSP_MSG;
    cmd_ptr->tc_msg.msg.cfs_rsp.last_cfsrm_seq = last_cfsrm_seq;

    cmd_ptr->tc_msg.msg.cfs_rsp.tot_off_tm_fwd =
      cand_freq_srch_parm.off_times.tot_off_time_fwd;
    cmd_ptr->tc_msg.msg.cfs_rsp.tot_off_tm_rev =
      cand_freq_srch_parm.off_times.tot_off_time_rev;
    cmd_ptr->tc_msg.msg.cfs_rsp.max_off_tm_fwd =
      cand_freq_srch_parm.off_times.max_off_time_fwd;
    cmd_ptr->tc_msg.msg.cfs_rsp.max_off_tm_rev =
      cand_freq_srch_parm.off_times.max_off_time_rev;

    if (P_REV_IS_6_OR_GREATER)
    {
      /* Units are in frames */
      cmd_ptr->tc_msg.msg.cfs_rsp.pcg_off_times = 0;
      cmd_ptr->tc_msg.msg.cfs_rsp.align_timing_used = 0;

    }
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    /* Send message to RXTX to be transmitted to the base station */
    rxtx_cmd( cmd_ptr );
  }

} /* send_cand_freq_srch_resp_msg */

/*===========================================================================

FUNCTION SEND_CAND_FREQ_SRCH_RPT_MSG

DESCRIPTION
  This function sends a Candidate Frequency Search Report Message
  (ack required)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_cand_freq_srch_rpt_msg( void )
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte i;
    /* Counter variable */
  int sf_rx_pwr=0;
    /* Variable to hold the SF received power */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
    /* report to DIAG that a significant event is about to take place */
    diag_inc_event_cnt();

    /* ------------------------------------------
    ** Fill in Candidate Frequency Report Message
    ** ------------------------------------------ */
    cmd_ptr->tc_msg.msg.cfs_rpt.fix.msg_type = CAI_CFS_RPT_MSG;
    cmd_ptr->tc_msg.msg.cfs_rpt.fix.last_srch_msg =
      ( mcctcho_ho_data.hard_handoff_in_progress ) ? 1 : 0;
    cmd_ptr->tc_msg.msg.cfs_rpt.fix.last_srch_seq =
      cand_freq_srch_parm.last_srch_msg_seq;
    cmd_ptr->tc_msg.msg.cfs_rpt.fix.srch_mode = 0;

    cmd_ptr->tc_msg.msg.cfs_rpt.var.mod0.band_class =
      mcctcho_ho_data.tf_band_class;
    cmd_ptr->tc_msg.msg.cfs_rpt.var.mod0.cdma_freq =
      mcctcho_ho_data.tf_cdma_freq;

    sf_rx_pwr = (int) srch_agc_to_dbm( srch_get_rx_agc() );

    cmd_ptr->tc_msg.msg.cfs_rpt.var.mod0.sf_tot_rx_pwr =
      (byte) MIN( MAX_VAL_FOR_SF_RX_PWR, ( ( sf_rx_pwr + 111 ) / 2 ) );

    cmd_ptr->tc_msg.msg.cfs_rpt.var.mod0.cf_tot_rx_pwr =
      (uint8)mcctcho_ho_data.tf_tot_rx_pwr;
    cmd_ptr->tc_msg.msg.cfs_rpt.var.mod0.num_pilots =
      (uint8)mcctcho_ho_data.cf_srch_results.num_pilots;
    for ( i = 0; i < mcctcho_ho_data.cf_srch_results.num_pilots; i++ )
    {
      cmd_ptr->tc_msg.msg.cfs_rpt.var.mod0.pilot[i].plt_pn_phase =
        mcctcho_ho_data.cf_srch_results.pilot[i].pilot_pn;
      cmd_ptr->tc_msg.msg.cfs_rpt.var.mod0.pilot[i].plt_str =
        (uint8)mcctcho_ho_data.cf_srch_results.pilot[i].ecio;

      #ifdef FEATURE_IS2000_REL_A
      if (P_REV_IS_7_OR_GREATER)
      {
        cmd_ptr->tc_msg.msg.cfs_rpt.var.mod0.pilot2[i].pilot_rec_incl = 
          populate_pil_rec_info_in_rtc_msg(&mcctcho_ho_data.cf_srch_results.pilot[i].pilot_rec_type,
                                        &mcctcho_ho_data.cf_srch_results.pilot[i].pilot_rec,
                                        &cmd_ptr->tc_msg.msg.cfs_rpt.var.mod0.pilot2[i].pilot_rec_type,
                                        &cmd_ptr->tc_msg.msg.cfs_rpt.var.mod0.pilot2[i].rec);
      }
      #endif /* FEATURE_IS2000_REL_A */

    }

    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    /* Send message to RXTX to be transmitted to the base station */
    rxtx_cmd( cmd_ptr );
  }

} /* send_cand_freq_srch_rpt_msg */

/*===========================================================================

FUNCTION CF_SRCH_SET_EMPTY

DESCRIPTION
  This function checks if the Candidate Frequency Neighbor Set is empty

DEPENDENCIES
  None

RETURN VALUE
  TRUE if empty; FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean cf_srch_set_empty( void )
{
  if ( cand_freq_srch_parm.num_pilots > 0 )
  {
    return( FALSE );
  }

  return( TRUE );

} /* cf_srch_set_empty */

/*===========================================================================

FUNCTION cf_srch_get_off_times

DESCRIPTION
  This function calls SRCH to get the total/maximum forward off times.
  Also, it calculates the total/maximum reverse off times based on the
  foward off times

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

/*lint -e818*/
void cf_srch_get_off_times
(
  caii_cfs_req_type *cfs_req,
  mcctcho_cand_freq_srch_off_time_type *off_times
)
{
  byte i;
  byte num_visit;
  byte num_pilots = 0;
  byte cf_max_win = 0;
  word max_off_time_fwd = 0;
  word tot_off_time_fwd = 0;
  word off_time_in_20ms_unit;

  /* Get the maximum neighbor search window size for the CF */
  for ( i = 0; i < cfs_req->var.mod0.num_pilot; i++ )
  {
    /* Examine only pilots that are meant to be searched */
    if ( cfs_req->var.mod0.pilot_update_rec[i].search_set )
    {
      num_pilots++;

      if ( ( cfs_req->var.mod0.cf_nghbr_srch_mode == SRCH_CF_WIN_ONLY ) ||
           ( cfs_req->var.mod0.cf_nghbr_srch_mode == SRCH_CF_PRI_AND_WIN ) )
      {
        if ( cfs_req->var.mod0.pilot_update_rec[i].srch_win_nghbr > cf_max_win )
        {
          cf_max_win = cfs_req->var.mod0.pilot_update_rec[i].srch_win_nghbr;
        }
      }
    }
  }

  if ( cf_max_win == 0 )
  {
    cf_max_win = cfs_req->var.mod0.cf_srch_win_n;
  }

  if ( num_pilots > 0 )
  {
    /* Get the total/maximum forward off times from SRCH */
    srch_cfs_off_time_fwd( cf_max_win, num_pilots,
                           &max_off_time_fwd, &tot_off_time_fwd );

    /* Get number of visits */
    num_visit = (uint8)(tot_off_time_fwd / max_off_time_fwd);

    /* Convert max_off_time_fwd to 20ms unit.  Round up if necessary */
    off_time_in_20ms_unit = ( max_off_time_fwd + 19 ) / 20;
    off_times->max_off_time_fwd = (uint8)off_time_in_20ms_unit;

    /* Convert tot_off_time_fwd to 20ms unit.  Round up if necessary */
    off_time_in_20ms_unit = ( tot_off_time_fwd + 19 ) / 20;
    off_times->tot_off_time_fwd = (uint8)off_time_in_20ms_unit;

    /* Estimate total/maximum reverse off times based on the forward off times */
    off_times->max_off_time_rev = off_times->max_off_time_fwd;

    /* After the mobile tunes back to the SF, it might need to wait another
     * 2 frames before restarting the transmitter if the interval between the
     * time that the mobile disables its transmitter and the time that it resumes
     * operation on the SF is equal to or greater than 12 frames
     */
    if ( off_times->max_off_time_rev >= cai_tmo.n2m )
    {
       off_times->max_off_time_rev += (uint8)cai_tmo.n3m;
    }
    off_times->tot_off_time_rev = (uint8)(off_times->max_off_time_rev * num_visit);

    off_times->max_off_time_fwd =
      MIN( MAX_VAL_FOR_OFF_TIME, off_times->max_off_time_fwd );
    off_times->tot_off_time_fwd =
      MIN( MAX_VAL_FOR_OFF_TIME, off_times->tot_off_time_fwd );
    off_times->max_off_time_rev =
      MIN( MAX_VAL_FOR_OFF_TIME, off_times->max_off_time_rev );
    off_times->tot_off_time_rev =
      MIN( MAX_VAL_FOR_OFF_TIME, off_times->tot_off_time_rev );
  }
  else
  {
    /* No pilot to search */
    off_times->max_off_time_fwd = 0;
    off_times->tot_off_time_fwd = 0;
    off_times->max_off_time_rev = 0;
    off_times->tot_off_time_rev = 0;
  }

  MSG_MED( "fwd off:tot=%d, max=%d", off_times->tot_off_time_fwd,
                                     off_times->max_off_time_fwd, 0 );
  MSG_MED( "rev off:tot=%d, max=%d", off_times->tot_off_time_rev,
                                     off_times->max_off_time_rev, 0 );

} /* cf_srch_get_off_times */
/*lint +e818*/

/*===========================================================================

FUNCTION cf_srch_period_too_short

DESCRIPTION
  This function checks if the Candidate Frequency Search Period is too short

DEPENDENCIES
  None

RETURN VALUE
  TRUE if too short; FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean cf_srch_period_too_short
(
  byte srch_period,
  word tot_off_time_fwd,
  word tot_off_time_rev
)
{
  /* Get max( fwd_time, rev_time ), where
   *
   * fwd_time is the total time (in ms) in which the mobile needs to suspend
   *          its current Foward Traffic Channel in order to search the
   *          Candidate Frequency Search Set
   * rev_time is the total time (in ms) in which the mobile needs to suspend
   *          its current Reverse Traffic Channel in order to search the
   *          Candidate Frequency Search Set
   */
  word off_time = ( tot_off_time_fwd > tot_off_time_rev ) ?
                    tot_off_time_fwd : tot_off_time_rev;

  MSG_MED( "Maximum total off time %d", off_time, 0, 0 );

  /* T71M is the maximum time for the mobile station to send a Candidate
   * Frequency Search Report Message after completing a search
   */
  if ( srch_period_value[ srch_period ] < ( ( off_time * 20 ) + CAI_T71M ) )
  {
    return TRUE;
  }

  return FALSE;

} /* cf_srch_period_too_short */

/*===========================================================================

FUNCTION cf_srch_current

DESCRIPTION
  This function is called whenever MC receives a SRCH_CFS_READY_R or
  SRCH_CF_NSET_R report from SRCH to determine if the reports are still
  valid for the current search.  This is to handle the case where MC
  might have aborted the current search right before receiving the reports

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the SRCH reports are current; FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean cf_srch_current
(
  byte cfs_seq
)
{
  /* Check for old CFS report.  Even though a new CFSReqM/CFSCtrlM would
   * have always aborted the current periodic search, a report could still
   * be sitting on MC queue; and if that's the case, we should always
   * ignore the report because the search is no longer current
   */
  if ( cfs_seq != cand_freq_srch_parm.cur_srch_seq_num )
  {
    /* This is probably an old request.  Throw away!! */
    MSG_HIGH("Mismatched CFS seq. Expect %d, got %d",  
              cand_freq_srch_parm.cur_srch_seq_num, 
              cfs_seq, 0);
    return FALSE;
  }

  /* Similar check as above.  A (E|G|U)HDM might have aborted the current
   * periodic search just when a SRCH report gets queued.  Therefore,
   * check for the current search type to make sure it's still a go
   */
  if ( cand_freq_srch_parm.cur_srch_type == SRCH_NO_CFS )
  {
    MSG_HIGH("Ignore CFS report from SRCH", 0, 0, 0);
    return FALSE;
  }

  return TRUE;

} /* cf_srch_current */

/*===========================================================================

FUNCTION VALIDATE_HANDOFF_MSG

DESCRIPTION
  This function validates the (E|G|U)HDM by checking for reject reasons
  defined by IS95 (6.6.6.2.5.1)

DEPENDENCIES
  None

RETURN VALUE
  0 if message is OK; CAI reject code otherwise

SIDE EFFECTS
  None

===========================================================================*/

uint8 validate_handoff_msg
(
  caii_rx_msg_type *msg_ptr,
    /* Pointer to message received from the base station */
  tc_rej_ord_parms_type *rej_parms
    /* Rejection parameters */
)
{
  caii_ho_msg_type *ho = &(msg_ptr->ho);
    /* Convenience pointer to handoff message. */
  byte i; /*Counter variable */

  #if defined(FEATURE_IS95B_MDR)
  mccscm_stat_enum_type mdr_reject_code;
  #endif // if defined(FEATURE_IS95B_MDR)

  mcc_fwd_chan_mask_type ho_fwd_chan_mask;
  mcc_rev_chan_mask_type ho_rev_chan_mask;
  byte ota_ext_chind = CAI_EXT_CHIND_UNSET;  /*lint !e641 */

  #if defined(FEATURE_IS2000_REL_C_DV) || defined(FEATURE_IS2000_DV_SIG_TEST)
#error code not present
  #endif /* FEATURE_IS2000_REL_C_DV || FEATURE_IS2000_DV_SIG_TEST */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Default to no rejection */
  rej_parms->rej_code = 0;

  if (((ho->msg_type == CAI_GEN_HO_DIR_MSG)
            || (ho->msg_type == CAI_UNIV_HO_DIR_MSG)
            #ifdef FEATURE_MEID_SUPPORT
            || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
            #endif
           ) 
        && (ho->parms_incl == 1)
        && (ho->extra.p_rev == 0)
      )
  {
    /* P_Rev = 0 is invalid */
    MSG_HIGH( "P_REV cannot be 0.", 0, 0, 0 );
    rej_parms->rej_code = CAI_REJ_FIELD;
    return (rej_parms->rej_code);
  }

  #ifdef FEATURE_MEID_SUPPORT
  /* --------------------------------------------------------------------------
  ** Do some error-checking on the MEID Universal Handoff Direction Message:
  ** the p-rev-in-use must be between 6 and 8, and the phone must have an MEID.  
  ** -------------------------------------------------------------------------- */
  if (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
  {
    if ( !P_REV_IS_6_OR_GREATER 
         #ifdef FEATURE_IS2000_REL_C
         || P_REV_IS_9_OR_GREATER
         #endif /* FEATURE_IS2000_REL_C */
       )
    {
      MSG_HIGH("Reject MEID UHDM, P_REV in use < 6 or > 8",0,0,0);
      rej_parms->rej_code = CAI_REJ_STRUCT;
      return(rej_parms->rej_code);
    }
    if (!mccap_is_meid_supported())
    {
      MSG_HIGH("Reject MEID UHDM, mobile has no MEID",0,0,0);
      rej_parms->rej_code = CAI_REJ_CAP;
      return(rej_parms->rej_code);
    }
  }
  #endif /* FEATURE_MEID_SUPPORT */

  /* Validate the received CH_IND values received in UHDM */
  if (P_REV_IS_6_OR_GREATER)
  {
    if ( ( ho->msg_type == CAI_UNIV_HO_DIR_MSG )
         #ifdef FEATURE_MEID_SUPPORT
         || ( ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG )
         #endif /* FEATURE_MEID_SUPPORT */
       )
    {
      /*lint -e641 -e655*/
      if  ((ho->ch_ind == CAI_CHIND_FCH_ONLY) ||   
                         /* This value reserved */
           (ho->ch_ind == (CAI_CHIND_FCH_ONLY | CAI_CHIND_DCCH_ONLY)) || 
                         /* This value reserved */
           (ho->ch_ind == CAI_CHIND_REV_PILOT)  ||  
                         /* This value reserved */
           ((ho->ch_ind & CAI_CHIND_DCCH_ONLY) && !mccap_item_is_supported(MCCAP_DF_DCCH)) ||  
                         /* Error if DCCH specified but not supported */
           ((ho->ch_ind == CAI_CHIND_RESERVED_1) 
           #ifdef FEATURE_IS2000_REL_C
           && (!P_REV_IS_9_OR_GREATER || !mccap_is_for_pdch_supported())
              /* Zero value only supported for DV */
           #endif /* FEATURE_IS2000_REL_C */
      ))
      /*lint +e641 +e655*/
      {
        MSG_HIGH( "Unsupported chind in UHDM: %d", ho->ch_ind, 0, 0 );
        rej_parms->rej_code = CAI_REJ_CAP;
        return (rej_parms->rej_code);
      }
    } /* ho->msg_type == UHDM */
  } /* P_REV is 6 or greater */

  #if defined(FEATURE_IS2000_REL_C_DV) || defined(FEATURE_IS2000_DV_SIG_TEST)
#error code not present
  #endif /* FEATURE_IS2000_REL_C_DV || FEATURE_IS2000_DV_SIG_TEST */

  /* Calculate the proposed channel masks up front */
  ho_fwd_chan_mask = mccccl_code_channel_list.gen_fwd_chan_mask((cai_chind_type)ho->ch_ind, 
                                            (cai_ext_chind_type)ota_ext_chind);
  ho_rev_chan_mask = mccccl_code_channel_list.gen_rev_chan_mask((cai_chind_type)ho->ch_ind, 
                                            (cai_ext_chind_type)ota_ext_chind);

  /* check to see if we should reject the message */


  if (((ho->msg_type == CAI_UNIV_HO_DIR_MSG) 
       #ifdef FEATURE_MEID_SUPPORT
       || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
       #endif /* FEATURE_MEID_SUPPORT */
      ) && (ho->num_pilots == 0))
  {
    /* **********************************************************************  */
    /* Send a Reject Order if num_pilots is 0 in UHDM.                         */
    /* This checking is already being done in the parser for HDM, EHDM and     */
    /* GHDM.  Check here for UHDM because we can be more specific for our      */
    /* reject code.                                                            */
    /* **********************************************************************  */ 
    MSG_HIGH("Num_pilots in UHDM cannot be zero",0,0,0);
    rej_parms->rej_code = CAI_REJ_FIELD;
    return(rej_parms->rej_code);
  }

  if (ho->extra_parms)
  {
    /* --------------------------------------------------------------------
    ** If the Handoff Message is requesting that the mobile use the private
    ** long code mask, verify that the user wants it and that the mobile
    ** is capable of it.  If not, reject the Handoff Message.
    ** --------------------------------------------------------------------- */
    if (ho->extra.private_lcm)
    {
      #ifdef FEATURE_AUTH
      if (!cdma.vp_desired || !auth_private_lcm_available()
          #ifdef FEATURE_OTASP
          || otasp_reauth_in_progress
          #endif /* FEATURE_OTASP */
         )
      #endif /* FEATURE_AUTH */
      {
        MSG_HIGH("Private long code mask not supported/desired.",0,0,0);
        rej_parms->rej_code = CAI_REJ_CAP;
        return(rej_parms->rej_code);
      }
    }

    if ((ho->msg_type != CAI_HO_DIR_MSG) &&
        (!mcc_is_band_class_supported(ho->extra.band_class)
        ))
    {
      /* HDM does not contain a BAND_CLASS field.  Send Mobile station
         reject order if handoff to unsupported band.  We do not
         presently support CDMA handoff to a different band class. */

      MSG_HIGH( "Unsupported band class action.", 0, 0, 0 );
      rej_parms->rej_code = CAI_REJ_CAP;
      return(rej_parms->rej_code);
    }
    else if (((ho->msg_type == CAI_GEN_HO_DIR_MSG) || (ho->msg_type == CAI_UNIV_HO_DIR_MSG)
              #ifdef FEATURE_MEID_SUPPORT
              || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
              #endif
             ) && ho->extra.scr_included &&
             snm_eval_config
             (
               &ho->extra.cfg, NULL, FALSE, FALSE,
               ho_fwd_chan_mask,
               ho_rev_chan_mask
             ) != SNM_ACCEPT_CFG 
            )
    {
      /* Cannot support current service configuration. */
      MSG_HIGH( "Unsupported service config", 0, 0, 0 );
      rej_parms->rej_code = CAI_REJ_CFG;
      return(rej_parms->rej_code);
    }
    else if (((ho->msg_type == CAI_GEN_HO_DIR_MSG)
              || (ho->msg_type == CAI_UNIV_HO_DIR_MSG)
              #ifdef FEATURE_MEID_SUPPORT
              || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
              #endif
             ) && ho->extra.scr_included &&
             (cdma.serv_neg == CAI_SERV_OPT_NEG))
    {
      /* Cannot do service connection when using service option neg. */
      MSG_HIGH( "Service connect not allowed with SO neg.", 0, 0, 0 );
      rej_parms->rej_code = CAI_REJ_STATE;
      return (rej_parms->rej_code);
    }
    else if ((ho->msg_type == CAI_GEN_HO_DIR_MSG)
             || (ho->msg_type == CAI_UNIV_HO_DIR_MSG)
             #ifdef FEATURE_MEID_SUPPORT
             || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
             #endif
            )
    {
      if (ho->extra.periodic_srch && cf_srch_set_empty())
      {
        /* Requested periodic search with no candidate freq search set */
        MSG_HIGH( "GHDM srch set empty", 0, 0, 0 );
        rej_parms->rej_code = CAI_REJ_SRCH;
        return (rej_parms->rej_code);
      }
      else if ( ( ho->extra.periodic_srch ) &&
                ( cf_srch_period_too_short(
                    cand_freq_srch_parm.srch_period,
                    cand_freq_srch_parm.off_times.tot_off_time_fwd,
                    cand_freq_srch_parm.off_times.tot_off_time_rev ) ) )
      {
        /* Specified search period is too short to complete search */
        MSG_HIGH( "GHDM srch period too short", 0, 0, 0 );
        rej_parms->rej_code = CAI_REJ_TSHT;
        return (rej_parms->rej_code);
      }
    }

    /* when AES feature is defined, the field of encrypt_mode is validated according
     * to AES process. So we need skip the following block when AES feature is
     * defined. In fact, if AES feature is defined, the encrypt_mode could be 
     * CAI_EXTENDED_ENCRYPT_CALL_CTL_MSG, which is greater than CAI_ENCRYPT_CALL_CTL_MSGS
     */
    #if defined(FEATURE_IS2000_REL_A) && ( ! defined(FEATURE_IS2000_REL_A_AES) )
    if (ho->extra.encrypt_mode > CAI_ENCRYPT_CALL_CTL_MSGS)
    {
        /* Cannot support encypt mode. */
        MSG_HIGH( "Unsupported  encrypt mode %d", ho->extra.encrypt_mode, 0, 0 );
        rej_parms->rej_code = CAI_REJ_CFG;
        return (rej_parms->rej_code);
    }
    #endif /* FEATURE_IS2000_REL_A && ! FEATURE_IS2000_REL_A_AES */

  } // extra_parms

  /* Validate nnscr:
  Here, there is a need to evaluate the NNSCR even in scenarios when there may
  not be a SCR or NNSCR or both in a HO msg. Because a ch_ind change can cause
  scenarios that the MS can't handle even if underlying SCR/NNSCR don't change. So,
  whenever our PREV is >= 6, we'll evaluate the NNSCR (LPM fields within it
  specifically) to find out if we should reject the msg */
  if (P_REV_IS_6_OR_GREATER)
  {
    if (!(ho->extra_parms && ho->extra.nnscr_included))
    {
      /* nnscr not in msg, need to evaluate the current nnscr */
      ho->extra.non_neg_cfg = *(snm_get_current_nnscr());
    }

    if (!(ho->extra_parms && ho->extra.scr_included))
    {
      /* scr not in msg, use current scr while evaluating nnscr */
      ho->extra.cfg = *(snm_get_current_config());
    }

    snm_update_lpm_table(&ho->extra.non_neg_cfg, 
                         &ho->extra.non_neg_cfg,
                         &ho->extra.cfg,
                         ho_fwd_chan_mask,
                         ho_rev_chan_mask);

    if (snm_eval_non_neg_scr(&ho->extra.non_neg_cfg,
                              &ho->extra.cfg,
                              ho_fwd_chan_mask, 
                              ho_rev_chan_mask
                             ) != SNM_SUCCESS)
    {
      /* Cannot support NNSCR. This NNSCR may be stored or one that
      came in the message */
      MSG_HIGH( "NNSCR - stored or given - not acceptable", 0, 0, 0 );
      rej_parms->rej_code = CAI_REJ_CFG;
      return (rej_parms->rej_code);      
    }
  } /* P_REV_IS_6_OR_GREATER */

  if (P_REV_IS_6_OR_GREATER)
  {
    if ((ho->msg_type == CAI_UNIV_HO_DIR_MSG)
        #ifdef FEATURE_MEID_SUPPORT
        || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
        #endif
       )
    {
      #if defined(FEATURE_IS2000_SCH) || defined(FEATURE_IS2000_P2)
      const caii_srv_cfg_type  *config = snm_get_current_config();
      #endif /* defined(FEATURE_IS2000_P2) || defined(FEATURE_IS2000_SCH) */

      /* Check reverse only since if any DCCH, then R-DCCH and if any FCH
      ** R-FCH is present. */
      /*lint -e655*/
      if (((ho_rev_chan_mask & MCC_R_FCH) && (ho_rev_chan_mask & MCC_R_DCCH)) 
           && !mccap_is_simult_fch_dcch_supported())
      /*lint +e655*/
      {
        MSG_HIGH( "Unsupported chind %d", ho->ch_ind, 0, 0 );
        rej_parms->rej_code = CAI_REJ_CAP;
        return (rej_parms->rej_code);
      }

      #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
      #endif

      if (ho->sch_incl)
      {
        #ifdef FEATURE_IS2000_SCH
        uint16 for_sch_mux, rev_sch_mux;

        #ifdef FEATURE_IS2000_REL_A
        uint8 for_sch_max_rate = 0;
        uint8 rev_sch_max_rate = 0;
        #endif

        if ( ho->num_for_assign > mccap_num_f_sch_supported() )
        {
          MSG_HIGH( "Too many F-SCH assigned", 0, 0, 0 );
          rej_parms->rej_code = CAI_REJ_CAP;
          return (rej_parms->rej_code);
        }
        if ( ho->num_for_assign > 0 && ho->for_assign[0].for_sch_id == 1 )
        {
          MSG_HIGH( "F-SCH 1 not supported", 0, 0, 0 );
          rej_parms->rej_code = CAI_REJ_CAP;
          return (rej_parms->rej_code);
        }
        if ( ho->num_rev_assign > mccap_num_r_sch_supported() )
        {
          MSG_HIGH( "Too many R-SCH assigned", 0, 0, 0 );
          rej_parms->rej_code = CAI_REJ_CAP;
          return (rej_parms->rej_code);
        }
        if ( ho->num_rev_assign > 0 && ho->rev_assign[0].rev_sch_id == 1 )
        {
          MSG_HIGH( "R-SCH 1 not supported", 0, 0, 0 );
          rej_parms->rej_code = CAI_REJ_CAP;
          return (rej_parms->rej_code);
        }

        config = snm_get_current_config();

        if ( ho->num_for_assign > 0 )
        {
          const mccccl_sccl_entry_type* sccl_entry;

          //  If sch cc is not included but we are assigned SCH, then it's an
          // error.  Since we don't support SCH 1, we don't check for the SCH ID here.
          if ( (!config->for_sch_cc_incl || config->num_for_sch == 0) &&
               ( !ho->extra.scr_included ||
                 (!ho->extra.cfg.for_sch_cc_incl||ho->extra.cfg.num_for_sch==0)
               ))
          {
              MSG_HIGH( "(U/G)HDM conflicts with fsch config!", 0, 0, 0 );
              rej_parms->rej_code = CAI_REJ_CFG;
              return (rej_parms->rej_code);
          }

          // Now check to see if the SCCL entry is valid.  If it has never been downloaded before,
          // then num_pilots will be 0.
          // Note: SCH 0 assumed here
          sccl_entry = mccccl_code_channel_list.get_sccl_entry( 0, ho->for_assign[0].sccl_index );
          if ( sccl_entry->num_pilots == 0 )
          {
            // SCCL was not downloaded earlier, now we need to check if it is included in this message
            for ( i = 0; i < ho->sch_cfg.num_for_sch; ++i )
            {
              if ( ho->sch_cfg.for_sch_cfg[i].sccl_index == ho->for_assign[0].sccl_index )
              {
                // The SCCL entry is included in the message, we are fine.
                break;
              }
            }

            // SCCL is not found in message, this is an error
            if ( i == ho->sch_cfg.num_for_sch )
            {
              MSG_HIGH( "SCCL entry %d not valid!", ho->for_assign[0].sccl_index, 0, 0 );
              rej_parms->rej_code = CAI_REJ_CFG;
              return (rej_parms->rej_code);
            }
          }
        }

        if ( ho->num_rev_assign > 0 )
        {
          if ( (!config->rev_sch_cc_incl  || config->num_rev_sch == 0 )&&
               ( !ho->extra.scr_included ||
                 (!ho->extra.cfg.rev_sch_cc_incl||ho->extra.cfg.num_rev_sch==0)
               ))
          {
              MSG_HIGH( "(U/G)HDM conflicts with rsch config!", 0, 0, 0 );
              rej_parms->rej_code = CAI_REJ_CFG;
              return (rej_parms->rej_code);
          }
        }

        // Validate the rate based on mux option negotiated.  We know that rev_sch_cc must be
        // included if we are here.
        // Note: This is not specified in the standard, but we should do it anyway.
        // We support only SCH 0, so make that assumption here.
        // First figure out what for_sch_mux and rev_sch_mux will be after the handoff
        for_sch_mux = ( ho->extra.scr_included && ho->extra.cfg.for_sch_cc_incl &&
                        ho->extra.cfg.num_for_sch ) ?
                        ho->extra.cfg.for_sch[0].for_sch_mux : config->for_sch[0].for_sch_mux;
        rev_sch_mux = ( ho->extra.scr_included && ho->extra.cfg.rev_sch_cc_incl &&
                        ho->extra.cfg.num_rev_sch ) ?
                        ho->extra.cfg.rev_sch[0].rev_sch_mux : config->rev_sch[0].rev_sch_mux;

        #ifdef FEATURE_IS2000_REL_A
        // Validate the rate based on max rate negotiated.  We know that rev_sch_cc must be
        // included if we are here.
        // Note: This is not specified in the standard, but we should do it anyway.
        // We support only SCH 0, so make that assumption here.
        // First figure out what for_sch_max_rate and rev_sch_max_rate will be 
        // after the handoff
        if (P_REV_IS_7_OR_GREATER)
        {
          for_sch_max_rate 
             = ( ho->extra.scr_included && ho->extra.cfg.for_sch_cc_incl &&
                            ho->extra.cfg.num_for_sch ) ?
                            ho->extra.cfg.for_sch[0].sch_chn_cfg.max_rate : 
                            config->for_sch[0].sch_chn_cfg.max_rate;
          rev_sch_max_rate 
             = ( ho->extra.scr_included && ho->extra.cfg.rev_sch_cc_incl &&
                            ho->extra.cfg.num_rev_sch ) ?
                            ho->extra.cfg.rev_sch[0].sch_chn_cfg.max_rate : 
                            config->rev_sch[0].sch_chn_cfg.max_rate;
        }
        #endif

        for ( i = 0; i < ho->num_rev_assign; i++ )
        {

          #ifdef FEATURE_IS2000_REL_A 
          // This checking is simple in P_REV 7. In the service config record of
          // release A, there is MAX_RATE field, which is the maximun data rate 
          // that both BS and MS agreed upon. The rate came in UHDM should not 
          // be greater than MAX_RATE. 
          if ( P_REV_IS_7_OR_GREATER )  
          {
            if ( (cai_sch_rate_type) ho->rev_assign[i].rev_sch_rate > 
                  (cai_sch_rate_type)rev_sch_max_rate)
            {
              MSG_HIGH( "Rate %d is greater than MAX_RATE %d", ho->rev_assign[i].rev_sch_rate,
                      config->rev_sch[0].sch_chn_cfg.max_rate, 0 );
              rej_parms->rej_code = CAI_REJ_FIELD;
              return (rej_parms->rej_code);
            }
          }
          else
          #endif //FEATURE_IS2000_REL_A 

          {
            if ( !mccsch_verify_rate( rev_sch_mux, (cai_sch_rate_type) ho->rev_assign[i].rev_sch_rate ) )
            {
              MSG_HIGH( "Rate %d not allowed by rev_sch_mux 0x%x", ho->rev_assign[i].rev_sch_rate,
                      rev_sch_mux, 0 );
              rej_parms->rej_code = CAI_REJ_FIELD;
              return (rej_parms->rej_code);
            }
          }
        }

        // Validate the rate based on mux option negotiated.  We know that rev_sch_cc must be
        // included if we are here.
        // Note: This is not specified in the standard, but we should do it anyway.
        // We support only SCH 0, so make that assumption here.
        for ( i = 0; i < ho->sch_cfg.num_for_sch; i++ )
        {

          #ifdef FEATURE_IS2000_REL_A //32X
          // This checking is simple in release A. In the service config record of
          // release A, there is MAX_RATE field, which is the maximun data rate 
          // that both BS and MS agreed upon. The rate came in UHDM should not 
          // be greater than MAX_RATE. If P_REV_IN_USE is 6 or below, max_rate is
          // set to default valud based on the mux option.
          if ( P_REV_IS_7_OR_GREATER)
          {
            if ((cai_sch_rate_type) ho->sch_cfg.for_sch_cfg[i].for_sch_rate > 
                 (cai_sch_rate_type)for_sch_max_rate)
            {
              MSG_HIGH( "Rate %d is greater than MAX_RATE %d", ho->sch_cfg.for_sch_cfg[i].for_sch_rate,
                      config->for_sch[0].sch_chn_cfg.max_rate, 0 );
              rej_parms->rej_code = CAI_REJ_FIELD;
              return (rej_parms->rej_code);
            }
          }
          else
          #endif //FEATURE_IS2000_REL_A

          {
            if ( !mccsch_verify_rate( for_sch_mux, (cai_sch_rate_type)ho->sch_cfg.for_sch_cfg[i].for_sch_rate ) )
            {
              MSG_HIGH( "Rate %d not allowed by for_sch_mux 0x%x",
                      ho->sch_cfg.for_sch_cfg[i].for_sch_rate, for_sch_mux, 0 );

              rej_parms->rej_code = CAI_REJ_FIELD;
              return (rej_parms->rej_code);
            }
          }
        }

        for ( i = 0; i < ho->sch_cfg.num_rev_sch; i++ )
        {
          #ifdef FEATURE_IS2000_REL_A //32X
          // This checking is simple in release A. In the service config record of
          // release A, there is MAX_RATE field, which is the maximun data rate 
          // that both BS and MS agreed upon. The rate came in UHDM should not 
          // be greater than MAX_RATE. If P_REV_IN_USE is 6 or below, max_rate is
          // set to default valud based on the mux option.
          if ( P_REV_IS_7_OR_GREATER) 
          {
            if ((cai_sch_rate_type) ho->sch_cfg.rev_sch_cfg[i].rev_sch_rate > 
                 (cai_sch_rate_type)rev_sch_max_rate)
            {
              MSG_HIGH( "Rate %d is greater than MAX_RATE %d", ho->sch_cfg.rev_sch_cfg[i].rev_sch_rate,
                           config->rev_sch[0].sch_chn_cfg.max_rate, 0 );
              rej_parms->rej_code = CAI_REJ_FIELD;
              return (rej_parms->rej_code);
            }
          }
          else
          #endif //FEATURE_IS2000_REL_A //32X

          {
            if ( !mccsch_verify_rate( rev_sch_mux, (cai_sch_rate_type)ho->sch_cfg.rev_sch_cfg[i].rev_sch_rate ) )
            {
              MSG_HIGH( "Rate %d not allowed by rev_sch_mux 0x%x",
                        ho->sch_cfg.rev_sch_cfg[i].rev_sch_rate, rev_sch_mux, 0 );
              rej_parms->rej_code = CAI_REJ_FIELD;
              return (rej_parms->rej_code);
            }
          }
        }

        #ifdef FEATURE_IS2000_P2
        if ( !(ho_rev_chan_mask & MCC_R_PICH) && (ho->num_for_assign ||
              ho->num_rev_assign) )
        /*lint +e655*/
        {
          // Supplemental channels not allowed when pilot gating is on.
          MSG_HIGH( "Sup Asgn with bad CH_IND %x", ho->ch_ind, 0, 0 );
          rej_parms->rej_code = CAI_REJ_STRUCT;
          return (rej_parms->rej_code);
        }
        #endif // FEATURE_IS2000_P2

        #else
        MSG_HIGH( "Unsupported SCH assignment",0, 0, 0 );
        rej_parms->rej_code = CAI_REJ_CAP;
        return (rej_parms->rej_code);
        #endif //FEATURE_IS2000_SCH

      }
      #ifdef FEATURE_IS2000_P2 
      /*lint -e655*/
      if (!(ho_rev_chan_mask & MCC_R_PICH)) 
      /*lint +e655*/
      /* in control hold mode, we must have pilot gating rate either in stored
         nnscr, or in the current UHDM */
      {
        #ifdef FEATURE_IS2000_CHS
        if (mccap_item_is_supported(MCCAP_DF_CHS)) 
        {
          const caii_non_neg_srv_cfg_type *nnscr = snm_get_current_nnscr();
          byte fpc_mode;
  
          if ( !nnscr->gating_rate_incl &&
              (!ho->extra.nnscr_included || !ho->extra.non_neg_cfg.gating_rate_incl) )
          {
            MSG_HIGH("MS cannot go into control hold without pilot gating rate",0,0,0);
            rej_parms->rej_code = CAI_REJ_FIELD;
            return (rej_parms->rej_code);
          }
          
          if (ho->extra.nnscr_included && ho->extra.non_neg_cfg.fpc_incl)
          {
            fpc_mode =  ho->extra.non_neg_cfg.fpc_mode;
          }
          else
          {
            fpc_mode = nnscr->fpc_mode;
          }
          if (    (fpc_mode != CAI_FPC_MODE_800_PRI)  /*lint !e641 */
               && (fpc_mode != CAI_FPC_MODE_50_PRI)  /*lint !e641 */
               #ifdef FEATURE_IS2000_REL_A
               && (fpc_mode != CAI_FPC_MODE_QIB_50_PRI)  /*lint !e641 */
               #endif
             )
          {
            MSG_HIGH("fpc_mode %d, MS cannot go into CH" ,fpc_mode,0,0);
            rej_parms->rej_code = CAI_REJ_FIELD;
            return (rej_parms->rej_code);
          }
        }
        else
        #endif // FEATURE_IS2000_CHS
        {
          MSG_HIGH( "Control Hold Unsupported",0, 0, 0 );
          rej_parms->rej_code = CAI_REJ_CAP;
          return (rej_parms->rej_code);
        }
      }

      // If :
      // 1. FCH CC is not included in the message and current FCH RC < 3, or
      // 2. FCH CC is included in the message and RC < 3, or
      // 3. DCCH CC is not include in the message and current config does not contain DCCH, AND
      // 4. CH_IND indicates DCCH is included,
      // we need to reject.  Note: conflict between RC and DCCH CC info (which
      // is totally inside the SCR) is identified in SNM.
      /* Arbitrarily using R-DCCH (instead of F-DCCH) this check */
      /*lint -e641 -e655*/
      if ( ((!ho->extra.scr_included || !ho->extra.cfg.fch_cc_incl)
              && config->fch_cc_incl && config->for_fch_rc < CAI_RC_3 ||  
            (ho->extra.scr_included && ho->extra.cfg.fch_cc_incl
              && ho->extra.cfg.for_fch_rc < CAI_RC_3) || 
            (!config->dcch_cc_incl && (!ho->extra.scr_included || !ho->extra.cfg.dcch_cc_incl)))
            &&
            (ho_rev_chan_mask & MCC_R_DCCH) ) 
      /*lint +e641 +e655*/
      {
        MSG_HIGH( "Bad CH_IND %x for low RCs", ho->ch_ind, 0, 0 );
        rej_parms->rej_code = CAI_REJ_FIELD;
        return (rej_parms->rej_code);
      }

      // If FPC_PRI_CHAN is include, verify if it conflicts with CH_IND
      if ( ho->extra.nnscr_included && ho->extra.non_neg_cfg.fpc_incl )
      {
        if ( ((1 << ho->extra.non_neg_cfg.fpc_pri_chan) & ho_fwd_chan_mask) == 0 )  /*lint !e641 */
        {
          // Bit corresponding to FPC_PRI_CHAN is not assigned
          MSG_HIGH( "FPC_PRI_CHAN %d not active, fwd_chans=%d",
                    ho->extra.non_neg_cfg.fpc_pri_chan, ho_fwd_chan_mask, 0 );
          rej_parms->rej_code = CAI_REJ_FIELD;
          return (rej_parms->rej_code);
        }
      }
      #endif // FEATURE_IS2000_P2
      
      for (i = 0; i < ho->num_pilots; i++)
      {
        /* Make sure pilot rec type is good */
        if (ho->pilot[i].add_pilot_rec_incl)
        {
          if ( !validate_pilot_rec_type( (cai_pilot_rec_type)ho->pilot[i].pilot_rec_type,
                                        &ho->pilot[i].add_pilot_rec))
          {
            MSG_HIGH( "Special pilot included in UHDM", 0, 0, 0 );
            rej_parms->rej_code = CAI_REJ_CAP;
            return (rej_parms->rej_code);
          }
        }
      }
    }
  }

  #ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    if ((ho->msg_type == CAI_UNIV_HO_DIR_MSG)
        #ifdef FEATURE_MEID_SUPPORT
        || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
        #endif
       )
    {
      if (ho->extra_parms && ho->extra.scr_included && ho->cc.cc_info_incl)
      {
        if (!mccap_item_is_supported(MCCAP_DF_CS))
        {
          MSG_HIGH( "cc_info is included", 0, 0, 0 );
          rej_parms->rej_code = CAI_REJ_CAP;
          return (rej_parms->rej_code);
        }

        #ifdef FEATURE_IS2000_REL_A_SVD 
        else
        {
          /* We can support SVD */
          if( pre_process_cc_info( &ho->cc, rej_parms ) != 0 )      
          {
            MSG_HIGH("Call Assignment Rejected in UHDM", 0, 0, 0);
            return (rej_parms->rej_code);
          }

          /* If pre-process passes, CCI is created later */
        }
        #endif /* FEATURE_IS2000_REL_A_SVD */

      }
    }

    if ((ho->msg_type == CAI_UNIV_HO_DIR_MSG)
        #ifdef FEATURE_MEID_SUPPORT
        || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
        #endif
       )
    {
      // Arbitrarily use reverse channels to check for 3x info
      /*lint -e655*/
      if ( (((ho_rev_chan_mask & MCC_R_FCH) && ho->thx_and_ccsh_info.thx_fch_info_incl)
         ||((ho_rev_chan_mask & MCC_R_DCCH) && ho->thx_and_ccsh_info.thx_dcch_info_incl))
         && !mccap_get_thx_cch_supported()
         )
      /*lint +e655*/
      {
        MSG_HIGH( "3X info is included", 0, 0, 0 );
        rej_parms->rej_code = CAI_REJ_CAP;
        return (rej_parms->rej_code);
      }
    }
  }
  #endif

  #if defined(FEATURE_IS95B_MDR)
  if (ho->sup_chan_parms_inc &&
      ((mdr_reject_code = mccscm_mdr_pre_process(msg_ptr))
        != MDR_DONE_S))
  {
    MSG_HIGH( "HO msg: error in MDR fields", 0, 0, 0 );
    rej_parms->rej_code = 
      ( mdr_reject_code == MDR_REJ_CAP_S) ? CAI_REJ_CAP : CAI_REJ_STRUCT;
    
    return  rej_parms->rej_code;
  }
  #endif // if defined(FEATURE_IS95B_MDR)

  /* BS ignored MS reported capabilities.  If this optional IS-2000 feature 
  is not enabled, ignore a BS request to gate the reverse FCH, but do not 
  reject the handoff message.  Force field in internal HO req to FALSE */
  if ((P_REV_IS_6_OR_GREATER) && 
      ((ho->msg_type == CAI_UNIV_HO_DIR_MSG) || (ho->msg_type == CAI_GEN_HO_DIR_MSG)
       #ifdef FEATURE_MEID_SUPPORT
       || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
       #endif
      ) && (ho->rev_fch_gating_mode)
     )
  {
    if (!mccap_item_is_supported(MCCAP_DF_REV_FCH_GATING))
    {
      MSG_HIGH("R-FCH 1/8 gating not supported,ignore req in HO", 0, 0, 0);
      ho->rev_fch_gating_mode = FALSE;
    }
  }

  #if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_MEID_SUPPORT)
  if (
      #ifdef FEATURE_IS2000_REL_C
      (P_REV_IS_9_OR_GREATER && (ho->msg_type == CAI_UNIV_HO_DIR_MSG))
      #endif /* FEATURE_IS2000_REL_C */
 
      #if defined(FEATURE_IS2000_REL_C) && defined(FEATURE_MEID_SUPPORT)
      ||
      #endif /* FEATURE_IS2000_REL_C && FEATURE_MEID_SUPPORT */

      #ifdef FEATURE_MEID_SUPPORT
      (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
      #endif /* FEATURE_MEID_SUPPORT */
     )
  {
    if (ho->plcm_type_incl)
    {
      /* --------------------------------------------------------------------
      ** An MEID UHDM is rejected if the mobile does not have an MEID; if
      ** we got here, the mobile has an MEID, and all plcm_types are allowed.
      **
      ** A p-rev 9 UHDM can always set the plcm_type to ESN-based or 
      ** bs-specified.  If the MEID feature is defined, and the mobile has an
      ** MEID, it can also set the plcm_type to IMSI_M, IMSI_T, and MEID.
      ** -------------------------------------------------------------------- */
      switch (ho->plcm_type)
      {
        case CAI_PLCM_TYPE_ESN:
        case CAI_PLCM_TYPE_BS_SPEC:
          break;

        #ifdef FEATURE_MEID_SUPPORT
        case CAI_PLCM_TYPE_IMSI_O_M:
          if (cdma.imsi_type == CDMA_IMSI_T)
          {
            MSG_HIGH("PLCM_TYPE IMSI_M rcvd in UHDM while using IMSI_T",0,0,0);
            rej_parms->rej_code = CAI_REJ_PLCM_TYPE_MISMATCH;
            return(rej_parms->rej_code);
          }
          else if (!mccap_is_meid_supported())
          {
            MSG_HIGH("Mobile has no MEID, cannot use plcm_type: %d",ho->plcm_type,0,0);
            rej_parms->rej_code = CAI_REJ_CAP;
            return(rej_parms->rej_code);
          }
          break;

        case CAI_PLCM_TYPE_IMSI_O_T:
          if (cdma.imsi_type == CDMA_IMSI_M)
          {
            MSG_HIGH("PLCM_TYPE IMSI_T rcvd in UHDM while using IMSI_M",0,0,0);
            rej_parms->rej_code = CAI_REJ_PLCM_TYPE_MISMATCH;
            return(rej_parms->rej_code);
          }
          else if (!mccap_is_meid_supported())
          {
            MSG_HIGH("Mobile has no MEID, cannot use plcm_type: %d",ho->plcm_type,0,0);
            rej_parms->rej_code = CAI_REJ_CAP;
            return(rej_parms->rej_code);
          }
          break;

        case CAI_PLCM_TYPE_MEID:
          if (!mccap_is_meid_supported())
          {
            MSG_HIGH("Mobile has no MEID, cannot use plcm_type: %d",ho->plcm_type,0,0);
            rej_parms->rej_code = CAI_REJ_CAP;
            return(rej_parms->rej_code);
          }
          break;
        #endif /* FEATURE_MEID_SUPPORT */

        default:
          MSG_HIGH("Unknown plcm_type: %d in UHDM",ho->plcm_type,0,0);
          rej_parms->rej_code = CAI_REJ_FIELD;
          return(rej_parms->rej_code);
      } /* switch on plcm_type */

    } /* if plcm_type_incl is 1 */
  } /* end of UHDM/MEID UHDM error-checking */
  #endif /* FEATURE_IS2000_REL_C || FEATURE_MEID_SUPPORT */
  
  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  return 0;   // Successful return
 
} /* validate_handoff_msg */

/*===========================================================================

FUNCTION PRE_PROCESS_HANDOFF_MSG

DESCRIPTION
  This function pre-processes the (E|G|U)HDM by filling in default values for
  fields that is present in one kind of HDM but not the other kind; and
  saves all the values that is needed when post-processing the handoff, i.e.
  when the (U|G|E)HDM is no longer around.  Also, it converts implicit action
  time to explicit for frame offset handoff

DEPENDENCIES
  None

RETURN VALUE
  0 if successfully processed, CAI reject code otherwise

SIDE EFFECTS
  None

===========================================================================*/

void pre_process_handoff_msg
(
  mccrxtx_msg_type *msg_ptr
    /* Pointer to message received from the base station */
)
{
  caii_ho_msg_type *ho = &msg_ptr->msg.ho;
    /* Convenience pointer to handoff message. */
  mccdma_pilot_info_type pilot_rec[CAI_N6M];
   /* Active set pilot record */
  byte i, j, num_active_pilots;
  qword temp_time;

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

  /* --------------------------------------------------------------------------
  ** Check if the base station has directed the mobile station to transition
  ** between disjoint sets of base stations, different frequency assignments,
  ** or different frame offsets.  In other words, a hard handoff
  ** ----------------------------------------------------------------------- */

  /* Check for disjoint active sets handoff */
  num_active_pilots = mccccl_code_channel_list.get_pilots( pilot_rec );

  mcctcho_ho_data.disjoint_active_sets_ho = TRUE;
  for ( i = 0;
        ( ( i < ho->num_pilots ) && mcctcho_ho_data.disjoint_active_sets_ho && (i < CAI_HO_MSG_MAX_PILOTS));
        i++ )
  {
    for ( j = 0; j < num_active_pilots; j++ )
    {
      if ( ho->pilot[i].pilot_pn == pilot_rec[j].pilot_pn )
      {
        mcctcho_ho_data.disjoint_active_sets_ho = FALSE;
        break;
      }
    }
  }
  
#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

  mcctcho_ho_data.different_freq_ho = FALSE;
  mcctcho_ho_data.different_frame_offset_ho = FALSE;
  mcctcho_ho_data.different_band_ho = FALSE;

  mcctcho_ho_data.frame_offset = cdma.frame_offset;
  
  if (!ho->use_time)
  {
    ho->action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
  }

  if ( ho->extra_parms )
  {
    /* Check for different frequencies handoff */
    if ( ho->extra.cdma_freq != cdma.cdmach )
    {
      mcctcho_ho_data.different_freq_ho = TRUE;
    }

    /* Check for cross band handoff */
    if ( ho->extra.band_class != cdma.band_class )
    {
      mcctcho_ho_data.different_band_ho = TRUE;
      mcctcho_ho_data.different_freq_ho = TRUE;
    }

    /* Check for different frame offsets handoff */
    if ( ho->extra.frame_offset != cdma.frame_offset )
    {
      mcctcho_ho_data.different_frame_offset_ho = TRUE;
      mcctcho_ho_data.frame_offset = ho->extra.frame_offset;
    }
  } /* if (ho->extra_parms) */

  /* Convert implicit action to explicit if there's a change in the frame
   * offset or long code mask to match with when the change takes
   * place on the base station side according to 7.6.6.2.2.10 of IS95B.  
   */
  if ( mcctcho_ho_data.different_frame_offset_ho
       || mcctcho_different_long_code_mask(&msg_ptr->msg.ho))
  {
    /* Convert implicit action time to explicit */
    if ( ! ho->use_time )
    {
      ho->use_time = TRUE;
      
      /* --------------------------------------------------------------------
      ** Calculate action time based on the frame when the Handoff Direction
      ** Message was received:
      **
      ** action time =
      **   (20 ms frame when HO Direction Msg received + 8) -
      **     (20 ms frame when HO Direction Msg received mod 4)
      ** --------------------------------------------------------------------- */
      qw_equ( temp_time, msg_ptr->frm.frame );
      qw_inc( temp_time, 8UL );
      qw_dec( temp_time, qw_lo( msg_ptr->frm.frame ) % 4 );
      
      /* Get action time into 80 ms units */
      (void) qw_div_by_power_of_2( temp_time, temp_time, 2 );
      
      /* Get 80 ms time modulo 64 */
      ho->action_time =
      (byte) qw_div_by_power_of_2( temp_time, temp_time, 6 );

      /* If PC change has an implicit action time , then it just means it takes effect
      along with the rest of the HO mesg but since we converted that to be explicit
      convert this also, so that it is clear that they can take effect at the same time */
      if (P_REV_IS_6_OR_GREATER)
      {
        if ((ho->msg_type == CAI_GEN_HO_DIR_MSG) || (ho->msg_type == CAI_UNIV_HO_DIR_MSG)
            #ifdef FEATURE_MEID_SUPPORT
            || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
            #endif
           )
        {
          if (!ho->use_pc_time)
          {
            ho->use_pc_time = TRUE;
            ho->pc_action_time = ho->action_time;
          }
        }
      }
    }
  }
  
  get_abs_action_time( mcctcho_ho_data.abs_action_time, ho->action_time);
  mcctcho_ho_data.explicit_at = ho->use_time;

  /* Check for hard handoff */
  if ( mcctcho_ho_data.disjoint_active_sets_ho ||
       mcctcho_ho_data.different_freq_ho       ||
       mcctcho_ho_data.different_frame_offset_ho
#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */
     )
  {
    mcctcho_ho_data.hard_handoff_in_progress = TRUE;

    if (!ho->use_time)
    {
      ts_get_20ms_frame_offset_time( mcctcho_ho_data.abs_action_time );
      /* In this case, we want HO txn to begin at next ATMR
      interrupt  and HHO_Begin txn to begin one frane after that */
      qw_inc( mcctcho_ho_data.abs_action_time,2);
      /* If action time is set to to 2 frames now, the HO txn itself will start
      1 frame before that i.e 1 frame from now */

    }

    if (mcctc_ppsm_enabled)
    {
      mcctc_ppsm_enabled = FALSE;
      mcc_srch_buf.meas.hdr.cmd = SRCH_STOP_PPSM_F;
      mcc_srch_cmd( &mcc_srch_buf );
    }

    mcctcho_ho_data.hard_handoff_successful = TRUE;
  }

  /* --------------------------------------------------------------------------
  ** Save the (E|G|U)HDM for post-handoff processing
  ** ----------------------------------------------------------------------- */
  mcctcho_ho_data.ho_msg = *ho;
  if ( ho->extra_parms )
  {
    mcctcho_ho_data.pending_scr_nnscr_changed = ho->extra.scr_included
      || ho->extra.nnscr_included
      ;

    /* Remember the TF because if the HHO fails and returns to the SF, the
     * Candidate Frequency Search Report Message needs to include the TF
     */
    mcctcho_ho_data.tf_band_class = ho->extra.band_class;
    mcctcho_ho_data.tf_cdma_freq = ho->extra.cdma_freq;
  }
  else
  {
    mcctcho_ho_data.tf_band_class = cdma.band_class;
    mcctcho_ho_data.tf_cdma_freq = cdma.cdmach;
    mcctcho_ho_data.ho_msg.extra.band_class = cdma.band_class;
    mcctcho_ho_data.ho_msg.extra.cdma_freq = cdma.cdmach;
    mcctcho_ho_data.pending_scr_nnscr_changed = FALSE;
  }

  mcctcho_ho_data.cf_srch_results.num_pilots = 0;
  mcctcho_ho_data.restart_periodic_srch = FALSE;

  /* IS2000 Changes */
  /* We need to do this if SCCH info changes in a Soft handoff */
  mcctcho_ho_data.update_dec_sup = FALSE;
  
  if ( P_REV_IS_6_OR_GREATER &&
       ( (ho->msg_type == CAI_GEN_HO_DIR_MSG) || (ho->msg_type == CAI_UNIV_HO_DIR_MSG)
         #ifdef FEATURE_MEID_SUPPORT
         || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
         #endif
       ) )
  {
    mcctcho_ho_data.fpc_subchan_gain_incl = TRUE;
  }
  else
  {
    mcctcho_ho_data.fpc_subchan_gain_incl = FALSE;
    ho->rev_fch_gating_mode = cdma.rev_fch_gating_on;
    ho->rev_pwr_cntl_delay_incl = FALSE;
  }
  /* Although sch_incl only comes in UHDM, just do it here regardless anyway.  It will
     only be looked at later if the message is UHDM */
  if (ho->srch_inc)
  {
    if ((ho->msg_type == CAI_GEN_HO_DIR_MSG) || (ho->msg_type == CAI_UNIV_HO_DIR_MSG)
        #ifdef FEATURE_MEID_SUPPORT
        || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
        #endif
       )
    {
      /* Sign extend the intercepts if necessary. */

      if (ho->srch.add_intercept & CAI_HO_INTERCEPT_SIGN_BIT)
      {
        mcctcho_ho_data.ho_msg.srch.add_intercept =
          (int1)(ho->srch.add_intercept | CAI_HO_INTERCEPT_SIGN_EXT);
      }

      if (ho->srch.drop_intercept & CAI_HO_INTERCEPT_SIGN_BIT)
      {
        mcctcho_ho_data.ho_msg.srch.drop_intercept =
          (int1)(ho->srch.drop_intercept | CAI_HO_INTERCEPT_SIGN_EXT);
      }

    } // if (GHDM || UHDM)
    else
    {
      mcctcho_ho_data.ho_msg.srch.srch_win_n = mcc_ho_real.win_n;
      mcctcho_ho_data.ho_msg.srch.srch_win_r = mcc_ho_real.win_r;
      
      mcctcho_ho_data.ho_msg.srch.soft_slope = mcc_ho_real.soft_slope;
      // Note that add and drop intercepts are signed values here, but in
      // ho_msg, they are defined as unsigned.
      mcctcho_ho_data.ho_msg.srch.add_intercept  = mcc_ho_real.add_intercept;   //lint !e732
      mcctcho_ho_data.ho_msg.srch.drop_intercept = mcc_ho_real.drop_intercept;  //lint !e732
    }


    if (!P_REV_IS_4_OR_GREATER)
    {
      /* If P_REV is NOT 4 or greater, then this handoff message must be an
       HDM or EHDM (GHDM is only for P_REV of 4 and higher).  Zero out the
       soft handoff improvement fields, since the P_REV is not high enough
       to support this feature.  In the event of a GHDM without
       SRCH_INCLUDED fields or an HDM or EHDM when P_REV is 4 or greater,
       we use values from a previous message (stored in mcc_ho_real), even
       though HDM and EHDM do not know about these fields. */

      mcctcho_ho_data.ho_msg.srch.soft_slope     =
      mcctcho_ho_data.ho_msg.srch.add_intercept  =
      mcctcho_ho_data.ho_msg.srch.drop_intercept = 0;
    }

  } // if (srch_inc)


  /* Mark message as processed */
  ho->pre_proc_done = TRUE;

} /* pre_process_handoff_msg */

/*===========================================================================

FUNCTION MCCTCHO_LOG_HANDOFF_EVENT

DESCRIPTION
  This function performs the event reporting of soft and hard handoff.

DEPENDENCIES
  This function is only called on a HHO and after CCL is updated.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcctcho_log_handoff_event
(
  void
)
{
  event_id_enum_type event_id; /* Event ID */

  mclog_ms_handoff_type event_handoff = 
    { 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 
      0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0 
    }; /* Hard or Soft handoff event info */

  int i; /* Loop counter */

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */
  {

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */
    {
      /* Fill in handoff info for events report */
      for (i = 0; ((i< mcctcho_ho_data.ho_msg.num_pilots) && (i<CAI_N6M)); i++ )
      {
        event_handoff.pn[i] = mcctcho_ho_data.ho_msg.pilot[i].pilot_pn;
        switch (i)
        {
          case 0:
            event_handoff.in_sch_aset.s1 =
#ifdef FEATURE_IS2000_SCH
              mccccl_code_channel_list.pilot_is_in_sch_aset(
                                                           mcctcho_ho_data.ho_msg.pilot[i].pilot_pn);
#else
          FALSE;
#endif /* FEATURE_IS2000_SCH */
            event_handoff.pwr_comb_ind.sr1 =
              mcctcho_ho_data.ho_msg.pilot[i].pwr_comb_ind;
            break;

          case 1:
            event_handoff.in_sch_aset.s2 =
#ifdef FEATURE_IS2000_SCH
              mccccl_code_channel_list.pilot_is_in_sch_aset(
                                                           mcctcho_ho_data.ho_msg.pilot[i].pilot_pn);
#else
          FALSE;
#endif /* FEATURE_IS2000_SCH */
            event_handoff.pwr_comb_ind.sr2 = 
              mcctcho_ho_data.ho_msg.pilot[i].pwr_comb_ind;
            break;

          case 2:
            event_handoff.in_sch_aset.s3 =
#ifdef FEATURE_IS2000_SCH
              mccccl_code_channel_list.pilot_is_in_sch_aset(
                                                           mcctcho_ho_data.ho_msg.pilot[i].pilot_pn);
#else
          FALSE;
#endif /* FEATURE_IS2000_SCH */
            event_handoff.pwr_comb_ind.sr3 = 
              mcctcho_ho_data.ho_msg.pilot[i].pwr_comb_ind;
            break;

          case 3:
            event_handoff.in_sch_aset.s4 =
#ifdef FEATURE_IS2000_SCH
              mccccl_code_channel_list.pilot_is_in_sch_aset(
                                                           mcctcho_ho_data.ho_msg.pilot[i].pilot_pn);
#else
          FALSE;
#endif /* FEATURE_IS2000_SCH */
            event_handoff.pwr_comb_ind.sr4 = 
              mcctcho_ho_data.ho_msg.pilot[i].pwr_comb_ind;
            break;

          case 4:
            event_handoff.in_sch_aset.s5 =
#ifdef FEATURE_IS2000_SCH
              mccccl_code_channel_list.pilot_is_in_sch_aset(
                                                           mcctcho_ho_data.ho_msg.pilot[i].pilot_pn);
#else
          FALSE;
#endif /* FEATURE_IS2000_SCH */
            event_handoff.pwr_comb_ind.sr5 = 
              mcctcho_ho_data.ho_msg.pilot[i].pwr_comb_ind;
            break;

          case 5:
            event_handoff.in_sch_aset.s6 =
#ifdef FEATURE_IS2000_SCH
              mccccl_code_channel_list.pilot_is_in_sch_aset(
                                                           mcctcho_ho_data.ho_msg.pilot[i].pilot_pn);
#else
          FALSE;
#endif /* FEATURE_IS2000_SCH */
            event_handoff.pwr_comb_ind.sr6 = 
              mcctcho_ho_data.ho_msg.pilot[i].pwr_comb_ind;
            break;

          default:
            break;

        } /* end switch */
      } /* end for */
    }

    /* MS perform hard handoff */
    if (mcctcho_ho_data.hard_handoff_in_progress)
    {
      /* Frequency change */
      if ( mcctcho_ho_data.different_freq_ho)
      {
        event_id = EVENT_HARD_HANDOFF_FREQ_CHANGE_V2;
      }
      /* Disjoint active set */
      else if (mcctcho_ho_data.disjoint_active_sets_ho)
      {
        event_id = EVENT_HARD_HANDOFF_DISJOINT_ASET_V2;
      }
      /* Frame offset change */
      else
      {
        event_id = EVENT_HARD_HANDOFF_FRAME_OFFSET_CHANGE_V2;
      }
    }
    /* MS perform soft handoff */
    else
    {
      event_id = EVENT_SOFT_HANDOFF_V2;
    }

    /* Event report: Soft handoff or Hard handoff */
    event_report_payload(event_id, sizeof(event_handoff),
                         (void *) &event_handoff);
  }

} /* mcctcho_log_handoff_event */

/*===========================================================================

FUNCTION mcctcho_update_ccl_with_new_aset

DESCRIPTION
  This function updates code channel list table , this is called by the txn
  pre-processor in the server context

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Code Channel list table in mccccl.c gets modified.

===========================================================================*/

void mcctcho_update_ccl_with_new_aset
( 
  void
)
{
  /* This fn is called from TXN pre-processor, so keep it lean, no
  logging or event reporting here */

  caii_ho_msg_type *ho = &mcctcho_ho_data.ho_msg;

  #ifdef FEATURE_IS95B_MDR
  mccscm_stat_enum_type scm_status;
  /* Return value for duration check */
  #endif // ifdef (FEATURE_IS95B_MDR)

  mccccl_update_mode_type update_type;
  /* Indicates whether forward supplemental channels are updated too. */
  /* Is generate_aset function to act on forward supplemental info too? */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  mcctcho_ho_data.update_dec_sup = FALSE;
     /* Indicates if need to update number of sup for dec */

  #ifdef FEATURE_IS95B_MDR
  if (ho->sup_chan_parms_inc && ho->sup.for_included)
  {
    /* If GHDM includes forward sup. chan., it's not linked to SCAM, so we
     * need to clear any pending SCAM status */
    mccscm_link_data.use_for_hdm_seq = FALSE;

    /* Cancel previous duration timer, since it will be deleted
       or modified if for_included is TRUE. */

    mccscm_cancel_prev_dur_timer();

    if (ho->sup.for_sup_config == CAI_FOR_SUP_SPEC_STOP ||
        ho->sup.for_sup_config == CAI_FOR_SUP_SPEC_START)
    {
      /* Forward supplementals are listed, so
         update both pilots and supplementals. */
      update_type = MCCCCL_UPDATE_ALL;
    }
    else
    {
      /* No forward supplementals, so only update pilots. */
      update_type = MCCCCL_UPDATE_PILOTS;
    }

    if (ho->sup.for_sup_config == CAI_FOR_SUP_START ||
        ho->sup.for_sup_config == CAI_FOR_SUP_SPEC_START)
    {
      mcctcho_ho_data.sups_needed = TRUE;

      /* We are starting forward supplementals.  Check duration timer. */
      if (ho->sup.use_for_dur)
      {
        scm_status = mccscm_duration_timer_chk(ho->sup.for_dur,
                          !mcctcho_ho_data.explicit_at); //lint !e730 Yes, arg is boolean.
        ASSERT(scm_status == MDR_DONE_S); 

      }
    }
    else  /* We are to stop using forward supplemental channels. */
    {
      mcctcho_ho_data.sups_needed = FALSE;
    }
  }
  else  /* sup_chan_parms_inc == FALSE || for_included == FALSE */
  {
    /* if for_included == FALSE, we may have a SCAM link */
    if (mccscm_link_data.use_for_hdm_seq &&
        mccscm_link_data.for_linked_hdm_seq == cdma.hdm_seq )
    /* The linked SCAM has arrived */
    {
      /* we now need to update the supplemental info from the saved SCAM */
      if(scam_timer_msg.fix.for_sup_config == CAI_FOR_SUP_SPEC_START)
      {
        /* Update the code channel list information
        by calling the appropriate CCL function */

        mccccl_code_channel_list.update_sups(
          (scam_timer_msg.fix.num_sup_plt), /* Number of pilots with sups */
          (scam_timer_msg.fix.num_sup),     /* Number of fwd sups         */
          (scam_timer_msg.var)              /* Pointer to code chan info  */
        );
      }
      /* cancel prev. duration timer, since it will either be deleted or
         modified */
      mccscm_cancel_prev_dur_timer();
      mccscm_link_data.use_for_hdm_seq = FALSE;
      if (mccscm_link_data.scam_for_order == FALSE)
      // stop the forward supl. channels
      {
        mcctcho_ho_data.sups_needed = FALSE; // we no longer needed supl. channels
      }
      else
      {
        mcctcho_ho_data.sups_needed = TRUE;
        if (mccscm_link_data.scam_for_dur_mode)
        {
          scm_status = mccscm_duration_timer_chk(mccscm_link_data.for_dur, FALSE);
          /* We don't need to adjust the duration time, as in the case of
           * SCAM receiving an implicit start time */
          ASSERT(scm_status == MDR_DONE_S);
        }
      }
    }
    /* if GHDM is not linked to SCAM or SCAM hasn't arrived
       MS shall continue processing using the updated CCL. */
    /* No forward supplemental channel information included */
    update_type = MCCCCL_UPDATE_PILOTS;
  }
  #else
  update_type = MCCCCL_UPDATE_PILOTS;
  #endif // defined(FEATURE_IS95B_MDR)

  /* MUST update code channel list BEFORE generating and
     updating the active set. */

  if ( (ho->msg_type != CAI_UNIV_HO_DIR_MSG) 
       #ifdef FEATURE_MEID_SUPPORT
       && (ho->msg_type != CAI_MEID_UNIV_HO_DIR_MSG)
       #endif
     )
  {
    mccccl_code_channel_list.update_all(cdma.band_class, cdma.cdmach,
                                      ho->num_pilots, update_type,
                                      ho->sup.num_for_sup, ho->pilot, ho->ch_ind);
  }
  else
  {
    mccccl_code_channel_list.update_ccl_with_uhdm(ho);
  }

} /* mcctcho_update_ccl_with_new_aset */

/*===========================================================================

FUNCTION mcctcho_generate_aset

DESCRIPTION
  This function updates code channel list table and returns aset in
  the parameters passed as references

DEPENDENCIES
  CCL must already be updated

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

mccccl_sup_state_type mcctcho_generate_aset
(
  word * aset_count,
  srch_tc_aset_type * aset
    /* Pointer to active set parameters */
)
{
  mccccl_sup_state_type sup_state;
#ifdef FEATURE_IS95B_MDR
  uint8 i;
  caii_ho_msg_type *ho = &mcctcho_ho_data.ho_msg;
  boolean link_completed = FALSE;
#endif /* FEATURE_IS95B_MDR */

#ifdef FEATURE_IS95B_MDR
  if (mccscm_link_data.use_for_hdm_seq &&
      mccscm_link_data.for_linked_hdm_seq == cdma.hdm_seq )
  /* The linked SCAM has arrived */
  {
    link_completed = TRUE; //indicating the linking of SCAM-GHDM is completed
  }

  if ((ho->sup_chan_parms_inc && ho->sup.for_included)
      || link_completed
     )
  {
     mcctcho_ho_data.update_dec_sup = TRUE;

    /* Indicate forward supplementals are needed with the active set. */
    *aset_count  =
      mccccl_code_channel_list.
        generate_aset(aset, &sup_state, mcctcho_ho_data.sups_needed);
  }
  else
  {
    /* No forward supplemental information is included in message.
       Let code channel list manager decide about supplementals. */
    *aset_count  =
      mccccl_code_channel_list.
        generate_aset_default_sups(aset, &sup_state);
  }

  if ( *aset_count != ho->num_pilots)
  {
    MSG_ERROR( "ASET size %d not equal num_pilots %d", *aset_count, ho->num_pilots, 0 );
  }

  /* Need to inform RXC about SCCH */
  if (mcctcho_ho_data.update_dec_sup)
  {
    for (mcctcho_ho_data.num_sups=i=0; i < *aset_count; i++)
    {
      if ( aset[i].scch_info.num > 0 )
      {
        mcctcho_ho_data.num_sups = aset[i].scch_info.num;
        break;
      }
    }
  }
#else /* !FEATURE_IS95B_MDR */
  
  *aset_count  =
    mccccl_code_channel_list.
      generate_aset_default_sups(aset, &sup_state);

#endif /* FEATURE_IS95B_MDR */

  return sup_state;
 
} /* mcctcho_generate_aset */

/*===========================================================================

FUNCTION SEND_HO_CMD_TO_RXTX

DESCRIPTION
  This function reset layer 2; and set the
  appropriate encryption mode if necessary.

DEPENDENCIES
  This function is called unconditionally for a soft handoff and a HHO
  without Return-If-Fail at action time.  It is also called for a
  Return-If-Fail HHO after the handoff is declared to be successful

RETURN VALUE
  None

SIDE EFFECTS
  See description

===========================================================================*/

void send_ho_cmd_to_rxtx
( 
  void
)
{
  if ( mcctcho_ho_data.ho_msg.extra_parms )
  {
    /* ----------------------------------------------------------------
    ** If the RESET_L2 field is equal to '1', reset the acknowledgement
    ** procedures as specified in IS-95 6.6.4.1.3.3...
    ** ---------------------------------------------------------------- */
    mcc_rxtx_buf.ho.hdr.command = RXTX_HO_F;
    if ( mcctcho_ho_data.ho_msg.extra.reset_l2 )
    {
      MSG_MED( "Resetting L2", 0, 0, 0 );
      mcc_rxtx_buf.ho.reset_l2 = TRUE;
    }
    else
    {
      mcc_rxtx_buf.ho.reset_l2 = FALSE;
    }

    /* ...and reset the Forward Traffic Channel power control counters
       as specified in 6.6.4.1.1.1. */
    //if ( mcctcho_ho_data.reset_fpc )
    //{
     // MSG_MED( "Resetting FPC", 0, 0, 0 );
     // mcc_rx_buf.pwr_on.hdr.command = RXC_PWR_RESET_F;
     // mcc_rxc_cmd( &mcc_rx_buf );
   // }
    /* Initialize to default values */
    mcc_rxtx_buf.ho.encrypt_mode = CAI_ENCRYPTION_DISABLED;

    #ifdef FEATURE_AUTH
    /* Send command to Layer 2 task informing it of the encrypt mode */
    MSG_MED( "Setting encryption mode", 0, 0, 0 );
    //mcc_rxtx_buf.tc.hdr.command = RXTX_ENCRYPT_MODE_F;

    if (mcctcho_ho_data.ho_msg.extra.encrypt_mode == CAI_ENCRYPTION_DISABLED)
    {
      /* no need to check if encryption is possible */
      mcc_rxtx_buf.ho.encrypt_mode = CAI_ENCRYPTION_DISABLED;
    }
    else
    {
      mcc_rxtx_buf.ho.encrypt_mode =
        (auth_encryption_possible ()) ?
          mcctcho_ho_data.ho_msg.extra.encrypt_mode : CAI_ENCRYPTION_DISABLED;
    }
    /* Record to regional variable if we are using encryption */
    cdma.encrypt_mode = mcc_rxtx_buf.ho.encrypt_mode;
    #endif /* FEATURE_AUTH */

    mcc_rxtx_cmd( &mcc_rxtx_buf );
  } // if (extra_parms)

} /* send_ho_cmd_to_rxtx */

/*===========================================================================

FUNCTION DO_SUCCESSFUL_HANDOFF_OPERATIONS

DESCRIPTION
  This function performs all the operations associated with a successful
  handoff

DEPENDENCIES
  For a soft handoff, this function is always called.  For a HHO, this
  function is called only if the handoff is successful

RETURN VALUE
  None

SIDE EFFECTS
  This function may update the service negotiation type, packet zone ID,
  p_rev_in_use, and power control step.  It will also send a handoff
  completion message

===========================================================================*/

void do_successful_handoff_operations
( 
  void 
)
{
  db_items_value_type db_item;
  mccccim_event_type mccccim_evt;
    /* Event Buffer to be sent to CCIM, if necessary */

  #ifdef FEATURE_AUTH
  cm_mc_rpt_type * cm_ptr;
    /* Pointer to report that may be sent to CM */
  #endif /* FEATURE_AUTH */

  word sid = mccsyobj_get_sid(); 
  word nid = mccsyobj_get_nid(); 
  byte pzid = mccsyobj_get_pzid();
    /* sid, nid and pzid */

  #ifdef FEATURE_IS2000_REL_A_BSID
  boolean pilot_info_req_supported_in_hdm = FALSE;
    /* Indicates if field pilot_req_supported is in ?HDM */
  #endif /* FEATURE_IS2000_REL_A_BSID */

  #if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
  #endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */

  #ifdef FEATURE_IS2000_SCH
  /* now verify if the pending sch assignment is still valid after the HO */
  if (!mccsch_verify_fwd_assignment())
  {
    // if not valid anymore, stop the pending transaction
    MSG_MED("HO cancelled FWD SCH0 assignment",0,0,0);
    mctrans_void_trans(MCTRANS_F_SCH0_START);
  }
  #endif // FEATURE_IS2000_SCH

  #ifdef FEATURE_IS95B_MDR
  if (mcctcho_ho_data.update_dec_sup)
  {
    // Set up command for RXC
    mcc_rx_buf.scch.hdr.command = RXC_SCCH_F;
    mcc_rx_buf.scch.num_sup = mcctcho_ho_data.num_sups;
    mcc_rxc_cmd( &mcc_rx_buf );
    MSG_MED("Sent SCCH info to RXC #num_scch is %d",
            mcctcho_ho_data.num_sups,0,0);
  }
  #endif /* FEATURE_IS95B_MDR */

  /* Update freq and band class */
  if (mcctcho_ho_data.ho_msg.extra_parms)
  {
    cdma.cdmach = mcctcho_ho_data.ho_msg.extra.cdma_freq;
    cdma.band_class = mcctcho_ho_data.ho_msg.extra.band_class;
  }

  /* Update frame offset */
  cdma.frame_offset = mcctcho_ho_data.frame_offset;

  /* Update R-FCH gating */
  cdma.rev_fch_gating_on = mcctcho_ho_data.ho_msg.rev_fch_gating_mode;

  /* Update power control delay */
  if (mcctcho_ho_data.ho_msg.rev_pwr_cntl_delay_incl)
  {
    cdma.rev_pwr_cntl_delay = mcctcho_ho_data.ho_msg.rev_pwr_cntl_delay;
  }

  if (mcctcho_ho_data.ho_msg.extra_parms)  
  {
    if (mcctcho_ho_data.ho_msg.extra.scr_included)
    {
      /* Tell SRV state machine to go ahead and connect this SO */
      srv_event_type  ev;
      ev.ec = SRV_SCR_CMP_EC; 
      MSG_LOW("Action time for SCR has arrived",0,0,0);
      srv_rpt_event ( &ev );
      /* Now tell CCIM to be update its data base */
      mccccim_evt.update_conrec.event_code = MCCCCIM_UPDATE_CON_RECS;
      mccccim_evt.update_conrec.new_cfg_ptr = snm_get_current_config();
      mccccim_process_event( &mccccim_evt );
    }
  }

#ifdef FEATURE_IS2000_SCH
  if (mcctcho_ho_data.hard_handoff_in_progress &&
      mcctcho_ho_data.hard_handoff_successful &&
      mcctcho_ho_data.ho_msg.extra_parms && 
      mcctcho_ho_data.ho_msg.extra.return_if_ho_fail)
  {
    // If it is UHDM and f-sch assignment comes with this message, we have to
    // despatch transactions for it, for HHO-RIF case we wait until this point
    // i.e till HHO is successful
    mcctcho_start_sch_assignments(&mcctcho_ho_data.ho_msg);
  }
#endif 

  /* Update Search parameters */
  if ( mcctcho_ho_data.ho_msg.srch_inc )
  {
    mcc_ho_real.add_intercept = (int8)mcctcho_ho_data.ho_msg.srch.add_intercept;
    mcc_ho_real.drop_intercept = (int8)mcctcho_ho_data.ho_msg.srch.drop_intercept;
    mcc_ho_real.soft_slope = mcctcho_ho_data.ho_msg.srch.soft_slope;
    mcc_ho_real.win_a = mcctcho_ho_data.ho_msg.srch.srch_win_a;
    mcc_ho_real.win_n = mcctcho_ho_data.ho_msg.srch.srch_win_n;
    mcc_ho_real.win_r = mcctcho_ho_data.ho_msg.srch.srch_win_r;
    mcc_ho_real.t_add = mcctcho_ho_data.ho_msg.srch.t_add;
    mcc_ho_real.t_comp = mcctcho_ho_data.ho_msg.srch.t_comp;
    mcc_ho_real.t_drop = mcctcho_ho_data.ho_msg.srch.t_drop;
    mcc_ho_real.t_tdrop = mcctcho_ho_data.ho_msg.srch.t_tdrop;
  }

  /* Update FPC subchannel gain */
  if ( mcctcho_ho_data.process_subchan_gain_at_ho )
  {
    cdma.fpc_subchan_gain = mcctcho_ho_data.ho_msg.fpc_subchan_gain;
  }

  /* Send information about HO to RXTX if necessary */
  send_ho_cmd_to_rxtx();

  /* Update serv neg mode */
  if ( mcctcho_ho_data.ho_msg.extra_parms )
  {
    if (cdma.serv_neg && !mcctcho_ho_data.ho_msg.extra.serv_neg_type)
    {
      /* We have switched to SO negotiation now, inform CCIM */
      mccccim_evt.so_neg.event_code = MCCCCIM_SO_NEG;
      mccccim_process_event( &mccccim_evt );
    }
    /* This specifies whether to use service negotiation or SON */
    cdma.serv_neg = mcctcho_ho_data.ho_msg.extra.serv_neg_type;
  }
  
  /* Update power control step */
  if (mcctcho_ho_data.ho_msg.use_pwr_cntl_step)
  {
    cdma.pwr_cntl_step = mcctcho_ho_data.ho_msg.pwr_cntl_step;
  }

  /* Update retry delay */
  if ((mcctcho_ho_data.ho_msg.msg_type == CAI_UNIV_HO_DIR_MSG)
      #ifdef FEATURE_MEID_SUPPORT
      || (mcctcho_ho_data.ho_msg.msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
      #endif
     )
  {
    /* Clear all retry delay if requested */
    if (mcctcho_ho_data.ho_msg.clear_retry_delay)
    {
      update_retry_delay(CAI_RETRY_CLR_ALL, 0);
    }
  } /* end if (mcctcho_ho_data.ho_msg.msg_type == CAI_GEN_HO_DIR_MSG) */

  #ifdef FEATURE_IS2000_SCH
  /* Clear ignore ESCAM indicator */
  mccsch_update_ignore_escam(FALSE);
  #endif /* FEATURE_IS2000_SCH */

  /* *************************************************************
  ** Update specific fields present only in >= P REV 4 format ?HDM
  ** ************************************************************* */
  if (P_REV_IS_4_OR_GREATER)
  {
    if ((mcctcho_ho_data.ho_msg.msg_type == CAI_GEN_HO_DIR_MSG) ||
        (mcctcho_ho_data.ho_msg.msg_type == CAI_UNIV_HO_DIR_MSG)
        #ifdef FEATURE_MEID_SUPPORT
        || (mcctcho_ho_data.ho_msg.msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
        #endif
       )
    {
      if (mcctcho_ho_data.ho_msg.extra_parms)
      {
        pzid = mcctcho_ho_data.ho_msg.extra.packet_zone_id;
      }
    }
  }

  #ifdef FEATURE_IS2000_REL_A
  /* *************************************************************
  ** Update specific fields present only in >= P REV 7 format ?HDM
  ** ************************************************************* */
  if (P_REV_IS_7_OR_GREATER)
  {
    if ((mcctcho_ho_data.ho_msg.msg_type == CAI_GEN_HO_DIR_MSG) ||
        (mcctcho_ho_data.ho_msg.msg_type == CAI_UNIV_HO_DIR_MSG)
        #ifdef FEATURE_MEID_SUPPORT
        || (mcctcho_ho_data.ho_msg.msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
        #endif
       )
    {
      cdma.cs_supported = mcctcho_ho_data.ho_msg.cs_supported;

      #ifdef FEATURE_IS2000_REL_C
      if ( P_REV_IS_9_OR_GREATER && mcctcho_ho_data.ho_msg.cs_supported )
      {
        cdma.mob_qos = mcctcho_ho_data.ho_msg.mob_qos;
      }
      #endif /* FEATURE_IS2000_REL_C */
    }

    #ifdef FEATURE_IS2000_REL_C
    #ifdef FEATURE_IS2000_REL_A_BSID
    if ( P_REV_IS_9_OR_GREATER &&
         ( mcctcho_ho_data.ho_msg.msg_type == CAI_UNIV_HO_DIR_MSG )
       ) 
    {
      cdma.pilot_info_req_supported = mcctcho_ho_data.ho_msg.pilot_info_req_supported;
      pilot_info_req_supported_in_hdm = TRUE;
    }
    #endif /* FEATURE_IS2000_REL_A_BSID */
    #endif /* FEATURE_IS2000_REL_C */

  }
  #endif /* FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IS2000_REL_C
  /* *************************************************************
  ** Update specific fields present only in >= P REV 9 format ?HDM
  ** ************************************************************* */
  if (P_REV_IS_9_OR_GREATER)
  {
    /* -------------------------------------------------------------------
    ** P-rev 9 UHDMs contain the Off Time Reporting fields, so update our
    ** info accordingly.  P-rev 8 UHDMs do not contain these fields, so we 
    ** will use our stored values to determine whether to report (that is,
    ** we assume we are on the same network).
    ** -------------------------------------------------------------------- */
    mcctcsup_update_off_time_info(mcctcho_ho_data.ho_msg.cdma_off_time_rep_sup_ind,
                                  mcctcho_ho_data.ho_msg.cdma_off_time_rep_threshold_unit,
                                  mcctcho_ho_data.ho_msg.cdma_off_time_rep_threshold);

    cdma.tc_to_idle_info.release_to_idle_ind = mcctcho_ho_data.ho_msg.release_to_idle_ind;

    cdma.chm_supported = mcctcho_ho_data.ho_msg.chm_supported;

    if ( mcctcho_ho_data.ho_msg.register_in_idle )
    {
      mccreg_set_register_in_idle( TRUE );
    }

    if (mcctcho_ho_data.ho_msg.msg_type == CAI_UNIV_HO_DIR_MSG)
    {

      #ifdef FEATURE_IS2000_REL_C_FCS
      /* Save info that indicates if BS supports SYNC ID */
      cdma.use_sync_id = mcctcho_ho_data.ho_msg.use_sync_id;
      #endif /* FEATURE_IS2000_REL_C_FCS */

      /* Update SID */
      if (mcctcho_ho_data.ho_msg.sid_incl)
      {
        sid = mcctcho_ho_data.ho_msg.sid;
      }

      /* Update NID */
      if (mcctcho_ho_data.ho_msg.nid_incl)
      {
        nid = mcctcho_ho_data.ho_msg.nid;
      }

      /* Update SDB supported indicator */
      cdma.sdb_supported = mcctcho_ho_data.ho_msg.sdb_supported;
    }
  }
  #endif /* FEATURE_IS2000_REL_C */

  /* *******************
  ** P_REV_IN_USE update
  ** ******************* */
  if (((mcctcho_ho_data.ho_msg.msg_type == CAI_GEN_HO_DIR_MSG) &&
       mcctcho_ho_data.ho_msg.extra_parms)
      ||
      (((mcctcho_ho_data.ho_msg.msg_type == CAI_UNIV_HO_DIR_MSG) 
        #ifdef FEATURE_MEID_SUPPORT
        || (mcctcho_ho_data.ho_msg.msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
        #endif
       ) && mcctcho_ho_data.ho_msg.parms_incl)
     )
  {
    // Update p_rev_in_use, bs_p_rev and report events if necessary
    mcc_update_bs_p_rev( mcctcho_ho_data.ho_msg.extra.p_rev );
  }

  if ((mcctcho_ho_data.ho_msg.msg_type == CAI_EXT_HO_DIR_MSG) &&
      (mcctcho_ho_data.ho_msg.add.add_length > 0)
     )
  {
    /* This is here instead of above with the GHDM P_REV, because,
       at least in theory, the EHDM could have the P_REV field without
       having any "extra" parameters, although that would not make much
       sense. */
    // Update p_rev_in_use, bs_p_rev and report events if necessary
    mcc_update_bs_p_rev( mcctcho_ho_data.ho_msg.add.ext_ho_p_rev );
  } // if (EHDM && add_length)

  /* ***************************************************
  ** Adjust specific fields that depends on P_REV_IN_USE
  ** *************************************************** */
  if (!P_REV_IS_4_OR_GREATER)
  {
    pzid = MCCSYOBJ_PZID_UNKNOWN;
  }

  #ifdef FEATURE_IS2000_REL_A
  if (!P_REV_IS_7_OR_GREATER)
  {
    cdma.cs_supported = FALSE;
  }
  #endif /* FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IS2000_REL_A_BSID
  if (!pilot_info_req_supported_in_hdm)
  {
    cdma.pilot_info_req_supported = (P_REV_IS_7_OR_GREATER) ? TRUE : FALSE;
  }
  else if (!P_REV_IS_7_OR_GREATER)
  {
    cdma.pilot_info_req_supported = FALSE;
  }
  #endif /* FEATURE_IS2000_REL_A_BSID */

  /* ******************************************************************
  ** Stores new SID, NID and PZID and notify its reception if a handler
  ** function is registered.
  ** ****************************************************************** */
  mccsyobj_process_epzid(sid, nid, pzid, CDMA_TC);

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  #if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
  /* Update active Sync ID info after SID and NID are updated */
  mccsyncid_update_active_sync_id();
  #endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

  #if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
  #endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */

  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #endif /* FEATURE_IS2000_REL_A_AES */

  /* ---------------------------------------------------------------
  ** Send the Handoff Completion Message  or Extended Handoff Completion
  ** within T56m seconds after the
  ** action time of a received Handoff Direction Message
  ** --------------------------------------------------------------- */
  #ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    send_ext_handoff_complete_msg();

    #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
    #endif /* FEATURE_IS2000_REL_A_AES */
    
  }
  else
  #endif /* FEATURE_IS2000_REL_A */

  {
    send_handoff_complete_msg();
  }

  #ifdef FEATURE_GPSONE_UPDATE_BSID
#error code not present
  #endif /*  FEATURE_GPSONE_UPDATE_BSID */

  #ifdef FEATURE_GPS_MODE
#error code not present
  #endif /* FEATURE_GPS_MODE */

  /* Start a periodic CFS, if necessary */
  if (mcctcho_ho_data.restart_periodic_srch ||
      (mcctcho_ho_data.ho_msg.extra_parms &&
       mcctcho_ho_data.ho_msg.extra.periodic_srch &&
       (cand_freq_srch_parm.cur_srch_type != SRCH_PERIODIC_CFS) &&
       ((cand_freq_srch_parm.cf_band_class != cdma.band_class) ||
        (cand_freq_srch_parm.cf_cdma_freq != cdma.cdmach))))
  {
    /* If we're not re-starting a periodic search because of an emergency
     * abort, we should update the current LAST_SRCH_MSG_SEQ with the HDM_SEQ
     * field.  Otherwise, we should continue to use the old value, since it's
     * not the GHDM which starts the periodic search.
     *
     * Similarly, if we're not re-starting a periodic search, we should
     * increment the sequence number, since this is a new search request.
     */
    if (!mcctcho_ho_data.restart_periodic_srch)
    {
      cand_freq_srch_parm.last_srch_msg_seq = cdma.hdm_seq;
      cand_freq_srch_parm.cur_srch_seq_num++;
      MSG_LOW("Increment cur_srch_seq_num to %d", cand_freq_srch_parm.cur_srch_seq_num, 0, 0);
    }

    start_cf_srch( SRCH_PERIODIC_CFS );
  }

  #if defined(FEATURE_IS2000_REL_C_DV) && defined (FEATURE_IS2000_TDSO)
#error code not present
  #endif //defined(FEATURE_IS2000_REL_C_DV) && defined (FEATURE_IS2000_TDSO)

  /* ------------------------------------------------------------------
  ** With a successful handoff, we need to update serving system info
  ** to CM. What we know at this point is P_REV, maybe PACKET_ZONE_ID,
  ** Band Class and block.
  ** ------------------------------------------------------------------ */

  mcc_send_cm_srv_sys_info( CM_SS_SID_MASK|
                            CM_SS_NID_MASK|
                            CM_SS_PACKET_ZONE_MASK|
                            CM_SS_BS_P_REV_MASK|
                            CM_SS_P_REV_IN_USE_MASK,   /*lint !e747 */
                            sid,
                            nid,
                            0,
                            pzid
                          );

  // The HO message may contain a delayed FPC_SUBCHAN_GAIN.  Now that the handoff is successful
  // it's time to start the transaction for it now.
  if ( mcctcho_ho_data.fpc_subchan_gain_incl && !mcctcho_ho_data.process_subchan_gain_at_ho )
  {
    mcctxns_begin_subchan_gain_trans( mcctcho_ho_data.subchan_gain_action_time );
  }

  /* Increment the logging paramater HARD_HO_COUNT */
  if ( mcctcho_ho_data.hard_handoff_in_progress )
  {
    mclog_inc_nw_param(HARD_HO_COUNT);
  }

  /* Report soft handoff event */
  if ( !mcctcho_ho_data.hard_handoff_in_progress )
  {
    mcctcho_log_handoff_event();
  }

  /* Now update db information here so that DEBUG can get updated 
   * channel and PN information.
   *
   * in case there are multiple pilots in the Handoff messages,
   * db is updated with the first pilot info
   */
  db_item.pilot_pn_off = mcctcho_ho_data.ho_msg.pilot[0].pilot_pn;
  db_put(DB_PILOT_PN_OFF, &db_item);

  /* Events report: F-FCH or F-DCCH or F-FCH plus F-DCCH acquired */
  mclog_report_event_f_fdcch_acquired();

  /* Report possible long code mask change */
  mclog_report_event_long_code_mask(FALSE);

  #ifdef FEATURE_AUTH
  /* Indicate to CM that the privacy setting may have changed */
  if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cm_ptr->hdr.cmd = CM_PRIVACY_SETTING_F;
    cm_ptr->privacy_setting.long_code = 
      cdma.long_code.private_lcm_active ?  MC_LONG_CODE_PRIVATE : MC_LONG_CODE_PUBLIC; 
    mcc_cm_mc_rpt( cm_ptr );
  }
  else
  {
    MSG_HIGH( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }
  #endif /* FEATURE_AUTH */

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
  #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

  #if defined(FEATURE_IS2000_REL_C_DV) || defined(FEATURE_IS2000_DV_SIG_TEST)
#error code not present
  #endif /* FEATURE_IS2000_REL_C_DV || FEATURE_IS2000_DV_SIG_TEST */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

#if defined( FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING ) || \
      defined( FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING )

  /* Turn the clock rate down */
  clk_regime_deregister_for_cpu_resource( CLKRGM_SRCHNONTC );
#endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING */

} /* do_successful_handoff_operations */

/*===========================================================================

FUNCTION SAVE_SERVING_CONFIG_PARMS

DESCRIPTION
  This function saves the current configuration data, which will be restored
  in the case of a failed Return-If-Fail HHO

DEPENDENCIES
  This function is called for a Return-If-Fail HHO at the beginning of the
  processing of a GHDM.  This function is also called before a vist to the
  Candidate Frequency; however, note that it is not necessary to call its
  counterpart restore_serving_config_parms() after the CFS visit

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void save_serving_config_parms()
{
  /* Following are only needed for HHO */
  if (mcctcho_ho_data.hard_handoff_in_progress)
  {
    /* Save current active set */
    mccccl_code_channel_list.save();

    #ifdef FEATURE_AUTH
    serv_freq_config.private_lcm = cdma.long_code.private_lcm_active;
    #endif /* FEATURE_AUTH */

    #if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_MEID_SUPPORT)
    serv_freq_config.public_lcm_type = cdma.long_code.public_lcm_type;
    if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_BS_SPEC)
    {
      qw_set(serv_freq_config.bs_plcm_39, qw_hi(cdma.long_code.bs_plcm_39), 
             qw_lo(cdma.long_code.bs_plcm_39));
    }
    #endif /* FEATURE_IS2000_REL_C || FEATURE_MEID_SUPPORT */

  }

} /* save_serving_config_parms */

/*===========================================================================

FUNCTION RESTORE_SERVING_CONFIG_PARMS

DESCRIPTION
  This function restores the original configuration data in the case of a
  failed Return-If-Fail HHO

DEPENDENCIES
  This function is called only for a failed Return-If-Fail HHO after a
  SRCH_ABORT_R is received or the handoff timer has expired.  Note that
  a return from a CFS does not call this function, because none of the
  information below should be changed: frame offset, long code mask,
  handoff parameters, and active pilot information, as they are all
  pertaining to Hard Handoff

RETURN VALUE
  None

SIDE EFFECTS
  The following configuration data may be restored: frame offset, long code
  mask, numerous searcher data such as t_add, t_drop, win_a, and all the
  pilot information, etc.

===========================================================================*/

void restore_serving_config_parms
(
  void
)
{

  srv_event_type ev;
  #ifdef FEATURE_IS2000_REL_A  
  mccccim_event_type mccccim_evt;
  uint8 i;
  #endif

  mccccl_code_channel_list.restore();

  #ifdef FEATURE_IS2000_REL_A
  if (mcctcho_ho_data.ho_msg.cc.cc_info_incl)
  {
    /* Need to delete all new CCs created, They are
    not needed anymore */
    for (i=0; i<mcctcho_ho_data.ho_msg.cc.num_calls_assign; i++)
    {
      mccccim_evt.del_cc.event_code = MCCCCIM_DELETE_CC;
      mccccim_evt.del_cc.con_ref = mcctcho_ho_data.ho_msg.cc.conn[i].con_ref;
      mccccim_evt.del_cc.del_reason = MCCCCIM_HHO_FAIL;
      mccccim_process_event( &mccccim_evt);
    }

  }
  #endif

  /* Restore the SRV state machine back to Normal subf */
  if (mcctcho_ho_data.pending_scr_nnscr_changed)
  {
    ev.ec = SRV_RIF_HHO_EC;
    srv_rpt_event ( &ev );
  }

  #ifdef FEATURE_AUTH
  if (cdma.long_code.private_lcm_active != serv_freq_config.private_lcm)
  {
    cdma.long_code.private_lcm_active = serv_freq_config.private_lcm;
  }
  #endif /* FEATURE_AUTH */

  #if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_MEID_SUPPORT)
  if ((cdma.long_code.public_lcm_type != serv_freq_config.public_lcm_type)
      || ((cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_BS_SPEC) 
          && (qw_cmp(cdma.long_code.bs_plcm_39, serv_freq_config.bs_plcm_39) != 0)))
  {
    cdma.long_code.public_lcm_type = serv_freq_config.public_lcm_type;
    if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_BS_SPEC)
    {
      qw_set(cdma.long_code.bs_plcm_39, qw_hi(serv_freq_config.bs_plcm_39),
             qw_lo(serv_freq_config.bs_plcm_39));
    }
  }
  #endif /* FEATURE_IS2000_REL_C */

} /* restore_serving_config_parms */

/*===========================================================================

FUNCTION MCCTCHO_SAVE_ACTIONS_SENT_TO_LOWER_LAYERS

DESCRIPTION
  This saves all actions sent to lower layers so that they can be resent with 
  "old" parameters if we return upon a HHO failure

DEPENDENCIES
  This function is called for a Return-If-Fail HHO  at the HO
  txn pre-processor

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

/*lint -e818*/
void mcctcho_save_actions_sent_to_lower_layers
(
   mctrans_trans_info_type * ho_trans
)
{
  uint8 i, j;

  /* Save all actions for all servers */
  for (i=0; i < MCTRANS_SERVER_COUNT; i++)  /*lint !e641 */
  {
    for (j=0 ; j < MCTRANS_MAX_ACTION_IN_TRANS; j++)
    {
      mcctcho_ho_data.server[i].actions[j] = 
        ho_trans->trans.server[i].actions[j];
      if (ho_trans->trans.server[i].actions[j]
          == MCTRANS_MAX_ACTION)
      {
        /* Actions for this server over, continue with next one */
        break;
      }
    }
  }
 
} /* mcctcho_save_actions_sent_to_lower_layers */
/*lint +e818*/

#ifdef FEATURE_IS2000_SCH
/*===========================================================================

FUNCTION mcctcho_start_sch_assignments

DESCRIPTION
  This function sends SCH assigments to lower layers ( if any) 

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  SCH transactions are triggered ( if necessary)

===========================================================================*/

/*lint -e818*/
void mcctcho_start_sch_assignments
(
  caii_ho_msg_type * ho
)
{
  #ifdef FEATURE_IS2000_F_SCH
  // If it is UHDM and f-sch assignment comes with this message, we have to
  // despatch transactions for it
  if ( ((ho->msg_type == CAI_UNIV_HO_DIR_MSG) 
        #ifdef FEATURE_MEID_SUPPORT
        || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
        #endif
       ) && ho->sch_incl && ho->num_for_assign > 0 )
  {
    uint8   start_time;

    // Note: we support only 1 SCH and num_for_assign can only be 1 here.  Also SCH
    // ID must be zero (checked in validate_handoff_msg). We are making these assumptions
    // here.
    // First of all, see if start time specified
    start_time = ho->for_assign[0].for_sch_start_time_incl ? ho->for_assign[0].for_sch_start_time :
                                                             MCCTCHO_IMPLICIT_ACTION_TIME;
    mccsch_compute_start_time( mcctcho_ho_data.abs_action_time, start_time, 0, MCCSCH_FORWARD );

    // Then determine type of transaction
    if ( ho->for_assign[0].for_sch_duration != 0 )
    {
      mccsch_compute_stop_time( ho->for_assign[0].for_sch_duration, 0, MCCSCH_FORWARD );
    }

    // Now get transaction and then call mccsch_begin_trans to start the transaction
    mccsch_begin_trans( MCTRANS_F_SCH0_START, 0 );

  } // of if ( ho->msg_type...
  #endif // FEATURE_IS2000_F_SCH

  #ifdef FEATURE_IS2000_R_SCH
  // If it is UHDM and r-sch assignment comes with this message, we have to
  // despatch transactions for it
  if ( ((ho->msg_type == CAI_UNIV_HO_DIR_MSG) 
        #ifdef FEATURE_MEID_SUPPORT
        || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
        #endif
       ) && ho->sch_incl && ho->num_rev_assign > 0 )
  {
    uint8   start_time;

    // Note: we support only 1 SCH and num_rev_assign can only be 1 here.  Also SCH
    // ID must be zero (checked in validate_handoff_msg). We are making these assumptions
    // here.
    // First of all, see if start time specified
    start_time = ho->rev_assign[0].rev_sch_start_time_incl ? ho->rev_assign[0].rev_sch_start_time :
                                                             MCCTCHO_IMPLICIT_ACTION_TIME;
    mccsch_compute_start_time( mcctcho_ho_data.abs_action_time, start_time, 0, MCCSCH_REVERSE );

    // Then determine type of transaction
    if ( ho->rev_assign[0].rev_sch_duration != 0 )
    {
      mccsch_compute_stop_time( ho->rev_assign[0].rev_sch_duration, 0, MCCSCH_REVERSE );
    }

    // Now get transaction and then call mccsch_begin_trans to start the transaction
    mccsch_begin_trans( MCTRANS_R_SCH0_START, 0 );

  } // of if ( ho->msg_type...
  #endif // FEATURE_IS2000_R_SCH
} /* mcctcho_start_sch_assignments */
/*lint +e818*/
#endif /* FEATURE_IS2000_SCH */
/*===========================================================================

FUNCTION send_ho_cmds_to_lower_layers

DESCRIPTION
  This function sends HO commands to RXC/TXC/SRCH with its action time
  and corresponding payload. It gets transaction num from the mctrans module.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void send_ho_cmds_to_lower_layers
(
  caii_ho_msg_type * ho
)
{
  qword hho_begin_action_time;

  #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
  #endif /* FEATURE_IS2000_REL_C_DV */

  #ifdef FEATURE_IS2000_SCH
  // Before proceeding with handoff, check if it is a hard handoff.  If so,
  // we need to cancel any current/pending SCH assignment.
  if ( mcctcho_ho_data.hard_handoff_in_progress )
  {
    // Just void any F-SCH/R-SCH assignment pending.  It's harmless even if they don't exist
    mctrans_void_trans( MCTRANS_F_SCH0_START );
    #ifdef FEATURE_IS2000_R_SCH
    mctrans_void_trans( MCTRANS_R_SCH0_START );
    #endif // FEATURE_IS2000_R_SCH

    // Now cancel existing SCH assignments, if any
    mccsch_stop_sch0_trans( mcctcho_ho_data.abs_action_time );
  }

  // Update SCH assignment info. if included
  if (ho->sch_incl)
  {
    mccsch_update_sch_assign_with_uhdm( ho );
  }
  #endif // FEATURE_IS2000_SCH

  // Start handoff transaction.
  mcctxns_begin_ho_trans( mcctcho_ho_data.abs_action_time );

  if (mcctcho_ho_data.hard_handoff_in_progress)
  {
    qw_equ(hho_begin_action_time,mcctcho_ho_data.abs_action_time);
    /* We want HHO Begin txn to happen 1 frame after HO txn */
    qw_inc(hho_begin_action_time, 1);
    mcctxns_begin_hho_begin_trans( hho_begin_action_time );
  }

#ifdef FEATURE_IS2000_SCH
  // Start SCH transaction if not RIF-HHO 
  if ( mcctcho_ho_data.hard_handoff_in_progress &&
     mcctcho_ho_data.ho_msg.extra_parms &&
     mcctcho_ho_data.ho_msg.extra.return_if_ho_fail )
  {
    MSG_HIGH("Delaying SCH processing until after HHO success",0,0,0);
    /* If it is a RIF-HHO, don't bother sending SCH assignment now */
  }
  else
  {
    mcctcho_start_sch_assignments(ho);
  }
#endif

  #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
  #endif /* FEATURE_IS2000_REL_C_DV */
 
} /* send_ho_cmds_to_lower_layers */

/*===========================================================================

FUNCTION MCCTCHO_GET_HO_DATA

DESCRIPTION
  This function returns a pointer to the mcctcho_ho_data data structure.

DEPENDENCIES
  None.

RETURN VALUE
  Constant pointer to the mcctcho_ho_data.

SIDE EFFECTS
  None.

===========================================================================*/
mcctcho_ho_data_type *mcctcho_get_ho_data
(
  void
)
{
  return &mcctcho_ho_data;

} /* mcctcho_get_ho_data */

/*===========================================================================

FUNCTION MCCTCHO_DEFAULT_HANDOFF_MSG_FIELDS

DESCRIPTION
  This function defaults fields in the Handoff Direction Messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The Handoff Direction Message passed in will have fields defaulted.

===========================================================================*/
void mcctcho_default_handoff_msg_fields
(
  caii_ho_msg_type *ho
)
{
  uint16  i; 
  db_items_value_type db_item;

  if ((ho->msg_type != CAI_UNIV_HO_DIR_MSG)
#ifdef FEATURE_MEID_SUPPORT
      && (ho->msg_type != CAI_MEID_UNIV_HO_DIR_MSG)
#endif
     )
  {
    /* GHDM, HDM and EHDM can only assign FCH pilots.  Note that the entire active
       set is replaced when we get an HDM (see Standards section 2.6.6.2.6.1), so 
       we no longer have DCCH or DV pilots; this is why we set our ch_ind to FCH. */
    ho->ch_ind = CAI_CHIND_FCH_CONT_REV_PILOT;  /*lint !e641 */
    ho->sch_incl = 0;
    for (i = 0; i < ho->num_pilots; i++)
    {
      /* No other HDM other than UHDM can allocate a QOF so make sure to
         set that to 0 */
      ho->pilot[i].qof_mask_id_fch = 0;
      ho->pilot[i].add_pilot_rec_incl = 0;
      ho->pilot[i].srch_offset = 0;         // Default srch_offset to 0.
    }
    if (!P_REV_IS_6_OR_GREATER)
    {
      ho->extra.nnscr_included = FALSE;
    }

#ifdef FEATURE_IS2000_REL_A
    ho->cc.cc_info_incl = FALSE;
#endif

  }
  else
  {
    /*This is a UHDM message, Does not have anything to do with SCCH, set that
      to 0 */
    ho->sup_chan_parms_inc = 0;
    if (!ho->parms_incl)
    {
      /* Just set some defaults here */
      // get BS P_REV
      db_get( DB_BS_P_REV, &db_item );

      ho->extra.p_rev = db_item.bs_p_rev;

      ho->extra.serv_neg_type = cdma.serv_neg;

    }

#ifdef FEATURE_IS2000_REL_A
    if (!P_REV_IS_7_OR_GREATER || !ho->extra_parms ||
        !ho->extra.scr_included)
    {
      ho->cc.cc_info_incl = FALSE;
    }
#endif

  }

  if (ho->msg_type == CAI_EXT_HO_DIR_MSG)
  {
    /* May need to fill in some default values for EHDM. */
    ho->extra.return_if_ho_fail = FALSE;
    ho->extra.periodic_srch = FALSE;
    ho->extra.scr_included = FALSE;
    ho->extra.nnscr_included = FALSE;
    ho->use_pwr_cntl_step = FALSE;
    ho->sup_chan_parms_inc = 0;

    if (P_REV_IS_2_OR_3) //lint !e506 !e774 Yes, may be const boolean in if
    {
      /* NOM_PWR_EXT is not included in the EHDM for IS95-A */
      ho->extra.nom_pwr_ext = FALSE;
    }

#ifdef FEATURE_PASSPORT_MODE
    if (mcc_passport_mode_is_jcdma() &&
        ((cdma.band_class == CAI_BAND_CLASS3) ||
         (cdma.band_class == CAI_BAND_CLASS6)))
    {
      /* ARIB T-53 specifies that SERV_NEG_TYPE is always 1 (use service
         negotiation - not service option negotiation). */
      ho->extra.serv_neg_type = 1;
    }
    else
#endif /* FEATURE_PASSPORT_MODE */

    {
      if (ho->extra_parms && P_REV_IS_1) //lint !e506 !e774 Const boolean in if
      {
        /* J-STD-008 specifies that SERV_NEG_TYPE is always 1 (use service
           negotiation - not service option negotiation). */
        ho->extra.serv_neg_type = 1;
      }
    }
  } // if (EHDM)
  else if (ho->msg_type == CAI_HO_DIR_MSG)
  {
    /* Need to fill in some default values for HDM.  The parser always sets
       ho->extra_parms to TRUE, since many of the fields included in the
       HDM by default would fall under EXTRA_PARMS. */
    ho->extra.return_if_ho_fail = FALSE;
    ho->extra.periodic_srch = FALSE;
    ho->extra.scr_included = FALSE;
    ho->extra.nnscr_included = FALSE;
    ho->use_pwr_cntl_step = FALSE;
    ho->sup_chan_parms_inc = 0;

    if (!ho->extra.freq_incl)
    {
      /* Need to provide CDMA_FREQ for HDM if not included in msg. */
      ho->extra.cdma_freq  = cdma.cdmach;
    }

    if (ho->extra.reset_l2)
    {
      /* RESET_FPC is not part of HDM, but must do it if RESET_L2 is TRUE. */
      ho->extra.reset_fpc = TRUE;
    }

    /* BAND_CLASS and SERV_NEG_TYPE are not included in the HDM. */

    ho->extra.band_class = cdma.band_class;
    ho->extra.serv_neg_type = cdma.serv_neg;
  } // else if (HDM)
  else // Must be UHDM or GHDM
  {
    if ( !ho->extra_parms )
    {
      // Initialize scr_include and nnscr_included fields properly
      // for easier processing later
      ho->extra.nnscr_included =
        ho->extra.scr_included = FALSE;
    }
  }
} /* mcctcho_default_handoff_msg_fields */

/*===========================================================================

FUNCTION MCCTCHO_PROCESS_GENERIC_HANDOFF_MSG

DESCRIPTION
  This function processes a Generic Handoff Direction Message
  (HDM, EHDM, GHDM, ...).

DEPENDENCIES
  See dependencies of called functions

RETURN VALUE
  None.

SIDE EFFECTS
  It may cause the mobile to do a soft or hard handoff.  It may update the
  search parameters.  It may update the code channel list (both fundamental
  and supplemental channels) used by the searcher.  It may modify the forward
  supplemental channel duration timer.  It may change the CDMA frequency.  It
  may queue a reject message.  It may reset layer 2 and the forward power
  control parameters.  It may change a whole host of base station related
  parameters in the event of a hard handoff

===========================================================================*/

void mcctcho_process_generic_handoff_msg
(
  mccrxtx_msg_type *msg_ptr
    /* Pointer to message received from the base station */
)
{
  caii_ho_msg_type *ho = &msg_ptr->msg.ho;
    /* Convenience pointer to handoff message. */

  const caii_srv_cfg_type *curr_config;

  #ifdef FEATURE_IS2000_REL_A
  byte new_bs_p_rev;
  #endif

  byte    msg_error;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

  /* Does not support concept of BASE_CODE_CHAN. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* --------------------------------------------------------------------------
  ** Fill in default values
  ** We need to do this here because other functions called later may depend on
  ** these defaults.
  ** ----------------------------------------------------------------------- */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif
  {
    mcctcho_default_handoff_msg_fields(ho);  
  }
  
  if ( ! ho->pre_proc_done )
  {

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif
    {
  
      /* AKA/AES related validation needs to go first because its ui_encrypt_mode
       * needs to be checked against the capability in the message before it
       * goes into Service Negotiation.
       */
      if (
#if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */
         (msg_error = validate_handoff_msg( &(msg_ptr->msg), &rej_parms ) ) != 0
         )
      {
        rej_parms.rej_msg_type = ho->msg_type;
#if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */
        rej_parms.rej_code = msg_error;
        send_rej_order( &rej_parms);

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

        return;
      }

      /* --------------------------------------------------------------
      ** Inform system determination of hard handoff and determines the
      ** next state to be processed based on returned SS-Action.
      ** -------------------------------------------------------------- */
      if (ho->extra_parms)
      {
        if (sd_ss_ind_cdma_opr_hdm_cdma((sd_band_e_type)ho->extra.band_class,
                                        ho->extra.cdma_freq, NULL) == SD_SS_ACT_REJECT)
        {
          rej_parms.rej_msg_type = ho->msg_type;
          rej_parms.rej_code = CAI_REJ_CAP;
          send_rej_order( &rej_parms);

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

          return;
        }
      }
      else
      {
        if (sd_ss_ind_cdma_opr_hdm_cdma((sd_band_e_type)cdma.band_class,
                                        cdma.cdmach, NULL) == SD_SS_ACT_REJECT)
        {
          rej_parms.rej_msg_type = ho->msg_type;
          rej_parms.rej_code = CAI_REJ_CAP;
          send_rej_order( &rej_parms);

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

          return;
        }
      } /* end if (ho->extra_parms) */
    }

    pre_process_handoff_msg( msg_ptr );

    #if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
    /* Perform initialization for service configuration restoration */
    mccsyncid_hdm_init(ho);
    #endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */


    /* If a periodic search is in progress, check for abort conditions:
     *   1. The PERIODIC_SRCH field of the (E|G|U)HDM is set to 0
     *   2. We're doing a Hard Handoff
     * Note that even though the standard specifies that we shall abort a
     * periodic search (if one is in progress) at action time, we do it as
     * soon as we receive the GHDM.  This is to avoid the race condition
     * when we might receive a SRCH_CFS_READY_R report while waiting for
     * the action time of the GHDM.  This can cause a problem because the
     * processing of the SRCH_CFS_READY_R report would override the GHDM
     * action time, and the MCC_TXC_FRAME_SIG that is generated would be
     * used by CFS instead.  As a result, the handoff will never take place
     */
    if ( cand_freq_srch_parm.cur_srch_type == SRCH_PERIODIC_CFS )
    {
      if ( ( ( mcctcho_ho_data.ho_msg.extra_parms &&
             ! mcctcho_ho_data.ho_msg.extra.periodic_srch ) ) ||
           ( mcctcho_ho_data.hard_handoff_in_progress ) )
      {
        stop_cf_srch();
        MSG_MED("G|UHDM turned off periodic CFS", 0, 0, 0);
      }
    }

    #ifdef FEATURE_GPS_MODE
#error code not present
    #endif /* FEATURE_GPS_MODE */

    /* The indication if we are in a high RC call is needed in too
    often, compute that once and store the results to be reused */
    curr_config = snm_get_current_config();
    if (curr_config->dcch_cc_incl ||
        (curr_config->fch_cc_incl && (curr_config->for_fch_rc >=CAI_RC_3)))  /*lint !e641 */
    {
      mcctcho_ho_data.high_rc_before_ho = TRUE;
    }
    else
    {
      mcctcho_ho_data.high_rc_before_ho = FALSE;
    }
    mcctcho_ho_data.high_rc_after_ho = mcctcho_ho_data.high_rc_before_ho;

    if (((ho->msg_type == CAI_GEN_HO_DIR_MSG) ||
         (ho->msg_type == CAI_UNIV_HO_DIR_MSG)
         #ifdef FEATURE_MEID_SUPPORT
         || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
         #endif
        ) && ho->extra_parms)
    {
      if (ho->extra.scr_included || ho->extra.nnscr_included)
      {
        /* GHDM (and UHDM) can act as a Service Connect Message.  This is
           only true if we are using service negotiation (not service option
           negotiation).  We checked this when we validated the message. */

        /* We'll send this event to SRV even for a U/GHDM that has the nnscr,
           but no scr. In this case, the SRV event recognizes it and will not
           affect the srv subfunction */

        srv_event_type ev;
        const caii_complete_srv_cfg_type *config;
        ev.ec = SRV_HO_DIR_EC;
        ev.ep.ho_msg.msg_ptr = ho;
        srv_rpt_event ( &ev );

        mcctcho_ho_data.pending_scr_nnscr_changed = TRUE;
        config = snm_get_complete_pending_config();
        if (config->neg_scr.dcch_cc_incl)
        {
          mcctcho_ho_data.high_rc_after_ho = TRUE;
        }
        else
        {
          mcctcho_ho_data.high_rc_after_ho = 
            (config->neg_scr.for_fch_rc >= CAI_RC_3) ? TRUE:FALSE;  /*lint !e641 */
        }
      }
    }

    #ifdef FEATURE_IS2000_REL_A
    /* Need to set the p_rev_in_use AFTER HO correctly */
    if ( ((ho->msg_type == CAI_GEN_HO_DIR_MSG) &&
          ho->extra_parms ) 
         ||
         (((ho->msg_type == CAI_UNIV_HO_DIR_MSG) 
           #ifdef FEATURE_MEID_SUPPORT
           || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
           #endif
          ) && ho->parms_incl )
       )
    {
      new_bs_p_rev = ho->extra.p_rev;
    }
    else
    {
      if ( ( ho->msg_type == CAI_EXT_HO_DIR_MSG ) &&
           ( ho->add.add_length > 0 )
         )
      {
        new_bs_p_rev = ho->add.ext_ho_p_rev;
      }
      else
      {
        new_bs_p_rev = cdma.p_rev_in_use;
      }
    }
    mcctcho_ho_data.new_p_rev_in_use = (new_bs_p_rev > cdma.mob_cai_rev) ? 
                                   cdma.mob_cai_rev : new_bs_p_rev;

    #ifdef FEATURE_IS2000_REL_A_SVD
    /*
    ** CC fields has been evaluated in the pre_process_cc_info().
    ** If we get here, and there is CC info, CCI is initiated.
    */

    if (P_REV_IS_7_OR_GREATER)
    {
      /* CC info only comes in UHDM */
      if ((ho->msg_type == CAI_UNIV_HO_DIR_MSG)
          #ifdef FEATURE_MEID_SUPPORT
          || (ho->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
          #endif
         )
      {
        if ((ho->extra_parms && 
             ho->extra.scr_included && 
             ho->cc.cc_info_incl)&&
             mccap_item_is_supported(MCCAP_DF_CS))
        {
          /* The assumption is that pre_processing is called before */
          process_cc_info( &ho->cc );
        }
      }
    }
    #endif /* FEATURE_IS2000_REL_A_SVD */

    #endif //FEATURE_IS2000_REL_A

    #ifdef FEATURE_IS2000_P2
    mcctcho_ho_data.new_fpc_pri_chan = FALSE;
    #endif    
    action_time_pending = TRUE;
    send_ho_cmds_to_lower_layers(ho);
  } /* if ( ! ho->pre_proc_done ) */
 
} /* mcctcho_process_generic_handoff_msg */

/*===========================================================================

FUNCTION SEND_SUCCESSFUL_HHO_IND_TO_LOWER_LAYERS

DESCRIPTION
  This function is used for signalling to the SRCH/RXC/TXC that a HHO
  attempt has been determined to be successful. This serves as a sync point for
  the lower layers to proceed with things that have been waiting to make sure the
  HHO was successful like reset_fpc, programming nom_pwr, rlga etc.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  See description

===========================================================================*/

void send_successful_hho_ind_to_lower_layers
( 
  void
)
{
  if ( mcctcho_ho_data.ho_msg.extra_parms &&
       mcctcho_ho_data.ho_msg.extra.return_if_ho_fail )
  {
    /* Tell SRCH that HO went fine */
    mcc_srch_buf.hho_ok.hdr.cmd = SRCH_RIF_HHO_SUCCESS_F;
    mcc_srch_cmd( &mcc_srch_buf );

    /* Tell RXC that HHO went fine, We could have renamed this
       command to something more appropriate but left it the
       way it was since this is the only thing that RXC has to do */
    if ( mcctcho_ho_data.ho_msg.extra.reset_fpc )
    {
      MSG_MED( "Resetting FPC", 0, 0, 0 );
      mcc_rx_buf.pwr_on.hdr.command = RXC_PWR_RESET_F;
      mcc_rxc_cmd( &mcc_rx_buf );
    }
  }

  /* Now tell TXC that the HHO was successful, so that it can go ahead
     and set its preambles, nom_pwr, rev link gain values i.e get ready
     for TX to be turned on, There is no payload because all these values
     have already been given in the TXC_HO_F command */
  mcc_tx_buf.hho_ok.hdr.command = TXC_HHO_SUCCESS_F;
  mcc_txc_cmd( &mcc_tx_buf);

} /* send_successful_hho_ind_to_lower_layers */

 /*===========================================================================

FUNCTION MCCTCHO_HANDLE_HO_ACTION_TIME_ARRIVAL

DESCRIPTION
  This fn does processing related to HO action time arrival. For SHO this
  pretty much completes the HO processing, For HHO just a little bit of
  book keeping is done as the HO itself is not quite complete yet

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  See description

===========================================================================*/
void mcctcho_handle_ho_action_time_arrival
(
  void
)
{
  action_time_pending = FALSE;

  /* This assignment needs to be done here because whether the HHO
     fails or not, this needs to be updated */
  cdma.hdm_seq = mcctcho_ho_data.ho_msg.hdm_seq;

  if (mcctcho_ho_data.hard_handoff_in_progress)
  {
    /* send a command to layer 2 to indicate that the transmitter has
        been turned off */
    tc_inform_rxtx_about_tx();

  }
  else
  {
    /* Do more for soft handoff */
#ifdef FEATURE_IS2000_SCH
    mctrans_srch_f_sch_action_type sch_aset;  // Temporary storage for finding out
      // is SCH active set is 0

    // Handoff is just finished by device drivers, let's check to see if the last
    // leg containing F-SCH is dropped.  If so we need to deassign the SCH.
    if ( mccsch_sch_is_active( MCCSCH_FORWARD, 0 ) )
    {
      // Generate the SCH active set.  This is the easiest way to find out if SCH
      // active set is dropped to 0
      // No need to check function return below since we know that SCH is active
      (void)mccccl_code_channel_list.generate_sch_aset( &sch_aset );
      if ( sch_aset.sch[0].num_pilots == 0 )
      {
        MSG_MED("Dropping last leg on F-SCH 0!  Deassigning SCH.", 0, 0, 0 );
        mccsch_stop_f_sch0_trans_now();
      }
    }
#endif /* FEATURE_IS2000_SCH */

    // Now continue with rest of handoff processing.
    do_successful_handoff_operations();
  }
 
} /* mcctcho_handle_ho_action_time_arrival */

/*===========================================================================

FUNCTION MCCTCHO_POST_PROCESS_HANDOFF

DESCRIPTION
  This function is called under the following conditions:

    1. After a successful hard handoff
    2. After a failed Return-If-Fail HHO that returns to the original
       configuration
    3. After a visit to the Candidate Frequency for a single/periodic CFS
    4. After a visit to GPS Frequency

  A Candidate Frequency Search Report Message is sent only for case 2.

  For case 3, the function does nothing and returns immediately, because
  a Candidate Frequency Search Report Message is not sent until all visits
  to to the Candidate Frequency are completed, i.e. when we receive the
  SRCH_CF_NSET_R report from SRCH

  For case 4, the function does nothing and returns immdediately.

DEPENDENCIES
  In the case of a successful hard handoff, this function needs to be
  called before re-starting the transmitter because it needs to first
  update the transmit power and set the number of preambles to be sent

RETURN VALUE
  None.

SIDE EFFECTS
  This function reverts MC back to its lower-than-SRCH priority

===========================================================================*/

void mcctcho_post_process_handoff
( 
  void
)
{

  #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
  #endif /* FEATURE_IS2000_REL_C_DV */

  /* Check for conditions where this function might get called but nothing
   * is to be done
   *   1. The receipt of RXC_TX_ON_R while action time is still pending.
   *      This is rare, but still possible
   */
  if ( action_time_pending )
  {
    /* Do nothing */
    return;
  }

  /* After a successful hard handoff or failed RIF hard handoff */
  if (mcctcho_ho_data.hard_handoff_in_progress)
  {
    if ( mcctcho_ho_data.hard_handoff_successful )
    {
      send_successful_hho_ind_to_lower_layers();
      if ( mcctcho_ho_data.ho_msg.extra_parms &&
           mcctcho_ho_data.ho_msg.extra.return_if_ho_fail )
      {
        // If service is included, need to do time critical part of update configuration here.
        // The rest happens in srv later when do_successful_handoff_operations is called.
        if (mcctcho_ho_data.ho_msg.extra.scr_included)
        {
          snm_update_configuration();
        }
        else if (mcctcho_ho_data.ho_msg.extra.nnscr_included)
        {
          /* Update NNSCR */
          snm_update_non_neg_configuration();
        }

        #if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
        /* Update SYNC ID info to be restored */
        mccsyncid_update_restore_info();
        #endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

#ifdef FEATURE_IS2000_SCH
        // If it is UHDM and f-sch assignment comes with this message, we have to
        // despatch transactions for it, for HHO-RIF case we wait until this point
        // i.e till HHO is successful
        mcctcho_start_sch_assignments(&mcctcho_ho_data.ho_msg);
#endif
      }

      // Replace the following with TXC_HHO_SUCCESS command IS2000 changes

      do_successful_handoff_operations();

      #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
      #endif /* FEATURE_IS2000_REL_C_DV */

      #ifdef FEATURE_IS2000_TDSO
      // Do necessary HHO processing if full TDSO call.
      tdso_notify_hho_success();
      #endif /* FEATURE_IS2000_TDSO */

      #ifdef FEATURE_AUTH
      /* If we want voice privacy but don't have it, request it */
      if ((cdma.vp_desired) && (!cdma.long_code.private_lcm_active))
      {
        send_plc_req_ord(CAI_PLC_REQ_PRI);
      }
      #endif /* FEATURE_AUTH */

    }
    else
    {
      cand_freq_srch_parm.last_srch_msg_seq = cdma.hdm_seq;
      send_cand_freq_srch_rpt_msg();

      /* Start a periodic CFS, if necessary */
      if ( (mcctcho_ho_data.ho_msg.extra_parms) &&
           ( mcctcho_ho_data.ho_msg.extra.periodic_srch ) &&
           ( ! cf_srch_set_empty() ) )
      {
        cand_freq_srch_parm.cur_srch_seq_num++;
        MSG_LOW("Increment cur_srch_seq_num to %d", cand_freq_srch_parm.cur_srch_seq_num, 0, 0);
        start_cf_srch( SRCH_PERIODIC_CFS );
      }
    }

    mcctcho_ho_data.hard_handoff_in_progress = FALSE;

    #ifdef FEATURE_GPS_MODE
#error code not present
    #endif /* FEATURE_GPS_MODE */

  }

  else if ( mcctcho_ho_data.cfs_in_progress )
  {
    if ( mcctcho_ho_data.pending_cfs_meas_rpt_to_be_sent )
    {
      /* Back to serving freq, able to measure SF pwr, send CFS response msg. */
      send_cand_freq_srch_rpt_msg();

      mcctcho_ho_data.pending_cfs_meas_rpt_to_be_sent = FALSE;
    }

    /* Mark the end of a visit to the Candidate Frequency.*/
    mcctcho_ho_data.cfs_in_progress = FALSE;
    MSG_LOW("cfs_in_progress is set to 0", 0, 0, 0);

#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */

  }

  /* If this function is called because of a visit to GPS frequency, it
   * returns immediately here.
   */

} /* mcctcho_post_process_handoff */

/*===========================================================================

FUNCTION MCCTCHO_PROCESS_FAILED_HHO

DESCRIPTION
  This function processes a failed Return-If-Fail HHO.  If a candidate
  frequency search is in progress and the COMPLETE_SRCH field in the GHDM
  is set to 1, this function returns immediately and only send the
  SRCH_RIF_HHO_FAILURE_F command upon receipt of the SRCH_CF_NSET_R report;
  otherwise, it sends the SRCH_RIF_HHO_FAILURE_F command immediately

DEPENDENCIES
  This function is called upon the receipt of the SRCH_ABORT_R report or
  the expiration of the handoff timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcctcho_process_failed_hho
(
  mcctcho_hho_failure_reason_type hho_failure_reason
)
{
  /*
  ** Check if we've already started failed HHO processing; if so, ignore.
  ** This can happen when we receive the SRCH_ABORT_R report after the
  ** handoff timer expires and vice versa
  */
  if ( ! mcctcho_ho_data.hard_handoff_successful )
  {
    return;
  }

  mcctcho_ho_data.hard_handoff_successful = FALSE;

  if ( hho_failure_reason == MCCTCHO_HANDOFF_TIMER_EXPIRE )
  {
    MSG_HIGH( "The handoff timer expires at %lx",
              txc_get_time_in_1_25ms_unit(), 0, 0 );
  }

  /* If we failed a frequency change hard handoff where TF==CF
   * and COMPLETE_SRCH is TRUE for some reason other than
   * DIFF_RX_PWR_TOO_LOW, and we have not received the SRCH_CF_NSET
   * report yet, then do not send the SRCH_RIF_HHO_FAILURE command
   * yet.  Instead, wait until we get the SRCH report.
   */
  if ( ( mcctcho_ho_data.tf_cdma_freq ==
           cand_freq_srch_parm.cf_cdma_freq ) &&
       ( mcctcho_ho_data.tf_band_class ==
           cand_freq_srch_parm.cf_band_class ) &&
       ( mcctcho_ho_data.ho_msg.extra.complete_srch ) &&
       ( hho_failure_reason != MCCTCHO_DIFF_RX_PWR_TOO_LOW ) )
  {
    /* Wait for SRCH_CF_NSET_R report */
    if ( ! mcctcho_ho_data.cf_srch_completed )
    {
      MSG_HIGH( "Wait for CF_NSET_R", 0, 0, 0 );
      return;
    }
  }

  mcc_srch_buf.hho_fail.hdr.cmd = SRCH_RIF_HHO_FAILURE_F;
  mcc_srch_cmd( &mcc_srch_buf );

} /* mcctcho_process_failed_hho */

/*===========================================================================

FUNCTION MCCTCHO_RETURN_TO_SERV_FREQ

DESCRIPTION
  This function performs the return to the original configuration after a
  failed Return-If-Fail HHO or a single visit to the Candidate Frequency

DEPENDENCIES
  In a case of a failed Return-If-Fail HHO, this function is called upon
  the receipt of the SRCH_RIF_HHO_FAILURE_R report.  In the case of a CFS,
  this function is called upon the receipt of the SRCH_CF_MEAS_DONE_R
  report or the SRCH_CF_NSET_R report

RETURN VALUE
  None

SIDE EFFECTS
  This function may tune the RF back to the Serving Frequency.  This
  function will restore configuration data that is saved at the beginning
  of the handoff

===========================================================================*/

/*lint -e818*/
void mcctcho_return_to_serv_freq
(
  mccsrch_rpt_type *rpt_ptr
)
{
  qword curr_time;
  byte i, j;
  byte num_pilots;

  MSG_HIGH( "Returning to the SF at %lx",
            txc_get_time_in_1_25ms_unit(), 0, 0 );

  /* The above params can be changed in the restore_serving_
  config_parms() function */

  if (mcctcho_ho_data.hard_handoff_in_progress)
  {
    /* Restore the SF configuration */
    restore_serving_config_parms();
  }

  ts_get_20ms_frame_time( curr_time );
  mcctxns_begin_return_trans( curr_time);


  /* Save the information from the report only after a failed HHO
   * For a returned Candidate Frequency visit, restore MC priority
   */
  if ( mcctcho_ho_data.hard_handoff_in_progress )
  {

    /* Save the TF information from the report */
    mcctcho_ho_data.tf_tot_rx_pwr = rpt_ptr->hho_fail.total_rx_pwr;
    if ( mcctcho_ho_data.cf_srch_results.num_pilots == 0 )
    {
      mcctcho_ho_data.cf_srch_results.num_pilots = rpt_ptr->hho_fail.pilot_cnt;
      for ( i = 0; i < mcctcho_ho_data.cf_srch_results.num_pilots; i++ )
      {
        mcctcho_ho_data.cf_srch_results.pilot[i].pilot_pn =
          rpt_ptr->hho_fail.pilot_rec[i].pilot;
        mcctcho_ho_data.cf_srch_results.pilot[i].ecio =
          rpt_ptr->hho_fail.pilot_rec[i].ecio;
      }
    }
    else
    {
      /* Merge the pilot information in SRCH_CF_NSET_R and SRCH_RIF_HHO_FAILURE_R
       * Note that in the case of duplicate pilot, the data in
       * SRCH_RIF_HHO_FAILURE_R is used since it is more recent
       */
      num_pilots = (uint8)mcctcho_ho_data.cf_srch_results.num_pilots;
      for ( i = 0; ((i < rpt_ptr->hho_fail.pilot_cnt) && (i < SRCH_ASET_MAX)); i++ )
      {
        for ( j = 0; j < mcctcho_ho_data.cf_srch_results.num_pilots; j++ )
        {
          if ( rpt_ptr->hho_fail.pilot_rec[i].pilot ==
               mcctcho_ho_data.cf_srch_results.pilot[j].pilot_pn
             )
          {
            mcctcho_ho_data.cf_srch_results.pilot[j].ecio =
              rpt_ptr->hho_fail.pilot_rec[i].ecio;
            break;
          }
        }
        if ( j == mcctcho_ho_data.cf_srch_results.num_pilots )
        {
          /* New pilot */
          mcctcho_ho_data.cf_srch_results.pilot[num_pilots].pilot_pn =
            rpt_ptr->hho_fail.pilot_rec[i].pilot;
          mcctcho_ho_data.cf_srch_results.pilot[num_pilots].ecio =
            rpt_ptr->hho_fail.pilot_rec[i].ecio;
          num_pilots++;
        }
      }
      mcctcho_ho_data.cf_srch_results.num_pilots = num_pilots;
    }

    /* Event report: Unsuccessful hard handoff with return on failure */
    if (mcctcho_ho_data.ho_msg.extra_parms &&
      mcctcho_ho_data.ho_msg.extra.return_if_ho_fail)
    {
      event_report(EVENT_UNSUCCESSFUL_HARD_HANDOFF);
    }

    /* Inform SD about HO failure and update its parameters */ 
    sd_ss_ind_cdma_opr_hdm_cdma ((sd_band_e_type)cdma.band_class, cdma.cdmach, NULL);
  }
 
} /* mcctcho_return_to_serv_freq */
/*lint +e818*/

/*===========================================================================

FUNCTION MCCTCHO_PROCESS_CAND_FREQ_SRCH_REQ_MSG

DESCRIPTION
  This function processes a Candidate Frequency Search Request Message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void mcctcho_process_cand_freq_srch_req_msg
(
  caii_cfs_req_type *cfs_req
    /* Pointer to message received from the base station */
)
{
  qword abs_action_time;
  mcctcho_cand_freq_srch_off_time_type off_times;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

  if ( ! cfs_req->fix.pre_proc_done )
  {
    /* Check for analog search */
    if ( cfs_req->fix.srch_mode != 0 )
    {
      MSG_HIGH( "Analog search not supported", 0, 0, 0 );
      rej_parms.rej_msg_type = CAI_CF_SRCH_REQ_MSG;
      rej_parms.rej_code = CAI_REJ_CAP;
      send_rej_order( &rej_parms);
      return;
    }

    if (( cfs_req->fix.srch_type == SRCH_RSV_CFS ) ||
        ( cfs_req->fix.srch_type > SRCH_PERIODIC_CFS ))
    {
      MSG_HIGH( "Invalid value for SEARCH_TYPE in CFSRQM", 0, 0, 0 );
      rej_parms.rej_msg_type = CAI_CF_SRCH_REQ_MSG;
      rej_parms.rej_code = CAI_REJ_FIELD;
      send_rej_order( &rej_parms);
      return;
    }

    if ( ( cfs_req->fix.srch_type == SRCH_SINGLE_CFS ) ||
         ( cfs_req->fix.srch_type == SRCH_PERIODIC_CFS ) ||
		 ( cfs_req->fix.srch_type == SRCH_NO_CFS ) )
    {
      /* Check for unsupported band */
      if (!mcc_is_band_class_supported(cfs_req->var.mod0.band_class))
      {
        MSG_HIGH( "Unsupported band class", 0, 0, 0 );
        rej_parms.rej_msg_type = CAI_CF_SRCH_REQ_MSG;
        rej_parms.rej_code = CAI_REJ_CAP;
        send_rej_order( &rej_parms);
        return;
      }

      /* Check for invalid frequency assignment */
      if ( ( cdma.band_class == cfs_req->var.mod0.band_class ) &&
           ( cdma.cdmach == cfs_req->var.mod0.cdma_freq ) )
      {
        MSG_HIGH( "Invalid frequency assignment", 0, 0, 0 );
        rej_parms.rej_msg_type = CAI_CF_SRCH_REQ_MSG;
        rej_parms.rej_code = CAI_REJ_IFREQ;
        send_rej_order( &rej_parms);
        return;
      }

      /* Check for search set not specified */
      if ( cfs_req->var.mod0.pilot_update )
      {
        if ( cfs_req->var.mod0.num_pilot == 0 )
        {
          MSG_HIGH( "Search set not specified", 0, 0, 0 );
          rej_parms.rej_msg_type = CAI_CF_SRCH_REQ_MSG;
          rej_parms.rej_code = CAI_REJ_SRCH;
          send_rej_order( &rej_parms);
          return;
        }
      }
      else
      {
        if ( cf_srch_set_empty() )
        {
          MSG_HIGH( "Empty search set", 0, 0, 0 );
          rej_parms.rej_msg_type = CAI_CF_SRCH_REQ_MSG;
          rej_parms.rej_code = CAI_REJ_SRCH;
          send_rej_order( &rej_parms);
          return;
        }
      }
    }

    /* Get new total/maximum off times if we got a new Candidate Frequency
     * Search Set
     */
    if ( cfs_req->var.mod0.pilot_update )
    {
      cf_srch_get_off_times( cfs_req, &off_times );
    }

    /* Check for search period too short */
    if ( cfs_req->fix.srch_type == SRCH_PERIODIC_CFS )
    {
      if ( cfs_req->var.mod0.pilot_update )
      {
        /* Use new off times */
        if ( cf_srch_period_too_short( cfs_req->fix.srch_period,
                                       off_times.tot_off_time_fwd,
                                       off_times.tot_off_time_rev ) ) //lint !e644
        {
          MSG_HIGH( "Search period too short", 0, 0, 0 );
          rej_parms.rej_msg_type = CAI_CF_SRCH_REQ_MSG;
          rej_parms.rej_code = CAI_REJ_TSHT;
          send_rej_order( &rej_parms);
          return;
        }
      }
      else
      {
          /* Use off times from previous CFSReqM */
        if ( cf_srch_period_too_short(
               cfs_req->fix.srch_period,
               cand_freq_srch_parm.off_times.tot_off_time_fwd,
               cand_freq_srch_parm.off_times.tot_off_time_rev ) )
        {
          MSG_HIGH( "Search period too short", 0, 0, 0 );
          rej_parms.rej_msg_type = CAI_CF_SRCH_REQ_MSG;
          rej_parms.rej_code = CAI_REJ_TSHT;
          send_rej_order( &rej_parms);
          return;
        }
      }
    }

    #ifdef FEATURE_GPS_MODE
#error code not present
    #endif /* FEATURE_GPS_MODE */

    /* Message didn't get rejected.  Update total/maximum off times if we got
     * a new Candidate Frequency Search Set
     */
    if ( cfs_req->var.mod0.pilot_update )
    {
      cand_freq_srch_parm.off_times = off_times;
    }

    /* Send a Candidate Frequency Search Response Message as soon as we're
     * done checking all the reject conditions and calculating all the off
     * times
     */
    send_cand_freq_srch_resp_msg( cfs_req->fix.cfsrm_seq );

    /* Always abort any periodic search in progress */
    if ( cand_freq_srch_parm.cur_srch_type == SRCH_PERIODIC_CFS )
    {
      stop_cf_srch();
    }
  }

    /*  We only need to setup the transaction when srch_type is single_cfs
    **  or periodic_cfs.  Otherwise, No search should be started */

  if ( !cfs_req->fix.use_time )
  {
    cfs_req->fix.action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
  }
  get_abs_action_time( abs_action_time, cfs_req->fix.action_time);

  if (!cfs_req->fix.use_time)
  {
    /* For implicit action time, we still want to give time for CFS Rsp to go out
       (within T56 i.e. 200ms) so delay start of this txn by 3 frames
       Doing this is no guarantee that the Resp will indeed go out. 
       Note :  we are still std compliant because an implicit AT txn should
       start within 80ms of the message. */
    qw_inc( abs_action_time,3);
  }

  mcctxns_begin_cfs_trans( abs_action_time, (mctrans_pending_action_msg_type *) cfs_req); /*lint !e826 */
 
} /* mcctcho_process_cand_freq_srch_req_msg */

/*===========================================================================

FUNCTION MCCTCHO_UPDATE_CFS_PARAMS

DESCRIPTION
  This function updates CFS parameters locally stored in MC based on the
  CFS Req or the CFS Ctrl message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void mcctcho_update_cfs_params
(
 const mctrans_pending_action_msg_type *cfs_msg_ptr
)
{
  switch (cfs_msg_ptr->gen.msg_type)
  {
    case CAI_CF_SRCH_REQ_MSG:
    {
      /* Save only those CFSReqM parameters that will be used in MC.
       * Others are sent to SRCH
       */
      cand_freq_srch_parm.cf_band_class = cfs_msg_ptr->cfs_req.var.mod0.band_class;
      cand_freq_srch_parm.cf_cdma_freq = cfs_msg_ptr->cfs_req.var.mod0.cdma_freq;
      cand_freq_srch_parm.tf_wait_time = cfs_msg_ptr->cfs_req.var.mod0.tf_wait_time;
      cand_freq_srch_parm.num_pilots = cfs_msg_ptr->cfs_req.var.mod0.num_pilot;

      cand_freq_srch_parm.srch_period = cfs_msg_ptr->cfs_req.fix.srch_period;

      /* Save the CFSRM_SEQ.  This is used to set the LAST_SRCH_MSG_SEQ field
       * of the Candidate Frequency Search Report Message
       */
      cand_freq_srch_parm.last_srch_msg_seq = cfs_msg_ptr->cfs_req.fix.cfsrm_seq;

      /* Increment the sequence number.  This will serve to identify the new
       * search request
       */
      cand_freq_srch_parm.cur_srch_seq_num++;
      MSG_LOW("Increment cur_srch_seq_num to %d", cand_freq_srch_parm.cur_srch_seq_num, 0, 0);

      if (!mcctcho_ho_data.hard_handoff_in_progress) 
      {
        start_cf_srch( cfs_msg_ptr->cfs_req.fix.srch_type );
      }
    }
    break;

    case CAI_CFS_CTRL_MSG:
    {
      /* Save the CFSCM_SEQ.  This is used to set the LAST_SRCH_MSG_SEQ field
       * of the Candidate Frequency Search Report Message
       */
      cand_freq_srch_parm.last_srch_msg_seq = cfs_msg_ptr->cfs_ctrl.cfscm_seq;

      /* Increment the sequence number.  This will serve to identify the new
       * search request
       */
      cand_freq_srch_parm.cur_srch_seq_num++;
      MSG_LOW("Increment cur_srch_seq_num to %d", cand_freq_srch_parm.cur_srch_seq_num, 0, 0);

      if (!mcctcho_ho_data.hard_handoff_in_progress) 
      {
        start_cf_srch( cfs_msg_ptr->cfs_ctrl.srch_type );
      }
    }
    break;

  } //lint !e744 Some enums are not used in this switch
 
} /* mcctcho_update_cfs_params */

/*===========================================================================

FUNCTION MCCTCHO_PROCESS_CAND_FREQ_SRCH_CTRL_MSG

DESCRIPTION
  This function processes a Candidate Frequency Search Control Message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void mcctcho_process_cand_freq_srch_ctrl_msg
(
  caii_cfs_ctrl_type * cfs_ctrl
    /* Pointer to message received from the base station */
)
{
  qword abs_action_time;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  /* Process action time */
  if ( ! cfs_ctrl->pre_proc_done )
  {
    /* Check for reject reasons */

    if (( cfs_ctrl->srch_type == SRCH_RSV_CFS ) ||
        ( cfs_ctrl->srch_type > SRCH_PERIODIC_CFS ))
    {
      MSG_HIGH( "Invalid value for SEARCH_TYPE in CFSCNM", 0, 0, 0 );
      rej_parms.rej_msg_type = CAI_CFS_CTRL_MSG;
      rej_parms.rej_code = CAI_REJ_FIELD;
      send_rej_order( &rej_parms);
      return;
    }

    if ( ( cfs_ctrl->srch_type == SRCH_SINGLE_CFS ) ||
         ( cfs_ctrl->srch_type == SRCH_PERIODIC_CFS ) )
    {
      /* Check for invalid search request */
      if ( ( cdma.band_class == cand_freq_srch_parm.cf_band_class ) &&
           ( cdma.cdmach == cand_freq_srch_parm.cf_cdma_freq ) )
      {
        rej_parms.rej_msg_type = CAI_CFS_CTRL_MSG;
        rej_parms.rej_code = CAI_REJ_ISRCH;
        send_rej_order( &rej_parms);
        return;
      }

      /* Check for search set not specified */
      if ( cf_srch_set_empty() )
      {
        rej_parms.rej_msg_type = CAI_CFS_CTRL_MSG;
        rej_parms.rej_code = CAI_REJ_SRCH;
        send_rej_order( &rej_parms);
        return;
      }
    }

    /* Check for search period too short */
    if ( cfs_ctrl->srch_type == SRCH_PERIODIC_CFS )
    {
      if  ( cf_srch_period_too_short(
              cand_freq_srch_parm.srch_period,
              cand_freq_srch_parm.off_times.tot_off_time_fwd,
              cand_freq_srch_parm.off_times.tot_off_time_rev ) )
      {
        rej_parms.rej_msg_type = CAI_CFS_CTRL_MSG;
        rej_parms.rej_code = CAI_REJ_TSHT;
        send_rej_order( &rej_parms);
        return;
      }
    }

    #ifdef FEATURE_GPS_MODE
#error code not present
    #endif /* FEATURE_GPS_MODE */

    /* Always abort any periodic search in progress */
    if ( cand_freq_srch_parm.cur_srch_type == SRCH_PERIODIC_CFS )
    {
      stop_cf_srch();
    }

    if (cfs_ctrl->srch_type != SRCH_NO_CFS) 
    {
      /*  We only need to setup the transaction when srch_type is single_cfs
      **  or periodic_cfs.  Otherwise, No search should be started */
      if ( !cfs_ctrl->use_time )
      {
        cfs_ctrl->action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
      }
      get_abs_action_time( abs_action_time, cfs_ctrl->action_time);
      mcctxns_begin_cfs_trans(abs_action_time, (mctrans_pending_action_msg_type *)cfs_ctrl); /*lint !e826 */
    }
  }
 
} /* mcctcho_process_cand_freq_srch_ctrl_msg */

/*===========================================================================

FUNCTION MCCTCHO_START_CF_MEAS

DESCRIPTION
  This function starts a visit to the Candidate Frequency

DEPENDENCIES
  This function is called upon the receipt of the SRCH_CFS_READY_R report


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

/*lint -e818*/
void mcctcho_start_cf_meas
(
  mccsrch_rpt_type *rpt_ptr
    /* Pointer to the SRCH_CFS_READY_R report received from SRCH */
)
{
  qword cfs_action_time;
  /* Check for old CFS report */
  if ( ! cf_srch_current( rpt_ptr->cfs_ready_rpt.cfs_seq ) )
  {
    return;
  }

  /* This check is used to handle the race condition when MC is asked to do a
   * Candidate Frequency visit while waiting for an explicit action time for a
   * soft HO (For HHO, we should never get here, because we stop any periodic
   * search in progress immediately without waiting for action time).  When
   * this happens, MC simply ignores the request and stops periodic search
   * altogether.  Note that the standard specifies all the conditions in which
   * the mobile shall stop periodic search and soft HO is not one of the
   * conditions, so our solution here is actually standard non-compliant.
   * However, given the assumption that soft HO always has implicit action
   * time, the probability of encountering the race condition should be
   * minimal, and the solution is straight-forward and least intrusive.
   */
  if ( action_time_pending )
  {
    MSG_MED( "Race condition detected!! Stopping periodic search", 0, 0, 0 );

    stop_cf_srch();

    /* Restart the periodic search after the race condition is resolved */
    mcctcho_ho_data.restart_periodic_srch = TRUE;

    return;
  }

  #ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */

  mcctcho_ho_data.cfs_in_progress = TRUE;
  MSG_LOW("cfs_in_progress is set to 1", 0, 0, 0);

  get_abs_action_time( cfs_action_time, MCCTCHO_IMPLICIT_ACTION_TIME);
  /* Save the current configuration */
  save_serving_config_parms();

  /* Mark that TXC is to stop transmitting */
  cdma.tc_tx_to_be_on = FALSE;
  mcctxns_begin_cfs_gps_prep_trans(cfs_action_time);

#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif
 
} /* mcctcho_start_cf_meas */
/*lint +e818*/

/*===========================================================================

FUNCTION MCCTCHO_PROCESS_SRCH_CF_NSET_RPT

DESCRIPTION
  This function processes the SRCH_CF_NSET_R report

DEPENDENCIES
  This function is called upon the receipt of the SRCH_CF_NSET_R report

RETURN VALUE
  None

SIDE EFFECTS
  See description

===========================================================================*/

void mcctcho_process_srch_cf_nset_rpt
(
  mccsrch_rpt_type *rpt_ptr
)
{
  byte i;

  /* Check for old CFS report.  If a hard handoff is in progress, the
   * report is always current
   */
  if ( ( mcctcho_ho_data.hard_handoff_in_progress ) ||
       ( cf_srch_current( rpt_ptr->cfnset_meas_rpt.cfs_seq ) ) )
  {
    mcctcho_ho_data.tf_tot_rx_pwr = rpt_ptr->cfnset_meas_rpt.total_rx_pwr;
    mcctcho_ho_data.cf_srch_results.num_pilots =
      rpt_ptr->cfnset_meas_rpt.pilot_cnt;
    for ( i = 0; i < mcctcho_ho_data.cf_srch_results.num_pilots; i++ )
    {
      mcctcho_ho_data.cf_srch_results.pilot[i].pilot_pn =
        rpt_ptr->cfnset_meas_rpt.pilot_rec[i].pilot;
      mcctcho_ho_data.cf_srch_results.pilot[i].ecio =
        rpt_ptr->cfnset_meas_rpt.pilot_rec[i].ecio;

      #ifdef FEATURE_IS2000_REL_A
      mcctcho_ho_data.cf_srch_results.pilot[i].pilot_rec_type =
        rpt_ptr->cfnset_meas_rpt.pilot_rec[i].pilot_rec_type;
      mcctcho_ho_data.cf_srch_results.pilot[i].pilot_rec =
        rpt_ptr->cfnset_meas_rpt.pilot_rec[i].pilot_rec;
      #endif /* FEATURE_IS2000_REL_A */
    }
  }
  else
  {
    if ((cand_freq_srch_parm.cur_srch_type == SRCH_NO_CFS) &&
        mcctcho_ho_data.cfs_in_progress) 
    {
      /* A (E|G|U)HDM might have aborted the current
       * periodic search just when a SRCH report gets queued.  
       * Tell the search to return to the serving frequency.
       */
      mcctcho_return_to_serv_freq( rpt_ptr );
    }
    return;
  }

  if ( mcctcho_ho_data.hard_handoff_in_progress )
  {
    mcctcho_ho_data.cf_srch_completed = TRUE;

    /* If the successful HHO flag is still TRUE, either the HHO is already
     * successful or it hasn't failed yet.  In both cases, we don't have to
     * do any further processing
     */
    if ( ( ! mcctcho_ho_data.hard_handoff_successful ) &&
             mcctcho_ho_data.ho_msg.extra.complete_srch )
    {
      /* The hard handoff has failed and SRCH has now reported the results of
       * its search of the candidate frequency search set.  SRCH does not
       * send this report if TF!=CF or if the failure was due to
       * DIFF_RX_PWR_TOO_LOW, which means TF is equal to CF and the
       * failure was due to a timeout or TOTAL_PILOT_EC_IO_TOO_LOW.  If
       * COMPLETE_SRCH is TRUE, then we were waiting for this report before
       * informing SRCH of the handoff failure, so we should do so now.
       * If COMPLETE_SRCH is FALSE then we would have returned already if the
       * handoff failed, so we can just ignore the report
       */
      mcc_srch_buf.hho_fail.hdr.cmd = SRCH_RIF_HHO_FAILURE_F;
      mcc_srch_cmd( &mcc_srch_buf );
    }
  }
  else
  {
    mcctcho_return_to_serv_freq( rpt_ptr );

    mcctcho_ho_data.tf_band_class = cand_freq_srch_parm.cf_band_class;
    mcctcho_ho_data.tf_cdma_freq = cand_freq_srch_parm.cf_cdma_freq;

    /* Wait till MS returns to serving freq and able to measure the SF PWR 
       before packing the CFS response Msg. */
    mcctcho_ho_data.pending_cfs_meas_rpt_to_be_sent = TRUE;

    if ( cand_freq_srch_parm.cur_srch_type == SRCH_SINGLE_CFS )
    {
      /* We are done!! */
      cand_freq_srch_parm.cur_srch_type = SRCH_NO_CFS;
    }
  }
 
} /* mcctcho_process_srch_cf_nset_rpt */

#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */

/*===========================================================================

FUNCTION MCCTCHO_DIFFERENT_LONG_CODE_MASK

DESCRIPTION
  This function determines if the long code mask specified in the handoff
  message is different from the one we are currently using.  Note that MEID 
  UHDM, UHDM, GHDM and EHDM can switch between the private and public long 
  code masks.  UHDM and MEID UHDM can switch between public long code mask 
  types.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if handoff message will change long code mask
  FALSE if handoff message will not change long code mask

SIDE EFFECTS
  None.

===========================================================================*/
boolean mcctcho_different_long_code_mask
(
  caii_ho_msg_type *ho_msg /*lint -esym(715, ho_msg)*/
)
{
  boolean long_code_mask_changing = FALSE;

  /* P-rev 9 UHDM and MEID UHDM can change the public long code mask type
     that the mobile is using. */
  #if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_MEID_SUPPORT)
  if (
      #ifdef FEATURE_IS2000_REL_C
      ((ho_msg->msg_type == CAI_UNIV_HO_DIR_MSG) && P_REV_IS_9_OR_GREATER)
      #endif /* FEATURE_IS2000_REL_C */

      #if defined(FEATURE_IS2000_REL_C) && defined(FEATURE_MEID_SUPPORT)
      ||
      #endif /* FEATURE_IS2000_REL_C && FEATURE_MEID_SUPPORT */

      #ifdef FEATURE_MEID_SUPPORT
      (ho_msg->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
      #endif /* FEATURE_MEID_SUPPORT */
     )
  {
    if (ho_msg->plcm_type_incl)
    {

      #ifdef FEATURE_AUTH
      /* Normally the base station turns off privacy explicitly, by including
         private_lcm and setting it to 0.  That case is handled below.  But
         if the base station did not include private_lcm, and plcm_type_incl 
         is 1, and the mobile is using privacy, the mobile switches to public. */
      if (!ho_msg->extra_parms && cdma.long_code.private_lcm_active)
      {
        long_code_mask_changing = TRUE;
      }
      else
      #endif /* FEATURE_AUTH */

        /* For bs-specified plcm, check if the mobile is not currently using 
           bs-specified plcm, and also check if the plcm_39 is changing */
      if (ho_msg->plcm_type == CAI_PLCM_TYPE_BS_SPEC)  /*lint !e641 */
      {
        if ((cdma.long_code.public_lcm_type != CAI_PLCM_TYPE_BS_SPEC)
            || (qw_cmp(ho_msg->plcm_39, cdma.long_code.bs_plcm_39) != 0))
        {
          long_code_mask_changing = TRUE;
        }
      }
      /* Check if the public long code mask type specified in the message is
         different from what the mobile is currently using. */
      else if (ho_msg->plcm_type != cdma.long_code.public_lcm_type)  /*lint !e641 */
      {
        long_code_mask_changing = TRUE;
      }
    }
  }
  #endif /* FEATURE_IS2000_REL_C || FEATURE_MEID_SUPPORT */

  #ifdef FEATURE_AUTH
  /* The bs can turn privacy on or off by including the private_lcm field.
     Check if the mobile needs to switch from public to private or vica versa. */
  if (ho_msg->extra_parms)
  {
    if (cdma.long_code.private_lcm_active != ho_msg->extra.private_lcm)
    {
      long_code_mask_changing = TRUE;
    }
  }
  #endif /* FEATURE_AUTH */

  return(long_code_mask_changing);
 
} /* mcctcho_different_long_code_mask */ /*lint !e715*/

/*===========================================================================

FUNCTION MCCTCHO_UPDATE_LONG_CODE_MASK_INFO

DESCRIPTION
  This function updates our internal long code mask information based
  on a handoff direction message (EHDM, GHDM, UHDM, MEID UHDM).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctcho_update_long_code_mask_info
(
  caii_ho_msg_type * ho_msg
)
{
  #ifdef FEATURE_AUTH
  if (ho_msg->extra_parms)
  {
    if (cdma.long_code.private_lcm_active != ho_msg->extra.private_lcm)
    {
      /* The mobile is switching from private to public, or vica versa. */
      cdma.long_code.private_lcm_active = ho_msg->extra.private_lcm;
    }
  }
  #endif /* FEATURE_AUTH */

  #if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_MEID_SUPPORT)
  if (
      #ifdef FEATURE_IS2000_REL_C
      ((ho_msg->msg_type == CAI_UNIV_HO_DIR_MSG) && P_REV_IS_9_OR_GREATER)
      #endif /* FEATURE_IS2000_REL_C */

      #if defined(FEATURE_IS2000_REL_C) && defined(FEATURE_MEID_SUPPORT)
      ||
      #endif /* FEATURE_IS2000_REL_C && FEATURE_MEID_SUPPORT */

      #ifdef FEATURE_MEID_SUPPORT
      (ho_msg->msg_type == CAI_MEID_UNIV_HO_DIR_MSG)
      #endif /* FEATURE_MEID_SUPPORT */
     )
  {
    if (ho_msg->plcm_type_incl)
    {
      /* Update the public long code mask type. */
      cdma.long_code.public_lcm_type = (cai_plcm_type)ho_msg->plcm_type;

      if (ho_msg->plcm_type == CAI_PLCM_TYPE_BS_SPEC)  /*lint !e641 */
      {
        /* If the plcm_type is bs-specified plcm, update the plcm_39 */
        qw_set(cdma.long_code.bs_plcm_39, qw_hi(ho_msg->plcm_39), 
               qw_lo(ho_msg->plcm_39));
      }
      #ifdef FEATURE_AUTH
      /* Normally the base station turns off privacy explicitly, by including
         private_lcm and setting it to 0.  That case is handled above.  But
         if the base station did not include private_lcm, and plcm_type_incl 
         is 1, and the mobile is using privacy, the mobile switches to public. */
      if (!ho_msg->extra_parms && cdma.long_code.private_lcm_active)
      {
        cdma.long_code.private_lcm_active = FALSE;
      }
      #endif /* FEATURE_AUTH */

    }
  }
  #endif /* FEATURE_IS2000_REL_C || FEATURE_MEID_SUPPORT */
 
} /* mcctcho_update_long_code_mask_info */ /*lint !e715*/

/*===========================================================================

FUNCTION MCCTCHO_IS_HHO_IN_PROGRESS

DESCRIPTION
  This function determines if a HHO is in progress.

DEPENDENCIES
  None

RETURN VALUE
  True, if HHO is in progress.  False otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcctcho_is_hho_in_progress( void )
{
  return mcctcho_ho_data.hard_handoff_in_progress;

} /* mcctcho_is_hho_in_progress */

