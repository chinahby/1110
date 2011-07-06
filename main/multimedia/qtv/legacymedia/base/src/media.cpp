/* =======================================================================
                              media.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/base/main/latest/src/media.cpp#21 $
$DateTime: 2010/04/20 22:35:42 $
$Change: 1273913 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "media.h"
#include "fileMedia.h"

#include "ITrackSelectionPolicy.h"
#include "DefaultTrackSelectionPolicy.h"



#ifdef FEATURE_QTV_BCAST_FLO
#error code not present
#endif /* FEATURE_QTV_BCAST_FLO */

#ifdef FEATURE_QTV_GENERIC_BCAST
#include "GenericBcastMedia.h"
#endif


/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
//Audio parameters
const int Media::EVRC_SAMPLES_PER_FRAME     = 160;
int Media::QCELP_SAMPLES_PER_FRAME          = 160;
const int Media::AMR_SAMPLES_PER_FRAME      = 160;
const int Media::MP3_SAMPLES_PER_FRAME      = 576;
const int Media::AMR_WB_SAMPLES_PER_FRAME          = 320;
/* Setting with default value because not using this in AMR WB+ */
const int Media::AMR_WB_PLUS_SAMPLES_PER_FRAME     = 2048;
const int Media::EVRC_NB_SAMPLES_PER_FRAME      = 160;
const int Media::EVRC_WB_SAMPLES_PER_FRAME   = 320;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */


Media::Media()
{
  BaseInitData();
}

Media::~Media()
{
}

/* ======================================================================
FUNCTION
  Media::CanPlay

DESCRIPTION
  Tell whether player supports the argument track.

DEPENDENCIES
  None

RETURN VALUE
  True if supported, false otherwise.

SIDE EFFECTS
  None
========================================================================== */
bool Media::CanPlay(const TrackInfo *pTrackInfo)
{
  bool bRet = false;

  if (pTrackInfo == NULL)
  {
     return false;
  }

  if (pTrackInfo->contentVersion > PLAYABLE_CONTENT_VERSION)
  {
    return false;
  }

  switch (pTrackInfo->codec)
  {
    case AAC_CODEC:
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
    if ((pTrackInfo->numAudioChannels < 1) ||
        (pTrackInfo->numAudioChannels > 2))
    {
       bRet = false;
    }
    else
    {
       bRet = true;
    }
    break;

#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case WMA_CODEC:
    // Windows Media Audio not supported on LTK yet.
#ifdef PLATFORM_LTK
#error code not present
#else
    bRet = true;
#endif /* PLATFORM_LTK */
    break;
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
    case WMA_PRO_CODEC:
    case WMA_PRO_PLUS_CODEC:
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */
    case EVRC_CODEC:
    case QCELP_CODEC:
#ifdef FEATURE_MP3
    case MP3_CODEC:
#endif /* FEATURE_MP3 */
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
    case Media::NONMP4_MP3_CODEC:   
    case Media::NONMP4_AAC_CODEC:
    case Media::NONMP4_AMR_CODEC:
    case Media::QCP_CODEC:
    case Media::MIDI_CODEC:
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#ifdef FEATURE_MP4_AMR
    case GSM_AMR_CODEC:
#endif
    case MPEG4_CODEC:
    case H263_CODEC:
#ifdef FEATURE_H264_DECODER
    case H264_CODEC:
#endif /* FEATURE_H264_DECODER */
#ifdef FEATURE_QTV_OSCAR_DECODER
#error code not present
#endif /* FEATURE_QTV_OSCAR_DECODER */
    case STILL_IMAGE_CODEC:
    case STILL_IMAGE_H263_CODEC:
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    case TIMED_TEXT_CODEC:
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#ifdef FEATURE_QTV_ISDB_SUBTITLES
#error code not present
#endif
#ifdef FEATURE_QTV_SKT_MOD
    case JPEG_CODEC:
#endif /* FEATURE_QTV_SKT_MOD */
#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case WMV1_CODEC:
    case WMV2_CODEC:
    case WMV3_CODEC:
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#ifdef FEATURE_QTV_3GPP_AMR_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB */
#ifdef FEATURE_QTV_3GPP_AMR_WB_PLUS
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB_PLUS */
#ifdef FEATURE_QTV_3GPP_EVRC_NB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_NB */
#ifdef FEATURE_QTV_3GPP_EVRC_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_WB */
#ifdef FEATURE_DIVX_311_ENABLE
    case DIVX311_CODEC:
#endif
#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /*  FEATURE_QTV_AVI_AC3 */ 
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */
      bRet = true;
      break;

    case UNKNOWN_CODEC:
    default:
      bRet = false;
      break;
  }
  return bRet;
}

