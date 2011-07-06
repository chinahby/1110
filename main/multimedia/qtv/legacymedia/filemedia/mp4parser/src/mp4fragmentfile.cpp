/* =======================================================================
                              mp4FragmentFile.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/mp4fragmentfile.cpp#32 $
$DateTime: 2010/10/25 00:21:29 $
$Change: 1491087 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Common definitions                      */
#include "assert.h"

#if defined(FEATURE_FILE_FRAGMENTATION)

#include "mp4FragmentFile.h"
#include "cmx.h"


#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
extern "C" {
#include "event.h"
#include "qtv_event.h"
}
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

//Define following feature to have lots of debug messages when doing inter fragment repositioning.
//#define FEATURE_INTER_FRAGMENT_REPOS_DEBUG
/*===========================================================================

FUNCTION  Mp4FragmentFile

DESCRIPTION
  This is the Mp4FragmentFile class constructor - when executed is parses and caches
  the meta-data of a fragmented file just opened.

===========================================================================*/
Mp4FragmentFile::Mp4FragmentFile(  OSCL_STRING filename,
                                   Mpeg4Player *pMpeg4Player,
                                    unsigned char *pFileBuf,
                                   uint32 bufSize,
                                   bool bPlayVideo,
                                   bool bPlayAudio,
                                   bool bPlayText
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#if defined (FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                                   ,bool bPseudoStream
                                   ,uint32 wBufferOffset
                                        ,uint32 wStartupTime
                                        ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize
                                        ,QtvPlayer::FetchBufferedDataT FetchBufferedData
                                        ,QtvPlayer::InstanceHandleT handle
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
                                )
                :Mpeg4File(filename,
                           pMpeg4Player,
                           pFileBuf,
                           bufSize,
                           bPlayVideo,
                           bPlayAudio,
                           bPlayText
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif
#if defined (FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                      ,bPseudoStream
                      ,wBufferOffset
                           ,wStartupTime
                           ,FetchBufferedDataSize
                           ,FetchBufferedData
                           ,handle
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
                           )
{
  if(_success)
  {
     Mp4FragmentFile::InitData();
  }
}

#ifdef FEATURE_QTV_DRM_DCF
/*===========================================================================

FUNCTION
  Mp4FragmentFile::Mp4FragmentFile

DESCRIPTION
  Constructor for creating mp4fragment file instance for DCF media

DEPENDENCIES
  None

INPUT PARAMETERS:
->inputStream:IxStream*
->bPlayVideo:Indicates if this is video instance
->bPlayAudio:Indicates if this is audio instance
->bPlayText:Indicates if this is text instance

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
Mp4FragmentFile::Mp4FragmentFile(  dcf_ixstream_type inputStream,
                                   Mpeg4Player *pMpeg4Player,
                                   bool bPlayVideo,
                                   bool bPlayAudio,
                                   bool bPlayText
                                   )
                :Mpeg4File(inputStream,
                           pMpeg4Player,
                           bPlayVideo,
                           bPlayAudio,
                           bPlayText)
{
  if(_success)
  {
     InitData();
  }
}
#endif

/*===========================================================================

FUNCTION  InitData

DESCRIPTION
  Initialize the members
===========================================================================*/
void Mp4FragmentFile::InitData()
{
#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
   QCUtils::InitCritSect(&videoFMT_Access_CS);
#endif
  fragmentinfoCount      = 0;
   QCUtils::InitCritSect(&m_trackwrite_CS);
   QCUtils::InitCritSect(&pause_CS);
   AudioPlayerPtr         = NULL;
   VideoPlayerPtr         = NULL;
   /*Make room initially for fragmentInfoArray to hold entires for a DEFAULTFRAGMENTCOUNT(5000) through malloc
   This would avoid numerous realloc of fragmentInfoArray during the playback of a large fragmented file
   that may lead to heap fragmentation*/
   int32 retstat = fragmentInfoArray.MakeRoomFor(DEFAULTFRAGMENTCOUNT);
   if(retstat == -1)
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Can't allocate memory for initial fragmentInfoArray" );
     _fileErrorCode = MEMORY_ALLOCATION_FAILED;
   }
                  }

/*===========================================================================

FUNCTION  getMovieDuration

DESCRIPTION
  Public method to parse the first fragment inside the MP4 file.

===========================================================================*/
uint32 Mp4FragmentFile::getMovieDuration() const
{
  if(m_isFragmentedFile)
    return m_videoFmtInfo.file_info.fragment_file_total_movie_duration;
  else
    return m_videoFmtInfo.file_info.total_movie_duration;
}

/*===========================================================================

FUNCTION  getNextFragTrackMaxBufferSizeDB

DESCRIPTION
  Get the size of the largest track, identified by parameter 'id'
  This function takes care of long overflow by taking MAX.

===========================================================================*/
int32 Mp4FragmentFile::getNextFragTrackMaxBufferSizeDB(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
    if(!p_track->largest)
        p_track->largest = 10*1024; //Defaulting to 10K.

    if ( (p_track->type == VIDEO_FMT_STREAM_AUDIO) &&
         ( (p_track->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_AMR) ||
           (p_track->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_EVRC)||
           (p_track->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_PUREVOICE)))
    {
      return 1024;
    }

    else if ( (p_track->type == VIDEO_FMT_STREAM_VIDEO) &&
              (p_track->subinfo.video.format == VIDEO_FMT_STREAM_VIDEO_H263) )

    {

      //Return the largest sample entry size in sample table and add 3 bytes in case
      // decoder looks forward 3 bytes since the H.263 track atom does not have any decoder specific info
      return MAX( (p_track->largest+3), p_track->largest );

    }
    else
      return p_track->largest;
  }
  else
    return 0;
}

/*===========================================================================

FUNCTION  parsePseudoStreamLocal

DESCRIPTION
  Public method used to parse the fragment

===========================================================================*/
bool Mp4FragmentFile::parsePseudoStreamLocal ( void )
{
  bool returnStatus = true;
  uint32 numFragment = m_currentParseFragment;
  m_currentParseFragment = 0;
  m_minOffsetRequired = 0;

  for(uint i = 0; i <= numFragment; i++)
  {
    m_currentParseFragment = i;
    returnStatus = getMetaDataSize();
  }

  if(returnStatus)
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "parsePseudoStreamLocal: m_bufferOffset=%d, m_minOffsetRequired=%d",
                  m_wBufferOffset, m_minOffsetRequired);

    if((m_wBufferOffset >= m_minOffsetRequired)
        && m_wBufferOffset && m_minOffsetRequired)
    {
       if(!ParseStream())
       {
          returnStatus = false;
          QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "parsePseudoStreamLocal: parsing failed for fragNo=%d, m_mp4ParseLastStatus=%d",
              (m_currentParseFragment-1),m_mp4ParseLastStatus);
       }
    }
    else
    {
        returnStatus = false;
    }
  }
  else
  {
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "parsePseudoStreamLocal: getFragmentSize() failed for fragNo=%d, m_mp4ParseLastStatus=%d",
          m_currentParseFragment,m_mp4ParseLastStatus);
  }
  return returnStatus;
}

/*===========================================================================

FUNCTION  getParseFragmentNum

DESCRIPTION
  Public method used send parser events

===========================================================================*/
uint16 Mp4FragmentFile::getParseFragmentNum( void )
{
    return (uint16)fragmentNumber;
}

/*===========================================================================

FUNCTION  getReadFragmentNum

DESCRIPTION
  Public method used send parser events

===========================================================================*/
uint16 Mp4FragmentFile::getReadFragmentNum( void )
{
    return (uint16)fragmentNumber;
}

uint16 Mp4FragmentFile::m_minTfraRewindLimit = QTV_MPEG4_MIN_TFRA_REW_LIMIT;
/* ======================================================================
FUNCTION
  Mp4FragmentFile::repositionAccessPoint

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
uint32 Mp4FragmentFile::repositionAccessPoint (int32 skipNumber, uint32 id, bool &bError ,uint32 currentPosTimeStampMsec)
{
  bError = FALSE;
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);
  video_fmt_mp4r_context_type  *context;

  if ( !p_track )
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "resetPlayback, unknown track id = %d", id);
    bError = TRUE;
    return 0;
  }

  context = (video_fmt_mp4r_context_type *) (m_videoFmtInfo.server_data);
  if ( !(context->mfra_present) || ((p_track->track_id)!= context->tfra.track_id))
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "resetPlayback, mfra atom is not present, track id = %d", id);
    bError = TRUE;
    return 0;
  }

  uint32 streamNum = p_track->stream_num;

  video_fmt_sample_info_type  sampleInfo;
  memset(&sampleInfo, 0x0, sizeof(video_fmt_sample_info_type));

  uint32 modTS; // initial value

  if ( getAccessPointSampleInfo (p_track, skipNumber, &sampleInfo, &modTS,currentPosTimeStampMsec) )
  {
    if ( m_nextSample[streamNum] == sampleInfo.sample )
    {
      // this is to avoid unnecessary disturbance, when no repositioning is needed
      return modTS;
    }

    m_reposStreamPending |= maskByte[streamNum];
    m_nextReposSample[streamNum] = sampleInfo.sample;
    m_sampleInfo[streamNum] = sampleInfo;
    return modTS;
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Reposition Access Point failed for track id = %d", id);
    // this is to avoid unnecessary disturbance, when repositioning can not be done
    float fTime;
    fTime = (float)m_sampleInfo[streamNum].time / (float)p_track->media_timescale * (float)TIMESCALE_BASE;
    bError = TRUE;
    return (uint32)fTime;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getAccessPointSampleInfo

DESCRIPTION
  Public method used to get the sample corresponding to a given timestamp.

===========================================================================*/
bool Mp4FragmentFile::getAccessPointSampleInfo(video_fmt_stream_info_type *p_track,
                                         int32                       skipNumber,
                                         video_fmt_sample_info_type *sampleInfo,
                                         uint32                     *newTimeStamp,
                                         uint32                     currentPosTimeStampMsec)
{

  boolean fragmentRepositioned = FALSE;
  uint32 streamNum = p_track->stream_num;

  uint32 reqSampleNum = 0;
  bool  iRewind = false;
   bool retStat = false;
  //get the Random Access point info
  video_fmt_tfra_entry_type   tfraEntry;
  *newTimeStamp = 0;

  uint32 currentPosTimeStamp = (uint32)(((float)currentPosTimeStampMsec/1000.0F)*p_track->media_timescale);
  if ( skipNumber < 0 )
  {
    iRewind = true;
  }

  if(iRewind)
  {
      if(p_track->fragment_number == 0)
      {
        /*Rewind back to 0th frame*/
        retStat = (getSampleInfo(streamNum, 0, 1, sampleInfo)>0)?true:false;
        if(!retStat)
        {
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "getAccesPointSampleInfo: getSampleInfo(0th Sample) failed..!");
        }
        return retStat;
      }

      do
      {
        retStat = getTfraEntryInfo(streamNum, skipNumber, iRewind, &tfraEntry,currentPosTimeStamp);
        if(retStat)
        {
            uint32 tfraTimeMsec = (uint32)((float)tfraEntry.access_point_time * 1000.0F / p_track->media_timescale);

            if(tfraTimeMsec + m_minTfraRewindLimit <= currentPosTimeStampMsec)
            {
                break;
            }
            else
            {
                QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "getAccesPointSampleInfo: Rew to a sync pt < %d ,currPlayBack=%d..!",
                    tfraTimeMsec,currentPosTimeStampMsec);
                currentPosTimeStamp = tfraEntry.access_point_time-1;
            }
        }
      }
      while(retStat);

      if((m_iodoneSize[streamNum] == 0) &&
          (m_mp4SyncLastStatus[streamNum] == VIDEO_FMT_IO_DONE))
      {
          /*You have reached the first tfra table entry while rewinding,
            so go back to the 0th frame*/
          retStat = getTimestampedSampleInfo(p_track,0,sampleInfo,newTimeStamp,TRUE,
                                             currentPosTimeStampMsec);
          return retStat;
      }
  }
  else
  {
    retStat = getTfraEntryInfo(streamNum, skipNumber, iRewind, &tfraEntry,currentPosTimeStamp);
  }
  QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "getAccesPointSampleInfo: iRewind=%d, m_sampleInfo.time=%d, tfraEntry.time=%d",
            iRewind, m_sampleInfo[streamNum].time, tfraEntry.access_point_time);

  if(!retStat)
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "getAccesPointSampleInfo: getTfraEntryInfo failed..!");
       return false;
  }

  retStat = findSampleFromTfra(p_track, iRewind, reqSampleNum, &tfraEntry,
                                 fragmentRepositioned);
  if(!retStat)
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "getAccesPointSampleInfo: findSampleFromTfra failed..!");
     // if the inter fragment repositioning failed and the fragment has been repositioned
      //Now reposition to the Original Fragment from where it started//
     (void)findiFrameFragment(p_track, m_sampleInfo[streamNum].sample,!iRewind, FALSE, fragmentRepositioned);
      return false;
  }

  retStat = (getSampleInfo(streamNum, reqSampleNum, 1, sampleInfo)>0)?true:false;
  if(retStat)
  {
      *newTimeStamp = (uint32)((float)sampleInfo->time * 1000.0F / p_track->media_timescale);
      if((!iRewind && (*newTimeStamp < currentPosTimeStampMsec))||(iRewind && (*newTimeStamp > currentPosTimeStampMsec)))
      {
            if(fragmentRepositioned)
            {
               // if the inter fragment repositioning failed and the fragment has been repositioned
               //Now reposition to the Original Fragment from where it started//
               iRewind = (m_sampleInfo[streamNum].sample < reqSampleNum)?true:false;
               (void)findiFrameFragment(p_track, m_sampleInfo[streamNum].sample, iRewind, FALSE, fragmentRepositioned);
            }
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "WRONG getAccesPointSampleInfo: getSampleInfo failed..!");

         return false;
      }
  }
  else
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "getAccesPointSampleInfo: getSampleInfo failed..!");
  }

  return retStat;
}


