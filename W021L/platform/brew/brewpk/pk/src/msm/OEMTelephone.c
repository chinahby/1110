/*=============================================================================

FILE: OEMTelephone.c

SERVICES: OEM Functions for Telephone Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright (c) 2003-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            Qualcomm Confidential and Proprietary
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
/* Lint global dont cares */
/*lint -esym(754, ICM::pvt) */
/*lint -esym(754, ICallOpts::pvt) */
/*lint -save -e740 */
/*lint -save -e611 */

#include "OEMFeatures.h"
#ifndef AEE_STATIC
#define AEE_STATIC
#endif
#include "err.h"
#include "AEE_OEM.h"
#include "AEE_OEMHeap.h"
#include "OEMTelephone.h"
#include "AEETelephone.h"
#include "OEMOS.h"
#include "AEEStdLib.h"
#include "OEMHeap.h"
#include "AEE_OEMDispatch.h"
#include "AEECriticalSection.h"
#include "OEMObjectMgr.h"
#include "OEMConfig.h"
#include "AEECM.h"
#include "OEMCall.h"
#include "OEMTSupps.h"

#include "AEEISignal.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#include "AEEConfig.h"
#include "OEMCFGI.h"
#endif

#if defined(FEATURE_ACM)
#include "OEMACM.h"
#if defined (FEATURE_ODM_UI)
#include "OEMAppResCtl.h"
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM
#ifdef PLATFORM_LTK
# define cm_client_sups_reg _cm_client_sups_reg
# define cm_client_ph_reg _cm_client_ph_reg
# define cm_mm_client_call_reg _cm_mm_client_call_reg
# define cm_client_sups_reg _cm_client_sups_reg
#endif

#ifdef AEE_SIMULATOR
#include "cm.h"
#define NV_MAX_NAMS 1
#define CAI_REL_NORMAL 0x00
  /* Value of ORDQ to indicate a normal release */
#else

#define CM_MAJ_VER 3
#define CM_MIN_VER 50
#define MIN_CM_REV(maj,min) (CM_MAJ_VER > maj || (CM_MAJ_VER == maj && CM_MIN_VER >= min))
# include "cm.h"

#include "sd.h" 
#include "tmc.h"
#endif

#include "sys.h"

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#include "cai.h"
#endif

#if defined (FEATURE_ACM)
#include "msg.h"
#include "err.h"
#include "OEMACM.h"
#if defined (FEATURE_ODM_UI)
#include "OEMAppResCtl.h"
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM

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

static struct {
   flg bErrorDebugMsg:1;
   flg bHighDebugMsg:1;
   flg bMedDebugMsg:1;
   flg bLowDebugMsg:1;
} gOEMCMDebug;

boolean OEMCMError_IsDebugMsgOn(void);
#define FARF_OEMCM_ERROR   OEMCMError_IsDebugMsgOn()

boolean OEMCMHigh_IsDebugMsgOn(void);
#define FARF_OEMCM_HIGH    OEMCMHigh_IsDebugMsgOn()

boolean OEMCMMed_IsDebugMsgOn(void);
#define FARF_OEMCM_MED     OEMCMMed_IsDebugMsgOn()

boolean OEMCMLow_IsDebugMsgOn(void);
#define FARF_OEMCM_LOW     OEMCMLow_IsDebugMsgOn()

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 


#define MSG_ERROR( sf, a, b, c )    FARF(OEMCM_ERROR, (sf, a, b, c))
#define MSG_HIGH( sf, a, b, c )     FARF(OEMCM_ERROR, ("*****"sf, a, b, c))
#define MSG_MED( sf, a, b, c )      FARF(OEMCM_MED, (sf, a, b, c))
#define MSG_LOW( sf, a, b, c )      FARF(OEMCM_LOW, (sf, a, b, c))


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
/* bearer capability definitions */
#ifndef FEATURE_GSM_HALF_RATE
#define FULL_RATE_ONLY                       1
#else
#define DUAL_RATE_FULL_RATE_PREFERRED        3
#endif

/* Information Transfer Capability definitions - see GSM 04.08 table 10.72 */

#define ITC_SPEECH                           0

/*coding standard*/
#define GSM                                  0

/*transfer mode*/
#define CIRCUIT                              0
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


#define AEET_IS_MODE_GW(mode) \
        ( ((mode) == AEET_SYS_MODE_GSM ||                \
          (mode) == AEET_SYS_MODE_WCDMA ||               \
          (mode) == AEET_SYS_MODE_GW)                    \
          ? TRUE : FALSE )

#define AEET_IS_MODE_CDMA(mode) \
        ( ((mode) == AEET_SYS_MODE_CDMA ||                 \
          (mode) == AEET_SYS_MODE_HDR)                 \
          ? TRUE : FALSE )
/*
#define IS_E911_CALLBACK_MODE( ph_ptr ) \
                     ( (ph_ptr->mode_pref == CM_MODE_PREF_EMERGENCY) && \
                       (ph_ptr->pref_term == CM_PREF_TERM_PWR_CYCLE) )
*/

#define OEMCM_MAX_NUM_CALLS   CM_CALL_ID_MAX  /* Max number of simultaneous calls */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#define OEMCM_GSM_END_VALUE   0x0100
#define OEMCM_GSM_REJ_VALUE   0x0200
#endif

#define OEMCM_CDMA_END_VALUE  0x0300

#define AEECM_INTERNAL_CALL_GROUP 0x05
#define AEECM_INTERNAL_PH_GROUP   0x06
#ifdef FEATURE_UUS
#define AEECM_USER_GROUP          0x09
#endif

/* limit continuous DTMF time reporting to two seconds */
#define AEECM_MAX_DTMF_RPT_TIME 2000

static AEECriticalSection gcmCriticalSection;

#define AEECM_MUTEX_LOCK()   AEECriticalSection_Enter(&gcmCriticalSection)
#define AEECM_MUTEX_UNLOCK() AEECriticalSection_Leave(&gcmCriticalSection)

/* suppress warning for unused parameter. */
#undef PARAM_NOT_REF
#define PARAM_NOT_REF(x) /*lint -esym(715,x) */
#define IN_RANGE(val, min, max) ( (val) >= (min) && (val) <= (max) )

#define OEMCM_NUM_CALLHISTORY_FIELDS 5

/* ACM AppResCtl states */
#if defined (FEATURE_ACM)
#define ACM_FREE        0x00
#define ACM_ACQUIRING   0x01
#define ACM_ACQUIRED    0x02
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
       
#define AEETSSINFO_VERSION       1

/* SCUDIF bearer Capability indicator matches with
   TS24.008, 10.5.4.22, Repeat Indicator: */
#define OEMCM_REPEAT_SERVICE_CHANGE_FALLBACK   0x04

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


typedef enum {
   CB_TYPE_NONE,
   CB_TYPE_ERRPKT,
   CB_TYPE_CALL_INFO,
   CB_TYPE_PH_INFO,
   CB_TYPE_SS_INFO,
   CB_TYPE_INBAND_INFO
}  AEECMEventCBType;

/* Generic Event callback structure.
 * Used for execution in BREW system context as a result of a CM event 
 * callback to send a notification.
 */
typedef struct AEECMEventCBS {
  AEECallback            cb;         
  AEECMEvent             event;
  void *                 data;
  uint8                  nRef;
  AEECMEventCBType            cbType;
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

/* For use by RefreshNetworks */
typedef struct {
  AEECallback *         pcb;
  int*          pnStatus;
  OEMTelephone* me;
} OEMTRefreshNetworksReq;

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
  char *                dtmfs;
  char *                curr;
  uint8                 on_length;
  uint8                 off_length;
  boolean               start;
} AEECMBurstDTMFPkt;

#if defined (FEATURE_ACM)
/* Passed to ACM callback for Mobile originated calls */
typedef struct ACM_MobileOriginatedCallInfo
{
  cm_call_id_type   call_id;
  AEETNotifyInfo * notify_info;
} ACM_MobileOriginatedCallInfo;

/* Passed to ACM callback for Network originated calls */
typedef struct ACM_NetworkOriginatedCallInfo
{
  void                   *data_block_ptr;
  cm_client_id_type      client_id;
  cm_call_id_type        call_id;
  cm_ans_params_s_type   answer_params;
} ACM_NetworkOriginatedCallInfo;

typedef struct ICM_ACMInfo
{
#if defined (FEATURE_ODM_UI)
  IAppResCtl *            m_pAppResCtl;
#endif // FEATURE_ODM_UI
  uint32                  m_uAcmRes_State;
  void *                  m_pCBInfo;
  ACM_Callback_data_type  m_acm_callback_data_type;
} ICM_ACMInfo;

typedef ICM_ACMInfo ACMInfo;
#endif  // FEATURE_ACM

/* Core Object, instantiated once */
typedef struct ICMCore {
  uint32                m_uRefs;

  AEETCallInfo          m_call_info[OEMCM_MAX_NUM_CALLS]; 
  AEECMPrivateCallState m_state[OEMCM_MAX_NUM_CALLS];

  uint32                m_dtmf_start_times[OEMCM_MAX_NUM_CALLS]; /* in ms */
  AEECMBurstDTMFPkt *   m_bdtmf[OEMCM_MAX_NUM_CALLS];

  cm_call_mode_info_e_type m_call_mode_type[OEMCM_MAX_NUM_CALLS];

  AEETPhInfo            m_ph_info;    
  AEETSSInfo            m_ss_info;   

  boolean               m_ph_valid;
  boolean               m_ss_valid;

  /* AEE callback and event data */
  AEECMEventCBS *       m_event_cbs;

  /* Default call parameters */
  cm_cdma_orig_params_s_type  def_cdma_orig_params;
  cm_gw_cs_orig_params_s_type def_gw_orig_params;

#ifdef FEATURE_ICM_DEBUG
  uint32                m_malloc_cnt;
#endif

  boolean               m_bDeepSleep;
  
  // ServingNetworks
  cm_mm_ss_info_s_type   ssInfo;
  ISignal               *piSignalSS;
  boolean                bInitSS;
  cm_ss_event_e_type     ssEvt;
  boolean                bSSInfoValid;

#if defined (FEATURE_ACM)
  ACMInfo *         m_pAcmInfo;
#endif // FEATURE_ACM

} ICMCore;

struct _OEMTelephone{
   uint32               dwRefs;
   ICM                  *pcm;
   ModelListener        *pListeners;
   AEECallback          *pRefreshNetworksReqList;
   OEMSupps             *pOEMSupps;
   AEECallback           m_OnExitCB;     
};

OEMTelephone *gpTelephone = NULL;   /* Single instance */
int OEMTelephone_EnableLineSwitch( OEMTelephone *pme, boolean bEnable );
int OEMTelephone_GetLineInfo( OEMTelephone *pme, AEETLineInfo *po, uint32 dwSize );

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

static cm_client_id_type   gCMID = 0;
static boolean             gbCMAct = FALSE;

typedef void (*PFNOEMCMCALLNOTIFY)(cm_mm_call_info_s_type *, AEECMEvent); 
typedef void (*PFNOEMCMPHNOTIFY)(cm_ph_info_s_type *, AEECMEvent);

/*===========================================================================

               INTERFACE   FUNCTION   DECLARATIONS   

===========================================================================*/

/* Interface to the CM object. */
static uint32 OEMCM_Release(ICM *po);
static int OEMCM_EndCall(ICM *po, AEECMCallID call_id);
static int OEMCM_EndAllCalls(ICM *po);
static uint16 OEMCM_GetActiveCallIDs(ICM *pMe, AEECMCallType call_type,
                             AEECMCallState call_state,
                             AEECMCallID *call_ids, uint32 size);
static int OEMCM_GetCallInfo(ICM *po, AEECMCallID call_id, 
                             AEETCallInfo *call_info, uint32 size);
static int OEMCM_SetPrivacyPref(ICM *po, AEECMPrivacyPref privacy_pref);
static int OEMCM_SendFlash_Int( const char *called, AEECMCallID flash_call_id );
static int OEMCM_OriginateCall_Int( ICM *pMe, 
                                    cm_call_type_e_type         call_type,
                                    cm_srv_type_e_type          srv_type,
                                    const cm_num_s_type         *calling_number,
                                    const cm_num_s_type         *called_number,
                                    const cm_orig_alpha_s_type  *alpha_buf,
                                    cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
                                    cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
                                    cm_call_id_type             *return_call_id_ptr );
static int OEMCM_AnswerCall_Int( ICM *pMe, cm_call_id_type call_id, cm_call_type_e_type call_type );
static int OEMCM_EndCall_Int( ICM *pMe, cm_call_id_type call_id );
static int OEMCM_BurstDTMF_Int(ICM *pMe, AEECMCallID call_id, 
                               AEECMDTMFOnLength on_length, 
                               AEECMDTMFOffLength off_length, const char *digits);
static int OEMCM_StartContDTMF_Int(ICM *pMe, AEECMCallID call_id, char digit);

  /* Phone Functions */
static int OEMCM_SetSystemPreference(ICM *po, AEETModePref mode_pref,
                             AEETPrefTerm pref_term, 
                             uint32 pref_duration,
                             AEETAcqOrderPref acq_order_pref,
                             AEETBandPref band_pref, 
                             AEETRoamPref roam_pref,
                             AEETHybrPref hybr_pref,
                             AEETServDomPref srv_domain_pref,
                             AEETNwSelModePref nw_sel_mode_pref,
                             AEETPLMNID * plmn_ptr, 
                             AEECMPhError *status,
                             AEECallback *cb);
static int OEMCM_SetOperatingMode(ICM *po, AEECMOprtMode oprt_mode);
static int OEMCM_SetRSSIDeltaThreshold(ICM *po, uint8 delta);
static int OEMCM_SetPrefNetworks(ICM *po,
                             AEECMUserPLMNList  *pref_networks_ptr,
                             AEECMPhError *status,
                             AEECallback *cb);
static int OEMCM_SetRTREConfig(ICM *po, AEECMRTREConfig rtre_config);
static int OEMCM_SetAnswerVoicePref(ICM *po, AEECMAnsVoicePref answer_voice,
                              uint32  duration);
static int OEMCM_AvoidServingSystem(ICM *po, AEECMAvoidSSPref avoid_type,
                              uint32 avoid_time);
static int OEMCM_ResetACM(ICM *pMe);
static int OEMCM_SetACMMax(ICM *pMe, uint32 acm_max);
static int OEMCM_SetDDTMPref(ICM *pMe, AEECMDDTMPref ddtm_pref);

  /* Inband functions */
static int OEMCM_StopContDTMF(ICM *po, AEECMCallID call_id);


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
static void OEMCM_DMSSInbandErrCB(void * data_block_ptr,
                              cm_inband_cmd_e_type inband_cmd,
                              cm_inband_cmd_err_e_type inband_cmd_err);

