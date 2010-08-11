/*===========================================================================

FILE: AEESound.c

SERVICES: ISound interface implementation

DESCRIPTION
  This file contains the ISoundr interface that provides basic
  audio services like playing a tone, tone list, etc.


PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright ?1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/


/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/


/*-------------------------------------------------------------------
            Include Files
-------------------------------------------------------------------*/

// AEE includes
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "OEMSound.h"
#include "OEMObjectMgr.h"
#include "AEEStdLib.h"
#include "AEECriticalSection.h"
#include "AEEResourceCtl.h"

#include "Hs_mb6550.h"			//Add By zzg 2010_07_27

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/
typedef struct AEESound AEESound;

typedef struct AEESoundCallback
{
   // Sound Server Callback - Stuffed in sound task and sent via ISHELL_Resume...
   AEECallback             m_cb;
   AEESound *              m_pOwner;
   boolean                 m_bUsed; // Indicates if callback buffer is allocated
   AEESoundCmd             m_cbType;
   AEESoundStatus          m_cbStatus;
   AEESoundCmdData         m_cbData;
} AEESoundCallback;

struct AEESound
{
   // Declare the VTBL
   VTBL(ISound)      m_pvt;

   IShell *          m_pShell;
   uint32            m_nRefs;

   AEECallback       m_cbSysObj;
   ACONTEXT         *m_pac;
   AEESoundInfo      m_SoundInfo;

   PFNSOUNDSTATUS    m_pfnUser;
   void *            m_pUser;

   int               m_nPlaying; // indicates # of pending play tones.

   // PlayList attributes set & modified in PlayToneList() and StatusCB() methods
   boolean           m_bPlayListActive;
   uint16            m_wPlayListIndex;
   uint16            m_wPlayListCount;
   AEESoundToneData * m_pPlayList;

   uint16            m_wVolume;  // saved during SetVolume callback
   uint16            m_wVolLevels; // saved during GetLevel callback
   boolean           m_bGetVolume; // == 1/0 if GetVolume()/SetVolume() is happening

#define AEE_SOUND_CB_MAX         4
   // Callback info buffers
   AEESoundCallback  m_arCB[AEE_SOUND_CB_MAX];

   AEEObjectHandle   m_hObject;

   IResourceCtl   *  m_pResCtl;
   AEECLSID          m_clsid;
   boolean           m_bAcquired;
};

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/

static void ResetPlayListData(AEESound * pMe);

// AEE interface functions

static uint32 AEESound_AddRef(ISound * po);
static uint32 AEESound_Release(ISound * po);
static void AEESound_RegisterNotify(ISound * po, PFNSOUNDSTATUS pfn, const void * pUser);
static int  AEESound_Set(ISound* po, const AEESoundInfo* pSoundInfo);
static void AEESound_Get(ISound* po, AEESoundInfo* pSoundInfo);
static void AEESound_SetDevice(ISound* po);
static void AEESound_PlayTone(ISound* po, AEESoundToneData toneData);
static void AEESound_PlayToneList(ISound* po, AEESoundToneData* pToneData, uint16 wDataLen);
static void AEESound_PlayFreqTone(ISound* po, uint16 wHiFreq, uint16 wLoFreq, uint16 wDuration);
static void AEESound_StopTone(ISound* po);
static void AEESound_Vibrate(ISound* po, uint16 wDuration);
static void AEESound_StopVibrate(ISound* po);
static void AEESound_SetVolume(ISound* po, uint16 wVolume);
static void AEESound_GetVolume(ISound* po);
static int  AEESound_GetResourceCtl(ISound * po, IResourceCtl ** ppo);

static void AEESound_AEECallbackNotify(AEESoundCallback * pcb);
static void AEESound_StatusNotify(AEESound * pMe, AEESoundStatus eStatus);
static void AEESound_InitCallback(AEESound * pme);
static void AEESound_CancelCallback(AEESound * pme);
static AEESoundCallback * AEESound_GetCallback(AEESound * pme);
static void AEESound_FreeCallback(AEESoundCallback * pCB, AEESoundCallback * pCopy);

