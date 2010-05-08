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


        Copyright © 2006-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMAlert.c#19 $
$DateTime: 2007/10/31 15:38:17 $
$Author: carls $
$Change: 559515 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/07   cvs     Fix stale pointer
02/27/07   jas     Featurizing voice recognition code
09/27/06   jks     Added support for QTV priority multimedia ringers
07/20/06   jks     Added singleton media ringer delay
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
#include "AEE.h"         // BREW headers.
#include "AEE_OEM.h"     // Brew OEM Headers
#include "AEEAlert.h"
#include "AEEFile.h"
#include "AEEStdLib.h"
#include "OEMHeap.h"
#include "AEE_OEMDispatch.h"
#include "uixsnd.h"


#ifdef FEATURE_BREW_3_0
#include "OEMCriticalSection.h"
#endif

#include "AEEMediaUtil.h"

#ifdef FEATURE_APP_MPEG4
#include "AEEMediaMPEG4.h"
#endif

#include "nv.h"
#ifdef FEATURE_PHONE_VR
#include "AEEVR.h"
#endif /* FEATURE_PHONE_VR */
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "AEEAddrBookExt.h"
#ifdef FEATURE_PHONE_VR
#include "OEMVR_priv.h"
#endif /* FEATURE_PHONE_VR */
#include "AppComFunc.h"
#ifndef USES_MMI
#include "uiutils.h"
#endif
#include "tmc.h"
#include "BREWVersion.h"

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
#ifdef FEATURE_PHONE_VR
  boolean                  m_bVRPlaying;      /* Identifies if VR is aplying the prompts*/
#endif /* FEATURE_PHONE_VR */
  uint8                    m_numTimeOut;      /* Timeout value*/
  boolean                  m_bStopRinger;
  AEECallback              m_acbDelayedRingCallback;
  int32                    m_nMediaRingerDelay;
  IMedia*                  m_pMedia;         /* Pointer to IMedia*/
#ifdef FEATURE_PHONE_VR
  IVR*                     m_pVR;            /* pointer to IVR*/
#endif /* FEATURE_PHONE_VR */
  IConfig*                 m_pConfig;        /* pointer to IConfig*/
  void *                   m_pac;        /* App context */
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
#endif /* FEATURE_PHONE_VR */

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
   OEMALERT_GetMediaRingerDelay
};



/***********************************************************************
 * D A T A    O B J E C T S
 ***********************************************************************/
static struct IALERT_NOTIFIER *IALERT_NOTIFIERobj = NULL;


typedef struct OEMALERT_NOTIFIER_notify_cb
{
  uint8 mask;
  void *user_data;
} OEMCARD_ALERT_NOTIFY_CB;


static int32 nGlobalMediaRingerDelay = 250;

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
  ISHELL_Resume(IALERT_NOTIFIERobj->m_pIShell, &oem_cb);

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
#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
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

   if (pMe->m_pMedia) {
     (void) IMEDIA_Release(pMe->m_pMedia);
     pMe->m_pMedia = NULL;
   }

#ifdef FEATURE_PHONE_VR
   if (pMe->m_pVR) {
     (void)IVR_Release(pMe->m_pVR);
     pMe->m_pVR = NULL;
   }
