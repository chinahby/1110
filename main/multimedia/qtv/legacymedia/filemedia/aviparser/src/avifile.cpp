/* =======================================================================
                              avifile.cpp
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

Copyright 2004 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             PERFORCE HEADER
$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/aviparser/main/latest/src/avifile.cpp#46 $
========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE
========================================================================== */
#include "customer.h"

#ifdef FEATURE_QTV_AVI

  #ifdef FEATURE_QTV_DIVX_DRM
    #include "DrmTypes.h"
    #include "DrmApi.h"
  #endif//#ifdef FEATURE_QTV_DIVX_DRM

 #include "avifile.h"
 #include "Events.h"
 #include "media.h"
 #include "aviParser.h"
 #include "Mpeg4Player.h"
 #include "filebase.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
//#define __TEST

#ifdef FEATURE_QTV_FILE_IO_CACHING
#define FILE_READ_BUFFER_SIZE_FOR_AVI 20480
#endif

#define ALLOW_REVERSE_4CC 1

#ifdef FEATURE_QTV_DIVX_DRM
MP4_ERROR_CODE MAP_DRM_ERROR_2_MP4_ERROR_CODE(drmErrorCodes_t error_code)
{
  if(error_code == DRM_NOT_AUTHORIZED)
  {
    return DRM_AUTHORIZATION_ERROR;
  }
  if(error_code == DRM_NOT_REGISTERED)
  {
    return DRM_ERROR_DEVICE_NOT_REGISTERED;
  }
  if(error_code == DRM_RENTAL_EXPIRED)
  {
    return DRM_RENTAL_COUNT_EXPIRED;
  }
  return DRM_PLAYBACK_GENERAL_ERROR;
}
#endif

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
                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION
  AVICallbakGetData

DESCRIPTION
  Its a callback function from the SDK to get the AVI data for decoding.
  This is not implemented by the SDK. It should be implemented by the app that calls the SDK.

ARGUMENTS
  nOffset                 Offset of the requested data (from beginning),
  nNumBytesRequest        Size of the requested data (in bytes).
  ppData                  Pointer to the buffer for filling in the ASF data
  u32UserData             Extra info From App. Given by user in aviin_open

DEPENDENCIES
  Used by AVI Parser.

RETURN VALUE
  The number of bytes actually being filled in

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
avi_int32 AVICallbakGetData (avi_int32         nOffset,
                             avi_int32         nNumBytesRequest,
                             unsigned char     *pData,
                             avi_int32         nMaxSize,
                             avi_uint32        u32UserData )
{
  if(u32UserData)
  {
    AVIFile *pAviFile = (AVIFile *)u32UserData;
    return( ( (pAviFile->FileGetData((uint32)nOffset, nNumBytesRequest, nMaxSize, pData))== nNumBytesRequest )?nNumBytesRequest:0);
  }
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "AVICallbakGetData u32UserData is NULL");
  return 0;
}
/* ======================================================================
FUNCTION:
  AVIFile::FileGetData

DESCRIPTION:
  To read the data from the file
INPUT/OUTPUT PARAMETERS:
  nOffset         : Offset from which data is being requested
  nNumBytesRequest: Total number of bytes requested
  ppData          : Buffer to be used for reading the data

RETURN VALUE:
 Number of bytes read

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::FileGetData(  uint32 nOffset,
                              uint32 nNumBytesRequest,
                              avi_int32 nMaxSize,
                              uint8* pData  )
{
  uint32 nRead = 0;
  if ( (m_AviFilePtr != NULL) && (m_pMpeg4Player != NULL) )
  {
    if(!m_pMpeg4Player->m_bMediaAbort)
    {
      nRead = FileBase::readFile(m_AviFilePtr, pData, nOffset, MIN(nNumBytesRequest,nMaxSize));
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Breaking, user Abort is true.");
    }
  }
  return nRead;
}
/* ==============================================================================
FUNCTION:
  AVIFile::getFileSize

DESCRIPTION:
  Returns the size of the file

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  File size

SIDE EFFECTS:
  None.
==============================================================================*/
uint64 AVIFile::getFileSize()
{
  return m_fileSize;
}

/* =============================================================================
FUNCTION:
  AVIFile::getCodecName

DESCRIPTION:
  Returns the name of the codec

INPUT/OUTPUT PARAMETERS:
  codecName          Buffer to copy the codec name into
  bufLen             Length of buffer to store the codec name
  mediaType          Specifies which codec name to get, audio or video

RETURN VALUE:
  None

SIDE EFFECTS:
  None
==============================================================================*/
void AVIFile::getCodecName(char *codecName,uint32 bufLen,uint32 trackId)
{
  CHUNK_t chunkType;
  int min = 0;
  avi_video_info pvInfo;
  avi_audio_info paInfo;

  if(!m_pAVIParser || (!codecName)|| (!bufLen))
  {
    return;
  }
  if(m_pAVIParser->GetTrackChunkType(trackId,&chunkType)==AVI_SUCCESS)
  {
    switch(chunkType)
    {

    case AVI_CHUNK_AUDIO:
      if(m_pAVIParser->GetAudioInfo(trackId,&paInfo)==AVI_SUCCESS)
      {
        if(paInfo.strnAudio.streamName)
        {
          min = (paInfo.strnAudio.streamNameSize <= bufLen)?paInfo.strnAudio.streamNameSize:bufLen;
          memcpy(codecName,paInfo.strnAudio.streamName,min);
        }
      }
      break;

    case AVI_CHUNK_VIDEO:
      if(m_pAVIParser->GetVideoInfo(trackId,&pvInfo)==AVI_SUCCESS)
      {
        if(pvInfo.strnVideo.streamName)
        {
          min = (pvInfo.strnVideo.streamNameSize <= bufLen)?pvInfo.strnVideo.streamNameSize:bufLen;
          memcpy(codecName,pvInfo.strnVideo.streamName,min);
        }
      }
      break;

    default:
      break;
    }
  }
}

/* ======================================================================
FUNCTION:
  AVIFile::AVIFile

DESCRIPTION:
  default constructor

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
AVIFile::AVIFile() // Default Constructor
{}
/* ======================================================================
FUNCTION:
  AVIFile::AVIFile

DESCRIPTION:
  local file playback Constructor

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
AVIFile::AVIFile( const OSCL_STRING &filename
                  ,Mpeg4Player *pMpeg4Player
                  ,unsigned char *pFileBuf
                  ,uint32 bufSize
                  ,bool bPlayVideo
                  ,bool bPlayAudio
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                 ,bool bHttpStream
                 ,uint32 wBufferOffset
                 ,uint32 wStartupTime
                 ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize
                 ,QtvPlayer::FetchBufferedDataT FetchBufferedData
                 ,QtvPlayer::InstanceHandleT handle
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
				  )
{
  _success = false;
  _fileErrorCode = DEFAULT_ERROR;

  m_pMpeg4Player= pMpeg4Player;
  m_playAudio = bPlayAudio;
  m_playVideo = bPlayVideo;
  m_playText = false;
  m_corruptFile = false;
  m_bVideoReposPending = false;
  m_bAudioReposPending = false;
  m_nNumStreams = 0;

  m_pAVIParser = NULL;
#ifdef FEATURE_QTV_DIVX_DRM
  m_pClipDrmInfo = NULL;
#endif

#ifdef FEATURE_QTV_DIVX_DRM
  drmContext = NULL;
  drmContextLength = 0;
  memset(drm_frame_info,0,DIVX_DRM_FRAME_DRM_INFO_SIZE);
#endif
#ifdef FEATURE_MPEG4_B_FRAMES
  m_videoFramesReadAhead.firstVideoFrame = false;
  m_videoFramesReadAhead.currentSampleIndex = 0;
  m_videoFramesReadAhead.validSampleIndex = 0;
  m_videoFramesReadAhead.allocatedForReadAhead = false;
  /* Initializing read ahead buffer
     This is to adjust the time stamps for Mpeg4 if there are any B-Frames
  */
  for(uint8 index = 0; index<AVI_MAX_VIDEO_FRAMES_READ_AHEAD; index++)
  {
    m_avi_video_samples[index].vop_type = NO_VOP;
    m_avi_video_samples[index].bVopCount = 0;
    m_avi_video_samples[index].size = 0;

    m_avi_video_samples[index].m_sampleInfo.time = 0;
    m_avi_video_samples[index].m_sampleInfo.size = 0;
    m_avi_video_samples[index].m_sampleInfo.sync = 0;
    m_avi_video_samples[index].m_sampleInfo.delta = 0;

    if( m_avi_video_samples[index].buff != NULL)
    {
      m_avi_video_samples[index].buff = NULL;
    }
  }
#endif /* FEATURE_MPEG4_B_FRAMES */

  m_pAudioPacketBuffer = NULL;
  m_nAudioPacketBufferSize = 0;

  //Initialized audio, video and binary streamIDs.
  m_nSelectedAudioStreamId = -1;
  m_nSelectedVideoStreamId = -1;
  m_nSelectedTextStreamId = -1;

  memset(m_sampleInfo, 0, sizeof(m_sampleInfo));
  memset(m_nDecodedDataSize, 0, sizeof(m_nDecodedDataSize));
  memset(m_nLargestFrame, 0, sizeof(m_nLargestFrame));

  //Initializing largest sample size for Audio and Video to 0
  m_audioLargestSize = 0;
  m_videoLargestSize = 0;

#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      bHttpStreaming = bHttpStream;
      QTV_USE_ARG1(wBufferOffset); // To avoid compiler warning
      bGetMetaDataSize = TRUE;
      bIsMetaDataParsed = FALSE;
      m_HttpDataBufferMinOffsetRequired.Offset = 0;
      m_HttpDataBufferMinOffsetRequired.bValid = FALSE;
      parserState = Common::PARSER_IDLE;
      m_startupTime = wStartupTime;
      memset(m_maxPlayableTime, 0, sizeof(m_maxPlayableTime));
      m_fpFetchBufferedDataSize = FetchBufferedDataSize;
      m_fpFetchBufferedData = FetchBufferedData;
      m_QtvInstancehandle = handle;
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

  if(!(m_playAudio || m_playVideo))
  {
    //we don't play anything except audio/video.
    //Do not create parser handle.
    _fileErrorCode = EVERYTHING_FINE;
    _success = true;
  }
  if (pFileBuf && bufSize)
  {
    m_pFileBuf = pFileBuf;
    m_FileBufSize = bufSize;
    m_fileSize = bufSize;
    m_filename = "";
    m_bStreaming = FALSE;
    m_AviFilePtr = OSCL_FileOpen (pFileBuf, bufSize);
  }
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  else if (m_fpFetchBufferedDataSize)
  {
     //Out here we dont know the file size
     //m_FileBufSize = 0xFFFFFFFF;
     //m_bStreaming = FALSE;

     /* since we only support PULL INTERFACE for local file playback, we are
       assuming all the data is available. We will change it when we will add
       supoport for Progressive Download */

     boolean bEndOfData = false;
     m_fpFetchBufferedDataSize(0, &m_FileBufSize, &bEndOfData, m_QtvInstancehandle);
     m_fpFetchBufferedDataSize(0, &m_fileSize, &bEndOfData, m_QtvInstancehandle);
     m_bStreaming = TRUE;

     //Using default trackId 0 for parsing..
     m_AviFilePtr = OSCL_FileOpen(NULL, 0, m_fpFetchBufferedData,0,m_QtvInstancehandle);
  }
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  else
  {
    m_filename = filename;
    m_pFileBuf = NULL;
    m_FileBufSize = 0;
    m_bStreaming = FALSE;
#ifdef FEATURE_QTV_FILE_IO_CACHING
    /* Calling with 10K cache  buffer size */
    m_AviFilePtr = OSCL_FileOpen(filename, (OSCL_TCHAR *) _T("rb"), FILE_READ_BUFFER_SIZE_FOR_AVI );
#else
    m_AviFilePtr = OSCL_FileOpen(filename, (OSCL_TCHAR *) _T("rb"));
#endif
    m_fileSize = OSCL_FileSize( m_filename );
  }

  if(m_AviFilePtr)
  {
    if(!ParseMetaData())
    {
      return;
    }
    UpdateTrackIdInFilePointer();

#ifdef __TEST
    if(m_playAudio||m_playVideo)
    {
      char hardCodedValues[] = {0x00,0x00,0x01,0xB0,0x08,0x00,0x00,0x01,0xB5,0x09};

      FILE*afp;
      FILE* vfp;
      afp = fopen("audioStream_avi.strm", "w");
      vfp = fopen("videoStream_avi.strm", "w");
      int max_video_count = 5;
      int max_audio_count = 5;

      uint8 buff[200000];
      uint32 index = 0;
      int count = 0;
      int nSize = 0;
      int trid = -1;
      CHUNK_t chunkType;
      for(int track = 0; track < m_nNumStreams && (trid < 0); track++)
      {
        // Get the chunk type corresponding to track number.
        m_pAVIParser->GetTrackChunkType(track,&chunkType);

        switch(chunkType)
        {
          case AVI_CHUNK_VIDEO:
          {
            if(m_playVideo)
            {
              trid = track;
              break;
            }
          }
        }
      }
      if(trid < 0)
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "AVIFile::AVIFile error occured while retrieving video track id");
        return;
      }
      bool bFirst = true;
      while(1)
      {
        if(m_playVideo)
        {
          uint8 vcodec = getTrackOTIType(trid);
          if(bFirst)
          {
            if(MPEG4_VIDEO == vcodec)
            {
              uint8* vol  = getTrackDecoderSpecificInfoContent(trid);
              uint32 size = getTrackDecoderSpecificInfoSize(trid);
              fwrite(hardCodedValues, 1, sizeof(hardCodedValues), vfp);
            }
            bFirst = false;
           continue;
          }
          else
          {
            nSize = getNextMediaSample(trid,buff,200000,index);
            if(nSize)
            {
              if(MPEG4_VIDEO != vcodec)
              {
                int wid = getTrackVideoFrameWidth(trid);
                int ht = getTrackVideoFrameHeight(trid);
                fwrite(&wid, 1, sizeof(int), vfp);
                fwrite(&ht, 1, sizeof(int), vfp);
              }
              fwrite(buff, 1, nSize, vfp);
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,"AVIFile::getNextMediaSample retrieved video chunk %d nSize %d",count++,nSize);
            }
          }
        }

        if(!nSize)
        {
          break;
        }
      }
      if(vfp)
      {
        fclose(vfp);
      }
    }
