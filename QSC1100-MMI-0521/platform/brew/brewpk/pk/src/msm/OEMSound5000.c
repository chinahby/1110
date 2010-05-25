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

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/
#ifdef AEE_SIMULATOR
#define TG  T_G
#define TARG_H "target.h"
#endif


/*-------------------------------------------------------------------
            Include Files
-------------------------------------------------------------------*/
#define COMDEF_H
#include "OEMSound.h"
#include "snd.h"
#include "rex.h"

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/
typedef struct
{
   uint16   sndDevice[(int)AEE_SOUND_DEVICE_LAST]; // snd_device_type
   uint16   sndMethod[(int)AEE_SOUND_METHOD_LAST]; // snd_method_type
   uint16   sndToneBase; // == SND_FIRST_TDB_TONE
   uint16   sndControlBase; // == SND_FIRST_CONTROL_TONE
} AEESoundCaps;

// This constant defines an arbitrary maximum sound level for all devices. 
// OEMs need to adjust this and/or define more constants that 
// correspond to the actual volume levels supported for each device type.
#define OEMSOUND_MAX_VOLUME_LEVELS     100

// This constant defines the maximum number of tones allowed in a single
// playlist. The tone list is statically allocated in OEMSound_PlayToneList()
// function. 
// OEMs can change this based on their requirements.
#define OEMSOUND_MAX_PLAYLIST_TONES    32

// These constants define the REX signals that arrive from sound task to
// BREW. It is called by OEM in the context of the thread in which BREW is
// running. 
// OEMs may need to change the values to avoid clashes in their environment
#define OEMSOUND_SIG_PLAY_DONE   0x0001

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/
// AEE to snd enum conversions
static snd_apath_type GetSndDevice(AEESoundDevice e);
static snd_class_type GetSndMethod(AEESoundMethod e);
static snd_tone_type GetSndTone(AEESoundTone e);

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/
static void *        gpUser = NULL;
static AEESoundCaps  gsSoundCaps;

// This is an arbitrary initial value. OEMs must set proper initial value 
// for the device/method pair that is selected at the start
static uint16        gwVolLevel = 100;

