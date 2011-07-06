/* =======================================================================
                              videoDec.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.

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

$Header: //source/qcom/qct/multimedia/qtv/player/videoplayer/main/latest/src/videodec.cpp#78 $
$DateTime: 2010/11/12 04:31:46 $
$Change: 1515318 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

#ifndef PLATFORM_LTK /* no timer services on LTK */
extern "C"
{
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "timer.h"
}
#endif /* PLATFORM_LTK */

#include "qcplayer.h"
#include "Video.h"

#include "Mpeg4Player.h"

#include "m4v_io.h"
#include "AVSync.h"
extern "C" {
#include "assert.h"
#include "err.h"
#include "mp4dec_api.h"
}

#include "event.h"
#include "qtv_event.h"

#include "AEE_OEMHeap.h"
#include "OEMHeap.h"
#include "vdecoder_utils.h"
#include "video_decoder_broadcast.h"
#include "qtv_vrend.h"

#include "StreamMedia.h"
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

// debug support
//Set test switches here...
// undef these for normal mode.
// #define SHOW_FRAMES
// #define SHOW_SYNC
// #define SHOW_RENDER
// #define SHOW_TIMING
// #define SHOW_LATE
#define SHOW_FRAME_REQUEST_COUNT
#define SHOW_FRAME_QUEUES

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

extern qtv_event_clip_started_type qtv_event_clip_started_payload;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

//Parameters for the drop-before-display function

// This enables dropping frames from being rendered
static bool bEnableDropBeforeDisplay = true;

// This is the size of the sliding window used for Lip Sync in msec.
const int  VideoDec::DISPLAY_THRESH_LOCAL_MSEC = 250;
const int  VideoDec::DISPLAY_THRESH_STREAM_MSEC = 250;
static const bool bForceBaseLayerIFrameDisplay = true;
//Time to wait for Suspend Sync(worst case)
#define SUSPEND_TIMEOUT_MSEC 10000

#ifdef FEATURE_QTV_M4V
#error code not present
#endif

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
extern "C" boolean qtv_cfg_enable_ts;
#endif /* FEATURE_MP4_TEMPORAL_SCALABILITY */

const uint64 UNDEFINED_TIMESTAMP = 0x0000000100000000ULL ;
const uint64 TIMESTAMP_MASK =  0x0FFFFFFFFULL ;
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

////////////////////////////////////////////////////////////////////
//
// create/destroy
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoDec::Create

DESCRIPTION:
  Creates the video

INPUT/OUPUT PARAMETERS:
  player

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::Create
(
void                           *player,
QtvPlayer::MallocOutputBufferT  MallocOutputBuffer,
QtvPlayer::FreeOutputBufferT    FreeOutputBuffer,
QTV_BroadcastTransmitter* const broadcaster_ptr,
AVSync* pAVSync
)
{
  m_broadcaster_ptr = broadcaster_ptr;

  stopTime.bStop=false;
  repos.bUpdate=false;
  repos.bRendering=true;

  pPlayer=(VideoPlayer *)player;
  m_Malloc = MallocOutputBuffer;
  m_Free   = FreeOutputBuffer;
  m_pAVSync = pAVSync;

  InitVideoInfo();
  InitPlayData();
#ifndef PLATFORM_LTK
#ifdef FEATURE_QTV_AVI
  m_pVideoDelayAnalyzer = QTV_New_Args( QTV_VideoDelayAnalyzer, 
                                        ( 3 , pPlayer->pAVPlayer));
#else
  m_pVideoDelayAnalyzer = QTV_New_Args( QTV_VideoDelayAnalyzer, 
                                        ( 3 ));
#endif
#endif // PLATFORM_LTK

#ifdef FEATURE_FRE
  m_bEnableQFRE.bFreEnable = pPlayer->m_pQtvConfigObject->GetQTVConfigItem(QtvConfig::QTVCONFIG_ENABLE_QFRE);
#endif

}

/* ======================================================================
FUNCTION:
  VideoDec::Destroy

DESCRIPTION:
  Cleans up

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::Destroy()
{
  if (NULL != m_pVideoDelayAnalyzer)
  {
    QTV_Delete( m_pVideoDelayAnalyzer );
    m_pVideoDelayAnalyzer = NULL;
  }

  CleanupPlay();
  pPlayer=NULL;
}


////////////////////////////////////////////////////////////////////
//
// data init & cleanup
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoDec::InitDataState

DESCRIPTION:
  Initializes the data state.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::InitDataState()
{
  //set data state to allow reading data.
  for (int i=0;i<COMMON_MAX_LAYERS;i++)
  {
    dataState[i]=Media::DATA_OK;
  }
  overallDataState=Media::DATA_OK;
}

/* ======================================================================
FUNCTION:
  VideoDec::InitDataState

DESCRIPTION:
  Initializes the frames.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::InitFrame()
{
  QCUtils::EnterCritSect(&frameReqUpdate_CS);
  nFramesProcessed=0;
  nFramesRendered=0;
  QCUtils::LeaveCritSect(&frameReqUpdate_CS);

  bFirstFrameRendered = false;
  bFirstIFrameRendered = false;

}

/* ======================================================================
FUNCTION:
  VideoDec::InitPlayData

DESCRIPTION:
  Initializes data once per playback

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::InitPlayData()
{
  //once per play
  InitFrame();

  //default: start rendering on first decode frame.
  repos.bRendering=false;
  repos.nRenderPos=0;
  (void)Set_State( VIDEODEC_BUSY );
  overallDataState = Media::DATA_INIT; // jg

  // clear flags
  stopH263VideoInit = false;
  QCUtils::ResetCondition(&m_suspendSync);
  InitStatistics();

  m_last_timestamp = 0;
  m_uLastTimestampBeforeRebuffering = 0;
  bIsFrameErrorThresholdMet = false;

  //initialized to false
  isbFirstFrameDecoded = false;
}

/* ======================================================================
FUNCTION:
  VideoDec::InitStatistics

DESCRIPTION:
  Initializes statistics.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::InitStatistics()
{
  QCUtils::EnterCritSect(&videoStat.CS);

  videoStat.nFailedDecode=0;
  videoStat.nFramesInterpolated = 0;
  videoStat.nIFrameTally = 0;
  videoStat.nPFrameTally = 0;
  videoStat.nBFrameTally = 0;
  videoStat.nMaxChunk=0;
  videoStat.nDroppedConvert=0;
  videoStat.nDroppedDecode=0;
  videoStat.nDroppedAVSync = 0;  /*Dropped due to AV Sync*/
  videoStat.nDroppedDisplay=0;
  videoStat.nDropMacroBlockErrs = 0;
  videoStat.nCurrentChunkDecode=0;
  videoStat.nCurrentChunkDisplay=0;
  videoStat.lastFrame.bValid=false;
  videoStat.lastFrame.nFrame=0;
  videoStat.lastFrame.nTime=0;
  videoStat.lastFrame.nTimestamp=0;
  videoStat.nStopPresTime=0;

  videoStat.nPlayMsec = 0;
  videoStat.nFPSProcessed = 0;
  videoStat.nFPSRendered = 0;
  videoStat.nMaxBehindMsec = 0;
  videoStat.nMaxAheadMsec = 0;
  videoStat.nFirstTimestamp = videoStat.nLastTimestamp = (-1);//unknown.

  videoStat.prevFrameTimestampMsec = 0;
  videoStat.averageInterFrameIntervalMsec = 0.0f;

  QCUtils::LeaveCritSect(&videoStat.CS);
}

/* ======================================================================
FUNCTION:
  VideoDec::ResetData

DESCRIPTION:
  Initializes reported data

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::ResetData()
{
  InitPlayData();
}

/* ======================================================================
FUNCTION:
  VideoDec::CleanupPlay

DESCRIPTION:
  Cleanup after playback

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::CleanupPlay(void)
{
  //Free memory
  for (int idx = 0; idx < COMMON_MAX_LAYERS; idx++)
  {
    if (decodeBuf.pData[idx])
    {
      QTV_Free(decodeBuf.pData[idx]);
      decodeBuf.pData[idx] = NULL;
    }
  }

  //Free H.264 blob if we terminate before playing
#ifdef FEATURE_H264_DECODER
  if (ppHeaderBlob)
  {
    for (int i = 0; i < videoInfo.nLayers; ++i)
    {
      vdec_free_blob( ppHeaderBlob[ i ] );
    }
    QTV_Free( ppHeaderBlob );
    ppHeaderBlob = NULL;
  }
#endif /* FEATURE_H264_DECODER */

  //cleanup decoder if it's been initialized.
  overallDataState=Media::DATA_END;

  CleanupDecoder();

  //Clear the media pointers.
  pMpeg4=NULL;
  pBitfile=NULL;
}

/* ======================================================================
FUNCTION:
  VideoDec::CleanupDecoder

DESCRIPTION:
  Cleanup the decoder

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::CleanupDecoder()
{
  static const unsigned long DECODE_DONE_POLL_INTERVAL = 200;
  int timeout=0;

  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoDec::CleanupDecoder" );

  // Wait for the final decode frame callback(s) before resetting the decoder
  // while ( frameRequestCount > 1 )
  while ((overallDataState==Media::DATA_OK) &&
         (_state == VIDEODEC_BUSY))
  {
    rex_sleep( DECODE_DONE_POLL_INTERVAL );
    timeout += DECODE_DONE_POLL_INTERVAL;
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "Waited %d ms for %d DecodeDone(s)",
                  timeout,
                  GetOutstandingFrameCount() );
  }

#ifdef SHOW_FRAME_REQUEST_COUNT
  QTV_MSG1( QTVDIAG_VIDEO_TASK,
            "frameRequestCount: %d",
            GetOutstandingFrameCount() );
#endif

  // This does the mp4_terminate( )
  (void)DoGenericVdecShutdown( );

  // Clean up accounting used while the decoder was running.
  (void)AlterOutstandingFrameCount( -1 * GetOutstandingFrameCount(),
                                    "CleanupDecoder" );

}


/* ======================================================================
FUNCTION:
  VideoDec::InitDecodeBuffer

DESCRIPTION:
  Initalizes the decode buffer

INPUT/OUPUT PARAMETERS:
  bPlayRestart

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::InitDecodeBuffer(bool bPlayRestart)
{
  InitDataState();

  //If restarting and buffers have been allocated then just discard current buffer data.
  //This catches the case where a restart occured before play actually started.
  if ((bPlayRestart && (decodeBuf.pData[0] != NULL))
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
     )
  {
    DiscardDecodeBuf();
    return true;
  }
  else
  {
    // Get max decode buf sizes
    int idx;

    bool bMp4Error = false;

    for (idx = 0; ((idx < videoInfo.nLayers) && (idx < COMMON_MAX_LAYERS)); idx++)
    {
      decodeBuf.maxSize[idx] = 0;
      if (pMpeg4)
      {

        if (pPlayer->pAVPlayer->isStreaming())
        {
          decodeBuf.maxSize[idx] = pMpeg4->GetMaxVideoBufferSizeDB(idx);
        }
        else
        {
          decodeBuf.maxSize[idx] = pMpeg4->GetMaxVideoBufferSizeDB(idx);
        }
        if (pMpeg4->CheckError(Common::GETMAXVIDEO)) bMp4Error=true;
      }
#ifdef FEATURE_QTV_M4V
#error code not present
#endif
    }

    // Error check
    if (bMp4Error)
    {
      overallDataState=Media::DATA_END;
      return false;
    }

    // Allocate decoding input buffers
    for (idx = 0; idx < COMMON_MAX_LAYERS; idx++)
    {
      if (decodeBuf.pData[idx]!=NULL)
      {
        QTV_Free(decodeBuf.pData[idx]);
        decodeBuf.pData[idx]=NULL;
      }
    }
    for (idx = 0; idx < videoInfo.nLayers; idx++)
    {
      decodeBuf.size[idx] = 0;
      decodeBuf.bUseAgain[idx] = false;
      decodeBuf.pos[idx] = 0;
      if (decodeBuf.maxSize[idx]>0)
      {
        decodeBuf.pData[idx] = (uint8*)QTV_Malloc(decodeBuf.maxSize[idx]);
      }
      if (decodeBuf.pData[idx] == NULL)
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "MEMORY ERROR: Malloc Failed %d Bytes", decodeBuf.maxSize[idx] );
        overallDataState=Media::DATA_END;
        return false;
      }
    }
  }
  return true;
}

/* ======================================================================
FUNCTION:
  VideoDec::StartPlaying
DESCRIPTION:
  Init for playback.  Assume prep was already done.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::StartPlaying(void)
{
  ////////////////////////
  //Init data
  ////////////////////////


  //in case Prep was not done, this will catch the error.
  if (!pMpeg4 && !pBitfile)
  {
    return false;
  }
  /* this code to get the frame rate is used to see if drift compensation for the arm clock
      must be applied in 6250 */
#ifdef FEATURE_QTV_6250_ARM_CLK_COMPENSATION

#ifdef FEATURE_QTV_VIDEO_EDITING
  if (pMpeg4 != NULL)
  {
    uint32 videoTrackID = pMpeg4->GetVideoTrackID(0);
    switch (pMpeg4->GetSource())
    {
      case Media::FILE_SOURCE:
        frameRate = pMpeg4->GetFile()->getTrackVideoFrameRate(videoTrackID);
        break;
      case Media::STREAM_SOURCE:
      {
        frameRate = pMpeg4->GetVideoFrameRate(0);
        break;
      }
      default:
        break;
    }
  }
#endif  /* FEATURE_QTV_VIDEO_EDITING */
#endif

  /////////////////////////////////////////////////////////////////////////////
  // Limit image size by maximum width the DSP can handle and image resolution
  /////////////////////////////////////////////////////////////////////////////

  if ((videoInfo.originalWidth > MP4_MAX_DSP_WIDTH) ||
      (videoInfo.originalHeight * videoInfo.originalWidth ) >
      (MP4_MAX_DECODE_WIDTH * MP4_MAX_DECODE_HEIGHT))
  {
    QTV_MSG_PRIO4(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Image too large: (%d,%d) > (%d,%d)",
                  videoInfo.originalWidth, videoInfo.originalHeight,
                  MP4_MAX_DECODE_WIDTH, MP4_MAX_DECODE_HEIGHT);
    overallDataState = Media::DATA_END;
    return false;
  }

  return true;
}

/* ======================================================================
FUNCTION:
  VideoDec::InitVideoInfo

DESCRIPTION:
  Initializes video info

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::InitVideoInfo()
{
  videoInfo.nLayers = 0;
  videoInfo.imageHeight = 0;
  videoInfo.imageWidth = 0;
  videoInfo.videoCodec = Media::MPEG4_CODEC;


  if ((pMpeg4) && (pPlayer))
  {
    pMpeg4->SetVideoPlayerData(pPlayer);
  }
}

/* ======================================================================
FUNCTION:
  VideoDec::NewClip

DESCRIPTION:
  Handles a new clip

INPUT/OUPUT PARAMETERS:
  pMpeg4In
  pBitfileIn

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::NewClip(Media * pMpeg4In, OSCL_FILE * pBitfileIn)
{
  //save file pointers.
  pMpeg4=pMpeg4In;
  pBitfile=pBitfileIn;
  if (pMpeg4==NULL && pBitfile==NULL)
  {
    return false;
  }

  InitVideoInfo();

  // Get N layers & video codec.

  bool bMp4Error = false;
  bRebufferHappened = true;   //treat fresh-start as rebuffer
  videoInfo.nLayers = 0;

  if (pMpeg4)
  {
    videoInfo.videoCodec = pMpeg4->GetVideoCodecType();
    videoInfo.nLayers = pMpeg4->GetNumberOfVideoTracks();

    // get number of layers for paylaod of event CLIP_START
    pPlayer->pAVPlayer->qtv_event_clip_started_payload.num_layers = (uint8)videoInfo.nLayers;

    QTV_MSG1( QTVDIAG_VIDEO_TASK, "Video has %d Layers", videoInfo.nLayers);
#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
    if (!qtv_cfg_enable_ts)
#endif /* FEATURE_MP4_TEMPORAL_SCALABILITY */
    {
      if (videoInfo.videoCodec != Media::OSCAR_CODEC)
      {
        if (videoInfo.nLayers > 1)
        {
          QTV_MSG1( MSG_LEGACY_ERROR, "ts not supported, ignoring enh layers: %d", videoInfo.nLayers );
          videoInfo.nLayers = 1;
        }
        if (pMpeg4->CheckError(Common::GETSCALABILITY))
        {
          bMp4Error=true;
        }
      }
    }
#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
    else
    {
      if (videoInfo.videoCodec == Media::H264_CODEC)
      {
        if (videoInfo.nLayers > 1)
        {
          QTV_MSG1( MSG_LEGACY_ERROR, "ts not supported, ignoring enh layers: %d", videoInfo.nLayers );
          videoInfo.nLayers = 1;
        }
      }
    }
#endif

  }
  else if (pBitfile)
  {
    //assume m4v files are mpeg4 codec.
    videoInfo.videoCodec = Media::MPEG4_CODEC;
#ifdef FEATURE_QTV_M4V
#error code not present
#endif
  }

  //make sure number of layers doesn't exceed qc player limit
  if (videoInfo.nLayers > COMMON_MAX_LAYERS)
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: Too many layers %d, only using %d ",
                  videoInfo.nLayers, COMMON_MAX_LAYERS );
    videoInfo.nLayers=COMMON_MAX_LAYERS;
  }

  // Error check
  if (bMp4Error || videoInfo.nLayers==0)
  {
    return false;
  }
  return true;
}


#ifdef FEATURE_H264_DECODER
/* ======================================================================
FUNCTION:
  VideoDec::PrependNalLength

DESCRIPTION:
Prepend the NAL length to a header stream

INPUT/OUPUT PARAMETERS:
buf - pointer to the start of stream
size - size of the bitstream in bytes

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::PrependNalLength
(
uint8 * buf, uint32 size, uint32 sizeOfNalLengthField
)
{
  uint8* psize = (uint8*) &size;
  //Need to prepend a 4 byte integer to the bitstream signifying the size
  (void)memmove(buf + sizeOfNalLengthField, buf, size);
  //ARM is big-endian, and decoder expects size in little-endian
  for (int i = 0; i < sizeOfNalLengthField; i++)
  {
    buf[i] = psize[ sizeOfNalLengthField - 1 - i ];
  }
  return true;
}

/* ======================================================================
FUNCTION:
  VideoDec::PreParseHeader

DESCRIPTION:
Process H.264 header in advance

INPUT/OUPUT PARAMETERS:
  bRestart

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::PreParseHeader
(
Media* pMpeg4In,
bool bRestart,
Media::CodecType audioCodec
)
{
  long nBytes = 0;
  Media::MediaStatus status = Media::DATA_OK;
  pMpeg4 = pMpeg4In;
  bool bOK = true;
  QTV_USE_ARG1(nBytes);
  QTV_USE_ARG1(status);
  m_bPreParseHeaderStatus = false;

  // Set the audio codec type got from Mpeg4Player (media source)
  AudioMediaCodecTypeSet = audioCodec;
  
  // if we are restarting, we don't need to parse header again.
  if (bRestart)
  {
    QTV_MSG1( MSG_LEGACY_FATAL, "PreParseHeader() w/ bRestart: %d", bRestart);
    return false;
  }

  videoInfo.videoCodec = pMpeg4In->GetVideoCodecType();
  videoInfo.nLayers = pMpeg4In->GetNumberOfVideoTracks();
  if (videoInfo.videoCodec == Media::H264_CODEC)
  {
    if (videoInfo.nLayers > 1)
    {
      //TS not supported for H264.
      videoInfo.nLayers = 1;
    }
  }
  if (!InitDecodeBuffer(bRestart))
  {
    return false;
  }

  switch (videoInfo.videoCodec)
  {
    case Media::H264_CODEC:
      ////////////////////////////
      //Create the Decoder
      ///////////////////////////
      if (DoGenericVdecCreate() == false)
      {
        bOK = false;
        break;
      }

      ////////////////////////////////////
      //Initialise the decoder
      ////////////////////////////////////
      if (DoGenericVdecInit(NULL,pMpeg4In) == false)
      {
        bOK = false;
        break;
      }

      //Decode the stream headers for H264.
      bOK = DecodeStreamHeader_H264();

      break;

    default:
      break;
  }

  if (bOK == true)
  {
    m_bPreParseHeaderStatus = true;
  }
  pMpeg4 = NULL;
  return bOK;

}
#endif /* FEATURE_H264_DECODER */

