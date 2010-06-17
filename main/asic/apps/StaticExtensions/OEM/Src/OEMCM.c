/*=============================================================================

FILE: OEMCM.c

SERVICES: OEM Functions for Call Manager Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2003-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //depot/asic/qsc60x0/apps/StaticExtensions/OEM/Src/OEMCM.c#2 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
04/13/06     gmy     Fix compile fail assoc'd w/intro of CATAPP/GSTK to SC1x
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
#include "BREWVersion.h"
#include "OEMFeatures.h"
#ifndef WIN32
#include "err.h"
#endif
#include "AEE_OEM.h"
#include "AEECM.h"
#include "AppComFunc.h"
#include "AEECallHistoryExt.h"
#include "oemos.h"
#include "AEEStdLib.h"
#include "OEMHeap.h"
#if MIN_BREW_VERSION(3,0)
#include "AEE_OEMDispatch.h"
#include "OEMCriticalSection.h"
#endif
#include "OEMObjectMgr.h"
#include "AEECriticalSection.h"

#include "cm.h"
#include "sys.h"
#ifndef WIN32
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#include "cai.h"
#endif

#include "sd.h" 

#include "tmc.h"
#else

#define MSG_LOW( sf, a, b, c )
#define MSG_ERROR(p,p1,p2,p3)
#define MSG_HIGH( sf, a, b, c )    
#define MSG_MED( sf, a, b, c )      
#define ERR( sf, a, b, c )    



#define NV_MAX_NAMS 1
#define NV_RUIM_NAM 2
#define CAI_REL_NORMAL 0x00
#define CAI_ALLOW_PI     0x00

#define CAI_SIG_TONE  0x00
  /* Tone signal */
#define CAI_SIG_ISDN  0x01
  /* ISDN Alerting */
#define CAI_SIG_IS54B 0x02


#endif

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */

#ifdef FEATURE_OTASP
#define ISVOICE(call_type)   \
        ( ( (call_type) == AEECM_CALL_TYPE_VOICE )     || \
          ( (call_type) == AEECM_CALL_TYPE_EMERGENCY ) || \
          ( (call_type) == AEECM_CALL_TYPE_STD_OTASP ) || \
          ( (call_type) == AEECM_CALL_TYPE_NON_STD_OTASP ) )
#else
#define ISVOICE(call_type)   \
        ( ( (call_type) == AEECM_CALL_TYPE_VOICE )  || \
          ( (call_type) == AEECM_CALL_TYPE_EMERGENCY ) )
#endif

/*
#define IS_E911_CALLBACK_MODE( ph_ptr ) \
                     ( (ph_ptr->mode_pref == CM_MODE_PREF_EMERGENCY) && \
                       (ph_ptr->pref_term == CM_PREF_TERM_PWR_CYCLE) )

*/


#define OEMCM_MAX_NUM_CALLS   10       /* Max number of simultaneous calls */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif

#define OEMCM_CDMA_END_VALUE  0x0300

#define AEECM_INTERNAL_CALL_GROUP 0x05
#define AEECM_INTERNAL_PH_GROUP   0x06

/* limit continuous DTMF time reporting to two seconds */
#define AEECM_MAX_DTMF_RPT_TIME 2000

#if MIN_BREW_VERSION(3,0)
static OEMCriticalSection gcmCriticalSection;
#define AEECM_MUTEX_LOCK()                       \
  /*lint -e545 */                                \
  AEECriticalSection_Enter(&gcmCriticalSection)  \
  /*lint +e545 */
#define AEECM_MUTEX_UNLOCK()                     \
  /*lint -e545 */                                \
  AEECriticalSection_Leave(&gcmCriticalSection)  \
  /*lint +e545 */
#else
#define AEECM_MUTEX_LOCK() OEMOS_MutexStart(); {
#define AEECM_MUTEX_UNLOCK() ;} OEMOS_MutexStop();
#endif

/* suppress warning for unused parameter. */
#undef PARAM_NOT_REF
#define PARAM_NOT_REF(x) /*lint -esym(715,x) */
#define IN_RANGE(val, min, max) ( (val) >= (min) && (val) <= (max) )

#define OEMCM_NUM_CALLHISTORY_FIELDS 5

/* To always memset allocated memory and print error message */
#define OEMCM_MALLOC(x) OEMCM_Malloc((x), __LINE__)
#define OEMCM_FREE(x) OEMCM_Free((x))

/* Adds a node to the head of a doubly linked list */
/* l-value of head should not be NULL */
#define OEMCM_ADD_NODE(head, node) \
        { \
          /*lint -e774 */ \
          if ((node)) { \
            if ((head)) (head)->pPrev = (node); \
            (node)->pPrev = NULL; \
            (node)->pNext = (head); \
            (head) = (node); \
          } \
          /*lint +e774 */ \
        }

/* Removes a node from the doubly linked list */
/* l-value of head should not be NULL */
#define OEMCM_REMOVE_NODE(head, node) \
        { \
          /*lint -e774 */ \
          if ((node)) { \
            if ((node)->pPrev) \
              (node)->pPrev->pNext = (node)->pNext; \
            else \
              (head) = (node)->pNext; \
            if ((node)->pNext) \
              (node)->pNext->pPrev = (node)->pPrev; \
          } \
          /*lint +e774 */ \
        }

#define FEATURE_ICM_DEBUG
                             
/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

/* General Comment:
 *
 * X = Call|Phone|Inband|SS
 * Y = Event|Err
 *
 * OEMCM_DMSS[X][Y]CB() is registered as DMSS CM callback.
 * OEMCM_BREW[X][Y]CB() is the callback executed in App context or 
 * BREW system context.
 * 
 * OEMCM_BREW[X]EventCB() is executed in BREW system context for sending 
 * event notifications to applications.
 * OEMCM_BREW[X]ErrCB() is executed in App context for executing 
 * status/error callbacks registered by application. 
 * status and error callback refer to the same.
 */

/* Generic Event callback structure.
 * Used for execution in BREW system context as a result of a CM event 
 * callback to send a notification.
 */
typedef struct AEECMEventCBS {
  AEECallback            cb;         
  AEECMEvent             event;
  void *                 data;
  uint8                  nRef;
  struct AEECMEventCBS * pNext;
  struct AEECMEventCBS * pPrev;
} AEECMEventCBS;

/* Generic Error callback structure.
 * Used for execution in App context as a result of a CM cmd callback
 */
typedef struct AEECMErrCBS {
  AEECallback *         pcb;
  void *                pError;
  ICM *                 pOwner;
  AEEObjectHandle       hObject;
  struct AEECMErrCBS *  pNext;
  struct AEECMErrCBS *  pPrev;
} AEECMErrCBS;

/* Special case for Get Networks: status(error) callback needs to
 * be executed in App context as a result of an event callback.
 */
typedef struct {
  boolean               bInUse;
  AEECallback *         pcb;
  void *                pError;
  void *                list;
  uint32                list_size;
  AEEObjectHandle       hObject;
  ICM *                 pOwner;
} AEECMGetNWCBS;

typedef struct AEECMPrivateCallState {
  flg           bAnswered:1;   /* Have we called answer before? */
  flg           bEnded:1;      /* Have we called end before? */
  flg           bSignalled:1;  /* Was signal event sent before */
  flg           bSetup:1;      /* Setup event received and incoming event 
                                  not received */
  flg           bValid:1;      /* initialized? */
} AEECMPrivateCallState; 

typedef struct AEECMBurstDTMFPkt {
  ICM *                 pOwner;
  AEECMCallID           call_id;
  AECHAR *              dtmfs;
  AECHAR *              curr;
  uint8                 on_length;
  uint8                 off_length;
  boolean               start;
} AEECMBurstDTMFPkt;

/* Core Object, instantiated once */
typedef struct ICMCore {
  uint32                m_uRefs;

  AEECMCallInfo         m_call_info[OEMCM_MAX_NUM_CALLS]; 
  AEECMPrivateCallState m_state[OEMCM_MAX_NUM_CALLS];

  uint32                m_dtmf_start_times[OEMCM_MAX_NUM_CALLS]; /* in ms */
  AEECMBurstDTMFPkt *   m_bdtmf[OEMCM_MAX_NUM_CALLS];

  ICM *                 m_call_owners[OEMCM_MAX_NUM_CALLS];

  AEECMPhInfo           m_ph_info;    
  AEECMSSInfo           m_ss_info;   

  boolean               m_ph_valid;
  boolean               m_ss_valid;

  /* AEE callback and event data */
  AEECMEventCBS *       m_event_cbs;

  /* Get Networks handling (only one allowed at a time) */
  AEECMGetNWCBS         m_getnw_cb;

  /* Default call parameters */
  cm_cdma_orig_params_s_type  def_cdma_orig_params;
  cm_gw_cs_orig_params_s_type def_gw_orig_params;

#ifdef FEATURE_ICM_DEBUG
  uint32                m_malloc_cnt;
#endif

} ICMCore;

/* ICM object, instantiated every time */
struct ICM {
  AEEVTBL(ICM) *        pvt;      
  IShell *              m_pIShell;
  uint32                m_uRefs; 
  AEECLSID              m_cls;  

  ICMCore *             m_coreObj;    /* pointer to core object */
  void *                m_pac;        /* App context */

  sys_plmn_id_s_type    m_cm_plmn_id; /* Storage for PLMN ID */

  /* AEE Callback and error */
  AEECMErrCBS *         m_err_cbs; 
};

/* ICMNotifier object, instantiated once */
struct ICMNotifier {
   AEEVTBL(ICMNotifier) *   pvt;      
   IShell *                 m_pIShell;
   uint32                   m_uRefs;  
   AEECLSID                 m_cls;    
    
   ICMCore *                m_coreObj;   /* pointer to core object */

   uint32                   m_dwNotify;  /* Notification bits */
};

/* ICallOpts Object */
struct ICallOpts {
  AEEVTBL(ICallOpts) *     pvt;
  IShell *                 m_pIShell;
  uint32                   m_uRefs;  
  AEECLSID                 m_cls;

  IWebOpts *               m_pIWebOpts;

  /* Temp hack to support AECHAR * strings till IWebOpts adds this support.
   * Only one AECHAR string (AEECM_CALLOPT_CALLED_SUBADDR) supported at 
   * this time. Hopefully AECHAR support will be added to IWebOpts soon.
   */
  AECHAR *                 m_str; 
  
};

/* Used to pass call error information from App to CM cmd callback to 
 * BREW callback.
 * The event callback structure is allocated and call id filled in 
 * when App invokes an interface function. Call id is filled in earlier 
 * because CM cmd callback does not give us a call id. Error is filled 
 * in during CM cmd callback. The event callback structure is freed in 
 * OEMCM_BREWCallErrCB() where AEECM_EVENT_CALL_ERROR event is sent to the
 * Apps.
 */
typedef struct {
  cm_call_id_type         call_id;
  AEECMCallError          call_error;
} AEECMCallErrCBPkt;
  
/*=========================================================================*/

static ICMCore *ICMCoreObj = NULL;
static struct ICMNotifier *ICMNotifierObj = NULL;

static cm_client_id_type   gCMID = 0;
static boolean             gbCMAct = FALSE;

typedef void (*PFNOEMCMCALLNOTIFY)(cm_mm_call_info_s_type *, AEECMEvent); 
typedef void (*PFNOEMCMPHNOTIFY)(cm_ph_info_s_type *, AEECMEvent);

/*===========================================================================

               INTERFACE   FUNCTION   DECLARATIONS   

===========================================================================*/

/* Interface to the CM Notifier object. */
static uint32 OEMCMNotifier_AddRef(ICMNotifier *po);
static uint32 OEMCMNotifier_Release(ICMNotifier *po);
static void OEMCMNotifier_SetMask(ICMNotifier *po, uint32 *pdwMask);

/* Interface to the CM object. */
static uint32 OEMCM_AddRef(ICM *po);
static uint32 OEMCM_Release(ICM *po);
static int OEMCM_QueryInterface(ICM *po, AEECLSID clsid, void **ppNew);
static int OEMCM_OriginateVoiceCall(ICM *po, AECHAR const *number, 
                                    AEECMCallID *call_id);
static int OEMCM_OriginateCall(ICM *po, AEECMCallType call_type, 
                             AECHAR const *number, AECHAR const *alpha,
                             ICallOpts * options, AEECMCallID  *call_id);
static int OEMCM_AnswerCall(ICM *po, AEECMCallID call_id);
static int OEMCM_EndCall(ICM *po, AEECMCallID call_id);
static int OEMCM_EndAllCalls(ICM *po);
static uint16 OEMCM_GetActiveCallIDs(ICM *pMe, AEECMCallType call_type,
                             AEECMCallState call_state,
                             AEECMCallID *call_ids, uint32 size);
static int OEMCM_GetCallInfo(ICM *po, AEECMCallID call_id, 
                             AEECMCallInfo *call_info, uint32 size);
static int OEMCM_SetPrivacyPref(ICM *po, AEECMPrivacyPref privacy_pref);
static AEECMIsCallOps OEMCM_CallOpsStringCheck(ICM *po,
                             AECHAR * dialed_string,
                             uint8 *call_num);
static int OEMCM_CallOpsRequest(ICM *po,
                         AECHAR * dialed_string,
                         AEECMCallID call_id);

  /* Phone Functions */
static int OEMCM_SetSystemPreference(ICM *po, AEECMModePref mode_pref,
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
                             AEECallback *cb);
static int OEMCM_SetNAMSel(ICM *po, AEECMNAM nam_sel);
static int OEMCM_SetOperatingMode(ICM *po, AEECMOprtMode oprt_mode);
static int OEMCM_GetPhoneInfo(ICM *po, AEECMPhInfo *info, uint32 size);
static int OEMCM_SetSubscriptionStatus(ICM *po, AEECMSysMode mode, 
                                    boolean status);
static int OEMCM_SetRSSIDeltaThreshold(ICM *po, uint8 delta);
static int OEMCM_GetNetworks(ICM *po, AEECMNetworkType list_type,
                             AEECMPhError *status,
                             void * list,
                             uint32 size,
                             AEECallback *cb);
