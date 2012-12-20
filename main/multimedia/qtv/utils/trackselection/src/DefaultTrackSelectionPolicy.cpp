/* =======================================================================
                       DefaultTrackSelectionPolicy.cpp
DESCRIPTION
  This module contains the definition of the 
  DefaultTrackSelectionPolicy class. This is implements the default algorithm
  for selecting tracks, which depends on the codec select criteria.
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/trackselection/main/latest/src/DefaultTrackSelectionPolicy.cpp#10 $
$DateTime: 2010/04/26 00:39:24 $
$Change: 1278389 $

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "media.h"
#include "IReferenceCountable.h"
#include "qcplayer_misc.h"
#include "DefaultTrackSelectionPolicy.h"
#include "QtvPlayer.h"

DefaultTrackSelectionPolicy::DefaultTrackSelectionPolicy()
 : m_selectionCriteria(DEFAULT_CRITERIA), m_doBlockH264(false)
{
  RESET_REFCOUNT;
}

DefaultTrackSelectionPolicy::DefaultTrackSelectionPolicy(
  streaming_codec_select_criteria selectionCriteria) 
  : m_selectionCriteria(selectionCriteria), m_doBlockH264(false)
{ 
  RESET_REFCOUNT;
}

DefaultTrackSelectionPolicy::~DefaultTrackSelectionPolicy()
{
  NONZERO_REFCOUNT_DESTRUCTOR_WARNING(DefaultTrackSelectionPolicy);
}

void DefaultTrackSelectionPolicy::SelectTracks(ITrackList *trackList /* in */, 
  bool reselect)
{
  QTV_USE_ARG1(reselect);

  int selectedAudioTrack = -1;
  int selectedVideoTrack = -1;

  QTV_NULL_PTR_CHECK(trackList, RETURN_VOID);

  QTV_MSG1(QTVDIAG_STREAMING, "QTVCONFIG_CODECSELECT_CRITERIA = %d", 
    m_selectionCriteria);

  int numTracks = (int) trackList->GetNumTracksAvailable();

  bool hasBaseMpeg4 = false;

  // Look for MPEG-4 base track.
  for (int i = 0; i < (int) numTracks; i++)
  {
    if (!trackList->IsPlayable(i))
    {
      continue;
    }

    Media::CodecType codecType = trackList->GetCodecType(i);

    //If an Mpeg4 base track. (Still image IS mpeg4!)
    if (((codecType == Media::MPEG4_CODEC) &&
         (trackList->GetDependsOnID(i) == -1) &&
         (selectedVideoTrack == -1)) ||
        (codecType == Media::STILL_IMAGE_CODEC))
    {
      // Set base mpeg4 track flag. 
      // CHECKME: Should we save the track index and use it to compare with the
      // dependsID of enhancement tracks?
      hasBaseMpeg4 = true;
      //Select track.
      if (trackList->SelectTrack(i))
      {
        selectedVideoTrack = i;
      }
    }
  }

  int evrcOrGsmIdx = -1;
  int32 trackBitRate = 0;
  int highRateCodecIndex = -1;
  int rejectedTrackIndex = -1;

  //Mark tracks to play based on the following:
  //AAC over EVRC or GSM, MPEG4 over H.263, first EVRC, AAC or MPEG4 track,
  //first MPEG4 track cannot be an enhanced track,
  //all MPEG4 tracks that depend on the first MPEG4 track.
  for (int i = 0; i < (int) numTracks; i++)
  {
    Media::CodecType codecType = trackList->GetCodecType(i);

    //Check whether player can play this track.
    if (!trackList->IsPlayable(i))
    {
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
      if (codecType == Media::AAC_CODEC )
#endif
      {
        rejectedTrackIndex = i;
      }
      continue;
    }

    //Audio selection block.
 #ifdef FEATURE_QTV_BSAC   
#error code not present
#else			
    if ((selectedAudioTrack < 0) && Media::IsAudioCodec(codecType))
#endif /* FEATURE_QTV_BSAC */
    {
      if ( (m_selectionCriteria == PERFORMANCE_BASED_STREAMING_CODEC_CRITERIA) ||
           (m_selectionCriteria == DEFAULT_CRITERIA) )
      {
        switch (codecType)
        {          
          case Media::GSM_AMR_CODEC:
          case Media::EVRC_CODEC:
          case Media::QCELP_CODEC:
            //Look for first GSM/EVRC track.
            if (evrcOrGsmIdx < 0)
            {
              //save this index but don't select the track yet,
              //since we still want to look for AAC.
              evrcOrGsmIdx = i;
            }
            break;

            // Disable audio if it's WMA on LTK, since the LTK doesn't 
            // support it yet.

#if defined(PLATFORM_LTK) && defined(FEATURE_QTV_WINDOWS_MEDIA)
#error code not present
#endif /* PLATFORM_LTK && FEATURE_QTV_WINDOWS_MEDIA */
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* PLATFORM_LTK && FEATURE_QTV_WINDOWS_MEDIA */

          default:
            if (trackList->SelectTrack(i))
            {
              selectedAudioTrack = i;
            }
            break;
        } // end of switch (codecType) 
      }
      else
      {
        QTV_MSG( QTVDIAG_MP4_PLAYER, "Codec Selection based on BitRate");
        if (highRateCodecIndex == -1)
        {
          trackBitRate = trackList->GetBitrateBPS(i);
          highRateCodecIndex = i;            
        }
        else
        {
          if (trackBitRate < trackList->GetBitrateBPS(i))
          {
            highRateCodecIndex = i;
          }
        }
      }
    } // if ((selectedAudioTrack < 0) && IsAudioCodec)

    //Video selection block.
    //If base Mpeg4 track has been selected, select all tracks that depend on it.
    if (hasBaseMpeg4)
    {
      //If this is an enhancement track for the base track, select it.
      if ((codecType == Media::MPEG4_CODEC) &&
          (trackList->GetDependsOnID(i) != -1))
      {
// This is messing up video timestamps when streaming Enhanced Layer Clips
// Don't select the enhancement layer(s) for QTV MSM 6050. Base Layer Video
// Only.
#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
        //Select enhancement track.
        if (trackList->SelectTrack(i))
        {
          selectedVideoTrack = i;
        }
#else
        //Don't Select enhancement track.
        (void) trackList->SelectTrack(i, false);
#endif  /* FEATURE_MP4_TEMPORAL_SCALABILITY */
      }
    }
    else
    {      
      if ((selectedVideoTrack < 0) &&
          (codecType == Media::H263_CODEC)||(codecType == Media::STILL_IMAGE_H263_CODEC))
      {
        //Select track.
        if (trackList->SelectTrack(i))
        {
          selectedVideoTrack = i;
        }
      }
#ifdef FEATURE_H264_DECODER
      else if ((selectedVideoTrack < 0)&& 
               (codecType == Media::H264_CODEC) && !m_doBlockH264)
      {
        //Select track.
        if (trackList->SelectTrack(i))
        {          
          selectedVideoTrack = i;
        }
      }
#endif /* FEATURE_H264_DECODER */
#ifdef FEATURE_QTV_OSCAR_DECODER
#error code not present
#endif /* FEATURE_QTV_OSCAR_DECODER */
#ifdef FEATURE_QTV_SKT_MOD
      else if ((selectedVideoTrack < 0) && 
               (codecType == Media::JPEG_CODEC))
      {
        //Select track.
        if (trackList->SelectTrack(i))
        {
          selectedVideoTrack = i;        
        }
      }
#endif /* FEATURE_QTV_SKT_MOD */
#ifdef FEATURE_QTV_WINDOWS_MEDIA
      else if ( (selectedVideoTrack < 0) && 
                ((codecType == Media::WMV1_CODEC) ||
                 (codecType == Media::WMV2_CODEC) ||
                 (codecType == Media::WMV3_CODEC)) )
      {
        //Select track.
        if (trackList->SelectTrack(i))
        {
          selectedVideoTrack = i;
        }
      }
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#ifdef FEATURE_DIVX_311_ENABLE
      else if ( (selectedVideoTrack < 0) && 
                (codecType == Media::DIVX311_CODEC) )                
      {
        //Select track.
        if (trackList->SelectTrack(i))
        {          
          selectedVideoTrack = i;
        }
      }
#endif
    } // if (hasBaseMpeg4)

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    // Always select timed text track if present
    if (codecType == Media::TIMED_TEXT_CODEC)
    {
      (void) trackList->SelectTrack(i);
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  } //track loop

  //After checking all tracks if we did not find any AAC track
  //then select the GSM/EVRC track if any.
  if ((selectedAudioTrack < 0) && (evrcOrGsmIdx >= 0) &&
      (m_selectionCriteria != BITRATE_BASED_STREAMING_CODEC_CRITERIA))
  {
    if (trackList->SelectTrack(evrcOrGsmIdx))
    {
      selectedAudioTrack = evrcOrGsmIdx;
    }
  }

  if ((selectedAudioTrack < 0) && (highRateCodecIndex >= 0) &&
      (m_selectionCriteria == BITRATE_BASED_STREAMING_CODEC_CRITERIA) )
  {
    if (trackList->SelectTrack(highRateCodecIndex))
    {
      selectedAudioTrack = highRateCodecIndex;
      QTV_MSG1( QTVDIAG_MP4_PLAYER, "Bit Rate selected = %d", 
        trackList->GetBitrateBPS(highRateCodecIndex));
    }
  }

  // This is a work around for a server problem. In case of AAC PV server sends us wrong
  // config information in the SDP which results in we rejecting the track. If no other
  // track information is present in the SDP we will go ahead and select that track because
  // the PC version of PV player plays that track
  if ((selectedAudioTrack < 0) && (rejectedTrackIndex >= 0))
  {
    if (trackList->SelectTrack(rejectedTrackIndex))
    {
      selectedAudioTrack = rejectedTrackIndex;
    }
  }  
}

void DefaultTrackSelectionPolicy::SetCodecSelectCriteria(
  streaming_codec_select_criteria selectionCriteria)
{
  m_selectionCriteria = selectionCriteria;
}

void DefaultTrackSelectionPolicy::BlockH264(bool doBlock)
{
  m_doBlockH264 = doBlock;
}

bool DefaultTrackSelectionPolicy::Notify(int eventType, void *value)
{
  QTV_USE_ARG1(eventType);
  QTV_USE_ARG1(value);

  return false;
}
