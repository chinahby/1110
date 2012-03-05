/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      M A I N  C O N T R O L   T A S K   C D M A   S U B - S Y S T E M

GENERAL DESCRIPTION
  This file contains the lowest level state machine and the entrance
  and exit states for the CDMA Sub-system of the Main Control task.  It also
  contains all the global variables required by the CDMA sub-system.

EXTERNALIZED FUNCTIONS
  mcc_subtask
    The lowest level state machine for the CDMA Sub-system.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The Main Control task calls the mcc_subtask procedure after it has
  performed mobile system initialization and determined that the CDMA
  cellular system should be acquired.

Copyright (c) 1990-2010 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccdma.c_v   1.40   07 Oct 2002 16:54:34   phosabet  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccdma.c#13 $ $DateTime: 2010/01/21 22:41:29 $ $Author: anishg $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/22/10   ag      Corrected the assignment of MEID_ME to MEID.
01/21/10   ag      Fix to set ESN to ESN_ME and MEID to MEID_ME when NVRUIM 
                   provides zero ESN/MEID.
01/13/10   ag      Fix to prevent reading of UIM ID when card has not initialized.
01/07/10   jj      Rearranged DB writes .
12/31/09   ag      Fix to read ESN irrespective of whether subscription
                   is available or not.
12/27/09   jj      Added changes to refresh few NV items, during powerup.
12/10/09   jj      Added gaurd check to read Mobile Directory Number for maximum
                   number of MDN digits.
09/30/09   jj      Fix to read MAX ENABLED OTASP features from RUIM card.
08/25/09   jj      Updated CSIM-P1 change.
08/14/09   jj      Added support for CSIM feature.
07/29/09   ag      Fix to receive UIM reports irrespective of command status
07/20/09   ag      Removed call to sd_ss_ind_misc_orig_success for MT call.
04/07/09   pk      Moved initialization of ESN_ME and MEID_ME before assigning
                   actual values.
04/07/09   pk      Modified ESN_ME & MEID_ME retrieval to remove dependency 
                   on TMC
03/30/09   pk      Added support to update MEID_ME in CDMA structure for 
                   supporting FEATURE_HWID_SUPPORT
09/30/08   ss      FTM Second Chain fix for FULL_TIME_SHDR_3.
06/19/08   bb      Changes for FEATURE_8K_STUFF_TO_BE_PURGED feature
06/11/08   ag      Fixed an error of wrong array indices being used.
05/15/08   bb      Changes for "MVS API Migration" feature
04/16/08   pg      Fixed a Lint warning in the previous change.
04/10/08   pg      Fixed the issue that mobile does not respond to Registration
                   Request Order in non-slotted mode when FEATURE_FULL_TIME_SHDR_3
                   is enabled.
03/13/08   pg      Read/write db_lp_sec, db_ltm_off, and db_daylt from/to NV
                   at the Sync Message processing time.
01/16/08   pg/yll  Added support for FEATURE_FULL_TIME_SHDR_3 so that it
                   allows 1x to do acq on RX0 regardless of the state of DO.
01/22/08   ag      Fixed compiler warning
01/08/08   ag      Mainlined FEATURE_LPM
12/14/07   an      Added Passport Mode
11/21/07   bb      Support interactions between MEID and UIMID
11/06/07   pg      Set esn for deriving default values of some NV items that 
                   are accessed from the RUIM.
10/29/07   pg      Discard SRCH_DEACTIVATE_DONE_R in mcc_powerdown().
07/27/07   pg      Added mccdma_get_cur_bs_info() to pass current base station
                   info to upper layer.
07/10/07   an      Fixed compilation error
06/14/07   an      Added support for White Grey Black Card Operation
05/11/07   pg      More Lint Cleanup.
03/09/07   pg      removed dependency on WMS and make sure it only depends on 
                   CM for SMS.
02/23/07   sb      Added support for new exit reason.
02/13/07   pg      Make DB_LP_SEC, DB_LTM_OFF, and DB_DAYLT persistent over 
                   power cycle.
02/15/07   sb      Fixed payload for new exit reason.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
09/30/06   fc      Changed featurization.
09/29/06   fh      AKA/AES: removed the AES testing code, and eliminated the
                            AES testing featurization of 
                            FEATURE_IS2000_REL_A_AES_BS_SUPPORTS
09/21/06   fh      AKA/AES: added a new access state 
                     MCC_ACCESS_STOPPED_MI_AES_RESYNC -- access stopped for 
                     AKA/AES crypto-sync procedure
08/04/06   fh      AKA/AES: cleaned up the usage of AES capability functions
08/01/06   fc      Changed debug messages.
07/25/06   fh      AKA/AES: put part of lab testing that is dependent on BS AES
                            support under a temporary feature of  
                            FEATURE_IS2000_REL_A_AES_BS_SUPPORTS
07/13/06   fh      AKA/AES: Cleaned up AES capability helper functions.
07/06/06   fc      Renamed MCC_RF_RESOURCE_LOST to MCC_SRCH_REQUEST_SYS_RESTART.
06/22/06   jqi     Eliminate a copy of PRL in CP/OTASP module to reduce the 
                   memory by the size of MAX PRL size and use SD PRL for 
                   uploading.
06/07/06   fh      Added Voice Service AES encryption/decryption under the features
06/02/06   fc      Cleanup umts interface. 
06/01/06   an      Added the bypass registration indicator changes
05/30/06   fc      Added support of MC command processing in slotted mode.
05/30/06   fc      Fixed compilation error.
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/19/06   fc      Added exit reason MCC_RF_RESOURCE_LOST.
05/19/06   fc      Added mcc_tx_int_msg and mcc_rx_int_msg.
05/19/06   an      Printing messages when sending command to other tasks.
05/17/06   fh      Merged in new development of AKA/AES features
04/26/06   an      Fixed Lint Errors
04/17/06   an      Added logic to enable/disable 4GV_WB SO using a NV item.
02/16/06   dna     Avoid HDR doing extra PCCC negotiation.
02/07/06   fc      Mainlined FEATURE_MC_SUPPORTS_SHDR.
01/20/06   an      Fixed compilation warning
01/20/06   fc      Merged the following :
           yll     Added key setup timer.
01/17/06   an      Added support to enable/disable 4GV_NB SO using NV item.
01/06/06   sb      Replaced cm_mc_rpt_free_q with cm_mc_rpt_get_buf_else_err_fatal().
01/06/06   fc      Moved function call cm_mc_unlock_resources to mcc_wait.
11/14/05   ht      Added MEID verification support for RUIM. 
11/02/05   fc      Corrected MEID check for Rel D.
10/16/05   fc      Changes on mcc_wait to call Searcher rude wakeup if MS is
                   sleeping and there commands other than 
                   PROT_GEN_CMD_SS_TIMER_EXP on MC queue.
10/15/05   fc      Added the validation on the availability of MEID for 
                   MOB_P_REV >= 11.
09/07/05   pg      Fixed a compilation error.
07/15/05   fc      Fixed lint errors.
07/14/05   pg      Changed code to Eliminate calls to mc.c local function 
                   mc_put_nv_item().
07/11/05   fh      Cleaned up header file including from MUX:
                     changed txc.h to txcmc.h
                     changed rxc.h to rxcmc.h
07/08/05   fh      Made SHDR phase II changes:
                     initialize cdma.transceiver_request_pending = FALSE
                     add a callback timer variable: 
                       rex_timer_type mcc_tr_request_timer;
                     set callback function for this callback timer
                     and clear the timer when it is initialized
07/12/05   fc      Fixed lint error.
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
05/31/05   fc      Merged the following from MSMSHARED:
05/31/05   va       sam.h hdr file include no longer needed.
05/17/05   pg      Changed "TXC_JUMP_F not supported" from MSG_ERROR to MSG_HIGH.
04/27/05   az      Submitted changes for CR 62361. This also requires 
                   corresponding changes in NV.
04/19/05   fc      Merged the following from MSMSHARED:
04/13/05   an       Mainlined FEATURE_SRCH_INACTIVE_STATE
04/04/05   fc      Merged the following from MSMSHARED:
03/28/05   va       Make slotted timer run run even if we do a system exit
                    so that when we go back to idle we still operate in
                    non-slotted mode till timer is active
03/29/05   pg       Initialize NV cmd in mcc_powerup_load() before checking
                    if 1x subscription is available.
03/25/05   az      Changed the label from MCC_USE_DEFAULT_SCI to MCC_NULL_SCI
03/25/05   az      Initialized last_reg_sci to MCC_INIT_LAST_REG_SCI in
                   mcc_powerup_load()
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated 
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/18/05   fc       Backed out changes of initialize the ovhd info only during
                    powerup.
02/08/05   an       Mainlined IS95B features
02/22/05   sb      Return from cdma_rf_init immediately for GPS PLT baseband.
02/18/05   fc      Merged the following from MSMSHARED:
02/03/05   fc       Fixed lint error.
01/07/05   an       Removed feature FEATURE_SMS and FEATURE_UASMS
02/09/05   va      Merged folowing from MSMSHARED tip
02/03/05   fc       Fixed lint error.
01/07/05   an       Removed feature FEATURE_SMS and FEATURE_UASMS
01/19/05   va      Temporary fix to ignore TXC return cmd status only for 
                   TXC_JUMP_F. Shd be removed once that is supported
                   in MUX
12/10/04   sb      Fixed lint errors.
12/06/04   fc      Fixed lint errors.
12/07/04   an      Initialize the ovhd info only during powerup.
12/06/04   yll     Added interface change for apiOne.
11/12/04   va      Added support for Network Personalization
11/09/04   pg      Really took out FEATURE_NV_RUIM around #include "mcnvdf.h".
11/04/04   fc      Fixed lint errors.
11/01/04   pg      Took out FEATURE_NV_RUIM around #include "mcnvdf.c" to
                   avoid compile warning.
10/29/04   pg      If 1X subscription is not available, use default values for 
                   subscription items.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
09/27/04   va      Merged from RELB/C archive
09/23/04   fc       Fixed featurization issue.
08/12/04   dna      Transitions to SRCH_START_STATE now go to SRCH_CDMA_STATE
08/11/04   dna      Remove cdma_continue() for FEATURE_SRCH_INACTIVE_STATE
08/11/04   dna      Remove cdma_continue() for FEATURE_SRCH_INACTIVE_STATE
07/21/04   sj       Added support for umts to 1x handover.
07/04/04   dna      For FEATURE_SRCH_INACTIVE_STATE, sent TXC_EXIT_F in 
                    cdma_continue().
06/30/04   dna      Changes for FEATURE_SRCH_INACTIVE_STATE.
                    This feature changes the MC<->SRCH interface for activating
                    and deactivating SRCH.  It moves the functionality in
                    cdma_rf_init(), to wait for the RF lock and tune the RF,
                   into SRCH. It moves TXC_CDMA_F into mccini, just before we 
                   enter idle, since cdma_enter() now runs without the RF lock
                   and this command turns off the PA.
06/18/04   ank     Mainlined FEATURE_GPSONE_DBM.
08/17/04   az      Rel D Negative SCI changes
08/06/04   bkm     Converted T50 and T51 timers to callback timers.
08/17/04   az      Rel D Negative SCI changes
08/06/04   bkm     Converted T50 and T51 timers to callback timers.
05/11/04   fc      Rollback the changes on cdma_exit.
05/05/04   fc      Changed function cdma_exit to send out TXC_EXIT_F as the
                   first command.
03/09/04   ph      Change slot to AUTOMATIC in command to UIM.
03/05/04   bkm     Lint cleanup.
02/24/04   va      Fixed compile errors
02/03/04   ck/ph   Added support to wait for UIM report and release the buffer
                   even when the report status was error since the cmd to UIM
                   is sent with ALWAYS_REPORT option.
02/09/04   va      Added  mccdma_clear_mc_queues_sigs_timers fn
01/15/04   yll     Release the SAM lock after failed 1x acquisition.
01/08/04   yll     Changed the function mccap_df_powerup_load to 
                   mccap_df_load, separated the function of initialize 
                   the DF item from NV and deciding if going offline.
01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
12/16/03   aaj     1x to request lock for 1sec for acq for HICPS2.0
11/21/03   yll     Removed the Jaguar 2nd chain changes from cdma_rf_init(), 
                   because they are done in Search now.
11/07/03   yll     Fixed the problem when releasing a call, RF is in a wrong
                   state. In cdma_continue, cdma_rf_init should be called 
                   after shutting off TXC.
10/31/03   gs      Modify cdma_continue to reflect multi-mode last good
                   pilot strategy (unslew to CDMA rather than START).
10/29/03   sst     T_M6500 in last checkin changed to proper T_MSM6500
10/29/03   sst     Surrounded SRCH_HAS_PILOT_GAIN_CNTL with T_M6500
10/29/04   yll/bmg Fixed a Jaguar second RF chain problem.
10/24/03   sst     Changed RF tuning calls with srch_tune_rfm(), featurized 
                   as SRCH_HAS_PILOT_GAIN_CNTL
09/11/03   yll     Added FEATURE_ACP to analog code.
09/08/03   ra      Added rfm_enter_mode and rfm_enable_rx to cdma_rf_init
09/04/03   fc      Added support for buffering UIM_ID and ESN_ME.
09/02/03   az      Added rude wakeup calls when there is a command on the
                   command queue while waiting on SRCH signal.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/11/03   az      Added support for FEATURE_MODIFIED_SCI
08/01/03   yll     Added support for Dynamic Feature.
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/30/03   az      Added support for FEATURE_SO_8026
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
06/10/03   bkm     Lint cleanup. 
06/03/03   sb      Added call to mccreg_clear_reg for MCC_ACCESS_FAILED_GSRM.
04/07/03   bkm     Removed code for obsolete variable cdma_active_set.
03/05/03   nb/yll  Set the UIM command report option to UIM_OPTION_ALWAYS_RPT
                   in mcc_uim_cmd().
11/26/02   sj      Renamed the class member in nv_accolc_type to ACCOLCpClass.
11/07/02   az      Removed the feature TMOBILE
10/07/02   ph      mccreg_init() done here only if multimode arch is OFF.
10/03/02   sb      Linted the code.
10/01/02   az      Mainlined FEATURE_IS2000 and T_MSM2P
09/30/02   sb      Mainlined FEATURE_SD20.
09/26/02   fc      Added exit reason MCC_ACCESS_FAILED_GSRM.
08/30/02   ts      Added support for Run Time R-UIM Enable.
08/15/02   az      Reverted the feature name surrounding mcc_pd_init back to
                   FEATURE_GPSONE_DBM
08/15/02   az      Removed extra #endif's taht were causing compiler errors
08/13/02   HQ      Added FEATURE_CDSMS.
07/10/02   fc/lc   Corrected MS unnecessary staying awake for changes in APM
                   and EAPM.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/08/02   yll     Added support for MC queue watermark
07/06/02   az      Added event reporting for CDMA exit reasons.
06/07/02   sj      Added CP support for multimode architecture.
06/06/02   jqi     Added support for FEATURE_MULTIMODE
                   1.Removed mcc_powerup_load from mcc_powerup_init.
05/20/02   fc      Added support for Universal Page Message.
05/06/02   yll     Changed Broadcast SMS function names.
04/17/02   sb      Removed redundant event.
04/16/02   va      Added support putting CM in E911 callback mode after
                   MC received ack for emergency flash.
04/09/02   yll     Added Broadcast SMS support on IS2000 RelA common channel.
03/09/02   lh      Merged from Common Archive.
  02/20/02   az      Featurized 13K voice service option
02/22/02   fc      Added support for Release A common channels.
02/13/02   va      Merged from MSM archive
  02/06/02 abh     Added code to go into GPS state when Phone is in Acquisition state.
  02/06/02 sh      Added hybird operation support
  12/17/01 zg      Featurized RF_ENABLE_RX_PWR_WAIT to support RF driver's
                   changes on the wait macros.
  12/14/01 fc      Removed function call mccreg_blksys_bc_init.
01/11/02   va      Featurized to make code work when SVD is not defined.
12/21/01   ph      Migrated mcc_l2_ack_timer to the callback timer mechanism.
12/13/01   va      Merged in changes from SVD branch.
  08/23/01 ph      Added initialization of tag timers (Tag manager) at powerup.
11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
11/01/01   RC      Now loading PRL into mcc_roaming_list every time NAM is
                   changed (SD 2.0 only).
08/21/01   fc      Added support for exit reasons MCC_ENCRYPTION_FAILED and
                   MCC_ACCT_BLOCKED.
08/16/01   kk      Merge from MSM_CP.05.00.89.
                   - Read SPC status from RUIM instead of from NV.
08/08/01   yll     Merged in the following from MSM archive:
                   -Rid unused mcc_ds_buf.
                   -(cah) In cdma_enter_init(), under case MCC_CONTINUE,
                    replaced the "new_mode_or_sys" with "block_or_sys"
                    to make it a type compatible comparison within the if
                    statement.
07/06/01   ph      Added support for Callback Timers
06/26/01   ph      Merged in from MSM_CP.05.00.81
                   06/13/01   fc      Added support for powerup registration of band class, freq
                                      block or Serv-sys switch.
                   05/25/01   ts      Added an interface to the new UIM server.  Changed old auth
                                      commands to the new UIM server API.
05/17/01   ph      Merged from MSM_CP.05.00.76
                   04/12/01   fc      Added events report: TMSI assigned and Power down.
                                      Renamed mc_report_event_op_mode to mclog_report_event_op_mode.
                   04/03/01   fc      Added the function call sd_ss_ind_misc_orig_success in
                                      mcc_cm_mc_rpt if call is connected (SD 2.0 only).
03/28/01   lh      Merged following from common archive.
                   03/08/01   va      Merged in some fixes for FEATURE_PLT
                   03/06/01   fc      Added support of events report:
                                      Operation mode change.
03/02/01   va      Merged the following from common archive:
           mai     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
           sko     Removed JCDMA feature to poll CDMA_CH by CM.
           fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
           bgc     Changed FEATURE_FACTORY_TESTMODE to be a runtime mode.
           ych     Merged T-53/JCDMA features for SK.
           ts      Added support for default NV values when RUIM is used.  The
                   following items were defaulted: NV_ACCOLC_I, NV_HOME_SID_NID_I,
                   NV_SID_NID_I,
           ht      Add params sid/nid/band_class to mcsys_cdma_continue().
           ht      Relay the NV EVRC usage settings to SRV.
           ht      In function cdma_rf_init(), add conditis to allow
                   RF init when band class changed.
           ck      Added reading of the EF OTASP_OTAPA_FEATURES in the function
                   mcc_powerup_load from the R-UIM.
                   Added ruim access buffer to hold the result from the R-UIM.
                   Added setting of the NAM lock state in the R-UIM for a user
                   initiated OTASP Call.
           lcc     Initialize BS_P_PREV and P_REV_IN_USE to MOB_P_REV at power
                   up so that they are consistent.  It used to be that BS_P_REV
                   was initialized to 1 (in db.c) and P_REV_IN_USE was initialized
                   to MOB_P_REV.
           bgc     Updated support for PLT and FTM.

01/10/01   va      Changed msg_okay macros into function calls.
11/13/00   lcc     Added event reporting for p_rev_in_use, band class, and CDMA
                   channel change events.
10/23/00   yll     Added checking the return status in mcc_rxtx_cmd to allow
                   reject for a start GPS command.
09/11/00   yll     Added support for Position Determination DBM.
09/08/00   jq      Added QPCH support in sleep state.
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
08/23/00   jq      Added mcc_set_rsch_dtx_sig ()
08/11/00   va      Cleaned up lint errors.
08/10/00   ts      Added support for default NV values when RUIM is used.
07/14/00   ks      Added support for Slotted mode timer feature.
05/30/00   ck      Modified mcc_nv_cmd to not error fatal when FEATURE_RUIM is
                   defined
05/10/00   fc      Modified cdma_continue() to fix the system lost exit for
                   the scenario of MS wants to acquire the CDMA channel
                   different from the last CDMA active channel.
04/06/00   va      Merged the following from PP (MSM3100_CP.03.00.17):
           jq      Added broadcast SMS feature support.
           ry      Added OTAPA support
           kmp     Added defined(FEATURE_OTASP) && !defined(FEATURE_NSOTASP))
                   in mcc_get_nam_info() around the loading of True IMSI
                   parameters into the cdma_ms structure.