/*===========================================================================

FUNCTION  getTfraEntryInfo

DESCRIPTION
  Public method used to request a media sample (frame)

===========================================================================*/
bool Mp4FragmentFile::getTfraEntryInfo (uint32 streamNum,
                                  int32  skipNum,
                                  bool   reverse,
                                  video_fmt_tfra_entry_type *buffer,
                                  uint32  currentPosTimeStamp)
{
  int loop = 0;
  uint32 sample_timestamp = currentPosTimeStamp;

  m_videoFmtInfo.access_point_cb( streamNum,
                           sample_timestamp,
                           skipNum,
                           reverse,
                           buffer,
                           m_videoFmtInfo.server_data,
                           mp4SyncStatusCallback,
                           &(m_clientData[streamNum]));

  while ( (m_mp4SyncLastStatus[streamNum] != VIDEO_FMT_IO_DONE) &&
          (m_mp4SyncLastStatus[streamNum] != VIDEO_FMT_FAILURE) &&
          (m_mp4SyncLastStatus[streamNum] != VIDEO_FMT_BUSY) &&
          (m_mp4SyncLastStatus[streamNum] != VIDEO_FMT_DATA_CORRUPT) &&
          (loop < MPEG4_VIDEOFMT_MAX_LOOP) )
  {
    m_mp4SyncContinueCb[streamNum] (m_mp4SyncServerData[streamNum]);
    loop++;
  }

  if( loop >= MPEG4_VIDEOFMT_MAX_LOOP )
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "VideoFMT hangs. StreamNum=%d, skipNum %d", streamNum, skipNum);
  }

  if( (m_mp4SyncLastStatus[streamNum] == VIDEO_FMT_FAILURE) ||
      (m_mp4SyncLastStatus[streamNum] == VIDEO_FMT_BUSY) ||
      (m_mp4SyncLastStatus[streamNum] == VIDEO_FMT_DATA_CORRUPT))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "VideoFMT get Tfra Entry Info failed.");
    return FALSE;
  }

  if(m_iodoneSize[streamNum] == 0)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "VideoFMT get Tfra Entry Info could not find a sample.");
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION  findSampleFromTfra

DESCRIPTION
  Public method used to get the sample corresponding to a given timestamp.

===========================================================================*/
bool Mp4FragmentFile::findSampleFromTfra(video_fmt_stream_info_type *input_track,
                                   bool                       iRewind,
                                   uint32                     &reqSampleNum,
                                   video_fmt_tfra_entry_type  *tfraEntry,
                                   boolean                    &fragmentRepositioned)
{
  int32 length = 0, index = 0;
  uint32 i = 0, input_streamnum;
  fragment_info_type *fragment_info = NULL;
  boolean foundFragment = FALSE, continueParsing = TRUE;
  bool returnValue = false, parseReturn = false;
  video_fmt_stream_info_type  *p_stream_info = NULL, *track = NULL;
  video_fmt_mp4r_stream_type  *p_stream = NULL;

  locateStreamData(&p_stream_info,&p_stream,input_track);

  if(!p_stream_info || !p_stream)
     return false;

  input_streamnum = input_track->stream_num;
  fragmentRepositioned = FALSE;

  if(iRewind)
  {
    if((tfraEntry->access_point_time >= input_track->track_frag_info.first_timestamp) &&
       (tfraEntry->access_point_time < input_track->media_duration))
    {
      // The sample is in the same fragment
      reqSampleNum = 0;

      fragment_info = fragmentInfoArray[input_track->fragment_number-1];
      reqSampleNum = fragment_info->frames[input_streamnum];

      if(reqSampleNum > 0)
        --reqSampleNum;
      for(i = 0 ;i < (uint32)(tfraEntry->trun_number - 1); i++)
      {
        reqSampleNum += p_stream->trun[i].table_size;
      }
      reqSampleNum += tfraEntry->sample_number;
      returnValue = true;
    }
    else
    {
      //The required sample is in a different fragment
      length = fragmentInfoArray.GetLength();
      index = length - 1;
         while( TRUE )
         {
            fragment_info = fragmentInfoArray[index];
            /*tfraEntry will allways point to some fragment other than the
            main fragment.*/
            if(fragment_info->timestamp[input_streamnum] <= tfraEntry->access_point_time)
            {
               fragment_info = fragmentInfoArray[index + 1];
               foundFragment = TRUE;
               break;
            }
            index--;
            if(index < 0)
               break;
         }
      fragmentRepositioned = TRUE;
      if(foundFragment == FALSE)
          return false;

      fragment_info = fragmentInfoArray[index];
      reqSampleNum  =  fragment_info->frames[input_streamnum];
      reinitializeFragmentData(input_track, index, reqSampleNum, iRewind);

         if(reqSampleNum > 0)
            --reqSampleNum;


#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
      if(bHttpStreaming)
      {
        parseReturn = parsePseudoStreamLocal();
      }
      else
#endif /*#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)*/
      {
        parseReturn = parseMetaData();
        if( (parseReturn) && (m_currentParseFragment > 0) )
        {
          //Decrement the current fragment count because we successfully parsed the fragment
          m_currentParseFragment -= 1;
        }
      }
      if(parseReturn)
      {
        QCUtils::EnterCritSect(&m_trackwrite_CS);        
        for(i = 0; i < m_trackCount; i++)
        {
          track = m_track[i];
            locateStreamData(&p_stream_info,&p_stream,track);
          p_stream->fragment_repositioned = TRUE;
        }

        fragmentRepositioned = TRUE;
        returnValue = true;
        QCUtils::LeaveCritSect(&m_trackwrite_CS);
        locateStreamData(&p_stream_info,&p_stream,input_track);
        for(i = 0 ;i < (uint32)(tfraEntry->trun_number - 1); i++)
        {
          reqSampleNum += p_stream->trun[i].table_size;
        }
        reqSampleNum += tfraEntry->sample_number;
      }
      else
      {
        returnValue = false;
        fragmentRepositioned = FALSE;
      }
    }
   //end of REW
  }
  else
  {
    //FFWD
    if((tfraEntry->access_point_time >= input_track->track_frag_info.first_timestamp) &&
       (tfraEntry->access_point_time < input_track->media_duration))
    {
      // The sample is in the same fragment
      reqSampleNum = 0;
      fragment_info = fragmentInfoArray[input_track->fragment_number-1];
      reqSampleNum = fragment_info->frames[input_streamnum];

      if(reqSampleNum > 0)
        --reqSampleNum;
      for(i = 0 ;i < (uint32)(tfraEntry->trun_number - 1); i++)
      {
        reqSampleNum += p_stream->trun[i].table_size;
      }
      reqSampleNum += tfraEntry->sample_number;
      returnValue = true;
    }
    else
    {
         if(!setMainFragmentBytes())
         {
            return FALSE;
         }

         boolean bDoParse = TRUE;
      //required sample is in a different fragment
      //parse and reposition till the end of the file
      while( TRUE )
      {
        length = fragmentInfoArray.GetLength();
        for(index = 0; index < length; index++)
        {
          fragment_info = fragmentInfoArray[index];
          if(fragment_info->timestamp[input_streamnum] > tfraEntry->access_point_time)
          {
            foundFragment = TRUE;
            continueParsing = FALSE;
            break;
          }
          else
          {
            continueParsing = TRUE;
            foundFragment = FALSE;
          }
        }
        if(continueParsing && !m_parsedEndofFile)
        {
        bDoParse = FALSE;
#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
          if(bHttpStreaming)
          {
            m_currentParseFragment = fragment_info->fragment_number + 1;
            parseReturn = parsePseudoStreamLocal();
          }
          else
#endif /*#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM) */
          {
            parseReturn = parseMetaData();
            fragmentRepositioned = TRUE;
            if(parseReturn)
            {
              //Increment the current fragment count because we successfully parsed the fragment
              m_currentParseFragment += 1;
            }
          }
          if(parseReturn != true)
          {
            fragmentRepositioned = FALSE;
            break;
          }         
        }
        else
          break;
      }

      if(foundFragment == FALSE)
        return false;

      //Fragment with I Frame exists
      if(!bDoParse)
      {
        uint32 stream_num = 0;
        for(i = 0; i < m_trackCount; i++)
        {
          track = m_track[i];
          stream_num = track->stream_num;
          locateStreamData(&p_stream_info, &p_stream, track);
          reinitializeFragmentStream(p_stream, fragment_info, index,
                                     stream_num, iRewind);
        }
      }
         else
         {
            reinitializeFragmentData(input_track, index, reqSampleNum, iRewind);
#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
        if(bHttpStreaming)
            {
               m_currentParseFragment = fragment_info->fragment_number;
               parseReturn = parsePseudoStreamLocal();
            }
            else
#endif /*#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)*/
            {
              parseReturn = parseMetaData();
              if(parseReturn)
              {
                //Increment the current fragment count because we successfully parsed the fragment
                m_currentParseFragment += 1;
              }
            }
            if(!parseReturn)
            {
               fragmentRepositioned  = FALSE;
               return FALSE;
            }            
         }

      fragment_info = fragmentInfoArray[index-1];
      reqSampleNum = fragment_info->frames[input_streamnum];

      if(reqSampleNum > 0)
          --reqSampleNum;

      QCUtils::EnterCritSect(&m_trackwrite_CS);
      for(i = 0; i < m_trackCount; i++)
      {
        track = m_track[i];
          locateStreamData(&p_stream_info,&p_stream,track);
        p_stream->fragment_repositioned = TRUE;
      }

      fragmentRepositioned = TRUE;
      returnValue = true;
      QCUtils::LeaveCritSect(&m_trackwrite_CS);
      locateStreamData(&p_stream_info,&p_stream,input_track);
      for(i = 0 ;i < (uint32)(tfraEntry->trun_number - 1); i++)
      {
        reqSampleNum += p_stream->trun[i].table_size;
      }
      reqSampleNum += tfraEntry->sample_number;
    }
    //end of FFWD
  }
  return returnValue;
}

/*===========================================================================

FUNCTION  setMainFragmentBytes

DESCRIPTION
   Private method used set the Main fragment bytes as stsz was not previously
   parsed.

===========================================================================*/
boolean Mp4FragmentFile::setMainFragmentBytes()
{
   for(uint8 trackIndex = 0; trackIndex < m_trackCount; trackIndex++)
   {
      video_fmt_stream_info_type  *p_stream_info = NULL;
      video_fmt_mp4r_stream_type  *p_stream = NULL;
      video_fmt_stream_info_type* track = m_track[trackIndex];

      if( (track->type == VIDEO_FMT_STREAM_VIDEO && m_playVideo) ||
         (track->type == VIDEO_FMT_STREAM_AUDIO && m_playAudio) ||
         (track->type == VIDEO_FMT_STREAM_TEXT && m_playText)
         )
      {
         locateStreamData(&p_stream_info,&p_stream,track);
         if(!p_stream)
         {
            return FALSE;
         }
         if(!p_stream->main_fragment_bytes)
         {
            video_fmt_sample_info_type  lastSampleInfo;
            if(getSampleInfo (track->stream_num,
               p_stream->main_fragment_frames-1,
               1,
               &lastSampleInfo) <= 0)
            {
               return FALSE;
            }
            track->bytes = lastSampleInfo.offset + lastSampleInfo.size;
            p_stream->main_fragment_bytes = track->bytes;
            fragmentInfoArray[(uint8)0]->bytes[track->stream_num] = track->bytes;
         }
      }
   }
   return TRUE;
}

