/* =======================================================================
mpeg4file.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/mpeg4file.cpp#67 $
$DateTime: 2010/11/11 20:41:30 $
$Change: 1514851 $

========================================================================== */

/* ==========================================================================

INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Common definitions                      */
#include "assert.h"             /* ASSERT definitions                      */

#include <stdio.h>
#include "fileMedia.h"
#include "mpeg4file.h"
#include "ztl.h"
#include "utf8conv.h"
#include "AVSync.h"
#include "atomdefs.h"
#include "Mpeg4Player.h"

#include "oscl_string.h"
#include "videofmt_mp4r.h"

#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || defined FEATURE_QTV_PSEUDO_STREAM
extern "C" {
#include "event.h"
}


#define QTV_3GPP_MIN_NUM_VIDEO_FRAMES_TO_BUFFER 6
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

#ifdef FEATURE_QTV_DRM_DCF
 #include "IxStream.h"
#endif

#ifdef CHECK_MP4_MAX_VIDEO_BITRATE
#define MAX_MP4_VIDEO_BIT_RATE 2000000
#endif
/* ==========================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
uint32 AtomUtils::fileSize = 0;

#ifdef FEATURE_H264_DECODER
#define ALLOC_MEMORY_HEADER 256
#endif
// OMA DRM Encryption Scheme Version
#define OMA_DRM_SCHEME_VERSION 0X0200
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

/* ======================================================================== */
/* <EJECT> */
/*===========================================================================

FUNCTION  Mpeg4File

DESCRIPTION
This is the Mpeg4File class constructor - initializes the class members.
===========================================================================*/
Mpeg4File::Mpeg4File(  OSCL_STRING filename,
                       Mpeg4Player *pMpeg4Player,
                       unsigned char *pFileBuf,
                       uint32 bufSize,
                       bool playVideo,
                       bool playAudio,
                       bool playText
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#if defined (FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                       ,bool bHttpStream
                       ,uint32 wBufferOffset
                       ,uint32 wStartupTime
                       ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize
                       ,QtvPlayer::FetchBufferedDataT FetchBufferedData
                       ,QtvPlayer::InstanceHandleT handle
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
                     )
{
  // Initialize
  Mpeg4File::InitData();

  m_pMpeg4Player= pMpeg4Player;
  m_playAudio = playAudio;
  m_playVideo = playVideo;
  m_playText  = playText;

  m_bAudioPresentInClip = false;
  m_bVideoPresentInClip = false;
  m_bTextPresentInClip  = false;
  m_fileSizeFound       = false;
#ifdef FEATURE_QTV_PDCF
  m_OffsetinEncryptionTypeArray = 0;
#endif /* FEATURE_QTV_PDCF */
  if(filename.size() && pFileBuf)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4File::Mpeg4File filename and pFileBuf both non NULL");
  }
  if(!(filename.size()) && !pFileBuf
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    && !FetchBufferedDataSize
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
    )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4File::Mpeg4File filename and pFileBuf both NULL");
  }
  m_pFileBuf  = pFileBuf;
  m_fileSize  = bufSize;
  m_filename  = filename;

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

  if(m_pFileBuf)
  {
    m_parseFilePtr = OSCL_FileOpen(m_pFileBuf, m_fileSize);
  }
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  else if(FetchBufferedDataSize)
  {
    uint32 nSize = 0;
    boolean bEndOfData = false;
    uint32 userData = 0;
    #ifdef FEATURE_QTV_DRM_DCF
      userData = (uint32)m_inputStream;
    #endif
    FetchBufferedDataSize(userData, &nSize, &bEndOfData, handle);
    /*if bEndofData is true,then OEM has indicated that the entire clip is available in the buffer
    Therefore we can use the atom like MFRA which is present at the end of the clip*/

    if(bEndOfData == TRUE)
    {
      m_fileSize = nSize;
    }
    else
    {
      //Out here we dont know the file size
      m_fileSize = 0xFFFFFFFF;
    }

    //Using default trackId 0 for parsing..
    m_parseFilePtr = OSCL_FileOpen(NULL, 0, FetchBufferedData, 0, handle);
  }
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
  else
  {
    m_parseFilePtr = OSCL_FileOpen(m_filename, (OSCL_TCHAR *) _T("rb"));
    m_fileSize = OSCL_FileSize( m_filename );
  }

  if(!m_parseFilePtr )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failed to open file");
    _success = false;
    return;
  }
  if(!m_fileSize)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid file size %d",m_fileSize);
    _success = false;
    return;
  }
  m_fileSizeFound = true;
  AtomUtils::fileSize   = m_fileSize;

#ifdef FEATURE_MEASURE_TIMING
  startTimeGetMetadata  = ZUtils::Clock();
#endif

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  bHttpStreaming    = bHttpStream;
  m_wBufferOffset   = wBufferOffset;
  m_startupTime     = wStartupTime;
  m_fpFetchBufferedDataSize    = FetchBufferedDataSize;
  m_fpFetchBufferedData        = FetchBufferedData;
  m_QtvInstancehandle = handle;
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
}

#ifdef FEATURE_QTV_DRM_DCF
/*===========================================================================

FUNCTION
  Mpeg4File::Mpeg4File

DESCRIPTION
  Constructor for creating mpeg4file file instance for DCF media

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
===========================================================================*/
Mpeg4File::Mpeg4File(dcf_ixstream_type inputStream,
                     Mpeg4Player *pMpeg4Player,
                     bool bPlayVideo,
                     bool bPlayAudio,
                     bool bPlayText
                     )
{
  IxStream* pStream;

  #if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    boolean bEndOfData = false;
  #endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */

  // Initialize
  InitData();
  m_pMpeg4Player= pMpeg4Player;
  m_playAudio     = bPlayAudio;
  m_playVideo     = bPlayVideo;
  m_playText      = bPlayText;
  m_inputStream   = inputStream;

  m_bAudioPresentInClip = false;
  m_bVideoPresentInClip = false;
  m_bTextPresentInClip  = false;

  if(inputStream == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4File::Mpeg4File input-stream is NULL");
    _success = false;
    return;
  }

  #if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    pStream = (IxStream* )inputStream;
    if(pStream->IsProgressive())
    {
      bHttpStreaming    = true;
      //Get from IxStream C interface
      m_fpFetchBufferedDataSize = (QtvPlayer::FetchBufferedDataSizeT) ixstream_length;
    }
  #endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */

  #if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    if(m_fpFetchBufferedDataSize)
    {
      uint32 nSize = 0;
      m_fpFetchBufferedDataSize((uint32)inputStream, &nSize, &bEndOfData, NULL);

      /*if bEndofData is true,then OEM has indicated that the entire clip is available in the buffer
      Therefore we can use the atom like MFRA which is present at the end of the clip*/

      if(bEndOfData == TRUE)
      {
        m_fileSize = nSize;
      }
      else
      {
        //Out here we dont know the file size
        m_fileSize = 0xFFFFFFFF;
      }
      m_parseFilePtr = OSCL_FileOpen(inputStream);
    }
    else
  #endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
    {
      m_parseFilePtr = OSCL_FileOpen(inputStream);
    }

  if(!m_parseFilePtr)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failed to open file");
    _success = false;
    return;
  }

  // File Size calculation

  pStream = (IxStream* )inputStream;
  if(pStream)
  {
    #if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    if(pStream->IsProgressive() && (bEndOfData == FALSE))
    {
      m_fileSize = 0x7FFFFFFF;
    }
    else
    #endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
    {
      pStream->Size(&m_fileSize);
    }
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Mpeg4File::Mpeg4File size %d",m_fileSize);
  }

  AtomUtils::fileSize = m_fileSize;


#ifdef FEATURE_MEASURE_TIMING
  startTimeGetMetadata = ZUtils::Clock();
#endif
}
#endif

/*===========================================================================

FUNCTION  InitData

DESCRIPTION
  Initialize all members.
===========================================================================*/
void Mpeg4File::InitData()
{

  _success = true;
  fragmentNumber         = 0;
  m_playAudio            = false;
  m_playVideo            = false;
  m_playText             = false;
  m_isFragmentedFile     = false;
  m_parsedEndofFragment  = false;
  m_parsedEndofFile      = false;
  m_corruptFile          = false;
  m_mp4ParseLastStatus = VIDEO_FMT_DONE;
  m_parseIODoneSize = 0;
  m_mp4ParseContinueCb = NULL;

  m_hasAudio             = false;
  m_hasVideo             = false;
  m_hasText              = false;
  m_allSyncVideo         = false;

  m_defaultCoarseJumpLimit = QTV_MPEG4_COARSE_REPOS_LIMIT;

  memset(&m_videoFmtInfo, 0xFF, sizeof(video_fmt_info_type));
  memset(&m_track[0], 0x0, sizeof(uint32)*VIDEO_FMT_MAX_MEDIA_STREAMS);

  memset(&m_sampleInfo[0], 0x0, sizeof(video_fmt_sample_info_type)*VIDEO_FMT_MAX_MEDIA_STREAMS);
  memset(&m_nextSample[0], 0x0, sizeof(uint32)*VIDEO_FMT_MAX_MEDIA_STREAMS);


  for(uint i=0; i<VIDEO_FMT_MAX_MEDIA_STREAMS; i++)
  {
    m_streamFilePtr[i] = NULL;
    m_track[i]         = NULL;
#ifdef FEATURE_QTV_PDCF
    m_EncryptionType[i].encryptionType = QtvPlayer::ENCRYPT_NONE;
#endif /* FEATURE_QTV_PDCF */
  }
#ifdef FEATURE_QTV_PDCF
  _pdcfAtom = NULL;
  m_pEncryptedDataBuffer = NULL;
#endif /* FEATURE_QTV_PDCF */

  m_reposStreamPending  = 0;
  m_trackCount          = 0;
  m_mp4ParseEndCb       = NULL;
  m_mp4ParseServerData  = NULL;

  m_pFileBuf            = NULL;
  m_fileSize            = 0;
  m_filename            = NULL;

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

  m_parseFilePtr        = NULL;
  m_fileSize            = 0;
  AtomUtils::fileSize   = 0;

#ifdef FEATURE_QTV_DRM_DCF
  m_inputStream         = NULL;
#endif

  _fileErrorCode        = (int32)EVERYTHING_FINE;

#ifdef FEATURE_MEASURE_TIMING
  startTimeGetMetadata  = 0;
#endif

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  textSampleEntryCount  = 0;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_MP4_KDDI_META_DATA
  _kddiDRMAtom                = NULL;
  _kddiContentPropertyAtom    = NULL;
  _kddiMovieMailAtom          = NULL;
  _kddiEncoderInformationAtom = NULL;
  _kddiGPSAtom                = NULL;
#endif /* FEATURE_MP4_KDDI_META_DATA */

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  _kddiTelopElement           = NULL;
  _pTsmlAtom                  = NULL;
#endif

#if defined(FEATURE_QTV_SKT_MOD_MIDI)
  _midiAtom                   = NULL;
  _linkAtom                   = NULL;
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

  _ftypAtom                   = NULL;
  _cprtAtom                   = NULL;
  _authAtom                   = NULL;
  _titlAtom                   = NULL;
  _dscpAtom                   = NULL;
  _dcmdAtom                   = NULL;
  _rtngAtom = NULL;
  _clsfAtom = NULL;
  _gnreAtom = NULL;
  _perfAtom = NULL;
  _lociAtom = NULL;
  _kywdAtom = NULL;
  _metaAtom = NULL;
   _albumAtom = NULL;
   _yrrcAtom  = NULL;

#ifdef FEATURE_H264_DECODER
  m_nextSeqSample             = 0;
  m_nextPicSample             = 0;
#endif /* FEATURE_H264_DECODER */

  /* initialize client data for videoFMT callback */
  for(uint i=0; i<VIDEO_FMT_MAX_MEDIA_STREAMS; i++)
  {
    m_clientData[i].Mp4FilePtr = this;
    m_clientData[i].streamNum  = (int)i;
  }

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  bHttpStreaming               = false;
  m_wBufferOffset              = 0;
  bGetMetaDataSize             = FALSE;
  bDataIncomplete              = FALSE;
  bQtvPlayerPaused             = TRUE;
  bsendParseFragmentCmd        = FALSE;
  m_currentParseFragment       = 0;
  m_minOffsetRequired          = 0;
  parserState                  = Common::PARSER_PAUSE;
  m_pbTime                     = 0;
  m_startupTime                = HTTP_DEFAULT_STARTUP_TIME;
  m_fpFetchBufferedDataSize    = NULL;
  m_fpFetchBufferedData        = NULL;
  m_QtvInstancehandle          = NULL;
  QCUtils::InitCritSect(&videoFMT_Access_CS);
  Initialized = FALSE;
  Parsed = FALSE;
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
  m_bUdtaAtomPresent           = false;
  UUIDatomEntryCount = 0;
  aac_data_type = AAC_FORMAT_UNK;
}

/*===========================================================================

FUNCTION  parseFirstFragment

DESCRIPTION
Private method to parse the first atom inside the MP4 file opened inside
the consturctor.

===========================================================================*/
void Mpeg4File::parseFirstFragment()
{
     if(_success)
     {
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
          if(!initializeVideoFMT())
          {
      _fileErrorCode = (int)READ_FAILED; // Read past EOF
               _success = false;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "VIDEO_FMT_FAILURE");
          }
#else
          // Parse the file as an MP4 file, building a list of atom information
          // structures.
          video_fmt_open (mp4ParseStatusCallback, this, VIDEO_FMT_MP4,0xff);

          while(  (m_mp4ParseLastStatus != VIDEO_FMT_INFO) &&
               (m_mp4ParseLastStatus != VIDEO_FMT_FAILURE) &&
          (m_mp4ParseLastStatus != VIDEO_FMT_DATA_INCOMPLETE) &&
          (m_mp4ParseLastStatus != VIDEO_FMT_FRAGMENT) &&
               (m_mp4ParseLastStatus != VIDEO_FMT_DATA_CORRUPT)  )
          {
               if( (m_mp4ParseContinueCb == NULL) ||
                    (m_mp4ParseServerData == NULL)  )
                    break;
               else
                    m_mp4ParseContinueCb(m_mp4ParseServerData);
          }
    if(m_mp4ParseLastStatus == VIDEO_FMT_FRAGMENT)
    {
      m_isFragmentedFile = true;
    }
#endif /*FEATURE_QTV_PSEUDO_STREAM */
          // Check for any atoms that may have read past the EOF that were not
          // already caught by any earlier error handling
          if( (m_mp4ParseLastStatus == VIDEO_FMT_FAILURE) ||
               (m_mp4ParseLastStatus == VIDEO_FMT_DATA_CORRUPT)  )
          {
               _fileErrorCode = (int32)READ_FAILED; // Read past EOF
               _success = false;
               (void)OSCL_FileClose( m_parseFilePtr );
               m_parseFilePtr = NULL;
                        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_FAILURE");
               return;
          }

#if defined FEATURE_QTV_PSEUDO_STREAM || defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
          if(!bHttpStreaming && m_parseFilePtr)
#else
          if(m_parseFilePtr)
#endif
          {
               (void)OSCL_FileClose( m_parseFilePtr );
               m_parseFilePtr = NULL;
      if(!getNumTracks() && _success)
      {
        _success = false;
        _fileErrorCode = NO_MEDIA_TRACKS_IN_FILE;
        return;
      }
          }

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
          //if parsing was successful, parse KDDI Telop Text
          if (_success && _kddiTelopElement)
          {
               process_kddi_telop_text();
          }
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

#if defined(FEATURE_QTV_SKT_MOD_MIDI)
          process_mod_midi_atom();
#endif /* FEATURE_QTV_SKT_MOD_MIDI */
          /* if video track does not have STSS table, then all video frames are SYNC frame */
          if(_success && m_playVideo)
          {
               /* if clip has video and we don't have STSS table, we can't reposition */
               video_fmt_stream_info_type *p_track;
               for(uint32 index = 0; index < m_videoFmtInfo.num_streams; index++)
               {
                    p_track = m_videoFmtInfo.streams + index;
                    if( p_track->type == VIDEO_FMT_STREAM_VIDEO )
                    {
                         video_fmt_mp4r_context_type  *context;
                         video_fmt_mp4r_stream_type   *stream;
                         context = (video_fmt_mp4r_context_type *) m_videoFmtInfo.server_data;
                         if(index <= context->num_streams)
                         {
                              stream = &context->stream_state [index];
                              if(!stream->stss.table_size)
                              {
                                   m_allSyncVideo = true;
                                   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "no Video STSS, assuming all frames are sync frames");
                              }
                         }
                    }
               }
          } /* end of if(m_playVideo) */

          if((_success)&& (m_playAudio))
          {
               video_fmt_stream_info_type *p_track=0;
               uint32 framelength=0;
               for(uint32 index = 0; index < m_videoFmtInfo.num_streams; index++)
               {
                    p_track = m_videoFmtInfo.streams + index;
                    if( (p_track)                                                   &&
                         (p_track->type == VIDEO_FMT_STREAM_AUDIO)                   &&
                         (p_track->subinfo.audio.format==VIDEO_FMT_STREAM_AUDIO_AMR) )
                    {
                         video_fmt_sample_info_type sampleInfo;
                         memset(&sampleInfo, 0x0, sizeof(video_fmt_sample_info_type));
                         bool retStat = false;
                         uint32 sampleId = 0;
                         do
                         {
                              //Keep reading untill we get valid sample with non zero DELTA and non zero SIZE
                              retStat = (getSampleInfo(p_track->stream_num, sampleId, 1, &sampleInfo)>0)?true:false;
                              sampleId++;
                         }while( (retStat) && (m_mp4ReadLastStatus[p_track->stream_num] == VIDEO_FMT_IO_DONE) &&
                              (!sampleInfo.delta || !sampleInfo.size) );


                         if( !retStat || (m_mp4ReadLastStatus[p_track->stream_num] != VIDEO_FMT_IO_DONE) )
                         {
                              _success = false;
                              QTV_MSG_PRIO(QTVDIAG_GENERAL,
                                   QTVDIAG_PRIO_HIGH,
                                   "parseFirstFragment encountered an error making (_success = false)");
                              return;
                         }

                         /* for some clips, SamplesPerFrame value is wrongly set to 10 and the correct
                         value was one. This check is to fix those clips and we also try to
                         minimize the scope of this fix by checking this value in clip and
                         size of first AMR sample from a clip in question/given clip*/
                         if( ((sampleInfo.size==32) || (sampleInfo.size==13)|| (sampleInfo.size==21) || (sampleInfo.size==18))&&
                              (p_track->subinfo.audio.audio_params.frames_per_sample==10) )
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                                   QTVDIAG_PRIO_ERROR,
                                   "SamplesPerFrame=%d for AMR track, but using 1.",
                                   p_track->subinfo.audio.audio_params.frames_per_sample);
                              p_track->subinfo.audio.audio_params.frames_per_sample = 1;
                         }
                         if(p_track->subinfo.audio.audio_params.frames_per_sample && p_track->media_timescale)
                         {
                              framelength=
                                   ((sampleInfo.delta*1000)/(p_track->subinfo.audio.audio_params.frames_per_sample))/(p_track->media_timescale);
                         }
                         if((framelength<(DURATION_OF_AMR_FRAME_BLOCK-1))||(framelength>(DURATION_OF_AMR_FRAME_BLOCK+1)))
                         {
                              //CMX validates AMR frame contents so QTV check is redundant.
                              //_success = false;
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Invalid(?) AMR Audio content:Duration of Frame Block=%d",framelength);
                         }
                    }
               }//for(uint32 index = 0; index < m_videoFmtInfo.num_streams; index++)
          }//if((_success)&& (m_playAudio))
     }//if(_success)
}

#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD||defined FEATURE_QTV_PSEUDO_STREAM
/*===========================================================================

FUNCTION  initializeVideoFMT

DESCRIPTION
Public method used to initialize VideoFMT

===========================================================================*/
boolean Mpeg4File::initializeVideoFMT ( void )
{
  boolean returnStatus = TRUE;

  // Parse the file as an MP4 file, building a list of atom information
  // structures.
  video_fmt_open (mp4ParseStatusCallback, this, VIDEO_FMT_MP4,0xff);

  while ((m_mp4ParseLastStatus != VIDEO_FMT_INIT)
    && (m_mp4ParseLastStatus != VIDEO_FMT_FAILURE)
    && (m_mp4ParseLastStatus != VIDEO_FMT_DATA_CORRUPT)
    )
  {
    if((m_mp4ParseContinueCb == NULL) ||
      (m_mp4ParseServerData == NULL))
      break;
    else
      m_mp4ParseContinueCb(m_mp4ParseServerData);
  }

  if( (m_mp4ParseLastStatus == VIDEO_FMT_FAILURE) ||
    (m_mp4ParseLastStatus == VIDEO_FMT_DATA_CORRUPT) ||
    (m_corruptFile == true) )
    return FALSE;

  if (m_mp4ParseLastStatus == VIDEO_FMT_INIT && bHttpStreaming)
  {
    m_currentParseFragment = 0;
    bGetMetaDataSize = TRUE;
    bDataIncomplete = TRUE;
    (void)ParseStream();
    returnStatus = TRUE;
  }
  else if((m_mp4ParseLastStatus == VIDEO_FMT_INIT) && !bHttpStreaming)
  {
    if(ParseStream())
      returnStatus = TRUE;
    else
      returnStatus = FALSE;
  }

  return returnStatus;
}

/*===========================================================================

FUNCTION  getMetaDataSize

DESCRIPTION
Public method used to determine the meta-data size of the fragment.

===========================================================================*/
bool Mpeg4File::getMetaDataSize ( void )
{
  video_fmt_mp4r_context_type *video_fmt_parse_context;

  QCUtils::EnterCritSect(&videoFMT_Access_CS);
  m_videoFmtInfo.fragment_size_cb (m_videoFmtInfo.server_data,
    m_currentParseFragment);

  while ((m_mp4ParseLastStatus != VIDEO_FMT_FRAGMENT_SIZE)
    && (m_mp4ParseLastStatus != VIDEO_FMT_FAILURE)
    && (m_mp4ParseLastStatus != VIDEO_FMT_DATA_INCOMPLETE)
    && (m_mp4ParseLastStatus != VIDEO_FMT_DATA_CORRUPT)
     && (m_mp4ParseLastStatus != VIDEO_FMT_INFO))
  {
    if((m_mp4ParseContinueCb == NULL) ||
      (m_mp4ParseServerData == NULL))
      break;
    else
      m_mp4ParseContinueCb (m_mp4ParseServerData);
  }
  QCUtils::LeaveCritSect(&videoFMT_Access_CS);

  switch(m_mp4ParseLastStatus)
  {
  case VIDEO_FMT_FAILURE:
  case VIDEO_FMT_DATA_CORRUPT:
    return false;

  case VIDEO_FMT_FRAGMENT_SIZE:
    video_fmt_parse_context = (video_fmt_mp4r_context_type *)m_videoFmtInfo.server_data;
    if(m_currentParseFragment == video_fmt_parse_context->fragment_requested)
    {
      if(m_currentParseFragment == 0)
        m_minOffsetRequired = video_fmt_parse_context->fragment_size;
      else
      m_minOffsetRequired += video_fmt_parse_context->fragment_size;
      /* Added this to support 3g2 files */
      mdat_size = video_fmt_parse_context->mdat_size;
      fragmentNumber = video_fmt_parse_context->fragment_requested;
      m_currentParseFragment = fragmentNumber + 1;
      /* Added this to support 3g2 files  */
    }
    bDataIncomplete = FALSE;
    return true;

  case VIDEO_FMT_DATA_INCOMPLETE:
    bDataIncomplete = TRUE;
    return false;

  default:
    break;
  }
  return false;
}