03/31/00   ry      Removed the featurization of rf_digital_band_select()
                   since FEATURE_SRCH_DYNAMIC_BAND_CLASS will always be on
03/30/00   ry      Featurized the call to rf_digital_band_select() under
                   FEATURE_SRCH_DYNAMIC_BAND_CLASS
03/28/00   ry      Fixed compiler warning introduced from the previous merge
03/10/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           vn      CR 12509 - Corrected the handling of case statement for
                   RESELECTION in mcc_subtask (caused by last merge).
           ych     Modified exit_parms_out() to add a case for MC_SILENT
                   _REDIAL_F.
           vn      Do not exit due to Reselection if in E911 service acq. mode.
           ht      Add call to mcsyspr_set_last_cdma after rf_tune_to_chan.
           vn      For trimode phone, peek back at the redirecting system on
                   a Reselection, when redirected by a GSRM.
           ck      Updated band class in cdma_continue()
                   Added band class parameter to cdma_rf_init() and included
                   band class in all the function calls to cdma_rf_init.
                   Added rf_digital_band_select() to notify RF the band class
                   before tuning to a channel.
01/26/00   va      Merged from GV40401
                   Removed mcc_handoff_timer.
11/03/99   lcc     Merged from GD4021:
           fc      Vocoder driver feature clean up. FEATURE_VOC_TASK replaced
                   FEATURE_QDSP2.
           lh      Merged in the following changes for MSM3100.
           spf     Added FEATURE_QDSP2 featurization to support new vocoder
                   interface.
10/28/99   lcc     Moved PLT related code to plt_mc.c.
10/21/99    eh     Added processing of PLT_CMD_DEMOD_QPCH_FRAME command
10/18/99   rm      Added in support for SCCH command for PLT.
10/07/99   lcc     Merged from PLT 2.0:
           jcw     Removed rf_present flag in PLT commands
           eh      added the rest low level cmd handler in plt_low_level_cmd_proc
           jcw     Removed SIMULATED_RF flag since I/O card emulates RF
09/16/99   lcc     Merged in PLT support from PLT archive:
                   1. Added support for the plt reset command
                   2. Fixed problem with setting log mask.
                   3. Supported bozo_mode command for PLT.
                   4. Added PLT logging support for log mask.
                   5. Added nset_update, pc_aset_update, and tc_aset_update
                      commands for PLT.
                   6. Moved the function for PLT FCH command to mcctc.c.
                   7. Merged in PLT changes
                   8. Added support for PLT.
08/21/99   ks      Calling mcc_init_ovhd_info() function in cdma_continue()
                   to reprocesses OVHD messages to keep Neighbor Record
                   in Sync with SRCH.
08/21/99   ks      Modified
08/13/99   lh      CM and SMS phase 2 implementations.
08/09/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
07/28/99   lh      CM and two-way SMS support added.
07/19/99   nmn     Removed a check to change the IS-96A service option to
                   IS-96.
07/09/99   ry      Added a new rex timer for IS95B hard handoff
           ry      Updated mcc_wait to support 32-bit rex signal
06/22/99   kmp     Moved the definitions for IMSI_S1_ZERO, IMSI_S2_ZERO,
                   IMSI_MCC_ZERO and IMSI_11_12_ZERO from this file to
                   mccdma.h since these values are also used in mccidl.c in
                   function proc_true_imsi() when FEATURE_IS95B_TRUE_IMSI is
                   defined.
06/17/99   kmp     In mcc_get_nam_info changed FEATURE_OTASP || !defined
                   FEATURE_OTASP to FEATURE_OTASP && !defined FEATURE_NSOTASP.
06/02/99   kmp     Merged IS-95B changes below and change T_SMS, T_ACP and
                   T_AUTH to FEATURE_SMS, FEATURE_ACP and FEATURE_AUTH
                   respectively.
           kmp     Modified the mcc_pc_frames array to be 3D by adding
                   ADDR_TYPE checking for duplicate page messages for IS-95B.
           kk      Service Redirection parameters are used in case of an
                   MCC_ORIGINATION entry. Also new NDSS exit type processed.
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           sh      added IS95B soft handoff parameters
           kk      exit_parms_out modified to support new system reselection
                   exit code.
           lh      P_REV_IN_USE initialization added at power up.
           lh      IS-95B True IMSI implementation added.
06/02/99   ck      Added reading of IMSI_T items during powerup.
04/26/99   abh     Moved the code to initialise the Overhead Info in bs_info array up in
                   the process of CDMA System Acquisition.
03/18/99   ck      Added mcc_roaming_list to hold a copy of the roaming list
                   that is in NV and removed the feature and code under
                   OTASP_800_HACK
03/18/99   ck      Added the reading of spc_change_enabled item from NV during
                   powerup.
03/18/99   ck      Merged the following changes from rev 1.158 SPSW
           lh      Fixed CR8126. Put cdma_exit inside while loop to avoid
                   commanding RXC to exit before it is started.
           lh      Fixed CR5709. MC now re-tunes after continue in CDMA.
           ht      Add debug message for Standby Improvement.
           br      Added changes to use NAM value from RAM instead from NV.
           ro      Do not define mcc_voc_selftest if FEATURE_VOCODER_MANAGER.
10/09/98   ck      Added reading of Preferred Service Options during powerup

03/09/99   ram     Defined/Initialized SCAM/GHDM duration timer for IS95B MDR.
02/26/99   ejv     Added a MSG_HIGH to help with standby debug effort.
02/19/99   ram     Defined SCAM start timer for Is95B MDR.
01/27/99   ck      Featurised the EVRC Service Option Management changes under
                   FEATURE_EVRC_SO_MGMT
10/09/98   ck      Added reading of Preferred Service Options during powerup
                   for EVRC Service Negotiation
09/04/98   pms     Neared the variables to save ROM space.
09/01/98   lcc     Merged in ARM support.
08/25/98   ck      Added support for the DH task.
08/20/98   ck      Removed mcc_voc_capability, now using new voc_capability_
                   supported interface.
08/04/98   ych     Merged SSPR, E911 and Acquisition improvements.
05/21/98   abh     NV item SID_NID changed for SSPR-800
04/21/98   jca     Changed cdma_exit processing to avoid SRCH "out of buffers"
                   fatal error (SWIS #5054)
01/27/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/27/98   ych     Keep track of the idle timeout with clk_uptime() instead
                   of a rex timer. Also added #include "customer.h" inorder
                   to facilitate for DMSS migration of target.h to customer.h
08/13/97   dna     NAM related NV initialization changes.
07/16/97   lh      Added logging out-of-buffer errors.
07/01/97   rdh     Eliminated T_HO_DEBUG, made dependent upon TMOBILE instead.
06/27/97   ych     Moved some powerup initialization code from mcc_powerup_
                   init() to mcc_powerup_load().
06/17/97   dhh     Configured for Q (T_Q).
06/03/97   day     mcc_nv_buf should not be declared static.  Removing previous
                   change.
06/02/97   day     Declare variable as static to be compliant with ANSI C standard.
05/30/97   ych     Removed case for MC_LPM_F in cdma_enter(). Now, if the
                   mobile station is offline and receives MC_LPM_F, it will
                   ignore it and continue to stay in LPM.
05/20/97   ck      Added support for Low Power Mode
05/10/97   dna     Changed #ifdef on reading IMSI_ADDR_NUM for class 1 IMSI
04/09/97   lh      Remove processing for exit code OTASP_ORIG_RXED.
02/06/97   jca     Now exiting immediately to System Determination on
                   ACCESS_DENIED exit condition.
12/13/96   jca     Added force_amps capability for MC_ORIGINATION_F commands.
11/07/96   dna     Make sure cdma.otasp_call gets set to FALSE when necessary.
10/07/96   jrs     Seperated MCC_ACTIVATION from MCC_ORIGINATION in
                   cdma_enter_init().
09/13/96   dna     More support for OTASP and rewrote mcc_powerup_load().
08/28/96   jpf     Report Service Option in exit_parms_out()
08/08/96   dna     Read in Service Programming Code for OTASP subsidy lock.
07/10/96   day     Add mcc_pc_pg_frames array for layer2 duplicate detection.
07/09/96   dna     Masked return of mcc_wait() with requested_mask.
06/27/96   jca     Now exiting immediately to System Determination on
                   MAX_ACCESS_PROBES exit condition.
06/25/96   dna     For FEATURE_OTASP, loaded mob_dir from NV to cdma.mob_dir.
06/24/96   jrs     Added support for Preferred Roaming.
06/05/96   jpf     Don't clear TMSI timers on exit from CDMA state.
05/30/96   dna     Split support routines off into mccsup.c
05/28/96   dna     Class 1 IMSI support.
05/24/96   jjw     Gemini & ISS2 Data Services release.
05/20/96   rdh     #ifdef'd MC mssg poke code per Don's blessing to save ROM.
05/15/96   dna     Changed auth_on to auth_mode and encryption_enabled to
                   encrypt_mode.
05/13/96   jca     Updates to handle band_class and block_or_sys entry parms.
04/26/96   dna     Status request/response for PCS
04/23/96   jca     mcc_offline() saves NV params when called in ONLINE state.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
03/26/96   jca     Fixed bug where CDMA service indicator would light even
                   though we had not yet collected all the overhead msgs.
03/25/96   jca     Fixed ret_val bug in function mcc_process_info_records().
03/13/96   day     Added J-STD-008 PCS message modifications.
                   Using FEATURE_JSTD008 ifdef.
02/29/96   jca     Temporary fix to allow override of IS95A specified
                   value for N1m.
02/09/96   jca     (1) New system determination algorithm for bailing to
                       next system pref when access probes keep failing.
                   (2) Fixed bad array reference bug uncovered by Lint 7.0.
                   (3) CSS target (T_C1) now reading IMSI address from NV.
01/08/96   jca     Now calling cdma_rf_init on entry to CDMA offline mode.
01/02/96   dna     Fixed duplicate detection.
12/18/95   jca     Fixed function mcc_detect_duplicate() so that it will
                   correctly detect duplicate Paging Channel messages.
11/29/95   jca     No longer using sid_acq_list.
11/27/95   gb      Changed default MCC for test mobiles to be 310.
11/16/95   jca     Removed INTLOCK/INTFREE pair in mcc_queue_auth_rpt().
11/14/95   jca     Removed support for configurable service option 1 def.
09/21/95   rah     Changed voc_test_cmd parameter list.
08/28/95   rdb     Added check for band class qualifier in the processing
                   of operating mode info rec requests in mcc_info_rec.
08/24/95   rdb     Cleaned up mcc_info_rec function.
08/24/95   day     Removed SMS data declaration for mobile-originated
                   messages to recover 4.7k RAM bytes.
08/22/95   dna     Bug fixes for IMSI addressing
08/18/95   dna     Added support for assign mode '101' and IMSI bug fix.
08/17/95   jca     Added function mcc_process_info_records().
08/10/95   rdb     Added cdma_strsp variable to support sending a Status
                   Response Message on the access channel.
07/20/95   jca     Code updates to support subscriber access control.
07/13/95   rdb     Added support for service negotiation (assign mode 4).
07/13/95   gb      Changed mcc_validate_so_req to use srv_eval_so.
07/07/95   dna     Code review changes to authentication.
07/06/95   rah     Removed T_13K references.
07/06/95   jca     Now dealing with a single origination command.
06/25/95   dy      Added support for mobile-originated SMS messages and
                   validation of SMS service option request.
06/20/95   jca     Added support for mobile-originated SO2 Loopback calls.
06/16/95   rdh     Added powerdown command so Search can tidy up on powerdown.
06/15/95   jca     Code updates to support AutoNAM processing.
06/14/95   gb      Added T_13K support for Rate set 2 service options.
06/09/95   jca     Mods to mcc_wait() to use single rex_sig (MCC_Q_SIG) for
                   all MCC input queues.
06/07/95   dna     Added Authentication.
05/25/95   jca     Updates for IS-95A registration procedures.
05/08/95   ptw     Skip ERR_FATAL on bad vocoder for BTU mobiles.
04/13/95   dna     Added new functions for layer 2, shared by SA and IDLE
03/24/95   jca     Added support for IS-99 and packet service option numbers.
11/14/94   dna     Added two functions for Channel Assignment and moved three
                   support functions from mccidl.c.
11/09/94   jah     Changes to use voc_acquire() and voc_release().
11/01/94   gb      Sector Lock test functionality added.
10/13/94   dna     Changed MCC_ENTRY_INIT to not return neighbor config
09/28/94   jca     Fixed TMOBILE JUMP_TRAFFIC command to check vocoder
                   capability for Voice service option.
08/09/94   jca     Added support for user-configurable service option 1.
06/29/94   jah     Disable CODEC clock during Vocoder self-test
06/21/94   jca     Fixed bug with buffers not being returned properly to
                   rxtx_mc_free_q.
06/06/94   jah     Added PCN Portable specific code to turn off slotted-mode.
05/12/94   gb      Added support for new test commands.
04/25/94   gb      Parameterised all IS95 timeouts in cai_tmo.
04/21/94   jca     Modified for IS-95 Phase 2 upgrade.
04/18/94   gb      Changed cdma continue to clear srch_q & send SRCH_START_F.
04/05/94   gb      Changed cdma continue to go to sync acq.
02/26/94   jca     Fixed SEND/END and FM-Only NAM bugs.
01/06/94   jah     Changed watchdog iface to use dog_report().
12/22/93   jca     Modified for IS-95 Phase 1 upgrade.
08/10/93   jca     Now calling function mcsys_cdma_continue() before passing
                   control back to system determination.
06/11/93   jca     Changed processing so that we now power up RX portion of
                   RF h/w, delay 5 msecs, and THEN program the RX synths
                   as per the new RF hardware spec.
06/04/93   gb      Vocoder selftest fix & removed orig_min references.
05/17/93   jai     Fixed roam logic.
03/24/93   gb      Removed debug t_tdrop variable.
10/16/92   jai     Changed ordering of task commands in cdma_exit.
07/24/92   jai     Removed time-outs from procedures which send commands to
                   tasks and added some debug MSG calls.
06/26/92   jai     Modified module for DMSS.
12/20/90   jai     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include <memory.h>
#include "cai.h"
#include "caii.h"
#include "comdef.h"
#include "db.h"
#include "diag.h"
#include "diag.h"
#include "dog.h"
#include "err.h"
#include "mc.h"
#include "mcc.h"
#include "mccdma.h"
#include "mccreg.h"
#include "mccrx.h"
#include "mccrxtx.h"
#include "mccsrch.h"
#include "mci.h"
#include "snm.h"
#include "sd.h"
#include "msg.h"
#include "nv.h"
#include "queue.h"
#include "qw.h"
#include "rex.h"
#include "bit.h"

#ifdef FEATURE_MULTI_RX_CHAIN
#include "rfm.h"
#include "rfcom.h"
#else
#include "rf.h"
#endif /* FEATURE_MULTI_RX_CHAIN */

#include "rxcmc.h"
#include "rxtx.h"
#include "srchmc.h"
#include "srv.h"
#include "task.h"
#include "txcmc.h"
#include "voc.h"

#ifdef FEATURE_AUTH
#include "auth.h"
#endif

#include "tmsi.h"

#ifdef FEATURE_DH
#include "dh.h"
#endif /* FEATURE_DH */

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

#include "mcnvdf.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif

#include "mccccim.h"

#ifdef FEATURE_IS2000_REL_A_SVD
#include "mcctctag.h"
#endif

#ifdef FEATURE_MVS_MIGRATE
#error code not present
#endif

#include "event.h"
#include "mclog.h"
#include "mccbcsms.h"

#include "mmoc.h"

#ifdef FEATURE_UIM_RUIM
#ifdef FEATURE_MULTIMODE_ARCH
#include "tmc.h"
#endif /* FEATURE_MULTIMODE_ARCH */
#endif /* FEATURE_UIM_RUIM */

#include "mccapdf.h"

#if defined(FEATURE_HDR) && defined(T_MSM6500)
#error code not present
#endif

#ifdef FEATURE_NETWORK_PERSONALIZATION
#include "mccnp.h"
#endif

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

#ifdef FEATURE_MEID_SUPPORT
#include "mccmeid.h"
#endif /* FEATURE_MEID_SUPPORT */

#if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_IS2000_REL_A_AES
#error code not present
#endif /* FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif

#ifdef FEATURE_PASSPORT_MODE
#include "mmgsdilib.h"
#endif

#ifdef FEATURE_FULL_TIME_SHDR_3
#include "srch_rx_div.h"
#endif /* end of FEATURE_FULL_TIME_SHDR_3 */

#ifdef FEATURE_UIM_RUIM
#include "uim.h"
#endif

#include "mobile.h"

#include "otaspe.h"

/*lint -e818 */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Timer definitions */
rex_timer_type mcc_rpt_timer;
  /* Timer for reporting to watchdog */
rex_timer_type mcc_state_timer;
  /* Timer for states and substates defined in the CAI */
rex_timer_type mcc_bs_insanity_timer;
  /* Timer for monitoring BS in Idle state */
rex_timer_type mcc_action_timer;
  /* Timer to set for time between current time and received action time */
rex_timer_type mcc_cmd_timer;
  /* Timer for amount of time allowed for a task to respond to a command */
rex_timer_type mcc_so_ctl_timer;
  /* Timer to set for time between current time and received
     Service Option Control Order action time */

rex_timer_type mcc_srv_timer;
  /* Timer for service negotiation subfunctions */

rex_timer_type  mcc_full_tmsi_timer;
  /* Timer for full TMSI timer */

rex_timer_type  mcc_tmsi_exp_timer;
  /* Timer for TMSI expiration timer */

#ifdef FEATURE_IS95B_MDR
rex_timer_type mcc_scam_start_timer;
  /* Timer for the MDR SCAM message start timer */
#endif /* FEATURE_IS95B_MDR */

#if defined(FEATURE_IS95B_MDR)
rex_timer_type mcc_mdr_duration_timer;
#endif /* defined(FEATURE_IS95B_MDR) */

rex_timer_type mcc_slotted_timer;

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

#ifdef FEATURE_SO_8026
#error code not present
#endif /* FEATURE_SO_8026 */

/* this timer is a callback timer. when a transceiver request
 * times out on waiting for resource, the callback function 
 * will be called.
 */
rex_timer_type mcc_tr_request_timer;

rex_timer_type mcc_t50_timer;
rex_timer_type mcc_t51_timer;

#ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Huge structure including many Mobile Station specific variables         */

cdma_ms_type cdma;

/* Variable indicating if powerup of MS has happened */

/* This variable is used to track if  phone's powerup has happened
   based on which some of the NV items could be refreshed */

boolean mcc_is_schm_time_param_chng;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* For holding internal format of Status Response for Access Channel       */

caii_ac_strsp_type  cdma_strsp;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Internal format of (Enhanced) Access Channel Message                               */
caii_tx_msg_type mcc_tx_int_msg;

/* External format of (Enhanced) Access Channel Message                               */
cai_tx_msg_type mcc_tx_ext_msg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Information needed for Paging/Access Channel Layer 2 processing         */

mcc_layer2_type mcc_layer2;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Structure containing Paging Channel duplicate msg sequence time stamps  */
/* for non-page messages.                                                  */
qword mcc_idle_frames[MCC_MAX_ACK_TYPES][MCC_MAX_MSG_SEQ][MCC_MAX_ADDR_TYPE];

/* Structure containing Paging Channel duplicate msg sequence time stamps */
/* for page records.                                                      */
qword mcc_idle_pg_frames[MCC_MAX_MSG_SEQ];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Buffer for item to send to DB                                           */
db_items_value_type mcc_db_val;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Items for DTMF command processing                                       */

/* Maximum number of outstanding DTMF commands */
#define MCC_MAX_DTMFS_OUT 12

/* Buffers for outstanding continuous DTMF commands */
cdma_dtmf_type cdma_dtmf_bufs[MCC_MAX_DTMFS_OUT];

/* Queues for DTMF commands */
q_type mcc_dtmf_q;
q_type mcc_dtmf_free_q;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Structure containing exit information                                   */

mcc_exit_type mcc_exit;

#ifdef FEATURE_EVRC_SO_MGMT
/* Structure containing Preferred Voice Service Option information         */
nv_pref_voice_so_type mcc_pref_voice_so;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Command buffers                                                         */

nv_cmd_type mcc_nv_buf;    /* Buffer for command to NV task */
nv_item_type mcc_nv_data;   /* Buffer for data returned from NV task */
txc_cmd_type mcc_tx_buf;     /* Buffer for command to Transmit task */
rxc_cmd_msg_type mcc_rx_buf; /* Buffer for command to Receive task */
rxtx_cmd_type mcc_rxtx_buf;  /* Buffer for command to Layer 2 task */
srch_cmd_type mcc_srch_buf;  /* Buffer for command to Search task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Received handoff parameters                                             */

mcc_ho_cb_type mcc_ho_real =
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0XFF
   , 0, 0, 0
  };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined(FEATURE_OTASP) && !defined(FEATURE_NSOTASP) && !defined(FEATURE_MEM_REDUCE_PRL)