/* ======================================================================
FUNCTION:
  VideoDec::PrepPlay

DESCRIPTION:
  Process vol header and extract video info

INPUT/OUPUT PARAMETERS:
  bCannotPrep
  pMpeg4In
  pBitfileIn
  bRestart

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::PrepPlay
(
bool &bCannotPrep,
Media * pMpeg4In,
OSCL_FILE * pBitfileIn,
bool bRestart,
Media::CodecType audioCodec
)
{
  bool bOK = true;
  bool bGotVideoDimensions = false;
  VDEC_PARAMETER_DATA parm_data;
  Media* pVideoMedia;

  bCannotPrep=false;

 // Set the audio codec type got from Mpeg4Player (media source)
  AudioMediaCodecTypeSet = audioCodec;

if((pPlayer != NULL) && (pPlayer->pAVPlayer != NULL))
 {
     pVideoMedia = pPlayer->pAVPlayer->GetVideoMedia();
 
     if( (pVideoMedia != NULL) && pPlayer->pAVPlayer->IsLocalMediaPLayback())
     {
       if (pVideoMedia->isAviFileInstance() &&
          (videoInfo.videoCodec == Media::MPEG4_CODEC))
       {
          SetPrerollRendererCount(2);
       }
     }
      pVideoMedia = NULL;
 }
  
  // if we are restarting, we don't need to initialize decoder again.
  if (bRestart)
  {
    if (GetDataState()==Media::DATA_END && (!pPlayer->bPlayDoneRendering))
    {
      int videoLayerCount;
      for (videoLayerCount=0; videoLayerCount < videoInfo.nLayers; videoLayerCount++)
      {
        if (dataState[videoLayerCount] == Media::DATA_END)
        {
          dataState[videoLayerCount] = Media::DATA_OK;
        }
      }
      pPlayer->bPlayDoneDecoding = false;
      overallDataState = Media::DATA_OK;
    }
    if (!pPlayer->HasAudio())
    {
      bRebufferHappened = true;
    }
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PrepPlay() w/ bRestart, overallDataState=%d,rebuffer=%d",
                  overallDataState ,bRebufferHappened);
    return true;
  }
  else
  {
    SetLipSyncWindowSize(pMpeg4In);  

    //prep for new clip
    if (!NewClip(pMpeg4In,pBitfileIn)) /* pMpeg4 gets set here */
    {
      bOK = false;
    }

    if (bOK == true)
    {
#ifdef FEATURE_QTV_6250_ARM_CLK_COMPENSATION
      SetDriftConstantValue(audioCodec);
#endif
      //Init data for new playback.
      InitPlayData();

      ////////////////////////
      //Init decode buf
      ////////////////////////
      if (!InitDecodeBuffer(bRestart))
      {
        bCannotPrep=true;
        bOK = false;
      }
    }
    // The media object knows our frame dimensions.
    //
    // However, we're ignoring the return code from this function.
    // In 'failure' cases, when the media object doesn't know the video
    // dimensions, it returns 'safe guess' values which we can use just fine.
    //
    // Decoders which want to override this setting (to save memory) can
    // do so when they allocate frame buffers.
    //
    // We'll pick up the correct video dimensions when the first decoded
    // frame is returned, and adjust videoInfo accordingly.
    //
    // After this call, we've either got the right video dimensions,
    // or a good enough guess to get us through to the first decoded frame,
    // at which point we'll have the right video dimensions, so the result
    // code from this funtion doesn't matter.
    //

    bGotVideoDimensions = pMpeg4->GetVideoDimensions( (uint32 *) &videoInfo.originalWidth,
                                                      (uint32 *) &videoInfo.originalHeight );

    if ( !bGotVideoDimensions )
    {
      // Default VDEC_MAX_DECODE dimensions if we can't get the video
      // dimensions.
      videoInfo.originalWidth = 0;
      videoInfo.originalHeight = 0;
    }

    if((videoInfo.videoCodec != Media::WMV3_CODEC) 
        && (videoInfo.videoCodec != Media::WMV2_CODEC) 
        && (videoInfo.videoCodec != Media::WMV1_CODEC)
        && (videoInfo.videoCodec != Media::JPEG_CODEC))
    {
      videoInfo.imageWidth = MP4_MAX_DECODE_WIDTH;
      videoInfo.imageHeight = MP4_MAX_DECODE_HEIGHT;
    }
    else
    { 
	  if( videoInfo.originalWidth == 0 && videoInfo.originalHeight == 0)
	  {
		videoInfo.imageWidth = MP4_MAX_DECODE_WIDTH;
		videoInfo.imageHeight = MP4_MAX_DECODE_HEIGHT;		
	  }
	  else
	  {
		videoInfo.imageWidth  = videoInfo.originalWidth;
		videoInfo.imageHeight = videoInfo.originalHeight;
	  }
    }
    if (videoInfo.videoCodec != Media::H264_CODEC && bOK == true)
    {
      ////////////////////////////
      //Create the Decoder
      ///////////////////////////
      if (bOK == true)
      {
        bOK = DoGenericVdecCreate();
      }

      ////////////////////////////////////
      //Initialise the decoder
      ////////////////////////////////////
      if (bOK == true)
      {
        bOK = DoGenericVdecInit(NULL,pMpeg4In);
      }
    }

    ////////////////////////////////
    // Get VOL headers
    ////////////////////////////////
    if (bOK == true)
    {
      switch (videoInfo.videoCodec)
      {
        case Media::H264_CODEC:
          if((pPlayer != NULL) && (pPlayer->pAVPlayer != NULL))
          {
            pVideoMedia = pPlayer->pAVPlayer->GetVideoMedia();

            if((pVideoMedia && pVideoMedia->isAviFileInstance()))
            {
#ifdef FEATURE_H264_DECODER
              parm_data.startCodeDetection.bStartCodeDetectionEnable = TRUE;
#endif
              vdec_set_parameter(m_vdec,VDEC_PARM_START_CODE_DETECTION,&parm_data);
            }
            else
            {
#ifdef FEATURE_H264_DECODER
              parm_data.sizeOfNalLengthField.sizeOfNalLengthField = pMpeg4->GetSizeOfNALLengthField(0);
#endif
              vdec_set_parameter(m_vdec,VDEC_PARM_SIZE_OF_NAL_LENGTH_FIELD,&parm_data);
            }
          }
          //H264 stream header is parsed in PB_Ready state.
          if (m_bPreParseHeaderStatus == false)
          {
            bOK = false;
          }
          break;
        case Media::H263_CODEC:
		case Media::STILL_IMAGE_H263_CODEC:		
          //There is no header info to decode so we will do nothing.
#ifdef FEATURE_QTV_OSCAR_DECODER
#error code not present
#endif  /* FEATURE_QTV_OSCAR_DECODER */
#ifdef FEATURE_QTV_SKT_MOD
        case Media::JPEG_CODEC:
          //There is no header info to decode so we will do nothing.
#endif /* FEATURE_QTV_SKT_MOD */	 
          bOK = true;     
          break;
#ifdef FEATURE_DIVX_311_ENABLE
        case Media::DIVX311_CODEC:
          bOK = true;
          break;
#endif
        case Media::MPEG4_CODEC:
        case Media::STILL_IMAGE_CODEC:
          bOK = DecodeStreamHeader_MP4();
          break;

#ifdef FEATURE_QTV_WINDOWS_MEDIA
          /* For WMV7 no need to decode the stream header */
        case Media::WMV1_CODEC:
          break;
        case Media::WMV3_CODEC:
        case Media::WMV2_CODEC:
          bOK = DecodeStreamHeader_WMV();
          break;
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

        default:
          QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,"VideoDec::Prepplay failed!! error - unknown Codec type");
          bOK = false;
      }
    }
  }
  return bOK;
}

/* ======================================================================
FUNCTION:
  VideoDec::DecodeStreamHeader_MP4

DESCRIPTION:
  Process vol header and extract video info, MP4-decoder specific.

INPUT/OUPUT PARAMETERS:
  none

RETURN VALUE:
  true  - if everything works fine.
  false - in case something goes wrong.

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::DecodeStreamHeader_MP4( void )
{
  int idx;
  unsigned char *pvolData = NULL;
  unsigned int volSize=0;
  VDEC_ERROR err_status = VDEC_ERR_EVERYTHING_FINE;
  bool         bOK = true;

  if (pMpeg4)
  {
    for (idx = 0; idx < videoInfo.nLayers; idx++)
    {
      pvolData = pMpeg4->GetVOLHeader(idx,(int &)volSize);
      if (pvolData==NULL)
      {
        bOK = false;
        break;
      }

      memcpy(decodeBuf.pData[idx],pvolData,volSize);
      decodeBuf.size[idx] = volSize;
    }

    if (bOK == true)
    {
      m_pVDEC_InputBuffer->layers = videoInfo.nLayers;
      for (int i = 0; i < m_pVDEC_InputBuffer->layers; ++i)
      {
        m_pVDEC_InputBuffer->buffer     [ i ] = decodeBuf.pData    [ i ];
        m_pVDEC_InputBuffer->buffer_size[ i ] = decodeBuf.size     [ i ];
        m_pVDEC_InputBuffer->timestamp  [ i ] = decodeBuf.timestamp[ i ];
        m_pVDEC_InputBuffer->delta      [ i ] = decodeBuf.delta    [ i ];
        m_pVDEC_InputBuffer->buffer_pos [ i ] = decodeBuf.pos      [ i ];
        m_pVDEC_InputBuffer->userData   [ i ] = decodeBuf.userdata [ i ];
      }

      err_status = vdec_queue(m_vdec,
                              m_pVDEC_InputBuffer,
                              InputBufferFreeCallback,
                              this);
      switch (err_status)
      {
        case VDEC_ERR_EVERYTHING_FINE:
          (void)AlterOutstandingFrameCount( m_pVDEC_InputBuffer->layers,
                                            "initial worst-case - queuing Header data" );
          bOK = true;
          break;

        case VDEC_ERR_NO_INPUT_AVAILABLE:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                        "vdec_queue: %d. No Input available.", err_status );
          bOK = false;
          break;

        case VDEC_ERR_OUT_OF_BUFFERS:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                        "vdec_queue: %d. Out of buffers.", err_status );
          bOK = false;
          break;

        default:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                        "Failure in vdec_queue: %d", err_status );
          bOK = false;
      }

      if (bOK == true)
      {
        err_status = vdec_decode( m_vdec );
        last_vdec_err = err_status;
        switch (err_status)
        {
          case VDEC_ERR_EVERYTHING_FINE:
            bOK = true;
            break;

          case VDEC_ERR_RESOURCE_UNAVAILABLE:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                          "Failure in vdec_decode: %d - Resource unavailable", err_status );
            bOK = false;
            break;

          case VDEC_ERR_OUT_OF_MEMORY:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                          "Failure in vdec_decode: %d - Out of memory", err_status );
            bOK = false;
            break;

          case VDEC_ERR_INVALID_INPUT:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                          "Failure in vdec_decode (VDEC_ERR_INVALID_INPUT): %d", err_status );
            bOK = false;
            break;

          default:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                          "Failure in vdec_decode: %d", err_status );
            bOK = false;
        }
      }
    }
  }
  else if (pBitfile)
  {
#ifdef FEATURE_QTV_M4V
#error code not present
#endif
  }
  return bOK;
}

/* ======================================================================
FUNCTION:
  VideoDec::DecodeStreamHeader_H264

DESCRIPTION:
  Process SPS and PPS data present in H264 clip.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  true  - if everything works fine.
  false - in case something goes wrong.

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::DecodeStreamHeader_H264(void)
{
  Media::MediaStatus status[COMMON_MAX_LAYERS];
  VDEC_ERROR err_status (VDEC_ERR_EVERYTHING_FINE ); 
  long nBytes = 0;
  bool bOK = true;
  bool bPSFound = false;

  for (int idx = 0; idx < videoInfo.nLayers; idx++)
  {
    decodeBuf.size[idx] = 0;
    uint8 *buf_pos = decodeBuf.pData[idx];
 #ifdef FEATURE_H264_DECODER
    // reset parameter set index
    (void)pMpeg4->ResetParamSetNAL(idx);

    //initialize status[idx]
    status[idx] = Media::DATA_OK;

    while (true)
    {
      nBytes = pMpeg4->GetNextParamSetNAL( status[idx],
                                           idx,
                                           buf_pos,
                                           decodeBuf.maxSize[idx] );
      if (status[idx] != Media::DATA_OK)
      {
        //If there was an error getting the param set,
        //  or if we have all of them
        break;
      }

      if (pMpeg4->GetH264StartCodeDetectFlag(0))
      {
        // remove SC in SPS/PPS
        nBytes -= H264_NAL_LENGTH_SIZE;
        (void)memmove(buf_pos, buf_pos + H264_NAL_LENGTH_SIZE, nBytes);
      }

      if (nBytes != 0)
      {
        bPSFound = true;
        //We need to prepend NAL length if it is not there so the decoder
        // can separate pic and seq headers
        //Make sure that we have enough memory to Prepend the NAL Length.
        ASSERT( (uint32)decodeBuf.size[idx] + H264_NAL_LENGTH_SIZE <
                decodeBuf.maxSize[idx] );

        (void)PrependNalLength(buf_pos, nBytes,H264_NAL_LENGTH_SIZE);
        //The size of the nal length field is 4
        decodeBuf.size[idx] += nBytes + H264_NAL_LENGTH_SIZE;
        buf_pos += nBytes + H264_NAL_LENGTH_SIZE;
      }
    }//while
 #else
   // Since local variables nBytes and buf_pos are being used only if 
   // feature FEATURE_H264_DECODER is definesd so assign it to 
   // dummy variables just to curb any compiler warnings
   QTV_USE_ARG1(nBytes);
   QTV_USE_ARG1(buf_pos);

 #endif

    if (status[idx] != Media::DATA_END)
    {
      //There has been a failure in getting the NAL data!!
      bOK = false;
      break;
    }
  }//for

  if (bOK == true && bPSFound)
  {
    m_pVDEC_InputBuffer->layers = videoInfo.nLayers;
    for (int i = 0; i < m_pVDEC_InputBuffer->layers; ++i)
    {
      m_pVDEC_InputBuffer->buffer     [ i ] = decodeBuf.pData    [ i ];
      m_pVDEC_InputBuffer->buffer_size[ i ] = decodeBuf.size     [ i ];
      m_pVDEC_InputBuffer->timestamp  [ i ] = decodeBuf.timestamp[ i ];
      m_pVDEC_InputBuffer->buffer_pos [ i ] = decodeBuf.pos      [ i ];
      m_pVDEC_InputBuffer->userData   [ i ] = decodeBuf.userdata [ i ];
    }

    err_status = vdec_queue(m_vdec,
                            m_pVDEC_InputBuffer,
                            InputBufferFreeCallback,
                            this);
    switch (err_status)
    {
      case VDEC_ERR_EVERYTHING_FINE:
        (void)AlterOutstandingFrameCount( m_pVDEC_InputBuffer->layers,
                                          "initial worst-case - queuing Header data" );
        bOK = true;
        break;

      case VDEC_ERR_NO_INPUT_AVAILABLE:
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                      "vdec_queue: %d. No Input available.", err_status );
        bOK = false;
        break;

      case VDEC_ERR_OUT_OF_BUFFERS:
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                      "vdec_queue: %d. Out of buffers.", err_status );
        bOK = false;
        break;

      default:
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                      "Failure in vdec_queue: %d", err_status );
        bOK = false;
    }

    if (bOK == true)
    {
      err_status = vdec_decode( m_vdec );
      last_vdec_err = err_status;
      switch (err_status)
      {
        case VDEC_ERR_EVERYTHING_FINE:
          bOK = true;
          break;

        case VDEC_ERR_RESOURCE_UNAVAILABLE:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                        "Failure in vdec_decode: %d - Resource unavailable", err_status );
          bOK = false;
          break;

        case VDEC_ERR_OUT_OF_MEMORY:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                        "Failure in vdec_decode: %d - Out of memory", err_status );
          bOK = false;
          break;

        case VDEC_ERR_INVALID_INPUT:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                    "Failure in vdec_decode (VDEC_ERR_INVALID_INPUT): %d", err_status );
          bOK = false;
          break;

        default:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, 
                        "Failure in vdec_decode: %d", err_status );
          bOK = false;
      }
    }
  }
  return bOK;
}

/* ======================================================================
FUNCTION:
  VideoDec::DecodeStreamHeader_WMV

DESCRIPTION:
  Process WMV headers.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  true  - if everything works fine.
  false - in case something goes wrong.

SIDE EFFECTS:
  None.
========================================================================== */
bool  VideoDec::DecodeStreamHeader_WMV (void)
{
  unsigned char *pvolData = NULL;
  VDEC_ERROR err_status (VDEC_ERR_EVERYTHING_FINE ); 
  int size;
  bool bOK = true;

  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoDec::PrepPlay_WMV" );

  for (int idx = 0; idx < videoInfo.nLayers; ++idx)
  {
    pvolData = pMpeg4->GetVOLHeader( idx, size );

    if (pvolData == NULL)
    {
      bOK = false;
      break;
    }

    memcpy(decodeBuf.pData[idx],pvolData,size);
    decodeBuf.size[idx] = size;
  }

  if (bOK == true)
  {
    m_pVDEC_InputBuffer->layers = videoInfo.nLayers;
    for (int i = 0; i < m_pVDEC_InputBuffer->layers; ++i)
    {
      m_pVDEC_InputBuffer->buffer     [ i ] = decodeBuf.pData    [ i ];
      m_pVDEC_InputBuffer->buffer_size[ i ] = decodeBuf.size     [ i ];
      m_pVDEC_InputBuffer->timestamp  [ i ] = decodeBuf.timestamp[ i ];
      m_pVDEC_InputBuffer->buffer_pos [ i ] = decodeBuf.pos      [ i ];
      m_pVDEC_InputBuffer->userData   [ i ] = decodeBuf.userdata [ i ];
    }

    err_status = vdec_queue(m_vdec,
                            m_pVDEC_InputBuffer,
                            InputBufferFreeCallback,
                            this);
    switch (err_status)
    {
      case VDEC_ERR_EVERYTHING_FINE:
        (void)AlterOutstandingFrameCount( m_pVDEC_InputBuffer->layers,
                                          "initial worst-case - queuing Header data" );
        bOK = true;
        break;

      case VDEC_ERR_NO_INPUT_AVAILABLE:
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                      "vdec_queue: %d. No Input available.", err_status );
        bOK = false;
        break;

      case VDEC_ERR_OUT_OF_BUFFERS:
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                      "vdec_queue: %d. Out of buffers.", err_status );
        bOK = false;
        break;

      default:
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                      "Failure in vdec_queue: %d", err_status );
        bOK = false;
    }

    if (bOK == true)
    {
      err_status = vdec_decode( m_vdec );
      last_vdec_err = err_status;
      switch (err_status)
      {
        case VDEC_ERR_EVERYTHING_FINE:
          bOK = true;
          break;

        case VDEC_ERR_RESOURCE_UNAVAILABLE:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                        "Failure in vdec_decode: %d - Resource unavailable", err_status );
          bOK = false;
          break;

        case VDEC_ERR_OUT_OF_MEMORY:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                        "Failure in vdec_decode: %d - Out of memory", err_status );
          bOK = false;
          break;

        case VDEC_ERR_INVALID_INPUT:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                    "Failure in vdec_decode (VDEC_ERR_INVALID_INPUT): %d", err_status );
          bOK = false;
          break;
		  
        default:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                        "Failure in vdec_decode: %d", err_status );
          bOK = false;
      }
    }
  }
  return bOK;
}

////////////////////////////////////////////////////////////////////
//
// decode buffer access
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoDec::RefillDecodeBuf

DESCRIPTION:
  Refill decode buffer for all layers.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::RefillDecodeBuf()
{
  bool   avsyncTimestampSet = false;

  ASSERT( videoInfo.nLayers <= MAX_MP4_LAYERS );

  for (uint32 layer = 0; (layer < videoInfo.nLayers) && (layer < COMMON_MAX_LAYERS); layer++)
  {
    RefillDecodeBuf(layer);

    if (bRebufferHappened && dataState[layer] == Media::DATA_OK)
    {
      if (!avsyncTimestampSet)
      {
        avsyncTimestampSet = true;
      }
    }

    if (dataState[layer] != Media::DATA_OK)
    {
      //see if the global state should change from OK to an end condition.
      if ((overallDataState == Media::DATA_OK) ||
          (overallDataState == Media::DATA_FRAGMENT) ||
          (overallDataState == Media::DATA_REALLOCATE_BUFFER) ||
          (overallDataState == Media::DATA_INSUFFICIENT))
      {
        //see if any other layers are still OK.
        bool bOK=false;
        for (int i=0;i<videoInfo.nLayers && (!bOK);i++)
        {
          if (dataState[i]==Media::DATA_OK)
          {
            bOK=true;
          }
        }
        //if no layers are OK, then set the global state to
        //the layer state.
        if (!bOK)
        {
          /* if we hit end of media, inform player about this, but we want to
             inform only once, but don't change video player state, since we are
             still playing pending frames, after playing all the video frames,
             we will send VIDEO_DONE status
          */
          if (dataState[layer] != overallDataState)
          {
            if (dataState[layer] == Media::DATA_END)
            {
              pPlayer->Notify(pPlayer->GetState(), Common::VIDEO_MEDIA_END);
            }
            overallDataState=dataState[layer];
          }
        }
      }
    }
  }

  if (avsyncTimestampSet)
  {
    // for the case where a video-only clip rebuffers/fresh-start we need
    // to set the clock offset
    if (!pPlayer->HasAudio())
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "INFO: Video Rebuffering caused timeadjust" );
      if (m_pAVSync)
      {
        //Update to the latest timestamp while coming out of underrun
        m_uLastTimestampBeforeRebuffering = get_last_timestamp();
        m_pAVSync->ResetPlaybackOffset(AVSync::VideoAV);
      }
    }
    bRebufferHappened = false;
  }
}


/* ======================================================================
FUNCTION:
  VideoDec::RefillDecodeBuf

DESCRIPTION:
  Refill decode buffer for a layer.

INPUT/OUPUT PARAMETERS:
  layer

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::RefillDecodeBuf(const uint32 layer)
{
  /* if decode buffer's Use Same Frame Again is set, then don't get new frame */
  if (decodeBuf.bUseAgain[layer])
  {
    decodeBuf.bUseAgain[layer] = false;
    return;
  }

  if (pMpeg4)
  {
    do
    {
      RefillMpeg4DecodeBuf(layer);
      /* skip zero size frames */
    } while ((dataState[layer]==Media::DATA_OK) && (decodeBuf.size[layer]<=0));
  }
  else if (pBitfile)
  {
    RefillBitstreamDecodeBuf(layer);
  }
}