#endif
  }
}
#ifdef FEATURE_QTV_DIVX_DRM

/* ======================================================================
FUNCTION:
  AVIFile::initDivXDrmSystem

DESCRIPTION:
 Initializes the DivX DRM System.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
bool AVIFile::initDivXDrmSystem()
{
  bool bRet = true;
  if(m_playVideo && IsDRMProtection())
  {
    drmErrorCodes_t result;
	  avi_uint8 rentalMessageFlag = 0;
	  avi_uint8 useLimit;
	  avi_uint8 useCount;
	  avi_uint8 cgmsaSignal;
	  avi_uint8 acptbSignal;
	  avi_uint8 digitalProtectionSignal;
    int drm_size = 0;

    result = drmInitSystem( NULL,&drmContextLength );

    drmContext = (uint8_t*)QTV_Malloc( drmContextLength  );

    if(!drmContext)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmInitSystem memory allocation failed..");
      _fileErrorCode = DRM_OUT_OF_MEMORY;
      return false;
    }
    result = drmInitSystem( drmContext,
                            &drmContextLength );
    if ( DRM_SUCCESS!= result )
	  {
		  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmInitSystem failed result %ld",result);
      _fileErrorCode = MAP_DRM_ERROR_2_MP4_ERROR_CODE(result);
      return false;
	  }
    avi_uint8* drm_info = GetDRMInfo(&drm_size);
 	  result = drmInitPlayback( drmContext,drm_info );
	  if ( DRM_SUCCESS!= result )
	  {
		  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmInitPlayback failed result %ld",result);
      _fileErrorCode = MAP_DRM_ERROR_2_MP4_ERROR_CODE(result);
      return false;
	  }
    result = drmQueryRentalStatus( drmContext,
	                                 &rentalMessageFlag,
	                                 &useLimit,
	                                 &useCount );
	  if ( DRM_SUCCESS!= result )
	  {
		  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmQueryRentalStatus failed result %ld",result);
      _fileErrorCode = MAP_DRM_ERROR_2_MP4_ERROR_CODE(result);
      return false;
	  }
    for ( int i = 0; i < 3; i++ )
		{
		  drmSetRandomSample( drmContext );
		}
    result = drmQueryCgmsa( drmContext,&cgmsaSignal );
	  if ( DRM_SUCCESS!= result )
	  {
		  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmQueryCgmsa failed result %ld",result);
      _fileErrorCode = MAP_DRM_ERROR_2_MP4_ERROR_CODE(result);
      return false;
	  }
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "cgmsa signal %ld",cgmsaSignal);

	  result = drmQueryAcptb( drmContext,&acptbSignal );
	  if ( DRM_SUCCESS!= result )
	  {
		  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmQueryAcptb failed result %ld",result);
      _fileErrorCode = MAP_DRM_ERROR_2_MP4_ERROR_CODE(result);
      return false;
	  }
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "acptb signal %ld",acptbSignal);

	  result = drmQueryDigitalProtection( drmContext, &digitalProtectionSignal );
	  if ( DRM_SUCCESS!= result )
	  {
		  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmQueryDigitalProtection failed result %ld",result);
      _fileErrorCode = MAP_DRM_ERROR_2_MP4_ERROR_CODE(result);
      return false;
    }
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "digital protection signal %ld",digitalProtectionSignal);
    if(rentalMessageFlag == 0)
    {
	    result = drmCommitPlayback( drmContext );
	    if ( DRM_SUCCESS != result )
	    {
		    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmCommitPlayback failed result %ld",result);
        _fileErrorCode = MAP_DRM_ERROR_2_MP4_ERROR_CODE(result);
        return false;
	    }
    }
    m_pClipDrmInfo = QTV_Malloc( sizeof(QtvPlayer::ClipDrmInfoT));
    if(m_pClipDrmInfo)
    {
      memset(m_pClipDrmInfo,0,sizeof(QtvPlayer::ClipDrmInfoT));
      QtvPlayer::ClipDrmInfoT* clipdrminfo = (QtvPlayer::ClipDrmInfoT*)m_pClipDrmInfo;
      clipdrminfo->isRental = false;
      if(rentalMessageFlag > 0)
      {
        clipdrminfo->isRental = true;
      }
      clipdrminfo->useLimit = useLimit;
      clipdrminfo->useCounter = useCount;

      clipdrminfo->cgmsaSignal = cgmsaSignal;
	    clipdrminfo->acptbSignal = acptbSignal;
	    clipdrminfo->digitalProtectionSignal = digitalProtectionSignal;
    }
  }
  return bRet;
}
/* ======================================================================
FUNCTION:
  AVIFile::CommitDivXPlayback

DESCRIPTION:
 Commit the DivX Playback.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
bool AVIFile::CommitDivXPlayback()
{
  bool bRet = false;
  if(drmContext)
  {
    drmErrorCodes_t result = drmCommitPlayback( drmContext );
	  if ( DRM_SUCCESS == result )
	  {
      bRet = true;
	  }
    else
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmCommitPlayback failed result %ld",result);
	  _fileErrorCode = MAP_DRM_ERROR_2_MP4_ERROR_CODE(result);
    }
  }
  return bRet;
}
/* ======================================================================
FUNCTION:
  AVIFile::GetDRMContextInfo

DESCRIPTION:
 Retrieves the context info to be used for decrypting audio/video.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
avi_uint8* AVIFile::GetDRMContextInfo(avi_uint32* length)
{
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "AVIFile::GetDRMContextInfo m_playVideo %ld m_playAudio %d",m_playVideo,m_playAudio);
  if(!length)
  {
    return NULL;
  }
  *length = drmContextLength;
  return drmContext;
}
/* ======================================================================
FUNCTION:
  AVIFile::SetDRMContextInfo

DESCRIPTION:
 Sets the context info to be used for decrypting audio/video.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
void AVIFile::SetDRMContextInfo(avi_uint8 *context,avi_uint32 length)
{
  drmContext = context;
  drmContextLength = length;
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "AVIFile::SetDRMContextInfo m_playVideo %ld m_playAudio %d",m_playVideo,m_playAudio);
}
/* ======================================================================
FUNCTION:
  AVIFile::CopyDRMContextInfo

DESCRIPTION:
 Copies the DRM context info from 'ptr' in this object for decrypting audio/video.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
void AVIFile::CopyDRMContextInfo(void* ptr)
{
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "AVIFile::CopyDRMContextInfo m_playVideo %ld m_playAudio %d",m_playVideo,m_playAudio);
  if(ptr)
  {
    AVIFile* refAviHnd = (AVIFile*)ptr;
    avi_uint32 drmln = 0;
    avi_uint8* drmctx = refAviHnd->GetDRMContextInfo(&drmln);
    SetDRMContextInfo(drmctx,drmln);
  }
}
/* ======================================================================
FUNCTION:
  AVIFile::GetClipDrmInfo

DESCRIPTION:
 Retrieves clip specifc DRM information.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
void AVIFile::GetClipDrmInfo(void* ptr)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "AVIFile::GetClipDrmInfo");
  if(ptr)
  {
    QtvPlayer::ClipDrmInfoT* clip_drmInfo = (QtvPlayer::ClipDrmInfoT*)ptr;
    memset(clip_drmInfo,0,sizeof(QtvPlayer::ClipDrmInfoT));
    if(m_pClipDrmInfo)
    {
      memcpy(clip_drmInfo,(QtvPlayer::ClipDrmInfoT*)m_pClipDrmInfo,sizeof(QtvPlayer::ClipDrmInfoT) );
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "AVIFile::GetClipDrmInfo m_pClipDrmInfo is NULL");
    }
  }
}
#endif
/* ======================================================================
FUNCTION:
  AVIFile::ParseMetaData

DESCRIPTION:
  To begin parsing avi file and update media information such as
  total number of tracks, if drm is present etc.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
bool AVIFile::ParseMetaData()
{
  bool nRet = false;
  CHUNK_t chunkType;
  avi_video_info videoInfo;
  avi_audiotrack_summary_info audioSummaryInfo;
  //avi_int32 frames,bitmapFrames;
  avi_uint8 track;
  //avi_uint8 currAudioInfoIdx, currSubtitleInfoIdx;
  //avi_int8 initAudio = FALSE;
  //avi_int8 initOverlay = FALSE;
  //AVI_FILE_OFFSET currRiffOffset = 0;
  aviErrorType retError;

  #ifdef FEATURE_QTV_DIVX_DRM
    drmContext = NULL;
    drmContextLength = 0;
    memset(drm_frame_info,0,DIVX_DRM_FRAME_DRM_INFO_SIZE);
  #endif

  m_pAVIParser = new aviParser((avi_uint32)this,m_fileSize,(m_playAudio)?false:true,(m_playVideo)?false:true);
  if(m_pAVIParser)
  {
    if( (retError = m_pAVIParser->StartParsing()) == AVI_SUCCESS)
    {
      #ifdef FEATURE_QTV_DIVX_DRM
      if(!initDivXDrmSystem())
      {
        //Appropriate error code is set in initDivXDrmSystem.
        _success = false;
        return false;
      }
      #else
       QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW,
                  "ParseMetaData->FEATURE_QTV_DIVX_DRM is not defined..");
      #endif

      m_nNumStreams = m_pAVIParser->GetTotalNumberOfTracks();
      for(track = 0; track < m_nNumStreams; track++)
      {
        // Get the chunk type corresponding to track number.
        m_pAVIParser->GetTrackChunkType(track,&chunkType);

        switch(chunkType)
        {
          case AVI_CHUNK_AUDIO:
            // Get the audioInfo.
            if(m_pAVIParser->GetAudioTrackSummaryInfo(track,&audioSummaryInfo)==AVI_SUCCESS)
            {
              if( (m_playAudio)&&(m_nSelectedAudioStreamId < 0) )
              {
                //Only audio instance should select the audio track
                m_nSelectedAudioStreamId = track;
                _fileErrorCode = EVERYTHING_FINE;
                _success = true;
              }
            }
            break;

          case AVI_CHUNK_VIDEO:
            // Get videoInfo,and initialize video.
            if(m_pAVIParser->GetVideoInfo(track,&videoInfo)!=AVI_SUCCESS)
            {
              break;
            }
            if( (m_playVideo)&&(m_nSelectedVideoStreamId < 0))
            {
              //Only video instance should select the video track.
              m_nSelectedVideoStreamId = track;
              _fileErrorCode = EVERYTHING_FINE;
              _success = true;
            }
            break;
        }//switch(chunkType)
      }//for(track = 0; track < m_nNumStreams; track++)
      if( (_fileErrorCode == EVERYTHING_FINE) && (m_bStreaming) )
      {
        //send parser ready event to Mpeg4Player
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Common::PARSER_READY");
        sendParserEvent(Common::PARSER_READY);
      }
    }//if(m_pAVIParser->StartParsing() == AVI_SUCCESS)
    else
    {
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "AVIFile:ParseMetaData aviParing failed..retError %d",retError);
    }
  }//if(m_pAVIParser)

#ifdef WALK_INDEX_TABLES
  if( _success && (m_pAVIParser))
  {
    m_pAVIParser->WalkIDX1Table();
  }
#endif
 if(_success)
 {
   nRet = true;
 }
 return nRet;
}
/* ======================================================================
FUNCTION:
  AVIFile::~AVIFile()

DESCRIPTION:
  destructor

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
AVIFile::~AVIFile()
{
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::~AVIFile");
  if (m_AviFilePtr != NULL)
  {
    OSCL_FileClose(m_AviFilePtr);
    m_AviFilePtr = NULL;
  }

  if (m_pAudioPacketBuffer != NULL)
  {
    QTV_Free(m_pAudioPacketBuffer);
    m_pAudioPacketBuffer = NULL;
  }
  if (m_pAVIParser != NULL)
  {
    QTV_Delete(m_pAVIParser);
    m_pAVIParser = NULL;
  }
#ifdef FEATURE_QTV_DIVX_DRM
  if(m_playVideo && drmContext)
  {
    drmFinalizePlayback(drmContext);
    QTV_Free(drmContext);
    drmContext = NULL;
  }
  if(m_pClipDrmInfo)
  {
    QTV_Free(m_pClipDrmInfo);
    m_pClipDrmInfo = NULL;
  }
#endif
#ifdef FEATURE_MPEG4_B_FRAMES
  for(uint8 index = 0; index<AVI_MAX_VIDEO_FRAMES_READ_AHEAD; index++)
  {
    if( m_avi_video_samples[index].buff != NULL)
    {
      QTV_Free(m_avi_video_samples[index].buff);
      m_avi_video_samples[index].buff = NULL;
    }
  }
#endif /* FEATURE_MPEG4_B_FRAMES */
}