static int AEESound_AcquireResource(ISound * pMe);

extern int IResourceCtl_New(IShell * ps, AEECLSID cls, IBase * pQIObject, void **ppif);

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
            Global Data Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/

//Initialize the vtbl
static const VTBL(ISound) gsSoundMethods =
{
   AEESound_AddRef,
   AEESound_Release,
   AEESound_RegisterNotify,
   AEESound_Set,
   AEESound_Get,
   AEESound_SetDevice,
   AEESound_PlayTone,
   AEESound_PlayToneList,
   AEESound_PlayFreqTone,
   AEESound_StopTone,
   AEESound_Vibrate,
   AEESound_StopVibrate,
   AEESound_SetVolume,
   AEESound_GetVolume,
   AEESound_GetResourceCtl
};

static AEECriticalSection gSoundCriticalSection;

// Maximum callbacks pending

#define AEESOUND_ENTER_CRITICAL_SECTION()    AEECriticalSection_Enter(&gSoundCriticalSection); {
#define AEESOUND_LEAVE_CRITICAL_SECTION()  ;} AEECriticalSection_Leave(&gSoundCriticalSection);


// Resource management macro's
#define AEESOUND_CHECK_ACQUIRED(p) if (AEESound_AcquireResource(p)) return

/*===============================================================================
                     FUNCTION DEFINITIONS
===============================================================================*/

/*==================================================================

==================================================================*/
void Sound_Init(IShell * ps)
{
   // Initialize OEM sound shim
   OEMSound_Init();
}


static void AEESound_Delete(AEESound *pme)
{
   CALLBACK_Cancel(&pme->m_cbSysObj);
   //Set the user CBs to NULL.
   pme->m_pfnUser = NULL;
   pme->m_pUser = NULL;

   // Cancel pending callback, if any
   AEESound_CancelCallback(pme);

   OEMSound_DeleteInstance(&pme->m_SoundInfo);
   AEEObjectMgr_Unregister(pme->m_hObject);

   FREE(pme);
}

