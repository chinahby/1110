/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   C D M A    I D L E    S T A T E

GENERAL DESCRIPTION
  This module contains the state machine for the CDMA Sub-system,
  Idle State.  During this state the mobile processes overhead messages
  received from the base station on the Paging Channel.  The mobile
  also processes requests from the base station and the mobile station
  user that the mobile access the system.

EXTERNALIZED FUNCTIONS
  cdma_idle
    This entrance procedure to the Idle State which determines whether
    slotted or unslotted mode idle is required and calls the appropriate
    idle procedure to handle the idle mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  It is assumed that CDMA system initialization is completed before the
  Idle State is executed.

Copyright (c) 1990-2009 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccidl.c_v   1.104   09 Oct 2002 16:48:24   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccidl.c#7 $ $DateTime: 2009/11/17 04:10:24 $ $Author: sshahi $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
17/11/09   ssh     Added support for processing SRCH_THRESH_R during 
                   sleep sub-states (MCC_SLEEP, MCC_WAIT_FOR_MORE_SLEEP)
09/07/09   pk      Adding specific check for JCDMA phones not to go to IDLE INIT
                   state when the MS does a paging channel hash
09/01/09   ag      Added support for sending neighbor search mode to searcher
06/19/09   pk      Fixing the issue of failure of Idle HO to different freq in 
                   IS95A systems.
06/02/09   ag      Support to display network parameters on UI
06/01/09   ag      Featurized otasp session status under FEATURE_OTASP_STATUS
05/29/09   ag      Added support for sending OTASP session status to CM
04/28/09   ag      Fixed the issue of missing BCSMS when MS transitions from
                   system access to idle state.
02/24/09   ag      Fixed the issue of MS not going to access state after doing
                   AEHO followed by hashing to a different channel.
02/02/09   pk      Handling PCH hashing to process or clear stored overheads 
                   based on whether the overheads are current or not
01/30/09   pk      Enabling powerdown registration to be sent during OTASP
                   commits.
01/28/09   ag      Fixed handling of maximum invalid SPC validation attempts.
01/27/09   pk      Changed the state of the mobile from Slotted to Idle Init
                   if there is a paging channel hashing 
08/12/08   jj      Code changes to avoid loss of paging channel upon T72 expiry
                   in Idle state.
06/26/08   bb      FEATURE_FULL_TIME_SHDR_3 code changes for non-slotted mobiles 
05/16/08   bb      MVS API UPdate feature changes
05/15/08   bb      Code changes for IS707B addendum feature
04/10/08   pg      Fixed the issue that mobile does not respond to Registration
                   Request Order in non-slotted mode when FEATURE_FULL_TIME_SHDR_3
                   is enabled.
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
01/16/08   pg/yll  Added support for FEATURE_FULL_TIME_SHDR_3 so that it
                   allows 1x to do acq on RX0 regardless of the state of DO.
01/08/08   ag      Mainlined FEATURE_LPM
12/14/07   an      Added Passport Mode
12/13/07   sb      Check ddtm only in Idle state, not System Access.
09/10/07   yll     Fixed a problem that MS doesn't receive consecutive and
                   large broadcast messages that take 3 paging channel slots 
                   to send.
06/14/07   an      Added support for White Grey Black Card Operation
06/07/07   pg      Lint clean up.
05/14/07   pg      Do not let srch do QPCH after a failed reacq. 
05/11/07   pg      More Lint Cleanup.fIS707B
04/11/07   pg      Tell TX to go back Idle state in case it got stuck in the 
                   TXC_JUMP_STATE. 
03/19/07   pg      Lint Cleanup.
03/13/07   sb      Corrected action used in mmoc generic cmd processing.
03/09/07   pg      removed dependency on WMS and make sure it only depends on 
                   CM for SMS.
02/23/07   sb      Added support for operational mode feature.
02/14/07   an      Honor GSRDM when ovhd incomplete for 2 cycles
02/03/07   fc      Changes to reference ovhd message's config_msg_seq instead
                   of rx.config_msg_seq.
11/21/06   sb      Added support for VOIP-1X handoffs.
10/25/06   fc      Changes to not use entry reason to indicate TC to Idle
                   transition.
10/19/06   fc      Changes per code review comments.
09/25/06   fc      UMTS changes.
09/19/06   fh      AKA/AES: changed the mechanism to send Security Mode Request
                    Message. Now it is sent under CDMA_MSG rather than 
                    CDMA_ORD_RESP
09/18/06   fc      Fixed the issue of MS missed data burst message for enhanced
                   broadcast.
08/29/06   fh      AKA/AES: cleaned up the usage of AES capability functions
08/16/06   pg      Added support for release D Packet Zone Hysteresis.
08/02/06   fc      Added the SD system exit reason reporting for system lost. 
08/01/06   fc      Merged the following from MSMSHARED:
07/26/06   fh      Removed Locked state check in mccidl_autonomous_wakeup
                   since this check now is moved into function
                   mccreg_idle_check
07/25/06   fc      Fixed featurization.
07/24/06   fh      AKA/AES: changed the AES field processing for MC-RR and ANSI-41
07/17/06   fc      Corrected mcc_srch_lookup_nghbr_info on neighbor type 
                   reporting for Ofreq neigbors. 
07/12/06   fc      Fixed the issue of MS not honoring autonomous registration
                   and continuous filed SRCH_ACCESS_F.
07/11/06   fc      Added the base id reporting and pseudo-idle support for umts. 
07/10/06   fc      Fixed featurization.
07/06/06   fc      Renamed SRCH_LOST_RF_RESOURCE_R to 
                   SRCH_REQUEST_SYS_RESTART_R.
06/23/06   pg      RPC cleanup.
06/12/06   fc      Fixed compilation errors.
06/12/06   fc      Added the support to share the setting of sdb_supported 
                   and mob_qos in idle and traffic state.
06/09/06   fc      Moved all gsrm/egsrm related functions back to mccidl.c.
                   Fixed the data corruption issue of sharing a common data 
                   structure for gsrm/egsrm and srdm processing. 
06/02/06   fc      Cleanup umts interface.
06/01/06   an      Added the bypass registration indicator changes
05/31/06   pg      Changed function mcc_inform_sd_redirection() interface.
                   Release D EGSRM code cleanup.
05/30/06   fc      Fixed lint errors.
05/30/06   fc      Added support of MC command processing in slotted mode.
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/24/06   fc      Fixed the issue of coupling encryption with voice privacy.
05/22/06   fc      Added support for SRCH_LOST_RF_RESOURCE_R.
                   Added support for RXC_PC_FS_TMO_R.
05/22/06   an      Declared two macros for min and max range check
05/19/06   fc      Renamed matched_msg to mccidl_matched_msg.
05/19/06   fh      Merged in new development of AKA/AES features
05/17/06   an      Fixed lint errors.
05/08/06   fc      Changes to store hashed band class in CLM processing.
04/27/06   an      Fixed lint errors
04/24/06   fc      Fixed merge error on UMTS support.
                   Fixed Rel D featurization.
04/17/06   fh      Fixed a bug so that the mobile does not attempt to send Registrations
                   when it is locked.
04/12/06   pg      Added support for release D EGSRM.
04/09/06   fc      Trimmed the size of intermediate generated ECL based on
                   MOB_P_REV,
04/02/06   fh      For AES purpose, saved the BS AES support info int AES status 
                   variables when ANSI-41, MC-RR, and ESPM messages are received
03/27/06   fh      Added AES support
03/21/06   an      Declared the valid_chnlist in ECL a static variable.
03/21/06   fc      Added missing break for MC_END_F command processing.
03/21/06   fc      Reset the entry reason back to MCC_ACQUISITION for TC to 
                   Idle transition.
03/14/06   pg      Added checking for call_id in MC_END_F command.
03/14/06   sb      Added fix for KDDI related to high persistence.
03/03/06   fc      Corrected the issue of NSET being sent before ASET after
                   idle handoff..
02/23/06   fc      Corrected the assignment of nghbr_tx_duration.
02/17/06   fc      Added the support to allow SRCH specifing system exit is 
                   required or not in SRCH_RAFAIL_R.
02/16/06   dna     Avoid HDR doing extra PCCC negotiation.
02/13/06   an      Added ECCLM Cross Band Hashing Support.
02/07/06   fc      Mainlined FEATURE_MC_SUPPORTS_SHDR.
02/07/06   fc      Corrected the reporting of EVENT_REDIRECTION.
01/27/06   fh      Fixed a bug that Powerdown Registration fails when SHDR feature
                   is turned on.
01/23/06   fh      Fixed a bug that IHO was allowed during the transceiver request
                   for registration
01/20/06   fc      Fixed BCCH FER issue on OOS changes.
01/20/06   fc      Merged the following :
           yll     Added support for AKA.
12/23/05   fc      OOS changes.
12/22/05   sb      Replaced cm_mc_rpt_free_q with cm_mc_rpt_get_buf_else_err_fatal().
12/21/05   fc      Fixed the issue of providing incorrect new common channel 
                   configuration to Searcher in the scenario of
                   back-to-back fast RAHOs from expensive HO to cheap HO.  
11/24/05   fc      Changes to delay rescan processing for fast raho.
11/22/05   fh      Fixed an issue so that when transceiver request times out 
                   on report, MC also sends a SRCH_IDLE_F to searcher to bring
                   it from Access State back into Idle state
11/16/05   fc      Fixed the issue of NSET being sent before ASET after HO.
11/11/05   fc      Fixed the issue of MS stuck forever in re-processing 
                   overheads under the scenario of ping pong between two 
                   frequencies or paging channel. 
10/16/05   fc      Added a function named mccidl_in_sleep_state. 
10/15/05   fc      Added support for rescan bit on new common channel.
                   Changes to reprocess stored ovhds after channel hashing.
09/15/05   fc      Fixed the issue of MC mistakenly response to stale access
                   request that was timout on waiting for transceiver 
                   resource.
09/01/05   fc      Fixed the issue of MC not waiting for transceiver resource
                   before changing state to SA for received data burst msg.
08/22/05   grl     Changed criteria for sending search immediate or low 
                   priority access attempts.
08/20/05   fc      Added a function call to let HDR unlock RF when 1x is
                   requesting RF resources for DCA.
07/29/05   fc      Changes to disable MER statistic processing if MUX already
                   incremented their MER counters for received Page Message.
08/01/05   fc      Fixed merge error.
07/29/05   fc      Changes on DCA failure recovery.
07/28/05   fc      Merged the following from MSMSHARED:
07/24/05   fh       Renamed SRCH_FIND_STRONGEST_PILOT_F, SRCH_STRONG_PILOT_F
                    and related report names, data types, and data structures
07/26/05   fc      Fixed lint errors.
07/25/05   fc      Changes for Rel D Addendum f-csh messages.
07/24/05   sb      Added support for tri-band under FEATURE_T53.
07/18/05   pg      Removed unnecessary early jump command from mccidl_check_ovhd().
07/15/05   fc      Fixed lint errors and SHDR II changes for DCA.
07/14/05   fc      Fixed the issue of NSET not being sent if CLM/ECCLM rxed
                   after SPM/A41SPM and NLM/ENLM/GNLM/UNLM.
07/12/05   an      Allowed fast raho to nghbr_config = 2 if paging freq same
07/11/05   fh      cleaned up header file including from MUX
                      changed rxc.h to rxcmc.h
                      changed txc.h to txcmc.h
                      changed FFPC_DEFAULT_FCH_INIT_SETPT_VAL 
                       to function call of ffpc_get_default_fch_init_setpt_val()
07/08/05   fh      Added SHDR phase II support: perform AEHO or Overhead Check
                   after the transceiver resource becomes available; Continue
                   CDMA processing while waiting for transceiver resource           
06/27/05   fc      Changes to use slot mask value from SRCH_WAKEUP_R in
                   RXC_WAKE_F and RXC_SLOT_F.
06/27/05   fh      Fixed a bug when SHDR is turned on. Added MCC_SRCH_Q_SIG
                   into the wait mask when the mobile is not CDMA_IDLE state
                   in the function mccidl_online.
                   When SHDR is turned ON, SRCH_PC_MEAS_R is ignored in AEHO 
                   since it is waiting for SRCH_STRONGEST_PILOT_MEAS_R.
06/17/05   fc      Fixed the issue of MER counters being incremented 
                   incorrectly if multiple GPM received in the assigned slot.
                   Added the suport for MER benchmark in non-slotted mode.
06/06/06   pg      Featurelized call txc_ok_to_sleep() under 
                   FEATURE_RL_JUMP_ENHANCEMENT.
06/03/05   fc      Merged the following from MSMSHARED:
06/02/05   va       Removed RF tunes from MC
06/02/05   fc       Fixed the setting of freq for pilots with neighbor config
                    2.
05/31/05   fc      Merged the following from MSMSHARED:
05/31/05   yll      Fixed the problem that MS used wrong PCH walsh code after
                    fast RAHO with Neighbor config = 1.
05/27/05   fc       Fixed the issue of OTA being occasionally dropped in
                    non-slotted mode.
05/20/05   pg      Removed FEATURE_IS2000_REL_D_JUMP.
05/20/05   pg      Renamed FEATURE_JUMP to FEATURE_IS2000_REL_D_JUMP.
05/20/05   pg      Refined support for early jump to hyperspace under FEATURE_JUMP.
04/27/05   fc      Changes to make MS process ANSI41RAND in case RAND value
                   got changed.
04/25/05   fc      Merged the following from MSMSHARED:
04/20/04   fc       Changes for the update of reg_sid, reg_nid, reg_reg_zone, 
                    auth and rand during AHO/APHO.
04/08/05   fc      Corrected AEHO citeria check.
04/04/05   fc      Merged the following from MSMSHARED:
03/28/05   va       Make slotted timer run run even if we do a system exit
                    so that when we go back to idle we still operate in
                    non-slotted mode till timer is active
03/25/05   an       Changed the Message level from ERROR to HIGH in
                    mcc_srch_lookup_nghbr_info
03/28/05   fc      Added support for DCA.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated 
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/18/05   fc       Backed out changes of OVHD mesaegs proc while going IDLE if 
                    OVHD cfg is okay.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/08/05   yll      Added back sending SRCH_IDLE_F in mccidl_idle_init().
02/09/05   va      Merged following from MSMSHARED Tip:
02/03/05   fc       Fixed lint error.
01/07/05   an       Removed feature FEATURE_SMS and FEATURE_UASMS
01/25/05   sb      Added support for the Fast Call Setup Order.
01/10/05   va      Merged the following from REL_B_C archive:
12/23/04   va        Fixed the issue where processing SPM from wrong cur_bs_ptr
                     after CL hash can cause zone list to be cleared needlessly.
12/22/04   fc        Corrected the issue of stored overheads not being 
                     reprocessed upon re-enter idle for the scenario of MS did
                     not complete the overhead collection for the BS before idle 
                     handoff but idle on that BS upon exit traffic.
12/16/04   an        Added mcc_srch_lookup_nghbr_info()
12/10/04   sb        Fixed lint errors.
12/07/04   an        Process OVHD Messages while going IDLE if OVHD Cfg is okay.
12/06/04   yll       Added the interface change for apiOne.
12/27/04   pg      Added early jump to hyperspace support.
12/08/04   va      Merge the following from REL_B_C archive:
11/22/04   yll       Check the num_bcch_bcast in the MC-RR msg, and only uses
                     6 entries based on the standard change.
11/02/04   fc        Corrected pilot freq used in mcc_entry_init for the 
                     scenario of idle handoff to pilot with neighbor config 2,
                     ngbr_freq is not specified and active pilot freq is
                     different from the first entry specified in CLM/ECCLM.
11/08/04   va      Fixed a compile error
11/05/04   va      Merged following from REL_B_C archive:
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
10/19/04   sb      Clear (E)GSRM pending/ignored flags after Paging Channel 
                   hashing in SPM processing.
10/19/04   fc      Obsolete SRCH_FADE_F interface.
09/27/04   va      Fixed some compiler errors
09/27/04   va      Merged following from REL B/C archive
09/10/04   va       Check for min_p_rev in ESPM and MC-RR mesg
08/19/04   yll      Clean up MC/SRCH interface.
07/29/04   fc       Renamed bcch_index to bcn.
07/23/04   yll      Added changes for Standalone GPS support. Mainlined the
                    FEATURE_INT_ROAMING per Rotem's request.
07/21/04   sj       Added support for umts to 1x handover.
07/20/04   bkm      Update MER support from 7/8 for 6100.
07/08/04   bkm      Distinguish processing a page in the "next" slot after 
                    processing a page in the "assigned" slot from missing the 
                    "assigned" slot and processing only "next" slot to support
                    MER statistics.  No requirement to add this for counters 
                    on telescoping SCI page matches.
07/01/04   va       Corrected lint errors
06/29/04   fc       Included the criteria of overhead message freshness in
                    mcc_srch_nbr_checking.
08/23/04   pg      Added support for new RETRY_TYPE: '100' and '101'.
08/18/04   fc      Removed silk incl flags on overhead messages.
08/17/04   az      Rel D Negative SCI changes
06/30/04   bkm     Lint cleanup - indentation only.
06/16/04   fc      Bundled Aset update with channel config update.
06/14/04   fc      SR_ID enum changes.
06/08/04   bkm     Fixed typos in feature names within comments.
05/21/04   fc      Lint clean up.
05/19/04   sb      Made following merge official via integrate.
05/14/04   sb      Merged the following from msmshared:
                   04/06/04   yll     Include the Non-Primary BCCH in the 
                   chan_config command in RXC and SRCH.
                   03/15/04   yll     Added one more byte in WAKEUP_SLOT event.
                   03/13/04   fc      Removed redundant code.
                   03/05/04   bkm     Lint cleanup.
                   03/04/04   yll     After receiving the RXC_SLOT_R when 
                   monitoring the bcast data slot,  send the chan config to 
                   RXC to disable 2nd BCCH.
03/19/04   va      Merged following from msmshared:
 02/24/04  RC      Added support for FEATURE_INT_ROAMING
03/13/03   fc      Removed redundant code.
03/12/04   fc      Modularized channel supervision.
02/09/04   va      Moved the common part in procesing of stored ovhds 
                   b/w TC to idle transition & idle HO into its own
                   fn
01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
12/16/03   fc      Added support for EVENT_SEG_UPM_ADDR_MISMATCH.
12/09/03   fc      Changes to always update the rx.config_msg_seq for overhead 
                   messages.
11/26/03   fc      Featurized the manipulation of sleep ok variables after HO
                   under non-ZZ2. 
11/25/03   bkm     Lint cleanup.
11/25/03   bkm     Allow T53/JCDMA with BC6.
11/20/03   ph      Removed setting of rp.* in mccidl_proc_acc_parm(), it is now 
                   done in mcc_put_msg() for all PREVs.
11/19/03   fc      Added mcc_srch_get_nbr_config.
11/13/03   yll     Moved the resetting of cdma.call_orig outside the function
                   mcc_send_orig_failure(). Added rejecting the 2nd orig if
                   CP is waiting for Search's pilot reporting.
11/06/03   bkm     Added paging MER counters to support new DIAG cmd.
11/04/03   yll     Changes with srch_tune_rfm() interface.
10/29/03   sst     T_M6500 in last checkin changed to proper T_MSM6500
10/29/03   sst     Surrounded SRCH_HAS_PILOT_GAIN_CNTL with T_M6500
10/24/03   sst     Changed RF tuning calls with srch_tune_rfm(), featurized 
                   as SRCH_HAS_PILOT_GAIN_CNTL
10/23/03   yll     Added support for updating HDR PANID.
10/23/03   bkm     Added support for enhanced Idle and Access HO events.
10/23/03  va       Added reject order sending for SMCM with
                   unsupported fields
10/03/03   fc      Added config_msg_seq update for page message received in
                   update overhead information substate.
10/03/03   yll     Added the Fast RAHO post processing to cancel the bcast
                   scheduling.
10/03/03   bkm     Add match type to EVENT_CP_MATCHED_MSG.
09/30/03   bkm     Changed Ec/Io parameter name in mcc_srch_fast_raho().
09/29/03   bkm     Added Ec/Io parameters to mcc_srch_fast_raho().
09/24/03   sb      Changed MSG_LOW to MSG_MED (when AHO not allowed).
09/10/03   va      Corrected the "=" to "==" in the if condition
09/09/03   va      Added case to ignore pages, if MS is in AEHO state
                   awaiting SRCH report, for an orig mesg.
08/25/03   anr     added changes to support setting base_lat and base_long
                   in BSINFO struct.
08/20/03   yll     Changed FEATURE_HDR_HYBRID to FEATURE_HDR.
08/11/03   bkm     Removed section of code in mccidl_ftm_init() that was 
                   under FEATURE_ENHANCED_STANDY_II.  Code is this section
                   is not needed.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/11/03   az      Added support for FEATURE_MODIFIED_SCI
08/01/03   yll     Added Dynamic Feature support.
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/24/03   fc      Fixed channel supervision for UPM config sequence change. 
07/23/03   bkm     Mainlined FEATURE_ANTIREGISTRATION_BIAS and
                   FEATURE_ENHANCED_STANDBY_III (plus a bit of
                   FEATURE_ENHANCED_STANDBY_II).
07/23/03   bkm     Some more lint cleanup. 
07/16/03   fc      Changes to wait for ANSI41 system parameter message before
                   sending parameters to SRCH.
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
06/26/03   fc      If re-entered update overhead information substate from 
                   system access, report system info to SD after overhead
                   update is completed.
07/03/03   va      Changed mccidl_supervise_channel_monitoring call to conform 
                   to the new prototype.
06/25/03   fc      Changed debug message.
06/18/03   fc      Added the processing for deferred EGSRM on BCCH.
06/02/03   bkm     Lint cleanup. 
06/03/03   jrp     Code review changes for FEATURE_DELAYED_TX_CLOCK_SHUTDOWN.
05/27/03   yll     Changes for simultaneous monitoring of FCCCH and 2nd BCCH.
05/27/03   az      Fixed the reporting of IMSI hash key during RAHO
05/16/03   sj      Inform mccsyobj of sid/nid change only after deciding to stay
                   on current paging channel.
05/13/03   fc      Obsolete CAM for P_REV in use greater than or equal to 8.
05/09/03   fc      Reset state to CDMA_INIT after paging channel assignment.
05/08/03   fc      Removed P_MIN checking for Synch Channel workaround.
04/24/03   bkm     Corrected typo in call to mccidl_check_ovhd().
04/24/03   fc      Fixed ECCLM processing and DB code channel update.
04/15/03   jrp     Added support for FEATURE_DELAYED_TX_CLOCK_SHUTDOWN.
04/14/03   va      Added support for TC to idle transition.
04/07/03   fc      Added the support for simulatenous monitoing BCCH and FCCCH
                   in substate CDMA_OVHD.
04/07/03   bkm     Removed code for obsolete variable cdma_active_set.
04/04/03   fc      Changed aset command payload to support simultaneously
                   monitoring multiple channels.
03/25/03   fc      Corrected feature name.
02/25/03   va      Removed changing MC and SRCH priority by MC.
02/17/03   fc      Modified mccidl_is_egsr_synch_workaround.
02/11/03   fc      Suppressed the channel monitoring debug message upon wakeup
                   if MS is going to exit to SD.
                   Fixed a compiler error.
02/10/03   yll     Changed the debug messages
02/05/03   fc      Corrected the tuning of RF in idle handoff and processing
                   of old handoff report.
                   Cleanup for f-csh forward compatability.
01/24/03   ph      Fix compile error
01/24/03   lh      Modified pzid change processing interface.
01/24/03   fc      Added a debug message for FCCCH hashing.
                   Updated active_f_csch_info in mccidl_send_srch_idle_aset.
01/15/03   fc      Fixed a compiler warning.
01/10/03   fc      Changes for forward compatibility of f-csh.
                   Added support for sync channel workaround.
                   Added support for FEATURE_PAGE_MATCH_REQUEST.
01/09/03   yll     When the sleep is allowed in an idle handoff, set the
                   sleep slot mask also.
12/12/02   hxw     Cleared MCC_L2_ACK_TIMER_SIG properly.
12/02/02   yll     Added event logging for wakeup report, matched msg.
12/02/02   yll     Changed to not send chan config to RXC for Non-pri BCCH,
                   because Search will do that.
11/27/02   fc      Corrected the state change for SID/NID after AHO or APHO.
11/26/02   yll     Fixed a problem when CP doesn't send chan config to RXC.
11/25/02   va      Merged Force dormancy changes from MSM archive.
11/25/02   fc      Fixed compiler errors .
11/25/02   fc      Changes for channel supervision.
11/21/02   fc      Changes for new common channel TD support.
11/19/02   bkm     Added missing break in mccidl_proc_hs_cmd() when MC receives
                   MO DBM req from CM while in UPD_OVHD or AEHO.
11/14/02   sb      Featurized rand parameters under FEATURE_AUTH.
11/13/02   yll     Moved the broadcast schedule info from the sleep command
                   to bc_info_f.
11/07/02   az      Mainlined FEATURE_IS95B
11/07/02   az      Removed the feature TMOBILE
10/31/02   va      Added support for dynamic p_rev changes
11/04/02   fc      Corrected the tuning of RF in MCC_SLEEP state.
                   Marked fast RAHO as expensive idle handoff if TD support
                   changed.
                   Changed channel monitoring debug messages.
10/25/02   yll     Don't send the reject for the broadcast SMS message.
10/24/02   yll     Move several parms from sleep command to parm command.
10/15/02   fc      Enabled standby debug messages.
10/10/02   fc      Changes to make MS picking up APM/EAPM if CP substate is
                   CDMA_IDLE_INIT before MS starts to sleep.
                   Fixed featurization issue.
10/07/02   fc      Added debug messages.
10/02/02   fc      Added more sanity checks in channel monitoring.
10/02/02   az      Mainlined FEATURE_IS2000
09/30/02   sb      Mainlined FEATURE_SD20.
09/27/02   fc      Added debug messages.
09/26/02   fc      Fixed compilation errors if FEATURE_IS2000_REL_A_CC is off.
9/25/02    bkm     Featurized optional IS-2000 slotted timer feature.
09/23/02   fc      If all overhead messages are current except access
                   parameter message and access is pending, change substate
                   to CDMA_OVHD instead of staying in CDMA_IDLE_INIT.
                   This avoids flooding CM with reports which causes out of
                   CM report buffer issue.
09/18/02   fc      Updated channel supervision.
09/06/02   fc      Changed aset update.
09/03/02   fc      Corrected the update of neighbor set by NLM, ENLM and GNLM.
08/30/02   ts      Added support for Run Time R-UIM Enable
08/28/02   fc      Cleaned lint errors.
08/13/02   HQ      Added FEATURE_CDSMS.
08/09/02   fc      Changed debug message.
08/08/02   fc      Fixed access issues caused by changes of correcting MS
                   unnecessary staying awake for changes in APM and EAPM.
08/02/02   fc      Cleared pending broadcast page upon page match.
08/02/02   va      Stored cs_supported field in cdma struct from ANSI-41
                   message.
08/01/02   fc      Added support for the count of the number of powerup
                   registration performed.
                   Corrected the processing of pending broadcast page.
07/30/02   fc      Changes to disable IMSI and TMSI page match for pending
                   broadcast UPM.
07/29/02   fc      Changes to the processing of pending broadcast page to
                   support UPM.
07/26/02   fc      Corrected NID comparsion in SPM and ANSI41 SPM.
07/25/02   fc      Fixed BS insanity timer implementation.
07/10/02   fc/lc   Corrected MS unnecssary staying awake for changes in APM
                   and EAPM.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/01/02   yll     Added support for MC Queue Watermark
06/28/02   lh      SVD code review comment changes.
06/28/02   fc      Changed GPM/UPM slot monitoring to support simultaneous
                   monitoring of non-primary BCCH and FCCCH.
06/27/02   fc      Changes for universal page message slot monitoring.
06/25/02   jrp     Fixed logic error in mccidl_supervise_channel_monitoring
                   for the case of AHO.
06/13/02   yll     Changed the BCSMS feature name to be under Rel A CC.
06/07/02   sj      Added CP support for multimode architecture.
05/22/02   fc      Changes to Universal Page Message slot monitoring.
05/22/02   yll     Saved the enhanced broadcast page when bcast_index was not
                   initialized after power up. Fixed the broadcast_done bit
                   problem in GPM when bcast_index was 0.
05/20/02   fc      Added support for Universal Page Message.
05/10/02   sb      Moved reset of qpch_num into proc_ext_sys_parm.
05/07/02   ph      Fixed compile errors when REL A is turned off.
05/06/02   yll     Changed the Broadcast SMS function names.
04/30/02   sb      Added layer to payload of mcc_send_orig_failure.
04/26/02   sb      Replaced temp array with over-the-air msg in EAPM processing
04/24/02   sb      Added EVENT_ENCRYPTION_FAILURE.
04/09/02   yll     Added Broadcast SMS support on IS2000 RelA common channel.
04/09/02   sb      Added check to ensure EGSRM is sent on pilot beacon.
04/08/02   yll     Fixed a problem that on IS95 Paging channel, the mobile
                   does not go to sleep when it is in multi-slotted broadcast
                   message delivery mode and when the GPM's broadcast_done
                   bit is 0.
04/08/02   sb      Added check to ensure BRAT's are within valid range.
04/05/02   fc      Changed the default return value of
                   mccidl_decode_fccch_frame_size and
                   mccidl_decode_fccch_data_rate.
04/01/02   fc      Added walsh code channel as payload of RXC_CHAN_CONFIG_F.
03/28/02   sb      Added mcc_rand_parms_type.
03/27/02   sb      Added bcch_acquired event.
03/26/02   sb      Removed redundant check of overhead channel in ECCLM processing.
03/25/02   fc      Optimize the channel monitoring mechanism in update overhead
                   info substate.
03/21/02   sb      Ensure we do not loop past supported number of interfaces in UNLM
03/14/02   fc      Changed the function prototype of mccidl_get_bs_index.
                   Changes for fast RAHO.
03/13/02   fc      RelA addendum 2 changes.
03/06/02   sb      No longer incrementing values in EAPM
03/05/02   fc      Corrected the problem of not updating
                   cur_bs_ptr->rx.acc_msg_seq for EAPM and APM.
02/22/02   fc      Added support for Release A common channels.
02/14/02   fc      Merged from 1.95 in MSM archive.
 02/06/02  sh      Added hybird operation support
 01/25/02  sj      Added support for Tiered Services - User Zone feature.
 01/21/02  az      Added a parameter in a call to mcsys_process_pref_sys_change()
 01/15/02  sj      Inform SD if sid or nid is changed in SPM.
 01/12/02  az      Assigned favored_roam_ind variable from cmd queue to the exit
                   reason data structure
 01/08/02  fc      Changes to add debug message and disable the report of new
                   system exit to SD 2.0 for illegal fast RAHO to pilot with
                   neighbor config 3.
02/07/02   lh      Added missing call_ids.
01/17/02   va      Merged from 1.86.1.0 in MSM archive.
  12/17/01 lcc     Removed incorrect check for ext_sys_parameter bit in SPM in
                   function idle_msg_handler.
  12/12/01 abh     Added support for gpsOne factory test.
  12/11/01 sj      Added support for Registration Accepted Order with ordq =
                   roam_indi. Will set the roaming indication on the phone
                   without triggering SD.
12/04/01   lcc     In idle_msg_handler, when ESPM is received, calls proc_ext_sys_parm
                   before PZID notification, which is P_REV dependent.
01/08/02   lh      Remove FEATURE_IS2000_REL_A_SVD from MC-CM interface.
12/21/01   ph      Migrated mcc_l2_ack_timer to the callback timer mechanism.
12/20/01   va      Changed CM_INVALID_CALL_ID to CM_CALL_ID_UNASSIGNED
12/13/01   va      Merged in SVD changes
  11/30/01 va      BS cs_supported flag is now stored in cdma.cs_supported.
  11/21/01 lh      Changes for CM/UI integration.
  08/28/01 lh      Added support for Concurrent Service.
12/06/01   lh      Merged from MSM_CP.05.00.108.
  11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                     mechanism.
  11/30/01   hxw     Bug fix for MC telling SRCH to sleep when MC COMMAND
                     queue is not empty.
  10/26/01   ht      Added code for BS calibration testing.
10/24/01   kk      Merge from common archive.
                   10/18/01   lcc     Corrected problem of attempting to process a CCLM/ECCLM/GSRM/EGSRM
                   when the pending flag is set on an older base station.
                   09/21/01   fc      Fixed compiler error when BROADCAST_SMS is turned off.
10/02/01   jrp     Merged from MSM_CP.05.00.96
                   09/19/01   az      Copied the fwd_link_failure flag from the cmd queue to the
                                      exit reason data structure
09/21/01   fc      Fixed compiler error when BROADCAST_SMS is not defined.
09/06/01   fc      Corrected the override problem of RESCAN exit by the
                   processing of pending broadcast page.
                   Added functions mccidl_get_bcast_slot_cycle,
                   mccidl_is_bcast_init and mccidl_update_sleep_slot_mask.
                   Renamed variables mcc_need_full_wakeup to
                   mccidl_need_full_wakeup, mccdma_bc_init to bcast_init,
                   bc_pending to bcast_pending, bc_slot_cycle to
                   bcast_slot_cycle and mccdma_bc_page to bcast_page.
08/24/01   fc      Corrected new system exit processing in proc_ext_sys_parm().
08/21/01   fc      Added support for Base Station Reject Order.
08/22/01   sb      Proc_ext_sys_parm can now return a state of exit, if bcch is
                   supported.  Fixed range-checking in Enhanced Access Parms.
08/16/01   fc      Merged the following from common archive:
           08/10/01   lcc     Added returning of mccidl_cheap_idle_ho in mcc_srch_fast_raho.
                              This allows SRCH to determine whether to go back to sleep
                              after the RAHO.  Also removed unnecessary calls to
                              send_srch_pc_cmd in proc_cl_msg and proc_ecl_msg.
           08/10/01   fc      Corrected the processing of unsuccessful Access Entry
                              Handoff.
           08/07/01   fc      Corrected SD2.0 #ifdef for T_53.
08/09/01   jrp     Merged the following from the common archive (MSM_CP.05.00.81 to -r1.72)
           08/06/01   fc      Changes to make EGSRM and ECLM processing be consistent.
           07/30/01   fc      Changed MC_MO_DBM_F failure reason to CM_DBM_MO_HOLD_ORIG
                              (SD2.0 only) .
           07/19/01   ych     Modified proc_ext_sys_parm to correctly process extended
                              system parameters message.
           07/11/01   ht      Set default p_rev for BS that does not send ESPM.
           06/18/01   fc      Changes to enable the processing of new origination or SMS
                              request while MS is in update overhead info substate
                              or waiting for access entry handoff report (SD20 only).
08/03/01   fc      Changes to make EGSRM and ECLM processing be consistent.
                   Corrected SD exit processing for Registration Accepted
                   Order with Roaming indication and encryption mode enabled.
07/17/01   fc      Corrected ECLM processing for paging channel.
07/06/01   ph      Added support for Callback Timers
06/26/01   fc      Merged the following from common archive:
           06/18/01   fc      Changes to enable the processing of new
                              origination or SMS request while MS is in update
                              overhead info substate or waiting for access entry
                              handoff report (SD20 only).
           06/11/01   fc      Changes to support the split of fade report
                              between idle and system access substates.
                              Added support for powerup registration of band
                              class, Serv-sys or Freq block switch.
                              Corrected the PREV check for EGSRM.
           05/31/01   lcc     Corrected compilation problem when IS95B is not
                              turned on.
           05/29/01   fc      Changed mcc_inform_sd_redirection function
                              prototype (SD20 only).
           05/25/01   ts      Perform the BS challenge command when processing
                              a SSD Update page.  Removed BS Challenge after
                              SSD update has completed.
                              Converted code for new UIM server.
           05/18/01   lcc     Removed checking of ext_nghbr_list bit when band
                              class is PCS.  This is somewhat redundant and may
                              cause non-BC 1 systems not to work.  Also requires
                              ext_sys_parameter to be set whenever P_REV is 4 or
                              higher rather than only when it's PCS.
           05/14/01   jq      Added ECL num_freq check in proc_ecl_msg().
05/17/01   ph      Merged from MSM_CP.05.00.76
                   05/09/01   lcc     1. When ESPM is received, initialize the p_rev and min_p_rev
                                         fields properly if they are not in the message (P_REV <= 3).
                                      2. In mcc_pc_ho, during idle handoff, if the ESPM of the target
                                        BS has already been received, update the P_REV before we
                                        reprocess all the stored overhead messages.  This prevents
                                       the overhead messages to be cleared in proc_ext_sys_parm which
                                       will result in mobile waking up a lot to collect overhead messages
                                       if the two BSs have different P_REVs.
                   05/09/01   ych     Fixed compiler error when FEATURE_JCDMA is turned ON
                   05/04/01   ych     Enabled SPECIAL_MDR support for base station P_REV greater
                                      than 5
                   05/02/01   lh      Fixed possible mis-alignment in process_pc_streq(),
                                      idle_msg_proc().
                   05/01/01   ych     Modified proc_ext_sys_parm( ) so that Data services P_REV
                                      information is updated.
                   04/17/01   va      Added support for passing on pilot_rec_types received in
                                      GNLM to SRCH.
                   04/16/01   fc      For MS supporting P_REV 6 or above, ignore GSRM with
                                      EXCL_P_REV_MS = 1.
                   04/12/01   fc      Added support of events report: Access handoff, Access
                                      probe handoff, SMS received, Paging channel acquired, TMSI
                                      assigned, System reselection, Lock, Unlock, NDSS off,
                                      Redirection, Registration rejected, Loss of acquisition
                                      after sleep, Power down and Call release request.
                                      Renamed mcc_report_event_cp_state to
                                      mclog_report_event_cp_state and mcc_report_event_timer to
                                      mclog_report_event_timer.
                   04/05/01   fc      If MS is in CDMA_IDLE_INIT substate, ignore RESCAN bit in
                                      system parameter message until all overhead messages are
                                      current. Otherwise, process RESCAN bit in system parameter
                                      message immediately.
                   03/30/01   lh      When we receive fade report from RXC, exit if we are in
                                      IDLE_INIT.
                   03/30/01   fc      For system determination 2.0, ignore RESCAN bit in system
                                      parameter message until all overhead messages are current.
                   03/29/01   rc      Now failing origination with CM_CALL_ORIG_ERR_HOLD_ORIG in
                                      CDMA_IDLE_INIT state (SD 2.0 only).
                   03/28/01   fc      Cleanup code for the report of GSRM to system determination
                                      2.0.
                   03/27/01   rc      Added a parameter to sd_ss_ind_user_ss_pref (SD 2.0 only).
                   02/27/01   jq      Fixed a bug related to channel hashing in ECL msg processing

05/04/01   va      Added initializing ovhd_chan in cur_bs_ptr for idle handoffs.
04/18/01   va      Added passing in pilot rec types from GNLM to SRCH even for
                   Rel 0
03/28/01   lh      Merged following from common archive.
                   03/15/01   fc      Fixed HOLD_ORIG issue in scenario of Fast RAHO and SRCH
                                      missed PC_F in sleep bound.
                                      Fixed REG_TYPE reset problem for ordered registration.
                                      Added processing to wake up SRCH from sleep to speed up
                                      background system measurement for system determination 2.0.
                   03/14/01   va      Moved init of old_p_rev_in_use before p_rev update in
                                      proc_ext_sys_parm()
                   03/14/01   lh      Fixed an MC auto redial bug caused by failing to reset
                                      cdma.call_orig before going back to idle .
                   03/06/01   fc      Added support of events report: Call processing state change,
                                      Registration zone change, Slotted mode operation, QPCH in
                                      use, Idle handoff, New system idle handoff, Rescan and
                                      Timer expired.
                   03/06/01   ht      Fixed a mis-aligned paren under FEATURE_IS95B_NDSS or
                                      FEATURE_T53.
03/07/01   ph      changes to the way mcc_info_rec() is called to reflect new
                   logic in compute_independent_msg_len() of mccsa.c
03/02/01   ph      Merged in from MSM_CP.05.00.55
                        02/27/01   jq      Fixed a bug related to channel hashing in CL msg processing
                        02/26/01   sko     Removed JCDMA feature to poll CDMA_CH by CM.
                        02/23/01   ht      Enclosed bs_p_rev in FEATURE_IS95B to avoid compiler warning.
                        02/21/01   ych     Correctly featurized FEATURE_JCDMA_DS in proc_ext_sys_parm
                        02/16/01   fc      Added support for system determination 2.0. All changes
                                            are featurized under FEATURE_SD20.
                                          Cleanup debug messages.
                        02/16/01   sh      added support for MC_END_F ack
                        02/12/01   lcc     Added validation of service option back to hs_idle_cmd when
                                           an origination is received.  However, the validation is done
                                           only if the SO is not wildcard.
                        02/13/01   ht      In function build_temp_nl(), added code to match freq only
                                           if it is included.
                                           In function mcc_pc_ho(), modify MSG format to make it print
                                            the old pilot during IHO.
                        02/09/01   ks      Making sure the SRCH is in PC state before sending a change
                                           of PC channel by calling send_srch_pc_cmd().
                        01/25/01   ych     Replaced FEATURE_JCDMA with FEATURE_JCDMA_DS in proc_ext_
                                          sys_parm( )
                        01/24/01   ych     Merged T-53/JCDMA features for SK.
                        01/16/01   jq      Made some naming changes to variables and functions. Added
                                          FEAUTURE ifdefs to make it compilable when SCH is undefed
                        01/09/01   va      Made the following fixes:
                                           Changed access to add_pilot_rec_incl and srch_offset_incl in
                                           GNLM under is2000_incl
                                           Fixed the setting of num analog neighbors.
                                           Removed some MSG_ERROR messages which are not relevant anymore.
                        12/19/00   ht      Fix a mis-alligned parenthesis.
                        12/18/00   ks      Calling proc_ecl_rx if ecl_rx is 1 and ext_chan_list field
                                          in SPM is also enabled.  Added ECL support in proc_true_imsi
                                          and during Fast RAHO.
                        12/18/00   ht      In cdma_idle_online(), do not allow idle timeout if
                                          the MS is in AEHO.
                        12/07/00   ht      Add ERR statement when bs_p_rev is undetermined due to
                                          unsupported band class and avoid writing DB with an
                                          arbitrary value.
                        11/29/00   ht       Correct a FEATURE typo.
                        11/28/00   jq      Changed pc_wakeup() interface to include current slot
                                            parameter passing to rxc for non-broadcast SMS targets.
                                          Added QPCH initialization when ESPM is updated.
                        11/28/00   ck      Added support to set the NAM lock state in the R-UIM when an
                                          OTASP Call is originated.
                                           Added the issuing of a Base station Challenge command after
                                           receving the result for an SSD update for R-UIM.
                                           Added ESN parameter to the Update SSD command for R-UIM.
                        11/17/00   ks      Added processing of ECL and EGSRM messages in mcc_pc_ho.
02/26/01   ph      added check for enh. encryption status from parser (rel A).
01/29/01   ph      Bug fix for REL A (Sec Mode Cmd msg now being processed.)
01/10/01   va      Added support for REL A overhead messages on PCH.
                   Added partial support for REL A overhead messages on BCCH.
01/10/01   ph      1. Added processing for Reg Acc. Order (Roam Ind & Encryption)
                      to process_ord_msg().
                   2. Added checking of L2 Encryption fields (filled in by parser)
                      to idle_msg_proc()
                   3. Added processing of Security Mode Command Msg to idle_msg_proc()
11/13/00   lcc     Added initial support for event reports.  Moved update of
                   BS_P_REV and p_rev_in_rev to mcc_update_bs_p_rev (mccsup)
                   and that of SID and NID to process_sid and process_nid
                   (in mccsyobj).
11/03/00   jq      Fixed a sleep problem in BC slot with slotted page.
11/01/00   lcc     Removed so check when MC_ORIGINATION_F is received.  Wildcard
                   SO may be indicate instead of the actual SO.  The check is made
                   in send_origination_msg later on anyway.
10/30/00   ks      proc_cl_msg returns if SPM is not received.
10/27/00   ks      Fixed the ifdef and else combination while checking for
                   nghbr_config = 2 and chaning the primary channel.
10/04/00   jq      Fixed a sci_mask init problem in mcc_sleep_srch_rpt().
10/02/00   yll     Added support for Position Determination DBM.
09/21/00   ks      For idle handoff with nghr_config = 010, and freq_incl=
                   false, MS uses 1st CDMA channel of current Ext Channel
                   list for the freq to tune to, if ECL is sent.
09/20/00   jq      Cleaned all lint errors.
09/08/00   jq      Added QPCH support in sleep state.
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
09/05/00   lh      Make AEHO region processes the same set of msg as in the
                   Update Ovhd Msg substate of System Access State.
08/31/00   ks      Added checking for global_redirect while calling the
                   function proc_gsr() in proc_sys_parm() if GSR is pending.
08/23/00   jq      Fixed a problem with BCSMS init with ext. sys. param. msg.
08/18/00   fc      Added support for retry order.
08/11/00   va      Cleaned up lint errors.
08/04/00   jq      Moved DB sleep status check in start_sleep() to SRCH
07/21/00   jq      Fixed error about unexpected SRCH rpt.
07/14/00   ks      Added support for new Paging channel messages (Extended
                   Channel List Message and Extended Global Service Redir).
                   Added support for Slotted Mode Timer feature.
07/07/00   cah     Non95B & non-Special MDR compiler error fixes to
                   proc_sys_parm() and mcc_get_bs_hash_key().
07/06/00   ks      Added case RXC_PC_T72M_TMO_R: in pc_rx_rpt to declare the
                   Loss of paging channel report if T72M expires in Update
                   Overhead Information substate.
06/26/00   sh      Added AEHO_OVHD_MSG_OK for AEHO OVHD checking
06/15/00   jq      Fixed CR13691. Changed done_monitoring_pc_slot() to move
                   sleep_slot_mask setting code out of ifdef FEATURE_TMSI.
06/14/00   kk      Failure flag set to false in case of System Lost exit
                   in callback idle state.
06/14/00   cah     FEATURE_ERRLOG_DEBUG support will now log Lost Acquisition
                   after sleep, Performing Idle HO and Lost Paging Channel.
06/14/00   kk      Fixed emergency call case in non preferred exit.
06/12/00   jq      Fixed two lint and some BAND_CLASS checking errors
06/04/00   jq      Per code review comments, changed mcc_parse_page()'s
                   parameter to include mccdma_bc_pending.
06/02/00   lcc     For IS2000, moved setting of ack_type in mcc_layer2 to
                   mcc_parse_page.
05/26/00   jq      Added the concept of slot_mask. Convert relative slot
                   schedule to absolute.
05/26/00   cah     Fixed compile warning due to wrong FEATURE_IS95B define.
05/26/00   ck      Added a call to auth_ruim_on as soon as we enter idle state
                   as we may not require the services of RUIM as long as we are
                   in the idle state.
                   Changed the interface for AUTH_SSD_UPDATE_F if FEATURE_RUIM
                   is defined.
05/25/00   ks      Added support for Neighbor Search Window offsets.
04/27/00   jq      Fixed CR13341, modified hs_idle_cmd() to handle
                   MC_BCSMS_ENABLE_F
04/25/00   cah     Ifdef'd vars in mcc_get_bs_hash_key that are only used
                   for FEATURE_IS95B_TRUE_IMSI.
04/25/00   sh      Standby enhancement. No longer check the NID for System
                   Selection purposes. No longer clear the Overhead Messages
                   on a hash to a new frequency.
04/24/00   sh      Get the right hash key before computing the pc_walsh for
                   neighbor config=1. Remove the access parameter checking for
                   overhead info is current or not
04/11/00   lcc     Added update of imsi_hash_key member in bs_info to support
                   QPCH.
04/06/00   va      Merged with MSM3100_CP.03.00.17
04/03/00   jq      Fixed CR 13327: Mobile not waking up 3 slots later. Changed
                   proc_ext_sys_parm() and idle_msg_handler().
03/31/00   ram     In function proc_cl_msg() changed MSG_ERROR to MSG_HIGH when
                   the hashed channel is different than the current channel. Changed
                   some comments in start_sleep() related to fast RAHO for
                   better clarity.
03/27/00   jq      Fixed CR 13312:PP Rel 3.0 has higher awake time resulting in
                   lower battery standby life
03/16/00   jq      Fixed a broadcst SMS initialization bug
03/16/00   jq      Added broadcast SMS feature support.
03/16/00   fc      Merged the following fixes from TB0400 & TB1006.
           vc      Modified proc_ext_sys_parm() to initialize AHO and AEHO
                   fields if not included in the ESPM message.
           vc      Modified cdma_idle_online() to handle the race condition
                   between AEHO and data burst msg processing.
03/14/00   ry      Added OTAPA support
03/06/00   kmp     Modified proc_ext_sys_parm() to remove processing of the
                   mcc=0x3FF and imsi_11_12=0x7F special case. Modified
                   idle_msg_handler() to perform this processing if these
                   values are received in the ESPM. The processing used to
                   be performed in mcc_put_msg() in mccsup.c for non IS-95B
                   systems, and in proc_ext_sys_parm() for IS-95B systems.
03/30/00   ry      Removed the featurization of band class information in
                   mcc_srch_nbr_checking() since FEATURE_SRCH_DYNAMIC_BAND_CLASS
                   will always be on
03/29/00   ry      Featurized the nbr_band parameter of mcc_srch_nbr_checking()
                   under FEATURE_SRCH_DYNAMIC_BAND_CLASS
03/23/00   ry      Added IS2000 featurization
           ry      Merged the following from PP (MSM3100_CP.03.00.10):
02/14/00   cah     (ajn) Modified sleep_ok time to get the timestamp from the
                                   report from search.
02/11/00   ram     Added a function mcc_get_bs_p_rev() to determine the P_REV
                   of a given base station using the stored base station info
                   from either the extended system parameters message or the
                   sync channel message. Modified mcc_srch_min_bs_p_rev() to
                   use this function. Earlier, mcc_srch_min_bs_p_rev() used only
                   the sync channel message to find the P_REV. This resulted
                   in the function returning a value of 0 when we've never
                   received the sync channel message in the current base station.
                   Also, added a band_class check in mcc_srch_min_bs_p_rev().
02/04/00   cah     Conditional compile out send_srch_parms_flag if not 95B
                                        in start_sleep().
02/03/00   vn      Do not call cmd_done() from inside idle_msg_handler() and
                   call it in the calling function instead - CR12770.
02/02/00   ht      (evanmell)Modify start_sleep() to call send_srch_parms()
                   and send_srch_neighs() only once when needed.
                   CR12837: Skip checking for reselection timeout if MS
                   is updating overhead info and is in call origination.
01/21/00   vn      CR12776: Added a function I/F to read gsr_ignored.
01/17/99   ks      Modified proc_ext_sys_parm() to check MCCr=3FF &
                   IMSI_11_12=7F after True IMSI processing.
12/28/99   ks      Modified mcc_pc_ho to not process Channel List message
                   after a successful AHO or APHO to a known base station.01/14/00   ht      Record the last active CDMA channel after idle handoff.
01/14/00   ht      Record the last active CDMA channel after idle handoff.
01/07/00   ram     While processing the General Neighbor List Message, in the
                   function set_nghbr_band_freq the freq_incl field for
                   for each of the neighbor records was not being set. This
                   resulted in incorrect values being sent to srch, and
                   Inter-band and Inter-frequency idle-handoffs were failing.
                   This has now been fixed and the correct freq_incl value is
                   sent to srch.
12/15/99   ks      Added function to return minimum P_REV of all known BS's
                   this is for ENHANCED_STANDBY_III feature.
           vn      Set roaming ind. only after processing pending GSRM.
                   ych     Added support for Trimode Silent Redial
                   ht      Add call to mcsyspr_set_last_cdma after rf_tune_to_chan.
                   vn      - Additional checks for band_class in build_temp_nl(),
                                     get_neigh_index() and mcc_pc_ho().
                                   - Service Indicator turned on after processing
                                         pending GSRM in idle_state().
                                   - Check for supported bands before passing neighbors
                                         to Searcher.
                   ck      Added band class parameter to SRCH_FADE_F report.
                   ck      Added band class parameter to the following functions
                     - mcc_srch_nbr_checking()
                     - mcc_pc_ho()
                     - mcc_get_nghbr_index()
                     - mcc_get_nbrcfg1_pc_walsh()
                     - mcc_entry_info()
03/23/00   va      Aligned start of info rec structs on dword boundaries.
02/13/00   va      Addendum Changes.
02/01/00   va      Fixed CR#12232 completely. Commands from CM are now queued
                   up when the MS is waiting for AEHO rpt from SRCH.
01/26/00   va      Merged the GV40401 changes.
11/15/99   kmp     Fixed a problem in hs_idle_cmd() where a check of the
                   service negotiation manager was not correct handled. It
                   later led to an error fatal when originating an MDR call.
11/11/99   kk      Fixed a flag initialization problem dring GSRM processing.
11/02/99   kmp     Fix in idle_state() changed FEATRUE_IS95B to FEATURE_IS95B.
11/15/99   va      IS2000 Voice Changes.
                   Support for processing enhanced overhead messages.
11/03/99   lcc     Merged from GD4021:

10/27/99   doj     Modified PACKET_ZONE_ID reporting such that it will now
                   report for IS-95B as well as for FEATURE_SPECIAL_MDR.
10/22/99   ks      Modified code to remove FEATURE_ENHANCED_STANDBY_II
                   & FEATURE_ENHANCED_STANDBY features. Moved the ACTION_TIMER
                   & DTMF TIMER signal checking at the beginning of mcc_wait
                   in cdma_idle_online().
           lh      Serving System update for CM is added.
10/26/99   lcc     Moved PLT related code to plt_mc.c.
09/16/99   lcc     Merged in PLT support from PLT archive:
                   1. Checked PLT state instead of command status in plt_idle().
                   2. Moved the code that set Idle state for PLT.
                   3. Added support for PLT.
09/09/99   fc      Fixed the idle_msg_handler() to store the Access Parameter
                   Msg Sequence Number if Access Parameter Msg is a duplicate
                   and MS is in Update Overhead Info State, In doing so, MS
                   can proceed with access in case of Access Parameter MSg is
                   received ahead of General Page Msg.
09/01/99   lh      Fixed compiling problem without FEATURE_UASMS defined.
08/26/99   lh      Added release reason to report to CM. Fix for CR6158.
08/25/99   ks      Added support to process SRCH_THRESH_R SRCH report in
                   mcc_waking_up_srch_rpt(). Also added a condition for AEHO.
08/24/99   fc      Fixed the bogus pilot reporting, by requesting on SRCH to
                   provide the count of pilots in Pilot Strength Measurement
                   report.
08/21/99   ks      Added P_REV_IN_USE checking for AEHO during Origination.
08/21/99   ks      Fixed the Pilot reporting inconsistency, by waiting for SRCH
                   to finish searching all neighbors before Accessing BS.
08/19/99   lh      Add reject SMS at exit point.
08/13/99   lh      CM and SMS phase 2 implementations.
08/06/99   doj     Replaced constants in sign extension of add_intercept
                   and drop_intercept with CAI masks.
07/07/99   ht      Add code to populate data for New System Exit.
07/28/99   lh      CM and two-way SMS support added.
07/28/99   ks      Removed the FEATURE_ENHANCED_STANDBY_II in mcc_pc_ho where
                   it was used for testing the feature.
07/27/99   fc      Removed an excessive variable i from set_access_ho_info().
07/22/99   fc      Changes in Pilot Reporting to support Access Probe Handoff.
07/20/99   abh     Moved the code to invalidate the Access Message Sequence number
                   after AHO or APHO outside the FEATURE_ENHANCED_STANDBY_II.
07/19/99   nmn     Removed a check to change service option IS-96A to IS-96.
07/19/99   abh     Changes for Access Probe Handoff
07/14/99   lcc     Added calls to SID change handler in proc_sys_parm and to
                   PZID change handler in idle_msg_handler, which were moved
                   from mcc_put_msg.  SID change handler will now be called
                   even if the stored SPM is used.
06/30/99   kmp     In pc_rx_rpt() fixed an &d to a %d in an ERR() call.
06/24/99   ks      Forcing Access Parameters msg_seq to INVALID during AHO or
                   APHO, this makes the mobile to have latest ACC Parms.
06/21/99   kmp     Merged in the two changes listed below.
           ks      Added processing for Registration Access Order with
                   Enhanced Roaming Indication included in process_ord_msg().
           sh      Added sign extension for add_intercept and drop_intercept
                   in proc_ext_sys_parm()
06/18/99   ram     Merged the following fix made by wjn in the SS archive -
           wjn     Modified the code so that even under a bad network
                   configuration, the mobile will not be hashed to a different
                   CDMA channel while mcc_srch_fast_raho() function is being
                   called.
05/10/99   kmp     Merged the following IS-95B related changes.
           ks      The Overhead messages are processed in System Access sub
                   state during Access Handoff. Modified code to reflect this.
05/04/99   kmp     Merged all of the IS-95B changes below.
           kk      Service Redirection with NDSS feature processing added.
           kmp     Removed extra P_REV check when sending srch parameters in
                   proc_ext_sys_parm().
           ks      Added the processing of setting_srch_win field during GNLM
                   processing. Also added processing of srch_priority and freq
                   band during ENLM processing.
           kmp     Merged the changes listed below from the TGP IS-95B baseline.
           jq      Modified process_chn_asn due to code review
           fc      Modified SEND_SRCH_NEIGHS() to include ACCESS_ATTEMPTED as
                   part of information for sending to searcher.
           ks      Setting Channel List message as received in case of AEHO or
                   AHO. MC calls start_sleep only if its in CDMA_IDLE_HO state.
           kmp     merged in IS-95B changes, ported IS-95B changes to ARM,
                   removed FEATURE_IS95B where possible and replaced with
                   P_REV checking, but left FEATURE_IS95B as a comment
                   Merged in changes for FEATURE_ENHANCED_STANDBY_II into
                   IS-95B code. Made no changes to FEATURE_ENHANCED_STANDBY_II
                   only software since this feature is not supported in IS-95B
                   at this time.
                   Replaced all occcurences of T_AUTH, T_SMS, and T_ACP with the
                   applicable FEATURE name.
                   Following are the IS-95B additions:
           fc      Renamed plt_rpt_bookeep to pilot_rpt_ovhd_rec
           jq      Modified idle_msg_handle(), idle_msg_proc(),
                   pending_idle_msg_proc() to support CAM/ECAM
           lh      Authentication using IMSI_M.
           fc      Added the update logic of PLT_RPT_BOOKEEP.PREV_PILOT in
                   BUILD_TEMP_NL.
           fc      Added support for pilot reporting.
           ks      Modified AEHO feature to ACCESS_ENTRY_HANDOFF and optimized
                   AEHO code.
           ks      Added support for IS95B Access Handoff.
           ks      Clearing the HO state after AEHO or exit during AEHO.
           sh      Changed send_srch_parms to mcc_send_srch_parms
           ks      Added code for Access Entry HO Feature and also modified
                   function pc_idle_ho to mcc_pc_ho.
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           kmp     Added support for Phased Deployment in functions
                   idle_msg_proc(), and idle_msg_handler()
           sh      Added IS96B soft handoff support
           lh      Added i.
           kk      Added the threshold notification processing from Search.
           fc      Added IS-95B Extended Status Response Message processing.
           ks      Updating the db for all values of P_REV in ESPM. Also added
                   a new macro to check the status of Nghbr List Messages.
           lh      Applied necessary run-time P_REV checking to IMSI_T.
                   Changed soft_slop to soft_slope.
           ks      Added IS-95B Overhead Message processing and run-time P_REV
                   and Band Class checking. Also modularized the processing
                   of various Neighbor List messages (NLM,ENLM & GNLM).
           lh      IS-95B True IMSI implementation added.
04/08/99   dna     Merged the two changes below.
03/12/99   ht      Add condition to fully wake up when rudely awakened
                   but srch actually never slept due to off frequency
                   searching(rhughes)
03/10/99   br      Debug messages for MER analysis set to MSG_MED level.
02/19/99   lh      Fixed GSRM got remembered and processed after CLM hashing.

03/02/99   ejv     Added wjn's data call origination fix.
03/01/99   ejv     Merged all the FEATURE_ENHANCED_STANDBY_II and FEATURE_
                   ANTIREGISTRATION_BIAS changes.  These changes are quite
                   extensive and detailed.
02/12/99   lcc     Added checking for disabled service options when originating.
11/30/98   dna     Merged Paging channel debug message suppport in
                   idle_page_match () function (merged from ASW 1.16)
11/24/98   br      Added changes for storing new NAM.
11/20/98   ht      Add FEATURE_ENHANCED_STANDBY label for code already
                   checked in.
09/21/98   ht      Process Global Service Redir Message before setting
                   digital icon if gsr_ignored is set.
09/14/98   gb      After Idle HO, do not clear msg seq, and do allow sleep.
08/24/98   dna     Retain message encryption on CDMA->AMPS handoff by
                   separating auth_reset from auth_resync.
08/24/98   pms     Fixed an = vs == bug, commented fix.
08/20/98   ck      Now using voc_capability_supported interface.
08/04/98   ych     Merged SSPR, E911 and Acquisition improvements.
07/09/98   lcc     Fixed compilation error when FEATURE_DS is not defined.
07/06/98   pms     Provided better (working) fix for below change.
06/26/98   pms     Fixed bug where UI was not informed when DS ended a call.
06/25/98   kis     Added code for ASCII digit mode call origination.
06/17/98   wjn     Added Feature E911 Support.
05/11/98   na      Fixed CRs 5174, 4337, 5118. (1) MC was not sending
                   CM_DATACALL_F in the initialization state. (2) MC was not
                   processing MC_DATA_END_F in the idle state.
03/20/98   abh     Review comments for PRL Force Mode
02/02/98   abh     Merged changes for PRL Forced Mode
01/27/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/27/98   ych     Keep track of the idle timeout with clk_uptime() instead
                   of a rex timer. Also added #include "customer.h" inorder
                   to facilitate for DMSS migration of target.h to customer.h
12/02/97   dna     Fixed CR 4466.  MC can no longer get stuck taking all CPU
                   from lower priority tasks and cause a watchdog timeout.
10/10/97   abh     changes so that after global redirection to analog, phone
                   doesn't get stuck in analog system forever.
08/22/97   dna     Fixed bug where changing acc_msg_seq could stop phone from
                   sleeping if general page with new sequence number was
                   received before the access parameters message.
08/18/97   ych     Extended support for the all the targets to ignore a global
                   service redirection until all the over head messages are
                   received. Previously, only targets with FEATURE_PREFERRED_
                   ROAMING defined used to do this.
08/06/97   jrs     For preferred roaming targets, set the roaming indicator
                   after the system has been selected instead of during
                   System Parameter Message processing.
08/05/97   lh      Added code for buffer leak prevention.
07/30/97   lh      Let system determination decide if a redirection is valid.
07/16/97   lh      Added logging out-of-buffer errors.
07/15/97   dna     Reject unknown data bursts, modularize GSR processing,
                   overhead message processing improvements.
07/01/97   rdh     Eliminated T_HO_DEBUG, made dependent upon TMOBILE instead.
06/16/97   dna     Set hold_orig when returning to idle from update ovrhd
06/07/97   dna     Only mark GSR not rxed if we will later mark it as rxed.
06/02/97   day     Declare variables as static to be compliant with ANSI C
                   standard.
05/20/97   ck      Added support for Low Power Mode
05/08/97   jca     Fixed GSR msg powerdown bug (SWIS #1941).
04/09/97   lh      Add ifdef FEATURE_JSTD008 for dual band phone.
03/20/97   lh      Delete OTASP_ORIG_RXED processing.
01/25/97   dna     More flexible reject of status request.
12/23/96   smo     Added Pilot Sniffer functionality.
12/13/96   gb      Fixed storing of active set win size from paging channel.
12/13/96   jca     Added force_amps capability for MC_ORIGINATION_F commands.
11/07/96   dna     Make sure cdma.otasp_call gets set to FALSE when necessary.
10/17/96   jjw     Bug fix for AMPS originations from DS task
10/11/96   dna     Build temp neighbor list after idle handoff.
10/11/96   jjw     Added AMPS data task initiated call capability.
10/07/96   dna     Fixed "stuck powering down" bug.
10/07/96   jrs     For Preferred Roaming, process an ignored Global Service
                   Redirection Message after a system has been selected.
09/26/96   dna     Range check JSTD008 sys parms, do MCC_ORIG_CMD_RXED exit
                   for orig during CDMA_IDLE_INIT, new OTASP orig processing.
09/13/96   dna     Added processing for OTASP activation origination, etc.
09/11/96   jca     Fixed DTMF decoding bug.
09/05/96   jrs     For Preferred Roaming, ignore Global Service Redirection
                   and Service Redirection until mcsys has selected a system.
08/28/96   dna     set cdma.sa_state for MCC_ORIGINATION entry to allow
                   chan assign modes 1 & 5 to work properly with RESPOND = 1.
08/28/96   jpf     idle_state() uses entry block's S.O., checks for 0, then
                   checks for mcc_voc_capability.
08/27/96   jrs     Retrieve the acquired SID/NID from the System Parameters
                   Message instead of the Sync Channel Message.
07/30/96   jgr     Added new sms buffers system, changes are ifdef NEW_SMS.
07/10/96   day     Add input parameter to identify page/non-page messages
                   when calling mcc_detect_duplicate.
06/27/96   jrs     Added the MCC_NOT_PREFERRED exit code.
06/24/96   jrs     Added support for Preferred Roaming.
06/17/96   dna     Handle special cases for status request/response.
05/15/96   dna/so  Support for other freq neighbor handoff in PCS.
05/04/96   rdh     Made debug handoff parms #ifdef T_HO_DEBUG to save ROM space.
05/04/96   rdh     Removed references to PN autoswitching.
04/24/96   dna     Send RXC_WAKE_F if srch returns TOOLATE status, so RXC
                   is sure to know we are operating in slotted mode.
04/15/96   jca     Modified code to use regionally declared NV command
                   buffers instead of stack variables to avoid potential
                   memory overwrite problems.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
04/09/96   jca     SRCH is now always informed when a Channel List Message
                   causes the MS to hash to a new Paging Channel frequency.
04/02/96   jca     Added base station insanity timer for CDMA_IDLE_INIT state.
03/21/96   dna     Send ack_type 2 for response to general page.
03/13/96   day     Check assign mode validity.  Using FEATURE_JSTD008 ifdef.
03/13/96   day     Added service negotiation check.  Using FEATURE_JSTD008.
03/13/96   day     Added PCS band class check.  Using FEATURE_JSTD008 ifdef.
03/13/96   day     Added J-STD-008 PCS message modifications.
                   Using FEATURE_JSTD008 ifdef.
02/14/96   rdh     Added stricter checking for priorities when leaving idle.
02/01/96   rdh     Added changes to speed up idle sleep responsiveness.
01/12/96   jca     Fixed code so that phone can't transition to CDMA_OVHD
                   state without setting DB indicators to show CDMA service.
01/02/96   dna     Fixed duplicate detection.
12/28/95   gb      Added chan 0 as the normal channel to sector lock.
12/19/95   gb      Store requested service option for jump to traffic.
12/12/95   jca     Modified code to send Page Response msg with "off hook"
                   service option while WLL phone is off hook.
11/30/95   rdh     Added Search fade report to handle broken Paging channels.
11/30/95   jca     Removed mccreg_get_mob_term check on received pages.
11/29/95   jca     No longer using sid_acq_list.
11/14/95   jca     Removed support for configurable service option 1 def.
11/10/95   jca     Fixed Lock UPC access problems (SCTS bug #547).  Fixed
                   invalid exit code problems (SCTS bug #681).
10/12/95   jca     Fixed call status/no service state UI/MC mismatch bugs.
10/09/95   day     Copy CDMA SMS translated message information to CDMA
                   global data for authentication purposes.
10/05/95   dna     Allowed zero to be a valid value for RAND.
08/22/95   dna     Bug fixes for assign mode '101' and IMSI addressing
08/18/95   dna     Added support for assign mode '101' and origination cont.
08/17/95   jca     Added code to process the Feature Notification Message.
08/11/95   jca     Now sending CALL_RELEASE to UI if ORIGINATION received
                   while the phone is in a locked state.
08/10/95   rdb     Added support for Status Request Message.
08/03/95   day     Move data burst validation from caix.c to layer 3 level
                   so layer 2 ack will always be generated for this message.
08/02/95   day     Change sms_process_mt_msg parameter to pass by address
                   instead by value.  Code optimization eliminated the
                   code which updated this value.
07/27/95   jca     Added code to call UI when Abbreviated Alert order rxed.
07/26/95   day     Move local declarations to regional - prevent task stack
                   overflow.
07/20/95   jca     Code updates to support subscriber access control.
07/18/95   gb      Moved jump_access command changes back to diag.
07/07/95   dna     Code Review changes to authentication.
07/06/95   jca     Now dealing with a single origination command.
07/05/95   rdh     Added BIO_SW9 dipsw control to sleep disable feature.
06/28/95   rdh     Made sleep disable more generic to DB types.
06/25/95   dy      Added support for mobile-originated and mobile-terminated
                   SMS messages.
06/22/95   rdh     Added sleep disable function using DB structure.
06/21/95   rdh     Fixed wakeup problem in start_sleep now that RXC hits chipx8.
06/20/95   jca     Added support for mobile-originated SO2 Loopback calls.
06/15/95   jca     Code updates to support AutoNAM processing.
06/09/95   jca     Mods to support new queue sigs interface to mcc_wait().
06/07/95   dna     Added Authentication.
05/30/95   gb      Fixed JUMP_ACCESS Test Mobile support.
05/25/95   jca     Updates for IS-95A registration procedures.
04/28/95   rdh     Wake RXC up early on WAKEUP command from Search.
04/25/95   dna     Two bug fixes: Lock/Unlock and pc_wakeup() missing pages
04/13/95   dna     Range checks for overhead messages.  Common layer 2
                   processing for directed messages other than pages.
03/30/95   rdh     Eliminated slam bug catch code. Fixed in Search task.
03/30/95   rdh     Changed exit reason for PN mask change PC loss to ACQ_LOST.
03/15/95   dna     Released to group for code review, for the first time.
01/10/95   dna     Reject pages while ISS1 off hook.
12/13/94   dna     Lock, unlock, maintenance orders handled.
11/29/94   gb      Fixed PN_mask to NV setting for TMOBILE targets.
11/22/94   gb      Rhometer test functionality changed to standard TMOBILE.
11/14/94   dna     Allowed re-entry to CDMA_OVHD from other SA states.
                   Added CAI_PAGING_CHAN_ASSIGN assign mode processing.
11/09/94   jah     Changes to use voc_acquire() and voc_release().
11/09/94   gb      Fixed Sector Lock disable functionality.
11/01/94   gb      Sector Lock test functionality added.
10/24/94   rdh     Additions for PN offset autoswitch/MSM error correction.
09/12/94   dna     Moved ms_orig and xlate_dialed_num to mccdma.c
                   Made many IS-95 compliance changes
08/15/94   jca     Fixed config_msg_seq bug.
08/09/94   jca     Added support for user-configurable service option 1.
07/30/94   rdh     Changed srch_send_neighs to send only when have valid list.
05/12/94   gb      Added support for new test commands.
04/25/94   gb      Parameterised all IS95 timeouts in cai_tmo.
04/20/94   jca     Mods for IS-95 Phase 2 upgrade.
04/18/94   gb      Changed start sleep to always mark sleep as attempted.
04/05/94   gb      Changed idle handoff unjump to normal exit.
04/04/94   gb      Fix start_sleep synchronization between srch & rxc.
03/24/94   gb      Fixed portable ERR_F on sleep idle handoff.
03/01/94   jah     Added pc_wakeup(), and changed pc_idle_ho() to take a pilot
                   instead of a report pointer.  Added SRCH_RAHO_R processing.
02/26/94   jca     Fixed SEND/END bug.
12/30/93   gb      Added commands to pc_srch_rpt and sleep_start.
12/22/93   jca     Mods for IS-95 Phase 1 upgrade.
09/20/93   gb      Fixed safco jumptraffic voice problem with voc_power.
03/29/93   jai     Added registration
03/24/93   gb      Removed debug t_tdrop, fixed send_srch_parms.
01/29/93   jai     Added slotted mode.
07/24/92   jai     Removed time-outs from calling parameters to procedures which
                   send commands to other tasks. Added debug MSG call.
06/30/92   jai     Modified module for DMSS.
12/20/90   jai     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "qw.h"
#include "rex.h"
#include "queue.h"
#include "task.h"
#include "cai.h"
#include "caii.h"
#include "caix.h"
#include "srchmc.h"
#include "rxcmc.h"
#include "txcmc.h"
#include "rf.h"
#include "nv.h"
#include "err.h"
#include "mccdma.h"
#include "mc.h"
#include "mci.h"
#include "db.h"
#include "cmd.h"
#include "ran.h"
#include "parm.h"
#include "msg.h"

#ifdef FEATURE_MULTI_RX_CHAIN
#include "rfm.h"
#else
#include "rf.h"
#endif /* FEATURE_MULTI_RX_CHAIN */

#include "mccreg.h"
#include "sd.h"
#include "sdprl.h"
#include "voc.h"
#include "mobile.h"
#include "mccrx.h"
#include "mccrxtx.h"
#include "mccsrch.h"
#include "bio.h"
#include "distreg.h"
#include "mccsyobj.h"
#include "mcctcsup.h"

#ifdef FEATURE_AUTH
#include "auth.h"
#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
#include "uim.h"
#endif /* FEATURE_UIM_RUIM */

#include "tmsi.h"
#include <string.h>
#include "snm.h"

#include "ts.h"

#include "mccqpch.h"
#include "mccap.h"
#include "mccsch.h"

#include "event.h"
#include "mclog.h"
#include "mccbcsms.h"

#ifdef FEATURE_PASSPORT_MODE
#include "bit.h"
#endif /* FEATURE_PASSPORT_MODE */

#include "mmoc.h"
#include "cmxll.h"

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

#if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_IS2000_REL_A_AES
#error code not present
#endif /* FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_FULL_TIME_SHDR_3
#include "bio.h"
#endif /* FEATURE_FULL_TIME_SHDR_3 */

#ifdef FEATURE_OTASP
#include "otaspi.h"
#endif /* FEATURE_OTASP */

/*lint -e818 */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

static db_items_value_type db_item;
  /* Buffer to send data to DB */

/************************************************************************/
/*                    Idle Slotted Variables                            */
/************************************************************************/

/* CDMA Idle Sleep Substate */
typedef enum
{
  MCC_NON_SLEEP,
  MCC_SLEEP,
  MCC_WAKING_UP,
  MCC_WAIT_FOR_MORE_SLEEP
} mccidl_sleep_state_type;

/* Sleep Substate for the new machine gun searcher algorithm */
static mccidl_sleep_state_type mccidl_sleep_state = MCC_NON_SLEEP;

/* Flag used for MC slotted sleep state machine control. Set to TRUE
 * if MC is rudely awakened or MC autonomously decides to get out sleep.
 */
static boolean mccidl_need_full_wakeup = TRUE;

/* Flag used to indicate whether Idle handoffs can occur quickly */
static boolean mccidl_cheap_idle_ho = FALSE;

/* This flag tell cdma_idle_online that mccreg_idle_check() has been
 * called and a registration with type of cdma.reg_type is pending.
 */
static boolean mccidl_reg_required = FALSE;

/* Old SID/NID */
static word mccidl_old_bs_sid;
static word mccidl_old_bs_nid;

/* Indicates whether Fast RAHO in progress */
static boolean mccidl_fast_raho = FALSE;

/* Indicates whether fast RAHO had performed */
static boolean mccidl_fast_raho_performed = FALSE;

/* Idle substate which Mobile Station to return to after wakeup */
static word mccidl_fast_raho_state;

/* Indicates if re-acquired after Fast RAHO */
static boolean mccidl_reacq_after_fast_raho = FALSE;

/* Indicates whether or not we have ignored a RESCAN */
static boolean rescan_ignored = FALSE;

/* Indicates whether or not we have ignored a Global Service Redirection
   Message */
static boolean gsr_ignored = FALSE;

/* Indicates whether we have ignored a Ext. Global Service Redirection
   Message */
static boolean egsr_ignored = FALSE;

/* Indicates whether the mobile is operating in slotted mode */
static boolean idle_slotted;

/* The smaller of SLOT_CYCLE_INDEXp and MAX_SLOT_CYCLE_INDEX */
static int8 slot_cycle_index_s = 0;

/* Hashed Paging Channel slot (0-2047) */
static word idle_pgslot = 0;

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/* Mask to get position-in-slot from pgslot */
static word sci_mask;

/* Indicates whether conditions other than configuration messages being
   current are satisfied for sleep */
static boolean mccidl_sleep_ok;

/* Indicates whether searcher task has already transitioned to sleep */
static boolean mccidl_sleep_active = FALSE;

/* Indicates whether access fade timer is in use */
static boolean access_fade_timer_in_use;

/* Indicates if we have to notify SD on overhead info in Update Overhead Information State */
static boolean oh_sd_rpt_pending_in_upd_ovhd = FALSE;

/* Indicates if we have re-entered Update Overhead Information State with
   a Page Response Retransmission Indication */
static boolean page_res_retransmission;

/* Any sleep requests generated before this time are ignored by call
   processing.  This avoids sleep commands hanging around in a message
   queue during sleep, if the mobile was put to sleep due to a different
   reason. */
static qword mccidl_sleep_ok_time;

/* Structure to hold receive message we are to process */
static mccdma_matched_msg_type mccidl_matched_msg;

#ifdef FEATURE_BROADCAST_SMS
/* Structure to hold the Page message when broadcast    */
/* index is not initialized after powerup.              */
static mccrxtx_msg_type bcast_page;

/* Indicates if there is a pending (enhanced) broadcast */
/* page to be processed after powerup.                  */
static boolean bcast_pending = FALSE;
#endif /*  FEATURE_BROADCAST_SMS */

/* Flag to indicate if Channel List Message is pending to process */
static boolean cl_proc_pending  = FALSE;

/* Flag to indicate if Extended Channel List Message is pending to process */
static boolean ecl_proc_pending = FALSE;

/* Flag to indicate if Global Service redirection Message is pending to
   process */
static boolean gsd_proc_pending = FALSE;

/* Flag to indicate if Extended Global Service Redirection Message is pending
   to process */
static boolean egsd_proc_pending = FALSE;

/* the slot(s) whose sleeping criteria are satisfied */
static byte sleep_slot_mask = 0;

#ifdef FEATURE_IS2000_REL_A_CC
static boolean mccidl_monitor_next_slot = FALSE;
 /* Whether MS need to monitor next slot because of Universal Page Message's
    READ_NEXT_SLOT or READ_NEXT_SLOT_BCAST is set */
#endif /* FEATURE_IS2000_REL_A_CC */

struct
{
  byte                              channel_monitor_mask;
  #ifdef FEATURE_IS2000_REL_A_CC
  mcc_channel_monitor_priority_type bcch_monitor_priority;
  byte                              bcch_walsh_chan;
  cai_bcch_rate_type                bcch_data_rate;
  cai_code_rate_type                bcch_code_rate;
  mcc_channel_monitor_priority_type fccch_monitor_priority;
  byte                              fccch_walsh_chan;
  cai_fccch_rate_type               fccch_data_rate;
  cai_code_rate_type                fccch_code_rate;
  #endif /* FEATURE_IS2000_REL_A_CC */
  word                              pch_walsh_chan;
  cai_pch_rate_type                 pch_data_rate;
} active_f_csch_info;
  /* Information of active Idle channel */

/* Flag to indicate if EPZID processing is pending. */
boolean mccidl_epzid_proc_pending;
 
/************************************************************************/
/*                       Other variables                                */
/************************************************************************/

/* Stores the channel that messages are received on so the channel
   can be checked to make sure it hasn't changed before going to
   System Access State */
static cdma_msg_rxed_chan_type msg_rxed_chan;

/* Flags that we want to power down but are attempting to do a powerdown
   registration first.  */
static boolean attempting_powerdown_reg;

#ifdef FEATURE_IS2000_REL_D
#error code not present
#else
/* The length of the intermediate channel list is num_freq. */ 
#define VALID_CHNLIST_MAX   CAI_EXT_CHNLIST_FREQ_MAX 
#endif /* FEATURE_IS2000_REL_D */

static cdma_int_chn_list_type valid_chnlist[VALID_CHNLIST_MAX];

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

#ifdef FEATURE_GPS_BS_CAL_TEST
#error code not present
#endif

#if (defined(FEATURE_HIGH_PSIST) && defined (FEATURE_PASSPORT_MODE))
/* The following indicates the threshold above which KDDI wants the
   persistence value to be considered "high".  This means we will 
   block Registrations and mobile-originated DBMs when a DO call 
   originates. */
#define KDDI_HIGH_PSIST_THRESHOLD   24

/* Indicates the last psist value that we reported to CM */
static cm_psist_e_type mccidl_last_reported_psist_value = CM_PSIST_UNKNOWN;
#endif /* FEATURE_HIGH_PSIST && FEATURE_PASSPORT_MODE */

/* Allow Search to re-sleep then retry reacq, a number of times after RAFAIL */
#define MCCIDL_MAX_RAFAIL_RETRY_CNT   3
static word mccidl_rafail_retry_cnt; 

static byte num_ovhd_cycles = 0;     /* Number of ovhd cycles */

#ifdef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
static struct 
{
  boolean             egsr_is_synch_workaround;
    /* Indicates whether EGSR is used for Sync Chan Workaround */
  uint8               egsr_rec_index;
    /* Indicates the index of the redirecton record in EGSR that's used for 
       Sync Chan Workaround. */
    /* By default egsr_rec_index is set to '0' to indicate the first record
       is used for synch workaround */
} mccidl_synch_workaround = {FALSE, 0};
#endif /* FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND */

/* Indicates the size of redirection record list MC passes to SD */
#ifdef FEATURE_IS2000_REL_D
#error code not present
#else 
#define MCC_MAX_REDIR_RECS  1
#endif /* FEATURE_IS2000_REL_D */

/* List of redirection records from GSR / EGSR. */
static struct 
{
  uint8  num_recs;
    /* Number redirection records in redir_rec_list */

  sd_redir_rec_s_type rec_list[MCC_MAX_REDIR_RECS];
    /* Holds the redirection records MC passes to SD */
} mccidl_glob_redir_rec_list;

/*************************************************/
/*            Function prototypes                */
/*************************************************/

word mccidl_proc_cl_msg(word new_state);

word mccidl_proc_gsr(word new_state);

word mccidl_proc_ecl_msg(word new_state);

word mccidl_proc_egsr(word new_state);

word mccidl_process_stored_ovhds(word new_state);

#ifdef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
boolean mccidl_is_egsr_synch_workaround(const caii_ext_glb_redir_info_type * redir_rec_ptr);

word mccidl_proc_synch_workaround(word new_state, 
                                  const caii_ext_glb_redir_info_type * redir_rec_ptr);
#endif /* FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*===========================================================================

FUNCTION MCCIDL_RETRYING_SLEEP

DESCRIPTION
  This function returns whether the phone is in the process of retrying
  sleep via OOS phase 1.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we're retrying sleep in between reacq failures, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccidl_retrying_sleep(void)
{

  return (mccidl_rafail_retry_cnt != MCCIDL_MAX_RAFAIL_RETRY_CNT);

} /* mccidl_retrying_sleep */

/*===========================================================================

FUNCTION MCCIDL_SET_BS_INSANITY_TIMER

DESCRIPTION
  This procedure starts the BS insanity timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_set_bs_insanity_timer(void)
{

  /* Set base station insanity timer */
  MSG_LOW("Start BS insanity timer", 0, 0, 0);
  (void) mcc_set_callback_timer(&mcc_bs_insanity_timer, 
    MCC_VALID_OVHD_TIMEOUT, MCC_BS_INSANITY_TIMER_ID);

} /* mccidl_set_bs_insanity_timer */

/*===========================================================================

FUNCTION MCCIDL_CLEAR_BS_INSANITY_TIMER

DESCRIPTION
  This procedure clears the BS insanity timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_clear_bs_insanity_timer(void)
{
  /* Clear BS insanity timer */
  MSG_LOW("Clear BS insanity timer", 0, 0, 0);
  (void) mcc_clr_callback_timer(&mcc_bs_insanity_timer,
    MCC_BS_INSANITY_TIMER_ID);

} /* mccidl_clear_bs_insanity_timer */

/*===========================================================================

FUNCTION MCCIDL_SEND_SRCH_IDLE_CMD

DESCRIPTION
  This procedure sends the Search task a SRCH_IDLE_F command to move it back
  to Idle state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_srch_idle_cmd(void)
{
  mcc_srch_buf.idle.hdr.cmd = SRCH_IDLE_F;
  mcc_srch_cmd(&mcc_srch_buf);

} /* mccidl_send_srch_idle_cmd */

/*===========================================================================

FUNCTION MCCIDL_SEND_SRCH_PARMS

DESCRIPTION
  This procedure sends the Search task a SRCH_PARM_F command to update
  searh parameters.

DEPENDENCIES
  mcc_ho_real must have been updated appropriately

RETURN VALUE
 None. 

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_srch_parms(void)
{
  /* Check to see if all necessary overhead messages are received */
  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
   if (ANSI41SP_MSG_NOT_RECVD || MCRRP_MSG_NOT_RECVD ||
       UNIV_NGHBR_LIST_MSG_NOT_RECVD)
   {
     return;
   }
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  {
    if (SP_MSG_NOT_RECVD || ESP_MSG_NOT_RECVD)
    {
      return;
    }
  }

  /* Send search command with new search parameters */
  mcc_send_srch_parms();

} /* mccidl_send_srch_parms() */

/*===========================================================================

FUNCTION MCCIDL_SEND_SRCH_IDLE_ASET

DESCRIPTION
  This procedure sends the Search task a SRCH_IDLE_ASET_F command to update
  the active set information.

DEPENDENCIES
  Information in variables active_f_csch_info and cdma must be up-to-date.  

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_srch_idle_aset(void)
{
  dword mask = 0xA0000000UL;
  int i;

  mcc_srch_buf.pc_aset.hdr.cmd = SRCH_IDLE_ASET_F;
  mcc_srch_buf.pc_aset.aset_pilot = (int2) cur_bs_ptr->pilot_pn;
  mcc_srch_buf.pc_aset.band_class = cdma.band_class;
  mcc_srch_buf.pc_aset.cdma_freq = cdma.cdmach;
  mcc_srch_buf.pc_aset.num_chan = 0;

  mcc_srch_buf.pc_aset.pilot_rec_type = CAI_PILOT_1X_COMMON;

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    /* If non-primary BCCH is under monitoring, just include */
    /* the FCCCH channel aset info to send to Search         */  
    if ((active_f_csch_info.channel_monitor_mask & MCC_FCCCH)
        #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
         ||
        (active_f_csch_info.channel_monitor_mask & MCC_NON_PRI_BCCH)
        #endif /* #ifdef FEATURE_IS2000_REL_A_CC_BCSMS */
        )
    {
      i = active_f_csch_info.fccch_monitor_priority;  /*lint !e641 */
      if (i < MCC_INVALID_PRIORITY) /*lint !e641 */
      {
        /* Pack FCCCH info */
        if (i < CAI_MAX_NUM_CHAN_MONITOR)
        {
          mcc_srch_buf.pc_aset.aset_info[i].chan_id = CAI_F_CCCH;
          mcc_srch_buf.pc_aset.aset_info[i].aset_walsh =
          active_f_csch_info.fccch_walsh_chan          =
            cur_bs_ptr->csp.fccch_info.fccch_code_chan[cdma.fccch_id-1];
          mask |= 0x08000000UL;
          qw_set(mcc_srch_buf.pc_aset.aset_info[i].pn_mask, 0x319L, mask);
          mcc_srch_buf.pc_aset.num_chan++;
        }
      }
      else
      {
        MSG_ERROR("Invalid FCCCH monitor priority", 0, 0, 0);
      }
    }

    if (active_f_csch_info.channel_monitor_mask & MCC_PRI_BCCH)
    {
      i = active_f_csch_info.bcch_monitor_priority; /*lint !e641 */
      if (i < MCC_INVALID_PRIORITY) /*lint !e641 */
      {
        /* Pack BCCH info */
        if (i < CAI_MAX_NUM_CHAN_MONITOR)
        {
          mcc_srch_buf.pc_aset.aset_info[i].chan_id = CAI_BCCH;
          mcc_srch_buf.pc_aset.aset_info[i].aset_walsh =
          active_f_csch_info.bcch_walsh_chan           =
            cdma.bcch;
          mask |= 0x04000000UL;
          mask |= ((dword)(MCC_PRI_BCCH_BCN & 7) << 21); /*lint !e737*/
          mask |= cur_bs_ptr->pilot_pn &0x1FF ;
          qw_set(mcc_srch_buf.pc_aset.aset_info[i].pn_mask, 0x319L, mask);
          mcc_srch_buf.pc_aset.num_chan++;
        }
      }
      else
      {
        MSG_ERROR("Invalid FCCCH monitor priority", 0, 0, 0);
      }
    }

    if (cur_bs_ptr->td_supported)
    {
      mcc_srch_buf.pc_aset.pilot_rec_type = CAI_PILOT_REC_1X_COM_TD;
      mcc_srch_buf.pc_aset.pilot_rec.rec0.td_mode = cdma.td_mode;
      mcc_srch_buf.pc_aset.pilot_rec.rec0.td_power_level = cdma.td_power_level;
    }
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  {
    /* Pack PCH info */
    i =  mcc_srch_buf.pc_aset.num_chan;
    mcc_srch_buf.pc_aset.aset_info[i].chan_id = CAI_PCH;
    mcc_srch_buf.pc_aset.aset_info[i].aset_walsh =
    #ifdef FEATURE_IS2000_REL_A_CC
    active_f_csch_info.pch_walsh_chan            =
    #endif /* FEATURE_IS2000_REL_A_CC */
      cdma.pagech;
    mask |= ((dword)(cdma.pagech & 7) << 21); /*lint !e737*/
    mask |= cur_bs_ptr->pilot_pn &0x1FF ;
    qw_set(mcc_srch_buf.pc_aset.aset_info[i].pn_mask, 0x319L, mask);
    mcc_srch_buf.pc_aset.num_chan++;
  }

  mcc_srch_cmd(&mcc_srch_buf);

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    db_item.code_channel = cdma.bcch;
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  {
    db_item.code_channel = cdma.pagech;
  }
  db_put(DB_CODE_CHANNEL, &db_item);

}/* mccidl_send_srch_idle_aset */

/*===========================================================================

FUNCTION MCCIDL_SEND_SRCH_IDLE_NSET

DESCRIPTION
  This procedure sends the Search task a SRCH_IDLE_NSET_F command to update
  the neighbor set information.

DEPENDENCIES
  Depends on new neighbor list message being in bs_info.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_srch_idle_nset( void )
{
  word i; /* loop counter */
  word n = 0; /* Index in extended neighbor list */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check to see if all necessary overhead messages are received */
  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    if (MCRRP_MSG_NOT_RECVD || UNIV_NGHBR_LIST_MSG_NOT_RECVD)
    {
      return;
    }
    /* Temporary neigbor list is only for CP reference */
    else if (cur_bs_ptr->unlm_config_msg_seq == TEMP_NL_SEQ)
    {
      return;
    }
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  {
    if (SP_MSG_NOT_RECVD || ESP_MSG_NOT_RECVD || NGHBR_LIST_MSGS_NOT_RECVD)
    {
      return;
    }
    /* Temporary neigbor list is only for CP reference */
    else if (cur_bs_ptr->nl_config_msg_seq == TEMP_NL_SEQ)
    {
      return;
    }
  }

  /* ------------------
  ** Access in Progress
  ** ------------------ */
  if (P_REV_IS_4_OR_GREATER && mccsa_access_attempt_in_progress())
  {
    /* ---------------------------------------------------------------
    ** Update the neighbor record of all pilots in Access Attempt list
    ** based on neighbor list info
    ** --------------------------------------------------------------- */
    mccsa_update_access_attempt_list();
  }

  /* send search command with new neighbor list */
  mcc_srch_buf.nset.hdr.cmd   = SRCH_IDLE_NSET_F;

  mcc_srch_buf.nset.pilot_inc = cur_bs_ptr->nl.pilot_inc;
  mcc_srch_buf.nset.nghbr_srch_mode = cur_bs_ptr->nl.nghbr_srch_mode;
  mcc_srch_buf.nset.nset_cnt  = cur_bs_ptr->nl.num_nghbr_pns;

  for (i=0; i < cur_bs_ptr->nl.num_nghbr_pns && i < SRCH_NSET_MAX; i++)
  {
    memset(&mcc_srch_buf.nset.nset[n], 0, sizeof(mcc_srch_buf.nset.nset[n]));

    /* -------------------------------------------------------------------
    ** FEATURE_JSTD008 is removed from here, because now we are using a
    ** unified MC-SRCH interface to pass neighbor list info. All the
    ** unused/undefined fields have been defaulted in the neighbor list
    ** message processing function.
    ** ------------------------------------------------------------------- */
    mcc_srch_buf.nset.nset[n].pilot = (int2) cur_bs_ptr->nl.nset[i].pilot_pn;
    mcc_srch_buf.nset.nset[n].pri = (byte) cur_bs_ptr->nl.nset[i].search_pri;
    mcc_srch_buf.nset.nset[n].freq_incl = cur_bs_ptr->nl.nset[i].freq_incl;
    mcc_srch_buf.nset.nset[n].band_class = (byte) cur_bs_ptr->nl.nset[i].band;
    mcc_srch_buf.nset.nset[n].cdma_freq = (word) cur_bs_ptr->nl.nset[i].freq;
    mcc_srch_buf.nset.nset[n].config = cur_bs_ptr->nl.nset[i].config;

    mcc_srch_buf.nset.nset[n].srch_offset_nghbr =
      cur_bs_ptr->nl.nset[i].srch_offset_nghbr;
    if (cur_bs_ptr->nl.nset[i].add_pilot_rec_incl)
    {
      mcc_srch_buf.nset.nset[n].pilot_rec_type =
        cur_bs_ptr->nl.nset[i].nghbr_pilot_rec_type;
      mcc_srch_buf.nset.nset[n].pilot_rec =
        cur_bs_ptr->nl.nset[i].nghbr_pilot_rec;
    }
    else
    {
      mcc_srch_buf.nset.nset[n].pilot_rec_type = CAI_PILOT_1X_COMMON;
    }

    #ifdef FEATURE_IS2000_REL_A
    mcc_srch_buf.nset.nset[n].bcch_supported = cur_bs_ptr->nl.nset[i].bcch_supported;
    #endif /* FEATURE_IS2000_REL_A */

    if (P_REV_IS_4_OR_GREATER) /*lint !e506*/
    {
      /* Set the Access Handoff control parameters */
      mcc_srch_buf.nset.nset[n].access_entry_ho =
        cur_bs_ptr->nl.nset[i].access_entry_ho;
      mcc_srch_buf.nset.nset[n].access_ho_allowed =
        cur_bs_ptr->nl.nset[i].access_ho_allowed;
      mcc_srch_buf.nset.nset[n].srch_win_nghbr =
        cur_bs_ptr->nl.nset[i].srch_win_nghbr;
      /* Set the Timing offsets of the Neighbor pilot */
      mcc_srch_buf.nset.nset[n].timing_incl =
        cur_bs_ptr->nl.nset[i].timing_incl;
      if (cur_bs_ptr->nl.nset[i].timing_incl)
      {
        mcc_srch_buf.nset.nset[n].nghbr_tx_offset =
          cur_bs_ptr->nl.nset[i].nghbr_tx_offset;
        mcc_srch_buf.nset.nset[n].nghbr_tx_duration =
          cur_bs_ptr->nl.nset[i].nghbr_tx_duration;
        mcc_srch_buf.nset.nset[n].nghbr_tx_period =
          cur_bs_ptr->nl.nset[i].nghbr_tx_period;
      }
    }

    /* Initialize all pilot's access attempted flag to FALSE */
    mcc_srch_buf.nset.nset[n].access_attempted = FALSE;

    /* Test for supported band-class */
    if (cur_bs_ptr->nl.nset[i].freq_incl &&
        (!mcc_is_band_class_supported(cur_bs_ptr->nl.nset[i].band)))
    {
      /* Reduce count by 1 for every filtered neighbor. */
      mcc_srch_buf.nset.nset_cnt--;
    }
    else
    {
      /* Increment the index */
      n++;
    }
  } /* for */

  /* --------------------------
  ** Access attempt in Progress
  ** -------------------------- */
  if (P_REV_IS_4_OR_GREATER && mccsa_access_attempt_in_progress())
  {
    /* ---------------------------------------------------------------------
    ** (1) Add pilots which are in Access Attempt List but not in Neighbor
    **     List to Searcher Neighbor Set. MS must report those pilots in
    **     pilot reporting.
    ** (2) For those pilots which are in both Access Attempt List and
    **     Neighbor List, set the ACCESS_HO_ALLOWED and ACCESS_ATTEMPTED of
    **     its record in Searcher Neighbor Set to TRUE, irrespectivly what
    **     is in their neighbor record.
    ** --------------------------------------------------------------------- */
    mccsa_update_srch_neighs();
  }

  mcc_srch_cmd(&mcc_srch_buf);

}/* mccidl_send_srch_idle_nset */

/*===========================================================================

FUNCTION MCCIDL_INIT_CHANNEL_MONITORING

DESCRIPTION
  This procedure initialize channel monitoring state variables.

DEPENDENCIES
  None.

RETURN VALUE
  Number of channel configurations in the priority list.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_init_channel_monitoring(void)
{

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    active_f_csch_info.channel_monitor_mask =  MCC_PRI_BCCH;
    /* Save active channel configuration for channel monitoring supervision */
    active_f_csch_info.bcch_monitor_priority = MCC_FIRST_PRIORITY;
    active_f_csch_info.bcch_walsh_chan = (byte) cdma.bcch;
    active_f_csch_info.bcch_data_rate = cdma.brat;
    active_f_csch_info.bcch_code_rate = cdma.bcch_code_rate;
    active_f_csch_info.fccch_monitor_priority = MCC_INVALID_PRIORITY;
    active_f_csch_info.fccch_walsh_chan = 0;
    active_f_csch_info.fccch_data_rate = CAI_9600_BPS_20MS_FRAME;
    active_f_csch_info.fccch_code_rate = CAI_CODE_RATE_QTR; 
    MSG_HIGH("MS is monitoring PRI BCCH", 0, 0, 0);
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  {
    active_f_csch_info.channel_monitor_mask = MCC_PCH;
    /* Save active channel configuration for channel monitoring supervision */
    active_f_csch_info.pch_walsh_chan = cdma.pagech;
    active_f_csch_info.pch_data_rate = cdma.prat;

    #ifdef FEATURE_IS2000_REL_A_CC
    active_f_csch_info.bcch_monitor_priority = MCC_INVALID_PRIORITY;
    active_f_csch_info.fccch_monitor_priority = MCC_INVALID_PRIORITY;
    #endif /* FEATURE_IS2000_REL_A_CC */

    MSG_HIGH("MS is monitoring PCH", 0, 0, 0);
  }

  /* Event report: Common Channel Monitored */
  mclog_report_event_common_channel_monitored(
    active_f_csch_info.channel_monitor_mask);

} /* mccidl_init_channel_monitoring */

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION MCCIDL_DECODE_FCCCH_DATA_RATE

DESCRIPTION
  This procedure decodes the Forward Common Control Channel's data rate
  based on the Forward Common Control Channel's rate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

cai_fccch_data_rate_type mccidl_decode_fccch_data_rate
(
  cai_fccch_rate_type rate
)
{

  cai_fccch_data_rate_type data_rate = CAI_FCCCH_DATA_RATE_INVALID;

  switch (rate)
  {
    case CAI_9600_BPS_20MS_FRAME:
      data_rate = CAI_FCCCH_9600_RATE;
      break;


    case CAI_19200_BPS_20MS_FRAME:
    case CAI_19200_BPS_10MS_FRAME:
      data_rate = CAI_FCCCH_19200_RATE;
      break;

    case CAI_38400_BPS_20MS_FRAME:
    case CAI_38400_BPS_10MS_FRAME:
    case CAI_38400_BPS_5MS_FRAME:
      data_rate = CAI_FCCCH_38400_RATE;
      break;

    default:
      /* Shall never be here */
      MSG_ERROR("Invalid FCCCH data rate", 0, 0, 0);
      break;
  }
  return data_rate;

} /* mccidl_decode_fccch_data_rate */

/*===========================================================================

FUNCTION MCCIDL_DECODE_FCCCH_FRAME_SIZE

DESCRIPTION
  This procedure decodes the Forward Common Control Channel's frame size
  based on the Forward Common Control Channel's rate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

cai_fccch_frame_size_type mccidl_decode_fccch_frame_size
(
  cai_fccch_rate_type rate
)
{

  cai_fccch_frame_size_type frame_size = CAI_FCCCH_FRAME_SIZE_INVALID;

  switch (rate)
  {
    case CAI_9600_BPS_20MS_FRAME:
    case CAI_19200_BPS_20MS_FRAME:
    case CAI_38400_BPS_20MS_FRAME:
      frame_size = CAI_FCCCH_20MS_FRAME;
      break;


    case CAI_19200_BPS_10MS_FRAME:
    case CAI_38400_BPS_10MS_FRAME:
      frame_size = CAI_FCCCH_10MS_FRAME;
      break;

    case CAI_38400_BPS_5MS_FRAME:
      frame_size = CAI_FCCCH_5MS_FRAME;
      break;

    default:
      /* Shall never be here */
      MSG_ERROR("Invalid FCCCH frame size", 0, 0, 0);
      break;
  }
  return frame_size;

} /* mccidl_decode_fccch_frame_size */

/*===========================================================================

FUNCTION MCCIDL_ARE_BCCH_DATA_RATES_VALID

DESCRIPTION
  This function determines if BCCH data rates (BRAT's) are within the valid
  range.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all BCCH data rates are within the valid range.
  FALSE if any are invalid.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccidl_are_bcch_data_rates_valid
(
 cdma_bcast_bcch_info_type * bcch_info_type
)
{
  int i;

  for (i = 0; i < bcch_info_type->num_bcch_bcast; i++)
  {
    if (bcch_info_type->bcch_info[i].brat < CAI_BCCH_RATE_COUNT)
    {
      continue;
    }
    else
    {
      MSG_ERROR("Invalid BCCH data rate", 0, 0, 0);
      return FALSE;
    }
  } /* for */

  return TRUE;
 
} /* mccidl_are_bcch_data_rates_valid */

/*===========================================================================

FUNCTION MCCIDL_SEND_SRCH_CHAN_CONFIG_CMD

DESCRIPTION
  This procedure sends the Search task a SRCH_CHAN_CONFIG_F command to update
  monitoring channels' configurations. The parameter of the command is a
  priority list in the order:
    FCCCH, Primary BCCH, non-primary BCCH
                  or
    Paging Channel

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_srch_chan_config_cmd(void)
{
  int i = 0;

  mcc_srch_buf.chan_config.hdr.cmd = SRCH_CHAN_CONFIG_F;
  mcc_srch_buf.chan_config.num_chan = 0;

  /* Pack the priority list */

  if (OVHD_CHAN_IS_BCCH)
  {
    /* See if FCCCH ready to be monitored */
    if ((active_f_csch_info.channel_monitor_mask & MCC_FCCCH) &&
        (mcc_srch_buf.chan_config.num_chan < CAI_MAX_NUM_CHAN_MONITOR))
    {
      i = active_f_csch_info.fccch_monitor_priority;  /*lint !e641 */
      if (i < MCC_INVALID_PRIORITY) /*lint !e641 */
      {
        /* Pack FCCCH config */
        if (i < CAI_MAX_NUM_CHAN_MONITOR)
        {
          mcc_srch_buf.chan_config.chan_info[i].chan_id = CAI_F_CCCH;
          mcc_srch_buf.chan_config.chan_info[i].config.fccch.fccch_id =
            cdma.fccch_id;
          mcc_srch_buf.chan_config.chan_info[i].config.fccch.walsh_chn =
            cur_bs_ptr->csp.fccch_info.fccch_code_chan[cdma.fccch_id-1];
          mcc_srch_buf.chan_config.chan_info[i].config.fccch.c_rate =
            cur_bs_ptr->csp.fccch_info.fccch_code_rate;
          mcc_srch_buf.chan_config.chan_info[i].config.fccch.rate =
            mccidl_decode_fccch_data_rate(cur_bs_ptr->csp.fccch_info.fccch_rate);
          mcc_srch_buf.chan_config.chan_info[i].config.fccch.size =
            mccidl_decode_fccch_frame_size(cur_bs_ptr->csp.fccch_info.fccch_rate);
          mcc_srch_buf.chan_config.num_chan++;
        }
      }
      else
      {
        MSG_ERROR("Invalid FCCCH monitor priority", 0, 0, 0);
      }
    }

    /* See if Primary BCCH ready to be monitored */
    if ((active_f_csch_info.channel_monitor_mask & MCC_PRI_BCCH) &&
        (mcc_srch_buf.chan_config.num_chan < CAI_MAX_NUM_CHAN_MONITOR))
    {
      i = active_f_csch_info.bcch_monitor_priority; /*lint !e641 */
      if (i < MCC_INVALID_PRIORITY) /*lint !e641 */
      {
        /* Pack Primary BCCH config */
        if (i < CAI_MAX_NUM_CHAN_MONITOR)
        {
          mcc_srch_buf.chan_config.chan_info[i].chan_id = CAI_BCCH;
          mcc_srch_buf.chan_config.chan_info[i].config.bcch.walsh_chn =
            cdma.bcch;
          mcc_srch_buf.chan_config.chan_info[i].config.bcch.rate = cdma.brat;
          mcc_srch_buf.chan_config.chan_info[i].config.bcch.c_rate =
            cdma.bcch_code_rate;
          mcc_srch_buf.chan_config.num_chan++;
        }
      }
      else
      {
        MSG_ERROR("Invalid Primary BCCH monitor priority", 0, 0, 0);
      }
    }

    #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
    /* See if Non-Primary BCCH ready to be monitored */
    else if ((active_f_csch_info.channel_monitor_mask & MCC_NON_PRI_BCCH) &&
             (mcc_srch_buf.chan_config.num_chan < CAI_MAX_NUM_CHAN_MONITOR))
    {
      uint8 bcn = mccbcsms_get_bcn() - 2;
      
      i = active_f_csch_info.bcch_monitor_priority; /*lint !e641 */

      if (i < MCC_INVALID_PRIORITY) /*lint !e641 */
      {
        /* Pack Primary BCCH config */
        if (i < CAI_MAX_NUM_CHAN_MONITOR)
        {
          mcc_srch_buf.chan_config.chan_info[i].chan_id = CAI_BCCH;
          mcc_srch_buf.chan_config.chan_info[i].config.bcch.walsh_chn =
            cur_bs_ptr->csp.bcast_bcch_info.bcch_info[bcn].bcch_code_chan;
          mcc_srch_buf.chan_config.chan_info[i].config.bcch.rate = 
            cur_bs_ptr->csp.bcast_bcch_info.bcch_info[bcn].brat;
          mcc_srch_buf.chan_config.chan_info[i].config.bcch.c_rate =
            cur_bs_ptr->csp.bcast_bcch_info.bcch_info[bcn].bcch_code_rate;
          mcc_srch_buf.chan_config.num_chan++;
        }
      }
      else
      {
        MSG_ERROR("Invalid Non-Primary BCCH monitor priority", 0, 0, 0);
      }
    }
    #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  }
  else
  {
    if (active_f_csch_info.channel_monitor_mask & MCC_PCH)
    {
      /* Pack PCH config */
      i = mcc_srch_buf.chan_config.num_chan;
      mcc_srch_buf.chan_config.chan_info[i].chan_id = CAI_PCH;
      mcc_srch_buf.chan_config.chan_info[i].config.pch.walsh_chn = cdma.pagech;
      mcc_srch_buf.chan_config.chan_info[i].config.pch.rate = cdma.prat;
      mcc_srch_buf.chan_config.num_chan++;
    }
  }

  if (mcc_srch_buf.chan_config.num_chan == 0)
  {
    MSG_ERROR("ERR Channel Monitor : ovhd %d Mask %d", cur_bs_ptr->ovhd_chan_info.ovhd_chan,
      active_f_csch_info.channel_monitor_mask, 0);
  }
  else
  {
    /* Send command to SRCH */
    mcc_srch_cmd( &mcc_srch_buf );
  }

} /* mccidl_send_srch_chan_config_cmd */

/*===========================================================================

FUNCTION MCCIDL_SEND_RXC_CHAN_CONFIG_CMD

DESCRIPTION
  This procedure sends the RXC task a RXC_CHAN_CONFIG_F command to update
  monitoring channels' configurations. The parameter of the command is a
  priority list in the order:
    FCCCH, Primary BCCH, Non-Primary BCCH
                  or
    Paging Channel

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_rxc_chan_config_cmd(void)
{
  int i;

  mcc_rx_buf.chan_config.hdr.command = RXC_CHAN_CONFIG_F;
  mcc_rx_buf.chan_config.num_chan = 0;
  mcc_rx_buf.chan_config.skip_frame = FALSE;

  /* Pack the priority list */

  if (OVHD_CHAN_IS_BCCH)
  {
    /* See if FCCCH ready to be monitored */
    if ((active_f_csch_info.channel_monitor_mask & MCC_FCCCH) &&
        (mcc_rx_buf.chan_config.num_chan < CAI_MAX_NUM_CHAN_MONITOR))
    {
      i = active_f_csch_info.fccch_monitor_priority; /*lint !e641 */
      if (i < MCC_INVALID_PRIORITY) /*lint !e641 */
      {
        /* Pack FCCCH config */
        if (i < CAI_MAX_NUM_CHAN_MONITOR)
        {
          mcc_rx_buf.chan_config.chan_info[i].chan_id = CAI_F_CCCH;
          mcc_rx_buf.chan_config.chan_info[i].config.fccch.code_chan =
            cur_bs_ptr->csp.fccch_info.fccch_code_chan[cdma.fccch_id-1];
          mcc_rx_buf.chan_config.chan_info[i].config.fccch.crate =
            cur_bs_ptr->csp.fccch_info.fccch_code_rate;
          mcc_rx_buf.chan_config.chan_info[i].config.fccch.rate =
            mccidl_decode_fccch_data_rate(cur_bs_ptr->csp.fccch_info.fccch_rate);
          mcc_rx_buf.chan_config.chan_info[i].config.fccch.frame_size =
            mccidl_decode_fccch_frame_size(cur_bs_ptr->csp.fccch_info.fccch_rate);
          mcc_rx_buf.chan_config.chan_info[i].ebcsms = FALSE;
          mcc_rx_buf.chan_config.num_chan++;
        }
      }
      else
      {
        MSG_ERROR("Invalid FCCCH monitor priority", 0, 0, 0);
      }
    }

    /* See if Primary BCCH ready to be monitored */
    if ((active_f_csch_info.channel_monitor_mask & MCC_PRI_BCCH) &&
        (mcc_rx_buf.chan_config.num_chan < CAI_MAX_NUM_CHAN_MONITOR))
    {
      i = active_f_csch_info.bcch_monitor_priority; /*lint !e641 */
      if (i < MCC_INVALID_PRIORITY) /*lint !e641 */
      {
        /* Pack Primary BCCH config */
        if (i < CAI_MAX_NUM_CHAN_MONITOR)
        {
          mcc_rx_buf.chan_config.chan_info[i].chan_id = CAI_BCCH;
          mcc_rx_buf.chan_config.chan_info[i].config.bcch.code_chan = cdma.bcch;
          mcc_rx_buf.chan_config.chan_info[i].config.bcch.rate = cdma.brat;
          mcc_rx_buf.chan_config.chan_info[i].config.bcch.crate = cdma.bcch_code_rate;
          mcc_rx_buf.chan_config.chan_info[i].ebcsms = FALSE;
          mcc_rx_buf.chan_config.num_chan++;
        }
      }
      else
      {
        MSG_ERROR("invalid Primary BCCH monitor priority", 0, 0, 0);
      }
    }
    #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
    /* See if Non-Primary BCCH ready to be monitored */
    else if ((active_f_csch_info.channel_monitor_mask & MCC_NON_PRI_BCCH) &&
             (mcc_rx_buf.chan_config.num_chan < CAI_MAX_NUM_CHAN_MONITOR))
    {
      byte  bcn = mccbcsms_get_bcn() - 2;

      i = active_f_csch_info.bcch_monitor_priority; /*lint !e641 */
      if (i < MCC_INVALID_PRIORITY) /*lint !e641 */
      {
        /* Pack Non-Primary BCCH config */
        if (i < CAI_MAX_NUM_CHAN_MONITOR)
        {
          mcc_rx_buf.chan_config.chan_info[i].chan_id = CAI_BCCH;
          mcc_rx_buf.chan_config.chan_info[i].config.bcch.code_chan =
            cur_bs_ptr->csp.bcast_bcch_info.bcch_info[bcn].bcch_code_chan;
          mcc_rx_buf.chan_config.chan_info[i].config.bcch.rate = 
            cur_bs_ptr->csp.bcast_bcch_info.bcch_info[bcn].brat;
          mcc_rx_buf.chan_config.chan_info[i].config.bcch.crate = 
            cur_bs_ptr->csp.bcast_bcch_info.bcch_info[bcn].bcch_code_rate;
          mcc_rx_buf.chan_config.chan_info[i].ebcsms = TRUE;
          mcc_rx_buf.chan_config.num_chan++;
        }
      }
      else
      {
        MSG_ERROR("invalid Non-Primary BCCH monitor priority", 0, 0, 0);
      }
    }
    #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
  }
  else
  {
    if (active_f_csch_info.channel_monitor_mask & MCC_PCH)
    {
      /* Pack PCH config */
      i = mcc_rx_buf.chan_config.num_chan;
      mcc_rx_buf.chan_config.chan_info[i].chan_id = CAI_PCH;
      mcc_rx_buf.chan_config.chan_info[i].config.pch.rate = cdma.prat;
      mcc_rx_buf.chan_config.chan_info[i].ebcsms = FALSE;
      mcc_rx_buf.chan_config.num_chan++;
    }
  }

  if (mcc_rx_buf.chan_config.num_chan == 0)
  {
    MSG_ERROR("ERR Channel Monitor: Ovhd %d Mask %d", cur_bs_ptr->ovhd_chan_info.ovhd_chan,
        active_f_csch_info.channel_monitor_mask, 0);
  }
  else
  {
    /* Send command to RXC */
    mcc_rxc_cmd( &mcc_rx_buf );
  }

} /* mccidl_send_rxc_chan_config_cmd */
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION MCCIDL_UPDATE_CHANNEL_MONITORING_INFO

DESCRIPTION
  This procedure updates the internal channel monitoring information.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_update_channel_monitoring_info
(
  mcc_channel_monitor_priority_type pch_monitor_priority,
  mcc_channel_monitor_priority_type pri_bcch_monitor_priority,
  mcc_channel_monitor_priority_type fccch_monitor_priority,
  mcc_channel_monitor_priority_type non_pri_bcch_monitor_priority
)
{
  if ( ( pch_monitor_priority == MCC_INVALID_PRIORITY) 
       #ifdef FEATURE_IS2000_REL_A_CC
       && ( pri_bcch_monitor_priority == MCC_INVALID_PRIORITY )
       && ( fccch_monitor_priority == MCC_INVALID_PRIORITY ) 
       && ( non_pri_bcch_monitor_priority == MCC_INVALID_PRIORITY )
       #endif /* FEATURE_IS2000_REL_A_CC */
     )
  {
    /* Should not happen. No common channel active */
    MSG_ERROR("No common channel active", 0, 0, 0);
  }

  #ifdef FEATURE_IS2000_REL_A_CC
  else if ((pri_bcch_monitor_priority != MCC_INVALID_PRIORITY) && 
      (non_pri_bcch_monitor_priority != MCC_INVALID_PRIORITY))
  {
    /* Should not happen. Primary BCCH and Non-Primary BCCH cannot be active
       at the same time */
    MSG_ERROR("Pri and Non-Pri BCCH active at the same time", 0, 0, 0);
  }
  else if ((pch_monitor_priority != MCC_INVALID_PRIORITY) &&
           ((pri_bcch_monitor_priority != MCC_INVALID_PRIORITY) ||
            (fccch_monitor_priority != MCC_INVALID_PRIORITY) ||
            (non_pri_bcch_monitor_priority != MCC_INVALID_PRIORITY)))
  {
    /* Should not happen. New common channels and old common channel cannot
       be active at the same time */
    MSG_ERROR("NCC and OCC active at the same time", 0, 0, 0);
  }
  #endif /* FEATURE_IS2000_REL_A_CC */  

  else
  {
    /* Update active channel monitoring info */
    active_f_csch_info.channel_monitor_mask = MCC_NONE;

    #ifdef FEATURE_IS2000_REL_A_CC
    active_f_csch_info.bcch_monitor_priority = MCC_INVALID_PRIORITY;
    active_f_csch_info.fccch_monitor_priority = MCC_INVALID_PRIORITY;
    #endif /* FEATURE_IS2000_REL_A_CC */  

    if (pch_monitor_priority != MCC_INVALID_PRIORITY)
    {
      /* Monitor PCH */
      active_f_csch_info.channel_monitor_mask |= MCC_PCH;
    }

    #ifdef FEATURE_IS2000_REL_A_CC
    else
    {
      #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      if ((non_pri_bcch_monitor_priority != MCC_INVALID_PRIORITY) &&
          ((non_pri_bcch_monitor_priority == MCC_FIRST_PRIORITY) ||
           (CAI_MAX_NUM_CHAN_MONITOR > 1))) /*lint !e506 !e774*/
      {
        /* Monitor Non-Primary BCCH */
        active_f_csch_info.channel_monitor_mask |= MCC_NON_PRI_BCCH;
        active_f_csch_info.bcch_monitor_priority = non_pri_bcch_monitor_priority;
      }
      else
      #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
      if ((pri_bcch_monitor_priority != MCC_INVALID_PRIORITY) &&
          ((pri_bcch_monitor_priority == MCC_FIRST_PRIORITY) ||
           (CAI_MAX_NUM_CHAN_MONITOR > 1))) /*lint !e506 !e774*/
      {
        /* Monitor Primary BCCH */
        active_f_csch_info.channel_monitor_mask |= MCC_PRI_BCCH;
        active_f_csch_info.bcch_monitor_priority = pri_bcch_monitor_priority;
      }

      if ((fccch_monitor_priority != MCC_INVALID_PRIORITY) &&
          ((fccch_monitor_priority == MCC_FIRST_PRIORITY) ||
           (CAI_MAX_NUM_CHAN_MONITOR > 1))) /*lint !e506 !e774*/
      {
        /* Monitor FCCCH */
        active_f_csch_info.channel_monitor_mask |= MCC_FCCCH;
        active_f_csch_info.fccch_monitor_priority = fccch_monitor_priority;
      }
    } 
    #endif /* FEATURE_IS2000_REL_A_CC */  
  }

} /* mccidl_update_channel_monitoring_info */

/*===========================================================================

FUNCTION MCCIDL_SUPERVISE_CHANNEL_MONITORING

DESCRIPTION
  This procedure updates the requirements of channel monitoring. It also 
  notifies the lower layers if the requirement changed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_supervise_channel_monitoring
(
  word new_state,
    /* Next state to be processed */
  mccsrch_wakeup_rpt_type *wakeup_rpt_ptr,
    /* Search wakeup report */
  boolean enh_bc_done
    /* Is channel switching due to enh broadcast SMS done */
)
{
  byte old_channel_monitor_mask = active_f_csch_info.channel_monitor_mask;

  #ifdef FEATURE_IS2000_REL_A_CC
  mcc_channel_monitor_priority_type old_bcch_monitor_priority =
    active_f_csch_info.bcch_monitor_priority;
  mcc_channel_monitor_priority_type old_fccch_monitor_priority =
    active_f_csch_info.fccch_monitor_priority;
  #endif /* FEATURE_IS2000_REL_A_CC */

  boolean srch_channel_config_changed = FALSE;
  boolean rxc_channel_config_changed = FALSE;

  /* ********************************** *
   * Figure out new active channel mask *
   * ********************************** */

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    /* Release A common channel */

    #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
    if (enh_bc_done)
    {
      if ((old_channel_monitor_mask & MCC_NON_PRI_BCCH) == 0)
      {
        MSG_ERROR("Chan monitor out of sync!", 0, 0, 0);
      }

      MSG_MED("EBSMS done", 0, 0, 0);
      /* Enhanced Broadcast SMS is received and Non-Primary BCCH 
         monitoring is done. 
         Channel supervision scheme: 
           PCH              (not monitor), 
           Primary BCCH     (not monitor),
           FCCCH            (monitor with 1st priority) and 
           Non-Primary BCCH (not monitor) */
      mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
        MCC_INVALID_PRIORITY, MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY);

      /* Only Non-Primary BCCH is under monitoring. See if OK to sleep.
         We have decided that it's OK to follow the chan_config cmd with
         a sleep command. */
      if ((old_channel_monitor_mask & MCC_FCCCH) == 0)
      {
        qword cur_time;

        ts_get_20ms_frame_time(cur_time);

        /* Update sleep slot mask */
        mccidl_update_sleep_slot_mask(BC_DATA_SLOT);
  
        if (mccidl_assert_sleep_ok(cur_time))
        {
          MSG_MED("BC SMS Rec'd, Sleep OK",0,0,0);
        }
        else
        {
          MSG_MED("Sleep_ok not asserted",0,0,0);
        }
      }
    }

    /* Currently, monitoring secondary BCCH for EBCSMS, don't
       change channel supervision till EBCSMS is done. */
    else if ( (old_channel_monitor_mask & MCC_NON_PRI_BCCH) != 0 )
    {
      MSG_MED("EBCSMS not yet done, don't change channel supervision", 0, 0, 0);
    }
    else
    #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

    /* Pilot beacon */
    if (cdma.num_fccch_s == 0)
    {
      MSG_MED("Pilot beacon", 0, 0, 0);
      /* Pilot beacon.
         Channel supervision scheme: 
           PCH              (not monitor), 
           Primary BCCH     (monitor with 1st priority),
           FCCCH            (not monitor) and
           Non-Primary BCCH (not monitor) */
      mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
        MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY, MCC_INVALID_PRIORITY);
    }
    /* Update overhead info substate */
    else if (new_state == CDMA_UPDATE_OVH) /*lint !e641 */
    {

      #ifdef FEATURE_IS2000_REL_A_CC_SMMC
      if (!mcc_is_ovhd_msg_okay())
            /* Overhead info not up-to-date */
      #else /* FEATURE_IS2000_REL_A_CC_SMMC */
      if (!mcc_is_ovhd_cfg_okay()
            /* Overhead info other than access parameters not up-to-update */
          #ifdef FEATURE_AUTH
          || !mcc_are_rand_parms_okay()
            /* RAND not up-to-date */
          #endif /* FEATURE_AUTH */
          || (!mcc_is_ovhd_msg_okay() && !mcc_is_upd_ovhd_msg_okay()))
            /* Access parameters not up-to-date */
      #endif /* FEATURE_IS2000_REL_A_CC_SMMC */
      {
        if (ANSI41SP_MSG_IS_RECVD && MCRRP_MSG_IS_RECVD &&
            EXT_CHAN_LIST_MSG_IS_RECVD)
        {
          /* Sufficient info to monitor FCCCH.
             Channel supervision scheme: 
               PCH              (not monitor), 
               Primary BCCH     (monitor with 1st priority),
               FCCCH            (monitor with 2nd priority) and
               Non-Primary BCCH (not monitor) */
          mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
            MCC_FIRST_PRIORITY, MCC_SECOND_PRIORITY, MCC_INVALID_PRIORITY);
        }
        else
        {
          /* Insufficient info to monitor FCCCH.
             Channel supervision scheme: 
               PCH              (not monitor), 
               Primary BCCH     (monitor with 1st priority),
               FCCCH            (not monitor) and
               Non-Primary BCCH (not monitor) */
          mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
            MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY, MCC_INVALID_PRIORITY);
        }
      }
      else
      {
        /* Channel supervision scheme: 
             PCH              (not monitor), 
             Primary BCCH     (not monitor),
             FCCCH            (monitor with 1st priority) and
             Non-Primary BCCH (not monitor) */
        mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
          MCC_INVALID_PRIORITY, MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY);
      }
    }
    /* In idle state */
    else if (MAIN_STATE(new_state) == CDMA_IDLE)
    {
      if (!mcc_is_ovhd_cfg_okay() ||
            /* Overhead info other than access parameters not up-to-update */
          ((new_state == CDMA_IDLE_INIT) && !mcc_is_ovhd_msg_okay()) || /*lint !e641 */
              /* In the process of collecting overhead and overhead info not
                 up-to-date yet */
          (((cdma.sa_tc_state != CDMA_IDLE) || (new_state == CDMA_OVHD)) && /*lint !e641 */
              /* Pending access */
           #ifdef FEATURE_IS2000_REL_A_CC_SMMC
           (!mcc_is_ovhd_msg_okay()       /* Overhead info not up-to-date */
           #else /* FEATURE_IS2000_REL_A_CC_SMMC */
           ((mcc_is_ovhd_cfg_okay() && !mcc_is_ovhd_msg_okay() &&
             !mcc_is_upd_ovhd_msg_okay()) /* Access parameters not up-to-date */
           #endif /* FEATURE_IS2000_REL_A_CC_SMMC */
            #ifdef FEATURE_AUTH
            || !mcc_are_rand_parms_okay() /* RAND not up-to-date */
            #endif /* FEATURE_AUTH */
           )))
      {
        if (ANSI41SP_MSG_IS_RECVD && MCRRP_MSG_IS_RECVD)
        {
          /* Sufficient info to monitor FCCCH.
             Channel supervision scheme: 
               PCH              (not monitor), 
               Primary BCCH     (monitor with 1st priority),
               FCCCH            (monitor with 2nd priority) and
               Non-Primary BCCH (not monitor) */
          mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
            MCC_FIRST_PRIORITY, MCC_SECOND_PRIORITY, MCC_INVALID_PRIORITY);
        }
        else
        {
          /* Insufficient info to monitor FCCCH. 
             Channel supervision scheme: 
               PCH              (not monitor), 
               Primary BCCH     (monitor with 1st priority),
               FCCCH            (not monitor) and
               Non-Primary BCCH (not monitor) */
          mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
            MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY, MCC_INVALID_PRIORITY);
        }
      }
      else
      {

        #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
        if (wakeup_rpt_ptr != NULL) 
        {
          if (!idle_slotted ||
              (wakeup_rpt_ptr->cur_slot_mask == 0) ||
                /* Wakeup for user request, O-freq etc */ 
              ((wakeup_rpt_ptr->cur_slot_mask & PAGE_SLOT) != 0) ||
                /* Wakeup for Page Msg */
              ((wakeup_rpt_ptr->cur_slot_mask & BC_PAGE_SLOT) != 0)
                /* Wakeup for BC Page Msg */
             )
          {
            /* Non-slotted mode, page slot or broadcast page slot. */
            if (((wakeup_rpt_ptr->cur_slot_mask & BC_DATA_SLOT) != 0) ||
                ((old_channel_monitor_mask & MCC_NON_PRI_BCCH) != 0)) /*lint !e506 !e774 */
            {
              /* Broadcast data slot is coming or currently in broadcast data 
                 slot. 
                 Channel supervision scheme: 
                   PCH              (not monitor), 
                   Primary BCCH     (not monitor),
                   FCCCH            (monitor with 1st priority) and
                   Non-Primary BCCH (monitor with 2nd priority) */
              mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
                MCC_INVALID_PRIORITY, MCC_FIRST_PRIORITY, MCC_SECOND_PRIORITY);
              
            }
            else
            {
              /* Channel supervision scheme: 
                   PCH              (not monitor), 
                   Primary BCCH     (not monitor),
                   FCCCH            (monitor with 1st priority) and
                   Non-Primary BCCH (not monitor) */
              mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
                MCC_INVALID_PRIORITY, MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY);
            }
          }

          /* Wakeup for BC DBM */
          else if ((wakeup_rpt_ptr->cur_slot_mask & BC_DATA_SLOT) != 0)
          {
            /* Broadcast data slot is coming.
               Channel supervision scheme: 
                 PCH              (not monitor), 
                 Primary BCCH     (not monitor),
                 FCCCH            (not monitor) and
                 Non-Primary BCCH (monitor with 1st priority) */
            mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
              MCC_INVALID_PRIORITY, MCC_INVALID_PRIORITY, MCC_FIRST_PRIORITY);
          }

          else
          {
            /* Channel supervision scheme: 
                 PCH              (not monitor), 
                 Primary BCCH     (not monitor),
                 FCCCH            (monitor with 1st priority) and
                 Non-Primary BCCH (not monitor) */
            mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
              MCC_INVALID_PRIORITY, MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY);
          }
        }
        else
        #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

        {
          /* Channel supervision scheme: 
               PCH              (not monitor), 
               Primary BCCH     (not monitor),
               FCCCH            (monitor with 1st priority) and
               Non-Primary BCCH (not monitor) */
          mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
            MCC_INVALID_PRIORITY, MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY);
        }
      }
    }
    /* In system access state */
    else if (MAIN_STATE(new_state) == CDMA_SYS_ACC)
    {
      /* Overhead messages are not current */
      if (!mcc_is_aho_ovhd_msg_okay())
      {
        if (cdma.ho_state == CDMA_APHO)
        {
          /* Access probe handoff region. */ 
          if (ANSI41SP_MSG_IS_RECVD && MCRRP_MSG_IS_RECVD)
          {
            /* Sufficient info to monitor FCCCH.
               Channel supervision scheme: 
                 PCH              (not monitor), 
                 Primary BCCH     (monitor with 1st priority),
                 FCCCH            (monitor with 2nd priority) and
                 Non-Primary BCCH (not monitor) */
            mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
              MCC_FIRST_PRIORITY, MCC_SECOND_PRIORITY, MCC_INVALID_PRIORITY);
          }
          else
          {
            /* Insufficient info to monitor FCCCH. 
               Channel supervision scheme: 
                 PCH              (not monitor), 
                 Primary BCCH     (monitor with 1st priority),
                 FCCCH            (not monitor) and
                 Non-Primary BCCH (not monitor) */
            mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
              MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY, MCC_INVALID_PRIORITY);
          }
        }
        else if (cdma.ho_state == CDMA_AHO)
        {
          /* Access handoff region. */

          if (ANSI41SP_MSG_IS_RECVD && MCRRP_MSG_IS_RECVD)
          {
            /* Sufficient info to monitor FCCCH. 
               Channel supervision scheme: 
                 PCH              (not monitor), 
                 Primary BCCH     (monitor with 2nd priority),
                 FCCCH            (monitor with 1st priority) and
                 Non-Primary BCCH (not monitor) */
            mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
              MCC_SECOND_PRIORITY, MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY);
          }
          else
          {
            /* Insufficient info to monitor FCCCH. 
               Channel supervision scheme: 
                 PCH              (not monitor), 
                 Primary BCCH     (monitor with 1st priority),
                 FCCCH            (not monitor) and
                 Non-Primary BCCH (not monitor) */
            mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
              MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY, MCC_INVALID_PRIORITY);
          }
        }
        else
        {
          /* Shall never be here */
          MSG_ERROR("Ovhd not current in Sys Acc", 0, 0, 0);

          /* Channel supervision scheme: 
               PCH              (not monitor), 
               Primary BCCH     (monitor with 2nd priority),
               FCCCH            (monitor with 1st priority) and
               Non-Primary BCCH (not monitor) */
          mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
            MCC_SECOND_PRIORITY, MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY);
        }
      }
      else
      {
        /* Channel supervision scheme: 
             PCH              (not monitor), 
             Primary BCCH     (not monitor),
             FCCCH            (monitor with 1st priority) and
             Non-Primary BCCH (not monitor) */
        mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
          MCC_INVALID_PRIORITY, MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY);
      }
    }
    /* Not in idle or system access state */
    else
    {
      /* Overhead is not current */
      if (!mcc_is_ovhd_cfg_okay())
      {
        if (ANSI41SP_MSG_IS_RECVD && MCRRP_MSG_IS_RECVD &&
            EXT_CHAN_LIST_MSG_IS_RECVD)
        {
          /* Sufficient info to monitor FCCCH.
             Channel supervision scheme: 
               PCH              (not monitor), 
               Primary BCCH     (monitor with 1st priority),
               FCCCH            (monitor with 2nd priority) and
               Non-Primary BCCH (not monitor) */
          mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
            MCC_FIRST_PRIORITY, MCC_SECOND_PRIORITY, MCC_INVALID_PRIORITY);
        }
        else
        {
          /* Insufficient info to monitor FCCCH.
             Channel supervision scheme: 
               PCH              (not monitor), 
               Primary BCCH     (monitor with 1st priority),
               FCCCH            (not monitor) and
               Non-Primary BCCH (not monitor) */
          mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
            MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY, MCC_INVALID_PRIORITY);
        }
      }
      /* Overhead is current */
      else
      {
        /* Channel supervision scheme: 
           PCH              (not monitor), 
           Primary BCCH     (not monitor),
           FCCCH            (monitor with 1st priority) and
           Non-Primary BCCH (not monitor) */
        mccidl_update_channel_monitoring_info(MCC_INVALID_PRIORITY,
          MCC_INVALID_PRIORITY, MCC_FIRST_PRIORITY, MCC_INVALID_PRIORITY);
      }
    }
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  {
    /* Release 0 common channel.
       Channel supervision scheme: 
           PCH              (monitor with 1st priority), 
           Primary BCCH     (not monitor),
           FCCCH            (not monitor) and
           Non-Primary BCCH (not monitor) */
      mccidl_update_channel_monitoring_info(MCC_FIRST_PRIORITY,
        MCC_INVALID_PRIORITY, MCC_INVALID_PRIORITY, MCC_INVALID_PRIORITY);
  }

  /* Channel to be monitored changed */
  if (old_channel_monitor_mask != active_f_csch_info.channel_monitor_mask)
  {
    srch_channel_config_changed = rxc_channel_config_changed  = TRUE;

    /* Event report: Common Channel Monitored */
    mclog_report_event_common_channel_monitored(
      active_f_csch_info.channel_monitor_mask);
  }

  #ifdef FEATURE_IS2000_REL_A_CC
  /* Check whether BCCH or FCCCH configurations changed */
  else if (OVHD_CHAN_IS_BCCH)
  {
    if (active_f_csch_info.channel_monitor_mask & MCC_PRI_BCCH)
    {
      /* Channel monitor priority,data rate and code rate changed */
      if ((active_f_csch_info.bcch_monitor_priority !=
           old_bcch_monitor_priority) ||
          (active_f_csch_info.bcch_data_rate != cdma.brat) ||
          (active_f_csch_info.bcch_code_rate != cdma.bcch_code_rate))
      {
        srch_channel_config_changed = rxc_channel_config_changed  = TRUE;
      }
      /* Walsh channel changed */
      else if (active_f_csch_info.bcch_walsh_chan != cdma.bcch)
      {
        srch_channel_config_changed = TRUE;
      }
    }

    /* FCCCH channel configuration changed */
    if (active_f_csch_info.channel_monitor_mask & MCC_FCCCH)
    {
      /* Channel monitoring priority, data rate and code rate changed */
      if ((active_f_csch_info.fccch_monitor_priority !=
           old_fccch_monitor_priority) ||
          (active_f_csch_info.fccch_data_rate !=
           cur_bs_ptr->csp.fccch_info.fccch_rate) ||
          (active_f_csch_info.fccch_code_rate !=
           cur_bs_ptr->csp.fccch_info.fccch_code_rate))
      {
        srch_channel_config_changed = rxc_channel_config_changed  = TRUE;
      }
      /* Walsh channel changed */
      else if (active_f_csch_info.fccch_walsh_chan !=
               cur_bs_ptr->csp.fccch_info.fccch_code_chan[cdma.fccch_id-1])
      {
        srch_channel_config_changed = TRUE;
      }
    }

    #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
    /* Non-Primary BCCH configuartion changed */
    if (active_f_csch_info.channel_monitor_mask & MCC_NON_PRI_BCCH)
    {
      /* Channel monitor priority, data rate and code rate changed */
      if (active_f_csch_info.bcch_monitor_priority !=
          old_bcch_monitor_priority )
      {
        srch_channel_config_changed = rxc_channel_config_changed = TRUE;
      }
      else if ((wakeup_rpt_ptr != NULL) &&
               (wakeup_rpt_ptr->cur_slot_mask & BC_DATA_SLOT) &&
               (wakeup_rpt_ptr->bcn[0] != mccbcsms_get_bcn()) )
      {
        srch_channel_config_changed = rxc_channel_config_changed = TRUE;
      }
      MSG_MED("Npri BCCH: config srch %d rxc %d",
               srch_channel_config_changed, rxc_channel_config_changed, 0);
    }
    #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  }
  #endif /* FEATURE_IS2000_REL_A_CC */

  /* Check whether PCH channel configuration */
  else if (active_f_csch_info.channel_monitor_mask & MCC_PCH)
  {
    /* Walsh channel changed */
    if (active_f_csch_info.pch_walsh_chan != cdma.pagech)
    {
      srch_channel_config_changed = TRUE;
    }
    /* Data rate and code rate changed */
    if (active_f_csch_info.pch_data_rate != cdma.prat)
    {
      srch_channel_config_changed = rxc_channel_config_changed  = TRUE;
    }
  }

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  /* Sanity check against the sync up of the channel monitoring between MC and SRCH */
  if ((wakeup_rpt_ptr != NULL) && wakeup_rpt_ptr->srch_slept &&
      (wakeup_rpt_ptr->cur_slot_mask & BC_DATA_SLOT) &&
      ((active_f_csch_info.channel_monitor_mask & MCC_PRI_BCCH) != 0))
  {
    MSG_ERROR("Channel monitoring out of sync MC %d SRCH %d",
      active_f_csch_info.channel_monitor_mask, wakeup_rpt_ptr->cur_slot_mask, 0);
  }
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  /* ---------------------------------------------------------------
  ** For Fast RAHO or full wakeup, Searcher task already updated the
  ** channel monitoring requirement. So no need to notify drivers
  ** again for the change.
  ** --------------------------------------------------------------- */

  if (!mccidl_fast_raho)
  {

    #ifdef FEATURE_IS2000_REL_A_CC 
    /* Under these conditions, Search will send the chan_config: */
    /*   -- when Search has slept, or                            */
    /*   -- when the BC_DATA_SLOT wakeup                         */
    /* Otherwise, MC will send the chan_config to RXC.           */

    if (wakeup_rpt_ptr &&
        (wakeup_rpt_ptr->srch_slept ||
         (wakeup_rpt_ptr->cur_slot_mask & BC_DATA_SLOT))
       )
    {
      MSG_MED("Wakeup / BCSMS, SRCH config channels", 0, 0, 0);
    }
    else
    #endif /* FEATURE_IS2000_REL_A_CC */

    {
      #ifdef FEATURE_IS2000_REL_A_CC 
      if (rxc_channel_config_changed)
      {
        /* Notify Receive task of channel configuration changes */
        mccidl_send_rxc_chan_config_cmd();
      }

      if (srch_channel_config_changed)
      {
        /* Notify the Searcher Task about channel configuration changes */
        mccidl_send_srch_chan_config_cmd();
      }
      #endif /* FEATURE_IS2000_REL_A_CC */
    }
  }

  if (srch_channel_config_changed)
  {
    
    #ifdef FEATURE_IS2000_REL_A_CC
    /* Record active channel configuration for channel supervision operation */
    if (OVHD_CHAN_IS_BCCH)
    {
      active_f_csch_info.bcch_walsh_chan = cdma.bcch;
      active_f_csch_info.bcch_data_rate = cdma.brat;
      active_f_csch_info.bcch_code_rate = cdma.bcch_code_rate;
      active_f_csch_info.fccch_walsh_chan =
        cur_bs_ptr->csp.fccch_info.fccch_code_chan[cdma.fccch_id-1];
      active_f_csch_info.fccch_data_rate =
        cur_bs_ptr->csp.fccch_info.fccch_rate;
      active_f_csch_info.fccch_code_rate =
        cur_bs_ptr->csp.fccch_info.fccch_code_rate;
    }
    else
    #endif /* FEATURE_IS2000_REL_A_CC */

    {
      active_f_csch_info.pch_walsh_chan = cdma.pagech;
      active_f_csch_info.pch_data_rate = cdma.prat;
    }

    if (active_f_csch_info.channel_monitor_mask == MCC_PCH)
    {
      MSG_HIGH("MS is monitoring PCH", 0, 0, 0);
    }

    #ifdef FEATURE_IS2000_REL_A_CC
    else if ((active_f_csch_info.channel_monitor_mask & MCC_PRI_BCCH) &&
             (active_f_csch_info.channel_monitor_mask & MCC_FCCCH))
    {
      MSG_HIGH("MS is monitoring PRI BCCH and FCCCH", 0, 0, 0);
    }
    else if (active_f_csch_info.channel_monitor_mask & MCC_PRI_BCCH)
    {
      MSG_HIGH("MS is monitoring PRI BCCH", 0, 0, 0);
    }
    #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
    else if ((active_f_csch_info.channel_monitor_mask & MCC_NON_PRI_BCCH) &&
             (active_f_csch_info.channel_monitor_mask & MCC_FCCCH))
    {
      MSG_HIGH("MS is monitoring Non-PRI BCCH and FCCCH", 0, 0, 0);
    }
    else if (active_f_csch_info.channel_monitor_mask & MCC_NON_PRI_BCCH)
    {
      MSG_HIGH("MS is monitoring Non-PRI BCCH", 0, 0, 0);
    }
    #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
    else if (active_f_csch_info.channel_monitor_mask & MCC_FCCCH)
    {
      MSG_HIGH("MS is monitoring FCCCH", 0, 0, 0);
    }
    #endif /* FEATURE_IS2000_REL_A_CC */
    else
    {
      /* Should never happen */
      MSG_ERROR("Invalid channel monitor mask %d",
        active_f_csch_info.channel_monitor_mask, 0, 0);
    }
  }

} /* mccidl_supervise_channel_monitoring */

/*===========================================================================

FUNCTION MCCIDL_CHECK_GSR_IGNORED

DESCRIPTION
  This function returns the status of gsr_ignored \ egsr_ignored.

DEPENDENCIES
  None.

RETURN VALUE
  true      if gsr_ignored/egrs_ignored is TRUE and there is a valid GSRM/EGSRM
  false     otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean mccidl_check_gsr_ignored(void)
{
  return (gsr_ignored && GSR_MSG_IS_RECVD) || (egsr_ignored && EGSR_MSG_IS_RECVD);
}

/*===========================================================================

FUNCTION MCCIDL_SAVE_MSG_RXED_CHAN

DESCRIPTION
  This function saves what channel message was received on.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mccidl_save_msg_rxed_chan(void)
{
  msg_rxed_chan.cdmach = cdma.cdmach;

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    msg_rxed_chan.chan_type = CAI_F_CCCH;
    msg_rxed_chan.chan_id = cdma.fccch_id;
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  {
    msg_rxed_chan.chan_type = CAI_PCH;
    msg_rxed_chan.chan_id = cdma.pagech;
  }
} /* mccidl_save_msg_rxed_chan */

/*===========================================================================

FUNCTION MCCIDL_ASSERT_SLEEP_OK

DESCRIPTION
  This function asserts the mccidl_sleep_ok signal.

  If two events occur which both trigger a sleep, the MCC task may go
  to sleep before the second trigger is removed from the queue.  Any
  sleep command which was generated before the "sleep_ok_time" is
  ignored to prevent this lingering sleep trigger from causing the
  mobile to go to sleep immediately after waking up.

DEPENDENCIES
  None.

RETURN VALUE
  true      sleep ok was asserted.
  false     sleep ok was not asserted.

SIDE EFFECTS
  None

===========================================================================*/

boolean mccidl_assert_sleep_ok
(
  qword rpt_time
    /* Time the sleep trigger was issued */
)
{
  boolean status = FALSE;
  if (qw_cmp(rpt_time, mccidl_sleep_ok_time) < 0)
  {
    MSG_HIGH("SleepOk not set! T_rpt=%x, T_ok=%x",
      (word) qw_lo(rpt_time), (word) qw_lo(mccidl_sleep_ok_time), 0);
  }
  else
  {
    status = mccidl_sleep_ok = TRUE;
    /* Assert the sleep_ok signal. */
  }
  return status;

} /* mccidl_assert_sleep_ok */

/*===========================================================================

FUNCTION MCCIDL_CHECK_OVHD

DESCRIPTION
  This function sets up for the transition to Update Overhead Information
  State of System Access State.  It stores the pending System Access State
  in cdma.sa_tc_state.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state to be processed.

SIDE EFFECTS
  Stores the state we are going to after our overhead messages are current
  in cdma.sa_tc_state.

===========================================================================*/

word mccidl_check_ovhd
(
  word curr_state,
    /* Current Idle substate */
  word sa_tc_state
    /* System Access or Traffic substate to enter after overhead messages are current */
)
{
  MSG_HIGH("Checking Overhead for SA 0x%x", sa_tc_state, 0 ,0);

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
  #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

  {
    /* ---------------------------------------------------------------
    ** If the phone is locked the mobile is prohibited from performing
    ** a system access; remain in Idle state.
    ** --------------------------------------------------------------- */
    if ( sd_misc_is_cdma_locked() )
    {
      return (curr_state);
    }

    /* -----------------------------------------
    ** Set timer for receiving overhead messages
    ** in the Update Overhead Information State.
    ** ----------------------------------------- */
    (void) rex_set_timer(&mcc_state_timer, cai_tmo.t41m);

    /* -------------------------------------------
    ** Send a message to the Receive task to start
    ** using the System Access Fade timeout.
    ** ------------------------------------------- */
    mcc_rx_buf.acc_fade.hdr.command = RXC_ACC_FADE_F;
    mcc_rxc_cmd(&mcc_rx_buf);

    /* Mark that Receive task has been told to use Access timer */
    access_fade_timer_in_use = TRUE;

    if (((sa_tc_state != CDMA_PAGE_RES)  /*lint !e641 */

         #ifdef FEATURE_IS2000_REL_D
#error code not present
         #endif /* FEATURE_IS2000_REL_D */
       ))
    {
      /* -------------------------------------------------------------------
      ** Force the mobile to get latest Access Parameter Message or Enhanced
      ** Access Parameter Message.
      ** ------------------------------------------------------------------- */
      CLR_ACC_MSG_SEQ();
    }

    #if defined(FEATURE_IS2000_REL_A_CC) && defined(FEATURE_AUTH)
    if ( OVHD_CHAN_IS_BCCH && cur_bs_ptr->csp.sending_a41_rand
         #ifdef FEATURE_IS2000_REL_D
#error code not present
         #endif /* FEATURE_IS2000_REL_D */
       )
    {
      /* -------------------------------------------------------
      ** Force the mobile to get latest ANSI41 Rand Message.
      ** Even for DCA cases, we need it for voice privacy setup.
      ** ------------------------------------------------------- */
      cur_bs_ptr->rx.a41randm_rx = FALSE;
    }
    #endif /* FEATURE_IS2000_REL_A_CC && FEATURE_AUTH */

    /* --------------------------------------------------------------
    ** To receive the latest Access Parameter Msg or ANSI41 Rand Msg,
    ** MS may need to switch from FCCCH to BCCH. So, peform channel
    ** monitoring supervision.
    ** -------------------------------------------------------------- */
    mccidl_supervise_channel_monitoring(curr_state, NULL, FALSE);
  }

  /* ----------------------------------------------------------------------
  ** cdma.sa_tc_state serves as the origination indication or page response
  ** indication or Other wireless technology -> 1x handover while in Update
  ** Overhead Information Substate.
  ** ---------------------------------------------------------------------- */
  cdma.sa_tc_state = sa_tc_state;

  if (curr_state != CDMA_IDLE_INIT) /*lint !e641 */
  {
    return (CDMA_OVHD); /*lint !e641 */
  }
  else
  {
    return (curr_state);
  }

} /* end mccidl_check_ovhd() */

/*===========================================================================
FUNCTION MCCIDL_INIT_AEHO_INFO

DESCRIPTION
  This function sets up the Access Entry Handoff state information in MC and
  also informs the SRCH task about the AEHO state.

DEPENDENCIES
   The NGHBR_SET_ENTRY_INFO needs to be enabled by the BS in ESPM and
   the MS should receive all the overhead messages for the current BS.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccidl_init_aeho_info
(
  word sa_tc_state 
    /* System access or Traffic channel substate to enter after AEHO */
)
{
  if ( cdma.ho_state == CDMA_AEHO )
  {
    MSG_HIGH("Already init AEHO for SA 0x%x", sa_tc_state, 0, 0);
  }
  else
  {
    MSG_HIGH("Performing AEHO init for SA 0x%x", sa_tc_state, 0 ,0);

    /* Command Search to find the strongest pilot */
    mcc_srch_buf.hdr.cmd = SRCH_IDLE_FIND_STRONG_PILOT_F;
    MSG_HIGH("Sending SRCH_IDLE_FIND_STRONGEST_PILOT_F command to Searcher...", 0, 0, 0);
  
    mcc_srch_cmd(&mcc_srch_buf);

    /* Set the current handoff state to AEHO */
    cdma.ho_state = CDMA_AEHO;
  }

  /* Retain the System access or Traffic channel substate */
  cdma.ho_sa_tc_state = sa_tc_state;

} /* mccidl_init_aeho_info */

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

/*===========================================================================

FUNCTION MCCIDL_PROC_HS_CMD

DESCRIPTION
  This function processes commands received from the handset while in the
  Idle State or Update Overhead Information Substate.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_proc_hs_cmd
(
  mc_msg_type *cmd_ptr,
    /* pointer to message received from the handset */
  word new_state
    /* next state to be processed */
)
{
  uint8 call_id_index;       /* loop variable */
  sd_ss_act_e_type ss_action;
#ifdef FEATURE_OPERATIONAL_MODE
  sd_ss_act_s_type ss_act_param;
#endif

  switch ( cmd_ptr->hdr.cmd )
  {
    case MC_ORIGINATION_F:
    {
      MSG_MED( "Rxed Orig Cmd - SO = %d",
                cmd_ptr->origination.service_option, 0, 0 );

      cmd_ptr->hdr.status = MC_DONE_S;          /* Default, unless bad */

      /*lint -e641*/
      if ( ( cdma.sa_tc_state != CDMA_IDLE ) 
           && ( cdma.sa_tc_state != CDMA_PAGE_RES )
           && ( cdma.sa_tc_state != CDMA_REG ) 
           && ( cdma.sa_tc_state != CDMA_MSG )
           #if defined(FEATURE_IS2000_REL_D) || defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
#error code not present
           #endif /* FEATURE_IS2000_REL_D || FEATURE_UMTS_1X_HANDOVER_1XMSM */
         )
      /*lint +e641*/
      {
        cmd_ptr->hdr.status = MC_BADCMD_S;
        MSG_MED("Cann't orig in UPD_OVHD", 0, 0, 0);
        mcc_send_orig_failure( CM_CALL_ORIG_ERR_HOLD_ORIG,
          cmd_ptr->origination.call_id, CM_CALL_ORIG_ERR_LAYER_OTHER );

        if (cdma.sa_tc_state != CDMA_ORIG) /*lint !e641 */
        {
          /* Don't reset call_orig flag if it's already in orig */
          cdma.call_orig = FALSE;
        }

        /* Clean up command processing */
        cmd_done (&cmd_ptr->hdr.cmd_hdr);

        return (new_state);
      }

      if ( ( ( ( cdma.ho_state == CDMA_AEHO ) 
               || cdma.transceiver_request_pending 
             ) 
             && ( cdma.ho_sa_tc_state == CDMA_ORIG ) /*lint !e641 */
           )
           #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
           #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */
         )
      {
        cmd_ptr->hdr.status = MC_BADCMD_S;
        MSG_MED("Cann't orig, another call is in prog", 0, 0, 0);
        mcc_send_orig_failure(CM_CALL_ORIG_ERR_HOLD_ORIG,
                              cmd_ptr->origination.call_id,
                              CM_CALL_ORIG_ERR_LAYER_OTHER
                             );
        /* Clean up command processing */
        cmd_done (&cmd_ptr->hdr.cmd_hdr);

        return (new_state);
      }

      #if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
      /* ------------------------------------------------------------
      ** If we have an MO DBM pending, reject MO DBM first before
      ** handling the origination.
      ** ------------------------------------------------------------*/
      mcc_check_mo_dbm_status(CM_DBM_MO_ABORT);
      #endif /* FEATURE_CDSMS || FEATURE_DBM */

      /* Do not originate if we are locked or if the requested service option
         is invalid. */
      if ( sd_misc_is_cdma_locked() ||
           (
             #ifdef FEATURE_EVRC_SO_MGMT
             ( cmd_ptr->origination.service_option != CAI_SO_VOICE_WILDCARD ) &&
             #endif /* FEATURE_EVRC_SO_MGMT */
             !snm_so_is_enabled(cmd_ptr->origination.service_option)
           )
         )
      {
        /* ---------------------------------------------------------------
        ** If the phone is locked the mobile is prohibited from performing
        ** a system access; reject command and remain in Idle state.
        ** For now, we don't take special action to distinguish LOCK and
        ** SNM complaint.
        ** --------------------------------------------------------------- */
        MSG_HIGH("Can't originate SO %d", cmd_ptr->origination.service_option, 0, 0);

        mcc_send_orig_failure( CM_CALL_ORIG_ERR_ABORT,
                               cmd_ptr->origination.call_id,
                               CM_CALL_ORIG_ERR_LAYER_OTHER );
        cdma.call_orig = FALSE;

        /* Clean up command processing */
        cmd_done (&cmd_ptr->hdr.cmd_hdr);

        return (new_state);
      }

      /* ***********************************
      ** Get here, proceed with origination. 
      ** ***********************************/

      #ifdef FEATURE_OTASP
      /* ----------------------------------------
      ** Special processing for OTASP origination
      ** ---------------------------------------- */
      if (cmd_ptr->origination.otasp_activation)
      {
        if (!otasp_is_otasp_allowed())
    	{
	  cmd_ptr->hdr.status = MC_BADCMD_S;
          mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                                cmd_ptr->origination.call_id,
                                CM_CALL_ORIG_ERR_LAYER_L3
                        	);
	  cdma.call_orig = FALSE;
          break;
	}
	      
        #ifdef FEATURE_UIM_RUIM

        #ifdef FEATURE_UIM_RUN_TIME_ENABLE 
        /* Determine if the R-UIM is available for this NAM */
        if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
        #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
        {
          /* Set the NAM lock state in the R-UIM */
          otasp_set_ruim_nam_lock_state(USER_INIT_SRV_PROV);
        }
        #endif /* FEATURE_UIM_RUIM */
      }
      #endif /* FEATURE_OTASP */

      if (new_state == CDMA_IDLE_INIT) /*lint !e641 */
      {
        cmd_ptr->hdr.status = MC_BADCMD_S;
        /* ----------------------------
        ** Also tell CM that no service
        ** ---------------------------- */
        mcc_send_orig_failure(CM_CALL_ORIG_ERR_HOLD_ORIG,
                              cmd_ptr->origination.call_id,
                              CM_CALL_ORIG_ERR_LAYER_OTHER
                             );
        cdma.call_orig = FALSE;
      }
      else
      {
        /* -------------------------
        ** User is initiating a call
        ** ------------------------- */
        mcc_ms_orig(&cmd_ptr->origination);

        #ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
        #endif /* FEATURE_GPSONE_NEW_L2_ACK */

        #if (defined (FEATURE_HDR_HANDOFF) && defined(FEATURE_IS2000_REL_A))
#error code not present
        #endif /* FEATURE_HDR_HANDOFF && FEATURE_IS2000_REL_A */

        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */

        if (cdma.sa_tc_state != CDMA_IDLE)
        {
          if (cdma.sa_tc_state == CDMA_REG) /*lint !e641 */
          {
            /* Report that this Origination Message is causing us to 
               abandon a Registration Message. */
            mclog_report_registration_suppressed(cdma.reg_type);
          }

          /** In this case, we have already obtained a transceiver resource.
           ** So here it will be safe to single point wait an upgrade of the 
           ** resource.
           */
          if (cdma.sa_tc_state != CDMA_ORIG) /*lint !e641 */
          {
            if ( mcc_wait_transceiver_resource(SRCH_HIGH_PERF_ACCESS, 
                 SRCH_ACCESS_MOBILE_ORIG_IMMED_TYPE) == 
                 MCC_TRANSCEIVER_RESOURCE_LOST )
            {
              /* -------------------------------------------------------------
              ** Searcher requested system restart. Need to do system exit .
              ** Inform system determination of system lost and determines the
              ** next state to be processed based on the returned SS-Action.
              ** ------------------------------------------------------------- */
              if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                      sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_ADVISE_UNLOCK_RF, NULL ) )
                 )
              {
                mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
                new_state = CDMA_EXIT;
                return new_state;
              }
            }

            /* -------------------------------------------------------
            ** We allow the origination to override a page response or
            ** registration access.  If we were going to CDMA_PAGE_RES
            ** or CDMA_REG states, go to CDMA_ORIG instead.
            ** ------------------------------------------------------- */
            cdma.sa_tc_state = CDMA_ORIG; /*lint !e641 */
          }
        }
        else
        {
          /** If transceiver resource is available or if the request is 
           ** pending, send this request to update the transceiver request
           ** parameters. If the transceiver resource is not available,
           ** send this request to obtain one. */
          mcc_request_transceiver_resource(SRCH_HIGH_PERF_ACCESS, 
            SRCH_ACCESS_MOBILE_ORIG_IMMED_TYPE, CDMA_ORIG); /*lint !e641 */
        }

        #ifdef FEATURE_OTASP
        if (cmd_ptr->origination.otasp_activation)
        {
          cdma.otasp_call = USER_INIT_SRV_PROV;          
        }
        #endif /* FEATURE_OTASP */
      }

      break;
    }

    case MC_END_F:
    {
      /* ----------------------------------------
      ** If an origination attempt is underway...
      ** ---------------------------------------- */

      cmd_ptr->hdr.status = MC_DONE_S;  /* Default, unless bad */

      if ( cdma.call_orig

           #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
           #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */
         )
      {
        /* Loop through all the call_ids in the command.  If one of the
           call_ids does not match with cdma.call_id, return MC_BADPARM_S 
           to caller */
        for ( call_id_index = 0; call_id_index < cmd_ptr->end.num_call_ids; 
              call_id_index++ ) 
        {
          if ( cmd_ptr->end.call_ids[call_id_index] == cdma.call_id ) 
          {
            cdma.call_orig = FALSE;

            #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
            #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

            /* ---------------------------------------
            ** Inform system determination of Release. 
            ** --------------------------------------- */
            if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                    sd_ss_ind_cdma_opr_release(NULL) ) )
            {
              mcc_exit.exit = MCC_RELEASE;
              new_state = CDMA_EXIT;
            }
            else
            {
              if ( cdma.transceiver_request_pending )
              {
                cdma.transceiver_request_pending = FALSE;

                /* Send SRCH_IDLE_F command to bring it back to Idle state */
                mccidl_send_srch_idle_cmd();
              }

              /* Back to IDLE_INIT and restart BS insanity timer since overhead 
                 message processing was skipped while UMTS -> 1x handover is in 
                 progress. */
              new_state = CDMA_IDLE_INIT; /*lint !e641 */
              mccidl_set_bs_insanity_timer();

              #ifdef FEATURE_OTASP
              /* Reset OTASP call status */
              cdma.otasp_call = NO_SRV_PROV;
              #endif
            }

            cmd_ptr->hdr.status = MC_DONE_S;

            /* Events report: Call release request */
            event_report(EVENT_CALL_RELEASE_REQUEST);

            /* Only single origination is supported in Idle. As soon a match
               on call_id is identified, processing is done. */
            break;
          }
          else 
          {
            MSG_ERROR("Call_id %d from CM mismatched with current call!", 
                      cmd_ptr->end.call_ids[call_id_index], 0, 0);
            cmd_ptr->hdr.status = MC_BADPARM_S;
          }
        } /* end for (call_id_index) */
      }
      else
      {
        ERR( "Invalid  cmd %d",cmd_ptr->hdr.cmd,0, 0 );
        cmd_ptr->hdr.status = MC_BADPARM_S;
      }

      break;
    }

    case MC_MO_DBM_F:
    {
      #if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
      /* ----------------------------------------------------------
      ** If we are in Update Overhead Info substate or AEHO, SMS
      ** has to yield till other access is over or AEHO processing
      ** is over.
      ** ---------------------------------------------------------*/
      if ( ( cdma.sa_tc_state != CDMA_IDLE ) || ( cdma.ho_state == CDMA_AEHO )
           || ( cdma.transceiver_request_pending )
         )
      {
        MSG_MED("DBM rxed in UPD_OVHD or AEHO", 0, 0, 0);
        mcc_send_mo_dbm_status(cmd_ptr->mo_dbm.burst_type,
          CM_DBM_MO_HOLD_ORIG);

        break;
      }

      /* ------------------------------------------------------------
      ** We should not receive another MO DBM when one is pending.
      ** ------------------------------------------------------------*/
      if (cdma.dbm_wait_for_l2_ack)
      {
        /* In stead of err_fatal, we just reject the current request.
         * CM is expected to only send one DBM origination at a time.
         * but in case it sends a SMS and then a PD, for example, we
         * should reject the PD request. */
        MSG_ERROR("DBM received while in progress %d %d",
                  cmd_ptr->mo_dbm.burst_type, cdma.dbm_wait_for_l2_ack, 0);
        mcc_send_mo_dbm_status(cmd_ptr->mo_dbm.burst_type, CM_DBM_MO_ABORT);

        break;
      }
      #endif /* FEATURE_CDSMS || FEATURE_DBM */

      if ( sd_misc_is_cdma_locked() )
      {
        #if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
        /* We are locked, reject the MO DBM */
        MSG_HIGH( "Rejecting MO DBM request CDMA_IDLE LOCKED.", 0, 0, 0 );
        mcc_send_mo_dbm_status( cmd_ptr->mo_dbm.burst_type,
                                CM_DBM_MO_L2_ACK_FAILURE );
        #endif /* FEATURE_CDSMS || FEATURE_DBM */

        break;
      }

      MSG_MED ( "Received Data Burst - SMS mobile-origination", 0, 0, 0 );

      #if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
      if (new_state == CDMA_IDLE_INIT) /*lint !e641 */
      {
        cmd_ptr->hdr.status = MC_BADCMD_S;
          /* Notify CM about the race condition */
        mcc_send_mo_dbm_status(cmd_ptr->mo_dbm.burst_type,
          CM_DBM_MO_HOLD_ORIG);
      }
      else
      {
        /* ----------------------------------------------------------
        ** We've received a mobile orig DBM from CM SMS or PD
        ** Since L2 ACK is always required for SMS or PD delivered on
        ** Access Channel, we also use this flag to indicate that
        ** an SMS or PD has been received
        ** ---------------------------------------------------------*/
        cdma.dbm_wait_for_l2_ack = TRUE;

        if ( cmd_ptr->mo_dbm.dbm_buf_ptr != NULL )
        {
          cdma.mo_dbm_type = cmd_ptr->mo_dbm.burst_type;
          cdma.dbm_buf_ptr = cmd_ptr->mo_dbm.dbm_buf_ptr;
        }
        else
        {
          ERR_FATAL( "NULL SMS pointer", 0, 0, 0);  
        }
      #endif /* defined(FEATURE_CDSMS) || FEATURE_DBM */

        /* user is initiating a data burst transmit */
        mcc_ms_sms(cmd_ptr);

        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */

        if (cdma.sa_tc_state != CDMA_IDLE)
        {
          if (cdma.sa_tc_state == CDMA_REG) /*lint !e641 */
          {
            /* Report that this Data Burst Message is causing us to 
               abandon a Registration Message. */
            mclog_report_registration_suppressed(cdma.reg_type);
          }

          /** Since CDMA_MSG and CDMA_REG are using low performance required
           * resource, here we do not sent another resource request.
           */
             
          /* -------------------------------------------------------
          ** We allow the SMS to override a page response or
          ** registration access.  If we were going to CDMA_PAGE_RES
          ** or CDMA_REG states, go to CDMA_MSG instead.
          ** For other SA substates:
          ** 1. SMS is deferred for Order Response.
          ** 2. SMS is not going to be received after Orig.
          ** 3. SMS is not going to be received while in CDMA_MSG.
          ** ------------------------------------------------------- */
          cdma.sa_tc_state = CDMA_MSG; /*lint !e641 */
          cdma.sa_msg_tx_msg = CAI_AC_BURST_MSG;
        }
        else
        {
          /* set the message being transmitted to be CAI_AC_BURST_MSG */
          cdma.sa_msg_tx_msg = CAI_AC_BURST_MSG;
          /* Request the transceiver resource */
          mcc_request_transceiver_resource(SRCH_LOW_POWER_ACCESS, 
            SRCH_ACCESS_MOBILE_ORIG_TYPE, CDMA_MSG); /*lint !e641 */
        }

      #if defined(FEATURE_CDSMS) || defined (FEATURE_DBM)
      }
      #endif /* FEATURE_CDSMS || FEATURE_DBM */

      cmd_ptr->hdr.status = MC_DONE_S;

      break;
    }

    #ifdef FEATURE_BROADCAST_SMS
    case MC_BCSMS_ENABLE_F:
    {
      MSG_MED( "Rxed BCSMS_ENABLE = %d in IDLE",
                cmd_ptr->bcsms_enable.enable,0,0 );
      mccbcsms_enable(cmd_ptr->bcsms_enable.enable);
      cmd_ptr->hdr.status = MC_DONE_S;

      break;
    }
    #endif /* FEATURE_BROADCAST_SMS */

    case MC_CDMA_PROT_DEACT_CMD_F:
    {
      mcc_exit.mmoc_trans_id = cmd_ptr->deact_prot.info.trans_id;

      if ( ( cmd_ptr->deact_prot.info.reason == PROT_DEACT_PWR_DOWN ) ||
           ( cmd_ptr->deact_prot.info.reason == PROT_DEACT_LPM ) ||
	   ( cmd_ptr->deact_prot.info.reason == PROT_DEACT_SUBSC_CHGD &&
	     otasp_is_successful_commit())
         )
      {
        /* --------------------------------------------------------------
        ** If we haven't gotten through IDLE_INIT substate then we really
        ** haven't acquired the system yet, so we don't bother attempting
        ** a powerdown registration.
        ** -------------------------------------------------------------- */
        cdma.reg_type = mccreg_idle_powerdown();

        if ( ( cdma.reg_type == CAI_POWERDOWN_REG ) &&
             ( new_state != CDMA_IDLE_INIT ) && /*lint !e641 */
             !sd_misc_is_cdma_locked()
           )
        {
          #ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
          #endif /* FEATURE_GPSONE_NEW_L2_ACK */

          /* Need to do a powerdown registration */
          attempting_powerdown_reg = TRUE;

          cdma.powerdown_reg_type = CDMA_NORMAL;

          if ( cmd_ptr->deact_prot.info.reason == PROT_DEACT_LPM )
          {
            cdma.powerdown_reg_type = CDMA_LPM;
          }
          
          #ifdef FEATURE_IS2000_REL_D
#error code not present
          #endif /* FEATURE_IS2000_REL_D */

          if (cdma.sa_tc_state == CDMA_IDLE)
          {
            /* to make sure the power down registration type will not be overwritten, 
               we set the following flag */
            mccidl_reg_required = TRUE;

            /** Request the transceiver resource */
            mcc_request_transceiver_resource(SRCH_LOW_POWER_ACCESS, 
              SRCH_ACCESS_MOBILE_ORIG_TYPE, CDMA_REG); /*lint !e641 */
          }
          else
          {

            /** Since CDMA_REG uses low performance required resources, we don't need
             * send a new resource request here. What currently we have will be
             * good enough.
             */

            /* ------------------------------------------------
            ** We are already in CDMA_OVHD, so we don't want to
            ** do any of the processing in mccidl_check_ovhd().
            ** So just change where we are going to.
            ** ------------------------------------------------ */
            cdma.sa_tc_state = CDMA_REG; /*lint !e641 */
          }
        }
        else
        {
          /* powerdown registration is not required */
          mcc_exit.exit = mc_map_deact_to_mcc_exit_reason(
                            cmd_ptr->deact_prot.info.reason );
          new_state = CDMA_EXIT;
        }
      }
      else
      {
        /* Deactivation is not because of power down or LPM. */
        mcc_exit.exit = mc_map_deact_to_mcc_exit_reason(
                          cmd_ptr->deact_prot.info.reason );
        new_state = CDMA_EXIT;
      }

      cmd_ptr->hdr.status = MC_DONE_S;

      break;
    }

    case MC_CDMA_PROT_GEN_CMD_F:
    {
      /* Process the MMOC generic protocol command */
      ss_action = mmoc_proc_prot_gen_cmd( TRUE, TRUE, &cmd_ptr->gen_cmd.info );

#ifdef FEATURE_OPERATIONAL_MODE
      if (ss_action == SD_SS_ACT_PWR_SAVE)
      {
        /* Call into SD to determine the action.  Note that sd_ss_act_get
           will only return the correct action for some MMOC generic commands;
           so just call it if this is a power save. */
        sd_ss_act_get(&ss_act_param);
      }

      if ((ss_action == SD_SS_ACT_PWR_SAVE) && 
          (ss_act_param.prm.pwr_save.reas == SD_SS_PWR_SAVE_REAS_PWR_DOWN))
      {
        /* 1X is being deactivated.  Determine if we need to perform 
           powerdown registration. */
        cdma.reg_type = mccreg_idle_powerdown();

        /* --------------------------------------------------------------
        ** If we haven't gotten through IDLE_INIT substate then we really
        ** haven't acquired the system yet, so we don't bother attempting
        ** a powerdown registration.
        ** -------------------------------------------------------------- */
        if ( ( cdma.reg_type == CAI_POWERDOWN_REG ) &&
             ( new_state != CDMA_IDLE_INIT ) && /*lint !e641 */
             !sd_misc_is_cdma_locked()
           )
        {
#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

          /* Need to do a powerdown registration */
          attempting_powerdown_reg = TRUE;

          /* 1X is shutting down */
          cdma.powerdown_reg_type = CDMA_SHUTDOWN;

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

          if (cdma.sa_tc_state == CDMA_IDLE)
          {
            /* to make sure the power down registration type will not be overwritten, 
               we set the following flag */
            mccidl_reg_required = TRUE;

            /** Request the transceiver resource */
            mcc_request_transceiver_resource(SRCH_LOW_POWER_ACCESS, 
                                             SRCH_ACCESS_MOBILE_ORIG_TYPE, CDMA_REG); /*lint !e641 */
          }
          else
          {
            /** Since CDMA_REG uses low performance required resources, we don't need
             * send a new resource request here. What currently we have will be
             * good enough.
             */

            /* ------------------------------------------------
            ** We are already in CDMA_OVHD, so we don't want to
            ** do any of the processing in mccidl_check_ovhd().
            ** So just change where we are going to.
            ** ------------------------------------------------ */
            cdma.sa_tc_state = CDMA_REG; /*lint !e641 */
          }
        }
        else
        {
          /* Either we can't do powerdown registration, or we don't
             need to. */
          mcc_exit.exit = MCC_SHUTDOWN;
          new_state = CDMA_EXIT;
        }
      }
      else
#endif /* FEATURE_OPERATIONAL_MODE */
      {
        /* Determine if we need to exit. */
        if (!mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED, ss_action ))
        {
          mcc_exit.exit = 
            mc_map_gen_cmd_to_mcc_exit_reason(&cmd_ptr->gen_cmd.info );
          new_state = CDMA_EXIT;
        }
      }

      cmd_ptr->hdr.status = MC_DONE_S;

      break;
    }

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
    #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

	#ifdef FEATURE_MVS_MIGRATE
#error code not present
    #endif /* FEATURE_MVS_MIGRATE */


    default:
    {
      ERR( "Invalid UI cmd %d", cmd_ptr->hdr.cmd, 0, 0 );
      cmd_ptr->hdr.status = MC_BADPARM_S;
      break;
    }
  }

  /* clean up command processing */
  cmd_done (&cmd_ptr->hdr.cmd_hdr);

  return (new_state);
 
} /* mccidl_proc_hs_cmd */

#if (defined(FEATURE_HIGH_PSIST) && defined (FEATURE_PASSPORT_MODE))
/*===========================================================================

FUNCTION MCCIDL_UPDATE_PSIST_INFO

DESCRIPTION
  This function sends a new psist report to CM if necessary.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_update_psist_info
(
 cai_tx_channel_id_type chan,
 /* one of {CAI_ACH, CAI_EACH} to indicate that the parm_ptr is a ap or eap */
 void *parm_ptr
)
{
  cm_psist_e_type psist_value_to_report = CM_PSIST_LOW;
  caii_accparm_type *ap_ptr;        /* Pointer to current Access Parms Msg */
  #ifdef FEATURE_IS2000_REL_A_CC
  caii_en_ac_parm_type *eap_ptr;      /* Pointer to current Enhanced Access Params Msg */
  #endif /* FEATURE_IS2000_REL_A_CC */

  if(mcc_passport_mode_is_jcdma())
  {
    if (chan == CAI_ACH)
    {
      ap_ptr = (caii_accparm_type *) parm_ptr;
      /* Check if the psist is greater than the threshold.  Note that since KDDI's
         threshold is 24, accolc >= 10 cannot exceed the threshold, since the max
         psist value is 7 for psist_10, etc.  */
      if ((cdma.accolc < 10) && (ap_ptr->psist_0_9 >= KDDI_HIGH_PSIST_THRESHOLD))
      {
        psist_value_to_report = CM_PSIST_HIGH;
      }
    }

    #ifdef FEATURE_IS2000_REL_A_CC
    else if (chan == CAI_EACH)
    {
      eap_ptr = (caii_en_ac_parm_type *) parm_ptr;
      /* Check if the psist is greater than the threshold.  Note that since KDDI's
         threshold is 24, accolc >= 10 cannot exceed the threshold, since the max
         psist value is 7 for psist_10, etc.  */
      if ((cdma.accolc < 10) && (eap_ptr->psist_0_9_each >= KDDI_HIGH_PSIST_THRESHOLD))
      {
        psist_value_to_report = CM_PSIST_HIGH;
      }
    }
    #endif /* FEATURE_IS2000_REL_A_CC */
  }

  /* Check if the new value is different from what we previously reported */
  if (psist_value_to_report != mccidl_last_reported_psist_value)
  {
    /* We need to send CM a new psist report */
    mcc_send_cm_psist_info(psist_value_to_report);
    /* Remeber what we just reported */
    mccidl_last_reported_psist_value = psist_value_to_report;
  }

} /* mccidl_update_psist_info */
#endif /* FEATURE_HIGH_PSIST && FEATURE_PASSPORT_MODE */

/*===========================================================================

FUNCTION MCCIDL_BUILD_TEMP_NL

DESCRIPTION
  If possible, this procedure constructs a temporary neighbor list to be
  used until a real one can be received.  This list is only referenced
  by call processing, it is not sent to the searcher.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -esym(715, nghbr_bcch_supported) */
/*lint -esym(715, old_bs_td_mode) */
/*lint -esym(715, old_bs_td_power_level) */
void mccidl_build_temp_nl
(
  cdma_bs_type *old_bs_ptr,
    /* Pointer to old BS info */
  byte old_bs_td_mode,
    /* Old BS td mode */
  byte old_bs_td_power_level,
    /* Old BS td power level */
  byte nghbr_config,
    /* Neighbor config used for the idle handoff */
  boolean nghbr_bcch_supported 
    /* whether BCCH is supported on the new BS */
)
{
  boolean added_old_bs = FALSE;
    /* Flags if the old BS has been added to the temp nl */
  word i; /* Loop counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------------------------------------
  ** Four reasons why we won't build a temp nl are:
  ** 1.  We overwrote the old BS info with the new
  ** 2.  The handoff was to a new system
  ** 3.  The old BS info doesn't have a nl
  ** 4.  The new BS info already has a nl
  ** 5.  The handoff was from a system monitoring PCH
         to a system monitoring BCCH
  ** ----------------------------------------------- */

  if ((old_bs_ptr == cur_bs_ptr) || (nghbr_config == 3) ||
      ((old_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_PC) && !old_bs_ptr->rx.nl_rx) ||
      (OVHD_CHAN_IS_PCH && cur_bs_ptr->rx.nl_rx)
      #ifdef FEATURE_IS2000_REL_A_CC
      || ((old_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_BCCH) && !old_bs_ptr->rx.unlm_rx)
      || (OVHD_CHAN_IS_BCCH && cur_bs_ptr->rx.unlm_rx)
      #endif /* FEATURE_IS2000_REL_A_CC */
      #ifdef FEATURE_IS2000_REL_A
      || ((old_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_PC) && nghbr_bcch_supported)
      #endif /* FEATURE_IS2000_REL_A */
     )
  {
    return;
  }

  /* ---------------------------------------------------------------------
  ** Mark we have a temporary neighbor list. It will be replaced with real
  ** one when it comes.
  ** --------------------------------------------------------------------- */

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    cur_bs_ptr->rx.unlm_rx = TRUE;
    cur_bs_ptr->unlm_config_msg_seq = TEMP_NL_SEQ;
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  {
    cur_bs_ptr->rx.nl_rx = TRUE; /* Mark we have a temporary neighbor list */
    cur_bs_ptr->nl_config_msg_seq = TEMP_NL_SEQ;
  }

  /* Assume same pilot increment same as for last base station */
  cur_bs_ptr->nl.pilot_inc = old_bs_ptr->nl.pilot_inc;

  cur_bs_ptr->nl.num_nghbr_pns = old_bs_ptr->nl.num_nghbr_pns;

  for (i=0; i < old_bs_ptr->nl.num_nghbr_pns; i++)
  {
    if ((old_bs_ptr->nl.nset[i].pilot_pn == cur_bs_ptr->pilot_pn) &&
        (!old_bs_ptr->nl.nset[i].freq_incl ||
         ((old_bs_ptr->nl.nset[i].band == cur_bs_ptr->band_class) &&
          (old_bs_ptr->nl.nset[i].freq == cur_bs_ptr->cdma_ch))))
    {
      /* ----------------------------------------------------------
      ** This is the entry we used for the handoff.  Replace it
      ** with a fake one which represents where we handed off from.
      ** ---------------------------------------------------------- */
      added_old_bs = TRUE;
      cur_bs_ptr->nl.nset[i].pilot_pn = old_bs_ptr->pilot_pn;
      cur_bs_ptr->nl.nset[i].config = nghbr_config; /* config is communitave */

      cur_bs_ptr->nl.nset[i].search_pri = 3;  /* Highest priority */
      if ((cur_bs_ptr->cdma_ch == old_bs_ptr->cdma_ch) &&
          (cur_bs_ptr->band_class == old_bs_ptr->band_class))
      {
        cur_bs_ptr->nl.nset[i].freq_incl = FALSE;
      }
      else
      {
        /* We initialize band and freq from the old base station */
        cur_bs_ptr->nl.nset[i].freq_incl = TRUE;
        cur_bs_ptr->nl.nset[i].band = old_bs_ptr->band_class;
        cur_bs_ptr->nl.nset[i].freq = old_bs_ptr->cdma_ch;
      }

      #ifdef FEATURE_IS2000_REL_A
      #ifdef FEATURE_IS2000_REL_A_CC
      if (old_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_BCCH)
      {
        cur_bs_ptr->nl.nset[i].bcch_supported = TRUE;
        if ((OVHD_CHAN_IS_PCH) || !old_bs_ptr->td_supported)
        {
          cur_bs_ptr->nl.nset[i].add_pilot_rec_incl = FALSE;
        }
        else
        {
          cur_bs_ptr->nl.nset[i].add_pilot_rec_incl = TRUE;
          cur_bs_ptr->nl.nset[i].nghbr_pilot_rec.rec0.td_mode = old_bs_td_mode;
          cur_bs_ptr->nl.nset[i].nghbr_pilot_rec.rec0.td_power_level =
            old_bs_td_power_level;
        }
      }
      else
      #endif /* FEATURE_IS2000_REL_A_CC */

      {
        cur_bs_ptr->nl.nset[i].bcch_supported = FALSE;
      }
      #endif /* FEATURE_IS2000_REL_A */

    }
    else
    {
      /* Use the same entry with some modifications */
      cur_bs_ptr->nl.nset[i] = old_bs_ptr->nl.nset[i];

      /* ----------------------------------------------------------------
      ** It is possible that this neighbor has the same number of
      ** frequencies or paging channels / forward common control channels
      ** as the current BS, but had a different number than the previous
      ** BS.  In this case the config value assigned will be greater than
      ** it should be. This is not optimal, but is the best we can do at
      ** this time.
      ** ---------------------------------------------------------------- */

      switch (nghbr_config)
      {

        #ifdef FEATURE_IS2000_REL_A_CC
        case 4:
          if (OVHD_CHAN_IS_BCCH)
          {
            /* ---------------------------------------------------------
            ** We know neighbors with config 0 have same broadcast
            ** control channel configuartion, but we know nothing about
            ** how many forward common control channels they have, so we
            ** assume config 4.
            ** --------------------------------------------------------- */
            if (cur_bs_ptr->nl.nset[i].config == 0)
            {
              cur_bs_ptr->nl.nset[i].config = 4;
            }
          }
          else
          {
            /* Shall never be here */
            MSG_ERROR("Invalid neighbor config %d", nghbr_config, 0, 0);
          }

          break;
        #endif /* FEATURE_IS2000_REL_A_CC */

        case 2:
          /* We know nothing about the neighbors. Assume they are all 2 */
          if (cur_bs_ptr->nl.nset[i].config != 3)
          {
            #ifdef FEATURE_IS2000_REL_A_CC
            if (OVHD_CHAN_IS_BCCH)
            { 
              if (cur_bs_ptr->nl.nset[i].config != 1)
              {
                cur_bs_ptr->nl.nset[i].config = 2;
              }
            }
            else
            #endif /* FEATURE_IS2000_REL_A_CC */

            {
              cur_bs_ptr->nl.nset[i].config = 2;
            }
          }
          break;

        case 1:
          #ifdef FEATURE_IS2000_REL_A_CC
          if (old_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_BCCH)
          {
            /* -----------------------------------------------------------
            ** We know neighbors with config 1 have same number of
            ** frequencies, but we know nothing about neighbors with other
            ** config.
            ** -------------------------------------------------------- */
            if (cur_bs_ptr->nl.nset[i].config != 1)
            {
              cur_bs_ptr->nl.nset[i].config = 3;
            }
          }
          else
          #endif /* FEATURE_IS2000_REL_A_CC */

          {
            /* --------------------------------------------------------
            ** We know neighbors with config 0 or 1 have same number of
            ** frequencies, but we know nothing about how many paging
            ** channels they have, so we assume config 1.
            ** -------------------------------------------------------- */
            if (cur_bs_ptr->nl.nset[i].config < 2)
            {
              cur_bs_ptr->nl.nset[i].config = 1;
            }
          }
          break;

        case 0:
          /* We have same config as prev BS, so all config values are good */
          break;

        default:
          MSG_ERROR("Invalid nghbr_config %d", nghbr_config, 0, 0);  
          break;
      }

      if (cur_bs_ptr->nl.nset[i].freq_incl)
      {
        if ((cur_bs_ptr->nl.nset[i].freq == cur_bs_ptr->cdma_ch) &&
            (cur_bs_ptr->nl.nset[i].band == cur_bs_ptr->band_class))
        {
          cur_bs_ptr->nl.nset[i].freq_incl = FALSE;
        }
      }
      else
      {
        if ((old_bs_ptr->cdma_ch != cur_bs_ptr->cdma_ch) ||
            (old_bs_ptr->band_class != cur_bs_ptr->band_class))
        {
        /* We assume we came from current band before we can support tri-mode */
          cur_bs_ptr->nl.nset[i].freq_incl = TRUE;
          cur_bs_ptr->nl.nset[i].band = old_bs_ptr->band_class;
          cur_bs_ptr->nl.nset[i].freq = old_bs_ptr->cdma_ch;
        }
      }
    }
  } /* end for */

  if (!added_old_bs)
  {
    /* -------------------------------------------------------------------
    ** We somehow handed off to a pilot not in the nl using config 0, 1, 2
    ** or 4.
    ** ------------------------------------------------------------------- */
    MSG_ERROR("Didn't find source BS in neighbor list", 0, 0, 0);

    #ifdef FEATURE_IS2000_REL_A_CC
    if (OVHD_CHAN_IS_BCCH)
    {
      cur_bs_ptr->rx.unlm_rx = FALSE;
    }
    else
    #endif /* FEATURE_IS2000_REL_A_CC */

    {
      cur_bs_ptr->rx.nl_rx = FALSE;
      cur_bs_ptr->rx.gnl_rx = FALSE;
    }
  }

} /* mccidl_build_temp_nl() */
/*lint +esym(715, nghbr_bcch_supported) */
/*lint +esym(715, old_bs_td_mode) */
/*lint +esym(715, old_bs_td_power_level) */

/*===========================================================================

FUNCTION MCCIDL_PROC_SYS_PARM

DESCRIPTION
  This procedure processes a received System Parameters Message as described
  in IS-95A section 6.6.2.2.1.  This procedure is used for Idle State and
  Update Overhead Information Substate of System Access State.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_proc_sys_parm
(
  word new_state
    /* Current state being processed */
)
{
  mccrxtx_cmd_type *rx_ptr;
    /* Pointer to message received after hashing to a new Paging Channel */
  dword sigs;
    /* Return signals from mcc_wait() call */
  byte pagech = 0;

  byte bs_p_rev;

  bs_info_id_type bs_info_id;
    /* Fields unqiuely identify BS */
  int i;

  static word prev_sid, prev_nid;

  #ifdef FEATURE_GPSONE
#error code not present
  #endif /* FEATURE_GPSONE */

  if ((new_state != CDMA_ORIG) && (new_state != CDMA_PAGE_RES)) /*lint !e641 */
  {                                              
    /*----------------------------------------------------------------------
    ** If the Mobile is not in the process of Access Probe Handoff or Access
    ** Handoff, the Mobile will update its Registration parameters SID, NID
    ** and REG_ZONE with the new base station's values. 
    **---------------------------------------------------------------------*/
    mccreg_set_idle_reg_info(&cur_bs_ptr->csp.sp);
  }
 
  if (!P_REV_IS_4_OR_GREATER)
  {

    #ifdef FEATURE_PASSPORT_MODE
    if (!mcc_passport_mode_is_jcdma() ||
        ((cdma.band_class != CAI_BAND_CLASS3) &&
         (cdma.band_class != CAI_BAND_CLASS6)))
    #endif /* FEATURE_PASSPORT_MODE */

    {
      if (mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, cdma.band_class))
      {
        /* For PREV < 4, if JSTD008 (PCS), 1 means ENLM will be sent 
        ** by BS.  For PREV < 4, if not JSTD008 (i.e. cellular) 
        ** this field not in msg.
        ** Initialize spm field for non-T53 phones if the band is 
        ** cellular and PREV < 4.  
        ** For PREV >= 4 or T53, do what the SPM says to do.  
        */
        cur_bs_ptr->csp.sp.ext_nghbr_list = 0;
      }
    }
  } 

  /* ---------------------------------------------------------------
  ** When the mobile station receives the System Parameters Message
  ** with the EXT_SYS_PARAMETER field set equal to '0', it indicates
  ** that the Extended System Parameters Message is current by
  ** setting EXT_SYS_PAR_MSG_SEQs to SPM's CONFIG_MSG_SEQs.
  ** --------------------------------------------------------------- */
  if (!cur_bs_ptr->csp.sp.ext_sys_parameter)
  {
    cur_bs_ptr->csp.esp.config_msg_seq = cur_bs_ptr->csp.sp.config_msg_seq;
    cur_bs_ptr->rx.esp_rx = TRUE;

    // This BS is a pre-IS 95B system, we will not be receiving a ESPM
    // fill in default P_REV info in the internal ESPM message buffer

    if ( cur_bs_ptr->rx.sy_rx )
    {
      // If Sync Channel Message was received, we can just copy from it
      cur_bs_ptr->csp.esp.p_rev = cur_bs_ptr->sy.p_rev;
      cur_bs_ptr->csp.esp.min_p_rev = cur_bs_ptr->sy.min_p_rev;
    }
    else
    {
      // If Sync Channel Message not received for this BS (must be idle handoff),
      // derive the P_REV from the band class.  Note that if we get an invalid
      // band class, the P_REV is set to 0.
      switch (mcc_get_group_for_band_class(cdma.band_class))
      {
        case MCC_CELLULAR_GROUP:
        {
          cur_bs_ptr->csp.esp.p_rev =
            cur_bs_ptr->csp.esp.min_p_rev = P_REV_IS95A;
          break;

        }
        case MCC_PCS_GROUP:
        {
          cur_bs_ptr->csp.esp.p_rev =
            cur_bs_ptr->csp.esp.min_p_rev = P_REV_JSTD008;
          break;
        }
        default:
        {
          ERR("Band Class Error, P_REV Unknown", 0,0,0);
          cur_bs_ptr->csp.esp.p_rev =
            cur_bs_ptr->csp.esp.min_p_rev = CAIX_P_REV_UNKNOWN;
        }
      }
    }

    #ifdef FEATURE_BROADCAST_SMS
    /* According to IS-95A, 6.6.2.2.1.1, set the bcast_index  */
    /* to MAX_SLOT_CYCLE_INDEX.                               */
    mccbcsms_proc_msg(CAI_SYS_PARM_MSG, cur_bs_ptr);
    #endif /*  FEATURE_BROADCAST_SMS */

    /* Set the IMSI_11_12 & MCC to the current values */
    cur_bs_ptr->csp.esp.imsi_11_12 = cdma.imsi_m_11_12;
    cur_bs_ptr->csp.esp.mcc = cdma.imsi_m_mcc;

    cur_bs_ptr->csp.ota_mcc         = 0x3FF;
    cur_bs_ptr->csp.ota_imsi_11_12  = 0x7F;

    if( cdma.imsi_type != CDMA_IMSI_M )
    {
      /* Copy IMSI_M into IMSI_O  */
      MSG_MED("Switching to IMSI_M",0,0,0);
      cdma.imsi_s1    = cdma.imsi_m_s1;
      cdma.imsi_s2    = cdma.imsi_m_s2;
      qw_equ( cdma.imsi_s, cdma.imsi_m_s );
      cdma.imsi_11_12 = cdma.imsi_m_11_12;
      cdma.mcc        = cdma.imsi_m_mcc;
      cdma.imsi_addr_num = cdma.imsi_m_addr_num;

      /* Set IMSI type flag to IMSI_M */
      cdma.imsi_type  = CDMA_IMSI_M;

      /* Re-calculate HASH_KEY (see IS-95B section 6.6.7.1) */
      cdma.imsi_hash_key = cdma.imsi_s1 + (((dword) cdma.imsi_s2) << 24);

      // Note: this key has to be remembered for QPCH operation
      // We can't generate the QPCH number here yet because the ESPM
      // may not have been received.
      cur_bs_ptr->imsi_hash_key = cdma.imsi_hash_key;

      /* Re-calculate page slot in case we don't go through idle_init */
      idle_pgslot = hash(cdma.imsi_hash_key, 2048,
                         (word) (6*(cdma.imsi_hash_key & 0xfff)));

      #ifdef FEATURE_HDR
#error code not present
      #endif /* FEATURE_HDR */

      /* Force to receive CDMA Channel List Msg and rehash CDMA
         Channel if necessary. Rehash of Paging channel will be
         performed in the following. Therefore, above hash key
         re-calculation need to be done before that.            */
      cur_bs_ptr->rx.cl_rx = FALSE;
      cur_bs_ptr->rx.ecl_rx = FALSE;

      /* Following two lines is to force the rehashing of the
         paging channel.  */
      cdma.pagech = PRI_PAGECH;
      cdma.page_chan_s = 1;
    }

    /*----------------------------------------------------------------
    ** Set P_REV to '00000011' for Band class 0 or PREV to '00000001'
    ** for Band class 1. Update the database so that other services
    ** can access the PREV information.
    **---------------------------------------------------------------*/
    switch (mcc_get_group_for_band_class(cdma.band_class))
    {
      case MCC_CELLULAR_GROUP:
      {
        bs_p_rev = P_REV_IS95A;

        // Update p_rev_in_use, bs_p_rev and report events if necessary
        mcc_update_bs_p_rev(bs_p_rev);

        break;

      }
      case MCC_PCS_GROUP:
      {
        bs_p_rev = P_REV_JSTD008;

        // Update p_rev_in_use, bs_p_rev and report events if necessary
        mcc_update_bs_p_rev(bs_p_rev);

        break;
      }
      default:
      {
        ERR("Band Class Error, Do Not Update DB", 0,0,0);
      }
    }

    /* Set the Reselect_included field to 0 */
    cur_bs_ptr->csp.esp.reselect_included = 0;

  } /* end extended sys paramater current check */

  /* ---------------------------------------------------------------
  ** When the mobile station receives the System Parameters Message
  ** with the GEN_NGBR_LST field set equal to '0', it indicates
  ** that the General Neighbor List Message is current by setting
  ** GEN_NGBR_LST_MSG_SEQs to SPM's CONFIG_MSG_SEQs.
  ** --------------------------------------------------------------- */
  if (!P_REV_IS_4_OR_GREATER || !cur_bs_ptr->csp.sp.gen_nghbr_list)
  {
    cur_bs_ptr->gnl_config_msg_seq = cur_bs_ptr->csp.sp.config_msg_seq;
    cur_bs_ptr->rx.gnl_rx = TRUE;

    /*---------------------------------------------------------------
    ** Set the following fields.
    ** a) TIMING_INCL field of NGHBR_REC to '0' for all entries.
    ** b) SRCH_WIN_NGHBR field of NGHBR_REC to SRCH_WIN_Ns.
    ** c) NUM_ANALOG_NGHBR to '000' and ANALOG_NGHBR_LIST to NULL.
    **--------------------------------------------------------------*/
    cur_bs_ptr->anl.num_analog_nghbr = 0;
    for (i=0; i < SRCH_NSET_MAX; i++)
    {
      cur_bs_ptr->nl.nset[i].srch_win_nghbr = cur_bs_ptr->csp.sp.srch_win_n;
      cur_bs_ptr->nl.nset[i].timing_incl = FALSE;
      cur_bs_ptr->nl.nset[i].add_pilot_rec_incl = FALSE;
      cur_bs_ptr->nl.nset[i].srch_offset_nghbr = 0;

      #ifdef FEATURE_IS2000_REL_A
      cur_bs_ptr->nl.nset[i].bcch_supported = FALSE;
      #endif /* FEATURE_IS2000_REL_A */

      /*-------------------------------------------------------------------
      ** if EXT_NGHBR_LIST = 0 Set SRCH_PRIORITY to '10', NGHBR_BAND field
      ** to CDMABAND and NGHBR_FREQ to CDMACH for all entries in NGHBR_REC.
      **------------------------------------------------------------------*/
      if (!cur_bs_ptr->csp.sp.ext_nghbr_list)
      {
        cur_bs_ptr->nl.nset[i].search_pri = 0x02;
        cur_bs_ptr->nl.nset[i].band = cdma.band_class;
        cur_bs_ptr->nl.nset[i].freq = cdma.cdmach;
      }
    }
  }
  /*-----------------------------------------------------------------------
  ** If GEN_NGHBR_LSTs is equal to '1', GEN_NGHBR_LST_MSG_SEQs is equal to
  ** CONFIG_MSG_SEQs, and SETTING_SEARCH_WIN is equal to '1', Set the
  ** SRCH_WIN_NGHBR field of each NGHBR_REC to SRCH_WIN_Ns for all
  ** NGHBR_SET_SIZEs entries. Also set SETTING_SEARCH_WIN field to '0'.
  **----------------------------------------------------------------------*/
  else if (GNLM_MSG_IS_RECVD && cur_bs_ptr->setting_search_win)
  {
    for (i=0; (i < cur_bs_ptr->nl.num_nghbr_pns) && (i < SRCH_NSET_MAX);i++)
    {
      cur_bs_ptr->nl.nset[i].srch_win_nghbr = cur_bs_ptr->csp.sp.srch_win_n;
    }
    cur_bs_ptr->setting_search_win = 0;
  }

  if (!P_REV_IS_6_OR_GREATER || !cur_bs_ptr->csp.sp.ext_chan_list)
  {
    ecl_proc_pending = FALSE;
    /*-----------------------------------------------------------------------
    ** If EXT_CHAN_LIST is equal to 0, then the mobile station shall set
    ** EXT_CHAN_LST_MSG_SEQ s to SPM's CONFIG_MSG_SEQ s.
    ** ----------------------------------------------------------------------*/
    cur_bs_ptr->ecl.config_msg_seq = cur_bs_ptr->csp.sp.config_msg_seq;
    cur_bs_ptr->rx.ecl_rx = TRUE;
    /* Process the Channel List message if it was deferred */
    if (cl_proc_pending && CHAN_LIST_MSG_IS_RECVD)
    {
      new_state = mccidl_proc_cl_msg(new_state);

      if (!(SP_MSG_IS_RECVD))
      {
        /* If we are here that means CL processing cleared
           all ovhds due to a re-hash of freq, so stop processing
           fields now , cur_bs_ptr has changed, all fields in SP
           are now INVALID */
        return (new_state);
      }
    }
  }
  else
  {
    cl_proc_pending = FALSE;
    /* Process the Extended Channel List message if it was deferred */
    if (ecl_proc_pending && EXT_CHAN_LIST_MSG_IS_RECVD)
    {
      new_state = mccidl_proc_ecl_msg(new_state);
      if (!(SP_MSG_IS_RECVD))
      {
        /* If we are here that means ECL processing cleared
           all ovhds due to a re-hash of freq, so stop processing
           fields now , cur_bs_ptr has changed, all fields in SP
           are now INVALID */
        return (new_state);
      }
    }
  }

  if (!P_REV_IS_6_OR_GREATER || !cur_bs_ptr->csp.sp.ext_global_redirect)
  {
    /*-----------------------------------------------------------------------
    ** If EXT_GLOBAL_REDIRECT is equal to 0, then the mobile station shall set
    ** EXT_GLOBAL_REDIRECT_MSG_SEQ s to SPM's CONFIG_MSG_SEQ s.
    ** ----------------------------------------------------------------------*/
    cur_bs_ptr->egsd.config_msg_seq = cur_bs_ptr->csp.sp.config_msg_seq;
    cur_bs_ptr->rx.egsd_rx = TRUE;

    egsd_proc_pending = FALSE;
    egsr_ignored = FALSE;

    /* Process the Global Service Redirection message if it was deferred */
    if (gsd_proc_pending && GSR_MSG_IS_RECVD)
    {
      if (cur_bs_ptr->csp.sp.global_redirect)
      {
        new_state = mccidl_proc_gsr(new_state);
      }
      else
      {
        gsd_proc_pending = FALSE;
        gsr_ignored = FALSE;
      }
    }
  }
  else
  {
    gsd_proc_pending = FALSE;
    gsr_ignored = FALSE;

    /* Process the Extended Global Service Redirection message if it was deferred */
    if (egsd_proc_pending && EGSR_MSG_IS_RECVD)
    {
      new_state = mccidl_proc_egsr(new_state);
    }
  }

  /* ---------------------------------------------------------------
  ** When the mobile station receives the System Parameters Message
  ** with the GLOBAL_REDIRECT field set equal to '0', it indicates
  ** that the Global Service Redirection Message is current by
  ** setting GLOB_SERV_REDIR_MSG_SEQs to SPM's CONFIG_MSG_SEQs.
  ** --------------------------------------------------------------- */
  if (!cur_bs_ptr->csp.sp.global_redirect)
  {
    cur_bs_ptr->gsd.config_msg_seq = cur_bs_ptr->csp.sp.config_msg_seq;
    cur_bs_ptr->rx.gsd_rx = TRUE;
  }

  /* Reset flag */
  rescan_ignored = FALSE; 

  if (cur_bs_ptr->csp.sp.rescan)
  {
    if ( ( new_state == CDMA_IDLE_INIT ) || !mcc_is_ovhd_msg_okay() || mccidl_fast_raho ) /*lint !e641 */ 
    {
      /* ------------------------------------------------------
      ** Ignore rescan until all overhead messages are current.
      ** ------------------------------------------------------ */
      MSG_MED("Delay rescan proc till system selected", 0, 0, 0);

      rescan_ignored = TRUE;

      if ( MAIN_STATE(new_state) == CDMA_IDLE ) 
      {
        /* In case RESCAN is ignored, we need to change state or
           set relevant flag to trigger RESCAN be processed later. */ 
        if ( new_state == CDMA_OVHD ) /*lint !e641 */
        {
          oh_sd_rpt_pending_in_upd_ovhd = TRUE;
        }
        else if ( new_state != CDMA_IDLE_INIT ) /*lint !e641 */
        {
          new_state = CDMA_IDLE_INIT; /*lint !e641 */
          if ( !mccidl_fast_raho )
          {
            /* Start the BS insanity timer */
            mccidl_set_bs_insanity_timer();
          }
        }
      }
    }
    else
    {
      /* Event report: Rescan */
      event_report(EVENT_RESCAN);

      /* ------------------------------------------
      ** Inform system determination of rescan exit
      ** ------------------------------------------ */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_opr_rescan(NULL)))
      {
        mcc_exit.exit = MCC_RESCAN;
        return (CDMA_EXIT);
      }
    }
  }

  pagech = (byte) hash(cdma.imsi_hash_key, cur_bs_ptr->csp.sp.page_chan,
                      (word) (2 * (cdma.imsi_hash_key & 0xFFF))) + 1;

  cdma.page_chan_s = cur_bs_ptr->csp.sp.page_chan;

  if (pagech != cdma.pagech)
  {
    /* ----------------------------------------------------------------
    ** The SPM processing should not hash PC for Fast RAHO, this is
    ** because, the PC is hashed by SRCH task if required, by means of
    ** calling the function mcc_srch_get_nbr_info.
    ** ------------------------------------------------------------- */
    if (mccidl_fast_raho)
    {
      MSG_MED( "SPM processing not hash PCH for a Fast RAHO", 0, 0, 0 );
      return (new_state);
    }
    else
    {
      MSG_MED( "Hashed to PCH %d", pagech, 0, 0 );
      cdma.pagech = pagech;

      bs_info_id.ovhd_chan = CAIX_PC;
      bs_info_id.pilot_pn = cur_bs_ptr->csp.sp.pilot_pn;
      bs_info_id.cdma_ch = cdma.cdmach;
      bs_info_id.band_class = cdma.band_class;
      bs_info_id.ovhd_walsh_ch = cdma.pagech;

      mcc_entry_init(&bs_info_id, FALSE);

      /* Command searcher to new Paging Channel */
      mccidl_send_srch_idle_aset(); 

      /* Forget about the GSRM or EGSRM received, if any */
      gsr_ignored = FALSE;
      gsd_proc_pending = FALSE;
      egsr_ignored = FALSE;
      egsd_proc_pending = FALSE;

      /* Wait for Paging Channel mask to take effect */
      (void) rex_set_timer(&mcc_cmd_timer, CHANGE_PAGE_CHAN_WAIT);

      do
      {
        sigs = mcc_wait( MCC_CMD_TIMER_SIG | MCC_RXTX_Q_SIG );

        if ((sigs & MCC_RXTX_Q_SIG) != 0)
        {
          /* --------------------------------------------------------------
          ** We have been directed to another channel so no more useful
          ** messages should come on this Paging Channel.  To avoid filling
          ** the RXTX queue while we wait for the new Paging Channel mask
          ** mask to take effect, we will throw these messages out.
          ** -------------------------------------------------------------- */
          rx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q );
          cmd_done( &rx_ptr->hdr.cmd_hdr );
        }
      } while ((sigs & MCC_CMD_TIMER_SIG) == 0);

      (void) rex_clr_sigs( &mc_tcb, MCC_CMD_TIMER_SIG );

      /* Paging Channel Changed...Check if overheads are current */
      if (mcc_is_ovhd_msg_okay())
      {
	 /* Process all the stored overheads because it is a new PCH */
	 mccidl_process_stored_ovhds(new_state);
      }  
      else
      {
         mcc_clear_ovhd_msgs();
      }
      /* Return back and ignore rest of SPM processing
      ** as the paging channel hashing has happened */
      return (new_state);
    }
  }
  else if ( !mccidl_fast_raho )
  {
    MSG_MED( "On PCH %d", cdma.pagech, 0, 0 );
  }


  if (new_state != CDMA_EXIT)
  {
    /* Process registration variable of System Parameters message */
    mccreg_sysparm(&cur_bs_ptr->csp.sp);

    /* Calculate and store the operational sci */
    (void)mcc_set_operational_sci();  

    /* store real handoff parameters */
    mcc_ho_real.win_a         = cur_bs_ptr->csp.sp.srch_win_a;
    mcc_ho_real.win_n         = cur_bs_ptr->csp.sp.srch_win_n;
    mcc_ho_real.win_r         = cur_bs_ptr->csp.sp.srch_win_r;
    mcc_ho_real.nghbr_max_age = cur_bs_ptr->csp.sp.nghbr_max_age;
    mcc_ho_real.t_add         = cur_bs_ptr->csp.sp.t_add;
    mcc_ho_real.t_drop        = cur_bs_ptr->csp.sp.t_drop;
    mcc_ho_real.t_comp        = cur_bs_ptr->csp.sp.t_comp;
    mcc_ho_real.t_tdrop       = cur_bs_ptr->csp.sp.t_tdrop;

    if (!P_REV_IS_4_OR_GREATER)
    {
      /* disable IS95B soft handoff */
       mcc_ho_real.soft_slope     =
       mcc_ho_real.add_intercept  =
       mcc_ho_real.drop_intercept = 0;
    }

    if (!mccidl_fast_raho)
    {
      /* Update Searcher's handoff parameter */
      mccidl_send_srch_parms();

      /* Update Searcher's neighbor set */ 
      mccidl_send_srch_idle_nset();
    }

    /* -------------------------------------------------------------------
    ** If SID/NID changed in Idle State, update state or relevant flag so  
    ** that modules like SD will be notified of the change.
    ** ------------------------------------------------------------------- */
    /*lint -e727 */
    if ((MAIN_STATE(new_state) == CDMA_IDLE) &&
        ((cur_bs_ptr->csp.sp.sid != prev_sid) ||
         (cur_bs_ptr->csp.sp.nid != prev_nid)))
    /*lint +e727 */
    {
      if ( new_state == CDMA_OVHD ) /*lint !e641 */
      {
        oh_sd_rpt_pending_in_upd_ovhd = TRUE;
      }
      else if ( new_state != CDMA_IDLE_INIT ) /*lint !e641 */
      {
        new_state = CDMA_IDLE_INIT; /*lint !e641 */
        if ( !mccidl_fast_raho )
        {
          /* Start the BS insanity timer */
          mccidl_set_bs_insanity_timer();
        }
      }

      prev_sid = cur_bs_ptr->csp.sp.sid;
      prev_nid = cur_bs_ptr->csp.sp.nid;

      /* Mark EPZID processing is pending. */
      mccidl_epzid_proc_pending = TRUE;
    }

    #if defined (FEATURE_IS2000_REL_C_AKA) || defined (FEATURE_IS2000_REL_C_MI)
#error code not present
    #endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_C_MI */

  } /* End if (new_state != CDMA_EXIT) */

  return (new_state);

} /* mccidl_proc_sys_parm */

/*===========================================================================

FUNCTION MCCIDL_PROC_ACC_PARM

DESCRIPTION
  This procedure processes a received Access Parameters Message as described
  in IS-95A section 6.6.2.2.2.  This procedure is used for Idle State and
  Update Overhead Information Substate of System Access State.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  This function is only called for FEATURE_IS95B or later capable mobiles.

===========================================================================*/
word mccidl_proc_acc_parm
(
  word new_state
)
{
  if (mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, cdma.band_class))
  {
    cur_bs_ptr->ap.nom_pwr_ext = 0;
  }

  #ifdef FEATURE_AUTH
  /* Only if the phone is capable of IS-95B or greater */
  if (P_REV_IS_4_OR_GREATER)
  {
    /*-----------------------------------------------------------------------
    ** If MS is not doing AHO/APHO, update authentication parameters from new
    ** base station.
    **-----------------------------------------------------------------------*/
    if ((new_state != CDMA_ORIG) && (new_state != CDMA_PAGE_RES)) /*lint !e641 */
    {
      cur_bs_ptr->rp.auth = cur_bs_ptr->ap.auth;
      cur_bs_ptr->rp.rand = cur_bs_ptr->ap.rand;
    }
  } /* end P_REV check */
  #endif /* FEATURE_AUTH */

  #if (defined(FEATURE_HIGH_PSIST) && defined (FEATURE_PASSPORT_MODE))
  if(mcc_passport_mode_is_jcdma())
  {
    mccidl_update_psist_info(CAI_ACH, &cur_bs_ptr->ap);
  }
  #endif /* FEATURE_HIGH_PSIST && FEATURE_PASSPORT_MODE */

  return (new_state);

} /* end mccidl_proc_acc_parm() */

/*===========================================================================
FUNCTION : MCCIDL_SET_NGHBR_LIST_INFO

DESCRIPTION
  This function is an access function to update the Neighbor List information
  common to all the received Neighbor pilots. It updates the Pilot Increment,
  Number of Neighbors and the active pilot of the Neighbor List.

DEPENDENCIES
  The pil_inc and num_nghbrs are passed as parameters.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_set_nghbr_list_info
(
 caii_rx_msg_type* msg_ptr
)
{

  /* Set default neighbor search mode as no priorities or windows */
  cur_bs_ptr->nl.nghbr_srch_mode = SRCH_NO_PRI_OR_WIN;

  if (msg_ptr->gen.msg_type == CAI_NL_MSG)
  {
    cur_bs_ptr->nl.pilot_inc = msg_ptr->nghbr.pilot_inc;
    if (P_REV_IS_4_OR_GREATER && SP_MSG_IS_RECVD &&
        cur_bs_ptr->csp.sp.gen_nghbr_list && GNLM_MSG_IS_RECVD)
    {
      cur_bs_ptr->nl.num_nghbr_pns =
        MIN(SRCH_NSET_MAX, MAX(cur_bs_ptr->nl.num_nghbr_pns,
                               msg_ptr->nghbr.num_nghbrs));
    }
    else
    {
      cur_bs_ptr->nl.num_nghbr_pns =
        MIN(SRCH_NSET_MAX, msg_ptr->nghbr.num_nghbrs);
    }
  }

  else if (msg_ptr->gen.msg_type == CAI_EXT_NL_MSG)
  {
    cur_bs_ptr->nl.pilot_inc = msg_ptr->ext_nghbr.pilot_inc;
    if (P_REV_IS_4_OR_GREATER && SP_MSG_IS_RECVD &&
        cur_bs_ptr->csp.sp.gen_nghbr_list && GNLM_MSG_IS_RECVD)
    {
      cur_bs_ptr->nl.num_nghbr_pns =
        MIN(SRCH_NSET_MAX, MAX(cur_bs_ptr->nl.num_nghbr_pns,
                               msg_ptr->ext_nghbr.num_nghbrs));
    }
    else
    {
      cur_bs_ptr->nl.num_nghbr_pns =
        MIN(SRCH_NSET_MAX, msg_ptr->ext_nghbr.num_nghbrs);
    }
  }

  else if (P_REV_IS_4_OR_GREATER && (msg_ptr->gen.msg_type == CAI_GEN_NL_MSG))
  {
    cur_bs_ptr->nl.pilot_inc = msg_ptr->gen_nghbr.pilot_inc;
    cur_bs_ptr->nl.nghbr_srch_mode = msg_ptr->gen_nghbr.nghbr_srch_mode;
    if (((msg_ptr->gen_nghbr.nghbr_config_pn_incl) &&
         (msg_ptr->gen_nghbr.freq_fields_incl)) ||
        !(NLM_MSG_IS_RECVD))
    {
      cur_bs_ptr->nl.num_nghbr_pns =
        MIN(SRCH_NSET_MAX, msg_ptr->gen_nghbr.num_nghbr);
    }
    else
    {
      cur_bs_ptr->nl.num_nghbr_pns =
        MIN(SRCH_NSET_MAX, MAX(cur_bs_ptr->nl.num_nghbr_pns,
                               msg_ptr->gen_nghbr.num_nghbr));
    }
  }

} /* end mccidl_set_nghbr_list_info */

/*===========================================================================
FUNCTION : MCCIDL_SET_NGHBR_PN_CONFIG

DESCRIPTION
  This function sets the pilot_pn and config fields of a particular Neighbor
  pilot in the Neighbor List. This access function is used in NLM, ENLM and
  GNLM processing functions.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  This functions sets the pn and config fields of a particular instance of a
  Neighbor List pilot.  Care should be taken to pass the appropriate instance
  value of the Neighbor for which this function is used.
===========================================================================*/
void mccidl_set_nghbr_pn_config
(
  word              inst,
  caii_rx_msg_type* msg_ptr
)
{
  if (msg_ptr->gen.msg_type == CAI_NL_MSG)
  {
    cur_bs_ptr->nl.nset[inst].pilot_pn = msg_ptr->nghbr.nghbr[inst].nghbr_pn;
    cur_bs_ptr->nl.nset[inst].config = msg_ptr->nghbr.nghbr[inst].nghbr_config;
    if (GNLM_MSG_NOT_RECVD)
    {
      cur_bs_ptr->nl.nset[inst].add_pilot_rec_incl = FALSE;
      #ifdef FEATURE_IS2000_REL_A
      cur_bs_ptr->nl.nset[inst].bcch_supported = FALSE;
      #endif /* FEATURE_IS2000_REL_A */
    }
  }

  else if (msg_ptr->gen.msg_type == CAI_EXT_NL_MSG)
  {
    cur_bs_ptr->nl.nset[inst].pilot_pn =
      msg_ptr->ext_nghbr.nghbr[inst].nghbr_pn;
    cur_bs_ptr->nl.nset[inst].config =
      msg_ptr->ext_nghbr.nghbr[inst].nghbr_config;
    if (GNLM_MSG_NOT_RECVD)
    {
      cur_bs_ptr->nl.nset[inst].add_pilot_rec_incl = FALSE;
      #ifdef FEATURE_IS2000_REL_A
      cur_bs_ptr->nl.nset[inst].bcch_supported = FALSE;
      #endif /* FEATURE_IS2000_REL_A */
    }
  }

  else if ((msg_ptr->gen.msg_type == CAI_GEN_NL_MSG) &&
           (P_REV_IS_4_OR_GREATER))
  {
    if (msg_ptr->gen_nghbr.nghbr_config_pn_incl)
    {
      cur_bs_ptr->nl.nset[inst].config =
        msg_ptr->gen_nghbr.nghbr[inst].nghbr_config;
      cur_bs_ptr->nl.nset[inst].pilot_pn =
        msg_ptr->gen_nghbr.nghbr[inst].nghbr_pn;
    }
    if (msg_ptr->gen_nghbr.is2000_incl)
    {
      cur_bs_ptr->nl.nset[inst].add_pilot_rec_incl =
        msg_ptr->gen_nghbr.add_nghbr[inst].add_pilot_rec_incl;
      if (cur_bs_ptr->nl.nset[inst].add_pilot_rec_incl)
      {
        cur_bs_ptr->nl.nset[inst].nghbr_pilot_rec_type =
          (cai_pilot_rec_type) msg_ptr->gen_nghbr.add_nghbr[inst].nghbr_pilot_rec_type;
        cur_bs_ptr->nl.nset[inst].nghbr_pilot_rec =
          msg_ptr->gen_nghbr.add_nghbr[inst].add_pilot_rec;
      }

      #ifdef FEATURE_IS2000_REL_A
      if (P_REV_IS_7_OR_GREATER && msg_ptr->gen_nghbr.bcch_ind_incl)
      {
        cur_bs_ptr->nl.nset[inst].bcch_supported =
          msg_ptr->gen_nghbr.bcch_support[inst];
      }
      else
      {
        cur_bs_ptr->nl.nset[inst].bcch_supported = FALSE;
      }
      #endif //FEATURE_IS2000_REL_A

    }
    else
    {
      cur_bs_ptr->nl.nset[inst].add_pilot_rec_incl = FALSE;
      #ifdef FEATURE_IS2000_REL_A
      cur_bs_ptr->nl.nset[inst].bcch_supported = FALSE;
      #endif /* FEATURE_IS2000_REL_A */
    }
  }

} /* mccidl_set_pn_config */

/*===========================================================================
FUNCTION : MCCIDL_SET_NGHBR_TIMING

DESCRIPTION
  This function sets the timing information for a particular Neighbor Pilot
  in the Neighbor List. This function updates the timing related fields for
  an individual pilot based on the type of message (NLM, ENLM & GNLM )

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  This functions sets the timing related fields of a particular instance of a
  Neighbor List pilot.  Care should be taken to pass the appropriate instance
  of the Neighbor for which the values are updated.
  This function is only called for FEATURE_IS95B or later capable mobiles.
===========================================================================*/
void mccidl_set_nghbr_timing
(
  word              inst,
  caii_rx_msg_type* msg_ptr
)
{
  /* Only if the phone is capable of IS-95B or greater */
  if (P_REV_IS_4_OR_GREATER)
  {
    if (((msg_ptr->gen.msg_type == CAI_NL_MSG) ||
        (msg_ptr->gen.msg_type == CAI_EXT_NL_MSG)) && GNLM_MSG_NOT_RECVD)
    {
      /* Set the default value for the timing_incl field to '0' */
      cur_bs_ptr->nl.nset[inst].timing_incl = FALSE;
    }
    else if (msg_ptr->gen.msg_type == CAI_GEN_NL_MSG)
    {
      if ((msg_ptr->gen_nghbr.use_timing) &&
          (msg_ptr->gen_nghbr.nghbr[inst].timing_incl))
      {
        cur_bs_ptr->nl.nset[inst].timing_incl =
          msg_ptr->gen_nghbr.nghbr[inst].timing_incl;
        cur_bs_ptr->nl.nset[inst].nghbr_tx_offset =
          msg_ptr->gen_nghbr.nghbr[inst].nghbr_tx_offset;

        if (msg_ptr->gen_nghbr.global_timing_incl)
        {
          cur_bs_ptr->nl.nset[inst].nghbr_tx_duration =
            msg_ptr->gen_nghbr.global_tx_duration;
          cur_bs_ptr->nl.nset[inst].nghbr_tx_period =
            msg_ptr->gen_nghbr.global_tx_period;
        }
        else
        {
          cur_bs_ptr->nl.nset[inst].nghbr_tx_duration =
            msg_ptr->gen_nghbr.nghbr[inst].nghbr_tx_duration;
          cur_bs_ptr->nl.nset[inst].nghbr_tx_period =
            msg_ptr->gen_nghbr.nghbr[inst].nghbr_tx_period;
        }
      }
      else
      {
        cur_bs_ptr->nl.nset[inst].timing_incl = FALSE;
      }

    } /* end neighbor list message type check */
  } /* end P_REV checking */

} /* mccidl_set_nghbr_timing */

/*===========================================================================
FUNCTION : MCCIDL_SET_NGHBR_SRCH_PRI

DESCRIPTION
  This function sets the search priority of a particular Pilot in the Neighbor
  List.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  This functions sets the search Priority of a Neighbor. Care should be taken
  to pass the appropriate instance of the Neighbor.
===========================================================================*/
void mccidl_set_nghbr_srch_pri
(
  word              inst,
  caii_rx_msg_type* msg_ptr
)
{
  if (msg_ptr->gen.msg_type == CAI_EXT_NL_MSG)
  {
    cur_bs_ptr->nl.nset[inst].search_pri =
      msg_ptr->ext_nghbr.nghbr[inst].search_pri;
  }

  /* The following else condiditions are only evaluated for FEATURE_IS95B */
  /* or later capable mobiles.                                            */
  else if ((msg_ptr->gen.msg_type == CAI_NL_MSG) && GNLM_MSG_NOT_RECVD &&
           (P_REV_IS_4_OR_GREATER))
  {
    cur_bs_ptr->nl.nset[inst].search_pri = 0x02;
  }


  else if ((msg_ptr->gen.msg_type == CAI_GEN_NL_MSG) &&
           (P_REV_IS_4_OR_GREATER))
  {
    /********************************************************************
    ** If NGHBR_SRCH_MODE = 00 or 10 and EXT_NGHBR_LIST_MSG is not equal
    ** to CONFIG_MSG_SEQ, set the SEARCH_PRIORITY to '10' for all records.
    ********************************************************************/
    if (((msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_NO_PRI_OR_WIN) ||
        (msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_WIN_ONLY)) &&
        (cur_bs_ptr->nl_config_msg_seq != cur_bs_ptr->rx.config_msg_seq))
    {
      cur_bs_ptr->nl.nset[inst].search_pri = SRCH_WIN_ONLY;
    }

    /********************************************************************
    ** If NGHBR_SRCH_MODE = 01 or 11, set the SEARCH_PRIORITY to the ith
    ** occurrence.
    ********************************************************************/
    else if ((msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_PRI_ONLY) ||
             (msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_PRI_AND_WIN))
    {
      cur_bs_ptr->nl.nset[inst].search_pri =
        msg_ptr->gen_nghbr.nghbr[inst].search_pri;
    }
  }

} /* mccidl_set_nghbr_srch_pri */

/*===========================================================================
FUNCTION : MCCIDL_SET_NGHBR_SRCH_WIN

DESCRIPTION
  This function sets the search window of a particular Pilot in the Neighbor
  List.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Care should be taken to pass the appropriate instance of the Neighbor.
  This should only be executed by FEATURE_IS95B or greater capable mobiles.
===========================================================================*/
void mccidl_set_nghbr_srch_win
(
  word             inst,
  caii_rx_msg_type *msg_ptr
)
{
  /* Only if the phone is capable of IS-95B or greater */
  if (P_REV_IS_4_OR_GREATER)
  {
    if (((msg_ptr->gen.msg_type == CAI_NL_MSG) ||
        (msg_ptr->gen.msg_type == CAI_EXT_NL_MSG)) && GNLM_MSG_NOT_RECVD)
    {
      if (SP_MSG_IS_RECVD)
      {
        cur_bs_ptr->nl.nset[inst].srch_win_nghbr =
          cur_bs_ptr->csp.sp.srch_win_n;
      }
    }

    else if (msg_ptr->gen.msg_type == CAI_GEN_NL_MSG)
    {
      /********************************************************************
      ** If NGHBR_SRCH_MODE = 00 or 01, set the SRCH_WIN_NGHBR field of the
      ** NGHBR_REC to SEARCH_WINs for all the records.
      ********************************************************************/
      if ((msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_NO_PRI_OR_WIN) ||
          (msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_PRI_ONLY))
      {
        if (SP_MSG_IS_RECVD)
        {
          cur_bs_ptr->nl.nset[inst].srch_win_nghbr =
            cur_bs_ptr->csp.sp.srch_win_n;
        }
        else
        {
          /********************************************************************
          ** Set the setting_search_win flag to true, This is a case when the
          ** GNLM arrived before the SP message and there is no srch_win_n to
          ** set the default value. When the SP message arrives later, it sets
          ** the default values based on this flag.
          ********************************************************************/
          cur_bs_ptr->setting_search_win = TRUE;
        }
      }

      /********************************************************************
      ** If NGHBR_SRCH_MODE = 10 or 11, set the SRCH_WIN_NGHBR field of the
      ** NGHBR_REC to SEARCH_WIN_NGHBR for all the records.
      ********************************************************************/
      else if ((msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_WIN_ONLY) ||
          (msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_PRI_AND_WIN))
      {
        cur_bs_ptr->nl.nset[inst].srch_win_nghbr =
          msg_ptr->gen_nghbr.nghbr[inst].srch_win_nghbr;
      }
    }
  } /* end of P_REV check */

} /* mccidl_set_srch_win_nghbr */

/*===========================================================================
FUNCTION : MCCIDL_SET_NGHBR_SRCH_OFFSET

DESCRIPTION
  This function sets the search offset of a particular Pilot in the Neighbor
  List.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void mccidl_set_nghbr_srch_offset
(
  word             inst,
  caii_rx_msg_type *msg_ptr
)
{
  /* Only if the phone is capable of IS-2000 or greater */
  if (P_REV_IS_4_OR_GREATER)
  {
    if (((msg_ptr->gen.msg_type == CAI_NL_MSG) ||
         (msg_ptr->gen.msg_type == CAI_EXT_NL_MSG)) && GNLM_MSG_NOT_RECVD)
    {
      cur_bs_ptr->nl.nset[inst].srch_offset_nghbr = 0;   /* Initialize to 0 */
    }

    else if (msg_ptr->gen.msg_type == CAI_GEN_NL_MSG)
    {
      if ((msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_NO_PRI_OR_WIN) ||
          (msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_PRI_ONLY))
      {
        cur_bs_ptr->nl.nset[inst].srch_offset_nghbr = 0; /* Initialize to 0 */
      }

      else if ((msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_WIN_ONLY) ||
               (msg_ptr->gen_nghbr.nghbr_srch_mode == SRCH_PRI_AND_WIN))
      {
        if (P_REV_IS_6_OR_GREATER && msg_ptr->gen_nghbr.srch_offset_incl)
        {
          cur_bs_ptr->nl.nset[inst].srch_offset_nghbr =
            msg_ptr->gen_nghbr.add_nghbr[inst].srch_offset_nghbr;
        }
        else
        {
          cur_bs_ptr->nl.nset[inst].srch_offset_nghbr = 0;
        }
      }
    }
  } /*  end of P_REV_IS_4_OR_GREATER */
  else
  {
    cur_bs_ptr->nl.nset[inst].srch_offset_nghbr = 0;
  }

} /* mccidl_set_nghbr_srch_offset */

/*===========================================================================
FUNCTION : MCCIDL_SET_NGHBR_BAND_FREQ

DESCRIPTION
  This function sets the band and frequency information of a particular
  Pilot in the Neighbor List.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Care should be taken to pass the appropriate instance of the Neighbor.
===========================================================================*/
void mccidl_set_nghbr_band_freq
(
  word             inst,
  caii_rx_msg_type *msg_ptr
)
{
  boolean valid_freq_incl = TRUE;

  if (msg_ptr->gen.msg_type == CAI_EXT_NL_MSG)
  {
    cur_bs_ptr->nl.nset[inst].freq_incl =
      msg_ptr->ext_nghbr.nghbr[inst].freq_incl;

    if (msg_ptr->ext_nghbr.nghbr[inst].freq_incl)
    {
      cur_bs_ptr->nl.nset[inst].band =
        msg_ptr->ext_nghbr.nghbr[inst].nghbr_band;
      cur_bs_ptr->nl.nset[inst].freq =
        msg_ptr->ext_nghbr.nghbr[inst].nghbr_freq;
    }
    else
    {
      cur_bs_ptr->nl.nset[inst].band = cdma.band_class;
      cur_bs_ptr->nl.nset[inst].freq = cdma.cdmach;

      /* ------------------------------------------------------------
      ** In case of APHO and AHO, ECCLM is not processed. So we won't
      ** be able to know real values of freq an band in those cases. 
      ** So the best guess is to initialize freq and band to be the 
      ** same as active pilot.
      ** ------------------------------------------------------------ */ 

      if (cur_bs_ptr->nl.nset[inst].config == 2)
      {
        if (SP_MSG_IS_RECVD && ESP_MSG_IS_RECVD && CHAN_LIST_MSG_IS_RECVD &&
            EXT_CHAN_LIST_MSG_IS_RECVD)
        {
          if (P_REV_IS_6_OR_GREATER && cur_bs_ptr->csp.sp.ext_chan_list)
          {
            if (cur_bs_ptr->ecl.cdma_freq[0] != cdma.cdmach)
            {
              cur_bs_ptr->nl.nset[inst].freq_incl = TRUE;
              cur_bs_ptr->nl.nset[inst].freq = cur_bs_ptr->ecl.cdma_freq[0];
            }
          }
          else
          {
            if (cur_bs_ptr->cl.cdma_freq[0] != cdma.cdmach)
            {
              cur_bs_ptr->nl.nset[inst].freq_incl = TRUE;
              cur_bs_ptr->nl.nset[inst].freq = cur_bs_ptr->cl.cdma_freq[0];
            }
          }
        }
      }
    }
  }

  /* The following else condiditions are only evaluated for FEATURE_IS95B */
  /* or later capable mobiles.                                            */
  else if ((msg_ptr->gen.msg_type == CAI_NL_MSG) && GNLM_MSG_NOT_RECVD &&
           (P_REV_IS_2_OR_3 || P_REV_IS_4_OR_GREATER))
  {
    cur_bs_ptr->nl.nset[inst].freq_incl = FALSE;
    cur_bs_ptr->nl.nset[inst].band = cdma.band_class;
    cur_bs_ptr->nl.nset[inst].freq = cdma.cdmach;

    /* ------------------------------------------------------------
    ** In case of APHO and AHO, ECCLM is not processed. So we won't
    ** be able to know real values of freq in those cases. So the 
    ** best guess is to initialize freq to be the same as active
    ** pilot.
    ** ------------------------------------------------------------ */ 

    if (cur_bs_ptr->nl.nset[inst].config == 2)
    {
      if (P_REV_IS_4_OR_GREATER)
      {
        if (SP_MSG_IS_RECVD && ESP_MSG_IS_RECVD && CHAN_LIST_MSG_IS_RECVD &&
            EXT_CHAN_LIST_MSG_IS_RECVD)
        {
          if (P_REV_IS_6_OR_GREATER && cur_bs_ptr->csp.sp.ext_chan_list)
          {
            if (cur_bs_ptr->ecl.cdma_freq[0] != cdma.cdmach)
            {
              cur_bs_ptr->nl.nset[inst].freq_incl = TRUE;
              cur_bs_ptr->nl.nset[inst].freq = cur_bs_ptr->ecl.cdma_freq[0];
            }
          }
          else
          {
            if (cur_bs_ptr->cl.cdma_freq[0] != cdma.cdmach)
            {
              cur_bs_ptr->nl.nset[inst].freq_incl = TRUE;
              cur_bs_ptr->nl.nset[inst].freq = cur_bs_ptr->cl.cdma_freq[0];
            }
          }
        }
      }
      else /* For IS-95A */
      {
        /* Check for SPM, ESPM and CCLM.ECCLM is not applicable in IS95 */   
        if (SP_MSG_IS_RECVD && ESP_MSG_IS_RECVD && CHAN_LIST_MSG_IS_RECVD)
        {
          if (cur_bs_ptr->cl.cdma_freq[0] != cdma.cdmach)
          {
            cur_bs_ptr->nl.nset[inst].freq = cur_bs_ptr->cl.cdma_freq[0];
          }
        }
      }
    }
  }
  else if ((msg_ptr->gen.msg_type == CAI_GEN_NL_MSG) &&
           (P_REV_IS_4_OR_GREATER))
  {
    /********************************************************************
    ** if FREQ_FIELDS_INCL and FREQ_INC equal 1, set the NGHBR_BAND and
    ** NGHBR_FREQ of the ith record of the NGHBR_REC.
    ********************************************************************/
    if (msg_ptr->gen_nghbr.freq_fields_incl)
    {
      /* Copy the freq_incl field for the ith record */
      cur_bs_ptr->nl.nset[inst].freq_incl =
        msg_ptr->gen_nghbr.nghbr[inst].freq_incl;

      if (msg_ptr->gen_nghbr.nghbr[inst].freq_incl)
      {
        cur_bs_ptr->nl.nset[inst].band =
          msg_ptr->gen_nghbr.nghbr[inst].nghbr_band;
        cur_bs_ptr->nl.nset[inst].freq =
          msg_ptr->gen_nghbr.nghbr[inst].nghbr_freq;
      }
      else
      {
        valid_freq_incl = FALSE;
      }
    }

    else if (cur_bs_ptr->nl_config_msg_seq != cur_bs_ptr->rx.config_msg_seq )
    {
      valid_freq_incl = FALSE;
    }

    /********************************************************************
    ** For each Neighboring base stations contained in the GNLM, if
    ** a) FREQ_FIELDS_INCL equals '1' and FREQ_INCL equals '0' or
    ** b) FREQ_FIELDS_INCL equals '0' and EXT_NGHBR_LST_MSG_SEQ is not
    **    equal to CONFIG_MSG_SEQ, set the following info.
    **
    ********************************************************************/
    if (!valid_freq_incl)
    {
      cur_bs_ptr->nl.nset[inst].band = cdma.band_class;
      cur_bs_ptr->nl.nset[inst].freq = cdma.cdmach;

      /* ------------------------------------------------------------
      ** In case of APHO and AHO, ECCLM is not processed. So we won't
      ** be able to know real values of freq an band in those cases. 
      ** So the best guess is to initialize freq and band to be the 
      ** same as active pilot.
      ** ------------------------------------------------------------ */ 

      if (cur_bs_ptr->nl.nset[inst].config == 2)
      {
        if (SP_MSG_IS_RECVD && ESP_MSG_IS_RECVD && CHAN_LIST_MSG_IS_RECVD &&
            EXT_CHAN_LIST_MSG_IS_RECVD)
        {
          if (P_REV_IS_6_OR_GREATER && cur_bs_ptr->csp.sp.ext_chan_list)
          {
            if (cur_bs_ptr->ecl.cdma_freq[0] != cdma.cdmach)
            {
              cur_bs_ptr->nl.nset[inst].freq_incl = TRUE;
              cur_bs_ptr->nl.nset[inst].freq = cur_bs_ptr->ecl.cdma_freq[0];
            }
          }
          else
          {
            if (cur_bs_ptr->cl.cdma_freq[0] != cdma.cdmach)
            {
              cur_bs_ptr->nl.nset[inst].freq_incl = TRUE;
              cur_bs_ptr->nl.nset[inst].freq = cur_bs_ptr->cl.cdma_freq[0];
            }
          }
        }
      }
    }
  }

} /* mccidl_set_nghbr_band_freq */

/*===========================================================================

FUNCTION : MCCIDL_SET_ACCESS_HO_INFO

DESCRIPTION
  This function sets the access handoff related fields for a particular pilot
  in the Neighbor List.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Care should be taken to pass the appropriate instance of the Neighbor.
  This should only be executed by FEATURE_IS95B or greater capable mobiles.

===========================================================================*/
void mccidl_set_access_ho_info
(
  word             inst,
  caii_rx_msg_type *msg_ptr
)
{
  /* Only if the phone is capable of IS-95B or greater */
  if (P_REV_IS_4_OR_GREATER)
  {
    /**************************************************************************
    If the received Message is either NLM, ENLM or GNLM, and if the Extended
    System Parameters is not received yet, the access_entry_ho and
    access_ho_allowed fields are set to default values '0'.
    **************************************************************************/
    if ((msg_ptr->gen.msg_type == CAI_NL_MSG) ||
        (msg_ptr->gen.msg_type == CAI_EXT_NL_MSG) ||
        (msg_ptr->gen.msg_type == CAI_GEN_NL_MSG))
    {
      if (ESP_MSG_NOT_RECVD || !cur_bs_ptr->csp.esp.nghbr_set_entry_info)
      {
        cur_bs_ptr->nl.nset[inst].access_entry_ho = FALSE;
      }
      if (ESP_MSG_NOT_RECVD || !cur_bs_ptr->csp.esp.nghbr_set_access_info)
      {
        cur_bs_ptr->nl.nset[inst].access_ho_allowed = FALSE;
      }
    }
  } /* end of P_REV check */

} /* mccidl_set_access_ho_info () */

/*===========================================================================
FUNCTION MCCIDL_SET_ANALOG_NGHBRS

DESCRIPTION
  This procedure sets the Analog Neighbors information based on the type of
  Neighbor List Message received over the air.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  This should only be executed by FEATURE_IS95B or greater capable mobiles.

/===========================================================================*/
void mccidl_set_analog_nghbrs
(
  caii_rx_msg_type *msg_ptr
)
{
  /* Only if the phone is capable of IS-95B or greater */
  if (P_REV_IS_4_OR_GREATER)
  {
    word i; /* counter */

    /*************************************************************************
    ** For NLM and ENLM, Set the Number of Analog Neighbors to '0' if GNLM
    ** Msg is not received.
    *************************************************************************/
    if (((msg_ptr->gen.msg_type == CAI_NL_MSG) ||
        (msg_ptr->gen.msg_type == CAI_EXT_NL_MSG)) && GNLM_MSG_NOT_RECVD)
    {
      cur_bs_ptr->anl.num_analog_nghbr = 0;
    }

    else if (msg_ptr->gen.msg_type == CAI_GEN_NL_MSG)
    {
      /* Store the Analog Neighbor List entries in case of a GNLM message */
      cur_bs_ptr->anl.num_analog_nghbr = msg_ptr->gen_nghbr.num_analog_nghbr;
      for (i=0; i < msg_ptr->gen_nghbr.num_analog_nghbr; i++)
      {
        cur_bs_ptr->anl.anl_nghbr[i].band_class =
          msg_ptr->gen_nghbr.analog_nghbr[i].band_class;
        cur_bs_ptr->anl.anl_nghbr[i].sys_a_b =
          msg_ptr->gen_nghbr.analog_nghbr[i].sys_a_b;
      }
    }
  } /* end P_REV check */

} /* mccidl_set_analog_nghbrs() */

/*===========================================================================

FUNCTION MCCIDL_PROC_TRUE_IMSI

DESCRIPTION
  This procedure processes the TRUE IMSI fields of the received Extended
  System Parameters Message and ANSI41 System Parameters Message.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_proc_true_imsi
(
  boolean imsi_t_supported
)
{
  if (mcc_change_imsi_type(imsi_t_supported))
  {
    /* Re-calculate HASH_KEY (see IS-95B section 6.6.7.1) */
    cdma.imsi_hash_key = cdma.imsi_s1 + (((dword) cdma.imsi_s2) << 24);
    
    // Note: this key has to be remembered for QPCH operation
    cur_bs_ptr->imsi_hash_key = cdma.imsi_hash_key;

    /* Re-calculate page slot in case we don't go through idle_init */
    idle_pgslot = hash(cdma.imsi_hash_key, 2048,
                       (word) (6*(cdma.imsi_hash_key & 0xfff)));

    #ifdef FEATURE_HDR
#error code not present
    #endif /* FEATURE_HDR */

    #ifdef FEATURE_IS2000_REL_A_CC
    if (OVHD_CHAN_IS_BCCH)
    {
      /* ------------------------------------------------------------
      ** Force to receive Extended CDMA Channel List Msg and ANSI-41
      ** System Parameter Msg. This is to ensure we rehash to correct
      ** CDMA channel and Forward Common Control Channel
      ** ------------------------------------------------------------ */
      cur_bs_ptr->rx.ecl_rx = FALSE;
      cur_bs_ptr->rx.mcrrpm_rx = FALSE;
      /* ----------------------------------------------------
      ** Following two lines is to force the rehashing of the
      ** forward common control channel.
      ** ---------------------------------------------------- */
      cdma.fccch_id = PRI_FCCCH;
      cdma.num_fccch_s = 1;
    }
    else
    #endif /* FEATURE_IS2000_REL_A_CC */

    {
     /* ------------------------------------------------------------
      ** Force to receive Extended CDMA Channel List Msg and System
      ** Parameter Msg. This is to ensure we rehash to correct CDMA
      ** channel and Paging Channel
      ** ------------------------------------------------------------ */
      cur_bs_ptr->rx.cl_rx = FALSE;
      cur_bs_ptr->rx.ecl_rx = FALSE;
      cur_bs_ptr->rx.sp_rx = FALSE;

      /* ----------------------------------------------------
      ** Following two lines is to force the rehashing of the
      ** paging channel.
      ** ---------------------------------------------------- */
      cdma.pagech = PRI_PAGECH;
      cdma.page_chan_s = 1;
    }
  }

} /* mccidl_proc_true_imsi */

/*===========================================================================

FUNCTION MCCIDL_PROC_EXT_SYS_PARM

DESCRIPTION
  This procedure processes a received Extended System Parameters Message as
  described in IS-95A/B section 6.6.2.2.5.  This procedure is used for Idle
  State and Update Overhead Information Substate of System Access State.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  This should only be executed by FEATURE_IS95B or greater capable mobiles.

/===========================================================================*/
word mccidl_proc_ext_sys_parm
(
  word new_state
    /* Current state being processed */
)
{
  word i; /* FOR LOOP COUNT */

  // Remember old p_rev_in_use.  mcc_update_bs_p_rev below may change p_rev_in_use
  byte old_p_rev_in_use = cdma.p_rev_in_use;

  // Note, the P_REV field in the message has been properly initialize when received,
  // except if an invalid band class has been received.  In such case, p_rev contains 0.
  if (cur_bs_ptr->csp.esp.p_rev != CAIX_P_REV_UNKNOWN)
  {
    mcc_update_bs_p_rev(cur_bs_ptr->csp.esp.p_rev);
  }

  if (cdma.p_rev_in_use != old_p_rev_in_use)
  {
    mcc_clear_ovhd_msgs_prev_mismatch();
  }

  #ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER && cur_bs_ptr->csp.esp.bcch_supported)
  {
    /* ----------------------------------------------------------
    ** If the Broadcast Control Channel is supported, the mobile
    ** station shall enter System Determination with a new system
    ** indication.
    ** ------------------------------------------------------- */
    event_report(EVENT_NEW_SYSTEM_IDLE_HANDOFF);

    /* ----------------------------------------------
    ** Inform system determination of new system exit
    ** ---------------------------------------------- */
    if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
         sd_ss_ind_cdma_opr_new_sys((sd_band_e_type) cdma.band_class,
         (sd_chan_type) cdma.cdmach, NULL)))
    {
      mcc_exit.exit = MCC_NEW_SYSTEM;
      MSG_MED("BCCH supported in esp, so enter SD", 0, 0, 0);
      return (CDMA_EXIT);
    }
  }
  #endif /* FEATURE_IS2000_REL_A */

  if (P_REV_IS_4_OR_GREATER)
  {
    if (cdma.mob_cai_rev < cur_bs_ptr->csp.esp.min_p_rev)
    {
      /* --------------------------------------------------------
      ** If the protocol revision level supported by the mobile
      ** statiom is less than the minimum protocol revision level
      ** supported by the base station, the mobile station shall
      ** enter the System Determination Substate with a protocol
      ** mismatch indication.
      ** -------------------------------------------------------- */
      MSG_HIGH( "Min CAI rev > mob_cai_rev", 0, 0, 0 );

      /* Event report: Protocol mismatch */
      event_report(EVENT_PROTOCOL_MISMATCH);

      /* ------------------------------------------------------------
       ** Inform system determination of Protocol Mismatch exit.
       ** Determine the next state to be processed based on returned
       ** SS-Action.
       ** ------------------------------------------------------------ */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
         sd_ss_ind_cdma_acq_prot_mis(NULL)))
      {
        mcc_exit.exit = MCC_INCOMP_CAI_REV;
        return (CDMA_EXIT);
      }
    }

    if ((cur_bs_ptr->csp.esp.nghbr_set_entry_info) ||
       (cur_bs_ptr->csp.esp.nghbr_set_access_info))
    {
      cur_bs_ptr->nl.num_nghbr_pns = cur_bs_ptr->csp.esp.nghbr_set_size;
    }

    /************************************************************************
    ** Set the ACCESS_ENTRY_HO and ACCESS_HO_ALLOWED fields of the NGHBR_REC
    ** based on the NGHBR_SET_ENTRY_INFO and NGHBR_SET_ACCESS_INFO fields.
    ************************************************************************/
    for (i = 0; (i < SRCH_NSET_MAX) && (i < cur_bs_ptr->nl.num_nghbr_pns); i++)
    {
      cur_bs_ptr->nl.nset[i].access_entry_ho =
        (cur_bs_ptr->csp.esp.nghbr_set_entry_info) ?
        cur_bs_ptr->csp.esp.access_entry_ho[i] : FALSE;

      cur_bs_ptr->nl.nset[i].access_ho_allowed =
        (cur_bs_ptr->csp.esp.nghbr_set_access_info) ?
        cur_bs_ptr->csp.esp.access_ho_allowed[i] : FALSE;
    }

    /* -----------------------------------------------------------------
    ** Processing of DELETE_FOR_TMSI bit as in IS-95B Section 6.6.2.2.5.
    ** The mobile station shall set all bits of TMSI_CODEs-p to '1' if
    ** all of the following are met:
    ** (1) The bits of TMSI_CODEs-p are not all equal to '1',
    ** (2) DELETE_FOR_TMSIs is equal to '1', and
    ** (3) ASSIGNING_TMSI_ZONE_LENs-p is not equal to TMSI_ZONE_LENs, or
    **     the least significant ASSIGNING_TMSI_ZONE_LENs-p octets of
    **     ASSIGNING_TMSI_ZONEs-p are not equal to TMSI_ZONEs.
    ** ------------------------------------------------------------------ */
    if (cur_bs_ptr->csp.esp.delete_for_tmsi &&
        (cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE) &&
        ((cdma.assigning_tmsi.zone_len != cur_bs_ptr->csp.esp.tmsi_zone_len) ||
         (qw_cmp(cur_bs_ptr->csp.esp.tmsi_zone, cdma.assigning_tmsi.info.tmsi_zone) != 0)))
    {
      cdma.assigning_tmsi.info.tmsi_code = INVALID_TMSI_CODE;

      /* Events update: TMSI assigned */
      event_report_payload(EVENT_TMSI_ASSIGNED,
        sizeof(event_tmsi_assigned_type), &cdma.assigning_tmsi.info.tmsi_code);
    }
  } /* end of P_REV check */

  /* -----------------------------------------------------------------------
  ** Process the TRUE IMSI fields in the Extended System Parameters Message
  ** ----------------------------------------------------------------------*/
  if (P_REV_IS_4_OR_GREATER)
  {
    mccidl_proc_true_imsi(cur_bs_ptr->csp.esp.imsi_t_supported);
  }

  /* reset the QPCH number, since we rec'd a new ESPM */
  cur_bs_ptr->qpch_num = QPCH_INVALID_CH;

  if (P_REV_IS_4_OR_GREATER)
  {
    mcc_ho_real.soft_slope = cur_bs_ptr->csp.esp.soft_slope;

    if (cur_bs_ptr->csp.esp.add_intercept & CAI_HO_INTERCEPT_SIGN_BIT)
    {
      /* extend the sign bit */
      mcc_ho_real.add_intercept =
        (int1)(cur_bs_ptr->csp.esp.add_intercept | CAI_HO_INTERCEPT_SIGN_EXT);
    }
    else
    {
      mcc_ho_real.add_intercept = (int1)cur_bs_ptr->csp.esp.add_intercept;
    }

    if (cur_bs_ptr->csp.esp.drop_intercept & CAI_HO_INTERCEPT_SIGN_BIT)
    {
      /* extend the sign bit */
      mcc_ho_real.drop_intercept =
        (int1)(cur_bs_ptr->csp.esp.drop_intercept | CAI_HO_INTERCEPT_SIGN_EXT);
    }
    else
    {
      mcc_ho_real.drop_intercept = (int1)cur_bs_ptr->csp.esp.drop_intercept;
    }
  }
  else
  {
    /* disable IS95B soft handoff */
    mcc_ho_real.soft_slope     =
    mcc_ho_real.add_intercept  =
    mcc_ho_real.drop_intercept = 0;
  }

  /* Calculate and store the operational sci */
  (void)mcc_set_operational_sci();  

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif

  if (!mccidl_fast_raho)
  {
    /* Update Searcher's handoff parameters */
    mccidl_send_srch_parms();

    /* Update Searcher's neighbor set */
    mccidl_send_srch_idle_nset();
  }

  /* -------------------------------------------------------------------------
  ** Initialize the setting of BS parameters that shared in both idle and
  ** traffic state, so that reference to those parameters inside access
  ** functions will always return correct values irrespective of state. 
  ** ------------------------------------------------------------------------- */
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

  #ifdef FEATURE_IS2000_REL_C
  if ( P_REV_IS_9_OR_GREATER )
  {
    cdma.use_sync_id = cur_bs_ptr->csp.esp.use_sync_id;
  }
  else
  {
    cdma.use_sync_id = FALSE;
  }
  #endif /* FEATURE_IS2000_REL_C */

  #ifdef FEATURE_BROADCAST_SMS
  /* Process the Broadcast configuration fields of ESPM */
  mccbcsms_proc_msg(CAI_EXT_SYS_PARM_MSG, cur_bs_ptr);
  #endif /* FEATURE_BROADCAST_SMS */

  /* Lastly, Process the Channel List message if it was deferred */
  /* It is better to do this in the end because this can change cur_bs_ptr
     and hence can change/invalidate the fields of ESPM */
  if (cl_proc_pending && CHAN_LIST_MSG_IS_RECVD)
  {
    new_state = mccidl_proc_cl_msg(new_state);
  }
  /* Process the Extended Channel List message if it was deferred */
  if (ecl_proc_pending && EXT_CHAN_LIST_MSG_IS_RECVD)
  {
    new_state = mccidl_proc_ecl_msg(new_state);
  }

  if (new_state != CDMA_EXIT)
  {
    /* Mark EPZID processing is pending. */
    mccidl_epzid_proc_pending = TRUE;
  }

  #if defined (FEATURE_IS2000_REL_C_AKA) || defined (FEATURE_IS2000_REL_C_MI)
#error code not present
  #endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_C_MI */

  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #endif /* FEATURE_IS2000_REL_A_AES */

  return (new_state);

} /* mccidl_proc_ext_sys_parm */

/*===========================================================================
FUNCTION MCCIDL_PROC_NGHBR_LIST_MSG

DESCRIPTION
  This procedure processes all Received Neighbor List Messages on paging channel only.

  Neighbor List Message           (NLM)
  Extended Neighbor List Message  (ENLM)
  General Neighbor List Message   (GNLM)

  This does not process UNLM because the format if this message is quite different

  The processing of these messages are defined in TIA/EIA - 95B in the
  following sections: 6.6.2.2.3, 6.6.2.2.7, 6.6.2.2.8.

  This procedure is used for Idle State and Update Overhead Information
  Substate of System Access State.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array and set proper configuration sequences.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_proc_nghbr_list_msg
(
  caii_rx_msg_type *msg_ptr,
    /* Pointer to System Parameter message  */
  word new_state
)
{
  word i;
    /* Counter for loop */
  word num_nghbr_pns = 0;
    /* Number of neighbor pilots in the message */

  /* Set General information about all the Neighbors in the List */
  mccidl_set_nghbr_list_info(msg_ptr);

  if (msg_ptr->gen.msg_type == CAI_NL_MSG)
  {
    num_nghbr_pns = msg_ptr->nghbr.num_nghbrs;
  }
  else if (msg_ptr->gen.msg_type == CAI_EXT_NL_MSG)
  {
    num_nghbr_pns = msg_ptr->ext_nghbr.num_nghbrs;
  }
  else if ((msg_ptr->gen.msg_type == CAI_GEN_NL_MSG) &&
           (P_REV_IS_4_OR_GREATER))
  {
    num_nghbr_pns = msg_ptr->gen_nghbr.num_nghbr;
  }

  /***************************************************************************
  ** The following access functions are called for Each Neighbor in the
  ** Received Neighbor List Message. The Instance of the corresponding Pilot
  ** and the received Message is sent to the Access functions for processing.
  ***************************************************************************/
  for (i = 0; (i < SRCH_NSET_MAX) && (i < cur_bs_ptr->nl.num_nghbr_pns) &&
       (i < num_nghbr_pns) ; i++)
  {
    mccidl_set_nghbr_pn_config(i, msg_ptr);  /* Set PN and CONFIG Information */
    mccidl_set_nghbr_srch_pri (i, msg_ptr);  /* Set the Search Priority */
    mccidl_set_nghbr_band_freq(i, msg_ptr);  /* Set the band and Frequency */
    mccidl_set_nghbr_timing   (i, msg_ptr);  /* Set the Timing offset information */
    mccidl_set_nghbr_srch_win (i, msg_ptr);  /* Set the Nghbr Search Window */
    mccidl_set_access_ho_info (i, msg_ptr);  /* Set the Access Handoff fields */
    mccidl_set_nghbr_srch_offset (i, msg_ptr);  /* Set the Nghbr Search offset */
  }

  /* Set the Analog Neighbor List Information */
  mccidl_set_analog_nghbrs(msg_ptr);

  if (!mccidl_fast_raho)
  {
    /* Update Searcher's neighbor set */
    mccidl_send_srch_idle_nset();
  }

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  return new_state;
} /* mccidl_proc_nghbr_list_msg() */

/*===========================================================================

FUNCTION MCCIDL_PROC_CL_MSG

DESCRIPTION
  This function processes the CDMA Channel List Message as described in
  IS-95A section 6.6.2.2.4.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_proc_cl_msg 
(
  word new_state
    /* Current state being processed */
)
{
  word hashed_cdmach;
  bs_info_id_type bs_info_id;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* -------------------------------------------------------------------------
  ** If SYS_PAR_MSG_SEQ s is current and
  ** If EXT_CHAN_LIST s is equal to 1, the mobile station ignores the message.
  ** If EXT_CHAN_LIST s is equal to 0, the mobile station processes the message
  ** ----------------------------------------------------------------------- */
  if (SP_MSG_IS_RECVD)
  {
    if (P_REV_IS_6_OR_GREATER && cur_bs_ptr->csp.sp.ext_chan_list)
    {
      /* Ignore this message, Extended Channel List will be processed instead */
      cl_proc_pending = FALSE;
      return new_state;
    }
    else if (ESP_MSG_NOT_RECVD)
    {
      /* ESP message is not received, Defer processing it until ESP is recvd. */
      cl_proc_pending = TRUE;
      return new_state;
    }
  }
  else
  {
    /* SP message is not received, Defer processing it until SP is recvd. */
    cl_proc_pending = TRUE;
    return new_state;
  }

  /* -----------------------------------------------
  ** The Channel List Message should never have zero
  ** entries, and if it has only one entry it need
  ** not necessarily be the current channel.
  ** ----------------------------------------------- */

  /* Hash channel value and compare to current */
  hashed_cdmach = cur_bs_ptr->cl.cdma_freq[hash(cdma.imsi_hash_key,
                                           cur_bs_ptr->cl.num_freq, 0)];

  /* Store the hash channel and band class value */
  cur_bs_ptr->hashed_cdmach = hashed_cdmach;
  cur_bs_ptr->hashed_band = cdma.band_class;

  if (cdma.cdmach != hashed_cdmach)
  {

    #ifdef FEATURE_GPS_BS_CAL_TEST
#error code not present
    #endif

    {
      if (mccidl_fast_raho)
      {
        /* --------------------------------------------------
        ** Set the Cheap Idle Handoff to FALSE if the Hashed
        ** CDMA channel is different than what the mobile is
        ** using.
        ** ------------------------------------------------ */
        mccidl_cheap_idle_ho = FALSE;
        MSG_HIGH("CL Hash! Not cheap IHO", 0, 0, 0);
      }
      else  /* No Fast RAHO */
      {
        cdma.cdmach = hashed_cdmach;
        cdma.block_or_sys = (nv_block_or_sys_enum_type) sdprl_map_chan_to_blksys(SD_MODE_CDMA, 
                                                                                 (sd_band_e_type) cdma.band_class, 
                                                                                 cdma.cdmach); 
          // func returns 32bit value, but values are low enough that 16 bits is OK
        cdma.page_chan_s = 1;

        MSG_HIGH("CL hash to freq %d", hashed_cdmach, 0, 0);

        if (cdma.pagech != PRI_PAGECH)
        {
          MSG_MED( "Switching back to primary Paging Channel", 0, 0, 0 );

          cdma.pagech = PRI_PAGECH;

        }

        /* --------------------------------------
        ** Since we have changed channels we need
        ** a new entry in the bs_info array.
        ** -------------------------------------- */
        bs_info_id.ovhd_chan = CAIX_PC;
        bs_info_id.pilot_pn = cur_bs_ptr->pilot_pn;
        bs_info_id.cdma_ch = cdma.cdmach;
        bs_info_id.band_class = cdma.band_class;
        bs_info_id.ovhd_walsh_ch = cdma.pagech;

        mcc_entry_init(&bs_info_id, FALSE);                       

        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */

        /* -----------------------------
        ** Tell SRCH the Active Set info
        ** ----------------------------- */
        mccidl_send_srch_idle_aset();

        /* --------------------------------------------------------------------
        ** IS95A told MS to trash all stored ovhds.
        ** IS95B told MS to treat stored ovhds as current if stored ovhds
        ** are valid according to T31M.
        ** Our implementation chooses to follow IS95A spec because IS95B spec 
        ** can potentially cause MS stuck forever in re-processing  
        ** stored overheads if the network setup causes MS to ping pong between
        ** two frequencies. 
        ** -------------------------------------------------------------------- */ 
        mcc_clear_ovhd_msgs();

	/* Hashed to a new frequency, need to process the overheads again */
	/* If the mobile is slotted mode, move back to idle_init */
	/* Exception of JCDMA phones which wont go back to process overheads */
	#ifdef FEATURE_PASSPORT_MODE
	if(mcc_passport_mode_is_jcdma() == FALSE)
        #endif /*  FEATURE_PASSPORT_MODE */
        {		
	   if(new_state == CDMA_SLOTTED)
	   {
              new_state = CDMA_IDLE_INIT;
 	   }
        }
        /* Forget about the (E)GSRM received and rescan, if any */ 
        gsr_ignored = FALSE;
        gsd_proc_pending = FALSE;
        egsr_ignored = FALSE;
        egsd_proc_pending = FALSE;
        rescan_ignored = FALSE;
      
      } /* No Fast RAHO */
    }
  } /* end if (cdma.cdmach != hashed_cdmach) */

  if (!NGHBR_LIST_MSGS_NOT_RECVD)
  {
    int i;
    boolean upd_nset_needed = FALSE;

    /*****************************************************
    ** Set the FREQ field of the NGHBR_REC for CONFIG = 2.
    ******************************************************/
    for (i = 0; (i < SRCH_NSET_MAX) && (i < cur_bs_ptr->nl.num_nghbr_pns); i++)
    {
      if ((cur_bs_ptr->nl.nset[i].config == 2) && 
          !cur_bs_ptr->nl.nset[i].freq_incl)
      {
        cur_bs_ptr->nl.nset[i].freq = cur_bs_ptr->cl.cdma_freq[0];
        cur_bs_ptr->nl.nset[i].band = cdma.band_class;
        cur_bs_ptr->nl.nset[i].freq_incl = TRUE;               
        upd_nset_needed = TRUE;
      }
    }

    if (upd_nset_needed && !mccidl_fast_raho)
    {
      /* Update Searcher's neighbor set */
      mccidl_send_srch_idle_nset();
    }
  }

  cl_proc_pending = FALSE;  // clear the pending flag now

  if(cdma.ho_state == CDMA_AEHO)
  {
    /* Mark the message that received before AEHO and is pending for
       response as received from new active channel. Do so to ensure  
       that mccidl_check_chan() will honor the system access for the 
       message response */

    mccidl_save_msg_rxed_chan();
    MSG_MED("CCLM processing. Updated rxed channel information.", 0, 0, 0);
  }

  return new_state;

} /* end mccidl_proc_cl_msg() */

/*===========================================================================

FUNCTION MCCIDL_PROC_ECL_MSG

DESCRIPTION
  This function processes the Extended CDMA Channel List Message as described in
  IS-2000 section 2.6.2.2.12.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_proc_ecl_msg
(
  word new_state
    /* Current state being processed */
)
{
  
  byte band_class, old_band;
  word tot_num_chan_in_first_chnlist = 0, tot_num_chan_in_second_chnlist = 0;
  byte num_entries_in_first_chnlist = 0, num_entries_in_second_chnlist = 0;
  cdma_int_chn_list_type new_entry;        /* Entry added to temp chn list */
  boolean td_select = FALSE;               /* Indicates if TD is supported */
  boolean qpch_rc_select = FALSE;          
  /* Hashed CDMA band and hashed CDMA freq */
  byte hashed_cdma_band, hash_ecclm_band_index = 0, hash_ecclm_freq_index = 0;
  word hashed_cdmach, old_freq;
  bs_info_id_type bs_info_id;
  boolean upd_nset_needed = FALSE;
  byte index, insert_index, band_index, freq_index;
  word hash_index;
  byte last_valid_index = 0xFF;
  cdma_bs_type *old_bs_ptr = NULL;

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */
  
  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    if (ANSI41SP_MSG_NOT_RECVD || MCRRP_MSG_NOT_RECVD)
    {
     /* ANSI-41 System Parameter Message or MC-RR System Parameter Message is
      * not received. Defer processing until those messages are received. */
      ecl_proc_pending = TRUE;
      return new_state;
    }
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  /* -------------------------------------------------------------------------
  ** For Paging Channel:
  **
  ** If System Parameter Message is current and EXT_CHAN_LISTs is equal to 0,
  ** the mobile station ignores the message.
  ** ------------------------------------------------------------------------*/
  {
    if (SP_MSG_IS_RECVD)
    {
      if (!P_REV_IS_6_OR_GREATER || !cur_bs_ptr->csp.sp.ext_chan_list)
      {
        ecl_proc_pending = FALSE;
        /* Ignore this message, CLM will be processed instead */
        return new_state;
      }
      else if (ESP_MSG_NOT_RECVD)
      {
        /* Extended System Parameter Message is not received. Defer processing
        until those messages are received */
        ecl_proc_pending = TRUE;
        return new_state;
      }
    }
    else
    {
      /* System Parameter message is not received. Defer processing until those
       * messages are received */
      ecl_proc_pending = TRUE;
      return new_state;
    }
  }

  /* Get the current band class */
  band_class = cdma.band_class;

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  {
    #ifdef FEATURE_IS2000_REL_A_CC  
    /* Check the MS capability to support TD */
    if (OVHD_CHAN_IS_BCCH && cur_bs_ptr->ecl.td_sel_incl &&
        mccap_is_csch_td_mode_supported(cur_bs_ptr->ecl.td_mode))
    {
      td_select = TRUE;
    }
    #endif /* FEATURE_IS2000_REL_A_CC */    

    /* Parse through the frequencies in ECCLM */
    for(index = 0; index < cur_bs_ptr->ecl.num_freq; index++)
    {
      new_entry.cdma_band = band_class;
      new_entry.cdma_freq = cur_bs_ptr->ecl.cdma_freq[index];
      /* Initialize weights to default = 1. In our implementation we prefer to
       * to have an entry of the cdma weight in the channel list structure so
       * that we dont have to repeat the entries in the channel list. For 
       * hashing we go over the cdma weights in the final list and add them
       * recursively to compute the total number of channels. This way we save
       * substantial amount of RAM */
      new_entry.cdma_freq_weight = 1;  
      /* Band index is not applicable for the frequency loop */
      new_entry.ecclm_band_index = BAND_INDEX_NA;
      new_entry.ecclm_freq_index = (byte) index;

      #ifdef FEATURE_IS2000_REL_D
#error code not present
      #endif /* FEATURE_IS2000_REL_D */
      {
        #ifdef FEATURE_IS2000_REL_A_CC
        /*-----------------------------------------------------------------
         ** If TD_SEL_INCL r is equal to 1, the mobile station shall
         ** select those channels with TD_HASH_IND matched with MS preference
         ** from the CDMA channel list as first subset.
         ** ----------------------------------------------------------------*/
        if (OVHD_CHAN_IS_BCCH && cur_bs_ptr->ecl.td_sel_incl)
        {
          /* If the td_hash_ind = 1 for this freq and MS does not support TD or
           * if the td_hash_ind = 0 for this freq and MS supports TD
           * then skip this freq and continue with the next freq */
          if((cur_bs_ptr->ecl.td_sel[index].td_hash_ind && !td_select) ||
             (!cur_bs_ptr->ecl.td_sel[index].td_hash_ind && td_select))
          {
            continue;
          }
        }
        #endif /* FEATURE_IS2000_REL_A_CC */

        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */
        {
          /* If the P_REV is less than 11, we dont sort the freq's in the 
           * channel list */
          insert_index = num_entries_in_first_chnlist;
          valid_chnlist[insert_index] = new_entry;
          num_entries_in_first_chnlist++;
          tot_num_chan_in_first_chnlist += new_entry.cdma_freq_weight;          
        }
      } 
    } /* for(index = 0 */ 
  } /* if(band_subclass_supported) */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  /* Uptil now we have generated the first channel list based on the band 
   * subclass, TD mode. This list is sorted in the ascending order of the
   * band and also the freq within each band if P_REV >= 11. Now we generate
   * the second channel list by filtering out freq based on RC criteria */

  /* -------------------------------------------------------------------
   ** If RC_QPCH_SEL_INCL r is equal to 1, the mobile station shall
   **  eliminate those channels with RC_QPCH_CAP_IND r = 0 from the CDMA
   ** channel list.
   ** ---------------------------------------------------------------- */
  qpch_rc_select = mccap_is_enhanced_operation_supported();

  if(cur_bs_ptr->ecl.rc_qpch_sel_incl && qpch_rc_select)
  {
    for(index = 0; index < num_entries_in_first_chnlist; index++)
    {
      band_index = valid_chnlist[index].ecclm_band_index;
      freq_index = valid_chnlist[index].ecclm_freq_index;

      #ifdef FEATURE_IS2000_REL_D        
#error code not present
      #endif /* FEATURE_IS2000_REL_D */

      /* Check for the RC criteria */
      if(((band_index == BAND_INDEX_NA) &&
          cur_bs_ptr->ecl.rc_qpch_hash_ind[freq_index]) 
         #ifdef FEATURE_IS2000_REL_D
#error code not present
         #endif /* FEATURE_IS2000_REL_D */
         )
      {
        num_entries_in_second_chnlist++;
        tot_num_chan_in_second_chnlist += 
          valid_chnlist[index].cdma_freq_weight;

        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */

        if(last_valid_index != 0xFF)
        {
          valid_chnlist[last_valid_index] = valid_chnlist[index];
          last_valid_index++;
        }
      }
      else
      {
        if(last_valid_index == 0xFF)
        {
          last_valid_index = index;
        }
      }
      
    } /* for(index = 0; */
  } /* if(cur_bs_ptr->ecl.rc_qpch_sel_incl && qpch_rc_select) */
  else
  {
    /* If QPCH is not supported by the BS or the MS we set the second
     * channel list equal to the first channel list */
    num_entries_in_second_chnlist = num_entries_in_first_chnlist;
    tot_num_chan_in_second_chnlist = tot_num_chan_in_first_chnlist;
    #ifdef FEATURE_IS2000_REL_D
#error code not present
    #endif /* FEATURE_IS2000_REL_D */
  }

  /* If the channel list is empty, we quit to system determination with a 
   * SYSTEM_LOST indication */
  if((num_entries_in_second_chnlist == 0) || 
     (tot_num_chan_in_second_chnlist == 0))
  {
    /* -------------------------------------------------------------
     ** Inform system determination of system lost and determines the
     ** next state to be processed based on the returned SS-Action.
     ** ------------------------------------------------------------- */
    if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
            sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_UNKNOWN, NULL ) )
       )
    {
       MSG_HIGH("ECL Empty! Doing System Lost", 0, 0, 0);
       mcc_exit.exit = MCC_SYSTEM_LOST;
       new_state    = CDMA_EXIT;
    }
    return new_state;
  }

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */
  {
    /* There is just a single band, so we use the first entry to get the a
     * hashed_cdma_band */
    hashed_cdma_band = valid_chnlist[0].cdma_band;
  }


  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */
  {
    /* There is just one band in the second channel list. So we directly hash
     * based on tot_num_chan_in_second_chnlist */
    
    hash_index = hash(cdma.imsi_hash_key, tot_num_chan_in_second_chnlist, 0); /*lint !e734 */

    #ifdef FEATURE_IS2000_REL_D
#error code not present
    #endif
    {
      hash_ecclm_band_index = valid_chnlist[hash_index].ecclm_band_index;
      hash_ecclm_freq_index = valid_chnlist[hash_index].ecclm_freq_index;
    }
  }

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  /* Store the hash freq and the hash band. Also store whether the TD mode is 
   * supported for the hashed cdma band and freq */
  cur_bs_ptr->hashed_band = hashed_cdma_band;
  td_select = FALSE;

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif
  {
    hashed_cdmach = 
      cur_bs_ptr->hashed_cdmach = 
      cur_bs_ptr->ecl.cdma_freq[hash_ecclm_freq_index];
    
    #ifdef FEATURE_IS2000_REL_A_CC  
    /* Check the MS capability to support TD */
    if (OVHD_CHAN_IS_BCCH && cur_bs_ptr->ecl.td_sel_incl &&
        mccap_is_csch_td_mode_supported(cur_bs_ptr->ecl.td_mode))
    {
      td_select = TRUE;
    }
    #endif /* FEATURE_IS2000_REL_A_CC */    
  }

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    /* -----------------------------------------------------------
    ** Check whether Broadcast Control Channel's configuration has
    ** changed.
    ** ----------------------------------------------------------- */
    if (td_select != cur_bs_ptr->td_supported)
    {
      /* -------------------------------------
       ** Update Broadcast Control Channel info
       ** ------------------------------------- */
      if (td_select)
      {
        if(hash_ecclm_band_index == BAND_INDEX_NA)
        {
          cdma.td_mode = cur_bs_ptr->ecl.td_mode;
          cdma.td_power_level = 
            cur_bs_ptr->ecl.td_sel[hash_ecclm_freq_index].td_power_level;
        }
        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */
        cdma.bcch = cdma.bcch_td;
        cdma.brat = cdma.brat_td;
        cdma.bcch_code_rate = cdma.bcch_code_rate_td;
      }
      else
      {
        cdma.bcch = cdma.bcch_non_td;
        cdma.brat = cdma.brat_non_td;
        cdma.bcch_code_rate = cdma.bcch_code_rate_non_td;
      }
    }
  }
  #endif /* FEATURE_IS2000_REL_A_CC */

  /* Hash to the CDMA band and channel */
  if ((cdma.cdmach != cur_bs_ptr->hashed_cdmach) || 
      (cdma.band_class != hashed_cdma_band))
  {
    
    #ifdef FEATURE_GPS_BS_CAL_TEST
#error code not present
    #endif

    {
      if (mccidl_fast_raho)
      {
        /* --------------------------------------------------
         ** Set the Cheap Idle Handoff to FALSE if the Hashed
         ** CDMA channel is different than what the mobile is
         ** using.
         ** ------------------------------------------------ */
        mccidl_cheap_idle_ho = FALSE;
        MSG_HIGH("ECL Hash! Not cheap IHO",0,0,0);
      }
      else  /* No Fast RAHO */
      {
        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */

        /* Store the pointer to the cur_bs_ptr */
        old_bs_ptr = cur_bs_ptr;
        old_band = cdma.band_class;
        old_freq = cdma.cdmach;
        cdma.cdmach = hashed_cdmach;
        cdma.band_class = hashed_cdma_band;
        cdma.block_or_sys = 
          (uint16) sdprl_map_chan_to_blksys(
            SD_MODE_CDMA, (sd_band_e_type) cdma.band_class, cdma.cdmach); 
        
        MSG_HIGH( "ECL hash to freq %d", hashed_cdmach, 0, 0 );
        
        #ifdef FEATURE_IS2000_REL_A_CC
        if (OVHD_CHAN_IS_BCCH)
        {
          /* ------------------------------------------------------------
          ** Following two lines is to force the rehashing of the Forward
          ** Common Control Channel.
          ** ------------------------------------------------------------ */
          cdma.num_fccch_s = 1;

          if (cdma.fccch_id != PRI_FCCCH)
          {
            MSG_MED( "Switching back to primary FCCCH", 0, 0, 0 );
            cdma.fccch_id = PRI_FCCCH;
          }
        }
        else
        #endif /* FEATURE_IS2000_REL_A_CC */
        {
          cdma.page_chan_s = 1;

          if (cdma.pagech != PRI_PAGECH)
          {
            MSG_MED( "Switching back to primary Paging Channel", 0, 0, 0 );
            cdma.pagech = PRI_PAGECH;
          }
        }
        
        /* --------------------------------------
        ** Since we have changed channels we need
        ** a new entry in the bs_info array.
        ** -------------------------------------- */
        bs_info_id.ovhd_chan = cur_bs_ptr->ovhd_chan_info.ovhd_chan;
        bs_info_id.pilot_pn = cur_bs_ptr->pilot_pn;
        bs_info_id.cdma_ch = cdma.cdmach;
        bs_info_id.band_class = cdma.band_class;
        bs_info_id.ovhd_walsh_ch =
        #ifdef FEATURE_IS2000_REL_A_CC
          (OVHD_CHAN_IS_BCCH) ? cdma.bcch :
        #endif /* FEATURE_IS2000_REL_A_CC */
          cdma.pagech;
        
        mcc_entry_init(&bs_info_id, td_select);
       
        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */
       
        /* --------------------------------------
        ** Tell SRCH to go to this new RF channel
        ** -------------------------------------- */
        mccidl_send_srch_idle_aset();

        /* --------------------------------------------------------------------
        ** IS95A told MS to trash all stored ovhds.
        ** IS95B told MS to treat stored ovhds as current if stored ovhds
        ** are valid according to T31M.
        ** Our implementation chooses to follow IS95A spec because IS95B spec 
        ** can potentially cause MS stuck forever in re-processing  
        ** stored overheads if the network setup causes MS to ping pong between
        ** two frequencies. 
        ** -------------------------------------------------------------------*/ 
        mcc_clear_ovhd_msgs();

	/* Hashed to a new frequency, need to process the overheads again */
	/* If the mobile is slotted mode, move back to idle_init */
        /* Exception of JCDMA phones which wont go back to process overheads */
	#ifdef FEATURE_PASSPORT_MODE
        if(mcc_passport_mode_is_jcdma() == FALSE)
	#endif /*  FEATURE_PASSPORT_MODE */
        {		
	   if(new_state == CDMA_SLOTTED)
	   {
              new_state = CDMA_IDLE_INIT;
	   }
        }
        /* Forget about the (E)GSRM received and rescan, if any */ 
        gsr_ignored = FALSE;
        gsd_proc_pending = FALSE;
        egsr_ignored = FALSE;
        egsd_proc_pending = FALSE;
        rescan_ignored = FALSE;

        /* If the band or freq has changed in channel hashing we also create a 
         * temp NLM on the new system. We do a blind copy of the NLM on the 
         * current system to the new system. This is done because the hashed 
         * freq may not be a vertical neighbor. So to assist in idle handoff 
         * we copy the current BS NLM to the new BS */

        /* --------------------------------------------------------------------
         ** Mark we have a temporary neighbor list. It will be replaced with 
         ** real one when it comes.
         ** ------------------------------------------------------------------*/

        if((old_band != hashed_cdma_band) ||
           (old_freq != hashed_cdmach))
        {
          #ifdef FEATURE_IS2000_REL_A_CC
          if (OVHD_CHAN_IS_BCCH)
          {
            cur_bs_ptr->rx.unlm_rx = TRUE;
            cur_bs_ptr->unlm_config_msg_seq = TEMP_NL_SEQ;
          }
          else
          #endif /* FEATURE_IS2000_REL_A_CC */
          {
            cur_bs_ptr->rx.nl_rx = TRUE; /* Mark we have temporary NL */
            cur_bs_ptr->nl_config_msg_seq = TEMP_NL_SEQ;
          }
        
          /* Assume same pilot increment same as for last base station */
          cur_bs_ptr->nl.pilot_inc = old_bs_ptr->nl.pilot_inc;
          cur_bs_ptr->nl.num_nghbr_pns = old_bs_ptr->nl.num_nghbr_pns;
        
          for(index=0; index < cur_bs_ptr->nl.num_nghbr_pns; index++)
          {
            cur_bs_ptr->nl.nset[index] = old_bs_ptr->nl.nset[index];
          }
        }      
               
      } /* No Fast RAHO */
    }
  } /* end if (cdma.cdmach != hashed_cdmach) */

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    /* ------------------------------------------------------------
    ** In case of TD/non-TD changed without CDMA frequency changed,
    ** we need to update cur_bs_ptr->td_supported explicitly.
    ** ------------------------------------------------------------ */
    if (td_select != cur_bs_ptr->td_supported)
    {
      if (mccidl_fast_raho)
      {
        /* --------------------------------------------------
        ** Set the Cheap Idle Handoff to FALSE if TD support
        ** changed.
        ** ------------------------------------------------ */
        mccidl_cheap_idle_ho = FALSE;
        MSG_HIGH("TD support changed! Not cheap IHO",0,0,0);
      }
      else
      {
        cur_bs_ptr->td_supported = td_select;
        /* -----------------------------
        ** Update Search Active Set info
        ** ----------------------------- */
        mccidl_send_srch_idle_aset();
      }
    }
    
    if (!UNIV_NGHBR_LIST_MSG_NOT_RECVD) 
    {
      /*****************************************************
       ** Set the FREQ field of the NGHBR_REC for CONFIG = 2.
       ******************************************************/
      for (index = 0; 
            (index < SRCH_NSET_MAX) && (index < cur_bs_ptr->nl.num_nghbr_pns);
             index++)
      {
        if ((cur_bs_ptr->nl.nset[index].config == 2) && 
            !cur_bs_ptr->nl.nset[index].freq_incl)
        {
          cur_bs_ptr->nl.nset[index].freq = cur_bs_ptr->ecl.cdma_freq[0];
          upd_nset_needed = TRUE;
        }
      }
      
      if (upd_nset_needed && !mccidl_fast_raho)
      {
        /* Update Searcher's neighbor set */
        mccidl_send_srch_idle_nset();
      }
    }
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  {
    if (!NGHBR_LIST_MSGS_NOT_RECVD) 
    {
      /*****************************************************
      ** Set the FREQ field of the NGHBR_REC for CONFIG = 2.
      ******************************************************/
      for (index = 0; 
            (index < SRCH_NSET_MAX) && (index < cur_bs_ptr->nl.num_nghbr_pns);
             index++)
      {
        if ((cur_bs_ptr->nl.nset[index].config == 2) && 
            !cur_bs_ptr->nl.nset[index].freq_incl)
        {
          cur_bs_ptr->nl.nset[index].freq = cur_bs_ptr->ecl.cdma_freq[0];
          upd_nset_needed = TRUE;
        }
      }
      
      if (upd_nset_needed && !mccidl_fast_raho)
      {
        /* Update Searcher's neighbor set */
        mccidl_send_srch_idle_nset();
      }
    }
  }

  ecl_proc_pending = FALSE;
  
  if(cdma.ho_state == CDMA_AEHO)
  {
    /* Mark the message that received before AEHO and is pending for
       response as received from new active channel. Do so to ensure  
       that mccidl_check_chan() will honor the system access for the 
       message response */
    mccidl_save_msg_rxed_chan();
    MSG_MED("ECCLM processing. Updated rxed channel information.", 0, 0, 0);
  }

  return new_state;

} /* end mccidl_proc_ecl_msg() */

/*===========================================================================

FUNCTION MCCIDLE_CLEAR_GLOB_REDIR_REC_LIST

DESCRIPTION
  This function initialize all fields in mccidl_glob_redir_rec_list to 0.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_clear_glob_redir_rec_list
(
  void
)
{
  uint8 rec_index;

  for (rec_index=0; rec_index<MCC_MAX_REDIR_RECS; rec_index++) 
  {
    mccidl_glob_redir_rec_list.rec_list[rec_index].record_type = SD_REDIR_REC_NDSS_OFF;
    memset(&(mccidl_glob_redir_rec_list.rec_list[rec_index].rec), 0,
      sizeof(mccidl_glob_redir_rec_list.rec_list[rec_index].rec)); 
    mccidl_glob_redir_rec_list.rec_list[rec_index].is_delete_tmsi = FALSE;
  }

  mccidl_glob_redir_rec_list.num_recs = 0;

} /* mccidl_clear_glob_redir_rec_list */

/*===========================================================================

FUNCTION MCCIDL_ADD_TO_GLOB_REDIR_REC_LIST

DESCRIPTION
  This function adds a redirection rec to the mccidl_glob_redir_rec_list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_add_to_glob_redir_rec_list
(
  sd_redir_rec_s_type input_redir_rec
)
{
  switch (input_redir_rec.record_type) 
  {
    case SD_REDIR_REC_CDMA:
      /* If it's a CDMA record, copy the CDMA record contains. */
      mccidl_glob_redir_rec_list.rec_list[mccidl_glob_redir_rec_list.num_recs].rec.cdma_rec = 
        input_redir_rec.rec.cdma_rec;
      break;

    case SD_REDIR_REC_AMPS:
      /* If it's an AMPS record, copy the AMPS record contains. */
      mccidl_glob_redir_rec_list.rec_list[mccidl_glob_redir_rec_list.num_recs].rec.amps_rec = 
        input_redir_rec.rec.amps_rec;
      break;

    default:
      MSG_HIGH ("Redirection type %d not supported. Ignore!", 
                input_redir_rec.record_type, 0, 0);
      return;

  } /* switch (input_redir_rec.record_type) */

  /* Copy the rest of fields in the record */
  mccidl_glob_redir_rec_list.rec_list[mccidl_glob_redir_rec_list.num_recs].record_type = 
    input_redir_rec.record_type;
  mccidl_glob_redir_rec_list.rec_list[mccidl_glob_redir_rec_list.num_recs].is_delete_tmsi = 
    input_redir_rec.is_delete_tmsi;

  mccidl_glob_redir_rec_list.num_recs++;

  MSG_LOW("Added redirection record to GLOB_REDIR_REC_LIST", 0, 0, 0);
  
} /* mccidl_add_to_glob_redir_rec_list */

/*===========================================================================

FUNCTION MCCIDL_CONSTRUCT_REDIR_REC_LIST_FROM_GSR

DESCRIPTION
  This function processes the redirection record in the GSRM and adds
  the valid records to mccidl_globa_redir_rec_list so that it can be passed to 
  SD later.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The number of redirection records have been added to mccidl_glob_redir_rec_list.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 mccidl_construct_redir_rec_list_from_gsr 
(
  void
)
{
  sd_redir_rec_s_type curr_redir_rec;
  uint8 chan_index;
  
  /* Clear GLOB_REDIRECT_REC_LIST */
  mccidl_clear_glob_redir_rec_list();

  MSG_MED("Processing GSR ...", 0, 0, 0);

  /* -----------------------------------------------------------
  ** If the subfield corresponding to the access overload class,
  ** ACCOLCp, of the mobile station is set equal to '1' in the
  ** REDIRECT_ACCOLCr field of the received message, the mobile
  ** station shall enter the System Determination Substate with
  ** a redirection indication.
  ** ----------------------------------------------------------- */
  if ((cur_bs_ptr->gsd.redirect_accolc >> (15 - cdma.accolc)) & 0x1)
  {
    /* Add the valid record to the redir_rec_list */

    switch ( cur_bs_ptr->gsd.record_type )
    {
      case CAI_CDMA_REDIRECTION_REC:
      {
        curr_redir_rec.record_type = (sd_redir_rec_type_e_type)cur_bs_ptr->gsd.record_type;
        curr_redir_rec.is_delete_tmsi = cur_bs_ptr->gsd.delete_tmsi;
        curr_redir_rec.rec.cdma_rec.band_class = 
          (sys_band_class_e_type)cur_bs_ptr->gsd.redir.rec2.band_class;
        curr_redir_rec.rec.cdma_rec.expected_sid = cur_bs_ptr->gsd.redir.rec2.expected_sid;
        curr_redir_rec.rec.cdma_rec.expected_nid = cur_bs_ptr->gsd.redir.rec2.expected_nid;
        curr_redir_rec.rec.cdma_rec.num_chans = cur_bs_ptr->gsd.redir.rec2.num_chans;

        for (chan_index = 0; chan_index < cur_bs_ptr->gsd.redir.rec2.num_chans; chan_index++) 
        {
          curr_redir_rec.rec.cdma_rec.chan[chan_index] = 
            cur_bs_ptr->gsd.redir.rec2. cdma_chan[chan_index];
        }

        /* Add the valid rec to GLOB_REDIR_REC_LIST */
        mccidl_add_to_glob_redir_rec_list(curr_redir_rec);

        break;
      }

      case CAI_ANALOG_REDIRECTION_REC:
      {
        curr_redir_rec.record_type = (sd_redir_rec_type_e_type)cur_bs_ptr->gsd.record_type;
        curr_redir_rec.is_delete_tmsi = cur_bs_ptr->gsd.delete_tmsi;
        curr_redir_rec.rec.amps_rec.expected_sid = cur_bs_ptr->gsd.redir.rec1.expected_sid;
        curr_redir_rec.rec.amps_rec.is_ignore_cdma = cur_bs_ptr->gsd.redir.rec1.ignore_cdma;
        curr_redir_rec.rec.amps_rec.sys_ord =
          (sd_ss_sys_ord_e_type)cur_bs_ptr->gsd.redir.rec1.sys_ordering;
        curr_redir_rec.rec.amps_rec.max_redirect_delay = cur_bs_ptr->gsd.redir.rec1.max_redirect_delay;

        /* Add the valid rec to GLOB_REDIR_REC_LIST */
        mccidl_add_to_glob_redir_rec_list(curr_redir_rec);

        break;
      }

      default:
        MSG_HIGH ("Redirection type %d not supported.", 
          cur_bs_ptr->gsd.record_type, 0, 0);
        break;
        
    }
  }

  return mccidl_glob_redir_rec_list.num_recs;

} /* mccidl_construct_glob_redir_rec_list_from_gsr */ 

/*===========================================================================

FUNCTION MCCIDL_PROC_GSR

DESCRIPTION
  This function processes the global service redirection message as described
  in IS-95A section 6.6.2.2.6.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_proc_gsr
(
  word new_state
    /* Current state being processed */
)
{
  mcc_redir_info_type redir_info; /* Redirection info */

  /* ------------------------------------------------------------
  ** Redirection Record Type must equal 1 or 2 (see IS-95A table
  ** 7.7.2.3.2.16-1) and if type 2 the Band Class value must be
  ** a band supported by the phone (see TSB58 table 6.1-1).
  ** However, we are going to allow the system determination to
  ** consider that.
  ** Redirection Record Type is also admits to 3 (JTACS) in 95B.
  ** ------------------------------------------------------------ */

  /* -----------------------------------------------------------------
  ** If the P_REV_IN_USE s is equal to or greater than 6, the mobile
  ** station shall ignore this message, if any of the following
  ** is true: a) EXT_GLOBAL_REDIRECTs = 1, b) EXCL_P_REV_MSr = 1
  ** -------------------------------------------------------------- */
  if (SP_MSG_IS_RECVD)
  {
    if (!cur_bs_ptr->csp.sp.global_redirect ||
        (P_REV_IS_6_OR_GREATER && cur_bs_ptr->csp.sp.ext_global_redirect) ||
        cur_bs_ptr->gsd.excl_p_rev_ms)
    {
      gsd_proc_pending = FALSE; //clear the pending flag now
      gsr_ignored = FALSE;      //clear the ignored flag now
      return (new_state);
    }
  }
  /* --------------------------------------------------------------------
  ** Even if system parameter is not received and EXCL_P_REV_MSr = 1, the
  ** mobile station shall ignore this message.
  ** -------------------------------------------------------------------- */
  else if (cur_bs_ptr->gsd.excl_p_rev_ms)
  {
    gsd_proc_pending = FALSE; //clear the pending flag now
    gsr_ignored = FALSE;      //clear the ignored flag now
    return (new_state);
  }
  else
  {
    /* SP message is not received, deferr processing until received. */
    gsd_proc_pending = TRUE;
    return (new_state);
  }

  if  (mccidl_construct_redir_rec_list_from_gsr() > 0)
  {
    /* --------------------------------------------------------
    ** Ignore Global Service Redirection
    ** until we have selected a system.
    ** -------------------------------------------------------- */
    if ( ( new_state == CDMA_IDLE_INIT ) || !mcc_is_ovhd_msg_okay() )  /*lint !e641 */
    {
      MSG_MED("Delay GSR proc till system selected", 0, 0, 0);

      gsd_proc_pending = FALSE;
      gsr_ignored = TRUE;
      
      if ( MAIN_STATE(new_state) == CDMA_IDLE ) 
      {
        /* In case GSRM is ignored, we need to change state or
           set relevant flag to trigger GSRM be processed later. */ 
        if ( new_state == CDMA_OVHD ) /*lint !e641 */
        {
          oh_sd_rpt_pending_in_upd_ovhd = TRUE;
        }
        else if ( new_state != CDMA_IDLE_INIT ) /*lint !e641 */
        {
          new_state = CDMA_IDLE_INIT; /*lint !e641 */
          if ( !mccidl_fast_raho )
          {
            /* Start the BS insanity timer */
            mccidl_set_bs_insanity_timer();
          }
        }
      }

      return (new_state);
    } /* if ( ( new_state == CDMA_IDLE_INIT ) || !mcc_is_ovhd_msg_okay() ) */


    /* Events report: Redirection */
    event_report(EVENT_REDIRECTION);

    redir_info.return_if_fail = cur_bs_ptr->gsd.return_if_fail;
    redir_info.num_recs = mccidl_glob_redir_rec_list.num_recs;
    redir_info.redir_rec_list = mccidl_glob_redir_rec_list.rec_list;

    /* ------------------------------------------
    ** Inform system determination of redirection
    ** ------------------------------------------ */
    if (!mcc_inform_sd_redirection(SD_SS_REDIR_CDMA_GSRDM, &redir_info))
    {
      MSG_MED("SD rejected redirection", 0, 0, 0);
    }
    /* ------------------------------------------------------------------
    ** Inform system determination of done with accept/reject redirection
    ** and determines the next state to be processed based on SS-Action.
    ** ------------------------------------------------------------------ */
    if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
        sd_ss_ind_cdma_opr_acpt_rej(NULL)))
    {
      new_state = CDMA_EXIT;
      mcc_exit.exit = MCC_REDIRECTION;
    }

  } /* if (mccidl_construct_redir_rec_list_from_gsr() > 0) */

  gsr_ignored = FALSE; // clear the ignore flag now
  gsd_proc_pending = FALSE; //clear the pending flag now

  return (new_state);

} /* mccidl_proc_gsr */

#ifdef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
/*===========================================================================

FUNCTION MCCIDL_SET_EGSR_SYNCH_WORKAROUND

DESCRIPTION
  This function sets the global variable egsr_is_synch_workaround to TRUE
  and record the index of the Synch Workaround Record.  It also reset
  mcc_redir_rec_list.num_recs to '0'.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  mcc_redir_rec_list.num_recs is set to '0'.

===========================================================================*/
void mccidl_set_egsr_synch_workaround
(
  uint8 rec_index
)
{
  /* Mark EGSR as synch workaround */
  mccidl_synch_workaround.egsr_is_synch_workaround = TRUE;
    
  /* Record the index of the Synch Workaround Record (the same as 
  ** number of redirect records on REDIR_REC_LIST. */

  mccidl_synch_workaround.egsr_rec_index = rec_index;

  /* EGSRM is used for synch workaround, no redirection records to pass to SD. */
  mccidl_glob_redir_rec_list.num_recs = 0;

} /* mccidl_set_egsr_synch_workaround */
#endif /* FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND */

/*===========================================================================

FUNCTION MCCIDL_CONSTRUCT_REDIR_REC_LIST_FROM_EGSR

DESCRIPTION
  This function processes the redirection records in the EGSRM and adds
  the valid records to redir_rec_list so that it can be passed to SD later.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The number of redirection records have been added to redir_rec_list.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccidl_construct_redir_rec_list_from_egsr 
(
  void
)
{
  #ifdef FEATURE_IS2000_REL_D 
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  sd_redir_rec_s_type curr_redir_rec;
  #ifdef FEATURE_TREX
#error code not present
  #endif /* FEATURE_TREX */

  /* Clear GLOB_REDIRECT_REC_LIST */
  mccidl_clear_glob_redir_rec_list();

  #ifdef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
  mccidl_synch_workaround.egsr_is_synch_workaround = FALSE;
  #endif /* FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND */

  MSG_MED("Processing EGSR ...", 0, 0, 0);

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */
  {
    MSG_HIGH("PREV_IN_USE < 11; needs only look at the 1st redirection record",
             0, 0, 0);
  }

  /* ===========================================================
  ** Process the first redirect record in the EGSRM 
  ** =========================================================== */

  MSG_LOW("First record:", 0, 0, 0);
  #ifdef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
  if(mccidl_is_egsr_synch_workaround(&(cur_bs_ptr->egsd.first)))
  {
    /* Mark EGSR as synch workaround and 
    ** Record the index of the Synch Workaround Record */
    mccidl_set_egsr_synch_workaround(0);
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND */
  
  /* -----------------------------------------------------------
  ** the mobile should only consider the record if the subfield 
  ** corresponding to the access overload class, ACCOLCp, of 
  ** the mobile station is set equal to '1' in the
  ** REDIRECT_ACCOLCr field of the received message.
  ** ----------------------------------------------------------- */

  if ((cur_bs_ptr->egsd.first.redirect_accolc >> (15 - cdma.accolc)) & 0x1) 
  {
    /* -----------------------------------------------------------------------
    ** If MOB_P_REV p is not in the redirection mobile protocol revision range
    ** (i.e., REDIRECT_P_REV_INCL r = 1 and EXCL_P_REV_IND r = 0, and
    **  MOB_P_REV p < REDIRECT_P_MIN r or MOB_P_REV p > REDIRECT_P_MAX r ).
    ** (see IS2000 section 2.6.2.2.11), the mobile station shall ignore the 
    ** record and proceed to the next record.
    ** -------------------------------------------------------------------- */
    if (cur_bs_ptr->egsd.first.redirect_p_rev_incl &&
        !cur_bs_ptr->egsd.first.excl_p_rev_ind &&
        ((cdma.mob_cai_rev < cur_bs_ptr->egsd.first.redirect_p_min) ||
         (cdma.mob_cai_rev > cur_bs_ptr->egsd.first.redirect_p_max)))
    { 
      MSG_MED("MOB_P_REVp is not in the redirection mobile protocol revision range",
              0, 0, 0);
    }

    /* -----------------------------------------------------------------------
    ** If MOB_P_REV p is not in the redirection mobile protocol revision range
    ** (i.e., REDIRECT_P_REV_INCL r = 1 and EXCL_P_REV_IND r = 1, and
    **  (REDIRECT_P_MIN r <=MOB_P_REV p <= REDIRECT_P_MAX r ), the mobile
    ** station shall ignore the record and proceed to the next record.
    ** -------------------------------------------------------------------- */
    else if (cur_bs_ptr->egsd.first.redirect_p_rev_incl &&
             cur_bs_ptr->egsd.first.excl_p_rev_ind &&
             ((cdma.mob_cai_rev >= cur_bs_ptr->egsd.first.redirect_p_min) &&
              (cdma.mob_cai_rev <= cur_bs_ptr->egsd.first.redirect_p_max)))
    {
      MSG_MED("MOB_P_REVp is not in the redirection mobile protocol revision range",
              0, 0, 0);
    }

    else
    {
      /* Add the valid record to the redir_rec_list */
      switch (cur_bs_ptr->egsd.first.record_type) 
      {
        case CAI_CDMA_REDIRECTION_REC:
          /* Check if band/subclass/chans is supported and identify a list of
          ** freqs that the mobile supported and place them a SD format of 
          ** record - curr_redir_rec. */
          if (mcc_cdma_chans_are_supported(&cur_bs_ptr->egsd.first.redir.rec2,
                &curr_redir_rec.rec.cdma_rec))
          {
            /* Add the valid rec to REDIR_REC_LIST */
            curr_redir_rec.record_type = SD_REDIR_REC_CDMA;
            curr_redir_rec.is_delete_tmsi = cur_bs_ptr->egsd.first.delete_tmsi;

            mccidl_add_to_glob_redir_rec_list(curr_redir_rec);

            #ifdef FEATURE_IS2000_REL_D
#error code not present
            #endif /* FEATURE_IS2000_REL_D */

          }
          break;

        case CAI_ANALOG_REDIRECTION_REC:

          /* Add the valid rec to REDIR_REC_LIST */
          curr_redir_rec.record_type = SD_REDIR_REC_AMPS;
          curr_redir_rec.is_delete_tmsi = cur_bs_ptr->egsd.first.delete_tmsi;

          curr_redir_rec.rec.amps_rec.expected_sid = 
            cur_bs_ptr->egsd.first.redir.rec1.expected_sid;
          curr_redir_rec.rec.amps_rec.is_ignore_cdma =
            cur_bs_ptr->egsd.first.redir.rec1.ignore_cdma;
          curr_redir_rec.rec.amps_rec.sys_ord =
            (sd_ss_sys_ord_e_type) cur_bs_ptr->egsd.first.redir.rec1.sys_ordering;
          curr_redir_rec.rec.amps_rec.max_redirect_delay =
            cur_bs_ptr->egsd.first.redir.rec1.max_redirect_delay;

          mccidl_add_to_glob_redir_rec_list(curr_redir_rec);

          #ifdef FEATURE_IS2000_REL_D
#error code not present
          #endif /* FEATURE_IS2000_REL_D */

          break;

        default:
          MSG_HIGH ("Redirection type %d not supported.", 
                    cur_bs_ptr->egsd.first.record_type, 0, 0);
          break;
      } /* switch (cur_bs_ptr->egsd.first.record_type) */
    }
  } /* if ((cur_bs_ptr->egsd.first.redirect_accolc >> 
            (15 - cdma.accolc)) & 0x1) */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */
      
  MSG_MED("%d applicable redirection records in the EGSRM", 
          mccidl_glob_redir_rec_list.num_recs, 0, 0);

  /* ===========================================================
  ** All redirection records have been processed. If there are 
  ** records applied to the mobile station, it shall enter the
  ** System Determination Substate with a redirection indication.
  ** =========================================================== */

  #ifdef FEATURE_TREX
#error code not present
  #endif /* FEATURE_TREX */
  
  return mccidl_glob_redir_rec_list.num_recs;
    
} /* mccidl_construct_redir_rec_list_from_egsr */

/*===========================================================================

FUNCTION MCCIDL_PROC_EGSR

DESCRIPTION
  This function processes the extended global service redirection message as
  described in IS-2000 section 2.6.2.2.11

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_proc_egsr
(
  word new_state
    /* Current state being processed */
)
{
  mcc_redir_info_type redir_info; /* Redirection info */

  /* If System Parameter Message or MC-RR System Parameter Message
     is not received, defer processing until SP or MCRRSP is received. */

  if ( (OVHD_CHAN_IS_PCH && !SP_MSG_IS_RECVD) 
       #ifdef FEATURE_IS2000_REL_A_CC
       || (OVHD_CHAN_IS_BCCH && !MCRRP_MSG_IS_RECVD) 
       #endif /* FEATURE_IS2000_REL_A_CC */
     ) 
  {
    MSG_ERROR("Delay EGSR proc till SP or MCRRSP is received", 0, 0, 0);
    egsd_proc_pending = TRUE;
    return (new_state);
  }

  if (cur_bs_ptr->csp.sp.ext_global_redirect) 
  {

    if (mccidl_construct_redir_rec_list_from_egsr() > 0) 
    {
      
      if ( ( new_state == CDMA_IDLE_INIT ) || !mcc_is_ovhd_msg_okay() ) /*lint !e641 */
      {
        /* --------------------------------------------------------
        ** Ignore Extended Global Service Redirection
        ** until we have selected a system.
        ** -------------------------------------------------------- */
        MSG_MED("Delay EGSR proc till system selected", 0, 0, 0);

        egsd_proc_pending = FALSE;
        egsr_ignored = TRUE;
    
        if ( MAIN_STATE(new_state) == CDMA_IDLE ) 
        {
          /* In case EGSRM is ignored, we need to change state or
             set relevant flag to trigger EGSRM be processed later. */ 
          if ( new_state == CDMA_OVHD ) /*lint !e641 */
          {
            oh_sd_rpt_pending_in_upd_ovhd = TRUE;
          }
          else if ( new_state != CDMA_IDLE_INIT ) /*lint !e641 */
          {
            new_state = CDMA_IDLE_INIT; /*lint !e641 */

            if ( !mccidl_fast_raho )
            {
              /* Start the BS insanity timer */
              mccidl_set_bs_insanity_timer();
            }
          } /* if ( MAIN_STATE(new_state) == CDMA_IDLE ) */
        } /* if ( MAIN_STATE(new_state) == CDMA_IDLE ) */

        return (new_state);

      } /* if ( ( new_state == CDMA_IDLE_INIT ) || !mcc_is_ovhd_msg_okay() ) */

      /* Events report: Redirection */
      event_report(EVENT_REDIRECTION);
    
      redir_info.return_if_fail = cur_bs_ptr->egsd.return_if_fail;
      redir_info.num_recs = mccidl_glob_redir_rec_list.num_recs;
      redir_info.redir_rec_list = mccidl_glob_redir_rec_list.rec_list;

      /* ------------------------------------------
      ** Inform system determination of redirection
      ** ------------------------------------------ */
      if (!mcc_inform_sd_redirection(SD_SS_REDIR_CDMA_GSRDM, &redir_info))
      {
        MSG_MED("SD rejected redirection", 0, 0, 0);
      }
      /* --------------------------------------------------------------------
      ** Inform system determination of done with accept / reject redirection
      ** and determines the next state to be processed based on returned
      ** SS-Action.
      ** -------------------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_opr_acpt_rej(NULL)))
      {
        new_state = CDMA_EXIT;
        mcc_exit.exit = MCC_REDIRECTION;
      }
    } /* if (mccidl_construct_redir_rec_list_from_egsr() > 0) */

    #ifdef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
    else if (mccidl_synch_workaround.egsr_is_synch_workaround) 
    {
      if (mccidl_synch_workaround.egsr_rec_index == 0) 
      {
        new_state = mccidl_proc_synch_workaround(new_state,
                      &cur_bs_ptr->egsd.first);
      }

      #ifdef FEATURE_IS2000_REL_D 
#error code not present
      #endif /* FEATURE_IS2000_REL_D */

      return (new_state);
    }
    #endif /* FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND */

    else
    {
      MSG_ERROR("No applicable redirection records in the EGSRM", 0, 0, 0);
    }
  } /* if (P_REV_IS_6_OR_GREATER && cur_bs_ptr->csp.sp.ext_global_redirect) */

  egsd_proc_pending = FALSE; /* Reset pending flag */
  egsr_ignored = FALSE;      /* Reset ignored flag */

  return (new_state);

} /* mccidl_proc_egsr() */

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION MCCIDL_PROC_ANSI41_SYS_PARM

DESCRIPTION
  This procedure processes a received ANSI-41 System Parameters Message as
  described in IS-2000 Release A section 2.6.2.2.13.  This procedure is used
  for Idle State and Update Overhead Information Substate of System Access
  State.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  New state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_proc_ansi41_sys_parm
(
  word new_state
    /* Current state being processed */
)
{
  static word prev_sid, prev_nid;

  #ifdef FEATURE_GPSONE
#error code not present
  #endif /* FEATURE_GPSONE */

  /*----------------------------------------------------------------------
  ** If the Mobile is not in the process of Access Probe Handoff or Access
  ** Handoff, the Mobile will update its Registration parameters SID, NID
  ** and REG_ZONE with the new base station's values. 
  **---------------------------------------------------------------------*/
  if ((new_state != CDMA_ORIG) && (new_state != CDMA_PAGE_RES)) /*lint !e641 */
  {                                              
    mccreg_set_idle_reg_info(&cur_bs_ptr->csp.sp);
  }

  /* --------------------------------------------------------------------
  ** Processing of DELETE_FOR_TMSI bit as in Release A Section 2.6.2.2.13
  ** The mobile station shall set all bits of TMSI_CODEs-p to '1' if
  ** all of the following are met:
  ** (1) The bits of TMSI_CODEs-p are not all equal to '1',
  ** (2) DELETE_FOR_TMSIs is equal to '1', and
  ** (3) ASSIGNING_TMSI_ZONE_LENs-p is not equal to TMSI_ZONE_LENs, or
  **     the least significant ASSIGNING_TMSI_ZONE_LENs-p octets of
  **     ASSIGNING_TMSI_ZONEs-p are not equal to TMSI_ZONEs.
  ** --------------------------------------------------------------------- */
  if (cur_bs_ptr->csp.esp.delete_for_tmsi &&
     (cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE) &&
     ((cdma.assigning_tmsi.zone_len != cur_bs_ptr->csp.esp.tmsi_zone_len) ||
       (qw_cmp(cur_bs_ptr->csp.esp.tmsi_zone, cdma.assigning_tmsi.info.tmsi_zone) != 0)))
  {
    cdma.assigning_tmsi.info.tmsi_code = INVALID_TMSI_CODE;

    /* Events update: TMSI assigned */
    event_report_payload(EVENT_TMSI_ASSIGNED,
      sizeof(event_tmsi_assigned_type), &cdma.assigning_tmsi.info.tmsi_code);
  }

  /* ---------------------------------------------------------------------
  ** Process the TRUE IMSI fields in the ANSI-41 System Parameters Message
  ** ---------------------------------------------------------------------*/
  mccidl_proc_true_imsi(cur_bs_ptr->csp.esp.imsi_t_supported);

  if (MCRRP_MSG_IS_RECVD)
  {
    /* Process registration parameters */
    mccreg_sysparm(&cur_bs_ptr->csp.sp);

    /* Calculate and store the operational sci */
    (void)mcc_set_operational_sci();  

    if (!mccidl_fast_raho)
    {
      /* Update Searcher's parameters */
      mccidl_send_srch_parms();

      /* Update Searcher's neighbor set */
      mccidl_send_srch_idle_nset();
    }
  }

  /* -------------------------------------------------------------------------
  ** Initialize the setting of BS parameters that shared in both idle and
  ** traffic state, so that reference to those parameters inside access
  ** functions will always return correct values irrespective of state. 
  ** ------------------------------------------------------------------------- */
  cdma.sdb_supported = cur_bs_ptr->csp.esp.sdb_supported;
  cdma.cs_supported = cur_bs_ptr->csp.esp.cs_supported;
  cdma.mob_qos = cur_bs_ptr->csp.esp.mob_qos;

  #ifdef FEATURE_IS2000_REL_C
  if ( P_REV_IS_9_OR_GREATER )
  {
    cdma.use_sync_id = cur_bs_ptr->csp.esp.use_sync_id;
  }
  else
  {
    cdma.use_sync_id = FALSE;
  }
  #endif /* FEATURE_IS2000_REL_C */

  if (ecl_proc_pending && EXT_CHAN_LIST_MSG_IS_RECVD)
  {
    new_state = mccidl_proc_ecl_msg(new_state);
  }

  /* Mark EPZID processing is pending. */
  mccidl_epzid_proc_pending = TRUE;

  /* -------------------------------------------------------------------
  ** If SID/NID changed in Idle State, move state back to CDMA_IDLE_INIT
  ** so that System Determination will be informed of the change.
  ** ------------------------------------------------------------------- */
  /*lint -e727*/
  if ((MAIN_STATE(new_state) == CDMA_IDLE) &&
      ((cur_bs_ptr->csp.sp.sid != prev_sid) ||
       (cur_bs_ptr->csp.sp.nid != prev_nid)))
  /*lint +e727*/
  {
    if ( new_state == CDMA_OVHD ) /*lint !e641 */
    {
      oh_sd_rpt_pending_in_upd_ovhd = TRUE;
    }
    else if ( new_state != CDMA_IDLE_INIT ) /*lint !e641 */
    {
      new_state = CDMA_IDLE_INIT; /*lint !e641 */
      if ( !mccidl_fast_raho )
      {
        /* Start the BS insanity timer */
        mccidl_set_bs_insanity_timer();
      }
    }
  }

  prev_sid = cur_bs_ptr->csp.sp.sid;
  prev_nid = cur_bs_ptr->csp.sp.nid;

  #if defined (FEATURE_IS2000_REL_C_AKA) || defined (FEATURE_IS2000_REL_C_MI)
#error code not present
  #endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_C_MI */

  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #endif /* FEATURE_IS2000_REL_A_AES */

  return new_state;

} /* mccidl_proc_ansi41_sys_parm */

/*===========================================================================

FUNCTION MCCIDL_PROC_MCRR_PARM

DESCRIPTION
  This procedure processes a received MC-RR Parameters Message as
  described in IS-2000 Release A section 2.6.2.2.14.  This procedure is used
  for Idle State and Update Overhead Information Substate of System Access
  State.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_proc_mcrr_parm
(
  word new_state
    /* Current state being processed */
)
{
  byte old_p_rev_in_use = cdma.p_rev_in_use;
    /* Remember old p_rev_in_use.  mcc_update_bs_p_rev below may
       change p_rev_in_use */
  byte fccch_id; /* Hashed FCCCH number */

  #ifdef FEATURE_GPSONE
#error code not present
  #endif /* FEATURE_GPSONE */

  if (cdma.mob_cai_rev < cur_bs_ptr->csp.esp.min_p_rev)
  {
    /* --------------------------------------------------------
    ** If the protocol revision level supported by the mobile
    ** statiom is less than the minimum protocol revision level
    ** supported by the base station, the mobile station shall
    ** enter the System Determination Substate with a protocol
    ** mismatch indication.
    ** -------------------------------------------------------- */
    MSG_HIGH( "Min CAI rev > mob_cai_rev", 0, 0, 0 );

    /* Event report: Protocol mismatch */
    event_report(EVENT_PROTOCOL_MISMATCH);

    /* ------------------------------------------------------------
     ** Inform system determination of Protocol Mismatch exit.
     ** Determine the next state to be processed based on returned
     ** SS-Action.
     ** ------------------------------------------------------------ */
    if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
       sd_ss_ind_cdma_acq_prot_mis(NULL)))
    {
      mcc_exit.exit = MCC_INCOMP_CAI_REV;
      new_state = CDMA_EXIT;
      return(new_state);
    }
  }

 /* -------------------------------------------------------------------
  ** Verify that all BRAT's are valid.  If we are not on a pilot beacon,
  ** also ensure that frame size (determined by fccch_rate) is supported.
  ** -------------------------------------------------------------------- */
  if (!mccidl_are_bcch_data_rates_valid(&cur_bs_ptr->csp.bcast_bcch_info) ||
      ((cur_bs_ptr->csp.fccch_info.num_fccch != 0) &&
       ((mccidl_decode_fccch_data_rate(cur_bs_ptr->csp.fccch_info.fccch_rate) ==
         CAI_FCCCH_DATA_RATE_INVALID) ||
        !mccap_is_csch_f_ccch_frame_size_supported(mccidl_decode_fccch_frame_size
                                                   (cur_bs_ptr->csp.fccch_info.fccch_rate)))))
    {
      MSG_MED("Invalid rate/frame size not supported", 0, 0, 0);
    /* ----------------------------------------------
    ** Inform system determination of new system exit
    ** ---------------------------------------------- */
    if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
        sd_ss_ind_cdma_opr_reg_rej(NULL)))
    {
      new_state     = CDMA_EXIT;
      mcc_exit.exit = MCC_INCOMP_CAI_REV;
      return (new_state);
    }
  }

  /* ------------------------------------------------------------------------
  ** Extended Global Service Redirect Message must be sent on a pilot beacon.
  ** ------------------------------------------------------------------------ */
  if ((cur_bs_ptr->csp.fccch_info.num_fccch == 0) && 
      !cur_bs_ptr->csp.sp.ext_global_redirect)
  {
    MSG_ERROR("MCRRPM: Pilot beacon must send EGSRM", 0, 0, 0);
    if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
            sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_UNKNOWN, NULL ) )
       )
    {
      mcc_exit.exit = MCC_SYSTEM_LOST;
      new_state = CDMA_EXIT;
      return (new_state);
    }
  }

  /* Reset flag */
  rescan_ignored = FALSE; 

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  mcc_update_bs_p_rev(cur_bs_ptr->csp.esp.p_rev);

  if (cdma.p_rev_in_use != old_p_rev_in_use)
  {
    mcc_clear_ovhd_msgs_prev_mismatch();
  }

  #ifdef FEATURE_AUTH
  if ((new_state != CDMA_ORIG) && (new_state != CDMA_PAGE_RES)) /*lint !e641 */
  {
    /*-----------------------------------------------------------------------
    ** If the Mobile is not in the process of Access Probe Handoff or Access
    ** Handoff, update authentication parameters from new base station. 
    ** ---------------------------------------------------------------------- */
    if (cur_bs_ptr->csp.sending_a41_rand)
    {
      cur_bs_ptr->rp.auth = CAI_AUTH;
    }
    else
    {
      cur_bs_ptr->rp.auth = CAI_NO_AUTH;
    }
  }
  #endif /* FEATURE_AUTH */

  if ((cur_bs_ptr->csp.esp.mcc == 0x3FF) &&
      (cur_bs_ptr->csp.esp.imsi_11_12 == 0x7F))
  {
    if (cdma.imsi_type != CDMA_IMSI_M)
    {
      /* Copy IMSI_M into IMSI_O  */
      MSG_MED("Switching to IMSI_M",0,0,0);
      cdma.imsi_s1    = cdma.imsi_m_s1;
      cdma.imsi_s2    = cdma.imsi_m_s2;
      qw_equ( cdma.imsi_s, cdma.imsi_m_s );
      cdma.imsi_11_12 = cdma.imsi_m_11_12;
      cdma.mcc        = cdma.imsi_m_mcc;
      cdma.imsi_addr_num = cdma.imsi_m_addr_num;

      /* Set IMSI type flag to IMSI_M */
      cdma.imsi_type  = CDMA_IMSI_M;

      /* Re-calculate HASH_KEY (see IS-95B section 6.6.7.1) */
      cdma.imsi_hash_key = cdma.imsi_s1 + (((dword) cdma.imsi_s2) << 24);

      // Note: this key has to be remembered for QPCH operation
      cur_bs_ptr->imsi_hash_key = cdma.imsi_hash_key;

      /* Re-calculate page slot in case we don't go through idle_init */
      idle_pgslot = hash(cdma.imsi_hash_key, 2048,
                         (word) (6*(cdma.imsi_hash_key & 0xfff)));

      #ifdef FEATURE_HDR
#error code not present
      #endif /* FEATURE_HDR */

      /* Following two lines is to force the rehashing of the
         forward common control channel. */
      cdma.fccch_id = PRI_FCCCH;
      MSG_MED( "Switching to primary FCCCH", 0, 0, 0);
      cdma.num_fccch_s = 1;

      /* Force to receive Extended CDMA Channel List Msg and
         rehash CDMA Channel if necessary. Rehash of Broadcast
         Control channel will be performed in the following.
         Therefore, above hash key re-calculation need to be
         done before that.                                     */
      cur_bs_ptr->rx.ecl_rx = FALSE;
    }
    /* Default MCC and IMSI_11_12 */
    MSG_MED("MCC and MNC defaulted in esp", 0, 0, 0);
    cur_bs_ptr->csp.esp.mcc = cdma.mcc;
    cur_bs_ptr->csp.esp.imsi_11_12 = cdma.imsi_11_12;
  }

  if (ANSI41SP_MSG_IS_RECVD)
  {
    /* Process registration parameters */
    mccreg_sysparm(&cur_bs_ptr->csp.sp);

    /* Calculate and store the operational sci */
    (void)mcc_set_operational_sci(); 
  }

  if (!cur_bs_ptr->csp.sp.ext_global_redirect)
  {
    /*-----------------------------------------------------------------------
    ** If EXT_GLOBAL_REDIRECT is equal to 0, then the mobile station shall set
    ** EXT_GLOBAL_REDIRECT_MSG_SEQ s to MCRRPM's CONFIG_MSG_SEQ s.
    ** ----------------------------------------------------------------------*/
    cur_bs_ptr->egsd.config_msg_seq = cur_bs_ptr->mcrrpm_config_msg_seq;
    cur_bs_ptr->rx.egsd_rx = TRUE;
  }
  else
  {
    /* Process the Extended Global Service Redirection message if it was 
       deferred */
    if (egsd_proc_pending && EGSR_MSG_IS_RECVD)
    {
      new_state = mccidl_proc_egsr(new_state);
    }
  }

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  /* Process the enhanced broadcast configuration fields of MCRRPM */
  mccbcsms_proc_msg(CAI_MC_RR_PARM_MSG, cur_bs_ptr);
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  cdma.num_fccch_s = cur_bs_ptr->csp.fccch_info.num_fccch;

  /* -------------------------------------------------------------------------
  ** If we have received a MC-RR Parameters Message, but it has a different
  ** number of Forward Common Control Channels specified, then we need to hash
  ** for the new channel and switch to it.
  ** ------------------------------------------------------------------------- */
  if ( cur_bs_ptr->csp.fccch_info.num_fccch == 0 )
  {
    fccch_id = 0;
  }
  else
  {
    fccch_id = (byte) hash(cdma.imsi_hash_key, cdma.num_fccch_s,
                          (word) (2 * (cdma.imsi_hash_key & 0xFFF))) + 1;
  }

  if ( fccch_id != cdma.fccch_id )
  {
    /* ------------------------------------------------------------------
    ** The MCRRPM processing should not hash FCCCH for Fast RAHO, this is
    ** because, the PC is hashed by SRCH task if required, by means of
    ** calling the function mcc_srch_get_nbr_info.
    ** --------------------------------------------------------------- */
    if ( mccidl_fast_raho )
    {
      MSG_MED( "MCRRPM processing not hash FCCCH for a Fast RAHO", 0, 0, 0 );
    }
    else
    {
      MSG_MED( "Hashed to FCCCH %d", fccch_id, 0, 0 );
      cdma.fccch_id = fccch_id;

      /* ----------------------------------------------------------------
      ** We don't need to clear ACC_MSG_SEQs here because MS always makes
      ** sure that it got the latest APM/EAPM before proceed with access
      ** from Idle State or continue access after APHO/AHO.
      ** ---------------------------------------------------------------- */
    }
  }
  else if ( !mccidl_fast_raho )
  {
    MSG_MED( "On FCCCH %d", cdma.fccch_id, 0, 0 );
  }

  /* Reset the QPCH number, since we rec'd a new MCRR parms Msg */
  cur_bs_ptr->qpch_num = QPCH_INVALID_CH;

  /* Store real handoff parameters */
  mcc_ho_real.win_a         = cur_bs_ptr->csp.sp.srch_win_a;
  mcc_ho_real.win_r         = cur_bs_ptr->csp.sp.srch_win_r;
  mcc_ho_real.nghbr_max_age = cur_bs_ptr->csp.sp.nghbr_max_age;
  mcc_ho_real.t_add         = cur_bs_ptr->csp.sp.t_add;
  mcc_ho_real.t_drop        = cur_bs_ptr->csp.sp.t_drop;
  mcc_ho_real.t_comp        = cur_bs_ptr->csp.sp.t_comp;
  mcc_ho_real.t_tdrop       = cur_bs_ptr->csp.sp.t_tdrop;

  /* ---------------------------------------------------------------------
  ** Restore SRCH_WIN_N from stored value. This step is needed for handoff
  ** since the Universal Neighbor List Message is not reprocessed.
  ** --------------------------------------------------------------------- */
  mcc_ho_real.win_n         = cur_bs_ptr->csp.sp.srch_win_n;

  mcc_ho_real.soft_slope = cur_bs_ptr->csp.esp.soft_slope;

  if (cur_bs_ptr->csp.esp.add_intercept & CAI_HO_INTERCEPT_SIGN_BIT)
  {
    /* extend the sign bit */
    mcc_ho_real.add_intercept =
      (int1)(cur_bs_ptr->csp.esp.add_intercept | CAI_HO_INTERCEPT_SIGN_EXT);
  }
  else
  {
    mcc_ho_real.add_intercept = (int1)cur_bs_ptr->csp.esp.add_intercept;
  }

  if (cur_bs_ptr->csp.esp.drop_intercept & CAI_HO_INTERCEPT_SIGN_BIT)
  {
    /* extend the sign bit */
    mcc_ho_real.drop_intercept =
      (int1)(cur_bs_ptr->csp.esp.drop_intercept | CAI_HO_INTERCEPT_SIGN_EXT);
  }
  else
  {
    mcc_ho_real.drop_intercept = (int1)cur_bs_ptr->csp.esp.drop_intercept;
  }

  if (!mccidl_fast_raho)
  {
    /* Update Searcher's handoff parameters */
    mccidl_send_srch_parms();

    /* Update Searcher's neighbor set */
    mccidl_send_srch_idle_nset();
  }

  if (ecl_proc_pending && EXT_CHAN_LIST_MSG_IS_RECVD)
  {
    new_state = mccidl_proc_ecl_msg(new_state);
  }

  if (new_state != CDMA_EXIT)
  {
    /* Mark EPZID processing is pending. */
    mccidl_epzid_proc_pending = TRUE;
  }


  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #endif /* FEATURE_IS2000_REL_A_AES */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif

  return (new_state);

} /* mccidl_proc_mcrr_parm */

#ifdef FEATURE_AUTH
/*===========================================================================

FUNCTION MCCIDL_PROC_ANSI41_RAND

DESCRIPTION
  This procedure processes a received ANSI-41 RAND Message asdescribed in
  IS-2000 Release A section 2.6.2.2.1.16.  This procedure is
  used for Idle State and Update Overhead Information Substate of System
  Access State.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mccidl_proc_ansi41_rand
(
  caii_a41_rand_type *msg_ptr,
    /* Pointer to ANSI-41 RAND Message */
  word new_state
    /* Current state being processed */
)
{
  /*-----------------------------------------------------------------------
  ** If the Mobile is not in the process of Access Probe Handoff or Access
  ** Handoff, the Mobile will update its authentication parameters with the 
  ** new base station's values.
  ** ---------------------------------------------------------------------- */
  if ((new_state != CDMA_ORIG) && (new_state != CDMA_PAGE_RES)) /*lint !e641 */
  {
    cur_bs_ptr->rp.rand = msg_ptr->rand;
  }

} /* end mccidl_proc_ansi41_rand */
#endif /* FEATURE_AUTH */

/*===========================================================================
FUNCTION MCCIDL_PROC_UNIV_NGHBR_LIST_MSG

DESCRIPTION
  This procedure processes Received Universal Neighbor List Messages.

  The processing of the Universal Neighbor List message as described in
  2.6.2.2.17 of the IS2000 REL A signaling standard.

  This procedure is used for Idle State and Update Overhead Information
  Substate of System Access State.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_proc_univ_nghbr_list_msg
(
  caii_univ_nl_type *msg_ptr,
   /* Pointer to Universal Neighbor List message */
  word new_state
)
{
  int i,j;

  /* Initialize NGHBR_REC */
  cur_bs_ptr->nl.pilot_inc         =
  cur_bs_ptr->nl.num_nghbr_pns     =
  cur_bs_ptr->anl.num_analog_nghbr = 0;

  /* Default SRCH_WIN_N */
  mcc_ho_real.win_n             =
  cur_bs_ptr->csp.sp.srch_win_n = CAI_DEFAULT_SRCH_WIN;

  for (i=0; i<msg_ptr->num_radio_interface && i<CAI_MAX_NUM_RADIO_INTERFACE; i++)
  {
    switch (msg_ptr->ri_info[i].radio_interface_type)
    {
      case 0:
      {
        /* Radio interface type is MC */
        caii_ri_type0_rec_type * mc_nbr_ptr;
        mc_nbr_ptr = &msg_ptr->ri_info[i].ri.type0;

        if ((mc_nbr_ptr->pilot_inc == 0) || (mc_nbr_ptr->pilot_inc > 15))
        {
          /* ----------------------------------------------------------
          ** If PILOT_INC field is not within the valid range, MS shall
          ** ignore entire record that contains it.
          ** ---------------------------------------------------------- */
          MSG_ERROR("Ignore Univ nghbr rec %d pilot_inc = %d", i,
            mc_nbr_ptr->pilot_inc, 0);
          break;
        }

        /* These are CDMA neighbors, set their information */
        cur_bs_ptr->nl.pilot_inc = mc_nbr_ptr->pilot_inc;
        cur_bs_ptr->nl.nghbr_srch_mode = mc_nbr_ptr->nghbr_srch_mode;
        cur_bs_ptr->nl.num_nghbr_pns = MIN(SRCH_NSET_MAX,
                                       mc_nbr_ptr->num_nghbr);

        for (j=0; j < mc_nbr_ptr->num_nghbr && j < SRCH_NSET_MAX ; j++)
        {
          cur_bs_ptr->nl.nset[j].pilot_pn = mc_nbr_ptr->nghbr[j].nghbr_pn;
          cur_bs_ptr->nl.nset[j].config = mc_nbr_ptr->nghbr[j].nghbr_config;

          if (cur_bs_ptr->nl.nset[j].config == 0x03)
          {
            /* 011 config, Only the BS shd tell us if this supports BCCH or
               not */
            cur_bs_ptr->nl.nset[j].bcch_supported =
              mc_nbr_ptr->nghbr[j].bcch_support;
          }
          else if (cur_bs_ptr->nl.nset[j].config == 0x01)
          {
            /* The BS directing us to the PCH, so may be this BS does not
               support BCCH */
            cur_bs_ptr->nl.nset[j].bcch_supported = FALSE;
          }
          else
          {
            /* All other neighbor configs imply BCCH support */
            cur_bs_ptr->nl.nset[j].bcch_supported = TRUE;
          }

          /* Set pilot record type and type specific fields */
          cur_bs_ptr->nl.nset[j].add_pilot_rec_incl =
            mc_nbr_ptr->nghbr[j].add_pilot_rec_incl;
          if (cur_bs_ptr->nl.nset[j].add_pilot_rec_incl)
          {
            cur_bs_ptr->nl.nset[j].nghbr_pilot_rec_type =
              (cai_pilot_rec_type) mc_nbr_ptr->nghbr[j].nghbr_pilot_rec_type;
            cur_bs_ptr->nl.nset[j].nghbr_pilot_rec =
              mc_nbr_ptr->nghbr[j].pilot_rec;
          }

          /* Set srch priority */
          if ((mc_nbr_ptr->nghbr_srch_mode == SRCH_PRI_ONLY) ||
              (mc_nbr_ptr->nghbr_srch_mode == SRCH_PRI_AND_WIN))
          {
            cur_bs_ptr->nl.nset[j].search_pri =
              mc_nbr_ptr->nghbr[j].search_priority;
          }
          else
          {
            cur_bs_ptr->nl.nset[j].search_pri = 0x02; /* Set it to high */
          }

          /* Set srch window related fields */
          if ((mc_nbr_ptr->nghbr_srch_mode == SRCH_NO_PRI_OR_WIN) ||
              (mc_nbr_ptr->nghbr_srch_mode == SRCH_PRI_ONLY))
          {
            cur_bs_ptr->nl.nset[j].srch_win_nghbr = mc_nbr_ptr->srch_win_n;
            cur_bs_ptr->nl.nset[j].srch_offset_nghbr = 0;
          }
          else
          {
            cur_bs_ptr->nl.nset[j].srch_win_nghbr =
              mc_nbr_ptr->nghbr[j].srch_win_nghbr;
            if (mc_nbr_ptr->srch_offset_incl)
            {
              cur_bs_ptr->nl.nset[j].srch_offset_nghbr =
                mc_nbr_ptr->nghbr[j].srch_offset_nghbr;
            }
            else
            {
              cur_bs_ptr->nl.nset[j].srch_offset_nghbr = 0;
            }
          }
          /* Set nghbr band and frequency */
          if (mc_nbr_ptr->freq_fields_incl &&
              mc_nbr_ptr->nghbr[j].freq_incl)
          {
            cur_bs_ptr->nl.nset[j].freq_incl = TRUE;
            cur_bs_ptr->nl.nset[j].band = mc_nbr_ptr->nghbr[j].nghbr_band;
            cur_bs_ptr->nl.nset[j].freq = mc_nbr_ptr->nghbr[j].nghbr_freq;
          }
          else
          {
            cur_bs_ptr->nl.nset[j].freq_incl = FALSE;
            cur_bs_ptr->nl.nset[j].band = cdma.band_class;
            cur_bs_ptr->nl.nset[j].freq = cdma.cdmach;

            /* ------------------------------------------------------------
            ** In case of APHO and AHO, ECCLM is not processed. So we won't
            ** be able to know real value of freq in those cases. So the
            ** best guess is to initialize freq to be the same as active 
            ** pilot.
            ** ------------------------------------------------------------ */ 

            if (EXT_CHAN_LIST_MSG_IS_RECVD)
            {
              if ((cur_bs_ptr->nl.nset[j].config == 2) &&
                  (cur_bs_ptr->ecl.cdma_freq[0] != cdma.cdmach))
              {
                cur_bs_ptr->nl.nset[j].freq_incl = TRUE;
                cur_bs_ptr->nl.nset[j].freq = cur_bs_ptr->ecl.cdma_freq[0];
              }
            }
          }

          /* Set Timing related fields */
          if (mc_nbr_ptr->use_timing && mc_nbr_ptr->nghbr[j].timing_incl)
          {
            cur_bs_ptr->nl.nset[j].timing_incl =
              mc_nbr_ptr->nghbr[j].timing_incl;
            cur_bs_ptr->nl.nset[j].nghbr_tx_offset =
              mc_nbr_ptr->nghbr[j].nghbr_tx_offset;
            if (mc_nbr_ptr->global_timing_incl)
            {
              cur_bs_ptr->nl.nset[j].nghbr_tx_duration =
                mc_nbr_ptr->global_tx_duration;
              cur_bs_ptr->nl.nset[j].nghbr_tx_period =
                mc_nbr_ptr->global_tx_period;
            }
            else
            {
              cur_bs_ptr->nl.nset[j].nghbr_tx_duration =
                mc_nbr_ptr->nghbr[j].nghbr_tx_duration;
              cur_bs_ptr->nl.nset[j].nghbr_tx_period =
                mc_nbr_ptr->nghbr[j].nghbr_tx_period;
            }
          }
          else
          {
            cur_bs_ptr->nl.nset[j].timing_incl = FALSE;
          }

          /* Set access entry handoff values */
          if (!mc_nbr_ptr->nghbr_set_entry_info ||
              !mc_nbr_ptr->nghbr[j].access_entry_ho)
          {
            cur_bs_ptr->nl.nset[j].access_entry_ho = FALSE;
          }
          else
          {
            cur_bs_ptr->nl.nset[j].access_entry_ho = TRUE;
          }

          /* Set access handoff and access probe handoff values */
          if (!mc_nbr_ptr->nghbr_set_access_info ||
              !mc_nbr_ptr->nghbr[j].access_ho_allowed)
          {
            cur_bs_ptr->nl.nset[j].access_ho_allowed = FALSE;
          }
          else
          {
            cur_bs_ptr->nl.nset[j].access_ho_allowed = TRUE;
          }
        } /* for loop for num_nghbrs */

        /* --------------------------------------------------------------
        ** Store SRCH_WIN_N in mcc_ho_real and cur_bs_ptr->csp.sp to make
        ** it compatible with existing code.
        ** -------------------------------------------------------------- */
        if ((mc_nbr_ptr->nghbr_srch_mode == SRCH_NO_PRI_OR_WIN) ||
            (mc_nbr_ptr->nghbr_srch_mode == SRCH_PRI_ONLY))
        {
          mcc_ho_real.win_n             =
          cur_bs_ptr->csp.sp.srch_win_n = mc_nbr_ptr->srch_win_n;
        }

        /* Update Neighbor Set access entry handoff and access
           handoff indicators */
        cur_bs_ptr->csp.esp.nghbr_set_entry_info =
          mc_nbr_ptr->nghbr_set_entry_info;
        cur_bs_ptr->csp.esp.nghbr_set_access_info =
          mc_nbr_ptr->nghbr_set_access_info;
        break;
      }

      case 1:
      {
        /* Radio interface type is MC */
        caii_ri_type1_rec_type * ana_nbr_ptr;
        ana_nbr_ptr = &msg_ptr->ri_info[i].ri.type1;
        cur_bs_ptr->anl.num_analog_nghbr = ana_nbr_ptr->num_analog_nghbr;
        for (j=0; j < cur_bs_ptr->anl.num_analog_nghbr; j++)
        {
          cur_bs_ptr->anl.anl_nghbr[j].band_class =
            ana_nbr_ptr->nghbr[j].band_class;
          cur_bs_ptr->anl.anl_nghbr[j].sys_a_b = ana_nbr_ptr->nghbr[j].sys_a_b;
        }
        break;
      }

      default:
        MSG_ERROR("Unknown Radio Interface type %d in UNLM",
          msg_ptr->ri_info[i].radio_interface_type, 0, 0);

    } /* switch radio interface type */
  } /*for loop for num of radio interface records*/

  if (!mccidl_fast_raho)
  {
    /* Update Searcher's handoff parameters */
    mccidl_send_srch_parms();

    /* Update Searcher's neighbor set */
    mccidl_send_srch_idle_nset();
  }

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */
  
  return new_state;

} /* mccidl_proc_univ_nghbr_list_msg */

/*===========================================================================

FUNCTION MCCIDL_PROC_EN_ACC_PARM

DESCRIPTION
  This procedure processes a received Enhanced Access Parameters Message as
  described in Release A section 2.6.2.2.15.This procedure is used for Idle
  State and Update Overhead Information Substate of System Access State.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_proc_en_acc_parm
(
 caii_en_ac_parm_type * msg_ptr
 /* Pointer to over-the-air EAPM */
)
{
  int i, j;
  byte mode_mask;

  /* Use the over-the-air msg as temporary storage as we must rearrange the
     mode_parm array.  Must initialize to zero. */
  for (i=0; i < CAI_MAX_NUM_MODE_PARM_REC; i++)
  {
    memset(&msg_ptr->mode_parms[i], 0, sizeof(caii_mode_parm_rec_type));
  }

  /* Translate the array of mode parameter records so that it can be indexed
     by the access mode. */
  for (i=0; i < cur_bs_ptr->eapm.num_mode_parm_rec+1; i++)
  {
    for (j=0; j < CAI_MAX_NUM_MODE_PARM_REC; j++)
    {
      mode_mask = 1 << (CAI_MAX_NUM_MODE_PARM_REC - j - 1); /*lint !e834 !e734 */

      /* If the (j+1)st bit of applicable_modes is turned on, the record's
             parameters are valid for this access mode. */
      if (cur_bs_ptr->eapm.mode_parms[i].applicable_modes & mode_mask)
      {
        msg_ptr->mode_parms[j] = cur_bs_ptr->eapm.mode_parms[i];
      }
    }
  }

  for (i=0; i < CAI_MAX_NUM_MODE_PARM_REC; i++)
  {
    cur_bs_ptr->eapm.mode_parms[i] = msg_ptr->mode_parms[i];
  }

  #if (defined(FEATURE_HIGH_PSIST) && defined (FEATURE_PASSPORT_MODE))
  if(mcc_passport_mode_is_jcdma())
  {
    mccidl_update_psist_info(CAI_EACH, &cur_bs_ptr->eapm);
  }
  #endif /* FEATURE_HIGH_PSIST && FEATURE_PASSPORT_MODE */

} /* mccidl_proc_en_acc_parm */
#endif /* FEATURE_IS2000_REL_A_CC */

#ifdef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
/*===========================================================================

FUNCTION MCCIDL_IS_EGSR_SYNCH_WKAD

DESCRIPTION
  This function checks whether the extended global service redirection message
  is used for Synch Channel workaround.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  True  if rxed EGSRM is for Sync Channel workaround.
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccidl_is_egsr_synch_workaround
(
  const caii_ext_glb_redir_info_type * redir_rec_ptr
)
{
  return ((OVHD_CHAN_IS_PCH) && (cdma.mob_cai_rev >= P_REV_IS2000_REL_A) &&
          redir_rec_ptr->redirect_p_rev_incl &&
          !redir_rec_ptr->excl_p_rev_ind &&
          ((redir_rec_ptr->redirect_p_max == CAI_SC_WKAD_P_REV_254) ||
           (redir_rec_ptr->redirect_p_max == CAI_SC_WKAD_P_REV_255)) &&
          (redir_rec_ptr->record_type == CAI_CDMA_REDIRECTION_REC) &&
          (redir_rec_ptr->redir.rec2.num_chans > 0));

} /* mccidl_is_egsr_synch_workaround */

/*===========================================================================

FUNCTION MCCIDL_PROC_SYNCH_WORKAROUND

DESCRIPTION
  This function performs Synch Channel workaround processing as specified in
  CDG 78.

DEPENDENCIES
  mcc_put_msg() must already have been called to place this message into
  the bs_info array.

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_proc_synch_workaround
(
  word new_state,
    /* Current state being processed */
  const caii_ext_glb_redir_info_type * redir_rec_ptr
    /* Current redirection record being processed */
)
{
  bs_info_id_type bs_info_id;    /* Fields unqiuely identify BS */

  /* Check whether specified band class is supported */
  if (!mcc_is_band_class_supported(redir_rec_ptr->redir.rec2.band_class))
  {
    MSG_MED("Ignore Sync Ch wkad, unsupported band %d",
            redir_rec_ptr->redir.rec2.band_class, 0, 0);
    return new_state;
  }

  MSG_MED("Proc Sync Channel workaround", 0, 0, 0);

  /* Initialize BCCH channel info */
  cdma.bcch        =
  cdma.bcch_non_td =
  cdma.bcch_td     = CAI_SC_WKAD_BCCH_CODE_CHAN;
  if (redir_rec_ptr->redirect_p_max == CAI_SC_WKAD_P_REV_254)
  {
    cdma.brat =
    cdma.brat_non_td =
    cdma.brat_td = CAI_BCCH_9600_RATE;
  }
  else
  {
    cdma.brat =
    cdma.brat_non_td =
    cdma.brat_td = CAI_BCCH_4800_RATE;
  }
  cdma.bcch_code_rate =
  cdma.bcch_code_rate_non_td =
  cdma.bcch_code_rate_td = CAI_CODE_RATE_HALF;

  if ((cdma.band_class != redir_rec_ptr->redir.rec2.band_class) ||
      (cdma.cdmach != redir_rec_ptr->redir.rec2.cdma_chan[0]))
  {
    if (cdma.band_class != redir_rec_ptr->redir.rec2.band_class)
    {
      cdma.band_class = redir_rec_ptr->redir.rec2.band_class;

    }

    if (cdma.cdmach != redir_rec_ptr->redir.rec2.cdma_chan[0])
    {
      cdma.cdmach = redir_rec_ptr->redir.rec2.cdma_chan[0];
    }

    cdma.block_or_sys = (uint16) sdprl_map_chan_to_blksys(SD_MODE_CDMA, 
                                                          (sd_band_e_type) cdma.band_class, cdma.cdmach); 
      // func returns 32bit value, but values are low enough that 16 bits is OK
  }

  /* --------------------------------------
  ** Since we have changed channels we need
  ** a new entry in the bs_info array.
  ** -------------------------------------- */
  bs_info_id.ovhd_chan = CAIX_BCCH;
  bs_info_id.pilot_pn = cur_bs_ptr->pilot_pn;
  bs_info_id.cdma_ch = cdma.cdmach;
  bs_info_id.band_class = cdma.band_class;
  bs_info_id.ovhd_walsh_ch = cdma.bcch;
  mcc_entry_init(&bs_info_id, FALSE);

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  /* Notify RXC to skip frames */
  mcc_idle_ho_rx();
    
  /* Update Searcher's active set */
  mccidl_send_srch_idle_aset();

  /* Reprocess stored overheads if any. */
  new_state = mccidl_process_stored_ovhds(new_state);

  /* Update Searcher's neighbor set */
  mccidl_send_srch_idle_nset();

  /* -------------------------------------------------------------------
  ** MS perform channel monitoring supervision to see whether it needs
  ** to switch between PCH/BCCH/FCCCH.
  ** ------------------------------------------------------------------- */
  mccidl_supervise_channel_monitoring(new_state, NULL, FALSE);

  return new_state;

} /* mccidl_proc_synch_workaround */
#endif /* FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND */

/*===========================================================================

FUNCTION MCCIDL_IS_ASSIGNED_PAGE_SLOT

DESCRIPTION
  This function checks if the subframe whcih message received is an assigned
  page slot.

DEPENDENCIES
  The msg_ptr parameter points to a received page message.
  The page_type parameter is set to the correct type of page.
  The page_info field has been filled in based on the page_type.

RETURN VALUE
  TRUE, if it is an assigned slot.
  FALSE, Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccidl_is_assigned_page_slot
(
  byte  msg_type, /* Message type */
  qword frame     /* Subframe message received */
)
{
  word slot_pos = 0;    /* Position relative to the desired slot */
  word slot = 0;        /* Desired slot */

  if (msg_type != CAI_PAGE_MSG)
  {
    /* -----------------------------------------------------
    ** For slotted pages we must first calculate which slot
    ** we are in, and which slot we are to receive pages in.
    ** See IS-95A sections 6.6.2.1.1.1 and 6.6.2.1.1.3.1.
    ** ----------------------------------------------------- */
    /* This is the slot number relative to our slot cycle */
    slot_pos = ((word)(qw_lo(frame)) >> 2) & sci_mask;

    /* This is the slot we are to monitor, relative to our slot cycle */
    slot = (idle_pgslot & sci_mask);
  }

  return ((slot_pos == slot) || (slot_pos == ((slot + 1) & sci_mask)));

} /* mccidl_is_assigned_page_slot */

/*===========================================================================

FUNCTION MCCIDL_IS_SPM_DONE_MONITORING_SLOT

DESCRIPTION
  Given a mobile that is operating in slotted mode, the mobile monitors
  the assigned paging channel slot until one of a series of conditions is
  true for the received Slotted Page Message. This function provides the
  check on those conditions.

DEPENDENCIES
  The msg_ptr parameter points to a received page message.
  The page_type parameter is set to the correct type of page.
  The page_info field has been filled in based on the page_type.

RETURN VALUE
  TRUE,  If there is nothing more for the mobile to monitor in this slot
  FALSE, Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccidl_is_spm_done_monitoring_slot
(
  mccrxtx_msg_type *msg_ptr /* Pointer to received message */
)
{
  /* IS-95A Case 1 --- The Slotted Page Msg is not part of J-STD-008 or
  IS-95B, but we'll keep this check here in case the BS tries to treat the
  mobile as an IS-95A phone. */
  if (msg_ptr->msg.slt_page.more_pages == 0)
  {
    /* Update sleep slot mask */
    sleep_slot_mask |= PAGE_SLOT | BC_PAGE_SLOT;
    return (TRUE);
  }

  /* None of the checks passed, we are not finished monitoring, return FALSE */
  sleep_slot_mask &= ~PAGE_SLOT;  /* assigned paging is not done */
  return (FALSE);

} /* mccidl_is_spm_done_monitoring_slot */

/*===========================================================================

FUNCTION MCCIDL_IS_GPM_DONE_MONITORING_SLOT

DESCRIPTION
  Given a mobile that is operating in slotted mode, the mobile monitors
  the assigned paging channel slot until one of a series of conditions is
  true for the received General Page Message. This function provides the
  check on those conditions.

DEPENDENCIES
  The msg_ptr parameter points to a received page message.
  The page_type parameter is set to the correct type of page.
  The page_info field has been filled in based on the page_type.

RETURN VALUE
  TRUE,  If there is nothing more for the mobile to monitor in this slot
  FALSE, Otherwise

SIDE EFFECTS
  sleep_slot_mask and next_slot updated.

===========================================================================*/
boolean mccidl_is_gpm_done_monitoring_slot
(
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  mcc_page_info_type *page_info
    /* Information from a page which was addressed to the mobile */
)
{

  #ifdef FEATURE_IS2000_REL_A_CC
  /* Clear monitor next slot flag */
  mccidl_monitor_next_slot = FALSE;
  #endif /* FEATURE_IS2000_REL_A_CC */

  #ifdef FEATURE_BROADCAST_SMS
  /*----------------------------------------------------------------------
                      bc_page_slot                  !bc_page_slot
          --------------------------------  ------------------------------
            normal_slot     !normal_slot      normal_slot    !normal_slot
          ---------------  ---------------  --------------  --------------
          bcdone  !bcdone  bcdone  !bcdone  bcdone !bcdone  bcdone !bcdone

  bc_data   F        F       F        F       F       F       F       F

  !bc_data  P1       F       T        F       P1      P2      F       F

  P1: Return value depends on the other Done bits in GPM.
  P2: if bcsms_enabled, return FALSE,
      Otherwise, P1.
  ----------------------------------------------------------------------*/
  if (msg_ptr->msg.gen_page.broadcast_done) //lint !e613
  {
    /* Update sleep slot mask for broadcast done */
    sleep_slot_mask |= BC_PAGE_SLOT;
  }

  if (mccbcsms_is_bc_data_slot(msg_ptr->frm.frame))
  {
    /* ----------------------------------------------------------------
    ** we are waiting for a data burst msg in the current slot, and the
    ** Data Burst hasn't arrived yet, so we can't go back to sleep.
    ** ---------------------------------------------------------------- */
    MSG_LOW("GPM arrived, but still waiting for Data Burst",0,0,0);
    return (FALSE);
  }

  if (mccbcsms_is_bc_page_slot(msg_ptr->frm.frame))
  {
    /* Broadcast page slot */

    if (!mccidl_is_assigned_page_slot(msg_ptr->msg.gen.msg_type,
        msg_ptr->frm.frame))
    {
      /* Not assigned page slot */

      if (msg_ptr->msg.gen_page.broadcast_done)
      {
        /* Broadcast done, done with slot monitoring */
        return (TRUE);
      }
      else
      {
        /* Broadcast not done, continue with slot monitoring */
        return (FALSE);
      }
    }
    else
    {
      /* Assigned page slot */

      if (!msg_ptr->msg.gen_page.broadcast_done) //lint !e613
      {
        /* Broadcast not done, continue with slot monitoring */
        return (FALSE);
      }
    }
  }
  else
  {
    /* It's an assigned only slot! According to IS-2000 standard  */
    /* 2.6.2.1.1.3.1: -                                           */
    /* If the mobile station is configured to receive broadcast   */
    /* addresses and the mobile station is monitoring a Paging    */
    /* Channel, the mobile station shall continue to monitor the  */
    /* Paging Channel or the Forward Common Control Channel until */
    /* one of the preceding conditions is satisfied and should    */
    /* monitor the Paging Channel or the Forward Common Control   */
    /* Channel until it has received a General Page Message with  */
    /* BROADCAST_DONE equal to '1' or until Layer 3 receives a    */
    /* broadcast address mismatch indication from Layer 2.        */

    /* It is understood from the standard that broadcast_done bit */
    /* also applies to the messages with broadcast address type   */
    /* other than the General Page Message. Therefore, even if    */
    /* the bcast_index = 0 on PCH, where no broadcast page will be*/
    /* sent, the mobile still needs to wait for other broadcast   */
    /* messages if broadcast_done = 0.                            */

    if (mccbcsms_is_enabled() &&
        (!msg_ptr->msg.gen_page.broadcast_done)    //lint !e613
       )
    {
      /* Broadcast not done, continue with slot monitoring */
      return (FALSE);
    }
  }

  /* ----------------------------------------------------------------------
  ** If broadcast paging is supported, MS should monitor the paging channel
  ** or forward common control channel until it has received BROADCAST_DONE
  ** = 1, and until one of the following conditions is satisfied, IS95B
  ** 6.6.2.1.1.3.1.
  ** ---------------------------------------------------------------------- */

  /* ----------------------------------------------------------------------
  ** We are here because either it's not a broadcast slot, or an overlapped
  ** broadcast page and assigned page slot with BC_DONE bit set.
  ** ---------------------------------------------------------------------- */
  #endif /* FEATURE_BROADCAST_SMS */

  sleep_slot_mask |= PAGE_SLOT;
  /* assuming assigned paging slot is done for now*/

  /* Here we check the conditions specified in J-STD-008 */

  /* Check the Class 0 cases first */
  if ((cdma.imsi_addr_num == IMSI_CLASS_0_ADDR_NUM) &&
      (msg_ptr->msg.gen_page.class_0_done)) //lint !e613
  {

    /* --------------------------------------------------------------------
    ** JSTD008 Case 1 --- MS has a class 0 IMSI assigned, all the bits of
    ** TMSI_CODE are equal to 1 and the MS receives a General Page Msg with
    ** CLASS_0_DONE set to 1.
    ** -------------------------------------------------------------------- */
    if (cdma.assigning_tmsi.info.tmsi_code == INVALID_TMSI_CODE)
    {
      return (TRUE);
    }
    /* --------------------------------------------------------------------
    ** JSTD008 Case 3 --- MS has a class 0 IMSI assigned, the bits of
    ** TMSI_CODE are not all equal to 1 and the MS receives a General Page
    ** Msg with CLASS_0_DONE set to 1 and TMSI_DONE set to 1.
    ** -------------------------------------------------------------------- */
    if ((cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE) &&
        (msg_ptr->msg.gen_page.tmsi_done))  //lint !e613
    {
      return (TRUE);
    }

    /* --------------------------------------------------------------------
    ** JSTD008 Case 5 --- MS has a class 0 IMSI assigned, the bits of
    ** TMSI_CODE are not all equal to 1 and the MS receives a General Page
    ** Msg with CLASS_0_DONE set to 1, ORDERED_TMSIS set to 1 and a record
    ** with TMSI_CODE value greater than the stored TMSI_CODE.
    ** -------------------------------------------------------------------- */
    if ((cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE) &&
        (msg_ptr->msg.gen_page.ordered_tmsis) &&
        (cdma.assigning_tmsi.info.tmsi_code < page_info->tmsi_code))  //lint !e613
    {
      return (TRUE);
    }

  } /* end Class 0 cases */

  /* Check the Class 1 cases. */
  if ((CLASS_1_IMSI(cdma.imsi_addr_num)) &&
      (msg_ptr->msg.gen_page.class_1_done)) //lint !e613
  {

    /* --------------------------------------------------------------------
    ** JSTD008 Case 2 --- MS has a class 1 IMSI assigned, all the bits of
    ** TMSI_CODE are equal to 1 and the MS receives a General Page Msg with
    ** CLASS_1_DONE set to 1.
    ** -------------------------------------------------------------------- */
    if (cdma.assigning_tmsi.info.tmsi_code == INVALID_TMSI_CODE)
    {
      return (TRUE);
    }

    /* --------------------------------------------------------------------
    ** JSTD008 Case 4 --- MS has a class 1 IMSI assigned, the bits of
    ** TMSI_CODE are not all equal to 1 and the MS receives a General Page
    ** Msg with CLASS_1_DONE set to 1 and TMSI_DONE set to 1.
    ** -------------------------------------------------------------------- */
    if ((cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE) &&
        (msg_ptr->msg.gen_page.tmsi_done))  //lint !e613
    {
      return (TRUE);
    }

    /* --------------------------------------------------------------------
    ** JSTD008 Case 6 --- MS has a class 1 IMSI assigned, the bits of
    ** TMSI_CODE are not all equal to 1 and the MS receives a General Page
    ** Msg with CLASS_1_DONE set to 1, ORDERED_TMSIS set to 1 and a record
    ** with TMSI_CODE value greater than the stored TMSI_CODE.
    ** -------------------------------------------------------------------- */
    if ((cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE) &&
        (msg_ptr->msg.gen_page.ordered_tmsis) &&
        (cdma.assigning_tmsi.info.tmsi_code < page_info->tmsi_code))  //lint !e613
    {
      return (TRUE);
    }

  } /* end Class 1 cases */


  /* ----------------------------------------------------------------------
  ** J-STD-008 Case 7 (same as IS-95A Case 6) is handled in rxc.c and is
  ** reported to the MC task using the RXC_SLOT_R report.
  ** ---------------------------------------------------------------------- */

  /* None of the checks passed, we are not finished monitoring, return FALSE */
  sleep_slot_mask &= ~PAGE_SLOT;  /* assigned paging is not done */

  return (FALSE);

} /* mccidl_is_gpm_done_monitoring_slot */

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION MCCIDL_IS_UPM_DONE_MONITORING_SLOT

DESCRIPTION
  Given a mobile that is operating in slotted mode, the mobile monitors
  the assigned forward common control channel slot until one of a series of
  conditions is true for the received Universal Page Message. This function
  provides the check on those conditions.

DEPENDENCIES
  The msg_ptr parameter points to a received page message.
  The page_type parameter is set to the correct type of page.
  The page_info field has been filled in based on the page_type.

RETURN VALUE
  TRUE,  If there is nothing more for the mobile to monitor in this slot
  FALSE, Otherwise

SIDE EFFECTS
  sleep_slot_mask and mccidl_monitor_next_slot updated.

===========================================================================*/
boolean mccidl_is_upm_done_monitoring_slot
(
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  mccdma_page_match_result_type page_match
    /* Page match result */
)
{
  qword frame;
    /* Subframe message received */
  boolean read_next_slot;
    /* Flag indicates if next page slot should be monitored */

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  boolean read_next_slot_bcast;
    /* Flag indicates if next broadcast slot should be monitored */

  if (mccbcsms_is_bc_data_slot(msg_ptr->frm.frame))
  {
    /* ----------------------------------------------------------------
    ** We are waiting for a data burst msg in the current slot, and the
    ** Data Burst hasn't arrived yet, so we can't go back to sleep.
    ** ---------------------------------------------------------------- */
    MSG_MED("UPM arrived, but still waiting for Data Burst",0,0,0);
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  {
    if (page_match == NO_MATCH)
    {
      /* Done with slot monitoring */

      MSG_LOW("Done with slot", 0, 0, 0);

      #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      /* Update sleep slot mask for broadcast done */
      sleep_slot_mask |= BC_PAGE_SLOT;
      #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

      /* Update sleep slot mask for page done */
      sleep_slot_mask |= PAGE_SLOT;

      /* Update next slot monitoring requirement */
      mccidl_monitor_next_slot = FALSE;
      if (msg_ptr->msg.gen.msg_type == CAI_UNIVERSAL_PAGE_MSG)
      {
        qw_equ(frame, msg_ptr->frm.frame);
        read_next_slot = msg_ptr->msg.upm.common.read_next_slot;

        #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
        read_next_slot_bcast = msg_ptr->msg.upm.common.read_next_slot_bcast;
        #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

      }
      else
      {
        qw_equ(frame, mccdma_seg_univ_page.frame);
        read_next_slot = mccdma_seg_univ_page.common.read_next_slot;

        #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
        read_next_slot_bcast = mccdma_seg_univ_page.common.read_next_slot_bcast;
        #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

        /* -------------------------------------------------------------------
        ** Per Release A LAC requirement,  the Base Station shall not transmit
        ** PDUs derived from the same Universal Page Block in different FCCCH
        ** slots. So, we don't need to worry about the validility of
        ** READ_NEXT_SLOT and READ_NEXT_SLOT_BCAST in segmented Universal Page
        ** Message.
        ** ------------------------------------------------------------------- */
      }

      if (mccidl_is_assigned_page_slot(msg_ptr->msg.gen.msg_type, frame))
      {
        /* Assigned page slot, update monitor next slot flag based on
           READ_NEXT_SLOT */
        mccidl_monitor_next_slot = read_next_slot;

        #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
        if (!read_next_slot && mccbcsms_is_enabled() && read_next_slot_bcast)
        {
          /* If READ_NEXT_SLOT is false and broadcast SMS is enabled, update
             monitor next slot flag based on READ_NEXT_SLOT_BCAST */
          mccidl_monitor_next_slot = read_next_slot_bcast;
        }
        #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

      }

      #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      else if (mccbcsms_is_enabled() && mccbcsms_is_bc_page_slot(frame))
      {
        /* Broadcast page slot and broadcast SMS enabled, update monitor next
           slot flag based on READ_NEXT_SLOT_BCAST */
        mccidl_monitor_next_slot = read_next_slot_bcast;
      }
      #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

      return (TRUE);
    }
  }

  /* Continue with slot monitoring */
  return (FALSE);

} /* mccidl_is_upm_done_monitoring */
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION MCCIDL_DONE_MONITORING_SLOT

DESCRIPTION
  Given a mobile that is operating in slotted mode, the mobile monitors
  the assigned paging channel slot until one of a series of conditions is
  true.  This function provides the check on those conditions.

DEPENDENCIES
  The msg_ptr parameter points to a received page message.
  The page_type parameter is set to the correct type of page.
  The page_info field has been filled in based on the page_type.

RETURN VALUE
  TRUE,  If there is nothing more for the mobile to monitor in this slot
  FALSE, Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -esym(715, page_match) */
boolean mccidl_done_monitoring_slot
(
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  mcc_page_info_type *page_info,
    /* Information from a page which was addressed to the mobile */
  mccdma_page_match_result_type page_match //lint !e715
    /* Page match result */
)
{
  boolean done_monitoring = FALSE;

  #ifdef FEATURE_BROADCAST_SMS
  if (bcast_pending)
  {
    /* This is a saved (enhanced) broadcast page message.   */
    /* Don't go based on info from this page message.       */
    return (FALSE);
  }

  if (!mccidl_is_assigned_page_slot(msg_ptr->msg.gen.msg_type,
      msg_ptr->frm.frame) && !mccbcsms_is_bc_page_slot(msg_ptr->frm.frame))
  {
    if (!mccbcsms_is_bc_data_slot(msg_ptr->frm.frame))
    {
      /* This page is received in none of slots, don't sleep */
      return (done_monitoring);
    }
  }
  #endif /* FEATURE_BROADCAST_SMS */

  switch (msg_ptr->msg.gen.msg_type)
  {
    case CAI_SLT_PAGE_MSG:
      done_monitoring = mccidl_is_spm_done_monitoring_slot(msg_ptr);
      break;

    case CAI_GENERAL_PAGE_MSG:
      done_monitoring = mccidl_is_gpm_done_monitoring_slot(msg_ptr, page_info);
      break;

    #ifdef FEATURE_IS2000_REL_A_CC
    case CAI_UPM_FIRST_SEG_MSG:
    case CAI_UPM_MIDDLE_SEG_MSG:
    case CAI_UPM_FINAL_SEG_MSG:
    case CAI_UNIVERSAL_PAGE_MSG:
      done_monitoring = mccidl_is_upm_done_monitoring_slot(msg_ptr, page_match);
      break;
    #endif /* FEATURE_IS2000_REL_A_CC */

    default:
      MSG_ERROR("Invalid page message %d", msg_ptr->msg.gen.msg_type, 0, 0);
      break;
  }

  return (done_monitoring);

} /* mccidl_done_monitoring_slot() */
/*lint +esym(715,page_match */

/*===========================================================================

FUNCTION MCCIDL_INIT_SLEEP

DESCRIPTION
  This function performs processing for sleep request.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
word mccidl_init_sleep
(
  qword msg_frame,
    /* Frame that message received */
  word curr_state
    /* Current state to be processed */
)
{
  word next_state = curr_state; /* Next state to be processed */

  if (mccidl_assert_sleep_ok(msg_frame))
  {
    MSG_LOW("Sleep ok", 0, 0, 0);

    if ( BIO_GET_SW(BIO_SW16_M) )
    {
      /* Acq test */
      /* -------------------------------------------------------------
      ** Inform system determination of system lost and determines the
      ** next state to be processed based on the returned SS-Action.
      ** ------------------------------------------------------------- */
      if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_UNKNOWN, NULL ) )
         )
      {
         mcc_exit.exit = MCC_SYSTEM_LOST;
         next_state    = CDMA_EXIT;
      }
    }
  }
  else
  {
    MSG_MED("Page msg: sleep ok not asserted", 0, 0, 0);
  }

  return (next_state);

} /* mccidl_init_sleep */

/*===========================================================================

FUNCTION MCCIDL_PAGE_MATCH

DESCRIPTION
  This function performs the Mobile Station Page Match Operation of IS-95A
  and ANSI J-STD-008.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_page_match
(
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  word new_state
    /* Next state to be processed */
)
{
  word slot_pos = 0;
    /* position relative to the desired slot */
  word slot = 0;
    /* desired slot */
  mcc_page_info_type page_info;
    /* Information from a page which was addressed to the mobile */
  mccdma_page_match_result_type page_match;
    /* Page match result */
  #ifdef T_W1
#error code not present
  #endif /* T_W1 */

  word real_slot, real_slot_pos, real_sci_mask;
  boolean empty_page = FALSE;
  srch_access_type_enum access_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr->msg.gen.msg_type != CAI_PAGE_MSG)
  {
    /* -----------------------------------------------------
    ** For slotted pages we must first calculate which slot
    ** we are in, and which slot we are to receive pages in.
    ** See IS-95A sections 6.6.2.1.1.1 and 6.6.2.1.1.3.1.
    ** ----------------------------------------------------- */

    /* This is the slot number relative to our slot cycle */
    slot_pos = ((word)(qw_lo( msg_ptr->frm.frame )) >> 2 ) & sci_mask;

    /* This is the slot we are to monitor, relative to our slot cycle */
    slot = (idle_pgslot & sci_mask);
  }

  /* ---------------------------------------------------------------
  ** mcc_parse_page returns NO_MATCH if none of the pages are addressed
  ** to the mobile, or if the page is a duplicate. Otherwise it
  ** returns information from the page in page_info.
  ** --------------------------------------------------------------- */
  page_match = mcc_parse_page(new_state, FALSE, msg_ptr, &page_info);

  if (page_match != NO_MATCH)
  {
    /* ------------------------------------
    ** Increment the count PAG_3 or FCCCH_3
    ** ------------------------------------ */
    mcc_increment_pag_3_or_fccch_3();

    /* Log the matched page - type of page_match corresponds to
    ** mclog_match_type for now 
    */
    mclog_report_matched_msg(msg_ptr->msg.gen.msg_type, 
                             (mclog_match_type) page_match );
  }

  #ifdef FEATURE_IS2000_REL_A_CC
  else if (msg_ptr->msg.gen.msg_type == CAI_UPM_FINAL_SEG_MSG)
  {
    mclog_report_event_seg_upm_addr_mismatch(MCLOG_FULL_ADDR_MISMATCH, /*lint !e641 */
      mccdma_seg_univ_page.segment_seq);
  }
  #endif /* FEATURE_IS2000_REL_A_CC */

  switch (page_match)
  {

    #ifdef FEATURE_BROADCAST_SMS
    case BROADCAST_MATCH:

      /* Broadcast Paging doesn't need layer2 ack. */
      mcc_layer2.valid_ack = FALSE;

      /* Since page response is not needed, AEHO doesn't apply here */

      if (mccidl_done_monitoring_slot(msg_ptr, &page_info, page_match))
      {
        /* -----------------------------------------------------------------
        ** We just got a slotted page message in the designated or next slot
        ** which informed us that no more pages are coming for us, so if or
        ** when we have all the overhead messages we can go to sleep.
        ** The sleep function will schedule broadcast msg wake-ups
        ** ---------------------------------------------------------------- */
        new_state = mccidl_init_sleep(msg_ptr->frm.frame, new_state);
      }

      /* Update the Search with new BC schedules */
      mccbcsms_schedule_bcast();
      break;
      #endif /* FEATURE_BROADCAST_SMS */

    case PAGE_MATCH:

      #ifdef FEATURE_BROADCAST_SMS
      /* if a page match is found, all pending broadcast msgs are reset to
       * prevent complications when page response is finished. Broadcasts must
       * restart after that. */
      mccbcsms_reset_bc_pages();

      /* Clear pending broadcast page */
      bcast_pending = FALSE;

      /* Cancel the broadcast schedule */
      mccbcsms_cancel_bcast();
      #endif /* FEATURE_BROADCAST_SMS */

      /* Remember what channel page was received on */
      mccidl_save_msg_rxed_chan();

      cdma.call_orig = FALSE;  /* Call is not mobile originated */
      mcc_validate_so_req( &page_info );

      #ifdef T_W1
#error code not present
      #endif /* T_W1 */

      MSG_HIGH( "Set cdma.so_req to 0x%x",  cdma.so_req, 0, 0 );

      /* Update Layer2 ack info for page response message */
      mcc_set_layer2_ack(page_info.msg_seq, page_info.ack_type, TRUE);

      if (cdma.sa_tc_state == CDMA_REG) /*lint !e641 */
      {
        /* Report that this Page Response Message is causing us to 
           abandon a Registration Message. */
        mclog_report_registration_suppressed(cdma.reg_type);
      }

      /* Determine the type of transceiver request. */
      if (CAI_IS_SO_SMS(page_info.service_option))
      {
        /* If the page is for an SMS, it is not an urgent page. */
        access_type = SRCH_ACCESS_MOBILE_TERM_TYPE;
      }
      else
      {
        access_type = SRCH_ACCESS_MOBILE_TERM_IMMED_TYPE;
      }

      /* Request transceiver request */
      if ( cdma.sa_tc_state == CDMA_IDLE )
      {
        mcc_request_transceiver_resource(SRCH_HIGH_PERF_ACCESS, 
          access_type, CDMA_PAGE_RES); /*lint !e641 */
      }
      /* Upgrade transceiver resources. */
      else
      {
        if ( mcc_wait_transceiver_resource(SRCH_HIGH_PERF_ACCESS, 
             access_type) == MCC_TRANSCEIVER_RESOURCE_LOST )
        {
          /* -------------------------------------------------------------
          ** Searcher requested system restart. Need to do system exit .
          ** Inform system determination of system lost and determines the
          ** next state to be processed based on the returned SS-Action.
          ** ------------------------------------------------------------- */
          if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                  sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_ADVISE_UNLOCK_RF, NULL ) ) 
             )
          {
            new_state = CDMA_EXIT;
            mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
          }
        }

        if ( new_state != CDMA_EXIT )
        {
          cdma.sa_tc_state = CDMA_PAGE_RES; /*lint !e641 */
        }
      }

      break;

    #ifdef FEATURE_IS2000_REL_A_CC
    case ANNOUNCEMENT_MATCH:

      /* Announcement Paging doesn't need layer2 ack. */
      mcc_layer2.valid_ack = FALSE;
      break;
    #endif /* FEATURE_IS2000_REL_A_CC */

    case NO_MATCH:

      if (msg_ptr->msg.gen.msg_type != CAI_PAGE_MSG)
      {
        /* --------------------------------------
        ** We have received slotted page messages
        ** but none were addressed to us.
        ** -------------------------------------- */
        if (mccidl_done_monitoring_slot(msg_ptr, &page_info, page_match))
        {
          /* -----------------------------------------------------------------
          ** We just got a slotted page message in the designated or next slot
          ** which informed us that no more pages are coming for us, so if or
          ** when we have all the overhead messages we can go to sleep.
          ** ---------------------------------------------------------------- */
          new_state = mccidl_init_sleep(msg_ptr->frm.frame, new_state);
          empty_page = TRUE;
        }
      }
      break;

    default:
      break;
  } /* end switch */

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH &&
      ((msg_ptr->msg.gen.msg_type == CAI_UPM_FIRST_SEG_MSG) ||
       (msg_ptr->msg.gen.msg_type == CAI_UPM_MIDDLE_SEG_MSG) ||
       (msg_ptr->msg.gen.msg_type == CAI_UPM_FINAL_SEG_MSG)))
  {
    /* ------------------------------------------------------------------
    ** Page match done, empty segmented Universal Page Message reassembly
    ** buffer.
    ** ------------------------------------------------------------------ */
    mcc_empty_seg_upm_buffer();
  }
  #endif /* FEATURE_IS2000_REL_A_CC */

  if (msg_ptr->page_match_counter_incremented)
  {
    /* MUX already counted the page message for MER statistic, disable CP MER
       statistical operation for page slot. */
    mclog_mer_config(FALSE, 0);
  }
  else if ((slot_pos == slot) || (slot_pos == ((slot + 1) & sci_mask)))
  {
    /* Compute the real slot, untelescoped */
    
    real_sci_mask = mcc_calculate_sci_mask(slot_cycle_index_s);
    real_slot_pos = ((word)(qw_lo(msg_ptr->frm.frame)) >> 2) & real_sci_mask;
    real_slot = (idle_pgslot & real_sci_mask);

    if (empty_page)
    {
      if ( real_slot_pos == real_slot )
      {
        /* Yes, this happens to be our real valid slot. */
        MSG_MED("Got empty page, slot=%d, our=%d, sci=%d", real_slot_pos,
          real_slot, slot_cycle_index_s);
        /* Update real sci, in-slot MER statistics. */
        mclog_mer_update_counter(FALSE, TRUE, msg_ptr->frm.frame);
      }
      else if ( (real_slot_pos == ((real_slot + 1) & real_sci_mask)) )
      {
        /* This is also valid, just the next rather than our actual slot */
        MSG_MED("Got empty page, next=%d, our=%d, sci=%d", real_slot_pos,
          real_slot, slot_cycle_index_s);
        /* Update real sci, next-slot MER statistics. */
        mclog_mer_update_counter(FALSE, FALSE, msg_ptr->frm.frame);
      }
      else if ( slot_pos == slot )
      {
        /* This is in-slot, but it was a wakeup via telescoping SCI. */
        MSG_MED("Got empty page, tele=%d, our=%d, sci=%d", slot_pos,
          real_slot, slot_cycle_index_s);

        /* Update telescoping sci, in-slot MER statistics. */
        mclog_mer_update_counter(TRUE, TRUE, msg_ptr->frm.frame);
      }
      else /* (slot_pos == ((slot + 1) & sci_mask)) */
      {
        /* This is also a telescoping slot msg, just the next rather than in-slot */
        MSG_MED("Got empty next, tele=%d, our=%d, sci=%d", slot_pos, real_slot,
          slot_cycle_index_s);

        /* Update telescoping sci, next-slot MER statistics. */
        mclog_mer_update_counter(TRUE, FALSE, msg_ptr->frm.frame);
      }
    }
    else
    {
      if ( real_slot_pos == real_slot )
      {
        /* Yes, this happens to be our real valid slot. */
        MSG_MED("Got page msg,   slot=%d, our=%d, sci=%d", real_slot_pos, real_slot,
           slot_cycle_index_s);
        /* Update real sci, in-slot MER statistics. */
        mclog_mer_update_counter(FALSE, TRUE, msg_ptr->frm.frame);
      }
      else if ( (real_slot_pos == ((real_slot + 1) & real_sci_mask)) )
      {
        /* This is also valid, just the next rather than our actual slot */
        MSG_MED("Got page msg,   next=%d, our=%d, sci=%d", real_slot_pos, real_slot,
          slot_cycle_index_s);

        /* Update real sci, next-slot MER statistics. */
        mclog_mer_update_counter(FALSE, FALSE, msg_ptr->frm.frame);
      }
      else if ( slot_pos == slot )
      {
        /* This is in-slot, but it was a wakeup via telescoping SCI. */
        MSG_MED("Got page msg,   tele=%d, our=%d, sci=%d", slot_pos, real_slot,
          slot_cycle_index_s);

        /* Update telescoping sci, in-slot MER statistics. */
        mclog_mer_update_counter(TRUE, TRUE, msg_ptr->frm.frame);
      }
      else /* (slot_pos == ((slot + 1) & sci_mask)) */
      {
        /* This is also a telescoping slot msg, just the next rather than in-slot */
        MSG_MED("Got page,  next tele=%d, our=%d, sci=%d", slot_pos, real_slot,
          slot_cycle_index_s);

        /* Update telescoping sci, next-slot MER statistics. */
        mclog_mer_update_counter(TRUE, FALSE, msg_ptr->frm.frame);
      }
    }
  }

  return (new_state);

} /* mccidl_page_match */

/*===========================================================================

FUNCTION MCCIDL_LOCK_MOB_PROC

DESCRIPTION
  This function processes the CAI Mobile Station Order and Message
  Processing task when the phone is locked.  Essentially the received order
  message is searched for an Unlock order for the mobile.  If the unlock
  order if found, it is processed appropriately.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  All orders other than the Unlock order are dropped even if they are
  addressed to the mobile.

===========================================================================*/

word mccidl_lock_mob_proc
(
  caii_rx_msg_type *msg_ptr,
    /* Pointer to received message */
  word new_state
    /* Next state to be processed */
)
{
  word i;
    /* Index for orders */
  boolean unlock;
    /* Indicator of whether the phone was unlocked */
  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to handset command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  unlock = FALSE;

  if (msg_ptr->gen.msg_type == CAI_PC_ORD_MSG)
  {
    /* Search for Unlock Order in Paging Channel Order Message */
    for (i=0; i < msg_ptr->pc_ord.num_ords; i++)
    {
      if ((msg_ptr->pc_ord.ords[i].gen.order == CAI_LOCK_OR_MAINT_ORD) &&
          (msg_ptr->pc_ord.ords[i].ordq.ordq == CAI_UNLOCK_MASK))
      {
        if (mcc_comp_addr( &msg_ptr->pc_ord.ords[i].gen.hdr
                           ,NULL /* this field is used for BCSMS only */
           ))
        {
          /* Order is an unlock order for this mobile */
          MSG_MED( "Received Unlock Order", 0, 0, 0 );

          unlock = TRUE;

          /* ------------------------------------
          ** Increment the count PAG_3 or FCCCH_3
          ** ------------------------------------ */
          mcc_increment_pag_3_or_fccch_3();
        }
      }
    } /* end of for */
  }

  if (unlock)
  {
    /* --------------------------------------------------------------
    ** Inform system determination of unlock.
    ** Determine the next state to be processed based on the returned
    ** SS-Action.
    ** -------------------------------------------------------------- */
    if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
        sd_ss_ind_cdma_opr_unlock(NULL)))
    {
      /* Exit CDMA with indication that Unlock order was received */
      mcc_exit.exit = MCC_UNLOCK;
      new_state = CDMA_EXIT;
    }

    /* Send unlock command to UI */
    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->hdr.cmd = CM_UNLOCK_F;
      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      ERR("No free buffers on cm_mc_rpt_free_q",0,0,0 );
    }
  }

  return (new_state);

} /* mccidl_lock_mob_proc */

/*===========================================================================

FUNCTION MCCIDL_PROC_TMSI_ASSIGN

DESCRIPTION
  This function processes a TMSI Assignment Message received on the
  Paging Channel or Forward Common Control Channel while in Idle State.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
word mccidl_proc_tmsi_assign
(
  mccdma_matched_msg_type *matched_msg_ptr,
    /* Pointer to Paging Channel TMSI Assignment Message */
  word new_state
    /* Next state to be processed */
)
{
  /* --------------------------------------------------------------
  ** Perform Layer 3 processing of the TMSI Assignment Message
  ** -------------------------------------------------------------- */

  MSG_MED( "Rxed TMSI Assignment in idle", 0, 0, 0 );

  /* Save new TMSI information */
  tmsi_info_update (&matched_msg_ptr->pc_tmsi.data);

  cdma.sa_resp_msg = CAI_AC_TMSI_CMP_MSG;
  new_state = CDMA_ORD_RES; /*lint !e641 */

  return (new_state);

} /* mccidl_proc_tmsi_assign */

/*===========================================================================

FUNCTION MCCIDL_PROC_DATA_BURST

DESCRIPTION
  This function processes a Data Burst Message received on the
  Paging Channel or Forward Common Control Channel while in Idle State.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
word mccidl_proc_data_burst
(
  mccdma_matched_msg_type *matched_msg_ptr,
    /* Pointer to Paging Channel Data Burst Message */
  word new_state
    /* Next state to be processed */
)
{

  #ifdef FEATURE_PASSPORT_MODE
  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to command to send to UI task */
  #endif /* FEATURE_PASSPORT_MODE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* --------------------------------------------------------------
  ** Perform Layer 3 processing of the Data Burst Message
  ** -------------------------------------------------------------- */

  MSG_MED( "Rxed Data Burst in idle", 0, 0, 0 );

  #ifdef FEATURE_CDSMS
  /* Check if valid Data Burst SMS message before processing */
  if ((matched_msg_ptr->pc_burst.burst_type == CAI_SHORT_MSG_SERVICES) &&
     (matched_msg_ptr->pc_burst.msg_number == 1) &&
     (matched_msg_ptr->pc_burst.num_msgs == 1) &&
     (matched_msg_ptr->pc_burst.num_fields > 0))
  {
    /* Events report: SMS received */
    event_report(EVENT_SMS_RECEIVED);

    /* ------------ */
    /* SMS Message  */
    /* ------------ */
    /* Process mobile-terminated SMS message */
    cm_mt_dbm_msg( (byte) CAI_SHORT_MSG_SERVICES,
                   (byte) CM_SMS_FORMAT_CDMA,
                   (byte*) matched_msg_ptr->pc_burst.chari,
                   (word) matched_msg_ptr->pc_burst.num_fields );

    /* done with CDMA mt sms msg */
    return new_state;  /* SHORT-RETURN */
  }
  #endif /* FEATURE_CDSMS */

  #ifdef FEATURE_DBM
  /* Check if valid Data Burst SMS message before processing */
  if ((matched_msg_ptr->pc_burst.burst_type == CAI_POSITION_DET) &&
     (matched_msg_ptr->pc_burst.msg_number == 1) &&
     (matched_msg_ptr->pc_burst.num_msgs == 1) &&
     (matched_msg_ptr->pc_burst.num_fields > 0))
  {
    /* ------------------------------- */
    /* Position Determination Message  */
    /* ------------------------------- */
    /* Process mobile-terminated PD message */

    mcc_send_mt_dbm_data(
                     (uint8) CAI_POSITION_DET,   /* burst_type */
                     (byte*) matched_msg_ptr->pc_burst.chari,
                     (uint8) matched_msg_ptr->pc_burst.num_fields );

    #ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
    #endif /* FEATURE_GPSONE_NEW_L2_ACK */

    /* done with CDMA mt PD msg */
    return new_state;  /* SHORT-RETURN */
  }
  #endif /* FEATURE_DBM */

  #ifdef FEATURE_PASSPORT_MODE
  /* Check for an Extended Burst Type - International Data Burst */
  if (mcc_passport_mode_is_jcdma() &&
      matched_msg_ptr->pc_burst.burst_type == CAI_EXTENDED_BURST_TYPE_INTL)
  {
    /* ----------------------------------- */
    /* Extended Burst Type - International */
    /* ----------------------------------- */
    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->hdr.cmd = CM_EXT_BRST_INTL_F;
      cmd_ptr->ext_brst_intl_rec.mcc =
        b_unpackw( matched_msg_ptr->pc_burst.chari,
          FPOS( cai_ext_brst_intl_type, mcc ),
          FSIZ( cai_ext_brst_intl_type, mcc ));

      cmd_ptr->ext_brst_intl_rec.db_subtype =
        b_unpackb( matched_msg_ptr->pc_burst.chari,
          FPOS( cai_ext_brst_intl_type, db_subtype ),
          FSIZ( cai_ext_brst_intl_type, db_subtype ));

      cmd_ptr->ext_brst_intl_rec.chg_ind =
        b_unpackb( matched_msg_ptr->pc_burst.chari,
          FPOS( cai_ext_brst_intl_type, chg_ind ),
          FSIZ( cai_ext_brst_intl_type, chg_ind ));

      cmd_ptr->ext_brst_intl_rec.sub_unit =
        b_unpackb( matched_msg_ptr->pc_burst.chari,
          FPOS( cai_ext_brst_intl_type, sub_unit ),
          FSIZ( cai_ext_brst_intl_type, sub_unit ));

      cmd_ptr->ext_brst_intl_rec.unit =
        b_unpackb( matched_msg_ptr->pc_burst.chari,
          FPOS( cai_ext_brst_intl_type, unit ),
          FSIZ( cai_ext_brst_intl_type, unit ));

      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      ERR( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
    return (new_state);
  }
  #endif /* FEATURE_PASSPORT_MODE */

  #ifdef FEATURE_DBM
  /* Check if valid Short Data Burst message before processing */
  if (mccap_item_is_supported(MCCAP_DF_SDB) &&
     (matched_msg_ptr->pc_burst.burst_type == CAI_SHORT_DATA_BURST) &&
     (matched_msg_ptr->pc_burst.msg_number == 1) &&
     (matched_msg_ptr->pc_burst.num_msgs == 1) &&
     (matched_msg_ptr->pc_burst.num_fields > 0))
  {
    /* ------------ */
    /* SDB Message  */
    /* ------------ */
    /* Process mobile-terminated SDB message */

    mcc_send_mt_dbm_data(
                     (uint8) CAI_SHORT_DATA_BURST,   /* burst_type */
                     (byte*) matched_msg_ptr->pc_burst.chari,
                     (uint8) matched_msg_ptr->pc_burst.num_fields );

    /* done with CDMA mt sdb msg processing*/
    return new_state;
  }
  #endif /* FEATURE_DBM */


  /* ----------------------------------------------------------------------
  ** We only get here if we did not recognize the Data Burst as one we know
  ** how to process.  Reject as per IS-95A (JSTD-008) section 6(2).6.4.4
  ** ---------------------------------------------------------------------- */
  /* Data Bursts not implemented. */

  /* Don't send the reject for the broadcast data burst message. */
  if (matched_msg_ptr->gen_pc.hdr.addr_type != CAI_BCAST_ADDR_TYPE)
  {
    mcc_setup_reject_ord( CAI_REJ_CAP, CAI_PC_BURST_MSG, 0, 0 );
    new_state = CDMA_ORD_RES; /*lint !e641 */
  }
  else
  {
    MSG_HIGH("BC DBM not supported", 0, 0, 0);
  }

  return (new_state);

} /* mccidl_proc_data_burst */

/*===========================================================================

FUNCTION MCCIDL_PROC_STREQ

DESCRIPTION
  This function processes a Status Request Message received on the
  Paging Channel or Forward Common Control Channel while in Idle State.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
word mccidl_proc_streq
(
  mccdma_matched_msg_type *matched_msg_ptr,
    /* Pointer to Paging Channel Status Request Message */
  word new_state
    /* Next state to be processed */
)
{
  caii_pc_streq_type  *req_ptr = &matched_msg_ptr->pc_streq;
    /* Pointer to status request message */
  caii_ac_strsp_type  *rsp_ptr = &cdma_strsp;
    /* Pointer to status response message */
  int rec_siz;
    /* For receiving status from called function */
  word all_rec_siz;
    /* For computing cumulative length of all records */
  word i;
    /* For indexing through rec information types */
  boolean ok;
    /* For checking status during loop */
  byte reject_reason = CAI_REJ_UNSP;
    /* Reason for rejecting status request message */

  /* --------------------------------------------------------------
  ** Perform Layer 3 processing of the Status Request Message
  ** -------------------------------------------------------------- */

  MSG_MED( "Rxed Status Request in idle", 0, 0, 0 );

  if (P_REV_IS_4_OR_GREATER)
  {
     rsp_ptr->msg_type = CAI_AC_EXT_STRSP_MSG;
  }
  else
  {
     rsp_ptr->msg_type = CAI_AC_STRSP_MSG;
  }

  rsp_ptr->qual     = req_ptr->qual;
  rsp_ptr->band     = req_ptr->band;
  rsp_ptr->op_mode  = req_ptr->op_mode;
  rsp_ptr->num_recs = 0;

  /* -----------------------------------------------------
  ** If QUAL_INFO_TYPE doesn't include a field, it implies
  ** the current band class or operating mode.
  ** ----------------------------------------------------- */
  if (req_ptr->qual == CAI_NO_QUAL)
  {
    switch (mcc_get_group_for_band_class(cdma.band_class))
    {
      case MCC_CELLULAR_GROUP:
      {
        /* We need to set band and op_mode to CELLULAR (IS95 BC0)*/
        req_ptr->band = cdma.band_class;
        req_ptr->op_mode = CAI_IS95_CDMA_BC0_OP_MODE;
        /* Notice that CAI_IS95_CDMA_OP_MODE is the same as
           CAI_IS95_CDMA_BC0_OP_MODE */
        break;

      }
      case MCC_PCS_GROUP:
      {
        /* We need to set band and op_mode to JSTD008 (IS95 BC1) */
        req_ptr->band = cdma.band_class;      
        req_ptr->op_mode = CAI_IS95_CDMA_BC1_OP_MODE;
        /* Notice that CAI_J_STD_008_OP_MODE is the same as
           CAI_IS95_CDMA_BC1_OP_MODE */
        break;
      }
      default:
      {
        ERR("Unsupported band class=%d",mc_get_band_class(),0,0);
      }
    }
  }
  else if (req_ptr->qual == CAI_BAND_QUAL)
  {
    switch (mcc_get_group_for_band_class(cdma.band_class))
    {
      case MCC_CELLULAR_GROUP:
      {
        /* We need to set the op_mode to IS95 CDMA */
        req_ptr->op_mode = CAI_IS95_CDMA_BC0_OP_MODE;
        /* Notice that CAI_IS95_CDMA_OP_MODE is the same as
           CAI_IS95_CDMA_BC0_OP_MODE */
        break;

      }
      case MCC_PCS_GROUP:
      {
        /* We need to set the op_mode to JSTD008 */
        req_ptr->op_mode = CAI_IS95_CDMA_BC1_OP_MODE;
        /* Notice that CAI_J_STD_008_OP_MODE is the same as
           CAI_IS95_CDMA_BC1_OP_MODE */
        break;
      }
      default:
      {
        ERR("Unsupported band class=%d",mc_get_band_class(),0,0);
      }
    }
  }

  /* -------------------------
  ** Build information records
  ** ------------------------- */

  all_rec_siz = 0;
  ok = TRUE;

  if (req_ptr->num_fields == 0)
  {
    /* We can't send a response with no info, so we must reject */
    ok = FALSE;
  }

  for ( i = 0; i < req_ptr->num_fields; i++ )
  {
    rec_siz =
      mcc_info_rec(TRUE, /* paging channel or forward common vontrol channel*/
                   req_ptr->band,
                   req_ptr->op_mode,
                   req_ptr->rec_typ[ i ],
                   (void *) &rsp_ptr->recs[ all_rec_siz/sizeof(dword) ],
                   (word)(sizeof(rsp_ptr->recs) - all_rec_siz),
                   &reject_reason);

    if (rec_siz < 0)
    {
      ok = FALSE;
      break;
    }

    else if (rec_siz == 0)
    {
      break;
    }

    else
    {
      all_rec_siz += (word) rec_siz;
      //We do not want to cast away a non-dword aligned pointer into our struct, so adjust
      //accordingly
      all_rec_siz = (all_rec_siz%4)?(all_rec_siz+(sizeof(dword))-all_rec_siz%4):all_rec_siz;
      rsp_ptr->num_recs++;
    }
  } /* for */

  if (ok)
  {
    if (P_REV_IS_4_OR_GREATER)
    {
      cdma.sa_resp_msg = CAI_AC_EXT_STRSP_MSG;
    }
    else
    {
      cdma.sa_resp_msg = CAI_AC_STRSP_MSG;
    }

    new_state = CDMA_ORD_RES; /*lint !e641 */
  }
  else
  {
    mcc_setup_reject_ord ( reject_reason, CAI_PC_STREQ_MSG, 0, 0 );
    new_state = CDMA_ORD_RES; /*lint !e641 */
  }

  return (new_state);

} /* mccidl_proc_streq */

/*===========================================================================

FUNCTION MCCIDL_PROC_ORD_MSG

DESCRIPTION
  This function processes an Order Message received on the Paging Channel
  or Forward Common Control Channel while in Idle State.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
word mccidl_proc_ord_msg
(
  mccdma_matched_msg_type *matched_msg_ptr,
    /* Pointer to Paging Channel Order Message */
  word new_state
    /* Next state to be processed */
)
{
  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to handset command */

  #ifdef FEATURE_AUTH
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  auth_rpt_type *auth_rpt_ptr;
    /* Pointer to report from authentication task */
  #endif /* FEATURE_AUTH */

  #ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
  #endif /* FEATURE_IS2000_REL_C_AKA */
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* --------------------------------------------------------------
  ** Perform Layer 3 processing of the Paging Channel Order Message
  ** -------------------------------------------------------------- */

  switch (matched_msg_ptr->pc_ord.order)
  {
    case CAI_LOCK_OR_MAINT_ORD:
      if (matched_msg_ptr->pc_ord.ordq == CAI_UNLOCK_MASK)
      {
        MSG_MED("Rxed Unlock Order in idle", 0, 0, 0);
        /* ------------
        ** Unlock Order
        ** ------------ */
        /* Send unlock command to UI */
        if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cmd_ptr->hdr.cmd = CM_UNLOCK_F;
          mcc_cm_mc_rpt( cmd_ptr );
        }
        else
        {
          ERR("No free buffers on cm_mc_rpt_free_q",0,0,0 );
        }

        /* Events report: Unlock */
        event_report(EVENT_UNLOCK);

      }
      else if (matched_msg_ptr->pc_ord.ordq & CAI_LUPC_MASK)
      {
        /* -----------------------------
        ** Lock Until Power-Cycled Order
        ** ----------------------------- */
        MSG_MED( "Rxed Lock Order in idle", 0, 0, 0 );

        /* --------------------------------------------------------------
        ** Inform system determination of lock.
        ** Determine the next state to be processed based on the returned
        ** SS-Action.
        ** -------------------------------------------------------------- */
        if (mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
            sd_ss_ind_cdma_opr_lock(NULL)))
        {
          /* No ack will be sent for LUPC */
          matched_msg_ptr->gen_pc.hdr.ack_req = FALSE;
        }
        else
        {
          new_state = CDMA_EXIT;
          mcc_exit.exit = MCC_LOCK;
        }

        mcc_nv_data.lckrsn_p.lock_order = TRUE;
        mcc_nv_data.lckrsn_p.lock_reason =
          matched_msg_ptr->pc_ord.ordq & 0x0F;

        /* Write reason for lock to NVRAM */
        mcc_nv_buf.item = NV_LCKRSN_P_I;
        mcc_nv_buf.cmd = NV_WRITE_F;
        mcc_nv_buf.data_ptr = &mcc_nv_data;

        (void) mcc_nv_cmd(&mcc_nv_buf);

        /* Send lock command to UI */
        if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cmd_ptr->hdr.cmd = CM_LOCK_F;
          mcc_cm_mc_rpt(cmd_ptr);
        }
        else
        {
          ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
        }
        /* Clear call state indicator flag */
        cdma.call_orig = FALSE;

        /* Events report: Lock */
        event_report(EVENT_LOCK);
      }

      else if (matched_msg_ptr->pc_ord.ordq & CAI_MAINT_REQ_MASK)
      {
        MSG_MED("Rxed Maintenance Required Order in idle", 0, 0, 0);
        /* --------------------------
        ** Maintenance Required Order
        ** -------------------------- */
        /* Write reason for maintenance required to NVRAM */
        mcc_nv_buf.item = NV_MAINTRSN_I;
        mcc_nv_buf.cmd = NV_WRITE_F;
        mcc_nv_data.maintrsn.maint_order = TRUE;
        mcc_nv_data.maintrsn.maint_reason =
          matched_msg_ptr->pc_ord.ordq & 0x0F;
        mcc_nv_buf.data_ptr = &mcc_nv_data;

        (void) mcc_nv_cmd(&mcc_nv_buf);

        /* Send maintenance required command to UI */
        if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cmd_ptr->hdr.cmd = CM_MAINTREQ_F;
          mcc_cm_mc_rpt(cmd_ptr);
        }
        else
        {
          ERR("No free buffers on cm_mc_rpt_free_q",0,0,0 );
        }
      }
      else
      {
        /* -------------------------------------------------------
        ** Send a Mobile Station Reject Order (invalid ordq field)
        ** ------------------------------------------------------- */
        mcc_setup_reject_ord( CAI_REJ_FIELD, CAI_PC_ORD_MSG,
          CAI_LOCK_OR_MAINT_ORD, matched_msg_ptr->pc_ord.ordq );
        new_state = CDMA_ORD_RES; /*lint !e641 */
      }
      break;

    case CAI_ABBR_ALERT_ORD:
      /* ------------------------------------------------------
      ** Abbreviated Alert Order --> Notify or remind the user
      ** user that previously selected alternative call routing
      ** features are currently enabled
      ** ------------------------------------------------------ */
      MSG_MED("Rxed Abbreviated Alert Order in Idle", 0, 0, 0);

      /* Send Abbreviated Alert command to UI */
      if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
      {
        cmd_ptr->abrv.hdr.cmd = CM_ABRV_ALERT_F;
        cmd_ptr->abrv.call_id = CM_CALL_ID_UNASSIGNED;
        mcc_cm_mc_rpt(cmd_ptr);
      }
      else
      {
        ERR( "No free buffers on cm_mc_rpt_free_q", 0, 0, 0 );
      }
      break;

    case CAI_REG_ORD:
      /* ----------------------------------------------------
      ** Registration Request, registration accepted or
      ** registration rejected order
      ** ---------------------------------------------------- */
      if (matched_msg_ptr->pc_ord.ordq == CAI_REG_ACK)
      {
        /* ---------------------------
        ** Registration accepted order
        ** --------------------------- */
      }
      else if (matched_msg_ptr->pc_ord.ordq == CAI_REG_ACC)
      {
        /* --------------------------------------------
        ** Registration accepted order with ROAM_INDI
        ** ------------------------------------------ */
        MSG_MED("Rxed Registration Accept with ROAM_INDI: %d",
                matched_msg_ptr->pc_ord.roam_indi, 0,0);

        #ifdef FEATURE_REG_ACC_ROAM_IND
        /*  Interchange roam indication, since DB values
        **  and TSB58 roam values are interchanged for
        **  ROAM_ON and ROAM_OFF
        **  Note that for SD20 db_roam_type & sd_roam_e_type
        **  are equivalent for now.
        */
        sd_si_roam_update(
                            (sd_roam_e_type)
                            matched_msg_ptr->pc_ord.roam_indi
                         );
        #else
        /* ------------------------------------------------------------
        **  The Roaming indicator would normally be set here with the
        **  value received from the Base Station. The standard indicates
        **  that it may be set. Notice the keyword "may". The Roaming
        **  Indicator is not set for of the following reasons:
        **
        **  1) The base station may transmit a value in conflict with
        **     the value of the roaming indicator stored in the PRL.
        **     This may present the user with incorrect, or unreliable
        **     roaming status.
        **  2) After registering, we may go into a fast fade scenario,
        **     and not register after we re-acquire the system. This
        **     may lead to a incorrect roaming status, since we have
        **     not received the roaming indicator, in the Registration
        **     accepted order.
        **
        **  Therefore, we simply display an F3 Message on the DM, and
        **  ignore the roaming indicator in this order. We still display
        **  the roaming indicator as programmed in the PRL.
        ** -----------------------------------------------------------*/
        #endif /* FEATURE_REG_ACC_ROAM_IND */

      }
      else if (matched_msg_ptr->pc_ord.ordq == CAI_REG_REQ)
      {
        #ifdef FEATURE_PASSPORT_MODE
        if(mcc_passport_mode_is_jcdma() &&
           mcc_msg_is_blocked(CDMA_MSG_TYPE_REG))
        {
          MSG_HIGH("Registration Blocked!",0, 0, 0);
        }
        else
        #endif /* FEATURE_PASSPORT_MODE */
        {      
          /* --------------------------
          ** Registration request order
          ** -------------------------- */
          MSG_MED("Rxed Registration Request Order in idle", 0, 0, 0);
          new_state = CDMA_REG; /*lint !e641 */
          cdma.reg_type = CAI_ORDERED_REG;

          /* ---------------------------------------------------------------
           * Set flag mccidl_reg_required, so mccidl_online() can directly
           * enter "CDMA_REG" without calling mccreg_idle_check() again.
           * --------------------------------------------------------------- */
          mccidl_reg_required = TRUE;
        }
      }
      else if (matched_msg_ptr->pc_ord.ordq == CAI_REG_REJ)
      {
        /* ---------------------------
        ** Registration rejected order
        ** --------------------------- */

        /* Events report: Registration rejected */
        event_report(EVENT_REGISTRATION_REJECTED);

        if (!matched_msg_ptr->pc_ord.hdr.ack_req)
        {
          /* Registration rejected and no TMSI delete */
          /* so reset full TMSI timer.                */
          tmsi_reset_full_timer ();

          /* --------------------------------------------------------
          ** Inform system determination of registration rejected and
          ** determines the next state to be processed based on the
          ** returned SS-Action.
          ** -------------------------------------------------------- */
          if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_opr_reg_rej(NULL)))
          {
            new_state = CDMA_EXIT;
            mcc_exit.exit = MCC_REG_REJECTED;
          }
        }
        else
        {
          /*****************************
          ** Check for reject, if not  *
          ** Save for later processing *
          ******************************/
          mcc_layer2.idle_msg_pending = TRUE;
          mcc_layer2.pending_idle_msg = *matched_msg_ptr;
        }
      }
      else if (matched_msg_ptr->pc_ord.ordq == CAI_REG_REJ_DEL_TMSI)
      {
        /* -----------------------------------------
        ** Registration rejected order, delete TMSI
        ** ----------------------------------------- */

        /* Events report: Registration rejected */
        event_report(EVENT_REGISTRATION_REJECTED);

        if (!matched_msg_ptr->pc_ord.hdr.ack_req)
        {
          /* Registration rejected and TMSI delete   */
          /* so reset TMSI code and all TMSI timers. */
          tmsi_code_and_timer_reset ();

          /* --------------------------------------------------------
          ** Inform system determination of registration rejected and
          ** determines the next state to be processed based on the
          ** returned SS-Action.
          ** -------------------------------------------------------- */
          if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_opr_reg_rej(NULL)))
          {
            new_state = CDMA_EXIT;
            mcc_exit.exit = MCC_REG_REJECTED;
          }
        }
        else
        {
          /*****************************
          ** Check for reject, if not  *
          ** Save for later processing *
          ******************************/
          mcc_layer2.idle_msg_pending = TRUE;
          mcc_layer2.pending_idle_msg = *matched_msg_ptr;
        }
      }

      #ifdef FEATURE_IS2000_REL_A
      else if (matched_msg_ptr->pc_ord.ordq ==
               CAI_REG_ACCEPT_ORDER_ROAM_IND_AND_ENC_INC)
      {
        /* We ignore the Roaming indicator, see reasoning above (CAI_REG_ACC) */
        #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
        #else
        if (matched_msg_ptr->pc_ord.c_sig_encrypt_mode !=
            CAI_SIG_ENC_MODE_DISABLED)
        #endif /* FEATURE_IS2000_REL_A_AES */
        {
          /* ----------------------------------------------------
          ** At this point, we don't support Signaling Encryption
          ** ---------------------------------------------------- */
          /* --------------------------------------------------------
          ** Inform system determination of registration rejected and
          ** determines the next state to be processed based on the
          ** returned SS-Action.
          ** -------------------------------------------------------- */
          if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_opr_reg_rej(NULL)))
          {
            new_state = CDMA_EXIT;
            mcc_exit.exit = MCC_REG_REJECTED;
          }
        }
      
        #if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
        #endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */
      } /* CAI_REG_ACCEPT_ORDER_ROAM_IND_AND_ENC_INC */
      #endif /* FEATURE_IS2000_REL_A */

      else
      {
        /* -------------------------------------------------------
        ** Send a Mobile Station Reject Order (invalid ordq field)
        ** ------------------------------------------------------- */
        mcc_setup_reject_ord( CAI_REJ_FIELD, CAI_PC_ORD_MSG,
          CAI_REG_ORD, matched_msg_ptr->pc_ord.ordq );
        new_state = CDMA_ORD_RES; /*lint !e641 */
      }
      break;

    case CAI_AUDIT_ORD:
      MSG_MED( "Rxed Audit Order", 0, 0, 0 );
      break;

    case CAI_BS_ACK_ORD:
      MSG_MED( "Rxed BS_ACK Order", 0, 0, 0 );
      break;

    case CAI_BS_CHAL_CONF_ORD:
      #ifdef FEATURE_AUTH
      cdma.sa_resp_msg = CAI_AC_ORD_MSG;
      cdma.sa_resp_ord.ordq.order = CAI_SSD_UPDATE_ORD;

      /* Unless all goes will we will send an SSD Update Rejection Order */
      cdma.sa_resp_ord.ordq.ordq = CAI_SSD_UPDATE_REJ;

      if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
      {
        auth_cmd_ptr->hdr.command = AUTH_FINISH_SSD_UPDATE_F;
        auth_cmd_ptr->f_ssd.authbs = matched_msg_ptr->pc_ord.authbs;
        auth_cmd_ptr->f_ssd.rpt_function = mcc_queue_auth_rpt;

        mcc_auth_cmd(auth_cmd_ptr);

        if (auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S)
        {
          /* Wait for auth to report back if SSD Update was successful */
          /* ----------------------------------
          ** Wait for MCC_AUTH_Q_SIG to be set.
          ** ---------------------------------- */
          MSG_MED("Waiting for AUTH report", 0, 0, 0);

          (void) mcc_wait(MCC_AUTH_Q_SIG);

          auth_rpt_ptr = (auth_rpt_type*) q_get(&mc_auth_q);

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

            /* Send SSD Update Confirmation Order */
            cdma.sa_resp_ord.ordq.ordq = CAI_SSD_UPDATE_CONF;

            #if defined(FEATURE_OTASP) && defined(FEATURE_OTASP_STATUS)
            /* Send OTASP status to CM */
            otasp_report_status_to_cm(CM_OTASP_STATUS_SSD_UPDATED);
            #endif /* FEATURE_OTASP && FEATURE_OTASP_STATUS */
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
      #else
      mcc_setup_reject_ord(CAI_REJ_CAP, CAI_PC_ORD_MSG,
        matched_msg_ptr->pc_ord.order, matched_msg_ptr->pc_ord.ordq );
      #endif /* FEATURE_AUTH */

      new_state = CDMA_ORD_RES; /*lint !e641 */
      break;

    case CAI_LOCAL_CTL_ORD:
      /* -----------------------------------------
      ** Local control orders are not implemented.
      ** Reject as unsupported capability.
      ** ----------------------------------------- */
      mcc_setup_reject_ord(CAI_REJ_CAP, CAI_PC_ORD_MSG,
        matched_msg_ptr->pc_ord.order, matched_msg_ptr->pc_ord.ordq );
      new_state = CDMA_ORD_RES; /*lint !e641 */
      break;

    case CAI_REORDER_ORD:
    case CAI_INTERCEPT_ORD:
    case CAI_RELEASE_ORD:
      /* --------------------------------------------------------
      ** Invalid order for Idle State --> IS-95A section 6.6.2.4
      ** says we no longer reject these messages. (Table 6.7.3-1)
      ** -------------------------------------------------------- */
      break;

    case CAI_SLOTTED_MODE_ORD:
      /* ---------------------------
      ** Received Slotted Mode order
      ** --------------------------- */
      if ((cdma.scm & SCM_SLOTTED_CLASS_M) == SCM_SLOTTED_V)
      {
        idle_slotted = TRUE;
        mccidl_sleep_ok = TRUE;
        /* Clear the pending Slotted mode timer */
        MSG_MED("Slotted Mode Order, clear timer", 0, 0, 0);
        (void) rex_clr_timer( &mcc_slotted_timer );
      }
      break;

    case CAI_RETRY_ORD:
      /* -----------
      ** Retry order
      ** ----------- */
      if (P_REV_IS_6_OR_GREATER)
      {
        if ( (matched_msg_ptr->pc_ord.retry_type == CAI_RETRY_CLR_ALL) || /*lint !e641 */
             (matched_msg_ptr->pc_ord.retry_type == CAI_RETRY_ORIG)  /*lint !e641 */
      #ifdef FEATURE_IS2000_REL_D
#error code not present
      #endif
           )
        {
          if (!matched_msg_ptr->pc_ord.hdr.ack_req)
          {
            update_retry_delay((cai_retry_order_type) matched_msg_ptr->pc_ord.retry_type, 
              matched_msg_ptr->pc_ord.retry_delay);
          }
          else
          {
            /* --------------------------------------------------------
            ** We need to send an ack before processing this messsage.
            ** Save this message for later processing.
            ** -------------------------------------------------------- */
            mcc_layer2.idle_msg_pending = TRUE;
            mcc_layer2.pending_idle_msg = *matched_msg_ptr;
          } /* end if (!matched_msg_ptr->pc_ord.hdr.ack_req) */
        }
        else
        {
          /* -------------------------------------------------------
          ** Send a Mobile Station Reject Order (invalid ordq field)
          ** ------------------------------------------------------- */
          mcc_setup_reject_ord( CAI_REJ_FIELD, CAI_PC_ORD_MSG,
            CAI_RETRY_ORD, matched_msg_ptr->pc_ord.ordq );
          new_state = CDMA_ORD_RES; /*lint !e641 */
        } /* end if CAI_RETRY_CLR_ALL || CAI_RETRY_ORIG */
      }
      else
      {
        /* -------------------------------------------
        ** Reject Order if P_REV_IN_USE is less than 6
        ** ------------------------------------------- */
        mcc_setup_reject_ord(CAI_REJ_UNSP, CAI_PC_ORD_MSG, CAI_RETRY_ORD,
          matched_msg_ptr->pc_ord.ordq);
        new_state = CDMA_ORD_RES; /*lint !e641 */
      } /* end if (!P_REV_IS_6_OR_GREATER) */
      break;

   #ifdef FEATURE_IS2000_REL_A
   case CAI_BS_REJ_ORD:
     MSG_MED("BS Reject Order", 0, 0, 0);
     /* --------------------------------------------------------------------
     ** Enhanced encryption is not supported, in the meanwhile MS just exit
     ** to system determination if Base Station Reject Order is received.
     ** -------------------------------------------------------------------- */
     /* -------------------------------------------------------------------
     ** Inform system determination of encryption failure as registration
     ** rejected and determines whether the mobile shall continue on the
     ** current system.
     ** ------------------------------------------------------------------- */
     if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
         sd_ss_ind_cdma_opr_reg_rej(NULL)))
     {
       new_state = CDMA_EXIT;
       mcc_exit.exit = MCC_ENCRYPTION_MI_FAILED;
     }
     break;
   #endif /* FEATURE_IS2000_REL_A */

   #ifdef FEATURE_IS2000_REL_D
#error code not present
   #endif /* FEATURE_IS2000_REL_D */

    default:
      /* -------------------------------------------------
      ** Invalid order code --> We no longer reject these.
      ** ------------------------------------------------- */
      break;

  } /* end switch (order type) */

  return (new_state);

} /* mccidl_proc_ord_msg */

/*===========================================================================

FUNCTION MCCIDL_PROC_CHN_ASN

DESCRIPTION
  This function processes a Channel Assignment or Extended Channel
  Assignment message received on the Paging Channel or Forward Common Control
  Channel while in Idle State.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
word mccidl_proc_chn_asn
(
  mccdma_matched_msg_type *matched_msg_ptr,
    /* Pointer to Paging Channel Message */
  word curr_state
    /* Current state to be processed */
)
{
  word new_state = curr_state;
  word num_pilots;
    /* Number of entries in pilot_pn array */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (matched_msg_ptr->chnasn.assign_mode == CAI_EXT_PAGING_CHAN_ASSIGN
      || matched_msg_ptr->chnasn.assign_mode == CAI_PAGING_CHAN_ASSIGN)
  {
    MSG_MED("Got (Extended) Page Chan Assign", 0, 0, 0);
    num_pilots = matched_msg_ptr->chnasn.mode.am1.num_pilots;
    if (matched_msg_ptr->chnasn.assign_mode == CAI_PAGING_CHAN_ASSIGN &&
        matched_msg_ptr->chnasn.msg_type == CAI_CHN_ASN_MSG && 
        (mcc_is_band_class_in_group(MCC_PCS_GROUP, cdma.band_class))
       ) //lint !e506 !e774
    {
      /* Assignment mode is not supported for BC 1 */
      mcc_setup_reject_ord(CAI_REJ_CAP, CAI_CHN_ASN_MSG, 0, 0);
      new_state = CDMA_ORD_RES; /*lint !e641 */
    }
    else if (num_pilots != 0)
    {
      /* ----------------------------------------------------------
      ** In the following if statement, if FREQ_INCL is 1. We check
      ** if the proposed band is supported by the mobile. This is
      ** a hook for tri-mode operation. SUPPORTED_CDMA_BAND is set
      ** at compile time, therefore, following checking is also
      ** valid for single band targets.
      ** ---------------------------------------------------------- */
      if ((matched_msg_ptr->chnasn.mode.am1.freq_incl) &&
          (!mcc_is_band_class_supported(matched_msg_ptr->chnasn.mode.am1.band_class)))
      {
        /* Assignment to this band_class is not supported */
        mcc_setup_reject_ord(CAI_REJ_CAP, matched_msg_ptr->chnasn.msg_type, 0, 0);
        new_state = CDMA_ORD_RES; /*lint !e641 */
      }
      else if (matched_msg_ptr->chnasn.hdr.ack_req)
      {
        /* ----------------------------------------
        ** Delay processing of Channel Assignment
        ** until after we send the acknowledgement.
        ** ---------------------------------------- */
        mcc_layer2.idle_msg_pending = TRUE;
        mcc_layer2.pending_idle_msg = *matched_msg_ptr;
      }
      else
      {
        /* ------------------------------------------------------
        ** Inform system determination of CAM/ECAM to CDMA paging
        ** channel assignment and determines the next state to be
        ** processed based on the returned SS-Action.
        ** ------------------------------------------------------ */
        if (mcc_inform_sd_chan_assign(&matched_msg_ptr->chnasn,
            MEAS_BACK_ALLOWED))
        {
          mcc_pc_assign(new_state, &matched_msg_ptr->chnasn);
          /* -----------------------------
          ** Reset Idle State to CDMA_INIT
          ** ----------------------------- */
          new_state = CDMA_IDLE_INIT; /*lint !e641 */

          /* Start the BS insanity timer */
          mccidl_set_bs_insanity_timer();
        }
        else
        {
          mcc_setup_reject_ord(CAI_REJ_CAP, matched_msg_ptr->chnasn.msg_type, 0,
            0);
          new_state = CDMA_ORD_RES; /*lint !e641 */
        }

      }
    }
    else /* No pilots! */
    {
      mcc_setup_reject_ord(CAI_REJ_FIELD, matched_msg_ptr->chnasn.msg_type,
        0, 0);
      new_state = CDMA_ORD_RES; /*lint !e641 */
    }
  }

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  else /* Invalid ASSIGN_MODE for Idle State */
  {
    /* See IS-95A 6.6.2.4 item 6 */
    mcc_setup_reject_ord(CAI_REJ_STATE, matched_msg_ptr->chnasn.msg_type, 0, 0);

    new_state = CDMA_ORD_RES; /*lint !e641 */
  }

  return (new_state);

} /* mccidl_proc_chn_asn */

/*===========================================================================

FUNCTION MCCIDL_PROC_PAGE

DESCRIPTION
  This function processes a Page Message or Slotted Page Message received
  on the Paging Channel or Forward Common Control Channel while in Idle
  State or Update Overhead Information Substate of System Access State.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
word mccidl_proc_page
(
  mccrxtx_msg_type *msg_ptr,
    /* CDMA message in internal format */
  word new_state
    /* Next state to be processed */
)
{
  boolean continue_page_match = TRUE;
    /* Indicates if we are going to do page match */

  /* ---------------------------------------------------------------------
  ** Process page if one of the following conditions is true.
  ** 1. Always process pages when in Idle state and not in Update Overhead
  **    Information Substate.
  ** 2. Let a Page override a registration access when in Update Overhead
  **    Information Substate.
  ** 3. Let a Page delay a data burst transmission when in Update Overhead
  **    Information Substate. If SMS transmission is aborted as a result of
  **    page response, we may exit or go to do page response. In all cases,
  **    we will notify SMS as soon as we leave idle state. So we don't have
  **    to check it here.
  ** 4. Even though SA state may be IDLE, MS may be in AEHO awaiting
  **    SRCH report for an origination or page response, Consider that case
  **    as well.
  ** ---------------------------------------------------------------------- */

  if ( ( ( cdma.sa_tc_state == CDMA_IDLE ) 
         && ( ( ( cdma.ho_state != CDMA_AEHO ) 
                && !cdma.transceiver_request_pending 
              )
              || 
              ( ( cdma.ho_sa_tc_state != CDMA_ORIG )  /*lint !e641 */
                && ( cdma.ho_sa_tc_state != CDMA_PAGE_RES ) /*lint !e641 */
                #ifdef FEATURE_IS2000_REL_D
#error code not present
                #endif /* FEATURE_IS2000_REL_D */
              )
            ) 
       )
       || (cdma.sa_tc_state == CDMA_REG) || (cdma.sa_tc_state == CDMA_MSG) /*lint !e641 */
     )
  {
    #ifdef FEATURE_IS2000_REL_A_CC
    if ((msg_ptr->msg.gen.msg_type == CAI_UPM_FIRST_SEG_MSG) ||
        (msg_ptr->msg.gen.msg_type == CAI_UPM_MIDDLE_SEG_MSG) ||
        (msg_ptr->msg.gen.msg_type == CAI_UPM_FINAL_SEG_MSG))
    {
      /* Reassemble Universal Page Message from received segments */
      if (mcc_reassemble_seg_upm(msg_ptr, &mccdma_seg_univ_page))
      {
        /* Perform partial address match if address info is available */
        if ((mccdma_seg_univ_page.msg_field == CAII_UPM_ADDR_BLOCK) ||
            (mccdma_seg_univ_page.msg_field == CAII_UPM_DONE))
        {
          if (!mcc_match_upm_partial_addr(
              mccdma_seg_univ_page.page_block.length,
              mccdma_seg_univ_page.page_block.data,
              &mccdma_seg_univ_page.page_block.offset,
              &mccdma_seg_univ_page.interleaved_addr))
          {
            /* Partial address mismatch, no need to perform page match */

            mclog_report_event_seg_upm_addr_mismatch(MCLOG_PART_ADDR_MISMATCH, /*lint !e641 */
              mccdma_seg_univ_page.segment_seq);

            continue_page_match = FALSE;

            if (mccdma_seg_univ_page.msg_field > CAII_UPM_CONFIG_MSG_SEQ)
            {
              mcc_put_seq(mccdma_seg_univ_page.common.config_msg_seq, FALSE);
              mcc_update_acc_msg_seq(mccdma_seg_univ_page.common.acc_msg_seq);
            }

            /* Check whether there are more pages */
            if (mccidl_done_monitoring_slot(msg_ptr, NULL, NO_MATCH))
            {
              /* ------------------------------------------------------------
              ** We just got a slotted page message in the designated or next
              ** slot which informed us that no more pages are coming for us,
              ** so if or when we have all the overhead messages we can go to
              ** sleep. The sleep function will schedule broadcast msg
              ** wake-ups.
              ** ------------------------------------------------------------ */
              new_state = mccidl_init_sleep(msg_ptr->frm.frame, new_state);
            }

            /* Done with reassembly and address match, empty reassembly buffer */
            mcc_empty_seg_upm_buffer();
          }
          else if (msg_ptr->msg.gen.msg_type != CAI_UPM_FINAL_SEG_MSG)
          {
            /* Not all segements received, delay page match */
            continue_page_match = FALSE;
          }
        }
        else
        {
          /* Not received enough info to proceed partial address match or page
             match */
          continue_page_match = FALSE;
        }
      }
      else
      {
        /* Failure to reassemble segmented Universal Page Message */
        continue_page_match = FALSE;
      }
    }
    else if (msg_ptr->msg.gen.msg_type == CAI_UNIVERSAL_PAGE_MSG)
    {
      if (!mcc_match_upm_partial_addr(
          msg_ptr->msg.upm.page_block.length,
          msg_ptr->msg.upm.page_block.data,
          &msg_ptr->msg.upm.page_block.offset,
          &msg_ptr->msg.upm.interleaved_addr))
      {
        /* Partial address mismatch, no need to perform page match */
        continue_page_match = FALSE;

        mcc_put_seq(msg_ptr->msg.upm.common.config_msg_seq, FALSE);
        mcc_update_acc_msg_seq(msg_ptr->msg.upm.common.acc_msg_seq);

        /* Check whether there are more pages */
        if (mccidl_done_monitoring_slot(msg_ptr, NULL, NO_MATCH))
        {
          /* -----------------------------------------------------------------
          ** We just got a slotted page message in the designated or next slot
          ** which informed us that no more pages are coming for us, so if or
          ** when we have all the overhead messages we can go to sleep.
          ** The sleep function will schedule broadcast msg wake-ups
          ** ---------------------------------------------------------------- */
          new_state = mccidl_init_sleep(msg_ptr->frm.frame, new_state);
        }
      }
    }
    #endif /* FEATURE_IS2000_REL_A_CC */

    if (continue_page_match) //lint !e774
    {
      /* --------------------------------------------------------------------
      ** Process page if one of the following conditions is true.
      ** 1. Always process pages when in Idle state and not in Update
      **    Overhead Information Substate.
      ** 2. Let a Page override a registration access when in Update
      **    Overhead Information Substate.
      ** 3. Let a Page delay a data burst transmission when in Update
      **    Overhead Information Substate. If SMS transmission is aborted as
      **    a result of page response, we may exit or go to do page response.
      **    In all cases, we will notify SMS as soon as we leave idle state.
      **    So we don't have to check it here.
      ** -------------------------------------------------------------------- */
      new_state = mccidl_page_match(msg_ptr, new_state);
    }
  }
  else
  {
    /* -----------------------------------------------------
    ** Ignore pages if we are in Update Overhead Information
    ** State for any other reason, but still store the
    ** access parameter message sequence number.
    ** ----------------------------------------------------- */
    if (msg_ptr->msg.gen.msg_type == CAI_SLT_PAGE_MSG)
    {
      mcc_update_acc_msg_seq(msg_ptr->msg.slt_page.acc_msg_seq);
      mcc_put_seq(msg_ptr->msg.slt_page.config_msg_seq, FALSE);
    }
    else if (msg_ptr->msg.gen.msg_type == CAI_PAGE_MSG)
    {
      mcc_update_acc_msg_seq(msg_ptr->msg.page.acc_msg_seq);
      mcc_put_seq(msg_ptr->msg.page.config_msg_seq, FALSE);
    }
    else if (msg_ptr->msg.gen.msg_type == CAI_GENERAL_PAGE_MSG)
    {
      mcc_update_acc_msg_seq(msg_ptr->msg.gen_page.acc_msg_seq);
      mcc_put_seq(msg_ptr->msg.gen_page.config_msg_seq, FALSE);
    }
    #ifdef FEATURE_IS2000_REL_A_CC
    else if (msg_ptr->msg.gen.msg_type == CAI_UNIVERSAL_PAGE_MSG)
    {
      mcc_update_acc_msg_seq(msg_ptr->msg.upm.common.acc_msg_seq);
      mcc_put_seq(msg_ptr->msg.upm.common.config_msg_seq, FALSE);
    }
    else if ((msg_ptr->msg.gen.msg_type == CAI_UPM_FIRST_SEG_MSG) ||
             (msg_ptr->msg.gen.msg_type == CAI_UPM_MIDDLE_SEG_MSG) ||
             (msg_ptr->msg.gen.msg_type == CAI_UPM_FINAL_SEG_MSG))
    {
      /* Reassemble Universal Page Message from segments */
      if (mcc_reassemble_seg_upm(msg_ptr, &mccdma_seg_univ_page))
      {
        if (mccdma_seg_univ_page.msg_field > CAII_UPM_CONFIG_MSG_SEQ)
        {
          mcc_update_acc_msg_seq(mccdma_seg_univ_page.common.acc_msg_seq);
          mcc_put_seq(mccdma_seg_univ_page.common.config_msg_seq, FALSE);
        }
      }
    }
    #endif /* FEATURE_IS2000_REL_A_CC */
  }

  /* In Pseudo-Idle Mode, as long as MS receives OTA with config_msg_seq. MS
     shall goes back to sleep as soon as overheads are current. */
  if ( ( cdma.entry_ptr->entry == MCC_PSEUDO_IDLE )
       && !mccidl_sleep_ok )
  {
    if ( mccidl_assert_sleep_ok(msg_ptr->frm.frame) )
    {
      MSG_MED("Sleep ok for Pseudo-Idle", 0, 0, 0);

      /* Update sleep slot mask */
      mccidl_update_sleep_slot_mask(PAGE_SLOT);
    }
  }

  return (new_state);

} /* mccidl_proc_page() */

#ifdef FEATURE_BROADCAST_SMS
/*===========================================================================

FUNCTION MCCIDL_PROC_PENDING_BCAST_PAGE

DESCRIPTION
  This function processes a pending broadcast page in Idle State.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
word mccidl_proc_pending_bcast_page
(
  mccrxtx_msg_type *msg_ptr,
    /* CDMA message in internal format */
  word new_state
    /* Next state to be processed */
)
{
  mccdma_page_match_result_type page_match;
    /* Page match result */

  /* ---------------------------------------------------------------------
  ** Process page if one of the following conditions is true.
  ** 1. Always process pages when in Idle state and not in Update Overhead
  **    Information Substate.
  ** 2. Let a Page override a registration access when in Update Overhead
  **    Information Substate.
  ** 3. Let a Page delay a data burst transmission when in Update Overhead
  **    Information Substate. If SMS transmission is aborted as a result of
  **    page response, we may exit or go to do page response. In all cases,
  **    we will notify SMS as soon as we leave idle state. So we don't have
  **    to check it here.
  ** ---------------------------------------------------------------------- */

  if ((cdma.sa_tc_state == CDMA_IDLE) || (cdma.sa_tc_state == CDMA_REG) || /*lint !e641 */
      (cdma.sa_tc_state == CDMA_MSG)) /*lint !e641 */
  {
    MSG_MED("Process pending bcast page", 0, 0, 0);
    /* --------------------------------------------------------------------
    ** Process page if one of the following conditions is true.
    ** 1. Always process pages when in Idle state and not in Update
    **    Overhead Information Substate.
    ** 2. Let a Page override a registration access when in Update
    **    Overhead Information Substate.
    ** 3. Let a Page delay a data burst transmission when in Update
    **    Overhead Information Substate. If SMS transmission is aborted as
    **    a result of page response, we may exit or go to do page response.
    **    In all cases, we will notify SMS as soon as we leave idle state.
    **    So we don't have to check it here.
    ** -------------------------------------------------------------------- */
    page_match = mcc_parse_page(new_state, TRUE, msg_ptr, NULL);

    if (page_match == BROADCAST_MATCH)
    {
      /* ------------------------------------
      ** Increment the count PAG_3 or FCCCH_3
      ** ------------------------------------ */
      mcc_increment_pag_3_or_fccch_3();
    }

    /* Don't need layer 2 ack */
    mcc_layer2.valid_ack = FALSE;

    /* It is an old page, so we don't use it to decide channel monitoring
       requirement */
  }

  /* Clear pending broadcast page */
  bcast_pending = FALSE;

  return (new_state);

} /* mccidl_proc_pending_bcast_page() */
#endif /* FEATURE_BROADCAST_SMS */

/*===========================================================================

FUNCTION MCCIDL_OVHD_PROC_MSG

DESCRIPTION
  This function performs Order and Message Processing while in Update
  Overhead Information substate.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/

word mccidl_ovhd_proc_msg
(
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received order message */
  word new_state
    /* Next state to be processed */
)
{
  word i;
    /* Index through messages */
  boolean record_found = FALSE;
    /* Indicator of whether an order or channel assignment record
       directed to the mobile station was found */
  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to handset command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*******************
  * Process layer 2 *
  *******************/

  #ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    /* Validate to see if parser decrypted msg successfully */
    if (msg_ptr->parser_decryption_failure)
    {
      /* We don't support Ext. Encryption as yet, so this is a BS error and there
      is no point in staying on the system. The reason we use MCC_REG_REJECTED is to
      be consistent with the way we react when we receive a REJ. ACC. ORDER with
      the encryption fields set to an algorithm/procedure we don't support */

      /* --------------------------------------------------------
      ** Inform system determination of registration rejected and
      ** determines the next state to be processed based on the
      ** returned SS-Action.
      ** -------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_opr_reg_rej(NULL)))
      {
        new_state = CDMA_EXIT;
        mcc_exit.exit = MCC_REG_REJECTED;

        MSG_HIGH("Idle: Rxed Msg with Ext. Encrypt, exit to Sys Det", 0, 0, 0 );
        return (new_state);
      }
    }
  }
  #endif /* FEATURE_IS2000_REL_A */

  if (msg_ptr->msg.gen.msg_type == CAI_PC_ORD_MSG)
  {
    /* -----------------------------------------------------------
    ** Paging Channel Order Message --> search for an order record
    ** addressed to this mobile station
    ** ----------------------------------------------------------- */
    for (i=0; i < msg_ptr->msg.pc_ord.num_ords; i++)
    {
      if (mcc_comp_addr(&msg_ptr->msg.pc_ord.ords[i].gen.hdr,
          NULL /* this field is used for BCSMS only */
         ))
      {
        /* -----------------------------------------
        ** Found an order record addressed to mobile
        ** ----------------------------------------- */
        record_found = TRUE;
        break;
      }
    } /* end for */

    if (record_found &&
        !mcc_detect_duplicate(FALSE, &msg_ptr->msg.pc_ord.ords[i].gen.hdr))
    {
      /* -----------------------------------------------------------------
      ** Increment the count PAG_3 or FCCCH_3 unless we have a BS_ACK_ORD.
      ** ----------------------------------------------------------------- */
      if (msg_ptr->msg.pc_ord.ords[i].gen.order != CAI_BS_ACK_ORD)
      {
        mcc_increment_pag_3_or_fccch_3();
      }
      /* Log the matched msg */
      mclog_report_matched_msg(msg_ptr->msg.gen.msg_type, 
                               MCLOG_MSG_MATCH);

      /* -----------------------------------------------------------------
      ** The only order we process in Update Overhead Information Substate
      ** is the Lock Until Power Cycled Order.  We ignore any other order.
      ** ----------------------------------------------------------------- */

      if ((msg_ptr->msg.pc_ord.ords[i].gen.order == CAI_LOCK_OR_MAINT_ORD) &&
          (msg_ptr->msg.pc_ord.ords[i].ordq.ordq != CAI_UNLOCK_MASK) &&
          (msg_ptr->msg.pc_ord.ords[i].ordq.ordq & CAI_LUPC_MASK))
      {
        /* -----------------------------
        ** Lock Until Power-Cycled Order
        ** ----------------------------- */
        MSG_MED("Rxed Lock Order in OVHD", 0, 0, 0);

        /* --------------------------------------------------------------
        ** Inform system determination of lock.
        ** Determine the next state to be processed based on the returned
        ** SS-Action.
        ** -------------------------------------------------------------- */
        if (mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
            sd_ss_ind_cdma_opr_lock(NULL)))
        {
          /* No ack will be sent for LUPC */
          msg_ptr->msg.pc_ord.ords[i].gen.hdr.ack_req = FALSE;
        }
        else
        {
          new_state     = CDMA_EXIT;
          mcc_exit.exit = MCC_LOCK;
        }

        mcc_nv_data.lckrsn_p.lock_order = TRUE;
        mcc_nv_data.lckrsn_p.lock_reason =
          msg_ptr->msg.pc_ord.ords[i].ordq.ordq & 0x0F;

        /* Write reason for lock to NVRAM */
        mcc_nv_buf.item = NV_LCKRSN_P_I;
        mcc_nv_buf.cmd = NV_WRITE_F;
        mcc_nv_buf.data_ptr = &mcc_nv_data;

        (void) mcc_nv_cmd(&mcc_nv_buf);

        /* Send lock command to UI */
        if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cmd_ptr->hdr.cmd = CM_LOCK_F;
          mcc_cm_mc_rpt( cmd_ptr );
        }
        else
        {
          ERR("No free buffers on cm_mc_rpt_free_q",0,0,0 );
        }

        /* Events report: Lock */
        event_report(EVENT_LOCK);

      } /* end if (we have a lock order) */
    } /* end if (record_found) && (!mcc_detect_duplicate)) */
  } /* end if (msg_ptr->gen.msg_type == CAI_PC_ORD_MSG) */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  #ifdef FEATURE_BROADCAST_SMS
  else if( (msg_ptr->msg.gen.msg_type == CAI_PC_BURST_MSG) &&
           (msg_ptr->msg.gen_pc.hdr.addr_type == CAI_BCAST_ADDR_TYPE) &&
	   (cm_wms_bc_addr_match(msg_ptr->msg.gen_pc.hdr.addr.type5.bc_addr))
         )
  {
    MSG_MED("Ignoring the BCSMS in update overhead substate", 0, 0, 0);
    mcc_bcsms_in_progress = FALSE;
    qw_set(mcc_bcsms_start_slot, 0, 0);
  }
  #endif /* FEATURE_BROADCAST_SMS */

  /*  We ignore any other message */

  return (new_state);

} /* mccidl_ovhd_proc_msg */

/*===========================================================================

FUNCTION MCCIDL_PROC_PENDING_MSG

DESCRIPTION
  This function performs the processing for a message which was left pending
  in order to send an acknowledgement before processing it.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/

word mccidl_proc_pending_msg
(
  mccdma_matched_msg_type *matched_msg_ptr,
    /* Pointer to pending message */
  word curr_state
    /* Current state to be processed */
)
{
  word next_state = curr_state; /* Current state to be processed */

  if ((matched_msg_ptr->gen_pc.msg_type == CAI_PC_ORD_MSG) &&
      (matched_msg_ptr->pc_ord.order ==  CAI_REG_ORD) &&
     ((matched_msg_ptr->pc_ord.ordq == CAI_REG_REJ) ||
      (matched_msg_ptr->pc_ord.ordq == CAI_REG_REJ_DEL_TMSI)))
  {
    /* Registration rejected and TMSI delete   */
    /* so reset TMSI code and all TMSI timers. */
    if (matched_msg_ptr->pc_ord.ordq == CAI_REG_REJ_DEL_TMSI)
    {
      /* Reset TMSI code and all timers */
      tmsi_code_and_timer_reset ();
    }
    /* Registration rejected and no TMSI delete */
    /* so reset full TMSI timer.                */
    else
    {
      /* Reset full TMSI timer indicator and    */
      /* reset full TMSI timer and timer signal */
      tmsi_reset_full_timer ();
    }

    /* -------------------------------------------------------------------
    ** Inform system determination of registration rejected and determines
    ** whether the mobile shall continue on the current system.
    ** ------------------------------------------------------------------- */
    if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
        sd_ss_ind_cdma_opr_reg_rej(NULL)))
    {
      next_state = CDMA_EXIT;
      mcc_exit.exit = MCC_REG_REJECTED;
    }
  }
  else if (matched_msg_ptr->gen_pc.msg_type == CAI_SERVICE_REDIR_MSG)
  {
    /* ------------------------------------------------------------
    ** Redirection Record Type must equal 1 or 2 (see IS-95A table
    ** 7.7.2.3.2.16-1) and if type 2 the Band Class value must be 0
    ** (see TSB58 table 6.1-1). However, we do not check whether we
    ** are redirected to a supported band/system here.
    ** Instead, we will allow the system determination to decide what
    ** to do.
    ** ------------------------------------------------------------ */

    {
      /* ----------------------------------------------------------------
      ** The mobile station shall enter the System Determination Substate
      ** with a redirection indication.
      ** ---------------------------------------------------------------- */
      /* Reset TMSI timers and code if delete TMSI flag */
      /* has been set.                                  */
      if (matched_msg_ptr->serv_redir.delete_tmsi)
      {
        tmsi_code_and_timer_reset();
      }
      /* If the record type the redirection is an NDSS off indication
         exit to System Determination with MCC_NDSS_OFF */
      if (matched_msg_ptr->serv_redir.record_type == CAI_NDSS_OFF_INDICATION )
      {
        /* ------------------------------------------------------------
        ** Inform system determination of NDSS off.
        ** Determine the next state to be processed based on the
        ** returned SS-Acction.
        ** ------------------------------------------------------------ */
        if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
            sd_ss_ind_cdma_opr_ndss_off(NULL)))
        {
          next_state = CDMA_EXIT;
          mcc_exit.exit = MCC_NDSS_OFF;
        }
        /* Events report: NDSS off */
        event_report(EVENT_NDSS_OFF);
      }

      else /* else follow the Standard Redirection */
      {

        /* ------------------------------------------------------------
        ** Inform system determination of done with accept redirection.
        ** Determine the next state to be processed based on the
        ** returned SS-Action.
        ** ------------------------------------------------------------ */
        if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_opr_acpt_rej(NULL)))
        {
          next_state = CDMA_EXIT;
          mcc_exit.exit = MCC_REDIRECTION;
        }

        /* Events report: Redirection */
        event_report(EVENT_REDIRECTION);
      }
    }

  }
  else if ((matched_msg_ptr->gen_pc.msg_type == CAI_CHN_ASN_MSG
           || matched_msg_ptr->gen_pc.msg_type == CAI_EXT_CHN_ASN_MSG
           ) &&
           ((matched_msg_ptr->chnasn.assign_mode == CAI_PAGING_CHAN_ASSIGN) ||
            (matched_msg_ptr->chnasn.assign_mode == CAI_EXT_PAGING_CHAN_ASSIGN)))
  {
    MSG_MED("Pending Chan Assign '001' or '101'", 0, 0, 0);
    /* ------------------------------------------------------
    ** Inform system determination of CAM/ECAM to CDMA paging
    ** channel assignment and determines the next state to be
    ** processed based on the returned SS-Action.
    ** ------------------------------------------------------ */
    if (mcc_inform_sd_chan_assign(&matched_msg_ptr->chnasn, MEAS_BACK_ALLOWED))
    {
      mcc_pc_assign(next_state, &matched_msg_ptr->chnasn);
      /* -----------------------------
      ** Reset Idle State to CDMA_INIT
      ** ----------------------------- */
      next_state = CDMA_IDLE_INIT; /*lint !e641 */

      /* Start BS insanity timer */
      mccidl_set_bs_insanity_timer();
    }
  }

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  else if ((matched_msg_ptr->gen_pc.msg_type == CAI_PC_ORD_MSG) &&
           (matched_msg_ptr->pc_ord.order == CAI_RETRY_ORD))
  {
    update_retry_delay((cai_retry_order_type) matched_msg_ptr->pc_ord.retry_type, 
      matched_msg_ptr->pc_ord.retry_delay);
  }

  return (next_state);

} /* mccidl_proc_pending_msg */

/*===========================================================================

FUNCTION MCCIDL_PROC_MSG

DESCRIPTION
  This function performs the IS-95 Mobile Station Order and Message
  Processing operation.  During this operation all messages besides Page
  and overhead messages are processed.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/

word mccidl_proc_msg
(
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received order message */
  word curr_state
    /* Current Idle substate */
)
{
  word new_state = curr_state;
    /* Next state to be processed */

  #ifdef FEATURE_AUTH
  dword randbs;
    /* Random number used for authentication during SSD update */
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  #ifdef FEATURE_UIM_RUIM
  auth_rpt_type *auth_rpt_ptr;
    /* Pointer to report from authentication task */
  #endif /* FEATURE_UIM_RUIM */
  #endif /* FEATURE_AUTH */

  mcc_redir_info_type redir_info; /* Redirection info */
  sd_redir_rec_s_type curr_redir_rec;
  uint8 chan_index;

  #ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
  #endif /* FEATURE_IS2000_REL_C_AKA */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /***************************************
   * Check to see if the phone is locked *
   ***************************************/

  if (sd_misc_is_cdma_locked())
  {
    /* Phone is locked --> search for Unlock Order */
    new_state = mccidl_lock_mob_proc(&msg_ptr->msg, curr_state);

    return (new_state);
  }

  /******************************
   * Check message for correct  *
   * address and for duplicate. *
   ******************************/

  /* ----------------------------------------------------------
  ** Check if the address of the message matches.
  ** If it does not, stop processing the message.
  ** Otherwise it is copied into mccidl_matched_msg.
  ** ---------------------------------------------------------- */
  if (!mcc_match_msg(msg_ptr, &mccidl_matched_msg, TRUE, new_state))
  {
    return (curr_state);
  }

  /*******************
  * Process layer 2 *
  *******************/

  #ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    /* Validate to see if parser decrypted msg successfully */
    if (msg_ptr->parser_decryption_failure)
    {
      /* We don't support Ext. Encryption as yet, so this is a BS error and there
      is no point in staying on the system. The reason we use MCC_REG_REJECTED is to
      be consistent with the way we react when we receive a REJ. ACC. ORDER with
      the encryption fields set to an algorithm/procedure we don't support */

      /* --------------------------------------------------------
      ** Inform system determination of registration rejected and
      ** determines the next state to be processed based on the
      ** returned SS-Action.
      ** -------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_opr_reg_rej(NULL)))
      {
        new_state = CDMA_EXIT;
        mcc_exit.exit = MCC_REG_REJECTED;

        MSG_HIGH("Idle: Rxed Msg with Ext. Encrypt, exit to Sys Det", 0, 0, 0 );
        return (new_state);
      }
    }
  }
  #endif /* FEATURE_IS2000_REL_A */

  if ((mccidl_matched_msg.gen_pc.hdr.addr_type != CAI_BCAST_ADDR_TYPE) &&
      (mccidl_matched_msg.gen_pc.hdr.ack_req))
  {
    /* ------------------------------------------
    ** If broadcast message, no need to ACK.
    ** There was an ack_required, so save Layer 2
    ** field values for acknowledgement message.
    ** ------------------------------------------ */
    mcc_set_layer2_ack(mccidl_matched_msg.gen_pc.hdr.msg_seq,
      mccidl_matched_msg.gen_pc.hdr.addr_type, TRUE);
  }
  else
  {
    /* ---------------------------------------------
    ** There was no ack required, so make sure if we
    ** send a layer 3 response or reject message,
    ** valid_ack is set to FALSE in the message.
    ** --------------------------------------------- */
    mcc_layer2.valid_ack = FALSE;
  }

  /* -------------------------------------------------------
  ** Check if this is a duplicate message.  If it is, do not
  ** repeat the layer 3 processing we have already done.
  ** ------------------------------------------------------- */

  if (!mcc_detect_duplicate(FALSE, &mccidl_matched_msg.gen_pc.hdr))
  {
    /* --------------------------------------
    ** Increment the counter PAG_3 or FCCCH_3
    ** -------------------------------------- */

    if ((mccidl_matched_msg.gen_pc.msg_type != CAI_PC_ORD_MSG) ||
        (mccidl_matched_msg.pc_ord.order != CAI_BS_ACK_ORD))
    {
      /* ---------------------------------------------------------
      ** As long as this was not a BS_ACK_ORD we need to increment
      ** the count PAG_3 or FCCCH_3.
      ** --------------------------------------------------------- */
      mcc_increment_pag_3_or_fccch_3();
    }

    /* Log the matched msg */
    mclog_report_matched_msg(mccidl_matched_msg.gen_pc.msg_type,
                             MCLOG_MSG_MATCH);

    /*******************
     * Process layer 3 *
     *******************/

    switch (mccidl_matched_msg.gen_pc.msg_type)
    {
      case CAI_PC_ORD_MSG:
        new_state = mccidl_proc_ord_msg(&mccidl_matched_msg, curr_state);
        break;

      case CAI_CHN_ASN_MSG:
      case CAI_EXT_CHN_ASN_MSG:
        #ifdef FEATURE_IS2000_REL_B
        if (P_REV_IS_8_OR_GREATER &&
            (mccidl_matched_msg.gen_pc.msg_type == CAI_CHN_ASN_MSG))
        {
          /* Obsolete CAM for Release B and onwards */
          MSG_HIGH("Reject CAM, P_REV in use >= 8", 0, 0, 0);
          mcc_setup_reject_ord(CAI_REJ_CODE, CAI_CHN_ASN_MSG, 0, 0);
          new_state = CDMA_ORD_RES;  /*lint !e641 */
        }
        else
        #endif /* FEATURE_IS2000_REL_B */

        if (!P_REV_IS_4_OR_GREATER &&
            (mccidl_matched_msg.gen_pc.msg_type == CAI_EXT_CHN_ASN_MSG))
        {
          /* ECAM is for IS95B and onwards */
          MSG_HIGH("Reject ECAM, P_REV in use < 4", 0, 0, 0);
          mcc_setup_reject_ord(CAI_REJ_CODE, CAI_EXT_CHN_ASN_MSG, 0, 0);
          new_state = CDMA_ORD_RES; /*lint !e641 */
        }
        else
        {
          new_state = mccidl_proc_chn_asn(&mccidl_matched_msg, curr_state);
        }
        break;

      case CAI_PC_TMSI_ASGN_MSG:
        new_state = mccidl_proc_tmsi_assign(&mccidl_matched_msg, new_state);
        break;

      case CAI_PC_BURST_MSG:
        new_state = mccidl_proc_data_burst(&mccidl_matched_msg, curr_state);
        break;

      case CAI_PC_STREQ_MSG:
        new_state = mccidl_proc_streq(&mccidl_matched_msg, curr_state);
        break;

      case CAI_FEATURE_MSG:
        /* ----------------------------
        ** Feature Notification Message
        ** ---------------------------- */
        MSG_HIGH( "Rxed Feature Notification Msg", 0, 0, 0 );
        (void) mcc_process_info_records( CM_CALL_ID_UNASSIGNED,
                                         mccidl_matched_msg.pc_feat.msg_type,
                                         mccidl_matched_msg.pc_feat.num_recs,
                                         (byte*)mccidl_matched_msg.pc_feat.recs );
        break;

      case CAI_PC_AUTH_MSG:
        /* ----------------------
        ** Auth Challenge Message
        ** ---------------------- */
        MSG_HIGH("Rxed Auth Challenge Message", 0, 0, 0);

        new_state = CDMA_ORD_RES; /*lint !e641 */

        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */

        #ifdef FEATURE_AUTH
        cdma.sa_resp_msg = CAI_AC_AUTH_RESP_MSG;
        cdma.randu = mccidl_matched_msg.pc_auth.randu;

        #if ((defined FEATURE_IS2000_REL_C_AKA) && (defined FEATURE_IS2000_REL_D))
#error code not present
        #endif /* FEATURE_IS2000_REL_C_AKA && FEATURE_IS2000_REL_D */

        #else
        mcc_setup_reject_ord(CAI_REJ_CAP, CAI_PC_AUTH_MSG, 0, 0);
        #endif /* FEATURE_AUTH */        
    
        break;

      case CAI_PC_SSD_UP_MSG:
        /* ------------------
        ** SSD Update Message
        ** ------------------ */
        MSG_HIGH("Rxed SSD Update Message", 0, 0, 0);

        #ifdef FEATURE_AUTH
        /* Get a random number for the Base Station Challange. */
        randbs = ran_next();

        #ifdef FEATURE_UIM_RUIM
        #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        /* Determine if the R-UIM is available for this NAM */
        if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
        #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
        {
          /* Get an auth command buffer. */
          if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
          {
            /* Send the BS challenge command.  */
            auth_cmd_ptr->hdr.command = AUTH_BS_CHAL_F;
            auth_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
            /* Use the random number as the seed*/
            auth_cmd_ptr->chal.randseed = randbs;
            /* Use the Auth reportback function. */
            auth_cmd_ptr->chal.rpt_function = mcc_queue_auth_rpt;

            /* Using the Auth queue signal */
            (void) rex_clr_sigs(&mc_tcb, MCC_AUTH_Q_SIG);

            /* Send the command and wait for the initial status */
            mcc_auth_cmd( auth_cmd_ptr );

            /* Check the initial response from auth */
            if ( auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S )
            {
              /* Wait for the command to be done */
              /* Note that we are waiting on MCC_AUTH_Q_SIG */
              (void) mcc_wait( MCC_AUTH_Q_SIG );

              /* Retrieve the report from the auth report queue */
              auth_rpt_ptr = (auth_rpt_type*) q_get( &mc_auth_q );

              /* Did the command compete without any problems. */
              if (auth_rpt_ptr->rpt_status == UIM_PASS)
              {
                /* Process the RANDBS result */
                randbs = auth_rpt_ptr->rpt.randbs;
              }
              else /* report was invalid */
              {
                ERR( "Failure Report from AUTH!", 0, 0, 0 );
                /* Use zero as randbs.  This should fail the SSD update. */
                randbs = 0;
              } /* end if - check the status of the AUTH command. */

              /* Place the AUTH report buffer back on the free queue. */
              if (auth_rpt_ptr != NULL)
              {
                cmd_done( &auth_rpt_ptr->rpt_hdr );
              }
            }
            else /* Status returned from command was an error condition */
            {
              ERR( "Bad status from AUTH %d", auth_cmd_ptr->hdr.status, 0, 0 );
              /* Use zero as randbs.  This should fail the SSD update. */
              randbs = 0;
            } /* end if - the BS challenge command was received by auth */
          }
          else /* Unable to get an auth command buffer from the free queue. */
          {
            ERR( "No free buffers on auth_free_q", 0, 0, 0 );
            /* Use zero as randbs.  This should fail the SSD update. */
            randbs = 0;
          } /* Got a AUTH cmd buffer from the free queue. */
        }
        #endif /* FEATURE_UIM_RUIM */

        /* Get an auth command buffer. */
        if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
        {

          #ifdef FEATURE_UIM_RUIM
          #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          /* Determine if the R-UIM is available for this NAM */
          if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
          #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
          {
            auth_cmd_ptr->hdr.command = AUTH_SSD_UPDATE_F;
            qw_equ( auth_cmd_ptr->ssd.randssd,
                    mccidl_matched_msg.pc_ssd.randssd );
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
            qw_equ( auth_cmd_ptr->ssd.randssd, mccidl_matched_msg.pc_ssd.randssd );
          }
          #endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

          mcc_auth_cmd(auth_cmd_ptr);
          if (auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S)
          {
             cdma.sa_resp_msg = CAI_AC_ORD_MSG;
             cdma.sa_resp_ord.chal.order = CAI_BS_CHAL_ORD;
             cdma.sa_resp_ord.chal.ordq = 0;
             cdma.sa_resp_ord.chal.randbs = randbs;
          }
          else
          {
             ERR("Bad status from AUTH %d", auth_cmd_ptr->hdr.status, 0, 0);
             mcc_setup_reject_ord(CAI_REJ_UNSP, CAI_PC_SSD_UP_MSG, 0, 0);
          }
        }
        else /* Unable to get a Auth cmd buffer from the free queue. */
        {
          ERR("No free buffers on auth_free_q", 0, 0, 0);
        } /* get an auth cmd buffer from the free queue. */
        #else
        mcc_setup_reject_ord( CAI_REJ_CAP, CAI_PC_SSD_UP_MSG, 0, 0 );
        #endif /* FEATURE_AUTH */

        new_state = CDMA_ORD_RES; /*lint !e641 */
        break;

      case CAI_SERVICE_REDIR_MSG:

        /* ----------------------------
        ** Service Redirection Message
        ** ---------------------------- */

        /* ---------------------------------------------------
        ** If the MS is redirected to an unsupported mode or  
        ** band class, MS will resond back with a Reject Order 
        ** In this case, idl_msg_pending flag will not be set.
        ** --------------------------------------------------- */
        if ( !mcc_check_if_csch_srdm_is_okay( &mccidl_matched_msg.serv_redir ) )
        {
          mcc_setup_reject_ord( CAI_REJ_CAP, CAI_SERVICE_REDIR_MSG, 0, 0 );
          new_state = CDMA_ORD_RES; /*lint !e641 */
        }
        else
        {
          curr_redir_rec.record_type = 
            (sd_redir_rec_type_e_type)mccidl_matched_msg.serv_redir.record_type;
          curr_redir_rec.is_delete_tmsi = mccidl_matched_msg.serv_redir.delete_tmsi;

          if (curr_redir_rec.record_type == CAI_CDMA_REDIRECTION_REC)  /*lint !e641 */
          {
            curr_redir_rec.rec.cdma_rec.band_class = 
              (sys_band_class_e_type)mccidl_matched_msg.serv_redir.redir.rec2.band_class;
            curr_redir_rec.rec.cdma_rec.expected_sid = 
              mccidl_matched_msg.serv_redir.redir.rec2.expected_sid;
            curr_redir_rec.rec.cdma_rec.expected_nid = 
              mccidl_matched_msg.serv_redir.redir.rec2.expected_nid;
            curr_redir_rec.rec.cdma_rec.num_chans = 
              mccidl_matched_msg.serv_redir.redir.rec2.num_chans;
            for (chan_index = 0; chan_index < mccidl_matched_msg.serv_redir.redir.rec2.num_chans; 
                 chan_index++) 
            {
              curr_redir_rec.rec.cdma_rec.chan[chan_index] = 
                mccidl_matched_msg.serv_redir.redir.rec2.cdma_chan[chan_index];
            }
          }

          else if (curr_redir_rec.record_type == CAI_ANALOG_REDIRECTION_REC) /*lint !e641 */
          {
            curr_redir_rec.rec.amps_rec.expected_sid = 
              mccidl_matched_msg.serv_redir.redir.rec1.expected_sid;
            curr_redir_rec.rec.amps_rec.is_ignore_cdma =
              mccidl_matched_msg.serv_redir.redir.rec1.ignore_cdma;
            curr_redir_rec.rec.amps_rec.sys_ord =
              (sd_ss_sys_ord_e_type)mccidl_matched_msg.serv_redir.redir.rec1.sys_ordering;
            curr_redir_rec.rec.amps_rec.max_redirect_delay = 0;
          }

          redir_info.return_if_fail = mccidl_matched_msg.serv_redir.return_if_fail;
          redir_info.num_recs = 1;
          redir_info.redir_rec_list = &curr_redir_rec;

          /* ------------------------------------------------
          ** Inform system determination of NDSS redirection.
          ** ------------------------------------------------ */
          if ((mccidl_matched_msg.serv_redir.redirect_type ==
               CAI_NDSS_REDIRECTION) &&
              (mccidl_matched_msg.serv_redir.record_type !=
               CAI_NDSS_OFF_INDICATION) &&
              (!mcc_inform_sd_redirection(SD_SS_REDIR_CDMA_NDSS, &redir_info)))
          {
            mcc_setup_reject_ord( CAI_REJ_CAP, CAI_SERVICE_REDIR_MSG, 0, 0 );
            new_state = CDMA_ORD_RES; /*lint !e641 */
          }
          else if ((mccidl_matched_msg.serv_redir.redirect_type == CAI_NORMAL_REDIRECTION) &&
                   (!mcc_inform_sd_redirection(SD_SS_REDIR_CDMA_SRDM, &redir_info)))
          {
            mcc_setup_reject_ord( CAI_REJ_CAP, CAI_SERVICE_REDIR_MSG, 0, 0 );
            new_state = CDMA_ORD_RES; /*lint !e641 */
          }
          else
          {
            /* This message is not rejected so let's process it */
            if (!mccidl_matched_msg.serv_redir.hdr.ack_req)
            {
              /* If the record type the redirection is an NDSS off indication
              exit to System Determination with MCC_NDSS_OFF */
              if (mccidl_matched_msg.serv_redir.record_type == CAI_NDSS_OFF_INDICATION )
              {
                /* Reset TMSI timers and code if delete TMSI flag */
                /* has been set.                                  */
                if (mccidl_matched_msg.serv_redir.delete_tmsi)
                {
                  tmsi_code_and_timer_reset();
                }

                /* ----------------------------------------------------------
                ** Inform system determination of NDSS off and determines the
                ** next state to be processed based on returned SS-Action.
                ** ---------------------------------------------------------*/
                if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
                    sd_ss_ind_cdma_opr_ndss_off(NULL)))
                {
                  new_state = CDMA_EXIT;
                  mcc_exit.exit = MCC_NDSS_OFF;
                }

                /* Events report: NDSS off */
                event_report(EVENT_NDSS_OFF);
              }
              else /* else follow the Standard Redirection */
              {

                /* ---------------------------------------------------------
                ** Inform system determination done with accept redirection.
                ** Determine the next state to be processed based on the
                ** returned SS-Action.
                ** ---------------------------------------------------------*/
                if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
                    sd_ss_ind_cdma_opr_acpt_rej(NULL)))
                {
                  new_state = CDMA_EXIT;
                  mcc_exit.exit = MCC_REDIRECTION;
                }

                /* Events report: Redirection */
                event_report(EVENT_REDIRECTION);
              }
            }
            else
            {
              /* Leave the service redirection pending while we send an ack */
              mcc_layer2.idle_msg_pending = TRUE;
              mcc_layer2.pending_idle_msg = mccidl_matched_msg;
            }
          }
        } /* If message rejected */

        break;

      case CAI_PACA_MSG:
        /* ------------------
        ** PACA Message
        ** ------------------ */
        MSG_HIGH ("Rxed PACA Message", 0, 0, 0);

        #ifdef FEATURE_IS95B_PACA
        /* PACA Message processing would occur here */
        MSG_MED ("PACA Not Currently Supported", 0, 0, 0);
        #else
        mcc_setup_reject_ord(CAI_REJ_CAP, CAI_PACA_MSG, 0, 0);
        new_state = CDMA_ORD_RES; /*lint !e641 */
        #endif /* FEATURE_IS95B_PACA */

        break;

      #ifdef FEATURE_IS2000_REL_A
      case CAI_SEC_MODE_CMD_MSG:
        /* -------------------------------
        ** SECURITY MODE COMMAND MESSAGE
        ** ------------------------------- */
        MSG_HIGH ("Rxed Security Mode Command Message", 0, 0, 0);

        #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
        #else
        if (mccidl_matched_msg.sec_mode_cmd.c_sig_encrypt_mode != CAI_SIG_ENC_MODE_DISABLED)
        {
          /* Signaling Encryption Not supported for now */
          MSG_HIGH("invalid c_sig_encrypt_mode %d", mccidl_matched_msg.sec_mode_cmd.c_sig_encrypt_mode, 0, 0);
          mcc_setup_reject_ord(CAI_REJ_CAP, CAI_SEC_MODE_CMD_MSG, 0, 0);
          new_state = CDMA_ORD_RES; /*lint !e641 */
        }
        #endif /* FEATURE_IS2000_REL_A_AES */

        #if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
        #endif /* FEEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */
        break;
      #endif /* FEATURE_IS2000_REL_A */

      #ifdef FEATURE_IS2000_REL_C
      case CAI_AUTH_REQ_MSG:

        /* Authentication Request Message, for AKA */
        MSG_HIGH ( "Received Auth Request Message", 0, 0, 0 );

        #ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
        #endif /* FEATUR_IS2000_REL_C_AKA */

        if (mccidl_matched_msg.sec_mode_cmd.msg_int_info_incl && 
            !mccap_is_msg_integrity_supported())
        {
          /* Send a Mobile Station Reject Order */
          mcc_setup_reject_ord ( CAI_REJ_CAP, CAI_AUTH_REQ_MSG, 0, 0 );
          new_state = CDMA_ORD_RES; /*lint !e641 */

          MSG_ERROR ( "AKA can't be performed", 0, 0, 0 );
        }

        break;
      #endif /* FEATURE_IS2000_REL_C */

      default:
        /* ------------------------------------
        ** According to IS-95A section 6.6.2.4,
        ** we ignore any other message.
        ** ------------------------------------ */
        break;

    } /* end switch (message type) */
  } /* end if (not a duplicate) */

  /************************
   * Send Acknowledgement *
   * if still required.   *
   ************************/

  if ((MAIN_STATE( new_state ) == CDMA_IDLE) &&
      mccidl_matched_msg.gen_pc.hdr.ack_req)
  {

    #ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
    #endif /* FEATURE_GPSONE_NEW_L2_ACK */
    {
      /* -------------------------------------------------
      ** The message requires an acknowledgement and we
      ** have not decided to send a layer 3 response or a
      ** reject message, so we will send a Mobile Station
      ** Acknowledgement Order to acknowledge the message.
      ** ------------------------------------------------- */
      new_state                  = CDMA_ORD_RES; /*lint !e641 */
      cdma.sa_resp_msg           = CAI_AC_ORD_MSG;
      cdma.sa_resp_ord.gen.order = CAI_MS_ACK_ORD;
    }
  }

  /*********************************************************************
   ** Check Overhead messages and save channel if going to System Access
   ** or Traffic Channel.                                                
   **********************************************************************/

  if ((MAIN_STATE(new_state) == CDMA_SYS_ACC)
      #ifdef FEATURE_IS2000_REL_D
#error code not present
      #endif /* FEATURE_IS2000_REL_D */
     )
  {
    /* Remember what channel the order or message was received on */
    mccidl_save_msg_rxed_chan();

    #ifdef FEATURE_IS2000_REL_D
#error code not present
    #endif /* FEATURE_IS2000_REL_D */

    {
      /* Send transceiver request update request: We just need to send out the */
      /* L2 ack on the Access channel, so use the low_power access    */
      /* where the RX diversity is not enabled.                       */
      mcc_request_transceiver_resource(SRCH_LOW_POWER_ACCESS, SRCH_ACCESS_MOBILE_ORIG_TYPE, 
        new_state);
    }

    return curr_state;
  }

  return new_state;

} /* mccidl_proc_msg */

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

/*===========================================================================

FUNCTION MCCIDL_MSG_HANDLER

DESCRIPTION
  This function processes a message received from the Receive task.  Based
  on the message type it will call the appropriate procedure to process the
  message.

DEPENDENCIES
  Input message is in internal format.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  If the header within the input caii_rx_msg_type requests that the buffer
  be placed on a queue after processing is completed this procedure will
  do so.

===========================================================================*/

word mccidl_msg_handler
(
  mccrxtx_msg_type *msg_ptr,
    /* CDMA message in internal format */
  word new_state
    /* Next state to be processed */
)
{
  boolean valid_msg = TRUE;
    /* Assume that message will be from right channel and right pilot_pn */
  long int extended_lat, extended_long;
    /* Latitude and longitude of base station, sign extended. */
  boolean valid_ovhd_msg = FALSE;
    /* Indicate if valid overhead message is received */
  boolean sync_channel_monitor = FALSE;
    /* Indicate if channel monitoring re-synchronization is needed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH &&
      (msg_ptr->msg.gen.msg_type != CAI_UPM_FIRST_SEG_MSG) &&
      (msg_ptr->msg.gen.msg_type != CAI_UPM_MIDDLE_SEG_MSG) &&
      (msg_ptr->msg.gen.msg_type != CAI_UPM_FINAL_SEG_MSG))
  {
    /* ----------------------------------------------------------------------
    ** Empty segmented Universal Page Message reassembly buffer if received
    ** message is not a UPM First Segment Message, UPM Middle Segment Message
    ** and UPM Final Segment.
    ** ---------------------------------------------------------------------- */
    mcc_empty_seg_upm_buffer();
  }
  #endif /* FEATURE_IS2000_REL_A_CC */

  if ((OVHD_CHAN_IS_PCH && (msg_ptr->chn != RXC_CDMA_PC)) /*lint !e641 */
      #ifdef FEATURE_IS2000_REL_A_CC
      || (OVHD_CHAN_IS_BCCH && (msg_ptr->chn != RXC_CDMA_BCCH) && /*lint !e641 */
          (msg_ptr->chn != RXC_CDMA_FCCCH)) /*lint !e641 */
      #endif /* FEATURE_IS2000_REL_A_CC */
     )
  {
    /* Flag bad message with error call */
    MSG_MED( "Message received on wrong chan %d", msg_ptr->chn, 0, 0 );
    return (new_state);
  }

  /* Process Paging Channel message */
  switch (msg_ptr->msg.gen.msg_type)
  {
    case CAI_ACC_PARM_MSG:
      /* -------------------------
      ** Access Parameters Message
      ** ------------------------- */
      if (msg_ptr->msg.accparm.pilot_pn != cur_bs_ptr->pilot_pn)
      {
        /* Message is not from current pilot_pn */
        valid_msg = FALSE;
      }
      else
      {
        valid_ovhd_msg = TRUE;
        if (!cur_bs_ptr->rx.ap_rx ||
            (cur_bs_ptr->ap.acc_msg_seq !=
              msg_ptr->msg.accparm.acc_msg_seq))
        {

          /* Check for range violations */
          if ((msg_ptr->msg.accparm.probe_pn_ran > 9) ||
              (msg_ptr->msg.accparm.max_req_seq == 0) ||
              (msg_ptr->msg.accparm.max_rsp_seq == 0))
          {
            MSG_ERROR("Ignore Acc Params Msg - Range Violation", 0, 0, 0);
          }
          else
          {
            /* Put message into base station information array */
            mcc_put_msg(CAI_ACC_PARM_MSG, &msg_ptr->msg);

            new_state = mccidl_proc_acc_parm(new_state);
          }
        }
        else
        {
          mcc_put_seq(msg_ptr->msg.accparm.acc_msg_seq, TRUE);
        }
      }
      break;

    case CAI_SYS_PARM_MSG:
      /* -------------------------
      ** System Parameters Message
      ** ------------------------- */
      if (msg_ptr->msg.sysparm.pilot_pn != cur_bs_ptr->pilot_pn)
      {
        /* Message is not from current pilot_pn */
        valid_msg = FALSE;
      }
      else
      {
        valid_ovhd_msg = TRUE;
        mcc_put_seq(msg_ptr->msg.sysparm.config_msg_seq, FALSE);
      }

      if (valid_msg)
      {
        if ((!cur_bs_ptr->rx.sp_rx) ||
            (cur_bs_ptr->csp.sp.config_msg_seq !=
             msg_ptr->msg.sysparm.config_msg_seq))
        {

          /* -----------------------------------------
          ** Check for range violations.  If any field
          ** is out of range ignore the message.  See
          ** IS-95A sections 6.6.2.2.1 and 7.7.2.3.2.1
          ** ----------------------------------------- */

          extended_lat = EXTEND22( msg_ptr->msg.sysparm.base_lat ); //lint !e737
          extended_long = EXTEND23( msg_ptr->msg.sysparm.base_long ); //lint !e737

          if ((msg_ptr->msg.sysparm.page_chan == 0) ||
              ((msg_ptr->msg.sysparm.reg_prd != 0) &&
               ((msg_ptr->msg.sysparm.reg_prd < 29) ||
                (msg_ptr->msg.sysparm.reg_prd > 85))) ||
              (extended_lat < -1296000L) || (extended_lat > 1296000L) ||
              (extended_long < -2592000L) || (extended_long > 2592000L) ||
              ((msg_ptr->msg.sysparm.pwr_thresh_enable) &&
               (msg_ptr->msg.sysparm.pwr_rep_thresh == 0)))
          {
            MSG_ERROR("Ignore Sys Params Msg - Range Violation", 0, 0, 0);
          }
          else
          {
            /* Increment the ovhd cycles count */
            if(num_ovhd_cycles <= 1)
            {
              num_ovhd_cycles++;
            }

            /* ----------------------------------------------------
            ** Place message in base station information array.
            ** Message must be put in array before it is processed.
            ** ---------------------------------------------------- */
            mcc_put_msg( CAI_SYS_PARM_MSG, &msg_ptr->msg );
            new_state = mccidl_proc_sys_parm(new_state);

            #ifdef FEATURE_BROADCAST_SMS
            if ((new_state != CDMA_EXIT) &&
                mccbcsms_is_bcast_init() &&
                bcast_pending)
            {
              /* the ext sys msg initialized the bc_index and there's a pending
               * GPM with bc page rec pending, so process it now */
              new_state = mccidl_proc_pending_bcast_page(&bcast_page, new_state);
            }
            #endif /*  FEATURE_BROADCAST_SMS */
          }
        }
      }
      break;

    case CAI_EXT_SYS_PARM_MSG:
      /* ----------------------------------
      ** Extended System Parameters Message
      ** ---------------------------------- */
      if (msg_ptr->msg.ext_sysparm.pilot_pn != cur_bs_ptr->pilot_pn)
      {
        /* Message is not from current pilot_pn */
        valid_msg = FALSE;
      }
      else
      {
        valid_ovhd_msg = TRUE;
        mcc_put_seq(msg_ptr->msg.ext_sysparm.config_msg_seq, FALSE);

        if ((!cur_bs_ptr->rx.esp_rx) ||
            (cur_bs_ptr->csp.esp.config_msg_seq !=
             msg_ptr->msg.ext_sysparm.config_msg_seq))
        {

          // Before storing the ESPM, since P_REV 3 or lower ESPM does not contain the
          // P_REV fields, let's try to initialize them properly here.
          if (msg_ptr->msg.ext_sysparm.p_rev == CAIX_P_REV_UNKNOWN)
          {
            // This is a P_REV 3 or less ESPM
            if (cur_bs_ptr->rx.sy_rx)
            {
              // If Sync Channel Message was received, we can just copy from it
              msg_ptr->msg.ext_sysparm.p_rev = cur_bs_ptr->sy.p_rev;
              msg_ptr->msg.ext_sysparm.min_p_rev = cur_bs_ptr->sy.min_p_rev;
            }
            else
            {
              // If Sync Channel Message not received for this BS (must be idle handoff),
              // derive the P_REV from the band class.  Note that if we get an invalid
              // band class, the P_REV in the message remains 0.
              switch (mcc_get_group_for_band_class(cdma.band_class))
              {
                case MCC_CELLULAR_GROUP:
                {
                  msg_ptr->msg.ext_sysparm.p_rev =
                    msg_ptr->msg.ext_sysparm.min_p_rev = P_REV_IS95A;
                  break;

                }
                case MCC_PCS_GROUP:
                {
                  msg_ptr->msg.ext_sysparm.p_rev =
                    msg_ptr->msg.ext_sysparm.min_p_rev = P_REV_JSTD008;
                  break;
                }
                default:
                {
                  ERR("Band Class Error, P_REV not updated", 0,0,0);
                }
              }              
              /*lint +e506 +e774 */
            }
          }

          /* Place message in base station information array */
          mcc_put_msg( CAI_EXT_SYS_PARM_MSG, &msg_ptr->msg );

          /* The following calls to ...process_when_idle() are outside of
             mccidl_proc_ext_sys_parm to avoid updating PZID twice during a handoff
             - once by mcc_entry_init then again by ...process_when_idle. */

          new_state = mccidl_proc_ext_sys_parm(new_state);

          #ifdef FEATURE_BROADCAST_SMS
          if ((new_state != CDMA_EXIT) &&
              mccbcsms_is_bcast_init()
              && bcast_pending)
          {
            /* the ext sys msg initialized the bc_index and there's a pending
             * GPM with bc page rec pending, so process it now */
            new_state = mccidl_proc_pending_bcast_page(&bcast_page, new_state);
          }
          #endif /*  FEATURE_BROADCAST_SMS */

          /* ------------------------------------------------------------
          ** According to TSB58 for IS-95A, these values signify that the
          ** Base Station does not want to use any mcc and imsi_11_12
          ** values.  We overwrite them with our own before we store the
          ** message, as if the base station was actually using the same
          ** values as us.  This way the values will always match ours.
          ** IS-95B section 6.6.2.2.5 is the new reference.
          ** ------------------------------------------------------------ */
          if ((cur_bs_ptr->csp.esp.mcc == 0x3FF) &&
              (cur_bs_ptr->csp.esp.imsi_11_12 == 0x7F))
          {
            MSG_MED("MCC and MNC defaulted in esp", 0, 0, 0);
            cur_bs_ptr->csp.esp.mcc = cdma.mcc;
            cur_bs_ptr->csp.esp.imsi_11_12 = cdma.imsi_11_12;
          }
        }
      } /* end not from current pn check */
      break;

    case CAI_GLOBAL_REDIR_MSG:
      /* ----------------------------------
      ** Global Service Redirection Message
      ** ---------------------------------- */
      if (msg_ptr->msg.glob_redir.pilot_pn != cur_bs_ptr->pilot_pn)
      {
        /* Message is not from current pilot_pn */
        valid_msg = FALSE;
      }
      else
      {
        valid_ovhd_msg = TRUE;
        mcc_put_seq(msg_ptr->msg.glob_redir.config_msg_seq, FALSE);

        if ((!cur_bs_ptr->rx.gsd_rx) ||
            (cur_bs_ptr->gsd.config_msg_seq !=
             msg_ptr->msg.glob_redir.config_msg_seq))
        {
          /* Place message in base station information array */
          mcc_put_msg(CAI_GLOBAL_REDIR_MSG, &msg_ptr->msg);
          new_state = mccidl_proc_gsr(new_state);
        }
      }
      break;

    case CAI_EXT_GLB_REDIR_MSG:
      /* ---------------------------------------------
      ** Extended Global Service Redirection Message
      ** ------------------------------------------- */
      if (P_REV_IS_6_OR_GREATER)
      {
        if ((msg_ptr->msg.egsrm.pilot_pn != cur_bs_ptr->pilot_pn)
            #ifdef FEATURE_DYNAMIC_P_REV
            || (cdma.mob_cai_rev < P_REV_IS2000)
            #endif /*FEATURE_DYNAMIC_P_REV*/
           )
        {
          /* Message is not from current pilot_pn */
          valid_msg = FALSE;
        }
        else
        {
          valid_ovhd_msg = TRUE;
          mcc_put_seq(msg_ptr->msg.egsrm.config_msg_seq, FALSE);

          #ifdef FEATURE_IS2000_REL_A_CC
          /* -----------------------------------------------------------
          ** Channel configuration or channel monitoring requirement may
          ** change as a result of new overhead message is received.
          ** ----------------------------------------------------------- */
          sync_channel_monitor = TRUE;
          #endif /* FEATURE_IS2000_REL_A_CC */

          if ((!cur_bs_ptr->rx.egsd_rx) ||
              (cur_bs_ptr->egsd.config_msg_seq !=
               msg_ptr->msg.egsrm.config_msg_seq))
          {
            /* Place message in base station information array */
            mcc_put_msg(CAI_EXT_GLB_REDIR_MSG, &msg_ptr->msg);
            new_state = mccidl_proc_egsr(new_state);
          }
        }
      }
      break;

    case CAI_EXT_NL_MSG:
      /*  -----------------------------------------------------------
      **  For JCDMA, always allow processing of Ext Neighbor List Msg
      **  on Band Classes 3 and 6.  Otherwise, the Extended Neighbor 
      **  List Message is processed for P_REV_IN_USE of P_REV_JSTD008 
      **  and P_REV_IS95B (band class 1).
      **  -----------------------------------------------------------*/
      if (((P_REV_IS_1 || P_REV_IS_4_OR_GREATER)
           && mcc_is_band_class_in_group(MCC_PCS_GROUP, cdma.band_class))
          #ifdef FEATURE_PASSPORT_MODE
          || (mcc_passport_mode_is_jcdma() &&
              ( (cdma.band_class == CAI_BAND_CLASS3)
                || (cdma.band_class == CAI_BAND_CLASS6)))
         #endif /* FEATURE_PASSPORT_MODE */
         )
      {       /*lint +e506 */
        if (msg_ptr->msg.ext_nghbr.pilot_pn != cur_bs_ptr->pilot_pn)
        {
          /* Message is not from current pilot_pn */
          valid_msg = FALSE;
        }
        else
        {
          valid_ovhd_msg = TRUE;
          mcc_put_seq(msg_ptr->msg.ext_nghbr.config_msg_seq, FALSE);

          if ((!cur_bs_ptr->rx.nl_rx) ||
              (cur_bs_ptr->nl_config_msg_seq !=
               msg_ptr->msg.ext_nghbr.config_msg_seq))
          {
            /* Check for range violations */
            if ((msg_ptr->msg.ext_nghbr.pilot_inc == 0) ||
                (msg_ptr->msg.ext_nghbr.pilot_inc > 15))
            {
              MSG_ERROR("Ignore Ext Nghbr List Msg - Range Violation", 0, 0, 0);
            }
            else
            {
              /* Place message in base station information array */
              mcc_put_msg(CAI_EXT_NL_MSG, &msg_ptr->msg);
              new_state = mccidl_proc_nghbr_list_msg(&msg_ptr->msg, new_state);
            }
          }
        }
      } /* END OF P_REV CHECKING */
      else
      {
        MSG_ERROR("Ignore Ext Nghbr List Msg - wrong prev_in_use", 0, 0, 0);
      }
      break;

    case CAI_NL_MSG:
      #ifdef FEATURE_PASSPORT_MODE
      if (mcc_passport_mode_is_jcdma() &&
          ((cdma.band_class == CAI_BAND_CLASS3) ||
           (cdma.band_class == CAI_BAND_CLASS6)))
      {
        /*  ----------------------------------------------------------
        **  The Neighbor List message is ignored by the mobile for
        **  JCDMA, band classes 3 and 6.
        **  ----------------------------------------------------------*/
        MSG_HIGH(" Ignore Nghbr List Msg - not supported ", 0, 0, 0);
      }
      else
      #endif /* FEATURE_PASSPORT_MODE */
      {
        /*  ----------------------------------------------------------
        **  The Neighbor List message is processed by the mobile for
        **  P_REV_IN_USE values of P_REV_IS95A or P_REV_IS95B.
        **  ----------------------------------------------------------*/
        if ((P_REV_IS_2_OR_3 || P_REV_IS_4_OR_GREATER) && 
            (mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, cdma.band_class))
          ) //lint !e774 !e506
        {
          if (msg_ptr->msg.nghbr.pilot_pn != cur_bs_ptr->pilot_pn)
          {
            /* Message is not from current pilot_pn */
            valid_msg = FALSE;
          }
          else
          {
            valid_ovhd_msg = TRUE;
            mcc_put_seq(msg_ptr->msg.nghbr.config_msg_seq, FALSE);

            if ((!cur_bs_ptr->rx.nl_rx) ||
                (cur_bs_ptr->nl_config_msg_seq !=
                 msg_ptr->msg.nghbr.config_msg_seq))
            {

              /* Check for range violations */
              if ((msg_ptr->msg.nghbr.pilot_inc == 0) ||
                  (msg_ptr->msg.nghbr.pilot_inc > 15))
              {
                MSG_ERROR("Ignore Nghbr List Msg - Range Violation", 0, 0, 0);
              }
              else
              {
                /* Place message in base station information array */
                mcc_put_msg(CAI_NL_MSG, &msg_ptr->msg);
                new_state = 
                  mccidl_proc_nghbr_list_msg(&msg_ptr->msg, new_state);
              }
            }
          }
        } /* End of P_REV checking */
        else
        {
          MSG_ERROR("Ignore Nghbr List Msg - wrong prev_in_use", 0, 0, 0);
        }
      }
      break;

     case CAI_GEN_NL_MSG:
       /* ----------------------------------------------------------
       ** The General Neigbor List is processed by the mobile for
       ** P_REV_IN_USE value at least P_REV_IS95B or greater
       ** ----------------------------------------------------------*/
       if (P_REV_IS_4_OR_GREATER)
       {
         if (msg_ptr->msg.gen_nghbr.pilot_pn != cur_bs_ptr->pilot_pn)
         {
          /* Message is not from current pilot_pn */
          valid_msg = FALSE;
         }
         else
         {
           valid_ovhd_msg = TRUE;
           mcc_put_seq(msg_ptr->msg.gen_nghbr.config_msg_seq, FALSE);

           if ((!cur_bs_ptr->rx.gnl_rx) ||
               (cur_bs_ptr->gnl_config_msg_seq !=
                msg_ptr->msg.gen_nghbr.config_msg_seq))
           {
             /* Check for range violations */
             if ((msg_ptr->msg.gen_nghbr.pilot_inc == 0) ||
                 (msg_ptr->msg.gen_nghbr.pilot_inc > 15))
             {
               MSG_ERROR("Ignore Gen Nghbr List Msg - Range Violation", 0, 0, 0);
             }
             else
             {
               /* ----------------------------------------------------
               ** Place message in base station information array.
               ** Message must be put in array before it is processed.
               ** ---------------------------------------------------- */
               mcc_put_msg(CAI_GEN_NL_MSG, &msg_ptr->msg);
               new_state = 
                 mccidl_proc_nghbr_list_msg(&msg_ptr->msg, new_state);
             }
           }
         }
       } /* If p_rev_in_use checking */
       else
       {
         MSG_ERROR("Ignore Gen Nghbr List Msg - prev_in_use < 4 ", 0, 0, 0);
       }
       break;

    case CAI_CL_MSG:
      /* -------------------------
      ** CDMA Channel List Message
      ** ------------------------- */
      if (msg_ptr->msg.chnlist.pilot_pn != cur_bs_ptr->pilot_pn)
      {
        /* Message is not from current pilot_pn */
        valid_msg = FALSE;
      }
      else
      {
        valid_ovhd_msg = TRUE;
        mcc_put_seq(msg_ptr->msg.chnlist.config_msg_seq, FALSE);

        if ((!cur_bs_ptr->rx.cl_rx) ||
            (cur_bs_ptr->cl.config_msg_seq !=
             msg_ptr->msg.chnlist.config_msg_seq))
        {
          if (msg_ptr->msg.chnlist.num_freq == 0)
          {
            MSG_ERROR("Ignore Chn List Msg - Zero freq", 0, 0, 0);
          }
          else
          {
            /* ----------------------------------------------------
            ** Place message in base station information array.
            ** Message must be put in array before it is processed.
            ** ---------------------------------------------------- */
            mcc_put_msg(CAI_CL_MSG, &msg_ptr->msg);
            new_state = mccidl_proc_cl_msg(new_state);
          }
        }
      }
      break;

    case CAI_EXT_CDMA_CHN_LIST_MSG:
      /* -----------------------------------
      ** Extended CDMA Channel List Message
      ** --------------------------------- */
      if (P_REV_IS_6_OR_GREATER)
      {
        if ((msg_ptr->msg.ecclm.pilot_pn != cur_bs_ptr->pilot_pn)
            #ifdef FEATURE_DYNAMIC_P_REV
            || (cdma.mob_cai_rev < P_REV_IS2000)
            #endif /*FEATURE_DYNAMIC_P_REV*/
           )
        {
          /* Message is not from current pilot_pn */
          valid_msg = FALSE;
        }
        else
        {
          valid_ovhd_msg = TRUE;
          mcc_put_seq(msg_ptr->msg.ecclm.config_msg_seq, FALSE);

          #ifdef FEATURE_IS2000_REL_A_CC
          /* -----------------------------------------------------------
          ** Channel configuration or channel monitoring requirement may
          ** change as a result of new overhead message is received.
          ** ----------------------------------------------------------- */
          sync_channel_monitor = TRUE;
          #endif /* FEATURE_IS2000_REL_A_CC */

          if ((!cur_bs_ptr->rx.ecl_rx) ||
              (cur_bs_ptr->ecl.config_msg_seq !=
               msg_ptr->msg.ecclm.config_msg_seq))
          {
            if (msg_ptr->msg.ecclm.num_freq == 0)
            {
              MSG_ERROR("Ignore Ext Chn List Msg - Zero freq", 0, 0, 0);
            }
            else
            {
              /* ----------------------------------------------------
              ** Place message in base station information array.
              ** Message must be put in array before it is processed.
              ** ---------------------------------------------------- */
              mcc_put_msg(CAI_EXT_CDMA_CHN_LIST_MSG, &msg_ptr->msg);
              new_state = mccidl_proc_ecl_msg(new_state);
            }
          }
        }
      }
      break;

    #ifdef FEATURE_IS2000_REL_A_CC
    case CAI_ANSI41_SYS_PARM_MSG:
      if (msg_ptr->msg.a41spm.pilot_pn != cur_bs_ptr->pilot_pn)
      {
        valid_msg = FALSE;
      }
      else
      {
        valid_ovhd_msg = TRUE;
        mcc_put_seq(msg_ptr->msg.a41spm.config_msg_seq, FALSE);

        /* --------------------------------------------------------------
        ** Channel configuration or channel monitoring requirement may
        ** change as a result of new ANSI-41 System Parm msg is received.
        ** -------------------------------------------------------------- */
        sync_channel_monitor = TRUE;

        if ((!cur_bs_ptr->rx.a41spm_rx) ||
            (cur_bs_ptr->a41spm_config_msg_seq !=
             msg_ptr->msg.a41spm.config_msg_seq))
        {
          /* Check for any range violations */
          if ((msg_ptr->msg.a41spm.reg_prd != 0) &&
              ((msg_ptr->msg.a41spm.reg_prd < 29) ||
               (msg_ptr->msg.a41spm.reg_prd > 85)))
          {
            MSG_ERROR("Ignore ANSI41 Sys Params Msg - Range Violation", 0, 0, 0);
          }
          else if (msg_ptr->msg.a41spm.other_info_incl)
          {
            MSG_ERROR("Ignore ANSI41 Sys Params Msg - OTHER_INFO_INCL = 1", 0, 0, 0);
          }
          else
          {
            /* ----------------------------------------------------
            ** Place message in base station information array.
            ** Message must be put in array before it is processed.
            ** ---------------------------------------------------- */
            mcc_put_msg(CAI_ANSI41_SYS_PARM_MSG, &msg_ptr->msg);
            new_state = mccidl_proc_ansi41_sys_parm(new_state);
          }
        }
      }
      break;

    case CAI_MC_RR_PARM_MSG:
      if (msg_ptr->msg.mcrrpm.pilot_pn != cur_bs_ptr->pilot_pn)
      {
        valid_msg = FALSE;
      }
      else
      {
        valid_ovhd_msg = TRUE;
        mcc_put_seq(msg_ptr->msg.mcrrpm.config_msg_seq, FALSE);

        /* -----------------------------------------------------------
        ** Channel configuration or channel monitoring requirement may
        ** change as a result of new MC-RR Parm msg is received.
        ** ----------------------------------------------------------- */
        sync_channel_monitor = TRUE;

        if ((!cur_bs_ptr->rx.mcrrpm_rx) ||
            (cur_bs_ptr->mcrrpm_config_msg_seq !=
             msg_ptr->msg.mcrrpm.config_msg_seq))
        {
          /* Check for any range violations */
          /* Check for BS lat and long, pwr_rep_thresh */
          extended_lat = EXTEND22( msg_ptr->msg.mcrrpm.base_lat ); //lint !e737
          extended_long = EXTEND23( msg_ptr->msg.mcrrpm.base_long ); //lint !e737

          if ((extended_lat < -1296000L) || (extended_lat > 1296000L) ||
              (extended_long < -2592000L) || (extended_long > 2592000L) ||
              ((msg_ptr->msg.mcrrpm.pwr_thresh_enable) &&
               (msg_ptr->msg.mcrrpm.pwr_rep_thresh == 0)))
          {
            MSG_ERROR("Ignore MC-RR Params Msg - Range Violation", 0, 0, 0);
          }
          else if (!msg_ptr->msg.mcrrpm.cch_info_incl)
          {
            MSG_ERROR("Ignore MC-RR Params Msg - CCH_INFO_INCL = 0", 0, 0, 0);
          }
          else
          {
            /* -------------------------------------------------------------
            ** If the base station sets the num_bcch_bcast = 7 which violates
            ** the standard, the mobile just uses the first 6 entries for
            ** the secondary BCCH.
            ** -------------------------------------------------------------*/
            if (msg_ptr->msg.mcrrpm.num_bcch_bcast > CAI_MAX_NON_PRI_BCCH_CHAN)
            {
              MSG_ERROR("MC-RR Error: num_bcch_bcast=%d > 6, use only 6",
                        msg_ptr->msg.mcrrpm.num_bcch_bcast, 0, 0);
              msg_ptr->msg.mcrrpm.num_bcch_bcast = CAI_MAX_NON_PRI_BCCH_CHAN;
            }

            /* ----------------------------------------------------
            ** Place message in base station information array.
            ** Message must be put in array before it is processed.
            ** ---------------------------------------------------- */
            mcc_put_msg( CAI_MC_RR_PARM_MSG, &msg_ptr->msg );
            new_state = mccidl_proc_mcrr_parm(new_state);

            #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
            if ((new_state != CDMA_EXIT) &&
                mccbcsms_is_bcast_init() &&
                bcast_pending)
            {
              /* the MCRRPM initialized the bc_index and there's a      */
              /* pending GPM with bc page rec pending, so process it now*/
              new_state = mccidl_proc_pending_bcast_page(&bcast_page, new_state);
            }
            #endif /*  FEATURE_IS2000_REL_A_CC_BCSMS */
          }
        }
      }
      break;

    case CAI_ANSI41_RAND_MSG:
      if (msg_ptr->msg.a41rm.pilot_pn != cur_bs_ptr->pilot_pn)
      {
        valid_msg = FALSE;
      }
      #ifdef FEATURE_AUTH
      else
      {
        valid_ovhd_msg = TRUE;

        /* --------------------------------------------------------------------
        ** This message is only needed in Update Overhead Information Substate,
        ** so we set the rx flag to false beforehand (in mccidl_check_ovhd) to
        ** ensure we have the latest version. We don't need the message other-
        ** wise, so to increase efficiency, we just process it the first time,
        ** and ignore it afterwards.
        ** However, if the RAND value got updated, MS shall process it since
        ** we will need the update for pending access.
        ** -------------------------------------------------------------------- */
        if (!cur_bs_ptr->rx.a41randm_rx ||
            (cur_bs_ptr->rp.rand != msg_ptr->msg.a41rm.rand))
        {
          /* -----------------------------------------------------------
          ** Channel configuration or channel monitoring requirement may
          ** change as a result of ANSI-41 Rand msg is received.
          ** ----------------------------------------------------------- */
          sync_channel_monitor = TRUE;

          mcc_put_msg(CAI_ANSI41_RAND_MSG, &msg_ptr->msg);
          mccidl_proc_ansi41_rand(&msg_ptr->msg.a41rm, new_state);
        }
      }
      #endif /* FEATURE_AUTH */
      break;

    case CAI_EN_AC_PARM_MSG:
      if (msg_ptr->msg.enacpm.pilot_pn != cur_bs_ptr->pilot_pn)
      {
        valid_msg = FALSE;
      }
      else
      {
        valid_ovhd_msg = TRUE;

        /* -----------------------------------------------------------
        ** Channel configuration or channel monitoring requirement may
        ** change as a result of Enhanced Access Parm msg is received.
        ** ----------------------------------------------------------- */
        sync_channel_monitor = TRUE;

        if (!cur_bs_ptr->rx.eapm_rx ||
            (cur_bs_ptr->eapm.acc_msg_seq !=
             msg_ptr->msg.enacpm.acc_msg_seq))
        {
          if ((msg_ptr->msg.enacpm.max_req_seq == 0) ||
              (msg_ptr->msg.enacpm.max_rsp_seq == 0))
          {
            MSG_ERROR( "Ignore Enhanced Acc Params Msg - Range Violation", 0, 0, 0 );
          }
          else
          {

            mcc_put_msg(CAI_EN_AC_PARM_MSG, &msg_ptr->msg);
            mccidl_proc_en_acc_parm(&msg_ptr->msg.enacpm);
          }
        }
        else
        {
          mcc_put_seq( msg_ptr->msg.enacpm.acc_msg_seq, TRUE);
        }
      }
      break;

    case CAI_UNIV_NL_MSG:
      if (msg_ptr->msg.unlm.pilot_pn != cur_bs_ptr->pilot_pn)
      {
        valid_msg = FALSE;
      }
      else
      {
        valid_ovhd_msg = TRUE;
        mcc_put_seq(msg_ptr->msg.unlm.config_msg_seq, FALSE);

        /* ---------------------------------------------------------------
        ** Channel configuration or channel monitoring requirement may
        ** change as a result of new Universal Neigh List msg is received.
        ** --------------------------------------------------------------- */
        sync_channel_monitor = TRUE;

        if (!cur_bs_ptr->rx.unlm_rx ||
            (cur_bs_ptr->unlm_config_msg_seq !=
             msg_ptr->msg.unlm.config_msg_seq))
        {
          mcc_put_msg( CAI_UNIV_NL_MSG, &msg_ptr->msg );
          new_state = 
            mccidl_proc_univ_nghbr_list_msg(&msg_ptr->msg.unlm, new_state);
        }
      }
      break;
    #endif /* FEATURE_IS2000_REL_A_CC */

    case CAI_PAGE_MSG:
    case CAI_SLT_PAGE_MSG:
    case CAI_GENERAL_PAGE_MSG:

    #ifdef FEATURE_IS2000_REL_A_CC
    case CAI_UNIVERSAL_PAGE_MSG:
    case CAI_UPM_FIRST_SEG_MSG:
    case CAI_UPM_MIDDLE_SEG_MSG:
    case CAI_UPM_FINAL_SEG_MSG:
    #endif /* FEATURE_IS2000_REL_A_CC */

      if ( !sd_misc_is_cdma_locked() 
           #ifdef FEATURE_PASSPORT_MODE
           /* Ignore the page if block_msg_mask blocks it. If JCDMA mode is not
              ON, we do not check if the page is blocked.  */
           && (!mcc_passport_mode_is_jcdma() ||
               !mcc_msg_is_blocked(CDMA_MSG_TYPE_PAGE))
           #endif

           )
      {

        #ifdef FEATURE_IS2000_REL_A_CC
        /* May need to update channel supervision in case of change in
           config_msg_seq. */
        sync_channel_monitor = TRUE;
        #endif /* FEATURE_IS2000_REL_A_CC */

        new_state = mccidl_proc_page(msg_ptr, new_state);
      }
      break;

    case CAI_PC_ORD_MSG:
    case CAI_CHN_ASN_MSG:
    case CAI_PC_TMSI_ASGN_MSG:
    case CAI_PC_BURST_MSG:
    case CAI_FEATURE_MSG:
    case CAI_PC_AUTH_MSG:
    case CAI_PC_SSD_UP_MSG:
    case CAI_SERVICE_REDIR_MSG:
    case CAI_PC_STREQ_MSG:
    case CAI_PACA_MSG:
    case CAI_EXT_CHN_ASN_MSG:

    #ifdef FEATURE_IS2000_REL_A
    case CAI_SEC_MODE_CMD_MSG:
    #endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_C
    case CAI_AUTH_REQ_MSG:
#endif /* FEATURE_IS2000_REL_C */

      /* ---------------------------
      ** Order or Directed Message.
      ** ------------------------- */
      if ( ( cdma.sa_tc_state != CDMA_IDLE ) || ( cdma.ho_state == CDMA_AEHO ) 
           || cdma.transceiver_request_pending
         )
      {
        /* -----------------------------------------------------------------
        ** We only process a subset of the messages (mostly overheads)
        ** if:
        ** 1. we are in Update Overhead Info substate of SA, i.e.,
        ** cdma.curr_state == CDMA_IDLE && cdma.sa_tc_state != CDMA_IDLE,
        ** or
        ** 2. We are in the AEHO region of IDLE state.
        ** ----------------------------------------------------------------- */
        new_state = mccidl_ovhd_proc_msg(msg_ptr, new_state);
      }
      else
      {
        new_state = mccidl_proc_msg(msg_ptr, new_state);
      }
      break;

    default:
      MSG_HIGH( "Invalid message type %d", msg_ptr->msg.gen.msg_type, 0, 0 );
      /* ---------------------------------------------------
      ** We used to reject any message with an invalid type,
      ** but IS-95A 6.6.2.4 now says to just ignore it.
      ** --------------------------------------------------- */
      valid_msg = FALSE;
      break;

  } /* end switch (msg_type) */

  if (valid_msg)
  {
    /* ---------------------------------------------------------
    ** Update time stamp for last valid message received with
    ** the time this message was received, even if this was not
    ** an overhead message or was previously stored for this
    ** paging channel.  See IS-95A section 6.6.2.2.
    ** --------------------------------------------------------- */
    qw_equ( cur_bs_ptr->rx.msg_time, msg_ptr->frm.frame );
  }

  if ( sync_channel_monitor )
  {
    /* --------------------------------------------------------------
    ** After OVHD / GPM / UPM / CAM / ECAM processing, MS may need to
    ** update channel supervision on BCCH / FCCCH / PCH.
    ** -------------------------------------------------------------- */
    mccidl_supervise_channel_monitoring(new_state, NULL, FALSE);
  }

  if (valid_ovhd_msg)
  {
    if (OVHD_CHAN_IS_PCH)
    {
      /* Events report: Paging channel acquired */
      mclog_report_event_pch_acquired(TRUE, cdma.pagech, cur_bs_ptr->pilot_pn);
    }
    #ifdef FEATURE_IS2000_REL_A_CC
    else
    {
      /* Events report: Broadcast Control Channel acquired */
      mclog_report_event_bcch_acquired(TRUE, cdma.bcch, cur_bs_ptr->pilot_pn);
    }
    #endif /* FEATURE_IS2000_REL_A_CC */

    /* In Pseudo-Idle Mode, as long as MS receives OTA with config_msg_seq. MS
       shall goes back to sleep as soon as overheads are current. */
    if ( ( msg_ptr->msg.gen.msg_type != CAI_ACC_PARM_MSG )
         #ifdef FEATURE_IS2000_REL_A_CC
         && ( msg_ptr->msg.gen.msg_type != CAI_EN_AC_PARM_MSG )
         && ( msg_ptr->msg.gen.msg_type != CAI_ANSI41_RAND_MSG )
         #endif /* FEATURE_IS2000_REL_A_CC */
       )
    {
      if ( ( cdma.entry_ptr->entry == MCC_PSEUDO_IDLE ) 
           && !mccidl_sleep_ok )
      {
        if ( mccidl_assert_sleep_ok(msg_ptr->frm.frame) )
        {
          MSG_MED("Sleep ok for Pseudo-Idle", 0, 0, 0);

          /* Update sleep slot mask */
          mccidl_update_sleep_slot_mask(PAGE_SLOT);
        }
      }
    }

  }

  return (new_state);

} /* end mccidl_msg_handler() */

/*===========================================================================

FUNCTION MCCIDL_PROC_RX_RPT

DESCRIPTION
  This procedure processes the idle Receive task reports.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_proc_rx_rpt
(
  mccrx_rpt_type *rpt_ptr,
    /* report received from Receive task */
  word new_state
    /* next state to be processed */
)
{
  switch ( rpt_ptr->hdr.rpt)
  {
    /* -----------------
    ** Idle fade timeout
    ** ----------------- */
    case RXC_PC_T30M_TMO_R:
    /* -------------------------------------------------------------------
    ** In Update Overhead Information substate, the Mobile cannot perform
    ** any Access Handoffs, hence Paging Channel loss is declared when the
    ** T40M expires.
    ** ----------------------------------------------------------------- */
    case RXC_PC_T40M_TMO_R:
    /* -----------------------------------------------------------------
    ** No valid message on BCCH/FCCH in 4 secs when MS is simulateously 
    ** monitoring both BCCH and FCCCH.
    ** ----------------------------------------------------------------- */
    case RXC_PC_FS_TMO_R:
    {
      /* Check current state before process fade report */
      if ((!access_fade_timer_in_use && (rpt_ptr->hdr.rpt == RXC_PC_T30M_TMO_R)) ||
          (access_fade_timer_in_use && ((rpt_ptr->hdr.rpt == RXC_PC_T40M_TMO_R)
         )))
      {
        #ifndef FEATURE_ERRLOG_DEBUG

        if ( OVHD_CHAN_IS_PCH )
        {
          MSG_HIGH( "Lost Paging Channel, timer %d expired", rpt_ptr->hdr.rpt, 0, 0 );
        }

        #ifdef FEATURE_IS2000_REL_A_CC
        else 
        {
          MSG_HIGH( "Lost BCCH/FCCCH, timer %d expired", rpt_ptr->hdr.rpt, 0, 0 );
        }
        #endif /* FEATURE_IS2000_REL_A_CC */

        #else /* FEATURE_ERRLOG_DEBUG */

        if ( OVHD_CHAN_IS_PCH )
        {
          ERR( "Lost Paging Channel, timer %d expired", rpt_ptr->hdr.rpt, 0, 0 );
        }

        #ifdef FEATURE_IS2000_REL_A_CC
        else 
        {
          ERR( "Lost BCCH/FCCCH, timer %d expired", rpt_ptr->hdr.rpt, 0, 0 );
        }
        #endif /* FEATURE_IS2000_REL_A_CC */

        #endif /* FEATURE_ERRLOG_DEBUG */

        /* Need to restart - we have lost sync */
        if (cdma.sa_tc_state == CDMA_IDLE)
        {
          /* -------------------------------------------------------------
          ** Inform system determination of system lost and determines the
          ** next state to be processed based on the returned SS-Action.
          ** ------------------------------------------------------------- */
          if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                  sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_UNKNOWN, NULL ) )
             )
          {
            new_state = CDMA_EXIT;
            mcc_exit.exit = MCC_SYSTEM_LOST;
          }

        }
        else
        {
          /* -------------------------------------------------------------
          ** Inform system determination of system lost and determines the
          ** next state to be processed based on the returned SS-Action.
          ** ------------------------------------------------------------- */
          if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                  sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_UNKNOWN, NULL ) )
             )
          {
            mcc_exit.exit = MCC_SYSTEM_LOST;
            new_state = CDMA_EXIT;
          }

          /* Clear state timer */
          (void) rex_clr_timer( &mcc_state_timer );
          (void) rex_clr_sigs( &mc_tcb, MCC_STATE_TIMER_SIG );

          /* --------------------------------------------
          ** Send a message to RX to start using the Idle
          ** fade timer instead of the Access fade timer.
          ** -------------------------------------------- */
          mcc_rx_buf.idl_fade.hdr.command = RXC_IDL_FADE_F;
          mcc_rx_buf.idl_fade.hdr.cmd_hdr.done_q_ptr = NULL;

          mcc_rxc_cmd( &mcc_rx_buf );
        }
      }
      break;
    }
    case RXC_PC_T72M_TMO_R:
    {
      MSG_MED("Got T72M Fade report in Idle State, ignoring..", 0, 0, 0 );    
      break;
    }
    case RXC_SLOT_R:
    {
      /* ---------------------------------------------------
      ** 2 slots have expired and a valid msg was received.
      ** We can sleep if configuration messages are current.
      ** --------------------------------------------------- */
      MSG_MED("CP rec'd SLOT_R, Slot_mask=%d",rpt_ptr->slot.slot_mask, 0, 0);

      #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      if (OVHD_CHAN_IS_BCCH && (rpt_ptr->slot.slot_mask == BC_DATA_SLOT))
      {
        if (qw_cmp(rpt_ptr->slot.rpt_time, mccidl_sleep_ok_time) < 0)
        {
          MSG_HIGH("SLOT_R for 2nd BCCH ignored (from last slot?)", 0, 0, 0);
        }
        else
        {
          MSG_HIGH("SLOT_R for 2nd BCCH", 0, 0, 0);
          mccidl_supervise_channel_monitoring(new_state, NULL, TRUE);
        }
      }
      else
      #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
      {
        if ( !mccidl_assert_sleep_ok(rpt_ptr->slot.rpt_time) )
        {
          MSG_HIGH("SLOT_R command ignored (from last slot?)", 0, 0, 0);
        }

        /* Register the type slot that wants to sleep */
        sleep_slot_mask |= rpt_ptr->slot.slot_mask;     
        MSG_HIGH("MC received SLOT_R, slot mask=%d", rpt_ptr->slot.slot_mask, 0, 0);
      }
      break;
    }

    default:
    {
      ERR("Invalid report type %d",rpt_ptr->hdr.rpt,0,0 );
      break;
    }
  } /* switch */

  /* do report cleanup processing */
  cmd_done(&rpt_ptr->hdr.rpt_hdr);

  return (new_state);

}/* mccidl_proc_rx_rpt */

/*===========================================================================

FUNCTION MCCIDL_PROCESS_STORED_OVHDS

DESCRIPTION
  This procedure processes stored current ovhds for BS after an
  idle handoff or after a TC to idle transition

DEPENDENCIES
  Ovhds must be verified to be current before calling this

RETURN VALUE
  new_state if processing of ovhds results in a state change

SIDE EFFECTS
  None.

===========================================================================*/
word mccidl_process_stored_ovhds
( 
  word new_state
)
{
  boolean rx_flag = FALSE;
  cdma_bs_type *old_bs_ptr = cur_bs_ptr; /* Pointer to old BS info */

  /*
  ** At this point, we just changed to use the new base station
  ** (cur_bs_ptr new points to new BS.  Here, let's check if the P_REV of the
  ** new base station is different.  If so, we should change to that P_REV
  ** here such that all messages
  ** processed below are processed with the new P_REV.  Can't do this for
  ** pre-IS95B since P_REV is not in ESPM.
  */
  if ( ( OVHD_CHAN_IS_PCH && ESP_MSG_IS_RECVD )
       #ifdef FEATURE_IS2000_REL_A_CC
       || ( OVHD_CHAN_IS_BCCH && MCRRP_MSG_IS_RECVD )
       #endif /* FEATURE_IS2000_REL_A_CC */
     )
  {
    /*
    ** If the ESPM or MCRRPM was good, then check the P_REV.  We know that the
    ** P_REV contained in this message is good even if it's P_REV 3 or lower
    ** since it is properly initialized when it was received.
    */
    mcc_update_bs_p_rev(cur_bs_ptr->csp.esp.p_rev);
  }

  /* Reset flag */ 
  rescan_ignored = FALSE;

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    if (ANSI41SP_MSG_IS_RECVD)
    {
      /* --------------------------------------------------------------
      ** Set rx.mcrrpm_rx to FALSE, so that registration initialization
      ** will not performed twice.
      ** ------------------------------------------------------------ */
      if (MCRRP_MSG_IS_RECVD)
      {
        cur_bs_ptr->rx.mcrrpm_rx = FALSE;
        rx_flag = TRUE;
      }

      /* note that mccidl_proc_ansi41_sys_parm() may change
         mccidl_cheap_idle_ho. */
      new_state = mccidl_proc_ansi41_sys_parm(new_state);

      /* Restore MC-RR Parameter Message received flag */
      if (rx_flag)
      {
        cur_bs_ptr->rx.mcrrpm_rx = TRUE;
      }
    }

    /* ---------------------------------------------------------------
    ** Since MC-RR Parameter Message may change IMSI hashing key, so we
    ** process it before Channel List Message or Extended Channel List
    ** Message.
    ** --------------------------------------------------------------- */
    if (MCRRP_MSG_IS_RECVD)
    {
      new_state = mccidl_proc_mcrr_parm(new_state);
    }

    #if (defined(FEATURE_HIGH_PSIST) && defined (FEATURE_PASSPORT_MODE))
    if(mcc_passport_mode_is_jcdma())
    {     
      /* If the persistence info has changed, send a report to CM.  Fast
         raho's are not in MC's context, so wait until we wake up to report. */
      if (ENH_ACC_PARM_MSG_IS_RECVD && !mccidl_fast_raho)
      {
        mccidl_update_psist_info(CAI_EACH, &cur_bs_ptr->eapm);
      }
    }
    #endif /* FEATURE_HIGH_PSIST && FEATURE_PASSPORT_MODE */

  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  {
    if (SP_MSG_IS_RECVD)
    {
      /* ------------------------------------------------------------
      ** Set rx.esp_rx to FALSE, so that soft handoff parameters will
      ** be updated correctly before sending SRCH_PARM_F commands to
      ** Searcher.
      ** ------------------------------------------------------------ */
      if (ESP_MSG_IS_RECVD)
      {
        cur_bs_ptr->rx.esp_rx = FALSE;
        rx_flag = TRUE;
      }

      /* note that proc_sys_parm() may change mccidl_cheap_idle_ho. */
      new_state = mccidl_proc_sys_parm(new_state);

      /* Restore Extended System Parameter Message received flag.
         PCH hashing may cause value change in cur_bs_ptr. Use old_bs_ptr
         to make sure the right flag is restored. */
      if (rx_flag)
      {
        old_bs_ptr->rx.esp_rx = TRUE;
      }
    } /* end of sys parm msg check */

    /* ---------------------------------------------------------------
     ** Since Extended System Parameter Message may change IMSI hashing
     ** key, so we process it before Channel List Message or Extended
     ** Channel List Message.
     ** --------------------------------------------------------------- */
    if ( ESP_MSG_IS_RECVD && ( old_bs_ptr == cur_bs_ptr ) )
    {
      new_state = mccidl_proc_ext_sys_parm(new_state);
    }

    #if (defined(FEATURE_HIGH_PSIST) && defined (FEATURE_PASSPORT_MODE))
    if(mcc_passport_mode_is_jcdma())
    {    
      /* If the persistence info has changed, send a report to CM.  Fast
         raho's are not in MC's context, so wait until we wake up to report. */
      if (AP_MSG_IS_RECVD && !mccidl_fast_raho)
      {
        mccidl_update_psist_info(CAI_ACH, &cur_bs_ptr->ap);
      }
    }
    #endif /* FEATURE_HIGH_PSIST && FEATURE_PASSPORT_MODE */
    
  }

  if ( rescan_ignored )
  {
    MSG_HIGH("MS must follow rescan", 0, 0, 0);

    if (mccidl_fast_raho)
    {
      /* Not a cheap idle handoff */
      mccidl_cheap_idle_ho = FALSE;
    }
  }

  /* -----------------------------------------------------------------
   ** If the Mobile is performing AHO or APHO, Mobile need not process
   ** the Channel List message or Extended Channel List Message.
   ** --------------------------------------------------------------- */
  if ((cdma.ho_state != CDMA_AHO) && (cdma.ho_state != CDMA_APHO))
  {
    
    #ifdef FEATURE_IS2000_REL_A_CC
    if (OVHD_CHAN_IS_BCCH)
    {
      if (EXT_CHAN_LIST_MSG_IS_RECVD)
      {
        /* note that mccidl_proc_ecl_msg() may change mccidl_cheap_idle_ho. */
        new_state = mccidl_proc_ecl_msg(new_state);
      }
    }
    else
    #endif /* FEATURE_IS2000_REL_A_CC */

    {
      if (EXT_CHAN_LIST_MSG_IS_RECVD)
      {
        /* note that mccidl_proc_ecl_msg() may change mccidl_cheap_idle_ho. */
        new_state = mccidl_proc_ecl_msg(new_state);
      }

      if (CHAN_LIST_MSG_IS_RECVD)
      {
        /* note that mccidl_proc_cl_msg() may change mccidl_cheap_idle_ho. */
        new_state = mccidl_proc_cl_msg(new_state);
      }
    }
  }

  /* -------------------------------------------------------------
  ** Wait for new Global Service Redirection Message on the new PN
  ** ------------------------------------------------------------- */
  gsr_ignored = FALSE;
  gsd_proc_pending = FALSE;

  /* ----------------------------------------------------------------------
  ** Wait for new Extended Global Service Redirection Message on the new PN
  ** ---------------------------------------------------------------------- */
  egsr_ignored = FALSE;
  egsd_proc_pending = FALSE;

  /* -----------------------------------------------------------------
  ** If a Extended Global Service Redirection Message is being sent on
  ** the new PN, ensure that we will process it.  We will wait for a
  ** new one to be received so that we don't defeat special case
  ** processing for Preferred Roaming.
  ** ----------------------------------------------------------------- */
  if (EGSR_MSG_IS_RECVD)
  {
    if ( ((OVHD_CHAN_IS_PCH && SP_MSG_IS_RECVD) 
          #ifdef FEATURE_IS2000_REL_A
          || (OVHD_CHAN_IS_BCCH && MCRRP_MSG_IS_RECVD) 
          #endif /* FEATURE_IS2000_REL_A */
         )
         && cur_bs_ptr->csp.sp.ext_global_redirect
         && (mccidl_construct_redir_rec_list_from_egsr() 
         #ifdef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
         || mccidl_synch_workaround.egsr_is_synch_workaround
         #endif /* FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND */
         )
       )
    {
      /* Extended Global Service Redirection Message received. And the
       * mobile must follow the EGSR message.
       */
      MSG_MED("Mobile must follow EGSR on NEW BS, rx=%d, gd=%d",
              cur_bs_ptr->rx.sp_rx , cur_bs_ptr->csp.sp.ext_global_redirect,0);

      if (mccidl_fast_raho)
      {
        /* Not a cheap idle handoff */
        mccidl_cheap_idle_ho = FALSE;
      }

      /* Set egsr_ignored to true, so that Mobile will process the received
         EGSRM when going back to CDMA_IDLE_INIT state */
      egsr_ignored = TRUE;
    }
  }

   /* -----------------------------------------------------------------
   ** If a Global Service Redirection Message is being sent on the new
   ** PN, ensure that we will process it.  We will wait for a new one
   ** to be received so that we don't defeat special case processing
   ** for Preferred Roaming.
   ** ----------------------------------------------------------------- */
  if (OVHD_CHAN_IS_PCH && SP_MSG_IS_RECVD && 
      cur_bs_ptr->csp.sp.global_redirect && 
      (!P_REV_IS_6_OR_GREATER || !cur_bs_ptr->csp.sp.ext_global_redirect) &&
      GSR_MSG_IS_RECVD &&
      mccidl_construct_redir_rec_list_from_gsr())
  {
    /* Global Service Redirection Message received. And the mobile must
     * follow the GSR message.
     */
    MSG_MED("Mobile must follow GSR on NEW BS, rx=%d, gd=%d",
      cur_bs_ptr->rx.sp_rx , cur_bs_ptr->csp.sp.global_redirect, 0);

    if (mccidl_fast_raho)
    {
      /* Not a cheap idle handoff */
      mccidl_cheap_idle_ho = FALSE;
    }

    /* Set gsr_ignored to true, so that Mobile will process the received
       GSRM when going back to CDMA_IDLE_INIT state */
    gsr_ignored = TRUE;
  } /* end if (OVHD_CHAN_IS_PCH && SP_MSG_IS_RECVD && ...) */

  return new_state;

} /* mccidl_process_stored_ovhds */

/*===========================================================================

FUNCTION MCCIDL_GET_NGHBR_INDEX

DESCRIPTION
  This procedure gets the index of a specified pilot in the neighbor list.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the index value of this pilot in the neighbor list.
  "-1" indicates that this pilot is not in the neighbor list.

SIDE EFFECTS
  None.

===========================================================================*/
int2 mccidl_get_nghbr_index
(
  word pilot_pn,
  word new_freq,
  byte new_band
)
{
/* FEATURE_JSTD008 was removed for a common interface */
  int2 nghbr_index = -1;
    /* Return value */

  int2 i;
    /* Loop counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((OVHD_CHAN_IS_PCH && cur_bs_ptr->rx.nl_rx)
      #ifdef FEATURE_IS2000_REL_A_CC
      || (OVHD_CHAN_IS_BCCH && cur_bs_ptr->rx.unlm_rx)
      #endif /* FEATURE_IS2000_REL_A_CC */
     )
  {
    for (i=0; i<cur_bs_ptr->nl.num_nghbr_pns; i++)
    {
      if (cur_bs_ptr->nl.nset[i].pilot_pn == pilot_pn)
      {
        /* ----------------------------------------------------------
        ** We've only found the neighbor if the frequency and band
        ** matches, or if there is no frequency (meaning it is the
        ** current one).
        ** ---------------------------------------------------------- */
        if ((!cur_bs_ptr->nl.nset[i].freq_incl && (cdma.cdmach == new_freq) &&
             (cdma.band_class == new_band))            ||
            (cur_bs_ptr->nl.nset[i].freq_incl &&
             (cur_bs_ptr->nl.nset[i].freq == new_freq) &&
             (cur_bs_ptr->nl.nset[i].band == new_band)))
        {
          nghbr_index = i;
          break;
        }
      }
    }
  }

  return( nghbr_index );

}/* mccidl_get_ngbr_index */

/*===========================================================================

FUNCTION MCCIDL_HANDOFF

DESCRIPTION
  This procedure performs handoff processing when the Mobile is in
  Idle state and System Access state.  Idle Handoff, Access Entry HO,
  Access Probe Handoff and Access Handoff are processed with this
  function.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_handoff
(
  word new_pilot,
    /* New pilot received in searcher report */
  word new_freq,
    /* New cdma frequency received in searcher report. */
  byte new_band_class,
    /* New band class received in searcher report */
  word new_state,
    /* Next state to be processed */
  int8 ecio_old_pilot,
  int8 ecio_new_pilot,
  cdma_ho_type ho_type
    /* Information for event reporting */
)
{
  int2 i;
    /* Index to the neighbor list */
  word parm_stat;
    /* Status returned by parm_inc procedure */
  byte nghbr_config = 3;
    /* Default assume neighbor config = 3 */
  cdma_bs_type *old_bs_ptr;
    /* Keeps pointer to old BS info for temp nl construction */

  word    old_state;    /* Current state */
  boolean slotted_mode; /* Indicator whether in slotted mode of operation */
  #ifndef FEATURE_IS2000_REL_A
  boolean qpch_in_use;  /* Indicator whether QPCH in use */
  #endif /* !FEATURE_IS2000_REL_A */
  bs_info_id_type bs_info_id;
  boolean nghbr_bcch_supported = FALSE;
    /* Indicates if BCCH is supported on the new pilot */
  byte old_td_mode = 0;
    /* TD mode of the old pilot */
  byte old_td_power_level = 0;
    /* TD power level of the old pilot */
  boolean td_supported = FALSE;
    /* Indicates if TD is supported on the new pilot */
  #ifdef FEATURE_IS2000_REL_A_CC
  byte td_mode = 0;         /* TD mode of the new pilot */
  byte td_power_level = 0;  /* TD power level of the new pilot */
  #endif /* FEATURE_IS2000_REL_A_CC */

  boolean inter_freq_handoff = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store current state */
  old_state = new_state;

  /* Default the type of overhead channel */
  bs_info_id.ovhd_chan = cur_bs_ptr->ovhd_chan_info.ovhd_chan;

  if (mccidl_fast_raho)
  {
    /* Make it a cheap idle handoff for now */
    mccidl_cheap_idle_ho = TRUE;
  }

  /* -----------------------------------------------------
  ** If we are already on the new_pilot and, for JSTD-008,
  ** the new_freq, there is no need to hand off
  ** ----------------------------------------------------- */
  if ((cur_bs_ptr->pilot_pn == new_pilot) && (cdma.cdmach == new_freq)
      && (cdma.band_class == new_band_class))
  {
    /*-------------------------------------------------------------------
    ** During AEHO handoff state, If the Current Pilot is strong enough,
    ** proceed with Accessing the System. Proceed to the Update Overhead
    ** Information state on the Active pilot and access it.
    **------------------------------------------------------------------*/
    if (cdma.ho_state == CDMA_AEHO)
    {
      new_state = mccidl_check_ovhd(CDMA_UPDATE_OVH, cdma.ho_sa_tc_state); /*lint !e641 */
    }

    return (new_state);
  }

  /* Get neighbor index for new pilot */
  i = mccidl_get_nghbr_index(new_pilot, new_freq, new_band_class);

  /* Pilot found in the neighbor list */
  if (i != -1)
  {
    /* Get neighbor config for new pilot */
    nghbr_config = cur_bs_ptr->nl.nset[i].config;

    #ifdef FEATURE_IS2000_REL_A
    nghbr_bcch_supported = cur_bs_ptr->nl.nset[i].bcch_supported;
    td_supported = FALSE;

    if (OVHD_CHAN_IS_PCH && nghbr_bcch_supported)
    {
      MSG_MED("HO to pilot with BCCH_SUPPORTED=1, exit to SD", 0, 0, 0);
      nghbr_config = 3; /* Mark handoff as new system handoff */
    }

    #ifdef FEATURE_IS2000_REL_A_CC
    old_td_mode = cdma.td_mode;
    old_td_power_level = cdma.td_power_level;

    if (OVHD_CHAN_IS_BCCH && cur_bs_ptr->nl.nset[i].add_pilot_rec_incl &&
        (cur_bs_ptr->nl.nset[i].nghbr_pilot_rec_type == CAI_PILOT_REC_1X_COM_TD))
    {
      if (mccap_is_csch_td_mode_supported(
          cur_bs_ptr->nl.nset[i].nghbr_pilot_rec.rec0.td_mode))
      {
        td_supported = TRUE;
        td_mode = cur_bs_ptr->nl.nset[i].nghbr_pilot_rec.rec0.td_mode;
        td_power_level = cur_bs_ptr->nl.nset[i].nghbr_pilot_rec.rec0.td_power_level;
      }
      else
      {
        MSG_MED("HO to pilot with TD mode=%d, exit to SD",
          cur_bs_ptr->nl.nset[i].nghbr_pilot_rec.rec0.td_mode, 0, 0);
        nghbr_config = 3; /* Mark handoff as new system handoff */
      }
    }
    #endif /* FEATURE_IS2000_REL_A_CC */
    #endif /* FEATURE_IS2000_REL_A */

    /*-----------------------------------------------------------------
    ** If the Mobile is performing Access Entry Handoff, make sure the
    ** AEHO can be performed in the following scenarios
    ** a) If Mobile is CDMA_ORIG, CDMA_REG and CDMA_MEG substate.
    ** b) If Mobile is PAGE_RES substate & pilot is allowed for AEHO.
    ** c) In CDMA_ORD_RES substate & pilot is allowed for AEHO.
    ** d) If Mobile is DCA Update Overhead indication & pilot is 
    **    allowed for AEHO.
    **----------------------------------------------------------------*/
    if (cdma.ho_state == CDMA_AEHO)
    {
      if (P_REV_IS_4_OR_GREATER && 
          ((cdma.ho_sa_tc_state == CDMA_ORIG) ||  /*lint !e641 */
           ((cdma.ho_sa_tc_state == CDMA_PAGE_RES) && /*lint !e641 */
            cur_bs_ptr->nl.nset[i].access_entry_ho) ||
           #ifdef FEATURE_IS2000_REL_D
#error code not present
           #endif /* FEATURE_IS2000_REL_D */
           ((cdma.ho_sa_tc_state == CDMA_ORD_RES) &&  /*lint !e641 */
            cur_bs_ptr->nl.nset[i].access_entry_ho) ||
           (cdma.ho_sa_tc_state == CDMA_REG) || /*lint !e641 */
           (cdma.ho_sa_tc_state == CDMA_MSG)))  /*lint !e641 */
      {
        MSG_MED("Pilot %d is allowed for AEHO", new_pilot, 0, 0);
        /*-------------------------------------------------------------
        ** The Mobile succeeded in Performing Access Handoff to the new
        ** pilot. Set the state of MS to Update Overhead Information
        ** and continue into the System Access state.
        -------------------------------------------------------------*/
        new_state = mccidl_check_ovhd(CDMA_UPDATE_OVH, cdma.ho_sa_tc_state); /*lint !e641 */  
        
        /* Mark the message that received before AEHO and is pending for
           response as received from new active pilot. Do so to ensure  
           that mccidl_check_chan() will honor the system access for the 
           message response */
        mccidl_save_msg_rxed_chan();
      }
      else
      {
        MSG_MED("Pilot %d AEHO not Allowed", new_pilot, 0, 0);
        new_state = mccidl_check_ovhd(CDMA_UPDATE_OVH, cdma.ho_sa_tc_state); /*lint !e641 */
        return (new_state);
      }
    }

    if (cdma.ho_state == CDMA_AHO)
    {
      if (!P_REV_IS_4_OR_GREATER ||
          !cur_bs_ptr->nl.nset[i].access_ho_allowed ||
          !mccsa_is_in_access_ho_list(cur_bs_ptr->nl.nset[i].pilot_pn))
      {
        MSG_MED ("Pilot %d AHO not Allowed", new_pilot, 0, 0);
        return (new_state);
      }
      else
      {
        MSG_MED ("Pilot %d is allowed for AHO", new_pilot, 0, 0);
      }

      #ifdef FEATURE_GPSONE_UPDATE_BSID
#error code not present
      #endif /* FEATURE_GPSONE_UPDATE_BSID */
    }

    if (cdma.ho_state == CDMA_APHO)
    {
      /* Check if the Access Probe Handoff to this pilot is allowed */

      if (!P_REV_IS_4_OR_GREATER || !cur_bs_ptr->nl.nset[i].access_ho_allowed)
      {
        MSG_MED("Pilot %d APHO not allowed", new_pilot, 0, 0);
        return new_state;
      }

      /* If the control is here, then MS can perfrom APHO to new_pilot */
      MSG_MED("APHO Allowed to Pilot %d", new_pilot, 0, 0);

      #ifdef FEATURE_GPSONE_UPDATE_BSID
#error code not present
      #endif /* UPDAT_BSID */
    }
  }
  /* Pilot not found in neighbor list. */
  else
  {
    /* Do not honour any AEHO, APHO and AHO.*/
    if (cdma.ho_state == CDMA_AEHO)
    {
      MSG_MED("Pilot %d AEHO not Allowed", new_pilot, 0, 0);
      new_state = mccidl_check_ovhd(CDMA_UPDATE_OVH, cdma.ho_sa_tc_state); /*lint !e641 */
      return (new_state);
    }

    if (cdma.ho_state == CDMA_AHO)
    {
      MSG_MED ("Pilot %d AHO not Allowed", new_pilot, 0, 0);
      return (new_state);
    }

    if (cdma.ho_state == CDMA_APHO)
    {
      MSG_MED("Pilot %d APHO not allowed", new_pilot, 0, 0);
      return new_state;
    }
  }

  if (cdma.ho_state == CDMA_IDLE_HO)
  {
    MSG_HIGH("Performing IDLE HO to Pilot %d, %d %d", new_pilot, new_freq,
      new_band_class);

    /* Increment number of idle handoffs */
    if ((parm_stat = parm_inc(CAI_PAG_7_ID, 1)) != PARM_DONE_S)
    {
      MSG_HIGH("Invalid parameter increment: status %d", parm_stat,0,0 );
    }
  }
  else if (cdma.ho_state == CDMA_AEHO)
  {
    MSG_HIGH("Performing AEHO to Pilot %d %d %d", new_pilot, new_freq,
      new_band_class);
  }
  else if (cdma.ho_state == CDMA_AHO)
  {
    MSG_HIGH("Performing AHO to Pilot %d %d %d", new_pilot, new_freq,
      new_band_class);
  }
  else if (cdma.ho_state == CDMA_APHO)
  {
    MSG_HIGH("Performing APHO to Pilot %d %d %d", new_pilot, new_freq,
      new_band_class);
  }

  /* Event reporting IHO, APHO or AHO */
  mclog_report_event_csch_ho( cdma.ho_state, mccidl_cheap_idle_ho,
    new_pilot, new_freq, new_band_class, ecio_old_pilot, ecio_new_pilot, ho_type );

  if (!mccidl_fast_raho)
  {
    /* Update the current frequency, band class and freq block/serv-sys,
       if SRCH changed it */
    if ((cdma.band_class != new_band_class) || (cdma.cdmach != new_freq))
    {
      if (cdma.band_class != new_band_class)
      {
        cdma.band_class = new_band_class;
      }

      if (cdma.cdmach != new_freq)
      {
        cdma.cdmach = new_freq;
	inter_freq_handoff = TRUE;
      }

      cdma.block_or_sys = (uint16) sdprl_map_chan_to_blksys(SD_MODE_CDMA, 
        (sd_band_e_type) cdma.band_class, cdma.cdmach); 
        // func returns 32bit value, but values are low enough that 16 bits is OK
    }
  }

  /* -----------------------------------------------------------
  ** Do appropriate kind of handoff according to neighbor config
  ** value as specified in IS-95A section 6.6.2.1.4.2
  ** ----------------------------------------------------------- */
  switch (nghbr_config)
  {
    case 0:     /* use current configuration */
      break;

    case 1:     /* Change to Primary Paging Channel of current CDMA channel */
      bs_info_id.ovhd_chan = CAIX_PC;
      cdma.pagech = PRI_PAGECH;
      cdma.page_chan_s = 1;

      #ifdef FEATURE_IS2000_REL_A_CC
      if (OVHD_CHAN_IS_BCCH)
      {
        /* Set default value for Paging Channel data rate */
        cdma.prat = CAI_PC_1_RATE;
      }
      #endif /* FEATURE_IS2000_REL_A_CC */
      break;

    case 2:
      #ifdef FEATURE_IS2000_REL_A_CC
      if (OVHD_CHAN_IS_BCCH)
      {
        /* Force the hashing of Forward Common Control Channel */
        cdma.fccch_id = PRI_FCCCH;
        cdma.num_fccch_s = 1;
      }
      else
      #endif /* FEATURE_IS2000_REL_A_CC */
      {
        /* Force the hashing of Paging Channel */
        cdma.pagech = PRI_PAGECH;
        cdma.page_chan_s = 1;
      }
      break;

    case 3:
      /* Event report: New system idle handoff */
      if (cdma.ho_state == CDMA_IDLE_HO)
      {
        event_report(EVENT_NEW_SYSTEM_IDLE_HANDOFF);
      }

      if (mccidl_fast_raho)
      {
        /* It should never happen */
        ERR("Fast RAHO to pn=%d config=3", new_pilot, 0, 0);
        /* ----------------------------------------------
        ** If the NGHBR_CONFIG field is '011', the mobile
        ** station shall enter System Determination with
        ** a new system indication.
        ** ---------------------------------------------- */
        new_state     = CDMA_EXIT;
        mcc_exit.exit = MCC_NEW_SYSTEM;
      }
      else
      {
        /* ----------------------------------------------
        ** Inform system determination of new system exit
        ** ---------------------------------------------- */
        if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_opr_new_sys((sd_band_e_type) new_band_class,
          (sd_chan_type) new_freq, NULL)))
        {
          /* ----------------------------------------------
          ** If the NGHBR_CONFIG field is '011', the mobile
          ** station shall enter System Determination with
          ** a new system indication.
          ** ---------------------------------------------- */
          new_state     = CDMA_EXIT;
          mcc_exit.exit = MCC_NEW_SYSTEM;
        }
      }
      break;

    #ifdef FEATURE_IS2000_REL_A_CC
    case 4:
      if (OVHD_CHAN_IS_BCCH)
      {
        /* Force the hashing of Forward Common Control Channel */
        cdma.fccch_id = PRI_FCCCH;
        cdma.num_fccch_s = 1;
      }
      else
      {

        MSG_HIGH("Bogus neighbor config", 0, 0, 0);
        /* ----------------------------------------------
        ** Inform system determination of new system exit
        ** ---------------------------------------------- */
        if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
            sd_ss_ind_cdma_opr_new_sys((sd_band_e_type) new_band_class,
            (sd_chan_type) new_freq, NULL)))
        {
          /* ----------------------------------------------
          ** If the NGHBR_CONFIG field is '011', the mobile
          ** station shall enter System Determination with
          ** a new system indication.
          ** ---------------------------------------------- */
          new_state     = CDMA_EXIT;
          mcc_exit.exit = MCC_NEW_SYSTEM;
        }
      }
      break;
      #endif /* FEATURE_IS2000_REL_A_CC */

    default:
      MSG_HIGH("Bogus neighbor config", 0, 0, 0);
      /* ----------------------------------------------
      ** Inform system determination of new system exit
      ** ---------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_opr_new_sys((sd_band_e_type) new_band_class,
          (sd_chan_type) new_freq, NULL)))
      {
        /* ----------------------------------------------
        ** If the NGHBR_CONFIG field is '011', the mobile
        ** station shall enter System Determination with
        ** a new system indication.
        ** ---------------------------------------------- */
        new_state     = CDMA_EXIT;
        mcc_exit.exit = MCC_NEW_SYSTEM;
      }
      break;
  }

  if (MAIN_STATE(new_state) != CDMA_EXIT) /* Not doing NEW_SYSTEM_EXIT */
  {
    mccidl_old_bs_sid = cur_bs_ptr->csp.sp.sid;
    mccidl_old_bs_nid = cur_bs_ptr->csp.sp.nid;

    if (mccidl_fast_raho && ((nghbr_config == 1)
                          || (nghbr_config == 2)
        #ifdef FEATURE_IS2000_REL_A_CC
        || (OVHD_CHAN_IS_BCCH && (nghbr_config == 4))
        #endif /* FEATURE_IS2000_REL_A_CC */
       ))
    {
      /* Fast RAHO to a neighbor with NBR_CFG = 1 or 4. At this point,
       * SRCH already loaded the PC long code mask of hashed Page channel
       * or Forward Common Control Channel for the new neighbor BS; i.e.,
       * function mcc_srch_get_nbr_info() has been called. Sync with SRCH now.
      */

      /* Fast RAHO to a neighbor with NBR_CFG = 1 or 4. At this point,
       * Page channel or Forward Common Control Channel has been hashed
       * based on neighbor's System Parameter Message or MC-RR Parameter Message
       * for both MC and SRCH. Sync cdma.pagech or cdma.fccch_id as well
       * before mccidl_proc_sys_parm(). Otherwise, cdma.pagech or
       * cdma.fccch_id remains 1.
       */

      if (mcc_srch_get_nbr_info(new_pilot, &mcc_srch_buf.chan_config))
      {
        #ifdef FEATURE_IS2000_REL_A_CC
        if (mcc_srch_buf.chan_config.chan_info[0].chan_id == CAI_F_CCCH)
        {
          cdma.fccch_id =
            mcc_srch_buf.chan_config.chan_info[0].config.fccch.fccch_id;

          MSG_MED( "On FCCCH %d", cdma.fccch_id, 0, 0 );
        }
        else
        #endif /* FEATURE_IS2000_REL_A_CC */
        if (mcc_srch_buf.chan_config.chan_info[0].chan_id == CAI_PCH)
        {
          cdma.pagech =
            (byte) mcc_srch_buf.chan_config.chan_info[0].config.pch.walsh_chn;

          MSG_MED( "On PCH %d", cdma.pagech, 0, 0 );
        }
      }
      else
      {
        /* We should never get here */
        MSG_ERROR("Invalid walsh channel ID", 0, 0, 0);
      }
    }

    /* -------------------------------------------
    ** Update the current base station information
    ** and re-process any overhead info we have.
    ** ------------------------------------------- */

    old_bs_ptr = cur_bs_ptr; /* Save pointer to old BS info */

    bs_info_id.pilot_pn = new_pilot;
    bs_info_id.cdma_ch = new_freq;
    bs_info_id.band_class = new_band_class;
    bs_info_id.ovhd_walsh_ch =
                          #ifdef FEATURE_IS2000_REL_A_CC
                          (bs_info_id.ovhd_chan == CAIX_BCCH) ? cdma.bcch :
                          #endif /* FEATURE_IS2000_REL_A_CC */
                          cdma.pagech;

    #ifdef FEATURE_IS2000_REL_A_CC
    /* Update BS's TD configuration */
    cdma.td_mode = td_mode;
    cdma.td_power_level = td_power_level;
    if (cur_bs_ptr->td_supported)
    {
      cdma.bcch = cdma.bcch_td;
      cdma.brat = cdma.brat_td;
      cdma.bcch_code_rate = cdma.bcch_code_rate_td;
    }
    else
    {
      cdma.bcch = cdma.bcch_non_td;
      cdma.brat = cdma.brat_non_td;
      cdma.bcch_code_rate = cdma.bcch_code_rate_non_td;
    }
    #endif /* FEATURE_IS2000_REL_A_CC */

    mcc_entry_init(&bs_info_id, td_supported);

    #ifdef FEATURE_IS2000_REL_D
#error code not present
    #endif /* FEATURE_IS2000_REL_D */

    if (!mccidl_fast_raho)
    {
      /* Notify RXC to skip frames */
      mcc_idle_ho_rx();
  
      /* Update Searcher's active set */
      mccidl_send_srch_idle_aset();
    }

    /* Reprocess stored overheads if any. */
    new_state = mccidl_process_stored_ovhds(new_state);

    /* -------------------------------------------------------------------
    ** MS perform channel monitoring supervision to see whether it needs
    ** to switch between PCH/BCCH/FCCCH.
    ** ------------------------------------------------------------------- */
    mccidl_supervise_channel_monitoring(new_state, NULL, FALSE);

    /* ---------------------------------------------------------------
    ** If the Mobile is performing AHO or APHO to a Base station whose
    ** configuration parameters are current, force acc_msg_seq to NULL
    ** to make sure that the Mobile has current ACC_MSG_SEQ on new BS.
    ** --------------------------------------------------------------- */
    if ((cdma.ho_state == CDMA_AHO) || (cdma.ho_state == CDMA_APHO))
    {
      #ifdef FEATURE_IS2000_REL_A_CC
      if (OVHD_CHAN_IS_BCCH)
      {
        cur_bs_ptr->eapm.acc_msg_seq = INVALID_SEQ;
      }
      else
      #endif /* FEATURE_IS2000_REL_A_CC */
      {
        cur_bs_ptr->ap.acc_msg_seq = INVALID_SEQ;
      }
    }

    /* ------------------------------------------------------------------
    ** If we don't have a stored neighbor list for this new base station,
    ** but we do have one from the last base station we were on, we will
    ** build a temporary one from the old information.
    ** ------------------------------------------------------------------ */
    mccidl_build_temp_nl(old_bs_ptr, old_td_mode, old_td_power_level,
      nghbr_config, nghbr_bcch_supported);

    /* ------------------
    ** Access in Progress
    ** ------------------ */
    if (P_REV_IS_4_OR_GREATER && mccsa_access_attempt_in_progress())
    {
      /* -----------------------------------------------------------------
      ** Set Previous pilot and its neighbor record in Access Attempt List
      ** ----------------------------------------------------------------- */
      mccsa_set_prev_pilot_info(old_bs_ptr);
    }

    if (!mccidl_fast_raho)
    {
      /* Update Searcher's neighbor set */
      mccidl_send_srch_idle_nset();

      if (new_state != CDMA_EXIT)
      {
        /* ----------------------------------------------------------------
        ** Do not set the CDMA state to "CDMA_IDLE_INIT" if fast RAHO is in
        ** progress.
        ** -------------------------------------------------------------- */
        if ( cdma.ho_state == CDMA_IDLE_HO )
        {
          new_state = CDMA_IDLE_INIT; /*lint !e641 */
          /* Start the BS insanity timer */
          mccidl_set_bs_insanity_timer();

          /* Clear config_msg_seq to make sure overhead is current before
             next sleep. */
          MSG_MED("HO clr cfg seq", 0, 0, 0); 
          cur_bs_ptr->rx.config_msg_seq = INVALID_SEQ;
        }
      }
    }

    /* Set mccidl_sleep_ok to TRUE so the mobile can sleep immediately
    ** after the OVHD_MSG_OK, instead of waiting until receiving a
    ** General Page Message or a Slotted page message which says sleep_ok.
    ** This is also safe for a Fast RAHO during sleep since
    ** mccidl_sleep_ok will be set to FALSE when a quick page match
    ** fails.
    */
    mccidl_sleep_ok = TRUE;

    /* If we allow sleep here, we need to set the sleep slot mask. */
    /* Usually sleep_slot_mask is set when receiving RXC_SLOT_R,   */
    /* or the general page message with done bits. But we need to  */
    /* do it here to allow the mobile to sleep immediately.        */
    if ( !mccidl_fast_raho )
    {
      if (sleep_slot_mask == 0)
      {
        sleep_slot_mask = PAGE_SLOT;
        MSG_MED("HO: set sleep mask %d", sleep_slot_mask, 0, 0);
      }
    }
  } /* End of if (MAIN_STATE( new_state ) != CDMA_EXIT) */

  if (mccidl_fast_raho)
  {
    /* -------------------------------------------------------
    ** Set the cheap handoff flag to FALSE if the Overhead
    ** Messages are not current on the current Basestation
    ** or the SID & NID doesn't match with the new BS
    ** ----------------------------------------------------- */
    if ((new_state == CDMA_EXIT) ||
        !mcc_is_ovhd_cfg_okay() ||
        (mccidl_old_bs_sid != cur_bs_ptr->csp.sp.sid) ||
        (mccidl_old_bs_nid != cur_bs_ptr->csp.sp.nid)
       )
    {
      /* Not a cheap idle handoff */
      mccidl_cheap_idle_ho = FALSE;
      MSG_MED("New State: %d, OVHD_OK=%d,Cheap=%d", new_state,
              mcc_is_ovhd_cfg_okay(), mccidl_cheap_idle_ho);
    }

    if (MAIN_STATE( new_state ) == CDMA_IDLE)
    {
      if (!mccidl_cheap_idle_ho)
      {
        /* -----------------------------
        ** Reset Idle State to CDMA_INIT
        ** ----------------------------- */
        new_state = CDMA_IDLE_INIT; /*lint !e641 */
      }
    } /* end if (MAIN_STATE(new_state) == CDMA_IDLE) */
  } /* end if (mccidl_fast_raho) */

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    db_item.code_channel = cdma.bcch;
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */
  {
    db_item.code_channel = cdma.pagech;
  }

  db_put(DB_CODE_CHANNEL, &db_item);

  db_item.pilot_pn_off = new_pilot;
  db_put(DB_PILOT_PN_OFF, &db_item);

  /* Continues in slotted mode after idle handoff */
  if (cdma.ho_state == CDMA_IDLE_HO)
  {
    if ((new_state == old_state) && (old_state == CDMA_SLOTTED)) /*lint !e641 */
    {
      /* Events report: Slotted mode operation */
      slotted_mode = TRUE;
      event_report_payload(EVENT_SLOTTED_MODE_OPERATION,
        sizeof(event_slotted_mode_operation_type), &slotted_mode);

      #ifndef FEATURE_IS2000_REL_A
      /* Events report: QPCH in use */
      if (!mccap_item_is_supported(MCCAP_DF_QPCH) &&
          !mccap_is_enhanced_operation_supported())
      {
        qpch_in_use = FALSE;
      }
      else if (!P_REV_IS_6_OR_GREATER || !cur_bs_ptr->csp.sp.ext_sys_parameter)
      {
        qpch_in_use = FALSE;
      }
      else
      {
        qpch_in_use = cur_bs_ptr->csp.esp.qpch_supported;
      }

      event_report_payload(EVENT_QPCH_IN_USE,
        sizeof(event_qpch_in_use_type), (void *) &qpch_in_use);
      #endif /* !FEATURE_IS2000_REL_A */

    } /* end if ((new_state == old_state) && (old_state == CDMA_SLOTTED)) */
  } 

  /* Increment the logging parameter INTER_FREQ_IDLE_HO_COUNT */
  if( (new_state != CDMA_EXIT) && inter_freq_handoff )
  {
    mclog_inc_nw_param(INTER_FREQ_IDLE_HO_COUNT);
  }

  return (new_state);

} /* mccidl_handoff */

/*===========================================================================

FUNCTION MCCIDL_WAKEUP

DESCRIPTION
  This procedure sends commands to RXC to have it transition
  to the Idle State after sleep.

DEPENDENCIES
  RXC should be in sleep state

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_wakeup
(
  boolean full_wakeup,
  boolean page_match, /*lint -esym(715,page_match) */
  byte slot_mask,
  word slot /* the slot RXC is waking up to */
)
{
  mccrxtx_cmd_type *rxtx_ptr;
    /* Pointer to item from mcc_rxtx_q */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------------------------------------------------------------
  ** If MS is sleeping, all received OTA message in the queue is empty GPM 
  ** as guaranteed by fast page match algorithm. So it makes sense to drop 
  ** them. However, if MS is awake, it shall not drop any OTA message.
  ** ----------------------------------------------------------------------- */
  if (mccidl_sleep_active)
  {
    /* ------------------------------------------------------------
    ** Clear the receive message queue in case something was queued
    ** up just before we went to sleep (i.e. a slotted page with
    ** more_pages = 0 which would erroniously put us back to sleep)
    ** ------------------------------------------------------------ */
    while ((rxtx_ptr = (mccrxtx_cmd_type*) q_get(&mcc_rxtx_q)) != NULL)
    {
      q_put( rxtx_ptr->hdr.cmd_hdr.done_q_ptr, &rxtx_ptr->hdr.cmd_hdr.link);
    }
  }

  /* send a command to Receive to stop sleep processing */

  if (!full_wakeup)
  {
    mcc_rx_buf.wake.hdr.command = RXC_SLOT_F;
  }
  else /* Full wakeup desired */
  {
    mcc_rx_buf.wake.hdr.command = RXC_WAKE_F;
  }
  mcc_rx_buf.wake.idle_pgslot = idle_pgslot;
  mcc_rx_buf.wake.sci_mask = sci_mask;
  mcc_rx_buf.wake.hdr.cmd_hdr.done_q_ptr = NULL;
  mcc_rx_buf.wake.slot_mask = slot_mask;
  mcc_rx_buf.wake.slot = slot;

  #ifdef FEATURE_PAGE_MATCH_REQUEST
  mcc_rx_buf.wake.page_match = page_match;
  #endif /* FEATURE_PAGE_MATCH_REQUEST */

  MSG_LOW("RXC_WAKE_F, mask %d slot 0x%x", slot_mask, slot, 0);

  mcc_rxc_cmd(&mcc_rx_buf);

} /* mccidl_wakeup */

/*===========================================================================

FUNCTION MCCIDL_PROC_SRCH_RPT

DESCRIPTION
  This procedure processes the idle search reports.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_proc_srch_rpt
(
  mccsrch_rpt_type *rpt_ptr,
    /* Report received from searcher */
  word new_state
    /* Next state to be processed */
)
{
  int i, j; 
    /* used as local loop variable for system reselection */
  sd_sys_s_type sys_list[CAI_GEN_ANALOG_NGHBR_MAX];
    /* List of systems for reselection */

  mccsrch_rpt_type *tmp_ptr; 
    /* Report received from searcher */

  boolean init_aeho;
    /* Flag indicates whether AEHO is to be initiated */

  switch (rpt_ptr->hdr.rpt)
  {
    /* -------------------------------------------------------------
    ** Check to see if report generated by searcher is an Idle Pilot
    ** Measurement Report.  If the searcher is already asleep, wait
    ** for it to wake up and report the idle handoff again.
    ** ------------------------------------------------------------- */
    case SRCH_PC_MEAS_R:
      MSG_HIGH("Received SRCH_PC_MEAS_R report", 0, 0, 0);

      if ( ( cdma.ho_state == CDMA_AEHO ) || cdma.transceiver_request_pending )
      {
        MSG_HIGH("SRCH_PC_MEAS_R is unexpected, ignore it.", 0, 0, 0);
      }
      else if ( !mccidl_sleep_active )
      {
        /* -------------------------------------------------------------
        ** JSTD008 feature definition was replaced by the following code
        ** -------------------------------------------------------------*/
        new_state = mccidl_handoff((word) rpt_ptr->pc_meas.pilot,
                                   (word) rpt_ptr->pc_meas.freq,
                                   (byte) rpt_ptr->pc_meas.band_class,
                                   new_state,
                                   (int8) rpt_ptr->pc_meas.ecio_old_pilot,
                                   (int8) rpt_ptr->pc_meas.ecio_new_pilot,
                                   CDMA_IDLE_HO);
      }
      break;

    case SRCH_IDLE_STRONG_PILOT_MEAS_R:
      MSG_HIGH("Received SRCH_IDLE_STRONG_PILOT_MEAS_R report", 0, 0, 0);  
       
      if ( !mccidl_sleep_active )
      {
        new_state = mccidl_handoff((word) rpt_ptr->idle_strong_pilot_meas.strongest_pilot,
                                   (word) rpt_ptr->idle_strong_pilot_meas.strongest_freq,
                                   (byte) rpt_ptr->idle_strong_pilot_meas.strongest_band_class,
                                   new_state,
                                   (int8) rpt_ptr->idle_strong_pilot_meas.active_ecio / 4,
                                   (int8) rpt_ptr->idle_strong_pilot_meas.strongest_ecio / 4,
                                   CDMA_IDLE_HO);
      }
      break;

    case SRCH_ACCESS_READY_R:
      MSG_HIGH("Received searcher report of transceiver available", 0, 0, 0);

      /* clear the callback timer first */
      (void) mcc_clr_callback_timer(&mcc_tr_request_timer, MCC_TR_REQUEST_TIMER_ID);

      if ( !cdma.transceiver_request_pending )
      {
        MSG_MED("Pending access request was timout on transceiver resource, ignore SRCH_ACCESS_READY_R report", 0, 0, 0);
        break;
      }

      /* Clear the transceiver_request_pending flag */
      cdma.transceiver_request_pending = FALSE;

      /* Default not to initate AEHO */
      init_aeho = FALSE;

      /* Check to see if AEHO has to be initated */
      if ( P_REV_IS_4_OR_GREATER && 
           mcc_is_aeho_ovhd_msg_okay()&& 
           mccap_item_is_supported( MCCAP_DF_AEHO ) 
           #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
           #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */
         )
      {
        switch ( cdma.ho_sa_tc_state )
        {
          case CDMA_ORIG:
            /* --------------------------------------------------------
            ** Due to the Pilot Reporting, MC waits for SRCH to finish
            ** a search of its neighbors before going to Update OVHD
            ** Information state. Thus SRCH has all the strengths at
            ** the time of Pilot Reporting while accessing the System.
            ** ------------------------------------------------------ */
            init_aeho = TRUE;
            break;

          #ifdef FEATURE_IS2000_REL_D
#error code not present
          #endif /* FEATURE_IS2000_REL_D */

          case CDMA_PAGE_RES:
            if ( cur_bs_ptr->csp.esp.nghbr_set_entry_info )
            {
              init_aeho = TRUE;
            }
            break;

          case CDMA_ORD_RES:
            if ( cur_bs_ptr->csp.esp.acc_ent_ho_order ) 
            {
              init_aeho = TRUE;
            }
            break;
 
          default:
            break;
        }
      }

      /* -------------------------------------------------------------
      ** Decided to initiate AEHO. Mobile waits for SRCH report for an 
      ** AEHO and then puts MC into the Update Overhead information 
      ** state.
      ** ------------------------------------------------------------- */
      if ( init_aeho )
      {
        mccidl_init_aeho_info( cdma.ho_sa_tc_state );
      }
      /* --------------------------------------------------
      ** Put MC into the Update Overhead information state.
      ** -------------------------------------------------- */
      else
      {
        new_state = mccidl_check_ovhd( new_state, cdma.ho_sa_tc_state );
      } 

      break;
    
    case SRCH_RAFAIL_R:    /* Failed to reacquire after sleep report */
      if ( !rpt_ptr->rafail.require_system_exit && (mccidl_rafail_retry_cnt > 0) )
      {   
        MSG_HIGH("Stay asleep after RAFAIL retry=%d",mccidl_rafail_retry_cnt,0,0);
        mccidl_rafail_retry_cnt--;

        /* Since we got a RAFAIL, we will get a page match status
        ** report from SEARCH next. Wait for this and discard it 
        */
        (void) mcc_wait( MCC_SRCH_Q_SIG );

        if ((tmp_ptr = (mccsrch_rpt_type *)q_get( &mcc_srch_q )) != NULL)
        {   
          if ( tmp_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
          {
            /* -------------------------------------------------------------
            ** Searcher requested system restart. Need to do system exit .
            ** Inform system determination of system lost and determines the
            ** next state to be processed based on the returned SS-Action.
            ** ------------------------------------------------------------- */
            if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                    sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_ADVISE_UNLOCK_RF, NULL ) ) 
               )
            {
              MSG_HIGH( "SRCH request sys restart",0,0,0 );
              mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
              new_state = CDMA_EXIT;
            }
          }

          else
          {
            MSG_HIGH( "Discarding page match message",0,0,0 );
          }

          /* clean up report processing */
          cmd_done( &tmp_ptr->hdr.rpt_hdr );
        }

        if ( new_state != CDMA_EXIT )
        {
          /* Go back to sleep again without a complete wakeup */
          mccidl_sleep_state = MCC_SLEEP;

          /* Send a command to the Search task to sleep. 
          ** Treat it as an assigned slot. this
          ** might need optimization later 
          */
          mcc_srch_buf.sleep.hdr.cmd = SRCH_SLEEP_F;
          mcc_srch_buf.sleep.hdr.cmd_hdr.task_ptr = NULL;
          mcc_srch_buf.sleep.hdr.cmd_hdr.done_q_ptr = NULL;
          mcc_srch_buf.sleep.cur_slot_mask = PAGE_SLOT;
          mcc_srch_cmd(&mcc_srch_buf);

          /* Search is going to sleep at this point
          ** Tell RXC to go night-night, too
          */
          mcc_rx_buf.sleep.hdr.command = RXC_SLEEP_F;
          mcc_rx_buf.sleep.hdr.cmd_hdr.done_q_ptr = NULL;
          mcc_rxc_cmd( &mcc_rx_buf);
        }

      }

      else
      {
        #ifndef FEATURE_ERRLOG_DEBUG
        MSG_HIGH ( "Lost Acquisition after sleep!, %d", 
          rpt_ptr->rafail.require_system_exit, 0, 0);
        #else /* FEATURE_ERRLOG_DEBUG */
        ERR ( "Lost Acquisition after sleep!, %d", 
          rpt_ptr->rafail.require_system_exit, 0, 0);
        #endif /* FEATURE_ERRLOG_DEBUG */

        /* -------------------------------------------------------------
        ** Inform system determination of system lost and determines the
        ** next state to be processed based on the returned SS-Action.
        ** ------------------------------------------------------------- */
        if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_UNKNOWN, NULL ) )
           )
        {
          mcc_exit.exit = MCC_SYSTEM_LOST;
          new_state = CDMA_EXIT;
        }

        /* Events report: Loss of acquisition after sleep */
        event_report(EVENT_LOSS_OF_ACQ_AFTER_SLEEP);
      }

      break;

    case SRCH_THRESH_R:     /* Pilot power below threshold, exit to SD */
      MSG_HIGH ( "Pilot power below threshold!", 0, 0, 0 );

      memset(sys_list, 0, sizeof(sd_sys_s_type) * CAI_GEN_ANALOG_NGHBR_MAX);  
      /* -----------------------------------------------------------------
      ** Inform system determination of system re-selection and determines
      ** the next state to be processed based on the returned SS-Action.
      ** ----------------------------------------------------------------- */
      for (i=0, j=0; i < cur_bs_ptr->anl.num_analog_nghbr; i++)
      {
        if (cur_bs_ptr->anl.anl_nghbr[i].sys_a_b & 0x01)
        {
          sys_list[j].mode = SD_MODE_AMPS;
          sys_list[j].band = cur_bs_ptr->anl.anl_nghbr[i].band_class;
          sys_list[j].chan = SD_CELL_SYS_A; /*lint !e641 */
          j++;
        }

        if (cur_bs_ptr->anl.anl_nghbr[i].sys_a_b & 0x02)
        {
          sys_list[j].mode = SD_MODE_AMPS;
          sys_list[j].band = cur_bs_ptr->anl.anl_nghbr[i].band_class;
          sys_list[j].chan = SD_CELL_SYS_B; /*lint !e641 */
          j++;
        }
      }

      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_opr_sys_resel(sys_list, j, NULL)))
      {
        mcc_exit.exit = MCC_SYSTEM_RESELECTION ;
        new_state = CDMA_EXIT;
      }
      else
      {
        /* Tell SRCH to continue on current system */
        mcc_srch_buf.sys_reselect.hdr.cmd = SRCH_RESELECT_MEAS_F;
        mcc_srch_cmd(&mcc_srch_buf);
      }

      /* Events report: System reselection */
      event_report(EVENT_SYSTEM_RESELECTION);
      break;

    case SRCH_RAHO_R:      /* Reacquired, but idle HandOff required report */
      /* have SRCH transition from sleep to Idle state */
      mccidl_send_srch_idle_cmd();

      /* -------------------------------------
      ** Do this last so that tasks transition
      ** between states before handoff
      ** ------------------------------------- */
      /* JSTD008 feature definition was removed */
      MSG_MED("RAHO to pilot %d, freq %d",rpt_ptr->raho.pilot,
                                          rpt_ptr->raho.freq, 0);

      new_state = mccidl_handoff((word) rpt_ptr->raho.pilot,
                                 (word) rpt_ptr->raho.freq,
                                 (byte) rpt_ptr->raho.band_class,
                                 new_state,
                                 (int8) rpt_ptr->raho.ecio_old_pilot,
                                 (int8) rpt_ptr->raho.ecio_new_pilot,
                                 CDMA_RAHO);
      break;

    case SRCH_REACQ_R:    /* Fully reacquired after sleep report */
      /* have SRCH transition from sleep to Idle state */
      mccidl_send_srch_idle_cmd();
      break;

    case SRCH_WAKEUP_R:    /* Waking up from sleep report */

      /* Check if a duplicate wakeup_r received from Search */
      if (qw_cmp(rpt_ptr->wakeup.valid_time, mccidl_sleep_ok_time) == 0)
      {
        MSG_HIGH("Wakeup 0x%x 0x%x already recvd",
                 qw_hi(rpt_ptr->wakeup.valid_time),
                 qw_lo(rpt_ptr->wakeup.valid_time), 0);
      }

      /* Searcher tells us what slot cycle it is really using */
      sci_mask = mcc_calculate_sci_mask(rpt_ptr->wakeup.slot_cycle_index);

      qw_equ(mccidl_sleep_ok_time, rpt_ptr->wakeup.valid_time );
      qw_shift(mccidl_sleep_ok_time, -2);

      MSG_MED("SRCH_WAKEUP_R, mask %d slot 0x%x",
        rpt_ptr->wakeup.cur_slot_mask, qw_lo(mccidl_sleep_ok_time), 0);

      mclog_report_wakeup_slot(rpt_ptr->wakeup.cur_slot_mask,
                               rpt_ptr->wakeup.srch_slept);
      
      /* Based on slot mask of wakeup report, enable or disable MER statistics 
         for page slot. */
      mclog_mer_config((rpt_ptr->wakeup.cur_slot_mask & PAGE_SLOT),
        rpt_ptr->wakeup.valid_time);

      /* Search is sending a wakeup report while already awake.     */
      /* A new slot is coming, check if we need to switch channels. */
      mccidl_supervise_channel_monitoring(new_state, &rpt_ptr->wakeup, FALSE);

      #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      mccbcsms_proc_srch_rpt(rpt_ptr);
      #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

      /* Have RXC transition from sleep to the Paging Channel */
      mccidl_wakeup(FALSE,
                    #ifdef FEATURE_PAGE_MATCH_REQUEST
                    rpt_ptr->wakeup.page_match
                    #else
                    TRUE
                    #endif /* FEATURE_PAGE_MATCH_REQUEST */
                    ,rpt_ptr->wakeup.cur_slot_mask,
                    (word) qw_lo(mccidl_sleep_ok_time));

      /* We use mccidl_sleep_ok_time to calc. slot. 
         Ignore any sleep requests generated before now. */
      qw_equ(mccidl_sleep_ok_time, rpt_ptr->wakeup.valid_time);

      MSG_LOW("Sleep Ok Time = 0x%x", (word) qw_lo(mccidl_sleep_ok_time), 0, 0);

      #ifdef FEATURE_BROADCAST_SMS
      if ((rpt_ptr->wakeup.cur_slot_mask & BC_DATA_SLOT) != 0)
      {
        MSG_HIGH("BCSMS: send schedule to Search early upon wakeup report",0,0,0);
        mccbcsms_sched_bcast_upon_wakeup(rpt_ptr->wakeup.valid_time);
      }
      #endif /* FEATURE_BROADCAST_SMS */

      break;

    case SRCH_TRYSLEEP_R:   /* Retry a previously vetoed sleep */
      /* Searcher previously vetoed a command to sleep.  It is
       * now telling us that we should reattempt the sleep command.
       */
      if ( mccidl_assert_sleep_ok( rpt_ptr->trysleep.rpt_time ) == FALSE)
      {
        MSG_MED("Retry sleep request aborted. Previous slot?",0,0,0);
      }
      break;

    case SRCH_SYS_MEAS_R:
      /* Report system measurements to System Determination */
      mcc_sys_meas_rpt(rpt_ptr);
      /* --------------------------------------------------------------
      ** Inform system determination of done with reporting system
      ** measurement.
      ** Determine the next state to be processed based on the returned
      ** SS-Action.
      ** -------------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
        sd_ss_ind_misc_meas_rprt(rpt_ptr->sys_meas.meas_id, TRUE, NULL)))
      {
        mcc_exit.exit = MCC_RESELECTION;
        new_state = CDMA_EXIT;
        event_report(EVENT_PREF_SYS_RESEL);
      }
      break;

    case SRCH_REQUEST_SYS_RESTART_R:
      /* -------------------------------------------------------------
      ** Searcher requested system restart. Need to do system exit .
      ** Inform system determination of system lost and determines the
      ** next state to be processed based on the returned SS-Action.
      ** ------------------------------------------------------------- */
      if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_ADVISE_UNLOCK_RF, NULL ) ) 
         )
      {
        MSG_HIGH( "SRCH request sys restart",0,0,0 );
        mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
        new_state = CDMA_EXIT;
      }
      break;

    default:
      break;
  }

  /* clean up report processing */
  cmd_done( &rpt_ptr->hdr.rpt_hdr );

  return (new_state);

}/* mccidl_proc_srch_rpt */

/*===========================================================================

FUNCTION MCCIDL_AUTONOMOUS_WAKEUP

DESCRIPTION
  This function checks if MC needs to completely wake up itself at slotted
  wakeup. MC needs a complete wakeup when one of the following condition is
  true:

  1. There is an MC CMD waiting for processing;
  2. One of the registration timers expires.

  Note this function will not change CDMA state.

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if MC needs to wake up itself autonomously;
  otherwise, returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccidl_autonomous_wakeup(void)
{
  boolean need_complete_wakeup = FALSE;
  rex_sigs_type sigs;      /* Signals returned by rex_get_sig */


  /* Get current Sig values for MC */
  sigs = rex_get_sigs(rex_self());

  if (q_check(&mc_cmd_q) != NULL)
  {
    /* Got a CMD. Need to wake up */
    need_complete_wakeup = TRUE;
    MSG_HIGH("MC wakes up due to pending CMD.", 0, 0, 0);
    return(need_complete_wakeup);
  }


  /* Before checking any registration timer, we need to process TMSI
   * and FULL TMSI timer
   */
  if ( (sigs & MCC_CALLBACK_TIMER_SIG) &&
       mcc_timer_on_callback_q(MCC_TMSI_EXP_TIMER_ID)
     )
  {
    /* If valid TMSI code has expired,     */
    /* then reset code and all TMSI timers */
    if (tmsi_expired ())
    {
      /* Reset TMSI code and all timers */
      tmsi_code_and_timer_reset ();
    }
    /* If valid TMSI code has not expired,  */
    /* check if TMSI expiration timer needs */
    /* to be set again to check expiration  */
    /* time.                                */
    else
    {
      /* Set TMSI expiration timer if required */
      tmsi_set_exp_timer ();
    }
  }

  if ( (sigs & MCC_CALLBACK_TIMER_SIG) &&
       mcc_timer_on_callback_q(MCC_FULL_TMSI_TIMER_ID)
     )
  {
    /* If full TMSI timer has expired,     */
    /* then reset code and all TMSI timers */
    MSG_HIGH("Full TMSI timer expired", 0L, 0L, 0L);
    tmsi_code_and_timer_reset ();

    /* Delete SID_NID_LIST and ZONE_LIST */
    /* See J-STD-008, 2.6.5.5.2.5        */
    mccreg_clear_reg();
  }

  /* Check registration status */
  if ( (cdma.reg_type = mccreg_idle_check()) != MCCREG_NO_REG_REQUIRED )
  {
    /* a registration pending. Need to wake up */
    need_complete_wakeup = TRUE;

    /* Set flag mccidl_reg_required, so mccidl_online() can directly
     * enter "CDMA_REG" without calling mccreg_idle_check() again.
     */
    mccidl_reg_required = TRUE;
    MSG_HIGH("MC wakes up due to registration.", 0, 0, 0);
    return(need_complete_wakeup);
  }

  /* If code gets here, no autonomoous wakeup needed */
  MSG_LOW("No Autonomous Wakeup Needed", 0, 0, 0);
  return(need_complete_wakeup);

} /* mccidl_autonomous_wakeup */

/*===========================================================================

FUNCTION MCCIDL_GET_BS_HASH_KEY

DESCRIPTION

  This function get the hash key for the specified entry in BS_INFO

DEPENDENCIES

  BS_INFO has received all overhead configuration messages

RETURN VALUE

  TRUE  - hash key found
  FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccidl_get_bs_hash_key(byte bs_idx, dword *hash_key_ptr)
{
  /* check the bs_idx is within range */
  if (bs_idx >= MC_MAX_BASES)
  {
    return FALSE;
  }

  *hash_key_ptr = bs_info[bs_idx].imsi_hash_key; 

  return TRUE;

} /* mccidl_get_bs_hash_key */

/*===========================================================================

FUNCTION MCCIDL_CHECK_BS_OVHD_CURRENT

DESCRIPTION

  This function check whether the overhead info is current for the specified
  BS

DEPENDENCIES

  None.

RETURN VALUE
  TRUE   - Overhead info is current
  FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccidl_check_bs_ovhd_current(byte bs_idx)
{

  qword current_time, time_diff;
  cdma_bs_type *bs_ptr;


  if (bs_idx >= MC_MAX_BASES)
  {
     return FALSE;
  }

  bs_ptr = &bs_info[bs_idx];

  /* Get current system time in units of 20 msecs */
  ts_get_20ms_frame_time(current_time);

  qw_sub(time_diff, current_time, bs_ptr->rx.msg_time );

  if ((qw_hi(time_diff) != 0) ||
      (qw_lo(time_diff) > (CAI_T31M/20L)))
  {
    /* Last overhead msg received more than 10 minutes */
    return FALSE;
  }

  /* check all overhead msgs are received */
  if (((bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_PC) &&
       (bs_ptr->csp.sp.config_msg_seq  == bs_ptr->rx.config_msg_seq) &&
       (bs_ptr->nl_config_msg_seq  == bs_ptr->rx.config_msg_seq) &&
       (bs_ptr->gnl_config_msg_seq == bs_ptr->rx.config_msg_seq) &&
       (bs_ptr->cl.config_msg_seq  == bs_ptr->rx.config_msg_seq) &&
       (bs_ptr->csp.esp.config_msg_seq == bs_ptr->rx.config_msg_seq) &&
       (bs_ptr->gsd.config_msg_seq == bs_ptr->rx.config_msg_seq) &&
       (bs_ptr->ecl.config_msg_seq  == bs_ptr->rx.config_msg_seq) &&
       (bs_ptr->egsd.config_msg_seq == bs_ptr->rx.config_msg_seq) &&
       bs_ptr->rx.sp_rx  &&
       bs_ptr->rx.nl_rx  &&
       bs_ptr->rx.cl_rx  &&
       bs_ptr->rx.ecl_rx  &&
       bs_ptr->rx.egsd_rx &&
       bs_ptr->rx.gnl_rx &&
       bs_ptr->rx.esp_rx &&
       bs_ptr->rx.gsd_rx)
      #ifdef FEATURE_IS2000_REL_A_CC
      ||
      ((bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_BCCH) &&
       (bs_ptr->a41spm_config_msg_seq  == bs_ptr->rx.config_msg_seq) &&
       (bs_ptr->unlm_config_msg_seq == bs_ptr->rx.config_msg_seq) &&
       (bs_ptr->mcrrpm_config_msg_seq == bs_ptr->rx.config_msg_seq) &&
       (bs_ptr->ecl.config_msg_seq  == bs_ptr->rx.config_msg_seq) &&
       (bs_ptr->egsd.config_msg_seq == bs_ptr->rx.config_msg_seq) &&
       bs_ptr->rx.a41spm_rx  &&
       bs_ptr->rx.unlm_rx &&
       bs_ptr->rx.mcrrpm_rx &&
       bs_ptr->rx.ecl_rx &&
       bs_ptr->rx.egsd_rx)
      #endif /* FEATURE_IS2000_REL_A_CC */
     )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* mccidl_check_bs_ovhd_current */

/*===========================================================================

FUNCTION MCCIDL_GET_NBRCFG_PCH_WALSH

DESCRIPTION
  This function returns paging channel of neighbor BS.

DEPENDENCIES
  None.

RETURN VALUE
  The page channel or broadcast control channel of neighbor BS.

SIDE EFFECTS
  None.

===========================================================================*/

byte mccidl_get_nbrcfg_pch_walsh
(
  word pilot, 
  word freq, 
  byte band
)
{
  byte    walsh_ch = PRI_PAGECH; /* Primary Paging Channel by defualt */
  byte    i;
  dword   hash_key;
  byte    bs_index[MC_MAX_BASES];
  byte    num_bs = 0;

  /* Search base station information array for entry */
  for (i=0; i< MC_MAX_BASES; i++)
  {
    if ((bs_info[i].valid_pn) &&
        (bs_info[i].pilot_pn == pilot) &&
        (bs_info[i].cdma_ch == freq) &&
        (bs_info[i].band_class == band)
        && (bs_info[i].ovhd_chan_info.ovhd_chan == CAIX_PC)
       )
    {
      /* found an entry in bs_info continue for the remaining entries*/
      bs_index[num_bs] = i;
      num_bs++;
    }
    else
    {
      bs_index[i] = 0;
    }
  } /* end of for */

  for (i=0; i < num_bs; i++)
  {
    if (mccidl_check_bs_ovhd_current(bs_index[i]))  
    {
      /* overhead is current */
      if (mccidl_get_bs_hash_key(bs_index[i], &hash_key))
      {
        /* hash key is available */
        walsh_ch = (byte) hash(hash_key, bs_info[bs_index[i]].csp.sp.page_chan,
                              (word) (2 * (hash_key & 0xFFF)) ) + 1;
        break;
      }
    }
  }

  return (walsh_ch);

} /* mccidl_get_nbrcfg_pch_walsh */

/*===========================================================================

FUNCTION MCC_SRCH_GET_ACTIVE_INFO

DESCRIPTION
  This function retrieves active pilot information for overhead channel.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_srch_get_active_info
(
  srch_idle_chan_info_type *chan_info
)
{
#ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    chan_info->chan_id = CAI_BCCH;
    chan_info->config.bcch.walsh_chn = cdma.bcch;
    chan_info->config.bcch.rate = cdma.brat;
    chan_info->config.bcch.c_rate = cdma.bcch_code_rate;
  }
  else
#endif /* !FEATURE_IS2000_REL_A_CC */
  {
    chan_info->chan_id = CAI_PCH;
    chan_info->config.pch.walsh_chn = cdma.pagech;
    chan_info->config.pch.rate = cdma.prat;
  }

} /* mcc_srch_get_active_info */

/*===========================================================================

FUNCTION MCCIDL_GET_BS_INDEX

DESCRIPTION
  This function gets the index of a specified pilot in the stored base station
  info array.
  current.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the index value of this pilot in the stored base station info array.
  "-1" indicates that this pilot is not found in bs_info[].
  None

SIDE EFFECTS
  None.

===========================================================================*/
int2 mccidl_get_bs_index
(
  bs_info_id_type *bs_info_id
    /* Keys that identify the pilot */
)
{
  int2 i;

  for (i=0; i< MC_MAX_BASES; i++)
  {
    if ((bs_info[i].valid_pn) &&
        (bs_info[i].ovhd_chan_info.ovhd_chan == bs_info_id->ovhd_chan) &&
        (bs_info[i].pilot_pn == bs_info_id->pilot_pn) &&
        (bs_info[i].cdma_ch == bs_info_id->cdma_ch) &&
        (bs_info[i].band_class == bs_info_id->band_class) &&
        (bs_info[i].ovhd_chan_info.ovhd_walsh_ch == bs_info_id->ovhd_walsh_ch))
    {
      break;
    }
  } /* end for */

  if (i == MC_MAX_BASES)
  {
    i = -1;
  }
  return i;

} /* mccidl_get_bs_index */

/*===========================================================================

FUNCTION MCC_SRCH_GET_NBR_INFO

DESCRIPTION
  This function retrieves page channel, broadcast control channel or forward
  control channel info of neighbor BS with config 0, 1, 2 or 4. The function is
  called in the context of the Searcher task during a fast RAHO so there is
  enough time for the new PN long code mask to settle down. If the config = 2
  the neighbor BS overheads should be current.

  Note: This function is called before function mcc_srch_fast_raho() and
  will not change cur_bs_ptr.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if channel info is available. Otherwise FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_srch_get_nbr_info
(
  word pilot_pn,
  srch_chan_config_cmd_type *chan_config
)
{
  int2 i;
  int  j;
  byte nghbr_cfg;
  dword hash_key;
  bs_info_id_type bs_info_id;

  /* Validate the neighbor config */
  if ((i = mccidl_get_nghbr_index(pilot_pn, cdma.cdmach, cdma.band_class)) == -1)
  {
    MSG_HIGH("Neigh info not found : Pilot PN=%d", pilot_pn, 0, 0);
    return FALSE;
  }

  nghbr_cfg = cur_bs_ptr->nl.nset[i].config;

  if ( ( OVHD_CHAN_IS_PCH && (nghbr_cfg > 2 ) )
      #ifdef FEATURE_IS2000_REL_A_CC
      || ( OVHD_CHAN_IS_BCCH && (nghbr_cfg > 2 ) && ( nghbr_cfg != 4 ) )
      #endif /* FEATURE_IS2000_REL_A_CC */
     )
  {
    MSG_HIGH("Neigh info not found : Pilot PN=%d", pilot_pn, 0, 0);
    return FALSE;
  }

  /* Initialize the channel monitoring info */
  j = chan_config->num_chan = 0;

  switch (nghbr_cfg)
  {
    case 0:
      #ifdef FEATURE_IS2000_REL_A_CC
      if (OVHD_CHAN_IS_BCCH)
      {
        /* Check to see if overhead of new BS is current */
        bs_info_id.ovhd_chan = cur_bs_ptr->ovhd_chan_info.ovhd_chan;
        bs_info_id.pilot_pn = pilot_pn;
        bs_info_id.cdma_ch = cdma.cdmach;
        bs_info_id.band_class = cdma.band_class;
        bs_info_id.ovhd_walsh_ch = cdma.bcch;

        /* New BS visited before. */
        if ( ( i = mccidl_get_bs_index(&bs_info_id) ) != -1 ) 
        {
          /* Overhead of new BS is current, FCCCH shall be monitored */
          if ( mccidl_check_bs_ovhd_current((byte)i) &&
               mccidl_get_bs_hash_key((byte)i, &hash_key) )
          {
            if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
            {
              /* Pack FCCCH info */
              chan_config->chan_info[j].chan_id = CAI_F_CCCH;
              chan_config->chan_info[j].config.fccch.fccch_id =
                (byte) hash(hash_key, bs_info[i].csp.fccch_info.num_fccch,
                            (word) (2 * (hash_key & 0xFFF))) + 1;
              chan_config->chan_info[j].config.fccch.walsh_chn =
                bs_info[i].csp.fccch_info.fccch_code_chan[chan_config->chan_info[j].config.fccch.fccch_id-1];
              chan_config->chan_info[j].config.fccch.c_rate =
                bs_info[i].csp.fccch_info.fccch_code_rate;
              chan_config->chan_info[j].config.fccch.rate =
                mccidl_decode_fccch_data_rate(bs_info[i].csp.fccch_info.fccch_rate);
              chan_config->chan_info[j].config.fccch.size =
                mccidl_decode_fccch_frame_size(bs_info[i].csp.fccch_info.fccch_rate);
              MSG_MED("FCCCH cfg: wch %d cr %d dr %d",
                chan_config->chan_info[j].config.fccch.walsh_chn,
                chan_config->chan_info[j].config.fccch.c_rate,
                chan_config->chan_info[j].config.fccch.rate);
              ++chan_config->num_chan;
            }
          }
          /* Overhead of new BS is not current, BCCH shall be monitored and
             FCCCH may be monitored. */
          else
          {
            if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
            {
              /* Pack BCCH info */
              chan_config->chan_info[j].chan_id = CAI_BCCH;
              chan_config->chan_info[j].config.bcch.walsh_chn = cdma.bcch;
              chan_config->chan_info[j].config.bcch.rate = cdma.brat;
              chan_config->chan_info[j].config.bcch.c_rate = cdma.bcch_code_rate;
              MSG_MED("BCCH cfg: wch %d cr %d dr %d",
                chan_config->chan_info[j].config.bcch.walsh_chn,
                chan_config->chan_info[j].config.bcch.c_rate,
                chan_config->chan_info[j].config.bcch.rate);
              j = ++chan_config->num_chan;
            }

            if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
            {
              /* Base on stored overhead of new BS, we know the FCCCH 
                 configuration. */  
              if ( ( bs_info[i].a41spm_config_msg_seq ==
                     bs_info[i].rx.config_msg_seq) &&
                   ( bs_info[i].mcrrpm_config_msg_seq ==
                     bs_info[i].rx.config_msg_seq ) &&
                   ( bs_info[i].ecl.config_msg_seq ==
                     bs_info[i].rx.config_msg_seq ) && bs_info[i].rx.a41spm_rx &&
                   bs_info[i].rx.mcrrpm_rx && bs_info[i].rx.ecl_rx &&
                   mccidl_get_bs_hash_key((byte)i, &hash_key) )
              {
                /* Pack FCCCH info */
                chan_config->chan_info[j].chan_id = CAI_F_CCCH;
                chan_config->chan_info[j].config.fccch.fccch_id =
                  (byte) hash(hash_key, bs_info[i].csp.fccch_info.num_fccch,
                              (word) (2 * (hash_key & 0xFFF))) + 1;
                chan_config->chan_info[j].config.fccch.walsh_chn =
                  bs_info[i].csp.fccch_info.fccch_code_chan[chan_config->chan_info[j].config.fccch.fccch_id-1];
                chan_config->chan_info[j].config.fccch.c_rate =
                  bs_info[i].csp.fccch_info.fccch_code_rate;
                chan_config->chan_info[j].config.fccch.rate =
                  mccidl_decode_fccch_data_rate(bs_info[i].csp.fccch_info.fccch_rate);
                chan_config->chan_info[j].config.fccch.size =
                  mccidl_decode_fccch_frame_size(bs_info[i].csp.fccch_info.fccch_rate);
                MSG_MED("FCCCH cfg: wch %d cr %d dr %d",
                  chan_config->chan_info[j].config.fccch.walsh_chn,
                  chan_config->chan_info[j].config.fccch.c_rate,
                  chan_config->chan_info[j].config.fccch.rate);
                ++chan_config->num_chan;
              }
              /* Overhead of current BS is current, we can deduce new BS's
                 FCCCH configurations based on it. */
              else if ( mcc_is_ovhd_cfg_okay() )
              {
                /* Pack FCCCH info */
                chan_config->chan_info[j].chan_id = CAI_F_CCCH;
                chan_config->chan_info[j].config.fccch.fccch_id = cdma.fccch_id;
                chan_config->chan_info[j].config.fccch.walsh_chn =
                  cur_bs_ptr->csp.fccch_info.fccch_code_chan[cdma.fccch_id-1];
                chan_config->chan_info[j].config.fccch.c_rate =
                  cur_bs_ptr->csp.fccch_info.fccch_code_rate;
                chan_config->chan_info[j].config.fccch.rate =
                  mccidl_decode_fccch_data_rate(cur_bs_ptr->csp.fccch_info.fccch_rate);
                chan_config->chan_info[j].config.fccch.size =
                  mccidl_decode_fccch_frame_size(cur_bs_ptr->csp.fccch_info.fccch_rate);
                MSG_MED("FCCCH cfg: wch %d cr %d dr %d",
                  chan_config->chan_info[j].config.fccch.walsh_chn,
                  chan_config->chan_info[j].config.fccch.c_rate,
                  chan_config->chan_info[j].config.fccch.rate);
                ++chan_config->num_chan;
              }
            }
          }
        }
        /* New BS not visited before, BCCH shall be monitored and FCCCH may be
           monitored if we can deduce FCCCH configuration from current BS. */
        else
        {
          if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
          {
            /* Pack BCCH info */
            chan_config->chan_info[j].chan_id = CAI_BCCH;
            chan_config->chan_info[j].config.bcch.walsh_chn = cdma.bcch;
            chan_config->chan_info[j].config.bcch.rate = cdma.brat;
            chan_config->chan_info[j].config.bcch.c_rate = cdma.bcch_code_rate;
            MSG_MED("BCCH cfg: wch %d cr %d dr %d",
              chan_config->chan_info[j].config.bcch.walsh_chn,
              chan_config->chan_info[j].config.bcch.c_rate,
              chan_config->chan_info[j].config.bcch.rate);
            j = ++chan_config->num_chan;
          }

          if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
          {
            if ( mcc_is_ovhd_cfg_okay() )
            {
              /* Pack FCCCH info */
              chan_config->chan_info[j].chan_id = CAI_F_CCCH;
              chan_config->chan_info[j].config.fccch.fccch_id = cdma.fccch_id;
              chan_config->chan_info[j].config.fccch.walsh_chn =
                cur_bs_ptr->csp.fccch_info.fccch_code_chan[cdma.fccch_id-1];
              chan_config->chan_info[j].config.fccch.c_rate =
                cur_bs_ptr->csp.fccch_info.fccch_code_rate;
              chan_config->chan_info[j].config.fccch.rate =
                mccidl_decode_fccch_data_rate(cur_bs_ptr->csp.fccch_info.fccch_rate);
              chan_config->chan_info[j].config.fccch.size =
                mccidl_decode_fccch_frame_size(cur_bs_ptr->csp.fccch_info.fccch_rate);
              MSG_MED("FCCCH cfg: wch %d cr %d dr %d",
                chan_config->chan_info[j].config.fccch.walsh_chn,
                chan_config->chan_info[j].config.fccch.c_rate,
                chan_config->chan_info[j].config.fccch.rate);
              ++chan_config->num_chan;
            }
          }
        }
      }
      else
      #endif /* FEATURE_IS2000_REL_A_CC */
      {
        /* Pack PCH info */
        chan_config->chan_info[j].chan_id = CAI_PCH;
        chan_config->chan_info[j].config.pch.walsh_chn = cdma.pagech;
        chan_config->chan_info[j].config.pch.rate = cdma.prat;
        MSG_MED("PCH cfg: wch %d r %d",
          chan_config->chan_info[j].config.pch.walsh_chn,
          chan_config->chan_info[j].config.pch.rate, 0);
        ++chan_config->num_chan;
      }
      break;

    case 1:
      /* Pack PCH info */
      chan_config->chan_info[j].chan_id = CAI_PCH;

      /* Check to see if overhead is current and update walsh channel */
      bs_info_id.ovhd_chan = CAIX_PC;
      bs_info_id.pilot_pn = pilot_pn;
      bs_info_id.cdma_ch = cdma.cdmach;
      bs_info_id.band_class = cdma.band_class;
      bs_info_id.ovhd_walsh_ch = mccidl_get_nbrcfg_pch_walsh ( bs_info_id.pilot_pn, 
                                   bs_info_id.cdma_ch, 
                                   bs_info_id.band_class );

      if ( ( ( i = mccidl_get_bs_index(&bs_info_id) ) != -1 ) &&
           mccidl_check_bs_ovhd_current((byte)i) &&
           mccidl_get_bs_hash_key((byte)i, &hash_key) )
      {
        chan_config->chan_info[j].config.pch.walsh_chn =
          (byte) hash(hash_key, bs_info[i].csp.sp.page_chan,
                      (word) (2 * (hash_key & 0xFFF))) + 1;
      }
      else
      {
        chan_config->chan_info[j].config.pch.walsh_chn = PRI_PAGECH;
      }

      #ifdef FEATURE_IS2000_REL_A_CC
      if (OVHD_CHAN_IS_BCCH)
      {
        chan_config->chan_info[j].config.pch.rate = CAI_PC_1_RATE;
      }
      else
      #endif /* FEATURE_IS2000_REL_A_CC */
      {
        chan_config->chan_info[j].config.pch.rate = cdma.prat;
      }

      MSG_MED("PCH cfg: wch %d r %d",
        chan_config->chan_info[j].config.pch.walsh_chn,
        chan_config->chan_info[j].config.pch.rate, 0);
      ++chan_config->num_chan;
      break;

    case 2:
      /* Check to see if overhead is current */
      bs_info_id.ovhd_chan = cur_bs_ptr->ovhd_chan_info.ovhd_chan;
      bs_info_id.pilot_pn = pilot_pn;
      bs_info_id.cdma_ch = cdma.cdmach;
      bs_info_id.band_class = cdma.band_class;

      #ifdef FEATURE_IS2000_REL_A_CC
      if (OVHD_CHAN_IS_BCCH)
      {
        bs_info_id.ovhd_walsh_ch = cdma.bcch;
      }
      else
      #endif /* FEATURE_IS2000_REL_A_CC */
      {
        bs_info_id.ovhd_walsh_ch = mccidl_get_nbrcfg_pch_walsh ( 
          bs_info_id.pilot_pn, bs_info_id.cdma_ch, bs_info_id.band_class);
      }

      /* New BS visited before and stored overhead is current. */
      if ( ( ( i = mccidl_get_bs_index(&bs_info_id) ) != -1 ) &&
           mccidl_check_bs_ovhd_current((byte)i) &&
           mccidl_get_bs_hash_key((byte)i, &hash_key) )
      {

        #ifdef FEATURE_IS2000_REL_A_CC
        if(OVHD_CHAN_IS_BCCH)
        {
          /* Overhead of new BS is current, FCCCH shall be monitored */
          if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
          {
            /* Pack FCCCH info */
            chan_config->chan_info[j].chan_id = CAI_F_CCCH;
            chan_config->chan_info[j].config.fccch.fccch_id =
              (byte) hash(hash_key, bs_info[i].csp.fccch_info.num_fccch,
                         (word) (2 * (hash_key & 0xFFF))) + 1;
            chan_config->chan_info[j].config.fccch.walsh_chn =
              bs_info[i].csp.fccch_info.fccch_code_chan[chan_config->chan_info[j].config.fccch.fccch_id-1];
            chan_config->chan_info[j].config.fccch.c_rate =
              bs_info[i].csp.fccch_info.fccch_code_rate;
            chan_config->chan_info[j].config.fccch.rate =
              mccidl_decode_fccch_data_rate(bs_info[i].csp.fccch_info.fccch_rate);
            chan_config->chan_info[j].config.fccch.size =
              mccidl_decode_fccch_frame_size(bs_info[i].csp.fccch_info.fccch_rate);
            MSG_MED("FCCCH cfg: wch %d cr %d dr %d",
              chan_config->chan_info[j].config.fccch.walsh_chn,
              chan_config->chan_info[j].config.fccch.c_rate,
              chan_config->chan_info[j].config.fccch.rate);
            ++chan_config->num_chan;
          }
        }
        else
        #endif /* FEATURE_IS2000_REL_A_CC */
        {
          chan_config->chan_info[j].chan_id = CAI_PCH;
          chan_config->chan_info[j].config.pch.walsh_chn =
            (byte) hash(hash_key, bs_info[i].csp.sp.page_chan, (word) (2 * (hash_key & 0xFFF))) + 1;
          chan_config->chan_info[j].config.pch.rate = cdma.prat;
          MSG_MED("PCH cfg: wch %d r %d",
            chan_config->chan_info[j].config.pch.walsh_chn,
            chan_config->chan_info[j].config.pch.rate, 0);
          ++chan_config->num_chan;
        }
      }
      /* New BS not visited before or stored overhead is not current. */
      else
      {
        #ifdef FEATURE_IS2000_REL_A_CC
        if (OVHD_CHAN_IS_BCCH)
        {
          /* Pack BCCH info */
          if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
          {
            chan_config->chan_info[j].chan_id = CAI_BCCH;
            chan_config->chan_info[j].config.bcch.walsh_chn = cdma.bcch;
            chan_config->chan_info[j].config.bcch.rate = cdma.brat;
            chan_config->chan_info[j].config.bcch.c_rate = cdma.bcch_code_rate;
            MSG_MED("BCCH cfg: wch %d cr %d dr %d",
              chan_config->chan_info[j].config.bcch.walsh_chn,
              chan_config->chan_info[j].config.bcch.c_rate,
              chan_config->chan_info[j].config.bcch.rate);
            j = ++chan_config->num_chan;

            if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
            {
              /* Base on stored overhead of new BS, we know the FCCCH 
                 configuration. */  
              if ( ( ( i = mccidl_get_bs_index(&bs_info_id) ) != -1 ) &&
                   ( bs_info[i].a41spm_config_msg_seq ==
                     bs_info[i].rx.config_msg_seq) &&
                   ( bs_info[i].mcrrpm_config_msg_seq ==
                     bs_info[i].rx.config_msg_seq ) &&
                   ( bs_info[i].ecl.config_msg_seq ==
                     bs_info[i].rx.config_msg_seq ) && bs_info[i].rx.a41spm_rx &&
                   bs_info[i].rx.mcrrpm_rx && bs_info[i].rx.ecl_rx &&
                   mccidl_get_bs_hash_key((byte)i, &hash_key) )
              {
                /* Pack FCCCH info */
                chan_config->chan_info[j].chan_id = CAI_F_CCCH;
                chan_config->chan_info[j].config.fccch.fccch_id =
                  (byte) hash(hash_key, bs_info[i].csp.fccch_info.num_fccch,
                              (word) (2 * (hash_key & 0xFFF))) + 1;
                chan_config->chan_info[j].config.fccch.walsh_chn =
                  bs_info[i].csp.fccch_info.fccch_code_chan[chan_config->chan_info[j].config.fccch.fccch_id-1];
                chan_config->chan_info[j].config.fccch.c_rate =
                  bs_info[i].csp.fccch_info.fccch_code_rate;
                chan_config->chan_info[j].config.fccch.rate =
                  mccidl_decode_fccch_data_rate(bs_info[i].csp.fccch_info.fccch_rate);
                chan_config->chan_info[j].config.fccch.size =
                  mccidl_decode_fccch_frame_size(bs_info[i].csp.fccch_info.fccch_rate);
                MSG_MED("FCCCH cfg: wch %d cr %d dr %d",
                  chan_config->chan_info[j].config.fccch.walsh_chn,
                  chan_config->chan_info[j].config.fccch.c_rate,
                  chan_config->chan_info[j].config.fccch.rate);
                ++chan_config->num_chan;
              }
              j = ++chan_config->num_chan;
            }
          }
          /* Overhead is not current. */
        }
        else
        #endif /* FEATURE_IS2000_REL_A_CC */
        {
          chan_config->chan_info[j].chan_id = CAI_PCH;
          chan_config->chan_info[j].config.pch.walsh_chn = PRI_PAGECH;
          chan_config->chan_info[j].config.pch.rate = cdma.prat;
          MSG_MED("PCH cfg: wch %d r %d",
            chan_config->chan_info[j].config.pch.walsh_chn,
            chan_config->chan_info[j].config.pch.rate, 0);
          ++chan_config->num_chan;
        }
      }
      break;

    #ifdef FEATURE_IS2000_REL_A_CC
    case 4:
      /* Check to see if overhead is current */
      bs_info_id.ovhd_chan = cur_bs_ptr->ovhd_chan_info.ovhd_chan;
      bs_info_id.pilot_pn = pilot_pn;
      bs_info_id.cdma_ch = cdma.cdmach;
      bs_info_id.band_class = cdma.band_class;
      bs_info_id.ovhd_walsh_ch = cdma.bcch;

      /* New BS visited before. */
      if ( ( i = mccidl_get_bs_index(&bs_info_id) ) != -1 ) 
      {
        /* Stored overhead of new BS is current, FCCCH shall be monitored */
        if ( mccidl_check_bs_ovhd_current((byte)i) &&
             mccidl_get_bs_hash_key((byte)i, &hash_key) )
        {
          if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
          {
            /* Pack FCCCH info */
            chan_config->chan_info[j].chan_id = CAI_F_CCCH;
            chan_config->chan_info[j].config.fccch.fccch_id =
              (byte) hash(hash_key, bs_info[i].csp.fccch_info.num_fccch,
                         (word) (2 * (hash_key & 0xFFF))) + 1;
            chan_config->chan_info[j].config.fccch.walsh_chn =
              bs_info[i].csp.fccch_info.fccch_code_chan[chan_config->chan_info[j].config.fccch.fccch_id-1];
            chan_config->chan_info[j].config.fccch.c_rate =
              bs_info[i].csp.fccch_info.fccch_code_rate;
            chan_config->chan_info[j].config.fccch.rate =
              mccidl_decode_fccch_data_rate(bs_info[i].csp.fccch_info.fccch_rate);
            chan_config->chan_info[j].config.fccch.size =
              mccidl_decode_fccch_frame_size(bs_info[i].csp.fccch_info.fccch_rate);
            MSG_MED("FCCCH cfg: wch %d cr %d dr %d",
              chan_config->chan_info[j].config.fccch.walsh_chn,
              chan_config->chan_info[j].config.fccch.c_rate,
              chan_config->chan_info[j].config.fccch.rate);
            ++chan_config->num_chan;
          }
        }
        /* Stored overhead of new BS is not current, BCCH shall be monitored and 
           FCCCH may be monitored */
        else
        {
          /* Pack BCCH info */
          if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
          {
            chan_config->chan_info[j].chan_id = CAI_BCCH;
            chan_config->chan_info[j].config.bcch.walsh_chn = cdma.bcch;
            chan_config->chan_info[j].config.bcch.rate = cdma.brat;
            chan_config->chan_info[j].config.bcch.c_rate = cdma.bcch_code_rate;
            MSG_MED("BCCH cfg: wch %d cr %d dr %d",
              chan_config->chan_info[j].config.bcch.walsh_chn,
              chan_config->chan_info[j].config.bcch.c_rate,
              chan_config->chan_info[j].config.bcch.rate);
            j = ++chan_config->num_chan;

            if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
            {
              if ( ( bs_info[i].a41spm_config_msg_seq ==
                     bs_info[i].rx.config_msg_seq) &&
                   ( bs_info[i].mcrrpm_config_msg_seq ==
                     bs_info[i].rx.config_msg_seq ) &&
                   ( bs_info[i].ecl.config_msg_seq ==
                     bs_info[i].rx.config_msg_seq ) && bs_info[i].rx.a41spm_rx &&
                   bs_info[i].rx.mcrrpm_rx && bs_info[i].rx.ecl_rx &&
                   mccidl_get_bs_hash_key((byte)i, &hash_key) )
              {
                /* Pack FCCCH info */
                chan_config->chan_info[j].chan_id = CAI_F_CCCH;
                chan_config->chan_info[j].config.fccch.fccch_id =
                  (byte) hash(hash_key, bs_info[i].csp.fccch_info.num_fccch,
                              (word) (2 * (hash_key & 0xFFF))) + 1;
                chan_config->chan_info[j].config.fccch.walsh_chn =
                  bs_info[i].csp.fccch_info.fccch_code_chan[chan_config->chan_info[j].config.fccch.fccch_id-1];
                chan_config->chan_info[j].config.fccch.c_rate =
                  bs_info[i].csp.fccch_info.fccch_code_rate;
                chan_config->chan_info[j].config.fccch.rate =
                  mccidl_decode_fccch_data_rate(bs_info[i].csp.fccch_info.fccch_rate);
                chan_config->chan_info[j].config.fccch.size =
                  mccidl_decode_fccch_frame_size(bs_info[i].csp.fccch_info.fccch_rate);
                MSG_MED("FCCCH cfg: wch %d cr %d dr %d",
                  chan_config->chan_info[j].config.fccch.walsh_chn,
                  chan_config->chan_info[j].config.fccch.c_rate,
                  chan_config->chan_info[j].config.fccch.rate);
                ++chan_config->num_chan;
              }
            }
          }
        }
      }
      /* New BS not visited before, BCCH shall be monitored. */
      else
      {
        /* Pack BCCH info */
        if (j < CAI_MAX_NUM_CHAN_MONITOR) //lint !e506 !e774
        {
          chan_config->chan_info[j].chan_id = CAI_BCCH;
          chan_config->chan_info[j].config.bcch.walsh_chn = cdma.bcch;
          chan_config->chan_info[j].config.bcch.rate = cdma.brat;
          chan_config->chan_info[j].config.bcch.c_rate = cdma.bcch_code_rate;
          MSG_MED("BCCH cfg: wch %d cr %d dr %d",
            chan_config->chan_info[j].config.bcch.walsh_chn,
            chan_config->chan_info[j].config.bcch.c_rate,
            chan_config->chan_info[j].config.bcch.rate);
          ++chan_config->num_chan;
        }
      }
      break;
      #endif /* FEATURE_IS2000_REL_A_CC */

    default:
      /* Should never be here */
      ERR("Invalid nghbr config %d", nghbr_cfg, 0, 0);
      break;
  }

  return TRUE;

} /* mcc_srch_get_nbr_info */

/*===========================================================================

FUNCTION MCC_SRCH_FAST_RAHO

DESCRIPTION
  This function is called in the context of the Searcher task during idle
  sleep to expedite MC's response to a fast RAHO.

DEPENDENCIES
  None.

RETURN VALUE
  Value of mccidl_cheap_idle_ho, which when TRUE indicates to SRCH that it
  can go back to sleep

SIDE EFFECTS
  The values of the global varibles mccidl_fast_raho_state and
  mccidl_cheap_idle_ho may be changed.

  Caution: this function may be called before "SRCH_WAKEUP_R" sleep-
  report.

===========================================================================*/

boolean mcc_srch_fast_raho
(
  word new_pilot,
    /* New pilot received in searcher report */
  byte ecio_old_pilot,
  byte ecio_new_pilot
)
{
  MSG_LOW("Enter MC Fast RAHO.", 0, 0, 0);

  /* At entrance of this function, cdma_state has to be "CDMA_SLOTTED" */
  mccidl_fast_raho_state = CDMA_SLOTTED; /*lint !e641 */

  mccidl_reacq_after_fast_raho = FALSE;

  /* Remember this is going to be a "fast" RAHO. */
  mccidl_fast_raho = TRUE;

  /* Note that mccidl_raho() will change mccidl_cheap_idle_ho flag */
  /* FEATURE_JSTD008 was removed here for a uniform interface */
  /* Fast RAHO always has to be on the same frequency and same band class */
  mccidl_fast_raho_state = mccidl_handoff(new_pilot, cdma.cdmach,
                           cdma.band_class, mccidl_fast_raho_state,
                           (int8) ecio_old_pilot, (int8) ecio_new_pilot,
                           CDMA_FAST_RAHO);

  /* We've used up this fast-RAHO command */
  mccidl_fast_raho = FALSE;
  mccidl_fast_raho_performed = TRUE;

  MSG_LOW("End of MC Fast RAHO with Cheap HO: %d and CDMA State: %d",
           mccidl_cheap_idle_ho, mccidl_fast_raho_state, 0);

  return mccidl_cheap_idle_ho;

} /* mcc_srch_fast_raho */

/*===========================================================================

FUNCTION MCCIDL_GET_BS_P_REV

DESCRIPTION
  This function determines the P_REV of a base station. It checks if
  the base station information we have is current (ie <= T31m). If so,
  it tries to determine the P_REV from either the sync channel message
  or the extended system parameters message. If it's unable to determine
  the P_REV, it returns FALSE.

  NOTE: This function is internal to mccidl and is intended only for
  mcc_srch_min_bs_p_rev.

DEPENDENCIES
  None

RETURN VALUE
  True if the P_REV can be determined and is current (ie, <= t31m old).

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccidl_get_bs_p_rev
(
  cdma_bs_type * bs,
    /* Base station from which the p_rev is required */

  qword old_info_time,
    /* Information of the p_rev must be at least this current */

  byte *p_rev
    /* Returned p_rev */
)
{
  /* If the most recent msg is at from "old_info_time" or later, then
     consider the information to be current. */

  if ( qw_cmp( bs->rx.msg_time, old_info_time) >= 0)
  {
    /* Get the P_REV from Sync Channel Message, if available */
    if ( bs->rx.sy_rx )
    {
      *p_rev = bs->sy.p_rev;
      return TRUE;
    }

    /* Else get the P_REV from Extended System Parameters Message
    if available */

    if (((bs->ovhd_chan_info.ovhd_chan == CAIX_PC) && bs->rx.esp_rx  &&
         (bs->csp.esp.config_msg_seq == bs->rx.config_msg_seq))
        #ifdef FEATURE_IS2000_REL_A_CC
        || ((bs->ovhd_chan_info.ovhd_chan == CAIX_BCCH) && bs->rx.mcrrpm_rx &&
            (bs->mcrrpm_config_msg_seq == bs->rx.config_msg_seq))
        #endif /* FEATURE_IS2000_REL_A_CC */
       )
    {
      *p_rev = bs->csp.esp.p_rev;
      if( *p_rev == CAIX_P_REV_UNKNOWN )
      {
        /* This is not a 95B base station. P_REV is unknown.
        We may be able to guess the P_REV from the band_class
        information, but for now, just return FALSE. */
        return FALSE;
      }
      return TRUE;
    }

  } /* If information is current */

  /* Information is not current, return FALSE */
  return FALSE;

} /* mccidl_get_bs_p_rev */

/*===========================================================================

FUNCTION MCC_SRCH_MIN_BS_P_REV

DESCRIPTION
  This function returns the minimum P_REV among the known base station on the
  CDMA channel of the ASET. The known base stations are those with current
  configuration parameters and those for which we have received the BS P_REV.

DEPENDENCIES
  None.

RETURN VALUE
  The minimum P_REV among the known base station.

SIDE EFFECTS
  None.

===========================================================================*/
byte mcc_srch_min_bs_p_rev(byte band, word freq)
{
  byte min_p_rev;
  byte p_rev;
  word i;
    /* index to loop through bs_info[] array */
  qword current_time;
    /* the current time in 20ms time stamp */
  qword age_limit;
    /* the age limit of BS info */
  qword old_info_time;
    /* oldest BS info allowed to be */

  ts_get_20ms_frame_time(current_time);
    /* Get current system time in units of 20 msecs */

  qw_set(age_limit, 0, cai_tmo.t31m / 20L);
    /* Age limit for base station information */

  qw_sub(old_info_time, current_time, age_limit);
    /* Before this time, BS info is invalid */

  if (mccidl_get_bs_p_rev(cur_bs_ptr, old_info_time, &p_rev))
  {
    min_p_rev = p_rev;
  }
  else
  {
    /* mccidl_get_bs_p_rev returned FALSE. This could mean we Idle HO'ed
    to this BS and haven't received enough information to know the P_REV.
    Default to the stored p_rev in use */

    min_p_rev = mc_get_p_rev_in_use();
  }

  for (i=0; i<MC_MAX_BASES; i++)
  {
    if ((bs_info[i].valid_pn) && (bs_info[i].band_class == band) &&
        (bs_info[i].cdma_ch == freq))
    {
      if (mccidl_get_bs_p_rev( &bs_info[i], old_info_time, &p_rev))
      {
        if (min_p_rev > p_rev)
        {
          min_p_rev = p_rev;
        }
      } /* p_rev is valid */
    }  /* valid pn, and same band/freq */
  }  /* for all base stations */

  return (min_p_rev);

} /* mcc_srch_min_bs_p_rev */

/*===========================================================================

FUNCTION MCCIDL_WAKEUP_SYS_MEAS

DESCRIPTION
  This procedure sends command to SEARCH to have it transition back to
  Paging Channel state from sleep for speeding up background system
  measurement.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_wakeup_sys_meas(void)
{
  MSG_LOW("MC needs to wake up for system measurement",0,0,0);

  /* have SRCH transition from sleep to idle state */
  mccidl_send_srch_idle_cmd();

  /* Setting the default values before Exiting Sleep */
  mccidl_sleep_state = MCC_NON_SLEEP;
  mccidl_need_full_wakeup = TRUE;
  mccidl_cheap_idle_ho = FALSE;
  mccidl_reg_required = FALSE;

} /* mccidl_wakeup_sys_meas */

/*===========================================================================

FUNCTION MCCIDL_SLEEP_PROC_SRCH_RPT

DESCRIPTION
  This procedure processes the search report at MCC_SLEEP state.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_sleep_proc_srch_rpt
(
  mccsrch_rpt_type *rpt_ptr,
    /* Report received from searcher */
  word new_state
    /* Next state to be processed */
)
{
  int i, j;    
    /* used as local loop variable for system reselection */
  sd_sys_s_type sys_list[CAI_GEN_ANALOG_NGHBR_MAX];
    /* List of system for reselection */
    
  switch (rpt_ptr->hdr.rpt)
  {
    /* At "MCC_SLEEP" state, MC can receive "SRCH_WAKEUP_R".
     */
    case SRCH_QPCH_REACQ_R:    /* Fully reacquired after sleep */
      MSG_LOW("QPCH Reacq rpt rxed from SRCH",0,0,0);

      if (mccidl_fast_raho_performed)
      {
        MSG_MED("Fast RAHOed before Reacq", 0, 0, 0);
        new_state = mccidl_fast_raho_state;

        mccidl_reacq_after_fast_raho = TRUE;

        if (!mccidl_cheap_idle_ho )
        {
          /* Remember we need to wake up */
          mccidl_need_full_wakeup = TRUE;
        }
      }

      if (!mccidl_need_full_wakeup)
      {
        /* MC decides autonomously if it needs to be awake */
        mccidl_need_full_wakeup = mccidl_autonomous_wakeup();
      }

      if (mccidl_need_full_wakeup)
      {
         /* remind SRCH that it needs to transition to Idle state in the
            future */
         MSG_LOW("MC needs to wake-up after REACQ",0,0,0);
         mccidl_send_srch_idle_cmd();
      }  /* mccidl_need_full_wakeup */

      /* no state transition is needed since SRCH is sleeping until the real
       * paging slot comes */
      break;

    case SRCH_WAKEUP_R:    /* Wakeup Report */

      /* Searcher tells us what slot cycle it is really using */
      sci_mask = mcc_calculate_sci_mask(
                   rpt_ptr->wakeup.slot_cycle_index);

      qw_equ(mccidl_sleep_ok_time, rpt_ptr->wakeup.valid_time);
      qw_shift(mccidl_sleep_ok_time, -2);

      MSG_MED("SRCH_WAKEUP_R, mask %d slot 0x%x",
               rpt_ptr->wakeup.cur_slot_mask, qw_lo(mccidl_sleep_ok_time), 0);

      mclog_report_wakeup_slot(rpt_ptr->wakeup.cur_slot_mask,
                               rpt_ptr->wakeup.srch_slept);

      /* Based on slot mask of wakeup report, enable or disable MER statistics 
         for page slot. */
      mclog_mer_config((rpt_ptr->wakeup.cur_slot_mask & PAGE_SLOT),
        rpt_ptr->wakeup.valid_time);

      /* Search is sending a wakeup report while already awake.     */
      /* A new slot is coming, check if we need to switch channels. */
      mccidl_supervise_channel_monitoring(new_state, &rpt_ptr->wakeup, FALSE);

      #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      mccbcsms_proc_srch_rpt(rpt_ptr);
      #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

      /* Have RXC transition from sleep to the Paging Channel
         Arm page match feature if needed */
      mccidl_wakeup(rpt_ptr->wakeup.srch_slept,
                    #ifdef FEATURE_PAGE_MATCH_REQUEST
                    rpt_ptr->wakeup.page_match
                    #else
                    FALSE
                    #endif /* FEATURE_PAGE_MATCH_REQUEST */
                    , rpt_ptr->wakeup.cur_slot_mask, (word)qw_lo(mccidl_sleep_ok_time));

      /* We use mccidl_sleep_ok_time to calc. slot. 
         Ignore any sleep requests generated before now. */
      qw_equ(mccidl_sleep_ok_time, rpt_ptr->wakeup.valid_time);

      MSG_LOW("Sleep Ok Time = 0x%x",
              (word) qw_lo(mccidl_sleep_ok_time), 0, 0);

      /* if we were rudely awakened, OR if we never actually slept due to off
       * frequency searching, possibly leading to a slow RAHO, then we need to
       * go to the paging channel
       */
      mccidl_need_full_wakeup =
        (rpt_ptr->wakeup.rude_awakening || !rpt_ptr->wakeup.srch_slept);

      /* Half Waking up from sleep. Transition to "MCC_WAKING_UP" state */
      mccidl_sleep_state = MCC_WAKING_UP;

      #ifdef FEATURE_BROADCAST_SMS
      if ((rpt_ptr->wakeup.cur_slot_mask & BC_DATA_SLOT) != 0)
      {
        MSG_HIGH("BCSMS: send schedule to Search early upon wakeup report",0,0,0);
        mccbcsms_sched_bcast_upon_wakeup(rpt_ptr->wakeup.valid_time);
      }
      #endif /* FEATURE_BROADCAST_SMS */

      break;

    case SRCH_SYS_MEAS_R:
      /* Report system measurements to System Determination */
      mcc_sys_meas_rpt(rpt_ptr);
      /* --------------------------------------------------------------
      ** Inform system determination of done with reporting system
      ** measurement.
      ** Determine the next state to be processed based on the returned
      ** SS-Action.
      ** -------------------------------------------------------------- */
      if ( !mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
           sd_ss_ind_misc_meas_rprt(rpt_ptr->sys_meas.meas_id, TRUE, NULL)) )
      {
        MSG_LOW("MC needs to wake up for reselection",0,0,0);

        /* Setting the default values before Exiting to SD */
        mccidl_sleep_state = MCC_NON_SLEEP;
        mccidl_need_full_wakeup = TRUE;
        mccidl_cheap_idle_ho = FALSE;
        mccidl_reg_required = FALSE;

        mcc_exit.exit = MCC_RESELECTION;
        new_state = CDMA_EXIT;
        event_report(EVENT_PREF_SYS_RESEL);
      }
      /* Wake up Search to speed up background system measurement */
      else if (sd_ss_act_get(NULL) == SD_SS_ACT_MEAS_BACK)
      {
        mccidl_wakeup_sys_meas();
      }
      break;

    case SRCH_REQUEST_SYS_RESTART_R:
      /* -------------------------------------------------------------
      ** Searcher requested system restart. Need to do system exit .
      ** Inform system determination of system lost and determines the
      ** next state to be processed based on the returned SS-Action.
      ** ------------------------------------------------------------- */
      if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_ADVISE_UNLOCK_RF, NULL ) )
         )
      {
        MSG_HIGH( "SRCH request sys restart",0,0,0 );

        /* Setting the default values before Exiting to SD */
        mccidl_sleep_state = MCC_NON_SLEEP;
        mccidl_need_full_wakeup = TRUE;
        mccidl_cheap_idle_ho = FALSE;
        mccidl_reg_required = FALSE;

        mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
        new_state = CDMA_EXIT;
      }
      break;
     
    case SRCH_THRESH_R:     /* Pilot power below threshold, exit to SD */
      MSG_HIGH ( "Pilot power below threshold!", 0, 0, 0 );
      memset(sys_list, 0, sizeof(sd_sys_s_type) * CAI_GEN_ANALOG_NGHBR_MAX);  
      /* -----------------------------------------------------------------
      ** Inform system determination of system re-selection.
      ** Determines the next state to be processed based on returned
      ** SS-Action.
      ** ----------------------------------------------------------------- */
      for (i=0, j=0; i < cur_bs_ptr->anl.num_analog_nghbr; i++)
      {
        if (cur_bs_ptr->anl.anl_nghbr[i].sys_a_b & 0x01)
        {
          sys_list[j].mode = SD_MODE_AMPS;
          sys_list[j].band = cur_bs_ptr->anl.anl_nghbr[i].band_class;
          sys_list[j].chan = SD_CELL_SYS_A; /*lint !e641 */
          j++;
        }

        if (cur_bs_ptr->anl.anl_nghbr[i].sys_a_b & 0x02)
        {
          sys_list[j].mode = SD_MODE_AMPS;
          sys_list[j].band = cur_bs_ptr->anl.anl_nghbr[i].band_class;
          sys_list[j].chan = SD_CELL_SYS_B; /*lint !e641 */
          j++;
        }
      }

      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_opr_sys_resel(sys_list, j, NULL)))

      {
        /* Setting the default values before Exiting to SD */
        mccidl_sleep_state = MCC_NON_SLEEP;
        mccidl_need_full_wakeup = TRUE;
        mccidl_cheap_idle_ho = FALSE;
        mccidl_reg_required = FALSE;

        /* Exit to the SD with MCC_SYSTEM_RESELECTION exit reason */
        mcc_exit.exit = MCC_SYSTEM_RESELECTION ;
        new_state = CDMA_EXIT;
      }

      else
      {
        /* Tell SRCH to continue on current system */
        mcc_srch_buf.sys_reselect.hdr.cmd = SRCH_RESELECT_MEAS_F;
        mcc_srch_cmd(&mcc_srch_buf);

        /* Wake up Search to speed up background system measurement */
        if (sd_ss_act_get(NULL) == SD_SS_ACT_MEAS_BACK)
        {
          mccidl_wakeup_sys_meas();
        }
      }

      /* Events report: System reselection */
      event_report(EVENT_SYSTEM_RESELECTION);
      break;
      
    default:
      ERR ("Unexpected SRCH Rpt %d at MCC_SLEEP State %d ",
            rpt_ptr->hdr.rpt, 0, 0);
      break;
  }
  return (new_state);

} /* mccidl_sleep_proc_srch_rpt */

/*===========================================================================

FUNCTION MCCIDL_WAKING_UP_PROC_SRCH_RPT

DESCRIPTION
  This procedure processes the search report at MCC_WAKING_UP state.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_waking_up_proc_srch_rpt
(
  mccsrch_rpt_type *rpt_ptr,
    /* Report received from searcher */
  word new_state
    /* Next state to be processed */
)
{
  int i, j;    
    /* used as local loop variable for system reselection */
  sd_sys_s_type sys_list[CAI_GEN_ANALOG_NGHBR_MAX];
    /* List of system for reselection */
  mccsrch_rpt_type *tmp_ptr; 
    /* Report received from searcher */

  switch (rpt_ptr->hdr.rpt)
  {
    /* At "MCC_WAKING_UP" state, MC can receive "SRCH_RAFAIL_R",
     * "SRCH_RAHO_R" or "SRCH_REACQ_R".
     */
    case SRCH_RAFAIL_R:    /* Failed to reacquire after sleep report */
      if ( !rpt_ptr->rafail.require_system_exit && (mccidl_rafail_retry_cnt > 0) )
      {   

        MSG_HIGH("Stay asleep after RAFAIL retry=%d",mccidl_rafail_retry_cnt,0,0);
        mccidl_rafail_retry_cnt--;

        /* Since we got a RAFAIL, we will get a page match status
        ** report from SARCH next. Wait for this and discard it 
        */
        (void) mcc_wait( MCC_SRCH_Q_SIG );

        if ( (tmp_ptr = (mccsrch_rpt_type *)q_get( &mcc_srch_q )) != NULL )
        {   
          if ( tmp_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
          {
            /* -------------------------------------------------------------
            ** Searcher requested system restart. Need to do system exit .
            ** Inform system determination of system lost and determines the
            ** next state to be processed based on the returned SS-Action.
            ** ------------------------------------------------------------- */
            if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                    sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_ADVISE_UNLOCK_RF, NULL ) ) 
               )
            {
              MSG_HIGH( "SRCH request sys restart",0,0,0 );

              /* Setting the default values before Exiting to SD */
              mccidl_sleep_state = MCC_NON_SLEEP;
              mccidl_need_full_wakeup = TRUE;
              mccidl_cheap_idle_ho = FALSE;
              mccidl_reg_required = FALSE;

              mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
              new_state = CDMA_EXIT;
            }
          }

          else
          {
            MSG_HIGH( "Discarding page match message",0,0,0 );
          }

          /* clean up report processing */
          cmd_done( &tmp_ptr->hdr.rpt_hdr );
        }
        
        if ( new_state != CDMA_EXIT )
        {
          /* Go back to sleep again without a complete wakeup */
          mccidl_sleep_state = MCC_SLEEP;

          /* Send a command to the Search task to sleep. 
          ** Treat it as an assigned slot. this
          ** might need optimization later 
          */
          mcc_srch_buf.sleep.hdr.cmd = SRCH_SLEEP_F;
          mcc_srch_buf.sleep.hdr.cmd_hdr.task_ptr = NULL;
          mcc_srch_buf.sleep.hdr.cmd_hdr.done_q_ptr = NULL;
          mcc_srch_buf.sleep.cur_slot_mask = PAGE_SLOT;
          mcc_srch_cmd(&mcc_srch_buf);

          /* Search is going to sleep at this point
          ** Tell RXC to go night-night, too
          */
          mcc_rx_buf.sleep.hdr.command = RXC_SLEEP_F;
          mcc_rx_buf.sleep.hdr.cmd_hdr.done_q_ptr = NULL;
          mcc_rxc_cmd( &mcc_rx_buf);
        }
      }

      else
      {
        #ifndef FEATURE_ERRLOG_DEBUG
        MSG_HIGH ( "Lost Acquisition after sleep!, %d", 
          rpt_ptr->rafail.require_system_exit, 0, 0);
        #else /* FEATURE_ERRLOG_DEBUG */
        ERR ( "Lost Acquisition after sleep!, %d", 
          rpt_ptr->rafail.require_system_exit, 0, 0);
        #endif /* FEATURE_ERRLOG_DEBUG */

        /* -------------------------------------------------------------
        ** Inform system determination of system lost and determines the
        ** next state to be processed based on the returned SS-Action.
        ** ------------------------------------------------------------- */
        if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_UNKNOWN, NULL ) )
           )
        {
          mcc_exit.exit = MCC_SYSTEM_LOST;
          new_state = CDMA_EXIT;
        }
        mccidl_sleep_state = MCC_NON_SLEEP;

        /* Events report: Loss of acquisition after sleep */
        event_report(EVENT_LOSS_OF_ACQ_AFTER_SLEEP);
      }

      break;

    case SRCH_RAHO_R:      /* Reacquired, but idle HandOff required report */
      MSG_LOW("RAHO - MC needs to wake up", 0, 0, 0);

      /* have SRCH transition from sleep to idle state */
      mccidl_send_srch_idle_cmd();

      /* Perform reacquisition Hand off */
      /* Note that mccidl_raho() will change mccidl_cheap_idle_ho flag */
      /* FEATURE_JSTD008 removed for unified interface */
      new_state = mccidl_handoff((word) rpt_ptr->raho.pilot, (word) rpt_ptr->raho.freq,
                                 (byte) rpt_ptr->raho.band_class, new_state,
                                 (int8) rpt_ptr->raho.ecio_old_pilot,
                                 (int8) rpt_ptr->raho.ecio_new_pilot,
                                 CDMA_RAHO);

      /* Always come awake on a regular RAHO */
      mccidl_sleep_state = MCC_NON_SLEEP;
      break;

    case SRCH_REACQ_R:    /* Fully reacquired after sleep */
      MSG_LOW("Reacq rpt rxed from SRCH",0,0,0);

      /* Allow Search to retry a number of times after RAFAIL */
      mccidl_rafail_retry_cnt = MCCIDL_MAX_RAFAIL_RETRY_CNT;

      if (mccidl_fast_raho_performed)
      {
        MSG_MED("Fast RAHOed before Reacq", 0, 0, 0);
        new_state = mccidl_fast_raho_state;

        mccidl_reacq_after_fast_raho = TRUE;

        if (!mccidl_cheap_idle_ho )
        {
          /* Remember we need to wake up */
          mccidl_need_full_wakeup = TRUE;
        }
      }

      if (!mccidl_need_full_wakeup)
      {
        /* MC decides autonomously if it needs to be awake */
        mccidl_need_full_wakeup = mccidl_autonomous_wakeup();
      }

      if (mccidl_need_full_wakeup)
      {
        /* have SRCH transition from sleep to the Paging Channel */
         MSG_LOW("MC needs to wake-up after REACQ",0,0,0);
         mccidl_send_srch_idle_cmd();


        if (mcc_srch_buf.hdr.status == SRCH_SLEEP_S)
        {
          /* Oops, SRCH missed SRCH_IDLE_F before the sleep
           * boundary, MC will go ahead to process "SRCH_FAST_SLEEP_R" in the
           * report queue and wakes up in the next cycle.
           */
          MSG_HIGH("SRCH missed IDLE_F before the sleep boundary", 0, 0, 0);
          mccidl_sleep_state = MCC_WAIT_FOR_MORE_SLEEP;
        }
        else
        {
          mccidl_sleep_state = MCC_NON_SLEEP;
        }
      }  /* mccidl_need_full_wakeup */

      else
      {
        /* Wait for decoder isr report to make sleep decision */
        mccidl_sleep_state = MCC_WAIT_FOR_MORE_SLEEP;
      }
      break;

    case SRCH_THRESH_R:     /* Pilot power below threshold, exit to SD */
      MSG_HIGH ( "Pilot power below threshold!", 0, 0, 0 );
      memset(sys_list, 0, sizeof(sd_sys_s_type) * CAI_GEN_ANALOG_NGHBR_MAX);  
      /* -----------------------------------------------------------------
      ** Inform system determination of system re-selection.
      ** Determines the next state to be processed based on returned
      ** SS-Action.
      ** ----------------------------------------------------------------- */
      for (i=0, j=0; i < cur_bs_ptr->anl.num_analog_nghbr; i++)
      {
        if (cur_bs_ptr->anl.anl_nghbr[i].sys_a_b & 0x01)
        {
          sys_list[j].mode = SD_MODE_AMPS;
          sys_list[j].band = cur_bs_ptr->anl.anl_nghbr[i].band_class;
          sys_list[j].chan = SD_CELL_SYS_A; /*lint !e641 */
          j++;
        }

        if (cur_bs_ptr->anl.anl_nghbr[i].sys_a_b & 0x02)
        {
          sys_list[j].mode = SD_MODE_AMPS;
          sys_list[j].band = cur_bs_ptr->anl.anl_nghbr[i].band_class;
          sys_list[j].chan = SD_CELL_SYS_B; /*lint !e641 */
          j++;
        }
      }

      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_opr_sys_resel(sys_list, j, NULL)))

      {
        /* Setting the default values before Exiting to SD */
        mccidl_sleep_state = MCC_NON_SLEEP;
        mccidl_need_full_wakeup = TRUE;
        mccidl_cheap_idle_ho = FALSE;
        mccidl_reg_required = FALSE;

        /* Exit to the SD with MCC_SYSTEM_RESELECTION exit reason */
        mcc_exit.exit = MCC_SYSTEM_RESELECTION ;
        new_state = CDMA_EXIT;
      }

      else
      {
        /* Tell SRCH to continue on current system */
        mcc_srch_buf.sys_reselect.hdr.cmd = SRCH_RESELECT_MEAS_F;
        mcc_srch_cmd(&mcc_srch_buf);

        /* Wake up Search to speed up background system measurement */
        if (sd_ss_act_get(NULL) == SD_SS_ACT_MEAS_BACK)
        {
          mccidl_wakeup_sys_meas();
        }
      }

      /* Events report: System reselection */
      event_report(EVENT_SYSTEM_RESELECTION);
      break;


    case SRCH_WAKEUP_R:
      /* Wakeup rpt is ignored here, since it's sent periodically and has no
       * meaning in this context. */
      MSG_MED("Ignoring SRCH_WAKEUP_R in MCC_WAKEUP_UP state",0,0,0);
      break;

    case SRCH_SYS_MEAS_R:
      /* Report system measurements to System Determination */
      mcc_sys_meas_rpt(rpt_ptr);
      /* --------------------------------------------------------------
      ** Inform system determination of done with reporting system
      ** measurement.
      ** Determine the next state to be processed based on the returned
      ** SS-Action.
      ** -------------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_misc_meas_rprt(rpt_ptr->sys_meas.meas_id, TRUE, NULL)))
      {
        MSG_LOW("MC needs to wake up for reselection",0,0,0);

        /* Setting the default values before Exiting to SD */
        mccidl_sleep_state = MCC_NON_SLEEP;
        mccidl_need_full_wakeup = TRUE;
        mccidl_cheap_idle_ho = FALSE;
        mccidl_reg_required = FALSE;

        mcc_exit.exit = MCC_RESELECTION;
        new_state = CDMA_EXIT;
        event_report(EVENT_PREF_SYS_RESEL);
      }
      /* Wake up Search to speed up background system measurement */
      else if (sd_ss_act_get(NULL) == SD_SS_ACT_MEAS_BACK)
      {
        mccidl_wakeup_sys_meas();
      }
      break;

    case SRCH_REQUEST_SYS_RESTART_R:
      /* -------------------------------------------------------------
      ** Searcher requested system restart. Need to do system exit .
      ** Inform system determination of system lost and determines the
      ** next state to be processed based on the returned SS-Action.
      ** ------------------------------------------------------------- */
      if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_ADVISE_UNLOCK_RF, NULL ) ) 
         )
      {
        MSG_HIGH( "SRCH request sys restart", 0, 0, 0 );

        /* Setting the default values before Exiting to SD */
        mccidl_sleep_state = MCC_NON_SLEEP;
        mccidl_need_full_wakeup = TRUE;
        mccidl_cheap_idle_ho = FALSE;
        mccidl_reg_required = FALSE;

        mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
        new_state = CDMA_EXIT;
      }
      break;
     
    default:
      ERR ("Unexpected SRCH Rpt %d at MCC_WAKING_UP State %d ",
            rpt_ptr->hdr.rpt, 0, 0);
      break;
  }
  return (new_state);

}/* mccidl_waking_up_proc_srch_rpt */

/*===========================================================================

FUNCTION MCCIDL_WAIT_FOR_SLEEP_PROC_SRCH_RPT

DESCRIPTION
  This procedure processes the search report at MCC_WAIT_FOR_MORE_SLEEP state.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_wait_for_sleep_proc_srch_rpt
(
  mccsrch_rpt_type *rpt_ptr,
    /* Report received from searcher */
  word new_state
    /* Next state to be processed */
)
{
  int i, j;    
    /* used as local loop variable for system reselection */
  sd_sys_s_type sys_list[CAI_GEN_ANALOG_NGHBR_MAX];
    /* List of system for reselection */
    
  switch (rpt_ptr->hdr.rpt)
  {
    case SRCH_FAST_WAKE_R:    /* Completely Waking up from sleep */
      mccidl_sleep_state = MCC_NON_SLEEP;

      /* Set mccidl_sleep_ok to False. So we don't start_sleep until
       * we get "RXC_SLOT_R" or General Page Message or Slotted page
       * message which says we can sleep again.
       */
      mccidl_sleep_ok = FALSE;

      MSG_LOW("No match - Must full wakeup", 0,0,0);

      /* have SRCH transition from sleep to idle state */
      mccidl_send_srch_idle_cmd();
      break;

    case SRCH_FAST_SLEEP_R:
      /* A valid page channel message is received, reset the time stamp
       * for T31m. See IS-95A section 6.6.2.2. */
      ts_get_20ms_frame_time(cur_bs_ptr->rx.msg_time);

      /* Go back to sleep again without a complete wakeup */
      mccidl_sleep_state = MCC_SLEEP;

      /* Search is already asleep at this point
      ** Tell RXC to go night-night, too
      */
      mcc_rx_buf.sleep.hdr.command = RXC_SLEEP_F;
      mcc_rx_buf.sleep.hdr.cmd_hdr.done_q_ptr = NULL;
      mcc_rxc_cmd( &mcc_rx_buf);

      MSG_LOW("Staying asleep",0,0,0);
      break;

    case SRCH_SYS_MEAS_R:
      /* Report system measurements to System Determination */
      mcc_sys_meas_rpt(rpt_ptr);
      /* --------------------------------------------------------------
      ** Inform system determination of done with reporting system
      ** measurement.
      ** Determine the next state to be processed based on the returned
      ** SS-Action.
      ** -------------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_misc_meas_rprt(rpt_ptr->sys_meas.meas_id, TRUE, NULL)))
      {
        MSG_LOW("MC needs to wake up for reselection",0,0,0);

        /* Setting the default values before Exiting to SD */
        mccidl_sleep_state = MCC_NON_SLEEP;
        mccidl_need_full_wakeup = TRUE;
        mccidl_cheap_idle_ho = FALSE;
        mccidl_reg_required = FALSE;

        mcc_exit.exit = MCC_RESELECTION;
        new_state = CDMA_EXIT;
        event_report(EVENT_PREF_SYS_RESEL);
      }
      /* Wake up Search to speed up background system measurement */
      else if (sd_ss_act_get(NULL) == SD_SS_ACT_MEAS_BACK)
      {
        mccidl_wakeup_sys_meas();
      }
      break;

    case SRCH_REQUEST_SYS_RESTART_R:
      /* -------------------------------------------------------------
      ** Searcher requested system restart. Need to do system exit .
      ** Inform system determination of system lost and determines the
      ** next state to be processed based on the returned SS-Action.
      ** ------------------------------------------------------------- */
      if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_ADVISE_UNLOCK_RF, NULL ) )
         )
      {
        MSG_HIGH( "SRCH request sys restart", 0, 0, 0 );

        /* Setting the default values before Exiting to SD */
        mccidl_sleep_state = MCC_NON_SLEEP;
        mccidl_need_full_wakeup = TRUE;
        mccidl_cheap_idle_ho = FALSE;
        mccidl_reg_required = FALSE;

        mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
        new_state = CDMA_EXIT;
      }
      break;

    case SRCH_THRESH_R:     /* Pilot power below threshold, exit to SD */
      MSG_HIGH ( "Pilot power below threshold!", 0, 0, 0 );
      memset(sys_list, 0, sizeof(sd_sys_s_type) * CAI_GEN_ANALOG_NGHBR_MAX);  
      /* -----------------------------------------------------------------
      ** Inform system determination of system re-selection.
      ** Determines the next state to be processed based on returned
      ** SS-Action.
      ** ----------------------------------------------------------------- */
      for (i=0, j=0; i < cur_bs_ptr->anl.num_analog_nghbr; i++)
      {
        if (cur_bs_ptr->anl.anl_nghbr[i].sys_a_b & 0x01)
        {
          sys_list[j].mode = SD_MODE_AMPS;
          sys_list[j].band = cur_bs_ptr->anl.anl_nghbr[i].band_class;
          sys_list[j].chan = SD_CELL_SYS_A; /*lint !e641 */
          j++;
        }

        if (cur_bs_ptr->anl.anl_nghbr[i].sys_a_b & 0x02)
        {
          sys_list[j].mode = SD_MODE_AMPS;
          sys_list[j].band = cur_bs_ptr->anl.anl_nghbr[i].band_class;
          sys_list[j].chan = SD_CELL_SYS_B; /*lint !e641 */
          j++;
        }
      }

      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_opr_sys_resel(sys_list, j, NULL)))

      {
        /* Setting the default values before Exiting to SD */
        mccidl_sleep_state = MCC_NON_SLEEP;
        mccidl_need_full_wakeup = TRUE;
        mccidl_cheap_idle_ho = FALSE;
        mccidl_reg_required = FALSE;

        /* Exit to the SD with MCC_SYSTEM_RESELECTION exit reason */
        mcc_exit.exit = MCC_SYSTEM_RESELECTION ;
        new_state = CDMA_EXIT;
      }

      else
      {
        /* Tell SRCH to continue on current system */
        mcc_srch_buf.sys_reselect.hdr.cmd = SRCH_RESELECT_MEAS_F;
        mcc_srch_cmd(&mcc_srch_buf);

        /* Wake up Search to speed up background system measurement */
        if (sd_ss_act_get(NULL) == SD_SS_ACT_MEAS_BACK)
        {
          mccidl_wakeup_sys_meas();
        }
      }

      /* Events report: System reselection */
      event_report(EVENT_SYSTEM_RESELECTION);
      break;
      
    default:
      ERR ("Unexpected SRCH Rpt %d at MCC_WAITING_FOR_MORE_SLEEP State %d ",
            rpt_ptr->hdr.rpt, 0, 0);
      break;
  }
  return (new_state);

}/* mccidl_wait_for_sleep_proc_srch_rpt */

/*===========================================================================

FUNCTION MCCIDL_SLOTTED_PROC_SRCH_RPT

DESCRIPTION
  This procedure processes the search report processing during Slotted Sleep
  based on the state machine variable of mccidl_sleep_state.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_slotted_proc_srch_rpt
(
  mccsrch_rpt_type *rpt_ptr,
    /* Report received from searcher */
  word new_state
    /* Next state to be processed */
)
{
  switch (mccidl_sleep_state)
  {
    case MCC_SLEEP:
      new_state = mccidl_sleep_proc_srch_rpt(rpt_ptr, new_state);
      break;
    case MCC_WAKING_UP:
      new_state = mccidl_waking_up_proc_srch_rpt(rpt_ptr, new_state);
      break;
    case MCC_WAIT_FOR_MORE_SLEEP:
      new_state = mccidl_wait_for_sleep_proc_srch_rpt(rpt_ptr, new_state);
      break;
    default:
      ERR ("Function is Called at Wrong State", 0, 0, 0);
      break;
  }

  /* clean up report processing */
  cmd_done( &rpt_ptr->hdr.rpt_hdr );

  return (new_state);

}/* mccidl_slotted_proc_srch_rpt */

/*===========================================================================

FUNCTION MCCIDL_SLOTTED_PROC_HS_CMD

DESCRIPTION
  This function processes commands received from the handset while in the
  slotted mode operation. Only commands that do not require RF reource are
  processed here while others are processed in non-slotted mode.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_slotted_proc_hs_cmd
(
  mc_msg_type *cmd_ptr,
    /* pointer to message received from the handset */
  word new_state,
    /* next state to be processed */
  boolean *cmd_proc_needed_rf_resource
    /* Indicates if RF resource needed to continue command processing */ 
)
{
  sd_ss_act_e_type ss_action;
#ifdef FEATURE_OPERATIONAL_MODE
  sd_ss_act_s_type ss_act_param;
#endif

  switch ( cmd_ptr->hdr.cmd )
  {
    case MC_END_F:
    {
      MSG_MED( "Rxed END_F in slotted mode", 0, 0, 0 );

      cmd_ptr = (mc_msg_type *) q_get( &mc_cmd_q );

      /* Call not in progress, bad command. */
      cmd_ptr->hdr.status = MC_BADPARM_S;

      /* Clean up command processing */
      cmd_done (&cmd_ptr->hdr.cmd_hdr);
      break;
    }

    #ifdef FEATURE_BROADCAST_SMS
    case MC_BCSMS_ENABLE_F:
    {
      MSG_MED( "Rxed BCSMS_ENABLE = %d in slotted mode", 
        cmd_ptr->bcsms_enable.enable, 0, 0 );

      cmd_ptr = (mc_msg_type *) q_get( &mc_cmd_q );
      
      /* Update BCSMS enable/disable status */
      mccbcsms_enable(cmd_ptr->bcsms_enable.enable);

      cmd_ptr->hdr.status = MC_DONE_S;

      /* Clean up command processing */
      cmd_done (&cmd_ptr->hdr.cmd_hdr);
      break;
    }
    #endif /* FEATURE_BROADCAST_SMS */

    case MC_CDMA_PROT_DEACT_CMD_F:
    {
      if ( ( cmd_ptr->deact_prot.info.reason == PROT_DEACT_PWR_DOWN ) ||
           ( cmd_ptr->deact_prot.info.reason == PROT_DEACT_LPM ) ||
	   ( cmd_ptr->deact_prot.info.reason == PROT_DEACT_SUBSC_CHGD && 
	     otasp_is_successful_commit()) 
	 )
      {
        MSG_MED( "Rxed PROT_DEACT with PWR_DOWN/LPM ind in slotted mode", 0, 0, 0 );
       
        if ( mccreg_idle_powerdown() == MCCREG_NO_REG_REQUIRED )  
        {
          MSG_MED( " PWR_DOWN REG not required", 0, 0, 0 );

          cmd_ptr = (mc_msg_type *) q_get( &mc_cmd_q );

          mcc_exit.mmoc_trans_id = cmd_ptr->deact_prot.info.trans_id;
          mcc_exit.exit = mc_map_deact_to_mcc_exit_reason( 
            cmd_ptr->deact_prot.info.reason );

          new_state = CDMA_EXIT;

          cmd_ptr->hdr.status = MC_DONE_S;

          /* Clean up command processing */
          cmd_done (&cmd_ptr->hdr.cmd_hdr);
        }
        else
        {
          MSG_MED( " PWR_DOWN REG required, delay proc till RF resource available", 0, 0, 0 );
          *cmd_proc_needed_rf_resource = TRUE;
        }
      }

      /* Deactivation is not because of power down or LPM. */
      else
      {
        MSG_MED( "Rxed PROT_DEACT with reason %d in slotted mode", 
          cmd_ptr->deact_prot.info.reason, 0, 0 );

        cmd_ptr = (mc_msg_type *) q_get( &mc_cmd_q );

        mcc_exit.mmoc_trans_id = cmd_ptr->deact_prot.info.trans_id;
        mcc_exit.exit = mc_map_deact_to_mcc_exit_reason( 
          cmd_ptr->deact_prot.info.reason );

        new_state = CDMA_EXIT;

        cmd_ptr->hdr.status = MC_DONE_S;

        /* Clean up command processing */
        cmd_done (&cmd_ptr->hdr.cmd_hdr);
      }

      break;
    }

    case MC_CDMA_PROT_GEN_CMD_F:
    {
      MSG_MED( "Rxed PROT_GEN cmd with reason %d in slotted mode", 
               cmd_ptr->gen_cmd.info.cmd_type, 0, 0 );

      /* Process the MMOC generic protocol command */
      ss_action = mmoc_proc_prot_gen_cmd( TRUE, TRUE, &cmd_ptr->gen_cmd.info );

#ifdef FEATURE_OPERATIONAL_MODE
      if (ss_action == SD_SS_ACT_PWR_SAVE)
      {
        /* Call into SD to determine the action.  Note that sd_ss_act_get
           will only return the correct action for some MMOC generic commands;
           so just call it if this is a power save. */
        sd_ss_act_get(&ss_act_param);
      }

      if ((ss_action == SD_SS_ACT_PWR_SAVE) && 
          (ss_act_param.prm.pwr_save.reas == SD_SS_PWR_SAVE_REAS_PWR_DOWN))
      {
        /* 1X is being deactivated.  Perform powerdown registration. */
        if ( mccreg_idle_powerdown() == MCCREG_NO_REG_REQUIRED )
        {
          MSG_MED( " PWR_DOWN REG not required", 0, 0, 0 );
          cmd_ptr = (mc_msg_type *) q_get( &mc_cmd_q );
          mcc_exit.exit = MCC_SHUTDOWN;
          new_state = CDMA_EXIT;
          cmd_ptr->hdr.status = MC_DONE_S;

          /* Clean up command processing */
          cmd_done (&cmd_ptr->hdr.cmd_hdr);
        }
        else
        {
          MSG_MED( " PWR_DOWN REG required, delay proc till RF resource available", 0, 0, 0 );
          *cmd_proc_needed_rf_resource = TRUE;
        }
      }
      else
#endif /* FEATURE_OPERATIONAL_MODE */
      {
        cmd_ptr = (mc_msg_type *) q_get( &mc_cmd_q );
        /* Determine if we need to exit. */
        if (!mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED, ss_action ))
        {
          mcc_exit.exit = 
            mc_map_gen_cmd_to_mcc_exit_reason(&cmd_ptr->gen_cmd.info );
          new_state = CDMA_EXIT;
        }
        cmd_ptr->hdr.status = MC_DONE_S;

        /* Clean up command processing */
        cmd_done (&cmd_ptr->hdr.cmd_hdr);
      }

      break;
    }

    case MC_ORIGINATION_F:
    case MC_MO_DBM_F:

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
    #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */
    {
      MSG_MED( "Rxed MC cmd %d in slotted mode, delay proc till RF resource available", 
        cmd_ptr->hdr.cmd, 0, 0 );
      *cmd_proc_needed_rf_resource = TRUE;
      break;
    }

    default:
    {
      ERR( "Invalid MC cmd %d", cmd_ptr->hdr.cmd, 0, 0 );

      cmd_ptr = (mc_msg_type *) q_get( &mc_cmd_q );
      cmd_ptr->hdr.status = MC_BADPARM_S;

      /* Clean up command processing */
      cmd_done (&cmd_ptr->hdr.cmd_hdr);
      break;
    }
  }

  return (new_state);
 
} /* mccidl_slotted_proc_hs_cmd */

/*===========================================================================

FUNCTION MCCIDL_START_SLEEP

DESCRIPTION
  This function sends a command to the searcher task to go to sleep.

DEPENDENCIES
  None.

RETURN VALUE
  New state.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_start_sleep
(
  word new_state
    /* Next state to be processed */
)
{
  mccsrch_rpt_type *rpt_ptr;
    /* Report received from searcher */

  mc_msg_type *cmd_ptr;
    /* Command from MC command queue */

  boolean clr_config_msg_seq_upon_wakeup = FALSE;
    /* Indicates whether config msg seq need to be cleared upon full wakeup */

  boolean cmd_proc_needed_rf_resource = FALSE;
    /* Indicates whether RF resource is needed to continue MC command processing */

  int last_tot_num_mc_cmd_queued = 0;
    /* Total number of commands queued in MC command queue */   
  
  dword wait_mask; /* Mask of signals to wait on */

  dword sigs; /* Signals returned by mcc_wait */

  #ifdef FEATURE_BROADCAST_SMS
  /* Before sending the sleep command, check the broadcast
  ** schedules.
  */
  mccbcsms_schedule_bcast();
  #endif /* FEATURE_BROADCAST_SMS */

  /* ---------------------------------------------------------
  ** Mark sleep as attempted here even if we don't actually go
  ** to sleep so that we don't keep hammering on the searcher.
  ** --------------------------------------------------------- */
  mccidl_sleep_ok = FALSE;

  /* Reset the Re-Acq fail retry count to the max value to indicate
     we had no REACQ failures yet*/
  mccidl_rafail_retry_cnt = MCCIDL_MAX_RAFAIL_RETRY_CNT;

  /* Build a command to the Search task to sleep */
  mcc_srch_buf.sleep.hdr.cmd = SRCH_SLEEP_F;
  mcc_srch_buf.sleep.hdr.cmd_hdr.task_ptr = NULL;
  mcc_srch_buf.sleep.hdr.cmd_hdr.done_q_ptr = NULL;
  mcc_srch_buf.sleep.cur_slot_mask = sleep_slot_mask;

  #ifdef FEATURE_IS2000_REL_A_CC
  /* Universal Page Message's READ_NEXT_SLOT or READ_NEXT_SLOT_BCAST is set */
  if ( mccidl_monitor_next_slot 

       /* In Pseudo-Idle Mode, as long as MS receives OTA with config_msg_seq. MS
          shall goes back to sleep as soon as overheads are current. */
       && ( cdma.entry_ptr->entry != MCC_PSEUDO_IDLE )
     )
  {
    mcc_srch_buf.sleep.monitor_next_slot = TRUE;
    mccidl_monitor_next_slot = FALSE;
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */

  {
    mcc_srch_buf.sleep.monitor_next_slot = FALSE;
  }

  /* One last check to see if the searcher wants to handoff */
  if ((rpt_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) == NULL)
  {
    /* send the command to the Search task to sleep */
    mcc_srch_cmd(&mcc_srch_buf);
    sleep_slot_mask = 0; /* clear the mask, since it's already sent to SRCH */

    /* If not SRCH_DONE_S, for 'sleep' this means go back to Idle State */
    if (mcc_srch_buf.sleep.hdr.status == SRCH_DONE_S)
    {
      MSG_LOW("send RXC SLEEP_F",  0, 0, 0);
      /* Send a command to Receive to start sleep processing */
      mcc_rx_buf.sleep.hdr.command = RXC_SLEEP_F;
      mcc_rx_buf.sleep.hdr.cmd_hdr.done_q_ptr = NULL;
      mcc_rxc_cmd(&mcc_rx_buf);

      mccidl_sleep_active = TRUE;

      /* Going to sleep, disable MER statistical operation for page slot. */
      mclog_mer_config(FALSE, 0);

      /* Use MC's slotted sleep procedure for the Enhanced Standby Searcher */
      /* enter "MCC_SLEEP" state and Wait for searcher Rpt. */
      mccidl_sleep_state = MCC_SLEEP;

      mccidl_need_full_wakeup = FALSE;
      mccidl_fast_raho_performed = FALSE;

      #ifdef FEATURE_FULL_TIME_SHDR_3
      /* MS is ready to go to sleep (releasing RX0), disable the MC DDTM. 
      ** If CM DDTM is active, then that will continue to take effect */
      mcc_set_mc_ddtm_status( FALSE );
      #endif /* FEATURE_FULL_TIME_SHDR_3 */


      while (mccidl_sleep_state != MCC_NON_SLEEP)
      {
        wait_mask = MCC_SRCH_Q_SIG;

        /* RF resource not needed to continue MC command processing. */
        if ( !cmd_proc_needed_rf_resource )
        {
          wait_mask |= MCC_CMD_Q_SIG;
        }
       
        sigs = mcc_wait( wait_mask );

        if ( ( sigs & MCC_SRCH_Q_SIG ) != 0 )
        {
          if ( ( rpt_ptr = (mccsrch_rpt_type *) q_get( &mcc_srch_q ) ) != NULL )
          {
            /* ---------------------------------------------------------------
            ** MC command processing suspended because of waiting for RF 
            ** resource.  New MC command enqueued, call cm_mc_unlock_resources
            ** to let CM check whether there is any new requirement to unlock 
            ** HDR RF resource. 
            ** --------------------------------------------------------------- */
            if ( cmd_proc_needed_rf_resource &&
                 ( last_tot_num_mc_cmd_queued < q_cnt( &mc_cmd_q ) ) )
            {
              MSG_MED( "Rude wakeup", 0, 0, 0 );
              srch_rude_wakeup( TRUE );

              /* Remember the total number of MC commands queued at the moment. 
                 This is used to determine when srch_rude_wakeup and
                 cm_mc_unlock_resources to be call again if new MC comamnd
                 enqueued. */
              last_tot_num_mc_cmd_queued = q_cnt( &mc_cmd_q );

              /* Unlock HDR RF resource for 1x wakeup if CM sees the need. */
              cm_mc_unlock_resources();
            } 

            /* process the SRCH report for the State "MCC_SLEEP",
             * "MCC_WAKING_UP" and "MCC_WAIT_FOR_MORE_SLEEP".
             * Note that the function mccidl_slotted_proc_srch_rpt() will change
             * not only the CDMA state but also the MC slotted
             * sleep state "mccidl_sleep_state".
             */
            MSG_LOW("Rxed SRCH rpt %d at Sleep State %d",
              rpt_ptr->hdr.rpt, mccidl_sleep_state, 0);

            new_state = mccidl_slotted_proc_srch_rpt (rpt_ptr, new_state);

            MSG_LOW("MCC transitions to Sleep State %d",
              mccidl_sleep_state, 0, 0);

            if (mccidl_fast_raho_performed)
            {
              clr_config_msg_seq_upon_wakeup = TRUE;
            }
          }

          if ( mccidl_fast_raho_performed && mccidl_reacq_after_fast_raho )
          {

            #if (defined(FEATURE_HIGH_PSIST) && defined (FEATURE_PASSPORT_MODE))
            if(mcc_passport_mode_is_jcdma())
            {
              
              /* If we performed a fast raho while sleeping, send CM
                 the updated psist info. */

              #ifdef FEATURE_IS2000_REL_A_CC
              if ( OVHD_CHAN_IS_BCCH )
              {
                if (ENH_ACC_PARM_MSG_IS_RECVD)
                {
                  mccidl_update_psist_info(CAI_EACH, &cur_bs_ptr->eapm);
                }
              }
              else
              #endif /* FEATURE_IS2000_REL_A_CC */
              {
                if (AP_MSG_IS_RECVD)
                {
                   mccidl_update_psist_info(CAI_ACH, &cur_bs_ptr->ap);
                }
              }
            }
            #endif /* FEATURE_HIGH_PSIST && FEATURE_PASSPORT_MODE */
        
            if ((mccidl_sleep_state == MCC_NON_SLEEP)
                || ((mccidl_sleep_state == MCC_SLEEP) && mccidl_cheap_idle_ho))
            {
              /* We are either fully waking up or we are back
                 to sleep. Send Search the overhead information
                 suppressed during fast-RAHO */

              MSG_MED("Perform supressed action by fast RAHO", 0, 0, 0);

              /* Update Searcher's handoff parameters */
              mccidl_send_srch_parms();

              /* Update Searcher's neighbor set */
              mccidl_send_srch_idle_nset();

              #ifdef FEATURE_BROADCAST_SMS
              /* Post fast RAHO processing for broadcast SMS. If bcast_index
               * or non-primary BCCH channel configuration has changed, we 
               * need to cancel the previous bcast schedule.
               */
              mccbcsms_post_fraho_proc();
              #endif /* FEATURE_BROADCAST_SMS */

              /* Process CDMA Channel List Message if we fast RAHOed to a BS
               * that hashed the mobile to a different Frequency
               */
              #ifdef FEATURE_IS2000_REL_A_CC
              if (OVHD_CHAN_IS_BCCH)
              {
                if (EXT_CHAN_LIST_MSG_IS_RECVD)
                {
                  /* note that mccidl_proc_ecl_msg() may change
                     mccidl_cheap_idle_ho. */
                  new_state = mccidl_proc_ecl_msg(new_state);
                }
              }
              else
              #endif /* FEATURE_IS2000_REL_A_CC */

              {
                if (EXT_CHAN_LIST_MSG_IS_RECVD)
                {
                  /* note that mccidl_proc_ecl_msg() may change
                     mccidl_cheap_idle_ho. */
                  new_state = mccidl_proc_ecl_msg(new_state);
                }

                if (CHAN_LIST_MSG_IS_RECVD)
                {
                  /* note that mccidl_proc_cl_msg() may change
                     mccidl_cheap_idle_ho. */
                  new_state = mccidl_proc_cl_msg(new_state);
                }
              }
           
              /* ----------------------------------------------------
              ** Cheap idle handoff shall imply overhead messages are
              ** all updated. But to be safe, do sanity check on the 
              ** status of overhead.
              ** ---------------------------------------------------- */  
              if (mccidl_cheap_idle_ho && mcc_is_ovhd_cfg_okay())
              {
                MSG_MED("Cheap RAHO, process epzid", 0, 0, 0);

                /* Overhead messages are all updated. Stores new SID, NID and PZID and
                   notify its reception if a handler function is registered. */
                mccsyobj_process_epzid(cur_bs_ptr->csp.sp.sid, cur_bs_ptr->csp.sp.nid, 
                  (P_REV_IS_4_OR_GREATER ? cur_bs_ptr->csp.esp.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN), CDMA_IDLE);

                #ifdef FEATURE_IS2000_REL_D
#error code not present
                #endif /* FEATURE_IS2000_REL_D */

                /* Mark EPZID processing is not pending */
                mccidl_epzid_proc_pending = FALSE;

                /* -----------------------------------------------------------
                ** Following is the serving system info update for CM. It is
                ** done once we acquire a service.
                ** Notice that Digital indicator is on, CM can safely poll it
                ** to know whether the data is for CDMA or for AMPS. Also
                ** NO_SVC is set correctly now.
                ** ----------------------------------------------------------- */
                mcc_send_cm_srv_sys_info( CM_SS_SID_MASK|
                                          CM_SS_NID_MASK|
                                          CM_SS_REG_ZONE_MASK|
                                          CM_SS_PACKET_ZONE_MASK|
                                          CM_SS_BS_P_REV_MASK|
                                          CM_SS_P_REV_IN_USE_MASK, /*lint !e747 */
                                          cur_bs_ptr->csp.sp.sid,
                                          cur_bs_ptr->csp.sp.nid,
                                          cur_bs_ptr->csp.sp.reg_zone,
                                          (P_REV_IS_4_OR_GREATER ? cur_bs_ptr->csp.esp.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN)
                                        );
              }

              /* We've completed the fast-RAHO. */
              mccidl_fast_raho_performed = FALSE;
            }
          }
        } /* end if ( ( sigs & MCC_SRCH_Q_SIG ) != 0 ) */

        else if ( ( sigs & MCC_CMD_Q_SIG ) != 0 )
        {
          if ( ( cmd_ptr = (mc_msg_type *) q_check( &mc_cmd_q ) ) != NULL )
          {
            new_state = mccidl_slotted_proc_hs_cmd( cmd_ptr, new_state,
              &cmd_proc_needed_rf_resource );
          }

          /* MC command processing resulted in system exit. */
          if ( new_state == CDMA_EXIT )
          {
            /* Setting the default values before Exiting to SD */
            mccidl_sleep_state = MCC_NON_SLEEP;
            mccidl_need_full_wakeup = TRUE;
            mccidl_cheap_idle_ho = FALSE;
            mccidl_reg_required = FALSE;
            mccidl_sleep_state = MCC_NON_SLEEP;
          }

          /* Need RF resource to continue MC command processing. */
          else if ( cmd_proc_needed_rf_resource )
          {
            MSG_MED( "Rude wakeup", 0, 0, 0 );
            srch_rude_wakeup( TRUE );

            /* Remember the total number of MC commands queued at the moment. 
               This is used to determine when srch_rude_wakeup and
               cm_mc_unlock_resources to be call again if new MC comamnd
               enqueued. */
            last_tot_num_mc_cmd_queued = q_cnt( &mc_cmd_q );

            /* Unlock HDR RF resource for 1x wakeup if CM sees the need. */
            cm_mc_unlock_resources();
          }

        } /* end if ( ( sigs & MCC_CMD_Q_SIG ) != 0 ) */

      }  /* End of while (mccidl_sleep_state != MCC_NON_SLEEP) */

      mccidl_sleep_active = FALSE;
 
      /* --------------------------------------------------------
      ** Overhead reprocessing after expensive fast RAHO may need
      ** channel supervision update.
      ** -------------------------------------------------------- */
      mccidl_supervise_channel_monitoring(new_state, NULL, FALSE);

      if (new_state != CDMA_EXIT)
      {
        if (new_state == CDMA_IDLE_INIT) /*lint !e641 */
        { 
          /* Start BS insanity timer */
          mccidl_set_bs_insanity_timer();
        }

        if (clr_config_msg_seq_upon_wakeup)
        {
          /* Clear config_msg_seq to make sure overhead is current before
             next sleep. */
          cur_bs_ptr->rx.config_msg_seq = INVALID_SEQ;
          MSG_MED("NS clr cfg seq", 0, 0, 0); 
        }
      }
    }
    else if ( mcc_srch_buf.sleep.hdr.status == SRCH_TOOLATE_S )
    {
      MSG_MED("MC rec'd SRCH_TOOLATE_S",0,0,0);

      #ifdef FEATURE_FULL_TIME_SHDR_3
      if (BIO_GET_SW(BIO_SW9_M)) 
      {
        /* Dip switch is on, Sleep is diabled.  Make sure HDR declares
        ** system lost.  And disable the MC DDTM so that accessing the
        ** network on RX0 is possible.
        ** If CM DDTM is active, it will continue to take effect. */
        cm_unlock_hdr_resources();
        mcc_set_mc_ddtm_status(FALSE);
      }
      #endif /* FEATURE_FULL_TIME_SHDR_3 */

      /* -------------------------------------------------------------
      ** We were too late to sleep, but still send RXC_SLOT_F to make
      ** sure RXC will report receipt of a valid message in our 2 slot
      ** window.
      ** ------------------------------------------------------------- */
      /* the new SRCH code sends WAKEUP_R at every slot boundry,
         nothing needs to be done at this point
      mccidl_wakeup(FALSE, FALSE, mcc_srch_buf.sleep.cur_slot_mask,
                    (word)qw_lo(cur_slot)); */

      #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
      /* If the mobile is currently monitoring the secondary BCCH, */
      /* and if Search sends too late for the sleep command, CP    */
      /* needs to configure the Search and RXC back to the F-CCCH. */
      /* If Search OKs with the sleep command, it will config      */
      /* RXC upon the next wakeup slot, either to F-CCCH or BCCH.  */
      if ((active_f_csch_info.channel_monitor_mask & MCC_NON_PRI_BCCH) != 0)
      {
        mccidl_supervise_channel_monitoring(new_state, NULL, FALSE);
      }
      #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
    }
  }
  else
  {
    /* process the searcher report which is not a wakeup */
    new_state = mccidl_proc_srch_rpt(rpt_ptr, new_state);
  }

  return (new_state);

} /* mccidl_start_sleep */

/*===========================================================================

FUNCTION MCCIDL_CHECK_CHAN

DESCRIPTION
   If we are planning on going to PAGE_RES or ORD_RES state, this function
   checks if we have have changed channels since the page or order/message
   was received.  If we have we should go back to Idle State and wait for a
   message on the new channel.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_check_chan
(
  word new_state
    /* The next state to be processed */
)
{
  if ( ( ( new_state == CDMA_PAGE_RES ) && !page_res_retransmission ) /*lint !e641*/
       || ( new_state == CDMA_ORD_RES ) ) /*lint !e641 */
  {
    if ((OVHD_CHAN_IS_PCH &&
        ((msg_rxed_chan.cdmach != cdma.cdmach)  ||
         (msg_rxed_chan.chan_type != CAI_PCH)||
         (msg_rxed_chan.chan_id != cdma.pagech)))
        #ifdef FEATURE_IS2000_REL_A_CC
        || (OVHD_CHAN_IS_BCCH &&
            ((msg_rxed_chan.cdmach != cdma.cdmach) ||
             (msg_rxed_chan.chan_type != CAI_F_CCCH) ||
             (msg_rxed_chan.chan_id != cdma.fccch_id)))
        #endif /* FEATURE_IS2000_REL_A_CC */
       )
    {
      new_state = CDMA_IDLE_INIT; /*lint !e641 */
      cdma.sa_tc_state = CDMA_IDLE;

      /* Start BS insanity timer */
      mccidl_set_bs_insanity_timer();

      #ifdef FEATURE_IS2000_REL_D
#error code not present
      #endif /* FEATURE_IS2000_REL_D */

    }
  }

  return (new_state);

} /* mccidl_check_chan */

/*===========================================================================

FUNCTION MCCIDL_IDLE_INIT

DESCRIPTION
  Idle State initialization processing.
  Sets timers and indications, sets up for slotted or unslotted mode,
  sets Slot Cycle Index, and performs other initialization processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccidl_idle_init(void)
{
  #ifdef FEATURE_AUTH
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  #endif /* FEATURE_AUTH */

  /* Variable that stores remaining time on an already runnning
     slotted timer  (if any) */
  dword remaining_t_slotted_time = 0;

  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #endif /* FEATURE_IS2000_REL_A_AES */


  /* Calculate HASH_KEY (see IS-95 section 6.6.7.1) */
  cdma.imsi_hash_key = cdma.imsi_s1 + (((dword) cdma.imsi_s2) << 24);
  
  // Note: this key has to be remembered for QPCH operation
  cur_bs_ptr->imsi_hash_key = cdma.imsi_hash_key;

  /* Send a message to RX to start using the Idle fade timer */
  mcc_rx_buf.idl_fade.hdr.command = RXC_IDL_FADE_F;
  mcc_rx_buf.idl_fade.hdr.cmd_hdr.done_q_ptr = NULL;
  mcc_rxc_cmd(&mcc_rx_buf);

  /* Send a message to tell TX to go back Idle state in case it got stuck
   * in the TXC_JUMP_STATE. If it is already in the Idle state, TXC will 
   * return BAD_CMD status. */
  mccsa_tx_stop();

  /* ---------------------------------------------------------------------
  ** Reset Handoff state to Idle HO whenever the Mobile Enters IDLE state.
  ** --------------------------------------------------------------------*/
  cdma.ho_state = CDMA_IDLE_HO;

  /* when the mobile comes into idle init, clear the transceiver request pending
   * request flag
   */
  cdma.transceiver_request_pending = FALSE;

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  /* -----------------------------------------------------------------------
  ** SRCH will be notified of the IDLE state information whenever MC enters
  ** IDLE state.  This resets the SRCH back to IDLE from any previous
  ** Access Handoffs states (AEHO, APHO/AHO). It enables SRCH to search all
  ** the neighbors for Idle Handoffs.
  ** ---------------------------------------------------------------------*/
  mccidl_send_srch_idle_cmd();
  MSG_MED( "SENDING SRCH_IDLE_F to the SRCH",0,0,0 );

  /* ---------------------------------------------------
  ** Check if station class mark indicates slotted mode.
  ** If so set up for slotted mode and set Slot Cycle
  ** Index, otherwise set up for unslotted mode.
  ** --------------------------------------------------- */
  if ((cdma.scm & SCM_SLOTTED_CLASS_M) == SCM_SLOTTED_V)
  {
    /* Set up for slotted operation */

    /* -------------------------------------------------
    ** At this point you can only operate in the smaller
    ** of MAX_SLOT_CYCLE or 1 if we haven't registered.
    ** ------------------------------------------------- */

    if ( !mcc_set_operational_sci() )
    {
      /* ---------------------------------------------------------
      ** We have no clue what slot messages might come in, and we
      ** won't go to slotted mode until we get a system parameters
      ** message anyway, so set it to zero for now.
      ** --------------------------------------------------------- */
      slot_cycle_index_s = 0;
    }

    sci_mask = mcc_calculate_sci_mask(slot_cycle_index_s);    
    idle_pgslot = hash(cdma.imsi_hash_key, 2048,
                       (word) (6*(cdma.imsi_hash_key & 0xfff)));

    #ifdef FEATURE_HDR
#error code not present
    #endif /* FEATURE_HDR */

    /* Update search's parameters */
    mccidl_send_srch_parms();

    /* Update Searcher's neighbor set */
    mccidl_send_srch_idle_nset();

    /* ------------------------------------------------------------
    ** Set SLOTTED s to YES if T_SLOTTED s is equal to 00000000
    ** or if a previously started slotted timer is still running
    ** or if the mobile station does not support the slotted timer.
    ** Otherwise set the slotter timer duration with t_slotted.
    ** -----------------------------------------------------------*/

   if ((cdma.t_slotted != 0) || 
       ((remaining_t_slotted_time = rex_get_timer(&mcc_slotted_timer))!=0)
      )
    {
      if (cdma.t_slotted)
      {
        /* Set the t_slotted timer, multiply by 80 for number of msec */
        MSG_MED("Set Slotted time %d (80ms unit)", cdma.t_slotted, 0, 0);
        (void)rex_set_timer (&mcc_slotted_timer, (word)cdma.t_slotted * 80);
      }
      else
      {
        MSG_HIGH("Slotted Timer running Rem_time = %d",remaining_t_slotted_time , 0, 0);
      }
      /* ----------------------------------------------------------------
      ** Mobile station shall not go to sleep until slotted timer expired
      ** ---------------------------------------------------------------- */
      idle_slotted = FALSE;
      mccidl_sleep_ok = FALSE;
      cdma.t_slotted = 0;
    }
    else
    {
      idle_slotted = TRUE;
      mccidl_sleep_ok = TRUE;
    }
  }
  else
  {
    /* Set variables for non-slotted mode */
    idle_slotted = FALSE;
    mccidl_sleep_ok = FALSE;
  }

  mccidl_reg_required = FALSE;
    /* Initialize reg required in case we do an exit in the near future */

  ts_get_20ms_frame_time(mccidl_sleep_ok_time);
  MSG_LOW("Sleep Ok Time = 0x%x", (word) qw_lo(mccidl_sleep_ok_time), 0, 0);
    /* Ignore any sleep requests generated before now. */

  /* Clear the State timer when the mobile is in IDLE state*/
  (void) rex_clr_timer( &mcc_state_timer );
  (void) rex_clr_sigs( &mc_tcb, MCC_STATE_TIMER_SIG );

  /* Reset the Re-Acq fail retry count to the max value to indicate
     we had no REACQ failures yet*/
  mccidl_rafail_retry_cnt = MCCIDL_MAX_RAFAIL_RETRY_CNT;

  /* -------------------------------------------
  ** Mark that Receive task has NOT been told to
  ** use access timer, that we are entering Idle
  ** State with no pending System Access state.
  ** ------------------------------------------- */
  access_fade_timer_in_use = FALSE;
  cdma.sa_tc_state = CDMA_IDLE;

  /* These flags are only used when re-entering OVHD state from SA */
  oh_sd_rpt_pending_in_upd_ovhd = FALSE;
  page_res_retransmission = FALSE;

  /* -------------------------------------------------------------------
  ** Idle State sets this flag if it leaves a message pending to ACK it.
  ** System Access State will return to CDMA_IDLE_MSG_PENDING state if
  ** it sees this flag set.
  ** ------------------------------------------------------------------- */
  mcc_layer2.idle_msg_pending = FALSE;

  /* We are not attempting a powerdown registration at this time */
  attempting_powerdown_reg = FALSE;

  #ifdef FEATURE_IS2000_REL_A
  /* Clear SR ID info */
  cdma.is_sr_id_valid = FALSE;
  #endif /* FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IS2000_REL_A_CC
  /* Clear monitor next slot flag */
  mccidl_monitor_next_slot = FALSE;
  /* Empty segmented Universal Page Message reassembly buffer. */
  mcc_empty_seg_upm_buffer();
  #endif /* FEATURE_IS2000_REL_A_CC */

  /* Set BS insanity timer */
  mccidl_set_bs_insanity_timer();

  /* Show that we haven't ignore a Global Service Redirection Message */
  gsr_ignored = FALSE;

  /* Show that we haven't ignore a Ext. Global Service Redirection Message */
  egsr_ignored = FALSE;

  #ifdef FEATURE_AUTH
  /* Reset the auth task state each time we enter idle. */
  if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
  {
    auth_cmd_ptr->hdr.command = AUTH_RESET_F;
    auth_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
    auth_cmd( auth_cmd_ptr );
  }
  else
  {
    ERR( "No free buffers on auth_free_q", 0, 0, 0 );
  }
  #endif /* FEATURE_AUTH */

  /* ---------------------------------------
  ** Sync up channel monitoring supervision.
  ** --------------------------------------- */
  mccidl_supervise_channel_monitoring(CDMA_IDLE_INIT, NULL, FALSE); /*lint !e641 */

  #if (defined(FEATURE_HIGH_PSIST) && defined (FEATURE_PASSPORT_MODE))
  if(mcc_passport_mode_is_jcdma())
  {
    /* Initialize to unknown so that we will report the psist at least once */
    mccidl_last_reported_psist_value = CM_PSIST_UNKNOWN;
  }
  #endif /* FEATURE_HIGH_PSIST && FEATURE_PASSPORT_MODE */

  #ifdef FEATURE_OTASP
  /* Reset OTASP call status */
  cdma.otasp_call = NO_SRV_PROV;
  #endif

} /* mccidl_idle_init */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

/*===========================================================================

FUNCTION MCCIDL_OVHD_INIT

DESCRIPTION
  Update Overhead Information Substate initialization processing.  Called
  only when this substate is entered from somewhere other than Idle State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccidl_ovhd_init(void)
{
  /* Indicated that we have to notify other modules on overhead info or
     process ignored redirection/rescan in Update Overhead Information State */
  oh_sd_rpt_pending_in_upd_ovhd = TRUE;

  page_res_retransmission = ((cdma.sa_tc_state == CDMA_PAGE_RES) /*lint !e641 */
                             #ifdef FEATURE_IS2000_REL_D
#error code not present
                             #endif /* FEATURE_IS2000_REL_D */
                            );

  /* ----------------------------------------------------------------
  ** Reset Handoff state to Idle HO whenever the Mobile Enters Update
  ** Overhead Information Substate.
  ** ----------------------------------------------------------------*/
  cdma.ho_state = CDMA_IDLE_HO;

  /* Reset the Re-Acq fail retry count to the max value to indicate
     we had no REACQ failures yet*/
  mccidl_rafail_retry_cnt = MCCIDL_MAX_RAFAIL_RETRY_CNT;

  /* when the mobile comes into overhead init, clear the transceiver request 
   * pending request flag
   */
  cdma.transceiver_request_pending = FALSE;

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  /* Clear state timer */
  (void) rex_clr_timer(&mcc_state_timer);
  (void) rex_clr_sigs(&mc_tcb, MCC_STATE_TIMER_SIG );

  /* Set timer for receiving overhead messages in the Update Overhead
     information state */
  (void) rex_set_timer(&mcc_state_timer, cai_tmo.t41m);

  /* Mark that Receive task is already using Access timer */
  access_fade_timer_in_use = TRUE;

  /* We are not attempting a powerdown registration at this time */
  attempting_powerdown_reg = FALSE;

  /* -------------------------------------------------------------------
  ** This flag should never be set if we re-enter Update Overhead State
  ** from somewhere other than Idle State, but we clear it just in case.
  ** ------------------------------------------------------------------- */
  mcc_layer2.idle_msg_pending = FALSE;

  #ifdef FEATURE_IS2000_REL_A_CC
  /* Clear monitor next slot flag */
  mccidl_monitor_next_slot = FALSE;
  /* Empty segmented Universal Page Message reassembly buffer. */
  mcc_empty_seg_upm_buffer();
  #endif /* FEATURE_IS2000_REL_A_CC */

  /* ---------------------------------------
  ** Sync up channel monitoring supervision.
  ** --------------------------------------- */
  mccidl_supervise_channel_monitoring(CDMA_OVHD, NULL, FALSE); /*lint !e641 */

} /* mccidl_ovhd_init */

/*===========================================================================

FUNCTION MCCIDL_PROC_IGNORED_OVHD

DESCRIPTION
  This function processed rescan or (extended) global service redirection 
  that was ignored because overheads are not current at the time 
  SPM / ANS41SPM / GSRM / EGSRM was received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

word mccidl_proc_ignored_ovhd
(
  word curr_state /* Current state */
)
{
  word new_state = curr_state;
  mcc_redir_info_type redir_info; /* Redirection info */

  /* --------------------------------------------
  ** If we have ignored a rescan, process it now.
  ** -------------------------------------------- */
  if ( rescan_ignored &&
       ( ( OVHD_CHAN_IS_PCH && SP_MSG_IS_RECVD )
         #ifdef FEATURE_IS2000_REL_A_CC
         || ( OVHD_CHAN_IS_BCCH && MCRRP_MSG_IS_RECVD )
         #endif /* FEATURE_IS2000_REL_A_CC */
       )
     )   
  {
    MSG_MED("Proc delayed rescan", 0, 0, 0);

    /* Event report: Rescan */
    event_report(EVENT_RESCAN);

    rescan_ignored = FALSE;

    /* -------------------------------------------
    ** Inform system determination for rescan exit
    ** ------------------------------------------- */
    if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
        sd_ss_ind_cdma_opr_rescan(NULL)))
    {
      mcc_exit.exit = MCC_RESCAN;
      new_state = CDMA_EXIT;
    }
  }

  /* --------------------------------------------------------
  ** If we have ignored a global service redirection message,
  ** process it now.
  ** -------------------------------------------------------- */
  if ( ( new_state != CDMA_EXIT) && gsr_ignored && GSR_MSG_IS_RECVD )
  {
    MSG_MED("Proc delayed GSR", 0, 0, 0);

    gsr_ignored = FALSE;

    /* Events report: Redirection */
    event_report(EVENT_REDIRECTION);

    redir_info.return_if_fail = cur_bs_ptr->gsd.return_if_fail;
    redir_info.num_recs = mccidl_glob_redir_rec_list.num_recs;
    redir_info.redir_rec_list = mccidl_glob_redir_rec_list.rec_list;

    /* ------------------------------------------
    ** Inform system determination of redirection
    ** ------------------------------------------ */
    if (!mcc_inform_sd_redirection(SD_SS_REDIR_CDMA_GSRDM, &redir_info))
    {
      MSG_MED("SD rejected GSRM", 0, 0, 0);
    }

    /* --------------------------------------------------------------
    ** Inform system determination of done with accept / reject
    ** redirection and determine the next state to be processed based
    ** on the returned SS-Action.
    ** -------------------------------------------------------------- */
    if ( !mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
         sd_ss_ind_cdma_opr_acpt_rej(NULL)) )
    {
      new_state = CDMA_EXIT;
      mcc_exit.exit = MCC_REDIRECTION;
    }
  }

  /* -----------------------------------------------------------------
  ** If we have ignored a Extended global service redirection message,
  ** process it now.
  ** ----------------------------------------------------------------- */
  if ( ( new_state != CDMA_EXIT) && egsr_ignored && EGSR_MSG_IS_RECVD )
  {
    MSG_MED("Proc delayed EGSR", 0, 0, 0);

    egsr_ignored = FALSE;

    #ifdef FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND
    if (mccidl_synch_workaround.egsr_is_synch_workaround) 
    {
      if (mccidl_synch_workaround.egsr_rec_index == 0) 
      {
        new_state = mccidl_proc_synch_workaround(new_state,
                      &cur_bs_ptr->egsd.first);
      }

      #ifdef FEATURE_IS2000_REL_D
#error code not present
      #endif /* FEATURE_IS2000_REL_D */

    }
    else
    #endif /* FEATURE_IS2000_REL_A_CC_SYNCH_WORKAROUND */

    {
      /* Events report: Redirection */
      event_report(EVENT_REDIRECTION);
    
      redir_info.return_if_fail = cur_bs_ptr->egsd.return_if_fail;
      redir_info.num_recs = mccidl_glob_redir_rec_list.num_recs;
      redir_info.redir_rec_list = mccidl_glob_redir_rec_list.rec_list;

      /* ------------------------------------------
      ** Inform system determination of redirection
      ** ------------------------------------------ */
      if (!mcc_inform_sd_redirection(SD_SS_REDIR_CDMA_GSRDM, &redir_info))
      {
        MSG_MED("SD rejected EGSRM", 0, 0, 0);
      }
      /* --------------------------------------------------------------
      ** Inform system determination of done with accept / reject
      ** redirection and determine the next state to be processed based
      ** on the returned SS-Action.
      ** -------------------------------------------------------------- */
      if ( !mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
           sd_ss_ind_cdma_opr_acpt_rej(NULL)) )
      {
        new_state = CDMA_EXIT;
        mcc_exit.exit = MCC_REDIRECTION;
      }
    } /* if (mccidl_synch_workaround.egsr_is_synch_workaround) */
  }

  return new_state;

} /* mccidl_proc_ignored_ovhd */

/*===========================================================================

FUNCTION MCCIDL_STATE

DESCRIPTION
  This procedure does Idle State state machine processing.  If we are in
  INIT state and our overhead messages are now updated for the first time
  we go to slotted mode (if allowed).  If we are in slotted mode we go to
  sleep (if allowed).  If we are transitioning to System Access State we
  make sure our overhead messages are updated then go to the appropriate
  substate System Access State.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_state
(
  word new_state
    /* Next state to be processed */
)
{
  /* Event report: Call processing state change */
  mclog_report_event_cp_state(new_state);

  switch (new_state)
  {
    case CDMA_IDLE_INIT: /* Initial wait for all overhead messages */
    {

      #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
      #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

      /* ----------------------------------
      ** All overhead messages are current. 
      ** ---------------------------------- */
      if ( mcc_is_ovhd_msg_okay() )
      {
        /* Clear BS insanity timer */
        mccidl_clear_bs_insanity_timer();

        /* Clear the num ovhd cycles */
        num_ovhd_cycles = 0;

        /* Clear ovhd SD report pending flag. */
        oh_sd_rpt_pending_in_upd_ovhd = FALSE;

        /* ------------------------------------------------------------------
        ** Inform system determination that all overhead messages are current
        ** Determines the next state to be processed based on the returned
        ** SS-Action.
        ** ---------------------------------------------------------------- */
        if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
            sd_ss_ind_cdma_opr_oh_info2(
              (sd_band_e_type) cdma.band_class, cdma.cdmach, 
               cur_bs_ptr->csp.sp.sid, cur_bs_ptr->csp.sp.nid,
               mccidl_check_gsr_ignored(), cur_bs_ptr->csp.ota_mcc,
               cur_bs_ptr->csp.ota_imsi_11_12,
               #ifdef FEATURE_SD_SUPPORTS_BASE_INFO
               cur_bs_ptr->csp.sp.srch_win_n,
               EXTEND22(cur_bs_ptr->csp.sp.base_lat), /*lint !e737*/
               EXTEND23(cur_bs_ptr->csp.sp.base_long), /*lint !e737*/
               cur_bs_ptr->csp.sp.base_id,
               #endif /* FEATURE_SD_SUPPORTS_BASE_INFO */
               NULL)
            )) 
        {
          mcc_exit.exit = MCC_NOT_PREFERRED;
          new_state = CDMA_EXIT;
          break;
        }

        /* -------------------------------------------------------------------
        ** If we have ignored a rescan, process it now.
        ** If we have ignored a (extended) global service redirection message,
        ** process it now.
        ** ------------------------------------------------------------------- */
        if ( ( new_state = mccidl_proc_ignored_ovhd(new_state) ) == CDMA_EXIT )
        { 
          break;
        }

        #ifdef FEATURE_USERZONE
        /* Acquired service, update, if any, user zone
        ** info reqd for registration
        */
        mccreg_uz_update();
        #endif /* FEATURE_USERZONE */

        /* -------------------------------------------------------------------
        ** Overhead messages are all updated. Stores new SID, NID and PZID and
        ** notify its reception if a handler function is registered.
        ** ------------------------------------------------------------------- */
        MSG_MED("Idle Init, process epzid", 0, 0, 0);

        /* Mark EPZID processing is not pending */
        mccidl_epzid_proc_pending = FALSE;

        mccsyobj_process_epzid(cur_bs_ptr->csp.sp.sid, cur_bs_ptr->csp.sp.nid, 
          (P_REV_IS_4_OR_GREATER ? cur_bs_ptr->csp.esp.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN), CDMA_IDLE);

        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */

        /* -----------------------------------------------------------
        ** Following is the serving system info update for CM. It is
        ** done once we acquire a service.
        ** Notice that Digital indicator is on, CM can safely poll it
        ** to know whether the data is for CDMA or for AMPS. Also
        ** NO_SVC is set correctly now.
        ** ----------------------------------------------------------- */
        mcc_send_cm_srv_sys_info( CM_SS_SID_MASK|
                                  CM_SS_NID_MASK|
                                  CM_SS_REG_ZONE_MASK|
                                  CM_SS_PACKET_ZONE_MASK|
                                  CM_SS_BS_P_REV_MASK|
                                  CM_SS_P_REV_IN_USE_MASK, /*lint !e747 */
                                  cur_bs_ptr->csp.sp.sid,
                                  cur_bs_ptr->csp.sp.nid,
                                  cur_bs_ptr->csp.sp.reg_zone,
                                  (P_REV_IS_4_OR_GREATER ? cur_bs_ptr->csp.esp.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN)
                                );

        if ( cdma.sa_tc_state != CDMA_IDLE )
        {
          #ifdef FEATURE_AUTH
          if ( !mcc_are_rand_parms_okay()

               #ifdef FEATURE_IS2000_REL_D
#error code not present
               #endif /* FEATURE_IS2000_REL_D */

             )
          {
            /* wait to go to system access */
            new_state = CDMA_OVHD; /*lint !e641 */
          }
          else
          #endif /* FEATURE_AUTH */

          {
            /* Received all overhead messages - turn off the state timer */
            (void) rex_clr_timer(&mcc_state_timer);
            (void) rex_clr_sigs(&mc_tcb, MCC_STATE_TIMER_SIG);

            #ifdef FEATURE_IS2000_REL_D
#error code not present
            #endif /* FEATURE_IS2000_REL_D */

            /* Send request to Transmit task to Initialize encoder timing */
            mcc_send_txc_early_jump_cmd();

            /* Going to System Access state or Traffic Channel Initialization 
               state */
            new_state = cdma.sa_tc_state;
          }
        }

        else if ( idle_slotted )
        {
          /* wait to go to sleep */ 
          new_state = CDMA_SLOTTED; /*lint !e641 */
        }

        else
        {
          /* wait to go to slotted mode. */
          new_state = CDMA_UNSLOTTED; /*lint !e641 */
        }
      }
      else if((num_ovhd_cycles > 1) && SP_MSG_IS_RECVD && ESP_MSG_IS_RECVD &&
              ((GSR_MSG_IS_RECVD && gsr_ignored) || 
               (EGSR_MSG_IS_RECVD && egsr_ignored)))
      {

        /* Reset ovhd cycles counter */
        num_ovhd_cycles = 0;

        /* ------------------------------------------------------------------
        ** Inform system determination that all overhead messages are current
        ** Determines the next state to be processed based on the returned
        ** SS-Action.
        ** ---------------------------------------------------------------- */
        if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
            sd_ss_ind_cdma_opr_oh_info3((sys_band_class_e_type)cdma.band_class, 
                                        cdma.cdmach,
                                        cur_bs_ptr->csp.sp.sid,
                                        cur_bs_ptr->csp.sp.nid,
                                        mccidl_check_gsr_ignored(),
                                        cur_bs_ptr->csp.ota_mcc,
                                        cur_bs_ptr->csp.ota_imsi_11_12,
#ifdef FEATURE_SD_SUPPORTS_BASE_INFO
                                        cur_bs_ptr->csp.sp.srch_win_n,
                                        EXTEND22(cur_bs_ptr->csp.sp.base_lat), /*lint !e737*/
                                        EXTEND23(cur_bs_ptr->csp.sp.base_long), /*lint !e737*/
                                        cur_bs_ptr->csp.sp.base_id,
                                        SD_SS_OVHD_STATUS_NONE,
#endif /* FEATURE_SD_SUPPORTS_BASE_INFO */
                                        NULL)))
        {
          mcc_exit.exit = MCC_NOT_PREFERRED;
          new_state = CDMA_EXIT;
          break;
        }

        /* -------------------------------------------------------------------
        ** If we have ignored a rescan, process it now.
        ** If we have ignored a (extended) global service redirection message,
        ** process it now.
        ** ------------------------------------------------------------------- */
        new_state = mccidl_proc_ignored_ovhd(new_state);
          

      }

      break;
    }

    case CDMA_UNSLOTTED:
    {
      /* -------------------------------------------------------------
      ** Nothing in particular to do for unslotted mode at some point. 
      ** Might be waiting for registration to go to slotted mode.
      ** ---------------------------------------------------------------- */

      if (mcc_is_ovhd_cfg_okay() && mccidl_epzid_proc_pending)
      {
        /* -------------------------------------------------------------------
        ** Overhead messages are all updated. Stores new SID, NID and PZID and
        ** notify its reception if a handler function is registered.
        ** ------------------------------------------------------------------- */
        MSG_MED("Unslotted, process epzid", 0, 0, 0);

        mccsyobj_process_epzid(cur_bs_ptr->csp.sp.sid, cur_bs_ptr->csp.sp.nid, 
          (P_REV_IS_4_OR_GREATER ? cur_bs_ptr->csp.esp.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN), CDMA_IDLE);

        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */

        /* Mark EPZID processing is not pending */
        mccidl_epzid_proc_pending = FALSE;

        /* -----------------------------------------------------------
        ** Following is the serving system info update for CM. It is
        ** done once we acquire a service.
        ** Notice that Digital indicator is on, CM can safely poll it
        ** to know whether the data is for CDMA or for AMPS. Also
        ** NO_SVC is set correctly now.
        ** ----------------------------------------------------------- */
        mcc_send_cm_srv_sys_info( CM_SS_SID_MASK|
                                  CM_SS_NID_MASK|
                                  CM_SS_REG_ZONE_MASK|
                                  CM_SS_PACKET_ZONE_MASK|
                                  CM_SS_BS_P_REV_MASK|
                                  CM_SS_P_REV_IN_USE_MASK, /*lint !e747 */
                                  cur_bs_ptr->csp.sp.sid,
                                  cur_bs_ptr->csp.sp.nid,
                                  cur_bs_ptr->csp.sp.reg_zone,
                                  (P_REV_IS_4_OR_GREATER ? cur_bs_ptr->csp.esp.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN)
                                );

     #ifdef FEATURE_FULL_TIME_SHDR_3
        /* Mobile is operating on non-slotted mode, sleep is diabled. 
	** Make sure HDR declares system lost.  And disable the MC 
	** DDTM so that accessing the network on RX0 is possible.
        ** If CM DDTM is active, it will continue to take effect. 
	*/
        cm_unlock_hdr_resources();
        mcc_set_mc_ddtm_status(FALSE);
     #endif /* FEATURE_FULL_TIME_SHDR_3 */

      }

      /* ----------------------------------------------------
      ** The mobile could be in UN_SLOTTED if the Slotted
      ** timer has not expired.  Check if the Idle_Slotted
      ** is set and begin slotted mode again.
      ** ---------------------------------------------------*/
      if (idle_slotted)
      {
        MSG_MED("Begin Slotted mode operation", 0, 0, 0);
        /* Begin slotted mode */ 
        new_state = CDMA_SLOTTED;    /*lint !e641 */
      }

      break;
    }

    case CDMA_SLOTTED:   /* operating in slotted mode */
    {
      #ifdef FEATURE_BROADCAST_SMS
      qword current_time;
      qword tmp_qw1, tmp_qw2;
  
      // get current system time in units of 20msecs
      ts_get_20ms_frame_time(current_time);

      if (mcc_bcsms_in_progress)
      {
        qw_equ(tmp_qw1, current_time); // 20ms frame
        qw_shift(tmp_qw1, -2); // 80ms slot

        qw_equ(tmp_qw2, mcc_bcsms_start_slot);
        qw_inc(tmp_qw2, 2); // add 2 slots

        if (qw_cmp(tmp_qw1, tmp_qw2) > 0)
        {
          MSG_MED("All 3 slots of the started BCSMS have passed", 0, 0, 0);
          mcc_bcsms_in_progress = FALSE;
          qw_set(mcc_bcsms_start_slot, 0, 0);
        }
      }

      if(mccbcsms_is_bc_data_slot(current_time) || mcc_bcsms_in_progress)
      {
        MSG_MED("Sleep not allowed", 0, 0, 0);
        mccidl_sleep_ok = FALSE;
      }
      #endif //FEATURE_BROADCAST_SMS

      if ( mcc_is_ovhd_cfg_okay() && mccidl_sleep_ok
           && ( cdma.sa_tc_state == CDMA_IDLE ) 
           && ( cdma.ho_state == CDMA_IDLE_HO )
           && ( q_cnt( &mc_cmd_q ) == 0 )
           /* when it is waiting for transceiver resource, do not go to sleep */
           && !cdma.transceiver_request_pending

           #ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
           #endif /* FEATURE_GPSONE_NEW_L2_ACK */

           #if (defined FEATURE_DELAYED_TX_CLOCK_SHUTDOWN || defined FEATURE_RL_JUMP_ENHANCEMENT)
           && txc_ok_to_sleep() 
           #endif // FEATURE_DELAYED_TX_CLOCK_SHUTDOWN || FEATURE_RL_JUMP_ENHANCEMENT

           #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
           #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */
         )
      {
        if (mccidl_epzid_proc_pending)
        {
          /* -------------------------------------------------------------------
          ** Overhead messages are all updated. Stores new SID, NID and PZID and
          ** notify its reception if a handler function is registered.
          ** ------------------------------------------------------------------- */
          MSG_MED("Slotted, process epzid", 0, 0, 0);

          mccsyobj_process_epzid(cur_bs_ptr->csp.sp.sid, cur_bs_ptr->csp.sp.nid, 
            (P_REV_IS_4_OR_GREATER ? cur_bs_ptr->csp.esp.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN), CDMA_IDLE);

          #ifdef FEATURE_IS2000_REL_D
#error code not present
          #endif /* FEATURE_IS2000_REL_D */

          /* Mark EPZID processing is not pending */
          mccidl_epzid_proc_pending = FALSE;

          /* -----------------------------------------------------------
          ** Following is the serving system info update for CM. It is
          ** done once we acquire a service.
          ** Notice that Digital indicator is on, CM can safely poll it
          ** to know whether the data is for CDMA or for AMPS. Also
          ** NO_SVC is set correctly now.
          ** ----------------------------------------------------------- */
          mcc_send_cm_srv_sys_info( CM_SS_SID_MASK|
                                    CM_SS_NID_MASK|
                                    CM_SS_REG_ZONE_MASK|
                                    CM_SS_PACKET_ZONE_MASK|
                                    CM_SS_BS_P_REV_MASK|
                                    CM_SS_P_REV_IN_USE_MASK, /*lint !e747 */
                                    cur_bs_ptr->csp.sp.sid,
                                    cur_bs_ptr->csp.sp.nid,
                                    cur_bs_ptr->csp.sp.reg_zone,
                                    (P_REV_IS_4_OR_GREATER ? cur_bs_ptr->csp.esp.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN)
                                  );
        }

        /* All conditions satisifed for sleep */
        MSG_MED("Start sleep", 0, 0, 0);
        new_state = mccidl_start_sleep(new_state);
      }

      break;
    }

    case CDMA_OVHD:   /* System Access Update Overhead info state */
    {

      #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
      #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

      if ( ( mcc_is_ovhd_msg_okay() 
             #ifdef FEATURE_AUTH 
             &&  (
                   mcc_are_rand_parms_okay()

                   #ifdef FEATURE_IS2000_REL_D 
#error code not present
                   #endif /* FEATURE_IS2000_REL_D */
                  )
             #endif /* FEATURE_AUTH */
           )
         )
      {
  
        {
          if ( oh_sd_rpt_pending_in_upd_ovhd )
          {
            oh_sd_rpt_pending_in_upd_ovhd = FALSE;

            /* ------------------------------------------------------------------
            ** Inform system determination that all overhead messages are current
            ** Determines the next state to be processed based on the returned
            ** SS-Action.
            ** ---------------------------------------------------------------- */
            
            if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
                  sd_ss_ind_cdma_opr_oh_info2(
                    (sd_band_e_type) cdma.band_class, cdma.cdmach,
                    cur_bs_ptr->csp.sp.sid, cur_bs_ptr->csp.sp.nid,
                    mccidl_check_gsr_ignored(), cur_bs_ptr->csp.ota_mcc,
                    cur_bs_ptr->csp.ota_imsi_11_12,
                    #ifdef FEATURE_SD_SUPPORTS_BASE_INFO
                    cur_bs_ptr->csp.sp.srch_win_n,
                    EXTEND22(cur_bs_ptr->csp.sp.base_lat), /*lint !e737*/
                    EXTEND23(cur_bs_ptr->csp.sp.base_long), /*lint !e737*/
                    cur_bs_ptr->csp.sp.base_id,
                    #endif /* FEATURE_SD_SUPPORTS_BASE_INFO */
                    NULL)
               ))
            {
              mcc_exit.exit = MCC_NOT_PREFERRED;
              new_state = CDMA_EXIT;
              break;
            }
          }

          /* -------------------------------------------------------------------
          ** If we have ignored a rescan, process it now.
          ** If we have ignored a (extended) global service redirection message,
          ** process it now.
          ** ------------------------------------------------------------------- */
          if ( ( new_state = mccidl_proc_ignored_ovhd(new_state) ) == CDMA_EXIT )
          { 
            break;
          }

          if (mccidl_epzid_proc_pending)
          {
            /* -------------------------------------------------------------------
            ** Overhead messages are all updated. Stores new SID, NID and PZID and
            ** notify its reception if a handler function is registered.
            ** ------------------------------------------------------------------- */
            MSG_MED("Upd Ovhd, process epzid", 0, 0, 0);

            mccsyobj_process_epzid(cur_bs_ptr->csp.sp.sid, cur_bs_ptr->csp.sp.nid, 
            (P_REV_IS_4_OR_GREATER ? cur_bs_ptr->csp.esp.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN), CDMA_IDLE);

            #ifdef FEATURE_IS2000_REL_D
#error code not present
            #endif /* FEATURE_IS2000_REL_D */

            /* Mark EPZID processing is not pending */
            mccidl_epzid_proc_pending = FALSE;

            /* -----------------------------------------------------------
            ** Following is the serving system info update for CM. It is
            ** done once we acquire a service.
            ** Notice that Digital indicator is on, CM can safely poll it
            ** to know whether the data is for CDMA or for AMPS. Also
            ** NO_SVC is set correctly now.
            ** ----------------------------------------------------------- */
            mcc_send_cm_srv_sys_info( CM_SS_SID_MASK|
                                      CM_SS_NID_MASK|
                                      CM_SS_REG_ZONE_MASK|
                                      CM_SS_PACKET_ZONE_MASK|
                                      CM_SS_BS_P_REV_MASK|
                                      CM_SS_P_REV_IN_USE_MASK, /*lint !e747 */
                                      cur_bs_ptr->csp.sp.sid,
                                      cur_bs_ptr->csp.sp.nid,
                                      cur_bs_ptr->csp.sp.reg_zone,
                                      (P_REV_IS_4_OR_GREATER ? cur_bs_ptr->csp.esp.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN)
                                    );
          }
        }

        /* Received all overhead messages - turn off the state timer and
           switch to system access */
        (void) rex_clr_timer( &mcc_state_timer );
        (void) rex_clr_sigs( &mc_tcb, MCC_STATE_TIMER_SIG );

        /* Reset the flag to FALSE */
        mccidl_reg_required = FALSE;

        #ifdef FEATURE_IS2000_REL_D
#error code not present
        #endif /* FEATURE_IS2000_REL_D */

        /* Send request to Transmit task to Initialize encoder timing */
        mcc_send_txc_early_jump_cmd();
        
        /* Going to System Access state */
        new_state = cdma.sa_tc_state;
      }

      break;
    }

    default:
    {
      ERR_FATAL("Bad State %d",new_state,0,0);  
    }

  } /* end switch */

  return (new_state);

} /* end mccidl_state() */

#if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */

/*===========================================================================

FUNCTION MCCIDL_ONLINE

DESCRIPTION
  This procedure waits for the receipt of a message, report, user command,
  or timer expiration, and does the appropriate processing for the event.
  If entered with a message pending, the pending message is processed first.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_online
(
  word new_state
    /* State to be returned to calling procedure */
)
{
  mccrxtx_cmd_type *rx_ptr; /* Pointer to received message */
  mc_msg_type *cmd_ptr;     /* Command from the MC_CMD_Q queue */
  mccrx_rpt_type *rpt_ptr;  /* Report from the MCC_RX_RPT_Q queue */
  mccsrch_rpt_type *srch_ptr;      /* Report from the MC_SRCH_Q queue */
  dword sigs;                      /* Signals returned by mcc_wait */
  dword wait_mask;                 /* Mask of signals to wait on */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  wait_mask = sigs = 0;

  /* Initialize EPZID processing flag */
  mccidl_epzid_proc_pending = FALSE;

  /* Just enter Idle State, disable MER statistical operation for page alot. */
  mclog_mer_config(FALSE, 0);

  #ifdef FEATURE_IS2000_REL_B
  if ( cdma.entry_ptr->gen.enter_idle_from_tc )
  {
    /* Clear the flag that indicates TC to Idle transition. */
    cdma.entry_ptr->gen.enter_idle_from_tc = FALSE;

    if (mcc_is_ovhd_cfg_okay())
    {
      MSG_HIGH("Stored Ovhds good", 0, 0, 0);
      new_state = mccidl_process_stored_ovhds(new_state);

      /* May be it is time to switch to FCCCH */
      mccidl_supervise_channel_monitoring(new_state, NULL, FALSE);

      if (new_state == CDMA_EXIT)
      {
        MSG_HIGH("Exitting IDLE right away",0,0,0);
        return new_state;
      }
    }
    else
    {
      MSG_HIGH("Stored Ovhds Bad", 0, 0, 0);
      mcc_clear_ovhd_msgs();
    }

    #ifdef FEATURE_IS2000_REL_D
#error code not present
    #endif /* FEATURE_IS2000_REL_D */

  }
  #endif /* FEATURE_IS2000_REL_B */

  /* ---------------------------------------------------
  ** We could be re-entering Idle State with a Channel
  ** Assignment, Registration Rejected Order, or Service
  ** Redirection Message still waiting to be processed.
  ** --------------------------------------------------- */
  if ( new_state == CDMA_IDLE_MSG_PENDING ) /*lint !e641 */
  {
    /* Reset Idle State to CDMA_INIT */
    new_state = CDMA_IDLE_INIT;   /*lint !e641 */
    cdma.sa_tc_state = CDMA_IDLE;
    new_state = mccidl_proc_pending_msg( &mcc_layer2.pending_idle_msg, new_state );
  }
  else
  {
    /* Reset idle msg pending flag */
    mcc_layer2.idle_msg_pending = FALSE;
  }

  do
  {
    #ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
    #endif /* FEATURE_GPSONE_NEW_L2_ACK */

    sigs = mcc_wait(wait_mask);

    if ( ( ( sigs & MCC_CALLBACK_TIMER_SIG ) != 0 ) && 
         mcc_timer_on_callback_q( MCC_BS_INSANITY_TIMER_ID ) )
    {
      MSG_MED("BS insanity timer expired", 0, 0, 0);

      /* Clear BS insanity timer */
      mccidl_clear_bs_insanity_timer();

      if ( !mcc_is_ovhd_cfg_okay() )
      {
        /* -------------------------------------------------------------
        ** Inform system determination of system lost and determines the
        ** next state to be processed based on the returned SS-Action.
        ** ------------------------------------------------------------- */
        if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_UNKNOWN, NULL ) ) 
           )
        {
          mcc_exit.exit = MCC_SYSTEM_LOST;
          new_state = CDMA_EXIT;
        }

      } /* if (new_state == CDMA_IDLE_INIT) */
    }

    else if ( ( sigs & MCC_SRCH_Q_SIG ) != 0 )
    {
      if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
      {
        /* Process searcher report */
        new_state = mccidl_proc_srch_rpt( srch_ptr, new_state );
      }
    } /* end if ( ( sigs & MCC_SRCH_Q_SIG ) != 0 ) */

    else if ( ( sigs & MCC_RXTX_Q_SIG ) != 0 )
    {
      if ((rx_ptr = (mccrxtx_cmd_type*) q_get(&mcc_rxtx_q)) != NULL)
      {

        #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
        #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

        if (rx_ptr->hdr.command == MCCRXTX_MSG_F)
        {
          /* Process received message */
          new_state = mccidl_msg_handler(&rx_ptr->msg, new_state);

          /* Return message to return_q if requested */
          cmd_done(&rx_ptr->hdr.cmd_hdr);
        }

        #if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
        #endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */
    
        else
        {
          /* Properly dispose the buffer */
          ERR( "Invalid RXTX command %d", rx_ptr->hdr.command, 0, 0);
          cmd_done(&rx_ptr->hdr.cmd_hdr);
        }
      }
    } /* end else if ((sigs & MCC_RXTX_Q_SIG) != 0) */

    else if ((sigs & MCC_CMD_Q_SIG) != 0)
    {
      if ((cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q)) != NULL)
      {
        new_state = mccidl_proc_hs_cmd(cmd_ptr, new_state);
      }
    } /* end else if ((sigs & MCC_CMD_Q_SIG) != 0) */

    else if ((sigs & MCC_RX_RPT_Q_SIG) != 0)
    {
      if ((rpt_ptr = (mccrx_rpt_type*) q_get(&mcc_rx_rpt_q)) != NULL)
      {
        #ifdef FEATURE_MC_QUEUE_WATERMARK
        mc_update_watermark(MCWM_RX_FREE_Q);
        #endif

        new_state = mccidl_proc_rx_rpt(rpt_ptr, new_state);
      }
    } /* end else if ((sigs & MCC_RX_RPT_Q_SIG) != 0) */

    else if ((sigs & MCC_STATE_TIMER_SIG) != 0)
    {
      #ifndef FEATURE_ERRLOG_DEBUG
      MSG_HIGH("Lost Paging Channel: no messages", 0, 0, 0);
      #else /* FEATURE_ERRLOG_DEBUG */
      ERR("Lost Paging Channel: no messages", 0, 0, 0);
      #endif /* FEATURE_ERRLOG_DEBUG */
      (void) rex_clr_sigs(&mc_tcb, MCC_STATE_TIMER_SIG);

      /* -------------------------------------------------------------
      ** Inform system determination of system lost and determines the
      ** next state to be processed based on the returned SS-Action.
      ** ------------------------------------------------------------- */
      if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_opr_sys_lost2( SD_SS_SYS_LOST_UNKNOWN, NULL ) ) 
      )
      {
        new_state     = CDMA_EXIT;
        mcc_exit.exit = MCC_SYSTEM_LOST;
      }

      /* Event report: Timer T41m expired */
      mclog_report_event_timer(CAI_TIMER_T41M);
    } /* end else if ((sigs & MCC_STATE_TIMER_SIG) != 0) */

    else if ((sigs & MCC_SLOTTED_TIMER_SIG) != 0)
    {
      /* -----------------------------------------------------------------
      ** Upon expiration of the slotted T MS Slotted timer, mobile station
      ** shall disable the timer and set SLOTTEDs to YES.
      ** ----------------------------------------------------------------*/
      MSG_MED( "Slotted Timer expired ",0,0,0);

      idle_slotted = TRUE;
      mccidl_sleep_ok = TRUE;
      /* clear slotted timer signal */
      (void) rex_clr_timer(&mcc_slotted_timer);
      (void) rex_clr_sigs(&mc_tcb, MCC_SLOTTED_TIMER_SIG);
    }

    /* TMSI expiration timer */
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
               mcc_timer_on_callback_q(MCC_TMSI_EXP_TIMER_ID)
            )
    {
      /* If valid TMSI code has expired,     */
      /* then reset code and all TMSI timers */
      if (tmsi_expired ())
      {
        /* Reset TMSI code and all timers */
        tmsi_code_and_timer_reset ();
      }
      /* If valid TMSI code has not expired,  */
      /* check if TMSI expiration timer needs */
      /* to be set again to check expiration  */
      /* time.                                */
      else
      {
        /* Set TMSI expiration timer if required */
        tmsi_set_exp_timer ();
      }
    }

    /* Full TMSI timer */
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_FULL_TMSI_TIMER_ID)
            )
    {
      /* If full TMSI timer has expired,     */
      /* then reset code and all TMSI timers */
      MSG_LOW("Full TMSI timer expired", 0L, 0L, 0L);
      tmsi_code_and_timer_reset ();

      /* Delete SID_NID_LIST and ZONE_LIST */
      /* See J-STD-008, 2.6.5.5.2.5        */
      mccreg_clear_reg();
    }

    #ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
    #endif /*  FEATURE_GPSONE_NEW_L2_ACK */

    else if ( ( ( sigs & MCC_CALLBACK_TIMER_SIG ) != 0 ) &&
              mcc_timer_on_callback_q(MCC_TR_REQUEST_TIMER_ID)
            )
    {
      /* clear the timer */
      (void) mcc_clr_callback_timer(&mcc_tr_request_timer, MCC_TR_REQUEST_TIMER_ID);

      /* Send SRCH_IDLE_F command to bring it back to Idle state */
      mccidl_send_srch_idle_cmd();

      /* Check if there is a pending request */
      if (!cdma.transceiver_request_pending)
      {
        MSG_ERROR("No pending request, ignore TRREQ timer expiration", 0, 0, 0);
        break;
      }

      cdma.transceiver_request_pending = FALSE;

      if ( cdma.sa_tc_state != CDMA_IDLE )
      {
        /* This is the case for scenario like CDMA_PAGE_RES / CDMA_TCI
           overrides CDMA_ORD_RES while MS is pending on AEHO */
        MSG_MED("In UPD OVHD, ignore TRREQ timer expiration", 0, 0, 0);
          
        /* Sanity check, should never happen */
        if ( cdma.sa_tc_state != cdma.ho_sa_tc_state )
        {
          MSG_ERROR("HO SA state 0x%x SA state 0x%x mismatch", 
            cdma.sa_tc_state, cdma.ho_sa_tc_state, 0);
   
          /* To recover, update SA state to match HO SA state */
          cdma.sa_tc_state = cdma.ho_sa_tc_state;
        }
        
        break;
      }

      MSG_ERROR("Transceiver request timer expired, abort pending SA state 0x%x", 
        cdma.ho_sa_tc_state, 0, 0);

      if ( cdma.ho_sa_tc_state == CDMA_ORIG ) /*lint !e641 */
      {
        /* Tell CM origination failure */
        MSG_ERROR("No transceiver resource to originate call_id %d", 
          cdma.call_id, 0, 0);
        mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                              cdma.call_id,
                              CM_CALL_ORIG_ERR_LAYER_OTHER
                             );
      }

      #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
      #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

      /* ------------------------------------
      ** Pending access aborted, do clean up.
      ** ------------------------------------ */ 
      cdma.ho_state = CDMA_IDLE_HO;
      cdma.call_orig = FALSE; 
  
      mcc_layer2.idle_msg_pending = FALSE;
      mccidl_reg_required = FALSE;

      #ifdef FEATURE_IS2000_REL_A
      cdma.is_sr_id_valid = FALSE;
      #endif /* FEATURE_IS2000_REL_A */

      #ifdef FEATURE_IS2000_REL_D
#error code not present
      #endif /* FEATURE_IS2000_REL_D */
       
      #ifdef FEATURE_OTASP
      /* Reset OTASP call status */
      cdma.otasp_call = NO_SRV_PROV;
      #endif
    }
  
    #ifdef FEATURE_IS2000_REL_C_AKA 
#error code not present
    #endif /* FEATURE_IS2000_REL_C_AKA */

    /* -------------------------------------------------------------
    ** Check registration status each time and send registration
    ** message if required.  This checking is done as long the mobile
    ** is in IDLE state.
    ** ------------------------------------------------------------- */
    if ( ( MAIN_STATE(new_state) == CDMA_IDLE )  
         && ( cdma.sa_tc_state == CDMA_IDLE )
         && ( cdma.ho_state != CDMA_AEHO )
         && !cdma.transceiver_request_pending
       )
    {
      if (!mccidl_reg_required)
      {
        cdma.reg_type = mccreg_idle_check();
      }

      /* The cdma.reg_type was already set if mccidl_reg_required is true */
      if (cdma.reg_type != MCCREG_NO_REG_REQUIRED)
      {
        if ( ( cdma.ho_state != CDMA_AEHO )
             && !cdma.transceiver_request_pending
           ) /* Make sure it is not in AEHO */
        {
          /* Registration is required - go to Update Overhead Info state */
          MSG_MED("Registration required %d", cdma.reg_type, 0, 0);

          #ifdef FEATURE_MCC_POWERUP_REG_COUNT
          if (cdma.reg_type == CAI_POWERUP_REG)
          {
            /* Increment the count of powerup registration performed */
            mccreg_increment_powerup_reg_count();
          }
          #endif /* FEATURE_MCC_POWERUP_REG_COUNT */

	  #ifdef FEATURE_IS707B_ADDENDUM 
#error code not present
	   #endif /* FEATURE_IS707B_ADDENDUM */
	   {   
            /* --------------------------------------------------------------
             ** Send Search the ACCESS command: We just need to send out the 
             ** registration, so use the low_power access where the RX       
             ** Diversity is not enabled.                       
             ** ------------------------------------------------------------*/
            mcc_request_transceiver_resource(SRCH_LOW_POWER_ACCESS, 
              SRCH_ACCESS_MOBILE_ORIG_TYPE, CDMA_REG); /*lint !e641 */
           }
        }
      }
      else
      {
        /* Reset the flags to default values */
        mccidl_reg_required = FALSE;
      }
    } /* end if ((MAIN_STATE(new_state) == CDMA_IDLE) && ... */

    /* Wait mask depends on state */
    if (cdma.sa_tc_state == CDMA_IDLE)
    {
      wait_mask = (MCC_CMD_Q_SIG | MCC_RXTX_Q_SIG | MCC_SRCH_Q_SIG |
                   MCC_RX_RPT_Q_SIG);

      /* Set the Slotted timer Signal */
      wait_mask |= MCC_SLOTTED_TIMER_SIG;
    }
    else
    {
      /* --------------------------------------------------------
      ** In Update Overhead Information Substate we don't do Idle
      ** Handoffs, so don't wait on signal from searcher task.
      ** -------------------------------------------------------- */
      wait_mask = (MCC_STATE_TIMER_SIG | MCC_CMD_Q_SIG | MCC_RXTX_Q_SIG |
                   MCC_RX_RPT_Q_SIG);
    }

    wait_mask |= MCC_CALLBACK_TIMER_SIG;


    /* -----------------------------------------------------------------
    ** If we were attempting a powerdown registration and some event has
    ** caused the state to change from CDMA_OVHD (e.g. to CDMA_EXIT
    ** if we lost the system) or if some event has caused the intended
    ** system access substate to change from CDMA_REG (e.g. to CDMA_PAGE
    ** if we got a page which overrides the registration) then we need
    ** to exit with an MCC_POWERDOWN exit right away.
    ** ----------------------------------------------------------------- */
    if ( attempting_powerdown_reg &&
         ( ( cdma.transceiver_request_pending &&
             ( cdma.ho_sa_tc_state != CDMA_REG ) /*lint !e641 */
           )
           || 
           ( !cdma.transceiver_request_pending &&
             ( ( new_state != CDMA_OVHD ) || ( cdma.sa_tc_state != CDMA_REG ) ) /*lint !e641 */
           )
         )
       )
    {
      new_state     = CDMA_EXIT;

      if (cdma.powerdown_reg_type == CDMA_NORMAL)
      {
        mcc_exit.exit = MCC_POWERDOWN;
      }
      else if (cdma.powerdown_reg_type == CDMA_SHUTDOWN)
      {
        mcc_exit.exit = MCC_SHUTDOWN;
      }
      else if (cdma.powerdown_reg_type == CDMA_LPM)
      {
        mcc_exit.exit = MCC_LPM;
      }
    }

    if (MAIN_STATE(new_state) == CDMA_IDLE)
    {
      new_state = mccidl_state(new_state);
    }

  } while (MAIN_STATE(new_state) == CDMA_IDLE);

  /* Exit Idle state, disable MER statistical operation for page slot. */
  mclog_mer_config(FALSE, 0);

  /* Clear HO state */
  cdma.ho_state = CDMA_IDLE_HO;

  /* Clear transceiver request timer */
  (void) mcc_clr_callback_timer(&mcc_tr_request_timer, MCC_TR_REQUEST_TIMER_ID);
  
  if (MAIN_STATE(new_state) == CDMA_SYS_ACC)
  {
    if (!access_fade_timer_in_use)
    {
      /* Tell Receive Task to start using the System Access fade timeout */
      mcc_rx_buf.acc_fade.hdr.command = RXC_ACC_FADE_F;
      mcc_rx_buf.acc_fade.hdr.cmd_hdr.done_q_ptr = NULL;
      mcc_rxc_cmd( &mcc_rx_buf );
    }
  }

  #if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
  /* -------------------------------------------------------------------
  ** If we have received an MO DBM request from CM/SMS, we should leave
  ** IDLE and enter CDMA_MSG substate of SA. However, DBM request can
  ** be pre-empted by origination, page response, etc. I think all the
  ** cases has been covered. The following code is there for debugging.
  **  Process for CDMA_EXIT is not considered
  ** here, will be considered as soon as this function returns.
  ** -------------------------------------------------------------------*/
  if ( new_state != CDMA_EXIT &&     /*lint !e641 */
       (! ( (new_state == CDMA_MSG) && /*lint !e641 */
            (cdma.sa_msg_tx_msg == CAI_AC_BURST_MSG )  /*lint !e641 */
          )
       )
     ) /*lint !e641 */
  {
    /* Any other state transition, notify CM that MO DBM has failed */

    //MSG_ERROR( "SMS: DEAD CODE RUNNING", 0, 0, 0 );
    mcc_check_mo_dbm_status( CM_DBM_MO_L2_ACK_FAILURE );
  }
  #endif /* FEATURE_CDSMS || FEATURE_DBM */

  /* Reset the number of ovhd cycles */
  num_ovhd_cycles = 0;
  return (new_state);

} /* mccidl_online */

/*===========================================================================

FUNCTION MCCIDL_OFFLINE

DESCRIPTION
  This procedure performs the approprate offline function.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_offline
(
  word new_state
    /* State to be returned to calling procedure */
)
{
  return (new_state);

} /* mccidl_offline */

/*===========================================================================

FUNCTION CDMA_IDLE

DESCRIPTION
  This procedure performs initialization, determines which mode of idle
  operation is required, and calls the appropriate procedure to handle
  the designated mode.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_idle( void )
{
  word new_state = cdma.curr_state;
    /* State to be returned to calling procedure */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("Enter CDMA IDLE", 0, 0, 0);

  /* Event report: Call processing state change */
  mclog_report_event_cp_state(cdma.curr_state);

  /* Clear TMSI timer if delete_tmsi is set to 1.  The flag is set by a
     redirection message.  It was then passed to SD and saved with the
     redirection record. */
  if (sd_misc_is_del_tmsi()) 
  {
    MSG_LOW("Delete_tmsi flag is set by EGSRM", 0, 0, 0);
    tmsi_code_and_timer_reset();
  }

  #ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
  #endif /* FEATURE_IS2000_REL_C_AKA */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  {
    /* ------------------------------------------------------------
    ** Perform initialization.  If we entered with Update Overhead
    ** Information Substate as the current state we really aren't
    ** entering Idle State so we don't do Idle State Initialization.
    ** ------------------------------------------------------------- */
    if (new_state == CDMA_OVHD) /*lint !e641 */
    {
      mccidl_ovhd_init();
    }
    else
    {
      mccidl_idle_init();
    }

    if (cdma.entry_ptr->entry == MCC_ENTER_OFFLINE)
    {
      new_state = mccidl_offline( new_state );
    }
    else
    {
      #ifdef FEATURE_UIM_RUIM
      /* Indicate to UIM that is not likely to be busy */
      (void) uim_power_control( UIM_MC, FALSE);
      #endif /* FEATURE_UIM_RUIM */

      new_state = mccidl_online(new_state);
    }

    /* -----------------------------------------------------
    ** Verify that, if we are going to respond to a message,
    ** the message was received on the current channel.
    ** ----------------------------------------------------- */
    new_state = mccidl_check_chan(new_state);

    if (new_state == CDMA_EXIT)
    {
      /* ----------------------
      ** Inform CM orig failure
      ** ---------------------- */
      if (cdma.call_orig)
      {
        mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                              cdma.call_id,
                              CM_CALL_ORIG_ERR_LAYER_OTHER
                              );
        cdma.call_orig = FALSE;
      }

      #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
      #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

      #ifdef FEATURE_IS2000_REL_A
      if (mcc_exit.exit == MCC_ENCRYPTION_MI_FAILED)
      {
        event_report(EVENT_ENCRYPTION_FAILURE);
      }
      #endif /* FEATURE_IS2000_REL_A */

      #if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
      /* ---------------------------------------------------
      ** If SMS is pending, we want CM to know it is failed.
      ** --------------------------------------------------- */
      mcc_check_mo_dbm_status( CM_DBM_MO_L2_ACK_FAILURE );
      #endif /* FEATURE_CDSMS || FEATURE_DBM */

    }
  }

  return (new_state);

} /* cdma_idle */

#ifdef FEATURE_BROADCAST_SMS
/*===========================================================================

FUNCTION MCCIDL_SAVE_BCAST_PAGE

DESCRIPTION
  This procedure stores broadcast page and set broadcast pending flag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_save_bcast_page
(
  mccrxtx_msg_type *msg_ptr
    /* Pointer to received message */
)
{
  /* Mark pending broadcast page flag */
  bcast_pending = TRUE;

  /* Save broadcast page for later processing */
  bcast_page = *msg_ptr;

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  if (msg_ptr->msg.gen.msg_type == CAI_UPM_FINAL_SEG_MSG)
  {
    /* Save page as unsegmented UPM since message reassembly is done  */
    bcast_page.msg.upm.msg_type = CAI_UNIVERSAL_PAGE_MSG;
    bcast_page.msg.upm.common = mccdma_seg_univ_page.common;
    bcast_page.msg.upm.interleaved_addr =
      mccdma_seg_univ_page.interleaved_addr;
    bcast_page.msg.upm.page_block.length =
      mccdma_seg_univ_page.page_block.length;
    memcpy(bcast_page.msg.upm.page_block.data, mccdma_seg_univ_page.page_block.data,
      mccdma_seg_univ_page.page_block.length);
    /* Rewind offset back to the first page record */
    bcast_page.msg.upm.page_block.offset =
      mccdma_seg_univ_page.page_block.first_page_rec_offset;
    /* Disable IMSI page match */
    qw_set(bcast_page.msg.upm.interleaved_addr.imsi_match, 0, 0);
    /* Disable TMSI page match */
    qw_set(bcast_page.msg.upm.interleaved_addr.tmsi_match, 0, 0);
  }

  else if (msg_ptr->msg.gen.msg_type == CAI_UNIVERSAL_PAGE_MSG)
  {
    /* Rewind offset back to the first page record */
    bcast_page.msg.upm.page_block.offset =
      msg_ptr->msg.upm.page_block.first_page_rec_offset;
    /* Disable IMSI page match */
    qw_set(bcast_page.msg.upm.interleaved_addr.imsi_match, 0, 0);
    /* Disable TMSI page match */
    qw_set(bcast_page.msg.upm.interleaved_addr.tmsi_match, 0, 0);
  }
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

} /* mccidl_save_bcast_page */

#endif /* FEATURE_BROADCAST_SMS */

/*===========================================================================

FUNCTION MCCIDL_UPDATE_SLEEP_SLOT_MASK

DESCRIPTION
  This procedure updates sleep_slot_mask with OR operation with input mask.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_update_sleep_slot_mask
(
  byte mask
)
{
  sleep_slot_mask |= mask;
} /* mccidl_update_sleep_slot_mask */

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION MCCIDL_GET_SCI_MASK

DESCRIPTION
  This procedure gets position-in-slot from pgslot.

DEPENDENCIES
  None

RETURN VALUE
  Position-in-slot.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_get_sci_mask(void)
{
  return (sci_mask);
} /* mccidl_get_sci_mask */
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION MCCIDL_SET_SCI_MASK

DESCRIPTION
  This procedure sets position-in-slot from pgslot.

DEPENDENCIES
  None

RETURN VALUE
  Position-in-slot.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_set_sci_mask(word new_sci_mask)
{
  sci_mask = new_sci_mask;
} /* mccidl_set_sci_mask */

#if defined(FEATURE_PLT) || defined(FEATURE_FACTORY_TESTMODE)
/*===========================================================================

FUNCTION MCCIDL_FTM_INIT

DESCRIPTION
  This procedure initializes idle state non-slotted mode operation.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_ftm_init(void)
{
  /* Set variables for non-slotted mode */
  idle_slotted = FALSE;
  mccidl_sleep_ok = FALSE;

  /* Clear state timer */
  (void) rex_clr_timer( &mcc_state_timer );
  (void) rex_clr_sigs( &mc_tcb, MCC_STATE_TIMER_SIG );

  /* -------------------------------------------
  ** Mark that Receive task has NOT been told to
  ** use access timer, that we are entering Idle
  ** State with no pending System Access state.
  ** ------------------------------------------- */
  access_fade_timer_in_use = FALSE;
  cdma.sa_tc_state = CDMA_IDLE;

  /* These flags are only used when re-entering OVHD state from SA */
  oh_sd_rpt_pending_in_upd_ovhd = FALSE;
  page_res_retransmission = FALSE;

  /* -------------------------------------------------------------------
  ** Idle State sets this flag if it leaves a message pending to ACK it.
  ** System Access State will return to CDMA_IDLE_MSG_PENDING state if
  ** it sees this flag set.
  ** ------------------------------------------------------------------- */
  mcc_layer2.idle_msg_pending = FALSE;

  /* We are not attempting a powerdown registration at this time */
  attempting_powerdown_reg = FALSE;

  #ifdef FEATURE_IS2000_REL_A_CC
  /* Clear monitor next slot flag */
  mccidl_monitor_next_slot = FALSE;
  #endif /* FEATURE_IS2000_REL_A_CC */

  /* Show that we haven't ignore a Global Service Redirection Message */
  gsr_ignored = FALSE;

} /* mccidl_ftm_init */
#endif /* FEATURE_PLT || FEATURE_FACTORY_TESTMODE */

/*===========================================================================

FUNCTION MCCIDL_GET_SLOT_CYCLE_INDEX_S

DESCRIPTION
  This procedure gets the slot cycle index.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

int8 mccidl_get_slot_cycle_index_s(void)
{
  return slot_cycle_index_s;
} /* mccidl_get_slot_cycle_index_s */

/*===========================================================================

FUNCTION MCCIDL_SET_SLOT_CYCLE_INDEX_S

DESCRIPTION
  This procedure sets the slot cycle index.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_set_slot_cycle_index_s(int8 sci_s)
{
  MSG_HIGH("sci_s being set to %d",sci_s,0,0);
  slot_cycle_index_s = sci_s;

} /* mccidl_set_slot_cycle_index_s */

/*===========================================================================

FUNCTION MCC_GET_IDLE_PGSLOT

DESCRIPTION
  This procedure gets the idle page slot.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

word mcc_get_idle_pgslot(void)
{
  return idle_pgslot;
} /* mccidl_get_idle_pgslot */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*===========================================================================

FUNCTION MCCIDL_IS_FAST_RAHO

DESCRIPTION
  This procedure returns the value of mccidl_fast_raho.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccidl_is_fast_raho(void)
{
  return mccidl_fast_raho;

} /* mccidl_is_fast_raho */

/*===========================================================================

FUNCTION MCCIDL_IS_ACCESS_FADE_TIMER_IN_USE

DESCRIPTION
  This procedure returns the value of access_fade_timer_in_use.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccidl_is_access_fade_timer_in_use(void)
{
  return access_fade_timer_in_use;

} /* mccidl_is_access_fade_timer_in_use */

/*===========================================================================

FUNCTION MCC_SRCH_LOOKUP_NGHBR_INFO

DESCRIPTION
  This function looks up the neighbor information for the specified neighbor.
  The information includes: 1) Neighbor Config, 2) Status of Neighbor Overhead,
  3) Neighbor Type and 4) The Overhead timestamp of the neighbor.           

  The definition of the neighbor type is as shown below:

                       Type 13
                         ^
                         | System determination
                         |
                       Type 1
                         ^
                         | No cost
                         |
             RF tune          Ovhd collection           RF tune
  Type 3,4 <--------- Active -----------------> Type 2 ----------> Type 5,6
                          
                         |                            Same frequency neighbors
  ____________________________________________________________________________                          

                         | RF tune                             OFreq neighbors
                         |
              RF tune    v     Ovhd collection           RF tune
  Type 9,10 <--------- Type 7 -----------------> Type 8 ----------> Type 11,12

                         |
                         | System determination
                         v
                       Type 14


DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void mcc_srch_lookup_nghbr_info
(
  word pilot_pn,
  byte band,
  word freq,
  mcc_srch_nghbr_info *nghbr_info
)
{
  int2 nghbr_index = -1;
  int2 nghbr_bs_index = -1;
  bs_info_id_type bs_info_id;
 
  boolean same_pilot_freq = FALSE;
  mcc_srch_nbr_paging_freq_type paging_freq = NBR_BS_UNKNOWN_PAGING_FREQ;
  boolean known_paging_chan = FALSE;

  /* Determine the index to neighbor record entries */
  nghbr_index = mccidl_get_nghbr_index(pilot_pn, freq, band);

  /* **********************************
  ** Probe info for the neighbor config 
  ** ********************************** */
  if (nghbr_index == -1)
  {
    /* It is an invalid neighbor. */
    nghbr_info->nghbr_config = 3;
    nghbr_info->nghbr_ovhd = NBR_BS_OVHD_NOT_OK;
    nghbr_info->nghbr_type = NBR_TYPE_UNKNOWN;
    qw_set (nghbr_info->nghbr_ovhd_time_stamp, 0xFFFF, 0xFFFF);

    /* Invalid neighbor. No more info can be probed */
    MSG_6(MSG_SSID_DFLT,MSG_LEGACY_HIGH,
      "Nbr PN %d [%d,%d] cfg %d, ovhd %d, type %d (Invalid Nbr)",
      pilot_pn,
      band,
      freq,
      nghbr_info->nghbr_config,
      nghbr_info->nghbr_ovhd,
      nghbr_info->nghbr_type);

    return;
  } 
  else if ( ( cur_bs_ptr->nl.nset[nghbr_index].config > 2 ) &&
            ( OVHD_CHAN_IS_PCH
              #ifdef FEATURE_IS2000_REL_A_CC
              || ( OVHD_CHAN_IS_BCCH &&
                   ( cur_bs_ptr->nl.nset[nghbr_index].config != 4 )
                 )
              #endif /* FEATURE_IS2000_REL_A_CC */
            ) 
          )
  {
    /* It is an unknown neighbor. */
    nghbr_info->nghbr_config = 3;
    nghbr_info->nghbr_ovhd = NBR_BS_OVHD_NOT_OK;
    if (cur_bs_ptr->nl.nset[nghbr_index].config != 3)
    {
      nghbr_info->nghbr_type = NBR_TYPE_UNKNOWN;
    }
    else if (freq == cur_bs_ptr->cdma_ch) 
    {
      nghbr_info->nghbr_type = NBR_TYPE_13;
    }
    else
    {
      nghbr_info->nghbr_type = NBR_TYPE_14;
    }
    qw_set (nghbr_info->nghbr_ovhd_time_stamp, 0xFFFF, 0xFFFF);

    /* Unknown neighbor. No more info can be probed */
    MSG_6(MSG_SSID_DFLT, MSG_LEGACY_LOW,
      "Nbr PN %d [%d,%d] cfg %d, ovhd %d, type %d (Unknown Nbr)",
      pilot_pn,
      band,
      freq,
      nghbr_info->nghbr_config,
      nghbr_info->nghbr_ovhd,
      nghbr_info->nghbr_type);

    return;
  }
  else
  {
    nghbr_info->nghbr_config = cur_bs_ptr->nl.nset[nghbr_index].config;
  }

  /* From here onwards, neigbor config has been verified to be valid and
     has the value of 0, 1, 2 or 4. */

  /* *******************************************
  ** Probe info for the neighbor overhead status
  ** ******************************************* */
  /* Determine the paging channel or broadcast control channel the MS 
     is to listen to on the neighbor base station, so as to figure out the
     corresponding bs_info[] entry. */
  bs_info_id.ovhd_chan = cur_bs_ptr->ovhd_chan_info.ovhd_chan;
  bs_info_id.pilot_pn = pilot_pn;
  bs_info_id.cdma_ch = freq;
  bs_info_id.band_class = band;

  #ifdef FEATURE_IS2000_REL_A_CC
  if (OVHD_CHAN_IS_BCCH)
  {
    if ((nghbr_info->nghbr_config == 0) || 
        (nghbr_info->nghbr_config == 4))
    {
      bs_info_id.ovhd_walsh_ch = cdma.bcch;
    }
    else if (nghbr_info->nghbr_config == 1)
    {
      bs_info_id.ovhd_chan = CAIX_PC;
      bs_info_id.ovhd_walsh_ch = mccidl_get_nbrcfg_pch_walsh(pilot_pn, freq, band);
    }
    else
    {
      /* Neighbor has nbr_cfg 2. Get frequency and walsh channel that
      * the mobile is going to listen to on the neighbor BS.
      */
      bs_info_id.ovhd_walsh_ch = cdma.bcch;
    }
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC */
  {
    if (nghbr_info->nghbr_config == 0)
    {
      bs_info_id.ovhd_walsh_ch = cdma.pagech;
    }
    else if (nghbr_info->nghbr_config == 1)
    {
      bs_info_id.ovhd_walsh_ch = mccidl_get_nbrcfg_pch_walsh(pilot_pn, freq, band);
    }
    else
    {
      /* Neighbor has nbr_cfg 2. Get frequency and walsh channel that
       * the mobile is going to listen to on the neighbor BS.
       */
      bs_info_id.ovhd_walsh_ch = mccidl_get_nbrcfg_pch_walsh(pilot_pn, freq, band);
    }
  }

  /* MS had visited neighbor base station before */
  if ((nghbr_bs_index = mccidl_get_bs_index(&bs_info_id)) != -1) 
  {
    /* The neighbor base station's configuration parameters are current */
    if (mccidl_check_bs_ovhd_current((byte)nghbr_bs_index))
    {

      #ifdef FEATURE_IS2000_REL_A
      /* ---------------------------------------------------
      ** Check if the paging channel neighbor supports BCCH.
      ** --------------------------------------------------- */
      if ((bs_info[nghbr_bs_index].ovhd_chan_info.ovhd_chan == CAIX_PC) &&
          (bs_info[nghbr_bs_index].csp.sp.ext_sys_parameter) &&
          (bs_info[nghbr_bs_index].csp.esp.p_rev >= 7) &&
          (bs_info[nghbr_bs_index].csp.esp.bcch_supported))
      {
        /* It needs to exit to System Determination after handoff. */
        nghbr_info->nghbr_ovhd = NBR_BS_OVHD_NOT_OK;
      }
      else
      #endif /* FEATURE_IS2000_REL_A */

      /* -----------------------------------------------------------------
      ** Check if a parameter-based or zone-based registration is needed
      ** on the neighbor base station. Note that registrations are checked
      ** in the order that they are checked in "mccreg_idle_check()".
      ** ----------------------------------------------------------------- */
      if ((bs_info[nghbr_bs_index].csp.sp.parameter_reg) &&
          mccreg_parameter_reg_required(bs_info[nghbr_bs_index].csp.sp.sid,
                                        bs_info[nghbr_bs_index].csp.sp.nid))
      {
        /* Parameter-based registration is needed on the neighbor BS */
        MSG_LOW("Parameter-based Reg require on Nbr PN=%d", pilot_pn, 0, 0);
        nghbr_info->nghbr_ovhd = NBR_BS_IMMEDIATE_REG;
      }
      else if ((bs_info[nghbr_bs_index].csp.sp.total_zones != 0) &&
               mccreg_zone_reg_required(bs_info[nghbr_bs_index].csp.sp.reg_zone,
                                        bs_info[nghbr_bs_index].csp.sp.sid,
                                        bs_info[nghbr_bs_index].csp.sp.nid))
      {
        /* Zone-based registration is needed on the neighbor BS */
        MSG_LOW("Zone-based Reg require on Nbr PN=%d", pilot_pn, 0, 0);
        nghbr_info->nghbr_ovhd = NBR_BS_IMMEDIATE_REG;
      }
      else
      {
        /* No zone-based or parameter-based registration is needed on the
         * neighbor base station.
         */
        nghbr_info->nghbr_ovhd = NBR_BS_OVHD_OK;
      }
    }
    else
    {
      /* The neighbor base station's configuration is not current */
      nghbr_info->nghbr_ovhd = NBR_BS_OVHD_NOT_OK;
    }
  }
  else
  {
    /* The neighbor is not in bs_info[] array. No more info can be probed */
    nghbr_info->nghbr_ovhd = NBR_BS_OVHD_NOT_OK;
    qw_set (nghbr_info->nghbr_ovhd_time_stamp, 0xFFFF, 0xFFFF);
  }

  /* ********************************
  ** Probe info for the neighbor type
  ** ******************************** */
  if (freq == cur_bs_ptr->cdma_ch)
  {
    same_pilot_freq = TRUE;
  }

  /* Overhead is current, we know paging freq and paging channel */
  if (nghbr_info->nghbr_ovhd != NBR_BS_OVHD_NOT_OK)
  {
    known_paging_chan = TRUE;
    
    /* nghbr_bs_index is guaranteed to be valid if ovhd is okay */
    if (nghbr_bs_index < 0) 
    {
      ERR_FATAL("negative nghbr_bs_index", 0, 0, 0);
    }

    if ((bs_info[nghbr_bs_index].hashed_cdmach == cur_bs_ptr->cdma_ch) &&
        (bs_info[nghbr_bs_index].hashed_band == cur_bs_ptr->band_class))
    {
      paging_freq = NBR_BS_SAME_PAGING_FREQ;
    }
    else
    {
      paging_freq = NBR_BS_OTHER_PAGING_FREQ;
    }
  }
  /* Overhead is not current, special cases for neighbor config 0 and 1 */
  else 
  {
    /* Neighbor config 0 on the same pilot freq always guarantee same paging
       freq and known paging channel */
    if (cur_bs_ptr->nl.nset[nghbr_index].config == 0)
    {
      paging_freq = NBR_BS_SAME_PAGING_FREQ;
      known_paging_chan = TRUE;
    }
    /* Neighbor config 1 on the same pilot freq always guarantee same paging
       freq */
    else if (cur_bs_ptr->nl.nset[nghbr_index].config == 1)
    {
      paging_freq = NBR_BS_SAME_PAGING_FREQ;
    }
  }

  /* Ovhd config is already known. Based on the combination of the pilot_freq, 
   * paging_freq and paging_chan, decide the neighbor type */

  if (same_pilot_freq && (paging_freq == NBR_BS_SAME_PAGING_FREQ) && known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_1;
  }
  else if (same_pilot_freq && (paging_freq == NBR_BS_SAME_PAGING_FREQ) && !known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_2;
  }
  else if (same_pilot_freq && (paging_freq == NBR_BS_OTHER_PAGING_FREQ) && known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_3;
  }
  else if (same_pilot_freq && (paging_freq == NBR_BS_OTHER_PAGING_FREQ) && !known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_4;
  }
  else if (same_pilot_freq && (paging_freq == NBR_BS_UNKNOWN_PAGING_FREQ) && known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_5;
  }
  else if (same_pilot_freq && (paging_freq == NBR_BS_UNKNOWN_PAGING_FREQ) && !known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_6;
  }
  else if (!same_pilot_freq && (paging_freq == NBR_BS_SAME_PAGING_FREQ) && known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_7;
  }
  else if (!same_pilot_freq && (paging_freq == NBR_BS_SAME_PAGING_FREQ) && !known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_8;
  }
  else if (!same_pilot_freq && (paging_freq == NBR_BS_OTHER_PAGING_FREQ) && known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_9;
  }
  else if (!same_pilot_freq && (paging_freq == NBR_BS_OTHER_PAGING_FREQ) && !known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_10;
  }
  else if (!same_pilot_freq && (paging_freq == NBR_BS_UNKNOWN_PAGING_FREQ) && known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_11;
  }
  else if (!same_pilot_freq && (paging_freq == NBR_BS_UNKNOWN_PAGING_FREQ) && !known_paging_chan)
  {
    nghbr_info->nghbr_type = NBR_TYPE_12;
  }
  else
  {
    /* Should never happen */
    nghbr_info->nghbr_type = NBR_TYPE_UNKNOWN;
  }

  /* *********************************************************
  ** Probe info for neighbor base station's overhead timestamp
  ** ********************************************************* */
  if (nghbr_info->nghbr_ovhd == NBR_BS_OVHD_NOT_OK)
  {
    /* If the OVHD is not OK then invalidate the ovhd timestamp */ 
    qw_set(nghbr_info->nghbr_ovhd_time_stamp, 0xFFFF, 0xFFFF);
  }
  else
  {
    /* nghbr_bs_index is guaranteed to be valid if ovhd is okay */

    if (nghbr_bs_index < 0) 
    {
      ERR_FATAL("negative nghbr_bs_index", 0, 0, 0);
    }

    qw_equ(nghbr_info->nghbr_ovhd_time_stamp, bs_info[nghbr_bs_index].rx.msg_time);
    
  }


  MSG_8(MSG_SSID_1X_DCP, MSG_LEGACY_MED,
    "Nbr PN %d [%d,%d] cfg %d, ovhd %d, type %d, ts [%d,%d]",
    pilot_pn,
    band,
    freq,
    nghbr_info->nghbr_config,
    nghbr_info->nghbr_ovhd,
    nghbr_info->nghbr_type,
    qw_hi(nghbr_info->nghbr_ovhd_time_stamp),
    qw_lo(nghbr_info->nghbr_ovhd_time_stamp));

} /* mcc_srch_lookup_nghbr_info */