static void OEMCM_CancelBurstDTMF(ICM * pMe);
static int OEMCM_AllocBurstDTMF(ICM *pMe, AEECMCallID call_id,
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
static AEECMEventCBS * OEMCM_AllocEventCBS(ICMCore *pMe, AEECMEventCBType cbType);
static void OEMCM_FreeEventCBS(AEECMEventCBS *cb);

static void OEMCM_CancelAllErrCB(ICM *pMe);
static AEECMErrCBS *OEMCM_AllocErrCBS(ICM *pMe);
static void OEMCM_FreeErrCBS(ICM * pMe, AEECMErrCBS *pcb);

static boolean OEMCM_InitCallManager(void);
static void OEMCM_ReleaseCallManager(void);

static void OEMCM_AppCancelErrCB(AEECallback *pcb);

static void OEMCM_InitDefaultCallParams(ICMCore *);
static void OEMCM_HandleCommandError(cm_call_id_type call_id,
                        cm_call_cmd_e_type call_cmd,
                        cm_call_cmd_err_e_type call_cmd_err);
static void * OEMCM_Malloc(uint32 size, int line);
static void OEMCM_Free(void *);
static AEETSysMode OEMT_GetSystemMode(void);
static AEECMCallType OEMCM_GetCallType(cm_call_id_type call_id);
static int OEMCM_InitNum(cm_num_s_type *num, const char *number);
static void OEMCM_CopyInCallInfo(AEETCallInfo *dest, 
                                 const cm_mm_call_info_s_type *src);
static void OEMTelephone_CopyInPhInfo(AEETPhInfo *dest,
                               const cm_ph_info_s_type *src);
static void OEMTelephone_CopyInSSInfo(const cm_mm_ss_info_s_type *src);
static void OEMTelephone_CopyInMMInfo(AEETMMInfo *mmInfo, 
                                      const sys_mm_information_s_type *mm_info);
static void OEMTelephone_CopyInSysID(AEETSysID *dest, 
                                     const sys_sys_id_s_type *src);
static void OEMTelephone_CopyInFullNWName(AEETNWName *dest, 
                                   const sys_network_full_name_s_type *src);
static void OEMTelephone_CopyInShortNWName(AEETNWName *dest, 
                                   const sys_network_short_name_s_type *src);
static void OEMCM_CopyInAvailableNetworks(AEETDetailedPLMNList *dest, 
                                    const sys_detailed_plmn_list_s_type *src);
static void OEMCM_CopyInUserPrefNetworks(AEETUserPLMNList *dest,
                                   const sys_user_pref_plmn_list_s_type *src);
static void OEMCM_CopyOutUserPrefNetworks(sys_user_pref_plmn_list_s_type *dest,
                                          AEECMUserPLMNList *src);
static void OEMCM_ResetPrivateCallState(cm_call_id_type call_id);
static boolean OEMCM_IsValidAppCallID(AEECMCallID call_id);
static boolean OEMCM_IsValidCallID(cm_call_id_type call_id);
static AEETPhInfo * OEMCM_GetValidPhInfo(ICM *pMe);
static AEECMCallState OEMCM_GetCallState(cm_call_id_type call_id);
static void OEMCM_CopyNumToOtherParty(const cm_num_s_type *num, 
                                      AEETCallInfo *core_call_info);
static void OEMCM_CopyNumToAEECMNum(cm_num_s_type *num, AEETNumber *aee_num);
static void OEMCM_CopyCharLenToAECHAR(char *cmchar, uint8 len, 
                                      AECHAR *aeechar, uint16 size);

static void OEMCM_CopyInSignal(AEETSignal *aee_signal, 
                               cm_signal_s_type *cm_signal);
                               
static AEECMCallType OEMCM_CallTypeToAEECM(cm_call_type_e_type call_type);
static cm_call_type_e_type OEMCM_CallTypeToDMSS(AEECMCallType call_type);

static AEECMPrivacyPref OEMCM_PrivacyPrefToAEECM(cm_privacy_pref_e_type pref);
static cm_privacy_pref_e_type OEMCM_PrivacyPrefToDMSS(AEECMPrivacyPref pref);
static AEETHybrPref Prv_XlateHybrPref(cm_hybr_pref_e_type pref);
static cm_hybr_pref_e_type AEET_XlateHybrPref(AEETHybrPref pref);

static AEECMEvent OEMCM_CallEventToAEECM(cm_call_event_e_type evt);
static void OEMCM_ValidCallEvent(AEECMEvent *event, 
                                const cm_mm_call_info_s_type *call_info_ptr);

static AEECMEvent OEMCM_PhEventToAEECM(cm_ph_event_e_type event);
static AEECMEvent OEMCM_SSEventToAEECM(cm_ss_event_e_type event);
static AEECMEvent OEMCM_InbandEventToAEECM(cm_inband_event_e_type inband_event);
static uint32 OEMTelephone_XlateSSMask(uint32);
static AEECMCallError OEMCM_CallErrorToAEECM(cm_call_cmd_err_e_type error);
static AEECMPhError OEMCM_PhErrorToAEECM(cm_ph_cmd_err_e_type error);


static boolean OEMCM_CheckDTMFDigits( uint8 cnt, uint8 *digit_buf);

static int OEMCM_RejectCall(ICM *pMe, cm_call_id_type call_id);

static void  OEMCM_CallNotifyInitDefault(AEETNotifyInfo *notify_info, 
                                         cm_call_id_type call_id,
                                         sys_sys_mode_e_type sys_mode,
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
static void OEMCM_HandleCallCopyRedirNum(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallAlphaUpdate(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallExtDisplay(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallSupsStateChange(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallProgress(cm_mm_call_info_s_type *call_info, 
                                            AEECMEvent event);
static void OEMCM_HandleCallDormant(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallSignal(cm_mm_call_info_s_type *call_info, AEECMEvent event);
static void OEMCM_HandleCallSrvOpt(cm_mm_call_info_s_type *call_info, AEECMEvent event);

static void OEMCM_HandleCallSetupInd(cm_mm_call_info_s_type *call_info, AEECMEvent);
static void OEMCM_HandleCallMngCallsConf(cm_mm_call_info_s_type *, AEECMEvent);

static void OEMCM_HandlePhDefault(cm_ph_info_s_type *ph_info, AEECMEvent event);
static void OEMCM_HandlePhTestControl(cm_ph_info_s_type *ph_info, AEECMEvent event);
static void OEMCM_HandlePhInternalDefault(cm_ph_info_s_type *ph_info, AEECMEvent event);
static void OEMTelephone_CopyCharLen(char *cmchar, uint8 len, char *dest, uint16 size);

static void OEMTelephone_RefreshNetworks_Cb(void *data);
extern void OEMTelephone_Notify(AEETNotifyInfo *pData);

static cm_srv_type_e_type OEMTelephone_AEETToDMSSSrvType( AEETServiceType srv );
static AEETSysMode OEMTelephone_SysModeToAEET(sys_sys_mode_e_type sys_mod);
#ifdef FEATURE_UUS
static void OEMCM_HandleUserDataInd(cm_mm_call_info_s_type *call_info, AEECMEvent event);

static void OEMCM_InitUserData(cm_user_data_s_type *user_data, const AEETUserDataInfo *in_data);
#endif

#ifdef FEATURE_ACM
static int  ACMInfo_CreateInstance(ACMInfo * pAcmInfo);
static int  ACMInfo_Acquire(ACMInfo * pAcmInfo, uint32 reasonCode);
static int  ACMInfo_Free(ACMInfo * pAcmInfo);
static void OEMCM_ACMChangeNotify(void *p);
static void OEMCM_ACM_NetworkOriginatedCallChangeNotify(void *p);
static void OEMCM_ACM_MobileOriginatedCallChangeNotify(void *p);
static int  OEMCM_CallCM_AnswerCall(cm_call_cmd_cb_f_type  cmd_cb_func,
                                    void                   *data_block_ptr,
                                    cm_client_id_type      client_id,
                                    cm_call_id_type        call_id,
                                    cm_ans_params_s_type   *ans_params_ptr);
static void OEMCM_CallOrig_Notify(AEETNotifyInfo * notify_info);
#endif // FEATURE_ACM


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
  OEMCM_HandleCallCopyRedirNum,     /* AEET_EVENT_CALL_REDIRECTING_NUMBER */
  OEMCM_HandleCallDefault           /* AEET_EVENT_CALL_EXT_BUST_INTL */
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

                   ICM FUNCTION DEFINITIONS   

===========================================================================*/
/*=============================================================================
FUNCTION: OEMTelephoneCM_New

DESCRIPTION:  
  Allocates a new ICM object, creates a Core object if it is not present,
  stores the App context and initializes object data. A new ICM object is
  created for every ISHELL_CreateInstance().

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMTelephoneCM_New(IShell *pIShell, void **ppif)
{
   ICM *pNew;
  
   // Check parameters.
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }

   // Allocate the object.
   pNew = (ICM *) MALLOC(sizeof(ICM)); 
   if (pNew == NULL)
      return ENOMEMORY;


   /* Initialize the values. */
   pNew->m_pIShell = pIShell;
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
      (void) OEMCMCore_Release();
   }

   // Ref count is zero.  Release memory
   FREE(pMe);
   return (0);
}

/*---------------------------------------------------------------------------
                          CALL FUNCTIONS
---------------------------------------------------------------------------*/

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
static int OEMCM_OriginateCall_Int( ICM *pMe, 
                                    cm_call_type_e_type         call_type,
                                    cm_srv_type_e_type          srv_type,
                                    const cm_num_s_type         *calling_number,
                                    const cm_num_s_type         *called_number,
                                    const cm_orig_alpha_s_type  *alpha_buf,
                                    cm_cdma_orig_params_s_type  *cdma_orig_params_ptr,
                                    cm_gw_cs_orig_params_s_type *gw_cs_orig_params_ptr,
                                    cm_call_id_type             *return_call_id_ptr )
{
   AEETCallInfo *        core_call_info;
   AEECMEventCBS *        event_cb;
   AEECMCallErrCBPkt *   err_cb_pkt;

  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, CB_TYPE_ERRPKT);

  if (!event_cb) {
    MSG_ERROR("OEMCM_OriginateVoiceCall: Failed to allocate event callback", 0, 0, 0);
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
  if( FALSE == cm_mm_call_cmd_orig( OEMCM_DMSSCallErrCB, event_cb, gCMID, call_type,
                                    srv_type, calling_number, called_number, alpha_buf, 
                                    cdma_orig_params_ptr, gw_cs_orig_params_ptr,
                                    NULL, (cm_call_id_type *)&(err_cb_pkt->call_id)) 
                           || CM_CALL_ID_INVALID == (cm_call_id_type)err_cb_pkt->call_id ) {
    OEMCM_FreeEventCBS( event_cb );
    return EFAILED;
  }
  
  OEMCM_ResetPrivateCallState(err_cb_pkt->call_id);
  *return_call_id_ptr = (AEECMCallID)(err_cb_pkt->call_id + 1);

  /* OK we got the call id */
  OEMCM_FreeEventCBS(event_cb); 

  core_call_info = &pMe->m_coreObj->m_call_info[*return_call_id_ptr-1];

  (void) MEMSET(core_call_info, 0, sizeof(AEETCallInfo));

  OEMCM_CopyNumToOtherParty(called_number, core_call_info);

  core_call_info->call_state = AEECM_CALL_STATE_ORIG;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_type);

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
static int OEMCM_AnswerCall_Int( ICM *pMe, cm_call_id_type call_id, cm_call_type_e_type call_type )
{
   cm_ans_params_s_type    answer_params;
   AEECMEventCBS *         event_cb;
   AEECMCallErrCBPkt *     err_cb_pkt;

#if defined (FEATURE_ACM)
   ACM_NetworkOriginatedCallInfo * ACM_NOCallInfo;
#endif // FEATURE_ACM

  (void) MEMSET( &answer_params, 0, sizeof( cm_ans_params_s_type));

  switch (pMe->m_coreObj->m_call_mode_type[call_id]) {
  case CM_CALL_MODE_INFO_GW_CS:
     answer_params.info_type = CM_CALL_MODE_INFO_GW_CS;
     answer_params.ans_params.gw_cs_ans.accept = TRUE;
     answer_params.ans_params.gw_cs_ans.call_type = call_type;
     break;
  case CM_CALL_MODE_INFO_CDMA:
     answer_params.info_type = CM_CALL_MODE_INFO_CDMA;
     answer_params.ans_params.cdma_ans.call_type = call_type;
     break;
#ifdef CM_API_IP_CALL
  case CM_CALL_MODE_INFO_IP:
     answer_params.info_type = CM_CALL_MODE_INFO_IP;
     answer_params.ans_params.ip_ans.call_type = call_type;
     answer_params.ans_params.ip_ans.accept = TRUE;
     break;
#endif
  default:
     return EFAILED;
  }

  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, CB_TYPE_ERRPKT);

  if (!event_cb) {
    MSG_ERROR("OEMCM_AnswerCall: Failed to allocate event callback", 0, 0, 0);
    return EFAILED;
  }

  /* Fill in call_id here. */
  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
  err_cb_pkt->call_id = call_id;

#if defined (FEATURE_ACM)
  if (CM_CALL_TYPE_VOICE == call_type ||
      CM_CALL_TYPE_EMERGENCY == call_type)
  {
    /* Fill in data required for acquire & delayed acquire for Network Originated voice calls */
    ACM_NOCallInfo = (ACM_NetworkOriginatedCallInfo *) OEMCM_MALLOC (sizeof(ACM_NetworkOriginatedCallInfo));

    ACM_NOCallInfo->data_block_ptr    = event_cb;
    ACM_NOCallInfo->client_id         = gCMID;
    ACM_NOCallInfo->call_id           = call_id;
    MEMCPY(&ACM_NOCallInfo->answer_params, &answer_params, sizeof(cm_ans_params_s_type));

    ICMCoreObj->m_pAcmInfo->m_pCBInfo = ACM_NOCallInfo;

    MSG_HIGH ("OEMCM: Check with ACM for MT Voice Call resources", 0, 0, 0);
    /* Acquire resource from Application Concurrency Manager */
    return ACMInfo_Acquire(ICMCoreObj->m_pAcmInfo, ACM_NETWORK_ORIGINATED_VOICE_CALL);
  }
  else
  {
    /* Invoke CM cmd */
    if (!cm_mm_call_cmd_answer(OEMCM_DMSSCallErrCB, event_cb, gCMID, call_id,
                               &answer_params)) {
      pMe->m_coreObj->m_state[call_id].bAnswered = FALSE;
      return EFAILED;
    }
    return AEE_SUCCESS;
  }
#else // !FEATURE_ACM
  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_answer(OEMCM_DMSSCallErrCB, event_cb, gCMID, call_id, 
                             &answer_params)) {
    pMe->m_coreObj->m_state[call_id].bAnswered = FALSE;
    OEMCM_FreeEventCBS( event_cb );
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
  AEECMCallState        call_state;
  AEETPhInfo *          ph_info = NULL;
  boolean               end = FALSE;
  cm_call_id_type           call_id;
  AEETSysMode          sys_mode;

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

  sys_mode = OEMT_GetSystemMode();

  if (call_state == AEECM_CALL_STATE_INCOM && 
      AEET_IS_MODE_GW(sys_mode)) {
    return OEMCM_RejectCall(pMe, call_id);
  }

  return OEMCM_EndCall_Int(pMe,call_id);
}

static int OEMCM_EndCall_Int( ICM *pMe, cm_call_id_type call_id )
{
   cm_end_params_s_type  end_param;
   AEETSysMode          sys_mode;
   AEECMEventCBS *        event_cb;
   AEECMCallErrCBPkt *   err_cb_pkt;

  (void ) MEMSET(&end_param, 0, sizeof(cm_end_params_s_type));

  end_param.call_id = call_id;

  sys_mode = OEMT_GetSystemMode();
  if (AEET_IS_MODE_GW(sys_mode)) {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    if (OEMCM_GetCallType(call_id) == AEECM_CALL_TYPE_PS_DATA) {
      end_param.info_type = CM_CALL_MODE_INFO_GW_PS;
      end_param.end_params.gw_ps_end.nsapi.valid = FALSE;
      end_param.end_params.gw_ps_end.tear_down_ind.valid = FALSE;
      end_param.end_params.gw_ps_end.cause = SM_REGULAR_DEACTIVATION;
    } else {
      end_param.info_type = CM_CALL_MODE_INFO_GW_CS;
    }
#endif
  } else {
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    end_param.info_type = CM_CALL_MODE_INFO_CDMA;
    end_param.end_params.cdma_end.end_reason_included = TRUE; 
    end_param.end_params.cdma_end.end_reason = CAI_REL_NORMAL;
#else
    MSG_ERROR("CDMA Feature not defined in this build !!", 0, 0, 0);
#endif
  }
  
  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, CB_TYPE_ERRPKT);

  if (!event_cb) {
    MSG_ERROR("OEMCM_EndCall: Failed to allocate event callback", 0, 0, 0);
    return EFAILED;
  }

  /* Fill in call_id here. */
  err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
  err_cb_pkt->call_id = call_id;

  /* Invoke CM cmd */
  if (!cm_mm_call_cmd_end(OEMCM_DMSSCallErrCB, event_cb, gCMID, 1, &end_param)) {
    pMe->m_coreObj->m_state[call_id].bEnded = FALSE;
    OEMCM_FreeEventCBS( event_cb );
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
      MSG_ERROR("OEMCM_EndAllCalls failed to end call %d with error %d\n", call_id_list[i], ret, 0);
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
                      AEETCallInfo *call_info,
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
                             AEETCallInfo *call_info,
                             uint32 size)
{
  if (!pMe || !call_info)
    return EBADPARM;
  
  if (!OEMCM_IsValidAppCallID(call_id))
    return EBADPARM;

  (void ) MEMCPY(call_info, &pMe->m_coreObj->m_call_info[call_id-1], 
                 size);

  call_info->sys_mode = OEMT_GetSystemMode();

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
  AEETSysMode          sys_mode;
  AEECMCallID           aee_call_id=0;

  if (!pMe)
    return EBADPARM;

  sys_mode = OEMT_GetSystemMode();

  if (!AEET_IS_MODE_CDMA(sys_mode))
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
                                   (cm_call_id_type)(aee_call_id - 1), 
                                   OEMCM_PrivacyPrefToDMSS(privacy_pref))) {
    return EFAILED;
  }

  return AEE_SUCCESS;
}

/*---------------------------------------------------------------------------
                          PHONE FUNCTIONS
---------------------------------------------------------------------------*/

/*=============================================================================
FUNCTION: OEMTelephone_CMModePrefToAEET

DESCRIPTION:  
   Map CM pref mode CM_MODE_PREF_XXX to AEET_MODE_PREF_XXX 


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEETModePref OEMTelephone_CMModePrefToAEET(cm_mode_pref_e_type cm_mode_pref)
{
   AEETModePref modePref = AEET_MODE_PREF_NONE;
   
   switch(cm_mode_pref)
   {       
      case CM_MODE_PREF_AMPS_ONLY:
         modePref = AEET_MODE_PREF_AMPS_ONLY;
         break;

      case CM_MODE_PREF_DIGITAL_ONLY:
         modePref = AEET_MODE_PREF_DIGITAL_ONLY;
         break;

      case CM_MODE_PREF_AUTOMATIC:
         modePref = AEET_MODE_PREF_AUTOMATIC;
         break;

      case CM_MODE_PREF_EMERGENCY:
         modePref = AEET_MODE_PREF_EMERGENCY;
         break;

      case CM_MODE_PREF_CDMA_ONLY:
         modePref = AEET_MODE_PREF_CDMA_ONLY;
         break;

      case CM_MODE_PREF_HDR_ONLY:
         modePref = AEET_MODE_PREF_HDR_ONLY;
         break;

      case CM_MODE_PREF_CDMA_AMPS_ONLY:
         modePref = AEET_MODE_PREF_CDMA_AMPS_ONLY;
         break;

      case CM_MODE_PREF_GPS_ONLY:
         modePref = AEET_MODE_PREF_GPS_ONLY;
         break;   

      case CM_MODE_PREF_GSM_ONLY:
         modePref = AEET_MODE_PREF_GSM_ONLY;
         break;

      case CM_MODE_PREF_WCDMA_ONLY:
         modePref = AEET_MODE_PREF_WCDMA_ONLY;
         break;    

      case CM_MODE_PREF_PERSISTENT:
         modePref = AEET_MODE_PREF_PERSISTENT;
         break;   

      case CM_MODE_PREF_NO_CHANGE:
         modePref = AEET_MODE_PREF_NO_CHANGE;
         break;   
      
      case CM_MODE_PREF_CDMA_HDR_ONLY:
         modePref = AEET_MODE_PREF_CDMA_HDR_ONLY;
         break; 
#ifdef CM_API_WLAN
      case CM_MODE_PREF_WLAN_ONLY:
         modePref = AEET_MODE_PREF_WLAN_ONLY;
         break; 
#endif
      case CM_MODE_PREF_GSM_WCDMA_ONLY:
         modePref = AEET_MODE_PREF_GSM_WCDMA_ONLY;
         break;  

      default:
         modePref = (AEETModePref)cm_mode_pref;
         MSG_ERROR("OEMTelephone_CMModePrefToAEET: No AEETModePref for cm pref mode : %d", 
            cm_mode_pref, 0, 0);
         
   }
   
   return modePref;
}

/*=============================================================================
FUNCTION: OEMTelephone_CMModePrefFromAEET

DESCRIPTION:  
   Map AEET_MODE_PREF_XXX to CM_MODE_PREF_XXX

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static cm_mode_pref_e_type OEMTelephone_CMModePrefFromAEET(AEETModePref modePref)
{
   cm_mode_pref_e_type cm_mode_pref = CM_MODE_PREF_NONE;
   
   switch(modePref)
   {       
      case AEET_MODE_PREF_AMPS_ONLY:
         cm_mode_pref = CM_MODE_PREF_AMPS_ONLY;
         break;

      case AEET_MODE_PREF_DIGITAL_ONLY:
         cm_mode_pref = CM_MODE_PREF_DIGITAL_ONLY;
         break;

      case AEET_MODE_PREF_AUTOMATIC:
         cm_mode_pref = CM_MODE_PREF_AUTOMATIC;
         break;

      case AEET_MODE_PREF_EMERGENCY:
         cm_mode_pref = CM_MODE_PREF_EMERGENCY;
         break;

      case AEET_MODE_PREF_CDMA_ONLY:
         cm_mode_pref = CM_MODE_PREF_CDMA_ONLY;
         break;

      case AEET_MODE_PREF_HDR_ONLY:
         cm_mode_pref = CM_MODE_PREF_HDR_ONLY;
         break;

      case AEET_MODE_PREF_CDMA_AMPS_ONLY:
         cm_mode_pref = CM_MODE_PREF_CDMA_AMPS_ONLY;
         break;

      case AEET_MODE_PREF_GPS_ONLY:
         cm_mode_pref = CM_MODE_PREF_GPS_ONLY;
         break;   

      case AEET_MODE_PREF_GSM_ONLY :
         cm_mode_pref = CM_MODE_PREF_GSM_ONLY;
         break;

      case AEET_MODE_PREF_WCDMA_ONLY :
         cm_mode_pref = CM_MODE_PREF_WCDMA_ONLY;
         break;    

      case AEET_MODE_PREF_PERSISTENT:
         cm_mode_pref = CM_MODE_PREF_PERSISTENT ;
         break;   

      case AEET_MODE_PREF_NO_CHANGE:
         cm_mode_pref = CM_MODE_PREF_NO_CHANGE ;
         break;   
      
      case AEET_MODE_PREF_CDMA_HDR_ONLY:
         cm_mode_pref = CM_MODE_PREF_CDMA_HDR_ONLY ;
         break; 

#ifdef CM_API_WLAN
      case AEET_MODE_PREF_WLAN_ONLY:
         cm_mode_pref = CM_MODE_PREF_WLAN_ONLY ;
         break; 
#endif
      case AEET_MODE_PREF_GSM_WCDMA_ONLY:
         cm_mode_pref = CM_MODE_PREF_GSM_WCDMA_ONLY ;
         break;  

      default:
         MSG_ERROR("OEMTelephone_CMModePrefFromAEET: Invalid AEETPrefMode : %d",
            modePref, 0, 0);
   }
   
   return cm_mode_pref;
}


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
static int OEMCM_SetSystemPreference(ICM *pMe, AEETModePref mode_pref,
                             AEETPrefTerm pref_term, uint32 pref_duration,
                             AEETAcqOrderPref acq_order_pref,
                             AEETBandPref band_pref, AEETRoamPref roam_pref,
                             AEETHybrPref hybr_pref,
                             AEETServDomPref srv_domain_pref,
                             AEETNwSelModePref nw_sel_mode_pref,
                             AEETPLMNID * plmn_ptr, 
                             AEECMPhError *status,
                             AEECallback *cb)
{
  AEECMErrCBS * err_cb = NULL;
  cm_mode_pref_e_type cm_mode_pref;

  if (!pMe) {
    return AEE_EBADPARM;
  }

  if (mode_pref == AEET_MODE_PREF_WLAN_ONLY) {
     return AEE_EUNSUPPORTED;
  }
  
  cm_mode_pref = OEMTelephone_CMModePrefFromAEET(mode_pref);
  // Invalid AEETModePref
  if (CM_MODE_PREF_NONE == cm_mode_pref) {
     return AEE_EBADPARM;
  }

  if (cb) {

    if (!cb->pfnNotify) {
      MSG_ERROR("Null function pointer provided as callback", 0, 0, 0);
      return AEE_EBADPARM;
    }

    if (!status) {
      MSG_ERROR("Null status pointer", 0, 0, 0);
      return AEE_EBADPARM;
    }

    err_cb = OEMCM_AllocErrCBS(pMe);
    if (!err_cb) {
      MSG_ERROR("Unable to get ICM Error Callback", 0, 0, 0);
      return AEE_EFAILED;
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

  if (!cm_ph_cmd_sys_sel_pref(
                           OEMCM_DMSSPhErrCB,
                           (err_cb ? (void *)err_cb->hObject : NULL), 
                           gCMID,
                           cm_mode_pref,
                           (cm_pref_term_e_type) pref_term,
                           (dword) pref_duration,
                           (cm_gw_acq_order_pref_e_type) acq_order_pref,
                           (cm_band_pref_e_type) band_pref,
#if defined(CM_API_SYS_SEL_PRL_PREF)
                           CM_PRL_PREF_NO_CHANGE,
#endif
                           (cm_roam_pref_e_type) roam_pref,
                           AEET_XlateHybrPref(hybr_pref),
                           (cm_srv_domain_pref_e_type) srv_domain_pref,
                           (cm_network_sel_mode_pref_e_type) nw_sel_mode_pref,
                           &pMe->m_cm_plmn_id
#ifdef CM_API_WLAN
                           , NULL
#endif
                           )) {
    if (cb) {
      cb->pfnCancel = NULL;
      cb->pCancelData = NULL;

      OEMCM_FreeErrCBS(pMe, err_cb);
    }
    return AEE_EFAILED;
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
static int OEMCM_SetNAMSel(ICM *pMe, cm_nam_e_type nam_sel)
{
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  
  /* Invoke CM command */
  if (!cm_ph_cmd_nam_sel(OEMCM_DMSSPhErrCB, NULL, gCMID, nam_sel)) {
    return EFAILED;
  }

  return AEE_SUCCESS;
#else
  return EUNSUPPORTED;
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
  AEETSysMode sys_mode;

  if (!pMe)
    return EBADPARM;

  sys_mode = OEMT_GetSystemMode();
  if (!AEET_IS_MODE_GW(sys_mode))
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
#if defined(FEATURE_RUIM)
  AEETPhInfo * ph_info;
#endif

  if (!pMe)
    return EBADPARM;

  
  if (!IN_RANGE(rtre_config, AEECM_RTRE_CONFIG_RUIM_ONLY,
                AEECM_RTRE_CONFIG_RUIM_OR_DROP_BACK)) {
    MSG_ERROR("OEMCM_SetRTREConfig: CM_PH_CMD_ERR_RTRE_CONFIG_P", 0, 0, 0);
    return EBADPARM;              
  }

#if defined(FEATURE_RUIM)

  if ((ph_info = OEMCM_GetValidPhInfo(pMe)) == NULL) {
    MSG_ERROR("OEMCM_SetRTREConfig: Phone info not available", 0, 0, 0);
    return EFAILED;
  }

  if (ph_info->curr_nam != NV_RUIM_NAM) {
    MSG_ERROR("OEMCM_SetRTREConfig: CM_PH_CMD_ERR_CURRENT_NAM_IS_NOT_RUIM_NAM_S", 0, 0, 0);
    return EBADSTATE;
  }

#endif
  
#if defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE)
  if (!cm_ph_cmd_change_rtre_config(OEMCM_DMSSPhErrCB, NULL, gCMID, 
                                    (cm_rtre_config_e_type)rtre_config)) {
    return EFAILED;
  }
  
  return AEE_SUCCESS;
#else
  return EUNSUPPORTED;
#endif
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
  AEETSysMode sys_mode;

  if (!pMe)
    return EBADPARM;

  sys_mode = OEMT_GetSystemMode();
  if (!AEET_IS_MODE_CDMA(sys_mode))
    return EUNSUPPORTED;

  if (!IN_RANGE(answer_voice, AEECM_ANSWER_VOICE_AS_VOICE,
                AEECM_ANSWER_VOICE_AS_MODEM_ALWAYS)) {
    MSG_ERROR("OEMCM_SetAnswerVoicePref: CM_PH_CMD_ERR_ANSWER_VOICE_P",0, 0, 0);
    return EBADPARM;
  }

  if ((answer_voice == AEECM_ANSWER_VOICE_AS_FAX_ONCE ||
       answer_voice == AEECM_ANSWER_VOICE_AS_MODEM_ONCE) &&
       duration == 0) {
    MSG_ERROR("OEMCM_SetAnswerVoicePref: CM_PH_CMD_ERR_ANSWER_DURATION_P", 0, 0, 0);
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
  AEETPhInfo  * ph_info;

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

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  if (!cm_ph_cmd_aoc_reset_acm(OEMCM_DMSSPhErrCB, NULL, gCMID))
    return EFAILED;
  return AEE_SUCCESS;
#else
  return EUNSUPPORTED;
#endif
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

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  if (!cm_ph_cmd_aoc_set_acmmax(OEMCM_DMSSPhErrCB, NULL, gCMID, acm_max))
    return EFAILED;
  return AEE_SUCCESS;
#else
  return EUNSUPPORTED;
#endif
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
  #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  #ifdef SYS_DDTM_MAX_SO_LIST_SIZE
    sys_srv_opt_type so_list[] = { (sys_srv_opt_type) CAI_SO_NULL };
  #endif
  #endif

  if (!pMe)
    return EBADPARM;

  if (!IN_RANGE(ddtm_pref, AEECM_DDTM_PREF_OFF, AEECM_DDTM_PREF_NO_CHANGE)) 
    return EBADPARM;


  if (!cm_ph_cmd_ddtm_pref(OEMCM_DMSSPhErrCB,
                           NULL, 
                           gCMID, 
                           (cm_ddtm_pref_e_type)ddtm_pref
#ifdef SYS_DDTM_MAX_SO_LIST_SIZE
                          ,SYS_DDTM_ACT_MASK_ALL,
                           CM_DDTM_SO_LIST_ACT_ADD,
                           ARR_SIZE( so_list),
                           so_list
#endif
                           )) 
  {
    MSG_ERROR("Failed to set DDTM pref to %d", ddtm_pref, 0, 0);
    return EFAILED;
  }

  return AEE_SUCCESS;
#endif
}

/*---------------------------------------------------------------------------
                           INBAND FUNCTIONS
---------------------------------------------------------------------------*/

/*=============================================================================
FUNCTION: OEMTelephone_CMDTMFOnLengthFromAEET

DESCRIPTION:  
  Map AEET_DTMF_ON_XXX to uint8 used by cm

=============================================================================*/
static uint8 OEMTelephone_CMDTMFOnLengthFromAEET(AEETDTMFOnLength onLength)
{ 
   uint8 cm_on_length = 0;

   switch(onLength) {
   case AEET_DTMF_ON_95:
      cm_on_length = 0;
      break;

   case AEET_DTMF_ON_150:
      cm_on_length = 1;
      break;

   case AEET_DTMF_ON_200:
      cm_on_length = 2;
      break;

   case AEET_DTMF_ON_250:
      cm_on_length = 3;
      break;

   case AEET_DTMF_ON_300:
      cm_on_length = 4;
      break;

   case AEET_DTMF_ON_350:
      cm_on_length = 5;
      break;

   case AEET_DTMF_ON_SMS:
      cm_on_length = 7;
      break;

   default:
      break;
   }   

   return cm_on_length; 
}

/*=============================================================================
FUNCTION: OEMTelephone_CMDTMFOffLengthFromAEET

DESCRIPTION:  
  Map AEET_DTMF_OFF_XXX to uint8 used by cm

=============================================================================*/
static uint8 OEMTelephone_CMDTMFOffLengthFromAEET(AEETDTMFOffLength offLength)
{ 
   uint8 cm_off_length = 0;

   switch(offLength) {
   case AEET_DTMF_OFF_60:
      cm_off_length = 0;
      break;

   case AEET_DTMF_OFF_100:
      cm_off_length = 1;
      break;
   
   case AEET_DTMF_OFF_150:
      cm_off_length = 2;
      break;

   case AEET_DTMF_OFF_200:
      cm_off_length = 3;
      break;

   default:
      break;
   }   

   return cm_off_length; 
}

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
static int OEMCM_BurstDTMF_Int(ICM *pMe, AEECMCallID call_id, 
                               AEECMDTMFOnLength on_length, 
                               AEECMDTMFOffLength off_length, const char *digits)
{
  AEECMBurstDTMFPkt *dtmf_pkt;
  int ret;

  // Convert to DMSS call ID
  cm_call_id_type id = (cm_call_id_type) (call_id - 1);

    // Range check the call ID
  if (id >= OEMCM_MAX_NUM_CALLS) {
     return EFAILED;
  }  

  switch (pMe->m_coreObj->m_call_mode_type[id]) {

    case CM_CALL_MODE_INFO_CDMA:
      /* Normal burst DTMF */
      if (!cm_mm_inband_cmd_burst_dtmf(OEMCM_DMSSInbandErrCB, NULL, gCMID, 
                                      (cm_call_id_type)(call_id -1),
                                      OEMTelephone_CMDTMFOnLengthFromAEET(on_length), 
                                      OEMTelephone_CMDTMFOffLengthFromAEET(off_length), 
                                      (uint8) STRLEN(digits),
                                      (uint8 *)digits)) {
        return EFAILED;
      }
      break;

    case CM_CALL_MODE_INFO_GW_CS:
    case CM_CALL_MODE_INFO_GW_PS:
#ifdef CM_API_IP_CALL    
    case CM_CALL_MODE_INFO_IP:
#endif
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
static int OEMCM_StartContDTMF_Int(ICM *pMe, AEECMCallID call_id, char digit)
{
   if (!pMe || !pMe->m_coreObj)
     return EBADPARM;

   if (!OEMCM_IsValidAppCallID(call_id))
     return EBADPARM;

   if (!OEMCM_CheckDTMFDigits(1, (uint8 *)&digit)) 
     return EBADPARM;

   if (pMe->m_coreObj->m_dtmf_start_times[call_id -1] != 0)
     return EBADSTATE;
  
   if (!cm_mm_inband_cmd_start_cont_dtmf(OEMCM_DMSSInbandErrCB, NULL, gCMID, 
                                        (cm_call_id_type)(call_id -1),
                                        (uint8)digit)) {
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
                                       (cm_call_id_type)(call_id-1), 
                                       (word)duration)) {
    return EFAILED;
  }

  pMe->m_coreObj->m_dtmf_start_times[call_id - 1] = 0;
  
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
  AEETCallInfo *core_call_info;
  
  MSG_HIGH("Received Call Event 0x%X from DMSS CM", call_event, 0, 0);

  if (!ICMCoreObj || !call_info_ptr) {
    MSG_ERROR("OEMCM_DMSSCallEventCB: Unexpected NULL pointer", 0, 0, 0);
    return;
  }
  
  if (call_event == CM_CALL_EVENT_INFO) {
      
    if(call_info_ptr->call_id >= OEMCM_MAX_NUM_CALLS){
       return; 
    }
    /* copy and return. Received only if ICM called get info */
    core_call_info = &(ICMCoreObj->m_call_info[call_info_ptr->call_id]);
    OEMCM_CopyInCallInfo(core_call_info, call_info_ptr);
    
    return;
  }

  evt = OEMCM_CallEventToAEECM(call_event);

  //Check for valid call events
  OEMCM_ValidCallEvent(&evt, call_info_ptr); 
  
  if (AEECM_EVENT_NONE == evt) {
    return;
  }    
  
  MSG_MED("AEECMEvent is %d\n", evt, 0, 0);

  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, CB_TYPE_CALL_INFO);
  if (!event_cb)
    return;

  event_cb->event = evt;
  (void) MEMCPY(event_cb->data, call_info_ptr, sizeof(cm_mm_call_info_s_type));

  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWCallEventCB, event_cb);
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
  AEECMEventCBS *event_cb;
  AEECMCallErrCBPkt *params;
  AEECMCallError  err;

  PARAM_NOT_REF(call_cmd);

  if (call_cmd_err == CM_CALL_CMD_ERR_NOERR) {
     MSG_MED("OEMCM_DMSSCallErrCB received result 0x%X for Call command %d", 
                                          call_cmd_err, call_cmd, 0);  
  }
  else {
     MSG_ERROR("OEMCM_DMSSCallErrCB received result 0x%X for Call command %d", 
                                          call_cmd_err, call_cmd, 0);  
  }

  /* Call error Notifications are sent up even if the app that 
     caused the error exits */
  if (!ICMCoreObj)
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

  AEE_ResumeCallback(&(event_cb->cb), 0);
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

  MSG_HIGH("OEMCM_DMSSOtherCallErrCB received result 0x%X for Call command %d", cmd_err, cmd, 0);  

  if (!ICMCoreObj) {
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
  
  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, CB_TYPE_ERRPKT);
  if (!event_cb)
    return;

  event_cb->event = AEECM_EVENT_CALL_ERROR;
  params = (AEECMCallErrCBPkt *)event_cb->data;
  params->call_id = call_id;
  params->call_error = err;

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
      OEMTelephone_CopyInPhInfo(&ICMCoreObj->m_ph_info, ph_info_ptr);
      ICMCoreObj->m_ph_valid = TRUE;
      return;

    case CM_PH_EVENT_PREFERRED_NETWORKS_CONF:
      return;

#ifdef FEATURE_ALS
    case CM_PH_EVENT_SELECTED_LINE:
      /* Selected line changed, FEATURE_ALS */
      ICMCoreObj->m_ph_info.line_info.dwSelectedLineId = ph_info_ptr->line;
      MSG_MED("OEMCM_DMSSPhEventCB: line 0x%x", ph_info_ptr->line, 0, 0);
      break;

    case CM_PH_EVENT_LINE_SWITCHING:
      ICMCoreObj->m_ph_info.line_info.bLineSwitchingEnabled 
         = (ph_info_ptr->line_switching == CM_ALS_LINE_SWITCHING_ALLOWED)?TRUE:FALSE;
      break;
#endif

    default:
      break;
  }

  evt = OEMCM_PhEventToAEECM(ph_event);
  if (evt == AEECM_EVENT_NONE)
    return;

  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, CB_TYPE_PH_INFO);
  if (!event_cb)
    return;

  event_cb->event = evt;
  (void) MEMCPY(event_cb->data, ph_info_ptr, sizeof(cm_ph_info_s_type)); 

   if (AEET_EVENT_PH_AVAILABLE_NETWORKS == evt) {
      CALLBACK_Init(&(event_cb->cb), OEMTelephone_RefreshNetworks_Cb, 
                    event_cb);
   }
   else {
  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWPhEventCB, event_cb);
   }

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

  ICMCoreObj->ssEvt = ss_event;
  MEMCPY(&ICMCoreObj->ssInfo, ss_info_ptr, sizeof(cm_mm_ss_info_s_type));
  ICMCoreObj->bSSInfoValid = TRUE;
  
  if(TRUE == ICMCoreObj->bInitSS){
     if(ICMCoreObj->piSignalSS){
        ISignal_Set(ICMCoreObj->piSignalSS);
     }
  }
    
  if (ss_event == CM_SS_EVENT_INFO) {
    /* Copy and Return */
    OEMTelephone_CopyInSSInfo(ss_info_ptr);
    return;
  }

  evt = OEMCM_SSEventToAEECM(ss_event);
  if (evt == AEECM_EVENT_NONE)
    return;

  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, CB_TYPE_SS_INFO);
  if (!event_cb) {
     if (AEECM_EVENT_SS_SRV_CHANGED == evt) {
        /* Copy */
        OEMTelephone_CopyInSSInfo(ss_info_ptr);
     }
     return;
  }

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

  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, CB_TYPE_INBAND_INFO);
  if (!event_cb)
    return;

  event_cb->event = evt;
  (void) MEMCPY(event_cb->data, inband_info_ptr, sizeof(cm_inband_info_s_type));

  CALLBACK_Init(&(event_cb->cb), OEMCM_BREWInbandEventCB, event_cb);

  AEE_ResumeCallback(&(event_cb->cb), 0);
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

  if (ph_cmd_err == CM_PH_CMD_ERR_NOERR) {
     MSG_MED("OEMCM_DMSSPhErrCB received result 0x%X for Phone command %d", 
                                     ph_cmd_err, ph_cmd, 0);  
  }
  else {
     MSG_ERROR("OEMCM_DMSSPhErrCB received result 0x%X for Phone command %d", 
                                     ph_cmd_err, ph_cmd, 0);  
  }

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

  pfnNotify = NULL;
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
    if (fn_index < ARR_SIZE(gCallEventHandlers)) {
      pfnNotify = gCallEventHandlers[fn_index];
    }

  } else if (AEECM_EVENT_GROUP(event) == AEECM_INTERNAL_CALL_GROUP) {

    /* Internal call events that are not sent to the App */
    if (fn_index < ARR_SIZE(gCallIntEventHandlers)) {
      pfnNotify = gCallIntEventHandlers[fn_index];
    }

#ifdef FEATURE_UUS
  } else if (AEECM_EVENT_GROUP(event) == AEECM_USER_GROUP) {

    if (fn_index == 1) {
      pfnNotify = OEMCM_HandleUserDataInd;
    }

#endif
  } else {
    MSG_ERROR("OEMCM_BREWCallEventCB: Unknown event (0x%X)", event, 0, 0);
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
  AEETNotifyInfo * notify_info;
  AEETCallInfo *   core_call_info;

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    OEMCM_FreeEventCBS(event_cb);
    return;
  }

  notify_info->event = event_cb->event;
  params = (AEECMCallErrCBPkt *)event_cb->data;
  notify_info->event_data.call.cd = params->call_id+1;
  notify_info->event_data.call.call_error = params->call_error;

  core_call_info = &(ICMCoreObj->m_call_info[params->call_id]);
  /* Copy call info from core object to Notify info */
  if (core_call_info->call_state != AEECM_CALL_STATE_IDLE &&
      core_call_info->call_state != AEECM_CALL_STATE_ENDED ) {
    (void) MEMCPY(&(notify_info->event_data.call.call_info), core_call_info, 
           sizeof(AEETCallInfo));
  }

  OEMTelephone_Notify(notify_info);
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
   AEECMEventCBS *          event_cb = (AEECMEventCBS *)data;
   cm_mm_ss_info_s_type *   ss_info;
   AEETSSInfo *             pSSInfo;
   boolean                  bNotify = TRUE;
   uint32                   dwMask = 0;

   if (!event_cb || !ICMCoreObj)
      return;

   ss_info = (cm_mm_ss_info_s_type *)event_cb->data;
   pSSInfo = &ICMCoreObj->m_ss_info;

   switch(event_cb->event) {
   case AEECM_EVENT_SS_SRV_CHANGED:
      dwMask = OEMTelephone_XlateSSMask(ss_info->changed_fields);
      OEMTelephone_CopyInSSInfo(ss_info);
      if (!dwMask) {
         bNotify = FALSE;
      }
      break;

   case AEECM_EVENT_SS_RSSI:
      pSSInfo->rssi = ss_info->rssi;
      break;

   case AEECM_EVENT_SS_HDR_RSSI:
      #ifdef FEATURE_HDR
      pSSInfo->hdr_rssi = ss_info->hdr_rssi;
      #endif
      break;

   case AEET_EVENT_SS_WLAN_RSSI:
      #ifdef CM_API_WLAN
      pSSInfo->wlan_rssi = ss_info->wlan_rssi;
      #endif
      break;

   default:
      bNotify = FALSE;
   }

   if (bNotify) {
      AEETNotifyInfo *notify_info;

      /* Allocate a notify buffer for broadcast to applications */
      notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo));
      if (notify_info) {

         /* Load the data to notify buffer */
         notify_info->event = event_cb->event;
         notify_info->event_data.ss.changed_fields = dwMask;
         (void) MEMCPY(&notify_info->event_data.ss.ss_info, pSSInfo, 
                       sizeof(AEETSSInfo));
         OEMTelephone_Notify(notify_info);
         OEMCM_FREE(notify_info);
      }
   }

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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  cm_inband_info_s_type * inband_info;

  if (!data || !ICMCoreObj)
    return;

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
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

  notify_info->event_data.inb.cd = call_id+1;

  OEMTelephone_Notify(notify_info);
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
 
  MSG_MED("OEMCM_HandleCallSetupInd received event 0x%X", event, 0, 0);

  if (event != AEECM_EVENT_INTERNAL_SETUP_IND) {
    MSG_ERROR("OEMCM_HandleCallSetupInd received unexpected event 0x%X", 
                                                              event, 0, 0);
    return;
  }