#ifdef FEATURE_QTV_GENERIC_BCAST
/* ======================================================================
FUNCTION
  Media::CanCodecPlay

DESCRIPTION
  Tell whether player supports the argument track.
  This routine allows unknown channel configuration also. 
  Otherwise this is very similar to CanPlay routine. 

DEPENDENCIES
  None

RETURN VALUE
  True if supported, false otherwise.

SIDE EFFECTS
  None
========================================================================== */
bool Media::CanCodecPlay(TrackInfo *pTrackInfo)
{
  bool bRet = false;

  if (pTrackInfo == NULL)
  {
     return false;
  }

  if (pTrackInfo->contentVersion > PLAYABLE_CONTENT_VERSION)
  {
    return false;
  }

  switch (pTrackInfo->codec)
  {
    case AAC_CODEC:
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
    if (pTrackInfo->numAudioChannels > 2)
    {
       bRet = false;
    }
    else
    {
       bRet = true;
    }
    break;

#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case WMA_CODEC:
    // Windows Media Audio not supported on LTK yet.
#ifdef PLATFORM_LTK
#error code not present
#else
    bRet = true;
#endif /* PLATFORM_LTK */
    break;
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

    case EVRC_CODEC:
    case QCELP_CODEC:
#ifdef FEATURE_MP3
    case MP3_CODEC:
#endif /* FEATURE_MP3 */
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
    case Media::NONMP4_MP3_CODEC:   
    case Media::NONMP4_AAC_CODEC:
    case Media::NONMP4_AMR_CODEC:
    case Media::QCP_CODEC:
    case Media::MIDI_CODEC:
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#ifdef FEATURE_MP4_AMR
    case GSM_AMR_CODEC:
#endif
    case MPEG4_CODEC:
    case H263_CODEC:
#ifdef FEATURE_H264_DECODER
    case H264_CODEC:
#endif /* FEATURE_H264_DECODER */
#ifdef FEATURE_QTV_OSCAR_DECODER
#error code not present
#endif /* FEATURE_QTV_OSCAR_DECODER */
    case STILL_IMAGE_CODEC:
    case STILL_IMAGE_H263_CODEC:
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    case TIMED_TEXT_CODEC:
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#ifdef FEATURE_QTV_ISDB_SUBTITLES
#error code not present
#endif /* FEATURE_QTV_ISDB_SUBTITLES */
#ifdef FEATURE_QTV_SKT_MOD
    case JPEG_CODEC:
#endif /* FEATURE_QTV_SKT_MOD */
#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case WMV1_CODEC:
    case WMV2_CODEC:
    case WMV3_CODEC:
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
    case WMA_PRO_CODEC:
    case WMA_PRO_PLUS_CODEC:
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER)  || || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)*/
#ifdef FEATURE_QTV_3GPP_AMR_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB */
#ifdef FEATURE_QTV_3GPP_AMR_WB_PLUS
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB_PLUS */
#ifdef FEATURE_QTV_3GPP_EVRC_NB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_NB */
#ifdef FEATURE_QTV_3GPP_EVRC_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_WB */
#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /*  FEATURE_QTV_AVI_AC3 */ 
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */ 
      bRet = true;
      break;

    case UNKNOWN_CODEC:
    default:
      bRet = false;
      break;
  }
  return bRet;
}
#endif 
/* ======================================================================
FUNCTION
  Media::IsAudioCodec

DESCRIPTION
  Tell whether the argument codec is an audio codec.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None
========================================================================== */
bool Media::IsAudioCodec(CodecType codec)
{
  switch (codec)
  {
    case AAC_CODEC:
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
    case EVRC_CODEC:
    case QCELP_CODEC:
#ifdef FEATURE_MP3
    case MP3_CODEC:
#endif /* FEATURE_MP3 */
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
    case NONMP4_MP3_CODEC:   
    case NONMP4_AAC_CODEC:
    case NONMP4_AMR_CODEC:
    case QCP_CODEC:
    case MIDI_CODEC:
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#ifdef FEATURE_MP4_AMR
    case GSM_AMR_CODEC:
#endif
#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case WMA_CODEC:
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
    case WMA_PRO_CODEC:
    case WMA_PRO_PLUS_CODEC:
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */
#ifdef FEATURE_QTV_3GPP_AMR_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB */
#ifdef FEATURE_QTV_3GPP_AMR_WB_PLUS
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB_PLUS */
#ifdef FEATURE_QTV_3GPP_EVRC_NB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_NB */
#ifdef FEATURE_QTV_3GPP_EVRC_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_WB */
#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /*  FEATURE_QTV_AVI_AC3 */ 
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */
    return true;

    default:
    return false;
  }
}

