#ifndef OEMCM_H
#define OEMCM_H
/*=============================================================================

FILE: OEMCM.h

SERVICES: OEM Functions for Call Manager Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2004-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Inc/OEMCM.h#49 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
11/08/07     alb     Remove CM_API_MBMS ifdefs.
11/02/07     alb     Added MBMS API support.
08/06/07     alb     API for editing PRL records added.
08/10/07     cvs     Add number of call ID's to ICM
03/20/07     jas/ak  Adding support for persistent preference values.
12/04/06     ypmw    Fix wlan compiler error
09/19/06     jas     Adding band value to AEECMWLANPref structure.
07/07/06     ak      Merged ACM changes originally made by ACM feature team
06/12/06     jks     Ported IUsrSysClock to new BREW ISYSCLOCK interface
03/27/06     jas     Adding featurization to remove a build warning.
01/20/06     RI      Added handling of Call Hold/Retrieve events from CM.
01/19/06     cvs     support for emergency call categories
11/15/05     bpw     Fixed internal call id max and suppress get info error callback.
07/20/05     AT      Added new API to set the subscription_status as available
                     not available for all technologies in one CM commands.
07/15/05     AT      Added API to Reset Origination Throttling.
06/07/05     sun     Added support for CCBS
06/03/05     sun     Added support for incoming voice call during 
                     Streaming Video
06/02/05     RI      Modified the critical section definition.
06/02/05     jas     Added support for WLAN pref info in AEECMPhInfo
05/16/05     sun     Fixed case for include files
05/03/05     CH      Change to remove BREW's Bridle.
04/06/05     RI      Support for TTY.
03/15/05     ak      Change in Signature since an arg is output
03/10/05     ak      Added CPHS EONS Support
03/10/05     jas     Updating profile of OEMCM_fill_wlan
02/18/05     sun     Added Handler for CM_CALL_EVENT_LINE_CTRL
01/12/05     AT      Removed the simple call ID macros in favor of functions.
12/15/04     JAS     Updated featurization of include file
12/03/04     AT/JAS  Added support for WLAN
09/23/04     SUN     Merged Changed for Brew 3.1
09/14/04     SUN     Do not change call state on notifications
09/03/04     SUN     Added Support for ALS
08/04/04     PA      Release ICMNotifier object on BREW exit.
07/27/04     PA      Include OEMNV.h
07/08/04     PA      Added utility functions.
06/28/04     PA      Make AEESysObject first field and featurize for brew 3.0
06/25/04     PA      Added call cmd to AEECMCallErrCBPkt.
06/18/04     PA      Link ICM as a system object
05/25/04     PA      Added OEMCM_IsValidOprtModeSwitch().
05/03/04     PA      Include cmutil.h for cm_util_def_alphabet_to_ascii()
04/30/04     PA      Lint cleanup
04/29/04     SUN     Added new function to Convert from Default to Ascii
04/19/04     PA      Initial Revision
=============================================================================*/

#include "OEMFeatures.h"
#include "BREWVersion.h"


#if MIN_BREW_VERSION(3,1)

#ifndef AEE_STATIC
#define AEE_STATIC
#endif

#include "AEECM.h"
#include "OEMTelephone.h"
#include "AEETelephone.h"
#include "AEECriticalSection.h"
#include "OEMConfig.h"
#include "AEEModel.h"

#if defined(FEATURE_ACM)
#include "OEMACM.h"
#if defined (FEATURE_ODM_UI) 
#include "OEMAppResCtl.h"
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM

#ifdef AEE_SIMULATOR
#include "cm.h"
#define CAI_REL_NORMAL 0x00  /* Value of ORDQ to indicate a normal release */
#endif // AEE_SIMULATOR

#include "AEECallHistory.h"
#else


#include "AEECM.h"
#include "AppComFunc.h"

#endif // MIN_BREW_VERSION(3,1)

#include "AEECallHistoryExt.h"

#include "err.h"
#include "AEE_OEM.h"
#include "OEMOS.h"
#include "AEEStdLib.h"
#include "OEMHeap.h"

