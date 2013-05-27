/*=============================================================================
FILE:  OEMAlert.c

SERVICES:  OEM functions for supporting BREW Alert Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW IAlert interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

      OEMALERT_AddRef            Adds a brew reference to the ALERT object.
      OEMALERT_Release           Removes a brew reference to the ALERT object.
      OEMALERT_QueryInterface    Returns a new interface, not yet supported.

      OEMALERT_NOTIFIER_AddRef   Adds a brew reference to the ALERT
                                   Notifier object.

      OEMALERT_NOTIFIER_Release  Removes a brew reference to the ALERT
                                   Notifier object.

      OEMALERT_NOTIFIER_SetMask  Called by Notifier interface to set a mask.


        Copyright ?2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc60x0/apps/StaticExtensions/OEM/Src/OEMAlert.c#2 $
$DateTime: 2006/08/11 09:14:34 $
$Author: johns $
$Change: 362969 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/11/06   jas     Fixing a compiler warning
03/14/06   jas     Lint fixes.
02/17/06   jks     Added CEPT sounds
09/19/05   RI      Fixed the VR abort if VR is not playing.
07/29/05   cvs     Play callwaiting alert in earpiece if in call.
06/10/05   cvs     Support for Smart USB Carkit
05/24/05   aps     code cleanup. updated indentation and if-braces in
                   accordance with coding standards.
05/20/05   aps     changed snd_sound_stop() call to uisnd_snd_stop()
05/19/05   aps     changed snd_sound_id_start() call to uisnd_snd()
03/16/05   SUN     Changed to Multi-Instance
03/08/05   SUN     Fixed Compiler Error
01/07/04   sun     Fixed Compilation Warnings
10/14/04   SUN     Added new alert Signal
09/30/04   GAT     Added capability to get media ringer delay
07/27/04   SUN     Fixed Compilation for Brew 3.0
07/22/04   SUN     Free all memory during OEM_FreeAlert
07/18/04   SUN     Fixed Compile Error
07/07/04   SUN     Code Review Changes
07/01/04   RAG     Add capability for apps to delay media ringers
06/10/04   SUN     Do not Stop Ringer on Call Waiting
06/09/04   SUN     Register with AEE_EXIT
04/16/04   SUN     Do not decline Call if VR fails
04/14/04   SUN     Do not play VR when VR is disabled and TTY is not OFF
04/09/04   SUN     Fixed Lint Errors
04/05/04   BPW     Alert changes for failed calls
03/18/04   SUN     Do not Play the Ringer again
02/17/04   SUN     Fixed Playing of MultiMediaRingers
02/11/04   SUN     Fixed Compile errors
02/01/04   SUN     Added VR functionality back
01/29/04   BPW     Fixed lint errors
01/28/04   SUN     Do not play VR prompts during Incoming call
01/21/04   SUN     Fixed playing of different Ringer Types
12/16/03   BPW     Added base station signaling support
12/03/03   BPW     Linted
12/02/03   sun     Brew 3.0 Related Changes
12/02/03   BPW     Linted
11/26/03   sun     Initial release.

=============================================================================*/
//lint -save -e611
//lint -save -e740
//lint -save -e715  // temporary since it is still in development
//lint -save -e754  // temporary since it is still in development

#include "comdef.h"     // Get DMSS type declarations.
#include "err.h"        // Error functions
#include "uixsnd.h"
#include "AEE.h"         // BREW headers.
#include "AEE_OEM.h"     // Brew OEM Headers
#include "AEEAlert.h"
#include "AEEFile.h"
#include "AEEStdLib.h"
#include "OEMHeap.h"
#include "AEE_OEMDispatch.h"
#include "sndid.h"
#include "OEMNV.h"
#include "OEMCriticalSection.h"
#include "AEEMediaUtil.h"
#include "nv.h"
#include "AEEVR.h"
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "AEEAddrBookExt.h"
#include "OEMVR_priv.h"
#include "AppComFunc.h"
#include "tmc.h"
#include "BREWVersion.h"
#include "AEERinger.h"
#include "db.h"
#include "voc.h"
#include "AEEShell.h"
#include "AEEBacklight.h"
#include "OEMSound.h"

#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif
#include "OEMClassIDs.h"
#include "AEEAnnunciator.h"

#define INCREMENT_ESCALATING_RINGER(vol) \
                           ( (vol) = (OEMSound_Volume_Type)((int)(vol) + 2) )
                           
// How long the phone vibrates for each "ring"
#define TIME_MS_RINGERVIBRATE_DURATION          2000

// How long the phone vibrates for an incoming call, if using the Ring Then
// Vibrate setting (must be divisible evenly by TIME_MS_RINGERVIBRATE_DURATION)

#ifdef FEATURE_VERSION_W317A
#define TIME_MS_RINGERVIBRATE_ALERT_DURATION    4000
#else
#define TIME_MS_RINGERVIBRATE_ALERT_DURATION    12000
#endif

// Twice the number of times the phone should vibrate before starting to ring
// when using the Vibrate Then Ring setting
#define COUNT_VIBRATE_ALERTS_THEN_RING  \
      ( TIME_MS_RINGERVIBRATE_ALERT_DURATION / TIME_MS_RINGERVIBRATE_DURATION )
      
// Time between CAlert_HandleMissedCallTimer() callbacks
#define TIMEOUT_MS_MISSEDCALL_TIMER             (60 * 1000)

// Time between OEMALERT_HandleRingerTimer() callbacks
#define TIMEOUT_MS_RINGERVIB_TIMER     TIME_MS_RINGERVIBRATE_DURATION

// How long the phone vibrates for a SMS alert
#ifdef FEATURE_SMSTONETYPE_MID  
#define TIME_MS_SMSVIBRATE_DURATION             8000
#define COUNT_SMSVIBRATE_ALERTS_THEN_RING       2
#else
#define TIME_MS_SMSVIBRATE_DURATION             2000
#endif //#ifdef FEATURE_SMSTONETYPE_MID  

//The time of Key beep tone play
#define NORMAL_TONE_MS  200
#define LONG_TONE_MS    400


/* Ringer type for the standard alert */
/* It is translated into ringer_type in sound module using the */
/* mapping array uisnd_ringer_snd_map. Make sure both          */
/* uisnd_ringer_type and uisnd_ringer_snd_map are in sync      */
typedef enum {
  OEMALERT_RINGER_NORMAL = 0,          /* Normal alert                          */
  OEMALERT_RINGER_1 = 1,               /* User selectable ring 1                */
  OEMALERT_RINGER_2 = 2,               /* User selectable ring 2                */
  OEMALERT_RINGER_3 = 3,               /* User selectable ring 3                */
  OEMALERT_RINGER_4 = 4,               /* User selectable ring 4                */
  //  Enhanced Rings
  OEMALERT_RINGER_5 = 5,               /* User selectable ring 5                */
  OEMALERT_RINGER_6 = 6,               /* User selectable ring 6                */
  OEMALERT_RINGER_7 = 7,               /* User selectable ring 7                */
  OEMALERT_RINGER_8 = 8,               /* User selectable ring 8                */
  OEMALERT_RINGER_9 = 9,               /* User selectable ring 9                */
  OEMALERT_RINGER_MAX= 0xFF      /* Should be last item in table          */
} OEMALERTRingerType;

typedef struct
{
   uint8  oemAlertRinger;         /* local value of Ringer */
   uint16 sndRinger;              /* Snd Values */
} OEMAlertRingerTableType;

const OEMAlertRingerTableType OEMAlertRingerMap [] =
{
  (uint8)OEMALERT_RINGER_NORMAL , (uint16)SND_NORMAL_ALERT,          /* Normal alert                          */
  (uint8)OEMALERT_RINGER_1 , (uint16)SND_RING_1,               /* User selectable ring 1                */
  (uint8)OEMALERT_RINGER_2 , (uint16)SND_RING_2,               /* User selectable ring 2                */
  (uint8)OEMALERT_RINGER_3 , (uint16)SND_RING_3,               /* User selectable ring 3                */
  (uint8)OEMALERT_RINGER_4 , (uint16)SND_RING_4,               /* User selectable ring 4                */
  //  Enhanced Rings
  (uint8)OEMALERT_RINGER_5 , (uint16)SND_RING_5,               /* User selectable ring 5                */
  (uint8)OEMALERT_RINGER_6 , (uint16)SND_RING_6,               /* User selectable ring 6                */
  (uint8)OEMALERT_RINGER_7 , (uint16)SND_RING_7,               /* User selectable ring 7                */
  (uint8)OEMALERT_RINGER_8 , (uint16)SND_RING_8,               /* User selectable ring 8                */
  (uint8)OEMALERT_RINGER_9 , (uint16)SND_RING_9,               /* User selectable ring 9                */
};
/* The following structure is for the Card object when
 * it is allocated. */

struct IALERT
{
  AEEVTBL(IALERT)         *pvt;               /* Vector table of ALERT methods. */
  AEECallback              m_cbSysObj;
  IShell                  *m_pIShell;         /* Pointer to the IShell object. */
  uint32                   m_uRefs;           /* Number of references to this obj */
  AEECLSID                 m_cls;             /* Class ID of this object. */
  boolean                  m_bMMRinger;        /* Multimedia Ringer is set*/
  char *                   m_ringerFile;      /* MultiMedia Ringer File*/
  AEEALERTType             m_alertType;       /* Current Alert Type*/
  OEMALERTRingerType       m_ringerType;      /* Current Ringer Playing*/
  boolean                  m_bVRPlaying;      /* Identifies if VR is aplying the prompts*/
  uint8                    m_numTimeOut;      /* Timeout value*/
  boolean                  m_bStopRinger;
  AEECallback              m_acbDelayedRingCallback;
  int32                    m_nMediaRingerDelay;
  IMedia*                  m_pMedia;         /* Pointer to IMedia*/
  IVR*                     m_pVR;            /* pointer to IVR*/
  IConfig*                 m_pConfig;        /* pointer to IConfig*/
  void *                   m_pac;        /* App context */
  IAnnunciator *           m_pIAnn;
  
  IRingerMgr              *m_pRingerMgr;
  ISound                  *m_pSound;   
  int32                    m_ringVibCount;   // Used for Vib+Ring ringer type
  OEMSound_Volume_Type     m_ringCurVol;     // Current ringer volume
  OEMSound_Volume_Type     m_ringEndVol;     // Final ringer volume (for escalating)
  uint32                   ring_id; 
  uint32                   CurrRingerId;   
  boolean                  ringer_playing; 
  int                      alert_count;  
  int                      smsID; 
  IBacklight               *m_pBacklight;  
#ifdef FEATURE_ICM
  ICM                      *m_pICM;  
#else
  ITelephone               *m_pITelephone;  
#endif
  ALERT_SND_TYPE   m_snd_type;
  boolean                  m_mp3Ring;
  boolean                  m_iAlert;
};

/* The following structure is for the notifier object when
 * it is allocated. */
struct IALERT_NOTIFIER
{
   AEEVTBL(IALERT_NOTIFIER)  *pvt;         /* Vector table for notifier object. */
   IShell                    *m_pIShell;   /* Pointer to the Ishell object. */
   uint32                     m_uRefs;     /* Number of Notifier references. */
   AEECLSID                   m_cls;       /* Class ID of this object. */
   uint32                     m_dwNotify;  /* Merger of all notify masks. */

};

typedef struct IALERT_CurStatus
{
    IRingerMgr          *m_pCurRingerMgr;
    boolean             m_bplaying; 
}IALERT_CurStatus;

/***********************************************************************
                 * INTERNAL      FUNCTIONS*
 ***********************************************************************/
#ifdef FEATURE_PHONE_VR
void OEMALERT_VrCallback( void* callbackData, AEEVRStatus status,
			  AEEVRFailure failure, AEEVRUtterance utterance);

static boolean GetPhonebookMatchByNumber
  (IALERT *pMe,
  AECHAR *pszNumber,
  char *pszName,
  int *rwid);
#endif

static int OEMALERT_PlayRinger(IALERT *pMe, uint8,AECHAR*,AEEALERTType);
static int OEMALERT_StopRinger(IALERT* pMe);
static void OEMALERT_PlayMediaCallback(IALERT *pMe);
static int OEMALERT_PlayMedia(IALERT *pMe);
static int OEMALERT_PlayMedia2(IALERT *pMe);

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

/* Interface to the Notifier object. */
static uint32 OEMALERT_NOTIFIER_AddRef (IALERT_NOTIFIER *po);
static uint32 OEMALERT_NOTIFIER_Release(IALERT_NOTIFIER *po);
#if MIN_BREW_BUILD(3,1,4,7)
static void   OEMALERT_NOTIFIER_SetMask(IALERT_NOTIFIER *po, uint32 *pdwMask);
#else
static void   OEMALERT_NOTIFIER_SetMask(IALERT_NOTIFIER *po, const uint32 *pdwMask);
#endif // MIN_BREW_BUILD(3,1,4,7)

/* Interface to the Alert object. */
static uint32 OEMALERT_AddRef        (IALERT *po);
static uint32 OEMALERT_Release       (IALERT *po);
static int    OEMALERT_QueryInterface(IALERT *po, AEECLSID id, void **ppNew);
static int    OEMALERT_StartAlerting (IALERT *po, uint8 call_type,
                				      AECHAR *phoneNumber,
                				      AEEALERTType alert_type);
static int    OEMALERT_StopAlerting  (IALERT *po);
static int    OEMALERT_SetMuteCtl    (IALERT *po, boolean muteOn);
static int    OEMALERT_SetRect       (IALERT *po, AEERect *tRect);
static int    OEMALERT_GetRect       (IALERT *po, AEERect *pRect);
static int    OEMALERT_GetInfo       (IALERT *po, uint8 *call_type,
                                      AECHAR *phoneNumber,
                                                                                                                                                        uint16 phoneNumberBufSize,
                                                                                                                                                        AEEALERTType *alert_type) ;
static int    OEMALERT_SetMediaRingerDelay
                                     (IALERT *po, int32 nMediaRingerDelay);
static int    OEMALERT_GetMediaRingerDelay
                                     (IALERT *po, int32 *pnMediaRingerDelay);
                                     
static void   OEMALERT_StartRingerAlert(IALERT *po,uint32 id,ALERT_SND_TYPE type);
static int    OEMALERT_StartMp3Alert(IALERT *po, char* id, ALERT_SND_TYPE type); 
static void   OEMALERT_StopMp3Alert(IALERT *po);
static void   OEMALERT_MediaNotify(void * pUser, AEEMediaCmdNotify * pCmdNotify);
static void   OEMALERT_StopRingerAlert(IALERT *po);                                   
static void   OEMALERT_HandleRingerAlertTimer(void *pUser);
static void   OEMALERT_StartRingerPreview(IALERT *po, AEERingerID ringer);
static void   OEMALERT_StartMp3Preview(IALERT * po, char *id);
static void   OEMALERT_StartMissedCallAlert(IALERT *po);
static void   OEMALERT_StopMissedCallAlert(IALERT *po);
static void   OEMALERT_HandleMissedCallTimer(void *pUser);
static void   OEMALERT_StartSMSAlert(IALERT *po, int ring_id);

