/*=============================================================================

FILE: OEMSound.c

SERVICES: OEM Functions for Basic Sound Services

GENERAL DESCRIPTION:
A sample implementation of the Sound functions. The functions in this
file are essentially wrappers to QCT's Sound functions.
  
PUBLIC CLASSES AND STATIC FUNCTIONS:

   OEMSound_PlayTone

INITIALIZATION AND SEQUENCING REQUIREMENTS:
No additional module-level initialization or sequencing requirements.

        Copyright ?? 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/
#include <stdlib.h>
#include "OEMFeatures.h"

#ifdef AEE_SIMULATOR
#define TG  T_G
#define TARG_H "target.h"
#endif

/*-------------------------------------------------------------------
            Include Files
-------------------------------------------------------------------*/
// **NOTE: OEMs should define FEATURE_UIXSND_IMPLEMENTATION if they plan to use
// the uixsnd.c implementation of sound. If not defined then the snd.c implementation 
// will be used.

#ifdef FEATURE_UIXSND_IMPLEMENTATION
#include "uixsnd.h"
#endif

#include "snd.h"


#include "OEMSound.h"
#include "OEMCriticalSection.h"
#include "AEE_OEMSound.h"
#include "AEEComdef.h"
#include "AEEStdLib.h"
#if defined(FEATURE_ACM) && !defined(FEATURE_ODM_UI)
#include "AEEShell.h"
#include "AEEClassIDs.h"
#include "AEE_OEM.h"
#include "OEMHeap.h"
#include "OEMACM.h"
#endif // FEATURE_ACM && !FEATURE_ODM_UI

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/
typedef struct
{
   uint16   sndStatus[(int)AEE_SOUND_LAST]; // snd_status_type
   uint16   sndDevice[(int)AEE_SOUND_DEVICE_LAST]; // snd_device_type
   uint16   sndMethod[(int)AEE_SOUND_METHOD_LAST]; // snd_method_type
   uint16   sndToneBase; // == SND_FIRST_TDB_TONE
   uint16   sndControlBase; // == SND_FIRST_CONTROL_TONE
} AEESoundCaps;

typedef struct CAudioDevice
{
   int32          m_nRefs;
   boolean        m_bVibratorON;
} CAudioDevice;

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/
static AEESoundCaps  gsSoundCaps;

#ifndef FEATURE_UIXSND_IMPLEMENTATION
static boolean  gsVibrateON = FALSE;
#endif

static int gsRefs = 0;
static AEESoundInfo gsDevice;

static OEMCriticalSection gSoundCriticalSection;

#if defined(FEATURE_ACM) && !defined(FEATURE_ODM_UI)
#define OPERATION_PLAY_TONE        1
#define OPERATION_PLAY_FREQ_TONE   2
#define OPERATION_VIBRATE          3

// This is the maximum additional time that we will be waiting for
// the snd or vibration callback.
// Need to be big enough to cover the normal delay
#define MAX_CALLBACK_DELAY_MS      200

// Maximum sound operation requests that ACM can handle
// at the same time
#define MAX_SOUND_OPERATION_REQUESTS 5

// pointer to shell
static IShell* pOEMSoundShell;

typedef struct
{
   int                      operationType; // play tone, dual tone, or vibration
   void                    *uniqueID;      // uniqueID for this entry
   AEESoundTone             tone;          // for single tone playing
   int                      hiFreq;        // for freq tone playing
   int                      loFreq;
   int                      duration;      // duration for tone, dual tone or vibration
   AEESoundInfo             soundInfo;     // saved AEESoundInfo for delayed acquiring

   ACM_resource_state_type  acmState;      // ACM state for this entry
   ACM_Callback_data_type   cbAcmData;     // acm callback data for force release and
                                           // delayed acquiring
   boolean                  forceReleased; // whether this entry has been force released
} sound_acmCBdata;

static sound_acmCBdata sound_acmCBarray[MAX_SOUND_OPERATION_REQUESTS];
#endif // FEATURE_ACM && !FEATURE_ODM_UI


/*---------------------------------------------------------------------------
    Helper Function Prototypes
---------------------------------------------------------------------------*/

// Internal Callback functions
static void OEMSound_StatusCB( const void * pClientData, snd_status_type status );
static void OEMSound_VolumeCB( const void * pClientData, snd_status_type status, uint16 wVolume );
static void OEMSound_LevelCB( const void * pClientData, snd_status_type status, uint16 wLevel );

// AEE to/from snd enum conversions
static AEESoundStatus OEMSound_GetAEESoundStatus(snd_status_type e);
static snd_device_type OEMSound_GetSndDevice(AEESoundDevice e);
static snd_method_type OEMSound_GetSndMethod(AEESoundMethod e);
static snd_tone_type OEMSound_GetSndTone(AEESoundTone e);
#ifdef FEATURE_UIXSND_IMPLEMENTATION
static AEESoundDevice OEMSound_GetAEESndDevice(snd_device_type  e);
#endif // FEATURE_UIXSND_IMPLEMENTATION

#if defined(FEATURE_ACM) && !defined(FEATURE_ODM_UI)

// the peer version of OEMSound_StatusCB with ACM feature turned on
// use to receivet the status callback from the snd layer when
// playing tone or dual tone.
static void OEMSound_StatusCB_With_ACM         (const void * pClientData,
                                                snd_status_type status );

static sound_acmCBdata *OEMSound_Get_AcmCBdata (void);

static int  OEMSound_ACM_Acquire               (sound_acmCBdata *pSoundCBdata);
static void OEMSound_ACM_Free                  (sound_acmCBdata *pSoundCBdata);

// callback for force release or delayed acquiring
static void OEMSound_Tone_ACM_StatusCB         (void *uniqueID);
static void OEMSound_Vibrate_ACM_StatusCB      (void *uniqueID);

// timer callback for tone or vibration, the timer callback will
// be fired if the tone playing or vibration callback does not
// callback after the grace period
static void OEMSound_Tone_ACM_TimerCallback    (void *pClientData);
static void OEMSound_Vibrate_ACM_TimerCallback (void *pClientData);
#endif // FEATURE_ACM && !FEATURE_ODM_UI