#ifdef FEATURE_MULTIMEDIA_FALLBACK
  /* If this is a data call or a voice call in dual mode do not send setup response to CM
   * Data services will take care of this and UI will only get an indication from CM that the
   * setup response was sent
   */

  if((call_info->call_type == CM_CALL_TYPE_CS_DATA) ||
     ((call_info->call_type == CM_CALL_TYPE_VOICE) &&
      (call_info->mode_info.info.gw_cs_call.bearer_capability_1.present) &&
      (call_info->mode_info.info.gw_cs_call.bearer_capability_2.present) &&
      (OEMCM_REPEAT_SERVICE_CHANGE_FALLBACK == call_info->mode_info.info.gw_cs_call.bc_repeat_indicator.repeat_indication)))
  {
    return;
  }
#else

  if (call_info->call_type != CM_CALL_TYPE_VOICE)
    return;
#endif

  (void) MEMSET(&setup_params, CM_CALL_CMD_PARAM_DEFAULT_VALUE, 
         sizeof(cm_setup_res_params_s_type));

  if (call_info->call_type == CM_CALL_TYPE_VOICE) {
    (void) MEMCPY(&setup_params.bearer_capability_1, 
                  &ICMCoreObj->def_gw_orig_params.bearer_capability_1, 
                  sizeof(cm_bearer_capability_s_type));
    
    OEMCall_compose_setup_params(&setup_params);

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
static __inline
boolean Prv_IsActiveCall(active_calls_list_s_type* p_calls, cm_call_id_type call_id)
{
   boolean bRet = FALSE;
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
   int i;

   for (i = 0; i < p_calls->size; i++) {
      if (p_calls->table[i] == call_id) {
         bRet = TRUE;
         break;
      }
   }
#endif

   return bRet;
}

static void OEMCM_HandleCallMngCallsConf(cm_mm_call_info_s_type *call_info, 
                                         AEECMEvent event)
{
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
   cm_gw_cs_call_info_s_type *     gw_cs_call;
   cm_call_id_type                 call_id;
   AEETNotifyInfo *               notify_info;


   MSG_MED("OEMCM_HandleCallMngCallsConf received event 0x%X", event, 0, 0);

   if (event != AEECM_EVENT_INTERNAL_MNG_CALLS_CONF) {
      MSG_ERROR("OEMCM_HandleCallMngCallsConf received unexpected event 0x%X", 
                event, 0, 0);
      return;
   }

   if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
      MSG_ERROR("OEMCM_HandleCallMngCallsConf : Out of memory", 0, 0, 0);
      return;
   }

   gw_cs_call = &call_info->mode_info.info.gw_cs_call;
   if (gw_cs_call->call_ss_success) {

      /* For each call find in the find if it is in active call list */
      for (call_id = 0; call_id < OEMCM_MAX_NUM_CALLS; call_id++) {

         if (Prv_IsActiveCall(&gw_cs_call->active_calls_list, call_id)) {
            /* If the local call state is on hold, retrieve */
            if (AEET_CALL_STATE_ONHOLD == OEMCM_GetCallState(call_id)) {

               ICMCoreObj->m_call_info[call_id].call_state = AEET_CALL_STATE_CONV;

               OEMCM_CallNotifyInitDefault(notify_info, call_id, call_info->sys_mode,
                                           AEECM_EVENT_CALL_RETRIEVED);
               OEMTelephone_Notify(notify_info);
            }
         }
         else {
            /* If the local call state is on Active conversation, put it on hold */
            if (AEET_CALL_STATE_CONV == OEMCM_GetCallState(call_id)) {

               ICMCoreObj->m_call_info[call_id].call_state = AEET_CALL_STATE_ONHOLD;

               OEMCM_CallNotifyInitDefault(notify_info, call_id, call_info->sys_mode,
                                           AEECM_EVENT_CALL_ON_HOLD);
               OEMTelephone_Notify(notify_info);
            }
         }
      }
   } 
   else {
      /* Send SUPS ERROR */
      OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode,
                                  AEECM_EVENT_CALL_OPS_ERROR);

      notify_info->event_data.call.callops_error = (AEESuppSvcSSErrorType)
      (gw_cs_call->ss_error.error_code |
       (gw_cs_call->ss_error.error_code_tag << 16));
      OEMTelephone_Notify(notify_info);
   }

   OEMCM_FREE(notify_info);
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
  AEETNotifyInfo * notify_info;

  MSG_MED("OEMCM_HandleCallDefault received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  AEETCallInfo * core_call_info;

  MSG_MED("OEMCM_HandleCallDefault received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  core_call_info = &(ICMCoreObj->m_call_info[call_info->call_id]);  

  core_call_info->call_progress = AEET_PROGIND_UNKNOWN;

  // save the given call mode info for later use.
  ICMCoreObj->m_call_mode_type[call_info->call_id] = call_info->mode_info.info_type;

  if (CM_CALL_MODE_INFO_GW_CS == call_info->mode_info.info_type) {
     core_call_info->call_progress = (AEETCallProgress) 
                        call_info->mode_info.info.gw_cs_call.call_progress;
  }
#ifdef CM_API_IP_CALL
  else if (CM_CALL_MODE_INFO_IP == call_info->mode_info.info_type) {

     if (CM_CALL_PROG_INFO_REMOTE_ALERT == call_info->mode_info.info.ip_call.call_prog_info) {
        core_call_info->call_progress = AEET_PROGIND_REMOTE_ALERT;
     }
  }
#endif
  
  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;

  MSG_MED("OEMCM_HandleCallNumUpdate received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  OEMCM_CopyNumToAEECMNum(&call_info->num, 
                          &notify_info->event_data.call.number);

  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  AEETCallInfo   * core_call_info;

  MSG_MED("OEMCM_HandleCallCopyNum received event 0x%X", event, 0, 0);

  if (!call_info || !ICMCoreObj)
    return;

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  core_call_info = &(ICMCoreObj->m_call_info[call_info->call_id]);

  OEMCM_CopyNumToOtherParty(&(call_info->num), core_call_info);

  OEMTelephone_CopyCharLen((char *)call_info->num.subaddr.chari, 
                             call_info->num.subaddr.num_char,
                             core_call_info->other_party_subaddr, 
                             sizeof(core_call_info->other_party_subaddr)); 

  OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf, 
                            call_info->alpha.len,
                            core_call_info->alpha, 
                            sizeof(core_call_info->alpha));

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  OEMCM_CopyNumToAEECMNum(&call_info->num, 
                          &notify_info->event_data.call.number);

  OEMTelephone_Notify(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleCallCopyRedirNum

DESCRIPTION:  
  Copies redirected number to notify info and updates the core call info
  redirected number

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_HandleCallCopyRedirNum(cm_mm_call_info_s_type *call_info, AEECMEvent event)
{
  AEETNotifyInfo * notify_info;
  AEETCallInfo   * core_call_info;

  MSG_MED("OEMCM_HandleCallCopyRedirNum received event 0x%X", event, 0, 0);

  if (!call_info || !ICMCoreObj)
    return;

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  core_call_info = &(ICMCoreObj->m_call_info[call_info->call_id]);
  
  /* Copy the redirection number into our core call information struct */
  OEMCM_CopyNumToAEECMNum(&call_info->redirecting_number.num, 
                          &core_call_info->redirect_party_number);

  /* Initalize the notify call information with the core call information */
  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  /* Copy the redirection number into the number buffer specific to this event */
  OEMCM_CopyNumToAEECMNum(&call_info->redirecting_number.num, 
                          &notify_info->event_data.call.number);

  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallAlphaUpdate received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  call_id = (call_info->call_id >= OEMCM_MAX_NUM_CALLS) ? 0 : call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);  

  OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf, 
                            call_info->alpha.len,
                            core_call_info->alpha, 
                            sizeof(core_call_info->alpha));

  OEMCM_CallNotifyInitDefault(notify_info, call_id, call_info->sys_mode, event);

  (void) WSTRLCPY(notify_info->event_data.call.alpha, core_call_info->alpha,
           sizeof(notify_info->event_data.call.alpha)/sizeof(AECHAR));// Gemsea sizeof IN AECHAR

  notify_info->event_data.call.disp_type = AEECM_DISP_NORMAL;

  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;

  PARAM_NOT_REF(event);
  MSG_MED("OEMCM_HandleCallExtDisplay received event 0x%X", event, 0, 0);

  if (call_info->mode_info.info_type != CM_CALL_MODE_INFO_CDMA) {
    MSG_ERROR("CM_CALL_EVENT_EXT_DISP received with wrong mode %d", 
                                               call_info->mode_info.info_type,
                                               0, 0);
    return;
  }

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  call_id = (call_info->call_id >= OEMCM_MAX_NUM_CALLS) ? 0 : call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);  

  OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf, 
                            call_info->alpha.len,
                            core_call_info->alpha, 
                            sizeof(core_call_info->alpha));

  OEMCM_CallNotifyInitDefault(notify_info, call_id, call_info->sys_mode,
                              AEECM_EVENT_CALL_DISPLAY);

  OEMCM_CopyNumToAEECMNum(&call_info->num, &notify_info->event_data.call.number);

#ifdef FEATURE_IS95B_EXT_DISP
  { /* extract subrecords. For each extracted subrecord send events. */
     cm_ext_disp_itr_type disp_itr;
     cm_ext_disp_subrecord disp_subrec;

     for (cm_ext_disp_itr_reset(&disp_itr, &call_info->alpha);
          cm_ext_disp_itr_more(&disp_itr);
          cm_ext_disp_itr_next(&disp_itr)) {

        ZEROAT(&disp_subrec);
        cm_ext_disp_itr_get(&disp_itr, &disp_subrec);

        if (AEET_DISP_BLANK == disp_subrec.display_tag
            || AEET_DISP_SKIP == disp_subrec.display_tag) {
           continue;
        }
        notify_info->event_data.call.disp_type 
           = (AEECMDispType)call_info->mode_info.info.cdma_call.ext_disp_type;

        OEMCM_CopyCharLenToAECHAR((char *)&disp_subrec.chari, 
                                  disp_subrec.display_len,
                                  notify_info->event_data.call.alpha,
                                  sizeof(notify_info->event_data.call.alpha));

        OEMTelephone_Notify(notify_info);
     }
  }
#else

  (void) WSTRLCPY(notify_info->event_data.call.alpha, core_call_info->alpha,
           sizeof(notify_info->event_data.call.alpha)/sizeof(AECHAR));// Gemsea sizeof IN AECHAR

  notify_info->event_data.call.disp_type = (AEECMDispType) 
                             call_info->mode_info.info.cdma_call.ext_disp_type;

  OEMTelephone_Notify(notify_info);
#endif /* FEATURE_IS95B_EXT_DISP */
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;
#if defined (FEATURE_ACM)
  ACM_MobileOriginatedCallInfo * ACM_MOCallInfo;
#endif // FEATURE_ACM

  MSG_MED("OEMCM_HandleCallOrig received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);  

  /* Update core info */

  OEMCM_ResetPrivateCallState(call_id);

  (void) MEMSET(core_call_info, 0, sizeof(AEETCallInfo));

  core_call_info->call_state = AEECM_CALL_STATE_ORIG;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);
  core_call_info->direction = AEECM_CALL_DIRECTION_MO;
  
  OEMCM_CopyNumToOtherParty(&(call_info->num), core_call_info);

  OEMTelephone_CopyCharLen((char *)call_info->num.subaddr.chari, 
                             call_info->num.subaddr.num_char,
                             core_call_info->other_party_subaddr, 
                             sizeof(core_call_info->other_party_subaddr)); 

  OEMCM_CopyCharLenToAECHAR((char *)call_info->alpha.buf, 
                            call_info->alpha.len,
                            core_call_info->alpha, 
                            sizeof(core_call_info->alpha));
  
  ICMCoreObj->m_call_mode_type[call_id] = call_info->mode_info.info_type;

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
    core_call_info->srv_opt = 
            (AEECMServiceOption)call_info->mode_info.info.cdma_call.srv_opt;
  }

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  (void) WSTRLCPY(notify_info->event_data.call.alpha, core_call_info->alpha,
           sizeof(notify_info->event_data.call.alpha)/sizeof(AECHAR));// Gemsea sizeof IN AECHAR

  OEMCM_CopyNumToAEECMNum(&call_info->num, 
                          &notify_info->event_data.call.number);