#if !defined(FEATURE_SMSTONETYPE_MID)
static void   OEMALERT_HandleSMSTimer(void *pUser);
static void   OEMALERT_HandleMP3Timer(void *pUser);
#endif

static void   OEMALERT_StopSMSAlert(IALERT *po);
static void   OEMALERT_KeyBeep(IALERT *po, AVKType key, boolean bPressed);
static void   OEMALERT_keeybeep_stop(void *pUser);
static AEESoundTone OEMALERT_MapKeyToTone(AVKType key);
static void   OEMALERT_GetRingerVol(IALERT *po);
static void   OEMALERT_SetRingerVol(IALERT *po, boolean bEscalate);
static boolean OEMALERT_InCall(IALERT *po);  
static boolean OEMALERT_RingInHeadset(IALERT *po);

#ifdef FEATURE_APP_MUSICPLAYER
static void OEMALERT_NotifyMP3Player(IALERT *pMe,boolean bStartAlert);
static void OEMALERT_NotifyMP3PlayerCB(IALERT * pMe);
#endif
#ifdef FEATURE_SMSTONETYPE_MID     
static void   OEMALERT_HandleStopRingerTimer(void *pUser);
#endif //#if defined FEATURE_SMSTONETYPE_MID		    

static void   OEMALERT_HandleStopMp3Timer(void *pUser);
static void   OEMALERT_StartSMSAlertPreview(IALERT *po, int ring_id);

static void   OEMALERT_MinuteAlert(IALERT *pMe);

/* The function supported by the NOTIFIER object. */
static const AEEVTBL(IALERT_NOTIFIER) gOEMALERT_NOTIFIERFuncs = {
   OEMALERT_NOTIFIER_AddRef,
   OEMALERT_NOTIFIER_Release,
   OEMALERT_NOTIFIER_SetMask
};

/* The functions supported by this object. */
static const AEEVTBL(IALERT) gOEMALERTFuncs = {
   OEMALERT_AddRef,
   OEMALERT_Release,
   OEMALERT_QueryInterface,
   OEMALERT_StartAlerting,
   OEMALERT_StopAlerting,
   OEMALERT_SetMuteCtl,
   OEMALERT_SetRect,
   OEMALERT_GetRect,
   OEMALERT_GetInfo,
   OEMALERT_SetMediaRingerDelay,
   OEMALERT_GetMediaRingerDelay,
   OEMALERT_StartMp3Alert,
   OEMALERT_StopMp3Alert,
   OEMALERT_StartRingerAlert,
   OEMALERT_StopRingerAlert,
   OEMALERT_StartRingerPreview,  
   OEMALERT_StartSMSAlert,
   OEMALERT_StopSMSAlert,
   OEMALERT_StartMissedCallAlert, 
   OEMALERT_StopMissedCallAlert, 
   OEMALERT_KeyBeep,
   OEMALERT_StartSMSAlertPreview,
   OEMALERT_MinuteAlert,
   OEMALERT_StartMp3Preview,
};



/***********************************************************************
 * D A T A    O B J E C T S
 ***********************************************************************/
static struct IALERT_NOTIFIER *IALERT_NOTIFIERobj = NULL;

static IALERT_CurStatus gCurStatus = {0,};


typedef struct OEMALERT_NOTIFIER_notify_cb
{
  uint8 mask;
  void *user_data;
} OEMCARD_ALERT_NOTIFY_CB;


/*===========================================================================
FUNCTION OEMALERT_DoNotify

DESCRIPTION
  This function will send the EVT_NOTIFY to all registered tasks.  It
  is called via the RESUME callback mechanism and guarentees we are in
  Brew space to send the event.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with the display
===========================================================================*/
void OEMALERT_DoNotify(OEMCARD_ALERT_NOTIFY_CB *pUser)
{
  if(IALERT_NOTIFIERobj != NULL)
    (void) ISHELL_Notify(IALERT_NOTIFIERobj->m_pIShell, AEECLSID_ALERT_NOTIFIER,
          pUser->mask, pUser->user_data);
//lint -save -esym(818, pUser)  Suppress "could be ptr to const"
}
//lint -restore

/*===========================================================================
FUNCTION OEMALERT_SendNotification

DESCRIPTION
  This function will call the Donotify function with the correct information

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMALERT_SendNotification(uint8 mask, boolean data)
{
  static boolean stateInfo = FALSE;

  static OEMCARD_ALERT_NOTIFY_CB notifyData;
  static AEECallback oem_cb;

  stateInfo = data;
  notifyData.mask = mask;
  notifyData.user_data = &stateInfo;

  oem_cb.pfnNotify = (PFNNOTIFY) OEMALERT_DoNotify;
  oem_cb.pNotifyData = &notifyData;
#ifndef WIN32 //wlh
  ISHELL_Resume(IALERT_NOTIFIERobj->m_pIShell, &oem_cb);
#endif
  return SUCCESS;
}

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/


/*===========================================================================
FUNCTION OEMALERT_NOTIFIER_New

DESCRIPTION
  This function is called when a AEEALERT_NOTIFER object needs to
  be created.  Brew makes sure this is done before the SetMask is called.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int OEMALERT_NOTIFIER_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
#ifndef WIN32
#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif
#endif
   // Check parameters.
   if ((!pIShell) || (!ppif)) {
      return EBADPARM;
   }

   if (cls != AEECLSID_ALERT_NOTIFIER) {
      return EUNSUPPORTED;
   }

   /* If the object doesn't already exist, allocate it now. */
   if (IALERT_NOTIFIERobj == NULL)
   {
     // Allocate the object.
//lint -save -e826 Suppress "Suspicious ptr-to-ptr conversion (area too small)
     IALERT_NOTIFIERobj = (IALERT_NOTIFIER*)AEE_NewClassEx((IBaseVtbl*)&
							   gOEMALERT_NOTIFIERFuncs,
							   sizeof(IALERT_NOTIFIER),
							   TRUE);
//lint -restore
     // Allocate the object.
     if (IALERT_NOTIFIERobj == NULL) {
        return ENOMEMORY;
     }

     /* Initialize the values. */

     IALERT_NOTIFIERobj->m_pIShell = pIShell;
     IALERT_NOTIFIERobj->m_cls = cls;
     IALERT_NOTIFIERobj->m_uRefs = 1;


     *ppif = IALERT_NOTIFIERobj;
   }
   else
   {
     (void) OEMALERT_NOTIFIER_AddRef(IALERT_NOTIFIERobj);
     *ppif = IALERT_NOTIFIERobj;
   }

   return AEE_SUCCESS;
}


/*===========================================================================
FUNCTION OEMALERT_NOTIFIER_AddRef

DESCRIPTION
  This function is called when we want to indicate someone else cares
  about the AEEALERT_NOTIFIER object.  We only need one object overall,
  so this just keeps the count.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint32 OEMALERT_NOTIFIER_AddRef(IALERT_NOTIFIER *pMe)
{
  if (pMe) {
    return (++pMe->m_uRefs);
  }
  return 0;
}

/*===========================================================================
FUNCTION OEMALERT_NOTIFIER_Release

DESCRIPTION
  When someone is done with a AEEALERT_NOTIFIER object, this function is
  called.  It decrements the counter and if it reaches 0, frees the memory.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint32 OEMALERT_NOTIFIER_Release(IALERT_NOTIFIER *pMe)
{
   if (!pMe) {
      return (0);
   }

   if (--pMe->m_uRefs != 0) {
      return pMe->m_uRefs;
   }

   // Ref count is zero.  Release memory
   FREE(pMe);
   IALERT_NOTIFIERobj = NULL;
   return (0);
}

/*===========================================================================
FUNCTION OEMALERT_NOTIFIER_SetMask

DESCRIPTION
  This function is called to inform the NOTIFIER what events someone
  cares about.  For now, we send all events supported to BREW, so we
  don't keep track what events are registered.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void OEMALERT_NOTIFIER_SetMask(IALERT_NOTIFIER *po,
#if MIN_BREW_BUILD(3,1,4,7)
                                      uint32 *pdwMask)
#else
                                      const uint32 *pdwMask)
#endif // MIN_BREW_BUILD(3,1,4,7)
{
  return;
//lint -save -e818  Suppress "could be ptr to const"
}
//lint -restore

/*===========================================================================
  FUNCTION OEM_FreeAlert

  DESCRIPTION
    Free all memory

  DEPENDENCIES
    None.

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 ===========================================================================*/
 static void OEM_FreeAlert(IALERT *pMe)
{
   if(pMe == NULL)
     return;

   CALLBACK_Cancel(&pMe->m_acbDelayedRingCallback);
   CALLBACK_Cancel(&pMe->m_cbSysObj);
   
   if (pMe->m_ringerFile) {
     FREE(pMe->m_ringerFile);
     pMe->m_ringerFile = NULL;
   }
   
   if (pMe->m_pMedia) 
   {
     (void) IMEDIA_Release(pMe->m_pMedia);
     pMe->m_pMedia = NULL;
   }

#ifdef FEATURE_PHONE_VR
   if (pMe->m_pVR) 
   {
     (void)IVR_Release(pMe->m_pVR);
     pMe->m_pVR = NULL;
   }
#endif

   if (pMe->m_pConfig) 
   {
     (void) ICONFIG_Release(pMe->m_pConfig);
     pMe->m_pConfig = NULL;
   }

   if (pMe->m_pRingerMgr) 
   {
     (void) IRINGERMGR_Release(pMe->m_pRingerMgr);
     
     if (gCurStatus.m_pCurRingerMgr == pMe->m_pRingerMgr )
     {
         gCurStatus.m_bplaying = FALSE;
         gCurStatus.m_pCurRingerMgr = NULL;
     }
     pMe->m_pRingerMgr= NULL;
   }   

   if (pMe->m_pSound) 
   {
     (void) ISOUND_Release(pMe->m_pSound);
     pMe->m_pSound= NULL;
   }  
      
   if (pMe->m_pBacklight) 
   {
     (void) IBACKLIGHT_Release(pMe->m_pBacklight);
     pMe->m_pBacklight= NULL;
   }     
#ifdef FEATURE_ICM
   if (pMe->m_pICM) 
   {
     (void) ICM_Release(pMe->m_pICM);
     pMe->m_pICM= NULL;
   }
#else
   if (pMe->m_pITelephone) 
   {
     (void) ITELEPHONE_Release(pMe->m_pITelephone);
     pMe->m_pITelephone= NULL;
   }    
#endif
   if (pMe->m_pIAnn) 
   {
       IANNUNCIATOR_Release(pMe->m_pIAnn);
       pMe->m_pIAnn= NULL;
   }

   FREE(pMe);
   pMe= NULL;
}

/*===========================================================================
FUNCTION OEMALERT_New

DESCRIPTION
  This function is called to create a new Supplementary Services object.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int OEMALERT_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
  IALERT *pNew;
   // Check parameters.
  if ((!pIShell) || (!ppif)) {
       return EBADPARM;
  }

  if (cls != AEECLSID_ALERT) {
       return EUNSUPPORTED;
  }

  // Allocate the object.
//lint -save -e826  Suppress "suspicious ptr-to-ptr conversion (area too small)"
  pNew = (IALERT *) AEE_NewClass((IBaseVtbl*)&gOEMALERTFuncs, sizeof(IALERT));
//lint -restore
  if (pNew == NULL)
       return ENOMEMORY;

  /* Initialize the values. */
  pNew->m_pIShell = pIShell;
  pNew->m_cls = cls;
  pNew->m_uRefs = 1;
  pNew->m_ringerFile = NULL;
  pNew->m_nMediaRingerDelay = 0;


  pNew->m_pMedia = NULL;
  pNew->m_pVR = NULL;
  pNew->m_pConfig = NULL;
  pNew->m_pSound = NULL;
  pNew->m_pRingerMgr = NULL;
  pNew->m_snd_type = ALERT_NORMAL_SND;
  pNew->m_iAlert = FALSE;
#ifdef FEATURE_PHONE_VR
/* IVR object should be available*/
  if(ISHELL_CreateInstance(pNew->m_pIShell, AEECLSID_VR,
			   (void **)&pNew->m_pVR) != SUCCESS) 
  {
      OEM_FreeAlert(pNew);
      return EFAILED;
  }
#endif
  if (ISHELL_CreateInstance(pNew->m_pIShell, AEECLSID_CONFIG,
			    (void **) &pNew->m_pConfig)
      != SUCCESS) 
  {
     OEM_FreeAlert(pNew);
     return EFAILED;
  }

  if (ISHELL_CreateInstance(pNew->m_pIShell, AEECLSID_SOUND,
			    (void **) &pNew->m_pSound)
      != SUCCESS) 
  {
     OEM_FreeAlert(pNew);
     return EFAILED;
  }
  
  if (ISHELL_CreateInstance(pNew->m_pIShell, AEECLSID_RINGERMGR,
			    (void **) &pNew->m_pRingerMgr)
      != SUCCESS) 
  {
     OEM_FreeAlert(pNew);
     return EFAILED;
  }  
  
  if (AEE_SUCCESS != ISHELL_CreateInstance(pNew->m_pIShell,
                                           AEECLSID_BACKLIGHT,
                                           (void **)&pNew->m_pBacklight))
  {
     OEM_FreeAlert(pNew);
     return EFAILED;
  }     
#ifdef FEATURE_ICM
  if (AEE_SUCCESS != ISHELL_CreateInstance(pNew->m_pIShell,
                                           AEECLSID_CM,
                                           (void **)&pNew->m_pICM))
  {
     OEM_FreeAlert(pNew);
     return EFAILED;
  }       
#else
  if (AEE_SUCCESS != ISHELL_CreateInstance(pNew->m_pIShell,
                                           AEECLSID_TELEPHONE,
                                           (void **)&pNew->m_pITelephone))
  {
     OEM_FreeAlert(pNew);
     return EFAILED;
  }    
#endif
  if (AEE_SUCCESS != ISHELL_CreateInstance(pNew->m_pIShell, AEECLSID_ANNUNCIATOR, 
          (void **) &pNew->m_pIAnn))
  {
      OEM_FreeAlert(pNew);
      return EFAILED;
  }
  pNew->m_pac = AEE_GetAppContext();

  CALLBACK_Init(&pNew->m_cbSysObj, OEM_FreeAlert, pNew);
  AEE_LinkSysObject(&pNew->m_cbSysObj);

  *ppif = pNew;

  return AEE_SUCCESS;

}