/*==================================================================
Function: OEMSound_Init

Description: Maps Sound enumerations to AEE_SOUNDPLAYER enumerations

Prototype:
   void OEM_GetMMCaps( AEEMMCaps * pCaps );

Parameter(s):  
   pCaps [out]:  MM capabilities structure

Return Value:  Floating point 
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEMSound_Init(void)
{
   int ii;
   for(ii=0; ii<AEE_SOUND_LAST; ++ii) {
      gsSoundCaps.sndStatus[ii] = (uint16)SND_FAILURE;
   }
   // Mapping: snd_status_type to AEESoundStatus
   gsSoundCaps.sndStatus[(int)AEE_SOUND_UNKNOWN]     = (uint16) SND_FAILURE;
   gsSoundCaps.sndStatus[(int)AEE_SOUND_SUCCESS]     = (uint16) SND_SUCCESS;
   gsSoundCaps.sndStatus[(int)AEE_SOUND_PLAY_DONE]   = (uint16) SND_PLAY_DONE;
   gsSoundCaps.sndStatus[(int)AEE_SOUND_FAILURE]     = (uint16) SND_FAILURE;

   for(ii=0; ii<AEE_SOUND_DEVICE_LAST; ++ii) {
      gsSoundCaps.sndDevice[ii] = (uint16)SND_DEVICE_CURRENT;
   }
   // Mapping: snd_device_type to AEESoundDevice
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_UNKNOWN]    = (uint16) SND_DEVICE_MAX;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_HANDSET]    = (uint16) SND_DEVICE_HANDSET;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_HFK]        = (uint16) SND_DEVICE_HFK;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_HEADSET]    = (uint16) SND_DEVICE_HEADSET;
#ifdef FEATURE_ANALOG_HFK
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_AHFK]       = (uint16) SND_DEVICE_AHFK;
#else
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_AHFK]       = (uint16) SND_DEVICE_CURRENT;
#endif
#ifdef FEATURE_STEREO_DAC
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_SDAC]       = (uint16) SND_DEVICE_SDAC;
#else
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_SDAC]       = (uint16) SND_DEVICE_CURRENT;
#endif
#ifdef FEATURE_TTY
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_HFK]    = (uint16) SND_DEVICE_TTY_HFK;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_HEADSET]= (uint16) SND_DEVICE_TTY_HEADSET;
#else
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_HFK]    = (uint16) SND_DEVICE_CURRENT;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_HEADSET]= (uint16) SND_DEVICE_CURRENT;
#endif
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_CURRENT]    = (uint16) SND_DEVICE_CURRENT;
#ifdef FEATURE_SPEAKER_PHONE
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_SPEAKER]    = (uint16) SND_DEVICE_SPEAKER_PHONE;
#else
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_SPEAKER]    = (uint16) SND_DEVICE_CURRENT;
#endif
#ifdef FEATURE_BT_AG
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_BT_HEADSET] = (uint16) SND_DEVICE_BT_HEADSET;   
#else
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_BT_HEADSET] = (uint16) SND_DEVICE_CURRENT;
#endif
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_BT_STEREO_HEADSET] = (uint16) SND_DEVICE_CURRENT;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_STEREO_HEADSET] = (uint16) SND_DEVICE_STEREO_HEADSET;
#ifdef FEATURE_FM_OEM
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_HEADSET_FM] = (uint16) SND_DEVICE_HEADSET_FM;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_SPEAKER_FM] = (uint16) SND_DEVICE_SPEAKER_FM;

#endif
#ifdef FEATURE_USB_ISOC_AUDIO
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_USB] = (uint16) SND_DEVICE_USB;
#else
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_USB] = (uint16) SND_DEVICE_CURRENT;
#endif
#ifdef FEATURE_TTY
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_HCO] = (uint16) SND_DEVICE_TTY_HCO;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_VCO] = (uint16) SND_DEVICE_TTY_VCO;
#else
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_HCO] = (uint16) SND_DEVICE_CURRENT;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_VCO] = (uint16) SND_DEVICE_CURRENT;
#endif
#ifdef FEATURE_TTY_FULL
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_FULL] = (uint16) SND_DEVICE_TTY_FULL;
#else
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_FULL] = (uint16)SND_DEVICE_CURRENT;
#endif
#ifdef CUST_EDITION	  
#if defined( FEATURE_SOUND_DEVICE_HEADSET_SPEAKER)
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_HEADSET_SPEAKER] = (uint16)SND_DEVICE_HEADSET_SPEAKER;
#endif
#endif /*CUST_EDITION*/
   for(ii=0; ii<AEE_SOUND_METHOD_LAST; ++ii) {
      gsSoundCaps.sndMethod[ii] = (uint16)SND_METHOD_MAX;
   }
   // Mapping: snd_method_type to AEESoundMethod
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_UNKNOWN]    = (uint16) SND_METHOD_MAX;
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_VOICE]      = (uint16) SND_METHOD_VOICE;
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_BEEP]       = (uint16) SND_METHOD_KEY_BEEP;
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_MESSAGE]    = (uint16) SND_METHOD_MESSAGE;
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_RING]       = (uint16) SND_METHOD_RING;
#ifdef FEATURE_CLICK
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_CLICK]      = (uint16) SND_METHOD_CLICK;
#else
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_CLICK]      = (uint16) SND_METHOD_MAX;
#endif
#ifdef FEATURE_AUDIO_FORMAT
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_MIDI]       = (uint16) SND_METHOD_MIDI;
#else
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_MIDI]       = (uint16) SND_METHOD_MAX;
#endif
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_AUX]        = (uint16) SND_METHOD_AUX;

   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_AUX+1]      = (uint16) SND_METHOD_AUX_LINE_IN;

   gsSoundCaps.sndToneBase = SND_FIRST_TDB_TONE;
   gsSoundCaps.sndControlBase = SND_FIRST_CONTROL_TONE;

#if defined(FEATURE_ACM) && !defined(FEATURE_ODM_UI)
   pOEMSoundShell = AEE_GetShell();

   {
      int i = 0;
      for (i = 0; i < MAX_SOUND_OPERATION_REQUESTS; i++)
      {
         sound_acmCBarray[i].acmState = ACM_RESOURCE_UNKNOWN;
         sound_acmCBarray[i].uniqueID = 0;
      }
   }
#endif // FEATURE_ACM && !FEATURE_ODM_UI
}