/*==================================================================
Function: Sound_New

Description:
   Called by QCOMShell to create Sound class.

Prototype:
   IBase * Sound_New(IShell * pShell, PFNCLASSsys_freeD pfnCleanup);

Parameter(s):
   IShell * pShell: Pointer to the public IShell object
   PFNCLASSsys_freeD pfnCleanup: Cleanup function pointer

Return Value: 
   IBase *: Pointer to the newly created Sound object

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
int Sound_New( IShell * pShell,AEECLSID cls, void **ppif )
{
   AEESound *  pMe;
   int         nErr;

   pMe = (AEESound *)AEE_NewClass((IBaseVtbl *)&gsSoundMethods, sizeof(AEESound));
   if (!pMe)
      return ENOMEMORY;

   nErr = AEEObjectMgr_Register(pMe, &pMe->m_hObject);
   if (nErr)
      goto Done;

   nErr = OEMSound_NewInstance(&pMe->m_SoundInfo);
   if (nErr)
      goto Done;

   // Initialize AEESoundInfo data members with default values
   pMe->m_pShell = pShell;
//   pMe->m_SoundInfo.eDevice = AEE_SOUND_DEVICE_CURRENT;
   pMe->m_SoundInfo.eMethod = AEE_SOUND_METHOD_BEEP;
   pMe->m_SoundInfo.eAPath = AEE_SOUND_APATH_LOCAL;
//   pMe->m_SoundInfo.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
//   pMe->m_SoundInfo.eMicMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;

   // Initialize callback info stuctures
   AEESound_InitCallback(pMe);

   ResetPlayListData(pMe);
   pMe->m_wVolLevels = 1;

   CALLBACK_Init(&pMe->m_cbSysObj,AEESound_Delete, pMe);
   AEE_LinkSysObject(&pMe->m_cbSysObj);

   pMe->m_pac = AEE_GetAppContext();

   if (IResourceCtl_New(pMe->m_pShell, AEECLSID_RESCTL_SOUND, (IBase *)pMe, (void **)&pMe->m_pResCtl))
      goto Done;

   *ppif = pMe;
   pMe->m_clsid = cls;   

   return SUCCESS;

Done:
   if (pMe->m_hObject)
      AEEObjectMgr_Unregister(pMe->m_hObject);
   FREE(pMe);
   return nErr;
}

/*==================================================================
Function: AEESound_AddRef

Description:
   Called to increment the object reference count.

Prototype:
   uint32 AEESound_AddRef(ISound * po);

Parameter(s):
   ISound * po: Pointer to the public ISound object

Return Value: 
   uint32: Reference count

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
static uint32 AEESound_AddRef(ISound * po)
{
   AEESound * pme = (AEESound *) po;

   return (++pme->m_nRefs);
}

/*==================================================================
Function: AEESound_Release

Description:
   Called to decrement the object reference count and if the new count
   is 0 (i.e. object isn't used) internal resources are cleaned.

Prototype:
   uint32 AEESound_Release(ISound * po);

Parameter(s):
   ISound * po: Pointer to the public ISound object

Return Value: 
   uint32: Reference count

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
static uint32 AEESound_Release(ISound * po)
{
   AEESound * pme = (AEESound *) po;

   if(--pme->m_nRefs)
         return pme->m_nRefs;

   if (pme->m_pResCtl) {
      IRESOURCECTL_Release(pme->m_pResCtl);
      pme->m_pResCtl = NULL;
   }

   AEESound_Delete(pme);

   return 0;
}

/*==================================================================
Function: AEESound_RegisterNotifyCB

Description:
   It registers the status event callback function.

Prototype:
   void AEESound_RegisterNotifyCB(ISound * po, AEE_SoundStatusCBFnType pCBFn);

Parameter(s):
   ISound * po: Pointer to the public ISound object
   AEE_SoundStatusCBFnType pCBFn: Status Callback Function Pointer

Return Value: 
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
static void AEESound_RegisterNotify(ISound * po, PFNSOUNDSTATUS pfn, const void * pUser)
{
   AEESound * pMe = (AEESound *) po;

   pMe->m_pfnUser = pfn;
   pMe->m_pUser = (void *) pUser;

}

/*==================================================================
Function: AEESound_Set

Description:

Prototype:
   int  AEESound_Set(ISound* po, const AEESoundInfo* pSoundInfo);

Parameter(s):
   ISound * po: Pointer to the public ISound object
   AEESoundInfo* pSoundInfo: New method type for sound output

Return Value: 
   None.

Comments:

Side Effects:
   None.

See Also:
   AEESound_SetDevice and AEESound_SetAPath
==================================================================*/
static int AEESound_Set(ISound* po, const AEESoundInfo* pSoundInfo)
{
   AEESound * pMe = (AEESound *) po;

   if ( !pSoundInfo )
      return EBADPARM;

   // Validate the parameters
   if ( pSoundInfo->eDevice >= AEE_SOUND_DEVICE_LAST
        || pSoundInfo->eMethod >= AEE_SOUND_METHOD_LAST
        || pSoundInfo->eAPath >= AEE_SOUND_APATH_LAST
        || pSoundInfo->eEarMuteCtl >= AEE_SOUND_MUTECTL_LAST
        || pSoundInfo->eMicMuteCtl >= AEE_SOUND_MUTECTL_LAST )
   {
      return (EBADPARM);
   }

   MEMCPY(&pMe->m_SoundInfo, pSoundInfo, sizeof(AEESoundInfo));
   return AEE_SUCCESS;
}

/*==================================================================
Function: AEESound_Get

Description:

Prototype:
   void AEESound_Get(ISound* po, AEESoundInfo* pSoundInfo);

Parameter(s):
   ISound * po: Pointer to the public ISound object
   AEESoundInfo* pSoundInfo: New method type for sound output

Return Value: 
   None.

Comments:

Side Effects:
   None.

See Also:
   AEESound_SetDevice and AEESound_SetAPath
==================================================================*/
static void AEESound_Get(ISound * po, AEESoundInfo * pSoundInfo)
{
   AEESound * pMe = (AEESound *) po;

   // Update the sound info.
   OEMSound_Get(&pMe->m_SoundInfo);

   if ( pSoundInfo )
      MEMCPY(pSoundInfo, &pMe->m_SoundInfo, sizeof(AEESoundInfo));
}