/*===========================================================================
FUNCTION OEMALERT_AddRef

DESCRIPTION
  This function is called to add an additional reference to the SuppSvc
  object.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint32 OEMALERT_AddRef(IALERT *pMe)
{
  if (pMe) {
    return (++pMe->m_uRefs);
  }
  return 0;
}

/*===========================================================================
FUNCTION OEMALERT_Release

DESCRIPTION
  This function is called when a reference is removed.  It decrements
  the count and if the count reaches 0, it frees the memory.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint32 OEMALERT_Release(IALERT *pMe)
{
  if (!pMe) {
    return (0);
  }
  
  if(pMe->m_uRefs)
  {
      if (--pMe->m_uRefs != 0) {
        return pMe->m_uRefs;
      }
  }
  else
  {
    return (0);
  }

  // 释放与 pMe 相关的全部定时器
  (void) ISHELL_CancelTimer(pMe->m_pIShell, NULL, pMe);

  OEM_FreeAlert(pMe);
  return 0;
}


/*===========================================================================
FUNCTION OEMALERT_QueryInterface

DESCRIPTION
  This function is called to provide the QueryInterface method.
  Right now, we only support them creating a ALERT object from
  this method.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_QueryInterface(IALERT *pMe, AEECLSID id,
                                  void **ppNew)
{
   int ret_val = AEE_SUCCESS;

   if ((!pMe) || (!ppNew)) {
      return EBADPARM;
   }

   switch (id) {
      case AEECLSID_ALERT:
         (void) IALERT_AddRef(pMe);
         *ppNew = pMe;
         break;
      default:
         *ppNew = NULL;
         ret_val = ECLASSNOTSUPPORT;
         break;
   }

   return ret_val;
}


/*===========================================================================
FUNCTION OEMALERT_SetMediaRingerDelay

DESCRIPTION
  Sets the amount of time from when IALERT_StartAlerting() is called to
  when ringing begins if the ringer is a multimedia ringer.  Some BREW
  applications need extra time to release all their resources when an
  incoming call occurs.  Those BREW applications can call this function
  to set a delay so that the phone will not try to ring (and fail) before
  it is ready.  Those applications should set the delay back to 0 before
  they exit.  There is generally not a resource conflict with the
  piezoelectric ringer, so it is not delayed if that is the ringer that
  is selected.

DEPENDENCIES
  None.

RETURN VALUE
  AEE_SUCCESS

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_SetMediaRingerDelay(IALERT *po, int32 nMediaRingerDelay)
{
  if (!po) return EBADPARM;
  po->m_nMediaRingerDelay = nMediaRingerDelay;
  return AEE_SUCCESS;
}

/*===========================================================================
FUNCTION OEMALERT_GetMediaRingerDelay

DESCRIPTION
  Gets the amount of time from when IALERT_StartAlerting() is called to
  when ringing begins if the ringer is a multimedia ringer.  See
  OEMALERT_SetMediaRingerDelay description for details and rationale for
  the delay.

DEPENDENCIES
  None.

RETURN VALUE
  AEE_SUCCESS

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_GetMediaRingerDelay(IALERT *po, int32 *pnMediaRingerDelay)
{
  if (!po || !pnMediaRingerDelay) return EBADPARM;
  *pnMediaRingerDelay = po->m_nMediaRingerDelay;
  return AEE_SUCCESS;
//lint -save -esym(818, po)  Suppress "could be ptr to const"
}
//lint -restore


/*===========================================================================
FUNCTION OEMALERT_StartAlerting

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_StartAlerting
(
  IALERT *pMe,
  uint8 callType,
  AECHAR* phoneNumber,
  AEEALERTType alertType
)
{
 int nRetVal;


#ifdef FEATURE_PHONE_VR
  uint32 device = 0;
  boolean playPrompt =FALSE;
  char nameTag[40];
  boolean vrEnabled = FALSE;
#ifdef FEATURE_TTY
  OEMTTY tty = OEMNV_TTY_OFF;
#endif


  OEMVRSound sndArray[] =
  {
    { AEEVRUTTERANCE_ID_CALLING, NULL },
    { AEEVRUTTERANCE_NONE, NULL },
    { AEEVRUTTERANCE_NONE, NULL }
  };

  int rwid = 0;

  if(NULL == pMe)
  {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return EBADPARM;
  }

  pMe->m_bStopRinger = FALSE;
  pMe->m_alertType = alertType;

  if((alertType == AEEALERT_ALERT_NORMAL) ||
     (alertType == AEEALERT_ALERT_CALLWAITING))
  {
    pMe->m_bVRPlaying = FALSE;
    /*VR prompts are available. Check if VR prompts should be played*/
    if(ICONFIG_GetItem(pMe->m_pConfig, CFGI_VR, &vrEnabled,sizeof(boolean)) == SUCCESS)
    {
       if(vrEnabled && (ICONFIG_GetItem(pMe->m_pConfig, CFGI_DEVICE,
					&device,sizeof(uint32)) == SUCCESS)) {
      // Check for headset device and HFK
      if(device &
	 (OEMNV_STEREO_HEADSET_MASK|OEMNV_HEADSET_MASK|
	  OEMNV_ANALOG_HFK_MASK|OEMNV_USB_HFK_ROUTE_AUDIO_MASK)) {
#ifdef FEATURE_TTY
           //Check to see if TTY is disabled.
            if(ICONFIG_GetItem(pMe->m_pConfig, CFGI_TTY,
			       &tty,sizeof(OEMTTY)) == SUCCESS) {
              if(tty == OEMNV_TTY_OFF)
                 playPrompt = TRUE;
            }
#else
            playPrompt = TRUE;
#endif
         }
       }
    }

    if(playPrompt && pMe->m_pVR) {
      pMe->m_bVRPlaying = TRUE;
      MSG_MED("VR is playing the prompt",0,0,0);

      if(GetPhonebookMatchByNumber(pMe, phoneNumber, nameTag,&rwid))
      {
	  sndArray[1].wUtt = (AEEVRUtterance)(AEEVRUTTERANCE_TYPE_VOICETAG |
					      (uint16)rwid);
      }
      (void) IVR_RegisterNotify(pMe->m_pVR,(PFNVRSTATUS)OEMALERT_VrCallback,pMe);
      (void) IVR_PlayUtterance(pMe->m_pVR,AEEVRUTTERANCE_ID_INCOMING_CALL_ANSWER);
      (void) OEMALERT_SendNotification(NMASK_ALERT_ONOFF, TRUE);
      return SUCCESS;
    }
  }

#endif

  pMe->m_bStopRinger = FALSE;
  nRetVal = OEMALERT_PlayRinger(pMe, callType, phoneNumber, alertType);
  if(nRetVal != SUCCESS) {
    return nRetVal;
  }

  (void) OEMALERT_SendNotification(NMASK_ALERT_ONOFF, TRUE);

  return SUCCESS;
}

/*===========================================================================
FUNCTION OEMALERT_StopAlerting

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_StopAlerting
(
  IALERT *pMe
)
{
  int nRetVal;

  if(NULL == pMe) {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return EBADPARM;
  }

  pMe->m_bStopRinger = TRUE;
  nRetVal = OEMALERT_StopRinger(pMe);

  if(nRetVal != SUCCESS) {
    return nRetVal;
  }

#ifdef FEATURE_PHONE_VR
  if( (pMe->m_pVR) && (pMe->m_bVRPlaying) ) {
    nRetVal = IVR_AbortOperation(pMe->m_pVR);
    if(nRetVal != SUCCESS) {
      return nRetVal;
    }
  }
#endif

  pMe->m_alertType = AEEALERT_ALERT_NONE;

  (void) OEMALERT_SendNotification(NMASK_ALERT_ONOFF, FALSE);

  return SUCCESS;
}

/*===========================================================================
FUNCTION OEMALERT_SetMuteCtl

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_SetMuteCtl
(
  IALERT *pMe,
  boolean muteOn
)
{

  if(NULL == pMe) {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return EBADPARM;
  }

  if(muteOn){

  #ifdef FEATURE_PHONE_VR
    if(pMe->m_pVR)
      (void) IVR_AbortOperation(pMe->m_pVR);
  #endif

    if (pMe->m_pMedia) {
        (void) IMEDIA_Stop(pMe->m_pMedia);
        (void) IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }
#ifndef WIN32
    uisnd_snd_stop();
#endif
  }
  (void) OEMALERT_SendNotification(NMASK_ALERT_MUTED, muteOn);
  return SUCCESS;
}

/*===========================================================================
FUNCTION OEMALERT_SetRect

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_SetRect
(
  IALERT *po,
  AEERect *tRect
)
{
  return EUNSUPPORTED;
//lint -save -e818  Suppress "could be ptr to const"
}
//lint -restore

/*===========================================================================
FUNCTION OEMALERT_GetRect

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_GetRect
(
  IALERT *po,
  AEERect *pRect
)
{
  return EUNSUPPORTED;
//lint -save -esym(818, pRect)  Suppress "could be ptr to const"
}
//lint -restore

/*===========================================================================
FUNCTION OEMALERT_GetInfo

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_GetInfo
(
  IALERT *po,
  uint8 *callType,
  AECHAR *phoneNumber,
  uint16 phoneNumberBufSize,
  AEEALERTType *alert_type
)
{
  if(NULL == po) {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return EBADPARM;
  }

  if(callType != NULL) {
    //TODO: return call type
  }

  if(phoneNumberBufSize > 0) {
    //TODO: phone number
  }

  if(alert_type != NULL) {
    *alert_type = po->m_alertType;
  }

  return SUCCESS;
//lint -save -e818 Suppress "could be ptr to const"
}
//lint -restore


/*===========================================================================
FUNCTION OEMALERT_GetRingerType

DESCRIPTION
  Get ringer type from NV, update current static variable

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
static int  OEMALERT_GetRingerType
(
 IALERT *pMe,
 OEMALERTRingerType *ringerType
)
{
#ifndef WIN32
  //lint -save -esym(550, ringerType)
  nv_item_type nvi;
  nv_stat_enum_type result;

  if(NULL == pMe) {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return EBADPARM;
  }

#ifndef USES_MMI  
  result = ui_get_nv(NV_RINGER_TYPE_I, &nvi);
#else
  result = OEMNV_Get(NV_RINGER_TYPE_I, &nvi);
#endif  

  if(result == NV_DONE_S) { /* successfully obtain from NV */
    /* range checking */
    if (nvi.ringer_type >=(uint8) OEMALERT_RINGER_MAX) {
        nvi.ringer_type = (uint8) OEMALERT_RINGER_NORMAL;
#ifndef USES_MMI  
      (void)ui_put_nv (NV_RINGER_TYPE_I, &nvi);
#else      
      (void)OEMNV_Put(NV_RINGER_TYPE_I, &nvi);
#endif  
    }

    *ringerType = (OEMALERTRingerType)nvi.ringer_type;

#ifndef USES_MMI  
    result = ui_get_nv (NV_MM_RINGER_FILE_I, &nvi);
#else      
    result = OEMNV_Get(NV_MM_RINGER_FILE_I, &nvi);
#endif  

    if (result == NV_DONE_S) {
      if (pMe->m_ringerFile) {
        FREE(pMe->m_ringerFile);
        pMe->m_ringerFile = NULL;
      }

#ifdef FEATURE_MULTIMEDIA
      if (MEMCMP ((void *) nvi.mm_ringer_file, "Std",3) != 0) {
        pMe->m_ringerFile = MALLOC(AEE_MAX_FILE_NAME*sizeof(char));
        (void) MEMCPY (pMe->m_ringerFile, (void *) nvi.mm_ringer_file,
		       sizeof (nvi.mm_ringer_file));
        pMe->m_bMMRinger = TRUE;
      }
      else
#endif /* FEATURE_MULTIMEDIA */
      {
        pMe->m_bMMRinger = FALSE;
      }

    }
    return SUCCESS;
  }

  return EFAILED;
#else
	return SUCCESS;
#endif
} /*OEMALERT_GetRingerType*/

#ifdef FEATURE_PHONE_VR
/*===========================================================================
FUNCTION OEMALERT_YesNoCallback

DESCRIPTION
  Find out if the user answer the call or rejected it

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
void OEMALERT_YesNoCallback
(
  void* pUser,
  AEEVRStatus status,
  AEEVRFailure failure,
  AEEVRUtterance utterance
)
{
    IALERT *pMe = (IALERT*)pUser;

    if(NULL == pMe) {
      MSG_ERROR("Invalid Pointer",0,0,0);
      return;
    }

    if (failure != AEEVRFAILURE_NONE) {
      if(failure == AEEVRFAILURE_SILENCE) {  //timeout
            pMe->m_numTimeOut++;
	if(pMe->m_numTimeOut>=2) {
                pMe->m_numTimeOut = 0;
                (void) IVR_AbortOperation(pMe->m_pVR);
	  (void) OEMALERT_PlayRinger(pMe, AEEALERT_CALLTYPE_VOICE,
				     NULL, pMe->m_alertType);
            }
        }
    }
    else //if(f == AEEVRFAILURE_NONE)//reset number of timeout
    {
        pMe->m_numTimeOut = 0;
    }


   switch(status){
     case AEEVRSTATUS_ABORTED:    //        VR operation aborted by IVR_AbortOperation.
     case AEEVRSTATUS_FAILED:     //
       MSG_MED("VR Aborted",0,0,0);
       /*VR Failed, Play the regular ringer*/
       (void) IVR_AbortOperation(pMe->m_pVR);
       break;

    case AEEVRSTATUS_RESULT_YES:
      (void) AnswerVoiceCall(TRUE);
      (void) OEMALERT_SendNotification(NMASK_ALERT_ANSWER, TRUE);
      break;

    case AEEVRSTATUS_RESULT_NO:
      (void) AnswerVoiceCall(FALSE);
      (void) OEMALERT_SendNotification(NMASK_ALERT_ANSWER, FALSE);
      break;

    case AEEVRSTATUS_DONE:
       (void) IVR_YesNoConfirm(pMe->m_pVR);
       break;

    default:
      break;
   }

}


void OEMALERT_VrCallback
(
  void* pUser,
  AEEVRStatus status,
  AEEVRFailure failure,
  AEEVRUtterance utterance
)
{
   IALERT *pMe = (IALERT*)pUser;

   if(NULL == pMe) {
     MSG_ERROR("Invalid Pointer",0,0,0);
     return;
   }

   switch(status)
   {
     case AEEVRSTATUS_DONE:       //        VR operation completed successfully.
       (void) IVR_RegisterNotify(pMe->m_pVR,(PFNVRSTATUS)OEMALERT_YesNoCallback,pMe);
       (void) IVR_YesNoConfirm(pMe->m_pVR);

       break;

     case AEEVRSTATUS_FAILED:     //        VR operation failed.
     case AEEVRSTATUS_ABORTED:    //        VR operation aborted by IVR_AbortOperation.
       MSG_MED("VR Failed with Status = %d",status,0,0);
       /*VR Failed, Play the regular ringer*/
       (void) OEMALERT_PlayRinger(pMe,AEEALERT_CALLTYPE_VOICE, NULL, pMe->m_alertType);
       break;

     default:
       break;
   }
}

#endif