/*===========================================================================
FUNCTION OEMSound_NewInstance

DESCRIPTION
  This function increases ref count of underlying audio device. It returns
  current AEESoundInfo.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int OEMSound_NewInstance(AEESoundInfo * psi)
{
   if (!psi)
      return EFAILED;

   if((gsRefs++) == 0) {
      OEMSound_Get(&gsDevice);
   }
   OEMSound_Get(psi);
   // Set the path to default
   psi->eAPath = AEE_SOUND_APATH_LOCAL;
   psi->eMethod = AEE_SOUND_METHOD_BEEP;

   return SUCCESS;
}


#ifdef FEATURE_UIXSND_IMPLEMENTATION
/*===========================================================================
FUNCTION OEMSound_DeleteInstance

DESCRIPTION
  This function decreases ref count of underlying audio device and restores
  the default settings when ref count goes to zero.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_DeleteInstance(AEESoundInfo * psi)
{
   AEESoundInfo   si;

   if (gsRefs <= 0)
      return;

   if (--gsRefs > 0)
      return;

   //
   // Restore the state to that before the creation of first ISound instance.
   // Note: 
   // (1) Here only device setting is restored
   // (2) OEMs can restore the volume settings for various methods.
   OEMSound_Get(&si);
   if (si.eEarMuteCtl != gsDevice.eEarMuteCtl ||
       si.eMicMuteCtl != gsDevice.eMicMuteCtl )
   {
      uisnd_force_device_mute( OEMSound_GetSndDevice(si.eDevice),
                      (snd_mute_control_type)gsDevice.eEarMuteCtl,
                      (snd_mute_control_type)gsDevice.eMicMuteCtl,
                      NULL, 
                      NULL);
   }
}

/*===========================================================================

FUNCTION OEMSound_Get

DESCRIPTION
  This function gets the sound output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void OEMSound_Get(AEESoundInfo * psi)
{
   uisnd_notify_data_s_type notifyData;

   if (!psi)
      return;

   uisnd_get_device(&notifyData);

   //
   // NOTE to OEMs:
   // (1) Get the sound device info from your native layer.
   // (2) Do not modify psi->eMethod.and psi->eAPath
   //
   psi->eDevice = OEMSound_GetAEESndDevice(notifyData.out_device);
   psi->eEarMuteCtl = (AEESoundMuteCtl) (notifyData.speaker_mute);
   psi->eMicMuteCtl = (AEESoundMuteCtl)(notifyData.microphone_mute);
}

/*===========================================================================

FUNCTION OEMSound_SetDevice

DESCRIPTION
  This function issues a command to set the sound output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void OEMSound_SetDevice(AEESoundInfo * psi, void * pUser)
{
   //
   // NOTE to OEMs:
   // (1) Update this sound device in your native layer.
   // (2) Ignore psi->eMethod.
   //
   uisnd_force_device_mute( OEMSound_GetSndDevice(psi->eDevice),
                                    (snd_mute_control_type)psi->eEarMuteCtl,
                                    (snd_mute_control_type)psi->eMicMuteCtl,
                                    &OEMSound_StatusCB, 
                                    pUser);
}

/*===========================================================================
FUNCTION OEMSound_PlayTone

DESCRIPTION
  This function issues a command to play a specified tone.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_PlayTone(AEESoundInfo * psi, AEESoundToneData* toneData, void * pUser)
{
#if defined(FEATURE_ACM) && !defined(FEATURE_ODM_UI)

   sound_acmCBdata *pSoundCBdata = OEMSound_Get_AcmCBdata ();

   if (pSoundCBdata == NULL)
   {
      AEESound_StatusCB (pUser, AEE_SOUND_RESOURCE_BUSY);
      return;
   }

   pSoundCBdata->operationType = OPERATION_PLAY_TONE;
   pSoundCBdata->uniqueID      = pUser;
   pSoundCBdata->forceReleased = FALSE;

   if (OEMSound_ACM_Acquire (pSoundCBdata) != SUCCESS)
   {
      AEESound_StatusCB (pUser, AEE_SOUND_RESOURCE_BUSY);
      return;
   }
   else if (pSoundCBdata->acmState == ACM_RESOURCE_ACQUIRING)
   {
      pSoundCBdata->tone     = toneData->eTone;
      pSoundCBdata->duration = toneData->wDuration;
      MEMCPY (&pSoundCBdata->soundInfo, psi, sizeof (AEESoundInfo));
      return;
   }
   else
   {
      // acquired
      ISHELL_SetTimer (pOEMSoundShell,
                       toneData->wDuration + MAX_CALLBACK_DELAY_MS,
                       OEMSound_Tone_ACM_TimerCallback,
                       (void *) pSoundCBdata);

      uisnd_play_tone_generic( OEMSound_GetSndDevice(psi->eDevice),
                               OEMSound_GetSndMethod(psi->eMethod),
                               OEMSound_GetSndTone(toneData->eTone),
                               toneData->wDuration,
                               (snd_apath_type)psi->eAPath,
                               &OEMSound_StatusCB_With_ACM,
                               pUser);
   }
#else // !FEATURE_ACM || FEATURE_ODM_UI
   uisnd_play_tone_generic( OEMSound_GetSndDevice(psi->eDevice),
                            OEMSound_GetSndMethod(psi->eMethod), 
                            OEMSound_GetSndTone(toneData->eTone), 
                            toneData->wDuration, 
                            (snd_apath_type)psi->eAPath, 
                            &OEMSound_StatusCB, 
                            pUser);                   
#endif //FEATURE_ACM && !FEATURE_ODM_UI
}


/*===========================================================================
FUNCTION OEMSound_PlayFreqTone

DESCRIPTION
  This function issues a command to play a specified pair of tone frequencies. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_PlayFreqTone(AEESoundInfo * psi, uint16 wHiFreq, uint16 wLoFreq, uint16 wDuration, void * pUser)
{
#if defined(FEATURE_ACM) && !defined(FEATURE_ODM_UI)

   sound_acmCBdata *pSoundCBdata = OEMSound_Get_AcmCBdata ();

   if (pSoundCBdata == NULL)
   {
      AEESound_StatusCB (pUser, AEE_SOUND_RESOURCE_BUSY);
      return;
   }

   pSoundCBdata->operationType = OPERATION_PLAY_FREQ_TONE;
   pSoundCBdata->uniqueID      = pUser;
   pSoundCBdata->forceReleased = FALSE;

   if (OEMSound_ACM_Acquire (pSoundCBdata) != SUCCESS)
   {
      AEESound_StatusCB (pUser, AEE_SOUND_RESOURCE_BUSY);
      return;
   }
   else if (pSoundCBdata->acmState == ACM_RESOURCE_ACQUIRING)
   {
      pSoundCBdata->hiFreq   = wHiFreq;
      pSoundCBdata->loFreq   = wLoFreq;
      pSoundCBdata->duration = wDuration;
      MEMCPY (&pSoundCBdata->soundInfo, psi, sizeof (AEESoundInfo));
      return;
   }
   else
   {
      // acquired
      ISHELL_SetTimer (pOEMSoundShell,
                       wDuration + MAX_CALLBACK_DELAY_MS,
                       OEMSound_Tone_ACM_TimerCallback,
                       (void *) pSoundCBdata);

      uisnd_freq_tone_start( OEMSound_GetSndDevice(psi->eDevice),
                             OEMSound_GetSndMethod(psi->eMethod),
                             wHiFreq,
                             wLoFreq,
                             wDuration,
                             (snd_apath_type) psi->eAPath,
                             &OEMSound_StatusCB_With_ACM,
                             pUser);
   }
#else // !FEATURE_ACM || FEATURE_ODM_UI
   uisnd_freq_tone_start( OEMSound_GetSndDevice(psi->eDevice),
                        OEMSound_GetSndMethod(psi->eMethod), 
                        wHiFreq, 
                        wLoFreq, 
                        wDuration, 
                        (snd_apath_type) psi->eAPath,
                        &OEMSound_StatusCB, 
                        pUser);
#endif // FEATURE_ACM && !FEATURE_ODM_UI
}

/*===========================================================================
FUNCTION OEMSound_Proceed_StopTone (snd_cb_func_ptr_type callback, void * pUser)

DESCRIPTION
  This function issues a command to stop playing the current tone.

COMMENTS
   In ACM world, will only be called if the vibration was started
   by the same user.

SIDE EFFECTS
  None.
===========================================================================*/
static void OEMSound_Proceed_StopTone(snd_cb_func_ptr_type callback, void * pUser)
{
   uisnd_tone_stop_generic(callback, pUser);
}