/* buffer to hold the copy of the roaming list that is in NV */
nv_roaming_list_type mcc_roaming_list;
#endif /*  FEATURE_OTASP && !FEATURE_NSOTASP && !FEATURE_MEM_REDUCE_PRL */

#ifdef FEATURE_UIM_RUIM
nv_esn_type uim_id_cache_buf;
byte uim_access_buffer[UIM_MAX_CHARS];
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_IS2000_REL_A_CC
/* Segmented Universal Page Message re-assembly buffer */
caii_seg_univ_page_type mccdma_seg_univ_page;
#endif /* FEATURE_IS2000_REL_A_CC */

#ifdef FEATURE_PASSPORT_MODE
/* Mask to store the messages which need to be blocked */
byte cdma_block_action_mask;
#endif /* FEATURE_PASSPORT_MODE */

#if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )
#error code not present
#endif /*#if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )*/

/*===========================================================================*

FUNCTION MCC_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mcc_rpt_timer.

DEPENDENCIES
  Depends on previously having used the mcc_rpt_timer to time when to
  set the watchdog signal.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/

dword mcc_wait
(
  dword requested_mask
    /* Mask of signals to wait for */
)
{
  dword queue_mask;       /* Mask of signals indicating queue status    */
  dword rex_signals_mask; /* Mask of signals returned by rex_get_sigs() */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (requested_mask == 0)
  {
    /* No sig to wait for --> Kick the watchdog and return to caller */
    dog_report( DOG_MC_RPT );
    return (0);
  }

  /* Initialize temporary mask holder for queue information */
  queue_mask = 0;

  /* -------------------------------------------------------------------
  ** Check received message queue: If the request mask contains a check
  ** for MCC_RXTX_Q_SIG then we should check the queue, and set in the
  ** queue_mask an indication that something is waiting to be serviced
  ** on that queue.  The rex signal is also cleared regardless of the
  ** queue count.  This is to cover the case where there is a signal set
  ** but nothing on the queue.
  ** ------------------------------------------------------------------- */
  (void) rex_clr_sigs( rex_self(), MCC_Q_SIG );

  if (requested_mask & MCC_RXTX_Q_SIG)
  {
    if (q_cnt( &mcc_rxtx_q ))
    {
      queue_mask |= MCC_RXTX_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }

  /* Same check for command queue */
  if (requested_mask & MCC_CMD_Q_SIG)
  {
    if (q_cnt( &mc_cmd_q ))
    {
      queue_mask |= MCC_CMD_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }

  /* Same check for RXC report queue */
  if (requested_mask & MCC_RX_RPT_Q_SIG)
  {
    if (q_cnt( &mcc_rx_rpt_q ))
    {
      queue_mask |= MCC_RX_RPT_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }

  /* Same check for RXTX acknowledgement queue */
  if (requested_mask & MCC_RXTX_ACK_Q_SIG)
  {
    if (q_cnt( &mcc_rxtx_ack_q ))
    {
      queue_mask |= MCC_RXTX_ACK_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }

  /* Same check for SRCH report queue */
  if (requested_mask & MCC_SRCH_Q_SIG)
  {
    if (q_cnt( &mcc_srch_q ))
    {
      queue_mask |= MCC_SRCH_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }

#ifdef FEATURE_AUTH
  /* Same check for AUTH report queue */
  if (requested_mask & MCC_AUTH_Q_SIG)
  {
    if (q_cnt( &mc_auth_q ))
    {
      queue_mask |= MCC_AUTH_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }
  #endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
  /* Same check for UIM report queue */
  if (requested_mask & MCC_UIM_Q_SIG)
  {
    if (q_cnt( &mc_uim_q ))
    {
      queue_mask |= MCC_UIM_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }
#endif /* FEATURE_UIM_RUIM */

#if (defined(FEATURE_DH) && defined(FEATURE_DH_EXP))
  /* Same check for DH report queue */
  if (requested_mask & MCC_DH_Q_SIG)
  {
    if (q_cnt( &mc_dh_q ))
    {
      queue_mask |= MCC_DH_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }
#endif /* FEATURE_DH && FEATURE_DH_EXP */

  /* Get current state of rex signals */
  rex_signals_mask = rex_get_sigs( rex_self() );

  /* --------------------------------------------------------------------
  ** Loop until the current signals from rex (including the queue signals
  ** faked due to an item on a queue, if any) match with one or more of
  ** the requested signals.
  ** -------------------------------------------------------------------- */
  while ((queue_mask == 0) &&
         ((requested_mask & rex_signals_mask & ~MCC_Q_SIG) == 0))
  {
    /* Kick the watchdog */
    dog_report( DOG_MC_RPT );
    (void) rex_set_timer( &mcc_rpt_timer, DOG_MC_RPT_TIME );

    /* Perform the 'real' wait */
    rex_signals_mask = rex_wait( requested_mask | MCC_RPT_TIMER_SIG );

    /* ---------------------------------------------------------------
    ** Make sure that if we got a queue signal, that there is actually
    ** an item on the queue
    ** --------------------------------------------------------------- */
    if (rex_signals_mask & MCC_Q_SIG)
    {
      (void) rex_clr_sigs( rex_self(), MCC_Q_SIG );

      if (requested_mask & MCC_RXTX_Q_SIG)
      {
        if (q_cnt( &mcc_rxtx_q ))
        {
          queue_mask |= MCC_RXTX_Q_SIG;
        }
      }

      if (requested_mask & MCC_CMD_Q_SIG)
      {
        if (q_cnt( &mc_cmd_q ))
        {
          queue_mask |= MCC_CMD_Q_SIG;
        }
      }

      if (requested_mask & MCC_RX_RPT_Q_SIG)
      {
        if (q_cnt( &mcc_rx_rpt_q ))
        {
          queue_mask |= MCC_RX_RPT_Q_SIG;
        }
      }

      if (requested_mask & MCC_RXTX_ACK_Q_SIG)
      {
        if (q_cnt( &mcc_rxtx_ack_q ))
        {
          queue_mask |= MCC_RXTX_ACK_Q_SIG;
        }
      }

      if (requested_mask & MCC_SRCH_Q_SIG)
      {
        if (q_cnt( &mcc_srch_q ))
        {
          queue_mask |= MCC_SRCH_Q_SIG;
        }
      }

#ifdef FEATURE_AUTH
      if (requested_mask & MCC_AUTH_Q_SIG)
      {
        if (q_cnt( &mc_auth_q ))
        {
          queue_mask |= MCC_AUTH_Q_SIG;
        }
      }
#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
      if (requested_mask & MCC_UIM_Q_SIG)
      {
        if (q_cnt( &mc_uim_q ))
        {
          queue_mask |= MCC_UIM_Q_SIG;
        }
      }
#endif /* FEATURE_UIM_RUIM */

#if (defined(FEATURE_DH) && defined(FEATURE_DH_EXP))
      if (requested_mask & MCC_DH_Q_SIG)
      {
        if (q_cnt( &mc_dh_q ))
        {
          queue_mask |= MCC_DH_Q_SIG;
        }
      }
      #endif /* FEATURE_DH && FEATURE_DH_EXP */

    } /* end if (rex_signals_mask & MCC_Q_SIG) */
  } /* end while */

  /* Kick watchdog one last time on the way out */
  dog_report( DOG_MC_RPT );
  (void) rex_set_timer( &mcc_rpt_timer, DOG_MC_RPT_TIME );

  /* We added the MCC_Q_SIG to requested_mask if we needed to wait on a
   * queue.  Now that we know one of the real requested signals is set,
   * remove the MCC_Q_SIG from the mask so we only return the signals
   * that were requested.
   */
  requested_mask &= ~(dword)MCC_Q_SIG;

  /* Assemble return dword (note that queue_mask uses the MS word) */
  return ((rex_signals_mask | queue_mask) & requested_mask);

} /* mcc_wait */

/*===========================================================================

FUNCTION MCC_SRCH_CMD

DESCRIPTION
  This function sends a command to the Searcher task.  If the Searcher task
  responds with a status other than SRCH_DONE_S,  this function will call
  ERR_FATAL to log the fatal error.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.  If this function returns, the status returned by the Searcher
  task was SRCH_DONE_S.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_srch_cmd
(
  srch_cmd_type *cmd_ptr
    /* pointer to command to send to Searcher */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->hdr.cmd_hdr.task_ptr = &mc_tcb;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );

  /* Send Command to Search task */
  srch_cmd( cmd_ptr );

  MSG_MED("mc->srch cmd=%d", cmd_ptr->hdr.cmd, 0, 0);

  /* Wait for response. For PLT, this signal is set
     when SRCH task has completed the PLT command */
  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );

  if(( cmd_ptr->hdr.status != SRCH_DONE_S ) &&
     ( cmd_ptr->hdr.status != SRCH_TOOLATE_S ) &&
     ( cmd_ptr->hdr.status != SRCH_SLEEP_S ) )
  {
    //ERR_FATAL( "Bad status %d to SRCH cmd %d", cmd_ptr->hdr.status,
    ERR(         "Bad status %d to SRCH cmd %d", cmd_ptr->hdr.status,
                 cmd_ptr->hdr.cmd,  0 );
  }

  #ifdef FEATURE_PLT
#error code not present
  #endif /* FEATURE_PLT */

} /* mcc_srch_cmd */

/*===========================================================================

FUNCTION MCC_RXC_CMD

DESCRIPTION
  This function sends a command to the CDMA Receive subtask.  If the Receive
  responds with a status other than RXC_DONE_S,  this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.  If this function returns the status returned by RXC was RXC_DONE_S.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_rxc_cmd
(
  rxc_cmd_msg_type *cmd_ptr
    /* pointer to command to send to Receive task */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->hdr.cmd_hdr.task_ptr = &mc_tcb;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );
  /* Send command to RX */
  rxc_cmd( cmd_ptr );

  MSG_MED("mc->rxc cmd=%d", cmd_ptr->hdr.command, 0, 0);

  /* Wait for response */
  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );

  if (cmd_ptr->hdr.status != RXC_DONE_S)
  {
    #ifdef FEATURE_FACTORY_TESTMODE
    if (ftm_mode == FTM_MODE)
    {
      ERR("Bad status %d to RXC cmd %d", cmd_ptr->hdr.status,
        cmd_ptr->hdr.command, 0);
    }
    else
    {
      ERR_FATAL("Bad status %d to RXC cmd %d", cmd_ptr->hdr.status,
        cmd_ptr->hdr.command, 0);
    }
    #else /* FEATURE_FACTORY_TESTMODE */
    ERR_FATAL("Bad status %d to RXC cmd %d", cmd_ptr->hdr.status,
      cmd_ptr->hdr.command, 0);
    #endif /* FEATURE_FACTORY_TESTMODE */
  }

  #ifdef FEATURE_FACTORY_TESTMODE
  if (ftm_mode == FTM_MODE)
  {
    /* Determine FTM command status */
    switch (cmd_ptr->hdr.status)
    {
      /* TEST command is successfully executed */
      case RXC_DONE_S:
        test_cmd_status = TEST_SUCCESS;
        break;

      /* Invalid state for the TEST command */
      case RXC_BADSEQ_S:
        test_cmd_status = TEST_INVALID_STATE;
        break;

      /* Fail to complete the TEST command */
      default:
        test_cmd_status = TEST_NO_SUCCESS;
        break;
    } /* end switch */
  }
  #endif /* FEATURE_FACTORY_TESTMODE */

} /* mcc_rxc_cmd */

/*===========================================================================

FUNCTION MCC_TXC_CMD

DESCRIPTION
  This function sends a command to the CDMA Transmit subtask.  If the Transmit
  task responds with a status other than TXC_DONE_S,  this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_txc_cmd
(
  txc_cmd_type *cmd_ptr
    /* pointer to command to send to Transmit task */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->hdr.cmd_hdr.task_ptr = &mc_tcb;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );
  txc_cmd( cmd_ptr );

  MSG_MED("mc->txc cmd=%d", cmd_ptr->hdr.command, 0, 0);

  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );

  if( cmd_ptr->hdr.status != TXC_DONE_S )
  {
    if (cmd_ptr->hdr.command == TXC_JUMP_F)
    {
      MSG_HIGH("TXC JUMP_F not supported", 0,0,0);
    }
    else
    {
      ERR_FATAL( "Bad status %d to TXC cmd %d", cmd_ptr->hdr.status,
                  cmd_ptr->hdr.command,  0 );
    }
  }

} /* mcc_txc_cmd */

/*===========================================================================

FUNCTION MCC_RXTX_CMD

DESCRIPTION
  This function sends a command to the Layer 2 task.  If the Layer 2
  responds with a status other than RXTX_DONE_S, this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_rxtx_cmd
(
  rxtx_cmd_type *cmd_ptr
    /* pointer to command to send to Layer 2 task */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->hdr.cmd_hdr.task_ptr = &mc_tcb;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );
  /* Send command to Layer 2 task */
  rxtx_cmd( cmd_ptr );

  MSG_MED("mc->rxtx cmd=%d", cmd_ptr->hdr.command, 0, 0);

  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );

  if( cmd_ptr->hdr.status != RXTX_DONE_S )
  {
    #ifdef FEATURE_GPS_MODE
#error code not present
    #endif /* FEATURE_GPS_MODE */
    {
      ERR_FATAL( "Bad status %d to RXTX cmd %d", cmd_ptr->hdr.status,
                  cmd_ptr->hdr.command,  0 );
    }
  }

} /* mcc_rxtx_cmd */

/*===========================================================================

FUNCTION MCC_NV_CMD
  This function sends a command to the NV task.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  Status returned by nv_cmd procedure.

SIDE EFFECTS
  If the status indicates an error (not DONE or NOTACTIVE) then ERR_FATAL
  is called.

===========================================================================*/

nv_stat_enum_type mcc_nv_cmd
(
  nv_cmd_type *cmd_ptr
    /* Pointer to command to send to NV */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->done_q_ptr = NULL;
  cmd_ptr->tcb_ptr = &mc_tcb;
  cmd_ptr->sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );
  nv_cmd( cmd_ptr );

  MSG_MED("mc->nv cmd=%d", cmd_ptr->cmd, 0, 0);

  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );

  if ((cmd_ptr->status != NV_DONE_S) && (cmd_ptr->status != NV_NOTACTIVE_S))
  {
    #ifdef FEATURE_UIM_RUIM

    #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
    #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      ERR("NV failed for cmd %d item %d", cmd_ptr->cmd, cmd_ptr->item, 0);
    }

    #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
    #endif /* FEATURE_UIM_RUN_TIME_ENABLE */

    #endif /* FEATURE_UIM_RUIM */

    #if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      ERR_FATAL("NV failed for cmd %d item %d", cmd_ptr->cmd, cmd_ptr->item, 0);
    }
    #endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  }

  return (cmd_ptr->status);

} /* mcc_nv_cmd */

#ifdef FEATURE_AUTH
/*===========================================================================

FUNCTION MCC_AUTH_CMD

DESCRIPTION
  This function sends a command to the Authentication Task then waits
  for the task to signal back that the command has been received.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_auth_cmd
(
  auth_cmd_type *cmd_ptr
    /* pointer to command to send to Authentication task */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.task_ptr = &mc_tcb;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );
  auth_cmd( cmd_ptr );

  MSG_MED("mc->auth cmd=%d", cmd_ptr->hdr.command, 0, 0);

  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );

} /* mcc_auth_cmd */
#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION MCC_UIM_CMD

DESCRIPTION
  This function sends a command to the UIM Task then waits
  for the task to signal back that the command has been received.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Waits for command to be received and UIM task to START processing
  the command.

===========================================================================*/

void mcc_uim_cmd
(
  uim_cmd_type *cmd_ptr
    /* pointer to command to send to UIM task */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.task_ptr = &mc_tcb;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;
  cmd_ptr->hdr.protocol = UIM_CDMA;
  #if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )
#error code not present
  #endif /*#if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )*/
  cmd_ptr->hdr.slot = UIM_SLOT_AUTOMATIC;
  cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;

  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );
  uim_cmd( cmd_ptr );

  MSG_MED("mc->uim cmd=%d", cmd_ptr->hdr.command, 0, 0);

  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( &mc_tcb,  MCC_CMD_SIG );

} /* mcc_uim_cmd */
#endif /* FEATURE_UIM_RUIM */

#if (defined (FEATURE_DH) && defined(FEATURE_DH_EXP))
/*===========================================================================

FUNCTION MCC_DH_CMD

DESCRIPTION
  This function sends a command to the DH Task aynschronously.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_dh_cmd
(
  dh_cmd_type *cmd_ptr
    /* pointer to command to send to DH task */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.task_ptr = &mc_tcb;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  dh_cmd( cmd_ptr );

  MSG_MED("mc->dh cmd=%d", cmd_ptr->hdr.command, 0, 0);

} /* mcc_dh_cmd */
#endif /* FEATURE_DH && FEATURE_DH_EXP */

/* Stub out the vocoder structures/APIs for 8K targets */
#ifndef FEATURE_8K_STUFF_TO_BE_PURGED
/*===========================================================================

FUNCTION MCC_VOC_SELFTEST

DESCRIPTION
  This procedure powers up and sends the selftest command to the vocoder.
  If the selftest fails the phone err_fatals, else the vocoder version is
  stored.

DEPENDENCIES
  None.

RETURN VALUE
  Result of tests in vocoder.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_voc_selftest( void )
{

#ifndef FEATURE_VOC_TASK
#error code not present
  #endif /* !FEATURE_VOC_TASK */
} /* end mcc_voc_selftest() */
#endif /* !FEATURE_8K_STUFF_TO_BE_PURGED */

/*===========================================================================

FUNCTION MCC_GET_DEFAULT_NAM_INFO

DESCRIPTION
  This function loads the CDMA structure with the default values associated 
  with the given NAM.

DEPENDENCIES
  The value of cdma.esn needs to already be correct.

RETURN VALUE
  Whether or not the NAM information was read OK.  For FEATURE_OTASP, TRUE
  is always returned, since the NAM is allowed to be uninitialized.

SIDE EFFECTS
  None
  
===========================================================================*/