#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#include "OEMCriticalSection.h"
#endif

#include "OEMObjectMgr.h"
#include "OEMNV.h"

#include "cm.h"
#include "cmutil.h"
#include "sys.h"
#include "sys_gw.h"
#include "sd.h"
#include "tmc.h"
#ifndef USES_MMI
#include "uiupbm.h"
#endif
#include "prl.h"

#include "SysModelBase_priv.h"

#if defined(FEATURE_WLAN)
#error code not present
#endif

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#include "cai.h"
#endif

#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
#endif

#ifdef AEE_SIMULATOR

#ifdef MSG_ERROR
   #undef MSG_ERROR
#endif
#ifdef MSG_HIGH
   #undef MSG_HIGH
#endif
#ifdef MSG_MED
   #undef MSG_MED
#endif
#ifdef MSG_LOW
   #undef MSG_LOW
#endif

#define FARF_OEMCM_ERROR   1
#define FARF_OEMCM_HIGH    1
#define FARF_OEMCM_MED     0
#define FARF_OEMCM_LOW     0

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p


#define MSG_ERROR( sf, a, b, c )    FARF(OEMCM_ERROR, (sf, a, b, c))
#define MSG_HIGH( sf, a, b, c )     FARF(OEMCM_HIGH, (sf, a, b, c))
#define MSG_MED( sf, a, b, c )      FARF(OEMCM_MED, (sf, a, b, c))
#define MSG_LOW( sf, a, b, c )      FARF(OEMCM_LOW, (sf, a, b, c))
#endif /* AEE_SIMULATOR */

#ifdef FEATURE_DS_IS2000
#include "rxc.h"
#include "txc.h"
#include "dsrlp.h"
#endif /* FEATURE_DS_IS2000 */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#include "OEMCFGI.h"
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


/* Max number of simultaneous calls */
#define OEMCM_MAX_NUM_CALLS   (CM_CALL_ID_MAX+1) // CM starts at 0 and ICM starts at 1

/* check number of call IDs */
#if OEMCM_MAX_NUM_CALLS > AEECM_MAX_NUM_CALLS
  #error "Number of CM call IDs exceeds the number of ICM call IDs"
#endif

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif

#define OEMCM_CDMA_END_VALUE  0x0300

#define AEECM_INTERNAL_CALL_GROUP 0x05
#define AEECM_INTERNAL_PH_GROUP   0x06

/* limit continuous DTMF time reporting to two seconds */
#define AEECM_MAX_DTMF_RPT_TIME 2000

#ifdef FEATURE_BREW_3_0

#if __ARMCC_VERSION >= 120000 // ADS 1.2
#pragma arm section zidata = "bridle_critical_section_zi"
#endif // __ARMCC_VERSION >= 120000 // ADS 1.2
static AEECriticalSection gcmCriticalSection;
#if __ARMCC_VERSION >= 120000 // ADS 1.2
#pragma arm section zidata
#endif // __ARMCC_VERSION >= 120000 // ADS 1.2

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

#ifdef FEATURE_DS_IS2000
#define MAX_NO_SCH_RATES    6
#define OEMCM_DS_DATA_RATE_9_6    96     /* 9.6 */
#define OEMCM_DS_DATA_RATE_19_2   192    /* 19.2 */
#define OEMCM_DS_DATA_RATE_28_8   288    /* 28.8 */
#define OEMCM_DS_DATA_RATE_48     480    /* 48.0 */
#define OEMCM_DS_DATA_RATE_86_4   864    /* 86.4 */
#define OEMCM_DS_DATA_RATE_163_2  1632   /* 163.2 */
#define OEMCM_DS_DATA_RATE_316_8  3168   /* 316.8 */


#define OEMCM_DS_DATA_RATE_14_4    144     /* 14.4 */
#define OEMCM_DS_DATA_RATE_43_2    432     /* 43.2 */
#define OEMCM_DS_DATA_RATE_72      720     /* 72.0 */
#define OEMCM_DS_DATA_RATE_129_6   1296    /* 129.6 */
#define OEMCM_DS_DATA_RATE_244_8   2448    /* 244.8 */
#define OEMCM_DS_DATA_RATE_475_2   4752    /* 475.2 */

