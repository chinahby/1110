#ifndef MC_H
#define MC_H
/*===========================================================================

          M A I N   C O N T R O L   T A S K    H E A D E R

DESCRIPTION
  This file contains global declarations and external references
  for Main Control task.  This file contains the data structures
  and interface definitions to allow any task to interface to the
  Main Control task (MC) and its subtasks (MCCDMA and ACPMC).  All
  messages sent to MC and its subtasks are queued to the MC command
  queue via services provided by the MC_CMD unit.

Copyright (c) 1990-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mc.h_v   1.32   02 Oct 2002 13:47:10   sbrelsfo  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mc.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/08   bb      Code changes for "MVS API Update feature"
05/14/08   ag      Watermarking for trans_queue.
12/14/07   an      Added Passport Mode
01/30/07   sb      Changed function prototype for mc_get_csna_message_type.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
08/07/06   fc      Cleanup featurization.
06/23/06   pg      Added support for RPC.
05/22/06   an      Fixed compilation warnings
04/26/06   an      Fixed Lint Errors
02/07/06   fc      Mainlined FEATURE_MC_SUPPORTS_SHDR.
01/20/06   fc      Merged the following :
           yll     Added key setup timer.
11/03/05   an      Moved MCC_CHK_PTR macro definition to mc.h from mccdma.h
10/27/05   fc      Changed MCC_TR_REQUEST_TIMER_ID to 0x00002000 to resolve
                   conflict.
07/22/05   va      Removed is_jcdma_emergency flag
07/08/05   fh      Added a new callback timer ID in the enumerate definition:
                     MCC_TR_REQUEST_TIMER_ID = 0x00000F00
                   This timer is for transceiver request callback timer use
04/05/05   fc      Merged the following change from MSMSHARED:
04/05/05   an       Defined P_REV_IS2000_REL_D
03/23/05   fc      Merged the following change from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated 
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design
03/01/05   fc      Merged the following change from MSMSHARED:
           ka      Subscription info is only reloaded if CM informed there is
                   a subscription change
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/03/05   fc      Fixed lint error.
01/07/05   an      Removed feature FEATURE_SMS and FEATURE_UASMS
09/27/04   va      Merged following from REL B/C archive
07/21/04   sj        Added support for umts to 1x handover.
06/03/04   fc        Obsolete old P_REV macro.
08/20/04   bkm     Removed MCC_TXC_JUMP_DONE_SIG, MUX does set
                   TXC_DONE_SIG when TC timing jump completes.
08/09/04   bkm     Converted T50 and T51 timers to callback timers
08/06/04   bkm     Added MCC_TXC_JUMP_DONE_SIG.
06/03/04   fc      P_REV macro cleanup.
                   Support for Release D P_REV.
05/28/04   bkm     Merged the following from IS2000_REL_B_C
  03/25/04   bkm     Use NV #define for maximum digits to allow for 32 or 36
                     based on FEATURE_36_DIGIT_DIAL_STRING.
  03/11/04   yll     Added support for KDDI requirement to ignore async data
                     and async fax 1x pages during HDR connected state.
  03/05/04   bkm     Lint cleanup.
04/08/04   bkm     Moved channel mask enums to mcc.h
02/05/04   bkm     Merged the following from IS2000_REL_B_C
  01/14/04   ic      Undoing previous change
  01/14/04   ic      Cleanup, removed command types associated with MC commands
                     that had been removed:
                     - MC_ONLINE_F / mc_online_type
                     - MC_BSSTATUS_REQ / mc_bsstatus_req_type
                     - MC_PREF_SYS_CHANGED_F / mc_pref_sys_changed_type
                     - MC_ONLINE_DIGITAL_F / mc_online_digital_type
                     - MC_POWERSAVE_HDR_F / mc_powersave_hdr_type
                     - MC_ONLINE_HDR_F / mc_online_hdr_type
                     - MC_DEACTIVATE_HDR_F / mc_deact_reason_e_type / mc_deactivate_hdr_type
                     - MC_STOP_FWD_PREF_SYS_CMD_F / mc_stop_fwd_pref_sys_cmd_type
                     - MC_IDLE_PGSLOT_CHANGED_F / mc_idle_pgslot_changed_type
                     - MC_DDTM_CNTL_F / mc_ddtm_cntl_type
                     Removed from mc_msg_type union fields corresponding to the above.
  01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
01/05/04   bkm     Added enums for fwd/rev channels.
11/11/03   yll     Added support for JCDMA emergency calls.
11/06/03   fc      Modified MOB_P_REV_IS_VALID() macro to cover Release B
                   and Release C.
11/04/03   ph      Added some comments.
11/04/03   yll     Added MOB_P_REV_IS_VALID() macro.
10/20/03   ic      Removed HDR-related commands from mc_name_type.
08/20/03   yll     Changed FEATURE_HDR_HYBRID to FEATURE_HDR.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/08/03   fc      Added support for IS2000 Release C P_REV.
07/30/03   az      Added support for FEATURE_SO_8026
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
04/24/03   bkm     Changed length of digit array for mc_burst_dtmf_type from
                   32 to 255 to match the spec.
03/28/03   va      Added P_REV_IS_8_OR_GREATER macro
01/31/03   mpa/bkm Added MC_PPP_CLOSED & MC_UNLOCK_HDR_RF_F for HDR Hybird Mode.
11/07/02   az      Mainlined FEATURE_IS95B
10/31/02   az      Added support for HDR Data dedicated transmission mode
                   access prevention.
10/15/02   yll     Merged in control hold changes.
10/02/02   sb      Renamed is_silent_redial, silent_redial_count to orig_reason, orig_count.
10/01/02   az      Mainlined FEATURE_IS2000
09/30/02   sb      Mainlined FEATURE_SD20.
08/13/02   HQ      Added FEATURE_CDSMS.
07/29/02   jqi     Undefined mc_get_esn_me() when multimode arch is defined.
07/02/02   jqi     Added support for multimode architecture.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/01/02   yll     Added support for MC Queue Watermark.
06/07/02   sj      Added CP support for multimode architecture.
06/05/02   jqi     Added support for FEATURE_MULTIMODE.
04/12/02   lh      Added support for subaddress in Orig_C.
13/03/02   sh      Added MC_ONLINE_ANALOG_F
03/09/02   lh      Merged from CP119.
03/03/02   sh      Removed MC_STOP_FWD_PREF_SYS_CMD_F and MC_DEACT_PREF_SYS_CHANGED
02/13/02   jrp     Merged from common archive:
                   02/06/02   sh      Added hybird operation support
                   01/25/02   sj      Added new parameters for pref_sys_changed_type command.
                   01/19/02   az      Added a new enum type mc_pref_reas_type. Added a new variable
                                      in data structure for MC_PREF_SYS_CHANGED_F
                   01/12/02   az      Replaced 2 variables with a single variable favored_roam_ind
                                      in mc_sr_type
                   01/10/02   jqi     Added GBLOCK and HBLOCK in mc_activate_code_type
                                      for support of band class 5 in SD 1.0.
                   12/08/01   sh      Renamed MC_DIGITAL_MODE_NONE to MC_DIGITAL_MODE_INACT
02/11/02   va      Fixed the mc_flash_type interface.
02/07/02   lh      Change some T_53 into FEATURE_T53.
01/22/02   va      Fixed some misc errors.
01/17/02   va      Fixed some compile errors if REL_A ia not defined.
01/17/01   va      Featurized cm_flash_type_e_type under FEATURE_IS2000_REL_A
01/08/02   lh      Remove FEATURE_IS2000_REL_A_SVD from MC-CM interface.
12/21/01   ph      Migrated mcc_l2_ack_timer to the callback timer mechanism.
12/11/01   ph      Merged in from SVD branch
                   11/21/01   lh      Changes for CM/UI integration.
                   08/28/01   lh      Support for concurrent service.
                   08/23/01   ph      Fixed error in Timer Type enums.
12/06/01   lh      Merged from MSM_CP.05.00.108.
  11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                     mechanism.
  11/17/01   sh      Added 1xhdr data session handoff support
10/15/01   kk      Fixed compiler errors when Release A is not defined.
10/02/01   jrp     Merged from MSM_CP.05.00.96
                   09/19/01   az      Added a boolean flag fwd_link_failure in mc_sr_type to determine
                                    whether it is a forward link failure or reverse link failure. This
                                    flag is used during the building of silent redial scan list.
10/02/01   kk      Qos Parameters Added.
09/27/01   kk      Added emergency indicator for Flash.
08/08/01   yll     Merged in the following from MSM archive:
                   Removed the definitions required for store esn_me and
                   externalized the function to export cdma.esn_me
07/06/01   ph      Added support for Callback Timers
06/26/01   ph      Merged from MSM_CP.05.00.81
                   05/25/01   ts      Added signals and queues for new UIM server task.  Changed
                                      UIM feature switches to new server.
05/17/01   ph      Merged from MSM_CP.05.00.76
                   05/11/01   jq      Changed MC_SCRM_F to MC_RES_REQ_F.
                   03/27/01   rc      Added a parameter to MC_PREF_SYS_CHANGE (SD 2.0 only).
03/27/01   kk      Merged changes from common archive
                   03/08/01   va      Changed MCC_TXC_REV_DTX_SIG from 0x40000000 to 0x80000000, as it
                   was contending with FS_OP_COMPLETE_SIG which MC waits on during
                   boot up.
                   03/06/01   fc      Added enumeration mc_event_op_mode_type.
02/28/01   ph      Merged in MSM_CP.05.00.55
                   02/16/01   fc      Added support for system determination 2.0. All changes
                                      are featurized under FEATURE_SD20.
                   01/24/01   ych     Merged T-53 Changes
                   12/15/00   lh      Add support for MS originated Long Code Transistion Request
                                      Order
                   12/14/00   cah     Remove sms uasmsi.h inclusion.
                   12/13/00   cah     Replaced obsolete smsi.h inclusion with uasmsi.h.
                   11/30/00   yll     Removed FEATURE_GPSONE_TAWD_TEST_ONLY.
                   11/28/00   ck      Added constants and masks to support STORE_ESN_ME command.


02/27/01   kk      Added Release A specific parameters to CM Origination
01/10/01   va      Added P_REV_IS_7_OR_GREATER macros.
11/01/00   lcc     Added member DRS in mc_origination_type.
10/02/00   yll     Added support for Position Determination DBM.
08/23/00   jq      Added MCC_TXC_REV_DTX_SIG.
08/02/00   yll     Added GPSONE TAWD (tuning away w/o drop) test support.
07/17/00   jq      Modified mc_end_type to include end_reason.
07/14/00   ks      Defined new signal MCC_SLOTTED_TIMER_SIG.
06/19/00   fc      Added MC_SCRM_F to support SCRM.
06/12/00   lcc     Removed MC_TRANSACTION_DONE_F which was added by mistake.
04/24/00   lcc     Added MC_TRANSACTION_DONE_F.
04/11/00   lcc     Added definition for P_REV 6.
04/07/00   ry      Merged the following from PP (MSM3100_CP.03.00.17):
           jq      Added Broadcast SMS feature support.
03/07/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ych     Added support for Tri-mode Silent Redial
           ram     Added two Macros BAND_IS_CELLULAR and BAND_IS_PCS to
                   check the current band class during Tri-mode operation.
11/15/99   va      IS2000 Changes.
                   New macro added for p_rev 6 check
10/28/99   lcc     Moved PLT related code to plt_mc.c.
09/16/99   lcc     Merged in signals for PLT command interface from PLT archive.
08/21/99   ks      Changed Signal MCC_ACCPR_HO_OK_SIG to MCC_TXC_APHO_OK_SIG.
08/13/99   lh      CM and SMS phase 2 implementations.
08/09/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
07/30/99   fc      Put in changes based on code review comments on Analog
                   Registration Randomization.
07/28/99   lh      CM and two-way SMS support added.
07/23/99   fc      Added a new signal ACPMC_REDIRECT_TIMER_SIG.
07/19/99   abh     Changes for Access Probe Handoff.
07/09/99   ry      Updated MCC_SCAM_TIMER_SIG, MCC_MDR_DURATION_TIMER_SIG,
                   and MCC_TXC_PROBE_SIG to new 32-bit constants
           ry      Added MCC_TXC_FRAME_SIG and MCC_HANDOFF_TIMER_SIG
05/06/99   kmp     Merged IS-95B changes below.
           fc      Put in changes based on input from code review.
           fc      Redefined the signal value for MCC_TXC_PROBE_SIG.
           fc      Renamed MCC_ACC_PROBE_SIG to MCC_TXC_PROBE_SIG and
                   defined it under FEATURE_IS95B_PILOT_RPT.
           ks      Removed the AEHO and AHO Feature ifdefs.
           ks      Added AEHO and AHO related FEATURE ifdef checking.
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           fc      Added new signal MCC_ACC_PROBE_SIG.
           lh      P_REV_IS_5_OR_GREATER macro added.
           ks      Added defines for representing various P_REVs in mobile.
           lh      Added support for run-time P_REV and Band Class checking.
02/09/99   br      FEATURE_CDMA_800 changed into FEATURE_SSPR_800, T_AUTH
                   into FEATURE_AUTH, T_SMS into FEATURE_SMS and T_ACP
                   FEATURE_ACP.
03/09/99   ram     Defined SCAM/GHDM duration timer signal for IS95B MDR
02/19/99   ram     Defined SCAM start timer signal for IS95B MDR
01/13/99   lcc     Merged from MDR branch:
                   - Added run-time P_REV and Band Class checking functions from
                     IS95B.
11/24/98   br      Added changes for storing new NAM.
08/25/98   ck      Added the signal MCC_OTASP_AKEY_COMMIT_SIG
08/25/98   ck      Added DH support for builds with FEATURE_DH defined.
08/04/98   ych     Merged E911, SSPR-800, and Acq. Improvements
06/25/98   kis     Added ASCII mode related fields in ms_origination_type.
06/17/98   wjn     Added Feature E911 Support -- Modification of
                   "mc_pref_sys_changed_type".
03/20/98   abh     Moved mcsys function prototype for PRL Force Mode
                   in mcsyspr.h
01/28/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/28/98   wjn     Added a prototype UI-MC Interface function
                   mc_get_prl_version();
05/20/96   ych     Added new command MC_LPM_F.
12/13/96   jca     Modified mc_origination_type.
09/13/96   dna     Added two fields to origination cmd for FEATURE_OTASP.
06/25/96   dna     Added MCC_OTASP_COMMIT_SIG for FEATURE_OTASP.
05/24/96   jjw     Gemini & ISS2 Data Services release.
04/15/96   jca     Changed ms data type in mc_stop_cont_dtmf_type to word.
04/15/96   day     Added OTASP MC command.  Using FEATURE_OTASP ifdef.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
12/08/95   bns     Removed MC_REG_F for ISS2 forced registration
12/06/95   bns     Added MC_REG_F for ISS2 forced registration
11/27/95   gb      Fixed length field of AC_MSG tmob command.
08/24/95   day     Removed SMS data declaration for mobile-originated
                   messages to recover 4.7k RAM bytes.
07/13/95   rdb     Added MCC_SRV_TIMER_SIG for use with service negotiation.
07/07/95   dna     Added new signals for ACP for Authentication
07/06/95   jca     Now have one consolidated ORIGINATION command with a
                   specified service option number.
06/25/95   dy      Modified MC_DATA_BURST_F to MC_SMS_F and updated
                   corresponding data structure for new SMS interface.
06/20/95   jca     Added MC_LOOPBACK_F command.  Added new parameters to
                   MC_DATA_BURST_F command.
06/15/95   jca     MC, MCC and ACPMC now using same signal definitions for
                   NV commands and kicking the watchdog to avoid conflicts.
06/09/95   jca     Now using one rex_sig (MCC_Q_SIG) for all MCC queues.
06/07/95   dna     Added Authentication Definitions
03/24/95   jca     Added service option field to MC_DATA_F command.
11/01/94   gb      Sector Lock test functionality added.
05/12/94   gb      Added new test commands.
04/20/94   jca     Added MC_DATA_END_F command.
01/06/94   jah     Updated mc_task_start() prototype for watchdog iface change
12/22/93   jca     Modified for IS-95 Phase 1 upgrade.
01/05/93   jai     Added support for Mobile originated markov calls and
                   data services.
12/09/92    ip     Added duration of continuous DTMF to STOP_CONT_DTMF
                   message and added MCC_DTMF_TIMER_SIG signal.
08/22/92    ip     Fixes after code review and initial integration.
08/10/92    ip     Initial release for code review.
07/13/92    ip     Included CDMA signals definitions.
07/13/92    ip     Updated to include new cmd_hdr type from cmd.h.
07/06/92    ip     Initial release to PVCS.
05/05/92    ip     Initial porting of file from Brassboard to DMSS.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include "cmd.h"
#include "cai.h"
#include "dog.h"
#include "sd.h"
#include "prot.h"
#include "cm.h"

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif

/*
** When other task includes mc.h, it also includes tmc.h implicitly.
*/
#include "tmc.h"
#include "mmoc.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/* Signals are defined below.  Signals are shared between MC, MCC and      */
/* ACPMC, as shown below.  The only exception is the main command queue    */
/* signal, since this has to be known by an outside calling task.          */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*                              MC signals                                 */
/*                              ----------                                 */