/* ======================================================================
FUNCTION:
  VideoDec::RefillMpeg4DecodeBuf

DESCRIPTION:
  Refills the MP-4 decode buffer

INPUT/OUPUT PARAMETERS:
  layer

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::RefillMpeg4DecodeBuf(const int layer)
{

  //get data if necessary
  //and update the data state for this layer
  if (decodeBuf.size[layer] <= 0)
  {
    decodeBuf.size[layer] = 0;
    if (pMpeg4)
    {
      //In case of TS clips,at times the server doesn't stream TS data
      //continously, hence there might be a DATA_UNDERRUN when retrieving the
      //TS data from the streamer which should be ignored
      if ((decodeBuf.pData[layer])
          && ((dataState[layer]==Media::DATA_OK) || (dataState[layer]==Media::DATA_FRAGMENT)
              || (dataState[layer]==Media::DATA_REALLOCATE_BUFFER)
              || (dataState[layer]==Media::DATA_INSUFFICIENT)
#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
              || ((layer > 0) && (dataState[layer]==Media::DATA_UNDERRUN))
#endif /*  FEATURE_MP4_TEMPORAL_SCALABILITY */
             ))
      {
        uint32 timeStamp = 0;
        uint32 timeStampDelta = 0;

        Media::MediaStatus status = Media::DATA_OK;
        long nBytes = 0;

        //get sample
        nBytes = pMpeg4->GetNextLayeredVideoSample( status,
                                                    layer,
                                                    decodeBuf.pData[layer],
                                                    decodeBuf.maxSize[layer],
                                                    decodeBuf.pos[layer],
                                                    &decodeBuf.userdata[layer] );
        //initialize the buffer read pos to 0.
        decodeBuf.pos[layer] = 0;

        //check status
        switch (status)
        {
          case Media::DATA_OK:
            //get timestamp.  this does an unsigned to signed conversion,
            //if the timestamp is unknown it will be (-1) here.
            timeStamp = pMpeg4->GetTimestampForCurrentLayeredVideoSample(layer);
            timeStampDelta = pMpeg4->GetTimestampDeltaForCurrentLayeredVideoSample(layer);

            //decoder wants msec input.
            if (timestamp_is_defined( timeStamp ))
            {
              decodeBuf.timestamp[layer] = (uint64)pMpeg4->
                                           ConvertVideoTimestamp ( timeStamp,layer );
              decodeBuf.delta[layer] = pMpeg4->ConvertVideoTimestamp( timeStampDelta,layer );

            }
            else
            {
              decodeBuf.timestamp[layer] = UNDEFINED_TIMESTAMP | timeStamp;
            }

            //catch repositioning
            if (repos.bUpdate)
            {
              QTV_MSG(QTVDIAG_VIDEO_TASK,
                      "Caught repositioning");
              repos.bUpdate=false;
              //turn off rendering since actual video position will
              //go to the I-frame before the desired position.
              repos.bRendering=false;
              repos.nRenderPos=(long)repos.nPos;
              repos.timestamp = (int32)(decodeBuf.timestamp[layer] &TIMESTAMP_MASK);
              QTV_MSG2(QTVDIAG_VIDEO_TASK,
                       "Render Position %ld, Reposition TS %ld",
                       repos.nRenderPos,repos.timestamp );

#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
              /* A reposition is detected while getting a sample from a particular
               * video layer.  The other layers now contain invalid samples from
               * the "old" position.  For these other layers, replace the invalid
               * samples with valid ones from the new position.
               */
              for (int i = 0; i < videoInfo.nLayers; i++)
              {
                if (i != layer)
                {
                  Media::MediaStatus mediaStatus = Media::DATA_OK;

                  int numBytes = pMpeg4->GetNextLayeredVideoSample( mediaStatus,
                                                                    i,
                                                                    decodeBuf.pData[i],
                                                                    decodeBuf.maxSize[i],
                                                                    decodeBuf.pos[i],
                                                                    &decodeBuf.userdata[i] );
                //initialize the buffer read pos to 0.
                decodeBuf.pos[layer] = 0;

                  timeStamp =
                  pMpeg4->GetTimestampForCurrentLayeredVideoSample(i);
                  if (timestamp_is_defined( timeStamp ))
                  {
                    decodeBuf.timestamp[i] =
                    (uint64)pMpeg4->ConvertVideoTimestamp(timeStamp, i);
                  }
                  else
                  {
                    decodeBuf.timestamp[i] = UNDEFINED_TIMESTAMP | timeStamp;
                  }

                  dataState[i] = mediaStatus;

                  if (!pMpeg4->CheckError(Common::GETVIDEOTIME) && numBytes > 0)
                  {
                    decodeBuf.size[i] = numBytes;
                  }
                  else if (( i > 0 ) && ( mediaStatus == Media::DATA_UNDERRUN))
                  {
                    QTV_MSG2(QTVDIAG_VIDEO_TASK,"repos:DATA_UNDERRRUN in layer %d when getting data for %d",i,layer);
                    decodeBuf.size[i] = 0;
                  }
                  else
                  {
                    dataState[i] = Media::DATA_ERROR;
                  }
                }
                else
                {
                  continue;
                }
              }
#endif /* FEATURE_MP4_TEMPORAL_SCALABILITY */
            }

#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
            QTV_MSG2(QTVDIAG_VIDEO_TASK, "Time stamps: %d %d",
                     decodeBuf.timestamp[0], decodeBuf.timestamp[1]);
#endif /* FEATURE_MP4_TEMPORAL_SCALABILITY */

            if (!pMpeg4->CheckError(Common::GETVIDEOTIME))
            {
              decodeBuf.size[layer] = nBytes;
              if (nBytes > 0)
              {
                if ((dataState[layer] == Media::DATA_FRAGMENT) ||
                    (dataState[layer] == Media::DATA_REALLOCATE_BUFFER) ||
                    (dataState[layer] == Media::DATA_INSUFFICIENT))
                {
                  dataState[layer] = Media::DATA_OK;
                  overallDataState = dataState[layer];
                  QTV_MSG( QTVDIAG_VIDEO_TASK,"Changing overallDataState to DATA_OK" );
                }
              }
              else
              {
                /* this is zero size frame, which we need to skip */
                dataState[layer] = Media::DATA_OK;
              }
            }
            else
            {
              //unexpected error
              dataState[layer] = Media::DATA_ERROR;
            }
            break;


          case Media::DATA_UNDERRUN:
#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
            //Since the server maynot stream TS data continously,ignore
            //DATA_UNDERRUN for the enhanced layers
            if ( layer == 0 )
#endif /* FEATURE_MP4_TEMPORAL_SCALABILITY */
            {
              bRebufferHappened = true;
              // Remember  the last timestamp received before rebuffering
              m_uLastTimestampBeforeRebuffering = get_last_timestamp();
            }
#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
            else
            {
              QTV_MSG1( QTVDIAG_VIDEO_TASK,"DATA_UNDERRN for layer %d",layer );
            }
#endif /* FEATURE_MP4_TEMPORAL_SCALABILITY */

            //fallthru to next case...

          case Media::DATA_ERROR:
            //get error detail
            (void)pMpeg4->CheckError(Common::GETVIDEO);
            //fallthru to next case...
          case Media::DATA_END:
            dataState[layer] = status;
#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
            {
              int b,tmp = 0;
              for (b=0; b<videoInfo.nLayers; b++)
              {
                if (dataState[b] == Media::DATA_END)
                  ++tmp;  // DATA_END = 1
              }
              /* if all layer done, set overall state*/
              if (tmp == b)
              {
                /* if we hit end of media, inform player about this, but we want to
                  inform only once, but don't change video player state, since we are
                  still playing pending frames, after playing all the video frames,
                  we will send VIDEO_DONE status
                */
                if (overallDataState != Media::DATA_END)
                {
                  pPlayer->Notify(pPlayer->GetState(), Common::VIDEO_MEDIA_END);
                }
                overallDataState = Media::DATA_END;
              }
            }
#endif /* FEATURE_MP4_TEMPORAL_SCALABILITY */
            break;

#ifdef FEATURE_FILE_FRAGMENTATION
          case Media::DATA_FRAGMENT:
            dataState[layer] = Media::DATA_FRAGMENT;
            break;
#endif  /* FEATURE_FILE_FRAGMENTATION */

          case Media::DATA_REALLOCATE_BUFFER:
            for (int idx = 0; idx < videoInfo.nLayers; idx++)
            {
              uint32 bufferSize = 0;
              if (pMpeg4)
              {
                bufferSize = pMpeg4->GetMaxVideoBufferSizeDB(idx);
                if (bufferSize > decodeBuf.maxSize[idx])
                {
                  decodeBuf.maxSize[idx] = bufferSize;
                  if (decodeBuf.pData[idx])
                  {
                    QTV_Free(decodeBuf.pData[idx]);
                    decodeBuf.pData[idx] = NULL;
                  }
                  decodeBuf.pData[idx] = (uint8*)QTV_Malloc(decodeBuf.maxSize[idx]);
                  if (decodeBuf.pData[idx] == NULL)
                  {
                    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "MEMORY ERROR: Malloc Failed %d Bytes", decodeBuf.maxSize[idx] );
                    dataState[idx] = Media::DATA_END;
                    overallDataState=Media::DATA_END;
                  }
                  else
                  {
                    decodeBuf.size[layer] = 0;
                    decodeBuf.userdata[layer] = NULL;
                    dataState[layer] = Media::DATA_OK;
                  }
                }
              }
            }
            break;

          case Media::DATA_INSUFFICIENT:
            dataState[layer] = Media::DATA_INSUFFICIENT;
            QTV_MSG( QTVDIAG_VIDEO_TASK,"Changing dataState[layer] = Media::DATA_INSUFFICIENT" );
            break;

          default:
            //unexpected status.
            dataState[layer] = Media::DATA_ERROR;
            break;
        }
      }
    }
  }
}

/* ======================================================================
FUNCTION:
  VideoDec::RefillBitstreamDecodeBuf

DESCRIPTION:
  Refill the bitstream

INPUT/OUPUT PARAMETERS:
  layer

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::RefillBitstreamDecodeBuf(const int layer)
{
  //get data if necessary
  //and set the data state for this layer.
  if (layer==0)
  {
    if (decodeBuf.size[layer] <= 0)
    {
      decodeBuf.size[layer]=0;
      if (dataState[layer]==Media::DATA_OK)
      {
#ifdef FEATURE_QTV_M4V
#error code not present
#endif
      }
    }
  }
  if (decodeBuf.size[layer] <=0)
  {
    dataState[layer]=Media::DATA_END;
  }
}

/* ======================================================================
FUNCTION:
  VideoDec::ResetVideoPlayback

DESCRIPTION:
  Resets playback

INPUT/OUPUT PARAMETERS:
  layer

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::ResetVideoPlayback(const int layer)
{
  if (pMpeg4)
  {
    (void)pMpeg4->ResetLayeredVideoPlayback(layer);
  }
  else if (pBitfile)
  {
    if (layer==0)
    {
#ifdef FEATURE_QTV_M4V
#error code not present
#endif
    }
  }
}

/* ======================================================================
FUNCTION:
  VideoDec::DiscardDecodeLayer

DESCRIPTION:
  Discards the specified decode layer

INPUT/OUPUT PARAMETERS:
  nLayer

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::DiscardDecodeLayer(const int nLayer)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoDec::DiscardDecodeLayer" );
  decodeBuf.size[nLayer]=0;
}

/* ======================================================================
FUNCTION:
  VideoDec::DiscardDecodeBuf

DESCRIPTION:
  discard the current contents of the decode buf.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::DiscardDecodeBuf()
{
  for (int layer = 0; layer < videoInfo.nLayers; layer++)
  {
    decodeBuf.size[layer]=0;
  }
}

//////////////////////////////////////////////////////////////////////
//
// video stats
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoDec::FrameDisplayed

DESCRIPTION:
  a frame was just displayed, update stats

INPUT/OUPUT PARAMETERS:
  nPresFrame
  nPresTime
  time

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::FrameDisplayed(long nPresFrame,long nPresTime,long lTime)
{
  QCUtils::EnterCritSect(&videoStat.CS);
  videoStat.lastFrame.bValid=true;
  videoStat.lastFrame.nFrame=nPresFrame;
  videoStat.lastFrame.nTimestamp=nPresTime;
  videoStat.lastFrame.nTime=lTime;
  QCUtils::LeaveCritSect(&videoStat.CS);
}

/* ======================================================================
FUNCTION:
  VideoDec::UpdateStats

DESCRIPTION:
  Update the stats

INPUT/OUPUT PARAMETERS:
  action

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::UpdateStats(StatUpdateType action)
{
  //Update counts.
  QCUtils::EnterCritSect(&videoStat.CS);
  switch (action)
  {
    case DROP_BEFORE_DECODE:
      videoStat.nDroppedDecode++;
      break;
    case DROP_BEFORE_DISPLAY:
      videoStat.nDroppedDisplay++;
      break;
  case DROP_REQUIRED_QUALITY_FAILURE:
    videoStat.nDropMacroBlockErrs++;
    break;
    case FAILED_DECODE:
      videoStat.nFailedDecode++;
      break;
    case INTERPOLATED:
      videoStat.nFramesInterpolated++;
      break;
    case CONVERTED:
    case DISPLAYED:
      break;
    case DROP_AVSYNC:
      videoStat.nDroppedAVSync++;   /*Dropped due to AV sync*/
      break;
    default:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Unknown stat action" );
      break;
  }

  //update drop chunks.
  switch (action)
  {
    case CONVERTED:
      videoStat.nCurrentChunkDecode=0;
      break;

    case DISPLAYED:
      videoStat.nCurrentChunkDisplay=0;
      break;

    case FAILED_DECODE:
    case DROP_BEFORE_DECODE:
      videoStat.nCurrentChunkDecode++;
      videoStat.nMaxChunk=ZMAX(videoStat.nMaxChunk,videoStat.nCurrentChunkDecode);
      break;

    case DROP_AVSYNC:
    case DROP_BEFORE_DISPLAY:
  case DROP_REQUIRED_QUALITY_FAILURE:
      videoStat.nCurrentChunkDisplay++;
      videoStat.nMaxChunk=ZMAX(videoStat.nMaxChunk,videoStat.nCurrentChunkDisplay);
      break;
  }
  QCUtils::LeaveCritSect(&videoStat.CS);
}

/* ======================================================================
FUNCTION:
  VideoDec::UpdateStats

DESCRIPTION:
  Update the stats

INPUT/OUPUT PARAMETERS:
  action

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::UpdateStats(StatUpdateType action, void * const pFrame)
{
  //Update counts.
  QCUtils::EnterCritSect(&videoStat.CS);
  switch (action)
  {
    case CONVERTED:
        if (((VDEC_FRAME*)pFrame)->frameType == VDEC_FRAMETYPE_I)
        {
          ++videoStat.nIFrameTally;
        }
        else if (((VDEC_FRAME*)pFrame)->frameType == VDEC_FRAMETYPE_P)
        {
          ++videoStat.nPFrameTally;
        }
        else if (((VDEC_FRAME*)pFrame)->frameType == VDEC_FRAMETYPE_B)
        {
          ++videoStat.nBFrameTally;
        }

	IncrementFrameCount();
      break;
  }
  QCUtils::LeaveCritSect(&videoStat.CS);
}


/* ======================================================================
FUNCTION:
  VideoDec::UpdateFrameIntervalStats

DESCRIPTION:
  Update the average frame interval and previous timestamp.

INPUT/OUPUT PARAMETERS:
  timestamp - The timestamp of the current frame.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::UpdateFrameIntervalStats(long timestampMsec)
{
  QCUtils::EnterCritSect(&videoStat.CS);

  // After seek, don't update average interval.
  // UpdateFrameIntervalStatsForReposition() sets
  // videoStat.prevFrameTimestampMsec to a huge value so we can be sure that
  // afterwards any play time is smaller.
  if (timestampMsec > videoStat.prevFrameTimestampMsec)
  {
    // Make sure the moving average is not too tiny and that we never divide
    // by 0.
    float delta = (float) MAX(timestampMsec - videoStat.prevFrameTimestampMsec, 1);
    if (videoStat.averageInterFrameIntervalMsec > 0.0f)
    {
      /* In the moving average, weigh the smaller one more. That way if we get
       * large intervals corresponding to non-consecutive frames, the moving
       * increases slowly, but if we get smaller intervals, the moving average
       * decreases quickly.
       *
       * Let D  = delta (current interval)
       *     A  = moving average so far
       *     A' = updated moving average
       *
       * If (D <= A)
       *    A' = (small weight) * A + (large weight) * D  (1)
       * Else D > A
       *    A' = (small weight) * D + (large weight) * A  (2)
       *
       * small weight + large weight = 1
       *
       * In (1), we find that small weight = D / (A + D),
       * large weight = A / (A + D) works.
       *
       * In (2), we find that small weight = A / (A + D),
       * large weight = D / (A + D) works.
       *
       * Putting everything together, we find a single formula works for
       * both cases:
       *
       * A' = AD / (A + D) + AD / (A + D) = 2AD / (A + D)
       */
      videoStat.averageInterFrameIntervalMsec = 2.0f *
                                                videoStat.averageInterFrameIntervalMsec * delta /
                                                (videoStat.averageInterFrameIntervalMsec + delta);
    }
    else
    {
      // in case of first frame
      videoStat.averageInterFrameIntervalMsec = delta;
    }

    QTV_MSG_PRIO1(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                  "Average FPS = %d", videoStat.averageInterFrameIntervalMsec);
  }

  videoStat.prevFrameTimestampMsec = timestampMsec;

  QCUtils::LeaveCritSect(&videoStat.CS);
}


/* ======================================================================
FUNCTION:
  VideoDec::UpdateFrameIntervalStatsForReposition

DESCRIPTION:
  Update the frame interval statistics to handle a reposition event.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::UpdateFrameIntervalStatsForReposition()
{
  QCUtils::EnterCritSect(&videoStat.CS);

  videoStat.prevFrameTimestampMsec = 0x7FFFFFFF;

  QCUtils::LeaveCritSect(&videoStat.CS);
}


/* ======================================================================
FUNCTION:
  VideoDec::EndPlayInterval

DESCRIPTION:
  reached the end of a play interval

INPUT/OUPUT PARAMETERS:
  time

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::EndPlayInterval(long lTime)
{
  if (!isPlayTimeCalculated)
  {
    videoStat.tPause=lTime;
    videoStat.nPlayMsec += (videoStat.tPause - videoStat.tResume);

    //To avoid calculating Playmsec multiple times
    isPlayTimeCalculated = true;
  }
  //make a note of cases where we never got to start rendering.
  //this can happen if the app is repositioning too rapidly.
  if (!repos.bRendering)
  {
    QTV_MSG( QTVDIAG_VIDEO_TASK, "Warning, video ended before rendering had begun" );
  }
}

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
/* ======================================================================
FUNCTION:
  VideoDec::ReleaseDSP

DESCRIPTION:
  This function interfaces with the VDEC to perform a QDSP release.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  DSP can only be restored with the call to RestoreDSP().
========================================================================== */
bool VideoDec::ReleaseDSP()
{
  if (DoGenericVdecSuspend())
  {
    return true;
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoDec: ReleaseDSP failed" );
    return false;
  }
}
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
/* ======================================================================
FUNCTION:
  VideoDec::RestoreDSP

DESCRIPTION:
  This function interfaces with the VDEC to restore the DSP image that
  was present before ReleaseDSP() was called. Does nothing if restoration
  is not required.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  ReleaseDSP() should be called first.
========================================================================== */
void VideoDec::RestoreDSP()
{
  if (!DoGenericVdecResume())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoDec: RestoreDSP failed" );
  }
}
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

/* ======================================================================
FUNCTION:
  VideoDec::StartPlayInterval

DESCRIPTION:
  Do last-minute init at start of play or resume.

INPUT/OUPUT PARAMETERS:
  time

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::StartPlayInterval( long lTime )
{
  bTimeOffsetValid=false;
  //note the start time.
  videoStat.tResume=lTime;
  isPlayTimeCalculated = false;
}

/* ======================================================================
FUNCTION:
  VideoDec::EndPlay

DESCRIPTION:
  end of playback.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::EndPlay()
{
  // Allow the audio thread to exit cleanly
  overallDataState=Media::DATA_END;
}

/* ======================================================================
FUNCTION:
  VideoDec::SetStopTime

DESCRIPTION:
  Sets the stop time.

INPUT/OUPUT PARAMETERS:
  stop

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::SetStopTime(long stop)
{
  if (stop<0)
  {
    stopTime.bStop=false;
  }
  else
  {
    stopTime.bStop=true;
    stopTime.nStop=(unsigned long)stop;
  }
}

/* ======================================================================
FUNCTION:
  VideoDec::SetPos

DESCRIPTION:
  Sets the position

INPUT/OUPUT PARAMETERS:
  pos

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::SetPos(long pos)
{
  if (pos>=0)
  {
    //set flags so the position will be updated when
    //filling the decode buffer.
    repos.nPos=(unsigned long)pos;
    QTV_MSG1(QTVDIAG_VIDEO_TASK,"Set video position to %ld",repos.nPos);
    repos.bUpdate=true;
    repos.bRendering=false;
    repos.timestamp = pos;

#ifdef FEATURE_QTV_6250_ARM_CLK_COMPENSATION
    reposition = true;
#endif

    // patch to deal with a specific scenario where after FF to very close to end
    // of clip, there is not video traffic coming in. In that case, the timeoffset will
    // be stale value and thus prevent video player to stop properly after end of clip
    timeOffset = ZUtils::Clock() - pos;


    UpdateFrameIntervalStatsForReposition();
  }
}


////////////////////////////////////////////////////////////////////
//
// Timing
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoDec::GetElapsedTime

DESCRIPTION:
  Calculates and returns the time elapsed since the start of the clip.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  The elapsed time.

SIDE EFFECTS:
  None.
========================================================================== */
long VideoDec::GetElapsedTime()
{
  long lTime = ZUtils::Clock();
  long elapsedTime = lTime - timeOffset;

  return elapsedTime;
}

/* ======================================================================
FUNCTION:
  VideoDec::CheckFrameTime

DESCRIPTION:
  Compare frame timestamp against clock time and threshold.

INPUT/OUPUT PARAMETERS:
  presentationtime - frame presentation time, or (-1) if none
  checkContext - context
  bNoDrop
  pSleep - sleep time

RETURN VALUE:
  Return true if frame is too late.

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::CheckFrameTime(
                             const unsigned long presentationtime,
#ifdef SHOW_LATE
                             const FrameCheckContext checkContext,
#endif /* SHOW_LATE */
                             const long bNoDrop,
                             long *pSleep)
{
  //default is display now.
  long delay = 0;
  bool bLate = false;
  uint32 now =ZUtils::Clock();


#ifdef FEATURE_QTV_6250_ARM_CLK_COMPENSATION
  /* the following code compensates for the clock drift, since we do not
    know the frame rate while streaming,the sleep time of video is
    adjusted by increasing the clock time read from ZUtils::Clock by
    the drift factor stored in drftValue. The value of driftConstant
    is set in PrepPlay */
  float drftValue = driftConstant;

  /* resumedTime is used to cater to reposition case, when
  we need to clear the drift compensation and start all over again. the value of reposition
  is set in SetPos function */

  if (reposition)
  {
    resumedTime = presentationtime;
    reposition = false;
  }

  // The original calculation here was:
  //
  //   if((presentationtime-resumedTime) >= 0)
  //   {
  //     lTime = lTime + (long)((presentationtime - resumedTime)*drftValue);
  //   }
  //
  // This fails if timestamps roll over at some point other than 2^32 - 1.
  int32 PTminusRT( Difference( resumedTime, presentationtime, GetMaxTimestamp() ) );
  if (PTminusRT >= 0)
  {
    now = Sum( now, PTminusRT * drftValue );
  }
#endif


  //
  // Get time from now til time to display.
  //
  // update AVsync point
  UpdateTimeOffset(presentationtime);

  /* if we are in single step mode, we don't need to either check the timing
     or drop the frame */
  if (bSingleStepOn)
    return false;

  // if we know the desired presentation time and the playback time offset
  // then we can compute the delay.  otherwise we'll display now.
  if ((long)presentationtime >= 0 && bTimeOffsetValid)
  {
    //delay = desired display time - clock time.
    //desired display time = current frame timestamp + playback time offset
    //playback time offset = clock time - timestamp of reference frame.
#ifdef SHOW_SYNC
    QTV_MSG3( QTVDIAG_VIDEO_TASK, "pres %ld offset %ld clock %ld",presentationtime,timeOffset,now);
#endif

    /* To enable fast playback the timestamps need to be shrunk, i.e.
    ** PTS/playbackSpeed. We represent playback speed as a ratio, this will
    ** translate to PTS/(Num/Den) = PTS*Den/Num. Presentation time is
    ** unmodified for 1x speed.
    */
    // The original calculation here was:
    //
    // delay = (((presentationtime*m_playbackSpeedDen)/m_playbackSpeedNum) + timeOffset - lTime);
    //
    // This fails if timestamps roll over at some point other than 2^32 - 1.
    uint32 PTtimesSpeedMultiplier( ( ( presentationtime*m_playbackSpeedDen)/m_playbackSpeedNum ) % ( GetMaxTimestamp() + 1 ) );
    uint32 displayTime( Sum( PTtimesSpeedMultiplier, timeOffset, GetMaxTimestamp() ) );
    delay = Difference( now, displayTime, GetMaxTimestamp() );
  }

#ifdef SHOW_TIMING
  QTV_MSG3( QTVDIAG_VIDEO_TASK, "Video Pres %ld, delay %ld now %ld",presentationtime,delay, now - timeOffset);
#endif

// If this is a video-only streaming clip, and the delay is very big, adjust the delay and
  // A/V sync so that the frame is displayed sooner.
  if(pMpeg4)
  {
    if ((pMpeg4->GetSource() == Media::STREAM_SOURCE) &&
        (pMpeg4->GetNumberOfAudioTracks() == 0))
    {
      long oldDelay = delay;
      float frameRate = pMpeg4->GetVideoFrameRate();
      int32 interval = 0;

      if (frameRate > 0.0f)
      {
        interval = (int32) (1000.0f / frameRate);
      }
      else
      {
        interval = (int32) videoStat.averageInterFrameIntervalMsec;
      }

      delay = pMpeg4->AdjustInterFrameDelay(oldDelay, interval);

      if (oldDelay != delay)
      {
        QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
                       "Video delay adjusted to %d", delay);
        if(m_pAVSync)
        {
          m_pAVSync->SetPlaybackOffset(AVSync::VideoAV, ZUtils::Clock() + delay,
                                  presentationtime);
        }
      }
    }
  }

  if (delay < 0)
  {
    //Frame is late.
    QTV_MSG( QTVDIAG_VIDEO_TASK,"Frame is Late. Check Lip Sync..");

    //keep stats on max "lateness" of frames.
    videoStat.nMaxBehindMsec = ZMAX(videoStat.nMaxBehindMsec,(-delay));

    //delare the frame too old if it meets the threshold.
    if ((-delay) >  LipSyncWindowSize)
    {
      //this frame is too old
      QTV_MSG( QTVDIAG_VIDEO_TASK,"Lip Sync: Frame is too old");
      bLate = true;
    }
  }
  else
  {
    //Frame is early or exactly on time.

    QTV_MSG( QTVDIAG_VIDEO_TASK,"Frame is early or exactly on Time");
    //keep stats on maximum "earliness".
    videoStat.nMaxAheadMsec = ZMAX(delay,videoStat.nMaxAheadMsec);

  }