/* ======================================================================
FUNCTION:
  AVIFile::getNextMediaSample

DESCRIPTION:
  gets next sample of the given track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 size of sample

SIDE EFFECTS:
  None.
======================================================================*/
int32 AVIFile::getNextMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 &index)
{
  CHUNK_t Type;
  int32 nOutDataSize = 0;
#ifdef FEATURE_MPEG4_B_FRAMES
  uint32 temp_time;
  uint8 noOfBVops = 0;
  //uint32 width = 0;
  //uint32 height = 0;
  uint8 readIndex = 0;
  uint8 noOfBVopsInSameChunk = 0;
  uint8 totalNoOfBVops = 0;
#endif /* FEATURE_MPEG4_B_FRAMES */

  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"AVIFile::GetNextMediaSample");
  if(m_pAVIParser->GetTrackChunkType(id,&Type) == AVI_SUCCESS)
  {
    switch(Type)
    {
      case AVI_CHUNK_VIDEO:
#ifdef FEATURE_MPEG4_B_FRAMES
        /* This piece of code is applicable to MPEG4 video only. in future if we
           need to add for H264 then we may have to remove this check.
        */
        if(getTrackOTIType(id) == (uint8)MPEG4_VIDEO)
        {
          /* We need to allocate this memory once for the playback. This is
             to read ahead to know whether the following frame is B-Frame or not.
          */
          if(m_videoFramesReadAhead.allocatedForReadAhead == false)
          {
            //width = getTrackVideoFrameWidth(id);
            //height = getTrackVideoFrameHeight(id);
            for(readIndex = 0; readIndex<AVI_MAX_VIDEO_FRAMES_READ_AHEAD; readIndex++)
            {
              //m_avi_video_samples[readIndex].buff = (uint8*)QTV_Malloc(width * height *2);
              m_avi_video_samples[readIndex].buff = (uint8*)QTV_Malloc(AVI_DEFAULT_VIDEO_BUF_SIZE);
              if(m_avi_video_samples[readIndex].buff == NULL)
              {
                QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,
                                  "AVIFile::GetNextMediaSample memory allocation failed for size %d", AVI_DEFAULT_VIDEO_BUF_SIZE);
                return nOutDataSize;
              }
              m_avi_video_samples[readIndex].buff_Size = AVI_DEFAULT_VIDEO_BUF_SIZE;
            }
            m_videoFramesReadAhead.allocatedForReadAhead = true;
          }
          /* If we are reading for the first time just read the frame */
          if(m_videoFramesReadAhead.firstVideoFrame == false)
          {
            if(m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff_Size <  size)
            {
              QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                                  "AVIFile::GetNextMediaSample memory re allocating for size %d", size);
              m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff = (uint8 *)QTV_Realloc(m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff, size);
              if( m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff != NULL)
              {
                m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff_Size = size;
              }
              else
              {
                QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,
                                 "AVIFile::GetNextMediaSample memory re allocation failed for size %d", size);
                return 0;
              }
            }
            nOutDataSize = getNextAVIMediaSample(id, m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff, size, index);
            if(nOutDataSize > 0)
            {
              //Fill in sample information.
              m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.time = m_sampleInfo[id].time;
              m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.sync = m_sampleInfo[id].sync;
              m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.delta = m_sampleInfo[id].delta;
              m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.sample = m_sampleInfo[id].sample;
              m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].size = nOutDataSize;
              m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].vop_type = whichVop( m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff, nOutDataSize, &noOfBVopsInSameChunk);
              m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].bVopCount = noOfBVopsInSameChunk;
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"AVIFile::GetNextMediaSample vop type  %d  no. of Bvops  %d ", m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].vop_type, noOfBVopsInSameChunk);
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                              "AVIFile::GetNextMediaSample Reading first vedeo frame VIDEO nTimeStamp %d nDuration %d nSize %d",
                            m_sampleInfo[id].time,nOutDataSize);
            }
            else
            {
              return nOutDataSize;
            }
          }
          /* If we already read a frame from the file and that frame is a B-Frame then send that frame no need to read
             any frames ahead
             Here we already computed the time stamp of the B-frame
          */
          if(m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].vop_type == MPEG4_B_VOP)
          {
            /* copy the frame into the supplied buffer */
            memcpy(buf,m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff,m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].size);
            m_videoFramesReadAhead.currentSampleIndex = (m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD;
            QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "AVIFile::GetNextMediaSample Video nTimeStamp this is B-Frame %d nSize %d",
                            m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time ,
                            m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].size);
            return m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].size;
          }

          /* Read the next frame */
          m_videoFramesReadAhead.validSampleIndex = (m_videoFramesReadAhead.validSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD;
          if(m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff_Size <  size)
          {
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                                       "AVIFile::GetNextMediaSample memory re allocating for size %d", size);
            m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff = (uint8 *)QTV_Realloc(m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff, size);
            if( m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff != NULL)
            {
              m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff_Size = size;
            }
            else
            {
              QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,
                                  "AVIFile::GetNextMediaSample memory re allocation failed for size %d", size);
              return 0;
            }
          }
          nOutDataSize = getNextAVIMediaSample(id, m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff, size, index);
          if(nOutDataSize > 0)
          {
            //Fill in sample information.
            m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].m_sampleInfo.time = m_sampleInfo[id].time;
            m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].m_sampleInfo.sync = m_sampleInfo[id].sync;
            m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].m_sampleInfo.delta = m_sampleInfo[id].delta;
            m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].m_sampleInfo.sample = m_sampleInfo[id].sample;
            m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].size = nOutDataSize;
            m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].vop_type = whichVop( m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff, nOutDataSize, &noOfBVopsInSameChunk);
            m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].bVopCount = noOfBVopsInSameChunk;
            QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"AVIFile::GetNextMediaSample vop type  %d  no. of Bvops  %d ", m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].vop_type, noOfBVopsInSameChunk);
          }
          else
          {
            // Failed to read so decrementing the write pointer by 1
            m_videoFramesReadAhead.validSampleIndex = (m_videoFramesReadAhead.validSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD;
            return nOutDataSize;
          }
          m_videoFramesReadAhead.firstVideoFrame = true;
          /* After I or P see how many B-Frames are following this is required to compute the
             TS for the current I/P frame and the consecutive B-Frames */
          for(readIndex=0; readIndex<AVI_MAX_VIDEO_FRAMES_READ_AHEAD; readIndex++)
          {
            if(m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].vop_type == MPEG4_B_VOP)
            {
              /* Increment the B-VoP counter */
              ++noOfBVops;
              QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"AVIFile::GetNextMediaSample consecutive B-Frame count %d",noOfBVops);
              m_videoFramesReadAhead.validSampleIndex = (m_videoFramesReadAhead.validSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD;
              if(m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff_Size <  size)
              {
                QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                                  "AVIFile::GetNextMediaSample memory re allocating for size %d", size);
                m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff = (uint8 *)QTV_Realloc(m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff, size);
                if( m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff != NULL)
                {
                  m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff_Size = size;
                }
                else
                {
                  QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,
                                  "AVIFile::GetNextMediaSample memory re allocation failed for size %d", size);
                  return 0;
                }
              }
              nOutDataSize = getNextAVIMediaSample(id, m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff, size, index);
              if(nOutDataSize > 0)
              {
                m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].m_sampleInfo.time = m_sampleInfo[id].time;
                m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].m_sampleInfo.sync = m_sampleInfo[id].sync;
                m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].m_sampleInfo.delta = m_sampleInfo[id].delta;
                m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].m_sampleInfo.sample = m_sampleInfo[id].sample;
                m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].size = nOutDataSize;
                m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].vop_type = whichVop( m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].buff, nOutDataSize, &noOfBVopsInSameChunk);
                m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].bVopCount = noOfBVopsInSameChunk;
                QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"AVIFile::GetNextMediaSample vop type  %d  no. of Bvops  %d ", m_avi_video_samples[m_videoFramesReadAhead.validSampleIndex].vop_type, noOfBVopsInSameChunk);
                continue;
              }
              else
              {
                // Failed to read so decrementing the write pointer by 1
                m_videoFramesReadAhead.validSampleIndex = (m_videoFramesReadAhead.validSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD;
                return nOutDataSize;
              }
            }
            else
            {
              break;
            }
          }
		  // This is to take care of the B-Frames in the current access unit and following in a seperate access unit
          totalNoOfBVops = m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].bVopCount + noOfBVops;
          /* If we encounter a clip where there are 4 or more consecutive B-Frames we don't support so
             we need to return with an error
          */
          if(totalNoOfBVops == 4)
          {
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"AVIFile::GetNextMediaSample we don't support clips with more than 3 consecutive B-Frames  %d",totalNoOfBVops);
            return 0;
          }
          switch (totalNoOfBVops)
          {
            case 0:
              // There are no B-Frames following this P/I frame so no need to compute any new TS
              memcpy(buf,m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff,m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].size);
              m_videoFramesReadAhead.currentSampleIndex = (m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD;
              nOutDataSize = m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].size;
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "AVIFile::GetNextMediaSample Video nTimeStamp %d nSize %d",
                            m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time, nOutDataSize);
              break;
            case 1:
              // one B-frame is following P/I frame
              memcpy(buf,m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff,m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].size);
              // Recalculate the new time stamps for P/I and B-Frame
              temp_time = m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.time;
              m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.time += m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.delta;
			  // Only one B-Frame and it is following in a seperate access unit
              if(m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].bVopCount == 0)
              {
              m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time = temp_time;
              }
              //Increment the read index
              m_videoFramesReadAhead.currentSampleIndex = (m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD;
              nOutDataSize = m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].size;
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "AVIFile::GetNextMediaSample Video nTimeStamp %d nSize %d",
                            m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time, nOutDataSize);
	      break;
            case 2:
              // two B-frames are following this current P/I frame
              memcpy(buf,m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff,m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].size);
  	      // Recalculate the new time stamps for P/I and the consecutive two B-Frame
              temp_time = m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.time;
              m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.time += m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.delta * totalNoOfBVops;
			  // Two B-Frames and two are following in a seperate access units
              if(m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].bVopCount == 0)
              {
              m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 2) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time = m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time;
              m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time = temp_time;
              }
			  //Increment the read index
              m_videoFramesReadAhead.currentSampleIndex = (m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD;
              nOutDataSize = m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].size;
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "AVIFile::GetNextMediaSample Video nTimeStamp %d nSize %d",
                            m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time, nOutDataSize);
              break;
            case 3:
              // two B-frames are following this current P/I frame
              memcpy(buf,m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].buff,m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].size);
              // Recalculate the new time stamps for P/I and the consecutive three B-Frame
              temp_time = m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.time;
              m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.time += m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].m_sampleInfo.delta * totalNoOfBVops;
			  // Three B-Frames and two are following in a seperate access units
              if(m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].bVopCount == 0)
              {
              m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 3) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time = m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 2) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time;
              m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 2) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time = m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time;
              m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time = temp_time;
              }
			  // Three B-Frames are there and two are in the same access unit and one in a seperate access unit
              else if(m_avi_video_samples[m_videoFramesReadAhead.currentSampleIndex].bVopCount == 2)
              {
                m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time = m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time + m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.delta;
              }
			  //Increment the read index
              m_videoFramesReadAhead.currentSampleIndex = (m_videoFramesReadAhead.currentSampleIndex + 1) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD;
              nOutDataSize = m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].size;
              QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "AVIFile::GetNextMediaSample Video nTimeStamp %d nSize %d",
                            m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time, nOutDataSize);
              break;
            default :
              break;
          }
        }
        else
        {
          //other than Mpeg4 we don't support B-Frames
          nOutDataSize = getNextAVIMediaSample(id, buf, size, index);
        }
#else
        nOutDataSize = getNextAVIMediaSample(id, buf, size, index);
#endif /*  FEATURE_MPEG4_B_FRAMES */
        break;
      case AVI_CHUNK_AUDIO:
        nOutDataSize = getNextAVIMediaSample(id, buf, size, index);
        break;
      default:
        break;
    }
  }
  return nOutDataSize;
}

/* ======================================================================
FUNCTION:
  AVIFile::getNextAVIMediaSample

DESCRIPTION:
  gets next sample of the given track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 size of sample

SIDE EFFECTS:
  None.
======================================================================*/
int32 AVIFile::getNextAVIMediaSample(uint32 id, uint8 *buf, uint32 size, uint32 &index)
{
  avi_uint16 trackID;
  //AVI_FILE_OFFSET filePos;
  aviErrorType retVal = AVI_SUCCESS;
  avi_int32 nOutDataSize = 0;
  avi_sample_info minfo;
  avi_audio_info aInfo ;

#ifdef FEATURE_QTV_DIVX_DRM
  bool bDecrypt = false;
#endif

  if(!m_pAVIParser)
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL,
                  "AVIFile::getNextAVIMediaSample NULL PARSER HANDLE for track id = %d", id);
    return nOutDataSize;
  }

  while( (!nOutDataSize) && (retVal == AVI_SUCCESS) )
  {
    retVal = m_pAVIParser->GetNextSampleInfo(id,&minfo,size,&trackID);

    if(  ( (trackID != id) || ( (minfo.nSampleSize == 0)&&( (retVal == AVI_SUCCESS) || (retVal == AVI_INSUFFICIENT_BUFFER) ) ) )&&
         (minfo.chunkType != AVI_CHUNK_DRM) )
    {
      continue;
    }
    if( ((retVal == AVI_SUCCESS)||(retVal == AVI_INSUFFICIENT_BUFFER)) && ((trackID == id)||(minfo.chunkType == AVI_CHUNK_DRM)) )
    {
      if((minfo.nSampleSize > size)||(retVal == AVI_INSUFFICIENT_BUFFER))
      {
        QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,
                     "Error:AVIFile::getNextAVIMediaSample minfo.nSampleSize %d > available buffer size %d",minfo.nSampleSize,size);
        if (minfo.chunkType == AVI_CHUNK_AUDIO)
        {
          m_audioLargestSize = minfo.nSampleSize;
        }
        else if (minfo.chunkType == AVI_CHUNK_VIDEO)
        {
          m_videoLargestSize = minfo.nSampleSize;
        }
        return INSUFFICIENT_BUFFER_SIZE;
      }
      else
      {
        if(m_pAVIParser->GetCurrentSample(trackID,buf,size,minfo.nSampleSize)==AVI_SUCCESS)
        {

          if(minfo.chunkType == AVI_CHUNK_DRM)
          {
            #ifdef FEATURE_QTV_DIVX_DRM
            //Next chunk is encrypted.Record frame info and continue.
            if(drmContext && drmContextLength)
            {
              //ASSERT(minfo.nSampleSize <= DIVX_DRM_FRAME_DRM_INFO_SIZE);//To DO
              memcpy(drm_frame_info,buf,DIVX_DRM_FRAME_DRM_INFO_SIZE);
              bDecrypt = true;
              continue;
            }
            #else
            nOutDataSize = 0;
            retVal = AVI_END_OF_FILE;
            #endif

          }
          else
          {
            //Fill in sample information.
            m_sampleInfo[id].time = minfo.nTimeStamp;
            m_sampleInfo[id].size = minfo.nSampleSize;
            m_sampleInfo[id].sync = 1;
            m_sampleInfo[id].delta = minfo.nDuration;
            m_sampleInfo[id].sample = m_sampleInfo[id].sample++;
            nOutDataSize = minfo.nSampleSize;
            if(minfo.chunkType == AVI_CHUNK_AUDIO)
            {
              QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "AVIFile::getNextAVIMediaSample AUDIO nTimeStamp %d nDuration %d nSize %d",
                             minfo.nTimeStamp,minfo.nDuration,minfo.nSampleSize);
              if(m_pAVIParser->GetAudioInfo(id,&aInfo)==AVI_SUCCESS)
              {
                if(aInfo.strfAudio.wFormatTag == AVI_AUDIO_AC3)
                {
                  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "AVIFile::GetNextMediaSample AC3 AUDIO frame need to be decoded by OEM decoder So returning 0 as the frame size");
                  nOutDataSize = 0;
                }
              }
              #ifdef FEATURE_QTV_DIVX_DRM
              if(bDecrypt)
              {
                drmErrorCodes_t result = drmDecryptAudio( drmContext, buf, minfo.nSampleSize);
				        if ( DRM_SUCCESS!= result )
				        {
                   QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "AVIFile::getNextAVIMediaSample AUDIO frame decryption failed..");
                   nOutDataSize = 0;

                }
                bDecrypt = false;
              }//if(bDecrypt)
              #endif
            }
            else if(minfo.chunkType == AVI_CHUNK_VIDEO)
            {
              QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "AVIFile::getNextAVIMediaSample VIDEO nTimeStamp %d nDuration %d nSize %d",
                            minfo.nTimeStamp,minfo.nDuration,minfo.nSampleSize);

              #ifdef FEATURE_QTV_DIVX_DRM
              if(bDecrypt)
              {
                drmErrorCodes_t result = drmDecryptVideo( drmContext, buf, minfo.nSampleSize, drm_frame_info );
				        if ( DRM_SUCCESS!= result )
				        {
                   QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                            "AVIFile::getNextAVIMediaSample VIDEO frame decryption failed..");
                   nOutDataSize = 0;

                }
                bDecrypt = false;
              }//if(bDecrypt)
              #endif
            }//else if(minfo.chunkType == AVI_CHUNK_VIDEO)
          }//end of else of if(minfo.chunkType == AVI_CHUNK_DRM)
        }//if(m_pAVIParser->GetCurrentSample...
      }
    }
    if(retVal == AVI_END_OF_FILE)
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,
                 "AVIFile::getNextAVIMediaSample encountered end of file..");
    }
    else if(retVal != AVI_SUCCESS)
    {
      QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,
                 "AVIFile::getNextAVIMediaSample encountered an error while getting next_chunk");
    }
  }
  return nOutDataSize;
}


