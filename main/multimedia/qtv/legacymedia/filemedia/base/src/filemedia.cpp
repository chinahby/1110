/* =======================================================================
                              fileMedia.cpp  
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/base/main/latest/src/filemedia.cpp#39 $
$DateTime: 2010/06/24 17:18:48 $
$Change: 1346396 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"

#include "atomdefs.h"
#include "fileMedia.h"
#include "Mpeg4Player.h"
#include "videofmt_common.h"

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#include "kddiuserdataatom.h"
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#include "AVSync.h"
#include "Video.h"

#include "DefaultTrackSelectionPolicy.h"
#include "TrackList.h"

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
const int32 FileMedia::MIN_BUFFER_DURATION = 2000;
#endif

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

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

/* ======================================================================
FUNCTION
  FileMedia::GetSelectedTrackList

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
bool FileMedia::GetSelectedTrackList(ITrackList **ppTrackList /* out */)
{
  QTV_NULL_PTR_CHECK(ppTrackList, false);

  *ppTrackList = NULL;

  int32 numTracks = 0;

  if ( pBaseFile && pBaseFile->getNumTracks() )
  {
    numTracks = (int32) pBaseFile->getNumTracks();
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
      "FM GetSelectedTrackList: Number of Tracks = %d",numTracks);

    //Free up old list if one existed.
    m_trackList = NULL;

    TrackInfo *trackInfoArray = QTV_New_Array(TrackInfo, numTracks);

    if (trackInfoArray == NULL)
    {
      SetError();
      return false;
    }

    //Allocate temporary ID list.
    uint32 *trackIDList = QTV_New_Array(uint32, numTracks);   
    (void)pBaseFile->getTrackWholeIDList(trackIDList);

    if (trackIDList == NULL)
    {
      QTV_Delete_Array(trackInfoArray);
      return false;
    }

    for ( uint32 i = 0; i < numTracks; i++ )
    {
      OSCL_String<char> oTemp;
      trackInfoArray[i].trackID = trackIDList[i];
      // May be wrong for WM
      trackInfoArray[i].actualTrackID = trackIDList[i];
      trackInfoArray[i].bitrate = pBaseFile->getTrackAverageBitrate(trackIDList[i]);
      trackInfoArray[i].numAudioChannels = pBaseFile->GetNumAudioChannels(trackIDList[i]);
      if ( GetAudioTrackLanguage(oTemp, trackIDList[i]) )
      {         
         ZUtils::StrcpyN(trackInfoArray[i].language,oTemp.c_str(),QTV_MAX_LANGUAGE_BYTES);
      }      
      trackInfoArray[i].dependsOnID = (int)pBaseFile->trackDependsOn(trackIDList[i]);
#ifdef FEATURE_QTV_PDCF
      trackInfoArray[i].encryptionType = pBaseFile->getEncryptionType(trackIDList[i]); 
#endif /* FEATURE_QTV_PDCF */
      QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
               "Track ID: %d, BR: %d, depends on track ID: %d",
               trackInfoArray[i].trackID,trackInfoArray[i].bitrate,
               trackInfoArray[i].dependsOnID);
      //For some reason, file format and streaming return a different value if
      //the track doesn't depend on any other track (0 - file format, (-1) - streaming).
      //Make them the same for now.
      if ( trackInfoArray[i].dependsOnID == 0 ) trackInfoArray[i].dependsOnID = -1;
      trackInfoArray[i].contentVersion = pBaseFile->getTrackContentVersion(trackIDList[i]);
      
      switch ( pBaseFile->getTrackOTIType(trackIDList[i]) )
      {
        case AMR_AUDIO:
          trackInfoArray[i].codec = GSM_AMR_CODEC;
          break;
        case EVRC_AUDIO:
          trackInfoArray[i].codec = EVRC_CODEC;
          break;
        case PUREVOICE_AUDIO:
        case PUREVOICE_AUDIO_2:
          trackInfoArray[i].codec = QCELP_CODEC;
          break;
          //case AAC_AUDIO:
        case MPEG2_AAC_LC:
        case MPEG4_AUDIO:
          trackInfoArray[i].codec = AAC_CODEC;
          break;
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
        case MPEG4_VIDEO:
          trackInfoArray[i].codec = MPEG4_CODEC;
          break;
        case H263_IMAGE:
          trackInfoArray[i].codec = STILL_IMAGE_H263_CODEC;
          break;		  
        case MPEG4_IMAGE:
          trackInfoArray[i].codec = STILL_IMAGE_CODEC;
          break;
        case H263_VIDEO:
          trackInfoArray[i].codec = H263_CODEC;
          break;
#ifdef FEATURE_H264_DECODER
        case H264_VIDEO:
          trackInfoArray[i].codec = H264_CODEC;
          break;
#endif /* FEATURE_H264_DECODER */
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        case TIMED_TEXT:
          trackInfoArray[i].codec = TIMED_TEXT_CODEC;
          break;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
        case MP3_AUDIO:
          trackInfoArray[i].codec = MP3_CODEC;
          break;
#ifdef FEATURE_QTV_SKT_MOD
        case JPEG_VIDEO:
          trackInfoArray[i].codec = JPEG_CODEC;
          break;
        case BMP_VIDEO:
          trackInfoArray[i].codec = UNKNOWN_CODEC; /* not supported yet */
          break;
        case SKT_MOD_TEXT:
          trackInfoArray[i].codec = TIMED_TEXT_CODEC;
          break;
#endif /* FEATURE_QTV_SKT_MOD */

#ifdef FEATURE_QTV_WINDOWS_MEDIA

		case WM_VIDEO_7:
           trackInfoArray[i].codec = WMV1_CODEC;
           break;

        case WM_VIDEO_8:
           trackInfoArray[i].codec = WMV2_CODEC;
           break;

        case WM_VIDEO_9:
           trackInfoArray[i].codec = WMV3_CODEC;
          break;

        case WM_AUDIO:
          trackInfoArray[i].codec = WMA_CODEC;
          break;
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
        case WM_PRO_AUDIO:          
          trackInfoArray[i].codec = WMA_PRO_CODEC;
          break;
        case WM_PRO_PLUS_AUDIO:
          trackInfoArray[i].codec = WMA_PRO_PLUS_CODEC;
          break;
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

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
        case NONMP4_MP3_AUDIO:
          trackInfoArray[i].codec = NONMP4_MP3_CODEC;
          break;        

        case QCP_AUDIO:
          trackInfoArray[i].codec = QCP_CODEC;
          break;
        case NONMP4_AAC_AUDIO:
          trackInfoArray[i].codec = NONMP4_AAC_CODEC;
          break;
        case NONMP4_AMR_AUDIO:
          trackInfoArray[i].codec = NONMP4_AMR_CODEC;
          break;
        case MIDI_AUDIO:
          trackInfoArray[i].codec = MIDI_CODEC;
          break;
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

#ifdef FEATURE_DIVX_311_ENABLE
        case DIVX311_VIDEO:
          trackInfoArray[i].codec =  DIVX311_CODEC;
          break;
#endif
#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /* FEATURE_QTV_AVI_AC3 */
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */
          
        default:
          trackInfoArray[i].codec = UNKNOWN_CODEC;
          break;
      }

      trackInfoArray[i].frameWidth = 
        pBaseFile->getTrackVideoFrameWidth(trackInfoArray[i].trackID);
        
      trackInfoArray[i].frameHeight =
        pBaseFile->getTrackVideoFrameHeight(trackInfoArray[i].trackID);

      trackInfoArray[i].isPlayable = Media::CanPlay(&trackInfoArray[i]);

      trackInfoArray[i].isTrackSelected = false;
    }

    QTV_Delete_Array(trackIDList);

    // Pass ownership of trackInfoArray to trackList.
    m_trackList = (ITrackList *)
      QTV_New_Args(TrackList, (trackInfoArray, numTracks));

    if (m_trackList == NULL)
    {
      QTV_Delete_Array(trackInfoArray);
      SetError();
      return false;
    }

    if (!SelectTracks(MakeInPointer(m_trackList), 
                     (ITrackSelectionPolicy *) 
                     MakeInPointer(m_defaultTrackSelectionPolicy)))
    {
      SetError();
      return false;
    }
  }
  else
  {
    SetError();
    return false;
  }

  // Create dummy track list with no tracks
  if (m_trackList == NULL)
  {
    m_trackList = QTV_New_Args(TrackList, ());
  }
  
  HandleSelectedTrackList();

  m_trackList.SaveToOutPointer(ppTrackList);    

  return true;
}

/* ======================================================================
FUNCTION
  FileMedia::isAudioPresentInClip()

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
bool FileMedia::isAudioPresentInClip()
{
  if ( pBaseFile )
  {
    return pBaseFile->isAudioPresentInClip();
  }
  return false;
}

#ifdef FEATURE_QTV_PDCF
/* ======================================================================
FUNCTION
  FileMedia::getEncryptionType()

DESCRIPTION
  Returns the type of encryption if any of the track is encrypted.

DEPENDENCIES
  None

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::EncryptionTypeT FileMedia::getEncryptionType()
{
  if ( pBaseFile )
  {
    return pBaseFile->getEncryptionType();
  }
  return QtvPlayer::ENCRYPT_NONE;
}

/* ======================================================================
FUNCTION
  FileMedia::getEncryptionType()

DESCRIPTION
  Returns the type of encryption for the track with the given track id.

DEPENDENCIES
  None

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::EncryptionTypeT FileMedia::getEncryptionType(uint32 track_id )
{
  if ( pBaseFile )
  {
    return pBaseFile->getEncryptionType(track_id);
  }
  return QtvPlayer::ENCRYPT_NONE;
}
#endif /* FEATURE_QTV_PDCF */

