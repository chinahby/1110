/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C D M A   T R A F F I C   C H A N N E L   S T A T E
                    S U P P O R T   R O U T I N E S

GENERAL DESCRIPTION
  This module contains support routines for the CDMA Sub-system,
  Mobile Station Control on the Traffic Channel state.

EXTERNALIZED FUNCTIONS

-- Service Option Related Routines --
  TC_SO_CTL - Processes received Service Option (SO) Control Order
  TC_SRV_CTL_MSG - Processes received Service Option Control Message
  TC_SO_CHANGE - Change Service Options
  TC_SO_REQ - Processes Service Option Request Message
  TC_SO_RESP - Processes Service Option Response Message
  SO_CONV - Performs SO actions for transition to Conversation State
  SO_WFA - Performs SO actions for transition to Waiting For Answer State
  MCC_AUTO_ANSWER - Performs an auto-answer if a loopback or markov SO
                    becomes active

-- Authentication Related Routines (FEATURE_AUTH only) --
  PROCESS_PRIVACY_PREF - Process CM privacy preference command
  SEND_AUTH_RESP_MSG - Sends Authentication Response Message
  SEND_SSD_UPDATE_ORD - Sends SSD Update Order
  SEND_PARAM_UPDATE_ORD - Sends Parameter Update Order
  START_SSD_UPDATE - Starts SSD Update
  START_AUTH_SIGNATURE - Commands AUTH to do Auth Signature
  TC_AUTH_RPT - Processes a report from the AUTH task

-- Other Message Transmission Routines --
  SEND_REJ_ORDER - Sends a reject order
  SEND_ORIG_C_MSG - Sends Origination Continuation Message
  SEND_STOP_DTMF - Sends Stop DTMF Order
  SEND_STATUS_MSG - Sends Status Message
  SEND_STATUS_RSP_MSG - Sends Status Response Message
  SEND_PARM_RESP - Sends Parameter Response Message
  SEND_TC_DATA_BURST - Sends a Data Burst Message
  TC_SEND_FLASH_INFO_MSG - sends a flash with info messge on TC

-- Sending Commands to Other Tasks --
  STOP_TX - Sends command to transmit task to turn off transmitter
  START_TX - Sends command to transmit task to turn on transmitter
  TC_INCOMING_CALL - Sends Incoming Call Notification to UI and DS
  TC_STOP_ALERT - Sends command to UI to stop ringing
  REQ_PIL_STR - Sends command to SRCH to make SRCH report pilot strengths
  TC_INFORM_SCR_CHANGE - Sends SCR information to TXC and RXC
  TC_BLOCK_SRCH_GPS_VISIT - Block/unblock SRCH's GPS visit request

-- Processing Commands/Reports from Other Tasks --
  START_CONT_DTMF - Processes UI request to send Continuous DTMF Order
  STOP_CONT_DTMF - Processes UI request to stop sending Continuous DTMF Order
  BURST_DTMF - Processes UI request to send Burst DTMF Order
  TC_RX_RPT - Processes reports from the Recrive Task
  TC_SRCH_RPT - Processes reports from the Searcher Task
  TC_TRANS_DONE_RPT - Processes transaction done report from transaction servers

-- Processing Received Over-The-Air Messages --
  PROCESS_PWR_MSG - Processes Power Control Parameters Message
  TC_PROC_ACK - Processes received acknowledgements
  TC_LOCK_ORD - Processes Lock or Maintenance Order
  TC_MOB_REG - Processes Mobile Station Registered Message
  PROCESS_FTC_TMSI_ASSIGN - Processes TMSI Assignment Message
  PROCESS_TC_DATA_BURST - Processes Data Burst Message
  PROCESS_NLU_MSG - Processes Neighbor List Update Message
  PROCESS_SP_MSG - Processes In-Traffic System Parameters Message
  TC_SET_PARM - Processes Set Parameters Message
  TC_ACTION_CHK - Checks the action time of a received message
  PROCESS_PWR_CNTL_STEP - Processed received power control step.
  PROCESS_PWR_CTRL_MSG - Processes the enhanced power control message.


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1990-2010 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctcsup.c_v   1.64   03 Oct 2002 15:46:44   phosabet  $
$Header: //depot/asic/qsc1100/modem/1x/cp/src/mcctcsup.c#2 $ $DateTime: 2010/11/25 07:53:16 $ $Author: sshahi $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/10   ssh     Added processing of SMCM with use_time as TRUE.
09/01/09   ag      Added support for sending neighbor search mode to searcher
07/29/09   ag      Calling sd_ss_ind_misc_orig_success only for MO call.
06/01/09   ag      Featurized otasp session status under FEATURE_OTASP_STATUS
05/29/09   ag      Added support for sending OTASP session status to CM
05/13/09   pk      Fixing compiler warnings
01/13/09   ag      Added support for passing BS ack for connect order to CM under
                   FEATURE_JCDMA_2.
10/14/08   pk      Fixed the issue of wrong reject code specified in reject
                   order message for CLAM.
09/13/08   pk      Fix compilation issues
07/24/08   bb      Fix compilcation issues with MVS migration feature
07/21/08   ag      Fixed compiler warning.
07/14/08   ag      Stopped processing SOCM when it is rejected.
06/30/08   ag      Added code for rejecting SOCM for SO-70 with reserved values
                   of RATE_REDUC field.
05/15/08   bb      Changes for "MVS API Migration" feature
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
03/20/08   bb      Fixed klocwork errors
12/14/07   an      Added Passport Mode
07/11/07   an      Fixed linking errors
06/11/07   pg      Lint Cleanup: Remove the first 2 arguments from 
                   mccccim_is_clam_so_compatible().  They are not used.
06/07/07   pg      Lint clean up.
05/11/07   pg      More Lint Cleanup.
03/19/07   pg      Lint Cleanup.
03/09/07   an      Put FEATURE_GPSONE featurization
03/09/07   pg      removed dependency on WMS and make sure it only depends on 
                   CM for SMS.
03/08/07   an      Fixed Critical Lint Errors
02/28/07   pg      Featurization changes to support C-GPS code.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
09/20/06   an      Fixed dereferencing NULL ptr issue
08/16/06   pg      Added support for release D Packet Zone Hysteresis.
07/11/06   fc      Updated function prototype for mcc_send_cm_srv_sys_info.
07/06/06   fc      Renamed SRCH_LOST_RF_RESOURCE_R to 
                   SRCH_REQUEST_SYS_RESTART_R.
06/23/06   pg      RPC cleanup.
06/12/06   fc      Added the support to share the setting of mob_qos in idle 
                   and traffic state.
05/18/06   fc      Added support for SRCH_LOST_RF_RESOURCE_R.
05/17/06   fh      Merged in new development of AKA/AES features
04/24/06   fc      Fixed Rel A featurization.
04/24/06   an      Fixed lint errors
03/21/06   fc      Fixed the issue of reporting incorrect burst type for SDB
                   failure. 
03/15/06   fc      Fixed the issue of wrong reject code specified in reject
                   order message for CLAM.
03/06/06   fh      Added AES support
01/20/06   fc      Merged the following :
           fh      Added support for AKA.
12/20/05   sb      Replaced cm_mc_rpt_free_q with cm_mc_rpt_get_buf_else_err_fatal().
11/10/05   va      Checked for NULL pointer with mctran_get_pending_msg
10/10/05   fh      Removed including of enc.h
09/29/05   sb      Replaced DB_VOICE_PRIVACY with report to CM.
09/15/05   fc      Fixed lint errors.
08/20/05   va      Made tc_inform_scr_change_init_service external
07/24/05   sb      Removed unused variable.
07/14/05   va      Changed tc_block_srch_gps_visit to make a SRCH fn call as 
                   opposed to setting the SRCH signal in MC file.
07/11/05   fh      Cleaned up header file including from MUX:
                     changed txc.h to txcmc.h
                     changed rxc.h to rxcmc.h
07/07/05   va      Merged from MSMSHARED:
07/05/05   sb       Added MEID support.
06/29/05   an      Added check for DF SDB bit on receving SDB in TC
05/03/05   fc      Merged the following from MSMSHARED:
04/27/05   an       Added support for 4GV service option
04/25/05   fc      Merged the following from MSMSHARED:
04/20/05   fc       Added support of reporting L2 failure for pending EOM.
03/24/05   an       Replaced ds707.h by ds707_extif.h
04/04/05   fc      Merged the following from MSMSHARED:
03/31/05   fc       Added support for EPZID.
03/28/05   fc      Added support for DCA.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated 
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/08/05   va       Added support for passing on base_lat/long from BS status 
                    Rsp message thro CM.
02/18/05   fc      Merged the following from MSMSHARED:
02/09/05   ph       Fix so the setpt_incl field is initialized correctly in PPSMM
                    for PREV<=6
02/09/05   fh       changed including of ds.h to be ds707.h, as data team requested
02/08/05   an       Mainlined IS95B features
02/10/05   va      Merged changes for msmshared Tip:
02/03/05   fc       Fixed lint error.
01/11/04   fh       Fixed the checking of PMRM power strength field. From the spec,
                    this field has a size of 6 bits
01/07/05   an       Removed feature FEATURE_SMS and FEATURE_UASMS
01/05/05   va       Made base_lat_long_incl per pilot in BSS status Rsp 
                    CM Rpt
02/03/05   az      Changes for Rel D DV TDSO.
01/25/05   va      Fixed compiler error when REL_C is not defined
01/10/05   va      Merged the following from REL_B-C archive:
12/10/04   sb        Fixed lint warnings.
12/06/04   yll       Added the interface for apiOne.
01/03/05   va      mcctxns_begin_tx_on_off now takes an argument for ON/OFF
12/29/04   va      Added support for TX on/off procedures for DV
12/08/04   va      Merged following from REL_B_C archive:
11/19/04   va       Added base_lat_long_incl in the BSS status response interface to CM
11/19/04   va       Interface  change to tell RXC if tune away is for GPS
12/01/04   sb      Fixes for DV RIF HHO's.
11/08/04   va      Fixed lint error.
11/08/04   va      Merged following from REL_B_C archive:
10/20/04   an        Replaced inclusion of srch.h with srchmc.h.
10/19/04   va        Handle ERM in release substate
10/01/04   pg        Set chm_supported to 1 for p_rev less than 9.
11/05/04   sb      Removed tc_populate_scr_info; added switch if transmitter
                   off for too long.
10/28/04   pg      Combined some cases in process_retry_order() to reduce 
                   code size.
10/25/04   az      Supported for Rel C sdb_supported bit in ITSPM and UHDM
10/22/04   va      Fixed the condition that checks for CH mode in process_ram
10/01/04   az      Fixed Lint errors.
09/30/04   sb      Fixed compiler error.
09/29/04   az      Fixed the ERM condition to check if all physical
                   channels need to be released.
09/27/04   va      Merged from REL B/C archive
08/19/04   yll      Clean up the MC/SRCH interface.
08/11/04   pg       Added Control Hold Mode for release C addendum ITSPM.
07/14/04   bkm      Fixed handling of rate_reduc field in SOCM.
07/13/04   bkm      Fixed a compiling error in build_ppm().
06/24/04   sb       Release C addendum changes for PLCM.
06/24/04   fc       Separate the event processing for sunfunction timer 
                    expiration and SCR transaction completion.
                    Fix FCS featurization.
09/24/04   va       Added initialization of REL D fields.
09/16/04   fc      Added support for reverse DV.
09/01/04   pg      Lint clean-up
08/24/04   az      Lint clean-up
08/23/04   pg      Added support for new RETRY_TYPE: '100' and '101'.
08/19/04   ht      Added checks to clear DV TDSO counters when the flag is set
08/09/04   fc      Service negotiation changes for Release C and D.
08/05/04   va      Changed get_abs_action_time to operate on frame offset
                   adjusted time ( This only makes a diff if we want to
                   know the exact FO adjusted time we are in). 
08/03/04   pg      Added support for Rel D timer based registration.
08/02/04   bkm     Lint cleanup with Rel B, C, D undefined.
08/02/04   va      Removed mcctcho_handle_cfs_at_arival fn call
07/21/04   ht      Added TDSO support for Rel C DV channels
07/19/04   lcc     Fixed compilation error with call to mccdv_begin_rate_chg_trans.
07/09/04   bkm     Fixed compilation issue for REL B undefined.
07/06/04   bkm     Range check REV_CQICH_REPS and REV_ACKCH_REPS.
06/29/04   bkm     Changes tied to DV function signature changes.
06/29/04   pg      Return more accurate message rejection reason for RATCHGM.
                   Release Rate Change Message when it's done.
06/28/04   bkm     Merged the following from IS2000_REL_B_C:
  06/24/04   sb      Release C addendum changes for PLCM.
  06/24/04   fc      Separate the event processing for sunfunction timer 
                     expiration and SCR transaction completion.
                     Fix FCS featurization.
06/28/04   va      Fixed some compile errors
06/16/04   va      Added support for HHO/CFS/GPS clean up
06/16/04   fc      Release C addendum changes for Sync ID.
06/15/04   pg      Added support for Rate Change Message.
06/08/04   bkm     Featurization changes to allowing building w/o Rel B/C/D.
06/04/04   bkm     Lint cleanup.
06/03/04   bkm     Merged the following from IS2000_REL_B_C:
  06/02/04   sb      Fixed lint warnings.
  05/21/04   fc      Code review comments.
05/28/04   bkm     Added FEATURE_GPS_MODE around mcctcsup_update_off_time_info().
05/28/04   bkm     Merged the following from IS2000_REL_B_C:
  05/17/04   sb      Added p-rev 9 Off Time Reporting fields to ITSPM.
  05/07/04   fc      Some code review comments.
  04/29/04   pg      Defined constants for range checking in cai.h.
  04/28/04   pg      Added range checking for fields in Power Measurement Report 
                     Message(PMRM).
  04/09/04   sb      Misc fixes for Decoupling PLCM from ESN.
  03/30/04   ph      Clean up SNM when we get a retry order for an EOM.
  03/19/04   lh      Adjusted the reject reasons when processing ERM.
  03/05/04   bkm     Lint cleanup.
04/22/04   bkm     Do not reject PCNM with FFPC info if no fwd-fundicated
                   channel is active. Store the info for later.
04/19/04   bkm     ERM updates for shortened EXT_CHIND enum names.
04/16/04   bkm     Fixed a few compile issues.
04/14/04   az      Added support for ERM - Rel C DV chan cfg change
04/08/04   bkm     Channel mask updates.
04/01/04   bkm     Simplified checking against active channels in process_
                   pwr_ctrl_msg() by removing not defined p2 code which
                   is no longer necessary given channel masks.
03/26/04   bkm     Merged the following from IS2000_REL_B_C:  
  03/19/04   sb      Added support for Release C Decoupling PLCM from ESN
  03/12/04   fc      Added support for Release C fast call setup.
  03/04/04   az      Added support for new transaction engine
  03/02/04   pg      Include outer loop setpoint information corresponding only 
                     to the physical channel specified by FPC_PRI_CHAN in pre-
                     REL C PPSMM.
  02/09/04   va      Switched around the processing of TC to Idle info
                     in ERM until after all error checks
  01/27/04   sb      Reject RAM if pri_chan is 0 and FCH is not to be restored.
  01/12/04   sb      No longer pack rej_param_id for Retrieve Parms Msg.
02/10/04   az      Changed name from mccsch_r_sch0_extend_trans() to 
                   mccsch_begin_r_sch0_extend_trans()
02/05/04   bkm     Merged from IS2000_REL_B_C
  01/27/04   sb      Reject RAM if pri_chan is 0 and FCH is not to be restored.
  01/12/04   sb      No longer pack rej_param_id for Retrieve Parms Msg.
01/28/04   bkm     Set ERM chind to well defined value when EXT_CHIND is in msg
01/15/04   bkm     Updated PCM handling for Rel C DV.
01/05/04   bkm     Misc updates for DV F-PDCH support.
12/02/03   bkm     Split internal chind into fwd and rev channel masks.
12/12/03   sb      Changed threshold and added messages for Cancel COTRM.
11/19/03   va      Bug fixes in ERM : Before doing ControlHold error check in ERM make
                   sure it is not a "release all" case
10/29/03   va      Support for REL C ERM
10/28/03   va      Changed access of msg_type in PPSMM
10/21/03   sb      Add another super-frame to cdma_off_time_start.
10/17/03   sb      Fixed off time estimate.
09/23/03   sb      Added check to reject GPS visit request if handoff pending.
09/16/03   ph      Before GPS tuneaway, check if active CCs are not in WFO.
09/04/03   sb      Fix for compiler error.
08/10/03   sb      Added fix to handle case where tc_gps_in_progress can be
                   reset while waiting for a COTRM confirmation (eg. fading).
08/06/03   sb      Added support for CDMA Off Time Reporting.
08/19/03   ph      Removed outdated comments in tc_populate_scr_info().
08/11/03   bkm     Lint cleanup.
08/01/03   yll     Added support for Dynamic Feature.
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/14/02   fc      Changed the debug message about ignoring old wakeup report
                   from searcher.
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
06/10/03   bkm     Lint cleanup.
05/27/03   ph      In processing of CLAM, validate SO before call ID is allocated.
05/19/03   ph      In EOM, default prev_sid_incl, prev_nid_incl, prev_pzid_incl.
05/15/03   ph      Reject a CLAM that comes with an invalid SO, saves problems
                   for UI/CM.
04/14/03   va      Changed references to ovhd_chan in cur_bs_ptr and
                   merged the following change as well.
04/11/03   ph      Fixed featurization for enc_capability in send_eom()
04/09/03   sb      Changed EPSMM to use CAI_EPSMM_MAX_NUM_PILOTS.
03/05/03   rs      Added condition check in process_erm() for modification
                   of fpc_pri_chan prior to snm parameters check.
02/06/03   yll     Fixed the enc_info_incl field in EOM.
02/05/03   ph      Fixed erroneous fpc_pri_chan checking for ERM.
01/31/03   bkm     Shortened F3 msg on MCTRANS_F_SCH0_START for readability.
01/24/03   lh      Modified pzid change and retry delay processing interface.
12/16/02   lcc     In build_pwr_meas, set sch_pwr_meas_incl to false when
                   FEATURE_IS2000_SCH is not defined.
11/21/02   va      Fixed compilation problems
11/19/02   hxw     Modified process_srch_gps_visit_req to inform RXC when GPS
                   is actually started.
11/18/02   va      Added support for informing CM of retry orders rxed for
                   originations in the TC state.
11/13/02   sb      Fixed featurization.
11/11/02   va      Corrected featurization around process_erm()
11/07/02   az      Mainlined FEATURE_IS95B
11/07/02   az      Removed the feature TMOBILE
10/15/02   yll     Merged in Control Hold changes.
10/03/02   ph      Lint Cleanup
10/02/02   ph      OTASP featurization fix.
10/02/02   az      Mainlined FEATURE_IS2000
10/02/02   sb      Changed orig_fail layer to L3 (since we are post-L2 Ack).
09/30/02   sb      Mainlined FEATURE_SD20.
09/25/02   bkm     Featurized the optional IS-2000 slotted timer feature.
09/20/02   fc      Included mcsys.h only if FEATURE_SD20 is not defined.
09/16/02   ph      Added error checks to ERM processing for LPM.
08/30/02   ts      Added support for Run Time R-UIM Enable
08/16/02   ck      Removed a duplicate send_rej_order int the fn mc_bs_challenge
                   Modified the send_rej_order for BS Chal report
                   Wrapped tc_dh_rpt around !FEATURE_UIM_RUIM
08/12/02   yll     Changes to include alt SOs in SO_BITMAP field in the
                   EOM message, even if max_alt_so is equal to 0.
08/07/02   yll     Fixed the compiling error when FEATURE_IS95B_MAHHO is
                   turned off.
08/06/02   HQ      Added FEATURE_CDSMS.
08/01/02   ph      fixed problem related to rate set for SO 54/55.
07/11/02   lh      VP1 code reveiw changes.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/02/02   az      Added new voice service option FEATURE_SO_VOICE_SMV
06/28/02   lh      SVD code review comment changes.
06/18/02   jrp     Changed enum to match updated cai_fpc_mode_type.
06/07/02   ph      Fix compile errors
06/06/02   ph      Fixed compile errors.
06/05/02   ph      For HHO RIF, use pending config when restoring config.
05/31/02   va      VP2 changes.
05/30/02   ph      Changes to support LPM.
05/20/02   lh      Fixed uninitialized so_bitmap_ind for EOM.
05/07/02   ph      Fixed compile errors when REL A is turned off.
05/02/02   lh      Fixed UI/CM/MC out of sync for the 2nd call.
04/30/02   sb      Added layer to payload of mcc_send_orig_failure.
04/19/02   jrp     Fixed support for setpoint information in PPSMM.
04/17/02   ph      Removed mcctcsup_validate_scm() as the functionality is now captured
                   in srv_process_con_msg().
04/16/02   va      Added support for processing L2 ack for emergency
                   flash.
04/12/02   fc      Corrected the reporting of extended release event.
04/12/02   lh      Added support for subaddress in Orig_C.
03/11/02   va      Merged from MSM CP VU 119.
  02/20/02 az      Featurized 13K voice service option
03/06/02   jrp     Fixed merge error.
02/11/02   va      Corrected temp_so initialization in create_cc event
                   for CLAM and SCM.
01/25/02   jrp     Corrected references to neg_scr.
01/24/02   ph      Fixed QOS record being put in EOM as per standard.
01/24/02   ph      Added SVD capability checking in pre_process_cc_info() for BS
                   assigned calls.
01/24/02   va      Updated populate_scr_info fn to conform to new interface struct
01/23/02   ph      Added support for including EOM functionality for SNM.
01/14/02   lh      Changed return interface for CC info processing in SCM and UHDM.
01/11/02   va      Fixed cases where SVD feature is not defined.
12/20/01   lh      Added setting of tag value when rejecting a CLAM with bad tag.
12/11/01   lh      Merged following from SVD branch.
                   12/02/01   lh      Added rejection of call assignment if MS doesn't support CCS.
                   11/30/01   va      Support for storing cs_supported from ITSPM
                   11/21/01   lh      Changes for CM/UI integration.
                   10/04/01   kk      Added QoS support for EOM.
                   08/28/01   lh      Added support for Concurrent Service.
                   08/23/01   ph      Updates to definition of tc_so_change() for SVD.
                   08/23/01   ph      SVD updates
11/28/01   fc      Added call to mccsch_update_r_sch_parm_trans() for the
                   update of R-SCH parameters when a R-SCH burst has started
                   or stopped.
11/27/01   lcc     Changes to accomodate new definition for mctrans_fpc_olpc_param_type.
10/23/01   jrp     Merged the following from common archive (MSM_CP.05.00.99)
                   10/05/01   lcc     Added call to mccsch_r_sch0_extend_trans() for R-SCH burst
                                      extension when a R-SCH burst has started.
                   10/01/01   ht      Add a feature label to remove a compiler error.
                   09/20/01   lcc     When PMRM is sent due to end of F-SCH burst, do not trigger
                                      the normal PMRM delay mechanism.
10/15/01   kk      Fixed compiler errors when Release A is not defined.
08/09/01   ph      Merged in from 1.52 MSM
                   08/06/01   kk      DCCH Full TDSO support added.
                   08/02/01   jrp     Added support for SO54, SO55.
07/06/01   ph      Added support for Callback Timers
06/26/01   va      Merged the following from MSM_CP.05.00.81
  06/12/01 ht      Fixed a typo in parameters of a ERR_FATAL call.
  06/11/01 lcc     Corrected compilation errors for non-IS2000 targets.
  06/06/01 lcc     1. Added support for retry order for RRM.
                   2. Added setting of rejected_pdu_type in MS reject order.
  06/05/01 ht      Use default window size in the Neighbor List Update
                   message if IS95B is defined.
  06/01/01 jq      Wrapped some code in FEATURE_IS95B_MAHHO.
  05/31/01 lcc     Allows releasing of FCH/DCCH with ERM to result in releasing
                   the call when neither channel exists after the release.
  05/25/01 ts      Added authentication interfaces for new UIM server.
06/14/01   va      TC re-work needed for REL A call model. All call specific message
                   processing has been moved to MCCCCIM module. Changed prototype
                   of send_rej_order to make it more flexible.
05/17/01   ph      Merged from MSM_CP.05.00.76
                   05/11/01   fc      Added support for DCCH STDSO.
                   05/09/01   bkm     Added explicit cast for rec_ptr assignment in
                                      tc_proc_cm_call_hold.
                   05/04/01   ych     Enabled SPECIAL_MDR support for base station P_REV greater
                                      than 5
                   05/03/01   va      Removed frame_offset setting from RXC_RETURNED_F interface.
                   05/03/01   jq      Made ERRM ack_req = TRUE.
                   05/03/01   va      Changed code to support new "rxc_returned_cmd_type" struct.
                   05/02/01   fc      Added support of T_ADD_ABORT.
                   04/10/01   jq      Changes to conform to the move of SCRM related functions
                                      to mccds.c.
                   05/02/01   lh      Fixed possible mis-alignment in send_status_rsp_msg() and
                                     tc_send_flash_info_msg().
                   04/23/01   lcc     Corrected a problem in send_parm_resp() where rsp_parms was
                                     incremented twice as frequently.
                   04/17/01   va      Added support for passing on pilot_rec_types from ENLUM to SRCH.
                   04/16/01   ks      The rsp_parms variable in the send_parm_resp() function is
                                      properly set, avoids CAIX parser returning a CAIX_INV_LEN_S.
                   04/16/01   fc      Added support of events report: Extended Release.
                   04/12/01   fc      Added support of events report: Service option negotiation
                                        completed.
                   04/11/01   jq      Added FEATURE ifdef to build_pwr_meas()
                   04/10/01   jq      Added msg processing releated to P2 & control hold
                   04/05/01   va      Added IS-733 SO in a case statement where it was missing,
                                      added p_rev_in_use check for srch_offset.
05/04/01   va      Added p_rev_in_use in RXC_RETURNED_F interface.
04/09/01   va      Added p_rev_in_use check before accessing srch_offset fields.
                   Added validate_pilot_rec_info to validate pilot rec fields.
03/28/01   va      Added function populate_pil_rec_info_in_rtc_msg to populate
                   pilot rec types in Rev Traf messages. Added support for
                   Extended PSMM message.
03/27/01   va      Merged from common archive:
  03/07/01 lcc     Removed a unnecessary debug message in tc_trans_done_rpt.
  03/01/01 va      Initialized some variables in the RETURNED_F command to RXC
                   for both GPS and IS2000 operation.
03/08/01   kk      fixed mcc_info_rec
03/02/01   va      Merged the following from common archive:
           mai     Changed FEATURE_GPSONE to FEATURE_GPS_MODE to move code needed
                   only for GPS part of position location to a new #define.
           jq      Added FEATURE_SCH, so it can be compiled without SCH defined.
           fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
           bgc     Changed FEATURE_FACTORY_TESTMODE to be a runtime mode.
           va      Check for active transactions by calling mctrans_is_okay_to_tune_away
                   before doing GPS tune aways if FEATURE_IS2000 is defined.
           ych     Merged T-53/JCDMA features for SK.
           jq      Changed naming of some variables and functions so that the
                   software can compile with FEATURE_IS2000_SCH undefined.
           lh      Add support for MS originated Long Code Transistion Request
                   Order
           ht      Add ERR statement when bs_p_rev is undetermined due to
                   unsupported band class and avoid writing DB with an
                   arbitrary value.
           yll     Removed FEATUR_GPSONE_TAWD_TEST_ONLY.
           ks      Added STDSO and Full TDSO checking in so_conv() and so_wfa()
                   functions to support MS terminated call.
           ck      Added the issuing of a Base station Challenge command after
                   receving the result for an SSD update.
                   Added esn to the Update SSD command when FEATURE RUIM is
                   defined.
                   Added the processing of OTASP reports recd from the R_UIM
                   and added functionality for tc_auth_rpt() to return the next
                   state.
                   Re-arranged code in tc_auth_rpt() to place the report buffer
                   back using cmd_done fumction.
02/27/01   va      Preliminary TC message changes for Rel A. Added
                   support for changes in periodic pilot mesaurement,
                   con_ref in cont DTMF order and DTMF burst message.
11/13/00   lcc     1. Moved update of p_rev into mcc_update_bs_p_rev.
                   2. Removed update of SIDand NID db items because they
                      are updates in process_nid and process_sid in mccsyobj.
11/06/00   ks      Added support for Simple TDSO and Full TDSO service options.
11/01/00   lcc     Added support for FEATURE_COMPLETE_SNM.
10/30/00   yll     Added checking if any messages or layer 2 ACKs need to be
                   transmitted (in rxtx), and if so, reject search's GPS
                   visit request.
10/27/00   jq      Merged T_AUTH to FEATURE_AUTH.
10/11/00   ts      Check RUIM status when waiting for an auth signature in
10/02/00   yll     Added support for Position Determination DBM.
09/25/00   yll     (ashokb) Fixed a problem because of race condition between
                   TX and MC in process_srch_gps_visit_req(), where TX sends
                   a second MCC_TXC_FRAME_SIG to MC, but MC clears the signal
                   when processing the first one.
09/20/00   jq      Cleaned all lint errors.
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
08/23/00   lcc     1. At completion of SHO, added checking for the condition where
                     the last leg on F-SCH is dropped.  If so, start a immediate
                     "stop F-SCH 0" transaction.
                   2. Lint cleanup.
08/18/00   fc      Added support for Retry Order.
08/21/00   nmn     CR #14237: Changed Service Option Control Message Reject
                   to reject correct message for correct reason (config).
08/02/00   yll     Added GPSONE TAWD (tuning away w/o drop) testing support.
07/28/00   fc      Mainline FEATURE_CS_ACTION_TIME_TRANS.
07/26/00   ks      Added backward compatibility in tc_send_flash_info_msg()
                   so that feature indication record is sent properly.
07/21/00   fc      Modified tc_trans_done_rpt() to use transaction type from
                   transaction report directly.
07/15/00   va      Added support for CFS transactions.
07/14/00   ks      Processing t_slotted field in process_sp_message().
07/10/00   cah     Compiler error fix.
07/06/00   yll     Added more GPS changes:
                   - tc_gps_init() to init variables when entering TC state.
                   - send SRCH_OFF_TX_OFF while turning off the transmitter.
                   - handle RXC_TX_ON_R when in release substate.
                   - handle search rpt when in release substate.
07/06/00   yll     Added GPS support.
                   - suspend CDMA operation for GPS visit.
                   - resume CDMA operation after GPS visit done.
                   - block the searcher's GPS visit request when something
                     pending or in progress which should not be interrupted.
06/21/00   fc      Modified process_sp_msg() and tc_srch_rpt() to support
                   SCRM.
06/13/00   fc      Added OLRM support.
06/12/00   lcc     1. Modified tc_inform_scr_change to conform to new transaction
                      processing design.
                   2. Added processing for SCR, HO, and PWR_CTRL_MSG transaction
                      reports.
                   3. As part of unifying action time processing, migrated transactions
                      HO, SCR, FPC_SUBCHAN_GAIN, and PWR_CTRL_MSG to mcctxns.
06/05/00   ks      Sending proper Keypad Facility information record in the
                   tc_send_flash_info_msg function during WFA & CONV states.
06/02/00   lcc     Changes in processing of power control message to conform to
                   published addendum.
06/02/00   lcc     Corrected a merge error in tc_trans_done_rpt.
06/01/00   lcc     In tc_trans_done_rpt, reads all reports in in report queue
                   instead of reading just the first; otherwise if multiple reports
                   are present in the queue, reports beyond the first one are not
                   processed when the report signal is received.
05/26/00   ck      Changed the SSD_update interface when FEATURE_RUIM is
                   defined.
                   Made a change to request for plc only when the report
                   from AUTH indicates success when FEATURE_RUIM is defined.
05/25/00   ks      Added support for Search Window offets during the processing
                   of NLUM and ENLUM messages.
05/15/00   lcc     Correct a problem with passing an invalid SO into tc_so_change
                   when call is established with service option negotiation.
                   snm_update_configuration and snm_continue_update_config is
                   now used instead.
4/24/00    lcc     Moved invokation of connection initialize to snm_udpate_configuration.
                   Added function tc_trans_done_rpt.  Added processing of SCH FER
                   fields in PMRM.
04/06/00   va      Merged the following from PP (MSM3100_CP.03.00.17):
03/14/00   ry      Added OTAPA support.
03/23/00   ry      Added IS2000 featurization
           ry      Merged the following from PP (MSM3100_CP.03.00.10):
           fc      Added support for IS-733 service option 17.
03/23/00   va      Aligned start of info record structs on dword boundaries.
03/06/00   lcc     Minor cleanup.
02/13/00   va      Addendum Changes
02/05/00   va      Fixed bugs related to loopback and markov inits. These
                   are inited before commands are sent to TXC/RXC.
01/27/00   va      Merged the following changes:
                   Added processing of RXC_HHO_TIMEOUT_R report
                   Changed TXC_TC_F to not set random signals for data calls.
01/06/00   bgc     Added #ifndef FEATURE_PLT around process_pwr_ctrl_msg().
12/20/99   va      Added full support for power control message
11/15/99   va      All the changes needed for IS2000 voice support. This includes:
                   --New tc_inform_scr_change function, to handle the new action time
                     handling at the lower layers.
                   --tc_so_change has been modified to send the SO change information
                     to CM and DS only.
                   --Get_abs_action_time converts an action time from units of modulo 64
                     super frame to an absolute frame time.
                   --Processing of new reports RXC_SCR_AT_R and SRCH_AT_R from rxc and srch
                     respectively to handle notification of action time arrival
                   --process_pwr_ctrl_msg to handle the new fields in the power control
                     message
                   --Added tc_get_plc function which returns the private or public LCM
                     when given the appropriate type required.
11/03/99   lcc     Merged from GD4021:
           doj     Modified PACKET_ZONE_ID reporting such that it will now
                   report for IS-95B as well as for FEATURE_SPECIAL_MDR.
           ks      Added a missing ; for MSG_MED in case SRCH_FING_DONE_R
                   in tc_srch_rpt function.
           lh      Serving system update to CM is added.
           ry      Updated tc_action_chk() to NOT check for
                   mcctcho_action_time_pending because we now support two
                   action times, as long as one is not from a (E|G)HDM
           jq      Added a case statement to handle SRCH_FING_DONE_R in
                   tc_srch_rpt().
           nmn     Added IS-96A vocoder capability checking
10/20/99   lcc     Corrected minor error with SMS service options (with
                   change_pri) when IS2000 is not defined.
09/20/99   lcc     Changes as a result of the new interfaces to txc and rxc.
09/14/99   ks      Added SRCH_PRIORITY and TIMIN_INCL processing in the
                   process_enlu_msg().
09/07/99   nmn     Fixed problem from IS-96 removal
09/01/99   lh      Clean up compilation for non FEATURE_UASMS build.
08/26/99   lh      Added release reason to report to CM. Fix for CR6158.
08/23/99   jq      Minor spelling changes in tc_send_flash_info_msg() due to
                   code review.
08/21/99   ks      Fixed Processing of ENLUM to avoid overwriting the Neighbor
                   record.
08/16/99   kk      Added support for FEATURE_IS95B_ALERT_ANS_BYPASS feature.
08/13/99   jq      Added support for FEATURE_IS95B_INC_CALL_FWD_ENH. Added
                   tc_send_flash_info_msg().
08/13/99   lh      CM and SMS phase 2 implementations.
08/09/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
           ry      Changed FEATURE_IS95B_CFS to FEATURE_IS95B_MAHHO
           ry      Added the processing of SRCH_CFS_READY_R and
                   SRCH_CF_MEAS_DONE_R
08/06/99   doj     Replaced constants in sign extension of add_intercept
                   and drop_intercept with CAI masks.
08/06/99   nmn     Removed IS-96
07/28/99   lh      Two-way SMS and CM support.
07/26/99   kk      Fixed CR 10999. After reception of ENLU, neighbor list size
                   was updated properly but was not reflected to Search if
                   the size of the list exceeds maximum allowable limit.
07/09/99   ry      Removed the processing of (E|G)HDM from tc_action_chk()
                   since action time is now handled through the MCCTCHO module
           ry      Moved all post handoff processing to the MCCTCHO module
           ry      Added the processing of SRCH_ABORT_R, SRCH_FAILURE_R. and
                   SRCH_CF_NSET_R
07/08/99   ych     Added support for processing Periodic Pilot Measurment
                   Message
06/17/99   kmp     Merged in the change listed below.
06/04/99   sh      added sign extension for add_intercept and drop_intercept
06/02/99   kmp     Merge in the eight IS-95B changes below, replaced T_AUTH
                   and T_SMS with FEATURE_AUTH and FEATURE_SMS respectively.
05/26/99   fc      Added support for closed loop power control step size.
           lh      take out P_REV checking in IT_SPM and EHDM.
           kmp     merged in following IS-95B changes
                   Following are the IS-95B additions:
           lh      Authentication using IMSI_M.
           sh      removed send_srch_parms
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           kk      Added process_enlu_msg.
           sh      added IS-95B soft handoff support
           lh      IS-95B True IMSI implementation added.
05/19/99   doj     Moved SEND_HANDOFF_COMPLETE_MSG, PROCESS_FM_HO_MSG and
                   INIT_FRAME_OFFSET_HANDOFF to mcctcho.c.  Removed
                   PROCESS_HO_DIR_MSG, PROCESS_EXT_HO_DIR_MSG,
                   GHDM_VALID_AND_HANDOFF_OK and PROCESS_GEN_HO_DIR_MSG and
                   consolidated them in the function
                   MCCTCHO_PROCESS_GENERIC_HANDOFF_MSG in mcctcho.c.
                   Consolidated handoff processing in TC_ACTION_CHK.  Fixed
                   some Lint errors.
04/25/99   doj     Fixed bug where SUP_CHAN_PARMS_INC was not being checked
                   before calling MCCSCM_MDR_PRE_PROCESS.
04/21/99   doj     Added SRCH_WIN_N and SRCH_WIN_R support to GHDM, EHDM and
                   HDM (for TMOBILE too).  Suppressed Lint 730 error (boolean
                   parameter in function call) at call to
                   MCCSCM_DURATION_TIMER_CHK, since the parameter is
                   intended to be a boolean.
04/08/99   dna     Cast a q_get return value to fix a compiler warning
04/08/99   lcc     In tc_so_change, moved definition of con_init outside
                   FEATURE_DS.  It is used even FEATURE_DS is not defined.
04/06/99   ram     Removed the RXC calls to change the RXDSP clock
                   to 2/3 TCXO when an MDR call is up, and to
                   restore the RXDSP clock to CHIPX8 when calls end.
                   this is no longer needed because RXDSP clock is always
                   at 2/3 TCXO.
03/26/99   doj     Added logical negation to use_time parameter in call to
                   mccscm_duration_timer_chk to correspond to boolean sense
                   of implicit_start in prototype in mccscm.c.  Changed name
                   of mccscm_cancel_prev_dur_time to
                   mccscm_cancel_prev_dur_timer to agree with prototype in
                   mccscm.c
03/18/99   ck      Added support for Re-Authentication and processing of report
                   from DH task and removed the feature OTASP_800_HACK.
03/11/99   doj     In ghdm_valid_and_handoff_ok: changed reject_code to
                   mccscm_stat_enum_type; no longer rejects on FOR_SUP_CONFIG
                   or USE_FOR_DURATION when FEATURE_SPECIAL_MDR is defined.
                   In process_gen_ho_dir_msg: Added support for FOR_DURATION;
                   corrected use of sups_needed in generate_aset call -
                   sups_needed now indicates if we are to start using sup
                   chans; added call to generate_aset_default_sups for the
                   case when no forward supplemental info is included; added
                   scm_status for return value of mccscm_duration_timer_chk.
                   Added misc. comments.  None of the above have a CR number.
02/25/99   mdl     Removed extraneous mcc_rxc_cmd in tc_rxc_rpt in the RXC_VALID_R case
02/18/99   nmn     Updated use of service option report from SNM
02/16/99   nmn     Cleaned, linted, and tested SOCM support
02/13/99   doj     Added support for General Handoff Direction Message (GHDM).
                   Also includes several misc. updates by many people to
                   support FEATURE_SPECIAL_MDR.  Modified
                   process_ext_ho_dir_msg and process_ho_dir_msg to support
                   new active set format for MDR.
01/15/99   lcc     Merged following 6 items from MDR branch:
01/05/99   smp     Fixed bug in call to rlp_establish_srvc_type() - for MDR
                   service options, the RLP Type parameter should be set to
                   RLP_TYPE_II (not RLP_TYPE_I).
12/10/98   smp     Added support for the MDR RLP interface, under the feature
                   FEATURE_IS95B_MDR.
12/09/98   lcc     Allows data calls to be handled without SNM in tc_so_change.
12/08/98   lcc     Featurized DPT with FEATURE_MDR_DPT_MARKOV.
12/03/98   lcc     Added support for DPT service options.
11/30/98   lcc     Added service negotiation manager support.
08/25/98   ck      Added the function tc_dh_rpt.
08/11/98   ck      Added support for EVRC Voice Option
06/26/98   pms     mobile no longer drops call if it receives a SO message to
                   change rates while in WFO/WFA state.  Closes CR #5519
06/25/98   kis     Added support for ASCII digit mode during the origination
                   continuation message in send_orig_c_msg() function.
03/09/98   dna     Fixed TXC_TC_F to not set random signals when done.
01/27/98   ldg     Commented usages of the unusual DS_CASE macro.
01/26/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
11/03/97   ldg     Replaced references to specific data service options with
                   a macro which represents the same information.
08/05/97   lh      Added code for buffer leak prevention.
07/01/97   rdh     Eliminated T_HO_DEBUG, made dependent upon TMOBILE instead.
06/02/97   day     Declare variable as static to be compliant with ANSI C standard.
05/29/97   na      Added correct data service option numbers - fixes service
                   option change while in traffic channel. Changes under ifdef
                   DS.
04/18/97   dna     Added OTASP_800_HACK to allow every call to do OTASP
01/25/97   dna     More flexible reject of status request.
11/14/96   jjw     Added Rate Set 2 data service capabilities
11/07/96   dna     Reject Data Burst if burst type unknown.
11/04/96   rdh     Only printing PSMM msgs if using faked handoff parms.
09/19/96   rdh     Added centralized RXTX header loading fncts to save ROM.
09/13/96   dna     Re-merged changes from 1.6 which got lost in 1.7.
                   Allow process_tc_data_burst to return state (to power down)
09/12/96   day     Fixed memory scribble caused by otasp_msg variable being
                   declared as a stack variable.
09/11/96   jca     Fixed DTMF decoding bug.
09/06/96   gb      Changed action time handling for HO_DIR messages.
08/08/96   dna     process_tc_data_burst() now can change next_state if too
                   many OTASP subsidy unlock attempts fail.
07/30/96   jgr     Added new sms buffers system, changes are ifdef NEW_SMS.
06/28/96   rdh     Merged SRCH_OFF/ON_TX commands for PSMM holdoff if TX off.
06/25/96   dna     Added support for Phase 1 of OTASP.
06/17/96   dna     Handle special cases for status request/response.
06/05/96   dna     Change mux option if negotiating up to 13K in WFO or WFA.
05/30/96   dna     Took the support routines out of MCCTC.C to create this
                   module.  Look in MCCTC.C for history prior to this date.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <string.h>
#include "assert.h"
#include "cai.h"
#include "caii.h"
#include "comdef.h"
#include "db.h"
#include "diag.h"
#include "err.h"
#include "loopback.h"
#include "mar.h"
#include "mcc.h"
#include "mccdma.h"
#include "mccreg.h"
#include "mccrx.h"
#include "mccrxtx.h"
#include "mccsrch.h"
#include "mccsyobj.h"
#include "mcctcho.h"
#include "mcctci.h"
#include "msg.h"
#include "parm.h"
#include "queue.h"
#include "qw.h"
#include "ran.h"
#include "rex.h"
#ifdef FEATURE_MULTI_RX_CHAIN
#include "rfm.h"
#else
#include "rf.h"
#endif /* FEATURE_MULTI_RX_CHAIN */
#include "rxcmc.h"
#include "rxtx.h"
#include "srchmc.h"
#include "target.h"
#include "customer.h"
#include "ts.h"
#include "txcmc.h"
#include "voc.h"
#include "srv.h"
#include "snm.h"

#ifdef FEATURE_CDSMS
#include "wmsmsg.h"
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_AUTH
#include "auth.h"
#endif /* FEATURE_AUTH */
#include "mci.h"  /* mc_cmd_q */
#ifdef FEATURE_DS
#include "ds707_extif.h"
#endif /* FEATURE_DS */
#include "tmsi.h"
#ifdef FEATURE_OTASP
#include "otaspi.h"
#include "otaspx.h"
#ifdef FEATURE_OTASP_OTAPA
#include "otaspe.h"
#endif /* FEATURE_OTASP_OTAPA */
#endif /* FEATURE_OTASP */

#include "mccccl.h"
#include "mccscm.h"

#ifdef FEATURE_MVS_MIGRATE
#error code not present
#endif /* FEATURE_MVS_MIGRATE */

#include "caix.h"

#include "mccsch.h"
#include "mccap.h"
#include "mccds.h"
#include "mcctxns.h"
#include "mctrans.h"
#include "mcctcsup.h"

#if (defined FEATURE_DBM && defined FEATURE_GPSONE && !defined FEATURE_CGPS)
#error code not present
#endif /* FEATURE_DBM && ! FEATURE_CGPS */

#if defined(FEATURE_IS2000_TDSO) || defined(FEATURE_PASSPORT_MODE)
#include "bit.h"
#endif /* FEATURE_IS2000_TDSO || FEATURE_PASSPORT_MODE */

#ifdef FEATURE_IS2000_TDSO
#include "tdso.h"
#endif /* FEATURE_IS2000_TDSO */
#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif

#include "event.h"
#include "mccccim.h"


#ifdef FEATURE_IS2000_REL_A_SVD
#include "mcctctag.h"
#include "mccsrid.h"
#endif /* FEATURE_IS2000_REL_A_SVD */

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

#if defined(FEATURE_IS2000_REL_C_DV) || defined (FEATURE_IS2000_DV_SIG_TEST)
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV || FEATURE_IS2000_DV_SIG_TEST */

#if defined(FEATURE_IS2000_REL_C_DV) && defined (FEATURE_IS2000_TDSO)
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV && FEATURE_IS2000_TDSO */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

#ifdef FEATURE_MEID_SUPPORT
#include "mccmeid.h"
#endif

#ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif

/*lint -e818 */ 
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Markov forward power control parameters */
#define TC_PARM_GOOD_FULL    32000   /* Markov expected full,  got full */
#define TC_PARM_GOOD_OVERALL 32001   /* Markov expected i,  go i where i =
                                        full, 1/2, 1/4 1/8 */
#define TC_PARM_BAD_FULL     32002    /* Markov expected full got one of;
                                        1/2, 1/4, 1/8, FErr, Erasure */
#define TC_PARM_BAD_OVERALL  32003    /* Markov expected i got j not equal
                                         to j and i = full, 1/2, 1/4, 1/8
                                         j= full, 1/2, 1/4, 1/8, FErr Erase */

#define TC_MARKOV_RESET      32767     /* reset Markov statistics */

#define MAX_DTMF_TIME 32767U
  /* Maximum amount of time that DTMF key depressed timer is set for */

#ifdef FEATURE_OTASP
  otaspi_ftc_msg_type otasp_msg;
#endif /* FEATURE_OTASP */

/* MC uses this flag to determine wheter the PPSMM is enabled or not. If
 * mcctcsup_ppsmm_enabled is set to TRUE, it means that PPSMM Processing
 * is enabled, else disabled.  This flag is used to determine whether
 * to really send a SRCH_STOP_PPSM_F to SRCH in case of GHDM, EHDM
 * processing and one time PPSMM. MC will only send SRCH_STOP_PPSM_F if
 * PPSMM is enable. This way MC will not unneccsarily send CMDS to SRCH
 */
boolean mcctc_ppsm_enabled = FALSE;

/* We just want enough memory to store at least 2 power control outstanding transactions
   2 is an arbitrary number under the assumption we will not have more than 2
   outstanding transactionsat any given time, The effect of not updating a
   transaction in our non neg SCR just means,if we return back from a failed
   HHO with SCR,  we will not correctly update the "FFPC" parameters correspodning
   to the missed transaction  with our non neg SCR but till that time we will be
   operating fine */
#define MCCTCSUP_MAX_PWR_CTRL_TXNS 2
mccdma_pwr_ctrl_info_type mcctcsup_pcm_info[MCCTCSUP_MAX_PWR_CTRL_TXNS];
byte mcctcsup_pcm_index=0;

#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */

#ifdef FEATURE_UIM_RUIM
  /* Random number given by base station for use in SSD generation */
  LOCAL qword save_randssd;
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_IS2000_REL_A
// True if Periodic Pilot Measurement Request Order specified incl_setpt=1.
// This will cause setpoint values to be transmitted in the Periodic Pilot
// Strength Measurement Message.
boolean mcctc_ppsm_setpt_incl = FALSE;
#endif //FEATURE_IS2000_REL_A

/* Power Control Reporting state indicators                                */

/* Periodic power control reporting state  */
boolean mcc_rx_periodic_rpt_on;

/* Threshold power control reporting state */
boolean mcc_rx_threshold_rpt_on;

word mar_rate;
    /* Markov rate to send to Markov Service Option */


/*===========================================================================

FUNCTION MCCTC_FILL_RXTX_HDR_ACK_Q

DESCRIPTION
  This function fills in the header of the RXTX_TC_MSG command for
  messages which need to be returned to the ack queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctc_fill_rxtx_hdr_ack_q
(
  rxtx_cmd_type *cmd_ptr
   /* Pointer to command to send to layer 2 task */
)
{
  cmd_ptr->tc_msg.hdr.command = RXTX_TC_MSG_F;
  cmd_ptr->tc_msg.hdr.cmd_hdr.task_ptr   = &mc_tcb;
  cmd_ptr->tc_msg.hdr.cmd_hdr.sigs       = MCC_Q_SIG;
  cmd_ptr->tc_msg.hdr.cmd_hdr.done_q_ptr = &mcc_rxtx_ack_q;

} /* mcctc_fill_rxtx_hdr_ack_q */

/*===========================================================================

FUNCTION MCCTC_FILL_RXTX_HDR_FREE_Q

DESCRIPTION
  This function fills in the header of the RXTX_TC_MSG command for
  messages which do NOT need to be returned to the ack queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctc_fill_rxtx_hdr_free_q
(
  rxtx_cmd_type *cmd_ptr
   /* Pointer to command to send to layer 2 task */
)
{
  cmd_ptr->tc_msg.hdr.command = RXTX_TC_MSG_F;
  cmd_ptr->tc_msg.hdr.cmd_hdr.task_ptr   = NULL;
  cmd_ptr->tc_msg.hdr.cmd_hdr.sigs       = 0;
  cmd_ptr->tc_msg.hdr.cmd_hdr.done_q_ptr = &rxtx_mc_free_q;

} /* mcctc_fill_rxtx_hdr_free_q */

/*===========================================================================

FUNCTION SEND_REJ_ORDER

DESCRIPTION
  This function builds and sends a Mobile Station Reject Order to the
  layer 2 task to be sent to the Base Station.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_rej_order
(
  tc_rej_ord_parms_type * rej_parms_ptr
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
    /* ---------------------------------------------
    ** Build the Mobile Station Reject Order message
    ** --------------------------------------------- */
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.msg_type     = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.order        = CAI_MS_REJECT_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.ordq         = rej_parms_ptr->rej_code;
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.rej_msg_type = rej_parms_ptr->rej_msg_type;

    if (rej_parms_ptr->rej_msg_type == CAI_TC_FWD_ORD_MSG)
    {
      cmd_ptr->tc_msg.msg.tc_ord.rej.var.order.rej_order = rej_parms_ptr->rej_order;
      cmd_ptr->tc_msg.msg.tc_ord.rej.var.order.rej_ordq  = rej_parms_ptr->rej_ordq;
    }
    else if (rej_parms_ptr->rej_msg_type == CAI_SET_PARMS_MSG)
    {
      cmd_ptr->tc_msg.msg.tc_ord.rej.var.param.rej_param_id = rej_parms_ptr->rej_param_id;
    }
    else if ((rej_parms_ptr->rej_msg_type == CAI_FWD_FLASH_MSG) ||
             (rej_parms_ptr->rej_msg_type == CAI_ALERT_MSG)
#ifdef FEATURE_IS2000_REL_A
             || (rej_parms_ptr->rej_msg_type == CAI_EXT_ALERT_W_INFO_MSG)
             || (rej_parms_ptr->rej_msg_type == CAI_EXT_FLASH_W_INFO_MSG)
#endif
            )
    {
      cmd_ptr->tc_msg.msg.tc_ord.rej.var.rec.rej_record = rej_parms_ptr->rej_record;
    }

#ifdef FEATURE_IS2000_REL_A
    if ( (rej_parms_ptr->rej_code == CAI_REJ_CALL_ASSIGN_REJ) ||
         (rej_parms_ptr->rej_code == CAI_REJ_NO_CC_INST) ||
         (rej_parms_ptr->rej_code == CAI_REJ_CC_ALREADY_PRESENT) ||
         (rej_parms_ptr->rej_code == CAI_REJ_CC_TAG_MISMATCH)
       )
    {
      cmd_ptr->tc_msg.msg.tc_ord.rej.con_ref = rej_parms_ptr->con_ref;
    }
    if (rej_parms_ptr->rej_code == CAI_REJ_CC_TAG_MISMATCH)
    {
      cmd_ptr->tc_msg.msg.tc_ord.rej.tag = rej_parms_ptr->tag;
    }
#endif

/* We only support 20ms frames now, so this will always be 0 */
    cmd_ptr->tc_msg.msg.tc_ord.rej.rejected_pdu_type = 0;

    cmd_ptr->tc_msg.ack_req = FALSE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }

} /* end send_rej_order() */

#ifdef FEATURE_AUTH
#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION MC_BS_CHALLENGE

DESCRIPTION
  This function sends a command to the Authentication Task to perform a Base
  Station challenge.  This is to start the SSD Update process when using an UIM
  card for authentication.  It then sends a BS Challenge command to auth and does
  NOT wait for the response.  The response is processed by tc_auth_rpt.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The response is processed by tc_auth_rpt.  This is where the SSD update
  command is sent as well as the Base Station Challenge order.

===========================================================================*/
void mc_bs_challenge
(
  qword randssd
    /* Random number given by base station for use in SSD generation */
)
{
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Save the randssd in a static buffer for the SSD update command. */
  qw_equ (save_randssd, randssd);

  /* Get an auth command buffer. */
  if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
  {
    /* Send the BS challenge command.  */
    auth_cmd_ptr->hdr.command = AUTH_BS_CHAL_F;
    auth_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
    /* Use the random number as the seed*/
    auth_cmd_ptr->chal.randseed = ran_next();
    /* Use the Auth reportback function. */
    auth_cmd_ptr->chal.rpt_function = mcc_queue_auth_rpt;

    /* Using the Auth queue signal */
    (void) rex_clr_sigs(&mc_tcb, MCC_AUTH_Q_SIG);

    /* Send the command and wait for the initial status */
    mcc_auth_cmd( auth_cmd_ptr );

    /* Check the initial response from auth */
    if ( auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S )
    {
      ERR( "Bad status from AUTH %d", auth_cmd_ptr->hdr.status, 0, 0 );
      /* The SSD update will not succeed.  Reject the SSD Update */
      rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
      rej_parms.rej_code = CAI_REJ_UNSP;
      send_rej_order( &rej_parms);
    } /* end if - the BS challenge command was received by auth */
  }
  else /* Unable to get an auth command buffer from the free queue. */
  {
    ERR( "No free buffers on auth_free_q", 0, 0, 0 );
    rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
    rej_parms.rej_code = CAI_REJ_UNSP;
    send_rej_order( &rej_parms);
  }
} /* mc_bs_challenge() */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION START_SSD_UPDATE

DESCRIPTION
  This function sends a command to the Authentication Task to begin the
  SSD Update process.  It then sends a BS Challenge Order, which forces
  the base station to authenticate itself before we agree to update our SSD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the SSD Update can not be started, the SSD Update Message is rejected.

===========================================================================*/
void start_ssd_update
(
  qword randssd,
    /* Random number given by base station for use in SSD generation */
  dword randbs
    /* Random number from mobile for use in SSD generation. */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
  {
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      auth_cmd_ptr->hdr.command = AUTH_SSD_UPDATE_F;
      qw_equ (auth_cmd_ptr->ssd.randssd, randssd);
      auth_cmd_ptr->ssd.process_control = RUIM_DEFLT_PROC_CNTRL;
      auth_cmd_ptr->ssd.esn = cdma.esn;
    }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      auth_cmd_ptr->hdr.command = AUTH_SSD_UPDATE_F;
      auth_cmd_ptr->ssd.auth_data = mcc_get_auth_imsi_s1();
      auth_cmd_ptr->ssd.randbs = randbs;
      qw_equ (auth_cmd_ptr->ssd.randssd, randssd);
    }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
    mcc_auth_cmd( auth_cmd_ptr );
    if ( auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S )
    {
      /* Send BS Challenge Order */
      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) != NULL)
      {
        cmd_ptr->tc_msg.msg.tc_ord.chal.msg_type = CAI_TC_REV_ORD_MSG;
        cmd_ptr->tc_msg.msg.tc_ord.chal.order    = CAI_BS_CHAL_ORD;
        cmd_ptr->tc_msg.msg.tc_ord.chal.ordq     = 0;
        cmd_ptr->tc_msg.msg.tc_ord.chal.randbs   = randbs;

        cmd_ptr->tc_msg.ack_req = TRUE;

        /* Fill in the command header */
        mcctc_fill_rxtx_hdr_ack_q ( cmd_ptr );

        rxtx_cmd( cmd_ptr );
      }
      else
      {
        ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
        rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
        rej_parms.rej_code = CAI_REJ_UNSP;
        send_rej_order( &rej_parms);
      }
    }
    else
    {
      ERR( "Bad status from AUTH %d", auth_cmd_ptr->hdr.status, 0, 0 );
      rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
      rej_parms.rej_code = CAI_REJ_UNSP;
      send_rej_order( &rej_parms);
    }
  }
  else
  {
    ERR( "No free buffers on auth_free_q", 0, 0, 0 );
    rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
    rej_parms.rej_code = CAI_REJ_UNSP;
    send_rej_order( &rej_parms);
  }
} /* start_ssd_update() */

/*===========================================================================

FUNCTION SEND_AUTH_RESP_MSG

DESCRIPTION
  This function sends an Authentication Response Message with the
  auth signature value passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void send_auth_resp_msg
(
  dword authu
    /* Authentication Signature to be included with message */
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
    cmd_ptr->tc_msg.msg.auth_resp.msg_type = CAI_TC_AUTH_RESP_MSG;
    cmd_ptr->tc_msg.msg.auth_resp.authu    = authu;
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
} /* send_auth_resp_msg() */

/*===========================================================================

FUNCTION SEND_SSD_UPDATE_ORD

DESCRIPTION
  This function queries the authentication task to find out if the BS
  Challenge was successful, and sends an SSD Update Accepted or SSD
  Update Rejected order as appropriate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_ssd_update_ord
(
  dword authbs
    /* Value of AUTHBS calculated by Base Station */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  auth_rpt_type *auth_rpt_ptr;
    /* Pointer to report from authentication task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->tc_msg.msg.tc_ord.ordq.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.order    = CAI_SSD_UPDATE_ORD;

    /* Unless all goes will we send an SSD Update Rejection Order */
    cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq     = CAI_SSD_UPDATE_REJ;
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
    {
      auth_cmd_ptr->hdr.command = AUTH_FINISH_SSD_UPDATE_F;
      auth_cmd_ptr->f_ssd.authbs = authbs;
      auth_cmd_ptr->f_ssd.rpt_function = mcc_queue_auth_rpt;

      mcc_auth_cmd( auth_cmd_ptr );

      if ( auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S )
      {
        /* Wait for auth to report back success of SSD Update */

        /* ----------------------------------
        ** Wait for MCC_AUTH_Q_SIG to be set.
        ** ---------------------------------- */
        MSG_MED( "Waiting for AUTH report", 0, 0, 0 );

        (void) mcc_wait( MCC_AUTH_Q_SIG );

        auth_rpt_ptr = (auth_rpt_type*) q_get( &mc_auth_q );

        if ((auth_rpt_ptr != NULL) &&
            (auth_rpt_ptr->rpt_type == AUTH_FINISH_SSD_R) &&
            (auth_rpt_ptr->rpt.ssd_update_ok))
        {
          /* -----------------------------------------------------------
          ** Since we have changed the SSD, we need to recalculate authr
          ** next time we do a registration.  Setting cdma.authr_valid
          ** to FALSE will force this to happen.
          ** ----------------------------------------------------------- */
          cdma.authr_valid = FALSE;

          /* All went well, so we will send SSD Update Confirmation Order */
          cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq     = CAI_SSD_UPDATE_CONF;

          #if defined(FEATURE_OTASP) && defined(FEATURE_OTASP_STATUS)
          otasp_report_status_to_cm(CM_OTASP_STATUS_SSD_UPDATED);
          #endif  /* FEATURE_OTASP && FEATURE_OTASP_STATUS */
        }
        else /* report was invalid */
        {
          ERR( "Report from AUTH was invalid!", 0, 0, 0 );
        }

        if (auth_rpt_ptr != NULL)
        {
          cmd_done( &auth_rpt_ptr->rpt_hdr );
        }
      }
      else /* Status returned from command was an error condition */
      {
        ERR( "Bad status from AUTH %d", auth_cmd_ptr->hdr.status, 0, 0 );
      }
    }
    else
    {
      ERR( "No free buffers on auth_free_q", 0, 0, 0 );
    }

    /* Now send message */
    rxtx_cmd( cmd_ptr );
  }
} /* send_ssd_update_ord() */

/*===========================================================================

FUNCTION SEND_PARAM_UPDATE_ORD

DESCRIPTION
  This function sends a Parameter Update Confirmation order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_param_update_ord
(
  byte ordq
    /* Value of ordq field of parameter update order */
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
    cmd_ptr->tc_msg.msg.tc_ord.ordq.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.order    = CAI_PARAM_UPDATE_CONF_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq     = ordq;
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
} /* send_param_update_ord() */

/*===========================================================================

FUNCTION SEND_PLC_REQ_ORD

DESCRIPTION
  This function sends a Long Code Transition Request Order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_plc_req_ord
(
  byte req_ordq
    /* Value of ordq field of Long Code Transition Request Order */
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
    /* Send the message */
    cmd_ptr->tc_msg.msg.tc_ord.ordq.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.order    = CAI_PLC_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq     = req_ordq;
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
} /* send_plc_req_ord() */

/*===========================================================================

FUNCTION SEND_PLC_RESP_ORD

DESCRIPTION
  This function sends a Long Code Transition Response Order.  The ORDQ
  value in the response is determined by the ORDQ value of the request and
  the ability of the mobile to honor the request.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we are doing what was requested, FALSE if we are refusing the
  request.

SIDE EFFECTS
  None.

===========================================================================*/

boolean send_plc_resp_ord
(
  byte req_ordq
    /* Value of ordq field of Long Code Transition Request Order */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte resp_ordq = 0;
   /* Value of ordq field to be put in Long Code Transition Response Order */
   /* Initialize resp_ordq to make lint happy */
  boolean obey = TRUE;
   /* Indicates if we are going to obey the request, or deny it */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
    /* Determine what ORDQ to send, and decide whether the mobile will obey the request. */
    if (req_ordq == CAI_PLC_REQ_PUB)
    {
      /* The mobile always accepts a request to switch to public. */ 
      resp_ordq = CAI_PLC_RSP_PUB;
    }
    else if (req_ordq == CAI_PLC_REQ_PRI)
    {
      if (!cdma.vp_desired)
      {
        /* ------------------------------------------------
        ** We don't want voice privacy.  Refuse the request
        ** by requesting public.
        ** ------------------------------------------------ */
        MSG_HIGH( "Refusing privacy request: mobile does not want voice privacy", 0, 0, 0 );
        resp_ordq = CAI_PLC_RSP_PUB;
        obey = FALSE;
        if (cdma.long_code.private_lcm_active)
        {
          MSG_HIGH("Continuing to use Voice Privacy when we don't want to!", 0, 0, 0);
        }
      }

      else if (((!cdma.long_code.private_lcm_active) && (!auth_private_lcm_available()))
#if (defined (FEATURE_AUTH) && defined(FEATURE_OTASP))
               || (otasp_reauth_in_progress)
#endif
              )
      {
        /* We are using the public long code and can not go to private
        ** or we are in the process of reauthenticating */
        MSG_HIGH( "Refusing privacy request: mobile cannot support privacy",0,0,0);
        resp_ordq = CAI_PLC_RSP_PUB;
        obey = FALSE;
      }
      else
      {
        /* We can switch, or we don't need to switch */
        resp_ordq = CAI_PLC_RSP_PRI;
      }
    }
    else
    {
      /* ----------------------------------------------------------
      ** The base station sent an invalid ORDQ.  Refuse the request
      ** by responding that we are not changing our long code.
      ** ---------------------------------------------------------- */
      ERR( "Invalid ORDQ field in PLC_ORD", 0, 0, 0 );
      resp_ordq = (cdma.long_code.private_lcm_active) ? CAI_PLC_RSP_PRI : CAI_PLC_RSP_PUB;

      /* Since we don't know what was requested, we consider this a denial */
      obey = FALSE;
    }

    /* Then send the message */
    cmd_ptr->tc_msg.msg.tc_ord.ordq.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.order    = CAI_PLC_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq     = resp_ordq;
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }

  return(obey);

} /* send_plc_resp_ord() */

/*===========================================================================

FUNCTION PROCESS_PRIVACY_PREF

DESCRIPTION
  This function processes a privacy preferenc request from CM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_privacy_pref
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
)
{
  if (msg_ptr->privacy_mode.long_code == MC_LONG_CODE_PUBLIC)
  {
    MSG_HIGH("CM request PUBLIC LCM", 0, 0, 0);
    
    /*  we only send PLC Req if private long code is active */
    if (cdma.long_code.private_lcm_active)
    {
      send_plc_req_ord(CAI_PLC_REQ_PUB);
      cdma.vp_desired = FALSE;
    }
  }
  else if (msg_ptr->privacy_mode.long_code == MC_LONG_CODE_PRIVATE)
  {
    MSG_HIGH("CM request Private LCM", 0, 0, 0);

    /* We only request Private LCM if Public LCM is active */
    if (!cdma.long_code.private_lcm_active && auth_private_lcm_available()
#ifdef FEATURE_OTASP
        && !otasp_reauth_in_progress
#endif
       )
    {
      send_plc_req_ord(CAI_PLC_REQ_PRI);
      cdma.vp_desired = TRUE;
    }
  }
  else
  {
    ERR_FATAL("Unknown Privacy Preference", 0, 0, 0);
  }

} /* process_privacy_pref() */

#if ((defined FEATURE_AUTH) && (defined FEATURE_IS2000_REL_C_AKA) && (defined FEATURE_IS2000_REL_D))
#error code not present
#endif /* FEATURE_AUTH && FEATURE_IS2000_REL_C_AKA && FEATURE_IS2000_REL_D */

/*===========================================================================

FUNCTION START_AUTH_SIGNATURE

DESCRIPTION
  This function sends a command to AUTH to perform the auth_signature
  for an auth challenge message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If we can not perform the auth signature we reject the auth challenge
  message.

===========================================================================*/

void start_auth_signature ( void )
{
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
#ifdef FEATURE_UIM_RUIM
  word i;
   /* Index variable */
#endif /* FEATURE_UIM_RUIM */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
  {
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      auth_cmd_ptr->hdr.command = AUTH_SIGNATURE_F;
      auth_cmd_ptr->sig.rand_type = RUIM_UNIQUE_RAND_CHAL;
      auth_cmd_ptr->sig.rand_chal = cdma.randu;
      auth_cmd_ptr->sig.dig_len = 0;
      for (i = 0; i < 3 ; i++)
      {
         auth_cmd_ptr->sig.digits[i] = 0;
      }
      auth_cmd_ptr->sig.process_control = RUIM_DEFLT_PROC_CNTRL;
      auth_cmd_ptr->sig.esn = cdma.esn;
      auth_cmd_ptr->sig.rpt_function = mcc_queue_auth_rpt;
    }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      auth_cmd_ptr->hdr.command = AUTH_SIGNATURE_F;
      auth_cmd_ptr->sig.rand_chal = (cdma.randu << 8) |
                                      (mcc_get_auth_imsi_s2() & 0xFF);
#ifndef FEATURE_AUTH_DIGITS
      auth_cmd_ptr->sig.auth_data = 0;
#else
      auth_cmd_ptr->sig.auth_data = mcc_get_auth_imsi_s1();
#endif /* FEATURE_AUTH_DIGITS */

      auth_cmd_ptr->sig.save_reg = FALSE;
      auth_cmd_ptr->sig.rpt_function = mcc_queue_auth_rpt;
    }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

    mcc_auth_cmd( auth_cmd_ptr );

    if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
    {
      /* -----------------------------------------------
      ** We have a serious problem and can not calculate
      ** authu, so we should reject this message.
      ** ----------------------------------------------- */
      ERR( "Bad status from AUTH %d", auth_cmd_ptr->hdr.status,0,0);
      rej_parms.rej_msg_type = CAI_TC_AUTH_MSG;
      rej_parms.rej_code = CAI_REJ_UNSP;
      send_rej_order( &rej_parms);
    }
  }
  else
  {
    ERR( "No free buffers on auth_free_q", 0, 0, 0 );
    rej_parms.rej_msg_type = CAI_TC_AUTH_MSG;
    rej_parms.rej_code = CAI_REJ_UNSP;
    send_rej_order( &rej_parms);
  }
} /* start_auth_signature() */

/*===========================================================================

FUNCTION TC_AUTH_RPT

DESCRIPTION
  This function processes a report from the Authentication task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word tc_auth_rpt
(
  auth_rpt_type *auth_rpt_ptr,
    /* Pointer to authentication report */
  word next_state
    /* Next state */
)
{
#ifdef FEATURE_UIM_RUIM
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
#endif

  MSG_MED( "Got auth report", 0, 0, 0 );

  switch(auth_rpt_ptr->rpt_type)
  {
    case AUTH_SIGNATURE_R :
#ifdef FEATURE_OTASP
       if (!otasp_reauth_in_progress)
       {
#endif /* FEATURE_OTASP */

#ifdef FEATURE_UIM_RUIM
           /* Check the RUIM status, send a reject order if signature failed */
           if ((auth_rpt_ptr->rpt_status == RUIM_FAIL)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
           /* Determine if the R-UIM is available for this NAM */
           && (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
           )
           {
              /* Send a reject order */
             rej_parms.rej_msg_type = CAI_TC_AUTH_MSG;
             rej_parms.rej_code = CAI_REJ_UNSP;
             send_rej_order( &rej_parms);
           }
           else
#endif /* FEATURE_UIM_RUIM */
           {
#if ((defined FEATURE_AUTH) && (defined FEATURE_IS2000_REL_C_AKA) && (defined FEATURE_IS2000_REL_D))
#error code not present
#else
             /* ------------------------------------------------------------
             ** We can send the Authentication Challenge Response Message
             ** ------------------------------------------------------------*/
             send_auth_resp_msg( auth_rpt_ptr->rpt.auth );
#endif /* FEATURE_AUTH && FEATURE_IS2000_REL_C_AKA && FEATURE_IS2000_REL_D */
           }

#ifdef FEATURE_OTASP
       }
       else
       {
#ifdef FEATURE_AUTH
           /* reauthentication is in progress  */
           otasp_process_auth_report(auth_rpt_ptr);
#endif /* FEATURE_AUTH */
       }
#endif /* FEATURE_OTASP */

     break;

#ifdef FEATURE_UIM_RUIM
    case AUTH_BS_CHAL_R:
      /* Did the command compete without any problems. */
      if (auth_rpt_ptr->rpt_status == UIM_PASS)
      {
        /* Send the RANDBS to start the SSD update and send the
           BS Challenge order. */
        start_ssd_update (save_randssd, auth_rpt_ptr->rpt.randbs);
      }
      else /* report was invalid */
      {
        ERR( "Failure Report from AUTH!", 0, 0, 0 );
        /* The SSD update will not succeed.  Reject the SSD Update */
        rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
        rej_parms.rej_code = CAI_REJ_UNSP;
        send_rej_order( &rej_parms);
      } /* end if - check the status of the AUTH command. */
      break;
#endif /* FEATURE_UIM_RUIM */

    case AUTH_GENERATE_KEY_R :
#ifdef FEATURE_OTASP
      if (! otasp_reauth_in_progress)
      {
#endif /* FEATURE_OTASP */
         /* ------------------------------------
         ** We now have a valid CMEA key and VPM
         ** ------------------------------------ */
         /* If we want voice privacy but don't have it, request it */
         if ((cdma.vp_desired) && (!cdma.long_code.private_lcm_active)
#ifdef FEATURE_UIM_RUIM
                         && ((auth_rpt_ptr->rpt_status == RUIM_PASS)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
                         /* Determine if the R-UIM is available for this NAM */
                         || (!nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
                            )
#endif /* FEATURE_UIM_RUIM */
                         )
         {
            send_plc_req_ord(CAI_PLC_REQ_PRI);
         }

#if ((defined FEATURE_AUTH) && (defined FEATURE_IS2000_REL_C_AKA) && (defined FEATURE_IS2000_REL_D))
#error code not present
#endif /* FEATURE_AUTH && FEATURE_IS2000_REL_C_AKA && FEATURE_IS2000_REL_D */

#ifdef FEATURE_OTASP
      }
      else
      {
         /* reauthentication is in progress  */
         otasp_process_auth_report(auth_rpt_ptr);
      }
#endif /* FEATURE_OTASP */
      break;

    case AUTH_COMMIT_KEYS_R:
#ifdef FEATURE_OTASP
       /* Flag set to result of commit operation by AUTH */
      otasp_akey_commit_result = auth_rpt_ptr->rpt.commit_keys_ok;
#endif /* FEATURE_OTASP */
      break;

#ifdef FEATURE_OTASP_OTAPA
    case AUTH_SIGNATURE_SPASM_R:
      /* Process the auth_otapa for SPASM validation */
      MSG_MED(" received AUTH_SIG_SPASM_R", 0,0,0);
      otasp_process_auth_report(auth_rpt_ptr);
      break;
#endif /* FEATURE_OTASP_OTAPA */

    default :
      /* we got the wrong report */
      ERR( "Wrong report %d from AUTH!", auth_rpt_ptr->rpt_type, 0, 0 );
      break;
  } //lint !e788

  /* Put the report buffer back */
  cmd_done( &auth_rpt_ptr->rpt_hdr );

  return(next_state);

} /* tc_auth_rpt() */
#endif /* FEATURE_AUTH */

#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */

/*===========================================================================

FUNCTION TC_ACTION_CHK

DESCRIPTION
  This function checks the action time in a message.  If the action time
  is valid, it sets the action_timer to wake up on the action time and
  copies the received message to the tc_action_msg buffer.  If the action
  time is invalid, it sends a Mobile Station Reject Order to the base station
  indicating that the message was invalid.  If the function finds that the
  action timer is already set, this function will send a Mobile Station
  Reject Order since IS-95 only allows for one message with an outstanding
  action time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See function description.

===========================================================================*/

void tc_action_chk
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to message with action time field */
)
{
  qword curr_time;
    /* Current time */
  word phase_adjust;
    /* 20ms offset into 80ms superframe */
  word action_time;
    /* Action time from message */
  word temp_time;
    /* Current time in 80 msec units modulo 64 */
  word wait_time;
    /* Amount of time to set action time for */
  boolean send_reject;
    /* Indicator of whether a Mobile Station Reject Order should be sent */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize variables */
  send_reject = FALSE;
  rej_parms.rej_order = rej_parms.rej_ordq = 0;
  action_time = 0;

  /* ---------------------------------------------------
  ** First check to see if action timer is already going
  ** --------------------------------------------------- */

  if ((msg_ptr->gen_tc.msg_type == CAI_TC_FWD_ORD_MSG) &&
      (msg_ptr->tc_ord.gen.order == CAI_SO_CTL_ORD))
  {
    /* ------------------------------------------------
    ** Process Service Option Control Orders separately
    ** since they have their own timer
    ** ------------------------------------------------ */

    if ((cdma.so_cur_pri == CAI_SO_NULL) ||
        (rex_get_timer( &mcc_so_ctl_timer ) != 0))
    {
      /* ----------------------------------------------------------------
      ** No currently active service option OR already have a pending
      ** Service Option Ctrl Order --> send a Mobile Station Reject Order
      ** ---------------------------------------------------------------- */
      send_reject = TRUE;
      rej_parms.rej_order   = msg_ptr->tc_ord.ordq.order;
      rej_parms.rej_ordq    = msg_ptr->tc_ord.ordq.ordq;
    }
  } /* end if Service Option Control Order received */

  else /* Process all other messages with action times */
  {
    if (rex_get_timer( &mcc_action_timer ) != 0)
    {
      /* For now, the handoff module shares the tc_action_msg buffer,
       * so we need to check to make sure the buffer is not being used.
       */

      /* ----------------------------------------------------------------
      ** Already have a pending msg -> send a Mobile Station Reject Order
      ** ---------------------------------------------------------------- */
      send_reject = TRUE;

      if (msg_ptr->gen_tc.msg_type == CAI_TC_FWD_ORD_MSG)
      {
        rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
        rej_parms.rej_ordq  = msg_ptr->tc_ord.ordq.ordq;
      }
    }
  }

  if (send_reject)
  {
    rej_parms.rej_msg_type = msg_ptr->gen_tc.msg_type;
    rej_parms.rej_code =  CAI_REJ_UNSP;
    send_rej_order(  &rej_parms );
  }
  else
  {
    /* Get action time from message based on message type */
    switch (msg_ptr->gen_tc.msg_type)
    {
      case CAI_TC_FWD_ORD_MSG:
        /* Forward Traffic Channel Order */
        action_time = msg_ptr->tc_ord.gen.action_time;
        break;

      #ifdef FEATURE_IS2000_REL_A
      case CAI_SECURITY_MODE_MSG:
        action_time = msg_ptr->tcsmcm.action_time;
        break;
      #endif /* FEATURE_IS2000_REL_A */

      default:
        /* Message does not have action time - software madness */
        ERR_FATAL( "Bad msg %d ", msg_ptr->gen_tc.msg_type, 0, 0 );
    } /* end switch (msg_ptr->gen_tc.msg_type) */

    /* ---------------------
    ** Calculate action time
    ** --------------------- */

    /* Get current time */
    ts_get_20ms_frame_time( curr_time );

    /* Get time into 80 ms units */
    phase_adjust = qw_div( curr_time, curr_time, 4U );

    /* Get 80 ms time modulo 64 */
    temp_time = qw_div( curr_time, curr_time, 64U );

    if (temp_time < action_time)
    {
      /* Set action_timer for number of 80 ms remaining */
      wait_time = (word)((action_time - temp_time) * 80);
    }
    else
    {
      wait_time = (word)((64 + action_time - temp_time) * 80);
    }

    /* Compensate for 20ms frame offset into 80ms superframe */
    wait_time -= (word)((phase_adjust * 20));

    if ((msg_ptr->gen_tc.msg_type == CAI_TC_FWD_ORD_MSG) &&
        (msg_ptr->tc_ord.gen.order == CAI_SO_CTL_ORD))
    {
      /* ------------------------------------------------
      ** Process Service Option Control Orders separately
      ** since they have their own timer
      ** ------------------------------------------------ */

      /* Set SO ctl timer */
      (void) mcc_set_callback_timer( &mcc_so_ctl_timer,  wait_time, MCC_SO_CTL_TIMER_ID );

      /* Move message to action message buffer */
      tc_action_so_ctl_order = msg_ptr->tc_ord;

      /* Set use_time to 0 to mark that action time has been processed */
      tc_action_so_ctl_order.gen.use_time = FALSE;
    }

    else /* Process all other messages with action times */
    {
      /* Set action_time timer */
      (void) mcc_set_callback_timer( &mcc_action_timer, wait_time, MCC_ACTION_TIMER_ID );

      switch (msg_ptr->gen_tc.msg_type)
      {
        case CAI_TC_FWD_ORD_MSG:
          /* Move message to action message buffer */
          tc_action_msg.tc_ord = msg_ptr->tc_ord;

          /* Set use_time to 0 to mark that action time has been processed */
          tc_action_msg.tc_ord.gen.use_time = FALSE;

          MSG_MED("Started the action timer for CAI_TC_FWD_ORD_MSG", 0, 0, 0);
          break;

        #ifdef FEATURE_IS2000_REL_A
        case CAI_SECURITY_MODE_MSG:          
          /* Move message to action message buffer */
          tc_action_msg.tcsmcm = msg_ptr->tcsmcm;

          /* Set use_time to 0 to mark that action time has been processed */
          tc_action_msg.tcsmcm.use_time = FALSE;

          MSG_MED("Started the action timer for CAI_SECURITY_MODE_MSG", 0, 0, 0);
          break;
        #endif /* FEATURE_IS2000_REL_A */

        default:
          /* Message does not have action time - software madness */
          ERR_FATAL( "Bad msg %d ",msg_ptr->gen_tc.msg_type, 0, 0 );

      } /* end switch (msg_ptr->gen_tc.msg_type) */
    }
#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */
  }

} /* end tc_action_chk() */

/*===========================================================================

FUNCTION TC_INFORM_RXTX_ABOUT_TX

DESCRIPTION
  This function sends a message to the RXTX (L2) task  that
  the TX is either ON or OFF

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_inform_rxtx_about_tx( void )
{

  if (cdma.tc_tx_on)
  {
    MSG_MED( "Sending RXTX_START_TX_F at %lx",
             txc_get_time_in_1_25ms_unit(), 0, 0 );
    /* send a command to layer 2 to indicate that the transmitter has
    been turned on */
    mcc_rxtx_buf.stop_tx.hdr.command = RXTX_START_TX_F;
    mcc_rxtx_cmd( &mcc_rxtx_buf );
  }
  else
  {
    MSG_MED( "Sending RXTX_STOP_TX_F at %lx",
             txc_get_time_in_1_25ms_unit(), 0, 0 );
    /* send a command to layer 2 to indicate that the transmitter has
    been turned on */
    mcc_rxtx_buf.stop_tx.hdr.command = RXTX_STOP_TX_F;
    mcc_rxtx_cmd( &mcc_rxtx_buf );
  }

} /* start_tx */

/*===========================================================================

FUNCTION SEND_SRV_CTL_MAR

DESCRIPTION
  This function sends a Service Option Control message in response to a
  Retreive Parameters directive.  If this procedure determines that it
  cannot supply a value for a parameter it will instead send a Mobile
 Station Reject Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_srv_ctl_mar
(
  byte vect_counter_id,   /* id of vector of counters to be returned */
  byte con_ref            /* connection reference for service option */
)
{
  dword tsf [13];  /* counter storage */
  word i;
  byte len;
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte *tptr;                  /* pointer to tsf fields */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
    return;
  }

  MSG_MED( "vector to retrieve %d", vect_counter_id, 0, 0 );

  if (vect_counter_id > 7)
  {
    rej_parms.rej_msg_type = CAI_RET_PARMS_MSG;
    rej_parms.rej_code =  CAI_REJ_STRUCT;
    rej_parms.rej_param_id =  0;
    send_rej_order( &rej_parms);
    q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
    return;
  }

  /* Retrieve Markov statistics */
  len = mar_get_vect ( vect_counter_id, tsf);

  cmd_ptr->tc_msg.msg.srv_ctl.tsf[MAR_CTL_TYP] = MAR_RET_DIR;
  cmd_ptr->tc_msg.msg.srv_ctl.tsf[MAR_VECT] = vect_counter_id;

  tptr = &cmd_ptr->tc_msg.msg.srv_ctl.tsf[MAR_VECT+1];

  /* convert vector elements into 24 bit MSB first values */
  for (i=0;i<len;i++)
  {
    //lint -e661 -e662 No out-of-bounds use here
    tptr[3*i+0] = (tsf [i] >> 16) & 0xFF;
    tptr[3*i+1] = (tsf [i] >> 8)  & 0xFF;
    tptr[3*i+2] =  tsf [i]        & 0xFF;
    //lint +e661 +e662
  }

  /* Fill in rest of message */
  cmd_ptr->tc_msg.msg.srv_ctl.msg_type = CAI_RTC_SRV_CTL_MSG;
  cmd_ptr->tc_msg.ack_req = FALSE;
  cmd_ptr->tc_msg.msg.srv_ctl.con_ref = con_ref;
  cmd_ptr->tc_msg.msg.srv_ctl.so = cdma.so_cur_pri;
  cmd_ptr->tc_msg.msg.srv_ctl.tsf_len = (byte) (3 * len + 2);

  /* Fill in the command header */
  mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

  rxtx_cmd( cmd_ptr );

} /* end send_srv_ctl_mar() */

/*===========================================================================

FUNCTION TC_SO_CTL

DESCRIPTION
  This function processes a received Service Option Control Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tc_so_ctl( caii_rx_msg_type *msg_ptr )
{
  boolean valid_ordq = FALSE;
    /* Indicates whether we received a valid ORDQ in the
       Service Option Control Order */
  byte rate_reduc;
  byte mar_mux = CAI_MUX1 - 1;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (cdma.so_cur_pri)
  {
    case CAI_SO_VOICE_IS96A:
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
    case CAI_SO_VOICE_13K_IS733:
    case CAI_SO_VOICE_13K:
#endif
#ifdef FEATURE_EVRC
    case CAI_SO_VOICE_EVRC:
#endif /* FEATURE_EVRC */
      switch (msg_ptr->tc_ord.ordq.ordq & 0x03)
      {
         case 1:
         /* --------------------------------------------------------
         ** If ORDQ equals 'xxx000x1', then the mobile station shall
         ** initialize both the transmitting and receiving sides of
         ** the speech codec.
         ** -------------------------------------------------------- */
         rxc_reset_voc_init ( );
         valid_ordq = TRUE;
         break;

         case 3:
         /* ----------------------------------------------------------------
         ** In addition, if ORDQ equals 'xxx00011', then the mobile station
         ** should disable the audio output of the speech codec for 1 second
         ** after initialization, and the mobile station should process each
         ** received Blank packet as an erasure packet.
         ** ---------------------------------------------------------------- */
         rxc_reset_voc_init ( );
         /* Send a message to the receive task to treat all blank and burst
            messages as erasures if the service option is not EVRC*/
         if (cdma.so_cur_pri != CAI_SO_VOICE_EVRC)
         {
            mcc_rx_buf.erase.hdr.command = RXC_ERASE_F;
            mcc_rxc_cmd( &mcc_rx_buf);
         }
         valid_ordq = TRUE;
         break;

         default:
         break;
      }

      if ((rate_reduc = ((msg_ptr->tc_ord.ordq.ordq & 0xE0) >> 5)) != 0)
      {
         valid_ordq = TRUE;
         /* --------------------------------------------------------
         ** If rate reduc is non zero then mobile station should cause
         ** the speech codec to reduce its rate by the specified factor.
         ** -------------------------------------------------------- */
#if (defined(FEATURE_8K_STUFF_TO_BE_PURGED) && defined(FEATURE_MVS_MIGRATE))
        /* Change vocoder API to MVS API */
        (void) mvs_voc_tx_rate_limit(rate_reduc * 2);
#elif defined (FEATURE_MVS_MIGRATE)
        /* Change vocoder API to MVS API */
       (void) mvssup_voc_tx_rate_limit(rate_reduc * 2);
#else
       (void) voc_tx_rate_limit(rate_reduc * 2);  
#endif /* FEATURE_MVS_MIGRATE */

      }

      if (!valid_ordq)
      {
        rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
        rej_parms.rej_code =  CAI_REJ_FIELD;
        rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
        rej_parms.rej_ordq = msg_ptr->tc_ord.ordq.ordq;
        send_rej_order( &rej_parms);
      }
      break;
  /* --------------------------------------------------------------------
  ** procedures for MARKOV service option control fields
  ** -------------------------------------------------------------------- */
    /*lint -e616 the fallthrough cases are intentional */
    case CAI_SO_MARKOV_13K:
    case CAI_SO_RS2_MARKOV:
      mar_mux = CAI_MUX2 - 1;
    /*lint -fallthrough */
    case CAI_SO_MARKOV:
    case CAI_SO_RS1_MARKOV:
    case CAI_SO_MARKOV_SO54:
    {
      const caii_srv_cfg_type *temp_cfg = snm_get_current_config();  
      /* Correct mar_mux for CAI_SO_MARKOV_SO54 case */
      if ((cdma.so_cur_pri == CAI_SO_MARKOV_SO54)
           && RATE_SET_OF_FWD_RC(temp_cfg->for_fch_rc)==CAI_RATE_14400) //lint !e666 !e641 function argument
      {
        mar_mux = CAI_MUX2 - 1;
      }

      mar_rate = (msg_ptr->tc_ord.ordq.ordq & MAR_TST) >> MAR_TST_SHFT;
      mar_init ( mar_rate,
          (boolean) (msg_ptr->tc_ord.ordq.ordq & MAR_ZERO),
          (boolean) (msg_ptr->tc_ord.ordq.ordq & MAR_COPY),
          cdma.esn, mar_mux );
      break;
    /*lint +e616 */

    }
    default:
    /* --------------------------------------------------------------------
    ** No Service Option Control Orders specified for other service options
    ** -------------------------------------------------------------------- */
      rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
      rej_parms.rej_code =  CAI_REJ_UNSP;
      rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
      rej_parms.rej_ordq = msg_ptr->tc_ord.ordq.ordq;
      send_rej_order( &rej_parms);
  }

} /* end tc_so_ctl() */

#ifdef FEATURE_IS2000_TDSO
/*===========================================================================

FUNCTION SEND_SRV_CTL_TDSO_STAT

DESCRIPTION
  This function sends a Service Option Control message in response to a
  Retrieve TDSO Counters directive.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_srv_ctl_tdso_stat
(
  uint8 ctl_rec_type,      /* Control record type */
  uint8 vect_counter_id,   /* id of vector of counters to be returned */
  uint8 con_ref            /* connection reference for service option */
)
{
  uint32 tsf[36];  /* counter storage */
  word i;
  byte len;
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte *tptr;                  /* pointer to tsf fields */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
    return;
  }

  MSG_MED( "vector to retrieve %d", vect_counter_id, 0, 0 );

#ifdef FEATURE_IS2000_P2
  if ((ctl_rec_type == TDSO_CNT_FCH) || (ctl_rec_type == TDSO_CNT_DCCH)) /*lint !e641 */
#else
  if (ctl_rec_type == TDSO_CNT_FCH) /*lint !e641 */
#endif /* FEATURE_IS2000_P2 */
  {
    if ((vect_counter_id == TDSO_FDCH_5M_TX_CNT) ||  /*lint !e641 */
        (vect_counter_id == TDSO_FDCH_5M_RX_CNT))    /*lint !e641 */
    {
      /* Currently, 5ms frame is not supported */
      rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
      rej_parms.rej_code =  CAI_REJ_CAP;
      send_rej_order( &rej_parms );
      q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
      return;
    }
    else if (vect_counter_id > TDSO_FDCH_5M_RX_CNT) /*lint !e641 */
    {
      /* Invalid vector counter ID */
      rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );
      q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
      return;
    }
  }
  else if ((ctl_rec_type == TDSO_CNT_SCH0) &&   /*lint !e641 */
           (vect_counter_id > TDSO_SCH_TX_CNT))  /*lint !e641 */
  {
    /* Invalid vector counter ID */
    rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
    q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
    return;
  } /* end if (ctl_rec_type == TDSO_CNT_FCH) */

#if defined(FEATURE_IS2000_REL_C_DV) && defined (FEATURE_IS2000_TDSO)
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV && FEATURE_IS2000_TDSO */
  {
      /* Retrieve TDSO statistics */
      len = tdso_get_vect (ctl_rec_type, vect_counter_id, tsf);
  }

  cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_TYPE]    = ctl_rec_type;
  cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_VECT_CNT_ID] = vect_counter_id;

  tptr = &cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_VECT_CNT_ID+1];

  /* convert vector elements into 24 bit MSB first values */
  for (i=0;i<len;i++)
  {
    //lint -e661 -e662 No out-of-bounds use here
    tptr[3*i+0] = (tsf [i] >> 16) & 0xFF;
    tptr[3*i+1] = (tsf [i] >> 8)  & 0xFF;
    tptr[3*i+2] =  tsf [i]        & 0xFF;
    //lint +e661 +e662
  } /* end for */

  /* Fill in rest of message */
  cmd_ptr->tc_msg.msg.srv_ctl.msg_type = CAI_RTC_SRV_CTL_MSG;
  cmd_ptr->tc_msg.ack_req = FALSE;
#if defined(FEATURE_IS2000_REL_C_DV) && defined (FEATURE_IS2000_TDSO)
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV && FEATURE_IS2000_TDSO */
  cmd_ptr->tc_msg.msg.srv_ctl.con_ref = con_ref;
  cmd_ptr->tc_msg.msg.srv_ctl.so = cdma.so_cur_pri;
  cmd_ptr->tc_msg.msg.srv_ctl.tsf_len = (byte) (3 * len + 2);

  /* Fill in the command header */
  mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

  rxtx_cmd( cmd_ptr );

} /* end send_srv_ctl_tdso_stat */

/*===========================================================================

FUNCTION BUILD_TDSO_CTL_DIR

DESCRIPTION
  This function builds TDSO control directive based on the input TDSO test
  parameters.

DEPENDENCIES
  None

RETURN VALUE
  Length of test parameters.

SIDE EFFECTS
  None

===========================================================================*/

uint8 build_tdso_ctl_dir
(
  tdso_tst_par_type tst_par, /* TDSO test parameters */
  uint8             *tsf     /* Control directive storage */
)
{
  uint8 len;

  /* ----------------------------
  ** Pack Channel direction field
  ** ---------------------------- */
  b_packb(tst_par.ch_dir, tsf, 0, 2);

  /* ------------------------
  ** Pack Copy counetrs field
  ** ------------------------ */
  b_packb(tst_par.cpy_cnts, tsf, 2, 1);

  /* -------------------------
  ** Pack Clear counetrs field
  ** ------------------------- */
  b_packb(tst_par.clr_cnts, tsf, 3, 1);

  /* ----------------------
  ** Pack Data source field
  ** ---------------------- */
  b_packb(tst_par.data_src, tsf, 4, 1);

  /* -----------------------
  ** Pack Frame source field
  ** ----------------------- */
  b_packb(tst_par.frame_src, tsf, 5, 2);

  /* -------------------------
  ** Pack Frame activity field
  ** ------------------------- */
  b_packb(tst_par.frame_act, tsf, 7, 1);

  len = 1;

  /* -----------------------
  ** Pack Test options field
  ** ----------------------- */
  tsf[len++] = tst_par.tst_opt;

  /* ------------------------------------------------------------
  ** Pack Number of full-rate frames in the circular buffer field
  ** ------------------------------------------------------------ */
  if (tst_par.frame_src == N_FULL_RATE_FRAME)  /*lint !e641 */
  {
    tsf[len++] = tst_par.num_circ_buf_frames;
  } /* end if (tst_par.data_src) */

  /* ----------------------------------------------------------
  ** If random frame activitity, pack "On" state to "Off" state
  ** transition probability and "Off" state to "On" state
  ** transition probability fields
  ** --------------------------------------------------------- */
  if (tst_par.frame_act)
  {
    tsf[len++] = tst_par.on_to_off_prob;
    tsf[len++] = tst_par.off_to_on_prob;
  }
  /* ------------------------------------------------------------------
  ** If deterministic frame activitity, pack Transmission on period and
  ** Transmission off period fields
  ** ------------------------------------------------------------------ */
  else
  {
    tsf[len++] = tst_par.tx_on_period;
    tsf[len++] = tst_par.tx_off_period;
  } /* end if (tst_par.frame_act) */

  /* ------------------------------------------------------
  ** If If selectable data pattern, pack Data pattern field
  ** ------------------------------------------------------ */
  if (!tst_par.data_src)
  {
    tsf[len++] = tst_par.data_pattern;
  }

  return len;

} /* build_tdso_ctl_dir */

/*===========================================================================

FUNCTION SEND_SRV_CTL_TDSO_CTL_DIR

DESCRIPTION
  This function sends a Service Option Control message that proposes TDSO
  test parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_srv_ctl_tdso_ctl_dir
(
  tdso_tst_par_type fdch_tst_par,   /* FCH or DCCH TDSO test parameters */
  tdso_tst_par_type sch0_tst_par    /* SCH0 TDSO test parameters */
)
{
  uint8 len;                        /* Length of test parameter storage */
  const caii_srv_cfg_type *srv_cfg; /* Service configuration */
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Retrieve the current service configuration */
  srv_cfg = snm_get_current_config();

  /* -------------------------------------------------------
  ** All physical channels use the same TDSO test parameters
  ** ------------------------------------------------------- */
  if (memcmp(&fdch_tst_par, &sch0_tst_par, sizeof(fdch_tst_par)) == 0)
  {
    /* Get buffer from rxtx_mc_free_q to send message in */
    if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
    {
      ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
      return;
    }
    cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_TYPE] = TDSO_CTL_ALL;  /*lint !e641 */
    cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_CODE] = 0;
    len = build_tdso_ctl_dir(fdch_tst_par,
          &cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CHN_DIR]);

    /* Fill in rest of message */
    cmd_ptr->tc_msg.msg.srv_ctl.msg_type = CAI_RTC_SRV_CTL_MSG;
    cmd_ptr->tc_msg.ack_req = TRUE;
    cmd_ptr->tc_msg.msg.srv_ctl.con_ref = srv_cfg->con[0].con_ref;
    cmd_ptr->tc_msg.msg.srv_ctl.so = cdma.so_cur_pri;
    cmd_ptr->tc_msg.msg.srv_ctl.tsf_len = (uint8) (len + 2);

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
  /* -----------------------------------------------
  ** FCH/DCCH and SCH0 use different TDSO test parameters
  ** ----------------------------------------------- */
  else
  {
    /* -------------------------
    ** Control directive for FCH
    ** ------------------------- */
    /* Get buffer from rxtx_mc_free_q to send message in */
    if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
    {
      ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
      return;
    }

#ifdef FEATURE_IS2000_P2
    if (srv_cfg->dcch_cc_incl)
    {
      cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_TYPE] = TDSO_CTL_DCCH;  /*lint !e641 */
    }
    else if (srv_cfg->fch_cc_incl)
    {
      cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_TYPE] = TDSO_CTL_FCH;  /*lint !e641 */
    }
    else
    {
      ERR( "Neither FCH nor DCCH included!", 0, 0, 0 );
    }
#else
    cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_TYPE] = TDSO_CTL_FCH;
#endif /* FEATURE_IS2000_P2 */

    cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_CODE] = 0;
    len = build_tdso_ctl_dir(fdch_tst_par,
          &cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CHN_DIR]);

    /* Fill in rest of message */
    cmd_ptr->tc_msg.msg.srv_ctl.msg_type = CAI_RTC_SRV_CTL_MSG;
    cmd_ptr->tc_msg.ack_req = TRUE;
    cmd_ptr->tc_msg.msg.srv_ctl.con_ref = srv_cfg->con[0].con_ref;
    cmd_ptr->tc_msg.msg.srv_ctl.so = cdma.so_cur_pri;
    cmd_ptr->tc_msg.msg.srv_ctl.tsf_len = (uint8) (len + 2);

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );

    /* --------------------------
    ** Control directive for SCH0
    ** -------------------------- */
    /* Get buffer from rxtx_mc_free_q to send message in */
    if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
    {
      ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
      return;
    }
    cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_TYPE] = TDSO_CTL_SCH0;  /*lint !e641 */
    cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_CODE] = 0;
    len = build_tdso_ctl_dir(sch0_tst_par,
          &cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CHN_DIR]);

    /* Fill in rest of message */
    cmd_ptr->tc_msg.msg.srv_ctl.msg_type = CAI_RTC_SRV_CTL_MSG;
    cmd_ptr->tc_msg.ack_req = TRUE;
    cmd_ptr->tc_msg.msg.srv_ctl.con_ref = srv_cfg->con[0].con_ref;
    cmd_ptr->tc_msg.msg.srv_ctl.so = cdma.so_cur_pri;
    cmd_ptr->tc_msg.msg.srv_ctl.tsf_len = (uint8) (len + 2);

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  } /* end if (fch_tst_par == sch0_tst_par) */

} /* end send_srv_ctl_tdso_ctl_dir */


#endif /* FEATURE_IS2000_TDSO */

/*===========================================================================

FUNCTION TC_SRV_CTL_MSG

DESCRIPTION
  This function processes a received Service Option Control Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tc_srv_ctl_msg( caii_rx_msg_type *msg_ptr )
{
  byte rate;
  byte mar_mux = CAI_MUX1 - 1;
  const caii_srv_cfg_type* csct_ptr = snm_get_current_config();
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

#if defined(FEATURE_IS2000_REL_C_DV) && defined (FEATURE_IS2000_TDSO)
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV && FEATURE_IS2000_TDSO */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* only do this if message's service option and connection reference
     match the current SO and ConRef */

  if ((csct_ptr->con[0].so == msg_ptr->srv_ctl.so) &&
      (csct_ptr->con[0].con_ref == msg_ptr->srv_ctl.con_ref))
  {
    switch (cdma.so_cur_pri)
    {
      case CAI_SO_VOICE_IS96A:
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
      case CAI_SO_VOICE_13K_IS733:
      case CAI_SO_VOICE_13K:
#endif
#ifdef FEATURE_EVRC
      case CAI_SO_VOICE_EVRC:
#endif /* FEATURE_EVRC  */
        rate = ((msg_ptr->srv_ctl.tsf[0] & 0xE0) >> 5);
        if (rate > 4) /* Valid values are 0-4 */
        {
            rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
            rej_parms.rej_code =  CAI_REJ_FIELD;
            send_rej_order( &rej_parms );
            break;
        }
        else
        {
           /* --------------------------------------------------------
           ** If rate reduc is non zero then mobile station should cause
           ** the speech codec to reduce its rate by the specified factor.
           ** To be compatible with the old rate reduction parameters in
           ** the driver we need to multiply the new factor by 2.
           ** -------------------------------------------------------- */
        #if (defined(FEATURE_8K_STUFF_TO_BE_PURGED) && defined(FEATURE_MVS_MIGRATE))
           /* Change vocoder API to MVS API */
          (void) mvs_voc_tx_rate_limit(rate * 2);
        #elif (defined (FEATURE_MVS_MIGRATE))
          /* Change vocoder API to MVS API */
          (void) mvssup_voc_tx_rate_limit(rate * 2);
        #else
          (void) voc_tx_rate_limit(rate * 2);  
        #endif /* FEATURE_MVS_MIGRATE */
        }
        if (msg_ptr->srv_ctl.tsf[0] & 0x02)
        {
           /* ----------------------------------------------------------------
           ** If mobile_to_mobile is set, the mobile station should process each
           ** received Blank packet as an erasure packet.
           ** Send a message to the receive task to treat all blank and burst
           ** messages as erasures.
           ** --------------------------------------------------------------- */
           mcc_rx_buf.erase.hdr.command = RXC_ERASE_F;
           mcc_rxc_cmd( &mcc_rx_buf);
        }
        if (msg_ptr->srv_ctl.tsf[0] & 0x01)
        {
           rxc_reset_voc_init ( );
        }
        break;

#if defined(FEATURE_SO_VOICE_SMV) || defined(FEATURE_VOC_4GV)
#ifdef FEATURE_SO_VOICE_SMV
      case CAI_SO_VOICE_SMV:
#endif /* FEATURE_SO_VOICE_SMV */
#ifdef FEATURE_VOC_4GV
      case CAI_SO_VOICE_4GV_NB:
#endif /* FEATURE_VOC_4GV */ 
        /* --------------------------------------------------------
        ** The mobile station should cause the speech codec to reduce
        ** its rate by the specified factor.
        ** To be compatible with the old rate reduction parameters in
        ** the driver we need to multiply the new factor by 2.
        ** -------------------------------------------------------- */

        rate = ((msg_ptr->srv_ctl.tsf[0] & 0xE0) >> 5);

        #if (defined(FEATURE_8K_STUFF_TO_BE_PURGED) && defined(FEATURE_MVS_MIGRATE))
        /* Change vocoder API to MVS API */
          (void) mvs_voc_tx_rate_limit(rate * 2);
        #elif (defined (FEATURE_MVS_MIGRATE))
        /* Change vocoder API to MVS API */
          (void) mvssup_voc_tx_rate_limit(rate * 2);
        #else
          (void) voc_tx_rate_limit(rate * 2);  
        #endif /* FEATURE_MVS_MIGRATE */

        if (msg_ptr->srv_ctl.tsf[0] & 0x02)
        {
           /* ----------------------------------------------------------------
           ** If mobile_to_mobile is set, the mobile station should process each
           ** received Blank packet as an erasure packet.
           ** Send a message to the receive task to treat all blank and burst
           ** messages as erasures.
           ** --------------------------------------------------------------- */
           mcc_rx_buf.erase.hdr.command = RXC_ERASE_F;
           mcc_rxc_cmd( &mcc_rx_buf);
        }
        if (msg_ptr->srv_ctl.tsf[0] & 0x01)
        {
           rxc_reset_voc_init ( );
        }
        break;
#endif /* defined(FEATURE_SO_VOICE_SMV) || defined(FEATURE_VOC_4GV) */

#ifdef FEATURE_VOC_4GV
      case CAI_SO_VOICE_4GV_WB:
        rate = ((msg_ptr->srv_ctl.tsf[0] & 0xE0) >> 5);
        if ((rate != 0) && (rate != 4) && (rate != 7)) /* Valid values are 0, 4 and 7 */
        {
          rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
          rej_parms.rej_code =  CAI_REJ_FIELD;
          send_rej_order( &rej_parms );
          break;
        }
        else
        {
          /* --------------------------------------------------------
          ** The mobile station should cause the speech codec to reduce
          ** its rate by the specified factor.
          ** To be compatible with the old rate reduction parameters in
          ** the driver we need to multiply the new factor by 2.
          ** -------------------------------------------------------- */
          #if (defined(FEATURE_8K_STUFF_TO_BE_PURGED) && defined(FEATURE_MVS_MIGRATE))
          /* Change vocoder API to MVS API */
            (void) mvs_voc_tx_rate_limit(rate * 2);
          #elif (defined (FEATURE_MVS_MIGRATE))
          /* Change vocoder API to MVS API */
            (void) mvssup_voc_tx_rate_limit(rate * 2);
          #else
            (void) voc_tx_rate_limit(rate * 2);  
          #endif /* FEATURE_MVS_MIGRATE */      
        }

        if (msg_ptr->srv_ctl.tsf[0] & 0x02)
        {
           /* ----------------------------------------------------------------
           ** If mobile_to_mobile is set, the mobile station should process each
           ** received Blank packet as an erasure packet.
           ** Send a message to the receive task to treat all blank and burst
           ** messages as erasures.
           ** --------------------------------------------------------------- */
           mcc_rx_buf.erase.hdr.command = RXC_ERASE_F;
           mcc_rxc_cmd( &mcc_rx_buf);
        }
        if (msg_ptr->srv_ctl.tsf[0] & 0x01)
        {
           rxc_reset_voc_init ( );
        }
        break;
#endif /* FEATURE_VOC_4GV */

    /* --------------------------------------------------------------------
    ** procedures for MARKOV service option control fields
    ** -------------------------------------------------------------------- */

      /*lint -e616 the fallthrough cases are intentional */
      case CAI_SO_MARKOV_13K:
      case CAI_SO_RS2_MARKOV:
        mar_mux = CAI_MUX2 - 1;

      /*lint -fallthrough */ 
      case CAI_SO_MARKOV:
      case CAI_SO_RS1_MARKOV:
      case CAI_SO_MARKOV_SO54:
      {
        const caii_srv_cfg_type *temp_cfg = snm_get_current_config();  
        
        /* Correct mar_mux for CAI_SO_MARKOV_SO54 case */
        if ((cdma.so_cur_pri == CAI_SO_MARKOV_SO54)
             && RATE_SET_OF_FWD_RC(temp_cfg->for_fch_rc)==CAI_RATE_14400) //lint !e666 !e641 function argumet
        {
          mar_mux = CAI_MUX2 - 1;
        }

        /* Test if Control Directive in the CTL_REC_TYPE */
        if (msg_ptr->srv_ctl.tsf[MAR_CTL_TYP] == MAR_CTL_DIR)
        {
          mar_rate = (msg_ptr->srv_ctl.tsf[MAR_CTL] & MAR_TST) >> MAR_TST_SHFT;
          mar_init ( mar_rate,
             (boolean) (msg_ptr->srv_ctl.tsf[MAR_CTL] & MAR_ZERO),
             (boolean) (msg_ptr->srv_ctl.tsf[MAR_CTL] & MAR_COPY),
             cdma.esn, mar_mux );
        }
        /* Test if Counter Retrieval Directive in the CTL_REC_TYPE */
        else if (msg_ptr->srv_ctl.tsf[MAR_CTL_TYP] == MAR_RET_DIR)
        {
          send_srv_ctl_mar (msg_ptr->srv_ctl.tsf[MAR_VECT],
                            msg_ptr->srv_ctl.con_ref );
        }
        break;
        /*lint +e616 */
      }
#ifdef FEATURE_IS2000_TDSO
    /* --------------------------------------------------------------------
    ** procedures for TDSO service option control fields
    ** -------------------------------------------------------------------- */
    case CAI_SO_FULL_TDSO:

#if defined(FEATURE_IS2000_REL_C_DV) && defined (FEATURE_IS2000_TDSO)
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV && FEATURE_IS2000_TDSO */

      /* ----------------------------
      ** FCH / SCH0 Control Directive
      ** ---------------------------- */
      /*lint -e641 */
      if ((msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CTL_ALL) ||
          (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CTL_FCH) ||
#ifdef FEATURE_IS2000_P2
          (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CTL_DCCH) ||
#endif
          (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CTL_SCH0))
      /*lint +e641 */ 
      {
        /* BS supported TDSO test parameters */
        if (((msg_ptr->srv_ctl.tsf[TDSO_CTL_CODE] > 0) &&
             (msg_ptr->srv_ctl.tsf[TDSO_CTL_CODE] < 4)) ||
            (msg_ptr->srv_ctl.tsf[TDSO_CTL_CODE] == 6))
        {
          /* Update FCH / SCH0 TDSO test parameters */
          if (!tdso_process_socm_msg(msg_ptr->srv_ctl.tsf,
              msg_ptr->srv_ctl.tsf_len))
          {
            /* Invalid parameters */
            rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
            rej_parms.rej_code =  CAI_REJ_FIELD;
            send_rej_order( &rej_parms );
          }
        } /* end if (msg_ptr->srv_ctl.tsf[TDSO_CTL_CODE] != 5) */
      }

      /* --------------------------------------
      ** FCH / SCH0 Counter Retrieval Directive
      ** -------------------------------------- */
      /*lint -e641 */
      else if ((msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CNT_FCH) ||
#ifdef FEATURE_IS2000_P2
                (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CNT_DCCH) ||
#endif
                (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CNT_SCH0))
      /*lint +e641 */ 
      {
        send_srv_ctl_tdso_stat(msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE],
         msg_ptr->srv_ctl.tsf[TDSO_VECT_CNT_ID], msg_ptr->srv_ctl.con_ref);
      }

      /* ------------------------------------------------------------
      ** DCCH / SCH1 Control Directive or Counter Retrieval Directive
      ** ------------------------------------------------------------ */
#ifdef FEATURE_IS2000_P2
      else if ((msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CTL_SCH1) ||
               (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CNT_SCH1))
#else

      else if ((msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CTL_DCCH) ||
               (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CTL_SCH1) ||
               (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CNT_DCCH) ||
               (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CNT_SCH1))
#endif /* FEATURE_IS2000_P2 */

      {
        /* Currently, DCCH and SCH1 are not supported */
        rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
        rej_parms.rej_code =  CAI_REJ_CAP;
        send_rej_order( &rej_parms );
      }

      else
      {
        /* Message field was not in valid range */
        rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
        rej_parms.rej_code =  CAI_REJ_FIELD;
        send_rej_order( &rej_parms );
      }
      break;


#endif /* FEATURE_IS2000_TDSO */

    default : // For all other Service Options

      if (snm_process_srv_opt_ctl_msg(msg_ptr) != SNM_VALID)
      {
      /* --------------------------------------------------------------------
      ** This will happen if the SO does not exist or there is a problem
      ** with the internal (type-specific field) format of the message
      ** -------------------------------------------------------------------- */
        rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
        rej_parms.rej_code =  CAI_REJ_CFG;
        send_rej_order( &rej_parms );
      }
    }
  }
  else /* the message had bad service option or con ref */
  {
    MSG_ERROR("Reject SOCM current so=%d con_ref=%d",csct_ptr->con[0].so,csct_ptr->con[0].con_ref,0);
    rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
    rej_parms.rej_code =  CAI_REJ_CFG;
    send_rej_order( &rej_parms );
  }

} /* end tc_srv_ctl_msg() */

/*===========================================================================

FUNCTION BUILD_PIL_STR

DESCRIPTION
  This function builds a Pilot Strength Measurement Message in
  response to a report from the Searcher.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void build_pil_str
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to buffer to place message in */
  mccsrch_rpt_type *rpt_ptr
    /* Pointer to measurement report from searcher */
)
{
  
  word i;    /* Index through pilot strength measurements */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_ptr->pil_str.msg_type = CAI_PIL_STR_MSG;
  msg_ptr->pil_str.ref_pn   = (word) rpt_ptr->tc_meas.ref_pilot;
  msg_ptr->pil_str.keep     = rpt_ptr->tc_meas.keep;


  msg_ptr->pil_str.pilot_strength = rpt_ptr->tc_meas.ecio;
  msg_ptr->pil_str.num_msr = MIN( rpt_ptr->tc_meas.meas_cnt, CAI_PIL_STR_MAX ); /*lint !e834 */

  /**check the valid range of msg_ptr->pil_str.num_msr, if this value
   *exceeds more than maxmum number of pilots then truncate the
   *number of pilot strengths to (SRCH_NSET_MAX + SRCH_ASET_MAX - 1) 
   **/
  if (msg_ptr->pil_str.num_msr > (SRCH_NSET_MAX + SRCH_ASET_MAX - 1))
  {
	  msg_ptr->pil_str.num_msr = (SRCH_NSET_MAX + SRCH_ASET_MAX - 1);
  }

  for( i=0; (i< msg_ptr->pil_str.num_msr); i++ )
  {
    msg_ptr->pil_str.meas[i].pilot_pn_phase =
       rpt_ptr->tc_meas.meas[i].pn_phase;
    msg_ptr->pil_str.meas[i].keep = rpt_ptr->tc_meas.meas[i].keep;
    msg_ptr->pil_str.meas[i].pilot_strength = rpt_ptr->tc_meas.meas[i].ecio;
  } /* for */

} /* end build_pil_str() */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION POPULATE_PIL_REC_INFO_IN_RTC_MSG

DESCRIPTION
  The pilot rec types that the MS/SRCH stores internally for each pilot is
  what the BS gave us but when we are reporting pilot rec types for the same pilots
  back to the BS in a Reverse Traffic Channel (RTC) message the BS is interested
  in only knowing if it is an aux pilot, they do not care about the TD info for
  these pilots as they "know" that already, This function dervives the info that
  needs to be sent back to the BS from the information we have
  stored i.e. in other words it maps pilot rec types referred by BS using table
  3.7.2.3.2.33-1 into table 2.7.2.3.34-1

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if additional pilot rec neds to be included for this, FALSE otherwise.

SIDE EFFECTS
  The destination pilot rec types and pilot rec pointers are populated with correct
  values.

===========================================================================*/

boolean populate_pil_rec_info_in_rtc_msg
(
  const cai_pilot_rec_type  *src_pilot_rec_type, /* Pointer to derive dst pilot rec from */
  const caii_add_pilot_rec_type  * src_pilot_rec, /* Pointer to ilot rec type for the pilot*/
  byte *dst_pilot_rec_type, /* Pointer to where pilot rec type has to be populated */
  caii_add_pilot_rec1_type *dst_pilot_rec /* Pointer where pilot rec has to be populated */
)
{
  /* Indicates if there is pilot information to be included for
     this pilot, For eg. if the pilot is just TD the BS already knows about that
     and we do not have to include that in a RTC message */
  boolean incl_add_pilot_rec_in_rtc = FALSE;
  switch( *src_pilot_rec_type)
  {
    case CAI_PILOT_REC_1X_AUX:
    {
      *dst_pilot_rec_type = CAI_RTC_TYPE_PILOT_REC_1X_AUX;  /*lint !e641 */
      dst_pilot_rec->qof = src_pilot_rec->rec1.qof;
      dst_pilot_rec->walsh_length = src_pilot_rec->rec1.walsh_length;
      dst_pilot_rec->aux_pilot_walsh = src_pilot_rec->rec1.aux_pilot_walsh;
      incl_add_pilot_rec_in_rtc = TRUE;
    }
    break;

    case CAI_PILOT_REC_1X_AUX_TD:
    {
      /* It is a AUX and TD pilot but the BS knows the TD portion of it,
      Need to tell BS only about the aux walsh */
      *dst_pilot_rec_type = CAI_RTC_TYPE_PILOT_REC_1X_AUX;  /*lint !e641 */
      dst_pilot_rec->qof = src_pilot_rec->rec2.qof;
      dst_pilot_rec->walsh_length = src_pilot_rec->rec2.walsh_length;
      dst_pilot_rec->aux_pilot_walsh = src_pilot_rec->rec2.aux_walsh;
      incl_add_pilot_rec_in_rtc = TRUE;
    }
    break;

    case CAI_PILOT_REC_1X_COM_TD:
    case CAI_PILOT_REC_3X_COM:
    case CAI_PILOT_REC_3X_AUX:
    case CAI_PILOT_1X_COMMON:
    {
      /* nothing needs to be done, to satisfy lint and make future linting useful */
      break;
    }
  }
  return incl_add_pilot_rec_in_rtc;

} //populate_pil_rec_info_in_rtc_msg

/*===========================================================================

FUNCTION BUILD_EXT_PIL_STR

DESCRIPTION
  This function builds a Extended Pilot Strength Measurement Message in
  response to a report from the Searcher.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void build_ext_pil_str
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to buffer to place message in */
  mccsrch_rpt_type *rpt_ptr
    /* Pointer to measurement report from searcher */
)
{
  word i;    /* Index through pilot strength measurements */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_ptr->epsmm.msg_type = CAI_EPSMM_MSG;
  msg_ptr->epsmm.ref_pn   = (word) rpt_ptr->tc_meas.ref_pilot;
  msg_ptr->epsmm.keep     = rpt_ptr->tc_meas.keep;

  msg_ptr->epsmm.pilot_strength = rpt_ptr->tc_meas.ecio;
  msg_ptr->epsmm.num_pilots = MIN( rpt_ptr->tc_meas.meas_cnt, CAI_EPSMM_MAX_NUM_PILOTS ); //lint !e734 loss of precision

  msg_ptr->epsmm.ref_pilot_rec_incl =
    populate_pil_rec_info_in_rtc_msg(&rpt_ptr->tc_meas.ref_pilot_rec_type,
                                 &rpt_ptr->tc_meas.ref_pilot_rec,
                                 &msg_ptr->epsmm.ref_pilot_rec_type,
                                 &msg_ptr->epsmm.ref_rec);

  for( i=0; i< msg_ptr->epsmm.num_pilots; i++ )
  {
    msg_ptr->epsmm.meas[i].pilot_pn_phase =
       rpt_ptr->tc_meas.meas[i].pn_phase;
    msg_ptr->epsmm.meas[i].keep = rpt_ptr->tc_meas.meas[i].keep;
    msg_ptr->epsmm.meas[i].pilot_strength = rpt_ptr->tc_meas.meas[i].ecio;
    /* Need to add pilot rec info here */
    msg_ptr->epsmm.meas[i].pilot_rec_incl =
      populate_pil_rec_info_in_rtc_msg(&rpt_ptr->tc_meas.meas[i].pilot_rec_type,
                                   &rpt_ptr->tc_meas.meas[i].pilot_rec,
                                   &msg_ptr->epsmm.meas[i].pilot_rec_type,
                                   &msg_ptr->epsmm.meas[i].rec);
  } /* for */

  msg_ptr->epsmm.sf_rx_power = rpt_ptr->tc_meas.sf_rx_pwr;

} /* end build_ext_pil_str() */
#endif //FEATURE_IS2000_REL_A

/*===========================================================================

FUNCTION BUILD_PPM

DESCRIPTION
  This function builds a Periodic Pilot Strength Measurement Message in
  response to a report from the Searcher.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void build_ppm
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to buffer to place message in */
  mccsrch_rpt_type *rpt_ptr
    /* Pointer to measurement report from searcher */
)
{
  word i;    /* Index through pilot strength measurements */

  #ifdef FEATURE_IS2000_REL_A
  /* ptr to retrieve the non negotiable srv config records */
  caii_non_neg_srv_cfg_type *nnscr;
  #endif /* FEATURE_IS2000_REL_A */

  msg_ptr->per_plt.msg_type  = CAI_PER_PLT_STR_MSG;
  msg_ptr->per_plt.fix.ref_pn    = (word) rpt_ptr->ppsm.ref_plt;
  msg_ptr->per_plt.fix.keep      = rpt_ptr->ppsm.keep;
  msg_ptr->per_plt.fix.sf_rx_pwr = rpt_ptr->ppsm.sf_rx_pwr;

  msg_ptr->per_plt.fix.pilot_str = rpt_ptr->ppsm.ecio;
  msg_ptr->per_plt.fix.num_pilot =
    MIN( rpt_ptr->ppsm.meas_cnt, (SRCH_CSET_MAX + SRCH_ASET_MAX - 1) );

  /* Pilot strength measurement report can hold CAI_PLT_STR_MEAS_MAX number of 
  ** reports so set the maximum number of pilots to CAI_PLT_STR_MEAS_MAX if 
  ** number of pilots value is greater than CAI_PLT_STR_MEAS_MAX value
  */
  if (msg_ptr->per_plt.fix.num_pilot > CAI_PLT_STR_MEAS_MAX)
  {
	 msg_ptr->per_plt.fix.num_pilot = CAI_PLT_STR_MEAS_MAX;
  }

  for( i=0; i < msg_ptr->per_plt.fix.num_pilot; i++ )
  {
    msg_ptr->per_plt.var[i].plt_pn_phase = rpt_ptr->ppsm.meas[i].pn_phase;
    msg_ptr->per_plt.var[i].keep = rpt_ptr->ppsm.meas[i].keep;
    msg_ptr->per_plt.var[i].plt_str = rpt_ptr->ppsm.meas[i].ecio;
#ifdef FEATURE_IS2000_REL_A
    if (P_REV_IS_7_OR_GREATER)
    {
      msg_ptr->per_plt.var2[i].pilot_rec_incl =
        populate_pil_rec_info_in_rtc_msg(&rpt_ptr->ppsm.meas[i].pilot_rec_type,
                                         &rpt_ptr->ppsm.meas[i].pilot_rec,
                                         &msg_ptr->per_plt.var2[i].pilot_rec_type,
                                         &msg_ptr->per_plt.var2[i].rec);
    }
#endif
  } /* for */

#ifdef FEATURE_IS2000_REL_A
  /* The setpt_incl field is applicable only to PREV 7 and later.
    However, we don't check PREV here because the field is always
    populated correctly for any PREV */
  msg_ptr->per_plt.fix2.setpt_incl = mcctc_ppsm_setpt_incl;

  // Only include setpoint information if it was requested in the
  // order message.
  if (mcctc_ppsm_setpt_incl)
  {
    /* Now get set pt info from RXC */
    mcc_rx_buf.setpt.hdr.command = RXC_GET_SETPT_F;
    mcc_rxc_cmd( &mcc_rx_buf);

    /* After the command is done, the setpt values would
    have been populated in the same buffer, access them now for
    completing PPSMM message */
    if (mcc_rx_buf.setpt.hdr.status != RXC_DONE_S)
    {
       ERR_FATAL("Setpoint request to RXC returned failure code.",0,0,0);
    }
    else
    {
#ifdef FEATURE_IS2000_REL_C
      if (P_REV_IS_9_OR_GREATER) 
      {
        msg_ptr->per_plt.fix2.fch_incl = mcc_rx_buf.setpt.fch_incl;
        msg_ptr->per_plt.fix2.fpc_fch_curr_setpt =
            mcc_rx_buf.setpt.fpc_fch_curr_setpt;
        msg_ptr->per_plt.fix2.dcch_incl = mcc_rx_buf.setpt.dcch_incl;
        msg_ptr->per_plt.fix2.fpc_dcch_curr_setpt =
            mcc_rx_buf.setpt.fpc_dcch_curr_setpt;
      } 
      else 
#endif /* FEATURE_IS2000_REL_C */
      {
        nnscr = (caii_non_neg_srv_cfg_type *) snm_get_current_nnscr();
        if (nnscr->fpc_pri_chan == 0)
        {
          msg_ptr->per_plt.fix2.fch_incl = mcc_rx_buf.setpt.fch_incl;
          msg_ptr->per_plt.fix2.fpc_fch_curr_setpt =
              mcc_rx_buf.setpt.fpc_fch_curr_setpt;
          msg_ptr->per_plt.fix2.dcch_incl = 0;
        }
        else
        {
          msg_ptr->per_plt.fix2.fch_incl = 0;
          msg_ptr->per_plt.fix2.dcch_incl = mcc_rx_buf.setpt.dcch_incl;
          msg_ptr->per_plt.fix2.fpc_dcch_curr_setpt =
              mcc_rx_buf.setpt.fpc_dcch_curr_setpt;
        }
      }

      msg_ptr->per_plt.fix2.num_sup = mcc_rx_buf.setpt.num_sup;
      for (i=0; i<msg_ptr->per_plt.fix2.num_sup; i++)
      {
        msg_ptr->per_plt.fix2.sup[i].sch_id =
            mcc_rx_buf.setpt.sch_setpt_info[i].sch_id;
        msg_ptr->per_plt.fix2.sup[i].fpc_sch_curr_setpt =
            mcc_rx_buf.setpt.sch_setpt_info[i].fpc_sch_curr_setpt;
      } // for loop
    }
  } // if setpt_incl
#endif //FEATURE_IS2000_REL_A

} /* build_ppm */

/*===========================================================================

FUNCTION REQ_PIL_STR

DESCRIPTION
  This function asks SRCH to send a Pilot Strength Measurement message,
  in response to a Pilot strength measurement request order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void req_pil_str( void )
{
  /* Send a request to the Searcher for the Pilot strength report */
  mcc_srch_buf.meas.hdr.cmd = SRCH_MEAS_F;
  mcc_srch_cmd( &mcc_srch_buf );

} /* req_pil_str */

/*===========================================================================

FUNCTION REQ_PPM_RPT

DESCRIPTION
  This function asks SRCH to send a Period Pilot Strength Measurement message,
  in response to a Periodic Pilot strength measurement request order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void req_ppm_rpt(mccrxtx_msg_type *msg_ptr)
{
#ifdef FEATURE_IS2000_REL_A
    // Save flag indicating if setpoint information should be included
    // in future Periodic Pilot Strength Measurement Messages.
    if (P_REV_IS_7_OR_GREATER)
    {
      mcctc_ppsm_setpt_incl= msg_ptr->msg.tc_ord.ppm.incl_setpt;
    }
    else
    {
      mcctc_ppsm_setpt_incl= FALSE;
    }
#endif //FEATURE_IS2000_REL_A

    /* First check the ORDQ of the received message */
    if (msg_ptr->msg.tc_ord.ppm.ordq == 0xFF)
    {
      /* If ORDQ == 0xFF, then command SRCH for a One time PPSMM Report
       */

      /* Set the cmd to "one time PPSMM"
       */
      mcc_srch_buf.meas.hdr.cmd = SRCH_PPSM_F;

      if (mcctc_ppsm_enabled == TRUE)
      {
        /* If we are here means that a continous PPSMM is enabled, sending
         * a one time PPSMM will cause the SRCH to abort continous PPSMM.
         * So, set the mcctcsup_ppsmm_enabled to FALSE
         */
        mcctc_ppsm_enabled = FALSE;
      }
    }
    else
    {
      /* If ORDQ != 0xFF, then command SRCH for it to start maintaining
       * a PPSMM timer as per IS-95B, 6.6.6.2.12
       */

      /* Set the cmd to "Continous PPSMM "
       */
      mcc_srch_buf.meas.hdr.cmd = SRCH_START_PPSM_F;

      /* Convert the received ORDQ value into units of ms and send the
       * resultant value, along with following threshold values, to SRCH
       */
      mcc_srch_buf.ppsmm.per_timer = (msg_ptr->msg.tc_ord.ppm.ordq * 80);
      mcc_srch_buf.ppsmm.min_pil_pwr_thr = msg_ptr->
        msg.tc_ord.ppm.min_pilot_pwr_thresh;
      mcc_srch_buf.ppsmm.min_pil_ec_io_thr = msg_ptr->
        msg.tc_ord.ppm.min_pilot_ec_io_thresh;

      /* Set the mcctcsup_ppsmm_enable to TRUE. MC uses this flag to
       * determine wheter the PPSMM is enabled or not. If
       * mcctcsup_ppsmm_enable is set to TRUE, it means that PPSMM Processing
       * is enabled, else disabled. MC uses this flag to determine whether
       * to really send a SRCH_STOP_PPSM_F to SRCH in case of GHDM, EHDM
       * processing and one time PPSMM. MC will only send SRCH_STOP_PPSM_F if
       * PPSMM is enable. This way MC will not unneccsarily send CMDS to SRCH
       */
      mcctc_ppsm_enabled = TRUE;
    }

  /* Send the command to the Searcher
   */
  mcc_srch_cmd( &mcc_srch_buf );

} /* req_ppm_rpt */

/*===========================================================================

FUNCTION BUILD_PWR_MEAS

DESCRIPTION
  This function builds a Power Measurement report from an RX task
  power measurement report.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void build_pwr_meas
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to Power Measurement Report Message */
  mccrx_rpt_type *rpt_ptr
    /* Pointer to RX task power measurement report */
)
{
  word i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_ptr->pwr_rpt.msg_type = CAI_PWR_MSR_MSG;

  if (rpt_ptr->pwr.errors_detected > CAI_5_BIT_FIELD_MAX) 
    msg_ptr->pwr_rpt.errors_detected = CAI_5_BIT_FIELD_MAX;
  else
    msg_ptr->pwr_rpt.errors_detected = rpt_ptr->pwr.errors_detected;

  msg_ptr->pwr_rpt.pwr_meas_frames = rpt_ptr->pwr.pwr_meas_frames;

  msg_ptr->pwr_rpt.last_hdm_seq    = cdma.hdm_seq;

  /* Get Active Set pilot strength measurements from Searcher */
  mcc_srch_buf.fpc_meas.hdr.cmd = SRCH_FPC_MEAS_F;
  mcc_srch_cmd( &mcc_srch_buf );

  msg_ptr->pwr_rpt.num_pilots = (byte) mcc_srch_buf.fpc_meas.meas_cnt;

  if ((msg_ptr->pwr_rpt.num_pilots == 0) ||
      (msg_ptr->pwr_rpt.num_pilots > cai_tmo.n6m))
  {
    ERR_FATAL( "Invalid Active Set size %d",
                msg_ptr->pwr_rpt.num_pilots, 0, 0 );
  }

  for (i=0; i < msg_ptr->pwr_rpt.num_pilots; i++)
  {
    if (mcc_srch_buf.fpc_meas.meas[i].ecio > CAI_6_BIT_FIELD_MAX) 
      msg_ptr->pwr_rpt.pilot_strength[i] = CAI_6_BIT_FIELD_MAX;
    else
      msg_ptr->pwr_rpt.pilot_strength[i] = mcc_srch_buf.fpc_meas.meas[i].ecio;
  }

  #ifdef FEATURE_IS2000_SCH
  // We may need to add the SCH FER report portion as well

  if ( (msg_ptr->pwr_rpt.sch_pwr_meas_incl = rpt_ptr->pwr.sch_pwr_meas_included) != FALSE )
  {
    msg_ptr->pwr_rpt.sch_id = rpt_ptr->pwr.sch_id;

    if (rpt_ptr->pwr.sch_errors_detected > CAI_10_BIT_FIELD_MAX) 
      msg_ptr->pwr_rpt.sch_errors_detected = CAI_10_BIT_FIELD_MAX;
    else 
      msg_ptr->pwr_rpt.sch_errors_detected = rpt_ptr->pwr.sch_errors_detected;

    msg_ptr->pwr_rpt.sch_pwr_meas_frames = rpt_ptr->pwr.sch_pwr_meas_frames;
  }
  #else
  // SCH not supported
  msg_ptr->pwr_rpt.sch_pwr_meas_incl = FALSE;
  #endif // FEATURE_IS2000_SCH
  #ifdef FEATURE_IS2000_P2
  if ( (msg_ptr->pwr_rpt.dcch_pwr_meas_incl = rpt_ptr->pwr.dcch_pwr_meas_incl) != FALSE)
  {
    msg_ptr->pwr_rpt.dcch_pwr_meas_frames = rpt_ptr->pwr.dcch_pwr_meas_frames;
    if (rpt_ptr->pwr.dcch_errors_detected > CAI_5_BIT_FIELD_MAX) 
      msg_ptr->pwr_rpt.dcch_errors_detected = CAI_5_BIT_FIELD_MAX;
    else
      msg_ptr->pwr_rpt.dcch_errors_detected = rpt_ptr->pwr.dcch_errors_detected;
  }
  #else
  // DCCH not supported
  msg_ptr->pwr_rpt.dcch_pwr_meas_incl = FALSE;
  #endif // FEATURE_IS2000_P2
} /* end build_pwr_meas() */

/*===========================================================================

FUNCTION PROCESS_PWR_MSG

DESCRIPTION
  This function processes an input Power Control Parameters Message.
  In response to this message a Power Measurement Report message is
  formatted.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_pwr_msg
(
  caii_rx_msg_type *msg_ptr
   /* Pointer to received Power Control Parameters Message */
)
{
  /* -------------------------------------------------
  ** Send power control parameters to the Receive task
  ** ------------------------------------------------- */
  mcc_rx_buf.pwr.hdr.command = RXC_PWR_F;

  mcc_rx_buf.pwr.pwr_thresh_enable = msg_ptr->pwr_ctl.pwr_thresh_enable;
  mcc_rx_buf.pwr.pwr_period_enable = msg_ptr->pwr_ctl.pwr_period_enable;
  mcc_rx_buf.pwr.pwr_rep_thresh    = msg_ptr->pwr_ctl.pwr_rep_thresh;
  mcc_rx_buf.pwr.pwr_rep_frames =
    pwr_rep_frames[msg_ptr->pwr_ctl.pwr_rep_frames];
  mcc_rx_buf.pwr.pwr_rep_delay = msg_ptr->pwr_ctl.pwr_rep_delay;

  mcc_rxc_cmd( &mcc_rx_buf );

  /* Set power control reporting state indicators */
  mcc_rx_periodic_rpt_on  = msg_ptr->pwr_ctl.pwr_period_enable;
  mcc_rx_threshold_rpt_on = msg_ptr->pwr_ctl.pwr_thresh_enable;

} /* end process_pwr_msg() */

/*===========================================================================

FUNCTION TC_PROC_ACK

DESCRIPTION
  This function processes messages for which RXTX is required to signal
  MC when the acknowldegement is received for the message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Command buffer will be placed back on the rxtx_md_free_q.

===========================================================================*/

void tc_proc_ack
(
  rxtx_cmd_type *cmd_ptr
    /* Pointer to rxtx command taken from mc_rxtx_ack_q */
)
{
#ifdef FEATURE_AUTH
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
#endif /* FEATURE_AUTH */

#if defined(FEATURE_IS2000_REL_B) && defined(FEATURE_GPS_MODE)
#error code not present
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (cmd_ptr->tc_msg.msg.gen.msg_type)
  {
    case CAI_PWR_MSR_MSG:
      /* -----------------------------------------------
      ** Command RXC to commence PWR_REP_DELAY countdown
      ** ----------------------------------------------- */
      mcc_rx_buf.pwr_on.hdr.command = RXC_PWR_REP_DELAY_F;
      mcc_rx_buf.pwr_rep_delay.pwr_thresh_enable = mcc_rx_threshold_rpt_on;
      mcc_rx_buf.pwr_rep_delay.pwr_period_enable = mcc_rx_periodic_rpt_on;
      mcc_rxc_cmd( &mcc_rx_buf );
      break;

    case CAI_TC_REV_ORD_MSG:
    {
      /* Order message - check for Start or Stop Continous DTMF Order */
      if(( cmd_ptr->tc_msg.msg.tc_ord.gen.order == CAI_DTMF_ORD )
         #ifdef FEATURE_JCDMA_2
         || (cmd_ptr->tc_msg.msg.tc_ord.gen.order == CAI_MS_CONNECT_ORD)
         #endif /* FEATURE_JCDMA_2 */
        )
      {
        /* Needs to be handled by CCIM */
        mccccim_process_ack( cmd_ptr );
      }
#ifdef FEATURE_AUTH
      else if (cmd_ptr->tc_msg.msg.tc_ord.gen.order == CAI_BS_CHAL_ORD)
      {
        /* --------------------------------------------------
        ** We rxed the ack to a Base Station Challenge Order.
        ** Tell auth to set timer in case the Base Station
        ** Challenge Confirmation Order doesn't come in T64m
        ** -------------------------------------------------- */

        if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
        {
          auth_cmd_ptr->hdr.command = AUTH_START_TIMER_F;
          auth_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
          auth_cmd( auth_cmd_ptr );
        }
        else
        {
          ERR( "No free buffers on auth_free_q", 0, 0, 0 );
        }
      }
#endif /* FEATURE_AUTH */

      break;
    }

    case CAI_SEND_DTMF_MSG:
    {
      /* Needs to be handled by CCIM */
      mccccim_process_ack( cmd_ptr );
      break;
    }

    case CAI_TC_REV_BURST_MSG:
    {
#ifdef FEATURE_CDSMS
      /* L2 ACK for the data burst is successfully received,
         let's first figure out what burst type it is */
      if( cmd_ptr->tc_msg.msg.tc_burst.burst_type == CAI_SHORT_MSG_SERVICES )
      {
        /* If burst type is SMS, notify SMS of the ACK */
        mcc_send_mo_dbm_status( (byte) CAI_SHORT_MSG_SERVICES,
                                CM_DBM_MO_OK );
      }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_DBM
      if( (cmd_ptr->tc_msg.msg.tc_burst.burst_type == CAI_POSITION_DET) ||
          (cmd_ptr->tc_msg.msg.tc_burst.burst_type == CAI_SHORT_DATA_BURST))

      {
        /* Notify CMDBM of the ACK */
        mcc_send_mo_dbm_status( (byte)cmd_ptr->tc_msg.msg.tc_burst.burst_type,
                                CM_DBM_MO_OK );
      }
#endif /* FEATURE_DBM */

      break;
    }

    case CAI_REV_FLASH_MSG:
#ifdef FEATURE_IS2000_REL_A
    case CAI_EFWIM_MSG:
#endif
    {
      /* Needs to be handled by CCIM */
      mccccim_process_ack( cmd_ptr );
      break;
    }

#if defined(FEATURE_IS2000_REL_B) && defined(FEATURE_GPS_MODE)
#error code not present
#endif /* FEATURE_IS2000_REL_B && FEATURE_GPS_MODE */

#ifdef FEATURE_IS2000_REL_A
    case CAI_EOM_MSG:
    {
      /* Mark L2 ACK received for EOM. */
      mcctctag_set_l2_ack_rxed_by_tag_id(cmd_ptr->tc_msg.msg.eom.tag);
      break;
    }
#endif /* FEATURE_IS2000_REL_A */

    #if ((defined FEATURE_IS2000_REL_C_AKA) && (defined FEATURE_IS2000_REL_D))
#error code not present
    #endif /* FEATURE_IS2000_REL_C_AKA && FEATURE_IS2000_REL_D */

    default:
    {
      break;
    }
  } /* switch */

  /* put message back on to free q */
  q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );

} /* tc_proc_ack */

/*===========================================================================

FUNCTION TC_LOCK_ORD

DESCRIPTION
  This function processes a Lock Until Power-Cycled order, a Maintenance
  Required Order, and an Unlock Order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

word tc_lock_ord
(
  caii_ftc_ord_type *msg_ptr,
    /* Pointer to received Forward Traffic Channel Order Message */
  word curr_state
    /* Traffic Channel substate currently being processed */
)
{
  word next_state = curr_state;
    /* Next Traffic Channel state to be processed */
  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to UI command buffer */
  mccccim_event_type mccccim_evt;
    /* Buffer for sending event to CCIM */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr->ordq.ordq == CAI_UNLOCK_MASK)
  {
    /* ------------------------------------------
    ** We are not locked. Ignore an unlock order.
    ** ------------------------------------------ */
    MSG_MED( "Rxed Unlock Order", 0, 0, 0 );
  }
  else if (msg_ptr->ordq.ordq & CAI_LUPC_MASK)
  {
    /* -----------------------------
    ** Lock Until Power-Cycled Order
    ** ----------------------------- */
    MSG_MED( "Rxed Lock Order", 0, 0, 0 );

    /* We got a lock order, Delete all CC instances */
    mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
    mccccim_evt.del_all_cc.del_reason = MCCCCIM_LOCK;
    mccccim_process_event( &mccccim_evt );
    /* No need to check status */

    next_state    = CDMA_EXIT;
    mcc_exit.exit = MCC_LOCK;

    /* Send a message to TXC to stop transmitting */
    mcctxns_begin_tx_on_off_trans(FALSE);

    /* Write reason for lock to NVRAM */
    mcc_nv_data.lckrsn_p.lock_order  = TRUE;
    mcc_nv_data.lckrsn_p.lock_reason = msg_ptr->ordq.ordq & 0x0F;

    mcc_nv_buf.item = NV_LCKRSN_P_I;
    mcc_nv_buf.cmd  = NV_WRITE_F;
    mcc_nv_buf.data_ptr = &mcc_nv_data;

    (void) mcc_nv_cmd( &mcc_nv_buf );

    /* Send lock command to UI */

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->hdr.cmd = CM_LOCK_F;
      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
  }
  else if (msg_ptr->ordq.ordq & CAI_MAINT_REQ_MASK)
  {
    /* --------------------------
    ** Maintenance Required Order
    ** -------------------------- */
    MSG_MED( "Rxed Maint Required Order", 0, 0, 0 );

    /* Write reason for maintenance required to NVRAM */
    mcc_nv_data.maintrsn.maint_order  = TRUE;
    mcc_nv_data.maintrsn.maint_reason = msg_ptr->ordq.ordq & 0x0F;

    mcc_nv_buf.item = NV_MAINTRSN_I;
    mcc_nv_buf.cmd  = NV_WRITE_F;
    mcc_nv_buf.data_ptr = &mcc_nv_data;

    (void) mcc_nv_cmd( &mcc_nv_buf );

    /* Send maintenance required command to UI */

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->hdr.cmd = CM_MAINTREQ_F;
      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
  }
  else
  {
    /* -------------------------------------------------------
    ** Send a Mobile Station Reject Order (invalid ordq field)
    ** ------------------------------------------------------- */
    rej_parms.rej_order = CAI_LOCK_OR_MAINT_ORD;
    rej_parms.rej_ordq = msg_ptr->ordq.ordq;
    rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
  }

  return (next_state);

} /* end tc_lock_ord() */

/*===========================================================================

FUNCTION MCC_CALLING_PARTY_NUM_INFO_REC

DESCRIPTION
  This function builds a specified information record into a specified
  buffer.
  This function can potentially being called by functions sending ORIG_C,
  FLASH, or EOM.

DEPENDENCIES
  This function should be called if num_char is > 0.

RETURN VALUE
   This function returns an integer value as follows:
     0 - if the requested record will not fit in the specified buffer.
     n - where n is a positive integer indicating the size, in bytes,
         of the requested information record.

SIDE EFFECTS
  None

===========================================================================*/
int mcc_calling_party_num_info_rec
(
  mc_calling_party_number_type *calling_party_number,
    /* Pointer to the data structure that stores Calling Praty Number */
  void *buf_ptr,
    /* Pointer to place to build information record. */
  word buf_siz
    /* Size of information record buffer, in bytes */
)
{
  int rec_size = 0;    /* return value */
  int i;               /* Loop variable */

  /* This function should be called if there is characters */
  if ( calling_party_number->num_char > 0 )
  {
    rec_size = sizeof(caii_calling_rec_fix_type) +
      calling_party_number->num_char * sizeof (caii_calling_rec_var_type);

    if ( rec_size <= buf_siz )
    {
      ((caii_calling_rec_type *) buf_ptr)->fix.hdr.record_type = CAI_REV_CALLING_REC;

      ((caii_calling_rec_type *) buf_ptr)->fix.number_type =
        calling_party_number->number_type;

      ((caii_calling_rec_type *) buf_ptr)->fix.number_plan =
        calling_party_number->number_plan;

      ((caii_calling_rec_type *) buf_ptr)->fix.pi =
        calling_party_number->pi;  /*lint !e641 */

      ((caii_calling_rec_type *) buf_ptr)->fix.si =
        calling_party_number->si;  /*lint !e641 */

      ((caii_calling_rec_type *) buf_ptr)->fix.num_char =
        calling_party_number->num_char;

      for (i=0; i<calling_party_number->num_char; i++)
      {
        ((caii_calling_rec_type *) buf_ptr)->var[i] =
          calling_party_number->chari[i];
      }
    }
    else
    {
      rec_size = 0;
    }
  }

  return rec_size;

} /* mcc_calling_party_num_info_rec */

/*===========================================================================

FUNCTION MCC_SUBADDRESS_INFO_REC

DESCRIPTION
  This function builds a specified information record into a specified
  buffer.
  This function can potentially being called by functions sending ORIG_C,
  FLASH, or EOM.

DEPENDENCIES
  This function should be called if num_char is > 0.

RETURN VALUE
   This function returns an integer value as follows:
     0 - if the requested record will not fit in the specified buffer.
     n - where n is a positive integer indicating the size, in bytes,
         of the requested information record.

SIDE EFFECTS
  None

===========================================================================*/
int mcc_subaddress_info_rec
(
  byte subaddr_type,
    /* Subaddress type to be processed */
  mc_subaddress_type *subaddress,
    /* Pointer to the data structure that stores subaddress info */
  void *buf_ptr,
    /* Pointer to place to build information record. */
  word buf_siz
    /* Size of information record buffer, in bytes */
)
{
  int rec_size = 0;    /* return value */
  int i;               /* Loop variable */

  if ( subaddress->num_char > 0 )
  {
    rec_size = sizeof(caii_subaddr_rec_fix_type) +
      subaddress->num_char * sizeof (caii_subaddr_rec_var_type);

    if (rec_size <= buf_siz)
    {
      ((caii_subaddr_rec_type *) buf_ptr)->fix.hdr.record_type = subaddr_type;

      ((caii_subaddr_rec_type *) buf_ptr)->fix.extension_bit =
        subaddress->extension_bit;

      ((caii_subaddr_rec_type *) buf_ptr)->fix.subadd_type =
        subaddress->subadd_type;  /*lint !e641 */

      ((caii_subaddr_rec_type *) buf_ptr)->fix.odd_even_ind =
        subaddress->odd_even_ind;  /*lint !e641 */

      ((caii_subaddr_rec_type *) buf_ptr)->fix.num_char =
        subaddress->num_char;

      for (i=0; i<subaddress->num_char; i++)
      {
        ((caii_subaddr_rec_type *) buf_ptr)->var[i] =
          subaddress->chari[i];
      }
    }
    else
    {
      rec_size = 0;
    }
  }

  return rec_size;

} /* mcc_subaddress_info_rec */

/*===========================================================================

FUNCTION SEND_ORIG_C_MSG

DESCRIPTION
  This function sends an Origination Continuation Message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void send_orig_c_msg( void )
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  word i;
   /* Index */
  word start_digit;
   /* First digit not yet sent */
  int rec_size;
   /* record size */
  int all_rec_size = 0;
   /* Total record size */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
    /* -----------------------------------------------------------
    ** Set Digit Mode Indicator for either DTMF code or ASCII mode
    ** ----------------------------------------------------------- */

    cmd_ptr->tc_msg.msg.orig_c.msg_type   = CAI_ORIG_C_MSG;

    MSG_MED("Sending ORIG_C with:", 0, 0, 0);

    /* Fill in remaining digits if there is any */
    cmd_ptr->tc_msg.msg.orig_c.num_fields = cdma.remaining_digits;

    if (cdma.remaining_digits > 0)
    {
      cmd_ptr->tc_msg.msg.orig_c.digit_mode = cdma.digit_mode;
      start_digit = cdma.dial.num_digits - cdma.remaining_digits;

      for (i = 0; i < cdma.remaining_digits; i++)
      {
        cmd_ptr->tc_msg.msg.orig_c.chari[i] = cdma.dial.digits[start_digit + i];
      }

      MSG_MED("  Called Party Number %d Digits", cdma.remaining_digits, 0, 0);
    }

    /* Fill in address info record if there is any */
    /* Default num_recs to 0 */
    cmd_ptr->tc_msg.msg.orig_c.num_recs = 0;

    /* Add calling party number */
    if ( cdma.calling_party_number.num_char > 0 )
    {
      rec_size = mcc_calling_party_num_info_rec(&cdma.calling_party_number,
                                                (void*) &cmd_ptr->tc_msg.msg.orig_c.recs[0],
                                                (word)sizeof(cmd_ptr->tc_msg.msg.orig_c.recs)
                                                );
      if (rec_size > 0)
      {
        /* We got a record */
        cmd_ptr->tc_msg.msg.orig_c.num_recs++;
        all_rec_size += rec_size;

        /* Adjust alignment */
        all_rec_size = ALIGN_DWORD((dword)all_rec_size); //lint !e713 loss of precision

        MSG_MED("  Calling Party Number %d Digits",
            cdma.calling_party_number.num_char, 0, 0);
      }
      else
      {
        MSG_ERROR("No Room for Calling Pty Num", 0, 0, 0);
      }
    }

    /* Add Calling Party Subaddress Info */
    if (cdma.calling_party_subaddr.num_char > 0)
    {
      rec_size = mcc_subaddress_info_rec(CAI_REV_CALLING_SUB_REC,
                                         &cdma.calling_party_subaddr,
                                         (void*) &cmd_ptr->tc_msg.msg.orig_c.recs[all_rec_size/sizeof(dword)], //lint !e573 !e737 signed/unsigned mix
                                         (word)sizeof(cmd_ptr->tc_msg.msg.orig_c.recs)-all_rec_size
                                        ); //lint !e734 loss of precision in arg4
      if (rec_size > 0)
      {
        /* We got a record */
        cmd_ptr->tc_msg.msg.orig_c.num_recs++;
        all_rec_size += rec_size;

        /* Adjust alignment */
        all_rec_size = ALIGN_DWORD((dword)all_rec_size); //lint !e713 loss of precision

        MSG_MED("  Calling Party Subaddr %d Digits",
            cdma.calling_party_subaddr.num_char, 0, 0);
      }
      else
      {
        MSG_ERROR("No Room for Calling Pty SubAddr", 0, 0, 0);
      }
    }

    /* Add Called Party Subaddress */
    if (cdma.called_party_subaddr.num_char > 0)
    {
      rec_size = mcc_subaddress_info_rec(CAI_REV_CALLED_SUB_REC,
                                         &cdma.called_party_subaddr,
                                         (void*) &cmd_ptr->tc_msg.msg.orig_c.recs[all_rec_size/sizeof(dword)], //lint !e573 !e737 signed/unsigned mix
                                         (word)sizeof(cmd_ptr->tc_msg.msg.orig_c.recs)-all_rec_size
                                        ); //lint !e734 loss of precision in arg4
      if (rec_size > 0)
      {
        /* We got a record */
        cmd_ptr->tc_msg.msg.orig_c.num_recs++;
        all_rec_size += rec_size;

        /* Adjust alignment */
        all_rec_size = ALIGN_DWORD((dword)all_rec_size); //lint !e713 precision loss

        MSG_MED("  Called Party Subaddr %d Digits",
            cdma.called_party_subaddr.num_char, 0, 0);

      }
      else
      {
        MSG_ERROR("No Room for Called Pty SubAddr", 0, 0, 0);
      }
    }

    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );

    /* Ensure Orig_c is one shot, just for safety */
    cdma.remaining_digits = 0;
    cdma.calling_party_number.num_char = 0;
    cdma.called_party_subaddr.num_char = 0;
    cdma.calling_party_subaddr.num_char = 0;
  }
} /* send_orig_c_msg() */

/*===========================================================================

FUNCTION TC_MOB_REG

DESCRIPTION
  This function processes the Mobile Station Registered Message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The roam status and the registration variables will be updated.

===========================================================================*/
void tc_mob_reg
(
  caii_registered_type *msg_ptr
    /* pointer to received mobile station registered message */
)
{
  /* Stores new SID, NID and notify its reception if a handler function is registered. */
  mccsyobj_process_epzid(msg_ptr->sid, msg_ptr->nid, 
    mccsyobj_get_pzid(), CDMA_TC);

  /* ------------------------------------------------------------------
  ** There are some serving system related info coming in in this msg.
  ** Give the info to CM.
  ** ------------------------------------------------------------------ */
  mcc_send_cm_srv_sys_info( CM_SS_SID_MASK|
                            CM_SS_NID_MASK|
                            CM_SS_REG_ZONE_MASK|
                            CM_SS_BS_P_REV_MASK|
                            CM_SS_P_REV_IN_USE_MASK,  /*lint !e747 */                            
                            msg_ptr->sid,
                            msg_ptr->nid,
                            msg_ptr->reg_zone,
                            mccsyobj_get_pzid()
                          );

  /* Update registration variables */
  mccreg_ms_reg( msg_ptr );

}/* tc_mob_reg */

/*===========================================================================

FUNCTION PROCESS_FTC_TMSI_ASSIGN

DESCRIPTION
  This function processes a TMSI Assignment Message received on the
  Forward Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_ftc_tmsi_assign
(
  caii_rx_msg_type *msg_ptr
    /* pointer to received Data Burst message */
)
{
  rxtx_cmd_type *cmd_buf;
   /* Buffer for command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* --------------------------------------------------------------
  ** Perform Layer 3 processing of the TMSI Assignment Message
  ** -------------------------------------------------------------- */

  /* Save new TMSI information */
  tmsi_info_update (&msg_ptr->tc_tmsi.data);

  /* Reset full TMSI timer */
  tmsi_reset_full_timer ();

  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_buf = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
    /* ------------------------------------------------
    ** Build the TMSI Assignment Completion Message
    ** ------------------------------------------------ */

    cmd_buf->tc_msg.msg.tc_tmsi_cmp.msg_type = CAI_RTC_TMSI_CMP_MSG;

    cmd_buf->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_buf );

    rxtx_cmd( cmd_buf );

    MSG_HIGH( "Sent RTC TMSI Assignment Completion Message", 0, 0, 0 );
  }

} /* process_ftc_tmsi_assign */

/*===========================================================================

FUNCTION SEND_TC_DATA_BURST
  This function builds an Data Burst message for Reverse Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_tc_data_burst
(
  byte burst_type,
    /* Type of Data Burst */
  byte chari_len,
    /* Byte length of chari data */
  byte *chari_data_ptr,
    /* Pointer to chari data */
  boolean ack_req
    /* Layer 2 ack required */
)
{
  
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte i;
    /* Index variable */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );

#if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
    if( burst_type == CAI_SHORT_MSG_SERVICES ||
        burst_type == CAI_POSITION_DET ||
        burst_type == CAI_SHORT_DATA_BURST
      )
    {
      mcc_send_mo_dbm_status(burst_type, CM_DBM_MO_OUT_OF_RESOURCES );
    }
#endif /* FEATURE_CDSMS || FEATURE_GPDONE_DBM */
  }
  else
  {
    /* -----------------------------
    ** Build the Data Burst Message
    ** ----------------------------- */
    cmd_ptr->tc_msg.msg.tc_burst.msg_type = CAI_TC_REV_BURST_MSG;

    cmd_ptr->tc_msg.ack_req = ack_req;

    /* Fill in the command header */
    if( ack_req )
    {
      mcctc_fill_rxtx_hdr_ack_q( cmd_ptr );
    }
    else
    {
      mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );
    }

    cmd_ptr->tc_msg.msg.tc_burst.msg_number = 1;
    cmd_ptr->tc_msg.msg.tc_burst.burst_type = burst_type;
    cmd_ptr->tc_msg.msg.tc_burst.num_msgs = 1;

    if( chari_len <= CAI_REV_TC_BURST_MAX )     /*lint !e834 */
    {
      for (i=0; i < chari_len; i++)
      {
        cmd_ptr->tc_msg.msg.tc_burst.chari[i] = *(chari_data_ptr++);
      }

      cmd_ptr->tc_msg.msg.tc_burst.num_fields = chari_len;

      rxtx_cmd( cmd_ptr );

      if (burst_type == CAI_OTA_ACTIVATION)
      {
        MSG_HIGH("Responding with OTASP msg type %d", *chari_data_ptr,0,0); /*lint !e794*/
      }
      else
      {
        MSG_HIGH( "Sent TC Data Burst Message %d", burst_type, 0, 0 );
      }

#if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
      if( burst_type == CAI_SHORT_MSG_SERVICES ||
          burst_type == CAI_POSITION_DET ||
          burst_type == CAI_SHORT_DATA_BURST
        )
      {
        if( !ack_req )
        {
          /* If msg length is OK, and L2 ack is not required, send ACK
             to CM task immediately */
          mcc_send_mo_dbm_status( burst_type, CM_DBM_MO_OK );
        }
        else
        {
          /* If L2 ack is required, we will notify SMS after L2 ACK is
             reported from RXTX */
          /* Set the dbm_wait_for_l2_ack for the specific mask only */

          cdma.dbm_wait_for_l2_ack = TRUE;
          cdma.mo_dbm_type = burst_type;
        }
      }
#endif /* FEATURE_CDSMS || FEATURE_DBM */
    }
    else
    {
      MSG_ERROR( "Data Burst too long!", 0, 0, 0 );

#if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
      if( burst_type == CAI_SHORT_MSG_SERVICES ||
          burst_type == CAI_POSITION_DET ||
          burst_type == CAI_SHORT_DATA_BURST
         )
      {
        /* If the msg is too large, the parser will refuse to pack it
         Let CM task know */
        mcc_send_mo_dbm_status( burst_type, CM_DBM_MO_DTC_TOO_LARGE );
      }
#endif /* FEATURE_CDSMS || FEATURE_DBM */
    }
  }

} /* end send_tc_data_burst */

#if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
/*===========================================================================

FUNCTION SEND_TC_MO_DBM
  This function sends out a mobile originated data burst message (Position
  Determination, SMS or SDB) by calling send_tc_data_burst function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_tc_mo_dbm
(
  mc_mo_dbm_type mo_dbm
    /* mobile originated data burst message type to be sent over the air.*/
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (mo_dbm.burst_type)
  {
#ifdef FEATURE_CDSMS
    case CAI_SHORT_MSG_SERVICES:
      send_tc_data_burst(
           (byte) CAI_SHORT_MSG_SERVICES,
           (byte) ((cm_sms_msg_type*)(mo_dbm.dbm_buf_ptr))->ota.data_len,
           (uint8*) ((cm_sms_msg_type*)(mo_dbm.dbm_buf_ptr))->ota.data,
           mo_dbm.ack_req );
      break;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_DBM
    case CAI_POSITION_DET:
      send_tc_data_burst(
           (byte) mo_dbm.burst_type,
           (byte) ((cm_dbm_cmd_info_s_type *)(mo_dbm.dbm_buf_ptr))->num_bytes,
           ((cm_dbm_cmd_info_s_type *)(mo_dbm.dbm_buf_ptr))->payload,
           mo_dbm.ack_req );
      break;
#endif /* FEATURE_DBM */

#ifdef FEATURE_DBM
    case CAI_SHORT_DATA_BURST:

      if (mccap_item_is_supported(MCCAP_DF_SDB) && cdma.sdb_supported)
      {
        send_tc_data_burst(
             (byte) mo_dbm.burst_type,
             (byte) ((cm_dbm_cmd_info_s_type *)(mo_dbm.dbm_buf_ptr))->num_bytes,
             ((cm_dbm_cmd_info_s_type *)(mo_dbm.dbm_buf_ptr))->payload,
             mo_dbm.ack_req );
      }
      else
      {
        mcc_send_mo_dbm_status(mo_dbm.burst_type, CM_DBM_MO_OUT_OF_RESOURCES);
      }
      break;
#endif /* FEATURE_DBM */

    default:
      MSG_ERROR( "MO DBM type %d not supported", mo_dbm.burst_type, 0, 0);
      break;
  }

} /* end send_tc_mo_dbm */
#endif /* FEATURE_CDSMS || FEATURE_DBM */

/*===========================================================================

FUNCTION PROCESS_TC_DATA_BURST

DESCRIPTION
  This function processes a Data Burst Message received on the
  Foward Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
word process_tc_data_burst
(
  caii_rx_msg_type *msg_ptr,
    /* pointer to received Data Burst message */
  word next_state
    /* Next state to be processed */
)
{
#ifdef FEATURE_OTASP
  word status;
    /* Translation status */
#endif /* FEATURE_OTASP */

#ifdef FEATURE_PASSPORT_MODE
  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to command to send to UI task */
#endif /* FEATURE_PASSPORT_MODE */

  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------------------
  ** Perform Layer 3 processing of the Data Burst Message
  ** ---------------------------------------------------- */

#ifdef FEATURE_CDSMS
  /* Check if valid Data Burst SMS message before processing */
  if ((msg_ptr->tc_burst.burst_type == CAI_SHORT_MSG_SERVICES) &&
     (msg_ptr->tc_burst.msg_number == 1) &&
     (msg_ptr->tc_burst.num_msgs == 1) &&
     (msg_ptr->tc_burst.num_fields > 0))
  {
    /* ------------ */
    /* SMS Message  */
    /* ------------ */
    /* Process and translate mobile-terminated SMS message */
    cm_mt_dbm_msg( (byte) CAI_SHORT_MSG_SERVICES,
                   (byte) CM_SMS_FORMAT_CDMA,
                   (byte*) msg_ptr->tc_burst.chari,
                   (word) msg_ptr->tc_burst.num_fields );

    return (next_state);  /* SHORT-RETURN */

  } /* if */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_OTASP
  /* Check if valid Data Burst OTASP message before processing */
  if ((msg_ptr->tc_burst.burst_type == CAI_OTA_ACTIVATION) &&
      (msg_ptr->tc_burst.msg_number == 1) &&
      (msg_ptr->tc_burst.num_msgs == 1) &&
      (msg_ptr->tc_burst.num_fields > 0))
  {
    /* OTAPA session can happen on an existing CDMA traffic channel
    ** (e.g., during a voice call). So we translate the message first
    ** and to see if it is an OTAPA_REQ message.
    **/
      status = otaspx_ext_to_int(msg_ptr->tc_burst.num_fields,
                                 msg_ptr->tc_burst.chari,
                                 &otasp_msg);
    if ((cdma.curr_state == CDMA_TRAFFIC) &&  /*lint !e641 */
        ((cdma.otasp_call != NO_SRV_PROV)
#ifdef FEATURE_OTASP_OTAPA
         ||(otasp_msg.gen.msg_type == OTASP_OTAPA_REQ_MSG)
#endif
        ))
    {
      MSG_MED("Received OTASP msg type %d", otasp_msg.gen.msg_type,0,0);

      switch (status)
      {
        case OTASPX_DONE_S:
          next_state = otasp_process_msg( &otasp_msg );
        break;

        case OTASPX_INV_LEN_S:
          MSG_MED("Reject:Invalid length",0,0,0);
          rej_parms.rej_msg_type = CAI_TC_FWD_BURST_MSG;
          rej_parms.rej_code =  CAI_REJ_STRUCT;
          send_rej_order( &rej_parms );
        break;

        case OTASPX_INV_MSG_S:
          MSG_MED("Reject:Invalid msg",0,0,0);
          rej_parms.rej_msg_type = CAI_TC_FWD_BURST_MSG;
          rej_parms.rej_code =  CAI_REJ_CAP;
          send_rej_order( &rej_parms );
        break;

        default:
          ERR( "Failed OTASP xlate for unknown reason",0,0,0);
          rej_parms.rej_msg_type = CAI_TC_FWD_BURST_MSG;
          rej_parms.rej_code =  CAI_REJ_UNSP;
          send_rej_order( &rej_parms );
        break;
      }
    }
    else
    {
      MSG_MED("OTASP msg rxed in wrong state",0,0,0);
      rej_parms.rej_msg_type = CAI_TC_FWD_BURST_MSG;
      rej_parms.rej_code =  CAI_REJ_STATE;
      send_rej_order( &rej_parms );
    }
    return (next_state);
  }
#endif /* FEATURE_OTASP */

#ifdef FEATURE_DBM
  /* Check if valid Data Burst message before processing */
  if (
      ((msg_ptr->tc_burst.burst_type == CAI_POSITION_DET) ||
      ((msg_ptr->tc_burst.burst_type == CAI_SHORT_DATA_BURST)
       && 
       mccap_item_is_supported(MCCAP_DF_SDB))) &&
       (msg_ptr->tc_burst.msg_number == 1) &&
       (msg_ptr->tc_burst.num_msgs == 1) &&
       (msg_ptr->tc_burst.num_fields > 0)
      )
  {
    /* ------------------------------- */
    /* Data Burst Message SDB and PD */
    /* ------------------------------- */
    /* Process and translate mobile-terminated SDB and PD
       message */

    mcc_send_mt_dbm_data(
                     (uint8) msg_ptr->tc_burst.burst_type,   /* burst_type */
                     (byte*) msg_ptr->tc_burst.chari,
                     (uint8) msg_ptr->tc_burst.num_fields );

    return (next_state);  /* SHORT-RETURN */

  } /* if */
#endif /* FEATURE_DBM */

#ifdef FEATURE_PASSPORT_MODE
  if(mcc_passport_mode_is_jcdma())
  {  
    /* Check for an Extended Burst Type - International Data Burst */
    if (msg_ptr->tc_burst.burst_type == CAI_EXTENDED_BURST_TYPE_INTL)
    {
      /* ----------------------------------- */
      /* Extended Burst Type - International */
      /* ----------------------------------- */
      if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
      {
        cmd_ptr->hdr.cmd = CM_EXT_BRST_INTL_F;
        cmd_ptr->ext_brst_intl_rec.mcc =
            b_unpackw( msg_ptr->tc_burst.chari,
            FPOS( cai_ext_brst_intl_type, mcc ),
            FSIZ( cai_ext_brst_intl_type, mcc ));

        cmd_ptr->ext_brst_intl_rec.db_subtype =
          b_unpackb( msg_ptr->tc_burst.chari,
            FPOS( cai_ext_brst_intl_type, db_subtype ),
            FSIZ( cai_ext_brst_intl_type, db_subtype ));

        cmd_ptr->ext_brst_intl_rec.chg_ind =
          b_unpackb( msg_ptr->tc_burst.chari,
            FPOS( cai_ext_brst_intl_type, chg_ind ),
            FSIZ( cai_ext_brst_intl_type, chg_ind ));

        cmd_ptr->ext_brst_intl_rec.sub_unit =
          b_unpackb( msg_ptr->tc_burst.chari,
            FPOS( cai_ext_brst_intl_type, sub_unit ),
            FSIZ( cai_ext_brst_intl_type, sub_unit ));

        cmd_ptr->ext_brst_intl_rec.unit =
          b_unpackb( msg_ptr->tc_burst.chari,
            FPOS( cai_ext_brst_intl_type, unit ),
            FSIZ( cai_ext_brst_intl_type, unit ));
        mcc_cm_mc_rpt( cmd_ptr );
      }
      else
      {
        ERR("No free buffer cm_mc_rpt_free_q",0,0,0);
       }
      return (next_state);
    }
  }
#endif /* FEATURE_PASSPORT_MODE */

  /* ----------------------------------------------------------------------
  ** We only get here if we did not recognize the Data Burst as one we know
  ** how to process.  Reject as per IS-95A (JSTD-008) section 6(2).6.4.4
  ** ---------------------------------------------------------------------- */
  rej_parms.rej_msg_type = CAI_TC_FWD_BURST_MSG;
  rej_parms.rej_code =  CAI_REJ_CAP;
  send_rej_order( &rej_parms );
  return (next_state);

} /* end process_tc_data_burst() */

#ifdef FEATURE_IS2000_REL_A_BSID
/*===========================================================================

FUNCTION PROCESS_BS_STATUS_RSP

DESCRIPTION
  This function processes a BS Status Response received on the 
  Foward Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None

===========================================================================*/
void process_tc_bs_status_rsp
(
   caii_bs_status_rsp_type *msg_ptr
)
{
    cm_mc_rpt_type *cmd_ptr;
    int max_pi_cnt,pi_cnt;

    MSG_LOW("In process_tc_bs_status_rsp!", 0, 0, 0);

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->hdr.cmd = CM_BS_STATUS_RSP_F;
      /*Note: for the time being, it is assumed that NUM_RECORDS will be 1
        The for loop below copies out the contents of the message into the
        cmd to be sent to cm and evenutally to pdsm*/
      if( msg_ptr->num_record > 1 )
        MSG_ERROR("Max Rec count should never be > 1", 0, 0, 0);

      max_pi_cnt = msg_ptr->bss_rec[0].pi.num_pilots;

      cmd_ptr->bss_status_rsp.num_pilot = max_pi_cnt; //lint !e734
      cmd_ptr->bss_status_rsp.status    = TRUE;

      for( pi_cnt = 0; pi_cnt < max_pi_cnt; pi_cnt++)
      {
         cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_id
           = msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].base_id;
         cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].nid
           = msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].nid;
         cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].pilot_pn
           = msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].pilot_pn;
         cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].sid
           = msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].sid;
         #ifdef FEATURE_IS2000_REL_C
         if (P_REV_IS_9_OR_GREATER)
         {
           cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_lat_long_incl
             =  msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].base_lat_long_incl;
           if (cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_lat_long_incl)
           {
             cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_lat
               = (sys_base_lat_type) msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].base_lat;
             cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_long
               =  (sys_base_long_type) msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].base_long;
           }
         }
         else
         #endif
         {
           /* If p_rev_in_use is less than 9, BS cannot communicate this
              value */
           cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_lat_long_incl
             = FALSE;
         }
      }
      /*send message from mc to cm*/
      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      ERR("No free buffers on cm_mc_rpt_free_q",0,0,0 );
    }

}//process_tc_bs_status_rsp

/*===========================================================================

FUNCTION SEND_BS_STATUS_RSP_FAIL_TO_CM 

DESCRIPTION
  This function is used to send back a failure message to the cm and to apps
  beyond to indicate that the BSStatusReq send was a failure.

DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None

===========================================================================*/
void send_bs_status_rsp_fail_to_cm( void )
{
    cm_mc_rpt_type *cmd_ptr;

    MSG_LOW("In send_bs_status_rsp_fail_to_cm fn.",0,0,0);

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->hdr.cmd = CM_BS_STATUS_RSP_F;

      //Setting numpilots to 0 to indicate that an error occured.
      cmd_ptr->bss_status_rsp.num_pilot = 0;
      cmd_ptr->bss_status_rsp.status = FALSE;

      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      ERR("No free buffers on cm_mc_rpt_free_q",0,0,0 );
    }
}//end send_bs_status_rsp_fail_to_cm

/*===========================================================================

FUNCTION SEND_TC_MO_BSSTATUS_REQ
  This function builds a BS Status Req message for sending on
  Reverse Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_tc_mo_bsstatus_req
(
  uint16 num_pilots,
  /*number pilots for which we need bsid, sid, nid etc*/
  word *pilot_pn_list
  /*list of pilots for which we need bsid, sid, nid etc*/
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  int plt_cnt;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW("In send_tc_mo_bsstatus_req function", 0, 0, 0);

  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
    /* -----------------------------
    ** Build the BS Status Req Message
    ** ----------------------------- */
    cmd_ptr->tc_msg.msg.bssreq.msg_type = CAI_BS_STATUS_REQ_MSG;

    //Request a layer2 Ack.
    cmd_ptr->tc_msg.ack_req = TRUE;

    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    /*It seems like a realistic assumption that the num pilots will never 
        be greater than 16, and so num_rec will never be anything other than 
        1 so hard code*/
    cmd_ptr->tc_msg.msg.bssreq.num_record = 1;

    cmd_ptr->tc_msg.msg.bssreq.qual = CAI_NO_QUAL ;
    /*since we are using qual mode cai_no_qual we dont set op_mode
    or band
    cmd_ptr->tc_msg.msg.bssreq.op_mode = ; 
    cmd_ptr->tc_msg.msg.bssreq.band = ; */

        /*The num records is being assumed to be 1*/
    cmd_ptr->tc_msg.msg.bssreq.bss_rec[0].record_type 
        = CAI_BSS_PILOT_INFO_REQ; 
    cmd_ptr->tc_msg.msg.bssreq.bss_rec[0].pi.sid_nid_req
        = TRUE;
    cmd_ptr->tc_msg.msg.bssreq.bss_rec[0].pi.num_pilots
        = num_pilots; //lint !e734

    for( plt_cnt=0 ; plt_cnt < num_pilots; plt_cnt++) 
    { 
      cmd_ptr->tc_msg.msg.bssreq.bss_rec[0].pi.pilot_pn[plt_cnt]
        = pilot_pn_list[plt_cnt];
    }

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif
    rxtx_cmd( cmd_ptr );

    MSG_HIGH( "Sent BS Status Req Message", 0, 0, 0 );

  }

} /*end send_tc_mo_bsstatus_req*/
#endif /*FEATURE_IS2000_REL_A_BSID*/

/*===========================================================================

FUNCTION PROCESS_NLU_MSG

DESCRIPTION
  This function processes an input Neighbor List Update message. The
  Neighbor set is update accordingly and a command is sent to the
  Searcher task to use the new neighbor set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_nlu_msg
(
     caii_rx_msg_type *msg_ptr
       /* pointer to received Neighbor List Update message */
)
{
  word i;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr->nlu.num_nghbr == 0)
  {
    rej_parms.rej_msg_type = CAI_TC_NLU_MSG;
    rej_parms.rej_code =  CAI_REJ_STRUCT;
    send_rej_order( &rej_parms );
    return;
  }

  /* send request to Searcher to update neighbor list */
  mcc_srch_buf.nset.hdr.cmd   = SRCH_TC_NSET_F;

  mcc_srch_buf.nset.pilot_inc = msg_ptr->nlu.pilot_inc;
  mcc_srch_buf.nset.nghbr_srch_mode = SRCH_NO_PRI_OR_WIN;
  mcc_srch_buf.nset.nset_cnt  = msg_ptr->nlu.num_nghbr;

  for(i=0; i< msg_ptr->nlu.num_nghbr && i < SRCH_NSET_MAX; i++ )
  {
    memset(&mcc_srch_buf.nset.nset[i], 0, sizeof(mcc_srch_buf.nset.nset[i]));

    mcc_srch_buf.nset.nset[i].pilot = ( int2 )msg_ptr->nlu.nghbr[i].nghbr_pn;
    mcc_srch_buf.nset.nset[i].srch_win_nghbr = mcc_ho_real.win_n;
    mcc_srch_buf.nset.nset[i].srch_offset_nghbr = 0; /* Initialize offset */
    mcc_srch_buf.nset.nset[i].pilot_rec_type = CAI_PILOT_1X_COMMON;
  }

  mcc_srch_cmd( &mcc_srch_buf );

} /* process_nlu_msg */

/*===========================================================================

FUNCTION PROCESS_ENLU_MSG

DESCRIPTION
  This function processes an input Extended Neighbor List Update message.
  The Neighbor set is update accordingly and a command is sent to the
  Searcher task to use the new neighbor set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_enlu_msg
(
     caii_rx_msg_type *msg_ptr
       /* pointer to received Extended Neighbor List Update message */
)
{
  word i;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr->ext_nlu.num_nghbr == 0)
  {
    rej_parms.rej_msg_type = CAI_EXT_NLU_MSG;
    rej_parms.rej_code =  CAI_REJ_STRUCT;
    send_rej_order( &rej_parms );
    return;
  }

  /* send request to Searcher to update neighbor list */
  mcc_srch_buf.nset.hdr.cmd   = SRCH_TC_NSET_F;

  mcc_srch_buf.nset.pilot_inc = msg_ptr->ext_nlu.pilot_inc;
  mcc_srch_buf.nset.nghbr_srch_mode = msg_ptr->ext_nlu.nghbr_srch_mode;
  mcc_srch_buf.nset.nset_cnt  =
                          MIN( SRCH_NSET_MAX , msg_ptr->ext_nlu.num_nghbr );

  for ( i=0; i< msg_ptr->ext_nlu.num_nghbr && i < SRCH_NSET_MAX; i++ )
  {
    memset(&mcc_srch_buf.nset.nset[i], 0, sizeof(mcc_srch_buf.nset.nset[i]));

    mcc_srch_buf.nset.nset[i].pilot = ( int2 )msg_ptr->ext_nlu.nghbr[i].nghbr_pn;

    /* Default the Srch Offset field per neighbor to 0 */
    mcc_srch_buf.nset.nset[i].srch_offset_nghbr = 0;

    if (P_REV_IS_6_OR_GREATER &&
        msg_ptr->ext_nlu.add_nghbr[i].add_pilot_rec_incl)
    {
      mcc_srch_buf.nset.nset[i].pilot_rec_type =
        (cai_pilot_rec_type) msg_ptr->ext_nlu.add_nghbr[i].nghbr_pilot_rec_type;
      mcc_srch_buf.nset.nset[i].pilot_rec =
        msg_ptr->ext_nlu.add_nghbr[i].add_pilot_rec;
    }
    else
    {
      mcc_srch_buf.nset.nset[i].pilot_rec_type = CAI_PILOT_1X_COMMON;
    }

    /* If no priority or search window size information is received,
       default values will be used. */

    switch (msg_ptr->ext_nlu.nghbr_srch_mode)
    {
      case SRCH_NO_PRI_OR_WIN:
        mcc_srch_buf.nset.nset[i].pri = SRCH_WIN_ONLY;
        mcc_srch_buf.nset.nset[i].srch_win_nghbr =
          msg_ptr->ext_nlu.srch_win_n;
        break;

      case SRCH_WIN_ONLY:
        mcc_srch_buf.nset.nset[i].pri = SRCH_WIN_ONLY;
        mcc_srch_buf.nset.nset[i].srch_win_nghbr =
          msg_ptr->ext_nlu.nghbr[i].srch_win_nghbr;
        if (P_REV_IS_6_OR_GREATER)
        {
          mcc_srch_buf.nset.nset[i].srch_offset_nghbr =
            msg_ptr->ext_nlu.add_nghbr[i].srch_offset_nghbr;
        }
        break;

      case SRCH_PRI_ONLY:
        mcc_srch_buf.nset.nset[i].pri =
          msg_ptr->ext_nlu.nghbr[i].search_pri;
        mcc_srch_buf.nset.nset[i].srch_win_nghbr =
          msg_ptr->ext_nlu.srch_win_n;
        break;

      case SRCH_PRI_AND_WIN:
        mcc_srch_buf.nset.nset[i].pri =
          msg_ptr->ext_nlu.nghbr[i].search_pri;
        mcc_srch_buf.nset.nset[i].srch_win_nghbr =
          msg_ptr->ext_nlu.nghbr[i].srch_win_nghbr;
        if (P_REV_IS_6_OR_GREATER)
        {
          mcc_srch_buf.nset.nset[i].srch_offset_nghbr =
            msg_ptr->ext_nlu.add_nghbr[i].srch_offset_nghbr;
        }
        break;
    } //lint !e744  No default for this switch

    if ((msg_ptr->ext_nlu.use_timing) &&
          (msg_ptr->ext_nlu.nghbr[i].timing_incl))
    {
      mcc_srch_buf.nset.nset[i].timing_incl =
        msg_ptr->ext_nlu.nghbr[i].timing_incl;
      mcc_srch_buf.nset.nset[i].nghbr_tx_offset =
        msg_ptr->ext_nlu.nghbr[i].nghbr_tx_offset;

      if (msg_ptr->ext_nlu.global_timing_incl)
      {
        mcc_srch_buf.nset.nset[i].nghbr_tx_duration =
          msg_ptr->ext_nlu.global_tx_duration;
        mcc_srch_buf.nset.nset[i].nghbr_tx_period =
          msg_ptr->ext_nlu.global_tx_period;
      }
      else
      {
        mcc_srch_buf.nset.nset[i].nghbr_tx_duration =
          msg_ptr->ext_nlu.nghbr[i].nghbr_tx_duration;
        mcc_srch_buf.nset.nset[i].nghbr_tx_period =
          msg_ptr->ext_nlu.nghbr[i].nghbr_tx_period;
      }
    }
  }

  mcc_srch_cmd( &mcc_srch_buf );

} /* process_enlu_msg */

/*===========================================================================

FUNCTION PROCESS_SP_MSG

DESCRIPTION
  This function processes an input In-Traffic System Parameters message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_sp_msg
(
  caii_rx_msg_type *msg_ptr
    /* pointer to received In-Traffic System Parameters message */
)
{
  #ifdef FEATURE_IS2000_REL_A_BSID
  byte old_p_rev_in_use = mc_get_p_rev_in_use();
  #endif /* FEATURE_IS2000_REL_A_BSID */

  #ifdef FEATURE_IS2000_REL_B
  word old_sid = mccsyobj_get_sid();
  word old_nid = mccsyobj_get_nid();
  #endif /* FEATURE_IS2000_REL_B */

  /* Store real handoff parameters */
  mcc_ho_real.t_add = msg_ptr->tc_sys.t_add;
  mcc_ho_real.t_drop = msg_ptr->tc_sys.t_drop;
  mcc_ho_real.t_comp = msg_ptr->tc_sys.t_comp;
  mcc_ho_real.t_tdrop = msg_ptr->tc_sys.t_tdrop;
  mcc_ho_real.win_a = msg_ptr->tc_sys.srch_win_a;
  mcc_ho_real.win_n = msg_ptr->tc_sys.srch_win_n;
  mcc_ho_real.win_r = msg_ptr->tc_sys.srch_win_r;
  mcc_ho_real.nghbr_max_age = msg_ptr->tc_sys.nghbr_max_age;

  /* send the new handoff parameters to the Searcher task */

  if (msg_ptr->tc_sys.p_rev != CAIX_P_REV_UNKNOWN )
  {
    // Update p_rev_in_use, bs_p_rev and report events if necessary
    mcc_update_bs_p_rev( msg_ptr->tc_sys.p_rev );
  }
  else
  {
    uint8 bs_p_rev;

    switch (mcc_get_group_for_band_class(cdma.band_class))
    {
      case MCC_CELLULAR_GROUP:
      {
        bs_p_rev = msg_ptr->tc_sys.p_rev = P_REV_IS95A;

        // Update p_rev_in_use, bs_p_rev and report events if necessary
        mcc_update_bs_p_rev( bs_p_rev );
        break;

      }
      case MCC_PCS_GROUP:
      {
        bs_p_rev = msg_ptr->tc_sys.p_rev = P_REV_JSTD008;

        // Update p_rev_in_use, bs_p_rev and report events if necessary
        mcc_update_bs_p_rev( bs_p_rev );
        break;
      }
      default:
      {
        ERR("Band Class Error, Do Not Update DB", 0,0,0);
      }
    }                      
  }

  /* ******************************************
  ** Update fields specific to P_REV >= 4 ITSPM
  ** ****************************************** */
  if (P_REV_IS_4_OR_GREATER)
  {
    /* update IS95B soft handoff parameters */
    mcc_ho_real.soft_slope = msg_ptr->tc_sys.soft_slope;

    if (msg_ptr->tc_sys.add_intercept & CAI_HO_INTERCEPT_SIGN_BIT)
    {
      /* extend the sign bit */
      mcc_ho_real.add_intercept =
        (int1)(msg_ptr->tc_sys.add_intercept | CAI_HO_INTERCEPT_SIGN_EXT);
    }
    else
    {
      mcc_ho_real.add_intercept = (int1)msg_ptr->tc_sys.add_intercept;
    }

    if (msg_ptr->tc_sys.drop_intercept & CAI_HO_INTERCEPT_SIGN_BIT)
    {
      /* extend the sign bit */
      mcc_ho_real.drop_intercept =
        (int1)(msg_ptr->tc_sys.drop_intercept | CAI_HO_INTERCEPT_SIGN_EXT);
    }
    else
    {
      mcc_ho_real.drop_intercept = (int1)msg_ptr->tc_sys.drop_intercept;
    }

    #ifdef FEATURE_IS2000_R_SCH
    if (msg_ptr->tc_sys.extension)
    {
      mccds_update_t_mulchan(msg_ptr->tc_sys.t_mulchan);
    }
    #endif /* FEATURE_IS2000_R_SCH */
  }
  else
  {
    /* disable IS95B soft handoff */
    mcc_ho_real.soft_slope     =
    mcc_ho_real.add_intercept  =
    mcc_ho_real.drop_intercept = 0;
  }

  /* ******************************************
  ** Update fields specific to P_REV >= 6 ITSPM
  ** ****************************************** */
  /* Set the slotted timer if it is included in the message */
  if (P_REV_IS_6_OR_GREATER && msg_ptr->tc_sys.t_slotted_incl)
  {
    if (mccap_item_is_supported(MCCAP_DF_SLOTTED_TIMER))
    {
      cdma.t_slotted = msg_ptr->tc_sys.t_slotted;
    }
    else
    {
      MSG_HIGH("Slotted Timer not supported, Ignore BS request", 0,0,0);
    }
  }

  /* ******************************************
  ** Update fields specific to P_REV >= 7 ITSPM
  ** ****************************************** */
  #ifdef FEATURE_IS2000_REL_A
  cdma.cs_supported = (P_REV_IS_7_OR_GREATER) ? msg_ptr->tc_sys.cs_supported : FALSE;
  #endif /*FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #endif /* FEATURE_IS2000_REL_A_AES */

  #ifdef FEATURE_IS2000_REL_A_BSID
  /* BS can specify PILOT_INFO_REQ_SUPPORTED in UHDM. So make sure
     the assumption of P_REV 7 BS always supports pilot req info
     holds only of P_REV_IN_USE change through ITSPM. */ 
  if (old_p_rev_in_use != mc_get_p_rev_in_use())
  {
    cdma.pilot_info_req_supported = (P_REV_IS_7_OR_GREATER) ? TRUE : FALSE; 
  }
  #endif /* FEATURE_IS2000_REL_A_BSID */

  /* ******************************************
  ** Update fields specific to P_REV >= 9 ITSPM
  ** ****************************************** */
  #ifdef FEATURE_IS2000_REL_B
  #ifdef FEATURE_IS2000_REL_C
  if (P_REV_IS_9_OR_GREATER)
  {
    /* ------------------------------------------------------------------
    ** P-rev 9 ITSPMs contain the Off Time Reporting fields, so update
    ** our info accordingly.
    ** ------------------------------------------------------------------ */
    mcctcsup_update_off_time_info(msg_ptr->tc_sys.cdma_off_time_rep_sup_ind,
                                  msg_ptr->tc_sys.cdma_off_time_rep_threshold_unit,
                                  msg_ptr->tc_sys.cdma_off_time_rep_threshold);
  }
  else
  #endif /* FEATURE_IS2000_REL_C */
  if (P_REV_IS_8_OR_GREATER)
  {
    /* -------------------------------------------------------------------
    ** P-rev 8 ITSPMs do not contain the Off Time Reporting fields.
    ** If the sid/nid has changed, the mobile will no longer send the CDMA
    ** Off Time Report Message (we are probably on a different network).
    ** -------------------------------------------------------------------- */
    if ((msg_ptr->tc_sys.sid != old_sid) || (msg_ptr->tc_sys.nid != old_nid))
    {
      cdma.off_time_info.cdma_off_time_supported = FALSE;
    }
  }
  #endif /* FEATURE_IS2000_REL_B */

  #ifdef FEATURE_IS2000_REL_C
  cdma.chm_supported = (P_REV_IS_9_OR_GREATER) ? msg_ptr->tc_sys.chm_supported : 1;

  if (P_REV_IS_9_OR_GREATER)
  {
    cdma.sdb_supported = msg_ptr->tc_sys.sdb_supported;

    if ( msg_ptr->tc_sys.cs_supported )
    {
      cdma.mob_qos = msg_ptr->tc_sys.mob_qos;
    }
  }
  #endif /* FEATURE_IS2000_REL_C */

  /*
  ** Note, that this will always append the latest system reselection
  ** criteria recieved in the extended system parameter message (if
  ** available to search, even in the traffic channel. This should
  ** not be a problem.
  */
  /* Send search parameters to searcher */
  mcc_send_srch_parms( );

  /* ************************************************************************
  ** Stores new SID, NID, PZID and notify its reception if a handler function
  ** is registered. 
  ** ************************************************************************ */
  mccsyobj_process_epzid(msg_ptr->tc_sys.sid, msg_ptr->tc_sys.nid, 
    (P_REV_IS_4_OR_GREATER ? msg_ptr->tc_sys.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN),
    CDMA_TC);

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  /* ------------------------------------------------------------------
  ** There are some serving system related info coming in in this msg.
  ** Give the info to CM.
  ** Notice: This block has dependency on p_rev fields, do not move it
  ** before p_rev processing.
  ** ------------------------------------------------------------------ */
  mcc_send_cm_srv_sys_info( CM_SS_SID_MASK|
                            CM_SS_NID_MASK|
                            CM_SS_PACKET_ZONE_MASK|
                            CM_SS_BS_P_REV_MASK|
                            CM_SS_P_REV_IN_USE_MASK,  /*lint !e747 */
                            msg_ptr->tc_sys.sid,
                            msg_ptr->tc_sys.nid,
                            0,
                            (P_REV_IS_4_OR_GREATER ? msg_ptr->tc_sys.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN)
                          );

} /* process_sp_msg */

/*===========================================================================

FUNCTION CALC_MAR_PARM

DESCRIPTION
  This function calculates a manufacturer specific parameter from retrieved
  Markov statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Calculated statistic

SIDE EFFECTS
  None.

===========================================================================*/
word calc_mar_parm
(
  word stat,
    /* statistic to calculate */
  mar_stat_type *mar_ptr
    /* pointer to markvo statistics to do calculation on */
)
{
  word ret_val = 0;
    /* value to return to caller */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( stat )
  {
    case TC_PARM_GOOD_FULL:
    {
      /* expected full,  got full */
      ret_val = mar_ptr->mar_test[0][0];
      break;
    }

    case TC_PARM_GOOD_OVERALL:
    {
      ret_val = mar_ptr->mar_test[0][0] + mar_ptr->mar_test[1][5] +
                mar_ptr->mar_test[2][6] + mar_ptr->mar_test[3][7];
      break;
    }

    case TC_PARM_BAD_FULL:
    {
      ret_val = mar_ptr->mar_test[0][5] + mar_ptr->mar_test[0][6] +
                mar_ptr->mar_test[0][7] + mar_ptr->mar_test[0][8] +
                mar_ptr->mar_test[0][9];
      break;
    }

    case TC_PARM_BAD_OVERALL:
    {
      ret_val = mar_ptr->mar_test[0][5];
      ret_val += mar_ptr->mar_test[0][6];
      ret_val += mar_ptr->mar_test[0][7];
      ret_val += mar_ptr->mar_test[0][8];
      ret_val += mar_ptr->mar_test[0][9];
      ret_val += mar_ptr->mar_test[1][0];
      ret_val += mar_ptr->mar_test[1][6];
      ret_val += mar_ptr->mar_test[1][7];
      ret_val += mar_ptr->mar_test[1][8];
      ret_val += mar_ptr->mar_test[1][9];
      ret_val += mar_ptr->mar_test[2][0];
      ret_val += mar_ptr->mar_test[2][5];
      ret_val += mar_ptr->mar_test[2][7];
      ret_val += mar_ptr->mar_test[2][8];
      ret_val += mar_ptr->mar_test[2][9];
      ret_val += mar_ptr->mar_test[3][0];
      ret_val += mar_ptr->mar_test[3][5];
      ret_val += mar_ptr->mar_test[3][6];
      ret_val += mar_ptr->mar_test[3][8];
      ret_val += mar_ptr->mar_test[3][9];
      break;
    }

    default:
    {
      ERR( "Bad statistic request: stat %d", stat, 0, 0 );
      break;
    }

  }/* switch */

  return( ret_val );

} /* calc_mar_parm */

/*===========================================================================

FUNCTION SEND_PARM_RESP

DESCRIPTION
  This function sends a Parameter Response message in response to a Retreive
  Parameters message.  If this procedure determines that it cannot supply a
  value for a parameter it will instead send a Mobile Station Reject Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_parm_resp
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to Retreive Parameters message */
)
{
  word i;
  boolean got_markov;
  static mar_stat_type mar_stats;
  static parm_get_type get;
  word rsp_parms = 0;
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  word total_length = 0;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  got_markov = FALSE;  /* Initialize to not having retrieved Markov stats */
  get.num_parms = 0;   /* Initialize number of parameters to zero */

  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
    return;
  }

  MSG_MED( "num_parms to retrieve %d", msg_ptr->ret.num_parms, 0, 0 );

  if (msg_ptr->ret.num_parms == 0)
  {
    rej_parms.rej_msg_type = CAI_RET_PARMS_MSG;
    rej_parms.rej_code =  CAI_REJ_STRUCT;
    rej_parms.rej_param_id =  0;
    send_rej_order( &rej_parms );
    q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
    return;
  }

  /* Retrieve parameters */
  for (i=0; i < msg_ptr->ret.num_parms; i++)
  {
    if ((msg_ptr->ret.parameter_id[i] >= TC_PARM_GOOD_FULL) &&
        (msg_ptr->ret.parameter_id[i] <= TC_PARM_BAD_OVERALL))
    {
      /* Retrieving forward Power control statistics */
      if (!got_markov)
      {
        /* Retrieve Markov statistics */
        mar_get_stat( &mar_stats );
        got_markov = TRUE;
      }
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter_id =
        msg_ptr->ret.parameter_id[i];
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter.parm_w =
        calc_mar_parm( msg_ptr->ret.parameter_id[i], &mar_stats );
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms++].parameter_len = 15;
      total_length += 16;
    }
    else if (msg_ptr->ret.parameter_id[i] == CAI_OTH_SYS_TIME_ID)
    {
      /* parm_get can't return a qword, so any qword parameters must
        be retrieved separately */

      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter_id =
        CAI_OTH_SYS_TIME_ID;
      /* PARAMETER_LEN is Length -1 */
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter_len =
        CAI_OTH_SYS_TIME_LEN -1;

      parm_ret_sync_time(
        cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter.parm_q );

      MSG_MED( "retrieved OTH_SYS_TIME %ld:%ld ",
      qw_hi( cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter.parm_q ),
      qw_lo( cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter.parm_q ),
      0 );

      /* Increment number of parameters in the response */
      rsp_parms++;

      total_length += CAI_OTH_SYS_TIME_LEN;
    }
    else
    {
      get.parm[get.num_parms++].id = msg_ptr->ret.parameter_id[i];
    }
  } /* end of for */


  if (get.num_parms > 0)
  {
    parm_get( &get );
  }

  /* We need to make sure a stupid base station didn't request more
     parameters than we have storage for. */

  for (i=0; (i < get.num_parms) && (rsp_parms < CAI_PARM_RSP_MAX); i++) /*lint !e834 */
  {
    cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter_id =
      get.parm[i].id;
    if ( (get.parm[i].len != 0) &&
        ((get.parm[i].id >= CAI_FIRST_PARM_ID) &&
             (get.parm[i].id <= CAI_LAST_PARM_ID)))
    {
      /* PARAMETER_LEN is Length - 1 */
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter_len =
        get.parm[i].len - 1;
      total_length += get.parm[i].len;
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms++].parameter.parm_d =
        get.parm[i].value;
    }
    else
    {
      /* Set length of unretrievable parameter to '11 1111 1111'
         (See IS-95A section 6.7.2.3.2.11) */
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms++].parameter_len = 0x3ff;
    }
  }

  /* Each parameter has a 16 bit PARAMETER_ID and a 10 bit PARAMETER_LEN.
     The Parameters Response Message also has 17 bits of layer 2 stuff.
     0-7 reserved bits are also added, but they won't affect the comparison */
  total_length += (word)(26 * rsp_parms + 17);

  if ((rsp_parms >= CAI_PARM_RSP_MAX) ||        /*lint !e834 */
      (total_length > CAI_REV_TC_MSG_BODY_SIZE)) /*lint !e834 */
  {
      MSG_MED( "Requested %d parms, of %d length.  Too many!",
                rsp_parms, total_length, 0 );
      rej_parms.rej_msg_type = CAI_RET_PARMS_MSG;
      rej_parms.rej_code =  CAI_REJ_UNSP;
      rej_parms.rej_param_id =  0;
      send_rej_order( &rej_parms );
  }
  else
  {
    /* Fill in rest of message */
    cmd_ptr->tc_msg.msg.parm_rsp.msg_type = CAI_PARM_RSP_MSG;
    cmd_ptr->tc_msg.msg.parm_rsp.num_parms = rsp_parms;

    cmd_ptr->tc_msg.ack_req = FALSE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }

} /* end send_parm_resp() */

/*===========================================================================

FUNCTION TC_SET_PARM

DESCRIPTION
  This function responds to a Set Parameters message.  If the Set Parameters
  message contains a parameter which cannot be set NONE of the parameters
  will be set.

DEPENDENCIES
  None.

RETURN VALUE
  MC_DONE_S if parameters were set.
  MC_BADPARM_S if a parameter was invalid.

SIDE EFFECTS
  None.

===========================================================================*/
void tc_set_parm
(
   caii_rx_msg_type *msg_ptr
     /* Pointer to Set Parameters Message */
)
{
  static parm_set_type set;
    /* Received parameter set */
  word parm_stat;
    /* Status returned by parm_set procedure */
  word parm_id_ptr;
    /* Pointer to where to place bad parameter id if PARM_DONE_S
       is not returned */
  word i;
    /* Index variable */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set up parameter set and verify that it can be set */
  /* For now, check first for Markov reset parameter */
  if ((msg_ptr->set.num_parms == 1) &&
      (msg_ptr->set.parms[0].parameter_id == TC_MARKOV_RESET))
  {
    /* Reset Markov statistics */
    rxc_markov_reset();
  }
  else
  {
    /* Check for normal parameter set */
    set.num_parms = msg_ptr->set.num_parms;
    for (i=0; i < msg_ptr->set.num_parms; i++)
    {
      set.parm[i].id = msg_ptr->set.parms[i].parameter_id;
      if (msg_ptr->set.parms[i].parameter_len < 16)
      {
        set.parm[i].value = (dword) msg_ptr->set.parms[i].parameter.parm_w;
      }
      else if (msg_ptr->set.parms[i].parameter_len < 32)
      {
        set.parm[i].value = msg_ptr->set.parms[i].parameter.parm_d;
      }
      else
      {
        /* Since there are no qwords that are settable mark this as invalid */
        rej_parms.rej_param_id = msg_ptr->set.parms[i].parameter_id;
        rej_parms.rej_msg_type = CAI_SET_PARMS_MSG;
        rej_parms.rej_code =  CAI_REJ_FIELD;
        send_rej_order( &rej_parms );
        return;
      }
    } /* for */

    if ((parm_stat = parm_set( &set, &parm_id_ptr )) != PARM_DONE_S)
    {
       MSG_MED( "Invalid parm set %d", parm_stat, 0, 0 );
       /* -----------------------------------------------------------
       ** Bad parameter has been placed into parm_id_ptr by parm_set,
       ** so parm_id_ptr is not NULL.
       ** ----------------------------------------------------------- */
       rej_parms.rej_param_id = parm_id_ptr; 
       rej_parms.rej_msg_type = CAI_SET_PARMS_MSG;
       rej_parms.rej_code =  CAI_REJ_FIELD;
       send_rej_order( &rej_parms );
    }
  }

} /* end tc_set_parm() */ 

/*===========================================================================

FUNCTION TC_SO_CHANGE

DESCRIPTION
  This function changes service options.  It does not do any checking to
  see if the new service option is different from the current service option.
  All error checking regarding the new service option should be done prior to
  calling this procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_so_change
(
  caii_srv_cfg_type  *cfg_ptr
  // config to get SO's from
)
{
  
  byte con_index;

  for (con_index = 0; con_index < cfg_ptr->num_con; con_index++)
  {
    if ( (cfg_ptr->con[con_index].fwd_traf == CAI_PRI_TRAF) ||  /*lint !e641 */
         (cfg_ptr->con[con_index].rev_traf == CAI_PRI_TRAF)  /*lint !e641 */
       )
    {
      cdma.so_cur_pri = cfg_ptr->con[con_index].so;
    }
    else if ( (cfg_ptr->con[con_index].fwd_traf == CAI_SEC_TRAF) ||  /*lint !e641 */
              (cfg_ptr->con[con_index].rev_traf == CAI_SEC_TRAF)  /*lint !e641 */
            )
    {
      cdma.so_cur_sec = cfg_ptr->con[con_index].so;
    }
  }

} /* tc_so_change */

/*===========================================================================

FUNCTION tc_send_rel_to_rxc_txc

DESCRIPTION
This function sends release command to RXC/TXC making them release their
Service Option (SO)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void tc_send_rel_to_rxc_txc()
{
  /* Send release command to RXC and TXC */
  mcc_rx_buf.rel.hdr.command = RXC_RELEASE_F;
  mcc_tx_buf.rel.hdr.command = TXC_RELEASE_F;
  mcc_rxc_cmd( &mcc_rx_buf );
  mcc_txc_cmd( &mcc_tx_buf );
}

/*===========================================================================

FUNCTION GET_ABS_ACTION_TIME

DESCRIPTION
  This function converts an action time in units of mod 64 super frame
  into an absolute time in frames for lower layers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The input argument abs_action_time is passed by address and contains the return value

===========================================================================*/

void get_abs_action_time
(
  qword abs_action_time,
  byte action_time
)
{
  qword dummy,adder;
  dword diff_time;
  byte  curr_mod_64_time;
  ts_get_20ms_frame_offset_time( abs_action_time );

  // If implicit action time, we are done here.  Otherwise compute time to
  // wait.
  if ( action_time != MCCTCHO_IMPLICIT_ACTION_TIME )
  {
    (void) qw_div( abs_action_time, abs_action_time, 4U);
    curr_mod_64_time = (byte) qw_div_by_power_of_2( dummy, abs_action_time, 6); // Div by 64
    if (action_time > curr_mod_64_time)
    {
       diff_time = action_time - curr_mod_64_time;

    }
    else
    {
       diff_time = action_time + 64 - curr_mod_64_time;
    }
    adder[0] = diff_time;
    adder[1] = 0;
    qw_add( abs_action_time,abs_action_time,adder);
    qw_shift( abs_action_time, 2); // Multiply by 4
  }
}

/*===========================================================================

FUNCTION TC_INFORM_SCR_CHANGE

DESCRIPTION
  This function changes service options.  This function queues up
  new SCR with the lower layers with the corresponding action time.
  The SCR itself does not become current till the action time.

DEPENDENCIES
  if use_pending_cfg is TRUE, then the function will disregard new_so
  and iterate through the connections in the pending srv cfg.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tc_inform_scr_change
(
  word new_so,
    /* Service option to change to */
  byte action_time,
    /* Time when the SCR takes effect 0xff for implicit time*/
  boolean use_pending_cfg
    /* Whether we should disregard new_so and use the info from pending SCR */
)
{
  qword abs_action_time;
  const caii_complete_srv_cfg_type * pending_cfg;
  byte index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // If service option is established with Service Option Negotiation, need to
  // generate a service configuration based on the service option.
  if ( !cdma.serv_neg )
  {
    (void)snm_gen_son_config( new_so );
  }

  if (!use_pending_cfg)
  {
    /* Need to init these services now, cannot wait until RXC/TXC are ready */
    tc_inform_scr_change_init_service(new_so);
  }
  else
  {
    for (pending_cfg = srv_get_pending_cfg(), index = 0;
         index < pending_cfg->neg_scr.num_con;
         index++
        )
    {
      /* Need to init these services now, cannot wait until RXC/TXC are ready */
      tc_inform_scr_change_init_service(pending_cfg->neg_scr.con[index].so);
    }

  }

  get_abs_action_time( abs_action_time, action_time);
  mcctxns_begin_scr_trans( abs_action_time );
} /* tc_inform_scr_change */

/*===========================================================================

FUNCTION TC_INFORM_SCR_CHANGE_INIT_SERVICE

DESCRIPTION
  This function initializes service options, if needed

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_inform_scr_change_init_service
(
  word new_so
)
{
  if (new_so == CAI_SO_LOOPBACK)
  {
    loopback_init( CAI_MUX1 );
  }
  else if (new_so == CAI_SO_LOOPBACK_13K)
  {
    loopback_init( CAI_MUX2 );
  }
  else if (new_so == CAI_SO_LOOPBACK_SO55)
  {
    const caii_complete_srv_cfg_type *temp_cfg = snm_get_complete_pending_config();
    if (RATE_SET_OF_FWD_RC(temp_cfg->neg_scr.for_fch_rc) == CAI_RATE_9600) //lint !e666 !e641 func argument
    {
      loopback_init( CAI_MUX1 );
    }
    else
    {
      loopback_init( CAI_MUX2 );
    }
  }
  else if (new_so == CAI_SO_RS2_MARKOV || new_so == CAI_SO_MARKOV_13K)
  {
    mar_init( mar_rate, TRUE, FALSE, cdma.esn, TRUE);

  }
  else if (new_so == CAI_SO_MARKOV_SO54)
  {
    const caii_complete_srv_cfg_type *temp_cfg = snm_get_complete_pending_config();
    mar_init( mar_rate, TRUE, FALSE, cdma.esn,
      RATE_SET_OF_FWD_RC(temp_cfg->neg_scr.for_fch_rc) ); //lint !e666 !e641 func argument
  }
  else if (new_so == CAI_SO_RS1_MARKOV || new_so == CAI_SO_MARKOV)
  {
    mar_init( mar_rate, TRUE, FALSE, cdma.esn, FALSE);
  }
  #ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
  #endif

}

/*===========================================================================

FUNCTION BUILD_OUTER_LOOP_REPORT_MSG

DESCRIPTION
  This function builds an outer Loop Report message from an RX task outer
  loop report.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void build_outer_loop_report_msg
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to Outer Loop Report Message */
  mccrx_rpt_type *rpt_ptr
    /* Pointer to RX task Outer Loop report */
)
{
  
  int i;

  MSG_MED("Rxed RXC_OL_R rpt", 0, 0, 0);
  msg_ptr->outer_rpt.msg_type = CAI_OUTER_RPT_MSG;

  msg_ptr->outer_rpt.fch_incl = rpt_ptr->ol.fch_incl;
  // Just copy curr_setpt anyway, it's harmless and saves code
  msg_ptr->outer_rpt.fpc_fch_curr_setpt = rpt_ptr->ol.fpc_fch_curr_setpt;

  msg_ptr->outer_rpt.dcch_incl = rpt_ptr->ol.dcch_incl;
  // Just copy curr_setpt anyway, it's harmless and saves code
  msg_ptr->outer_rpt.fpc_dcch_curr_setpt = rpt_ptr->ol.fpc_dcch_curr_setpt;

  msg_ptr->outer_rpt.num_sup = rpt_ptr->ol.num_sup;
  for (i=0; i<msg_ptr->outer_rpt.num_sup && i<CAI_OUTER_RPT_MAX_SUP; i++)
  {
    msg_ptr->outer_rpt.sup[i].sch_id = rpt_ptr->ol.sch[i].sch_id;
    msg_ptr->outer_rpt.sup[i].fpc_sch_curr_setpt =
      rpt_ptr->ol.sch[i].fpc_sch_curr_setpt;
  } /* end for */

} /* build_outer_loop_report_msg */

/*===========================================================================

FUNCTION TC_SO_REQ

DESCRIPTION
  This function processes a Service Option Request Order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_so_req
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to received SOR Request message */
)
{
  
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte action_time; /*action time of this order */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
    return;
  }
  if (msg_ptr->tc_ord.gen.use_time)
  {
     action_time = msg_ptr->tc_ord.gen.action_time;
  }
  else
  {
     action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
  }

#ifdef FEATURE_EVRC
  if ((cdma.so_req == CAI_SO_VOICE_EVRC) &&
           ((msg_ptr->tc_ord.so_req.service_option == CAI_SO_VOICE_IS96A)
		#ifndef FEATURE_MVS_MIGRATE
            && (voc_capability_supported(VOC_CAP_IS96A))
		#endif
		))
  {
     /* ----------------------------------------------------
    ** Acceptable to negotiate EVRC down to IS96A
    ** ------------------------------------------------- */
    cmd_ptr->tc_msg.msg.tc_ord.so.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.so.order    = CAI_SO_RESP_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.so.ordq     = 0x00;
    cmd_ptr->tc_msg.msg.tc_ord.so.service_option =
      msg_ptr->tc_ord.so_req.service_option;

    /* -------------------
    ** Set SO_REQs to NULL
    ** ------------------- */
    cdma.so_req = CAI_SO_NULL;

    /* ------------------------------------------------------------
    ** Send a Service Option Response Order accepting the requested
    ** service option
    ** ------------------------------------------------------------ */
    tc_inform_scr_change( msg_ptr->tc_ord.so_req.service_option,action_time, FALSE );
  }
  else
#endif /* FEATURE_EVRC */

  if (((cdma.so_cur_pri == CAI_SO_MARKOV) &&
            (msg_ptr->tc_ord.so_req.service_option == CAI_SO_MARKOV)) ||
           ((cdma.so_cur_pri == CAI_SO_MARKOV_SO54) &&
            (msg_ptr->tc_ord.so_req.service_option == CAI_SO_MARKOV_SO54)) ||
           ((cdma.so_cur_pri == CAI_SO_MARKOV_13K) &&
            (msg_ptr->tc_ord.so_req.service_option == CAI_SO_MARKOV_13K)) )
  {

    const caii_srv_cfg_type *temp_cfg = snm_get_current_config();
    /* ---------------------------------------------------------
    ** Must be a change to Markov rate --> send a Service Option
    ** Response Order accepting the requested service option
    ** ---------------------------------------------------------- */
    if (msg_ptr->tc_ord.gen.use_time)
    {
       // action time for SCR change is handled by the lower layers,
       // This is a case where SCR is not changing, so let's just resort to
       // the old way of doing things
       tc_action_chk( msg_ptr);
       return;
    }

    cmd_ptr->tc_msg.msg.tc_ord.so.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.so.order    = CAI_SO_RESP_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.so.ordq     = 0x00;

    /* --------------------------------------
    ** Echo back the requested service option
    ** -------------------------------------- */
    cmd_ptr->tc_msg.msg.tc_ord.so.service_option =
      msg_ptr->tc_ord.so_req.service_option;

    /* ----------------------
    ** Adjust the Markov rate
    ** ---------------------- */
    mar_rate = msg_ptr->tc_ord.so_req.ordq;
    mar_init( mar_rate, TRUE, FALSE, cdma.esn,
              (boolean) (cdma.so_cur_pri == CAI_SO_MARKOV_13K)
              || ( (cdma.so_cur_pri == CAI_SO_MARKOV_SO54)&& RATE_SET_OF_FWD_RC(temp_cfg->for_fch_rc)==CAI_RATE_9600) //lint !e666 !e641 func argument
            ); //lint !e730 boolean arg okay
  }

  else
  {
    /* -------------------
    ** Set SO_REQs to NULL
    ** ------------------- */
    cdma.so_req = CAI_SO_NULL;

    /* ----------------------------------------------------------
    ** Send a Service Option Response Order to reject the request
    ** ---------------------------------------------------------- */
    cmd_ptr->tc_msg.msg.tc_ord.so.msg_type       = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.so.order          = CAI_SO_RESP_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.so.ordq           = 0x00;
    cmd_ptr->tc_msg.msg.tc_ord.so.service_option = CAI_SO_REJ;
  }

  cmd_ptr->tc_msg.ack_req = TRUE;

  /* Fill in the command header */
  mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

  rxtx_cmd( cmd_ptr);

} /* end tc_so_req() */

/*===========================================================================

FUNCTION TC_SO_RESP

DESCRIPTION
  This function processes a Service Option Response Order.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.  If it is not necessary to change the
  current state as a result of the received Service Option Response
  Order, the return state will be equal to the current state.

SIDE EFFECTS
  None

===========================================================================*/

word tc_so_resp
(
  caii_rx_msg_type *msg_ptr,
    /* Pointer to received SOR Response message */
  word curr_state
    /* Traffic Channel substate currently being processed */
)
{
  word next_state = curr_state;
    /* Next Traffic Channel state to be processed */
  byte action_time;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  mccccim_event_type mccccim_evt;
  /* Event data struct to be sent to CCIM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr->tc_ord.gen.use_time)
  {
     action_time = msg_ptr->tc_ord.gen.action_time;
  }
  else
  {
     action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
  }


  if (cdma.so_req == CAI_SO_NULL)
  {
    /* -------------------------------------------------------
    ** No request is outstanding --> need to send reject order
    ** ------------------------------------------------------- */
    rej_parms.rej_order = msg_ptr->tc_ord.so_res.order;
    rej_parms.rej_ordq = msg_ptr->tc_ord.so_res.ordq;
    rej_parms.rej_msg_type = msg_ptr->gen.msg_type;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
  }

  else if (msg_ptr->tc_ord.so_res.service_option == CAI_SO_REJ)
  {
    /* ---------------------------------------------------------------
    ** Order indicates service option rejection --> tear down the call
    ** --------------------------------------------------------------- */
    next_state = CDMA_RELEASE;  /*lint !e641 */
    cdma.rel_orig = TRUE;       /* Release is mobile originated */

    /* Send Notification to MCCCCIM to release all CC instances */
    mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
    /* All CC instances should already be in release substate */
    mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_SO_REJ;
    mccccim_process_event( &mccccim_evt );
    /* No need check return status in this case */

    /* -------------------
    ** Set SO_REQs to NULL
    ** ------------------- */
    cdma.so_req = CAI_SO_NULL;
  }

  else
  {
    if (cdma.so_req == msg_ptr->tc_ord.so_res.service_option)
    {
      /* Requested service option has been accepted */
      mar_rate = msg_ptr->tc_ord.so_res.ordq;
      tc_inform_scr_change( msg_ptr->tc_ord.so_res.service_option,action_time, FALSE );
      event_report(EVENT_SO_NEGOTIATION_COMPLETED);
    }
    else
    {
      /* Response did match request --> send reject order... */
      rej_parms.rej_order = msg_ptr->tc_ord.so_res.order;
      rej_parms.rej_ordq = msg_ptr->tc_ord.so_res.ordq;
      rej_parms.rej_msg_type = msg_ptr->gen.msg_type;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );

      /* Though MS has decided to end this call now, we still ned to
      inform CM, hence use the BS release code for this */

      /* Send Notification to MCCCCIM to release all CC instances */
      mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
      /* All CC instances should already be in release substate */
      mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_BS_END;
      mccccim_process_event( &mccccim_evt );
      /* No need check return status in this case */

      /* ...and tear down the call */
      next_state = CDMA_RELEASE;  /*lint !e641 */
      cdma.rel_orig = TRUE;       /* Release is mobile originated */

    }

    /* Mark no long have outstanding SOR request */
    cdma.so_req = CAI_SO_NULL;
  }

  return (next_state);

} /* end tc_so_resp */

/*===========================================================================

FUNCTION SEND_STATUS_MSG

DESCRIPTION
  This function sends a Status Message in response to a received
  Status Request Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_status_msg
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to Status Request Order */
)
{
  rxtx_cmd_type *cmd_ptr;
    /* Pointer to command to send to layer 2 task */
  int rec_siz;
    /* For receiving status from called function */
  byte reject_reason;
    /* Reason for rejecting status request message */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
    return;
  }

  /* ------------------------
  ** Build information record
  ** ------------------------ */

  rec_siz =
    mcc_info_rec ( FALSE, /* traffic channel */
                   cdma.band_class,
                   CAI_IS95_CDMA_OP_MODE,
                   msg_ptr->tc_ord.stat_req.ordq,
                   (void *) cmd_ptr->tc_msg.msg.status.recs,
                   sizeof(cmd_ptr->tc_msg.msg.status.recs),
                   &reject_reason );

  if ( rec_siz < 0 )
  {
    q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
      /* Return tx message buf to free queue. */

    /* ---------------------------------------
    ** Reject --> Invalid Status Request Order
    ** --------------------------------------- */
    rej_parms.rej_order = CAI_STATUS_ORD;
    rej_parms.rej_ordq =  msg_ptr->tc_ord.stat_req.ordq;
    rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
    rej_parms.rej_code =  reject_reason;
    send_rej_order( &rej_parms );
  }
  else if ( rec_siz == 0 )
  {
    q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
      /* Return tx message buf to free queue. */

    ERR ( "Unable to build single info rec", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->tc_msg.msg.status.msg_type = CAI_STATUS_MSG;

    cmd_ptr->tc_msg.ack_req = FALSE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }

} /* end send_status_msg() */

/*===========================================================================

FUNCTION SEND_STATUS_RSP_MSG

DESCRIPTION
  This function sends a Status Response Message in response to a received
  Status Request Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_status_rsp_msg
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to Status Request Message */
)
{
  rxtx_cmd_type *cmd_ptr;
    /* Pointer to command to send to layer 2 task */
  int rec_siz;
    /* For receiving status from called function */
  word all_rec_siz;
    /* For computing cumulative length of all records */
  word i;
    /* For indexing through rec information types */
  boolean ok;
    /* For detecting problems in loop */
  byte reject_reason = CAI_REJ_UNSP;
    /* Reason for rejecting status request message */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
    return;
  }

  cmd_ptr->tc_msg.msg.tc_strsp.msg_type = CAI_TC_STRSP_MSG;

  cmd_ptr->tc_msg.msg.tc_strsp.qual     = msg_ptr->tc_streq.qual;
  cmd_ptr->tc_msg.msg.tc_strsp.band     = msg_ptr->tc_streq.band;
  cmd_ptr->tc_msg.msg.tc_strsp.op_mode  = msg_ptr->tc_streq.op_mode;
  cmd_ptr->tc_msg.msg.tc_strsp.num_recs = 0;

  switch (mcc_get_group_for_band_class(cdma.band_class))
  {
    case MCC_CELLULAR_GROUP:
    {
      /* -----------------------------------------------------
      ** If QUAL_INFO_TYPE doesn't include a field, it implies
      ** the current band class or operating mode.
      ** ----------------------------------------------------- */
      if (msg_ptr->tc_streq.qual == CAI_NO_QUAL)
      {
        /* We need to set band and op_mode to CELLULAR */
        msg_ptr->tc_streq.band = cdma.band_class;      
        msg_ptr->tc_streq.op_mode = CAI_IS95_CDMA_OP_MODE;
      }
      else if (msg_ptr->tc_streq.qual == CAI_BAND_QUAL)
      {
        /* We need to set the op_mode to IS95 CDMA */
        msg_ptr->tc_streq.op_mode = CAI_IS95_CDMA_OP_MODE;
      }
      break;

    }
    case MCC_PCS_GROUP:
    {
      /* -----------------------------------------------------
      ** If QUAL_INFO_TYPE doesn't include a field, it implies
      ** the current band class or operating mode.
      ** ----------------------------------------------------- */
      if (msg_ptr->tc_streq.qual == CAI_NO_QUAL)
      {
        /* We need to set band and op_mode to JSTD008 */
        msg_ptr->tc_streq.band = cdma.band_class;      
        msg_ptr->tc_streq.op_mode = CAI_J_STD_008_OP_MODE;
      }
      else if (msg_ptr->tc_streq.qual == CAI_BAND_QUAL)
      {
        /* We need to set the op_mode to JSTD008 */
        msg_ptr->tc_streq.op_mode = CAI_J_STD_008_OP_MODE;
      }
      break;
    }
    default:
    {
      MSG_ERROR( "Invalid Band Class",0,0,0);
    }
  }                      

  /* -------------------------
  ** Build information records
  ** ------------------------- */

  all_rec_siz = 0;
  ok = TRUE;

  if (msg_ptr->tc_streq.num_fields == 0)
  {
    /* We can't send a response with no info, so we must reject */
    ok = FALSE;
  }

  for ( i = 0; i < msg_ptr->tc_streq.num_fields; i++ )
  {
    rec_siz =
      mcc_info_rec
        ( FALSE, /* traffic channel */
          msg_ptr->tc_streq.band,
          msg_ptr->tc_streq.op_mode,
          msg_ptr->tc_streq.rec_typ[ i ],
          (void *) &cmd_ptr->tc_msg.msg.tc_strsp.recs[ all_rec_siz/sizeof(dword) ],
          sizeof(cmd_ptr->tc_msg.msg.tc_strsp.recs) - all_rec_siz,
          &reject_reason );

    if ( rec_siz < 0 )
    {
      ok = FALSE;
      break;
    }

    else if ( rec_siz == 0 )
    {
      break;
    }

    else
    {
      all_rec_siz += (word) rec_siz;
      //We do not want to cast away a non-dword aligned pointer into our struct, so adjust
      //accordingly
      all_rec_siz = (all_rec_siz%4)?(all_rec_siz+(sizeof(dword))-all_rec_siz%4):all_rec_siz;
      cmd_ptr->tc_msg.msg.tc_strsp.num_recs++;
    }
  } /* for */

  if ( ok )
  {
    cmd_ptr->tc_msg.ack_req = FALSE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
  else
  {
      q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
        /* Return tx message buf to free queue. */
      rej_parms.rej_msg_type = CAI_TC_STREQ_MSG;
      rej_parms.rej_code =  reject_reason;
      send_rej_order( &rej_parms );

  }

} /* send_status_rsp_msg() */

#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */

/*===========================================================================

FUNCTION TC_RX_RPT

DESCRIPTION
  This function processes reports from the Receive task during the Traffic
  Channel substates.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word tc_rx_rpt
(
  mccrx_rpt_type *rpt_ptr,
    /* pointer to report received from RX */
  word next_state
    /* next state to be processed */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  mccccim_event_type mccccim_evt;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MC enters release substate while a CFS or GPS is still
   * in progress, (for example, user presses END key), we still
   * need to wait and process the RXC report, namely, RXC_TX_ON_R,
   * in order to turn on the transmitter, send the release order
   * to the base station, wait for the base station's release
   * order, etc, etc...
   */
  if ( (cdma.curr_state == CDMA_RELEASE)  /*lint !e641 */
       && (rpt_ptr->hdr.rpt != RXC_TX_ON_R)
     )
  {
    cmd_done( &rpt_ptr->hdr.rpt_hdr );
    return (next_state);
  }

  switch (rpt_ptr->hdr.rpt)
  {
    case RXC_TX_OFF_R:
    {
      #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
      #endif /* FEATURE_IS2000_REL_C_DV */

      if( cdma.entry_ptr->entry != MCC_ENTER_OFFLINE )
      {
        /* stop transmitting but stay in this state */
        MSG_HIGH( "Bad frames stop tx", 0, 0, 0 );
        cdma.tc_tx_to_be_on = FALSE;

        #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
        #endif /* FEATURE_IS2000_REL_C_DV */

        {
          mcctxns_begin_tx_on_off_trans( FALSE);
        }
      }
      break;
    }

    case RXC_TX_ON_R:
    {
      #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
      #endif /* FEATURE_IS2000_REL_C_DV */
      
      cdma.tc_tx_to_be_on = TRUE;

      /* Start transmitting again */
      if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
      {
        MSG_HIGH( "Good frames resume tx", 0, 0, 0 );
      }

      /* This signals the event that the handoff is finished.
       * Pass control to the handoff module to do any handoff
       * processing required for this event.
       */
      mcctcho_post_process_handoff();

      #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
      #endif /* FEATURE_IS2000_REL_C_DV */
      {
        mcctxns_begin_tx_on_off_trans( TRUE);
      }

      /* Tell TX to start transmitting again */
      MSG_MED( "Initiating transmission again at %lx",
             txc_get_time_in_1_25ms_unit(), 0, 0 );

      #ifdef FEATURE_GPS_MODE
#error code not present
      #endif /* FEATURE_GPS_MODE */

      #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
      #endif /* FEATURE_IS2000_REL_C_DV */

      break;
    }

    case RXC_PWR_ON_R:
    {
      MSG_MED( "Rxed RXC_PWR_ON_R", 0, 0, 0 );

      /* If power measurement reporting was on turn it back on */
      if (mcc_rx_periodic_rpt_on || mcc_rx_threshold_rpt_on)
      {
        MSG_MED( "Sending RXC_PWR_ON_F", 0, 0, 0 );
        mcc_rx_buf.pwr_on.hdr.command = RXC_PWR_ON_F;
        mcc_rx_buf.pwr_on.pwr_thresh_enable = mcc_rx_threshold_rpt_on;
        mcc_rx_buf.pwr_on.pwr_period_enable = mcc_rx_periodic_rpt_on;
        mcc_rxc_cmd( &mcc_rx_buf );
      }
      break;
    }

    case RXC_TC_TMO_R:
    {
      MSG_HIGH( "Rxed RXC_TC_TMO_R", 0, 0, 0 );

      if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
      {
        MSG_ERROR( "Bad frames - call aborted", 0, 0, 0 );
        /* We hit a fade, Delete all CC instances */
        mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
        mccccim_evt.del_all_cc.del_reason = MCCCCIM_FADE;
        mccccim_process_event( &mccccim_evt );
        /* No need to check status */

        mcc_exit.exit = MCC_SYSTEM_LOST;
        next_state = CDMA_EXIT;
      }
      break;
    }

    case RXC_PWR_R:
    {
      /* Build a Power Measurement Report Message and send it */

      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
      {
        ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
      }
      else
      {
        build_pwr_meas( &cmd_ptr->tc_msg.msg,  rpt_ptr );

        /* ---------------------------------------------------
        ** Send the Power Measurement message as not requiring
        ** acknowledgement as per the latest IS95 TSB's and
        ** for data testing at high FER.
        ** --------------------------------------------------- */
        cmd_ptr->tc_msg.ack_req = FALSE;

        if ( cmd_ptr->tc_msg.msg.pwr_rpt.sch_pwr_meas_incl )
        {
          /* If SCH measurements are included, we don't need to know when the
             message is actually transmitted because this is not the normal
             PMRM */
          mcctc_fill_rxtx_hdr_free_q( cmd_ptr );
        }
        else
        {
          /* -------------------------------------------------------------
          ** We need to know when this message is txed for timing purposes
          ** ------------------------------------------------------------- */
          mcctc_fill_rxtx_hdr_ack_q ( cmd_ptr );
        }

        rxtx_cmd( cmd_ptr );
      }
      break;
    }

    case RXC_VALID_R:
    {
      /* Good frames received on new frequency Traffic Channel */

      /* Send command to the Receive task to start receiving messages */
      mcc_rx_buf.tc.hdr.command = RXC_TC_F;
      mcc_rxc_cmd( &mcc_rx_buf );

      /* If power measurement reporting was on turn it back on */
      if (mcc_rx_periodic_rpt_on || mcc_rx_threshold_rpt_on)
      {
        mcc_rx_buf.pwr_on.hdr.command = RXC_PWR_ON_F;
        mcc_rx_buf.pwr_on.pwr_thresh_enable = mcc_rx_threshold_rpt_on;
        mcc_rx_buf.pwr_on.pwr_period_enable = mcc_rx_periodic_rpt_on;
        mcc_rxc_cmd( &mcc_rx_buf );
      }

      /* Tell TX to start transmitting again */
      mcctxns_begin_tx_on_off_trans( TRUE);
      break;
    }

    case RXC_HHO_TIMEOUT_R:
    {
      /* Pass control to the handoff module to process this event. */
      mcctcho_process_failed_hho( MCCTCHO_HANDOFF_TIMER_EXPIRE );
      break;
    }

    case RXC_OL_R:
    {
      /* Build the Outer Loop Report Message and send it */

      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
      {
        ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
      }
      else
      {
        build_outer_loop_report_msg( &cmd_ptr->tc_msg.msg,  rpt_ptr );

        /* Send msg in assured mode */
        cmd_ptr->tc_msg.ack_req = TRUE;

        /* Fill in the command header */
        mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

        rxtx_cmd( cmd_ptr );
      } //lint !e787
      break;
    }

    default:
    {
      break;
    }
  } /* switch */ /*lint !e788 */

  cmd_done( &rpt_ptr->hdr.rpt_hdr );

  return( next_state );

} /* tc_rx_rpt */

/*===========================================================================

FUNCTION TC_SRCH_RPT

DESCRIPTION
  This function processes the reports from the Searcher task during the
  Conversation substate.

DEPENDENCIES
  None.

RETURN VALUE
  Reason for system exit.

SIDE EFFECTS
  None.

===========================================================================*/

mcc_exit_name_type tc_srch_rpt
(
  mccsrch_rpt_type *rpt_ptr,
    /* report received from Searcher */
  mcc_exit_name_type curr_tc_exit
    /* Reason of pending tc exit */
)
{
  rxtx_cmd_type *cmd_ptr;
  mcc_exit_name_type new_tc_exit = curr_tc_exit;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MC enters release substate while a CFS or GPS is still
   * in progress, (for example, user presses END key), we still
   * need to wait and process the search reports in order to
   * tune RF back to the serving CDMA frequency, turn on the
   * transmitter, send the release order to the base station,
   * wait for the base station's release order, etc, etc...
   */
  if ( (cdma.curr_state == CDMA_RELEASE)  /*lint !e641 */
         && (rpt_ptr->hdr.rpt != SRCH_RIF_HHO_ABORT_R)
         && (rpt_ptr->hdr.rpt != SRCH_RIF_HHO_FAILURE_R)
         && (rpt_ptr->hdr.rpt != SRCH_CF_MEAS_DONE_R)
         && (rpt_ptr->hdr.rpt != SRCH_CF_NSET_R)
         #ifdef FEATURE_GPS_MODE
#error code not present
         #endif /* FEATURE_GPS_MODE */
         && ( rpt_ptr->hdr.rpt != SRCH_REQUEST_SYS_RESTART_R )
     )
  {
    cmd_done( &rpt_ptr->hdr.rpt_hdr );
    return new_tc_exit;
  }

  switch (rpt_ptr->hdr.rpt)
  {
    case SRCH_TC_MEAS_R:
    {
      /* Generate Autonomous Pilot Strength Measurement message */
      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
      {
        ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
      }
      else
      {
        #ifdef FEATURE_IS2000_REL_A
        if (P_REV_IS_7_OR_GREATER)
        {
          /* Need to send Ext PSMM message */
          build_ext_pil_str( &cmd_ptr->tc_msg.msg, rpt_ptr );
        }
        else
        #endif /* FEATURE_IS2000_REL_A */

        {
          build_pil_str( &cmd_ptr->tc_msg.msg, rpt_ptr );
        }

        cmd_ptr->tc_msg.ack_req = TRUE;

        /* Fill in the command header */
        mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

        rxtx_cmd( cmd_ptr );
      }

      break;
    }

    case SRCH_PPM_R:
    {
      /* Generate a PPSMM */
      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
      {
        ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
      }
      else
      {
        /* build the PPSM message */
        build_ppm( &cmd_ptr->tc_msg.msg, rpt_ptr );
        cmd_ptr->tc_msg.ack_req = FALSE;

        /* Fill in the command header */
        mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );
        rxtx_cmd( cmd_ptr );
      }

      break;
    }

    case SRCH_RIF_HHO_ABORT_R:
    {
      MSG_MED( "Rxed SRCH_RIF_HHO_ABORT_R at %lx",
               txc_get_time_in_1_25ms_unit(), 0, 0 );
      MSG_MED( "HHO failure reason = %d",
               rpt_ptr->hho_abort.hho_fail_reason, 0, 0 );

      switch( rpt_ptr->hho_abort.hho_fail_reason )
      {
        case DIFF_RX_PWR_TOO_LOW:
        {
          mcctcho_process_failed_hho( MCCTCHO_DIFF_RX_PWR_TOO_LOW );
          break;
        }
        case TOTAL_PILOT_EC_IO_TOO_LOW:
        {
          mcctcho_process_failed_hho( MCCTCHO_TOTAL_PILOT_EC_IO_TOO_LOW );
          break;
        }
        default:
        {
          ERR_FATAL( "Bad hho failure reason %d ",
            rpt_ptr->hho_abort.hho_fail_reason, 0, 0 );
          break;
        }
      } /*lint !e788 */

      break;
    }

    case SRCH_RIF_HHO_FAILURE_R:
    {
      MSG_MED( "Rxed SRCH_RIF_HHO_FAILURE_R at %lx",
               txc_get_time_in_1_25ms_unit(), 0, 0 );

      mcctcho_return_to_serv_freq( rpt_ptr );
      break;
    }

    case SRCH_CFS_READY_R:
    {
      MSG_MED( "Rxed SRCH_CFS_READY_R at %lx",
               txc_get_time_in_1_25ms_unit(), 0, 0 );

      mcctcho_start_cf_meas( rpt_ptr );
      break;
    }

    case SRCH_CF_MEAS_DONE_R:
    {
      MSG_MED( "Rxed SRCH_CF_MEAS_DONE_R at %lx",
               txc_get_time_in_1_25ms_unit(), 0, 0 );

      mcctcho_return_to_serv_freq( rpt_ptr );
      break;
    }

    case SRCH_CF_NSET_R:
    {
      MSG_MED( "Rxed SRCH_CF_NSET_R at %lx",
               txc_get_time_in_1_25ms_unit(), 0, 0 );

      mcctcho_process_srch_cf_nset_rpt( rpt_ptr );
      break;
    }

    #ifdef FEATURE_GPS_MODE
#error code not present
    #endif /* FEATURE_GPS_MODE */

    case SRCH_FING_DONE_R:
    {
      MSG_MED( "Rxed SRCH_FING_DONE_R ",
               0, 0, 0 );
      /* Nothing needs to done when it's not MAHHO */
      break;
    }

    case SRCH_AT_R:
    {
      MSG_MED( "Rxed SRCH_AT_R at %lx",
               txc_get_time_in_1_25ms_unit(), 0, 0 );
      /* Eventually a trans_num will be passed to this */
      do_successful_handoff_operations();
      break;
    }

#ifdef FEATURE_IS2000_R_SCH

    case SRCH_SCRM_MEAS_R:
    {
      MSG_MED("Rxed SRCH_SCRM_MEAS_R", 0, 0, 0);

      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
      {
        ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
      }
      else
      {
        /* Build Supplemental Channel Request Message */
        mccds_build_sup_chan_req_msg(&cmd_ptr->tc_msg.msg, rpt_ptr, FALSE);
        cmd_ptr->tc_msg.ack_req = FALSE;

        /* Fill in the command header */
        mcctc_fill_rxtx_hdr_free_q( cmd_ptr );
        rxtx_cmd( cmd_ptr );
      }

      break;
    }

    case SRCH_T_ADD_ABORT_R:
    {
      MSG_MED("Rxed SRCH_T_ADD_ABORT_R", 0, 0, 0);
      mccsch_process_t_add_abort(rpt_ptr);
      break;
    }
#endif /* FEATURE_IS2000_R_SCH */

    case SRCH_SYS_MEAS_R:
    {
      /* Report system measurements to System Determination */
      mcc_sys_meas_rpt(rpt_ptr);
      /* -------------------------------------------------------------
      ** Inform system determination of system selection timer expires
      ** and reselection is not allowed in the current state.
      ** ------------------------------------------------------------- */
      (void) sd_ss_ind_misc_meas_rprt (rpt_ptr->sys_meas.meas_id, FALSE,
               NULL);
      break;
    }

    case SRCH_WAKEUP_R:
    {
      MSG_MED("Ignoring SRCH_WAKEUP_R", 0, 0, 0);
      break;
    }

    #ifdef FEATURE_IS2000_REL_C_DV
#error code not present
    #endif /* FEATURE_IS2000_REL_C_DV */

    case SRCH_REQUEST_SYS_RESTART_R:
    {
      MSG_MED( "SRCH request sys restart", 0, 0, 0 );
      new_tc_exit = MCC_SRCH_REQUEST_SYS_RESTART;
      break;
    }

    default:
    {
      ERR("Invalid SRCH Report %d", rpt_ptr->hdr.rpt, 0, 0);
      break;
    }

  } /* end switch */ /*lint !e788 */

  cmd_done( &rpt_ptr->hdr.rpt_hdr );

  return new_tc_exit;

} /* tc_srch_rpt */

/*===========================================================================

FUNCTION TC_TRANS_DONE_RPT

DESCRIPTION
  This function processes the transaction done reports from transaction servers
  (can be RXC, TXC, or SRCH).  Note that no report type is included in this
  report because it is not needed.  This report is currently used only to indicate
  the completion of a particular transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_trans_done_rpt( void )
{
#ifdef FEATURE_AUTH
  cm_mc_rpt_type * cm_ptr;
    /* Pointer to report that may be sent to CM */
#endif /* FEATURE_AUTH */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mctrans_rpt_type *rpt_ptr;

  // Retrive all transaction reports from transaction report queue.
  while ( (rpt_ptr = (mctrans_rpt_type*) q_get( &mctrans_rpt_q )) != NULL )
  {
    // We need to find out if this transaction belongs to the new transaction type,
    // if so, we'll call the appropriate
    switch ( rpt_ptr->trans_type )
    {
      case MCTRANS_LONG_CODE_MASK:

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

        /* Report long code mask change */
        mclog_report_event_long_code_mask(FALSE);
        mctrans_release_pending_msg(rpt_ptr->trans_num);
        break;

      #ifdef FEATURE_IS2000_SCH
      case MCTRANS_F_SCH0_START :
        // A start F-SCH transaction just complete, and since we get this signal
        // we now go ahead and generate the stop transaction (note: for infinite
        // burst, we won't get this signal).
        MSG_MED("txn %d done:cont w/stop SCH burst txn",rpt_ptr->trans_num,0,0);
        mccsch_stop_f_sch0_trans();
        break;
      #ifdef FEATURE_IS2000_R_SCH
      case MCTRANS_R_SCH0_START :
                // A start R-SCH transaction just complete, we need to generate the
        // update transcation, stop transaction and the burst extension
        // transaction if necessary
        if (mccsch_is_use_t_add_abort_changed(rpt_ptr->trans_type))
        {
          mccsch_update_r_sch_parm_trans();
        }

        if (mccsch_is_r_sch_finite_burst(0))
        {
          mccsch_begin_r_sch0_extend_trans();
          mccsch_stop_r_sch0_trans();
        }
        break;

      case MCTRANS_R_SCH0_STOP:
        // A STOP R-SCH transaction just complete, we need to generate the
        // update transaction if necessary.
        mccsch_update_r_sch_parm_trans();
        break;


      #endif // FEATURE_IS2000_R_SCH
      #endif // FEATURE_IS2000_SCH

      case MCTRANS_SCR :
      {
        // This is received when a SCR transaction just finished (by RXC/TXC)
        // We need to continue with some cleanup stuff (e.g. notifying UI, sending
        // service connect completion message).
        srv_event_type  ev;
        mccccim_event_type  mccccim_evt;
        const caii_srv_cfg_type * cfg_ptr;

        if ( cdma.serv_neg )
        {
          ev.ec = SRV_SCR_CMP_EC;
          srv_rpt_event ( &ev );
          /* Now tell CCIM to be update its data base */
          mccccim_evt.update_conrec.event_code = MCCCCIM_UPDATE_CON_RECS;
          mccccim_evt.update_conrec.new_cfg_ptr = snm_get_current_config();
          mccccim_process_event( &mccccim_evt );

        }
        else
        {
          // Do the non-time critical part of configuration update
          snm_continue_update_config();
          /* Now tell CCIM to change the SO for the existing CC */
          mccccim_evt.update_so.event_code = MCCCCIM_UPDATE_SO;
          cfg_ptr = snm_get_current_config();
          if (cfg_ptr->num_con)
          {
            mccccim_evt.update_so.so = cfg_ptr->con[0].so;
            mccccim_process_event( &mccccim_evt );
          }
        }

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
        /* Update active Sync ID info */
        mccsyncid_update_active_sync_id();
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

        break;
      }
      case MCTRANS_HO :
      {
        mcctcho_handle_ho_action_time_arrival();
        break;
      }

      case MCTRANS_HHO_BEGIN:
      {
        /* Log the HO event */
        mcctcho_log_handoff_event();

        break;
      }
      case MCTRANS_PWR_CTRL_MSG :
      {
        const mctrans_pending_action_msg_type* pending_msg = mctrans_get_pending_msg( rpt_ptr->trans_num );

        if (pending_msg != NULL)
        {
          // When pwr_cntl_step is updated, update that in cdma structure
          cdma.pwr_cntl_step = pending_msg->pwr_ctrl.pwr_cntl_step;

          // Now also update the nnscr, if needed
          snm_update_nn_scr_from_pcnm( &pending_msg->pwr_ctrl );

          // Release the message now.
          mctrans_release_pending_msg( rpt_ptr->trans_num );
        }

        break;
      }

      case MCTRANS_CFS_NSET:
      case MCTRANS_CFS_CTRL:
      {
        const mctrans_pending_action_msg_type* pending_msg = mctrans_get_pending_msg( rpt_ptr->trans_num );
        if (pending_msg != NULL)
        {
          /* Update CFS parameters locally and start srch if necessary */
          mcctcho_update_cfs_params( pending_msg);

          // Release the message now.
          mctrans_release_pending_msg( rpt_ptr->trans_num );
        }
        break;
      }

      case MCTRANS_TX_ON_OFF:
      {
        tc_inform_rxtx_about_tx();
        break;
      }

      case MCTRANS_CFS_GPS_PREP:
      {
#ifdef FEATURE_GPS_MODE
#error code not present
#endif
        {
          /* This is for CFS */
          tc_inform_rxtx_about_tx();
        }

        break;
      }

#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV */
    } //lint !e787 Some enums are not used in this switch

    cmd_done( &rpt_ptr->rpt_hdr );
  }
} /* tc_trans_done_rpt */

/*===========================================================================

FUNCTION SO_CONV

DESCRIPTION
  This function processes the service option actions needed on the change to
  Conversation substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void so_conv
(
void
)
{
  cm_mc_rpt_type *cm_ptr;
    /* pointer to command to send to ui */

  /* Tell TXC we are in the conv sub state
  so that it can connect SO to vocoder once a SO
  is available */
  mcc_tx_buf.conn_so.hdr.command = TXC_CONNECT_SO_F;
  mcc_txc_cmd( &mcc_tx_buf );

  if ((cdma.so_cur_pri == CAI_SO_VOICE_IS96A)
#ifdef FEATURE_EVRC
      || (cdma.so_cur_pri == CAI_SO_VOICE_EVRC)
#endif /* FEATURE_EVRC */
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
      || (cdma.so_cur_pri == CAI_SO_VOICE_13K_IS733)
      || (cdma.so_cur_pri == CAI_SO_VOICE_13K)
#endif
#ifdef FEATURE_SO_VOICE_SMV
      || (cdma.so_cur_pri == CAI_SO_VOICE_SMV)
#endif /* FEATURE_EVRC */
#ifdef FEATURE_VOC_4GV
      || ((cdma.so_cur_pri == CAI_SO_VOICE_4GV_NB) ||
          (cdma.so_cur_pri == CAI_SO_VOICE_4GV_WB)) 
#endif /* FEATURE_VOC_4GV */
     )
  {
    /* -------------------------------------------
    ** Send indicator to UI that call is connected
    ** ------------------------------------------- */

    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
    else
    {
      cm_ptr->connect.hdr.cmd = CM_CALL_CONNECTED_F;
      cm_ptr->connect.so      = cdma.so_cur_pri;
      mcc_cm_mc_rpt( cm_ptr );
    }
  } /* end if (cdma.so_cur_pri == CAI_SO_VOICE) */

  else if ((cdma.so_cur_pri == CAI_SO_LOOPBACK) ||
           (cdma.so_cur_pri == CAI_SO_LOOPBACK_13K) ||
           (cdma.so_cur_pri == CAI_SO_RS1_MARKOV) ||
           (cdma.so_cur_pri == CAI_SO_RS2_MARKOV)
           || (cdma.so_cur_pri == CAI_SO_MARKOV_SO54)
           || (cdma.so_cur_pri == CAI_SO_LOOPBACK_SO55)
           #ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
           #endif /* FEATURE_MDR_DPT_MARKOV */
           #ifdef FEATURE_IS2000_TDSO
           || (cdma.so_cur_pri == CAI_SO_TDSO)
           || (cdma.so_cur_pri == CAI_SO_FULL_TDSO)
           #endif // FEATURE_IS2000_TDSO
           )
  {

    /* -------------------------------------------
    ** Send indicator to UI that call is connected
    ** ------------------------------------------- */

    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
    else
    {
      cm_ptr->connect.hdr.cmd = CM_CALL_CONNECTED_F;
      cm_ptr->connect.so      = cdma.so_cur_pri;
      mcc_cm_mc_rpt( cm_ptr );
    }
  }

#ifdef FEATURE_DS
  else if(DS_ISDATA(cdma.so_cur_pri))
  {

    /* -------------------------------------------
    ** Send indicator to UI that call is connected
    ** ------------------------------------------- */

    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
    else
    {
      cm_ptr->connect.hdr.cmd = CM_CALL_CONNECTED_F;
      cm_ptr->connect.so      = cdma.so_cur_pri;
      mcc_cm_mc_rpt( cm_ptr );
    }
  }
#endif /* FEATURE_DS */

#ifdef FEATURE_CDSMS
  else if( cdma.so_cur_pri == CAI_SO_SMS ||
           cdma.so_cur_pri == CAI_SO_RS2_SMS )
  {
    /* ------------------------------------------------------
    ** Send indicator to CM that call is connected
    ** ------------------------------------------------------ */
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
    else
    {
      cm_ptr->connect.hdr.cmd = CM_CALL_CONNECTED_F;
      cm_ptr->connect.so      = cdma.so_cur_pri;
      mcc_cm_mc_rpt( cm_ptr );
    }
  } /* end of SMS SO */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_OTASP_OTAPA
  else if( cdma.so_cur_pri == CAI_SO_RS1_OTAPA ||
           cdma.so_cur_pri == CAI_SO_RS2_OTAPA )
  {

    /* ------------------------------------------------------
    ** Send indicator to CM that call is connected
    ** ------------------------------------------------------ */
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
    else
    {
      cm_ptr->connect.hdr.cmd = CM_CALL_CONNECTED_F;
      cm_ptr->connect.so      = cdma.so_cur_pri;
      mcc_cm_mc_rpt( cm_ptr );
    }
  } /* end of OTAPA SO */
#endif /* FEATURE_OTASP_OTAPA */

#ifdef FEATURE_DBM
  else if( cdma.so_cur_pri == CAI_SO_RS1_PD ||
           cdma.so_cur_pri == CAI_SO_RS2_PD )
  {
    /* ------------------------------------------------------
    ** Send indicator to CM that call is connected
    ** ------------------------------------------------------ */
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
    else
    {
      cm_ptr->connect.hdr.cmd = CM_CALL_CONNECTED_F;
      cm_ptr->connect.so      = cdma.so_cur_pri;
      mcc_cm_mc_rpt( cm_ptr );
    }
  } /* end of PD SO */
#endif /* FEATURE_DBM*/

  /* Inform system determination of MO call connected */
  if(cdma.call_orig == TRUE)
  {
    (void) sd_ss_ind_misc_orig_success(NULL);
  }

} /* end of so_cov() */

#if (defined (FEATURE_DH) && defined(FEATURE_DH_EXP))
/*===========================================================================

FUNCTION TC_DH_RPT

DESCRIPTION
  This function processes a report from the DH task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tc_dh_rpt
(
  dh_rpt_type *dh_rpt_ptr
    /* Pointer to dh report */
)
{
  MSG_MED( "Got a DH report while in the traffic channel", 0, 0, 0 );

  if (dh_rpt_ptr->rpt_type == DH_EXP_R )
  {
#ifdef FEATURE_OTASP
     otasp_process_dh_report( dh_rpt_ptr );
#endif /* FEATURE_OTASP */
  }
  else    /* we got the wrong report */
  {
    ERR( "Wrong report %d from DH!", dh_rpt_ptr->rpt_type, 0, 0 );
  }
  cmd_done( &dh_rpt_ptr->rpt_hdr );

} /* tc_dh_rpt() */
#endif /* FEATURE_DH */

/*===========================================================================

FUNCTION PROCESS_PWR_CTRL_MSG

DESCRIPTION
  This function processes the forward and reverse power control parameters
  received in the power control message. The forward power control parameters 
  are sent to RXC and the reverse power control parameters are sent 
  to TXC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_pwr_ctrl_msg
(
  caii_rx_msg_type * msg_ptr
)
{
  qword abs_action_time;
  byte max_fpc_mode;
  int i;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

  mcc_fwd_chan_mask_type fwd_chan_mask;
 
#ifdef FEATURE_FACTORY_TESTMODE
  if (ftm_mode == FTM_MODE)
  {
    return;
  }
#endif /* FEATURE_FACTORY_TESTMODE */

  /* First validate the message */
  if ( msg_ptr->pwr_ctrl.pwr_cntl_step > CAI_PWR_CNTL_STEP_0_25_DB )  /*lint !e641 */
  {
    MSG_MED( "PCM Rejected", 0,0,0 );
    rej_parms.rej_msg_type = CAI_FTC_PWR_CTRL_MSG;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
    return;
  }

  if ( P_REV_IS_6_OR_GREATER )
  {
    /* Check for invalid record types in RPC info. */
    if ( msg_ptr->pwr_ctrl.rpc_incl )
    {
      for (i = 0; i < msg_ptr->pwr_ctrl.rpc_num_rec; i++ )
      {
        if ( msg_ptr->pwr_ctrl.rpc_recs[i].rpc_adj_rec_type >= 
             CAI_RPC_ADJ_MAX )  /*lint !e641 */
        {
          rej_parms.rej_msg_type = CAI_FTC_PWR_CTRL_MSG;
          rej_parms.rej_code =  CAI_REJ_FIELD;
          send_rej_order( &rej_parms );
          return;
        }
      }
    }

    /* Check for invalid record types in FPC info. */
    if ( msg_ptr->pwr_ctrl.fpc_incl )
    {
      /* FPC mode is from 0 to 3 for p_rev 6 and 0 to 6 for
         p_rev 7 and above */
#ifdef FEATURE_IS2000_REL_A
      if (P_REV_IS_7_OR_GREATER)
      {
        max_fpc_mode = CAI_FPC_MODE_400_PRI_EIB_50_SEC;  /*lint !e641 */
      }
      else
#endif /* FEATURE_IS2000_REL_A */
      {
        max_fpc_mode = CAI_FPC_MODE_50_PRI;  /*lint !e641 */
      }
      if ( msg_ptr->pwr_ctrl.fpc_mode > max_fpc_mode)
      {
        rej_parms.rej_msg_type = CAI_FTC_PWR_CTRL_MSG;
        rej_parms.rej_code =  CAI_REJ_FIELD;
        send_rej_order( &rej_parms );
        return;
      }

      /* If there is a forward fundicated channel assigned and 
         FPC_PRI_CHAN specifies a forward channel that's not currently 
         assigned, reject (per section 2.6.4.1.1.3 in IS2000 Vol 5).
         -- The condition below (A) checks if either FCH and/or DCCH is in 
         the forward channel mask and (B) takes the fpc_pri_chan value 
         from the pcm (0=FCH, 1=DCCH) and shifts a "1" by that amount... 
         so FCH results in 1 and DCCH results in 2, and  
         that value is bit-wise AND'd with the fwd channel mask to see if 
         the channel is active. The (A) part of the condition is so 
         the MS accepts an FPC_PRI_CHAN for FCH/DCCH in cases where
         no-forward fundicated channel is active.  In that case, FPC
         info is stored for future use, but not applied.  */
      
      fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();      
       
      if ( mccccl_code_channel_list.fwd_fundicated_chan_is_active()
           &&
           ((fwd_chan_mask & (1 << msg_ptr->pwr_ctrl.fpc_pri_chan )) == 0) )  /*lint !e641 */
      {
        MSG_HIGH("Reject PCM - FPC_PRI_CHAN=%d not active", 
                  msg_ptr->pwr_ctrl.fpc_pri_chan,0,0);
        rej_parms.rej_msg_type = CAI_FTC_PWR_CTRL_MSG;
        rej_parms.rej_code =  CAI_REJ_CFG;
        send_rej_order( &rej_parms );
        return;
      }
    }

#ifdef FEATURE_IS2000_REL_D_DV
#error code not present
#endif /* FEATURE_IS2000_REL_D_DV */

  }
  else
  {
    /* For P_REV lower than 6, no RPC, FPC info or action time, so initialize 
       them */
    msg_ptr->pwr_ctrl.fpc_incl = msg_ptr->pwr_ctrl.rpc_incl = FALSE;
    msg_ptr->pwr_ctrl.use_time = 0;
  }

  /* Now that we have a valid message, compute the action time and 
     despatch transaction */
  if ( !msg_ptr->pwr_ctrl.use_time )
  {
    msg_ptr->pwr_ctrl.action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
  }

  get_abs_action_time( abs_action_time, msg_ptr->pwr_ctrl.action_time );
  mcctxns_begin_pwr_ctrl_trans( abs_action_time, (mctrans_pending_action_msg_type*)msg_ptr );

} /* process_pwr_ctrl_msg */

/*===========================================================================

FUNCTION PROCESS_OUTER_LOOP_REPORT_ORDER

DESCRIPTION
  This function processes the input outer loop report request order message.
  The message will be rejected if P_REV_IN_USE is less than 6. Otherwise,
  a command is sent to RXC to request the outer loop report.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_outer_loop_report_order
(
  caii_rx_msg_type * msg_ptr
)
{
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  if (!P_REV_IS_6_OR_GREATER)
  {
    /* Reject if P_REV_IN_USE is less than 6 */
    rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
    rej_parms.rej_ordq = msg_ptr->tc_ord.ordq.ordq;
    rej_parms.rej_msg_type = msg_ptr->gen.msg_type;
    rej_parms.rej_code =  CAI_REJ_UNSP;
    send_rej_order( &rej_parms );
  }
  else
  {
    /* Send command to RXto request outer loop report */
    mcc_rx_buf.olr.hdr.command = RXC_OLR_F;
    mcc_rxc_cmd( &mcc_rx_buf);
    MSG_MED("Send RXC_OLR_F cmd", 0, 0, 0);
  } /* end if */

} /* process_outer_loop_report_order */

/*===========================================================================

FUNCTION PROCESS_RETRY_ORDER

DESCRIPTION
  This function processes the input retry order message. The message will be
  rejected if P_REV_IN_USE is less than 6. Otherwise, if the retry type is
  valid, it will inform DS the changes in retry delay if DS wants.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_retry_order
(
  caii_rx_msg_type * msg_ptr
)
{
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  if (!P_REV_IS_6_OR_GREATER)
  {
    /* Reject if P_REV_IN_USE is less than 6 */
    rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
    rej_parms.rej_ordq = msg_ptr->tc_ord.ordq.ordq;
    rej_parms.rej_msg_type = msg_ptr->gen.msg_type;
    rej_parms.rej_code =  CAI_REJ_UNSP;
    send_rej_order( &rej_parms );
  }
  else
  {
    switch (msg_ptr->tc_ord.retry.retry_type)
    {
  
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

      case CAI_RETRY_ORIG:
      {
        #ifdef FEATURE_IS2000_REL_A
        mcctctag_tag_info_type tag_info;
        /* Retry orders do not have tag ids in them, so just try to get
        the first outstanding data call and assume this retry order
        is for this call , This is a bug in the std */
        if (mcctctag_get_first_found_data_call(&tag_info))
        {
          /* Okay we found an outstanding data call, now inform CM that
          this call has failed due to a retry order */
          /* Note this tag is no longer active now, the std is silent about
          whether this tag should be removed now or if we have to wait for t42m
          to expire before removing this tag, For now just do this as this is
          a more optimal solution */
          mcc_send_orig_fail_retry_delay(msg_ptr->tc_ord.retry.retry_type,
          msg_ptr->tc_ord.retry.retry_delay, tag_info.so, tag_info.call_id);

          /* need to tell SNM that the EOM is gone and clean up */
          snm_update_for_deleted_connection(tag_info.sr_id);
        }
        #endif /*FEATURE_IS2000_REL_A*/

        /* Inform DS the retry delay changes */
        update_retry_delay((cai_retry_order_type)msg_ptr->tc_ord.retry.retry_type,
          msg_ptr->tc_ord.retry.retry_delay);

        break;

      }

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

      case CAI_RETRY_CLR_ALL:
      case CAI_RETRY_RESA:
      case CAI_RETRY_SCRM:
        /* Inform DS the retry delay changes */
          update_retry_delay((cai_retry_order_type)msg_ptr->tc_ord.retry.retry_type,
          msg_ptr->tc_ord.retry.retry_delay);
        break;


      default:
        /* Reject if invalid ordq field */
        rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
        rej_parms.rej_ordq = msg_ptr->tc_ord.ordq.ordq;
        rej_parms.rej_msg_type = msg_ptr->gen.msg_type;
        rej_parms.rej_code =  CAI_REJ_FIELD;
        send_rej_order( &rej_parms );
        break;
    } /* end switch */
  } /* end if */

} /* process_retry_order */

/*===========================================================================

FUNCTION VALIDATE_PILOT_REC_TYPE

DESCRIPTION
  This function validates the pilot rec types received in a TC message like
  G/UHDM, ESCAM.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if pilot_record is good, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean validate_pilot_rec_type
(
  cai_pilot_rec_type pilot_rec_type,
  caii_add_pilot_rec_type * pilot_rec
)
{
  boolean result = TRUE;
  /* For p_rev = 6, only one pilot rec type is allowed */
  if (
      #ifdef FEATURE_IS2000_REL_A
      !P_REV_IS_7_OR_GREATER && 
      #endif /* FEATURE_IS2000_REL_A */
      (pilot_rec_type > CAI_PILOT_REC_1X_COM_TD)) //lint !e506
  {
    MSG_HIGH("pilot rec type not supported %d",pilot_rec_type,0,0);
    return FALSE;
  }

  switch (pilot_rec_type)
  {
    case CAI_PILOT_REC_1X_COM_TD:
      if (!mccap_is_td_mode_supported(pilot_rec->rec0.td_mode))
      {
        MSG_HIGH("pilot rec type and td mode not supported %d, %d",pilot_rec_type,
                 pilot_rec->rec0.td_mode,0);
        result = FALSE;
      }
      break;

    case CAI_PILOT_REC_1X_AUX:
      /* No verification for now, but pilot record type is allowed */
      break;
#ifdef FEATURE_IS2000_REL_A
    case CAI_PILOT_REC_1X_AUX_TD:
      if (!mccap_is_td_mode_supported(pilot_rec->rec2.td_mode))
      {
        MSG_HIGH("pilot rec type and td mode not supported %d, %d",pilot_rec_type,
                 pilot_rec->rec2.td_mode,0);
        result = FALSE;
      }
      break;
#endif /* FEATURE_IS2000_REL_A */
    default:
      MSG_HIGH("pilot rec type not supported %d",pilot_rec_type,0,0);
      result = FALSE;
  }
  return result;

}


#if defined (FEATURE_IS2000_CHS) || defined (FEATURE_IS2000_REL_A)
/*===========================================================================

FUNCTION PROCESS_ERM

  This function process the extended release message

DEPENDENCIES
  None

RETURN VALUE
  The next state.

SIDE EFFECTS
  None

===========================================================================*/

word process_erm( caii_erm_type *erm, word cur_state )
{
  /* This stores the current fwd chanel mask. */
  mcc_fwd_chan_mask_type cur_fwd_chan_mask;
  
  /* This stores the mask of channels obtained by calling gen_fwd_chan_mask_erm 
     It is only used for processing ch_ind i.e. when use_ext_chind is set to 
     FALSE. 
  */
  mcc_fwd_chan_mask_type erm_fwd_chan_mask; 

  /* This stores the fwd channels to release */
  mcc_fwd_chan_mask_type fwd_chans_to_release;
  
  /* By symmetry, rev chan masks are for reverse channels. See description 
     for fwd channels above
  */
  mcc_rev_chan_mask_type cur_rev_chan_mask, erm_rev_chan_mask,
                        rev_chans_to_release;

#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif
  
  mcc_rev_chan_mask_type new_rev_chan_mask;

  boolean reject_erm = FALSE;

  qword abs_action_time;
  rxtx_cmd_type *cmd_ptr;
  /* ptr to retrieve the non negotiable srv config records */
  caii_non_neg_srv_cfg_type *nnscr;
  byte saved_fpc_pri_chan;
  snm_status_type eval_result;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  mccccim_event_type mccccim_evt;
    /* Event rpt struct for sending primitives to CCIM */
  cai_ext_chind_type ota_extchind;
  word new_state=cur_state;
    /* Variable for returing new state to calling fn */
#ifdef FEATURE_IS2000_REL_C
  int i;
#endif

  /* Retrieve the current internal fwd/rev channel masks */
  cur_fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask(); 
  cur_rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask(); 

  ota_extchind = CAI_EXT_CHIND_UNSET;


  MSG_HIGH("process_erm called",0,0,0);


#ifdef FEATURE_IS2000_REL_C
  if (!P_REV_IS_9_OR_GREATER)
  {
    erm->use_ext_ch_ind = 0;
  }
  if (erm->use_ext_ch_ind)
  {
#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#else
    MSG_HIGH("ERM received with wrong use_ext_ch_ind",0,0,0);
    rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
    return (new_state);
#endif
  }
#endif

  /* Split OTA chind into fwd/rev channel masks indicated channels to release */
  erm_fwd_chan_mask = mccccl_code_channel_list.gen_fwd_chan_mask_erm((cai_chind_type)erm->ch_ind, ota_extchind);
  erm_rev_chan_mask = mccccl_code_channel_list.gen_rev_chan_mask_erm((cai_chind_type)erm->ch_ind, ota_extchind);

  /* start error checking */
  if (!P_REV_IS_6_OR_GREATER)
  {
    MSG_HIGH("ERM received in wrong P_REV",0,0,0);
    rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
    rej_parms.rej_code =  CAI_REJ_CODE;
    send_rej_order( &rej_parms );
    return (new_state);
  }

  /* If received ext_ch_ind == 0 reject message */
#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif

#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif


#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif
  {
    fwd_chans_to_release = mccccl_code_channel_list.gen_fwd_chan_mask((cai_chind_type)erm->ch_ind, CAI_EXT_CHIND_UNSET);
    
    // On DV to Non-DV transition must also release F-PDCH
    fwd_chans_to_release |= ((mccccl_code_channel_list.get_fwd_chan_mask() & MCC_F_PDCH) ? MCC_F_PDCH : 0); 

    // On DV to Non-DV transition must also release F-CPCCH if allocated
    fwd_chans_to_release |= ((mccccl_code_channel_list.get_fwd_chan_mask() & MCC_F_CPCCH) ? MCC_F_CPCCH : 0);  

    rev_chans_to_release = mccccl_code_channel_list.gen_rev_chan_mask((cai_chind_type)erm->ch_ind, CAI_EXT_CHIND_UNSET);

    /* Construct the new channel mask */
    new_rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask() & ~rev_chans_to_release;  


    /* erm_fwd_chan_mask
    */

    /* There is a bug of asymmetry here.
       Channel indicator only tells us to remove DCCH. It does not explicitly
       say to remove F-DCCH or R-DCCH. Hence if we only have R-DCCH or only F-DCCH 
       and we get a ch_ind to remove DCCH, then we may be removing either forward
       or reverse DCCH that we currently don't have.

       One suggestion is to "And" the following two conditions together
    */

    if ((erm_fwd_chan_mask | fwd_chans_to_release) != erm_fwd_chan_mask)
    {
      MSG_HIGH("ch_ind - fwd channel addition",0,0,0);
      reject_erm = TRUE;
    }
    
    if ((erm_rev_chan_mask | rev_chans_to_release) != erm_rev_chan_mask)
    {
      MSG_HIGH("ch_ind - rev channel addition",0,0,0);
      reject_erm = TRUE;
    }
  }

  /* if received channels are not a subset of current channels, reject 
     Note that with asymmetric fundicated channel release the erm*mask 
     may evaluate to zero, see mccccl.c,gen_fwd/rev_chan_mask_erm 
  */

  if (reject_erm)
  {
    MSG_HIGH("ERM: Invalid cfg to go to, ch_ind=%d, ext_chind=%d", 
                          erm->ch_ind,
                          ota_extchind,
                          0);
    rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
    rej_parms.rej_code =  CAI_REJ_CFG;
    send_rej_order( &rej_parms );
    return new_state;
  }

  /* If all of FCH/DCCH are released, we release the call
     So do additional checks only if this is not a 
     complete release of all PHY channels  - for now only look at reverse
     channels.  BKM - will need to come back to this for Rel D.
  */
  if (new_rev_chan_mask & (MCC_R_FCH | MCC_R_DCCH))
  {
    #ifdef FEATURE_IS2000_CHS
    if (mccap_item_is_supported(MCCAP_DF_CHS))
    {
      if ((erm_rev_chan_mask & MCC_R_PICH) &&
           (!(cur_rev_chan_mask & MCC_R_DCCH)))
      {
        /*can't release pilot and go into control hold with DCCH inactive*/
        /* For 1x CHS, test R-DCCH. DV CHM is possible w/o F-DCCH or R-DCCH */
        MSG_HIGH("ERM DCCH not enabled for control hold",0,0,0);
        rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
        rej_parms.rej_code =  CAI_REJ_CFG;
        send_rej_order( &rej_parms );
        return new_state;
      }

      if ((erm_rev_chan_mask & MCC_R_PICH) &&
          ( cur_fwd_chan_mask & MCC_F_FCH) &&
           (!(erm_fwd_chan_mask & MCC_F_FCH)))
      {
        /*can't release pilot and go into control hold with FCH active*/
        /* For 1x CHS, test F-FCH since F-FCH implies R-FCH for 1x.
           For DV CHM, channel assignments may be asymmetrical and R-FCH 
           may be active in DV CHM, but not F-FCH. */ 
        MSG_HIGH("ERM FCH not disabled for control hold",0,0,0);
        rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
        rej_parms.rej_code =  CAI_REJ_CFG;
        send_rej_order( &rej_parms );
        return new_state;
      }
    }
    else
#endif
    if (erm_rev_chan_mask & MCC_R_PICH)
    {
      MSG_HIGH("control hold not supported",0,0,0);
      rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
      rej_parms.rej_code =  CAI_REJ_CAP;
      send_rej_order( &rej_parms );
      return new_state;
    }
  }

  if (cdma.curr_state == CDMA_RELEASE)  /*lint !e641 */
  {
    if (new_rev_chan_mask & (MCC_R_FCH | MCC_R_DCCH))
    {
#ifdef FEATURE_IS2000_REL_C
      MSG_HIGH("Rel sub ERM: not releasing all %d %d",erm->ch_ind,
                                           erm->ext_ch_ind,0);
#else
      MSG_HIGH("Rel sub ERM: not releasing all %d ",erm->ch_ind,0,0);
#endif
      rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );
      return new_state;
    }
    else
    {
      /* Events report: Extended release */
       event_report(EVENT_EXTENDED_RELEASE);
       new_state = CDMA_EXIT;
    }
  }
  else if (!(new_rev_chan_mask & (MCC_R_FCH | MCC_R_DCCH)))
  {
    /* If no reverse fundicated channel is assigned, call is released */
    /* BKM - Come back to this for Rel D */

    /* Events report: Extended release */
    event_report(EVENT_EXTENDED_RELEASE);

    MSG_HIGH("ERM received to release all channels",0,0,0);
    /* release the call */
    /* the ext release order will be send in the release state */
    cdma.end_reason = CAI_EXT_REL_IND;
    cdma.rel_orig = FALSE;

    /* Send Notification to MCCCCIM to release all CC instances */
    mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
    /* All CC instances should already be in release substate */
    mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_BS_END;
    mccccim_process_event( &mccccim_evt );
    /* No need check return status in this case */

    new_state = CDMA_RELEASE;  /*lint !e641 */
  }
  else
  {
    nnscr = (caii_non_neg_srv_cfg_type *) snm_get_current_nnscr();

    /* Save fpc_pri_chan and set it to a channel that is legal, so
    that snm_eval_non_neg_scr() will not reject it. It is restored
    below after the call to snm_eval_non_neg_scr(). The actual
    change in the fpc_pri_chan is done later through the FFPC
    transaction */
    saved_fpc_pri_chan = nnscr->fpc_pri_chan;

    /* check if channel to drop is either FCH or DCCH and only 
       modifying fpc_pri_chan if this is the case. */
    if (erm_fwd_chan_mask & (MCC_F_FCH | MCC_F_DCCH))
    {
      nnscr->fpc_pri_chan = erm_fwd_chan_mask & MCC_F_FCH ? CAI_FPC_PRI_CHAN_DCCH : CAI_FPC_PRI_CHAN_FCH; 
    }

    eval_result = snm_eval_non_neg_scr(nnscr,
                                       (caii_srv_cfg_type *)snm_get_current_config(),
                                        cur_fwd_chan_mask & (~erm_fwd_chan_mask),
                                        cur_rev_chan_mask & (~erm_rev_chan_mask)
                                      );
    
    /* Restore the fpc_pri_chan in nnscr */
    nnscr->fpc_pri_chan = saved_fpc_pri_chan;

    if (eval_result != SNM_SUCCESS)
    {
      /* There were services that depended exclusively on the channel(s) being
      removed by the ERM. The least we can do is reject the ERM */
      MSG_HIGH("ERM trying to remove ch vital to call, ch_ind=%d", erm->ch_ind,0,0);
      rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );
      return new_state;
    }

  #ifdef FEATURE_IS2000_CHS
    if ((erm_rev_chan_mask & MCC_R_PICH) &&
        (erm->gating_rate_incl == FALSE) &&
        (nnscr->gating_rate_incl == FALSE))
    {
      /* reject if erm is releasing cont rev pilot and it doesn't have
      gating rate and NNSCR doesn't have it either */
      MSG_HIGH("Can't go into control hold without gating rate", 0,0,0);
      rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );
      return new_state;
    }

    if ((erm_rev_chan_mask & MCC_R_PICH) &&
        ((nnscr->fpc_mode != CAI_FPC_MODE_800_PRI )  /*lint !e641 */
         && (nnscr->fpc_mode != CAI_FPC_MODE_50_PRI)  /*lint !e641 */
  #ifdef FEATURE_IS2000_REL_A
         && (nnscr->fpc_mode != CAI_FPC_MODE_QIB_50_PRI)  /*lint !e641 */
  #endif
        )
       )
    {
      /* reject if erm is releasing cont rev pilot and we are not in
        right FPC_MODE */
      MSG_HIGH("Can't go into control hold with fpc_mode %d", nnscr->fpc_mode,0,0);
      rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );
      return new_state;
    }
  #endif /* FEATURE_IS2000_CHS */

    /* send the ext release msg and go into control hold */
    if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
    {
      ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
    }
    else
    {
      cmd_ptr->tc_msg.msg.errm.msg_type = CAI_EXT_REL_RSP_MSG;
      cmd_ptr->tc_msg.ack_req = TRUE;
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif
      /* Fill in the command header */
      mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );
      rxtx_cmd (cmd_ptr);
    }

    /* store the FCH configuration if needed */

    /* Send Extended release transaction */
    if (!erm->use_time)
    {
      erm->action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
    }
    get_abs_action_time(abs_action_time, erm->action_time);
    mcctxns_begin_ext_rel_trans(abs_action_time, (mctrans_pending_action_msg_type*)erm); /*lint !e826 */
  }

#ifdef FEATURE_IS2000_REL_C
  /* Store the Direct TC to Idle Info if present */
  if (P_REV_IS_9_OR_GREATER && erm->direct_to_idle_info_incl)
  {
    cdma.rxed_tc_to_idle_info.info_valid = TRUE;
    cdma.rxed_tc_to_idle_info.release_type = erm->release_type;

    if (erm->release_type != CAI_RELEASE_TO_SYS_DETERMINATION)  /*lint !e641 */
    {
      /* Release Type is not SD, so store all the info */
      cdma.rxed_tc_to_idle_info.sid = erm->sid;

      cdma.rxed_tc_to_idle_info.nid = erm->nid;

      cdma.rxed_tc_to_idle_info.freq_incl = erm->freq_incl;

      if (erm->freq_incl)
      {
        cdma.rxed_tc_to_idle_info.cdma_freq = erm->cdma_freq;

        cdma.rxed_tc_to_idle_info.band_class = erm->band_class;
      }
      /* If the pilot list is not present or set to 0, then this info
      applies to any pilot we would choose */
      cdma.rxed_tc_to_idle_info.num_pilots_d2i_incl =
           erm->num_pilots_d2i_incl;

      if (erm->num_pilots_d2i_incl)
      {
        /* Copy the pilots list */
        cdma.rxed_tc_to_idle_info.num_pilots_d2i = erm->num_pilots_d2i;
        for (i=0; i < erm->num_pilots_d2i ; i++)
        {
          cdma.rxed_tc_to_idle_info.pilot_pn[i] =
              erm->pilot_pn[i];
        }
      }
      else
      {
        cdma.rxed_tc_to_idle_info.num_pilots_d2i = 0;
      }

      if (erm->release_type == CAI_RELEASE_TO_IDLE_ON_PCH)  /*lint !e641 */
      {
        /* copy PCH information */
        cdma.rxed_tc_to_idle_info.page_ch = erm->page_ch;
        cdma.rxed_tc_to_idle_info.prat = erm->prat;
      }

      else if (erm->release_type == CAI_RELEASE_TO_IDLE_ON_BCCH)  /*lint !e641 */
      {
        /* copy BCCH information */
        cdma.rxed_tc_to_idle_info.sr1_bcch_code_chan_non_td = erm->sr1_bcch_code_chan_non_td;
        cdma.rxed_tc_to_idle_info.sr1_crat_non_td = erm->sr1_crat_non_td;
        cdma.rxed_tc_to_idle_info.sr1_brat_non_td = erm->sr1_brat_non_td;
      }
   
      else if (erm->release_type == CAI_RELEASE_TO_IDLE_ON_BCCH_TD)  /*lint !e641 */
      {
        /* copy BCCH TD information */
        cdma.rxed_tc_to_idle_info.sr1_td_mode = erm->sr1_td_mode;
        cdma.rxed_tc_to_idle_info.sr1_bcch_code_chan_td = erm->sr1_bcch_code_chan_td;
        cdma.rxed_tc_to_idle_info.sr1_crat_td = erm->sr1_crat_td;
        cdma.rxed_tc_to_idle_info.sr1_brat_td = erm->sr1_brat_td;
      }

    }  /* if (int_ptr->erm.release_type != CAI_RELEASE_TO_SYS_DETERMINATION*/

  } /* If direct to idle info present */
#endif

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

  return new_state;

}
#endif /* defined (FEATURE_IS2000_CHS) || defined (FEATURE_IS2000_REL_A)*/

#ifdef FEATURE_IS2000_CHS
/*===========================================================================

FUNCTION PROCESS_RAM

  This function process the resource allocation msg

DEPENDENCIES
  None

RETURN VALUE
  The next state.

SIDE EFFECTS
  None

===========================================================================*/

word process_ram( caii_ram_type *ram, word cur_state )
{
  qword abs_action_time;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

  /* start error checking */
  if (!P_REV_IS_6_OR_GREATER)
  {
    MSG_HIGH("RAM received in wrong P_REV",0,0,0);
    rej_parms.rej_msg_type = CAI_RES_ALLOC_MSG;
    rej_parms.rej_code =  CAI_REJ_CODE;
    send_rej_order( &rej_parms );
    return (cur_state);
  }

  /* Should only receive this msg in Control Hold Mode */
  if (!mccccl_code_channel_list.in_control_hold())
  {
    MSG_HIGH("RAM is received in none control hold mode",0,0,0);
    rej_parms.rej_msg_type = CAI_RES_ALLOC_MSG;
    rej_parms.rej_code =  CAI_REJ_CODE;
    send_rej_order( &rej_parms );
    return cur_state;
  }

  /* -------------------------------------------------------------------
  ** Reject a RAM with an fpc_pri_chan of 0 (FCH) if we are transitioning
  ** to DCCH-only.  In Release 0, it is only possible to transition to 
  ** DCCH-only, hence we always reject a RAM with fpc_pri_chan 0.  In 
  ** Release A, we can transition to either ch_ind 6 (DCCH-only), or 
  ** ch_ind 7 (FCH + DCCH); so check if FCH is to be restored.
  ** -------------------------------------------------------------------- */
#ifdef FEATURE_IS2000_REL_A_SVD
  if ((!mccccl_code_channel_list.get_fch_to_be_restored()) && (ram->fpc_pri_chan == 0))
#else
  if (ram->fpc_pri_chan == 0)
#endif /* FEATURE_IS2000_REL_A_SVD */
  {
    MSG_HIGH("RAM's fpc_pri_chan is 0, but transitioning to DCCH-only",0,0,0);
    rej_parms.rej_msg_type = CAI_RES_ALLOC_MSG;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
    return(cur_state);
  }

  if (!ram->use_time)
  {
    ram->action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
  }
  get_abs_action_time(abs_action_time, ram->action_time);

  /* begin transition back to active mode */
  mcctxns_begin_active_trans(abs_action_time, (mctrans_pending_action_msg_type*)ram); /*lint !e826 */

  return cur_state;

}
#endif /* FEATURE_IS2000_CHS */


#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION IS_CON_REF_IN_CUR_SCR

  This function searches the current SCR to see if there is a record contains
  CON_REF.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if found. FALSE if not.

SIDE EFFECTS
  None

===========================================================================*/
boolean is_con_ref_in_cur_scr
(
  byte con_ref
    /* Connection Reference */
)
{
  caii_srv_cfg_type* cur_scr;
    /* Pointer to the current SCR */
  byte i;
    /* loop variable */

  cur_scr = (caii_srv_cfg_type*)snm_get_current_config();

  for(i=0; i<cur_scr->num_con; i++)
  {
    if (cur_scr->con[i].con_ref == con_ref)
    {
      return TRUE;
    }
  }

  return FALSE;

}  /* is_con_ref_in_cur_scr */

/*===========================================================================

FUNCTION SEND_EOM

  This function sends an Enhanced Origination Message.

DEPENDENCIES
  None

RETURN VALUE
  Status

SIDE EFFECTS
  None

===========================================================================*/
mcctc_eom_status_type send_eom
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte i;
    /* Index variable */
  word so_req;

  byte max_alt_so = 0;               /* Number of alt SOs */
  #ifdef FEATURE_AUTH
  caii_enc_cap_type enc_capability;  /* Temp for ENC cap */
  #endif
  mcc_fwd_chan_mask_type cur_fwd_chan_mask;          
  mcctc_eom_status_type status = EOM_DONE;  /* Initialize as DONE */
  cdma_dial_type dial;               /* Translated dialing digits */
  mcctctag_tag_info_type tag_info;   /* Tag information */
  boolean sr_id_allocated = msg_ptr->origination.sr_id_included; 
    /* sr_id to use */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
    status = EOM_NO_RXTX_BUF;
  }
  else
  {
    /* Set the EOM message type first */
    cmd_ptr->tc_msg.msg.eom.msg_type = CAI_EOM_MSG;

    /*************************
    * SO related fields here
    **************************/

    so_req = cmd_ptr->tc_msg.msg.eom.service_option = msg_ptr->origination.service_option;

    /* Following are the same process as in Origination Msg */
    if ( cur_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_PC)
    {
      if (cur_bs_ptr->rx.sp_rx && cur_bs_ptr->csp.sp.ext_sys_parameter &&
         cur_bs_ptr->rx.esp_rx && cur_bs_ptr->csp.esp.p_rev >= P_REV_IS95B )
      {
        max_alt_so = MIN(CAI_AC_ALT_SO_MAX, cur_bs_ptr->csp.esp.max_num_alt_so);
      }
      else
      {
        max_alt_so = 0;
      }
    }
    else
    {
      /* On BCCH, this parameter is always included */
      max_alt_so = MIN(CAI_AC_ALT_SO_MAX, cur_bs_ptr->csp.esp.max_num_alt_so);
    }

    /* First, find all the alternate Service Options available.*/
    /* Note here the max_alt_so doesn't govern the number of   */
    /* SOs we might include in the SO_BITMAP. So we find as    */
    /* many alternate SOs as available.                        */
    cmd_ptr->tc_msg.msg.eom.num_alt_so =
    snm_get_alt_so_list(so_req, CAI_AC_ALT_SO_MAX,
                          cmd_ptr->tc_msg.msg.eom.alt_so);

    if (cmd_ptr->tc_msg.msg.eom.num_alt_so > 0)
    {
       /* Now that we've decided what alternative Service Options MS
        will report, we need to convert those SO's to the bitmap to
        save space.

         Following function takes the first three parameters as input,
        fills the next three using them. It also may modify the
        first two parameters num_alt_so and alt_so. If any SO can not
        be represented in bitmap format, it is returned in alt_so
        byte array. Total number of such SO'a are returned in
        num_alt_so.

        As a result all fields related to alternative SO's and
        SO bitmap will be populated in this function. */

       snm_convert_alt_so_bitmap (
        so_req,                                 // primary Service Option
        &cmd_ptr->tc_msg.msg.eom.num_alt_so,    // total number of alt SO's
        cmd_ptr->tc_msg.msg.eom.alt_so,         // byte array that contains
                                                // alternative SO's.
        &cmd_ptr->tc_msg.msg.eom.so_bitmap_ind, // tells the length of the bitmap
        &cmd_ptr->tc_msg.msg.eom.so_group_num,  // SO Group number that main SO
                                                // belongs to.
        &cmd_ptr->tc_msg.msg.eom.so_bitmap      // SO bitmap
        );

      /* We have stuffed as many alt SOs in the SO_BITMAP as allowed */
      /* Now make sure we report the right number of SOs in the      */
      /* alt_so list                                                 */
      if (max_alt_so < cmd_ptr->tc_msg.msg.eom.num_alt_so)
      {
        cmd_ptr->tc_msg.msg.eom.num_alt_so = max_alt_so;
      }
    }
    else
    {
      cmd_ptr->tc_msg.msg.eom.so_bitmap_ind = SO_BITMAP_SIZE_0;
    }

    /*
    ** At this point ALT_SO fields and SO_BITMAP has been set.
    ** Need to set MORE_SO_INCL field.
    */
    if((cmd_ptr->tc_msg.msg.eom.num_alt_so != 0) ||
       (cmd_ptr->tc_msg.msg.eom.so_bitmap_ind != SO_BITMAP_SIZE_0))
    {
      cmd_ptr->tc_msg.msg.eom.more_so_incl = TRUE;
    }
    else
    {
      cmd_ptr->tc_msg.msg.eom.more_so_incl = FALSE;
    }


    /***************************
    * Encryption Fields
    ****************************/

    /* According to IS-2000 RelA A2 std about enc_info_incl field:     */
    /* "The mobile station shall set this field to 1 if it is unable to*/
    /* determine the base station support for encryption. The mobile   */
    /* station shall set this field to 0 if the base station does not  */
    /* support encryption or the mobile station does not support any of*/
    /* the encryption modes supported by the base station."            */
    /* The encryption key is generated at the call setup. If it hasn't */
    /* generated then (eg, AUTH_MODE is off), we can't do encryption.  */
    /* This is true even if MS handoff to a enc-capable BS, So we use  */
    /* the following rule to set the field:                            */
    /* 1. MS doesn't support encryption,                    set to 0   */
    /* 2. MS supports AUTH/CMEA, BS turns AUTH off at call setup,  0   */
    /* 3. MS supports AUTH/CMEA, BS turns AUTH on at call setup,   1   */

#ifdef FEATURE_AUTH
    if (cdma.auth_mode != CAI_NO_AUTH)
    {
      cmd_ptr->tc_msg.msg.eom.enc_info_incl = TRUE;
      mccap_get_encryption_cap_info ( &enc_capability );

      /* User information encryption in Traffic Channel is not supported. */
      cmd_ptr->tc_msg.msg.eom.ui_enc_sup = enc_capability.ui_enc_sup;

      /* User information encryption in Traffic Channel is neither
         requested nor supported. */
      cmd_ptr->tc_msg.msg.eom.ui_enc_req = FALSE;
    }
    else
#endif /* FEATURE_AUTH */
    {
      cmd_ptr->tc_msg.msg.eom.enc_info_incl = FALSE;
    }

    /****************************
    * Emergency Call Fields
    *****************************/
    /* CM has indicated an emergency call.  */
    if (msg_ptr->origination.is_emergency)
    {
      cmd_ptr->tc_msg.msg.eom.global_em_flag = TRUE;

      /* Note that this field is only included in case of an emergency call
         This flag indicates if the MS is to initiate a position location
         determination session with an emergency call. In the current
         design, MS does not start a position location determination session
         with this emergency call, hence this flag will always be zero. */
      cmd_ptr->tc_msg.msg.eom.ms_init_pos_loc_ind = FALSE;
    }
    else
    {
      cmd_ptr->tc_msg.msg.eom.global_em_flag = FALSE;
    }

    /********************************
    * Set CHIND required
    *********************************/

#ifdef FEATURE_IS2000_P2
    /* Get the current CHIND, for now just use forward */
    cur_fwd_chan_mask =  mccccl_code_channel_list.get_fwd_chan_mask();

    /*
    ** We are going to do something simple here for VP1. If we currently have
    ** FCH, then ask for nothing. Otherwise, ask for FCH.
    ** Of course, always ask for FCH is another solution provided that
    ** the BS is smart.
    ** This is an area that we can make it as fancy as possible later.
    */

    if (!(cur_fwd_chan_mask & MCC_F_FCH))
    {
      /* If we not yet have FCH, ask for it */
      cmd_ptr->tc_msg.msg.eom.ch_ind = CAI_CH_IND_FUNDAMENTAL;
    }
    else
#endif //FEATURE_IS2000_P2
    {
      /* If we have FCH,  or P2 is not supported, ask for nothing */
      cmd_ptr->tc_msg.msg.eom.ch_ind = CAI_CH_IND_NO_ADDITIONAL;
    }


    /****************************
    * Data Service Related Fields
    *****************************/

    cmd_ptr->tc_msg.msg.eom.drs = msg_ptr->origination.drs;

    /* Following three fields are only included in case of dormant packet
       data handoff. MS is required to include the previous SID, NID and
       the Packet Zone ID if there has been a change as a result of dormant
       packet data handoff.

       If the MS had successfully originated on a new system (this means it had
       received a Layer 2 ack as a response to a Packet Data Origination),
       old values must have been updated now.

       At this point if these parameters were modified, a dormant packet data
       handoff must have occured. Standard mandates inclusion of the previous
       values in outgoing Origination message if this is a packet data call.
       */

    if (msg_ptr->origination.is_dormant_packet_data)
    {
      /* Start with SID */
      if (mccsyobj_get_sid() != mccsyobj_get_old_pd_orig_sid())
      {
        cmd_ptr->tc_msg.msg.eom.prev_sid_incl = TRUE;
        cmd_ptr->tc_msg.msg.eom.prev_sid = mccsyobj_get_old_pd_orig_sid();
      }
      else
      {
        cmd_ptr->tc_msg.msg.eom.prev_sid_incl = FALSE;
      }

      /* check NID */
      if (mccsyobj_get_nid() != mccsyobj_get_old_pd_orig_nid())
      {
        cmd_ptr->tc_msg.msg.eom.prev_nid_incl = TRUE;
        cmd_ptr->tc_msg.msg.eom.prev_nid = mccsyobj_get_old_pd_orig_nid();
      }
      else
      {
        cmd_ptr->tc_msg.msg.eom.prev_nid_incl = FALSE;
      }

      /* and Packet Zone Id */
      if (mccsyobj_get_pzid() != mccsyobj_get_old_pd_orig_pzid())
      {
        cmd_ptr->tc_msg.msg.eom.prev_pzid_incl = TRUE;
        cmd_ptr->tc_msg.msg.eom.prev_pzid = mccsyobj_get_old_pd_orig_pzid();
      }
      else
      {
        cmd_ptr->tc_msg.msg.eom.prev_pzid_incl = FALSE;
      }
    } /* is dormant packet data */
    else
    {
      cmd_ptr->tc_msg.msg.eom.prev_sid_incl = FALSE;
      cmd_ptr->tc_msg.msg.eom.prev_nid_incl = FALSE;
      cmd_ptr->tc_msg.msg.eom.prev_pzid_incl = FALSE;
    }


    if (msg_ptr->origination.cnt != 0)
    {
      /* Processing dialing digits */
      cmd_ptr->tc_msg.msg.eom.dialed_digs_incl = TRUE;

      /* Translate the digits first */
      xlate_dialed_num( &msg_ptr->origination, &dial );

      /* Copy the translated digits */
      for (i=0; i<dial.num_digits; i++)
      {
        cmd_ptr->tc_msg.msg.eom.chari[i] = dial.digits[i];
      }

      cmd_ptr->tc_msg.msg.eom.num_fields = dial.num_digits;
      cmd_ptr->tc_msg.msg.eom.digit_mode = msg_ptr->origination.digit_mode;

      if (cmd_ptr->tc_msg.msg.eom.digit_mode)
      {
        cmd_ptr->tc_msg.msg.eom.number_type = msg_ptr->origination.number_type;
        cmd_ptr->tc_msg.msg.eom.number_plan = msg_ptr->origination.number_plan;
      }
    }
    else
    {
      /* No dialing digits */
      cmd_ptr->tc_msg.msg.eom.dialed_digs_incl = FALSE;
    }

    /**************************/
    /* Process QoS parameters */
    /**************************/

    /* If Data Services had requested to propose QoS parameters and if
       Base Station allows to propose, Qos Parameters are included */
    if (
        msg_ptr->origination.qos_parms_incl &&
        msg_ptr->origination.is_packet_data &&
        (msg_ptr->origination.qos_parms_len <= CAI_ORIG_QOS_LEN_MAX) &&
        cdma.mob_qos
       )
    {
      caii_qos_parms_type * qos_rec_ptr = (caii_qos_parms_type *) cmd_ptr->tc_msg.msg.eom.recs; //lint !e740 unusual casting

      cmd_ptr->tc_msg.msg.eom.num_recs = 1;

      qos_rec_ptr->hdr.record_type = CAI_QOS_PARAM_REC;
      qos_rec_ptr->qos_parms_len   = msg_ptr->origination.qos_parms_len;

      (void) memcpy
        ( qos_rec_ptr->qos_parms,
          msg_ptr->origination.qos_parms,
          msg_ptr->origination.qos_parms_len
        );
    }
    else
    {
       /* Quality of Service parameters are not included */
       cmd_ptr->tc_msg.msg.eom.num_recs = 0;
    }

#ifdef FEATURE_IS2000_REL_C
    if (P_REV_IS_9_OR_GREATER)
    {
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      /* See if SYNC_ID can be specified */
      cmd_ptr->tc_msg.msg.eom.sync_id_incl = 
        mccsyncid_find_compatible_sync_id(FALSE, so_req, cdma.use_sync_id,
        &sr_id_allocated, &msg_ptr->origination.sr_id,  
        &cmd_ptr->tc_msg.msg.eom.sync_id_len, cmd_ptr->tc_msg.msg.eom.sync_id,
        &cmd_ptr->tc_msg.msg.eom.sr_id);
#else
      cmd_ptr->tc_msg.msg.eom.sync_id_incl = FALSE; 
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
    }
#endif /* FEATURE_IS2000_REL_C */

    /*******************************
    * CCS Related Fields
    ********************************/

    /*
    ** Let's find out what SR_ID to propose. The rule is:
    ** If the call is a data call, use the supplied SR_ID.
    ** Otherwise, get one from the SRID Manager.
    */

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
    if (!cmd_ptr->tc_msg.msg.eom.sync_id_incl)
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
    {
      if (msg_ptr->origination.sr_id_included)
      {
        /*
        ** Let's check if the SR_ID is still reserved for the
        ** same owner.
        */
        if (!mccsrid_check_specific_srid(msg_ptr->origination.sr_id))
        {
          status = EOM_VALIDATION_FAILURE;
          MSG_ERROR("SRID validation failure", 0, 0, 0);
        }
        else
        {
          cmd_ptr->tc_msg.msg.eom.sr_id = msg_ptr->origination.sr_id;
        }
      }
      else
      {
        cmd_ptr->tc_msg.msg.eom.sr_id = mccsrid_allocate_srid(SRID_OWNER_MC);

        if (cmd_ptr->tc_msg.msg.eom.sr_id == MCCSRID_INVALID_SRID)
        {
          status = EOM_INVALID_SRID;
          MSG_ERROR("Invalid SR_ID", 0, 0, 0);
        }
        else
        {
          /* MC allocates SR_ID for UI, but not for DS. DS takes care of
          ** allocation & deallocation.
          */
          sr_id_allocated = TRUE;
          MSG_MED("SRID %d allocated", cmd_ptr->tc_msg.msg.eom.sr_id, 0, 0);
        }
      }
    }

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

    if (status == EOM_DONE)
    {
      // initialize snm's database with the SO and sr_id
      if (!snm_eom_init(cmd_ptr->tc_msg.msg.eom.service_option, 
          cmd_ptr->tc_msg.msg.eom.sr_id))
      {
        status = EOM_SNM_INIT_FAILED;
        MSG_ERROR("SNM EOM init failed", 0, 0, 0);
      }
    } // if (status == ..)

    if (status == EOM_DONE)
    {
      /* Now try to get a new tag for this call */
      tag_info.call_id = msg_ptr->origination.call_id;
      tag_info.sr_id = cmd_ptr->tc_msg.msg.eom.sr_id;
      tag_info.so = cmd_ptr->tc_msg.msg.eom.service_option;
      tag_info.l2_ack_rxed = FALSE;

      if((cmd_ptr->tc_msg.msg.eom.tag = mcctctag_get_new_tag(&tag_info)) ==
         MCCTCTAG_INVALID_TAG)
      {
        status = EOM_INVALID_TAG;
        MSG_ERROR("Out of TAG", 0, 0, 0);

        /* need to tell SNM that the EOM is off! */
        snm_update_for_deleted_connection(cmd_ptr->tc_msg.msg.eom.sr_id);
      }
    }

    /* This checking catches all errors and will release rxtx buffer */
    if (status == EOM_DONE)
    {
      /* Send command to rxtx if nothing is wrong */
      MSG_HIGH("Sent EOM. SO=%d, SRID=%d, TAG=%d",
        cmd_ptr->tc_msg.msg.eom.service_option, cmd_ptr->tc_msg.msg.eom.sr_id,
        cmd_ptr->tc_msg.msg.eom.tag);

      cmd_ptr->tc_msg.ack_req = TRUE;

#ifdef FEATURE_OTASP
      if (msg_ptr->origination.otasp_activation)
      {
        MSG_MED("OTASP activation request received in traffic", 0, 0, 0);
        /* Is it per call instance ??? */
        cdma.otasp_call = USER_INIT_SRV_PROV;

#ifdef FEATURE_RUIM
        /* Set the NAM lock state in the R-UIM */
        otasp_set_ruim_nam_lock_state(USER_INIT_SRV_PROV);
#endif /* FEATURE_RUIM */
      }
#endif /* FEATURE_OTASP */

      /* ------------------------------------------------------------
      ** We need to know when this message is acked to decide whether
      ** the failure is caused by L2 or L3.
      ** ------------------------------------------------------------ */
      mcctc_fill_rxtx_hdr_ack_q ( cmd_ptr );

      rxtx_cmd(cmd_ptr);
    }
    else
    {
      /* Free the rxtx buffer */
      q_put(&rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link);
    }

    /* Deallocate SR_ID if one is allocated and EOM failed */
    if ((status != EOM_DONE) && sr_id_allocated && 
        !msg_ptr->origination.sr_id_included) 
    {
      mccsrid_free_srid(cmd_ptr->tc_msg.msg.eom.sr_id, SRID_OWNER_MC);
      MSG_MED("SRID %d deallocated", cmd_ptr->tc_msg.msg.eom.sr_id, 0, 0);
    }
  }

  return status;

} /* send_eom */

/*===========================================================================

FUNCTION SEND_CLCM

  This function sends an Call Cancel Message.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void send_clcm
(
  byte tag_id,
    /* Tag id for the cancelled EOM */
  mcctctag_tag_info_type *tag_info
    /* Other Information for CLCM */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get buffer from rxtx_mc_free_q to send message in */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->tc_msg.msg.ccm.msg_type = CAI_CLCM_MSG;
    cmd_ptr->tc_msg.ack_req = TRUE;
    cmd_ptr->tc_msg.msg.ccm.tag = tag_id;

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif
    // What to do with this flag?
    //cdma.call_orig = FALSE;  /* Call is mobile originated */

    // What to do with this?
    //cdma.otasp_call = ;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );

    MSG_MED("CLCM sent for tag %d", tag_id, 0, 0);

  }

} /* send_clcm */ //lint !e715 tag_info not being used
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION PROCESS_CLAM

  This function processes a Call Assignment Message.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
word process_clam
(
  caii_call_assignment_type *clam,
    /* CLAM pointer */
  word curr_state
    /* current TC state */
)
{
  int i;
  tc_rej_ord_parms_type rej_parms; /* Reject parameters */
  word next_state = curr_state;    /* Next TC state */
#ifdef FEATURE_IS2000_REL_A_SVD
  mcctctag_tag_info_type tag_info; /* Parameters needed to send Reject Orders */
  mccccim_event_type mccccim_evt;  /* CCIM input/output data structure */
  boolean cci_created = FALSE;     /* Flag whether CCI will be created */
  cm_mc_rpt_type *cm_ptr;          /* pointer of CM buffer */

  /* Temporary hold the list of allocated Call ID */
  uint8 num_call_id = 0;
  cm_call_id_type call_id[CAII_SRV_CON_MAX];

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
  const restore_srv_cfg_info_type *restore_srv_cfg_info = NULL;
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

  if (clam->response_ind)
  {
    if (!mcctctag_get_by_tag_id(clam->tag, &tag_info))
    {
      /* TAG not found, reject the CLAM */
      MSG_HIGH( "CLAM rejected. No matching TAG", 0, 0, 0);
      rej_parms.rej_msg_type = clam->msg_type;
      rej_parms.rej_code = CAI_REJ_CC_TAG_MISMATCH;
      rej_parms.con_ref = clam->con_ref;
      rej_parms.tag = clam->tag;
      send_rej_order(&rej_parms);

      /* Do not notify CM for this case. We will keep waiting... */
      return next_state;
    }

    /* A matching tag is found and taken out of the outstanding list */

    num_call_id = 1;
    call_id[0] = tag_info.call_id;

    if (!clam->accept_ind)
    {
      /* Free the allocated SR_ID first */
      mccsrid_free_srid(tag_info.sr_id, SRID_OWNER_MC);

      /* Inform SNM that it can clean up the EOM negotiation info */
      snm_update_for_deleted_connection(tag_info.sr_id);

      /* Call is not accepted */
      mcc_send_orig_failure(CM_CALL_ORIG_ERR_REJECTED_BY_BS, call_id[0], 
        CM_CALL_ORIG_ERR_LAYER_L3);
      MSG_HIGH("EOM rejected by BS", 0, 0, 0);

      /* Check to see if release criteria are met */
      if ((mcctctag_num_tag_outstanding() == 0) &&
          (mccccim_get_num_ccs_in_non_rel_state() == 0))
      {
        MSG_MED("Go to CDMA_RELEASE", 0, 0, 0);
        next_state = CDMA_RELEASE;  /*lint !e641 */
      }

      return next_state;
    }
   
    /* accept_ind == 1 */

    rej_parms.rej_code = 0;

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
    if (((rej_parms.rej_code = mccsyncid_validate_clam(clam)) == 0) &&
         (!P_REV_IS_9_OR_GREATER || !clam->use_old_serv_config))
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

    {
      if (!clam->con_ref_incl)
      {
        /* Reject the wrong message format */
        MSG_HIGH("CLAM rejected. CON_REF_INCL = 0", 0, 0, 0);
        rej_parms.rej_code = CAI_REJ_FIELD;
      }
      else
      {
        /*
        ** Notice that there is no need to check CON_REF_INCL,
        ** because if accept_ind is '1', BS shall set CON_REF_INCL to '1'.
        ** But we check it anyway in case the BS is crazy.
        */

        if (mccccim_is_con_ref_exist(clam->con_ref))
        {
          /* Duplicated CCI exist, reject the CLAM */
          MSG_HIGH( "CLAM rejected. Duplicated CON_REF", 0, 0, 0);
          rej_parms.rej_code = CAI_REJ_CC_ALREADY_PRESENT;
        }
        else if (clam->so_incl && !snm_so_is_enabled(clam->so))
        {
          /* CLAM SO is invalid, reject the CLAM */
          MSG_HIGH("CLAM rejected. SO %d invalid", clam->so, 0, 0);
          rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
        }
      } /* end if (!con_ref_incl) */
    }

    if (rej_parms.rej_code == 0)
    {

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      if (P_REV_IS_9_OR_GREATER && clam->use_old_serv_config)
      {
        srv_event_type ev;
     
        ev.ec = SRV_CLAM_MSG_EC;
        ev.ep.clam_msg.msg_ptr = clam;

        srv_rpt_event(&ev);

        if (!ev.ep.clam_msg.clam_validation_success)
        {
          /* If service negotiation in progress, reject the CLAM */
          MSG_HIGH("CLAM rejected - service negotiation in progress", 0, 0, 0);
          rej_parms.rej_code = CAI_REJ_STATE;
        }
        else
        {
          /* Create CCI */
          restore_srv_cfg_info = mccsyncid_get_restore_config();

          for (i = restore_srv_cfg_info->cci_con_index; 
               i < restore_srv_cfg_info->cfg.neg_scr.num_con; i++)
          {
            mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
            mccccim_evt.create_cc.con_ref = 
              restore_srv_cfg_info->cfg.neg_scr.con[i].con_ref;
            mccccim_evt.create_cc.is_null_cc = FALSE;   /* Default */
            mccccim_evt.create_cc.bypass_alert_ans = TRUE; /* MS originated */
            mccccim_evt.create_cc.orig_c_required = FALSE;
            mccccim_evt.create_cc.sr_id = 
              restore_srv_cfg_info->cfg.neg_scr.con[i].sr_id;
            mccccim_evt.create_cc.temp_so = 
              restore_srv_cfg_info->cfg.neg_scr.con[i].so;

            if (restore_srv_cfg_info->cfg.neg_scr.con[i].sr_id == tag_info.sr_id) 
            {
              mccccim_evt.create_cc.mob_orig = TRUE; /* MS originated */
              mccccim_evt.create_cc.call_id = call_id[0];
            }
            else if ((call_id[num_call_id++] = cm_call_id_allocate()) == CM_CALL_ID_INVALID)
            {
              /* Failed to allocate call_id, reject the CLAM */
              MSG_ERROR("CLAM rejected. Failed to allocate call_id", 0, 0, 0);
              rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
              break;
            }
            else
            {
              mccccim_evt.create_cc.mob_orig = FALSE; /* Not MS originated */
              mccccim_evt.create_cc.call_id = call_id[num_call_id - 1];
            }

            mccccim_process_event(&mccccim_evt);

            if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
            {
              /* Shouldn't be here */
              rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
              ERR_FATAL("Failed to create CCI", 0, 0, 0);
            }
          } /* end for */
        } /* end if (!ev.ep.clam_msg.clam_validation_success) */
      }
      else
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
      {
        mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
        mccccim_evt.create_cc.call_id = call_id[0];
        mccccim_evt.create_cc.con_ref = clam->con_ref;
        mccccim_evt.create_cc.mob_orig = TRUE;
        mccccim_evt.create_cc.sr_id = tag_info.sr_id;
        mccccim_evt.create_cc.is_null_cc = FALSE; /* Default */
        mccccim_evt.create_cc.bypass_alert_ans = TRUE; /* MS originated */

        if (clam->so_incl)
        {
          mccccim_evt.create_cc.temp_so = clam->so;
        }
        else
        {
          mccccim_evt.create_cc.temp_so = tag_info.so;
        }

        mccccim_evt.create_cc.orig_c_required = FALSE;

        mccccim_process_event(&mccccim_evt);

        if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
        {
          /* Failed to create CCI, reject the CLAM */
          MSG_HIGH("CLAM rejected. Failed to create CCI", 0, 0, 0);
          rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
        }
      }
    }

    if (rej_parms.rej_code != 0)
    {
      rej_parms.rej_msg_type = clam->msg_type;
      rej_parms.rej_record = 0;
      rej_parms.con_ref = clam->con_ref;
      send_rej_order(&rej_parms);

      /* Free SR_ID first */
      mccsrid_free_srid(tag_info.sr_id, SRID_OWNER_MC);

      /* Inform SNM that it can clean up the EOM negotiation info */
      snm_update_for_deleted_connection(tag_info.sr_id);

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      /* Clear MS request for SYNC ID restoration if any */
      mccsyncid_clear_ms_req_info();
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

      /* Notify CM about the EOM failure */
      mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT, call_id[0], 
        CM_CALL_ORIG_ERR_LAYER_L3);

      /* Notice that we've allocated more call_ids. Time to release it */
      if (num_call_id > 1)
      {
        if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cm_ptr->hdr.cmd = CM_CALL_FADE_F;
          cm_ptr->fade.num_call_ids = num_call_id;
          for (i = 1; i < num_call_id; i++)
          {
            cm_ptr->fade.call_ids[i - 1] = call_id[i];
          } 
          mcc_cm_mc_rpt(cm_ptr);
        }
        else
        {
          ERR_FATAL("No buffers on cm_mc_rpt_free_q", 0, 0, 0);
        }
      }
    }
    else
    {
      /* Indicate a CCI has been created */
      cci_created = TRUE;
    }
  }

  /* ------------------------   response_ind == 0 --------------------- */

  else /* response_ind == 0 */
  {
    rej_parms.rej_code = 0;

    /*
    ** Notice that there is no need to check CON_REF_INCL,
    ** because if response_ind is '0', BS shall set CON_REF_INCL to '1'.
    ** But we check it anyway in case the BS is crazy.
    */

    if (!clam->con_ref_incl
#ifdef FEATURE_IS2000_REL_C
        && (!P_REV_IS_9_OR_GREATER || !clam->use_old_serv_config)
#endif /* FEATURE_IS2000_REL_C */
       ) 
    {
      /* Reject the wrong message format */
      MSG_HIGH("CLAM rejected. CON_REF_INCL = 0", 0, 0, 0);
      rej_parms.rej_code = CAI_REJ_FIELD;
    }

    /*
    ** BS is attempting to bring up a SVD call. We need to check if MS
    ** supports CCS or not (to test MS reject BS CLAM cases ).
    ** There is no need to check whether BS supports SVD.
    */
    else if (!mccap_item_is_supported(MCCAP_DF_CS))
    {
      /* Reject CLAM because MS does not support CCS */
      MSG_HIGH("CLAM rejected. CCS not supported by MS", 0, 0, 0);
      rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
    }
    /* cs supported */

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
    else if (((rej_parms.rej_code = mccsyncid_validate_clam(clam)) == 0) &&
              (!P_REV_IS_9_OR_GREATER || !clam->use_old_serv_config))
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

    {
      if (mccccim_is_con_ref_exist(clam->con_ref))
      {
        /* Duplicated CCI exist, reject the CLAM */
        MSG_HIGH("CLAM rejected. Duplicated CON_REF", 0, 0, 0);
        rej_parms.rej_code = CAI_REJ_CC_ALREADY_PRESENT;
      }
      else if (clam->so_incl && (!snm_so_is_enabled(clam->so))) 
      {
        /* CLAM SO is invalid, reject CLAM */
        MSG_HIGH("CLAM rejected. SO %d invalid", clam->so, 0, 0);
        rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
      }
    }

    if (rej_parms.rej_code == 0)
    {
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      if (P_REV_IS_9_OR_GREATER && clam->use_old_serv_config)
      {   
        srv_event_type ev;
     
        ev.ec = SRV_CLAM_MSG_EC;
        ev.ep.clam_msg.msg_ptr = clam;

        srv_rpt_event(&ev);

        if (!ev.ep.clam_msg.clam_validation_success)
        {
          /* If service negotiation in progress, reject the CLAM */
          MSG_HIGH("CLAM rejected - service negotiation in progress", 0, 0, 0);
          rej_parms.rej_code = CAI_REJ_STATE;
        }
        else
        {
          /* Create CCI */
          restore_srv_cfg_info = mccsyncid_get_restore_config();

          for (i = restore_srv_cfg_info->cci_con_index; 
               i < restore_srv_cfg_info->cfg.neg_scr.num_con; i++)
          {
            if ((call_id[num_call_id++] = cm_call_id_allocate()) == CM_CALL_ID_INVALID)
            {
              /* Shouldn't be here */
              rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
              ERR_FATAL("Failed to allocate call_id", 0, 0, 0);
            }  
            else
            {
              mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
              mccccim_evt.create_cc.call_id = call_id[num_call_id - 1];
              mccccim_evt.create_cc.con_ref = 
                restore_srv_cfg_info->cfg.neg_scr.con[i].con_ref;
              mccccim_evt.create_cc.mob_orig = FALSE;     /* Not MS originated */
              mccccim_evt.create_cc.is_null_cc = FALSE;   /* Default */
              if (i == restore_srv_cfg_info->cci_con_index)
              {
                mccccim_evt.create_cc.bypass_alert_ans = 
                  clam->bypass_alert_answer;
              }
              else
              {
                mccccim_evt.create_cc.bypass_alert_ans = TRUE;
              }
              mccccim_evt.create_cc.orig_c_required = FALSE;
              mccccim_evt.create_cc.sr_id = 
                restore_srv_cfg_info->cfg.neg_scr.con[i].sr_id;
              mccccim_evt.create_cc.temp_so = 
                restore_srv_cfg_info->cfg.neg_scr.con[i].so;

              mccccim_process_event(&mccccim_evt);

              if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
              {
                /* Shouldn't be here */
                rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
                ERR_FATAL("Failed to create CCI", 0, 0, 0);
              }
            }
          } /* end for */
        } /* end if (!ev.ep.clam_msg.clam_validation_success) */
      }
      else
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

      {
        if ((call_id[num_call_id++] = cm_call_id_allocate()) == CM_CALL_ID_INVALID)
        {
          /* Failed to allocate call_id, reject the CLAM */
          MSG_ERROR("CLAM rejected. Failed to allocate call_id", 0, 0, 0);
          rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
        }  
        else
        {
          mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
          mccccim_evt.create_cc.call_id = call_id[num_call_id - 1];
          mccccim_evt.create_cc.con_ref = clam->con_ref;
          mccccim_evt.create_cc.mob_orig = FALSE;   /* Not MS originated */
          mccccim_evt.create_cc.is_null_cc = FALSE; /* Default */
          mccccim_evt.create_cc.bypass_alert_ans = clam->bypass_alert_answer;
          mccccim_evt.create_cc.orig_c_required = FALSE;

          /* We don't have SR_ID now. Initialize it to 0 -reserved value */
          mccccim_evt.create_cc.sr_id = 0;

          if (clam->so_incl)
          {
            mccccim_evt.create_cc.temp_so = clam->so;
          }
          else
          {
            mccccim_evt.create_cc.temp_so = CAI_SO_NULL;
          }

          mccccim_process_event(&mccccim_evt);

          if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
          {
            /* Failed to create CCI, reject the CLAM */
            MSG_HIGH("CLAM rejected. Failed to create CCI", 0, 0, 0);
            rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
          }
        }
      }
    }

    if (rej_parms.rej_code != 0)
    {
      rej_parms.rej_msg_type = clam->msg_type;
      rej_parms.rej_record = 0;
      rej_parms.con_ref = 0;
      rej_parms.con_ref = clam->con_ref;
      send_rej_order(&rej_parms);
     
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      /* Clear MS request for SYNC ID restoration if any */
      mccsyncid_clear_ms_req_info();
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

      if (num_call_id > 0)
      {
        /* Notice that we've allocated a call_id. Time to release it */
        if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cm_ptr->hdr.cmd = CM_CALL_FADE_F;
          cm_ptr->fade.num_call_ids = num_call_id;
          for (i = 0; i < num_call_id; i++)
          {
            cm_ptr->fade.call_ids[i] = call_id[i]; /*lint !e644 */
          }
          mcc_cm_mc_rpt(cm_ptr);
        }
        else
        {
          ERR_FATAL("No buffers on cm_mc_rpt_free_q", 0, 0, 0);
        }
      }
    }
    else
    {
      /* Indicate a CCI has been created */
      cci_created = TRUE;
    }
  } /* end if (clam->response_ind) */

  if (cci_created)
  {

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D) 
    if (!P_REV_IS_9_OR_GREATER || !clam->use_old_serv_config)
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
    {
      /*
      ** Check the existing SCR to see if there is a reocrd indentified
      ** with CON_REF.
      */
      if (is_con_ref_in_cur_scr(clam->con_ref))
      {
        /*
        ** We just found a connection record identified by con_ref.
        ** That means service negotiation has already complete.
        ** Just update the CCI with the existing SCR.
        */
        mccccim_evt.update_conrec.event_code = MCCCCIM_UPDATE_CON_RECS;
        mccccim_evt.update_conrec.new_cfg_ptr = snm_get_current_config();
        mccccim_process_event(&mccccim_evt);
      }
      else
      {
        /*
        ** Now we have a CCI for the new con_ref, need to see
        ** what to do with the SO. Notice that we should not
        ** reject this CLAM because we don't like the SO. We
        ** should either send Service Request Msg (if we don't
        ** like it), or do nothing(if we are happy with the SO)
        */
        if (clam->so_incl)
        {
          /*
          ** If SO is included, first check if this so is
          ** compatible with the one that we proposed.
          */
          if (!mccccim_is_clam_so_compatible(clam->so)) 
          {
            /*
            ** If we do not like the SO from the CLAM,
            ** send Service Request.
            */

            /*CCS_SNM
              Need to call an SNM function to send SRQM.
              Looks like we need to define a new event for SNM.

              If SNM is in the state that can send SRQM, it will
              send it. And no more processing is needed here.

              Otherwise, ideally, we should reject the CLAM, and
              delete the CCI. But CCI cannot be undo easily. So
              our current decision is to keep silence. Wait till
              SCM comes and reject that. This should be sufficient
              to cover this rare case.
            */
          }
        } /* end if (clam->so_incl) */
      } /* end if (is_con_ref_in_cur_scr(clam->con_ref)) */
    }
  } /* end if (cci_created) */

#else /* SVD not supported */
  /* Reject CLAM */
  MSG_HIGH("CLAM rejected. CCS unsupported", 0, 0, 0);
  rej_parms.rej_msg_type = clam->msg_type;
  rej_parms.rej_code = CAI_REJ_CAP;
  send_rej_order(&rej_parms);
#endif /* FEATURE_IS2000_REL_A_SVD */

  return next_state;

} /* process_clam */
#endif //FEATURE_IS2000_REL_A

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION PRE_PROCESS_CC_INFO

  This function pre-processes a CC_INFO fields received from UHDM or SCM.

DEPENDENCIES
  The CC fields passed in should be valid.

RETURN VALUE
  Reject reason code.

SIDE EFFECTS
  None

===========================================================================*/
byte pre_process_cc_info
(
  caii_cc_info_type *cc_info,
    /* cc_info pointer */
  tc_rej_ord_parms_type *rej_parms
    /* Rejection parameters */
)
{
  int i, j;             /* loop variables */
  mcctctag_tag_info_type tag_info;    /* tag info */

  /* Default to no rejection */
  rej_parms->rej_code = 0;



  if (cc_info->cc_info_incl)
  {
    if (cc_info->num_calls_assign > mccccim_num_available_cc())
    {
      /*
       ** Don't have enough resource to create CCI.
       */
      rej_parms->rej_code = CAI_REJ_CAP;

      MSG_HIGH("No CCI available!", 0, 0, 0);
      return rej_parms->rej_code;
    }

    for (i=0; i<cc_info->num_calls_assign; i++)
    {
      /* Check to see a conflict con_ref */
      if (mccccim_is_con_ref_exist(cc_info->conn[i].con_ref))
      {
        MSG_HIGH("con_ref %d present, reject msg!", cc_info->conn[i].con_ref, 0, 0);
        rej_parms->rej_code = CAI_REJ_CC_ALREADY_PRESENT;
        rej_parms->con_ref = cc_info->conn[i].con_ref;

        /* used to have break here, but since used return
        ** in the inter loop below for simplicity. So I use
        ** return for consistency
        */
        return rej_parms->rej_code;
      }

      /* CON_REF is OK. Look at RESPONSE_IND */
      if (cc_info->conn[i].response_ind)
      {
        if (!mcctctag_is_tag_outstanding(cc_info->conn[i].tag, &tag_info))
        {
          /* We are not waiting for this tag */
          MSG_HIGH("Wrong tag %d", cc_info->conn[i].tag, 0, 0);
          rej_parms->rej_code = CAI_REJ_CC_TAG_MISMATCH;
          rej_parms->con_ref = cc_info->conn[i].con_ref;
          rej_parms->tag = cc_info->conn[i].tag;

          return rej_parms->rej_code;
        }
      }
      else
      {
        // response_ind set to '0'
        if (!mccap_item_is_supported(MCCAP_DF_CS))
        {
          // We don't support SVD, can't accept the call from BS.
          MSG_HIGH("Don't support SVD, reject con_ref %d",cc_info->conn[i].con_ref,0,0);
          rej_parms->rej_code = CAI_REJ_CALL_ASSIGN_REJ;
          rej_parms->con_ref = cc_info->conn[i].con_ref;
          rej_parms->tag = cc_info->conn[i].tag;
          return rej_parms->rej_code;
        }
      }

      /* Check to see if there is any duplicated con_ref */
      for (j=i+1; j<cc_info->num_calls_assign; j++)
      {
        if (cc_info->conn[i].con_ref == cc_info->conn[j].con_ref)
        {
          /* We found a dup of Con_ref, reject */
          MSG_HIGH("Dup con_ref %d", cc_info->conn[i].con_ref, 0, 0);
          rej_parms->rej_code = CAI_REJ_CALL_ASSIGN_REJ;
          rej_parms->con_ref = cc_info->conn[i].con_ref;

          /* Break is too complicated, let's do it in simple way */
          return rej_parms->rej_code;
        }
      }
    }
  }
  else
  {
    /* Shouldn't even be here */
    ERR_FATAL("BAD CC_INFO", 0, 0, 0);
  }

  return rej_parms->rej_code;

} /* pre_process_cc_info */

/*===========================================================================

FUNCTION PROCESS_CC_INFO

  This function processes a CC_INFO fields received from UHDM or SCM.

DEPENDENCIES
  pre_process_cc_info has been called before this function, and there is
  no other manipulation of CCI in between.

RETURN VALUE
  Reject reason code.

SIDE EFFECTS
  None

===========================================================================*/
void process_cc_info
(
  caii_cc_info_type *cc_info
    /* cc_info pointer */
)
{
  int i;
    /* loop variable */
  mcctctag_tag_info_type tag_info;
    /* tag info */
  mccccim_event_type mccccim_evt;
    /* CCIM input/output data structure */

  if (cc_info->cc_info_incl)
  {
    for (i=0; i<cc_info->num_calls_assign; i++)
    {
      /* CON_REF is OK. Look at RESPONSE_IND */

      if (cc_info->conn[i].response_ind)
      {
        if (mcctctag_get_by_tag_id(cc_info->conn[i].tag, &tag_info))
        {
          /* A matching tag is found and taken out of the outstanding list */

          if (mccccim_is_con_ref_exist(cc_info->conn[i].con_ref))
          {
            /* Shouldn't be here if pre-processing is doing the right thing */
            ERR_FATAL("BAD CON_REF", 0, 0, 0);
          }
          else
          {
            /* Create CCI */
            mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
            mccccim_evt.create_cc.call_id = tag_info.call_id;
            mccccim_evt.create_cc.con_ref = cc_info->conn[i].con_ref;
            mccccim_evt.create_cc.mob_orig = TRUE;
            mccccim_evt.create_cc.sr_id = tag_info.sr_id;
            mccccim_evt.create_cc.is_null_cc = FALSE;      /* Default */
            mccccim_evt.create_cc.bypass_alert_ans = TRUE; /* MS originated */
            mccccim_evt.create_cc.temp_so = tag_info.so;
            mccccim_evt.create_cc.orig_c_required = FALSE;

            mccccim_process_event(&mccccim_evt);

            if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
            {
              /* We should be able to create after pre-processing */
              ERR_FATAL("Cannot create CCI", 0, 0, 0);
            }
          }
        }
        else /* Tag not found */
        {
          /* This shouldn't happen after pre-processing */
          ERR_FATAL("INVALID TAG", 0, 0, 0);
        }
      }
      else
      {
        if (mccccim_is_con_ref_exist(cc_info->conn[i].con_ref))
        {
          /* Shouldn't be here if pre-processing is doing the right thing */
          ERR_FATAL("BAD CON_REF", 0, 0, 0);
        }
        else
        {
          /* Create CCI */
          mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
          mccccim_evt.create_cc.call_id = cm_call_id_allocate();

          if (mccccim_evt.create_cc.call_id == CM_CALL_ID_INVALID)
          {
            /* There is no call_id available. Something must
            ** be wrong, and we cannot do anything about it.
            ** Just log the error and move on.
            ** If bypass_alert_answer is 0, we will timeout on WFO.
            ** If bypass_alert_answer is 1, the call may comeup.
            */
            ERR("No Call ID", 0, 0, 0);
            return;
          }
          else
          {
            mccccim_evt.create_cc.con_ref = cc_info->conn[i].con_ref;
            mccccim_evt.create_cc.mob_orig = FALSE;
            mccccim_evt.create_cc.is_null_cc = FALSE; /* Default */
            mccccim_evt.create_cc.bypass_alert_ans =
              cc_info->conn[i].bypass_alert_answer ;  /* MS originated */
            mccccim_evt.create_cc.orig_c_required = FALSE;
            mccccim_evt.create_cc.sr_id = 0; /* default SR_ID to 0 */
            mccccim_evt.create_cc.temp_so = CAI_SO_NULL; /* default temp_so */

            mccccim_process_event(&mccccim_evt);

            if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
            {
              /* We should be able to create after pre-processing */
              ERR_FATAL("Cannot create CCI", 0, 0, 0);
            }
          }

        }
      }
      /* At the action time of the SCR, the new call connections will
      take place */
    }

  }
  else
  {
    /* Shouldn't even be here */
    ERR_FATAL("BAD CC_INFO", 0, 0, 0);
  }

} /* process_cc_info */

/*===========================================================================

FUNCTION MCCTCSUP_CLEAN_UP_TAGS

  This function clean-up all outstanding tags. It is called before leaving
  Traffic Channel to prevent call_id leak/Stuck in calling.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Outstanding tags are removed.

===========================================================================*/
void mcctcsup_clean_up_tags ( void )
{
  mcctctag_tag_info_type tag_info;
    /* storage for tag info returned by tag manager */
  byte i;
    /* Loop variable */
  cm_mc_rpt_type *cm_ptr;
    /* pointer of CM buffer */
  int num_call_ids = 0;
    /* number of call_ids still in tag manager */
  cm_call_id_type  call_ids[CM_CALL_ID_MAX];
    /* Array to store tags */

  for (i = 0; i<= MAX_TAG_ID; i++)
  {
    /* Loop through all the possible tags */
    if (mcctctag_get_by_tag_id(i, &tag_info))
    {
      /* if this tag happens to be active, remember it */
      if (num_call_ids >= CM_CALL_ID_MAX)
      {  
        //This should be impossible!
        ERR_FATAL("num_call_ids out of range", 0,0,0);
        /* return; */
      }
        
      /* -------------------------------------------------------------
      ** L2 ACK not received, use Orig Fail rpt to inform L2 failure. 
      ** ------------------------------------------------------------- */
      if (!tag_info.l2_ack_rxed)
      {
        MSG_MED("L2 ACK not rxed for call id %d", tag_info.call_id, 0, 0);
        mcc_send_orig_failure(CM_CALL_ORIG_ERR_NO_RESPONSE_FROM_BS,
                              tag_info.call_id,
                              CM_CALL_ORIG_ERR_LAYER_L2);
      }
      /* -------------------------------------------------------------
      ** L2 ACK received, use Fade rpt to inform L3 failure.
      ** ------------------------------------------------------------- */
      else
      {
        call_ids[num_call_ids] = tag_info.call_id;
        num_call_ids++;
      }
    }
  }

  if (num_call_ids > 0)
  {
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cm_ptr->hdr.cmd = CM_CALL_FADE_F;
      cm_ptr->fade.num_call_ids = num_call_ids; //lint !e734 loss of precision

      /* Copy all the call ids */
      for (i = 0; i < num_call_ids; i++)
      {
        cm_ptr->fade.call_ids[i] = call_ids[i]; //lint !e644 call_ids not init. But they are!
      }

      mcc_cm_mc_rpt( cm_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
  }
}

#endif /* FEATURE_IS2000_REL_A_SVD */

#if defined(FEATURE_IS2000_CHS) || defined(FEATURE_IS2000_REL_A)
/*===========================================================================

FUNCTION MCCTCSUP_SEND_RRRM

  This function sends a Resource Release Request Message to the BS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mcctcsup_send_rrrm
(
  boolean gating_disconnect_ind,
    /* Pilot reverse gating or service disconnect indicator */
  byte con_ref,
    /* CON_REF to release */
  boolean purge_service
    /* for PKT data call */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */


  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    ERR( "No free buffers on rxtx_mc_free_q", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->tc_msg.msg.rrrm.msg_type = CAI_RES_REL_REQ_MSG;
    cmd_ptr->tc_msg.ack_req = TRUE;
    cmd_ptr->tc_msg.msg.rrrm.gating_disconnect_ind = gating_disconnect_ind;
    if (!gating_disconnect_ind)
    {
      /* If the service option connection with CON_REF to be released */
      cmd_ptr->tc_msg.msg.rrrm.con_ref = con_ref;
      cmd_ptr->tc_msg.msg.rrrm.purge_service = purge_service;
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif
    }

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
} /* mcctcsup_send_rrrm */
#endif /* FEATURE_IS2000_CHS or FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_B
/*===========================================================================

FUNCTION MCCTCSUP_UPDATE_OFF_TIME_INFO

  This function updates our internal variables based on Off Time information
  received from the base station.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctcsup_update_off_time_info
(
  boolean off_time_supported,
   /* indicates whether or not CDMA Off Time Reporting is enabled */
  byte off_time_threshold_unit,
   /* unit of off_time_threshold */
  byte off_time_threshold
   /* indicates the threshold above which the mobile is to report */
)
{
  if (off_time_supported)
  {
    cdma.off_time_info.cdma_off_time_supported = TRUE;
    /* -------------------------------------------------------------------
    ** Convert the threshold into milliseconds so we can compare it to 
    ** SRCH's estimate.
    ** -------------------------------------------------------------------- */
    if (off_time_threshold_unit == MCC_UNIT_80_MS)  /*lint !e641 */
    {
      cdma.off_time_info.cdma_off_time_threshold = off_time_threshold * 80;
    }
    else if (off_time_threshold_unit == MCC_UNIT_HALF_A_SECOND)    /*lint !e641 */
    {
      cdma.off_time_info.cdma_off_time_threshold = off_time_threshold * 500; /*lint !e734*/
    }
  }
  else
  {
    cdma.off_time_info.cdma_off_time_supported = FALSE;
  }
} /* mcctcsup_update_off_time_info */
#endif /* FEATURE_IS2000_REL_B */

/*===========================================================================

FUNCTION MCCTCSUP_GET_PUBLIC_LONG_CODE_MASK

  This function gets the public long code mask, based on the type of public
  long code mask which is currently in use.

DEPENDENCIES
  Assumes that cdma.long_code fields are set correctly.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mcctcsup_get_public_long_code_mask(qword plcm)
{
#ifdef FEATURE_MEID_SUPPORT
  dword plcm_hi;   /* higher 32 bits of the plcm */
  dword plcm_lo;   /* lower 32 bits of the plcm */

  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_MEID)
  {
    /* The MEID hash is stored in cdma.meidhash 
     */

    /* using PLCM_40 */
    /* bit 38-0 SHA-1 digest of the MEID
     * bit 39 - 0
     * bit 40 - 0
     * bit 41 - 1
     */
    plcm_hi = 0x200L | (cdma.meidhash[3] & 0x7FL); 
    plcm_lo = cdma.meidhash[4]; /*  lower_32_bits_of_meid_39bits_digest */
    qw_set(plcm, plcm_hi, plcm_lo);
  }
  else if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_IMSI_O_M)
  {
    /* using PLCM_37 */
    /* bit 33-0 IMSI_O_S based on IMSI_O_M
     * bit 36-34 - '001'
     * bit 39-37 - '000'
     * bit 40 - 1
     * bit 41 - 1
     */
    plcm_hi = 0x304L | (qw_hi(cdma.imsi_s) & 0x3);  /* include top 2 bits of imsi_s */
    plcm_lo = qw_lo(cdma.imsi_s);     /* lower_32_bits_of_imsi_o_s */
    qw_set(plcm, plcm_hi, plcm_lo);
  }
  else if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_IMSI_O_T)
  {
    /* using PLCM_37 */
    /* bit 33-0 IMSI_O_S based on IMSI_O_T
     * bit 36-34 - '000'
     * bit 39-37 - '000'
     * bit 40 - 1
     * bit 41 - 1
     */
    plcm_hi = 0x300L | (qw_hi(cdma.imsi_s) & 0x3);  /* include top 2 bits of imsi_s */
    plcm_lo = qw_lo(cdma.imsi_s);     /* lower_32_bits_of_imsi_o_s */
    qw_set(plcm, plcm_hi, plcm_lo);
  }
  else
#endif /* FEATURE_MEID_SUPPORT */
#if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_MEID_SUPPORT)
  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_BS_SPEC)
  {
    /* --------------------------------------------------------------------
    ** When using the base station-specified public long code mask, Section
    ** 2.3.6 specifies how to calculate plcm_42 (used by the lower layers)
    ** from plcm_39 (sent by the bs): the mobile appends "101" to plcm_39.
    ** -------------------------------------------------------------------- */
    qw_set(plcm, (0x280L | qw_hi(cdma.long_code.bs_plcm_39)), 
           qw_lo(cdma.long_code.bs_plcm_39));
  }
  else
#endif /* FEATURE_IS2000_REL_C || FEATURE_MEID_SUPPORT */
  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_ESN)
  {
    /* -------------------------------------------------------------------
    ** Section 2.3.6 specifies the mobile must append "1100011000" to the
    ** mobile's permuted ESN.
    ** -------------------------------------------------------------------- */
    qw_set(plcm, 0x318L, permute_esn(cdma.esn));
  }
  else
  {
    /* We reject any OTA message which sets the plcm_type out of range; so if we are
       here, something went wrong in our software.  Reset to ESN-based.  This may
       cause the call to go down; hopefully the mobile will recover in the next call. */
    MSG_ERROR("Unsupported plcm_type=%d, resetting to ESN-based", cdma.long_code.public_lcm_type,0,0);
    cdma.long_code.public_lcm_type = CAI_PLCM_TYPE_ESN;
    qw_set(plcm, 0x318L, permute_esn(cdma.esn));
  }
} /* mcctcsup_get_public_long_code_mask */

/*===========================================================================

FUNCTION MCCTCSUP_DISPLAY_LONG_CODE_MASK

  This function displays the long code mask that is currently being used.

DEPENDENCIES
  Assumes that the cdma.long_code fields are set correctly and that the
  current long code mask is passed in.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mcctcsup_display_long_code_mask(qword plcm)
{
#ifdef FEATURE_AUTH
  if (cdma.long_code.private_lcm_active)
  {
    /* We don't display the private long code mask */
    MSG_HIGH("Using private long code mask",0,0,0);
  }
  else
#endif /* FEATURE_AUTH */
#ifdef FEATURE_MEID_SUPPORT
  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_MEID)
  {
    MSG_HIGH("Using MEID plcm: high=0x%1x, low=0x%1x", qw_hi(plcm), qw_lo(plcm),0);
  }
  else if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_IMSI_O_M)
  {
    MSG_HIGH("Using IMSI_M plcm: high=0x%1x, low=0x%1x", qw_hi(plcm), qw_lo(plcm),0);
  }
  else if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_IMSI_O_T)
  {
    MSG_HIGH("Using IMSI_T plcm: high=0x%1x, low=0x%1x", qw_hi(plcm), qw_lo(plcm),0);
  }
  else
#endif /* FEATURE_MEID_SUPPORT */
#if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_MEID_SUPPORT)
  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_BS_SPEC)
  {
    MSG_HIGH("Using bs-specified plcm: high=0x%1x, low=0x%1x", qw_hi(plcm), qw_lo(plcm),0);
  }
  else
#endif /* FEATURE_IS2000_REL_C || FEATURE_MEID_SUPPORT */
  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_ESN)
  {
    MSG_HIGH("Using ESN-based plcm: high=0x%1x, low=0x%1x", qw_hi(plcm), qw_lo(plcm),0);
  }
} /* mcctcsup_display_long_code_mask */

/*===========================================================================

FUNCTION MCCTCSUP_GET_LONG_CODE_MASK

  This function gets the long code mask which is currently in use.  The
  mobile could be using the private long code mask, or it could be using
  one of the public long code masks.

DEPENDENCIES
  Assumes that cdma.long_code fields are set correctly.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mcctcsup_get_long_code_mask(qword plcm)
{
#ifdef FEATURE_AUTH
  if (cdma.long_code.private_lcm_active)
  {
    auth_get_private_lcm(plcm);
  }
  else
#endif /* FEATURE_AUTH */
  {
    mcctcsup_get_public_long_code_mask(plcm);
  }
  mcctcsup_display_long_code_mask(plcm);
} /* mcctcsup_get_long_code_mask */

#ifdef FEATURE_IS2000_REL_C
/*===========================================================================

FUNCTION PROCESS_RATCHGM

  This function process the rate change message

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void process_ratchgm( caii_rate_change_type *ratchgm )
{
#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif
  tc_rej_ord_parms_type rej_parms;
    
  if (!P_REV_IS_9_OR_GREATER) 
  {
    MSG_HIGH("RATCHGM received in wrong P_REV", 0, 0, 0);
    rej_parms.rej_msg_type = CAI_RATE_CHANGE_MSG;
    rej_parms.rej_code = CAI_REJ_CFG;
    send_rej_order( &rej_parms );
    return;
  }

  /* Should only receive this msg in DV supported build */
  if (!mccap_is_for_pdch_supported()) 
  {
    MSG_HIGH("RATCHGM received in none DV supported build", 0, 0, 0);
    rej_parms.rej_msg_type = CAI_RATE_CHANGE_MSG;
    rej_parms.rej_code = CAI_REJ_CAP;
    send_rej_order( &rej_parms );
    return;
  }

#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV */

  return;

} //lint !e715 ratchgm not referenced unless REL_C_DV is defined
#endif /* FEATURE_IS2000_REL_C */

/*===========================================================================

FUNCTION MCCTCSUP_CHECK_IF_TCH_SRDM_IS_OKAY

DESCRIPTION
  This function validates the service redirection message received on traffic
  channel to see if it can be supported by the MS.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE indicates MS supports the service redirection. Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcctcsup_check_if_tch_srdm_is_okay
(
  caii_ftc_srv_redir_type *serv_redir_ptr
)
{
  /* Check if analog redirection is supported. */
  if ( ( serv_redir_ptr->record_type == CAI_ANALOG_REDIRECTION_REC ) &&
       !( SUPPORTED_OP_MODES & IS95_ANAL_OP_MODE_MASK ) ) //lint !e506 !e774
  {
    MSG_HIGH("Redirecting to unsupported AMPS mode", 0, 0, 0);
    
    return FALSE;
  }
          
  /* Check unsupported TACS redirection. */
  else if ( ( serv_redir_ptr->record_type == CAI_TACS_REDIRECTION_REC) ||
            ( serv_redir_ptr->record_type == CAI_JTACS_REDIRECTION_REC ) )
  {
    MSG_HIGH(" Redirecting to unsupported TACS", 0, 0, 0 );

    return FALSE;
  }

  /* Check if band class of CDMA redirection is supported. */
  else if ( ( serv_redir_ptr->record_type == CAI_CDMA_REDIRECTION_REC ) &&
            !mcc_is_band_class_supported( serv_redir_ptr->redir.rec2.band_class ) )
  {
    MSG_HIGH("Redirecting to unsupported band class %d", serv_redir_ptr->redir.rec2.band_class, 0, 0);

    return FALSE;
  }

  /* Check if redirection type is supported. */ 
  else if ( !P_REV_IS_4_OR_GREATER &&
            ( serv_redir_ptr->redirect_type != CAI_NORMAL_REDIRECTION ) )
  {
    MSG_HIGH("Redirect type %d not supported", serv_redir_ptr->redirect_type, 0, 0);

    return FALSE;
  }
  
  /* Check if NDSS redirection or a NDSS off indication */
  else if ( ( serv_redir_ptr->record_type != CAI_NDSS_OFF_INDICATION ) 
            && ( serv_redir_ptr->redirect_type != CAI_NDSS_REDIRECTION ) )
  {
    MSG_HIGH("Invalid redirect type %d, record type %d on TC",
      serv_redir_ptr->redirect_type, serv_redir_ptr->record_type, 0);

    return FALSE;
  }

  /* Check the record type of NDSS redirection. */
  else if ( ( serv_redir_ptr->redirect_type == CAI_NDSS_REDIRECTION )
            && ( serv_redir_ptr->record_type != CAI_CDMA_REDIRECTION_REC ) 
            && ( serv_redir_ptr->record_type != CAI_ANALOG_REDIRECTION_REC )
            && ( serv_redir_ptr->record_type != CAI_NDSS_OFF_INDICATION ) )
  {
    MSG_HIGH("Invalid record type %d for NDSS redirection",
      serv_redir_ptr->record_type, 0, 0 );

    return FALSE;
  }
      
  return TRUE;
        
} /* mcctcsup_check_if_tch_srdm_is_okay */

/*===========================================================================

FUNCTION MCCTCSUP_INITIALIZE_CP

DESCRIPTION
  This function calls the functions necessary to initialize Call Processing
  for Traffic Channel Initialization state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctcsup_initialize_cp (void)
{
  /* --------------------------------------------------
  ** Initialize handoff in progress indicators to FALSE
  ** -------------------------------------------------- */
  mcctcho_tc_init();

  /* Initialize Call Control Instance Manager */
  mccccim_initialize();

#ifdef FEATURE_IS2000_REL_A_SVD
  mcctctag_initialize();
#endif /* FEATURE_IS2000_REL_A_SVD */

  /* Initialize registration */
  mccreg_tc_init();

#ifdef FEATURE_OTASP
  /* Initialize OTASP */
  otasp_init();
#endif /* FEATURE_OTASP */

#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */

  /* Do some initialization required for the MC transaction engine. */
  mctrans_initialize();

#ifdef FEATURE_IS2000_SCH
  /* Perform initialization required for SCH */
  mccsch_initialize ();
#endif

} /* mcctcsup_initialize_cp */

/*===========================================================================

FUNCTION MCCTCSUP_INIT_SEND_SRCH_TC

DESCRIPTION
  This function tells Receive Task to tune to the Traffic Channel and begin
  sending frame quality metrics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_init_send_srch_tc
(
  mccdma_chnasn_type *chn_assign,
    /* Details about the channel assignment */
  qword long_code_mask

  #if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
  , boolean restore_ind
    /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
    /* Service configuration to be restored */
  #endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
)
{
  int i;

  mcc_fwd_chan_mask_type fwd_chan_mask
    = mccccl_code_channel_list.get_fwd_chan_mask();

#ifdef FEATURE_IS2000_REL_D_DV
#error code not present
#endif /* FEATURE_IS2000_REL_D_DV */

  /* Send a message to Searcher to switch to Traffic Channel */
  mcc_srch_buf.tc.hdr.cmd = SRCH_TC_F;

  /* Send new active set to Searcher and update internal Active Set.
     Note: Remember the num_pilot is the actual num of pilots not num of
     pilots - 1 as definded in IS95B */
  mcc_srch_buf.tc.cdma_freq = cdma.cdmach;
  mcc_srch_buf.tc.band_class = cdma.band_class;
  mcc_srch_buf.tc.aset_cnt  = chn_assign->mode.am0.num_pilots;

  mcc_srch_buf.tc.fpc_pri_chan = 
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
    restore_ind ?  restore_srv_cfg_info->cfg.non_neg_scr.fpc_pri_chan :
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
    chn_assign->mode.am0.fpc_pri_chan;  

  for (i = 0; i < chn_assign->mode.am0.num_pilots && i < SRCH_ASET_MAX; i++)
  {
    mcc_srch_buf.tc.aset[i].pilot = 
      (int2) chn_assign->mode.am0.pilot_rec[i].pilot_pn;

    mcc_srch_buf.tc.aset[i].pwr_comb_ind =
      chn_assign->mode.am0.pilot_rec[i].pwr_comb_ind;

    if (fwd_chan_mask & MCC_F_FCH)
    {
      mcc_srch_buf.tc.aset[i].fch_info.active = TRUE;
      mcc_srch_buf.tc.aset[i].fch_info.walsh_chn =
        chn_assign->mode.am0.pilot_rec[i].code_chan_fch;
      mcc_srch_buf.tc.aset[i].fch_info.qof_mask_id =
        chn_assign->mode.am0.pilot_rec[i].qof_mask_id_fch;

      MSG_MED("TC to SRCH, pn=%d, chan=%d, pwr=%d",
              mcc_srch_buf.tc.aset[i].pilot,
              mcc_srch_buf.tc.aset[i].fch_info.walsh_chn,
              mcc_srch_buf.tc.aset[i].pwr_comb_ind);
    }
    else
    {
      mcc_srch_buf.tc.aset[i].fch_info.active = FALSE;
    }

#ifdef FEATURE_IS2000_P2
    if (fwd_chan_mask & MCC_F_DCCH)
    {
      mcc_srch_buf.tc.aset[i].dcch_info.active = TRUE;
      mcc_srch_buf.tc.aset[i].dcch_info.walsh_chn =
        chn_assign->mode.am0.pilot_rec[i].code_chan_dcch;
      mcc_srch_buf.tc.aset[i].dcch_info.qof_mask_id =
        chn_assign->mode.am0.pilot_rec[i].qof_mask_id_dcch;

      MSG_MED("TC to SRCH, pn=%d, dcch=%d, pwr=%d",
              mcc_srch_buf.tc.aset[i].pilot,
              mcc_srch_buf.tc.aset[i].dcch_info.walsh_chn,
              mcc_srch_buf.tc.aset[i].pwr_comb_ind);
    }
    else
#endif /* FEATURE_IS2000_P2 */
    {
      mcc_srch_buf.tc.aset[i].dcch_info.active = FALSE;
    }

    if (((fwd_chan_mask & MCC_F_PDCH) == 0) &&  
        (!mcc_srch_buf.tc.aset[i].dcch_info.active &&
         !mcc_srch_buf.tc.aset[i].fch_info.active))
    {
      /* Must have some forward channel */
      ERR_FATAL("No forward channels",0,0,0);
    }

#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif

    /* Default pilot rec type value */         
    mcc_srch_buf.tc.aset[i].pilot_rec_type = CAI_PILOT_1X_COMMON;

#ifdef FEATURE_IS2000_REL_A
    if (P_REV_IS_6_OR_GREATER &&
        chn_assign->mode.am0.pilot_rec[i].add_pilot_rec_incl)
    {
      mcc_srch_buf.tc.aset[i].pilot_rec_type = 
        (cai_pilot_rec_type)chn_assign->mode.am0.pilot_rec[i].pilot_rec_type;
      mcc_srch_buf.tc.aset[i].pilot_rec = 
        chn_assign->mode.am0.pilot_rec[i].add_pilot_rec;
    }
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV */

#ifdef FEATURE_IS2000_REL_D_DV
#error code not present
#endif /* FEATURE_IS2000_REL_D_DV */

  } /* end Pilot for loop */

  /* Calculate Traffic Channel Mask */
  qw_set(mcc_srch_buf.tc.pn_mask, qw_hi(long_code_mask), qw_lo(long_code_mask));

  if (fwd_chan_mask & MCC_F_FCH)
  {
    mcc_srch_buf.tc.fch_rc.included = TRUE;
    mcc_srch_buf.tc.fch_rc.rc = 
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.for_fch_rc :
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
      chn_assign->mode.am0.for_rc;  
  }
  else
  {
    mcc_srch_buf.tc.fch_rc.included = FALSE;
  }

#ifdef FEATURE_IS2000_P2
  // Included DCCH if assigned
  if (fwd_chan_mask & MCC_F_DCCH)
  {
    mcc_srch_buf.tc.dcch_rc.included = TRUE;
    mcc_srch_buf.tc.dcch_rc.rc = 
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.for_dcch_rc :
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
      chn_assign->mode.am0.for_rc;  
  }
  else
#endif /* FEATURE_IS2000_P2 */
  {
    mcc_srch_buf.tc.dcch_rc.included = FALSE;
  }

#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV */

#ifdef FEATURE_IS2000_REL_D_DV
#error code not present
#endif /* FEATURE_IS2000_REL_D_DV */

  mcc_srch_cmd(&mcc_srch_buf);
 
} /* mcctcsup_init_send_srch_tc */

/*===========================================================================

FUNCTION MCCTCSUP_INIT_SEND_RXC_TCI

DESCRIPTION
  This function sends TCI command to Receive Task to have it tune to the 
  Traffic Channel and begin sending frame quality metrics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_init_send_rxc_tci
(
  mccdma_chnasn_type *chn_assign
    /* Details about the channel assignment */
  #if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
  , boolean restore_ind
    /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
    /* Service configuration to be restored */
  #endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
)
{
  mcc_fwd_chan_mask_type fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  mcc_rx_buf.tci.hdr.command = RXC_TCI_F;

#ifdef FEATURE_IS2000_REL_A
  mcc_rx_buf.tci.p_rev_in_use = mc_get_p_rev_in_use();
#endif /* FEATURE_IS2000_REL_A */

  mcc_rx_buf.tci.fpc.included = FALSE;

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
  if (restore_ind)
  {
    /* P_REV >= 9, hence FPC parameters must be included in stored
       service configuration */
    mcc_rx_buf.tci.fpc.included = TRUE;
  }
  else
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

  if (P_REV_IS_6_OR_GREATER && ((chn_assign->msg_type == CAI_EXT_CHN_ASN_MSG)
#ifdef FEATURE_MEID_SUPPORT
                                || (chn_assign->msg_type == CAI_MEID_EXT_CHN_ASN_MSG)
#endif
                               ))
  {
    /* ECAM with p_rev >= 6, hence FPC parameters must be included in this
       message */
    mcc_rx_buf.tci.fpc.included = TRUE;
  }

  mcc_rx_buf.tci.fpc.fch_olpc.included = FALSE;

  mcc_rx_buf.tci.for_fch.included = (fwd_chan_mask & MCC_F_FCH) ? TRUE : FALSE;
#ifdef FEATURE_IS2000_P2
  mcc_rx_buf.tci.fpc.dcch_olpc.included = FALSE;
  mcc_rx_buf.tci.for_dcch.included = (fwd_chan_mask & MCC_F_DCCH) ? TRUE : FALSE;

#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV */

  /* Included FCH if assigned */
  if (mcc_rx_buf.tci.for_fch.included)
#endif /* FEATURE_IS2000_P2 */

  {
    mcc_rx_buf.tci.for_fch.rc =  
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.for_fch_rc : 
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
      chn_assign->mode.am0.for_rc;  

    MSG_MED("Going into fwd FCH RC %d", mcc_rx_buf.tci.for_fch.rc, 0, 0);
  }

#ifdef FEATURE_IS2000_P2
  /* Included DCCH if assigned */
  if (mcc_rx_buf.tci.for_dcch.included)
  {
    mcc_rx_buf.tci.for_dcch.rc =  
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.for_dcch_rc : 
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
      chn_assign->mode.am0.for_rc;  

    MSG_MED("Going into fwd DCCH RC %d", mcc_rx_buf.tci.for_dcch.rc, 0, 0);
  }
#endif /* FEATURE_IS2000_P2 */

  /* Temporary */
  mcc_rx_buf.tci.ilpc_enabled = mcc_rx_buf.tci.olpc_enabled =
                                mcc_rx_buf.tci.fpc.included;

  if (mcc_rx_buf.tci.fpc.included)
  {
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
    if (restore_ind)
    {
      mcc_rx_buf.tci.fpc.fpc_mode = (cai_fpc_mode_type)restore_srv_cfg_info->cfg.non_neg_scr.fpc_mode ;
      mcc_rx_buf.tci.fpc.fpc_pri_chan = (cai_fpc_pri_chan_type)restore_srv_cfg_info->cfg.non_neg_scr.fpc_pri_chan;
    }
    else
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

    {
#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV */

      {
        mcc_rx_buf.tci.fpc.fpc_mode = CAI_FPC_MODE_800_PRI; // Default FPC mode

#ifdef FEATURE_IS2000_DV_SIG_TEST
#error code not present
#else
        mcc_rx_buf.tci.fpc.fpc_pri_chan = (cai_fpc_pri_chan_type)chn_assign->mode.am0.fpc_pri_chan;
#endif
      }
    }

    mcc_rx_buf.tci.fpc.fpc_subchan_gain = cdma.fpc_subchan_gain = 
                                          chn_assign->mode.am0.fpc_subchan_gain;

    if (mcc_rx_buf.tci.for_fch.included)
    {
      mcc_rx_buf.tci.fpc.fch_olpc.included = TRUE;
      if (fwd_chan_mask & MCC_F_FCH)
      {
        mcc_rx_buf.tci.fpc.fch_olpc.init_setpt = 
          chn_assign->mode.am0.fpc_fch_init_setpt;
      }
      else
      {
        MSG_ERROR("BS did not specify FCH init setpt", 0, 0, 0);
        mcc_rx_buf.tci.fpc.fch_olpc.init_setpt = 0; 
      }
      mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.set_to_current = FALSE;
      mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.set_to_current = FALSE;

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      if (restore_ind)
      {
        mcc_rx_buf.tci.fpc.fch_olpc.params.target_fer =
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_fch_fer;
        mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.value = 
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_fch_min_setpt;
        mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.value = 
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_fch_max_setpt;
      }
      else
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

      {
        mcc_rx_buf.tci.fpc.fch_olpc.params.target_fer = 
          chn_assign->mode.am0.fpc_fch_fer;
        mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.value = 
          chn_assign->mode.am0.fpc_fch_min_setpt;
        mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.value = 
          chn_assign->mode.am0.fpc_fch_max_setpt;
      }
    }

#ifdef FEATURE_IS2000_P2
    if (mcc_rx_buf.tci.for_dcch.included)
    {
      mcc_rx_buf.tci.fpc.dcch_olpc.included = TRUE;
      if (fwd_chan_mask & MCC_F_DCCH)
      {
        mcc_rx_buf.tci.fpc.dcch_olpc.init_setpt = 
          chn_assign->mode.am0.fpc_dcch_init_setpt;
      }
      else
      {
        MSG_ERROR("BS did not specify DCCH init setpt", 0, 0, 0);
        mcc_rx_buf.tci.fpc.dcch_olpc.init_setpt = 0; 
      }
      mcc_rx_buf.tci.fpc.dcch_olpc.params.max_setpt.set_to_current = FALSE;
      mcc_rx_buf.tci.fpc.dcch_olpc.params.min_setpt.set_to_current = FALSE;

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      if (restore_ind)
      {
        mcc_rx_buf.tci.fpc.dcch_olpc.params.target_fer = 
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_dcch_fer;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.min_setpt.value = 
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_dcch_min_setpt;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.max_setpt.value = 
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_dcch_max_setpt;
      }
      else
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

      {
        mcc_rx_buf.tci.fpc.dcch_olpc.params.target_fer = 
          chn_assign->mode.am0.fpc_dcch_fer;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.min_setpt.value = 
          chn_assign->mode.am0.fpc_dcch_min_setpt;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.max_setpt.value = 
          chn_assign->mode.am0.fpc_dcch_max_setpt;
      }
    }
#endif /* FEATURE_IS2000_P2 */
  }

  if (((RC_CLASS(mcc_rx_buf.tci.for_fch.rc) > 1)
       && mcc_rx_buf.tci.for_fch.included) 
#ifdef FEATURE_IS2000_P2
      || mcc_rx_buf.tci.for_dcch.included  
#endif /* FEATURE_IS2000_P2 */
#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV */
     )
  {
    /* If p_rev is not 6 or greater we will not get here, hence
       cdma.rev_pwr_cntl_delay will always be initialized. */
    mcc_rx_buf.tci.rev_pwr_cntl_delay = cdma.rev_pwr_cntl_delay;
  }
  else
  {
    mcc_rx_buf.tci.rev_pwr_cntl_delay = CAI_DEFAULT_REV_PWR_CNTL_DELAY;
  }

  /* RXC programs fundicated frame offset for Receive side */
  mcc_rx_buf.tci.frame_offset = cdma.frame_offset;

  mcc_rxc_cmd(&mcc_rx_buf);

} /* mcctcsup_init_send_rxc_tci */

/*===========================================================================

FUNCTION MCCTCSUP_INIT_SEND_TXC_TC

DESCRIPTION
  This function sends the TC command to Transmit Task to have it go to 
  Traffic channel which will cause it to start sending Traffic Channel
  preambles.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_init_send_txc_tc
(
  mccdma_chnasn_type *chn_assign,
  /* Details about the channel assignment */
  qword long_code_mask
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
  , boolean restore_ind
  /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
  /* Service configuration to be restored */
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
)
{
  /* Only forward channel TXC cares about is F-PDCH */
  mcc_fwd_chan_mask_type fwd_chan_mask;
  mcc_rev_chan_mask_type rev_chan_mask;

  fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();

  mcc_tx_buf.tc.hdr.command = TXC_TC_F;
  mcc_tx_buf.tc.sigs = MCC_TXC_DONE_SIG;
  mcc_tx_buf.tc.tcb_ptr = &mc_tcb;
  qw_set(mcc_tx_buf.tc.mask, qw_hi(long_code_mask), qw_lo(long_code_mask));

#ifdef FEATURE_IS2000_REL_A
  mcc_tx_buf.tc.p_rev_in_use = mc_get_p_rev_in_use();
#endif /* FEATURE_IS2000_REL_A */

  mcc_tx_buf.tc.signaling_phy_channel = 0;

  mcc_tx_buf.tc.rev_fch.included = (rev_chan_mask & MCC_R_FCH) ? TRUE : FALSE;

#ifdef FEATURE_IS2000_P2
  mcc_tx_buf.tc.rev_dcch.included = (rev_chan_mask & MCC_R_DCCH) ? TRUE : FALSE;
#endif /* FEATURE_IS2000_P2 */

  /* Included FCH if assigned */
  if (mcc_tx_buf.tc.rev_fch.included)
  {
    mcc_tx_buf.tc.rev_fch.rc =  
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.rev_fch_rc : 
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
      chn_assign->mode.am0.rev_rc;  

    MSG_MED("Going into TC rev FCH RC %d", mcc_tx_buf.tc.rev_fch.rc, 0, 0);

    mcc_tx_buf.tc.signaling_phy_channel |= MCTRANS_PHY_CHAN_FCH;
  }

#ifdef FEATURE_IS2000_P2
  /* Included DCCH if assigned */
  if (mcc_tx_buf.tc.rev_dcch.included)
  {
    mcc_tx_buf.tc.rev_dcch.rc =  
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.rev_dcch_rc : 
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
      chn_assign->mode.am0.rev_rc;  

    MSG_MED("Going into TC rev DCCH RC %d", mcc_tx_buf.tc.rev_dcch.rc, 0, 0);

    mcc_tx_buf.tc.signaling_phy_channel |= MCTRANS_PHY_CHAN_DCCH;
  }
#endif /* FEATURE_IS2000_P2 */

  /* TXC programs fundicated frame offset for Transmit side */
  mcc_tx_buf.tc.frame_offset = cdma.frame_offset;

  mcc_tx_buf.tc.rl_gain_adj = 0;
  mcc_tx_buf.tc.rlgain_traffic_pilot = 0;

  if (chn_assign->mode.am0.rev_fch_gating_on && 
      !mccap_item_is_supported(MCCAP_DF_REV_FCH_GATING))
  {
    MSG_HIGH("R-FCH 1/8 gating not supported,ignore req in ECAM", 0, 0, 0);
  }

  if (((RC_CLASS(mcc_tx_buf.tc.rev_fch.rc) == 1) && mcc_tx_buf.tc.rev_fch.included)
      || mcc_tx_buf.tc.rev_dcch.included || (fwd_chan_mask & MCC_F_PDCH))
  {
    mcc_tx_buf.tc.rev_fch_gating_on = FALSE;
  }
  else
  {
    mcc_tx_buf.tc.rev_fch_gating_on = cdma.rev_fch_gating_on ;
  }

  if (P_REV_IS_6_OR_GREATER)
  {
    /* The rlgain_traffic_pilot value in the ESP is a 6-bit 2's
       compliment number stored as a byte, here we are trying to convert
       that into a 8 bit 2's compliment number for the TXC */
    if (cur_bs_ptr->csp.esp.rlgain_traffic_pilot <= 0x1f)
    {
      mcc_tx_buf.tc.rlgain_traffic_pilot = 
        cur_bs_ptr->csp.esp.rlgain_traffic_pilot; //lint !e713
    }
    else
    {
      mcc_tx_buf.tc.rlgain_traffic_pilot =
        cur_bs_ptr->csp.esp.rlgain_traffic_pilot|0xc0; //lint !e734
    }

    /* rl gain adj value is a 4 bit 2's compliment number, convert it into
       a 8 bit 2's compliment number */
    if (chn_assign->mode.am0.rl_gain_adj <= 7)
    {
      mcc_tx_buf.tc.rl_gain_adj = chn_assign->mode.am0.rl_gain_adj;
    }
    else
    {
      mcc_tx_buf.tc.rl_gain_adj = chn_assign->mode.am0.rl_gain_adj | 0xf0;
    }
  }

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

#ifdef FEATURE_IS2000_REL_C_DV
#error code not present
#endif /* FEATURE_IS2000_REL_C_DV */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

  (void) rex_clr_sigs(&mc_tcb, MCC_TXC_DONE_SIG);

  mcc_txc_cmd(&mcc_tx_buf);

  cdma.tc_tx_on = TRUE;
  cdma.tc_tx_to_be_on = TRUE;

} /* mcctcsup_init_send_txc_tc */

/*===========================================================================

FUNCTION MCCTCSUP_INIT_INSTANTIATE_CC

DESCRIPTION
  This function instantiates call control instances.

DEPENDENCIES
  NEGOTIATION_INFO must be initialized (srv_init does it).
  

RETURN VALUE
  TRUE if call control instances are created successfully. 
  Otherwise, return FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcctcsup_init_instantiate_cc
(
  mccdma_chnasn_type *chn_assign,
  /* Details about the channel assignment */
  cm_call_id_type *call_id_ptr
  /* Pointer to allocated Call IDs */
#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
  , uint8 *num_call_id_ptr  /*lint -esym(715, num_call_id_ptr) */
  /* Pointer to the number of allocated Call IDs */
  , boolean restore_ind
  /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
  /* Service configuration to be restored */
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */
)
{
  mccccim_event_type mccccim_evt;
  /* Event rpt struct for sending primitives to CCIM */

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
  /* See whether restore service configuration is needed */
  if (restore_ind)
  {
    int i;

    /* Instantiate the Call Control Instances */
    for (i = restore_srv_cfg_info->cci_con_index; 
        i < restore_srv_cfg_info->cfg.neg_scr.num_con; i++)
    {
      mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
      mccccim_evt.create_cc.sr_id = 
        restore_srv_cfg_info->cfg.neg_scr.con[i].sr_id;
      mccccim_evt.create_cc.con_ref = 
        restore_srv_cfg_info->cfg.neg_scr.con[i].con_ref;
      mccccim_evt.create_cc.temp_so = 
        restore_srv_cfg_info->cfg.neg_scr.con[i].so;
      mccccim_evt.create_cc.is_null_cc = (i == 0) ? TRUE : FALSE;

      if (i == restore_srv_cfg_info->cci_con_index)
      {
        mccccim_evt.create_cc.call_id = call_id_ptr[0]; 
        mccccim_evt.create_cc.mob_orig = cdma.call_orig;
        mccccim_evt.create_cc.orig_c_required = !cdma.call_orig ? FALSE:
                                                (cdma.remaining_digits > 0  ||
                                                 cdma.calling_party_number.num_char > 0  ||
                                                 cdma.calling_party_subaddr.num_char > 0 ||
                                                 cdma.called_party_subaddr.num_char > 0);
      }
      else
      {
        /* Allocate additional call ID */
        if ((call_id_ptr[*num_call_id_ptr] = cm_call_id_allocate()) == CM_CALL_ID_INVALID)
        {
          /* This shouldn't happen for the first call. Power cycle to recover. */
          ERR_FATAL("No Call_ID", 0, 0, 0);
        }
        else
        {
          mccccim_evt.create_cc.call_id = call_id_ptr[*num_call_id_ptr];
          *num_call_id_ptr = *num_call_id_ptr + 1;
        }

        mccccim_evt.create_cc.mob_orig = FALSE;
        mccccim_evt.create_cc.orig_c_required = FALSE;
      }

      if (i == restore_srv_cfg_info->cci_con_index)
      {
        mccccim_evt.create_cc.bypass_alert_ans =
          chn_assign->mode.am0.bypass_alert_answer;
      }
      else
      {
        mccccim_evt.create_cc.bypass_alert_ans = TRUE;
      }

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
  #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

      mccccim_process_event(&mccccim_evt);

      if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
      {
        return FALSE;
      }
    } /* end for */
  }
  else
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

  {
    /* Instantiate the Call Control Instance */
    mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;

    if ( cdma.call_orig
#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */
       )
    {
      /* CCS: Once call_id is in CM interface, get the value
         from where it is stored while processing CM cmd */
      mccccim_evt.create_cc.call_id = call_id_ptr[0];

      /* SR_ID should be set here so CCIM knows what was
         used in origination and can free it in case the BS gives us another
         one for this call instance.  */
#ifdef FEATURE_IS2000_REL_A
      mccccim_evt.create_cc.sr_id = cdma.sr_id;
#else
      mccccim_evt.create_cc.sr_id = CAI_SR_ID_1;
#endif /* FEATURE_IS2000_REL_A */
    }
    else
    {
      mccccim_evt.create_cc.call_id = call_id_ptr[0];
      if (cdma.serv_neg)
      {
        /* sr_id is not valid at this point since we are */
        /* supposed to get a SCM.                        */
        mccccim_evt.create_cc.sr_id = CAI_SR_ID_0;  /*lint !e641 */
      }
      else
      {
        /* Service option negotiation can only have sr_id=1 */
        mccccim_evt.create_cc.sr_id = CAI_SR_ID_1;  /*lint !e641 */
      }
    }

    mccccim_evt.create_cc.mob_orig = cdma.call_orig;
    mccccim_evt.create_cc.con_ref = 0;
    /* For CCS, sr_id is the one we choose for sending in
       Origination message , For MS terminated calls just set
       it to 0 */
    mccccim_evt.create_cc.temp_so = 
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */
      cdma.so_req;
    mccccim_evt.create_cc.is_null_cc = TRUE;
    mccccim_evt.create_cc.bypass_alert_ans =
      chn_assign->mode.am0.bypass_alert_answer;
    mccccim_evt.create_cc.orig_c_required =
      (cdma.remaining_digits > 0  ||
       cdma.calling_party_number.num_char > 0  ||
       cdma.calling_party_subaddr.num_char > 0 ||
       cdma.called_party_subaddr.num_char > 0);

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

    mccccim_process_event(&mccccim_evt);

    if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
    {
      return FALSE;
    }
  } 

  return TRUE;

} /* mcctcsup_init_instantiate_cc */

/*===========================================================================

FUNCTION MCCTCSUP_INIT_CDMA_PARM_FROM_OVHD

DESCRIPTION
  This function initializes CDMA parameters used in traffic channel 
  processing from overheads.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_init_cdma_parm_from_ovhd
(
  void
)
{
  cdma.sdb_supported = cur_bs_ptr->csp.esp.sdb_supported;

#ifdef FEATURE_IS2000_REL_A
  if ( P_REV_IS_7_OR_GREATER )
  {
    cdma.cs_supported = cur_bs_ptr->csp.esp.cs_supported;
    cdma.mob_qos = cur_bs_ptr->csp.esp.mob_qos;
  }
  else
  {
    cdma.cs_supported = FALSE;
    cdma.mob_qos = FALSE;
  }
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A_BSID
  /* Default value for pilot information request supported indicator */
  if (P_REV_IS_7_OR_GREATER && cur_bs_ptr->csp.esp.pilot_info_req_supported)
  {
    cdma.pilot_info_req_supported = TRUE;
  }
  else
  {
    cdma.pilot_info_req_supported = FALSE;
  }
#endif /* FEATURE_IS2000_REL_A_BSID */

#ifdef FEATURE_IS2000_REL_B
  /* ----------------------------------------------------------------------
  ** Initialize CDMA Off Time Reporting variables (based on Overhead Msgs). 
  ** ---------------------------------------------------------------------- */

  #ifdef FEATURE_GPS_MODE
#error code not present
  #endif /* FEATURE_GPS_MODE */
  {
    cdma.off_time_info.cdma_off_time_supported = FALSE;
  }
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_C
  /* The field in ESPM better be valid if p_rev_in_use is REL C */
  cdma.chm_supported = (P_REV_IS_9_OR_GREATER) ? cur_bs_ptr->csp.esp.chm_supported : 1;

  if ( P_REV_IS_9_OR_GREATER )
  {
    cdma.use_sync_id = cur_bs_ptr->csp.esp.use_sync_id;
  }
  else
  {
    cdma.use_sync_id = FALSE;
  }
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

} /* mcctcsup_init_cdma_parm_from_ovhd */

/*===========================================================================

FUNCTION MCCTCSUP_CP_EXIT_PROCESSING

DESCRIPTION
  This function performs the exit processing for CP.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctcsup_cp_exit_processing (void)
{
  cm_mc_rpt_type * cm_ptr;

  rxtx_cmd_type *cmd_ptr;
  /* pointer to command left on mc_rxtx_ack_q */

  cdma_dtmf_type *dtmf_ptr;
  /* pointer to dtmf command to queue for later */

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

  mcctcho_tc_exit();

  mccccim_tc_exit();

#ifdef FEATURE_IS2000_REL_A_SVD
  /* For CCS, send release indication to CM for sig only calls*/

  /* For CCS, Let sr_id manager know that we are leaving TC */
  mccsrid_release();

  /*
  ** Catch all to prevent possible sr_id leak
  */
  mccsrid_free_all_owned_srids(SRID_OWNER_MC);

  /* If there is still some tags, free them all here */
  mcctcsup_clean_up_tags();
#endif /* FEATURE_IS2000_REL_A_SVD */

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
  /* Clear Sync ID call setup variables */
  mccsyncid_clear_call_info();
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

  /* Tell CM that the Traffic Channel is down now */
  if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cm_ptr->hdr.cmd = CM_EXIT_TC_F;
    mcc_cm_mc_rpt( cm_ptr );
  }
  else
  {
    ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }

#if defined(FEATURE_MCLK_DYNAMIC_SPEED_CTRL)
  mclk_switching_throttle(CLK_NORM);
#endif /* FEATURE_MCLK_DYNAMIC_SPEED_CTRL */

  /* clear items off of mcc_rxtx_ack_q */
  while ( (cmd_ptr = (rxtx_cmd_type*) q_get( &mcc_rxtx_ack_q ))!= NULL )
  {
    q_put(&rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
  }

  /* Clear callback timers in case action is pending */
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID );
  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );
  (void) mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );


#ifdef FEATURE_AUTH
  /* -----------------------------------------------------------------
  ** We may be in the middle of doing some authentication calculation,
  ** like an auth signature, which sends a report when it completes.
  ** if this is the case, we wait for that report now to ensure it is
  ** not improperly interpreted later, and to ensure the report buffer
  ** is returned to the right queue.
  ** ----------------------------------------------------------------- */
  mcc_resync_auth();
#endif /* FEATURE_AUTH */

  /* Return pending dtmfs to free queue */
  while ( (dtmf_ptr = (cdma_dtmf_type*) q_get( &mcc_dtmf_q )) != NULL )
  {
    q_put( &mcc_dtmf_free_q, &dtmf_ptr->link );
  }

#ifdef FEATURE_OTASP
  if ( (cdma.otasp_call != NO_SRV_PROV)
  #ifdef FEATURE_OTASP_OTAPA
       || otasp_get_otapa_session()
  #endif /* FEATURE_OTASP_OTAPA */
     )
  {

    #ifdef FEATURE_OTASP_OTAPA
    MSG_MED("TC exit, otasp_call type %d, otapa %d", cdma.otasp_call,
            otasp_get_otapa_session(),0);
    #endif /* FEATURE_OTASP_OTAPA */

    cdma.otasp_call = NO_SRV_PROV;

    /*
     * If were in an OTASP call and we are being handed off to AMPS,
     * we don't want to end the activation until the call is complete.
     * We'll do the OTASP processing at the end of the AMPS call.
     */
    if (MCC_AHO_EXIT_TYPE(mcc_exit.exit))
    {
      MSG_MED("AHO during OTASP/OTAPA", 0,0,0);
      otasp_set_aho_handoff(TRUE);
    }
  }
#endif /* FEATURE_OTASP */

#if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
  /*-----------------------------------------------------------
  ** We are about to leave TC state, check to see if CM task
  ** is still waiting for a L2 ACK from BS. If yes, we send
  ** NAK to CM SMS/PD.
  **-----------------------------------------------------------*/
  mcc_check_mo_dbm_status( CM_DBM_MO_L2_ACK_FAILURE );
#endif /* FEATURE_CDSMS || FEATURE_DBM */

  rxc_reset_voc_init ();

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

} /* mcctcsup_cp_exit_processing */

/*lint +e818 */