/*===========================================================================

FUNCTION  processFragmentBoundary

DESCRIPTION
  Public method used to request a media sample (frame)

===========================================================================*/
int32 Mp4FragmentFile::processFragmentBoundary(video_fmt_stream_info_type *track)
{
  int32  returnVal = 0;
  uint32 streamNum = track->stream_num;
  video_fmt_mp4r_context_type * VideoFMTContext;
  qtv_fragment_event_fire_info_type FragmentEventSourceInfo;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mp4FragmentFile::processFragmentBoundary" );


#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  if(bHttpStreaming)
  {
    qtv_event_fragment_pb_info_type qtv_event_fragment_pb_complete;
    qtv_event_fragment_pb_complete.fragment_number = (uint16)fragmentNumber;
#ifndef FEATURE_WINCE
    event_report_payload(EVENT_QTV_FRAGMENT_PLAYBACK_COMPLETE,
                        sizeof(qtv_event_fragment_pb_info_type),
                        &qtv_event_fragment_pb_complete);
#endif
  }
#endif

  if(!setMainFragmentBytes())
    return FRAGMENT_CORRUPT;

  if (parseUntilSampleFound(track))
  {
    if(m_isFragmentedFile)
    {
      VideoFMTContext = (video_fmt_mp4r_context_type *)m_videoFmtInfo.server_data;
      if(VideoFMTContext)
      {
        if(track->type == VIDEO_FMT_STREAM_AUDIO)
        {
          FragmentEventSourceInfo.bPlayAudio = 1;
          FragmentEventSourceInfo.bPlayVideo = 0;
        }
        else if(track->type == VIDEO_FMT_STREAM_VIDEO)
        {
          FragmentEventSourceInfo.bPlayAudio = 0;
          FragmentEventSourceInfo.bPlayVideo = 1;
        }
        else if(track->type == VIDEO_FMT_STREAM_TEXT)
        {
          FragmentEventSourceInfo.bPlayAudio = 0;
          FragmentEventSourceInfo.bPlayVideo = 0;
        }

        if(m_mp4ParseLastStatus == VIDEO_FMT_INFO)
        {
          if(VideoFMTContext->current_sequence_number == 1)
          {
            //Throw EVENT_QTV_FRAGMENTED_FILE_DECODE_START
#ifndef FEATURE_WINCE
            event_report_payload(EVENT_QTV_FRAGMENTED_FILE_DECODE_START,
                                 sizeof(qtv_fragment_event_fire_info_type),
                                 &FragmentEventSourceInfo);
#endif
          }
        }//end of if(m_mp4ParseLastStatus == VIDEO_FMT_INFO)

        else if(m_mp4ParseLastStatus == VIDEO_FMT_FRAGMENT)
        {
          if(VideoFMTContext->current_sequence_number == 1)
          {
            //Throw EVENT_QTV_FRAGMENTED_FILE_DECODE_START
#ifndef FEATURE_WINCE
            event_report_payload(EVENT_QTV_FRAGMENTED_FILE_DECODE_START,
                                 sizeof(qtv_fragment_event_fire_info_type),
                                 &FragmentEventSourceInfo);
#endif
          }
          else if(VideoFMTContext->current_sequence_number > 1)
          {
            qtv_event_fragment_pb_info_type qtv_event_fragment_pb_complete;
            qtv_event_fragment_pb_complete.fragment_number = (uint16)VideoFMTContext->current_sequence_number;
#ifndef FEATURE_WINCE
            event_report_payload(EVENT_QTV_FRAGMENT_PLAYBACK_COMPLETE,
                                 sizeof(qtv_event_fragment_pb_info_type),
                                 &qtv_event_fragment_pb_complete);
#endif
          }
        }//end of if(m_mp4ParseLastStatus == VIDEO_FMT_FRAGMENT)
       }//end of if(VideoFMTContext)
     }//end of if(m_isFragmentedFile)

    returnVal = getSampleInfo (streamNum,
                               m_nextSample[streamNum],
                               1,
                               &m_sampleInfo[streamNum]);
    if (returnVal <= 0)
    {
      return 0;
    }
    else
    {
#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
      if(bHttpStreaming)
      {
        qtv_event_fragment_pb_info_type qtv_event_fragment_pb_begin;
        qtv_event_fragment_pb_begin.fragment_number = (uint16)fragmentNumber;
#ifndef FEATURE_WINCE
        event_report_payload(EVENT_QTV_FRAGMENT_PLAYBACK_BEGIN,
                            sizeof(qtv_event_fragment_pb_info_type),
                            &qtv_event_fragment_pb_begin);
#endif
      }
#endif
      return returnVal;
    }
  }
  else
  {
    if(m_parsedEndofFile)
    {
       return(0);
    }
    else
    {
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mp4FragmentFile->Ret:FRAGMENT_BOUNDARY A:%d V:%d",m_playAudio,m_playVideo);
        return FRAGMENT_BOUNDARY;
    }
  }
}

/*===========================================================================

FUNCTION  parseUntilSampleFound

DESCRIPTION
  Public method used to switch contexts and call the parseFragment event

===========================================================================*/
boolean Mp4FragmentFile::parseUntilSampleFound (video_fmt_stream_info_type *track)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, " Mp4FragmentFile::parseUntilSampleFound" );
  if(!m_parsedEndofFile)
  {
#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
    if(bHttpStreaming)
    {
      //keep parsing the next fragment (if present) until
      //u find audio/video/text frame || u reach the EndOfFile
      //If u dont have sufficeint data to parse then pauseTrack.

      if(track->type == VIDEO_FMT_STREAM_AUDIO)
      {
        do
        {
          //Handling a special case when u had to pause: when
          //u knew the fragment size but did not have sufficient data
          //to continue fragment parsing.
          if(m_mp4ParseLastStatus == VIDEO_FMT_FRAGMENT_SIZE)
            bGetMetaDataSize = FALSE;
          else
            bGetMetaDataSize = TRUE;

          if(!ParseStream())
          {
            m_hasAudio = false;
            if((m_wBufferOffset < m_minOffsetRequired) ||
               (m_wBufferOffset >= m_minOffsetRequired && bDataIncomplete))
            {
              sendParserEvent(Common::PARSER_PAUSE);
            }
            break;
          }
        }while(!m_hasAudio && !m_parsedEndofFile);

        return m_hasAudio;

      }
      else if(track->type == VIDEO_FMT_STREAM_VIDEO)
      {
        do
        {
          if(m_mp4ParseLastStatus == VIDEO_FMT_FRAGMENT_SIZE)
            bGetMetaDataSize = FALSE;
          else
            bGetMetaDataSize = TRUE;

          if(!ParseStream())
          {
            m_hasVideo = false;
            if((m_wBufferOffset < m_minOffsetRequired) ||
               (m_wBufferOffset >= m_minOffsetRequired && bDataIncomplete))
            {
              sendParserEvent(Common::PARSER_PAUSE);
            }
            break;
          }
        }while(!m_hasVideo && !m_parsedEndofFile);

        return m_hasVideo;
      }
      else if(track->type == VIDEO_FMT_STREAM_TEXT)
      {
        do
        {
          if(m_mp4ParseLastStatus == VIDEO_FMT_FRAGMENT_SIZE)
            bGetMetaDataSize = FALSE;
          else
            bGetMetaDataSize = TRUE;

          if(!ParseStream())
          {
            m_hasText = false;
            //Pause the track..insufficient download data
            if((m_wBufferOffset < m_minOffsetRequired) ||
               (m_wBufferOffset >= m_minOffsetRequired && bDataIncomplete))
            {
              sendParserEvent(Common::PARSER_PAUSE);
            }
            break;
          }
        }while(!m_hasText && !m_parsedEndofFile);

        return m_hasText;
      }
    }
    else
#endif /*#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)*/
    {
      //keep parsing the next fragment until u find
      //audio/video/text frame || u reach the EndOfFile
      if(track->type == VIDEO_FMT_STREAM_AUDIO)
      {
        do
        {
          if(!ParseStream())
          {
            m_hasAudio = false;
            break;
          }
        }while(!m_hasAudio && !m_parsedEndofFile);

        return m_hasAudio;
      }
      else if (track->type == VIDEO_FMT_STREAM_VIDEO)
      {
        do
        {
          if(!ParseStream())
          {
            m_hasVideo = false;
            break;
          }
        }while(!m_hasVideo && !m_parsedEndofFile);

        return m_hasVideo;
      }
      else if (track->type == VIDEO_FMT_STREAM_TEXT)
      {
        do
        {
          if(!ParseStream())
          {
            m_hasText = false;
            break;
          }
        }while(!m_hasText && !m_parsedEndofFile);

        return m_hasText;
      }
    }
  }//!m_parsedEndofFile

  return FALSE;
  //If u return FALSE and if(!m_parsedEndofFile) then the calling thread will come
  //againg after COMMON_FRAGMENT_WAIT_TIMEOUT_MSEC, to check if a new sample is available.
}

/*===========================================================================

FUNCTION  postMessage

DESCRIPTION
  Public method used for Event to Message translation routines.

===========================================================================*/
void Mp4FragmentFile::postMessage(QCMessageType *pEvent)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mp4FragmentFile::PostEvent" );

  QCUtils::PostMessage(pEvent, 0, NULL);
}

/*===========================================================================

FUNCTION  locateStreamData

DESCRIPTION
  Method used to find the stream context

===========================================================================*/
void Mp4FragmentFile::locateStreamData(
                video_fmt_stream_info_type  **p_stream_info,
                video_fmt_mp4r_stream_type  **p_stream,
                video_fmt_stream_info_type  *input_track)
{
  video_fmt_mp4r_context_type *video_fmt_context = NULL;
  uint32 i = 0;

  video_fmt_context = (video_fmt_mp4r_context_type *)m_videoFmtInfo.server_data;

  if(video_fmt_context)
  {
    for(i = 0; i < m_videoFmtInfo.num_streams; i++)
    {
      if((input_track->track_id == video_fmt_context->stream_info[i].track_id)
          && (input_track->stream_num == video_fmt_context->stream_info[i].stream_num)
          && (input_track->type == video_fmt_context->stream_info[i].type))
      {
        *p_stream_info = video_fmt_context->stream_info + i;
        *p_stream = video_fmt_context->stream_state + i;
        break;
      }
    }
  }
}


/*===========================================================================

FUNCTION  setAudioPlayerData

DESCRIPTION

===========================================================================*/
void Mp4FragmentFile::setAudioPlayerData( const void *client_data )
{
  AudioPlayerPtr = (AudioPlayer *)client_data;
}

/*===========================================================================

FUNCTION  setVideoPlayerData

DESCRIPTION

===========================================================================*/
void Mp4FragmentFile::setVideoPlayerData( const void *client_data )
{
  VideoPlayerPtr = (VideoPlayer *)client_data;
}

/*===========================================================================

FUNCTION  setTextPlayerData

DESCRIPTION

===========================================================================*/
void Mp4FragmentFile::setTextPlayerData( const void *client_data )
{
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  TextPlayerPtr = (TimedText *)client_data;
#endif
}