#ifndef PLATFORM_LTK
  /* Log the A/V sync information for this frame.
  */
  LogAVSync( presentationtime, -delay,(bNoDrop)?FALSE:bLate);
#endif /* ! PLATFORM_LTK */

  //set sleep time if a pointer was provided.
  if (pSleep)
  {
    *pSleep = delay;
    QTV_MSG2( QTVDIAG_VIDEO_TASK,
              "CheckFrame: sleep time %ld for presentation time %ld",
              delay,
              presentationtime );
  }


  if (bLate)
  {
#ifndef FEATURE_QTV_GENERIC_BCAST
    if (Media::BCAST_FLO_SOURCE == pMpeg4->GetSource())
#else
    if (IsLiveBcastMediaSource(pMpeg4))
#endif
    {
      QTV_MSG_PRIO5(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                    "Video fr %d is late - PTS: %d, TOD: %d, delta: %dms, audio time: %d",
                    nFramesProcessed,
                    presentationtime + timeOffset,
                    now,
                    now-(presentationtime + timeOffset),
                    now + timeOffset);
    }
  }


#ifdef SHOW_LATE
  if (bLate)
  {
    QTV_MSG3( QTVDIAG_VIDEO_TASK, "VIDEO ts %ld d %ld c %ld\n ",presentationtime,delay,now);
    switch (checkContext)
    {
      case CHECK_BEFORE_DECODE:
        QTV_MSG1( QTVDIAG_VIDEO_TASK, "...before decode, bNoDrop %d",bNoDrop); break;
      case CHECK_BEFORE_DISPLAY:
        QTV_MSG1( QTVDIAG_VIDEO_TASK, "...before display, bNoDrop %d ",bNoDrop); break;
      case CHECK_BEFORE_CONVERT:
        QTV_MSG1( QTVDIAG_VIDEO_TASK, "...before convert, bNoDrop %d ",bNoDrop); break;
      case CHECK_BEFORE_DISPLAY_AGAIN:
        QTV_MSG1( QTVDIAG_VIDEO_TASK, "...before display 2, bNoDrop %d ",bNoDrop); break;
      case CHECK_BEFORE_QUEUE:
        QTV_MSG1( QTVDIAG_VIDEO_TASK, "...before queue, bNoDrop %d ",bNoDrop); break;
      default:
        QTV_MSG1( QTVDIAG_VIDEO_TASK, "...before ???, bNoDrop %d ",bNoDrop); break;
    }
  }
#endif //SHOW_LATE

  if (bNoDrop)
  {
    return false;
  }
  return bLate;
}

/* ======================================================================
FUNCTION:
  VideoDec::UpdateTimeOffset

DESCRIPTION:
  Updates time offset

INPUT/OUPUT PARAMETERS:
  nPresTime

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::UpdateTimeOffset(const long nPresTime)
{
  // Note: If this offset has not been set by the audio player (such as when
  //   there is no audio), then this call will actually set the offset
  //   to match the first video timestamp.  That's why we call this only
  //   after decoding, so the time will be as close as possible to the
  //   display time.
  if (nPresTime >= 0)
  {
    bool ballowVideoToSetAVSyncOffSet = false;

    if ((nPresTime==0) && pMpeg4->bLoopTrackFlag && (nPresTime >= get_last_timestamp()))
    {
      QTV_MSG(QTVDIAG_VIDEO_TASK,
              "Video Track is looping. Allow video to set AV sync offset");

      if (m_pAVSync)
      {
        m_pAVSync->ResetPlaybackOffset(AVSync::VideoAV);
      }
      ballowVideoToSetAVSyncOffSet = true;
    }
    else if (pPlayer->IsAudioPlaying() && (pPlayer->pAVPlayer != NULL) &&
             !pPlayer->pAVPlayer->IsAudioOutputDelayed())
    {
      QTV_MSG_PRIO(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                   "Player has audio. Preventing video from setting AV sync offset");
      ballowVideoToSetAVSyncOffSet = false;
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                   "Player has no audio. Allow video to set AV sync offset");

      ballowVideoToSetAVSyncOffSet = true;
      if (bSingleStepOn)
      {
        if (m_pAVSync)
        {
          m_pAVSync->ResetPlaybackOffset(AVSync::VideoAV);
        }
      }
    }

    long prev_offset = timeOffset;
#ifndef FEATURE_QTV_GENERIC_BCAST
    if ((Media::BCAST_FLO_SOURCE == pPlayer->decIF.pMpeg4->GetSource()) &&
#else
    // audio is not available then set video as the master
    if ((IsLiveBcastMediaSource(pPlayer->decIF.pMpeg4,m_pAVSync)) &&
#endif
        pPlayer->HasAudio() && m_pAVSync &&
        !m_pAVSync->IsOffsetValid(AVSync::AudioAV))
    {
      /*---------------------------------------------------------------------
        This clip has audio, however, its offset is not valid, and its
        not currently available (no data).  Allow video to set av sync
      ---------------------------------------------------------------------*/
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Audio not yet available, allowing video to set AV sync");
      ballowVideoToSetAVSyncOffSet = true;

      /* Reset this flag. it is enough to set once */
      pPlayer->pAVPlayer->bAllowVidToSetOffSetOnAudUnderrun = false;

    }
    //in case of Streaming and if Audio has not yet set the Playback offset 
    //Allow Video to set the playback offset once 
    if(((pPlayer->decIF.pMpeg4 != NULL) && (Media::STREAM_SOURCE == pPlayer->decIF.pMpeg4->GetSource())) &&
	   pPlayer->HasAudio() && m_pAVSync &&
        !m_pAVSync->IsOffsetValid(AVSync::AudioAV) )
	{
		/*---------------------------------------------------------------------
		  This clip has audio, however, its offset is not valid, and its
		  not currently available Audio Thread is late in pulling .
		  Allow video to set av sync
		---------------------------------------------------------------------*/
		QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
				"Audio not yet available, allowing video to set AV sync");
		ballowVideoToSetAVSyncOffSet = true;

		/* Reset this flag. it is enough to set once */
		pPlayer->pAVPlayer->bAllowVidToSetOffSetOnAudUnderrun = false;
	}

    if (m_pAVSync)
    {
      long int delay = 0;

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif
      {
        // If the frame under consideration does not belong to current interval
        // then do not change the time offset.
        if (ballowVideoToSetAVSyncOffSet && !CheckPlayIntervalMatch(nPresTime))
        {
          ballowVideoToSetAVSyncOffSet = false;
        }
      }

      /* To enable fast playback the timestamps need to be shrunk, i.e.
      ** PTS/playbackSpeed. We represent playback speed as a ratio, this will
      ** translate to PTS/(Num/Den) = PTS*Den/Num. Presentation time is
      ** unmodified for 1x speed.
      */
      timeOffset = m_pAVSync->ReadPlaybackOffset(AVSync::VideoAV,
                                                 ZUtils::Clock() + delay,
                                                 ((nPresTime*m_playbackSpeedDen)/m_playbackSpeedNum),
                                                 ballowVideoToSetAVSyncOffSet );
    }

    if (abs((prev_offset-timeOffset)) > 1)
    {
      // Only want to print this when the time actually changes, as opposed
      // to common 1ms drifts up or down (which are due to floating point
      // divide error)
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                    "AV time offset changed - old: %d, new: %d",
                    prev_offset, timeOffset);
    }

    bTimeOffsetValid = true;
#ifdef SHOW_TIMING
    QTV_MSG1( QTVDIAG_VIDEO_TASK, "video Time offset %ld",timeOffset );
#endif
  }
}

/* ======================================================================
FUNCTION:
  VideoDec::RenderVideoFrame

DESCRIPTION:
  Render the video frame

INPUT/OUPUT PARAMETERS:
  bAbort
  cbData

RETURN VALUE:
  RENDER_FRAME_RESULT
    RVF_OK,   // Frame was rendered successfully
    RVF_DROP, // Frame was dropped
    RVF_DEFER // Frame must be deferred due to player state change

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::RenderVideoFrame
(
bool         &bAbort,
VDEC_FRAME   &frame
#ifdef FEATURE_QTV_MDP
,bool        &bFreeFrame
#endif /* FEATURE_QTV_MDP */
)
{
  long nPresentationTime;
  bAbort = false;
  bool bDropOK;
  bool bDisplayed;

#ifdef FEATURE_QTV_MDP
  // Default to freeing the frame in case of an early return.
  bFreeFrame = true;
#endif /* FEATURE_QTV_MDP */

  nPresentationTime = frame.timestamp;

  if (nPresentationTime < 0)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "QTV Timestamp is %ld, NOT VALID!", nPresentationTime );
    //act like the end of data was reached.
    overallDataState=Media::DATA_END;
    bAbort = true;
    return false;
  }

  if (frame.frameType == VDEC_FRAMETYPE_I)
  {
    QTV_MSG(QTVDIAG_VIDEO_TASK,"I-Frame");
  }
  else // Don't Care
  {
    QTV_MSG(QTVDIAG_VIDEO_TASK,"P-Frame");
  }

#ifdef SHOW_FRAMES
  QTV_MSG3( QTVDIAG_VIDEO_TASK, "Vid Frame2 %d ts %ld l %d",nFramesProcessed,nPresentationTime,nLayer);
#endif

  ////////////////////////////////////////////////
  // Detect stop time
  ////////////////////////////////////////////////
  if (stopTime.bStop && (unsigned long)nPresentationTime>stopTime.nStop)
  {
    //act like the end of data was reached.
    overallDataState=Media::DATA_END;
    videoStat.nStopPresTime=nPresentationTime;
    return false;
  }

  ////////////////////////////////////////////////
  // Detect start of rendering
  //
  // Because we do a synchronous flush when we reposition (or during other
  // discontinuities in playback) we do not need to 'filter' old frames
  // for which decoding began before the discontinuity occurred.  The first
  // call to RenderVideoFrame after the break will be an appropriate frame,
  // and we can just take whatever we get on faith.

  if (!repos.bRendering)
  {
    repos.bRendering=true;
    QTV_MSG1(QTVDIAG_VIDEO_TASK,"Start Rendering at ts %ld", repos.timestamp);

    // for the case where a video-only clip repositioning we need
    // to set the clock offset
    if (pPlayer->HasAudio() != true)
    {
      QTV_MSG( QTVDIAG_VIDEO_TASK, "INFO: Video repositioning caused timeadjust" );
      if (m_pAVSync)
      {
        long int delay = 0;

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif
        m_pAVSync->SetPlaybackOffset(AVSync::VideoAV, ZUtils::Clock() + delay,
                                     nPresentationTime);
      }
    }

    //tell the video player to start rendering.
    if (pPlayer)
    {
      pPlayer->StartRendering();
    }
  }

  ////////////////////////////////////////////////
  // update processed frame info
  ////////////////////////////////////////////////

  //timestamp range.
  //If its a first frame
  if (nFramesProcessed==1)
  {
    videoStat.nFirstTimestamp=nPresentationTime;
  }
  //check for continuity.
  else if (nPresentationTime < videoStat.nLastTimestamp)
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "WEIRD video TS seq. %ld %ld",
                  videoStat.nLastTimestamp, nPresentationTime);
  }
  videoStat.nLastTimestamp=nPresentationTime;

  //evaluate forced display condition.
  bDropOK = IsItOkayToDropThisFrame( &frame );

  if (pPlayer)
  {
    //store last frame info which is now displayed (frame is purged
    //as soon as it is displayed), this information is required for
    //recording stream
    lastRenderedFrameInfo.layer = frame.layer;
    lastRenderedFrameInfo.bIsIntra = (frame.frameType == VDEC_FRAMETYPE_I);
    lastRenderedFrameInfo.timestamp = frame.timestamp;

#ifdef FEATURE_QTV_STREAM_RECORD
    RecordFrame( &lastRenderedFrameInfo );
#endif // FEATURE_QTV_STREAM_RECORD

    // Record the information about the frame we'll send up.  Do this
    // before we send it to avoid race conditions.  If the render fails,
    // we'll forget the frame immediately.
    bDisplayed = pPlayer->DisplayFrame
                 (
                 frame,
                 nPresentationTime,
                 bSingleStepOn || !bDropOK,
                 nFramesProcessed
#ifdef FEATURE_QTV_MDP
                 ,bFreeFrame
#endif /* FEATURE_QTV_MDP */
                 );

    if (bDisplayed)
    {
      // Increment the number of frames rendered
      QCUtils::EnterCritSect(&frameReqUpdate_CS);
      nFramesRendered++;
      QCUtils::LeaveCritSect(&frameReqUpdate_CS);

#ifndef PLATFORM_LTK
      // Log information about the rendered frame.
      LogFrameRenderInfo( frame );
      LogRenderEvents( frame);

#endif /* ! PLATFORM_LTK */
      return true;
    }
    else
    {
      //PVDisplayVideoFrame Failed
      return false;
    } // bDisplayed
  } // pPlayer != NULL
  return true;
}


/* ======================================================================
FUNCTION:
  VideoDec::DecodeOneFrame

DESCRIPTION:
  Retrieve and decode a frame.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if healthy decodes were started, false if we saw errors.

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::DecodeOneFrame
(
bool &bSleepAfter
)
{
  bool               bQueueResult ( false );
  bool               bDecodeResult( false );
  VDEC_ERROR         err          ( VDEC_ERR_EVERYTHING_FINE );
  QTV_Broadcast*     broadcast_ptr( 0 );

#ifndef PLATFORM_LTK /* no timer services on LTK */
  timetick_type      start_time   ( 0 );
#endif /* PLATFORM_LTK */

  // By default, callers should not sleep after decoding.
  //
  bSleepAfter = false;

  if (_state == VIDEODEC_DISABLED)
  {
    return false;
  }

  // If we can, fill and queue an input buffer.
  //
  if (m_pVDEC_InputBuffer != 0)
  {
    // The VDEC input buffer is available, which means we need to refill it
    // and queue it.
    //
    RefillDecodeBuf();
    if (overallDataState == Media::DATA_OK) // the refill worked
    {
      if ((pMpeg4->GetSource() == Media::STREAM_SOURCE) &&
          (pMpeg4->GetVideoFrameRate() <= 0.0f))
      {
        UpdateFrameIntervalStats((int32)(decodeBuf.timestamp[0] & TIMESTAMP_MASK));
      }

      m_pVDEC_InputBuffer->layers = videoInfo.nLayers;
      for (int i = 0; i < m_pVDEC_InputBuffer->layers; ++i)
      {
        m_pVDEC_InputBuffer->buffer     [ i ] = decodeBuf.pData    [ i ];
        m_pVDEC_InputBuffer->buffer_size[ i ] = decodeBuf.size     [ i ];
        m_pVDEC_InputBuffer->timestamp  [ i ] = decodeBuf.timestamp[ i ];
        m_pVDEC_InputBuffer->delta      [ i ] = decodeBuf.delta    [ i ];
        m_pVDEC_InputBuffer->buffer_pos [ i ] = decodeBuf.pos      [ i ];
        m_pVDEC_InputBuffer->userData   [ i ] = decodeBuf.userdata [ i ];
      }

      err = vdec_queue( m_vdec,
                        m_pVDEC_InputBuffer,
                        InputBufferFreeCallback,
                        this );
      switch (err)
      {
        case VDEC_ERR_EVERYTHING_FINE:
          // Increment the outstanding frame count by our 'high water' value.
          // It'll get corrected when the buffer is returned and we see which
          // layers will actually generate frames.
          //
          (void)AlterOutstandingFrameCount( m_pVDEC_InputBuffer->layers,
                                            "initial worst-case" );
          m_pVDEC_InputBuffer = 0; // Input buffer is now 'out'!
          bQueueResult        = true;
          break;

        case VDEC_ERR_NO_INPUT_AVAILABLE:
        case VDEC_ERR_OUT_OF_BUFFERS:
          // These are 'backoff' cases: errors which, instead of indicating
          // trouble in the decoder, merely mean that it's out of resources
          // and we should stop trying to cram bits down its throat for
          // a little while.
          //
          // Unlike the backoff from vdec_decode, we do NOT set bSleepAfter
          // in this case - if the input pipes are full we need to keep
          // turning the handle on the grinder in order to make more room.
          // Quietly sleeping when the input queue is full would result
          // in a stall.
          //
          bQueueResult = true;
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                        "vdec_queue: %d. Queue will be deferred.", err );
          break;

        default:
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                        "Failure in vdec_queue: %d", err );
      } // switch ( err )
    } // overallDataState OK?
    else if(overallDataState == Media::DATA_END)
    {
      //We have reached DATA_END. queue a EOS buffer to decoder
      err = vdec_queue_eos(m_vdec,videoInfo.nLayers);
      if(err != VDEC_ERR_EVERYTHING_FINE)
      {
        bQueueResult = false;
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                      "Failure in vdec_queue_eOS: %d", err );
      }
      else
      {
        bQueueResult = true;
      }
    }
  } // m_pVDEC_InputBuffer != 0?
  else
  {
    // Frame already queued case
    bQueueResult = true;
  }

#ifndef PLATFORM_LTK /* no timer services on LTK */
  start_time = timetick_get();
#endif /* PLATFORM_LTK */

  // Wind the decoding machinery through another turn!
  //
  err = vdec_decode( m_vdec );
  last_vdec_err = err;

  if( isbFirstFrameDecoded == false)
  {
	VDEC_PARAMETER_DATA parm_data;
	isbFirstFrameDecoded = true; 
	vdec_get_parameter(m_vdec, VDEC_PARM_DIMENSIONS, &parm_data);
	pPlayer->m_pRenderer->notify_clipInfo_Dimensions( parm_data.dimensions.width, parm_data.dimensions.height);
	videoInfo.originalWidth = parm_data.dimensions.width;
	videoInfo.originalHeight = parm_data.dimensions.height;
  }

#ifndef PLATFORM_LTK /* no timer services on LTK */
  if (m_broadcaster_ptr != 0)
  {
    static uint32 prevTimestamp = 0xffffffff;
    uint32 timeStamp = (uint32)(decodeBuf.timestamp[0] & TIMESTAMP_MASK);

    /*-----------------------------------------------------------------------
      Make sure we don't send multiple messages for the same video frame.
    -----------------------------------------------------------------------*/
    if (!( timeStamp == prevTimestamp))
    {
      broadcast_ptr = QTV_New_Args( qtv_VIDEO_DECODE_TIMING_bcast_class,
                                    ( timetick_get_elapsed( start_time, T_MSEC ),
                                      timeStamp,
                                      err ) );
      if (broadcast_ptr != 0)
      {
        m_broadcaster_ptr->Transmit( broadcast_ptr );
      }

      prevTimestamp = timeStamp;
    }
  } /* m_broadcaster_ptr valid */
#endif /* PLATFORM_LTK */

  switch (err)
  {
    case VDEC_ERR_EVERYTHING_FINE:
    {
      bDecodeResult = true;
      break;
    }
    /*
    ** These are 'backoff' cases: errors which, instead of indicating
    ** trouble in the decoder, merely mean that it's out of resources
    ** and we should stop trying to cram bits down its throat for
    ** little while.
    */
    case VDEC_ERR_NO_INPUT_AVAILABLE:
    case VDEC_ERR_OUT_OF_BUFFERS:
    {
      bDecodeResult = true;
      bSleepAfter   = true;
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                    "vdec_decode: %d. Backing off...", err );
      break;
    }
    case VDEC_ERR_RESOURCE_UNAVAILABLE:
    case VDEC_ERR_OUT_OF_MEMORY:
    case VDEC_ERR_UNSUPPORTED_DIMENSIONS:
    case VDEC_ERR_UNSUPPORTED_PROFILE:
    {
      QTV_MSG_PRIO1(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL,
                    "Fatal error, aborting(%d)", err );
      overallDataState = Media::DATA_ERROR;
      break;
    }
    case VDEC_ERR_INVALID_INPUT:
    {
      // Not updating the FAILED_DECODE stats as VDEC_ERR_INVALID_INPUT is
      // returned only when decoder is fed with invalid data
      // and this should not be treated as failure in decoding

#ifdef FEATURE_QTV_VIDEO_RENDERING_QUALITY_ENABLE
#error code not present
#endif // FEATURE_QTV_VIDEO_RENDERING_QUALITY_ENABLE
      if (overallDataState == Media::DATA_UNDERRUN)
      {
        // Underruns are very common and not cause for concern.
        QTV_MSG_PRIO2( QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                       "Decoder is fed with invalid data: %d overallDataState %d",
                       err, overallDataState );
      }
      else
      {
        // Debug message to know that decoder is fed with invalid data and
        // because of which small chunks of data might not be decoded
        QTV_MSG_PRIO2( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                       "Decoder is fed with invalid data: %d overallDataState %d",
                       err, overallDataState );
      }
      break;
    }
    case VDEC_ERR_MYSTERY_ERROR:
    {
#ifdef FEATURE_QTV_VIDEO_RENDERING_QUALITY_ENABLE
#error code not present
#endif // FEATURE_QTV_VIDEO_RENDERING_QUALITY_ENABLE
    }
    default:
    {
      UpdateStats(FAILED_DECODE);
      if (overallDataState == Media::DATA_UNDERRUN)
      {
        // Underruns are very common and not cause for concern.
        QTV_MSG_PRIO2( QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                       "Failure in vdec_decode: %d overallDataState %d",
                       err, overallDataState );
      }
      else
      {
        QTV_MSG_PRIO2( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                       "Failure in vdec_decode: %d overallDataState %d",
                       err, overallDataState );
      }
    }
  } // switch ( err )

  // Under no circumstances should we wait for videoSyncEvent
  // if we're single-stepping.
  //
  bSleepAfter = bSleepAfter && !bSingleStepOn;

  /* If overallDataState is not one of a few known nice & tidy states,
  ** sleeping must be avoided - we need to process whatever weirdness
  ** is transpirin'! */
  bSleepAfter = bSleepAfter && ( overallDataState == Media::DATA_OK );

  return ( bQueueResult && bDecodeResult );
}