#if defined(FEATURE_ACM)
    /* Fill in data required for acquire & delayed acquire for Mobile Originated voice calls */
    if (CM_CALL_TYPE_VOICE == call_info->call_type ||
        CM_CALL_TYPE_EMERGENCY == call_info->call_type)
    {
      ACM_MOCallInfo = (ACM_MobileOriginatedCallInfo *) OEMCM_MALLOC (sizeof(ACM_MobileOriginatedCallInfo));

      ACM_MOCallInfo->notify_info = notify_info;
      ACM_MOCallInfo->call_id     = call_id;

      ICMCoreObj->m_pAcmInfo->m_pCBInfo = ACM_MOCallInfo;

      MSG_HIGH ("OEMCM: Check with ACM for MO Voice Call resources", 0, 0, 0);
      /* Acquire resource from Application Concurrency Manager */
      ACMInfo_Acquire(ICMCoreObj->m_pAcmInfo, ACM_MOBILE_ORIGINATED_VOICE_CALL);
    }
    else
    {
  OEMTelephone_Notify(notify_info);
  OEMCM_FREE(notify_info);
}
#else // !FEATURE_ACM
  OEMTelephone_Notify(notify_info);
  OEMCM_FREE(notify_info);
#endif // FEATURE_ACM
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallAnswer received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
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
    if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_GW_CS &&
        call_info->mode_info.info.gw_cs_call.accept) {

      cm_connected_subaddress_s_type *subaddr;
      subaddr = &call_info->mode_info.info.gw_cs_call.connected_subaddress;
      if (subaddr->present) {
        OEMTelephone_CopyCharLen((char *)subaddr->data, subaddr->length,
                  core_call_info->other_party_subaddr,
                  sizeof(core_call_info->other_party_subaddr));
      }

    }
 #endif

  }

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  OEMCM_CopyNumToAEECMNum(&call_info->num, 
                          &notify_info->event_data.call.number);

  OEMTelephone_Notify(notify_info);
  OEMCM_FREE(notify_info);
}


/*=============================================================================
FUNCTION: OEMTelephone_AMSS_To_AEETEndStatus

DESCRIPTION:  
  Conver AMSS cm_call_end_e_type to AEETEndStatus type

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEETEndStatus OEMTelephone_AMSS_To_AEETEndStatus(cm_call_end_e_type endStatus)
{
   AEETEndStatus status = AEET_CALL_END_NONE;

   if(AEET_CALL_END_NONE <= endStatus && AEET_CALL_END_HDR_CS_TIMEOUT >= endStatus ){
      status = (AEETEndStatus)endStatus;
   }
#ifdef CM_API_OTASP_SPC_ERR
   else if(CM_CALL_END_OTASP_SPC_ERR == endStatus){
      status = AEET_CALL_END_OTASP_SPC_ERR;
   }
#endif

   return status;
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;
  boolean           missed = FALSE;

  PARAM_NOT_REF(event);
  MSG_MED("OEMCM_HandleCallEnd received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  /* Update core call info */

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);  
  OEMCM_ResetPrivateCallState(call_id);

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
//      OEMCM_AddCallHistory(core_call_info);
      break;

    default:
      break;
  }

  if (!AEECM_IS_CALLSTATE_CONNECTED(core_call_info->call_state) &&
       core_call_info->direction == AEECM_CALL_DIRECTION_MT)
    missed = TRUE;

  core_call_info->call_state = AEECM_CALL_STATE_ENDED;

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode,
                              AEECM_EVENT_CALL_END);

  notify_info->event_data.call.missed = missed;
  notify_info->event_data.call.end_status = 
                              OEMTelephone_AMSS_To_AEETEndStatus(call_info->end_status);

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
  
    notify_info->event_data.call.end_additional_info = (AEECMEndCause)
                   (((uint8)call_info->mode_info.info.cdma_call.end_reason) |
                            OEMCM_CDMA_END_VALUE);
  } else {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_GW_CS) {
    
      if (call_info->mode_info.info.gw_cs_call.cc_cause.present) {
      
        if(call_info->mode_info.info.gw_cs_call.cc_cause.cause_value != NORMAL_CALL_CLEARING){
          notify_info->event_data.call.end_additional_info = (AEECMEndCause)
          (OEMCM_GSM_END_VALUE | 
          ((uint8)call_info->mode_info.info.gw_cs_call.cc_cause.cause_value));

        }
      } else {
        if (call_info->mode_info.info.gw_cs_call.cc_reject.present) {
          if(call_info->mode_info.info.gw_cs_call.cc_reject.rej_type != MM_REJECT_CAUSE) {
            notify_info->event_data.call.end_additional_info = (AEECMEndCause)
            (OEMCM_GSM_REJ_VALUE |
            ((uint8)call_info->mode_info.info.gw_cs_call.cc_reject.rej_value));
          }
        }
      }
    }
#endif
  }

#if defined (FEATURE_ACM)
  if ((AEECM_CALL_STATE_ENDED == ICMCoreObj->m_call_info[call_id].call_state) &&
      AEECM_CALL_TYPE_VOICE == ICMCoreObj->m_call_info[call_id].call_type ||
      AEECM_CALL_TYPE_EMERGENCY == ICMCoreObj->m_call_info[call_id].call_type)
  {
    MSG_HIGH("OEMCM: Call ended.  Check if we need to free ACM resources", 0, 0, 0);
    ACMInfo_Free(ICMCoreObj->m_pAcmInfo);
  }
#endif // FEATURE_ACM

  OEMTelephone_Notify(notify_info);
  OEMCM_FREE(notify_info);
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;
  boolean           bSignalled = FALSE;

  MSG_MED("OEMCM_HandleCallIncom received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
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

  (void) MEMSET(core_call_info, 0, sizeof(AEETCallInfo));

  core_call_info->call_state = AEECM_CALL_STATE_INCOM;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);
  core_call_info->direction = AEECM_CALL_DIRECTION_MT;
  core_call_info->start_time = GETUPTIMEMS();
                                                
  ICMCoreObj->m_call_mode_type[call_id] = call_info->mode_info.info_type;
                                                
  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
    if(call_info->num.pi != CAI_ALLOW_PI)       
    {
      call_info->num.len = 0;
    }   
#endif 
  }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  if (call_info->mode_info.info.gw_cs_call.cause_of_no_cli.present) {
    core_call_info->cause_of_no_cli = (AEECMNoCLICause)
            call_info->mode_info.info.gw_cs_call.cause_of_no_cli.cause_value;
  }
#endif 
                 
  OEMCM_CopyNumToOtherParty(&(call_info->num), core_call_info);

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
    
      OEMTelephone_CopyCharLen((char *)call_info->num.subaddr.chari, 
                             call_info->num.subaddr.num_char,
                             core_call_info->other_party_subaddr, 
                             sizeof(core_call_info->other_party_subaddr)); 

    core_call_info->srv_opt = 
            (AEECMServiceOption)call_info->mode_info.info.cdma_call.srv_opt;
    core_call_info->alert_ans_bypass = 
            call_info->mode_info.info.cdma_call.alert_ans_bypass;
  } 

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
    (void) MEMCPY(&notify_info->event_data.call.signal, &call_info->signal, 
           sizeof(AEECMSignal));
  }     

  OEMCM_CopyNumToAEECMNum(&call_info->num, 
                          &notify_info->event_data.call.number);

  if (call_info->signal.is_signal_info_avail && !bSignalled)
    OEMCM_HandleCallSignal(call_info, AEECM_EVENT_CALL_SIGNAL);

  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallIncomFwd received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);  

  /* Update core call info */

  OEMCM_ResetPrivateCallState(call_id);

  (void) MEMSET(core_call_info, 0, sizeof(AEETCallInfo));

  core_call_info->call_state = AEECM_CALL_STATE_INCOM;
  core_call_info->call_type = OEMCM_CallTypeToAEECM(call_info->call_type);
                                                
  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallConnect received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
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

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallConnectedNum received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  /* Update core call info */

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);  

  if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_GW_CS) {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    cm_connected_no_s_type *          connected_num;
    cm_connected_subaddress_s_type *  subaddr;

    /* Copy connected num */
    connected_num = &call_info->mode_info.info.gw_cs_call.connected_num;
    if (connected_num->present)
      OEMCM_CopyNumToOtherParty(&(call_info->num), core_call_info);

    /* Copy connected sub address */
    subaddr = &call_info->mode_info.info.gw_cs_call.connected_subaddress;
    if (subaddr->present)
      OEMTelephone_CopyCharLen(
                           (char *)subaddr->data, 
                           subaddr->length,
                           core_call_info->other_party_subaddr, 
                           sizeof(core_call_info->other_party_subaddr));
#endif
  } else {

    if (call_info->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {

      /* Copy connected num */
      if (call_info->num.number_type == CM_NUM_TYPE_INTERNATIONAL
          && call_info->num.buf[0] != '+') {
        core_call_info->other_party_no[0] = '+';
        OEMTelephone_CopyCharLen((char *)call_info->num.buf, 
                                 call_info->num.len,
                                 core_call_info->other_party_no+1, 
                                 sizeof(core_call_info->other_party_no)-2); 
      } else {
        OEMTelephone_CopyCharLen(
                           (char *)call_info->num.buf, 
                           call_info->num.len,
                           core_call_info->other_party_no, 
                           sizeof(core_call_info->other_party_no));
      }

    }
  }

  /* Update Notify info */

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  OEMCM_CopyNumToAEECMNum(&call_info->num, 
                          &notify_info->event_data.call.number);

  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallSupsStateChange received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
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

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallDormant received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  call_id = call_info->call_id;
  core_call_info = &(ICMCoreObj->m_call_info[call_id]);  

  if (event == AEECM_EVENT_CALL_ENTER_DORMANT) {
    core_call_info->call_state = AEECM_CALL_STATE_DORMANT;
  } else {
    core_call_info->call_state = AEECM_CALL_STATE_CONV;
  }

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type call_id;

  MSG_MED("OEMCM_HandleCallSignal received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  // If this event is out of order then the call ID is not yet assinged
  call_id = call_info->call_id;

  if (CM_CALL_ID_UNASSIGNED == call_id) {
    AEECMCallID  callIDs[OEMCM_MAX_NUM_CALLS];
    AEECMCallType call_type;
    int num;

#ifdef FEATURE_OTASP
    call_type = (AEECMCallType)( AEECM_CALL_TYPE_VOICE 
                | AEECM_CALL_TYPE_EMERGENCY 
                | AEECM_CALL_TYPE_STD_OTASP 
                | AEECM_CALL_TYPE_NON_STD_OTASP);
#else
    call_type = (AEECMCallType)( AEECM_CALL_TYPE_VOICE 
                | AEECM_CALL_TYPE_EMERGENCY);
#endif

    // Find the appropriate voice call for this event
    num = OEMCM_GetActiveCallIDs(gpTelephone->pcm, (AEECMCallType)call_type,
                             AEECM_CALL_STATE_NONE,
                             callIDs, sizeof(callIDs));
    if (num < 1) {
       return;
    }

    // Use the first active call as the recipient of this event
    call_id = callIDs[0];
  }

  OEMCM_CallNotifyInitDefault(notify_info, call_id, call_info->sys_mode, event);

  OEMCM_CopyInSignal(&notify_info->event_data.call.signal, &call_info->signal);

  ICMCoreObj->m_state[call_id].bSignalled = TRUE;
  
  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  cm_call_id_type       call_id;
  AEETCallInfo *   core_call_info;

  MSG_MED("OEMCM_HandleCallSrvOpt received event 0x%X", event, 0, 0);

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
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

  OEMCM_CallNotifyInitDefault(notify_info, call_info->call_id, call_info->sys_mode, event);

  OEMTelephone_Notify(notify_info);
  OEMCM_FREE(notify_info);
}

/*=============================================================================
FUNCTION: OEMCM_HandleUserDataInd

DESCRIPTION:  
  Copy user data and notify application.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
#ifdef FEATURE_UUS
static void OEMCM_HandleUserDataInd(cm_mm_call_info_s_type *call_info, 
                                    AEECMEvent event)
{
  AEETNotifyInfo *               notify_info;
  cm_call_event_user_data_s_type user_data;

  MSG_MED("OEMCM_HandleUserDataInd received event 0x%X", event, 0, 0);

  if (call_info->mode_info.info_type != CM_CALL_MODE_INFO_GW_CS) {
    MSG_ERROR("User event (0x%X) does not contain GW CS info", event, 0, 0);
    return;
  }

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  notify_info->event = event;

  notify_info->event_data.user.cd = (AEECMCallID)call_info->call_id+1;

  (void) MEMCPY(&(notify_info->event_data.user.call_info), 
           &(ICMCoreObj->m_call_info[call_info->call_id]),
           sizeof(AEETCallInfo));

  notify_info->event_data.user.call_info.sys_mode = OEMT_GetSystemMode();

  if (call_info->mode_info.info.gw_cs_call.user_data_func) {
    call_info->mode_info.info.gw_cs_call.user_data_func(&user_data, call_info);

    notify_info->event_data.user.user_data.type = user_data.mt_user_data.user_user_type;
    notify_info->event_data.user.user_data.length = user_data.mt_user_data.user_user_data_length;
    if (notify_info->event_data.user.user_data.length > AEET_MAX_USER_DATA_LENGTH) {
       notify_info->event_data.user.user_data.length = AEET_MAX_USER_DATA_LENGTH;
    }
    (void) MEMCPY(notify_info->event_data.user.user_data.user_data, 
                  user_data.mt_user_data.user_user_data,
                  notify_info->event_data.user.user_data.length);
  }

  OEMTelephone_Notify(notify_info);
  OEMCM_FREE(notify_info);
}
#endif

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
                                const char *digits,
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
  
  dtmf_pkt->dtmfs = STRDUP(digits);
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
    if (OEMCM_StartContDTMF_Int(bdtmf->pOwner, bdtmf->call_id, *bdtmf->curr) != 
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
static AEECMEventCBS * OEMCM_AllocEventCBS(ICMCore *pMe, AEECMEventCBType cbType)
{
  AEECMEventCBS *pcb = NULL;
  uint32        dwSize;

  if (!pMe)
    return NULL;

  switch( cbType ) {
  case CB_TYPE_INBAND_INFO:
     dwSize = sizeof(cm_inband_info_s_type);
     break;
  case CB_TYPE_SS_INFO:
     dwSize = sizeof(cm_mm_ss_info_s_type);
     break;
  case CB_TYPE_PH_INFO:
     dwSize = sizeof(cm_ph_info_s_type);
     break;
  case CB_TYPE_CALL_INFO:
     dwSize = sizeof(cm_mm_call_info_s_type);
     break;
  case CB_TYPE_ERRPKT:
     dwSize = sizeof(AEECMCallErrCBPkt);
     break;
  case CB_TYPE_NONE:
  default:
     dwSize = 0;
  }

  pcb = (AEECMEventCBS *) sys_malloc(sizeof(AEECMEventCBS)+dwSize);
  if (!pcb)
    return NULL;

  (void) MEMSET(pcb, 0, (sizeof(AEECMEventCBS)+dwSize));

  if (dwSize) {
    pcb->data = pcb + 1;
  }

  pcb->nRef = 1;

  AEECM_MUTEX_LOCK();

  OEMCMCore_AddRef();

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
    return;

  CALLBACK_Cancel(&pcb->cb);

  AEECM_MUTEX_LOCK();

  OEMCMCore_Release();
  
  AEECM_MUTEX_UNLOCK();  

  sys_free(pcb);
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

    ICMCoreObj->m_ss_info.tssinfo_version = AEETSSINFO_VERSION;
    OEMCM_InitDefaultCallParams(ICMCoreObj);

    ICMCoreObj->m_event_cbs = NULL;
    
    ICMCoreObj->bSSInfoValid = FALSE;
    ICMCoreObj->bInitSS = FALSE;

    if(!OEMCM_InitCallManager()) {
      OEMCM_FREE(ICMCoreObj);
      ICMCoreObj = NULL;
      return NULL;
    }

    for (i = 0; i < OEMCM_MAX_NUM_CALLS; i++)
      (void) cm_mm_call_cmd_get_call_info(NULL, NULL, gCMID, i); 

    (void) cm_ph_cmd_get_ph_info(NULL, NULL, gCMID);

    (void) cm_ss_cmd_get_ss_info(NULL, NULL, gCMID);    
    
#if defined (FEATURE_ACM)
    {
      ICMCoreObj->m_pAcmInfo = (ACMInfo *) OEMCM_MALLOC (sizeof (ACMInfo));
      ACMInfo_CreateInstance(ICMCoreObj->m_pAcmInfo);
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
    if (cm_client_init(
#if defined(CM_API_CLIENT_TYPE_OEMTELEPHONE)
        CM_CLIENT_TYPE_OEMTELEPHONE,
#else
        CM_CLIENT_TYPE_FLOATING1,
#endif /* #if defined(CM_API_CLIENT_TYPE_OEMTELEPHONE) */
        &gCMID) != CM_CLIENT_OK 
        || !gCMID) {
      ERR("Unable to Initialize DMSS Call Manager", 0, 0, 0);   
      return FALSE;
    }

#ifdef PLATFORM_LTK
    IMStartupSync();
#endif

    /* Register callbacks */
    (void) cm_mm_client_call_reg(gCMID, OEMCM_DMSSCallEventCB, CM_CLIENT_EVENT_REG,
                                 CM_CALL_EVENT_ORIG, CM_CALL_EVENT_MAX-1, 
                                 OEMCM_DMSSOtherCallErrCB);
    (void) cm_client_ph_reg(gCMID, OEMCM_DMSSPhEventCB, CM_CLIENT_EVENT_REG, 
                            CM_PH_EVENT_OPRT_MODE, CM_PH_EVENT_MAX-1,
                            NULL);
    (void) cm_mm_client_ss_reg(gCMID, OEMCM_DMSSSSEventCB, CM_CLIENT_EVENT_REG, 
                               CM_SS_EVENT_SRV_CHANGED, CM_SS_EVENT_MAX-1,
                               NULL);
#ifdef FEATURE_APPS_POWER_COLLAPSE
   (void) cm_mm_client_ss_pwr_collapse_reg(gCMID, OEMCM_DMSSSSEventCB,           
                                       CM_PROCESSOR_APPS_1,                       
                                       CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT,  
                                       CM_CLIENT_EVENT_REG,                       
                                       CM_SS_EVENT_SRV_CHANGED,                   
                                       CM_SS_EVENT_MAX-1);                        
#endif
    (void) cm_mm_client_inband_reg(gCMID, OEMCM_DMSSInbandEventCB, CM_CLIENT_EVENT_REG, 
                                   CM_INBAND_EVENT_REV_BURST_DTMF, CM_INBAND_EVENT_MAX-1, 
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
=============================================================================*/
static __inline
boolean cm_compat_call_cmd_sups(cm_call_cmd_cb_f_type cmd_cb_func, 
                                void* data_block_ptr, 
                                cm_client_id_type client_id, 
                                cm_call_sups_type_e_type sups_type,
                                cm_call_sups_params_s_type* sups_params_ptr)
{
   /* SUPS number needs to be at least initialized */
   if (CM_CALL_SUPS_TYPE_CALL_DEFLECTION != sups_type
       && CM_CALL_SUPS_TYPE_SIMPLE_FLASH != sups_type) {
      cm_num_init(&sups_params_ptr->sups_number);
   }

   /* SUPS call commands need to provide valid call id,
   ** ie. call id of an existing call */
   if (CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL != sups_type
       && CM_CALL_SUPS_TYPE_SELECT_CALL_X != sups_type
       && CM_CALL_SUPS_TYPE_SIMPLE_FLASH != sups_type) {
      cm_call_id_type call_id;
      for (call_id = 0; call_id < OEMCM_MAX_NUM_CALLS; call_id++) {
         if (TRUE == ICMCoreObj->m_state[call_id].bValid
             && AEET_CALL_STATE_ENDED != ICMCoreObj->m_call_info[call_id].call_state
             && OEMTelephone_IsCallTypeVoice(ICMCoreObj->m_call_info[call_id].call_type)) {
            sups_params_ptr->call_id = call_id;
         }
      }
   }

   if (CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL == sups_type) {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
      sups_params_ptr->cause = CM_CAUSE_DEFAULT;
#endif
   }

   return cm_mm_call_cmd_sups(cmd_cb_func, data_block_ptr, client_id,
                              sups_type, sups_params_ptr);
} /* cm_compat_call_cmd_sups() */

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

      case CM_CALL_CMD_ORIG:
        ICMCoreObj->m_state[call_id].bValid = FALSE;
        break;

      default:
        break;
    }
  }
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
  
  int nErr = OEMCM_InitNum(&called_number, called);
  if (SUCCESS != nErr)
  {
    MSG_ERROR("OEMCM_InitNum Failed",
               0, 0, 0);
    return nErr;
  }
  
  /* Allocate event callback structure */
  event_cb = OEMCM_AllocEventCBS(ICMCoreObj, CB_TYPE_ERRPKT);

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

  if (!cm_compat_call_cmd_sups(OEMCM_DMSSCallErrCB, event_cb, gCMID, 
                           CM_CALL_SUPS_TYPE_SIMPLE_FLASH,
                           &sups_params)) {
    MSG_ERROR("Call to cm_compat_call_cmd_sups failed", 0, 0, 0);
    OEMCM_FreeEventCBS( event_cb );
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
  cm_gw_cs_orig_params_s_type *gw;
  cm_bearer_capability_s_type default_bearer_capability =
  {
     TRUE,                  /* validity */
  
     /* octet 3 */
  #ifdef FEATURE_GSM_HALF_RATE
     DUAL_RATE_FULL_RATE_PREFERRED,
  #else
     FULL_RATE_ONLY,        /* radio channel requirement : full rate pref */
  #endif /* FEATURE_GSM_HALF_RATE */
  
     GSM,                   /* coding_standard : GSM */
     CIRCUIT,               /* transfert mode : circuit mode */
     ITC_SPEECH,            /* information transfer capability : speech */

#ifdef FEATURE_TTY
     CTM_NOT_SUPPORTED,     /* CTM bit to indicate TTY support */
#endif /* FEATURE_TTY */
  
     /*  support for 3 speech versions: EFR, FR and AMR  */
  #ifdef FEATURE_GSM_AMR
     /* octet 3a */
     GSM_FR_SPEECH_VER_3,  /* GSM AMR Full Rate */
  
     /* octet 3c */
     GSM_FR_SPEECH_VER_2,  /* GSM Enhanced Full Rate  */
  
     /* octet 3b */
     GSM_FR_SPEECH_VER_1,  /* GSM FR */
  #else  
     /* octet 3a */
     GSM_FR_SPEECH_VER_2,
  
     /* octet 3b */
     GSM_FR_SPEECH_VER_1,
  
     /* octet 3c */
  #ifdef FEATURE_GSM_HALF_RATE
     GSM_HR_SPEECH_VER_1, /* GSM Half Rate Version 0 or 1 */
  #else
     GSM_INVALID_SPEECH_VER,
  #endif /* FEATURE_GSM_HALF_RATE */
  #endif /* FEATURE_GSM_AMR */
  
  #ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
  #ifdef FEATURE_GSM_AMR
     /* octet 3d */
     GSM_HR_SPEECH_VER_3,
  
     /* octet 3e */
     GSM_HR_SPEECH_VER_1,
  #else
     /* octet 3d */
     GSM_INVALID_SPEECH_VER,
  
     /* octet 3e */
     GSM_INVALID_SPEECH_VER,
  #endif /* FEATURE_GSM_AMR */
  #endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
  
     /* octet 4 */
     0x00,                  /* compression */
     0x00,                  /* structure */
     0x01,                  /* duplex mode : full duplex */
     0x00,                  /* configuration */
     0x00,                  /* NIRR */
     0x00,                  /* establishment */
  
     /* octet 5 */
     0x00,                  /* access id */
     0x00,                  /* rate adaption : no rate adaption */
     0x00,                  /* signalling access control */
  
     /* octet 5a */
     0x00,                  /* other_ITC */
     0x00,                  /* other_rate_adaption */
  
     /* octet 5b */
     0x00,                  /* rate_adaption_header */
     0x00,                  /* multiple_frame_establishment */
     0x00,                  /* mode_of_operation */
     0x00,                  /* logical_link_identifier_negotiation */
     0x00,                  /* assignor_assignee */
     0x00,                  /* inband_outband_negotiation */
  
     /* octet 6 */
     0x01,                  /* layer1 id */
     0x00,                  /* user info layer 1 protocol */
     0x00,                  /* sync - async */
  
     /* octet 6a */
     0x00,                  /* number of stop bits */
     0x00,                  /* negotiation */
     0x00,                  /* number of data bits */
     0x00,                  /* user rate */
  
     /* octet 6b */
     0x00,                  /* intermediate rate */
     0x00,                  /* NIC on Tx */
     0x00,                  /* NIC on Rx */
     0x00,                  /* parity */
  
     /* octet 6c */
     0x00,                  /* connection element */
     0x00,                  /* modem type */
  
     /* These values are for GSM Phase2+/WCDMA */
     /* octet 6d */
     OMT_UNDEFINED,         /* Other Modem type */
     FNUR_UNDEFINED,        /* Fixed Network User Rate */
  
     /* octet 6e */
     ACC_NONE,                    /* Acceptable Channel Codings */
     MAX_NUMBER_OF_TCH_UNDEFINED, /* Max Number of TCHs */
  
     /* octet 6f */
     UIMI_UNDEFINED,              /* User Initiated Modifiaction Indication */
     WAIUR_UNDEFINED,             /* Wanted Air User Rate */
  
     /* octet 6g */
     0x00,                        /* acceptable_channel_coding_extended */
     0x00,                        /* asymmetry_indication - EDGE channel coding */
  
     /* octet 7 */
     0x02,                        /* layer 2 ID */
     0x00,                        /* user info layer 2 protocol */
  
     /* For logging purpose */
     0                            /* MT fall-back rate */
  };
#endif

  if (pMe) {
    cdma = &(pMe->def_cdma_orig_params);
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    gw = &(pMe->def_gw_orig_params);
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
  gw->bc_repeat_indicator.present = FALSE;
  gw->bc_repeat_indicator.repeat_indication = 0;
  gw->bearer_capability_1 = default_bearer_capability;
  gw->bearer_capability_2.present = FALSE;
  gw->llc_repeat_indicator.present = FALSE;
  gw->llc_repeat_indicator.repeat_indication = 0;
  gw->low_layer_compatibility_1.present = FALSE;
  gw->low_layer_compatibility_2.present = FALSE;
  gw->hlc_repeat_indicator.present = FALSE;
  gw->hlc_repeat_indicator.repeat_indication = 0;
  gw->high_layer_compatibility_1.present = FALSE;
  gw->high_layer_compatibility_1.not_applicable = TRUE;
  gw->high_layer_compatibility_2.present = FALSE;
  gw->high_layer_compatibility_2.not_applicable = TRUE;
  gw->called_party_bcd_number.present = FALSE;
  gw->called_party_subaddress.present = FALSE;
  gw->calling_party_subaddress.present = FALSE;
  gw->forward_cug_info.present = FALSE;
  gw->cm_clir = 0;
#ifdef FEATURE_GSTK
  gw->call_control_completed = FALSE;
#endif
#endif
}


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

  if((p = sys_malloc(size)) == NULL) {
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

#ifdef FEATURE_ICM_DEBUG
    if(ICMCoreObj && ((void *)(ICMCoreObj) != p))
      ICMCoreObj->m_malloc_cnt--;
#endif
  }
}