/* ======================================================================
FUNCTION
  FileMedia::GetTitle

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
bool FileMedia::GetTitle(OSCL_STRING &oTitle)
{
  oTitle = (char *)NULL;
  if ( pBaseFile )
  {
    oTitle = pBaseFile->getTitle();
    SetError();
  }
  if ( oTitle.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetAuthor

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
bool FileMedia::GetAuthor(OSCL_STRING &oAuthor)
{
  oAuthor = (char *)NULL;
  if ( pBaseFile )
  {
    oAuthor = pBaseFile->getAuthor();
    SetError();
  }

  if ( oAuthor.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetRating

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
bool FileMedia::GetRating(OSCL_STRING &oRating)
{
  oRating = (char *)NULL;
  if ( pBaseFile )
  {
    oRating = pBaseFile->getRating();
    SetError();
  }

  if ( oRating.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetCopyright

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
bool FileMedia::GetCopyright(OSCL_STRING &oCopyright)
{
  oCopyright = (char *)NULL;
  if ( pBaseFile )
  {
    oCopyright = pBaseFile->getCopyright();
    SetError();
  }

  if ( oCopyright.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetDescription

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
bool FileMedia::GetDescription(OSCL_STRING &oDescription)
{
  oDescription = (char *)NULL;
  if ( pBaseFile )
  {
    oDescription = pBaseFile->getDescription();
    SetError();
  }

  if ( oDescription.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetVersion

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
bool FileMedia::GetVersion(OSCL_STRING &oStr)
{
  oStr = (char *)NULL;
  if ( pBaseFile )
  {
    oStr = pBaseFile->getVersion();
    SetError();
  }

  if ( oStr.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetCreationDate

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
bool FileMedia::GetCreationDate(OSCL_STRING &oStr)
{
  oStr = (char *)NULL;
  if ( pBaseFile )
  {
    oStr = pBaseFile->getCreationDate();
    SetError();
  }

  if ( oStr.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetArtist

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
bool FileMedia::GetArtist(OSCL_STRING &oArtist)
{
  oArtist = (char *)NULL;
  if ( pBaseFile )
  {
    oArtist = pBaseFile->getPerf();
    SetError();
  }

  if ( oArtist.get_cstr() == NULL )
  return false;
  else
    return true;
}
/* ======================================================================
FUNCTION
  FileMedia::GetSTitle

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
bool FileMedia::GetSTitle(OSCL_STRING &oTitle)
{
  oTitle = NULL;
  return false;
}
/* ======================================================================
FUNCTION
  FileMedia::GetAlbum

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
bool FileMedia::GetAlbum(OSCL_STRING &oAlbum)
{
  oAlbum = (char *)NULL;
  if ( pBaseFile )
  {
    oAlbum = pBaseFile->getAlbum();
    SetError();
  }

  if ( oAlbum.get_cstr() == NULL )
  return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetYrrc

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
bool FileMedia::GetYrrc(OSCL_STRING &oYrrc)
{
  oYrrc = (char *)NULL;
  if ( pBaseFile )
  {
    oYrrc = pBaseFile->getYrrc();
    SetError();
  }

  if ( oYrrc.get_cstr() == NULL )
  return false;
  else
    return true;
}
/* ======================================================================
FUNCTION
  FileMedia::GetInfo

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
bool FileMedia::GetInfo(OSCL_STRING &oInfo)
{
  oInfo = NULL;
  return false;
}
/* ======================================================================
FUNCTION
  FileMedia::GetPerformance

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
bool FileMedia::GetPerf(OSCL_STRING &oPerformance)
{
 oPerformance = (char *)NULL;
  if ( pBaseFile )
  {
    oPerformance = pBaseFile->getPerf();
    SetError();
  }

  if ( oPerformance.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetGenre

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
bool FileMedia::GetGenre(OSCL_STRING &oGenre)
{
 oGenre = (char *)NULL;
  if ( pBaseFile )
  {
    oGenre = pBaseFile->getGenre();
    SetError();
  }

  if ( oGenre.get_cstr() == NULL )
    return false;
  else
	return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetClsf

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
bool FileMedia::GetClsf(OSCL_STRING &oClsf)
{
 oClsf = (char *)NULL;
  if ( pBaseFile )
  {
    oClsf = pBaseFile->getClsf();
    SetError();
  }

  if ( oClsf.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetKywd

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
bool FileMedia::GetKywd(OSCL_STRING &oKywd)
{
 oKywd = (char *)NULL;
  if ( pBaseFile )
  {
    oKywd = pBaseFile->getKywd();
    SetError();
  }

  if ( oKywd.get_cstr() == NULL )
    return false;
  else
    return true;
}
/* ======================================================================
FUNCTION
  FileMedia::GetLoci

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
bool FileMedia::GetLoci(OSCL_STRING &oLoci)
{
 oLoci= (char *)NULL;
  if ( pBaseFile )
  {
    oLoci = pBaseFile->getLoci();
    SetError();
  }

  if (oLoci.get_cstr() == NULL )
    return false;
  else
    return true;
}



#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#ifdef FEATURE_MP4_KDDI_META_DATA
/* ======================================================================
FUNCTION
  FileMedia::GetCopyProhibitionFlag

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
unsigned long FileMedia::GetCopyProhibitionFlag()
{
  unsigned long value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getCopyProhibitionFlag();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetValidityEffectiveDate

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
unsigned long FileMedia::GetValidityEffectiveDate()
{
  unsigned long value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getValidityEffectiveDate();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetValidityPeriod

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
unsigned long FileMedia::GetValidityPeriod()
{
  unsigned long value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getValidityPeriod();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetNumberOfAllowedPlayBacks

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
unsigned long FileMedia::GetNumberOfAllowedPlayBacks()
{
  unsigned long value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getNumberofAllowedPlayBacks();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetContentPropertyTitle

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
bool FileMedia::GetContentPropertyTitle(OSCL_STRING &oContentPropertyTitle)
{
  oContentPropertyTitle = (char *)NULL;
  if ( pBaseFile )
  {
    oContentPropertyTitle = pBaseFile->getContentPropertyTitle();
    SetError();
  }

  if ( oContentPropertyTitle.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetContentPropertyCopyRight

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
bool FileMedia::GetContentPropertyCopyRight(OSCL_STRING &oContentPropertyCopyright)
{
  oContentPropertyCopyright = (char *)NULL;
  if ( pBaseFile )
  {
    oContentPropertyCopyright = pBaseFile->getContentPropertyCopyRight();
    SetError();
  }

  if ( oContentPropertyCopyright.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION
  FileMedia::GetContentPropertyAuthor

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
bool FileMedia::GetContentPropertyAuthor(OSCL_STRING &oContentPropertyAuthor)
{
  oContentPropertyAuthor = (char *)NULL;
  if ( pBaseFile )
  {
    oContentPropertyAuthor = pBaseFile->getContentPropertyAuthor();
    SetError();
  }

  if ( oContentPropertyAuthor.get_cstr() == NULL )
    return false;
  else
    return true;

}

/* ======================================================================
FUNCTION
  FileMedia::GetContentPropertyMemo

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
bool FileMedia::GetContentPropertyMemo(OSCL_STRING &oContentPropertyMemo)
{
  oContentPropertyMemo = (char *)NULL;
  if ( pBaseFile )
  {
    oContentPropertyMemo = pBaseFile->getContentPropertyMemo();
    SetError();
  }

  if ( oContentPropertyMemo.get_cstr() == NULL )
    return false;
  else
    return true;

}

/* ======================================================================
FUNCTION
  FileMedia::GetAuthorDLLVersion

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
unsigned long FileMedia::GetAuthorDLLVersion()
{
  unsigned long value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getAuthorDLLVersion();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetEditFlags

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
unsigned long FileMedia::GetEditFlags()
{
  unsigned long value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getEditFlags();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetRecordingMode

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
unsigned int FileMedia::GetRecordingMode()
{
  unsigned int value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getRecordingMode();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetRecordingDate

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
unsigned long FileMedia::GetRecordingDate()
{
  unsigned long value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getRecordingDate();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetDeviceName

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
void FileMedia::GetDeviceName(uint8** oDeviceName)
{
  *oDeviceName = NULL;
  if ( pBaseFile )
  {
    *oDeviceName = pBaseFile->getDeviceName();
    SetError();
  }
}

/* ======================================================================
FUNCTION
  FileMedia::GetModelName

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
void FileMedia::GetModelName(uint8** oModelName)
{
  *oModelName = NULL;
  if ( pBaseFile )
  {
    *oModelName = pBaseFile->getModelName();
    SetError();
  }
}

/* ======================================================================
FUNCTION
  FileMedia::GetEncoderInformation

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
void FileMedia::GetEncoderInformation(uint8** oEncoderInformation)
{
  *oEncoderInformation = NULL;
  if ( pBaseFile )
  {
    *oEncoderInformation = pBaseFile->getEncoderInformation();
    SetError();
  }
}

/* ======================================================================
FUNCTION
  FileMedia::GetMuxInformation

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
void FileMedia::GetMuxInformation(uint8** oMuxInformation)
{
  *oMuxInformation = NULL;
  if ( pBaseFile )
  {
    *oMuxInformation = pBaseFile->getMuxInformation();
    SetError();
  }
}

/* ======================================================================
FUNCTION
  FileMedia::GetGPSByteOrder

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
uint16 FileMedia::GetGPSByteOrder()
{
  uint16 value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getGPSByteOrder();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::getGPSVersionID

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
uint32  FileMedia::getGPSVersionID()
{
  uint32 value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getGPSVersionID();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::getGPSLatitudeRef

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
uint32  FileMedia::getGPSLatitudeRef()
{
  uint32 value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getGPSLatitudeRef();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::getGPSLongitudeRef

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
uint32  FileMedia::getGPSLongitudeRef()
{
  uint32 value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getGPSLongitudeRef();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::getGPSAltitudeRef

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
uint32  FileMedia::getGPSAltitudeRef()
{
  uint32 value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getGPSAltitudeRef();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetGPSLatitudeArray

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
uint64 *FileMedia::GetGPSLatitudeArray()
{
  uint64 *value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getGPSLatitudeArray();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetGPSLongitudeArray

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
uint64 *FileMedia::GetGPSLongitudeArray()
{
  uint64 *value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getGPSLongitudeArray();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetGPSAltitude

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
uint64 FileMedia::GetGPSAltitude()
{
  uint64 value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getGPSAltitude();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetGPSTimeArray

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
uint64 *FileMedia::GetGPSTimeArray()
{
  uint64 *value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getGPSTimeArray();
    SetError();
  }
  return value;
}

/* ======================================================================
FUNCTION
  FileMedia::GetGPSSurveyData

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
bool FileMedia::GetGPSSurveyData(OSCL_STRING &oGPSSurveyData)
{
  oGPSSurveyData = (char *)NULL;
  if ( pBaseFile )
  {
    oGPSSurveyData = pBaseFile->getGPSSurveyData();
    SetError();
  }

  if ( oGPSSurveyData.get_cstr() == NULL )
    return false;
  else
    return true;

}

/* ======================================================================
FUNCTION
  FileMedia::GetPositioningMethod

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
bool FileMedia::GetPositioningMethod(OSCL_STRING &oGPSPositioningMethod)
{
  oGPSPositioningMethod = (char *)NULL;
  if ( pBaseFile )
  {
    oGPSPositioningMethod = pBaseFile->getPositoningMethod();
    SetError();
  }

  if ( oGPSPositioningMethod.get_cstr() == NULL )
    return false;
  else
    return true;

}

/* ======================================================================
FUNCTION
  FileMedia::GetPositioningName

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
bool FileMedia::GetPositioningName(OSCL_STRING &oGPSPositioningName)
{
  oGPSPositioningName = (char *)NULL;
  if ( pBaseFile )
  {
    oGPSPositioningName = pBaseFile->getPositioningName();
    SetError();
  }

  if ( oGPSPositioningName.get_cstr() == NULL )
    return false;
  else
    return true;

}

/* ======================================================================
FUNCTION
  FileMedia::GetGPSDate

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
bool FileMedia::GetGPSDate(OSCL_STRING &oGPSDate)
{
  oGPSDate = (char *)NULL;
  if ( pBaseFile )
  {
    oGPSDate = pBaseFile->getGPSDate();
    SetError();
  }

  if ( oGPSDate.get_cstr() == NULL )
    return false;
  else
    return true;

}

/* ======================================================================
FUNCTION
  FileMedia::GetGPSExtensionMapScaleInfo

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
uint64 FileMedia::GetGPSExtensionMapScaleInfo()
{
  uint64 value = 0;
  if ( pBaseFile )
  {
    value = pBaseFile->getGPSExtensionMapScaleInfo();
    SetError();
  }
  return value;
}
#endif
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

/* ======================================================================
FUNCTION
  FileMedia::GetDurationMsec

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
int FileMedia::GetDurationMsec()
{
  int ret = 0;
  if ( !pBaseFile )
  {
    return ret;
  }

  //get duration in msec.
  uint32 duration = pBaseFile->getMovieDuration();
  SetError();
  if ( nMp4Error == (int)EVERYTHING_FINE )
  {
    uint32 timescale = pBaseFile->getMovieTimescale();
    SetError();
    if ( nMp4Error== (int)EVERYTHING_FINE )
    {
      if ( timescale!=0 )
      {
        ret = (int)((float)duration * (float)TIMESCALE_BASE / (float)timescale);
      }
    }
  }

  /* if moov atom does not have duration, then get it from the tracks */
  if (ret == 0)
  {
    int audioDuration = 0;
    int videoDuration = 0;
    int textDuration = 0;
    int Duration = 0;
    if(numAudioTracks > 0)
    {
      if ( audioTimescale[0]!=0 )
      {
        //Get the duration of the base audio track (index 0).
        audioDuration = pBaseFile->getTrackMediaDuration(audioTrackIDs[0]);
        audioDuration = (int)ConvertAudioTimestamp(audioDuration);
      }
    }
    if(numVideoTracks > 0)
    {
      if ( videoTimescale[0]!=0 )
      {
        //Get the duration of the base video track (index 0).
        videoDuration = pBaseFile->getTrackMediaDuration(videoTrackIDs[0]);
        videoDuration = (int)ConvertVideoTimestamp(videoDuration,0);
      }
    }
    if(numTextTracks > 0)
    {
      if ( textTimescale[0]!=0 )
      {
        //Get the duration of the base text track (index 0).
        textDuration = pBaseFile->getTrackMediaDuration(textTrackIDs[0]);
        textDuration = (int)ConvertTextTimestamp(textDuration,0);
      }
    }
    /* get maximum of all the tracks as overall movie duration */
    Duration = MAX(audioDuration, videoDuration);
    Duration = MAX(Duration, textDuration);
    ret = Duration;
  }
  return ret;
}

/* ======================================================================
FUNCTION
  FileMedia::IsRepositioningAllowed

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
bool FileMedia::IsRepositioningAllowed()
{
  bool repos=false;
  if ( pBaseFile )
  {
    repos = !pBaseFile->randomAccessDenied();
    SetError();
  }
  return repos;
}

/*===========================================================================
FUNCTION:
  FileMedia::GetCurVideoSampleInfo

DESCRIPTION:
  Gets the sample information for the current video sample.

INPUT/OUPUT PARAMETERS:
  track_id - the video track ID
  timeStamp - the video timestamp of the current video sample in milliseconds
  timeDuration - the duration of the current video sample in milliseconds

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
===========================================================================*/
void FileMedia::GetCurVideoSampleInfo(uint32 idx, uint32 *timeStamp, uint32 *timeDuration)
{
  int returnCode;
  uint32 timescale;

  if ( pBaseFile )
  {
    file_sample_info_type SampleInfo;

    timescale  = pBaseFile->getTrackMediaTimescale(videoTrackIDs[idx]);
    returnCode = (int)pBaseFile->peekCurSample(videoTrackIDs[idx], &SampleInfo);
    if(returnCode == (int)EVERYTHING_FINE && timescale)
    {
      *timeStamp    = (uint32) ( ( (uint64)(SampleInfo.time) * 1000 ) / timescale ); 
      *timeDuration = (SampleInfo.delta*1000)/timescale;
    }
  }
}

/* ======================================================================
FUNCTION
  FileMedia::IsVideoOnlyPlaybackAllowed

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
bool FileMedia::IsVideoOnlyPlaybackAllowed()
{
  bool ans=false;
  if ( pBaseFile )
  {
    ans = pBaseFile->getAllowVideoOnly()!=0;
    SetError();
  }
  return ans;
}

/* ======================================================================
FUNCTION
  FileMedia::IsAudioOnlyPlaybackAllowed

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
bool FileMedia::IsAudioOnlyPlaybackAllowed()
{
  bool ans=false;
  if ( pBaseFile )
  {
    ans = pBaseFile->getAllowAudioOnly()!=0;
    SetError();
  }
  return ans;
}

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
/* ======================================================================
FUNCTION
  FileMedia::GetFileType

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
Media::MediaFileType FileMedia::GetFileType()
{
  //This function should be called after track selection.
  if ( pBaseFile == NULL )
  {
    return CONTENT_UNKNOWN;
  }

  bool hasAudio = false;
  bool hasVideo = false;
  bool hasStillImage = false;
  bool hasText = false;
  
  QTV_NULL_PTR_CHECK(m_trackList, CONTENT_UNKNOWN);

  for ( int32 i = 0; i < m_trackList->GetNumTracksAvailable(); i++ )
  {
    Media::CodecType codecType = m_trackList->GetCodecType(i);

    switch ( codecType )
    {
      case EVRC_CODEC:
      case QCELP_CODEC:
      case AAC_CODEC:
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
      case MP3_CODEC:
      case GSM_AMR_CODEC:
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
      case NONMP4_MP3_CODEC:      
      case NONMP4_AAC_CODEC:
      case NONMP4_AMR_CODEC:
      case QCP_CODEC:
      case MIDI_CODEC:
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
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
#endif /* FEATURE_QTV_AVI_AC3 */
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */
        hasAudio = true;
        break;
      case MPEG4_CODEC:
      case H263_CODEC:
#ifdef FEATURE_H264_DECODER
      case H264_CODEC:
#endif /* FEATURE_H264_DECODER */
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
        hasVideo = true;
        break;
      case STILL_IMAGE_CODEC:
      case STILL_IMAGE_H263_CODEC:
        hasStillImage = true;
        break;
      case TIMED_TEXT_CODEC:
        hasText = true;
        break;
      default:
        break;
    } // switch ( codecType )
  } // for all tracks

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  // Telop Text is a special case: it is not considered to be a "track"
  // in the sense that 3GPP text is.  Must check explicitly for it.
  if ( pBaseFile->IsTelopPresent() )
  {
    hasText = true;
  }
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */
 
  return GetFileTypeByComponents(hasAudio, hasVideo, hasStillImage, hasText);
}
/* ======================================================================
FUNCTION
  FileMedia::isGenericAudioFileInstance

DESCRIPTION
  Tells whether the created media instancce is geniric audio file instance or not

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool FileMedia::isGenericAudioFileInstance()
{
  if (pBaseFile)
  {
    return pBaseFile->isGenericAudioFileInstance();
  }
  return false;
}
/* ======================================================================
FUNCTION
  FileMedia::isAviFileInstance

DESCRIPTION
  Tells whether the created media instancce is avi file instance or not

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool FileMedia::isAviFileInstance()
{
  if (pBaseFile)
  {
    return pBaseFile->isAviFileInstance();
  }
  return false;
}
#ifdef FEATURE_QTV_DIVX_DRM
/* ======================================================================
FUNCTION
  FileMedia::CopyDRMContextInfo

DESCRIPTION
  DRM context info is copied from 'ptr' into pBaseFile.

DEPENDENCIES
  None

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void FileMedia::CopyDRMContextInfo(void* ptr)
{
  if (pBaseFile)
  {
    pBaseFile->CopyDRMContextInfo(ptr);
  }
}
/* ======================================================================
FUNCTION
  FileMedia::GetClipDrmInfo

DESCRIPTION
  Retrieve clip specific DRM information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================== */
void FileMedia::GetClipDrmInfo(void* ptr)
{
  if (pBaseFile)
  {
    pBaseFile->GetClipDrmInfo(ptr);
  }
}
/* ======================================================================
FUNCTION
  FileMedia::IsDRMProtection

DESCRIPTION
  Determines if the clip is DRM protected or not.

DEPENDENCIES
  None

RETURN VALUE
  True if the clip is DRM protected,else returns false;

SIDE EFFECTS
  None
========================================================================== */
bool FileMedia::IsDRMProtection()
{
  if (pBaseFile)
  {
    return pBaseFile->IsDRMProtection();
  }
  return false;
}
/* ======================================================================
FUNCTION
  FileMedia::CommitDivXPlayback

DESCRIPTION
  Commit the divx playback.

DEPENDENCIES
  None

RETURN VALUE
  True if the clip is DRM protected,else returns false;

SIDE EFFECTS
  None
========================================================================== */
bool FileMedia::CommitDivXPlayback()
{
  if (pBaseFile)
  {
    return pBaseFile->CommitDivXPlayback();
  }
  return false;
}
#endif

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
/* ======================================================================
FUNCTION
  FileMedia::IsTelopPresent

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
bool FileMedia::IsTelopPresent()
{
  if (pBaseFile)
  {
    return pBaseFile->IsTelopPresent();
  }
  return false;
}

/* ======================================================================
FUNCTION
  FileMedia::GetNextTelopElementPtr

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
TelopElement *FileMedia::GetNextTelopElementPtr()
{
  TelopElement *pTE = NULL;
  if (pBaseFile)
  {
    pTE = pBaseFile->getNextTelopElement();
    if(bLoopTrackFlag && !pTE)
    {
      QtvPlayer::PlayerStateRecordT ps;
      bool bError = false;

      (void)QtvPlayer::GetPlayerState(ps);
#ifndef FEATURE_WINCE
      ((TimedText *)TextPlayerDataPtr)->Notify(TimedText::ETTGetText, Common::CPVTEXT_RESTARTING_TRACK);
      ((TimedText *)TextPlayerDataPtr)->SetPlayTimes(0, -1);
#endif	  
      if(m_pMpeg4Player)
      {
        m_pMpeg4Player->m_mediaSync.ResetPlaybackOffset(AVSync::TextAV);
      }
      // reset text position
      (void)SetTextPosition(0,bError,ps.playbackMsec);
      pTE = pBaseFile->getNextTelopElement();
    }
  }
  return pTE;
}
/* ======================================================================
FUNCTION
  FileMedia::GetTelopHeader

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
TelopHeader *FileMedia::GetTelopHeader()
{
  if (pBaseFile)
  {
    return pBaseFile->getTelopHeader();
  }
  return NULL;
}
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

/* ======================================================================
FUNCTION
  FileMedia::GetMaxTextBufferSizeDB

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
unsigned long FileMedia::GetMaxTextBufferSizeDB(int idx)
{
  unsigned long n = 0;
  if ( pBaseFile )
  {
    n = (unsigned long)pBaseFile->getTrackMaxBufferSizeDB(textTrackIDs[idx]);
    SetError();
  }
  return n?n:2048;
}

#ifdef FEATURE_FILE_FRAGMENTATION
/* ======================================================================
FUNCTION
  FileMedia::setPausedVideo

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
void FileMedia::setPausedVideo( boolean bPauseVideo )
{
  if ( pBaseFile)
  {
    pBaseFile->setPausedVideo(bPauseVideo);
  }
}

/* ======================================================================
FUNCTION
  FileMedia::setPausedAudio

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
void FileMedia::setPausedAudio( boolean bPauseAudio )
{
  if ( pBaseFile)
  {
     pBaseFile->setPausedAudio(bPauseAudio);
  }
}

/* ======================================================================
FUNCTION
  FileMedia::setPausedText

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
void FileMedia::setPausedText( boolean bPauseText )
{
  if ( pBaseFile)
  {
    pBaseFile->setPausedText(bPauseText);
  }
}

/* ======================================================================
FUNCTION
  FileMedia::getPausedVideo

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
boolean FileMedia::getPausedVideo( void )
{
  if ( pBaseFile)
  {
    return pBaseFile->getPausedVideo();
  }
  return TRUE;
}

/* ======================================================================
FUNCTION
  FileMedia::getPausedAudio

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
boolean FileMedia::getPausedAudio( void )
{
  if ( pBaseFile)
  {
     return pBaseFile->getPausedAudio();
  }
  return TRUE;
}

/* ======================================================================
FUNCTION
  FileMedia::setPausedText

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
boolean FileMedia::getPausedText( void )
{
  if ( pBaseFile)
  {
    return pBaseFile->getPausedText();
  }
  return TRUE;
}

/* ======================================================================
FUNCTION
  FileMedia::resumeMedia

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
void FileMedia::resumeMedia( void )
{
  if ( pBaseFile)
  {
    pBaseFile->resumeMedia();
  }
}

/* ======================================================================
FUNCTION
  FileMedia::GetNextFragMaxTextBufferSizeDB

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
unsigned long FileMedia::GetNextFragMaxTextBufferSizeDB(int idx)
{
  unsigned long n = 0;
  if ( pBaseFile )
  {
    n = (unsigned long)pBaseFile->getNextFragTrackMaxBufferSizeDB(textTrackIDs[idx]);
    SetError();
  }
  return n?n:2048;
}
#endif /*FEATURE_FILE_FRAGMENTATION*/

