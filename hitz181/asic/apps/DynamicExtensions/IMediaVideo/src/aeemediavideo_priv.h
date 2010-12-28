/*============================================================================
FILE:   aeemediavideo_priv.h

SERVICES:  OEM MediaVideo functions

DESCRIPTION:
   Definitions, typedefs, etc. for OEM MediaVideo functions.

PUBLIC CLASSES:  IMediaVideo

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2005-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/* =======================================================================
                             Edit History
$Header: //depot/asic/msmshared/apps/DynamicExtensions/IMediaVideo/src/aeemediavideo_priv.h#5 $
$DateTime: 2008/01/02 15:18:09 $
$Change: 588138 $

========================================================================== */
#ifndef AEEMEDIAVIDEO_PRIV_H
#define AEEMEDIAVIDEO_PRIV_H

#include "AEE.h"
#include "AEEShell.h"	
#include "AEEIMediaVideo.h"

typedef struct MVStreamingSetupTime
{
	uint32                m_dwStartupTime;   /* min. playback time required for startup */
	uint32                m_dwBufferingTime; /* min. playback time required for restart after buffering */
}MVStreamingSetupTime;

typedef struct
{
  int32 m_nHeight;
  int32 m_nWidth;
}MVClipDim;

typedef struct
{
  int32 m_nMPEG4;
  int32 m_nMV;
}MVMessageDataType;


typedef struct _MediaVideo
{
   // Declare our VTable
   DECLARE_VTBL(IMedia)

   IShell *                       m_pIShell;
   IModule*                       m_pIModule;
   int32                          m_nRefs;
   AEECLSID                       m_clsSelf;
   AEECallback                    m_pcbExit;
   PFNMEDIANOTIFY                 m_pfnNotify;
   void *                         m_pUser;

   AEEMediaVideoData              m_StatusData;
   IMedia *                       m_pIMediaMPEG4;       // point to IMediaMPEG4
   boolean                        m_bIMediaVideoMode;
   boolean                        m_bResume;
   MVStreamingSetupTime           m_defaultTimeSetting; // min playback time required for startup
                                                        // and restart after buffering
   AEEMediaMPEG4Spec*             m_pSpec;       
   boolean                        m_bSpec;              // spec is avaliable

   AEEMediaVideo3GPPTimedText*    m_pTimedText;
   AEEMediaVideoTelopTextElement* m_pTelopText;

   MVMessageDataType*             m_pTypeTable_PARM;      // MPEG4 and MediaVideo message type comparison table    
   int32                          m_VideoRotation;        // current rotation
   uint32                         m_VideoScaling_H;       // current horizontal scalling
   uint32                         m_VideoScaling_V;       // current vertical scalling

   boolean                        m_bRotationPending;     // true, if rotation is pending
   int                            m_nPendingRotation;
   boolean                        m_bScalingPending;      // true, if scaling is pending
   int                            m_PendingVideoScaling_H;
   int                            m_PendingVideoScaling_V;
}MediaVideo;


void MediaVideo_FreeIF(IBase ** ppif);
#define MV_RELEASEIF(p) MediaVideo_FreeIF((IBase **)&(p))


#endif //AEEMEDIAVIDEO_PRIV_H