/*==================================================================
Function: AEESound_SetDevice

Description:
   It sets the device to be used for sound output. The following
   values are supported:
   
Prototype:
   void AEESound_SetDevice(ISound * po);

Parameter(s):
   ISound * po: Pointer to the public ISound object

Return Value: 
   None.

Comments:
   The default device is set to AEE_SND_DEVICE_CURRENT.

Side Effects:
   None.

See Also:
   AEESound_SetMethod and AEESound_SetAPath

==================================================================*/
static void AEESound_SetDevice( ISound * po )
{
   AEESound * pMe = (AEESound *) po;

   AEESOUND_CHECK_ACQUIRED(po);
   OEMSound_SetDevice(&pMe->m_SoundInfo, (void *)pMe->m_hObject);
}

/*==================================================================
Function: AEESound_PlayTone

Description:
   It plays a tone given a tone ID for the specified amount of time.
                           
Prototype:
   void AEESound_PlayTone(ISound * po, AEE_SoundToneIDType eToneID, uint16 u16Duration, void * pClientData);

Parameter(s):
   ISound * po: Pointer to the public ISound object
   AEE_SoundIDToneType eToneID: Tone ID to be played (see the type
                                definition for details)
   uint16 u16Duration: Tone play duration in milliseconds
   void * pClientData: Identifying data from the client

Return Value: 
   None.

Comments:
   If the duration is set to 0, the tone will play until the AEESound_StopTone
   call is issued.

Side Effects:
   None.

See Also:
   AEESound_SetMethod, AEESound_SetDevice, AEESound_SetAPath,
   AEESound_PlayFreqTone and AEESound_StopTone

==================================================================*/
static void AEESound_PlayTone(ISound* po, AEESoundToneData toneData)
{
   AEESound *  pMe = (AEESound *) po;
   
   //if ( toneData.eTone <= AEE_TONE_FIRST || toneData.eTone >= AEE_TONE_LAST )
   if ( toneData.eTone <= AEE_TONE_FIRST || toneData.eTone > AEE_TONE_FEEDBACK_STOP)
   {
      AEESound_StatusNotify(pMe, AEE_SOUND_FAILURE);
      return;
   }

   AEESOUND_CHECK_ACQUIRED(po);
   pMe->m_nPlaying++;

   OEMSound_PlayTone(&pMe->m_SoundInfo, &toneData, (void *)pMe->m_hObject);
}

/*==================================================================
Function: AEESound_PlayToneList

Description:
   It plays a tone given a tone ID for the specified amount of time.
                           
Prototype:

Parameter(s):
   ISound * po: Pointer to the public ISound object
   AEE_SoundIDToneType eToneID: Tone ID to be played (see the type
                                definition for details)
   uint16 u16Duration: Tone play duration in milliseconds
   void * pClientData: Identifying data from the client

Return Value: 
   None.

Comments:
   If the duration is set to 0, the tone will play until the AEESound_StopTone
   call is issued.

Side Effects:
   None.

See Also:
   AEESound_SetMethod, AEESound_SetDevice, AEESound_SetAPath,
   AEESound_PlayFreqTone and AEESound_StopTone

==================================================================*/
static void AEESound_PlayToneList(ISound* po, AEESoundToneData* pToneData, uint16 wDataLen)
{
   AEESound *  pMe = (AEESound *) po;
   AEESoundToneData  toneData;

   if ( wDataLen == 0 || pToneData == NULL )
   {
      AEESound_StatusNotify(pMe, AEE_SOUND_FAILURE);
      return;
   }

   // Set the app play list attributes
   pMe->m_bPlayListActive = TRUE;
   pMe->m_wPlayListIndex = 0;
   pMe->m_wPlayListCount = wDataLen;
   pMe->m_pPlayList = pToneData;

   toneData.eTone = pMe->m_pPlayList[0].eTone;
   toneData.wDuration = pMe->m_pPlayList[0].wDuration;

   AEESound_PlayTone((ISound *)pMe, toneData);
}