static int OEMCM_SetPrefNetworks(ICM *po,
                             AEECMUserPLMNList  *pref_networks_ptr,
                             AEECMPhError *status,
                             AEECallback *cb);
static int OEMCM_WakeupFromStandby(ICM *po);
static int OEMCM_SetRTREConfig(ICM *po, AEECMRTREConfig rtre_config);
static int OEMCM_SetAnswerVoicePref(ICM *po, AEECMAnsVoicePref answer_voice,
                              uint32  duration);
static int OEMCM_AvoidServingSystem(ICM *po, AEECMAvoidSSPref avoid_type,
                              uint32 avoid_time);
static int OEMCM_ResetACM(ICM *pMe);
static int OEMCM_SetACMMax(ICM *pMe, uint32 acm_max);
static int OEMCM_SetDDTMPref(ICM *pMe, AEECMDDTMPref ddtm_pref);

  /* Serving System Function */
static int OEMCM_GetSSInfo(ICM *po, AEECMSSInfo *ss_info, uint32 size);

  /* Inband functions */
static int OEMCM_BurstDTMF(ICM *po, AEECMCallID call_id, 
                           AEECMDTMFOnLength on_length, 
                           AEECMDTMFOffLength off_length, AECHAR *digits);
static int OEMCM_StartContDTMF(ICM *po, AEECMCallID call_id, AECHAR digit);
static int OEMCM_StopContDTMF(ICM *po, AEECMCallID call_id);
  

/* Helper functions */
static boolean OEMCM_IsValidModeBandPref(ICM *pMe, AEECMModePref mode_pref,
                                            AEECMBandPref band_pref);
static AEECMSysMode OEMCM_GetSysMode(ICM *po);
static boolean OEMCM_IsMyCall(ICM *po, AEECMCallID call_id);

/* Interface to the CallOpts Object */
static uint32 OEMCallOpts_AddRef(ICallOpts *pMe);
static uint32 OEMCallOpts_Release(ICallOpts *pMe);
static int OEMCallOpts_QueryInterface(ICallOpts *pMe, AEECLSID id, void **ppo);
static int OEMCallOpts_AddOpt(ICallOpts *pMe, CallOpt *pOpt);
static int OEMCallOpts_RemoveOpt(ICallOpts *pMe, int32 nOptId);
static int OEMCallOpts_GetOpt(ICallOpts *pMe, int32 nOptId, CallOpt *pOpt);
static cm_band_pref_e_type OEMCM_BandPrefToDMSS(AEECMBandPref pref);
static cm_srv_domain_pref_e_type OEMCM_SrvDomainPrefToDMSS(AEECMServDomPref pref);
static cm_roam_pref_e_type OEMCM_RoamPrefToDMSS(AEECMRoamPref pref);
static cm_gw_acq_order_pref_e_type OEMCM_AcqOrderPrefToDMSS(AEECMAcqOrderPref pref);
static cm_pref_term_e_type OEMCM_PrefTermToDMSS(AEECMPrefTerm pref);


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
   OEMCM_IsMyCall
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

               INTERNAL FUNCTION DECLARATIONS   

===========================================================================*/
static ICMCore * OEMCMCore_New(void);
static uint32 OEMCMCore_AddRef(void);
static uint32 OEMCMCore_Release(void);

static void OEMCM_BREWCallEventCB(void *data);
static void OEMCM_BREWCallErrCB(void *data);
static void OEMCM_BREWPhEventCB(void *data);
static void OEMCM_BREWSSEventCB(void *data);
static void OEMCM_BREWInbandEventCB(void *data);

static void OEMCM_DMSSCallEventCB(cm_call_event_e_type call_event,
                             const cm_mm_call_info_s_type *call_info_ptr);
static void OEMCM_DMSSPhEventCB(cm_ph_event_e_type  ph_event,
                              const cm_ph_info_s_type *ph_info_ptr);
static void OEMCM_DMSSInbandEventCB(cm_inband_event_e_type inband_event,
                               const cm_inband_info_s_type *inband_info_ptr);
static void OEMCM_DMSSSSEventCB(cm_ss_event_e_type ss_event,
                           const cm_mm_ss_info_s_type   *ss_info_ptr);

static void OEMCM_DMSSCallErrCB (void *data_block_ptr,
                             cm_call_cmd_e_type call_cmd,
                             cm_call_cmd_err_e_type call_cmd_err);
static void OEMCM_DMSSOtherCallErrCB(cm_call_id_type call_id,
                                 cm_client_id_type client_id,
                                 cm_call_cmd_e_type cmd,
                                 cm_call_cmd_err_e_type cmd_err);
static void OEMCM_DMSSPhErrCB(void *data_block_ptr,
                         cm_ph_cmd_e_type ph_cmd,
                         cm_ph_cmd_err_e_type ph_cmd_err);
static void OEMCM_DMSSGetNWErrCB(void *data_block_ptr,
                         cm_ph_cmd_e_type ph_cmd,
                         cm_ph_cmd_err_e_type ph_cmd_err);
static void OEMCM_DMSSGetNWEventCB(const cm_ph_info_s_type *ph_info, 
                                   cm_ph_event_e_type event);
static void OEMCM_DMSSInbandErrCB(void * data_block_ptr,
                              cm_inband_cmd_e_type inband_cmd,
                              cm_inband_cmd_err_e_type inband_cmd_err);

static void OEMCM_CancelBurstDTMF(ICM * pMe);
static int OEMCM_AllocBurstDTMF(ICM *pMe, AEECMCallID call_id,
                                AECHAR *digits,
                                AEECMDTMFOnLength on_length,
                                AEECMDTMFOffLength off_length,
                                AEECMBurstDTMFPkt **bdtmf);
static void OEMCM_FreeBurstDTMF(AEECMBurstDTMFPkt *dtmf_pkt);
static void OEMCM_HandleBurstDTMF(void *pUser);
static int32 OEMCM_DTMFOnToMS(AEECMDTMFOnLength on_length);
static int32 OEMCM_DTMFOffToMS(AEECMDTMFOffLength off_length);

static void OEMCM_CancelAllEventCB(ICMCore *pMe);
static void OEMCM_AddRefEventCBS(AEECMEventCBS *pcb);
static AEECMEventCBS * OEMCM_AllocEventCBS(ICMCore *pMe, uint32 data_size);
static void OEMCM_FreeEventCBS(AEECMEventCBS *cb);

static void OEMCM_CancelAllErrCB(ICM *pMe);
static AEECMErrCBS *OEMCM_AllocErrCBS(ICM *pMe);
static void OEMCM_FreeErrCBS(ICM * pMe, AEECMErrCBS *pcb);

static boolean OEMCM_InitCallManager(void);
static void OEMCM_ReleaseCallManager(void);

static void OEMCM_AppCancelErrCB(AEECallback *pcb);
static void OEMCM_AppCancelGetNW(AEECallback *pcb);

static void OEMCM_InitDefaultCallParams(ICMCore *);
void OEMCM_HandleCommandError(cm_call_id_type call_id,
                        cm_call_cmd_e_type call_cmd,
                        cm_call_cmd_err_e_type call_cmd_err);
static int OEMCM_SendFlash(AECHAR const *number, AEECMCallID flash_call_id);
static uint32  OEMCM_MergeMasks(const uint32 * pdwMasks);
static void * OEMCM_Malloc(uint32 size, int line);
static void OEMCM_Free(void *);
static AEECMSysMode OEMCM_GetSystemMode(void);
static AEECMCallType OEMCM_GetCallType(cm_call_id_type call_id);
static void OEMCM_InitNum(cm_num_s_type *num, char *number);
static void OEMCM_CopyInCallInfo(AEECMCallInfo *dest, 
                                 const cm_mm_call_info_s_type *src);
static void OEMCM_CopyInPhInfo(AEECMPhInfo *dest,
                               const cm_ph_info_s_type *src);
static void OEMCM_CopyInSSInfo(AEECMSSInfo *dest,
                               const cm_mm_ss_info_s_type *src);
static void OEMCM_CopyInMMInfo(AEECMMMInfo *mmInfo, 
                               const sys_mm_information_s_type *mm_info);
static void OEMCM_CopyInSysID(AEECMSysID *dest, const sys_sys_id_s_type *src);
static void OEMCM_CopyInFullNWName(AEECMNWName *dest, 
                                   const sys_network_full_name_s_type *src);
static void OEMCM_CopyInShortNWName(AEECMNWName *dest, 
                                   const sys_network_short_name_s_type *src);
static void OEMCM_CopyInAvailableNetworks(AEECMDetailedPLMNList *dest, 
                                    const sys_detailed_plmn_list_s_type *src);
static void OEMCM_CopyInUserPrefNetworks(AEECMUserPLMNList *dest,
                                   const sys_user_pref_plmn_list_s_type *src);
static void OEMCM_CopyOutUserPrefNetworks(sys_user_pref_plmn_list_s_type *dest,
                                          AEECMUserPLMNList *src);
static void OEMCM_ResetPrivateCallState(cm_call_id_type call_id);
static boolean OEMCM_IsValidAppCallID(AEECMCallID call_id);
static boolean OEMCM_IsValidCallID(cm_call_id_type call_id);
static AEECMPhInfo * OEMCM_GetValidPhInfo(ICM *pMe);
static AEECMCallState OEMCM_GetCallState(cm_call_id_type call_id);
static void OEMCM_CopyNumToOtherParty(cm_num_s_type *num, 
                                      AEECMCallInfo *core_call_info);
static void OEMCM_CopyNumToAEECMNum(cm_num_s_type *num, AEECMNumber *aee_num);
static void OEMCM_CopyCharLenToAECHAR(char *cmchar, uint8 len, 
                                      AECHAR *aeechar, uint16 size);

static void OEMCM_CopyInSignal(AEECMSignal *aee_signal, 
                               cm_signal_s_type *cm_signal);
                               
static AEECMCallType OEMCM_CallTypeToAEECM(cm_call_type_e_type call_type);
static cm_call_type_e_type OEMCM_CallTypeToDMSS(AEECMCallType call_type);

static AEECMPrivacyPref OEMCM_PrivacyPrefToAEECM(cm_privacy_pref_e_type pref);
static cm_privacy_pref_e_type OEMCM_PrivacyPrefToDMSS(AEECMPrivacyPref pref);
static AEECMHybrPref OEMCM_HybrPrefToAEECM(cm_hybr_pref_e_type pref);
static cm_hybr_pref_e_type OEMCM_HybrPrefToDMSS(AEECMHybrPref pref);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
static void OEMCM_FillInCDMACallOptions(
                                      cm_cdma_orig_params_s_type* cdma_orig_ptr,
                                      cm_srv_type_e_type *srv_type,
                                      ICallOpts * options);

static void OEMCM_NotifyApps(AEECMNotifyInfo *notify_info);

static AEECMEvent OEMCM_CallEventToAEECM(cm_call_event_e_type evt);
static AEECMEvent OEMCM_PhEventToAEECM(cm_ph_event_e_type event);
static AEECMEvent OEMCM_SSEventToAEECM(cm_ss_event_e_type event);
static AEECMEvent OEMCM_InbandEventToAEECM(cm_inband_event_e_type inband_event);
static uint32 OEMCM_SSMaskToAEECM(uint32 cm_mask);
static AEECMCallError OEMCM_CallErrorToAEECM(cm_call_cmd_err_e_type error);
static AEECMPhError OEMCM_PhErrorToAEECM(cm_ph_cmd_e_type error);


static boolean OEMCM_CheckDTMFOnLength(int32 length);
static boolean OEMCM_CheckDTMFOffLength(int32 off_length);
static boolean OEMCM_CheckDTMFDigits( uint8 cnt, uint8 *digit_buf);

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
static boolean OEMCM_CheckValidOprtMode(AEECMPhInfo * ph_ptr);
#endif
#if 0
static void OEMCM_AddCallHistory(AEECMCallInfo *call_info);
#endif
static int OEMCM_RejectCall(ICM *pMe, cm_call_id_type call_id);

static void  OEMCM_CallNotifyInitDefault(AEECMNotifyInfo *notify_info, 
                                         cm_call_id_type call_id,
                                         AEECMEvent event);