/* ======================================================================
FUNCTION
  FileMedia::GetNextTextSample

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
uint32 FileMedia::GetNextTextSample(MediaStatus &status, int idx, unsigned char *buf, int size)
{
  uint32 index;
  int32 nBytes=0;
  status=DATA_ERROR;
#ifdef FEATURE_QTV_SKT_MOD
  unsigned char *pBuffer = buf;
#endif

  if ( !IsTextAvailable() )
  {
    status = DATA_UNDERRUN;
  }
  else if ( pBaseFile )
  {
#ifdef FEATURE_QTV_SKT_MOD
    int OTIType = pBaseFile->getTrackOTIType(textTrackIDs[idx]);
    if(OTIType == (int)SKT_MOD_TEXT)
    {
      /* we need insert size of text in network byte order
          as first two bytes to make it compatible with 3GPP timed text */
      pBuffer = buf+2;
    }
    nBytes = getNextMediaSample(textTrackIDs[idx], pBuffer, (uint32) size, index, TEXT_REQUEST);
#else
    nBytes = getNextMediaSample(textTrackIDs[idx], buf, (uint32) size, index, TEXT_REQUEST);
#endif /* FEATURE_QTV_SKT_MOD */

    SetError();
#ifdef FEATURE_FILE_FRAGMENTATION
    if ( nBytes == FRAGMENT_BOUNDARY )
    {
      /* this is NO-DATA fragment frame */
      nBytes = 0;
      status = DATA_FRAGMENT;
    }
    else if( nBytes == FRAGMENT_CORRUPT)
    {
      nBytes = 0;
      status = DATA_ERROR;
    }
    else
#endif //FEATURE_FILE_FRAGMENTATION
    if ( nBytes == INSUFFICIENT_BUFFER_SIZE)
    {
      /* current buffer size is small to hold the complete sample */
      nBytes = 0;
      status = DATA_REALLOCATE_BUFFER;
    }
    else if ( nBytes == INSUFFICIENT_DATA)
    {
      /* Data from databuffer/file is insufficient (data underrun) */
      nBytes = 0;
      status = DATA_INSUFFICIENT;
    }
    else if ( nBytes>0 )
    {
      status = DATA_OK;
#ifdef FEATURE_QTV_SKT_MOD
      if(OTIType == (int)SKT_MOD_TEXT)
      {
        /* insert first two bytes as size of text in network byte order
           to make it compatible with 3GPP timed text */
        buf[0] = (byte)((uint32)nBytes >> 8);
        buf[1] = (byte)(nBytes & 0x00ff);
        nBytes = nBytes + 2;
      }
#endif /* FEATURE_QTV_SKT_MOD */
    }
    else if ( nBytes==0 )
    {
      status = DATA_END;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of Text Trak");
    }
    else if( nBytes == READ_FAILURE)
    {
      nBytes = 0;
      status = DATA_ERROR;
    }
    else if(nBytes==ZERO_SAMPLE_SIZE)
    {
      /* this sample size is zero, but there are more samples */
      status = DATA_OK;
      nBytes = 0;
#ifdef FEATURE_QTV_SKT_MOD
      if(OTIType == (int)SKT_MOD_TEXT)
      {
        /* insert first two bytes as size of text in network byte order
           to make it compatible with 3GPP timed text */
        buf[0] = 0;
        buf[1] = 0;
        nBytes = 2;
      }
#endif /* FEATURE_QTV_SKT_MOD */
    }
  }
  return (uint32)nBytes;

}

/* ======================================================================
FUNCTION
  FileMedia::GetTimestampForCurrentTextSample

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
unsigned long FileMedia::GetTimestampForCurrentTextSample(unsigned long idx)
{
  unsigned long n=0;

  if ( pBaseFile )
  {
    n = pBaseFile->getMediaTimestampForCurrentSample(textTrackIDs[idx]);

    SetError();
    if ( nMp4Error==(int)EVERYTHING_FINE )
    {
      if(bLoopTrackFlag)
      {
          n += textLoopTrackTimeOffset;
      }
      nTimestampT=(long)n;
    }
  }
  else
  {
    n = (unsigned long)nTimestampT;
  }
  return n;
}

/* ======================================================================
FUNCTION
  FileMedia::GetCurTextSampleInfo

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
void FileMedia::GetCurTextSampleInfo(uint32 *timeStamp, uint32 *timeDuration, uint32 *sampleDescIndex)
{
  int returnCode;

  if ( pBaseFile )
  {
    file_sample_info_type SampleInfo;
    returnCode = (int)pBaseFile->peekCurSample(textTrackIDs[0], &SampleInfo);
    if(returnCode == (int)EVERYTHING_FINE)
    {
      *timeStamp          = SampleInfo.time;
      *timeDuration       = SampleInfo.delta;
      *sampleDescIndex    = SampleInfo.sample_desc_index;
    }
  }
}

/* ======================================================================
FUNCTION
  FileMedia::getTextSampleEntryAt

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
TextSampleEntry *FileMedia::getTextSampleEntryAt(uint32 trackId, uint32 index)
{
  if (pBaseFile)
  {
    return pBaseFile->getTextSampleEntryAt(trackId, index);
  }
  return NULL;
}

/* ======================================================================
FUNCTION
  FileMedia::HandleSelectedTrackList

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
void FileMedia::HandleSelectedTrackList()
{
  QTV_NULL_PTR_CHECK( pBaseFile, RETURN_VOID );

  QTV_NULL_PTR_CHECK( m_trackList, RETURN_VOID );

  if(lockTrackList == true)
    return;

  // Do any FileMedia specific members need to be reset? If so we should call
  // FileInitData() but that overwrites stuff set by the constructor.
  BaseInitData();

  for (int32 i = 0; i < m_trackList->GetNumTracksAvailable(); i++)
  {
    if (!m_trackList->IsTrackSelected(i))
    {
      continue;
    }

    Media::CodecType codecType = m_trackList->GetCodecType(i);    
    uint32 trackID = m_trackList->GetTrackID(i);

    switch ( codecType )
    {
      case EVRC_CODEC:
      case AAC_CODEC:
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
      case MP3_CODEC:
      case QCELP_CODEC:
      case GSM_AMR_CODEC:
  #ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
      case NONMP4_MP3_CODEC:     
      case NONMP4_AAC_CODEC:
      case NONMP4_AMR_CODEC:
      case QCP_CODEC:
      case MIDI_CODEC:
  #endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
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
#endif /* FEATURE_QTV_AVI_AC3 */
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */
        //Audio track
      if (numAudioTracks < COMMON_MAX_LAYERS )
      {
        audioTimescale[numAudioTracks]    = 
        pBaseFile->getTrackMediaTimescale((unsigned long) trackID);
        audioSamplingFreq[numAudioTracks] = 
        pBaseFile->getTrackAudioSamplingFreq((unsigned long) trackID);

        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,  
                      "Timescale: %d", audioTimescale[numAudioTracks]);

        audioTrackIDs[numAudioTracks] = (uint32) trackID;
        numAudioTracks++;
        audioCodecType = codecType;
      }

        switch ( codecType )
        {
          case EVRC_CODEC:
          case QCELP_CODEC:
          case MP3_CODEC:
    #ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
        case NONMP4_MP3_CODEC:       
        case NONMP4_AAC_CODEC:
        case NONMP4_AMR_CODEC:
        case QCP_CODEC:
        case MIDI_CODEC:
    #endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
        case GSM_AMR_CODEC:
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
#endif /* FEATURE_QTV_AVI_AC3 */
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */
          audioSamplesPerFrame = pBaseFile->getAudioSamplesPerFrame(
            (unsigned long) trackID);
          break;

        case AAC_CODEC:
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
        audioSamplesPerFrame = (int) aacSamplesPerFrame;
        break;

        default:
        break;
      }
      break;

      case MPEG4_CODEC:
      case STILL_IMAGE_CODEC:
      case STILL_IMAGE_H263_CODEC:
      case H263_CODEC:
  #ifdef FEATURE_H264_DECODER
      case H264_CODEC:
  #endif /* FEATURE_H264_DECODER */
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
        //Video track
        videoTimescale[numVideoTracks] = 
          pBaseFile->getTrackMediaTimescale((unsigned long) trackID);
        videoTrackIDs[numVideoTracks] = (uint32) trackID;
        numVideoTracks++;
        videoCodecType = codecType;
        break;

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    case TIMED_TEXT_CODEC:
      //Timed text track
      textTimescale[numTextTracks] = 
        pBaseFile->getTrackMediaTimescale((unsigned long) trackID);
      textTrackIDs[numTextTracks] = (uint32) trackID;
      numTextTracks++;
      textCodecType = codecType;
      break;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

    default:
      break;
    } // switch ( trackInfo->codec )
  } // for (int32 i = 0; i < totalTracks; i++)
}

/* ======================================================================
FUNCTION
  FileMedia::IsTextAvailable

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
bool FileMedia::IsTextAvailable(MediaStatus* status)
{
  //<tbd>: always return true for now
  if ( status != NULL )
  {
     *status = DATA_OK;
  }
  return true;
}

#else /* FEATURE_MP4_3GPP_TIMED_TEXT */
/* ======================================================================
FUNCTION
  FileMedia::GetFileType

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
Media::MediaFileType FileMedia::GetFileType()
{
  //This function should be called after track selection.
  bool hasAudio, hasVideo, hasStillImage;
  MediaFileType type = CONTENT_UNKNOWN;

  if ( pBaseFile )
  {
    hasAudio = hasVideo = hasStillImage = false;

    for ( int32 i = 0; i < m_trackList->GetNumTracksAvailable(); i++ )
    {
      switch ( trackInfo->codec )
      {
      case EVRC_CODEC:
      case QCELP_CODEC:
      case AAC_CODEC:
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
      case GSM_AMR_CODEC:
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
      case MP3_CODEC:
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
      case NONMP4_MP3_CODEC:            
      case NONMP4_AAC_CODEC:
      case NONMP4_AMR_CODEC:
      case QCP_CODEC:
      case MIDI_CODEC:
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /* FEATURE_QTV_AVI_AC3 */
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */
        hasAudio = true;
        break;
      case MPEG4_CODEC:
      case H263_CODEC:
        hasVideo = true;
        break;
      case STILL_IMAGE_CODEC:
      case STILL_IMAGE_H263_CODEC:
        hasStillImage = true;
        break;
      default:
        break;
      }
    }

    return GetFileType(hasAudio, hasVideo, hasStillImage);
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */


/* ======================================================================
FUNCTION
  FileMedia::GetMaxVideoBufferSizeDB

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
unsigned long FileMedia::GetMaxVideoBufferSizeDB(int idx)
{
  unsigned long n=0;
  if ( pBaseFile )
  {
    n = (unsigned long)pBaseFile->getTrackMaxBufferSizeDB(videoTrackIDs[idx]);
    SetError();
  }
  return n;
}

/* ======================================================================
FUNCTION
  FileMedia::GetMaxVideoFrameSize

DESCRIPTION
  Get the largest frame size in the given video track

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
unsigned long FileMedia::GetMaxVideoFrameSize(int idx)
{
  unsigned long n=0;
  if ( pBaseFile )
  {
    n = (unsigned long)pBaseFile->getLargestFrameSize(videoTrackIDs[idx]);
    SetError();
  }
  return n;
}

/* ======================================================================
FUNCTION
  FileMedia::GetMaxAudioFrameSize

DESCRIPTION
  Get the largest frame size in the given audio track

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
unsigned long FileMedia::GetMaxAudioFrameSize(int idx)
{
  unsigned long n=0;
  if ( pBaseFile )
  {
    n = (unsigned long)pBaseFile->getLargestFrameSize(audioTrackIDs[idx]);
    SetError();
  }
  return n;
}


/* ======================================================================
FUNCTION
  FileMedia::GetMaxTextFrameSize

DESCRIPTION
  Get the largest frame size in the given text track

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
unsigned long FileMedia::GetMaxTextFrameSize(int idx)
{
  unsigned long n=0;
  if ( pBaseFile )
  {
    n = (unsigned long)pBaseFile->getLargestFrameSize(textTrackIDs[idx]);
    SetError();
  }
  return n;
}


#ifdef FEATURE_FILE_FRAGMENTATION
/* ======================================================================
FUNCTION
  FileMedia::GetNextFragMaxVideoBufferSizeDB

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
unsigned long FileMedia::GetNextFragMaxVideoBufferSizeDB(int idx)
{
  unsigned long n=0;
  if ( pBaseFile )
  {
    n = (unsigned long)pBaseFile->getNextFragTrackMaxBufferSizeDB(videoTrackIDs[idx]);
    SetError();
  }
  return n;
}
#endif /*FEATURE_FILE_FRAGMENTATION*/

/* ======================================================================
FUNCTION
  FileMedia::GetMaxAudioBufferSizeDB

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  This function takes care of unsigned long overflow by taking
  MAX.

========================================================================== */
unsigned long FileMedia::GetMaxAudioBufferSizeDB(int idx)
{
  unsigned long n = 0;
  if ( pBaseFile )
  {
    n = (unsigned long)pBaseFile->getTrackMaxBufferSizeDB(audioTrackIDs[idx]);

    //We have to add the frame byte for EVRC and QCELP!
    if ( audioCodecType == EVRC_CODEC )
    {
      n = MAX( (n+EVRC_SAMPLE_HEADER_SIZE), n );
    }
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
    else if ( audioCodecType == AAC_CODEC )
    {
      n = MAX( (n+4), n ); /* to support 2 bytes sync marker and 2 byte size */
    }
#endif

#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
#if defined (FEATURE_QTV_3GPP_AMR_WB) || defined (FEATURE_QTV_3GPP_AMR_WB_PLUS)
#error code not present
#endif /* (FEATURE_QTV_3GPP_AMR_WB) || defined (FEATURE_QTV_3GPP_AMR_WB_PLUS) */

    // 1024 samples should not be larger than this size compressed
    // Windows media does have larger frames, so we don't need this code
    /*if ( n > 1024*2 )
      n = 1024*2;*/

    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,  "MaxAudioBufferSize: %d", n);

    SetError();
  }
  return n;
}

#ifdef FEATURE_FILE_FRAGMENTATION
/* ======================================================================
FUNCTION
  FileMedia::GetNextFragMaxAudioBufferSizeDB

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  This function takes care of unsigned long overflow by taking
  MAX.

========================================================================== */
unsigned long FileMedia::GetNextFragMaxAudioBufferSizeDB(int idx)
{
  unsigned long n=0;
  if ( pBaseFile )
  {
    n = (unsigned long)pBaseFile->getNextFragTrackMaxBufferSizeDB(audioTrackIDs[idx]);

    //We have to add the frame byte for EVRC and QCELP!
    if ( audioCodecType == EVRC_CODEC )
    {
      n = MAX( (n+EVRC_SAMPLE_HEADER_SIZE), n );
    }
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
    else if ( audioCodecType == AAC_CODEC )
    {
      n = MAX( (n+4), n ); /* to support 2 bytes sync marker and 2 byte size */
    }
#endif
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */

    // 1024 samples should not be larger than this size compressed
    if ( n > 1024*2 )
      n = 1024*2;

    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,  "MaxAudioBufferSize: %d", n);

    SetError();
  }
  return n;
}
#endif /*FEATURE_FILE_FRAGMENTATION*/