/*===========================================================================
FUNCTION OEMSound_Init

DESCRIPTION
  This function issues a command to initialize OEMSound

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_Init(void)
{
   // Mapping: snd_device_type to AEESoundDevice
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_UNKNOWN]    = (uint16) SND_APATH_NONE;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_HANDSET]    = (uint16) SND_APATH_PHONE;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_HFK]        = (uint16) SND_APATH_EXT;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_HEADSET]    = (uint16) SND_APATH_EXT;
#ifdef FEATURE_ANALOG_HFK
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_AHFK]       = (uint16) SND_APATH_EXT;
#else
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_AHFK]       = (uint16) SND_APATH_NONE;
#endif
#ifdef FEATURE_STEREO_DAC
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_SDAC]       = (uint16) SND_APATH_EXT;
#else
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_SDAC]       = (uint16) SND_APATH_NONE;
#endif
#ifdef FEATURE_TTY
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_HFK]    = (uint16) SND_APATH_EXT;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_HEADSET]= (uint16) SND_APATH_EXT;
#else
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_HFK]    = (uint16) SND_APATH_NONE;
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_TTY_HEADSET]= (uint16) SND_APATH_NONE;
#endif
   gsSoundCaps.sndDevice[(int)AEE_SOUND_DEVICE_CURRENT]    = (uint16) SND_APATH_NONE;

   // Mapping: snd_method_type to AEESoundMethod
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_UNKNOWN]    = (uint16) SND_CLASS_MAX;
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_VOICE]      = (uint16) SND_CLASS_VOICE;
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_BEEP]       = (uint16) SND_CLASS_BEEP;
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_MESSAGE]    = (uint16) SND_CLASS_DEFAULT;
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_RING]       = (uint16) SND_CLASS_RING;

   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_CLICK]      = (uint16) SND_CLASS_MAX;
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_MIDI]       = (uint16) SND_CLASS_MAX;
   gsSoundCaps.sndMethod[(int)AEE_SOUND_METHOD_AUX]        = (uint16) SND_CLASS_MAX;

   gsSoundCaps.sndToneBase = SND_FIRST_TONE + 1; // since it is set to -1
   gsSoundCaps.sndControlBase = SND_FIRST_CONTROL_TONE;
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
   snd_packets_type * psp; // Pointer to sound packet

   gpUser = pUser;

   // Get a sound packet, fill-in the command contents and pass it to sound task
   psp= snd_get_packet();
   if ( psp )
   {
      psp->hdr.cmd = SND_SET_PATH;
      psp->path.apath = GetSndDevice(psi->eDevice);
      psp->path.out_mute_s = (snd_mute_control_type)psi->eEarMuteCtl;
      psp->path.in_mute_s  = (snd_mute_control_type)psi->eMicMuteCtl;

      // Disable the callback
      psp->hdr.task_ptr = NULL;
      psp->hdr.sigs = 0;

      snd_cmd( psp ); // Send command to sound task

      // Note: This callback is sent before the sound task completes this command
      AEESound_StatusCB(gpUser, AEE_SOUND_SUCCESS);
   }
   else
   {
      // No sound packet available
      AEESound_StatusCB(gpUser, AEE_SOUND_FAILURE);
   }
}

/*===========================================================================
FUNCTION OEMSound_PlayTone

DESCRIPTION
  This function issues a command to play a specified tone. Note that the 
  global volume level corresponding to the device/class is set here. The
  volume level is saved during OEMSound_SetVolume().

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_PlayTone(AEESoundInfo * psi, AEESoundToneData toneData, void * pUser)
{
   snd_packets_type * psp; // Pointer to sound packet

   gpUser = pUser;

   // Get a sound packet, fill-in the command contents and pass it to sound task
   psp= snd_get_packet();
   if ( psp )
   {
      psp->hdr.cmd = SND_TONE_START;
      psp->tone.apath = GetSndDevice(psi->eDevice);
      psp->tone.sclass = GetSndMethod(psi->eMethod);
      psp->tone.dtmf_path = (snd_dtmf_apath_type)psi->eAPath;

      psp->tone.vol.phone = (byte) gwVolLevel;
      psp->tone.vol.ext = (byte)  gwVolLevel;
      psp->tone.vol.priv = (byte)  gwVolLevel;

      psp->tone.tone.tone = GetSndTone(toneData.eTone);
      psp->tone.tone.duration_ms = toneData.wDuration;

      // Set the callback via signals
      psp->hdr.task_ptr = rex_self();
      psp->hdr.sigs = OEMSOUND_SIG_PLAY_DONE;
      (void) rex_clr_sigs( rex_self(), OEMSOUND_SIG_PLAY_DONE);

      snd_cmd( psp ); // Send command to sound task

      // Note: This callback is sent before the sound task completes this command
      AEESound_StatusCB(gpUser, AEE_SOUND_SUCCESS);
   }
   else
   {
      // No sound packet available
      AEESound_StatusCB(gpUser, AEE_SOUND_FAILURE);
   }
}

/*===========================================================================
FUNCTION OEMSound_PlayFreqTone

DESCRIPTION
  This function issues a command to play a specified pair of tone frequencies. 
  Note that the global volume level corresponding to the device/class is set here. 
  The volume level is saved during OEMSound_SetVolume().

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_PlayFreqTone(AEESoundInfo * psi, uint16 wHiFreq, uint16 wLoFreq, uint16 wDuration, void * pUser)
{
#ifdef FEATURE_PARAMETRIC_ALERTS
   snd_packets_type * psp; // Pointer to sound packet

   gpUser = pUser;

   // Get a sound packet, fill-in the command contents and pass it to sound task
   psp= snd_get_packet();
   if ( psp )
   {
      psp->hdr.cmd = SND_TONE_START;
      psp->tone.apath = GetSndDevice(psi->eDevice);
      psp->tone.sclass = GetSndMethod(psi->eMethod);
      psp->tone.dtmf_path = (snd_dtmf_apath_type)psi->eAPath;

      psp->tone.vol.phone = (byte) gwVolLevel;
      psp->tone.vol.ext = (byte)  gwVolLevel;
      psp->tone.vol.priv = (byte)  gwVolLevel;

      psp->tone.tone.tone = SND_FREQ;
      psp->tone.tone.duration_ms = toneData.wDuration;
      psp->tone.tone.freq.freq_hi = wHiFreq;
      psp->tone.tone.freq.freq_lo = wLoFreq;

      // Set the callback via signals
      psp->hdr.task_ptr = rex_self();
      psp->hdr.sigs = OEMSOUND_SIG_PLAY_DONE;
      (void) rex_clr_sigs( rex_self(), OEMSOUND_SIG_PLAY_DONE);

      snd_cmd( psp ); // Send command to sound task

      // Note: This callback is sent before the sound task completes this command
      AEESound_StatusCB(gpUser, AEE_SOUND_SUCCESS);
   }
   else
   {
      // No sound packet available
      AEESound_StatusCB(gpUser, AEE_SOUND_FAILURE);
   }
#else
   gpUser = pUser;

   // Feature not available
   AEESound_StatusCB(gpUser, AEE_SOUND_FAILURE);
#endif // FEATURE_PARAMETRIC_ALERTS
}

/*===========================================================================
FUNCTION OEMSound_PlayToneList

DESCRIPTION
  This function issues a command to play a list of tones. Note that the 
  global volume level corresponding to the device/class is set here. The
  volume level is saved during OEMSound_SetVolume().

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_PlayToneList(AEESoundInfo * psi, AEESoundToneData * pToneData, uint16 wDataLen, void * pUser)
{
   snd_packets_type * psp; // Pointer to sound packet
   static snd_type ToneArray[OEMSOUND_MAX_PLAYLIST_TONES];
   int i;

   gpUser = pUser;

   if (wDataLen >= OEMSOUND_MAX_PLAYLIST_TONES)
   {
      AEESound_StatusCB(gpUser, AEE_SOUND_FAILURE);
      return;
   }

   for (i = 0; i < wDataLen; i++)
   {
      ToneArray[i].tone = GetSndTone(pToneData[i].eTone);
      ToneArray[i].duration_ms = pToneData[i].wDuration;
   }

   ToneArray[i].tone = SND_STOP;
   ToneArray[i].duration_ms = 0;

   // Get a sound packet, fill-in the command contents and pass it to sound task
   psp= snd_get_packet();
   if ( psp )
   {
      psp->hdr.cmd = SND_MULTI_START;

      psp->multi.apath     = GetSndDevice(psi->eDevice);
      psp->multi.sclass    = GetSndMethod(psi->eMethod);
      psp->multi.dtmf_path = (snd_dtmf_apath_type) psi->eAPath;

      psp->multi.multi_ptr = (snd_type *)ToneArray;

      psp->multi.vol.phone  = (byte) gwVolLevel;
      psp->multi.vol.ext    = (byte) gwVolLevel;
      psp->multi.vol.priv   = (byte) gwVolLevel;

      // Set the callback via signals
      psp->hdr.task_ptr = rex_self();
      psp->hdr.sigs = OEMSOUND_SIG_PLAY_DONE;
      psp->multi.rpt_sigs  = 0;
      (void) rex_clr_sigs( rex_self(), OEMSOUND_SIG_PLAY_DONE);

      snd_cmd( psp ); // Send command to sound task

      // Note: This callback is sent before the sound task completes this command
      AEESound_StatusCB(gpUser, AEE_SOUND_SUCCESS);
   }
   else
   {
      // No sound packet available
      AEESound_StatusCB(gpUser, AEE_SOUND_FAILURE);
   }
}

/*===========================================================================
FUNCTION OEMSound_StopTone

DESCRIPTION
  This function issues a command to stop a single tone or playlist

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_StopTone(boolean bPlayList, void * pUser)
{
   snd_packets_type * psp; // Pointer to sound packet

   gpUser = pUser;

   // Get a sound packet, fill-in the command contents and pass it to sound task
   psp= snd_get_packet();
   if ( psp )
   {
      psp->hdr.cmd = bPlayList ? SND_MULTI_STOP : SND_TONE_STOP;

      // Disable the callback
      psp->hdr.task_ptr = NULL;
      psp->hdr.sigs = 0;

      snd_cmd( psp ); // Send command to sound task

      // Note: This callback is sent before the sound task completes this command
      AEESound_StatusCB(gpUser, AEE_SOUND_SUCCESS);
   }
   else
   {
      // No sound packet available
      AEESound_StatusCB(gpUser, AEE_SOUND_FAILURE);
   }
}

/*===========================================================================
FUNCTION OEMSound_GetLevels

DESCRIPTION
  This function returns the number of volume levels supported for the 
  device/class pair. It is called when user issues ISOUND_GetVolume() or
  ISOUND_SetVolume(). OEMs must return the correct volume level for the
  device/class pair. This level is used by AEE to compute & scale the volume
  Note that GET_LEVELS command is not supported in MSM5000. As a result, the
  local value is returned.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_GetLevels(AEESoundInfo * psi, void * pUser)
{
   gpUser = pUser;

   // For Device/Class: GetSndDevice(psi->eDevice)/GetSndMethod(psi->eMethod)
   AEESound_LevelCB(gpUser, AEE_SOUND_SUCCESS, OEMSOUND_MAX_VOLUME_LEVELS);
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
   snd_packets_type * psp; // Pointer to sound packet

   gpUser = pUser;

   // Get a sound packet, fill-in the command contents and pass it to sound task
   psp= snd_get_packet();
   if ( psp )
   {
      psp->hdr.cmd = SND_VOLUME;

      // Save the volume level
      gwVolLevel = wLevel;

      psp->vol.volume.phone = (byte) gwVolLevel;
      psp->vol.volume.ext = (byte) gwVolLevel;
      psp->vol.volume.priv = (byte) gwVolLevel;

      // Disable the callback
      psp->hdr.task_ptr = NULL;
      psp->hdr.sigs = 0;

      snd_cmd( psp ); // Send command to sound task

      // Note: This callback is sent before the sound task completes this command
      AEESound_StatusCB(gpUser, AEE_SOUND_SUCCESS);
   }
   else
   {
      // No sound packet available
      AEESound_StatusCB(gpUser, AEE_SOUND_FAILURE);
   }
}

/*===========================================================================
FUNCTION OEMSound_GetVolume

DESCRIPTION
  This function returns the volume level of device/class pair. 
  Note that GET_VOLUME command is not supported in MSM5000. As a result, the
  local value is returned.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_GetVolume(AEESoundInfo * psi, void * pUser)
{
   gpUser = pUser;

   // For Device/Class: GetSndDevice(psi->eDevice)/GetSndMethod(psi->eMethod)
   AEESound_VolumeCB(gpUser, AEE_SOUND_SUCCESS, gwVolLevel);

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
// MSM5000 does not have AUX method. As a result, vibration may not be possible
// unless OEM can implement it thru other means
#if 0
   snd_packets_type * psp; // Pointer to sound packet
   static snd_type VibBuf[2];

   gpUser = pUser;

   VibBuf[0].tone = SND_1;
   VibBuf[0].duration_ms = 1000;

   VibBuf[0].tone = SND_STOP;
   VibBuf[0].duration_ms = 0;

   // Get a sound packet, fill-in the command contents and pass it to sound task
   psp= snd_get_packet();
   if ( psp )
   {
      psp->hdr.cmd = SND_MULTI_START;

      psp->multi.apath     = SND_APATH_EXT;
      psp->multi.sclass    = SND_CLASS_DEFAULT;
      psp->multi.dtmf_path = SND_DTMF_APATH_MUTE;

      psp->multi.multi_ptr = VibBuf;

      psp->multi.vol.phone  = (byte) gwVolLevel;
      psp->multi.vol.ext    = (byte) gwVolLevel;
      psp->multi.vol.priv   = (byte) gwVolLevel;

      // Disable the callback
      psp->hdr.task_ptr = NULL;
      psp->hdr.sigs = 0;
      psp->multi.rpt_sigs  = 0;

      snd_cmd( psp ); // Send command to sound task

      // No callback
   }
   else
   {
      // No sound packet available
      // No callback
   }
#endif // if 0
}

/*==================================================================
Function: OEMSound_StopVibrate

Description:
   It stops the current vibration.
                           
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
// MSM5000 does not have AUX method. As a result, vibration may not be possible
// unless OEM can implement it thru other means
#if 0
   snd_packets_type * psp; // Pointer to sound packet

   gpUser = pUser;

   // Get a sound packet, fill-in the command contents and pass it to sound task
   psp= snd_get_packet();
   if ( psp )
   {
      psp->hdr.cmd = SND_MULTI_STOP;

      // Disable the callback
      psp->hdr.task_ptr = NULL;
      psp->hdr.sigs = 0;

      snd_cmd( psp ); // Send command to sound task

      // No callback
   }
   else
   {
      // No sound packet available
      // No callback
   }
#endif // if 0
}

/*===========================================================================
FUNCTION OEMSound_SigHandler

DESCRIPTION
   This method needs to be called by OEM task signal handler when it
   receives a REX signal that corresponds to the OEMSound commands.
   In this hadler, based on the signal type, call corresponding AEE ISound callbacks

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void OEMSound_SigHandler( uint32 dwSig )
{
   if ( dwSig & OEMSOUND_SIG_PLAY_DONE )
   {
      AEESound_StatusCB(gpUser, AEE_SOUND_PLAY_DONE);
   }
}

//
// AEE to SND enum conversion functions
//
static snd_apath_type GetSndDevice(AEESoundDevice e)
{
   if (e < AEE_SOUND_DEVICE_LAST)
      return (snd_apath_type) gsSoundCaps.sndDevice[(int)e];
   else
      return (snd_apath_type) gsSoundCaps.sndDevice[0];
}

static snd_class_type GetSndMethod(AEESoundMethod e)
{
   if (e < AEE_SOUND_METHOD_LAST)
      return (snd_class_type) gsSoundCaps.sndMethod[(int)e];
   else
      return (snd_class_type) gsSoundCaps.sndMethod[0];
}

static snd_tone_type GetSndTone(AEESoundTone e)
{
   if (e == AEE_TONE_SILENCE)
      return (snd_tone_type)(gsSoundCaps.sndControlBase);
   else
   {
      // Offset by 4 (These are extra tones for A, B, C, D in MSM3300, MSM5100 and above)
      if (e > AEE_TONE_9)
         e = (AEESoundTone)((uint16)e - 4);
      return (snd_tone_type)((uint16)e + gsSoundCaps.sndToneBase);
   }
}