#ifdef FEATURE_MPEG4_B_FRAMES
/*===========================================================================

FUNCTION:
  whichVop

DESCRIPTION:
  This function tells the application whether a the bitstream passed contains
  a VOP and if yes then what kind of VOP is it - I/P/B or S VOP.

INPUT/OUTPUT PARAMETERS:
  pBitstream - pointer to raw bitstream.
  size - size of the bitstream.

RETURN VALUE:
   I_VOP: bitstream contains an I-VOP
   P_VOP: bitstream contains an P-VOP
   B_VOP: bitstream contains an B-VOP
   S_VOP: bitstream contains an S-VOP
   NO_VOP: bitstream contains no VOP.
SIDE EFFECTS:
  None.

===========================================================================*/
AVI_VOP_TYPE AVIFile::whichVop(uint8* pBitstream, int size, uint8 * noOfBVopsInSameChunk )
{
  int i;
  int j;
  int code = 0;
  int vopType;
  AVI_VOP_TYPE  returnVopType = NO_VOP;

  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,"AVIFile::whichVop");
  *noOfBVopsInSameChunk = 0;
  for (i=0;i<size;++i)
  {
    code <<= 8;
    code |= (0x000000FF & (pBitstream[i]));
    {
      if(MPEG4_VOP_START_CODE == code)
      {
        if((i+1)<size)
        {
          vopType = 0x000000c0 & (pBitstream[i+1]);
          if(0x00000000 == vopType) returnVopType = MPEG4_I_VOP;
          else if(0x00000040 == vopType)  returnVopType = MPEG4_P_VOP;
          else if(0x00000080 == vopType)  returnVopType = MPEG4_B_VOP;
          else if(0x000000c0 == vopType)  returnVopType = MPEG4_S_VOP;
        break;
      }
    }
  }
  }
  if(returnVopType == MPEG4_P_VOP)
  {
    if((i+2)<size)
    {
      code = 0;
      for (j=i+2;j<size;++j)
      {
        code <<= 8;
        code |= (0x000000FF & (pBitstream[j]));
        {
          if(MPEG4_VOP_START_CODE == code)
          {
            vopType = 0x000000c0 & (pBitstream[j+1]);
            if(0x00000080 == vopType) (*noOfBVopsInSameChunk) = (*noOfBVopsInSameChunk) + 1;
          }
        }
      }
    }
  }
  return returnVopType;
}
#endif /* FEATURE_MPEG4_B_FRAMES */
/* ======================================================================
FUNCTION:
  AVIFile::getMediaTimestampForCurrentSample

DESCRIPTION:
  gets time stamp of current sample of the track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 time stamp in track time scale unit

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getMediaTimestampForCurrentSample(uint32 id)
{
#ifdef FEATURE_MPEG4_B_FRAMES
  CHUNK_t Type;
  uint32 nTimeStamp = 0;
  if(!m_pAVIParser)
  {
    return AVI_PARSE_ERROR;
}
  if( id < m_pAVIParser->GetTotalNumberOfTracks() )
  {
    if(m_pAVIParser->GetTrackChunkType(id,&Type) == AVI_SUCCESS)
    {
      switch(Type)
      {
        case AVI_CHUNK_VIDEO:
          if(getTrackOTIType(id) == (uint8)MPEG4_VIDEO)
          {
            nTimeStamp = m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo.time;
          }
          else
          {
            nTimeStamp = m_sampleInfo[id].time;
          }
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "AVIFile::getMediaTimestampForCurrentSample Video TS %d",nTimeStamp);
          break;
        case AVI_CHUNK_AUDIO:
          nTimeStamp = m_sampleInfo[id].time;
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "AVIFile::getMediaTimestampForCurrentSample Audio TS %d",nTimeStamp);
          break;
        default:
          break;
      }
    }
  }
  return nTimeStamp;
#else
  return m_sampleInfo[id].time;
#endif /* FEATURE_MPEG4_B_FRAMES */
}

/* ======================================================================
FUNCTION:
  AVIFile::getMediaTimestampDeltaForCurrentSample

DESCRIPTION:
  gets time stamp delta of current sample of the track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 time stamp in track time scale unit

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getMediaTimestampDeltaForCurrentSample(uint32 id)
{
  QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR, "Timestamp delta @avifile: %d, ", m_sampleInfo[id].delta);
   return m_sampleInfo[id].delta;
}

#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
/* ======================================================================
FUNCTION:
  AVIFile::skipNSyncSamples

DESCRIPTION:
  Skips specified sync samples in forward or backward direction.

INPUT/OUTPUT PARAMETERS:


RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::skipNSyncSamples(int nSyncSamplesToSkip,
                                 uint32 id,
                                 bool *bError,
                                 uint32 currentPosTimeStamp)
{
  avi_sample_info sinfo;

  //Do quick sanity check
  if(!bError)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_FATAL, "AVIFile::skipNSyncSamples bError is NULL");
    return 0;
  }
  if(nSyncSamplesToSkip == 0)
  {
    //??? To avoid unnecessary Seek, just return 0 and set bError to TRUE.
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,
                 "AVIFile::skipNSyncSamples nSyncSamplesToSkip is 0");
    *bError = true;
    return 0;
  }
  //Now seek in parser

  if(  (!m_pAVIParser)                                     ||
       (m_pAVIParser->Seek((avi_uint8)id,
                           (avi_uint64)0,
                           currentPosTimeStamp,
                           &sinfo,false,
                           nSyncSamplesToSkip)!=AVI_SUCCESS) )
  {
    *bError = true;
    QTV_MSG_PRIO3(QTVDIAG_FILE_OPS,
                  QTVDIAG_PRIO_FATAL,
                  "AVIFile::skipNSyncSamples failed for trackid %ld nSyncSamplesToSkip %ld current TS %ld",
                  id,nSyncSamplesToSkip,currentPosTimeStamp);
    return 0;
  }
  *bError = false;
  QTV_MSG_PRIO4(QTVDIAG_FILE_OPS,
                QTVDIAG_PRIO_HIGH,
                "AVIFile::skipNSyncSamples trackid %d nSyncSamplesToSkip %ld current TS %ld TS returned %d",
                id,nSyncSamplesToSkip,currentPosTimeStamp,sinfo.nTimeStamp);

  m_sampleInfo[id].time = sinfo.nTimeStamp;
  m_sampleInfo[id].size = sinfo.nSampleSize;
  m_sampleInfo[id].sync = 1;
  m_sampleInfo[id].delta = sinfo.nDuration;
  return sinfo.nTimeStamp;
}
#endif
/* ======================================================================
FUNCTION:
  AVIFile::resetPlayback

DESCRIPTION:
  resets the playback time/sample to zero.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
void AVIFile::resetPlayback()
{}

/* ======================================================================
FUNCTION:
  AVIFile::resetPlayback

DESCRIPTION:
  resets the playback time to given time(pos) for a track.
  Also tells if we need to goto closest sync sample or not.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::resetPlayback(  uint32 repos_time, uint32 id, bool bSetToSyncSample,
                                bool *bError, uint32 currentPosTimeStamp)
{
  avi_sample_info sinfo;
  if(!m_pAVIParser)
  {
    *bError = true;
    QTV_MSG_PRIO2(QTVDIAG_FILE_OPS,
                  QTVDIAG_PRIO_FATAL,
                  "AVIFile::resetPlayback reposition failed for trackid %d repos_time %d ",
                  id,repos_time);
    return 0;
  }
  if(m_pAVIParser->Seek((avi_uint8)id,
                        (avi_uint64)repos_time,
                        currentPosTimeStamp,
                        &sinfo,bSetToSyncSample,0)!=AVI_SUCCESS)
  {
    *bError = true;
    QTV_MSG_PRIO2(QTVDIAG_FILE_OPS,
                  QTVDIAG_PRIO_FATAL,
                  "AVIFile::resetPlayback reposition failed for trackid %d repos_time %d ",
                  id,repos_time);
    return 0;
  }
  *bError = false;
  QTV_MSG_PRIO3(QTVDIAG_FILE_OPS,
                QTVDIAG_PRIO_HIGH,
                "AVIFile::resetPlayback trackid %d repos_time %d TS returned %d",
                id,repos_time,sinfo.nTimeStamp);

  m_sampleInfo[id].time = sinfo.nTimeStamp;
  m_sampleInfo[id].size = sinfo.nSampleSize;
  m_sampleInfo[id].sync = 1;
  m_sampleInfo[id].delta = sinfo.nDuration;
#ifdef FEATURE_MPEG4_B_FRAMES
  //After seek we need to reset these variables to fill the ahead buffer with fresh data.
  m_videoFramesReadAhead.firstVideoFrame = false;
  m_videoFramesReadAhead.currentSampleIndex = 0;
  m_videoFramesReadAhead.validSampleIndex = 0;
#endif /* FEATURE_MPEG4_B_FRAMES */
  return sinfo.nTimeStamp;
}
/* ======================================================================
FUNCTION:
  AVIFile::resetPlayback

DESCRIPTION:
  resets the playback time to zero for a track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
bool AVIFile::resetMediaPlayback(uint32)
{
  return 0;
}

/* ======================================================================
FUNCTION:
  AVIFile::randomAccessDenied

DESCRIPTION:
  gets if repositioning is allowed or not.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 AVIFile::randomAccessDenied()
{
  uint8 nRet = FALSE;
  return nRet;
}

/* ======================================================================
FUNCTION:
  AVIFile::getTitle

DESCRIPTION:
  gets title

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING AVIFile::getTitle() const
{
  OSCL_STRING sTitle;
  avi_uint16 titleSize=0;
  char *ptr;
  ptr = NULL;
  bool isAvailable = false;

  if(m_pAVIParser != NULL)
  {
    isAvailable = m_pAVIParser->getDataFromINFO("INAM",ptr,&titleSize);
    if ((isAvailable ==  true) && (titleSize!=0))
    {
      ptr = (char *)QTV_Malloc(titleSize+1);
      if(!ptr)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "AVIFile::getTitle memory allocation failed");
        sTitle = 0;
      }
      isAvailable = m_pAVIParser->getDataFromINFO("INAM",ptr,&titleSize);
      sTitle = ptr;
      QTV_Free(ptr);
    }
    else
    {
      sTitle = 0;
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::getTitle m_pAVIParser is NULL");
    sTitle = 0;
  }
  return sTitle;
}

/* ======================================================================
FUNCTION:
  AVIFile::getAuthor

DESCRIPTION:
  gets Author

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING AVIFile::getAuthor() const
{
  OSCL_STRING sAuthor;
  avi_uint16 authorSize=0;
  char *ptr;
  ptr = NULL;
  bool isAvailable = false;

  if(m_pAVIParser != NULL)
  {
    isAvailable = m_pAVIParser->getDataFromINFO("IART",ptr,&authorSize);
    if ((isAvailable ==  true) && (authorSize!=0))
    {
      ptr = (char *)QTV_Malloc(authorSize+1);
      if(!ptr)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "AVIFile::getAuthor memory allocation failed");
        sAuthor = 0;
      }
      isAvailable = m_pAVIParser->getDataFromINFO("IART",ptr,&authorSize);
      sAuthor = ptr;
      QTV_Free(ptr);
    }
    else
    {
      sAuthor = 0;
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::getAuthor m_pAVIParser is NULL");
    sAuthor = 0;
  }
  return sAuthor;
}

/* ======================================================================
FUNCTION:
  AVIFile::getDescription

DESCRIPTION:
  gets description

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING AVIFile::getDescription() const
{
  OSCL_STRING sDescription;
  avi_uint16 descriptionSize=0;
  char *ptr;
  ptr = NULL;
  bool isAvailable = false;

	if(m_pAVIParser != NULL)
  {
    isAvailable = m_pAVIParser->getDataFromINFO("ICMT",ptr,&descriptionSize);
    if ((isAvailable ==  true) && (descriptionSize!=0))
    {
      ptr = (char *)QTV_Malloc(descriptionSize+1);
      if(!ptr)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "AVIFile::getDescription memory allocation failed");
        sDescription = 0;
      }
      isAvailable= m_pAVIParser->getDataFromINFO("ICMT",ptr,&descriptionSize);
      sDescription = ptr;
      QTV_Free(ptr);
    }
    else
    {
      sDescription = 0;
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::getDescription m_pAVIParser is NULL");
    sDescription = 0;
  }
  return sDescription;
}

/* ======================================================================
FUNCTION:
  AVIFile::getRating

DESCRIPTION:
  gets rating

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING AVIFile::getRating() const
{
  /*if(m_pContentDesc && m_pContentDesc->uiRating_len)
  {
    OSCL_STRING sRating;
    char *pStr = (char *)QTV_Malloc(m_pContentDesc->uiRating_len+1);
    ConvertUnicodeToAscii(pStr, m_pContentDesc->pchRating, m_pContentDesc->uiRating_len);
    pStr[m_pContentDesc->uiRating_len] = '\0';
    sRating = pStr;
    QTV_Free(pStr);
    return sRating;
  }*/
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::getRating Not Implemented");
  return NULL;
}