/*=============================================================================
FUNCTION: OEMTelephone_SysModeToAEET

DESCRIPTION:  
   Map system mode SYS_SYS_MODE_XXX to AEET_SYS_MODE_XXX 


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEETSysMode OEMTelephone_SysModeToAEET(sys_sys_mode_e_type sys_mod)
{
   AEETSysMode mode = AEET_SYS_MODE_NONE;
   
   switch(sys_mod)
   {       
      case SYS_SYS_MODE_NO_SRV:
         mode = AEET_SYS_MODE_NO_SRV;
         break;
      case SYS_SYS_MODE_AMPS:
         mode = AEET_SYS_MODE_AMPS;
         break;
      case SYS_SYS_MODE_CDMA:
         mode = AEET_SYS_MODE_CDMA;
         break;
      case SYS_SYS_MODE_GSM:
         mode = AEET_SYS_MODE_GSM;
         break;
      case SYS_SYS_MODE_HDR:
         mode = AEET_SYS_MODE_HDR;
         break;
      case SYS_SYS_MODE_WCDMA:
         mode = AEET_SYS_MODE_WCDMA;
         break;
      case SYS_SYS_MODE_GPS:
         mode = AEET_SYS_MODE_GPS;
         break;
      case SYS_SYS_MODE_GW:
         mode = AEET_SYS_MODE_GW;
         break;   
#ifdef  CM_API_WLAN
      case SYS_SYS_MODE_WLAN:
         mode = AEET_SYS_MODE_WLAN;
         break;
#endif        
      default:
         MSG_ERROR("OEMTelephone_SysModeToAEET: No AEET mode for system mode : %d",
                                                      sys_mod, 0, 0);
         mode = (AEETSysMode)sys_mod;
   }
   
   return mode;
}


/*=============================================================================
FUNCTION: OEMT_GetSystemMode

DESCRIPTION:  


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEETSysMode OEMT_GetSystemMode()
{
  return OEMTelephone_SysModeToAEET(cm_get_current_acq_sys_mode());
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
  if(NULL == ICMCoreObj || call_id >= OEMCM_MAX_NUM_CALLS){
    return FALSE;
  }
  
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
static AEETPhInfo * OEMCM_GetValidPhInfo(ICM *pMe)
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
  AEETCallInfo * call_info;

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
static int OEMCM_InitNum(cm_num_s_type *num, const char *number)
{ 
   char buf[CM_MAX_NUMBER_CHARS+1];
   char *c;
  
   if (!num || !number) {
      return EBADPARM;
   }

   if (*number == '+') {
      num->number_type = (uint8)CM_NUM_TYPE_INTERNATIONAL;
   } 
   else {
      num->number_type = (uint8)CM_NUM_TYPE_UNKNOWN;
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
  
   (void) aee_strlcpy((char *) num->buf, buf, sizeof(num->buf));

   num->len = (byte)aee_strlen((char *) num->buf);

   /* Make sure digit mode is set correctly depending on the number */
   num->digit_mode = (0 == aee_memchr(num->buf, '+', num->len)) ? 
      CM_DIGIT_MODE_4BIT_DTMF : CM_DIGIT_MODE_8BIT_ASCII;

   num->number_plan = (uint8) CM_NUM_PLAN_ISDN;
   num->pi = 0;
   num->si = 0;
   num->subaddr.extension_bit = 0;
   num->subaddr.subadd_type = 0;
   num->subaddr.odd_even_ind = 0;
   num->subaddr.num_char = 0;

   return SUCCESS;
}

/*=============================================================================
FUNCTION: OEMCM_InitUserData

DESCRIPTION:  


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
#ifdef FEATURE_UUS
static void OEMCM_InitUserData(cm_user_data_s_type *user_data, const AEETUserDataInfo *in_data)
{ 
   ZEROAT(&user_data->user_user_data);

   user_data->present = TRUE;
   user_data->more_data_indicator = FALSE;   
   if ((in_data->type >= UUS1_IMPLICIT) && (in_data->type <= UUS1_NOT_REQUIRED)) {
      user_data->user_user_type = (uus_type_T) in_data->type;
   } else {
      user_data->user_user_type = UUS1_IMPLICIT;
   }
   user_data->user_user_protocol_disc = 0;
   if (in_data->length > MAX_USER_USER_DATA_1_LENGTH) {
      user_data->user_user_data_length = MAX_USER_USER_DATA_1_LENGTH;
   } else {
      user_data->user_user_data_length = (byte)in_data->length;
   }
   MEMCPY(&user_data->user_user_data, in_data->user_data, user_data->user_user_data_length);
}
#endif

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

  if (tmp) {
    (void) MEMCPY(tmp, cmchar, len);
    tmp[len] = '\0';

    (void) STRTOWSTR(tmp, aeechar, size);

    OEMCM_FREE(tmp);
  }
}

static void OEMTelephone_CopyCharLen(char *cmchar, uint8 len, 
                                      char *dest, uint16 size)
{

  len = MIN((size-1),len);

  MEMCPY(dest,cmchar,len);
  dest[len] = '\0';
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
                                      AEETCallInfo *core_call_info)
{
  if (!num || !core_call_info)
    return;

  if (num->len) {
    if(num->number_type == (uint8)CM_NUM_TYPE_INTERNATIONAL
       && num->buf[0] != '+') {
      core_call_info->other_party_no[0] = '+';
      OEMTelephone_CopyCharLen((char *)num->buf, num->len,
                               core_call_info->other_party_no+1, 
                               sizeof(core_call_info->other_party_no)-2); 
    } else {
      OEMTelephone_CopyCharLen((char *)num->buf, num->len,
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
static void OEMCM_CopyNumToAEECMNum(cm_num_s_type *num, AEETNumber *aee_num)
{
  OEMTelephone_CopyCharLen((char *)num->buf, num->len, aee_num->buf, 
                            sizeof(aee_num->buf));

  aee_num->number_type = (AEETNumberType)(num->number_type >> 4);
  aee_num->number_plan = (AEETNumberPlan)num->number_plan;
  aee_num->pi = (AEETPI)num->pi; /* same value */
  aee_num->si = (AEETSI)num->si; /* same value */
  
  aee_num->subaddr.extension_bit = num->subaddr.extension_bit;
  aee_num->subaddr.odd_even_ind = num->subaddr.odd_even_ind;

  OEMTelephone_CopyCharLen((char *)num->subaddr.chari, num->subaddr.num_char,
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

  ICMCoreObj->m_dtmf_start_times[call_id] = 0;
  ICMCoreObj->m_call_mode_type[call_id] = CM_CALL_MODE_INFO_NONE;
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
static void  OEMCM_CallNotifyInitDefault(AEETNotifyInfo *notify_info, 
                                         cm_call_id_type call_id,
                                         sys_sys_mode_e_type sys_mode,
                                         AEECMEvent event)
{
  notify_info->event = event;

  notify_info->event_data.call.cd = (AEECMCallID)call_id+1;

  (void) MEMCPY(&(notify_info->event_data.call.call_info), 
           &(ICMCoreObj->m_call_info[call_id]),
           sizeof(AEETCallInfo));

  if ((sys_mode > SYS_SYS_MODE_NO_SRV) && (sys_mode < SYS_SYS_MODE_MAX)) {
     notify_info->event_data.call.call_info.sys_mode =
        OEMTelephone_SysModeToAEET(sys_mode);
  }
  else {
     // if sys_mode is not valid, use primary sys_mode 
     notify_info->event_data.call.call_info.sys_mode = OEMT_GetSystemMode();
  }
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
  AEETNotifyInfo * notify_info;
  AEETPhInfo *    core_ph_info;

  if (!ICMCoreObj) {
    return;
  }

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  core_ph_info = &ICMCoreObj->m_ph_info;

  OEMTelephone_CopyInPhInfo(core_ph_info, ph_info);

  if (event == AEECM_EVENT_PH_INFO_AVAIL)
    ICMCoreObj->m_ph_valid = TRUE;

  notify_info->event = event;
  (void) MEMCPY(&notify_info->event_data.ph, core_ph_info, sizeof(AEETPhInfo));

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  notify_info->event_data.ph.aoc_info.ccm = cm_ph_cmd_aoc_get_ccm(); 
  notify_info->event_data.ph.aoc_info.acm = cm_ph_cmd_aoc_get_acm(); 
  notify_info->event_data.ph.aoc_info.acm_max = cm_ph_cmd_aoc_get_acmmax(); 
#endif
  
  OEMTelephone_Notify(notify_info);
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
  AEETNotifyInfo * notify_info;
  AEETPhInfo *   core_ph_info;

  if (!ICMCoreObj) {
    return;
  }

  if ((notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo))) == NULL) {
    return;
  }

  core_ph_info = &ICMCoreObj->m_ph_info;

  OEMTelephone_CopyInPhInfo(core_ph_info, ph_info);

  core_ph_info->test_control_type = (AEECMTestControlType)
                                    ph_info->test_control_type;

  if (event == AEECM_EVENT_PH_INFO_AVAIL)
    ICMCoreObj->m_ph_valid = TRUE;

  notify_info->event = AEECM_EVENT_PH_OPRT_MODE;

  (void) MEMCPY(&notify_info->event_data.ph, core_ph_info, sizeof(AEETPhInfo));

  OEMTelephone_Notify(notify_info);
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
  OEMTelephone_CopyInPhInfo(&ICMCoreObj->m_ph_info, ph_info);

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
  AEETCallInfo *         call_info;
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
  event_cb = OEMCM_AllocEventCBS(pMe->m_coreObj, CB_TYPE_ERRPKT);

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
    OEMCM_FreeEventCBS( event_cb );
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
static void OEMCM_CopyInCallInfo(AEETCallInfo *dest, 
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
  dest->direction = AEET_CALL_DIRECTION_NONE;

#ifdef FEATURE_ALS
  dest->line_id = src->line;
#endif /*FEATURE_ALS */


  /* TODO: Copy in other fields */
  if (src->mode_info.info_type == CM_CALL_MODE_INFO_CDMA) {
    dest->srv_opt = (AEECMServiceOption) src->mode_info.info.cdma_call.srv_opt;
    dest->alert_ans_bypass = src->mode_info.info.cdma_call.alert_ans_bypass;

    dest->privacy_pref = 
        OEMCM_PrivacyPrefToAEECM(src->mode_info.info.cdma_call.privacy_pref);
  } else {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
    if (src->mode_info.info.gw_cs_call.cause_of_no_cli.present) {
      dest->cause_of_no_cli = (AEECMNoCLICause)
                        src->mode_info.info.gw_cs_call.cause_of_no_cli.cause_value;
    }
#endif
  }
  
  OEMCM_ResetPrivateCallState(src->call_id);
}

/*=============================================================================
FUNCTION: OEMTelephone_CopyInPhInfo

DESCRIPTION:  


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMTelephone_CopyInPhInfo(AEETPhInfo *dest,
                               const cm_ph_info_s_type *src)
{
  if (!dest || !src) {
    MSG_ERROR("OEMTelephone_CopyInPhInfo received a NULL pointer", 0, 0, 0);
    return;
  }

  dest->is_in_use = src->is_in_use;
  dest->oprt_mode = (AEETOprtMode)src->oprt_mode;
  dest->cdma_lock_mode = (AEETCDMALockMode)src->cdma_lock_mode;
  dest->answer_voice = (AEETAnsVoicePref)src->answer_voice;
  dest->mode_pref = OEMTelephone_CMModePrefToAEET(src->mode_pref);
  dest->pref_term = (AEETPrefTerm)
                    (src->pref_term > CM_PREF_TERM_1_CALL_OR_TIME ? 
                                      CM_PREF_TERM_1_CALL_OR_TIME :
                                      src->pref_term);
  dest->acq_order_pref = (AEETAcqOrderPref)src->acq_order_pref;
  dest->srv_domain_pref = (AEETServDomPref)src->srv_domain_pref;
  dest->band_pref = (AEETBandPref)src->band_pref;
  dest->roam_pref = (AEETRoamPref)src->roam_pref;
  dest->nam_sel = (AEETNAM)src->nam_sel;
  dest->curr_nam = (AEETNAM)src->curr_nam;
  dest->hybr_pref = Prv_XlateHybrPref(src->hybr_pref);
  dest->network_sel_mode_pref = (AEETNwSelModePref)src->network_sel_mode_pref;
  (void) MEMCPY(&dest->plmn, &src->plmn, sizeof(AEETPLMNID));

  OEMCM_CopyInUserPrefNetworks(&dest->user_pref_networks, 
                               &src->user_pref_networks);

  OEMCM_CopyInAvailableNetworks(&dest->available_networks, 
                                &src->available_networks);

  dest->is_cdma_subscription_available = src->is_cdma_subscription_available;
  dest->is_gw_subscription_available = src->is_gw_subscription_available;
  dest->rtre_config = (AEETRTREConfig)src->rtre_config;
  dest->rtre_control = (AEETRTREControl)src->rtre_control;

#ifdef FEATURE_DDTM_CNTL
  dest->ddtm_pref = (AEETDDTMPref)src->ddtm_pref;
#endif

#ifdef FEATURE_ALS
  dest->line_info.dwMaxSupportedLines = 2;
#endif

  ICMCoreObj->m_ph_valid = TRUE;
}

/*=============================================================================
FUNCTION: OEMTelephone_CopyInSSInfo

DESCRIPTION:  


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMTelephone_CopyInSSInfo(const cm_mm_ss_info_s_type *src)
{
   AEETSSInfo *pSav = &ICMCoreObj->m_ss_info;
   if (!pSav || !src) {
      MSG_ERROR("OEMTelephone_CopyInSSInfo received a NULL pointer", 0, 0, 0);
      return;
   }

   pSav->srv_status = (AEETSrvStatus)src->srv_status;
   #if defined( FEATURE_HDR ) || defined (AEE_SIMULATOR)
   pSav->hdr_srv_status = (AEETSrvStatus)src->hdr_srv_status;
   #else
   pSav->hdr_srv_status = AEET_SRV_STATUS_NO_SRV;
   #endif
   pSav->srv_domain = (AEETSrvDomain)src->srv_domain;
   pSav->srv_capability = (AEETSrvCapability)src->srv_capability;
   pSav->sys_mode = OEMTelephone_SysModeToAEET(src->sys_mode);
   pSav->roam_status = (AEETRoamStatus)src->roam_status;
   #ifdef FEATURE_HDR
   pSav->hdr_roam_status = (AEETRoamStatus)src->hdr_roam_status;
   #else
   pSav->hdr_roam_status = AEET_ROAM_STATUS_OFF;
   #endif
   OEMTelephone_CopyInSysID(&pSav->sys_id, &src->sys_id);

   pSav->rssi = src->rssi;
   #if defined( FEATURE_HDR ) || defined (AEE_SIMULATOR)
   pSav->hdr_hybrid = src->hdr_hybrid;
   if (src->hdr_hybrid){
      pSav->hdr_rssi = src->hdr_rssi;
   } 
   else if (OEMT_GetSystemMode() == AEET_SYS_MODE_HDR) {
      pSav->hdr_rssi = src->rssi;
      pSav->hdr_roam_status = src->roam_status;
      pSav->hdr_srv_status = src->srv_status;
   }
   #endif

   if (AEET_IS_MODE_CDMA(pSav->sys_mode)) {
      pSav->band_class = (AEETSysBandClass)src->mode_info.cdma_info.band_class;
      pSav->block_or_system = (AEETSysBlocks) src->mode_info.cdma_info.block_or_system;
      pSav->is_registered = src->mode_info.cdma_info.is_registered;
      pSav->uz_id = src->mode_info.cdma_info.uz_id;
      pSav->ccs_supported = src->mode_info.cdma_info.ccs_supported;
      pSav->p_rev_in_use = src->mode_info.cdma_info.p_rev_in_use;

      (void) MEMCPY(&pSav->uz_name, &src->mode_info.cdma_info.uz_name,
                    sizeof(AEETUZName));

   } else {
      pSav->sim_state = (AEETSIMState) src->mode_info.gw_info.sim_state;
      pSav->plmn_forbidden = src->mode_info.gw_info.plmn_forbidden;

#ifdef CM_API_EMERGENCY_NUM_LIST      
      pSav->band_class = (AEETSysBandClass)src->active_band;
#endif

      OEMTelephone_CopyInMMInfo(&pSav->mm_information, 
                         &src->mode_info.gw_info.mm_information);

#ifdef CM_API_CELL_SUPP_SERVICES
      pSav->ccs_supported = (src->cell_srv_ind.dtm_supp == SYS_DTM_SUPPORT_AVAIL);
#endif
   }

   ICMCoreObj->m_ss_valid = TRUE;
}

/*=============================================================================
FUNCTION: OEMTelephone_CopyInMMInfo

DESCRIPTION:  
  Copy in Mobility Mangement Informtion

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMTelephone_CopyInMMInfo(AEETMMInfo *mmInfo, 
                               const sys_mm_information_s_type *mm_info)
{

  mmInfo->plmn_avail                = mm_info->plmn_avail;
  mmInfo->full_name_avail           = mm_info->full_name_avail;
  mmInfo->short_name_avail          = mm_info->short_name_avail;
  mmInfo->time_and_time_zone_avail  = mm_info->univ_time_and_time_zone_avail;
  mmInfo->time_zone_avail           = mm_info->time_zone_avail;
  mmInfo->lsa_identity_avail        = mm_info->lsa_identity_avail;

  if (mm_info->plmn_avail)
    (void) MEMCPY(&mmInfo->plmn, &mm_info->plmn, sizeof(AEETPLMNID));

  if (mm_info->full_name_avail)
    OEMTelephone_CopyInFullNWName(&mmInfo->full_name, &mm_info->full_name);
  
  if (mm_info->short_name_avail)
    OEMTelephone_CopyInShortNWName(&mmInfo->short_name, &mm_info->short_name);

  if (mm_info->univ_time_and_time_zone_avail) {
    JulianType julian_time;

    julian_time.wYear   = 2000+mm_info->univ_time_and_time_zone.year; /* GT 2000 */
    julian_time.wMonth  = mm_info->univ_time_and_time_zone.month;
    julian_time.wDay    = mm_info->univ_time_and_time_zone.day;
    julian_time.wHour   = mm_info->univ_time_and_time_zone.hour;
    julian_time.wMinute = mm_info->univ_time_and_time_zone.minute;
    julian_time.wSecond = mm_info->univ_time_and_time_zone.second;
    julian_time.wWeekDay = 0;

    /* Get the week day populated */
    GETJULIANDATE(JULIANTOSECONDS(&julian_time), &julian_time);
    (void) MEMCPY(&mmInfo->time_and_time_zone.time, &julian_time,
                  sizeof(JulianType));
    mmInfo->time_and_time_zone.time_zone = (AEETTimeZone)
                        mm_info->univ_time_and_time_zone.time_zone;
  }

  if (mm_info->daylight_saving_adj_avail) {
    mmInfo->daylight_saving_adj = (int8)mm_info->daylight_saving_adj;
  }
  else {
     mmInfo->daylight_saving_adj = 0;
  }

  if (mm_info->time_zone_avail)
    mmInfo->time_zone = (AEETTimeZone) mm_info->time_zone;

  if (mm_info->lsa_identity_avail) {
    mmInfo->lsa_identity.type = (AEETLSAIdentityType)
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
static void OEMTelephone_CopyInSysID(AEETSysID *dest, const sys_sys_id_s_type *src)
{
  dest->id_type = (AEETSysIDType) src->id_type;

  (void) MEMCPY(&dest->id, &src->id, sizeof(AEETSysIDData));
}

/* copy of cm_util_ussd_unpack modified to use const byte* packed_data */
uint32 prv_util_ussd_unpack(
    byte* str, 
    const byte* packed_data, 
    uint32 num_bytes
)
{
  uint32 stridx = 0;
  uint32 pckidx = 0;
  byte prev = 0;
  byte curr = 0;
  byte shift;

  while(pckidx < num_bytes)
  {
    shift = (byte)(stridx & 0x07);
    curr = packed_data[pckidx++];
    
    /* A 7-bit character can be split at the most between two bytes of packed
    ** data.
    */
    str[stridx++] = ( (curr << shift) | (prev >> (8-shift)) ) & 0x7F;
    
    /* Special case where the whole of the next 7-bit character fits inside
    ** the current byte of packed data.
    */
    if(shift == 6)
    {
      /* If the next 7-bit character is a CR (0x0D) and it is the last
      ** character, then it indicates a padding character. Drop it.
      */

      if((curr >> 1) == 0x0D && pckidx == num_bytes)
      {
        break;
      }
      str[stridx++] = curr >> 1;
    }

    prev = curr;
  }
  
  return stridx;
} /* cm_util_ussd_unpack() */

/* return number of unpacked characters */
static __inline
uint32 OEMTelephone_Unpack7bitChars(byte *pDest, uint32 uDest, 
                                    const byte *pSrc, uint32 uSrc)
{
   uint32 uRet = (uSrc * 8)/7;

   uRet = MIN(uDest, uRet);

   prv_util_ussd_unpack(pDest, pSrc, (7 * (uRet >> 3)) + (uRet & 0x07));

   return uRet;
}

static __inline
void OEMTelephone_CopyInNWName(AEETNWName *dest, 
                               const sys_network_full_name_s_type *src)
{
   dest->coding_scheme = (AEETNWCodingType)src->coding_scheme;
   dest->add_country_initials = src->add_country_initials;

   if (AEET_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET 
       == dest->coding_scheme) {
      dest->length = (byte)OEMTelephone_Unpack7bitChars(dest->name, AEET_MAX_NW_NAME_CHARS,
                                                  src->name, src->length);
   }
   else {
      unsigned octets = MIN(src->length, AEET_MAX_NW_NAME_CHARS);
      dest->length = octets >>1;
      (void) MEMCPY(dest->name, src->name, octets);
   }
}

/*=============================================================================
FUNCTION: OEMTelephone_CopyInFullNWName

DESCRIPTION:  

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMTelephone_CopyInFullNWName(AEETNWName *dest, 
                                   const sys_network_full_name_s_type *src)
{
   OEMTelephone_CopyInNWName(dest, src);
}

/*=============================================================================
FUNCTION: OEMTelephone_CopyInShortNWName

DESCRIPTION:  

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMTelephone_CopyInShortNWName(AEETNWName *dest, 
                                    const sys_network_short_name_s_type *src)
{
   OEMTelephone_CopyInNWName(dest, (const sys_network_full_name_s_type*)src);
}

/*=============================================================================
FUNCTION: OEMCM_CopyInAvailableNetworks

DESCRIPTION:  

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_CopyInAvailableNetworks(AEETDetailedPLMNList *dest, 
                                      const sys_detailed_plmn_list_s_type *src)
{
  uint32 i = 0;

  dest->length = MIN(src->length, AEET_PLMN_LIST_MAX_LENGTH);

  for ( i = 0; i < dest->length; i++) {

    (void) MEMCPY(&dest->info[i].plmn, &src->info[i].plmn, sizeof(AEETPLMNID));

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
static void OEMCM_CopyInUserPrefNetworks(AEETUserPLMNList *dest,
                                    const sys_user_pref_plmn_list_s_type *src)
{
  int i = 0;


  dest->length = (src->length > MIN(SYS_USER_PPLMN_LIST_MAX_LENGTH,
                                    AEET_USER_PPLMN_LIST_MAX_LENGTH)) ? 
                  MIN(SYS_USER_PPLMN_LIST_MAX_LENGTH,
                      AEET_USER_PPLMN_LIST_MAX_LENGTH) :
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

 
  dest->length = (src->length > MIN(AEECM_USER_PPLMN_LIST_MAX_LENGTH, 
                                    SYS_USER_PPLMN_LIST_MAX_LENGTH)) ? 
                  MIN(AEECM_USER_PPLMN_LIST_MAX_LENGTH, 
                      SYS_USER_PPLMN_LIST_MAX_LENGTH) :
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
static void OEMCM_CopyInSignal(AEETSignal *aee_signal, 
                               cm_signal_s_type *cm_signal)
{
  AEETSysMode sys_mode;
  aee_signal->is_signal_info_avail = cm_signal->is_signal_info_avail;

  if (!cm_signal->is_signal_info_avail)
    return;

  sys_mode = OEMT_GetSystemMode();
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  if (AEET_IS_MODE_GW(sys_mode)) {
    switch(cm_signal->signal_type) {
      case CM_SIGNAL_NETWORK_ALERT:
        aee_signal->signal_type = AEET_SIGNAL_GW_TONE;
        aee_signal->signal_pitch = AEET_PITCH_NONE;
        aee_signal->signal.gw_tone = (AEETGWSignalTone) cm_signal->signal;
        break;
      default:
        MSG_ERROR("Unknown GW signal type %d", cm_signal->signal_type, 0, 0);
        aee_signal->is_signal_info_avail = FALSE;
        break;
    }
  } 
#endif
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  if (AEET_IS_MODE_CDMA(sys_mode)) {
    switch(cm_signal->signal_type) {
      case CAI_SIG_TONE:
        aee_signal->signal_type = AEET_SIGNAL_CDMA_TONE;
        aee_signal->signal_pitch = AEET_PITCH_NONE;
        aee_signal->signal.cdma_tone = (AEETCDMASignalTone) cm_signal->signal;
        break;

      case CAI_SIG_ISDN:
        aee_signal->signal_type = AEET_SIGNAL_CDMA_ISDN;
        aee_signal->signal_pitch = AEET_PITCH_NONE;
        aee_signal->signal.cdma_isdn = (AEETCDMASignalISDN) cm_signal->signal;
        break;

      case CAI_SIG_IS54B:
        aee_signal->signal_type = AEET_SIGNAL_CDMA_IS54B;
        aee_signal->signal_pitch = (AEETSignalPitch) cm_signal->alert_pitch;
        aee_signal->signal.cdma_is54b = 
                       (AEETCDMASignalIS54B) cm_signal->signal;
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
FUNCTION: Prv_XlateHybrPref

DESCRIPTION:  
  Convert hybrid preference enumeration from DMSS to AEECM  

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static AEETHybrPref Prv_XlateHybrPref(cm_hybr_pref_e_type pref) 
{
  switch(pref) {
    case CM_HYBR_PREF_OFF:
      return AEET_HYBR_PREF_OFF;

    case CM_HYBR_PREF_ON:
      return AEET_HYBR_PREF_ON;

    case CM_HYBR_PREF_NO_CHANGE:
      return AEET_HYBR_PREF_NO_CHANGE;

    default:
      return AEET_HYBR_PREF_NONE;
  }
}

/*=============================================================================
FUNCTION: AEET_XlateHybrPref

DESCRIPTION:  
  Convert hybrid preference enumeration from AEECM to DMSS

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static cm_hybr_pref_e_type AEET_XlateHybrPref(AEETHybrPref pref) 
{
  switch(pref) {
    case AEET_HYBR_PREF_OFF:
      return CM_HYBR_PREF_OFF;

    case AEET_HYBR_PREF_ON:
      return CM_HYBR_PREF_ON;

    case AEET_HYBR_PREF_NO_CHANGE:
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
static AEECMPhError OEMCM_PhErrorToAEECM(cm_ph_cmd_err_e_type error)
{
  AEECMPhError err = AEECM_PH_ERROR_NONE;

  switch(error) {
    case CM_PH_CMD_ERR_NOERR:
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
    case CM_PH_CMD_ERR_LINE_SWITCHING_P:
    case CM_PH_CMD_ERR_LINE_SWITCHING_S:
    case CM_PH_CMD_ERR_SELECTED_LINE_P:
    case  CM_PH_CMD_ERR_ALS_NOT_SUPPORTED_S:
      err = AEET_PH_ERROR_LINE_SWITCHING;
      break;

    case CM_PH_CMD_ERR_ALS_SIM_WRITE_FAILED_S:
      err = AEET_PH_ERROR_ALS_SIM_WRITE_FAILED;
      break;
#endif

    case CM_PH_CMD_ERR_NO_BUF_L:
    case CM_PH_CMD_ERR_MC_COM_L:
    case CM_PH_CMD_ERR_OTHER:
#if MIN_CM_REV(3,100)
    case CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE:
#endif
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

    case CM_CALL_CMD_ERR_NO_COLLOC_HDR:
    case CM_CALL_CMD_ERR_NO_BUF_L:
    case CM_CALL_CMD_ERR_MC_COM_L:
    case CM_CALL_CMD_ERR_OTHER:
#if MIN_CM_REV(3,100)
    case CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE:
#endif
      err = AEECM_CALL_ERROR_OTHER;
      break;

    case CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED:
      err = AEECM_CALL_ERROR_CALL_CONTROL_REJECTED;
      break;

    case CM_CALL_CMD_ERR_INVALID_SIM_STATE_S:
       err = AEET_CALL_ERROR_INVALID_SIM_STATE;
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

#ifdef FEATURE_UUS
    case CM_CALL_EVENT_USER_DATA_IND:
       evt = AEET_EVENT_USER_USER_DATA;
       break;
#endif

    case CM_CALL_EVENT_EXT_BRST_INTL:
       evt = AEET_EVENT_CALL_EXT_BRST_INTL;
       break;
    
    case CM_CALL_EVENT_CALL_CONF:
    case CM_CALL_EVENT_PRIVACY_PREF:
    case CM_CALL_EVENT_SETUP_RES:
    case CM_CALL_EVENT_NDSS_START:
    case CM_CALL_EVENT_NDSS_CONNECT:
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
#ifndef FEATURE_PS_DORMANT_PWR_SAVE
    case CM_CALL_EVENT_PS_DATA_AVAILABLE:
#endif /* ! FEATURE_PS_DORMANT_PWR_SAVE */
    case CM_CALL_EVENT_EXIT_TC:
    default:
      /* Not sent to BREW by ICM */
      break;
  }

  return evt;
}