/* ======================================================================
FUNCTION
  FileMedia::GetAudioDurationMsec

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
int FileMedia::GetAudioDurationMsec()
{
  int n = 0;
  if ( pBaseFile && (numAudioTracks > 0) )
  {
    if ( audioTimescale[0]!=0 )
    {
      //Get the duration of the base audio track (index 0).
      n = (int)pBaseFile->getTrackMediaDuration((unsigned long)audioTrackIDs[0]);
      SetError();
      n = (int)ConvertAudioTimestamp((unsigned long)n);
    }
  }
  return n;
}


/* ======================================================================
FUNCTION
  FileMedia::GetNextLayeredVideoSample

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
int FileMedia::GetNextLayeredVideoSample(MediaStatus &status, int idx,
                                         unsigned char * buf, int size,
                                         uint32 /*unused*/, void ** /*unused*/)
{
  uint32 index;
  int nBytes=0;

  status=DATA_ERROR;

  if ( !IsVideoAvailable() )
  {
    status = DATA_UNDERRUN;
  }
  else if ( pBaseFile )
  {
    nBytes = getNextMediaSample(videoTrackIDs[idx], buf, (uint32) size, index, VIDEO_REQUEST);

    SetError();

#ifdef FEATURE_FILE_FRAGMENTATION
    if ( nBytes == FRAGMENT_BOUNDARY)
    {
      /* this is NO-DATA fragment frame */
      nBytes = 0;
      status = DATA_FRAGMENT;
    }
    else if( nBytes == FRAGMENT_CORRUPT)
    {
      nBytes = 0;
      status = DATA_ERROR;
    }
    else
#endif //FEATURE_FILE_FRAGMENTATION
    if (nBytes == INSUFFICIENT_BUFFER_SIZE )
    {
      /* current buffer size is small to hold the complete sample */
      nBytes = 0;
      status = DATA_REALLOCATE_BUFFER;
    }
    else if ( nBytes == INSUFFICIENT_DATA)
    {
      /* Data from databuffer/file is insufficient (data underrun) */
      nBytes = 0;
      status = DATA_INSUFFICIENT;
    }
    else if ( nBytes==0 )
    {
      /* In case bLoopTrackFlag is set there is no concept of DATA_END. 
       * So we need to send status as DATA_OK and nBytes as 0 in case 
       * we reach end of file. 
       * So that in case of H264 or MP4 with mutiple vol headers the 
       * reading sequence will be consistent while playing second time.
       * Other wise it will fail for H264 and MP4 with mutiple vol headers
       */
      if(!bLoopTrackFlag)
      {
      status = DATA_END;
      }
      else
      {
        status = DATA_OK;
      }
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of Video Trak");
    }
    else if(( nBytes == READ_FAILURE) 
#ifdef FEATURE_QTV_PDCF
            || ( nBytes == FAILED_TO_DECRYPT)
#endif
            )
    {
      nBytes = 0;
      status = DATA_ERROR;
    }
    else if(nBytes==ZERO_SAMPLE_SIZE)
    {
      /* this sample size is zero, but there may be more samples */
      status = DATA_OK;
      nBytes = 0;
    }
    else if ( nBytes>0 )
    {
      status = DATA_OK;
    }
  }
  return nBytes;
}

/* ======================================================================
FUNCTION
  FileMedia::resetInitialization

DESCRIPTION
  Resets m_bMediaInitialized to indicate that media is not initialized.
  Currently, only used for generic audio formats.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

========================================================================== */
void FileMedia::resetInitialization()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
               "FileMedia::resetInitialization");  
  if(pBaseFile)
  {
    pBaseFile->resetInitialization();
  }
  else
  {
    m_bMediaInitialized = false;
  } 
}

/* ======================================================================
FUNCTION
  FileMedia::ResetLayeredVideoPlayback

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
bool FileMedia::ResetLayeredVideoPlayback(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->resetMediaPlayback(videoTrackIDs[idx]);
  }
  return false;
}

/* ======================================================================
FUNCTION
  FileMedia::GetTimestampForCurrentLayeredVideoSample

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
unsigned long FileMedia::GetTimestampForCurrentLayeredVideoSample(int idx)
{
  long n=0;
  if ( pBaseFile )
  {
    n = (long)pBaseFile->getMediaTimestampForCurrentSample(videoTrackIDs[idx]);

    SetError();
    if ( nMp4Error==(int)EVERYTHING_FINE )
    {
      if(bLoopTrackFlag)
      {
          //Effective only for video tracks. Offet the presentation time of each video frame with ts value
          //of the last video frame of the last playback loop.
          n += videoLoopTrackTimeOffset;
      }
      nTimestampV=n;
    }
  }
  return (unsigned long)n;
}

/* ======================================================================
FUNCTION
  FileMedia::GetTimestampDeltaForCurrentAudioSample

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
unsigned long FileMedia::GetTimestampDeltaForCurrentAudioSample(int idx)
{
  long n=0;
  if ( pBaseFile )
  {
    n = (long)pBaseFile->getMediaTimestampDeltaForCurrentSample(audioTrackIDs[idx]);

    SetError();
  }
  return (unsigned long)n;
}

/* ======================================================================
FUNCTION
FileMedia::GetTimestampDeltaForCurrentLayeredVideoSample

DESCRIPTION
Gets the current time stamp delta for the input layer/track.

DEPENDENCIES
  None

INPUT  
  layer

RETURN VALUE
  The timestamp delta value for the current sample. Default to 0.

SIDE EFFECTS
  None.
========================================================================== */
unsigned long FileMedia::GetTimestampDeltaForCurrentLayeredVideoSample(int layer)
{
  long n=0;
  if ( pBaseFile )
  {
    n = (long)pBaseFile->getMediaTimestampDeltaForCurrentSample(videoTrackIDs[layer]);
    QTV_MSG_PRIO2( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR, "Timestamp delta @filemedia: %d, layer=%d", n,layer);
  }
  return (unsigned long)n;
}

/* ======================================================================
FUNCTION
  FileMedia::GetVOLHeader

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
unsigned char *FileMedia::GetVOLHeader(int idx, int &size)
{
  uint8 *volHdr = NULL;
  size = 0;

  if ( pBaseFile )
  {
    volHdr = pBaseFile->getTrackDecoderSpecificInfoContent(videoTrackIDs[idx]);
    size = (int)pBaseFile->getTrackDecoderSpecificInfoSize(videoTrackIDs[idx]);
    SetError();
  }

  return volHdr;
}

/* ======================================================================
FUNCTION
  FileMedia::GetNextAudioSample

DESCRIPTION
  Thorough, meaningful description of what this function does.

  WZeng, add "int channel" to support aac silence insertion when streaming,
  not used here, 06/05/02

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int FileMedia::GetNextAudioSample(MediaStatus &status, unsigned char * buf, int size, int *numSample, unsigned long idx, int)
{
  int32 nBytes=0;
  uint32 index;
  status=DATA_ERROR;
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
  if ( numSample ) *numSample=1;
  // NULL_AUDIO is a EVRC BLANK sample, it should play nothing (1 byte).
  static const char NULL_AUDIO = 0x00;

  if ( !IsAudioAvailable() )
  {
    status = DATA_UNDERRUN;
  }
  else if ( pBaseFile )
  {
    switch ( audioCodecType )
    {
    case EVRC_CODEC:
      {
        uint8 evrcType = pBaseFile->getTrackAudioFormat(audioTrackIDs[idx]);
        if ( evrcType == (uint8)VIDEO_FMT_STREAM_AUDIO_EVRC_PV )
        {
          //Use buf+1 because the frame type is NOT prepended to the sample data.
          nBytes = getNextMediaSample( audioTrackIDs[idx],
                                       (buf + EVRC_SAMPLE_HEADER_SIZE),
                                       (uint32) size - EVRC_SAMPLE_HEADER_SIZE, index, AUDIO_REQUEST );
          SetError();
#ifdef FEATURE_FILE_FRAGMENTATION
          if ( nBytes == FRAGMENT_BOUNDARY )
          {
            /* this is NO-DATA fragment frame */
            nBytes = 0;
            status = DATA_FRAGMENT;
          }
          else if( nBytes == FRAGMENT_CORRUPT)
          {
             nBytes = 0;
             status = DATA_ERROR;
          }
          else
#endif /*FEATURE_FILE_FRAGMENTATION*/
          if ( nBytes == INSUFFICIENT_BUFFER_SIZE)
          {
            /* current buffer size is small to hold the complete sample */
            nBytes = 0;
            status = DATA_REALLOCATE_BUFFER;
          }
          else if ( nBytes == INSUFFICIENT_DATA)
          {
            /* Data from databuffer/file is insufficient (data underrun) */
            nBytes = 0;
            status = DATA_INSUFFICIENT;
          }
          else if ( nBytes == ZERO_SAMPLE_SIZE)
          {
            /* it is a valid zero size sample */
            nBytes = 0;
            status = DATA_OK;
          }
          else if ( nBytes==0 )
          {
            status = DATA_END;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of Audio (EVRC) Trak");
          }
          else if(( nBytes == READ_FAILURE) 
#ifdef FEATURE_QTV_PDCF
            || ( nBytes == FAILED_TO_DECRYPT)
#endif
            )
          {
            nBytes = 0;
            status = DATA_ERROR;
          }
          else if ( nBytes>0 )
          {
            //If SDI has changed, then frame type has changed!
            if ( index != (uint32)curAudioSDI )
            {
              video_fmt_dec_specific_info_type *decInfo;

              //Get the frame type byte.
              decInfo =
              (video_fmt_dec_specific_info_type *)pBaseFile->getTrackDecoderSpecificInfoAtSDI(audioTrackIDs[idx], index);

              if ( decInfo )
              {
                evrcFrameType = decInfo->info[5];
              }

              //Save the current SDI
              curAudioSDI = (long)index;
            }

            //Prepend the evrc frame type byte.
            if ( evrcFrameType )
            {
              *buf = evrcFrameType;
              nBytes++;
              status = DATA_OK;
            }
          }
        }
        else /* evrcType == VIDEO_FMT_STREAM_AUDIO_EVRC this is 3gpp type track */
        {
          /* Frame type is prepended to the sample data. */
          nBytes = getNextMediaSample(audioTrackIDs[idx], buf, (uint32)size, index, AUDIO_REQUEST);
          SetError();

#ifdef FEATURE_FILE_FRAGMENTATION
          if ( nBytes == FRAGMENT_BOUNDARY )
          {
            /* this is NO-DATA fragment frame */
            nBytes = 0;
            status = DATA_FRAGMENT;
          }
          if( nBytes == FRAGMENT_CORRUPT)
          {
            nBytes = 0;
            status = DATA_ERROR;
          }
          else
#endif /*FEATURE_FILE_FRAGMENTATION*/
          if ( nBytes == INSUFFICIENT_BUFFER_SIZE)
          {
            /* current buffer size is small to hold the complete sample */
            nBytes = 0;
            status = DATA_REALLOCATE_BUFFER;
          }
          else if ( nBytes == INSUFFICIENT_DATA)
          {
            /* Data from databuffer/file is insufficient (data underrun) */
            nBytes = 0;
            status = DATA_INSUFFICIENT;
          }
          else if ( nBytes == ZERO_SAMPLE_SIZE)
          {
            /* it is a valid zero size sample */
            nBytes = 0;
            status = DATA_OK;
          }
          else if ( nBytes==0 )
          {
            status = DATA_END;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of Audio (EVRC) Trak");
          }
          else if(( nBytes == READ_FAILURE) 
#ifdef FEATURE_QTV_PDCF
            || ( nBytes == FAILED_TO_DECRYPT)
#endif
            )
          {
            nBytes = 0;
            status = DATA_ERROR;
          }
          else if ( nBytes>0 )
          {
            status = DATA_OK;
            if((0x01 != buf[0]) && (0x02 != buf[0]) &&
             (0x03 != buf[0]) && (0x04 != buf[0]) &&
             (0x0E != buf[0]))
             {
#ifndef FEATURE_WINCE
                //return BLANK frame
                QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                "Corrupted EVRC frame with size %d,Data:%d-->Replacing with BLANK frame",
                     size,buf[0]);
#endif
                     memcpy(buf, &NULL_AUDIO, sizeof(NULL_AUDIO));
                nBytes = 1;
             }
          }
        }
      }

      if ( numSample ) *numSample=1;
      break;

    case QCELP_CODEC:
      //Use buf+1 because the frame type IS prepended to the sample data.
      nBytes = getNextMediaSample(audioTrackIDs[idx],
                                  // (buf + QCELP_SAMPLE_HEADER_SIZE),
                                  buf,
                                  (uint32) size,
                                  index, AUDIO_REQUEST);
      SetError();

#ifdef FEATURE_FILE_FRAGMENTATION
      if ( nBytes == FRAGMENT_BOUNDARY )
      {
        /* this is NO-DATA fragment frame */
        nBytes = 0;
        status = DATA_FRAGMENT;
      }
      if( nBytes == FRAGMENT_CORRUPT)
      {
        nBytes = 0;
        status = DATA_ERROR;
      }
      else
#endif /*FEATURE_FILE_FRAGMENTATION*/
      if ( nBytes == INSUFFICIENT_BUFFER_SIZE)
      {
        /* current buffer size is small to hold the complete sample */
        nBytes = 0;
        status = DATA_REALLOCATE_BUFFER;
      }
      else if ( nBytes == INSUFFICIENT_DATA)
      {
        /* Data from databuffer/file is insufficient (data underrun) */
        nBytes = 0;
        status = DATA_INSUFFICIENT;
      }
      else if ( nBytes == ZERO_SAMPLE_SIZE)
      {
        /* it is a valid zero size sample */
        nBytes = 0;
        status = DATA_OK;
      }
      else if ( nBytes==0 )
      {
        status = DATA_END;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of Audio (QCELP) Trak");
      }
      else if(( nBytes == READ_FAILURE) 
#ifdef FEATURE_QTV_PDCF
            || ( nBytes == FAILED_TO_DECRYPT)
#endif
            )
      {
        nBytes = 0;
        status = DATA_ERROR;
      }
      else if ( nBytes>0 )
      {
        status = DATA_OK;
      }
      if ( numSample ) *numSample=1;
      break;

    case AAC_CODEC:
    case MP3_CODEC:
      //WZeng, 05/20/02
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
      if ( aacFrameMarkerNeeded )
      {
        nBytes = getNextMediaSample(  audioTrackIDs[idx],
                                      buf+4,
                                      (uint32)size-4,
                                      index, AUDIO_REQUEST);
        SetError();
         /**************************/
        /* writing one extra byte, so that DSP can get last frame
           even if it ends at odd boundary since DSP only takes data
           2 bytes at a time */
        if ( (nBytes==0) && !bAACPaddingSent)
        {
          *(buf+4) = 0;
          nBytes = 1;
          bAACPaddingSent = TRUE;
        }
        /**************************/
        if ( nBytes==0 )
        {
          status = DATA_END;
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of Audio (AAC/MP3) Trak");
        }
        else if(( nBytes == READ_FAILURE) 
#ifdef FEATURE_QTV_PDCF
            || ( nBytes == FAILED_TO_DECRYPT)
#endif
            )
        {
           nBytes = 0;
           status = DATA_ERROR;
        }
#ifdef FEATURE_FILE_FRAGMENTATION
        else if ( nBytes== FRAGMENT_BOUNDARY )
        {
          /* this is NO-DATA fragment frame */
          nBytes = 0;
          status = DATA_FRAGMENT;
        }
        else if( nBytes == FRAGMENT_CORRUPT)
        {
          nBytes = 0;
          status = DATA_ERROR;
        }
#endif/*FEATURE_FILE_FRAGMENTATION*/
        else if( nBytes == INSUFFICIENT_BUFFER_SIZE)
        {
          /* current buffer size is small to hold the complete sample */
          nBytes = 0;
          status = DATA_REALLOCATE_BUFFER;
        }
        else if ( nBytes == ZERO_SAMPLE_SIZE)
        {
          /* it is a valid zero size sample */
          nBytes = 0;
          status = DATA_OK;
        }
        else if ( nBytes == INSUFFICIENT_DATA)
        {
          /* Data from databuffer/file is insufficient (data underrun) */
          nBytes = 0;
          status = DATA_INSUFFICIENT;
        }
        else if ( nBytes>0 )
        {
          buf[0]=0xff;
          buf[1]=0xff;
          buf[2]= (byte)((nBytes & 0xff00)>>8);
          buf[3]= (byte)(nBytes & 0x00ff);
          nBytes = nBytes+4;
          status = DATA_OK;
        }
      }
      else