/* ======================================================================
FUNCTION:
  VideoDec::ShowNextFrame

DESCRIPTION:
  Shows the next frame

INPUT/OUPUT PARAMETERS:

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::ShowNextFrame()
{
  // This timeout is the longest we could take on one frame if we are decoding
  // 15FPS content (15FPS seems a common bottom-point), times 150% for slop.
  // So: ( 1000 ms/s / min f/s ) * ( 3 / 2 ) = 1500 / min f/s

  const int DECODE_TIMEOUT_MS   = (1500 / 15);
  const int SINGLE_STEP_TIMEOUT_MS   = (10*DECODE_TIMEOUT_MS);
  const int DECODE_CHECK_IVL_MS = 10;

  int  decodeTimeRemaining;
  int singleStepTimeRemaining = 0;
  bool bRet = false;
  bool bSleepAfter;
  bool renderResult = false;

  /* Set the single step flag to true to indicate that we wish to render the
  ** frame without having to meet the AV sync conditions for display. This
  ** flag is checked by the renderer thread.
  */
  bSingleStepOn = true;

  // We probably have a decoded frame queued and ready for rendering.
  // Try that before doing anything tougher.
  //
  if ( pPlayer != NULL && pPlayer->m_pRenderer != NULL )
  {
    renderResult = pPlayer->m_pRenderer->single_step();
  }

  if ( renderResult )
  {
    bSingleStepOn = false;
  }
  else // failed to render on the first try
  {
    // If RenderOneFrame couldn't get a frame to render, it'll fail to clear
    // bSingleStepOn.  If that's the case, decode a frame and render that!
    //
    for
    (
      singleStepTimeRemaining = SINGLE_STEP_TIMEOUT_MS;
      bSingleStepOn && singleStepTimeRemaining > 0;
      singleStepTimeRemaining -= DECODE_TIMEOUT_MS
    )
    {
    if ( DecodeOneFrame( bSleepAfter ) )
    {
      // Give the DSP a chance to get the frame decoded before we
      // try and render it...
      //
      for
      (
        decodeTimeRemaining = DECODE_TIMEOUT_MS;
        bSingleStepOn && decodeTimeRemaining > 0;
        decodeTimeRemaining -= DECODE_CHECK_IVL_MS
      )
      {
        if ( pPlayer != NULL && pPlayer->m_pRenderer != NULL )
        {
          zrex_sleep( DECODE_CHECK_IVL_MS );
          renderResult = pPlayer->m_pRenderer->single_step();
        }
        else
        {
          renderResult = false;
        }

        if ( renderResult )
        {
          bSingleStepOn = false;
        }
      }

      if ( bSingleStepOn )
      {
        QTV_MSG( QTVDIAG_VIDEO_TASK,
                 "Failed to render after decode during single step!" );
      }
    }
    else
    {
      QTV_MSG( QTVDIAG_VIDEO_TASK,
               "DecodeOneFrame failed during step forward!" );
    }
    }  // for

    // It's possible that our call to DecodeOneFrame has just moved us
    // past the end-of-clip.  If so, we need to tell the player this.
    //
    switch ( GetDataState() )
    {
    case Media::DATA_END:
      if( GetOutstandingFrameCount() == 0  && pPlayer != NULL )
      {
        QTV_MSG( QTVDIAG_VIDEO_TASK, "Video done decoding during single-step");
        pPlayer->bPlayDoneDecoding = true;
      }
      break;

    case Media::DATA_ERROR:
      //error ending
      (void)AbortDecoding();
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "video decode error during single-step" );
      break;

    default:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failed to single-step" );
      break;
    }
  } // failed to render on the first try

  /* The bSingleStepOn flag is set to false if the frame has been successfully
  ** rendered, so we need to check for that here to determine whether the
  ** previous frame is available for display.
  */
  bRet = !bSingleStepOn;
  bSingleStepOn = false;

  return bRet;
}

/* ======================================================================
FUNCTION:
  VideoDec::ShowPrevFrame

DESCRIPTION:
  Shows the previous frame.

INPUT/OUPUT PARAMETERS:

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::ShowPrevFrame()
{
  // We don't support step-back...

  return false;
}

/* ======================================================================
FUNCTION:
  VideoDec::setStopH263VideoInitFlag

DESCRIPTION:

  Set the flag to stop H.263 decoder initialization during initial preroll
  buffering. Flag is not valid for pseudo streaming and local file playback


INPUT/OUPUT PARAMETERS:

RETURN VALUE:
 NONE

SIDE EFFECTS:
 None.
========================================================================== */
void VideoDec::setStopH263VideoInitFlag(void)
{
  QTV_MSG(QTVDIAG_VIDEO_TASK,"H.263 video init stop flag is SET");
  stopH263VideoInit = true;
}

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
/* ======================================================================
FUNCTION:
  VideoDec::RotateVideo

DESCRIPTION:
  Rotate the video in the specified direction.

  This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.

INPUT/OUPUT PARAMETERS:
  direction

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::RotateVideo(QtvPlayer::RotationType direction)
{
  MP4TransformType videoRotation;
  bool bOK=true;

  switch(direction)
  {
  case QtvPlayer::ROTATE_90_CW:
    videoRotation = MP4_ROTATE_RIGHT;
    break;

  case QtvPlayer::ROTATE_90_CCW:
    videoRotation = MP4_ROTATE_LEFT;
    break;

  case QtvPlayer::ROTATE_180:
    videoRotation = MP4_ROTATE_180;
    break;

  case QtvPlayer::ROTATE_NO:
  default:
    videoRotation = MP4_NO_TRANSFORM;
    break;
  };

  if(m_pDecoderSettings)
  {
    bOK = PVRotateVideo(videoRotation);
  }

  // Since we are going to be returning true, we must ensure the callback is
  // notified.
  if (bOK)
  {
    pPlayer->NotifyTransform(Common::VIDEO_ROTATION_DONE);
  }

  return bOK;
}

/* ======================================================================
FUNCTION:
  VideoDec::ScaleVideo

DESCRIPTION:
  Scale the video by the specified scale factor.

This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.


INPUT/OUPUT PARAMETERS:
  scale

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::ScaleVideo(QtvPlayer::ScalingType scale)
{
  MP4TransformType videoScaling;
  bool bOK=true;

  switch(scale)
  {
    case QtvPlayer::SCALE_4X_ZOOM:
      videoScaling = MP4_4X_ZOOM;
      break;

    case QtvPlayer::SCALE_2X_ZOOM:
      videoScaling = MP4_2X_ZOOM;
      break;

    case QtvPlayer::SCALE_2X_SHRINK:
      videoScaling = MP4_2X_SHRINK;
      break;

    case QtvPlayer::SCALE_4X_SHRINK:
      videoScaling = MP4_4X_SHRINK;
      break;

    case QtvPlayer::SCALE_NONE:
    default:
      videoScaling = MP4_NO_TRANSFORM;
      break;
  };

  if(m_pDecoderSettings)
  {
    bOK = PVScaleVideo(videoScaling);
  }

  // Since we are going to be returning true, we must ensure the callback is
  // notified.
  if (bOK)
  {
    pPlayer->NotifyTransform(Common::VIDEO_SCALING_DONE);
  }

  return bOK;
}
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#ifdef FEATURE_QTV_XSCALE_VIDEO
/* ======================================================================
FUNCTION:
  VideoDec::ScaleVideo

DESCRIPTION:
  Scale the video by the specified scale factor via the DSP xScalar
  module.

INPUT/OUPUT PARAMETERS:
  scaledOutFrameWidth
    output width of the scaled frame.
  scaledOutFrameHeight
    output height of the scaled frame.
  VDEC_SCALE_CB_FN scaleCbFn
    callback function to return the status of the Scale() function call.
    We need this since the decoder might not have been
    created when the video player gets the xScale command.
  void * const pCbData
    the client data the application passed in.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::ScaleVideo(uint32 scaledOutFrameWidth,
                          uint32 scaledOutFrameHeight,
                          VDEC_SCALE_CB_FN scaleCbFn,
                          void * const     pCbData)
{

  VDEC_ERROR         err( VDEC_ERR_EVERYTHING_FINE );

  if(m_vdec)
  {
    VDEC_DIMENSIONS  outFrameSize;
    outFrameSize.width  = scaledOutFrameWidth;
    outFrameSize.height = scaledOutFrameHeight;

    err = vdec_scale( m_vdec, outFrameSize, scaleCbFn, pCbData);
  }
  else
  {
    //Save for later use after decoder gets created
    m_outFrameSize.width  = scaledOutFrameWidth;
    m_outFrameSize.height = scaledOutFrameHeight;
    m_scaleCbFn = scaleCbFn;
    m_scaleCbData = pCbData;
  }

  return (err==VDEC_ERR_EVERYTHING_FINE);
}
#endif /* FEATURE_QTV_XSCALE_VIDEO */


/* ======================================================================
FUNCTION:
  VideoDec::SetAudioOverrideFormat

DESCRIPTION:
  Set the audio override format to the specified format

INPUT/OUPUT PARAMETERS:
  format

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::SetAudioOverrideFormat(int format)
{
  if(m_pDecoderSettings == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "m_pDecoderSettings is NULL");
    return false;
  }
  /* no need to convert to VDEC format here, we will convert while sending to VDEC */
  m_pDecoderSettings->audioFormat = format;

  QTV_MSG_PRIO1(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH, "VideoDec::SetAudioOverrideFormat: %d", format);

  return true;
}

//////////////////////////////////////////////////////////////////////
//
// Info retrieval
//
////////////////////////////////////////////////////////////////////
/* ======================================================================
FUNCTION:
  VideoDec::GetDataState

DESCRIPTION:
  Get the data state

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
   Return the data state

SIDE EFFECTS:
  None.
========================================================================== */
Media::MediaStatus VideoDec::GetDataState()
{
  return overallDataState;
}

/* ======================================================================
FUNCTION:
  VideoDec::GetWidth

DESCRIPTION:
  Get the width

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  Return the width

SIDE EFFECTS:
  None.
========================================================================== */
long VideoDec::GetWidth()
{
  return videoInfo.originalWidth;
}

/* ======================================================================
FUNCTION:
  VideoDec::GetHeight

DESCRIPTION:
  Return the height.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  Return the height.

SIDE EFFECTS:
  None.
========================================================================== */
long VideoDec::GetHeight()
{
  return videoInfo.originalHeight;
}

/* ======================================================================
FUNCTION:
  VideoDec::GetPosition

DESCRIPTION:
  Get the position (number of frames processed)

INPUT/OUPUT PARAMETERS:
  pos

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::GetPosition(int32 &pos)
{
  // This may be called from the player thread at any time.
  QCUtils::EnterCritSect(&frameReqUpdate_CS);
  pos=nFramesProcessed;
  QCUtils::LeaveCritSect(&frameReqUpdate_CS);
}

/* ======================================================================
FUNCTION:
  VideoDec::GetStatistics

DESCRIPTION:
  Get statistics

INPUT/OUPUT PARAMETERS:
  stat
  lastFrame

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::GetStatistics(QtvPlayer::AudioVideoStatisticsT &stat,
                             Common::LastFrame &lastFrame)
{
  // This may be called from the API thread at any time.
  QCUtils::EnterCritSect(&videoStat.CS);

  if ( pMpeg4 != NULL )
  {
    stat.Video.format             = (int)pMpeg4->GetVideoCodecType();
  }
  stat.Video.frames             = nFramesProcessed;
  stat.Video.nIFrameTally       = videoStat.nIFrameTally;
  stat.Video.nPFrameTally       = videoStat.nPFrameTally;
  stat.Video.nBFrameTally       = videoStat.nBFrameTally;
  stat.Video.framesInterpolated = videoStat.nFramesInterpolated;
  stat.Video.skippedDecode      = videoStat.nDroppedDecode;
  stat.Video.skippedDisplay     = videoStat.nDroppedDisplay; // drop Before Display (UI Drop)
  stat.Video.avSyncDrops     =  videoStat.nDroppedAVSync; // drop due to AV sync
  stat.Video.skippedDisplayDueToErrs = videoStat.nDropMacroBlockErrs;
  stat.Video.failedDecode       = videoStat.nFailedDecode;
  stat.Video.maxDrop            = videoStat.nMaxChunk;

  int32 playMsec;
  if(isPlayTimeCalculated)
  {
    playMsec = videoStat.nPlayMsec;
  }
  else
  {
    //If this function is called before calculating PlayMsec, then
    //calculate here
    playMsec = videoStat.nPlayMsec + (ZUtils::Clock() - videoStat.tResume);
    StartPlayInterval(ZUtils::Clock());
  }

  if ( playMsec > 0 )
  {
     QCUtils::EnterCritSect(&frameReqUpdate_CS);
     videoStat.nFPSProcessed = (float)nFramesProcessed;
     videoStat.nFPSRendered  = (float)nFramesRendered;
     QCUtils::LeaveCritSect(&frameReqUpdate_CS);
     videoStat.nFPSProcessed /= playMsec;
     videoStat.nFPSProcessed *= 1000.0;
     videoStat.nFPSRendered  /= playMsec;
     videoStat.nFPSRendered  *= 1000.0;
  }

  stat.Video.decodedFrameRate   = (uint32) (ceil (videoStat.nFPSProcessed));
  stat.Video.displayedFrameRate = (uint32) (ceil (videoStat.nFPSRendered));

  stat.AVSync.avSyncValid       = TRUE;
  stat.AVSync.videoAheadMsec    = videoStat.nMaxAheadMsec;
  stat.AVSync.videoBehindMsec   = videoStat.nMaxBehindMsec;

  lastFrame = videoStat.lastFrame;

  stat.Video.encodedBitrate   = 0;
  stat.Video.encodedFrameRate = 0;

  if( pMpeg4 != NULL )
  {
    uint32 videoTrackID = pMpeg4->GetVideoTrackID(0);
    switch( pMpeg4->GetSource() )
    {
      case Media::FILE_SOURCE:
      {
         FileBase * pFile = pMpeg4->GetFile();
         if(pFile)
         {
            stat.Video.encodedBitrate   = pFile->getTrackAverageBitrate(videoTrackID);
            stat.Video.encodedFrameRate = (uint32) (ceil(pFile->getTrackVideoFrameRate(videoTrackID)));
         }
         break;
      }
      case Media::STREAM_SOURCE:
      {
        stat.Video.encodedBitrate = pMpeg4->GetVideoBitRate(0);
        stat.Video.encodedFrameRate = (uint32)pMpeg4->GetVideoFrameRate(0);
        if(stat.Video.encodedFrameRate == 0 )
        {
          if (videoStat.averageInterFrameIntervalMsec)
          {
            stat.Video.encodedFrameRate = (uint32)(1000/videoStat.averageInterFrameIntervalMsec);
          }
        }
        break;
      }
#ifndef FEATURE_QTV_GENERIC_BCAST
      case Media::BCAST_FLO_SOURCE:
      {
        /* Start off with base layer */
        stat.Video.encodedBitrate = pMpeg4->GetAvgVideoBitRate(0);
        if (pMpeg4->GetNumberOfVideoTracks() > 1)
        {
          /* Add in enhanced layer */
          stat.Video.encodedBitrate  += pMpeg4->GetAvgVideoBitRate(1);
        }

        stat.Video.encodedFrameRate = (uint32)pMpeg4->GetVideoFrameRate();


        break;
      }
#endif

      default: /* Already been initialized to 0 */
        break;
    }
#ifdef FEATURE_QTV_GENERIC_BCAST
   if(IsLiveBcastMediaSource(pMpeg4))
    {
      /* Start off with base layer */
      stat.Video.encodedBitrate = pMpeg4->GetAvgVideoBitRate(0);
      if (pMpeg4->GetNumberOfVideoTracks() > 1)
      {
        /* Add in enhanced layer */
        stat.Video.encodedBitrate  += pMpeg4->GetAvgVideoBitRate(1);
      }

      stat.Video.encodedFrameRate = (uint32)pMpeg4->GetVideoFrameRate();
    }
#endif
  }

  QCUtils::LeaveCritSect(&videoStat.CS);
}


#ifdef SHOW_INFO
/* ======================================================================
FUNCTION:
  VideoDec::ShowInfo

DESCRIPTION:
  show video clip info

INPUT/OUPUT PARAMETERS:
  bEnd

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::ShowInfo(bool bEnd)
{
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "***** Video Frames %d",nFramesProcessed);

  QTV_MSG( QTVDIAG_VIDEO_TASK, "Video Info" );
  QTV_MSG2( QTVDIAG_VIDEO_TASK, "   Height %d Width %d ",videoInfo.imageHeight,videoInfo.imageWidth);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "   N Layers %d",videoInfo.nLayers);

  //Video perf info
  if (bEnd)
  {
    uint16 fpsWhole = (uint16) ZWHOLE(videoStat.nFPSRendered);
    uint16 fpsDecimal = (uint16) ZDECIMAL(videoStat.nFPSRendered);

    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Play Time= %d Msec",videoStat.nPlayMsec);
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Frames/Sec %d.%03d", fpsWhole, fpsDecimal );

    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Max Ahead %d Behind %d",videoStat.nMaxAheadMsec,videoStat.nMaxBehindMsec);
  }

  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Timestamp range %ld %ld",videoStat.nFirstTimestamp,videoStat.nLastTimestamp);

  //Decode buf info
  QTV_MSG( QTVDIAG_VIDEO_TASK, "Decode Info" );
  for (int i=0;i<videoInfo.nLayers;i++)
  {
    QTV_MSG1( QTVDIAG_VIDEO_TASK, "   Max Size: %d",decodeBuf.maxSize[i]);
  }

  //video stats
  QTV_MSG( QTVDIAG_VIDEO_TASK, "Video Stats" );
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Total Number of Frames: %d",nFramesProcessed);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Total Number of I-Frames: %d",videoStat.nIFrameTally);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Total Number of P-Frames: %d",videoStat.nPFrameTally);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Total Number of B-Frames: %d",videoStat.nBFrameTally);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Frame Interpolated %d",videoStat.nFramesInterpolated);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Fail decode %d",videoStat.nFailedDecode);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Dropped Decode %d", videoStat.nDroppedDecode);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Dropped Convert %d",videoStat.nDroppedConvert);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Dropped Display %d",videoStat.nDroppedDisplay);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Dropped Macroblock Errors %d",videoStat.nDropMacroBlockErrs);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Max Chunk %d",videoStat.nMaxChunk);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Stop PresTime %ld",videoStat.nStopPresTime);
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Last frame %d",videoStat.lastFrame.nFrame);
  QTV_MSG2( QTVDIAG_VIDEO_TASK, "timestamp %ld time %ld",videoStat.lastFrame.nTimestamp,videoStat.lastFrame.nTime);

//  QTV_MSG( QTVDIAG_VIDEO_TASK, " *****************" );
}
#endif //show

//////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoDec::VideoDec

DESCRIPTION:
  Default constructor

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
VideoDec::VideoDec
(
QCConditionType   &wakeupSync
)
: m_wakeupSync       ( wakeupSync ),
m_vdec( NULL ),
m_outstandingFrameCount( 0 ),
m_pVDEC_InputBuffer( &m_VDEC_InputBuffer ),
  m_bPreParseHeaderStatus(false),
m_last_timestamp( 0 ),
m_maxTimestamp( MAX_UINT32 ),
  m_pVideoDelayAnalyzer( 0 )
{
  //  init allocated storage ptrs
  int idx;
  for (idx=0;idx<COMMON_MAX_LAYERS;idx++)
  {
    decodeBuf.pData[idx] = NULL;
  }
  m_pDecoderSettings = NULL;
  memset( &decodeBuf, 0, sizeof( DecodeBuffer ) );
  QCUtils::CreateCondition(&m_suspendSync,false,false);

  pMpeg4=NULL;
  pBitfile=NULL;
  bRebufferHappened = false;
  m_uLastTimestampBeforeRebuffering = 0;

  bSingleStepOn = false;
  stopH263VideoInit = false;
  LastFrameRenderedTS = 0;
  pPlayer = NULL;

#ifdef FEATURE_QTV_6250_ARM_CLK_COMPENSATION
  // added the following variables to support drift fix
  driftConstant = 0;
  reposition = false;
  resumedTime=0;
#endif

#ifdef FEATURE_H264_DECODER
  //Blob pointer
  ppHeaderBlob = NULL;
#endif /* FEATURE_H264_DECODER */

#ifdef FEATURE_QTV_XSCALE_VIDEO
  m_outFrameSize.width= 0;
  m_outFrameSize.height=0;
  m_scaleCbFn =0;
  m_scaleCbData =0;
#endif //FEATURE_QTV_XSCALE_VIDEO

  QCUtils::InitCritSect(&videoStat.CS);
  QCUtils::InitCritSect(&frameReqUpdate_CS);

  InitPlayData();
  InitVideoInfo();

  m_playbackSpeedNum = 1;
  m_playbackSpeedDen = 1;

  lastRenderedFrameInfo.layer = 0;
  lastRenderedFrameInfo.bIsIntra = false;
  lastRenderedFrameInfo.timestamp = 0;

  QCUtils::InitCritSect( &m_shutdown_CS );
  QCUtils::InitCritSect( &m_vdecCreationLock );

  m_pDecoderSettings = InitDecoderSettings( (int)Media::UNKNOWN_CODEC );
   if (m_pDecoderSettings == NULL)
   {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "VideoDec::VideoDec, m_pDecoderSettings memory allocation faild" );
   }
   // assign it to default value
  AudioMediaCodecTypeSet = Media::UNKNOWN_CODEC;
  nInCallVocodertype = Media::UNKNOWN_CODEC;
}

/* ======================================================================
FUNCTION:
  VideoDec::AbortDecoding

DESCRIPTION:
  Wait for outstanding DecodeOneFrame() calls to complete,
  Called from the video decoding thread.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
========================================================================== */
bool VideoDec::AbortDecoding()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoDec::AbortDecoding..." );

  (void)Set_State( VIDEODEC_DISABLED );

  /* Flush the render (decoded) queue */
  pPlayer->Flush();

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "VideoDec::AbortDecoding Done, Disabled" );

  return true;
}

/* ======================================================================
FUNCTION:
  VideoDec::~VideoDec

DESCRIPTION:
  Destructor

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
VideoDec::~VideoDec()
{

  QCUtils::EnterCritSect( &m_shutdown_CS );
  if (m_pDecoderSettings)
  {
#ifdef FEATURE_QTV_IMEM //Sathiya
#error code not present
#else
    QTV_Free( m_pDecoderSettings );
#endif
    m_pDecoderSettings = NULL;
  }
  QCUtils::LeaveCritSect( &m_shutdown_CS );

  QCUtils::DestroyCondition(&m_suspendSync);
  pPlayer = NULL;

  //initialized to false
  isbFirstFrameDecoded = false;

  QCUtils::DinitCritSect(&videoStat.CS);
  QCUtils::DinitCritSect(&frameReqUpdate_CS);
  QCUtils::DinitCritSect( &m_shutdown_CS );
  QCUtils::DinitCritSect( &m_vdecCreationLock );

}/*lint!e1540 */