/* ======================================================================
FUNCTION
  Media::IsVideoCodec

DESCRIPTION
  Tell whether the argument codec is a video codec.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None
========================================================================== */
bool Media::IsVideoCodec(CodecType codec)
{
  switch (codec)
  {
    case MPEG4_CODEC:
    case H263_CODEC:
#ifdef FEATURE_H264_DECODER
    case H264_CODEC:
#endif /* FEATURE_H264_DECODER */
#ifdef FEATURE_QTV_OSCAR_DECODER
#error code not present
#endif /* FEATURE_QTV_OSCAR_DECODER */
    case STILL_IMAGE_CODEC:
    case STILL_IMAGE_H263_CODEC:
#ifdef FEATURE_QTV_SKT_MOD
    case JPEG_CODEC:
#endif /* FEATURE_QTV_SKT_MOD */
#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case WMV1_CODEC:
    case WMV2_CODEC:
    case WMV3_CODEC:
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#ifdef FEATURE_DIVX_311_ENABLE
    case DIVX311_CODEC:
#endif
    return true;

    default:
    return false;
  }
}

/* ======================================================================
FUNCTION
  Media::IsTextCodec

DESCRIPTION
  Tell whether the argument codec is a text codec.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None
========================================================================== */
bool Media::IsTextCodec(CodecType codec)
{
  switch (codec)
  {
    case TIMED_TEXT_CODEC:
    case GENERIC_TEXT_CODEC:
      return true;

    default:
    return false;
  }
}

#if defined (FEATURE_QTV_WINDOWS_MEDIA) || defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
/* ======================================================================
FUNCTION
  Media::IsWindowsMediaCodec

DESCRIPTION
  Determine if the codec is Windows Media audio or video.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None
========================================================================== */
bool Media::IsWindowsMediaCodec(CodecType codec)
{
  switch (codec)
  {
#ifdef FEATURE_QTV_WINDOWS_MEDIA   
    case WMA_CODEC:
    case WMV1_CODEC:
    case WMV2_CODEC:
    case WMV3_CODEC:
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#ifdef FEATURE_QTV_WMA_PRO_DECODER
    case WMA_PRO_CODEC:
    case WMA_PRO_PLUS_CODEC:
#endif /* FEATURE_QTV_WMA_PRO_DECODER */
    return true;

    default:
    return false;
  }
}
#endif /* defined (FEATURE_QTV_WINDOWS_MEDIA) || defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)*/