void mcc_get_default_nam_info
(
  byte nam
    /* Which nam to read information for */
)
{

  word i;

  /* Set MIN1 to default value */
  mcc_nv_buf.item = NV_MIN1_I;
  mcc_nv_data.min1.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_s1 = mcc_nv_data.min1.min1[NV_CDMA_MIN_INDEX];

  /* Set MIN2 to default value */
  mcc_nv_buf.item = NV_MIN2_I;
  mcc_nv_data.min2.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_s2 = mcc_nv_data.min2.min2[NV_CDMA_MIN_INDEX];

  /* Set IMSI_ADDR_NUM to default value */
  mcc_nv_buf.item = NV_IMSI_ADDR_NUM_I;
  mcc_nv_data.imsi_addr_num.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_addr_num = mcc_nv_data.imsi_addr_num.num;

  /* Set IMSI_MCC to default value */
  mcc_nv_buf.item = NV_IMSI_MCC_I;
  mcc_nv_data.imsi_mcc.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.mcc = mcc_nv_data.imsi_mcc.imsi_mcc;

  /* Set IMSI_11_12 to default value */
  mcc_nv_buf.item = NV_IMSI_11_12_I;
  mcc_nv_data.imsi_11_12.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_11_12 = mcc_nv_data.imsi_11_12.imsi_11_12;

  /* Set IMSI_T_S1 to default value */
  mcc_nv_buf.item = NV_IMSI_T_S1_I;
  mcc_nv_data.imsi_t_s1.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_t_s1 = mcc_nv_data.imsi_t_s1.min1[NV_CDMA_MIN_INDEX];

  /* Set IMSI_T_S2 to default value */
  mcc_nv_buf.item = NV_IMSI_T_S2_I;
  mcc_nv_data.imsi_t_s2.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_t_s2 = mcc_nv_data.imsi_t_s2.min2[NV_CDMA_MIN_INDEX];

  /* Set IMSI_T_ADDR_NUM to default value */
  mcc_nv_buf.item = NV_IMSI_T_ADDR_NUM_I;
  mcc_nv_data.imsi_t_addr_num.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_t_addr_num = mcc_nv_data.imsi_t_addr_num.num;

  /* Set IMSI_T_MCC to default value */
  mcc_nv_buf.item = NV_IMSI_T_MCC_I;
  mcc_nv_data.imsi_t_mcc.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_t_mcc = mcc_nv_data.imsi_t_mcc.imsi_mcc;

  /* Set IMSI_T_11_12 to default value */
  mcc_nv_buf.item = NV_IMSI_T_11_12_I;
  mcc_nv_data.imsi_t_11_12.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_t_11_12 = mcc_nv_data.imsi_t_11_12.imsi_11_12;

  /* Set ACCOLC to default value */
  mcc_nv_buf.item = NV_ACCOLC_I;
  mcc_nv_data.accolc.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.accolc = mcc_nv_data.accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX];

  /* Set (SID,NID) pairs to default values */
  #ifdef FEATURE_SSPR_ENHANCEMENTS
  mcc_nv_buf.item = NV_HOME_SID_NID_I;
  mcc_nv_data.home_sid_nid.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);

  /* When FEATURE_SSPR_ENHANCEMENTS is defined, 
        the size of cdma.pair is NV_MAX_HOME_SID_NID */
  for (i=0; i<NV_MAX_HOME_SID_NID; i++)
  {
    cdma.pair[i].sid = mcc_nv_data.home_sid_nid.pair[i].sid;
    cdma.pair[i].nid = mcc_nv_data.home_sid_nid.pair[i].nid;
  }
  #else
  mcc_nv_buf.item = NV_SID_NID_I;
  mcc_nv_data.home_sid_nid.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);

  /* When FEATURE_SSPR_ENHANCEMENTS is not defined,
        the size of cdma.pair is NV_MAX_SID_NID */
  for (i=0; i<NV_MAX_SID_NID; i++)
  {
    cdma.pair[i].sid = mcc_nv_data.sid_nid.pair[i].sid;
    cdma.pair[i].nid = mcc_nv_data.sid_nid.pair[i].nid;
  }
  #endif /* FEATURE_SSPR_ENHANCEMENTS */
  
  return;

} /* mcc_get_default_nam_info */

/*===========================================================================

FUNCTION MCC_GET_NAM_INFO

DESCRIPTION
  This function loads the CDMA structure with the NV items associated with
  the given NAM.  Items are set to default values before the 1x subscription
  is available.  NV or RUIM access would only happen after 1x subscription
  is available.

DEPENDENCIES
  The value of cdma.esn needs to already be correct.

RETURN VALUE
  Whether or not the NAM information was read OK.  For FEATURE_OTASP, TRUE
  is always returned, since the NAM is allowed to be uninitialized.

SIDE EFFECTS
  If an item has never been written it is set to a default vaule.

===========================================================================*/

boolean mcc_get_nam_info
(
  byte nam
    /* Which nam to read information for */
)
{
  word    j;
    /* Index into (SID,NID) pairs */
  boolean read_ok = TRUE;
    /* indicates if items were read from NV successfully */

#ifdef FEATURE_EVRC_SO_MGMT
  nv_stat_enum_type    nv_read_status; /* for status from NV read         */
#endif

  nv_stat_enum_type    nv_status_for_default; /* used for defaulting NV items */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the common fields of the command we will send to NV */
  mcc_nv_buf.cmd        = NV_READ_F;            /* We will do reads        */
  mcc_nv_buf.data_ptr   = &mcc_nv_data;         /* Initialize data pointer */

  /* Initialize Operational IMSI information */
  cdma.imsi_s1 = IMSI_S1_ZERO;
  cdma.imsi_s2 = IMSI_S2_ZERO;
  cdma.mcc = IMSI_MCC_ZERO;
  cdma.imsi_11_12 = IMSI_11_12_ZERO;
  cdma.imsi_addr_num = IMSI_CLASS_0_ADDR_NUM;

  /* ---------------------------------------------------------------
  ** For IS-683A mobiles that are not IS-95B compliant, we will just
  ** zero out the operational and true imsi fields of the cdma
  ** structure. In the non IS-95B case, the operation imsi and min
  ** based imsi are identical.
  ** --------------------------------------------------------------- */
  /* Zero out the min based imsi values */
  cdma.imsi_m_s1 = IMSI_S1_ZERO;
  cdma.imsi_m_s2 = IMSI_S2_ZERO;
  cdma.imsi_m_mcc = IMSI_MCC_ZERO;
  cdma.imsi_m_11_12 = IMSI_11_12_ZERO;
  cdma.imsi_m_addr_num = IMSI_CLASS_0_ADDR_NUM;

  /* Zero out the true imsi values */
  cdma.imsi_t_s1 = IMSI_S1_ZERO;
  cdma.imsi_t_s2 = IMSI_S2_ZERO;
  cdma.imsi_t_mcc = IMSI_MCC_ZERO;
  cdma.imsi_t_11_12 = IMSI_11_12_ZERO;
  cdma.imsi_t_addr_num = IMSI_CLASS_0_ADDR_NUM;

  cdma.accolc = 0;
  cdma.mob_term_home = 1;
  cdma.mob_term_for_sid = 1;
  cdma.mob_term_for_nid = 1;

#ifdef FEATURE_SSPR_ENHANCEMENTS
  for (j=0; j < NV_MAX_HOME_SID_NID; j++)
#else
  for (j=0; j < NV_MAX_SID_NID; j++)
#endif
  {
    cdma.pair[j].sid = 0;
    cdma.pair[j].nid = 0;
  }

  /* Initialize feature specific MIN information */
#ifdef FEATURE_OTASP
  cdma.mob_dir.n_digits = 0;
#endif /* FEATURE_OTASP */

  cdma.imsi_addr_num = IMSI_CLASS_0_ADDR_NUM;

  /* The rest are TMSI items */
  cdma.assigning_tmsi.info.tmsi_code = INVALID_TMSI_CODE;

  /* Events update: TMSI assigned */
  event_report_payload(EVENT_TMSI_ASSIGNED, sizeof(event_tmsi_assigned_type),
    &cdma.assigning_tmsi.info.tmsi_code);

  cdma.assigning_tmsi.zone_len = 0;
  qw_set ( cdma.assigning_tmsi.info.tmsi_zone, 0L, 0L );
  cdma.assigning_tmsi.exp_time = 0;

#ifdef FEATURE_OTASP_OTAPA
  cdma.otapa_enabled = TRUE;
  cdma.nam_lock = TRUE;
#endif /* FEATURE_OTASP_OTAPA */

#if defined(FEATURE_OTASP) && !defined(FEATURE_NSOTASP) && !defined(FEATURE_MEM_REDUCE_PRL)
  mcc_roaming_list.valid = FALSE;
  mcc_roaming_list.size = 0;
#endif /* FEATURE_OTASP */

#ifdef FEATURE_EVRC_SO_MGMT
  /* Set Preferred Voice Service Options to default values */
  mcc_pref_voice_so.evrc_capability_enabled  =  FALSE ;

  mcc_pref_voice_so.home_page_voice_so =  CAI_SO_VOICE_WILDCARD;

#ifndef FEATURE_NO_13K_VOICE_SUPPORT
  mcc_pref_voice_so.home_orig_voice_so =  CAI_SO_VOICE_13K ;
  mcc_pref_voice_so.roam_orig_voice_so =  CAI_SO_VOICE_13K ;
#else
#error code not present
#endif /* FEATURE_NO_13K_VOICE_SUPPORT */

#endif /* FEATURE_EVRC_SO_MGMT */

  if (!cdma.is_cdma_subs_avail) 
  {
    MSG_HIGH("1X subscription is not available, loading default nam info", 
              0, 0, 0);
    mcc_get_default_nam_info(nam);
  }

  else 
  {
    /* 1X Subscription is available */

    /* ---------------------------------------------------------------
    ** Note that UI should have already ensured that the five NV items
    ** associated with the IMSI (min1, min2, imsi_mcc, imsi_11_12, and
    ** imsi_addr_num) are active in NV and are defaulted correctly.
    ** Also, we are reading the MIN based values and loading into the
    ** operational imsi parameters (imsi_s1 and imsi_s2).
    ** If we have been downloaded a true imsi (imsi_t) value from an
    ** OTASP session, we just ignore it unless the mobile is IS-95B
    ** True IMSI compliant.
    ** --------------------------------------------------------------- */

    /* Read in MIN1 (actually IMSI_S1, already encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_MIN1_I;
    mcc_nv_data.min1.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_s1 = mcc_nv_data.min1.min1[NV_CDMA_MIN_INDEX];
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.imsi_s1 = mcc_nv_data.min1.min1[NV_CDMA_MIN_INDEX];
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      ERR( "IMSI_S1 was NOT ACTIVE!",0,0,0);
      read_ok = FALSE;
    }

    /* Read in MIN2 (actually IMSI_S2, already encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_MIN2_I;
    mcc_nv_data.min2.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_s2 = mcc_nv_data.min2.min2[NV_CDMA_MIN_INDEX];
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.imsi_s2 = mcc_nv_data.min2.min2[NV_CDMA_MIN_INDEX];
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      read_ok = FALSE;
    }

    /* Read in IMSI_ADDR_NUM (Number of digits in NMSI - 4) */
    mcc_nv_buf.item = NV_IMSI_ADDR_NUM_I;
    mcc_nv_data.imsi_addr_num.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_addr_num = mcc_nv_data.imsi_addr_num.num;
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.imsi_addr_num = mcc_nv_data.imsi_addr_num.num;
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      read_ok = FALSE;
    }

    /* Read in IMSI_MCC (10 bits encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_IMSI_MCC_I;
    mcc_nv_data.imsi_mcc.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.mcc = mcc_nv_data.imsi_mcc.imsi_mcc;
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.mcc = mcc_nv_data.imsi_mcc.imsi_mcc;
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      read_ok = FALSE;
    }

    /* Read in IMSI_11_12 (7 bits encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_IMSI_11_12_I;
    mcc_nv_data.imsi_11_12.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_11_12 = mcc_nv_data.imsi_11_12.imsi_11_12;
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.imsi_11_12 = mcc_nv_data.imsi_11_12.imsi_11_12;
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      read_ok = FALSE;
    }


    /* ---------------------------------------------------------------
    ** A copy of IMSI_T will also be kept in mobile
    ** --------------------------------------------------------------- */
    /* Read in IMSI_T_S1 */
    mcc_nv_buf.item = NV_IMSI_T_S1_I;
    mcc_nv_data.imsi_t_s1.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_t_s1 = mcc_nv_data.imsi_t_s1.min1[NV_CDMA_MIN_INDEX];
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.imsi_t_s1 = mcc_nv_data.imsi_t_s1.min1[NV_CDMA_MIN_INDEX];
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      ERR( "IMSI_T_S1 was NOT ACTIVE!",0,0,0);
      read_ok = FALSE;
    }

    /* Read in IMSI_T_S2 */
    mcc_nv_buf.item = NV_IMSI_T_S2_I;
    mcc_nv_data.imsi_t_s2.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_t_s2 = mcc_nv_data.imsi_t_s2.min2[NV_CDMA_MIN_INDEX];
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.imsi_t_s2 = mcc_nv_data.imsi_t_s2.min2[NV_CDMA_MIN_INDEX];
    }

#endif /* FEATURE_NV_RUIM */
    else
    {
      ERR( "IMSI_T_S2 was NOT ACTIVE!",0,0,0);
      read_ok = FALSE;
    }

    /* Read in IMSI_T_ADDR_NUM (Number of digits in NMSI - 4) */
    mcc_nv_buf.item = NV_IMSI_T_ADDR_NUM_I;
    mcc_nv_data.imsi_t_addr_num.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_t_addr_num = mcc_nv_data.imsi_t_addr_num.num;
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.imsi_t_addr_num = mcc_nv_data.imsi_t_addr_num.num;
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      ERR( "IMSI_T_ADDR_NUM was NOT ACTIVE!",0,0,0);
      read_ok = FALSE;
    }

    /* Read in IMSI_T_MCC (10 bits encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_IMSI_T_MCC_I;
    mcc_nv_data.imsi_t_mcc.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_t_mcc = mcc_nv_data.imsi_t_mcc.imsi_mcc;
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          /* Determine if the R-UIM is available for this NAM */
          && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.imsi_t_mcc = mcc_nv_data.imsi_t_mcc.imsi_mcc;
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      ERR( "IMSI_T_MCC was NOT ACTIVE!",0,0,0);
      read_ok = FALSE;
    }

    /* Read in IMSI_T_11_12 (7 bits encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_IMSI_T_11_12_I;
    mcc_nv_data.imsi_t_11_12.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_t_11_12 = mcc_nv_data.imsi_t_11_12.imsi_11_12;
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.imsi_t_11_12 = mcc_nv_data.imsi_t_11_12.imsi_11_12;
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      ERR( "IMSI_T_11_12 was NOT ACTIVE!",0,0,0);
      read_ok = FALSE;
    }

#ifdef FEATURE_OTASP
    /* Read Mobile Directory Number */
    mcc_nv_buf.item = NV_DIR_NUMBER_PCS_I;
    mcc_nv_data.mob_dir_number.nam = nam;

    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      cdma.mob_dir.n_digits = mcc_nv_data.mob_dir_number.n_digits;
      if(cdma.mob_dir.n_digits > OTASPI_N_DIGITS_MAX)
      {
        ERR( "Num MDN digits %d read from NV > DIGITS_MAX",
             cdma.mob_dir.n_digits,0,0 );
        cdma.mob_dir.n_digits = OTASPI_N_DIGITS_MAX;
      }
      (void) memcpy (cdma.mob_dir.digitn, (byte*) mcc_nv_data.mob_dir_number.digitn,
                     cdma.mob_dir.n_digits);
    }
#endif /* FEATURE_OTASP */

    /* Read in ACCOLC */
    mcc_nv_buf.item = NV_ACCOLC_I;
    mcc_nv_data.accolc.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.accolc = mcc_nv_data.accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX];
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.accolc = mcc_nv_data.accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX];
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      read_ok = FALSE;
    }

    /* Read in MOB_TERM_HOME */
    mcc_nv_buf.item = NV_MOB_TERM_HOME_I;
    mcc_nv_data.mob_term_home.nam = nam;

    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      cdma.mob_term_home = mcc_nv_data.mob_term_home.enabled[NV_CDMA_MIN_INDEX];
    }

    /* Read in MOB_TERM_FOR_SID  */
    mcc_nv_buf.item = NV_MOB_TERM_FOR_SID_I;
    mcc_nv_data.mob_term_for_sid.nam = nam;

    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      cdma.mob_term_for_sid =
        mcc_nv_data.mob_term_for_sid.enabled[NV_CDMA_MIN_INDEX];
    }

    /* Read in MOB_TERM_FOR_NID  */
    mcc_nv_buf.item = NV_MOB_TERM_FOR_NID_I;
    mcc_nv_data.mob_term_for_nid.nam = nam;

    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      cdma.mob_term_for_nid =
        mcc_nv_data.mob_term_for_nid.enabled[NV_CDMA_MIN_INDEX];
    }

#ifdef FEATURE_SSPR_ENHANCEMENTS
    /* Read in (SID,NID) pairs */
    mcc_nv_buf.item = NV_HOME_SID_NID_I;
    mcc_nv_data.home_sid_nid.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      for (j=0; j < NV_MAX_HOME_SID_NID; j++)
      {
        cdma.pair[j].sid = mcc_nv_data.home_sid_nid.pair[j].sid;
        cdma.pair[j].nid = mcc_nv_data.home_sid_nid.pair[j].nid;
      }
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);

      for (j=0; j < NV_MAX_HOME_SID_NID; j++)
      {
        cdma.pair[j].sid = mcc_nv_data.home_sid_nid.pair[j].sid;
        cdma.pair[j].nid = mcc_nv_data.home_sid_nid.pair[j].nid;
      }
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      read_ok = FALSE;
    }

#else

    /* Read in (SID,NID) pairs */
    mcc_nv_buf.item = NV_SID_NID_I;
    mcc_nv_data.sid_nid.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      for (j=0; j < NV_MAX_SID_NID; j++)
      {
        cdma.pair[j].sid = mcc_nv_data.sid_nid.pair[NV_CDMA_MIN_INDEX][j].sid;
        cdma.pair[j].nid = mcc_nv_data.sid_nid.pair[NV_CDMA_MIN_INDEX][j].nid;
      }
    }

#if defined( FEATURE_NV_RUIM )
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);

      for (j=0; j < NV_MAX_SID_NID; j++)
      {
        cdma.pair[j].sid = mcc_nv_data.sid_nid.pair[NV_CDMA_MIN_INDEX][j].sid;
        cdma.pair[j].nid = mcc_nv_data.sid_nid.pair[NV_CDMA_MIN_INDEX][j].nid;
      }
    }
#endif /* FEATURE_NV_RUIM */

    else
    {
      read_ok = FALSE;
    }

#endif

    /* read in TMSI_CODE */
    mcc_nv_buf.item = NV_TMSI_CODE_I;
    mcc_nv_data.tmsi_code.nam = nam;

    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      cdma.assigning_tmsi.info.tmsi_code = mcc_nv_data.tmsi_code.code;

      /* Events update: TMSI assigned */
      event_report_payload(EVENT_TMSI_ASSIGNED, sizeof(event_tmsi_assigned_type),
        &cdma.assigning_tmsi.info.tmsi_code);
    }

    /* read in TMSI_ZONE_LEN */
    if (cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE)
    {
      mcc_nv_buf.item = NV_ASSIGNING_TMSI_ZONE_LEN_I;
      mcc_nv_data.assigning_tmsi_zone_length.nam = nam;

      if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
      {
        cdma.assigning_tmsi.zone_len =
           mcc_nv_data.assigning_tmsi_zone_length.length;
      }

      /* read in TMSI_ZONE */
      mcc_nv_buf.item = NV_ASSIGNING_TMSI_ZONE_I;
      mcc_nv_data.assigning_tmsi_zone.nam = nam;

      if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
      {
        qw_equ(cdma.assigning_tmsi.info.tmsi_zone,
               mcc_nv_data.assigning_tmsi_zone.zone);
      }
      else
      {
        cdma.assigning_tmsi.info.tmsi_code = INVALID_TMSI_CODE;

        /* Events update: TMSI assigned */
        event_report_payload(EVENT_TMSI_ASSIGNED, sizeof(event_tmsi_assigned_type)
          , &cdma.assigning_tmsi.info.tmsi_code);
      }

      /* read in TMSI_EXP_TIME */
      mcc_nv_buf.item = NV_TMSI_EXP_I;
      mcc_nv_data.tmsi_exp_timer.nam = nam;

      if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
      {
        cdma.assigning_tmsi.exp_time = mcc_nv_data.tmsi_exp_timer.time;
      }
      else
      {
        cdma.assigning_tmsi.info.tmsi_code = INVALID_TMSI_CODE;

        /* Events update: TMSI assigned */
        event_report_payload(EVENT_TMSI_ASSIGNED,
          sizeof(event_tmsi_assigned_type), &cdma.assigning_tmsi.info.tmsi_code);
      }
    }     /* end if (tmsi_code != INVALID_TMSI_CODE)    */

    /* Sanity check: A code without a zone is like a Rebel Without a Clue    */
    if ((cdma.assigning_tmsi.zone_len < 1) ||
        (cdma.assigning_tmsi.zone_len > sizeof(qword)))
    {
      cdma.assigning_tmsi.info.tmsi_code = INVALID_TMSI_CODE;

      /* Events update: TMSI assigned */
      event_report_payload(EVENT_TMSI_ASSIGNED,
        sizeof(event_tmsi_assigned_type), &cdma.assigning_tmsi.info.tmsi_code);
    }

