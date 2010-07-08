/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMSound.h

SERVICES:  OEM Basic Sound interface

GENERAL DESCRIPTION:
        Definitions, typedefs, etc. for OEM Sound

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef OEM_SOUND_H
#define OEM_SOUND_H


#include "AEESound.h"
#ifdef CUST_EDITION
#include "snd.h"
#endif /*CUST_EDITION*/
#if defined(__cplusplus)
extern "C" {
#endif

/*---------------------------------------------------------------------------
    OEMCMX Public Function Prototypes
---------------------------------------------------------------------------*/
extern void OEMSound_Init(void);
extern int  OEMSound_NewInstance(AEESoundInfo * psi);
extern void OEMSound_DeleteInstance(AEESoundInfo * psi);
extern void OEMSound_SetDevice(AEESoundInfo * psi, void * pUser);
extern void OEMSound_Get(AEESoundInfo * psi);
extern void OEMSound_PlayTone(AEESoundInfo * psi, AEESoundToneData * toneData, void * pUser);
extern void OEMSound_PlayFreqTone(AEESoundInfo * psi, uint16 wHiFreq, uint16 wLoFreq, uint16 wDuration, void * pUser);
extern void OEMSound_PlayToneList(AEESoundInfo * psi, AEESoundToneData * pToneData, uint16 wDataLen, void * pUser);
extern void OEMSound_StopTone(boolean bPlayList, void * pUser);
extern void OEMSound_GetLevels(AEESoundInfo * psi, void * pUser);
extern void OEMSound_SetVolume(AEESoundInfo * psi, uint16 wLevel, void * pUser);
extern void OEMSound_GetVolume(AEESoundInfo * psi, void * pUser);
extern void OEMSound_Vibrate(uint16 wDuration, void * pUser);
extern void OEMSound_StopVibrate(void * pUser);
#ifdef CUST_EDITION	
extern void OEMSOUND_Sound_Id_Start (snd_device_type       device,
                                    snd_method_type       method,
                                    snd_sound_id_type     sound_id,
                                    snd_priority_type     priority,
                                    snd_apath_type        dtmf_path,
                                    snd_cb_func_ptr_type  callback_ptr );
extern void OEMSOUND_Sound_Stop(snd_priority_type priority);
#endif /*CUST_EDITION*/
#if defined(__cplusplus)
}

