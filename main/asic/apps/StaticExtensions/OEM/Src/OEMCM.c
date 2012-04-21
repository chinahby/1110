/*=============================================================================

FILE: OEMCM.c

SERVICES: OEM Functions for Call Manager Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2003-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMCM.c#231 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
09/14/08     kcp     Allocate memory for NULL char
07/09/08     sg      Handle CM_SS_EVENT_INFO in OEMCM_BREWSSEventCB instead of
                     OEMCM_DMSSSSEventCB
04/18/08     yc      Do not response the call setup indicator event if the call
                     is SCUDIF call.
04/15/08     cvs     Add support for AMR WB
04/14/08     cvs     fix compile warning
04/10/08     cvs     Add support for AMR WB
03/26/08     cvs     Remove deprecated functions
03/10/08     sg      Update the RSSI bars using new UI signal strength indicator
                     algorithm
03/04/08     clm     Adjust mm_info julian time zone info for DST for OEMCM_HandleSsMobilityMgmt()
11/08/07     alb     Remove CM_API_MBMS ifdefs
11/02/07     alb     Added MBMS API support
10/05/07     ypmw    Fix the CNAP alpha string display
10/03/07     alb     Convert the AEECM call id to CM call id in SendFlashSupps
10/01/07     sg      Fix some more potential memory leak cases
09/21/07     ypmw    Fix the gsm conversion for CNAP string
08/06/07     alb     API for editing PRL records added.
08/17/07     jks     Fix memory leak
06/19/07     alb     Copy the alpha from cm that comes with the incoming call.
05/07/07     cvs     Fix lint criticals
04/12/07     ypmw    Fix compiler error
04/10/07     ypmw    Fix the day light saving time adjustment
03/26/07     ypmw    Add EAP authentication scheme for automatic wlan mode
03/26/07     cvs     Fix compile warning
03/22/07     cvs     Don't add to call history in HDK builds
03/20/07     jas/ak  Adding support for persistent preference values.
01/10/07     jks     Added support for VoIP ringback (remote alert)
12/11/06     ak      Connected number not being shown in IP calls
12/04/06     ypmw    Fix wlan compiler error
11/29/06     ypmw    Fix wlan compiler warnings
11/28/06     cvs     Fix TTY bearer capabilities for devices detected as Stereo
11/22/06     ak      Release the call if Release record is sent from n/w
11/17/06     ak      Display Protocol Revision for HDR
11/15/06     ak      Use the correct paramater for the ascii conversion function
11/14/06     ypmw    Fix wlan compiler warnings
11/01/06     ypmw    Add SSID for start adhoc network
09/27/06     jas     Fixing compiler warnings on RVCT
09/26/06     ak      Only send flash for CDMA calls. Set Default value for TTY variable
09/22/06     jas     Adding some debug information
09/20/06     jas     Fixing featurization from prior checkin
09/19/06     jas     Adding band value to AEECMWLANPref structure.
09/10/06     ypmw    Fix the error display message when it is in invalid_sim_state
08/23/06     jks     Added support for WLAN Adhoc networks
07/19/06     ak      Send the network type other than automatic for WLAN
07/14/06     ak      Merged ACM changes originally made by ACM feature team
07/13/06     alb     Fix compile warnings for 7500 targets.
06/28/06     cvs     Don't alloc call ID's in error cb
06/12/06     jks     Ported UsrSysClock to new BREW ISYSCLOCK interface
05/23/06     cvs     Fix race condition / remove duplicate code
05/03/06     cvs     Don't wake up for RSSI in apps power collapse.
04/24/06     alb     Change to handle 80 character dial strings plus the '+'
04/18/06     alb     BCMCS annunciator support.
04/17/06     alb     Remove handling of AEECM_EVENT_SS_CELL_SUPP_SERVICES_IND,
                     HSDPA indication now uses CM_SS_EVENT_SRV_CHANGED.
03/23/06     ak      Added User Mangling Changes for Stride
03/23/06     alb     Protect handling of AEECM_EVENT_SS_CELL_SUPP_SERVICES_IND
                     under both FEATURE_HSDPA and CM_API_CELL_SUPP_SERVICES.
03/22/06     jas     Fixing lint directives
03/21/06     alb     Added handling of AEECM_EVENT_SS_CELL_SUPP_SERVICES_IND.
03/21/06     jas     Add conversions for some band pref values
03/17/06     alb     Removed line of code which overrides the cm end_status
                     so that the correct end status gets propagated to ICM.
                     Resolved lint errors.
03/14/06     jks     Removed WLAN profile_id (obsolete)
03/03/06     jks     Added call event AEECM_EVENT_CALL_ALL_CALLS_ENDED
02/27/06     ak      Added Debug statements for mem heap debugging
02/22/06     sj      Copied hdrhybrid info into the AEECMSSInfo structure
01/24/06     jks     Fixed WLAN tech to tech pref translation
01/20/06     RI      Added handling of Call Hold/Retrieve events from CM.
01/19/06     cvs     support for emergency call categories
01/04/06     jks     Fixed SSID truncation
12/05/05     jks     Added null termination to SSID string copy
11/15/05     bpw     Fixed internal call id max and suppress get info error callback.
11/10/05     JAS     Fixing compiler warning
11/03/05     RI      Modified the processing of Manage Calls Conf when
                     an error is received.
08/29/05     RI      Updated the processing of MM info.
08/18/05     jas     Fixing handling of WLAN authentication data
08/08/05     RI      Fixed featurization of MM info processing for GSM/WCDMA modes.
08/01/05     SUN     Removed unnecessary check from OEMCM_HandleCallEnd
07/28/05     RI      Changes as per new interface definitions for USSD in
                     Call Manager.
07/27/05     cvs     Code Review cleanups
07/20/05     AT      Added new API to set the subscription_status as available
                     not available for all technologies in one CM commands.
07/15/05     AT      Added API to Reset Origination Throttling.
07/11/05     aps     Added PI field lookup when Flashed W/ Info frm BS.
07/07/05     cvs     Register OnExit callback in system context
07/06/05     jas     Adding case for "no authentication" when copying WLAN info
07/01/05     jas     WLAN featurization fix
06/27/05     sun     Play the QTV again if call ends prematurely
06/22/05      AT     When originating a call, ensure the number length
                     is appropriate.
06/22/05     jas     Adding support for WLAN RSSI
                     Changing TLS security to TLS Certificate
06/15/05     sun     CCBS can be activated even there is no call up
06/14/05     sun     Fixed call id value and do not handle setup indication for
                     calls other than of type voice.
06/09/05     sun     Code Review comments.
06/07/05     sun     Added support for CCBS
06/03/05     sun     Added support for incoming voice call during
                     Streaming Video
05/26/05     cvs     support for PBM API changes to make APIs more multiprocessor friendly05/13/05     JAS/AT  Modifying 8021X id/pw handling to match CM changes
05/31/05     aps     1<send> & 1x<send> flash with info fix in CDMA mode
06/02/05     JAS     Add more WLAN fields to AEECMPhInfo and AEECMSSInfo
05/13/05     JAS/AT  Modifying 8021X id/pw handling to match CM changes
05/12/05     jas     Change net_list_copy_func to cm_ph_net_list_copy calls
05/03/05     CH      Making changes to remove Bridle for BREW 3.1.4
04/28/05     sun     Fixed ALS range
04/20/05     JAS     Modifying handling of WLAN subscription info
04/19/05     JAS     Band value in BSS list is class, not pref
04/15/05     RI      Featurized TTY for GSM/WCDMA.
04/13/05     sun     Get the Call Mode Info from CM for call related events
04/06/05     RI      Support for TTY.
03/29/05     jas     Corrected copy of WLAN tech value in OEMCM_CopyInBSSList
03/22/05     jas     Support for conversion to 32-bit band preferences
03/18/05     JAS     Added support for WLAN stats
03/18/05     cvs     Add interface to SINR values
03/15/05     ak      Change in Signature since an arg is output
03/10/05     ak      Added CPHS EONS Support
03/14/05     JAS     Fixing copy of SSID from CM to ICM structure
03/07/05     JAS     Added support for WLAN stats
03/04/05     JAS     Enabled explicit WLAN subscription changes
02/22/05     JAS     Linting with WLAN content.
02/22/05     jas     Removing feature defines for CM changes for WLAN
02/18/05     sun     Added Handler for CM_CALL_EVENT_LINE_CTRL
02/14/05     sun     Added Support for Ending CS Data Calls
01/17/05     sun     Register for all CM events
01/11/05     sun     NPI should be set to ISDN
01/12/05     AT      Renumbered the call IDs per GSM spec.
01/07/05     sun     Fixed Compilation Warnings
12/14/04     AT      Change to fill in call timer information.
12/03/04     AT      Fill in the WLAN preferences.
11/15/04     SUN     Do not check mode while waking up CM.
11/09/04     JAS     Use BREW extended number instead of QCT (BREW 3.1.2.7)
11/02/04     SUN     Hold/Retrieve only Voice Calls
10/01/04     SUN     Handle all CM Ph error cases
09/23/04     SUN     Merged Changed for Brew 3.1
09/27/04     SUN     Handle CM_PH_EVENT_TERMINATE_GET_NETWORKS from CM
09/14/04     SUN     Do not change call state on notifications
09/03/04     SUN     Added Support for ALS
08/20/04     PA      Added support for new cm_ph_cmd_sys_sel_pref API,
                     removed reference to CM_CALL_EVENT_PS_DATA_AVAILABLE,
                     fix featurization.
08/04/04     PA      Release ICMNotifier object on BREW exit.
07/28/04     PA      Set bearer capability present to TRUE
07/27/04     PA      Check AMR setting in NV before sending setup response,
                     Do not allow call origination if another voice call
                     origination is pending.
07/26/04     PA      Prevent multiple answer call on the same call.
07/12/04     PA      Fix call id assignment, add parameter checks in utility fn
07/09/04     PA      Fix for flash using NULL string
07/08/04     PA      Handle emergency flash and added utility functions for
                     call origination.
06/30/04     PA      Fix compilation error for Jaguar and added check for valid
                     system mode before copying SS info.
06/28/04     PA      Make AEESysObject first field and featurize for brew 3.0
06/25/04     PA      Fix issue with calling helper functions during call error
                     notification.
06/18/04     PA      Link ICM object as a system object
06/15/04     PA      BCD to binary conversion of MMInfo is already done by NAS.
06/03/04     PA      Added error for invalid SIM state, add call history
                     for video calls and translate year data from BCD to Julian
05/24/04     PA      Added checks for changing operating mode.
05/03/04     PA      Handle change in CM enums
04/30/04     PA      Pass stricter saber linting
04/29/04     SUN     Converted PLMN Name from Default to Ascii
04/29/04     PA      Reflect change in CM number type enumeration.
04/14/04     PA      Provide data call statistics in call info.
                     Save data call counters to NV at the end of a GW data call.
                     Support for FEATURE_DS_END_KEY_DROPS_CALL.
                     Cleanup of CM call id vs AEE call id.
                     Reset AEECallback cancel function before scheduling it.
                     Moved type and macro definitions to a separate header file.
03/26/04     PA      Pass on '+' in dialed string to CM.
03/25/04     PA      Check if call id returned from CM is invalid before using
                     it. Merge OriginateVoiceCall and OriginateCall into one.
03/17/04     PA      Set subscription status to FALSE before changing
                     RTRE config. Reflect change in cm_ph_cmd_ddtm_pref.
03/16/03     PA      Send END_REQ to Apps
02/27/03     PA      Using NUMBER_EXT field in callhistory instead of NUMBER,
                     Updated check for call id in answer call, Not filling in
                     calling number to avoid CM confusion.
02/11/03     PA      Change bearer capability based on AMR settings
                     Update call info on CALLER_ID event.
02/10/03     PA      Find a valid call id for cm_mm_call_cmd_privacy_pref
02/05/03     PA      Fixed range check for RTRE config
01/22/03     PA      Fixed call id for call deflection, removed skipping
                     over '+' in dialed string.
01/16/03     PA      Change state for error in call origination, Skip over
                     invalid dialed characters, Suppress errors from PD client,
                     Fixed check in answer call
01/09/04     SUN     Set the DRS bit to TRUE
01/09/04     PA      Changed default NPI in GSM/WCDMA to ISDN.
01/07/04     SUN     Added Call id for Call Deflection
12/10/03     PA      Changed international number handling, Signal handling,
                     check CUG while originating calls and other fixes.
12/05/03     PA      Fixed lint warnings for Jaguar.
12/04/03     PA      Changed digit mode in call origination and
                     fixed lint warnings.
12/01/03     ram     Fix HDR RSSI annunciators.
12/01/03     PA      Featurize AEE_OEMDispatch.h with BREW 3.0
11/26/03     PA      Replace BREW malloc with DMSS malloc for thread safety
11/20/03     PA      Copy call progress field in corresponding event.
11/20/03     PA      Support for DDTM
11/19/03     PA      Changes for BREW 3.0
11/18/03     PA      Send 2 Send for GSM/WCDMA answer call.
11/17/03     PA      Fixed compile error for non-GSTK builds, support for Burst
                     DTMF in GSM/WCDMA, prevent multiple cancel of callbacks.
11/13/03     PA      Support for ICallOpts
11/13/03     SUN     Fixed Typo in OEMCM_EndCall
11/12/03     PA      Update sys_mode in GetCallInfo()
11/12/03     PA      Add Call History entry correctly when current time
                     is not available from real time clock.
11/11/03     PA      Fixed Typo in ending PS_DATA calls
11/11/03     SUN     Fixed Typo in OEMCM_RejectCall
11/07/03     PA      Update call type on AEECM_EVENT_CALL_INCOM_FWD_CALL
11/06/03     PA      Changed Event Callback array to linked list
11/03/03     PA      Removed support for ICM_SetUIMSlotPref().
11/03/03     PA      Fix to translate enums from CM that are not 32-bits
10/24/03     PA      Support for AEECM_SYS_MODE_HDR and other fixes
10/23/03     PA      Fixes for not confusing data calls with voice calls.
10/22/03     PA      Support for AEECallback, GetNetworks(),
                     dynamic error callback list and other fixes
10/22/03     BPW     Changed return value for CallOps in CDMA to unsupported
10/22/03     SUN     Fixed Compile Errors
10/17/03     SUN     Filled the correct connected number
10/17/03     SUN     Check for Cause values in Call End before sending error to
                     Client
10/16/03     PA      Changed MM time information to JulianType,
                     Fixed flash in AnswerCall, combined EXT_DISPLAY event with
                     DISPLAY event, removed CALL_CONF handling, added sim_state,
                     Support for HDR, Changed call start/duration to uptime,
                     Adding "Unavailable" to call history for no name and alpha
10/09/03     PA      AOC and Dual Slot SIM functionality, added ICM_IsMyCall().
10/08/03     SUN     Fixed Compile Error, Removed mode check from  HandleMngCall
10/08/03     BPW     Fixed linking error
10/07/03     PA      Handle reject call, added GetSysMode() and other fixes
10/06/03     BPW     Removed call id decrement on CallOpsRequest for CDMA
10/03/03     SUN     Fixed Connected Number Display
09/30/03     PA      Simplified ICM error values, changed call_type
                     and call state to bit mask. Added IsValidModeBandPref().
                     Fixed typo in BurstDTMF.
09/26/03     PA      Flash support in answer call. Handle CLIR in call
                     origination.
09/25/03     PA      Added sups string check function and other changes.
                     Incorporated code review comments on function names.
09/19/03     PA      Added more debug messages and fixed call id off by one bug
                     Changed flash handling logic
09/08/03     PA      Support for Originate Call, supps changes and call id
                     mapping.
09/04/03     SUN     Changed AeeSuppSys.h to AEESuppSvc.h
08/22/03     PA      Initial Revision
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
/* Lint global dont cares */
/*lint -esym(754, ICM::pvt) */
/*lint -esym(754, ICMNotifier::pvt) */
/*lint -esym(754, ICallOpts::pvt) */
/*lint -save -e740 */
/*lint -save -e611 */

#include "OEMCM.h"
#include "OEMCMHelper.h"

#include "SysModelBase_priv.h"
#include "AEEModel.h"
#if defined(FEATURE_QTV_QDSP_RELEASE_RESTORE) && defined (FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif

#ifdef FEATURE_USRSYSCLOCK
#include "AEEStdLib.h"
#include "USRSYSCLOCK.BID"
#include "AEEUsrSysClock.h"
#endif // FEATURE_USRSYSCLOCK

#include "prl.h"

#ifdef srIMON
#error code not present
#endif

#if defined (FEATURE_ACM)
#include "msg.h"
#include "err.h"
#include "OEMACM.h"
#if defined (FEATURE_ODM_UI)
#include "OEMAppResCtl.h"
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM

/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

static ICMCore *ICMCoreObj = NULL;
static struct ICMNotifier *ICMNotifierObj = NULL;
static struct ICMModel *ICMModelObj = NULL;

static cm_client_id_type   gCMID = 0;
static boolean             gbCMAct = FALSE;

#ifdef FEATURE_GSTK_FDN_CC_SUPPORT
cm_cc_generic_params_s_type gCMGenericParams = {TRUE};
#endif
#ifdef FEATURE_DS_IS2000
const byte oemcm_dec_sch_rate_table[MAX_NO_SCH_RATES][2] = {
  {DEC_SCH_RATE_1X,  1},
  {DEC_SCH_RATE_2X,  2},
  {DEC_SCH_RATE_4X,  4},
  {DEC_SCH_RATE_8X,  8},
  {DEC_SCH_RATE_16X, 16},
#ifdef FEATURE_32X_SCH_DATA_RATE
  {DEC_SCH_RATE_32X, 32}
#endif  //FEATURE_32X_SCH_DATA_RATE
};

const byte oemcm_enc_sch_rate_table[MAX_NO_SCH_RATES][2] = {
  {ENC_SCH_FULL_RATE, 1},
  {ENC_SCH_2X_RATE,   2},
  {ENC_SCH_4X_RATE,   4},
  {ENC_SCH_8X_RATE,   8},
  {ENC_SCH_16X_RATE,  16},
#ifdef FEATURE_32X_SCH_DATA_RATE
  {ENC_SCH_32X_RATE,  32}
#endif  //FEATURE_32X_SCH_DATA_RATE
};
#endif /* FEATURE_DS_IS2000 */

#ifndef CM_API_SIGNAL_STRENGTH_INDICATION
  #define AEECM_DEFAULT_RSSI_TO_R5 -106
  #define AEECM_DEFAULT_ECIO_TO_E5 -17
#endif
#ifdef FEATURE_UI_PBM
#include "pbmlib.h"
#endif
/* The following table is used to convert back and
 * forth between CM and ICM band preferences.  Usage of the
 * table works on the assumption that both are constructed
 * from bit masks, which is true at the point of construction
 * of these tables.
 *
 * _NO_CHANGE, _ANY and _PERSISTENT values are not included
 * in this table and must be handled as special cases.  _NONE
 * is also not included, since no conversion is necessary
 * for that case.
 */
static OEMCM_BandPrefConvType OEMCM_BandPrefConv[] =
{
  { CM_BAND_PREF_BC0_A,               AEECM_BAND_PREF_BC0_A },
  { CM_BAND_PREF_BC0_B,               AEECM_BAND_PREF_BC0_B },
  { CM_BAND_PREF_BC0,                 AEECM_BAND_PREF_BC0 },
  { CM_BAND_PREF_BC1,                 AEECM_BAND_PREF_BC1 },
  { CM_BAND_PREF_BC3,                 AEECM_BAND_PREF_BC3 },
  { CM_BAND_PREF_BC4,                 AEECM_BAND_PREF_BC4 },
  { CM_BAND_PREF_BC5,                 AEECM_BAND_PREF_BC5 },
  { CM_BAND_PREF_GSM_DCS_1800,        AEECM_BAND_PREF_GSM_DCS_1800 },
  { CM_BAND_PREF_GSM_EGSM_900,        AEECM_BAND_PREF_GSM_EGSM_900 },
  { CM_BAND_PREF_GSM_PGSM_900,        AEECM_BAND_PREF_GSM_PGSM_900 },
  { CM_BAND_PREF_BC6,                 AEECM_BAND_PREF_BC6 },
  { CM_BAND_PREF_BC7,                 AEECM_BAND_PREF_BC7 },
  { CM_BAND_PREF_BC8,                 AEECM_BAND_PREF_BC8 },
  { CM_BAND_PREF_BC9,                 AEECM_BAND_PREF_BC9 },
  { CM_BAND_PREF_BC10,                AEECM_BAND_PREF_BC10 },
  { CM_BAND_PREF_BC11,                AEECM_BAND_PREF_BC11 },

#ifdef CM_API_BAND_CLASS_12_14_15_16
  { CM_BAND_PREF_BC12,                AEECM_BAND_PREF_NONE }, /* No ICM value */
  { CM_BAND_PREF_BC14,                AEECM_BAND_PREF_NONE }, /* No ICM value */
  { CM_BAND_PREF_BC15,                AEECM_BAND_PREF_BC15 },
  { CM_BAND_PREF_BC16,                AEECM_BAND_PREF_BC16 },
#endif /* CM_API_BAND_CLASS_12_14_15_16 */

  { CM_BAND_PREF_GSM_450,             AEECM_BAND_PREF_GSM_450 },
  { CM_BAND_PREF_GSM_480,             AEECM_BAND_PREF_GSM_480 },
  { CM_BAND_PREF_GSM_750,             AEECM_BAND_PREF_GSM_750 },
  { CM_BAND_PREF_GSM_850,             AEECM_BAND_PREF_GSM_850 },
  { CM_BAND_PREF_GSM_RGSM_900,        AEECM_BAND_PREF_GSM_RGSM_900 },
  { CM_BAND_PREF_GSM_PCS_1900,        AEECM_BAND_PREF_GSM_PCS_1900 },
  { CM_BAND_PREF_WCDMA_I_IMT_2000,    AEECM_BAND_PREF_WCDMA_I_IMT_2000 },
  { CM_BAND_PREF_WCDMA_II_PCS_1900,   AEECM_BAND_PREF_WCDMA_II_PCS_1900 },
  { CM_BAND_PREF_WCDMA_III_1700,      AEECM_BAND_PREF_WCDMA_III_1700 },
  { CM_BAND_PREF_WCDMA_IV_1700,       AEECM_BAND_PREF_WCDMA_IV_1700 },
  { CM_BAND_PREF_WCDMA_V_850,         AEECM_BAND_PREF_WCDMA_V_850 },
  { CM_BAND_PREF_WCDMA_VI_800,        AEECM_BAND_PREF_WCDMA_VI_800 },

#ifdef CM_API_WCDMA900
  { CM_BAND_PREF_WCDMA_VII_2600,      AEECM_BAND_PREF_WCDMA_VII_2600 },
  { CM_BAND_PREF_WCDMA_VIII_900,      AEECM_BAND_PREF_WCDMA_VIII_900 },
  { CM_BAND_PREF_WCDMA_IX_1700,       AEECM_BAND_PREF_WCDMA_IX_1700 },
#endif /* CM_API_WCDMA900 */

#ifdef CM_API_WLAN
  { CM_BAND_PREF_WLAN_2400_US,        AEECM_BAND_PREF_WLAN_2400_US },
  { CM_BAND_PREF_WLAN_2400_JP,        AEECM_BAND_PREF_WLAN_2400_JP },
  { CM_BAND_PREF_WLAN_2400_ETSI,      AEECM_BAND_PREF_WLAN_2400_ETSI },
  { CM_BAND_PREF_WLAN_2400_SP,        AEECM_BAND_PREF_WLAN_2400_SP },
  { CM_BAND_PREF_WLAN_2400_FR,        AEECM_BAND_PREF_WLAN_2400_FR },
  { CM_BAND_PREF_WLAN_5000_US,        AEECM_BAND_PREF_WLAN_5000_US },
  { CM_BAND_PREF_WLAN_5000_JP,        AEECM_BAND_PREF_WLAN_5000_JP },
  { CM_BAND_PREF_WLAN_5000_ETSI,      AEECM_BAND_PREF_WLAN_5000_ETSI },
  { CM_BAND_PREF_WLAN_5000_SP,        AEECM_BAND_PREF_WLAN_5000_SP },
  { CM_BAND_PREF_WLAN_5000_FR,        AEECM_BAND_PREF_WLAN_5000_FR },

  /* No entry is provided for "WLAN ANY" for several reasons.
   * First, there is no corresponding value defined in CM.
   * Second, it is not necessary, as it simply aggregates
   * existing ICM WLAN band pref values supported in this table.
   */

#endif /* CM_API_WLAN */

  { CM_BAND_PREF_CELL_A, AEECM_BAND_PREF_CELL_A },
  { CM_BAND_PREF_CELL_B, AEECM_BAND_PREF_CELL_B },
  { CM_BAND_PREF_CELL, AEECM_BAND_PREF_CELL },
  { CM_BAND_PREF_PCS, AEECM_BAND_PREF_PCS },
};

/*===========================================================================

                            INTERFACE FUNCTION TABLE

===========================================================================*/

/* The functions supported by CM Notifier object. */
static const AEEVTBL(ICMNotifier) gOEMCMNotifierFuncs = {
   OEMCMNotifier_AddRef,
   OEMCMNotifier_Release,
   OEMCMNotifier_SetMask
};

/* The function supported by CM object. */
static const AEEVTBL(ICM) gOEMCMFuncs = {
   OEMCM_AddRef,
   OEMCM_Release,
   OEMCM_QueryInterface,
   OEMCM_OriginateVoiceCall,
   OEMCM_OriginateCall,
#ifdef FEATURE_GSTK_FDN_CC_SUPPORT
   OEMCM_OriginateCall_SkipFDNChk,
#endif
   OEMCM_AnswerCall,
   OEMCM_EndCall,
   OEMCM_EndAllCalls,
   OEMCM_GetActiveCallIDs,
   OEMCM_GetCallInfo,
   OEMCM_SetPrivacyPref,
   OEMCM_CallOpsStringCheck,
   OEMCM_CallOpsRequest,
   OEMCM_SetSystemPreference,
   OEMCM_SetNAMSel,
   OEMCM_SetOperatingMode,
   OEMCM_GetPhoneInfo,
   OEMCM_SetSubscriptionStatus,
   OEMCM_SetRSSIDeltaThreshold,
   OEMCM_GetNetworks,
   OEMCM_SetPrefNetworks,
   OEMCM_WakeupFromStandby,
   OEMCM_SetRTREConfig,
   OEMCM_SetAnswerVoicePref,
   OEMCM_AvoidServingSystem,
   OEMCM_ResetACM,
   OEMCM_SetACMMax,
   OEMCM_SetDDTMPref,
   OEMCM_GetSSInfo,
   OEMCM_BurstDTMF,
   OEMCM_StartContDTMF,
   OEMCM_StopContDTMF,
   OEMCM_IsValidModeBandPref,
   OEMCM_GetSysMode,
   OEMCM_IsMyCall,
   OEMCM_SelectLine,
   OEMCM_SetLineSwitching,
   OEMCM_GetPLMNFromIMSI,
   OEMCM_SysMatchPLMN,
   OEMCM_RecallRsp,
   OEMCM_ResetOrigThrottle,
   OEMCM_SetSubscriptionStatusEx,
   OEMCM_SendFlashSupps,
   OEMCM_PrlExtractAcqRecord,
   OEMCM_PrlUpdateAcqRecord,
   OEMCM_PrlCommit,
   OEMCM_SendMBMSCommand
};

/* The functions supported by ICallOpts */
static const AEEVTBL(ICallOpts) gOEMCallOptsFuncs = {
  OEMCallOpts_AddRef,
  OEMCallOpts_Release,
  OEMCallOpts_QueryInterface,
  OEMCallOpts_AddOpt,
  OEMCallOpts_RemoveOpt,
  OEMCallOpts_GetOpt
};

/*===========================================================================

                           EVENT HANDLERS

===========================================================================*/

/* Call Event Handlers */
static PFNOEMCMCALLNOTIFY gCallEventHandlers[] = {
  NULL,                             /* Nothing */
  OEMCM_HandleCallOrig,             /* AEECM_EVENT_CALL_ORIG */
  OEMCM_HandleCallAnswer,           /* AEECM_EVENT_CALL_ANSWER */
  OEMCM_HandleCallEnd,              /* AEECM_EVENT_CALL_END */
  OEMCM_HandleCallIncom,            /* AEECM_EVENT_CALL_INCOM */
  OEMCM_HandleCallConnect,          /* AEECM_EVENT_CALL_CONNECT */
  NULL,                             /* AEECM_EVENT_CALL_ERROR */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_OPS_ERROR */
  OEMCM_HandleCallNumUpdate,        /* AEECM_EVENT_CALL_OPS */
  OEMCM_HandleCallCopyNum,          /* AEECM_EVENT_CALL_CALLER_ID */
  OEMCM_HandleCallAlphaUpdate,      /* AEECM_EVENT_CALL_DISPLAY */
  OEMCM_HandleCallCopyNum,          /* AEECM_EVENT_CALL_CALLED_PARTY */
  OEMCM_HandleCallConnectedNum,     /* AEECM_EVENT_CALL_CONNECTED_NUM */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_BARRED */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_IS_WAITING */
  OEMCM_HandleCallHold,             /* AEECM_EVENT_CALL_ON_HOLD */
  OEMCM_HandleCallRetrieved,        /* AEECM_EVENT_CALL_RETRIEVED */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_ORIG_FWD_STATUS */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_FORWARDED */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_BEING_FORWARDED */
  OEMCM_HandleCallIncomFwd,         /* AEECM_EVENT_CALL_INCOM_FWD_CALL */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_RESTRICTED */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_CUG_INFO_RECEIVED */
  OEMCM_HandleCallAlphaUpdate,      /* AEECM_EVENT_CALL_CNAP_INFO_RECEIVED */
  OEMCM_HandleCallEmergencyFlashed, /* AEECM_EVENT_CALL_EMERGENCY_FLASHED */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_DEFLECTION */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_TRANSFERRED */
  OEMCM_HandleCallSrvOpt,           /* AEECM_EVENT_CALL_SRV_OPT */
  OEMCM_HandleCallSrvOpt,           /* AEECM_EVENT_CALL_PRIVACY */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_ABRV_ALERT */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_ABRV_REORDER */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_ABRV_INTERCEPT */
  OEMCM_HandleCallSignal,           /* AEECM_EVENT_CALL_SIGNAL */
  OEMCM_HandleCallProgress,         /* AEECM_EVENT_CALL_PROGRESS_INFO_IND */
  OEMCM_HandleCallDormant,          /* AEECM_EVENT_CALL_ENTER_DORMANT */
  OEMCM_HandleCallDormant,           /* AEECM_EVENT_CALL_EXIT_DORMANT */
  OEMCM_HandleCallLineCtrl,         /* AEECM_EVENT_CALL_LINE_CTRL */
  OEMCM_HandleCallDefault,          /*AEECM_EVENT_CALL_RECALL_IND*/
  OEMCM_HandleCallDefault,          /*AEECM_EVENT_CALL_CCBS_ALLOWED*/
  OEMCM_HandleCallRecallConf,       /* AEECM_EVENT_CALL_CCBS_SET*/
  NULL,                             /* AEECM_EVENT_CALL_ALL_CALLS_ENDED */
  OEMCM_HandleCallDefault           /* AEECM_CALL_EVENT_NSS_REL_REC */
};

/* Call internal event handlers */
static PFNOEMCMCALLNOTIFY gCallIntEventHandlers[] = {
  NULL,                             /* Nothing */
  OEMCM_HandleCallSetupInd,         /* AEECM_EVENT_INTERNAL_SETUP_IND */
  OEMCM_HandleCallMngCallsConf,     /* AEECM_EVENT_INTERNAL_MNG_CALLS_CONF */
  OEMCM_HandleCallEnd,              /* AEECM_EVENT_INTERNAL_END_REQ */
  OEMCM_HandleCallExtDisplay,       /* AEECM_EVENT_INTERNAL_EXT_DISP */
  OEMCM_HandleCallQOS,              /* AEECM_EVENT_INTERNAL_PDP_MODIFY_IND */
  OEMCM_HandleCallQOS,              /* AEECM_EVENT_INTERNAL_PDP_MODIFY_CONF */
  OEMCM_HandleCallRecallRsp         /* AEECM_EVENT_INTERNAL_RECALL_RSP*/
};

/* Phone event handlers */
static PFNOEMCMPHNOTIFY gPhEventHandlers[] = {
  NULL,                          /* Nothing */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_OPRT_MODE */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_SYSTEM_PREF */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_ANSWER_VOICE */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_NAM_SEL */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_CURR_NAM */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_IN_USE_STATE */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_CDMA_LOCK_MODE */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_MAINTREQ */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_STANDBY_SLEEP */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_STANDBY_WAKE */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_INFO_AVAIL */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_SUBSCRIPTION */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_FUNDS_LOW */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_NVRUIM_CONFIG */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_ACM_RESET */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_ACMMAX_SET */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_SELECTED_LINE */
  OEMCM_HandlePhDefault,         /* AEECM_EVENT_PH_LINE_SWITCHING */
};

/* phone internal event handlers */
static PFNOEMCMPHNOTIFY gPhIntEventHandlers[] = {
  NULL,                          /* Nothing */
  OEMCM_HandlePhTestControl,     /* AEECM_EVENT_INTERNAL_TEST_CONTROL */
  OEMCM_HandlePhInternalDefault, /* AEECM_EVENT_INTERNAL_PREF_NW_SET */
  OEMCM_HandlePhInternalDefault, /* AEECM_EVENT_INTERNAL_DDTM_PREF */
  OEMCM_HandlePhInternalDefault  /* AEECM_EVENT_INTERNAL_WAKEUP */
};

/* Since there are only a few SS and Inband events, an event handler
 * table is not used for these events.  The events are handled in
 * OEMCM_BREWSSEventCB and OEMCM_AppInbandEventCB directly.
 *
 * < Not available: gSSEventHandlers PFNOEMCMSSNOTIFY OEMCM_HandleSS
 *   gInbandEventHandlers PFNOEMCMINBANDNOTIFY OEMCM_HandleInband >
 */


#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /*  FEATURE_QTV_QDSP_RELEASE_RESTORE && FEATURE_QTV_QOS_SELECTION */

static AEECMDSTAdj OEMCM_SysDSTToAEECMDST(sys_daylight_saving_adj_e_type sys_dst);
#ifdef FEATURE_USRSYSCLOCK
static void OEMCM_HandleSsMobilityMgmt( const sys_mm_information_s_type *mm_info_ptr, AEECMMMInfo *mmInfo);
#endif // FEATURE_USRSYSCLOCK

#ifdef FEATURE_ACM
static int  OEMCM_ACMCreateInstance(ICM_ACMInfo * pAcmInfo);
static int  OEMCM_ACMAcquire(ICM * pMe, ICM_ACMInfo * pAcmInfo, uint32 reasonCode);
static int  OEMCM_ACMFree(ICM * pme, ICM_ACMInfo * pAcmInfo);
static void OEMCM_ACMChangeNotify(void *p);
static void OEMCM_ACM_NetworkOriginatedCallChangeNotify(void *p);
static void OEMCM_ACM_MobileOriginatedCallChangeNotify(void *p);
static int  OEMCM_CallCM_AnswerCall(cm_call_cmd_cb_f_type  cmd_cb_func,
                                    void                   *data_block_ptr,
                                    cm_client_id_type      client_id,
                                    cm_call_id_type        call_id,
                                    cm_ans_params_s_type   *ans_params_ptr);
static void OEMCM_CallOrig_Notify(AEECMNotifyInfo * notify_info);
#endif // FEATURE_ACM

/* The following table simplifies the mappings of CM band mask values
 * and ICM band pref values.
 */
#if defined(FEATURE_WLAN) && defined(CM_API_WLAN)
#error code not present
#endif /* defined(FEATURE_WLAN) && defined(CM_API_WLAN) */

#ifdef FEATURE_ALS
#error code not present
#endif

#define OEMCM_ALPHABET_MASK                    0x0F
#define OEMCM_USS_DEF_ALPHABET_LANG_UNSPEC     0x0F

/*===========================================================================

                  ICMNOTIFIER FUNCTION DEFINITIONS

===========================================================================*/
/*=============================================================================
FUNCTION: OEMCMNotifier_New

DESCRIPTION:
  Allocates a new ICMNotifier object and initializes it or adds reference to
  an existing object.
  Allocates a core object if it was not already present.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCMNotifier_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
   ICMNotifier *pNew;
#if !MIN_BREW_VERSION(3,1)
#ifdef AEE_SIMULATOR
   /* Do not create an object is this is being run on the SDK. */
   *ppif = NULL;
   return EUNSUPPORTED;
#endif
#endif //!MIN_BREW_VERSION(3,1)
   PACONTEXT pacLast;

   /* Check parameters. */
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }

   if (cls != AEECLSID_CM_NOTIFIER) {
      return EUNSUPPORTED;
   }

   /* If the object doesn't already exist, allocate it now. */
   if (ICMNotifierObj == NULL) {
     /* Allocate the object. */

     pNew = (ICMNotifier*)AEE_NewClassEx((IBaseVtbl*)&gOEMCMNotifierFuncs,
                                         sizeof(ICMNotifier), TRUE);

     if (pNew == NULL)
        return ENOMEMORY;

     /* Initialize the values. */
     pNew->m_pIShell = pIShell;
     pNew->m_cls = cls;
     pNew->m_uRefs = 1;
     pNew->m_dwNotify = 0;

     /* Create core object */
     pNew->m_coreObj = OEMCMCore_New();

     if(!pNew->m_coreObj) {
        (void) OEMCMNotifier_Release(pNew);
        return EFAILED;
     }


     /* Exit callback needs to be registered in the system callback */
     pacLast = AEE_EnterAppContext(NULL);
     CALLBACK_Init(&pNew->m_cbExit, OEMCMNotifier_FreeOnAEEExit, 0);
     ISHELL_OnExit(pNew->m_pIShell, &pNew->m_cbExit);
     AEE_LeaveAppContext(pacLast);

     ICMNotifierObj = pNew;
   }
   else
   {
     (void) OEMCMNotifier_AddRef(ICMNotifierObj);
   }

   *ppif = ICMNotifierObj;

   return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCMNotifier_AddRef

DESCRIPTION:
  Indicates that one or more object is now pointing to ICMNotifier object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMCMNotifier_AddRef(ICMNotifier *pMe)
{
  if (pMe) {
    return (++pMe->m_uRefs);
  }
  return 0;
}

/*=============================================================================
FUNCTION: OEMCMNotifier_Release

DESCRIPTION:
  Decreases the number of references to the ICMNotifier object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMCMNotifier_Release(ICMNotifier *pMe)
{
   if (!pMe)
      return (0);

   if (--pMe->m_uRefs)
      return pMe->m_uRefs;

   /* Ref count is zero.  Release memory */
   if (pMe->m_coreObj) {
      (void) OEMCMCore_Release();
      pMe->m_coreObj = NULL;
   }

   CALLBACK_Cancel(&pMe->m_cbExit);

   FREE(pMe);
   ICMNotifierObj = NULL;
   return (0);
}

/*=============================================================================
FUNCTION: OEMCMNotifier_SetMask

DESCRIPTION:
  This function is called by Brew when an application does an
  ISHELL_RegisterNotify for the ICM_NOTIFIER object.  We merge
  all the masks for all the registrations and save a bitmap of
  the sum of all the events being requested.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
#if MIN_BREW_BUILD(3,1,4,7)
static void OEMCMNotifier_SetMask(ICMNotifier *pMe, uint32 *pdwMask)
#else
static void OEMCMNotifier_SetMask(ICMNotifier *pMe, const uint32 *pdwMask)
#endif // MIN_BREW_BUILD(3,1,4,7)
{
  if (!pMe || !pdwMask)
    return;

  pMe->m_dwNotify = OEMCM_MergeMasks(pdwMask);
}

/*===========================================================================

FUNCTION: OEMCMModel_New

DESCRIPTION
  Allocates a new object from the heap and stores the IShell pointer
  used to generate the events.

DEPENDENCIES
  none

RETURN VALUE
  none
===========================================================================*/
int OEMCMModel_New(IShell *pIShell, AEECLSID cls, void **ppif)
{

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }

#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   if (cls != AEECLSID_CM_MODEL) {
      return EUNSUPPORTED;
   }

   if (NULL == ICMModelObj)
   {
      /* Allocate the object. */
      ICMModelObj = (ICMModel *)sys_malloc(sizeof(ICMModel));

      if (NULL == ICMModelObj)
      {
         return ENOMEMORY;
      }

      MEMSET((void *)ICMModelObj, 0, sizeof(ICMModel));

      SysModelBase_Ctor(&ICMModelObj->base,
                        &ICMModelObj->vtModel,
                        OEMCMModel_Dtor);

      /* Register for AEE Exit event */
      ICMModelObj->m_pcbExit = sys_malloc(sizeof(AEECallback));
      if (!ICMModelObj->m_pcbExit)
      {
         SysModelBase_Release(SYSMODELBASE_TO_IMODEL(&ICMModelObj->base));
         return ENOMEMORY;
      }

      CALLBACK_Init(ICMModelObj->m_pcbExit, (PFNNOTIFY)OEMCMModel_AEEExitNotify,
                    ICMModelObj);
#ifndef FEATURE_BREW_3_0
      AEE_RegisterSystemCallback(ICMModelObj->m_pcbExit, AEE_SCB_AEE_EXIT,
                                 AEE_RESUME_CB_SYS,0);
#else
      ISHELL_OnExit(pIShell, ICMModelObj->m_pcbExit);
#endif

     /* Create core object */
     ICMModelObj->m_coreObj = OEMCMCore_New();

     if(!ICMModelObj->m_coreObj) {
         (void) SysModelBase_Release(SYSMODELBASE_TO_IMODEL(&ICMModelObj->base));
        return EFAILED;
     }

   }
   else
   {
      IModel *pModel = SYSMODELBASE_TO_IMODEL(&ICMModelObj->base);
      IMODEL_AddRef(pModel);
   }

   *ppif = SYSMODELBASE_TO_IMODEL(&ICMModelObj->base);

   return AEE_SUCCESS;
}

/*===========================================================================

FUNCTION: OEMCMModel_Dtor

DESCRIPTION
  Deallocates the model object and associated resources.

DEPENDENCIES
  none

RETURN VALUE
  none
===========================================================================*/
static void OEMCMModel_Dtor(SysModelBase *pMe)
{

   if (!pMe) {
      return;
   }

   if (NULL == ICMModelObj) {
      return;
   }

   SysModelBase_Dtor(pMe);

   if (NULL != ICMModelObj->m_coreObj ) {
      (void) OEMCMCore_Release();
      ICMModelObj->m_coreObj = NULL;
   }

   if (ICMModelObj->m_pcbExit) {
     CALLBACK_Cancel(ICMModelObj->m_pcbExit);
     sys_free(ICMModelObj->m_pcbExit);
     ICMModelObj->m_pcbExit = NULL;
   }

   sys_free(ICMModelObj);

   ICMModelObj = NULL;
}

/*===========================================================================

                   ICM FUNCTION DEFINITIONS

===========================================================================*/
/*=============================================================================
FUNCTION: OEMCM_New

DESCRIPTION:
  Allocates a new ICM object, creates a Core object if it is not present,
  stores the App context and initializes object data. A new ICM object is
  created for every ISHELL_CreateInstance().

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCM_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
   ICM *pNew;

#ifdef AEE_SIMULATOR
   /* Do not create an object is this is being run on the SDK. */
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }

   if (cls != AEECLSID_CALLMANAGER) {
      return EUNSUPPORTED;
   }

   // Allocate the object.
   pNew = (ICM *) AEE_NewClass((IBaseVtbl*)&gOEMCMFuncs, sizeof(ICM));
   if (pNew == NULL)
      return ENOMEMORY;

#if !MIN_BREW_VERSION(3,0)
   (void) AEE_LinkSysObject(&pNew->u.m_so);
#endif

   /* Initialize the values. */
   pNew->m_pIShell = pIShell;
   pNew->m_cls = cls;
   pNew->m_uRefs = 1;

   pNew->m_err_cbs = NULL;

   pNew->m_coreObj = OEMCMCore_New();
   if (!pNew->m_coreObj) {
     (void) OEMCM_Release(pNew);
     return EFAILED;
   }

   pNew->m_pac = AEE_GetAppContext();

  *ppif = pNew;

   return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_AddRef

DESCRIPTION:
  Indicates that one or more object is now pointing to object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMCM_AddRef(ICM *pMe)
{
  if (pMe) {
    return (++pMe->m_uRefs);
  }
  return 0;
}

/*=============================================================================
FUNCTION: OEMCM_Release

DESCRIPTION:
  Decreases the number of references to the ICM object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMCM_Release(ICM *pMe)
{
   int i;

   if (!pMe) {
      return (0);
   }

   if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
   }

   /* Cancel scheduled error callbacks */
   OEMCM_CancelAllErrCB(pMe);

   OEMCM_CancelBurstDTMF(pMe);
   if( pMe->m_coreObj ) {
     for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i)
       if (pMe->m_coreObj->m_call_owners[i] == pMe)
         pMe->m_coreObj->m_call_owners[i] = NULL;
      (void) OEMCMCore_Release();
   }

   // Ref count is zero.  Release memory
   FREEIF(pMe);
   return (0);
}

/*===========================================================================

FUNCTION: OEMCM_QueryInterface

DESCRIPTION
  Allows applications to query if ICM supports other interfaces and then
  returns a pointer to that new interface supported via ICM.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/

static int OEMCM_QueryInterface(ICM *pMe, AEECLSID id, void **ppNew)
{
   int ret_val = AEE_SUCCESS;

   if ((!pMe) || (!ppNew)) {
      return EBADPARM;
   }

   switch (id) {
      case AEECLSID_CALLMANAGER:
         if (OEMCM_AddRef(pMe)) {
            *ppNew = pMe;
         } else {
            *ppNew = NULL;
            ret_val = ECLASSNOTSUPPORT;
         }
         break;

#if defined (FEATURE_ACM)
#if defined (FEATURE_ODM_UI)
      case AEECLSID_APPRESCTL:
         if (ICMCoreObj->m_pAcmInfo->m_pAppResCtl != NULL)
         {
             IBASE_AddRef ((IBase*)ICMCoreObj->m_pAcmInfo->m_pAppResCtl);
             *ppNew = ICMCoreObj->m_pAcmInfo->m_pAppResCtl;
         }
         else
         {
            *ppNew = NULL;
            ret_val = ECLASSNOTSUPPORT;
         }

         break;
#endif // FEATURE_ODM_UI
#endif //FEATURE_ACM

      default:
         *ppNew = NULL;
         ret_val = ECLASSNOTSUPPORT;
         break;
   }


   return ret_val;
}


/*---------------------------------------------------------------------------
                          CALL FUNCTIONS
---------------------------------------------------------------------------*/

/*=============================================================================
FUNCTION: OEMCM_AEECALLID

DESCRIPTION:
  This function will first search for a call ID associated with the
  CM call id.  If found, it returns that, if not, it will allocate a
  new one.

PROTOTYPE:
  AEECMCallID OEMCM_AEECALLID(cm_call_id_type call_id)

PARAMETERS:
  call_id           [in]: CM generated call_id.

RETURN VALUE:
  ICM generated call ID if successful, 0 if not.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEECMCallID OEMCM_AEECALLID(cm_call_id_type call_id)
{
  uint8 i;

  /* This function requires that the ICMCoreObj has been created. */
  if (ICMCoreObj == NULL)
     return 0;

  /* First search through the existing call ids, looking for the given call. */
  for (i = 1; i < ARR_SIZE(ICMCoreObj->m_internal_call_ids); i++)
  {
    if (ICMCoreObj->m_internal_call_ids[i] == call_id)
    {
      /* If found, return that. */
      return ((AEECMCallID) i);
    }
  }

  /* If we reach here, the CM call ID has not yet been allocated. */
  /* Now we must search for an empty slot. */
  for (i = 1; i < ARR_SIZE(ICMCoreObj->m_internal_call_ids); i++)
  {
    if (ICMCoreObj->m_internal_call_ids[i] == CM_CALL_ID_INVALID)
    {
      /* We found an empty slot, use this slot for the current call id. */
      ICMCoreObj->m_internal_call_ids[i] = call_id;
      MSG_HIGH("Gave CM call ID %d OEMCM Call ID %d", call_id, i, 0);
      return ((AEECMCallID) i);
    }
  }

  /* If we reach here, we did not give a valid call ID. */
  ERR("No available call ID to give to CM call ID %d", call_id, 0, 0);
  return ((AEECMCallID) 0);
}

/*=============================================================================
FUNCTION: OEMCM_OriginateVoiceCall

DESCRIPTION:
  Takes the ICM interface call ID and returns a CM call ID associated with
  it.  If not found, it returns CM_CALL_ID_INVALID.

PROTOTYPE:
  cm_call_id_type OEMCM_CMCALLID(AEECMCallID callID)

PARAMETERS:
  callID            [in]: ICM interface call ID.

RETURN VALUE:
  The CM call ID associated with this call ID.  If not found, return
  CM_CALL_ID_INVALID

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
cm_call_id_type OEMCM_CMCALLID(AEECMCallID callID)
{
  if(ICMCoreObj && callID > 0 && callID < OEMCM_MAX_NUM_CALLS)
    return ICMCoreObj->m_internal_call_ids[callID];

  ERR("Could not find a CM call ID associated with %d", callID, 0, 0);
  return CM_CALL_ID_INVALID;
}


/*=============================================================================
FUNCTION: OEMCM_OriginateVoiceCall

DESCRIPTION:
  When a call is complete, this function will free the associated call ID
  from the allocated array to allow the ICM call ID to be reused.

PROTOTYPE:
  void OEMCM_FREE_CALLID(cm_call_id_type call_id)

PARAMETERS:
  callID          [in]: CM Call ID

RETURN VALUE:
  None.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEMCM_FREE_CALLID(cm_call_id_type call_id)
{
  int i;
  boolean freed = FALSE;

  /* Search through our array for the given CM call ID. */
  for (i = 1; i < ARR_SIZE(ICMCoreObj->m_internal_call_ids); i++)
  {
    if (call_id == ICMCoreObj->m_internal_call_ids[i])
    {
      /* If found, we will free up an entry. */
      ICMCoreObj->m_internal_call_ids[i] = CM_CALL_ID_INVALID;
      freed = TRUE;
    }
    /* This code makes sure that the allocated call IDs all point to valid
     * calls.  */
    if ((ICMCoreObj->m_internal_call_ids[i] != CM_CALL_ID_INVALID) &&
        (!ICMCoreObj->m_state[ICMCoreObj->m_internal_call_ids[i]].bValid))
    {
      ERR("Unexpected mismatch AEE call ID %d valid, Cm call ID %d not valid",
          i, ICMCoreObj->m_internal_call_ids[i], 0);
    }
  }

  /* If the CM call ID could not be found, print an error message. */
  if (!freed)
    ERR("Could not free call ID associated with %d", call_id, 0, 0);
}


/*=============================================================================
FUNCTION: OEMCM_OriginateVoiceCall

DESCRIPTION:
  This function is called to initialize the call ID list to all entries
  unused.

PROTOTYPE:
  void OEMCM_INIT_CALLIDS(void)

PARAMETERS:
  None.

RETURN VALUE:
  None.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEMCM_INIT_CALLIDS(void)
{
  int i;

  /* Fill in all our call IDs as not allocated yet by CM. */
  for (i = 0; i < ARR_SIZE(ICMCoreObj->m_internal_call_ids); i++)
  {
    ICMCoreObj->m_internal_call_ids[i] = CM_CALL_ID_INVALID;
  }
}

/*=============================================================================
FUNCTION: OEMCM_OriginateVoiceCall

DESCRIPTION:
  Originate a Voice call with default options.

PROTOTYPE:
  int OEMCM_OriginateVoiceCall(ICM *po,
                        AECHAR const *number,
                        AEECMCallID  *pcallID)

PARAMETERS:
  po                [in]: ICM interface pointer.
  number            [in]: Number to be dialed
  pcallID          [out]: Call ID assigned to the call

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EBADPARM:     If number is NULL and the resulting operation is not a flash.
  EFAILED:      Otherwise.

COMMENTS:
  OriginateVoiceCall handles GSM CLIR suppression and invocation based on
  dialed digits. It also handles CDMA flash if there are other calls connected.


SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_OriginateVoiceCall(ICM *pMe, AECHAR const *number,
                                    AEECMCallID *pcallID)
{
  return OEMCM_OriginateCall(pMe, AEECM_CALL_TYPE_VOICE, number, NULL, NULL,
                             pcallID);
}

/*=============================================================================
FUNCTION: OEMCM_OriginateCall

DESCRIPTION:
  Originate a call as specified by the call_type.

PROTOTYPE:
  int OEMCM_OriginateCall(ICM *po,
                        AEECMCallType call_type,
                        AECHAR const *number,
                        AECHAR const *alpha,
                        ICallOpts * options,
                        AEECMCallID  *pcallID)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  call_type         [in]: Type of call to be originated
  number            [in]: Number to be dialed
  alpha             [in]: Text to be displayed along with origination
  options           [in]: Call specific options, if required
  pcallID          [out]: Call ID assigned to the call

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EBADPARM:     If number is NULL or call_type is not defined.
  EFAILED:      Otherwise.

COMMENTS:
  Input parameters alpha and options are optional and can be NULL.
  When options paramater is NULL, default ICM options are used to make the call.

  OriginateCall handles GSM CLIR suppression and invocation based on
  dialed digits. It also handles CDMA flash if there are other calls connected.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_OriginateCall(ICM *pMe, AEECMCallType call_type,
                             AECHAR const *wszNum, AECHAR const *wszAlpha,
                             ICallOpts * options, AEECMCallID *pcallID)
{
  AECHAR                wszCalling[] = {'\0'};
  cm_num_s_type         calling_number, called_number;
  cm_orig_alpha_s_type  cm_alpha;
  uint8                 i;
  AEECMCallInfo *       call_info;
  AEECMPhInfo *         ph_info;
  AEECMSysMode          sys_mode;
  cm_cdma_orig_params_s_type cdma_orig;
  cm_cdma_orig_params_s_type * cdma_orig_ptr = NULL;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
  cm_gw_cs_orig_params_s_type * gw_orig_ptr = NULL;
  cm_srv_type_e_type    srv_type = CM_SRV_TYPE_AUTOMATIC;
  boolean               flash = FALSE;
  cm_call_id_type       flash_call_id = 0;
  cm_call_id_type       call_id;
  AEECMCallID           callIDs[OEMCM_MAX_NUM_CALLS];
  int                   ret;
  uint8                 ecc_cat;   //Emergency call category

  if (!pMe || !pMe->m_coreObj || !pcallID)
    return EBADPARM;

  sys_mode = OEMCM_GetSystemMode();

  if (AEECM_IS_MODE_CDMA(sys_mode)) {
    for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) {
      if (pMe->m_coreObj->m_state[i].bValid) {
        call_info = &(pMe->m_coreObj->m_call_info[i]);
        if (AEECM_IS_CALLSTATE_CONNECTED(call_info->call_state) &&
            AEECM_IS_CALL_MODE_CDMA(call_info->call_mode_info)  &&
            AEECM_IS_CALLTYPE_VOICE(call_info->call_type)) {
          flash = TRUE;
          flash_call_id = i;
          break;
        }
      }
    }
  }

  /* Return error if phone is offline */
  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL)
    return EFAILED;

  if (ph_info->oprt_mode != AEECM_OPRT_MODE_ONLINE)
    return EBADSTATE;


  /* If a number is supplied, make sure it is the correct length. */
  if (wszNum)
  {
    uint32 number_length = WSTRLEN(wszNum);

    if (number_length > sizeof(called_number.buf))
      return EBADPARM;
  }
  MSG_FATAL("OEMCM_OriginateCall %d %d",flash,flash_call_id,0);
  if (flash) {
    /* Handle flash */

    if (AEECM_IS_MODE_CDMA(sys_mode)) {
      /* Handle CDMA flash */

      if (OEMCM_IsEmergencyNumber(wszNum, &ecc_cat)) {
        /* Make an emergency call */
        return OEMCM_SendEmergencyFlash(pMe, wszNum, wszAlpha, pcallID, ecc_cat);
      } else {
        /* Send simple flash */
        if((ret = OEMCM_SendFlash(wszNum, flash_call_id)) == AEE_SUCCESS)
          *pcallID = OEMCM_AEECALLID(flash_call_id);
        return ret;
      }

    } else {
      /* Non-CDMA call origination with other calls, do not flash */
      if (!wszNum)
        return EUNSUPPORTED;
    }
  } else {
    /* Call origination */
    if (!wszNum)
      return EBADPARM;
  }

  /* Check if another voice call origination is in progress. */
  /*lint -save -e655 */
  if (AEECM_IS_CALLTYPE_VOICE(call_type) &&
      (OEMCM_GetActiveCallIDs(pMe,
                             (AEECMCallType)(AEECM_CALL_TYPE_VOICE |
                                AEECM_CALL_TYPE_EMERGENCY |
                                AEECM_CALL_TYPE_STD_OTASP |
                                AEECM_CALL_TYPE_NON_STD_OTASP),
                             AEECM_CALL_STATE_ORIG,
                             callIDs, sizeof(callIDs)) > 0)) {
    MSG_ERROR("Another voice call origination in progress %d",
               callIDs[0], 0, 0);
    return EBADSTATE;
  }
  /*lint -restore */

  /* Handle origination parameters */
  if (AEECM_IS_MODE_GW(sys_mode)) {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
  } else {

    cdma_orig_ptr = &cdma_orig;
    (void) MEMCPY(cdma_orig_ptr, &pMe->m_coreObj->def_cdma_orig_params,
                 sizeof(cm_cdma_orig_params_s_type));

    if (options)
      OEMCM_FillInCDMACallOptions(cdma_orig_ptr, &srv_type, options);
  }

  OEMCM_FillCallNum(&called_number, &calling_number, &cm_alpha,
                         wszNum, wszCalling, wszAlpha);

  ret = OEMCM_CMCallCmdOrig(pMe, OEMCM_CallTypeToDMSS(call_type),
                             srv_type,
                             &calling_number,
                             &called_number,
                             &cm_alpha,
                             cdma_orig_ptr,
                             gw_orig_ptr,
                             &call_id);

  if (ret == AEE_SUCCESS)
    *pcallID = OEMCM_AEECALLID(call_id);

  return ret;
}
#ifdef FEATURE_GSTK_FDN_CC_SUPPORT
/*=============================================================================
FUNCTION: OEMCM_OriginateCall_SkipFDNChk

DESCRIPTION:
  Originate a call as specified by the call_type.

PROTOTYPE:
  int OEMCM_OriginateCall_SkipFDNChk(ICM *po,
                        AEECMCallType call_type,
                        AECHAR const *number,
                        AECHAR const *alpha,
                        ICallOpts * options,
                        AEECMCallID  *pcallID)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  call_type         [in]: Type of call to be originated
  number            [in]: Number to be dialed
  alpha             [in]: Text to be displayed along with origination
  options           [in]: Call specific options, if required
  pcallID          [out]: Call ID assigned to the call

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EBADPARM:     If number is NULL or call_type is not defined.
  EFAILED:      Otherwise.

COMMENTS:
  Input parameters alpha and options are optional and can be NULL.
  When options paramater is NULL, default ICM options are used to make the call.

  OriginateCall handles GSM CLIR suppression and invocation based on
  dialed digits. It also handles CDMA flash if there are other calls connected.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_OriginateCall_SkipFDNChk(ICM *pMe, AEECMCallType call_type,
                             AECHAR const *wszNum, AECHAR const *wszAlpha,
                             ICallOpts * options, AEECMCallID *pcallID)
{
   gCMGenericParams.is_fdn_to_check = FALSE;
   return OEMCM_OriginateCall(pMe, call_type, wszNum, wszAlpha, options,
                              pcallID);
}
#endif
/*=============================================================================
FUNCTION: OEMCM_SendSetupResponse

DESCRIPTION:
  Sends the Setup Response.

PROTOTYPE:
  void OEMCM_SendSetupResponse(cm_call_id_type call_id,boolean accept)

PARAMETERS:
  call_id            [in]: Call ID of the call to be responded to
  accept             [in]: Accept or not to accept the call

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean OEMCM_SendSetupResponse(cm_call_id_type call_id,boolean accept)
{
  cm_setup_res_params_s_type setup_params;
#ifdef FEATURE_GSM_AMR
#error code not present
#endif

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM)*/


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */

  if (!ICMCoreObj)
    return FALSE;

  if (ICMCoreObj->m_call_info[call_id].call_state == AEECM_CALL_STATE_ENDED) {
    return FALSE;
  }


  (void) MEMSET(&setup_params, CM_CALL_CMD_PARAM_DEFAULT_VALUE,
         sizeof(cm_setup_res_params_s_type));

  setup_params.bearer_capability_1 = ICMCoreObj->def_gw_orig_params.
                                                      bearer_capability_1;
#ifdef FEATURE_GSM_AMR
#error code not present
#endif  /* FEATURE_GSM_AMR */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */

  setup_params.accept = accept;
  if (!cm_mm_call_cmd_setup_res(OEMCM_DMSSCallErrCB, NULL, gCMID,
                                  call_id,
                                  &setup_params)) {
    MSG_ERROR("OEMCM: cm_mm_call_cmd_setup_res failed", 0, 0, 0);
    return FALSE;
  }

  ICMCoreObj->m_state[call_id].bSetup = TRUE;

  return TRUE;

}

/*=============================================================================
FUNCTION: OEMCM_AnswerCall

DESCRIPTION:
  Answer an incoming call.

PROTOTYPE:
  int OEMCM_AnswerCall(ICM *po,
                        AEECMCallID callID)

PARAMETERS:
  po                [in]: ICM interface pointer.
  callID            [in]: Call ID of the call to be answered

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_AnswerCall(ICM *pMe, AEECMCallID callID)
{
  AEECMCallInfo *         call_info;
  AEECMPhInfo *           ph_info;
  AEECMCallType           call_type;
  boolean                 ans = FALSE;
  cm_call_id_type         call_id;
  uint8                   i;
  AECHAR                  twoSend[] = {'2', '\0'};
  AEECMPrivateCallState  *m_state;

  if (!pMe || !pMe->m_coreObj)
    return EBADPARM;

  if (!OEMCM_IsValidAppCallID(callID))
    return EBADPARM;

  /* Change App call id to DMSS call id */
  call_id = OEMCM_CMCALLID(callID);

  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL)
    return EFAILED;

  if (ph_info->oprt_mode != AEECM_OPRT_MODE_ONLINE)
    return EBADSTATE;

  for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) {
    call_info = &(pMe->m_coreObj->m_call_info[i]);

    if (pMe->m_coreObj->m_state[i].bValid)
    {
      /*For CDMA, the call ID should be the same as the call ID that is supplied.
        FOR GSM, the call ID should not be the same =>
        If it not CDMA (then it is GSM) and it not the same call id, then the condition will still
        hold true.*/
      if(!(call_info->call_mode_info!=AEECM_CALL_MODE_INFO_CDMA && i == call_id))
      {
        if ((call_info->call_state == AEECM_CALL_STATE_CONV) &&
           AEECM_IS_CALLTYPE_VOICE(call_info->call_type)) {

          switch(call_info->call_mode_info) {
            case AEECM_CALL_MODE_INFO_CDMA:
              return OEMCM_SendFlash(NULL, i);

            case AEECM_CALL_MODE_INFO_GW_CS:
            case AEECM_CALL_MODE_INFO_IP:
              return OEMCM_CallOpsRequest(pMe, twoSend, callID);

            default:
              break;
          }

          break; /* no need to look further */
        }
      }
    }
  }

  call_info = &(pMe->m_coreObj->m_call_info[call_id]);
  if (call_info->call_state != AEECM_CALL_STATE_INCOM)
    return EBADPARM;

  if (call_info->alert_ans_bypass)
    return EBADSTATE;

  call_type = OEMCM_GetCallType(call_id);
  if (!ISVOICE(call_type))
    return EBADSTATE;

  m_state = &pMe->m_coreObj->m_state[call_id];

  /* check if somebody else has already answered the call */
  /* Assuming that nobody else directly uses DMSS CM to answer calls
     that BREW Apps are interested in */
  AEECM_MUTEX_LOCK();
  if (!m_state->bAnswered) {
    m_state->bAnswered = TRUE;
    ans = TRUE;
  }
  AEECM_MUTEX_UNLOCK();

  if (!ans) {
    MSG_ERROR("Already answered this call: %d", call_id, 0, 0);
    return EBADSTATE;
  }

  return OEMCM_AnswerCall_Int( pMe, call_id, OEMCM_CallTypeToDMSS(call_type) );
}
/*=============================================================================
FUNCTION: OEMCM_AnswerCall_Int

DESCRIPTION:
  Answer an incoming call.

PROTOTYPE:
  int OEMCM_AnswerCall_Int(ICM *po,
                        cm_call_id_type call_id,
                        cm_call_type_e_type cm_call_type)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID of the call to be answered
  cm_call_type      [in]: Type of call to be answered

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_AnswerCall_Int( ICM *pMe, cm_call_id_type call_id, cm_call_type_e_type cm_call_type )
{
   cm_ans_params_s_type    answer_params;
   AEECMEventCBS *         event_cb;
   AEECMCallErrCBPkt *     err_cb_pkt;
   AEECMPrivateCallState  *m_state;
#if defined (FEATURE_ACM)
   OEMCM_ACM_NetworkOriginatedCallInfo * ACM_NOCallInfo;
#endif // FEATURE_ACM

   m_state = &pMe->m_coreObj->m_state[call_id];

  (void) MEMSET( &answer_params, 0, sizeof( cm_ans_params_s_type));

   answer_params.info_type = (cm_call_mode_info_e_type)pMe->m_coreObj->m_call_info[call_id].call_mode_info;

  if (AEECM_IS_CALL_MODE_GW((AEECMCallModeInfo)answer_params.info_type)) {
      answer_params.ans_params.gw_cs_ans.accept = TRUE;
      answer_params.ans_params.gw_cs_ans.call_type = cm_call_type;
  } else {
      answer_params.ans_params.cdma_ans.call_type = cm_call_type;
  }


  pMe->m_coreObj->m_call_owners[call_id] = pMe;

  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, sizeof(AEECMCallErrCBPkt));

  if (!event_cb) {
    MSG_ERROR("OEMCM_AnswerCall: Failed to allocate event callback",
               0, 0, 0);
    m_state->bAnswered = FALSE;
    return EFAILED;
  }

  /* Fill in call_id here. */
  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
  err_cb_pkt->call_id = call_id;

#if defined (FEATURE_ACM)
  if (CM_CALL_TYPE_VOICE == cm_call_type || CM_CALL_TYPE_EMERGENCY == cm_call_type)
  {
    /* Fill in data required for acquire & delayed acquire for Network Originated voice calls */
    ACM_NOCallInfo = (OEMCM_ACM_NetworkOriginatedCallInfo *)
                               OEMCM_MALLOC (sizeof(OEMCM_ACM_NetworkOriginatedCallInfo));

    ACM_NOCallInfo->data_block_ptr    = event_cb;
    ACM_NOCallInfo->client_id         = gCMID;
    ACM_NOCallInfo->call_id           = call_id;
    MEMCPY(&ACM_NOCallInfo->answer_params, &answer_params, sizeof(cm_ans_params_s_type));

    ICMCoreObj->m_pAcmInfo->m_pCBInfo = ACM_NOCallInfo;

    MSG_HIGH ("OEMCM: Check with ACM for MT Voice Call resources", 0, 0, 0);
    /* Acquire resource from Application Concurrency Manager */
    return OEMCM_ACMAcquire(pMe, ICMCoreObj->m_pAcmInfo, ACM_NETWORK_ORIGINATED_VOICE_CALL);
  }
  else
  {
    /* Invoke CM cmd */
    if (!cm_mm_call_cmd_answer(OEMCM_DMSSCallErrCB, event_cb, gCMID, call_id,&answer_params))
    {
      m_state->bAnswered = FALSE;
      return EFAILED;
    }
    return AEE_SUCCESS;
  }
#else // !FEATURE_ACM
  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_answer(OEMCM_DMSSCallErrCB, event_cb, gCMID, call_id,
                             &answer_params)) {
    m_state->bAnswered = FALSE;
    return EFAILED;
  }

  return AEE_SUCCESS;
#endif // FEATURE_ACM
}


/*=============================================================================
FUNCTION: ICM_EndCall

DESCRIPTION:
  End a call specified by call id.

PROTOTYPE:
  int ICM_EndCall(ICM *po,
                  AEECMCallID callID)

PARAMETERS:
  po                [in]: ICM interface pointer.
  callID            [in]: Call ID of the call to be ended

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  End reason is assumed to be always normal.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_EndCall(ICM *pMe, AEECMCallID callID)
{
  AEECMCallState        call_state;
  AEECMPhInfo *         ph_info = NULL;
  boolean               end = FALSE;
  cm_call_id_type           call_id;
  AEECMPrivateCallState *m_state;

  if (!pMe || !pMe->m_coreObj)
    return EBADPARM;

  if (!OEMCM_IsValidAppCallID(callID))
    return EBADPARM;

  /* Change App call id to DMSS call id */
  call_id = OEMCM_CMCALLID(callID);

  call_state = OEMCM_GetCallState(call_id);
  if (call_state == AEECM_CALL_STATE_IDLE ||
      call_state == AEECM_CALL_STATE_ENDED ||
      call_state == AEECM_CALL_STATE_SETUP)
    return EBADSTATE;

  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL)
    return EFAILED;

  if (ph_info->oprt_mode != AEECM_OPRT_MODE_ONLINE) {
    return EBADSTATE;
  }

  m_state = &pMe->m_coreObj->m_state[call_id];

  if ((call_state == AEECM_CALL_STATE_INCOM) &&
      (AEECM_IS_CALL_MODE_GW(pMe->m_coreObj->m_call_info[call_id].call_mode_info))
      && (!m_state->bAnswered)) {
    return OEMCM_RejectCall(pMe, callID);
  }

  /* check if somebody else has already ended the call */
  AEECM_MUTEX_LOCK();
  if (!m_state->bEnded) {
    m_state->bEnded = TRUE;
    end = TRUE;
  }
  AEECM_MUTEX_UNLOCK();

  if (!end) {
    MSG_ERROR("Already ended this call: %d", call_id, 0, 0);
    return EBADSTATE;
  }

  return OEMCM_EndCall_Int(pMe,call_id);
}

/*=============================================================================
FUNCTION: OEMCM_EndCall_Int

DESCRIPTION:
  End a call specified by call id.

PROTOTYPE:
  int OEMCM_EndCall_Int(ICM *pMe,
                       cm_call_id_type call_id )

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID of the call to be ended

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  End reason is assumed to be always normal.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_EndCall_Int( ICM *pMe, cm_call_id_type call_id )
{
   cm_end_params_s_type  end_param;
   AEECMEventCBS *        event_cb;
   AEECMCallErrCBPkt *   err_cb_pkt;
   AEECMPrivateCallState *m_state;

   m_state = &pMe->m_coreObj->m_state[call_id];

  (void ) MEMSET(&end_param, 0, sizeof(cm_end_params_s_type));
  end_param.call_id = call_id;

  if (AEECM_IS_CALL_MODE_GW(pMe->m_coreObj->m_call_info[call_id].call_mode_info))
  {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
  } else {
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    end_param.info_type = (cm_call_mode_info_e_type)pMe->m_coreObj->m_call_info[call_id].call_mode_info;
    end_param.end_params.cdma_end.end_reason_included = TRUE;
    end_param.end_params.cdma_end.end_reason = CAI_REL_NORMAL;
#ifdef FEATURE_DS_END_KEY_DROPS_CALL
#define OEMCM_PROTOCOL_REV_6 6
    if ((OEMCM_GetCallType(call_id) == AEECM_CALL_TYPE_PS_DATA) &&
        (pMe->m_coreObj->m_p_rev_in_use >= OEMCM_PROTOCOL_REV_6))
      end_param.end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
#endif /* FEATURE_DS_END_KEY_DROPS_CALL */
#else
    MSG_ERROR("CDMA Feature not defined in this build !!", 0, 0, 0);
    m_state->bEnded = FALSE;
    return EUNSUPPORTED;
#endif
  }

  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, sizeof(AEECMCallErrCBPkt));

  if (!event_cb) {
    MSG_ERROR("OEMCM_EndCall: Failed to allocate event callback",
               0, 0, 0);
    m_state->bEnded = FALSE;
    return EFAILED;
  }

  /* Fill in call_id here. */
  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
  err_cb_pkt->call_id = call_id;

  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_end(OEMCM_DMSSCallErrCB,
                          event_cb, gCMID, 1, &end_param)) {
    m_state->bEnded = FALSE;
    return EFAILED;
  }

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_EndAllCalls

DESCRIPTION:
  End calls specified in the call list

PROTOTYPE:
  int OEMCM_EndAllCalls(ICM *po)

PARAMETERS:
  po                [in]: ICM interface pointer.

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  End reason is assumed to be normal.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_EndAllCalls(ICM *pMe)
{
  AEECMCallID callID_list[OEMCM_MAX_NUM_CALLS];
  uint16 i;
  uint16 num;
  int ret_val = AEE_SUCCESS;
  int ret;

  if (!pMe)
    return EBADPARM;

  num = OEMCM_GetActiveCallIDs(pMe, AEECM_CALL_TYPE_NONE,
                               AEECM_CALL_STATE_NONE,
                               callID_list,
                               sizeof(callID_list));

  for (i = 0; i < num; i++) {
    ret = OEMCM_EndCall(pMe, callID_list[i]);
    if (ret != AEE_SUCCESS) {
      /* Try ending other calls */
      ret_val = EFAILED;
      MSG_ERROR("OEMCM_EndAllCalls failed to end call %d with error %d\n",
                                              callID_list[i], ret, 0);
    }
  }

  return ret_val;
}

/*=============================================================================
FUNCTION: OEMCM_CountActiveCalls

DESCRIPTION:
  Obtain the number of active calls matching a call_type, or all calls if call_type is
  AEECM_CALL_TYPE_NONE. Return value gives the number of active calls and if not NULL,
  map flags for active calls into a boolean array

PROTOTYPE:
  uint16 OEMCM_CountActiveCalls( AEECMCallType call_type,
                                 AEECMCallState call_state,
                                 boolean * callActiveFlags)

PARAMETERS:
  call_type         [in]: Call type
  call_state        [in]: Call state
  flag              [out]: active call flags

RETURN VALUE:
  Number of calls, which calls are active

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
static uint16 OEMCM_CountActiveCalls(AEECMCallType call_type,
                              AEECMCallState call_state,
                              boolean * callActiveFlags)
{
  int  i;
  uint16  num = 0;
  AEECMCallState icall_state;

  for (i = 0;  i < OEMCM_MAX_NUM_CALLS; ++i) {

    icall_state = OEMCM_GetCallState(i);
    MSG_FATAL("OEMCM_CountActiveCalls %d 0x%x",i,icall_state,0);
    if (icall_state == AEECM_CALL_STATE_NONE ||
        icall_state == AEECM_CALL_STATE_IDLE ||
        icall_state == AEECM_CALL_STATE_ENDED ||
        icall_state == AEECM_CALL_STATE_SETUP)
      continue;

    if ((call_type == AEECM_CALL_TYPE_NONE ||
        ((int)call_type & (int)OEMCM_GetCallType(i))) &&
        (call_state == AEECM_CALL_STATE_NONE ||
        ((int)call_state & (int)icall_state))) {
      if ( callActiveFlags != NULL) callActiveFlags[i] = TRUE;
      num++;
    }
  }
  return num;
}

/*=============================================================================
FUNCTION: OEMCM_GetActiveCallIDs

DESCRIPTION:
  Obtain the calls matching a call_type, or all calls if call_type is
  AEECM_CALL_TYPE_NONE. Return value gives the number of call ids filled
  in.

  The function can be called without specifying the callIDs array to obtain
  just the number of calls.

PROTOTYPE:
  uint16 ICM_GetActiveCallIDs(ICM *po,
                    AEECMCallType call_type,
                    AEECMCallState call_state,
                    AEECMCallID *callIDs,
                    uint32 size)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_type         [in]: Call type
  call_state        [in]: Call state
  callIDs           [out]: Optional array of Call IDs to be filled in
  size              [in]: Optional size of call ID array in bytes
                          (Required if callIDs is non-NULL)

RETURN VALUE:
  Number of calls or the number of call ids stored in the
  call ID array, if specified.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint16 OEMCM_GetActiveCallIDs(ICM *pMe, AEECMCallType call_type,
                             AEECMCallState call_state,
                             AEECMCallID *callIDs, uint32 size)
{
  boolean flag[OEMCM_MAX_NUM_CALLS] = {FALSE,};
  uint16 num = 0;
  uint8 i, j;

  if (!pMe)
    return 0;

  num = OEMCM_CountActiveCalls( call_type, call_state, flag);

  if (!callIDs)
    return num;

  j = 0;
  for(i = 0; (i < OEMCM_MAX_NUM_CALLS) && (j < size/sizeof(AEECMCallID)); i++) {
    if (flag[i])
      callIDs[j++] = OEMCM_AEECALLID(i);
  }

  return j;
}

/*=============================================================================
FUNCTION: OEMCM_GetCallInfo

DESCRIPTION:
  Get information about the call specified by call id.

PROTOTYPE:
  int OEMCM_GetCallInfo(ICM *po,
                      AEECMCallID callID,
                      AEECMCallInfo *call_info,
                      uint32 size)

PARAMETERS:
  po                [in]: ICM interface pointer.
  callID            [in]: Call ID of the call to be queried
  call_info        [out]: Return call information
  size              [in]: Size of Call info structure (bytes).


RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_GetCallInfo(ICM *pMe, AEECMCallID callID,
                             AEECMCallInfo *call_info,
                             uint32 size)
{
  AEECMCallInfo *core_call_info;

  if (!pMe || !pMe->m_coreObj || !call_info)
    return EBADPARM;

  if (!OEMCM_IsValidAppCallID(callID))
    return EBADPARM;

  core_call_info = &pMe->m_coreObj->m_call_info[OEMCM_CMCALLID(callID)];

  /* Update System mode */
  core_call_info->sys_mode = OEMCM_GetSystemMode();

  /* Update Data Call Stats */
  OEMCM_RefreshDataCallStats(pMe->m_coreObj, callID);

  (void ) MEMCPY(call_info, core_call_info, size);

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_SetPrivacyPref

DESCRIPTION:
  Change privacy preference for all calls while a call is in progress.

PROTOTYPE:
  int OEMCM_SetPrivacyPref(ICM *po,
                            AEECMPrivacyPref privacy_pref)

PARAMETERS:
  po                [in]: ICM interface pointer.
  privacy_pref      [in]: New privacy preference

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA.
  EFAILED:      Otherwise.

COMMENTS:
  This command can be used only when a call is going on and applies to all
  calls. Valid only in CDMA mode.
  Out-of-call privacy preference change is not supported through lower layers
  at this time.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetPrivacyPref(ICM *pMe, AEECMPrivacyPref privacy_pref)
{
  AEECMSysMode          sys_mode;
  AEECMCallID           callID;

  if (!pMe)
    return EBADPARM;

  sys_mode = OEMCM_GetSystemMode();

  if (!AEECM_IS_MODE_CDMA(sys_mode))
    return EUNSUPPORTED;

  if (!IN_RANGE(privacy_pref, AEECM_PRIVACY_PREF_STANDARD,
                AEECM_PRIVACY_PREF_ENHANCED)) {
    return EBADPARM;
  }

  /* Workaround: Find a valid call id since CM fix for not checking call ids
   * is not yet there in all targets */
  if (!OEMCM_GetActiveCallIDs(pMe, AEECM_CALL_TYPE_NONE,
                              AEECM_CALL_STATE_CONV,
                              &callID, sizeof(callID))) {
    return EBADSTATE;
  }

  /* Lower layer errors will not reach App */
  if (!cm_mm_call_cmd_privacy_pref(OEMCM_DMSSCallErrCB, NULL, gCMID,
                                   OEMCM_CMCALLID(callID),
                                   OEMCM_PrivacyPrefToDMSS(privacy_pref))) {
    return EFAILED;
  }

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_SetCallOption

DESCRIPTION:
  Set a call option in the call options list. This object can be passed later
  to ICM_OriginateCall() to specify supported options. Multiple options can be
  specified by calling this function multiple times.

PROTOTYPE:
  int OEMCM_SetCallOption(ICM *pMe,
                        AEECMCallOptions options,
                        AEECMCallOptionType  option_to_set,
                        void * pData)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  options           [in]: Call options object
  option_to_set     [in]: Option type to be set
  pData             [in]: Data for the specified option type

RETURN VALUE:
  AEE_SUCCESS:  If successful
  ENOMEMORy:    Out of memory
  EFAILED:      Otherwise.

COMMENTS:
  Data will be copied internally to the call options object.
  ICM_SetCallOption can be called multiple times to set different options.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: ICM_CallOpsStringCheck

DESCRIPTION:
  Check if the dialed string is a call related operation which does
  not result in call origination.

  In some cases ICM_CallOpsRequest() need a call_id associated with the
  number that user dialed. For eg: the call_id assocated with X if
  user types 1 X <send>. call_num will contain the value X in this case
  after ICM_CallOpsStringCheck() is called.

  If display used by Application is different from the call_id provided
  by ICM, X should be mapped to the appropriate call_id before calling
  ICM_CallOpsRequest(). Otherwise, the call_num returned can be directly
  used in ICM_CallOpsRequest().

PROTOTYPE:
  int ICM_CallOpsStringCheck(ICM *pMe,
                             AEECHAR * dialed_string,
                             uint8 *call_num)

PARAMETERS:
  po                [in]: ICM interface pointer.
  dialed_string     [in]: Dialed String (NULL terminated)
  call_num       [out]:   The number used to refer to a call.
                          (X in 1 X <send>)

RETURN VALUE:
  AEECM_IS_CALLOPS:             If dialed string is a call related operation.
  AEECM_IS_CALLOPS_NEED_CALLID: If dialed string is a call related operation
                           and ICM_CallOpsRequest() need a call id.
                           In this case call_num is filled in.
  AEECM_NOT_CALLOPS:            If dialed string is not a call related operation.
  EFAILED:                 Otherwise

COMMENTS:
  When user dials a number the first check should be whether it is a call
  independent supplementary service string (ISUPPSVC_StringCheck).
  If that check fails, ICM_CallOpsStringCheck() should be called.

  If ICM_CallOpsStringCheck() returns AEECM_IS_CALLOPS or AEECM_IS_CALLOPS_NEED_CALLID,
  ICM_CallOpsRequest() should be called. Otherwise the dialed string should be
  used to Originate[Voice]Call().

SIDE EFFECTS:

SEE ALSO:
  ICM_CallOpsRequest(), ISUPPSVC_StringCheck(), ISUPPSVC_StartSS(),
  ICM_OriginateVoiceCall()

=============================================================================*/
static AEECMIsCallOps OEMCM_CallOpsStringCheck(ICM *pMe,
                             AECHAR * dialed_string,
                             uint8 *call_num)
{
  AEECMCallState         call_state;
  AEECMCallType          call_type;
  boolean                connected = FALSE;
  int                    str_len;
  uint8                  i;
  AEECMIsCallOps         ret = AEECM_NOT_CALLOPS;
  AEECMCallID            connectedCallID = 0;

  if (!pMe || !pMe->m_coreObj || !dialed_string)
    return AEECM_NOT_CALLOPS;

  for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) {
    call_state = OEMCM_GetCallState(i);
    call_type = OEMCM_GetCallType(i);
    if ((AEECM_IS_CALLSTATE_CONNECTED(call_state) ||
        call_state == AEECM_CALL_STATE_INCOM) &&
        AEECM_IS_CALLTYPE_VOICE(call_type)) {
      connectedCallID = i;
      connected = TRUE;
      break;
    }
  }

  str_len = WSTRLEN(dialed_string);
  if (str_len == 0)
    return AEECM_NOT_CALLOPS;


  if (AEECM_IS_CALL_MODE_GW(pMe->m_coreObj->m_call_info[connectedCallID].call_mode_info)) {
    switch(dialed_string[0]) {
      case '0':
      case '3':
        if (!connected)
        {
            return AEECM_NOT_CALLOPS;
        }
        if (str_len == 1)
          ret = AEECM_IS_CALLOPS;
        break;

      case '5':
        if (str_len == 1)
          ret = AEECM_IS_CALLOPS;
        break;

      case '1':
      case '2':
        if (!connected)
        {
          return AEECM_NOT_CALLOPS;
        }

        if (str_len > 2)
          break;

        if (str_len == 1) {
          ret = AEECM_IS_CALLOPS;
        } else {

          if (!call_num)
            return AEECM_NOT_CALLOPS;

          *call_num = (uint8) WSTRTOFLOAT(dialed_string+1);
          ret = AEECM_IS_CALLOPS_NEED_CALLID;
        }
        break;

      case '4':
        if (!connected)
        {
          return AEECM_NOT_CALLOPS;
        }

        if (str_len == 1 ||
            (str_len > 2 && dialed_string[1] == '*'))
          ret = AEECM_IS_CALLOPS;
        break;

      default:
        break;
    }

  }

  return ret;
}

/*=============================================================================
FUNCTION: ICM_CallOpsRequest

DESCRIPTION:
  Send a call related supplementary service request

PROTOTYPE:
  int ICM_CallOpsRequest(ICM *po,
                         AECHAR * dialed_string,
                         AEECMCallID callID)

PARAMETERS:
  po                [in]: ICM interface pointer.
  dialed_string     [in]: Dialed String (NULL terminated)
  callID            [in]: Optional callID

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EBADPARM:     If callID is not valid in an operation that
                requires callID.
  EFAILED:      Otherwise.

COMMENTS:
  When user dials a number the first check should be whether it is a call
  independent supplementary service string (ISUPPSVC_StringCheck).
  If that check fails, ICM_CallOpsStringCheck() should be called.

  If ICM_CallOpsStringCheck() returns AEECM_IS_CALLOPS or AEECM_IS_CALLOPS_NEED_CALLID,
  ICM_CallOpsRequest() should be called. Otherwise the dialed string should be
  used to Originate[Voice]Call().

SIDE EFFECTS:

SEE ALSO:
  ICM_CallOpsStringCheck()

=============================================================================*/
static int OEMCM_CallOpsRequest(ICM *pMe,
                                AECHAR * dialed_string,
                                AEECMCallID callID)
{
  char                        number[CM_MAX_DIALED_DIGITS+1];
  cm_call_sups_params_s_type  sups_params;
  cm_call_sups_params_s_type * sups_params_ptr;
  cm_call_sups_type_e_type    sups_type = (cm_call_sups_type_e_type)0;
  AEECMPhInfo *               ph_info;
  AEECMEventCBS *             event_cb;
  AEECMCallErrCBPkt *         err_cb_pkt;
  int                         str_len;
  uint8                       i;
  AEECMCallState              call_state;
  AEECMCallType               call_type;
  boolean                     connected = FALSE;
  boolean                     call_ind_sups = FALSE;
  cm_call_id_type             call_id = OEMCM_CMCALLID(callID);
  AEECMCallID                 connectCallID = 0;
  boolean                     cdma = FALSE;

  if (!pMe || !pMe->m_coreObj || !dialed_string)
    return EBADPARM;

  for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) {
    call_state = OEMCM_GetCallState(i);
    call_type = OEMCM_GetCallType(i);
    if ((AEECM_IS_CALLSTATE_CONNECTED(call_state) ||
        call_state == AEECM_CALL_STATE_INCOM) &&
        AEECM_IS_CALLTYPE_VOICE(call_type))
    {
      connectCallID = i;
      connected = TRUE;
    }
  }

  /* Return error if phone is offline */
  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL)
    return EFAILED;

  if (ph_info->oprt_mode != AEECM_OPRT_MODE_ONLINE)
    return EBADSTATE;

  str_len = WSTRLEN(dialed_string);

  if (str_len == 0)
    return EBADPARM;



  /* CDMA cases */
  /* If call is in CDMA, let the network handle all requests.*/

  if (!OEMCM_IsValidAppCallID(callID))
  {
    if(pMe->m_coreObj->m_call_info[callID].call_mode_info == AEECM_CALL_MODE_INFO_CDMA)
      cdma = TRUE;
  }
  else
  {
    if(pMe->m_coreObj->m_call_info[connectCallID].call_mode_info == AEECM_CALL_MODE_INFO_CDMA)
      cdma = TRUE;
  }

  if (cdma) {
      return EUNSUPPORTED;
  }

  if (str_len == 1) {
    (void ) MEMSET(&sups_params, 0, sizeof(cm_call_sups_params_s_type));
    sups_params_ptr = &sups_params;

    //Get the Last Connected Call ID
    if(connected)
      sups_params_ptr->call_id = connectCallID;

    switch(dialed_string[0]) {
      case '0':
        if(!connected)
          return EBADSTATE;
        sups_type = CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB;
        break;

      case '1':
        if(!connected)
          return EBADSTATE;
          sups_type = CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL;
        break;

      case '2':
        if(!connected)
          return EBADSTATE;
          sups_type = CM_CALL_SUPS_TYPE_HOLD_CALL;
        break;

      case '3':
        if(!connected)
          return EBADSTATE;
          sups_type = CM_CALL_SUPS_TYPE_MULTIPARTY_CALL;
        break;

      case '4':
        if(!connected)
          return EBADSTATE;
        sups_type = CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER;
        break;

#ifdef FEATURE_CCBS
#error code not present
#endif

      default:
        return EBADPARM;
    }
  } else {
    (void ) MEMSET(&sups_params, 0, sizeof(cm_call_sups_params_s_type));
    sups_params_ptr = &sups_params;

    switch(dialed_string[0]) {
      case '1':
        if(!connected)
          return EBADSTATE;
        if (!OEMCM_IsValidAppCallID(callID))
          return EBADPARM;

        sups_params_ptr->call_id = call_id;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
        sups_type = CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL;
        break;

      case '2':
        if(!connected)
          return EBADSTATE;

        if (!OEMCM_IsValidAppCallID(callID))
          return EBADPARM;

        sups_params_ptr->call_id = OEMCM_CMCALLID(callID);
        sups_type = CM_CALL_SUPS_TYPE_SELECT_CALL_X;
        break;

      case '4':
        if(!connected)
          return EBADSTATE;

        if (str_len > 2 && dialed_string[1] == '*') {
          AEECMCallID incomCallID;

          if (!OEMCM_GetActiveCallIDs(pMe, AEECM_CALL_TYPE_NONE,
                                      AEECM_CALL_STATE_INCOM,
                                      &incomCallID, sizeof(incomCallID))) {
            return EBADSTATE;
          }

          sups_params_ptr->call_id = OEMCM_CMCALLID(incomCallID);

          (void ) WSTRTOSTR(dialed_string+2,
                            number, sizeof(number));
          OEMCM_InitNum(&sups_params_ptr->sups_number, number);

          sups_type = CM_CALL_SUPS_TYPE_CALL_DEFLECTION;
        } else {
          return EBADPARM;
        }
        break;

      default:
        return EBADPARM;
    }
  }

  if (!call_ind_sups) {

    event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, sizeof(AEECMCallErrCBPkt));

    if (!event_cb)
      return EFAILED;

    err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;

    if (sups_type == CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL ||
        sups_type == CM_CALL_SUPS_TYPE_SELECT_CALL_X)
      err_cb_pkt->call_id = call_id;

    if (!cm_mm_call_cmd_sups(OEMCM_DMSSCallErrCB, event_cb, gCMID,
                             sups_type,
                             sups_params_ptr)) {
      return EFAILED;
    }

  } else {
    /* TODO */
  }

  return AEE_SUCCESS;


}

/*---------------------------------------------------------------------------
                          PHONE FUNCTIONS
---------------------------------------------------------------------------*/


/*=============================================================================
FUNCTION: OEMCM_SetSystemPreference

DESCRIPTION:
  Change the phone's system selection preference (mode, band, and
  roam preference).

PROTOTYPE:
  int ICM_SystemPreference(ICM *po,
                        AEECMModePref mode_pref,
                        AEECMPrefTerm pref_term,
                        uint32 pref_duration,
                        AEECMAcqOrderPref acq_order_pref,
                        AEECMBandPref band_pref,
                        AEECMRoamPref roam_pref,
                        AEECMHybrPref hybr_pref,
                        AEECMServDomPref srv_domain_pref,
                        AEECMNwSelModePref nw_sel_mode_pref,
                        AEECMPLMNID * plmn_ptr,
                        AEECMPhError *status,
                        AEECallback *status_cb)

PARAMETERS:
  po                [in]: ICM interface pointer.
  mode_pref         [in]: Preferred mode
  pref_term         [in]: Preference span
  pref_duration     [in]: Indicates duration in seconds for pref term
  acq_order_pref    [in]: GW acquisition order preference
  band_pref         [in]: Band preference
  roam_pref         [in]: Roam preference
  hybr_pref         [in]: Hybrid Preference
  srv_domain_pref   [in]: Service domain preference
  nw_sel_mode_pref  [in]: Network should be selected automatically or manually
  plmn_ptr          [in]: If network selection is set to manual,
                          this specifies PLMN ID
  wlan_ptr          [in]: WLAN preference
  status_cb         [in]: Status Callback

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  If status_cb is registered, asynchronous result of the function from lower
  layers is provided to the application. The function always returns
  immediately without waiting for the result from lower layers irrespective of
  whether status_cb is used or not.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
//lint -save -esym(715, wlan_ptr)   "not referenced"
static int OEMCM_SetSystemPreference(ICM *pMe, AEECMModePref mode_pref,
                             AEECMPrefTerm pref_term, uint32 pref_duration,
                             AEECMAcqOrderPref acq_order_pref,
                             AEECMBandPref band_pref, AEECMRoamPref roam_pref,
                             AEECMHybrPref hybr_pref,
                             AEECMServDomPref srv_domain_pref,
                             AEECMNwSelModePref nw_sel_mode_pref,
                             AEECMPLMNID * plmn_ptr,
                             AEECMWLANPref * wlan_ptr,
                             AEECMPhError *status,
                             AEECallback *cb)
{
  AEECMErrCBS * err_cb = NULL;
#if defined(FEATURE_WLAN) && defined(CM_API_WLAN)
#error code not present
#endif
  cm_band_pref_e_type band_pref_conv;

  MSG_MED("OEMCM_SetSystemPreference called. mode_pref=%d, pref_term=%d, pref_duration=%d",
          mode_pref, pref_term, pref_duration);
  MSG_MED("OEMCM_SetSystemPreference: acq_order_pref=%d, ", acq_order_pref, 0, 0);
  MSG_MED("OEMCM_SetSystemPreference: band_pref-hi=0x%08x, band_pref-lo=0x%08x",
          (band_pref > 32), (uint32)band_pref, 0);
  MSG_MED("OEMCM_SetSystemPreference: roam_pref=0x%08x, hybr_pref=%d, srv_domain_pref=%d",
          roam_pref, hybr_pref, srv_domain_pref);
  MSG_MED("OEMCM_SetSystemPreference: nw_sel_mode_pref=%d", nw_sel_mode_pref, 0, 0);

  if (plmn_ptr != NULL)
  {
    MSG_MED("OEMCM_SetSystemPreference: plmn=0x%08x", *(uint32 *)plmn_ptr, 0, 0);
  }
  else
  {
    MSG_MED("OEMCM_SetSystemPreference: NULL plmn pointer", 0, 0, 0);
  }

  if (wlan_ptr != NULL)
  {
    MSG_MED("OEMCM_SetSystemPreference: W-nw_sel_mode_pref=%d, W-scan_mode=%d",
            wlan_ptr->network_sel_mode_pref, wlan_ptr->scan_mode, 0);
    MSG_MED("OEMCM_SetSystemPreference: W-bss_id-hi=0x%08x, W-bss_id_lo=0x%08x",
            (wlan_ptr->bss_id >> 32), (uint32)wlan_ptr->bss_id, 0);
    MSG_MED("OEMCM_SetSystemPreference: W-ssid=0x%08x, W-chan=0x%04x, W-tech_pref=0x%08x",
            (uint32)(wlan_ptr->ssid), wlan_ptr->chan, wlan_ptr->tech_pref);
    MSG_MED("OEMCM_SetSystemPreference: W-bss_type=%d, W-bss_type_pref=%d",
            wlan_ptr->bss_type, wlan_ptr->bss_type_pref, 0);
    MSG_MED("OEMCM_SetSystemPreference: W-encrypt=%d, W-auth=%d",
            wlan_ptr->encrypt, wlan_ptr->auth, 0);
    MSG_MED("OEMCM_SetSystemPreference: W-band-hi=0x%08x, W-band-lo=0x%08x",
            (wlan_ptr->band >> 32), (uint32)wlan_ptr->band, 0);
  }
  else
  {
    MSG_MED("OEMCM_SetSystemPreference: NULL WLAN pointer", 0, 0, 0);
  }

  if (!pMe)
    return EBADPARM;

  if (cb) {

    if (!cb->pfnNotify) {
      MSG_ERROR("Null function pointer provided as callback", 0, 0, 0);
      return EBADPARM;
    }

    if (!status) {
      MSG_ERROR("Null status pointer", 0, 0, 0);
      return EBADPARM;
    }

    err_cb = OEMCM_AllocErrCBS(pMe);
    if (!err_cb) {
      MSG_ERROR("Unable to get ICM Error Callback", 0, 0, 0);
      return EFAILED;
    }

    err_cb->pcb = cb;
    err_cb->pError = status;

    CALLBACK_Cancel(cb);

    cb->pfnCancel = OEMCM_AppCancelErrCB;
    cb->pCancelData = err_cb;
  }

  if (plmn_ptr)
    (void) MEMCPY(&pMe->m_cm_plmn_id, plmn_ptr, sizeof(sys_plmn_id_s_type));
  else
    (void) MEMSET(&pMe->m_cm_plmn_id, 0xFF, sizeof(sys_plmn_id_s_type));

  band_pref_conv = OEMCM_BandPrefToDMSS(band_pref);

#if defined(FEATURE_WLAN) && defined(CM_API_WLAN)
#error code not present
#endif /*  defined(FEATURE_WLAN) && defined(CM_API_WLAN) */

#ifdef CM_API_SYS_SEL_PRL_PREF
  if (!cm_ph_cmd_sys_sel_pref(
                           OEMCM_DMSSPhErrCB,
                           (err_cb ? (void *)err_cb->hObject : NULL),
                           gCMID,
                           (cm_mode_pref_e_type) mode_pref,
                           OEMCM_PrefTermToDMSS(pref_term),
                           (dword) pref_duration,
                           OEMCM_AcqOrderPrefToDMSS(acq_order_pref),
                           band_pref_conv,
                           CM_PRL_PREF_NO_CHANGE,
                           OEMCM_RoamPrefToDMSS(roam_pref),
                           OEMCM_HybrPrefToDMSS(hybr_pref),
                           OEMCM_SrvDomainPrefToDMSS(srv_domain_pref),
                           (cm_network_sel_mode_pref_e_type) nw_sel_mode_pref,
                           &pMe->m_cm_plmn_id
#ifdef CM_API_WLAN
#ifdef FEATURE_WLAN
#error code not present
#else
                           , NULL
#endif /* FEATURE_WLAN */
#endif /* CM_API_WLAN */
                           ))
#else // ! CM_API_SYS_SEL_PRL_PREF
  if (!cm_ph_cmd_sys_sel_pref(
                           OEMCM_DMSSPhErrCB,
                           (err_cb ? (void *)err_cb->hObject : NULL),
                           gCMID,
                           (cm_mode_pref_e_type) mode_pref,
                           OEMCM_PrefTermToDMSS(pref_term),
                           (dword) pref_duration,
                           OEMCM_AcqOrderPrefToDMSS(acq_order_pref),
                           band_pref_conv,
                           OEMCM_RoamPrefToDMSS(roam_pref),
                           OEMCM_HybrPrefToDMSS(hybr_pref),
                           OEMCM_SrvDomainPrefToDMSS(srv_domain_pref),
                           (cm_network_sel_mode_pref_e_type) nw_sel_mode_pref,
                           &pMe->m_cm_plmn_id
#ifdef CM_API_WLAN
#ifdef FEATURE_WLAN
#error code not present
#else
                           , NULL
#endif /* FEATURE_WLAN */
#endif /* CM_API_WLAN */
                           ))
#endif /* CM_API_SYS_SEL_PRL_PREF */
  {
    if (cb) {
      cb->pfnCancel = NULL;
      cb->pCancelData = NULL;

      OEMCM_FreeErrCBS(pMe, err_cb);
    }
    return EFAILED;
  }

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_SetNAMSel

DESCRIPTION:
  Change the phone's NAM selection.

PROTOTYPE:
  int OEMCM_SetNAMSel(ICM *po,
                       AEECMNAM nam_sel)

PARAMETERS:
  po                [in]: ICM interface pointer.
  nam_sel           [in]: Change NAM selection to this NAM

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If CDMA is not supported
  EFAILED:      Otherwise.

COMMENTS:
  Valid only for CDMA mode

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetNAMSel(ICM *pMe, AEECMNAM nam_sel)
{
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  AEECMPhInfo *         ph_info = NULL;
#endif

  if (!pMe)
    return EBADPARM;

#if (!defined(FEATURE_CDMA_800) && !defined(FEATURE_CDMA_1900))
  PARAM_NOT_REF(nam_sel);

  return EUNSUPPORTED;
#else

  /* Parameter check */
  if (!IN_RANGE(nam_sel, AEECM_NAM_1, AEECM_NAM_AUTO)) {
    MSG_ERROR("OEMCM_SetNAMSel: CM_PH_CMD_ERR_NAM_P", 0, 0, 0);
    return EBADPARM;
  }

  #if (NV_MAX_NAMS == 1)
  if (nam_sel == AEECM_NAM_AUTO) {
    MSG_ERROR("OEMCM_SetNAMSel: CM_PH_CMD_ERR_NAM_P", 0, 0, 0);
    return EBADPARM;
  }
  #endif

  /* State check */
  if (!pMe->m_coreObj) {
    MSG_ERROR("OEMCM_SetNAMSel: Unexpected NULL core pointer", 0, 0, 0);
    return EFAILED;
  }

  if (pMe->m_coreObj->m_ph_valid) {
    ph_info =  &pMe->m_coreObj->m_ph_info;
  } else {
    MSG_ERROR("OEMCM_SetNAMSel: Phone info not available yet", 0, 0, 0);
    return EFAILED;
  }

  if (ph_info->is_in_use) {
    MSG_ERROR("OEMCM_SetNAMSel: CM_PH_CMD_ERR_IN_USE_S", 0, 0, 0);
    return EBADSTATE;
  }

  if (!OEMCM_CheckValidOprtMode(ph_info)) {
    MSG_ERROR("OEMCM_SetNAMSel: CM_PH_CMD_ERR_OPRT_MODE_S", 0, 0, 0);
    return EBADSTATE;
  }

  /* Invoke CM command */
  if (!cm_ph_cmd_nam_sel(OEMCM_DMSSPhErrCB, NULL, gCMID, (cm_nam_e_type)nam_sel)) {
    return EFAILED;
  }

  return AEE_SUCCESS;
#endif
}

/*=============================================================================
FUNCTION: OEMCM_SetOperatingMode

DESCRIPTION:
  Change the phone's operating mode.

PROTOTYPE:
  int OEMCM_SetOperatingMode(ICM *pMe,
                              AEECMOprtMode oprt_mode)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  oprt_mode         [in]: Switch to this operating mode

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetOperatingMode(ICM *pMe, AEECMOprtMode oprt_mode)
{
  if (!pMe || !pMe->m_coreObj)
    return EBADPARM;

  if (!IN_RANGE(oprt_mode, AEECM_OPRT_MODE_PWROFF, AEECM_OPRT_MODE_RESET)) {
    MSG_ERROR("OEMCM_SetOperatingMode: CM_CMD_ERR_BAD_OPRT_MODE_P", 0, 0, 0);
    return EBADPARM;
  }

  /* Check if operating mode transition is valid */
  if (!OEMCM_IsValidOprtModeSwitch(pMe->m_coreObj->m_ph_info.oprt_mode,
                                   oprt_mode)) {
    MSG_HIGH("Not a valid a transition from operating mode %d to %d",
              pMe->m_coreObj->m_ph_info.oprt_mode, oprt_mode, 0);
    return EBADSTATE;
  }

  if (!cm_ph_cmd_oprt_mode(OEMCM_DMSSPhErrCB, NULL, gCMID,
                           (sys_oprt_mode_e_type)oprt_mode)) {
    return EFAILED;
  }

  return AEE_SUCCESS;
}



/*=============================================================================
FUNCTION: OEMCM_PLMNFromIMSI

DESCRIPTION:
  Extracts PLMN from IMSI
PROTOTYPE:
  int OEMCM_GetPLMNFromIMSI(ICM *pMe, AEECMPLMNID *pPLMN, byte *pIMSI)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  plmn             [out]: PLMN Info pointer
  imsi             [in]: IMSI pointer

RETURN VALUE:
  SUCCESS:      If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_GetPLMNFromIMSI(ICM *pMe, AEECMPLMNID *pPLMN, byte *pIMSI)
{
  if (!pMe || !pMe->m_coreObj || !pPLMN || !pIMSI)
    return EFAILED;

  /*
  ** Fill MCC Digit 1 and MCC Digit 2
  */
  pPLMN->identity[0] = (byte)(( ( pIMSI[1] & 0xF0 ) / 0x10 ) +
                             ( ( pIMSI[2] & 0x0F ) * 0x10 ));

  /*
  ** Fill MCC Digit 3 and MNC Digit 3
  */
  pPLMN->identity[1] = (byte)(( ( pIMSI[2] & 0xF0 ) / 0x10 ) +
                             ( ( pIMSI[4] & 0x0F ) * 0x10 ));

  /*
  ** Fill MNC Digit 1 and MNC Digit 2
  */
  pPLMN->identity[2] = pIMSI[3];
  return SUCCESS;
}


/*=============================================================================
FUNCTION: OEMCM_SysMatchPLMN

DESCRIPTION:
  Matches two given PLMNs
PROTOTYPE:
  boolean OEMCM_SysMatchPLMN(ICM *pMe, AEECMPLMNID PLMNa, AEECMPLMNID PLMNb)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  plmna             [in]: PLMN Info pointer for first PLMN
  plmnb             [in]: PLMN Info pointer for second PLMN

RETURN VALUE:
  TRUE:       If procedure was started correctly.
  FALSE:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean OEMCM_SysMatchPLMN(ICM *pMe, AEECMPLMNID PLMNa, AEECMPLMNID PLMNb)
{
  if (!pMe || !pMe->m_coreObj)
    return FALSE;

  return(sys_plmn_match(*((sys_plmn_id_s_type*)&PLMNa), *((sys_plmn_id_s_type*)&PLMNb)));
}




/*=============================================================================
FUNCTION: OEMCM_GetPhoneInfo

DESCRIPTION:
  Get a snapshot of the current phone information

PROTOTYPE:
  int OEMCM_GetPhoneInfo(ICM *po,
                       AEECMPhInfo *info,
                       uint32 size)

PARAMETERS:
  po                [in]: ICM interface pointer.
  info             [out]: Information about the Phone
  size              [in]: Size of Phone info structure (bytes)

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_GetPhoneInfo(ICM *pMe, AEECMPhInfo *info, uint32 size)
{
  if (!pMe || !info)
    return EBADPARM;

  if (!pMe->m_coreObj || !pMe->m_coreObj->m_ph_valid) {
    return EFAILED;
  }

  (void) MEMCPY(info, &pMe->m_coreObj->m_ph_info, size);

  info->aoc_info.ccm = cm_ph_cmd_aoc_get_ccm();
  info->aoc_info.acm = cm_ph_cmd_aoc_get_acm();
  info->aoc_info.acm_max = cm_ph_cmd_aoc_get_acmmax();

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_SetSubscriptionStatusEx

DESCRIPTION:
  Informs the Call Manager that the subscription information is available
  or not available.  This function looks at the current subscription
  status and compares it to the new subscription status requests.

PROTOTYPE:
  int OEMCM_SetSubscriptionStatusEx(ICM *pMe,
                       boolean   cdma_status,
                       boolean   gw_status)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  cdma_status       [in]: What to set CDMA status to.
  gw_status         [in]: What to set GW status to.
  wlan_status       [in]: What to set the WLAN Status to.

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetSubscriptionStatusEx(ICM *pMe, AEECMSUB cdma_status,
                                     AEECMSUB gw_status, AEECMSUB wlan_status)
{
  boolean             ret_val1 = FALSE;
  boolean             ret_val2 = FALSE;
  AEECMPhInfo *       ph_info;
  cm_subscription_status_e_type avail_change_cdma = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
  cm_subscription_status_e_type not_avail_change_cdma = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
  cm_subscription_status_e_type avail_change_gw = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
  cm_subscription_status_e_type not_avail_change_gw = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
#ifdef CM_API_WLAN
  cm_subscription_status_e_type avail_change_wlan = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
  cm_subscription_status_e_type not_avail_change_wlan = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
#endif

  MSG_MED("OEMCM_SetSubscriptionStatusEx: cdma_status=%d, gw_status=%d, wlan_status=%d",
           cdma_status, gw_status, wlan_status);

  if (!pMe)
    return EBADPARM;

  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL)
    return EFAILED;

  if (!OEMCM_CheckValidOprtMode(ph_info)) {
    MSG_HIGH("Phone not in valid operating mode (%d) for SetSubscriptionStatus",
              ph_info->oprt_mode, 0, 0);
    return EBADSTATE;
  }

  /* Based on the desired CDMA subscription status, figure out if
     either CDMA changed for the cm_ph_cmd_subscription_not_available
     or the cm_ph_cmd_subscription_available commands. */
  switch (cdma_status)
  {
    case AEECM_SUB_NO_CHANGE:
      break;
    case AEECM_SUB_AVAILABLE:
      /* We want it available, if its not, then available changes. */
      if (!ph_info->is_cdma_subscription_available)
        avail_change_cdma = CM_SUBSCRIPTION_STATUS_CHANGE;
      break;
    case AEECM_SUB_NOT_AVAILABLE:
      /* We want it un available, if it is available, then not
         available changes. */
      if (ph_info->is_cdma_subscription_available)
        not_avail_change_cdma = CM_SUBSCRIPTION_STATUS_CHANGE;
      break;
  }

  /* Based on the desired GSM/UMTS subscription status, figure out if
     either GSM changed for the cm_ph_cmd_subscription_not_available
     or the cm_ph_cmd_subscription_available commands. */
  switch (gw_status)
  {
    case AEECM_SUB_NO_CHANGE:
      break;
    case AEECM_SUB_AVAILABLE:
      /* We want it available, if its not, then available changes. */
      if (!ph_info->is_gw_subscription_available)
        avail_change_gw = CM_SUBSCRIPTION_STATUS_CHANGE;
      break;
    case AEECM_SUB_NOT_AVAILABLE:
      /* We want it un available, if it is available, then not
         available changes. */
      if (ph_info->is_gw_subscription_available)
        not_avail_change_gw = CM_SUBSCRIPTION_STATUS_CHANGE;
      break;
  }

  /* If the WLAN feature is not available, then we will leave the
     subscription status unchanged regardless of what the user
     requested.
   */
#ifdef FEATURE_WLAN
#error code not present
#endif

  /* First, we will change subscriptions that are available and
     not desired to be not available.  We only call this API if
     some technology has changed.
  */
  if ((not_avail_change_cdma == CM_SUBSCRIPTION_STATUS_CHANGE) ||
      (not_avail_change_gw == CM_SUBSCRIPTION_STATUS_CHANGE)
#ifdef CM_API_WLAN
      || (not_avail_change_wlan == CM_SUBSCRIPTION_STATUS_CHANGE)
#endif
     )
  {
    /* Something has changed, be nice and say which one. */
    if (not_avail_change_cdma == CM_SUBSCRIPTION_STATUS_CHANGE)
      MSG_HIGH("Changing CDMA to not available.", 0, 0, 0);
    if (not_avail_change_gw == CM_SUBSCRIPTION_STATUS_CHANGE)
      MSG_HIGH("Changing GW to not available.", 0, 0, 0);
#ifdef CM_API_WLAN
    if (not_avail_change_wlan == CM_SUBSCRIPTION_STATUS_CHANGE)
      MSG_HIGH("Changing WLAN to not available.", 0, 0, 0);
#endif
    ret_val1 = cm_ph_cmd_subscription_not_available(OEMCM_DMSSPhErrCB, NULL, gCMID,
                                                   not_avail_change_cdma,
                                                   not_avail_change_gw
#ifdef CM_API_WLAN
                                                   , not_avail_change_wlan
#endif
                                                  );
    if (!ret_val1)
    {
      MSG_ERROR("Bad return code cm_ph_cmd_subscription_not_available", 0, 0, 0);
    }
  }
  else
  {
    MSG_HIGH("No subscriptions becoming unavailable", 0, 0, 0);
    ret_val1 = TRUE;
  }

  /* Now we will issue the command to change something that was not available
     and tell it to become available. We only call the CM API if something
     is changing. */
  if ((avail_change_cdma == CM_SUBSCRIPTION_STATUS_CHANGE) ||
      (avail_change_gw == CM_SUBSCRIPTION_STATUS_CHANGE)
#ifdef CM_API_WLAN
      || (avail_change_wlan == CM_SUBSCRIPTION_STATUS_CHANGE)
#endif
     )
  {
    /* Print out which techologies are changing. */
    if (avail_change_cdma == CM_SUBSCRIPTION_STATUS_CHANGE)
      MSG_HIGH("Changing CDMA to available.", 0, 0, 0);
    if (avail_change_gw == CM_SUBSCRIPTION_STATUS_CHANGE)
      MSG_HIGH("Changing GW to available.", 0, 0, 0);
#ifdef CM_API_WLAN
    if (avail_change_wlan == CM_SUBSCRIPTION_STATUS_CHANGE)
      MSG_HIGH("Changing WLAN to available.", 0, 0, 0);
#endif
    ret_val2 = cm_ph_cmd_subscription_available(OEMCM_DMSSPhErrCB, NULL, gCMID,
                                                avail_change_cdma,
                                                avail_change_gw
#ifdef CM_API_WLAN
                                                , avail_change_wlan
#endif
                                               );
    if (!ret_val2)
    {
      MSG_ERROR("Bad return code cm_ph_cmd_subscription_not_available", 0, 0, 0);
    }
  }
  else
  {
    MSG_HIGH("No subscriptions becoming available", 0, 0, 0);
    ret_val2 = TRUE;
  }

  /* If either CM command failed, we will report the failure. */
  if (!ret_val1 || !ret_val2)
  {
    MSG_ERROR("OEMCM_SetSubscriptionStatusEx Returning Failure %d %d",
        ret_val1, ret_val2, 0);
    return EFAILED;
  }

  /* If nothing failed, return success.  Note: we didn't necessary do anything */
  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_SetSubscriptionStatus

DESCRIPTION:
  Informs the Call Manager that the subscription information is available
  or not available.

PROTOTYPE:
  int OEMCM_SetSubscriptionStatus(ICM *pMe,
                       AEECMSysMode     mode,
                       boolean   available)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  available         [in]: TRUE if Available and FALSE if Not Available
  mode              [in]: AEECM_SYS_MODE_CDMA or AEECM_SYS_MODE_GSM or
                          AEECM_SYS_MODE_WCDMA or AEECM_SYS_MODE_WLAN

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetSubscriptionStatus(ICM *pMe, AEECMSysMode mode,
                                    boolean available)
{
  boolean             ret_val = FALSE;
  AEECMPhInfo *       ph_info;

  MSG_MED("OEMCM_SetSubscriptionStatus: mode=%d, available=%d",
          mode, available, 0);

  if (!pMe)
    return EBADPARM;

  if (mode != AEECM_SYS_MODE_CDMA &&
      mode != AEECM_SYS_MODE_GSM &&
      mode != AEECM_SYS_MODE_WCDMA &&
      mode != AEECM_SYS_MODE_WLAN) {
    return EUNSUPPORTED;
  }

  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL)
    return EFAILED;

  if (!OEMCM_CheckValidOprtMode(ph_info)) {
    MSG_HIGH("Phone not in valid operating mode (%d) for SetSubscriptionStatus",
              ph_info->oprt_mode, 0, 0);
    return EBADSTATE;
  }

  if (mode == AEECM_SYS_MODE_CDMA) {
    if (available) {
      if (ph_info->is_cdma_subscription_available)
        return EBADSTATE;
      ret_val = cm_ph_cmd_subscription_available(OEMCM_DMSSPhErrCB, NULL, gCMID,
                                              CM_SUBSCRIPTION_STATUS_CHANGE,
                                              CM_SUBSCRIPTION_STATUS_NO_CHANGE
#ifdef CM_API_WLAN
                                              , CM_SUBSCRIPTION_STATUS_NO_CHANGE
#endif
                                              );
    } else {
      if (!ph_info->is_cdma_subscription_available)
        return EBADSTATE;
      ret_val = cm_ph_cmd_subscription_not_available(OEMCM_DMSSPhErrCB, NULL, gCMID,
                                              CM_SUBSCRIPTION_STATUS_CHANGE,
                                              CM_SUBSCRIPTION_STATUS_NO_CHANGE
#ifdef CM_API_WLAN
                                              , CM_SUBSCRIPTION_STATUS_NO_CHANGE
#endif
                                              );
    }
  } else  if (mode == AEECM_SYS_MODE_GSM || mode == AEECM_SYS_MODE_WCDMA) {
    if (available) {
      if (ph_info->is_gw_subscription_available)
        return EBADSTATE;
      ret_val = cm_ph_cmd_subscription_available(OEMCM_DMSSPhErrCB, NULL, gCMID,
                                              CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                              CM_SUBSCRIPTION_STATUS_CHANGE
#ifdef CM_API_WLAN
                                              ,CM_SUBSCRIPTION_STATUS_NO_CHANGE
#endif
                                              );
    } else {
      if (!ph_info->is_gw_subscription_available)
        return EBADSTATE;
      ret_val = cm_ph_cmd_subscription_not_available(OEMCM_DMSSPhErrCB, NULL, gCMID,
                                              CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                              CM_SUBSCRIPTION_STATUS_CHANGE
#ifdef CM_API_WLAN
                                              ,CM_SUBSCRIPTION_STATUS_NO_CHANGE
#endif
                                              );
    }
  } else { /* mode == AEECM_SYS_MODE_WLAN */
#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */
  }

  if (!ret_val)
    return EFAILED;

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_SetRSSIDeltaThreshold

DESCRIPTION:
  Change RSSI-reporting delta threshold. The default delta threshold is 5 dBm.

PROTOTYPE:
  int OEMCM_SetRSSIDeltaThreshold(ICM *po,
                             uint8 delta)

PARAMETERS:
  po                [in]: ICM interface pointer.
  delta             [in]: Delta threshold in dBm for RSSI reporting

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetRSSIDeltaThreshold(ICM *pMe, uint8 delta)
{
  if (!pMe)
    return EBADPARM;

  if (!cm_ph_cmd_rssi_delta_threshold(OEMCM_DMSSPhErrCB, NULL, gCMID, delta)) {
    return EFAILED;
  }

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_GetNetworks

DESCRIPTION:
  Retrieve a list of preferred or available networks depending on the type
  specified. Only one pending GetNetworks() operation is allowed at a time.

  The reference to application memory should be valid till
  AEECallback is issued. This function cannot be invoked synchronously.

PROTOTYPE:
  int OEMCM_GetNetworks(ICM *po,
                      AEECMNetworkType list_type,
                      AEECMPhError *status,
                      void *list,
                      uint32 list_size,
                      AEECallback *cb)

PARAMETERS:
  po                [in]: ICM interface pointer.
  list_type         [in]: Indicates whether to retrieve the preferred or
                          available networks
  status            [in]: Contains status of the command on callback.
  list              [in]; Address of network list.
  size              [in]: Size of network list structure (bytes).
                          sizeof(AEECMUserPLMNList) or
                          sizeof(AEECMDetailedPLMNList)
  cb                [in]: Callback issued when ICM_GetNetworks is complete

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not GSM/WCDMA
  EFAILED:      Otherwise.

COMMENTS:
  This function is only valid for GSM/WCDMA modes.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_GetNetworks(ICM *pMe,
                             AEECMNetworkType list_type,
                             AEECMPhError *status,
                             void * list,
                             uint32 list_size,
                             AEECallback *cb)
{
  boolean already = FALSE;
  AEECMGetNWCBS *err_cb = NULL;
  AEECMSysMode sys_mode;

  if (!pMe || !pMe->m_coreObj || !status || !list || !cb)
    return EBADPARM;

  if (!IN_RANGE(list_type, AEECM_NETWORK_LIST_AVAILABLE,
                AEECM_NETWORK_LIST_BSS)) {
    return EBADPARM;
  }

  sys_mode = OEMCM_GetSystemMode();
  if ((((list_type == AEECM_NETWORK_LIST_AVAILABLE) ||
        (list_type == AEECM_NETWORK_LIST_USER_PREFERRED)) &&
       !AEECM_IS_MODE_GW(sys_mode))
#ifndef FEATURE_WLAN
       || (list_type == AEECM_NETWORK_LIST_BSS)
#endif
     )
  {
    return EUNSUPPORTED;
  }

  AEECM_MUTEX_LOCK();

  err_cb = &pMe->m_coreObj->m_getnw_cb;
  if (err_cb->bInUse)
    already = TRUE;
  else
    err_cb->bInUse = TRUE;

  AEECM_MUTEX_UNLOCK();

  if (already)
    return EALREADY;

  if (!cb->pfnNotify) {
    MSG_ERROR("Null function pointer provided as callback", 0, 0, 0);
    return EBADPARM;
  }

  err_cb->pcb = cb;
  err_cb->pError = status;
  err_cb->list = list;
  err_cb->list_size = list_size;
  err_cb->pOwner = pMe;
  err_cb->list_type = list_type;

  if (AEEObjectMgr_Register(err_cb, &(err_cb->hObject))) {
    err_cb->bInUse = FALSE;
    err_cb->hObject = NULL;
    return EFAILED;
  }

  CALLBACK_Cancel(cb);

  cb->pfnCancel = OEMCM_AppCancelGetNW;
  cb->pCancelData = err_cb;

  if (!cm_ph_cmd_get_networks(OEMCM_DMSSGetNWErrCB,
                              (void *)err_cb->hObject,
                              gCMID,
                              (list_type == AEECM_NETWORK_LIST_USER_PREFERRED)?
                              CM_NETWORK_LIST_USER_PREFERRED:
                              CM_NETWORK_LIST_AVAILABLE
#ifdef CM_API_WLAN
#ifdef FEATURE_WLAN
#error code not present
#else
                              , CM_MODE_PREF_GSM_WCDMA_ONLY
#endif
#endif
                              )) {
    (void) AEEObjectMgr_Unregister(err_cb->hObject);
    err_cb->hObject = NULL;
    err_cb->bInUse = FALSE;
    err_cb->hObject = NULL;
    return EFAILED;
  }

  return AEE_SUCCESS;

}

/*=============================================================================
FUNCTION: OEMCM_SetPrefNetworks

DESCRIPTION:
  Save the preferred networks to the SIM/USIM

PROTOTYPE:
  int OEMCM_SetPrefNetworks(ICM *pMe,
                          AEECMUserPLMNList *pref_networks_ptr,
                          AEECMPhError *status,
                          AEECallback *status_cb)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  pref_networks_ptr [in]: Preferred networks to be saved
  status_cb         [in]: Status Callback

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not GSM/WCDMA
  EFAILED:      Otherwise.

COMMENTS:
  This function is only valid for GSM/WCDMA modes.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetPrefNetworks(ICM *pMe,
                             AEECMUserPLMNList  *pref_networks_ptr,
                             AEECMPhError *status,
                             AEECallback *cb)
{
  AEECMErrCBS *                    err_cb = NULL;
  sys_user_pref_plmn_list_s_type  pref_networks;
  AEECMSysMode sys_mode;

  if (!pMe)
    return EBADPARM;

  sys_mode = OEMCM_GetSystemMode();
  if (!AEECM_IS_MODE_GW(sys_mode))
    return EUNSUPPORTED;

  if (cb) {

    if (!cb->pfnNotify) {
      MSG_ERROR("Null function pointer provided as callback", 0, 0, 0);
      return EBADPARM;
    }

    if (!status) {
      MSG_ERROR("Null status pointer", 0, 0, 0);
      return EBADPARM;
    }

    err_cb = OEMCM_AllocErrCBS(pMe);
    if (!err_cb) {
      MSG_ERROR("Unable to get ICM Error Callback", 0, 0, 0);
      return EFAILED;
    }

    err_cb->pcb = cb;
    err_cb->pError = status;

    CALLBACK_Cancel(cb);

    cb->pfnCancel = OEMCM_AppCancelErrCB;
    cb->pCancelData = err_cb;
  }

  OEMCM_CopyOutUserPrefNetworks(&pref_networks, pref_networks_ptr);

  if(!cm_ph_cmd_set_preferred_networks(OEMCM_DMSSPhErrCB,
                                       (err_cb ?
                                        (void *) err_cb->hObject :
                                        NULL),
                                       gCMID,
                                       &pref_networks)) {
    if (cb) {
      cb->pfnCancel = NULL;
      cb->pCancelData = NULL;

      OEMCM_FreeErrCBS(pMe, err_cb);
    }
    return EFAILED;
  }

  return AEE_SUCCESS;

}

/*=============================================================================
FUNCTION: OEMCM_WakeupFromStandby

DESCRIPTION:
  Informs the lower layers to wake up from deep sleep (standby mode).

PROTOTYPE:
  int OEMCM_WakeupFromStandby(ICM *pMe)

PARAMETERS:
  pMe                [in]: ICM interface pointer.

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  This function is only valid for CDMA mode

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_WakeupFromStandby(ICM *pMe)
{
  if (!pMe)
    return EBADPARM;

  if (!cm_ph_cmd_wakeup_from_standby(OEMCM_DMSSPhErrCB, NULL, gCMID)) {
    return EFAILED;
  }

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_SetRTREConfig

DESCRIPTION:
  Changes the RTRE configuration to the client specified value.

PROTOTYPE:
  int OEMCM_SetRTREConfig(ICM *pMe,
                           AEECMRTREConfig rtre_config)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  rtre_config       [in]: The desired RTRE configuration

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  This function is only valid for CDMA mode

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetRTREConfig(ICM *pMe, AEECMRTREConfig rtre_config)
{
  int ret;
  AEECMPhInfo * ph_info;

  if (!pMe)
    return EBADPARM;


  if (!IN_RANGE(rtre_config, AEECM_RTRE_CONFIG_RUIM_ONLY,
                AEECM_RTRE_CONFIG_RUIM_OR_DROP_BACK)) {
    MSG_ERROR("OEMCM_SetRTREConfig: CM_PH_CMD_ERR_RTRE_CONFIG_P", 0, 0, 0);
    return EBADPARM;
  }

  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL) {
    MSG_ERROR("OEMCM_SetRTREConfig: Phone info not available", 0, 0, 0);
    return EFAILED;
  }

#if defined(FEATURE_RUIM)
  if (ph_info->curr_nam != NV_RUIM_NAM) {
    MSG_ERROR("OEMCM_SetRTREConfig: CM_PH_CMD_ERR_CURRENT_NAM_IS_NOT_RUIM_NAM_S"
               , 0, 0, 0);
    return EBADSTATE;
  }

#endif

  /* Set subscription status to false before RTRE change */
  if (ph_info->is_cdma_subscription_available) {
    if ((ret = OEMCM_SetSubscriptionStatus(pMe, AEECM_SYS_MODE_CDMA, FALSE))
         != AEE_SUCCESS) {
      MSG_ERROR("Unable to set CDMA subscription to unavailable %d", ret, 0, 0);
      return ret;
    }
  }

  if (ph_info->is_gw_subscription_available) {
    if ((ret = OEMCM_SetSubscriptionStatus(pMe, AEECM_SYS_MODE_GSM, FALSE))
         != AEE_SUCCESS) {
      MSG_ERROR("Unable to set GW subscription to unavailable %d", ret, 0, 0);
      return ret;
    }
  }

  if (!cm_ph_cmd_change_rtre_config(OEMCM_DMSSPhErrCB, NULL, gCMID,
                                    (cm_rtre_config_e_type)rtre_config)) {
    return EFAILED;
  }

  return AEE_SUCCESS;

}

/*=============================================================================
FUNCTION: ICM_AvoidServingSystem

DESCRIPTION:
  Avoid the serving system specified for a specified time.

PROTOTYPE:
  int ICM_AvoidServingSystem(ICM *pMe,
                             AEECMAvoidSSPref avoid_type,
                             uint32 avoid_time)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  avoid_type        [in]: Type of system to avoid
  avoid_time        [in]: Avoid time in seconds

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  Valid only for CDMA mode

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetAnswerVoicePref(ICM *pMe, AEECMAnsVoicePref answer_voice,
                              uint32  duration)
{
  AEECMSysMode sys_mode;

  if (!pMe)
    return EBADPARM;

  sys_mode = OEMCM_GetSystemMode();
  if (!AEECM_IS_MODE_CDMA(sys_mode))
    return EUNSUPPORTED;

  if (!IN_RANGE(answer_voice, AEECM_ANSWER_VOICE_AS_VOICE,
                AEECM_ANSWER_VOICE_AS_MODEM_ALWAYS)) {
    MSG_ERROR("OEMCM_SetAnswerVoicePref: CM_PH_CMD_ERR_ANSWER_VOICE_P",
               0, 0, 0);
    return EBADPARM;
  }

  if ((answer_voice == AEECM_ANSWER_VOICE_AS_FAX_ONCE ||
       answer_voice == AEECM_ANSWER_VOICE_AS_MODEM_ONCE) &&
       duration == 0) {
    MSG_ERROR("OEMCM_SetAnswerVoicePref: CM_PH_CMD_ERR_ANSWER_DURATION_P",
               0, 0, 0);
    return EBADPARM;
  }

  if (!cm_ph_cmd_answer_voice(OEMCM_DMSSPhErrCB, NULL, gCMID,
                              (cm_answer_voice_e_type)answer_voice,
                              (dword)duration)) {
    return EFAILED;
  }

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_AvoidServingSystem

DESCRIPTION:
  Avoid the serving system specified for a specified time.

PROTOTYPE:
  int OEMCM_AvoidServingSystem(ICM *pMe,
                             AEECMAvoidSSPref avoid_type,
                             uint32 avoid_time)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  avoid_type        [in]: Type of system to avoid
  avoid_time        [in]: Avoid time in seconds

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  Valid only for CDMA mode

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_AvoidServingSystem(ICM *pMe, AEECMAvoidSSPref avoid_type,
                              uint32 avoid_time)
{
  AEECMPhInfo * ph_info;

  if (!pMe)
    return EBADPARM;

  if (!IN_RANGE(avoid_type, AEECM_SS_AVOID_SYS_SRV_IF_UZ,
                AEECM_SS_AVOID_SYS_SRV)) {
    MSG_ERROR("OEMCM_AvoidServingSystem: CM_CMD_ERR_BAD_AVOID_SYS_P", 0, 0, 0);
    return EBADPARM;
  }

  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL) {
    MSG_ERROR("OEMCM_AvoidServingSystem: Phone info not available yet", 0, 0, 0);
    return EFAILED;
  }

  if (ph_info->is_in_use) {
    MSG_ERROR("OEMCM_AvoidServingSystem: CM_PH_CMD_ERR_IN_USE_S", 0, 0, 0);
    return EBADSTATE;
  }

  if (ph_info->oprt_mode != AEECM_OPRT_MODE_ONLINE) {
    MSG_ERROR("OEMCM_AvoidServingSystem: CM_PH_CMD_ERR_ONLINE_S", 0, 0, 0);
    return EBADSTATE;
  }

  /* TODO: how do we check uz_id ? */
  if (!cm_ph_cmd_avoid_sys(OEMCM_DMSSPhErrCB, NULL, gCMID,
                           (cm_ss_avoid_sys_e_type)avoid_type,
                           (dword)avoid_time)) {
    return EFAILED;
  }

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_ResetACM

DESCRIPTION:
  Reset the Accumulate Call Meter to 0 in a system supporting Advice of Charge.

PROTOTYPE:
  int OEMCM_ResetACM(ICM *po)

PARAMETERS:
  po                [in]: ICM interface pointer.

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not GSM/WCDMA
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_ResetACM(ICM *pMe)
{
  if (!pMe)
    return EBADPARM;

  if (!cm_ph_cmd_aoc_reset_acm(OEMCM_DMSSPhErrCB, NULL, gCMID))
    return EFAILED;

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_SetACMMax

DESCRIPTION:
  Set the Maximum value for Accumulate Call Meter.

PROTOTYPE:
  int OEMCM_SetACMMax(ICM *po, uint32 new_max)

PARAMETERS:
  po                [in]: ICM interface pointer.
  new_max           [in]: New value for ACMMAX

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not GSM/WCDMA
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetACMMax(ICM *pMe, uint32 acm_max)
{
  if (!pMe)
    return EBADPARM;

  if (!cm_ph_cmd_aoc_set_acmmax(OEMCM_DMSSPhErrCB, NULL, gCMID, acm_max))
    return EFAILED;

  return AEE_SUCCESS;

}

/*=============================================================================
FUNCTION: OEMCM_SetDDTMPref

DESCRIPTION:
  Change the phone's data dedicated transmission mode (DDTM)
  preference to a specified selection, such as ON or OFF etc.

PROTOTYPE:
  int OEMCM_SetDDTMPref(ICM *po, AEECMDDTMPref ddtm_pref)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  ddtm_pref         [in]: Dedicated data transmission mode

RETURN VALUE:
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If DDTM is not supported
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetDDTMPref(ICM *pMe, AEECMDDTMPref ddtm_pref)
{
#ifndef FEATURE_DDTM_CNTL
  PARAM_NOT_REF(pMe);
  PARAM_NOT_REF(ddtm_pref);
  return EUNSUPPORTED;
#else
  sys_srv_opt_type so_list[] = { (sys_srv_opt_type) CAI_SO_NULL };

  if (!pMe)
    return EBADPARM;

  if (!IN_RANGE(ddtm_pref, AEECM_DDTM_PREF_OFF, AEECM_DDTM_PREF_NO_CHANGE))
    return EBADPARM;


  if (!cm_ph_cmd_ddtm_pref(OEMCM_DMSSPhErrCB,
                           NULL,
                           gCMID,
                           (cm_ddtm_pref_e_type)ddtm_pref,
                           SYS_DDTM_ACT_MASK_ALL,
                           CM_DDTM_SO_LIST_ACT_ADD,
                           ARR_SIZE( so_list),
                           so_list)) {
    MSG_ERROR("Failed to set DDTM pref to %d", ddtm_pref, 0, 0);
    return EFAILED;
  }

  return AEE_SUCCESS;
#endif
}

/*---------------------------------------------------------------------------
                       SERVING SYSTEM FUNCTIONS
---------------------------------------------------------------------------*/

/*=============================================================================
FUNCTION: OEMCM_GetSSInfo

DESCRIPTION:
  Get the current serving system information and registration status.

PROTOTYPE:
  int OEMCM_GetSSInfo(ICM *po,
                    AEECMSSInfo *ss_info,
                    uint32 size)

PARAMETERS:
  po                [in]: ICM interface pointer.
  ss_info          [out]: Return Serving System Information
  size              [in]: Size of Serving System Info structure (bytes)

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_GetSSInfo(ICM *pMe, AEECMSSInfo *ss_info, uint32 size)
{
  if (!pMe || !ss_info)
    return EBADPARM;

  if (!pMe->m_coreObj) {
    MSG_ERROR("OEMCM_GetSSInfo: Null core object", 0, 0, 0);
    return EFAILED;
  }

  if (!pMe->m_coreObj || !pMe->m_coreObj->m_ss_valid) {
    return EFAILED;
  }

  pMe->m_coreObj->m_ss_info.sys_mode = OEMCM_GetSystemMode();

  (void) MEMCPY(ss_info, &pMe->m_coreObj->m_ss_info, size);

  return AEE_SUCCESS;
}


/*---------------------------------------------------------------------------
                           INBAND FUNCTIONS
---------------------------------------------------------------------------*/

/*=============================================================================
FUNCTION: OEMCM_BurstDTMF

DESCRIPTION:
  Send a Burst DTMF

PROTOTYPE:
  int OEMCM_BurstDTMF(ICM *po,
                    AEECMCallID callID,
                    AEECMDTMFOnLength on_length,
                    AEECMDTMFOffLength off_length,
                    AECHAR *digits)

PARAMETERS:
  po                [in]: ICM interface pointer.
  callID            [in]: Call to send DTMFs
  on_length         [in]: DTMF pulse width
  off_length        [in]: DTMF inter-digit interval
  digits            [in]: Null terminated string of DTMF digits

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  Valid only for CDMA mode.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_BurstDTMF(ICM *pMe, AEECMCallID callID,
                           AEECMDTMFOnLength on_length,
                           AEECMDTMFOffLength off_length, AECHAR *digits)
{
  char    dtmf_digits[CM_INBAND_MAX_FWD_DIALED_DIGITS+1];

  if (!pMe || !pMe->m_coreObj)
    return EBADPARM;

  if (!OEMCM_IsValidAppCallID(callID))
    return EBADPARM;

  if (!digits)
    return EBADPARM;

  if (!OEMCM_CheckDTMFOnLength(on_length))
    return EBADPARM;

  if (!OEMCM_CheckDTMFOffLength(off_length))
    return EBADPARM;

  (void ) WSTRTOSTR(digits, dtmf_digits, sizeof(dtmf_digits));

  if (!OEMCM_CheckDTMFDigits((uint8)STRLEN(dtmf_digits), (uint8 *)dtmf_digits))
    return EBADPARM;

  return OEMCM_BurstDTMF_Int( pMe, callID, on_length, off_length, dtmf_digits );
}

/*=============================================================================
FUNCTION: OEMCM_BurstDTMF_Int

DESCRIPTION:
  Send a Burst DTMF

PROTOTYPE:
  int OEMCM_BurstDTMF_Int(ICM *pMe,
                          AEECMCallID callID,
                          AEECMDTMFOnLength on_length,
                          AEECMDTMFOffLength off_length,
                          const char * dtmf_digits)

PARAMETERS:
  po                [in]: ICM interface pointer.
  callID            [in]: Call to send DTMFs
  on_length         [in]: DTMF pulse width
  off_length        [in]: DTMF inter-digit interval
  dtmf_digits       [in]: Null terminated string of DTMF digits

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EUNSUPPORTED: If mode is not CDMA
  EFAILED:      Otherwise.

COMMENTS:
  Valid only for CDMA mode.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_BurstDTMF_Int(ICM *pMe, AEECMCallID callID,
                               AEECMDTMFOnLength on_length,
                               AEECMDTMFOffLength off_length, const char * dtmf_digits)
{
  AEECMCallModeInfo call_mode_info;
  AEECMBurstDTMFPkt *dtmf_pkt;
  int ret;

  call_mode_info = pMe->m_coreObj->m_call_info[OEMCM_CMCALLID(callID)].call_mode_info;

  switch (call_mode_info) {

    case AEECM_CALL_MODE_INFO_CDMA:
      /* Normal burst DTMF */
      if (!cm_mm_inband_cmd_burst_dtmf(OEMCM_DMSSInbandErrCB, NULL, gCMID,
                                       OEMCM_CMCALLID(callID),
                                      (uint8) on_length, (uint8) off_length,
                                      (uint8) STRLEN(dtmf_digits),
                                      (uint8 *)dtmf_digits)) {
        return EFAILED;
      }
      break;

    case AEECM_CALL_MODE_INFO_GW_CS:
    case AEECM_CALL_MODE_INFO_IP:
    case AEECM_CALL_MODE_INFO_GW_PS:

      /* Translate burst DTMF to start/stop */

      if((ret = OEMCM_AllocBurstDTMF(pMe, callID, dtmf_digits,
                                     on_length, off_length,
                                     &dtmf_pkt)) != SUCCESS) {
        return ret;
      }

      if (ISHELL_SetTimer(pMe->m_pIShell, 0, OEMCM_HandleBurstDTMF, dtmf_pkt) !=
                                                          AEE_SUCCESS) {
        OEMCM_FreeBurstDTMF(dtmf_pkt);
        MSG_ERROR("Failed to start timer for Burst DTMF", 0, 0, 0);
      }
      break;

    default:
      return EUNSUPPORTED;
  }

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_StartContDTMF

DESCRIPTION:
  Start a continous DTMF tone

PROTOTYPE:
  int OEMCM_StartContDTMF(ICM *po,
                        AEECMCallID callID,
                        AECHAR digit)

PARAMETERS:
  po                [in]: ICM interface pointer.
  callID            [in]: Call ID
  digit             [in]: DTMF digit

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_StartContDTMF(ICM *pMe, AEECMCallID callID, AECHAR digit)
{
  return OEMCM_StartContDTMF_Int( pMe, callID, (char)digit );
}

/*=============================================================================
FUNCTION: OEMCM_StartContDTMF_Int

DESCRIPTION:
  Start a continous DTMF tone

PROTOTYPE:
  int OEMCM_StartContDTMF_Int(ICM *po,
                              AEECMCallID callID,
                              char dtmf_digit)

PARAMETERS:
  po                [in]: ICM interface pointer.
  callID            [in]: Call ID
  dtmf_digit        [in]: DTMF digit

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_StartContDTMF_Int(ICM *pMe, AEECMCallID callID, char dtmf_digit)
{


  if (!pMe || !pMe->m_coreObj)
    return EBADPARM;

  if (!OEMCM_IsValidAppCallID(callID))
    return EBADPARM;

  if (!OEMCM_CheckDTMFDigits(1, (uint8 *)&dtmf_digit))
    return EBADPARM;

  if (pMe->m_coreObj->m_dtmf_start_times[OEMCM_CMCALLID(callID)] != 0)
    return EBADSTATE;

  if (!cm_mm_inband_cmd_start_cont_dtmf(OEMCM_DMSSInbandErrCB, NULL, gCMID,
                                        OEMCM_CMCALLID(callID),
                                        (uint8)dtmf_digit)) {
    return EFAILED;
  }

  pMe->m_coreObj->m_dtmf_start_times[OEMCM_CMCALLID(callID)] = GETUPTIMEMS();

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_StopContDTMF

DESCRIPTION:
  Stop continous DTMF tone

PROTOTYPE:
  int OEMCM_StopContDTMF(ICM *po,
                       AEECMCallID callID)

PARAMETERS:
  po                [in]: ICM interface pointer.
  callID            [in]: Call ID

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_StopContDTMF(ICM *pMe, AEECMCallID callID)
{
  dword duration;

  if (!pMe || !pMe->m_coreObj)
    return EBADPARM;

  if (!OEMCM_IsValidAppCallID(callID))
    return EBADPARM;

  duration = (GETUPTIMEMS() -
              pMe->m_coreObj->m_dtmf_start_times[OEMCM_CMCALLID(callID)]);
  if (duration > AEECM_MAX_DTMF_RPT_TIME)
    duration = AEECM_MAX_DTMF_RPT_TIME;

  if (!cm_mm_inband_cmd_stop_cont_dtmf(OEMCM_DMSSInbandErrCB, NULL, gCMID,
                                       OEMCM_CMCALLID(callID),
                                       (word)duration)) {
    return EFAILED;
  }

  pMe->m_coreObj->m_dtmf_start_times[OEMCM_CMCALLID(callID)] = 0;

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_IsValidModeBandPref

DESCRIPTION:
  If only mode_pref is specified (band_pref is AEECM_BAND_PREF_NONE),
  checks whether the given mode preference is valid with the current band
  preference.

  If both mode_pref and band_pref is specified, checks whether the combination
  is supported by the current target.

PROTOTYPE:
  boolean OEMCM_IsValidModeBandPref(ICM *po,
                                     AEECMModePref mode_pref,
                                     AEECMBandPref band_pref)

PARAMETERS:
  po                [in]: ICM interface pointer.
  mode_pref         [in]: mode preference
  band_pref         [in]: band preference

RETURN VALUE:
  TRUE:      If valid mode and band preference
  FALSE:     Otherwise

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean OEMCM_IsValidModeBandPref(ICM *pMe, AEECMModePref mode_pref,
                                         AEECMBandPref band_pref)
{
  cm_band_pref_e_type band_pref_conv;

  if (!pMe || !pMe->m_coreObj)
    return FALSE;

  if (mode_pref == AEECM_MODE_PREF_NONE &&
      band_pref == AEECM_BAND_PREF_NONE)
    return FALSE;

  band_pref_conv = OEMCM_BandPrefToDMSS(band_pref);

  if (mode_pref == AEECM_MODE_PREF_NONE)
    return cm_is_valid_mode_band_pref((cm_mode_pref_e_type)
                                       pMe->m_coreObj->m_ph_info.mode_pref,
                                       band_pref_conv);

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  if (band_pref == AEECM_BAND_PREF_NONE)
    return cm_is_valid_mode_pref((cm_mode_pref_e_type)mode_pref);
#endif

  return cm_is_valid_mode_band_pref((cm_mode_pref_e_type)mode_pref,
                                    band_pref_conv);
}

/*=============================================================================
FUNCTION: OEMCM_GetSysMode

DESCRIPTION:
  Gets the current system mode or the mode that is being acquired.

PROTOTYPE:
  AEECMSysMode OEMCM_GetSysMode(ICM *po)

PARAMETERS:
  po                [in]: ICM interface pointer.

RETURN VALUE:
  AEECMSysMode

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static AEECMSysMode OEMCM_GetSysMode(ICM *pMe)
{
  if (!pMe)
    return AEECM_SYS_MODE_NONE;

  return OEMCM_GetSystemMode();
}

/*=============================================================================
FUNCTION: OEMCM_IsMyCall

DESCRIPTION:
  Checks if the call was originated or answered by this application/module
  (using this ICM interface pointer).

PROTOTYPE:
  boolean OEMCM_IsMyCall(ICM *po, AEECMCallID callID)

PARAMETERS:
  po                [in]: ICM interface pointer.

RETURN VALUE:
  TRUE if call was originated/answered using this interface.
  FALSE otherwise

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean OEMCM_IsMyCall(ICM *pMe, AEECMCallID callID)
{
  if (!pMe || !pMe->m_coreObj)
    return FALSE;

  if (!OEMCM_IsValidAppCallID(callID))
    return FALSE;

  return (pMe->m_coreObj->m_call_owners[OEMCM_CMCALLID(callID)] == pMe) ?
                                                              TRUE : FALSE;
}


/*=============================================================================
FUNCTION: OEMCM_SelectLine

DESCRIPTION:
  Allows selection of a line

PROTOTYPE:
  int OEMCM_SelectLine(ICM *po,
                       AEECMAlsLine selectLine)

PARAMETERS:
  po                [in]: ICM interface pointer.
  selectLine        [in]: Line Number

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  This function is meaningful only for GSM/WCDMA modes and if FEATURE_ALS is defined

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SelectLine(ICM *pMe, AEECMAlsLine selectLine,
                                      AEECMPhError *status,AEECallback *cb)
{

#ifdef FEATURE_ALS
#error code not present
#else
    MSG_ERROR("ALS Not Supported", 0, 0, 0);
    return EUNSUPPORTED;
#endif
}

/*=============================================================================
FUNCTION: OEMCM_SetLineSwitching

DESCRIPTION:
  Enables/Disables switching of lines

PROTOTYPE:
  int OEMCM_SetLineSwitching(ICM *po,
                             AEECMAlsLineSwitching lineSwitching)

PARAMETERS:
  po                [in]: ICM interface pointer.
  lineSwitching     [in]: Enable/Disable line Switching

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  This function is meaningful only for GSM/WCDMA modes and if FEATURE_ALS is defined

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_SetLineSwitching(ICM *pMe, AEECMAlsLineSwitching lineSwitching, AEECMPhError *status,AEECallback *cb)
{
#ifdef FEATURE_ALS
#error code not present
#else
    MSG_ERROR("ALS Not Supported", 0, 0, 0);
    return EUNSUPPORTED;
#endif
}

/*=============================================================================
FUNCTION: OEMCM_Recall

DESCRIPTION:
  Responds to the Recall Indication

PROTOTYPE:
  int OEMCM_Recall(ICM *po,
                 AEECMRecallRspType recallRsp,
                 AEECMCallID callID)

PARAMETERS:
  po                [in]: ICM interface pointer.
  recallRsp         [in]: Response to the recall indication
  callID            [in]: CCBS Index=> call ID

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  This function is meaningful only for GSM/WCDMA modes

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_RecallRsp(ICM *pMe, AEECMRecallRspType recallRsp, AEECMCallID callID)
{
#ifdef FEATURE_CCBS
#error code not present
#else
    MSG_ERROR("ALS Not Supported", 0, 0, 0);
    return EUNSUPPORTED;
#endif
}

/*=============================================================================
FUNCTION: OEMCM_ResetOrigThrottle

DESCRIPTION:
  Calls the CM Throttle API.  Used for testing.

PROTOTYPE:
  int ResetOrigThrottle(ICM *po)

PARAMETERS:
  po                [in]: ICM interface pointer.
  recallRsp         [in]: Response to the recall indication
  callID            [in]: CCBS Index=> call ID

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  Debug function.  Not expected to be used outside of test Apps.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_ResetOrigThrottle(ICM *pMe)
{
#if defined(CM_API_ORIG_THR) && defined(FEATURE_CALL_THROTTLE)
  if (cm_ss_cmd_orig_thr_update_tbl(
                          NULL,         /* Pointer to the call back function */
                          NULL,         /* Pointer to user data */
                          gCMID,        /* requesting client ID */
                          CM_SS_ORIG_THR_RESET_TBL, /* The action to be taken */
                          CM_SS_ORIG_THR_DS_MIP_UP_SUCCESS, /* The call end reason */
                          CM_CALL_TYPE_VOICE, /* Type of the call, e.g PS_DATA */
                          SYS_SYS_MODE_NO_SRV, /* The mode on which the call was connected */
                          NULL, /* The sys id of the system, SID,NID */
                          SYS_BAND_BC0  /* cellular or PCS */
#ifdef CM_API_ORIG_THR_PKT_ZONE
                          , 0             /* The packet zone ID. */
#endif
                          ))
  {
    return AEE_SUCCESS;
  }
  else
  {
    return EFAILED;
  }
#else
  return EUNSUPPORTED;
#endif
}

/*===========================================================================

                   ICallOpts FUNCTION DEFINITIONS

===========================================================================*/
/*=============================================================================
FUNCTION: OEMCallOpts_New

DESCRIPTION:
  Allocates a new ICallOpts object for every ISHELL_CreateInstance().
  An IWebOpts is also created for each ICallOpts object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMCallOpts_New (IShell *pIShell, AEECLSID cls, void **ppif)
{
   ICallOpts *pNew;
   IWebOpts  *pWebOpts;

#ifdef AEE_SIMULATOR
   /* Do not create an object is this is being run on the SDK. */
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   // Check parameters.
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }

   if (cls != AEECLSID_CALLOPTS) {
      return EUNSUPPORTED;
   }

   // Allocate the object.
   pNew = (ICallOpts *) AEE_NewClass((IBaseVtbl*)&gOEMCallOptsFuncs,
                               sizeof(ICallOpts));
   if (pNew == NULL)
      return ENOMEMORY;


   /* Initialize the values. */
   pNew->m_pIShell = pIShell;
   pNew->m_cls = cls;
   pNew->m_uRefs = 1;

   (void) ISHELL_CreateInstance(pIShell, AEECLSID_WEBOPTS, (void **) &pWebOpts);
   if (pWebOpts == NULL) {
      FREE(pNew);
      return ENOMEMORY;
   }

   pNew->m_pIWebOpts = pWebOpts;
   pNew->m_str = NULL;

   *ppif = (void **)pNew;

   return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCallOpts_AddRef

DESCRIPTION:
  Indicates that one or more object is now pointing to object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMCallOpts_AddRef(ICallOpts *pMe)
{
  if (pMe) {
    return (++pMe->m_uRefs);
  }
  return 0;
}

/*=============================================================================
FUNCTION: OEMCallOpts_Release

DESCRIPTION:
  Decreases the number of references to the ICallOpts object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMCallOpts_Release(ICallOpts *pMe)
{
   if (!pMe) {
      return (0);
   }

   if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
   }

   if (pMe->m_str) {
      OEMCM_FREE(pMe->m_str);
      pMe->m_str = NULL;
   }

   (void) IWEBOPTS_Release(pMe->m_pIWebOpts);

   FREE(pMe);

   return (0);
}

/*===========================================================================

FUNCTION: OEMCallOpts_QueryInterface

DESCRIPTION
  Allows applications to query if ICallOpts supports other interfaces and then
  returns a pointer to that new interface supported via ICallOpts.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMCallOpts_QueryInterface(ICallOpts *pMe, AEECLSID id, void **ppNew)
{
   int ret_val = AEE_SUCCESS;

   if ((!pMe) || (!ppNew)) {
      return EBADPARM;
   }

   switch (id) {
      case AEECLSID_CALLOPTS:
         if (OEMCallOpts_AddRef(pMe)) {
            *ppNew = pMe;
         } else {
            *ppNew = NULL;
            ret_val = ECLASSNOTSUPPORT;
         }
         break;

      default:
         *ppNew = NULL;
         ret_val = ECLASSNOTSUPPORT;
         break;
   }


   return ret_val;
}

/*=============================================================================
FUNCTION: OEMCallOpts_AddOpt

DESCRIPTION:
  Adds an ID-Value pair to ICallOpts. Only one value is allowed for each
  ID unlike IWebOpts which maintains a stack of values for each ID.

PROTOTYPE:
  int OEMCallOpts_AddOpt(ICallOpts *pMe, CallOpt *pOpt)

PARAMETERS:
  pMe                [in]: ICallOpts interface pointer.
  pOpt               [in]: Call option

RETURN VALUE:
  SUCCESS:   If addition was successful.
  ENOMEMORY: If enough memory couldn't be allocated
  EBADPARM:  If any of the input parameters is NULL.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_AddOpt()

=============================================================================*/
static int OEMCallOpts_AddOpt(ICallOpts *pMe, CallOpt *pOpt)
{
  WebOpt *pWebOpt;
  WebOpt tmp[2] = {0, NULL, WEBOPT_END, NULL};
  char *szTmp = NULL;
  uint32 len;
  int ret;

  if (!pMe || !pMe->m_pIWebOpts || !pOpt)
    return EBADPARM;

  if (IWEBOPTS_RemoveOpt(pMe->m_pIWebOpts, pOpt->nId, 0) == SUCCESS) {
    MSG_LOW("Option already found in IWebOpts", 0, 0, 0);
  }

  /* If option is an AECHAR * string, translate it to char * */
  if (pOpt->nId == AEECM_CALLOPT_CALLED_SUBADDR) {

    len = (uint32) WSTRLEN(pOpt->pVal);
    if (!len)
      return EBADPARM;

    szTmp = (char *) OEMCM_MALLOC((len+1)*sizeof(char));
    if (!szTmp)
      return ENOMEMORY;

    (void) WSTRTOSTR((const AECHAR *)pOpt->pVal, szTmp, (int)(len+1));

    tmp[0].nId = pOpt->nId;
    tmp[0].pVal = szTmp;

  } else {
    tmp[0] = pOpt[0];
  }

  pWebOpt = tmp;

  if ((ret = IWEBOPTS_AddOpt(pMe->m_pIWebOpts, pWebOpt)) != SUCCESS) {
    MSG_ERROR("Unable to add call option to IWebOpts: %d", ret, 0, 0);
  }

  OEMCM_FREE(szTmp);

  return ret;
}

/*=============================================================================
FUNCTION: OEMCallOpts_RemoveOpt

DESCRIPTION:
  Remove the ID-value pair matching the specified ID.

PROTOTYPE:
  boolean OEMCallOpts_RemoveOpt(ICallOpts *pMe, int nOptId)

PARAMETERS:
  pMe                [in]: ICallOpts interface pointer.
  nOptId            [in]: Option ID.

RETURN VALUE:
  SUCCESS: If remove was successful
  EFAILED: If ID was not found

COMMENTS:
  Corresponds to IWEBOPTS_RemoveOpt() with index 0.

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_RemoveOpt()

=============================================================================*/
static int OEMCallOpts_RemoveOpt(ICallOpts *pMe, int32 nOptId)
{
  int ret;

  if (!pMe || !pMe->m_pIWebOpts)
    return EBADPARM;

  if ((ret = IWEBOPTS_RemoveOpt(pMe->m_pIWebOpts, nOptId, 0)) != SUCCESS) {
    MSG_ERROR("Unable to remove call option %d from IWebOpts: %d", ret, nOptId,
               0);
  }

  if (nOptId == AEECM_CALLOPT_CALLED_SUBADDR && pMe->m_str) {
    OEMCM_FREE(pMe->m_str);
    pMe->m_str = NULL;
  }

  return ret;
}

/*=============================================================================
FUNCTION: OEMCallOpts_GetOpt

DESCRIPTION:
  Find the ID-value pair matching the specified ID.

PROTOTYPE:
  int OEMCallOpts_GetOpt(ICalOpts *pMe, int32 nOptId, CallOpt *pOpt)

PARAMETERS:
  pMe                [in]: ICallOpts interface pointer.
  nOptId            [in]: Option ID to search for.
  pOpt          [in/out]: Reference to CallOpt where matching call option
                          is copied.

RETURN VALUE:
  SUCCESS: If a matching call option is found.
  EFAILED: Otherwise

COMMENTS:
  Only the CallOpt structure is copied and not the data pointed by pVal field.

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_GetOpt()

=============================================================================*/
static int OEMCallOpts_GetOpt(ICallOpts *pMe, int32 nOptId, CallOpt *pOpt)
{
  WebOpt tmp;
  int ret;
  int len;

  if (!pMe || !pMe->m_pIWebOpts || !pOpt)
    return EBADPARM;

  if ((ret = IWEBOPTS_GetOpt(pMe->m_pIWebOpts, nOptId, 0, &tmp))
                                                       != SUCCESS) {
    MSG_LOW("Unable to obtain option for id %d from IWebOpts: %d",
               nOptId, ret, 0);
    return ret;
  }

  /* Translate char to AECHAR */
  if (tmp.nId == AEECM_CALLOPT_CALLED_SUBADDR) {
    len = (int) STRLEN(tmp.pVal);

    if (pMe->m_str) {
      OEMCM_FREE(pMe->m_str);
      pMe->m_str = NULL;
    }

    pMe->m_str = (AECHAR *) OEMCM_MALLOC((uint32)(len + 1)*sizeof(AECHAR));
    if (!pMe->m_str)
      return ENOMEMORY;

    (void) STRTOWSTR((const char *)tmp.pVal, pMe->m_str,
                      (int)(((uint32)(len + 1))*sizeof(AECHAR)));

    tmp.pVal = pMe->m_str;
  }

  *pOpt = tmp;

  return AEE_SUCCESS;

}

/*===========================================================================

                    Callbacks Registered to DMSS Call Manager

===========================================================================*/

/*=============================================================================
FUNCTION: OEMCM_DMSSCallEventCB

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSCallEventCB(cm_call_event_e_type call_event,
                             const cm_mm_call_info_s_type *call_info_ptr)
{
  AEECMEventCBS *event_cb;
  AEECMEvent evt;
  AEECMCallInfo *core_call_info;

  MSG_FATAL("Received Call Event 0x%X from DMSS CM", call_event, 0, 0);

  if (!ICMCoreObj || !call_info_ptr) {
    MSG_ERROR("OEMCM_DMSSCallEventCB: Unexpected NULL pointer", 0, 0, 0);
    return;
  }

  if (call_event == CM_CALL_EVENT_INFO) {
    /* copy and return. Received only if ICM called get info */
    core_call_info = &(ICMCoreObj->m_call_info[call_info_ptr->call_id]);
    OEMCM_CopyInCallInfo(core_call_info, call_info_ptr);

    return;
  }

#ifdef FEATURE_CCBS
#error code not present
#endif

  evt = OEMCM_CallEventToAEECM(call_event);
  if ( evt == AEECM_EVENT_NONE)
    return;

  MSG_FATAL("AEECMEvent is 0x%x\n", evt, 0, 0);

  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, sizeof(cm_mm_call_info_s_type));
  if (!event_cb)
    return;

  event_cb->event = evt;
  (void) MEMCPY(event_cb->data, call_info_ptr, sizeof(cm_mm_call_info_s_type));

  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWCallEventCB, event_cb);
  AEE_ResumeCallback(&(event_cb->cb), 0);

  return;
}

/*=============================================================================
FUNCTION: OEMCM_DMSSCallErrCB_USR

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSCallErrCB_USR (void *data_block_ptr,
                             cm_call_cmd_e_type call_cmd,
                             cm_call_cmd_err_e_type call_cmd_err)
{
  AEECMEventCBS *event_cb;
  AEECMCallErrCBPkt *params;
  AEECMCallError  err;

  PARAM_NOT_REF(call_cmd);

  if (call_cmd_err == CM_CALL_CMD_ERR_NOERR)
    MSG_MED("OEMCM_DMSSCallErrCB received result 0x%X for Call command %d",
                                         call_cmd_err, call_cmd, 0);
  else
    MSG_ERROR("OEMCM_DMSSCallErrCB received result 0x%X for Call command %d",
                                         call_cmd_err, call_cmd, 0);

  /* Call error Notifications are sent up even if the app that
     caused the error exits */
  if (!ICMCoreObj || !ICMNotifierObj)
    return;

  event_cb = (AEECMEventCBS *)data_block_ptr;
  if (!event_cb)
    return;

  params = (AEECMCallErrCBPkt *)event_cb->data;
  if (!params)
  {
     OEMCM_FreeEventCBS(event_cb);
    return;
  }

  if (params->call_id >= CM_CALL_ID_MAX) {
    MSG_ERROR("OEMCM_DMSSCallErrCB received invalid call id from CM %d",
               params->call_id, 0, 0);
    OEMCM_FreeEventCBS(event_cb);
    return;

  }

  /* Handle state changes due to command error */
  OEMCM_HandleCommandError(params->call_id, call_cmd, call_cmd_err);

  err = OEMCM_CallErrorToAEECM(call_cmd_err);
  if (err == AEECM_CALL_ERROR_NONE) {
    OEMCM_FreeEventCBS(event_cb);
    return;
  }

  event_cb->event = AEECM_EVENT_CALL_ERROR;
  params->call_error = err;
  params->call_cmd = call_cmd;

  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWCallErrCB, event_cb);

  AEE_ResumeCallback(&(event_cb->cb), 0);

  return;
}

/*=============================================================================
FUNCTION: OEMCM_DMSSCallErrCB

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSCallErrCB (void *data_block_ptr,
                             cm_call_cmd_e_type call_cmd,
                             cm_call_cmd_err_e_type call_cmd_err)
{
  OEMCM_DMSSCallErrCB_USR ( data_block_ptr, call_cmd, call_cmd_err );
}

/*=============================================================================
FUNCTION: OEMCM_DMSSOtherCallErrCB

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSOtherCallErrCB(cm_call_id_type call_id,
                                 cm_client_id_type client_id,
                                 cm_call_cmd_e_type cmd,
                                 cm_call_cmd_err_e_type cmd_err)
{
  AEECMEventCBS *event_cb;
  AEECMCallErrCBPkt *params;
  AEECMCallError  err;

  PARAM_NOT_REF(cmd);

  MSG_HIGH("OEMCM_DMSSOtherCallErrCB received result 0x%X for Call command %d",
            cmd_err, cmd, 0);

  if (!ICMCoreObj || !ICMNotifierObj) {
    MSG_ERROR("OEMCM Call Callback called by DMSS", 0, 0, 0);
    return;
  }

  if (call_id >= CM_CALL_ID_MAX) {
    MSG_ERROR("OEMCM_DMSSOtherCallErrCB received invalid call id from CM %d",
               call_id, 0, 0);
    return;
  }

  /* suppress these command errors */
  if (cmd == CM_CALL_CMD_INFO_GET || cmd == CM_CALL_CMD_INFO_LIST_GET) {
    return;
  }

  /* Handle state changes due to command error */
  OEMCM_HandleCommandError(call_id, cmd, cmd_err);

  /* Suppress errors from PD client */
  if (client_id == (int)CM_CLIENT_TYPE_PD)
    return;

  err = OEMCM_CallErrorToAEECM(cmd_err);
  if (err == AEECM_CALL_ERROR_NONE)
    return;

  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, sizeof(AEECMCallErrCBPkt));
  if (!event_cb)
    return;

  event_cb->event = AEECM_EVENT_CALL_ERROR;
  params = (AEECMCallErrCBPkt *)event_cb->data;
  params->call_id = call_id;
  params->call_error = err;
  params->call_cmd = cmd;

  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWCallErrCB, event_cb);

  AEE_ResumeCallback(&(event_cb->cb), 0);

  return;
}

/*=============================================================================
FUNCTION: OEMCM_DMSSPhEventCB

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSPhEventCB(cm_ph_event_e_type  ph_event,
                              const cm_ph_info_s_type *ph_info_ptr)
{
  AEECMEventCBS *    event_cb;
  AEECMEvent        evt;

  MSG_HIGH("Received Phone Event 0x%X from DMSS CM", ph_event, 0, 0);

  if (!ICMCoreObj || !ph_info_ptr) {
    MSG_ERROR("OEMCM_DMSSPhEventCB: Unexpected NULL pointer", 0, 0, 0);
    return;
  }

  switch (ph_event) {
    case CM_PH_EVENT_INFO:
      /* Copy and Return */
      OEMCM_CopyInPhInfo(&ICMCoreObj->m_ph_info, ph_info_ptr);
      ICMCoreObj->m_ph_valid = TRUE;
      return;

    case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
    case CM_PH_EVENT_PREFERRED_NETWORKS_CONF:
    case CM_PH_EVENT_TERMINATE_GET_NETWORKS:
      OEMCM_DMSSGetNWEventCB(ph_info_ptr, ph_event);
      return;

#ifdef FEATURE_ALS
#error code not present
#endif

    default:
      break;
  }

  evt = OEMCM_PhEventToAEECM(ph_event);
  if (evt == AEECM_EVENT_NONE)
    return;

  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, sizeof(cm_ph_info_s_type));
  if (!event_cb)
    return;

  event_cb->event = evt;
  (void) MEMCPY(event_cb->data, ph_info_ptr, sizeof(cm_ph_info_s_type));

  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWPhEventCB, event_cb);

  AEE_ResumeCallback(&(event_cb->cb), 0);

  return;
}

/*=============================================================================
FUNCTION: OEMCM_DMSSSSEventCB

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSSSEventCB(cm_ss_event_e_type ss_event,
                           const cm_mm_ss_info_s_type   *ss_info_ptr)
{
  AEECMEventCBS *    event_cb;
  AEECMEvent        evt;

  MSG_HIGH("Received SS Event 0x%X from DMSS CM", ss_event, 0, 0);

  if (!ICMCoreObj) {
    MSG_ERROR("OEMCM SS Callback called by DMSS", 0, 0, 0);
    return;
  }

  evt = OEMCM_SSEventToAEECM(ss_event);
  if (evt == AEECM_EVENT_NONE)
    return;

  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, sizeof(cm_mm_ss_info_s_type));
  if (!event_cb)
    return;

  event_cb->event = evt;
  (void) MEMCPY(event_cb->data, ss_info_ptr, sizeof(cm_mm_ss_info_s_type));

  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWSSEventCB, event_cb);

  AEE_ResumeCallback(&(event_cb->cb), 0);

  return;
}

/*=============================================================================
FUNCTION: OEMCM_DMSSInbandEventCB

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSInbandEventCB(cm_inband_event_e_type inband_event,
                               const cm_inband_info_s_type *inband_info_ptr)
{
  AEECMEventCBS *event_cb;
  AEECMEvent evt;

  if (!ICMCoreObj) {
    MSG_ERROR("OEMCM Inband Callback called by DMSS", 0, 0, 0);
    return;
  }

  evt = OEMCM_InbandEventToAEECM(inband_event);
  if ( evt == AEECM_EVENT_NONE)
    return;

  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, sizeof(cm_inband_info_s_type));
  if (!event_cb)
    return;

  event_cb->event = evt;
  (void) MEMCPY(event_cb->data, inband_info_ptr, sizeof(cm_inband_info_s_type));

  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWInbandEventCB, event_cb);

  AEE_ResumeCallback(&(event_cb->cb), 0);

  return;
}

/*=============================================================================
FUNCTION: OEMCM_DMSSPhErrCB_USR

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/

static void OEMCM_DMSSPhErrCB_USR(void *data_block_ptr,
                         cm_ph_cmd_e_type ph_cmd,
                         cm_ph_cmd_err_e_type ph_cmd_err)
{

  AEEObjectHandle hObject;
  AEECMErrCBS *pcb;
  AEECallback *cb;
  ICM * pMe;
  void * pError;

  PARAM_NOT_REF(ph_cmd);

  if (ph_cmd_err == CM_PH_CMD_ERR_NOERR)
    MSG_MED("OEMCM_DMSSPhErrCB received result 0x%X for Phone command %d",
                                    ph_cmd_err, ph_cmd, 0);
  else
    MSG_ERROR("OEMCM_DMSSPhErrCB received result 0x%X for Phone command %d",
                                    ph_cmd_err, ph_cmd, 0);

  if (!data_block_ptr) {
    /* Dont care about error for this ph_cmd */
    return;
  }

  hObject = (AEEObjectHandle) data_block_ptr;

  pcb = AEEObjectMgr_GetObject(hObject);

  if(!pcb) {
    /* App has exited/cancelled, do not call error callback */
    return;
  }

  (void) AEEObjectMgr_Unregister(hObject);
  pcb->hObject = NULL;

  pMe = (ICM *) pcb->pOwner;
  cb = pcb->pcb;
  pError = pcb->pError;

  if (!pMe || !pMe->m_pac || !cb) {
    MSG_ERROR("Error callback corrupted", 0, 0, 0);
    return;
  }

  /* Free error callback structure */
  OEMCM_FreeErrCBS(pMe, pcb);

  /* Reset App callback cancel functions */
  cb->pfnCancel = NULL;
  cb->pCancelData = NULL;

  /* Copy error value to App status pointer */
  *((AEECMPhError *)pError) = OEMCM_PhErrorToAEECM(ph_cmd_err);

  /* Call App callback */
  AEE_ResumeCallback(cb, pMe->m_pac);

}

/*=============================================================================
FUNCTION: OEMCM_DMSSPhErrCB

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSPhErrCB(void *data_block_ptr,
                         cm_ph_cmd_e_type ph_cmd,
                         cm_ph_cmd_err_e_type ph_cmd_err)
{
  OEMCM_DMSSPhErrCB_USR ( data_block_ptr, ph_cmd, ph_cmd_err );
}

/*=============================================================================
FUNCTION: OEMCM_DMSSGetNWErrCB

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSGetNWErrCB(void *data_block_ptr,
                         cm_ph_cmd_e_type ph_cmd,
                         cm_ph_cmd_err_e_type ph_cmd_err)
{
  AEEObjectHandle hObject;
  AEECMGetNWCBS *pcb;
  AEECallback *cb;
  ICM * pMe;
  void * pError;

  PARAM_NOT_REF(ph_cmd);

  if (ph_cmd_err == CM_PH_CMD_ERR_NOERR) {
    /* Do not call callback on SUCCESS, it is called when the actual event
       occurs */
    MSG_MED("OEMCM_DMSSGetNWErrCB received result 0x%X for Phone command %d",
                                    ph_cmd_err, ph_cmd, 0);
    return;
  }

  MSG_ERROR("OEMCM_DMSSGetNWErrCB received result 0x%X for Phone command %d",
                                    ph_cmd_err, ph_cmd, 0);

  if (!data_block_ptr) {
    MSG_ERROR("Unexpected NULL pointer", 0, 0, 0);
    return;
  }

  hObject = (AEEObjectHandle) data_block_ptr;

  pcb = AEEObjectMgr_GetObject(hObject);

  if(!pcb) {
    /* App has exited/cancelled, do not call error callback */
    return;
  }

  (void) AEEObjectMgr_Unregister(hObject);
  pcb->hObject = NULL;

  pMe = (ICM *) pcb->pOwner;
  cb = pcb->pcb;
  pError = pcb->pError;

  if (!pMe || !pMe->m_pac || !cb) {
    MSG_ERROR("Error callback corrupted", 0, 0, 0);
    return;
  }

  /* Reset App callback cancel functions */
  cb->pfnCancel = NULL;
  cb->pCancelData = NULL;

  /* Copy error value to App status pointer */
  *((AEECMPhError *)pError) = OEMCM_PhErrorToAEECM(ph_cmd_err);

  /* Call App callback */
  AEE_ResumeCallback(cb, pMe->m_pac);
  pcb->bInUse = FALSE;
}


/*=============================================================================
FUNCTION: OEMCM_DMSSGetNWEventCB

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSGetNWEventCB(const cm_ph_info_s_type *ph_info,
                                   cm_ph_event_e_type event)
{
  AEECMGetNWCBS * err_cb;
  AEECMPhError *pError;

  if (!ICMCoreObj || !ph_info)
    return;

  err_cb = &ICMCoreObj->m_getnw_cb;

  if (!err_cb->bInUse)
    return;

  if (err_cb->hObject) {
    (void) AEEObjectMgr_Unregister(err_cb->hObject);
    err_cb->hObject = NULL;
  }

  pError = (AEECMPhError *)err_cb->pError;
  if (!pError || !err_cb->list || !err_cb->pcb) {
    MSG_ERROR("Get Network error callback corrupted", 0, 0, 0);
    return;
  }

  *pError = (AEECMPhError)AEE_SUCCESS;

  switch (event) {
    case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
#ifdef FEATURE_WLAN
#error code not present
#else
      OEMCM_CopyInAvailableNetworks(&ICMCoreObj->m_ph_info.available_networks,
                                    &ph_info->available_networks);
#endif
#ifdef FEATURE_WLAN
#error code not present
#endif
      {
        if (err_cb->list_type != AEECM_NETWORK_LIST_AVAILABLE)
        {
          MSG_ERROR("OEMCM_DMSSGetNWEventCB: Unexpected List Type %d",
              err_cb->list_type, 0, 0);
        }

        (void) MEMCPY(err_cb->list,
                      &ICMCoreObj->m_ph_info.available_networks,
                      err_cb->list_size);
      }
      break;

    case CM_PH_EVENT_PREFERRED_NETWORKS_CONF:
#ifdef FEATURE_WLAN
#error code not present
#else
      OEMCM_CopyInUserPrefNetworks(&ICMCoreObj->m_ph_info.user_pref_networks,
                                    &ph_info->user_pref_networks);
#endif
      if (err_cb->list_type != AEECM_NETWORK_LIST_USER_PREFERRED)
      {
        MSG_ERROR("OEMCM_DMSSGetNWEventCB: Unexpected List Type %d",
            err_cb->list_type, 0, 0);
      }
      (void) MEMCPY(err_cb->list,
                    &ICMCoreObj->m_ph_info.user_pref_networks,
                    err_cb->list_size);
      break;

    case CM_PH_EVENT_TERMINATE_GET_NETWORKS:
      *pError = AEECM_PH_ERROR_IN_USE_S;
    break;

    default:
      *pError = AEECM_PH_ERROR_OTHER;
      break;
  }

  err_cb->pcb->pfnCancel = NULL;
  err_cb->pcb->pCancelData = NULL;

  AEE_ResumeCallback(err_cb->pcb, err_cb->pOwner->m_pac);
  err_cb->bInUse = FALSE;
}

/*=============================================================================
FUNCTION: OEMCM_DMSSInbandErrCB_USR

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSInbandErrCB_USR(void * data_block_ptr,
                              cm_inband_cmd_e_type inband_cmd,
                              cm_inband_cmd_err_e_type inband_cmd_err)
{
  PARAM_NOT_REF(data_block_ptr);
  PARAM_NOT_REF(inband_cmd);
  PARAM_NOT_REF(inband_cmd_err);

  MSG_MED("OEMCM_DMSSInbandErrCB received result 0x%X for Inband command %d",
                                    inband_cmd_err, inband_cmd, 0);
}

/*=============================================================================
FUNCTION: OEMCM_DMSSInbandErrCB

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_DMSSInbandErrCB(void * data_block_ptr,
                              cm_inband_cmd_e_type inband_cmd,
                              cm_inband_cmd_err_e_type inband_cmd_err)
{
  OEMCM_DMSSInbandErrCB_USR (data_block_ptr,inband_cmd,inband_cmd_err);
}

#ifdef FEATURE_MBMS
/*=============================================================================
FUNCTION: OEMCM_DMSSMBMSEventCB

DESCRIPTION:
   This callback is registered with CM to receive MBMS related events.
   This function is called by CM and in turn queues a callback to be called
   in the BREW context.

PARAMETERS:
  mbms_event         [in]: MBMS related event from CM
  mbms_info_ptr      [in]: Struct that is filled by CM with MBMS event info

=============================================================================*/
static void OEMCM_DMSSMBMSEventCB(cm_mbms_event_e_type mbms_event,
                      const cm_mbms_info_s_type *mbms_info_ptr)
{
  AEECMEventCBS *event_cb;
  AEECMEvent    evt;

  if (NULL == ICMCoreObj)
  {
     ERR("OEMCM_DMSSMBMSEventCB: ICMCoreOb is NULL",0,0,0);
     return;
  }

  if (NULL == mbms_info_ptr)
  {
     ERR("OEMCM_DMSSMBMSEventCB: Null Param",0,0,0);
     return;
  }

  MSG_HIGH("Received MBMS Event 0x%X from DMSS CM", mbms_event, 0, 0);
  MSG_HIGH("Received Rpt_type %d ",mbms_info_ptr->mbms_generic_rpt.rpt_type,0,0);

  evt = OEMCM_MBMSEventToAEECM(mbms_event);
  if (evt == AEECM_EVENT_NONE)
  {
     MSG_ERROR("Invalid MBMS event %d",mbms_event,0,0);
     return;
  }

  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, sizeof(cm_mbms_info_s_type));
  if (NULL == event_cb)
  {
     MSG_ERROR("Could not allocate event CB",0,0,0);
     return;
  }

  event_cb->event = evt;

  if (NULL == event_cb->data)
  {
     ERR("OEMCM_DMSSMBMSEventCB: invalid NULL ptr",0,0,0);
     OEMCM_FreeEventCBS(event_cb);
     return;
  }

  (void) MEMCPY(event_cb->data, mbms_info_ptr, sizeof(cm_mbms_info_s_type));

  //Have BREW call OEMCM_BREWMBMSEventCB within it's context
  //Event and event data will be passed to the app through a notify event
  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWMBMSEventCB, event_cb);
  AEE_ResumeCallback(&(event_cb->cb), 0);
}

/*=============================================================================
FUNCTION: OEMCM_DMSSMbmsErrCB

DESCRIPTION:
   This callback is registed with CM through the call to cm_mbms_cmd_generic,
   cm calls this function if there is a problem with that function.

   Currently does nothing but spit out a debug message.
=============================================================================*/
static void OEMCM_DMSSMbmsErrCB(void *data_block_ptr,
                                cm_mbms_cmd_e_type mbms_cmd,
                                cm_mbms_cmd_err_e_type mbms_cmd_err)
{
  MSG_ERROR("OEMCM_DMSSMbmsErrCB: %d",mbms_cmd_err,0,0);
}

#endif //#ifdef FEATURE_MBMS

/*===========================================================================

                    Callbacks Executed in BREW/App Context

===========================================================================*/


/*=============================================================================
FUNCTION: OEMCM_BREWCallEventCB

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_BREWCallEventCB(void *data)
{
  AEECMEventCBS *          event_cb;
  AEECMEvent              event;
  PFNOEMCMCALLNOTIFY      pfnNotify = NULL;
  uint16                  fn_index;
  cm_mm_call_info_s_type * call_info;

  event_cb = (AEECMEventCBS *)data;

  if (!data)
  {
    MSG_ERROR("OEMCM_BREWCallEventCB: Unexpected NULL pointer", 0, 0, 0);
    return;
  }
  else if (!ICMCoreObj)
  {
    MSG_ERROR("OEMCM_BREWCallEventCB: Unexpected NULL pointer", 0, 0, 0);
     OEMCM_FreeEventCBS(event_cb);
    return;
  }

  MSG_FATAL("OEMCM_BREWCallEventCB: received AEECM event 0x%X", event_cb->event,
                                                          0, 0);

  event = event_cb->event;
  call_info = (cm_mm_call_info_s_type *) event_cb->data;

  fn_index = (uint16)((int32)event & 0x00FF);

  do
  {
  /* Find the event handler associated with this event.
   * Event handler table is indexed by the last two nibbles of event.
   * This is basically a huge switch case statement.
   */
    if (AEECM_EVENT_GROUP((uint16)event) == AEECM_CALL_GROUP)
    {
    /* Main call group */
      if (fn_index >= ARR_SIZE(gCallEventHandlers))
      {
      MSG_ERROR("OEMCM_BREWCallEventCB: No event handler for this event (0x%X)",
                                                              event, 0, 0);
        break;
    }
    pfnNotify = gCallEventHandlers[fn_index];
    }
    else if (AEECM_EVENT_GROUP((uint16)event) == AEECM_INTERNAL_CALL_GROUP)
    {
    /* Internal call events that are not sent to the App */
      if (fn_index >= ARR_SIZE(gCallIntEventHandlers))
      {
      MSG_ERROR("OEMCM_BREWCallEventCB: No event handler for this event (0x%X)",
                                                              event, 0, 0);
        break;
    }
    pfnNotify = gCallIntEventHandlers[fn_index];
    }
    else
    {
    MSG_ERROR("OEMCM_BREWCallEventCB: Unknown event (0x%X)", event, 0, 0);
      break;
  }


  /* Found the event handler, now call it */
    if (pfnNotify)
    {
    (*pfnNotify)(call_info, event);
    }
    else
    {
    MSG_ERROR("OEMCM_BREWCallEventCB: No event handler found for event 0x%X",
                                                                  event, 0, 0);
  }
  }
  while(0);

  OEMCM_FreeEventCBS(event_cb);

  return;
}

/*=============================================================================
FUNCTION: OEMCM_BREWCallErrCB

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_BREWCallErrCB(void *data)
{
  AEECMEventCBS *    event_cb = (AEECMEventCBS *)data;
  AEECMCallErrCBPkt *params;
  AEECMNotifyInfo * notify_info;
  AEECMCallInfo *   core_call_info;
  int i;

  if (!event_cb)
  {
     return;
  }
  else if (!ICMCoreObj)
  {
     OEMCM_FreeEventCBS(event_cb);
    return;
  }

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    OEMCM_FreeEventCBS(event_cb);
    return;
  }

  notify_info->event = event_cb->event;
  params = (AEECMCallErrCBPkt *)event_cb->data;
  notify_info->event_data.call.call_id = CM_CALL_ID_INVALID;
  notify_info->event_data.call.call_error = params->call_error;

  //see if we can match up to a call id
  for (i = 1; i < ARR_SIZE(ICMCoreObj->m_internal_call_ids); i++)
  {
    if (ICMCoreObj->m_internal_call_ids[i] == params->call_id)
    {
      notify_info->event_data.call.call_id = i;
    }
  }


  core_call_info = &(ICMCoreObj->m_call_info[params->call_id]);
  /* Copy call info from core object to Notify info */
  if (core_call_info->call_state != AEECM_CALL_STATE_IDLE &&
      core_call_info->call_state != AEECM_CALL_STATE_ENDED &&
      core_call_info->call_state != AEECM_CALL_STATE_SETUP) {
    (void) MEMCPY(&(notify_info->event_data.call.call_info), core_call_info,
           sizeof(AEECMCallInfo));
  }

#if (defined FEATURE_APP_MPEG4 && defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);

  /* Handle origination failure after event notification */
  if (params->call_cmd == CM_CALL_CMD_ORIG)
  {
    ICMCoreObj->m_state[params->call_id].bValid = FALSE;
    OEMCM_FREE_CALLID(params->call_id);
  }

  OEMCM_FreeEventCBS(event_cb);
}

/*=============================================================================
FUNCTION: OEMCM_BREWPhEventCB

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_BREWPhEventCB(void *data)
{
  AEECMEventCBS *          event_cb;
  AEECMEvent              event;
  PFNOEMCMPHNOTIFY        pfnNotify = NULL;
  uint16                  fn_index;
  cm_ph_info_s_type *     ph_info;

  event_cb = (AEECMEventCBS *)data;

  if (!data)
  {
     return;
  }
  else if (!ICMCoreObj)
  {
     OEMCM_FreeEventCBS(event_cb);
     return;
  }
  event = event_cb->event;
  ph_info = (cm_ph_info_s_type *) event_cb->data;

  fn_index = (uint16)((int)event & 0x00FF);

  /* Find the event handler associated with this event.
     Event handler table is indexed by the last two nibbles of event */

  if (AEECM_EVENT_GROUP((uint16)event) == AEECM_PH_GROUP) {
    if (fn_index >= ARR_SIZE(gPhEventHandlers)) {
      MSG_ERROR("OEMCM_BREWPhEventCB: No event handler for this event (0x%X)",
                                                              event, 0, 0);
      OEMCM_FreeEventCBS(event_cb);
      return;
    }
    pfnNotify = gPhEventHandlers[fn_index];
  } else if (AEECM_EVENT_GROUP((uint16)event) == AEECM_INTERNAL_PH_GROUP) {
    if (fn_index >= ARR_SIZE(gPhIntEventHandlers)) {
      MSG_ERROR("OEMCM_BREWPhEventCB: No event handler for this event (0x%X)",
                                                              event, 0, 0);
      OEMCM_FreeEventCBS(event_cb);
      return;
    }
    pfnNotify = gPhIntEventHandlers[fn_index];
  }

  /* Got the event handler */
  if (pfnNotify) {

    MSG_HIGH("Sending Phone event 0x%X to App", event, 0, 0);

    /* Call the event handler */
    (*pfnNotify)(ph_info, event);

  } else {

    MSG_ERROR("OEMCM_BREWPhEventCB: No event handler found for event 0x%X",
                                                                  event, 0, 0);
  }

  OEMCM_FreeEventCBS(event_cb);

  return;
}

/*=============================================================================
FUNCTION: OEMCM_BREWSSEventCB

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_BREWSSEventCB(void *data)
{
  AEECMEventCBS *        event_cb;
  AEECMNotifyInfo *     notify_info;
  cm_mm_ss_info_s_type * ss_info;
  AEECMSSInfo           * core_ss_info;

  event_cb = (AEECMEventCBS *)data;

  if (!data)
  {
     return;
  }
  else if (!ICMCoreObj)
  {
     OEMCM_FreeEventCBS(event_cb);
     return;
  }
  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    OEMCM_FreeEventCBS(event_cb);
    MSG_LOW("OEMCM DEBUG:: OEMCM Failed to allocate memory", 0, 0, 0);
    return;
  }
  MSG_LOW("OEMCM DEBUG:: OEMCM Alloc successful for 0x%X",notify_info, 0, 0);
  notify_info->event = event_cb->event;
  ss_info = (cm_mm_ss_info_s_type *)event_cb->data;

  core_ss_info = &ICMCoreObj->m_ss_info;

  switch(notify_info->event) {
    case AEECM_EVENT_SS_SRV_CHANGED:
      OEMCM_CopyInSSInfo(core_ss_info, ss_info);
      notify_info->event_data.ss.changed_fields =
                            OEMCM_SSMaskToAEECM(ss_info->changed_fields);
      break;

    case AEECM_EVENT_SS_RSSI:
      core_ss_info->rssi = ss_info->rssi;
#if defined(CM_API_SIGNAL_STRENGTH_INDICATION)
      core_ss_info->rssi2 = ss_info->rssi2;
      core_ss_info->ecio = ss_info->ecio;
      core_ss_info->ecio_for_display = ss_info->ecio_for_display;
#else
      /* Default to R5 so that 0 bars are displayed */
      core_ss_info->rssi2 = AEECM_DEFAULT_RSSI_TO_R5;
      /* Default to E5 so that 0 bars are displayed */
      core_ss_info->ecio = AEECM_DEFAULT_ECIO_TO_E5;
      core_ss_info->ecio_for_display = FALSE;
#endif
      break;

    case AEECM_EVENT_SS_HDR_RSSI:
      core_ss_info->hdr_rssi = ss_info->hdr_rssi;
      core_ss_info->hdr_sinr = ss_info->hdr_sinr;
      break;

#ifdef FEATURE_WLAN
#error code not present
#endif

    case AEECM_EVENT_SS_INFO:
      /* Copy and Return */
      OEMCM_CopyInSSInfo(core_ss_info, ss_info);
      OEMCM_FREE(notify_info);
      OEMCM_FreeEventCBS(event_cb);
      return;

    default:
      MSG_LOW("OEMCM DEBUG:: OEMCM Freeing for 0x%X",notify_info, 0, 0);
      OEMCM_FREE(notify_info);
      OEMCM_FreeEventCBS(event_cb);
      return;
  }

  /* if we're not in hybrid, we get RSSI events, not HDR_RSSI */
  if (!ss_info->hdr_hybrid && OEMCM_GetSystemMode() == AEECM_SYS_MODE_HDR) {
    core_ss_info->hdr_rssi = ss_info->rssi;
    core_ss_info->hdr_sinr = ss_info->sinr;
    core_ss_info->hdr_roam_status = (AEECMRoamStatus)ss_info->roam_status;
    core_ss_info->hdr_srv_status = (AEECMSrvStatus)ss_info->srv_status;
  }

  (void) MEMCPY(&notify_info->event_data.ss.ss_info, core_ss_info,
                sizeof(AEECMSSInfo));

  OEMCM_NotifyApps(notify_info);

  MSG_LOW("OEMCM DEBUG:: OEMCM Freeing for 0x%X",notify_info, 0, 0);
  OEMCM_FREE(notify_info);

  OEMCM_FreeEventCBS(event_cb);
}

/*=============================================================================
FUNCTION: OEMCM_BREWInbandEventCB

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_BREWInbandEventCB(void *data)
{
  AEECMEventCBS *    event_cb = (AEECMEventCBS *)data;
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  cm_inband_info_s_type * inband_info;

  if (!data)
  {
     return;
  }
  else if (!ICMCoreObj)
  {
     OEMCM_FreeEventCBS(event_cb);
    return;
  }

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    OEMCM_FreeEventCBS(event_cb);
    return;
  }

  notify_info->event = event_cb->event;

  inband_info = (cm_inband_info_s_type *) event_cb->data;

  call_id = inband_info->call_id;

  switch(notify_info->event) {

    case AEECM_EVENT_INBAND_FWD_BURST_DTMF:
    case AEECM_EVENT_INBAND_FWD_START_CONT_DTMF:
    case AEECM_EVENT_INBAND_FWD_STOP_CONT_DTMF:
      notify_info->event_data.inb.on_length = inband_info->on_length;
      notify_info->event_data.inb.off_length = inband_info->off_length;

      OEMCM_CopyCharLenToAECHAR((char *)inband_info->digits,
                                 inband_info->cnt,
                                 notify_info->event_data.inb.digits,
                                 sizeof(notify_info->event_data.inb.digits));
      break;

    default:
      MSG_ERROR("Unexpected event: 0x%X", notify_info->event,0,0);
      OEMCM_FREE(notify_info);
      OEMCM_FreeEventCBS(event_cb);
      return;
  }

  notify_info->event_data.inb.call_id = OEMCM_AEECALLID(call_id);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);

  OEMCM_FreeEventCBS(event_cb);

}

#ifdef FEATURE_MBMS

/*=============================================================================
FUNCTION: OEMCM_BREWMBMSEventCB

DESCRIPTION:
   This callback is registered when OEMCM_DMSSMBMSEventCB is called from
   CM context.  This is the equivalent callback that gets executed in
   BREW context and passes the MBMS related event and data from CM to
   the apps that care about it.

PARAMETERS:
  pData             [in]: AEECMEventCBS structure contains event data
                          Memory for this struct is freed in this function
                          and allocated in OEMCM_DMSSMBMSEventCB.
=============================================================================*/
static void OEMCM_BREWMBMSEventCB(void *pData)
{
  AEECMEventCBS       *pEvtCb;
  AEECMNotifyInfo     *pNotify;

  if (NULL == pData)
  {
    ERR("OEMCM_BREWMBMSEventCB: NULL param",0,0,0);
    return;
  }

  if (NULL == ICMCoreObj)
  {
    ERR("OEMCM_BREWMBMSEventCB: NULL param",0,0,0);
    return;
  }

  pEvtCb = (AEECMEventCBS *)pData;

  if ((pNotify = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL)
  {
    OEMCM_FreeEventCBS(pEvtCb);
    MSG_ERROR("OEMCM DEBUG:: OEMCM Failed to allocate memory", 0, 0, 0);
    return;
  }

  pNotify->event = pEvtCb->event;

  //Copy the items in the cm_mbms_info_s_type struct to the
  //AEECMMBMSEventData struct which will be passed to the apps
  OEMCMHelper_CopyMBMSInfo(&pNotify->event_data.mbms, (cm_mbms_info_s_type *)pEvtCb->data);

  OEMCM_NotifyApps(pNotify);

  OEMCM_FREE(pNotify);
  OEMCM_FreeEventCBS(pEvtCb);

}

#endif //#ifdef FEATURE_MBMS

/*===========================================================================

                          Event Handlers

===========================================================================*/
#if (defined FEATURE_APP_MPEG4 && defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* (defined FEATURE_APP_MPEG4 && defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION) */

/*=============================================================================
FUNCTION: OEMCM_HandleCallSetupInd

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallSetupInd(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  MSG_MED("OEMCM_HandleCallSetupInd received event 0x%X", event, 0, 0);

  if (event != AEECM_EVENT_INTERNAL_SETUP_IND)
  {
    MSG_ERROR("OEMCM_HandleCallSetupInd received unexpected event 0x%X",
              event, 0, 0);
    return;
  }

  ICMCoreObj->m_call_info[call_info->call_id].call_mode_info = (AEECMCallModeInfo)call_info->mode_info.info_type;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
  /* If this is a voice call in dual mode do not send setup response to CM.
   * Data services will take care of this and UI will only get an indication from CM that the
   * setup response was sent
   */
  if ((CM_CALL_TYPE_VOICE == call_info->call_type) &&
      (call_info->mode_info.info.gw_cs_call.bearer_capability_1.present) &&
      (call_info->mode_info.info.gw_cs_call.bearer_capability_2.present) &&
      ((REPEAT_SERVICE_CHANGE_FALLBACK == call_info->mode_info.info.gw_cs_call.bc_repeat_indicator.repeat_indication) ||
       (REPEAT_SUPPORTS_FALLBACK == call_info->mode_info.info.gw_cs_call.bc_repeat_indicator.repeat_indication)))
  {
    MSG_MED("OEMCM_HandleCallSetupInd Returning Without a Response, call type %d BC1 Flag=%d BC2 Flag=%d",
            call_info->call_type,
            call_info->mode_info.info.gw_cs_call.bearer_capability_1.present,
            call_info->mode_info.info.gw_cs_call.bearer_capability_2.present);
    return;
  }
#endif
  if (call_info->call_type != CM_CALL_TYPE_VOICE)
  {
    MSG_MED("OEMCM_HandleCallSetupInd returns, call type %d", call_info->call_type, 0, 0);
    return;
  }

  ICMCoreObj->m_call_info[call_info->call_id].call_state = AEECM_CALL_STATE_SETUP;


#if (defined FEATURE_APP_MPEG4 && defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* (defined FEATURE_APP_MPEG4 && defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION) */
  ICMCoreObj->m_bMediaPause = FALSE;
  OEMCM_SendSetupResponse(call_info->call_id,TRUE);
  if (call_info->signal.is_signal_info_avail)
    OEMCM_HandleCallSignal(call_info, AEECM_EVENT_CALL_SIGNAL);

}

/*=============================================================================
FUNCTION: OEMCM_HandleCallMngCallsConf

DESCRIPTION:
  Send a SUPS_ERROR to application if lower layer returns an error after
  a sups command.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallMngCallsConf(cm_mm_call_info_s_type *call_info,
                                            AEECMEvent event)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#else
  PARAM_NOT_REF(call_info);
  PARAM_NOT_REF(event);
#endif
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallDefault

DESCRIPTION:
  Copy nothing and notify application.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallDefault(cm_mm_call_info_s_type *call_info,
                                            AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;

  MSG_MED("OEMCM_HandleCallDefault received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  if(event == AEECM_EVENT_CALL_RECALL_IND)
  {
      /*Get the phone Info which will have the CCBS Information*/
    (void) cm_ph_cmd_get_ph_info(NULL, NULL, gCMID);
  }

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_CmIpCallProgToAeecmCallProg

DESCRIPTION:
  Map CmIpCallProgress To AeecmCallProgress

DEPENDENCIES:
  Called only if CM mode_info.info_type == CM_CALL_MODE_INFO_IP

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CmIpCallProgToAeecmCallProg( AEECMCallInfo *dest,
                                               const cm_mm_call_info_s_type *src)
{
#ifdef CM_API_IPCALL_SS_INFO
  if (src->mode_info.info.ip_call.call_prog_info == CM_CALL_PROG_INFO_REMOTE_ALERT)
  {
     MSG_HIGH("CM call prog info remote alert", 0, 0, 0);
      dest->call_progress = AEECM_PROGIND_IP_REMOTE_ALERT;
  }
  else
  {
    dest->call_progress = AEECM_PROGIND_IP_INFO_NONE;
  }
#else
  dest->call_progress = AEECM_PROGIND_IP_INFO_NONE;
#endif
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallProgress

DESCRIPTION:
  Copy nothing and notify application.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallProgress(cm_mm_call_info_s_type *call_info,
                                            AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  AEECMCallInfo * core_call_info;

  MSG_HIGH("OEMCM_HandleCallProgress received event 0x%X %d %d", event, call_info->call_id, call_info->mode_info.info.gw_cs_call.call_progress);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  core_call_info = &(ICMCoreObj->m_call_info[call_info->call_id]);

#ifdef CM_API_IP_CALL
  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_IP )
  {
    OEMCM_CmIpCallProgToAeecmCallProg(core_call_info, call_info);
    MSG_HIGH("CM call prog info remote alert", 0, 0, 0);
  }
  else
  {
    core_call_info->call_progress = (AEECMCallProgress)
                        call_info->mode_info.info.gw_cs_call.call_progress;
  }
#else
  core_call_info->call_progress = (AEECMCallProgress)
                      call_info->mode_info.info.gw_cs_call.call_progress;
#endif

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallNumUpdate

DESCRIPTION:
  Update number and notify application.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallNumUpdate(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;

  MSG_MED("OEMCM_HandleCallNumUpdate received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_CopyNumToAEECMNum(&call_info->num,
                          &notify_info->event_data.call.number);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallEmergencyFlashed

DESCRIPTION:
  Update call type and copy nummber.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallEmergencyFlashed(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMCallInfo   * core_call_info;

  if (!call_info || !ICMCoreObj)
    return;

  core_call_info = &(ICMCoreObj->m_call_info[call_info->call_id]);

  /* Update call type */
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);

  core_call_info->call_mode_info = (AEECMCallModeInfo)call_info->mode_info.info_type;
  /* Copy number and send event to application */
  OEMCM_HandleCallCopyNum(call_info, event);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallCopyNum

DESCRIPTION:
  Copies number to notify info and updates core call info other party no, subaddr
  and alpha.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallCopyNum(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  AEECMCallInfo   * core_call_info;

  AEECMPI pi = (AEECMPI) call_info->num.pi;

  MSG_MED("OEMCM_HandleCallCopyNum received event 0x%X", event, 0, 0);

  if (!call_info || !ICMCoreObj)
    return;

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL)
  {
    MSG_ERROR("Malloc failed",0,0,0);
    return;
  }

  (void) MEMSET(notify_info, 0x00, sizeof(AEECMNotifyInfo));

  core_call_info = &(ICMCoreObj->m_call_info[call_info->call_id]);

  OEMCM_CopyNumToOtherParty(&(call_info->num), core_call_info);

  /* check if Presentation Indicator is set before copying number */
  if (pi == AEECM_PI_RES)
  {
    MEMSET(core_call_info->alpha,0x00,sizeof(core_call_info->alpha));
    MEMSET(core_call_info->other_party_subaddr,0x00,sizeof(core_call_info->other_party_subaddr));
    MSG_HIGH("PI Restricted, suppressing alpha and other_party_subaddr copying",0,0,0);
  }
  else
  {
    OEMCM_CopyCharLenToAECHAR((char *)call_info->num.subaddr.chari,
                              call_info->num.subaddr.num_char,
                              core_call_info->other_party_subaddr,
                              sizeof(core_call_info->other_party_subaddr));

    OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf,
                              call_info->alpha.len,
                              core_call_info->alpha,
                              sizeof(core_call_info->alpha));
  }
  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_CopyNumToAEECMNum(&call_info->num,
                          &notify_info->event_data.call.number);

  core_call_info->call_mode_info = (AEECMCallModeInfo)call_info->mode_info.info_type;

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallAlphaUpdate

DESCRIPTION:
  Update alpha and notify application.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallAlphaUpdate(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEECMCallInfo *   core_call_info;
  char *buf = NULL;
  uint8 numChars = 0;

  MSG_MED("OEMCM_HandleCallAlphaUpdate received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);

  //check for coding scheme, if it is gsm encoding, we need to convert first

  MSG_MED("enc_alpha coding_scheme = %x", call_info->enc_alpha.coding_scheme, 0, 0);

  if ((call_info->enc_alpha.coding_scheme & OEMCM_ALPHABET_MASK)
     == OEMCM_USS_DEF_ALPHABET_LANG_UNSPEC)
  {
    //allocate memory for NULL termination char
    numChars = call_info->enc_alpha.num_characters;
    buf = OEMCM_MALLOC(numChars + 1);
    if (buf  == NULL) {
      MSG_ERROR("Can not allocate buf for gsm coding conversion", 0, 0, 0);
      OEMCM_FREE(notify_info);
      return;
    }

    //convert from gsm encoding to ascii encoding
    OEMCM_ConvertDefaultToAscii((uint8 *)buf, call_info->enc_alpha.buf,
       call_info->enc_alpha.num_bytes);

    MSG_MED("default ascii char with gsm coding converstion is %c%c%c...", buf[0], buf[1], buf[2]);

    OEMCM_CopyCharLenToAECHAR(buf, numChars,
                              core_call_info->alpha,
                              sizeof(core_call_info->alpha));
    OEMCM_FREE(buf);
  }
  else
  {

  OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf,
                            call_info->alpha.len,
                            core_call_info->alpha,
                            sizeof(core_call_info->alpha));
  }
  core_call_info->bAlphaRecvd = TRUE;

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  (void) WSTRLCPY(notify_info->event_data.call.alpha, core_call_info->alpha,
           sizeof(notify_info->event_data.call.alpha)/sizeof(AECHAR));// Gemsea sizeof IN AECHAR

  notify_info->event_data.call.call_info.is_last_cdma_info_rec = 
                             call_info->mode_info.info.cdma_call.is_last_cdma_info_rec; // Gemsea Add
  
  notify_info->event_data.call.disp_type = AEECM_DISP_NORMAL;

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);

}

/*=============================================================================
FUNCTION: OEMCM_HandleCallExtDisplay

DESCRIPTION:
  Update alpha, disp type and notify application.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallExtDisplay(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEECMCallInfo *   core_call_info;
  char *tmpAlpha;
  uint8 tmpAlphaLen;

  AEECMPI pi = (AEECMPI) call_info->num.pi;

  PARAM_NOT_REF(event);
  MSG_MED("OEMCM_HandleCallExtDisplay received event 0x%X", event, 0, 0);

  if (call_info->mode_info.info_type != CM_CALL_MODE_INFO_CDMA)
  {
    MSG_ERROR("CM_CALL_EVENT_EXT_DISP received with wrong mode %d",
              call_info->mode_info.info_type,
              0, 0);
    return;
  }


  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL)
  {
    MSG_ERROR("Malloc failed",0,0,0);
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);

  if (pi == AEECM_PI_RES)
  {
    /* don't copy alpha string if PI is "restricted" */
    MEMSET(core_call_info->alpha,0x00,(sizeof(core_call_info->alpha)*sizeof(AECHAR)));
    MSG_HIGH("PI Restricted, suppressing copying of alpha string",0,0,0);
  }
  else
  {
    /* When alpha string is passed from CM to OEMCM, the display tag
       characters as well as the length of the string are also being
       prepended to the alpha string. OEMCM needs to strip these 2
       characters before passing on the string */
    tmpAlpha = (char *)call_info->alpha.buf + 2; /* skip display tag
                                                    and length*/
    tmpAlphaLen = (uint8)*(call_info->alpha.buf + 1);

    MSG_HIGH("OEMCM received display_tag %x, length %d",
             call_info->alpha.buf[0], tmpAlphaLen, 0);

    OEMCM_CopyCharLenToAECHAR(tmpAlpha,
                              tmpAlphaLen,
                              core_call_info->alpha,
                              sizeof(core_call_info->alpha));
  }

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id,
                              AEECM_EVENT_CALL_DISPLAY);
  if (pi == AEECM_PI_RES)
  {
    MEMSET(notify_info->event_data.call.alpha,0x00,
           (sizeof(notify_info->event_data.call.alpha)*sizeof(AECHAR)));
  }
  else
  {
    (void) WSTRLCPY(notify_info->event_data.call.alpha, core_call_info->alpha,
                    sizeof(notify_info->event_data.call.alpha)/sizeof(AECHAR));// Gemsea sizeof IN AECHAR
  }
  
  notify_info->event_data.call.call_info.is_last_cdma_info_rec = 
                             call_info->mode_info.info.cdma_call.is_last_cdma_info_rec; // Gemsea Add
  
  notify_info->event_data.call.disp_type = (AEECMDispType)
                                           call_info->mode_info.info.cdma_call.ext_disp_type;

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallQOS

DESCRIPTION:
  Copy QOS fields

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallQOS(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

  PARAM_NOT_REF(event);
  MSG_MED("OEMCM_HandleCallQOS received event 0x%X", event, 0, 0);

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif
}


/*=============================================================================
FUNCTION: OEMCM_HandleCallRecallRsp

DESCRIPTION:
  Copy QOS fields

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallRecallRsp(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  PARAM_NOT_REF(event);
  MSG_MED("OEMCM_HandleCallRecallRsp received event 0x%X", event, 0, 0);

#if defined(FEATURE_CCBS)
#error code not present
#endif

}

/*=============================================================================
FUNCTION: OEMCM_HandleCallOrig

DESCRIPTION:
  Handle CM_CALL_EVENT_ORIG event.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallOrig(cm_mm_call_info_s_type *call_info,
                                  AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEECMCallInfo *   core_call_info;
#if defined (FEATURE_ACM)
  OEMCM_ACM_MobileOriginatedCallInfo * ACM_MOCallInfo;
#endif // FEATURE_ACM

  MSG_MED("OEMCM_HandleCallOrig received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);

  /* Update core info */

  OEMCM_ResetPrivateCallState(call_id);
  // TBD
  //(void) MEMSET(core_call_info, 0, sizeof(AEECMCallInfo));

  core_call_info->call_state = AEECM_CALL_STATE_ORIG;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);
  core_call_info->direction = AEECM_CALL_DIRECTION_MO;
  core_call_info->call_mode_info = (AEECMCallModeInfo)call_info->mode_info.info_type;

  OEMCM_CopyNumToOtherParty(&(call_info->num), core_call_info);

  OEMCM_CopyCharLenToAECHAR((char *)call_info->num.subaddr.chari,
                             call_info->num.subaddr.num_char,
                             core_call_info->other_party_subaddr,
                             sizeof(core_call_info->other_party_subaddr));

  OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf,
                            call_info->alpha.len,
                            core_call_info->alpha,
                            sizeof(core_call_info->alpha));

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
    core_call_info->srv_opt =
            (AEECMServiceOption)call_info->mode_info.info.cdma_call.srv_opt;
  }

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  (void) WSTRLCPY(notify_info->event_data.call.alpha, core_call_info->alpha,
           sizeof(notify_info->event_data.call.alpha)/sizeof(AECHAR));// Gemsea sizeof IN AECHAR

  OEMCM_CopyNumToAEECMNum(&call_info->num,
                          &notify_info->event_data.call.number);

#if defined(FEATURE_ACM)
  /* Fill in data required for acquire & delayed acquire for Mobile Originated voice calls */
  if (CM_CALL_TYPE_VOICE == call_info->call_type
          || CM_CALL_TYPE_EMERGENCY == call_info->call_type)
  {
    ACM_MOCallInfo = (OEMCM_ACM_MobileOriginatedCallInfo *) OEMCM_MALLOC (sizeof(OEMCM_ACM_MobileOriginatedCallInfo));

    ACM_MOCallInfo->notify_info = notify_info;
    ACM_MOCallInfo->call_id     = call_id;

    ICMCoreObj->m_pAcmInfo->m_pCBInfo = ACM_MOCallInfo;

    MSG_HIGH ("OEMCM: Check with ACM for MO Voice Call resources", 0, 0, 0);
    /* Acquire resource from Application Concurrency Manager */
    OEMCM_ACMAcquire(ICMCoreObj->m_call_owners[call_id], ICMCoreObj->m_pAcmInfo, ACM_MOBILE_ORIGINATED_VOICE_CALL);
  }
  else
  {
    OEMCM_NotifyApps(notify_info);
    OEMCM_FREE(notify_info);
  }
#else // !FEATURE_ACM
  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
#endif
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallAnswer

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallAnswer(cm_mm_call_info_s_type *call_info,
                                    AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEECMCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallAnswer received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);

  /* Update core call info */

  ICMCoreObj->m_state[call_id].bAnswered = TRUE;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);
  core_call_info->call_mode_info = (AEECMCallModeInfo)call_info->mode_info.info_type;

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {

    core_call_info->srv_opt =
            (AEECMServiceOption)call_info->mode_info.info.cdma_call.srv_opt;
    core_call_info->alert_ans_bypass =
            call_info->mode_info.info.cdma_call.alert_ans_bypass;

  } else {
  #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
 #endif

  }

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_CopyNumToAEECMNum(&call_info->num,
                          &notify_info->event_data.call.number);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallEnd

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallEnd(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type   call_id;
  AEECMCallInfo *   core_call_info;
  boolean           missed = FALSE;

  PARAM_NOT_REF(event);
  MSG_MED("OEMCM_HandleCallEnd received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  /* Update core call info */

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);
  
#if 0//def CUST_EDITION
  if (call_info->call_type == CM_CALL_TYPE_OTAPA)
  {
    MSG_MED("CM_CALL_TYPE_OTAPA release and return", 0, 0, 0);
    core_call_info->call_state = AEECM_CALL_STATE_ENDED;
    OEMCM_FREE(notify_info);
    ICMCoreObj->m_call_owners[call_id] = NULL;
    return;
  }
  
  if (call_info->call_type == CM_CALL_TYPE_SMS)
  {
    MSG_MED("CM_CALL_TYPE_SMS release and return", 0, 0, 0);
    core_call_info->call_state = AEECM_CALL_STATE_ENDED;
    OEMCM_FREE(notify_info);
    ICMCoreObj->m_call_owners[call_id] = NULL;
    return;
  }
#endif

  if (core_call_info->call_state == AEECM_CALL_STATE_ENDED) {
    OEMCM_FREE(notify_info);
    return;
  }


  if (core_call_info->start_time) {
    core_call_info->duration = (GETUPTIMEMS() -
                              core_call_info->start_time);
  } else {
    core_call_info->start_time = GETUPTIMEMS();
  }

  switch (call_info->call_type) {
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_EMERGENCY:
#if (defined FEATURE_APP_MPEG4 && defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif
#ifndef FEATURE_UIONE_HDK
      //OEMCM_AddCallHistory(core_call_info);		//Del By zzg 2010_07_16
#endif
      break;

    case CM_CALL_TYPE_CS_DATA:
      /* Only for Video Calls */
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* GSM || WCDMA */
      break;

    case CM_CALL_TYPE_PS_DATA:
      OEMCM_HandleDataCallEnd(&core_call_info->data_stats, call_info);
      break;

    default:
      break;
  }

  if (!AEECM_IS_CALLSTATE_CONNECTED(core_call_info->call_state) &&
       core_call_info->direction == AEECM_CALL_DIRECTION_MT)
    missed = TRUE;

  core_call_info->call_state = AEECM_CALL_STATE_ENDED;

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id,
                              AEECM_EVENT_CALL_END);

  notify_info->event_data.call.missed = missed;
  notify_info->event_data.call.end_status =
                                  (AEECMEndStatus)call_info->end_status;

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {

    notify_info->event_data.call.end_additional_info = (AEECMEndCause)
                   (((uint8)call_info->mode_info.info.cdma_call.end_reason) |
                            OEMCM_CDMA_END_VALUE);
  } else {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
  }

#if defined (FEATURE_ACM)
  if ((AEECM_CALL_STATE_ENDED == ICMCoreObj->m_call_info[call_id].call_state) &&
    AEECM_CALL_TYPE_VOICE == ICMCoreObj->m_call_info[call_id].call_type
          || AEECM_CALL_TYPE_EMERGENCY == ICMCoreObj->m_call_info[call_id].call_type)
  {
    MSG_HIGH("OEMCM: Call ended.  Check if we need to free ACM resources", 0, 0, 0);
    OEMCM_ACMFree(ICMCoreObj->m_call_owners[call_id], ICMCoreObj->m_pAcmInfo);
  }
#endif // FEATURE_ACM

  OEMCM_NotifyApps(notify_info);
  if (OEMCM_CountActiveCalls(AEECM_CALL_TYPE_NONE, AEECM_CALL_STATE_NONE, NULL) == 0)
  {
    OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, AEECM_EVENT_CALL_ALL_CALLS_ENDED);
    OEMCM_NotifyApps(notify_info);
  }
  OEMCM_FREE(notify_info);

  ICMCoreObj->m_call_owners[call_id] = NULL;

  //Set the flag to false incase the call is dropped before OEMCM_HandleCallIncom is called.
  core_call_info->bAlphaRecvd = FALSE;

  OEMCM_FREE_CALLID(call_id);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallIncom

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallIncom(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEECMCallInfo *   core_call_info;
  boolean           bSignalled = FALSE;
  AECHAR           *saved_alpha = NULL;
  int               size = 0;

  MSG_MED("OEMCM_HandleCallIncom received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);
#ifdef FEATURE_OEMOMH
  // 修正Caller ID in conversation测试的问题
  MSG_FATAL("core_call_info->call_state=0x%x",core_call_info->call_state,0,0);
  if(core_call_info->call_state == AEECM_CALL_STATE_CONV)
  {
    MSG_FATAL("IGNORE INCOM EVT IN CONV",0,0,0);
    OEMCM_FREE(notify_info);
    return;
  }
#endif
  /* Update core call info */
  if (ICMCoreObj->m_state[call_id].bSetup) {
    ICMCoreObj->m_state[call_id].bSetup = FALSE;
    bSignalled = ICMCoreObj->m_state[call_id].bSignalled;
  }

  OEMCM_ResetPrivateCallState(call_id);

  if (core_call_info->bAlphaRecvd == TRUE)
  {
     //save the alpha string
     size = WSTRLEN(core_call_info->alpha) + 1;
     saved_alpha = OEMCM_MALLOC(size*sizeof(AECHAR));
     (void)WSTRLCPY(saved_alpha, core_call_info->alpha, size);
  }

  (void) MEMSET(core_call_info, 0, sizeof(AEECMCallInfo));
  MSG_FATAL("OEMCM_HandleCallIncom %d 0x%x",call_id,event,0);
  core_call_info->call_state = AEECM_CALL_STATE_INCOM;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);
  core_call_info->direction = AEECM_CALL_DIRECTION_MT;
  core_call_info->start_time = GETUPTIMEMS();
  core_call_info->call_mode_info = (AEECMCallModeInfo)call_info->mode_info.info_type;

  if (saved_alpha != NULL)
  {
     //restore the alpha string
     (void)WSTRLCPY(core_call_info->alpha, saved_alpha, size);
     core_call_info->bAlphaRecvd = FALSE;
     OEMCM_FREE(saved_alpha);
  }
  else
  {
  OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf,
                            call_info->alpha.len,
                            core_call_info->alpha,
                            sizeof(core_call_info->alpha));
  }

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    if(call_info->num.pi != CAI_ALLOW_PI)
    {
      call_info->num.len = 0;
    }
#endif
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif

  OEMCM_CopyNumToOtherParty(&(call_info->num), core_call_info);

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {

      OEMCM_CopyCharLenToAECHAR((char *)call_info->num.subaddr.chari,
                             call_info->num.subaddr.num_char,
                             core_call_info->other_party_subaddr,
                             sizeof(core_call_info->other_party_subaddr));

    core_call_info->srv_opt =
            (AEECMServiceOption)call_info->mode_info.info.cdma_call.srv_opt;
    core_call_info->alert_ans_bypass =
            call_info->mode_info.info.cdma_call.alert_ans_bypass;
  }

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_CopyNumToAEECMNum(&call_info->num,
                          &notify_info->event_data.call.number);

  if (call_info->signal.is_signal_info_avail && !bSignalled)
    OEMCM_HandleCallSignal(call_info, AEECM_EVENT_CALL_SIGNAL);

#if (defined FEATURE_APP_MPEG4 && defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallIncomFwd

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallIncomFwd(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEECMCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallIncomFwd received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);

  /* Update core call info */

  OEMCM_ResetPrivateCallState(call_id);

  (void) MEMSET(core_call_info, 0, sizeof(AEECMCallInfo));
  MSG_FATAL("OEMCM_HandleCallIncomFwd %d 0x%x",call_id,event,0);
  core_call_info->call_state = AEECM_CALL_STATE_INCOM;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);
  core_call_info->call_mode_info = (AEECMCallModeInfo)call_info->mode_info.info_type;

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallConnect

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallConnect(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEECMCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallConnect received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  /* Update core call info */

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);

  core_call_info->call_state = AEECM_CALL_STATE_CONV;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);
  core_call_info->start_time = GETUPTIMEMS();
  core_call_info->call_mode_info = (AEECMCallModeInfo)call_info->mode_info.info_type;

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
    core_call_info->srv_opt =
            (AEECMServiceOption)call_info->mode_info.info.cdma_call.srv_opt;
    core_call_info->is_privacy =
            call_info->mode_info.info.cdma_call.is_privacy;

  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallConnectedNum

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallConnectedNum(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEECMCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallConnectedNum received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  /* Update core call info */

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);

  switch (call_info->mode_info.info_type) {
  case CM_CALL_MODE_INFO_GW_CS:
     {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
     }
     break;

  case CM_CALL_MODE_INFO_IP:
  case CM_CALL_MODE_INFO_CDMA:
     /* Copy connected num */
     if ((call_info->num.number_type == (uint8) CM_NUM_TYPE_INTERNATIONAL) &&
         (call_info->num.buf[0] != '+')) {
       core_call_info->other_party_no[0] = '+';
       OEMCM_CopyCharLenToAECHAR((char *)call_info->num.buf,
                                call_info->num.len,
                                core_call_info->other_party_no+1,
                                sizeof(core_call_info->other_party_no)-2);
     } else {
       OEMCM_CopyCharLenToAECHAR(
                          (char *)call_info->num.buf,
                          call_info->num.len,
                          core_call_info->other_party_no,
                          sizeof(core_call_info->other_party_no));
     }
     break;

  default:
     MSG_ERROR("OEMCM_HandleCallConnectedNum received Invalid Mode %d", call_info->mode_info.info_type, 0, 0);
     OEMCM_FREE(notify_info);
     return;

  }

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_CopyNumToAEECMNum(&call_info->num,
                          &notify_info->event_data.call.number);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallDormant

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallDormant(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEECMCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallDormant received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);

  if (event == AEECM_EVENT_CALL_ENTER_DORMANT) {
    core_call_info->call_state = AEECM_CALL_STATE_DORMANT;
  } else {
    core_call_info->call_state = AEECM_CALL_STATE_CONV;
  }

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallLineCtrl

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallLineCtrl(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;

  MSG_MED("OEMCM_HandleCallLineCtrl received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  if(call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA)
  {
    MSG_HIGH("Line Control Event Recvd, Polarity = %d, Toggle = %d, Reverse =%d",
                                        call_info->mode_info.info.cdma_call.line_ctrl_polarity_included,
                                        call_info->mode_info.info.cdma_call.line_ctrl_toggle,
                                        call_info->mode_info.info.cdma_call.line_ctrl_reverse);

    MSG_HIGH("%d",call_info->mode_info.info.cdma_call.line_ctrl_power_denial,0,0);

  }

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallSignal

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallSignal(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;

  MSG_MED("OEMCM_HandleCallSignal received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_CopyInSignal(&notify_info->event_data.call.signal, &call_info->signal);

  ICMCoreObj->m_state[call_info->call_id].bSignalled = TRUE;

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallSrvOpt

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallSrvOpt(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEECMCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallSrvOpt received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  if (event != AEECM_EVENT_CALL_SRV_OPT && event != AEECM_EVENT_CALL_PRIVACY) {
    MSG_ERROR("OEMCM_HandleCallSrvOpt received unexpected event: 0x%X",
                                                    event,0,0);
    return;
  }

  if (call_info->mode_info.info_type != CM_CALL_MODE_INFO_CDMA) {
    MSG_ERROR("CDMA event (0x%X) does not contain CDMA info", event, 0, 0);
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);
  core_call_info->srv_opt =
          (AEECMServiceOption)call_info->mode_info.info.cdma_call.srv_opt;
  core_call_info->is_privacy =
          call_info->mode_info.info.cdma_call.is_privacy;

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallRecallConf

DESCRIPTION:
  Send a SUPS_ERROR to application if lower layer returns an error after
  a sups command.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallRecallConf(cm_mm_call_info_s_type *call_info,
                                            AEECMEvent event)
{
#ifdef FEATURE_CCBS
#error code not present
#else
  PARAM_NOT_REF(call_info);
  PARAM_NOT_REF(event);
#endif
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallHold

DESCRIPTION:
  Handles the CM_CALL_EVENT_CALL_ON_HOLD

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallHold(cm_mm_call_info_s_type *call_info,
                                 AEECMEvent event)
{

#ifdef CM_API_IP_CALL
  cm_call_id_type                 call_id;
  AEECMCallInfo *                 core_call_info;
  AEECMNotifyInfo *               notify_info;


  MSG_MED("OEMCM_HandleCallHold received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;

  core_call_info = &(ICMCoreObj->m_call_info[call_id]);

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_IP ) {

    if ( call_info->end_params.end_params.ip_cause.cause == CM_IPAPP_CAUSE_NORMAL ){
      // successful case
      if( ( core_call_info->call_state == AEECM_CALL_STATE_CONV ) &&
          ( AEECM_IS_CALLTYPE_VOICE(core_call_info->call_type) ) ){

        core_call_info->call_state = AEECM_CALL_STATE_ONHOLD;

        OEMCM_CallNotifyInitDefault(notify_info, call_id, AEECM_EVENT_CALL_ON_HOLD);

        OEMCM_NotifyApps(notify_info);
      }
    } else {
      // SUPS ERROR

      OEMCM_CallNotifyInitDefault(notify_info, call_id, AEECM_EVENT_CALL_OPS_ERROR);

      notify_info->event_data.call.callops_error = AEESUPPSVC_MN_CALL_HOLD_REJ;

      OEMCM_NotifyApps(notify_info);
    }

  }
  OEMCM_FREE(notify_info);

#else
  PARAM_NOT_REF(call_info);
  PARAM_NOT_REF(event);
#endif
}
/*=============================================================================
FUNCTION: OEMCM_HandleCallRetrieved

DESCRIPTION:
  Handles the CM_CALL_EVENT_CALL_ON_RETRIEVED

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallRetrieved(cm_mm_call_info_s_type *call_info,
                                 AEECMEvent event)
{
#ifdef CM_API_IP_CALL
  cm_call_id_type                 call_id;
  AEECMCallInfo *                 core_call_info;
  AEECMNotifyInfo *               notify_info;


  MSG_MED("OEMCM_HandleCallRetreived received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_IP ) {

    if ( call_info->end_params.end_params.ip_cause.cause == CM_IPAPP_CAUSE_NORMAL ){
      // successful case
      if ( ( core_call_info->call_state == AEECM_CALL_STATE_ONHOLD ) &&
           ( AEECM_IS_CALLTYPE_VOICE(core_call_info->call_type) ) ) {

        core_call_info->call_state = AEECM_CALL_STATE_CONV;

        OEMCM_CallNotifyInitDefault(notify_info, call_id, AEECM_EVENT_CALL_RETRIEVED);

        OEMCM_NotifyApps(notify_info);

      }
    } else {
      // SUPS ERROR

      OEMCM_CallNotifyInitDefault(notify_info, call_id, AEECM_EVENT_CALL_OPS_ERROR);

      notify_info->event_data.call.callops_error = AEESUPPSVC_MN_CALL_RETRIEVE_REJ;

      OEMCM_NotifyApps(notify_info);
    }

  }
  OEMCM_FREE(notify_info);
#else
  PARAM_NOT_REF(call_info);
  PARAM_NOT_REF(event);
#endif
}
/*===========================================================================

                            Burst DTMF Utilities

===========================================================================*/

/*=============================================================================
FUNCTION: OEMCM_CancelBurstDTMF

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CancelBurstDTMF(ICM * pMe)
{
  int i;

  if (!pMe || !pMe->m_coreObj)
    return;

  for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) {
    if (pMe->m_coreObj->m_bdtmf[i] &&
        pMe->m_coreObj->m_bdtmf[i]->pOwner == pMe) {
      (void) ISHELL_CancelTimer(pMe->m_pIShell,
                         OEMCM_HandleBurstDTMF, pMe->m_coreObj->m_bdtmf[i]);
      OEMCM_FreeBurstDTMF(pMe->m_coreObj->m_bdtmf[i]);
    }
  }
}

/*=============================================================================
FUNCTION: OEMCM_AllocBurstDTMF

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMCM_AllocBurstDTMF(ICM *pMe, AEECMCallID callID,
                                const char *digits,
                                AEECMDTMFOnLength on_length,
                                AEECMDTMFOffLength off_length,
                                AEECMBurstDTMFPkt **bdtmf)
{
  AEECMBurstDTMFPkt * dtmf_pkt;
  boolean bInUse = FALSE;

  if (!bdtmf || !digits || !ICMCoreObj || !OEMCM_IsValidAppCallID(callID))
    return EFAILED;

  dtmf_pkt = (AEECMBurstDTMFPkt *)OEMCM_MALLOC(sizeof(AEECMBurstDTMFPkt));
  if (!dtmf_pkt)
    return ENOMEMORY;

  dtmf_pkt->dtmfs = STRDUP(digits);
  if (!dtmf_pkt->dtmfs) {
    OEMCM_FREE(dtmf_pkt);
    return ENOMEMORY;
  }

  /* See if somebody else is already doing burst on the same call */
  AEECM_MUTEX_LOCK();
  if (ICMCoreObj->m_bdtmf[OEMCM_CMCALLID(callID)])
    bInUse = TRUE;
  else
    ICMCoreObj->m_bdtmf[OEMCM_CMCALLID(callID)] = dtmf_pkt;
  AEECM_MUTEX_UNLOCK();

  if (bInUse) {
    OEMCM_FREE(dtmf_pkt->dtmfs);
    OEMCM_FREE(dtmf_pkt);
    MSG_MED("Previously started Burst DTMF not over yet", 0, 0, 0);
    return EBADSTATE;
  }

  (void) STRLCPY(dtmf_pkt->dtmfs, digits, STRLEN(digits)+1);
  dtmf_pkt->pOwner = pMe;
  dtmf_pkt->callID = callID;
  dtmf_pkt->curr = dtmf_pkt->dtmfs;
  dtmf_pkt->on_length = on_length;
  dtmf_pkt->off_length = off_length;
  dtmf_pkt->start = TRUE;

  *bdtmf = dtmf_pkt;

  return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_FreeBurstDTMF

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_FreeBurstDTMF(AEECMBurstDTMFPkt *dtmf_pkt)
{
  if (!ICMCoreObj)
    return;

  ICMCoreObj->m_bdtmf[OEMCM_CMCALLID(dtmf_pkt->callID)] = NULL;

  OEMCM_FREE(dtmf_pkt->dtmfs);
  OEMCM_FREE(dtmf_pkt);
}

/*=============================================================================
FUNCTION: OEMCM_HandleBurstDTMF

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleBurstDTMF(void *pUser)
{
  AEECMBurstDTMFPkt *bdtmf = (AEECMBurstDTMFPkt *)pUser;

  if (!bdtmf || !ICMCoreObj)
    return;

  if (bdtmf->start) {

    /* Do start continous DTMF */
    if (OEMCM_StartContDTMF(bdtmf->pOwner, bdtmf->callID, *bdtmf->curr) !=
                                                                  AEE_SUCCESS) {
      MSG_ERROR("Start continuous DTMF failed for %d, digit %c",
                        bdtmf->callID, (char)*bdtmf->curr, 0);
    } else {
      /* Do stop next time */
      bdtmf->start = FALSE;
    }

    bdtmf->curr++;

  } else {

    /* Do stop continous DTMF */
    if (OEMCM_StopContDTMF(bdtmf->pOwner, bdtmf->callID) != AEE_SUCCESS) {
      MSG_ERROR("Stop continuous DTMF failed for %d", bdtmf->callID, 0 , 0);
    }

    /* Do start next time */
    bdtmf->start = TRUE;
  }

  /* Check if we are done */
  if (bdtmf->start && *bdtmf->curr == '\0') {
    OEMCM_FreeBurstDTMF(bdtmf);
    return;
  }

  /* Schedule next timer */
  if (ISHELL_SetTimer(bdtmf->pOwner->m_pIShell,
                   bdtmf->start ?
                   OEMCM_DTMFOffToMS(bdtmf->off_length) :
                   OEMCM_DTMFOnToMS(bdtmf->on_length),
                   OEMCM_HandleBurstDTMF,
                   (void *)bdtmf) != AEE_SUCCESS) {
    OEMCM_FreeBurstDTMF(bdtmf);
    MSG_ERROR("Failed to start timer for Burst DTMF", 0, 0, 0);
  }
}

/*=============================================================================
FUNCTION: OEMCM_DTMFOnToMS

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int32 OEMCM_DTMFOnToMS(AEECMDTMFOnLength on_length)
{
  switch (on_length) {
    case AEECM_DTMF_ON_95:
      return 95;

    case AEECM_DTMF_ON_150:
    case AEECM_DTMF_ON_200:
    case AEECM_DTMF_ON_250:
    case AEECM_DTMF_ON_300:
    case AEECM_DTMF_ON_350:
      return ((int32)on_length+2)*50;

    default:
      return 95;
  }
}

/*=============================================================================
FUNCTION: OEMCM_DTMFOffToMS

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int32 OEMCM_DTMFOffToMS(AEECMDTMFOffLength off_length)
{
  switch(off_length) {
    case AEECM_DTMF_OFF_60:
      return 60;

    case AEECM_DTMF_OFF_100:
      return 100;

    case AEECM_DTMF_OFF_150:
      return 150;

    case AEECM_DTMF_OFF_200:
      return 200;

    default:
      return 60;
  }
}

/*===========================================================================

                    Event Callback Utilities

===========================================================================*/

/*=============================================================================
FUNCTION: OEMCM_CancelAllEventCB

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CancelAllEventCB(ICMCore *pMe)
{
  AEECMEventCBS *curr, *prev;

  if (!pMe)
    return;

  AEECM_MUTEX_LOCK();

  curr = pMe->m_event_cbs;

  while(curr) {
    prev = curr;
    curr = curr->pNext;

    /* Cancel and Remove previous node */
    CALLBACK_Cancel(&prev->cb);

    if (--prev->nRef > 0)
      continue;

    OEMCM_REMOVE_NODE(pMe->m_event_cbs, prev);

    /*lint -esym(613, prev) */
    /*lint -save -e424 */
    if (prev->data)
      mem_free(&tmc_heap, prev->data);

    mem_free(&tmc_heap, prev);
    /*lint -restore */
  }

  AEECM_MUTEX_UNLOCK();

}

/*=============================================================================
FUNCTION: OEMCM_AddRefEventCBS

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_AddRefEventCBS(AEECMEventCBS *pcb)
{
  ++pcb->nRef;
}

/*=============================================================================
FUNCTION: OEMCM_AllocEventCBS

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMEventCBS * OEMCM_AllocEventCBS(ICMCore *pMe, uint32 data_size)
{
  AEECMEventCBS *pcb = NULL;

  if (!pMe)
    return NULL;

  pcb = (AEECMEventCBS *) mem_malloc(&tmc_heap, sizeof(AEECMEventCBS));
  if (!pcb)
  {
    MSG_LOW("OEMCM DEBUG::OEMCM Failed to allocate memory in OEMCM", 0, 0, 0);
    return NULL;
  }


  (void) memset(pcb, 0, sizeof(AEECMEventCBS));
  MSG_LOW("OEMCM DEBUG::OEMCM Memory Allocation Successful for pcb = 0x%X", pcb, 0, 0);
  if (data_size) {
    pcb->data = mem_malloc(&tmc_heap, data_size);
    if (!pcb->data) {
      /*lint -save -e424 */
      MSG_LOW("OEMCM DEBUG::OEMCM Failed to allocate memory for pcb->data freeing for pcb = 0x%X ",pcb, 0, 0);
      mem_free(&tmc_heap, pcb);
      /*lint -restore */
      return NULL;
    }
    (void) memset(pcb->data, 0, data_size);
    MSG_LOW("OEMCM DEBUG::OEMCM Memory Allocation Successful for pcb->data = 0x%X", pcb->data, 0, 0);
  }

  pcb->nRef = 1;

  AEECM_MUTEX_LOCK();

  OEMCM_ADD_NODE(pMe->m_event_cbs, pcb);

  AEECM_MUTEX_UNLOCK();

  return pcb;
}

/*=============================================================================
FUNCTION: OEMCM_FreeEventCBS

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_FreeEventCBS(AEECMEventCBS *pcb)
{
  ICMCore *pMe = ICMCoreObj;

  if (!pcb || !pMe)
    return;


  if (--pcb->nRef > 0)
  {
    MSG_LOW("OEMCM DEBUG::OEMCM More Refs nRef = %d present cant free yet", pcb->nRef, 0, 0);
    return;
  }

  CALLBACK_Cancel(&pcb->cb);

  AEECM_MUTEX_LOCK();

  OEMCM_REMOVE_NODE(pMe->m_event_cbs, pcb);

  AEECM_MUTEX_UNLOCK();

  /*lint -save -e424 */
  if (pcb->data)
  {
    MSG_LOW("OEMCM DEBUG::OEMCM Freeing memory for 0x%X", pcb->data, 0, 0);
    mem_free(&tmc_heap, pcb->data);

  }

  MSG_LOW("OEMCM DEBUG::OEMCM Freeing memory for 0x%X", pcb, 0, 0);
  mem_free(&tmc_heap, pcb);
  /*lint -restore */
}


/*===========================================================================

                    Error Callback Utilities

===========================================================================*/

/*=============================================================================
FUNCTION: OEMCM_AllocErrCBS

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMErrCBS *OEMCM_AllocErrCBS(ICM *pMe)
{
  AEECMErrCBS *pcb = NULL;

  if (!pMe)
    return NULL;

  pcb = (AEECMErrCBS *) OEMCM_MALLOC(sizeof(AEECMErrCBS));

  if (!pcb)
    return NULL;

  if (AEEObjectMgr_Register(pcb, &(pcb->hObject))) {
    OEMCM_FREE(pcb);
    return NULL;
  }

  pcb->pOwner = pMe;

  AEECM_MUTEX_LOCK();

  OEMCM_ADD_NODE(pMe->m_err_cbs, pcb);

  AEECM_MUTEX_UNLOCK();

  return pcb;
}

/*=============================================================================
FUNCTION: OEMCM_FreeErrCBS

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_FreeErrCBS(ICM * pMe, AEECMErrCBS *pcb)
{
  if (!pMe || !pcb)
    return;

  if (pcb->hObject) {
    (void) AEEObjectMgr_Unregister(pcb->hObject);
    pcb->hObject = NULL;
  }

  AEECM_MUTEX_LOCK();

  OEMCM_REMOVE_NODE(pMe->m_err_cbs, pcb);

  AEECM_MUTEX_UNLOCK();

  OEMCM_FREE(pcb);
}

/*=============================================================================
FUNCTION: OEMCM_AppCancelGetNW

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_AppCancelGetNW(AEECallback *pcb)
{
  AEECMGetNWCBS *err_cb;

  if (!pcb || !ICMCoreObj)
    return;

  err_cb = (AEECMGetNWCBS *) pcb->pCancelData;

  pcb->pfnCancel = NULL;
  pcb->pCancelData = NULL;

  if (err_cb->hObject) {
    (void) AEEObjectMgr_Unregister(err_cb->hObject);
    err_cb->hObject = NULL;
  }

  if (!cm_ph_cmd_terminate_get_networks(OEMCM_DMSSPhErrCB,
                                          NULL,
                                          gCMID)) {
    MSG_ERROR("cm_ph_cmd_terminate_get_networks failed", 0, 0, 0);
  }


  err_cb->bInUse = FALSE;
}

/*=============================================================================
FUNCTION: OEMCM_AppCancelErrCB

DESCRIPTION:
  Used by Applications to cancel pending status callbacks

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_AppCancelErrCB(AEECallback *pcb)
{
  AEECMErrCBS *err_cb;

  if (!pcb || !ICMCoreObj)
    return;

  err_cb = (AEECMErrCBS *) pcb->pCancelData;

  pcb->pfnCancel = NULL;
  pcb->pCancelData = NULL;

  OEMCM_FreeErrCBS(err_cb->pOwner, err_cb);

}

/*=============================================================================
FUNCTION: OEMCM_CancelAllErrCB

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CancelAllErrCB(ICM *pMe)
{
  AEECMErrCBS *curr, *prev;

  if (!pMe || !pMe->m_coreObj)
    return;

  AEECM_MUTEX_LOCK();

  curr = pMe->m_err_cbs;

  while(curr) {
    prev = curr;
    curr = curr->pNext;

    prev->pcb->pfnCancel = NULL;
    prev->pcb->pCancelData = NULL;

    if (prev->hObject) {
      (void) AEEObjectMgr_Unregister(prev->hObject);
      prev->hObject = NULL;
    }

   OEMCM_REMOVE_NODE(pMe->m_err_cbs, prev);

   OEMCM_FREE(prev);

  }

  AEECM_MUTEX_UNLOCK();

  if (pMe->m_coreObj->m_getnw_cb.bInUse &&
      pMe->m_coreObj->m_getnw_cb.pOwner == pMe)
    CALLBACK_Cancel(pMe->m_coreObj->m_getnw_cb.pcb);
}

/*===========================================================================

                    ICMCore functions

===========================================================================*/


/*=============================================================================
FUNCTION: OEMCMCore_New

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static ICMCore * OEMCMCore_New()
{
  uint8 i;

  if (ICMCoreObj) {

    (void) OEMCMCore_AddRef();

  } else {

    /* The OEMCM_MALLOC gives us zero-initialized memory, so everything
     * starts out as zero/FALSE.
     */
    ICMCoreObj = (ICMCore *)OEMCM_MALLOC(sizeof(ICMCore));
    if (!ICMCoreObj)
       return NULL;

    ICMCoreObj->m_uRefs = 1;

    OEMCM_INIT_CALLIDS();

    OEMCM_InitDefaultCallParams(ICMCoreObj);

    ICMCoreObj->m_event_cbs = NULL;

    if(!OEMCM_InitCallManager()) {
      OEMCM_FREE(ICMCoreObj);
      ICMCoreObj = NULL;
      return NULL;
    }

    for (i = 0; i < CM_CALL_ID_MAX; i++)
      (void) cm_mm_call_cmd_get_call_info(NULL, NULL, gCMID, i);

    (void) cm_ph_cmd_get_ph_info(NULL, NULL, gCMID);

    (void) cm_ss_cmd_get_ss_info(NULL, NULL, gCMID);

#if defined (FEATURE_ACM)
    {
      ICMCoreObj->m_pAcmInfo = (ICM_ACMInfo *) OEMCM_MALLOC (sizeof (ICM_ACMInfo));
      OEMCM_ACMCreateInstance(ICMCoreObj->m_pAcmInfo);
    }
#endif // FEATURE_ACM
  }

  return ICMCoreObj;
}

/*=============================================================================
FUNCTION: OEMCMCore_AddRef

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMCMCore_AddRef()
{
  ICMCore *pMe = ICMCoreObj;

  if (pMe)
    return ++pMe->m_uRefs;
  else
    return 0;
}

/*=============================================================================
FUNCTION: OEMCMCore_Release

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMCMCore_Release()
{
  ICMCore *pMe = ICMCoreObj;

  if (!pMe)
    return 0;

  if (--pMe->m_uRefs != 0) {
    return pMe->m_uRefs;
  }

  OEMCM_ReleaseCallManager();

  OEMCM_CancelAllEventCB(pMe);

#if defined (FEATURE_ACM)
  OEMCM_FREE (ICMCoreObj->m_pAcmInfo);
#endif // FEATURE_ACM

  // Ref count is zero.  Release memory
  OEMCM_FREE(pMe);
  ICMCoreObj = NULL;
  return (0);
}

/*=============================================================================
FUNCTION: OEMCMCore_FreeOnAEEExit

DESCRIPTION:
  This function is called by ICMNotifier if it exits in response to BREW
  exiting. Hopefully the object reference is zero by now and somebody
  has already freed the core object. Just in case that has not happened, we do
  it here.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCMCore_FreeOnAEEExit()
{
  if (ICMCoreObj) {
    OEMCM_ReleaseCallManager();
    OEMCM_CancelAllEventCB(ICMCoreObj);
    OEMCM_FREE(ICMCoreObj);
    ICMCoreObj = NULL;
  }
}

/*=============================================================================
FUNCTION: OEMCMNotifier_FreeOnAEEExit

DESCRIPTION:
  This function is called in response to BREW exiting. Hopefully the notifier
  object reference is zero by now and somebody has already freed it.  Just in
  case that has not happened, we do it here.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCMNotifier_FreeOnAEEExit(void *pUser)
{
  PARAM_NOT_REF(pUser);

  if (ICMNotifierObj) {
    if (ICMNotifierObj->m_coreObj) {
      OEMCMCore_FreeOnAEEExit();
      ICMNotifierObj->m_coreObj = NULL;
    }

    FREE(ICMNotifierObj);
    ICMNotifierObj = NULL;
  }
}

/*===========================================================================

FUNCTION: OEMCMModel_AEEExitNotify

DESCRIPTION
  Deallocates the model object and associated resources when BREW exits.

DEPENDENCIES
  none

RETURN VALUE
  none
===========================================================================*/
static void OEMCMModel_AEEExitNotify(void * pUser)
{
  PARAM_NOT_REF(pUser);

  MSG_HIGH("Exit ICM_MODEL in AEEExitNotify, obj: 0x%x",
           ICMModelObj, 0, 0);
  if (NULL != ICMModelObj) {
    if (ICMModelObj->m_pcbExit) {
      sys_free(ICMModelObj->m_pcbExit);
      ICMModelObj->m_pcbExit = NULL;
    }
    if (NULL != ICMModelObj->m_coreObj ) {
      (void) OEMCMCore_Release();
      ICMModelObj->m_coreObj = NULL;
    }
    SysModelBase_Dtor(&ICMModelObj->base);
    sys_free(ICMModelObj);
    ICMModelObj = NULL;
  }
}

/*=============================================================================

                  DMSS Call manager Init and Release

=============================================================================*/

/*=============================================================================
FUNCTION: OEMCM_InitCallManager

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static boolean OEMCM_InitCallManager()
{
  if (!gCMID) {
    /* Init call manager */
    if (cm_client_init(CM_CLIENT_TYPE_BREW,&gCMID) != CM_CLIENT_OK
        || !gCMID) {
      ERR("Unable to Initialize DMSS Call Manager", 0, 0, 0);
      return FALSE;
    }

    /* Register callbacks */
    (void) cm_mm_client_call_reg(gCMID, OEMCM_DMSSCallEventCB, CM_CLIENT_EVENT_REG,
                          CM_CALL_EVENT_ORIG, (cm_call_event_e_type)(CM_CALL_EVENT_MAX-1),
                          OEMCM_DMSSOtherCallErrCB);
    (void) cm_client_ph_reg(gCMID, OEMCM_DMSSPhEventCB, CM_CLIENT_EVENT_REG,
                     CM_PH_EVENT_OPRT_MODE,  (cm_ph_event_e_type)(CM_PH_EVENT_MAX -1),
                     NULL);
    (void) cm_mm_client_ss_reg(gCMID, OEMCM_DMSSSSEventCB, CM_CLIENT_EVENT_REG,
                        CM_SS_EVENT_SRV_CHANGED, (cm_ss_event_e_type)(CM_SS_EVENT_MAX-1),
                        NULL);
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif

    (void) cm_mm_client_inband_reg(gCMID, OEMCM_DMSSInbandEventCB,
                            CM_CLIENT_EVENT_REG,
                            CM_INBAND_EVENT_REV_BURST_DTMF,
                            (cm_inband_event_e_type)(CM_INBAND_EVENT_MAX-1),
                            NULL);

#ifdef FEATURE_MBMS
    cm_client_mbms_reg(gCMID,
                       OEMCM_DMSSMBMSEventCB,
                       CM_CLIENT_EVENT_REG,
                       CM_MBMS_EVENT_GENERIC_CMD,
                       CM_MBMS_EVENT_GENERIC_RPT,
                       NULL);
#endif //#ifdef FEATURE_MBMS

  }

  if (!gbCMAct) {
    /* Activate callbacks */
    gbCMAct = (boolean)(cm_client_act(gCMID) == CM_CLIENT_OK);
    if (!gbCMAct) {
      ERR("Unable to Activate DMSS Call Manager", 0, 0, 0);
      return FALSE;
    }
  }

  return TRUE;
}

/*=============================================================================
FUNCTION: OEMCM_ReleaseCallManager

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_ReleaseCallManager()
{
  if (gbCMAct) {
    if (cm_client_deact(gCMID) != CM_CLIENT_OK) {
      ERR("Unable to Deactivate DMSS Call Manager", 0, 0, 0);
      return;
    }

    gbCMAct = FALSE;
  }

  if (gCMID) {
    if (cm_client_release(gCMID) != CM_CLIENT_OK) {
      ERR("Unable to Release DMSS Call Manager", 0, 0, 0);
      return;
    }

    gCMID = 0;
  }

  return;
}

/*=============================================================================

                       Other utility functions

=============================================================================*/

/*=============================================================================
FUNCTION: OEMCM_CMCallCmdOrig

DESCRIPTION:
  Wrapper for cm_mm_call_cmd_orig that takes care of allocating the error
  callbacks and updating internal states.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMCM_CMCallCmdOrig(
                             ICM                         *pMe,
                             cm_call_type_e_type          call_type,
                             cm_srv_type_e_type           srv_type,
                             const cm_num_s_type         *calling_number,
                             const cm_num_s_type         *called_number,
                             const cm_orig_alpha_s_type  *alpha_buf,
                             cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
                             cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
                             cm_call_id_type             *call_id_ptr)
{
  AEECMEventCBS         *event_cb;
  AEECMCallErrCBPkt     *err_cb_pkt;

  boolean               isTTY = FALSE;

  if (!pMe || !pMe->m_coreObj || !calling_number || !called_number ||
      !alpha_buf || (!cdma_orig_params_ptr && !gw_cs_orig_params_ptr) ||
      !call_id_ptr) {
    return EBADPARM;
  }

  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, sizeof(AEECMCallErrCBPkt));

  if (!event_cb) {
    MSG_ERROR("OEMCM_CMCallCmdOrig: Failed to allocate event callback",
               0, 0, 0);
    return EFAILED;
  }

  OEMCM_AddRefEventCBS(event_cb);

  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;

#ifdef FEATURE_GSTK_FDN_CC_SUPPORT
  if (!cm_mm_call_cmd_orig_exten2(OEMCM_DMSSCallErrCB, event_cb, gCMID,
                                  call_type,
                                  srv_type,
                                  calling_number,
                                  called_number,
                                  alpha_buf,
                                  cdma_orig_params_ptr,
                                  gw_cs_orig_params_ptr,
                                  NULL,
                                  CM_CALL_ID_INVALID,
                                  &gCMGenericParams,
                                  (cm_call_id_type *)&(err_cb_pkt->call_id)))
#else
  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_orig(OEMCM_DMSSCallErrCB, event_cb, gCMID,
                           call_type,
                           srv_type,
                           calling_number,
                           called_number,
                           alpha_buf,
                           cdma_orig_params_ptr,
                           gw_cs_orig_params_ptr,
                           NULL,
                           (cm_call_id_type *)&(err_cb_pkt->call_id)))
#endif
  {
    OEMCM_FreeEventCBS(event_cb);
    return EFAILED;
  }
#ifdef FEATURE_GSTK_FDN_CC_SUPPORT
  gCMGenericParams.is_fdn_to_check = TRUE;
#endif

  /* If call id is invalid, return error */
  if (err_cb_pkt->call_id >= CM_CALL_ID_MAX) {
    OEMCM_FreeEventCBS(event_cb);
    return EFAILED;
  }

  *call_id_ptr = err_cb_pkt->call_id;

  OEMCM_FreeEventCBS(event_cb);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (FEATURE_GSM) ||  (FEATURE_WCDMA) */

  /* Update state */
  OEMCM_HandleOriginateCall(pMe, call_type, called_number,
                            alpha_buf, *call_id_ptr, isTTY);

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_RefreshDataCallStats

DESCRIPTION:
  Update Data Call Statistics in call info structure of the specifed call.
  Called before notification of a call event or when user calls GetCallInfo().

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_RefreshDataCallStats(ICMCore *pMe, AEECMCallID callID)
{
  AEECMDataCallStats *data_stats;
  AEECMCallModeInfo call_mode_info;
  cm_call_id_type cm_call_id;

  if (!pMe || !OEMCM_IsValidAppCallID(callID))
    return;

  cm_call_id = OEMCM_CMCALLID(callID);
  data_stats = &(pMe->m_call_info[cm_call_id].data_stats);

#ifndef FEATURE_DS
  (void) MEMSET(data_stats, 0, sizeof(AEECMDataCallStats));
  return;
#else

  /* If not data call, nothing to do */
  if ((OEMCM_GetCallType(cm_call_id) != AEECM_CALL_TYPE_PS_DATA) &&
      (OEMCM_GetCallType(cm_call_id) != AEECM_CALL_TYPE_CS_DATA))
  {
    (void) MEMSET(data_stats, 0, sizeof(AEECMDataCallStats));
    return;
  }

  call_mode_info = pMe->m_call_info[cm_call_id].call_mode_info;

  /*Call is not connected.*/
  if (!AEECM_IS_CALLSTATE_CONNECTED(pMe->m_call_info[cm_call_id].call_state))
  {
    (void) MEMSET(data_stats, 0, sizeof(AEECMDataCallStats));
    return;
  }

  if (AEECM_IS_CALL_MODE_GW(call_mode_info)) {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
  } else if (AEECM_IS_CALL_MODE_CDMA(call_mode_info)) {
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    /* Update CDMA data call stats */
    OEMCM_RefreshCDMADataCallStats(data_stats, callID);
#endif
  }
#endif /* FEATURE_DS */
}

#ifdef FEATURE_DS
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* GSM/WCDMA */

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
/*=============================================================================
FUNCTION: OEMCM_RefreshCDMADataCallStats

DESCRIPTION:
  Update Data Call Statistics in call info structure of the specifed CDMA call.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_RefreshCDMADataCallStats(AEECMDataCallStats *data_stats,
                                         AEECMCallID callID)
{
  AEECMCallInfo *call_info;
#if (defined (FEATURE_DS_IS2000) || defined (FEATURE_HDR))
  uint32 data_rate;
#ifndef FEATURE_DATA_PS_CMI_REARCH
  dsrlp_stats_info_type *dsrlp_stats_ptr;
#endif
  boolean is_double;
  enc_sch_radio_config_type sch_rc;
  dec_rc_type dec_sch_rc;
  dec_sch_rate_type dec_sch_rate_index;
  enc_sch_rate_type enc_sch_rate_index;
  int i;
#endif /* FEATURE_DS_IS2000 || FEATURE_HDR */

  if (!OEMCM_IsValidAppCallID(callID) || !ICMCoreObj)
    return;

  call_info = &ICMCoreObj->m_call_info[OEMCM_CMCALLID(callID)];

  /* Update total byte count */
#ifdef FEATURE_DS_IS2000
  if ((call_info->srv_opt == AEECM_SO_PPP_PKT_DATA_3G) ||
      (call_info->srv_opt == AEECM_SO_MDR_PKT_DATA_FRS2_RRS1) ||
      (call_info->srv_opt == AEECM_SO_MDR_PKT_DATA_FRS2_RRS2)) {
#ifndef FEATURE_DATA_PS_CMI_REARCH  // Remoted dsrlp_get_stats() removed
    /* Set dsrlp_stats_ptr to NULL so RPC will allocate a new buffer.
     * Without this, RPC will try to reuse a buffer that it has
     * already reclaimed.
     */
    dsrlp_stats_ptr = NULL;
    if(dsrlp_get_stats(DSRLP_FIXED_SR_ID_INDEX, &dsrlp_stats_ptr)) {
      data_stats->rx_total_bytes = (uint32)dsrlp_stats_ptr->rx_total_bytes;
      data_stats->tx_total_bytes = (uint32)dsrlp_stats_ptr->tx_total_bytes;
    }
#else
    data_stats->rx_total_bytes = 0;
    data_stats->tx_total_bytes = 0;
#endif // FEATURE_DATA_PS_CMI_REARCH
  }
  else
#endif /* FEATURE_DS_IS2000 */
  {
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
    {
      /* Required info not available from RLP for IS95 */
      data_stats->rx_total_bytes = 0;
      data_stats->tx_total_bytes = 0;
    }
  }

  /* Update max rate */

#ifdef FEATURE_IS95B_MDR
  /* TODO */
#endif

#ifdef FEATURE_DS_IS2000
  if ((call_info->srv_opt == AEECM_SO_PPP_PKT_DATA_3G) ||
      (call_info->srv_opt == AEECM_SO_MDR_PKT_DATA_FRS2_RRS1) ||
      (call_info->srv_opt == AEECM_SO_MDR_PKT_DATA_FRS2_RRS2)) {
    /* Get transmit supplemental channel rate and calculate the data rate.
    */
    data_rate = 0;
#ifdef FEATURE_IS2000_P2
    if (txc_dcch_is_on()) {
      switch (txc_get_dcch_rate_set()) {
        case ENC_RATE_9600: data_rate = OEMCM_DS_DATA_RATE_9_6; break;
        case ENC_RATE_14400: data_rate = OEMCM_DS_DATA_RATE_14_4; break;
        default:
          break;
      }
    }
    else
#endif /* FEATURE_IS2000_P2 */
    {
      switch (txc_get_fch_rate_set()) {
        case ENC_RATE_9600: data_rate = OEMCM_DS_DATA_RATE_9_6; break;
        case ENC_RATE_14400: data_rate = OEMCM_DS_DATA_RATE_14_4; break;
        default:
          break;
      }
    }

    if (txc_get_sch_info(&sch_rc, &enc_sch_rate_index, &is_double)) {

      /* Check for single/double sized PDUs and use it for the data rate
       * calculation. The formula is (sch_rate*single_double * 9.6) + 9.6
       * (for the fundamantal channel).
       */
      for (i=0; i < MAX_NO_SCH_RATES; i++) {
        if (oemcm_enc_sch_rate_table[i][0] == enc_sch_rate_index) {
          if(sch_rc == ENC_SCH_RC_3)
          {
            data_rate += ((oemcm_enc_sch_rate_table[i][1] ) * OEMCM_DS_DATA_RATE_9_6);
          }
          else if(sch_rc == ENC_SCH_RC_4)
          {
            data_rate += ((oemcm_enc_sch_rate_table[i][1] ) * OEMCM_DS_DATA_RATE_14_4);
          }
          else
          {
              MSG_HIGH("Unknown rate set %d", sch_rc, 0, 0);
          }
          break;
        }
      }
    }

    data_stats->tx_max_rate = data_rate;

    /* Get Receive supplemental channel rate and calculate the data rate.
    */
    data_rate = 0;

#ifdef FEATURE_IS2000_P2
    if (rxc_dcch_is_on()) {
      switch (rxc_get_dcch_rate_set()) {
        case DEC_RATE_9600: data_rate = OEMCM_DS_DATA_RATE_9_6; break;
        case DEC_RATE_14400: data_rate = OEMCM_DS_DATA_RATE_14_4; break;
        default:
          break;
      }
    }
    else
#endif /* FEATURE_IS2000_P2 */
    {
      switch (rxc_get_fch_rate_set()) {
        case DEC_RATE_9600: data_rate = OEMCM_DS_DATA_RATE_9_6; break;
        case DEC_RATE_14400: data_rate = OEMCM_DS_DATA_RATE_14_4; break;
        default:
          break;
      }
    }

    if (rxc_get_sch_info (&dec_sch_rc, &dec_sch_rate_index, &is_double)) {
      /* Check for single/double sized PDUs and use it for the data rate
       * calculation. The formula is (sch_rate*single_double * 9.6) + 9.6
       * (for the fundamantal channel).
       */
      for (i=0; i < MAX_NO_SCH_RATES; i++) {
        if (oemcm_dec_sch_rate_table[i][0] == dec_sch_rate_index) {
          if((dec_sch_rc == DEC_RC3) || (dec_sch_rc == DEC_RC4))
          {
            data_rate += ((oemcm_dec_sch_rate_table[i][1]) * OEMCM_DS_DATA_RATE_9_6);
          }
          else if(dec_sch_rc == DEC_RC5)
          {
            data_rate += ((oemcm_dec_sch_rate_table[i][1] ) * OEMCM_DS_DATA_RATE_14_4);
          }
          else
          {
            MSG_HIGH("Unknown rate set", 0,0,0);
          }
          break;
        }
      }
    }

    data_stats->rx_max_rate = data_rate;

  }
#endif /* FEATURE_DS_IS2000 */

#ifdef FEATURE_HDR
#error code not present
#endif

}
#endif /* CDMA */
#endif /* FEATURE_DS */

/*=============================================================================
FUNCTION: OEMCM_HandleDataCallEnd

DESCRIPTION:
  Copy data call statistics from CM call info structure and update NV variables
  at the end of a data call.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleDataCallEnd(AEECMDataCallStats *data_stats,
                                    cm_mm_call_info_s_type *call_info)
{
  if (!data_stats || !call_info)
    return;

  if ((OEMCM_GetCallType(call_info->call_id) != AEECM_CALL_TYPE_PS_DATA) &&
      (OEMCM_GetCallType(call_info->call_id) != AEECM_CALL_TYPE_CS_DATA)) {
    data_stats->tx_total_bytes = 0;
    data_stats->rx_total_bytes = 0;
    return;
  }

  /* Copy data call stats from CM call_info */
  data_stats->tx_total_bytes = call_info->bytes_transmitted;
  data_stats->rx_total_bytes = call_info->bytes_received;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
}

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* GSM || WCDMA */

/*=============================================================================
FUNCTION: OEMCM_HandleCommandError

DESCRIPTION:
  Handle state changes due to command error.

Note: Should not make call id invalid on an origination failure since Apps
      might want to call ICM helper functions on the call from within the
      notification function.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCommandError(cm_call_id_type call_id,
                        cm_call_cmd_e_type call_cmd,
                        cm_call_cmd_err_e_type call_cmd_err)
{

  if (!ICMCoreObj)
    return;

  if (call_cmd_err != CM_CALL_CMD_ERR_NOERR) {
    switch(call_cmd) {
      case CM_CALL_CMD_ANSWER:
        ICMCoreObj->m_state[call_id].bAnswered = FALSE;
        break;

      case CM_CALL_CMD_END:
        ICMCoreObj->m_state[call_id].bEnded = FALSE;
        break;

      default:
        break;
    }
  }
}

/*=============================================================================
FUNCTION: OEMCM_HandleOriginateCall

DESCRIPTION:
  After originating a call and before receiving the actual ORIG event from
  CM, ICM call info should contain information used to originate the call. This
  information should be accessible to applications too. This function updates
  the call info structure and updates internal variables for this purpose.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleOriginateCall(ICM *pMe, cm_call_type_e_type call_type,
                                     const cm_num_s_type *called_number,
                                     const cm_orig_alpha_s_type *alpha,
                                     cm_call_id_type call_id,
                                     boolean isTTY)
{
  AEECMCallInfo * core_call_info;

  if (!pMe || !pMe->m_coreObj)
    return;

  pMe->m_coreObj->m_call_owners[call_id] = pMe;

  core_call_info = &pMe->m_coreObj->m_call_info[call_id];

  (void) MEMSET(core_call_info, 0, sizeof(AEECMCallInfo));

  core_call_info->call_state = AEECM_CALL_STATE_ORIG;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_type);

#ifdef FEATURE_TTY
  core_call_info->bTTYcall = isTTY;
#else /* FEATURE_TTY */
  core_call_info->bTTYcall = FALSE;
#endif /* FEATURE_TTY */

  OEMCM_CopyNumToOtherParty(called_number, core_call_info);

  OEMCM_CopyCharLenToAECHAR((char *)alpha->buf, alpha->len,
                            core_call_info->alpha,
                            sizeof(core_call_info->alpha));

  OEMCM_ResetPrivateCallState(call_id);
}

/*=============================================================================
FUNCTION: OEMCM_IsEmergencyNumber

DESCRIPTION:
  Check if the dialed digit is an emergency number. This is a wrapper around
  pbm_emergency_number.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static boolean OEMCM_IsEmergencyNumber(const AECHAR *number, uint8 *ecc_category)
{
  char called[CM_MAX_NUMBER_CHARS+1] = "";

  if (!number || !ecc_category)
    return FALSE;

  (void) WSTRTOSTR(number, (char *)called, sizeof(called));
#ifdef FEATURE_UI_PBM
  return pbm_emergency_number_cat(called, (uint8)STRLEN((char *)called), ecc_category);
#else
  return OEM_IsEmergency_Number(called, STRLEN(called));
#endif
}

/*=============================================================================
FUNCTION: OEMCM_SendEmergencyFlash

DESCRIPTION:
  In CDMA, if we originate a call when another call is up, we usually send a
  simple flash. If the call we are trying to make is an emergency call, we
  should instead go ahead and originate an emergency call.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMCM_SendEmergencyFlash(ICM *pMe, const AECHAR *num,
                                    const AECHAR *alpha, AEECMCallID *pcallID,
                                    uint8 ecc_category)
{
/*lint -save -esym(715,ecc_category) suppress not referenced */
  AECHAR                       calling[] = {'\0'};
  cm_num_s_type                calling_number;
  cm_num_s_type                called_number;
  cm_orig_alpha_s_type         cm_alpha;
  cm_cdma_orig_params_s_type   orig_params;
  cm_call_id_type              call_id;
  int                          ret;

  if (!pMe || !pcallID)
    return EBADPARM;

  orig_params.activate_code  = CM_OTASP_ACT_CODE_NONE;
  orig_params.drs_bit        = TRUE;
  orig_params.sr_id_included = FALSE;
  orig_params.qos_parms_incl = FALSE;

  OEMCM_FillCallNum(&called_number, &calling_number, &cm_alpha,
                    num, calling, alpha);

  //This doesn't currently support UMTS calls, so emergency category propogation
  //is not necessary or possible
  ret = OEMCM_CMCallCmdOrig(pMe, CM_CALL_TYPE_EMERGENCY,
                             CM_SRV_TYPE_AUTOMATIC,
                             &calling_number,
                             &called_number,
                             &cm_alpha,
                             &orig_params,
                             NULL,
                             &call_id);

  if (ret == AEE_SUCCESS)
    *pcallID = OEMCM_AEECALLID(call_id);

  return ret;
}
/*lint -restore*/

/*=============================================================================
FUNCTION: OEMCM_SendFlash

DESCRIPTION:
  Send CDMA Flash.


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMCM_SendFlash(AECHAR const *number, cm_call_id_type flash_call_id)
{
  char                        called[CM_MAX_NUMBER_CHARS+1];

  if (!ICMCoreObj)
    return EFAILED;

  if (number)
    (void) WSTRTOSTR(number, called, sizeof(called));
  else
    called[0] = '\0';

  return OEMCM_SendFlash_Int( called, flash_call_id );
}

/*=============================================================================
FUNCTION: OEMCM_SendFlash_Int

DESCRIPTION:
  Send CDMA Flash.


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMCM_SendFlash_Int( const char *called, cm_call_id_type flash_call_id )
{
  AEECMEventCBS *             event_cb;
  cm_call_sups_params_s_type  sups_params;
  AEECMCallErrCBPkt *         err_cb_pkt;
  cm_num_s_type               called_number;

  OEMCM_InitNum(&called_number, called);

  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, sizeof(AEECMCallErrCBPkt));

  if (!event_cb) {
    MSG_ERROR("Failed to allocate event callback",
               0, 0, 0);
    return EFAILED;
  }

  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
  err_cb_pkt->call_id = flash_call_id;

  sups_params.call_id = flash_call_id;
  (void) MEMCPY(&sups_params.sups_number, &called_number,
                sizeof(cm_num_s_type));
  MSG_FATAL("OEMCM_SendFlash_Int",0,0,0);
  if (!cm_mm_call_cmd_sups(OEMCM_DMSSCallErrCB, event_cb, gCMID,
                           CM_CALL_SUPS_TYPE_SIMPLE_FLASH,
                           &sups_params)) {
    MSG_ERROR("Call to cm_mm_call_cmd_sups failed", 0, 0, 0);
    return EFAILED;
  }

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_InitDefaultCallParams

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_InitDefaultCallParams(ICMCore *pMe)
{
  cm_cdma_orig_params_s_type *cdma;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif

  if (pMe) {
    cdma = &(pMe->def_cdma_orig_params);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
  } else {
    return;
  }

  cdma->srv_opt = 0;
  cdma->activate_code = CM_OTASP_ACT_CODE_NONE;
  cdma->drs_bit = TRUE;
  cdma->sr_id_included = FALSE;
  cdma->sr_id = 0;
  cdma->qos_parms_incl = FALSE;
  cdma->qos_parms_len = 0;
  cdma->last_act_data_net = SYS_SYS_MODE_NO_SRV;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

/*=============================================================================
FUNCTION: OEMCM_Malloc

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void * OEMCM_Malloc(uint32 size, int line)
{
  void *p;

  if (!size)
    return NULL;

/* Mask of various lint errors related to the OEM_Malloc macro */
//lint -save -esym(718, AEEHeap_Realloc) -esym(746, AEEHeap_Realloc) -e64
  if((p = OEM_Malloc(size)) == NULL) {
//lint -restore
    MSG_ERROR("Failed to allocate memory %d in line %d, total size %d",
               size, line, ICMCoreObj ? ICMCoreObj->m_malloc_cnt : 0);
    return NULL;
  }

  (void) MEMSET(p, 0, size);

#ifdef FEATURE_ICM_DEBUG
  if (ICMCoreObj) {
    ICMCoreObj->m_malloc_cnt++;

    MSG_LOW("ICM: Malloc cnt = %d, size = %d", ICMCoreObj->m_malloc_cnt,
                                               size, 0);
  }
#endif

  return p;
}

/*=============================================================================
FUNCTION: OEMCM_Free

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_Free(void *p)
{
  if (p) {
    /*lint -save -e534 */
    OEM_Free(p);
    /*lint -restore */

    if(ICMCoreObj)
      ICMCoreObj->m_malloc_cnt--;
  }
}

/*=============================================================================
FUNCTION: OEMCM_MergeMasks

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMCM_MergeMasks(const uint32 * pdwMasks)
{
   uint32   dw,dwm;

   dwm = 0;
   while((dw = *pdwMasks) != 0){
      dwm |= ((uint16)(dw & 0x0000FFFF));
      pdwMasks++;
   }
   return dwm;
}


/*=============================================================================
FUNCTION: OEMCM_GetSystemMode

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMSysMode OEMCM_GetSystemMode()
{
  return (AEECMSysMode) sd_misc_get_curr_acq_sys_mode();
}

/*=============================================================================
FUNCTION: OEMCM_GetCallType

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMCallType OEMCM_GetCallType(cm_call_id_type call_id)
{
  if (!ICMCoreObj || call_id >= OEMCM_MAX_NUM_CALLS)
    return AEECM_CALL_TYPE_NONE;

  return ICMCoreObj->m_call_info[call_id].call_type;
}

/*=============================================================================
FUNCTION: OEMCM_IsValidAppCallID

DESCRIPTION:
  Checks if the Application call_id is valid or not.


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static boolean OEMCM_IsValidAppCallID(AEECMCallID callID)
{
  if(!ICMCoreObj || !callID || callID > OEMCM_MAX_NUM_CALLS)
    return FALSE;

  return ICMCoreObj->m_state[OEMCM_CMCALLID(callID)].bValid;
}

/*=============================================================================
FUNCTION: OEMCM_IsValidCallID

DESCRIPTION:
  Checks if the DMSS call_id is valid or not.


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static boolean OEMCM_IsValidCallID(cm_call_id_type call_id)
{
  if(!ICMCoreObj || !call_id || call_id >= OEMCM_MAX_NUM_CALLS)
    return FALSE;

  return ICMCoreObj->m_state[call_id].bValid;
}

/*=============================================================================
FUNCTION: OEMCM_GetValidPhInfo

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMPhInfo * OEMCM_GetValidPhInfo(ICM *pMe)
{
  if (!pMe || !pMe->m_coreObj)
    return FALSE;

  if (pMe->m_coreObj->m_ph_valid)
    return &pMe->m_coreObj->m_ph_info;
  else
    return NULL;
}


/*===========================================================================

FUNCTION OEMCM_IsValidOprtModeSwitch

DESCRIPTION
  Check whether a given operating mode switch is legal.

DEPENDENCIES
  Should always match cmph.c: is_valid_oprt_mode_switch()

RETURN VALUE
  TRUE if operating mode switch is legal, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean OEMCM_IsValidOprtModeSwitch(

    AEECMOprtMode    fromOprtMode,
        /* from this operating mode */

    AEECMOprtMode    toOprtMode
        /* to this operating mode */
)
{
  sys_oprt_mode_e_type from_oprt_mode = (sys_oprt_mode_e_type)fromOprtMode;
  sys_oprt_mode_e_type to_oprt_mode = (sys_oprt_mode_e_type)toOprtMode;
  unsigned int i;
      /* loop counter */

  /* Table specifying valid operating mode switches
   * Copied from cmph.c: is_valid_oprt_mode_switch()
  */
  static const struct {

      sys_oprt_mode_e_type    from_oprt_mode;
          /* from operating mode */

      sys_oprt_mode_e_type    to_oprt_mode;
          /* to operating mode */

  } oprt_mode_tbl[] = {

        /* from oprt mode */         /* to oprt mode */
      { SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_ONLINE       },
      { SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_OFFLINE_AMPS },
      { SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_OFFLINE      },
      { SYS_OPRT_MODE_PWROFF,        SYS_OPRT_MODE_LPM          },

      { SYS_OPRT_MODE_OFFLINE_CDMA,  SYS_OPRT_MODE_PWROFF       },
      { SYS_OPRT_MODE_OFFLINE_CDMA,  SYS_OPRT_MODE_RESET        },
      { SYS_OPRT_MODE_OFFLINE_CDMA,  SYS_OPRT_MODE_OFFLINE_AMPS },
      { SYS_OPRT_MODE_OFFLINE_CDMA,  SYS_OPRT_MODE_OFFLINE      },

      { SYS_OPRT_MODE_OFFLINE_AMPS,  SYS_OPRT_MODE_PWROFF       },
      { SYS_OPRT_MODE_OFFLINE_AMPS,  SYS_OPRT_MODE_RESET        },
      { SYS_OPRT_MODE_OFFLINE_AMPS,  SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_OFFLINE_AMPS,  SYS_OPRT_MODE_OFFLINE      },

      { SYS_OPRT_MODE_OFFLINE,       SYS_OPRT_MODE_PWROFF       },
      { SYS_OPRT_MODE_OFFLINE,       SYS_OPRT_MODE_RESET        },
      { SYS_OPRT_MODE_OFFLINE,       SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_OFFLINE,       SYS_OPRT_MODE_OFFLINE_AMPS },

      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_OFFLINE_AMPS },
      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_OFFLINE      },
      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_LPM          },
      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_PWROFF       },
      #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
      #endif /* FEATURE_WCDMA, FEATURE_GSM */

      #ifdef FEATURE_FACTORY_TESTMODE
      { SYS_OPRT_MODE_ONLINE,        SYS_OPRT_MODE_FTM          },
      { SYS_OPRT_MODE_FTM,           SYS_OPRT_MODE_ONLINE       },
      { SYS_OPRT_MODE_FTM,           SYS_OPRT_MODE_PWROFF       },
      { SYS_OPRT_MODE_FTM,           SYS_OPRT_MODE_OFFLINE_CDMA },
      #endif /* FEATURE_FACTORY_TESTMODE */

      { SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_OFFLINE_CDMA },
      { SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_OFFLINE_AMPS },
      { SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_OFFLINE      },
      { SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_ONLINE       },
      { SYS_OPRT_MODE_LPM,           SYS_OPRT_MODE_PWROFF       }

  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!IN_RANGE((int)from_oprt_mode,
                     (int)SYS_OPRT_MODE_NONE,
                     (int)SYS_OPRT_MODE_MAX-1)) {
    MSG_HIGH("Invalid from_oprt_mode %d", from_oprt_mode, 0, 0);
    return FALSE;
  }

  if (!IN_RANGE((int)to_oprt_mode,
                     (int)SYS_OPRT_MODE_NONE,
                     (int)SYS_OPRT_MODE_MAX-1)) {
    MSG_HIGH("Invalid to_oprt_mode %d", to_oprt_mode, 0, 0);
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Switching into the same operating mode is always valid.
  */
  if( to_oprt_mode == from_oprt_mode )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check for a match in the oprt_mode_tbl to decide
  ** whether this is a valid operating mode switch.
  */
  for( i=0; i < ARR_SIZE(oprt_mode_tbl); i++ )
  {
    if( oprt_mode_tbl[i].from_oprt_mode == from_oprt_mode &&
        oprt_mode_tbl[i].to_oprt_mode   == to_oprt_mode )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, there is no match in the
  ** operating mode table - return FALSE.
  */
  return FALSE;

} /* OEMCM_IsValidOprtModeSwitch() */

/*=============================================================================
FUNCTION: OEMCM_GetCallState

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMCallState OEMCM_GetCallState(cm_call_id_type call_id)
{
  AEECMCallInfo * call_info;

  if (!ICMCoreObj ||
      call_id >= OEMCM_MAX_NUM_CALLS ||
      !ICMCoreObj->m_state[call_id].bValid)
    return AEECM_CALL_STATE_NONE;

  call_info = &ICMCoreObj->m_call_info[call_id];

  return call_info->call_state;
}

/*=============================================================================
FUNCTION: OEMCM_FillCallNum

DESCRIPTION:
  Translate AECHAR input strings to the equivalent CM structures.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_FillCallNum(cm_num_s_type*        called_number,
                              cm_num_s_type*        calling_number,
                              cm_orig_alpha_s_type *cm_alpha,
                              const AECHAR         *wszNumber,
                              const AECHAR         *wszCalling,
                              const AECHAR         *wszAlpha)
{
  char                  calling[CM_MAX_NUMBER_CHARS+1] = "";
  char                  called[CM_MAX_NUMBER_CHARS+1] = "";
  char                  alpha_buf[CM_MAX_ALPHA_LEN_FOR_CALL_ORIG+1];

  if (!called_number || !calling_number || !cm_alpha) {
    MSG_HIGH("Unexpected NULL pointer %p %p %p", called_number, calling_number,
                                                 wszAlpha);
    return;
  }

  if (wszNumber)
    (void) WSTRTOSTR(wszNumber, called, sizeof(called));

  OEMCM_InitNum(called_number, called);

  if (wszCalling)
    (void) WSTRTOSTR(wszCalling, calling, sizeof(calling));

  OEMCM_InitNum(calling_number, calling);

  if (wszAlpha) {
    (void) WSTRTOSTR(wszAlpha, alpha_buf, sizeof(alpha_buf));
    cm_alpha->len = (uint8)STRLEN(alpha_buf);
    (void) STRLCPY((char *)cm_alpha->buf, alpha_buf, sizeof(cm_alpha->buf));
  } else {
    cm_alpha->buf[0] = '\0';
    cm_alpha->len = 0;
  }
}

/*=============================================================================
FUNCTION: OEMCM_InitNum

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_InitNum(cm_num_s_type *num, const char *number)
{
  char buf[CM_MAX_NUMBER_CHARS+1];
  char *c;
  int  i;

  if (!num || !number)
    return;

  if (*number == '+')
    num->number_type = (uint8) CM_NUM_TYPE_INTERNATIONAL;
  else
    num->number_type = (uint8) CM_NUM_TYPE_UNKNOWN;

  c = buf;
  while(*number && c < (buf+CM_MAX_NUMBER_CHARS)) {
    if ((*number >= '0' && *number <= '9') || *number == '*' ||
         *number == '#' || *number == '+') {
      *c++ = *number;
    }

    number++;
  }
  *c = '\0';

  (void) STRLCPY((char *) num->buf, buf, sizeof(num->buf));

  num->len = (byte)STRLEN(buf);
  num->digit_mode = CM_DIGIT_MODE_4BIT_DTMF;

  /* Make sure digit mode is set correctly depending on the number */
  for ( i = 0; i < num->len; i++ ) {
    if ( !IN_RANGE(num->buf[i], '0', '9') &&
         (num->buf[i] != '*') && (num->buf[i] != '#' ))
      num->digit_mode = CM_DIGIT_MODE_8BIT_ASCII;
  }

  num->number_plan = (uint8) CM_NUM_PLAN_ISDN;
  num->pi = 0;
  num->si = 0;
  num->subaddr.extension_bit = 0;
  num->subaddr.subadd_type = 0;
  num->subaddr.odd_even_ind = 0;
  num->subaddr.num_char = 0;
}

/*=============================================================================
FUNCTION: OEMCM_CopyCharLenToAECHAR

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyCharLenToAECHAR(char *cmchar, uint8 len,
                                      AECHAR *aeechar, uint16 size)
{
  char *tmp;

  if (!size)
    return;

  if ((tmp = (char *)OEMCM_MALLOC((len+1)*sizeof(char))) == NULL) {
    aeechar[0] = (AECHAR) '\0';
    return;
  }

  (void) MEMCPY(tmp, cmchar, len);
  tmp[len] = '\0';

  (void) STRTOWSTR(tmp, aeechar, size);

  OEMCM_FREE(tmp);
}

/*=============================================================================
FUNCTION: OEMCM_CopyNumToOtherParty

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyNumToOtherParty(const cm_num_s_type *num,
                                      AEECMCallInfo *core_call_info)
{
  if (!num || !core_call_info)
    return;
  core_call_info->pi = (AEECMPI)num->pi;
  if(core_call_info->pi == AEECM_PI_RES) {
    MEMSET(core_call_info->other_party_no,0x00,sizeof(core_call_info->other_party_no));
    return;
  }

  if (num->len) {
    if((num->number_type == (uint8) CM_NUM_TYPE_INTERNATIONAL) &&
       (num->buf[0] != '+')) {
      core_call_info->other_party_no[0] = '+';
      OEMCM_CopyCharLenToAECHAR((char *)num->buf, num->len,
                               core_call_info->other_party_no+1,
                               sizeof(core_call_info->other_party_no)-2);
    } else {
      OEMCM_CopyCharLenToAECHAR((char *)num->buf, num->len,
                               core_call_info->other_party_no,
                               sizeof(core_call_info->other_party_no));
    }
  }
}

/*=============================================================================
FUNCTION: OEMCM_CopyNumToAEECMNum

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyNumToAEECMNum(cm_num_s_type *num, AEECMNumber *aee_num)
{
  aee_num->number_type = (AEECMNumberType)(num->number_type >> 4);
  aee_num->number_plan = (AEECMNumberPlan)num->number_plan;
  aee_num->pi = (AEECMPI)num->pi; /* same value */
  aee_num->si = (AEECMSI)num->si; /* same value */

  aee_num->subaddr.extension_bit = num->subaddr.extension_bit;
  aee_num->subaddr.odd_even_ind = num->subaddr.odd_even_ind;

  if (num->pi == AEECM_PI_RES)
  {
    MEMSET(aee_num->buf,0x00,sizeof(aee_num->buf));
    MEMSET(aee_num->subaddr.addr,0x00,sizeof(aee_num->subaddr.addr));
  }
  else
  {
    OEMCM_CopyCharLenToAECHAR((char *)num->buf, num->len, aee_num->buf,
                              sizeof(aee_num->buf));
    OEMCM_CopyCharLenToAECHAR((char *)num->subaddr.chari, num->subaddr.num_char,
                              aee_num->subaddr.addr,
                              sizeof(aee_num->subaddr.addr));
  }
}

/*=============================================================================
FUNCTION: OEMCM_ResetPrivateCallState

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_ResetPrivateCallState(cm_call_id_type call_id)
{
  AEECMPrivateCallState *state;

  if (!ICMCoreObj)
    return;

  state = &(ICMCoreObj->m_state[call_id]);

  state->bAnswered = FALSE;
  state->bEnded = FALSE;
  state->bSignalled = FALSE;
  state->bSetup = FALSE;
  state->bValid = TRUE;
}



/*=============================================================================
FUNCTION: OEMCM_CallNotifyInitDefault

DESCRIPTION:
  Initializes the notify_info structure passed along with each event.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void  OEMCM_CallNotifyInitDefault(AEECMNotifyInfo *notify_info,
                                         cm_call_id_type call_id,
                                         AEECMEvent event)
{
  notify_info->event = event;

  notify_info->event_data.call.call_id = OEMCM_AEECALLID(call_id);

  /* Update Data Call Stats */
  OEMCM_RefreshDataCallStats(ICMCoreObj, OEMCM_AEECALLID(call_id));

  (void) MEMCPY(&(notify_info->event_data.call.call_info),
           &(ICMCoreObj->m_call_info[call_id]),
           sizeof(AEECMCallInfo));

  notify_info->event_data.call.call_info.sys_mode = OEMCM_GetSystemMode();
}


/*=============================================================================
FUNCTION: OEMCM_HandlePhDefault

DESCRIPTION:
  Copy in phone info and send notification to application.


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandlePhDefault(cm_ph_info_s_type *ph_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  AEECMPhInfo *   core_ph_info;

  if (!ICMCoreObj) {
    return;
  }

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  core_ph_info = &ICMCoreObj->m_ph_info;

  OEMCM_CopyInPhInfo(core_ph_info, ph_info);

  if (event == AEECM_EVENT_PH_INFO_AVAIL)
    ICMCoreObj->m_ph_valid = TRUE;

  notify_info->event = event;
  (void) MEMCPY(&notify_info->event_data.ph, core_ph_info, sizeof(AEECMPhInfo));

  notify_info->event_data.ph.aoc_info.ccm = cm_ph_cmd_aoc_get_ccm();
  notify_info->event_data.ph.aoc_info.acm = cm_ph_cmd_aoc_get_acm();
  notify_info->event_data.ph.aoc_info.acm_max = cm_ph_cmd_aoc_get_acmmax();

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}


/*=============================================================================
FUNCTION: OEMCM_HandlePhTestControl

DESCRIPTION:
  Copy in phone info and send notification to application.


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandlePhTestControl(cm_ph_info_s_type *ph_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  AEECMPhInfo *   core_ph_info;

  if (!ICMCoreObj) {
    return;
  }

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  core_ph_info = &ICMCoreObj->m_ph_info;

  OEMCM_CopyInPhInfo(core_ph_info, ph_info);

  core_ph_info->test_control_type = (AEECMTestControlType)
                                    ph_info->test_control_type;

  if (event == AEECM_EVENT_PH_INFO_AVAIL)
    ICMCoreObj->m_ph_valid = TRUE;

  notify_info->event = AEECM_EVENT_PH_OPRT_MODE;

  (void) MEMCPY(&notify_info->event_data.ph, core_ph_info, sizeof(AEECMPhInfo));

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandlePhInternalDefault

DESCRIPTION:
  Copy in phone info and send notification to application.


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandlePhInternalDefault(cm_ph_info_s_type *ph_info, AEECMEvent event)
{
  if (!ICMCoreObj || !ph_info)
    return;

  /* Event not sent to App, copy latest info */
  OEMCM_CopyInPhInfo(&ICMCoreObj->m_ph_info, ph_info);

}

/*=============================================================================
FUNCTION: OEMCM_NotifyApps

DESCRIPTION:
  All events are notified to the application through this function.
  The different paths meet here.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_NotifyApps(AEECMNotifyInfo *notify_info)
{
  uint32  nMask;
  AEECMCallType call_type;

  if (!notify_info)
    return;

  switch (AEECM_EVENT_GROUP((uint16)notify_info->event)) {

    case AEECM_CALL_GROUP:
      call_type = notify_info->event_data.call.call_info.call_type;
      switch (call_type) {
        case AEECM_CALL_TYPE_VOICE:
        case AEECM_CALL_TYPE_EMERGENCY:
        case AEECM_CALL_TYPE_STD_OTASP:
        case AEECM_CALL_TYPE_NON_STD_OTASP:
          nMask = NMASK_CM_VOICE_CALL;
          break;
        case AEECM_CALL_TYPE_CS_DATA:
        case AEECM_CALL_TYPE_PS_DATA:
          nMask = NMASK_CM_DATA_CALL;
          break;
        case AEECM_CALL_TYPE_TEST:
          nMask = NMASK_CM_TEST_CALL;
          break;
        default:
          nMask = NMASK_CM_OTHER_CALL;
      }
      break;

    case AEECM_PH_GROUP:
      nMask = NMASK_CM_PHONE;
      break;

    case AEECM_SS_GROUP:
      nMask = NMASK_CM_SS;
      break;

    case AEECM_INBAND_GROUP:
      nMask = NMASK_CM_INBAND;
      break;

    case AEECM_MBMS_GROUP:
      nMask = NMASK_CM_MBMS;
      break;

    default:
      return;
  }

  MSG_HIGH("Sending event 0x%X to App", notify_info->event, 0, 0);

  if (NULL != ICMModelObj)
  {
    ModelEvent  evt;
    IModel     *pIModel = SYSMODELBASE_TO_IMODEL(&ICMModelObj->base);

    evt.evCode  = notify_info->event;
    evt.dwParam = (uint32)&notify_info->event_data;
    evt.pModel  = pIModel;

    IMODEL_Notify(pIModel, &evt);
  }

  if (NULL != ICMNotifierObj) {
      (void) ISHELL_Notify(ICMNotifierObj->m_pIShell, AEECLSID_CM_NOTIFIER, nMask, notify_info);
  }

  return;
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif

/*=============================================================================
FUNCTION: OEMCM_FillInCDMACallOptions

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_FillInCDMACallOptions(
                                      cm_cdma_orig_params_s_type* cdma_orig_ptr,
                                      cm_srv_type_e_type *srv_type,
                                      ICallOpts * options)
{
  CallOpt opt;

  if (!cdma_orig_ptr || !options) {
    return;
  }

  /* AEECM_CALLOPT_SERVICE_TYPE */
  if (OEMCallOpts_GetOpt(options, AEECM_CALLOPT_SERVICE_TYPE, &opt)
                                                             == SUCCESS) {
    *srv_type = (cm_srv_type_e_type) ((int)*((AEECMServiceType *)opt.pVal) - 1);
    MSG_LOW("Using %d as service type", *srv_type, 0, 0);
  }

  /* AEECM_CALLOPT_SRV_OPT */
  if (OEMCallOpts_GetOpt(options, AEECM_CALLOPT_SRV_OPT, &opt)
                                                             == SUCCESS) {
    cdma_orig_ptr->srv_opt = (uint16) *((AEECMServiceOption *)opt.pVal);
    MSG_LOW("Using %d as service option %d", cdma_orig_ptr->srv_opt, 0, 0);
  }

  /* AEECM_CALLOPT_OTASP_ACT_CODE */
  if (OEMCallOpts_GetOpt(options, AEECM_CALLOPT_OTASP_ACT_CODE, &opt)
                                                             == SUCCESS) {
    cdma_orig_ptr->activate_code = (cm_activate_code_e_type)
                                   *((AEECMActivateCode *)opt.pVal);
    MSG_LOW("Using %d as activation code", cdma_orig_ptr->activate_code,
                                           0, 0);
  }

}

/*=============================================================================
FUNCTION: OEMCM_CheckDTMFOnLength

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static boolean OEMCM_CheckDTMFOnLength(AEECMDTMFOnLength on_length)
{
    switch (on_length)
    {
      case AEECM_DTMF_ON_95:
      case AEECM_DTMF_ON_150:
      case AEECM_DTMF_ON_200:
      case AEECM_DTMF_ON_250:
      case AEECM_DTMF_ON_300:
      case AEECM_DTMF_ON_350:
          break;
#ifdef FEATURE_JCDMA
      case AEECM_DTMF_ON_SMS:
          break;
#endif
      default:
          return FALSE;
    }

    return TRUE;
}

/*=============================================================================
FUNCTION: OEMCM_CheckDTMFOffLength

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static boolean OEMCM_CheckDTMFOffLength(AEECMDTMFOffLength off_length)
{
    switch (off_length)
    {
      case AEECM_DTMF_OFF_60:
      case AEECM_DTMF_OFF_100:
      case AEECM_DTMF_OFF_150:
      case AEECM_DTMF_OFF_200:
          break;
      default:
          return FALSE;

    }

    return TRUE;
}

/*=============================================================================
FUNCTION: OEMCM_CheckDTMFDigits

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static boolean OEMCM_CheckDTMFDigits( uint8 cnt, uint8 *digit_buf)
{
  uint8  i;

  for( i=0; i < cnt; i++ )
  {
    if( !DECCHK(digit_buf[i]) &&
          digit_buf[i] != '*'   &&
          digit_buf[i] != '#' )
    {
      return FALSE;
    }
  }

  return TRUE;
}

/*=============================================================================
FUNCTION: OEMCM_CheckValidOprtMode

DESCRIPTION:
  cmph_in_valid_oprt_mode

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static boolean OEMCM_CheckValidOprtMode(AEECMPhInfo * ph_ptr)
{
  if( ( ph_ptr->oprt_mode == AEECM_OPRT_MODE_PWROFF      ) ||
      ( ph_ptr->oprt_mode == AEECM_OPRT_MODE_RESET       ) ||
      ( ph_ptr->oprt_mode == AEECM_OPRT_MODE_LPM         ) ||
      ( ph_ptr->oprt_mode == AEECM_OPRT_MODE_FTM )    )
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }

}

#ifndef FEATURE_UIONE_HDK
/*=============================================================================
FUNCTION: OEMCM_AllocateCallHistoryEntry

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static boolean OEMCM_AllocateCallHistoryEntry(AEECallHistoryEntry **ppEntry,
                                             uint8 ncnt, uint32 *pSizes)
{
  int i;
  AEECallHistoryField *fields;
  AEECallHistoryEntry *entry;

  if ( ( entry = MALLOC(sizeof(AEECallHistoryEntry))) == NULL ) {
    return FALSE;
  }

  if ( ( fields = MALLOC(ncnt * sizeof(AEECallHistoryField)) )
       == NULL ) {
    return FALSE;
  }

  for (i = 0; i < ncnt; i++) {
    if (pSizes[i] == 0)
        fields[i].pData = NULL;
    else {

      if ((fields[i].pData = MALLOC(pSizes[i])) == NULL) {
        for (--i; i >= 0; i--) {
           if (fields[i].pData)
            FREE(fields[i].pData);
        }
        FREE(fields);
        FREE(entry);
        return FALSE;
      }

    }
  }

  entry->pFields = fields;
  entry->wNumFields = ncnt;
  *ppEntry = entry;

  return TRUE;
}

/*=============================================================================
FUNCTION: FreeCallHistoryEntry

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_FreeCallHistoryEntry(AEECallHistoryEntry *pEntry)
{
   int i;

   for (i = 0; i < pEntry->wNumFields; ++i)
      if (pEntry->pFields[i].pData)
        FREE(pEntry->pFields[i].pData);

   FREE(pEntry->pFields);
   FREE(pEntry);
}

/*=============================================================================
FUNCTION: OEMCM_FillCallHistoryField

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_FillCallHistoryField(AEECallHistoryField *field, uint16 wID,
                                       uint32 wDataLen, void *pData)
{
  if (!field)
    return;

  field->ClsId = 0;
  field->wID = wID;
  (void) MEMCPY(field->pData, pData, (uint16)wDataLen);
  field->wDataLen = (uint16) wDataLen;
}

/*=============================================================================
FUNCTION: OEMCM_AddCallHistory

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_AddCallHistory(AEECMCallInfo *call_info)
{
  ICallHistory *         pICallHist = NULL;
  IShell *               pIShell = AEE_GetShell();
  int                    err;
  AEECallHistoryEntry *  pEntry;
  uint32                 field_lengths[OEMCM_NUM_CALLHISTORY_FIELDS];
  int                    i;
  uint16                 call_type;
  uint32                 start_time = 0;
  uint32                 curr_time;
  JulianType             julian_time;
  uint32                 duration;
  AECHAR                 unavail_str[AEECM_MAX_ALPHA_TAG_LENGTH];
  boolean                bUnavailable = FALSE;
  IConfig *              pIConfig = NULL;

  if (!pIShell)
    return;

  duration = call_info->duration/1000;

  MSG_HIGH("Saving a call with duration %d", duration, 0, 0);
  // Create the config object
  err = ISHELL_CreateInstance(pIShell,
                               AEECLSID_CONFIG,
                               (void **)&pIConfig);

  if (err != SUCCESS || pIConfig == NULL) {
    MSG_ERROR ("Failed to create instance of IConfig : %d",err, 0, 0);
  }
  else
  {
    uint32 timeSec;

    if (ICONFIG_SetItem(pIConfig,CFGI_LASTCALL_TIMER,&duration,sizeof(duration))!= SUCCESS)
      MSG_ERROR ("Failed to set Last Call Timer : %d",err, 0, 0);

    // Retrieve the total call timer information
    if (ICONFIG_GetItem(pIConfig,CFGI_LIFE_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
      MSG_ERROR("Failed to Get Life Timer",0,0,0);
    else
    {
      timeSec += duration;
      if (ICONFIG_SetItem(pIConfig,CFGI_LIFE_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
        MSG_ERROR("Failed to Set Liefe Timer ",0,0,0);
    }

    if (ICMCoreObj->m_ss_info.roam_status == AEECM_ROAM_STATUS_OFF)
    {
      // Retrieve the total home call timer information
      if (ICONFIG_GetItem(pIConfig,CFGI_AIR_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
        MSG_ERROR("Failed to Get Air Timer",0,0,0);
      else
      {
        timeSec += duration;
        if (ICONFIG_SetItem(pIConfig,CFGI_AIR_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
          MSG_ERROR("Failed to Set Air Timer",0,0,0);
      }
    }
    else
    {
      // Retrieve the total roam call timer information
      if (ICONFIG_GetItem(pIConfig,CFGI_ROAM_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
        MSG_ERROR("Failed to Get Roam Timer",0,0,0);
      else
      {
        timeSec += duration;
        if (ICONFIG_SetItem(pIConfig,CFGI_ROAM_TIMER,&timeSec,sizeof(timeSec))!= SUCCESS)
          MSG_ERROR("Failed to Set Roam Timer",0,0,0);
      }
    }
  }

  err = ISHELL_CreateInstance(pIShell, AEECLSID_CALLHISTORY,
                              (void **)&pICallHist);

  if (err != SUCCESS || pICallHist == NULL) {
    MSG_ERROR ("Failed to create instance of call hist : %d",err, 0, 0);
    return;
  }

  curr_time = GETTIMESECONDS();
  GETJULIANDATE(curr_time, &julian_time);

  if (julian_time.wYear != 1980)
    start_time = curr_time - call_info->duration/1000;

  /* CALL_TYPE */
  field_lengths[0] = sizeof(uint16);

  /* DURATION */
  field_lengths[1] = sizeof(uint32);

  /* NUMBER */
  field_lengths[2] = (uint32) ((uint32)(WSTRLEN(call_info->other_party_no)+1) *
                                sizeof(AECHAR));

  /* NAME */
  field_lengths[3] = (uint32) ((uint32)(WSTRLEN(call_info->alpha)+1) *
                                sizeof(AECHAR));

  /* DATE */
  if (start_time)
    field_lengths[4] = sizeof(uint32);


  /* Both name and number are null */
  if (field_lengths[2] == 2 && field_lengths[3] == 2) {
    bUnavailable = TRUE;
    (void) STRTOWSTR("Unavailable", unavail_str, sizeof(unavail_str));
    field_lengths[3] = (uint32)((uint32)(WSTRLEN(unavail_str)+1) *
                                sizeof(AECHAR));
  }

  /* Allocate field data */
  if(!OEMCM_AllocateCallHistoryEntry(&pEntry,
                                      start_time ?
                                      OEMCM_NUM_CALLHISTORY_FIELDS :
                                      OEMCM_NUM_CALLHISTORY_FIELDS - 1,
                                      field_lengths)) {
    MSG_ERROR("Failed to allocate data for call history fields", 0, 0, 0);
    (void) ICALLHISTORY_Release((void *)pICallHist);
    return;
  }

  i = 0;

  /* CALL_TYPE */
  if (!AEECM_IS_CALLSTATE_CONNECTED(call_info->call_state) &&
       call_info->direction == AEECM_CALL_DIRECTION_MT) {
    call_type = AEECALLHISTORY_CALL_TYPE_MISSED;
  } else {
    if (call_info->direction == AEECM_CALL_DIRECTION_MO)
      call_type = AEECALLHISTORY_CALL_TYPE_TO;
    else
      call_type = AEECALLHISTORY_CALL_TYPE_FROM;
  }

  OEMCM_FillCallHistoryField(&pEntry->pFields[i],
                             AEECALLHISTORY_FIELD_CALL_TYPE,
                             field_lengths[i],
                             &call_type);
  i++;



  /* DURATION */
  duration = call_info->duration/1000;
  OEMCM_FillCallHistoryField(&pEntry->pFields[i],
                             AEECALLHISTORY_FIELD_CALL_DURATION,
                             field_lengths[i],
                             &duration);
  i++;

  /* NUMBER */
  OEMCM_FillCallHistoryField(&pEntry->pFields[i],
                             AEECALLHISTORY_FIELD_NUMBER_EX,
                             field_lengths[i],
                             call_info->other_party_no);
  i++;

  /* NAME */
  if (bUnavailable) {
    OEMCM_FillCallHistoryField(&pEntry->pFields[i],
                             AEECALLHISTORY_FIELD_NAME,
                             field_lengths[i],
                             unavail_str);
  } else {
    OEMCM_FillCallHistoryField(&pEntry->pFields[i],
                             AEECALLHISTORY_FIELD_NAME,
                             field_lengths[i],
                             call_info->alpha);
  }

  i++;

  /* DATE */
  if (start_time) {
    OEMCM_FillCallHistoryField(&pEntry->pFields[i],
                             AEECALLHISTORY_FIELD_DATE_TIME,
                             field_lengths[i],
                             &start_time);

    i++;
  }

  if (ICALLHISTORY_AddEntry((void *)pICallHist,
                            pEntry) != SUCCESS) {
    MSG_ERROR("Unable to add call history pEntry", 0, 0, 0);
  }

  OEMCM_FreeCallHistoryEntry(pEntry);

  (void) ICALLHISTORY_Release((void *)pICallHist);

}
#endif //FEATURE_UIONE_HDK

/*=============================================================================
FUNCTION: OEMCM_RejectCall

DESCRIPTION:
  Reject an incoming call.

PROTOTYPE:
  int OEMCM_RejectCall(ICM *po,
                       AEECMCallID callID)

PARAMETERS:
  po                [in]: ICM interface pointer.
  callID            [in]: Call ID of the call to be rejected

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  This function is meaningful only for GSM/WCDMA modes.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_RejectCall(ICM *pMe, AEECMCallID callID)
{
  AEECMCallInfo *         call_info;
  cm_ans_params_s_type    answer_params;
  AEECMEventCBS *         event_cb;
  AEECMCallErrCBPkt *     err_cb_pkt;
  cm_call_id_type         call_id = OEMCM_CMCALLID(callID);
  boolean                 answer = FALSE;
  AEECMPrivateCallState  *m_state;

  call_info = &(pMe->m_coreObj->m_call_info[call_id]);
  if (call_info->alert_ans_bypass)
    return AEE_SUCCESS;

  m_state = &pMe->m_coreObj->m_state[call_id];

  AEECM_MUTEX_LOCK();
  if (!m_state->bAnswered) {
    m_state->bAnswered = TRUE;
    answer = TRUE;
  }
  AEECM_MUTEX_UNLOCK();

  if (!answer) {
    MSG_ERROR("Already answered this call: %d\n", call_id, 0, 0);
    return EBADSTATE;
  }

  (void) MEMSET( &answer_params, 0, sizeof( cm_ans_params_s_type));

  answer_params.info_type = (cm_call_mode_info_e_type)call_info->call_mode_info;
  answer_params.ans_params.gw_cs_ans.accept = FALSE;
  answer_params.ans_params.gw_cs_ans.call_type =
                           OEMCM_CallTypeToDMSS(OEMCM_GetCallType(call_id));

  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, sizeof(AEECMCallErrCBPkt));

  if (!event_cb) {
    MSG_ERROR("OEMCM_RejectCall: Failed to allocate event callback",
               0, 0, 0);
    m_state->bAnswered = FALSE;
    return EFAILED;
  }

  /* Fill in call_id here. */
  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
  err_cb_pkt->call_id = call_id;

  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_answer(OEMCM_DMSSCallErrCB, event_cb, gCMID, call_id,
                             &answer_params)) {
    m_state->bAnswered = FALSE;
    return EFAILED;
  }

  return AEE_SUCCESS;
}
/*===========================================================================

                            COPY DATA FUNCTIONS

===========================================================================*/

/*=============================================================================
FUNCTION: OEMCM_ALSLineTypeToAEECM

DESCRIPTION:
  Converts CM ALS Line type to ICM ALS Line type.


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
#ifdef FEATURE_ALS
#error code not present
#endif /* FEATURE_ALS */

#ifdef FEATURE_WLAN
#error code not present
#endif // FEATURE_WLAN

/*=============================================================================
FUNCTION: OEMCM_CopyInCallInfo

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyInCallInfo(AEECMCallInfo *dest,
                                 const cm_mm_call_info_s_type *src)
{
  if (!dest || !src) {
    MSG_ERROR("OEMCM_CopyInCallInfo received a NULL pointer", 0, 0, 0);
    return;
  }

  if (OEMCM_IsValidCallID(src->call_id)) {
    MSG_MED("ICM already received info about this call", 0, 0, 0);
    return;
  }

  dest->call_state = (AEECMCallState)AEECM_BM((int)src->call_state);
  dest->call_type = OEMCM_CallTypeToAEECM(src->call_type);
  dest->direction = AEECM_CALL_DIRECTION_NONE;
  dest->call_mode_info = (AEECMCallModeInfo)src->mode_info.info_type;

  if (dest->call_mode_info == AEECM_CALL_MODE_INFO_IP )
  {
    OEMCM_CmIpCallProgToAeecmCallProg(dest, src);
  }
  else
  {
    dest->call_progress = (AEECMCallProgress)
                          src->mode_info.info.gw_cs_call.call_progress;
  }

#ifdef FEATURE_ALS
#error code not present
#endif /*FEATURE_ALS */

  /* TODO: Copy in other fields */
  if (src->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
    dest->srv_opt = (AEECMServiceOption) src->mode_info.info.cdma_call.srv_opt;
    dest->alert_ans_bypass = src->mode_info.info.cdma_call.alert_ans_bypass;

    dest->privacy_pref =
        OEMCM_PrivacyPrefToAEECM(src->mode_info.info.cdma_call.privacy_pref);
  } else {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
  }

  OEMCM_ResetPrivateCallState(src->call_id);
}

/*=============================================================================
FUNCTION: OEMCM_CopyInPhInfo

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyInPhInfo(AEECMPhInfo *dest,
                               const cm_ph_info_s_type *src)
{
#ifdef FEATURE_CCBS
#error code not present
#endif

  if (! ICMCoreObj) {
    MSG_ERROR("OEMCM_CopyInPhInfo: Null core object ptr.", 0, 0, 0);
    return;
  }
  if (!dest || !src) {
    MSG_ERROR("OEMCM_CopyInPhInfo received a NULL pointer", 0, 0, 0);
    return;
  }

  dest->is_in_use = src->is_in_use;
  dest->oprt_mode = (AEECMOprtMode)src->oprt_mode;
  dest->cdma_lock_mode = (AEECMCDMALockMode)src->cdma_lock_mode;
  dest->answer_voice = (AEECMAnsVoicePref)src->answer_voice;
  dest->mode_pref = (AEECMModePref)src->mode_pref;
  dest->pref_term = OEMCM_PrefTermToAEECM(src->pref_term);
  dest->acq_order_pref = OEMCM_AcqOrderPrefToAEECM(src->acq_order_pref);
  dest->srv_domain_pref = OEMCM_SrvDomainPrefToAEECM(src->srv_domain_pref);
  dest->band_pref = OEMCM_BandPrefToAEECM(src->band_pref);
  dest->roam_pref = OEMCM_RoamPrefToAEECM(src->roam_pref);
  dest->nam_sel = (AEECMNAM)src->nam_sel;
  dest->curr_nam = (AEECMNAM)src->curr_nam;
  dest->hybr_pref = OEMCM_HybrPrefToAEECM(src->hybr_pref);
  dest->network_sel_mode_pref = (AEECMNwSelModePref)src->network_sel_mode_pref;
  (void) MEMCPY(&dest->plmn, &src->plmn, sizeof(AEECMPLMNID));

#ifdef FEATURE_WLAN
#error code not present
#else
  OEMCM_CopyInUserPrefNetworks(&dest->user_pref_networks,
                               &src->user_pref_networks);

  OEMCM_CopyInAvailableNetworks(&dest->available_networks,
                                &src->available_networks);
#endif // ! FEATURE_WLAN

  dest->is_cdma_subscription_available = src->is_cdma_subscription_available;
  dest->is_gw_subscription_available = src->is_gw_subscription_available;
#ifdef FEATURE_WLAN
#error code not present
#endif
  dest->rtre_config = (AEECMRTREConfig)src->rtre_config;
  dest->rtre_control = (AEECMRTREControl)src->rtre_control;

  dest->ddtm_pref = (AEECMDDTMPref)src->ddtm_pref;

#ifdef FEATURE_CCBS
#error code not present
#endif

#ifdef FEATURE_WLAN
#error code not present
#endif

  ICMCoreObj->m_ph_valid = TRUE;
}

/*=============================================================================
FUNCTION: OEMCM_XlateBcmcsSrvStatus

DESCRIPTION:
  Translates the BCMCS service status values that come up through CM to the
  equivalent ICM service status values.

DEPENDENCIES:
  Depends on the items in sys_bcmcs_srv_status_e_type (sys.h) and
  AEECMBCMCSSrvStatus (AEECM.h).

SIDE EFFECTS:
  None

=============================================================================*/
#ifdef FEATURE_BCMCS
#error code not present
#endif

#ifdef CM_API_HDR_ACTIVE_PROT
/*=============================================================================
FUNCTION: OEMCM_XlateHDRActiveProtType

DESCRIPTION:
  Translates the HDR Protocol Type values that come up through CM to the
  equivalent ICM service status values.

DEPENDENCIES:
  Depends on the items in sys_hdr_active_prot_e_type (sys.h) and
  AEECMActiveHDRProtType (AEECM.h).

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMActiveProtType OEMCM_XlateHDRActiveProtType(sys_active_prot_e_type activeHDRProt)
{
  AEECMActiveProtType retVal = AEECM_SYS_ACTIVE_PROT_NONE;

  switch(activeHDRProt)
  {
    case SYS_ACTIVE_PROT_HDR_REL0:
      retVal = AEECM_SYS_ACTIVE_PROT_HDR_REL0;
      break;

    case SYS_ACTIVE_PROT_HDR_RELA:
      retVal = AEECM_SYS_ACTIVE_PROT_HDR_RELA;
      break;

    default:
      MSG_MED("HDR Protocol Unavailable %d",activeHDRProt,0,0);
      break;
  }

  return(retVal);
}
#endif

/*=============================================================================
FUNCTION: OEMCM_CopyInSSInfo

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyInSSInfo(AEECMSSInfo *dest,
                               const cm_mm_ss_info_s_type *src)
{
  if (!dest || !src) {
    MSG_ERROR("OEMCM_CopyInSSInfo received a NULL pointer", 0, 0, 0);
    return;
  }

  dest->hdr_hybrid = src->hdr_hybrid;
  dest->srv_status = (AEECMSrvStatus)src->srv_status;
  dest->hdr_srv_status = (AEECMSrvStatus)src->hdr_srv_status;
  dest->srv_domain = (AEECMSrvDomain)src->srv_domain;
  dest->srv_capability = (AEECMSrvCapability)src->srv_capability;
  dest->sys_mode = (AEECMSysMode)src->sys_mode;
  dest->roam_status = (AEECMRoamStatus)src->roam_status;
  #ifdef CM_API_HDR_ACTIVE_PROT
  dest->hdr_active_prot = OEMCM_XlateHDRActiveProtType(src->hdr_active_prot);
  #endif
  dest->hdr_roam_status = (AEECMRoamStatus)src->hdr_roam_status;
  OEMCM_CopyInSysID(&dest->sys_id, &src->sys_id);

  dest->rssi = src->rssi;
#if defined(CM_API_SIGNAL_STRENGTH_INDICATION)
  dest->rssi2 = src->rssi2;
  dest->ecio = src->ecio;
  dest->ecio_for_display = src->ecio_for_display;
#else
  /* Default to R5 so that 0 bars are displayed */
  dest->rssi2 = AEECM_DEFAULT_RSSI_TO_R5;
  /* Default to E5 so that 0 bars are displayed */
  dest->ecio = AEECM_DEFAULT_ECIO_TO_E5;
  dest->ecio_for_display = FALSE;
#endif
  dest->hdr_rssi = src->hdr_rssi;
  dest->hdr_sinr = src->hdr_sinr;
#ifdef FEATURE_WLAN
#error code not present
#endif

  if (AEECM_IS_MODE_CDMA(dest->sys_mode)) {
    dest->band_class = (AEECMSysBandClass)src->mode_info.cdma_info.band_class;
    dest->block_or_system = (AEECMSysBlocks)
                             src->mode_info.cdma_info.block_or_system;
    dest->is_registered = src->mode_info.cdma_info.is_registered;
    dest->uz_id = src->mode_info.cdma_info.uz_id;

    (void) MEMCPY(&dest->uz_name, &src->mode_info.cdma_info.uz_name,
                  sizeof(AEECMUZName));

  } else if (AEECM_IS_MODE_GW(dest->sys_mode)) {
    dest->sim_state = (AEECMSIMState) src->mode_info.gw_info.sim_state;
    dest->plmn_forbidden = src->mode_info.gw_info.plmn_forbidden;

#ifdef FEATURE_USRSYSCLOCK
    OEMCM_HandleSsMobilityMgmt(&src->mode_info.gw_info.mm_information,
                               &dest->mm_information);
#else
    OEMCM_CopyInMMInfo(&dest->mm_information,
                       &src->mode_info.gw_info.mm_information);
#endif

  }

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

  ICMCoreObj->m_ss_valid = TRUE;
#ifdef FEATURE_DS_END_KEY_DROPS_CALL
  if (AEECM_IS_MODE_CDMA(dest->sys_mode))
    ICMCoreObj->m_p_rev_in_use = src->mode_info.cdma_info.p_rev_in_use;
#endif

#if defined(FEATURE_HSDPA) && defined(CM_API_CELL_SUPP_SERVICES)
#error code not present
#endif

#ifdef FEATURE_BCMCS
#error code not present
#endif

}

#ifndef FEATURE_USRSYSCLOCK
/*=============================================================================
FUNCTION: OEMCM_CopyInMMInfo

DESCRIPTION:
  Copy in Mobility Mangement Informtion

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyInMMInfo(AEECMMMInfo *mmInfo,
                               const sys_mm_information_s_type *mm_info)
{
  JulianType julian_time;
  uint32     secs;
  uint32     adj;

  mmInfo->plmn_avail                = mm_info->plmn_avail;
  mmInfo->full_name_avail           = mm_info->full_name_avail;
  mmInfo->short_name_avail          = mm_info->short_name_avail;
  mmInfo->time_and_time_zone_avail  = mm_info->univ_time_and_time_zone_avail;
  mmInfo->time_zone_avail           = mm_info->time_zone_avail;
  mmInfo->lsa_identity_avail        = mm_info->lsa_identity_avail;
  mmInfo->daylight_saving_adj_avail = mm_info->daylight_saving_adj_avail;
  mmInfo->daylight_saving_adj       = OEMCM_SysDSTToAEECMDST(mm_info->daylight_saving_adj);
  julian_time.wYear = 2000;

  if (mm_info->plmn_avail)
    (void) MEMCPY(&mmInfo->plmn, &mm_info->plmn, sizeof(AEECMPLMNID));

  if (mm_info->full_name_avail)
    OEMCM_CopyInFullNWName(&mmInfo->full_name, &mm_info->full_name);


  if (mm_info->short_name_avail)
    OEMCM_CopyInShortNWName(&mmInfo->short_name, &mm_info->short_name);

  if (mm_info->univ_time_and_time_zone_avail) {

    julian_time.wYear  += mm_info->univ_time_and_time_zone.year;
    julian_time.wMonth  = mm_info->univ_time_and_time_zone.month;
    julian_time.wDay    = mm_info->univ_time_and_time_zone.day;
    julian_time.wHour   = mm_info->univ_time_and_time_zone.hour;
    julian_time.wMinute = mm_info->univ_time_and_time_zone.minute;
    julian_time.wSecond = mm_info->univ_time_and_time_zone.second;
    julian_time.wWeekDay = 0;

    secs = JULIANTOSECONDS(&julian_time);

    if (mm_info->daylight_saving_adj_avail) {

      switch (mm_info->daylight_saving_adj) {
        case SYS_DAY_SAV_ADJ_PLUS_ONE_HOUR:
          adj = 1;
          break;
        case SYS_DAY_SAV_ADJ_PLUS_TWO_HOUR:
          adj = 2;
          break;
        default:
          adj = 0;
          break;
      }

      secs += (adj * 3600);
    }

    GETJULIANDATE(secs, &julian_time);

    (void) MEMCPY(&mmInfo->time_and_time_zone.time, &julian_time,
                  sizeof(JulianType));
    mmInfo->time_and_time_zone.time_zone = (AEECMTimeZone)
                        mm_info->univ_time_and_time_zone.time_zone;
  }

  if (mm_info->time_zone_avail)
    mmInfo->time_zone = (AEECMTimeZone) mm_info->time_zone;

  if (mm_info->lsa_identity_avail) {
    mmInfo->lsa_identity.type = (AEECMLSAIdentityType)
                                 mm_info->lsa_identity.type;
    (void) MEMCPY(mmInfo->lsa_identity.identity,
                  mm_info->lsa_identity.identity, 3);
  }

}
#endif

/*=============================================================================
FUNCTION: OEMCM_CopyInSysID

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyInSysID(AEECMSysID *dest, const sys_sys_id_s_type *src)
{
  dest->id_type = (AEECMSysIDType) src->id_type;

  (void) MEMCPY(&dest->id, &src->id, sizeof(AEECMSysIDData));
}

/*=============================================================================
FUNCTION: OEMCM_CopyInFullNWName

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyInFullNWName(AEECMNWName *dest,
                                   const sys_network_full_name_s_type *src)
{
  dest->length = src->length;
  dest->coding_scheme = (AEECMNWCodingType)src->coding_scheme;
  dest->add_country_initials = src->add_country_initials;

  if( src->coding_scheme ==  (sys_network_name_coding_scheme_type_e_type)AEECM_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET)
    (void) OEMCM_ConvertDefaultToAscii(dest->name, src->name, src->length);
  else
    (void) MEMCPY(dest->name, src->name, AEECM_MAX_NW_NAME_CHARS);
}

/*=============================================================================
FUNCTION: OEMCM_CopyInShortNWName

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyInShortNWName(AEECMNWName *dest,
                                    const sys_network_short_name_s_type *src)
{
  dest->length = src->length;
  dest->coding_scheme = (AEECMNWCodingType)src->coding_scheme;
  dest->add_country_initials = src->add_country_initials;

  if( src->coding_scheme ==  (sys_network_name_coding_scheme_type_e_type)AEECM_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET)
    (void) OEMCM_ConvertDefaultToAscii(dest->name, src->name, src->length);
  else
    (void) MEMCPY(dest->name, src->name, AEECM_MAX_NW_NAME_CHARS);
}

/*=============================================================================
FUNCTION: OEMCM_ConvertDefaultToAscii

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_ConvertDefaultToAscii(uint8 *dest, const uint8 *src, int length)
{
#ifndef CM_API_UTIL_USSD_FUNCTIONS
  uint8 numBits = length * 8 ;
  uint8 numChars = MIN((numBits / 7), AEECM_MAX_NW_NAME_CHARS);
#endif

#ifdef CM_API_UTIL_USSD_FUNCTIONS
  (void) cm_util_gsm_alphabet_to_ascii(  (byte *)dest,
                                         (byte *)src,
                                         (uint8)length );
#else
  cm_util_def_alphabet_to_ascii(  (byte *)src,
                                  (uint8)numChars,
                                  (byte *)dest);
#endif
}

#ifdef FEATURE_WLAN
#error code not present
#endif // FEATURE_WLAN

/*=============================================================================
FUNCTION: OEMCM_CopyInAvailableNetworks

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyInAvailableNetworks(AEECMDetailedPLMNList *dest,
                                      const sys_detailed_plmn_list_s_type *src)
{
  uint32 i = 0;

  dest->length = (src->length > AEECM_PLMN_LIST_MAX_LENGTH) ?
                  AEECM_PLMN_LIST_MAX_LENGTH :
                  src->length;

  for ( i = 0; i < dest->length; i++) {

    (void) MEMCPY(&dest->info[i].plmn, &src->info[i].plmn, sizeof(AEECMPLMNID));

    dest->info[i].rat = (AEECMRadioAccessTech) src->info[i].rat;
    dest->info[i].plmn_service_capability = (AEECMSrvCapability)
                                         src->info[i].plmn_service_capability;
    dest->info[i].plmn_forbidden = src->info[i].plmn_forbidden;
    dest->info[i].list_category = (AEECMDetailedPLMNCat)
                                  src->info[i].list_category;
    dest->info[i].signal_quality = (AEECMSignalQuality)
                                   src->info[i].signal_quality;
    dest->info[i].signal_strength = (AEECMSignalStrength)
                                    src->info[i].signal_strength;
  }

}

/*=============================================================================
FUNCTION: OEMCM_CopyInUserPrefNetworks

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyInUserPrefNetworks(AEECMUserPLMNList *dest,
                                    const sys_user_pref_plmn_list_s_type *src)
{
  int i = 0;


  dest->length = (src->length > AEECM_USER_PPLMN_LIST_MAX_LENGTH) ?
                  AEECM_USER_PPLMN_LIST_MAX_LENGTH :
                  src->length;

  for (i = 0; i < dest->length; i++) {

    (void) MEMCPY(&dest->info[i].plmn, &src->info[i].plmn, sizeof(AEECMPLMNID));

    dest->info[i].access_tech = (AEECMAccessTech) src->info[i].access_tech;
  }

}

/*=============================================================================
FUNCTION: OEMCM_CopyOutUserPrefNetworks

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyOutUserPrefNetworks(sys_user_pref_plmn_list_s_type *dest,
                                          AEECMUserPLMNList *src)
{
  int i = 0;


  dest->length = (src->length > SYS_USER_PPLMN_LIST_MAX_LENGTH) ?
                  SYS_USER_PPLMN_LIST_MAX_LENGTH :
                  src->length;

  for (i = 0; i < dest->length; i++) {

    (void) MEMCPY(&dest->info[i].plmn, &src->info[i].plmn, sizeof(AEECMPLMNID));

    dest->info[i].access_tech = (sys_access_tech_e_type)
                                src->info[i].access_tech;
  }

}

/*=============================================================================
FUNCTION: OEMCM_CopyInSignal

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyInSignal(AEECMSignal *aee_signal,
                               cm_signal_s_type *cm_signal)
{
  AEECMSysMode sys_mode;
  aee_signal->is_signal_info_avail = cm_signal->is_signal_info_avail;

  if (!cm_signal->is_signal_info_avail)
    return;

  sys_mode = OEMCM_GetSystemMode();
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  if (AEECM_IS_MODE_CDMA(sys_mode)) {
    switch(cm_signal->signal_type) {
      case CAI_SIG_TONE:
        aee_signal->signal_type = AEECM_SIGNAL_CDMA_TONE;
        aee_signal->signal_pitch = AEECM_PITCH_NONE;
        aee_signal->signal.cdma_tone = (AEECMCDMASignalTone) cm_signal->signal;
        break;

      case CAI_SIG_ISDN:
        aee_signal->signal_type = AEECM_SIGNAL_CDMA_ISDN;
        aee_signal->signal_pitch = AEECM_PITCH_NONE;
        aee_signal->signal.cdma_isdn = (AEECMCDMASignalISDN) cm_signal->signal;
        break;

      case CAI_SIG_IS54B:
        aee_signal->signal_type = AEECM_SIGNAL_CDMA_IS54B;
        aee_signal->signal_pitch = (AEECMSignalPitch) cm_signal->alert_pitch;
        aee_signal->signal.cdma_is54b =
                       (AEECMCDMASignalIS54B) cm_signal->signal;
        break;

      default:
        MSG_ERROR("Unknown CDMA signal type %d", cm_signal->signal_type, 0, 0);
        aee_signal->is_signal_info_avail = FALSE;
        break;
    }
  }
#endif

  return;
}

/*===========================================================================

                            TRANSLATION FUNCTIONS

===========================================================================*/

/*=============================================================================
FUNCTION: OEMCM_CallTypeToAEECM

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMCallType OEMCM_CallTypeToAEECM(cm_call_type_e_type call_type)
{
  if (!IN_RANGE(call_type, CM_CALL_TYPE_VOICE, CM_CALL_TYPE_EMERGENCY))
    return AEECM_CALL_TYPE_NONE;

  return (AEECMCallType) AEECM_BM((int)call_type);
}


/*=============================================================================
FUNCTION: OEMCM_CallTypeToDMSS

DESCRIPTION:

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static cm_call_type_e_type OEMCM_CallTypeToDMSS(AEECMCallType call_type)
{
  uint16 i = 0;

  if (!IN_RANGE(call_type, AEECM_CALL_TYPE_VOICE, AEECM_CALL_TYPE_EMERGENCY))
    return CM_CALL_TYPE_NONE;

  while(!((((uint32)call_type) >> i) & 0x01))
    i++;

  if ((int)call_type != (1 << i))
    return CM_CALL_TYPE_NONE;

  return (cm_call_type_e_type) i;
}
/*=============================================================================
FUNCTION: OEMCM_PrivacyPrefToAEECM

DESCRIPTION:
  Convert Privacy preference enumeration from DMSS to AEECM

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMPrivacyPref OEMCM_PrivacyPrefToAEECM(cm_privacy_pref_e_type pref)
{
  switch(pref) {
    case CM_PRIVACY_PREF_STANDARD:
      return AEECM_PRIVACY_PREF_STANDARD;

    case CM_PRIVACY_PREF_ENHANCED:
      return AEECM_PRIVACY_PREF_ENHANCED;

    default:
      return AEECM_PRIVACY_PREF_NONE;
  }
}

/*=============================================================================
FUNCTION: OEMCM_PrivacyPrefToDMSS

DESCRIPTION:
  Convert Privacy preference enumeration from AEECM to DMSS

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static cm_privacy_pref_e_type OEMCM_PrivacyPrefToDMSS(AEECMPrivacyPref pref)
{
  switch(pref) {
    case AEECM_PRIVACY_PREF_STANDARD:
      return CM_PRIVACY_PREF_STANDARD;

    case AEECM_PRIVACY_PREF_ENHANCED:
      return CM_PRIVACY_PREF_ENHANCED;

    default:
      return CM_PRIVACY_PREF_NONE;
  }
}


/*=============================================================================
FUNCTION: OEMCM_PrefTermToAEECM

DESCRIPTION:
  Convert preference term enumeration from DMSS to AEECM

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMPrefTerm OEMCM_PrefTermToAEECM(cm_pref_term_e_type pref)
{
  switch (pref)
  {
    case CM_PREF_TERM_NONE:
      return AEECM_PREF_TERM_NONE;

    case CM_PREF_TERM_PERMANENT:
      return AEECM_PREF_TERM_PERMANENT;

    case CM_PREF_TERM_PWR_CYCLE:
      return AEECM_PREF_TERM_PWR_CYCLE;

    case CM_PREF_TERM_1_CALL:
      return AEECM_PREF_TERM_1_CALL;

    case CM_PREF_TERM_1_CALL_OR_TIME:
    case CM_PREF_TERM_CM_1_CALL:
    case CM_PREF_TERM_CM_1_CALL_PLUS:
    case CM_PREF_TERM_CM_MO_SMS:
      return AEECM_PREF_TERM_1_CALL_OR_TIME;

#ifdef CM_API_PREF_TERM_MANUAL
    case CM_PREF_TERM_NET_SEL_MANUAL:
      return AEECM_PREF_TERM_NET_SEL_MANUAL;
#endif

    default:
      return AEECM_PREF_TERM_NONE;
  }
}

/*=============================================================================
FUNCTION: OEMCM_PrefTermToDMSS

DESCRIPTION:
  Convert preference term enumeration from AEECM to DMSS

DEPENDENCIES:
  None

SIDE EFFECTS:
 None

=============================================================================*/
static cm_pref_term_e_type OEMCM_PrefTermToDMSS(AEECMPrefTerm pref)
{
  switch (pref)
  {
    case AEECM_PREF_TERM_NONE:
      return CM_PREF_TERM_NONE;

    case AEECM_PREF_TERM_PERMANENT:
      return CM_PREF_TERM_PERMANENT;

    case AEECM_PREF_TERM_PWR_CYCLE:
      return CM_PREF_TERM_PWR_CYCLE;

    case AEECM_PREF_TERM_1_CALL:
      return CM_PREF_TERM_1_CALL;

    case AEECM_PREF_TERM_1_CALL_OR_TIME:
      return CM_PREF_TERM_1_CALL_OR_TIME;

    case AEECM_PREF_TERM_NET_SEL_MANUAL:
#ifdef CM_API_PREF_TERM_MANUAL
      return CM_PREF_TERM_MANUAL;
#else
      /* If the "MANUAL" value is not supported, PERMANENT is the next
       * closest thing.
       */
      return CM_PREF_TERM_PERMANENT;
#endif

    default:
      return CM_PREF_TERM_NONE;
  }
}

/*=============================================================================
FUNCTION: OEMCM_AcqOrderPrefToAEECM

DESCRIPTION:
  Convert acquisition order preference enumeration from DMSS to AEECM

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMAcqOrderPref OEMCM_AcqOrderPrefToAEECM(cm_gw_acq_order_pref_e_type pref)
{
  switch (pref)
  {
    case CM_GW_ACQ_ORDER_PREF_NONE:
      return AEECM_GW_ACQ_ORDER_PREF_NONE;

    case CM_GW_ACQ_ORDER_PREF_AUTOMATIC:
      return AEECM_GW_ACQ_ORDER_PREF_AUTOMATIC;

    case CM_GW_ACQ_ORDER_PREF_GSM_WCDMA:
      return AEECM_GW_ACQ_ORDER_PREF_GSM_WCDMA;

    case CM_GW_ACQ_ORDER_PREF_WCDMA_GSM:
      return AEECM_GW_ACQ_ORDER_PREF_WCDMA_GSM;

    case CM_GW_ACQ_ORDER_PREF_NO_CHANGE:
      return AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE;

#ifdef CM_API_PERSISTENT_PREF
    case CM_GW_ACQ_ORDER_PREF_PERSISTENT:
      return AEECM_GW_ACQ_ORDER_PREF_PERSISTENT;
#endif

    default:
      return AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE;
  }
}

/*=============================================================================
FUNCTION: OEMCM_AcqOrderPrefToDMSS

DESCRIPTION:
  Convert acquisition order preference enumeration from AEECM to DMSS

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static cm_gw_acq_order_pref_e_type OEMCM_AcqOrderPrefToDMSS(AEECMAcqOrderPref pref)
{
  switch (pref)
  {
    case AEECM_GW_ACQ_ORDER_PREF_NONE:
      return CM_GW_ACQ_ORDER_PREF_NONE;

    case AEECM_GW_ACQ_ORDER_PREF_AUTOMATIC:
      return CM_GW_ACQ_ORDER_PREF_AUTOMATIC;

    case AEECM_GW_ACQ_ORDER_PREF_GSM_WCDMA:
      return CM_GW_ACQ_ORDER_PREF_GSM_WCDMA;

    case AEECM_GW_ACQ_ORDER_PREF_WCDMA_GSM:
      return CM_GW_ACQ_ORDER_PREF_WCDMA_GSM;

    case AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE:
      return CM_GW_ACQ_ORDER_PREF_NO_CHANGE;

    case AEECM_GW_ACQ_ORDER_PREF_PERSISTENT:
#ifdef CM_API_PERSISTENT_PREF
      return CM_GW_ACQ_ORDER_PREF_PERSISTENT;
#else
      return CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
#endif

    default:
      return CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
  }
}

/*=============================================================================
FUNCTION: OEMCM_BandPrefToAEECM

DESCRIPTION:
  Convert band preference enumeration from DMSS to AEECM

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMBandPref OEMCM_BandPrefToAEECM(cm_band_pref_e_type pref)
{
  int32               nPrefIdx;
  AEECMBandPref       nConvPref = 0;
  cm_band_pref_e_type nCMBandPref;

  /* First, we handle the special cases.  The ADS compiler doesn't
   * allow 64-bit values for switch cases, so we'll do if..else here.
   */
  if (pref == CM_BAND_PREF_NONE)
  {
    return AEECM_BAND_PREF_NONE;
  }
  else if (pref == CM_BAND_PREF_NO_CHANGE)
  {
    return AEECM_BAND_PREF_NO_CHANGE;
  }
  else if (pref == CM_BAND_PREF_ANY)
  {
    return AEECM_BAND_PREF_ANY;
  }
#ifdef CM_API_PERSISTENT_PREF
  else if (pref == CM_BAND_PREF_PERSISTENT)
  {
    /* Per CM team, although CM_BAND_PREF_PERSISTENT is implemented
     * as a bitmask, the band pref is only considered "persistent"
     * if that is the *only bit set.  Hence the direct "==" comparison.
     */
    return AEECM_BAND_PREF_PERSISTENT;
  }
#endif

  /* For normal cases, we just traverse the OEMCM_BandPrefConv table
   * to construct the AEECM band pref value.
   */
  for (nPrefIdx = 0; nPrefIdx < ARRAY_SIZE(OEMCM_BandPrefConv); nPrefIdx++)
  {
    /* If the bit(s) for this table entry is/are set in the
     * given CM preference, and if the table entry is not
     * CM_BAND_PREF_NONE, then set the corresponding bits
     * in the converted preference.  (CM_BAND_PREF_NONE indicates
     * that there is no corresponding CM value, so we don't want
     * to set the bit(s) for the ICM value.)
     */
    nCMBandPref = OEMCM_BandPrefConv[nPrefIdx].cm_band_pref;
    if (((pref & nCMBandPref) == nCMBandPref) &&
         (OEMCM_BandPrefConv[nPrefIdx].cm_band_pref != CM_BAND_PREF_NONE))
    {
      nConvPref |= OEMCM_BandPrefConv[nPrefIdx].icm_band_pref;
    }
  }

  return nConvPref;
}

/*=============================================================================
FUNCTION: OEMCM_BandPrefToDMSS

DESCRIPTION:
  Convert band preference enumeration from AEECM to DMSS

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static cm_band_pref_e_type OEMCM_BandPrefToDMSS(AEECMBandPref pref)
{
  int32                nPrefIdx;
  cm_band_pref_e_type  nConvPref = 0;
  AEECMBandPref        nICMBandPref;

  /* First, we handle the special cases.  The ADS compiler doesn't
   * allow 64-bit values for switch cases, so we'll do if..else here.
   */
  if (pref == AEECM_BAND_PREF_NONE)
  {
    return CM_BAND_PREF_NONE;
  }
  else if (pref == AEECM_BAND_PREF_NO_CHANGE)
  {
    return CM_BAND_PREF_NO_CHANGE;
  }
  else if (pref == AEECM_BAND_PREF_ANY)
  {
    return CM_BAND_PREF_ANY;
  }
  else if (pref == AEECM_BAND_PREF_PERSISTENT)
  {
    /* Per CM team, although CM_BAND_PREF_PERSISTENT is implemented
     * as a bitmask, the band pref is only considered "persistent"
     * if that is the *only bit set.  Hence the direct "==" comparison.
     */
#ifdef CM_API_PERSISTENT_PREF
    return CM_BAND_PREF_PERSISTENT;
#else
    return CM_BAND_PREF_NO_CHANGE;
#endif
  }

  /* For normal cases, we just traverse the OEMCM_BandPrefConv table
   * to construct the CM band pref value.
   */
  for (nPrefIdx = 0; nPrefIdx < ARRAY_SIZE(OEMCM_BandPrefConv); nPrefIdx++)
  {
    /* If the bit(s) for this table entry is/are set in the
     * given ICM preference, and if the table entry is not
     * AEECM_BAND_PREF_NONE, then set the corresponding bits
     * in the converted preference.  (AEECM_BAND_PREF_NONE indicates
     * that there is no corresponding ICM value, so we don't want
     * to set the bit(s) for the CM value.)
     */
    nICMBandPref = OEMCM_BandPrefConv[nPrefIdx].icm_band_pref;
    if (((pref & nICMBandPref) == nICMBandPref) &&
         (OEMCM_BandPrefConv[nPrefIdx].icm_band_pref != AEECM_BAND_PREF_NONE))
    {
      nConvPref |= OEMCM_BandPrefConv[nPrefIdx].cm_band_pref;
    }
  }

  return nConvPref;
}

/*=============================================================================
FUNCTION: OEMCM_RoamPrefToAEECM

DESCRIPTION:
  Convert roaming preference enumeration from DMSS to AEECM

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMRoamPref OEMCM_RoamPrefToAEECM(cm_roam_pref_e_type pref)
{
  switch(pref) {
    case CM_ROAM_PREF_NONE:
      return AEECM_ROAM_PREF_NONE;

    case CM_ROAM_PREF_HOME:
      return AEECM_ROAM_PREF_HOME;

    case CM_ROAM_PREF_AFFIL:
      return AEECM_ROAM_PREF_AFFIL;

    case CM_ROAM_PREF_ANY:
      return AEECM_ROAM_PREF_ANY;

    case CM_ROAM_PREF_NO_CHANGE:
      return AEECM_ROAM_PREF_NO_CHANGE;

#ifdef CM_API_PERSISTENT_PREF
    case CM_ROAM_PREF_PERSISTENT:
      return AEECM_ROAM_PREF_PERSISTENT;
#endif

    default:
      return AEECM_ROAM_PREF_NO_CHANGE;
  }
}

/*=============================================================================
FUNCTION: OEMCM_RoamPrefToDMSS

DESCRIPTION:
  Convert roaming preference enumeration from AEECM to DMSS

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static cm_roam_pref_e_type OEMCM_RoamPrefToDMSS(AEECMRoamPref pref)
{
  switch(pref) {
    case AEECM_ROAM_PREF_NONE:
      return CM_ROAM_PREF_NONE;

    case AEECM_ROAM_PREF_HOME:
      return CM_ROAM_PREF_HOME;

    case AEECM_ROAM_PREF_AFFIL:
      return CM_ROAM_PREF_AFFIL;

    case AEECM_ROAM_PREF_ANY:
      return CM_ROAM_PREF_ANY;

    case AEECM_ROAM_PREF_NO_CHANGE:
      return CM_ROAM_PREF_NO_CHANGE;

    case AEECM_ROAM_PREF_PERSISTENT:
#ifdef CM_API_PERSISTENT_PREF
      return CM_ROAM_PREF_PERSISTENT;
#else
      return CM_ROAM_PREF_NO_CHANGE;
#endif

    default:
      return CM_ROAM_PREF_NO_CHANGE;
  }
}

/*=============================================================================
FUNCTION: OEMCM_SrvDomainPrefToAEECM

DESCRIPTION:
  Convert serving domain preference enumeration from DMSS to AEECM

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMServDomPref OEMCM_SrvDomainPrefToAEECM(cm_srv_domain_pref_e_type pref)
{
  switch(pref) {
    case CM_SRV_DOMAIN_PREF_NONE:
      return AEECM_SRV_DOMAIN_PREF_NONE;

    case CM_SRV_DOMAIN_PREF_CS_ONLY:
      return AEECM_SRV_DOMAIN_PREF_CS_ONLY;

    case CM_SRV_DOMAIN_PREF_PS_ONLY:
      return AEECM_SRV_DOMAIN_PREF_PS_ONLY;

    case CM_SRV_DOMAIN_PREF_CS_PS:
      return AEECM_SRV_DOMAIN_PREF_CS_PS;

    case CM_SRV_DOMAIN_PREF_ANY:
      return AEECM_SRV_DOMAIN_PREF_ANY;

    case CM_SRV_DOMAIN_PREF_NO_CHANGE:
      return AEECM_SRV_DOMAIN_PREF_NO_CHANGE;

    case CM_SRV_DOMAIN_PREF_PS_ATTACH:
      return AEECM_SRV_DOMAIN_PREF_PS_ATTACH;

    case CM_SRV_DOMAIN_PREF_PS_DETACH:
      return AEECM_SRV_DOMAIN_PREF_PS_DETACH;

#ifdef CM_API_PERSISTENT_PREF
    case CM_SRV_DOMAIN_PREF_PERSISTENT:
      return AEECM_SRV_DOMAIN_PREF_PERSISTENT;
#endif

    default:
      return AEECM_SRV_DOMAIN_PREF_NO_CHANGE;
  }
}

/*=============================================================================
FUNCTION: OEMCM_SrvDomainPrefToDMSS

DESCRIPTION:
  Convert serving domain preference enumeration from AEECM to DMSS

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static cm_srv_domain_pref_e_type OEMCM_SrvDomainPrefToDMSS(AEECMServDomPref pref)
{
  switch(pref) {
    case AEECM_SRV_DOMAIN_PREF_NONE:
      return CM_SRV_DOMAIN_PREF_NONE;

    case AEECM_SRV_DOMAIN_PREF_CS_ONLY:
      return CM_SRV_DOMAIN_PREF_CS_ONLY;

    case AEECM_SRV_DOMAIN_PREF_PS_ONLY:
      return CM_SRV_DOMAIN_PREF_PS_ONLY;

    case AEECM_SRV_DOMAIN_PREF_CS_PS:
      return CM_SRV_DOMAIN_PREF_CS_PS;

    case AEECM_SRV_DOMAIN_PREF_ANY:
      return CM_SRV_DOMAIN_PREF_ANY;

    case AEECM_SRV_DOMAIN_PREF_NO_CHANGE:
      return CM_SRV_DOMAIN_PREF_NO_CHANGE;

    case AEECM_SRV_DOMAIN_PREF_PS_ATTACH:
      return CM_SRV_DOMAIN_PREF_PS_ATTACH;

    case AEECM_SRV_DOMAIN_PREF_PS_DETACH:
      return CM_SRV_DOMAIN_PREF_PS_DETACH;

    case AEECM_SRV_DOMAIN_PREF_PERSISTENT:
#ifdef CM_API_PERSISTENT_PREF
      return CM_SRV_DOMAIN_PREF_PERSISTENT;
#else
      return CM_SRV_DOMAIN_PREF_NO_CHANGE;
#endif

    default:
      return CM_SRV_DOMAIN_PREF_NO_CHANGE;
  }
}

/*=============================================================================
FUNCTION: OEMCM_HybrPrefToAEECM

DESCRIPTION:
  Convert hybrid preference enumeration from DMSS to AEECM

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMHybrPref OEMCM_HybrPrefToAEECM(cm_hybr_pref_e_type pref)
{
  switch(pref) {
    case CM_HYBR_PREF_OFF:
      return AEECM_HYBR_PREF_OFF;

    case CM_HYBR_PREF_ON:
      return AEECM_HYBR_PREF_ON;

    case CM_HYBR_PREF_NO_CHANGE:
      return AEECM_HYBR_PREF_NO_CHANGE;

#ifdef CM_API_PERSISTENT_PREF
    case CM_HYBR_PREF_PERSISTENT:
      return AEECM_HYBR_PREF_PERSISTENT;
#endif

    default:
      return AEECM_HYBR_PREF_NO_CHANGE;
  }
}

/*=============================================================================
FUNCTION: OEMCM_HybrPrefToDMSS

DESCRIPTION:
  Convert hybrid preference enumeration from AEECM to DMSS

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static cm_hybr_pref_e_type OEMCM_HybrPrefToDMSS(AEECMHybrPref pref)
{
  switch(pref) {
    case AEECM_HYBR_PREF_OFF:
      return CM_HYBR_PREF_OFF;

    case AEECM_HYBR_PREF_ON:
      return CM_HYBR_PREF_ON;

    case AEECM_HYBR_PREF_NO_CHANGE:
      return CM_HYBR_PREF_NO_CHANGE;

    case AEECM_HYBR_PREF_PERSISTENT:
#ifdef CM_API_PERSISTENT_PREF
      return CM_HYBR_PREF_PERSISTENT;
#else
      return CM_HYBR_PREF_NO_CHANGE;
#endif

    default:
      return CM_HYBR_PREF_NO_CHANGE;
  }
}

/*=============================================================================
FUNCTION: OEMCM_PhErrorToAEECM

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMPhError OEMCM_PhErrorToAEECM(cm_ph_cmd_err_e_type error)
{
  AEECMPhError err = AEECM_PH_ERROR_NONE;

  switch(error) {
    case CM_PH_CMD_ERR_NOERR:
      err  = AEECM_PH_ERROR_NONE;
      break;

    case CM_PH_CMD_ERR_IN_USE_S:
      err = AEECM_PH_ERROR_IN_USE_S;
      break;

    case CM_PH_CMD_ERR_ONLINE_S:
      err = AEECM_PH_ERROR_ONLINE_S;
      break;

    case CM_PH_CMD_ERR_OPRT_MODE_S:
      err = AEECM_PH_ERROR_OPRT_MODE_S;
      break;

    case CM_PH_CMD_ERR_MODE_PREF_S:
      err = AEECM_PH_ERROR_MODE_PREF_S;
      break;

    case CM_PH_CMD_ERR_SYS_PREF_S:
      err = AEECM_PH_ERROR_SYS_PREF_S;
      break;

    case CM_PH_CMD_ERR_SYS_SEL_PREF_S:
      err = AEECM_PH_ERROR_SYS_SEL_PREF_S;
      break;

    case CM_PH_CMD_ERR_HYBR_PREF_S:
      err = AEECM_PH_ERROR_HYBR_PREF_S;
      break;

    case CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S:
      err = AEECM_PH_ERROR_SUBSCRIPTION_AVAILABLE_S;
      break;

    case CM_PH_CMD_ERR_SYS_NOT_IN_UZ:
      err = AEECM_PH_ERROR_SYS_NOT_IN_UZ;
      break;

    case CM_PH_CMD_ERR_CURRENT_NAM_IS_NOT_RUIM_NAM_S:
      err = AEECM_PH_ERROR_CURRENT_NAM_IS_NOT_RUIM_NAM_S;
      break;

#ifdef FEATURE_ALS
#error code not present
#endif

    case CM_PH_CMD_ERR_NO_BUF_L:
    case CM_PH_CMD_ERR_MC_COM_L:
    case CM_PH_CMD_ERR_OTHER:
    case CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE:
    case CM_PH_CMD_ERR_USER_PREFERRED_NETWORK_LIST_ACCESS:
      err = AEECM_PH_ERROR_OTHER;
      break;

    default:
      MSG_ERROR("Unexpected PH command error 0x%X", error, 0, 0);
      err = AEECM_PH_ERROR_OTHER;
      break;

  }

  return err;
}


/*=============================================================================
FUNCTION: OEMCM_CallErrorToAEECM

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMCallError OEMCM_CallErrorToAEECM(cm_call_cmd_err_e_type error)
{
  AEECMCallError err = AEECM_CALL_ERROR_NONE;

  switch (error) {
    case CM_CALL_CMD_ERR_OFFLINE_S:
      err = AEECM_CALL_ERROR_OFFLINE_S;
      break;

    case CM_CALL_CMD_ERR_CDMA_LOCK_S:
      err = AEECM_CALL_ERROR_CDMA_LOCK_S;
      break;

    case CM_CALL_CMD_ERR_FLASH_S:
      err = AEECM_CALL_ERROR_FLASH_S;
      break;

    case CM_CALL_CMD_ERR_ORIG_RES_S:
      err = AEECM_CALL_ERROR_ORIG_RES_S;
      break;

    case CM_CALL_CMD_ERR_NO_FUNDS_S:
      err = AEECM_CALL_ERROR_NO_FUNDS_S;
      break;

    case CM_CALL_CMD_ERR_EMERGENCY_MODE_S:
      err = AEECM_CALL_ERROR_EMERGENCY_MODE_S;
      break;

    case CM_CALL_CMD_ERR_IN_USE_S:
      err = AEECM_CALL_ERROR_IN_USE_S;
      break;

    case CM_CALL_CMD_ERR_SRV_TYPE_S:
      err = AEECM_CALL_ERROR_SRV_TYPE_S;
      break;

    case CM_CALL_CMD_ERR_CALL_TYPE_S:
      err = AEECM_CALL_ERROR_CALL_TYPE_S;
      break;

    case CM_CALL_CMD_ERR_CALL_STATE_S:
      err = AEECM_CALL_ERROR_CALL_STATE_S;
      break;

    case CM_CALL_CMD_ERR_SRV_STATE_S:
      err = AEECM_CALL_ERROR_SRV_STATE_S;
      break;

    case CM_CALL_CMD_ERR_NO_SO_S:
      err = AEECM_CALL_ERROR_NO_SO_S;
      break;

    case CM_CALL_CMD_ERR_ALERT_ANS_BYPASS_S:
      err = AEECM_CALL_ERROR_ALERT_ANS_BYPASS_S;
      break;

    case CM_CALL_CMD_ERR_UIM_NOT_PRESENT_S:
      err = AEECM_CALL_ERROR_UIM_NOT_PRESENT_S;
      break;

    case CM_CALL_CMD_ERR_BAD_PRIVACY_PREF_S:
      err = AEECM_CALL_ERROR_BAD_PRIVACY_PREF_S;
      break;

    case CM_CALL_CMD_ERR_NO_CALL_ID_S:
      err = AEECM_CALL_ERROR_NO_CALL_ID_S;
      break;

    case CM_CALL_CMD_ERR_CALL_ORIG_FAIL_S:
      err = AEECM_CALL_ERROR_CALL_ORIG_FAIL_S;
      break;

    case CM_CALL_CMD_ERR_INVALID_MODE_S:
      err = AEECM_CALL_ERROR_INVALID_MODE_S;
      break;

    case CM_CALL_CMD_ERR_INVALID_SIM_STATE_S:
      err = AEECM_CALL_ERROR_INVALID_SIM_STATE_S;
      break;

    case CM_CALL_CMD_ERR_NO_COLLOC_HDR:
      err = AEECM_CALL_ERROR_NO_COLLOC_HDR;
      break;

    case CM_CALL_CMD_ERR_NO_BUF_L:
    case CM_CALL_CMD_ERR_MC_COM_L:
    case CM_CALL_CMD_ERR_OTHER:
    case CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE:
      err = AEECM_CALL_ERROR_OTHER;
      break;

    case CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED:
      err = AEECM_CALL_ERROR_CALL_CONTROL_REJECTED;
      break;

    case CM_CALL_CMD_ERR_CLIENT_ID_P:
    case CM_CALL_CMD_ERR_CALL_ID_P:
    case CM_CALL_CMD_ERR_NUM_CALL_IDS_P:
    case CM_CALL_CMD_ERR_CALL_TYPE_P:
    case CM_CALL_CMD_ERR_SRV_TYPE_P:
    case CM_CALL_CMD_ERR_SRV_OPT_P:
    case CM_CALL_CMD_ERR_NO_NUM_P:
    case CM_CALL_CMD_ERR_NULL_NUM_P:
    case CM_CALL_CMD_ERR_NUM_P:
    case CM_CALL_CMD_ERR_NUM_LEN_P:
    case CM_CALL_CMD_ERR_NULL_ALPHA_P:
    case CM_CALL_CMD_ERR_ALPHA_LEN_P:
    case CM_CALL_CMD_ERR_MISMATCH_P:
    case CM_CALL_CMD_ERR_NULL_PARAM_P:
    case CM_CALL_CMD_ERR_ACTIVATE_CODE_P:
    case CM_CALL_CMD_ERR_CALL_SUPS_P:
    case CM_CALL_CMD_ERR_QOS_LEN_P:
    case CM_CALL_CMD_ERR_LAST_ACT_DATA_NET_P:
    default:
      /* Should not get these errors */
      err = AEECM_CALL_ERROR_OTHER;
      break;

    case CM_CALL_CMD_ERR_NOERR:
      err = AEECM_CALL_ERROR_NONE;
      break;
  }

  return err;
}

/*=============================================================================
FUNCTION: OEMCM_CallEventToAEECM

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMEvent OEMCM_CallEventToAEECM(cm_call_event_e_type event)
{
  AEECMEvent evt = AEECM_EVENT_NONE;

  switch (event) {
    case CM_CALL_EVENT_ORIG:
      evt = AEECM_EVENT_CALL_ORIG;
      break;

    case CM_CALL_EVENT_ANSWER:
      evt = AEECM_EVENT_CALL_ANSWER;
      break;

    case CM_CALL_EVENT_END:
    case CM_CALL_EVENT_END_REQ:
      evt = AEECM_EVENT_CALL_END;
      break;

    case CM_CALL_EVENT_INCOM:
      evt = AEECM_EVENT_CALL_INCOM;
      break;

    case CM_CALL_EVENT_CONNECT:
      evt = AEECM_EVENT_CALL_CONNECT;
      break;

    case CM_CALL_EVENT_SETUP_IND:
      evt = AEECM_EVENT_INTERNAL_SETUP_IND;
      break;

    case CM_CALL_EVENT_CONNECTED_NUM:
      evt = AEECM_EVENT_CALL_CONNECTED_NUM;
      break;

    case CM_CALL_EVENT_CALL_IS_WAITING:
      evt = AEECM_EVENT_CALL_IS_WAITING;
      break;

    case CM_CALL_EVENT_CALL_BARRED:
      evt = AEECM_EVENT_CALL_BARRED;
      break;

    case CM_CALL_EVENT_ORIG_FWD_STATUS:
      evt = AEECM_EVENT_CALL_ORIG_FWD_STATUS;
      break;

    case CM_CALL_EVENT_CALL_FORWARDED:
      evt = AEECM_EVENT_CALL_FORWARDED;
      break;

    case CM_CALL_EVENT_CALL_BEING_FORWARDED:
      evt = AEECM_EVENT_CALL_BEING_FORWARDED;
      break;

    case CM_CALL_EVENT_INCOM_FWD_CALL:
      evt = AEECM_EVENT_CALL_INCOM_FWD_CALL;
      break;

    case CM_CALL_EVENT_CALL_RESTRICTED:
      evt = AEECM_EVENT_CALL_RESTRICTED;
      break;

    case CM_CALL_EVENT_CALL_DEFLECTION:
      evt = AEECM_EVENT_CALL_DEFLECTION;
      break;

    case CM_CALL_EVENT_MNG_CALLS_CONF:
      evt = AEECM_EVENT_INTERNAL_MNG_CALLS_CONF;
      break;

#ifdef FEATURE_CCBS
#error code not present
#endif

    case CM_CALL_EVENT_SUPS:
      evt = AEECM_EVENT_CALL_OPS;
      break;

    case CM_CALL_EVENT_RAB_REL_IND:
      evt = AEECM_EVENT_CALL_ENTER_DORMANT;
      break;

    case CM_CALL_EVENT_RAB_REESTAB_IND:
      evt = AEECM_EVENT_CALL_EXIT_DORMANT;
      break;

    case CM_CALL_EVENT_SRV_OPT:
      evt = AEECM_EVENT_CALL_SRV_OPT;
      break;

    case CM_CALL_EVENT_PRIVACY:
      evt = AEECM_EVENT_CALL_PRIVACY;
      break;

    case CM_CALL_EVENT_CALLER_ID:
      evt = AEECM_EVENT_CALL_CALLER_ID;
      break;

    case CM_CALL_EVENT_ABRV_ALERT:
      evt = AEECM_EVENT_CALL_ABRV_ALERT;
      break;

    case CM_CALL_EVENT_ABRV_REORDER:
      evt = AEECM_EVENT_CALL_ABRV_REORDER;
      break;

    case CM_CALL_EVENT_ABRV_INTERCEPT:
      evt = AEECM_EVENT_CALL_ABRV_INTERCEPT;
      break;

    case CM_CALL_EVENT_SIGNAL:
      evt = AEECM_EVENT_CALL_SIGNAL;
      break;

    case CM_CALL_EVENT_DISPLAY:
      evt = AEECM_EVENT_CALL_DISPLAY;
      break;

    case CM_CALL_EVENT_CALLED_PARTY:
      evt = AEECM_EVENT_CALL_CALLED_PARTY;
      break;

    case CM_CALL_EVENT_EXT_DISP:
      evt = AEECM_EVENT_INTERNAL_EXT_DISP;
      break;

    case CM_CALL_EVENT_CUG_INFO_RECEIVED:
      evt = AEECM_EVENT_CALL_CUG_INFO_RECEIVED;
      break;

    case CM_CALL_EVENT_CNAP_INFO_RECEIVED:
      evt = AEECM_EVENT_CALL_CNAP_INFO_RECEIVED;
      break;

    case CM_CALL_EVENT_EMERGENCY_FLASHED:
      evt = AEECM_EVENT_CALL_EMERGENCY_FLASHED;
      break;

    case CM_CALL_EVENT_PROGRESS_INFO_IND:
      evt = AEECM_EVENT_CALL_PROGRESS_INFO_IND;
      break;

    case CM_CALL_EVENT_TRANSFERRED_CALL:
      evt = AEECM_EVENT_CALL_TRANSFERRED;
      break;

    case CM_CALL_EVENT_LINE_CTRL:
      evt = AEECM_EVENT_CALL_LINE_CTRL;
      break;

#ifdef FEATURE_CCBS
#error code not present
#endif

    case CM_CALL_EVENT_CALL_ON_HOLD:
      evt = AEECM_EVENT_CALL_ON_HOLD;
    break;
    case CM_CALL_EVENT_CALL_RETRIEVED:
      evt = AEECM_EVENT_CALL_RETRIEVED;
    break;

    case CM_CALL_EVENT_NSS_REL_REC:
       evt = AEECM_CALL_EVENT_NSS_REL_REC;
     break;

    case CM_CALL_EVENT_CALL_CONF:
    case CM_CALL_EVENT_PRIVACY_PREF:
    case CM_CALL_EVENT_SETUP_RES:
    case CM_CALL_EVENT_NDSS_START:
    case CM_CALL_EVENT_NDSS_CONNECT:
    case CM_CALL_EVENT_EXT_BRST_INTL:
    case CM_CALL_EVENT_NSS_CLIR_REC:
    case CM_CALL_EVENT_NSS_AUD_CTRL:
    case CM_CALL_EVENT_L2ACK_CALL_HOLD:
    case CM_CALL_EVENT_PDP_ACTIVATE_IND:
    case CM_CALL_EVENT_PDP_ACTIVATE_RES:
    case CM_CALL_EVENT_PDP_MODIFY_REQ:
    case CM_CALL_EVENT_PDP_MODIFY_REJ:
    case CM_CALL_EVENT_PDP_MODIFY_CONF:
    case CM_CALL_EVENT_PDP_MODIFY_IND:
    case CM_CALL_EVENT_RAB_REESTAB_REQ:
    case CM_CALL_EVENT_RAB_REESTAB_CONF:
    case CM_CALL_EVENT_RAB_REESTAB_REJ:
    case CM_CALL_EVENT_RAB_REESTAB_FAIL:
    case CM_CALL_EVENT_EXIT_TC:
    default:
      /* Not sent to BREW by ICM */
      break;
  }

  return evt;
}

/*=============================================================================
FUNCTION: OEMCM_PhEventToAEECM

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMEvent OEMCM_PhEventToAEECM(cm_ph_event_e_type event)
{
  AEECMEvent evt = AEECM_EVENT_NONE;

  switch(event) {
    case CM_PH_EVENT_OPRT_MODE:
      evt = AEECM_EVENT_PH_OPRT_MODE;
      break;

    case CM_PH_EVENT_TEST_CONTROL_TYPE:
      evt = AEECM_EVENT_INTERNAL_TEST_CONTROL;
      break;

    case CM_PH_EVENT_SYS_SEL_PREF:
      evt = AEECM_EVENT_PH_SYSTEM_PREF;
      break;

    case CM_PH_EVENT_ANSWER_VOICE:
      evt = AEECM_EVENT_PH_ANSWER_VOICE;
      break;

    case CM_PH_EVENT_NAM_SEL:
      evt = AEECM_EVENT_PH_NAM_SEL;
      break;

    case CM_PH_EVENT_CURR_NAM:
      evt = AEECM_EVENT_PH_CURR_NAM;
      break;

    case CM_PH_EVENT_IN_USE_STATE:
      evt = AEECM_EVENT_PH_IN_USE_STATE;
      break;

    case CM_PH_EVENT_CDMA_LOCK_MODE:
      evt = AEECM_EVENT_PH_CDMA_LOCK_MODE;
      break;

    case CM_PH_EVENT_MAINTREQ:
      evt = AEECM_EVENT_PH_MAINTREQ;
      break;

    case CM_PH_EVENT_STANDBY_SLEEP:
      evt = AEECM_EVENT_PH_STANDBY_SLEEP;
      break;

    case CM_PH_EVENT_STANDBY_WAKE:
      evt = AEECM_EVENT_PH_STANDBY_WAKE;
      break;

    case CM_PH_EVENT_INFO_AVAIL:
      evt = AEECM_EVENT_PH_INFO_AVAIL;
      break;

    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_CHANGED:
      evt = AEECM_EVENT_PH_SUBSCRIPTION;
      break;

    case CM_PH_EVENT_FUNDS_LOW:
      evt = AEECM_EVENT_PH_FUNDS_LOW;
      break;

    case CM_PH_EVENT_WAKEUP_FROM_STANDBY:
      evt = AEECM_EVENT_INTERNAL_WAKEUP;
      break;

    case CM_PH_EVENT_NVRUIM_CONFIG_CHANGED:
      evt = AEECM_EVENT_PH_NVRUIM_CONFIG;
      break;

    case CM_PH_EVENT_PREFERRED_NETWORKS_SET:
      evt = AEECM_EVENT_INTERNAL_PREF_NW_SET;
      break;

    case CM_PH_EVENT_DDTM_PREF:
      evt = AEECM_EVENT_INTERNAL_DDTM_PREF;
      break;

    case CM_PH_EVENT_RESET_ACM_COMPLETED:
      evt = AEECM_EVENT_PH_ACM_RESET;
      break;

    case CM_PH_EVENT_SET_ACMMAX_COMPLETED:
      evt = AEECM_EVENT_PH_ACMMAX_SET;
      break;

#ifdef FEATURE_ALS
#error code not present
#endif

    case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
    case CM_PH_EVENT_PREFERRED_NETWORKS_CONF:
      /* Handled separately */

    case CM_PH_EVENT_PS_ATTACH_FAILED:
      /* TODO */

    /* case CM_PH_EVENT_UZ_CHANGED: */
    case CM_PH_EVENT_INFO:
    case CM_PH_EVENT_PACKET_STATE:
    default:
      /* Not sent to BREW by ICM */
      break;
  }

  return evt;
}

/*=============================================================================
FUNCTION: OEMCM_SSEventToAEECM

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMEvent OEMCM_SSEventToAEECM(cm_ss_event_e_type event)
{
  AEECMEvent evt = AEECM_EVENT_NONE;

  switch(event) {
    case CM_SS_EVENT_SRV_CHANGED:
      evt = AEECM_EVENT_SS_SRV_CHANGED;
      break;

    case CM_SS_EVENT_RSSI:
      evt = AEECM_EVENT_SS_RSSI;
      break;

    case CM_SS_EVENT_HDR_RSSI:
      evt = AEECM_EVENT_SS_HDR_RSSI;
      break;

#ifdef FEATURE_WLAN
#error code not present
#endif

    case CM_SS_EVENT_INFO:
      evt = AEECM_EVENT_SS_INFO;
      break;

    case CM_SS_EVENT_REG_SUCCESS:
    case CM_SS_EVENT_REG_FAILURE:
    default:
      break;
  }

  return evt;
}

/*=============================================================================
FUNCTION: OEMCM_InbandEventToAEECM

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMEvent OEMCM_InbandEventToAEECM(cm_inband_event_e_type inband_event)
{
  AEECMEvent evt = AEECM_EVENT_NONE;

  switch (inband_event) {
    case CM_INBAND_EVENT_FWD_BURST_DTMF:
      evt = AEECM_EVENT_INBAND_FWD_BURST_DTMF;
      break;

    case CM_INBAND_EVENT_FWD_START_CONT_DTMF:
      evt = AEECM_EVENT_INBAND_FWD_START_CONT_DTMF;
      break;

    case CM_INBAND_EVENT_FWD_STOP_CONT_DTMF:
      evt = AEECM_EVENT_INBAND_FWD_STOP_CONT_DTMF;
      break;

    case CM_INBAND_EVENT_REV_BURST_DTMF:
    case CM_INBAND_EVENT_REV_START_CONT_DTMF:
    case CM_INBAND_EVENT_REV_STOP_CONT_DTMF:
    default:
      break;
  }

  return evt;
}

#ifdef FEATURE_MBMS
/*=============================================================================
FUNCTION: OEMCM_MBMSEventToAEECM

DESCRIPTION:
   Converts CM MBMS related events to the equivalent AEECM type.

=============================================================================*/
static AEECMEvent OEMCM_MBMSEventToAEECM(cm_mbms_event_e_type event)
{
  AEECMEvent evt = AEECM_EVENT_NONE;

  switch(event) {
    case CM_MBMS_EVENT_GENERIC_RPT:
      evt = AEECM_EVENT_MBMS_GENERIC_RPT;
      break;
    case CM_MBMS_EVENT_GENERIC_CMD:
      evt = AEECM_EVENT_MBMS_GENERIC_CMD;
      break;
    default:
      MSG_ERROR("OEMCM_MBMSEventToAEECM: Invalid event %d",event,0,0);
      break;
  }

  return evt;
}
#endif //#ifdef FEATURE_MBMS


/*=============================================================================
FUNCTION: OEMCM_SSMaskToAEECM

DESCRIPTION:


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMCM_SSMaskToAEECM(uint32 cm_mask)
{
  uint32 mask = 0;

  if (cm_mask & CM_SS_SRV_STATUS_MASK)
    mask |= AEECM_SS_SRV_STATUS_MASK;

  if (cm_mask & CM_SS_SRV_DOMAIN_MASK)
    mask |= AEECM_SS_SRV_DOMAIN_MASK;

  if (cm_mask & CM_SS_SRV_CAPABILITY_MASK)
    mask |= AEECM_SS_SRV_CAPABILITY_MASK;

  if (cm_mask & CM_SS_SYS_MODE_MASK)
    mask |= AEECM_SS_SYS_MODE_MASK;

  if (cm_mask & CM_SS_ROAM_STATUS_MASK)
    mask |= AEECM_SS_ROAM_STATUS_MASK;

  if (cm_mask & CM_SS_SYS_ID_MASK)
    mask |= AEECM_SS_SYS_ID_MASK;

  if (cm_mask & CM_SS_PLMN_FORBIDDEN_MASK)
    mask |= AEECM_SS_PLMN_FORBIDDEN_MASK;

  if (cm_mask & CM_SS_MOBILITY_MGMT_MASK)
    mask |= AEECM_SS_MOBILITY_MGMT_MASK;

  if (cm_mask & CM_SS_SRV_IND_MASK)
    mask |= AEECM_SS_SRV_IND_MASK;

  if (cm_mask & CM_SS_UZ_CHANGED_MASK)
    mask |= AEECM_SS_UZ_MASK;

  if (cm_mask & CM_SS_SIM_STATE_MASK)
    mask |= AEECM_SS_SIM_STATE_MASK;

  if (cm_mask & CM_SS_HDR_SRV_STATUS_MASK)
    mask |= AEECM_SS_HDR_SRV_STATUS_MASK;

  if (cm_mask & CM_SS_HDR_ROAM_STATUS_MASK)
    mask |= AEECM_SS_ROAM_STATUS_MASK;

  return mask;
}

#ifdef FEATURE_WLAN
#error code not present
#endif // FEATURE_WLAN

/*=============================================================================
FUNCTION: OEMCM_SysDSTToAEECMDST

DESCRIPTION:
  Convert sys_daylight_saving_adj_e_type to AEECMDSTAdj

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEECMDSTAdj OEMCM_SysDSTToAEECMDST(sys_daylight_saving_adj_e_type sys_dst)
{
  AEECMDSTAdj aeecm_dst = AEECM_DAY_SAV_ADJ_NO_ADJUSTMENT;

  switch (sys_dst) {
    case SYS_DAY_SAV_ADJ_NO_ADJUSTMENT:
      aeecm_dst = AEECM_DAY_SAV_ADJ_NO_ADJUSTMENT;
      break;

    case SYS_DAY_SAV_ADJ_PLUS_ONE_HOUR:
      aeecm_dst = AEECM_DAY_SAV_ADJ_PLUS_ONE_HOUR;
      break;

    case SYS_DAY_SAV_ADJ_PLUS_TWO_HOUR:
      aeecm_dst = AEECM_DAY_SAV_ADJ_PLUS_TWO_HOUR;
      break;

    default:
      MSG_ERROR("Convertion from sys_daylight_saving_adj_e_type not supported", 0, 0, 0);
      break;
  }

  return aeecm_dst;
}

#ifdef FEATURE_USRSYSCLOCK
/*=============================================================================
FUNCTION: OEMCM_HandleSsMobilityMgmt

DESCRIPTION:
  Handle a change in mobility management. In particular, this function will update the user clock
  and/or timezone with the value received from the network.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleSsMobilityMgmt( const sys_mm_information_s_type *mm_info_ptr, AEECMMMInfo *mmInfo)
{
/* chosen to be outside the range defined in sys_time_zone_is_valid */
#define INVALID_TZ 97
/* decade of base time 1/6/1980 */
#define BASE_DECADE      80
#define YEAR_2000        2000
#define YEAR_1900        1900

#define SECS_PER_HOUR    3600

   IShell *pIShell = NULL;
   IConfig *pIConfig = NULL;
   IUsrSysClock *pISysClock = NULL;

   boolean update_user_time = FALSE;

   int ret;
   JulianType utc_in_julian;
   uint32 utc_in_secs = 0;
   uint32 mm_info_utc_in_secs = 0;
   boolean bIsDST = FALSE;
   boolean bIsDSTAvail = FALSE;
   uint32 daylight_saving_adj_in_secs = 0;
   AEETimeZoneType timeZoneID = 0;


  mmInfo->plmn_avail                = mm_info_ptr->plmn_avail;
  mmInfo->full_name_avail           = mm_info_ptr->full_name_avail;
  mmInfo->short_name_avail          = mm_info_ptr->short_name_avail;
  mmInfo->time_and_time_zone_avail  = mm_info_ptr->univ_time_and_time_zone_avail;
  mmInfo->time_zone_avail           = mm_info_ptr->time_zone_avail;
  mmInfo->lsa_identity_avail        = mm_info_ptr->lsa_identity_avail;
  mmInfo->daylight_saving_adj_avail = mm_info_ptr->daylight_saving_adj_avail;
  mmInfo->daylight_saving_adj       = OEMCM_SysDSTToAEECMDST(mm_info_ptr->daylight_saving_adj);

  if (mm_info_ptr->plmn_avail)
    (void) MEMCPY(&mmInfo->plmn, &mm_info_ptr->plmn, sizeof(AEECMPLMNID));

  if (mm_info_ptr->full_name_avail)
    OEMCM_CopyInFullNWName(&mmInfo->full_name, &mm_info_ptr->full_name);

  if (mm_info_ptr->short_name_avail)
    OEMCM_CopyInShortNWName(&mmInfo->short_name, &mm_info_ptr->short_name);

  if (mm_info_ptr->lsa_identity_avail) {
    mmInfo->lsa_identity.type = (AEECMLSAIdentityType)
                                 mm_info_ptr->lsa_identity.type;
    (void) MEMCPY(mmInfo->lsa_identity.identity,
                  mm_info_ptr->lsa_identity.identity, 3);
  }

   /* Universal Time and Time Zone IE set */
   if (mm_info_ptr->univ_time_and_time_zone_avail) {

     /* Check whether the year is from YEAR 1980 onwards */
     utc_in_julian.wYear   = mm_info_ptr->univ_time_and_time_zone.year;
     // cannot distinguish between 1987 ? 2087 ?
     if ( utc_in_julian.wYear < BASE_DECADE )
     {
        utc_in_julian.wYear += YEAR_2000;
     }
     else
     {
        utc_in_julian.wYear += YEAR_1900;
     }

     utc_in_julian.wMonth  = mm_info_ptr->univ_time_and_time_zone.month;
     utc_in_julian.wDay    = mm_info_ptr->univ_time_and_time_zone.day;
     utc_in_julian.wHour   = mm_info_ptr->univ_time_and_time_zone.hour;
     utc_in_julian.wMinute = mm_info_ptr->univ_time_and_time_zone.minute;
     utc_in_julian.wSecond = mm_info_ptr->univ_time_and_time_zone.second;
     utc_in_julian.wWeekDay = 0;

     //Staying consistent with OEMCM_CopyInMMInfo
     //Add DST adjustments into the Julian structure of mmInfo
     mm_info_utc_in_secs = utc_in_secs = JULIANTOSECONDS(&utc_in_julian);

     if (mm_info_ptr->daylight_saving_adj_avail) {
       uint32     adj;

       switch (mm_info_ptr->daylight_saving_adj) {
         case SYS_DAY_SAV_ADJ_PLUS_ONE_HOUR:
           adj = 1;
           break;
         case SYS_DAY_SAV_ADJ_PLUS_TWO_HOUR:
           adj = 2;
           break;
         default:
           adj = 0;
           break;
       }

       mm_info_utc_in_secs += (adj * 3600);
     }

     GETJULIANDATE(mm_info_utc_in_secs, &utc_in_julian);

     (void) MEMCPY(&mmInfo->time_and_time_zone.time, &utc_in_julian,
                  sizeof(JulianType));

     mmInfo->time_and_time_zone.time_zone = (AEECMTimeZone)
                        mm_info_ptr->univ_time_and_time_zone.time_zone;
   }

   if (mm_info_ptr->time_zone_avail)
     mmInfo->time_zone = (AEECMTimeZone) mm_info_ptr->time_zone;

   pIShell = AEE_GetShell();
   if (!pIShell) {
      return;
   }
   if (ISHELL_CreateInstance(pIShell, AEECLSID_CONFIG, (void **)&pIConfig)
      != SUCCESS)
   {
      return;
   }
   if (pIConfig == NULL) return;

   if (ICONFIG_GetItem(pIConfig, CFGI_AUTO_TIME_ENABLE, (void *)&update_user_time,
      sizeof(update_user_time)) != SUCCESS) {
      update_user_time = FALSE;
   }

   ICONFIG_Release(pIConfig);
   pIConfig = NULL;

   if (FALSE == update_user_time) {
      /* User does not want to automatically update the time. */
      return;
   }

   do {

      ret = ISHELL_CreateInstance(pIShell, AEECLSID_USRSYSCLOCK, (void **)&pISysClock);
      if (SUCCESS != ret) {
         MSG_HIGH("Failed creating IUsrSysClock, retVal %d", ret, 0, 0);
         break;
      }

      if (mm_info_ptr->univ_time_and_time_zone_avail) {
         /*  Update user time, this will set the offset from the
             secure time
         */
         ret = IUSRSYSCLOCK_SetTimeS(pISysClock, utc_in_secs);
         if (SUCCESS != ret) {
            MSG_HIGH("Failed set time on IUsrSysClock, retVal %d", ret, 0, 0);
            break;
         }
      }

      /* Check if DST IE is present in the MM info */
      if ((mm_info_ptr->daylight_saving_adj_avail == TRUE) &&
          (mm_info_ptr->daylight_saving_adj != SYS_DAY_SAV_ADJ_NO_ADJUSTMENT)) {
         bIsDST = TRUE;
      }

      if (mm_info_ptr->daylight_saving_adj_avail == TRUE) {
         bIsDSTAvail = TRUE;
         daylight_saving_adj_in_secs = mm_info_ptr->daylight_saving_adj * SECS_PER_HOUR;
      }

      if (mm_info_ptr->univ_time_and_time_zone_avail == TRUE)
      {
         /* Update user time zone */
         ret = IUSRSYSCLOCK_SetNetworkTimeZone(pISysClock, TRUE,
                                            mm_info_ptr->univ_time_and_time_zone.time_zone * 15 * 60,
                                            bIsDST);
         if (SUCCESS != ret)
         {
            MSG_HIGH("Failed set network time zone, ret %d", ret, 0, 0);
            break;
         }
         if (bIsDSTAvail == TRUE)
         {
           ret = IUSRSYSCLOCK_ConvertOffsetToTimeZone(pISysClock,
             mm_info_ptr->univ_time_and_time_zone.time_zone * 15 * 60,
             daylight_saving_adj_in_secs, bIsDSTAvail, &timeZoneID);
         }
         else
         {
           ret = IUSRSYSCLOCK_ConvertOffsetToTimeZone(pISysClock,
             mm_info_ptr->univ_time_and_time_zone.time_zone * 15 * 60,
             0, bIsDSTAvail, &timeZoneID);
         }

         ret = IUSRSYSCLOCK_SetTimeZone(pISysClock, timeZoneID);

      }
      else if (mm_info_ptr->time_zone_avail == TRUE)
      {
         /* Update user time zone */
         ret = IUSRSYSCLOCK_SetNetworkTimeZone(pISysClock, TRUE,
                                       mm_info_ptr->time_zone * 15 * 60, bIsDST);
         if (SUCCESS != ret)
         {
            MSG_HIGH("Failed set network time zone, ret %d", ret, 0, 0);
            break;
         }

         if (bIsDSTAvail == TRUE)
         {
           ret = IUSRSYSCLOCK_ConvertOffsetToTimeZone(pISysClock,
             mm_info_ptr->time_zone * 15 * 60,
             daylight_saving_adj_in_secs, bIsDSTAvail, &timeZoneID);
         }
         else
         {
           ret = IUSRSYSCLOCK_ConvertOffsetToTimeZone(pISysClock,
             mm_info_ptr->time_zone * 15 * 60,
             0, bIsDSTAvail, &timeZoneID);
         }
         ret = IUSRSYSCLOCK_SetTimeZone(pISysClock, timeZoneID);
      }

   } while(FALSE);

   if (NULL != pISysClock) {
      (void)IUSRSYSCLOCK_Release(pISysClock);
      pISysClock = NULL;
   }

   return;
}
#endif // FEATURE_USRSYSCLOCK
#if defined (FEATURE_ACM)

/*=============================================================================
FUNCTION: OEMCM_ACMCreateInstance

DESCRIPTION: Create a new instance for APPRESCTL

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMCM_ACMCreateInstance(ICM_ACMInfo * pAcmInfo)
{
#if defined (FEATURE_ODM_UI)
  int                       ret     = SUCCESS;
  IShell *                  pIShell = AEE_GetShell();
  PACONTEXT                 pac     = NULL;

#ifdef FEATURE_BREW_3_0
  pac = AEE_EnterAppContext(NULL);
#else
  pac = AEE_SetAppContext(NULL);
#endif  // FEATURE_BREW_3_0

  /* Create AppResCtl object */
  ret=ISHELL_CreateInstance(pIShell,AEECLSID_APPRESCTL,(void**)(&pAcmInfo->m_pAppResCtl));
#ifdef FEATURE_BREW_3_0
   AEE_LeaveAppContext(pac);
#else
   AEE_SetAppContext(pac);
#endif // FEATURE_BREW_3_0

  if(ret != SUCCESS)
  {
    MSG_ERROR("ISHELL_CreateInstance failed. RC=%d",ret,0,0);
    pAcmInfo->m_pAppResCtl = NULL;
    return EFAILED;
  }

  MSG_LOW("ISHELL_CreateInstance success. Ptr=%x",pAcmInfo->m_pAppResCtl,0,0);
#endif // FEATURE_ODM_UI
  return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_ACMAcquire

DESCRIPTION: Acquire resources with ACM with specified reasonCode

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMCM_ACMAcquire(ICM * pMe, ICM_ACMInfo * pAcmInfo, uint32 reasonCode)
{
  uint32                    nRet;
#if defined (FEATURE_ODM_UI)
  uint32                    resctl_status;
#endif // FEATURE_ODM_UI
  ACM_resource_state_type   pmgr_status;
  PFNNOTIFY                 pcb = &OEMCM_ACMChangeNotify;
  uint16                    num = 0;
  AEECMCallID  callIDs[OEMCM_MAX_NUM_CALLS];

  /* Check if anyone is using the resources */
  num = OEMCM_GetActiveCallIDs(pMe,
                               (AEECM_CALL_TYPE_VOICE | AEECM_CALL_TYPE_EMERGENCY),
                               (AEECM_CALL_STATE_ORIG | AEECM_CALL_STATE_INCOM | AEECM_CALL_STATE_CONV
                                    | AEECM_CALL_STATE_ONHOLD | AEECM_CALL_STATE_DORMANT),
                               callIDs,
                               sizeof(callIDs));

  MSG_HIGH("OEMCM_ACMAcquire: Number of active calls = %d", num, 0, 0);

  if (1 == num)
  {
    // Start acquiring resource
    MSG_HIGH("OEMCM_ACMAcquire: Start acquiring ACM resources", 0, 0, 0);
#if defined (FEATURE_ODM_UI)
    if((nRet = IAPPRESCTL_Acquire(pAcmInfo->m_pAppResCtl, reasonCode, &resctl_status)) != SUCCESS)
      return nRet;

    if( resctl_status == RESCTL_STATUS_BUSY )
    {
      MSG_ERROR("IAPPRESCTL_Acquire failed with RESCTL_STATUS_BUSY",0,0,0);
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
    }

    MSG_HIGH("IAPPRESCTL_Acquire succeded",0,0,0);

    // Fill in call back information
    pAcmInfo->m_acm_callback_data_type.state = ACM_RESOURCE_UNKNOWN;
    pAcmInfo->m_acm_callback_data_type.pdata = pAcmInfo->m_pCBInfo;
    if (reasonCode == ACM_MOBILE_ORIGINATED_VOICE_CALL)
    {
      pcb = &OEMCM_ACM_MobileOriginatedCallChangeNotify;
    }
    else if (reasonCode == ACM_NETWORK_ORIGINATED_VOICE_CALL)
    {
      pcb = &OEMCM_ACM_NetworkOriginatedCallChangeNotify;
    }

    // Register callback for resouce state change
    nRet = OEMACM_StateMgr_OnStatusChangeNotify(pAcmInfo->m_pAppResCtl, pcb,
                                          &pAcmInfo->m_acm_callback_data_type);

    if(nRet != SUCCESS)
    {
      MSG_ERROR("OEMACM_OnStatusChangeNotify failed with RC=%d",nRet,0,0);
      OEMCM_ACMFree (pMe, pAcmInfo);
      return nRet;
    }

    // Get resource state from ACM state manager
    if((nRet = OEMACM_StateMgr_GetResourceState(pAcmInfo->m_pAppResCtl,&pmgr_status)) != SUCCESS)
    {
      MSG_ERROR("OEMACM_StateMgr_GetResourceState failed with RC=%d",nRet,0,0);
      OEMCM_ACMFree (pMe, pAcmInfo);
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
    }

    if(pmgr_status == ACM_RESOURCE_ACQUIRED)
    {
      // Immediate Acquire
      pAcmInfo->m_uAcmRes_State = OEMCM_ACM_ACQUIRED;
      MSG_ERROR("OEMACM_GetResourceState returned with Immediate Acquire",0,0,0);
    }
    else
    {
      // Delayed Acquire
      pAcmInfo->m_uAcmRes_State = OEMCM_ACM_ACQUIRING;
      MSG_HIGH("OEMACM_GetResourceState returned with Delayed Acquire",0,0,0);
      return SUCCESS;
    }
#else // !FEATURE_ODM_UI
    nRet = OEMACM_PriorityMgr_Acquire (pMe, reasonCode, &pmgr_status);
    if(nRet != SUCCESS)
    {
      return nRet;
    }

    if (pmgr_status == ACM_RESOURCE_BUSY)
    {
      MSG_ERROR("CM: OEMACM_PriorityMgr_Acquire failed with ACM_RESOURCE_BUSY",0,0,0);
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
    }

    // ACM_RESOURCE_ACQUIRING or ACM_RESOURCE_ACQUIRED
    MSG_HIGH("CM: OEMACM_PriorityMgr_Acquire succeded",0,0,0);

    // Fill in call back information
    pAcmInfo->m_acm_callback_data_type.state = ACM_RESOURCE_UNKNOWN;
    pAcmInfo->m_acm_callback_data_type.pdata = pAcmInfo->m_pCBInfo;
    if (reasonCode == ACM_MOBILE_ORIGINATED_VOICE_CALL)
    {
      pcb = &OEMCM_ACM_MobileOriginatedCallChangeNotify;
    }
    else if (reasonCode == ACM_NETWORK_ORIGINATED_VOICE_CALL)
    {
      pcb = &OEMCM_ACM_NetworkOriginatedCallChangeNotify;
    }

    // Register callback for resouce state change
    nRet = OEMACM_StateMgr_OnStatusChangeNotify(pMe, pcb,
                                          &pAcmInfo->m_acm_callback_data_type);

    if(nRet != SUCCESS)
    {
      MSG_ERROR("OEMACM_OnStatusChangeNotify failed with RC=%d",nRet,0,0);
      OEMCM_ACMFree (pMe, pAcmInfo);
      return nRet;
    }

    if(pmgr_status == ACM_RESOURCE_ACQUIRED)
    {
      // Immediate Acquire
      pAcmInfo->m_uAcmRes_State = OEMCM_ACM_ACQUIRED;
      MSG_ERROR("OEMACM_GetResourceState returned with Immediate Acquire",0,0,0);
    }
    else
    {
      // Delayed Acquire
      pAcmInfo->m_uAcmRes_State = OEMCM_ACM_ACQUIRING;
      MSG_HIGH("OEMACM_GetResourceState returned with Delayed Acquire",0,0,0);
      return SUCCESS;
    }
#endif // FEATURE_ODM_UI
  }
  else
  {
    // Call already in progress, so ICM has resource
    pAcmInfo->m_uAcmRes_State = OEMCM_ACM_ACQUIRED;
  }

  MSG_HIGH ("New OEMCM ACM state = %d",pAcmInfo->m_uAcmRes_State,0,0);

  if (pAcmInfo->m_uAcmRes_State == OEMCM_ACM_ACQUIRED)
  {
    // Locals for Network originated calls
    struct OEMCM_ACM_MobileOriginatedCallInfo * ACM_MOCallInfo;

    // Locals for Mobile originated calls
    struct OEMCM_ACM_NetworkOriginatedCallInfo * ACM_NOCallInfo;

    switch (reasonCode)
    {
      case ACM_MOBILE_ORIGINATED_VOICE_CALL:
        // Notify apps
        ACM_MOCallInfo = (OEMCM_ACM_MobileOriginatedCallInfo *)(pAcmInfo->m_pCBInfo);
        MSG_HIGH ("Immediate acquire for MO Call",0,0,0);
        OEMCM_CallOrig_Notify(ACM_MOCallInfo->notify_info);
      break;
      case ACM_NETWORK_ORIGINATED_VOICE_CALL:
        // Invoke CM to answer call
        ACM_NOCallInfo = (OEMCM_ACM_NetworkOriginatedCallInfo *)(pAcmInfo->m_pCBInfo);
        MSG_HIGH ("Immediate acquire for MT Call",0,0,0);
        OEMCM_CallCM_AnswerCall(OEMCM_DMSSCallErrCB,
                                ACM_NOCallInfo->data_block_ptr,
                                ACM_NOCallInfo->client_id,
                                ACM_NOCallInfo->call_id,
                                &ACM_NOCallInfo->answer_params);
        break;
      default:
        // Do nothing
        break;
    }
  }

  return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_ACMFree

DESCRIPTION: Release resources with ACM for call_id

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMCM_ACMFree(ICM * pMe, ICM_ACMInfo * pAcmInfo)
{
#if defined (FEATURE_ODM_UI)
  int ret = 0;
#endif // FEATURE_ODM_UI
  uint16 num = 0;
  AEECMCallID  callIDs[OEMCM_MAX_NUM_CALLS];

  MSG_HIGH("OEMCM_ACMFree: Start freeing ACM resources", 0, 0, 0);

  /* Check if anyone is using the resources.  Using TRUE, since pMe may have be destroyed */
  num = OEMCM_GetActiveCallIDs((ICM *) TRUE,
                               (AEECM_CALL_TYPE_VOICE | AEECM_CALL_TYPE_EMERGENCY),
                               (AEECM_CALL_STATE_CONV | AEECM_CALL_STATE_ONHOLD
                                    | AEECM_CALL_STATE_DORMANT),
                               callIDs,
                               sizeof(callIDs));

  MSG_HIGH("OEMCM_ACMFree: Number of active calls = %d", num, 0, 0);

  if (0 == num)
  {
#if defined (FEATURE_ODM_UI)
    /* Release resources with ACM */
    MSG_LOW("OEMCM_Release Releasing AppResCtl object %x",pAcmInfo->m_pAppResCtl,0,0);

    if (pAcmInfo->m_pAppResCtl != NULL)
    {
      ret=IAPPRESCTL_Free(pAcmInfo->m_pAppResCtl, NULL);
    }

    if (ret != SUCCESS)
    {
       MSG_ERROR("OEMCM_Release failed to release AppResCtl object with RC=%d",ret,0,0);
       return ret;
    }

#else
    OEMACM_PriorityMgr_Free (pMe);
#endif // FEATURE_ODM_UI

    pAcmInfo->m_uAcmRes_State = OEMCM_ACM_FREE;
    //OEMCM_FREE (pAcmInfo->m_pCBInfo);
    MSG_HIGH("OEMCM_Release Releasing AppResCtl object done",0,0,0);
  }

  return SUCCESS;

}


/*=============================================================================
FUNCTION: OEMCM_ACMChangeNotify

DESCRIPTION: Default callback for ACM status change notify


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_ACMChangeNotify(void *p)
{
    ACM_Callback_data_type *cbdata = (ACM_Callback_data_type*)p;
    ACM_resource_state_type state = cbdata->state;

    switch(state)
    {
        case ACM_RESOURCE_ACQUIRED:
            MSG_HIGH("OEMCM_ACMChangeNotify received ACM_RESOURCE_ACQUIRED",0,0,0);
            break;

        default:
            MSG_HIGH("OEMCM_ACMChangeNotify received resource state=%d",state,0,0);
            break;
    }

}


/*=============================================================================
FUNCTION: OEMCM_ACM_NetworkOriginatedCallChangeNotify

DESCRIPTION: ACM status change notify CB for Network Originated call

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_ACM_NetworkOriginatedCallChangeNotify(void *p)
{
    ACM_Callback_data_type *cbdata = (ACM_Callback_data_type*)p;
    ACM_resource_state_type state = cbdata->state;
    OEMCM_ACM_NetworkOriginatedCallInfo * noCBInfo = (OEMCM_ACM_NetworkOriginatedCallInfo *) cbdata->pdata;

    MSG_HIGH("OEMCM: In callback of delayed acquire for MT call. State=%d",state,0,0);
    switch(state)
    {
        case ACM_RESOURCE_ACQUIRED:
            OEMCM_CallCM_AnswerCall(OEMCM_DMSSCallErrCB,
                                    noCBInfo->data_block_ptr,
                                    noCBInfo->client_id,
                                    noCBInfo->call_id,
                                    &noCBInfo->answer_params);

            MSG_HIGH("OEMCM_ACMChangeNotify received ACM_RESOURCE_ACQUIRED",0,0,0);
            break;

        default:
            MSG_HIGH("OEMCM_ACMChangeNotify received resource state=%d",state,0,0);
            break;
    }

}

/*=============================================================================
FUNCTION: OEMCM_ACM_MobileOriginatedCallChangeNotify

DESCRIPTION: ACM status change notify CB for Mobile originated call

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_ACM_MobileOriginatedCallChangeNotify(void *p)
{
    ACM_Callback_data_type *cbdata = (ACM_Callback_data_type*)p;
    ACM_resource_state_type state = cbdata->state;
    OEMCM_ACM_MobileOriginatedCallInfo * moCBInfo = (OEMCM_ACM_MobileOriginatedCallInfo *) cbdata->pdata;
    AEECMNotifyInfo * notify_info = moCBInfo->notify_info;
    //cm_call_id_type call_id = moCBInfo->call_id;

    MSG_HIGH("OEMCM: In callback of delayed acquire for MO call. State=%d",state,0,0);
    switch(state)
    {
        case ACM_RESOURCE_ACQUIRED:
            OEMCM_CallOrig_Notify(notify_info);
            break;

        default:
            MSG_HIGH("OEMCM_ACM_MobileOriginatedCallChangeNotify received resource state=%d",state,0,0);
            break;

    }

}

/*=============================================================================
FUNCTION: OEMCM_CallCM_AnswerCall

DESCRIPTION: Call CM to answer call

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMCM_CallCM_AnswerCall(cm_call_cmd_cb_f_type  cmd_cb_func,
                                   void                   *data_block_ptr,
                                   cm_client_id_type      client_id,
                                   cm_call_id_type        call_id,
                                   cm_ans_params_s_type   *ans_params_ptr)
{

  MSG_HIGH("OEMCM: Answering MT call from delayed acquire",0,0,0);
  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_answer(OEMCM_DMSSCallErrCB, data_block_ptr, client_id, call_id,
                             ans_params_ptr))
  {
    ICMCoreObj->m_state[call_id].bAnswered = FALSE;
    MSG_ERROR("OEMCM: Answering MT call from delayed acquire FAILED",0,0,0);
    return EFAILED;
  }
  MSG_HIGH ("OEMCM: Answering MT call from delayed acquire PASSED",0,0,0);
  return AEE_SUCCESS;
}


/*=============================================================================
FUNCTION: OEMCM_CallOrig_Notify

DESCRIPTION: Notify apps of HandleCallOrig

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CallOrig_Notify(AEECMNotifyInfo * notify_info)
{
  MSG_HIGH ("OEMCM: Answering MO call from delayed acquire",0,0,0);
  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

#endif  // FEATURE_ACM

/* lint -restore */

/*=============================================================================
FUNCTION: OEMCM_SendFlashSupps

DESCRIPTION:
   Send a flash with info for the specified supplementary service.

PARAMETERS:
   pMe                [in]: ICM interface pointer.
   suppsType          [in]: Supplementary service type
   callId             [in]: Call id to operate on

RETURN VALUES:
   SUCCESS
   EFAILED

=============================================================================*/
static int OEMCM_SendFlashSupps(ICM *pMe,
                               AEECMSendFlashSuppsType suppsType,
                               AEECMCallID callId)
{
   AEECMEventCBS *             pEventCb;
   cm_call_sups_params_s_type  cmSupsParams = {0};
   cm_call_sups_type_e_type    cmSupsType;

   if (NULL == pMe)
   {
      ERR("OEMCM_SendFlashSupps: NULL param",0,0,0);
      return EBADPARM;
   }

   switch (suppsType)
   {
   case AEECM_SENDFLASH_CALL_FORWARD:
      cmSupsType = CM_CALL_SUPS_TYPE_CALL_FORWARD;
      break;
   case AEECM_SENDFLASH_EMERGENCY_CALL:
      cmSupsType = CM_CALL_SUPS_TYPE_EMERGENCY_CALL;
      break;
   case AEECM_SENDFLASH_ACT_ANSWER_HOLD:
      cmSupsType = CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD;
      break;
   case AEECM_SENDFLASH_DEACT_ANSWER_HOLD:
      cmSupsType = CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD;
      break;
   case AEECM_SENDFLASH_SIMPLE_FLASH:
      cmSupsType = CM_CALL_SUPS_TYPE_SIMPLE_FLASH;
      break;
   default:
      ERR("OEMCM_SendFlashSupps: Invalid suppsType %d",suppsType,0,0);
      return EBADPARM;
   }

   /* Allocate event callback structure */
   pEventCb = OEMCM_AllocEventCBS(ICMCoreObj, sizeof(AEECMCallErrCBPkt));

   if (NULL == pEventCb)
   {
      MSG_ERROR("OEMCM_SendFlashSupps: Failed to allocate event callback", 0, 0, 0);
      return EFAILED;
   }

   cmSupsParams.call_id = (cm_call_id_type)OEMCM_CMCALLID(callId);
   if (!cm_mm_call_cmd_sups(OEMCM_DMSSCallErrCB, pEventCb, gCMID,
                            cmSupsType,
                            &cmSupsParams))
   {
      MSG_ERROR("OEMCM_SendFlashSupps: Call to cm_mm_call_cmd_sups failed", 0, 0, 0);
      return EFAILED;
   }

   return SUCCESS;
}

/* PRL Related */

/*=============================================================================
FUNCTION: OEMCM_PrlExtractAcqRecord

DESCRIPTION:
   Extract a specified acquisition record from the currently loaded PRL.

PARAMETERS:
   pMe                [in]: ICM interface pointer.
   pAcqRec            [out]: Pointer to acq record structure in which to
                            fill the record info.
   index              [in]: Index of the acquisition record to extract


RETURN VALUES:
   SUCCESS
   EBADPARM
   EFAILED

=============================================================================*/
static int OEMCM_PrlExtractAcqRecord(ICM *pMe,
                                     AEECMPrlAcqRecord *pAcqRec,
                                     int index)
{
#ifdef FEATURE_EXPORT_PRL
   prli_acq_rec_type acqRec;

   if (NULL == pAcqRec || NULL == pMe)
   {
      ERR("OEMCM_PrlExtractAcqRecord: NULL param",0,0,0);
      return EBADPARM;
   }

   if (!prl_extract_acq_rec(&acqRec, index))
   {
      ERR("OEMCM_PrlExtractAcqRecord: prl_extract_acq_rec FAILED",0,0,0);
      return EFAILED;
   }

   OEMCMHelper_CopyFromPrlAcqRec(pAcqRec, &acqRec);

   return SUCCESS;

#else

   return EUNSUPPORTED;

#endif

}

/*=============================================================================
FUNCTION: OEMCM_PrlUpdateAcqRecord

DESCRIPTION:
   Update the PRL acquisition table entry with the specified acqusition record.

PARAMETERS:
   pMe                [in]: ICM interface pointer.
   pAcqRec            [in]: Pointer to AEECM acquisition record which will be
                            used to update the acquisition table.
   index              [in]: Index of table entry to update.

RETURN VALUES:
   SUCCESS
   EBADPARM
   EFAILED

=============================================================================*/
static int OEMCM_PrlUpdateAcqRecord(ICM *pMe,
                                    const AEECMPrlAcqRecord *pAcqRec,
                                    int index)
{
#ifdef FEATURE_EXPORT_PRL
   prli_acq_rec_type acqRec;

   if (NULL == pAcqRec || NULL == pMe)
   {
      ERR("OEMCM_PrlUpdateAcqRecord: NULL param",0,0,0);
      return EBADPARM;
   }

   OEMCMHelper_CopyToPrlAcqRec(&acqRec, pAcqRec);

   if (!prl_update_acq_rec(&acqRec, index))
   {
      ERR("OEMCM_PrlUpdateAcqRecord: prl_update_acq_rec FAILED",0,0,0);
      return EFAILED;
   }

   return SUCCESS;

#else

   return EUNSUPPORTED;

#endif

}

/*=============================================================================
FUNCTION: OEMCM_PrlCommit

DESCRIPTION:
   Commit PRL in NV

PARAMETERS:
   pMe                [in]: ICM interface pointer.

RETURN VALUES:
   SUCCESS
   EBADPARM
   EFAILED

=============================================================================*/
static int OEMCM_PrlCommit(ICM *pMe)
{
#ifdef FEATURE_EXPORT_PRL

   if (NULL == pMe)
   {
      ERR("OEMCM_PrlCommit: NULL param",0,0,0);
      return EBADPARM;
   }

   if (!prl_commit())
   {
      ERR("OEMCM_PrlCommit: prl_commit failed",0,0,0);
      return EFAILED;
   }

   return SUCCESS;

#else

   return EUNSUPPORTED;

#endif
}

/*=============================================================================
FUNCTION: OEMCM_SendMBMSCommand

DESCRIPTION:
  Sends a command to the MBMS Service Manager.
  Clients should fill in the appropriate MBMS request type and also
  provide the corresponding parameters in the mbms_generic_req struct.

  Aysynchronous command responses will be sent to the application
  via notify events.  Apps that wish to receive MBMS related events
  must register with ICM notifier to receive events with the mask
  NMASK_CM_MBMS.  Eg:
  ISHELL_RegisterNotify(pIShell, AEECLSID_SOMEAPP, AEECLSID_CM_NOTIFIER, NMASK_CM_MBMS)

  Event specific data will accompany the event in the dwParam->data within a structure of
  type AEECMNotifyInfo.event_data.mbms.  See AEECMMBMSEventData to see the specific event
  types and data that can be received related to MBMS.

PARAMETERS:
  pMe                [in]: ICM interface pointer.
  pMbmsCmd           [in]: MBMS command structure, used to specify the specific MBMS
                          command to be sent along with its parameters.
                          See AEECMMBMSCmdType for available commands.

RETURN VALUE:
  AEE_SUCCESS:  Command was sent.
  EFAILED:      Error while sending command.
=============================================================================*/
static int OEMCM_SendMBMSCommand(ICM *pMe, AEECMMBMSCmd *pMbmsCmd)
{
#ifdef FEATURE_MBMS
   cm_mbms_generic_cmd_s_type mbmsGenericCmd;
   int retVal = AEE_SUCCESS;

   if (NULL == pMe)
   {
      ERR("OEMCM_SendMBMSCommand: NULL param",0,0,0);
      return EBADPARM;
   }

   if (NULL == pMbmsCmd)
   {
      ERR("OEMCM_SendMBMSCommand: NULL param",0,0,0);
      return EBADPARM;
   }

   //Populate the mbmsGenericCmd params from the AEECM version of that struct
   OEMCMHelper_CopyMBMSCmdFromAEECM(&mbmsGenericCmd, pMbmsCmd);

   //Queue the command to cm
   //DataBlockPtr currently unused
   if (FALSE == cm_mbms_cmd_generic(OEMCM_DMSSMbmsErrCB, NULL, gCMID, &mbmsGenericCmd))
   {
      retVal = EFAILED;
   }

   return(retVal);
#else
   return EUNSUPPORTED;
#endif //FEATURE_MBMS
}

void OEMCM_TestCallEventNotify(AEECMEvent event, cm_mm_call_info_s_type *call_info)
{
  PFNOEMCMCALLNOTIFY      pfnNotify = NULL;
  uint16                  fn_index;

  if (!ICMCoreObj)
  {
    return;
  }

  fn_index = (uint16)((int32)event & 0x00FF);

  do
  {
  /* Find the event handler associated with this event.
   * Event handler table is indexed by the last two nibbles of event.
   * This is basically a huge switch case statement.
   */
    if (AEECM_EVENT_GROUP((uint16)event) == AEECM_CALL_GROUP)
    {
    /* Main call group */
      if (fn_index >= ARR_SIZE(gCallEventHandlers))
      {
      MSG_ERROR("OEMCM_BREWCallEventCB: No event handler for this event (0x%X)",
                                                              event, 0, 0);
        break;
    }
    pfnNotify = gCallEventHandlers[fn_index];
    }
    else if (AEECM_EVENT_GROUP((uint16)event) == AEECM_INTERNAL_CALL_GROUP)
    {
    /* Internal call events that are not sent to the App */
      if (fn_index >= ARR_SIZE(gCallIntEventHandlers))
      {
      MSG_ERROR("OEMCM_BREWCallEventCB: No event handler for this event (0x%X)",
                                                              event, 0, 0);
        break;
    }
    pfnNotify = gCallIntEventHandlers[fn_index];
    }
    else
    {
    MSG_ERROR("OEMCM_BREWCallEventCB: Unknown event (0x%X)", event, 0, 0);
      break;
  }


  /* Found the event handler, now call it */
    if (pfnNotify)
    {
    (*pfnNotify)(call_info, event);
    }
    else
    {
    MSG_ERROR("OEMCM_BREWCallEventCB: No event handler found for event 0x%X",
                                                                  event, 0, 0);
  }
  }
  while(0);
}