/*===========================================================================

FUNCTION  CanPlayTracks

DESCRIPTION
Public method used to determine if all the media tracks in this context,
can be played for the specified amouunt of time.

===========================================================================*/
bool Mpeg4File::CanPlayTracks(uint32 pbTime)
{
  //If audio only/video only clip then the m_trackCount
  //inthe other context is zero: so return true.
  if(!m_trackCount)
    return true;

  bool returnStatus = true;
  uint32 reqSampleNum = 0;

  if(pbTime <= m_pbTime)
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "CanPlayTracks: pbTime=%d, m_pbTime=%d", pbTime, m_pbTime);
    return true;
  }

  /*Check if all the tracks are playable for atleast pbTime*/
  for(uint8 index = 0; index < m_trackCount; index++)
  {
    video_fmt_sample_info_type sampleInfo;

    // Convert timestamp in miliseconds to timescale
    uint32 timescaledTime = (uint32)(((float)pbTime/1000.0F)*m_track[index]->media_timescale);

    uint32 streamNum = m_track[index]->stream_num;

    if((m_videoFmtInfo.streams[streamNum].type == VIDEO_FMT_STREAM_VIDEO) && !m_hasVideo)
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "CanPlayTracks: No video..m_playVideo=%d", m_playVideo);
      continue;
    }
    else if((m_videoFmtInfo.streams[streamNum].type == VIDEO_FMT_STREAM_AUDIO) && !m_hasAudio)
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "CanPlayTracks: No audio..m_playVideo=%d", m_playVideo);
      continue;
    }
    else if((m_videoFmtInfo.streams[streamNum].type == VIDEO_FMT_STREAM_TEXT) && !m_hasText)
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "CanPlayTracks: No text..m_playVideo=%d", m_playVideo);
      continue;
    }

    uint32 maxFrames  =  m_track[index]->frames;

    if(!m_pbTime)
    {
      m_track[index]->prevReqSampleNum = 0;
    }

    returnStatus = false;


    timescaledTime= MIN(timescaledTime,(m_track[index]->media_duration)/(m_track[index]->media_timescale));
    for (reqSampleNum = m_track[index]->prevReqSampleNum;
      reqSampleNum < maxFrames; ++reqSampleNum )
    {
      if (getSampleInfo(streamNum, reqSampleNum, 1, &sampleInfo) > 0 )
      {
        returnStatus = true;
        if ((sampleInfo.time + sampleInfo.delta) >= timescaledTime)
        {
          if(m_track[index]->type == VIDEO_FMT_STREAM_VIDEO)
          {
            /*For the video track we should atleast be able to send QTV_3GPP_MIN_NUM_VIDEO_FRAMES_TO_BUFFER
            to videoDec */
            if (reqSampleNum >= (m_sampleInfo[streamNum].sample + QTV_3GPP_MIN_NUM_VIDEO_FRAMES_TO_BUFFER))
            {
              m_track[index]->prevReqSampleNum = reqSampleNum;
              break;
            }
          }
          else
          {
            m_track[index]->prevReqSampleNum = reqSampleNum;
            break;
          }
        }
      }
      else
      {
        returnStatus = false;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "CanPlayTracks: getSampleInfo ERROR");
        break;
      }
    } // for(maxFrames)

    if(!returnStatus)
      break;

    /* Condition: pbTime > playable media time; then
    finish playing the remaining samples */
    if(reqSampleNum == maxFrames)
    {
      m_track[index]->prevReqSampleNum = reqSampleNum-1;
    }

    if(sampleInfo.offset && (sampleInfo.offset >= m_track[index]->header))
    {
      sampleInfo.offset -= m_track[index]->header;
    }

    /*Fetch the abs file offset for the sample at pbTime.*/
    uint32 absFileOffset = getSampleAbsOffset (streamNum,
      sampleInfo.offset,
      sampleInfo.size);

    if((!absFileOffset) || (m_wBufferOffset>0 && absFileOffset >= m_wBufferOffset))
    {
      returnStatus = false;
      QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "CanPlayTracks: absFileOffset=%d, m_wBufferOffset=%d, streamNum=%d",
        absFileOffset, m_wBufferOffset,streamNum);
      break;
    }

  }// for(m_trackCount)

  if(returnStatus)
  {
    //All the tracks can be played for atleast pbTime.
    m_pbTime = pbTime;
  }

  return returnStatus;
}

/*===========================================================================

FUNCTION  getSampleAbsOffset

DESCRIPTION
Public method used to fetch the absolute file offset for a given
media sample stream offset.

===========================================================================*/
uint32 Mpeg4File::getSampleAbsOffset (uint32 streamNum,
                                      uint32 sampleOffset,
                                      uint32 sampleSize)
{
  int loop = 0;
  bool bDone = false;
  m_absFileOffset[streamNum] = 0;
  m_videoFmtInfo.abs_file_offset_cb(streamNum,
    sampleOffset,
    sampleSize,
    m_videoFmtInfo.server_data,
    mp4ReadStatusCallback,
    &(m_clientData[streamNum]) );

  while ( !bDone )
  {
    while ( (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_ABS_FILE_OFFSET) &&
      (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_FAILURE) &&
      (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_BUSY) &&
      (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_DATA_CORRUPT) &&
      (loop<MPEG4_VIDEOFMT_MAX_LOOP) )
    {
      m_mp4ReadContinueCb[streamNum] (m_mp4ReadServerData[streamNum]);
      loop++;
    }

    if( loop >= MPEG4_VIDEOFMT_MAX_LOOP )
    {
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "getSampleAbsOffset: VideoFMT hangs. StreamNum=%d, startingAt %d", streamNum, sampleOffset);
      bDone = TRUE;
    }

    switch ( m_mp4ReadLastStatus[streamNum] )
    {
    case VIDEO_FMT_BUSY:
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "getSampleAbsOffset:BUSY FATAL starting at %d", sampleOffset);
        return 0;
      }

    case VIDEO_FMT_ABS_FILE_OFFSET:
      bDone = true;
      break;

    case VIDEO_FMT_FAILURE:
    case VIDEO_FMT_DATA_CORRUPT:
      {
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "getSampleAbsOffset:FAILURE FATAL starting at %d, size %d", sampleOffset, sampleSize);
        return 0;
      }
    default:
      break;
    }
  }

  return m_absFileOffset[streamNum];
}

/*===========================================================================

FUNCTION  updateBufferWritePtr

DESCRIPTION
Public method used to update the write buffer offset during Http streaming.

===========================================================================*/
void Mpeg4File::updateBufferWritePtr ( uint32 writeOffset )
{
  //Executing in the UI thread context.
  m_wBufferOffset = writeOffset;
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "m_wBufferOffset = %d",m_wBufferOffset);
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "m_minOffsetRequired = %d",m_minOffsetRequired);
  if((parserState == Common::PARSER_PAUSE) || (parserState == Common::PARSER_RESUME)
    && m_wBufferOffset>= m_minOffsetRequired)
  {
    //check if we got sufficient data to start parsing the
    //meta data.
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "sendParseHTTPStreamEvent");
    sendParseHTTPStreamEvent();
  }
}

/*===========================================================================

FUNCTION  parseHTTPStream

DESCRIPTION
Public method used to parse the Http Stream.

===========================================================================*/
bool Mpeg4File::parseHTTPStream ( void )
{
  //First Make sure next fragment moov/moof is available.
  if(peekMetaDataSize(m_currentParseFragment))
  {
    if(Initialized == FALSE)
    {
     //Then parse it and check for canPlayTracks().
      if(Parsed == FALSE && !parseMetaData())
      {
        //QTV_PS_PARSER_STATUS_PAUSED
        sendParserEvent(Common::PARSER_RESUME);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_RESUME, m_playVideo=%d",m_playVideo);
        return false;
      }
      Parsed = TRUE;
      if((parserState == Common::PARSER_RESUME || parserState == Common::PARSER_PAUSE)
        && CanPlayTracks(m_startupTime))
    {
        m_currentParseFragment++;
        Initialized = TRUE;
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_READY, m_playVideo=%d",m_playVideo);
      sendParserEvent(Common::PARSER_READY);
        return true;
    }
      return false;
  }
  else
  {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_READY, m_playVideo=%d",m_playVideo);
      sendParserEvent(Common::PARSER_READY);
      return true;
  }
  }
  return false;
}

/*===========================================================================

FUNCTION  parseMetaData

DESCRIPTION
Public method used to parse the fragment meta-data.

===========================================================================*/
bool Mpeg4File::parseMetaData ( void )
{
  bool returnValue = false;

  if((m_videoFmtInfo.parse_fragment_cb == NULL) ||
    (m_videoFmtInfo.server_data == NULL))
    return false;

  m_videoFmtInfo.parse_fragment_cb (m_videoFmtInfo.server_data);

  while ((m_mp4ParseLastStatus != VIDEO_FMT_INFO)
    && (m_mp4ParseLastStatus != VIDEO_FMT_FAILURE)
    && (m_mp4ParseLastStatus != VIDEO_FMT_FRAGMENT)
    && (m_mp4ParseLastStatus != VIDEO_FMT_DATA_INCOMPLETE)
    && (m_mp4ParseLastStatus != VIDEO_FMT_DATA_CORRUPT)
    && (_fileErrorCode == EVERYTHING_FINE))
  {
    if((m_mp4ParseContinueCb == NULL) ||
      (m_mp4ParseServerData == NULL))
      break;
    else
      m_mp4ParseContinueCb (m_mp4ParseServerData);
  }
  if(_fileErrorCode != EVERYTHING_FINE)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Mp4FragmentFile::parseFragment _fileErrorCode != EVERYTHING_FINE");
     return false;
  }
  if((m_parsedEndofFile || m_parsedEndofFragment) && !m_corruptFile)
  {
      if(m_mp4ParseLastStatus == VIDEO_FMT_FRAGMENT)
      {
          m_isFragmentedFile = true;
      }
      if (bQtvPlayerPaused)
      {
        //bQtvPlayerPaused: used to initially kickoff playing after the first
        //Http fragment is parsed.
        //QTV_PS_PARSER_STATUS_RESUME
        sendParserEvent(Common::PARSER_RESUME);
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_RESUME");
        bQtvPlayerPaused = FALSE;
      }
      bsendParseFragmentCmd = FALSE;

    returnValue = true;
#ifdef CHECK_MP4_MAX_VIDEO_BITRATE
	for (int i = 0; i < m_trackCount; i++)
	{
           if (m_track[i]->type == VIDEO_FMT_STREAM_VIDEO)
	   {
		if (m_track[i]->dec_specific_info.maxbitrate > MAX_MP4_VIDEO_BIT_RATE)
		{
		   _fileErrorCode = TRACK_VIDEO_UNSUPPORTED_BITRATE;
		   break;
		}
	   }
	}
#endif
  }
  else
  {
    returnValue = false;
  }

  // Let any waiting process know that we are done with fragment
  return returnValue;
}

/*===========================================================================

FUNCTION  sendParseHTTPStreamEvent

DESCRIPTION
Public method used to switch contexts and call the parseHttpStream.

===========================================================================*/
void Mpeg4File::sendParseHTTPStreamEvent(void)
{
  if (m_pMpeg4Player)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser Event:QTV_HTTP_BUFFER_UPDATE");
    QTV_HTTP_BUFFER_UPDATE_type *pEvent = QCCreateMessage(QTV_HTTP_BUFFER_UPDATE, m_pMpeg4Player);


    if (pEvent)
    {
      pEvent->bHasAudio = m_playAudio ;
      pEvent->bHasVideo = m_playVideo;
      pEvent->bHasText  = m_playText;
      QCUtils::PostMessage(pEvent, 0, NULL);
    }
  }
}

/*===========================================================================

FUNCTION  sendHTTPStreamUnderrunEvent

DESCRIPTION
Public method used to switch contexts and notify the player about buffer-underrun.

===========================================================================*/
void Mpeg4File::sendHTTPStreamUnderrunEvent(void)
{
  sendParserEvent(Common::PARSER_PAUSE);
}

/*===========================================================================

FUNCTION  sendParserEvent

DESCRIPTION
Public method used send parser events

===========================================================================*/
void Mpeg4File::sendParserEvent(Common::ParserStatusCode status)
{
  parserState = status;
  if(m_pMpeg4Player)
  {
	 QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser Event:QTV_HTTP_EVENT");
	 QTV_HTTP_EVENT_type *pEvent = QCCreateMessage(QTV_HTTP_EVENT, m_pMpeg4Player);
    if (pEvent)
    {
      pEvent->status = status;
      pEvent->bHasVideo = m_playVideo;
      pEvent->bHasAudio = m_playAudio;
      pEvent->bHasText = m_playText;
      QCUtils::PostMessage(pEvent, 0, NULL);
    }
  }
}

#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

/*===========================================================================

FUNCTION  resetPlaybackPos

DESCRIPTION
Public method used to reset the playback position after repositioning was successful.
Called by the player if it later decides not to reposition e.g: when there is
insufficient data to play after reposiitoning in case of 3GPP_Progressive_Dnld.

===========================================================================*/
void Mpeg4File::resetPlaybackPos(uint32 tId)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(tId);

  if ( !p_track )
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "resetPlayback, unknown track id = %d", tId);
    return;
  }

  uint32 streamNum = p_track->stream_num;

  if ( m_reposStreamPending )
  {
    if ( m_reposStreamPending & maskByte[streamNum] )
    {
      m_reposStreamPending &= ~maskByte[streamNum];
    }

    if(getSampleInfo (streamNum, m_nextSample[streamNum]-1, 1, &m_sampleInfo[streamNum]) <= 0 )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "resetPlaybackPos: getSampleInfo ERROR");
    }
  }
  return;
}


/* <EJECT> */
/*===========================================================================

FUNCTION  getSampleInfo

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
int32 Mpeg4File::getSampleInfo (uint32 streamNum,
                                uint32 startingSample,
                                uint32 sampleCount,
                                video_fmt_sample_info_type *buffer)
{
  int loop = 0;
  bool bDone = false;
  m_iodoneSize[streamNum] = 0;

  m_videoFmtInfo.sample_info_cb(  streamNum,
    startingSample,
    sampleCount,
    buffer,
    m_videoFmtInfo.server_data,
    mp4ReadStatusCallback,
    &(m_clientData[streamNum]) );

  while ( !bDone )
  {
    while ( (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_IO_DONE) &&
      (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_FAILURE) &&
      (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_BUSY) &&
      (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_DATA_CORRUPT) &&
      (loop < MPEG4_VIDEOFMT_MAX_LOOP) )
    {
      m_mp4ReadContinueCb[streamNum] (m_mp4ReadServerData[streamNum]);
      loop++;
    }

    if( loop >= MPEG4_VIDEOFMT_MAX_LOOP )
    {
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoFMT hangs. Stream=%d, StartSample=%d", streamNum, startingSample);
      bDone = TRUE;
    }

    switch ( m_mp4ReadLastStatus[streamNum] )
    {
    case VIDEO_FMT_BUSY:
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "getSampleInfo FATAL stuck on sample %d", startingSample);
        return READ_FAILURE;
      }
    case VIDEO_FMT_IO_DONE:
      bDone = true;
      break;
    case VIDEO_FMT_FAILURE:
    case VIDEO_FMT_DATA_CORRUPT:
      {
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "getSampleInfo FATAL sample %d, count %d", startingSample, sampleCount);
        return READ_FAILURE;
      }
    default:
      break;
    }
  }

  return (int32)m_iodoneSize[streamNum];
}


/* <EJECT> */
/*===========================================================================

FUNCTION  getSample

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
int32 Mpeg4File::getSample (uint32 streamNum,
                            video_fmt_data_unit_type unitDef,
                            uint32 startingUnit,
                            uint32 unitCount,
                            uint8 *buffer)
{
  int loop = 0;
  bool bDone = false;

  m_iodoneSize[streamNum] = 0;

#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || defined FEATURE_QTV_PSEUDO_STREAM

  video_fmt_mp4r_context_type *context = (video_fmt_mp4r_context_type *) m_videoFmtInfo.server_data;
  video_fmt_mp4r_stream_type  *stream = &context->stream_state [streamNum];
  if(bHttpStreaming)
  {
    if(m_fpFetchBufferedDataSize)
    {
       boolean bEndOfData = false;
       uint32 userData = 0;
       #ifdef FEATURE_QTV_DRM_DCF
         userData = (uint32)m_inputStream;
       #endif
      //Pull interface so pull dnld data size from OEM
      (*m_fpFetchBufferedDataSize)(userData, &m_wBufferOffset,&bEndOfData, m_QtvInstancehandle);
    }
    stream->wBufferOffset = m_wBufferOffset;
  }
  else
  {
    stream->wBufferOffset = 0;
  }

#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

  m_videoFmtInfo.read_cb( streamNum,
    unitDef,
    startingUnit,
    unitCount,
    buffer,
    m_videoFmtInfo.server_data,
    mp4ReadStatusCallback,
    &(m_clientData[streamNum]) );

  while ( !bDone )
  {
    while ( (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_IO_DONE) &&
      (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_FAILURE) &&
      (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_BUSY) &&
#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD||defined FEATURE_QTV_PSEUDO_STREAM
      (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_DATA_INCOMPLETE) &&
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_DATA_CORRUPT) &&
      (loop<MPEG4_VIDEOFMT_MAX_LOOP) )
    {
      m_mp4ReadContinueCb[streamNum] (m_mp4ReadServerData[streamNum]);
      loop++;
    }

    if( loop >= MPEG4_VIDEOFMT_MAX_LOOP )
    {
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoFMT hangs. StreamNum=%d, startingAt %d", streamNum, startingUnit);
      bDone = TRUE;
    }

    switch ( m_mp4ReadLastStatus[streamNum] )
    {
    case VIDEO_FMT_BUSY:
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "getSample FATAL starting at %d", startingUnit);
        return -1;
      }
    case VIDEO_FMT_IO_DONE:
      bDone = true;
      break;

#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD ||defined FEATURE_QTV_PSEUDO_STREAM
    case VIDEO_FMT_DATA_INCOMPLETE:
      /*Inform data-underrun to the player.*/
      sendHTTPStreamUnderrunEvent();
      QTV_MSG_PRIO4(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_DATA_INCOMPLETE: startingUnit=%d, unitCount=%d, wBufferOffset=%d, m_playVideo=%d",
        startingUnit, unitCount,stream->wBufferOffset,m_playVideo);
      return INSUFFICIENT_DATA;
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

    case VIDEO_FMT_FAILURE:
    case VIDEO_FMT_DATA_CORRUPT:
      {
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "getSample FATAL starting at %d, size %d", startingUnit, unitCount);
        return READ_FAILURE;
      }
    default:
      break;
    }
  }

  return (int32)m_iodoneSize[streamNum];
}

/*===========================================================================

FUNCTION  setAudioPlayerData

DESCRIPTION

===========================================================================*/
void Mpeg4File::setAudioPlayerData( const void *client_data )
{
  AudioPlayerPtr = (AudioPlayer *)client_data;
}

/*===========================================================================

FUNCTION  setVideoPlayerData

DESCRIPTION

===========================================================================*/
void Mpeg4File::setVideoPlayerData( const void *client_data )
{
  VideoPlayerPtr = (VideoPlayer *)client_data;
}

/*===========================================================================

FUNCTION  setTextPlayerData

DESCRIPTION

===========================================================================*/
void Mpeg4File::setTextPlayerData( const void *client_data )
{
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  TextPlayerPtr = (TimedText *)client_data;
#endif
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getNextMediaSample

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
int32 Mpeg4File::getNextMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 &index)
{
  int32  returnVal = 0;
  video_fmt_stream_info_type *p_track = 0;
#ifdef FEATURE_QTV_PDCF
  bool isOMADRMV2Encrypted = false;
#endif /* FEATURE_QTV_PDCF */

  //Fetch the track_info.
  p_track = getTrackInfoForID(id);

  /* track not found */
  if(!p_track)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Try to get data from unknown track ID=%d", id);
    return 0;
  }
#ifdef FEATURE_QTV_PDCF
  for(int i=0; i<VIDEO_FMT_MAX_MEDIA_STREAMS; i++)
  {
    if(( m_EncryptionType[i].track_id == p_track->track_id )
       && (m_EncryptionType[i].encryptionType == QtvPlayer::ENCRYPT_OMA_DRM_V2))
    {
      /* This track is encrypted with OMA DRM V2 encryption mechanism */
      isOMADRMV2Encrypted = true;
      /* Allocate the buffer for encrypted data */
      if((m_pEncryptedDataBuffer == NULL) && size)
      {
        m_pEncryptedDataBuffer = (uint8*) QTV_Malloc(size);
      }
      if (m_pEncryptedDataBuffer == NULL)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failed to allocate memory for m_pEncryptedDataBuffer");
        return 0;
      }
      break;
    }
  }
#endif /* FEATURE_QTV_PDCF */
  uint32 streamNum = p_track->stream_num;
  uint32 header = p_track->header;

  if ( m_reposStreamPending )
  {
    if ( m_reposStreamPending & maskByte[streamNum] )
    {
      m_nextSample[streamNum] = m_nextReposSample[streamNum];
      m_reposStreamPending &= ~maskByte[streamNum];
    }
  }

  returnVal = getSampleInfo (streamNum,
    m_nextSample[streamNum],
    1,
    &m_sampleInfo[streamNum]);

  if ( returnVal < 0 )
  {
    return returnVal;
  }
  else if ( returnVal == 0 )
  {
    if((returnVal = getSampleInfoError(p_track)) <= 0)
    {
      return returnVal;
    }
  }

  if(returnVal > 0)
  {
    if(size < (m_sampleInfo[streamNum].size))
    {
      if( m_fileSize && (m_sampleInfo[streamNum].size >= m_fileSize) )
      {
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Sample size=%ld bigger than File size=%ld", m_sampleInfo[streamNum].size, m_fileSize);
        return READ_FAILURE;
      }
      p_track->largest = MAX(m_sampleInfo[streamNum].size,p_track->largest);
#ifdef FEATURE_QTV_PDCF
      /* If the sample size is greater than the size of the
        m_pEncryptedDataBuffer then free the buffer so that next time
        we can allocate with the size of the audio or video requesting */
      if(isOMADRMV2Encrypted && (m_pEncryptedDataBuffer != NULL))
      {
        QTV_Free(m_pEncryptedDataBuffer);
      }
#endif /* FEATURE_QTV_PDCF */
      return INSUFFICIENT_BUFFER_SIZE;
    }
  }

  if ( !m_nextSample[streamNum] )
  {
    m_sampleInfo[streamNum].size -= header;
    m_sampleInfo[streamNum].offset += header;
  }

  if(m_sampleInfo[streamNum].size == 0)
  {
    m_nextSample[streamNum] = m_sampleInfo[streamNum].sample + 1;
    return ZERO_SAMPLE_SIZE;
  }
#ifdef FEATURE_QTV_PDCF
 /* If This track is encrypted with OMA DRM V2 encryption mechanism */
  if(isOMADRMV2Encrypted)
  {
    returnVal = getSample (streamNum,
      VIDEO_FMT_DATA_UNIT_BYTE,
      m_sampleInfo[streamNum].offset,
      m_sampleInfo[streamNum].size,
      m_pEncryptedDataBuffer);
  }
  else
#endif /*  FEATURE_QTV_PDCF */
  {
  returnVal = getSample (streamNum,
    VIDEO_FMT_DATA_UNIT_BYTE,
    m_sampleInfo[streamNum].offset,
    m_sampleInfo[streamNum].size,
    buf);
  }
  if ( returnVal <=0 )
  {
#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD ||defined FEATURE_QTV_PSEUDO_STREAM
    if(returnVal == INSUFFICIENT_DATA)
      return INSUFFICIENT_DATA;
    else
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
      if(returnVal == READ_FAILURE)
      {
  		/* If this READ_FAILURE is because of corrupted audio/video data
                  then return READ_FAILURE */
        if((m_pMpeg4Player && !m_pMpeg4Player->m_bMediaAbort)
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
            ||(bHttpStreaming)
#endif
    	  )
  		{
  		/* In case of corrupted audio/video data returning 0 instead of READ_FAILURE for KDDI specific code */
		/* Returning 0 implied as MEDIA_END at the QTV layer and returning READ_FAILURE implied as DATA_ERROR */
#ifdef FEATURE_PARSER_KDDI_MEDIA_END
#error code not present
#else
     		return READ_FAILURE;
#endif
  		}
  		else
        {
    	/*
          	  * In case of local playback, when read fails, simply return
	         * 0 to indicate DATA_END. This will avoid reporting AUDIO_ERROR/VIDEO_ERROR
       	  * to OEM.
	         */
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
          if(!bHttpStreaming)
#endif
    	  {
            return 0;
          }
        }
	  }
	  else
	      return 0;
  }
#ifdef FEATURE_QTV_PDCF
  /* This is to call the registered call back to decrypt the audio/video
     sample that we read based on the track_id*/
  if(isOMADRMV2Encrypted)
  {
   /* Size of the decrypted sample */
    uint32 dwDecryptedDataSize = size;

    if(m_pEncryptedDataBuffer)
    {
#ifdef FEATURE_QTV_DRM_DCF
      if(m_inputStream)
      {
        IxErrnoType result = ((IxStreamMedia*)(m_inputStream))->Decrypt(p_track->track_id,
                                                                        m_pEncryptedDataBuffer,
                                                                        returnVal,
                                                                        buf,
                                                                        &dwDecryptedDataSize);
        if(result != E_SUCCESS)
        {
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "IxStreamMedia->Decrypt failed. Result %d",result);
          return FAILED_TO_DECRYPT;
        }
      }
      /* call registered decrypt method to decrypt the sample */
      else if(m_pDRMDecryptFunction)
#else
      /* call registered decrypt method to decrypt the sample */
      if(m_pDRMDecryptFunction)
#endif
      {
        if( FALSE == m_pDRMDecryptFunction( p_track->track_id,
                                            m_pEncryptedDataBuffer,
                                            buf,
                                            returnVal,
                                            &dwDecryptedDataSize,
                                            m_pDRMClientData ) )
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Failed to Decrypt the sample");
          return FAILED_TO_DECRYPT;
        }
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation failure. Could not decrypt frame");
      return FAILED_TO_DECRYPT;
    }
    /* decrypting successful */
    /* after decrypting, size of sample should not change */
    ASSERT(dwDecryptedDataSize == m_sampleInfo[streamNum].size);
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "decrypting successful. encrypt size %d, decrypt size %d", returnVal, dwDecryptedDataSize);
  }
#endif /* FEATURE_QTV_PDCF */
  m_nextSample[streamNum] = m_sampleInfo[streamNum].sample + 1;

  index = 0;
  size = m_sampleInfo[streamNum].size;

  return (int32)m_sampleInfo[streamNum].size;

}

/* ======================================================================
FUNCTION
Mpeg4File::resetMediaPlayback

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
bool Mpeg4File::resetMediaPlayback(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);
  if ( !p_track ){
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "resetMediaPlayback, unknown track id = %d", id);
    return false;
  }

  uint32 streamNum = p_track->stream_num;

  memset(&m_sampleInfo[streamNum], 0x0, sizeof(video_fmt_sample_info_type));
  memset(&m_nextSample[streamNum], 0x0, sizeof(uint32));
  return true;

}

/* <EJECT> */
/*===========================================================================

FUNCTION  getMediaTimestampForCurrentSample

DESCRIPTION
Public method used to request the timestamp for the sample currently processed

===========================================================================*/
uint32 Mpeg4File::getMediaTimestampForCurrentSample(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);
  if ( !p_track ){
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "getMediaTimestampForCurrentSample, unknown track id = %d", id);
    return 0;
  }
  uint32 streamNum = p_track->stream_num;

  return m_sampleInfo[streamNum].time;
}