/*========================================================================
  INTERFACES DOCUMENTATION
=======================================================================
Interface Name:  OEMSound

Notes:
(1) OEMSound provides the ability to restore the device audio setting (like
    current active device, volume, etc.) that was on the device prior to
    creation of the first instance of ISound object.
    OEMs can choose to use this feature to resore sound settings after
    an app using ISound exits.

(2) Native Ring/Vibrate mode should be honored when an application is using
    ISound interface.

=======================================================================

Function: OEMSound_Init()

Description:
   Maps device sound layer enumerations to AEE_SOUND enumerations.

Prototype:
   void OEMSound_Init( void );

Parameters:
   None

Return Value:
   None

Comments:
   This function is called only once during BREW initialization.

Side Effects:
   None.

See Also:
   None

========================================================================
Function: OEMSound_NewInstance()

Description:
  This function increases ref count of underlying audio device. It returns
  current AEESoundInfo.

Prototype:
   int OEMSound_NewInstance(AEESoundInfo * psi);

Parameters:
   psi [out]: Current sound info

Return Value:
   SUCCESS if successful.
   Error code otherwise.

Comments:
   This function is called every time an instance of ISound is created.

Side Effects:
   None

See Also:
   OEMSound_DeleteInstance()

========================================================================

Function: OEMSound_DeleteInstance()

Description:
  This function decreases ref count of underlying audio device and gives
  an opprtunity to restore the default settings when ref count goes to zero.

Prototype:
   int OEMSound_NewInstance(AEESoundInfo * psi);

Parameters:
   psi [in]: Sound info of the ISound object

Return Value:
   SUCCESS if successful.
   Error code otherwise.

Comments:
   This function is called every time an instance of ISound is deleted.

Side Effects:
   None

See Also:
   OEMSound_NewInstance()

========================================================================

Function: OEMSound_Get()

Description:
  This function gets the current device, ear and mic mute control settings
  from the device native layer.

Prototype:
   void OEMSound_Get(AEESoundInfo * psi) SECTION_OEMSOUND;

Parameters:
   psi [in]: Sound info of the ISound object

Return Value:
   None

Comments:
   OEMs should remember to update these settings in the device native
   layer when ISound changes them. For example, OEMSound_SetDevice() changes
   the settings and the device native layer must be updated.

   Also, in AEESoundInfo structure, eMethod and eAPath should not be updated
   by this function call as they are locally maintained by ISound object.

Side Effects:
   None

See Also:
   OEMSound_SetDevice()

========================================================================

Function: OEMSound_SetDevice()

Description:
   This function issues a command to set the sound output device.

Prototype:
   void OEMSound_SetDevice(AEESoundInfo * psi, void * pUser);

Parameters:
   psi: new sound device information (See BREW SDK API Reference Guide for
   AEESoundInfo definition).
   pUser: user data that will be passed back to the caller through the
          status callback function.

Return Value:
   None.

Comments:
   None

Side Effects:
   AEESound_StatusCB is called with status.

See Also:
   None

===========================================================================

Function: OEMSound_PlayTone()

Description:
   This function issues a command to play a specified tone.

Prototype:
   void OEMSound_PlayTone(AEESoundInfo * psi, AEESoundToneData toneData, void * pUser);

Parameters:
   psi: sound device information (See BREW SDK API Reference Guide for
   AEESoundInfo definition).
   toneData: tone and duration to be played
   pUser: user data that will be passed back to the caller through the
          status callback function.

Return Value:
   None

Comments:
   None

Side Effects:
   AEESound_StatusCB is called with status.

See Also:
   None

==========================================================================

Function: OEMSound_PlayFreqTone()

Description:
   This function issues a command to play a specified pair of tone frequencies.

Prototype:
   void OEMSound_PlayFreqTone(AEESoundInfo * psi, uint16 wHiFreq, uint16 wLoFreq, uint16 wDuration, void * pUser);

Parameters:
   psi: sound device information (See BREW SDK API Reference Guide for
   AEESoundInfo definition).
   wHiFreq: high frequency of the specified pair of tone frequencies
   wLoFreq: low frequency of the specified pair of tone frequencies
   wDuration: duration of the tone play
   pUser: user data that will be passed back to the caller through the
          status callback function.

Return Value:
   None

Comments:
   None

Side Effects:
   AEESound_StatusCB is called with status.

See Also:
   None

========================================================================

Function: OEMSound_PlayToneList()

Description:
   This function issues a command to play a list of tones.

Prototype:
   void OEMSound_PlayToneList(AEESoundInfo * psi, AEESoundToneData * pToneData, uint16 wDataLen, void * pUser);

Parameters:
   psi: sound device information (See BREW SDK API Reference Guide for
   AEESoundInfo definition).
   pToneData: an array of tones and durations
   wDataLen: number of tones and durations
   pUser: user data that will be passed back to the caller through the
          status callback function.

Return Value:
   None

Comments:
   None

Side Effects:
   AEESound_StatusCB is called with status.

See Also:
   None

==========================================================================

Function: OEMSound_StopTone()

Description:
   This function issues a command to stop playing a single tone or a playlist

Prototype:
   void OEMSound_StopTone(boolean bPlayList, void * pUser);

Parameters:
   bPlayList: flag that specifies to stop playing a tone list playback.
   pUser: user data that will be passed back to the caller through the
          status callback function.

Return Value:
   None

Comments:
   None

Side Effects:
   AEESound_StatusCB is called with status.

See Also:
   None

========================================================================

Function: OEMSound_GetLevels()

Description:
   This function returns the number of volume levels supported for the
   device/class pair. It is called when user issues ISOUND_GetVolume() or
   ISOUND_SetVolume().

Prototype:
   void OEMSound_GetLevels(AEESoundInfo * psi, void * pUser);

Parameters:
   psi: sound device information (See BREW SDK API Reference Guide for
   AEESoundInfo definition).
   pUser: user data that will be passed back to the caller through the
          status callback function.

Return Value:
   None

Comments:
   None

Side Effects:
   AEESound_LevelCB is called with the result.

See Also:
   None

========================================================================

Function: OEMSound_SetVolume()

Description:
   This function issues a command to set the volume of sound device/class pair.

Prototype:
   void OEMSound_SetVolume(AEESoundInfo * psi, uint16 wLevel, void * pUser);

Parameters:
   psi: sound device information (See BREW SDK API Reference Guide for
   AEESoundInfo definition).
   wLevel: new volume for the device
   pUser: user data that will be passed back to the caller through the
          status callback function.

Return Value:
   None

Comments:
   None

Side Effects:
   AEESound_VolumeCB is called with the result.

See Also:
   None

========================================================================

Function: OEMSound_GetVolume()

Description:
   This function issues a command to get the volume level of device/class pair.

Prototype:
   void OEMSound_GetVolume(AEESoundInfo * psi, void * pUser);

Parameters:
   psi: sound device information (See BREW SDK API Reference Guide for
   AEESoundInfo definition).
   pUser: user data that will be passed back to the caller through the
          status callback function.

Return Value:
   None

Comments:
   None

Side Effects:
   AEESound_VolumeCB is called with the result.

See Also:
   None

===========================================================================

Function: OEMSound_Vibrate()

Description:
   It causes the handset to vibrate for the specified amount to time
   if the feature is supported; otherwise, it doesn't do anything.

Prototype:
   void OEMSound_Vibrate(uint16 wDuration, void * pUser);


Parameters:
   wDuration: duration of vibration in milliseconds
   pUser: user data that will be passed back to the caller through the
          status callback function.

Return Value:
   None

Comments:
   None

Side Effects:
   AEESound_StatusCB is called with status.

See Also:
   None

===========================================================================

Function: OEMSound_StopVibrate()

Description:
   It stops the current vibration.

Prototype:
   void OEMSound_StopVibrate(void * pUser);

Parameters:
   pUser: user data that will be passed back to the caller through the
          status callback function.

Return Value:
   None

Comments:
   None

Side Effects:
   AEESound_StatusCB is called with status.

See Also:
   None

==============================================================
AEESound

See the ISound Interface in the BREW API Reference.

========================================================================*/


#endif

#endif /* OEM_SOUND_H */