/*==================================================================
Function: AEESound_PlayFreqTone

Description:
   It plays a tone given a high frequecy and a low frequecy for the
   specified amount of time.
                           
Prototype:
   void AEESound_PlayFreqTone(ISound * po, uint16 u16HiFreq, uint16 u16LoFreq, uint16 u16Duration, void * pClientData);

Parameter(s):
   ISound * po: Pointer to the public ISound object
   uint16 u16HiFreq: Higher frequcy of the DTMF pair
   uint16 u16LoFreq: Lower frequcy of the DTMF pair
   uint16 u16Duration: Tone play duration in milliseconds
   void * pClientData: Identifying data from the client

Return Value: 
   None.

Comments:
   If the duration is set to 0, the tone will play until the AEESound_StopTone
   call is issued.

Side Effects:
   The result of the operation will be sent to the client via the
   callback function pointer. Also, when the operation is done, it
   notifies the client through the same callback function pointer.

See Also:
   AEESound_SetMethod, AEESound_SetDevice, AEESound_SetAPath,
   AEESound_PlayTone and AEESound_StopTone

==================================================================*/
static void AEESound_PlayFreqTone(ISound* po, uint16 wHiFreq, uint16 wLoFreq, uint16 wDuration)
{
   AEESound * pMe = (AEESound *) po;
   
   pMe->m_nPlaying++;

   AEESOUND_CHECK_ACQUIRED(po);
   OEMSound_PlayFreqTone(&pMe->m_SoundInfo, wHiFreq, wLoFreq, wDuration, (void *)pMe->m_hObject);
}

/*==================================================================
Function: AEESound_StopTone

Description:
   It stops playing the current tone.
                           
Prototype:
   void AEESound_StopTone(ISound * po, void * pClientData);

Parameter(s):
   ISound * po: Pointer to the public ISound object
   void * pClientData: Identifying data from the client

Return Value: 
   None.

Comments:
   None.

Side Effects:
   The result of the operation will be sent to the client via the
   callback function pointer. Also, when the operation is done, it
   notifies the client through the same callback function pointer.

See Also:
   AEESound_PlayTone and AEESound_PlayFreqTone

==================================================================*/
static void AEESound_StopTone( ISound * po )
{
   AEESound *  pMe = (AEESound *) po;

   if (pMe->m_bPlayListActive)
   {
      if (pMe->m_wPlayListIndex < pMe->m_wPlayListCount) {
         AEESOUND_CHECK_ACQUIRED(po);
         OEMSound_StopTone(pMe->m_bPlayListActive, (void *)pMe->m_hObject);
      }

      pMe->m_pPlayList = NULL;
      return;
   }

   AEESOUND_CHECK_ACQUIRED(po);
   OEMSound_StopTone(pMe->m_bPlayListActive, (void *)pMe->m_hObject);
}

/*==================================================================
Function: AEESound_Vibrate

Description:
   It causes the handset to vibrate for the specified amount to time
   if the feature is supported; otherwise, it doesn't do anything.
                           
Prototype:
   void AEESound_Vibrate(ISound * po, uint16 u16Duration, void * pClientData);

Parameter(s):
   ISound * po: Pointer to the public ISound object
   uint16 u16Duration: Duration of vibration in milliseconds
   void * pClientData: Identifying data from the client

Return Value: 
   None.

Comments:
   None.

Side Effects:
   The result of the operation will be sent to the client via the
   callback function pointer.

See Also:
   AEESound_StopVibrate

==================================================================*/
static void AEESound_Vibrate(ISound* po, uint16 wDuration)
{
   AEESound *  pMe = (AEESound *)po;

   AEESOUND_CHECK_ACQUIRED(po);
   OEMSound_Vibrate(wDuration, (void *)pMe->m_hObject);
}