/* ======================================================================
FUNCTION
  Media::ConvertAudioTimestamp

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int64 Media::ConvertAudioTimestamp(unsigned long timestamp,bool, int idx)
{
  if (audioTimescale[idx]!=0)
  {
    //must use float arithmetic to avoid overflow, noticed
    //that EVRC will overflow at 9 minutes.
    float f = (float) TIMESCALE_BASE * (float)timestamp / (float)audioTimescale[idx];
    return (int64)f;
  }
  else
  {
    return -1;
  }
}

/* ======================================================================
FUNCTION
  Media::ConvertVideoTimestamp

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int64 Media::ConvertVideoTimestamp(unsigned long timestamp,int idx)
{
  if (videoTimescale[idx]!=0)
  {
    //integer arithment might be OK here but use float to be safe.
    float f = (float)TIMESCALE_BASE * (float)timestamp  / (float)videoTimescale[idx];
    return (int64)f;
  }
  else
  {
    return -1;
  }
}

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
/* ======================================================================
FUNCTION
  Media::ConvertTextTimestamp

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int64 Media::ConvertTextTimestamp(unsigned long timestamp,int idx)
{
  if (textTimescale[idx]!=0)
  {
    //integer arithment might be OK here but use float to be safe.
    float f = (float)TIMESCALE_BASE * (float)timestamp  / (float)textTimescale[idx];
    return (int64)f;
  }
  else
  {
    return -1;
  }
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

/* ======================================================================
FUNCTION
  Media::BaseInitData

DESCRIPTION
  Initialize all members of Media.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Everything is reset.

========================================================================== */
void Media::BaseInitData()
{
  audioCodecType = UNKNOWN_CODEC;
  audioSamplesPerFrame = 0;
  videoCodecType = UNKNOWN_CODEC;
  numAudioTracks = 0;
  AACChannelMode = 0;
  numVideoTracks = 0;
  videoLoopTrackTimeOffset = 0;
  audioLoopTrackTimeOffset = 0;
  textLoopTrackTimeOffset  = 0;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  numTextTracks = 0;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */

  for (int i=0;i<COMMON_MAX_LAYERS;i++)
  {
    audioTrackIDs[i]=0;
    audioTimescale[i]=0;
    audioSamplingFreq[i]=0;
    videoTrackIDs[i]=0;
    videoTimescale[i]=0;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    textTrackIDs[i]=0;
    textTimescale[i]=0;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  }

#ifdef FEATURE_MP4_AAC_FRAME_MARKER
   aacFrameMarkerNeeded = false;
#endif /* FEATURE_MP4_AAC_FRAME_MARKER */
}

/* ======================================================================
FUNCTION
  Media::GetTrackList

DESCRIPTION
  Retrieve the track list created by GetSelectedTrackList().
  This method can possibly be removed if GetSelecteTrackList() is
  made to check if track selection has run already.

DEPENDENCIES

RETURN VALUE
  True iff successful.

SIDE EFFECTS
  None

========================================================================== */
bool Media::GetTrackList(ITrackList **ppTrackList /* out */)
{
  QTV_NULL_PTR_CHECK(ppTrackList, false);

  m_trackList.SaveToOutPointer(ppTrackList);
  return true;
}

/* ======================================================================
FUNCTION
  Media::SetTrackSelectionPolicy

DESCRIPTION
  Set the policy used to select tracks. 

DEPENDENCIES
  None.

RETURN VALUE
  Return iff successfull.

SIDE EFFECTS
  The argument will be saved and AddRefed.

========================================================================== */
bool Media::SetTrackSelectionPolicy(ITrackSelectionPolicy *policy /* in */)
{
  m_trackSelectionPolicy = policy;  
  return true;
}

// Now mark tracks selected.
// Use the user-supplied policy if present, otherwise use the fallback
// policy.
bool Media::SelectTracks(ITrackList *trackList /* in */,
                         ITrackSelectionPolicy *fallbackPolicy /* in */)
{
  QTV_NULL_PTR_CHECK(trackList, false);

  ReferenceCountedPointer<ITrackSelectionPolicy> trackSelector = 
    m_trackSelectionPolicy;

  if (trackSelector == NULL)
  {
    QTV_NULL_PTR_CHECK(fallbackPolicy, false);

    trackSelector = fallbackPolicy;
  }

  trackSelector->Notify(ITrackSelectionPolicy::EVENT_TYPE_MEDIA_SOURCE_TYPE,
    (void *) GetSource());

  trackSelector->SelectTracks(trackList, false);
        
  return true;
}

