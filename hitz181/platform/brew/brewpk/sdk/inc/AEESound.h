#ifndef AEESOUND_H
#define AEESOUND_H
/*============================================================================
FILE: AEESound.h

SERVICES:  Basic Sound Services

DESCRIPTION:
   This file provides definitions for basic sound interfaces
   available to application developers.


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

#include "../../inc/AEEISound.h"
#include "AEE.h"
#include "AEEResourceCtl.h"

/* added missing symbols */
#define ISOUND_AddRef ISound_AddRef
#define ISOUND_Get ISound_Get
#define ISOUND_GetResourceCtl ISound_GetResourceCtl
#define ISOUND_GetVolume ISound_GetVolume
#define ISOUND_PlayFreqTone ISound_PlayFreqTone
#define ISOUND_PlayTone ISound_PlayTone
#define ISOUND_PlayToneList ISound_PlayToneList
#define ISOUND_RegisterNotify ISound_RegisterNotify
#define ISOUND_Release ISound_Release
#define ISOUND_Set ISound_Set
#define ISOUND_SetDevice ISound_SetDevice
#define ISOUND_SetVolume ISound_SetVolume
#define ISOUND_StopTone ISound_StopTone
#define ISOUND_StopVibrate ISound_StopVibrate
#define ISOUND_Vibrate ISound_Vibrate


struct ISound {
   const AEEVTBL(ISound)* pvt;
};

#endif // AEESOUND_H