/*===========================================================================

FUNCTION  getMediaTimestampDeltaForCurrentSample

DESCRIPTION
Public method used to request the delta for the sample currently processed

===========================================================================*/
uint32 Mpeg4File::getMediaTimestampDeltaForCurrentSample(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);
  if ( !p_track ){
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "getMediaTimestampDeltaForCurrentSample, unknown track id = %d", id);
    return 0;
  }
  uint32 streamNum = p_track->stream_num;
  if(p_track->media_timescale)
  {
    return (uint32)((((float)m_sampleInfo[streamNum].delta)/ p_track->media_timescale) * 1000.0F);
  }
  else
  {
    return m_sampleInfo[streamNum].delta;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getSyncSampleInfo

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
bool Mpeg4File::getSyncSampleInfo (uint32 streamNum,
                                   uint32 sampleNum,
                                   bool   reverse,
                                   video_fmt_sample_info_type *buffer)
{
  int loop = 0;

  m_videoFmtInfo.sync_sample_cb(  streamNum,
    sampleNum,
    (boolean)reverse,
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
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "VideoFMT hangs. StreamNum=%d, sampleNum %d", streamNum, sampleNum);
  }

  if( (m_mp4SyncLastStatus[streamNum] == VIDEO_FMT_FAILURE) ||
    (m_mp4SyncLastStatus[streamNum] == VIDEO_FMT_BUSY) ||
    (m_mp4SyncLastStatus[streamNum] == VIDEO_FMT_DATA_CORRUPT))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "VideoFMT get sync sample failed.");
    return FALSE;
  }

  if(m_iodoneSize[streamNum] == 0)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "VideoFMT get sync sample could not find a sample.");
    return FALSE;
  }
  return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getSampleAtTimestamp

DESCRIPTION
Public method used to get the sample info for a sample placed at a given
timestamp (in the track timescale).

===========================================================================*/

bool Mpeg4File::getSampleAtTimestamp(video_fmt_stream_info_type *p_track,
                                     uint32                      timestamp,
                                     bool                        lRewind,
                                     video_fmt_sample_info_type *sampleInfo)
{
  bool retStat = false;
  uint32 streamNum = p_track->stream_num;
  int32 maxFrames = (int32)p_track->frames;
  int32 curSample = (int32)m_sampleInfo[streamNum].sample;

  int32 reqSampleNum;

  if ( lRewind )
  {

    for ( reqSampleNum = (int32)m_sampleInfo[streamNum].sample;
      reqSampleNum >= 0; --reqSampleNum )
    {
      if ( getSampleInfo(streamNum, (uint32)reqSampleNum, 1, sampleInfo) > 0 )
        retStat = true;
      if ( sampleInfo->time == timestamp )
      {
        break;
      }
      else if(sampleInfo->time < timestamp)
      {
        /*
        * For text/audio track, we should not pick the sample whose TS > what is being requested.
        * Otherwise, incorrect sample will be displayed for duration
        * which could be > sample's own duration. Example below:
        *
        * Lets say each text sample is of 5 seconds, and user does RW by 2 seconds,
        * when playback time is 5 seconds.
        * Now, if we pick sample#1, since it's TS (5 seconds) is > TS being requested(3 seconds),
        * then sample#1 would be displayed for 7 seconds (5 second from it's own duration + (5-3))
        *
        * Thus, picking sample#0 would make sure that it gets displayed only from 2 seconds.
        *
        */
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
    for ( reqSampleNum = (int32)m_sampleInfo[streamNum].sample;
      reqSampleNum < maxFrames; ++reqSampleNum )
    {
      if ( getSampleInfo(streamNum, (uint32)reqSampleNum, 1, sampleInfo) > 0 )
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
          (void)getSampleInfo(streamNum, (uint32)(reqSampleNum-1), 1, sampleInfo);
        break;
      }
    } // for
  } // forward
  return retStat;
}

/*===========================================================================

FUNCTION  getTimestampedSampleInfo

DESCRIPTION
Public method used to get the sample corresponding to a given timestamp.

===========================================================================*/

bool Mpeg4File::getTimestampedSampleInfo(
  video_fmt_stream_info_type *p_track,
  uint32                      TimeStamp,
  video_fmt_sample_info_type *sampleInfo,
  uint32                     *newTimeStamp,
  bool                        bSetToSyncSample,
  uint32                      currentPosTimeStamp)
{
  // media timescale = number of time units per second
  // if media_timescale == 1000, the time units are miliseconds (1000/sec)
  uint32 timescaledTime = (uint32)(((float)TimeStamp/1000.0F)*p_track->media_timescale);

  uint32 streamNum = p_track->stream_num;
  uint32 maxFrames = p_track->frames;

  uint32 sampleDelta = 1;
  uint32 reqSampleNum = 0;
#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD ||defined FEATURE_QTV_PSEUDO_STREAM
  uint32 absFileOffset = 0;
  boolean bEndOfData = false;
#endif // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  int32 timeOffset = 0;
  bool  lRewind = false;
  bool retStat = false;
  if(maxFrames == 0)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "No valid frames for given track %d",p_track->track_id);
    return false;
  }

  if( (m_nextSample[streamNum]==0) && (m_sampleInfo[streamNum].size==0) && (p_track->dec_specific_info.obj_type!=(int)MPEG4_IMAGE) )
  {
    /* we have not read any sample yet. So atleast read first good sample */
    uint32 sampleId = 0;
    do
    {
      retStat = (getSampleInfo(streamNum, sampleId, 1, &m_sampleInfo[streamNum])>0)?true:false;
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Reading first valid sampleInfo. SampleId=%d",sampleId);
      sampleId++;
    }while(retStat && (m_mp4ReadLastStatus[streamNum] == VIDEO_FMT_IO_DONE)
      && (!m_sampleInfo[streamNum].delta || !m_sampleInfo[streamNum].size));

    if(!retStat || (m_mp4ReadLastStatus[streamNum] != VIDEO_FMT_IO_DONE))
      return false;
  }

  timeOffset = (int32)m_sampleInfo[streamNum].time - timescaledTime;

  if ( timeOffset == 0 )
  {
    *sampleInfo = m_sampleInfo[streamNum];
    if(sampleInfo->sync)
      return true;
  }

  /* if all video frames are SYNC frames, we don't have to search for sync frame
  repositioing will work same as audio where all frames are always sync frames */
  if(p_track->type == VIDEO_FMT_STREAM_AUDIO)
    bSetToSyncSample = false;

  if( (p_track->type == VIDEO_FMT_STREAM_VIDEO) &&
    bSetToSyncSample)
  {
    uint32 frameTime;
    bool userRewind;
    uint32 scaledCurPosTime = (uint32)(((float)currentPosTimeStamp/1000.0F)*p_track->media_timescale);

    /* Special case: if user wants to goto beginning of the case, just jump to first frame */
    if(timescaledTime == 0)
    {
      retStat = (getSampleInfo(streamNum, 0, 1, sampleInfo)>0)?true:false;
      return retStat;
    }

    userRewind = (scaledCurPosTime > timescaledTime)?true:false;

    /* first estimate on which frame we have to go assuming fixed delta between video
    frames. This will take us may be ahead or before the desired time, but from that
    point we can either FF or REW by checking each I-Frame to get the exact reposition
    point (means closest I-Frame). This will reduce time in finding closest I-Frame, if
    user is doing very long seek (say 50 minutes)
    */

    /* if we have to seek more than pre-defined limit (5*60 secs), calculate approximately how many
    sample we have to rewind or forward assuming sample time delta for a track is fixed */
    if( m_sampleInfo[streamNum].delta && ((uint32)abs((int32)(TimeStamp-currentPosTimeStamp))>m_defaultCoarseJumpLimit) )
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
        if((m_sampleInfo[streamNum].sample + sampleDelta) >= (p_track->frames-1))
        {
          reqSampleNum = p_track->frames-1;
        }
        else
        {
          reqSampleNum = m_sampleInfo[streamNum].sample + sampleDelta;
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

    lRewind = (sampleInfo->time > timescaledTime)?true:false;
    if(sampleInfo->time == timescaledTime)
      lRewind = userRewind;

    frameTime = sampleInfo->time;
    retStat = true;

    /* loop till we get the desired time stamp */
    if(lRewind)
    {
      for( ; frameTime>timescaledTime && retStat ; )
      {
        retStat = getSyncSampleInfo(streamNum, reqSampleNum, lRewind, sampleInfo);
        frameTime = sampleInfo->time;

        /* user has actually pressed FF, but we are rewinding, then we should
        not go beyond current displaty time. We also should make sure we are
           beyond the time user has asked, not before */
        if(!userRewind)
        {
          if(scaledCurPosTime >= frameTime ||(timescaledTime > frameTime))
          {
            /* means we have gone before current position, but user has pressed FF,
            so we should look I-Frame in FF direction */
            reqSampleNum = sampleInfo->sample+1;
            lRewind = false;
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
        retStat = (getSampleInfo(streamNum, reqSampleNum, 1, sampleInfo)>0)?true:false;
      }
    }

    /* FF case */
    if(!lRewind)
    {
      for( ; frameTime<timescaledTime && retStat; )
      {
        retStat = getSyncSampleInfo(streamNum, reqSampleNum, lRewind, sampleInfo);
        frameTime = sampleInfo->time;

        /* if we have found the frame, so we don't need to go further */
        if(frameTime>=timescaledTime && retStat)
        {
          break;
        }
        /*if there are no sync samples further then set the time stamp to the
        previous sync sapmple. if the last sample is a sync sample then
        set the time stamp to that sync sample. */
        if((retStat == false && sampleInfo->sync) || (retStat  && (sampleInfo->sample == (maxFrames-1))))
        {
          *newTimeStamp = (uint32)((float)sampleInfo->time * 1000.0F / p_track->media_timescale);
          /* check if we got any sync sample later than current sample */
          if(*newTimeStamp > currentPosTimeStamp)
          {
#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD|| defined FEATURE_QTV_PSEUDO_STREAM
            if( bHttpStreaming)
            {
              if(m_fpFetchBufferedDataSize)
              {
                //Pull interface so pull dnld data size from OEM
                uint32 userData = 0;
                #ifdef FEATURE_QTV_DRM_DCF
                  userData = (uint32)m_inputStream;
                #endif
                (*m_fpFetchBufferedDataSize)(userData, &m_wBufferOffset,&bEndOfData, m_QtvInstancehandle);
              }
              /*Fetch the abs file offset for the sample at new time stamp*/
              absFileOffset = getSampleAbsOffset (streamNum, sampleInfo->offset, sampleInfo->size);
              if( absFileOffset && absFileOffset < m_wBufferOffset)
              {
                return true;
              }
            }
            else
#endif // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
            {
              return true;
            }
          }
        }

        if(sampleInfo->sample >= (maxFrames-1))
        {
          retStat = false;
          break;
        }
        reqSampleNum = sampleInfo->sample+1;
      }
    /* in FF case, if we did not find any SYNC sample, then we try to find SYNC sample in reverse
          direction. If we get any SYNC sample and its time stamp is ahead of current position, we
          should FF upto that sample. This is similar logic as above which does not work if we did not
          find any SYNC sample after reqSampleNum frame */
      if((retStat == false) && !sampleInfo->sync )
      {
        /* find SYNC sample in reverse direction */
        retStat = getSyncSampleInfo(streamNum, reqSampleNum, TRUE, sampleInfo);
        if(retStat)
        {
          *newTimeStamp = (uint32)((float)sampleInfo->time * 1000.0F / p_track->media_timescale);
          /* check if we got any sync sample later than current sample */
          if(*newTimeStamp > currentPosTimeStamp)
          {
#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD|| defined FEATURE_QTV_PSEUDO_STREAM
            if( bHttpStreaming)
            {
              if(m_fpFetchBufferedDataSize)
              {
                //Pull interface so pull dnld data size from OEM
                uint32 userData = 0;
                #ifdef FEATURE_QTV_DRM_DCF
                   userData = (uint32)m_inputStream;
                #endif
                (*m_fpFetchBufferedDataSize)(userData,&m_wBufferOffset,&bEndOfData, m_QtvInstancehandle);
              }
              /*Fetch the abs file offset for the sample at new time stamp*/
              absFileOffset = getSampleAbsOffset (streamNum, sampleInfo->offset, sampleInfo->size);
              if( absFileOffset && absFileOffset < m_wBufferOffset)
              {
                return true;
              }
            }
            else
#endif // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
            {
              return true;
            }
          }
          else
            retStat = false;
        }
      }
    }
    /* see if we were successful */
    if(retStat)
    {
      *newTimeStamp = (uint32)((float)sampleInfo->time * 1000.0F / p_track->media_timescale);
      /* if we end up on the same frame, tell repositioning failed, this will prevent
      restarting the same frame again */
      if(*newTimeStamp == currentPosTimeStamp)
        retStat = false;

#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD||defined FEATURE_QTV_PSEUDO_STREAM
      if( bHttpStreaming && retStat )
      {
        if(m_fpFetchBufferedDataSize)
        {
          //Pull interface so pull dnld data size from OEM
          uint32 userData = 0;
          #ifdef FEATURE_QTV_DRM_DCF
            userData = (uint32)m_inputStream;
          #endif
          (*m_fpFetchBufferedDataSize)(userData,&m_wBufferOffset,&bEndOfData, m_QtvInstancehandle);
        }

        /*Fetch the abs file offset for the sample at new time stamp*/
        absFileOffset = getSampleAbsOffset (streamNum, sampleInfo->offset, sampleInfo->size);

        if((!absFileOffset) || absFileOffset >= m_wBufferOffset)
        {
           retStat = false;
        }
      }
#endif // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

    }
    return retStat;
  }

  /* calculate approximately how many sample we have to rewind or forward
  assuming sample time delta for a track is fixed */
  if (m_sampleInfo[streamNum].delta )
  {
    sampleDelta = (uint32)(abs(timeOffset)) / m_sampleInfo[streamNum].delta;
  }

  if ( timeOffset > 0 )
  {
    // rewind processing (current time > desired timestamp)
    //------------------
    lRewind = true;
    if ( m_sampleInfo[streamNum].sample >= sampleDelta )
    {
      reqSampleNum = m_sampleInfo[streamNum].sample - sampleDelta;
    }
    else
    {
      reqSampleNum = 0;
    }
    if(timescaledTime == 0)
    {
      /* if the request is to reposition to 0 ms
      * insure that the clip repositions to the first
      * sample
      */
      reqSampleNum = 0;
    }
  }
  else
  {    // timeOffset < 0
    // fast forward processing (current time < desired timestamp)
    //------------------------
    reqSampleNum = m_sampleInfo[streamNum].sample + sampleDelta;

    if ( reqSampleNum >= maxFrames )
    {
      reqSampleNum = maxFrames - 1;
    }
  }

  if ( bSetToSyncSample )
  {
    retStat = getSyncSampleInfo(streamNum, reqSampleNum, lRewind, sampleInfo);
    /* if in this rewind case, only first frame is sync frame */
    if((timeOffset > 0) && (m_mp4SyncLastStatus[streamNum]==VIDEO_FMT_IO_DONE) && (m_iodoneSize[streamNum]==0) )
    {
      reqSampleNum = 0;
      retStat = (getSampleInfo(streamNum, reqSampleNum, 1, sampleInfo)>0)?true:false;
    }
  }
  else
  {
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
          lRewind = false;
        }
        else
        {
          lRewind = true;
        }

        retStat = getSampleAtTimestamp (p_track, timescaledTime, lRewind, sampleInfo);
        QTV_MSG_PRIO3( QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "AUDIO FineRepos: sampleInfo.sample=%d, sampleInfo.time=%d, lRewind=%d",
                       sampleInfo->sample, sampleInfo->time, lRewind);
      }

    }
    else if( p_track->type == VIDEO_FMT_STREAM_TEXT
      || p_track->type == VIDEO_FMT_STREAM_VIDEO )
    {
      retStat = getSampleAtTimestamp (p_track, timescaledTime, lRewind, sampleInfo);
    }
  }

  *newTimeStamp = (uint32)(((float)sampleInfo->time*1000.0F)/p_track->media_timescale);

#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD||defined FEATURE_QTV_PSEUDO_STREAM
  if( bHttpStreaming && retStat )
  {
    if(m_fpFetchBufferedDataSize)
    {
      //Pull interface so pull dnld data size from OEM
      uint32 userData = 0;
      #ifdef FEATURE_QTV_DRM_DCF
        userData = (uint32)m_inputStream;
      #endif
      (*m_fpFetchBufferedDataSize)(userData,&m_wBufferOffset,&bEndOfData, m_QtvInstancehandle);
    }

    /*Fetch the abs file offset for the sample at new time stamp*/
    absFileOffset = getSampleAbsOffset (streamNum, sampleInfo->offset, sampleInfo->size);

    if((!absFileOffset) || absFileOffset >= m_wBufferOffset)
    {
      retStat = false;
    }
  }
#endif // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

  return retStat;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  resetPlayback

DESCRIPTION
Public method used to get the sample corresponding to a given timestamp.

===========================================================================*/
void Mpeg4File::resetPlayback()
{
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  if (_pTsmlAtom != NULL)
  {
    _pTsmlAtom->ResetTelopVectorIndex();
  }
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

  memset(&m_sampleInfo[0], 0x0, sizeof(video_fmt_sample_info_type)*VIDEO_FMT_MAX_MEDIA_STREAMS);
  memset(&m_nextSample[0], 0x0, sizeof(uint32)*VIDEO_FMT_MAX_MEDIA_STREAMS);
}

/* ======================================================================
FUNCTION
Mpeg4File::resetPlayback

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
//nabrol
int qtv_cfg_aud_fixed_size_max  = 0;
//
uint32 Mpeg4File::resetPlayback (
                                 uint32 res_time,
                                 uint32 id,
                                 bool bSetToSyncSample,
                                 bool *bError,
                                 uint32 currentPosTimeStamp
                                 )
{
  *bError = FALSE;
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);
  if ( !p_track )
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "resetPlayback, unknown track id = %d", id);
    *bError = TRUE;
    return 0;
  }

  //nabrol

  if ( p_track->type == VIDEO_FMT_STREAM_AUDIO) {
    if( p_track->fixed_size == TRUE)
      qtv_cfg_aud_fixed_size_max = p_track->largest;

    else
      qtv_cfg_aud_fixed_size_max  = 0;
  }
  ///

  uint32 streamNum = p_track->stream_num;

  video_fmt_sample_info_type  sampleInfo;
  memset(&sampleInfo, 0x0, sizeof(video_fmt_sample_info_type));

  uint32 modTS = res_time; // initial value

  if ( getTimestampedSampleInfo (p_track, res_time, &sampleInfo,
    &modTS, bSetToSyncSample, currentPosTimeStamp) )
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
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Reposition failed for track id = %d", id);
    // this is to avoid unnecessary disturbance, when repositioning can not be done
    *bError = TRUE;
    float fTime;
    fTime = (float)m_sampleInfo[streamNum].time / (float)p_track->media_timescale * (float)TIMESCALE_BASE;
    return (uint32)fTime;
  }


}

/* ======================================================================
FUNCTION
Mpeg4File::skipNSyncSamples

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
#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
uint32 Mpeg4File::skipNSyncSamples(int offset, uint32 id, bool *bError, uint32 currentPosTimeStamp)
{

  int32 reqSampleNum = 0;
  int8  noOfSyncSamplesSkipped = 0;
  bool result = false;
  uint32 newTimeStamp =0;

  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  int streamNum = 0;
  if(p_track)
  {
      streamNum = p_track->stream_num;
  }
  else
  {
    *bError =  true;
    QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "seekToSync function Failed In Skipping %d Sync Samples", offset );
    return currentPosTimeStamp;
  }
  int maxFrames = p_track->frames;

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
        if( noOfSyncSamplesSkipped == abs(offset) )
        {
          // In successfull case return the latest sync sample time
          result = true;
          break;
        }
      }
      else
      {
        // Not successfull in skipping desired sync samplese so return the old time.
         break;
      }
    }
  }
  else
  {
    // forward case
    for(reqSampleNum = m_sampleInfo[streamNum].sample; reqSampleNum<maxFrames; ++reqSampleNum)
    {
       if(getSyncSampleInfo( streamNum, reqSampleNum, false, &sampleInfo ))
       {
         noOfSyncSamplesSkipped++;
         if( noOfSyncSamplesSkipped==offset )
         {
           // In successfull case return the last sync sample time
           result = true;
           break;
         }
       }
       else
       {
         // Not successfull in skipping desired sync samplese so return the old time.
         break;
       }
    }
  }

  // Check for The result
  if(result)
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

/* <EJECT> */
/*===========================================================================

FUNCTION  peekCurSample

DESCRIPTION

===========================================================================*/
uint32 Mpeg4File::peekCurSample(uint32 trackid, file_sample_info_type *pSampleInfo)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(trackid);
  if ( !p_track )
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "getNextMediaSample, unknown track id = %d", trackid);
    return (uint32)DEFAULT_ERROR;
  }

  uint32 streamNum = p_track->stream_num;

  ASSERT(pSampleInfo);

  //Simply return the current sample info without going to VideoFMT

  pSampleInfo->sample               = m_sampleInfo[streamNum].sample;
  pSampleInfo->size                 = m_sampleInfo[streamNum].size;
  pSampleInfo->offset               = m_sampleInfo[streamNum].offset;
  pSampleInfo->time                 = m_sampleInfo[streamNum].time;
  pSampleInfo->delta                = m_sampleInfo[streamNum].delta;
  pSampleInfo->sync                 = m_sampleInfo[streamNum].sync;
  pSampleInfo->sample_desc_index    = m_sampleInfo[streamNum].sample_desc_index;
  /* mpeg4file returns one sample at a time */
  pSampleInfo->num_frames = 1;

  return (uint32)EVERYTHING_FINE;
}

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
/* <EJECT> */
/*===========================================================================

FUNCTION  IsTelopPresent

DESCRIPTION
returns TRUE if telop is present, otherwise FALSE.

===========================================================================*/
bool Mpeg4File::IsTelopPresent()
{
  if (_pTsmlAtom != NULL)
  {
    return true;
  }
  return false;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTelopTrackDuration

DESCRIPTION
returns the duration of telop track.

===========================================================================*/
uint32 Mpeg4File::getTelopTrackDuration()
{
  if (_pTsmlAtom != NULL)
  {
    return(_pTsmlAtom->GetTelopTrackDuration());
  }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getNextTelopElement

DESCRIPTION
returns next telop element

===========================================================================*/
TelopElement *Mpeg4File::getNextTelopElement()
{
  if (_pTsmlAtom != NULL)
  {
    return(_pTsmlAtom->GetNextTelopElement());
  }
  return NULL;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getTelopHeader

DESCRIPTION
returns telop header

===========================================================================*/
TelopHeader *Mpeg4File::getTelopHeader()
{
  if (_pTsmlAtom != NULL)
  {
    return(_pTsmlAtom->GetTelopHeader());
  }
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  resetTelopPlayback

DESCRIPTION
resents telop timed text play back.

===========================================================================*/
uint32 Mpeg4File::resetTelopPlayback(uint32 startPos)
{
  if (_pTsmlAtom != NULL)
  {
    (void)_pTsmlAtom->ResetTelopVectorIndexByTime((int32)startPos);
  }
  return startPos;
}
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */


/* <EJECT> */
/*===========================================================================

FUNCTION  getTitle

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getTitle() const
{
     if(m_bUdtaAtomPresent && _titlAtom)
     {
          OSCL_STRING Title;
          uint32 dwDataToBeCopied;
          char *str =(char *)QTV_Malloc(_titlAtom->getUdtaTitlDataSize()+1);
        if (!str)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                                        "Mpeg4File::getTitle() failed to allocate memory");
          return 0;
        }
          dwDataToBeCopied=_titlAtom->getUdtaTitlData((uint8*)str,_titlAtom->getUdtaTitlDataSize()+1,2);
          /*Two bytes offset to skip Pad and Language fields*/
          str[dwDataToBeCopied]='\0';
          Title=str;
          QTV_Free(str);
          return Title;
     }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getAuthor

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getAuthor()const
{
  if(m_bUdtaAtomPresent && _authAtom)
     {
          OSCL_STRING Auth;
          uint32 dwDataToBeCopied;
          char *str=(char *)QTV_Malloc(_authAtom->getUdtaAuthDataSize()+1);
        if (!str)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                                        "Mpeg4File::getAuthor() failed to allocate memory");
          return 0;
        }
          dwDataToBeCopied=_authAtom->getUdtaAuthData((uint8*)str,_authAtom->getUdtaAuthDataSize()+1,2);
          /*Two bytes offset to skip Pad and Language fields*/
          str[dwDataToBeCopied]='\0';
          Auth=(char*)str;
          QTV_Free(str);
          return Auth;
     }
  return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getVersion

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getVersion()const
{
  return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getCopyright

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getCopyright()const
{
   if(m_bUdtaAtomPresent && _cprtAtom)
     {
          OSCL_STRING cprt;
          uint32 dwDataToBeCopied;
          char *str=(char *)QTV_Malloc(_cprtAtom->getUdtaCprtDataSize()+1);
        if (!str)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                                        "Mpeg4File::getCopyright() failed to allocate memory");
          return 0;
        }
          dwDataToBeCopied=_cprtAtom->getUdtaCprtData((uint8*)str,_cprtAtom->getUdtaCprtDataSize()+1,2);
          /*Two bytes offset to skip Pad and Language fields*/
          str[dwDataToBeCopied]='\0';
          cprt=(char*)str;
          QTV_Free(str);
          return cprt;
     }
  return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getDescription

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getDescription()const
{

  if(m_bUdtaAtomPresent && _dscpAtom)
     {
          OSCL_STRING cprt;
          uint32 dwDataToBeCopied;
          char *str=(char *)QTV_Malloc(_dscpAtom->getUdtaDscpDataSize()+1);
        if (!str)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                                        "Mpeg4File::getDescription() failed to allocate memory");
          return 0;
        }
          dwDataToBeCopied=_dscpAtom->getUdtaDscpData((uint8*)str,_dscpAtom->getUdtaDscpDataSize()+1,2);
          /*Two bytes offset to skip Pad and Language fields*/
          str[dwDataToBeCopied]='\0';
          cprt=(char*)str;
          QTV_Free(str);
          return cprt;
     }
  return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getRating

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getRating() const
{
  if(m_bUdtaAtomPresent && _rtngAtom)
     {
          OSCL_STRING rtng;
          uint32 dwDataToBeCopied;
          char *str=(char *)QTV_Malloc(_rtngAtom->getUdtaRtngDataSize()+1);
        if (!str)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                                        "Mpeg4File::getRating() failed to allocate memory");
          return 0;
        }
          dwDataToBeCopied=_rtngAtom->getUdtaRtngData((uint8*)str,_rtngAtom->getUdtaRtngDataSize()+1 ,2);
          /*Two bytes offset to skip Pad and Language fields*/
          str[dwDataToBeCopied]='\0';
          rtng=(char*)str;
          QTV_Free(str);
          return rtng;
     }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getperf

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getPerf() const
{
  if(m_bUdtaAtomPresent && _perfAtom)
     {
          OSCL_STRING perf;
          uint32 dwDataToBeCopied;
          char *str=(char *)QTV_Malloc(_perfAtom->getUdtaPerfDataSize()+1);
        if (!str)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                                        "Mpeg4File::getPerf() failed to allocate memory");
          return 0;
        }
          dwDataToBeCopied=_perfAtom->getUdtaPerfData((uint8*)str,_perfAtom->getUdtaPerfDataSize()+1 ,2);
          /*Two bytes offset to skip Pad and Language fields*/
          str[dwDataToBeCopied]='\0';
          perf=(char*)str;
          QTV_Free(str);
          return perf;
     }
  return 0;
}