/*=============================================================================
FUNCTION: OEMCM_ValidCallEvent

DESCRIPTION:  Check for valid call events  

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMCM_ValidCallEvent(AEECMEvent *event, 
                                 const cm_mm_call_info_s_type *call_info_ptr)
{
  if(call_info_ptr->call_id >= OEMCM_MAX_NUM_CALLS ) {
     switch (*event) {       
       case AEECM_EVENT_CALL_CALLER_ID:
       case AEECM_EVENT_CALL_CALLED_PARTY:
       case AEECM_EVENT_CALL_CONNECTED_NUM:
       case AEECM_EVENT_CALL_ABRV_ALERT:
       {
          *event = AEECM_EVENT_NONE;   
          break;
       }
     }
  }
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
      ICMCoreObj->m_bDeepSleep = TRUE;
      evt = AEECM_EVENT_PH_STANDBY_SLEEP;
      break;

    case CM_PH_EVENT_STANDBY_WAKE:
      ICMCoreObj->m_bDeepSleep = FALSE;
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

#ifdef FEATURE_DDTM_CNTL
    case CM_PH_EVENT_DDTM_PREF:
      evt = AEECM_EVENT_INTERNAL_DDTM_PREF;
      break;
#endif

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
    case CM_PH_EVENT_RESET_ACM_COMPLETED:
      evt = AEECM_EVENT_PH_ACM_RESET;
      break;

    case CM_PH_EVENT_SET_ACMMAX_COMPLETED:
      evt = AEECM_EVENT_PH_ACMMAX_SET;
      break;
#endif

#ifdef FEATURE_ALS   
    case CM_PH_EVENT_SELECTED_LINE:
    case CM_PH_EVENT_LINE_SWITCHING:
      evt = AEET_EVENT_PH_LINE_INFO;
      break;
#endif

    case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
       evt = AEET_EVENT_PH_AVAILABLE_NETWORKS;
       break;
    case CM_PH_EVENT_PREFERRED_NETWORKS_CONF:
      /* Handled separately */

#if MIN_CM_REV(3,100)
    case CM_PH_EVENT_PS_ATTACH_FAILED:
      /* TODO */
#endif

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

#ifdef FEATURE_HDR
    case CM_SS_EVENT_HDR_RSSI:
      evt = AEECM_EVENT_SS_HDR_RSSI;
      break;
#endif

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
FUNCTION: OEMTelephone_XlateSSMask

DESCRIPTION:  
  

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMTelephone_XlateSSMask(uint32 cm_mask)
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

#ifdef FEATURE_USERZONE
  if (cm_mask & CM_SS_UZ_CHANGED_MASK)
    mask |= AEECM_SS_UZ_MASK;
#endif

  if (cm_mask & CM_SS_SIM_STATE_MASK)
    mask |= AEECM_SS_SIM_STATE_MASK;

#if defined( FEATURE_HDR ) || defined (AEE_SIMULATOR)
  if (cm_mask & CM_SS_HDR_SRV_STATUS_MASK)
    mask |= AEECM_SS_HDR_SRV_STATUS_MASK;

  if (cm_mask & CM_SS_HDR_ROAM_STATUS_MASK)
    mask |= AEECM_SS_HDR_ROAM_STATUS_MASK;
#endif /* FEATURE_HDR || AEE_SIMULATOR*/

  if (cm_mask & CM_SS_P_REV_IN_USE_MASK)
    mask |= AEET_SS_P_REV_IN_USE_MASK;

  if (cm_mask & CM_SS_CCS_SUPPORTED_MASK)
    mask |= AEET_SS_CCS_SUPPORTED_MASK;
  
  return mask;
}

void OEMTelephone_Notify( AEETNotifyInfo *pNotify )
{
   OEMTelephone *pme = gpTelephone;
   ModelListener  *pl, *pn;

   if( pme ) {

      pl = pme->pListeners;
      while( pl ) {
         pn = pl->pNext;
         pl->pfnListener( pl->pListenerData, (ModelEvent *)pNotify );
         pl = pn;
      }
   }
}

#define MODEL_CancelListener( model_type, pl )              \
{                                                           \
   model_type *pme = (model_type *)pl->pCancelData;         \
                                                            \
   if( pme->pListeners == pl ) {                            \
      pme->pListeners = pl->pNext;                          \
   }                                                        \
   else {                                                   \
      pl->pPrev->pNext = pl->pNext;                         \
   }                                                        \
                                                            \
   if( pl->pNext ) {                                        \
      pl->pNext->pPrev = pl->pPrev;                         \
   }                                                        \
                                                            \
   pl->pfnCancel     = 0;                                   \
   pl->pCancelData   = 0;                                   \
}

#define MODEL_AddListener( model_type, pmodel, pl )         \
{                                                           \
   if( 0 == pl ) {                                          \
      return;                                               \
   }                                                        \
                                                            \
   LISTENER_Cancel( pl );                                   \
                                                            \
   pl->pfnCancel     = model_type##_CancelListener;         \
   pl->pCancelData   = pme;                                 \
   pl->pPrev         = 0;                                   \
   pl->pNext         = pme->pListeners;                     \
   pme->pListeners   = pl;                                  \
                                                            \
   if( pl->pNext ) {                                        \
      pl->pNext->pPrev = pl;                                \
   }                                                        \
}

void OEMTelephone_CancelListener( ModelListener *pl )
{
   MODEL_CancelListener( OEMTelephone, pl )
}

void OEMTelephone_AddListener( OEMTelephone *pme, ModelListener *pl )
{
   MODEL_AddListener( OEMTelephone, pme, pl )
}

static void OEMTelephone_Delete(OEMTelephone *me)
{
   if (gpTelephone == me) {
      gpTelephone = NULL;
   }
        
   if (0 != me->pcm) {
      OEMCM_Release(me->pcm);
   }

   sys_free(me);
}


int OEMTelephone_Open( OEMTelephone **ppfn )
{
   int nErr;
   OEMTelephone *pme = gpTelephone;
   
   if (NULL != pme) {
      pme->dwRefs++;
      *ppfn = pme;
      return SUCCESS;
   }

   pme = sys_malloc( sizeof(OEMTelephone) );
   if (NULL == pme) {
      return ENOMEMORY;
   }

   nErr = OEMTelephoneCM_New( AEE_GetShell(), (void **)&pme->pcm );
   if( SUCCESS != nErr ) {
      goto bail;
   }

   pme->dwRefs++;
   gpTelephone = *ppfn = pme;

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(AEE_SIMULATOR)
   nErr = OEMSuppsT_New(&pme->pOEMSupps);
   
   if (AEE_SUCCESS != nErr) {
      goto bail;
   }  
#endif

   pme->dwRefs++;   /* This is an extra reference held to protect cm callbacks */
   /* release this extra reference during shutdown. */
   CALLBACK_Init( &pme->m_OnExitCB, OEMTelephone_Close, pme );   
   AEE_RegisterSystemCallback( &pme->m_OnExitCB, AEE_SCB_AEE_EXIT, 0);
   

       
 bail:

   if (SUCCESS != nErr) {
      sys_free(pme);
      gpTelephone = NULL;
      OEMTelephone_Close(pme);
   }
   return nErr;
}

int OEMTelephone_Close( OEMTelephone *pme )
{
   if( 0 == --pme->dwRefs ) {
      OEMTelephone_Delete(pme);
   }
   
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(AEE_SIMULATOR)
   if(0 != pme->pOEMSupps && 2 == pme->dwRefs ) {
      OEMSuppsT_Release(pme->pOEMSupps);
   }
#endif

   
   return SUCCESS;
}

uint32 OEMTelephone_AddRef(OEMTelephone* me)
{
   return ++me->dwRefs;
}

uint32 OEMTelephone_Release(OEMTelephone* me)
{
   uint32 dwRefs = --me->dwRefs;
   if (0 == dwRefs) {
      OEMTelephone_Delete(me);
   }
   return dwRefs;
}

int OEMTelephone_SuppsReg(OEMTelephone* me, cm_sups_event_f_type* pfnEvent,
                          cm_sups_cmd_err_f_type* pfnCmdErr, cm_client_id_type* pId)
{
   (void)cm_client_sups_reg(gCMID,                             // pointer to the client struct
                            pfnEvent,                          // pointer to a cb function
                            (NULL == pfnEvent) ? CM_CLIENT_EVENT_DEREG : CM_CLIENT_EVENT_REG,
                            CM_SUPS_EVENT_REGISTER,            // from this call event
                            CM_SUPS_EVENT_GET_PASSWORD_RES,    // to this call event
                            pfnCmdErr);                        // pointer to a cb function for errors

   if (NULL != pId) {
      *pId = gCMID;
   }
   return SUCCESS;
}


// Function to support ServingNetworks functionality
int OEMTelephone_GetSSDetails(OEMTelephone *pme, cm_mm_ss_info_s_type *pSSInfo,
                              int nSize, cm_ss_event_e_type *pSSEvt)
{
   if(NULL == pSSInfo){
      return EBADPARM;
   }
   if( FALSE == ICMCoreObj->bSSInfoValid){
      return EFAILED;
   }
   
   MEMCPY(pSSInfo, &ICMCoreObj->ssInfo, MIN(nSize, sizeof(cm_mm_ss_info_s_type)));
   *pSSEvt = ICMCoreObj->ssEvt;
   
   return SUCCESS;
}

int OEMTelephone_RegisterSSEvts(OEMTelephone *pme, ISignal *piSignal)
{
   ICMCoreObj->bInitSS = TRUE;
  
   cm_ss_cmd_get_ss_info(NULL, NULL, gCMID);
   
   ICMCoreObj->piSignalSS = piSignal;
   ISignal_AddRef(ICMCoreObj->piSignalSS);
   return SUCCESS;
}

int OEMTelephone_DeregisterSSEvts(OEMTelephone *pme)
{
   ICMCoreObj->bInitSS = FALSE;
 
   IQI_RELEASEIF(ICMCoreObj->piSignalSS);
   ICMCoreObj->piSignalSS = NULL;
   return SUCCESS;
}


boolean OEMTelephone_IsModeCDMA( AEETSysMode theMode )
{
   return AEET_IS_MODE_CDMA( theMode );
}

boolean OEMTelephone_IsModeGW( AEETSysMode theMode )
{
   return AEET_IS_MODE_GW( theMode );
}

boolean OEMTelephone_IsModePrefGW( AEETModePref modePref )
{
   if (AEET_MODE_PREF_GSM_ONLY == modePref
      || AEET_MODE_PREF_WCDMA_ONLY == modePref 
      || AEET_MODE_PREF_GSM_WCDMA_ONLY == modePref
      || AEET_MODE_PREF_AUTOMATIC == modePref) {

      return TRUE;
   }

   return FALSE;
}

boolean OEMTelephone_IsCallTypeVoice( AEETCallType callType )
{
   return AEECM_IS_CALLTYPE_VOICE( callType );
}

AEETModePref OEMTelephone_GetModePreference(OEMTelephone* pme)
{
   AEETPhInfo *pPhoneInfo = OEMCM_GetValidPhInfo( pme->pcm );

   if( pPhoneInfo ) {
      return pPhoneInfo->mode_pref;
   }

   return AEET_MODE_PREF_NONE;
}

AEETOprtMode OEMTelephone_GetOperatingMode( OEMTelephone *pme )
{
   AEETPhInfo *pPhoneInfo = OEMCM_GetValidPhInfo( pme->pcm );

   if( pPhoneInfo ) {
      return pPhoneInfo->oprt_mode;
   }

   return AEET_OPRT_MODE_NONE;
}

AEETSysMode OEMTelephone_GetSystemMode( OEMTelephone *pme )
{
   return OEMT_GetSystemMode();
}

int OEMTelephone_EndAllCalls( OEMTelephone *pme )
{
   return OEMCM_EndAllCalls( pme->pcm );
}

static cm_srv_type_e_type OEMTelephone_AEETToDMSSSrvType( AEETServiceType srv )
{
   switch (srv)
   {
   case AEET_SRV_TYPE_AMPS:
      return CM_SRV_TYPE_AMPS;

   case AEET_SRV_TYPE_HDR:
      return CM_SRV_TYPE_HDR;

   case AEET_SRV_TYPE_CDMA_SPECIFIC:
      return CM_SRV_TYPE_CDMA_SPECIFIC;

   case AEET_SRV_TYPE_CDMA_AUTOMATIC:
      return CM_SRV_TYPE_CDMA_AUTOMATIC;

   case AEET_SRV_TYPE_AUTOMATIC:
      return CM_SRV_TYPE_AUTOMATIC;

   case AEET_SRV_TYPE_GSM_WCDMA:
      return CM_SRV_TYPE_GSM_WCDMA;

   case AEET_SRV_TYPE_WCDMA:
      return CM_SRV_TYPE_WCDMA;

   case AEET_SRV_TYPE_GSM:
      return CM_SRV_TYPE_GSM;

   default:
      return CM_SRV_TYPE_NONE;
   }
}

