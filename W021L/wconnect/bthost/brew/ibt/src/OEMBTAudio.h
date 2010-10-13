#ifndef OEMBTAUDIO_H
#define OEMBTAUDIO_H
/*=============================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMBTAudio.h

SERVICES:  AEE OEM BT Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required by the AEE to function with
BT.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEBT.h"

//----------------------------------------------------------------------------
#define OEMBT_SND_TYPE_NONE               0   // snd playback stopping
#define OEMBT_SND_TYPE_BT_SCO             1   // SCO supported sound
#define OEMBT_SND_TYPE_BT_OTHER           2   // other BT supported sound
#define OEMBT_SND_TYPE_BT_ALL             3   // SCO and other BT sound
#define OEMBT_SND_TYPE_OTHER_START        4   // non-BT support sound
#define OEMBT_SND_TYPE_OTHER_STOP         5   // 
#define OEMBT_SND_TYPE_DTMF_START         6   // key press sound
#define OEMBT_SND_TYPE_DTMF_STOP          7   // 
#define OEMBT_SND_TYPE_DTMF_RING_START    8   // DTMF ring sound
#define OEMBT_SND_TYPE_DTMF_RING_STOP     9   // 
typedef uint32 OEMBTSoundType;

#define OEMBT_SND_DEV_TYPE_AG             1
#define OEMBT_SND_DEV_TYPE_A2DP           2
typedef uint32 OEMBTSoundDevType;

#define OEMBT_STATE_AUDIO_CONNECTED            (1 << 0)
#define OEMBT_STATE_BRINGING_DOWN_AUDIO        (1 << 1)
#define OEMBT_STATE_BRINGING_UP_AUDIO          (1 << 2)
#define OEMBT_STATE_AG_CONNECTED               (1 << 3)
#define OEMBT_STATE_A2DP_CONNECTED             (1 << 4)
typedef uint32 OEMBTAudioState;

//----------------------------------------------------------------------------
typedef void (SoundChangeCallbackFn) (OEMBTSoundType);

//----------------------------------------------------------------------------
int  OEMBTAudio_RegisterSoundChangeCb (SoundChangeCallbackFn*, 
                                       OEMBTSoundDevType);
int  OEMBTAudio_DeregisterSoundChangeCb (SoundChangeCallbackFn*);
void OEMBTAudio_InitializeVolumes (OEMBTSoundDevType);
int  OEMBTAudio_EnableDevice (OEMBTSoundDevType);
int  OEMBTAudio_DisableDevice (OEMBTSoundDevType);

void     OEMBTAudio_SetState (OEMBTAudioState);
boolean  OEMBTAudio_DeregisterDeviceChangeCb (void);
boolean  OEMBTAudio_RegisterDeviceChangeCb (void);
#endif /* OEMBTAUDIO_H */