/*===========================================================================

FUNCTION  getGenre

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getGenre() const
{
  if(m_bUdtaAtomPresent && _gnreAtom)
     {
          OSCL_STRING gnre;
          uint32 dwDataToBeCopied;
          char *str=(char *)QTV_Malloc(_gnreAtom->getUdtaGnreDataSize()+1);
        if (!str)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                                        "Mpeg4File::getGenre() failed to allocate memory");
          return 0;
        }
          dwDataToBeCopied=_gnreAtom->getUdtaGnreData((uint8*)str,_gnreAtom->getUdtaGnreDataSize()+1 ,2);
          /*Two bytes offset to skip Pad and Language fields*/
          str[dwDataToBeCopied]='\0';
          gnre=(char*)str;
          QTV_Free(str);
          return gnre;
     }
  return 0;
}

/*===========================================================================

FUNCTION  getClsf

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getClsf() const
{
  if(m_bUdtaAtomPresent && _clsfAtom)
     {
          OSCL_STRING clsf;
          uint32 dwDataToBeCopied;
          char *str=(char *)QTV_Malloc(_clsfAtom->getUdtaClsfDataSize()+1);
        if (!str)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                                        "Mpeg4File::getClsf() failed to allocate memory");
          return 0;
        }
          dwDataToBeCopied=_clsfAtom->getUdtaClsfData((uint8*)str,_clsfAtom->getUdtaClsfDataSize()+1  ,2);
          /*Two bytes offset to skip Pad and Language fields*/
          str[dwDataToBeCopied]='\0';
          clsf=(char*)str;
          QTV_Free(str);
          return clsf;
     }
  return 0;
}
/*===========================================================================

FUNCTION  getKywd

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getKywd() const
{
  if(m_bUdtaAtomPresent && _kywdAtom)
     {
          OSCL_STRING kywd;
          uint32 dwDataToBeCopied;
          char *str=(char *)QTV_Malloc(_kywdAtom->getUdtaKywdDataSize()+1);
        if (!str)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                                        "Mpeg4File::getKywd() failed to allocate memory");
          return 0;
        }
          dwDataToBeCopied=_kywdAtom->getUdtaKywdData((uint8*)str,_kywdAtom->getUdtaKywdDataSize()+1  ,3);
          /*Three bytes offset to skip Pad,Language,Keywdcnt fields*/
          str[dwDataToBeCopied]='\0';
          kywd=(char*)str;
          QTV_Free(str);
          return kywd;
     }
  return 0;
}

/*===========================================================================

FUNCTION  getLoci

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getLoci() const
{
  if(m_bUdtaAtomPresent && _lociAtom)
     {
          OSCL_STRING loci;
          uint32 dwDataToBeCopied;
          char *str=(char *)QTV_Malloc(_lociAtom->getUdtaLociDataSize()+1);
        if (!str)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                                        "Mpeg4File::getLoci() failed to allocate memory");
          return 0;
        }
          dwDataToBeCopied=_lociAtom->getUdtaLociData((uint8*)str,_lociAtom->getUdtaLociDataSize()+1,2);
          /*Two bytes offset to skip Pad and Language fields*/
          str[dwDataToBeCopied]='\0';
          loci=(char*)str;
          QTV_Free(str);
          return loci;
     }
  return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getCreationDate

DESCRIPTION
Public method used to request a media sample (frame)

===========================================================================*/
OSCL_STRING Mpeg4File::getCreationDate()const
{
     if(m_videoFmtInfo.file_info.creation_time)
     {
       OSCL_STRING CreationDate;
       char str[128];
       /* This is 3GPP time (number of seconds elapsed since mid-night, 1 Jan,1904)
          Since we can pass string to OEM, converting it */
       std_strlprintf(str, sizeof(str), "%d", m_videoFmtInfo.file_info.creation_time);
       CreationDate=(char*)str;
       return CreationDate;
     }
  return 0;
}

/*===========================================================================

FUNCTION  getAlbum

DESCRIPTION
Public method used to request album data

===========================================================================*/
OSCL_STRING Mpeg4File::getAlbum() const
{
  if(m_bUdtaAtomPresent && _albumAtom)
  {
    OSCL_STRING Album;
    uint32 dwDataToBeCopied;
    char *str=(char *)QTV_Malloc(_albumAtom->getUdtaAlbumDataSize()+1);
    if (!str)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                               "Mpeg4File::getAlbum() failed to allocate memory");
      return 0;
    }
    dwDataToBeCopied=_albumAtom->getUdtaAlbumData((uint8*)str,_albumAtom->getUdtaAlbumDataSize()+1,2);
    /*Two bytes offset to skip Pad and Language fields*/
    str[dwDataToBeCopied]='\0';
    Album=(char*)str;
    QTV_Free(str);
    return Album;
  }
  return 0;
}

/*===========================================================================

FUNCTION  getYrrc

DESCRIPTION
Public method used to get recording year

===========================================================================*/
OSCL_STRING Mpeg4File::getYrrc() const
{
  if(m_bUdtaAtomPresent && _yrrcAtom)
  {
    OSCL_STRING Yrrc;
    uint32 dwDataToBeCopied;
    char *str=(char *)QTV_Malloc(_yrrcAtom->getUdtaYrrcDataSize()+1);
    if (!str)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                             "Mpeg4File::getYrrc() failed to allocate memory");
      return 0;
    }
    dwDataToBeCopied=_yrrcAtom->getUdtaYrrcData((uint8*)str,_yrrcAtom->getUdtaYrrcDataSize()+1,2);
    /*Two bytes offset to skip Pad and Language fields*/
    str[dwDataToBeCopied]='\0';
    Yrrc=(char*)str;
    QTV_Free(str);
    return Yrrc;
  }
  return 0;
}

#if defined(FEATURE_MP4_KDDI_META_DATA) || defined(FEATURE_QTV_SKT_MOD_MIDI)

#define DEFAULT_UUID_ATOM_HEADER_SIZE 24
#define DEFAULT_UDTA_CHILD_ATOM_HEADER_SIZE 8
/* <EJECT> */
/*===========================================================================

FUNCTION  mp4ParseUUIDAtom

DESCRIPTION
This private function create UUID atom by parsing media file.

===========================================================================*/
void Mpeg4File::mp4ParseUUIDAtom(video_fmt_uuid_data_type *pAtomInfo,OSCL_FILE* localFilePtr)
{
  Atom * entry = NULL ;
  int32 saveFilePos = OSCL_FileTell (localFilePtr);

  /* videoFMT offset is after atom header, but current parsing needs
     atom header as well, so goback 24 bytes */
  if (pAtomInfo->offset > DEFAULT_UUID_ATOM_HEADER_SIZE)
    (void)OSCL_FileSeek(localFilePtr, pAtomInfo->offset-DEFAULT_UUID_ATOM_HEADER_SIZE, SEEK_SET);

  switch (pAtomInfo->atom_type)
  {
#if defined(FEATURE_MP4_KDDI_META_DATA)
    case KDDI_DRM_ATOM:
      if(_kddiDRMAtom)
        QTV_Delete( _kddiDRMAtom );
      _kddiDRMAtom = QTV_New_Args( KDDIDrmAtom, (localFilePtr) );
      break;

    case KDDI_CONTENT_PROPERTY_ATOM:
      if(_kddiContentPropertyAtom)
        QTV_Delete( _kddiContentPropertyAtom );
      _kddiContentPropertyAtom = QTV_New_Args( KDDIContentPropertyAtom, (localFilePtr) );
      break;

    case KDDI_MOVIE_MAIL_ATOM:
      if(_kddiMovieMailAtom)
        QTV_Delete( _kddiMovieMailAtom );
      _kddiMovieMailAtom = QTV_New_Args( KDDIMovieMailAtom, (localFilePtr) );
      break;

    case KDDI_ENCODER_INFO_ATOM:
      if(_kddiEncoderInformationAtom)
        QTV_Delete( _kddiEncoderInformationAtom );
      _kddiEncoderInformationAtom = QTV_New_Args( KDDIEncoderInformationAtom, (localFilePtr) );
      break;

    case KDDI_GPS_ATOM:
      if(_kddiGPSAtom)
        QTV_Delete( _kddiGPSAtom );
      _kddiGPSAtom = QTV_New_Args( KDDIGPSAtom, (localFilePtr) );
      break;
#endif /* FEATURE_MP4_KDDI_META_DATA */

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
    case KDDI_TELOP_ATOM:
      //Telop parser lib needs to be called here.
      if(_kddiTelopElement)
        QTV_Delete( _kddiTelopElement );
      _kddiTelopElement = QTV_New_Args( KDDITelopAtom, (localFilePtr) );
      break;
#endif

#if defined(FEATURE_QTV_SKT_MOD_MIDI)
    case MIDI_TYPE:
      /* for child videoFMT offset is after atom size and type, but current parsing needs
        atom header as well, so goback 8 bytes */
      if (pAtomInfo->offset > DEFAULT_UDTA_CHILD_ATOM_HEADER_SIZE)
        (void)OSCL_FileSeek(localFilePtr, pAtomInfo->offset-DEFAULT_UDTA_CHILD_ATOM_HEADER_SIZE, SEEK_SET);
      if(_midiAtom)
        QTV_Delete( _midiAtom );
      _midiAtom = QTV_New_Args( UdtaMidiAtom, (localFilePtr) );
      break;

    case LINK_TYPE:
      /* for child videoFMT offset is after atom size and type, but current parsing needs
        atom header as well, so goback 8 bytes */
      if (pAtomInfo->offset > DEFAULT_UDTA_CHILD_ATOM_HEADER_SIZE)
        (void)OSCL_FileSeek(localFilePtr, pAtomInfo->offset-DEFAULT_UDTA_CHILD_ATOM_HEADER_SIZE, SEEK_SET);
      if(_linkAtom)
        QTV_Delete( _linkAtom );
      _linkAtom = QTV_New_Args( UdtaLinkAtom, (localFilePtr) );
      break;
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

    default:
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Unsupp atom %x", pAtomInfo->atom_type );
      break;
  }
  /* videoFMT offset is after atom header, but current parsing needs
     atom header as well, so goback 24 bytes */
  if (pAtomInfo->offset > DEFAULT_UUID_ATOM_HEADER_SIZE)
    OSCL_FileSeek(localFilePtr, pAtomInfo->offset - DEFAULT_UUID_ATOM_HEADER_SIZE, SEEK_SET);

  entry = QTV_New_Args( Atom, (localFilePtr) );
  if(entry != NULL)
  {
    if (!entry->FileSuccess())
    {
      QTV_Delete( entry );
      /* Here even though the UDTA child is corrupted let the playback continue */
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "UUID atom parsing failed");
    }
    else
    {
      entry->setParent(this);
      UUIDatomEntryArray += entry;
      UUIDatomEntryCount++;
    }
   }
  (void)OSCL_FileSeek(localFilePtr, saveFilePos, SEEK_SET);
}

/*===========================================================================

FUNCTION  getNumUUIDAtom

DESCRIPTION
returns the UUID atom count

===========================================================================*/
uint32 Mpeg4File::getNumUUIDAtom()
{
  return UUIDatomEntryCount;
}

/*---------------------------------- KDDI Meta Data Related APIs --------------------------------*/

/* <EJECT> */
/*===========================================================================

FUNCTION  getCopyProhibitionFlag

DESCRIPTION
returns Copy Prohibition Flag value from CopyGaurdAtom

===========================================================================*/
uint32 Mpeg4File::getCopyProhibitionFlag()
{
  if (_kddiDRMAtom != NULL)
  {
    return(_kddiDRMAtom->getCopyProhibitionFlag());
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getValidityEffectiveDate

DESCRIPTION
returns Validity Start Date from CopyGaurdAtom

===========================================================================*/
uint32 Mpeg4File::getValidityEffectiveDate()
{
  if (_kddiDRMAtom != NULL)
  {
    return(_kddiDRMAtom->getValidityEffectiveDate());
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getValidityPeriod

DESCRIPTION
returns Validity Period from Validity Start Date from CopyGaurdAtom

===========================================================================*/
uint32 Mpeg4File::getValidityPeriod()
{
  if (_kddiDRMAtom != NULL)
  {
    return(_kddiDRMAtom->getValidityPeriod());
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getNumberofAllowedPlayBacks

DESCRIPTION
returns number of allowed playback from CopyGaurdAtom

===========================================================================*/
uint32 Mpeg4File::getNumberofAllowedPlayBacks()
{
  if (_kddiDRMAtom != NULL)
  {
    return(_kddiDRMAtom->getNumberofAllowedPlayBacks());
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getContentPropertyTitle

DESCRIPTION
returns title of content from Content Property Atom

===========================================================================*/
OSCL_STRING Mpeg4File::getContentPropertyTitle()
{
  OSCL_STRING temp(_T(""));
  if (_kddiContentPropertyAtom != NULL)
  {
    return(_kddiContentPropertyAtom->getContentPropertyTitle());
  }
  return(temp);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getContentPropertyCopyRight

DESCRIPTION
returns Copy Right string from Content Property Atom

===========================================================================*/
OSCL_STRING Mpeg4File::getContentPropertyCopyRight()
{
  OSCL_STRING temp(_T(""));
  if (_kddiContentPropertyAtom != NULL)
  {
    return(_kddiContentPropertyAtom->getContentPropertyCopyRight());
  }
  return(temp);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getContentPropertyAuthor

DESCRIPTION
returns content author from Content Property Atom

===========================================================================*/
OSCL_STRING Mpeg4File::getContentPropertyAuthor()
{
  OSCL_STRING temp(_T(""));
  if (_kddiContentPropertyAtom != NULL)
  {
    return(_kddiContentPropertyAtom->getContentPropertyAuthor());
  }
  return(temp);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getContentPropertyMemo

DESCRIPTION
returns content memo from Content Property Atom

===========================================================================*/
OSCL_STRING Mpeg4File::getContentPropertyMemo()
{
  OSCL_STRING temp(_T(""));
  if (_kddiContentPropertyAtom != NULL)
  {
    return(_kddiContentPropertyAtom->getContentPropertyMemo());
  }
  return(temp);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getAuthorDLLVersion

DESCRIPTION
returns DLL version of content authoring tool from Content Property Atom

===========================================================================*/
uint32 Mpeg4File::getAuthorDLLVersion()
{
  if (_kddiContentPropertyAtom != NULL)
  {
    return(_kddiContentPropertyAtom->getAuthorDLLVersion());
  }
  return(0);
}


/* <EJECT> */
/*===========================================================================

FUNCTION  getEditFlags

DESCRIPTION
returns edit flags from Movie Mail Atom

===========================================================================*/
uint32 Mpeg4File::getEditFlags()
{
  if (_kddiMovieMailAtom != NULL)
  {
    return(_kddiMovieMailAtom->getEditFlags());
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getRecordingMode

DESCRIPTION
returns recording mode from Movie Mail Atom

===========================================================================*/
uint8 Mpeg4File::getRecordingMode()
{
  if (_kddiMovieMailAtom != NULL)
  {
    return(_kddiMovieMailAtom->getRecordingMode());
  }
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getRecordingDate

DESCRIPTION
returns recording date from Movie Mail Atom

===========================================================================*/
uint32 Mpeg4File::getRecordingDate()
{
  if (_kddiMovieMailAtom != NULL)
  {
    return(_kddiMovieMailAtom->getRecordingDate());
  }
  return(0);
}


/* <EJECT> */
/*===========================================================================

FUNCTION  getDeviceName

DESCRIPTION
returns encoding device from Encoder Information Atom

===========================================================================*/
uint8*  Mpeg4File::getDeviceName() const
{
  if (_kddiEncoderInformationAtom != NULL)
  {
    return(_kddiEncoderInformationAtom->getDeviceName());
  }
  return(NULL);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getModelName

DESCRIPTION
returns encoding device model name from Encoder Information Atom

===========================================================================*/
uint8*  Mpeg4File::getModelName() const
{
  if (_kddiEncoderInformationAtom != NULL)
  {
    return(_kddiEncoderInformationAtom->getModelName());
  }
  return(NULL);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getEncoderInformation

DESCRIPTION
returns encoder information from Encoder Information Atom

===========================================================================*/
uint8*  Mpeg4File::getEncoderInformation() const
{
  if (_kddiEncoderInformationAtom != NULL)
  {
    return(_kddiEncoderInformationAtom->getEncoderInformation());
  }
  return(NULL);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getMuxInformation

DESCRIPTION
returns encoding multiplexer information from Encoder Information Atom

===========================================================================*/
uint8*  Mpeg4File::getMuxInformation() const
{
  if (_kddiEncoderInformationAtom != NULL)
  {
    return(_kddiEncoderInformationAtom->getMuxInformation());
  }
  return(NULL);
}

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
/* <EJECT> */
/*===========================================================================

FUNCTION  getTelopInformationString

DESCRIPTION
returns telop information from Telop Atom

===========================================================================*/
uint8*  Mpeg4File::getTelopInformationString() const
{
  if (_kddiTelopElement != NULL)
  {
    return(_kddiTelopElement->getTelopInformationString());
  }
  return(NULL);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTelopInformationSize

DESCRIPTION
returns telop information size from Telop Atom

===========================================================================*/
uint32  Mpeg4File::getTelopInformationSize()
{
  if (_kddiTelopElement != NULL)
  {
    return(_kddiTelopElement->getTelopInformationSize());
  }
  return(0);
}
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSByteOrder

DESCRIPTION
returns GPS Byte Order from GPS Atom

===========================================================================*/
uint16 Mpeg4File::getGPSByteOrder()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getGPSByteOrder());
  }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSVersionID

DESCRIPTION
returns GPS Version ID from GPS Atom

===========================================================================*/
uint32  Mpeg4File::getGPSVersionID()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getVersionID());
  }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSLatitudeRef

DESCRIPTION
returns GPS Latitude Reference from GPS Atom

===========================================================================*/
uint32  Mpeg4File::getGPSLatitudeRef()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getLatitudeRef());
  }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSLongitudeRef

DESCRIPTION
returns GPS Longitude Reference from GPS Atom

===========================================================================*/
uint32  Mpeg4File::getGPSLongitudeRef()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getLongitudeRef());
  }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSAltitudeRef

DESCRIPTION
returns GPS Altitude Reference from GPS Atom

===========================================================================*/
uint32  Mpeg4File::getGPSAltitudeRef()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getAltitudeRef());
  }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSLatitudeArray

DESCRIPTION
returns GPS Latitude Array from GPS Atom

===========================================================================*/
uint64 *Mpeg4File::getGPSLatitudeArray()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getGPSLatitudeArray());
  }
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSLongitudeArray

DESCRIPTION
returns GPS Longitude Array from GPS Atom

===========================================================================*/
uint64 *Mpeg4File::getGPSLongitudeArray()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getGPSLongitudeArray());
  }
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSAltitude

DESCRIPTION
returns GPS Altitude from GPS Atom

===========================================================================*/
uint64 Mpeg4File::getGPSAltitude()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getGPSAltitude());
  }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSTimeArray

DESCRIPTION
returns GPS Time Array from GPS Atom

===========================================================================*/
uint64 *Mpeg4File::getGPSTimeArray()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getGPSTimeArray());
  }
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSSurveyData

DESCRIPTION
returns GPS Survey Data from GPS Atom

===========================================================================*/
OSCL_STRING Mpeg4File::getGPSSurveyData()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getGPSSurveyData());
  }
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getPositoningMethod

DESCRIPTION
returns GPS Positoning Method from GPS Atom

===========================================================================*/
OSCL_STRING Mpeg4File::getPositoningMethod()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getPositoningMethod());
  }
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getPositioningName

DESCRIPTION
returns GPS Positioning Name from GPS Atom

===========================================================================*/
OSCL_STRING Mpeg4File::getPositioningName()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getPositioningName());
  }
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSDate

DESCRIPTION
returns GPS Date string from GPS Atom

===========================================================================*/
OSCL_STRING Mpeg4File::getGPSDate()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getGPSDate());
  }
  return NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getGPSExtensionMapScaleInfo

DESCRIPTION
returns GPS Extension Map Scale Information from GPS Atom

===========================================================================*/
uint64 Mpeg4File::getGPSExtensionMapScaleInfo()
{
  if (_kddiGPSAtom != NULL)
  {
    return(_kddiGPSAtom->getGPSExtensionMapScaleInfo());
  }
  return 0;
}

#endif /* FEATURE_MP4_KDDI_META_DATA */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT

/* <EJECT> */
/*===========================================================================

FUNCTION  getTextSampleEntryAt

DESCRIPTION

===========================================================================*/
TextSampleEntry *Mpeg4File::getTextSampleEntryAt (uint32 trackid, uint32 index)
{
  uint32 length = 0;
  video_fmt_stream_info_type *p_track = getTrackInfoForID(trackid);
  if ( !p_track )
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "getNextMediaSample, unknown track id = %d", trackid);
    return NULL;
  }

  if( p_track->type == VIDEO_FMT_STREAM_TEXT )
  {
    length = (uint32)textSampleEntryArray.GetLength();
    if( length >= index )
    {
      return textSampleEntryArray[index-1];
    }
  }

  return NULL;
}

#define DEFAULT_TX3G_ATOM_HEADER_SIZE 8

/* ======================================================================
FUNCTION
Mpeg4File::Parse3GPPTimedTextAtom

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
void Mpeg4File::Parse3GPPTimedTextAtom(video_fmt_tx3g_data_type *tx3g_atom,OSCL_FILE* localFilePtr)
{
     int32 saveFilePos = OSCL_FileTell (localFilePtr);

     /* videoFMT offset is after atom header, but current parsing needs
     atom header as well, so goback 8 bytes */
     if (tx3g_atom->offset > DEFAULT_TX3G_ATOM_HEADER_SIZE)
          (void)OSCL_FileSeek(localFilePtr, (int32)(tx3g_atom->offset-DEFAULT_TX3G_ATOM_HEADER_SIZE), SEEK_SET);

     TextSampleEntry * entry = QTV_New_Args( TextSampleEntry, (localFilePtr) );

    if(!entry)
    {
      _success = false;
          _fileErrorCode = (int)MEMORY_ALLOCATION_FAILED;
    }
    else if (!(entry->FileSuccess()))
    {
      _fileErrorCode = entry->GetFileError();
      _success = false;
     QTV_Delete( entry );
    }
    else
    {
      entry->setParent(this);
      textSampleEntryArray += entry;
      textSampleEntryCount++;
    }

  (void)OSCL_FileSeek(localFilePtr, saveFilePos, SEEK_SET);
}

