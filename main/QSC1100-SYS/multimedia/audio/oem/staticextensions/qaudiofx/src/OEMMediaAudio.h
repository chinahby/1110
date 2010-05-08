#ifndef OEMMEDIAAUDIO_H
#define OEMMEDIAAUDIO_H
/*============================================================================
FILE:	OEMMediaAudio.h

SERVICES:  Internal interface for OEMAudio.c

DESCRIPTION:
   This file is contains functinos for OEMMediaAudio.c that can be called
   from elsewhere in the OEM layer
   
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#include "AEEMedia.h"
#include "AEEMediaAudio.h"

//---------------------------------------------------------------------------
//    Function Declarations
//---------------------------------------------------------------------------

int     IMediaAudioEnvFX_New(IShell * ps, AEECLSID cls, void **ppif, IMediaAudioEnv *pEnv);

#if defined (FEATURE_QAUDIOFX)
int     IMediaAudio3D_New(IShell * ps, AEECLSID cls, void **ppif, IMedia * pIMedia);
int     IMediaAudioMediaFX_New(IShell * ps, AEECLSID cls, void **ppif, IMedia * pIMedia);
#endif // defined (FEATURE_QAUDIOFX)
#endif // #ifndef OEMMEDIAAUDIO_H