int OEMTelephone_Originate( OEMTelephone *pme, AEETCallType ct, IWebOpts *pOpts, int *pncd )
{
   AEETSysMode         sys_mode;
   WebOpt               Opt;
   cm_srv_type_e_type   srv_type = CM_SRV_TYPE_AUTOMATIC;
   cm_cdma_orig_params_s_type cdma_orig;
   cm_cdma_orig_params_s_type * cdma_orig_ptr = NULL;
   cm_gw_cs_orig_params_s_type gw_orig;
   cm_gw_cs_orig_params_s_type * gw_orig_ptr = NULL;
   cm_num_s_type         calling_number, called_number;
   cm_orig_alpha_s_type  cm_alpha;
   cm_call_id_type       cm_id;
   int nErr = SUCCESS;
   cm_call_type_e_type cm_ct = OEMCM_CallTypeToDMSS(ct);

   if( cm_ct == CM_CALL_TYPE_NONE ) {
      return EBADPARM;
   }

   sys_mode = OEMT_GetSystemMode();

   if( AEET_IS_MODE_GW(sys_mode) ) {
      gw_orig_ptr = &gw_orig;
      (void) MEMCPY( gw_orig_ptr, &pme->pcm->m_coreObj->def_gw_orig_params, sizeof(cm_gw_cs_orig_params_s_type) );

      /* Set call parameters saved by user */
      OEMCall_compose_orig_params(gw_orig_ptr);

      /* Copy GSM specific options */
      if( SUCCESS == IWEBOPTS_GetOpt( pOpts, AEET_ORIGOPT_CALLED_SUBADDR, 0, &Opt ) ) {
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
         gw_orig_ptr->called_party_subaddress.present = TRUE;
         (void) STRLCPY( (char*)gw_orig_ptr->called_party_subaddress.data, Opt.pVal, CM_CALLED_PARTY_SUBADDRESS_LENGTH );
         gw_orig_ptr->called_party_subaddress.length = STRLEN((const char*)gw_orig_ptr->called_party_subaddress.data);
         
#endif
      }
      if( SUCCESS == IWEBOPTS_GetOpt( pOpts, AEET_ORIGOPT_FWD_CUG_INFO, 0, &Opt ) ) {
         (void) MEMCPY( &gw_orig_ptr->forward_cug_info, Opt.pVal, sizeof(AEETFwdCugInfo) ); 
      }
      if( SUCCESS == IWEBOPTS_GetOpt( pOpts, AEET_ORIGOPT_CLIR_INFO, 0, &Opt ) ) {
         gw_orig_ptr->cm_clir = (uint8)(uint32)Opt.pVal;
      }

#ifdef FEATURE_GSTK
      if( SUCCESS == IWEBOPTS_GetOpt( pOpts, AEET_ORIGOPT_CALL_CONTROL_COMPLETE, 0, &Opt ) ) {
         gw_orig_ptr->call_control_completed = (boolean)(uint32)Opt.pVal;
      }
#endif
   }
   else {
      cdma_orig_ptr = &cdma_orig;
      (void) MEMCPY( cdma_orig_ptr, &pme->pcm->m_coreObj->def_cdma_orig_params, sizeof(cm_cdma_orig_params_s_type) );
      
      /* Copy CDMA specific options */
      if( SUCCESS == IWEBOPTS_GetOpt( pOpts, AEET_ORIGOPT_SERVICE_OPTION, 0, &Opt ) ) {
         cdma_orig_ptr->srv_opt = (uint16)(uint32)Opt.pVal;
      }
      if( SUCCESS == IWEBOPTS_GetOpt( pOpts, AEET_ORIGOPT_OTASP_ACTIVATION_CODE, 0, &Opt ) ) {
         cdma_orig_ptr->activate_code = (cm_activate_code_e_type)(uint32)Opt.pVal;
      }
   }
   
   if( SUCCESS == IWEBOPTS_GetOpt( pOpts, AEET_ORIGOPT_SERVICE_TYPE, 0, &Opt ) ) {
      srv_type = OEMTelephone_AEETToDMSSSrvType((AEETServiceType)(uint32)Opt.pVal );
   }

   if( SUCCESS == IWEBOPTS_GetOpt( pOpts, AEET_ORIGOPT_CALLED_NUMBER, 0, &Opt ) ) {
      const char *pszCalledNumber = Opt.pVal;

      if( AEET_IS_MODE_GW(sys_mode) ) {
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
         if( 0 == STRNCMP(pszCalledNumber, "*31#", 4) ) {
            gw_orig_ptr->cm_clir = CM_CLIR_SUPPRESSION;
            pszCalledNumber += 4;
         }
         else if( 0 == STRNCMP(pszCalledNumber, "#31#", 4) ) {
            gw_orig_ptr->cm_clir = CM_CLIR_INVOCATION;
            pszCalledNumber += 4;
         }
#endif
      }
      
      if (SUCCESS != OEMCM_InitNum( &called_number, pszCalledNumber )) {
         ZEROAT(&called_number);
      }
   }
   else {
      ZEROAT(&called_number);
   }

   if( SUCCESS == IWEBOPTS_GetOpt( pOpts, AEET_ORIGOPT_CALLING_NUMBER, 0, &Opt ) ) {
      const char *pszCallingNumber = Opt.pVal;

      if (SUCCESS != OEMCM_InitNum( &calling_number, pszCallingNumber )) {
         ZEROAT(&calling_number);
      }
   }
   else {
      ZEROAT(&calling_number);
   }

   if( SUCCESS == IWEBOPTS_GetOpt( pOpts, AEET_ORIGOPT_ALPHATAG, 0, &Opt ) ) {
      const AECHAR *puzAlphaTag = Opt.pVal;

      (void) WSTRTOSTR( puzAlphaTag, (char *)cm_alpha.buf, CM_MAX_ALPHA_LEN_FOR_CALL_ORIG );
      cm_alpha.len = (uint8)STRLEN((char *)cm_alpha.buf);
   }
   else {
      cm_alpha.len = 0;
   }

#ifdef FEATURE_UUS
   if( SUCCESS == IWEBOPTS_GetOpt( pOpts, AEET_ORIGOPT_USER_DATA, 0, &Opt ) ) {
      if( AEET_IS_MODE_GW(sys_mode) ) {
         const AEETUserDataInfo *pUserInfo = Opt.pVal;

         if (pUserInfo) {
            OEMCM_InitUserData(&gw_orig_ptr->mo_user_data, pUserInfo);
         }
      }
   }
#endif

   /* Some more validation for call type voice, there is no number */
   if( OEMTelephone_IsCallTypeVoice(ct) 
       && 0 == called_number.len ) {
      nErr = EBADPARM;
   }
   else {
      nErr = OEMCM_OriginateCall_Int( pme->pcm,
                                      cm_ct, 
                                      srv_type, 
                                      &calling_number, 
                                      &called_number, 
                                      &cm_alpha, 
                                      cdma_orig_ptr, 
                                      gw_orig_ptr, 
                                      &cm_id );
      if( SUCCESS == nErr ) {
         *pncd = (int)cm_id;
      }
   }

   return nErr;
}

int OEMTelephone_SendFlash( OEMTelephone *pme, int cd, const char *pszNumber )
{
   cm_call_id_type call_id = (cm_call_id_type)(cd-1);

   return OEMCM_SendFlash_Int( pszNumber, call_id );
}

int OEMTelephone_Answer( OEMTelephone *pme, int cd )
{
   AEETCallInfo *         call_info;
   boolean                 ans = FALSE;
   cm_call_id_type         call_id = cd - 1;

   call_info = &(pme->pcm->m_coreObj->m_call_info[call_id]);

   if( call_info->call_state != AEECM_CALL_STATE_INCOM ) {
      return EBADSTATE;
   }
   
   /* check if somebody else has already answered the call */
   /* Assuming that nobody else directly uses DMSS CM to answer calls
      that BREW Apps are interested in */
   AEECM_MUTEX_LOCK();
   if (!pme->pcm->m_coreObj->m_state[call_id].bAnswered) {
     pme->pcm->m_coreObj->m_state[call_id].bAnswered = TRUE;
     ans = TRUE;
   } 
   AEECM_MUTEX_UNLOCK();

   if (!ans)
     return EALREADY;

   return OEMCM_AnswerCall_Int( pme->pcm, call_id, OEMCM_CallTypeToDMSS(call_info->call_type) );
}

int OEMTelephone_End( OEMTelephone *pme, int cd )
{
   boolean               end = FALSE;
   cm_call_id_type       call_id = (cm_call_id_type)(cd-1);

   /* check if somebody else has already ended the call */
   AEECM_MUTEX_LOCK();
   if (!pme->pcm->m_coreObj->m_state[call_id].bEnded) {
     pme->pcm->m_coreObj->m_state[call_id].bEnded = TRUE;
     end = TRUE;
   } 
   AEECM_MUTEX_UNLOCK();

   if (!end)
     return EALREADY;

   return OEMCM_EndCall_Int( pme->pcm, call_id );
}

int OEMTelephone_Reject( OEMTelephone *pme, int cd )
{
   cm_call_id_type         call_id = (cm_call_id_type)(cd - 1);
   AEETSysMode            sys_mode = OEMT_GetSystemMode();
   AEETCallInfo *         call_info;

   call_info = &(pme->pcm->m_coreObj->m_call_info[call_id]);

   if( call_info->call_state == AEECM_CALL_STATE_INCOM 
       && AEET_IS_MODE_CDMA(sys_mode) ) {
      return OEMTelephone_End(pme,cd);
   }

   return OEMCM_RejectCall( pme->pcm, call_id );
}

int OEMTelephone_Transfer( OEMTelephone *pme, int cd1, int cd2 )
{
   cm_call_id_type               call_id1 = (cm_call_id_type)(cd1 - 1);
   cm_call_id_type               call_id2 = (cm_call_id_type)(cd2 - 1);
   AEETSysMode                  sys_mode = OEMT_GetSystemMode();
   AEETCallInfo *               call_info;
   AEECMEventCBS *               event_cb;
   cm_call_sups_params_s_type    sups_params;
   AEECMCallErrCBPkt *           err_cb_pkt;
   boolean                       bConv = FALSE, bHold = FALSE;

   if( AEET_IS_MODE_CDMA(sys_mode) ) {
      return EUNSUPPORTED;
   }

   /* For transfer one call must be in conversation and other on hold */
   call_info = &(pme->pcm->m_coreObj->m_call_info[call_id1]);

   if( call_info->call_state == AEECM_CALL_STATE_CONV ) {
      bConv = TRUE;
   }
   else if( call_info->call_state == AEECM_CALL_STATE_ONHOLD ) {
      bHold = TRUE;
   }

   if( !bConv && !bHold ) {
      return EBADSTATE;
   }

   /* Check the other call */
   call_info = &(pme->pcm->m_coreObj->m_call_info[call_id2]);

   if( call_info->call_state == AEECM_CALL_STATE_CONV ) {
      bConv = TRUE;
   }
   else if( call_info->call_state != AEECM_CALL_STATE_ONHOLD ) {
      bHold = TRUE;
   }

   if( !(bConv && bHold) ) {
      return EBADSTATE;    /* does not meet transfer condition */
   }

   /* Allocate event callback structure */
   event_cb = OEMCM_AllocEventCBS(ICMCoreObj, CB_TYPE_ERRPKT);

   if (!event_cb) {
      return ENOMEMORY;
   }

   err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
   err_cb_pkt->call_id = call_id1;
   sups_params.call_id = call_id1;

   if( !cm_compat_call_cmd_sups(OEMCM_DMSSCallErrCB, event_cb, gCMID, 
                            CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER, &sups_params) ) {
      OEMCM_FreeEventCBS( event_cb );
      return EFAILED;
   }

   return SUCCESS;
}

int OEMTelephone_Redirect( OEMTelephone *pme, int cd, const char *pszNumber )
{
   cm_call_id_type               call_id = (cm_call_id_type)(cd - 1);
   AEETSysMode                  sys_mode = OEMT_GetSystemMode();
   AEETCallInfo *               call_info;
   AEECMEventCBS *               event_cb;
   cm_call_sups_params_s_type    sups_params;
   AEECMCallErrCBPkt *           err_cb_pkt;
   int                           nErr;

   call_info = &(pme->pcm->m_coreObj->m_call_info[call_id]);

   if( call_info->call_state != AEECM_CALL_STATE_INCOM ) {
      return EBADSTATE;
   }

   if( AEET_IS_MODE_CDMA(sys_mode) ) {
      return OEMTelephone_SendFlash(pme,cd,pszNumber);
   }

   /* Allocate event callback structure */
   event_cb = OEMCM_AllocEventCBS(ICMCoreObj, CB_TYPE_ERRPKT);

   if (!event_cb) {
      return ENOMEMORY;
   }

   err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
   err_cb_pkt->call_id = call_id;
   sups_params.call_id = call_id;
   
   nErr = OEMCM_InitNum( &sups_params.sups_number, pszNumber );
   if (SUCCESS != nErr)
   {
      MSG_ERROR("OEMCM_InitNum Failed",
                 0, 0, 0);
      return nErr;
   }

   if( !cm_compat_call_cmd_sups(OEMCM_DMSSCallErrCB, event_cb, gCMID, 
                            CM_CALL_SUPS_TYPE_CALL_DEFLECTION, &sups_params) ) {
      OEMCM_FreeEventCBS( event_cb );
      return EFAILED;
   }

   return SUCCESS;
}

int OEMTelephone_Activate( OEMTelephone *pme, int cd )
{
   cm_call_id_type               call_id = (cm_call_id_type)(cd-1);
   AEETSysMode                  sys_mode = OEMT_GetSystemMode();
   AEETCallInfo *                call_info;
   AEECMEventCBS *               event_cb;
   cm_call_sups_params_s_type    sups_params;
   AEECMCallErrCBPkt *           err_cb_pkt;

   call_info = &(pme->pcm->m_coreObj->m_call_info[call_id]);

   if( call_info->call_state != AEECM_CALL_STATE_CONV
       && call_info->call_state != AEECM_CALL_STATE_ONHOLD ) {
      return EBADSTATE;
   }
   
   if( AEET_IS_MODE_CDMA(sys_mode ) ) {
      return EUNSUPPORTED;
   }
   
   /* Allocate event callback structure */
   event_cb = OEMCM_AllocEventCBS(ICMCoreObj, CB_TYPE_ERRPKT);

   if (!event_cb) {
      return ENOMEMORY;
   }

   err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
   err_cb_pkt->call_id = call_id;
   sups_params.call_id = call_id;

   if( !cm_compat_call_cmd_sups(OEMCM_DMSSCallErrCB, event_cb, gCMID, 
                            CM_CALL_SUPS_TYPE_SELECT_CALL_X, &sups_params) ) {
      OEMCM_FreeEventCBS( event_cb );
      return EFAILED;
   }

   return SUCCESS;
}

int OEMTelephone_Hold( OEMTelephone *pme, int cd )
{
   cm_call_id_type               call_id = (cm_call_id_type)(cd-1);
   AEETSysMode                  sys_mode = OEMT_GetSystemMode();
   AEETCallInfo *               call_info;
   AEECMEventCBS *               event_cb;
   cm_call_sups_params_s_type    sups_params;
   AEECMCallErrCBPkt *           err_cb_pkt;

   call_info = &(pme->pcm->m_coreObj->m_call_info[call_id]);

   if( call_info->call_state != AEECM_CALL_STATE_CONV ) {
      return EBADSTATE;
   }
   
   if( AEET_IS_MODE_CDMA(sys_mode ) ) {
      return EUNSUPPORTED;
   }
   
   /* Allocate event callback structure */
   event_cb = OEMCM_AllocEventCBS(ICMCoreObj, CB_TYPE_ERRPKT);

   if (!event_cb) {
      return ENOMEMORY;
   }

   err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;
   err_cb_pkt->call_id = call_id;
   sups_params.call_id = call_id;

   if( !cm_compat_call_cmd_sups(OEMCM_DMSSCallErrCB, event_cb, gCMID, 
                            CM_CALL_SUPS_TYPE_HOLD_CALL, &sups_params) ) {
      OEMCM_FreeEventCBS( event_cb );
      return EFAILED;
   }

   return SUCCESS;
}

int OEMTelephone_GetCallInfo( OEMTelephone *pme, int cd, AEETCallInfo *pInfo, uint32 dwSize )
{
   if( OEMCM_IsValidAppCallID((AEECMCallID)cd) ) 
      return OEMCM_GetCallInfo( pme->pcm, (AEECMCallID)cd, (AEETCallInfo *)pInfo, dwSize );

   return AEET_ERR_BADDESC;
}

int OEMTelephone_GetPhoneInfo( OEMTelephone *pme, AEETPhInfo *pInfo, uint32 dwSize )
{
   if( !pme || !pInfo ) {
      return EBADPARM;
   }

   if( !pme->pcm->m_coreObj || !pme->pcm->m_coreObj->m_ph_valid ) {
     return EFAILED;
   }

   dwSize = MIN(dwSize,sizeof(pme->pcm->m_coreObj->m_ph_info));

   (void)MEMCPY(pInfo, &pme->pcm->m_coreObj->m_ph_info, dwSize);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   pInfo->aoc_info.ccm = cm_ph_cmd_aoc_get_ccm(); 
   pInfo->aoc_info.acm = cm_ph_cmd_aoc_get_acm(); 
   pInfo->aoc_info.acm_max = cm_ph_cmd_aoc_get_acmmax(); 
#endif

#if defined(FEATURE_ALS)
#else
   pInfo->line_info.bLineSwitchingEnabled = FALSE;
   pInfo->line_info.dwMaxSupportedLines   = 1;
   pInfo->line_info.dwSelectedLineId      = 1;
#endif

   return SUCCESS;
}

int OEMTelephone_GetServingSystemInfo( OEMTelephone *pme, AEETSSInfo *pInfo, uint32 dwSize )
{
   if( !pme || !pInfo ) {
      return EBADPARM;
   }

   if (!pme->pcm->m_coreObj || !pme->pcm->m_coreObj->m_ss_valid) {
     return EFAILED;
   }

   dwSize = MIN(dwSize,sizeof(pme->pcm->m_coreObj->m_ss_info));
   (void) MEMCPY(pInfo, &pme->pcm->m_coreObj->m_ss_info, dwSize);

   return SUCCESS;
}

int OEMTelephone_SetOperatingMode( OEMTelephone *pme, AEETOprtMode mode )
{
   return OEMCM_SetOperatingMode( pme->pcm, mode );
}

static boolean OEMTelephone_ModemOprtMode(AEETOprtMode mode) 
{
  if( ( mode == AEET_OPRT_MODE_PWROFF      ) ||
      ( mode == AEET_OPRT_MODE_RESET       ) ||
      ( mode == AEET_OPRT_MODE_LPM         ) ||
      ( mode == AEET_OPRT_MODE_FTM         ) || 
      ( mode == AEET_OPRT_MODE_NET_TEST_GW ) )
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }

}

int OEMTelephone_SetNamSel( OEMTelephone *pme, AEETNAM nam )
{
   cm_nam_e_type cm_nam;
   AEETPhInfo *pPhoneInfo;

   if( !OEMTelephone_IsModeCDMA(OEMTelephone_GetSystemMode(pme)) ) {
      return AEET_ERR_OP_NOT_ALLOWED;
   }

   switch( nam ) {
   case AEET_NAM_1:
      cm_nam = CM_NAM_1;
      break;
   case AEET_NAM_2:
      cm_nam = CM_NAM_2;
      break;
   case AEET_NAM_3:
      cm_nam = CM_NAM_3;
      break;
   case AEET_NAM_4:
      cm_nam = CM_NAM_4;
      break;
   case AEET_NAM_AUTO:
      cm_nam = CM_NAM_AUTO;
      break;
   default:
      cm_nam = CM_NAM_NONE;
   }

   if( 1 == NV_MAX_NAMS && CM_NAM_AUTO == cm_nam ) {
      cm_nam = CM_NAM_1;
   }

   if( CM_NAM_AUTO != cm_nam && !BETWEEN((int)cm_nam, (int)CM_NAM_NONE+1, (int)NV_MAX_NAMS) ){
      return AEET_ERR_OUTOF_RESOURCES;
   }

   if( CM_NAM_NONE == cm_nam ) {
      return AEET_ERR_OUTOF_RESOURCES;
   }

   if( !OEMTelephone_ModemOprtMode(OEMTelephone_GetOperatingMode(pme)) ){
      return AEET_ERR_PHONE_NOT_ACTIVE;
   }

   if (NULL == (pPhoneInfo = OEMCM_GetValidPhInfo( pme->pcm ))) {
      return AEET_ERR_OUTOF_RESOURCES;
   }
   
   if( pPhoneInfo->is_in_use ) {
      return AEET_ERR_PHONE_IN_USE;
   }

   return OEMCM_SetNAMSel( pme->pcm, cm_nam );
}

int OEMTelephone_SetRSSIDeltaThreshold( OEMTelephone *pme, byte thr )
{
   if( 0 == thr ){
      return EBADPARM;
   }

   return OEMCM_SetRSSIDeltaThreshold( pme->pcm, thr );
}

int OEMTelephone_WakeupFromStandby( OEMTelephone *pme )
{
   
   if (AEET_OPRT_MODE_ONLINE != OEMTelephone_GetOperatingMode(pme)) {
      return AEET_ERR_PHONE_NOT_ONLINE;
   }

   if (!pme->pcm->m_coreObj->m_bDeepSleep) {
      return EALREADY;
   }

   if (!cm_ph_cmd_wakeup_from_standby(OEMCM_DMSSPhErrCB, NULL, gCMID)) {
     return EFAILED;
   }

   return SUCCESS;
}

int OEMTelephone_SetSubscriptionStatus( OEMTelephone *pme, AEETSysMode mode, boolean status )
{
   cm_subscription_status_e_type cdma_subs = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
   cm_subscription_status_e_type gw_subs = CM_SUBSCRIPTION_STATUS_NO_CHANGE;
   boolean bRet;

   if( mode == AEET_SYS_MODE_CDMA ) {
      cdma_subs = CM_SUBSCRIPTION_STATUS_CHANGE;
   }
   else if(AEET_IS_MODE_GW(mode)) {
      gw_subs = CM_SUBSCRIPTION_STATUS_CHANGE;
   }
   else {
      return EUNSUPPORTED;
   }

   if( status ) {
      bRet = cm_ph_cmd_subscription_available( OEMCM_DMSSPhErrCB, NULL, gCMID,
                                               cdma_subs, gw_subs
#ifdef CM_API_WLAN
                                             , CM_SUBSCRIPTION_STATUS_NO_CHANGE
#endif
                                             );
   }
   else {
      bRet = cm_ph_cmd_subscription_not_available( OEMCM_DMSSPhErrCB, NULL, gCMID,
                                                   cdma_subs, gw_subs
#ifdef CM_API_WLAN
                                             , CM_SUBSCRIPTION_STATUS_NO_CHANGE
#endif
                                                 );
   }

   return (bRet)?SUCCESS:ENOMEMORY;
}

int OEMTelephone_BurstDTMF( OEMTelephone *pme, int cd, AEETDTMFOnLength on, AEETDTMFOffLength off, const char *pszDigits )
{
   return OEMCM_BurstDTMF_Int( pme->pcm, (AEECMCallID)cd, on, (uint32)off, pszDigits );
}

int OEMTelephone_StartDTMF( OEMTelephone *pme, int cd, char bDigit )
{
   return OEMCM_StartContDTMF_Int( pme->pcm, (AEECMCallID)cd, bDigit );
}

int OEMTelephone_StopDTMF( OEMTelephone *pme, int cd )
{
   return OEMCM_StopContDTMF( pme->pcm, (AEECMCallID)cd );
}

static void OEMTelephone_RefreshNetworks_Resume(OEMTelephone *pme, AEECallback* pcb,
                                                int nStatus)
{
   OEMTRefreshNetworksReq* pReq = pcb->pCancelData;

   pme->pRefreshNetworksReqList = 0;
   *pReq->pnStatus = nStatus;
   pReq->me = 0;
   AEE_ResumeCallback(pcb, 0);
}

/* Invoked on all OEMTelephone_RefreshNetworks_Resume() and
** CALLBACK_Cancel() on the AEECallback to OEMTelephone_RefreshNetworks() */
static void OEMTelephone_RefreshNetworks_Cancel(AEECallback* pcb)
{
   OEMTRefreshNetworksReq* pReq = pcb->pCancelData;

   pcb->pfnCancel = 0;
   pcb->pCancelData = 0;

   if (0 != pReq->me) {
      if (*pReq->pnStatus < 0) {        /* Application invoked cancel */ 
         pReq->me->pRefreshNetworksReqList = 0;
         cm_ph_cmd_terminate_get_networks(NULL, 0, gCMID);
      }

      OEMTelephone_Release(pReq->me);   /* Let go of OEMTelephone reference */
      pReq->me = 0;

   }
}

static void OEMTelephone_RefreshNetworks_CmdCb(void* po,
                                               cm_ph_cmd_e_type ph_cmd,
                                               cm_ph_cmd_err_e_type ph_cmd_err)
{
   AEECallback* pcb;
   OEMTelephone* pme = (OEMTelephone*)po;

   if (CM_PH_CMD_ERR_NOERR == ph_cmd_err) {
      return;
   }

   MSG_ERROR("ERR : cm_ph_cmd_get_networks()  0x%X ", ph_cmd_err, 0, 0);

   pcb = pme->pRefreshNetworksReqList;
   if (0 != pcb) {
      OEMTelephone_RefreshNetworks_Resume(pme, pcb, EFAILED);
   }
}

