/*===========================================================================

FILE: OEMMediaFormats.c

SERVICES: IMEDIA interface implementation

DESCRIPTION
   This file implements modtable of IMedia-based classes that handle various 
   multimedia formats.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"

#if defined(FEATURE_BREW_MULTIMEDIA)

#include "OEMMediaFormats.h"

#if defined(FEATURE_APP_REAL)
  #include "AEEMediaReal.h"
  #include "OEMMediaReal.h"
#endif

#if defined(FEATURE_APP_MPEG4)
  #include "OEMMediaMPEG4.h"
#endif

#ifdef FEATURE_AUDFMT_AMR_WB
#include "AEECLSID_MEDIAAMRWB.bid"
#endif

#include "AEEMedia.bid"

/*===========================================================================

                      MODTABLE DEFINITIONS

===========================================================================*/
extern void IMediaMain_Init(IShell * ps);
extern int  IMediaMain_New(IShell * ps, AEECLSID cls, void **ppif);

extern void IMediaBg_Init(IShell * ps);
extern int  IMediaBg_New(IShell * ps, AEECLSID cls, void **ppif);

#if defined(FEATURE_WEBAUDIO)
extern void IMediaPMD_Init(IShell * ps);
extern int  IMediaPMD_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

#if defined(FEATURE_MIDI_OUT)
extern void    IMediaMIDIOutMsg_Init(IShell * ps);
extern int     IMediaMIDIOutMsg_New(IShell * ps, AEECLSID cls, void **ppif);
#if defined(FEATURE_MIDI_OUT_QCP)
extern void    IMediaMIDIOutQCP_Init(IShell * ps);
extern int     IMediaMIDIOutQCP_New(IShell * ps, AEECLSID cls, void **ppif);
#endif // defined(FEATURE_MIDI_OUT_QCP)
#endif // defined(FEATURE_MIDI_OUT)

extern int IMediaUtil_New(IShell * ps, AEECLSID cls, void **ppif);

#if defined(FEATURE_DLS)
extern int DLS_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

const AEEStaticClass gAEEMediaClasses[] = 
{
   {AEECLSID_MEDIAUTIL,       ASCF_UPGRADE, 0, NULL,           IMediaUtil_New},
   {AEECLSID_MEDIAMIDI,       ASCF_UPGRADE, 0, IMediaBg_Init,  IMediaBg_New},
#ifdef FEATURE_MP3
   {AEECLSID_MEDIAMP3,        ASCF_UPGRADE, 0, IMediaBg_Init,  IMediaBg_New},
#endif
#if !defined(FEATURE_PEKTEST)
   {AEECLSID_MEDIAQCP,        ASCF_UPGRADE, 0, IMediaMain_Init,IMediaMain_New},
#ifdef FEATURE_WEBAUDIO
   {AEECLSID_MEDIAPMD,        ASCF_UPGRADE, 0, IMediaPMD_Init,  IMediaPMD_New},
#endif
#ifdef FEATURE_SMAF
   {AEECLSID_MEDIAMMF,        ASCF_UPGRADE, 0, IMediaBg_Init,  IMediaBg_New},
   {AEECLSID_MEDIAPHR,        ASCF_UPGRADE, 0, IMediaBg_Init,  IMediaBg_New},
#endif
#if defined(FEATURE_WAVE_PB)
   {AEECLSID_MEDIAPCM,        ASCF_UPGRADE, 0, IMediaMain_Init,IMediaMain_New},
#endif
#ifdef FEATURE_IMELODY
   {AEECLSID_MEDIAIMELODY,    ASCF_UPGRADE, 0, IMediaBg_Init,  IMediaBg_New},
#endif /* FEATURE_IMELODY */
#ifdef FEATURE_AUDFMT_AMR
   {AEECLSID_MEDIAAMR,        ASCF_UPGRADE, 0, IMediaMain_Init,  IMediaMain_New},
#endif
#ifdef FEATURE_AUDFMT_AMR_WB
   {AEECLSID_MEDIAAMRWB,      ASCF_UPGRADE, 0, IMediaBg_Init,  IMediaBg_New},
#endif
#ifdef FEATURE_AUDFMT_EVB
   {AEECLSID_MediaEVRCNB,      ASCF_UPGRADE, 0, IMediaBg_Init,  IMediaBg_New},
#endif
#if defined(FEATURE_MIDI_OUT)
   {AEECLSID_MEDIAMIDIOUTMSG, ASCF_UPGRADE, 0, IMediaMIDIOutMsg_Init,  IMediaMIDIOutMsg_New},
#if defined(FEATURE_MIDI_OUT_QCP)
   {AEECLSID_MEDIAMIDIOUTQCP, ASCF_UPGRADE, 0, IMediaMIDIOutQCP_Init,  IMediaMIDIOutQCP_New},
#endif // defined(FEATURE_MIDI_OUT_QCP)
#endif // defined(FEATURE_MIDI_OUT)
#endif // FEATURE_PEKTEST

#if defined(FEATURE_BREW_CMX_V3) 
#if !defined(FEATURE_PEKTEST)
#if defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_ADPCM)
   {AEECLSID_MEDIAADPCM,      ASCF_UPGRADE, 0, IMediaMain_Init,  IMediaMain_New},
#endif // defined(FEATURE_ADPCM)
#if defined(FEATURE_APP_REAL)
   {AEECLSID_MEDIAREAL,      ASCF_UPGRADE, 0, IMediaReal_Init, IMediaReal_New},
#endif // defined(FEATURE_APP_REAL)
#if defined(FEATURE_AAC)
   {AEECLSID_MEDIAAAC,        ASCF_UPGRADE, 0, IMediaBg_Init,  IMediaBg_New},
#endif // defined(FEATURE_AAC)
#if defined(FEATURE_XMF)
   {AEECLSID_MEDIAXMF,        ASCF_UPGRADE, 0, IMediaBg_Init,  IMediaBg_New},
#endif // defined(FEATURE_XMF)
#if defined(FEATURE_DLS)
   {AEECLSID_MEDIADLS,        ASCF_UPGRADE, 0, NULL,  DLS_New},
#endif // defined(FEATURE_DLS)
#endif // FEATURE_PEKTEST
#if defined(FEATURE_APP_MPEG4)
      {AEECLSID_MEDIAMPEG4,   ASCF_UPGRADE, 0, IMediaMPEG4_Init, IMediaMPEG4_New}, 
#endif // defined(FEATURE_APP_MPEG4)
#endif // defined(FEATURE_BREW_CMX_V3)

   {0,0,0,NULL,NULL}
};

//
// Returns TRUE if the specified class ID matches the specified entry in the 
// media modtable.
// One of pfnInit or pfnNew can be NULL if that checking needs to be avoided.
//
boolean CMediaFormat_InModTable(AEECLSID cls, void * pfnInit, void * pfnNew)
{
   AEEStaticClass *  psc;

   if (!cls)
      return FALSE;
   
   if (!pfnInit && !pfnNew)
      return FALSE;

   for (psc = (AEEStaticClass *)gAEEMediaClasses; psc->cls && psc->cls != cls; psc++)
      ;
   if (psc->cls && psc->cls == cls)
   {
      if (pfnInit && pfnInit != (void *)psc->pfnInit)
         return FALSE;

      if (pfnNew && pfnNew != (void *)psc->pfnNew)
         return FALSE;

      return TRUE;
   }

   return FALSE;
}

#endif // defined(FEATURE_BREW_MULTIMEDIA)