/* ======================================================================
FUNCTION
  DoGenericVdecInit

DESCRIPTION
  Initializes our vdec in preparation for decoding.
  This handles only the common, most general stuff.  Fancier video types
  may have to massage header data before calling this function (or do
  something else altogether)

RETURN VALUE
  boolean:  TRUE = yay, FALSE = boo

========================================================================== */
bool VideoDec::DoGenericVdecInit(VDEC_BLOB *pConfig,Media * pMpeg4)
{

  VDEC_ERROR returnCode;
  VDEC_DIMENSIONS frameSize;
  VDEC_CONCURRENCY_CONFIG concurrencyConfig;
  VDEC_PARAMETER_DATA parm_data;

  //fill frameSize with default values
  frameSize.height = MP4_MAX_DECODE_HEIGHT;
  frameSize.width  = MP4_MAX_DECODE_WIDTH;

  if (m_vdec == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
                 "VideoDec::DoGenericVdecInit failed!! error - null vdec stream id");
    return false;
  }

  QCUtils::EnterCritSect( &m_vdecCreationLock );

  ASSERT( videoInfo.nLayers <= MAX_MP4_LAYERS );

  /* On audio configuration information:
  **
  ** Audio information can come from multiple places, in multiple formats.
  ** We have to convert them all to a VDEC type here.
  **
  ** Here are the rules:
  ** 1) audOverrideFormat is one of the MP4QDSP_ defines from mp4codec.h.
  **    if this is anything but MP4QDSP_INVALID, we must use that.
  ** 2) if rule 1 doesn't apply, then we use audioFormat.  This is a
  **    Media::CodecType enum.
  */
  concurrencyConfig = VDEC_CONCURRENT_NONE;

  /*
    if audio override format set
    */
  if (m_pDecoderSettings->audioFormat != (int)Media::UNKNOWN_CODEC)
  {
     //incase of Audio override we should convert the override format to VDEC understandable format
     concurrencyConfig = MapToVdecConcurrencyFormat (m_pDecoderSettings->audioFormat );
  }

  if (concurrencyConfig == VDEC_CONCURRENT_NONE)
  {
#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif
     switch(( Media::CodecType )AudioMediaCodecTypeSet)
     {
       case Media::UNKNOWN_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_NONE;
       break;
       case Media::EVRC_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_EVRC ;
       break;
       case Media::QCELP_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_QCELP;
       break;
       case Media::AAC_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_AAC;
       break;
       case Media::BSAC_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_BSAC;
       break;
       case Media::GSM_AMR_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_AMR;
       break;
       case Media::MP3_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_MP3;
       break;
       case Media::WMA_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_NONE; //Right now, no WMA support. This will change soon.
       break;
       case Media::CONC_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_CONC;
       break;
       case Media::AMR_WB_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_AMR_WB;
       break;
       case Media::AMR_WB_PLUS_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_AMR_WB_PLUS;
       break;
       case Media::EVRC_NB_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_EVRC;
       break;
       case Media::EVRC_WB_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_EVRC;
       break;
       default:
         concurrencyConfig = VDEC_CONCURRENT_NONE;
       break;
     }

      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                    "Audio Codec Set from Media Source : AudioMediaCodecTypeSet (%d) -> concurrencyConfig (%d )",
                     AudioMediaCodecTypeSet, concurrencyConfig);
  }

  if (videoInfo.videoCodec == Media::H264_CODEC)
  {
  #ifdef FEATURE_H264_DECODER
    parm_data.sizeOfNalLengthField.sizeOfNalLengthField = H264_NAL_LENGTH_SIZE;
  #endif
    vdec_set_parameter(m_vdec,VDEC_PARM_SIZE_OF_NAL_LENGTH_FIELD,&parm_data);

#ifndef FEATURE_QTV_GENERIC_BCAST
    if (pMpeg4->GetSource() == Media::BCAST_FLO_SOURCE ||
        pMpeg4->GetSource() == Media::BCAST_ISDB_SOURCE)
    {
      parm_data.startCodeDetection.bStartCodeDetectionEnable = TRUE;
    }
    else
    {
#ifdef FEATURE_FRE
      parm_data.startCodeDetection.bStartCodeDetectionEnable = FALSE;
#else
#ifdef FEATURE_H264_DECODER
      parm_data.startCodeDetection.bStartCodeDetectionEnable =
      (boolean)pMpeg4->GetH264StartCodeDetectFlag(0);
#endif /* FEATURE_H264_DECODER */
#endif
    }
#else
#ifdef FEATURE_H264_DECODER

    // As the external media model exports the flag to identify whether the
    // H264 byteStream format used or not, similar thing is enquired
    // against all the media models. Broadcast media by default shall
    // return true to take care of the media FLO case. File/Stream media
    // shall return false for this accessor method.
    parm_data.startCodeDetection.bStartCodeDetectionEnable =
    (boolean)pMpeg4->GetH264StartCodeDetectFlag(0);
#endif /* FEATURE_H264_DECODER */
#endif
    vdec_set_parameter(m_vdec,VDEC_PARM_START_CODE_DETECTION,&parm_data);
#ifdef FEATURE_FRE
    parm_data.freEnable.bFreEnable = pPlayer->m_pQtvConfigObject->GetQTVConfigItem(QtvConfig::QTVCONFIG_ENABLE_QFRE);
#else
    parm_data.freEnable.bFreEnable = false;
#endif
    vdec_set_parameter(m_vdec,VDEC_PARM_FRE_ENABLE,&parm_data);
  }//end of H264 specific initialization

  parm_data.deblockerEnable.bDeblockerEnable = TRUE;
  vdec_set_parameter(m_vdec,VDEC_PARM_DEBLOCKER_ENABLE,&parm_data);
#ifdef FEATURE_QTV_XSCALE_VIDEO
  /*Enable xScaling support.This will enable decoder know that
    QTV app may exercise xScaling option*/
  if (videoInfo.videoCodec == Media::H264_CODEC)
    parm_data.xScalingEnable.bScalingEnable = TRUE;
  else
    parm_data.xScalingEnable.bScalingEnable = FALSE;
  vdec_set_parameter(m_vdec,VDEC_PARM_ENABLE_XSCALING,&parm_data);
#endif


  parm_data.TSMultiplyFactor.TSMultiplyFactor = pPlayer->m_pQtvConfigObject->GetQTVConfigItem(QtvConfig::QTVCONFIG_ENABLE_BRAZIL_SPECIFIC_H264_VUI_PARAM_TYPE);
  vdec_set_parameter(m_vdec,VDEC_PARM_SET_TS_MULTIPLY_FACTOR_BRAZIL,&parm_data);

  parm_data.VidKeyFrameEnable.bEnableVideoAtKeyFrame = pPlayer->m_pQtvConfigObject->GetQTVConfigItem(QtvConfig::QTVCONFIG_ENABLEVIDEO_AT_KEY_FRAME);
  vdec_set_parameter(m_vdec,VDEC_PARM_ENABLE_VIDEO_AT_KEY_FRAME,&parm_data);

#ifdef FEATURE_QTV_H264_VLD_DSP
  if (Media::BCAST_FLO_SOURCE == pMpeg4->GetSource())
  {
    //Disable VLD in DSP if source is MFLO
    parm_data.VLDinDSP.bEnableVDLinDSP  = FALSE;
    vdec_set_parameter(m_vdec,VDEC_PARM_ENABLE_VLD_IN_DSP,&parm_data);
  }
#endif

  GetFrameDimensionsFromMedia(pMpeg4);
  frameSize.height = videoInfo.imageHeight;
  frameSize.width  = videoInfo.imageWidth;

  SetIncallStateParameter();

  returnCode = vdec_initialize(m_vdec,
                               pConfig,
                               videoInfo.nLayers,
                               m_Malloc,
                               m_Free,
                               FrameBufferReadyCallback,
                               this,
                               frameSize,
                               concurrencyConfig);

  QCUtils::LeaveCritSect( &m_vdecCreationLock );

  if (returnCode != VDEC_ERR_EVERYTHING_FINE)
  {
    last_vdec_err = returnCode;
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,"vdec_initialize failed!! error - %d",returnCode);
  }
#ifdef FEATURE_FRE
  parm_data.freEnable.bFreEnable = m_bEnableQFRE.bFreEnable;
  vdec_set_parameter(m_vdec, VDEC_PARM_FRE_ENABLE, &parm_data);
#endif

#ifdef FEATURE_QTV_XSCALE_VIDEO
  if(m_vdec && m_scaleCbFn)
  {
     (void)vdec_scale( m_vdec,m_outFrameSize,m_scaleCbFn,m_scaleCbData);
      m_scaleCbFn = 0;
  }
#endif //FEATURE_QTV_XSCALE_VIDEO

  #ifdef FEATURE_QTV_AVI
  if (videoInfo.videoCodec == Media::MPEG4_CODEC
      || videoInfo.videoCodec == Media::H263_CODEC
      || videoInfo.videoCodec == Media::DIVX311_CODEC
      )
  {
    returnCode = InitExternalClock();
    if(returnCode != VDEC_ERR_EVERYTHING_FINE)
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
                    "InitExternalClock!! error - %d",returnCode);
    }
  }
  #endif

  return (returnCode == VDEC_ERR_EVERYTHING_FINE);
}
/* ======================================================================
FUNCTION
  InitExternalClock

DESCRIPTION
  Initializes the clock to internal if file playback or if codec is Mpeg4.

RETURN VALUE
  VDEC_ERROR

========================================================================== */

VDEC_ERROR VideoDec::InitExternalClock( void )
{
  bool external_clk_flag = true;
  VDEC_PARAMETER_DATA parm_data;
  Media* pVideoMedia = NULL;

  if((pPlayer != NULL) && (pPlayer->pAVPlayer != NULL))
  {
    pVideoMedia = pPlayer->pAVPlayer->GetVideoMedia();

    if((pPlayer->pAVPlayer->IsLocalMediaPLayback()) &&
       (pVideoMedia) && (pVideoMedia->isAviFileInstance()) &&
       (videoInfo.videoCodec == Media::MPEG4_CODEC))
    {
      external_clk_flag = false;
    }
  }

  if(external_clk_flag == true)
  {
    parm_data.externalClockEnable.bExternalClock = true;
  }
  else
  {
    parm_data.externalClockEnable.bExternalClock = false;
  }
  return vdec_set_parameter(m_vdec, VDEC_PARM_EXTERNAL_CLOCK_ENABLE,&parm_data);
}

/* ======================================================================
FUNCTION
  DoGenericVdecShutdown

DESCRIPTION
  Cleans up our vdec.

RETURN VALUE
  boolean:  TRUE = yay, FALSE = boo

========================================================================== */
boolean VideoDec::DoGenericVdecShutdown( void )
{
  boolean retVal = TRUE;
  VDEC_ERROR  vdec_error;

  QCUtils::EnterCritSect( &m_vdecCreationLock );

  if (m_vdec)
  {
    //Before destroying Suspend the decoder
    vdec_error = vdec_suspend( m_vdec );
    if(vdec_error == VDEC_ERR_EVERYTHING_FINE)
    {
      if (!QCUtils::WaitForCondition(&m_suspendSync,SUSPEND_TIMEOUT_MSEC))
      {
        QTV_MSG_PRIO(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR,"SuspendCalbackNot received");
        retVal =  false;
      }
      (void)vdec_destroy( m_vdec );
      m_vdec = NULL;
    }
    else
    {
      retVal = false;
    }
  }

  QCUtils::LeaveCritSect( &m_vdecCreationLock );
  return retVal;
}

/* ======================================================================
FUNCTION
  DoGenericVdecReuseFrame

DESCRIPTION
  Logs the reason for releasing frame and releases the buffer for reuse.

RETURN VALUE
  None

========================================================================== */
void VideoDec::DoGenericVdecReuseFrame
(
FrameReleaseReason  Reason,
VDEC_FRAME *        pFrame
)
{
  if (pFrame == NULL)
  {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR,
                  "DoGenericVdecReuseFrame got NULL pFrame" );
    return;
  }

  m_renderingFrames.Forget( pFrame );

  /* Check for out of order rendering. */
  if (Reason == FRAME_RENDERED)
  {
    if (LastFrameRenderedTS > pFrame->timestamp)
    {
      QTV_MSG_PRIO2( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                     "Frames rendered out of order. Last TS = %d, current TS = %d",
                     LastFrameRenderedTS,
                     pFrame->timestamp );
    }
    LastFrameRenderedTS = pFrame->timestamp;
  }

  /* Log the reason for reuse. */
  LogFrameReuseReason( Reason, pFrame->timestamp );

  /* Reuse the frame buffer. */
  if (m_vdec == pFrame->stream)
  {
    (void)vdec_reuse_frame_buffer( pFrame->stream, pFrame );
  }
  else
  {
    QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK,
                   QTVDIAG_PRIO_HIGH,
                   "stale reuse_frame ignored (%d)",
                   pFrame->timestamp );
  }

  //Poke the decoder when ever a frame is released from above irrespective of
  //the codec type. This is important when all the decoded frames are
  //sitting with the renderer and there is nothing further to drive the decoding
  //machinery. So as soon as a frame is released we should wakeup the video player
  //to further continue decoding.
  QCUtils::SetCondition( &m_wakeupSync );
}

/* ======================================================================
FUNCTION
  LogFrameReuseReason

DESCRIPTION
  Logs the reason for allowing a frame to be reused.

RETURN VALUE
  None

========================================================================== */
void VideoDec::LogFrameReuseReason
(
FrameReleaseReason  Reason,
uint32              TimeStamp
)
{
#ifndef PLATFORM_LTK
  log_qtv_frame_release_reason_type *pBuf = NULL;

  /* Check the log code status. */
  if (log_status( LOG_QTV_FRAME_BUFFER_RELEASE_REASON_C ))
  {
    /* Allocate a buffer for the log. */
    pBuf = ( log_qtv_frame_release_reason_type * ) log_alloc(
                                                            LOG_QTV_FRAME_BUFFER_RELEASE_REASON_C,
                                                            sizeof( log_qtv_frame_release_reason_type ) );

    if (pBuf)
    {
      /* Update the buffer statistics. */
      pBuf->ReleaseReason = ( uint8 )Reason;
      pBuf->TimeStamp     = TimeStamp;

      /* Log the buffer. */
      log_commit( pBuf );
    }
  }
#endif //!PLATFORM_LTK
}

/* ======================================================================
FUNCTION
  InputBufferFreeCallback

DESCRIPTION
  Callbacks for handling the release of input buffers from the vdec
  inside DecodeVideoFrame.

RETURN VALUE
  None, it's a callback.  Who'd listen?

========================================================================== */
void VideoDec::InputBufferFreeCallback
(
const VDEC_STREAM_ID      /* unused */ ,
VDEC_INPUT_BUFFER * const pBuffer,
void * const              pCbData
)
{
  VideoDec * const pThis = ( VideoDec * )pCbData;
  ASSERT( pThis );

  //
  // Don't put anything else in this stub.  All input-buffer handling
  // should be in InputBufferFreeHandler.
  //

  pThis->InputBufferFreeHandler( pBuffer );
}

/* ======================================================================
FUNCTION
  InputBufferFreeHandler

DESCRIPTION
  Callbacks for handling the release of input buffers from the vdec
  inside DecodeVideoFrame.

RETURN VALUE
  None, it's a callback.  Who'd listen?

========================================================================== */
void VideoDec::InputBufferFreeHandler
(
VDEC_INPUT_BUFFER * const pBuffer
)
{
  int nFramesPending( 0 );

  ASSERT( pBuffer );

  // Examine the input buffer and see how many frames are pending
  // as the result of its submission.
  for (int i = 0; i < pBuffer->layers; ++i)
  {
    if (pBuffer->bFrameEventPending[ i ] == true)
    {
      ++nFramesPending;
      QTV_MSG_PRIO3( QTVDIAG_DEBUG,
       QTVDIAG_PRIO_HIGH,
       "Frame event pending in layer %d @ timestamp %d nFramesPending %d",
        i,
        pBuffer->timestamp[ i ],
        nFramesPending);
    }
  }

  /* The application expects to see changes made to its buffer_size
  ** array which indicate how much of the buffers were actually used.
  ** Propogate those changes from our array to theirs.
  */
  for (int i = 0; i < pBuffer->layers; ++i)
  {
    decodeBuf.size[ i ] = pBuffer->buffer_size[ i ];
    decodeBuf.pos [ i ] = pBuffer->buffer_pos [ i ];
  }

  // When we first submitted this buffer, we increased OFC by the number
  // of occupied layers.  Now, to get the number correct again, we must
  // reduce by the number of layers which aren't going to generate a frame.

  (void)AlterOutstandingFrameCount( nFramesPending - pBuffer->layers,
                                    "input buffer free correction" );

  // All done!  Return this buffer for reuse.
  //
  m_pVDEC_InputBuffer = pBuffer;
}

/* ======================================================================
FUNCTION
  FrameBufferReadyCallback

DESCRIPTION
  Callback received when video frames are decoded.

RETURN VALUE
  None, it's a callback.  Who'd listen?

========================================================================== */
void VideoDec::FrameBufferReadyCallback
(
const VDEC_STREAM_ID, /* unused */
VDEC_CB_STATUS       status,
VDEC_FRAME * const   pFrame,
uint32               nBytesConsumed,
void * const         pCbData
)
{
  VideoDec * const pThis = ( VideoDec * )pCbData;
  ASSERT( pThis );

  // Don't put anything else in this stub.  All frame-buffer handling
  // should be in FrameBufferReadyHandler.

  /* "nBytesConsumed" variable declared but not used in the function,
       below statement is added to clear the compiler warning. */
  QTV_USE_ARG1(nBytesConsumed);
  pThis->FrameBufferReadyHandler( status, pFrame );
}

/* ======================================================================
FUNCTION
  FrameBufferReadyHandler

DESCRIPTION
  Callback received when video frames are decoded.

RETURN VALUE
  None.

========================================================================== */
void VideoDec::FrameBufferReadyHandler
(
const VDEC_CB_STATUS status,
VDEC_FRAME * const      pFrame
)
{
   if (pPlayer != NULL)
   {
       // in the player idle state ignore the frames
       if ( (pPlayer->GetState() == VideoPlayer::IDLE) &&
             (status != VDEC_STATUS_SUSPEND_DONE)      &&
             (status != VDEC_STATUS_EOS_INDICATION)    &&
             (status != VDEC_STATUS_FLUSH)             &&
             (status != VDEC_STATUS_FATAL_ERROR) )
       {
              QTV_MSG_PRIO(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
                  "VideoDec::FrameBufferReadyHandler:Player is in IDLE state, No need to Process FrameBufferReady");
              HandleDND_FrameReady( pFrame );
	       return;
       }
   }


  switch (status)
  {
  case VDEC_STATUS_GOOD_FRAME:
  {
    record_timestamp( pFrame );
    HandleFrameReady( pFrame );
    break;
  }
#if defined(FEATURE_FRE) || defined(FEATURE_H264_FRUC)
  case VDEC_STATUS_FRE_FRAME:
  {
    QTV_MSG_PRIO1(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_MED,
                  "VideoDec::FrameBufferReadyHandler:Recieved a FRUC Frame(%d)", status );
    UpdateStats(INTERPOLATED);
    if ( !bSingleStepOn )
    {
    HandleFrameReady( pFrame );
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
                  "VideoDec::FrameBufferReadyHandler:Recieved a FRUC Frame, ignoring during Single Step");
      HandleDND_FrameReady( pFrame );
    }
    break;
  }
#endif
  case VDEC_STATUS_DECODE_ERROR:
  {
#ifdef FEATURE_QTV_VIDEO_RENDERING_QUALITY_ENABLE
#error code not present
#endif // FEATURE_QTV_VIDEO_RENDERING_QUALITY_ENABLE
    /* Decode errors are treated just like do-not-display frames, which we
    ** inappropriately refer to as NOT_CODED, with one difference... */
    UpdateStats( FAILED_DECODE );
    HandleDND_FrameReady( pFrame );
    break;
  }
  case VDEC_STATUS_VOP_NOT_CODED:
  {
    /* VOP_NOT_CODED is misused, it actually means do-not-display.
    ** True NOT_CODED frames are concealed from us: the decoder presents
    ** them as regular decodes. */
    HandleDND_FrameReady( pFrame );
    break;
  }
  case VDEC_STATUS_FATAL_ERROR:
  {
    QTV_MSG_PRIO1(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL,
                  "Fatal error, aborting(%d)", status );
    overallDataState = Media::DATA_ERROR;
    break;
  }
  case VDEC_STATUS_EOS_INDICATION:
  {
    QTV_MSG_PRIO(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL,
                 "Eos Indication received..ignoring for now");
    break;
  }
  case VDEC_STATUS_SUSPEND_DONE:
  {
     QTV_MSG_PRIO(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,"Suspend CallBack received");
     QCUtils::SetCondition(&m_suspendSync);
     break;
  }
  case VDEC_STATUS_FLUSH:
  default:
  {
    /* These we don't care about and silently ignore. */
      QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                     "FrameBufferReadyCallback ignoring status %d",
                     status );
  }
  } /* switch ( status ) */


#ifndef PLATFORM_LTK
  if (pFrame)
  {
    LogFrameDecodeInfo( *pFrame );
  }
#endif /* ! PLATFORM_LTK */

  if ((status != VDEC_STATUS_SUSPEND_DONE) && (status != VDEC_STATUS_EOS_INDICATION))
  {
  /* A decode-done is one of the endpoint-events for decoding.
  ** Record it- and record it all the time!  We didn't discriminate
  ** recording them on the way IN, so we have to count them all on
  ** the way OUT, regardless of result. */
#if defined(FEATURE_FRE) || defined(FEATURE_H264_FRUC)
  if(status != VDEC_STATUS_FRE_FRAME)
#endif
   (void)AlterOutstandingFrameCount( -1, "frame done" );
#ifdef FEATURE_FRE
if(m_bEnableQFRE)
{
  unsigned int ofc;
  ofc = GetOutstandingFrameCount();
  if(!ofc)
    SetPrerollRendererCount(1);
}
#else
  unsigned int ofc;
  ofc = GetOutstandingFrameCount();
  if(!ofc)
    SetPrerollRendererCount(1);
#endif
}
}

/* ======================================================================
FUNCTION
  HandleFrameReady

DESCRIPTION
  Handles 'frame ready' events for frames that should be displayed

RETURN VALUE
  None.
========================================================================== */
void VideoDec::HandleFrameReady( VDEC_FRAME * const pFrame )
{
  if ( !StateIsOkForFrameReady() )
  {
    return;
  }

  if ( !pFrame )
  {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR,
                  "HandleFrameReady got nonsense 0 pFrame" );
    return;
  }

  // If the timestamp and increment are  undefined, we will 'recover' it by assuming
  // 15fps video. If the timestamp is undefined and increment is defined, we will
  // take the increment.
  repair_timestamp( pFrame );

  // By default frame is syncd
  bool bFrameSyncToPCR = true;
  #ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
  #endif
  if ( !pPlayer->m_pRenderer  || !bFrameSyncToPCR)
  {
    /* Without a renderer, we've got nobody to give the frame to. Best
    ** we can do is complain and release the frame. */
    if(!pPlayer->m_pRenderer)
    {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR,
                  "HandleFrameReady couldn't find renderer" );
    }
    DoGenericVdecReuseFrame( VideoDec::FRAME_DROPPED_RENDER_SUBMIT_FAILURE,
                             pFrame );
    return;
  }

  UpdateStats(CONVERTED, pFrame);

  // A healthy frame should be punted right to the renderer.

  if
  (
    pPlayer->decIF.m_vdec != NULL &&
    pPlayer->decIF.m_vdec == pFrame->stream &&
    pFrame->pBuf          != NULL
  )
  {
#ifdef FEATURE_QTV_VIDEO_RENDERING_QUALITY_ENABLE
#error code not present
#endif // FEATURE_QTV_VIDEO_RENDERING_QUALITY_ENABLE
    {
      /* Render the frame, after checking for minimum required frame quality */
      m_renderingFrames.Learn( *pFrame );
      pPlayer->m_pRenderer->render( pFrame );
    }
#ifdef FEATURE_QTV_VIDEO_RENDERING_QUALITY_ENABLE
#error code not present
#endif // FEATURE_QTV_VIDEO_RENDERING_QUALITY_ENABLE
  }
  else
  {
    QTV_MSG3( MSG_LEGACY_ERROR,
      "decode-done, VDEC_STATUS_GOOD_FRAME received, "
      "but vdec / frame is funky: "
      "(our VDEC ID is %x, frame belongs to VDEC %x, "
      "pBuf is %x)...ignoring...",
      pPlayer->decIF.m_vdec, pFrame->stream, pFrame->pBuf );
  }
}