#define  MC_CMD_Q_SIG         0x0001 /* Command queue signal               */
#define  MC_NV_CMD_SIG        0x0002 /* NV command response signal         */
#define  MC_RPT_TIMER_SIG     0x0004 /* Watchdog report signal             */
#define  MC_ACK_SIG           0x0008 /* State change ack signal            */
#define  MC_CMD_SIG           0x0010 /* Command response signal            */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*                            -------------                                */
/*                            ACPMC signals                                */
/*                            -------------                                */

#define ACPMC_CMD_Q_SIG        MC_CMD_Q_SIG
  /* Indicates new item on ACPMC CMD Q  */

#define ACPMC_NV_DONE_SIG      MC_NV_CMD_SIG
  /* NV command response signal         */

#define ACPMC_DOGRPT_TIMER_SIG MC_RPT_TIMER_SIG
 /* Watchdog report interval timer      */

#define ACPMC_ACPRX_MSG_Q_SIG  0x0008
  /* Indicates new item on ACPRX MSG Q  */

#define ACPMC_ACPRX_RPT_Q_SIG  0x0010
  /* Indicates new item on ACPRX RPT Q  */

#define ACPMC_ACPTX_RPT_Q_SIG  0x0020
  /* Indicates new item on ACPTX RPT Q  */