static void OEMCM_HandleCallOrig(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallAnswer(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallEnd(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallIncom(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallIncomFwd(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallConnect(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallConnectedNum(cm_mm_call_info_s_type *call_info, AEECMEvent event);

static void OEMCM_HandleCallDefault(cm_mm_call_info_s_type *call_info, 
                                            AEECMEvent event);
static void OEMCM_HandleCallNumUpdate(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallCopyNum(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallAlphaUpdate(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallExtDisplay(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallSupsStateChange(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallProgress(cm_mm_call_info_s_type *call_info, 
                                            AEECMEvent event);
static void OEMCM_HandleCallDormant(cm_mm_call_info_s_type *call_info, AEECMEvent event);
#if 1
static void OEMCM_HandleCallLineCtrl(cm_mm_call_info_s_type *call_info, AEECMEvent event);
#ifdef FEATURE_REDIRECTING_NUMBER_INFO_REC
static void OEMCM_HandleCallRedirUpdate(cm_mm_call_info_s_type *call_info, AEECMEvent event);
#endif
#endif
static void OEMCM_HandleCallSignal(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallSrvOpt(cm_mm_call_info_s_type *call_info, AEECMEvent event);

static void OEMCM_HandleCallSetupInd(cm_mm_call_info_s_type *call_info, AEECMEvent);
static void OEMCM_HandleCallMngCallsConf(cm_mm_call_info_s_type *, AEECMEvent);

static void OEMCM_HandlePhDefault(cm_ph_info_s_type *ph_info, AEECMEvent event);
static void OEMCM_HandlePhTestControl(cm_ph_info_s_type *ph_info, AEECMEvent event);
static void OEMCM_HandlePhInternalDefault(cm_ph_info_s_type *ph_info, AEECMEvent event);


/* Call Event Handlers */
static PFNOEMCMCALLNOTIFY gCallEventHandlers[] = {
  NULL,                             /* Nothing */
  OEMCM_HandleCallOrig,             /* AEECM_EVENT_CALL_ORIG */
  OEMCM_HandleCallAnswer,           /* AEECM_EVENT_CALL_ANSWER */
  OEMCM_HandleCallEnd,              /* AEECM_EVENT_CALL_END */
  OEMCM_HandleCallIncom,            /* AEECM_EVENT_CALL_INCOM */
  OEMCM_HandleCallConnect,          /* AEECM_EVENT_CALL_CONNECT */
  NULL,                             /* AEECM_EVENT_CALL_ERROR This event been process in 4322*/
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_OPS_ERROR */
  OEMCM_HandleCallNumUpdate,        /* AEECM_EVENT_CALL_OPS */
  OEMCM_HandleCallCopyNum,          /* AEECM_EVENT_CALL_CALLER_ID */
  OEMCM_HandleCallAlphaUpdate,      /* AEECM_EVENT_CALL_DISPLAY */
  OEMCM_HandleCallCopyNum,          /* AEECM_EVENT_CALL_CALLED_PARTY */
  OEMCM_HandleCallConnectedNum,     /* AEECM_EVENT_CALL_CONNECTED_NUM */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_BARRED */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_IS_WAITING */
  OEMCM_HandleCallSupsStateChange,  /* AEECM_EVENT_CALL_ON_HOLD */
  OEMCM_HandleCallSupsStateChange,  /* AEECM_EVENT_CALL_RETRIEVED */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_ORIG_FWD_STATUS */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_FORWARDED */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_BEING_FORWARDED */
  OEMCM_HandleCallIncomFwd,         /* AEECM_EVENT_CALL_INCOM_FWD_CALL */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_RESTRICTED */
  OEMCM_HandleCallDefault,          /* AEECM_EVENT_CALL_CUG_INFO_RECEIVED */
  OEMCM_HandleCallAlphaUpdate,      /* AEECM_EVENT_CALL_CNAP_INFO_RECEIVED */
  OEMCM_HandleCallNumUpdate,        /* AEECM_EVENT_CALL_EMERGENCY_FLASHED */
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
  OEMCM_HandleCallDormant,          /* AEECM_EVENT_CALL_EXIT_DORMANT */
#if 1
  OEMCM_HandleCallLineCtrl,         /* AEECM_EVENT_CALL_LINE_CTRL */
#ifdef FEATURE_REDIRECTING_NUMBER_INFO_REC
  OEMCM_HandleCallRedirUpdate       /* AEECM_EVENT_CALL_REDIR_NUM */
#endif
  //OEMCM_HandleCallDefault          /*AEECM_EVENT_CALL_EXIT_TC*/
#endif
};

/* Call internal event handlers */
static PFNOEMCMCALLNOTIFY gCallIntEventHandlers[] = {
  NULL,                             /* Nothing */
  OEMCM_HandleCallSetupInd,         /* AEECM_EVENT_INTERNAL_SETUP_IND */
  OEMCM_HandleCallMngCallsConf,     /* AEECM_EVENT_INTERNAL_MNG_CALLS_CONF */
  OEMCM_HandleCallEnd,              /* AEECM_EVENT_INTERNAL_END_REQ */
  OEMCM_HandleCallExtDisplay,       /* AEECM_EVENT_INTERNAL_EXT_DISP */
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
#ifndef WIN32
#ifdef AEE_SIMULATOR
   /* Do not create an object is this is being run on the SDK. */
   *ppif = NULL;
   return EUNSUPPORTED;
#endif
#endif//WIN32
#endif //!MIN_BREW_VERSION(3,1)

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

     pNew = (ICMNotifier*)AEE_NewClass((IBaseVtbl*)&gOEMCMNotifierFuncs,
                                                       sizeof(ICMNotifier)); 

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

   if (pMe->m_coreObj)
      (void) OEMCMCore_Release();

   // Ref count is zero.  Release memory
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
static void OEMCMNotifier_SetMask(ICMNotifier *pMe, uint32 *pdwMask)
{
  if (!pMe || !pdwMask)
    return;

  pMe->m_dwNotify = OEMCM_MergeMasks(pdwMask);  
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
#ifndef WIN32  
#ifdef AEE_SIMULATOR
   /* Do not create an object is this is being run on the SDK. */
   *ppif = NULL;
   return EUNSUPPORTED;
#endif
#endif//WIN32
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

   for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) 
    if (pMe->m_coreObj->m_call_owners[i] == pMe)
      pMe->m_coreObj->m_call_owners[i] = NULL;

   /* Release core */
   if (pMe->m_coreObj)
      (void) OEMCMCore_Release();

   // Ref count is zero.  Release memory
   FREE(pMe);
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
FUNCTION: OEMCM_OriginateVoiceCall

DESCRIPTION:  
  Originate a Voice call with default options.

PROTOTYPE:
  int OEMCM_OriginateVoiceCall(ICM *po,
                        AECHAR const *number, 
                        AEECMCallID  *call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.
  number            [in]: Number to be dialed
  call_id          [out]: Call ID assigned to the call

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
                                    AEECMCallID *ptr_call_id)
{
  char                  calling[CM_MAX_NUMBER_CHARS+1] = "";
  char                  called[CM_MAX_NUMBER_CHARS+1] = "";
  char *                pcalled = called;
  cm_num_s_type         calling_number, called_number;
  cm_orig_alpha_s_type  alpha;
  uint8                 i;
  AEECMCallInfo *       call_info;
  AEECMPhInfo *         ph_info;
  AEECMSysMode          sys_mode;
  cm_cdma_orig_params_s_type * cdma_orig_ptr = NULL;
  cm_gw_cs_orig_params_s_type * gw_orig_ptr = NULL;
  AEECMEventCBS *        event_cb;
  AEECMCallErrCBPkt *   err_cb_pkt;
  boolean               flash = FALSE;
  cm_call_id_type       flash_call_id = 0;
  AEECMCallInfo *       core_call_info;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)   
#error code not present
#endif  

  if (!pMe || !pMe->m_coreObj || !ptr_call_id)
    return EBADPARM;

  sys_mode = OEMCM_GetSystemMode();

  for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) {
    if (pMe->m_coreObj->m_state[i].bValid) {
      call_info = &(pMe->m_coreObj->m_call_info[i]);
      if (AEECM_IS_CALLSTATE_CONNECTED(call_info->call_state) &&
          AEECM_IS_CALLTYPE_VOICE(call_info->call_type)) {
        flash = TRUE;
        flash_call_id = i;
        break;
      }
    }
  }

  /* Return error if phone is offline */
  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL)
    return EFAILED;

  if (ph_info->oprt_mode != AEECM_OPRT_MODE_ONLINE)
    return EBADSTATE;

  if (flash) {

    if (AEECM_IS_MODE_CDMA(sys_mode)) {

      /* Handle CDMA flash */
      if (OEMCM_SendFlash(number, flash_call_id) != AEE_SUCCESS) 
        return EFAILED;
      *ptr_call_id = flash_call_id + 1;
      return AEE_SUCCESS;

    } else {
      
      /* Non-CDMA call origination with other calls */
      if (!number)
        return EUNSUPPORTED;

    }

  } else {

    /* Call origination */
    if (!number)
      return EBADPARM;

  }

  (void) WSTRTOSTR(number, called, sizeof(called));

  if (AEECM_IS_MODE_GW(sys_mode)) {

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)           
#error code not present
#endif 

  } else {
    cdma_orig_ptr = &(pMe->m_coreObj->def_cdma_orig_params);    
  }

  OEMCM_InitNum(&called_number, pcalled);

  OEMCM_InitNum(&calling_number, calling);

  alpha.buf[0] = '\0';
  alpha.len = 0;

  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, sizeof(AEECMCallErrCBPkt));

  if (!event_cb) {
    MSG_ERROR("OEMCM_OriginateVoiceCall: Failed to allocate event callback",
               0, 0, 0);
    return EFAILED;
  }

  /* Adding one more reference to event callback to avoid the race condition 
   * when OEMCM_BREWCallErrCB is called before we take out the call_id 
   * returned below. Passing address of call id in err_cb_pkt intead of 
   * call_id_ptr so that call error, if it occurs, has an associated call id. 
   */
  OEMCM_AddRefEventCBS(event_cb);

  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;

  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_orig(OEMCM_DMSSCallErrCB, event_cb, gCMID, 
                           CM_CALL_TYPE_VOICE,
                           CM_SRV_TYPE_AUTOMATIC, &calling_number,
                           &called_number, &alpha, 
                           cdma_orig_ptr,
                           gw_orig_ptr,
                           NULL,
                           (cm_call_id_type *)&(err_cb_pkt->call_id))) {
    return EFAILED;
  }
  
  *ptr_call_id = (AEECMCallID)(err_cb_pkt->call_id + 1);

  pMe->m_coreObj->m_call_owners[*ptr_call_id - 1] = pMe;

  /* OK we got the call id */
  OEMCM_FreeEventCBS(event_cb); 

  core_call_info = &pMe->m_coreObj->m_call_info[*ptr_call_id-1];
  
  (void) MEMSET(core_call_info, 0, sizeof(AEECMCallInfo));

  OEMCM_CopyNumToOtherParty(&called_number, core_call_info);

  core_call_info->call_state = AEECM_CALL_STATE_ORIG;
  core_call_info->call_type = AEECM_CALL_TYPE_VOICE;

  return AEE_SUCCESS;
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
                        AEECMCallID  *ptr_call_id)

PARAMETERS:
  pMe               [in]: ICM interface pointer.
  call_type         [in]: Type of call to be originated
  number            [in]: Number to be dialed
  alpha             [in]: Text to be displayed along with origination
  options           [in]: Call specific options, if required
  ptr_call_id       [out]: Call ID assigned to the call

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
                             AECHAR const *number, AECHAR const *alpha,
                             ICallOpts * options, AEECMCallID *ptr_call_id)
{
  char                  calling[CM_MAX_NUMBER_CHARS+1] = "";
  char                  called[CM_MAX_NUMBER_CHARS+1] = "";
  char *                pcalled = called;
  char                  alpha_buf[CM_MAX_ALPHA_LEN_FOR_CALL_ORIG+1];
  cm_num_s_type         calling_number, called_number;
  cm_orig_alpha_s_type  cm_alpha;
  uint8                   i;
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
  AEECMEventCBS *        event_cb;
  AEECMCallErrCBPkt *   err_cb_pkt;
  boolean               flash = FALSE;
  cm_call_id_type       flash_call_id = 0;
  AEECMCallInfo *       core_call_info;

  if (!pMe || !pMe->m_coreObj || !ptr_call_id)
    return EBADPARM;

  sys_mode = OEMCM_GetSystemMode();

  if (AEECM_IS_MODE_CDMA(sys_mode)) {
    for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) {
      if (pMe->m_coreObj->m_state[i].bValid) {
        call_info = &(pMe->m_coreObj->m_call_info[i]);
        if (AEECM_IS_CALLSTATE_CONNECTED(call_info->call_state) &&
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

  /* Handle flash */
  if (flash) {
    if (OEMCM_SendFlash(number, flash_call_id) != AEE_SUCCESS) 
      return EFAILED;
    *ptr_call_id = flash_call_id + 1;
    return AEE_SUCCESS;
  } else {
    if (!number)
      return EBADPARM;
  }

  (void) WSTRTOSTR(number, called, sizeof(called));

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

  OEMCM_InitNum(&called_number, pcalled);

  OEMCM_InitNum(&calling_number, calling);

  if (!alpha) {
    cm_alpha.buf[0] = '\0';
    cm_alpha.len = 0;
  } else {
    (void) WSTRTOSTR(alpha, alpha_buf, sizeof(alpha_buf));
    cm_alpha.len = (uint8)STRLEN(alpha_buf);
    (void) STRNCPY((char *)cm_alpha.buf, alpha_buf, cm_alpha.len);
  }

  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, sizeof(AEECMCallErrCBPkt));

  if (!event_cb) {
    MSG_ERROR("OEMCM_OriginateVoiceCall: Failed to allocate event callback",
               0, 0, 0);
    return EFAILED;
  }

  /* Adding one more reference to event callback to avoid the race condition 
   * when OEMCM_BREWCallErrCB is called before we take out the call_id returned 
   * below. Passing address of call id in err_cb_pkt intead of call_id_ptr 
   * so that call error, if it occurs, has an associated call id. 
   */
  OEMCM_AddRefEventCBS(event_cb);

  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;

  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_orig(OEMCM_DMSSCallErrCB, event_cb, gCMID, 
                           OEMCM_CallTypeToDMSS(call_type),
                           srv_type, &calling_number,
                           &called_number, &cm_alpha, 
                           cdma_orig_ptr,
                           gw_orig_ptr,
                           NULL,
                           (cm_call_id_type *)&(err_cb_pkt->call_id))) {
    return EFAILED;
  }
  
  *ptr_call_id = (AEECMCallID)(err_cb_pkt->call_id + 1);

  pMe->m_coreObj->m_call_owners[*ptr_call_id - 1] = pMe;


  /* OK we got the call id */
  OEMCM_FreeEventCBS(event_cb); 

  core_call_info = &pMe->m_coreObj->m_call_info[*ptr_call_id-1];

  (void) MEMSET(core_call_info, 0, sizeof(AEECMCallInfo));

  OEMCM_CopyNumToOtherParty(&called_number, core_call_info);

  core_call_info->call_state = AEECM_CALL_STATE_ORIG;
  core_call_info->call_type = call_type;

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_AnswerCall

DESCRIPTION:  
  Answer an incoming call.

PROTOTYPE:
  int OEMCM_AnswerCall(ICM *po,
                        AEECMCallID call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID of the call to be answered

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_AnswerCall(ICM *pMe, AEECMCallID callid)
{
  AEECMCallInfo *         call_info;
  AEECMPhInfo *           ph_info;
  cm_ans_params_s_type    answer_params;
  AEECMCallType           call_type;
  AEECMSysMode            sys_mode;
  boolean                 ans = FALSE;
  AEECMEventCBS *         event_cb;
  AEECMCallErrCBPkt *     err_cb_pkt;
  cm_call_id_type         call_id;
  uint8                   i;
  AECHAR                  twoSend[] = {'2', '\0'};

  if (!pMe || !pMe->m_coreObj)
    return EBADPARM;

  if (!OEMCM_IsValidAppCallID(callid))
    return EBADPARM;

  /* Change App call id to DMSS call id */
  call_id = callid-1;

  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL)
    return EFAILED;

  if (ph_info->oprt_mode != AEECM_OPRT_MODE_ONLINE)
    return EBADSTATE;

  sys_mode = OEMCM_GetSystemMode();

  for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) {
    if (pMe->m_coreObj->m_state[i].bValid &&
        !(AEECM_IS_MODE_GW(sys_mode) && i == call_id)) {
      call_info = &(pMe->m_coreObj->m_call_info[i]);

      if (AEECM_IS_CALLSTATE_CONNECTED(call_info->call_state) &&
          AEECM_IS_CALLTYPE_VOICE(call_info->call_type)) {

        switch(sys_mode) {
          case AEECM_SYS_MODE_CDMA:
          case AEECM_SYS_MODE_HDR:
            return OEMCM_SendFlash(NULL, i);

          case AEECM_SYS_MODE_GSM:
          case AEECM_SYS_MODE_WCDMA:
            return OEMCM_CallOpsRequest(pMe, twoSend, call_id);

          default:
            break;
        }
        
        break; /* no need to look further */
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

  /* check if somebody else has already answered the call */
  /* Assuming that nobody else directly uses DMSS CM to answer calls
     that BREW Apps are interested in */
  AEECM_MUTEX_LOCK();
  if (!pMe->m_coreObj->m_state[call_id].bAnswered) {
    pMe->m_coreObj->m_state[call_id].bAnswered = TRUE;
    ans = TRUE;
  } 
  AEECM_MUTEX_UNLOCK();

  if (!ans)
    return EBADSTATE;

  (void) MEMSET( &answer_params, 0, sizeof( cm_ans_params_s_type));
  
  if (AEECM_IS_MODE_GW(sys_mode)) {
      answer_params.info_type = CM_CALL_MODE_INFO_GW_CS;
      answer_params.ans_params.gw_cs_ans.accept = TRUE;
      answer_params.ans_params.gw_cs_ans.call_type = 
                                          OEMCM_CallTypeToDMSS(call_type);
  } else {
      answer_params.info_type = CM_CALL_MODE_INFO_CDMA;
      answer_params.ans_params.cdma_ans.call_type = 
                                          OEMCM_CallTypeToDMSS(call_type);
  }


  pMe->m_coreObj->m_call_owners[call_id] = pMe;

  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, sizeof(AEECMCallErrCBPkt));

  if (!event_cb) {
    MSG_ERROR("OEMCM_AnswerCall: Failed to allocate event callback",
               0, 0, 0);
    return EFAILED;
  }

  /* Fill in call_id here. */
  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
  err_cb_pkt->call_id = call_id;

  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_answer(OEMCM_DMSSCallErrCB, event_cb, gCMID, call_id, 
                             &answer_params)) {
    pMe->m_coreObj->m_state[call_id].bAnswered = FALSE;
    return EFAILED;
  }

  return AEE_SUCCESS;
}


/*=============================================================================
FUNCTION: ICM_EndCall

DESCRIPTION:  
  End a call specified by call id.

PROTOTYPE:
  int ICM_EndCall(ICM *po,
                  AEECMCallID call_id) 

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
static int OEMCM_EndCall(ICM *pMe, AEECMCallID callid)
{
  cm_end_params_s_type  end_param;
  AEECMSysMode          sys_mode;
  AEECMCallState        call_state;
  AEECMPhInfo *         ph_info = NULL;
  boolean               end = FALSE;
  AEECMEventCBS *        event_cb;
  AEECMCallErrCBPkt *   err_cb_pkt;
  cm_call_id_type           call_id;

  if (!pMe || !pMe->m_coreObj)
    return EBADPARM;
  
  if (!OEMCM_IsValidAppCallID(callid))
    return EBADPARM;

  /* Change App call id to DMSS call id */
  call_id = callid-1;

  call_state = OEMCM_GetCallState(call_id);
  if (call_state == AEECM_CALL_STATE_IDLE ||
      call_state == AEECM_CALL_STATE_ENDED)
    return EBADSTATE;

  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL)
    return EFAILED;
  
  if (ph_info->oprt_mode != AEECM_OPRT_MODE_ONLINE) {
    return EBADSTATE;
  }

  /* check if somebody else has already ended the call */
  AEECM_MUTEX_LOCK();
  if (!pMe->m_coreObj->m_state[call_id].bEnded) {
    pMe->m_coreObj->m_state[call_id].bEnded = TRUE;
    end = TRUE;
  } 
  AEECM_MUTEX_UNLOCK();

  if (!end)
    return EBADSTATE;

  sys_mode = OEMCM_GetSystemMode();

  if (call_state == AEECM_CALL_STATE_INCOM && 
      AEECM_IS_MODE_GW(sys_mode)) {
    return OEMCM_RejectCall(pMe, call_id);
  }

  (void ) MEMSET(&end_param, 0, sizeof(cm_end_params_s_type));

  end_param.call_id = call_id;

  if (AEECM_IS_MODE_GW(sys_mode)) {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
  } else {
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    end_param.info_type = CM_CALL_MODE_INFO_CDMA;
    end_param.end_params.cdma_end.end_reason_included = TRUE; 
    end_param.end_params.cdma_end.end_reason = CAI_REL_NORMAL;
#else
    MSG_ERROR("CDMA Feature not defined in this build !!", 0, 0, 0);
    return EUNSUPPORTED;
#endif
  }
  
  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, sizeof(AEECMCallErrCBPkt));

  if (!event_cb) {
    MSG_ERROR("OEMCM_EndCall: Failed to allocate event callback",
               0, 0, 0);
    return EFAILED;
  }

  /* Fill in call_id here. */
  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
  err_cb_pkt->call_id = call_id;

  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_end(OEMCM_DMSSCallErrCB, event_cb, gCMID, 1, &end_param)) {
    pMe->m_coreObj->m_state[call_id].bEnded = FALSE;
    return EFAILED;
  }

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_EndAllCalls

DESCRIPTION:  
  End calls specified in the call list

PROTOTYPE:
  int OEMCM_EndAllCalls(ICM *po,
                  AEECMCallID *call_id_ptr, 
                  uint16 num_of_callid)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id_ptr       [in]: Array of Call ID to be ended
  num_of_callid     [in]: Number of call ids in the array

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
  AEECMCallID call_id_list[OEMCM_MAX_NUM_CALLS];
  uint16 i;
  uint16 num;
  int ret_val = AEE_SUCCESS;
  int ret;

  if (!pMe)
    return EBADPARM;

  num = OEMCM_GetActiveCallIDs(pMe, AEECM_CALL_TYPE_NONE, 
                               AEECM_CALL_STATE_NONE,
                               call_id_list, 
                               sizeof(call_id_list));

  for (i = 0; i < num; i++) {
    ret = OEMCM_EndCall(pMe, call_id_list[i]);
    if (ret != AEE_SUCCESS) {
      /* Try ending other calls */
      ret_val = EFAILED;
      MSG_ERROR("OEMCM_EndAllCalls failed to end call %d with error %d\n", 
                                              call_id_list[i], ret, 0);
    }
  }

  return ret_val;
}

/*=============================================================================
FUNCTION: OEMCM_GetActiveCallIDs

DESCRIPTION:  
  Obtain the calls matching a call_type, or all calls if call_type is
  AEECM_CALL_TYPE_NONE. Return value gives the number of call ids filled
  in.

  The function can be called without specifying the call_ids array to obtain 
  just the number of calls. 

PROTOTYPE:
  uint16 ICM_GetActiveCallIDs(ICM *po,
                    AEECMCallType call_type,
                    AEECMCallState call_state,
                    AEECMCallID *call_ids, 
                    uint32 call_ids_size)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_type         [in]: Call type
  call_state        [in]: Call state
  call_ids         [out]: Optional array of Call IDs to be filled in
  call_ids_size     [in]: Optional size of call ID array in bytes 
                          (Required if call_ids is non-NULL)

RETURN VALUE:
  Number of calls or the number of call ids stored in the
  call ID array, if specified.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint16 OEMCM_GetActiveCallIDs(ICM *pMe, AEECMCallType call_type,
                             AEECMCallState call_state,
                             AEECMCallID *call_ids, uint32 size)
{
  boolean flag[OEMCM_MAX_NUM_CALLS] = {FALSE,};
  uint16 num = 0;
  uint8 i, j;
  AEECMCallState icall_state;

  if (!pMe)
    return 0;
  
  for (i = 0;  i < OEMCM_MAX_NUM_CALLS; ++i) {

    icall_state = OEMCM_GetCallState(i);
    if (icall_state == AEECM_CALL_STATE_NONE ||
        icall_state == AEECM_CALL_STATE_IDLE ||
        icall_state == AEECM_CALL_STATE_ENDED)
      continue;

    if ((call_type == AEECM_CALL_TYPE_NONE ||
         (call_type & OEMCM_GetCallType(i))) && 
        (call_state == AEECM_CALL_STATE_NONE ||
         (call_state & icall_state))) {
      flag[i] = TRUE;
      num++;
    }
  }

  if (!call_ids)
    return num;

  j = 0;
  for (i = 0; i < OEMCM_MAX_NUM_CALLS && j < size/sizeof(AEECMCallID); i++) {
    if (flag[i])
      call_ids[j++] = i+1;
  }

  return j;
}

/*=============================================================================
FUNCTION: OEMCM_GetCallInfo

DESCRIPTION:  
  Get information about the call specified by call id.

PROTOTYPE:
  int OEMCM_GetCallInfo(ICM *po,
                      AEECMCallID call_id, 
                      AEECMCallInfo *call_info,
                      uint32 size)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID of the call to be queried
  call_info        [out]: Return call information
  size              [in]: Size of Call info structure (bytes).


RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_GetCallInfo(ICM *pMe, AEECMCallID call_id, 
                             AEECMCallInfo *call_info,
                             uint32 size)
{
  if (!pMe || !call_info)
    return EBADPARM;
  
  if (!OEMCM_IsValidAppCallID(call_id))
    return EBADPARM;

  (void ) MEMCPY(call_info, &pMe->m_coreObj->m_call_info[call_id-1], 
                 size);

  call_info->sys_mode = OEMCM_GetSystemMode();

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
  AEECMCallID           aee_call_id;

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
                              &aee_call_id, sizeof(aee_call_id))) {
    return EBADSTATE;
  }

  /* Lower layer errors will not reach App */
  if (!cm_mm_call_cmd_privacy_pref(OEMCM_DMSSCallErrCB, NULL, gCMID, 
                                   aee_call_id - 1, 
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
  AEECMSysMode           sys_mode;
  AEECMCallState         call_state;
  AEECMCallType          call_type;
  boolean                connected = FALSE;
  int                    str_len;
  uint8                  i;
  int                    ret = AEECM_NOT_CALLOPS;

  if (!pMe || !pMe->m_coreObj || !dialed_string)
    return AEECM_NOT_CALLOPS;
    
  sys_mode = OEMCM_GetSystemMode();

  for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) {
    call_state = OEMCM_GetCallState(i);
    call_type = OEMCM_GetCallType(i);
    if ((AEECM_IS_CALLSTATE_CONNECTED(call_state) ||
        call_state == AEECM_CALL_STATE_INCOM) &&
        AEECM_IS_CALLTYPE_VOICE(call_type)) {
      connected = TRUE;
      break;
    }
  }

  if (!connected)
    return AEECM_NOT_CALLOPS;

  str_len = WSTRLEN(dialed_string);
  if (str_len == 0)
    return AEECM_NOT_CALLOPS;

  if (!AEECM_IS_MODE_GW(sys_mode)) {

    if (str_len > 2)
      return AEECM_NOT_CALLOPS;

    switch(dialed_string[0]) {
      case '1':
        if (str_len == 1) {
          ret = AEECM_IS_CALLOPS;
        } else {

          if (!call_num)
            return EBADPARM;

          *call_num = (uint8) WSTRTOFLOAT(dialed_string+1);
          ret = AEECM_IS_CALLOPS_NEED_CALLID;
        }        
        break;

      default:
        break;
    }

  } else {

    switch(dialed_string[0]) {
      case '0':
      case '3':
        if (str_len == 1)
          ret = AEECM_IS_CALLOPS;
        break;

      case '1':
      case '2':
        if (str_len > 2)
          break;

        if (str_len == 1) {
          ret = AEECM_IS_CALLOPS;
        } else {

          if (!call_num)
            return EBADPARM;

          *call_num = (uint8) WSTRTOFLOAT(dialed_string+1);
          ret = AEECM_IS_CALLOPS_NEED_CALLID;
        }
        break;

      case '4':
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
                         AEECMCallID call_id) 

PARAMETERS:
  po                [in]: ICM interface pointer.
  dialed_string     [in]: Dialed String (NULL terminated)
  call_id           [in]: Optional call_id

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EBADPARM:     If call_id is not valid in an operation that
                requires call_id.
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
                                AEECMCallID call_id)
{
  AEECMSysMode                sys_mode;
  char                        number[CM_MAX_DIALED_DIGITS+1];
  cm_call_sups_params_s_type  sups_params;
  cm_call_sups_params_s_type * sups_params_ptr;
  cm_call_sups_type_e_type    sups_type = 0;
  AEECMPhInfo *               ph_info;
  AEECMEventCBS *             event_cb;
  AEECMCallErrCBPkt *         err_cb_pkt;
  int                         str_len;
  uint8                       i;
  AEECMCallState              call_state;
  AEECMCallType               call_type;
  boolean                     connected = FALSE;
  boolean                     call_ind_sups = FALSE;

  if (!pMe || !pMe->m_coreObj || !dialed_string)
    return EBADPARM;
    
  sys_mode = OEMCM_GetSystemMode();

  for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) {
    call_state = OEMCM_GetCallState(i);
    call_type = OEMCM_GetCallType(i);
    if ((AEECM_IS_CALLSTATE_CONNECTED(call_state) ||
        call_state == AEECM_CALL_STATE_INCOM) &&
        AEECM_IS_CALLTYPE_VOICE(call_type))
      connected = TRUE;
  }

  if (!connected)
    return EBADSTATE;

  /* Return error if phone is offline */
  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL)
    return EFAILED;

  if (ph_info->oprt_mode != AEECM_OPRT_MODE_ONLINE)
    return EBADSTATE;

  str_len = WSTRLEN(dialed_string);

  if (str_len == 0)
    return EBADPARM;

  /* CDMA cases */
  if (!AEECM_IS_MODE_GW(sys_mode)) {
    if (dialed_string[0] != '1')
      return EUNSUPPORTED;

    if (str_len == 1) {
      return OEMCM_EndAllCalls(pMe);
    } else {
      if (!OEMCM_IsValidAppCallID(call_id))
        return EBADPARM;

      return OEMCM_EndCall(pMe, call_id);
    }
  }

  if (str_len == 1) {
    sups_params_ptr = NULL;
    switch(dialed_string[0]) {
      case '0':
        sups_type = CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB;
        break;
      
      case '1':
        sups_type = CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL;
        break;

      case '2':
        sups_type = CM_CALL_SUPS_TYPE_HOLD_CALL;
        break;

      case '3':
        sups_type = CM_CALL_SUPS_TYPE_MULTIPARTY_CALL;
        break;

      case '4':
        sups_type = CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER;
        break;

      case '5':
        call_ind_sups = TRUE;
        break;

      default:
        return EBADPARM;
    }
  } else {
    (void ) MEMSET(&sups_params, 0, sizeof(cm_call_sups_params_s_type));
    sups_params_ptr = &sups_params;

    switch(dialed_string[0]) {
      case '1':
        if (!OEMCM_IsValidAppCallID(call_id))
          return EBADPARM;

        sups_params_ptr->call_id = call_id - 1;
        sups_type = CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL;
        break;

      case '2':
        if (!OEMCM_IsValidAppCallID(call_id))
          return EBADPARM;

        sups_params_ptr->call_id = call_id - 1;
        sups_type = CM_CALL_SUPS_TYPE_SELECT_CALL_X;
        break;

      case '4':
        if (str_len > 2 && dialed_string[1] == '*') {
          AEECMCallID aee_call_id;
          
          if (!OEMCM_GetActiveCallIDs(pMe, AEECM_CALL_TYPE_NONE, 
                                      AEECM_CALL_STATE_INCOM, 
                                      &aee_call_id, sizeof(aee_call_id))) {
            return EBADSTATE;
          }

          sups_params_ptr->call_id = aee_call_id - 1;

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
      err_cb_pkt->call_id = call_id - 1;

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
static int OEMCM_SetSystemPreference(ICM *pMe, AEECMModePref mode_pref,
                             AEECMPrefTerm pref_term, uint32 pref_duration,
                             AEECMAcqOrderPref acq_order_pref,
                             AEECMBandPref band_pref, AEECMRoamPref roam_pref,
                             AEECMHybrPref hybr_pref,
                             AEECMServDomPref srv_domain_pref,
                             AEECMNwSelModePref nw_sel_mode_pref,
                             AEECMPLMNID * plmn_ptr, 
                             AEECMPhError *status,
                             AEECallback *cb)
{
  AEECMErrCBS * err_cb = NULL;
  cm_band_pref_e_type band_pref_conv;
  
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
  if (!pMe)
    return EBADPARM;

  if (!IN_RANGE(oprt_mode, AEECM_OPRT_MODE_PWROFF, AEECM_OPRT_MODE_RESET)) {
    MSG_ERROR("OEMCM_SetOperatingMode: CM_CMD_ERR_BAD_OPRT_MODE_P", 0, 0, 0);
    return EBADPARM;
  }
 
  /* TODO: check if operating mode transition is valid 
     (is_valid_oprt_mode_switch) */

  if (!cm_ph_cmd_oprt_mode(OEMCM_DMSSPhErrCB, NULL, gCMID, 
                           (sys_oprt_mode_e_type)oprt_mode)) {
    return EFAILED;
  }

  return AEE_SUCCESS;
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
                          AEECM_SYS_MODE_WCDMA

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

  if (!pMe) 
    return EBADPARM;

  if (mode != AEECM_SYS_MODE_CDMA &&
      mode != AEECM_SYS_MODE_GSM &&
      mode != AEECM_SYS_MODE_WCDMA) {
    return EUNSUPPORTED;
  }

  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL)
    return EFAILED;

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
                AEECM_NETWORK_LIST_USER_PREFERRED)) {
    return EBADPARM;
  }

  sys_mode = OEMCM_GetSystemMode();
  if (!AEECM_IS_MODE_GW(sys_mode))
    return EUNSUPPORTED;  

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
  AEECMSysMode sys_mode;

  if (!pMe)
    return EBADPARM;

  sys_mode = OEMCM_GetSystemMode();
  if (!AEECM_IS_MODE_CDMA(sys_mode))
    return EUNSUPPORTED;

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
#ifdef FEATURE_DDTM_CNTL
#error code not present
#endif /* FEATURE_DDTM_CNTL */
#ifndef FEATURE_DDTM_CNTL
  PARAM_NOT_REF(pMe);
  PARAM_NOT_REF(ddtm_pref);
  return EUNSUPPORTED;
#else
#error code not present
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
                    AEECMCallID call_id,
                    AEECMDTMFOnLength on_length,
                    AEECMDTMFOffLength off_length,
                    AECHAR *digits)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call to send DTMFs
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
static int OEMCM_BurstDTMF(ICM *pMe, AEECMCallID call_id, 
                           AEECMDTMFOnLength on_length, 
                           AEECMDTMFOffLength off_length, AECHAR *digits)
{
  char    dtmf_digits[CM_INBAND_MAX_FWD_DIALED_DIGITS+1];
  AEECMSysMode sys_mode;
  AEECMBurstDTMFPkt *dtmf_pkt;
  int ret;

  if (!pMe || !pMe->m_coreObj)
    return EBADPARM;

  if (!OEMCM_IsValidAppCallID(call_id))
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

  sys_mode = OEMCM_GetSystemMode();

  switch (sys_mode) {

    case AEECM_SYS_MODE_CDMA:
    case AEECM_SYS_MODE_HDR:
      /* Normal burst DTMF */
      if (!cm_mm_inband_cmd_burst_dtmf(OEMCM_DMSSInbandErrCB, NULL, gCMID, 
                                      (cm_call_id_type)call_id -1,
                                      (uint8) on_length, (uint8) off_length, 
                                      (uint8) STRLEN(dtmf_digits),
                                      (uint8 *)dtmf_digits)) {
        return EFAILED;
      }
      break;

    case AEECM_SYS_MODE_GSM:
    case AEECM_SYS_MODE_WCDMA:

      /* Translate burst DTMF to start/stop */

      if((ret = OEMCM_AllocBurstDTMF(pMe, call_id, digits,
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
                        AEECMCallID call_id,
                        AECHAR digit)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID
  digit             [in]: DTMF digit

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_StartContDTMF(ICM *pMe, AEECMCallID call_id, AECHAR digit)
{
  char dtmf_digit;

  if (!pMe || !pMe->m_coreObj)
    return EBADPARM;

  if (!OEMCM_IsValidAppCallID(call_id))
    return EBADPARM;

  dtmf_digit = (char)digit;

  if (!OEMCM_CheckDTMFDigits(1, (uint8 *)&dtmf_digit)) 
    return EBADPARM;

  if (pMe->m_coreObj->m_dtmf_start_times[call_id -1] != 0)
    return EBADSTATE;

  if (!cm_mm_inband_cmd_start_cont_dtmf(OEMCM_DMSSInbandErrCB, NULL, gCMID, 
                                        (cm_call_id_type)call_id -1,
                                        (uint8)dtmf_digit)) {
    return EFAILED;
  }

  pMe->m_coreObj->m_dtmf_start_times[call_id -1] = GETUPTIMEMS();

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_StopContDTMF

DESCRIPTION:  
  Stop continous DTMF tone

PROTOTYPE:
  int OEMCM_StopContDTMF(ICM *po,
                       AEECMCallID call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_StopContDTMF(ICM *pMe, AEECMCallID call_id)
{
  dword duration;
  
  if (!pMe || !pMe->m_coreObj)
    return EBADPARM;

  if (!OEMCM_IsValidAppCallID(call_id))
    return EBADPARM;

  duration = (GETUPTIMEMS() - pMe->m_coreObj->m_dtmf_start_times[call_id - 1]);
  if (duration > AEECM_MAX_DTMF_RPT_TIME)
    duration = AEECM_MAX_DTMF_RPT_TIME;

  if (!cm_mm_inband_cmd_stop_cont_dtmf(OEMCM_DMSSInbandErrCB, NULL, gCMID, 
                                       (cm_call_id_type)call_id-1, 
                                       (word)duration)) {
    return EFAILED;
  }

  pMe->m_coreObj->m_dtmf_start_times[call_id - 1] = 0;
  
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
  if (!pMe || !pMe->m_coreObj)
    return FALSE;

  if (mode_pref == AEECM_MODE_PREF_NONE && 
      band_pref == AEECM_BAND_PREF_NONE)
    return FALSE;

  if (mode_pref == AEECM_MODE_PREF_NONE)
    return cm_is_valid_mode_band_pref((cm_mode_pref_e_type)
                                       pMe->m_coreObj->m_ph_info.mode_pref,
                                      (cm_band_pref_e_type)
                                       band_pref);

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  if (band_pref == AEECM_BAND_PREF_NONE)
    return cm_is_valid_mode_pref((cm_mode_pref_e_type)mode_pref);
#endif

  return cm_is_valid_mode_band_pref((cm_mode_pref_e_type)mode_pref, 
                                    (cm_band_pref_e_type)band_pref);
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
    return EBADPARM;

  return OEMCM_GetSystemMode();
}

/*=============================================================================
FUNCTION: OEMCM_IsMyCall

DESCRIPTION:  
  Checks if the call was originated or answered by this application/module
  (using this ICM interface pointer).

PROTOTYPE:
  boolean OEMCM_IsMyCall(ICM *po, AEECMCallID call_id)

PARAMETERS:
  po                [in]: ICM interface pointer.

RETURN VALUE:
  TRUE if call was originated/answered using this interface.
  FALSE otherwise

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean OEMCM_IsMyCall(ICM *pMe, AEECMCallID callid)
{
  if (!pMe || !pMe->m_coreObj)
    return FALSE;

  if (!OEMCM_IsValidAppCallID(callid))
    return FALSE;

  return (pMe->m_coreObj->m_call_owners[callid - 1] == pMe) ? TRUE : FALSE;
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
  
  MSG_HIGH("Received Call Event 0x%X from DMSS CM", call_event, 0, 0);

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

  evt = OEMCM_CallEventToAEECM(call_event);
  if ( evt == AEECM_EVENT_NONE) 
    return;

  MSG_MED("AEECMEvent is %d\n", evt, 0, 0);

  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, sizeof(cm_mm_call_info_s_type));
  if (!event_cb)
    return;

  event_cb->event = evt;
  (void) MEMCPY(event_cb->data, call_info_ptr, sizeof(cm_mm_call_info_s_type));

  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWCallEventCB, event_cb);
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(event_cb->cb), 0);
#else
  AEE_ResumeCallback(&(event_cb->cb), AEE_RESUME_CB_SYS);
#endif
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

  /* Handle state changes due to command error */
  OEMCM_HandleCommandError(params->call_id, call_cmd, call_cmd_err);

  err = OEMCM_CallErrorToAEECM(call_cmd_err);
  if (err == AEECM_CALL_ERROR_NONE) {
    OEMCM_FreeEventCBS(event_cb);
    return;
  }
  
  event_cb->event = AEECM_EVENT_CALL_ERROR;
  params->call_error = err;

  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWCallErrCB, event_cb);

#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(event_cb->cb), 0);
#else
  AEE_ResumeCallback(&(event_cb->cb), AEE_RESUME_CB_SYS);
#endif
  return;
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
  PARAM_NOT_REF(client_id);

  MSG_HIGH("OEMCM_DMSSOtherCallErrCB received result 0x%X for Call command %d",
            cmd_err, cmd, 0);  

  if (!ICMCoreObj || !ICMNotifierObj) {
    MSG_ERROR("OEMCM Call Callback called by DMSS", 0, 0, 0);
    return;
  }

  /* Handle state changes due to command error */
  OEMCM_HandleCommandError(call_id, cmd, cmd_err);

  /* Suppress errors from PD client */
  if (client_id == CM_CLIENT_TYPE_PD)
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

  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWCallErrCB, event_cb);

#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(event_cb->cb), 0);
#else
  AEE_ResumeCallback(&(event_cb->cb), AEE_RESUME_CB_SYS);
#endif
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
      OEMCM_DMSSGetNWEventCB(ph_info_ptr, ph_event);
      return;

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

#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(event_cb->cb), 0);
#else
  AEE_ResumeCallback(&(event_cb->cb), AEE_RESUME_CB_SYS);
#endif
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

  if (ss_event == CM_SS_EVENT_INFO) {
    /* Copy and Return */
    OEMCM_CopyInSSInfo(&ICMCoreObj->m_ss_info, ss_info_ptr);
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

#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(event_cb->cb), 0);
#else
  AEE_ResumeCallback(&(event_cb->cb), AEE_RESUME_CB_SYS);
#endif
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

#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(&(event_cb->cb), 0);
#else
  AEE_ResumeCallback(&(event_cb->cb), AEE_RESUME_CB_SYS);
#endif
  return;
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
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(cb, pMe->m_pac);
#else
  AEE_ResumeEx(cb, 0, pMe->m_pac);
#endif
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
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(cb, pMe->m_pac);
#else
  AEE_ResumeEx(cb, 0, pMe->m_pac);
#endif
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

  *pError = AEE_SUCCESS;

  switch (event) {
    case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
      OEMCM_CopyInAvailableNetworks(&ICMCoreObj->m_ph_info.available_networks, 
                                    &ph_info->available_networks);
      (void) MEMCPY(err_cb->list, 
                    &ICMCoreObj->m_ph_info.available_networks,
                    err_cb->list_size);
      break;

    case CM_PH_EVENT_PREFERRED_NETWORKS_CONF:
      OEMCM_CopyInUserPrefNetworks(&ICMCoreObj->m_ph_info.user_pref_networks, 
                                    &ph_info->user_pref_networks);
      (void) MEMCPY(err_cb->list, 
                    &ICMCoreObj->m_ph_info.user_pref_networks,
                    err_cb->list_size);
      break;

    default:
      *pError = AEECM_PH_ERROR_OTHER;
      break;
  }
#if MIN_BREW_VERSION(3,0)
  AEE_ResumeCallback(err_cb->pcb, err_cb->pOwner->m_pac);
#else
  AEE_ResumeEx(err_cb->pcb, 0, err_cb->pOwner->m_pac);
#endif
  err_cb->bInUse = FALSE;
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
  PARAM_NOT_REF(data_block_ptr);
  PARAM_NOT_REF(inband_cmd);
  PARAM_NOT_REF(inband_cmd_err);

  MSG_MED("OEMCM_DMSSInbandErrCB received result 0x%X for Inband command %d", 
                                    inband_cmd_err, inband_cmd, 0);  
}

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
  PFNOEMCMCALLNOTIFY      pfnNotify;
  uint16                  fn_index;
  cm_mm_call_info_s_type * call_info;

  if (!data || !ICMCoreObj) {
    MSG_ERROR("OEMCM_BREWCallEventCB: Unexpected NULL pointer", 0, 0, 0);
    return;
  }

  event_cb = (AEECMEventCBS *)data;

  MSG_MED("OEMCM_BREWCallEventCB: received AEECM event 0x%X", event_cb->event,
                                                          0, 0);

  event = event_cb->event;
  call_info = (cm_mm_call_info_s_type *) event_cb->data;

  fn_index = (event & 0x00FF);

  /* Find the event handler associated with this event.
   * Event handler table is indexed by the last two nibbles of event.
   * This is basically a huge switch case statement.
   */
  if (AEECM_EVENT_GROUP(event) == AEECM_CALL_GROUP) {

    /* Main call group */
    if (fn_index >= ARR_SIZE(gCallEventHandlers)) {
      MSG_ERROR("OEMCM_BREWCallEventCB: No event handler for this event (0x%X)", 
                                                              event, 0, 0);
      return;
    }
    pfnNotify = gCallEventHandlers[fn_index];

  } else if (AEECM_EVENT_GROUP(event) == AEECM_INTERNAL_CALL_GROUP) {

    /* Internal call events that are not sent to the App */
    if (fn_index >= ARR_SIZE(gCallIntEventHandlers)) {
      MSG_ERROR("OEMCM_BREWCallEventCB: No event handler for this event (0x%X)", 
                                                              event, 0, 0);
      return;
    }
    pfnNotify = gCallIntEventHandlers[fn_index];

  } else {
    MSG_ERROR("OEMCM_BREWCallEventCB: Unknown event (0x%X)", event, 0, 0);
    return;
  }

  /* Found the event handler, now call it */
  if (pfnNotify) {
    (*pfnNotify)(call_info, event);
  } else {
    MSG_ERROR("OEMCM_BREWCallEventCB: No event handler found for event 0x%X", 
                                                                  event, 0, 0);
  }

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

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    OEMCM_FreeEventCBS(event_cb);
    return;
  }

  notify_info->event = event_cb->event;
  params = (AEECMCallErrCBPkt *)event_cb->data;
  notify_info->event_data.call.call_id = params->call_id+1;
  notify_info->event_data.call.call_error = params->call_error;

  core_call_info = &(ICMCoreObj->m_call_info[params->call_id]);
  /* Copy call info from core object to Notify info */
  if (core_call_info->call_state != AEECM_CALL_STATE_IDLE ||
      core_call_info->call_state != AEECM_CALL_STATE_ENDED ) {
    (void) MEMCPY(&(notify_info->event_data.call.call_info), core_call_info, 
           sizeof(AEECMCallInfo));
  }

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
  
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

  if (!data || !ICMCoreObj)
    return;

  event_cb = (AEECMEventCBS *)data;

  event = event_cb->event;
  ph_info = (cm_ph_info_s_type *) event_cb->data;

  fn_index = (event & 0x00FF);

  /* Find the event handler associated with this event.
     Event handler table is indexed by the last two nibbles of event */
  
  if (AEECM_EVENT_GROUP(event) == AEECM_PH_GROUP) {
    if (fn_index >= ARR_SIZE(gPhEventHandlers)) {
      MSG_ERROR("OEMCM_BREWPhEventCB: No event handler for this event (0x%X)", 
                                                              event, 0, 0);
      return;
    }
    pfnNotify = gPhEventHandlers[fn_index];
  } else if (AEECM_EVENT_GROUP(event) == AEECM_INTERNAL_PH_GROUP) {
    if (fn_index >= ARR_SIZE(gPhIntEventHandlers)) {
      MSG_ERROR("OEMCM_BREWPhEventCB: No event handler for this event (0x%X)", 
                                                              event, 0, 0);
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
  
  if (!data || !ICMCoreObj)
    return;

  event_cb = (AEECMEventCBS *)data;

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    OEMCM_FreeEventCBS(event_cb);
    return;
  }

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
      break;

    case AEECM_EVENT_SS_HDR_RSSI:
      core_ss_info->hdr_rssi = ss_info->hdr_rssi;
      break;

    default:
      OEMCM_FREE(notify_info);
      return;
  }

  if (!ss_info->hdr_hybrid && OEMCM_GetSystemMode() == AEECM_SYS_MODE_HDR) {
    core_ss_info->hdr_rssi = ss_info->rssi;
    core_ss_info->hdr_roam_status = ss_info->roam_status;
    core_ss_info->hdr_srv_status = ss_info->srv_status;
  }

  (void) MEMCPY(&notify_info->event_data.ss.ss_info, core_ss_info, 
                sizeof(AEECMSSInfo));

  OEMCM_NotifyApps(notify_info);
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

  if (!data || !ICMCoreObj)
    return;

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

  notify_info->event_data.inb.call_id = call_id+1;

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);

  OEMCM_FreeEventCBS(event_cb);

}

/*===========================================================================

                          Event Handlers

===========================================================================*/
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
  cm_setup_res_params_s_type setup_params;
  cm_bearer_capability_s_type * default_bearer_capability;
 
  MSG_MED("OEMCM_HandleCallSetupInd received event 0x%X", event, 0, 0);

  if (event != AEECM_EVENT_INTERNAL_SETUP_IND) {
    MSG_ERROR("OEMCM_HandleCallSetupInd received unexpected event 0x%X", 
                                                              event, 0, 0);
    return;
  }

  if (call_info->call_type != CM_CALL_TYPE_VOICE)
    return;

  default_bearer_capability = 
    &ICMCoreObj->def_gw_orig_params.bearer_capability_1;

  (void) MEMSET(&setup_params, CM_CALL_CMD_PARAM_DEFAULT_VALUE, 
         sizeof(cm_setup_res_params_s_type));

  if (call_info->call_type == CM_CALL_TYPE_VOICE) {
    setup_params.accept = TRUE;
    (void) MEMCPY(&setup_params.bearer_capability_1, 
                  default_bearer_capability, 
                  sizeof(cm_bearer_capability_s_type));

    if (!cm_mm_call_cmd_setup_res(OEMCM_DMSSCallErrCB, NULL, gCMID, 
                                    call_info->call_id,
                                    &setup_params)) {
      MSG_ERROR("OEMCM: cm_mm_call_cmd_setup_res failed", 0, 0, 0);
    }
  }

  ICMCoreObj->m_state[call_info->call_id].bSetup = TRUE;

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

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
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

  MSG_MED("OEMCM_HandleCallDefault received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  core_call_info = &(ICMCoreObj->m_call_info[call_info->call_id]);  

  core_call_info->call_progress = (AEECMCallProgress) 
                        call_info->mode_info.info.gw_cs_call.call_progress;
  
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

  MSG_MED("OEMCM_HandleCallCopyNum received event 0x%X", event, 0, 0);

  if (!call_info || !ICMCoreObj)
    return;

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  core_call_info = &(ICMCoreObj->m_call_info[call_info->call_id]);

  OEMCM_CopyNumToOtherParty(&(call_info->num), core_call_info);

  OEMCM_CopyCharLenToAECHAR((char *)call_info->num.subaddr.chari, 
                             call_info->num.subaddr.num_char,
                             core_call_info->other_party_subaddr, 
                             sizeof(core_call_info->other_party_subaddr)); 

  OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf, 
                            call_info->alpha.len,
                            core_call_info->alpha, 
                            sizeof(core_call_info->alpha));

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_CopyNumToAEECMNum(&call_info->num, 
                          &notify_info->event_data.call.number);

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

  MSG_MED("OEMCM_HandleCallAlphaUpdate received event 0x%X %d", event, call_info->call_id, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }
  if(call_info->call_id < OEMCM_MAX_NUM_CALLS)
  {
      call_id = call_info->call_id;
      core_call_info = &(ICMCoreObj->m_call_info[call_id]);  

      OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf, 
                                call_info->alpha.len,
                                core_call_info->alpha, 
                                sizeof(core_call_info->alpha));
      OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);
      (void) WSTRLCPY(notify_info->event_data.call.alpha, core_call_info->alpha,
           /*sizeof(notify_info->event_data.call.alpha)*/AEECM_MAX_ALPHA_TAG_LENGTH);
  }
  else
  {
    AECHAR  alpha[AEECM_MAX_ALPHA_TAG_LENGTH] = {0};
    OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf, 
                            call_info->alpha.len,
                            alpha, 
                            sizeof(alpha));
    OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);
    (void) WSTRLCPY(notify_info->event_data.call.call_info.alpha, alpha,AEECM_MAX_ALPHA_TAG_LENGTH);
  }
  notify_info->event_data.call.call_info.is_last_cdma_info_rec = 
                             call_info->mode_info.info.cdma_call.is_last_cdma_info_rec;
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

  PARAM_NOT_REF(event);
  MSG_MED("OEMCM_HandleCallExtDisplay received event 0x%X", event, 0, 0);

  if (call_info->mode_info.info_type != CM_CALL_MODE_INFO_CDMA) {
    MSG_ERROR("%d CM_CALL_EVENT_EXT_DISP received with wrong mode", 
                                               call_info->mode_info.info_type,
                                               0, 0);
    //return;//cdg 9.71
  }

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);  

  OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf, 
                            call_info->alpha.len,
                            core_call_info->alpha, 
                            sizeof(core_call_info->alpha));

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, 
                              AEECM_EVENT_CALL_DISPLAY);

  (void) WSTRLCPY(notify_info->event_data.call.alpha, core_call_info->alpha,
           /*sizeof(notify_info->event_data.call.alpha)*/AEECM_MAX_ALPHA_TAG_LENGTH);

  notify_info->event_data.call.disp_type = (AEECMDispType) 
                             call_info->mode_info.info.cdma_call.ext_disp_type;
  notify_info->event_data.call.call_info.is_last_cdma_info_rec = 
                             call_info->mode_info.info.cdma_call.is_last_cdma_info_rec;
  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallOrig

DESCRIPTION:  
  

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

  MSG_MED("OEMCM_HandleCallOrig received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);  

  /* Update core info */

  OEMCM_ResetPrivateCallState(call_id);

  (void) MEMSET(core_call_info, 0, sizeof(AEECMCallInfo));

  core_call_info->call_state = AEECM_CALL_STATE_ORIG;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);
  core_call_info->direction = AEECM_CALL_DIRECTION_MO;
  
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
           /*sizeof(notify_info->event_data.call.alpha)*/AEECM_MAX_ALPHA_TAG_LENGTH);

  OEMCM_CopyNumToAEECMNum(&call_info->num, 
                          &notify_info->event_data.call.number);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
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
  cm_call_id_type       call_id;
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
#if 1
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

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);

  ICMCoreObj->m_call_owners[call_id] = NULL;
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

  MSG_MED("OEMCM_HandleCallIncom received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);  

  /* Update core call info */
  if (ICMCoreObj->m_state[call_id].bSetup) {
    ICMCoreObj->m_state[call_id].bSetup = FALSE;
    bSignalled = ICMCoreObj->m_state[call_id].bSignalled;
  }

  OEMCM_ResetPrivateCallState(call_id);

  (void) MEMSET(core_call_info, 0, sizeof(AEECMCallInfo));

  core_call_info->call_state = AEECM_CALL_STATE_INCOM;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);
  core_call_info->direction = AEECM_CALL_DIRECTION_MT;
  core_call_info->start_time = GETUPTIMEMS();
                                                
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

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
    (void) MEMCPY(&notify_info->event_data.call.signal, &call_info->signal, 
           sizeof(AEECMSignal));
  }     

  OEMCM_CopyNumToAEECMNum(&call_info->num, 
                          &notify_info->event_data.call.number);

  if (call_info->signal.is_signal_info_avail && !bSignalled)
    OEMCM_HandleCallSignal(call_info, AEECM_EVENT_CALL_SIGNAL);

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

  core_call_info->call_state = AEECM_CALL_STATE_INCOM;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);
                                                
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

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
    core_call_info->srv_opt = 
            (AEECMServiceOption)call_info->mode_info.info.cdma_call.srv_opt;
    core_call_info->is_privacy = 
            call_info->mode_info.info.cdma_call.is_privacy;
            
  }

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

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_GW_CS) {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
  } else {

    if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {

      /* Copy connected num */
      if (call_info->num.number_type == CM_TON_INTERNATIONAL) {
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

    }
  }

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

  OEMCM_CopyNumToAEECMNum(&call_info->num, 
                          &notify_info->event_data.call.number);

  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallSupsStateChange

DESCRIPTION:  
  

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallSupsStateChange(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEECMNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEECMCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallSupsStateChange received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);  

  switch (event) {
    case AEECM_EVENT_CALL_ON_HOLD:
      core_call_info->call_state = AEECM_CALL_STATE_ONHOLD;
      break;

    case AEECM_EVENT_CALL_RETRIEVED:
      core_call_info->call_state = AEECM_CALL_STATE_CONV;
      break;

    default:
      MSG_ERROR("OEMCM_HandleCallSupsStateChange received unexpected event 0x%X", 
                        event, 0, 0);
      OEMCM_FREE(notify_info);
      return;
  }

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

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
#if 1
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
  if(call_info->call_state == CM_CALL_STATE_CONV && 
    call_info->mode_info.info.cdma_call.line_ctrl_polarity_included)
  {
      MSG_MED("call have been accept,send Notify to call",0,0,0);
      OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);
  }
  OEMCM_NotifyApps(notify_info);
  OEMCM_FREE(notify_info);
}
#ifdef FEATURE_REDIRECTING_NUMBER_INFO_REC
/**************************************************************************************************
*** void OEMCM_HandleCallRedirUpdate(cm_mm_call_info_s_type *call_info, AEECMEvent event)
***
*** Description
*** - Handle call forwarding number update
***
*** Parameter description:
***	- *call_info: Call info
***	- event: Call event
***
*** Return values' range and constant definition:
***	- N/A
***
*** Global variables used:
***	- N/A
***
*** Side effect:
*** - N/A
**************************************************************************************************/
static void OEMCM_HandleCallRedirUpdate(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
    AEECMNotifyInfo * notify_info;
    cm_call_id_type       call_id;
    AEECMCallInfo *   core_call_info;

    MSG_MED("OEMCM_HandleCallRedirUpdate received event 0x%x", event, 0, 0);

    if ((notify_info = OEMCM_MALLOC(sizeof(AEECMNotifyInfo))) == NULL)
    {
        return;
    }

    call_id = call_info->call_id;
    core_call_info = &(ICMCoreObj->m_call_info[call_id]);  
#if 0
    OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf, 
                                call_info->alpha.len,
                                core_call_info->forwarded_from,
                                sizeof(core_call_info->forwarded_from));