/*===========================================================================

FUNCTION  findiFrameFragment

DESCRIPTION
  Public method used to get the sample corresponding to a given timestamp.

===========================================================================*/
bool Mp4FragmentFile::findiFrameFragment(video_fmt_stream_info_type *input_track,
                                   uint32                     reqSampleNum,
                                   bool                       iRewind,
                                   boolean                    findiFrame,
                                   boolean                    &fragmentParsed)
{
  int32 length = 0, index = 0;
  uint32 i = 0, input_streamnum;
  fragment_info_type *fragment_info = NULL;
  boolean foundFragment = FALSE, continueParsing = TRUE;
  bool returnValue = false, parseReturn = false;
  video_fmt_stream_info_type  *p_stream_info = NULL, *track = NULL;
  video_fmt_mp4r_stream_type  *p_stream = NULL;

  if(!m_isFragmentedFile)
     return false;

  locateStreamData(&p_stream_info,&p_stream,input_track);

  if(!p_stream_info || !p_stream)
     return false;

  input_streamnum = input_track->stream_num;
  fragmentParsed = FALSE;

  if(iRewind)
  {
      length = fragmentInfoArray.GetLength();
      index = length - 1;
         while( TRUE )
         {
            fragment_info = fragmentInfoArray[index];
            if((fragment_info->fragment_number == 0) &&
               ((reqSampleNum + 1) <= fragment_info->frames[input_streamnum]))
            {
               if(fragment_info->fragment_i_frame[input_streamnum] || !findiFrame)
                  foundFragment = TRUE;
               break;
            }
            else if(fragment_info->frames[input_streamnum] < (reqSampleNum + 1))
            {
               fragment_info = fragmentInfoArray[index + 1];
               if((fragment_info) && (fragment_info->fragment_i_frame[input_streamnum] || !findiFrame))
               {
                  foundFragment = TRUE;
               }
               else if(fragmentInfoArray[index]->fragment_number == 0)
               {
                  reqSampleNum = 0;
                  if(fragmentInfoArray[index]->fragment_i_frame[input_streamnum] || !findiFrame)
                     foundFragment = TRUE;
               }
               if(foundFragment)
                  break;
            }
            index--;
            if(index < 0)
               break;
         }
         if(foundFragment == FALSE)
            return false;

      reinitializeFragmentData(input_track, index, reqSampleNum, iRewind);

#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
    if(bHttpStreaming)
      {
        parseReturn = parsePseudoStreamLocal();
      }
      else
#endif /*#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)*/
      {
      parseReturn = parseMetaData();
        if( (parseReturn) && (m_currentParseFragment > 0) )
        {
          //Decrement the current fragment count because we successfully parsed the fragment
          m_currentParseFragment -= 1;
        }
      }
      if(parseReturn)
      {
        QCUtils::EnterCritSect(&m_trackwrite_CS);          
        fragmentParsed = TRUE;
        for(i = 0; i < m_trackCount; i++)
        {
          track = m_track[i];
          locateStreamData(&p_stream_info,&p_stream,track);
          p_stream->fragment_repositioned = TRUE;
        }
        returnValue = true;
        QCUtils::LeaveCritSect(&m_trackwrite_CS);
      }
      else
      {
        returnValue = false;
        fragmentParsed = FALSE;
      }
  }
  else
  {
    //FFWD
    if(!setMainFragmentBytes())
    {
        fragmentParsed = FALSE;
        return FALSE;
    }

    boolean bDoParse = TRUE;
    //parse and reposition till the end of the file
    while(TRUE)
    {
      length = fragmentInfoArray.GetLength();
      for(index = 0; index < length; index++)
      {
        fragment_info = fragmentInfoArray[index];
        if(fragment_info->frames[input_streamnum] >= (reqSampleNum + 1))
        {
          QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "FFWD: fragment_info->frames = %d reqSampleNum + 1 = %d",
                                  fragment_info->frames[input_streamnum],
                                  (reqSampleNum + 1));
          if((fragment_info->fragment_i_frame[input_streamnum]) || (!findiFrame))
          {
             foundFragment = TRUE;
             continueParsing = FALSE;
          }
          else
          {
              continueParsing = TRUE;
              reqSampleNum = fragment_info->frames[input_streamnum];
          }
          break;
        }
        else
        {
          continueParsing = TRUE;
          foundFragment = FALSE;
        }
      }

      if(continueParsing && !m_parsedEndofFile)
      {
        bDoParse = FALSE;
#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
        if(bHttpStreaming)
        {
          m_currentParseFragment = fragment_info->fragment_number + 1;
          parseReturn = parsePseudoStreamLocal();
        }
        else
#endif /*#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)*/
        {
          parseReturn = parseMetaData();
          if(parseReturn)
          {
	    //Increment the current fragment count because we successfully parsed the fragment
            m_currentParseFragment += 1;
          }
        }
        if(parseReturn == true)
        {          
          fragmentParsed = TRUE;
          if(m_parsedEndofFile && (input_track->track_frag_info.first_frame!=0) && (reqSampleNum >= input_track->frames) )
          {
            reqSampleNum  = input_track->frames - 1;
          }
        }
        else
        {
            break;
        }
      }
      else
        break;
    }

    if(foundFragment == FALSE)
      return false;

    if(!bDoParse)
    {
      uint32 stream_num = 0;
      for(i = 0; i < m_trackCount; i++)
      {
        track = m_track[i];
        stream_num = track->stream_num;
        locateStreamData(&p_stream_info, &p_stream, track);
        reinitializeFragmentStream(p_stream, fragment_info, index,
                        stream_num, iRewind);
      }
    }
   //Fragment with I Frame exists
      else
      {
         reinitializeFragmentData(input_track, index, reqSampleNum, iRewind);
#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
      if(bHttpStreaming)
         {
            m_currentParseFragment = fragment_info->fragment_number;
            parseReturn = parsePseudoStreamLocal();
         }
         else
#endif /*#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)*/
         {
           parseReturn = parseMetaData();
           if(parseReturn)
           {
             //Increment the current fragment count because we successfully parsed the fragment
             m_currentParseFragment += 1;
           }
         }
      if(!parseReturn)
      {
            fragmentParsed = FALSE;
            return FALSE;
         }         
      }

    QCUtils::EnterCritSect(&m_trackwrite_CS);
    for(i = 0; i < m_trackCount; i++)
    {
      track = m_track[i];
     locateStreamData(&p_stream_info,&p_stream,track);
      p_stream->fragment_repositioned = TRUE;
    }
    returnValue = true;
    QCUtils::LeaveCritSect(&m_trackwrite_CS);
  }
  return returnValue;
}

/*===========================================================================

FUNCTION  reinitializeFragmentData

DESCRIPTION
  Public method used to get the sample corresponding to a given timestamp.

===========================================================================*/
void Mp4FragmentFile::reinitializeFragmentData(video_fmt_stream_info_type *input_track,
                                         uint32                     fragment_infoindex,
                                         uint32                     reqSampleNum,
                                         bool                       iRewind)
{
  fragment_info_type *fragment_info = NULL, *next_fragment_info = NULL, *previous_fragment_info = NULL;
  uint32 i, stream_num = 0;

  video_fmt_stream_info_type  *p_stream_info = NULL, *temp_track = NULL;
  video_fmt_mp4r_stream_type  *p_stream = NULL;

  video_fmt_mp4r_context_type *video_fmt_context = NULL;

  video_fmt_context = (video_fmt_mp4r_context_type *)m_videoFmtInfo.server_data;
  stream_num = input_track->stream_num;

  if(iRewind)
  {
    fragment_info = fragmentInfoArray[fragment_infoindex];

    if((fragment_info->fragment_number == 0) &&
       ((reqSampleNum + 1) <= fragment_info->frames[stream_num]))
    {
      //Rewind to first Fragment
      video_fmt_context->abs_pos = 0;
      video_fmt_context->in_buffer_size = 0;
      video_fmt_context->num_streams = 0;
      video_fmt_context->atom_stack_top = 0;

      for(i = 0; i < m_trackCount; i++)
      {
        temp_track = m_track[i];
        stream_num = temp_track->stream_num;
        locateStreamData(&p_stream_info, &p_stream, temp_track);
        reinitializeFragmentStream(p_stream, NULL, fragment_infoindex, stream_num, iRewind);
        reinitializeFragmentStreamInfo(p_stream_info, NULL, stream_num);
      }

#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
      if(bHttpStreaming)
      {
        m_currentParseFragment = 0;
      }
#endif /*#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)*/
    }
    else
    {
      next_fragment_info = fragmentInfoArray[fragment_infoindex + 1];
      if(next_fragment_info)
      {
        video_fmt_context->abs_pos = next_fragment_info->fragment_offset;
      }
      video_fmt_context->in_buffer_size = 0;

      for(i = 0; i < m_trackCount; i++)
      {
        temp_track = m_track[i];
        stream_num = temp_track->stream_num;
        locateStreamData(&p_stream_info, &p_stream, temp_track);
        reinitializeFragmentStream(p_stream, fragment_info, fragment_infoindex,
                              stream_num, iRewind);
        reinitializeFragmentStreamInfo(p_stream_info, fragment_info, stream_num);
      }
#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
      if(bHttpStreaming)
      {
        if(next_fragment_info)
        {
          m_currentParseFragment = next_fragment_info->fragment_number;
        }
      }
#endif /*#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)*/
    }
  }
  else
  {
    //FFWD
    fragment_info = fragmentInfoArray[fragment_infoindex];
    previous_fragment_info = fragmentInfoArray[fragment_infoindex - 1];
    if(previous_fragment_info && fragment_info)
    {
      for(i = 0; i < m_trackCount; i++)
      {
        temp_track = m_track[i];
        stream_num = temp_track->stream_num;
        locateStreamData(&p_stream_info, &p_stream, temp_track);
        reinitializeFragmentStream(p_stream, fragment_info, fragment_infoindex,
                        stream_num, iRewind);
        reinitializeFragmentStreamInfo(p_stream_info, previous_fragment_info, stream_num);
      }
      video_fmt_context->abs_pos = fragment_info->fragment_offset;
      video_fmt_context->in_buffer_size = 0;
#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)
      if(bHttpStreaming)
      {
        m_currentParseFragment = fragment_info->fragment_number;
      }
#endif /*#if defined( FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined(FEATURE_QTV_PSEUDO_STREAM)*/
    }
  }
}

/*===========================================================================

FUNCTION  reinitializeFragmentStream

DESCRIPTION
  Public method used to get the sample corresponding to a given timestamp.

===========================================================================*/
void Mp4FragmentFile::reinitializeFragmentStream(video_fmt_mp4r_stream_type  *input_stream,
                                           fragment_info_type          *fragment_info,
                                           uint32                      fragment_infoindex,
                                           uint32                      stream_num,
                                           bool                        iRewind)
{
  fragment_info_type          *previous_fragment_info = NULL;

  input_stream->current_trun = 0;

  if(!fragment_info)
  {
    input_stream->last_fragment_frames = 0;
    input_stream->last_fragment_bytes = 0;
    input_stream->last_fragment_timestamp = 0;
    input_stream->sample_byte_offset = 0;
    input_stream->sample_timestamp = 0;
    input_stream->sample_delta_count = 0;
    input_stream->sample_ctts_offset_count = 0;
    input_stream->last_sample_offset = 0;
    // Need to Initialize the tables because the repositioning
    /* in VideoFMT in main fragment is done by counting down the samples
    */
    input_stream->stsz.current_table_pos = 0;
    input_stream->stts.current_table_pos = 0;
    input_stream->stss.current_table_pos = 0;
    input_stream->stsc_info.current_table_pos = 0;
    input_stream->stsc.current_table_pos = 0;
    input_stream->stco.current_table_pos = 0;
    input_stream->stsz.cache_size = 0;
    input_stream->stts.cache_size = 0;
    input_stream->stss.cache_size = 0;
    input_stream->stsc_info.cache_size = 0;
    input_stream->stsc.cache_size = 0;
    input_stream->stco.cache_size = 0;
  }
  else
  {
    if(iRewind)
    {
      if(fragment_info->fragment_number == 0)
      {
        input_stream->last_fragment_frames = 0;
        input_stream->last_fragment_bytes = 0;
        input_stream->last_fragment_timestamp = 0;
        // Need to Initialize the tables because the repositioning
        /* in VideoFMT in main fragment is done by counting down the samples
        */
        input_stream->stsz.current_table_pos = 0;
        input_stream->stts.current_table_pos = 0;
        input_stream->stss.current_table_pos = 0;
        input_stream->stsc_info.current_table_pos = 0;
        input_stream->stsc.current_table_pos = 0;
        input_stream->stco.current_table_pos = 0;
        input_stream->stsz.cache_size = 0;
        input_stream->stts.cache_size = 0;
        input_stream->stss.cache_size = 0;
        input_stream->stsc_info.cache_size = 0;
        input_stream->stsc.cache_size = 0;
        input_stream->stco.cache_size = 0;
      }
      else
      {
        input_stream->last_fragment_frames = fragment_info->frames[stream_num] -
                      input_stream->main_fragment_frames;
        input_stream->last_fragment_bytes = fragment_info->bytes[stream_num] -
                      input_stream->main_fragment_bytes;
        input_stream->last_fragment_timestamp = fragment_info->timestamp[stream_num] -
                      input_stream->main_fragment_timestamp;
      }

      input_stream->sample_byte_offset = fragment_info->bytes[stream_num];
      input_stream->sample_timestamp = fragment_info->timestamp[stream_num];
      input_stream->last_sample_offset = fragment_info->frames[stream_num] - 1;
    }
    else
    {
      //FFWD
      if(fragment_info->fragment_number == 0)
      {
        input_stream->last_fragment_frames = 0;
        input_stream->last_fragment_bytes = 0;
        input_stream->last_fragment_timestamp = 0;
        input_stream->sample_byte_offset = 0;
        input_stream->sample_timestamp = 0;
        input_stream->last_sample_offset = 0;
      }
      else
      {
        previous_fragment_info = fragmentInfoArray[fragment_infoindex - 1];
        if(previous_fragment_info)
        {
          if(previous_fragment_info->fragment_number == 0)
          {
            input_stream->last_fragment_frames = 0;
            input_stream->last_fragment_bytes = 0;
            input_stream->last_fragment_timestamp = 0;
          }
          else
          {
            input_stream->last_fragment_frames =  previous_fragment_info->frames[stream_num] -
                      input_stream->main_fragment_frames;
            input_stream->last_fragment_bytes =  previous_fragment_info->bytes[stream_num] -
                      input_stream->main_fragment_bytes;
            input_stream->last_fragment_timestamp =  previous_fragment_info->timestamp[stream_num] -
                      input_stream->main_fragment_timestamp;
          }
          input_stream->sample_byte_offset =
                  previous_fragment_info->bytes[stream_num];
          input_stream->sample_timestamp =
                  previous_fragment_info->timestamp[stream_num];
          input_stream->last_sample_offset =
                  previous_fragment_info->frames[stream_num] - 1;
        }
      }
    } //(iRewind)
  }//(!fragment_info)
}