#define ACPMC_CMD_TIMER_SIG    0x0040
  /* Time to report back has expired    */
#define ACPMC_STATE_TIMER_SIG  0x0080
  /* Access timer(SA)/Release timer(VC) */

#define ACPMC_FLASH_TIMER_SIG  0x0100
  /* Active when valid flash terminates */

#define ACPMC_AUTH_Q_SIG       0x0200
  /* Indicates new item on MC_AUTH_Q */

#define ACPMC_CMD_SIG          0x0400
  /* Synchronizes Auth Commands */

#define ACPMC_REGDELAY_TIMER_SIG 0x0800
  /* Active when registration delay timer expires */

/* Currently not used */

#define ACPMC_RSVD_1000_SIG    0x1000

#ifdef FEATURE_UIM_RUIM
/* Synchronizes UIM server commands. */
#define ACPMC_UIM_Q_SIG        0x2000
#endif /* FEATURE_UIM_RUIM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*                             -----------                                 */
/*                             MCC signals                                 */
/*                             -----------                                 */

#define MCC_Q_SIG              MC_CMD_Q_SIG
  /* This signal is set when an item is placed on any MCC queue */

#define MCC_CMD_SIG            MC_NV_CMD_SIG
  /* This signal is set when either Searcher, Receive, Transmit, RXTX, NV
     or SND tasks complete processing of a command from the Main Control
     CDMA subtask */

#define MCC_RPT_TIMER_SIG      MC_RPT_TIMER_SIG
  /* This signal is set when the mc_rpt_timer expires     */

#define MCC_TXC_DONE_SIG       0x0008
  /* This signal is set by TXC when an access attempt or transition to
     Traffic Channel state is completed */

#define MCC_STATE_TIMER_SIG    0x0010
  /* This signal is set when the mc_state_timer expires   */

#define MCC_CMD_TIMER_SIG      0x0020
  /* This signal is set when the mc_cmd_timer expires     */

#define MCC_SLOTTED_TIMER_SIG  0x0040

#define MCC_CALLBACK_TIMER_SIG 0x0080
  /* This signal is set when any timer (using the REX+ timer callback) expires */

/* Currently not used */
#define MCC_RSVD_0100_SIG      0x0100
#define MCC_RSVD_0200_SIG      0x0200
#define MCC_RSVD_0400_SIG      0x0400
#define MCC_RSVD_0800_SIG      0x0800

#ifdef FEATURE_OTASP
#define MCC_OTASP_COMMIT_SIG   0x1000
#endif /* FEATURE_OTASP */

/* MCC_TXC_APHO_OK_SIG reuses OTASP Commit Signal. This can be done since */
/* both these signals are temporally exclusive                            */
#define MCC_TXC_APHO_OK_SIG   0x1000 /* This signal is set by TXC when it */
                                     /* is ok to perform APHO after it has*/
                                     /* been triggered by Srch/RXC.       */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Signals 0x2000 thru 0x8000 are system wide and defined in task.h        */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*                       -----------------                                 */
/*                       MCC queue signals                                 */
/*                       -----------------                                 */

#define MCC_CMD_Q_SIG      0x00010000 /* This signal is set when an item   */
                                      /* is placed on the mc_cmd_q         */

#define MCC_RXTX_Q_SIG     0x00020000 /* This signal is set when an item   */
                                      /* is put on the mcc_rxtx_q          */

#define MCC_SRCH_Q_SIG     0x00040000 /* This signal is set when an item   */
                                      /* is put on the mc_srch_q           */

#define MCC_RX_RPT_Q_SIG   0x00080000 /* This signal is set when an item   */
                                      /* is put on the mcc_rx_rpt_q        */

#define MCC_RXTX_ACK_Q_SIG 0x00100000 /* This signal is set when an item   */
                                      /* is put on the mcc_rxtx_ack_q      */
#ifdef FEATURE_AUTH
#define MCC_AUTH_Q_SIG     0x00200000 /* This signal is set when an item   */
                                      /* is put on the mc_auth_q           */
#endif /* FEATURE_AUTH */

#if (defined (FEATURE_DH) && defined(FEATURE_DH_EXP))
#define MCC_DH_Q_SIG       0x00400000 /* This signal is set when an item   */
                                      /* is put on the mc_dh_q           */
#endif /*FEATURE_DH */

#ifdef FEATURE_UIM_RUIM
#define MCC_UIM_Q_SIG      0x00800000 /* This signal is set when an item   */
                                      /* is put on the mc_uim_q            */
#endif /* FEATURE_UIM_RUIM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Signals 0x01000000 thru 0x80000000 are now available for REX signals    */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_IS95B_MDR
#define MCC_SCAM_TIMER_SIG         0x01000000
#endif /* FEATURE_IS95B_MDR */

#if defined(FEATURE_IS95B_MDR)
#define MCC_MDR_DURATION_TIMER_SIG 0x02000000
/* This is used for MDR supplemental channel duration timing */
#endif /* defined(FEATURE_IS95B_MDR) */

/* This signal is set by TX to notify MC to send the next access probe */
#define MCC_TXC_PROBE_SIG          0x04000000

#define MCC_TXC_FRAME_SIG          0x08000000

#define MCC_TRANS_DONE_SIG         0x20000000

/* Note, We cannot use the 0x40000000 signal as this is used
by FS FS_OP_COMPLETE_SIG,  which MC waits on during boot up when
it makes EFS function calls, on behalf of Runtime Dev Map module */

#define MCC_TXC_REV_DTX_SIG        0x80000000

/*-------------------------------------------------------------------------*/

/* The MC command free queue is declared as external here so that other    */
/* tasks can get free buffers from here before they queue them to the MC   */
/* command queue.                                                          */

extern q_type mc_cmd_free_q;

#ifdef FEATURE_AUTH
/* The auth report free queue is declared as external here so that other   */
/* tasks can get free buffers from here before they queue them to the      */
/* mc_auth_q.                                                              */

extern  q_type  mc_auth_free_q;
#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
/* The uim report free queue is declared as external here so that other    */
/* tasks can get free buffers from here before they queue them to the      */
/* mc_uim_q.                                                               */

extern  q_type  mc_uim_free_q;
#endif /* FEATURE_UIM_RUIM */

#if (defined(FEATURE_DH) && defined(FEATURE_DH_EXP))
/* The auth report free queue is declared as external here so that other   */
/* tasks can get free buffers from here before they queue them to the      */
/* mc_dh_free_q.                                                           */

extern  q_type  mc_dh_free_q;
#endif /*FEATURE_DH */

/* The queue holds a pool of free buffers to indicate timer expiration to MC task */
extern q_type  mc_expired_timers_free_q;