/* For Mixed Rates of FCH in RS1 and SCH in RS2
   Caculations below show FCH + (rate) * SCH
*/
#define OEMCM_DS_DATA_RATE_24_0    240    /* 9.6 + 1 * 14.4  */
#define OEMCM_DS_DATA_RATE_38_4    384    /* 9.6 + 2 * 14.4  */
#define OEMCM_DS_DATA_RATE_67_2    672    /* 9.6 + 4 * 14.4  */
#define OEMCM_DS_DATA_RATE_124_8   1248   /* 9.6 + 8 * 14.4  */
#define OEMCM_DS_DATA_RATE_240_0   2400   /* 9.6 + 16 * 14.4  */
#define OEMCM_DS_DATA_RATE_470_4   4704   /* 9.6 + 32 * 14.4  */


/* For Mixed Rates of FCH in RS2 and SCH in RS1
   Caculations below show FCH + (rate) * SCH
*/
#define OEMCM_DS_DATA_RATE_33_6    336    /* 14.4 +  2 * 9.6 */
#define OEMCM_DS_DATA_RATE_52_8    528    /* 14.4 +  4 * 9.6 */
#define OEMCM_DS_DATA_RATE_91_2    912    /* 14.4 +  8 * 9.6 */
#define OEMCM_DS_DATA_RATE_168_0   1680   /* 14.4 + 16 * 9.6 */
#define OEMCM_DS_DATA_RATE_321_6   3216   /* 14.4 + 32 * 9.6 */
#endif /* FEATURE_DS_IS2000 */

#ifdef FEATURE_HDR
#error code not present
#endif

/* ACM AppResCtl states */
#if defined (FEATURE_ACM)
#define OEMCM_ACM_FREE        0x00
#define OEMCM_ACM_ACQUIRING   0x01
#define OEMCM_ACM_ACQUIRED    0x02
#endif // FEATURE_ACM

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

                    TYPE DEFINITIONS

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
  AEECMNetworkType      list_type;
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
  AEECMCallID           callID;
  char *                dtmfs;
  char *                curr;
  AEECMDTMFOnLength     on_length;
  AEECMDTMFOffLength    off_length;
  boolean               start;
} AEECMBurstDTMFPkt;

#if defined (FEATURE_ACM)
/* Passed to ACM callback for Mobile originated calls */
typedef struct OEMCM_ACM_MobileOriginatedCallInfo
{
  cm_call_id_type   call_id;
  AEECMNotifyInfo * notify_info;
} OEMCM_ACM_MobileOriginatedCallInfo;

/* Passed to ACM callback for Network originated calls */
typedef struct OEMCM_ACM_NetworkOriginatedCallInfo
{
  void                   *data_block_ptr;
  cm_client_id_type      client_id;
  cm_call_id_type        call_id;
  cm_ans_params_s_type   answer_params;
} OEMCM_ACM_NetworkOriginatedCallInfo;

typedef struct ICM_ACMInfo
{
#if defined (FEATURE_ODM_UI)
  IAppResCtl *            m_pAppResCtl;
#endif // FEATURE_ODM_UI
  uint32                  m_uAcmRes_State;
  void *                  m_pCBInfo;
  ACM_Callback_data_type  m_acm_callback_data_type;
} ICM_ACMInfo;
#endif  // FEATURE_ACM

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

#ifdef FEATURE_DS_END_KEY_DROPS_CALL
  uint8                 m_p_rev_in_use;
#endif

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

  cm_call_id_type       m_internal_call_ids[OEMCM_MAX_NUM_CALLS];
  boolean               m_bMediaPause;
  cm_call_id_type       m_IncomCallID;

#if defined (FEATURE_ACM)
  ICM_ACMInfo *         m_pAcmInfo;
#endif // FEATURE_ACM

} ICMCore;