#endif /* FEATURE_MP4_AAC_FRAME_MARKER */
      {
        nBytes = getNextMediaSample(  audioTrackIDs[idx],
                                      buf,
                                      (uint32)size,
                                      index, AUDIO_REQUEST);
        SetError();
        if ( nBytes==0 )
        {
          status = DATA_END;
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of Audio (AAC/MP3) Trak");
        }
        else if(( nBytes == READ_FAILURE) 
#ifdef FEATURE_QTV_PDCF
            || ( nBytes == FAILED_TO_DECRYPT)
#endif
            )
        {
          nBytes = 0;
          status = DATA_ERROR;
        }
#ifdef FEATURE_FILE_FRAGMENTATION
        else if ( nBytes== FRAGMENT_BOUNDARY )
        {
          /* this is NO-DATA fragment frame */
          nBytes = 0;
          status = DATA_FRAGMENT;
        }
        else if( nBytes == FRAGMENT_CORRUPT)
        {
          nBytes = 0;
          status = DATA_ERROR;
        }
#endif //FEATURE_FILE_FRAGMENTATION
        else if ( nBytes== INSUFFICIENT_BUFFER_SIZE )
        {
          /* current buffer size is small to hold the complete sample */
          nBytes = 0;
          status = DATA_REALLOCATE_BUFFER;
        }
        else if ( nBytes == ZERO_SAMPLE_SIZE)
        {
          /* it is a valid zero size sample */
          nBytes = 0;
          status = DATA_OK;
        }
        else if ( nBytes == INSUFFICIENT_DATA)
        {
          /* Data from databuffer/file is insufficient (data underrun) */
          nBytes = 0;
          status = DATA_INSUFFICIENT;
        }
        else if ( nBytes>0 )
        {
          status = DATA_OK;
        }
      }
      break;
      //end, WZeng, 05/20/02
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */

    case GSM_AMR_CODEC: // jg amr addition
#ifdef FEATURE_MP4_AMR
      nBytes = getNextMediaSample(audioTrackIDs[idx],
                                  buf,
                                  (uint32)size,
                                  index, AUDIO_REQUEST);
      SetError();
      if ( nBytes==0 )
      {
        status = DATA_END;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of Audio (AMR) Trak");
      }
      else if(( nBytes == READ_FAILURE) 
#ifdef FEATURE_QTV_PDCF
            || ( nBytes == FAILED_TO_DECRYPT)
#endif
            )
      {
        nBytes = 0;
        status = DATA_ERROR;
      }
      else if ( nBytes == ZERO_SAMPLE_SIZE)
      {
        /* it is a valid zero size sample */
        nBytes = 0;
        status = DATA_OK;
      }
#ifdef FEATURE_FILE_FRAGMENTATION
      else if ( nBytes == FRAGMENT_BOUNDARY )
      {
        /* this is NO-DATA fragment frame */
        nBytes = 0;
        status = DATA_FRAGMENT;
      }
      else if( nBytes == FRAGMENT_CORRUPT)
      {
        nBytes = 0;
        status = DATA_ERROR;
      }
#endif //FEATURE_FILE_FRAGMENTATION
      else if ( nBytes == INSUFFICIENT_BUFFER_SIZE )
      {
        /* current buffer size is small to hold the complete sample */
        nBytes = 0;
        status = DATA_REALLOCATE_BUFFER;
      }
      else if ( nBytes == INSUFFICIENT_DATA)
      {
        /* Data from databuffer/file is insufficient (data underrun) */
        nBytes = 0;
        status = DATA_INSUFFICIENT;
      }
      else if ( nBytes>0 )
      {
        status = DATA_OK;
      }
      break;
#endif // Fall through if not defined

#if defined (FEATURE_QTV_WINDOWS_MEDIA) || defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case WMA_CODEC:
#endif  /* FEATURE_QTV_WINDOWS_MEDIA */
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
    case WMA_PRO_CODEC:
    case WMA_PRO_PLUS_CODEC:
#endif  /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */
      nBytes = getNextMediaSample(audioTrackIDs[idx],
                                  buf,
                                  (uint32) size,
                                  index, AUDIO_REQUEST);
      SetError();
      if ( nBytes==0 )
      {
        status = DATA_END;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of Audio (WMA) Trak");
      }
      else if(( nBytes == READ_FAILURE) 
#ifdef FEATURE_QTV_PDCF
            || ( nBytes == FAILED_TO_DECRYPT)
#endif
            )
      {
        nBytes = 0;
        status = DATA_ERROR;
      }
      else if ( nBytes == INSUFFICIENT_BUFFER_SIZE )
      {
        /* current buffer size is small to hold the complete sample */
        nBytes = 0;
        status = DATA_REALLOCATE_BUFFER;
      }
      else if ( nBytes == INSUFFICIENT_DATA)
      {
        /* Data from databuffer/file is insufficient (data underrun) */
        nBytes = 0;
        status = DATA_INSUFFICIENT;
      }
      else if ( nBytes == ZERO_SAMPLE_SIZE)
      {
        /* it is a valid zero size sample */
        nBytes = 0;
        status = DATA_OK;
      }
      else if ( nBytes>0 )
      {
        if ( numSample )
        {
          file_sample_info_type SampleInfo;
          int returnCode = pBaseFile->peekCurSample(audioTrackIDs[idx], &SampleInfo);
          if(returnCode == (int)EVERYTHING_FINE)
          {
            *numSample = SampleInfo.num_frames;
          }
        }
        status = DATA_OK;
      }
      break;
#endif /* (FEATURE_QTV_WINDOWS_MEDIA) || (FEATURE_QTV_WMA_PRO_DECODER) || (FEATURE_QTV_WMA_PRO_DSP_DECODER) */
#if defined (FEATURE_QTV_3GPP_AMR_WB) || defined (FEATURE_QTV_3GPP_AMR_WB_PLUS)
#error code not present
#endif  /*FEATURE_QTV_3GPP_AMR_WB  || FEATURE_QTV_3GPP_AMR_WB_PLUS */

#if defined (FEATURE_QTV_3GPP_EVRC_NB) || defined (FEATURE_QTV_3GPP_EVRC_WB)
#error code not present
#endif /* (FEATURE_QTV_3GPP_EVRC_NB) || defined (FEATURE_QTV_3GPP_EVRC_WB) */

#if defined (FEATURE_QTV_AVI_AC3) || defined (FEATURE_QTV_PCM) 
#error code not present
#endif /* (FEATURE_QTV_AVI_AC3) || defined (FEATURE_QTV_PCM) */

    default:
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio codec(%d) not supported in GetNextAudioSample",
                audioCodecType );
      break;
    }
  }

  return nBytes;
}

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
/* ======================================================================
FUNCTION
  FileMedia::GetNextGenericAudioSample

DESCRIPTION
  Thorough, meaningful description of what this function does.

  WZeng, add "int channel" to support aac silence insertion when streaming,
  not used here, 06/05/02

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int FileMedia::GetNextGenericAudioSample(MediaStatus &status, unsigned char * buf, int size, uint32 offset, int *numSample, unsigned long idx, int)
{
  int32 nBytes=0;
  uint32 index;
  status=DATA_ERROR;
  if ( numSample ) *numSample=1;
  
  if ( !IsAudioAvailable() )
  {
    status = DATA_UNDERRUN;
  }
  else if ( pBaseFile )
  {
    switch ( audioCodecType )
    {
    case NONMP4_MP3_CODEC:
    case NONMP4_AAC_CODEC:
    case NONMP4_AMR_CODEC:
    case QCP_CODEC:
    case MIDI_CODEC:
      nBytes = getNextGenericAudioMediaSample( audioTrackIDs[idx],
		                                       buf, (uint32)size,
                                               offset, index, AUDIO_REQUEST);
      SetError();
      if ( nBytes==0 )
      {
        status = DATA_END;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of Audio (AAC/MP3) Trak");
      }
      else if(( nBytes == READ_FAILURE) 
#ifdef FEATURE_QTV_PDCF
            || ( nBytes == FAILED_TO_DECRYPT)
#endif
            )
      {
        nBytes = 0;
        status = DATA_ERROR;
      }
      else if ( nBytes == INSUFFICIENT_BUFFER_SIZE )
      {
        /* current buffer size is small to hold the complete sample */
        nBytes = 0;
        status = DATA_REALLOCATE_BUFFER;
      }
      else if ( nBytes == ZERO_SAMPLE_SIZE)
      {
        /* it is a valid zero size sample */
        nBytes = 0;
        status = DATA_OK;
      }
      else if ( nBytes == INSUFFICIENT_DATA)
      {
        /* Data from databuffer/file is insufficient (data underrun) */
        nBytes = 0;
        status = DATA_INSUFFICIENT;
      }
      else if ( nBytes>0 )
      {
        status = DATA_OK;
      }      
      break;
    default:
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio codec(%d) not supported in GetNextAudioSample",
                audioCodecType );
      break;
    }
  }

  return nBytes;
}
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

/* ======================================================================
FUNCTION
  FileMedia::GetTimestampForCurrentAudioSample

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
unsigned long FileMedia::GetTimestampForCurrentAudioSample(unsigned long idx)
{
  unsigned long n=0;
  // if (pBaseFile && audioCodecType == EVRC_CODEC)
  // MAL 06-16-2002
  if ( pBaseFile )
  {
    n = pBaseFile->getMediaTimestampForCurrentSample(audioTrackIDs[idx]);

    SetError();
    if ( nMp4Error==(int)EVERYTHING_FINE )
    {
     if(bLoopTrackFlag)
      {
          n += audioLoopTrackTimeOffset;
      }
      nTimestampA=n;
    }
  }
  else
  {
    n = (unsigned long)nTimestampA;
  }
  return n;
}

/* ======================================================================
FUNCTION
  FileMedia::GetAACHeader

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
unsigned char *FileMedia::GetAACHeader(int idx, int &size)
{
  uint8 *volHdr = NULL;
  size = 0;

#ifdef FEATURE_QTV_BSAC
#error code not present
#else
  if ( pBaseFile && (audioCodecType == AAC_CODEC) )
#endif /* FEATURE_QTV_BSAC */
  {
    volHdr = pBaseFile->getTrackDecoderSpecificInfoContent(audioTrackIDs[idx]);
    size = (int)pBaseFile->getTrackDecoderSpecificInfoSize(audioTrackIDs[idx]);
    SetError();
  }

  return volHdr;
}

/* ======================================================================
FUNCTION
  FileMedia::SetError

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
void FileMedia::SetError()
{
  if ( pBaseFile )
  {
    nMp4Error = pBaseFile->FileSuccess() ? (int)EVERYTHING_FINE : pBaseFile->GetFileError();
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,  "FM SetError %d", 0);
    nMp4Error = (int)DEFAULT_ERROR;
  }
}

/* ======================================================================
FUNCTION
  FileMedia::CheckError

DESCRIPTION
  check for mp4 errors from the last library call.
  return true if error detected.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool FileMedia::CheckError(const Common::Mp4ErrorContext context)
{
  if ( nMp4Error!=(int)EVERYTHING_FINE )
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "File MP4 Error %d context %d",nMp4Error,context);
    return true;
  }
  return false;
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
bool FileMedia::setPDS2DloadObj( PVDL *pPDS2Obj)
{
  pPVDL =  pPDS2Obj;
  return true;
}

void FileMedia::setVideoPos(uint32 vPos)
{
  videoPos = vPos;
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Video Position set to %d", videoPos);
}
void FileMedia::setAudioPos(uint32 aPos)
{
  audioPos = aPos;
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Audio Position set to %d", audioPos);
}

#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

#undef REBUFFER_TEST
//#define REBUFFER_TEST

/* ======================================================================
FUNCTION
  FileMedia::SetVideoPosition

DESCRIPTION
  Sets the video position to the closest sync sample to the requested position
  among all video tracks.  If there are no sync samples after/before the
  requested position, then the video position remains the current one.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
uint32 FileMedia::SetVideoPosition(unsigned long pos, bool &bError, uint32 currentPosTimeStamp)
{
  uint32 syncPos = 0;
  bError = FALSE;

  if ( pBaseFile )
  {    
    syncPos = pBaseFile->resetPlayback(pos, videoTrackIDs[0], TRUE, &bError, currentPosTimeStamp);
    nTimestampV=(-1);
    /* Set the position of other video tracks to syncPos */
    for ( int i=1; i < (int)numVideoTracks; i++ )
    {
      (void)pBaseFile->resetPlayback(syncPos, videoTrackIDs[i], FALSE, &bError, currentPosTimeStamp);
    }
    return syncPos;     
  }
  return 0;
}


/* ======================================================================
FUNCTION
  FileMedia::seekToSync

DESCRIPTION
  if available skips the video offset number of sync samples

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
uint32 FileMedia::seekToSync(int offSet, bool &bError, uint32 currentTimeStamp)
{
  if( pBaseFile )
  {
    return pBaseFile->skipNSyncSamples(offSet, videoTrackIDs[0], &bError, currentTimeStamp);           
  }
  return 0;
}
#endif /* FEATURE_QTV_REPOSITION_SYNC_FRAME */

/* ======================================================================
FUNCTION
  FileMedia::ResetVideoPlaybackPos

DESCRIPTION
  Reset the playback position after repositioning was successful.
  Called by the player if it later decides not to reposition e.g: when there is
  insufficient data to play after repositioning.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void FileMedia::ResetVideoPlaybackPos()
{
  if ( pBaseFile )
  {
    // reset the position of all the video tracks to the location before
    // repositioning. At the point repositioning of the video track was
    // successful.
    for ( int i=0; i < (int)numVideoTracks; i++ )
    {
       pBaseFile->resetPlaybackPos(videoTrackIDs[i]);
    }
  }
  return;
}

/* ======================================================================
FUNCTION
  FileMedia::SetAudioPosition

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
uint32 FileMedia::SetAudioPosition(unsigned long pos, bool &bError, uint32 currentPosTimeStamp)
{
  uint32 retPos = 0;

  if ( pBaseFile )
  {
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
    bAACPaddingSent = false;
#endif
    bError = FALSE;
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
    for ( int i=0; i < (int)numAudioTracks; i++ )
    {
      retPos = pBaseFile->resetPlayback(pos, audioTrackIDs[i], FALSE, &bError, currentPosTimeStamp);
    }
    nTimestampA=(-1);
    curAudioSDI=(-1);
    return retPos;
  }
  return 0;
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
void FileMedia::DownloadDone()
{

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Download Done");
    bDownloadDone = true;

}
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */


#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
uint32 FileMedia::SetTextPosition(unsigned long pos,bool &bError,uint32 currentPosTimeStamp)
{
  uint32 retPos = 0;

  if ( pBaseFile )
  {
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
    if(pBaseFile->IsTelopPresent())
    {
      retPos = pBaseFile->resetTelopPlayback(pos);
    }
    else  /* check 3GPP timed text */
#endif
    {
      for ( int i=0; i < (int)numTextTracks; i++ )
      {
        retPos = pBaseFile->resetPlayback(pos, textTrackIDs[i], FALSE, &bError, currentPosTimeStamp);
      }
    }
    nTimestampA=(-1);
    curAudioSDI=(-1);
    return retPos;
  }
  return 0;
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_FILE_FRAGMENTATION
/* ======================================================================
FUNCTION
  FileMedia::ParseFragment

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
bool FileMedia::ParseFragment( void )
{
  bool retVal = false;

  if ( pBaseFile)
  {   /* File is fragmented */
    retVal = pBaseFile->parseFragment();
  }

  return retVal;
}
#endif  /* FEATURE_FILE_FRAGMENTATION */