/* ======================================================================
FUNCTION
  Media::GetFileTypeByComponents

DESCRIPTION
  Given a set of boolean flags indicating if each components is present
  return the corresponding MediaFileType.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None

========================================================================== */
Media::MediaFileType Media::GetFileTypeByComponents(bool hasAudio, 
  bool hasVideo, bool hasStillImage
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
, bool hasText
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */                                        
)
{
  MediaFileType type = CONTENT_UNKNOWN;

  if (hasAudio)
  {
    if (hasVideo)
    {
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      if (hasText)
      {
        type = CONTENT_AUDIO_VIDEO_TEXT;
      }
      else
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
      {
        type = CONTENT_AUDIO_VIDEO;
      }
    }
    else if (hasStillImage)
    {
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      if (hasText)
      {
        type = CONTENT_AUDIO_VIDEO_STILL_IMAGE_TEXT;
      }
      else
#endif /*  FEATURE_MP4_3GPP_TIMED_TEXT */
      {
        type = CONTENT_AUDIO_VIDEO_STILL_IMAGE;
      }
    }
    else
    {
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      if (hasText)
      {
        type = CONTENT_AUDIO_TEXT;
      }
      else
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
      {
        type = CONTENT_AUDIO;
      }
    }
  }
  else
  {
    if ( hasVideo )
    {
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      if (hasText)
      {
        type = CONTENT_VIDEO_TEXT;
      }
      else
#endif /*  FEATURE_MP4_3GPP_TIMED_TEXT */
      {
        type = CONTENT_VIDEO;
      }
    }
    else if ( hasStillImage )
    {
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      if (hasText)
      {
        type = CONTENT_VIDEO_STILL_IMAGE_TEXT;
      }
      else
#endif /*  FEATURE_MP4_3GPP_TIMED_TEXT */
      {
        type = CONTENT_VIDEO_STILL_IMAGE;
      }
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    else
	{
	  if (hasText)
      {
        type = CONTENT_TEXT;
      }
    }
#endif /*  FEATURE_MP4_3GPP_TIMED_TEXT */
  }
  return type;     
}

uint32 Media::GetAudioTrackID(int index)
{
  if(index < COMMON_MAX_LAYERS)
    return audioTrackIDs[index];
  else
    return 0;
}

uint32 Media::GetVideoTrackID(int index)
{
  if(index < COMMON_MAX_LAYERS)
    return videoTrackIDs[index];
  else
    return 0;
}

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
uint32 Media::GetTextTrackID(int index)
{
  if(index < COMMON_MAX_LAYERS)
    return textTrackIDs[index];
  else
    return 0;
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */


unsigned long Media::GetNumAudioChannels(int)
{
  return 0;
}

unsigned long Media::GetAudioBitRate(int)
{
  return 0;
}
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
unsigned long Media::SetAudioBitRate(uint32 idx)
{
  QTV_USE_ARG1(idx);
  return 0L;
}

uint32 Media::GetTimeStampFromMedia() 
{ 
  return 0; 
}

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
unsigned long Media::SetFileSize(uint32 idx)
{
  QTV_USE_ARG1(idx);
  return 0L;
}
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

float Media::GetVideoFrameRate(int)
{
  return 0;
}

unsigned long Media::GetVideoBitRate(int)
{
  return 0;
}

unsigned long Media::GetAvgAudioBitRate(int)
{
  return 0;
}

unsigned long Media::GetAvgVideoBitRate(int)
{
  return 0;
}

unsigned long Media::GetMaxVideoFrameSize(int)
{
  return 0;
}

unsigned long Media::GetMaxAudioFrameSize(int)
{
  return 0;
}

unsigned long Media::GetMaxTextFrameSize(int)
{
  return 0;
}

bool Media::GetAudioTrackLanguage(OSCL_STRING &, uint32)
{
  return 0;
}

unsigned long Media::GetAudioBitsPerSample(int)
{
  return 0;
}

#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) 

unsigned long Media::GetAudioCodecVersion(int)
{
  return 0;
}

unsigned long Media::GetFixedAsfAudioPacketSize(int)
{
  return 0;
}

unsigned long Media::GetAudioEncoderOptions(int)
{
  return 0;
}

uint16 Media::GetAudioAdvancedEncodeOptions(int)
{
  return 0;
}

uint32 Media::GetAudioAdvancedEncodeOptions2(int)
{
  return 0;
}

uint16 Media::GetAudioChannelMask(int)
{
  return 0;
}

uint16 Media::GetAudioArmDataReqThr(int)
{
  return 0;
}

uint8 Media::GetAudioValidBitsperSample(int)
{
  return 0;
}
#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef FEATURE_FILE_FRAGMENTATION
unsigned long Media::GetNextFragMaxAudioBufferSizeDB(int)
{
  return 0;
}

unsigned long Media::GetNextFragMaxVideoBufferSizeDB(int)
{
  return 0;
}

unsigned long Media::GetNextFragMaxTextBufferSizeDB(int)
{
  return 0;
}
#endif /* FEATURE_FILE_FRAGMENTATION */

/* ======================================================================
FUNCTION
  Media::OpenFile

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
#ifdef FEATURE_QTV_MFDRM
#error code not present
#else /* FEATURE_QTV_MFDRM */
Media * Media::OpenFile(  const OSCL_STRING &name,
                          Mpeg4Player *pMpeg4Player,
                          bool bPlayVideo ,
                          bool bPlayAudio ,
                          bool bPlayText )
{
  Media *pMedia = QTV_New_Args( FileMedia, 
    (name, pMpeg4Player, bPlayVideo, bPlayAudio, bPlayText) );

  if(!pMedia)
  {
    return NULL;
  }

  if ( pMedia->GetFile() )
  {
    return pMedia;
  }
  else
  {
    QTV_Delete( pMedia ); /*lint !e449 */
    return NULL;
  }
}
#endif  /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_QTV_DRM_DCF
/* ======================================================================
FUNCTION
  Media::OpenFile

DESCRIPTION
  Open Media instance for suppporting DCF playback

DEPENDENCIES
None

INPUT PARAMETERS:
->inputStream:IxStream*
->urnType:It should be URN_INPUTSTREAM
->bPlayVideo:Indicates if this is video instance
->bPlayAudio:Indicates if this is audio instance
->bPlayText:Indicates if this is text instance

RETURN VALUE
Media*

SIDE EFFECTS
========================================================================== */

Media * Media::OpenFile(  dcf_ixstream_type inputStream,
                          QtvPlayer::URNTypeT urnType,
                          Mpeg4Player *pMpeg4Player,
                          bool bPlayVideo ,
                          bool bPlayAudio ,
                          bool bPlayText)

{
  Media *pMedia = QTV_New_Args( FileMedia, 
     (inputStream, urnType, pMpeg4Player, 
      bPlayVideo, bPlayAudio, bPlayText) );

  if(!pMedia)
  {
    return NULL;
  }
  // Has the OpenMediaFile() succeeded?
  if ( pMedia->GetFile() != NULL)
  {
    return pMedia;
  }
  else
  {
    QTV_Delete( pMedia );
    return NULL;
  }
}
#endif

/* ======================================================================
FUNCTION
  Media::OpenFile

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
Media * Media::OpenFile(unsigned char * pBuf,
            uint32 bufSize,
            Mpeg4Player *pMpeg4Player,
            bool bPlayVideo ,
            bool bPlayAudio ,
            bool bPlayText            
#if defined (FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                       ,bool bPseudoStream
                       ,uint32 wBufferOffset
                       ,uint32 wStartupTime
                       ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize
                       ,QtvPlayer::FetchBufferedDataT FetchBufferedData
                       ,QtvPlayer::InstanceHandleT handle
#endif /* FEATURE_QTV_PSEUDO_STREAM */
       /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
                       )
{
  Media *pMedia = NULL;

#if defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
  pMedia = QTV_New_Args( FileMedia, (pBuf, bufSize, pMpeg4Player, bPlayVideo, bPlayAudio, bPlayText, bPseudoStream, wBufferOffset, wStartupTime, FetchBufferedDataSize, FetchBufferedData, handle) );
#else   /* FEATURE_QTV_PSEUDO_STREAM */ /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
  pMedia = QTV_New_Args( FileMedia,(pBuf, bufSize, pMpeg4Player, bPlayVideo, bPlayAudio, bPlayText) );
#endif

  if(!pMedia)
  {
    return NULL;
  }

  if ( pMedia->GetFile() )
  {
    return pMedia;
  }
  else
  {
    QTV_Delete( pMedia );
    return NULL;
  }
}

/* ======================================================================
FUNCTION
  Media::GetAACDataFormat

DESCRIPTION
  Returns the format used for the AAC data.

DEPENDENCIES
  None

RETURN VALUE
  Returns one of the following AAC data formats: raw, pseudo raw or adts

SIDE EFFECTS
  None

========================================================================== */
Media::AACDataFormatType Media::GetAACDataFormat(void)
{
#ifdef PLATFORM_LTK
#error code not present
#else
  return AAC_DATA_FORMAT_PSEUDO_RAW;
#endif
}


#ifdef FEATURE_QTV_GENERIC_BCAST
/* ======================================================================
FUNCTION
  Media::ConstructGenericBcastMedia

DESCRIPTION
  Constructs a Generic broadcast media object.

  Media source identifier and the player context are passed as arguments 
  to this function. 

DEPENDENCIES
  None.

RETURN VALUE
  Returns a ptr to the newly constructed object.  If the ptr is NULL, there
  was insufficient memory to allocate the object.

SIDE EFFECTS
  None.

========================================================================== */
Media* Media::ConstructGenericBcastMedia
(
  void*                        pContext,
  Common::GenericBcastMediaSourceID eID
)
{
  return QTV_New_Args( GenericBcastMedia, (pContext,eID) );
}

#endif /* FEATURE_QTV_BCAST_ISDB */

#ifdef FEATURE_QTV_BCAST_FLO
#error code not present
#endif /* FEATURE_QTV_BCAST_FLO */

/* ======================================================================
FUNCTION
  Media::SetMediaPlayerCb

DESCRIPTION
Sets the funtion pointer which will be called whenever Media Layer has some
request for Media Player to process

DEPENDENCIES

INPUT
fnptr - funtion pointer to the function to be called.

RETURN VALUE
  None

SIDE EFFECTS
  None.
========================================================================== */
void Media::SetMediaPlayerCb(pfnMediaPlayerCb MediaPlayerCb)
{
  MediaPlayerCbFn = MediaPlayerCb;
}

#ifdef FEATURE_FILE_FRAGMENTATION
/* ======================================================================
FUNCTION
  Media::RepositionVideoAccessPoint

DESCRIPTION
Sets the RepositionVideoAccessPoint
DEPENDENCIES

INPUT  
SkipNumber,currentPosTimeStampMsec


SIDE EFFECTS
  None.
========================================================================== */
 uint32 Media:: RepositionVideoAccessPoint(int32 /*skipNumber*/, bool &bError ,uint32 /*currentPosTimeStampMsec*/)
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "RepositionVideoAccessPoint not supported in current Media");
     bError = FALSE;
     return 0;
   }
  