/* ======================================================================
FUNCTION
  Mpeg4File::GetNumTX3GAtom

DESCRIPTION

========================================================================== */
uint32 Mpeg4File::GetNumTX3GAtom()
{
  return textSampleEntryCount;
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

/* <EJECT> */
/*===========================================================================

FUNCTION  ~Mpeg4File

DESCRIPTION
Destructor for the Mpeg4File class

===========================================================================*/
Mpeg4File::~Mpeg4File()
{
  uint32 i;

  if (m_mp4ParseEndCb && m_mp4ParseServerData)
  {
    m_mp4ParseEndCb (m_mp4ParseServerData);
    while ( (m_mp4ParseLastStatus != VIDEO_FMT_DONE) &&
      (m_mp4ParseLastStatus != VIDEO_FMT_FAILURE) )
    {
      if((m_mp4ParseContinueCb == NULL) ||
        (m_mp4ParseServerData == NULL))
        break;
      else
        m_mp4ParseContinueCb (m_mp4ParseServerData);
    }
    m_mp4ParseContinueCb = NULL;
    m_mp4ParseServerData = NULL;
    if (m_mp4ParseLastStatus == VIDEO_FMT_FAILURE)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4File Destructor: VIDEO_FMT_FAILURE");
    }
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  for (i = 0; i < textSampleEntryCount; i++)
  {
    if((textSampleEntryArray)[i] &&
      (textSampleEntryArray)[i] != NULL)
    {
      QTV_Delete( (textSampleEntryArray)[i] );
      (textSampleEntryArray)[i] = NULL;
    }
  }
  textSampleEntryCount = 0;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
for (i = 0; i < UUIDatomEntryCount; i++)
  {
    if((UUIDatomEntryArray)[i] &&
       (UUIDatomEntryArray)[i] != NULL)
    {
      QTV_Delete( (UUIDatomEntryArray)[i] );
      (UUIDatomEntryArray)[i] = NULL;
    }
  }
  UUIDatomEntryCount = 0;
  /* close all stream file pointers */
  for(i=0; i<VIDEO_FMT_MAX_MEDIA_STREAMS; i++)
  {
    if(m_streamFilePtr[i]!= NULL)
    {
      (void)OSCL_FileClose( m_streamFilePtr[i] );
      m_streamFilePtr[i] = NULL;
    }
  }

#ifdef FEATURE_MP4_KDDI_META_DATA
  if (_kddiDRMAtom != NULL)
  {
    QTV_Delete( _kddiDRMAtom );
  }
  if (_kddiContentPropertyAtom != NULL)
  {
    QTV_Delete( _kddiContentPropertyAtom );
  }
  if (_kddiMovieMailAtom != NULL)
  {
    QTV_Delete( _kddiMovieMailAtom );
  }
  if (_kddiEncoderInformationAtom != NULL)
  {
    QTV_Delete( _kddiEncoderInformationAtom );
  }
  if (_kddiGPSAtom != NULL)
  {
    QTV_Delete( _kddiGPSAtom );
  }
#endif /* FEATURE_MP4_KDDI_META_DATA */

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  if(_pTsmlAtom != NULL)
  {
    QTV_Delete( _pTsmlAtom );
  }
  if (_kddiTelopElement != NULL)
  {
    QTV_Delete( _kddiTelopElement );
  }
#endif

#if defined(FEATURE_QTV_SKT_MOD_MIDI)
  if(_midiAtom)
  {
    QTV_Delete( _midiAtom );
  }
  if(_linkAtom)
  {
    QTV_Delete( _linkAtom );
  }
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

  if(_ftypAtom)
  {
    QTV_Delete( _ftypAtom );
  }
  if(_cprtAtom)
  {
    QTV_Delete( _cprtAtom );
  }
  if(_authAtom)
  {
    QTV_Delete( _authAtom );
  }
  if(_albumAtom)
  {
    QTV_Delete( _albumAtom );
  }
  if(_yrrcAtom)
  {
    QTV_Delete( _yrrcAtom );
  }
  if(_titlAtom)
  {
    QTV_Delete( _titlAtom );
  }
  if(_dscpAtom)
  {
    QTV_Delete( _dscpAtom );
  }
  if(_dcmdAtom)
  {
    QTV_Delete( _dcmdAtom );

  }
  if(_rtngAtom)
  {
    QTV_Delete( _rtngAtom );
  }
  if(_kywdAtom)
  {
    QTV_Delete( _kywdAtom );
  }
  if(_clsfAtom)
  {
    QTV_Delete( _clsfAtom );
  }
  if(_lociAtom)
  {
    QTV_Delete( _lociAtom );
  }
  if(_gnreAtom)
  {
    QTV_Delete( _gnreAtom );
  }
  if(_perfAtom)
  {
    QTV_Delete( _perfAtom );
  }

  if(_metaAtom)
  {
    QTV_Delete( _metaAtom );
  }

  #ifdef FEATURE_QTV_PDCF
  if(_pdcfAtom)
  {
    QTV_Delete( _pdcfAtom );
  }
  /* free the m_pEncryptedDataBuffer*/
  if(m_pEncryptedDataBuffer)
  {
    QTV_Free(m_pEncryptedDataBuffer);
  }
#endif /* FEATURE_QTV_PDCF */

  /* now close the parse file pointer */

  if(m_parseFilePtr)
  {
    (void)OSCL_FileClose( m_parseFilePtr );

  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION  getMovieDuration

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 Mpeg4File::getMovieDuration() const
{
  return m_videoFmtInfo.file_info.total_movie_duration;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getMovieDurationMsec

DESCRIPTION
Get total movie duration in msec

===========================================================================*/
uint32 Mpeg4File::getMovieDurationMsec() const
{
  float msec=0;
  uint32 duration = getMovieDuration();
  uint32 timescale = getMovieTimescale();
  if (timescale==0)
  {
    msec = (float)duration * (float)TIMESCALE_BASE;
  }
  else
  {
    msec = (float)duration * (float)TIMESCALE_BASE / (float)timescale;
  }
  return (uint32)msec;
}

/*===========================================================================

FUNCTION  isFileFragmented

DESCRIPTION
Get total movie duration in msec

===========================================================================*/
bool Mpeg4File::isFileFragmented(void)
{
  return m_isFragmentedFile;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackVideoFrameRate

DESCRIPTION
Get total movie duration

===========================================================================*/
float  Mpeg4File::getTrackVideoFrameRate(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);
  if ( !p_track )
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "getNextMediaSample, unknown track id = %d", id);

  if ( p_track && p_track->type == VIDEO_FMT_STREAM_VIDEO )
  {
    return p_track->subinfo.video.frame_rate;
  }

  return 0;

}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackContentVersion

DESCRIPTION
Get total movie duration

===========================================================================*/
int16 Mpeg4File::getTrackContentVersion(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);
  if ( p_track )
  {
    return (int16)p_track->user_data.content_version;
  }

  return 0;
} // from 'cver' atom at track level

/* <EJECT> */
/*===========================================================================

FUNCTION  trackRandomAccessDenied

DESCRIPTION
Get total movie duration

===========================================================================*/
uint8 Mpeg4File::trackRandomAccessDenied(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
    return p_track->user_data.no_rand_access;
  }

  return 0; // by default allow random access

}// from 'rand' atom at track level
#ifdef FEATURE_QTV_PDCF
/*===========================================================================

FUNCTION  getEncryptionType

DESCRIPTION
Return the type of encryption if any of the track is encrypted.

===========================================================================*/
QtvPlayer::EncryptionTypeT Mpeg4File::getEncryptionType()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4File::getEncryptionType");
  for(int i=0; i<VIDEO_FMT_MAX_MEDIA_STREAMS; i++)
  {
    if( m_EncryptionType[i].encryptionType != QtvPlayer::ENCRYPT_NONE )
    {
      return m_EncryptionType[i].encryptionType;
    }
  }
  return QtvPlayer::ENCRYPT_NONE;
}

/*===========================================================================

FUNCTION  getEncryptionType

DESCRIPTION
Return the type of encryption if it is encrypted.

===========================================================================*/
QtvPlayer::EncryptionTypeT Mpeg4File::getEncryptionType(uint32 track_id)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4File::getEncryptionType");
  for(int i=0; i<VIDEO_FMT_MAX_MEDIA_STREAMS; i++)
  {
    if( m_EncryptionType[i].track_id == track_id )
    {
      return m_EncryptionType[i].encryptionType;
    }
  }
  return QtvPlayer::ENCRYPT_NONE;
}

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
bool Mpeg4File::RegisterDRMDecryptMethod(QtvPlayer::DRMDecryptMethodT pDRMDecryptFunction, void *pClientData)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4File::RegisterDRMDecryptMethod");
  m_pDRMClientData = pClientData;
  m_pDRMDecryptFunction = (QtvPlayer::DRMDecryptMethodT)pDRMDecryptFunction;
  return true;
}
#endif /* FEATURE_QTV_PDCF */
/* <EJECT> */
/*===========================================================================

FUNCTION  trackDependsOn

DESCRIPTION
Get total movie duration

===========================================================================*/
// From TrackReference
uint32  Mpeg4File::trackDependsOn(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
    if ( p_track->ref_track.ref_atom == DPND_TYPE /* 'dpnd' */)
    {
      return p_track->ref_track.track_id[0];
    }
  }

  return 0;

}

/* ======================================================================
FUNCTION
Mpeg4File::getTrackMediaDuration

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
// From MediaHeader
uint32 Mpeg4File::getTrackMediaDuration(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
    return p_track->media_duration;
  }
  else
    return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackMediaTimescale

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 Mpeg4File::getTrackMediaTimescale(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
    return p_track->media_timescale;
  }
  else
    return 0;
}

/*===========================================================================

FUNCTION  getTrackAudioSamplingFreq

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 Mpeg4File::getTrackAudioSamplingFreq(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

if ( p_track )
{
#ifdef FEATURE_MP4_MP3
   return p_track->subinfo.audio.sampling_frequency;
#else
   if (p_track->subinfo.audio.sampling_frequency)
   {
     return p_track->subinfo.audio.sampling_frequency;
   }
   else
   {
     return p_track->media_timescale;
   }
#endif /* FEATURE_MP4_MP3 */
}
else
   return 0;
}


/*===========================================================================

FUNCTION  getAudioTrackLanguage

DESCRIPTION
Get tarck language

===========================================================================*/
OSCL_STRING Mpeg4File::getAudioTrackLanguage(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);
  if ( p_track )
  {
    OSCL_STRING Language;
    char *str =(char *)QTV_Malloc(QTV_MAX_LANGUAGE_BYTES);
    if (!str)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                        "Mpeg4File::getAudioTrackLanguage() failed to allocate memory");
      return 0;
    }
    str[0] = (char)(p_track->media_language >> 10) + '`';
    str[1] = (char)((p_track->media_language >> 5) & 0x1F) + '`';
    str[2] = (char)(p_track->media_language & 0x1F) + '`';
    str[3] = '\0';
    Language = str;
    QTV_Free(str);
    return Language;
  }
  else
    return 0;
}


/* <EJECT> */

/*===========================================================================

FUNCTION  getAudioSamplesPerFrame

DESCRIPTION
Get total number of audio (codec) samples per MPEG4 sample entry
(which may contain one or more MPEG4 frames).

NOTE: Each MPEG4 frame contains a number of codec samples

===========================================================================*/
uint32 Mpeg4File::getAudioSamplesPerFrame(uint32 id)
{
  uint32 samplesPerFrame = 0;
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( !p_track )
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "getNextMediaSample, unknown track id = %d", id);

  if ( p_track && p_track->type == VIDEO_FMT_STREAM_AUDIO )
  {
    switch ( p_track->subinfo.audio.format )
    {
    case VIDEO_FMT_STREAM_AUDIO_EVRC:
      samplesPerFrame =
        (uint32)(p_track->subinfo.audio.audio_params.frames_per_sample * Media::EVRC_SAMPLES_PER_FRAME);
      break;

    case VIDEO_FMT_STREAM_AUDIO_EVRC_PV:
      samplesPerFrame = (uint32)(Media::EVRC_SAMPLES_PER_FRAME);
      break;

    case VIDEO_FMT_STREAM_AUDIO_PUREVOICE:
      samplesPerFrame =
        (uint32)(p_track->subinfo.audio.audio_params.frames_per_sample * Media::QCELP_SAMPLES_PER_FRAME);
      break;

    case VIDEO_FMT_STREAM_AUDIO_AMR:
      {
        video_fmt_sample_info_type sampleInfo;
        uint32 streamNum = p_track->stream_num;

        if ( getSampleInfo(streamNum, 0, 1, &sampleInfo) > 0 )
        {
          /* for some clips, SamplesPerFrame value is wrongly set to 10 and the correct
          value was one. This check is to fix those clips and we also try to
          minimize the scope of this fix by checking this value in clip and
          size of first AMR sample from a clip in question/given clip*/
          if( ((sampleInfo.size==32) || (sampleInfo.size==13) || (sampleInfo.size==21) || (sampleInfo.size==18))&&
               (p_track->subinfo.audio.audio_params.frames_per_sample==10) )
          {
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "SamplesPerFrame=%d for AMR track, but using 1.",
            p_track->subinfo.audio.audio_params.frames_per_sample);
            p_track->subinfo.audio.audio_params.frames_per_sample = 1;
          }
        }
      }
      samplesPerFrame = (uint32)(p_track->subinfo.audio.audio_params.frames_per_sample *
        Media::AMR_SAMPLES_PER_FRAME);
      break;

    case VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL:
      samplesPerFrame = (uint32)(Media::QCELP_SAMPLES_PER_FRAME);
      break;

#ifdef FEATURE_MP4_MP3
    case VIDEO_FMT_STREAM_AUDIO_MPEG1_L3:
    case VIDEO_FMT_STREAM_AUDIO_MPEG2_L3:
      samplesPerFrame = (uint32)(Media::MP3_SAMPLES_PER_FRAME);
      break;
#endif /* FEATURE_MP4_MP3 */
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

    default:
      break;
    }
  }

  return samplesPerFrame;

}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackDecoderSpecificInfoAtSDI

DESCRIPTION
Get total movie duration

===========================================================================*/
// From DecoderConfigDescriptor
uint8 *Mpeg4File::getTrackDecoderSpecificInfoAtSDI(uint32 trackID, uint32)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(trackID);

  if ( p_track )
  {
    if ((p_track->type == VIDEO_FMT_STREAM_AUDIO)
      && (p_track->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_AMR))
    {
      return (uint8 *)&p_track->subinfo.audio.audio_params;
    }
    else
      return(uint8 *)&p_track->dec_specific_info;
  }
  return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  Mpeg4File::getTrackDecoderSpecificInfoContent

DESCRIPTION
===========================================================================*/
MP4_ERROR_CODE Mpeg4File::getTrackDecoderSpecificInfoContent(uint32 id, uint8* buf, uint32 *pbufSize)
{
  int32 bufsize = 0;
  uint32 headersize=0;
  uint8 *poutputtemp = NULL;
  MP4_ERROR_CODE readstatus = EVERYTHING_FINE;

  if(pbufSize == NULL)
  {
     return (DEFAULT_ERROR);
  }

  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  /* Sanity check for pointer*/
  if(p_track == NULL)
  {
     return (DEFAULT_ERROR);
  }

  /*Initialize Parameters*/
  poutputtemp = buf;
  bufsize = *pbufSize;

#ifdef FEATURE_H264_DECODER
  /*Check if the input media type is H264*/
 if( (p_track->type == VIDEO_FMT_STREAM_VIDEO) &&
     (p_track->subinfo.video.format == VIDEO_FMT_STREAM_VIDEO_H264) )
 {
    int32 nBytes = 0;
    uint8* psize;
    uint8  *ptempbuf = NULL;

      /*Allocate temp buffer*/
    ptempbuf = (uint8 *) QTV_Malloc (ALLOC_MEMORY_HEADER);

    /*Check for allocation failure*/
    if(ptempbuf == NULL)
    {
       /*set header size to 0*/
       *pbufSize = 0;
       return (MEMORY_ALLOCATION_FAILED);
    }
    psize = (uint8 *) &nBytes;

    /*Reset the NAL parameters */
    resetParamSetNAL(id);

          do
          {
             /*read the next NAL unit in the file*/
                nBytes = getNextParamSetNAL(id, ptempbuf, ALLOC_MEMORY_HEADER);
                   if(nBytes == FRAGMENT_CORRUPT)
                   {
                       QTV_Free(ptempbuf);
                       return(DEFAULT_ERROR);
                   }
             if( poutputtemp != NULL   &&  /*Check if ouput buffer is valid*/
                    nBytes > 0    &&          /*Check if we have reached end of header*/
                    bufsize >= (nBytes + 4)   /* Check if we have enough space in input buffer*/
                    )
               {

                    //ARM is big-endian, and decoder expects size in little-endian
                    for( int i = 0; i < 4; i++ )
                    {
                         poutputtemp[i] = psize[ 3 - i ];
                    }
                    /*increment pointer*/
                    poutputtemp += 4;
                    /*Copy the header related stuff*/
                    memcpy (poutputtemp,ptempbuf,nBytes);
                    /*Increment the Source pointer*/
                    poutputtemp += nBytes;
                    /*Decrement the input buffer size*/
                    bufsize -= (nBytes + 4);
               }
               /*
               Buffer that client has supplied is not enough
               Hence Flag error
               */
               else if ( poutputtemp == NULL   ||
                    bufsize < (nBytes +4) ||
                    bufsize < 0
                    )
               {
                    readstatus = (MP4_ERROR_CODE)INSUFFICIENT_BUFFER_SIZE;
               }
               /*If nothing is read avoid incrementing headersize*/
               if(nBytes)
               {
                    /*Increment Header Size*/
                    headersize += (nBytes+4);
               }
          }
          while (nBytes);
          /*
          Not enough memory to add the nal length information
          then return error
          */
          if ( poutputtemp == NULL ||
               bufsize < 2 )
          {
               readstatus = (MP4_ERROR_CODE)INSUFFICIENT_BUFFER_SIZE;
          }
          else
          {
               //This is not part of header but output of below two methods is needed
               //to configure VDEC object for decoding H.264 clip.
               //Means, it will be input parameter for vdec_h264_init_decoder_specific_info
               poutputtemp[0]   = GetSizeOfNALLengthField(id);

               //We are going to work with Stored media, so fix this one as zero.
               poutputtemp[1] = 0;

               headersize+=2;
          }

          /*Reset the NAL parameters */
          resetParamSetNAL(id);

          /*
          Check if Buffer is allocated
          Free if allocated
          */
          QTV_Free (ptempbuf);

 } /*Video format is H.264*/
 else
#endif
 {
   /*Check if the input buffer has enough space*/
   if( (bufsize >= p_track->header) && (poutputtemp) )
   {
     /*Copy the header*/
     memcpy (poutputtemp,p_track->dec_specific_info.info,p_track->header);
     /*Update header size*/
     headersize = p_track->header;
   }
   else
   {
       /*
         Don't have enough space hence update the Status.
         Send actual header size
       */
     readstatus = (MP4_ERROR_CODE)INSUFFICIENT_BUFFER_SIZE;
     headersize = p_track->header;
   }
 }
   /*Update the size of header*/
 *pbufSize = headersize;
 return (readstatus);
}
/*===========================================================================

FUNCTION  getTrackDecoderSpecificInfoContent

DESCRIPTION
Get total movie duration

===========================================================================*/
uint8 *Mpeg4File::getTrackDecoderSpecificInfoContent(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
    return &p_track->dec_specific_info.info[0];
  }

  return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackDecoderSpecificInfoSize

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 Mpeg4File::getTrackDecoderSpecificInfoSize(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
    return p_track->header;
  }

  return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackOTIType

DESCRIPTION
Get total movie duration

===========================================================================*/
// Based on OTI (ObjectTypeIndication) value from the 'esds' atom
uint8  Mpeg4File::getTrackOTIType(uint32 id)
{
  uint8 format = 0xFF;
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
     format = p_track->dec_specific_info.obj_type;
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */

#ifdef FEATURE_H264_DECODER
    if (p_track->type == VIDEO_FMT_STREAM_VIDEO)
    {
      switch(p_track->subinfo.video.format)
      {
      case VIDEO_FMT_STREAM_VIDEO_H263:
        format = (uint8)H263_VIDEO;
        break;

      case VIDEO_FMT_STREAM_VIDEO_H264:
        format = (uint8)H264_VIDEO;
        break;

      default:
        break;
      }
    }
#else
    /* Since the VideoDec::PrepPlay performs special H.263 codec
    ** initialization only if the OTI value indicated is H263_VIDEO, we need
    ** to detect H.263 content that is packaged as MPEG-4 video in short
    ** header mode.  Otherwise, the function will mis-interpret the stream as
    ** MPEG-4 video and not H.263.
    */
    if ((p_track->type == VIDEO_FMT_STREAM_VIDEO)
      && (p_track->subinfo.video.format == VIDEO_FMT_STREAM_VIDEO_H263))
    {
      format = (uint8)H263_VIDEO;
    }
#endif /* FEATURE_H264_DECODER */
    if (!p_track->header &&(p_track->type == VIDEO_FMT_STREAM_VIDEO)&&
          (p_track->subinfo.video.format == VIDEO_FMT_STREAM_VIDEO_MPEG4))
    {
      format = (uint8)H263_VIDEO;
    }
    if ( (p_track->dec_specific_info.obj_type == MPEG4_VIDEO) &&
         (p_track->frames == 1) )
    {
      /* this is Mpeg4 Video CODEC with just one video frame. We will treat
       it as STILL IMAGE frame, so that audio track repositioning can work */
       format = MPEG4_IMAGE;
    }

    if((p_track->frames == 1)&&(p_track->type == VIDEO_FMT_STREAM_VIDEO)
        && (p_track->subinfo.video.format == VIDEO_FMT_STREAM_VIDEO_H263))
    {
      /* this is H263 Video CODEC with just one video frame. We will treat
       it as STILL IMAGE frame, so that audio track repositioning can work */
       format = H263_IMAGE;
    }

    /* Audio special treatment
    */
    if (p_track->type == VIDEO_FMT_STREAM_AUDIO)
    {
      switch(p_track->subinfo.audio.format)
      {
      case VIDEO_FMT_STREAM_AUDIO_AMR:
        format = (uint8)AMR_AUDIO;
        break;

      case VIDEO_FMT_STREAM_AUDIO_EVRC:
      case VIDEO_FMT_STREAM_AUDIO_EVRC_PV:
        format = (uint8)EVRC_AUDIO;
        break;

      case VIDEO_FMT_STREAM_AUDIO_PUREVOICE:
        format = (uint8)PUREVOICE_AUDIO; // 3GPP2 QCELP
        break;

#ifdef FEATURE_MP4_MP3
      case VIDEO_FMT_STREAM_AUDIO_MPEG1_L3:
      case VIDEO_FMT_STREAM_AUDIO_MPEG2_L3:
        format = (uint8)MP3_AUDIO;
        break;
#endif

      case VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL:
        format = (uint8)PUREVOICE_AUDIO_2; // 3GPP2 QCELP
        break;
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
      default:
        break;
      }
    }
  }

  return format;

}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTracAudioFormat

DESCRIPTION
Gets the audio format of given track in video_fmt_stream_audio_subtype type.

===========================================================================*/
uint8 Mpeg4File::getTrackAudioFormat(uint32 id)
{
  uint8 format = 0xFF;
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track && (p_track->type == VIDEO_FMT_STREAM_AUDIO) )
  {
    return (uint8)(p_track->subinfo.audio.format);
  }

  return format;
}


/* <EJECT> */
/*===========================================================================

FUNCTION  getFramesPerSample

DESCRIPTION
Gets the number frames per sample of given track in video_fmt_stream_audio_subtype type.

===========================================================================*/
uint8 Mpeg4File::getFramesPerSample(uint32 id)
{
  uint8 num_frames = 0x00;
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track && (p_track->type == VIDEO_FMT_STREAM_AUDIO) )
  {
    return (uint8)(p_track->subinfo.audio.audio_params.frames_per_sample);
  }

  return num_frames;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTotalNumberOfFrames

DESCRIPTION
Gets the total number of frames in a given track.

===========================================================================*/
uint16 Mpeg4File::getTotalNumberOfFrames(uint32 id)
{
  uint8 total_num_frames = 0x00;
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track && (p_track->type == VIDEO_FMT_STREAM_AUDIO) )
  {
    return (uint16)(p_track->frames);
  }

  return total_num_frames;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackMaxBufferSizeDB

DESCRIPTION
Get total movie duration
This function takes care of long overflow by taking MAX.

===========================================================================*/
int32 Mpeg4File::getTrackMaxBufferSizeDB(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if (p_track )
  {
    if(!p_track->largest)
      p_track->largest = 10240; //Defaulting to 10K.

    if( (p_track->type == VIDEO_FMT_STREAM_AUDIO) &&
      ( (p_track->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_AMR) ||
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
      (p_track->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_EVRC)||
      (p_track->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_PUREVOICE))  )
    {
     return (int32)MAX( 1024, p_track->largest );
    }

    else if ( (p_track->type == VIDEO_FMT_STREAM_VIDEO) &&
      (p_track->subinfo.video.format == VIDEO_FMT_STREAM_VIDEO_H263) )

    {

      //Return the largest sample entry size in sample table and add 3 bytes in case
      // decoder looks forward 3 bytes since the H.263 track atom does not have any decoder specific info
      return (int32)MAX( (p_track->largest+p_track->header+3), p_track->largest );

    }
#ifdef FEATURE_H264_DECODER
    else if ( (p_track->type == VIDEO_FMT_STREAM_VIDEO) &&
      (p_track->subinfo.video.format == VIDEO_FMT_STREAM_VIDEO_H264) )

    {
      //Return the largest sample entry size in sample table and add 1 byte to make it NAL.
      return (int32)MAX( (p_track->largest+p_track->header+1), p_track->largest );
    }
#endif /* FEATURE_H264_DECODER */
    else
    {
      return (int32)MAX( (p_track->largest+p_track->header), p_track->largest );
    }
  }
  else
    return 0;
}

/*===========================================================================

FUNCTION  getLargestFrameSize

DESCRIPTION
  Public method used to find the largest frame size in a given track.

===========================================================================*/
uint32 Mpeg4File::getLargestFrameSize ( uint32 id )
{
  uint32 nretVal = 0;
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);
  if (p_track)
  {
    if (!p_track->largest_found)
    {
      /* call this call back to find the largest frame size
          in the given track. */
      m_videoFmtInfo.largest_frame_size_cb (m_videoFmtInfo.server_data,
                                            p_track->stream_num);
      /* If the largest is found */
      if(p_track->largest_found)
      {
        /* Return the largest */
        nretVal = p_track->largest;
      }
    }
    else
    {
      nretVal = p_track->largest;
    }
  }
  return nretVal;
}