/*===========================================================================

FUNCTION  reinitializeFragmentStreamInfo

DESCRIPTION
  Public method used to get the sample corresponding to a given timestamp.

===========================================================================*/
void Mp4FragmentFile::reinitializeFragmentStreamInfo(video_fmt_stream_info_type  *input_streaminfo,
                                               fragment_info_type          *fragment_info,
                                                uint32                      stream_num)
{
  if(!fragment_info)
  {
    input_streaminfo->frames = 0;
    input_streaminfo->bytes = 0;
    input_streaminfo->media_duration = 0;
    input_streaminfo->fragment_number = 0;
    input_streaminfo->fragment_offset = 0;
  }
  else
  {
    input_streaminfo->frames = fragment_info->frames[stream_num];
    input_streaminfo->bytes = fragment_info->bytes[stream_num];
    input_streaminfo->media_duration = fragment_info->timestamp[stream_num];
    input_streaminfo->fragment_number = fragment_info->fragment_number;
    input_streaminfo->fragment_offset = fragment_info->fragment_offset;
  }
}

/*===========================================================================

FUNCTION  getSampleInfoError

DESCRIPTION
  Private method called from getNextMediaSample() in the base class,
  when a fragment boundary is reached.

===========================================================================*/

int32 Mp4FragmentFile::getSampleInfoError(video_fmt_stream_info_type *p_track)
{
   int32 returnVal = 0;
   qtv_fragment_event_fire_info_type FragmentEventSourceInfo;
   QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,"In getSampleInfoError %d %d",
    m_isFragmentedFile,m_corruptFile);

   if (m_isFragmentedFile && !m_corruptFile)
   {
      returnVal = processFragmentBoundary(p_track);
      if(returnVal <= 0)
      {
         if( (m_mp4ParseLastStatus == VIDEO_FMT_INFO) && (m_isFragmentedFile) && (returnVal == 0))
         {
            if(p_track->type == VIDEO_FMT_STREAM_AUDIO)
            {
               FragmentEventSourceInfo.bPlayAudio = 1;
               FragmentEventSourceInfo.bPlayVideo = 0;
            }
            else if(p_track->type == VIDEO_FMT_STREAM_VIDEO)
            {
               FragmentEventSourceInfo.bPlayAudio = 0;
               FragmentEventSourceInfo.bPlayVideo = 1;
            }
            else if(p_track->type == VIDEO_FMT_STREAM_TEXT)
            {
               FragmentEventSourceInfo.bPlayAudio = 0;
               FragmentEventSourceInfo.bPlayVideo = 0;
            }
#ifndef FEATURE_WINCE
            event_report_payload(EVENT_QTV_FRAGMENTED_FILE_END_SUCCESS,
               sizeof(qtv_fragment_event_fire_info_type),
               &FragmentEventSourceInfo);
#endif
         }
         return returnVal;
      }
   }
   return returnVal;
}

/*===========================================================================

FUNCTION  process_video_fmt_info

DESCRIPTION
  Private method called from mp4ParseStatus() in the base class.

===========================================================================*/
void Mp4FragmentFile::process_video_fmt_info(video_fmt_status_type status,
                                video_fmt_status_cb_info_type *info)
{
   Mpeg4File::process_video_fmt_info(status,info);

   //Adding fragment information.
    video_fmt_stream_info_type *p_track = NULL;
    fragment_info_type *fragment_info = NULL;
    boolean addfraginfo = TRUE, addData = FALSE;
   uint32 index=0;

   for (index = 0; index < m_videoFmtInfo.num_streams; index++)
   {
      p_track = m_videoFmtInfo.streams + index;
      addfraginfo = TRUE;

      if ((p_track != NULL) &&
         (((p_track->type == VIDEO_FMT_STREAM_VIDEO) && m_playVideo) ||
         ((p_track->type == VIDEO_FMT_STREAM_AUDIO) && m_playAudio) ||
         ((p_track->type == VIDEO_FMT_STREAM_TEXT) && m_playText))
         )
      {
         for (uint32 i = 0; i < fragmentinfoCount; i++)
         {
            fragment_info = fragmentInfoArray[i];
            if(p_track->frames == fragment_info->frames[p_track->stream_num])
            {
               addfraginfo = FALSE;
               break;
            }
         }
         if(addfraginfo)
         {
            addData = TRUE;
            fragment_info_type *frag_info_entry =
               (fragment_info_type*)QTV_Malloc(sizeof(fragment_info_type));
                                if(frag_info_entry == NULL)
                                {
                                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Can't allocate frag_info_entry" );
                          _fileErrorCode = MEMORY_ALLOCATION_FAILED;
                          addData = FALSE;
                                  break;
                                }
            memset(frag_info_entry, 0x0, sizeof(fragment_info_type));
            frag_info_entry->fragment_number = p_track->fragment_number;
            frag_info_entry->fragment_offset = p_track->fragment_offset;
            int32 oldsize = fragmentInfoArray.GetLength();
            fragmentInfoArray += frag_info_entry;
            int32 newsize = fragmentInfoArray.GetLength();
            /*Check for the newsize and the oldsize of the fragmentInfoArray after memory reallocarion*/
            if(newsize<=oldsize)
            {
              QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "fragmentInfoArray += frag_info_entry failed newsize %d oldsize %d",newsize,oldsize );
                  addData = FALSE;
              _fileErrorCode = MEMORY_ALLOCATION_FAILED;
                QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "setting _fileErrorCode = MEMORY_ALLOCATION_FAILED Mp4FragmentFile::process_video_fmt_info");
                  break;
            }
            fragmentinfoCount++;
            break;
         }
      }
   }

    for (index = 0; index < m_videoFmtInfo.num_streams; index++)
    {
      p_track = m_videoFmtInfo.streams + index;
      if (p_track != NULL)
      {
            switch ( p_track->type )
            {
            case VIDEO_FMT_STREAM_VIDEO:
               if(m_playVideo)
               {
                  m_hasVideo = false;
                  if(addData)
                  {
                     fragment_info = fragmentInfoArray[fragmentinfoCount - 1];
                     fragment_info->fragment_i_frame[p_track->stream_num] =
                        p_track->inter_frames;
                     fragment_info->frames[p_track->stream_num] =
                        p_track->frames;
                     fragment_info->bytes[p_track->stream_num] =
                        p_track->bytes;
                     fragment_info->timestamp[p_track->stream_num] =
                        p_track->media_duration;
                     m_hasVideo = true;
                  }
                  else
                  {
                     //Check if the fragment is in the fragmentInfoArray
                     for (uint32 i = 0; i < fragmentinfoCount; i++)
                     {
                        fragment_info = fragmentInfoArray[i];
                        if((fragment_info->fragment_number == p_track->fragment_number))
                        {
                           m_hasVideo = true;
                           break;
                        }
                     }
                  }
                        if(fragmentinfoCount)
                  {
                  QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "FragInfo: fNo=%d, fFrames=%d, fIFrame=%d", fragment_info->fragment_number,
                     fragment_info->frames[p_track->stream_num],fragment_info->fragment_i_frame[p_track->stream_num]);
                  QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "FragInfo: fOffset=%d, fBytes=%d, fTs=%d", fragment_info->fragment_offset,
                     fragment_info->bytes[p_track->stream_num],fragment_info->timestamp[p_track->stream_num]);
               }
               }
               break;

            case VIDEO_FMT_STREAM_AUDIO:
               if(m_playAudio)
               {
                  m_hasAudio = false;
                  if(addData)
                  {
                     fragment_info = fragmentInfoArray[fragmentinfoCount - 1];
                     fragment_info->fragment_i_frame[p_track->stream_num] =
                        TRUE;
                     fragment_info->frames[p_track->stream_num] =
                        p_track->frames;
                     fragment_info->bytes[p_track->stream_num] =
                        p_track->bytes;
                     fragment_info->timestamp[p_track->stream_num] =
                        p_track->media_duration;
                     m_hasAudio = true;
                  }
                  else
                  {
                     //Check if the fragment is in the fragmentInfoArray
                     for (uint32 i = 0; i < fragmentinfoCount; i++)
                     {
                        fragment_info = fragmentInfoArray[i];
                        if((fragment_info->fragment_number == p_track->fragment_number))
                        {
                           m_hasAudio = true;
                           break;
                        }
                     }
                  }
               }
               break;

            case VIDEO_FMT_STREAM_TEXT:
               if(m_playText)
               {
                  m_hasText = false;
                  if(addData)
                  {
                     fragment_info = fragmentInfoArray[fragmentinfoCount - 1];
                     fragment_info->fragment_i_frame[p_track->stream_num] = TRUE;
                     fragment_info->frames[p_track->stream_num] =
                        p_track->frames;
                     fragment_info->bytes[p_track->stream_num] =
                        p_track->bytes;
                     fragment_info->timestamp[p_track->stream_num] =
                        p_track->media_duration;
                     m_hasText = true;
                  }
                  else
                  {
                     //Check if the fragment is in the fragmentInfoArray
                     for (uint32 i = 0; i < fragmentinfoCount; i++)
                     {
                        fragment_info = fragmentInfoArray[i];
                        if((fragment_info->fragment_number == p_track->fragment_number))
                        {
                           m_hasText = true;
                           break;
                        }
                     }
                  }
               }
               break;

            default:
               break;
            }
      }
    }
}


/*===========================================================================

FUNCTION  ~Mp4FragmentFile

DESCRIPTION
  Destructor for the Mp4FragmentFile class

===========================================================================*/
Mp4FragmentFile::~Mp4FragmentFile()
{
  uint32 i;

  for (i = 0; i < fragmentinfoCount; i++)
  {
    if((fragmentInfoArray)[i] &&
       (fragmentInfoArray)[i] != NULL)
    {
      QTV_Free(fragmentInfoArray[i]);
      (fragmentInfoArray)[i] = NULL;
    }

  }
}

/*===========================================================================

FUNCTION  getSampleAtTimestamp

DESCRIPTION
  Public method used to get the sample info for a sample placed at a given
  timestamp (in the track timescale).

===========================================================================*/

