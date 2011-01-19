#ifndef AEEMEDIA_H
#define AEEMEDIA_H

/*============================================================================
FILE: AEEMedia.h

SERVICES:  BREW MultiMedia Services

DESCRIPTION:
   This file defines an abstract interface for BREW MultiMedia format. All
   media formats like MIDI, MP3, QCP must be derived from this class.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

#include "../../inc/AEEIMedia.h"
#include "AEE.h"

/* added missing symbols */
#define IMEDIA_EnableChannelShare IMedia_EnableChannelShare
#define IMEDIA_FastForward IMedia_FastForward
#define IMEDIA_GetClassID IMedia_GetClassID
#define IMEDIA_GetMediaData IMedia_GetMediaData
#define IMEDIA_GetRect IMedia_GetRect
#define IMEDIA_GetState IMedia_GetState
#define IMEDIA_GetTotalTime IMedia_GetTotalTime
#define IMEDIA_GetVolume IMedia_GetVolume
#define IMEDIA_IsChannelShare IMedia_IsChannelShare
#define IMEDIA_IsFrameCallback IMedia_IsFrameCallback
#define IMEDIA_Pause IMedia_Pause
#define IMEDIA_Record IMedia_Record
#define IMEDIA_Release IMedia_Release
#define IMEDIA_Resume IMedia_Resume
#define IMEDIA_Rewind IMedia_Rewind
#define IMEDIA_Seek IMedia_Seek
#define IMEDIA_SeekFrame IMedia_SeekFrame
#define IMEDIA_SetAudioDevice IMedia_SetAudioDevice
#define IMEDIA_SetAudioPath IMedia_SetAudioPath
#define IMEDIA_SetMediaDataEx IMedia_SetMediaDataEx
#define IMEDIA_SetMuteCtl IMedia_SetMuteCtl
#define IMEDIA_SetPan IMedia_SetPan
#define IMEDIA_SetRect IMedia_SetRect
#define IMEDIA_SetTickTime IMedia_SetTickTime
#define IMEDIA_SetVolume IMedia_SetVolume
#define IMEDIA_AddRef IMedia_AddRef
#define IMEDIA_EnableFrameCallback IMedia_EnableFrameCallback
#define IMEDIA_GetFrame IMedia_GetFrame
#define IMEDIA_GetMediaParm IMedia_GetMediaParm
#define IMEDIA_Play IMedia_Play
#define IMEDIA_QueryInterface IMedia_QueryInterface
#define IMEDIA_RegisterNotify IMedia_RegisterNotify
#define IMEDIA_SetMediaData IMedia_SetMediaData
#define IMEDIA_SetMediaParm IMedia_SetMediaParm
#define IMEDIA_Stop IMedia_Stop


struct IMedia {
   IMediaVtbl* pvt;
};

#endif // AEEMEDIA_H