/*==================================================================
Function: AEESound_StopVibrate

Description:
   It stops the current vibration.
                           
Prototype:
   void AEESound_StopVibrate(ISound * po, void * pClientData);

Parameter(s):
   ISound * po: Pointer to the public ISound object
   void * pClientData: Identifying data from the client

Return Value: 
   None.

Comments:
   None.

Side Effects:
   The result of the operation will be sent to the client via the
   callback function pointer.

See Also:
   AEESound_Vibrate

==================================================================*/
static void AEESound_StopVibrate( ISound * po )
{
   AEESound *  pMe = (AEESound *)po;

   AEESOUND_CHECK_ACQUIRED(po);
   OEMSound_StopVibrate((void *)pMe->m_hObject);
}

/*==================================================================
Function: AEESound_SetVolume

Description:
   It sets the volume to be used for the device/method pair.

Prototype:
   void AEESound_SetVolume(ISound * po, uint8 u8Volume, void * pClientData);

Parameter(s):
   ISound * po: Pointer to the public ISound object
   uint8 u8Volume: New volume level for device/method
   void * pClientData: Identifying data from the client
   
Return Value: 
   None.

Comments:
   The result of the operation will be sent to the client via the callback
   function pointer.

Side Effects:
   None.

See Also:
   AEESound_GetVolume, AEESound_SetDevice and AEESound_SetMethod

==================================================================*/
static void AEESound_SetVolume(ISound* po, uint16 wVolume)
{
    AEESound * pMe = (AEESound *) po;

    AEESOUND_CHECK_ACQUIRED(po);
      
    // First get the max volume level. Then in the AEESound_LevelCB, set the volume
    pMe->m_wVolume = wVolume;  // Save the volume
    pMe->m_bGetVolume = FALSE;

    //Add By zzg 2010_07_27  播放音乐里调音量没反应
    if (HS_HEADSET_ON())
    {
    	(&pMe->m_SoundInfo)->eDevice = AEE_SOUND_DEVICE_STEREO_HEADSET;
    }
    //Add End
    
    OEMSound_GetLevels(&pMe->m_SoundInfo, (void *)pMe->m_hObject);
}

/*==================================================================
Function: AEESound_GetVolume

Description:
   It retrieves the current volume used for the device/method pair.

Prototype:
   void AEESound_GetVolume(ISound * po, void * pClientData);

Parameter(s):
   ISound * po: Pointer to the public ISound object
   void * pClientData: Identifying data from the client

Return Value: 
   None.

Comments:
   None.

Side Effects:
   The volume level will be sent back to the client via the callback
   function pointer.

See Also:
   AEESound_SetVolume, AEESound_SetDevice and AEESound_SetMethod

==================================================================*/
static void AEESound_GetVolume(ISound* po)
{
   AEESound * pMe = (AEESound *)po;
   
   pMe->m_bGetVolume = TRUE;

   AEESOUND_CHECK_ACQUIRED(po);

   // First get the max volume level. Then in the AEESound_LevelCB, get the volume
   OEMSound_GetLevels(&pMe->m_SoundInfo, (void *)pMe->m_hObject);
}

// Internal callback functions
void AEESound_StatusCB( const void * pClientData, AEESoundStatus eStatus )
{
   AEESound *           pMe;
   AEESoundCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   AEESOUND_ENTER_CRITICAL_SECTION()

   pMe = AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pMe)
      goto CBExit;
   
   pcb = AEESound_GetCallback(pMe);

   // Fatal error: If no callback info space available, return
   if ( !pcb )
      goto CBExit;

   // Copy callback info
   pcb->m_cbType = AEE_SOUND_STATUS_CB;
   pcb->m_cbStatus = eStatus;
   MEMSET(&pcb->m_cbData, (int)0, sizeof(pcb->m_cbData));

   bResume = TRUE;

CBExit:
   AEESOUND_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback((AEECallback *)pcb, pMe->m_pac);
}