/*===========================================================================
FUNCTION OEMALERT_PlayMedia

DESCRIPTION
  This function determines whether a delay has been set or not, and either
  calls OEMALERT_PlayMedia2 directly, or sets up a timer and callback
  structure for it to be called later.

DEPENDENCIES
  None

RETURN VALUE
  If no delay is set, it will be the return value of OEMALERT_PlayMedia2().
  If a delay is set, it will always be SUCCESS.  We don't know what the
  ultimate return value of OEMALERT_PlayMedia2() will be because it hasn't
  been called yet.  One idea would be to have the caller pass in a callback
  pointer to get the return value.  But the whole idea was not to change
  the interface, and hide from the caller that delays are even taking
  place.  So this is a least-of-evils solution.

SIDE EFFECTS
  May set up a brew timer

===========================================================================*/
static int OEMALERT_PlayMedia(IALERT *pMe)
{
  IShell * pIShell;

  if(NULL == pMe) {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return EBADPARM;
  }

  pIShell = AEE_GetShell();
  if (NULL == pIShell) {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return EBADPARM;
  }

  if (pMe->m_nMediaRingerDelay > 0) {
    CALLBACK_Init(&pMe->m_acbDelayedRingCallback,
                  OEMALERT_PlayMediaCallback,
                  pMe);
    (void)ISHELL_SetTimerEx(pIShell, pMe->m_nMediaRingerDelay,
                            &pMe->m_acbDelayedRingCallback);
    return SUCCESS;
  }
  else
  {
    return OEMALERT_PlayMedia2(pMe);
  }
}


/*===========================================================================
FUNCTION OEMALERT_PlayMediaCallback

DESCRIPTION
  This is the call back function used in the callback structure for the
  brew timer.  The reason OEMALERT_PlayMedia2 isn't used directly here
  is that the prototype isn't right.  I didn't want to change the prototype
  for OEMALERT_PlayMedia2 because I still wanted to return a return
  value in the case there is no delay and it is not called from a timer
  callback.  So this little wrapper function was necessary.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void OEMALERT_PlayMediaCallback(IALERT *pMe)
{
  if(NULL == pMe) {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return;
  }

  (void) OEMALERT_PlayMedia2(pMe);
}


/*===========================================================================
FUNCTION OEMALERT_PlayMedia2(IALERT *pMe)

DESCRIPTION
  This is the function that does the real work of finding the media
  that the user has selected for the ringer, setting up the IMEDIA
  object, and playing it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_PlayMedia2(IALERT *pMe)
{
    AEEMediaData mediaData;

    if(NULL == pMe) {
      MSG_FATAL("Invalid Pointer",0,0,0);
      return EBADPARM;
    }

    if(pMe->m_pMedia) {
      (void) IMEDIA_Release(pMe->m_pMedia);
      pMe->m_pMedia = NULL;
    }
//	pMe->m_ringerFile = MALLOC(AEE_MAX_FILE_NAME*sizeof(char));
	
//	(void)STRCPY(pMe->m_ringerFile,"1fs:/hsmm/music/midi_06.mp3");

    mediaData.clsData = MMD_FILE_NAME;
    mediaData.pData = (void *)STRDUP(pMe->m_ringerFile);
    mediaData.dwSize = 0;
	MSG_FATAL("OEMALERT_PlayMedia2.....",0,0,0);
    if(AEEMediaUtil_CreateMedia(pMe->m_pIShell, &mediaData,
				&pMe->m_pMedia) == SUCCESS) {
		MSG_FATAL("IMEDIA_Play",0,0,0);		
//	(void)IMEDIA_SetVolume(pMe->m_pMedia,5); 
    (void) IMEDIA_Play(pMe->m_pMedia);
    }
    else
    {
      MSG_FATAL("IMEDIA could not be created",0,0,0);
      return EFAILED;
    }
    return SUCCESS;
}

/*===========================================================================
FUNCTION OEMALERT_PlayRinger

DESCRIPTION
  Plays the actual ringer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_PlayRinger
(
  IALERT *pMe,
  uint8 callType,
  AECHAR* phoneNumber,
  AEEALERTType alertType
)
{

  if(NULL == pMe) {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return EBADPARM;
  }

  if(pMe->m_bStopRinger)
    return SUCCESS;

  if(pMe->m_alertType != AEEALERT_ALERT_NONE) {
    // need to stop current sound before playing a new one
    (void) OEMALERT_StopRinger(pMe);
  }

  pMe->m_bVRPlaying = FALSE;

  /*Save the Alert Type for Future Use*/
  pMe->m_alertType = alertType;

  switch(alertType) {
  case AEEALERT_ALERT_NORMAL:
    {
      if(OEMALERT_GetRingerType(pMe, &pMe->m_ringerType) == EFAILED) {
        MSG_ERROR("Could not get Ringer from NV",0,0,0);
        /*Set Default ringer Type*/
        pMe->m_ringerType = OEMALERT_RINGER_NORMAL;
      }

      if(pMe->m_bMMRinger) {
         return OEMALERT_PlayMedia(pMe);
      }

      switch(pMe->m_ringerType) {
        /*Basic Ringers, call the sound task directly*/
      case OEMALERT_RINGER_NORMAL:
      case OEMALERT_RINGER_1:
      case OEMALERT_RINGER_2:
      case OEMALERT_RINGER_3:
      case OEMALERT_RINGER_4:
      case OEMALERT_RINGER_5:
      case OEMALERT_RINGER_6:
      case OEMALERT_RINGER_7:
      case OEMALERT_RINGER_8:
      case OEMALERT_RINGER_9:
#ifndef WIN32
        uisnd_snd( UI_ALERT_SND,
		   OEMAlertRingerMap[pMe->m_ringerType].sndRinger);
#endif
        break;

      default:
        MSG_ERROR("IMEDIA could not be created",0,0,0);
        return EFAILED;
      }
    }
    break;

  case AEEALERT_ALERT_CALLWAITING:
#ifdef FEATURE_CEPT
    uisnd_snd(UI_MSG_SND, (uint32)SND_CEPT_CALL_WAITING );
#else
    uisnd_snd(UI_MSG_SND, (uint32)SND_CALL_WAITING );
#endif
    break;

  case AEEALERT_ALERT_CALLWAIT_SIGNAL:
#ifdef FEATURE_CEPT
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_CEPT_CALL_WAITING );
#else
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_CALL_WAITING );
#endif
    break;

  case AEEALERT_ALERT_DIALTONE:
#ifdef FEATURE_CEPT
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_CEPT_DIAL_TONE );
#else
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_DIAL_TONE );
#endif
    break;

  case AEEALERT_ALERT_RINGBACK:
#ifdef FEATURE_CEPT
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_CEPT_RING );
#else
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_RING_BACK );
#endif
    break;

  case AEEALERT_ALERT_BUSY:
#ifdef FEATURE_CEPT
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_SUBSCRIBER_BUSY );
#else
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_BUSY_ALERT );
#endif
    break;

  case AEEALERT_ALERT_ANSWER:
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_ANSWER );
    break;

  case AEEALERT_ALERT_INTERCEPT:
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_INTERCEPT );
    break;

  case AEEALERT_ALERT_ABBR_INTERCEPT:
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_ABRV_INTERCEPT );
    break;

  case AEEALERT_ALERT_REORDER:
#ifdef FEATURE_CEPT
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_CONGESTION );
#else
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_REORDER );
#endif
    break;

  case AEEALERT_ALERT_ABBR_REORDER:
#ifdef FEATURE_CEPT
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_CONGESTION );
#else
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_ABRV_REORDER );
#endif
    break;

  case AEEALERT_ALERT_ABBR_ALERT:
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_ABRV_ALERT );
    break;

  case AEEALERT_ALERT_FADE_TONE:
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_FADE_TONE );
    break;

  case AEEALERT_ALERT_SPECIAL_INFO:
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_SPECIAL_INFO );
    break;

  case AEEALERT_ALERT_CONFIRM:
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_CONFIRMATION );
    break;

  case AEEALERT_ALERT_PIP:
    uisnd_snd(UI_CALL_EAR_SND, (uint32)SND_PIP_TONE );
    break;

  case AEEALERT_ALERT_ISDN_NORMAL:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_NORMAL_ALERT );
    break;

  case AEEALERT_ALERT_ISDN_INTERGROUP:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_INTR_GROUP_ALERT );
    break;

  case AEEALERT_ALERT_ISDN_SPECIAL:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_SPCL_ALERT );
    break;

  case AEEALERT_ALERT_ISDN_PING:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_PING_RING );
    break;

  case AEEALERT_ALERT_IS54B_LONG_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_LONG_M_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_LONG_H:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_LONG_H_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_LONG_L:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_LONG_L_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SS_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SS_M_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SS_H:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SS_H_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SS_L:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SS_L_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SSL_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SSL_M_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SSL_H:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SSL_H_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SSL_L:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SSL_L_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SS2_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SS2_M_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SS2_H:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SS2_H_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SS2_L:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SS2_L_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SLS_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SLS_M_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SLS_H:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SLS_H_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SLS_L:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SLS_L_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SSSS_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SSSS_M_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SSSS_H:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SSSS_H_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_SSSS_L:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_SSSS_L_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_LONG_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_LONG_M_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_LONG_H:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_LONG_H_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_LONG_L:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_LONG_L_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_SS_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_SS_M_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_SS_H:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_SS_H_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_SSL_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_SSL_M_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_SSL_H:
    uisnd_snd(UI_ALERT_SND,(uint32)SND_IS54B_PBX_SSL_H_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_SSL_L:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_SSL_L_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_SLS_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_SLS_M_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_SLS_H:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_SLS_H_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_SLS_L:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_SLS_L_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_SSSS_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_SSSS_M_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_SSSS_H:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_SSSS_H_ALERT );
    break;

  case AEEALERT_ALERT_IS54B_PBX_SSSS_L:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS54B_PBX_SSSS_L_ALERT );
    break;

  case AEEALERT_ALERT_IS53A_PPPP_M:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS53A_PPPP_M_ALERT );
    break;

  case AEEALERT_ALERT_IS53A_PPPP_H:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS53A_PPPP_H_ALERT );
    break;

  case AEEALERT_ALERT_IS53A_PPPP_L:
    uisnd_snd(UI_ALERT_SND, (uint32)SND_IS53A_PPPP_L_ALERT );
    break;
    
  case AEEALERT_ALERT_MINUTE_BEEP:
#ifndef WIN32
    OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                            SND_METHOD_VOICE,
                            SND_MESSAGE_ALERT,
                            SND_PRIO_MED,
                            SND_APATH_LOCAL,
                            NULL);  
#endif
    break;

  case AEEALERT_ALERT_LOW_BATTERY: 
    IBACKLIGHT_Enable(pMe->m_pBacklight);
    OEMALERT_GetRingerVol(pMe);
    #ifdef FEATURE_VERSION_C337
    pMe->m_ringCurVol = OEMSOUND_1ST_VOL;
    pMe->m_ringEndVol = OEMSOUND_1ST_VOL;
    #endif
    OEMALERT_SetRingerVol(pMe, FALSE);
    OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                           (OEMALERT_InCall(pMe) || OEMALERT_RingInHeadset(pMe)) ?
                                      SND_METHOD_VOICE : SND_METHOD_RING,
                           (int) SND_MESSAGE_ALERT,
                           SND_PRIO_MED,
                           SND_APATH_LOCAL,
                           NULL);  
    break;      
    
  case AEEALERT_ALERT_CHARGER_OVERVOLTAGE: 
    IBACKLIGHT_Enable(pMe->m_pBacklight);
    OEMALERT_GetRingerVol(pMe);
    OEMALERT_SetRingerVol(pMe, FALSE);
    OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                           (OEMALERT_InCall(pMe) || OEMALERT_RingInHeadset(pMe)) ?
                                      SND_METHOD_VOICE : SND_METHOD_RING,
                           (int) SND_MESSAGE_ALERT,
                           SND_PRIO_MED,
                           SND_APATH_LOCAL,
                           NULL);                             
    break;        
   
  case AEEALERT_ALERT_KEYGUARD_ENABLED:   
    IBACKLIGHT_Enable(pMe->m_pBacklight);
    OEMALERT_GetRingerVol(pMe);
    OEMALERT_SetRingerVol(pMe, FALSE);
    OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                           (OEMALERT_InCall(pMe) || OEMALERT_RingInHeadset(pMe)) ?
                                      SND_METHOD_VOICE : SND_METHOD_RING,
                           (int) SND_MESSAGE_ALERT,
                           SND_PRIO_MED,
                           SND_APATH_LOCAL,
                           NULL);    
    break;  
    
  case AEEALERT_ALERT_ROAMING: 
   IBACKLIGHT_Enable(pMe->m_pBacklight); 
   OEMALERT_GetRingerVol(pMe);
   OEMALERT_SetRingerVol(pMe, FALSE);
   OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                         (OEMALERT_InCall(pMe) || OEMALERT_RingInHeadset(pMe)) ?
                                     SND_METHOD_VOICE : SND_METHOD_RING,
                          (int) SND_SVC_CHNG_LESS_PREF,
                          SND_PRIO_MED,
                          SND_APATH_LOCAL,
                          NULL); 
    break;  
    
  case AEEALERT_ALERT_EXTERN_PWRON: 
    IBACKLIGHT_Enable(pMe->m_pBacklight); 
    OEMALERT_GetRingerVol(pMe);
    OEMALERT_SetRingerVol(pMe, FALSE);
    OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                           (OEMALERT_InCall(pMe) || OEMALERT_RingInHeadset(pMe)) ?
                                      SND_METHOD_VOICE : SND_METHOD_RING,
                           (int) SND_EXT_PWR_ON,
                           SND_PRIO_MED,
                           SND_APATH_LOCAL,
                           NULL);    
    break;  
    
  case AEEALERT_ALERT_EXTERN_PWROFF: 
    IBACKLIGHT_Enable(pMe->m_pBacklight); 
    OEMALERT_GetRingerVol(pMe);
    OEMALERT_SetRingerVol(pMe, FALSE);
    OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                           (OEMALERT_InCall(pMe) || OEMALERT_RingInHeadset(pMe)) ?
                                      SND_METHOD_VOICE : SND_METHOD_RING,
                           (int) SND_EXT_PWR_OFF,
                           SND_PRIO_MED,
                           SND_APATH_LOCAL,
                           NULL);    
    break;      
    
  case AEEALERT_ALERT_RUIMDOOR_REMOVED: 
    IBACKLIGHT_Enable(pMe->m_pBacklight);
    OEMALERT_GetRingerVol(pMe);
    OEMALERT_SetRingerVol(pMe, FALSE);
    OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                           SND_METHOD_RING,
                           (int) SND_SVC_LOST,
                           SND_PRIO_MED,
                           SND_APATH_LOCAL,
                           NULL);  
    break;            
    
  case AEEALERT_ALERT_ERR_SPECIAL: 
    IBACKLIGHT_Enable(pMe->m_pBacklight);
    OEMALERT_GetRingerVol(pMe);
    OEMALERT_SetRingerVol(pMe, FALSE);
    OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                           (OEMALERT_InCall(pMe) || OEMALERT_RingInHeadset(pMe)) ?
                                      SND_METHOD_VOICE : SND_METHOD_RING,
                           (int) SND_SPECIAL_INFO,
                           SND_PRIO_MED,
                           SND_APATH_LOCAL,
                           NULL);  
    break;                                               
      
  default:
    MSG_ERROR("Bad Alert Type",0,0,0);
    return EFAILED;
  }

  return SUCCESS;
