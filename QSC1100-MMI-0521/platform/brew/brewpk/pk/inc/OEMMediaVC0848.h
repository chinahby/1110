#ifndef OEMMEDIAVC0848_H
#define OEMMEDIAVC0848_H
/*============================================================================
FILE:	OOEMMediaVC0848.h

SERVICES:  Internal interface for OEMMediaVC0848.c

DESCRIPTION:
   This file is contains functinos for OEMMediaVC0848.c that can be called
   from elsewhere in the OEM layer
   
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

============================================================================*/
#include "AEEMedia.h"
#include "OEMMediaCMX.h"

//MPEG4
//#ifndef OEMMPEG4FORMAT_H
//#define OEMMPEG4FORMAT_H

#include "customer.h"
#ifndef WIN32
#include "mp4buf.h"
#else
#include "oemhelperfunctype.h"
#endif
#include "AEEMediaMPEG4.h"

#if defined (FEATURE_APP_MPEG4) || defined (FEATURE_SUPPORT_VC0848)

#include "AEE_OEM.h"
#include "AEEMediaFormats.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/*-------------------------------------------------------------------
      Macros Declarations
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT

typedef struct AEETelopTextElement
{
  int32       dwTelopSize;
  int32       dwBeginTime;
  int32       dwDuration;
  int32       dwNumberOfSubStrings;
  boolean     bWrapFlag;
  int32       dwBackgroundColor;
  int32       dwDefaultFontColor;
} AEETelopTextElement;

typedef struct AEETelopTextSubString
{
  boolean     bLineFeed;
  boolean     bUnderline;
  boolean     bReversal;
  boolean     bLinking;
  int32       dwFontColor;
  int32       dwTextSampleSize;  // in bytes
  int32       dwLinkSize;
  char        *pTextSubString;
  char        *pLinkValue;
} AEETelopTextSubString;

#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

#ifdef FEATURE_QTV_VIDEO_EDITING
typedef struct AEEDataFrameInfo
{
  uint32 FrameNum;      /* sample number (first sample is zero)               */
  uint32 FrameSize;     /* size of sample in bytes                            */
  uint32 FrameOffset;   /* offset of sample in stream, in bytes               */
  uint32 FrameTime;     /* composition time of sample, in the msec            */
  uint32 Framedelta;    /* difference between composition time of this sample */
                        /* and the next sample, in msec                       */
  uint32 IsSyncFrame;   /* Indication if sample is a random access point      */
                        /* (I-Frame)(non-zero) or not (zero)                  */
} AEEDataFrameInfo;
#endif /* FEATURE_QTV_VIDEO_EDITING */


#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif


#if defined(FEATURE_QTV_PDP_SELECTION) || defined(FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* if FEATURE_QTV_PDP_SELECTION || FEATURE_QTV_QOS_SELECTION */


typedef struct AEEMediaPlaybackPos
{
   uint32      dwVideoPos;    // video position in ms
   uint32      dwAudioPos;    // audio position in ms
   uint32      dwTextPos;     // text position in ms
   uint32      dwPlaybackPos; // max of the three positions in ms
} AEEMediaPlaybackPos;

#define IMEDIA_GetPlaybackPositions(p, pPlaybackPos) \
          IMEDIA_GetMediaParm((IMedia *)(p), \
                               MM_MP4_PARAM_PLAYBACK_POS,\
                               (int32*)(pPlaybackPos),\
                               NULL)

int AEEMedia848Util_CreateMediaEx( IShell * ps,
                                AEEMediaDataEx * pmdList,
                                int nListCount,
                                IMedia ** ppm );



/*===========================================================================

                      IMediaMPEG4 DECLARATIONS

===========================================================================*/

// Called by AEE_Init() during BREW initialization
void  IMediaVC0848MPEG4_Init(IShell * ps);
// Called by ISHELL_CreateInstance() during object instantiation
int   IMediaVC0848MPEG4_New(IShell * ps, AEECLSID cls, void **ppif);

boolean AllocateOutputBuffers(MP4BufType *pBuf, uint16 Height, uint16 Width);
void FreeOutputBuffers(MP4BufType *pBuf);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif  // FEATURE_APP_MPEG4

//#endif /* #ifndef OEMMPEG4FORMAT_H*/

//---------------------------------------------------------------------------
//    Function Declarations
//---------------------------------------------------------------------------
#endif // #ifndef OEMMEDIAVC0848_H