/* ICM object, instantiated every time */
struct ICM {
  union {
    AEEVTBL(ICM) *        pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject          m_so;
#endif
  } u;
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
   AEECallback              m_cbExit;

   ICMCore *                m_coreObj;   /* pointer to core object */

   uint32                   m_dwNotify;  /* Notification bits */
};

/* ICMModel object, instantiated once */
struct ICMModel {
   SysModelBase    base;
   AEEVTBL(IModel) vtModel;

   AEECallback                  *m_pcbExit;      /* Callback called on AEE_EXIT*/

   ICMCore *                m_coreObj;   /* pointer to core object */
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
  cm_call_cmd_e_type      call_cmd;
  AEECMCallError          call_error;
} AEECMCallErrCBPkt;

typedef void (*PFNOEMCMCALLNOTIFY)(cm_mm_call_info_s_type *, AEECMEvent);
typedef void (*PFNOEMCMPHNOTIFY)(cm_ph_info_s_type *, AEECMEvent);

/* The following structure enables easy mappings between CM band
 * mask values and ICM band pref values.
 */
typedef struct
{
  sys_band_mask_type CMBandMask;
  AEECMBandPref      ICMBandPref;
} OEMCMBandPrefMapTableType;

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


/*===========================================================================

               INTERFACE   FUNCTION   DECLARATIONS

===========================================================================*/

/* Interface to the CM Notifier object. */
static uint32 OEMCMNotifier_AddRef(ICMNotifier *po);
static uint32 OEMCMNotifier_Release(ICMNotifier *po);
#if MIN_BREW_BUILD(3,1,4,7)
static void OEMCMNotifier_SetMask(ICMNotifier *po, uint32 *pdwMask);
#else
static void OEMCMNotifier_SetMask(ICMNotifier *po, const uint32 *pdwMask);
#endif // MIN_BREW_BUILD(3,1,4,7)

/* Interface to the CM object. */
static uint32 OEMCM_AddRef(ICM *po);
static uint32 OEMCM_Release(ICM *po);
static int OEMCM_QueryInterface(ICM *po, AEECLSID clsid, void **ppNew);
static int OEMCM_OriginateVoiceCall(ICM *po, AECHAR const *number,
                                    AEECMCallID *call_id);
static int OEMCM_OriginateCall(ICM *po, AEECMCallType call_type,
                             AECHAR const *number, AECHAR const *alpha,
                             ICallOpts * options, AEECMCallID  *call_id);
static int OEMCM_AnswerCall(ICM *po, AEECMCallID callID);
static int OEMCM_EndCall(ICM *po, AEECMCallID callID);
static int OEMCM_EndAllCalls(ICM *po);
static uint16 OEMCM_GetActiveCallIDs(ICM *pMe, AEECMCallType call_type,
                             AEECMCallState call_state,
                             AEECMCallID *call_ids, uint32 size);
static int OEMCM_GetCallInfo(ICM *po, AEECMCallID callID,
                             AEECMCallInfo *call_info, uint32 size);
static int OEMCM_SetPrivacyPref(ICM *po, AEECMPrivacyPref privacy_pref);
static AEECMIsCallOps OEMCM_CallOpsStringCheck(ICM *po,
                             AECHAR * dialed_string,
                             uint8 *call_num);
static int OEMCM_CallOpsRequest(ICM *po,
                         AECHAR * dialed_string,
                         AEECMCallID callID);

static int OEMCM_SendFlash_Int( const char *called, AEECMCallID flash_call_id );
static int OEMCM_AnswerCall_Int( ICM *pMe, cm_call_id_type call_id, cm_call_type_e_type call_type );
static int OEMCM_EndCall_Int( ICM *pMe, cm_call_id_type call_id );
static int OEMCM_BurstDTMF_Int(ICM *pMe, AEECMCallID call_id,
                               AEECMDTMFOnLength on_length,
                               AEECMDTMFOffLength off_length, const char *digits);