/* ======================================================================
FUNCTION:
  AVIFile::getCopyright

DESCRIPTION:
  gets Copyright

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING AVIFile::getCopyright() const
{
  OSCL_STRING sCopyright;
  avi_uint16 copyrightSize=0;
  char *ptr;
  ptr = NULL;
  bool isAvailable = false;

	if(m_pAVIParser != NULL)
  {
    isAvailable = m_pAVIParser->getDataFromINFO("ICOP",ptr,&copyrightSize);
    if ((isAvailable ==  true) && (copyrightSize!=0))
    {
      ptr = (char *)QTV_Malloc(copyrightSize+1);
      if(!ptr)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "AVIFile::getCopyright memory allocation failed");
        sCopyright = 0;
      }
      isAvailable = m_pAVIParser->getDataFromINFO("ICOP",ptr,&copyrightSize);
      sCopyright= ptr;
      QTV_Free(ptr);
    }
    else
    {
      sCopyright = 0;
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::getCopyright m_pAVIParser is NULL");
    sCopyright = 0;
  }
  return sCopyright;
}

/* ======================================================================
FUNCTION:
  AVIFile::getVersion

DESCRIPTION:
  gets version

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING AVIFile::getVersion() const
{
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::getVersion Not Implemented");
  return NULL;
}

/* ======================================================================
FUNCTION:
  AVIFile::getCreationDate

DESCRIPTION:
  gets creation date

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
OSCL_STRING AVIFile::getCreationDate() const
{
  OSCL_STRING sCreationDate;
  avi_uint16 creationDateSize=0;
  char *ptr;
  ptr = NULL;
  bool isAvailable = false;

	if(m_pAVIParser!= NULL)
  {
    isAvailable = m_pAVIParser->getDataFromINFO("ICRD",ptr,&creationDateSize);
    if ((isAvailable ==  true) && (creationDateSize!=0))
    {
      ptr = (char *)QTV_Malloc(creationDateSize+1);
      if(!ptr)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "AVIFile::getCopyright memory allocation failed");
        sCreationDate = 0;
      }
      isAvailable = m_pAVIParser->getDataFromINFO("ICRD",ptr,&creationDateSize);
      sCreationDate = ptr;
      QTV_Free(ptr);
    }
    else
    {
      sCreationDate = 0;
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::getCreationDate m_pAVIParser is NULL");
    sCreationDate = 0;
  }
  return sCreationDate;
}

/* ======================================================================
FUNCTION:
  AVIFile::getMovieDuration

DESCRIPTION:
  gets movie duration in movie timescale unit.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getMovieDuration() const
{
  avi_uint32 nDuration = 0;
  if(!m_pAVIParser)
  {
    return nDuration;
  }
  nDuration = (avi_uint32)m_pAVIParser->GetClipDurationInMsec();
  return nDuration;
}

/* ======================================================================
FUNCTION:
  AVIFile::getMovieTimescale

DESCRIPTION:
  gets movie timescale

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getMovieTimescale() const
{
  uint32 nTimeScale = AVI_STREAM_TIME_SCALE;
  if(!m_pAVIParser)
  {
    return 0;
  }
  /*
  *If clip has video and is currently selcted for playback,
  *return video time scale.
  */
  //if(m_pAVIParser->m_hAviSummary.n_video_tracks && (m_nSelectedVideoStreamId >=0))
  //{
  //  return m_pAVIParser->m_hAviSummary.video_info[m_nSelectedVideoStreamId].frame_period_divisor;
  //}
  //else if(m_pAVIParser->m_hAviSummary.n_audio_tracks && (m_nSelectedAudioStreamId >=0))
  //{
  //  //If audio is selcted for playback, check if isVBR is TRUE/FALSE;
  //  if( (m_nSelectedAudioStreamId)&&
  //                                  (m_pAVIParser->m_hAviSummary.audio_info[m_nSelectedAudioStreamId].strfAudio.nBlockAlign == 1152 ||
  //                                  (m_pAVIParser->m_hAviSummary.audio_info[m_nSelectedAudioStreamId].strfAudio.nBlockAlign == 576))
  //    )
  //  {
  //    //VBR audio, timescale calculation is different than non VBR audio
  //    return m_pAVIParser->m_hAviSummary.audio_info[m_nSelectedAudioStreamId].strfAudio.nSamplesPerSec;
  //  }
  //  else
  //  {
  //    //Non VBR audio
  //    return m_pAVIParser->m_hAviSummary.audio_info[m_nSelectedAudioStreamId].strhAudio.dwRate;
  //  }
  //}
  return nTimeScale;
}

/* ======================================================================
FUNCTION:
  AVIFile::getMovieDurationMsec

DESCRIPTION:
  gets movie duration in milli secs

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getMovieDurationMsec() const
{
  avi_uint32 nDuration = 0;
  if(!m_pAVIParser)
  {
    return nDuration;
  }
  nDuration = (avi_uint32)m_pAVIParser->GetClipDurationInMsec();
  return nDuration;
}
/* ======================================================================
FUNCTION:
  AVIFile::GetMaximumBitRateForTrack(uint32 trackId)

DESCRIPTION:
  Returns the max bitrate for the track identified by trackId

INPUT/OUTPUT PARAMETERS:
  TrackId

RETURN VALUE:
 Max Data Bit Rate

SIDE EFFECTS:
  None.
======================================================================*/