/* This queue has all the expired timer IDs that MC needs to process */
extern q_type  mc_expired_timers_q;

/* Timer IDs for use with callback timers (REX+) */
/* The following convention is used to split the 32-bit timer ID

Bit 31..8 : Timer Type (Action, DTMF, Tag Mgr, State, etc.)
Bit 7..0  : Sequence Number (Interpretation dependent on Timer Type in MSB 24)

*/

#define MCC_TIMER_TYPE_MASK          0xFFFFFF00
#define MCC_TIMER_SEQUENCE_MASK      0x000000FF

/* Possible Timer Type/ID values for Callback timers */
enum 
{
    /* These enums must be 32-bit values with the LSByte (Sequence) set to '0x00' */
    MCC_ACTION_TIMER_ID       = 0x00000100,
    MCC_SLOTTED_TIMER_ID      = 0x00000200,
    MCC_SO_CTL_TIMER_ID       = 0x00000300,
    MCC_SRV_TIMER_ID          = 0x00000400,
    MCC_FULL_TMSI_TIMER_ID    = 0x00000500,
    MCC_TMSI_EXP_TIMER_ID     = 0x00000600,
    MCC_SCAM_TIMER_ID         = 0x00000700,
    MCC_MDR_DURATION_TIMER_ID = 0x00000800,
    MCC_CC_DTMF_TIMER_TYPE    = 0x00000900,
    MCC_CC_SUBSTATE_TIMER_TYPE= 0x00000A00,
#ifdef FEATURE_IS2000_REL_A_SVD
    MCC_TAG_MGR_TIMER_TYPE    = 0x00000B00,
#endif
#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif
    MCC_BS_INSANITY_TIMER_ID  = 0x00000D00,
#ifdef FEATURE_SO_8026
#error code not present
#endif /* FEATURE_SO_8026 */
    MCC_T50_TIMER_ID          = 0x00000F00,
    MCC_T51_TIMER_ID          = 0x00001000,
    MCC_TR_REQUEST_TIMER_ID = 0x00002000
#ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA */
 
};
 
/*-------------------------------------------------------------------------*/

/* Macro to check for NULL Pointers */
#define MCC_CHK_NULL_PTR(ptr) \
  if(ptr == NULL){ \
     ERR_FATAL("Null mcc ptr", 0, 0, 0); \
  }

#define MCC_CHK_MAX_INDEX_RANGE(var, max_val) \
  if((var) > (max_val)) { \
    ERR_FATAL("Range Error %d %d", var, max_val, 0); \
  }

#define MCC_CHK_MIN_INDEX_RANGE(var, min_val) \
  if((var) < (min_val)) { \
    ERR_FATAL("Range Error %d %d", var, min_val, 0); \
  }

/*-------------------------------------------------------------------------*/

/* Maximum dialed digits and data characters.  These are sent as ASCII     */
/* string.                                                                 */

#define MC_MAX_DIALED_DIGITS  NV_MAX_DIAL_DIGITS
#define MC_MAX_DTMF_DIGITS    255

/*-------------------------------------------------------------------------*/
/*  P_REV values                       */

#define P_REV_JSTD008          1
#define P_REV_IS95A            3
#define P_REV_IS95B            4
#define P_REV_IS2000           6
#define P_REV_IS2000_REL_A     7
#define P_REV_IS2000_REL_B     8
#define P_REV_IS2000_REL_C     9
#define P_REV_IS2000_REL_C_MI 10
#define P_REV_IS2000_REL_D    11

#define MOB_P_REV_IS_VALID(p_rev)              \
        (((p_rev) == P_REV_JSTD008)         || \
         ((p_rev) == P_REV_IS95A)           || \
         ((p_rev) == P_REV_IS95B)           || \
         ((p_rev) == P_REV_IS2000)          || \
         ((p_rev) == P_REV_IS2000_REL_A)    || \
         ((p_rev) == P_REV_IS2000_REL_B)    || \
         ((p_rev) == P_REV_IS2000_REL_C)    || \
         ((p_rev) == P_REV_IS2000_REL_C_MI) || \
         ((p_rev) == P_REV_IS2000_REL_D))

/*-------------------------------------------------------------------------*/
/* Definitions for run-time P_REV and Band Class checking                  */

/* Example of run-time BC checking:

   if ( BAND_IS_CELLULAR )
   {
     // Handle cellular processing here
   }
   else if( BAND_IS_PCS )
   {
     // Handle PCS processing here
   }
*/

/* For P_REV checking */
#define P_REV_IS_1            (mc_get_p_rev_in_use() == 1)
#define P_REV_IS_2_OR_3       (mc_get_p_rev_in_use() == 2 || \
                               mc_get_p_rev_in_use() == 3)
#define P_REV_IS_4_OR_GREATER (mc_get_p_rev_in_use() >= 4)
#define P_REV_IS_5_OR_GREATER (mc_get_p_rev_in_use() >= 5)
#define P_REV_IS_6_OR_GREATER (mc_get_p_rev_in_use() >= 6)

#ifdef FEATURE_IS2000_REL_A
#define P_REV_IS_7_OR_GREATER (mc_get_p_rev_in_use() >= 7)
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_B
#define P_REV_IS_8_OR_GREATER (mc_get_p_rev_in_use() >= 8)
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_C
#define P_REV_IS_9_OR_GREATER (mc_get_p_rev_in_use() >= 9)
#define P_REV_IS_10_OR_GREATER (mc_get_p_rev_in_use() >= 10)
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

/* Example of run-time P_REV checking

  if( P_REV_IS_1 )
  {
    // JSTD008 processing
  }
  else if( P_REV_IS_2_OR_3 )
  {
    // IS95A processing
  }
  else if( P_REV_IS_4_OR_GREATER )
  {
    // IS95B processing
  }
*/

/*-------------------------------------------------------------------------*/

/* Commands to be sent to the Main Control task */

typedef enum 
{

  /* Enter online operation.
  */
  MC_ONLINE_F = 0,

  /* Call origination.
  */
  MC_ORIGINATION_F = 1,

  /* Answer incoming call.
  */
  MC_ANSWER_F = 2,

  /* Send flash.
  */
  MC_FLASH_F = 3,

  /* End call.
  */
  MC_END_F = 4,

  /* End call. 
  */
  MC_DATA_END_F = 5,

  /* Start continuous DTMF.
  */
  MC_START_CONT_DTMF_F = 6,

  /* Stop continuous DTMF.
  */
  MC_STOP_CONT_DTMF_F = 7,

  /* Send burst DTMF.
  */
  MC_BURST_DTMF_F = 8,

  /* Send an mobile originated data burst message.
     Currently, it includes SMS and PD messages
  */
  MC_MO_DBM_F = 9,

  /* Enter privacy mode.
  */
  MC_PRIVACY_PREF_F = 10,

  /* Preferred system changed.
  */
  MC_PREF_SYS_CHANGED_F = 11,

  /* Switched to another NAM.
  */
  MC_NAM_CHANGED_F = 12,

  /* Enter offline operation, digital mode.
  */
  MC_OFFLINE_DIGITAL_F = 13,

  /* Enter offline operation, analog mode.
  */
  MC_OFFLINE_ANALOG_F = 14,

  /* Perform a reset.
  */
  MC_RESET_F = 15,

  /* Perform power down processing.
  */
  MC_POWERDOWN_F = 16,

  /* Jump to traffic offline test command.
  */
  MC_JUMP_TRAFFIC_F = 17,

  /* Jump to access offline test command.
  */
  MC_JUMP_ACCESS_F = 18,

  /* Use only one sector, ccdma_ch, pg_ch, acc_ch.
  */
  MC_SECTOR_LOCK_F = 19,

  /* Enter Low Power Mode
  */
  MC_LPM_F = 20,

  /* acknowledge SMS messages using an AWI
  */
  MC_AWI_SMS_ACK_F = 21,

  /* command from CM to MC to enable BCSMS
  */
  MC_BCSMS_ENABLE_F = 22,

  /* Silent Redial
  */
  MC_SILENT_REDIAL_F = 23,

  /* Resource request, formerly SCRM, request from DS
  */
  MC_RES_REQ_F = 24,

  /* Hold answering a call
  */
  MC_HOLD_F = 25,

  /* command from SD to MC when the system selection timer expires
  */
  MC_SS_TIMER_EXP_F = 26,

  /* Resource release request, request from DS to go control hold
  */
  MC_RES_REL_F = 27,

  /* Data Dedicated mode access prevention cmd
  */
  MC_DDTM_CNTL_F = 28,

  /* Commands required for Multi-mode architecture.
  */

  /* MMOC command to activate CDMA protocol.
  */
  MC_CDMA_PROT_ACT_CMD_F = 29,

  /* MMOC command to deactivate CDMA protocol.
  */
  MC_CDMA_PROT_DEACT_CMD_F = 30,

  /* MMoC command to inform ph status changed.
  */
  MC_CDMA_PROT_PH_STAT_CHGD_CMD_F = 31,

  /* Generic command from MMOC.
  */
  MC_CDMA_PROT_GEN_CMD_F = 32,

  /* MMOC command to activate AMPS protocol.
  */
  MC_AMPS_PROT_ACT_CMD_F = 33,

  /* MMOC command to deactivate AMPS protocol.
  */
  MC_AMPS_PROT_DEACT_CMD_F = 34,

  /* MMoC command to inform ph status changed.
  */
  MC_AMPS_PROT_PH_STAT_CHGD_CMD_F = 35,

  /* Generic command from MMOC.
  */
  MC_AMPS_PROT_GEN_CMD_F = 36,

  /* Command used to send BSStatusReq
     to the mobile
  */
  MC_BSSTATUS_REQ_F = 37,

  /* Command used to direct CDMA callp to complete
  ** UMTS -> 1x handover.
  */
  MC_UMTS_CDMA_HANDOVER_REQ_F = 38,

  /* Command used to send MC a message tunneled from DO.
  */
  MC_TUNNEL_MSG_F = 39,

  /* Command used to inform MC that the VOIP call
     has ended.
  */
  MC_VOIP_CALL_END_CNF_F = 40,

  /* Command used to inform MC about MVS enable 
     status 
   */
  MC_MVS_ENABLE_STATUS_F = 41   

} mc_name_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of PREF_SYS change reasons.
*/
typedef enum 
{
  MC_PREF_REAS_USER,         /* User changed the SS-Preference */
  MC_PREF_REAS_ORIG_START,   /* Origination starts */
  MC_PREF_REAS_ORIG_END,     /* Origination ends */
  MC_PREF_REAS_MAX

} mc_pref_reas_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Status returned from the Main Control task */