#endif /* FEATURE_PHONE_VR */

   if (pMe->m_pConfig) {
     (void) ICONFIG_Release(pMe->m_pConfig);
     pMe->m_pConfig = NULL;
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

#ifdef AEE_SIMULATOR
   // Do not create an object is this is being run on the SDK.
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

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
  pNew->m_pConfig = NULL;

#ifdef FEATURE_PHONE_VR
  /* IVR object should be available*/
  pNew->m_pVR = NULL;
  if(ISHELL_CreateInstance(pNew->m_pIShell, AEECLSID_VR,
			   (void **)&pNew->m_pVR) != SUCCESS) {
      OEM_FreeAlert(pNew);
      return EFAILED;
  }
#endif /* FEATURE_PHONE_VR */
  if (ISHELL_CreateInstance(pNew->m_pIShell, AEECLSID_CONFIG,
			    (void **) &pNew->m_pConfig)
      != SUCCESS) {
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

  if (pMe->m_uRefs == 0) {
    OEM_FreeAlert(pMe);
    return 0;
  }

  if (--pMe->m_uRefs != 0) {
    return pMe->m_uRefs;
  }


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
  nGlobalMediaRingerDelay = nMediaRingerDelay;
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
  *pnMediaRingerDelay = nGlobalMediaRingerDelay;
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

#endif /* FEATURE_PHONE_VR */

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
#endif /* FEATURE_PHONE_VR */

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
#endif /* FEATURE_PHONE_VR */

    if (pMe->m_pMedia) {
        (void) IMEDIA_Stop(pMe->m_pMedia);
        (void) IMEDIA_Release(pMe->m_pMedia);
        pMe->m_pMedia = NULL;
    }

    uisnd_snd_stop();
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
  //lint -save -esym(550, ringerType)
  nv_item_type nvi;
  nv_stat_enum_type result;

  if(NULL == pMe) {
    MSG_ERROR("Invalid Pointer",0,0,0);
    return EBADPARM;
  }
#ifndef CUST_EDITION
    result = ui_get_nv (NV_RINGER_TYPE_I, &nvi);
#else      
    result = OEMNV_Get(NV_RINGER_TYPE_I, &nvi);
#endif 
  if(result == NV_DONE_S) { /* successfully obtain from NV */
    /* range checking */
    if (nvi.ringer_type >=(uint8) OEMALERT_RINGER_MAX) {
        nvi.ringer_type = (uint8) OEMALERT_RINGER_NORMAL;
#ifndef FEATURE_OEMUI_TASK  
      (void)ui_put_nv (NV_RINGER_TYPE_I, &nvi);
#else      
      (void)OEMNV_Put(NV_RINGER_TYPE_I, &nvi);
#endif  
    }

    *ringerType = (OEMALERTRingerType)nvi.ringer_type;
#ifndef CUST_EDITION
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

#endif /* FEATURE_PHONE_VR */


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

  if (nGlobalMediaRingerDelay > 0) {
    CALLBACK_Init(&pMe->m_acbDelayedRingCallback,
                  OEMALERT_PlayMediaCallback,
                  pMe);
    (void)ISHELL_SetTimerEx(pIShell, nGlobalMediaRingerDelay,
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
    int  iResult;
    AEECLSID    cls;

    if(NULL == pMe) {
      MSG_ERROR("Invalid Pointer",0,0,0);
      return EBADPARM;
    }

    if(pMe->m_pMedia) {
      (void) IMEDIA_Release(pMe->m_pMedia);
      pMe->m_pMedia = NULL;
    }

    mediaData.clsData = MMD_FILE_NAME;
    mediaData.pData = (void *)STRDUP(pMe->m_ringerFile);
    mediaData.dwSize = 0;

    pMe->m_pMedia = NULL;

    // Find the class ID. This function uses ISHELL_DetectType() API to get the IMedia
    // MIME and subsequently class ID...
    iResult = AEEMediaUtil_FindClass(pMe->m_pIShell, &mediaData, &cls);
    if (iResult != SUCCESS)
       return iResult;

    // Create IMedia-based object
    if (ISHELL_CreateInstance(pMe->m_pIShell, cls, (void **)&pMe->m_pMedia) || !pMe->m_pMedia)
       return MM_ENOMEDIAMEMORY;

#ifdef MM_MP4_PARM_PRIORITY
    iResult = IMEDIA_SetMediaParm(pMe->m_pMedia, 
                                  MM_MP4_PARM_PRIORITY,
                                  1,
                                  0);
#endif

    // Set the media data and put IMedia in Ready state.
    iResult = IMEDIA_SetMediaData(pMe->m_pMedia, &mediaData);
    if (iResult != SUCCESS)
    {
       IMEDIA_Release(pMe->m_pMedia);
       pMe->m_pMedia = NULL;
       return iResult;
    }

    (void) IMEDIA_Play(pMe->m_pMedia);

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

#ifdef FEATURE_PHONE_VR
  pMe->m_bVRPlaying = FALSE;
#endif /* FEATURE_PHONE_VR */

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
        uisnd_snd( UI_ALERT_SND,
		   OEMAlertRingerMap[pMe->m_ringerType].sndRinger);
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
  MSG_MED ("Stop Ringing", 0, 0, 0);

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


//lint -restore