/* ======================================================================
FUNCTION
  IsBroadcastLeadCheckFailed

DESCRIPTION
  Returns true when the broadcast lead is greater than the maximum
  configured.

RETURN VALUE
  True/False depending the broadcast lead check failure.
========================================================================== */
bool VideoDec::IsBroadcastLeadCheckFailed( long nSleepMsec )
{
// By default frame is syncd
  bool bFail = false;
  #ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
  #else
  QTV_USE_ARG1(nSleepMsec);
  #endif
  return bFail;
}

/* ======================================================================
FUNCTION
  HandleDND_FrameReady

DESCRIPTION
  Handles 'ready' events for do-not-display frames

RETURN VALUE
  None.
========================================================================== */
void VideoDec::HandleDND_FrameReady( VDEC_FRAME * const pFrame )
{
  if ( (pFrame == NULL) || !StateIsOkForFrameReady() )
  {
     QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR,
                  "HandleDND_FrameReady got NULL pFrame or !StateIsOkForFrameReady()" );
    return;
  }


  DoGenericVdecReuseFrame( VideoDec::FRAME_DROPPED_DECODE_FAILURE,
                           pFrame );
}

/* ======================================================================
FUNCTION
  StateIsOkForFrameReady

DESCRIPTION
  Checks to see if we're in a good state to handle a frame ready.  If we're
  not, produces a (hopefully) useful diag message.

RETURN VALUE
  bool
    true if we are okay to handle the event.
========================================================================== */
bool VideoDec::StateIsOkForFrameReady( void ) const
{
  /* First, the parade of error checking.  These are all seperated
  ** so we can get useful diag messages. Of course, that means they
  ** need to be ordered roughly in order of priority... */

  if ( !pPlayer )
  {
    /* If pPlayer's empty that means we're before Create or after Destroy.
    ** Likely if we're here it means we're processing a 'stale' callback -
    ** a leftover from before our destruction.  Trying to release the frame
    ** in this case would probably be catastrophic, as we've already
    ** been destroyed.  The best we can do here is ignore it.  */
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR,
                  "HandleFrameReady found 0 pPlayer" );
    return false;
  }

  if ( _state == VideoDec::VIDEODEC_DISABLED )
  {
    /* If we're disabled, we either failed to prep (in which case,
    ** what are we doing getting frame done callbacks?) or we've
    ** aborted playback.  Aborting causes a flush, which resets
    ** our accounting (outstandingFrameCount) so if we try to release
    ** the frame we'll end up double-accounting for it. Just make
    ** a noise and then bail. */
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR,
                  "HandleFrameReady when videodec disabled" );
    return false;
  }

  return true;
}

#ifndef PLATFORM_LTK
/* ======================================================================
FUNCTION
  GetLogBuffer

DESCRIPTION
  Grab and initialize a log buffer.

RETURN VALUE
  None.

========================================================================== */
template<class T>
T* GetLogBuffer( const log_code_type logCode )
{
  T *pBuf = NULL;

  /* Check if logging is turned on */
  if ( log_status( logCode ) )
  {
    /* Allocate a buffer */
    pBuf = ( T* )log_alloc( logCode, sizeof(T) );

    if ( pBuf == NULL )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"No memory for logging!" );
    }
  }

  return pBuf;
}

/* ======================================================================
FUNCTION
  LogRenderEvents

DESCRIPTION
  Logs information about frames being rendered.

RETURN VALUE
  None.

========================================================================== */
void VideoDec::LogRenderEvents( const VDEC_FRAME &frame )
{
  if ( frame.frameType == VDEC_FRAMETYPE_I )
  { // I-frame
  if(bFirstIFrameRendered == false)
  {
    bFirstIFrameRendered = true;
    event_report(EVENT_QTV_FIRST_VIDEO_I_FRAME_RENDERED);
  }
  }

  if(bFirstFrameRendered == false)
  {
  bFirstFrameRendered = true;
  event_report(EVENT_QTV_FIRST_VIDEO_FRAME_RENDERED );
  }
  return;
}
/* ======================================================================
FUNCTION
  LogFrameRenderInfo

DESCRIPTION
  Logs information about frames being rendered.

RETURN VALUE
  None.

========================================================================== */
void VideoDec::LogFrameRenderInfo( const VDEC_FRAME &frame ) const
{
  LOG_QTV_FRAME_RENDER_C_type *pBuf =
    GetLogBuffer<LOG_QTV_FRAME_RENDER_C_type>( LOG_QTV_FRAME_RENDER_C );

  if ( pBuf )
  {
    /* Fill in the statistics */
    pBuf->timestamp = (int32)(frame.timestamp & TIMESTAMP_MASK);
    pBuf->nBytes    = 0; /* nBytes in the status buffer is wrong anyway */
    pBuf->height    = frame.dimensions.height;
    pBuf->width     = frame.dimensions.width;

    /* Log the buffer */
    log_commit( pBuf );
  }
}

/* ======================================================================
FUNCTION
  LogFrameDecodeInfo

DESCRIPTION
  Logs information about frames being decoded.

RETURN VALUE
  None.

========================================================================== */
void VideoDec::LogFrameDecodeInfo( const VDEC_FRAME &frame ) const
{
  LOG_QTV_FRAME_DECODE_C_type *pBuf =
    GetLogBuffer<LOG_QTV_FRAME_DECODE_C_type>( LOG_QTV_FRAME_DECODE_C );

  if ( pBuf )
  {
    /* Fill in the statistics */
    pBuf->timestamp       = (int32)(frame.timestamp & TIMESTAMP_MASK);
    pBuf->isIntraFrame    = ( (frame.frameType == VDEC_FRAMETYPE_I) ? 1 : 0 );
    pBuf->nBytes          = 0; /* nBytes in the status buffer is wrong */
    pBuf->nBytesProcessed = 0; /* nBytesProcessed is wrong in stats buffer */

    /*Calculate percentage of macroblocks*/
    uint32 nMacroBlocksInFrame = ((frame.dimensions.width)*(frame.dimensions.height))/(16*16);
    uint32 nPercentageOfMacroBlocksConcealed = 0;
    if(frame.extFrame.numConcealedMbs && nMacroBlocksInFrame)
    {
       nPercentageOfMacroBlocksConcealed = ((frame.extFrame.numConcealedMbs)*100)/nMacroBlocksInFrame;
    }
    /*log this info*/
    pBuf->percentConcealedMBs = nPercentageOfMacroBlocksConcealed;

    /* Log the buffer */
    log_commit( pBuf );
  }
}

/* ======================================================================
FUNCTION
  LogAVSync

DESCRIPTION
  Logs information about AV Sync info about frames being decoded.

RETURN VALUE
  None.

========================================================================== */
void VideoDec::LogAVSync
(
  const long timestamp,
  const long delay,
  const uint8 Framedrop
)
const
{
  LOG_QTV_AV_SYNC_C_type *pBuf =
    GetLogBuffer<LOG_QTV_AV_SYNC_C_type>( LOG_QTV_AV_SYNC_C );

  if ( pBuf )
  {
    /* Fill in the statistics */
    pBuf->timestamp = timestamp;
    pBuf->delay     = delay;
    pBuf->FrameDropped = Framedrop;
    /* Log the buffer */
    log_commit( pBuf );
  }
}

#endif /* ! PLATFORM_LTK */

/* ======================================================================
FUNCTION:
  VideoDec::InitDecoderSettings

DESCRIPTION:
  Allocate and initialize decoder settings to known defaults.

RETURN VALUE:
  Returns a pointer to the newly allocated settings structure.

SIDE EFFECTS:
  None.
========================================================================== */
VideoDecControls* VideoDec::InitDecoderSettings
(
  const uint32 audioFormat
)
{
#ifdef FEATURE_QTV_IMEM //Sathiya
#error code not present
#else
  VideoDecControls *pSettings = (VideoDecControls*)QTV_Malloc(sizeof(VideoDecControls));
#endif

  if ( pSettings )
  {
    pSettings->audioFormat       = audioFormat;
  }

  return pSettings;
}

#ifdef FEATURE_QTV_STREAM_RECORD
/* ======================================================================
FUNCTION:
  VideoDec::RecordFrame

DESCRIPTION:
  Parse the headers in the decode buffer and return an array
  of BLOBs describing those headers.  For MP4, this is VOLs.

RETURN VALUE:
  A pointer to the new array of BLOBs.  This must be freed later!

SIDE EFFECTS:
  None.
========================================================================== */
void VideoDec::RecordFrame
(
  const LastRenderedFrameInfo * const pFrameInfo
)
{
  if( (NULL != pFrameInfo) && (NULL != pMpeg4) &&
      (pMpeg4->GetSource()==Media::STREAM_SOURCE))
  {
    if (0 == pFrameInfo->layer)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, " Layer 0:");
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                 "Call recordVideoFrame with TS %ld Msec",
                 pFrameInfo->timestamp);

      pMpeg4->recordVideoFrame( (pFrameInfo->bIsIntra != 0),
                                 pFrameInfo->timestamp);
      }
    else if( 1 == pFrameInfo->layer)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, " Layer 1:");
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                 "Call recordVideoFrame with TS %ld Msec",
                 pFrameInfo->timestamp);

      pMpeg4->recordEnhLayerVideoFrame( (pFrameInfo->bIsIntra != 0),
                                         pFrameInfo->timestamp);
    }
    else
    {
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "Skip layer %d frame (TS: %ld Msec) recording",
               pFrameInfo->layer,
               pFrameInfo->timestamp);
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"pFrameInfo is NULL");
  }
}
#endif  /* FEATURE_QTV_STREAM_RECORD */

/* ======================================================================
FUNCTION:                                                            GRB
  VideoDec::UpdateVideoDimensions()

DESCRIPTION:
  Look at the provided frame and update our videoInfo
  structure to reflect current video dimensions

INPUT/OUTPUT PARAMETERS:
  VDEC_FRAME *pFrame_in
    The frame from which we'll pull dimension information.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoDec::UpdateVideoDimensions( const VDEC_FRAME& frame )
{
  unsigned short fr_height = frame.cwin.y2 - frame.cwin.y1 ;
  unsigned short fr_width  = frame.cwin.x2 - frame.cwin.x1;
  if
  (
    videoInfo.imageHeight != fr_height ||
    videoInfo.imageWidth  != fr_width
  )
  {
    videoInfo.imageHeight = fr_height;
    videoInfo.imageWidth  = fr_width;

    QTV_MSG2( QTVDIAG_VIDEO_TASK,
              "Video dimensions have changed: %dx%d",
              videoInfo.imageWidth,
              videoInfo.imageHeight );
  } // video dimensions changed
}

/* ======================================================================
FUNCTION:
  VideoDec::AlterOutstandingFrameCount()

DESCRIPTION:
  Applies the supplied delta to the OFC.  If a change
  occurs which should result in waking the decoder, we take care
  of that too.

INPUT/OUTPUT PARAMETERS:
  const int delta
    How much to change m_outstandingFrameCount.  Positive numbers mean
    that work has been added to the queue, negative mean that work has
    been completed.  Because of the abs() nature of OFC, note that if we
    call this function just once with a -1, we still end up with a +1 OFC,
    because the 'add' event is still pending so the decode's considered
    incomplete even though we've seen the 'final' done event.
    (Shorter version: you need every +1 to pair with a -1 before OFC
     will go to 0)

  const char* szDiagHint
    A little string to put at the end of our diag message to ease debugging.

RETURN VALUE:
  unsigned int
    The value of OFC after the change.

SIDE EFFECTS:
  The decoder task may start if it's waiting on wakeupSync.
======================================================================*/
int VideoDec::AlterOutstandingFrameCount
(
  const int delta,
  const char* szDiagHint
)
{
  int preOFC, postOFC;

  QCUtils::EnterCritSect(&frameReqUpdate_CS);

  preOFC  = GetOutstandingFrameCount();
  m_outstandingFrameCount += delta;
  if ( m_outstandingFrameCount < 0 )
  {
     QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK,
                    QTVDIAG_PRIO_ERROR,
                    "attempt to set OFC to %d.  Setting to 0.",
                    m_outstandingFrameCount );
     m_outstandingFrameCount = 0;
  }
  // else: positive OFCs are OK
  postOFC = GetOutstandingFrameCount();

  QCUtils::LeaveCritSect(&frameReqUpdate_CS);

#ifdef SHOW_FRAME_REQUEST_COUNT
  if ( szDiagHint != NULL )
  {
    QTV_MSG_SPRINTF_PRIO_4( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                            "OFC: %d + (%d) = %d (%s)",
                            preOFC, delta, postOFC, szDiagHint );
  }
  else
  {
    QTV_MSG_PRIO3( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                   "OFC: %d + (%d) = %d", preOFC, delta, postOFC );
  }
#endif

  // Poke the decoder.

    QTV_MSG_PRIO2( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_MED,
                   "Waking decoder, OFC %d -> %d",
                   preOFC, postOFC );

    if(NULL!=pPlayer)
    {
      if(QCUtils::IsThreadActive(&pPlayer->decoderTC))
      {
        QCUtils::SetCondition( &m_wakeupSync);
      }
      else
      {
        QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_MED," not setting Condition as pPlayer->decoderTC Thread is not Active ...." );
      }
    }
    else
    {
        QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_MED," not setting Condition as pPlayer is NULL ...." );  
    }

  return postOFC;
}

/* ======================================================================
FUNCTION:
  VideoDec::GetOutstandingFrameCount()

DESCRIPTION:
  Returns the current value of the outstandingFrameCount metric.
  The member variable(s) should never be used directly.

RETURN VALUE:
  unsigned int
    The value of outstandingFrameCount.

SIDE EFFECTS:
  None.
======================================================================*/
int VideoDec::GetOutstandingFrameCount( void )
{
  int ofc;

  QCUtils::EnterCritSect(&frameReqUpdate_CS);
    ofc = m_outstandingFrameCount;
  QCUtils::LeaveCritSect(&frameReqUpdate_CS);

  return ofc;
}

/* ======================================================================
FUNCTION
  DoGenericVdecSuspend

DESCRIPTION
  Suspends our vdec so that the DSP can be used for another purpose.
  vdec is not destroyed; it is left in a state that lends itself to
  to resume with only DSP enable (i.e. partial re-initialization).
  This handles only the common, most general stuff.  Fancier video types
  may have to massage header data before calling this function (or do
  something else altogether)

RETURN VALUE
  boolean:  TRUE = yay, FALSE = boo

========================================================================== */
bool VideoDec::DoGenericVdecSuspend( void )
{
  VDEC_ERROR  vdec_error;
  boolean retVal = TRUE;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "VideoDec::DoGenericVdecSuspend" );
#ifndef FEATURE_QTV_GENERIC_BCAST
  if ((pMpeg4->GetSource() == Media::BCAST_ISDB_SOURCE) ||
    (pMpeg4->GetSource() == Media::BCAST_FLO_SOURCE) ||
    (pMpeg4->IsLive() == TRUE))
#else
  if (IsLiveBcastMediaSource(pMpeg4,m_pAVSync))
#endif
  {
    /* Flush before suspend for media that cannot be paused. */
    vdec_error = vdec_flush( m_vdec );
    if (vdec_error != VDEC_ERR_EVERYTHING_FINE)
    {
      QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Error in vdec_flush" );
    }

    /* Any work that was in progress has now been lost,
    ** so reset frameRequestCount.
    */
    (void)AlterOutstandingFrameCount( -1 * (int)GetOutstandingFrameCount(),
                                     "DoGenericVdecSuspend" );
  }

  vdec_error = vdec_suspend( m_vdec );
  if(vdec_error == VDEC_ERR_EVERYTHING_FINE)
  {
    if(!QCUtils::WaitForCondition(&m_suspendSync,SUSPEND_TIMEOUT_MSEC))
    {
  	QTV_MSG_PRIO(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR,"SuspendCalbackNot received");
  	retVal =  false;
    }
  }
  else
    retVal = false;

  return ((retVal)?true:false);
}

/* ======================================================================
FUNCTION
  DoGenericVdecResume

DESCRIPTION
  Resumes our vdec in preparation for decoding.
  Performs partial initialization.
  This handles only the common, most general stuff.  Fancier video types
  may have to massage header data before calling this function (or do
  something else altogether)

RETURN VALUE
  boolean:  TRUE = yay, FALSE = boo

========================================================================== */
bool VideoDec::DoGenericVdecResume( void )
{
  VDEC_ERROR        vdec_error;
  VDEC_CONCURRENCY_CONFIG concurrencyConfig;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "VideoDec::DoGenericVdecResume" );

  /* On audio configuration information:
  **
  ** Audio information can come from multiple places, in multiple formats.
  ** We have to convert them all to a VDEC type here.
  ** Audio format for resume may be different from that specified in
  ** full initialization of codec (typically we toggle from audio to
  ** no audio).
  **
  ** Here are the rules:
  ** 1) audOverrideFormat is one of the MP4QDSP_ defines from mp4codec.h.
  **    if this is anything but MP4QDSP_INVALID, we must use that.
  ** 2) if rule 1 doesn't apply, then we use audioFormat.  This is a
  **    Media::CodecType enum.
  */
  concurrencyConfig = VDEC_CONCURRENT_NONE;

  /*
    if audio override format set
    */
  if (m_pDecoderSettings->audioFormat != (int)Media::UNKNOWN_CODEC)
  {
      //incase of Audio override we should convert the override format to VDEC understandable format
      concurrencyConfig = MapToVdecConcurrencyFormat(m_pDecoderSettings->audioFormat );
  }

  if (concurrencyConfig == VDEC_CONCURRENT_NONE)
  {
#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif
     switch(( Media::CodecType )AudioMediaCodecTypeSet)
     {
       case Media::UNKNOWN_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_NONE  ;
       break;
       case Media::EVRC_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_EVRC ;
       break;
       case Media::QCELP_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_QCELP;
       break;
       case Media::AAC_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_AAC;
       break;
       case Media::BSAC_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_BSAC;
       break;
       case Media::GSM_AMR_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_AMR;
       break;
       case Media::MP3_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_MP3;
       break;
       case Media::WMA_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_NONE; //Right now, no WMA support. This will change soon.
       break;
       case Media::CONC_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_CONC;
       break;
       case Media::AMR_WB_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_AMR_WB;
       break;
       case Media::AMR_WB_PLUS_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_AMR_WB_PLUS;
       break;
       case Media::EVRC_NB_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_EVRC;
       break;
       case Media::EVRC_WB_CODEC:
         concurrencyConfig = VDEC_CONCURRENT_AUDIO_EVRC;
       break;
       default:
         concurrencyConfig = VDEC_CONCURRENT_NONE;
       break;
     }
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                              "Audio Codec Set from Media Source : AudioMediaCodecTypeSet (%d) -> concurrencyConfig (%d )",
                              AudioMediaCodecTypeSet, concurrencyConfig);
  }

  SetIncallStateParameter();

  vdec_error = vdec_resume ( m_vdec, concurrencyConfig );

  return ( vdec_error == VDEC_ERR_EVERYTHING_FINE );
}

/* ======================================================================
FUNCTION:
  VideoDec::Disable

DESCRIPTION:
  This function uses vdec to suspend the decoder, thereby releasing
  QDSP.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  DSP can only be restored by calling Enable().
========================================================================== */
bool VideoDec::Disable()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "VideoDec::Disable");

  /* Suspend decoding.
   */
  return DoGenericVdecSuspend();
}

/* ======================================================================
FUNCTION:
  VideoDec::Enable

DESCRIPTION:
  This function uses vdec to resume the decoder, thereby restoring
  QDSP.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  Restores QDSP after a prior call to Disable().
========================================================================== */
bool VideoDec::Enable(void)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "VideoDec::Enable");

  /* Resume decoding.
   */
  return DoGenericVdecResume();
}

/* ======================================================================
FUNCTION:
  VideoDec::SetAudioCodec

DESCRIPTION:
  This function sets the audio codec.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  Updates m_pDecoderSettings.  Primarily intended for use prior to
  Enable(); for example, to resume decoding without audio, it is
  necessary to call SetAudioCodec(CONC_CODEC) followed by Enable().
========================================================================== */
void VideoDec::SetAudioCodec(Media::CodecType audioCodec)
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "VideoDec::SetAudioCodec, %d", audioCodec);

 /* Store the audio format.
  */
  AudioMediaCodecTypeSet = audioCodec;
}

/* ======================================================================
FUNCTION:                                                            GRB
  VideoDec::IsItOkayToDropThisFrame()

DESCRIPTION:
  This function allows the renderer to check if it can drop a frame.

INPUT/OUTPUT PARAMETERS:
  const VDEC_FRAME* const pFrame
    The frame that the renderer is thinking about dropping.

RETURN VALUE:
  bool
    true if we can drop the frame, false if not.

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoDec::IsItOkayToDropThisFrame( const VDEC_FRAME* const pFrame ) const
{
  bool bNoDrop( false );

  bNoDrop =
    bSingleStepOn ||
    !bEnableDropBeforeDisplay ||
    (
      bForceBaseLayerIFrameDisplay &&
      (pFrame->frameType == VDEC_FRAMETYPE_I) &&     /*lint !e506 */
      pFrame->layer == 0
    ) ||
    //never drop frame zero since it could be a still image.
    //nFramesProcessed is the number of frames decoded and therefore for frame 0, nFrameProcessed=1 and so on...
    ( nFramesProcessed == 1 );

  return !bNoDrop;
}

/* ======================================================================
FUNCTION:                                                            GRB
  VideoDec::GetRequiredRendererPreroll()

DESCRIPTION:
  This function allows the renderer to figure out how much of a preroll
  it should be maintaining from frame to frame.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  unsigned int
    The number of frames which must be in the rendering frame queue
    before the renderer may consume one.

SIDE EFFECTS:
  None.
======================================================================*/
uint32 VideoDec::GetRequiredRendererPreroll( void ) const
{
  uint32 result = 0;

  if ( pPlayer && pPlayer->m_pRenderer )
  {
    result = pPlayer->m_pRenderer->get_preroll();
  }

  return result;
}

/* ======================================================================
FUNCTION:
  VideoDec::SetPrerollRendererCount

DESCRIPTION:
  Sets the m_prerollRendererCount.

INPUT/OUTPUT PARAMETERS:
  uint32 prerollCount

RETURN VALUE:
  None.

SIDE EFFECTS:
  MFlo(OSCAR_DEC) should set dynamically on need.
======================================================================*/
void VideoDec::SetPrerollRendererCount( uint32 prerollCount )
{
  if ( pPlayer && pPlayer->m_pRenderer )
  {
    pPlayer->m_pRenderer->set_preroll( prerollCount );
  }
}