void AEESound_VolumeCB( const void * pClientData, AEESoundStatus eStatus, uint16 wVolume )
{
   AEESound *           pMe;
   AEESoundCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   AEESOUND_ENTER_CRITICAL_SECTION()

   pMe = AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pMe)
      goto CBExit;
   
   pcb = AEESound_GetCallback(pMe);

   // Fatal error: If no callback info space available, return
   if ( !pcb )
      goto CBExit;

   // Copy callback info
   pcb->m_cbType = AEE_SOUND_VOLUME_CB;
   pcb->m_cbStatus = eStatus;
   pcb->m_cbData.wVolume = (uint16) (wVolume * (AEE_MAX_VOLUME / pMe->m_wVolLevels));
   bResume = TRUE;

CBExit:
   AEESOUND_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback((AEECallback *)pcb, pMe->m_pac);
}

void AEESound_LevelCB( const void * pClientData, AEESoundStatus eStatus, uint16 wLevels )
{
   AEESOUND_ENTER_CRITICAL_SECTION()

   AEESound *  pMe = AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   uint16      wSetLevel = 0;
   
   if (!pMe) {
      goto CBExit;
   }
   
   // Now, set the volume, the resulting status callback is queued in AEESound_StatusCB
   switch ( eStatus )
   {
      case AEE_SOUND_SUCCESS:
         pMe->m_wVolLevels = wLevels > 0 ? wLevels : 1;
         wSetLevel = (uint16) (pMe->m_wVolume / (int)(AEE_MAX_VOLUME/pMe->m_wVolLevels));
         if (pMe->m_bGetVolume)
	       {
            OEMSound_GetVolume(&pMe->m_SoundInfo, (void *)pMe->m_hObject);
	       }
         else
	       {	       	
            OEMSound_SetVolume(&pMe->m_SoundInfo, wSetLevel, (void *)pMe->m_hObject);
	       }   
         break;

      case AEE_SOUND_FAILURE: // fall through....
      default:
         AEESound_StatusCB((void *)pMe->m_hObject, eStatus);
         break;
   }

CBExit:
   AEESOUND_LEAVE_CRITICAL_SECTION()
}

static int AEESound_GetResourceCtl(ISound * po, IResourceCtl ** ppo)
{
   AEESound * pMe = (AEESound *)po;

   if (ppo == NULL)
      return EBADPARM;

   if (pMe->m_pResCtl) {
      IRESOURCECTL_AddRef(pMe->m_pResCtl);
      *ppo = pMe->m_pResCtl;
      return SUCCESS;
   }
   else
      return EFAILED;
}

// This function is called in AEE context.
// It retrieves the callback, converts it to AEE, if necessary, calls user callback
// and sys_frees the callback memory
static void AEESound_AEECallbackNotify(AEESoundCallback * pcb)
{
   AEESound *           pMe;
   AEESoundCallback     cbCopy;

   if (!pcb)
      return;

   pMe = pcb->m_pOwner;

   if( !pMe )
      return;

   AEESound_FreeCallback(pcb, &cbCopy);
   pcb = NULL;

   ISOUND_AddRef((ISound *)pMe);
   if ( pMe->m_pfnUser ) {
      pMe->m_pfnUser(pMe->m_pUser, cbCopy.m_cbType, cbCopy.m_cbStatus, (uint32)&cbCopy.m_cbData);
   }
   ISOUND_Release((ISound *)pMe);

   // If callback is AEE_SOUND_STATUS_CB, then check if it is due to PlayTone()
   // or PlayToneList() calls
   if ( cbCopy.m_cbType == AEE_SOUND_STATUS_CB && pMe->m_nPlaying )
   {
      if (cbCopy.m_cbStatus == AEE_SOUND_PLAY_DONE || cbCopy.m_cbStatus == AEE_SOUND_FAILURE)
      {
         if (--pMe->m_nPlaying < 0)
            pMe->m_nPlaying = 0;
      }

      // If AEE_SOUND_PLAY_DONE is received, then check if playlist is active
      if ( cbCopy.m_cbStatus == AEE_SOUND_PLAY_DONE && pMe->m_bPlayListActive)
      {
         if ( pMe->m_pPlayList && ++pMe->m_wPlayListIndex < pMe->m_wPlayListCount )
            AEESound_PlayTone((ISound*)pMe, pMe->m_pPlayList[pMe->m_wPlayListIndex]);
         else
            pMe->m_bPlayListActive = FALSE;  // play list has ended
      }
      else if ( cbCopy.m_cbStatus == AEE_SOUND_FAILURE )
         pMe->m_bPlayListActive = FALSE;

   }
}