static int OEMCM_StartContDTMF_Int(ICM *pMe, AEECMCallID call_id, char digit);


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
                             AEECMWLANPref * wlan_ptr,
                             AEECMPhError *status,
                             AEECallback *cb);
static int OEMCM_SetNAMSel(ICM *po, AEECMNAM nam_sel);
static int OEMCM_SetOperatingMode(ICM *po, AEECMOprtMode oprt_mode);
static int OEMCM_GetPhoneInfo(ICM *po, AEECMPhInfo *info, uint32 size);
static int OEMCM_SetSubscriptionStatus(ICM *po, AEECMSysMode mode,
                                    boolean status);
static int OEMCM_SetSubscriptionStatusEx(ICM *pMe, AEECMSUB cdma_status,
                                     AEECMSUB gw_status, AEECMSUB wlan_status);
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
static int OEMCM_BurstDTMF(ICM *po, AEECMCallID callID,
                           AEECMDTMFOnLength on_length,
                           AEECMDTMFOffLength off_length, AECHAR *digits);
static int OEMCM_StartContDTMF(ICM *po, AEECMCallID callID, AECHAR digit);
static int OEMCM_StopContDTMF(ICM *po, AEECMCallID callID);

  /* MBMS */
static int OEMCM_SendMBMSCommand(ICM *pMe, AEECMMBMSCmd *pMbmsCmd);

/* Helper functions */
static boolean OEMCM_IsValidModeBandPref(ICM *pMe, AEECMModePref mode_pref,
                                            AEECMBandPref band_pref);
static AEECMSysMode OEMCM_GetSysMode(ICM *po);
static boolean OEMCM_IsMyCall(ICM *po, AEECMCallID callID);
static int OEMCM_SelectLine(ICM *po, AEECMAlsLine selectLine,AEECMPhError *status,AEECallback *cb);
static int OEMCM_SetLineSwitching(ICM *po, AEECMAlsLineSwitching lineSwitching,AEECMPhError *status,AEECallback *cb);
static int OEMCM_RecallRsp(ICM *pMe, AEECMRecallRspType recallRsp, AEECMCallID callID);
static int OEMCM_GetPLMNFromIMSI(ICM *po, AEECMPLMNID *pPLMN, byte *imsi);
static boolean OEMCM_SysMatchPLMN(ICM *po, AEECMPLMNID PLMNa, AEECMPLMNID PLMNb);

/* Interface to the CallOpts Object */
static uint32 OEMCallOpts_AddRef(ICallOpts *pMe);
static uint32 OEMCallOpts_Release(ICallOpts *pMe);
static int OEMCallOpts_QueryInterface(ICallOpts *pMe, AEECLSID id, void **ppo);
static int OEMCallOpts_AddOpt(ICallOpts *pMe, CallOpt *pOpt);
static int OEMCallOpts_RemoveOpt(ICallOpts *pMe, int32 nOptId);
static int OEMCallOpts_GetOpt(ICallOpts *pMe, int32 nOptId, CallOpt *pOpt);
#ifdef FEATURE_WLAN
#error code not present
#endif

/* CDMA based supplementary services */
static int OEMCM_SendFlashSupps(ICM *pMe,
                                AEECMSendFlashSuppsType suppsType,
                                AEECMCallID callId);
/* PRL Related */
static int OEMCM_PrlExtractAcqRecord(ICM *pMe,AEECMPrlAcqRecord *pAcqRec, int index);
static int OEMCM_PrlUpdateAcqRecord(ICM *pMe,const AEECMPrlAcqRecord *pAcqRec, int index);
static int OEMCM_PrlCommit(ICM *pMe);

/*===========================================================================

               INTERNAL FUNCTION DECLARATIONS

===========================================================================*/
static ICMCore * OEMCMCore_New(void);
static uint32 OEMCMCore_AddRef(void);
static uint32 OEMCMCore_Release(void);

static void OEMCMModel_Dtor(SysModelBase *pMe);