#ifdef FEATURE_EVRC_SO_MGMT
    /* read in Preferred Voice Service Options */
    mcc_nv_buf.item = NV_PREF_VOICE_SO_I;
    mcc_nv_data.pref_voice_so.nam = cdma.curr_nam;
    nv_read_status = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_read_status == NV_DONE_S)
    {
      mcc_pref_voice_so.evrc_capability_enabled  =
                            mcc_nv_data.pref_voice_so.evrc_capability_enabled;

      mcc_pref_voice_so.home_page_voice_so =
                            mcc_nv_data.pref_voice_so.home_page_voice_so;
      mcc_pref_voice_so.home_orig_voice_so =
                            mcc_nv_data.pref_voice_so.home_orig_voice_so;
      mcc_pref_voice_so.roam_orig_voice_so =
                            mcc_nv_data.pref_voice_so.roam_orig_voice_so;
    }
    else if (nv_read_status == NV_NOTACTIVE_S)
    {
      MSG_HIGH("Nam:%d, PREF_VOICE_SO INACTIVE! Use default", nam, 0, 0);
    }
    else
    {
      read_ok = FALSE;
    }
#endif

#ifdef FEATURE_OTASP_OTAPA
    /* Read in OTAPA_ENABLED */
    mcc_nv_buf.item = NV_OTAPA_ENABLED_I;
    mcc_nv_data.otapa_enabled.nam = nam;
    nv_read_status = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_read_status == NV_DONE_S )
    {
      cdma.otapa_enabled = mcc_nv_data.otapa_enabled.enabled;
      MSG_MED("Nam:%d OTAPA_ENABLED: %d", nam, cdma.otapa_enabled, 0);
    }
    else if (nv_read_status == NV_NOTACTIVE_S)
    {
      MSG_HIGH( "Nam:%d, OTAPA_ENABLED INACTIVE! Set TRUE",nam,0,0);
      /* otapa is enabled by default - IS683A 3.2.2 */
    }
    else
    {
      ERR( "Nam:%d OTAPA_ENABLED NV read error", nam, 0, 0);
      read_ok = FALSE;
    }

    /* Read in NAM_LOCK */
    mcc_nv_buf.item = NV_NAM_LOCK_I;
    mcc_nv_data.nam_lock.nam = nam;
    nv_read_status = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_read_status == NV_DONE_S )
    {
      cdma.nam_lock = mcc_nv_data.nam_lock.enabled;
      MSG_MED("Nam:%d NAM_LOCK: %d", nam, cdma.nam_lock, 0);
    }
    else if (nv_read_status == NV_NOTACTIVE_S)
    {
      MSG_HIGH( "Nam:%d NAM_LOCK NOT ACTIVE! Set TRUE",nam,0,0);
      /* NAM_LOC is locked if not specified */
    }
    else
    {
      ERR( "Nam:%d NAM_LOCK NV read error", nam, 0, 0);
      read_ok = FALSE;
    }
#endif /* FEATURE_OTASP_OTAPA */

    #if defined(FEATURE_OTASP) && !defined(FEATURE_MEM_REDUCE_PRL)
    /* Read in PRL */

    /* Set data pointer to point at PRL buffer */
    mcc_nv_buf.data_ptr   = (nv_item_type*) &mcc_roaming_list;

    mcc_nv_buf.item       = NV_ROAMING_LIST_683_I;
    mcc_roaming_list.nam  = nam;
    nv_read_status        = mcc_nv_cmd( &mcc_nv_buf );

    /* If no PRL is stored in NV or the stored PRL is invalid, mark PRL as
    ** invalid and set PRL size to 0.
    */
    if( nv_read_status == NV_NOTACTIVE_S || !mcc_roaming_list.valid )
    {
      mcc_roaming_list.valid  = FALSE;
      mcc_roaming_list.size   = 0;
    }

    /* Set back data pointer to point at common NV data buffer */
    mcc_nv_buf.data_ptr   = &mcc_nv_data;
    #endif /* FEATURE_OTASP && !FEATURE_MEM_REDUCE_PRL */

  }

  /* Set IMSI_S by concatenating IMSI_S2 and IMSI_S1 into a qword */
  qw_set( cdma.imsi_s, (dword) ((cdma.imsi_s2 & 0x0300) >> 8),
          ((cdma.imsi_s1 & 0x00FFFFFFL) |
           (((dword) cdma.imsi_s2 & 0x000000FFL) << 24)) );

  /* ---------------------------------------------------------------
  ** For IS-95B mobile, IMSI_O will be loaded with IMSI_M at power-up
  ** as usual. IMSI type will be initialized to IMSI_M.
  ** --------------------------------------------------------------- */
  cdma.imsi_type = CDMA_IMSI_M;

  /* ---------------------------------------------------------------
  ** A copy of IMSI_M will be kept in mobile
  ** --------------------------------------------------------------- */
  cdma.imsi_m_s1       = cdma.imsi_s1;
  cdma.imsi_m_s2       = cdma.imsi_s2;
  cdma.imsi_m_mcc      = cdma.mcc;
  cdma.imsi_m_11_12    = cdma.imsi_11_12;

  cdma.imsi_m_addr_num = cdma.imsi_addr_num;

  qw_equ( cdma.imsi_m_s, cdma.imsi_s );

  /* Set IMSI_T_S by concatenating IMSI_T_S2 and IMSI_T_S1 into a qword */
  qw_set( cdma.imsi_t_s, (dword) ((cdma.imsi_t_s2 & 0x0300) >> 8),
          ((cdma.imsi_t_s1 & 0x00FFFFFFL) |
           (((dword) cdma.imsi_t_s2 & 0x000000FFL) << 24)) );
 
  cdma.tmsi_code_len =
                 tmsi_determine_code_size(cdma.assigning_tmsi.info.tmsi_code);

#ifdef FEATURE_NETWORK_PERSONALIZATION
  /* ---------------------------------------------------------------
  ** If Network Personalization is used, check the IMSI to
  ** assure that it is associated with a particular network.
  **
  ** Set read_ok to FALSE if any of the following:
  **   If the personalization table is unavailable
  **   If IMSI is not properly associated
  **
  ** --------------------------------------------------------------- */
  if (read_ok) 
  {
    /* It is sufficient to check only IMSI_T and IMSI_M,
     * IMSI_O is guranteed to be one of these two */

    /* IMSI_M
    */
    if ( ! mccnp_imsi_valid( cdma.imsi_m_mcc,
                             cdma.imsi_m_11_12,
                             cdma.imsi_m_s1,
                             cdma.imsi_m_s2 ) )
    {
      MSG_HIGH("Bad IMSI",0,0,0);
      read_ok = FALSE;
    }

    /* IMSI_T
    */
    if ( ! mccnp_imsi_valid( cdma.imsi_t_mcc,
                             cdma.imsi_t_11_12,
                             cdma.imsi_t_s1,
                             cdma.imsi_t_s2 ) )
    {
      MSG_HIGH("Bad T IMSI",0,0,0);
      read_ok = FALSE;
    }
  }
#endif /* FEATURE_NETWORK_PERSONALIZATION */


  return (read_ok);

} /* mcc_get_nam_info */

/*===========================================================================

FUNCTION MCC_POWERUP_LOAD

DESCRIPTION
  This function loads CDMA parameters from NV.

DEPENDENCIES
  None.

RETURN VALUE
  Indicator of whether all parameters were successfully read from NV.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_powerup_load 
( 
  void
)
{

  boolean read_ok; /* indicates if items were read from NV successfully */

#ifdef FEATURE_UIM_RUIM
  uim_cmd_type *uim_cmd_ptr;
    /* Pointer to command for UIM task */
  uim_rpt_type *uim_rpt_ptr;
    /* Pointer to report from UIM task */
  #ifdef FEATURE_PASSPORT_MODE
  /* UIM Card Color */
  mmgsdi_jcdma_card_info_enum_type uim_card_color;
  #endif /* FEATURE_PASSPORT_MODE */
#endif /* FEATURE_UIM_RUIM */

#if defined(FEATURE_OTASP) && !defined(FEATURE_NSOTASP)
  nv_stat_enum_type status; /* status returned by NV */

#ifdef FEATURE_UIM_RUIM
  byte i,j,ruim_num_features_enabled ;
#endif /* FEATURE_UIM_RUIM */

#endif /* FEATURE_OTASP && !FEATURE_NSOTASP */

  nv_stat_enum_type    nv_status_for_default; /* used for defaulting NV items */

#ifdef FEATURE_MEID_SUPPORT
  #ifndef FEATURE_UIM_RUIM
  dword pesn;         /* Pseudo-esn */
  #endif
  unsigned long meidhash[5];  /* MEID SHA-1 digest */
  qword meid;         /* MEID */
#endif /* FEATURE_MEID_SUPPORT */
  nv_stat_enum_type     read_status;        /* NV read status */
  
  /* Initialize the common fields of the command we will send to NV */
  mcc_nv_buf.cmd        = NV_READ_F;            /* We will do reads        */
  mcc_nv_buf.data_ptr   = &mcc_nv_data;         /* Initialize data pointer */

  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Write MC parameter from local variable to NV */
  {
    /* NV_SLOT_CYCLE_INDEX_I */
    /* NV_SLOT_CYCLE_INDEX_I */

    /* Write SCI item only if we know that the item is being stored into NV.
     * Item is stored in NV if any of the folliwing conditions is true:
     *    1) FEATURE_RUIM is not enabled
     *    2) FEATURE_UIM_RUIM_SUPPORT_SCI is not enabled
     *    3) If RTRE configuration is set to NV.
     *
     * In all other cases, SCI item gets stored into the card and we will not 
     * write the SCI.
     */

  #if ( !defined(FEATURE_RUIM) )\
    || ( defined(FEATURE_UIM_RUN_TIME_ENABLE) )  \
    || ( !defined(FEATURE_UIM_RUIM_SUPPORT_SCI) )

  #ifdef FEATURE_RUIM 
  #ifndef FEATURE_UIM_RUIM_SUPPORT_SCI
    MSG_MED("RUIM_SUPPORT_SCI not defined, writing into memory",0,0,0);
    {
  #elif defined ( FEATURE_UIM_RUN_TIME_ENABLE )
    mcc_nv_buf.item = NV_CURR_NAM_I;
    read_status = mcc_nv_cmd( &mcc_nv_buf ); 
    if ( read_status == NV_DONE_S )
    { 
      if (!nv_rtre_use_ruim_for_nam(mcc_nv_data.curr_nam))
  #endif
  #endif /*FEATURE_RUIM*/
      {
        mcc_nv_buf.item = NV_SLOT_CYCLE_INDEX_I;
        read_status = mcc_nv_cmd( &mcc_nv_buf );        
        if (read_status == NV_NOTACTIVE_S) 
        {
          mcc_nv_data.slot_cycle_index = (int8) slot_cycle_index;  
          mcc_nv_buf.cmd = NV_WRITE_F;
          mcc_nv_buf.item = NV_SLOT_CYCLE_INDEX_I;
          if (mcc_nv_cmd( &mcc_nv_buf ) != NV_DONE_S) 
          {
            ERR_FATAL("Failed write item to NV %d", mcc_nv_buf.item, 0, 0);
          }
          /* Put NV command back to READ */
          mcc_nv_buf.cmd = NV_READ_F;   
        }
        /* limit SCI to a max of REL_D_MAX_SCI and a min of REL_D_MIN_SCI
        */
        if (mcc_nv_data.slot_cycle_index > MCC_MAX_SCI) 
        {
          mcc_nv_data.slot_cycle_index = MCC_MAX_SCI;  
          mcc_nv_buf.cmd = NV_WRITE_F;
          mcc_nv_buf.item = NV_SLOT_CYCLE_INDEX_I;
          if (mcc_nv_cmd( &mcc_nv_buf ) != NV_DONE_S) 
          {
            ERR_FATAL("Failed write item to NV %d", mcc_nv_buf.item, 0, 0);
          }
          /* Put NV command back to READ */
          mcc_nv_buf.cmd = NV_READ_F;   
        } 
        if (mcc_nv_data.slot_cycle_index < MCC_MIN_SCI) 
        {
          mcc_nv_data.slot_cycle_index = MCC_MIN_SCI; 
          mcc_nv_buf.cmd = NV_WRITE_F;
          mcc_nv_buf.item = NV_SLOT_CYCLE_INDEX_I;
          if (mcc_nv_cmd( &mcc_nv_buf ) != NV_DONE_S) 
          {
            ERR_FATAL("Failed write item to NV %d", mcc_nv_buf.item, 0, 0);
          }
          /* Put NV command back to READ */
          mcc_nv_buf.cmd = NV_READ_F;   
        }
      }
  #ifdef FEATURE_RUIM 
  #if  ( !defined (FEATURE_UIM_RUIM_SUPPORT_SCI) ) \
        || ( defined (FEATURE_UIM_RUN_TIME_ENABLE) )
    } 
    /* terminating brace for (!defined FEATURE_UIM_RUIM_SUPPORT_SCI) as well! */
  #endif 
  #endif  /*FEATURE_RUIM*/

  #endif  /* ( !defined( FEATURE_RUIM ) )\
              || ( defined( FEATURE_UIM_RUN_TIME_ENABLE ) )  \
              || ( ! defined( FEATURE_UIM_RUIM_SUPPORT_SCI ) ) */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* NV_NEG_SLOT_CYCLE_INDEX_I */
    #ifdef FEATURE_MODIFIED_SCI
#error code not present
    #endif /* FEATURE_MODIFIED_SCI */
  }
  
  /* We will do the powerup initialiazation. This will be executed */
  /* during powerup, NAM change, and upon exit from LPM            */
  mccreg_powerup();

  /* Reset the Paging Channel Layer 2 acknowledgement fields */
  mcc_layer2.msg_seq  = 7;
  mcc_layer2.ack_seq  = 7;  /* Binary '111' */
  mcc_layer2.ack_type = 0;
  mcc_layer2.idle_msg_pending = FALSE;

  cdma.t_slotted = 0;   /* Initialize the Slotted timer */

  /* Initialize rest of TMSI information before */
  /* overhead messages are received.            */

  /* Reset full TMSI timer indicator */
  cdma.tmsi_full_timer_set = FALSE;

  read_ok = TRUE;  /* assume everything is going to go ok */

  /* Initialize the cdma parameters */

#ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA */

  cdma.esn = 0;
  cdma.curr_nam = 0;
  cdma.scm = 0;

#ifdef FEATURE_UIM_RUIM
 /* Initialize CDMA parameters */
  cdma.esn_me = 0;
#if defined(FEATURE_MEID_SUPPORT) && defined(FEATURE_HWID_SUPPORT)
  qw_set(cdma.meid_me, 0L, 0L);   /* initialize meid to a value zero */
#endif /* FEATURE_MEID_SUPPORT && FEATURE_HWID_SUPPORT */
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_OTASP
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  (void) memset (cdma.spc, '0', NV_SEC_CODE_SIZE);
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_OTASP */

  cdma.slot_cycle_index_p = 0;

  (void)mcc_set_current_preferred_sci(cdma.slot_cycle_index_p); 
  
  #ifdef FEATURE_MODIFIED_SCI
#error code not present
  #endif /* FEATURE_MODIFIED_SCI */

  cdma.mob_cai_rev = 0;
  cdma.mob_firm_rev = 0;
  cdma.mob_model = 0;

  #if defined(FEATURE_OTASP) && !defined(FEATURE_NSOTASP)
#ifdef FEATURE_VERSION_W208S
  cdma.spc_change_enabled = TRUE;
#else
  cdma.spc_change_enabled = FALSE;
#endif
  #endif /* (FEATURE_OTASP) && !(FEATURE_NSOTASP) */

  #ifdef FEATURE_PASSPORT_MODE
  /* Initialize the mask to NONE */
  cdma_block_action_mask = CDMA_MSG_TYPE_NONE;
  #endif

  /* Flag to indicate if any of the Time indicating values changed in SCHM
   * at power-up, we always flush the value in NV */

  mcc_is_schm_time_param_chng = TRUE;  

  /* Reset the common fields of the command we will send to NV */
  mcc_nv_buf.cmd        = NV_READ_F;            /* We will do reads        */
  
  /* Read ESN */
  mcc_nv_buf.item = NV_ESN_I;
  if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
  {
    cdma.esn = mcc_nv_data.esn.esn;
  }
  else
  {
    ERR("Bad status from NV or RUIM", 0, 0, 0);
    read_ok = FALSE;
  }
  #ifdef FEATURE_UIM_RUIM
  #if (defined(FEATURE_IS2000_REL_C) || defined(FEATURE_HWID_SUPPORT))
  if (read_ok)
  {
    /* Read ESN ME */
    mcc_nv_buf.item = NV_ESN_ME_I;
    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      cdma.esn_me = mcc_nv_data.esn.esn;
      if ( (cdma.esn == 0) && (cdma.esn_me != 0) )
      {
        /* Set ESN to ESN_ME so that emergency call can be made with non-zero ESN */
        MSG_MED("Setting ESN equal to ESN_ME", 0, 0, 0);
        cdma.esn = cdma.esn_me;
      }
    }
    else
    {
      ERR("Bad status from NV for ESN ME", 0, 0, 0);
    }
  }
  #endif /* FEATURE_IS2000_REL_C || FEATURE_HWID_SUPPORT */
  #endif /* FEATURE_UIM_RUIM */

  /* Set esn for deriving default values of some NV items that are accessed from
     the RUIM */ 
  mc_set_esn_for_default(cdma.esn);

  /* Read in SCM */
  mcc_nv_buf.item = NV_SCM_I;
  if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
  {
    cdma.scm = mcc_nv_data.scm;
  }
  else
  {
    read_ok = FALSE;
  }