/* ======================================================================
FUNCTION
  FileMedia::SetAudioPlayerData

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
void FileMedia::SetAudioPlayerData(const void *client_data)
{
  if ( pBaseFile )
  {
    pBaseFile->setAudioPlayerData(client_data);
    AudioPlayerDataPtr = client_data;
  }
}

/* ======================================================================
FUNCTION
  FileMedia::SetVideoPlayerData

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
void FileMedia::SetVideoPlayerData(const void *client_data)
{
  if ( pBaseFile )
  {
    pBaseFile->setVideoPlayerData(client_data);
    VideoPlayerDataPtr = client_data;
  }
}

/* ======================================================================
FUNCTION
  FileMedia::SetTextPlayerData

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
void FileMedia::SetTextPlayerData(const void *client_data)
{
  if ( pBaseFile )
  {
    pBaseFile->setTextPlayerData(client_data);
    TextPlayerDataPtr = client_data;
  }
}


/* ======================================================================
FUNCTION
  FileMedia::SetLoopTrackFlag

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
void FileMedia::SetLoopTrackFlag(bool bLoop)
{
  bLoopTrackFlag = bLoop;
}

/* ======================================================================
FUNCTION
  FileMedia::getNextMediaSample

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
int32 FileMedia::getNextMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 &index, MediaReqType type)
{
  int32 nBytes = 0;

  file_sample_info_type SampleInfo;
  int returnCode = DEFAULT_ERROR;

  if (pBaseFile)
  {
    nBytes = pBaseFile->getNextMediaSample(id, buf, size, index);
  }

  if( (nBytes==0) && bLoopTrackFlag )
  {
    nMp4Error = (int)EVERYTHING_FINE;

    /* we create new mpeg4file for audio/video/text tracks */
    if( (type==VIDEO_REQUEST) || (type == AUDIO_REQUEST) || (type==TEXT_REQUEST) )
    {
      if ( pBaseFile )
      {
        memset(&SampleInfo,0,sizeof(SampleInfo));
				if(type == VIDEO_REQUEST)
				{
          /*For any track, store the timestamp of the last frame of the currently played track.
          This will be used to offset the timestamps of the successive frames after we start looping track.
					This effectively puts the next video frame at ts 0 at the end of the renderer queue after
					the video frames from previous playback loop.
					*/
					returnCode = pBaseFile->peekCurSample(videoTrackIDs[0], &SampleInfo);

					if(returnCode == EVERYTHING_FINE)
					{
						QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"FileMedia::getNextMediaSample: current videoLoopTrackTimeOffset = %d",videoLoopTrackTimeOffset);
						videoLoopTrackTimeOffset += (SampleInfo.delta + SampleInfo.time);
						QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"FileMedia::getNextMediaSample: Updated videoLoopTrackTimeOffset = %d",videoLoopTrackTimeOffset);
					}
					else
					{
						QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,"FileMedia::getNextMediaSample: peekCurSample returned an Error while updating videoLoopTrackTimeOffset = %d",videoLoopTrackTimeOffset);
					}
				}
        else if(type == AUDIO_REQUEST)
        {
          returnCode = pBaseFile->peekCurSample(audioTrackIDs[0], &SampleInfo);
          if(returnCode == EVERYTHING_FINE)
          {
            audioLoopTrackTimeOffset += (SampleInfo.delta + SampleInfo.time);
          }
        }
        else if(type==TEXT_REQUEST)
        {
          returnCode = pBaseFile->peekCurSample(textTrackIDs[0], &SampleInfo);
          if(returnCode == EVERYTHING_FINE)
          {
            textLoopTrackTimeOffset += (SampleInfo.delta + SampleInfo.time);
          }
        }
				QTV_Delete( pBaseFile );
				pBaseFile = NULL;
      }

      if(m_dwFileBufSize)
      {
        /* was buffer playback */
  #if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        pBaseFile = FileBase::openMediaFile( m_pFileBuf,
                                             m_pMpeg4Player,
                                              m_dwFileBufSize,
                                              m_bPlayVideo,
                                              m_bPlayAudio,
                                              m_bPlayText,
                                              m_bPseudoStream );
  #else
        pBaseFile = FileBase::openMediaFile(m_pFileBuf, m_pMpeg4Player, m_dwFileBufSize, m_bPlayVideo, m_bPlayAudio, m_bPlayText);
  #endif
      }
      else
      {
        /* was file playback */
#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
        pBaseFile = FileBase::openMediaFile(m_fileName, m_pMpeg4Player, m_bPlayVideo, m_bPlayAudio, m_bPlayText);
#endif /* FEATURE_QTV_MFDRM */
      }
      SetError();
    }

    if(pBaseFile)
    {

      pBaseFile->setAudioPlayerData(AudioPlayerDataPtr);
      pBaseFile->setVideoPlayerData(VideoPlayerDataPtr);
      pBaseFile->setTextPlayerData(TextPlayerDataPtr);

      if(type == VIDEO_REQUEST)
      {
        if(VideoPlayerDataPtr)
        {
#ifndef FEATURE_WINCE
          ((VideoPlayer *)VideoPlayerDataPtr)->Notify(VideoPlayer::PLAYING, Common::VIDEO_RESTARTING_TRACK);
#endif
        }
      }
      else if(type == AUDIO_REQUEST)
      {
        if(AudioPlayerDataPtr)
        {
          QTV_AUDIO_RESTARTING_TRACK_type *pEvent = QCCreateMessage(QTV_AUDIO_RESTARTING_TRACK, m_pMpeg4Player);
          if (pEvent)
          {
            QCUtils::PostMessage(pEvent, 0, NULL);
          }
        }
        if(m_pMpeg4Player)
        {
          m_pMpeg4Player->m_mediaSync.ResetPlaybackOffset(AVSync::AudioAV);
        }
        nBytes = pBaseFile->getNextMediaSample(id, buf, size, index);
      }
      else if(type == TEXT_REQUEST)
      {
        if(TextPlayerDataPtr)
        {
#ifndef FEATURE_WINCE
          ((TimedText *)TextPlayerDataPtr)->Notify(TimedText::ETTGetText, Common::CPVTEXT_RESTARTING_TRACK);
#endif
        }
        if(m_pMpeg4Player)
        {
          m_pMpeg4Player->m_mediaSync.ResetPlaybackOffset(AVSync::TextAV);
        }
        nBytes = pBaseFile->getNextMediaSample(id, buf, size, index);
      }
    }
    else
    {
      nBytes = 0;
    }
  }
  return nBytes;
}

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
/* ======================================================================
FUNCTION
  FileMedia::getNextGenericAudioMediaSample

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
int32 FileMedia::getNextGenericAudioMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 offset, uint32 &index, MediaReqType type)
{
  int32 nBytes = 0;
  
  file_sample_info_type SampleInfo;
  int returnCode = DEFAULT_ERROR;
  
  if (pBaseFile)
  {
    nBytes = pBaseFile->getNextGenericAudioMediaSample(id, buf, size, offset, index);
  }

  if( (nBytes==0) && bLoopTrackFlag )
  {
    nMp4Error = (int)EVERYTHING_FINE;

    /* we create new mpeg4file for audio/video/text tracks */
    if( (type==VIDEO_REQUEST) || (type == AUDIO_REQUEST) || (type==TEXT_REQUEST) )
    {
      if ( pBaseFile )
			{
				if(type == VIDEO_REQUEST)
				{
					/*For video track, store the timestamp of the last video frame of the currently played video track.
					This will be used to offset the timestamps of the successive video frames after we start looping video track.
					This effectively puts the next video frame at ts 0 at the end of the renderer queue after 
					the video frames from previous playback loop.
					*/
					memset(&SampleInfo,0,sizeof(SampleInfo));
					returnCode = pBaseFile->peekCurSample(videoTrackIDs[0], &SampleInfo);

					if(returnCode == EVERYTHING_FINE)
					{
						QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"FileMedia::getNextMediaSample: current videoLoopTrackTimeOffset = %d",videoLoopTrackTimeOffset);
						videoLoopTrackTimeOffset += (SampleInfo.delta + SampleInfo.time);
						QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"FileMedia::getNextMediaSample: Updated videoLoopTrackTimeOffset = %d",videoLoopTrackTimeOffset);
					}
					else
					{
						QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,"FileMedia::getNextMediaSample: peekCurSample returned an Error while updating videoLoopTrackTimeOffset = %d",videoLoopTrackTimeOffset);
					}
				}
				QTV_Delete( pBaseFile );
				pBaseFile = NULL;
      }

      if(m_dwFileBufSize)
      {
        /* was buffer playback */
  #if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) 
        pBaseFile = FileBase::openMediaFile( m_pFileBuf,
                                             m_pMpeg4Player,
                                              m_dwFileBufSize,
                                              m_bPlayVideo,
                                              m_bPlayAudio,
                                              m_bPlayText,
                                              m_bPseudoStream );
  #else
        pBaseFile = FileBase::openMediaFile(m_pFileBuf, m_pMpeg4Player, m_dwFileBufSize, m_bPlayVideo, m_bPlayAudio, m_bPlayText);
  #endif
      }
      else
      {
        /* was file playback */
#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
        pBaseFile = FileBase::openMediaFile(m_fileName, m_pMpeg4Player, m_bPlayVideo, m_bPlayAudio, m_bPlayText);
#endif /* FEATURE_QTV_MFDRM */
      }
      SetError();
    }

    if(pBaseFile)
    {

      pBaseFile->setAudioPlayerData(AudioPlayerDataPtr);
      pBaseFile->setVideoPlayerData(VideoPlayerDataPtr);
      pBaseFile->setTextPlayerData(TextPlayerDataPtr);

      if(type == VIDEO_REQUEST)
      {
        if(VideoPlayerDataPtr)
        {
#ifndef FEATURE_WINCE
          ((VideoPlayer *)VideoPlayerDataPtr)->Notify(VideoPlayer::PLAYING, Common::VIDEO_RESTARTING_TRACK);
#endif
        }
      }
      else if(type == AUDIO_REQUEST)
      {
        if(AudioPlayerDataPtr)
        {
#ifndef FEATURE_WINCE
          QTV_AUDIO_RESTARTING_TRACK_type *pEvent = QCCreateMessage(QTV_AUDIO_RESTARTING_TRACK, m_pMpeg4Player);
          if (pEvent)
          {
            QCUtils::PostMessage(pEvent, 0, NULL);
          }
#endif
        }
        if(m_pMpeg4Player)
        {
          m_pMpeg4Player->m_mediaSync.ResetPlaybackOffset(AVSync::AudioAV);
        }
      }
      else if(type == TEXT_REQUEST)
      {
        if(TextPlayerDataPtr)
        {
#ifndef FEATURE_WINCE
          ((TimedText *)TextPlayerDataPtr)->Notify(TimedText::ETTGetText, Common::CPVTEXT_RESTARTING_TRACK);
#endif
        }
        if(m_pMpeg4Player)
        {
          m_pMpeg4Player->m_mediaSync.ResetPlaybackOffset(AVSync::TextAV);
        }
      }

      nBytes = pBaseFile->getNextGenericAudioMediaSample(id, buf, size, offset, index);
    }
    else
    {
      nBytes = 0;
    }
  }
  return nBytes;
}
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */


#ifdef FEATURE_QTV_PSEUDO_STREAM
/* ======================================================================
FUNCTION
  FileMedia::UpdatePSBufferOffset

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
void FileMedia::UpdatePSBufferOffset( uint32 wBufferOffset )
{
  if ( pBaseFile)
  {
    pBaseFile->updateBufferWritePtr(wBufferOffset);
  }
}

/* ======================================================================
FUNCTION
  FileMedia::parsePseudoStream

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
bool FileMedia::ParsePseudoStream( void )
{
  bool returnValue = false;

  if ( pBaseFile)
  {
    returnValue = pBaseFile->parsePseudoStream();
  }
  return returnValue;
}
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

#ifdef FEATURE_FILE_FRAGMENTATION
/* ======================================================================
FUNCTION
  FileMedia::GetParseFragmentNum

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
uint16 FileMedia::GetParseFragmentNum( void )
{
  uint16 returnValue = 0;

  if ( pBaseFile)
  {
    returnValue = pBaseFile->getParseFragmentNum();
  }
  return returnValue;
}

/* ======================================================================
FUNCTION
  FileMedia::GetReadFragmentNum

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
uint16 FileMedia::GetReadFragmentNum( void )
{
  uint16 returnValue = 0;

  if ( pBaseFile)
  {
    returnValue = pBaseFile->getReadFragmentNum();
  }
  return returnValue;
}

#endif /* FEATURE_FILE_FRAGMENTATION */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
/* ======================================================================
FUNCTION
  FileMedia::UpdateHTTPBufferOffset

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
void FileMedia::UpdateHTTPBufferOffset( uint32 wBufferOffset )
{
  if ( pBaseFile)
  {
    pBaseFile->updateBufferWritePtr(wBufferOffset);
  }
}

/* ======================================================================
FUNCTION
  FileMedia::ProcessHTTPStream

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
bool FileMedia::ParseHTTPStream( void )
{
  bool returnValue = false;

  if ( pBaseFile)
  {
    returnValue = pBaseFile->parseHTTPStream();
  }
  return returnValue;
}

/* ======================================================================
FUNCTION
  FileMedia::HTTPCanPlayTrack

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
bool FileMedia::HTTPCanPlayTracks( uint32 pbTime )
{
  if ( pBaseFile )
  {
    return pBaseFile->CanPlayTracks(pbTime);
  }
  return false;
}
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

#ifdef FEATURE_FILE_FRAGMENTATION
/* ======================================================================
FUNCTION
  FileMedia::RepositionVideoAccessPoint

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
uint32 FileMedia::RepositionVideoAccessPoint( int32 skipNumber, bool &bError ,uint32 currentPosTimeStampMsec )
{
  uint32 retPos = 0;

  if ( pBaseFile )
  {
    for ( int i=0; i < (int)numVideoTracks; i++ )
    {
      retPos = pBaseFile->repositionAccessPoint(skipNumber,videoTrackIDs[i], bError ,currentPosTimeStampMsec );
    }
    nTimestampV=(-1);
    return retPos;
  }
  return 0;
}

/* ======================================================================
FUNCTION
  FileMedia::RepositionAudioAccessPoint

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
uint32 FileMedia::RepositionAudioAccessPoint( int32 skipNumber, bool &bError,uint32 currentPosTimeStampMsec )
{
  uint32 retPos = 0;

  if ( pBaseFile )
  {
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
    bAACPaddingSent = false;
#endif
    for ( int i=0; i < (int)numAudioTracks; i++ )
    {
      retPos = pBaseFile->repositionAccessPoint(skipNumber,audioTrackIDs[i], bError,currentPosTimeStampMsec);
    }
    nTimestampA=(-1);
    curAudioSDI=(-1);
    return retPos;
  }
  return 0;
}
#endif /*FEATURE_FILE_FRAGMENTATION*/

/* ======================================================================
FUNCTION
  FileMedia::GetVideoDimensions

DESCRIPTION
  returns height and width of video frame in track

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  true if info is available, else false.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool FileMedia::GetVideoDimensions(uint32 *pWidth, uint32 *pHeight, uint32 trackIdx)
{
  bool ret = FALSE;
  if ( pBaseFile && (numVideoTracks>trackIdx) )
  {
    *pWidth = pBaseFile->getTrackVideoFrameWidth(videoTrackIDs[trackIdx]);
    *pHeight = pBaseFile->getTrackVideoFrameHeight(videoTrackIDs[trackIdx]);
    ret = TRUE;
  }
  return ret;
}

#ifdef FEATURE_H264_DECODER

/* ======================================================================
FUNCTION
  FileMedia::GetPicParamSetNAL

DESCRIPTION
  copies pic parameter set NAL in given buffer.

PARAMETERS:
  status  OUTPUT  DATA_OK or DATA_END.
  idx     INPUT   index of video track.
  buf     OUTPUT  buffer for data to be copied.
  size    INPUT   size of buffer.

DEPENDENCIES

RETURN VALUE
  size of parameter set NAL or zero.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int FileMedia::GetNextParamSetNAL(MediaStatus &status, int idx, unsigned char * buf, int size)
{
  int nBytes=0;
  status=DATA_ERROR;

  if ( !IsParamSetAvailable(idx) )
  {
    status = DATA_UNDERRUN;
  }
  else if ( pBaseFile )
  {
    nBytes = pBaseFile->getNextParamSetNAL(videoTrackIDs[idx], buf, (uint32) size);
    SetError();
    if ( nBytes==0 )
    {
      status = DATA_END;
    }
    else if( nBytes == READ_FAILURE || nBytes == FRAGMENT_CORRUPT)
    {
      nBytes = 0;
      status = DATA_ERROR;
    }
    else if ( nBytes>0 )
    {
      status = DATA_OK;
    }
  }
  return nBytes;
}

/* ======================================================================
FUNCTION
  FileMedia::ResetParamSetNAL

DESCRIPTION
  resets the parameter set counter, so next time you can call
  GetNextParamSetNAL to access param sets again.

PARAMETERS:
  idx     INPUT   index of video track.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool FileMedia::ResetParamSetNAL(int trackIdx)
{
  if ( pBaseFile && numVideoTracks )
  {
    pBaseFile->resetParamSetNAL(videoTrackIDs[trackIdx]);
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION
  FileMedia::GetSizeOfNALLengthField

DESCRIPTION
 This function returns the size of NAL length field present before actual
 NAL in a frame. It is only valid for local file playback.

DEPENDENCIES

RETURN VALUE
  returns size of NAL field length in bytes.

SIDE EFFECTS
  Detail any side effects.


========================================================================== */
int FileMedia::GetSizeOfNALLengthField(int trackIdx)
{
  if ( pBaseFile && numVideoTracks )
  {
    return pBaseFile->GetSizeOfNALLengthField(videoTrackIDs[trackIdx]);
  }
  return 0;
}