typedef enum 
{
  MC_DONE_S,            /* General OK return */
  MC_BUSY_S,            /* Command is being processed */
  MC_BADCMD_S,          /* Invalid command */
  MC_BADPARM_S          /* Bad Parameters */

} mc_status_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Command header type.  Each command to MC starts with this header.       */

typedef struct 
{
  cmd_hdr_type       cmd_hdr;      /* Command header */
  mc_name_type       cmd;          /* Command name */
  mc_status_type     status;       /* Completion code status */

} mc_hdr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Expired Timer ID information, this is used with the "callback timers" effort */

typedef struct 
{
  q_link_type        link;
  dword              expired_timer_id;

} mc_expired_timer_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_ONLINE_F.                                                   */

typedef struct 
{
  mc_hdr_type  hdr;                   /* Header */

  byte                    nam;        /* Current NAM              */
  sd_ss_orig_mode_e_type  orig_mode;  /* Current origination mode */
  sd_ss_mode_pref_e_type  mode_pref;  /* Current mode preference  */
  sd_ss_band_pref_e_type  band_pref;  /* Current band preference  */
  sd_ss_roam_pref_e_type  roam_pref;  /* Current roam preference  */
  sd_ss_hybr_pref_e_type  hybr_pref;

} mc_online_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_ORIGINATION_F.                                              */

#ifdef FEATURE_OTASP
typedef enum 
{
  ABAND,
  BBAND,
  ABLOCK,
  BBLOCK,
  CBLOCK,
  DBLOCK,
  EBLOCK,
  FBLOCK,
  PREFERRED,

  /* Introduce Block G and Block H for Band Class 5.
  ** They are used for internal purpose to support Band Class 5
  ** There are not Block G and Block H defined in the Table 3.2-1 System
  ** Selection Code in TIA/EIA/IS683-A.
  **
  */
  GBLOCK,
  HBLOCK

} mc_activate_code_type;

/* Macro to convert Activation code to System */
/* Does not work with act_code == PREFERRED */
#define ACTIVATE_CODE_TO_SYS(act_code) ((act_code + 6)%8)
#endif /* FEATURE_OTASP */

#ifdef FEATURE_HDR_HANDOFF
#error code not present
#endif

#if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
typedef enum 
{
  ORIGINATION,    /* regular origination */
  SMS,            /* SMS origination */
  POSITION_DET,    /* PD message origination */
  SDB

} mc_orig_type_type;
#endif /* FEATURE_CDSMS || FEATURE_DBM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*
** Data type definition for Calling Party Number, Calling Party Subaddress,
** and Called Party Subaddress.
*/
#define MC_MAX_SUBADDRESS_DIGITS   21

typedef struct
{
  byte              number_type;  /* Type of number */
  byte              number_plan;  /* Numbering plan */
  cai_call_party_num_pi_type pi;  /* Presentation indicator */
  cai_call_party_num_si_type si;  /* Screening indicator */
  byte              num_char;     /* Number of characters in variable type */
  byte              chari[MC_MAX_DIALED_DIGITS];

} mc_calling_party_number_type;

typedef struct
{
  boolean                   extension_bit; /* The extension bit */
  cai_subaddress_types_type subadd_type;   /* Type of subaddress */
  cai_subaddress_eo_type    odd_even_ind;  /* The indicator of even/odd bits */
  byte                      num_char;      /* Number of characters in chari array */
  byte                      chari[MC_MAX_SUBADDRESS_DIGITS];

} mc_subaddress_type;

typedef struct 
{
  mc_hdr_type  hdr;                      /* Header                         */
  word         service_option;           /* Service option number          */

  #ifdef FEATURE_ACP
#error code not present
  #endif /* FEATURE_ACP */

  #ifdef FEATURE_DS
  boolean      ds_originated;            /* Source of Orig, UI or DS       */
  boolean      amps_data;                /* AMPS only data/voice indicator */
  #endif /* FEATURE_DS */

  boolean      digit_mode;               /* TRUE if digits have non-numeric*/
  byte         number_type;              /* Number type for ASCII digit */
  byte         number_plan;              /* Number plan for ASCII digit */
  byte         cnt;                      /* How many digits                */
  byte         called_address[MC_MAX_DIALED_DIGITS];  /* Called address */

  #ifdef FEATURE_OTASP
  boolean      otasp_activation;         /* If this is an OTASP activation */
  mc_activate_code_type activate_code;   /* Tells where to look for service*/
  #endif /* FEATURE_OTASP */

  boolean      drs;                      /* DRS (data ready to send) bit in ORM */

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif /* FEATURE_HDR_HANDOFF */

  cm_call_id_type       call_id;         /* call id to identify call instances */

  #ifdef FEATURE_IS2000_REL_A
  boolean               orig_reason;     /* Indicates if silent redial due to L2 failure. */
  byte                  orig_count;      
   /* If orig_reason is 1: Number of silent redials due to L2 failure; Else,
      Number of silent redials for the desired service option that have failed
      since the last successful connection (not implemented). */
  boolean               is_packet_data;                  /* indicates if packet data origination */
  boolean               is_dormant_packet_data;          /* indicates if dormant packet data */
  boolean               is_emergency;                    /* if this is an emergency call */
  boolean               qos_parms_incl;                  /* Qos Parameters indicator */
  byte                  qos_parms_len;
  byte                  qos_parms[CAI_ORIG_QOS_LEN_MAX];
  boolean               sr_id_included;                  /* sr_id is included */
  byte                  sr_id;                           /* sr_id */
  #endif /* FEATURE_IS2000_REL_A */

  mc_calling_party_number_type  calling_party_number;    /* Calling party number record */
  mc_subaddress_type            calling_party_subaddr;   /* Calling party subaddress record */
  mc_subaddress_type            called_party_subaddr;    /* Called Party subaddress record */

} mc_origination_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_ANSWER_F.                                                   */

typedef struct 
{
  mc_hdr_type  hdr;                          /* Header */

  #ifdef FEATURE_DS
  boolean      ds_answered;                  /* indicates DS answered call */
  boolean      a_data;                       /* indicates A-data call      */
  #endif /* FEATURE_DS */

  cm_call_id_type       call_id;

} mc_answer_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_FLASH_F.                                                    */