/*===========================================================================
FUNCTION OEMSound_StopTone

DESCRIPTION
  This function issues a command to stop playing a single tone or a playlist

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_StopTone(boolean bPlayList, void * pUser)
{
#if defined(FEATURE_ACM) && !defined(FEATURE_ODM_UI)
   sound_acmCBdata   *pSoundCBdata;
   int                i;

   // check whether this pUser is the one that currently
   // holds the resource for tone or dual tone playing
   pSoundCBdata = sound_acmCBarray;
   for (i = 0; i < MAX_SOUND_OPERATION_REQUESTS; i++)
   {
      if ((pSoundCBdata->operationType == OPERATION_PLAY_TONE ||
           pSoundCBdata->operationType == OPERATION_PLAY_FREQ_TONE) &&
          pSoundCBdata->uniqueID == pUser)
      {
         if (pSoundCBdata->acmState == ACM_RESOURCE_ACQUIRED)
         {
            // cancels the timer for playing the tone
            ISHELL_CancelTimer (pOEMSoundShell,
                                OEMSound_Tone_ACM_TimerCallback,
                                pSoundCBdata);

            // sets the guard timer for stopping the tone in case
            // callback for stopping the tone does not get deilivered
            ISHELL_SetTimer (pOEMSoundShell,
                             MAX_CALLBACK_DELAY_MS,
                             OEMSound_Tone_ACM_TimerCallback,
                             (void *) pSoundCBdata);

            // goes ahead and stops the tone
            OEMSound_Proceed_StopTone ((snd_cb_func_ptr_type) &OEMSound_StatusCB_With_ACM,
                                       pUser);

            return;
         }
         else if (pSoundCBdata->acmState == ACM_RESOURCE_ACQUIRING)
         {
            OEMSound_ACM_Free (pSoundCBdata);
            return;
         }
      }
      pSoundCBdata++;
   }


   // fake the callback if it is not the owner of the current playing tone
   // or dual tone
   AEESound_StatusCB (pUser, AEE_SOUND_PLAY_DONE);

#else // !FEATURE_ACM || FEATURE_ODM_UI
   OEMSound_Proceed_StopTone ((snd_cb_func_ptr_type)  &OEMSound_StatusCB, pUser);
#endif // FEATURE_ACM && !FEATURE_ODM_UI
}

/*===========================================================================
FUNCTION OEMSound_GetLevels

DESCRIPTION
 This function returns the number of volume levels supported for the 
  device/class pair. It is called when user issues ISOUND_GetVolume() or
  ISOUND_SetVolume(). 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_GetLevels(AEESoundInfo * psi, void * pUser)
{   
   uisnd_get_levels( OEMSound_GetSndDevice(psi->eDevice),
                     OEMSound_GetSndMethod(psi->eMethod),   
                     &OEMSound_LevelCB, 
                     pUser);   
}

/*===========================================================================
FUNCTION OEMSound_SetVolume

DESCRIPTION
  This function issues a command to set the volume of sound device/class pair.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_SetVolume(AEESoundInfo * psi, uint16 wLevel, void * pUser)
{
   uisnd_set_volume(OEMSound_GetSndDevice(psi->eDevice),
                  OEMSound_GetSndMethod(psi->eMethod), 
                  (uint8)wLevel, 
                  &OEMSound_StatusCB, 
                  pUser);
}

/*===========================================================================
FUNCTION OEMSound_GetVolume

DESCRIPTION
  This function issues a command to get the volume level of device/class pair. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_GetVolume(AEESoundInfo * psi, void * pUser)
{
   uisnd_get_volume(OEMSound_GetSndDevice(psi->eDevice),
                  OEMSound_GetSndMethod(psi->eMethod),
                  &OEMSound_VolumeCB, 
                  pUser);
}

/*==================================================================
Function: OEMSound_Proceed_Vibrate

Description:
   It causes the handset to vibrate for the specified amount to time
   if the feature is supported; otherwise, it doesn't do anything.
                           
Prototype:
   void OEMSound_Proceed_Vibrate(uint16 wDuration, void * pUser)

Parameter(s):
   uint16 u16Duration: Duration of vibration in milliseconds

Return Value:
   None.

Comments:
   With ACM feature enabled, this function will only be called when
   the resouce is granted.

See Also:
   AEESound_StopVibrate
   OEMSound_Vibrate
==================================================================*/
static void OEMSound_Proceed_Vibrate(uint16 wDuration, void * pUser)
{
   DBGPRINTF("OEMSound_Proceed_Vibrate %d",wDuration);
   uisnd_vibrate(wDuration, NULL, pUser);
}

/*==================================================================
Function: OEMSound_Vibrate

Description:
   It causes the handset to vibrate for the specified amount to time
   if the feature is supported; otherwise, it doesn't do anything.
                           
Prototype:
   void OEMSound_Vibrate(uint16 u16Duration);

Parameter(s):
   uint16 u16Duration: Duration of vibration in milliseconds

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
void OEMSound_Vibrate(uint16 wDuration, void * pUser)
{
#if defined(FEATURE_ACM) && !defined(FEATURE_ODM_UI)
   sound_acmCBdata *pSoundCBdata = OEMSound_Get_AcmCBdata ();

   if (pSoundCBdata == NULL)
   {
      return;
   }

   pSoundCBdata->operationType = OPERATION_VIBRATE;
   pSoundCBdata->uniqueID      = pUser;

   if (OEMSound_ACM_Acquire (pSoundCBdata) != SUCCESS)
   {
       return;
   }
   else if (pSoundCBdata->acmState == ACM_RESOURCE_ACQUIRING)
   {
      pSoundCBdata->duration = wDuration;
      return;
   }

   ISHELL_SetTimer (pOEMSoundShell,
                    wDuration + MAX_CALLBACK_DELAY_MS,
                    OEMSound_Vibrate_ACM_TimerCallback,
                    pSoundCBdata);
#endif // FEATURE_ACM && !FEATURE_ODM_UI

   OEMSound_Proceed_Vibrate (wDuration, pUser);
}

/*==================================================================
Function: OEMSound_Proceed_StopVibrate

Description:
   It stops the current vibration.

Prototype:
   void OEMSound_Proceed_StopVibrate(void * pUser);

Parameter(s):
   None.

Return Value:
   None.

Comments:
   In ACM world, will only be called if the vibration is started
   by the same user.

See Also:
   AEESound_StopVibrate
   OEMSound_StopVibrate
==================================================================*/
static void OEMSound_Proceed_StopVibrate (void * pUser)
{
   uisnd_stop_vibrate (NULL, NULL);
}

/*==================================================================
Function: OEMSound_StopVibrate

Description:
   It stops the current vibration.
                           
Prototype:
   void OEMSound_StopVibrate();

Parameter(s):
   None.

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
void OEMSound_StopVibrate(void * pUser)
{
#if defined(FEATURE_ACM) && !defined(FEATURE_ODM_UI)
   sound_acmCBdata   *pSoundCBdata;
   int                i;

   // check whether this pUser is the one that currently
   // holds the vibrator resource
   pSoundCBdata = sound_acmCBarray;
   for (i = 0; i < MAX_SOUND_OPERATION_REQUESTS; i++)
   {
      if (pSoundCBdata->operationType == OPERATION_VIBRATE &&
          pSoundCBdata->uniqueID == pUser)
      {
         if (pSoundCBdata->acmState == ACM_RESOURCE_ACQUIRED)
         {
            ISHELL_CancelTimer (pOEMSoundShell,
                                OEMSound_Vibrate_ACM_TimerCallback,
                                pSoundCBdata);
            OEMSound_Proceed_StopVibrate (pUser);
         }
         OEMSound_ACM_Free (pSoundCBdata);
         break;
      }
      pSoundCBdata++;
   }
#else
   OEMSound_Proceed_StopVibrate (pUser);
#endif
}

/*==================================================================
Function: OEMSound_IsVibratorON

Description:
   Can be used by other OEM layer code to check if vibrator is ON.
This isn't exposed to AEE layer and isn't included in OEMSound.h.

Prototype:
   void OEMSound_IsVibratorON();

Return Value: 
   TRUE - Vibrator is ON.
   FALSE - Vibrator is OFF

==================================================================*/
boolean OEMSound_IsVibratorON(void)
{
   return uisnd_is_vibrator_on();   
}

#else  // FEATURE_UIXSND_IMPLEMENTATION

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/
#if defined(FEATURE_OEMSOUND_NATIVE_IF)
//
// Copy current sound info from native layer.
// Do not modify psi->eMthod and psi->eAPath
//
extern void uisnd_GetSoundInfo(AEESoundInfo * psi);

//
// Set (update) current sound info in native layer.
// psi->eMthod and psi->eAPath are of no consequence.
//
extern void uisnd_SetSoundInfo(AEESoundInfo * psi);
#endif // FEATURE_OEMSOUND_NATIVE_IF