#else
    OEMCM_CopyCharLenToAECHAR((char *)call_info->redirecting_number.num.buf, 
                                call_info->redirecting_number.num.len,
                                core_call_info->redirect_party_number.buf,
                                sizeof(core_call_info->redirect_party_number.buf));
#endif
    notify_info->event_data.call.call_info.is_last_cdma_info_rec = 
                                call_info->mode_info.info.cdma_call.is_last_cdma_info_rec;
    OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, event);

    OEMCM_NotifyApps(notify_info);
    OEMCM_FREE(notify_info);
}
#endif
#endif
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
static int OEMCM_AllocBurstDTMF(ICM *pMe, AEECMCallID call_id,
                                AECHAR *digits,
                                AEECMDTMFOnLength on_length,
                                AEECMDTMFOffLength off_length,
                                AEECMBurstDTMFPkt **bdtmf)
{
  AEECMBurstDTMFPkt * dtmf_pkt;
  boolean bInUse = FALSE;
  
  if (!bdtmf || !digits || !ICMCoreObj || !OEMCM_IsValidAppCallID(call_id))
    return EFAILED;

  dtmf_pkt = (AEECMBurstDTMFPkt *)OEMCM_MALLOC(sizeof(AEECMBurstDTMFPkt));
  if (!dtmf_pkt)
    return ENOMEMORY;
  
  dtmf_pkt->dtmfs = (AECHAR *) 
                     OEMCM_MALLOC(((uint32)WSTRLEN(digits)+1)*sizeof(AECHAR));
  if (!dtmf_pkt->dtmfs) {
    OEMCM_FREE(dtmf_pkt);
    return ENOMEMORY;
  }

  /* See if somebody else is already doing burst on the same call */
  AEECM_MUTEX_LOCK();
  if (ICMCoreObj->m_bdtmf[call_id - 1])
    bInUse = TRUE;
  else
    ICMCoreObj->m_bdtmf[call_id - 1] = dtmf_pkt;
  AEECM_MUTEX_UNLOCK();

  if (bInUse) {
    OEMCM_FREE(dtmf_pkt->dtmfs);
    OEMCM_FREE(dtmf_pkt);
    MSG_MED("Previously started Burst DTMF not over yet", 0, 0, 0);
    return EBADSTATE;
  }

  (void) WSTRCPY(dtmf_pkt->dtmfs, digits);
  dtmf_pkt->pOwner = pMe;
  dtmf_pkt->call_id = call_id; 
  dtmf_pkt->curr = dtmf_pkt->dtmfs;
  dtmf_pkt->on_length = (uint8) on_length;
  dtmf_pkt->off_length = (uint8) off_length;
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

  ICMCoreObj->m_bdtmf[dtmf_pkt->call_id - 1] = NULL;

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
    if (OEMCM_StartContDTMF(bdtmf->pOwner, bdtmf->call_id, *bdtmf->curr) != 
                                                                  AEE_SUCCESS) {
      MSG_ERROR("Start continuous DTMF failed for %d, digit %c", 
                        bdtmf->call_id, (char)*bdtmf->curr, 0);
    } else {
      /* Do stop next time */
      bdtmf->start = FALSE;
    }

    bdtmf->curr++;

  } else {

    /* Do stop continous DTMF */
    if (OEMCM_StopContDTMF(bdtmf->pOwner, bdtmf->call_id) != AEE_SUCCESS) {
      MSG_ERROR("Stop continuous DTMF failed for %d", bdtmf->call_id, 0 , 0);
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
                   bdtmf) != AEE_SUCCESS) {
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
      return (on_length+2)*50;

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
#ifndef WIN32
    if (prev->data)
      mem_free(&tmc_heap, prev->data);

    mem_free(&tmc_heap, prev);    
#endif
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
#ifndef WIN32
  pcb = (AEECMEventCBS *) mem_malloc(&tmc_heap, sizeof(AEECMEventCBS));
  if (!pcb)
    return NULL;

  (void) memset(pcb, 0, sizeof(AEECMEventCBS));

  if (data_size) {
    pcb->data = mem_malloc(&tmc_heap, data_size);
    if (!pcb->data) {
      mem_free(&tmc_heap, pcb);
      return NULL;
    }
    (void) memset(pcb->data, 0, data_size); 
  }

  pcb->nRef = 1;

  AEECM_MUTEX_LOCK();

  OEMCM_ADD_NODE(pMe->m_event_cbs, pcb);

  AEECM_MUTEX_UNLOCK();
#endif
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
    return;

  CALLBACK_Cancel(&pcb->cb);

  AEECM_MUTEX_LOCK();

  OEMCM_REMOVE_NODE(pMe->m_event_cbs, pcb);
  
  AEECM_MUTEX_UNLOCK();  
#ifndef WIN32
  if (pcb->data)
    mem_free(&tmc_heap, pcb->data);

  mem_free(&tmc_heap, pcb);
#endif
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

    ICMCoreObj = (ICMCore *)OEMCM_MALLOC(sizeof(ICMCore));
    if (!ICMCoreObj)
       return NULL;

    ICMCoreObj->m_uRefs = 1;

    /* 
    for (i = 0; i < OEMCM_MAX_NUM_CALLS; ++i) { 
      ICMCoreObj->m_state[i].bAnswered = FALSE;
      ICMCoreObj->m_state[i].bEnded = FALSE;
      ICMCoreObj->m_state[i].bValid = FALSE;
      (void) MEMSET(&(ICMCoreObj->m_call_info[i]), 0, sizeof(AEECMCallInfo));

      ICMCoreObj->m_dtmf_start_times[i] = 0;
    }

    ICMCoreObj->m_ph_valid = FALSE;
    ICMCoreObj->m_ss_valid = FALSE;
    */

    OEMCM_InitDefaultCallParams(ICMCoreObj);

    ICMCoreObj->m_event_cbs = NULL;

    if(!OEMCM_InitCallManager()) {
      OEMCM_FREE(ICMCoreObj);
      ICMCoreObj = NULL;
      return NULL;
    }

    for (i = 0; i < OEMCM_MAX_NUM_CALLS; i++)
      (void) cm_mm_call_cmd_get_call_info(NULL, NULL, gCMID, i); 

    (void) cm_ph_cmd_get_ph_info(NULL, NULL, gCMID);

    (void) cm_ss_cmd_get_ss_info(NULL, NULL, gCMID);    
    
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

  // Ref count is zero.  Release memory
  OEMCM_FREE(pMe);
  ICMCoreObj = NULL;
  return (0);
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
#ifndef WIN32
    if (cm_client_init(CM_CLIENT_TYPE_BREW,&gCMID) != CM_CLIENT_OK 
        || !gCMID) {
      ERR("Unable to Initialize DMSS Call Manager", 0, 0, 0);   
      return FALSE;
    }
#endif
    /* Register callbacks */
#if 1
    (void) cm_mm_client_call_reg(  gCMID,          /*client*/
                            OEMCM_DMSSCallEventCB,       /*call_event_func*/
                            CM_CLIENT_EVENT_REG,             /*event_reg_type*/
                            CM_CALL_EVENT_ORIG,               /* register from this event (including) */
                            CM_CALL_EVENT_LINE_CTRL,       /* to this event (including) */
                            OEMCM_DMSSOtherCallErrCB);  /*call_cmd_err_func*/
#else
    (void) cm_mm_client_call_reg(gCMID, OEMCM_DMSSCallEventCB, CM_CLIENT_EVENT_REG,
                          CM_CALL_EVENT_ORIG, CM_CALL_EVENT_EXIT_TC, 
                          OEMCM_DMSSOtherCallErrCB);
#endif
    (void) cm_client_ph_reg(gCMID, OEMCM_DMSSPhEventCB, CM_CLIENT_EVENT_REG, 
                     CM_PH_EVENT_OPRT_MODE, CM_PH_EVENT_SET_ACMMAX_COMPLETED, 
                     NULL);
    (void) cm_mm_client_ss_reg(gCMID, OEMCM_DMSSSSEventCB, CM_CLIENT_EVENT_REG, 
                        CM_SS_EVENT_SRV_CHANGED, CM_SS_EVENT_HDR_RSSI, 
                        NULL);
    (void) cm_mm_client_inband_reg(gCMID, OEMCM_DMSSInbandEventCB, 
                            CM_CLIENT_EVENT_REG, 
                            CM_INBAND_EVENT_REV_BURST_DTMF, 
                            CM_INBAND_EVENT_REV_BURST_DTMF_ACK, 
                            NULL);
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
FUNCTION: OEMCM_HandleCommandError

DESCRIPTION:  
  Handle state changes due to command error

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void OEMCM_HandleCommandError(cm_call_id_type call_id,
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

      case CM_CALL_CMD_ORIG:
        ICMCoreObj->m_state[call_id].bValid = FALSE;
        break;

      default:
        break;
    }
  }
}