/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackAverageBitrate

DESCRIPTION
Get total movie duration

===========================================================================*/
int32 Mpeg4File::getTrackAverageBitrate(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);
  int32 bitRate = 0;

  if ( p_track )
  {
    /* Special case for AMR audio tracks */
#ifdef FEATURE_MP4_AMR
    if ((p_track->type == VIDEO_FMT_STREAM_AUDIO)
      && (p_track->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_AMR))
    {
      for (uint32 i = 0; i < 8; i++)
      {
        if ( p_track->subinfo.audio.audio_params.mode_set & AMRModeSetMask[i])
        {
          bitRate = AMRBitRates[i];
        }
      }
    }
    else
#endif /* FEATURE_MP4_AMR */
    { /* If the bitrate is encoded, then use that value */
      if( p_track->dec_specific_info.avgbitrate != 0 )
      {
        bitRate = (int32)(p_track->dec_specific_info.avgbitrate);
      }
      else
      {
        bitRate = 0;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Avg bitRate set to 0: p_track->bytes not available");
      }
    }
  }

  return bitRate;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackMaxBitrate

DESCRIPTION
Get total movie duration

===========================================================================*/
int32 Mpeg4File::getTrackMaxBitrate(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
#ifdef FEATURE_MP4_AMR
    if ((p_track->type == VIDEO_FMT_STREAM_AUDIO)
      && (p_track->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_AMR))
    {
      int32 bitRate = 0;
      for (uint32 i = 0; i < 8; i++)
      {
        if (p_track->subinfo.audio.audio_params.mode_set & AMRModeSetMask[i])
        {
          bitRate = AMRBitRates[i];
        }
      }
      return bitRate;
    }
    else
#endif /* FEATURE_MP4_AMR */
      return (int32)(p_track->dec_specific_info.maxbitrate);
  }
  else
    return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getMovieTimescale

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 Mpeg4File::getMovieTimescale() const
{
  return m_videoFmtInfo.file_info.movie_timescale;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackVideoFrameWidth

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 Mpeg4File::getTrackVideoFrameWidth(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
    return p_track->subinfo.video.width;
  }
  return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackVideoFrameHeight

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 Mpeg4File::getTrackVideoFrameHeight(uint32 id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
    return p_track->subinfo.video.height;
  }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  GetNumAudioChannels

DESCRIPTION
returns the number of audio channels

==========================================================================*/
unsigned long Mpeg4File::GetNumAudioChannels(int id)
{
  video_fmt_stream_info_type *p_track = getTrackInfoForID(id);

  if ( p_track )
  {
    /*Allowing specific clips given by the customer to work by
    setting the  num_channels to one even when the info in the clip is zero*/
    return MAX(1,p_track->subinfo.audio.num_channels);
  }
  return 0;
}


/* <EJECT> */
/*===========================================================================

FUNCTION  randomAccessDenied

DESCRIPTION
Get total movie duration

===========================================================================*/
uint8 Mpeg4File::randomAccessDenied()
{
  uint8 nRet = FALSE;
  if(m_videoFmtInfo.file_info.no_rand_access)
  {
    nRet = TRUE;
  }
  return nRet;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getAllowVideoOnly

DESCRIPTION
Get total movie duration

===========================================================================*/
uint8 Mpeg4File::getAllowVideoOnly()
{
  return m_videoFmtInfo.file_info.video_only_allowed;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getAllowAudioOnly

DESCRIPTION
Get total movie duration

===========================================================================*/
uint8 Mpeg4File::getAllowAudioOnly()
{
  return m_videoFmtInfo.file_info.audio_only_allowed;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getCompatibiltyMajorBrand

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 Mpeg4File::getCompatibiltyMajorBrand()
{
  return 0;
}
/* <EJECT> */
/*===========================================================================
getCompatibiltyMajorBrandVersion
FUNCTION  getTrackMediaTimescale

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 Mpeg4File::getCompatibiltyMajorBrandVersion()
{
  return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getCompatibiltyList

DESCRIPTION
Get total movie duration

===========================================================================*/
ZArray<uint32>* Mpeg4File::getCompatibiltyList()
{
  return 0;
}
/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackMediaTimescale

DESCRIPTION
Get total movie duration

===========================================================================*/
int32 Mpeg4File::getNumTracks()
{
  int32 totalTracks = 0;
  video_fmt_stream_info_type *p_track = 0;

  /* Errors may happen in mp4ParseStatus, i.e., VIDEO_FMT_ALLOC may fail,
     so need to check errors before obtaining tracks info
  */
  /*Sync Errors may happen due to Invalid AMR audio content */
  if(!_success)
  {
     return 0;
  }

  for ( uint32 index = 0; index < m_videoFmtInfo.num_streams; index++)
  {
    p_track = m_videoFmtInfo.streams + index;

    if ( (p_track->type == VIDEO_FMT_STREAM_AUDIO) ||
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      (p_track->type == VIDEO_FMT_STREAM_TEXT)  ||
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
      (p_track->type == VIDEO_FMT_STREAM_VIDEO) )
    {
      if(p_track->type == VIDEO_FMT_STREAM_AUDIO)
        m_bAudioPresentInClip = true;
      else if(p_track->type == VIDEO_FMT_STREAM_VIDEO)
        m_bVideoPresentInClip = true;
      else if(p_track->type == VIDEO_FMT_STREAM_TEXT)
        m_bTextPresentInClip = true;
      totalTracks++;
    }
    else
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                    "track id %d is unsupported/unknown track type", p_track->track_id);
  }

  /* Added this to correct the play information sent in constructor*/
  if(m_playAudio)
  {
    if(!m_bAudioPresentInClip)
      m_playAudio = false;
  }
  if(m_playVideo)
  {
    if(!m_bVideoPresentInClip)
      m_playVideo = false;
  }
  if(m_playText)
  {
    if(!m_bTextPresentInClip)
      m_playText = false;
  }

  return totalTracks;
}
/* <EJECT> */

/*===========================================================================

FUNCTION  Mpeg4File::isAudioPresentInClip()

DESCRIPTION
  Returns TRUE if AUDIO track is detected in the Clip.
  This is useful in Notifying UNSUPPORTED_AUDIO_TRACK if
  we fail to detect correct codec type but clip has AUDIO TRACK in it.

===========================================================================*/
bool  Mpeg4File::isAudioPresentInClip()
{
  return m_bAudioPresentInClip;
}
/* <EJECT> */

/*===========================================================================

FUNCTION  getTrackWholeIDList

DESCRIPTION
Get total movie duration

===========================================================================*/
uint32 Mpeg4File::getTrackWholeIDList(uint32 *ids)
{
  uint32 count = 0;
  video_fmt_stream_info_type *p_track = 0;

  if (ids == NULL)
  {
    return 0;
  }

  for (uint32 i = 0; i < m_videoFmtInfo.num_streams; i++)
  {
    p_track = m_videoFmtInfo.streams + i;

    /* some clips have erorr where we don't have time scale information.
    In that case, we will not play that track */
    if( !p_track->media_timescale )
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Non Playable Track=%d, TimeScale is zero", p_track->stream_num);
      continue;
    }

    if ( (p_track->type == VIDEO_FMT_STREAM_AUDIO) ||
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      (p_track->type == VIDEO_FMT_STREAM_TEXT)  ||
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
      (p_track->type == VIDEO_FMT_STREAM_VIDEO) )
    {
      (*ids) = p_track->track_id;
      ids++;
      count++;
    }
  }
  return count;
}


/* <EJECT> */
/*===========================================================================

FUNCTION  getTrackInfoForID

DESCRIPTION
Public method providing the capability to read data from a MP4 file.
It fetches the media track whose track_id matches the input parameter 'id'

===========================================================================*/
video_fmt_stream_info_type * Mpeg4File::getTrackInfoForID (uint32 id)
{
  for (uint32 index = 0; index < m_trackCount; index++)
  {
    if ( m_track[index]->track_id == id )
    {
      return m_track[index];
    }
  }
  return 0;
}

/*===========================================================================

FUNCTION  getTrackIdFromStreamNum

DESCRIPTION
Public method providing the capability to read trackId corresponding to the
given input parameter streamNum.

===========================================================================*/
uint32 Mpeg4File::getTrackIdFromStreamNum (uint32 streamNum)
{
  for (uint32 index = 0; index < m_trackCount; index++)
  {
    if ( m_track[index]->stream_num == streamNum )
    {
      return m_track[index]->track_id;
    }
  }
  return 0;
}

/*===========================================================================

FUNCTION  process_video_fmt_info

DESCRIPTION
Private method called from mp4ParseStatus().

===========================================================================*/
void Mpeg4File::process_video_fmt_info(video_fmt_status_type status,
                                       video_fmt_status_cb_info_type *info)
{
  status = status; //variable not in use, required to remove compile time warning

  if(status == VIDEO_FMT_INFO)
  {
    m_parsedEndofFile = true;
  }
  else if(status == VIDEO_FMT_FRAGMENT)
  {
    m_parsedEndofFragment = true;
    m_parsedEndofFile = false;
  }

  memcpy ( &m_videoFmtInfo, (void *)&info->info, sizeof(video_fmt_info_type) );

  // Save stream reading callback.
  m_mp4ParseServerData = info->info.server_data;

  // Calculate the frame rate for all tracks
  uint32 index;
  video_fmt_stream_info_type *p_track = NULL;

  //Recalculate the toatal number of tracks in this new parsed fragment.
  m_trackCount = 0;

  for (index = 0; index < m_videoFmtInfo.num_streams; index++)
  {
    p_track = m_videoFmtInfo.streams + index;

    if (p_track != NULL)
    {
#ifdef FEATURE_QTV_PDCF
      if(m_OffsetinEncryptionTypeArray < VIDEO_FMT_MAX_MEDIA_STREAMS)
      {
        if(( (p_track->subinfo.video.pdcf_info.scheme_type == ODKM_TYPE)
              && (p_track->subinfo.video.pdcf_info.scheme_version == OMA_DRM_SCHEME_VERSION))
           || ((p_track->subinfo.audio.pdcf_info.scheme_type == ODKM_TYPE)
              && (p_track->subinfo.audio.pdcf_info.scheme_version == OMA_DRM_SCHEME_VERSION)))
        {
          m_EncryptionType[m_OffsetinEncryptionTypeArray].track_id = p_track->track_id;
          m_EncryptionType[m_OffsetinEncryptionTypeArray].encryptionType = QtvPlayer::ENCRYPT_OMA_DRM_V2;
          m_OffsetinEncryptionTypeArray++;
        }
      }
#endif /* FEATURE_QTV_PDCF */
      switch ( p_track->type )
      {
      case VIDEO_FMT_STREAM_VIDEO:
        if(m_playVideo)
        {
          if(!p_track->frames)
          {
            m_hasVideo = false;
          }
          else
          {
            m_hasVideo = true;
          }

          if ( p_track->subinfo.video.frame_rate == 0 )
          {
            // frame_rate = frames / (track duration / track timescale)
            if ( p_track->media_duration > 0 )
            {
              /* temporarily typecast to float to prevent uint32 overflow for large clips */
              p_track->subinfo.video.frame_rate =
                (uint16)((float)p_track->frames * p_track->media_timescale / p_track->media_duration);
              /* since we return only uint16, make sure frame rate is at least one */
              p_track->subinfo.video.frame_rate = MAX(1, p_track->subinfo.video.frame_rate);
            }
            else
            {
              p_track->subinfo.video.frame_rate =
                (uint16)(p_track->frames * p_track->media_timescale);
            }
          }
        }
        /*
        * m_track is declared as
        * video_fmt_stream_info_type* m_track[VIDEO_FMT_MAX_MEDIA_STREAMS];
        * Make sure (m_trackCount+1) is within the correct limits before attempting to write to it.
        */
        if((m_trackCount+1)<VIDEO_FMT_MAX_MEDIA_STREAMS)
        {
          m_track[m_trackCount++] = p_track;
        }
        else
        {
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "VIDEO_FMT_STREAM_VIDEO:m_trackCount OVERFLOW %d", m_trackCount);
        }
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO: p_track->frames = %d", p_track->frames);
        break;

      case VIDEO_FMT_STREAM_AUDIO:
        if(m_playAudio)
        {
          if(!p_track->frames)
          {
            m_hasAudio = false;
          }
          else
          {
            m_hasAudio = true;
          }
        }
         /*
        * m_track is declared as
        * video_fmt_stream_info_type* m_track[VIDEO_FMT_MAX_MEDIA_STREAMS];
        * Make sure (m_trackCount+1) is within the correct limits before attempting to write to it.
        */
        if((m_trackCount+1)<VIDEO_FMT_MAX_MEDIA_STREAMS)
        {
          m_track[m_trackCount++] = p_track;
        }
        else
        {
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "VIDEO_FMT_STREAM_AUDIO:m_trackCount OVERFLOW %d", m_trackCount);
        }
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "AUDIO: p_track->frames = %d", p_track->frames);
        break;

      case VIDEO_FMT_STREAM_TEXT:
        if(m_playText)
        {
          if(!p_track->frames)
          {
            m_hasText = false;
          }
          else
          {
            m_hasText = true;
          }
        }
        /*
        * m_track is declared as
        * video_fmt_stream_info_type* m_track[VIDEO_FMT_MAX_MEDIA_STREAMS];
        * Make sure (m_trackCount+1) is within the correct limits before attempting to write to it.
        */
        if((m_trackCount+1)<VIDEO_FMT_MAX_MEDIA_STREAMS)
        {
          m_track[m_trackCount++] = p_track;
        }
        else
        {
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "VIDEO_FMT_STREAM_TEXT:m_trackCount OVERFLOW %d", m_trackCount);
        }
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "TEXT: p_track->frames = %d", p_track->frames);
        break;

      default:
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Unsupported VideoFMT track=%d", p_track->stream_num);
        break;
      }
    }
  }
#ifdef FEATURE_MEASURE_TIMING
  endTimeGetMetadata = ZUtils::Clock();
  int32 timeElapsed = endTimeGetMetadata - startTimeGetMetadata;
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "*** Time Elapsed = %d", timeElapsed );
#endif
}

/* ======================================================================
FUNCTION
Mpeg4File::mp4ParseStatus

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
void Mpeg4File::mp4ParseStatus (video_fmt_status_type status,
                                video_fmt_status_cb_info_type *info,
                                video_fmt_end_cb_func_type end)
{
 // Store the status code for later inspection.
  m_mp4ParseLastStatus = status;
  m_parsedEndofFragment = false;

  // Store session end callback function.
  m_mp4ParseEndCb = end;

  /*
  * m_parseFilePtr is valid only for initial parsing.
  * For subsequent fragment parsing, we will use one of the already opened
  * file pointer for playing track because we want to limit
  * opened file pointers to one per stream.
  * Some OEM have limited file pointers and for some OEM opening and
  * closing file pointers are very CPU expensive.
  *
  * Validation of localParseFilePtr is done below before use.
  */
  OSCL_FILE* localParseFilePtr = m_parseFilePtr;
  if(!localParseFilePtr)
  {
    //Most likely, this will happen when parsing subsequent fragments in fragmented clip.
    for(int i =0;i<VIDEO_FMT_MAX_MEDIA_STREAMS;i++)
    {
      if(m_streamFilePtr[i])
      {
        localParseFilePtr = m_streamFilePtr[i];
        break;
      }
    }
  }

  // Branch according to the status code received.
  switch (status)
  {
  case VIDEO_FMT_ALLOC:
    info->alloc.ptr = (char*) QTV_Malloc(info->alloc.size);
    if ( info->alloc.ptr == NULL )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_ALLOC failed to allocate memory");
    }
    else
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_ALLOC");
    break;

  case VIDEO_FMT_FREE:
    QTV_Free(info->free.ptr);
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_FREE");
    break;

  case VIDEO_FMT_FILESIZE:
    if(!bHttpStreaming)
    info->fSize.fileSize = (m_fileSizeFound) ? m_fileSize : 0;
    else
      info->fSize.fileSize = 0xFFFFFFFF;
    break;

  case VIDEO_FMT_GET_DATA:
    {
      if(localParseFilePtr == NULL)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "VIDEO_FMT_GET_DATA localParseFilePtr is NULL");
        break;
      }
      ASSERT(localParseFilePtr);
      m_mp4ParseContinueCb = info->get_data.callback_ptr;
      m_mp4ParseServerData = info->get_data.server_data;

        /*This flag will be set by the UI task if the user terminates the playback*/
        if(m_pMpeg4Player && m_pMpeg4Player->m_bMediaAbort)
        {
          info->get_data.num_bytes = 0;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Failed to readFile (mp4ParseStatus): User aborted playback..!!");
            _fileErrorCode = (int32)READ_FAILED;
          break;
        }

      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
         "Read offset %d Read bytes %d", info->get_data.offset,
          info->get_data.num_bytes);
      memset(info->get_data.buffer,0,info->get_data.num_bytes);
      // Read the given number of bytes from the given offset in the
      // file.
      info->get_data.num_bytes
        = MIN (info->get_data.num_bytes,
        (m_fileSize - MIN (m_fileSize, info->get_data.offset)));

      if(bHttpStreaming && !m_parsedEndofFile)
      {
        video_fmt_mp4r_context_type *context =
          (video_fmt_mp4r_context_type *) m_videoFmtInfo.server_data;
        if(context->get_data_src_in_mdat &&
            ((info->get_data.offset + info->get_data.num_bytes -1) >= m_wBufferOffset))
       {
          //eg: h.263
          bsendParseFragmentCmd = FALSE;
          memset(info->get_data.buffer,0x0,info->get_data.num_bytes);
          /* Update the minimum offset required */
          m_minOffsetRequired = info->get_data.offset + info->get_data.num_bytes;
          info->get_data.num_bytes = 0;
          break;
        }
      }


      if ( info->get_data.num_bytes )
      {
        uint32 totalReqBytes = info->get_data.num_bytes;
        if(bHttpStreaming)
        {
          if(info->get_data.offset >= m_wBufferOffset)
          {
            info->get_data.num_bytes = 0;
            memset(info->get_data.buffer,0x0,info->get_data.num_bytes);
            break;
          }
          else if((info->get_data.offset + info->get_data.num_bytes -1) >= m_wBufferOffset)
          {
            info->get_data.num_bytes = m_wBufferOffset - info->get_data.offset;
          }
        }

        if( (info->get_data.offset+info->get_data.num_bytes) > m_fileSize )
              {
           /*
           * This can happen if file has some junk data at the end
           * or in case of encrypted file, there is some padding at the end.
           * Make sure we don't read beyond the file size.
           */
           QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "Trying to read beyond file size: offset %d nRead %d file_size %d",
                         info->get_data.offset,info->get_data.num_bytes,m_fileSize);
           /*
           * Try to adjust number of bytes to read but make sure delta is non zero
           * as info->get_data.num_bytes is unsigned int thus -ve number will overflow.
           */
           if((signed) (m_fileSize - info->get_data.offset) >=(signed) 0)
           {
                         info->get_data.num_bytes = (m_fileSize - info->get_data.offset);
           }
           else
           {
             info->get_data.num_bytes = 0;
           }
           QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "Adjusted #of bytes to read %d", info->get_data.num_bytes);
                 }
        if(info->get_data.num_bytes > 0)
        {

          info->get_data.num_bytes = readFile (localParseFilePtr,
                                               info->get_data.buffer,
                                               info->get_data.offset,
                                               info->get_data.num_bytes);

          QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "Number of bytes read: %d data Offset: %d",
                         info->get_data.num_bytes,info->get_data.offset);

          memset(info->get_data.buffer + info->get_data.num_bytes,0,
            totalReqBytes - info->get_data.num_bytes);
          if (!info->get_data.num_bytes)
                {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failed to readFile");
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
            if(!bHttpStreaming)
#endif
            {
              //This should be done for local playback as
                    //for PD,PS, it is possible to have 0 bytes being read.
              //Set the error code to let player know the exact failure.
              //This will help to stop/abort the playback.
              _fileErrorCode = (int32)READ_FAILED;
            }
          }
        }
      }
    }
    break;

  case VIDEO_FMT_PUT_DATA:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_PUT_DATA");
    break;

  case VIDEO_FMT_CONTINUE:
    {
      m_mp4ParseContinueCb = info->cont.callback_ptr;
      m_mp4ParseServerData = info->cont.server_data;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_CONTINUE");
    }
    break;

  case VIDEO_FMT_INFO:
  case VIDEO_FMT_FRAGMENT:
    {
      process_video_fmt_info(status,info);
    }
    break;

#ifdef FEATURE_MP4_KDDI_META_DATA
  case VIDEO_FMT_UUID:
    mp4ParseUUIDAtom(&(info->uuid_atom),localParseFilePtr);
    break;
#endif /* FEATURE_MP4_KDDI_META_DATA */

#ifdef FEATURE_QTV_SKT_MOD_MIDI
  case VIDEO_FMT_UDTA_CHILD:
    mp4ParseUUIDAtom(&(info->uuid_atom),localParseFilePtr);
    break;
#endif /* FEATURE_MP4_KDDI_META_DATA */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  case VIDEO_FMT_TX3G:
    if(localParseFilePtr == NULL)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "VIDEO_FMT_GET_DATA localParseFilePtr is NULL");
      break;
    }
    Parse3GPPTimedTextAtom(&(info->tx3g_atom),localParseFilePtr);
    break;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  case VIDEO_FMT_IO_DONE:
    {
      m_parseIODoneSize = info->io_done.bytes;
    }
    break;

  case VIDEO_FMT_DONE:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_DONE");
    break;

  case VIDEO_FMT_HINT:
    {
                    video_fmt_mp4_atom_type* pAtomInfo = info->hint.mp4;
  #ifdef FEATURE_QTV_PDCF
      if( pAtomInfo->type == SCHM_TYPE || pAtomInfo->type == FRMA_TYPE
         || pAtomInfo->type == OHDR_TYPE || pAtomInfo->type == ODAF_TYPE
         || pAtomInfo->type == MDRI_TYPE)
      {
        QtvPlayer::DataT atom_type = QtvPlayer::DATA_ATOM_NONE;
        QtvPlayer::DataT atom_offset_type = QtvPlayer::DATA_ATOM_NONE;
        switch(pAtomInfo->type)
        {
          case SCHM_TYPE:
            atom_type = QtvPlayer::DATA_ATOM_SCHM;
            atom_offset_type = QtvPlayer::DATA_ATOM_SCHM_OFFSET;
            break;
          case FRMA_TYPE:
            atom_type = QtvPlayer::DATA_ATOM_FRMA;
            atom_offset_type = QtvPlayer::DATA_ATOM_FRMA_OFFSET;
            break;
          case OHDR_TYPE:
            atom_type = QtvPlayer::DATA_ATOM_OHDR;
            atom_offset_type = QtvPlayer::DATA_ATOM_OHDR_OFFSET;
            break;
          case ODAF_TYPE:
            atom_type = QtvPlayer::DATA_ATOM_ODAF;
            atom_offset_type = QtvPlayer::DATA_ATOM_ODAF_OFFSET;
            break;
          case MDRI_TYPE:
            atom_type = QtvPlayer::DATA_ATOM_MDRI;
            atom_offset_type = QtvPlayer::DATA_ATOM_MDRI_OFFSET;
            break;
          default:
            break;
        }
        if((m_playAudio && (info->hint.stream_info->type == VIDEO_FMT_STREAM_AUDIO)) ||
           (m_playVideo && (info->hint.stream_info->type == VIDEO_FMT_STREAM_VIDEO)))
        {
          if(_pdcfAtom == NULL)
          {
            _pdcfAtom = QTV_New( PdcfAtom );
          }
          if(!_pdcfAtom->saveAtom(atom_type, atom_offset_type,
                                          info->hint.stream_info->track_id,
                                          pAtomInfo->size, pAtomInfo->offset)
            )
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_HINT failed to store the pdcfAtom");
          }
     }
      }