typedef struct 
{
  mc_hdr_type  hdr;                                   /* Header */
  byte         cnt;                                   /* How many digits */
  byte         called_address[MC_MAX_DIALED_DIGITS];  /* Dialed digits */
  cm_call_id_type       call_id;

  #ifdef FEATURE_IS2000_REL_A
  cm_flash_type_e_type flash_type;                    /* Flash type */
  /* The following fields are only relevant for emergency
  flash */
  boolean      digit_mode;               /* TRUE if digits have non-numeric*/
  byte         number_type;              /* Number type for ASCII digit */
  byte         number_plan;              /* Number plan for ASCII digit */
  #endif /* FEATURE_IS2000_REL_A */

} mc_flash_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_END_F.                                                      */

/* Limit number of call_id to the max connection number */
#define MAX_CALL_IDS    CAI_MAX_NUM_CONNECTIONS

typedef struct
{
  mc_hdr_type  hdr;                                   /* Header */
  byte         num_call_ids;            /* number of call ids */
  byte         call_ids[MAX_CALL_IDS];   /* call id to identify call instances */
  byte         end_reasons[MAX_CALL_IDS];  /* the reason for ending a call */

} mc_end_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_START_CONT_DTMF_F.                                       */

typedef struct 
{
  mc_hdr_type  hdr;                    /* Header */
  byte         digit;                  /* DTMF digit (tone) to send */
  cm_call_id_type       call_id;

} mc_start_cont_dtmf_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_STOP_CONT_DTMF_F.                                           */

typedef struct 
{
  mc_hdr_type hdr;          /* Header */
  word        ms;           /* Duration of DTMF in milliseconds */
  cm_call_id_type       call_id;

} mc_stop_cont_dtmf_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_BURST_DTMF_F                                                */

typedef struct 
{
  mc_hdr_type hdr;          /* Header */
  byte        on_length;    /* DTMF pulse width
                               (see IS-95A Table 6.7.2.3.2.7-1) */
  byte        off_length;   /* DTMF interdigit interval
                               (see IS-95A Table 6.7.2.3.2.7-2) */
  byte        cnt;          /* Digit count */
  byte        digits[MC_MAX_DTMF_DIGITS]; /* DTMF digits */
  cm_call_id_type       call_id;

} mc_burst_dtmf_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_CDSMS
/* Type for MC_AWI_SMS_ACK_F.                                  */
typedef struct 
{
  mc_hdr_type              hdr;            /* Header           */
  boolean                  bf;             /* begin/final flag */
  byte                     error_class;    /* IS-41 error class*/
  byte                     status;         /* IS-41 error type */
  byte                     seq_no;         /* Current sequence number */

} mc_awi_sms_ack_type;
#endif /* FEATURE_CDSMS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_MO_DBM_F.                                       */

typedef struct 
{
  mc_hdr_type  hdr;        /* Header           */
  #if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
  uint8         burst_type; /* Data Burst Type. Currently, it
                              supports SMS and PD only. */
  boolean      ack_req;    /* L2 ACK required  */
  void         *dbm_buf_ptr;
                           /* Actual data is in a static buffer
                              to save space */
  #endif /* FEATURE_CDSMS || FEATURE_DBM */

} mc_mo_dbm_type;


#ifdef FEATURE_BROADCAST_SMS
typedef struct
{
  mc_hdr_type   hdr;
  boolean       enable;

} mc_bcsms_enable_type;
#endif /* FEATURE_BROADCAST_SMS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_IS2000_REL_A_BSID
/* Type for MC_BSSTATUS_REQ.*/

typedef struct 
{
  mc_hdr_type  hdr;        /* Header */
  uint16       num_pilots;
  word         pilot_pn[CAI_BSS_NUM_PILOTS];

} mc_bsstatus_req_type;
#endif /*FEATURE_IS2000_REL_A_BSID*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_PRIVACY_PREF_F.                                             */

typedef enum 
{
  MC_LONG_CODE_PUBLIC,
  MC_LONG_CODE_PRIVATE

} mc_long_code_e_type;

typedef struct 
{
  mc_hdr_type  hdr;                         /* Header */
  mc_long_code_e_type long_code;            /* On or Off */

} mc_privacy_mode_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_PREF_SYS_CHANGED_F.                                         */

typedef struct {
  mc_hdr_type  hdr;           /* Header */
  sd_ss_pref_reas_e_type  pref_reas;    /* Reason for sending this command */
  sd_ss_orig_mode_e_type  orig_mode;    /* New origination mode        */
  sd_ss_mode_pref_e_type  mode_pref;    /* New mode preference         */
  sd_ss_band_pref_e_type  band_pref;    /* New band preference         */
  sd_ss_roam_pref_e_type  roam_pref;    /* New roam preference         */
  sd_ss_hybr_pref_e_type  hybr_pref;
  sd_band_e_type          otasp_band;   /* OTASP band-class            */
  sd_blksys_e_type        otasp_blksys; /* OTASP PCS block/cell-system */
  sd_ss_avoid_sys_e_type  avoid_type;   /* Avoid type of the system    */
  dword                   avoid_time;   /* Avoid time in secs          */

} mc_pref_sys_changed_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_NAM_CHANGED_F.                                              */

typedef struct 
{
  mc_hdr_type  hdr;                   /* Header */
  byte         nam;                   /* New NAM */

  sd_ss_orig_mode_e_type  orig_mode;  /* New origination mode */
  sd_ss_mode_pref_e_type  mode_pref;  /* New mode preference  */
  sd_ss_band_pref_e_type  band_pref;  /* New band preference  */
  sd_ss_roam_pref_e_type  roam_pref;  /* New roam preference  */
  sd_ss_hybr_pref_e_type  hybr_pref;

} mc_nam_changed_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_OFFLINE_F.                                                  */

typedef struct
{
  mc_hdr_type  hdr;                             /* Header */

} mc_offline_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_OFFLINE_ANALOG_F.                                           */

typedef struct 
{
  mc_hdr_type  hdr;                             /* Header */

} mc_offline_analog_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_OFFLINE_DIGITAL_F.                                          */

typedef struct 
{
  mc_hdr_type  hdr;                             /* Header */

} mc_offline_digital_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_RESET_F.                                                    */

typedef struct 
{
  mc_hdr_type  hdr;                             /* Header */

} mc_reset_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_POWERDOWN_F.                                                */

typedef struct 
{
  mc_hdr_type  hdr;                             /* Header */

} mc_powerdown_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_JUMP_TRAFFIC_F.                                             */

typedef struct {
  mc_hdr_type  hdr;                                   /* Header */
  dword esn;             /* Electronic serial number */
  byte code_chan;        /* Code Channel */
  word cdma_freq;        /* Frequency Assignment for the CDMA channel */
  byte frame_offset;     /* Frame offset */
  byte encrypt_mode;     /* Message encryption mode */
  word so;               /* Service option to use */
  byte mar_rate;         /* Rate for Markov SO Call */

} mc_jump_traffic_type;


/* Type for MC_JUMP_ACCESS_F.                                             */

typedef struct {
  mc_hdr_type  hdr;                                   /* Header */

  word          len;              /* Number of bytes in message */
  boolean       req;              /* True: access request, False: response */
  byte          seq_num;          /* Max number of access probe sequences */

  /* set the next 3 parameters all to zero to get a long code mask
  ** of ZERO which implies no scrambling.
  */
  byte          pagech;           /* Current Paging Channel */
  word          base_id;          /* Base station identification */
  word          pilot_pn;         /* Pilot PN sequence offset index */

  byte          pam_sz;           /* Access Channel preamble length */
  byte          max_cap_sz;       /* Max Access Channel msg capsule size */
  byte          bkoff;            /* Probe sequence backoff range */
  byte          probe_bkoff;      /* Probe backoff range */
  byte          acc_tmo;          /* Acknowledgement timeout */
  byte          init_pwr;         /* Initial power offset for access */
  byte          pwr_step;         /* Power increment */
  byte          num_step;         /* Number of access probes */
  byte          acc_chan;         /* Number of access channels */
  word          rn;      /* Random number of chips backoff - ignored now */
  dword         p;                /* Persistence probability */

  byte          *msg_ptr;    /* pointer to message buffer */

} mc_jump_access_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for MC_SECTOR_LOCK_F.                                             */