/*=============================================================================
FUNCTION: OEMCM_SendFlash

DESCRIPTION:  
  Send CDMA Flash. 


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMCM_SendFlash(AECHAR const *number, AEECMCallID flash_call_id)
{
  AEECMEventCBS *             event_cb;
  cm_call_sups_params_s_type  sups_params;
  AEECMCallErrCBPkt *         err_cb_pkt;
  char                        called[CM_MAX_NUMBER_CHARS+1];
  cm_num_s_type               called_number;

  if (!ICMCoreObj)
    return EFAILED;

  if (number)
    (void) WSTRTOSTR(number, called, sizeof(called));
  else
    called[0] = '\0';

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

  if((p = OEM_Malloc(size)) == NULL) {
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
    /*lint -e534 */ 
    OEM_Free(p);
    /*lint +e534 */ 

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
#ifndef WIN32
  return (AEECMSysMode) sd_misc_get_curr_acq_sys_mode();
#else
	return 0;
#endif
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
static boolean OEMCM_IsValidAppCallID(AEECMCallID call_id)
{
  if(!ICMCoreObj || !call_id || call_id > OEMCM_MAX_NUM_CALLS)
    return FALSE;

  return ICMCoreObj->m_state[call_id-1].bValid;
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
FUNCTION: OEMCM_InitNum

DESCRIPTION:  


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_InitNum(cm_num_s_type *num, char *number)
{
  char buf[CM_MAX_NUMBER_CHARS+1];
  char *c;
  
  if (!num || !number)
    return;
  
  if (*number == '+') {
    num->number_type = CM_TON_INTERNATIONAL;
    number++;
  } else {
    num->number_type = CM_TON_UNKNOWN;
  }
  
  c = buf;
  while(*number && c < (buf+CM_MAX_NUMBER_CHARS)) {
    if ((*number >= '0' && *number <= '9') || *number == '*' || 
         *number == '#' || *number == '+') {
      *c++ = *number;
    }

    number++;
  }
  *c = '\0';
  
  (void) STRNCPY((char *) num->buf, buf, sizeof(num->buf));

  num->len = (byte)STRLEN(buf);
  num->digit_mode = CM_DIGIT_MODE_4BIT_DTMF;

#if ( defined(FEATURE_GSM) && defined(FEATURE_WCDMA) && !defined(FEATURE_CDMA_800) && !defined(FEATURE_CDMA_1900))  
  num->number_plan = CM_NPI_ISDN;
#else
  num->number_plan = CM_NPI_UNKNOWN;
#endif

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

  tmp = (char *)OEMCM_MALLOC((len+1)*sizeof(char));

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
static void OEMCM_CopyNumToOtherParty(cm_num_s_type *num, 
                                      AEECMCallInfo *core_call_info)
{
  if (!num || !core_call_info)
    return;

  if (num->len) {
    if(num->number_type == CM_TON_INTERNATIONAL) {
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
  OEMCM_CopyCharLenToAECHAR((char *)num->buf, num->len, aee_num->buf, 
                            sizeof(aee_num->buf));

  aee_num->number_type = (AEECMNumberType)(num->number_type >> 4);
  aee_num->number_plan = (AEECMNumberPlan)num->number_plan;
  aee_num->pi = (AEECMPI)num->pi; /* same value */
  aee_num->si = (AEECMSI)num->si; /* same value */
  
  aee_num->subaddr.extension_bit = num->subaddr.extension_bit;
  aee_num->subaddr.odd_even_ind = num->subaddr.odd_even_ind;

  OEMCM_CopyCharLenToAECHAR((char *)num->subaddr.chari, num->subaddr.num_char,
                            aee_num->subaddr.addr, 
                            sizeof(aee_num->subaddr.addr));
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

  notify_info->event_data.call.call_id = (AEECMCallID)call_id+1;

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

  if (!ICMNotifierObj || !notify_info)
    return;

  switch (AEECM_EVENT_GROUP(notify_info->event)) {

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

    default:
      return;
  }

  MSG_HIGH("Sending event 0x%X to App", notify_info->event, 0, 0);

  (void) ISHELL_Notify(ICMNotifierObj->m_pIShell, AEECLSID_CM_NOTIFIER, nMask, notify_info);

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
    *srv_type = (cm_srv_type_e_type) (*((AEECMServiceType *)opt.pVal) - 1);
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
static boolean OEMCM_CheckDTMFOnLength(int32 on_length)
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
static boolean OEMCM_CheckDTMFOffLength(int32 off_length)
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
#ifndef WIN32
    if( !DECCHK(digit_buf[i]) &&
          digit_buf[i] != '*'   &&
          digit_buf[i] != '#' )
    {
      return FALSE;
    }
#endif
  }

  return TRUE;
}

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
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
#endif
#if 0
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

  if (!pIShell)
    return;
    
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
                             AEECALLHISTORY_FIELD_NUMBER_EXT,
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
#endif
/*=============================================================================
FUNCTION: OEMCM_RejectCall

DESCRIPTION:  
  Reject an incoming call.

PROTOTYPE:
  int OEMCM_RejectCall(ICM *po,
                     cm_call_id_type call_id) 

PARAMETERS:
  po                [in]: ICM interface pointer.
  call_id           [in]: Call ID of the call to be rejected

RETURN VALUE:
  AEE_SUCCESS:  If procedure was started correctly.
  EFAILED:      Otherwise.

COMMENTS:
  This function is meaningful only for GSM/WCDMA modes.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMCM_RejectCall(ICM *pMe, cm_call_id_type call_id)
{
  AEECMCallInfo *         call_info;
  cm_ans_params_s_type    answer_params;
  AEECMEventCBS *         event_cb;
  AEECMCallErrCBPkt *     err_cb_pkt;

  call_info = &(pMe->m_coreObj->m_call_info[call_id]);
  if (call_info->alert_ans_bypass)
    return AEE_SUCCESS;

  (void) MEMSET( &answer_params, 0, sizeof( cm_ans_params_s_type));
  
  answer_params.info_type = CM_CALL_MODE_INFO_GW_CS;
  answer_params.ans_params.gw_cs_ans.accept = FALSE;
  answer_params.ans_params.gw_cs_ans.call_type = 
                                   OEMCM_CallTypeToDMSS(OEMCM_GetCallType(call_id));

  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, sizeof(AEECMCallErrCBPkt));

  if (!event_cb) {
    MSG_ERROR("OEMCM_RejectCall: Failed to allocate event callback",
               0, 0, 0);
    return EFAILED;
  }

  /* Fill in call_id here. */
  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
  err_cb_pkt->call_id = call_id;

  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_answer(OEMCM_DMSSCallErrCB, event_cb, gCMID, call_id, 
                             &answer_params)) {
    pMe->m_coreObj->m_state[call_id].bAnswered = FALSE;
    return EFAILED;
  }

  return AEE_SUCCESS;
}
/*===========================================================================

                            COPY DATA FUNCTIONS

===========================================================================*/

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

  dest->call_state = AEECM_BM(src->call_state);
  dest->call_type = OEMCM_CallTypeToAEECM(src->call_type);
  dest->direction = AEECM_CALL_DIRECTION_NONE;

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
  if (!dest || !src) {
    MSG_ERROR("OEMCM_CopyInPhInfo received a NULL pointer", 0, 0, 0);
    return;
  }

  dest->is_in_use = src->is_in_use;
  dest->oprt_mode = (AEECMOprtMode)src->oprt_mode;
  dest->cdma_lock_mode = (AEECMCDMALockMode)src->cdma_lock_mode;
  dest->answer_voice = (AEECMAnsVoicePref)src->answer_voice;
  dest->mode_pref = (AEECMModePref)src->mode_pref;
  dest->pref_term = (AEECMPrefTerm)
                    (src->pref_term > CM_PREF_TERM_1_CALL_OR_TIME ? 
                                      CM_PREF_TERM_1_CALL_OR_TIME :
                                      src->pref_term);
  dest->acq_order_pref = (AEECMAcqOrderPref)src->acq_order_pref;
  dest->srv_domain_pref = (AEECMServDomPref)src->srv_domain_pref;
  dest->band_pref = (AEECMBandPref)src->band_pref;
  dest->roam_pref = (AEECMRoamPref)src->roam_pref;
  dest->nam_sel = (AEECMNAM)src->nam_sel;
  dest->curr_nam = (AEECMNAM)src->curr_nam;
  dest->hybr_pref = OEMCM_HybrPrefToAEECM(src->hybr_pref);
  dest->network_sel_mode_pref = (AEECMNwSelModePref)src->network_sel_mode_pref;
  (void) MEMCPY(&dest->plmn, &src->plmn, sizeof(AEECMPLMNID));

  OEMCM_CopyInUserPrefNetworks(&dest->user_pref_networks, 
                               &src->user_pref_networks);

  OEMCM_CopyInAvailableNetworks(&dest->available_networks, 
                                &src->available_networks);

  dest->is_cdma_subscription_available = src->is_cdma_subscription_available;
  dest->is_gw_subscription_available = src->is_gw_subscription_available;
  dest->rtre_config = (AEECMRTREConfig)src->rtre_config;
  dest->rtre_control = (AEECMRTREControl)src->rtre_control;

  dest->ddtm_pref = (AEECMDDTMPref)src->ddtm_pref;  

  ICMCoreObj->m_ph_valid = TRUE;
}

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

  dest->srv_status = (AEECMSrvStatus)src->srv_status;
  dest->hdr_srv_status = (AEECMSrvStatus)src->hdr_srv_status;
  dest->srv_domain = (AEECMSrvDomain)src->srv_domain;
  dest->srv_capability = (AEECMSrvCapability)src->srv_capability;
  dest->sys_mode = (AEECMSysMode)src->sys_mode;
  dest->roam_status = (AEECMRoamStatus)src->roam_status;
  dest->hdr_roam_status = (AEECMRoamStatus)src->hdr_roam_status;
  OEMCM_CopyInSysID(&dest->sys_id, &src->sys_id);

  dest->rssi = src->rssi;
  dest->hdr_rssi = src->hdr_rssi;
  if (AEECM_IS_MODE_CDMA(dest->sys_mode)) {
    dest->band_class = (AEECMSysBandClass)src->mode_info.cdma_info.band_class;
    dest->block_or_system = (AEECMSysBlocks)
                             src->mode_info.cdma_info.block_or_system;
    dest->is_registered = src->mode_info.cdma_info.is_registered;
    dest->uz_id = src->mode_info.cdma_info.uz_id;
    
    (void) MEMCPY(&dest->uz_name, &src->mode_info.cdma_info.uz_name,
                  sizeof(AEECMUZName));

  } else {
    dest->sim_state = (AEECMSIMState) src->mode_info.gw_info.sim_state;
    dest->plmn_forbidden = src->mode_info.gw_info.plmn_forbidden;

    OEMCM_CopyInMMInfo(&dest->mm_information, 
                       &src->mode_info.gw_info.mm_information);

  }

  ICMCoreObj->m_ss_valid = TRUE;
}

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

  if (mm_info->plmn_avail)
    (void) MEMCPY(&mmInfo->plmn, &mm_info->plmn, sizeof(AEECMPLMNID));

  if (mm_info->full_name_avail)
    OEMCM_CopyInFullNWName(&mmInfo->full_name, &mm_info->full_name);
  
  if (mm_info->short_name_avail)
    OEMCM_CopyInShortNWName(&mmInfo->short_name, &mm_info->short_name);

  if (mm_info->univ_time_and_time_zone_avail) {

    julian_time.wYear   = mm_info->univ_time_and_time_zone.year;
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

/*=============================================================================
FUNCTION: OEMCM_CopyInAvailableNetworks

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
  
  (void) MEMCPY(dest->name, src->name, AEECM_MAX_NW_NAME_CHARS);
}

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

  return AEECM_BM(call_type);
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

  if (call_type != (1 << i))
    return CM_CALL_TYPE_NONE;

  return i;
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

    default:
      return AEECM_HYBR_PREF_NONE;
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

    default:
      return CM_HYBR_PREF_NONE;
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
static AEECMPhError OEMCM_PhErrorToAEECM(cm_ph_cmd_e_type error)
{
  AEECMPhError err = AEECM_PH_ERROR_NONE;

  switch(error) {
    case CM_PH_CMD_ERR_NOERR:
      break;

    case CM_PH_CMD_ERR_ANSWER_VOICE_F:
    case CM_PH_CMD_ERR_CLIENT_ID_P:
    case CM_PH_CMD_ERR_OPRT_MODE_P:
    case CM_PH_CMD_ERR_MODE_PREF_P:
    case CM_PH_CMD_ERR_PREF_TERM_P:
    case CM_PH_CMD_ERR_SYS_PREF_P:
    case CM_PH_CMD_ERR_BAND_PREF_P:
    case CM_PH_CMD_ERR_ROAM_PREF_P:
    case CM_PH_CMD_ERR_ANSWER_VOICE_P:
    case CM_PH_CMD_ERR_ANSWER_DURATION_P:
    case CM_PH_CMD_ERR_NAM_P:
    case CM_PH_CMD_ERR_PACKET_STATE_P:
    case CM_PH_CMD_ERR_HYBR_PREF_P:
    case CM_PH_CMD_ERR_SERVICE_DOMAIN_P:
    case CM_PH_CMD_ERR_ACQ_ORDER_PREF_P:
    case CM_PH_CMD_ERR_NETWORK_LIST_TYPE_P:
    case CM_PH_CMD_ERR_NETWORK_SEL_MODE_P:
    case CM_PH_CMD_ERR_SYSTEM_MODE_P:
    case CM_PH_CMD_ERR_PLMN_P:
    case CM_PH_CMD_ERR_RTRE_CONFIG_P:
    case CM_PH_CMD_ERR_AVOID_SYS_P:
    case CM_PH_CMD_ERR_GET_NETWORKS_S:
      MSG_ERROR("Unexpected PH command error 0x%X", error, 0, 0);
      err = AEECM_PH_ERROR_OTHER;
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

    case CM_PH_CMD_ERR_NO_BUF_L:
    case CM_PH_CMD_ERR_MC_COM_L:
    case CM_PH_CMD_ERR_OTHER:
    case CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE:
    case CM_PH_CMD_ERR_USER_PREFERRED_NETWORK_LIST_ACCESS:
      err = AEECM_PH_ERROR_OTHER;
      break;

    default:
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

    case CM_CALL_CMD_ERR_NO_COLLOC_HDR:
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
      /* Should not get these errors */
      err = AEECM_CALL_ERROR_OTHER;
      break;

    case CM_CALL_CMD_ERR_NOERR:
    default:
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
      
    case CM_CALL_EVENT_CALL_ON_HOLD:
      evt = AEECM_EVENT_CALL_ON_HOLD;
      break;

    case CM_CALL_EVENT_CALL_RETRIEVED:
      evt = AEECM_EVENT_CALL_RETRIEVED;
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
#ifdef FEATURE_REDIRECTING_NUMBER_INFO_REC
    case CM_CALL_EVENT_REDIRECTING_NUMBER:
      evt = AEECM_EVENT_CALL_REDIR_NUM;
      break;
#endif

    case CM_CALL_EVENT_LINE_CTRL:
      evt = AEECM_EVENT_CALL_LINE_CTRL;

    //case CM_CALL_EVENT_EXIT_TC:
    //  evt = AEECM_EVENT_CALL_EXIT_TC;
    case CM_CALL_EVENT_CALL_CONF:
    case CM_CALL_EVENT_PRIVACY_PREF:
    case CM_CALL_EVENT_SETUP_RES:
    case CM_CALL_EVENT_NDSS_START:
    case CM_CALL_EVENT_NDSS_CONNECT:
    case CM_CALL_EVENT_EXT_BRST_INTL:
    case CM_CALL_EVENT_NSS_CLIR_REC:
    case CM_CALL_EVENT_NSS_REL_REC:
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
    case CM_CALL_EVENT_PS_DATA_AVAILABLE:
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

    case CM_SS_EVENT_INFO:
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

/* lint -restore */

/* The following typedef is used to convert back and forth between
 * CM and ICM band preferences.  Usage of the table works on the
 * assumption that both are constructed from bit masks, which
 * is true at the point of construction of these tables.
 */
typedef struct
{
  cm_band_pref_e_type   cm_band_pref;
  AEECMBandPref         icm_band_pref;
} OEMCM_BandPrefConvType;

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

  { CM_BAND_PREF_CELL_A, AEECM_BAND_PREF_CELL_A },
  { CM_BAND_PREF_CELL_B, AEECM_BAND_PREF_CELL_B },
  { CM_BAND_PREF_CELL, AEECM_BAND_PREF_CELL },
  { CM_BAND_PREF_PCS, AEECM_BAND_PREF_PCS },
};

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

    default:
      return CM_SRV_DOMAIN_PREF_NO_CHANGE;
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

    default:
      return CM_ROAM_PREF_NO_CHANGE;
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

    default:
      return CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
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

    default:
      return CM_PREF_TERM_NONE;
  }
}