#endif /* FEATURE_QTV_PDCF */

               if( pAtomInfo->type == FTYP_TYPE || pAtomInfo->type == CPRT_TYPE ||
                    pAtomInfo->type == AUTH_TYPE || pAtomInfo->type == TITL_TYPE ||
                    pAtomInfo->type == DSCP_TYPE || pAtomInfo->type == DCMD_DRM_ATOM||
                    pAtomInfo->type == RTNG_TYPE || pAtomInfo->type == GNRE_TYPE||
                    pAtomInfo->type == PERF_TYPE || pAtomInfo->type == CLSF_TYPE||
                    pAtomInfo->type == KYWD_TYPE || pAtomInfo->type == LOCI_TYPE||
                    pAtomInfo->type == META_TYPE || pAtomInfo->type == UDTA_TYPE||
                                pAtomInfo->type == ALBM_TYPE || pAtomInfo->type == YRRC_TYPE
                    )
               {
                    int32 saveFilePos = OSCL_FileTell (localParseFilePtr);
                    /* for child videoFMT offset is after atom size and type, but
                    current parsing needs atom header as well, so goback 8 bytes */
                    if (pAtomInfo->offset >= DEFAULT_UDTA_CHILD_ATOM_HEADER_SIZE)
                         (void)OSCL_FileSeek(localParseFilePtr,
                         (int32)(pAtomInfo->offset-DEFAULT_UDTA_CHILD_ATOM_HEADER_SIZE),
                         SEEK_SET);

                    switch(pAtomInfo->type)
                    {
                    case UDTA_TYPE:
                         m_bUdtaAtomPresent=true;
                         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "3GPP Udta atom present..");
                         break;
                    case FTYP_TYPE: /* 'ftyp' */
                         if(_ftypAtom)
                              QTV_Delete( _ftypAtom );
                         _ftypAtom = QTV_New_Args( FtypAtom, (localParseFilePtr) );
                                 if(_ftypAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "FileType Atom Size = %d", _ftypAtom->getFtypDataSize());
                         }
                         break;
                    case CPRT_TYPE: /* 'cprt' */
                         if(_cprtAtom)
                              QTV_Delete( _cprtAtom );
                         _cprtAtom = QTV_New_Args( UdtaCprtAtom, (localParseFilePtr) );
                                if(_cprtAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "CopyRight Atom Size = %d", _cprtAtom->getUdtaCprtDataSize());
                         }
                         break;
                    case AUTH_TYPE: /* 'auth' */
                         if(_authAtom)
                              QTV_Delete( _authAtom );
                         _authAtom = QTV_New_Args( UdtaAuthAtom, (localParseFilePtr) );
                                  if(_authAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Author Atom Size = %d", _authAtom->getUdtaAuthDataSize());
                         }
                         break;
                    case TITL_TYPE: /* 'titl' */
                         if(_titlAtom)
                              QTV_Delete( _titlAtom );
                         _titlAtom = QTV_New_Args( UdtaTitlAtom, (localParseFilePtr) );
                                 if(_titlAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Title Atom Size = %d", _titlAtom->getUdtaTitlDataSize());
                         }
                         break;
                    case DSCP_TYPE: /* 'dscp' */
                         if(_dscpAtom)
                              QTV_Delete( _dscpAtom );
                         _dscpAtom = QTV_New_Args( UdtaDscpAtom, (localParseFilePtr) );
                                if(_dscpAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Description Atom Size = %d", _dscpAtom->getUdtaDscpDataSize());
                         }
                         break;
                    case  DCMD_DRM_ATOM: /* 'dcmd' */
                         if(_dcmdAtom)
                              QTV_Delete( _dcmdAtom );
                         _dcmdAtom = QTV_New_Args( DcmdDrmAtom, (localParseFilePtr) );
                                if(_dcmdAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "DCMD DRM Atom Size = %d", _dcmdAtom->getDcmdDataSize());
                         }
                         break;
                    case  RTNG_TYPE:
                         if(_rtngAtom)
                              QTV_Delete( _rtngAtom );
                         _rtngAtom= QTV_New_Args( UdtaRtngAtom, (localParseFilePtr) );
                         if(_rtngAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Rating Atom Size = %d", _rtngAtom->getUdtaRtngDataSize());
                         }
                         break;
                    case  GNRE_TYPE:
                         if(_gnreAtom)
                              QTV_Delete( _gnreAtom );
                         _gnreAtom= QTV_New_Args( UdtaGnreAtom, (localParseFilePtr) );
                    if(_gnreAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Genre Atom Size = %d", _gnreAtom->getUdtaGnreDataSize());
                         }
                         break;
                    case  PERF_TYPE:
                         if(_perfAtom)
                              QTV_Delete( _perfAtom );
                         _perfAtom= QTV_New_Args( UdtaPerfAtom, (localParseFilePtr) );
                    if(_perfAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Performance Atom Size = %d", _perfAtom->getUdtaPerfDataSize());
                         }
                         break;
                    case  CLSF_TYPE:
                         if(_clsfAtom)
                              QTV_Delete( _clsfAtom );
                         _clsfAtom= QTV_New_Args( UdtaClsfAtom, (localParseFilePtr) );
                  if(_clsfAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Classification Atom Size = %d", _clsfAtom->getUdtaClsfDataSize());
                         }
                         break;
                    case  KYWD_TYPE:
                         if(_kywdAtom)
                              QTV_Delete( _kywdAtom );
                         _kywdAtom= QTV_New_Args( UdtaKywdAtom, (localParseFilePtr) );
                           if(_kywdAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Keyword Atom Size = %d", _kywdAtom->getUdtaKywdDataSize());
                         }
                         break;
                    case  LOCI_TYPE:
                         if(_lociAtom)
                              QTV_Delete( _lociAtom );
                         _lociAtom= QTV_New_Args( UdtaLociAtom, (localParseFilePtr) );
                    if(_lociAtom != NULL)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Location Atom Size = %d", _lociAtom->getUdtaLociDataSize());
                         }
                         break;
                    case  META_TYPE:
                         if(_metaAtom)
                              QTV_Delete( _metaAtom );
                         _metaAtom= QTV_New_Args( UdtaMetaAtom, (localParseFilePtr) );
                         if(_metaAtom)
                         {
                              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Meta Atom Size = %d", _metaAtom->getUdtaMetaDataSize());
                         }
                         break;
                               case ALBM_TYPE: /* 'albm' */
                                       if(_albumAtom)
                                         QTV_Delete( _albumAtom );
                                         _albumAtom = QTV_New_Args( UdtaAlbumAtom, (localParseFilePtr) );
                                          if(_albumAtom != NULL)
                                            {
                                                 QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Album Atom Size = %d", _albumAtom->getUdtaAlbumDataSize());
                                            }
                                           break;
                              case YRRC_TYPE: /* 'yrrc' */
                                     if(_yrrcAtom)
                                        QTV_Delete( _yrrcAtom );
                                        _yrrcAtom = QTV_New_Args( UdtaYrrcAtom, (localParseFilePtr) );
                                    if(_yrrcAtom != NULL)
                                      {
                                          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Yrrc Atom Size = %d", _yrrcAtom->getUdtaYrrcDataSize());
                                      }
                                      break;
                    default:
                         break;
                    }
                    (void)OSCL_FileSeek(localParseFilePtr, saveFilePos, SEEK_SET);
               }
               //QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_HINT");
               break;
          }

  case VIDEO_FMT_FAILURE:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_FAILURE");
    if(!m_parsedEndofFile)
      m_parsedEndofFile = true;
    m_corruptFile = true;
    break;

  case VIDEO_FMT_BUSY:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_BUSY");
    break;

  case VIDEO_FMT_INIT:
    m_trackCount = 0;
    memcpy ( &m_videoFmtInfo, (void *)&info->info, sizeof(video_fmt_info_type) );
    // Save stream reading callback.
    m_mp4ParseServerData = info->info.server_data;
    break;

  case VIDEO_FMT_DATA_CORRUPT:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_DATA_CORRUPT");
    if(!m_parsedEndofFile)
      m_parsedEndofFile = true;
    m_corruptFile = true;
    break;

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

  case VIDEO_FMT_FRAGMENT_SIZE:
    memcpy ( &m_videoFmtInfo, (void *)&info->info, sizeof(video_fmt_info_type) );
    // Save stream reading callback.
    m_mp4ParseServerData = info->info.server_data;
    break;
  case VIDEO_FMT_FRAGMENT_PEEK:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "VIDEO_FMT_FRAGMENT_PEEK");
    break;

  case VIDEO_FMT_DATA_INCOMPLETE:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "VIDEO_FMT_DATA_INCOMPLETE");
    break;
#endif //FEATURE_QTV_PSEUDO_STREAM

  case VIDEO_FMT_STATUS_INVALID:
  default:
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid status = %d", status);
    break;
  }
}

/* ======================================================================
FUNCTION
Mpeg4File::mp4ParseStatusCallback

DESCRIPTION
This function is registered with the video format services as the status
callback function.  It receives any output data coming from that module.

IN video_fmt_status_type
The current status of the video format services.

IN void *client_data
This is actually a pointer to the Mpeg4File object which is the
client of the video format services.

INOUT video_fmt_status_cb_info_type *info
This points to a union containing information relating to the current
video format services status.  Depending on the status, the video
format services may require the callback function perform some
function with this data, and possibly modify the data.

IN video_fmt_end_cb_func_type end
This points to the function to be called when the current session of
the video format services should be closed.

DEPENDENCIES
List any dependencies for this function, global variables, state,
resource availability, etc.

RETURN VALUE
Enumerate possible return values

SIDE EFFECTS
Detail any side effects.

========================================================================== */
void Mpeg4File::mp4ParseStatusCallback (video_fmt_status_type status,
                                        void *client_data,
                                        video_fmt_status_cb_info_type *info,
                                        video_fmt_end_cb_func_type end)
{
  Mpeg4File * Mp4FilePtr = (Mpeg4File *) (client_data);
  Mp4FilePtr->mp4ParseStatus (status, info, end);
}

/* ======================================================================
FUNCTION
Mpeg4File::mp4ReadStatus

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
void Mpeg4File::mp4ReadStatus (uint32 streamNum,
                               video_fmt_status_type status,
                               video_fmt_status_cb_info_type *info,
                               video_fmt_end_cb_func_type)
{
  // Store the status code for later inspection.
  m_mp4ReadLastStatus[streamNum] = status;

  // Store session end callback function.

  // Branch according to the status code received.
  switch (status)
  {
  case VIDEO_FMT_ALLOC:
    info->alloc.ptr = (char*)QTV_Malloc(info->alloc.size);
    if ( info->alloc.ptr == NULL )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_ALLOC failed to allocate memory");
    }
    else
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_ALLOC");
    break;

  case VIDEO_FMT_FREE:
    QTV_Free(info->free.ptr);
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_FREE");
    break;

  case VIDEO_FMT_GET_DATA:
    {

      /*This flag will be set by the UI task if the user terminates the playback*/
        if(m_pMpeg4Player && m_pMpeg4Player->m_bMediaAbort)
        {
          info->get_data.num_bytes = 0;
          QTV_MSG (MSG_LEGACY_HIGH, "Failed to readFile (mp4ReadStatus): User aborted playback..!!");
            _fileErrorCode = (int32)READ_FAILED;
          break;
        }
      /* if file pointer for this stream is already not opened, then open it */
      if(m_streamFilePtr[streamNum]==NULL)
      {
#ifdef FEATURE_QTV_DRM_DCF
        if (m_inputStream != NULL)
        {
           m_streamFilePtr[streamNum] = OSCL_FileOpen(m_inputStream);
        }
        else
#endif
        if(m_pFileBuf)
        {
          m_streamFilePtr[streamNum] = OSCL_FileOpen(m_pFileBuf, m_fileSize);
        }
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        else if(m_fpFetchBufferedDataSize)
        {
          m_streamFilePtr[streamNum] = OSCL_FileOpen(NULL, 0,
            m_fpFetchBufferedData,
            getTrackIdFromStreamNum(streamNum), m_QtvInstancehandle);
        }
#endif //defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

        else
        {
          m_streamFilePtr[streamNum] = OSCL_FileOpen(m_filename, (OSCL_TCHAR *) _T("rb"));
        }
      }
      // Read the given number of bytes from the given offset in the
      // file.
      info->get_data.num_bytes
        = MIN (info->get_data.num_bytes,
        (m_fileSize - MIN (m_fileSize, info->get_data.offset)));

      if ( info->get_data.num_bytes )
      {
        info->get_data.num_bytes = readFile (m_streamFilePtr[streamNum],
          info->get_data.buffer,
          info->get_data.offset,
          info->get_data.num_bytes);
        if(info->get_data.num_bytes)
        {
          m_mp4ReadContinueCb[streamNum] = info->get_data.callback_ptr;
          m_mp4ReadServerData[streamNum] = info->get_data.server_data;
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failed to readFile");
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
          if(!bHttpStreaming)
#endif
          {
            //This should be done for local playback as
                  //for PD,PS, it is possible to have 0 bytes being read.
            //Set the error code to let player know the exact failure.
            //This will help to stop/abort the playback.
            _fileErrorCode = (int32)READ_FAILED;
          }
          }
      }
    }
    break;

  case VIDEO_FMT_PUT_DATA:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_PUT_DATA");
    break;

  case VIDEO_FMT_CONTINUE:
    {
      m_mp4ReadContinueCb[streamNum] = info->cont.callback_ptr;
      m_mp4ReadServerData[streamNum] = info->cont.server_data;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_CONTINUE");
    }
    break;

  case VIDEO_FMT_IO_DONE:
    {
      m_iodoneSize[streamNum] = info->io_done.bytes;
    }
    break;

  case VIDEO_FMT_DONE:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_DONE");
    break;

  case VIDEO_FMT_HINT:
    // QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_HINT");
    break;

  case VIDEO_FMT_FAILURE:
    {
      if(m_videoFmtInfo.streams[streamNum].type == VIDEO_FMT_STREAM_VIDEO)
      {
        m_hasVideo = false;
      }
      else if(m_videoFmtInfo.streams[streamNum].type == VIDEO_FMT_STREAM_AUDIO)
      {
        m_hasAudio = false;
      }
      else if(m_videoFmtInfo.streams[streamNum].type == VIDEO_FMT_STREAM_TEXT)
      {
        m_hasText = false;
      }
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_FAILURE");
      break;
    }

  case VIDEO_FMT_BUSY:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_BUSY");
    break;

#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD ||defined FEATURE_QTV_PSEUDO_STREAM
  case VIDEO_FMT_DATA_INCOMPLETE:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_DATA_INCOMPLETE");
    break;
  case VIDEO_FMT_ABS_FILE_OFFSET:
    m_absFileOffset[streamNum] = info->info.abs_file_offset;
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_ABS_FILE_OFFSET");
    break;
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

  case VIDEO_FMT_STATUS_INVALID:
  default:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_STATUS_INVALID");
    break;
  }
}

/* ======================================================================
FUNCTION
Mpeg4File::mp4ReadStatusCallback

DESCRIPTION
This function is registered with the video format services as the status
callback function when reading stream data.  It receives any output data
coming from that module.

IN video_fmt_status_type
The current status of the video format services.

IN void *client_data
This is actually a pointer to the CMP4parseDoc object which is the
client of the video format services.

INOUT video_fmt_status_cb_info_type *info
This points to a union containing information relating to the current
video format services status.  Depending on the status, the video
format services may require the callback function perform some
function with this data, and possibly modify the data.

IN video_fmt_end_cb_func_type end
This points to the function to be called when the current session of
the video format services should be closed.

DEPENDENCIES
List any dependencies for this function, global variables, state,
resource availability, etc.

RETURN VALUE
Enumerate possible return values

SIDE EFFECTS
Detail any side effects.

========================================================================== */
void Mpeg4File::mp4ReadStatusCallback (video_fmt_status_type status,
                                       void *client_data,
                                       video_fmt_status_cb_info_type *info,
                                       video_fmt_end_cb_func_type end)
{
  videoFMTClientData * clientData = (videoFMTClientData *) (client_data);
  clientData->Mp4FilePtr->mp4ReadStatus ((uint32)(clientData->streamNum), status, info, end);
}

/* ======================================================================
FUNCTION
Mpeg4File::mp4SyncStatus

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
void Mpeg4File::mp4SyncStatus (uint32 streamNum,
                               video_fmt_status_type status,
                               video_fmt_status_cb_info_type *info,
                               video_fmt_end_cb_func_type)
{
  // Store the status code for later inspection.
  m_mp4SyncLastStatus[streamNum] = status;

  // Store session end callback function.

  // Branch according to the status code received.
  switch (status)
  {

  case VIDEO_FMT_GET_DATA:
      /*This flag will be set by the UI task if the user terminates the playback*/
      if(m_pMpeg4Player && m_pMpeg4Player->m_bMediaAbort)
      {
        info->get_data.num_bytes = 0;
        QTV_MSG (MSG_LEGACY_HIGH, "Failed to readFile (mp4SyncStatus): User aborted playback..!!");
          _fileErrorCode = (int32)READ_FAILED;
        break;
      }

    /* if file pointer for this stream is already not opened, then open it */
    if(m_streamFilePtr[streamNum]==NULL)
    {
      if(m_pFileBuf)
      {
        m_streamFilePtr[streamNum] = OSCL_FileOpen(m_pFileBuf, m_fileSize);
      }
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      else if(m_fpFetchBufferedDataSize)
      {
        m_streamFilePtr[streamNum] = OSCL_FileOpen(NULL, 0,
          m_fpFetchBufferedData,
          getTrackIdFromStreamNum(streamNum),m_QtvInstancehandle);
      }
#endif //defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      else
      {
        m_streamFilePtr[streamNum] = OSCL_FileOpen(m_filename, (OSCL_TCHAR *) _T("rb"));
      }
    }
    // Read the given number of bytes from the given offset in the
    // file.
    info->get_data.num_bytes
      = MIN (info->get_data.num_bytes,
      (m_fileSize - MIN (m_fileSize, info->get_data.offset)));
    if ( info->get_data.num_bytes )
    {
      info->get_data.num_bytes = readFile (m_streamFilePtr[streamNum],
        info->get_data.buffer,
        info->get_data.offset,
        info->get_data.num_bytes);
      if (info->get_data.num_bytes)
      {
        m_mp4SyncContinueCb[streamNum] = info->get_data.callback_ptr;
        m_mp4SyncServerData[streamNum] = info->get_data.server_data;
      }
      else
       {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failed to readFile");
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
          if(!bHttpStreaming)
#endif
          {
            //This should be done for local playback as
                  //for PD,PS, it is possible to have 0 bytes being read.
            //Set the error code to let player know the exact failure.
            //This will help to stop/abort the playback.
            _fileErrorCode = (int32)READ_FAILED;
          }
       }
    }
    break;

  case VIDEO_FMT_CONTINUE:
    {
      m_mp4SyncContinueCb[streamNum] = info->cont.callback_ptr;
      m_mp4SyncServerData[streamNum] = info->cont.server_data;
    }
    break;

  case VIDEO_FMT_IO_DONE:
    {
      m_iodoneSize[streamNum] = info->io_done.bytes;
    }
    break;

  case VIDEO_FMT_DONE:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "VIDEO_FMT_DONE");
    break;

  case VIDEO_FMT_HINT:
    // QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_HINT");
    break;

  case VIDEO_FMT_FAILURE:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_DONE");
    break;

  case VIDEO_FMT_BUSY:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_BUSY");
    break;

  case VIDEO_FMT_STATUS_INVALID:
  default:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_FMT_STATUS_INVALID");
    break;
  }
}

/* ======================================================================
FUNCTION
Mpeg4File::mp4SyncStatusCallback

DESCRIPTION
This function is registered with the video format services as the status
callback function when reading stream data.  It receives any output data
coming from that module.

IN video_fmt_status_type
The current status of the video format services.

IN void *client_data
This is actually a pointer to the CMP4parseDoc object which is the
client of the video format services.

INOUT video_fmt_status_cb_info_type *info
This points to a union containing information relating to the current
video format services status.  Depending on the status, the video
format services may require the callback function perform some
function with this data, and possibly modify the data.

IN video_fmt_end_cb_func_type end
This points to the function to be called when the current session of
the video format services should be closed.

DEPENDENCIES
List any dependencies for this function, global variables, state,
resource availability, etc.

RETURN VALUE
Enumerate possible return values

SIDE EFFECTS
Detail any side effects.

========================================================================== */
void Mpeg4File::mp4SyncStatusCallback (video_fmt_status_type status,
                                       void *client_data,
                                       video_fmt_status_cb_info_type *info,
                                       video_fmt_end_cb_func_type end)
{
  videoFMTClientData * clientData = (videoFMTClientData *) (client_data);
  clientData->Mp4FilePtr->mp4SyncStatus ((uint32)(clientData->streamNum), status, info, end);
}

#ifdef FEATURE_H264_DECODER
/* ======================================================================
FUNCTION
Mpeg4File::getNumPicParamSet

DESCRIPTION
returns number of H264 picture parameter sets in the clip.

DEPENDENCIES
List any dependencies for this function, global variables, state,
resource availability, etc.

RETURN VALUE
number of H264 picture parameter sets.

SIDE EFFECTS
Detail any side effects.

========================================================================== */
uint32 Mpeg4File::getNumPicParamSet(uint32 trackId)
{
  uint32 numParam = 0;
  video_fmt_stream_info_type *pInfo = getTrackInfoForID(trackId);

  if(pInfo && (pInfo->type==VIDEO_FMT_STREAM_VIDEO) && (pInfo->subinfo.video.format==VIDEO_FMT_STREAM_VIDEO_H264) )
  {
    numParam = pInfo->dec_specific_info.h264_info.num_pic_param;
  }
  return numParam;
}