static void ResetPlayListData(AEESound * pMe)
{
   pMe->m_bPlayListActive = FALSE;
   pMe->m_wPlayListIndex = 0;
   pMe->m_wPlayListCount = 0;
   pMe->m_pPlayList = NULL;
}

static void AEESound_InitCallback(AEESound * pme)
{
   int   i;

   for (i = 0; i < AEE_SOUND_CB_MAX; i++)
   {
      AEESoundCallback *   pcb = &pme->m_arCB[i];
      CALLBACK_Init((AEECallback *)pcb, (PFNNOTIFY)AEESound_AEECallbackNotify, pcb);
      pcb->m_pOwner = pme;
   }
}

static void AEESound_CancelCallback(AEESound * pme)
{
   int   i;

   for (i = 0; i < AEE_SOUND_CB_MAX; i++)
   {
      AEESoundCallback *   pcb = &pme->m_arCB[i];
      CALLBACK_Cancel((AEECallback *)pcb);
      pcb->m_pOwner = NULL;
   }
}

static AEESoundCallback * AEESound_GetCallback(AEESound * pme)
{
   int                  i;
   AEESoundCallback *   pcb = NULL;

   for (i = 0; i < AEE_SOUND_CB_MAX; i++)
   {
      if (!pme->m_arCB[i].m_bUsed)
      {
         pme->m_arCB[i].m_bUsed = TRUE;
         pcb = &pme->m_arCB[i];
         break;
      }
   }

   return pcb;
}

static void AEESound_FreeCallback(AEESoundCallback * pcb, AEESoundCallback * pcbCopy)
{

   if (!pcb || !pcbCopy)
      return;

   AEESOUND_ENTER_CRITICAL_SECTION()

   MEMCPY(pcbCopy, pcb, sizeof(AEESoundCallback));
   
   pcb->m_bUsed = FALSE;

   AEESOUND_LEAVE_CRITICAL_SECTION()
}

static void AEESound_StatusNotify(AEESound * pMe, AEESoundStatus eStatus)
{
   ISOUND_AddRef((ISound *)pMe);
   if ( pMe->m_pfnUser ) {
      pMe->m_pfnUser(pMe->m_pUser, AEE_SOUND_STATUS_CB, eStatus, (uint32)0);
   }
   ISOUND_Release((ISound *)pMe);
}

static int AEESound_AcquireResource(ISound * po)
{
   AEESound *  pMe;
   uint32      dwStatus;
   int         nErr = 0;

   pMe = (AEESound *)po;
   pMe->m_bAcquired = FALSE;

   // Initialize the resource control object
   if (pMe->m_pResCtl) {
      dwStatus = IRESOURCECTL_GetStatus(pMe->m_pResCtl, NULL);
      switch (dwStatus) {
         // Other application is holding the control.
         case RESCTL_STATUS_BUSY:
            nErr = EITEMBUSY;
            break;

         // Already acquired
         case RESCTL_STATUS_ACQUIRED:
         case RESCTL_STATUS_FREE:
            nErr = SUCCESS;
            break;

         default:
            nErr = EFAILED;
      }
   } else {
      nErr = EFAILED;
   }

   if (nErr == EFAILED)
      AEESound_StatusCB((void *)pMe->m_hObject, AEE_SOUND_FAILURE);
   else
   if (nErr == EITEMBUSY) {
      if (pMe->m_clsid == AEECLSID_SOUND)
         AEESound_StatusCB((void *)pMe->m_hObject, AEE_SOUND_RESOURCE_BUSY);
      else
         AEESound_StatusCB((void *)pMe->m_hObject, AEE_SOUND_FAILURE);
   }

   return nErr;
}