uint32 AVIFile::GetMaximumBitRateForTrack(uint32 /*trackId*/)
{
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::GetMaximumBitRateForTrack Not Implemented");
  return 0;
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackWholeIDList

DESCRIPTION:
  gets list of track IDs

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getTrackWholeIDList(uint32 *ids)
{

  if(!m_pAVIParser)
  {
    return 0;
  }
  return ((avi_uint32)m_pAVIParser->GetTrackWholeIDList((avi_uint32*)ids));
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackContentVersion

DESCRIPTION:
  gets content version number

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
int16 AVIFile::getTrackContentVersion(uint32 id)
{
  return 0;
}

/* ======================================================================
FUNCTION:
  AVIFile::trackRandomAccessDenied

DESCRIPTION:
  gets repositioning permission for the track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 AVIFile::trackRandomAccessDenied(uint32 /*id*/)
{
  uint8 nRet = FALSE; /* by default random access is allowed */
  return nRet;
}
/* ======================================================================
FUNCTION:
  AVIFile::getTrackVideoFrameRate

DESCRIPTION:
  gets track video (if video) frame rate

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
float AVIFile::getTrackVideoFrameRate(uint32 id)
{
  avi_video_info videoInfo;

  if(!m_pAVIParser)
  {
    return 0.0;
  }
  if(m_pAVIParser->GetVideoInfo(id,&videoInfo)==AVI_SUCCESS)
  {
    if(videoInfo.strhVideo.dwScale)
    {
      return( (float)videoInfo.strhVideo.dwRate /(float)videoInfo.strhVideo.dwScale);
    }
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackVideoFrameWidth

DESCRIPTION:
  returns video track's frame width.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getTrackVideoFrameWidth(uint32 /*id*/)
{
  //avi_video_info videoInfo;
  avi_mainheader_avih avih;

  if(!m_pAVIParser)
  {
    return 0;
  }
  if(m_pAVIParser->GetAVIHeader(&avih)==AVI_SUCCESS)
  {
    return avih.dwWidth;
  }
  /*
  if(m_pAVIParser->GetVideoInfo(id,&videoInfo)==AVI_SUCCESS)
  {
    return videoInfo.strfVideo.biWidth;
  }
  */
  return 0;
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackVideoFrameHeight

DESCRIPTION:
  returns video track's frame height.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getTrackVideoFrameHeight(uint32 /*id*/)
{
  //avi_video_info videoInfo;
  avi_mainheader_avih avih;

  if(!m_pAVIParser)
  {
    return 0;
  }
  if(m_pAVIParser->GetAVIHeader(&avih)==AVI_SUCCESS)
  {
    return avih.dwHeight;
  }
  /*
  if(m_pAVIParser->GetVideoInfo(id,&videoInfo)==AVI_SUCCESS)
  {
    return videoInfo.strfVideo.biHeight;
  }
  */
  return 0;
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackMediaDuration

DESCRIPTION:
  gets track duration in track time scale unit

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getTrackMediaDuration(uint32 id)
{
  if(!m_pAVIParser)
  {
    return 0;
  }
  return (uint32)m_pAVIParser->GetTrackDuration((avi_uint8)id);
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackMediaTimescale

DESCRIPTION:
  gets track time scale

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getTrackMediaTimescale(uint32 id)
{
  //avi_audiotrack_summary_info audioSummaryInfo;
  //avi_video_info videoInfo;
  uint32 timeScale = AVI_STREAM_TIME_SCALE;
  //if(!m_pAVIParser)
  //{
  //  return timeScale;
  //}
  //
  //switch(m_pAVIParser->m_hAviSummary.stream_index[id].type)
  //{
  //  case AVI_CHUNK_AUDIO:
  //    if(m_pAVIParser->GetAudioTrackSummaryInfo(id,&audioSummaryInfo)==AVI_SUCCESS)
  //    {
  //      //If audio is selcted for playback, check if isVBR is TRUE/FALSE;
  //      if( (m_nSelectedAudioStreamId >=0)&& audioInfo.isVbr)
  //      {
  //        //VBR audio, timescale calculation is different than non VBR audio
  //        timeScale = m_pAVIParser->m_hAviSummary.audio_info[m_nSelectedAudioStreamId].strfAudio.nSamplesPerSec;
  //      }
  //      else
  //      {
  //        //Non VBR audio
  //        timeScale = m_pAVIParser->m_hAviSummary.audio_info[m_nSelectedAudioStreamId].strhAudio.dwRate;
  //      }
  //    }
  //  break;

  //  case AVI_CHUNK_VIDEO:
  //    if(m_pAVIParser->m_hAviSummary.n_video_tracks && (m_nSelectedVideoStreamId >=0))
  //    {
  //      timeScale = m_pAVIParser->m_hAviSummary.video_info[m_nSelectedVideoStreamId].frame_period_divisor;
  //    }
  //    break;

  //  default:
  //   break;
  //}
  return timeScale;
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackAudioSamplingFreq

DESCRIPTION:
  gets audio track's sampling frequency

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getTrackAudioSamplingFreq(uint32 id)
{
  avi_audiotrack_summary_info audioSummaryInfo;
  uint32 nSamplingFreq = 0;
  CHUNK_t cType;
  if(!m_pAVIParser)
  {
    return nSamplingFreq;
  }
  if(m_pAVIParser->GetTrackChunkType(id,&cType)==AVI_SUCCESS)
  {
    switch(cType)
    {
      case AVI_CHUNK_AUDIO:
        if(m_pAVIParser->GetAudioTrackSummaryInfo(id,&audioSummaryInfo)==AVI_SUCCESS)
        {
          nSamplingFreq = audioSummaryInfo.audioFrequency;
        }
        break;
      default:
       break;
    }
  }
  return nSamplingFreq;
}

/* ======================================================================
FUNCTION:
  ASFFile::UpdateTrackIdInFilePointer

DESCRIPTION:
  Updates pull buffer's Track ID.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
void AVIFile::UpdateTrackIdInFilePointer()
{
  CHUNK_t cType;
  if(m_pAVIParser)
  {
    int numStreams = m_pAVIParser->GetTotalNumberOfTracks();
    for(int i = 0; i < numStreams; i++)
    {
      if((m_pAVIParser->GetTrackChunkType(i,&cType)==AVI_SUCCESS) && (cType == AVI_CHUNK_AUDIO))
      {
        if((m_playAudio) && (m_AviFilePtr))
        {
          m_AviFilePtr->pullBuf.trackId = i;
        }
      }
      else if((m_pAVIParser->GetTrackChunkType(i,&cType)==AVI_SUCCESS) && (cType == AVI_CHUNK_VIDEO))
      {
        if((m_playVideo) && (m_AviFilePtr))
        {
          m_AviFilePtr->pullBuf.trackId = i;
        }
      }
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "AVIFile::UpdateTrackIdInFilePointer m_pAVIParser is NULL");
  }
}
/* ======================================================================
FUNCTION:
  AVIFile::getAudioFrameDuration

DESCRIPTION:
  Returns audio frame duration.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
long  AVIFile::getAudioFrameDuration(int trackid)
{
  long frame_duration = 0;
  avi_audiotrack_summary_info ainfo;
  avi_audio_info audtrackInfo;
  avi_audio_info aInfo ;
  CHUNK_t cType;

  if(!m_pAVIParser)
  {
    return frame_duration;
  }
  if(m_pAVIParser->GetTrackChunkType((avi_uint8)trackid,&cType)==AVI_SUCCESS)
  {
    switch(cType)
    {
      case AVI_CHUNK_AUDIO:
        if( (m_pAVIParser->GetAudioInfo((avi_uint8)trackid,&audtrackInfo)==AVI_SUCCESS) &&
            (m_pAVIParser->GetAudioTrackSummaryInfo((avi_uint8)trackid,&ainfo)==AVI_SUCCESS))
        {
          if(!ainfo.isVbr && (audtrackInfo.strhAudio.dwRate))
          {
            float framerate =  ( (float)audtrackInfo.strhAudio.dwScale /
                                             (float)audtrackInfo.strhAudio.dwRate );
            framerate *= 1000; //Convert to milliseconds.
            frame_duration = (long)framerate;
          }
          else if((audtrackInfo.strfAudio.nSamplesPerSec) && ainfo.isVbr && audtrackInfo.strhAudio.dwRate)
          {
            double framerate = ((double)audtrackInfo.strhAudio.dwScale/audtrackInfo.strhAudio.dwRate)* 1000.f;
            //framerate *= 1000; //Convert to milliseconds.
            frame_duration = (long)framerate;
          }
        }
        if(m_pAVIParser->GetAudioInfo(trackid,&aInfo)==AVI_SUCCESS)
        {
          if((aInfo.strfAudio.wFormatTag == AVI_AUDIO_AC3) ||
             (aInfo.strfAudio.wFormatTag == AVI_AUDIO_PCM))
          {
            frame_duration = (long) m_sampleInfo[trackid].delta;
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "frame_duration for AC3 or PCM is %d",frame_duration);
          }
        }
        break;
        default:
        break;
     }
  }
  return frame_duration;
}
/* ======================================================================
FUNCTION:
  AVIFile::SetIDX1Cache

DESCRIPTION:
  Sets IDX1 cache in AVIParser

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
void  AVIFile::SetIDX1Cache(void* ptr)
{
  if(m_pAVIParser)
  {
    m_pAVIParser->SetIDX1Cache(ptr);
  }
}
/* ======================================================================
FUNCTION:
  AVIFile::GetIDX1Cache

DESCRIPTION:
  Gets IDX1 cache from AVIParser

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
void* AVIFile::GetIDX1Cache()
{
  if(m_pAVIParser)
  {
    return m_pAVIParser->GetIDX1Cache();
  }
  return NULL;
}
/* ======================================================================
FUNCTION:
  AVIFile::getAudioSamplesPerFrame

DESCRIPTION:
  gets audio track's number of samples per frame.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getAudioSamplesPerFrame(uint32 id)
{
  uint32 dwSamplesPerFrame = 0;
  avi_audiotrack_summary_info audioSummaryInfo;
  CHUNK_t cType;

  if(!m_pAVIParser)
  {
    return dwSamplesPerFrame;
  }
  if(m_pAVIParser->GetTrackChunkType(id,&cType)==AVI_SUCCESS)
  {
    switch(cType)
    {
      case AVI_CHUNK_AUDIO:
        if(m_pAVIParser->GetAudioTrackSummaryInfo(id,&audioSummaryInfo)==AVI_SUCCESS)
        {
          if(audioSummaryInfo.audioFrequency <= 16000)
          {
            dwSamplesPerFrame = 1152;
          }
          else if(audioSummaryInfo.audioFrequency <= 32000 )
          {
            dwSamplesPerFrame = 1152;
          }
          else if(audioSummaryInfo.audioFrequency > 32000 )
          {
            dwSamplesPerFrame = 1152;
          }
        }
        break;
        default:
        break;
     }
  }
  return dwSamplesPerFrame;
}

/* ======================================================================
FUNCTION:
  AVIFile::GetAudioBitsPerSample

DESCRIPTION:
  returns bits per sample

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long AVIFile::GetAudioBitsPerSample(int id)
{
  avi_audiotrack_summary_info audioSummaryInfo;
  unsigned long nbitsPerAudioSample = 0;
  CHUNK_t cType;
  if(m_pAVIParser->GetTrackChunkType(id,&cType)==AVI_SUCCESS)
  {
    switch(cType)
    {
      case AVI_CHUNK_AUDIO:
        if(m_pAVIParser->GetAudioTrackSummaryInfo(id,&audioSummaryInfo)==AVI_SUCCESS)
        {
          nbitsPerAudioSample = audioSummaryInfo.nbitsPerAudioSample;
        }
        break;

      default:
       break;
    }
  }
  return nbitsPerAudioSample;
}

/* ======================================================================
FUNCTION:
  AVIFile::GetNumAudioChannels

DESCRIPTION:
  returns number of audio channels

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long AVIFile::GetNumAudioChannels(int id)
{
  unsigned long nChannels = 0;
  avi_audiotrack_summary_info audioSummaryInfo;
  CHUNK_t cType;

  if(m_pAVIParser->GetTrackChunkType(id,&cType)==AVI_SUCCESS)
  {
    switch(cType)
    {
      case AVI_CHUNK_AUDIO:
        if(m_pAVIParser->GetAudioTrackSummaryInfo(id,&audioSummaryInfo)==AVI_SUCCESS)
          nChannels = audioSummaryInfo.nChannels;
        break;

      case AVI_CHUNK_VIDEO:
       break;
      default:
        break;
    }
  }
  return nChannels;
}
/* ======================================================================
FUNCTION:
  AVIFile::GetAudioEncoderOptions

DESCRIPTION:
  returns the encoder options.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
unsigned long AVIFile::GetAudioEncoderOptions(int /*id*/)
{
  /*AUDIOSTREAMINFO *pAudioStreamInfo = GetAudioStreamInfo(id);
  if(pAudioStreamInfo)
  {
    return (unsigned long)( pAudioStreamInfo->nEncodeOpt );
  }*/
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::GetAudioEncoderOptions Not Implemented");
  return 0;
}

/* ======================================================================
FUNCTION:
  AVIFile::peekCurSample

DESCRIPTION:
  gets information about current sample of a track

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::peekCurSample(uint32 trackid, file_sample_info_type *pSampleInfo)
{
  CHUNK_t Type;
  if(!m_pAVIParser)
  {
    return AVI_PARSE_ERROR;
  }
  if( trackid < m_pAVIParser->GetTotalNumberOfTracks() )
  {
    if(m_pAVIParser->GetTrackChunkType(trackid,&Type) == AVI_SUCCESS)
    {
      switch(Type)
      {
        case AVI_CHUNK_VIDEO:
#ifdef FEATURE_MPEG4_B_FRAMES
          if(getTrackOTIType(trackid) == (uint8)MPEG4_VIDEO)
          {
            *pSampleInfo = m_avi_video_samples[(m_videoFramesReadAhead.currentSampleIndex + (AVI_MAX_VIDEO_FRAMES_READ_AHEAD -1)) % AVI_MAX_VIDEO_FRAMES_READ_AHEAD].m_sampleInfo;
          }
          else
          {
            *pSampleInfo = m_sampleInfo[trackid];
          }
#else
         *pSampleInfo = m_sampleInfo[trackid];
#endif /* FEATURE_MPEG4_B_FRAMES */
          break;
        case AVI_CHUNK_AUDIO:
          *pSampleInfo = m_sampleInfo[trackid];
          break;
        default:
          break;
      }
    }
    return EVERYTHING_FINE;
  }
  return DEFAULT_ERROR;
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackOTIType

DESCRIPTION:
  gets track OTI value

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 AVIFile::getTrackOTIType(uint32 id)
{
  uint8 format = 0xFF;
  CHUNK_t cType;
  avi_audio_info aInfo;
  avi_video_info vInfo;

  if(m_pAVIParser)
  {
    if(m_pAVIParser->GetTrackChunkType(id,&cType)==AVI_SUCCESS)
    {
      switch(cType)
      {
        case AVI_CHUNK_AUDIO:
         if(m_pAVIParser->GetAudioInfo(id,&aInfo)==AVI_SUCCESS)
          {
            switch(aInfo.strfAudio.wFormatTag)
            {
              case AVI_AUDIO_MP3:
              format = (uint8)MP3_AUDIO;
                break;
              case AVI_AUDIO_AC3:
                format = (uint8)AC3_AUDIO;
                break;
              case AVI_AUDIO_PCM:
                format = PCM_AUDIO;
                break;
              case AVI_AUDIO_AAC:
              case AVI_AUDIO_AAC_FAAD:
                format = (uint8)MPEG4_AUDIO;
                break;
              default:
              break;
            }
          }
          break;
        case AVI_CHUNK_VIDEO:
          if(m_pAVIParser->GetVideoInfo(id,&vInfo)==AVI_SUCCESS)
          {
#ifdef FEATURE_QTV_AVI_DIVX_PARSER
            if(
                (!memcmp(&vInfo.strhVideo.fccHandler,"divx",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"DIVX",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"XVID",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"xvid",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"dx50",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"DX50",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"FMP4",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"mp4v",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"MP4V",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"fmp4",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"YV12",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"yv12",sizeof(fourCC_t)))
              )
#else
            if(
                (!memcmp(&vInfo.strhVideo.fccHandler,"xvid",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"XVID",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"FMP4",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"mp4v",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"MP4V",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"fmp4",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"YV12",sizeof(fourCC_t))) ||
                (!memcmp(&vInfo.strhVideo.fccHandler,"yv12",sizeof(fourCC_t)))
              )
#endif  /* FEATURE_QTV_AVI_DIVX_PARSER */
              {
                format = (uint8)MPEG4_VIDEO;
              }
              else if( (!memcmp(&vInfo.strhVideo.fccHandler,"h263",sizeof(fourCC_t))) ||
                       (!memcmp(&vInfo.strhVideo.fccHandler,"H263",sizeof(fourCC_t)))
                     )
              {
                format = (uint8)H263_VIDEO;
              }
#ifdef FEATURE_DIVX_311_ENABLE
              else if(
                       (!memcmp(&vInfo.strhVideo.fccHandler,"div3",sizeof(fourCC_t))) ||
                       (!memcmp(&vInfo.strhVideo.fccHandler,"DIV3",sizeof(fourCC_t))) ||
                       (!memcmp(&vInfo.strhVideo.fccHandler,"DIV4",sizeof(fourCC_t))) ||
                       (!memcmp(&vInfo.strhVideo.fccHandler,"div4",sizeof(fourCC_t)))
                     )
              {
                format = (uint8)DIVX311_VIDEO;
              }
#endif
          }
          break;
        default:
          break;
      }
    }
  }
  return format;
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackAudioFormat

DESCRIPTION:
  gets track audio format based on VideoFMT enum

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 AVIFile::getTrackAudioFormat(uint32 id)
{
  uint8 format = 0xFF;
  avi_audiotrack_summary_info audioSummaryInfo;
  CHUNK_t cType;

  if(m_pAVIParser)
  {
    if(m_pAVIParser->GetTrackChunkType(id,&cType)==AVI_SUCCESS)
    {
      switch(cType)
      {
        case AVI_CHUNK_AUDIO:
          if(m_pAVIParser->GetAudioTrackSummaryInfo(id,&audioSummaryInfo)==AVI_SUCCESS)
            format = audioSummaryInfo.wFormatTag;
        break;

        default:
        break;
      }
    }
  }
  return format;
}

/* ======================================================================
FUNCTION:
AVIFile::getLargestFrameSize

DESCRIPTION:
  gets the largest frame size in the given track
INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getLargestFrameSize(uint32 /*id*/)
{
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::getLargestFrameSize Not Implemented");
  return 0;  /* not valid for WMV/WMA CODEC */
}

/* ======================================================================
FUNCTION:
  AVIFile::getFramesPerSample

DESCRIPTION:
  gets the number frames per sample of given track in video_fmt_stream_audio_subtype type.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 AVIFile::getFramesPerSample(uint32 /*id*/)
{
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::getFramesPerSample Not Implemented");
  return 0x00;
}

/* ======================================================================
FUNCTION:
  AVIFile::getTotalNumberOfFrames

DESCRIPTION:
  gets total number of frames in a given track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint16 AVIFile::getTotalNumberOfFrames(uint32 /*id*/)
{
  avi_mainheader_avih aviHdr;
  if(!m_pAVIParser || (m_pAVIParser->GetAVIHeader(&aviHdr)!=AVI_SUCCESS) )
  {
    return 0x00;
  }
  return aviHdr.dwTotalFrames;
}