//lint -save -esym(818, phoneNumber) Suppress "could be ptr to const"
}
//lint -restore

/*===========================================================================
FUNCTION OEMALERT_StopRinger

DESCRIPTION
  Stops playing the ringer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_StopRinger(IALERT *pMe)
{
#ifndef WIN32
  MSG_MED ("Stop Ringing", 0, 0, 0);
#endif
  if(NULL == pMe) {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return EBADPARM;
  }

  if(pMe->m_alertType == AEEALERT_ALERT_NORMAL) {
    if(pMe->m_bMMRinger) {
      // Nuke callback for any delayed ringer that hasn't
      // fired yet.
      CALLBACK_Cancel(&pMe->m_acbDelayedRingCallback);

      if (pMe->m_pMedia) {
        (void) IMEDIA_Stop(pMe->m_pMedia);
        (void) IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
      }
      pMe->m_bMMRinger = FALSE;
      return SUCCESS;
    }
    switch(pMe->m_ringerType) {
      /*Basic Ringers, call the sound task directly*/
      case OEMALERT_RINGER_NORMAL:
      case OEMALERT_RINGER_1:
      case OEMALERT_RINGER_2:
      case OEMALERT_RINGER_3:
      case OEMALERT_RINGER_4:
      case OEMALERT_RINGER_5:
      case OEMALERT_RINGER_6:
      case OEMALERT_RINGER_7:
      case OEMALERT_RINGER_8:
      case OEMALERT_RINGER_9:
      default:
      uisnd_snd_stop();
         break;
     }
  }
  else
  {
    uisnd_snd_stop();
  }

  return SUCCESS;
}

#ifdef FEATURE_PHONE_VR
//lint -save -esym(550, GetPhonebookMatchByNumber)
//lint -save -esym(528, GetPhonebookMatchByNumber)
/*===========================================================================
FUNCTION GetPhonebookMatchByNumber

DESCRIPTION
  Based on the number, find out the voice tag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean GetPhonebookMatchByNumber
(
  IALERT *pMe,
  AECHAR *phoneNum,
  char *pszName,
  int *rwid
 )
{
  //lint -save -esym(550, phoneNum)
  //lint -save -esym(528, phoneNum)

  IAddrBook *addr_bk;
  AECHAR name[40];
  char temp[5];
  AECHAR tempPhone[40];

  if(NULL == pMe) {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return EBADPARM;
  }

  (void) ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_ADDRBOOK,
                        (void **) &addr_bk);
  if (addr_bk == NULL)
    return FALSE;

  //STRTOWSTR(pszNumber, phoneNum, sizeof(phoneNum));
  (void) MEMCPY(temp, "1234",4);
  (void) STRTOWSTR(temp, tempPhone, sizeof(phoneNum));

  if (GetVoiceTagByNumber(addr_bk, tempPhone, (AEEVRUtterance *) rwid, name,
                                                            sizeof(name)) ) {
    (void) WSTRTOSTR(name, pszName, 40);

    (void) IADDRBOOK_Release(addr_bk);
    return TRUE;
  }

  (void) IADDRBOOK_Release(addr_bk);
  return FALSE;
}
#endif /* FEATURE_PHONE_VR */

/*=============================================================================
FUNCTION: OEMALERT_StartRingerAlert

DESCRIPTION:
   Start the ringer alert for incoming calls

PARAMETERS:
   *p: IAlert interface pointer
   id:personal contact ringer id

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_StartRingerAlert(IALERT *pMe,uint32 id,ALERT_SND_TYPE type)
{    
     byte alerttype;
     //boolean bKeyguardEnabled = FALSE;
     
     ASSERT(pMe != NULL);
     if (OEMALERT_InCall(pMe))
     {
         return;
     }
     //Get ringer id
     pMe->ring_id=id;
     pMe->m_mp3Ring = FALSE;
    //锁键盘后来电要恢复背光
    //   (void) ICONFIG_GetItem(pMe->m_pConfig,
    //                          CFGI_KEYGUARD_ENABLED,
    //                          &bKeyguardEnabled,
    //                          sizeof(bKeyguardEnabled));
    //   if(bKeyguardEnabled)
    //   {
    //      db_items_value_type  db_value;
    //      db_value.db_backlight_level = FALSE;
    //      db_put(DB_BACKLIGHT_LEVEL, &db_value);
    //
    //   } 
    
    IBACKLIGHT_Enable(pMe->m_pBacklight);
    
    // Initialize vibrate count to zero (assume we're going to ring)
    pMe->m_ringVibCount = 0;
    
    // Set ringer volume
    OEMALERT_GetRingerVol(pMe);    
    if (pMe->m_ringCurVol == OEMSOUND_ESCALATING_VOL) 
    {
        pMe->m_ringCurVol = OEMSOUND_1ST_VOL;
        pMe->m_ringEndVol = OEMSOUND_5TH_VOL;
    } 
    else 
    {
        pMe->m_ringEndVol = pMe->m_ringCurVol;
    }
    /*Only play for power on off snd*/     
    pMe->m_snd_type = type;
    if(type == ALERT_POWER_SND)
    {
        alerttype = OEMNV_ALERTTYPE_RINGER;
    //wangliang add! 2010-09-24
	#if defined(FEATURE_VERSION_SMART) || defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P)|| defined(FEATURE_VERSION_W027)
        pMe->m_ringCurVol = OEMSOUND_2ND_VOL;
        pMe->m_ringEndVol = OEMSOUND_2ND_VOL;
    #endif
    }
#ifdef FEATURE_SMSTONETYPE_MID      
    else if(type == ALERT_SMS_SND)
    {
        // Get sms ringer type
        (void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_SMS_RINGER,  &alerttype,sizeof(alerttype)); 
    }
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
    else
    {
        // Get ringer type
        (void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_ALERT_TYPE,  &alerttype,sizeof(alerttype)); 
    }

	MSG_FATAL("***zzg OEMALERT_StartRingAlert alerttype=%x***", alerttype, 0, 0);
	
     switch (alerttype) 
     {    
        default:
        case OEMNV_ALERTTYPE_RINGER:
        {
            //AEERingerID  ringerId;
            //AEERingerCat ci;
            OEMALERT_SetRingerVol(pMe, FALSE);  
            // Get the default ringer
            //ringerId = 0;            
            //(void) IRINGERMGR_EnumCategoryInit(pMe->m_pRingerMgr);
            //while (IRINGERMGR_EnumNextCategory(pMe->m_pRingerMgr, &ci)) 
            //{
            //    if (ci.id == AEE_RINGER_CATEGORY_ALL) 
            //    {
            //        ringerId = ci.idRinger;
            //        pMe->ring_id = ringerId;
            //        pMe->CurrRingerId = pMe->ring_id;
            //        break;
            //    }
            //}
            if(pMe->m_ringCurVol != OEMSOUND_MUTE_VOL)
            {
                if(SUCCESS != IRINGERMGR_Play(pMe->m_pRingerMgr, pMe->ring_id, 1))
                {
                    IRINGERMGR_Play(pMe->m_pRingerMgr, OEMNV_DEFAULTRINGER, 1);
                }
                gCurStatus.m_pCurRingerMgr = pMe->m_pRingerMgr;
                gCurStatus.m_bplaying = TRUE;
            }
            break; 
        }                            
    
        case OEMNV_ALERTTYPE_VIB:    
            // Nothing to do here, as initial call to
            // OEMALERT_HandleRingerTimer will vibrate the phone for us    
            break;
    
        case OEMNV_ALERTTYPE_VIBRINGER: 
            // Vibrate for a while before ringing                
#ifdef FEATURE_SMSTONETYPE_MID    
            if(type == ALERT_SMS_SND)
            {
                pMe->m_ringVibCount = COUNT_SMSVIBRATE_ALERTS_THEN_RING;
            }
            else
#endif //#if defined FEATURE_SMSTONETYPE_MID	
            pMe->m_ringVibCount = COUNT_VIBRATE_ALERTS_THEN_RING;
            break;

        case OEMNV_ALERTTYPE_VIBANDRINGER:    
            OEMALERT_SetRingerVol(pMe, FALSE); 
            if(pMe->m_ringCurVol != OEMSOUND_MUTE_VOL)
            {
                if(SUCCESS != IRINGERMGR_Play(pMe->m_pRingerMgr, pMe->ring_id, 1))
                {
                    IRINGERMGR_Play(pMe->m_pRingerMgr, OEMNV_DEFAULTRINGER, 1);
                }
                gCurStatus.m_pCurRingerMgr = pMe->m_pRingerMgr;
                gCurStatus.m_bplaying = TRUE;
            }		
			#ifdef FEATURE_VERSION_K202
			MSG_FATAL("pMe->m_iAlert..................00000000",0,0,0);
				if(!pMe->m_iAlert)
				{
					ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
					pMe->m_iAlert = TRUE;
				}
				else
				{
					ISound_StopVibrate(pMe->m_pSound);
					pMe->m_iAlert = FALSE;
				}
#else
				ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
#endif
            break;
    
        case OEMNV_ALERTTYPE_OFF:    
            // Nothing to do here, as initial call to
            // OEMALERT_HandleRingerTimer will flash the phone for us    
            break;    
     }    
    OEMALERT_HandleRingerAlertTimer(pMe);
#ifdef FEATURE_SMSTONETYPE_MID      
    if(type == ALERT_SMS_SND)
    {
        // Get sms ringer type
        (void) ISHELL_SetTimer(pMe->m_pIShell,
                                TIME_MS_SMSVIBRATE_DURATION,
                                OEMALERT_HandleStopRingerTimer,
                                pMe);   
    }
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
}
/*===========================================================================
OEMALERT_StartMp3Alert

DESCRIPTION
        播放mp3铃声

DEPENDENCIES
  None

RETURN VALUE

-SUCCESS
-FAILD

SIDE EFFECTS
  None
===========================================================================*/
static int OEMALERT_StartMp3Alert(IALERT * pMe, char *id, ALERT_SND_TYPE type)
{
    AEEMediaData mediaData;
    int          nRet = EFAILED;
    byte         alertType;
    uint16       volume;

    if(NULL == pMe) 
    {
        return EBADPARM;
    }
    
    if(type == ALERT_SMS_SND)
    {
        if(OEMALERT_InCall(pMe))
        {
            return EFAILED;            
        }
        
        if(AEECLSID_LAUNCHER != ISHELL_ActiveApplet(pMe->m_pIShell))
        {    
            if (gCurStatus.m_pCurRingerMgr != NULL &&
                gCurStatus.m_bplaying)
            {
            	#ifdef FEATURE_VERSION_C337
			 	INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
             	OEMALERT_SetRingerVol(pMe, TRUE);
             	#endif
                ISOUND_Vibrate(pMe->m_pSound,TIME_MS_SMSVIBRATE_DURATION); 
                return SUCCESS;
            }
        }     
    }
    ISHELL_CancelTimer(pMe->m_pIShell, OEMALERT_HandleStopMp3Timer, pMe);
    
    IBACKLIGHT_Enable(pMe->m_pBacklight);
    pMe->m_snd_type = type;
    pMe->m_mp3Ring = TRUE;
    
    if(pMe->m_pMedia) 
    {
        (void) IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }
    
    mediaData.clsData = MMD_FILE_NAME;
    mediaData.pData = (void *)id;
    mediaData.dwSize = 0;

    nRet = AEEMediaUtil_CreateMedia(pMe->m_pIShell, &mediaData,
                &pMe->m_pMedia) ;
    
    if(type == ALERT_SMS_SND)
    {
        (void) ICONFIG_GetItem(pMe->m_pConfig, CFGI_SMS_RINGER, &alertType, sizeof(alertType));
    }
    else
    {
        (void) ICONFIG_GetItem(pMe->m_pConfig, CFGI_ALERT_TYPE,  &alertType, sizeof(alertType)); 
    }
	
	MSG_FATAL("***zzg OEMALERT_StartMp3Alert alertType=%x***", alertType, 0, 0);
	
    OEMALERT_GetRingerVol(pMe);
    if(nRet == SUCCESS)         
    {
        if(type == ALERT_SMS_SND)
        {
            ISHELL_SetTimer(pMe->m_pIShell, TIME_MS_SMSMP3RING_DURATION, OEMALERT_HandleStopMp3Timer, pMe);
        }
        
        volume = ((uint16)(pMe->m_ringCurVol)) * AEE_MAX_VOLUME/5;
        (void)IMEDIA_SetVolume(pMe->m_pMedia,volume);  
		MSG_FATAL("alertType===================%d",alertType,0,0);
        
        switch(alertType)
        {
            default:
            case OEMNV_ALERTTYPE_RINGER:
                if(pMe->m_ringCurVol != OEMSOUND_MUTE_VOL)
                {
 #ifdef FEATURE_APP_MUSICPLAYER
                    if(type == ALERT_SMS_SND)
                    {
                       OEMALERT_NotifyMP3Player(pMe,TRUE);
                    }
#endif
                    (void) IMEDIA_Play(pMe->m_pMedia);
                    (void)IMEDIA_RegisterNotify(pMe->m_pMedia, OEMALERT_MediaNotify, pMe);
                }
                break;
                
            case OEMNV_ALERTTYPE_VIB:
				
                break;

            case OEMNV_ALERTTYPE_VIBRINGER:
				//#if 0
                if(type == ALERT_SMS_SND)
                {
                    pMe->m_ringVibCount = 2;
					#ifdef FEATURE_VERSION_C337
			 		INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
             		OEMALERT_SetRingerVol(pMe, TRUE);
             		#endif
					ISOUND_Vibrate(pMe->m_pSound,TIME_MS_SMSVIBRATE_DURATION);
	        	 	if(OEMSOUND_MUTE_VOL != pMe->m_ringCurVol)
	        	 	{	  
	            		// 为确保定时器未设置，先取消该定时器
	            		(void) ISHELL_CancelTimer(pMe->m_pIShell,OEMALERT_HandleMP3Timer, pMe);
	            
	            		// 设置闪烁图标定时器
	            		(void)ISHELL_SetTimer(pMe->m_pIShell,TIME_MS_SMSVIBRATE_DURATION, 
	                                    OEMALERT_HandleMP3Timer,
	                                    pMe);
	        		}
					return nRet;
                }    
                else
                {
                    pMe->m_ringVibCount = COUNT_VIBRATE_ALERTS_THEN_RING;
                }
				//#else
				
				//#endif
                break;

            case OEMNV_ALERTTYPE_VIBANDRINGER:  
                if(pMe->m_ringCurVol != OEMSOUND_MUTE_VOL)
                {
 #ifdef FEATURE_APP_MUSICPLAYER
                    if(type == ALERT_SMS_SND)
                    {
                       OEMALERT_NotifyMP3Player(pMe,TRUE);
                    }
#endif
                    (void) IMEDIA_Play(pMe->m_pMedia);
                    (void)IMEDIA_RegisterNotify(pMe->m_pMedia, OEMALERT_MediaNotify, pMe);
                }      
				#ifdef FEATURE_VERSION_K202
				MSG_FATAL("pMe->m_iAlert..................111111",0,0,0);
				if(!pMe->m_iAlert)
				{
					ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
					pMe->m_iAlert = TRUE;
				}
				else
				{
					ISound_StopVibrate(pMe->m_pSound);
					pMe->m_iAlert = FALSE;
				}
#else
				ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
#endif
                break;

            case OEMNV_ALERTTYPE_OFF:    
                // Nothing to do here, as initial call to
                // OEMALERT_HandleRingerTimer will flash the phone for us    
                break;  
        }
    }
    else
    {
        MSG_ERROR("IMEDIA could not be created  %x",nRet,0,0);
        return EFAILED;
    }
    //(void)IMEDIA_SetVolume(pMe->m_pMedia, oldVolume); 
    OEMALERT_HandleRingerAlertTimer(pMe);
    return nRet;
}