/*===========================================================================
FUNCTION OEMSound_DeleteInstance

DESCRIPTION
  This function decreases ref count of underlying audio device and restores
  the default settings when ref count goes to zero.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_DeleteInstance(AEESoundInfo * psi)
{
   AEESoundInfo   si;

   if (gsRefs <= 0)
      return;

   if (--gsRefs > 0)
      return;

   //
   // Restore the state to that before the creation of first ISound instance.
   // Note: 
   // (1) Here only device setting is restored
   // (2) OEMs can restore the volume settings for various methods.
   OEMSound_Get(&si);
   if (si.eEarMuteCtl != gsDevice.eEarMuteCtl ||
       si.eMicMuteCtl != gsDevice.eMicMuteCtl )
   {
      snd_set_device( OEMSound_GetSndDevice(si.eDevice), 
                      (snd_mute_control_type)gsDevice.eEarMuteCtl, 
                      (snd_mute_control_type)gsDevice.eMicMuteCtl, 
                      NULL,
                      NULL);
   }
}

/*===========================================================================

FUNCTION OEMSound_Get

DESCRIPTION
  This function gets the sound output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void OEMSound_Get(AEESoundInfo * psi)
{
   if (!psi)
      return;

   //
   // NOTE to OEMs:
   // (1) Get the sound device info from your native layer.
   // (2) Do not modify psi->eMethod.
   //
#if defined(FEATURE_OEMSOUND_NATIVE_IF)
   uisnd_GetSoundInfo(psi);
#endif
}



/*===========================================================================

FUNCTION OEMSound_SetDevice

DESCRIPTION
  This function issues a command to set the sound output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void OEMSound_SetDevice(AEESoundInfo * psi, void * pUser)
{
   //
   // NOTE to OEMs:
   // (1) Update this sound device in your native layer.
   // (2) Ignore psi->eMethod.
   //
#if defined(FEATURE_OEMSOUND_NATIVE_IF)
   uisnd_SetSoundInfo(psi);
#endif

   snd_set_device( OEMSound_GetSndDevice(psi->eDevice), 
                   (snd_mute_control_type)psi->eEarMuteCtl, 
                   (snd_mute_control_type)psi->eMicMuteCtl, 
                   &OEMSound_StatusCB, 
                   pUser);
}



/*===========================================================================
FUNCTION OEMSound_PlayTone

DESCRIPTION
  This function issues a command to play a specified tone.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_PlayTone(AEESoundInfo * psi, AEESoundToneData* toneData, void * pUser)
{
   snd_tone_start( OEMSound_GetSndDevice(psi->eDevice),
                   OEMSound_GetSndMethod(psi->eMethod), 
                   OEMSound_GetSndTone(toneData->eTone), 
                   toneData->wDuration, 
                   (snd_apath_type)psi->eAPath, 
                   &OEMSound_StatusCB, 
                   pUser);
}

/*===========================================================================
FUNCTION OEMSound_PlayFreqTone

DESCRIPTION
  This function issues a command to play a specified pair of tone frequencies. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_PlayFreqTone(AEESoundInfo * psi, uint16 wHiFreq, uint16 wLoFreq, uint16 wDuration, void * pUser)
{
   snd_freq_tone_start( OEMSound_GetSndDevice(psi->eDevice),
                        OEMSound_GetSndMethod(psi->eMethod), 
                        wHiFreq, 
                        wLoFreq, 
                        wDuration, 
                        (snd_apath_type) psi->eAPath,
                        &OEMSound_StatusCB, 
                        pUser);
}


/*===========================================================================
FUNCTION OEMSound_StopTone

DESCRIPTION
  This function issues a command to stop playing a single tone or a playlist

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_StopTone(boolean bPlayList, void * pUser)
{
   // bPlayList is not used except for MSM5000
   snd_tone_stop(&OEMSound_StatusCB, pUser);
}


/*===========================================================================
FUNCTION OEMSound_GetLevels

DESCRIPTION
 This function returns the number of volume levels supported for the 
  device/class pair. It is called when user issues ISOUND_GetVolume() or
  ISOUND_SetVolume(). 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_GetLevels(AEESoundInfo * psi, void * pUser)
{
   snd_get_levels( OEMSound_GetSndDevice(psi->eDevice),
                   OEMSound_GetSndMethod(psi->eMethod),
                   &OEMSound_LevelCB, 
                   pUser);
}


/*===========================================================================
FUNCTION OEMSound_SetVolume

DESCRIPTION
  This function issues a command to set the volume of sound device/class pair.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_SetVolume(AEESoundInfo * psi, uint16 wLevel, void * pUser)
{
   snd_set_volume(OEMSound_GetSndDevice(psi->eDevice),
                  OEMSound_GetSndMethod(psi->eMethod), 
                  (uint8)wLevel, 
                  &OEMSound_StatusCB, 
                  pUser);
}


/*===========================================================================
FUNCTION OEMSound_GetVolume

DESCRIPTION
  This function issues a command to get the volume level of device/class pair. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_GetVolume(AEESoundInfo * psi, void * pUser)
{
   snd_get_volume(OEMSound_GetSndDevice(psi->eDevice),
                  OEMSound_GetSndMethod(psi->eMethod),
                  &OEMSound_VolumeCB, 
                  pUser);

}


/*==================================================================
Function: OEM_Vibrate

Description:
   It causes the handset to vibrate for the specified amount to time
   if the feature is supported; otherwise, it doesn't do anything.
                           
Prototype:
   void OEM_Vibrate(uint16 u16Duration);

Parameter(s):
   uint16 u16Duration: Duration of vibration in milliseconds

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
void OEMSound_Vibrate(uint16 wDuration, void * pUser)
{
   snd_tone_start( SND_DEVICE_CURRENT,
                   SND_METHOD_AUX,
                   (snd_tone_type)(SND_1),
                   (uint16)(1000),
                   (snd_apath_type)(SND_APATH_LOCAL),
                   NULL, // no callback and pClientData specified
#ifndef AEE_SIMULATOR
                    NULL 
#else
                    (void *)&wDuration
#endif
                  );

   gsVibrateON = TRUE;
}


/*==================================================================
Function: OEMSound_StopVibrate

Description:
   It stops the  current vibration.
                           
Prototype:
   void OEM_StopVibrate();

Parameter(s):
   None.

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
void OEMSound_StopVibrate(void * pUser)
{
   snd_tone_stop( NULL, NULL );

   gsVibrateON = FALSE;
}


/*==================================================================
Function: OEMSound_IsVibratorON

Description:
   Can be used by other OEM layer code to check if vibrator is ON.
This isn't exposed to AEE layer and isn't included in OEMSound.h.

Prototype:
   void OEMSound_IsVibratorON();

Return Value: 
   TRUE - Vibrator is ON.
   FALSE - Vibrator is OFF

==================================================================*/
boolean OEMSound_IsVibratorON(void)
{
   return gsVibrateON;
}

#endif // FEATURE_UIXSND_IMPLEMENTATION



/*===========================================================================
FUNCTION OEMSound_PlayToneList

DESCRIPTION
  This function issues a command to play a list of tones

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_PlayToneList(AEESoundInfo * psi, AEESoundToneData * pToneData, uint16 wDataLen, void * pUser)
{
   // No implementation required. Is here just to make linker happy.
}
//
// Internal callbacks. Call corresponding AEE ISound callbacks
//
#if defined(FEATURE_ACM) && !defined(FEATURE_ODM_UI)

/*==================================================================
Function: OEMSound_Tone_ACM_TimerCallback

Description:
   Timer to be registered with playing a tone or dual tone. The timer
   is set to fire at the time of tone duration pluse grace period as
   specified in MAX_CALLBACK_DELAY_MS.

   When another tone is being played not through OEMSound.c. In this
   case, snd layer does not deliver us the callback, so the timer
   will eventually expire.

   When the timer expries, we will fake an error event to the
   application, and this entry will be removed from the acquired
   ACM entries.

Parameter(s):
   The registered client data.

Return Value:
   None.
==================================================================*/
static void OEMSound_Tone_ACM_TimerCallback (void *pClientData)
{
   sound_acmCBdata *pAcmCBData = (sound_acmCBdata*) pClientData;

   // For the force released case, we have already delivered
   // the status indicating resource is busy. So, we should not
   // deliver any more callback.
   if (pAcmCBData->forceReleased != TRUE)
   {
      AEESound_StatusCB(pAcmCBData->uniqueID, AEE_SOUND_FAILURE);
   }

   OEMSound_ACM_Free (pAcmCBData);
}