typedef struct 
{
  mc_hdr_type  hdr;                                   /* Header */
  word pn_offset;        /* Pilot PN to acquire */
  word cdma_chan;        /* Frequency Assignment for the CDMA channel */
  byte paging_chan;      /* Paging channel to use */
  byte access_chan;      /* Access channel to use */

} mc_sector_lock_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of system roam status.
**
** Note that the roaming indicator values (DB_ROAM_OFF and DB_ROAM_ON) that
** are defined in db.h are swapped compare to the PRL roaming indicator
** values that are defined in TSB-58.
*/
typedef enum 
{

  MC_ROAM_OFF,        //= DB_ROAM_OFF,       /* Roaming indicator is OFF */
  MC_ROAM_ON,         //= DB_ROAM_ON,        /* Roaming indicator is ON */
  MC_ROAM_FLASH,      //= DB_ROAM_BLINK,     /* Roaming indicator is flashing */

  MC_ROAM_MAX        = 0xFF

} mc_roam_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_PASSPORT_MODE
/* Type for MC_HOLD_F */
typedef struct {
  mc_hdr_type hdr;
  byte call_id;

} mc_hold_type;
#endif /* FEATURE_PASSPORT_MODE */

/*-------------------------------------------------------------------------*/

/* A type that associates the parameters in the Activate protocol
** command.
*/
typedef struct
{
  /* Header information.
  */
  mc_hdr_type                          hdr;

  /* Activate protocol information.
  */
  prot_act_s_type                      info;

} mc_prot_act_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type that associates the parameters in the Dectivate protocol
** command.
*/
typedef struct
{
  /* Header information.
  */
  mc_hdr_type                          hdr;

  /* Dectivate protocol information.
  */
  prot_deact_s_type                    info;

} mc_prot_deact_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type that associates the parameters in the phone status changed
** command.
*/
typedef struct
{
  /* Header information.
  */
  mc_hdr_type                          hdr;

  /* Phone status changed information.
  */
  prot_ph_stat_chgd_s_type             info;

} mc_prot_ph_stat_chgd_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type that associates the parameters in the generic protocol
** command.
*/
typedef struct
{
  /* Header information.
  */
  mc_hdr_type                          hdr;

  /* Generic protocol command information.
  */
  prot_gen_cmd_s_type                  info;

} mc_prot_gen_cmd_s_type;

/*-------------------------------------------------------------------------*/

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

/* Type for MC_MVS_ENABLE_STATUS_F. */ 

/* Enum is defined with possible MVS enable status types that can sent from 
** MVS task to MC. */
typedef enum
{
    MVS_ENABLE_SUCCESS,
    MVS_ENABLE_FAIL
}mc_mvs_enable_enum_type;

/* Structure is defined for command MC_MVS_ENABLE_STATUS_F 
** enable status command to MC from MVS task. 
 */

typedef struct
{
  /* Header information
   */
  mc_hdr_type  hdr; 

  /* MVS enable status 
   */
  mc_mvs_enable_enum_type enable_status;
}mc_mvs_enable_type;

/*-------------------------------------------------------------------------*/

/* Union of all command types.  The header is always present and it        */
/* specifies the command type and attributes.  The command messages        */
/* follow in the union.                                                    */

typedef union 
{
  mc_hdr_type                 hdr;
  mc_online_type              online;
  mc_origination_type         origination;
  mc_answer_type              answer;
  mc_flash_type               flash;
  mc_end_type                 end;
  mc_start_cont_dtmf_type     start_cont_dtmf;
  mc_stop_cont_dtmf_type      stop_cont_dtmf;
  mc_burst_dtmf_type          burst_dtmf;
  mc_mo_dbm_type              mo_dbm;
  mc_privacy_mode_type        privacy_mode;
  mc_pref_sys_changed_type    pref_sys_changed;
  mc_nam_changed_type         nam_changed;
  mc_offline_type             offline;
  mc_offline_analog_type      offline_analog;
  mc_offline_digital_type     offline_digital;
  mc_reset_type               reset;
  mc_powerdown_type           powerdown;
  mc_jump_traffic_type        jump_traffic;
  mc_jump_access_type         jump_access;
  mc_sector_lock_type         sector_lock;

  #ifdef FEATURE_CDSMS
  mc_awi_sms_ack_type         awi_ack;
  #endif /* FEATURE_CDSMS */

  #ifdef FEATURE_BROADCAST_SMS
  mc_bcsms_enable_type        bcsms_enable;
  #endif /* FEATURE_BROADCAST_SMS */

  #ifdef FEATURE_PASSPORT_MODE
  mc_hold_type                hold;
  #endif /* FEATURE_PASSPORT_MODE */

  #ifdef FEATURE_IS2000_REL_A_BSID
  mc_bsstatus_req_type        bssrequest;
  #endif /*FEATURE_IS2000_REL_A_BSID*/

  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */

  /* Activate protocol command. */
  mc_prot_act_s_type           act_prot;

  /* Deactivate protocol command. */
  mc_prot_deact_s_type         deact_prot;

  /* Generic protocol command. */
  mc_prot_gen_cmd_s_type       gen_cmd;

  /* Phone status changed command. */
  mc_prot_ph_stat_chgd_s_type  ph_stat_cmd;

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
  #endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

  #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
  #endif

  /* MVS Enable status command from MVS status */
  mc_mvs_enable_type         mvs_enable;
  
} mc_msg_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumerations for event report                                           */

/* Operation modes */
typedef enum
{
  MC_OP_MODE_POWERUP, /* Power up mode */
  MC_OP_MODE_AMPS,    /* Analog operation mode */
  MC_OP_MODE_CDMA     /* CDMA operation mode */
} mc_event_op_mode_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MC_QUEUE_WATERMARK
/* Enumerations for MC Buffer Watermark                                    */
typedef enum
{
  MCWM_CMD_FREE_Q   = 0,
  MCWM_RXTX_FREE_Q,
  MCWM_RX_FREE_Q,
  MCWM_TRANS_FREE_Q,
  MCWM_MAX_NUM_WM
} mc_wm_q_type;

typedef struct
{
  mc_wm_q_type  wm_q_name;
  q_type        *q_ptr;
  byte          max_alloc;  /* Maximum number of buffers allocated   */
  byte          max_use;    /* Maximum number of buffers used so far */
  byte          nv_value;   /* Maximum number of buffers used so far */
                            /* as stored in NV                       */
} mc_watermark_type;
#endif /* FEATURE_MC_QUEUE_WATERMARK */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of mc_queue_cmd status. */

typedef enum {

  MC_FREE_QUEUE_EMPTY,      /* No MC command links available */
  MC_CMD_QUEUED             /* The requested MC command has been queued */

} mc_queue_cmd_status_type;


/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MC_TASK

DESCRIPTION
  This is the MC task entry point.

DEPENDENCIES
  None.

RETURN VALUE
  None.  This function never returns.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mc_task
(
  dword param   /* Entry paramater, not used. */
);

/*===========================================================================

FUNCTION MC_CMD

DESCRIPTION
  This is the outside world's interface to the main control task.
  This function takes a filled out mc_msg_type which it places
  on the mc command queue, it marks the request as busy in the status
  field, it signals the mc task and then it returns to the caller.
  Return to the caller does not mean that the command has been executed
  yet, only that it is queued for execution.  When mc is done processing
  the request it returns the buffer to the specified queue if a return
  queue adress is provided and it signals the calling task if specified.

  An example MC origination request is as follows:

  mc_msg_type  msg_buf;                       - Define message buffer
  byte         digits[MC_MAX_DIALED_DIGITS]   - Called address buffer

  msg_buf.hdr.cmd = MC_ORIGINATION_F;         - Origination request
  msg_buf.hdr.task_ptr = rex_self();          - Caller's TCB
  msg_buf.hdr.sigs =    _MC_SIG;              - Caller's end signal
  msg_buf.hdr.done_q_ptr = NULL;              - No buffer return

  msg_buf.msg.origination.cnt = sizeof(called_address);
  strncpy (msg_buf.msg.origination.called_address, digits, sizeof(digits));

  rex_clr_sigs (rex_self(), XXX_MC_SIG);      - Clear the signal
  mc_cmd (&msg_buf);                          - Issue the command

DEPENDENCIES
  None.

RETURN VALUE
  None directly.  The status variable of the mc_hdr_type will be updated
  to reflect the current status of this command as it is processed.  In the
  above example msg_buf.origination.hdr.status is set to:

  MC_DONE_S             - General OK return
  MC_BUSY_S             - Command is being processed
  MC_BADCMD_S           - Invalid command
  MC_BADPARM_S          - Bad Parameters

SIDE EFFECTS
  The msg_buf is placed on the MC command queue.  It must not be modified
  until the command has been processed and its status changed to MC_DONE_S.

===========================================================================*/