/*===========================================================================
OEMALERT_StopMp3Alert

DESCRIPTION
       停止mp3铃声的播放

DEPENDENCIES
  None

RETURN VALUE

-SUCCESS
-FAILD

SIDE EFFECTS
  None
===========================================================================*/
static void OEMALERT_StopMp3Alert(IALERT *pMe)
{
    if(pMe->m_snd_type == ALERT_SMS_SND)
    {
        // Stop recurring alert timer
        (void) ISHELL_CancelTimer(pMe->m_pIShell,
                                 OEMALERT_HandleStopMp3Timer,
                                 pMe);   
    }
    
    (void) ISHELL_CancelTimer(pMe->m_pIShell,
                             OEMALERT_HandleRingerAlertTimer,
                             pMe);
	(void) ISHELL_CancelTimer(pMe->m_pIShell,OEMALERT_HandleMP3Timer, pMe);

    if(pMe->m_pMedia)
    {
 #ifdef FEATURE_APP_MUSICPLAYER
        if(pMe->m_snd_type == ALERT_SMS_SND)
        {
           OEMALERT_NotifyMP3Player(pMe,FALSE);
        }
#endif
        MSG_ERROR("ykmilan come here to release media....",0,0,0);
       (void)IMEDIA_RegisterNotify(pMe->m_pMedia, NULL, pMe);
       IMEDIA_Release(pMe->m_pMedia);
       pMe->m_pMedia = NULL;
    }
    ISOUND_StopVibrate(pMe->m_pSound);

}

/*===========================================================================
OEMALERT_MediaNotify

DESCRIPTION
       IMEDIA 的 回调函数

DEPENDENCIES
  None

RETURN VALUE

-SUCCESS
-FAILD

SIDE EFFECTS
  None
===========================================================================*/
static void OEMALERT_MediaNotify(void * pUser, AEEMediaCmdNotify * pCmdNotify)
{
    IALERT *pMe = (IALERT*)pUser;

    
    if (pCmdNotify->nCmd == MM_CMD_PLAY)
    {
        switch (pCmdNotify->nStatus)
        {
            case MM_STATUS_DONE:
                if(pMe->m_pMedia)
                {
                    IMEDIA_Stop(pMe->m_pMedia);
                    IMEDIA_Play(pMe->m_pMedia);
                    (void)IMEDIA_RegisterNotify(pMe->m_pMedia, OEMALERT_MediaNotify, pMe);
                }
                break;

            case MM_STATUS_ABORT:
                OEMALERT_StopMp3Alert(pMe);
                OEMALERT_StartRingerAlert(pMe, OEMNV_DEFAULTRINGER, ALERT_NORMAL_SND);
                break;

            default:
                break;
        }
    }
}