/*==================================================================
Function: OEMSound_StatusCB_With_ACM

Description:
   The peer version of OEMSound_StatusCB with ACM feature turned on.
   It is used to receive the status callback from the snd layer when
   playing tone or dual tone, or stop playing of tone or dual tone.

Parameter(s):
   The registered client data and the status of the request.

Return Value:
   None.

==================================================================*/
static void OEMSound_StatusCB_With_ACM (const void * pClientData,
                                        snd_status_type status )
{
   int              aeeStatus;
   sound_acmCBdata *pSoundCBdata = NULL;
   int              i;

   OEMCriticalSection_Enter(&gSoundCriticalSection);

   // check whether this pUser is the one that currently
   // holds the tone resource
   pSoundCBdata = sound_acmCBarray;
   for (i = 0; i < MAX_SOUND_OPERATION_REQUESTS; i++)
   {
      if ((pSoundCBdata->operationType == OPERATION_PLAY_TONE ||
           pSoundCBdata->operationType == OPERATION_PLAY_FREQ_TONE) &&
          (pSoundCBdata->uniqueID == pClientData))
      {
         if (pSoundCBdata->acmState == ACM_RESOURCE_ACQUIRED)
         {
            break;
         }
      }
      pSoundCBdata++;
   }

   // The timer must have been fired if (i == MAX_SOUND_OPERATION_REQUESTS)
   if (i != MAX_SOUND_OPERATION_REQUESTS)
   {
      aeeStatus = OEMSound_GetAEESoundStatus(status);

      if (aeeStatus != AEE_SOUND_SUCCESS)
      {
         // cancelling the timer
         ISHELL_CancelTimer (pOEMSoundShell,
                             OEMSound_Tone_ACM_TimerCallback,
                             (void*) pSoundCBdata);

         OEMSound_ACM_Free ((sound_acmCBdata *) pSoundCBdata);
      }

      // For the force released case, we have already delivered
      // the status indicating resource is busy. So, we should not
      // deliver any more callback since the callback is for our
      // internal stopping the tone or dual tone, which may be
      // mis-interpreted by the application as the statua of tone
      // or dual tone playing
      if (pSoundCBdata->forceReleased != TRUE)
      {
         AEESound_StatusCB (pClientData, aeeStatus);
      }
   }

   OEMCriticalSection_Leave(&gSoundCriticalSection);
}

/*==================================================================
Function: OEMSound_Vibrate_ACM_TimerCallback

Description:
   For vibration, since we do not receive the callback that indicates
   that vibration should have been finished by now, we use
   a timer to keep track when the vibration should be done.

   The timer is set to fire at the time of the vibration duration
   plus grace period as specified in MAX_CALLBACK_DELAY_MS.

   When the timer expries, the ACM entry will be removed from the
   acquired ACM entries.

Parameter(s):
   The registered client data.

Return Value:
   None.

==================================================================*/
static void OEMSound_Vibrate_ACM_TimerCallback (void *pClientData)
{
   sound_acmCBdata *pAcmCBData = (sound_acmCBdata*) pClientData;

   OEMSound_ACM_Free (pAcmCBData);
}

/*==================================================================
Function: OEMSound_ACM_Free

Description:
   Releases the ACM resource for OEMSound module. This inclues:
   playing tone, dual tone and vibration.

Parameter(s):
   pSoundCBdata: used to hold the necessary ACM information

Return Value:
   None.
==================================================================*/
static void OEMSound_ACM_Free (sound_acmCBdata *pSoundCBdata)
{
   if (pSoundCBdata->acmState != ACM_RESOURCE_UNKNOWN)
   {
      OEMACM_PriorityMgr_Free (pSoundCBdata->uniqueID);
      pSoundCBdata->acmState = ACM_RESOURCE_UNKNOWN;
      pSoundCBdata->uniqueID = NULL;
   }
}

/*==================================================================
Function: OEMSound_Get_AcmCBdata

Description:
   Reserves an entry in the sound_acmCBarray to be used for ACM
   transaction.

Parameter(s):
   None.

Return Value:
   sound_acmCBdata*: used to store the necessary ACM information
==================================================================*/
static sound_acmCBdata *OEMSound_Get_AcmCBdata ()
{
   sound_acmCBdata *pSoundCBdata = NULL;
   int              i;

   // Find an empty slot in the array
   pSoundCBdata = sound_acmCBarray;
   for (i = 0; i < MAX_SOUND_OPERATION_REQUESTS; i++)
   {
      if (pSoundCBdata->acmState == ACM_RESOURCE_UNKNOWN)
      {
         return pSoundCBdata;
      }
      pSoundCBdata++;
   }

   return NULL;
}

/*==================================================================
Function: OEMSound_ACM_Acquire

Description:
   This function is called to do resource acquiring for the operations
   in oemsound module. This includes, playing a tone, dual tone or
   vibration.

Parameter(s):
   pSoundCBdata: which has the necessary ACM information

Return Value:
   EFAILED: resource not granted
   SUCCESS: resrouce is acquired or resource will be acquired.
            In this case, acmState inside of pSoundCBdata will be set
            to ACM_RESOURCE_ACQUIRING or ACM_RESOURCE_ACQUIRED
            accordingly.
==================================================================*/
static int OEMSound_ACM_Acquire  (sound_acmCBdata *pSoundCBdata)
{
   ACM_resource_state_type acmState;
   ACM_acquire_reason_code reason;
   PFNNOTIFY               acmCB;
   int                     nRet;

   if (pSoundCBdata->operationType == OPERATION_VIBRATE)
   {
      reason = ACM_PHONE_VIBRATE;
   }
   else
   {
      reason = ACM_SOUND_BEEP;
   }

   // First try acquiring from priority manager
   nRet = OEMACM_PriorityMgr_Acquire (pSoundCBdata->uniqueID,
                                      reason,
                                      &acmState);

   if (nRet != SUCCESS || acmState == ACM_RESOURCE_BUSY)
   {
      return EFAILED;
   }

   // Register for status changes
   pSoundCBdata->cbAcmData.pdata = pSoundCBdata;
   pSoundCBdata->cbAcmData.state = ACM_RESOURCE_UNKNOWN;

   if (pSoundCBdata->operationType == OPERATION_VIBRATE)
   {
      acmCB = OEMSound_Vibrate_ACM_StatusCB;
   }
   else
   {
      acmCB = OEMSound_Tone_ACM_StatusCB;
   }


   if(OEMACM_StateMgr_OnStatusChangeNotify(pSoundCBdata->uniqueID,
                                           acmCB,
                                           &pSoundCBdata->cbAcmData) != SUCCESS)
   {
      OEMACM_PriorityMgr_Free (pSoundCBdata->uniqueID);
      return EFAILED;
   }

   if(acmState == ACM_RESOURCE_ACQUIRED)
   {
      pSoundCBdata->acmState = ACM_RESOURCE_ACQUIRED;
   }
   else
   {
      pSoundCBdata->acmState = ACM_RESOURCE_ACQUIRING;
   }
   return SUCCESS;
}