/* ======================================================================
FUNCTION
Mpeg4File::getNumSeqParamSet

DESCRIPTION
returns number of H264 sequence parameter sets in the clip.

DEPENDENCIES
List any dependencies for this function, global variables, state,
resource availability, etc.

RETURN VALUE
number of H264 picture parameter sets.

SIDE EFFECTS
Detail any side effects.

========================================================================== */
uint32 Mpeg4File::getNumSeqParamSet(uint32 trackId)
{
  uint32 numParam = 0;
  video_fmt_stream_info_type *pInfo = getTrackInfoForID(trackId);

  if(pInfo && (pInfo->type==VIDEO_FMT_STREAM_VIDEO) && (pInfo->subinfo.video.format==VIDEO_FMT_STREAM_VIDEO_H264) )
  {
    numParam = pInfo->dec_specific_info.h264_info.num_seq_param;
  }
  return numParam;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  getNextParamSetNAL

DESCRIPTION
Public method used to request next parameter set.

DESCRIPTION
copies pic parameter set NAL in given buffer.

PARAMETERS:
idx     INPUT   index of video track.
buf     OUTPUT  buffer for data to be copied.
size    INPUT   size of buffer.

DEPENDENCIES
First it will return all Sequence parameters, then picture parameters.

RETURN VALUE
size of parameter set NAL
zero if no more parameters to be sent.

SIDE EFFECTS
First it will return all Sequence parameters, then picture parameters.
ASSERTS if buffer size is smaller than size of NAL.

===========================================================================*/
int32 Mpeg4File::getNextParamSetNAL(uint32 trackId, uint8 *buf, uint32 size)
{
  int32  nBytes = 0;
  video_fmt_stream_info_type *pInfo = getTrackInfoForID(trackId);

  if(pInfo && (pInfo->type==VIDEO_FMT_STREAM_VIDEO) && (pInfo->subinfo.video.format==VIDEO_FMT_STREAM_VIDEO_H264) )
  {
    video_fmt_h264_dec_info_type *h264Info = &pInfo->dec_specific_info.h264_info;

    /* first send all sequence parameters */
    if(m_nextSeqSample < h264Info->num_seq_param)
    {
      nBytes = h264Info->seq_param_set[m_nextSeqSample].len;

      if (!(nBytes < (int32)size))
      {
         nBytes = FRAGMENT_CORRUPT;
      return nBytes;
      }
      memcpy(buf, h264Info->seq_param_set[m_nextSeqSample].data, (uint32)nBytes);
      m_nextSeqSample++;
      return nBytes ;
    }
    /* then send all picture parameters */
    else if(m_nextPicSample < h264Info->num_pic_param)
    {
      nBytes = h264Info->pic_param_set[m_nextPicSample].len;

      if (!(nBytes < (int32)size))
      {
     nBytes = FRAGMENT_CORRUPT;
     return nBytes;
      }
      memcpy(buf, h264Info->pic_param_set[m_nextPicSample].data, (uint32)nBytes);
      m_nextPicSample++;
      return nBytes;
    }
  }
  return nBytes;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  resetParamSetNAL

DESCRIPTION
Public method used to reset parameter set counters.

PARAMETERS:
idx     INPUT   index of video track.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void Mpeg4File::resetParamSetNAL(uint32)
{
  m_nextSeqSample = 0;
  m_nextPicSample = 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  GetSizeOfNALLengthField

DESCRIPTION
Public method used to obtain the size of NAL length field before NAL in
video frames.

PARAMETERS:
idx     INPUT   index of video track.

DEPENDENCIES

RETURN VALUE
size of NAL length field before NAL in video frames in bytes.

SIDE EFFECTS

===========================================================================*/
int Mpeg4File::GetSizeOfNALLengthField(uint32 trackId)
{
  int32  nBytes = 0;
  video_fmt_stream_info_type *pInfo = getTrackInfoForID(trackId);

  if(pInfo && (pInfo->type==VIDEO_FMT_STREAM_VIDEO) && (pInfo->subinfo.video.format==VIDEO_FMT_STREAM_VIDEO_H264) )
  {
    nBytes = pInfo->dec_specific_info.h264_info.len_minus_one + 1;
  }
  return nBytes;
}

#endif /* FEATURE_H264_DECODER */

#ifdef FEATURE_QTV_SKT_MOD_MIDI
/* ======================================================================
FUNCTION:
Mpeg4File::IsMidiDataPresent

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
bool Mpeg4File::IsMidiDataPresent()
{
  if ( _midiAtom )
  {
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION:
Mpeg4File::GetMidiDataSize

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
uint32 Mpeg4File::GetMidiDataSize()
{
  if(_midiAtom)
  {
    return _midiAtom->getUdtaMidiDataSize();
  }
  return 0;
}

/* ======================================================================
FUNCTION:
Mpeg4File::GetMidiData

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
uint32 Mpeg4File::GetMidiData(uint8 *pBuf, uint32 size, uint32 offset)
{
  if(_midiAtom)
  {
    return _midiAtom->getUdtaMidiData(pBuf, size, offset);
  }
  return 0;
}

/* ======================================================================
FUNCTION:
Mpeg4File::IsLinkDataPresent

DESCRIPTION:
if SKT specific LINK atom is present or not.

INPUT/OUTPUT PARAMETERS:
None.

RETURN VALUE:
TRUE - if LINK atom is present
FALSE - LINK atom not present.

SIDE EFFECTS:
None.
======================================================================*/
bool Mpeg4File::IsLinkDataPresent()
{
  if ( _linkAtom )
  {
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION:
Mpeg4File::GetLinkDataSize

DESCRIPTION:
returns LINK size.

INPUT/OUTPUT PARAMETERS:
None.

RETURN VALUE:
size of LINK
0 if LINK not present.

SIDE EFFECTS:
None.
======================================================================*/
uint32 Mpeg4File::GetLinkDataSize()
{
  if(_linkAtom)
  {
    return _linkAtom->getUdtaLinkDataSize();
  }
  return 0;
}

/* ======================================================================
FUNCTION:
Mpeg4File::GetLinkData

DESCRIPTION:
copies the LINK into supplied buffer.

INPUT/OUTPUT PARAMETERS:
pBuf  - INPUT/OUTPUT  - buffer for data to be copied.
size  - INPUT         - size of buffer and max data to be copied.

RETURN VALUE:
actual bytes copied into buffer
0 if no LINK data is available

SIDE EFFECTS:
None.
======================================================================*/
uint32 Mpeg4File::GetLinkData(uint8 *pBuf, uint32 size)
{
  if(_linkAtom)
  {
    return _linkAtom->getUdtaLinkData(pBuf, size);
  }
  return 0;
}

#endif /* FEATURE_QTV_SKT_MOD_MIDI */

/* ======================================================================
FUNCTION:
Mpeg4File::IsDataPresent

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
bool Mpeg4File::IsDataPresent(QtvPlayer::DataT dType, uint32 track_id)
{
  track_id = track_id; /* This is to remove the comiler warning */
  switch(dType)
  {
  case QtvPlayer::DATA_ATOM_FTYP:       /* File Type Atom*/
    if(_ftypAtom)
    {
      return true;
    }
    return false;
  case QtvPlayer::DATA_ATOM_UDTA_CPRT:        /* Copyright Atom*/
    if ( _cprtAtom )
    {
      return true;
    }
    return false;
  case QtvPlayer::DATA_ATOM_UDTA_AUTH:  /* Author Atom*/
    if ( _authAtom )
    {
      return true;
    }
    return false;
  case QtvPlayer::DATA_ATOM_UDTA_TITL:    /* Title Atom*/
    if ( _titlAtom )
    {
      return true;
    }
    return false;
  case QtvPlayer::DATA_ATOM_UDTA_DSCP:  /* Description Atom*/
    if ( _dscpAtom )
    {
      return true;
    }
    return false;
  case QtvPlayer::DATA_ATOM_DCMD:      /* DCMD DRM Atom*/
    if ( _dcmdAtom )
    {
      return true;
    }
    return false;
  case QtvPlayer::DATA_ATOM_UDTA_RTNG:  /* Rating Atom*/
     if ( _rtngAtom )
     {
      return true;
     }
     return false;
  case QtvPlayer::DATA_ATOM_UDTA_PERF:  /* Performance Atom*/
     if ( _perfAtom )
     {
       return true;
     }
     return false;
  case QtvPlayer::DATA_ATOM_UDTA_CLSF:  /* Classification Atom*/
     if ( _clsfAtom )
     {
       return true;
     }
     return false;
  case QtvPlayer::DATA_ATOM_UDTA_KYWD:  /* Keyword Atom*/
     if ( _kywdAtom )
     {
       return true;
     }
     return false;
  case QtvPlayer::DATA_ATOM_UDTA_LOCI:  /* Location Atom*/
     if ( _lociAtom )
     {
       return true;
     }
     return false;

  case QtvPlayer::DATA_ATOM_UDTA_META:  /* Meta Atom*/
     if ( _metaAtom )
     {
       return true;
     }
     return false;
case QtvPlayer::DATA_ATOM_UDTA_ALBM:  /* Album Atom*/
    if ( _albumAtom )
    {
      return true;
    }
    return false;
  case QtvPlayer::DATA_ATOM_UDTA_YRRC:  /* Yrrc Atom*/
    if ( _yrrcAtom )
    {
      return true;
    }
    return false;

#ifdef FEATURE_QTV_SKT_MOD_MIDI
  case QtvPlayer::DATA_ATOM_UDTA_MIDI:
    if ( _midiAtom )
    {
      return true;
    }
    return false;
  case QtvPlayer::DATA_ATOM_UDTA_LINK:
    if ( _linkAtom )
    {
      return true;
    }
    return false;
#endif //FEATURE_QTV_SKT_MOD_MIDI
  case QtvPlayer::DATA_TEXT_TKHD_ORIGIN_X:    /* Text track origin_x*/
  case QtvPlayer::DATA_TEXT_TKHD_ORIGIN_Y:    /* Text track origin_y*/
  case QtvPlayer::DATA_TEXT_TKHD_WIDTH:       /* Text track width*/
  case QtvPlayer::DATA_TEXT_TKHD_HEIGHT:       /* Text track height*/
    /*Check if the text track is present*/
    for (uint32 index = 0; index < m_trackCount; index++)
    {
      if ( m_track[index]->type == VIDEO_FMT_STREAM_TEXT )
      {
        return true;
      }
    }
    return false;
#ifdef FEATURE_QTV_PDCF
  case QtvPlayer::DATA_ATOM_SCHM:
  case QtvPlayer::DATA_ATOM_FRMA:
  case QtvPlayer::DATA_ATOM_OHDR:
  case QtvPlayer::DATA_ATOM_ODAF:
  case QtvPlayer::DATA_ATOM_MDRI:
  case QtvPlayer::DATA_ATOM_SCHM_OFFSET:
  case QtvPlayer::DATA_ATOM_FRMA_OFFSET:
  case QtvPlayer::DATA_ATOM_OHDR_OFFSET:
  case QtvPlayer::DATA_ATOM_ODAF_OFFSET:
  case QtvPlayer::DATA_ATOM_MDRI_OFFSET:
    if(_pdcfAtom)
    {
      return _pdcfAtom->isAtomPresent(dType, track_id);
    }
    break;
#endif /* FEATURE_QTV_PDCF */
  default:
    break;
  }
  return false;
}

/* ======================================================================
FUNCTION:
Mpeg4File::GetDataSize

DESCRIPTION:
returns the specified data-type data size.

INPUT/OUTPUT PARAMETERS:
dType - data-type.
offset - used as a track_id if the data is in multiple tracks.

RETURN VALUE:
size of the specified data-type.
0 if specified data not present.

SIDE EFFECTS:
None.
======================================================================*/
uint32 Mpeg4File::GetDataSize(QtvPlayer::DataT dType, uint32 offset )
{
  offset = offset; //required to remove compile time warning
  switch(dType)
  {
  case QtvPlayer::DATA_ATOM_FTYP:       /* File Type Atom*/
    if(_ftypAtom)
    {
      return _ftypAtom->getFtypDataSize();
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_CPRT:        /* Copyright Atom*/
    if(_cprtAtom)
    {
      return _cprtAtom->getUdtaCprtDataSize();
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_AUTH:  /* Author Atom*/
    if(_authAtom)
    {
      return _authAtom->getUdtaAuthDataSize();
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_TITL:    /* Title Atom*/
    if(_titlAtom)
    {
      return _titlAtom->getUdtaTitlDataSize();
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_DSCP:  /* Description Atom*/
    if(_dscpAtom)
    {
      return _dscpAtom->getUdtaDscpDataSize();
    }
    return 0;
  case QtvPlayer::DATA_ATOM_DCMD:      /* DCMD DRM Atom*/
    if(_dcmdAtom)
    {
      return _dcmdAtom->getDcmdDataSize();
    }
    return 0;

  case QtvPlayer::DATA_ATOM_UDTA_RTNG:      /* Rating*/
    if(_rtngAtom)
    {
        return _rtngAtom->getUdtaRtngDataSize();
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_PERF:      /* Performance Atom*/
    if(_perfAtom)
    {
        return _perfAtom->getUdtaPerfDataSize();
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_KYWD:      /* Keyword Atom*/
    if(_kywdAtom)
    {
        return _kywdAtom->getUdtaKywdDataSize();
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_CLSF:      /* Classification Atom*/
    if(_clsfAtom)
    {
        return _clsfAtom->getUdtaClsfDataSize();
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_LOCI:      /* Location Atom*/
    if(_lociAtom)
    {
        return _lociAtom->getUdtaLociDataSize();
    }
    return 0;

  case QtvPlayer::DATA_ATOM_UDTA_META:
    if(_metaAtom)
    {
        return _metaAtom->getUdtaMetaDataSize();
    }
    return 0;

 case QtvPlayer::DATA_ATOM_UDTA_ALBM:  /* Album Atom*/
    if(_albumAtom)
    {
      return _albumAtom->getUdtaAlbumDataSize();
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_YRRC:  /* Album Atom*/
    if(_yrrcAtom)
    {
      return _yrrcAtom->getUdtaYrrcDataSize();
    }
    return 0;
#ifdef FEATURE_QTV_SKT_MOD_MIDI
  case QtvPlayer::DATA_ATOM_UDTA_MIDI:
    if(_midiAtom)
    {
      return _midiAtom->getUdtaMidiDataSize();
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_LINK:
    if(_linkAtom)
    {
      return _linkAtom->getUdtaLinkDataSize();
    }
    return 0;
#endif //FEATURE_QTV_SKT_MOD_MIDI
  case QtvPlayer::DATA_TEXT_TKHD_ORIGIN_X:    /* Text track origin_x*/
  case QtvPlayer::DATA_TEXT_TKHD_ORIGIN_Y:    /* Text track origin_y*/
  case QtvPlayer::DATA_TEXT_TKHD_WIDTH:       /* Text track width*/
  case QtvPlayer::DATA_TEXT_TKHD_HEIGHT:       /* Text track height*/
    for (uint32 index = 0; index < m_trackCount; index++)
    {
      if ( m_track[index]->type == VIDEO_FMT_STREAM_TEXT )
      {
        return sizeof(uint32);
      }
    }
    return 0;
#ifdef FEATURE_QTV_PDCF
  case QtvPlayer::DATA_ATOM_SCHM:
  case QtvPlayer::DATA_ATOM_FRMA:
  case QtvPlayer::DATA_ATOM_OHDR:
  case QtvPlayer::DATA_ATOM_ODAF:
  case QtvPlayer::DATA_ATOM_MDRI:
    if(_pdcfAtom)
    {
      return _pdcfAtom->getAtomSize(dType, offset);
    }
    break;
#endif /* FEATURE_QTV_PDCF */
  default:
    break;
  }
  return 0;
}

/* ======================================================================
FUNCTION:
Mpeg4File::GetData

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
uint32 Mpeg4File::GetData(QtvPlayer::DataT dType, uint8 *pBuf, uint32 size, uint32 offset)
{
  switch(dType)
  {
  case QtvPlayer::DATA_ATOM_FTYP:       /* File Type Atom*/
    if(_ftypAtom)
    {
      return _ftypAtom->getFtypData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_CPRT:        /* Copyright Atom*/
    if(_cprtAtom)
    {
      return _cprtAtom->getUdtaCprtData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_AUTH:  /* Author Atom*/
    if(_authAtom)
    {
      return _authAtom->getUdtaAuthData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_TITL:    /* Title Atom*/
    if(_titlAtom)
    {
      return _titlAtom->getUdtaTitlData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_DSCP:  /* Description Atom*/
    if(_dscpAtom)
    {
      return _dscpAtom->getUdtaDscpData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_DCMD:      /* DCMD DRM Atom*/
    if(_dcmdAtom)
    {
      return _dcmdAtom->getDcmdData(pBuf, size, offset);
    }
    return 0;

  case QtvPlayer::DATA_ATOM_UDTA_RTNG:  /* Rating Atom*/
    if(_rtngAtom)
    {
        return _rtngAtom->getUdtaRtngData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_PERF:  /* Performance Atom*/
    if(_perfAtom)
    {
        return _perfAtom->getUdtaPerfData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_CLSF:  /* Classification Atom*/
    if(_clsfAtom)
    {
        return _clsfAtom->getUdtaClsfData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_KYWD:  /* Keyword Atom*/
    if(_kywdAtom)
    {
        return _kywdAtom->getUdtaKywdData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_LOCI:  /* Location Atom*/
    if(_lociAtom)
    {
        return _lociAtom->getUdtaLociData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_META:  /* Meta Atom*/
    if(_metaAtom)
    {
        return _metaAtom->getUdtaMetaData(pBuf, size, offset);
    }
    return 0;
    case QtvPlayer::DATA_ATOM_UDTA_ALBM:  /* Author Atom*/
    if(_albumAtom)
    {
      return _albumAtom->getUdtaAlbumData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_YRRC:  /* Yrrc Atom*/
    if(_yrrcAtom)
    {
      return _yrrcAtom->getUdtaYrrcData(pBuf, size, offset);
    }
    return 0;
#ifdef FEATURE_QTV_SKT_MOD_MIDI
  case QtvPlayer::DATA_ATOM_UDTA_MIDI:
    if(_midiAtom)
    {
      return _midiAtom->getUdtaMidiData(pBuf, size, offset);
    }
    return 0;
  case QtvPlayer::DATA_ATOM_UDTA_LINK:
    if(_linkAtom)
    {
      return _linkAtom->getUdtaLinkData(pBuf, size);
    }
    return 0;
#endif //FEATURE_QTV_SKT_MOD_MIDI

  case QtvPlayer::DATA_TEXT_TKHD_ORIGIN_X:    /* Text track origin_x*/
    if(size >= sizeof(uint32))
    {
      for (uint32 index = 0; index < m_trackCount; index++)
      {
        if ( m_track[index]->type == VIDEO_FMT_STREAM_TEXT )
        {
          *((uint32 *)pBuf) = m_track[index]->tkhd_origin_x >> 16;
          return sizeof(uint32);
        }
      }
    }
    return 0;

  case QtvPlayer::DATA_TEXT_TKHD_ORIGIN_Y:    /* Text track origin_y*/
    if(size >= sizeof(uint32))
    {
      for (uint32 index = 0; index < m_trackCount; index++)
      {
        if ( m_track[index]->type == VIDEO_FMT_STREAM_TEXT )
        {
          *((uint32 *)pBuf) = m_track[index]->tkhd_origin_y >> 16;
          return sizeof(uint32);
        }
      }
    }
    return 0;

  case QtvPlayer::DATA_TEXT_TKHD_WIDTH:       /* Text track width*/
    if(size >= sizeof(uint32))
    {
      for (uint32 index = 0; index < m_trackCount; index++)
      {
        if ( m_track[index]->type == VIDEO_FMT_STREAM_TEXT )
        {
          *((uint32 *)pBuf) = m_track[index]->tkhd_width >> 16;
          return sizeof(uint32);
        }
      }
    }
    return 0;

  case QtvPlayer::DATA_TEXT_TKHD_HEIGHT:       /* Text track height*/
    if(size >= sizeof(uint32))
    {
      for (uint32 index = 0; index < m_trackCount; index++)
      {
        if ( m_track[index]->type == VIDEO_FMT_STREAM_TEXT )
        {
          *((uint32 *)pBuf) = m_track[index]->tkhd_height >> 16;
          return sizeof(uint32);
        }
      }
    }
    return 0;

case QtvPlayer::DATA_ATOM_UUID:
  {
    if(m_parseFilePtr)
    {
      int32 saveFilePos = OSCL_FileTell (m_parseFilePtr);
      uint32 copiedSize = 0;
      Atom *UUIDatom = NULL;
      uint32 length = this->UUIDatomEntryArray.GetLength();
      if( length >= offset )
        UUIDatom = UUIDatomEntryArray[offset];
      else
        UUIDatom = NULL;

      if (UUIDatom != NULL)
      {
        OSCL_FileSeek(m_parseFilePtr, UUIDatom->getOffsetInFile(), SEEK_SET);
        uint32 uuidatomSize = UUIDatom->getSize();
        copiedSize = MIN(size, uuidatomSize);
        if ( OSCL_FileRead (pBuf, copiedSize,
                          1,  m_parseFilePtr) == 0 )
        {
          QTV_MSG (MSG_LEGACY_ERROR, "Failed to readFile");
          copiedSize = 0;
        }
      }
      (void)OSCL_FileSeek(m_parseFilePtr, saveFilePos, SEEK_SET);
      return copiedSize;
    }
    else
    {
      QTV_MSG (MSG_LEGACY_ERROR, "Failed to find open file pointer");
    }
    break;
  }

  case QtvPlayer::DATA_ATOM_TX3G:
  {
    if(m_parseFilePtr)
    {
      int32 saveFilePos = OSCL_FileTell (m_parseFilePtr);
      uint32 trackid = 0;
      for (uint32 index = 0; index < m_trackCount; index++)
      {
        if ( m_track[index]->type == VIDEO_FMT_STREAM_TEXT )
          trackid = m_track[index]->track_id;
      }
      TextSampleEntry *textAtom = this->getTextSampleEntryAt(trackid, (offset+1));
      uint32 copiedSize = 0;
      if (textAtom != NULL)
      {
        OSCL_FileSeek(m_parseFilePtr,
          textAtom->getOffsetInFile() +  DEFAULT_TX3G_ATOM_HEADER_SIZE, SEEK_SET);
        uint32 uuidatomSize = textAtom->getSize()-DEFAULT_TX3G_ATOM_HEADER_SIZE;
        copiedSize = MIN(size, uuidatomSize);
        if ( OSCL_FileRead (pBuf, copiedSize,
                          1,  m_parseFilePtr) == 0 )
        {
          QTV_MSG (MSG_LEGACY_ERROR, "Failed to readFile");
          copiedSize = 0;
        }
      }
      (void)OSCL_FileSeek(m_parseFilePtr, saveFilePos, SEEK_SET);
      return copiedSize;
    }
    else
    {
      QTV_MSG (MSG_LEGACY_ERROR, "Failed to find open file pointer");
    }
    break;
  }
#ifdef FEATURE_QTV_PDCF
  case QtvPlayer::DATA_ATOM_SCHM:
  case QtvPlayer::DATA_ATOM_FRMA:
  case QtvPlayer::DATA_ATOM_OHDR:
  case QtvPlayer::DATA_ATOM_ODAF:
  case QtvPlayer::DATA_ATOM_MDRI:
    if(_pdcfAtom)
    {
      if( m_parseFilePtr == NULL )
      {
        m_parseFilePtr = OSCL_FileOpen(m_filename, (OSCL_TCHAR *) _T("rb"));
      }
      if(m_parseFilePtr)
      {
        uint32 copiedSize = 0;
        copiedSize = _pdcfAtom->getAtomData(m_parseFilePtr, dType, pBuf, size, offset);
        OSCL_FileClose(m_parseFilePtr);
        m_parseFilePtr = NULL;
        return copiedSize;
      }
      else
      {
        QTV_MSG (MSG_LEGACY_ERROR, "Failed to open file");
      }
    }
    break;
  case QtvPlayer::DATA_ATOM_SCHM_OFFSET:
  case QtvPlayer::DATA_ATOM_FRMA_OFFSET:
  case QtvPlayer::DATA_ATOM_OHDR_OFFSET:
  case QtvPlayer::DATA_ATOM_ODAF_OFFSET:
  case QtvPlayer::DATA_ATOM_MDRI_OFFSET:
    if(_pdcfAtom)
    {
      return _pdcfAtom->getAtomOffset(dType,pBuf, size, offset);
    }
    break;
#endif /* FEATURE_QTV_PDCF */
  default:
    break;
  }
  return 0;
}


#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
void Mpeg4File::process_kddi_telop_text()
{
  if (_kddiTelopElement)
  {
    uint8* telopString = _kddiTelopElement->getTelopInformationString();

    uint32 telopStringSize = _kddiTelopElement->getTelopInformationSize();

    int32 telopParseResult = true;

    _pTsmlAtom = TsmlAtom::ParseTelopElementText( (char*)telopString,
      (int32)telopStringSize,
      (int32)getMovieDurationMsec(),
      &telopParseResult);
    if (telopParseResult != SUCCESSFUL)
    {
      /* for telop parsing error, we only drop telop playback.
      other tracks (audio/video) can still play */
      if(_pTsmlAtom != NULL)
      {
        QTV_Delete( _pTsmlAtom );
        _pTsmlAtom = NULL;
      }
      if (_kddiTelopElement != NULL)
      {
        QTV_Delete( _kddiTelopElement );
        _kddiTelopElement = NULL;
      }
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Telop Atom Parsing Failed." );
    }
    else
    {
      if(_pTsmlAtom)
        {
           _pTsmlAtom->ResetTelopVectorIndex();
        }
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "_kddiTelopElement pointer invalid");
  }
}
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

#if defined(FEATURE_QTV_SKT_MOD_MIDI)
void Mpeg4File::process_mod_midi_atom()
{
  if( _midiAtom && (_midiAtom->getUdtaMidiDataSize()==0) )
  {
    QTV_Delete( _midiAtom );
    _midiAtom = NULL;
  }
  if( _linkAtom && (_linkAtom->getUdtaLinkDataSize()==0) )
  {
    QTV_Delete( _linkAtom );
    _linkAtom = NULL;
  }
}
#endif /* FEATURE_QTV_SKT_MOD_MIDI */


bool   Mpeg4File::isADTSHeader()
{
   if(aac_data_type == AAC_FORMAT_ADTS)
     return true;
   if(aac_data_type == AAC_FORMAT_NON_ADTS)
     return false;

   uint8 byte[3];
   video_fmt_stream_info_type *p_track = NULL;
   video_fmt_sample_info_type sample_info;
   for (uint32 index = 0; index < m_trackCount; index++)
   {
    if (m_track[index]->type == VIDEO_FMT_STREAM_AUDIO &&
            m_track[index]->subinfo.audio.format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
    {
      p_track = m_track[index];
    }
   }
   if(p_track == NULL)
      return false;

   getSampleInfo (p_track->stream_num,
                  1,
                  1,
                  &sample_info);

   getSample (p_track->stream_num,
      VIDEO_FMT_DATA_UNIT_BYTE,
      sample_info.offset,
      2,
      byte);

   if((byte[0] == 0xFF) && (byte[1]&0xF0 == 0xF0))
   {
      aac_data_type = AAC_FORMAT_ADTS;
      return true;
   }
   else
   {
      aac_data_type = AAC_FORMAT_NON_ADTS;
      return false;
   }
}


/*===========================================================================

FUNCTION  peekMetaDataSize

DESCRIPTION
Public method used to determine the meta-data size of the fragment.

===========================================================================*/
bool Mpeg4File::peekMetaDataSize (uint32 fragment_num)
{
  //If the buffer size is zero while streaming, no need to check videofmt.
  if(m_wBufferOffset==0 && bHttpStreaming && ((m_videoFmtInfo.parse_fragment_cb == NULL) ||
      (m_videoFmtInfo.server_data == NULL)))
  {
    return false;
  }

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Trying to peek metadatasize");
  QCUtils::EnterCritSect(&videoFMT_Access_CS);
  m_videoFmtInfo.fragment_size_peek_cb (m_videoFmtInfo.server_data,
    fragment_num);

  while ((m_mp4ParseLastStatus != VIDEO_FMT_FAILURE)
    && (m_mp4ParseLastStatus != VIDEO_FMT_DATA_INCOMPLETE)
    && (m_mp4ParseLastStatus != VIDEO_FMT_DATA_CORRUPT)
    && (m_mp4ParseLastStatus != VIDEO_FMT_INFO)
    && (m_mp4ParseLastStatus != VIDEO_FMT_FRAGMENT_PEEK))
  {
    if((m_mp4ParseContinueCb == NULL) ||
      (m_mp4ParseServerData == NULL))
      break;
    else
      m_mp4ParseContinueCb (m_mp4ParseServerData);
  }
  QCUtils::LeaveCritSect(&videoFMT_Access_CS);

  video_fmt_mp4r_context_type *ctxt= (video_fmt_mp4r_context_type *)m_videoFmtInfo.server_data;
  switch(m_mp4ParseLastStatus)
  {
  case VIDEO_FMT_FAILURE:
  case VIDEO_FMT_DATA_CORRUPT:
    return false;

  case VIDEO_FMT_FRAGMENT_PEEK:
    m_minOffsetRequired = ctxt->fragment_size + ctxt->fragment_offset;
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "m_minOffsetRequired = %d", m_minOffsetRequired);
    bDataIncomplete = FALSE;
    return true;

  case VIDEO_FMT_DATA_INCOMPLETE:
    bDataIncomplete = TRUE;
    return false;

  case VIDEO_FMT_INFO:
    return true;

  default:
    break;
  }
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "m_minOffsetRequired = %d", m_minOffsetRequired);
  return false;
}


/*===========================================================================

FUNCTION  ParseStream

DESCRIPTION
Public method called at the end of every fragment and at the beginning.This is
to be used irrespective of whether PD or PS is in use.

===========================================================================*/
bool Mpeg4File::ParseStream ()
{
  //First Make sure next fragment moov/moof is available.
  if(!peekMetaDataSize(m_currentParseFragment))
  {
    sendParserEvent(Common::PARSER_RESUME);
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_RESUME");
    return false;
  }

  //Check for data present.
  if((m_wBufferOffset==0) || (m_wBufferOffset >= m_minOffsetRequired))
  {
    m_currentParseFragment++;
    //Then parse it and check for canPlayTracks().
    if(!parseMetaData())
    {
      //QTV_PS_PARSER_STATUS_PAUSED
      sendParserEvent(Common::PARSER_RESUME);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_RESUME, m_playVideo=%d",m_playVideo);
      return false;
    }
    else
    {
      Parsed = TRUE;
      if((parserState == Common::PARSER_RESUME || parserState == Common::PARSER_PAUSE)
        && CanPlayTracks(m_startupTime))
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_READY, m_playVideo=%d",m_playVideo);
        sendParserEvent(Common::PARSER_READY);
        return true;
      }
      else
      {
        return true;
      }
    }
  }
  else
  {
    sendParserEvent(Common::PARSER_RESUME);
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_RESUME, m_playVideo=%d",m_playVideo);
    return false;
  }
}