bool Mp4FragmentFile::getSampleAtTimestamp(video_fmt_stream_info_type *p_track,
                                     uint32                      timestamp,
                                     bool                        iRewind,
                                     video_fmt_sample_info_type *sampleInfo)
{
  bool retStat = false;

  uint32 streamNum = p_track->stream_num;
  int32 curSample = (int32)m_sampleInfo[streamNum].sample;

  int32 reqSampleNum;
  boolean fragmentParsed = FALSE;

  if ( iRewind )
  {
    if(m_sampleInfo[streamNum].sample >= p_track->frames)
    {
      //has been repositioned to an earlier fragment
      //actual frames go from 0 ....; p_track->frames go from 1 ....
      m_sampleInfo[streamNum].sample = p_track->frames - 1;
    }
    for ( reqSampleNum = m_sampleInfo[streamNum].sample;
         reqSampleNum >= 0; --reqSampleNum )
    {
      if ( m_isFragmentedFile && (reqSampleNum < p_track->track_frag_info.first_frame) )
      {
          //parse to the previous fragment..
          if(!findiFrameFragment(p_track, reqSampleNum, iRewind, FALSE, fragmentParsed))
          {
              break;
          }
          else
          {
              m_parsedEndofFile = false;
          }
      }
      if ( getSampleInfo(streamNum, reqSampleNum, 1, sampleInfo) > 0 )
        retStat = true;

      if ( sampleInfo->time == timestamp )
      {
        break;
      }
      if (( sampleInfo->time < timestamp ))
      {
        /* For Video, don't rewind before given time stamp, also in rewind, don't increase samples */
        if( (curSample > reqSampleNum) &&
            (p_track->type == VIDEO_FMT_STREAM_VIDEO) )
        {
          (void)getSampleInfo(streamNum, reqSampleNum+1, 1, sampleInfo);
        }
        break;
      }
    } // for
  } // rewind
  else
  {   // forward
    if(m_sampleInfo[streamNum].sample < p_track->track_frag_info.first_frame)
    {
      //has been repositioned to a later fragment
      m_sampleInfo[streamNum].sample = p_track->track_frag_info.first_frame;
    }
    for ( reqSampleNum = m_sampleInfo[streamNum].sample; ; ++reqSampleNum )
    {
      if((m_isFragmentedFile && m_parsedEndofFile) ||
         !m_isFragmentedFile)
      {
        if (reqSampleNum >= p_track->frames)
        {
          break;
        }
      }
      if ( m_isFragmentedFile && (reqSampleNum >= p_track->frames) )
      {
          //parse to the next fragment..
          if(!findiFrameFragment(p_track, reqSampleNum, iRewind, FALSE, fragmentParsed))
          {
              break;
          }
      }
      if ( getSampleInfo(streamNum, reqSampleNum, 1, sampleInfo) > 0 )
        retStat = true;
      if (  (sampleInfo->time==timestamp) ||
            /* for enhanced layer of temporal scalability clip we don't need this check
               since we always need to goto higher timestamp, so we make sure this
               check is only for text track */
            ( ((sampleInfo->time+sampleInfo->delta) > timestamp) &&
              (p_track->type == VIDEO_FMT_STREAM_TEXT) ) )
      {
        break;
      }
      else if(sampleInfo->time > timestamp)
      {
        /* don't forword after given time stamp, also don't decrease for forward */
        if((curSample < reqSampleNum) && p_track->type == VIDEO_FMT_STREAM_TEXT)
          (void)getSampleInfo(streamNum, reqSampleNum-1, 1, sampleInfo);
        break;
      }
    } // for
  } // forward

  return retStat;
}

/*===========================================================================

FUNCTION  skipNSyncSamples

DESCRIPTION
  Public method used to skip offSet Number of Sync Samples.

===========================================================================*/
#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
uint32 Mp4FragmentFile::skipNSyncSamples(int offset, uint32 id, bool *bError, uint32 currentPosTimeStamp)
{
  int32 reqSampleNum = 0;
  int8   noOfSyncSamplesSkipped = 0;
  bool   result = false;
  uint32 newTimeStamp =0;
  boolean fragmentParsed = FALSE;
  boolean fragmentRepositioned = FALSE;
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if(p_track == NULL)
  {
      QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Track Id is not available= %d", id);
  }

  int streamNum = p_track->stream_num;

  reqSampleNum = m_sampleInfo[streamNum].sample;

  video_fmt_sample_info_type  sampleInfo;

  if( offset < 0 )
  {
    // rewind case
    for(reqSampleNum = m_sampleInfo[streamNum].sample; reqSampleNum >= 0; --reqSampleNum)    //penta
    {
      if(getSyncSampleInfo (streamNum, reqSampleNum, true, &sampleInfo))
      {
        noOfSyncSamplesSkipped++;
        reqSampleNum = sampleInfo.sample;
        if( noOfSyncSamplesSkipped == abs(offset) )
        {
          // In successfull case return the latest sync sample time
          result = true;
          break;
        }
      }
      else
      {
        //check are we reached end of the file in case of fragmented file

        if ( m_isFragmentedFile )
        {
          //move to previous fragment..
          if(!findiFrameFragment(p_track, reqSampleNum, true, TRUE, fragmentParsed))
          {
            // No more fragments, so move to current sample and break the loop
            if(fragmentRepositioned || fragmentParsed)
            {
              (void)findiFrameFragment(p_track, m_sampleInfo[streamNum].sample, false, FALSE, fragmentParsed);
            }
            break;
          }
          else
          {
            // we have got one more fragment so keep searching in this fragment for Sync sample
            fragmentRepositioned = TRUE;
            continue;
          }
        }
        // This indicates no more Sync samples found
        break;
      }
    }
  }
  else
  {
    // forward case
    for(reqSampleNum = m_sampleInfo[streamNum].sample; ; ++reqSampleNum)
    {

      // Fragmented files count samples from 1 to p_track->frames
      if( m_isFragmentedFile && m_parsedEndofFile )
      {
        if (reqSampleNum > p_track->frames)
        {
          break;
        }
      }
      else
      {
        // Un Fragmented files count samples from 0 to p_track->frames -1
        if( !m_isFragmentedFile && (reqSampleNum > p_track->frames -1) )
        {
          break;
        }
      }

      if(getSyncSampleInfo( streamNum, reqSampleNum, false, &sampleInfo ))
      {
        noOfSyncSamplesSkipped++;
        reqSampleNum = sampleInfo.sample;
        if( noOfSyncSamplesSkipped==offset )
        {
          // In successfull case return the last sync sample time
          result = true;
          break;
        }
      }
      else
      {
        if ( m_isFragmentedFile && !m_parsedEndofFile )
        {
          // move to the next fragment..
          if(!findiFrameFragment(p_track, reqSampleNum, false, TRUE, fragmentParsed))
          {
            // No more fragments, so move to current sample and break the loop
            if(fragmentRepositioned || fragmentParsed)
            {
              (void)findiFrameFragment(p_track, m_sampleInfo[streamNum].sample, true, FALSE, fragmentParsed);
            }
            break;
          }
          else
          {
            // we have got one more fragment so keep searching in this fragment for Sync sample
            fragmentRepositioned = TRUE;
            continue;
          }
        }
        // Not successfull in skipping desired sync samplese so return the old time stamp.
        break;
      }
    }
  }

  if( result )
  {
    *bError =  false;
    newTimeStamp = (uint32)(((float)sampleInfo.time*1000.0F)/p_track->media_timescale);

    m_reposStreamPending |= maskByte[streamNum];
    m_nextReposSample[streamNum] = sampleInfo.sample;
    m_sampleInfo[streamNum] = sampleInfo;

    QTV_MSG_PRIO2( QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Time Stamp Returned after Skipping %d Sync Samples= %ld",
                   offset, newTimeStamp );

    return newTimeStamp;
  }
  else
  {
    *bError =  true;
    QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "seekToSync function Failed In Skipping %d Sync Samples", offset );
    return currentPosTimeStamp;
  }
}
#endif /* FEATURE_QTV_REPOSITION_SYNC_FRAME */

/*===========================================================================

FUNCTION  getTimestampedSampleInfo

DESCRIPTION
  Public method used to get the sample corresponding to a given timestamp.

===========================================================================*/