extern void mc_cmd
(
  mc_msg_type *msg_ptr  /* Pointer to mc cmd message */
);


/*===========================================================================

FUNCTION MC_QUEUE_CMD

DESCRIPTION
  This is the outside world's interface to the MC task and subtasks.
  This function will unqueue an MC command from the mc_cmd_free_q and 
  fill it out and places it on the mc command queue.  The function 
  returns to the caller after the command has been executed.

DEPENDENCIES
  The MC task must have been started up already.  

RETURN VALUE
  mc_queue_cmd_status_type  
  
SIDE EFFECTS
  None.

===========================================================================*/

mc_queue_cmd_status_type  mc_queue_cmd
(
  mc_name_type cmd_type
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mc_queue_cmd */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* <EJECT> */
/*===========================================================================

FUNCTION MC_STATE

DESCRIPTION
  This function returns the current state of MC.

DEPENDENCIES
  None.

RETURN VALUE
  Current state of MC.

SIDE EFFECTS
  None.

===========================================================================*/

extern byte mc_state( void );
/*===========================================================================

FUNCTION MC_GET_BAND_CLASS

DESCRIPTION
  This function returns the current operating band class of the mobile.

DEPENDENCIES
  None.

RETURN VALUE
  Current band class.

SIDE EFFECTS
  None.

===========================================================================*/

extern byte mc_get_band_class(void);

/*===========================================================================

FUNCTION MC_GET_P_REV_IN_USE

DESCRIPTION
  This function returns the current P_REV_IN_USE of the mobile.

DEPENDENCIES
  None.

RETURN VALUE
  Current P_REV_IN_USE.

SIDE EFFECTS
  None.

===========================================================================*/

extern byte mc_get_p_rev_in_use(void);

/*===========================================================================

FUNCTION MC_GET_MOB_CAI_REV

DESCRIPTION
  This function returns the protocol revision number of the mobile station.

DEPENDENCIES
  None.

RETURN VALUE
  The mobile's protocol revision number.

SIDE EFFECTS
  None.

===========================================================================*/

extern byte mc_get_mob_cai_rev(void);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mc_get_p_rev_in_use */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION MC_INFORM_DIAG_OFFLINE_COMPLETE

DESCRIPTION
  This function first updates the last CDMA / Analog entry reason which will
  be referred by Diag based on the current CDMA / Analog entry reason. It
  then signals DIAG task that MC is ready to begin offline operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_inform_diag_offline_complete(void);

/*===========================================================================

FUNCTION MC_REPORT_EVENT_OP_MODE

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

extern void mc_report_event_op_mode
(
  mc_event_op_mode_type new_op_mode
);

#ifdef FEATURE_MC_QUEUE_WATERMARK
/*===========================================================================

FUNCTION MC_INIT_WATERMARK

DESCRIPTION
  This function retrieves the MC queue watermark history values from NV,
  and initializes the queue watermark variable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mc_init_watermark(void);

/*===========================================================================

FUNCTION MC_UPDATE_WATERMARK

DESCRIPTION
  This function checks the MC's queue watermark against the history value.
  It updates the watermark and if necessary, store it into NV.

DEPENDENCIES
  MC's queue watermark has to be retrieved from NV upon powerup.

RETURN VALUE
  None

SIDE EFFECTS
  It may perform a NV write operation, which sometimes is slow.

===========================================================================*/

extern void mc_update_watermark
(
  mc_wm_q_type  q_name
);

/*===========================================================================

FUNCTION MC_STORE_WATERMARK

DESCRIPTION
  This function does a final check of the MC's queue watermark against the
  NV history value, and updates it if necessary. Note it does write to NV.

DEPENDENCIES
  MC's queue watermark has to be retrieved from NV upon powerup.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mc_store_watermark(void);
#endif /* FEATURE_MC_QUEUE_WATERMARK */

/*===========================================================================
FUNCTION mc_cdma_activate_protocol

DESCRIPTION
  This function activates the CDMA protocol to perform the operation
  specified in the act_reason parameter, could be ONLINE_ACQ, OFFLINE, FTM
  etc.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

extern void  mc_cdma_activate_protocol
(
  prot_act_e_type  act_reason
    /* Reason for activating the CDMA protocol. */
);

/*===========================================================================
FUNCTION mc_cdma_deactivate_protocol

DESCRIPTION
  This function deactivates the CDMA protocol.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mc_cdma_deactivate_protocol
(
  prot_deact_e_type deact_reason,
    /* Reason for deactivating the CDMA protocol. example, PWR_DOWN,
    ** RESET etc.
    */

  prot_trans_type trans_id
    /* Transaction Id for this request. */
);

/*===========================================================================
FUNCTION mc_cdma_ph_status_chgd_cmd

DESCRIPTION
  This function should be called to inform the CDMA protocol of the change
  in phone status, the protocol will initiate the housekeeping tasks if
  necessary.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mc_cdma_ph_status_chgd_cmd
(
  prot_ph_stat_chg_e_type chg_type,
    /* Type of phone status change( NAM change, operating mode change
    ** etc.)
    */

  byte nam,
    /* Current NAM selected by the user. */

  boolean is_cdma_subs_avail,
    /* Current CDMA subscription availability status. */

  prot_oprt_mode_e_type oprt_mode,
    /* Current operating mode of the phone. */

  prot_trans_type trans_id,
    /* Transaction Id for this request. */

  prot_subsc_chg_e_type prot_subsc_chg
    /* Enumeration of protocols that have a change in
    ** subscription available status
    */
);

/*===========================================================================
FUNCTION mc_cdma_generic_prot_cmd

DESCRIPTION
  This function should be used to command the CDMA protocol stack to process
  the generic command from MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mc_cdma_generic_prot_cmd
(
  prot_gen_cmd_s_type *gen_cmd_ptr
    /* Pointer to the generic command information. */
);

/*===========================================================================
FUNCTION mc_amps_activate_protocol

DESCRIPTION
  This function activates the AMPS protocol to perform the operation
  specified in the act_reason parameter, could be ONLINE_ACQ, OFFLINE, FTM
  etc.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mc_amps_activate_protocol
(
  prot_act_e_type act_reason
    /* Reason for activating the AMPS protocol. */

);

/*===========================================================================
FUNCTION mc_amps_deactivate_protocol

DESCRIPTION
  This function deactivates the AMPS protocol.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mc_amps_deactivate_protocol
(
  prot_deact_e_type deact_reason,
    /* Reason for deactivating the AMPS protocol. example, PWR_DOWN,
    ** RESET etc.
    */

  prot_trans_type trans_id
    /* Transaction Id for this request. */
);

/*===========================================================================
FUNCTION mc_amps_ph_status_chgd_cmd

DESCRIPTION
  This function should be called to inform the AMPS protocol of the change
  in phone status, the protocol will initiate the housekeeping tasks if
  necessary.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mc_amps_ph_status_chgd_cmd
(
  prot_ph_stat_chg_e_type chg_type,
    /* Type of phone status change( NAM change, operating mode change
    ** etc.)
    */

  byte nam,
    /* Current NAM selected by the user. */

  boolean is_cdma_subs_avail,
    /* Current CDMA subscription availability status. */

  prot_oprt_mode_e_type oprt_mode,
    /* Current operating mode of the phone. */

  prot_trans_type trans_id,
    /* Transaction Id for this request. */

  prot_subsc_chg_e_type prot_subsc_chg
    /* Enumeration of protocols that have a change in
    ** subscription available status
    */
);

/*===========================================================================
FUNCTION mc_amps_generic_prot_cmd

DESCRIPTION
  This function should be used to command the AMPS protocol stack to process
  the generic command from MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mc_amps_generic_prot_cmd
(
  prot_gen_cmd_s_type *gen_cmd_ptr
    /* Pointer to the generic command information. */
);

/*===========================================================================

FUNCTION MC_CDMA_MULTIMODE_SANITY_ERR_FATAL

DESCRIPTION
   The function to call the err_fatal because of the reason passed in.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mc_cdma_multimode_sanity_err_fatal
(
  mmoc_sanity_err_e_type reason
);

/*===========================================================================

FUNCTION MC_AMPS_MULTIMODE_SANITY_ERR_FATAL

DESCRIPTION
   The function to call the err_fatal because of the reason passed in.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mc_amps_multimode_sanity_err_fatal
(
  mmoc_sanity_err_e_type  reason
);

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

#endif /* MC_H */