#ifdef FEATURE_MEID_SUPPORT
  if (read_ok) 
  {
    /* Read the MEID */
    mcc_nv_buf.item = NV_MEID_I;
    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {

      /* Check the validity of MEID */
      if (qw_hi(mcc_nv_data.meid) != 0x0UL || qw_lo(mcc_nv_data.meid) != 0x0UL) 
      {
        /* put MEID into a local variable */
        qw_equ(meid, mcc_nv_data.meid);

        if (mcc_meid_create_sha1_digest(meid, meidhash))
        {
          #ifndef FEATURE_UIM_RUIM
          /* The hash computation was successful */
          /* compute the Pseudo-ESN */                
          pesn = (meidhash[4] & ~MCC_PESN_PREFIX_MASK) | (MCC_PESN_PREFIX);

          if (cdma.esn != pesn) 
          {
            /* The ESN mismatch, go offline */
            MSG_ERROR("Mismatched MEID 0x%1x-0x%1x ESN 0x%1x", qw_hi(mcc_nv_data.meid),
                      qw_lo(mcc_nv_data.meid), cdma.esn);

            read_ok = FALSE;      
          }
          else
          #endif /* FEATURE_UIM_RUIM */
          {
            /* meid and esn are consistent */
            /* store the MEID */
            qw_equ(cdma.meid, mcc_nv_data.meid);

            /* store the MEID hash */
            cdma.meidhash[0] = meidhash[0];
            cdma.meidhash[1] = meidhash[1];
            cdma.meidhash[2] = meidhash[2];
            cdma.meidhash[3] = meidhash[3];
            cdma.meidhash[4] = meidhash[4];
          }
        }
        else
        {
          read_ok = FALSE;      

          /* MEID hash computation failed */
          ERR_FATAL("MEID hash can not be created, phone is non-operative",0,0,0);
        }
      }

      if (read_ok) 
      {
        /* set the SCM bit 4 */
        cdma.scm = cdma.scm | CAI_SCM_MEID_CAPABLE_MASK;
        mcc_nv_data.scm = cdma.scm;
        mcc_nv_buf.cmd = NV_WRITE_F;
        mcc_nv_buf.item = NV_SCM_I;
        if (mcc_nv_cmd( &mcc_nv_buf ) != NV_DONE_S) 
        {
          ERR_FATAL("Failed write item to NV %d", mcc_nv_buf.item, 0, 0);
        }

        /* Put NV command back to READ */
        mcc_nv_buf.cmd = NV_READ_F; 
        #if (defined(FEATURE_UIM_RUIM) && defined(FEATURE_HWID_SUPPORT))
        /* Read the MEID ME */
        mcc_nv_buf.item = NV_MEID_ME_I;
        if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
        {
          /* Check the validity of MEID */
          if (qw_hi(mcc_nv_data.meid) != 0x0UL || qw_lo(mcc_nv_data.meid) != 0x0UL) 
          {
            /* put MEID into a CDMA variable */
            qw_equ(cdma.meid_me, mcc_nv_data.meid);
            if ( (cdma.meid == 0) && (cdma.meid_me !=0 ) )
            {
              /* Set MEID to MEID_ME so that emergency call can be made 
               * with non-zero MEID */
              MSG_MED("Setting MEID equal to MEID_ME", 0, 0, 0);
              qw_equ(cdma.meid, cdma.meid_me);
            }
          }
          else
          {
            MSG_MED("MEID ME not valid",0,0,0);
          }
        }
        else
        {
          MSG_MED("Failed to read item MEID_ME from NV",0,0,0);
          read_ok = FALSE;
        }
        #endif /* FEATURE_UIM_RUIM && FEATURE_HWID_SUPPORT */
      }
    }
    else
    {
      /* MEID was not provisioned or bad read */
      qw_set(cdma.meid, 0L, 0L);   /* initialize meid to a value zero */

      /* clear the SCM bit 4 */
      cdma.scm = cdma.scm & (~(byte)(CAI_SCM_MEID_CAPABLE_MASK));
      mcc_nv_data.scm = cdma.scm;
      mcc_nv_buf.cmd = NV_WRITE_F;
      mcc_nv_buf.item = NV_SCM_I;
      if (mcc_nv_cmd( &mcc_nv_buf ) != NV_DONE_S) 
      {
        ERR_FATAL("Failed write item to NV %d", mcc_nv_buf.item, 0, 0);
      }

      /* Put NV command back to READ */
      mcc_nv_buf.cmd = NV_READ_F;   
      
    }

  }
#endif /* FEATURE_MEID_SUPPORT */
  
  /* Read nam */
  mcc_nv_data.curr_nam = (byte) cm_get_curr_nam();

  if (mcc_nv_data.curr_nam < NV_MAX_NAMS)
  {
    cdma.curr_nam = mcc_nv_data.curr_nam;
  }
  else
  {
    /* -------------------------------------------
    ** If not active, UI writes a default 0 to NV,
    ** so we should never come here.
    ** ------------------------------------------- */
    read_ok = FALSE;
  }

  /* Read in NAM specific informaiton */
  if (!mcc_get_nam_info( cdma.curr_nam ))
  {
    read_ok = FALSE;
  }

  /* ------------------------------------------------------------
  ** The rest of these items are written explicitly into NV by MC
  ** prior to coming here, so if they are still 'not active', we
  ** have a SERIOUS problem and should go offline.
  ** ------------------------------------------------------------ */

#ifdef FEATURE_UIM_RUIM
  /* Initialize CDMA parameters */

  cdma.ruim_id_avail = FALSE;
  cdma.ruim_id = 0;


  if (!cdma.is_cdma_subs_avail)
  {
      MSG_MED("Subscription not available. UIM ID can not be read", 0, 0,0);
  }
  else
  {
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      /* Read UIM_ID */
      if ((uim_cmd_ptr = (uim_cmd_type*) q_get(&uim_free_q)) != NULL)
      {
        uim_cmd_ptr->access_uim.access = UIM_READ_F;
        uim_cmd_ptr->access_uim.item = UIM_CDMA_RUIM_ID;
        #if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )
#error code not present
        #endif /*#if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )*/
        uim_cmd_ptr->access_uim.offset = 1;
        uim_cmd_ptr->access_uim.num_bytes = (word) sizeof(((nv_item_type *) 0)->esn);
        uim_cmd_ptr->access_uim.data_ptr = (byte *) &uim_id_cache_buf;
        uim_cmd_ptr->hdr.command = UIM_ACCESS_F;
        uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

        mcc_uim_cmd(uim_cmd_ptr);

        /* ----------------------------------
        ** Wait for MCC_UIM_Q_SIG to be set.
        ** ---------------------------------- */
        MSG_MED("Waiting for UIM report on UIM_ID", 0, 0, 0);

        (void) mcc_wait(MCC_UIM_Q_SIG);

        uim_rpt_ptr = (uim_rpt_type*) q_get(&mc_uim_q);

        if (uim_cmd_ptr->hdr.status == UIM_RECEIVED_S)
        {
          if ((uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
              (uim_rpt_ptr->rpt_status == UIM_PASS))
          {
            cdma.ruim_id_avail = TRUE;
            cdma.ruim_id = uim_id_cache_buf.esn;
          }
          else
          {
            MSG_MED("Read Failed for UIM_ID ", 0, 0, 0);
          }
        }
        else
        {
          ERR("Bad status from UIM %d", uim_cmd_ptr->hdr.status, 0, 0);
        }

        /* Put the buffer back in the queue */
        if (uim_rpt_ptr != NULL)
        {
          cmd_done(&uim_rpt_ptr->rpt_hdr);
        }
      }
      else
      {
          ERR("No free buffers on uim_free_q", 0, 0, 0);
      }
    }
  }
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_OTASP

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

  {
    if (!cdma.is_cdma_subs_avail)
    {
      MSG_MED("Subscription not available. SPC_STATUS is 0", 0, 0,0);
      cdma.spc_status = 0;
    }

    /* With RUIM we read SPC status instead of the SPC */
    else if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
    {
        uim_cmd_ptr->access_uim.access = UIM_READ_F;
        uim_cmd_ptr->access_uim.item = UIM_CDMA_SPC_STATUS;
        #if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )
#error code not present
        #endif /*#if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )*/
        uim_cmd_ptr->access_uim.offset = 0;
        uim_cmd_ptr->access_uim.num_bytes = 1;
        uim_cmd_ptr->access_uim.data_ptr = uim_access_buffer;
        uim_cmd_ptr->hdr.command = UIM_ACCESS_F;
        uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

        mcc_uim_cmd( uim_cmd_ptr );

        /* ----------------------------------
        ** Wait for MCC_UIM_Q_SIG to be set.
        ** ---------------------------------- */
        MSG_MED( "Waiting for UIM report", 0, 0, 0 );
        (void) mcc_wait( MCC_UIM_Q_SIG );
        uim_rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_q );

        if ( uim_cmd_ptr->hdr.status == UIM_RECEIVED_S )
        {
           if ((uim_rpt_ptr != NULL) &&
               (uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
               (uim_rpt_ptr->rpt_status == UIM_PASS))
           {
               cdma.spc_status = uim_access_buffer[0] & UIM_SPC_STATUS_MASK;
           }
           else
           {
               MSG_MED( "Read Failed for SPC Status ", 0, 0, 0 );
               cdma.spc_status = 0;
           }
        }
        else
        {
           ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status, 0, 0 );
           cdma.spc_status = 0;
        }
        
        /* Put the buffer back in the queue */
        if (uim_rpt_ptr != NULL)
        {
           cmd_done( &uim_rpt_ptr->rpt_hdr );
        }
     }
     else
     {
        ERR( "No free buffers on uim_free_q", 0, 0, 0 );
        cdma.spc_status = 0;
     }

  }
#endif /* FEATURE_UIM_RUIM */

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    /* Read Service Programming Code */
    mcc_nv_buf.item = NV_SEC_CODE_I;
    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      (void) memcpy (cdma.spc, (byte*) mcc_nv_data.sec_code.digits, NV_SEC_CODE_SIZE);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_UIM_RUIM

#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

  {
    if (!cdma.is_cdma_subs_avail)
    {
      MSG_MED("Subscription not available. No OTA Features.", 0, 0,0);
      cdma.ruim_ota_features.num_features = 0;
    }


    /* Read the OTASP/OTAPA Features from R-UIM directly */

    /* First, read the number of features supported by the card */
    else if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
    {
        uim_cmd_ptr->access_uim.access = UIM_READ_F;
        uim_cmd_ptr->access_uim.item = UIM_CDMA_OTASP_OTAPA_FEATURES;
        #if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )
#error code not present
        #endif /*#if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )*/
        uim_cmd_ptr->access_uim.offset = 0;
        uim_cmd_ptr->access_uim.num_bytes = 1;
        uim_cmd_ptr->access_uim.data_ptr = uim_access_buffer;
        uim_cmd_ptr->hdr.command = UIM_ACCESS_F;
        uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

        mcc_uim_cmd( uim_cmd_ptr );

        /* ----------------------------------
        ** Wait for MCC_UIM_Q_SIG to be set.
        ** ---------------------------------- */
        MSG_MED( "Waiting for UIM report", 0, 0, 0 );
        (void) mcc_wait( MCC_UIM_Q_SIG );
        uim_rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_q );

        if ( uim_cmd_ptr->hdr.status == UIM_RECEIVED_S )
        {
           if ((uim_rpt_ptr != NULL) &&
               (uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
               (uim_rpt_ptr->rpt_status == UIM_PASS))
           {
               cdma.ruim_ota_features.num_features = uim_access_buffer[0];
           }
           else
           {
               MSG_MED( "Read Failed for OTA Features ", 0, 0, 0 );
               cdma.ruim_ota_features.num_features = 0;

               /* !!!!!! DEFAULTING ROUTINE WILL BE PROVIDED LATER !!!!!!*/
           }
        }
        else
        {
           ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status, 0, 0 );
           cdma.ruim_ota_features.num_features = 0;
        }

        /* Put the buffer back in the queue */
        if (uim_rpt_ptr != NULL)
        {
           cmd_done( &uim_rpt_ptr->rpt_hdr );
        }

        #ifdef FEATURE_PASSPORT_MODE
        if(mcc_passport_mode_is_jcdma())
        {          
          /* Read the card color */
          uim_card_color = mmgsdi_jcdma_card_get_info();

          if(uim_card_color == MMGSDI_JCDMA_GRAY_CARD)        
          {
            /* If card color is gray we block registration and any incoming page */
            cdma_block_action_mask = CDMA_MSG_TYPE_REG | CDMA_MSG_TYPE_PAGE;
          }
          else if(uim_card_color == MMGSDI_JCDMA_BLACK_CARD)
          {
            /* If card color is black we block nothing */
            cdma_block_action_mask = CDMA_MSG_TYPE_NONE;
          }
          else
          {
            MSG_ERROR("Invalid card color %d", uim_card_color, 0, 0);          
          }
        }
        #endif /* FEATURE_PASSPORT_MODE */
     }
     else
     {
        ERR( "No free buffers on uim_free_q", 0, 0, 0 );
        cdma.ruim_ota_features.num_features = 0;
     }

    /* Read the feature list */
    if (cdma.ruim_ota_features.num_features != 0)
    {
       MSG_MED("Total number of features supported by card=%d",
                cdma.ruim_ota_features.num_features,0,0);

       if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
       {
          uim_cmd_ptr->access_uim.access = UIM_READ_F;
          uim_cmd_ptr->access_uim.item = UIM_CDMA_OTASP_OTAPA_FEATURES;
          #if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )
#error code not present
          #endif /*#if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )*/
          uim_cmd_ptr->access_uim.offset = 1;
          uim_cmd_ptr->access_uim.num_bytes =
                        cdma.ruim_ota_features.num_features * 2;
          uim_cmd_ptr->access_uim.data_ptr = uim_access_buffer;
          uim_cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F;
          uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

          mcc_uim_cmd( uim_cmd_ptr );

          /* ----------------------------------
          ** Wait for MCC_UIM_Q_SIG to be set.
          ** ---------------------------------- */
          MSG_MED( "Waiting for UIM report", 0, 0, 0 );
          (void) mcc_wait( MCC_UIM_Q_SIG );
          uim_rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_q );

          if ( uim_cmd_ptr->hdr.status == UIM_RECEIVED_S )
          {
             if ((uim_rpt_ptr != NULL) &&
                 (uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
                 (uim_rpt_ptr->rpt_status == UIM_PASS))
             {
                /* parse the data into the cdma structure */
                /* instead of blindly reading OTASPI_FEATURE_MAX features, 
                now we will read the OTASPI_FEATURE_MAX ENABLED features */
                ruim_num_features_enabled = 0;
                for (i = 0, j = 0 ; 
                     i < cdma.ruim_ota_features.num_features && 
                      ruim_num_features_enabled < OTASPI_FEATURE_MAX;
                     i++)
                {
                  switch ( uim_access_buffer[j] )
                  {
                    case OTASP_NAM_DOWNLOAD:
                    case OTASP_PROG_LOCK:
                    case OTASP_PREF_ROAM:
                    case OTASP_KEY_EXCHANGE:
                    case OTASP_OTAPA:
                    {
                      cdma.ruim_ota_features.feature_id[ruim_num_features_enabled] =
                                                   uim_access_buffer[j++];
                      cdma.ruim_ota_features.feature_p_rev[ruim_num_features_enabled] =
                                                   uim_access_buffer[j++];
                       ruim_num_features_enabled++;
                       break;
                    }
                    default:
                    j+=2; // read the next item from buffer
                    break;
                  }
                }
                MSG_MED("Total number of enabled features = %d",
                         ruim_num_features_enabled,0,0);
                cdma.ruim_ota_features.num_features = ruim_num_features_enabled;           
             }
             else
             {
                 MSG_MED( "Read Failed for OTA Features ", 0, 0, 0 );
                 cdma.ruim_ota_features.num_features = 0;
             }
          }
          else
          {
              ERR( "Bad status from UIM %d", uim_cmd_ptr->hdr.status, 0, 0 );
              cdma.ruim_ota_features.num_features = 0;
          }

          /* Put the buffer back in the queue */
          if (uim_rpt_ptr != NULL)
          {
             cmd_done( &uim_rpt_ptr->rpt_hdr );
          }
       }
       else
       {
          ERR( "No free buffers on uim_free_q", 0, 0, 0 );
          cdma.ruim_ota_features.num_features = 0;
       }
    }
  }
#endif /* FEATURE_UIM_RUIM */

#endif /* FEATURE_OTASP */
  
  /* Read in SLOT_CYCLE_INDEX */
  mcc_nv_buf.item = NV_SLOT_CYCLE_INDEX_I;

  if (!cdma.is_cdma_subs_avail) 
  {
    MSG_MED("Subscription not available. No OTA Features.", 0, 0,0);
    /* Use the default value of this item. */
    mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
    cdma.slot_cycle_index_p = mcc_nv_data.slot_cycle_index; /*lint !e732*/
  }
  else
  {
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.slot_cycle_index_p = mcc_nv_data.slot_cycle_index; /*lint !e732*/
    }

#if (defined( FEATURE_NV_RUIM ) && defined( FEATURE_UIM_RUIM_SUPPORT_SCI))
    else if ((nv_status_for_default == NV_NOTACTIVE_S)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.slot_cycle_index_p = mcc_nv_data.slot_cycle_index;
    }
#endif /* FEATURE_NV_RUIM && FEATURE_UIM_RUIM_SUPPORT_SCI */

    else
    {
      ERR( "SCI was NOT available!",0,0,0);
      read_ok = FALSE;
    }
  }

  /* Make the current preferred SCI as SCIp obtained from NV */
  (void)mcc_set_current_preferred_sci(cdma.slot_cycle_index_p);  


  #ifdef FEATURE_MODIFIED_SCI
#error code not present
  #endif /* FEATURE_MODIFIED_SCI */

  /* read in MOB_P_REV */
  mcc_nv_buf.item = NV_MOB_CAI_REV_I;
  if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
  {
    cdma.mob_cai_rev = mcc_nv_data.mob_cai_rev;

    // Initialize BS_P_REV to same as mob_p_rev.  This may change when the
    // real base station comes
    mcc_update_bs_p_rev( cdma.mob_cai_rev );
  }
  else
  {
    read_ok = FALSE;
  }
  
  /* read in MOB_FIRM_REV */
  mcc_nv_buf.item = NV_MOB_FIRM_REV_I;
  if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
  {
    cdma.mob_firm_rev = mcc_nv_data.mob_firm_rev;
  }
  else
  {
    read_ok = FALSE;
  }

  /* read in MOB_MODEL */
  mcc_nv_buf.item = NV_MOB_MODEL_I;
  if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
  {
    cdma.mob_model = mcc_nv_data.mob_model;
  }
  else
  {
    read_ok = FALSE;
  }

#if defined(FEATURE_OTASP) && !defined(FEATURE_NSOTASP)
  if (cdma.is_cdma_subs_avail) 
  {
    mcc_nv_buf.item = NV_SPC_CHANGE_ENABLED_I;
    status = mcc_nv_cmd( &mcc_nv_buf ) ;
    if ( status == NV_DONE_S)
    {
      cdma.spc_change_enabled = mcc_nv_data.spc_change_enabled;
    }
    else if (status == NV_NOTACTIVE_S)
    {
      /* default to deny SPC change from default value to a non-default value
      ** as specified in IS-683A */
#ifdef FEATURE_VERSION_W208S
	  cdma.spc_change_enabled = TRUE;
#else
      cdma.spc_change_enabled = FALSE;
#endif            
    }
    else
    {
      read_ok = FALSE;
    }
  }
#endif

#ifdef FEATURE_IS2000_DYNAMIC_FEATURE
  /* Read in and verify the Dynamic Feature item. */
  if (!mccap_df_load())
  {
    read_ok = FALSE;
  }
#endif /* FEATURE_IS2000_DYNAMIC_FEATURE */

  /* Read whether 4GV_NB is enabled */
  mcc_nv_buf.item = NV_CDMA_SO68_ENABLED_I;
  status = mcc_nv_cmd( &mcc_nv_buf );

  if( status == NV_DONE_S)
  {
    if(!mcc_nv_data.cdma_so68_enabled)
    {
      (void)snm_disable_so( CAI_SO_VOICE_4GV_NB );
    }
  }
  else if(status == NV_NOTACTIVE_S)
  {
    (void)snm_disable_so( CAI_SO_VOICE_4GV_NB );
  }
  else 
  {
    read_ok = FALSE;
  }

  /* Read whether 4GV_WB is enabled */
  mcc_nv_buf.item = NV_CDMA_SO70_ENABLED_I;
  status = mcc_nv_cmd( &mcc_nv_buf );

  if( status == NV_DONE_S)
  {
    if(!mcc_nv_data.cdma_so70_enabled)
    {
      (void)snm_disable_so( CAI_SO_VOICE_4GV_WB );
    }
  }
  else if(status == NV_NOTACTIVE_S)
  {
    (void)snm_disable_so( CAI_SO_VOICE_4GV_WB );
  }
  else 
  {
    read_ok = FALSE;
  }

  #if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
  #endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */


  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */

  if( !read_ok )
  {
    MSG_HIGH( "Bad MCC powerup load", 0, 0, 0 );
  }

  #ifdef T_QUASAR   /* fake it for now until we figure out the problem with Quasar */
#error code not present
  #endif

  return( read_ok );

} /* mcc_powerup_load */

/*===========================================================================

FUNCTION MCC_POWERUP_INIT

DESCRIPTION
  This function defines timers for the Main Control task and initializes
  items on the mcc_rxtx_free_q.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if parameters were successfully loaded from NV.  FALSE otherwise.

SIDE EFFECTS
  mcc_rxtx_free_q and timers are ready to be used.

===========================================================================*/