/* ======================================================================
FUNCTION
  VideoDec::Flush

DESCRIPTION
// Call Flush() to flush out all decoder buffer queues.
// Video should be stopped or suspended when we do this, or the buffer   //
// queues'll fill up again...unless that's what you wanted...            //
//                                                                       //
// This function is synchronous - it will not return until all buffers   //
// have been flushed.  That means that the calling task will block.      //

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
========================================================================== */
void VideoDec::Flush( void )
{
  // All of our queues are managed by the decoder.
  if (NULL !=  m_vdec)
  {
    (void)vdec_flush( m_vdec );
  }

  // Zorch the input buffer in case any outstanding data is left in it.
  // (We can get this in 'backoff' situations)
  //
  memset( decodeBuf.size, 0, sizeof( decodeBuf.size ) );

  // Any work that was in progress has now been lost,
  // so reset frameRequestCount.

  (void)AlterOutstandingFrameCount( -1 * GetOutstandingFrameCount(),
                              "Flush" );
}

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif

/* ======================================================================
FUNCTION
  VideoDec::Set_State

DESCRIPTION
  Sets the _state member variable and takes care of any additional
  administravia that may be required when a _state change happens.

DEPENDENCIES
  None.

RETURN VALUE
  VideoDecoderState
    The new state.

SIDE EFFECTS
  None.
========================================================================== */
VideoDec::VideoDecoderState VideoDec::Set_State
(
  const VideoDecoderState newState
)
{
  _state = newState;

  if ( _state == VIDEODEC_DISABLED )
  {
    /* make sure that overallstate also indicate error or end condition */
    if ( overallDataState != Media::DATA_ERROR )
    {
      overallDataState = Media::DATA_END;
    }
  }

  return _state;
}

/* ======================================================================
FUNCTION
  DoGenericVdecCreate

DESCRIPTION
  Creates our vdec in preparation for decoding.
  This handles only the common, most general stuff.  Fancier video types
  may have to massage header data before calling this function (or do
  something else altogether)

RETURN VALUE
  boolean:  TRUE = yay, FALSE = boo

========================================================================== */

bool VideoDec::DoGenericVdecCreate(void)
{
   const char *fourcc[] = {"mp4v","i263","avc1","wmv1","wmv2","wmv3","rv40","jpeg","div3","div4","isom"};
                         /* 0      1      2      3      4      5      6      7      8      9      10*/
   unsigned int index = 0;
   VDEC_ERROR  vdec_error = VDEC_ERR_EVERYTHING_FINE;
   bool bOK = true;

   QCUtils::EnterCritSect( &m_vdecCreationLock );

  /* It's possible the application is just calling Init over and over with
  ** no CleanUp calls in between, so make sure we take care of that.
  */
  (void)DoGenericVdecShutdown( );

  switch(videoInfo.videoCodec)
  {
    case Media::MPEG4_CODEC: index = 0; break;
    case Media::H263_CODEC : index = 1; break;
    case Media::H264_CODEC : index = 2; break;
    case Media::WMV1_CODEC : index = 3; break;
    case Media::WMV2_CODEC : index = 4; break;
    case Media::WMV3_CODEC : index = 5; break;
    case Media::RV40_CODEC : index = 6; break;
    case Media::JPEG_CODEC : index = 7; break;
#ifdef FEATURE_DIVX_311_ENABLE
    case Media::DIVX311_CODEC : index = 8; break;
#endif
    case Media::STILL_IMAGE_CODEC : index = 10; break;
	case Media::STILL_IMAGE_H263_CODEC : index = 1; break;

    default:
      bOK = false;
	  vdec_error = VDEC_ERR_INVALID_PARAMETER;
      QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,"DoGenericVdecCreate failed!! error - unknown Codec type");
}

  if(bOK == true)
{
    QTV_MSG_PRIO1(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR,"vdec_create: %d", index);
    vdec_error = vdec_create(fourcc[index],&m_vdec);

    QCUtils::LeaveCritSect( &m_vdecCreationLock );

    if ( vdec_error != VDEC_ERR_EVERYTHING_FINE )
  {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failure in vdec_create: %d", vdec_error );
    }
  }

  return ( vdec_error == VDEC_ERR_EVERYTHING_FINE );
}

/* ======================================================================
FUNCTION
  setLipSyncWindowSize

DESCRIPTION
  Sets the LipSyncWindowSize Variable.

RETURN VALUE
  none

========================================================================== */
void VideoDec::SetLipSyncWindowSize(Media * pMpeg4In)
{
  if ( pMpeg4In->GetSource() != Media::STREAM_SOURCE )
    LipSyncWindowSize = VideoDec::DISPLAY_THRESH_LOCAL_MSEC;
  else
    LipSyncWindowSize = VideoDec::DISPLAY_THRESH_STREAM_MSEC;

#ifdef T_RUMI
#error code not present
#endif /* T_RUMI */

  QTV_MSG1( QTVDIAG_VIDEO_TASK, "Lip Sync Window Size %d ms", LipSyncWindowSize );
}

/* ======================================================================
FUNCTION
  GetFrameDimensionsFromMedia

DESCRIPTION
  Gets Frame Dimensions from Media.

RETURN VALUE
  none

========================================================================== */
void VideoDec::GetFrameDimensionsFromMedia(Media * pMpeg4)
{
  bool bGotVideoDimensions = false;

   if((videoInfo.videoCodec != Media::WMV3_CODEC)
      && (videoInfo.videoCodec != Media::WMV2_CODEC)
      && (videoInfo.videoCodec != Media::WMV1_CODEC)
      && (videoInfo.videoCodec != Media::JPEG_CODEC)
      && (videoInfo.videoCodec != Media::H264_CODEC)
      && (videoInfo.videoCodec != Media::MPEG4_CODEC)
      && (videoInfo.videoCodec != Media::H263_CODEC)
#ifdef FEATURE_DIVX_311_ENABLE
      && (videoInfo.videoCodec != Media::DIVX311_CODEC)
#endif
                                   )
  {
    videoInfo.imageWidth = MP4_MAX_DECODE_WIDTH;
    videoInfo.imageHeight = MP4_MAX_DECODE_HEIGHT;
  }
  else
    {
    bGotVideoDimensions = pMpeg4->GetVideoDimensions( (uint32 *) &videoInfo.imageWidth,
                                                    (uint32 *) &videoInfo.imageHeight );
    if ( !bGotVideoDimensions )
    {
      // Default VDEC_MAX_DECODE dimensions if we can't get the video
      // dimensions.
      videoInfo.imageWidth = MP4_MAX_DECODE_WIDTH;
      videoInfo.imageHeight = MP4_MAX_DECODE_HEIGHT;
    }
    else
    {
       /* validate dimensions from meda data with max supported */
       if( (videoInfo.imageWidth > MP4_MAX_DECODE_WIDTH)
            || ( (videoInfo.imageWidth * videoInfo.imageHeight) > (MP4_MAX_DECODE_WIDTH*MP4_MAX_DECODE_HEIGHT)))
       {
         // Default VDEC_MAX_DECODE dimensions if we got unsupported diemsions
         videoInfo.imageWidth = MP4_MAX_DECODE_WIDTH;
         videoInfo.imageHeight = MP4_MAX_DECODE_HEIGHT;
       }
    }
  }

  if ( !bGotVideoDimensions )
  {
   
     videoInfo.originalWidth = 0;
     videoInfo.originalHeight = 0;
  }
  else
  {
    videoInfo.originalWidth = videoInfo.imageWidth;
	videoInfo.originalHeight = videoInfo.imageHeight;
  }

  QTV_MSG2( QTVDIAG_VIDEO_TASK,
            "Video dimensions: %dx%d",
            videoInfo.imageWidth,
            videoInfo.imageHeight );
}

#ifdef FEATURE_QTV_6250_ARM_CLK_COMPENSATION
/* ======================================================================
FUNCTION
  SetDriftConstantValue

DESCRIPTION
  sets the value of driftConstant based on audioCodec type.

RETURN VALUE
  none

========================================================================== */
void SetDriftConstantValue(Media::CodecType audioCodec)
{
    /* driftConstant value will be used for arm clock drift compensation fix */
    driftConstant = 0.00085;

    /* incase of stream recording and qdsp release restore features, audio slows
      down earlier, so the drift compensation value has to be different for
      these two cases */

#if defined (FEATURE_QTV_STREAM_RECORD) && defined (FEATURE_AAC)
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
  if(audioCodec == Media::AAC_CODEC)
#endif /* FEATURE_QTV_BSAC */
    driftConstant = 0.0003;
#endif

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
  if(audioCodec == Media::AAC_CODEC)
#endif /* FEATURE_QTV_BSAC */
    driftConstant = 0.0003;
  else
    driftConstant = 0.00055;
#endif

    QTV_MSG1(QTVDIAG_VIDEO_TASK, "drift constant %d", (long)(driftConstant*100000));
}
#endif

/************************************************************************* */
/**
 * Returns true if a timestamp is 'defined'; if it has a healthy value.
 *
 * @param[in] ts The timestamp under scrutiny.
 * @return true or false (or maybe? oooooh...)
 *
 ************************************************************************* */
bool VideoDec::timestamp_is_defined( const uint32 ts )
{
  const uint32 UNDEF = uint32( -1 );

  return ( ts != UNDEF );
}

/************************************************************************* */
/**
 * Remembers this frame's timestamp as our 'last seen' in case we need
 * to repair future timestamps.
 *
 ************************************************************************* */
void VideoDec::record_timestamp( VDEC_FRAME* const frame_ptr )
{
  if (frame_ptr)
  {
    if( !(UNDEFINED_TIMESTAMP & frame_ptr->timestamp)  )
    {
      m_last_timestamp = frame_ptr->timestamp;
    }
    else if( !(UNDEFINED_TIMESTAMP & frame_ptr->extFrame.derivedTimestamp) )
    {
      m_last_timestamp = frame_ptr->extFrame.derivedTimestamp;
    }
  }
}

/************************************************************************* */
/**
 * Repair this frame's timestamp based on our last-seen timestamp.
 *
 ************************************************************************* */
void VideoDec::repair_timestamp( VDEC_FRAME* const frame_ptr )
{

  if (frame_ptr)
  {
    if (frame_ptr->timestamp & UNDEFINED_TIMESTAMP)
    {
      if (frame_ptr->extFrame.derivedTimestamp & UNDEFINED_TIMESTAMP)
      {
        /* Assume 15fps in absence of any TS info */
        frame_ptr->timestamp = m_last_timestamp + ( 1000 / 15 );
        QTV_MSG_PRIO2( QTVDIAG_VIDEO_TASK,
                       QTVDIAG_PRIO_DEBUG,
                       "## Found decoded frame with no timestamp.  Last good ts = %d.  Setting current ts = %d",
                       m_last_timestamp,
                       frame_ptr->timestamp );
		m_last_timestamp = frame_ptr->timestamp;
      }
      else /* inband timestamp ok */
      {
        frame_ptr->timestamp = frame_ptr->extFrame.derivedTimestamp;
        QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK,
                       QTVDIAG_PRIO_DEBUG,
                       "## Replacing undefined PES timestamp with bitstream timestamp: %d",
                       frame_ptr->timestamp );
      } /* inband timestamp ok */
    } /* PES timestamp undefined */
  } /* frame_ptr */
    }
uint32 VideoDec::get_last_timestamp( void )
  {
 return m_last_timestamp;
}

#ifdef FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY
/* ======================================================================
FUNCTION
  VideoDec::SetPlaybackSpeed

DESCRIPTION
  Sets the playback speed.

PARAMETERS
  uint32 pbSpeedNum
    The numerator portion of the desired playback speed ratio.
  uint32 pbSpeedDen
    The denominator portion of the desired playback speed ratio.

RETURN VALUE
  None.
========================================================================== */
void VideoDec::SetPlaybackSpeed(const uint32 pbSpeedNum, const uint32 pbSpeedDen)
{
  m_playbackSpeedNum = pbSpeedNum;
  m_playbackSpeedDen = pbSpeedDen;
}
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY */

/* ======================================================================
FUNCTION
  VideoDec::CheckPlayIntervalMatch

DESCRIPTION
  Check to make sure that presentation time stamp under consideration
  belong to the current play interval. Play intervals are separated by
  underruns.

  Assumption here is that timestamps are always increasing. Need to
  account for wrap around later if need arise. Qtv does not handle
  timestamp wrap-around any way for now.

PARAMETERS
  uint32 uPresTime
    Timestamp of top most frame in the renderer queue.


RETURN VALUE
  True if presentation timestamp belong to current time interval.
  false otherwise
========================================================================== */
bool VideoDec::CheckPlayIntervalMatch( uint32 uPresTime )
{

  if(m_uLastTimestampBeforeRebuffering >= uPresTime)
  {
    QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK,QTVDIAG_PRIO_DEBUG,
    "Renderer processing frame #%d belong to previous play interval",
    uPresTime );
  }
  return (uPresTime > m_uLastTimestampBeforeRebuffering);
}

/* ======================================================================
FUNCTION:
  VideoDec::ReleaseBuffer

DESCRIPTION:
  Notify the player that higher levels have finished rendering the
  current video frame.  The player is free to reuse or free the buffer
  after this function is called.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoDec::ReleaseBuffer( const QtvPlayer::FrameInfoT& frame )
{
   VDEC_FRAME* pVDEC_Frame( m_renderingFrames.Recall( frame ) );

   if ( pVDEC_Frame )
   {
      if (pVDEC_Frame->frameStatus == FRAME_WITH_UI)
      {
         DoGenericVdecReuseFrame( VideoDec::FRAME_RENDERED, pVDEC_Frame );
      }
      else
      {
        QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK,
             QTVDIAG_PRIO_HIGH, "invalid release, ignoring...");
      }
   }
   else
   {
      QTV_MSG_PRIO3( QTVDIAG_VIDEO_TASK,
                     QTVDIAG_PRIO_HIGH,
                     "ignoring stale released frame (%x, %x) %d",
                     frame.YUVBuffer,
                     frame.RGBBuffer,
                     frame.Timestamp );
   }
}

/* ======================================================================
FUNCTION
  VideoDec::GetAverageVideoFrameInterval

DESCRIPTION
 Gets the average video frame interval.

PARAMETERS

RETURN VALUE
  average video frame interval.
========================================================================== */
uint32 VideoDec::GetAverageVideoFrameInterval( )
{
  uint32 nAverageVideoFrameInterval = 0;

  if ( pMpeg4 && (pMpeg4->GetSource() == Media::STREAM_SOURCE) )
  {
    float frameRate = pMpeg4->GetVideoFrameRate();

    if (frameRate > 0.001f)
    {
      nAverageVideoFrameInterval = (uint32) (1000.0f / frameRate);
    }
    else
    {
      nAverageVideoFrameInterval = (uint32) videoStat.averageInterFrameIntervalMsec;
    }
  }
  return nAverageVideoFrameInterval;
}

/* ======================================================================
FUNCTION
  VideoDec::GetLastReadVideoFrameTimeStamp

DESCRIPTION
 Gets the timestamp of last read sample.

PARAMETERS

RETURN VALUE
  returns timestamp if defiend else returns -1.
========================================================================== */

uint32 VideoDec::GetLastReadVideoFrameTimeStamp( )
{
  if (!(decodeBuf.timestamp[0] & UNDEFINED_TIMESTAMP))
  {
    return(uint32)(decodeBuf.timestamp[0] & TIMESTAMP_MASK);
  }
  else
    return(uint32)-1;
}

/* ======================================================================
FUNCTION
  VideoDec::RunVideoQualityThreshold

DESCRIPTION
 Runs video quality threshold algorithm on input frame

PARAMETERS
 VDEC_FRAME

RETURN VALUE
 true, if video frame errors threshold met.
========================================================================== */
bool VideoDec::RunVideoQualityThreshold( VDEC_FRAME* const pFrame )
{
  bool bIsThresholdMet = false;
  uint32 nMacroBlocksInFrame = ((pFrame->dimensions.width)*(pFrame->dimensions.height))/(16*16);
  uint32 nPercentageOfMacroBlocksConcealed = 0;

  if(pFrame->extFrame.numConcealedMbs)
  {
    QTV_MSG_PRIO3( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
                  "frame dim, Width: %d Height: %d, num macroblocks Concealed: %d", pFrame->dimensions.width, pFrame->dimensions.height, pFrame->extFrame.numConcealedMbs );
    QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
                  "QTVCONFIG_GENERIC_VIDEO_RENDERING_QUALITY: %d", pPlayer->pAVPlayer->GetQTVConfigItem(QtvConfig::QTVCONFIG_GENERIC_VIDEO_RENDERING_QUALITY));

    if(nMacroBlocksInFrame)
    {
      nPercentageOfMacroBlocksConcealed = ((pFrame->extFrame.numConcealedMbs)*100)/nMacroBlocksInFrame;
      if( nPercentageOfMacroBlocksConcealed >
         (uint32) (pPlayer->pAVPlayer->GetQTVConfigItem(QtvConfig::QTVCONFIG_GENERIC_VIDEO_RENDERING_QUALITY)))
      {
        bIsThresholdMet = true;
        QTV_MSG_PRIO2( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
                      "Video frame error threshold met., since Percentage of macroblocks concealed: %d, TS: %d",nPercentageOfMacroBlocksConcealed, ((pFrame->timestamp)&TIMESTAMP_MASK) );
      }
    }
  }
  return bIsThresholdMet;
}

/* ======================================================================
FUNCTION
  VideoDec::SetVDECParameter

DESCRIPTION
  Sets the VDEC parameter

PARAMETERS
 VDEC_PARAMETER_ID inputParamId,
    input VDEC parameter id
  VDEC_PARAMETER_DATA *pInputParam
    pointer to input VDEC parameter value

RETURN VALUE
  VDEC_ERROR
    Enum of the VDEC return type.
========================================================================== */
VDEC_ERROR VideoDec::SetVDECParameter( VDEC_PARAMETER_ID inputParamId,  VDEC_PARAMETER_DATA * pInputParam)
{
  VDEC_ERROR nReturn = VDEC_ERR_MYSTERY_ERROR;

  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                "inputParamId: %d, pInputParam->freEnable.bFreEnable: %d",
                inputParamId, (pInputParam->freEnable.bFreEnable));
  if( pInputParam)
  {
    nReturn = vdec_set_parameter(m_vdec,inputParamId, pInputParam);
  }
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                "VDEC_ERROR return %d", nReturn);

  return nReturn;
}

/* ======================================================================
FUNCTION
  VideoDec::MapToVdecConcurrencyFormat

DESCRIPTION
  Maps the format to the Vdec override format (concurrency format), this will be executed when there is
  a SetAudioOverride call was done and the audio over ride format has been set successfully

PARAMETERS
  uint32 format

RETURN VALUE
  VDEC_CONCURRENCY_CONFIG
    cuncerrency format.
========================================================================== */
 VDEC_CONCURRENCY_CONFIG VideoDec::MapToVdecConcurrencyFormat(uint32 format)
{

  VDEC_CONCURRENCY_CONFIG concurrencyConfig = VDEC_CONCURRENT_NONE;

     switch(format)
     {
           case QtvPlayer::AUD_OVER_ENC_QCELP13K:
           {
                QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                              "VDEC Concurency Format : format: %d -> %d (VDEC_CONCURRENT_AUD_OVER_ENC_QCELP13K)",
                              format, concurrencyConfig);
                concurrencyConfig = VDEC_CONCURRENT_AUD_OVER_ENC_QCELP13K;
		  break;
           }
   	    case QtvPlayer::AUD_OVER_ENC_EVRC:
	    {
                QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                              "VDEC Concurency Format : format: %d -> %d (VDEC_CONCURRENT_AUD_OVER_ENC_EVRC)",
                              format, concurrencyConfig);
                concurrencyConfig = VDEC_CONCURRENT_AUD_OVER_ENC_EVRC;
		  break;
           }
   	    case QtvPlayer::AUD_OVER_ENC_AMR:
	    {
                QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                              "VDEC Concurency Format : format: %d -> %d (VDEC_CONCURRENT_AUD_OVER_ENC_AMR)",
                              format, concurrencyConfig);
                concurrencyConfig = VDEC_CONCURRENT_AUD_OVER_ENC_AMR;
		  break;
           }
	   /*Other formats have to be added (mentioned in Qtvplayer.h enum AudOverrideType) after there is a Support form Decoder,
             needs to be clarified by Sathiya N from QTV Decoder team. And also what should be
             the default cuncerrency type to be returend ????? */
	   default:
	   {
	   	  concurrencyConfig = VDEC_CONCURRENT_NONE;

                QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                              "VDEC Concurency Format : format: %d -> %d (VDEC_CONCURRENT_NONE -> Default))",
                              format, concurrencyConfig);
                break;
	   }
     }
     return concurrencyConfig;
}

/* ======================================================================
FUNCTION
  VideoDec::SetIncallStateParameter

DESCRIPTION
  This calls vdec_set_parameter inorder to select proper voice+qtv image/module

PARAMETERS
  uint32 format

RETURN VALUE
   void
========================================================================== */
void VideoDec::SetIncallStateParameter()
{
   VDEC_PARAMETER_DATA parm_data;
   Media::CodecType nIncallVoiceCodecType = GetIncallVoiceCodec();

  memset (&parm_data,0x00,sizeof(VDEC_PARAMETER_DATA));

  switch(nIncallVoiceCodecType)
  {
    case Media::EVRC_CODEC:
    {
        parm_data.audio_vtype.vtype = VTYPE_EVRC;

	break;
    }
    case Media::QCELP_CODEC:
    {
        parm_data.audio_vtype.vtype = VTYPE_13K;
	break;
    }
    case Media::FGV_CODEC_NB:
    case Media::FGV_CODEC_WB:
    {
        parm_data.audio_vtype.vtype = VTYPE_4GV;
	break;
    }
    case Media::GSM_AMR_CODEC:
    {
        parm_data.audio_vtype.vtype = VTYPE_AMR;
	break;
    }
    default :
    {
          // set to none
	 parm_data.audio_vtype.vtype = VTYPE_NONE;
    }
  }
   vdec_set_parameter(m_vdec,VDEC_PARM_AUDIO_VTYPE,&parm_data);
   QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                      "In call codec %d and vtype %d ",
                       nIncallVoiceCodecType,parm_data.audio_vtype.vtype);
}

/* ======================================================================
FUNCTION
  VideoDec::SetIncallVoiceCodec

DESCRIPTION
  This functions sets the voice codec type during incall video (QTV)

PARAMETERS
  Media::CodecType eVocoderType

RETURN VALUE
   void
========================================================================== */
void VideoDec::SetIncallVoiceCodec(Media::CodecType eVocoderType)
{
      nInCallVocodertype = eVocoderType;
}

/* ======================================================================
FUNCTION
  VideoDec::GetIncallVoiceCodec

DESCRIPTION
  This functions returns the voice codec type during incall video (QTV)

PARAMETERS
  void

RETURN VALUE
  Media::CodecType eVocoderType
========================================================================== */
Media::CodecType VideoDec::GetIncallVoiceCodec()
{
     return nInCallVocodertype;
}

/* ======================================================================
FUNCTION:
  VideoDec::UpdateClipInfoDimensions

DESCRIPTION:
 This is a sync funciton which updates the height and width of clipinfo.

INPUT/OUTPUT PARAMETERS:
  int height : height of enoded frame.
  int width  : width of encoded frame.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/
void VideoDec::UpdateClipInfoDimensions(int height, int width)
{
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
	  "VideoDec::UpdateClipInfoDimensions () ");
	if (pPlayer)
	{
       pPlayer->UpdateClipInfoDimensions(height, width);
	}
}