/*==================================================================
Function: OEMSound_Tone_ACM_StatusCB

Description:
   Callback function registered with ACM service to be invoked
   for force release and delayed acquiring of playing tone and
   dual tone.

Parameter(s):
   ACM_Callback_data_type *: from which we can retrieve the
                             sound_acmCBdata
Return Value:
   None
==================================================================*/
static void OEMSound_Tone_ACM_StatusCB (void *p)
{
   ACM_Callback_data_type *cbdata       = (ACM_Callback_data_type*)p;
   sound_acmCBdata        *pSoundCBdata = (sound_acmCBdata*)cbdata->pdata;

   // delayed acquiring
   if(cbdata->state == ACM_RESOURCE_ACQUIRED)
   {
      AEESoundInfo *psi = &pSoundCBdata->soundInfo;

      pSoundCBdata->acmState = ACM_RESOURCE_ACQUIRED;
      ISHELL_SetTimer (pOEMSoundShell,
                       pSoundCBdata->duration + MAX_CALLBACK_DELAY_MS,
                       OEMSound_Tone_ACM_TimerCallback,
                       pSoundCBdata);

      if (pSoundCBdata->operationType == OPERATION_PLAY_TONE)
      {
         uisnd_play_tone_generic( OEMSound_GetSndDevice(psi->eDevice),
                                  OEMSound_GetSndMethod(psi->eMethod),
                                  OEMSound_GetSndTone(pSoundCBdata->tone),
                                  pSoundCBdata->duration,
                                  (snd_apath_type)psi->eAPath,
                                  &OEMSound_StatusCB_With_ACM,
                                  pSoundCBdata->uniqueID);
      }
      else
      {
         uisnd_freq_tone_start( OEMSound_GetSndDevice(psi->eDevice),
                                OEMSound_GetSndDevice(psi->eMethod),
                                pSoundCBdata->hiFreq,
                                pSoundCBdata->loFreq,
                                pSoundCBdata->duration,
                                (snd_apath_type)psi->eAPath,
                                &OEMSound_StatusCB_With_ACM,
                                pSoundCBdata->uniqueID);
      }
   }
   // force release
   else
   {
      if (pSoundCBdata->acmState == ACM_RESOURCE_ACQUIRED)
      {
         // delivers a callback to app indicating resource being force released
         AEESound_StatusCB (pSoundCBdata->uniqueID, AEE_SOUND_RESOURCE_BUSY);

         // sets up the forceReleased flag to indicate that
         pSoundCBdata->forceReleased = TRUE;

         // cancels the timer for playing the tone
         ISHELL_CancelTimer (pOEMSoundShell,
                             OEMSound_Tone_ACM_TimerCallback,
                             pSoundCBdata);

         // sets the guard timer for stopping the tone in case
         // callback for stopping the tone does not get deilivered
         ISHELL_SetTimer (pOEMSoundShell,
                          MAX_CALLBACK_DELAY_MS,
                          OEMSound_Tone_ACM_TimerCallback,
                          (void *) pSoundCBdata);

         OEMSound_Proceed_StopTone ((snd_cb_func_ptr_type)  &OEMSound_StatusCB_With_ACM,
                                    pSoundCBdata->uniqueID);
      }
   }
}

/*==================================================================
Function: OEMSound_Vibrate_ACM_StatusCB

Description:
   Callback function registered with ACM service to be invoked
   for force release and delayed acquiring of vibration.

Parameter(s):
   ACM_Callback_data_type *: from which we can retrieve the
                             sound_acmCBdata
Return Value:
   None
==================================================================*/
static void OEMSound_Vibrate_ACM_StatusCB (void *p)
{
   ACM_Callback_data_type *cbdata       = (ACM_Callback_data_type*)p;
   sound_acmCBdata        *pSoundCBdata = (sound_acmCBdata*)cbdata->pdata;

   if(cbdata->state == ACM_RESOURCE_ACQUIRED)
   {
      pSoundCBdata->acmState = ACM_RESOURCE_ACQUIRED;
      ISHELL_SetTimer (pOEMSoundShell,
                       pSoundCBdata->duration + MAX_CALLBACK_DELAY_MS,
                       OEMSound_Vibrate_ACM_TimerCallback,
                       pSoundCBdata);
      OEMSound_Proceed_Vibrate ( pSoundCBdata->duration,
                                 pSoundCBdata->uniqueID);
   }
   // force release
   else
   {
      if (pSoundCBdata->acmState == ACM_RESOURCE_ACQUIRED)
      {
         // cancel the timer
         ISHELL_CancelTimer (pOEMSoundShell,
                             OEMSound_Vibrate_ACM_TimerCallback,
                             pSoundCBdata);

         OEMSound_Proceed_StopVibrate (pSoundCBdata->uniqueID);
      }
      OEMSound_ACM_Free (pSoundCBdata);
   }
}
#endif // FEATURE_ACM && !FEATURE_ODM_UI

static void OEMSound_StatusCB( const void * pClientData, snd_status_type status )
{
   OEMCriticalSection_Enter(&gSoundCriticalSection);
   AEESound_StatusCB(pClientData, OEMSound_GetAEESoundStatus(status));
   OEMCriticalSection_Leave(&gSoundCriticalSection);
}

static void OEMSound_VolumeCB( const void * pClientData, snd_status_type status, uint16 wVolume )
{
   OEMCriticalSection_Enter(&gSoundCriticalSection);
   AEESound_VolumeCB(pClientData, OEMSound_GetAEESoundStatus(status), wVolume);
   OEMCriticalSection_Leave(&gSoundCriticalSection);
}

static void OEMSound_LevelCB( const void * pClientData, snd_status_type status, uint16 wLevel )
{
   OEMCriticalSection_Enter(&gSoundCriticalSection);
   AEESound_LevelCB(pClientData, OEMSound_GetAEESoundStatus(status), wLevel);
   OEMCriticalSection_Leave(&gSoundCriticalSection);
}

//
// AEE to/from SND enum conversion functions
//
static AEESoundStatus OEMSound_GetAEESoundStatus(snd_status_type e)
{ 
   int i;
   AEESoundStatus eResult = AEE_SOUND_UNKNOWN;
   for (i = 1; i < (int)AEE_SOUND_LAST && (uint16)e != gsSoundCaps.sndStatus[i]; i++)
      ;

   if (i < (int)AEE_SOUND_LAST)
      eResult = (AEESoundStatus) i;

   return eResult;
}

static snd_device_type OEMSound_GetSndDevice(AEESoundDevice e)
{
   snd_device_type dt = SND_DEVICE_HANDSET;
  
   if (e < AEE_SOUND_DEVICE_LAST && e >= 0)
   {
   		dt = (snd_device_type) gsSoundCaps.sndDevice[(int)e];
   }
  
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
   if (e == AEE_SOUND_DEVICE_BT_STEREO_HEADSET)
   {
#ifdef FEATURE_UIXSND_IMPLEMENTATION
      if (uisnd_is_device_attached(SND_DEVICE_BT_A2DP_SCO_HEADSET))
      {
      	dt = SND_DEVICE_BT_A2DP_SCO_HEADSET;
      }
      else if (uisnd_is_device_attached(SND_DEVICE_BT_A2DP_HEADSET))
      {
      	dt = SND_DEVICE_BT_A2DP_HEADSET;
      }
#endif
   }	 
#endif

   if ((dt != SND_DEVICE_CURRENT) && (dt >= SND_DEVICE_MAX))
   {
   	  dt = SND_DEVICE_HANDSET;
   }
	
   return dt;
}