boolean mcc_powerup_init
( 
  void 
)
{
  word i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize timers */
  rex_def_timer( &mcc_rpt_timer, &mc_tcb, MCC_RPT_TIMER_SIG );
  rex_def_timer( &mcc_state_timer, &mc_tcb, MCC_STATE_TIMER_SIG );
  rex_def_timer_ex( &mcc_bs_insanity_timer, mcc_expired_timer_callback_func, MCC_BS_INSANITY_TIMER_ID );
  rex_def_timer_ex( &mcc_action_timer, mcc_expired_timer_callback_func, MCC_ACTION_TIMER_ID );
  rex_def_timer( &mcc_cmd_timer,  &mc_tcb, MCC_CMD_TIMER_SIG );
  rex_def_timer_ex( &mcc_so_ctl_timer, mcc_expired_timer_callback_func, MCC_SO_CTL_TIMER_ID );
  rex_def_timer_ex( &mcc_srv_timer, mcc_expired_timer_callback_func, MCC_SRV_TIMER_ID );
  rex_def_timer_ex( &mcc_full_tmsi_timer, mcc_expired_timer_callback_func, MCC_FULL_TMSI_TIMER_ID );
  rex_def_timer_ex( &mcc_tmsi_exp_timer, mcc_expired_timer_callback_func, MCC_TMSI_EXP_TIMER_ID );

  #ifdef FEATURE_IS95B_MDR
  rex_def_timer( &mcc_scam_start_timer, &mc_tcb, MCC_SCAM_TIMER_SIG );
  #endif /* FEATURE_IS95B_MDR */

  rex_def_timer( &mcc_slotted_timer, &mc_tcb, MCC_SLOTTED_TIMER_SIG );

  #if defined(FEATURE_IS95B_MDR) 
  rex_def_timer( &mcc_mdr_duration_timer, &mc_tcb, MCC_MDR_DURATION_TIMER_SIG );
  #endif /* defined(FEATURE_IS95B_MDR) */

  #ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
  #endif /* FEATURE_GPSONE_NEW_L2_ACK */

  #ifdef FEATURE_SO_8026
#error code not present
  #endif /* FEATURE_SO_8026 */

  rex_def_timer_ex( &mcc_t50_timer, mcc_expired_timer_callback_func, MCC_T50_TIMER_ID );
  rex_def_timer_ex( &mcc_t51_timer, mcc_expired_timer_callback_func, MCC_T51_TIMER_ID );

  #ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
  #endif /* FEATURE_IS2000_REL_C_AKA */

  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #endif /* FEATURE_IS2000_REL_A_AES */

/* set tranceiver request time out callback function to be mcc_expired_timer_callback_func. */
  rex_def_timer_ex(&mcc_tr_request_timer, mcc_expired_timer_callback_func, MCC_TR_REQUEST_TIMER_ID);

  /* Initialize Call Control Instance Timers */
  mccccim_initialize_timers();

  #ifdef FEATURE_IS2000_REL_A_SVD
  /* Initialize Tag Manager Timers */
  mcctctag_initialize_timers();
  #endif /* FEATURE_IS2000_REL_A_SVD */

  /* Initialize queues */
  mccrxtx_init();
  mccsrch_init();
  mccrx_init();

  /* Set up queues for continous dtmf processing */
  ( void )q_init(&mcc_dtmf_free_q);
  ( void )q_init(&mcc_dtmf_q);
  for(i=0; i< MCC_MAX_DTMFS_OUT; i++ )
  {
    q_put( &mcc_dtmf_free_q,  q_link( &cdma_dtmf_bufs[i],
                              &cdma_dtmf_bufs[i].link ) );
  }

  #ifdef FEATURE_GPSONE
#error code not present
  #endif /* FEATURE_GPSONE */

  #ifdef FEATURE_BROADCAST_SMS
  /* Initialize the Broadcast SMS services */
  mccbcsms_init();
  #endif /* FEATURE_BROADCAST_SMS */

  #ifdef FEATURE_FULL_TIME_SHDR_3
  /* Initialize the DDTM settings */
  mcc_ddtm_init();
  #endif /* FEATURE_FULL_TIME_SHDR_3 */

  /* Initialize the MC-Diag Subsystem Dispatch Command Handler. */
  mc_diag_init();

  /*
  ** If MultiMode feature is defined, mcc_powerup_load function will
  ** not be called during power up initialization. It will be called
  ** after phone receivd phone status change command,online,from call
  ** manager.
  */

  return TRUE;

} /* mcc_powerup_init */

/*===========================================================================

FUNCTION EXIT_PARMS_OUT

DESCRIPTION
  This procedure outputs the CDMA exit parameters with HIGH level messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void exit_parms_out()
{
  uint8 payload_type;

  switch (mcc_exit.exit)
  {
    case MCC_RESELECTION:
      MSG_HIGH( "RESELECTION EXIT", 0, 0, 0 );
      payload_type = 0;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACQ_FAILED:
      MSG_HIGH( "ACQ FAILED EXIT", 0, 0, 0 );
      payload_type = 1;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    #ifdef FEATURE_GPSONE_ACQ_MODE
#error code not present
    #endif /* FEATURE_GPSONE_ACQ_MODE */


    case MCC_RELEASE:
      MSG_HIGH( "RELEASE EXIT", 0, 0, 0 );
      payload_type = 4;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACCESS_FAILED_SYS_LOST:
    case MCC_SYSTEM_LOST:
      MSG_HIGH( "SYSTEM LOST EXIT ", 0, 0, 0 );
      payload_type = 5;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_NEW_SYSTEM:
      MSG_HIGH( "NEW SYSTEM EXIT ", 0, 0, 0 );
      payload_type = 6;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_MAX_ACCESS_PROBES:
      MSG_HIGH( "MAX ACCESS PROBES EXIT ", 0, 0, 0 );
      payload_type = 7;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_RESCAN:
      MSG_HIGH( "RESCAN EXIT ", 0, 0, 0 );
      payload_type = 8;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACCESS_FAILED_GSRM:
    case MCC_REDIRECTION:
      MSG_HIGH( "REDIRECTION EXIT ", 0, 0, 0 );
      payload_type = 9;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_REG_REJECTED:
      MSG_HIGH( "REGISTRATION REJECTED EXIT ", 0, 0, 0 );
      payload_type = 10;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACCESS_DENIED:
      MSG_HIGH( "ACCESS DENIED EXIT ", 0, 0, 0 );
      payload_type = 11;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    #ifdef FEATURE_ACP
#error code not present
    #endif /* FEATURE_ACP */

    case MCC_LOCK:
    {
      MSG_HIGH( "LOCK EXIT ", 0, 0, 0 );
      payload_type = 19;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_UNLOCK:
    {
      MSG_HIGH( "UNLOCK EXIT", 0, 0, 0 );
      payload_type = 20;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_INCOMP_CAI_REV:
    {
      MSG_HIGH( "INCOMPATIBLE CAI REV EXIT", 0, 0, 0 );
      payload_type = 21;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_PREF_SYS_CHANGED:
    {
      MSG_HIGH( "PREF SYS CHANGED EXIT", 0, 0, 0 );
      payload_type = 22;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_NAM_CHANGED:
    {
      MSG_HIGH( "NAM CHANGED EXIT", 0, 0, 0 );
      payload_type = 23;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_OFFLINE_ANALOG:
    {
      MSG_HIGH( "OFFLINE ANALOG EXIT", 0, 0, 0 );
      payload_type = 24;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_OFFLINE_DIGITAL:
    {
      MSG_HIGH( "OFFLINE DIGITAL EXIT", 0, 0, 0 );
      payload_type = 25;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_RESET:
    {
      MSG_HIGH( "RESET EXIT", 0, 0, 0 );
      payload_type = 26;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_POWERDOWN:
    {
      MSG_HIGH( "POWERDOWN EXIT", 0, 0, 0 );
      payload_type = 27;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }


    case MCC_NOT_PREFERRED:
      MSG_HIGH( "NOT PREFERRED EXIT", 0, 0, 0 );
      payload_type = 30;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_LPM:
      MSG_HIGH( "LPM EXIT", 0, 0, 0 );
      payload_type = 33;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    
    case MCC_SYSTEM_RESELECTION:          /* Pilot power below threshold, exit to SD */
      MSG_HIGH ( "Pilot power below threshold!", 0, 0, 0 );
      payload_type = 34;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_NDSS_OFF:          /* NDSS off exit to SD */
      MSG_HIGH ( "NDSS OFF EXIT", 0, 0, 0 );
      payload_type = 35;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;


    case MCC_ACCESS_COMPLETED:
      MSG_HIGH("ACCESS COMPLETED EXIT", 0, 0, 0);
      payload_type = 37;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACCESS_FAILED:
    case MCC_ACCESS_TIMEOUT:
      MSG_HIGH("ACCESS FAILED EXIT", 0, 0, 0);
      payload_type = 38;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_INTERCEPT:
      MSG_HIGH("INTERCEPT EXIT", 0, 0, 0);
      payload_type = 39;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_REORDER:
      MSG_HIGH("REORDER EXIT", 0, 0, 0);
      payload_type = 40;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    #ifdef FEATURE_IS2000_REL_A
    case MCC_ENCRYPTION_MI_FAILED:
      MSG_HIGH("ENCRYPTION / Message Integrity FAILED", 0, 0, 0);
      payload_type = 41;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACCT_BLOCKED:
      MSG_HIGH("ACCT BLOCKED", 0, 0, 0);
      payload_type = 42;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    #endif /* FEATURE_IS2000_REL_A */

    case MCC_SRCH_REQUEST_SYS_RESTART:
      MSG_HIGH("SRCH REQUEST SYS RESTART", 0, 0, 0);
      payload_type = 43;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    #if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
    #endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */

    #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
    #endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

    case MCC_SHUTDOWN:
      MSG_HIGH("SHUTDOWN EXIT", 0, 0, 0);
      payload_type = 46;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    default:
      MSG_HIGH("Invalid exit reason %d", mcc_exit.exit, 0, 0 );
      break;

  } /* switch */

}/* exit_parms_out */

/*===========================================================================

FUNCTION CDMA_EXIT

DESCRIPTION
  This procedure is the exit state for the CDMA state machine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RXTX task will go to start mode,  Receive task will exit CDMA mode,
  Transmit task will exit CDMA mode and Search task will go to Start
  mode.

===========================================================================*/

void cdma_exit
(
  boolean voc_pwr_off
    /* indicator of whether vocoder power needs to be turned off */
)
{

  #ifdef FEATURE_FULL_TIME_SHDR_3
  /* Variable that stores remaining time on an already runnning
     slotted timer  (if any) */
  dword remaining_t_slotted_time = 0;
  #endif /* FEATURE_FULL_TIME_SHDR_3 */

  /* Command Receive task to go to START mode */
  mcc_rx_buf.exit.hdr.command = RXC_EXIT_F;
  mcc_rxc_cmd( &mcc_rx_buf );   /* send command to receive task */

  /* Command Layer 2 task to go to START mode */
  mcc_rxtx_buf.start.hdr.command = RXTX_START_F;
  mcc_rxtx_cmd( &mcc_rxtx_buf );   /* send command to layer2 task */

  /* Send request to Transmit task to go to START mode */
  mcc_tx_buf.exit.hdr.command = TXC_EXIT_F;
  mcc_txc_cmd( &mcc_tx_buf ); /* Send command to Transmit task */

  if( voc_pwr_off )
  {
#ifdef FEATURE_MVS_MIGRATE
#error code not present
#else  
    voc_release( VOC_ACQ_CDMA );
#endif /* FEATURE_MVS_MIGRATE */
  }

  #ifdef FEATURE_FULL_TIME_SHDR_3
  /* Make sure 1x can do acquisition on RX0 regardless of the state
  ** of Ev-DO.
  */
  if (
      ( cdma.t_slotted != 0) || 
      ((remaining_t_slotted_time = rex_get_timer(&mcc_slotted_timer))!=0 ) ||
      ( srchrdiv_get_ftm_second_chain_test() == TRUE )
     ) 
  {
    /* Slotted timer is running, don't enable DDTM so that 1x is able
    ** to access the network on RX0.  But make sure that HDR is not using 
    ** RX0 to access the HDR by asking HDR declare system loss. */

    MSG_HIGH( "Unlock HDR RF for FTS_3", 0, 0, 0 );
    cm_unlock_hdr_resources();

   /* Make sure DDTM is turned OFF */
    mcc_set_mc_ddtm_status(FALSE);
    MSG_HIGH( "Turned OFF DDTM for FTS_3", 0, 0, 0 );
  }
  else
  {
    /* Mobile is in slotted mode.  Invoke DDTM until after 1x goes to
    ** sleep after acquisition and collecting overheads.  This allows
    ** 1x to do acquisition on RX0 but not accessing the network. */

    MSG_HIGH( "Turned ON DDTM for FTS_3", 0, 0, 0 );
    mcc_set_mc_ddtm_status( TRUE );
  }
  #endif /* FEATURE_FULL_TIME_SHDR_3 */

  /* Command Searcher task to go to CDMA mode - This will turn off
     interrupts.  Searcher task will not immediately go to CDMA state.
     If we exit CDMA mode we will send SRCH_DEACTIVATE_F at that time. 
     Otherwise we will send SRCH_ACQ_F when we pick a new system. */

  /* Command Searcher task to go to CDMA mode */
  mcc_srch_buf.hdr.cmd = SRCH_CDMA_F;
  mcc_srch_cmd( &mcc_srch_buf );

  /* Print out exit parameters */
  exit_parms_out();

} /* cdma_exit */

/*===========================================================================

FUNCTION CDMA_EXIT_INIT

DESCRIPTION
  This procedure initializes the Exit state for CDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cdma_exit_init
(
  void
)
{
  mccsrch_rpt_type *srch_ptr;
    /* pointer to report received from Searcher */
  mccrx_rpt_type *rpt_ptr;
    /* pointer to report from RX */
  mccrxtx_cmd_type *rxtx_ptr;
    /* Pointer to item from mcc_rxtx_q */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MC_QUEUE_WATERMARK
  mc_store_watermark();
#endif /* FEATURE_MC_QUEUE_WATERMARK */

  while ((rxtx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) != NULL)
  {
    q_put( rxtx_ptr->hdr.cmd_hdr.done_q_ptr, &rxtx_ptr->hdr.cmd_hdr.link );
  }

  while ((rxtx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_ack_q )) != NULL)
  {
    /* -----------------------------------------------------
    ** Explicitly return all buffers to rxtx_mc_free_q since
    ** done_q_ptr may point to mcc_rxtx_ack_q if the message
    ** was to be acked by the rxtx task
    ** ----------------------------------------------------- */
    q_put( &rxtx_mc_free_q, &rxtx_ptr->hdr.cmd_hdr.link );
  }

  while ( ( rpt_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q ) ) != NULL )
  {
    q_put (rpt_ptr->hdr.rpt_hdr.done_q_ptr, &rpt_ptr->hdr.rpt_hdr.link);
  }

  while ( (srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL )
  {
    q_put (srch_ptr->hdr.rpt_hdr.done_q_ptr, &srch_ptr->hdr.rpt_hdr.link);
  }

  /* Make sure all timers are cleared */
  (void) rex_clr_timer( &mcc_state_timer );
  (void) mcc_clr_callback_timer( &mcc_bs_insanity_timer, MCC_BS_INSANITY_TIMER_ID );
  (void) rex_clr_timer( &mcc_rpt_timer );
  (void) rex_clr_timer( &mcc_cmd_timer );
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID );

  (void) mcc_clr_callback_timer(&mcc_tr_request_timer, MCC_TR_REQUEST_TIMER_ID);
  
#ifdef FEATURE_IS95B_MDR
  /* If IS95B_MDR is supported, clear scam start timer */
  (void) rex_clr_timer( &mcc_scam_start_timer );
#endif /* FEATURE_IS95B_MDR */

#if defined(FEATURE_IS95B_MDR)
  /* If any MDR is supported, clear duration timer */
  (void) rex_clr_timer( &mcc_mdr_duration_timer );
#endif /* defined(FEATURE_IS95B_MDR) */

  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );

  /* Don't clear slotted timer here, we want this to still
     be running, so that when go back to idle we operate in 
     non-slotted mode while timer is still running */
  //(void) rex_clr_timer (&mcc_slotted_timer);

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

#ifdef FEATURE_SO_8026
#error code not present
#endif /* FEATURE_SO_8026 */

  (void) mcc_clr_callback_timer (&mcc_t50_timer, MCC_T50_TIMER_ID);
  (void) mcc_clr_callback_timer (&mcc_t51_timer, MCC_T51_TIMER_ID);

#ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA */

  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #endif /* FEATURE_IS2000_REL_A_AES */

  /* Clear all sigs */
  (void) rex_clr_sigs( &mc_tcb, 0xFFFF );

  /* Clear Callback Timer Queue */
  mcc_purge_timer_callback_q();

} /* cdma_exit_init */

/*===========================================================================

FUNCTION CDMA_RF_INIT

DESCRIPTION
  This procedure initializes rf for the CDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cdma_rf_init
(
  word channel,
    /* channel to acquire */
  byte band_class
    /* band class to acquire in */
)
{
  db_rf_mode_type temp_mode; /* Temporary storage for rf_mode */

  db_get( DB_RF_MODE, &mcc_db_val );
  temp_mode = mcc_db_val.rf_mode;

  db_get( DB_CHANNEL, &mcc_db_val );

  if ((mcc_db_val.channel != channel) ||
      ((temp_mode != DB_RF_CDMA) && (temp_mode != DB_RF_PCN))
      || (temp_mode == DB_RF_CDMA &&
          mcc_is_band_class_in_group(MCC_PCS_GROUP, band_class)
          )
      || (temp_mode == DB_RF_PCN && 
          mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, band_class)
          )
      )
  {
    MSG_MED( "db chan %d entry chan %d", mcc_db_val.channel, channel, 0 );

    /* Report events */
    event_report_payload( EVENT_BAND_CLASS_CHANGE, sizeof(event_band_class_change_type),
                       &band_class );
    event_report_payload( EVENT_CDMA_CH_CHANGE, sizeof(event_cdma_ch_change_type),
                       &channel );
  }

} /* cdma_rf_init */

#ifndef FEATURE_PLT
/*===========================================================================

FUNCTION CDMA_ENTER_INIT

DESCRIPTION
  This procedure is initializes variables used in the entrance state for
  the CDMA state machine.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  Certain variables used by the cdma_enter state will be initialized.

===========================================================================*/

word cdma_enter_init
(
  mcc_entry_type *mcc_entry_reason_ptr  /* Reason for entry */
)
{
  word next_state;
    /* Next state to be processed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize state to Enter State */
  next_state = CDMA_ENTER;


  /* store entry parameters for later use */
  cdma.entry_ptr = mcc_entry_reason_ptr;

  switch( mcc_entry_reason_ptr->entry )
  {
    case MCC_ACQUISITION:
    case MCC_PSEUDO_IDLE:
    {
      if ( mcc_entry_reason_ptr->entry == MCC_PSEUDO_IDLE )
      {
        MSG_HIGH("CDMA Pseudo-Idle Mode entry", 0, 0, 0);
      }
      else
      {
        MSG_HIGH("CDMA Acquisition entry", 0, 0, 0);
      }

      MSG_HIGH( "BLK_SYS %d BAND_CLASS %d CHANNEL %d",
        cdma.entry_ptr->gen.block_or_sys, cdma.entry_ptr->gen.band_class,
        cdma.entry_ptr->gen.cdma_channel);

      MSG_HIGH("New mode or blksys : %d",
        cdma.entry_ptr->gen.new_mode_or_blksys, 0, 0);

      /* Initialize rf for cdma */
      cdma_rf_init(cdma.entry_ptr->gen.cdma_channel,
        cdma.entry_ptr->gen.band_class);

      cdma.cdmach = cdma.entry_ptr->gen.cdma_channel;
      cdma.band_class   = cdma.entry_ptr->gen.band_class;
      cdma.block_or_sys = (nv_block_or_sys_enum_type)cdma.entry_ptr->gen.block_or_sys;
      break;
    }

    case MCC_ENTER_OFFLINE:
    {
      MSG_HIGH( "MCC OFFLINE ENTRY", 0, 0, 0 );

      /* Initialize rf for cdma */
      cdma_rf_init(mcc_entry_reason_ptr->gen.cdma_channel,
        mcc_entry_reason_ptr->gen.band_class);
      cdma.band_class = mcc_entry_reason_ptr->gen.band_class;
      cdma.cdmach = mcc_entry_reason_ptr->gen.cdma_channel;
      break;
    }

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

    default:
    {
      ERR( "INVALID ENTRY REASON %d", mcc_entry_reason_ptr->entry, 0, 0 );
      break;
    }
  }

  return (next_state);

} /* cdma_enter_init */