/* ======================================================================
FUNCTION
  Media::RepositionAudioAccessPoint

DESCRIPTION
Sets the RepositionAudioAccessPoint

DEPENDENCIES

INPUT  
SkipNumber,currentPosTimeStampMsec


SIDE EFFECTS
  None.
========================================================================== */
   uint32 Media::RepositionAudioAccessPoint(int32 /*skipNumber*/, bool &bError, uint32 /*currentPosTimeStampMsec*/)
  { 
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "RepositionAudioAccessPoint not supported in current Media");
     bError = FALSE;
     return 0;
   }
   
#endif /*FEATURE_FILE_FRAGMENTATION*/

/* ======================================================================
FUNCTION
Media::GetTimestampDeltaForCurrentLayeredVideoSample

DESCRIPTION
Gets the current time stamp delta for the input layer/track.
All the media sources can further implement this method to return the 
correct time stamp delta.

DEPENDENCIES
  None

INPUT  
  layer

RETURN VALUE
  The timestamp delta value for the current sample. Default to 0.
SIDE EFFECTS
  None.
========================================================================== */
unsigned long Media::GetTimestampDeltaForCurrentLayeredVideoSample(int)
{
  return 0;
}
/* ======================================================================
FUNCTION
Media::GetTimestampDeltaForCurrentAudioSample

DESCRIPTION

DEPENDENCIES
  None

INPUT  
  layer

RETURN VALUE
  The timestamp delta value for the current sample. Default to 0.
SIDE EFFECTS
  None.
========================================================================== */
unsigned long Media::GetTimestampDeltaForCurrentAudioSample(int)
{
  return 0;
}