bool Mp4FragmentFile::getTimestampedSampleInfo(
  video_fmt_stream_info_type *p_track,
  uint32                      TimeStamp,
  video_fmt_sample_info_type *sampleInfo,
  uint32                     *newTimeStamp,
  bool                        bSetToSyncSample,
  uint32                      currentPosTimeStamp)
{
  boolean fragmentRepositioned = FALSE;
  boolean fragmentParsed = FALSE;
  // media timescale = number of time units per second
  // if media_timescale == 1000, the time units are miliseconds (1000/sec)
  uint32 timescaledTime = (uint32)(((float)TimeStamp/1000.0F)*p_track->media_timescale);
  bool userRewind;
  uint32 scaledCurPosTime = (uint32)(((float)currentPosTimeStamp/1000.0F)*p_track->media_timescale);

  uint32 streamNum = p_track->stream_num;
  uint32 maxFrames = p_track->frames;

  uint32 minFrames = 0;
   uint32 sampleDelta = 1;
   uint32 reqSampleNum = 0;
   int32 timeOffset = 0;
   bool  iRewind = false;
   bool retStat = false;
  if( (!m_isFragmentedFile) && (maxFrames == 0) )
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "No valid frames for given track %d",p_track->track_id);
    return false;
  }

   if( (m_nextSample[streamNum]==0) && (m_sampleInfo[streamNum].size==0) && (p_track->dec_specific_info.obj_type!=MPEG4_IMAGE) )
   {
      /* we have not read any sample yet. So atleast read first good sample */
      uint32 sampleId = 0;
      do
      {
         retStat = (getSampleInfo(streamNum, sampleId, 1, &m_sampleInfo[streamNum])>0)?true:false;
         QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Reading first valid sampleInfo. SampleId=%d",sampleId);
         sampleId++;
      }
      while(retStat && (m_mp4ReadLastStatus[streamNum] == VIDEO_FMT_IO_DONE)
             && (!m_sampleInfo[streamNum].delta || !m_sampleInfo[streamNum].size));

      if(!retStat || (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_IO_DONE))
         return false;
   }

   timeOffset = m_sampleInfo[streamNum].time - timescaledTime;
   if ( p_track->type == VIDEO_FMT_STREAM_AUDIO )
   {
      userRewind = (timeOffset > 0)? true:false;
      if(p_track->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC &&
         aac_data_type == AAC_FORMAT_UNK)
      {
        uint8 buffer[3];
        getSample (streamNum,VIDEO_FMT_DATA_UNIT_BYTE,
                   m_sampleInfo[streamNum].offset,2,buffer);
        if((buffer[0] == 0xFF) && (buffer[1]&0xF0 == 0xF0))
        {
          aac_data_type = AAC_FORMAT_ADTS;
        }
        else
        {
          aac_data_type = AAC_FORMAT_NON_ADTS;
        }
      }
   }
   else
   {
      userRewind = (scaledCurPosTime > timescaledTime)?true:false;
   }

   if ( timeOffset == 0 )
   {
      *sampleInfo = m_sampleInfo[streamNum];
      if(sampleInfo->sync)
         return true;
   }

  QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "CurrentMpeg4Sample=%d, ReqdTS=%d, curPosTS=%d",
            m_sampleInfo[streamNum].sample,TimeStamp,currentPosTimeStamp);

  if( (p_track->type == VIDEO_FMT_STREAM_VIDEO) && bSetToSyncSample && !m_allSyncVideo)
  {
    uint32 frameTime;

    /* Special case: if user wants to goto beginning of the case, just jump to first frame */
    if((timescaledTime == 0) && (!m_isFragmentedFile))
    {
        retStat = (getSampleInfo(streamNum, 0, 1, sampleInfo)>0)?true:false;
        return retStat;
    }


    /* first estimate on which frame we have to go assuming fixed delta between video
    frames. This will take us may be ahead or before the desired time, but from that
    point we can either FF or REW by checking each I-Frame to get the exact reposition
    point (means closest I-Frame). This will reduce time in finding closest I-Frame, if
    user is doing very long seek (say 50 minutes)
    */

    /* if we have to seek more than pre-defined limit (5*60 secs), calculate approximately how many
       sample we have to rewind or forward assuming sample time delta for a track is fixed */
    if( !m_isFragmentedFile && (m_sampleInfo[streamNum].delta &&
        ((uint32)abs((int32)(TimeStamp-currentPosTimeStamp))>m_defaultCoarseJumpLimit) ))
    {

        sampleDelta = (uint32)abs(timeOffset) / m_sampleInfo[streamNum].delta;
        if(timeOffset > 0)
        {
           //rewind
           if(m_sampleInfo[streamNum].sample > sampleDelta)
           {
              reqSampleNum = m_sampleInfo[streamNum].sample - sampleDelta;
           }
           else
           {
              reqSampleNum = 0;
           }
        }
        else
        {
           //forward
           if(!m_isFragmentedFile &&
              ((m_sampleInfo[streamNum].sample + sampleDelta) >= (p_track->frames-1))
             )
           {
              reqSampleNum = p_track->frames-1;
           }
           else
           { //for fragmented files we dont know the final sample number which
              // we would only know after parsing to the last fragment.
              reqSampleNum = m_sampleInfo[streamNum].sample + sampleDelta;
           }
       }

        /* If the required sample is not in the current fragment skip the fragment
        * to go to the required fragment. Also insures that the target fragment has
        * an I Frame or else the repositioning will fail
        */

        if(((reqSampleNum < p_track->track_frag_info.first_frame) ||
            ((reqSampleNum >= maxFrames) && (!m_parsedEndofFile))) &&
            (m_isFragmentedFile))
        {
            iRewind = (reqSampleNum < m_sampleInfo[streamNum].sample)?true:false;
            if(findiFrameFragment(p_track, reqSampleNum, iRewind, FALSE, fragmentParsed))
            {
                fragmentRepositioned = TRUE;
                maxFrames = p_track->frames;
            }
            else
            {
                if((fragmentParsed) && !iRewind)
                {
                    // if reqSampleNum is beyond the total number of frames in the clip.
                    //then rewind to the correct fragment Position.
                    m_parsedEndofFile = false;
                    (void)findiFrameFragment(p_track, m_sampleInfo[streamNum].sample, true, FALSE, fragmentParsed);
                }
                //could not find any fragment containing reqSampleNum.
                *newTimeStamp = 0;
                return false;
            }
        }

        retStat = (getSampleInfo(streamNum, reqSampleNum, 1, sampleInfo)>0)?true:false;
        if(retStat)
        {
            if( (sampleInfo->time==timescaledTime) && (sampleInfo->sync!=TRUE) )
            {
            /* if it is not a sync sample, just change the time stamp, so that we continue to
                search for correct SYNC sample. For this if it is REW, we will increase the
                sample. Also if we have reached zero sample, we can only increase it */
            if(userRewind || !sampleInfo->sample)
                (void)getSampleInfo(streamNum, sampleInfo->sample+1, 1, sampleInfo);
            else
                (void)getSampleInfo(streamNum, sampleInfo->sample-1, 1, sampleInfo);
            }
        }
        else
        {
            reqSampleNum = m_sampleInfo[streamNum].sample;
            *sampleInfo = m_sampleInfo[streamNum];
        }
    }
    else
    {
        reqSampleNum = m_sampleInfo[streamNum].sample;
        *sampleInfo = m_sampleInfo[streamNum];
    }

    iRewind = (sampleInfo->time > timescaledTime)?true:false;
     if(sampleInfo->time == timescaledTime)
        iRewind = userRewind;

    frameTime = sampleInfo->time;
    retStat = true;

    /* loop till we get the desired time stamp */
    if(iRewind)
    {
      for( ; frameTime>timescaledTime && retStat ; )
      {
        /*reqSampleNum is always within the current parsed fragment at this point*/
        retStat = getSyncSampleInfo(streamNum, reqSampleNum, iRewind, sampleInfo);
        QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Rewind1: SynchSample=%d, frag_info.first_frame=%d, eof=%d"
                  , sampleInfo->sample, p_track->track_frag_info.first_frame,m_parsedEndofFile);

        bool iFrameOnFirstSample = false;
        iFrameOnFirstSample = retStat && sampleInfo->sample && (sampleInfo->sample == p_track->track_frag_info.first_frame)
            && (sampleInfo->time > timescaledTime) && m_isFragmentedFile;

        QTV_MSG_PRIO3 (QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "SynchSample=>offset=%d, =>size=%d, iFrameOnFirstSample=%d"
                  , sampleInfo->offset, sampleInfo->size,iFrameOnFirstSample);

        if((!retStat && m_isFragmentedFile) || iFrameOnFirstSample )
        {
          if(p_track->track_frag_info.first_frame)
          {
            //not the first fragment
            //move to the previous fragment.
            reqSampleNum = MIN(reqSampleNum,p_track->track_frag_info.first_frame - 1);
            if(findiFrameFragment(p_track, reqSampleNum, iRewind, TRUE, fragmentParsed))
            {
              m_parsedEndofFile = false;
              maxFrames = p_track->frames;
              fragmentRepositioned = TRUE;
              reqSampleNum = MIN(reqSampleNum,p_track->frames -1);
              retStat = true;
              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Rewind2..found iFrameFragment MaxFrames=%d",maxFrames);
              continue;
            }
            else
            {
              //Note:Rewind should always succeed.
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Rewind2..did not find an iFrameFragment");
              if((fragmentParsed) || (fragmentRepositioned))
              {
                // if the inter fragment repositioning failed and the fragment has been repositioned
                //Now reposition to the correct fragment
                iRewind = (m_sampleInfo[streamNum].sample < reqSampleNum)?true:false;
                (void)findiFrameFragment(p_track, m_sampleInfo[streamNum].sample, iRewind, FALSE, fragmentParsed);
              }
              *newTimeStamp = 0;
              retStat = false;
              break;
            }
          }
        }

        frameTime = sampleInfo->time;

        /* user has actually pressed FF, but we are rewinding, then we should
           not go beyond current displaty time. We also should make sure we are
           beyond the time user has asked, not before */
        if(!userRewind)
        {
          if( (scaledCurPosTime >= frameTime) ||(timescaledTime > frameTime) )
          {
            /* means we have gone before current position, but user has pressed FF,
               so we should look I-Frame in FF direction */
            iRewind = false;
            reqSampleNum = sampleInfo->sample+1;
         QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward0: !userRewind reqSampleNum=%d",reqSampleNum);
         if((sampleInfo->sample >= (maxFrames-1))&& m_isFragmentedFile)
         {
           findiFrameFragment(p_track, reqSampleNum, FALSE, FALSE, fragmentParsed);
         }
            break;
          }
        }

        if(sampleInfo->sample > 0)
          reqSampleNum = sampleInfo->sample-1;
        else
        {
          reqSampleNum = 0;
          break;
        }
      }
      /* if in this rewind case, go to the first frame */
      if(!retStat && (m_mp4SyncLastStatus[streamNum]==VIDEO_FMT_IO_DONE))
      {
         reqSampleNum = 0;
        if(m_isFragmentedFile)
           findiFrameFragment(p_track, reqSampleNum, TRUE, FALSE, fragmentParsed);
        retStat = (getSampleInfo(streamNum, reqSampleNum, 1, sampleInfo)>0)?true:false;
      }

    }

    /* FF case */
    if(!iRewind)
      {
         for( ; frameTime<=timescaledTime && retStat; )
         {
            video_fmt_sample_info_type tempSampleInfo;
            memset(&tempSampleInfo,0x00,sizeof(tempSampleInfo));
            /*reqSampleNum is always within the current parsed fragment at this point*/
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "invoking getSyncSampleInfo with reqSampleNum %d",
               reqSampleNum);
            retStat = getSyncSampleInfo(streamNum, reqSampleNum, iRewind, &tempSampleInfo);
            if(tempSampleInfo.sync)
               memcpy(sampleInfo,&tempSampleInfo,sizeof(tempSampleInfo));
            QTV_MSG_PRIO4(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward1: SynchSample=%d =>offset=%d =>size=%d time %d",
               sampleInfo->sample, sampleInfo->offset, sampleInfo->size,sampleInfo->time);

            if(!retStat  && m_isFragmentedFile)
            {
               /* If Sync Sample could not be found on this fragment FFWD to next fragment
               */
               //move to the next fragment.
               reqSampleNum = MAX(reqSampleNum,p_track->frames);
               if(!m_parsedEndofFile &&
                  findiFrameFragment(p_track, reqSampleNum, iRewind, TRUE, fragmentParsed))
               {
                  maxFrames = p_track->frames;
                  fragmentRepositioned = TRUE;
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "came back in getTimestampedSampleInfo from findiFrameFragment");
                  QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "reqSampleNum %d p_track->track_frag_info.first_frame %d maxFrames %d",reqSampleNum,p_track->track_frag_info.first_frame,maxFrames);
                  reqSampleNum = MAX(reqSampleNum,p_track->track_frag_info.first_frame);
                  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "updated reqsample by MAX reqSampleNum %d p_track->track_frag_info.first_frame %d",reqSampleNum,p_track->track_frag_info.first_frame);
                  retStat = true;
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward2.. found iFrameFragment");
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward2.. found iFrameFragment,continuing..");
                  continue;
               }
               else
               {
              if( sampleInfo->sync)
              /* Sync to the previous sync sample*/
                  {
                    *newTimeStamp = (uint32)((float)sampleInfo->time * 1000.0F / p_track->media_timescale);
                    /* check if we got any sync sample later than current sample */
                     if(*newTimeStamp > currentPosTimeStamp)
                    {
                       /* check if the sync sample is in the same fragment*/
                        if(fragmentParsed || fragmentRepositioned)
                        {
                          iRewind = (sampleInfo->sample < reqSampleNum)?true:false;
                          findiFrameFragment(p_track, sampleInfo->sample, iRewind, FALSE, fragmentParsed);
                        }
                        getSampleInfo(streamNum, sampleInfo->sample, 1, sampleInfo);
                        retStat = true;
                        break;
                    }
                  }
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward2..did not find an iFrameFragment");
                  QTV_MSG_PRIO7(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward2..sampleInfo->sample %d (maxFrames-1)%d m_parsedEndofFile %d fragmentParsed %d fragmentRepositioned %d reqSampleNum %d retStat %d",
                     sampleInfo->sample,(maxFrames-1),m_parsedEndofFile,fragmentParsed,fragmentRepositioned,reqSampleNum,retStat);
                  if((fragmentParsed) || (fragmentRepositioned))
                  {
                     // If the inter fragment repositioning failed and the fragment has been repositioned
                     // Now reposition to the correct fragment
                     m_parsedEndofFile = false;
                     iRewind = (m_sampleInfo[streamNum].sample < reqSampleNum)?true:false;
                     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward2..resetting m_parsedEndofFile and determining iRewind?");
                     QTV_MSG_PRIO7(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward2..sampleInfo->sample %d (maxFrames-1)%d m_parsedEndofFile %d fragmentParsed %d fragmentRepositioned %d reqSampleNum %d iRewind %d",
                        sampleInfo->sample,(maxFrames-1),m_parsedEndofFile,fragmentParsed,fragmentRepositioned,reqSampleNum,iRewind);
                     (void)findiFrameFragment(p_track, m_sampleInfo[streamNum].sample, iRewind, FALSE, fragmentParsed);
                     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward2..came back in getTimestampedSampleInfo from findiFrameFragment with frg. repositioned to m_sampleInfo[streamNum].sample %d",m_sampleInfo[streamNum].sample);
                  }
                  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward2..m_sampleInfo[streamNum].sample+1 %d p_track->frames %d",
                     (m_sampleInfo[streamNum].sample+1),p_track->frames);
                  //If this was the last sample in the current fragment, then reset the
                  //stream offset, as this fragment will not be processed in the videoFMT reader.
                  if(m_sampleInfo[streamNum].sample+1 == p_track->frames)
                  {
                     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward2..inside (m_sampleInfo[streamNum].sample+1 == p_track->frames)");
                       /* Since we fail the repositioning, we had parsed the current fragment again and
                        videoFMT will be pointing to the start of fragment.
                        By calling getSampleInfo, VideoFMT will move its current pointer again to the
                        current sample just like it was before repositioning started..
                      */
                getSampleInfo(streamNum, m_sampleInfo[streamNum].sample, 1, &m_sampleInfo[streamNum]);
                  }
                  *newTimeStamp = 0;
                  retStat = false;
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Forward2..making *newTimeStamp 0 and retStat as false");
                  break;
               }
            }
            frameTime = sampleInfo->time;
            QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "updated frameTime to sampleInfo->time %d timescaledTime %d retStat %d",
               frameTime,timescaledTime,retStat);

            /* if we have found the frame, so we don't need to go further */
            if(frameTime>=timescaledTime && retStat)
            {
               QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "breaking in (frameTime>=timescaledTime && retStat)frameTime %d timescaledTime %d",
                  frameTime,timescaledTime);
               break;
        }
        /*If the last sample is a sync sample then set the time stamp to that sync sample. */
         if((retStat && m_parsedEndofFile && (sampleInfo->sample == (p_track->frames-1))))
         {
           *newTimeStamp = (uint32)((float)sampleInfo->time * 1000.0F / p_track->media_timescale);
          /*check if we got any sync sample later than current sample */
          if(*newTimeStamp > currentPosTimeStamp)
           return true;
            }

         /**If the serach in the current Fragment for Sync frames completed, then move
          **to next available I frame fragment, if not available come out of loop
          */

        if((sampleInfo->sample >= (maxFrames-1)))
        {
          if(m_parsedEndofFile)
          {
            QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "(sampleInfo->sample >= (maxFrames-1)), sampleInfo->sample %d (maxFrames-1)%d m_parsedEndofFile %d",
                         sampleInfo->sample,(maxFrames-1),m_parsedEndofFile);
            retStat = false;
            QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "breaking (sampleInfo->sample >= (maxFrames-1)), as m_parsedEndofFile is TRUE sampleInfo->sample %d (maxFrames-1)%d",
                       sampleInfo->sample,(maxFrames-1));
            break;
          }
          else
          {
           /**This scenario normally happens, if the last sample in the fragment is a sync sample
           */
            reqSampleNum = MAX(reqSampleNum,p_track->frames);
            if( findiFrameFragment(p_track, reqSampleNum, iRewind, TRUE, fragmentParsed) )
            {
              maxFrames = p_track->frames;
              fragmentRepositioned = TRUE;
              reqSampleNum = MAX(reqSampleNum,p_track->track_frag_info.first_frame);
              retStat = true;
              continue;
            }
            else
            {
              retStat = false;
         break;
            }
          }
        }
        reqSampleNum = sampleInfo->sample+1;
            QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "sampleInfo->sample %d reqSampleNum(increasing sampleInfo->sample by 1) %d",
               sampleInfo->sample,reqSampleNum);
         }
         QTV_MSG_PRIO6(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "out of loop for loop retStat %d sampleInfo->sync %d m_parsedEndofFile %d sampleInfo->sample %d, (p_track->frames-1)%d,sampleInfo->time %d",
            retStat,sampleInfo->sync,m_parsedEndofFile,sampleInfo->sample,(p_track->frames-1),sampleInfo->time);

      if(!m_isFragmentedFile && retStat == false)
      {
        if(sampleInfo->sync)
       /* Sync to the previous sync sample*/
          retStat = (getSampleInfo(streamNum, sampleInfo->sample, 1, sampleInfo)>0)?true:false;
        else
      /* in FF case, if we did not find any SYNC sample, then we try to find SYNC sample in reverse
        direction. If we get any SYNC sample and its time stamp is ahead of current position, we
        should FF upto that sample*/
          retStat = getSyncSampleInfo(streamNum, reqSampleNum, TRUE, sampleInfo);
          if(retStat)
          {
            *newTimeStamp = (uint32)((float)sampleInfo->time * 1000.0F / p_track->media_timescale);
            /* check if we got any sync sample later than current sample */
            if(*newTimeStamp > currentPosTimeStamp)
              return true;
            else
            {
              retStat = false;
            }
          }
      }
    }
    /* see if we were successful */
    if(retStat)
    {
      *newTimeStamp = (uint32)((float)sampleInfo->time * 1000.0F / p_track->media_timescale);
      QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "out of for loop when retStat is TRUE updated *newTimeStamp to %d,sampleInfo->time %d,p_track->media_timescale %d",
                       (*newTimeStamp),sampleInfo->time,p_track->media_timescale);

    }
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "returning retStat as %d",
                       retStat);
    return retStat;
  }

  /* calculate approximately how many sample we have to rewind or forward
     assuming sample time delta for a track is fixed */
  if (m_sampleInfo[streamNum].delta )
  {
    sampleDelta = (uint32)abs(timeOffset) / m_sampleInfo[streamNum].delta;
  }

  if (userRewind )
  {
    bool bHasFrames= false;
    // rewind processing (current time > desired timestamp)
    //------------------
    iRewind = true;


    /* Check if current fragment has frames or not
     By Checking if the current fragment is in the fragmentInfoArray  */
    for (uint32 i = 0; i < fragmentinfoCount; i++)
    {
      fragment_info_type *fragment_info = fragmentInfoArray[i];
      if((fragment_info->fragment_number) == (p_track->fragment_number))
      {
        bHasFrames = true;
        break;
      }
    }
    if(p_track->type != VIDEO_FMT_STREAM_TEXT)
   {
      if ( m_sampleInfo[streamNum].sample >= sampleDelta )
      {
        reqSampleNum = m_sampleInfo[streamNum].sample - sampleDelta;
      }
      else
      {
        reqSampleNum = 0;
      }
   }
   else
   {
      reqSampleNum = m_sampleInfo[streamNum].sample;
   }
    if ( reqSampleNum <= minFrames )
    {
      reqSampleNum = minFrames;
    }

    if( ( (reqSampleNum < p_track->track_frag_info.first_frame)||(!bHasFrames) ) &&
     (m_isFragmentedFile))
    {
      if(findiFrameFragment(p_track, reqSampleNum, iRewind, TRUE, fragmentParsed))
      {
        fragmentRepositioned = TRUE;
      }
      else
      {
        *newTimeStamp = 0;
        return false;
      }
    }
  }
   else
   {    // timeOffset < 0
      // fast forward processing (current time < desired timestamp)
      //------------------------
      if(p_track->type != VIDEO_FMT_STREAM_TEXT)
      {
         reqSampleNum = m_sampleInfo[streamNum].sample + sampleDelta;
      }
      else
      {
         reqSampleNum = m_sampleInfo[streamNum].sample;
      }
      if((reqSampleNum >= maxFrames) && (!m_parsedEndofFile) &&
         (m_isFragmentedFile))

      {
         if(findiFrameFragment(p_track, reqSampleNum, iRewind, TRUE, fragmentParsed))
         {
            maxFrames = p_track->frames;
            fragmentRepositioned = TRUE;
         }
         else
         {
            if(fragmentParsed)
            {
               if(p_track->type == VIDEO_FMT_STREAM_AUDIO)
               {

                  /*
                  * This could happen if we are requesting more than the duarion of audio track.
                  * In this case, we should not fail the repositioning for audio track.
                  * Instead, we should take audio to the last sample.
                  *
                  * This will reduce the duration of video freeze for A+V clip as CheckFrame would be
                  * smaller than the case if we would have failed the repositioning.
                  */
                  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_HIGH,
                     "findiframefragment returned FALSE for VIDEO_FMT_STREAM_AUDIO for reqSampleNum %d",reqSampleNum);

                  int32 length = 0, index = 0;
                  fragment_info_type *fragment_info = NULL;

                  /*
                  * By this time, all fragments that have audio sample > what we are
                  * playing right now, would have been added when we invoked findiFrameFragment above
                  * and 'fragmentParsed' is set to TRUE.
                  *
                  * fragmentInfoArray.GetLength() will help to identify the correct fragment
                  * which has the last audio sample. It could be in the same fragment which has
                  * current audio sample being played.
                  */
                  length = fragmentInfoArray.GetLength();
                  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_HIGH,
                     "fragmentInfoArray.GetLength() returned %d",length);

                  /*
                  * Adjust the index of target fragment
                  */
                  ASSERT (length > 0);
                  index = length -1;

                  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_HIGH,
                     "adjusted index to %d",index);

                  /*
                  * Get the target fragment_info to identify the
                  * fragment which has lat audio sample.
                  */
                  fragment_info = fragmentInfoArray[index];


                  if(fragment_info)
                  {
                     QTV_MSG_PRIO2(QTVDIAG_GENERAL,
                        QTVDIAG_PRIO_HIGH,
                        "got fragment_info for index %d and fragment_info->frames[p_track->stream_num] %d",
                        index,fragment_info->frames[p_track->stream_num]);

                     /*
                     * Adjust the 'reqSampleNum' if we are asking
                     * more than what's in the audio track
                     */
                     if(reqSampleNum >= fragment_info->frames[p_track->stream_num])
                     {
                        /*
                        * p_track numbering runs from 1..., so subtract 1.
                        */
                        reqSampleNum = fragment_info->frames[p_track->stream_num] -1;
                        QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                           QTVDIAG_PRIO_HIGH,
                           "adjusted reqSampleNum to %d",reqSampleNum);

                        /*
                        * Invoke findiFrameFragment with adjusted 'reqSampleNum' to
                        * bring the correct fragment which has the last audio sample,
                        * so that we can invoke getSampleInfo on 'reqSampleNum'
                        */

                                                                /*
                                                                * In this case reqSampleNum is the last available audio sample
                                                                * we must search for the iFrameFragment for this reqSampleNum
                                                                * in the backward direction so rewind must be true
                                                                */

                                                                iRewind= TRUE;
                        if(findiFrameFragment(p_track, reqSampleNum, iRewind, TRUE, fragmentParsed))
                        {
                           maxFrames = p_track->frames;
                           fragmentRepositioned = TRUE;
                           QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                              QTVDIAG_PRIO_HIGH,
                              "current timescaledTime %d ",timescaledTime);

                           if(getSampleInfo(streamNum, reqSampleNum, 1, sampleInfo)>0)
                           {
                              /*
                              * Update the timescaledTime to match with the
                              * TS of the last valid audio sample
                              */
                              timescaledTime = (uint32)(((float)(sampleInfo->time)/1000.0F)*p_track->media_timescale);
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                                 QTVDIAG_PRIO_HIGH,
                                 "Updated timescaledTime to %d ",timescaledTime);
                           }

                        }
                        else
                        {
                           QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                              QTVDIAG_PRIO_HIGH,
                              "findiFrameFragment returned FALSE for adjusted reqSampleNum %d",reqSampleNum);
                        }
                     }
                  }//if(fragment_info)
                  else
                  {
                     QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                        QTVDIAG_PRIO_HIGH,
                        "got NULL fragment_info for index %d",index);
                  }
               }//if(p_track->type == VIDEO_FMT_STREAM_AUDIO)
               else
               {
                  // if the inter fragment FFWD failed and the fragment has been repositioned
                  //Now Rewind to the correct fragment Position
                  (void)findiFrameFragment(p_track, m_sampleInfo[streamNum].sample, true, FALSE, fragmentParsed);
               }
            }//if(fragmentParsed)
            if(p_track->type != VIDEO_FMT_STREAM_AUDIO)
            {
               *newTimeStamp = 0;
               return false;
            }
         }
      }

      if ( reqSampleNum >= maxFrames )
      {
         reqSampleNum = maxFrames - 1;
      }
   }

  if ( p_track->type == VIDEO_FMT_STREAM_AUDIO )
  {
   retStat = (getSampleInfo(streamNum, reqSampleNum, 1, sampleInfo)>0)?true:false;
    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "AUDIO CoarseRepos: reqSampleNum=%d, sampleInfo.sample=%d, sampleInfo.time=%d",
                  reqSampleNum, sampleInfo->sample, sampleInfo->time);
    if(retStat && (sampleInfo->time!=timescaledTime))
    {
        //perform coarse repositioning..
        m_sampleInfo[streamNum] = *sampleInfo;
        if(sampleInfo->time < timescaledTime)
        {
            iRewind = false;
        }
        else
        {
            iRewind = true;
        }

        retStat = getSampleAtTimestamp (p_track, timescaledTime, iRewind, sampleInfo);
        QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "AUDIO FineRepos: sampleInfo.sample=%d, sampleInfo.time=%d, iRewind=%d",
                  sampleInfo->sample, sampleInfo->time, iRewind);
    }
  }
  else if( p_track->type == VIDEO_FMT_STREAM_TEXT
          || p_track->type == VIDEO_FMT_STREAM_VIDEO )
  {
   retStat = getSampleAtTimestamp (p_track, timescaledTime, iRewind, sampleInfo);
  }

  *newTimeStamp = (uint32)(((float)sampleInfo->time*1000.0F)/p_track->media_timescale);
  QTV_MSG_PRIO4(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "newTimeStamp=%d, m_playVideo=%d, m_playAudio=%d, m_playText=%d",
                *newTimeStamp,m_playVideo,m_playAudio,m_playText);

  return retStat;
}


#endif //(FEATURE_FILE_FRAGMENTATION)