/* ======================================================================
FUNCTION:
  AVIFile::getTrackMaxBufferSizeDB

DESCRIPTION:
  gets maximum buffer size to play the track

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
int32 AVIFile::getTrackMaxBufferSizeDB(uint32 id)
{
  avi_audio_info audioInfo;
  CHUNK_t cType;
  avi_mainheader_avih aviHdr;
  int32 bufferSize = AVI_DEFAULT_AUDIO_BUF_SIZE;

  if(!m_pAVIParser)
  {
    return 0;
  }
  if(m_pAVIParser->GetTrackChunkType(id,&cType)==AVI_SUCCESS)
  {
    switch(cType)
    {
      case AVI_CHUNK_AUDIO:
        if(m_audioLargestSize)
        {
          bufferSize = (int32)m_audioLargestSize;
        }
        else
        {
          if(m_pAVIParser->GetAudioInfo(id,&audioInfo)==AVI_SUCCESS)
          {
            bufferSize = audioInfo.strhAudio.dwSuggestedBufferSize;
            if (bufferSize == 0)
            {
               bufferSize = AVI_DEFAULT_AUDIO_BUF_SIZE;
            }
          }
          else
          {
            bufferSize = AVI_DEFAULT_AUDIO_BUF_SIZE;
          }
        }
        break;

      case AVI_CHUNK_VIDEO:
        if (m_videoLargestSize)
        {
          bufferSize = (int32)m_videoLargestSize;
        }
        else
        {
          if(m_pAVIParser->GetAVIHeader(&aviHdr)==AVI_SUCCESS)
          {
            uint32 width = getTrackVideoFrameWidth(id);
            uint32 height = getTrackVideoFrameHeight(id);
            bufferSize = aviHdr.dwSuggestedBufferSize?aviHdr.dwSuggestedBufferSize:AVI_DEFAULT_VIDEO_BUF_SIZE;
            bufferSize = MIN( (width*height*1.5), bufferSize );
          }
          else
          {
            bufferSize = AVI_DEFAULT_VIDEO_BUF_SIZE;
          }
        }
        break;

      default:
        bufferSize = AVI_DEFAULT_AUDIO_BUF_SIZE;
        break;
    }
  }
  return bufferSize;
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackAverageBitrate

DESCRIPTION:
  gets track's average bitrate

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 average bitrate

SIDE EFFECTS:
  None.
======================================================================*/
int32 AVIFile::getTrackAverageBitrate(uint32 id)
{
  int32 nBitRate = 0;
  avi_audiotrack_summary_info audioSummaryInfo;
  //avi_video_info videoInfo;
  //int nFrames = 0;
  CHUNK_t cType;

  if(!m_pAVIParser)
  {
    return nBitRate;
  }

  if(m_pAVIParser->GetTrackChunkType(id,&cType)==AVI_SUCCESS)
  {
    switch(cType)
    {
      case AVI_CHUNK_AUDIO:
        if(m_pAVIParser->GetAudioTrackSummaryInfo(id,&audioSummaryInfo)==AVI_SUCCESS)
        {
          nBitRate = audioSummaryInfo.audioBytesPerSec * 8;
        }
        break;

        case AVI_CHUNK_VIDEO:
          avi_mainheader_avih avih;
          if(m_pAVIParser->GetAVIHeader(&avih)==AVI_SUCCESS)
          {
            //Since AVI format has no direct way of getting clip video bitrate, returning dwMaxByesPerSec
            nBitRate = 8*(avih.dwMaxBytesPerSec);
          }
          break;
    }
  }
  return nBitRate;
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackMaxBitrate

DESCRIPTION:
  gets track's max bitrate

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
int32 AVIFile::getTrackMaxBitrate(uint32 id)
{
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_LOW, "AVIFile::getTrackMaxBitRate Not Implemented");
  return 0;//getTrackAverageBitrate(id);
}
/* ======================================================================
 FUNCTION:
   AVIFile::GetTotalNumberOfAudioStreams()

 DESCRIPTION:
   returns total number of audio streams.

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 int AVIFile::GetTotalNumberOfAudioStreams()
 {
   if(m_pAVIParser == NULL)
     return 0;
   return m_pAVIParser->GetTotalNumberOfAudioTracks();
 }
 /* ======================================================================
 FUNCTION:
   AVIFile::GetTotalNumberOfVideoStreams()

 DESCRIPTION:
   returns total number of video streams.

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 int AVIFile::GetTotalNumberOfVideoStreams()
 {
   if(m_pAVIParser == NULL)
     return 0;
   return m_pAVIParser->GetTotalNumberOfVideoTracks();
 }
 /* ======================================================================
 FUNCTION:
   AVIFile::GetTotalNumberOfTextStreams()

 DESCRIPTION:
   returns total number of text streams.

 INPUT/OUTPUT PARAMETERS:
   None.

 RETURN VALUE:
  none

 SIDE EFFECTS:
   None.
 ======================================================================*/
 int AVIFile::GetTotalNumberOfTextStreams()
 {
  return 0;
 }

/* ======================================================================
FUNCTION:
  AVIFile::getAllowAudioOnly

DESCRIPTION:
  gets if audio only playback is allowed.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 AVIFile::getAllowAudioOnly()
{
  return 1; /* by default allow */
}

/* ======================================================================
FUNCTION:
  AVIFile::getAllowVideoOnly

DESCRIPTION:
  gets if video only playback is allowed.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 AVIFile::getAllowVideoOnly()
{
  return 1; /* by default allow */
}

/* ======================================================================
FUNCTION:
  AVIFile::getTrackDecoderSpecificInfoContent

DESCRIPTION:
  this returns Sequence (VOL) Header.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint8 *AVIFile::getTrackDecoderSpecificInfoContent(uint32 id)
{
  CHUNK_t cType;
  uint32 pbufSize = (sizeof(fourCC_t)/2); //We prepare only 2 byte header for AAC audio
  //MP4_ERROR_CODE errorCode = DEFAULT_ERROR;

  if(!m_pAVIParser)
  {
    return NULL;
  }
  if(m_pAVIParser->GetTrackChunkType(id,&cType)==AVI_SUCCESS)
  {
    switch(cType)
    {
      case AVI_CHUNK_VIDEO:
        return m_pAVIParser->GetAVIVolHeader((avi_uint8)id);

      case AVI_CHUNK_AUDIO:
       {
         avi_audiotrack_summary_info audioSummaryInfo;
         avi_audio_info aInfo ;
         if ( (m_pAVIParser->GetAudioTrackSummaryInfo(id,&audioSummaryInfo)==AVI_SUCCESS) &&
              (m_pAVIParser->GetAudioInfo(id,&aInfo)==AVI_SUCCESS) &&
              ((aInfo.strfAudio.wFormatTag == AVI_AUDIO_AAC) ||(aInfo.strfAudio.wFormatTag == AVI_AUDIO_AAC_FAAD) ))
         {
            uint32 index =0;
            bool bError;
            uint8* firstSampleBuf = (uint8*)QTV_Malloc(5000);
            if(firstSampleBuf)
            {
              getNextMediaSample(id, firstSampleBuf, 5000, index);
              avi_uint8 audio_object = ((firstSampleBuf[2] >> 6) & 0x03)+ 1;
              avi_uint8 samplingFrequencyIndex = ((firstSampleBuf[2] >> 2) & 0x0F);
              //avi_uint8 channelConfiguration   = ((firstSampleBuf[2] << 2) & 0x04) | ((firstSampleBuf[3] >> 6) & 0x03);
              if(MAKE_AAC_AUDIO_CONFIG(m_TrackDecoderSpecificInfo, audio_object, samplingFrequencyIndex, audioSummaryInfo.nChannels, (uint8*)pbufSize))
              {
                //errorCode = EVERYTHING_FINE;
              }
              resetPlayback(0, id, false, &bError, 1);
              QTV_Free(firstSampleBuf);
              return m_TrackDecoderSpecificInfo;
            }
        }
        break;
      }
    }
  }
  return NULL;
}
/* ======================================================================
FUNCTION:
  AVIFile::getTrackDecoderSpecificInfoSize

DESCRIPTION:
  this returns Sequence (VOL) Header size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
uint32 AVIFile::getTrackDecoderSpecificInfoSize(uint32 id)
{
 CHUNK_t cType;
 //uint8 *buf = NULL;
 uint32 pbufSize = (sizeof(fourCC_t)/2); //Only for AAC audio

 if(!m_pAVIParser)
 {
   return 0;
 }
 if(m_pAVIParser->GetTrackChunkType(id,&cType)==AVI_SUCCESS)
 {
   switch(cType)
   {
     case AVI_CHUNK_VIDEO:
       return (m_pAVIParser->GetAVIVolHeaderSize(id));

     case AVI_CHUNK_AUDIO:
       avi_audiotrack_summary_info audioSummaryInfo;
       avi_audio_info aInfo ;
       if ( (m_pAVIParser->GetAudioTrackSummaryInfo(id,&audioSummaryInfo)==AVI_SUCCESS) &&
            (m_pAVIParser->GetAudioInfo(id,&aInfo)==AVI_SUCCESS) &&
            ((aInfo.strfAudio.wFormatTag == AVI_AUDIO_AAC) ||(aInfo.strfAudio.wFormatTag == AVI_AUDIO_AAC_FAAD) ))
       {
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "AVIFile::getTrackDecoderSpecificInfoSize pbufSize %d", pbufSize);
        return pbufSize;
       }

     default:
       break;
   }
 }
 return 0;
}
#ifdef FEATURE_QTV_DIVX_DRM
/* =============================================================================
FUNCTION:
 AVIFile::GetDRMInfo

DESCRIPTION:
Returns DRM header and the size of the header.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
avi_uint8* AVIFile::GetDRMInfo(int* size)
{
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "AVIFile::GetDRMInfo");
  if(!m_pAVIParser)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "AVIFile::GetDRMInfo Error,returning NULL");
    return NULL;
  }
  return m_pAVIParser->GetDRMInfo(size);
}
/* =============================================================================
FUNCTION:
 AVIFile::IsDRMProtection

DESCRIPTION:
Returns True if file is protected else returns false.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
  None.
=============================================================================*/
bool AVIFile::IsDRMProtection()
{
  QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "AVIFile::IsDRMProtection");
  if(!m_pAVIParser)
  {
    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "AVIFile::IsDRMProtection Error,returning FALSE");
    return false;
  }
  return m_pAVIParser->IsDRMProtection();
}
#endif//#ifdef FEATURE_QTV_DIVX_DRM

/* ======================================================================
FUNCTION:
  AVIFile::SetTimeStampedSample

DESCRIPTION:
  gets closest sample's info of the closest frame of given time.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 none

SIDE EFFECTS:
  None.
======================================================================*/
bool AVIFile::SetTimeStampedSample(uint32 id, uint32 TimeStamp, uint32 *newTimeStamp, boolean isRewind)
{
  //U64_WMC u64TimeReturn = 0;
  //tWMCDecStatus Ret;
  //tMediaType_WMC Type;
  //U64_WMC u64TempTimeReturn = 0;
  //uint32 StreamNum;

  //if(GetMediaTypeForStreamID(&Type, id) == WMCDec_Succeeded)
  //{
  //  if( TimeStamp && (Type==Video_WMC) )
  //  {
  //    if(isRewind)
  //    {
  //      Ret = WMCDecSeek (m_hASFDecoder, TimeStamp, &u64TempTimeReturn);

  //      if(isIndexObjectPresent())
  //      {
  //        if(Ret == WMCDec_Succeeded)
  //    	  {
  //          Ret = WMCDecSeekToPrevI(m_hASFDecoder, (U16_WMC)id);
  //          if(Ret == WMCDec_Succeeded)
	 //   {
  //            Ret = WMCDecGetIFrameTime (m_hASFDecoder, (U16_WMC)id, u64TempTimeReturn, &u64TimeReturn, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	 //   }
  //    	  }//end of if(Ret == WMCDec_Succeeded)
  //      }
  //      else
  //      {
  //        if(Ret == WMCDec_Succeeded)
  //        {
  //          Ret = WMCDecSeekToPrevIFrame(m_hASFDecoder, (U16_WMC)id, &u64TimeReturn );
  //        }
  //      }
  //    }
  //    else
  //    {
  //      /* Now we can handle FF with/without Simple index object */
  //      Ret = WMCDecGetNextIFrameTime (m_hASFDecoder, (U16_WMC)id, TimeStamp, &u64TempTimeReturn, NULL, NULL, NULL, NULL );
  //      if(Ret == WMCDec_Succeeded)
  //    	{
  //        Ret = WMCDecSeek (m_hASFDecoder, u64TempTimeReturn, &u64TimeReturn);
  //      }
  //    }

  //    if(Ret == WMCDec_Succeeded)
  //    {
  //      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "SetTimeStampedSample, Time IN=%d, OUT=%d", TimeStamp,u64TimeReturn);
  //      *newTimeStamp = (uint32)u64TimeReturn;
  //      return true;
  //    }
  //  }//end of if( TimeStamp && (Type==Video_WMC) )
  //  else
  //  {
  //   if(GetStreamNumForID(&StreamNum, id) == WMCDec_Succeeded)
  //    {
  //      if( (m_bWMADecodeDone) && (!isRewind) )
  //      {
  //        //If audio is already done and user is attempting FWD, fail the repositioning.
  //        *newTimeStamp = m_sampleInfo[StreamNum].time;
  //        m_sampleInfo[StreamNum].size = 0;
  //        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"SetTimeStampedSample, Seek failed for track  %d as WMA track has ended.",id);
  //        return false;
  //      }
  //    }

  //    Ret = WMCDecSeek (m_hASFDecoder, TimeStamp, &u64TimeReturn);
  //    if( (Ret == WMCDec_Succeeded)|| (Ret == WMCDec_DecodeComplete) )
  //    {
  //      if(Ret == WMCDec_DecodeComplete)
  //      {
  //        *newTimeStamp = (uint32)TimeStamp;
  //        if(GetStreamNumForID(&StreamNum, id) == WMCDec_Succeeded)
  //        {
  //        	m_sampleInfo[StreamNum].time = (uint32)TimeStamp;
  //          m_bWMADecodeDone = true;
  //          m_nWMATotalDummyBytesSent = WMA_TOTAL_DUMMY_BYTES_TO_SEND;
  //          m_sampleInfo[StreamNum].size = 0;
  //          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"SetTimeStampedSample, Seek to EOF TS %d",TimeStamp);
  //        }
  //      }//end of if(Ret == WMCDec_DecodeComplete)
  //      else
  //      {
  //        *newTimeStamp = (uint32)u64TimeReturn;
  //      }
  //      QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH,"SetTimeStampedSample, Time IN=%d, OUT=%d", TimeStamp, *newTimeStamp);
  //      return true;
  //    }
  //  }
  //}
  //QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "SetTimeStampedSample failed");
  return false;
}
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
/*===========================================================================

FUNCTION  sendParserEvent

DESCRIPTION
  Public method used send parser events

===========================================================================*/
void AVIFile::sendParserEvent(Common::ParserStatusCode status)
{
  parserState = status;
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  QTV_HTTP_PARSER_STATUS_EVENT_type *pEvent = QCCreateMessage(QTV_HTTP_PARSER_STATUS_EVENT, m_pMpeg4Player);
#else
  QTV_PS_PARSER_STATUS_EVENT_type *pEvent = QCCreateMessage(QTV_PS_PARSER_STATUS_EVENT, m_pMpeg4Player);
#endif

  if (pEvent)
  {
    pEvent->status = status;
    pEvent->bHasVideo = m_playVideo;
    pEvent->bHasAudio = m_playAudio;
    pEvent->bHasText = m_playText;
    QCUtils::PostMessage(pEvent, 0, NULL);
  }
}