#endif /* FEATURE_H264_DECODER */

/* ======================================================================
FUNCTION
  FileMedia::HasClipMetaData

DESCRIPTION
  Only MP4 Clips have KDDI meta data.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  Detail any side effects.
========================================================================== */
bool FileMedia::HasClipMetaData()
{
  if (pBaseFile)
  {
    return pBaseFile->HasMetaData();
  }
  return false;
}

#ifdef FEATURE_QTV_SKT_MOD_MIDI
/* ======================================================================
FUNCTION:
  FileMedia::IsMidiDataPresent

DESCRIPTION:
  if SKT specific MIDI data is present or not.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  TRUE - if MIDI data is present
  FALSE - MIDI data not present.

SIDE EFFECTS:
  None.
======================================================================*/
bool FileMedia::IsMidiDataPresent()
{
  if ( pBaseFile )
  {
    return pBaseFile->IsMidiDataPresent();
  }
  return false;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetMidiDataSize

DESCRIPTION:
  returns MIDI data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of MIDI data
  0 if MIDI not present.

SIDE EFFECTS:
  None.
======================================================================*/
uint32 FileMedia::GetMidiDataSize()
{
  if ( pBaseFile )
  {
    return pBaseFile->GetMidiDataSize();
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetMidiData

DESCRIPTION:
  copies the MIDI data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf  - INPUT/OUTPUT  - buffer for data to be copied.
  size  - INPUT         - size of buffer and max data to be copied.
  offset- INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
  0 if no MIDI data is available

SIDE EFFECTS:
  None.
======================================================================*/
uint32 FileMedia::GetMidiData(uint8 *pBuf, uint32 size, uint32 offset)
{
  if ( pBaseFile )
  {
    return pBaseFile->GetMidiData(pBuf, size, offset);
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  FileMedia::IsLinkDataPresent

DESCRIPTION:
  if SKT specific LINK data is present or not.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  TRUE - if LINK data is present
  FALSE - LINK data not present.

SIDE EFFECTS:
  None.
======================================================================*/
bool FileMedia::IsLinkDataPresent()
{
  if ( pBaseFile )
  {
    return pBaseFile->IsLinkDataPresent();
  }
  return false;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetLinkDataSize

DESCRIPTION:
  returns LINK data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of LINK data
  0 if LINK ATOM not present.

SIDE EFFECTS:
  None.
======================================================================*/
uint32 FileMedia::GetLinkDataSize()
{
  if ( pBaseFile )
  {
    return pBaseFile->GetLinkDataSize();
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetLinkData

DESCRIPTION:
  copies the LINK data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf  - INPUT/OUTPUT  - buffer for data to be copied.
  size  - INPUT         - size of buffer and max data to be copied.

RETURN VALUE:
  actual bytes copied into buffer
  0 if no LINK data is available

SIDE EFFECTS:
  None.
======================================================================*/
uint32 FileMedia::GetLinkData(uint8 *pBuf, uint32 size)
{
  if ( pBaseFile )
  {
    return pBaseFile->GetLinkData(pBuf, size);
  }
  return 0;
}

#endif /* FEATURE_QTV_SKT_MOD_MIDI */


/* ======================================================================
FUNCTION:
  FileMedia::IsDataPresent

DESCRIPTION:
  Checks if the specified data-type data is present or not.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  TRUE - if specified data-type data is present
  FALSE - if specified data-type data not present.

SIDE EFFECTS:
  None.
======================================================================*/
bool FileMedia::IsDataPresent(QtvPlayer::DataT dType, uint32 track_id)
{
  if ( pBaseFile )
  {
    return pBaseFile->IsDataPresent(dType, track_id);
  }
  return false;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetDataSize

DESCRIPTION:
  returns the specified data-type data size.

INPUT/OUTPUT PARAMETERS:
  dType - data-type.
  offset - used as a track_id if data present in multiple tracks.

RETURN VALUE:
  size of the specified data-type.
  0 if specified data not present.

SIDE EFFECTS:
  None.
======================================================================*/
uint32 FileMedia::GetDataSize(QtvPlayer::DataT dType, uint32 offset)
{
    if ( pBaseFile )
    {
        return pBaseFile->GetDataSize(dType, offset);
    }
    return 0;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetData

DESCRIPTION:
  copies the specified data-type data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  dType - data-type.
  pBuf  - INPUT/OUTPUT  - buffer for data to be copied.
  size  - INPUT         - size of buffer and max data to be copied.
  offset- INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
  0 if no data is available

SIDE EFFECTS:
  None.
======================================================================*/
uint32 FileMedia::GetData(QtvPlayer::DataT dType, uint8 *pBuf, uint32 size, uint32 offset)
{
    if ( pBaseFile )
    {
        return pBaseFile->GetData(dType, pBuf, size, offset);
    }
    return 0;
}


/* ======================================================================
FUNCTION:
  FileMedia::GetNumAudioChannels

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long FileMedia::GetNumAudioChannels(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->GetNumAudioChannels(audioTrackIDs[idx]);
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetAudioTrackLanguage

DESCRIPTION:
   Get the audio track language
INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
bool FileMedia::GetAudioTrackLanguage(OSCL_STRING &oLanguage, uint32 id)
{
  oLanguage = (char *)NULL;
  if ( pBaseFile )
  {
    oLanguage = pBaseFile->getAudioTrackLanguage(id);
    SetError();
  }

  if ( oLanguage.get_cstr() == NULL )
    return false;
  else
    return true;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetAudioBitRate

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long FileMedia::GetAudioBitRate(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->getTrackAverageBitrate(audioTrackIDs[idx]);
  }
  return 0;

}
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
/* ======================================================================
FUNCTION:
  FileMedia::SetAudioBitRate

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:
          
SIDE EFFECTS:
  None.
======================================================================*/
unsigned long FileMedia::SetAudioBitRate(uint32 bitRate)
{
  if ( pBaseFile )
  {
    return pBaseFile->setTrackAverageBitrate(bitRate);
  }
  return 0;

}

/* ======================================================================
FUNCTION:
  FileMedia::setRepositioningAllowed

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:
          
SIDE EFFECTS:
  None.
======================================================================*/
void FileMedia::setRepositioningAllowed(bool repositioningNotAllowed)
{
  if ( pBaseFile )
  {
    pBaseFile->setRepositioningAllowedFlag(repositioningNotAllowed);
  }  
}

/* ======================================================================
FUNCTION:
  FileMedia::GetTimeStampFromMedia

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 FileMedia::GetTimeStampFromMedia()
{
  if ( pBaseFile )
  {
    return pBaseFile->GetTimeStampFromMedia( );
  }
  return 0;
}

/* ======================================================================
FUNCTION
  FileMedia::IsGenericAudioFileMediaInitialized

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
bool FileMedia::IsGenericAudioFileMediaInitialized()
{
  bool initialized=false;
  if ( pBaseFile )
  {
    initialized = pBaseFile->IsGenericAudioFileMediaInitialized();
    SetError();
  }
  return initialized;
}


/* ======================================================================
FUNCTION
  FileMedia::IsGenericAudioFileRepositioned

DESCRIPTION
  Returns if we issued reposition command or not to CMX or not 
  through media.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool FileMedia::IsGenericAudioFileRepositioned()
{
  bool initialized=false;
  if ( pBaseFile )
  {
    initialized = pBaseFile->IsGenericAudioFileRepositioned();
    SetError();
  }
  return initialized;
}


#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
/* ======================================================================
FUNCTION:
  FileMedia::SetFileSize

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:
          
SIDE EFFECTS:
  None.
======================================================================*/
unsigned long FileMedia::SetFileSize(uint32 fileSize)
{
  if ( pBaseFile )
  {
    return pBaseFile->setFileSize(fileSize);
  }
  return 0;
}
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
/* ======================================================================
   FUNCTION:
     FileMedia::GetVideoFrameRate

   DESCRIPTION:

   INPUT/OUTPUT PARAMETERS:

   RETURN VALUE:

   SIDE EFFECTS:
     None.
   ======================================================================*/
float FileMedia::GetVideoFrameRate(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->getTrackVideoFrameRate(videoTrackIDs[idx]);
  }
  return 0;
}

/* ======================================================================
   FUNCTION:

     FileMedia::GetVideoBitRate

   DESCRIPTION:

   INPUT/OUTPUT PARAMETERS:

   RETURN VALUE:

   SIDE EFFECTS:
     None.
   ======================================================================*/

unsigned long FileMedia::GetVideoBitRate(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->getTrackAverageBitrate(videoTrackIDs[idx]);
  }
  return 0;
}
#ifdef FEATURE_QTV_AVI
/* ======================================================================
FUNCTION:
  FileMedia::getAudioFrameDuration

DESCRIPTION:
  Returns duration for each audio frame

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
long FileMedia::getAudioFrameDuration(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->getAudioFrameDuration(audioTrackIDs[idx]);
  }
  return 0;
}
/* ======================================================================
FUNCTION:
  FileMedia::SetIDX1Cache

DESCRIPTION:
  Sets idx1 cache pointer

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
void  FileMedia::SetIDX1Cache(void* ptr)
{
  if ( pBaseFile )
  {
    pBaseFile->SetIDX1Cache(ptr);
  } 
}
/* ======================================================================
FUNCTION:
  FileMedia::GetIDX1Cache

DESCRIPTION:
  Returns idx1 cache pointer

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
void* FileMedia::GetIDX1Cache()
{
  if ( pBaseFile )
  {
    return pBaseFile->GetIDX1Cache();
  } 
  return NULL;
}
#endif

/* ======================================================================
FUNCTION:
  FileMedia::GetAudioBitsPerSample

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long FileMedia::GetAudioBitsPerSample(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->GetAudioBitsPerSample(audioTrackIDs[idx]);
  }
  return 0;
}

#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) 
/* ======================================================================
FUNCTION:
  FileMedia::GetAudioCodecVersion

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long FileMedia::GetAudioCodecVersion(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->getTrackContentVersion(audioTrackIDs[idx]);
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetFixedAsfAudioPacketSize

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long FileMedia::GetFixedAsfAudioPacketSize(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->GetFixedAsfAudioPacketSize(audioTrackIDs[idx]);
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetAudioEncoderOptions

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long FileMedia::GetAudioEncoderOptions(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->GetAudioEncoderOptions(audioTrackIDs[idx]);
  }
  return 0;
}
/* ======================================================================
FUNCTION:
  FileMedia::isStartOfDummyBytes()

DESCRIPTION:
  returns true if asffile has started sending dummy bytes.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
bool FileMedia::isStartOfDummyBytes(int idx)
{
  if(pBaseFile )
  {
    return pBaseFile->isStartOfDummyBytes(audioTrackIDs[idx]);
  }
  return false;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetAudioASFPacketDuration

DESCRIPTION:returns estimated audio packet duration in milliseconds.

INPUT/OUTPUT PARAMETERS:
  audio track id
RETURN VALUE:
  ASF audio packet duration
SIDE EFFECTS:
  None.
======================================================================*/
long FileMedia::GetAudioASFPacketDuration(int trackid)
{
  if(pBaseFile)
  {
    return pBaseFile->GetAudioASFPacketDuration(audioTrackIDs[trackid]);
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  FileMedia:: GetAudioVirtualPacketSize

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
uint16  FileMedia::GetAudioVirtualPacketSize( int idx)
{
  if(pBaseFile )
  {
    return pBaseFile->GetAudioVirtualPacketSize(audioTrackIDs[idx]);
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetAudioAdvancedEncodeOptions

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
uint16 FileMedia::GetAudioAdvancedEncodeOptions(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->GetAudioAdvancedEncodeOptions(audioTrackIDs[idx]);
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  FileMedia::GetAudioAdvancedEncodeOptions2

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
uint32 FileMedia::GetAudioAdvancedEncodeOptions2(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->GetAudioAdvancedEncodeOptions2(audioTrackIDs[idx]);
  }
  return 0;
}


/* ======================================================================
FUNCTION:
  FileMedia::GetAudioChannelMask

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
uint16 FileMedia::GetAudioChannelMask(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->GetAudioChannelMask(audioTrackIDs[idx]);
  }
  return 0;
}


/* ======================================================================
FUNCTION:
  FileMedia::GetAudioArmDataReqThr

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
uint16 FileMedia::GetAudioArmDataReqThr(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->GetAudioArmDataReqThr(audioTrackIDs[idx]);
  }
  return 0;
}


/* ======================================================================
FUNCTION:
  FileMedia::GetAudioValidBitsperSample

DESCRIPTION:

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
======================================================================*/
uint8 FileMedia::GetAudioValidBitsperSample(int idx)
{
  if ( pBaseFile )
  {
    return pBaseFile->GetAudioValidBitsperSample(audioTrackIDs[idx]);
  }
  return 0;
}
#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

uint32 FileMedia::GetAudioDurationAvailable()
{
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  int playbackTime = 0;
  if (pPVDL)
  {
    if (pPVDL->GetPlaybackTime(&playbackTime) == PVDL_SUCCESS)
    {
      return (uint32)playbackTime;
    }

  }
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
  return 0;
}

uint32 FileMedia::GetVideoDurationAvailable()
{
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  int playbackTime = 0;
  if (pPVDL)
  {
    if (pPVDL->GetPlaybackTime(&playbackTime) == PVDL_SUCCESS)
    {
      return (uint32)playbackTime;
    }

  }
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
  return 0;
}

bool FileMedia::IsAudioAvailable(  bool* )
{
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  int playbackTime=(-1);
  bool bPs=false;
  MediaFileType clipType;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "FileMedia::IsAudioAvailable");

  if(NULL == pPVDL )
  {
    return true;
  }
  clipType = GetFileType();

  if((clipType == CONTENT_VIDEO ) ||
     (clipType == CONTENT_VIDEO_STILL_IMAGE ) ||
     (clipType == CONTENT_VIDEO_STILL_IMAGE_TEXT)  )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Video only content IsAudioAvailable returning true");
    return true;
  }

  if (!bDownloadDone)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Audio Download not done");

    if (pPVDL)
    {
      if (pPVDL->GetPlaybackTime(&playbackTime) == PVDL_SUCCESS)
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Audio Playbacktime : %d", playbackTime);
        bPs=true;
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"GetPlaybackTime Failed");
      }
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Audio Download DONE");
  }

  if (bDownloadDone)
  {
    state = Play;
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Download DONE. Set audio state to Play");
    return true;
  }

  if (bPs && !bDownloadDone)
  {
    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Playback Pos %d, Audio Pos %d, Preroll %d",
                  playbackTime,
                  audioPos,
                  preroll);

    switch(state)
    {
      case Play:

        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Play State");
        if ((playbackTime - audioPos) > MIN_BUFFER_DURATION)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"IsAudioAvailable returning TRUE");
          return true;
        }
        else
        {
          state = Buffer;
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                        " Audio State changed to BUFFER when audio position is %d Msec less than download time",
                        (playbackTime - audioPos));

        }
        break;

      case Buffer:

        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Buffer State");
        pPVDL->getPreroll(preroll);
        if ((playbackTime - audioPos) > preroll)
        {
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                        " State changed to PLAY when download time is %d Msec more than Audio pos",
                        (playbackTime - audioPos));

          if (bCheckRate)
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH," Check Data Rate");
            if (CheckDataRate(playbackTime))
            {
              state = Play;
#ifndef FEATURE_WINCE			  
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH," CheckDataRate succeeded-->"
                           "IsAudioAvailable returning TRUE ");
#endif						   
              return true;
            }
          }
          else
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH," Check Data Rate is OFF");
            state = Play;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"IsAudioAvailable returning TRUE");
            return true;
          }

        }
        break;
    }
  }
  else
  {
    //pure file

#ifdef REBUFFER_TEST
    //rebuffer for 2 seconds out of every 10
    //if ((Utils::GetSystemTickCount()%10000) < 2000) return false;
    if (( ZUtils::Clock()%10000) < 5000)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Force Rebuffering on Audio track");
      return false;
    }
#endif
    return true;
  }
  return false;