/*===========================================================================

FUNCTION CDMA_ENTER

DESCRIPTION
  This procedure is the entrance state for the CDMA state machine.  It will
  request that the other tasks enter the CDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_enter
( 
  void
)
{
  static mc_msg_type *cmd_ptr;    /* Command from the MC_CMD_Q queue */
  word next_state = CDMA_ENTER;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cdma.entry_ptr->entry == MCC_ENTER_OFFLINE)
  {
    /* ------------------------------------------------
    ** Send request to Transmit task to go to CDMA mode
    ** ------------------------------------------------ */
    mcc_tx_buf.cdma.hdr.command = TXC_CDMA_F;
    mcc_txc_cmd( &mcc_tx_buf );

    /* -----------------------------------------------
    ** Send request to Receive task to go to CDMA mode
    ** ----------------------------------------------- */
    mcc_rx_buf.cdma.hdr.command = RXC_CDMA_F;
    mcc_rxc_cmd( &mcc_rx_buf );

    /* --------------------------------------------------------
    ** Inform DIAG task we are ready to begin offline operation
    ** -------------------------------------------------------- */
    mc_inform_diag_offline_complete();

    while (next_state == CDMA_ENTER)
    {
      /* ------------------
      ** Wait for a command
      ** ------------------ */
      (void) mcc_wait( MCC_CMD_Q_SIG );

      if ((cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q )) != NULL)
      {
        switch (cmd_ptr->hdr.cmd)
        {

#if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
          case MC_MO_DBM_F:
            /* MO DBM request received but we are in off-line.
               Should ignore. */
            MSG_HIGH( "Rejecting MO DBM request in CDMA_ENTER", 0, 0, 0 );
            mcc_send_mo_dbm_status( cmd_ptr->mo_dbm.burst_type,
                                    CM_DBM_MO_NO_SRV );
            cmd_ptr->hdr.status = MC_DONE_S;
            break;
#endif /* FEATURE_CDSMS || FEATURE_DBM */

          case MC_CDMA_PROT_DEACT_CMD_F:
            mcc_exit.mmoc_trans_id = cmd_ptr->deact_prot.info.trans_id;
            next_state = CDMA_EXIT;
            cmd_ptr->hdr.status = MC_DONE_S;
            mcc_exit.exit = mc_map_deact_to_mcc_exit_reason
                            (
                              cmd_ptr->deact_prot.info.reason
                            );
            break;

          case MC_CDMA_PROT_GEN_CMD_F:
            /* Process the MMoC's generic protocol command.
            */
            if (!mcc_continue_on_curr_sys
                 (
                   MEAS_BACK_ALLOWED,
                   mmoc_proc_prot_gen_cmd( TRUE,
                                           TRUE,
                                           &cmd_ptr->gen_cmd.info
                                         )
                 )
               )
            {
               mcc_exit.exit = mc_map_gen_cmd_to_mcc_exit_reason
                               (
                                 &cmd_ptr->gen_cmd.info
                               );
               next_state    = CDMA_EXIT;
            }
            cmd_ptr->hdr.status = MC_DONE_S;
            break;


          default:
            MSG_HIGH( "Unexpected cmd %d", cmd_ptr->hdr.cmd, 0, 0 );
            cmd_ptr->hdr.status = MC_BADCMD_S;
            break;
        } /* end switch (cmd_ptr->hdr.cmd) */

        cmd_done( &cmd_ptr->hdr.cmd_hdr );
      }
    }
  } /* end if (cdma.entry_ptr->entry == MCC_ENTER_OFFLINE) */

  else
  {

#ifdef FEATURE_OTASP
    /* Initialize to FALSE, set to TRUE for MCC_ACTIVATION entry */
    cdma.otasp_call = NO_SRV_PROV;
#endif /* FEATURE_OTASP */

    {
      cdma.call_orig  = FALSE;

#if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
      cdma.dbm_wait_for_l2_ack = FALSE;
#endif /* FEATURE_CDSMS || FEATURE_DBM */

    }

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif

    /* -----------------------------
    ** Send other tasks to CDMA mode
    ** ----------------------------- */

    /* First of all initialize the BS overhead info. */
    mcc_init_ovhd_info();

#ifdef FEATURE_FULL_TIME_SHDR_3
    /* If the second chain test is in progress, we want the MS to be able to
       respond to 1X Pages. So force the HDR out of the system and make sure
       DDTM is turned OFF.
     */
    if ( srchrdiv_get_ftm_second_chain_test() == TRUE )
    {
       MSG_HIGH( "Unlock HDR RF for FTS_3", 0, 0, 0 );
       cm_unlock_hdr_resources();

       /* Make sure DDTM is turned OFF */
       mcc_set_mc_ddtm_status(FALSE);
       MSG_HIGH( "Turned OFF DDTM for FTS_3. Second-chain test in-progress", 0, 0, 0 );
    }

#endif /* end of FEATURE_FULL_TIME_SHDR_3 */

    /* Transmit task is in CDMA mode, send request to Receive task to
       go to CDMA mode */
    mcc_rx_buf.cdma.hdr.command = RXC_CDMA_F;
    mcc_rxc_cmd( &mcc_rx_buf );

    /* Ready to attempt Pilot Acquisition */
    next_state = CDMA_PIL_ACQ;  /*lint !e641 */
  }

  return (next_state);

} /* end cdma_enter() */
#endif /* !FEATURE_PLT */

/*===========================================================================

FUNCTION MCCDMA_CLEAR_MC_QUEUES_SIGS_TIMERS

DESCRIPTION
  This fn will clear all of MC's timers, It clears all of MC's signals
  and purges RXTX, RXC & SRCH queues.

DEPENDENCIES
  None

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  Any active MC signal is cleared & any otems on the queue are lost

===========================================================================*/

void mccdma_clear_mc_queues_sigs_timers
(
  void
)
{
  mccrx_rpt_type *rx_ptr;
    /* Pointer to item from mcc_rx_rpt_q */
  mccsrch_rpt_type *srch_ptr;
    /* Pointer to item from mcc_srch_q */
  mccrxtx_cmd_type *rxtx_ptr;
    /* Pointer to item from mcc_rxtx_q */
  /* --------------------------------
  ** Make sure all timers are cleared
  ** -------------------------------- */
  (void) rex_clr_timer( &mcc_rpt_timer);
  (void) rex_clr_timer( &mcc_state_timer);
  (void) mcc_clr_callback_timer( &mcc_bs_insanity_timer, MCC_BS_INSANITY_TIMER_ID);
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID);

  (void) mcc_clr_callback_timer(&mcc_tr_request_timer, MCC_TR_REQUEST_TIMER_ID);
  
#ifdef FEATURE_IS95B_MDR
  /* If IS95B_MDR is supported, clear scam start timer */
  (void) rex_clr_timer( &mcc_scam_start_timer );
#endif /* FEATURE_IS95B_MDR */

#if defined(FEATURE_IS95B_MDR)
  /* If any MDR is supported, clear duration timer */
  (void) rex_clr_timer( &mcc_mdr_duration_timer );
#endif /* defined(FEATURE_IS95B_MDR) */

  (void) rex_clr_timer( &mcc_cmd_timer);
  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

#ifdef FEATURE_SO_8026
#error code not present
#endif /* FEATURE_SO_8026 */

  (void) mcc_clr_callback_timer (&mcc_t50_timer, MCC_T50_TIMER_ID);
  (void) mcc_clr_callback_timer (&mcc_t51_timer, MCC_T51_TIMER_ID);

#ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA */

  /* --------------
  ** Clear all sigs
  ** -------------- */
  (void) rex_clr_sigs ( &mc_tcb, 0xFFFF );

  /* Clear Callback Timer Queue */
  mcc_purge_timer_callback_q();

  /* -------------------------------------------------------------
  ** Clear any remaining buffers from mcc_rx_rpt_q, mcc_srch_q and
  ** mcc_rxtx_q
  ** ------------------------------------------------------------- */
  while ((rx_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q )) != NULL) {
    q_put( rx_ptr->hdr.rpt_hdr.done_q_ptr, &rx_ptr->hdr.rpt_hdr.link );
  }

  while ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL) {
    q_put( srch_ptr->hdr.rpt_hdr.done_q_ptr, &srch_ptr->hdr.rpt_hdr.link );
  }

  while ((rxtx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) != NULL) {
    q_put( rxtx_ptr->hdr.cmd_hdr.done_q_ptr, &rxtx_ptr->hdr.cmd_hdr.link );
  }

  while ((rxtx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_ack_q )) != NULL)
  {
    /* -----------------------------------------------------
    ** Explicitly return all buffers to rxtx_mc_free_q since
    ** done_q_ptr may point to mcc_rxtx_ack_q if the message
    ** was to be acked by the rxtx task
    ** ----------------------------------------------------- */
    q_put( &rxtx_mc_free_q, &rxtx_ptr->hdr.cmd_hdr.link );
  }

} /* mccdma_clear_mc_queues_sigs_timers */

#ifndef FEATURE_PLT
/*===========================================================================

FUNCTION MCC_SUBTASK

DESCRIPTION
  This procedure contains the state machine for the CDMA subtask.
  It is called from the Main MC when the phone is to operate in CDMA
  mode.

DEPENDENCIES
  This procedure assumes that the main control task timers, random number
  generating services and fine timer services are already initialized.

RETURN VALUE
  The return value contains the reason that the CDMA state machine was exited.

SIDE EFFECTS
  None.

===========================================================================*/

mcc_exit_type *mcc_subtask
(
  mcc_entry_type *mcc_entry_reason_ptr  /* Reason for entry */
)
{
  boolean voc_pwr_off = FALSE;
    /* Indicator of whether vocoder power needs to be turned off on exit */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the MMoC's transaction to None.
  */
  mcc_exit.mmoc_trans_id = PROT_TRANS_NONE;

  /* Event report: Operation mode change */
  mclog_report_event_op_mode(MCLOG_OP_MODE_CDMA);

  /* Initialize for entrance state */
  cdma.curr_state = cdma_enter_init( mcc_entry_reason_ptr );

  while (MAIN_STATE( cdma.curr_state ) != CDMA_EXIT)
  {
    switch (MAIN_STATE( cdma.curr_state ))
    {
      case CDMA_ENTER:
        cdma.curr_state = cdma_enter();  /* Process the enter state */
        break;

      case CDMA_INIT:
        cdma.curr_state = cdma_init();   /* Process initialization state */
        break;

      case CDMA_IDLE:
        cdma.curr_state = cdma_idle();   /* Process the idle state */
        break;

      case CDMA_SYS_ACC:
        cdma.curr_state = cdma_sa();     /* Process system access state */
        break;

      case CDMA_TC:
        cdma.curr_state = cdma_tc();     /* Process traffic channel state */
        /* Have to turn vocoder power off in exit */
        voc_pwr_off = TRUE;
        break;
        
      #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
      #endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

      default:
        ERR_FATAL( "BAD CDMA state %d", cdma.curr_state, 0, 0 );
        break;

    } /* switch */

    if (MAIN_STATE( cdma.curr_state ) == CDMA_EXIT)
    {
      /* ---------------------------------------
      ** Make sure timers and queues are cleared
      ** --------------------------------------- */
      cdma_exit_init();

      if ((mcc_exit.exit == MCC_REDIRECTION)
           || (mcc_exit.exit == MCC_ACCESS_FAILED_GSRM)
           )
      {
        /* Reset CDMA registration indicator */
        mccreg_clear_reg();
      }

      if( MAIN_STATE(cdma.curr_state) == CDMA_EXIT )
      {
        /* only way to exit is if state is still equal to EXIT */
        cdma_exit( voc_pwr_off);
      }
    }
  } /* end while ( MAIN_STATE(cdma.curr_state) != CDMA_EXIT ) */

  /* -------------------------------------------
  ** Any module that sets the state to CDMA_EXIT
  ** will fill out the exit reason in mcc_exit.
  ** ------------------------------------------- */

  return (&mcc_exit);

} /* mcc_subtask */
#endif /* !FEATURE_PLT */

/*===========================================================================

FUNCTION MCC_OFFLINE

DESCRIPTION
  This procedure should be called when the DMSS is to transition to
  OFFLINE state.  It performs the necessary processing required before
  OFFLINE state:
     - Saves necessary parameters to NV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_offline
( 
  void
)
{
  /* -------------------
  ** Check state of DMSS
  ** ------------------- */
  db_get( DB_DMSS_STATE, &mcc_db_val );

  /* ----------------------------------
  ** Only write values if we are ONLINE
  ** ---------------------------------- */
  if ((mcc_db_val.dmss_state == DB_DMSS_ONLINE_ANALOG_S) ||
      (mcc_db_val.dmss_state == DB_DMSS_ONLINE_DIGITAL_S))
  {
     /* --------------------------------
     ** Write necessary parameters to NV
     ** -------------------------------- */
     mccreg_powerdown();
  }
} /* end mcc_offline() */

/*===========================================================================

FUNCTION MCC_POWERDOWN

DESCRIPTION
    This procedure should be called once when the DMSS is to power down.
    It performs the necessary processing required before powering down.

DEPENDENCIES
    This procedure saves parameters in NV.  The NV task should be ready
    to receive commands when this procedure is called.

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/

void mcc_powerdown (void)
{
  mccsrch_rpt_type *srch_ptr;  /* pointer to report received from Searcher */
  boolean srch_deactivate_done_rpt_rxed = FALSE; /* Flag when rpt rxed */

  /* Perform registration powerdown housekeeping */
  mccreg_powerdown();

  /* Perform TMSI powerdown housekeeping */
  tmsi_powerdown ();

  /* Deactivate the Search task */
  mcc_srch_buf.hdr.cmd = SRCH_DEACTIVATE_F;
  mcc_srch_cmd( &mcc_srch_buf );

  /* Searcher always send MC SRCH_DEACTIVATE_DONE_R in response to 
     SRCH_DEACTIVATE_F.  Discard it.  Otherwise, mccsrch_bufs[] will
     be overflowed. */
  while (!srch_deactivate_done_rpt_rxed)
  {
    (void) mcc_wait( (dword) MCC_SRCH_Q_SIG );
    while ( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
    {
      if ( srch_ptr->hdr.rpt == SRCH_DEACTIVATE_DONE_R )
      {
        MSG_HIGH( "Rxed SRCH_DEACTIVATE_DONE_R, Discarded", 0, 0, 0 );
        srch_deactivate_done_rpt_rxed = TRUE;
        cmd_done( &srch_ptr->hdr.rpt_hdr );
        break;
      }
      MSG_MED("DISCARDED %d",  srch_ptr->hdr.rpt,0,0);
      cmd_done( &srch_ptr->hdr.rpt_hdr );
    }
  }

  /* Tell Search to perform powerdown housekeeping */
  mcc_srch_buf.hdr.cmd = SRCH_POWERDOWN_F;
  mcc_srch_cmd( &mcc_srch_buf );

} /* mcc_powerdown */

/*===========================================================================

FUNCTION MCC_CM_MC_RPT

DESCRIPTION
  This function sends a report to the CM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_cm_mc_rpt
(
  cm_mc_rpt_type *cmd_ptr
    /* pointer to command to send to cm */
)
{
  cmd_ptr->hdr.task_ptr = NULL;

  MSG_LOW ("CM MC rpt %d",cmd_ptr->hdr.cmd,0,0);
  cm_mc_rpt(cmd_ptr);

  /* Inform system determination of emergency call flashed */
  if (cmd_ptr->hdr.cmd == CM_CALL_EMERGENCY_FLASHED_F)
  {
    (void) sd_ss_ind_misc_orig_success(NULL);
  }

} /* mcc_cm_mc_rpt */

#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================

FUNCTION MCC_SET_RSCH_DTX_SIG

DESCRIPTION
  This function sets the MCC_TXC_REV_DTX_SIG. It is called by TXC when it
  detects REV DTX.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_set_rsch_dtx_sig (void)
{
  (void) rex_set_sigs( &mc_tcb, MCC_TXC_REV_DTX_SIG);   /*lint !e570*/
}
#endif /* FEATURE_IS2000_R_SCH */

/* the following two functions are hooks for voice service encryption
 * and decryption. If AES is not defined, the functions just do not
 * touch the voice frame buffer.
 *
 * for now we need surpress the lint error 715 that complains about
 * unreferrenced arguments of the functions
 */
/*lint -e715 */
/*===========================================================================

FUNCTION MCCDMA_ENCRYPT_VOICE_FRAME

DESCRIPTION
  This function encrypts a voice frame when voice encryption is turned on
  over voice services
  

DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_encrypt_voice_frame(
               qword sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint32 frame_offset,    /* the starting bit to be encrypted */
               uint32 frame_bits       /* the number of bits to be encrypted */
              )
{
  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #endif /* FEATURE_IS2000_REL_A_AES */
} /* mccdma_encrypt_voice_frame */


/*===========================================================================

FUNCTION MCCDMA_DECRYPT_VOICE_FRAME

DESCRIPTION
  This function decrypts a voice frame when voice decryption is turned on over
  voice services
  

DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_decrypt_voice_frame(
               qword sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint32 frame_offset,    /* the starting bit to be encrypted */
               uint32 frame_bits       /* the number of bits to be encrypted */
              )
{
  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #endif /* FEATURE_IS2000_REL_A_AES */
} /* mccdma_decrypt_voice_frame */

/*===========================================================================

FUNCTION MCCDMA_GET_CUR_BS_INFO

DESCRIPTION
  This function make a copy of the following fields of cur_bs_ptr and pass
  the values to the calling function.

DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_get_cur_bs_info
(
  word* sid,              /* System Identification */
  word* nid,              /* Network Identification */
  word* base_id,          /* Base Station Identification */
  byte* base_class,       /* Base Station Class */
  word* pilot_pn,         /* Pilot PN sequence offset index */
  byte* packet_zone_id,   /* Packet Zone Identification */
  word* mcc,              /* Mobile Country Code */
  dword* base_lat,         /* Base Station Latitude */
  dword* base_long         /* Base Station Longitude */
)
{
  *sid = cur_bs_ptr->csp.sp.sid;
  *nid = cur_bs_ptr->csp.sp.nid;
  *base_id = cur_bs_ptr->csp.sp.base_id;
  *base_class = cur_bs_ptr->csp.sp.base_class;
  *pilot_pn = cur_bs_ptr->csp.sp.pilot_pn;
  *packet_zone_id = cur_bs_ptr->csp.esp.packet_zone_id;
  *mcc = cur_bs_ptr->csp.esp.mcc;
  *base_lat = cur_bs_ptr->csp.sp.base_lat;
  *base_long = cur_bs_ptr->csp.sp.base_long;
} /* mccdma_get_cur_bs_info */

#ifdef FEATURE_PASSPORT_MODE

/*===========================================================================

FUNCTION MCC_MSG_IS_BLOCKED

DESCRIPTION
  Checks if the passed msg type is blocked 

DEPENDENCIES
  None

RETURN VALUE
  True    Passed msg type is blocked

  False   Passed msg type is not blocked

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_msg_is_blocked (cdma_msg_type msg_type)
{
  boolean msg_blocked = FALSE;

  if((cdma_block_action_mask & msg_type) == msg_type)
  {
    msg_blocked = TRUE;
  }

  return msg_blocked;
}

/*===========================================================================

FUNCTION MCC_PASSPORT_MODE_IS_JCDMA

DESCRIPTION
  Returns TRUE if the passport mode is JCDMA.

DEPENDENCIES
  None

RETURN VALUE
  True    Passport mode is world

  False   Passport mode is jcdma

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_passport_mode_is_jcdma (void)
{
  #ifdef FEATURE_JCDMA
  return TRUE;
  #else
  return FALSE;
  #endif
}

/*===========================================================================

FUNCTION MCC_INIT_PASSPORT_MODE

DESCRIPTION
  Initializes the variables required to support a particular passport mode

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void mcc_init_passport_mode (void)
{
  mccap_pm_init();
  mccap_df_pm_init();
}
#endif /* FEATURE_PASSPORT_MODE */

#if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) )
#error code not present
#endif /*#if ( defined(FEATURE_MMGSDI) ) && ( defined(FEATURE_CSIM) ) */
/*lint +e715 */
/*lint +e818 */