/*=============================================================================
FUNCTION: OEMALERT_StopRingerAlert

DESCRIPTION:
   Stop the incoming call alert

PARAMETERS:
   *p: IAlert interface pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_StopRingerAlert(IALERT *pMe)
{
    ASSERT(pMe != NULL);
#ifdef FEATURE_SMSTONETYPE_MID        
    if(pMe->m_snd_type == ALERT_SMS_SND)
    {
        // Stop recurring alert timer
        (void) ISHELL_CancelTimer(pMe->m_pIShell,
                                 OEMALERT_HandleStopRingerTimer,
                                 pMe);   
    }
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
    
    // Stop recurring alert timer
    (void) ISHELL_CancelTimer(pMe->m_pIShell,
                             OEMALERT_HandleRingerAlertTimer,
                             pMe);
    // Stop Ringer
    //(void) IRINGERMGR_Stop(pMe->m_pRingerMgr);
    if (gCurStatus.m_pCurRingerMgr != NULL &&
        gCurStatus.m_bplaying)
    {
        IRINGERMGR_Stop(gCurStatus.m_pCurRingerMgr);
        gCurStatus.m_bplaying = FALSE;
        gCurStatus.m_pCurRingerMgr = NULL;
    }
    else
    {
        IRINGERMGR_Stop(pMe->m_pRingerMgr);
    }
    
    // Stop vibrating
    ISOUND_StopVibrate(pMe->m_pSound);
}

/*=============================================================================
FUNCTION: OEMALERT_HandleRingerTimer

DESCRIPTION:  Ringer timer callback

PARAMETERS:
   *pUser: OEMALERT object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_HandleRingerAlertTimer(void *pUser)
{
    byte             alerttype;
    static boolean   vib = FALSE;
    
    IALERT * pMe = (IALERT *)pUser;
    
    ASSERT(pMe != NULL);
    
    if(pMe->m_snd_type == ALERT_POWER_SND)
    {
        alerttype = OEMNV_ALERTTYPE_RINGER;
    }
    else if(pMe->m_snd_type == ALERT_SMS_SND)
    {
        // Get sms ringer type
        (void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_SMS_RINGER,&alerttype,sizeof(alerttype));        
    }
    else
    {
        (void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_ALERT_TYPE,&alerttype,sizeof(alerttype));
    }

	MSG_FATAL("***zzg OEMALERT_HandleRingerAlertTimer alerttype=%x***", alerttype, 0, 0);
	
    switch (alerttype) 
    {
        case OEMNV_ALERTTYPE_OFF:
            // Nothing to do, as backlight's already on
            break;
        
        case OEMNV_ALERTTYPE_VIB:
            // Alternate between vibrating and not
            //vib = !vib;
            //if( vib == TRUE ) 
            {
            	#ifdef FEATURE_VERSION_C306
            	OEMSound_Volume_Type     m_ringCurVol = OEMSOUND_4TH_VOL;
            	AEESoundInfo         si; 
			    OEMSound_Volume_Type vol;
			    
			    MEMSET(&si, 0, sizeof(si));
			    vol = m_ringCurVol;
			    if (OEMSOUND_ESCALATING_VOL == vol) 
			    {
			        vol = OEMSOUND_AVG_VOL;
			    }
			    
			    si.eDevice = AEE_SOUND_DEVICE_HANDSET;
			    si.eMethod = AEE_SOUND_METHOD_MIDI; 
			    (void) ISOUND_Set(pMe->m_pSound, &si);
			    ISOUND_SetVolume(pMe->m_pSound,
			                    GET_ISOUND_VOL_LEVEL((uint8) vol));
			    
			    si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
			    si.eMethod = AEE_SOUND_METHOD_MIDI;
			    (void) ISOUND_Set(pMe->m_pSound, &si);
			    ISOUND_SetVolume(pMe->m_pSound,
			                    GET_ISOUND_VOL_LEVEL((uint8) vol));
			    
			    si.eDevice = AEE_SOUND_DEVICE_HANDSET;
			    si.eMethod = AEE_SOUND_METHOD_RING;
			    (void) ISOUND_Set(pMe->m_pSound, &si);
			    ISOUND_SetVolume(pMe->m_pSound,
			                    GET_ISOUND_VOL_LEVEL((uint8) vol));
			    
			    si.eDevice = AEE_SOUND_DEVICE_HANDSET;
			    si.eMethod = AEE_SOUND_METHOD_VOICE;
			    (void) ISOUND_Set(pMe->m_pSound, &si);
			    ISOUND_SetVolume(pMe->m_pSound,
			                    GET_ISOUND_VOL_LEVEL((uint8) vol));
			    
			    si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
			    si.eMethod = AEE_SOUND_METHOD_VOICE;
			    (void) ISOUND_Set(pMe->m_pSound, &si);
			    ISOUND_SetVolume(pMe->m_pSound,
			                    GET_ISOUND_VOL_LEVEL((uint8) vol));
			    
			    si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
			    si.eMethod = AEE_SOUND_METHOD_RING;
			    (void) ISOUND_Set(pMe->m_pSound, &si);
			    ISOUND_SetVolume(pMe->m_pSound,
			                    GET_ISOUND_VOL_LEVEL((uint8) vol));  
                #endif
                #if defined(FEATURE_VERSION_S1000T) || defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W208S)
                ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION/2);
                #else
                //ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
                #ifdef FEATURE_VERSION_C337
			 	INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
             	OEMALERT_SetRingerVol(pMe, TRUE);
             	#endif
				#ifdef FEATURE_VERSION_K202
				if(!pMe->m_iAlert)
				{
					ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
					pMe->m_iAlert = TRUE;
				}
				else
				{
					ISound_StopVibrate(pMe->m_pSound);
					pMe->m_iAlert = FALSE;
				}
				#else
				ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
				#endif
				#endif
            }
            break;
        
        case OEMNV_ALERTTYPE_VIBRINGER:
			
            if (pMe->m_ringVibCount > 0) 
            {
                --pMe->m_ringVibCount;            
                if (pMe->m_ringVibCount % 2) 
                {
                    //ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
                    #ifdef FEATURE_VERSION_C337
			 		INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
             		OEMALERT_SetRingerVol(pMe, TRUE);
             		#endif
					
#ifdef FEATURE_VERSION_K202
					if(!pMe->m_iAlert)
					{
						ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
						pMe->m_iAlert = TRUE;
					}
					else
					{
						ISound_StopVibrate(pMe->m_pSound);
						pMe->m_iAlert = FALSE;
					}
#else
					ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
#endif
					
                }
                break;
            } 
            else if (pMe->m_ringVibCount == 0) 
            {
                //AEERingerID  ringerId;
                //AEERingerCat ci;
                // Decrement the count to -1 so that we only begin
                // ringing a single time
                MSG_FATAL("m_ringVibCount====================0",0,0,0);
                --pMe->m_ringVibCount;
                                
                OEMALERT_GetRingerVol(pMe);
                OEMALERT_SetRingerVol(pMe, FALSE); 
                // Get the default ringer
                //ringerId = 0;
                //(void) IRINGERMGR_EnumCategoryInit(pMe->m_pRingerMgr);
                //while (IRINGERMGR_EnumNextCategory(pMe->m_pRingerMgr, &ci)) 
                //{
                //    if (AEE_RINGER_CATEGORY_ALL == ci.id) 
                //    {
                //        ringerId = ci.idRinger;
                //        pMe->ring_id = ringerId;
                //        break;
                //    }
                //}
                if(pMe->m_ringCurVol != OEMSOUND_MUTE_VOL)
                {
                    if(pMe->m_mp3Ring)
                    {
                        MSG_ERROR("play media,",0,0,0);
                        IMEDIA_Play(pMe->m_pMedia);
                        (void)IMEDIA_RegisterNotify(pMe->m_pMedia, OEMALERT_MediaNotify, pMe);
                    }
                    else
                    {
                        (void) IRINGERMGR_Play(pMe->m_pRingerMgr, pMe->ring_id, 1);
                        gCurStatus.m_pCurRingerMgr = pMe->m_pRingerMgr;
                        gCurStatus.m_bplaying = TRUE;
                    }
                }
            }
            else
            {
            	MSG_FATAL("m_ringVibCount====================-1",0,0,0);
                // increase the escalating ringer volume if necessary
                if (pMe->m_ringCurVol < pMe->m_ringEndVol) 
                {
                    INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
                    OEMALERT_SetRingerVol(pMe, TRUE);
                }
            }  
			
            break;               

            case OEMNV_ALERTTYPE_VIBANDRINGER:
                // Alternate between vibrating and not
                vib = !vib;
                if( vib == TRUE ) 
                {
                	#ifdef FEATURE_VERSION_C337
			 		INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
             		OEMALERT_SetRingerVol(pMe, TRUE);
             		#endif
                }      
#ifdef FEATURE_VERSION_K202
				MSG_FATAL("pMe->m_iAlert..................2222222",0,0,0);
				if(!pMe->m_iAlert)
				{
					ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
					pMe->m_iAlert = TRUE;
				}
				else
				{
					ISound_StopVibrate(pMe->m_pSound);
					pMe->m_iAlert = FALSE;
				}
#else
				ISOUND_Vibrate(pMe->m_pSound,TIME_MS_RINGERVIBRATE_DURATION);
#endif
                break;     
        
        case OEMNV_ALERTTYPE_RINGER:
        default:
            // increase the escalating ringer volume if necessary
            if (pMe->m_ringCurVol < pMe->m_ringEndVol) 
            {
                INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
                OEMALERT_SetRingerVol(pMe, TRUE);
            }            
            break;
    }       
    (void) ISHELL_SetTimer(pMe->m_pIShell,
                            TIMEOUT_MS_RINGERVIB_TIMER,
                            OEMALERT_HandleRingerAlertTimer,
                            pMe);
}

#ifdef FEATURE_SMSTONETYPE_MID       
/*=============================================================================
FUNCTION: OEMALERT_HandleStopRingerTimer

DESCRIPTION:  Ringer timer callback

PARAMETERS:
   *pUser: OEMALERT object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_HandleStopRingerTimer(void *pUser)
{  
    IALERT * pMe = (IALERT *)pUser;
    ASSERT(pMe != NULL);
     
    OEMALERT_StopRingerAlert(pMe);
    
}
#endif //FEATURE_SMSTONETYPE_MID
/*=============================================================================
FUNCTION: OEMALERT_HandleStopMp3Timer

DESCRIPTION:  Ringer timer callback

PARAMETERS:
   *pUser: OEMALERT object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void OEMALERT_HandleStopMp3Timer(void *pUser)
{
    IALERT *pMe = (IALERT *)pUser;
    ASSERT(pMe != NULL);

    OEMALERT_StopMp3Alert(pMe);
}

/*=============================================================================
FUNCTION:  OEMALERT_StartRingerPreview
 
DESCRIPTION:  Previews the specified ringer.
 
PARAMETERS:
   *p: IAlert interface pointer
  int: ringer to preview
 
RETURN VALUE:
   None

COMMENTS:
   Use IALERT_StartRingerPreview() to stop the playback.

   This method will always play the ringer, even if the 'silence all' is 
   enabled or the ringer type is set to Vibration or Off.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_StartRingerPreview(IALERT *pMe, AEERingerID ringer)
{
    byte ringerVol;
    if(OEMALERT_InCall(pMe))
    {
        return;
    }        
    //OEMALERT_GetRingerVol(pMe);
    {
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                        CFGI_RINGER_VOL,
                        &ringerVol,
                        sizeof(ringerVol));
    }
    pMe->m_ringCurVol = (OEMSound_Volume_Type)ringerVol;    
    OEMALERT_SetRingerVol(pMe, TRUE);
    (void) IRINGERMGR_Play(pMe->m_pRingerMgr, ringer, 0);  
    gCurStatus.m_pCurRingerMgr = pMe->m_pRingerMgr;
    gCurStatus.m_bplaying = TRUE;
}
static void   OEMALERT_StartMp3Preview(IALERT * pMe, char *id)
{
    byte ringerVol;
    AEEMediaData mediaData;
    int nRet = EFAILED;
    uint16 volume;
    
    if(OEMALERT_InCall(pMe))
    {
        return;
    }
    
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                        CFGI_RINGER_VOL,
                        &ringerVol,
                        sizeof(ringerVol));
    mediaData.clsData = MMD_FILE_NAME;
    mediaData.pData = (void *)id;
    mediaData.dwSize = 0;

    nRet = AEEMediaUtil_CreateMedia(pMe->m_pIShell, &mediaData,
                &pMe->m_pMedia) ;
    if(nRet == AEE_SUCCESS)
    {
        pMe->m_ringCurVol = (OEMSound_Volume_Type)ringerVol;
        OEMALERT_GetRingerVol(pMe);
            
        volume = ((uint16)(pMe->m_ringCurVol)) * AEE_MAX_VOLUME/5;
        (void)IMEDIA_SetVolume(pMe->m_pMedia,volume);
#ifdef FEATURE_APP_MUSICPLAYER
        OEMALERT_NotifyMP3Player(pMe,TRUE);
#endif
        //end
        (void) IMEDIA_Play(pMe->m_pMedia);
        (void)IMEDIA_RegisterNotify(pMe->m_pMedia, OEMALERT_MediaNotify, pMe);
        gCurStatus.m_pCurRingerMgr = pMe->m_pRingerMgr;
        gCurStatus.m_bplaying = TRUE;
    }
}

/*=============================================================================
FUNCTION: OEMALERT_StartInSMSAlert

DESCRIPTION: Start SMS alert

PARAMETERS:
   *pUser: OEMAlert object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static void OEMALERT_StartSMSAlert (IALERT *pMe, int ring_id)
{  
    byte sms_size = 1;
    
    MSG_FATAL("OEMALERT_StartSMSAlert Start",0,0,0);
    
    //dsp的能力不够强,需要留出stop的时间
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_SMS_RINGER,
                            &sms_size,
                            sizeof(sms_size));
    
    OEMALERT_GetRingerVol(pMe);
    MSG_FATAL("sms_size=%d, m_ringCurVol=%d",sms_size, pMe->m_ringCurVol,0);
    
    //判断当前是不是来电响铃状态
    if (OEMALERT_InCall(pMe))
    {
        if(OEMSOUND_MUTE_VOL != pMe->m_ringCurVol)
        {
            IBACKLIGHT_Enable(pMe->m_pBacklight);
            OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                                    SND_METHOD_VOICE,
                                    SND_ABRV_ALERT,
                                    SND_PRIO_MED,
                                    SND_APATH_LOCAL,
                                    NULL);
            if(OEMNV_SMS_RING != sms_size)
            {
            	#ifdef FEATURE_VERSION_C337
			 	INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
             	OEMALERT_SetRingerVol(pMe, TRUE);
             	#endif
                ISOUND_Vibrate(pMe->m_pSound,TIME_MS_SMSVIBRATE_DURATION); 
                
                 
            }
        }
        return;
    } 
    
    OEMALERT_SetRingerVol(pMe, FALSE);
    
    if(AEECLSID_LAUNCHER != ISHELL_ActiveApplet(pMe->m_pIShell))
    {    
        //(void) IRINGERMGR_Stop(pMe->m_pRingerMgr);
        if (gCurStatus.m_pCurRingerMgr != NULL &&
            gCurStatus.m_bplaying)
        {
            //IRINGERMGR_Stop(gCurStatus.m_pCurRingerMgr);
            //gCurStatus.m_bplaying = FALSE;
            //gCurStatus.m_pCurRingerMgr = NULL;
             #ifdef FEATURE_VERSION_C337
			 INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
             OEMALERT_SetRingerVol(pMe, TRUE);
             #endif
             ISOUND_Vibrate(pMe->m_pSound,TIME_MS_SMSVIBRATE_DURATION); 
            
            
            return;
        }
        else
        {
            IRINGERMGR_Stop(pMe->m_pRingerMgr);
        }
    }
    
#ifdef FEATURE_SMSTONETYPE_MID  
    OEMALERT_StartRingerAlert(pMe,ring_id,ALERT_SMS_SND);
#else 

    if(OEMNV_SMS_RING == sms_size)  
    {
        if(pMe->m_ringCurVol != OEMSOUND_MUTE_VOL)
        {
#ifdef FEATURE_APP_MUSICPLAYER
            OEMALERT_NotifyMP3Player(pMe,TRUE);
            (void)ISHELL_SetTimer(pMe->m_pIShell, 2000, (PFNNOTIFY)OEMALERT_NotifyMP3PlayerCB, pMe);
#endif
            OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                                    (OEMALERT_InCall(pMe) || OEMALERT_RingInHeadset(pMe)) ?
                                    SND_METHOD_VOICE : SND_METHOD_RING,
                                    ring_id,
                                    SND_PRIO_MED,
                                    SND_APATH_LOCAL,
                                    NULL);
        }
    }
    else if(OEMNV_SMS_VIBONLY == sms_size)
    {
    	 #ifdef FEATURE_VERSION_C337
		 INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
         OEMALERT_SetRingerVol(pMe, TRUE);
         #endif
         ISOUND_Vibrate(pMe->m_pSound,TIME_MS_SMSVIBRATE_DURATION); 
       
    }
    else if(OEMNV_SMS_RINGVIB == sms_size)//响铃振动
    {
        pMe->smsID = ring_id;
#ifdef FEATURE_APP_MUSICPLAYER
        if(pMe->m_ringCurVol != OEMSOUND_MUTE_VOL)
        {
            OEMALERT_NotifyMP3Player(pMe,TRUE);
            (void)ISHELL_SetTimer(pMe->m_pIShell, 2000, (PFNNOTIFY)OEMALERT_NotifyMP3PlayerCB, pMe);
        }
#endif       
		 #ifdef FEATURE_VERSION_C337
		 INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
         OEMALERT_SetRingerVol(pMe, TRUE);
		 #endif
         ISOUND_Vibrate(pMe->m_pSound,TIME_MS_SMSVIBRATE_DURATION);     
      
        
        if(OEMSOUND_MUTE_VOL != pMe->m_ringCurVol)
        {	         
            // 为确保定时器未设置，先取消该定时器
            AEE_CancelTimer(OEMALERT_HandleSMSTimer, pMe);
            
            // 设置闪烁图标定时器
            (void)AEE_SetSysTimer(TIME_MS_SMSVIBRATE_DURATION, 
                                    OEMALERT_HandleSMSTimer,
                                    pMe);
        }
    }
    else if(OEMNV_SMS_VIBANDRINGER == sms_size)
    {
#ifdef FEATURE_APP_MUSICPLAYER
        if(pMe->m_ringCurVol != OEMSOUND_MUTE_VOL)
        {
            OEMALERT_NotifyMP3Player(pMe,TRUE);
            (void)ISHELL_SetTimer(pMe->m_pIShell, 2000, (PFNNOTIFY)OEMALERT_NotifyMP3PlayerCB, pMe);
        }
#endif    
         #ifdef FEATURE_VERSION_C337
		 INCREMENT_ESCALATING_RINGER(pMe->m_ringCurVol);
         OEMALERT_SetRingerVol(pMe, TRUE);
		 #endif
         ISOUND_Vibrate(pMe->m_pSound,TIME_MS_SMSVIBRATE_DURATION);     
         
        if(pMe->m_ringCurVol != OEMSOUND_MUTE_VOL)
        {
            OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                                    (OEMALERT_InCall(pMe) || OEMALERT_RingInHeadset(pMe)) ?
                                    SND_METHOD_VOICE : SND_METHOD_RING,
                                    ring_id,
                                    SND_PRIO_MED,
                                    SND_APATH_LOCAL,
                                    NULL);
        }
    }  
#endif //#ifdef FEATURE_SMSTONETYPE_MID  
    MSG_FATAL("OEMALERT_StartSMSAlert End",0,0,0);
}    
/*=============================================================================
FUNCTION: OEMALERT_StopSMSAlert

DESCRIPTION:
   Stops the SMS message waiting alert

PARAMETERS:
   *p: IAlert interface pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_StopSMSAlert(IALERT *pMe)
{
#ifndef WIN32
#ifdef FEATURE_SMSTONETYPE_MID 
    pMe->m_snd_type = ALERT_SMS_SND;
    OEMALERT_StopRingerAlert(pMe);
#else
   OEMSOUND_Sound_Stop(SND_PRIO_MED);
#endif
   ISOUND_StopVibrate(pMe->m_pSound);

   // Stop recurring SMS alert (reminder) timer
   (void) ISHELL_CancelTimer(pMe->m_pIShell,
                             OEMALERT_HandleSMSTimer,
                             (void*)pMe);
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
}

#if !defined(FEATURE_SMSTONETYPE_MID)
/*=============================================================================
FUNCTION: OEMALERT_HandleSMSTimer

DESCRIPTION:
   响铃后振动的定时器回调函数

PARAMETERS:
   *pUser: CAlert object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_HandleSMSTimer(void *pUser)
{
   register IALERT *pMe = (IALERT *)pUser;

   ASSERT(pMe != NULL);
   
   OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                         SND_METHOD_RING,
                         pMe->smsID,
                         SND_PRIO_MED,
                         SND_APATH_LOCAL,
                         NULL);
}
/*=============================================================================
FUNCTION: OEMALERT_HandleMP3Timer

DESCRIPTION:
MP3

PARAMETERS:
   *pUser: CAlert object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void   OEMALERT_HandleMP3Timer(void *pUser)
{
	register IALERT *pMe = (IALERT *)pUser;
	
	   ASSERT(pMe != NULL);

#ifdef FEATURE_APP_MUSICPLAYER
		if(pMe->m_snd_type == ALERT_SMS_SND)
		{
			 OEMALERT_NotifyMP3Player(pMe,TRUE);
		}
#endif
		(void) IMEDIA_Play(pMe->m_pMedia);
		(void)IMEDIA_RegisterNotify(pMe->m_pMedia, OEMALERT_MediaNotify, pMe);

}
#endif

/*=============================================================================
FUNCTION: OEMALERT_StartMissedCallAlert

DESCRIPTION:
   Start the missed call alert

PARAMETERS:
   *p: IAlert interface pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_StartMissedCallAlert(IALERT *pMe)
{
   byte             missedCallAlert;

   IBACKLIGHT_Enable(pMe->m_pBacklight);
   (void) ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_MISSED_CALL_ALERT,
                          &missedCallAlert,
                          sizeof(byte));

   if (OEMNV_ALERT_ENABLE == missedCallAlert) 
   {
      OEMALERT_GetRingerVol(pMe);
      OEMALERT_SetRingerVol(pMe, FALSE);
      pMe->alert_count = 0; 
      OEMALERT_HandleMissedCallTimer(pMe);
   }
}


/*=============================================================================
FUNCTION: OEMALERT_StopMissedCallAlert

DESCRIPTION:
   Stop the missed call alert

PARAMETERS:
   *p: IAlert interface pointer

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_StopMissedCallAlert(IALERT *pMe)
{
   ASSERT(pMe != NULL);

   // Stop Ringer
   OEMSOUND_Sound_Stop(SND_PRIO_MED);

   // Stop vibrating
   ISOUND_StopVibrate(pMe->m_pSound);

   // Stop recurring alert timer
   (void) ISHELL_CancelTimer(pMe->m_pIShell,
                             OEMALERT_HandleMissedCallTimer,
                             (void*)pMe);
}

/*=============================================================================
FUNCTION: OEMALERT_HandleMissedCallTimer

DESCRIPTION: Missed call timer callback

PARAMETERS:
   *pUser: CAlert object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_HandleMissedCallTimer(void *pUser)
{
    register IALERT *pMe = (IALERT *)pUser;
    
    ASSERT(pMe != NULL);
    
    //规定错过一次来电提示的次数不多于三次,否则很耗电
    if(pMe->alert_count>= 3)
    {
        pMe->alert_count=0;//当达到3时，置为0，再直接返回。
        return;
    }
    pMe->alert_count++;
    
    // Don't pester the user while in a call
    if (!OEMALERT_InCall(pMe)) 
    {    
        IBACKLIGHT_Enable(pMe->m_pBacklight);
        
        OEMSOUND_Sound_Id_Start(SND_DEVICE_CURRENT,
                                OEMALERT_RingInHeadset(pMe) ?
                                SND_METHOD_VOICE :
                                SND_METHOD_RING,
                                (int) SND_MESSAGE_ALERT,
                                SND_PRIO_MED,
                                SND_APATH_LOCAL,
                                NULL);    
    }    
    (void) ISHELL_SetTimer(pMe->m_pIShell,
                            TIMEOUT_MS_MISSEDCALL_TIMER,
                            OEMALERT_HandleMissedCallTimer,
                            pMe);
}

/*=============================================================================
FUNCTION: OEMALERT_KeyBeep

DESCRIPTION:
   Makes the keybeep sound

PARAMETERS:
      *p: IAlert interface pointer
     key: the key that has been pressed
bPressed: TRUE if the key has been pressed, FALSE if it has been released.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_KeyBeep(IALERT *pMe, AVKType key, boolean bPressed)
{
   snd_method_type        method;
   byte                   method_ex = 0;
   keyToneLength          len;
   AEESoundToneData       td;
   byte                   btKeyVol = 0;
   boolean                headsetPresent;

   static AEESoundInfo si = 
   {
      AEE_SOUND_DEVICE_CURRENT,
      AEE_SOUND_METHOD_BEEP,
      AEE_SOUND_APATH_LOCAL,
      AEE_SOUND_MUTECTL_UNMUTED,
      AEE_SOUND_MUTECTL_UNMUTED,
   };
   static keyToneLength   lastLen    = OEMNV_KEYTONE_NORMAL;
#ifdef FEATURE_CLICK
   static snd_method_type lastMethod = SND_METHOD_CLICK;
#else
   static snd_method_type lastMethod = SND_METHOD_KEY_BEEP;
#endif

   ASSERT(pMe != NULL);

   
   if (gCurStatus.m_bplaying && (NULL != gCurStatus.m_pCurRingerMgr))
   {
      return;
   }
   
   (void) ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_KEYBEEP_SOUND,
                          &method_ex,
                          sizeof(byte));

   (void) ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_KEYTONE_LENGTH,
                          &len,
                          sizeof(keyToneLength));
                       
   (void) ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_BEEP_VOL,
                          &btKeyVol,
                          sizeof(byte));
   //DBGPRINTF("KeyBeep %d %d %d",method_ex,len,btKeyVol);
   method = (snd_method_type)method_ex;
   if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,
                                   CFGI_HEADSET_PRESENT,
                                   &headsetPresent,
                                   sizeof(headsetPresent)))
   {
       headsetPresent = FALSE;
   }
   
   if (!bPressed) 
   {
      // Make sure the tone is stopped if the user just 
      // switched from Long Tones to Normal Tones, or from
      // Long Tones to Clicks.
      if ((lastLen != len) || (lastMethod != method)) 
      {
         ISOUND_StopTone(pMe->m_pSound);
      }
      lastLen = len;
      lastMethod = method;
   }

   switch (method)
   {
#ifdef FEATURE_CLICK
      case SND_METHOD_CLICK:
      {
         if (bPressed) 
         {
            si.eMethod = AEE_SOUND_METHOD_CLICK;   
            td.eTone     = AEE_TONE_2;     // tone doesn't matter
            td.wDuration = NORMAL_TONE_MS; // duration doesn't matter            
            ISOUND_Set(pMe->m_pSound, &si);    
            ISOUND_SetVolume(pMe->m_pSound, GET_ISOUND_VOL_LEVEL(btKeyVol));
            if((btKeyVol != OEMSOUND_MUTE_VOL)&&(!headsetPresent))
            {            
                ISOUND_PlayTone( pMe->m_pSound, td); 
            }
         } 
         break;
      }
      
#endif
      default:
      case SND_METHOD_KEY_BEEP:   
      {         
         si.eMethod = AEE_SOUND_METHOD_BEEP;
   
         if (OEMNV_KEYTONE_LONG == len) 
         {
            static uint32 keystart_time;
            
            if (bPressed) 
            {  
               keystart_time = GETUPTIMEMS();   
               td.eTone = OEMALERT_MapKeyToTone(key);
   
               // Only play the END key tone for LONG_TONE_MS milliseconds,
               // the user needs to hold the END key for several seconds to 
               // power down the phone and it is annoying to have it beeping
               // for the whole time.  Same thing for the CLR key (ie, when
               // enabling the keyguard)
               if ((key == AVK_END) || (key == AVK_POWER) || (key == AVK_CLR)) 
               {
                  td.wDuration = LONG_TONE_MS; 
               } 
               else 
               {
                  // Play the tone for a long as possible (around 65 seconds)
                  td.wDuration = 1000;
               }               
               ISOUND_Set(pMe->m_pSound, &si);    
               ISOUND_SetVolume(pMe->m_pSound, GET_ISOUND_VOL_LEVEL(btKeyVol));

			   
               if((btKeyVol != OEMSOUND_MUTE_VOL)&&(!headsetPresent))
               {          
                   ISOUND_PlayTone( pMe->m_pSound, td);  
               }  
            } 
            else 
            {
               uint32 tone_time = GETUPTIMEMS() - keystart_time;

               // If the tone has been playing for longer than LONG_TIME_MS,
               // just stop it immediately.  Also need to check that SilenceAll
               // hasn't just been enabled, which might cause the tone to 
               // play for the entire 0xFFFF duration.
               if ( tone_time >= LONG_TONE_MS )
               {
                  ISOUND_StopTone(pMe->m_pSound);
               } 
               else 
               {   
                  // The key was released before LONG_TIME_MS millseconds, so
                  // continue playing the tone for the time remaining.
                  td.eTone = OEMALERT_MapKeyToTone(key);
                  td.wDuration = (uint16)(LONG_TONE_MS - tone_time); 
   
                  ISOUND_Set(pMe->m_pSound, &si);    
                  ISOUND_SetVolume(pMe->m_pSound, GET_ISOUND_VOL_LEVEL(btKeyVol));

				  
                  if((btKeyVol != OEMSOUND_MUTE_VOL)&&(!headsetPresent))
                  {                  
                     ISOUND_PlayTone(pMe->m_pSound, td); 
                  }
               }
            }
         } 
         else 
         {
            if (bPressed) 
            {
               td.eTone     = OEMALERT_MapKeyToTone(key);
               td.wDuration = NORMAL_TONE_MS; 
   
               ISOUND_Set(pMe->m_pSound, &si);    
               ISOUND_SetVolume(pMe->m_pSound, GET_ISOUND_VOL_LEVEL(btKeyVol));

			   
              // if((btKeyVol != OEMSOUND_MUTE_VOL)&&(!headsetPresent))
               if(btKeyVol != OEMSOUND_MUTE_VOL)
               {                
                  ISOUND_PlayTone(pMe->m_pSound, td); 
               }
            }
         }
         break;
      }
      
      //default:
      //   break;
      
   }
   //因为按键音播完不会关闭相关硬件造成电流大
   (void) ISHELL_CancelTimer(pMe->m_pIShell,
                             OEMALERT_keeybeep_stop,
                             pMe);
   (void) ISHELL_SetTimer(pMe->m_pIShell,
                          700,
                          OEMALERT_keeybeep_stop,
                          pMe);
   
}

/*=============================================================================
FUNCTION: OEMALERT_keeybeep_stop

DESCRIPTION:
   Stop the keybeep sound

PARAMETERS:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_keeybeep_stop(void *pUser)
{
    IALERT * pMe = (IALERT *)pUser;
	ISOUND_StopTone(pMe->m_pSound);
}
	
/*=============================================================================
FUNCTION:  OEMALERT_MapKeyToTone
 
DESCRIPTION: Returns the tone for the specified key
 
PARAMETERS:
   key: 
 
RETURN VALUE:
   AEESoundTone: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static AEESoundTone OEMALERT_MapKeyToTone(AVKType key)
{
//#if defined(FEATURE_PROJECT_SMART) || defined(FEATURE_PROJECT_M8)
#if defined (FEATURE_DEFAULT_KEYTONE_SOUND_LEVEL)
	return AEE_TONE_3;
#endif

   switch (key) 
   {
       case AVK_1:
          return AEE_TONE_1;
          
       case AVK_2:
          return AEE_TONE_2;
          
       case AVK_3:
          return AEE_TONE_3;
          
       case AVK_4:
          return AEE_TONE_4;
          
       case AVK_5:
          return AEE_TONE_5;
          
       case AVK_6:
          return AEE_TONE_6;
          
       case AVK_7:
          return AEE_TONE_7;
          
       case AVK_8:
          return AEE_TONE_8;
          
       case AVK_9:
          return AEE_TONE_9;
          
       case AVK_0:
          return AEE_TONE_0;
          
       case AVK_POUND:
          return AEE_TONE_POUND;
          
       case AVK_STAR:
          return AEE_TONE_STAR;
          
       case AVK_POWER:
       case AVK_END:
          return AEE_TONE_CTRL;
          
       default:
          return AEE_TONE_2ND;
   }
}

/*=============================================================================
FUNCTION: OEMALERT_GetRingerVol

DESCRIPTION:
   Retrieve the current ringer volume from NV and store it in
   OEMALERT::m_ringCurVol.

PARAMETERS:
   *pMe: OEMALERT object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_GetRingerVol(IALERT *pMe)
{
    byte ringerVol;
    boolean headsetPresent;
    byte    btActiveProfile;
    byte headsetVol[PROFILENUMBER];

    (void) ICONFIG_GetItem(pMe->m_pConfig, 
                   CFGI_HEADSET_PRESENT, 
                   &headsetPresent, 
                   sizeof(boolean));
    if  (headsetPresent == TRUE)
    {
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                        CFGI_PROFILE_CUR_NUMBER,
                        &btActiveProfile,
                        sizeof(btActiveProfile));     
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                        CFGI_PROFILE_EAR_VOL,
                        headsetVol,
                        sizeof(headsetVol));
        ringerVol = headsetVol[btActiveProfile];  
    }
    else
    {
        (void) ICONFIG_GetItem(pMe->m_pConfig,
                        CFGI_RINGER_VOL,
                        &ringerVol,
                        sizeof(ringerVol));
    }
    pMe->m_ringCurVol = (OEMSound_Volume_Type)ringerVol;
}

/*=============================================================================
FUNCTION: OEMALERT_SetRingerVol

DESCRIPTION:
   Set the ringer volume to the volume level specified
   by OEMALERT::m_ringCurVol

PARAMETERS:
   *pMe: OEMALERT object

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMALERT_SetRingerVol(IALERT *pMe, boolean bEscalate)
{
    AEESoundInfo         si; 
    OEMSound_Volume_Type vol;
    
    MEMSET(&si, 0, sizeof(si));
    vol = pMe->m_ringCurVol;
    if (OEMSOUND_ESCALATING_VOL == vol) 
    {
        vol = OEMSOUND_AVG_VOL;
    }
    
    si.eDevice = AEE_SOUND_DEVICE_HANDSET;
    si.eMethod = AEE_SOUND_METHOD_MIDI; 
    (void) ISOUND_Set(pMe->m_pSound, &si);
    ISOUND_SetVolume(pMe->m_pSound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));
    
    si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
    si.eMethod = AEE_SOUND_METHOD_MIDI;
    (void) ISOUND_Set(pMe->m_pSound, &si);
    ISOUND_SetVolume(pMe->m_pSound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));
    
    si.eDevice = AEE_SOUND_DEVICE_HANDSET;
    si.eMethod = AEE_SOUND_METHOD_RING;
    (void) ISOUND_Set(pMe->m_pSound, &si);
    ISOUND_SetVolume(pMe->m_pSound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));
    
    si.eDevice = AEE_SOUND_DEVICE_HANDSET;
    si.eMethod = AEE_SOUND_METHOD_VOICE;
    (void) ISOUND_Set(pMe->m_pSound, &si);
    ISOUND_SetVolume(pMe->m_pSound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));
    
    si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
    si.eMethod = AEE_SOUND_METHOD_VOICE;
    (void) ISOUND_Set(pMe->m_pSound, &si);
    ISOUND_SetVolume(pMe->m_pSound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));
    
    si.eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET; //AEE_SOUND_DEVICE_HEADSET;
    si.eMethod = AEE_SOUND_METHOD_RING;
    (void) ISOUND_Set(pMe->m_pSound, &si);
    ISOUND_SetVolume(pMe->m_pSound,
                    GET_ISOUND_VOL_LEVEL((uint8) vol));                    
}

/*=============================================================================
FUNCTION: OEMALERT_InCall

DESCRIPTION:
   Return TRUE if the phone is currently in a call

PARAMETERS:
   *pMe: OEMALERT object

RETURN VALUE:
   boolean

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean OEMALERT_InCall(IALERT *pMe)
{
#ifdef FEATURE_ICM
    return (AEECM_IS_VOICECALL_CONNECTED(pMe->m_pICM));
#else
	AEETCalls po;
	
	if(SUCCESS != ITELEPHONE_GetCalls(pMe->m_pITelephone, &po,sizeof(AEETCalls)))
	{
		return FALSE;
	}

    return (po.dwCount>0)?TRUE:FALSE;
#endif
}
/*=============================================================================
FUNCTION:  OEMALERT_RingInHeadset
 
DESCRIPTION: Returns TRUE if the headset is attached and headset ringing
             has been enabled.
 
PARAMETERS:
   None
 
RETURN VALUE:
   boolean: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean OEMALERT_RingInHeadset(IALERT *pMe)
{
    boolean headsetPresent;    
    if (SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,
                                    CFGI_HEADSET_PRESENT,
                                    &headsetPresent,
                                    sizeof(headsetPresent)))
    {
        headsetPresent = FALSE;
    }
    return (headsetPresent);    
}

static void OEMALERT_StartSMSAlertPreview(IALERT *pMe, int ring_id)
{
    ISOUND_StopTone(pMe->m_pSound);
    // Stop Ringer
    //IRINGERMGR_Stop(pMe->m_pRingerMgr);
    if (gCurStatus.m_pCurRingerMgr != NULL &&
        gCurStatus.m_bplaying)
    {
        IRINGERMGR_Stop(gCurStatus.m_pCurRingerMgr);
        gCurStatus.m_bplaying = FALSE;
        gCurStatus.m_pCurRingerMgr = NULL;
    }
    else
    {
        IRINGERMGR_Stop(pMe->m_pRingerMgr);
    }
    OEMALERT_GetRingerVol(pMe);
    OEMALERT_SetRingerVol(pMe, FALSE);
#ifndef WIN32
    snd_sound_id_start(SND_DEVICE_CURRENT,
                       (OEMALERT_InCall(pMe) || OEMALERT_RingInHeadset(pMe)) ?
                       SND_METHOD_VOICE : SND_METHOD_RING,
                       (int) ring_id,
                       SND_PRIO_MED,
                       SND_APATH_LOCAL,
                       NULL,NULL);
#endif
}



/*=======================================================================

Function: OEMALERT_MinuteAlert

Description:
   Plays the minute alert

Parameters:
   p: IAlert interface pointer

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:

=====================================================================*/
static void OEMALERT_MinuteAlert(IALERT *pMe)
{
    static  AEESoundToneData  td = 
    {
        AEE_TONE_TIME,  
        NORMAL_TONE_MS
    };
    static const AEESoundInfo      si = 
    {
        AEE_SOUND_DEVICE_CURRENT,
        AEE_SOUND_METHOD_VOICE,
        AEE_SOUND_APATH_LOCAL,
        AEE_SOUND_MUTECTL_UNMUTED,
        AEE_SOUND_MUTECTL_UNMUTED,
    };

    ASSERT(pMe != NULL);

    if (!OEMALERT_InCall(pMe)) 
    {
        return;
    }

    if(!OEMALERT_RingInHeadset(pMe))
    {
        OEMSound_PlayTone((AEESoundInfo *) &si, &td, NULL); 
    }
    else
    {      
        //IRINGERMGR_Stop(pMe->m_pRingerMgr);
        if (gCurStatus.m_pCurRingerMgr != NULL &&
            gCurStatus.m_bplaying)
        {
            IRINGERMGR_Stop(gCurStatus.m_pCurRingerMgr);
            gCurStatus.m_bplaying = FALSE;
            gCurStatus.m_pCurRingerMgr = NULL;
        }
        else
        {
            IRINGERMGR_Stop(pMe->m_pRingerMgr);
        }
#ifndef WIN32
        snd_sound_id_start(SND_DEVICE_CURRENT,                                        
                                        SND_METHOD_VOICE ,
                                        SND_MESSAGE_ALERT,
                                        SND_PRIO_MED,                                 
                                        SND_APATH_LOCAL,
                                        NULL,NULL);
#endif
    }
}

#ifdef FEATURE_APP_MUSICPLAYER
static void OEMALERT_NotifyMP3Player(IALERT *pMe,boolean bStartAlert)
{
    if(!IsMp3PlayerStatusNone())
    {
        (void)ISHELL_SendEvent(pMe->m_pIShell, 
                               AEECLSID_APP_MUSICPLAYER,
                               EVT_ALARM,
                               bStartAlert,
                               0);
    }
}
static void OEMALERT_NotifyMP3PlayerCB(IALERT * pMe)
{
    MSG_FATAL("OEMALERT_NotifyMP3PlayerCB Start",0,0,0);
    OEMALERT_NotifyMP3Player(pMe,FALSE);
    MSG_FATAL("OEMALERT_NotifyMP3PlayerCB End",0,0,0);
}
#endif