//*===========================================================================
//
//FUNCTION  sendParseHTTPStreamEvent
//
//DESCRIPTION
//  Public method used to switch contexts and call the parseHttpStream.
//
//===========================================================================*/
//void AVIFile::sendParseHTTPStreamEvent(void)
//{
//  QTV_PROCESS_HTTP_STREAM_type *pEvent = QCCreateMessage(QTV_PROCESS_HTTP_STREAM, m_pMpeg4Player);
//
//  if (pEvent)
//  {
//    pEvent->bHasAudio = (bool) m_playAudio;
//    pEvent->bHasVideo = (bool) m_playVideo;
//    pEvent->bHasText = (bool) m_playText;
//    QCUtils::PostMessage(pEvent, 0, NULL);
//  }
//}
//
///*===========================================================================
//
//FUNCTION  updateBufferWritePtr
//
//DESCRIPTION
//  Public method used to update the write buffer offset during Http streaming.
//
//===========================================================================*/
//void AVIFile::updateBufferWritePtr ( uint32 writeOffset )
//{
//  WMFDecoderEx *pDecoder = (WMFDecoderEx *)m_hASFDecoder;
//  //Executing in the UI thread context.
//
//  if(pDecoder)
//  {
//    pDecoder->wHttpDataBuffer.Offset = writeOffset;
//    pDecoder->wHttpDataBuffer.bValid = TRUE;
//  }
//
//  if((parserState == Common::PARSER_PAUSE) || (parserState == Common::PARSER_RESUME))
//  {
//     //check if we got sufficient data to start parsing the
//     //meta data.
//     sendParseHTTPStreamEvent();
//  }
//}
//
//
///*===========================================================================
//
//FUNCTION  getMetaDataSize
//
//DESCRIPTION
//  Public method used to determine the meta-data size of the fragment.
//
//===========================================================================*/
//tWMCDecStatus AVIFile::getMetaDataSize ( void )
//{
//  tWMCDecStatus wmerr = WMCDec_Fail;
//  uint32 nHttpDownLoadBufferOffset = 0;
//  boolean bHttpDownLoadBufferOffsetValid = GetHTTPStreamDownLoadedBufferOffset(&nHttpDownLoadBufferOffset);
//  WMFDecoderEx *pDecoder = (WMFDecoderEx *)m_hASFDecoder;
//  U32_WMC nAsfHeaderSize = 0;
//
//  if( pDecoder && bHttpDownLoadBufferOffsetValid && (nHttpDownLoadBufferOffset > (MIN_OBJECT_SIZE + sizeof(U32_WMC) + 2*sizeof(U8_WMC)) ) )
//  {
//    wmerr = GetAsfHeaderSize(&m_hASFDecoder,&nAsfHeaderSize );
//  }
//  if(wmerr == WMCDec_Succeeded)
//  {
//    m_HttpDataBufferMinOffsetRequired.Offset = nAsfHeaderSize;
//    m_HttpDataBufferMinOffsetRequired.bValid = TRUE;
//    bGetMetaDataSize = FALSE;
//    return wmerr;
//  }
//  else
//  {
//    bGetMetaDataSize = TRUE;
//    return wmerr;
//  }
//
//  return WMCDec_Fail;
//}
//
///*===========================================================================
//
//FUNCTION  parseHTTPStream
//
//DESCRIPTION
//  Public method used to parse the Http Stream.
//
//===========================================================================*/
//bool AVIFile::parseHTTPStream ( void )
//{
//
//  tWMCDecStatus wmerr = WMCDec_Succeeded;
//  bool returnStatus = true;
//  uint32 nHttpDownLoadBufferOffset = 0;
//  boolean bHttpDownLoadBufferOffsetValid = GetHTTPStreamDownLoadedBufferOffset(&nHttpDownLoadBufferOffset);
//
//
//  if(bGetMetaDataSize)
//  {
//     wmerr = getMetaDataSize();
//  }
//
//  if(wmerr != WMCDec_Succeeded)
//  {
//    //QTV_PS_PARSER_STATUS_PAUSED
//    sendParserEvent(Common::PARSER_PAUSE);
//    QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_PAUSE");
//    returnStatus = false;
//  }
//  else if(wmerr == WMCDec_Succeeded)
//  {
//     if((nHttpDownLoadBufferOffset >= m_HttpDataBufferMinOffsetRequired.Offset)
//        && bHttpDownLoadBufferOffsetValid && m_HttpDataBufferMinOffsetRequired.bValid)
//     {
//       if( !bIsMetaDataParsed )
//       {
//         if(ParseMetaData() == WMCDec_Succeeded)
//	 {
//           //QTV_PS_PARSER_STATUS_RESUME
//           bIsMetaDataParsed = TRUE;
//           m_HttpDataBufferMinOffsetRequired.bValid = FALSE;
//           sendParserEvent(Common::PARSER_RESUME);
//           returnStatus = true;
//           QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_RESUME");
//         }
//	 else
//	 {
//           //QTV_PS_PARSER_STATUS_PAUSED
//           sendParserEvent(Common::PARSER_PAUSE);
//           QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_PAUSE");
//           returnStatus = false;
//	 }
//       }
//     }
//
//     if ((parserState == Common::PARSER_RESUME) && CanPlayTracks(m_startupTime) )
//     {
//        //QTV_PS_PARSER_STATUS_READY
//        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Parser State = Common::PARSER_READY");
//        sendParserEvent(Common::PARSER_READY);
//	returnStatus = true;
//      }
//      else
//      {
//        returnStatus = false;
//      }
//  }
//
//  return returnStatus;
//}
//
//
///*===========================================================================
//
//FUNCTION  sendHTTPStreamUnderrunEvent
//
//DESCRIPTION
//  Public method used to switch contexts and notify the player about buffer-underrun.
//
//===========================================================================*/
//void AVIFile::sendHTTPStreamUnderrunEvent(void)
//{
//  QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_type *pEvent = QCCreateMessage(QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT, m_pMpeg4Player);
//
//  if (pEvent)
//  {
//    pEvent->bAudio = (bool) m_playAudio;
//    pEvent->bVideo = (bool) m_playVideo;
//    pEvent->bText = (bool) m_playText;
//    QCUtils::PostMessage(pEvent, 0, NULL);
//  }
//}
///*===========================================================================
//
//FUNCTION  GetHTTPStreamDownLoadedBufferOffset
//
//DESCRIPTION
//  Public method used to switch contexts and notify the player about buffer-underrun.
//
//===========================================================================*/
//boolean AVIFile::GetHTTPStreamDownLoadedBufferOffset(U32_WMC * pOffset)
//{
//  WMFDecoderEx *pDecoder = (WMFDecoderEx *)m_hASFDecoder;
//
//  if(pDecoder && pOffset)
//  {
//    if(m_fpFetchBufferedDataSize)
//    {
//      //Pull interface so pull dnld data size from OEM
//      m_fpFetchBufferedDataSize( 0, &(pDecoder->wHttpDataBuffer.Offset, m_QtvInstancehandle) );
//      pDecoder->wHttpDataBuffer.bValid = TRUE;
//    }
//    if( pDecoder->wHttpDataBuffer.bValid )
//    {
//      *pOffset = pDecoder->wHttpDataBuffer.Offset;
//      return TRUE;
//    }
//  }
//  return FALSE;
//}
//
/*===========================================================================

FUNCTION  GetTotalAvgBitRate

DESCRIPTION
  Public method used to switch contexts and notify the player about buffer-underrun.

===========================================================================*/
bool AVIFile::GetTotalAvgBitRate(uint32 * pBitRate)
{
  if(!m_nNumStreams)
  {
    return false;
  }

  //*pBitRate = 0;

  //for(uint8 index = 0; index < m_nNumStreams; index++)
  //{
  //   /* the Movie average bitrate from sum of all the tacks average bitrate */
  //   if(m_ptMediaTypeStreams)
  //   {
  //     *pBitRate = *pBitRate + getTrackAverageBitrate(m_ptMediaTypeStreams[index].wStreamId);
  //   }
  //}

  return true;
}

/*===========================================================================

FUNCTION  CanPlayTracks

DESCRIPTION
  Public method used to switch contexts and notify the player about buffer-underrun.

===========================================================================*/
bool AVIFile::CanPlayTracks(uint32 nTotalPBTime)
{
  //uint32 nMinBufferingTime = 0;
  //uint32 nMediaMaxPlayableTime = 0;
  //uint32  nTotalAvgBitRate = 0;
  //uint32 nHttpDownLoadBufferOffset = 0;
  //boolean bHttpDownLoadBufferOffsetValid = GetHTTPStreamDownLoadedBufferOffset(&nHttpDownLoadBufferOffset);

  //if( (m_HttpDataBufferMinOffsetRequired.bValid == FALSE) && GetTotalAvgBitRate( &nTotalAvgBitRate ) )
  //{
  //  /* Compute Required Buffering/Rebuffering time */

  //  (void)GetMediaMaxPlayableTime( &nMediaMaxPlayableTime );

  //  if(nTotalPBTime > nMediaMaxPlayableTime)
  //  {
  //    nMinBufferingTime = nTotalPBTime - nMediaMaxPlayableTime;
  //  }
  //  else
  //  {
  //    nMinBufferingTime = 0;
  //  }

  //  QTV_MSG_PRIO3(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_MED, " nTotalPBTime= %u, nMinBufferingTime= %u nMediaMaxPlayableTime = %u", nTotalPBTime, nMinBufferingTime,nMediaMaxPlayableTime);

  //  if( !nMinBufferingTime )
  //  {
  //    /* nBufferingTime from Mpeg4Plyer/OEM is '0', then take it as Preroll time */
  //    nMinBufferingTime = getPrerollTime();
  //    QTV_MSG_PRIO2(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_HIGH, "Taking Preroll as Rebuffering Time: nTotalPBTime= %d, nMinBufferingTime= %d", nTotalPBTime, nMinBufferingTime);
  //  }

  //  /* Estimate approximate minimum offset required */
  //  m_HttpDataBufferMinOffsetRequired.Offset = nHttpDownLoadBufferOffset +  (nTotalAvgBitRate * nMinBufferingTime)/8000;
  //  m_HttpDataBufferMinOffsetRequired.bValid = TRUE;

  //  if( m_HttpDataBufferMinOffsetRequired.Offset > (GetLastPacketOffset() + GetAsfPacketSize()) )
  //  {
  //     m_HttpDataBufferMinOffsetRequired.Offset =  (GetLastPacketOffset() + GetAsfPacketSize());
  //  }

  //}

  //if((nHttpDownLoadBufferOffset >= m_HttpDataBufferMinOffsetRequired.Offset) &&
  //    bHttpDownLoadBufferOffsetValid && m_HttpDataBufferMinOffsetRequired.bValid )
  //{
  //  return true;
  //}
  //
  //QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"CanPlayTracks: HttpDownLoadBufferOffset= %d, HttpDataBufferMinOffsetRequired= %d", nHttpDownLoadBufferOffset, m_HttpDataBufferMinOffsetRequired.Offset);

  return true;
}

/* ======================================================================
FUNCTION:
  AVIFile::GetMediaMaxTimeStampPlayable

DESCRIPTION:
  gets time stamp of current sample of the track.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 time stamp in track time scale unit

SIDE EFFECTS:
  None.
======================================================================*/
//tWMCDecStatus AVIFile::GetMediaMaxPlayableTime(U32_WMC *nMaxPBTime)
//{
//  uint32 nMaxPlayableTime = 0;  // default max playable time
//
//  if( (m_pStreamDecodePattern == NULL) || (nMaxPBTime == NULL) )
//  {
//    return WMCDec_InValidArguments;
//  }
//  for(uint16 i=0; i<(int)m_nNumStreams; i++)
//  {
//    if( m_maxPlayableTime[i] && (m_pStreamDecodePattern[i].tPattern != Discard_WMC) )
//    {
//      if(!nMaxPlayableTime)
//      {
//        /* initialize with valid track sample time */
//        nMaxPlayableTime = m_maxPlayableTime[i];
//        continue;
//      }
//      /* Take the MIN value to make sure all tracks are playable atleast nMaxPlayableTime */
//      nMaxPlayableTime = MIN(m_maxPlayableTime[i],nMaxPlayableTime);
//    }
//  }
//
//    *nMaxPBTime = nMaxPlayableTime;
//
//  return WMCDec_Succeeded;
//}
#endif //  FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
/* ======================================================================
FUNCTION:
  AVIFile::isADTSHeader()

DESCRIPTION:
  Check if audio has ADTS header

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
 TRUE or FALSE

SIDE EFFECTS:
  None.
======================================================================*/
bool AVIFile::isADTSHeader()
{
  CHUNK_t chunkType;
  for(int id = 0; id < m_nNumStreams; id++)
  {
    // Get the chunk type corresponding to track number.
    m_pAVIParser->GetTrackChunkType(id,&chunkType);

    if(chunkType == AVI_CHUNK_AUDIO)
    {
      avi_audiotrack_summary_info audioSummaryInfo;
      avi_audio_info aInfo ;
      if ( (m_pAVIParser->GetAudioTrackSummaryInfo(id,&audioSummaryInfo)==AVI_SUCCESS) &&
           (m_pAVIParser->GetAudioInfo(id,&aInfo)==AVI_SUCCESS) &&
           ((aInfo.strfAudio.wFormatTag == AVI_AUDIO_AAC) ||(aInfo.strfAudio.wFormatTag == AVI_AUDIO_AAC_FAAD) ))
      {
        //Get the first audio sample,check if it has ADTS header
        uint8* firstSampleBuf = (uint8*)QTV_Malloc(5000);
        uint32 index =0;
        bool bError;

        if(firstSampleBuf)
        {
          getNextMediaSample(id, firstSampleBuf, 5000, index);
          if((firstSampleBuf[0] == 0xFF) && (firstSampleBuf[1]&0xF0 == 0xF0))
          {
            return true;
          }
          else
          {
            return false;
          }
          resetPlayback(0, id, false, &bError, 1);
          QTV_Free(firstSampleBuf);
        }
      }
    }
  }
}
#endif /* FEATURE_QTV_AVI */