#else
  return true;
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
}

bool FileMedia::IsVideoAvailable()
{
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  int playbackTime=(-1);
  bool bPs=false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "FileMedia::IsVideoAvailable");

  if(NULL == pPVDL )
  {
    return true;
  }

  if (!bDownloadDone /*&& IsAudioAvailable()*/)
  {
    if(IsAudioAvailable())
    {

      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Video Download is not done");
      if (pPVDL)
      {
        if (pPVDL->GetPlaybackTime(&playbackTime) == PVDL_SUCCESS)
        {
          bPs=true;
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Video Playbacktime : %d", playbackTime);
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"GetPlaybackTime Failed");
        }

      }
    } /* End of IsAudioAvailable() */
    else
    {
      state = Buffer;
#ifndef FEATURE_WINCE	  	  
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Dload not Done and Audio is Not available"
                   "..Changing video player state to BUFFER");
#endif				   
      return false;

    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Video Download DONE");
  }


  if (bDownloadDone)
  {
    state = Play;
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Download DONE. Set video state to Play");
    return true;
  }

  if (bPs && !bDownloadDone)
  {
    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Playback Pos %d, Video Pos %d, Preroll %d",
                  playbackTime,
                  videoPos,
                  preroll);

    switch(state)
    {
      case Play:
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Play State");

        if ((playbackTime - videoPos) > MIN_BUFFER_DURATION)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"IsVideoAvailable returning TRUE");
          return true;
        }
        else
        {
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                        " State changed to BUFFER when video position is %d Msec less than download time",
                        (playbackTime - videoPos));
          state = Buffer;
        }
        break;

      case Buffer:

        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Buffer State");
        pPVDL->getPreroll(preroll);
        if ((playbackTime - videoPos) > preroll)
        {
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                        " State changed to PLAY when download time is %d Msec more than video pos",
                        (playbackTime - videoPos));

          if (bCheckRate)
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH," Check Data Rate");

            if (CheckDataRate(playbackTime))
            {
              state = Play;
#ifndef FEATURE_WINCE			  
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH," CheckDataRate succeeded-->"
                           "IsVideoAvailable returning TRUE ");
#endif
              return true;
            }
          }
          else
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH," Check Data Rate is OFF");
            state = Play;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"IsVideoAvailable returning TRUE");
            return true;
          }
        }
        break;
    }
  }
  else
  {
    //pure local file.
#ifdef REBUFFER_TEST
    //rebuffer for 2 seconds out of every 10
    // if ((Utils::GetSystemTickCount()%10000) < 2000) return false;
    if (( ZUtils::Clock()%10000) < 5000)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Force Rebuffering on Video track");
      return false;
    }
#endif /* REBUFFER_TEST */
    return true;
  }

  return false;
#else /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
  return true;
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
bool FileMedia::CheckDataRate(int playtime)
//return true if data rate is OK for playback.
{
  if (bDownloadDone)
  {
    return true;
  }
  else
  {
    int bytes;
    if (pPVDL->GetBytesRemaining(&bytes)==PVDL_SUCCESS)
    {
      long lTime=GetDurationMsec()-playtime;
      if (bytes<=0 || lTime<=0)
      {
        return true;
      }
      else
      {
        float rate;
        if (pPVDL->GetDataRate(&rate) == PVDL_SUCCESS)
        {
          // printf("Rate %f bytes %d time %d\n",rate,bytes,time);
          return  0.9*rate > (float)bytes/(float)lTime;
        }
      }
    }
    return false;
  }
}
#endif /*  FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

/* ======================================================================
FUNCTION
  FileMedia::FileMedia

DESCRIPTION
  Constructor

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
#else
FileMedia::FileMedia( const OSCL_STRING &name,
                      Mpeg4Player *pMpeg4Player,
                      bool bPlayVideo,
                      bool bPlayAudio,
                      bool bPlayText )
#endif /* FEATURE_QTV_MFDRM */
{
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  state       = Play;
  pPVDL       = NULL;
  audioPos    = 0;
  videoPos    = 0;
  bCheckRate  = true;
#endif

  m_pMpeg4Player  = pMpeg4Player;

  FileInitData();
  
  m_fileName      = name;
  m_bPlayVideo    = bPlayVideo;
  m_bPlayAudio    = bPlayAudio;
  m_bPlayText     = bPlayText;
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
  pBaseFile = FileBase::openMediaFile(name, m_pMpeg4Player, bPlayVideo, bPlayAudio, bPlayText);
#endif /* FEATURE_QTV_MFDRM */

  if ( pBaseFile )
  {
    SetError();
  }
}

#ifdef FEATURE_QTV_DRM_DCF
/* ======================================================================
FUNCTION
  FileMedia::FileMedia

DESCRIPTION
  Constructor for supporting DCF playback

DEPENDENCIES
  None

INPUT PARAMETERS:
->inputStream:IxStream*
->urnType:It should be URN_INPUTSTREAM
->bPlayVideo:Indicates if this is video instance
->bPlayAudio:Indicates if this is audio instance
->bPlayText:Indicates if this is text instance

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================== */
FileMedia::FileMedia(  dcf_ixstream_type inputStream,
                       QtvPlayer::URNTypeT urnType,
                       Mpeg4Player *pMpeg4Player,
                       bool bPlayVideo,
                       bool bPlayAudio,
                       bool bPlayText)
{
  m_pMpeg4Player  = pMpeg4Player;

  FileInitData();
  
  m_bPlayVideo    = bPlayVideo;
  m_bPlayAudio    = bPlayAudio;
  m_bPlayText     = bPlayText;

  pBaseFile = FileBase::openMediaFile(inputStream, m_pMpeg4Player, urnType,bPlayVideo, bPlayAudio, bPlayText);

  if ( pBaseFile != NULL)
  {
    SetError();
  }
}
#endif

/* ======================================================================
FUNCTION
  FileMedia::FileMedia

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
FileMedia::FileMedia(unsigned char *pBuf, uint32 bufSize, Mpeg4Player *pMpeg4Player,
                     bool bPlayVideo, bool bPlayAudio, bool bPlayText
#if defined (FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                    ,bool bPseudoStream
                    ,uint32 wBufferOffset
                    ,uint32 wStartupTime
                    ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize
                    ,QtvPlayer::FetchBufferedDataT FetchBufferedData
                    ,QtvPlayer::InstanceHandleT handle
#endif  /* FEATURE_QTV_PSEUDO_STREAM */
        /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
                     )
{
  m_pMpeg4Player  = pMpeg4Player;

  FileInitData();

  m_pFileBuf      = pBuf;
  m_dwFileBufSize = bufSize;
  m_bPlayVideo    = bPlayVideo;
  m_bPlayAudio    = bPlayAudio;
  m_bPlayText     = bPlayText;
  
#if defined (FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  m_bPseudoStream = bPseudoStream;
#endif /* FEATURE_QTV_PSEUDO_STREAM ||
          FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#if defined (FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      pBaseFile = FileBase::openMediaFile(pBuf,
                                         m_pMpeg4Player,
                                         bufSize,
                                         bPlayVideo,
                                         bPlayAudio,
                                         bPlayText,
                                         bPseudoStream,
                                         wBufferOffset,
                                         wStartupTime,
                                         FetchBufferedDataSize,
                                         FetchBufferedData,
                                         handle);
#else
      pBaseFile = FileBase::openMediaFile(pBuf, m_pMpeg4Player, bufSize, bPlayVideo, bPlayAudio,bPlayText);
#endif  /* FEATURE_QTV_PSEUDO_STREAM */
        /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

  if ( pBaseFile )
  {
    SetError();
  }
}

#ifdef FEATURE_QTV_WM_DRM_API
/* ======================================================================
FUNCTION
  Mpeg4Player::RegisterDRMDecryptMethod

DESCRIPTION
  Registers DRM decryption method.

DEPENDENCIES
  key can be set only in IDLE state.

RETURN VALUE
  TRUE if successful, else FALSE

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool FileMedia::RegisterDRMDecryptMethod(QtvPlayer::DRMDecryptMethodT pDRMDecriptFunction, void *pClientData)
{
  if ( pBaseFile)
  {
    return pBaseFile->RegisterDRMDecryptMethod(pDRMDecriptFunction, pClientData);
  }
  return false;
}
#endif /* FEATURE_QTV_WM_DRM_API */

/* ======================================================================
FUNCTION
  FileMedia::~FileMedia

DESCRIPTION
  Destructor

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
FileMedia::~FileMedia()
{
  if ( pBaseFile )
  {
    QTV_Delete( pBaseFile );
    pBaseFile = NULL;
  }
}

/* ======================================================================
FUNCTION
  FileMedia::FileInitData

DESCRIPTION
  Initialize memembers

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
========================================================================== */
void FileMedia::FileInitData()
{
  // Init base class members
  BaseInitData();

  nMp4Error       = (int)EVERYTHING_FINE;

  m_fileName      = "";
  m_pFileBuf      = NULL;
  m_dwFileBufSize = 0;
  m_bPlayVideo    = false;
  m_bPlayAudio    = false;
  m_bPlayText     = false;
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  bAACPaddingSent = false;
#endif
#if defined (FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  m_bPseudoStream = FALSE;
#endif /* FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD  */

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  nTimestampA=nTimestampV = nTimestampT = curAudioSDI = (-1);
#else
  nTimestampA=nTimestampV = curAudioSDI = (-1);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  evrcFrameType      = 0;
  amrFrameType       = 0;
  aacSamplesPerFrame = 0;     //ToDo: This should be moved to base class "Media"?
  preroll            = 0;

  AudioPlayerDataPtr = NULL;
  VideoPlayerDataPtr = NULL;
  TextPlayerDataPtr  = NULL;
  m_AACDataFormatType = AAC_DATA_FORMAT_UNKNOWN;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  pPVDL              = NULL;
  audioPos           = 0;
  videoPos           = 0;
  bDownloadDone      = false;
  bCheckRate         = false ;
  state              = Buffer;
#endif
  lockTrackList      = false;
  m_defaultTrackSelectionPolicy = 
    QTV_New_Args(DefaultTrackSelectionPolicy, ());

  if (m_pMpeg4Player != NULL)
  {
    m_defaultTrackSelectionPolicy->SetCodecSelectCriteria(
      (streaming_codec_select_criteria) m_pMpeg4Player->GetQTVConfigItem(
      QtvConfig::QTVCONFIG_CODECSELECT_CRITERIA));

    m_defaultTrackSelectionPolicy->BlockH264(
      (m_pMpeg4Player->GetQTVConfigItem(
       QtvConfig::QTVCONFIG_BLOCK_NON_MFLO_H264) != 0));
  }
}

/*===========================================================================
FUNCTION:
  FileMedia::UserCompare

DESCRIPTION:
  Compares the track list entries with currently selected ones and updates
  the chosen tracks accordingly. It takes the audio/video/text tracks as input. 
  
RETURN VALUE:
  Returns the result of the comparison. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int FileMedia::UserCompare(bool &bError,
                                            int nAudID, 
                                            int nVidID,
                                            int nTextID)
{
  unsigned int bRet = 0;    // bit mask 
  int idx           =-1;    // index   
  QTV_USE_ARG2(nVidID,bError);
  QTV_USE_ARG1(nTextID);
  if(nAudID >= 0)
  {
    // Compare the audio tracks first
    // Map the audio track to its associated index within the list
    idx = MapTrackToIndex(nAudID);
    if(idx >= 0)
    {
      for (int32 i = 0; i < m_trackList->GetNumTracksAvailable(); i++)
      {
        if (!m_trackList->IsTrackSelected(i))
        {
          continue;
        }        
        if(audioTrackIDs[0] == nAudID)
        {
#ifdef FEATURE_QTV_GENERIC_BCAST		  
          bRet = QTV_MEDIA_TRACK_LIST_IDENTICAL;
          break;
#endif /* FEATURE_QTV_GENERIC_BCAST */

        }
        if(Media::IsAudioCodec(m_trackList->GetCodecType(i)))
        {
          m_trackList->SelectTrack(i, false);        
      m_trackList->SelectTrack(idx, true);
      --numAudioTracks;
          lockTrackList = false;
      HandleSelectedTrackList();
          lockTrackList = true;
          break;
        }
      }
    }
    else
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,
                        QTVDIAG_PRIO_ERROR,
                    "User Compare:Invalid Audio Track ID; Ignoring it silently",nAudID);
      nAudID = -1;
    }
  }


  if(nTextID >= 0)
  {
    // Compare the audio tracks first
    // Map the audio track to its associated index within the list
    idx = MapTrackToIndex(nTextID);
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
      "FileMedia::MapTrackToIndex output = %d",idx);
    if(idx >= 0)
    {
      for (int32 i = 0; i < m_trackList->GetNumTracksAvailable(); i++)
      {
        if (!m_trackList->IsTrackSelected(i))
        {
          continue;
        }
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
          "FileMedia::UserCompare current track = %d",i);
        if(textTrackIDs[0] == nTextID)
        {
#ifdef FEATURE_QTV_GENERIC_BCAST
          bRet = QTV_MEDIA_TRACK_LIST_IDENTICAL;
#endif /* FEATURE_QTV_GENERIC_BCAST */
        }
        if(Media::IsTextCodec(m_trackList->GetCodecType(i)))
        {
          m_trackList->SelectTrack(i, false);
          m_trackList->SelectTrack(idx, true);
          --numTextTracks;
          lockTrackList = false;
          HandleSelectedTrackList();
          lockTrackList = true;
          break;
        }
      }
    }
    else
    {
#ifdef FEATURE_QTV_GENERIC_BCAST
      QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,
                        QTVDIAG_PRIO_ERROR,
                    "User Compare:Invalid Text Track ID; Ignoring it silently",nTextID);
#endif
      nTextID = -1;
    }
  }

  if(nVidID >= 0)
  {
    // Compare the audio tracks first
    // Map the audio track to its associated index within the list
    idx = MapTrackToIndex(nVidID);
    if(idx >= 0)
    {
      for (int32 i = 0; i < m_trackList->GetNumTracksAvailable(); i++)
      {
        if (!m_trackList->IsTrackSelected(i))
        {
          continue;
        }
        if(videoTrackIDs[0] == nVidID)
        {
#ifdef FEATURE_QTV_GENERIC_BCAST
          bRet = QTV_MEDIA_TRACK_LIST_IDENTICAL;
          break;
#endif /* FEATURE_QTV_GENERIC_BCAST */
        }
        if(Media::IsVideoCodec(m_trackList->GetCodecType(i)))
        {
          m_trackList->SelectTrack(i, false);
      m_trackList->SelectTrack(idx, true);
          --numVideoTracks;
          lockTrackList = false;
      HandleSelectedTrackList();
          lockTrackList = true;
          break;
        }
      }
    }
    else
    {
#ifdef FEATURE_QTV_GENERIC_BCAST
      QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST, 
                        QTVDIAG_PRIO_ERROR,
                    "User Compare:Invalid video Track ID; Ignoring it silently",nVidID);
#endif
      nVidID = -1;
    }
  }
  return bRet; 
} /* UserCompare */


/*===========================================================================
FUNCTION MapTrackToIndex

DESCRIPTION:
  Maps track identifier to its track list index

RETURN VALUE:
  Returns the array index if valid match found. Negative one otherwise. 
          
SIDE EFFECTS:
  None. 
===========================================================================*/
int FileMedia::MapTrackToIndex(int nTrackID)
{
  int bRet = -1;
  int32 numTracks = 0;

  if ( pBaseFile && pBaseFile->getNumTracks() )
  {
    numTracks = (int32) pBaseFile->getNumTracks();
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
      "FM MapTrackToIndex: Number of Tracks = %d",numTracks);

    //Allocate temporary ID list.
    uint32 *trackIDList = QTV_New_Array(uint32, numTracks);   
    if (trackIDList == NULL)
    {
      return bRet;
    }

    (void)pBaseFile->getTrackWholeIDList(trackIDList);

    for ( uint32 i = 0; i < numTracks; i++ )
    {
      if(trackIDList[i] == nTrackID)
      {
        bRet = i;
        break;
      }
    }   
   QTV_Delete_Array(trackIDList);
  }  
  return bRet;
}


Media::AACDataFormatType FileMedia::GetAACDataFormat(void)
{
   if(m_AACDataFormatType == AAC_DATA_FORMAT_UNKNOWN)
   {
      if(pBaseFile->isADTSHeader())
      {
          m_AACDataFormatType = AAC_DATA_FORMAT_ADTS;
      }
      else
      {
	      m_AACDataFormatType = AAC_DATA_FORMAT_PSEUDO_RAW;
      }
   }
   return m_AACDataFormatType;
}

