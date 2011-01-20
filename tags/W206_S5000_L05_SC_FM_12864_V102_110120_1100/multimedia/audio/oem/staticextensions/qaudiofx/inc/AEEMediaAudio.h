#ifndef AEEMEDIAAUDIO_H
#define AEEMEDIAAUDIO_H

/*============================================================================
FILE: AEEMediaAudio.h

SERVICES:  BREW Media Audio Special Effects Services

DESCRIPTION:
   Wrapper for interfaces to BREW Media Audio Special Effecs.

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2005-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "AEEIMediaAudioEnv.h"
#include "AEEIMediaAudio3D.h"
#include "AEEIMediaAudioFX.h"


/* added missing symbols */
#define AEEIID_MEDIAAUDIO3D               AEEIID_IMediaAudio3D
#define AEEIID_MEDIAAUDIOFX               AEEIID_IMediaAudioFX
#define AEEIID_MEDIAAUDIOENV              AEEIID_IMediaAudioEnv

#define IMEDIAAUDIO3D_AddRef              IMediaAudio3D_AddRef
#define IMEDIAAUDIO3D_Release             IMediaAudio3D_Release
#define IMEDIAAUDIO3D_QueryInterface      IMediaAudio3D_QueryInterface
#define IMEDIAAUDIO3D_GetParm             IMediaAudio3D_GetParm
#define IMEDIAAUDIO3D_SetParm             IMediaAudio3D_SetParm
#define IMEDIAAUDIO3D_GetPosition         IMediaAudio3D_GetPosition
#define IMEDIAAUDIO3D_SetPosition         IMediaAudio3D_SetPosition
#define IMEDIAAUDIO3D_GetRollOff          IMediaAudio3D_GetRollOff
#define IMEDIAAUDIO3D_SetRollOff          IMediaAudio3D_SetRollOff
#define IMEDIAAUDIO3D_GetMode             IMediaAudio3D_GetMode
#define IMEDIAAUDIO3D_SetMode             IMediaAudio3D_SetMode
#define IMEDIAAUDIO3D_GetVolume           IMediaAudio3D_GetVolume
#define IMEDIAAUDIO3D_SetVolume           IMediaAudio3D_SetVolume
#define IMEDIAAUDIO3D_GetVelocity         IMediaAudio3D_GetVelocity
#define IMEDIAAUDIO3D_SetVelocity         IMediaAudio3D_SetVelocity
#define IMEDIAAUDIO3D_GetPolarPosition    IMediaAudio3D_GetPolarPosition
#define IMEDIAAUDIO3D_SetPolarPosition    IMediaAudio3D_SetPolarPosition

#define IMEDIAAUDIOENV_AddRef             IMediaAudioEnv_AddRef
#define IMEDIAAUDIOENV_Release            IMediaAudioEnv_Release
#define IMEDIAAUDIOENV_QueryInterface     IMediaAudioEnv_QueryInterface
#define IMEDIAAUDIOENV_GetParm            IMediaAudioEnv_GetParm
#define IMEDIAAUDIOENV_SetParm            IMediaAudioEnv_SetParm
#define IMEDIAAUDIOENV_Commit             IMediaAudioEnv_Commit
#define IMEDIAAUDIOENV_Enable             IMediaAudioEnv_Enable
#define IMEDIAAUDIOENV_AddMedia           IMediaAudioEnv_AddMedia
#define IMEDIAAUDIOENV_RemoveMedia        IMediaAudioEnv_RemoveMedia
#define IMEDIAAUDIOENV_GetMediaList       IMediaAudioEnv_GetMediaList
#define IMEDIAAUDIOENV_GetListenerOrientation   IMediaAudioEnv_GetListenerOrientation
#define IMEDIAAUDIOENV_SetListenerOrientation   IMediaAudioEnv_SetListenerOrientation
#define IMEDIAAUDIOENV_GetListenerPosition      IMediaAudioEnv_GetListenerPosition
#define IMEDIAAUDIOENV_SetListenerPosition      IMediaAudioEnv_SetListenerPosition
#define IMEDIAAUDIOENV_EnableDeferCommit  IMediaAudioEnv_EnableDeferCommit
#define IMEDIAAUDIOENV_GetOutputDevice    IMediaAudioEnv_GetOutputDevice
#define IMEDIAAUDIOENV_GetListenerVelocity      IMediaAudioEnv_GetListenerVelocity
#define IMEDIAAUDIOENV_SetListenerVelocity      IMediaAudioEnv_SetListenerVelocity
#define IMEDIAAUDIOENV_SetOutputDevice    IMediaAudioEnv_SetOutputDevice

#define IMEDIAAUDIOFX_AddRef              IMediaAudioFX_AddRef
#define IMEDIAAUDIOFX_Release             IMediaAudioFX_Release
#define IMEDIAAUDIOFX_QueryInterface      IMediaAudioFX_QueryInterface
#define IMEDIAAUDIOFX_GetParm             IMediaAudioFX_GetParm
#define IMEDIAAUDIOFX_SetParm             IMediaAudioFX_SetParm
#define IMEDIAAUDIOFX_GetReverbGain       IMediaAudioFX_GetReverbGain
#define IMEDIAAUDIOFX_SetReverbGain       IMediaAudioFX_SetReverbGain
#define IMEDIAAUDIOFX_GetReverbPreset     IMediaAudioFX_GetReverbPreset
#define IMEDIAAUDIOFX_SetReverbPreset     IMediaAudioFX_SetReverbPreset
#define IMEDIAAUDIOFX_GetReverbDecayTime  IMediaAudioFX_GetReverbDecayTime
#define IMEDIAAUDIOFX_SetReverbDecayTime  IMediaAudioFX_SetReverbDecayTime
#define IMEDIAAUDIOFX_GetReverbDampingFactor    IMediaAudioFX_GetReverbDampingFactor
#define IMEDIAAUDIOFX_SetReverbDampingFactor    IMediaAudioFX_SetReverbDampingFactor

#endif // AEEMEDIAAUDIO_H