static void OEMCMNotifier_FreeOnAEEExit(void *pUser);
static void OEMCMModel_AEEExitNotify(void* pUser);
static void OEMCMCore_FreeOnAEEExit(void);

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
#ifdef FEATURE_MBMS
static void OEMCM_BREWMBMSEventCB(void *pData);
#endif
static void OEMCM_CancelBurstDTMF(ICM * pMe);
static int OEMCM_AllocBurstDTMF(ICM *pMe, AEECMCallID callID,
                                const char *digits,
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
static int OEMCM_CMCallCmdOrig(
                             ICM                         *pMe,
                             cm_call_type_e_type          call_type,
                             cm_srv_type_e_type           srv_type,
                             const cm_num_s_type         *calling_number,
                             const cm_num_s_type         *called_number,
                             const cm_orig_alpha_s_type  *alpha_buf,
                             cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
                             cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
                             cm_call_id_type             *call_id_ptr);
static void OEMCM_RefreshDataCallStats(ICMCore *pMe, AEECMCallID callID);
#ifdef FEATURE_DS
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
static void OEMCM_RefreshCDMADataCallStats(AEECMDataCallStats *data_stats,
                                         AEECMCallID callID);
#endif
#endif
static void OEMCM_HandleDataCallEnd(AEECMDataCallStats *data_stats,
                                    cm_mm_call_info_s_type *call_info);
#if defined(GSM) || defined(WCDMA)
static uint32 OEMCM_DecodeMaxBitRate(uint8 max_bit_rate);
#endif
static void OEMCM_HandleCommandError(cm_call_id_type call_id,
                        cm_call_cmd_e_type call_cmd,
                        cm_call_cmd_err_e_type call_cmd_err);
static boolean OEMCM_IsEmergencyNumber(const AECHAR *number, uint8 *ecc_category);
static int OEMCM_SendEmergencyFlash(ICM *pMe, const AECHAR *num,
                                    const AECHAR *alpha, AEECMCallID *pcallID, 
                                    uint8 ecc_category);
static void OEMCM_HandleOriginateCall(ICM *pMe, cm_call_type_e_type call_type,
                                     const cm_num_s_type *called_number,
                                     const cm_orig_alpha_s_type *alpha,
                                     cm_call_id_type call_id,
                                     boolean  isTTY);
static int OEMCM_SendFlash(AECHAR const *number, AEECMCallID flash_call_id);
static uint32  OEMCM_MergeMasks(const uint32 * pdwMasks);
static void * OEMCM_Malloc(uint32 size, int line);
static void OEMCM_Free(void *);
static AEECMSysMode OEMCM_GetSystemMode(void);
static AEECMCallType OEMCM_GetCallType(cm_call_id_type call_id);
static void OEMCM_FillCallNum(cm_num_s_type*        called_number,
                              cm_num_s_type*        calling_number,
                              cm_orig_alpha_s_type *cm_alpha,
                              const AECHAR         *wszNumber,
                              const AECHAR         *wszCalling,
                              const AECHAR         *wszAlpha);
static void OEMCM_InitNum(cm_num_s_type *num, const char *number);
static void OEMCM_CopyInCallInfo(AEECMCallInfo *dest,
                                 const cm_mm_call_info_s_type *src);
static void OEMCM_CopyInPhInfo(AEECMPhInfo *dest,
                               const cm_ph_info_s_type *src);
static void OEMCM_CopyInSSInfo(AEECMSSInfo *dest,
                               const cm_mm_ss_info_s_type *src);
#ifndef FEATURE_USRSYSCLOCK
static void OEMCM_CopyInMMInfo(AEECMMMInfo *mmInfo,
                               const sys_mm_information_s_type *mm_info);
#endif /* FEATURE_USRSYSCLOCK */
static void OEMCM_ConvertDefaultToAscii(uint8 *dest,
                                   const uint8 *src, int length);
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
#ifdef FEATURE_WLAN
#error code not present
#endif
static void OEMCM_ResetPrivateCallState(cm_call_id_type call_id);
static boolean OEMCM_IsValidAppCallID(AEECMCallID callID);
static boolean OEMCM_IsValidCallID(cm_call_id_type call_id);
static AEECMPhInfo * OEMCM_GetValidPhInfo(ICM *pMe);
static boolean OEMCM_IsValidOprtModeSwitch( AEECMOprtMode    fromOprtMode,
                                            AEECMOprtMode    toOprtMode);
static AEECMCallState OEMCM_GetCallState(cm_call_id_type call_id);
static void OEMCM_CopyNumToOtherParty(const cm_num_s_type *num,
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

static AEECMPrefTerm OEMCM_PrefTermToAEECM(cm_pref_term_e_type pref);
static cm_pref_term_e_type OEMCM_PrefTermToDMSS(AEECMPrefTerm pref);

static AEECMAcqOrderPref OEMCM_AcqOrderPrefToAEECM(cm_gw_acq_order_pref_e_type pref);
static cm_gw_acq_order_pref_e_type OEMCM_AcqOrderPrefToDMSS(AEECMAcqOrderPref pref);

static AEECMBandPref OEMCM_BandPrefToAEECM(cm_band_pref_e_type pref);
static cm_band_pref_e_type OEMCM_BandPrefToDMSS(AEECMBandPref pref);

static AEECMRoamPref OEMCM_RoamPrefToAEECM(cm_roam_pref_e_type pref);
static cm_roam_pref_e_type OEMCM_RoamPrefToDMSS(AEECMRoamPref pref);

static AEECMServDomPref OEMCM_SrvDomainPrefToAEECM(cm_srv_domain_pref_e_type pref);
static cm_srv_domain_pref_e_type OEMCM_SrvDomainPrefToDMSS(AEECMServDomPref pref);

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
#ifdef FEATURE_MBMS
static AEECMEvent OEMCM_MBMSEventToAEECM(cm_mbms_event_e_type event);
#endif
static uint32 OEMCM_SSMaskToAEECM(uint32 cm_mask);
static AEECMCallError OEMCM_CallErrorToAEECM(cm_call_cmd_err_e_type error);
static AEECMPhError OEMCM_PhErrorToAEECM(cm_ph_cmd_err_e_type error);


static boolean OEMCM_CheckDTMFOnLength(AEECMDTMFOnLength length);
static boolean OEMCM_CheckDTMFOffLength(AEECMDTMFOffLength off_length);
static boolean OEMCM_CheckDTMFDigits( uint8 cnt, uint8 *digit_buf);

static boolean OEMCM_CheckValidOprtMode(AEECMPhInfo * ph_ptr);

#ifndef FEATURE_UIONE_HDK
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
static void OEMCM_HandleCallEmergencyFlashed(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallCopyNum(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallAlphaUpdate(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallExtDisplay(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallQOS(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallProgress(cm_mm_call_info_s_type *call_info,
                                            AEECMEvent event);
static void OEMCM_HandleCallDormant(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallSignal(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallSrvOpt(cm_mm_call_info_s_type *call_info, AEECMEvent event);

static void OEMCM_HandleCallSetupInd(cm_mm_call_info_s_type *call_info, AEECMEvent);
static void OEMCM_HandleCallMngCallsConf(cm_mm_call_info_s_type *, AEECMEvent);
static void OEMCM_HandleCallRecallConf(cm_mm_call_info_s_type *, AEECMEvent);
static void OEMCM_HandleCallLineCtrl(cm_mm_call_info_s_type *call_info,
                                            AEECMEvent event);
static void OEMCM_HandlePhDefault(cm_ph_info_s_type *ph_info, AEECMEvent event);
static void OEMCM_HandlePhTestControl(cm_ph_info_s_type *ph_info, AEECMEvent event);
static void OEMCM_HandlePhInternalDefault(cm_ph_info_s_type *ph_info, AEECMEvent event);

static void OEMCM_HandleCallRecallRsp(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static int  OEMCM_ResetOrigThrottle(ICM *pMe);
static void OEMCM_HandleCallHold(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallRetrieved(cm_mm_call_info_s_type *call_info, AEECMEvent event);
#endif /* OEMCM_H */