static snd_method_type OEMSound_GetSndMethod(AEESoundMethod e)
{
	snd_method_type mt = SND_METHOD_KEY_BEEP;
   if (e < AEE_SOUND_METHOD_LAST && e >= 0)
      mt = (snd_method_type) gsSoundCaps.sndMethod[(int)e];

   if(mt >= SND_METHOD_MAX)
	   mt = SND_METHOD_KEY_BEEP;

   return mt;
}

#ifdef FEATURE_UIXSND_IMPLEMENTATION
/*
 *  Maps the snd_device_type to AEESoundDevice
 */
static AEESoundDevice OEMSound_GetAEESndDevice(snd_device_type  e)
{
   if (e == (uint16) SND_DEVICE_MAX)
      return AEE_SOUND_DEVICE_UNKNOWN;
   else if (e == (uint16) SND_DEVICE_HANDSET)
      return AEE_SOUND_DEVICE_HANDSET;
   else if (e == (uint16) SND_DEVICE_HFK)
      return AEE_SOUND_DEVICE_HFK;  
   else if (e == (uint16) SND_DEVICE_HEADSET)
      return AEE_SOUND_DEVICE_HEADSET;  
#ifdef FEATURE_ANALOG_HFK
   else if (e == (uint16) SND_DEVICE_AHFK)
      return AEE_SOUND_DEVICE_AHFK;  
#endif
#ifdef FEATURE_STEREO_DAC
   else if (e == (uint16) SND_DEVICE_SDAC)
      return AEE_SOUND_DEVICE_SDAC;  
#endif
#ifdef FEATURE_TTY
   else if (e == (uint16) SND_DEVICE_TTY_HFK)
      return AEE_SOUND_DEVICE_TTY_HFK;  
   else if (e == (uint16) SND_DEVICE_TTY_HEADSET)
      return AEE_SOUND_DEVICE_TTY_HEADSET;  
#endif
   else if (e == (uint16) SND_DEVICE_CURRENT)
      return AEE_SOUND_DEVICE_CURRENT; 
#ifdef FEATURE_SPEAKER_PHONE
   else if (e == (uint16) SND_DEVICE_SPEAKER_PHONE)
      return AEE_SOUND_DEVICE_SPEAKER;  
#endif
#ifdef FEATURE_BT_AG
   else if (e == (uint16) SND_DEVICE_BT_HEADSET)
      return AEE_SOUND_DEVICE_BT_HEADSET;  
#endif
   else if (e == (uint16) SND_DEVICE_STEREO_HEADSET)
      return AEE_SOUND_DEVICE_STEREO_HEADSET;  
#ifdef FEATURE_USB_ISOC_AUDIO
   else if (e == (uint16) SND_DEVICE_USB)
      return AEE_SOUND_DEVICE_USB;  
#endif
#ifdef FEATURE_TTY
   else if (e == (uint16) SND_DEVICE_TTY_HCO)
      return AEE_SOUND_DEVICE_TTY_HCO;
   else if (e == (uint16) SND_DEVICE_TTY_VCO)
      return AEE_SOUND_DEVICE_TTY_VCO;
#endif
#ifdef FEATURE_TTY_FULL
   else if (e == (uint16) SND_DEVICE_TTY_FULL)
      return AEE_SOUND_DEVICE_TTY_FULL;
#endif
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
   else if ( (e == SND_DEVICE_BT_A2DP_HEADSET) || (e == SND_DEVICE_BT_A2DP_SCO_HEADSET) )
      return AEE_SOUND_DEVICE_BT_STEREO_HEADSET;
#endif
#ifdef FEATURE_FM_OEM
   else if (e == (uint16) SND_DEVICE_HEADSET_FM)
      return AEE_SOUND_DEVICE_HEADSET_FM;
   else if (e == (uint16) SND_DEVICE_SPEAKER_FM)
      return AEE_SOUND_DEVICE_SPEAKER_FM;
#endif
   else
      return AEE_SOUND_DEVICE_UNKNOWN;
}
#endif // FEATURE_UIXSND_IMPLEMENTATION

static snd_tone_type OEMSound_GetSndTone(AEESoundTone e)
{
   if (e == AEE_TONE_SILENCE)
      return (snd_tone_type)(gsSoundCaps.sndControlBase);
   else if (e == AEE_TONE_PTT_ALERT_ARRIVAL)
      return (snd_tone_type)((uint16)AEE_TONE_RING_D5 + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_PTT_CALL_ARRIVAL)
      return (snd_tone_type)((uint16)AEE_TONE_RING_D5 + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_PTT_FAIL)
      return (snd_tone_type)((uint16)AEE_TONE_RING_D + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_PTT_FLOOR_DENY)
      return (snd_tone_type)((uint16)AEE_TONE_RING_A5 + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_PTT_FLOOR_GRANT)
      return (snd_tone_type)((uint16)AEE_TONE_RING_G5 + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_PTT_FLOOR_INTERRUPTION)
      return (snd_tone_type)((uint16)AEE_TONE_RING_A5 + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_PTT_ADD_MEMBER)
      return (snd_tone_type)((uint16)AEE_TONE_RING_C4 + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_PTT_SUCCESS)
      return (snd_tone_type)((uint16)AEE_TONE_RING_G5 + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_PTT_TALK_ANNOUNCE)
      return (snd_tone_type)((uint16)AEE_TONE_RING_E5 + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_PTT_FLOOR_AVAILABLE)
      return (snd_tone_type)((uint16)AEE_TONE_RING_E5 + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_RESERVED_1)
      return (snd_tone_type)((uint16)AEE_TONE_RING_A + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_RESERVED_2)
      return (snd_tone_type)((uint16)AEE_TONE_WARN + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_RESERVED_3)
      return (snd_tone_type)((uint16)AEE_TONE_WARN + gsSoundCaps.sndToneBase);
   else if (e == AEE_TONE_RESERVED_4)
      return (snd_tone_type)((uint16)AEE_TONE_ERR + gsSoundCaps.sndToneBase);
   else
   {
#define OEMSOUND_MAX_TONES    127
      // If this is a feedback tone, then by default, map it to existing tone in tone database.
      if (e >= AEE_TONE_FEEDBACK_0 && e <= OEMSOUND_MAX_TONES)
      {
         e = AEE_TONE_0 + (e - AEE_TONE_FEEDBACK_0);
      }
      return (snd_tone_type)((uint16)e + gsSoundCaps.sndToneBase);
   }
}

#ifdef CUST_EDITION	 
/*===========================================================================

FUNCTION OEMSOUND_Sound_Id_Start

DESCRIPTION
  Start playing a sound from the database using a particular device and method

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void OEMSOUND_Sound_Id_Start
(
  snd_device_type       device,
   /* The device chosen for this command    */
  snd_method_type       method,
    /* method                               */
  snd_sound_id_type     sound_id,
    /* Sound id                             */
  snd_priority_type     priority,
    /* Priority of the tone                 */
  snd_apath_type        dtmf_path,
    /* Play this tone OTA when in DFM call  */
  snd_cb_func_ptr_type  callback_ptr
    /* Call back function                   */
)
{
   snd_sound_id_start( device,
                       method,
                       sound_id,
                       priority,
                       dtmf_path,
                       callback_ptr,
                       NULL          );
} /* end of OEM_Snd_Sound_Id_Start */

/*===========================================================================
FUNCTION OEMSOUND_Sound_Stop

DESCRIPTION
  Stop playing the current sound

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command Packet queued to Sound Task.

===========================================================================*/
void OEMSOUND_Sound_Stop(snd_priority_type priority)
{
   snd_sound_stop(priority, NULL, NULL);
} /* end of OEM_Snd_Sound_Stop */
#endif /*CUST_EDITION*/