/* Invoked in BREW thread */
static void OEMTelephone_RefreshNetworks_Cb(void *data)
{
   AEECMEventCBS*          event_cb = (AEECMEventCBS*)data;
   cm_ph_info_s_type*      ph_info;
   AEETNotifyInfo*         notify_info;

   if (!event_cb || !ICMCoreObj) {
      return;
   }

   ph_info = (cm_ph_info_s_type *) event_cb->data;

   MSG_HIGH("Received a fresh list of available networks", 0, 0, 0);
   OEMCM_CopyInAvailableNetworks(&ICMCoreObj->m_ph_info.available_networks, 
                                 &ph_info->available_networks);

   /* Resume the waiting callback. */
   if (0 != gpTelephone->pRefreshNetworksReqList) {
      OEMTelephone_RefreshNetworks_Resume(gpTelephone, gpTelephone->pRefreshNetworksReqList, 
                                          AEE_SUCCESS);
   }

   /* Notify the listeners. */
   notify_info = OEMCM_MALLOC(sizeof(AEETNotifyInfo));
   if (NULL != notify_info) {

      notify_info->event = event_cb->event;
      (void) MEMCPY(&notify_info->event_data.ph, &ICMCoreObj->m_ph_info, sizeof(AEETPhInfo));
      #if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
      notify_info->event_data.ph.aoc_info.ccm = cm_ph_cmd_aoc_get_ccm(); 
      notify_info->event_data.ph.aoc_info.acm = cm_ph_cmd_aoc_get_acm(); 
      notify_info->event_data.ph.aoc_info.acm_max = cm_ph_cmd_aoc_get_acmmax(); 
      #endif

      OEMTelephone_Notify(notify_info);
      OEMCM_FREE(notify_info);
   }

   OEMCM_FreeEventCBS(event_cb);

   return;
}

/* Perform a scan for avaialble networks */
int OEMTelephone_RefreshNetworks(OEMTelephone *pme, AEECallback *pcb, int *pnStatus)
{
   AEETModePref modePref = OEMTelephone_GetModePreference(pme);
   AEECallback* pcbSys;
   OEMTRefreshNetworksReq* pReq;

   if ((AEECallback*)0 == pcb || (int*)0 == pnStatus) {
      return EBADPARM;
   }

   /* Device is not in GW mode or preference is not GW. */
   if (!OEMTelephone_IsModeGW(OEMTelephone_GetSystemMode(pme))
       || !OEMTelephone_IsModePrefGW(modePref)) {
      return AEET_ERR_OP_NOT_ALLOWED;
   }

   if( !OEMTelephone_ModemOprtMode(OEMTelephone_GetOperatingMode(pme)) ){
      return AEET_ERR_PHONE_NOT_ACTIVE;
   }

   if (0 != pme->pRefreshNetworksReqList) {  /* Only one request allowed. */
      return EALREADY;
   }

   /* Link to system Context Callback */
   pcbSys = AEE_LinkSysCallbackEx(pcb, sizeof(OEMTRefreshNetworksReq));
   if ((AEECallback*)0 == pcbSys) {
      return ENOMEMORY;
   }

   /* CALLBACK_Cancel(pcb); 
   ** done in AEE_LinkSysCallbackEx(). */ 

   *pnStatus = -1;                       /* Request in progress state, 
                                         ** cancel operation in this state
                                         ** will issue termination. */
   /* Get the memory for request */
   pReq = (OEMTRefreshNetworksReq*)(pcbSys+1);

   /* Construct the request */
   pReq->me = pme;
   OEMTelephone_AddRef(pme);            /* Request holds a reference to the OEMTelephone */
   pReq->pcb = pcbSys;
   pReq->pnStatus = pnStatus;

   /* Take ownership of the callback */
   pcbSys->pfnCancel = OEMTelephone_RefreshNetworks_Cancel;
   pcbSys->pCancelData = pReq;

   /* Allow multiple request ?? */
   pcbSys->pNext = pme->pRefreshNetworksReqList;
   pme->pRefreshNetworksReqList = pcbSys;

   /* Initiate the scan */
   if (!cm_ph_cmd_get_networks(OEMTelephone_RefreshNetworks_CmdCb,
                              pme,
                              gCMID,
                              CM_NETWORK_LIST_AVAILABLE
                              #ifdef CM_API_WLAN
                              ,CM_MODE_PREF_GSM_WCDMA_ONLY
                              #endif
                              )) {
      *pnStatus = EFAILED;
      AEE_ResumeCallback(pcbSys, 0);
      return EFAILED;
   }

   return AEE_SUCCESS;
}

int OEMTelephone_Command( OEMTelephone *pme, uint32 dwCmd, AEECallback *pcb, void *pInBuffer, 
                       uint32 dwInBufSize, void *pOutBuffer, uint32 dwOutBufSize )
{
   int nErr = EUNSUPPORTED;

   switch( dwCmd ) {
   case AEEPHCTL_CMD_SET_PREFERRED_NETWORKS:
      {
         AEETUserPLMNList *pPref = (AEETUserPLMNList *)pInBuffer;
         if( !pPref || dwInBufSize < sizeof(AEETUserPLMNList)
             || !pcb || !pOutBuffer ) {
            return EBADPARM;
         }
         return OEMCM_SetPrefNetworks( pme->pcm, (AEECMUserPLMNList *)pInBuffer, (AEECMPhError *)pOutBuffer, pcb );
      }
   case AEEPHCTL_CMD_SET_SYSTEM_PREFERENCE:
      {
         AEETSystemPreference *pPref = (AEETSystemPreference *)pInBuffer;
         if( !pPref || dwInBufSize < sizeof(AEETSystemPreference)) {
            return EBADPARM;
         }
         return OEMCM_SetSystemPreference( pme->pcm, pPref->mode_pref, pPref->pref_term, pPref->pref_duration,
                                           pPref->acq_order_pref, pPref->band_pref, pPref->roam_pref,
                                           pPref->hybr_pref, pPref->srv_domain_pref, pPref->nw_sel_mode_pref,
                                           &pPref->plmn, (AEECMPhError *)pOutBuffer, pcb );
      }
   case AEEPHCTL_CMD_SET_RTRE_CONFIG:
      return OEMCM_SetRTREConfig( pme->pcm, (AEECMRTREConfig)(uint32)pInBuffer );

   case AEEPHCTL_CMD_SET_ANSWER_VOICE_PREF:
      {
         struct {
            AEETAnsVoicePref p;
            uint32 dwD;
         } *plocal = pInBuffer;

         return OEMCM_SetAnswerVoicePref( pme->pcm, (AEECMAnsVoicePref)plocal->p, plocal->dwD );
      }

   case AEEPHCTL_CMD_AVOID_SERVING_SYSTEM:
      {
         struct {
            AEETAvoidSSPref p;
            uint32 dwD;
         } *plocal = pInBuffer;

         return OEMCM_AvoidServingSystem( pme->pcm, (AEECMAvoidSSPref)plocal->p, plocal->dwD );
      }
      
   case AEEPHCTL_CMD_SET_VOICE_PRIVACY:
      return OEMCM_SetPrivacyPref( pme->pcm, (AEECMPrivacyPref)(uint32)pInBuffer );

   case AEEPHCTL_CMD_RESET_ACM:
      return OEMCM_ResetACM( pme->pcm );

   case AEEPHCTL_CMD_SET_ACMMAX:
      return OEMCM_SetACMMax( pme->pcm, (uint32)pInBuffer );

   case AEEPHCTL_CMD_SET_DDTMPREF:
      return OEMCM_SetDDTMPref( pme->pcm, (AEECMDDTMPref)(uint32)pInBuffer );

   case AEEPHCTL_CMD_SELECT_LINE:
      return OEMTelephone_SelectLine( pme, (uint32)pInBuffer );

   case AEEPHCTL_CMD_ENABLE_LINESWITCH:
      return OEMTelephone_EnableLineSwitch( pme, (boolean)(uint32)pInBuffer );

   case AEEPHCTL_CMD_REFRESH_NETWORKS_LIST:
      return OEMTelephone_RefreshNetworks(pme, pcb, (int*)pOutBuffer);
   }

   return nErr;
}

static int Parse_SuppsCallOp( const char *pszString, byte *pN, const char **pDN, cm_call_sups_type_e_type *p_sups_type )
{
   int code;
   cm_call_sups_type_e_type sups_type = CM_CALL_SUPS_TYPE_NONE;
   int nErr = SUCCESS;
   
   *pDN = 0;
   *pN  = 0;

   if( !pszString || !pN || !pDN || !p_sups_type ) {
      return EBADPARM;
   }

   code = *pszString++;

   switch( code ) {
   case '0':
      if( '\0' == *pszString ){
         sups_type = CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB;
      }
      else {
         sups_type = CM_CALL_SUPS_TYPE_NONE;
         nErr = EINVALIDFORMAT;
      }
      break;
   case '1':
      if( *pszString ) {
         *pN = ATOI(pszString);
         if(*pN == 0) {
            sups_type = CM_CALL_SUPS_TYPE_NONE;
            nErr = EINVALIDFORMAT;
         }
         else {
            sups_type = CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL;
         }
      }
      else {
         sups_type = CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL;
      }
      break;
   case '2':
      if( *pszString ) {
         *pN = ATOI(pszString);
         sups_type = (*pN == 0) ? CM_CALL_SUPS_TYPE_NONE : CM_CALL_SUPS_TYPE_SELECT_CALL_X;
      }
      else {
         sups_type = CM_CALL_SUPS_TYPE_HOLD_CALL;
      }
      break;
   case '3':
      if( '\0' == *pszString ) {
         sups_type = CM_CALL_SUPS_TYPE_MULTIPARTY_CALL;
      }
      else {
         sups_type = CM_CALL_SUPS_TYPE_NONE;
         nErr = EINVALIDFORMAT;
      }
      break;
   case '4':
      if( *pszString ) {
         if( '*' == *pszString++ ) {
            *pDN = pszString;
            if( strlen( *pDN ) ) {
               sups_type = CM_CALL_SUPS_TYPE_CALL_DEFLECTION;
            }
            else {
               sups_type = CM_CALL_SUPS_TYPE_NONE;
               nErr = EINCOMPLETEITEM;
            }
         }
         else {
            nErr = EINVALIDFORMAT;
            sups_type = CM_CALL_SUPS_TYPE_NONE;
         }
      }
      else {
         sups_type = CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER;
      }
      break;
   default:
      nErr = EINVALIDFORMAT;
      break;
   }

   *p_sups_type = sups_type;

   return nErr;
}

int OEMTelephone_SuppsCallOp( OEMTelephone *pme, const char *pszString )
{
   byte X = 0;
   const char *DN = (const char *)0;
   int nErr = SUCCESS;
   AEECMEventCBS * event_cb = NULL;

   cm_call_sups_type_e_type sups_type = CM_CALL_SUPS_TYPE_NONE;
   
   if( !pszString ) {
      nErr = EBADPARM;
      goto Done;
   }

   if(AEET_SYS_MODE_CDMA == OEMTelephone_GetSystemMode(pme)) {
      nErr = AEET_ERR_OP_NOT_ALLOWED;
      goto Done;
   }

   nErr = Parse_SuppsCallOp(pszString, &X, &DN, &sups_type);

   /* if sups_type is identified issue the command */
   if( SUCCESS == nErr && sups_type != CM_CALL_SUPS_TYPE_NONE ) {
      cm_call_sups_params_s_type  sups_params;
      AEECMCallErrCBPkt *         err_cb_pkt;

      event_cb = OEMCM_AllocEventCBS(pme->pcm->m_coreObj, CB_TYPE_ERRPKT);
      if (!event_cb) {
         nErr = EFAILED;
         goto Done;
      }
      err_cb_pkt = (AEECMCallErrCBPkt *)event_cb->data;

      ZEROAT(&sups_params);
      /* Populate the sups_params */
      switch( X ) {
      case 0:
         break;
      case '*':
         if (SUCCESS != OEMCM_InitNum( &sups_params.sups_number, DN )) {
            nErr = EBADPARM;
            goto Done;
         }
         break;
      default:
         {
            cm_call_id_type      call_id;
            AEECMCallState       call_state;

            if (!OEMCM_IsValidAppCallID(X)){
               nErr = AEET_ERR_BADDESC;
               goto Done;
            }

            call_id = X-1;
            call_state = OEMCM_GetCallState(call_id);

            if (AEECM_CALL_STATE_IDLE == call_state
                || AEECM_CALL_STATE_ENDED == call_state) {
               nErr = EBADSTATE;
               goto Done;
            }

            sups_params.call_id = call_id;
            err_cb_pkt->call_id = call_id;
         }
      }
      
      if (!cm_compat_call_cmd_sups(OEMCM_DMSSCallErrCB, event_cb, gCMID, 
                                   sups_type, &sups_params)) {
        nErr = EFAILED;
        goto Done;
      }

      nErr = SUCCESS;
   }

Done:
   if ((SUCCESS != nErr) && (NULL != event_cb)){
      OEMCM_FreeEventCBS( event_cb );
   }

   return nErr;
}

int OEMTelephone_IsSuppsCallOp( OEMTelephone *pme, const char *pszString, uint32 *pnum )
{
   byte X = 0;
   const char *DN = (const char *)0;
   cm_call_sups_type_e_type sups_type;
   int nErr;
   
   if( !pszString ) {
      return EBADPARM;
   }

   nErr = Parse_SuppsCallOp(pszString, &X, &DN, &sups_type);

   /* if sups_type is identified issue the command */
   if( SUCCESS == nErr && sups_type != CM_CALL_SUPS_TYPE_NONE ) {
      if( (uint32 *)0 != pnum ) {
         *pnum = X;
      }
   }

   return nErr;
}

int OEMTelephone_GetSuppsTrans( OEMTelephone *pme, AEETSuppsTransId tid, ISuppsTrans **ppTrans )
{
   #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(AEE_SIMULATOR)
   extern int ISuppsTrans_New( IShell *pIShell, AEECLSID clsid, void **ppf );
   return ISuppsTrans_New( AEE_GetShell(), tid, (void **)ppTrans );
   #else
   return EUNSUPPORTED;
   #endif
}

int OEMTelephone_GetLineInfo( OEMTelephone *pme, AEETLineInfo *po, uint32 dwSize )
{

   if( !po || dwSize < sizeof(AEETLineInfo) ) {
      return EBADPARM;
   }

#ifndef FEATURE_ALS
   po->bLineSwitchingEnabled = FALSE;
   po->dwMaxSupportedLines   = 1;
   po->dwSelectedLineId      = 1;

   return SUCCESS;
#else
   {
      AEETPhInfo *pPhoneInfo = OEMCM_GetValidPhInfo( pme->pcm );

      if( pPhoneInfo ) {
         dwSize = MIN(dwSize, sizeof(pPhoneInfo->line_info));
         MEMCPY(po,&pPhoneInfo->line_info,dwSize);
         return SUCCESS;
      }
   }

   return EFAILED;
#endif
}

int OEMTelephone_SelectLine( OEMTelephone *pme, uint32 lineid )
{

#ifndef FEATURE_ALS

   return AEET_ERR_OP_NOT_ALLOWED;
#else
   AEETLineInfo li;
   int          nErr;

   nErr = OEMTelephone_GetLineInfo(pme,&li,sizeof(li));
   if( SUCCESS != nErr ) {
      return nErr;
   }

   if( 0 < lineid && lineid <= li.dwMaxSupportedLines ) {
      
      if( li.bLineSwitchingEnabled ) {
         if (!cm_ph_cmd_selected_line(NULL, NULL, gCMID, lineid)) {
           nErr = EFAILED;
         }
      }
      else {
         nErr = AEET_ERR_OP_NOT_ALLOWED;
      }

   }
   else {
      nErr = AEET_ERR_BADDESC;
   }

   return nErr;
#endif
}

int OEMTelephone_EnableLineSwitch( OEMTelephone *pme, boolean bEnable )
{
#ifndef FEATURE_ALS

   return EUNSUPPORTED;
#else
   if( OEMTelephone_IsModeCDMA(OEMTelephone_GetSystemMode(pme)) ) {
      return AEET_ERR_OP_NOT_ALLOWED;
   }

   if (!cm_ph_cmd_line_switching(NULL, NULL, gCMID, 
                                 bEnable ? CM_ALS_LINE_SWITCHING_ALLOWED : CM_ALS_LINE_SWITCHING_NOT_ALLOWED)) {
     return EFAILED;
   }

   return SUCCESS;
#endif
}

int OEMTelephone_GetMaxCallDescriptors( OEMTelephone *pme, uint32 *pdwCount )
{
   if( (uint32 *)0 == pdwCount ) {
      return EBADPARM;
   }

   *pdwCount = CM_CALL_ID_MAX;

   return SUCCESS;
}

boolean OEMCMError_IsDebugMsgOn()
{
   return gOEMCMDebug.bErrorDebugMsg;
}

void OEMCMError_EnableDebugMsg(boolean bOn)
{
   gOEMCMDebug.bErrorDebugMsg = (bOn ? TRUE : FALSE);
}

boolean OEMCMHigh_IsDebugMsgOn()
{
   return gOEMCMDebug.bHighDebugMsg;
}

void OEMCMHigh_EnableDebugMsg(boolean bOn)
{
   gOEMCMDebug.bHighDebugMsg = (bOn ? TRUE : FALSE);
}

boolean OEMCMMed_IsDebugMsgOn()
{
   return gOEMCMDebug.bMedDebugMsg;
}

void OEMCMMed_EnableDebugMsg(boolean bOn)
{
   gOEMCMDebug.bMedDebugMsg = (bOn ? TRUE : FALSE);
}

boolean OEMCMLow_IsDebugMsgOn()
{
   return gOEMCMDebug.bLowDebugMsg;
}

void OEMCMLow_EnableDebugMsg(boolean bOn)
{
   gOEMCMDebug.bLowDebugMsg = (bOn ? TRUE : FALSE);
}

#if defined (FEATURE_ACM)

/*=============================================================================
FUNCTION: ACMInfo_CreateInstance

DESCRIPTION: Create a new instance for APPRESCTL

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int ACMInfo_CreateInstance(ACMInfo * pAcmInfo)
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
FUNCTION: ACMInfo_Acquire

DESCRIPTION: Acquire resources with ACM with specified reasonCode

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int ACMInfo_Acquire(ACMInfo * pAcmInfo, uint32 reasonCode)
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
  num = OEMCM_GetActiveCallIDs(gpTelephone->pcm,
                               (AEECM_CALL_TYPE_VOICE | AEECM_CALL_TYPE_EMERGENCY),
                               (AEECM_CALL_STATE_ORIG | AEECM_CALL_STATE_INCOM | AEECM_CALL_STATE_CONV
                                    | AEECM_CALL_STATE_ONHOLD | AEECM_CALL_STATE_DORMANT),
                               callIDs,
                               sizeof(callIDs));

  MSG_HIGH("ACMInfo_Acquire: Number of active calls = %d", num, 0, 0);

  if (1 == num)
  {
    // Start acquiring resource
    MSG_HIGH("ACMInfo_Acquire: Start acquiring ACM resources", 0, 0, 0);
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
      ACMInfo_Free(pAcmInfo);
      return nRet;
    }

    // Get resource state from ACM state manager
    if((nRet = OEMACM_StateMgr_GetResourceState(pAcmInfo->m_pAppResCtl,&pmgr_status)) != SUCCESS)
    {
      MSG_ERROR("OEMACM_StateMgr_GetResourceState failed with RC=%d",nRet,0,0);
      ACMInfo_Free(pAcmInfo);
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
    }

    if(pmgr_status == ACM_RESOURCE_ACQUIRED)
    {
      // Immediate Acquire
      pAcmInfo->m_uAcmRes_State = ACM_ACQUIRED;
      MSG_ERROR("OEMACM_GetResourceState returned with Immediate Acquire",0,0,0);
    }
    else
    {
      // Delayed Acquire
      pAcmInfo->m_uAcmRes_State = ACM_ACQUIRING;
      MSG_HIGH("OEMACM_GetResourceState returned with Delayed Acquire",0,0,0);
      return SUCCESS;
    }
#else // !FEATURE_ODM_UI
    nRet = OEMACM_PriorityMgr_Acquire (gpTelephone->pcm, reasonCode, &pmgr_status);
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
    nRet = OEMACM_StateMgr_OnStatusChangeNotify(gpTelephone->pcm, pcb,
	                                        &pAcmInfo->m_acm_callback_data_type);

    if(nRet != SUCCESS)
    {
      MSG_ERROR("OEMACM_OnStatusChangeNotify failed with RC=%d",nRet,0,0);
      ACMInfo_Free(pAcmInfo);
      return nRet;
    }

    if(pmgr_status == ACM_RESOURCE_ACQUIRED)
    {
      // Immediate Acquire
      pAcmInfo->m_uAcmRes_State = ACM_ACQUIRED;
      MSG_ERROR("OEMACM_GetResourceState returned with Immediate Acquire",0,0,0);
    }
    else
    {
      // Delayed Acquire
      pAcmInfo->m_uAcmRes_State = ACM_ACQUIRING;
      MSG_HIGH("OEMACM_GetResourceState returned with Delayed Acquire",0,0,0);
      return SUCCESS;
    }
#endif // FEATURE_ODM_UI
  }
  else
  {
    // Call already in progress, so ICM has resource
    pAcmInfo->m_uAcmRes_State = ACM_ACQUIRED;
  }

  MSG_HIGH ("New OEMCM ACM state = %d",pAcmInfo->m_uAcmRes_State,0,0);

  if (pAcmInfo->m_uAcmRes_State == ACM_ACQUIRED)
  {
    // Locals for Network originated calls
    struct ACM_MobileOriginatedCallInfo * ACM_MOCallInfo;

    // Locals for Mobile originated calls
    struct ACM_NetworkOriginatedCallInfo * ACM_NOCallInfo;

    switch (reasonCode)
    {
      case ACM_MOBILE_ORIGINATED_VOICE_CALL:
        // Notify apps
        ACM_MOCallInfo = (ACM_MobileOriginatedCallInfo *)(pAcmInfo->m_pCBInfo);
        MSG_HIGH ("Immediate acquire for MO Call",0,0,0);
        OEMCM_CallOrig_Notify(ACM_MOCallInfo->notify_info);
      break;
      case ACM_NETWORK_ORIGINATED_VOICE_CALL:
        // Invoke CM to answer call
        ACM_NOCallInfo = (ACM_NetworkOriginatedCallInfo *)(pAcmInfo->m_pCBInfo);
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
FUNCTION: ACMInfo_Free

DESCRIPTION: Release resources with ACM for call_id

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int ACMInfo_Free(ACMInfo * pAcmInfo)
{
#if defined (FEATURE_ODM_UI)
  int ret = 0;
#endif // FEATURE_ODM_UI
  uint16 num = 0;
  AEECMCallID  callIDs[OEMCM_MAX_NUM_CALLS];

  MSG_HIGH("ACMInfo_Free: Start freeing ACM resources", 0, 0, 0);

  /* Check if anyone is using the resources. */
  num = OEMCM_GetActiveCallIDs(gpTelephone->pcm,
                               (AEECM_CALL_TYPE_VOICE | AEECM_CALL_TYPE_EMERGENCY),
                               (AEECM_CALL_STATE_CONV | AEECM_CALL_STATE_ONHOLD
                                    | AEECM_CALL_STATE_DORMANT),
                               callIDs,
                               sizeof(callIDs));

  MSG_HIGH("ACMInfo_Free: Number of active calls = %d", num, 0, 0);

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
    OEMACM_PriorityMgr_Free (gpTelephone->pcm);
#endif // FEATURE_ODM_UI

    pAcmInfo->m_uAcmRes_State = ACM_FREE;
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
    ACM_NetworkOriginatedCallInfo * noCBInfo = (ACM_NetworkOriginatedCallInfo *) cbdata->pdata;

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
    ACM_MobileOriginatedCallInfo * moCBInfo = (ACM_MobileOriginatedCallInfo *) cbdata->pdata;
    AEETNotifyInfo * notify_info = moCBInfo->notify_info;
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
static void OEMCM_CallOrig_Notify(AEETNotifyInfo * notify_info)
{
  MSG_HIGH ("OEMCM: Answering MO call from delayed acquire",0,0,0);
  OEMTelephone_Notify(notify_info);
  OEMCM_FREE(notify_info);
}

#endif  // FEATURE_ACM

/* lint -restore */

